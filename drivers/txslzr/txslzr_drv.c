/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_txslzrdrv_impl TXSLZR Driver Design
    @ingroup grp_txslzrdrv

    @addtogroup grp_txslzrdrv_impl
    @{

    @file txslzr_drv.c
    @brief TXSLZR Driver Design

    @version 0.1 Initial version
*/

#include <txslzr.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <txslzr_rdb.h>
#include <txslzr_intr_rdb.h>

/**
    @name TXSLZR Driver Design IDs
    @{
    @brief Driver Design IDs for TXSLZR
*/

#define BRCM_SWDSGN_TXSLZR_DRV_DEV_TYPE                    (0xB1C0U)  /**< @brief #TXSLZR_DrvDevType           */
#define BRCM_SWDSGN_TXSLZR_DRV_DEV_GLOBAL                  (0xB1C1U)  /**< @brief #TXSLZR_DrvDev               */
#define BRCM_SWDSGN_TXSLZR_REGS_GLOBAL                     (0xB1C2U)  /**< @brief #TXSLZR_Regs                 */
#define BRCM_SWDSGN_TXSLZR_INTR_REGS_GLOBAL                (0xB1C3U)  /**< @brief #TXSLZR_IntrRegs             */

#define BRCM_SWDSGN_TXSLZR_DRV_INIT_PROC                   (0xB1D0U)  /**< @brief #TXSLZR_DrvInit              */
#define BRCM_SWDSGN_TXSLZR_DRV_DEINIT_PROC                 (0xB1D1U)  /**< @brief #TXSLZR_DrvDeInit            */
#define BRCM_SWDSGN_TXSLZR_DRV_PAGE_CONFIG_PROC            (0xB1D2U)  /**< @brief #TXSLZR_DrvPageConfig        */
#define BRCM_SWDSGN_TXSLZR_DRV_PAT_GEN_ENABLE_PROC         (0xB1D3U)  /**< @brief #TXSLZR_DrvPatGenEnable      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_WR_MODE_PROC        (0xB1D4U)  /**< @brief #TXSLZR_IntPatGenWrMode      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_WR_ADDR_PROC        (0xB1D5U)  /**< @brief #TXSLZR_IntPatGenWrAddr      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_WR_DATA_PROC        (0xB1D6U)  /**< @brief #TXSLZR_IntPatGenWrData      */
#define BRCM_SWDSGN_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC        (0xB1D7U)  /**< @brief #TXSLZR_DrvPatGenWrPage      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_RD_MODE_PROC        (0xB1D8U)  /**< @brief #TXSLZR_IntPatGenRdMode      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_RD_ADDR_PROC        (0xB1D9U)  /**< @brief #TXSLZR_IntPatGenRdAddr      */
#define BRCM_SWDSGN_TXSLZR_INT_PAT_GEN_RD_DATA_PROC        (0xB1DAU)  /**< @brief #TXSLZR_IntPatGenRdData      */
#define BRCM_SWDSGN_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC        (0xB1DBU)  /**< @brief #TXSLZR_DrvPatGenRdPage      */
#define BRCM_SWDSGN_TXSLZR_DRV_PRBS_CONFIG_SET_PROC        (0xB1DCU)  /**< @brief #TXSLZR_DrvPrbsConfigSet     */
#define BRCM_SWDSGN_TXSLZR_DRV_PRBS_ENABLE_PROC            (0xB1DDU)  /**< @brief #TXSLZR_DrvPrbsEnable        */
#define BRCM_SWDSGN_TXSLZR_DRV_ENABLE_INTERRUPT_PROC       (0xB1E0U)  /**< @brief #TXSLZR_DrvEnableInterrupt   */
#define BRCM_SWDSGN_TXSLZR_DRV_INTERRUPT_STATUS_PROC       (0xB1E1U)  /**< @brief #TXSLZR_DrvInterruptStatus   */
#define BRCM_SWDSGN_TXSLZR_DRV_EVENT_STATUS_PROC           (0xB1E2U)  /**< @brief #TXSLZR_DrvEventStatus       */
#define BRCM_SWDSGN_TXSLZR_DRV_CLEAR_INTERRUPT_PROC        (0xB1E3U)  /**< @brief #TXSLZR_DrvClearInterrupt    */

#define BRCM_SWDSGN_TXSLZR_DRV_SEQSPICFG_PROC              (0xB1E4U)  /**< @brief #TXSLZR_DrvSeqSpiCfg*/
#define BRCM_SWDSGN_TXSLZR_DRV_SEQCONTROL_PROC             (0xB1E5U)  /**< @brief #TXSLZR_DrvSeqControl*/
#define BRCM_SWDSGN_TXSLZR_DRV_SEQMEMRDWR_PROC             (0xB1E6U)  /**< @brief #TXSLZR_DrvSeqMemRdWr*/
#define BRCM_SWDSGN_TXSLZR_DRV_SWSPIRDWR_PROC              (0xB1E7U)  /**< @brief #TXSLZR_DrvSwSpiRdWr*/
#define BRCM_SWDSGN_TXSLZR_DRV_SEQ_MODE_CFG_PROC           (0xB1E8U)  /**< @brief #TXSLZR_DrvSeqModeConfig*/

/** @} */

/**
    @trace #BRCM_SWARCH_TXSLZR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
*/
static TXSLZR_RDBType *const TXSLZR_Regs[TXSLZR_MAX_HW_ID] = {
#if (TXSLZR_MAX_HW_ID == 0UL)
#error "TXSLZR_MAX_HW_ID == 0"
#endif
    (TXSLZR_RDBType *) TXSLZR_BASE,
#if (TXSLZR_MAX_HW_ID > 1UL)
#error "TXSLZR_MAX_HW_ID > 1 is not supported by the driver"
#endif /* TXSLZR_MAX_HW_ID > 1 */
};

/**
    @trace #BRCM_SWARCH_TXSLZR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
*/
static TXSLZR_INTR_RDBType * const TXSLZR_IntrRegs[TXSLZR_INTR_MAX_HW_ID] =
{
#if (TXSLZR_INTR_MAX_HW_ID == 0UL)
#error "TXSLZR_MAX_HW_ID == 0U"
#endif
    (TXSLZR_INTR_RDBType *)TXSLZR_INTR_BASE,
#if (TXSLZR_INTR_MAX_HW_ID > 1UL)
#error "TXSLZR_MAX_HW_ID > 1 is not supported by the driver"
#endif /* TXSLZR_INTR_MAX_HW_ID > 1 */
};

/**
    @name TXSLZR Driver device type
    @{
    @brief TXSLZR Driver device type struct

    @trace #BRCM_SWARCH_TXSLZR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
 */
typedef struct sTXSLZR_DrvDevType {
    TXSLZR_StateType   state; /**< @brief State of the driver */
} TXSLZR_DrvDevType;
/** @} */

/**
    @name TXSLZR Driver device type
    @{
    @brief TXSLZR Driver device type struct

    @trace #BRCM_SWARCH_TXSLZR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
 */
#ifdef BCM8915X_PARAM_VALIDATION
static TXSLZR_DrvDevType TXSLZR_DrvDev[TXSLZR_MAX_HW_ID] COMP_SECTION(".bss.drivers");
#endif
/** @} */

/**
    @brief initializes TXSLZR driver

    @code{.unparsed}
    Check hardware instance with max supported device.
    Move state to TXSLZR_STATE_INITIALIZED state.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_INIT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
*/
int32_t TXSLZR_DrvInit(TXSLZR_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID || (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {

        TXSLZR_DrvDev[hwId].state = TXSLZR_STATE_INITIALIZED;
    }
#endif
    return ret;
}

/**
    @brief De-initializes TXSLZR driver

    @code{.unparsed}
    Check hardware instance with max supported device.
    Move state to TXSLZR_STATE_UNINIT.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INIT
*/
int32_t TXSLZR_DrvDeInit(TXSLZR_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
        TXSLZR_DrvDev[hwId].state = TXSLZR_STATE_UNINIT;
    }
#endif
    return ret;
}

/**
    @brief TXSLZR page configuration

    @code{.unparsed}
    Check hardware instance with max supported device.
    numOfPage*pageSize should be below 256.
    pageSize is non contiguous.
    page boundary is 256/numOfPages.
    Set numOfPage and pageSize accordingly.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
int32_t TXSLZR_DrvPageConfig(TXSLZR_HwIDType hwId, TXSLZR_NumOfPageType numOfPage, uint32_t pageSize)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t idx = 0UL;
    uint32_t idx_end = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if ((numOfPage * pageSize) >= 256UL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(TXSLZR_NUM_OF_PAGE_1 == numOfPage) {
            TXSLZR_Regs[hwId]->page_sz[0UL] &= ~TXSLZR_PAGE_SZ_0_0_MASK ;
            TXSLZR_Regs[hwId]->page_sz[0UL] |= (pageSize & TXSLZR_PAGE_SZ_0_0_MASK);
        } else if (TXSLZR_NUM_OF_PAGE_2 == numOfPage) {
            TXSLZR_Regs[hwId]->page_sz[0UL] &= ~(TXSLZR_PAGE_SZ_0_1_MASK | TXSLZR_PAGE_SZ_0_0_MASK);
            TXSLZR_Regs[hwId]->page_sz[0UL] |= (((pageSize << TXSLZR_PAGE_SZ_0_1_SHIFT) & TXSLZR_PAGE_SZ_0_1_MASK) | (pageSize & TXSLZR_PAGE_SZ_0_0_MASK));
        } else {
            if(TXSLZR_NUM_OF_PAGE_4 == numOfPage) {
                idx_end = 1UL ;
            } else if (TXSLZR_NUM_OF_PAGE_8 == numOfPage) {
                idx_end = 2UL ;
            } else {
                idx_end = 4UL ;
            }
            for(idx=0UL; idx<idx_end; idx++) {
                TXSLZR_Regs[hwId]->page_sz[idx] =  (((pageSize << TXSLZR_PAGE_SZ_0_3_SHIFT) & TXSLZR_PAGE_SZ_0_3_MASK) |
                                                    ((pageSize << TXSLZR_PAGE_SZ_0_2_SHIFT) & TXSLZR_PAGE_SZ_0_2_MASK) |
                                                    ((pageSize << TXSLZR_PAGE_SZ_0_1_SHIFT) & TXSLZR_PAGE_SZ_0_1_MASK) |
                                                     (pageSize & TXSLZR_PAGE_SZ_0_0_MASK));
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR pattern generator enable/disable

    @code{.unparsed}
    Check hardware instance with max supported device.
    Enable(1)/Disable(0) pattern generator.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
int32_t TXSLZR_DrvPatGenEnable(TXSLZR_HwIDType hwId, uint8_t enaDis)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0U == enaDis) {
            TXSLZR_Regs[hwId]->control &= ~TXSLZR_CONTROL_PG_EN_MASK ;
        } else {
            TXSLZR_Regs[hwId]->control |=  TXSLZR_CONTROL_PG_EN_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets auto address increment mode for write

    @code{.unparsed}
    Check hardware instance with max supported device.
    Set auto address increment mode for write
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenWrMode(TXSLZR_HwIDType hwId, TXSLZR_AddrModeType mode)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(TXSLZR_ADDR_MODE_DEFAULT == mode) {
            TXSLZR_Regs[hwId]->pg_mem_ind_wr_control &= ~TXSLZR_TPMIWC_PG_MEM_IND_WR_INCR_MODE_MASK ;
        } else {
            TXSLZR_Regs[hwId]->pg_mem_ind_wr_control |= TXSLZR_TPMIWC_PG_MEM_IND_WR_INCR_MODE_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets write address

    @code{.unparsed}
    Check hardware instance with max supported device.
    Set write address.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenWrAddr(TXSLZR_HwIDType hwId, uint32_t addr)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        TXSLZR_Regs[hwId]->pg_mem_ind_wr_control &= ~TXSLZR_TPMIWC_PG_MEM_IND_WR_ADDR_MASK ;
        TXSLZR_Regs[hwId]->pg_mem_ind_wr_control |= (addr & TXSLZR_TPMIWC_PG_MEM_IND_WR_ADDR_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets write data

    @code{.unparsed}
    Check hardware instance with max supported device.
    Set write data.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenWrData(TXSLZR_HwIDType hwId, uint32_t data)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        TXSLZR_Regs[hwId]->pg_mem_ind_wr_data = data;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Writes pattern generator memory page

    @code{.unparsed}
    Check hardware instance with max supported device.
    If page size is 1, then set non-incrementing addressing mode and write the page data from page start address.
    If page size is more than 1, then set auto incrementing addressing mode and write the page data until page size.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
int32_t TXSLZR_DrvPatGenWrPage(TXSLZR_HwIDType hwId, TXSLZR_PageInfoType *pageInfo)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t idx  = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = TXSLZR_IntPatGenWrMode(hwId, TXSLZR_ADDR_MODE_DEFAULT);
        if(BCM_ERR_OK == ret) {
            ret = TXSLZR_IntPatGenWrAddr(hwId, pageInfo->pageStartAddr);
        }
        if(1UL == pageInfo->pageSize) {
            if(BCM_ERR_OK == ret) {
                ret = TXSLZR_IntPatGenWrData(hwId, pageInfo->pageData[pageInfo->pageStartAddr & TXSLZR_TPMIWC_PG_MEM_IND_WR_ADDR_MASK]);
            }
        } else {
            if(BCM_ERR_OK == ret) {
                ret = TXSLZR_IntPatGenWrMode(hwId, TXSLZR_ADDR_MODE_AUTO_INC);
            }
            for(idx = 0UL; idx<pageInfo->pageSize; idx++) {
                ret = TXSLZR_IntPatGenWrData(hwId, pageInfo->pageData[idx]);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets auto address increment mode for read

    @code{.unparsed}
    Check hardware instance with max supported device.
    Set auto address increment mode for read.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenRdMode(TXSLZR_HwIDType hwId, TXSLZR_AddrModeType mode)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(TXSLZR_ADDR_MODE_DEFAULT == mode) {
            TXSLZR_Regs[hwId]->pg_mem_ind_rd_control &= ~TXSLZR_TPMIRC_PG_MEM_IND_RD_INCR_MODE_MASK ;
        } else {
            TXSLZR_Regs[hwId]->pg_mem_ind_rd_control |= TXSLZR_TPMIRC_PG_MEM_IND_RD_INCR_MODE_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets read address

    @code{.unparsed}
    Check hardware instance with max supported device.
    Set read address.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenRdAddr(TXSLZR_HwIDType hwId, uint32_t addr)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        TXSLZR_Regs[hwId]->pg_mem_ind_rd_control &= ~TXSLZR_TPMIRC_PG_MEM_IND_RD_ADDR_MASK ;
        TXSLZR_Regs[hwId]->pg_mem_ind_rd_control |= (addr & TXSLZR_TPMIRC_PG_MEM_IND_RD_ADDR_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Read data from pattern generator memory

    @code{.unparsed}
    Check hardware instance with max supported device.
    Read data from pattern generator memory.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
static int32_t TXSLZR_IntPatGenRdData(TXSLZR_HwIDType hwId, uint32_t *data)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        *data = TXSLZR_Regs[hwId]->pg_mem_ind_rd_data & TXSLZR_PG_MEM_IND_RD_DATA_MASK ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Read pattern generator memory page

    @code{.unparsed}
    Check hardware instance and page info.
    If page size is 1, then set non-incrementing addressing mode and read the page data from page start address.
    If page size is more than 1, then set auto incrementing addressing mode and read the page data until page size.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
*/
int32_t TXSLZR_DrvPatGenRdPage(TXSLZR_HwIDType hwId, TXSLZR_PageInfoType *pageInfo)
{
    int32_t ret   = BCM_ERR_OK;
    uint32_t idx  = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = TXSLZR_IntPatGenRdMode(hwId, TXSLZR_ADDR_MODE_DEFAULT);
        if(BCM_ERR_OK == ret) {
           ret = TXSLZR_IntPatGenRdAddr(hwId, pageInfo->pageStartAddr);
        }
        if(1UL == pageInfo->pageSize) {
            if(BCM_ERR_OK == ret) {
                ret = TXSLZR_IntPatGenRdData(hwId, &pageInfo->pageData[pageInfo->pageStartAddr & TXSLZR_TPMIRC_PG_MEM_IND_RD_ADDR_MASK]);
            }
        } else {
            if(BCM_ERR_OK == ret) {
                ret = TXSLZR_IntPatGenRdMode(hwId, TXSLZR_ADDR_MODE_AUTO_INC);
            }
            for(idx = 0UL; idx < pageInfo->pageSize; idx++) {
                ret = TXSLZR_IntPatGenRdData(hwId, &pageInfo->pageData[idx]);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set PRBS configuration

    @code{.unparsed}
    Check hardware instance and PRBS configuration.
    Set the PRBS configuration like polynomial, inversion and reverse.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_TXSLZR_PRBS
*/
int32_t TXSLZR_DrvPrbsConfigSet(TXSLZR_HwIDType hwId, TXSLZR_PrbsConfigType *prbsConfig)
{
    int32_t ret = BCM_ERR_OK;
    TXSLZR_TEST_CTRL_TYPE test_ctrl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == prbsConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        test_ctrl = TXSLZR_Regs[hwId]->test_ctrl ;
        test_ctrl &= ~(TXSLZR_TEST_CTRL_PG_PRBS_CFG_REVERSE_MASK | TXSLZR_TEST_CTRL_PG_PRBS_CFG_INV_MASK | TXSLZR_TEST_CTRL_PG_PRBS_CFG_ORDER_MASK);
        test_ctrl |=  (prbsConfig->poly    << TXSLZR_TEST_CTRL_PG_PRBS_CFG_ORDER_SHIFT  ) & TXSLZR_TEST_CTRL_PG_PRBS_CFG_ORDER_MASK   ;
        test_ctrl |=  (prbsConfig->invert  << TXSLZR_TEST_CTRL_PG_PRBS_CFG_INV_SHIFT    ) & TXSLZR_TEST_CTRL_PG_PRBS_CFG_INV_MASK     ;
        test_ctrl |=  (prbsConfig->reverse << TXSLZR_TEST_CTRL_PG_PRBS_CFG_REVERSE_SHIFT) & TXSLZR_TEST_CTRL_PG_PRBS_CFG_REVERSE_MASK ;
        TXSLZR_Regs[hwId]->test_ctrl = test_ctrl ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Enable/Disable PRBS generator

    @code{.unparsed}
    Check hardware instance with max supported device.
    Enable/Disable PRBS generator.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC
    @trace #BRCM_SWREQ_TXSLZR_PRBS
*/
int32_t TXSLZR_DrvPrbsEnable(TXSLZR_HwIDType hwId, uint32_t ena_dis)
{
    int32_t ret = BCM_ERR_OK;
    TXSLZR_TEST_CTRL_TYPE test_ctrl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        test_ctrl = TXSLZR_Regs[hwId]->test_ctrl ;
        test_ctrl &= ~(TXSLZR_TEST_CTRL_PG_PRBS_EN_MASK | TXSLZR_TEST_CTRL_PG_EN_MASK);
        test_ctrl |=  (ena_dis << TXSLZR_TEST_CTRL_PG_PRBS_EN_SHIFT ) & TXSLZR_TEST_CTRL_PG_PRBS_EN_MASK ;
        test_ctrl |=  (ena_dis << TXSLZR_TEST_CTRL_PG_EN_SHIFT      ) & TXSLZR_TEST_CTRL_PG_EN_MASK      ;
        TXSLZR_Regs[hwId]->test_ctrl = test_ctrl ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief TXSLZR enable interrupts

    @code{.unparsed}
    Check hardware instance with max supported device.
    Enable sequencer single shot done interrupt.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INTERRUPT
*/
int32_t TXSLZR_DrvEnableInterrupt(TXSLZR_HwIDType hwId, TXSLZR_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & TXSLZR_INTERRUPT_SEQ_SINGLE_SHOT_DONE)) {
            if(0UL == aEnable) {
                TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eier &= ~TXSLZR_INTR_ATIE2_ENABLE_SEQ_SINGLE_SHOT_DONE_MASK ;
            } else {
                TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eier |=  TXSLZR_INTR_ATIE2_ENABLE_SEQ_SINGLE_SHOT_DONE_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR interrupt status

    @code{.unparsed}
    Check hardware instance with max supported device.
    Get sequencer single shot done interrupt status.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_TXSLZR_INTERRUPT
*/
int32_t TXSLZR_DrvInterruptStatus(TXSLZR_HwIDType hwId, TXSLZR_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == intrSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eipr & TXSLZR_INTR_ATIE_P_SEQ_SINGLE_SHOT_DONE_MASK)) {
            *intrSts |= TXSLZR_INTERRUPT_SEQ_SINGLE_SHOT_DONE ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR event status

    @code{.unparsed}
    Check hardware instance with max supported device.
    Get sequencer single shot done event status.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_TXSLZR_INTERRUPT
*/
int32_t TXSLZR_DrvEventStatus(TXSLZR_HwIDType hwId, TXSLZR_InterruptType *eventSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == eventSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eisr & TXSLZR_INTR_ATIE1_SEQ_SINGLE_SHOT_DONE_W2CLR_MASK)) {
            *eventSts |= TXSLZR_INTERRUPT_SEQ_SINGLE_SHOT_DONE ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR clear interrupts

    @code{.unparsed}
    Check hardware instance with max supported device.
    Clear sequencer single shot done interrupt status.
    @endcode

    @trace #BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_TXSLZR_INTERRUPT
*/
int32_t TXSLZR_DrvClearInterrupt(TXSLZR_HwIDType hwId, TXSLZR_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    TXSLZR_INTR_ATIE1_TYPE acq_txslzr_intr_eisr = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(TXSLZR_DrvDev[hwId].state == TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        acq_txslzr_intr_eisr = TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eisr ;
        (void)acq_txslzr_intr_eisr ;
        if(0UL != (intrType & TXSLZR_INTERRUPT_SEQ_SINGLE_SHOT_DONE)) {
            TXSLZR_IntrRegs[hwId]->acq_txslzr_intr_eisr |= TXSLZR_INTR_ATIE1_SEQ_SINGLE_SHOT_DONE_W2CLR_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR sequencer mode configuration

    @code{.unparsed}
    validate hwId, Device state
    Update sequencer mode in sequence control register
    @endcode

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
    @trace #BRCM_SWARCH_TXSLZR_DRV_SEQ_MODE_CFG_PROC
*/

int32_t TXSLZR_DrvSeqModeConfig(TXSLZR_HwIDType  hwId,  uint8_t mode)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        TXSLZR_Regs[hwId]->seq_control &= ~TXSLZR_SEQ_CONTROL_CONT_MODE_MASK ;
        TXSLZR_Regs[hwId]->seq_control |= (mode == 1U) ? TXSLZR_SEQ_CONTROL_CONT_MODE_MASK : 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;

}
/**
    @brief TXSLZR sequencer control

    @code{.unparsed}
    validate hwId, Device state.
    Enable or disable Sequence control.
    @endcode

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
    @trace #BRCM_SWARCH_TXSLZR_DRV_SEQCONTROL_PROC
*/
int32_t TXSLZR_DrvSeqControl(TXSLZR_HwIDType  hwId,  uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t seqCtrl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    seqCtrl = TXSLZR_Regs[hwId]->seq_control ;
    seqCtrl &= ~(TXSLZR_SEQ_CONTROL_SEQ_SPI_CFG_MASK);
    seqCtrl |= ((uint32_t)enDis << TXSLZR_SEQ_CONTROL_SEQ_EN_SHIFT);
    TXSLZR_Regs[hwId]->seq_control = seqCtrl;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/**
    @brief TXSLZR sequencer SPI configuration

    @code{.unparsed}
    validate hwId, Device state.
    Configure Spi control, SPI sequence output and SPI config.
    @endcode

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
    @trace #BRCM_SWARCH_TXSLZR_DRV_SEQSPICFG_PROC
*/
int32_t TXSLZR_DrvSeqSpiCfg(TXSLZR_HwIDType  hwId, TXSLZR_SpiConfigType* spiCfg)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (spiCfg == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        TXSLZR_Regs[hwId]->seq_spi_control = spiCfg->spiControl;
        TXSLZR_Regs[hwId]->seq_control &= ~(TXSLZR_SEQ_CONTROL_SEQ_SPI_CFG_MASK | TXSLZR_SEQ_CONTROL_SEQ_OUTPUT_CFG_MASK);
        TXSLZR_Regs[hwId]->seq_control |= ((spiCfg->spiSeqOut << TXSLZR_SEQ_CONTROL_SEQ_OUTPUT_CFG_SHIFT) |
                                           (spiCfg->seqSpiCfg << TXSLZR_SEQ_CONTROL_SEQ_SPI_CFG_SHIFT));
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief TXSLZR sequencer SPI Memory configuration

    @code{.unparsed}
    Validate hwId, Device state
    Based on read/write configure read control and memory / Write control and memory
    @endcode

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
    @trace #BRCM_SWARCH_TXSLZR_DRV_SEQMEMRDWR_PROC
*/

int32_t TXSLZR_DrvSeqMemRdWr(TXSLZR_HwIDType  hwId, TXSLZR_SeqMemRdWrType *seqMem)
{
    int ret = BCM_ERR_OK;
    uint8_t index = 0U;
    uint32_t data=0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (seqMem == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if (seqMem->readWrite > 0U) {
            TXSLZR_Regs[hwId]->seq_mem_ind_wr_control = (TXSLZR_SEQ_MEM_IND_WR_CONTROL_INCR_MODE_MASK |
                                                         seqMem->startAddress);
            for (index = 0U; index < seqMem->dataSize; index++) {
                TXSLZR_Regs[hwId]->seq_mem_ind_wr_data = (seqMem->dataEndFlag[index] |
                        ((uint32_t)seqMem->dataRptCnt[index] << TXSLZR_SEQ_MEM_IND_WR_DATA_RPT_CNT_SHIFT)|
                        ((uint32_t)seqMem->spiData[index] << TXSLZR_SEQ_MEM_IND_WR_DATA_SPI_DATA_SHIFT));
            }
        } else {
            TXSLZR_Regs[hwId]->seq_mem_ind_rd_control = (TXSLZR_SEQ_MEM_IND_RD_CONTROL_INCR_MODE_MASK |
                                                      seqMem->startAddress);
            for (index = 0U; index < seqMem->dataSize; index++) {
                /* Data0 to be read at the end for increment mode*/
                data = TXSLZR_Regs[hwId]->seq_mem_ind_rd_data;
                seqMem->dataEndFlag[index] = (uint8_t)(data & TXSLZR_SEQ_MEM_IND_RD_DATA_END_FLAG_MASK);
                seqMem->dataRptCnt[index] =  (uint8_t)((data & TXSLZR_SEQ_MEM_IND_RD_DATA_RPT_CNT_MASK) >> TXSLZR_SEQ_MEM_IND_RD_DATA_RPT_CNT_SHIFT);
                seqMem->spiData[index] = (uint16_t)((data & TXSLZR_SEQ_MEM_IND_RD_DATA_SPI_DATA_MASK) >> TXSLZR_SEQ_MEM_IND_RD_DATA_SPI_DATA_SHIFT);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/**
    @brief TXSLZR sequencer Software SPI control

    @code{.unparsed}
    Validate hwId, Device state
    Based on read/write configure SPI software control and read/write data.
    @endcode

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
    @trace #BRCM_SWARCH_TXSLZR_DRV_SWSPIRDWR_PROC
*/
int32_t TXSLZR_DrvSwSpiRdWr(TXSLZR_HwIDType  hwId,  TXSLZR_SwSpiRdWrType *swSpi)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= TXSLZR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (swSpi == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (TXSLZR_DrvDev[hwId].state != TXSLZR_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    if (swSpi->readWrite > 0U) {
        TXSLZR_Regs[hwId]->seq_spi_sft_wr_data = swSpi->data;
        TXSLZR_Regs[hwId]->seq_spi_sft_control |= (TXSLZR_SEQ_SPI_SFT_CONTROL_WR_MASK);
        TXSLZR_Regs[hwId]->seq_spi_misc_control = swSpi->turnaround;
    } else {
        TXSLZR_Regs[hwId]->seq_spi_sft_control = (TXSLZR_SEQ_SPI_SFT_CONTROL_RD_MASK |
                                               swSpi->readCmd | swSpi->readCfg);
        swSpi->readDone = (uint8_t)(TXSLZR_Regs[hwId]->seq_spi_sft_rd_data >> TXSLZR_SEQ_SFT_RD_DATA_SPI_RD_DONE_SHIFT);
        swSpi->data = (uint16_t)(TXSLZR_Regs[hwId]->seq_spi_sft_rd_data & TXSLZR_SEQ_SFT_RD_DATA_SPI_SFT_RD_DATA_MASK);
        TXSLZR_Regs[hwId]->seq_spi_misc_control = swSpi->turnaround;
    }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @} */
