/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_xgmacmdio_drv_impl XGMAC MDIO Driver Design
    @ingroup grp_xgmacdrv

    @addtogroup grp_xgmacmdio_drv_impl
    @{

    @file drivers/ethernet/xgmac/mdio/xgmac_mdio.c
    @brief XGMAC MDIO Driver Design

    @version 0.1 Initial version
*/

#include "./../rdb/xgmac_core_rdb.h"
#include <xgmac.h>
#include <xgmac_mdio.h>
#include "hsio_common_rdb.h"
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>
#include <bcm_time.h>

/**
    @name XGMAC MDIO Driver Design IDs
    @{
    @brief Driver Design IDs for XGMAC MDIO
*/
#define BRCM_SWDSGN_XGMAC_MDIO_INIT_PROC           (0xB700U) /**< @brief #XGMAC_MdioInit          */
#define BRCM_SWDSGN_XGMAC_MDIO_DEINIT_PROC         (0xB701U) /**< @brief #XGMAC_MdioDeInit        */
#define BRCM_SWDSGN_XGMAC_MDIO_WRITE_PROC          (0xB702U) /**< @brief #XGMAC_MdioWrite         */
#define BRCM_SWDSGN_XGMAC_MDIO_READ_PROC           (0xB703U) /**< @brief #XGMAC_MdioRead          */
#define BRCM_SWDSGN_XGMAC_MDIO_RESET_PROC          (0xB704U) /**< @brief #XGMAC_MdioReset         */
#define BRCM_SWDSGN_XGMAC_CORE_REG_GLOBAL          (0xB705U) /**< @brief #XGMAC_CoreReg           */
#define BRCM_SWDSGN_XGMAC_DIVIDER_VALUE_TYPE       (0xB706U) /**< @brief #XGMAC_DividerValueType  */
#define BRCM_SWDSGN_XGMAC_MDIO_SINGLE_READ_MACRO   (0xB707U) /**< @brief #XGMAC_MDIO_SINGLE_READ  */
#define BRCM_SWDSGN_XGMAC_MDIO_SINGLE_WRITE_MACRO  (0xB708U) /**< @brief #XGMAC_MDIO_SINGLE_WRITE */
#define BRCM_SWDSGN_XGMAC_MDIO_DIV_VALUE_GET_PROC  (0xB709U) /**< @brief #XGMAC_MdioDivValueGet   */
#define BRCM_SWDSGN_XGMAC_WAIT_MII_UNTIL_DONE_PROC (0xB70AU) /**< @brief #XGMAC_WaitMiiUntilDone  */
#define BRCM_SWDSGN_XGMAC_COMMON_REGS_GLOBAL       (0xB70BU) /**< @brief #XGMAC_COMMON_REGS         */

/** @} */

/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_DEINIT_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_WRITE_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_READ_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_RESET_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/
static XGMAC_CORE_RDBType * const XGMAC_CoreReg[XGMAC_CORE_MAX_HW_ID] =
{

    (XGMAC_CORE_RDBType *)XG_CORE_BASE,

#if (XGMAC_CORE_MAX_HW_ID > 1UL)
#error "XGMAC_CORE_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @brief ET block base address

    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/
static HSIO_COMMON_RDBType * const XGMAC_COMMON_REGS[HSIO_COMMON_MAX_HW_ID] =
{
#if (HSIO_COMMON_MAX_HW_ID == 0UL)
#error "HSIO_COMMON_MAX_HW_ID = 0"
#endif
    (HSIO_COMMON_RDBType *const)HSIO_COMMON_BASE,
#if (HSIO_COMMON_MAX_HW_ID > 1UL)
#error "HSIO_COMMON_MAX_HW_ID > 1UL"
#endif
};


/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/

typedef uint32_t XGMAC_DividerValueType; /**< @brief When CRS bit is not set, divider is based on application clock range.            */
#define XGMAC_DIVIDER_VALUE_62   (0U)    /**< @brief Divider value 62 is set when application clock range input is 100-150 MHz range. */
#define XGMAC_DIVIDER_VALUE_102  (1U)    /**< @brief Divider value 62 is set when application clock range input is 150-250 MHz range. */
#define XGMAC_DIVIDER_VALUE_122  (2U)    /**< @brief Divider value 62 is set when application clock range input is 250-300 MHz range. */
#define XGMAC_DIVIDER_VALUE_142  (3U)    /**< @brief Divider value 62 is set when application clock range input is 300-350 MHz range. */
#define XGMAC_DIVIDER_VALUE_162  (4U)    /**< @brief Divider value 62 is set when application clock range input is 350-400 MHz range. */
#define XGMAC_DIVIDER_VALUE_202  (5U)    /**< @brief Divider value 62 is set when application clock range input is 400-500 MHz range. */

/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_READ_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION
*/
#define XGMAC_MDIO_SINGLE_READ   (0x3UL << XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_SHIFT)  /**< @brief MDIO single read command  */
/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_WRITE_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION
*/
#define XGMAC_MDIO_SINGLE_WRITE  (0x1UL << XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_SHIFT)  /**< @brief MDIO single write command  */

/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/
static int XGMAC_MdioDivValueGet(uint32_t csrClkInput, XGMAC_DividerValueType *divVal)
{
    int rv = BCM_ERR_OK;
    if((csrClkInput >=100U) && (csrClkInput <150U)) {
        *divVal = XGMAC_DIVIDER_VALUE_62;
    } else if((csrClkInput >=150U) && (csrClkInput <250U)) {
        *divVal = XGMAC_DIVIDER_VALUE_102;
    } else if((csrClkInput >=250U) && (csrClkInput <300U)) {
        *divVal = XGMAC_DIVIDER_VALUE_122;
    } else if((csrClkInput >=300U) && (csrClkInput <350U)) {
        *divVal = XGMAC_DIVIDER_VALUE_142;
    } else if((csrClkInput >=350U) && (csrClkInput <400U)) {
        *divVal = XGMAC_DIVIDER_VALUE_162;
    } else if((csrClkInput >=400U) && (csrClkInput <=500U)) {
        *divVal = XGMAC_DIVIDER_VALUE_202;
    } else {
        *divVal = XGMAC_DIVIDER_VALUE_62;
        rv = BCM_ERR_INVAL_PARAMS;
    }
    return rv;
}

/**
    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_WRITE_PROC
    @trace #BRCM_SWARCH_XGMAC_MDIO_READ_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/
static int XGMAC_WaitMiiUntilDone(XGMAC_HwIDType aId)
{
    int rv = BCM_ERR_OK;
    uint16_t sbusy, timeout_counter = 1000U;
    do {
        sbusy = (uint16_t)((XGMAC_CoreReg[aId]->mdio_single_command_control_data & XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_MASK) >> XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_SHIFT);
        --timeout_counter;
    } while ( (sbusy != 0U) && (timeout_counter > 0U) );

    if ( (timeout_counter == 0U) && (sbusy != 0U) ) {
        rv =  BCM_ERR_TIME_OUT;
    }
    return rv ;
}

/**
    @brief Initialize MDIO

    @code{.unparsed}
    Initialize MDIO
    @endcode

    @trace #BRCM_SWARCH_XGMAC_MDIO_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/

int XGMAC_MdioInit(XGMAC_HwIDType aId, XGMAC_MdioBusCfgType *aConfig)
{
    int rv = BCM_ERR_OK;
    XGMAC_DividerValueType divValue;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == aConfig) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(aConfig->clkRangeSel != 0U) {
            XGMAC_CoreReg[aId]->mdio_single_command_control_data |= XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CRS_MASK;
            XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_MASK;
            XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (aConfig->csrClkDiv << XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_SHIFT) ;
        } else {
            rv = XGMAC_MdioDivValueGet(aConfig->clkCsrInput, &divValue);
            if(BCM_ERR_OK == rv) {
                XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (divValue << XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_SHIFT) ;
            }
        }
        BCM_DelayUs(100UL);
        if (aConfig->preambleSuppressEn != 0U) {
            XGMAC_CoreReg[aId]->mdio_single_command_control_data |= XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_PSE_MASK;
        }
        /* Enable XGMAC sma output enable. */
        XGMAC_COMMON_REGS[aId]->hsio_ctrl_1 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_1_GPIO_SEL_MASK;
        XGMAC_COMMON_REGS[aId]->hsio_ctrl_1 |= 0x4UL;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return rv;
}

/** @brief De-Initialize MDIO bus

    @code{.unparsed}
    De-Initialize MDIO bus
    @endcode

    @trace #BRCM_SWARCH_XGMAC_MDIO_DEINIT_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
*/
int XGMAC_MdioDeInit(XGMAC_HwIDType aId)
{
    int rv = BCM_ERR_OK;
    /* Reset the MII clock TBD */
    rv = XGMAC_WaitMiiUntilDone(aId);
    if(BCM_ERR_OK == rv) {
        XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_MASK;
    }
    return rv;
}

/** @brief MDIO write

    @code{.unparsed}
    Perform MDIO write operation
    @endcode

    @trace #BRCM_SWARCH_XGMAC_MDIO_WRITE_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION
*/
int XGMAC_MdioWrite(XGMAC_HwIDType aId, uint32_t phyAddr, XGMAC_MdioAccClauseType clauseType, uint32_t phyRegAddr, uint16_t aData)
{
    uint32_t tmp = 0UL;
    uint32_t devAddr = (phyRegAddr >> 16UL);
    int rv = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else if((clauseType == XGMAC_MDIO_ACC_CLAUSE_CL22) && ((phyRegAddr > 31UL) || (phyAddr > 31UL))) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else if((clauseType != XGMAC_MDIO_ACC_CLAUSE_CL22) && ((devAddr > 31U) || (phyAddr > 31UL))) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(clauseType == XGMAC_MDIO_ACC_CLAUSE_CL22) {
            rv = XGMAC_WaitMiiUntilDone(aId);
            if(BCM_ERR_OK == rv) {
                /* Set port as Clause 22 */
                tmp |= BCM_BIT((uint32_t)phyAddr & 0x1FUL);
                XGMAC_CoreReg[aId]->mdio_clause_22_port |= tmp;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyAddr    & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_RA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyRegAddr & 0x1FUL);
                /* Write the data value */
                XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~(XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_MASK);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (aData | XGMAC_MDIO_SINGLE_WRITE | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_MASK);
                rv = XGMAC_WaitMiiUntilDone(aId);
            }
        } else {
            rv = XGMAC_WaitMiiUntilDone(aId);
            if(BCM_ERR_OK == rv) {
                /* Set port as Clause 45 */
                tmp |= BCM_BIT(phyAddr & 0x1FUL);
                XGMAC_CoreReg[aId]->mdio_clause_22_port &= ~tmp;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyAddr & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_DA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (devAddr & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_DA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_RA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyRegAddr & 0xFFFFUL);

				/* Write the data value */
                XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~(XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_MASK | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (aData | XGMAC_MDIO_SINGLE_WRITE);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_MASK);
                rv = XGMAC_WaitMiiUntilDone(aId);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return rv;
}

/** @brief MDIO read

    @code{.unparsed}
    Perform MDIO read operation
    @endcode

    @trace #BRCM_SWARCH_XGMAC_MDIO_READ_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION
*/
int XGMAC_MdioRead(XGMAC_HwIDType aId, uint32_t phyAddr, XGMAC_MdioAccClauseType clauseType, uint32_t phyRegAddr, uint16_t *const aData)
{
    uint32_t tmp = 0UL;
    uint32_t devAddr = (phyRegAddr >> 16UL);
    int rv = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else if((clauseType == XGMAC_MDIO_ACC_CLAUSE_CL22) && ((phyRegAddr > 31UL) || (phyAddr > 31UL))) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else if((clauseType != XGMAC_MDIO_ACC_CLAUSE_CL22) && ((devAddr > 31U) || (phyAddr > 31UL))) {
        rv = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(clauseType == XGMAC_MDIO_ACC_CLAUSE_CL22) {
            rv = XGMAC_WaitMiiUntilDone(aId);
            if(BCM_ERR_OK == rv) {
                /* Set port as Clause 22 */
                tmp |= BCM_BIT((uint32_t)phyAddr & 0x1FUL);
                XGMAC_CoreReg[aId]->mdio_clause_22_port |= tmp;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyAddr & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_RA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyRegAddr & 0x1FUL);
                /* Write the data value */
                XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~(XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_MASK);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (XGMAC_MDIO_SINGLE_READ | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_MASK);
                rv = XGMAC_WaitMiiUntilDone(aId);
                if(BCM_ERR_OK == rv) {
                    *aData = (uint16_t)(XGMAC_CoreReg[aId]->mdio_single_command_control_data & XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK);
                }
            }
        } else {
            rv = XGMAC_WaitMiiUntilDone(aId);
            if(BCM_ERR_OK == rv) {
                /* Set port as Clause 45 */
                tmp |= BCM_BIT((uint32_t)phyAddr & 0x1FUL);
                XGMAC_CoreReg[aId]->mdio_clause_22_port &= ~tmp;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyAddr & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_PA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_DA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (devAddr & 0x1FUL) << XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_DA_SHIFT;
                XGMAC_CoreReg[aId]->mdio_single_command_address &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_ADDRESS_RA_MASK;
                XGMAC_CoreReg[aId]->mdio_single_command_address |= (phyRegAddr & 0xFFFFUL);

                /* Write the data value */
                XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~(XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CMD_MASK | XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (XGMAC_MDIO_SINGLE_READ);
                XGMAC_CoreReg[aId]->mdio_single_command_control_data |= (XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SBUSY_MASK);
                rv = XGMAC_WaitMiiUntilDone(aId);
                if(BCM_ERR_OK == rv) {
                    *aData = (uint16_t)(XGMAC_CoreReg[aId]->mdio_single_command_control_data & XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_SDATA_MASK);
                }
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return rv;
}

/** @brief MDIO reset

    @code{.unparsed}
    Reset MDIO
    @endcode

    @trace #BRCM_SWARCH_XGMAC_MDIO_RESET_PROC
    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION

*/
int XGMAC_MdioReset(XGMAC_HwIDType aId)
{
    int rv = BCM_ERR_OK;
    /* Reset the clock TBD */
    rv = XGMAC_WaitMiiUntilDone(aId);
    if(BCM_ERR_OK == rv) {
        XGMAC_CoreReg[aId]->mdio_single_command_control_data &= ~XGMAC_CORE_MDIO_SINGLE_COMMAND_CONTROL_DATA_CR_MASK;
    }
    return rv;
}
/** @} */
