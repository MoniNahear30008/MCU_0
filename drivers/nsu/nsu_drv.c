/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_nsudrv_drv_impl Network Synchronization Unit driver Design
    @ingroup grp_nsudrv

    @addtogroup grp_nsudrv_drv_impl
    @{

    @file drivers/nsu/nsu_drv.c
    @brief NSU Driver Design

    @version 0.1 Initial version
*/

#include <nsu.h>
#include <nsu_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>

/**
    @name NSU Driver Design IDs
    @{
    @brief Driver Design IDs for NSU

*/
#define BRCM_SWDSGN_NSU_DRV_INIT_PROC                        (0xB101U)   /**< @brief #NSU_DrvInit             */
#define BRCM_SWDSGN_NSU_DRV_DEINIT_PROC                      (0xB102U)   /**< @brief #NSU_DrvDeInit           */
#define BRCM_SWDSGN_NSU_DRV_RESET_SRC_CONFIG_PROC            (0xB103U)   /**< @brief #NSU_DrvResetSrcConfig   */
#define BRCM_SWDSGN_NSU_DRV_RESET_PROC                       (0xB104U)   /**< @brief #NSU_DrvReset            */
#define BRCM_SWDSGN_NSU_DRV_CONTROL_PROC                     (0xB105U)   /**< @brief #NSU_DrvControl          */
#define BRCM_SWDSGN_NSU_DRV_READ_BIN_COUNTER_PROC            (0xB106U)   /**< @brief #NSU_DrvReadBinCounter   */
#define BRCM_SWDSGN_NSU_DRV_READ_1588_COUNTER_PROC           (0xB107U)   /**< @brief #NSU_DrvRead1588Counter  */
#define BRCM_SWDSGN_NSU_DRV_EVENT_CAPTURE_CTRL_PROC          (0xB108U)   /**< @brief #NSU_DrvEventCaptureCtrl */
#define BRCM_SWDSGN_NSU_DRV_READ_EVENT_CAP_TS_PROC           (0xB109U)   /**< @brief #NSU_DrvReadEventCapTs   */
#define BRCM_SWDSGN_NSU_DRV_EVENT_GENCTRL_PROC               (0xB10AU)   /**< @brief #NSU_DrvEventGenCtrl     */
#define BRCM_SWDSGN_NSU_DRV_WREVENT_GEN_COUNTER_PROC         (0xB10BU)   /**< @brief #NSU_DrvWrEventGenCounter */
#define BRCM_SWDSGN_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC    (0xB10CU)   /**< @brief #NSU_DrvSystemTickCtrl   */
#define BRCM_SWDSGN_NSU_DRV_WR_SYSTICK_COUNTER_PROC          (0xB10DU)   /**< @brief #NSU_DrvWrSysTickCounter */

#define BRCM_SWDSGN_NSU_REGS_GLOBAL                          (0xB10EU)    /**< @brief #NSU_Regs */
#define BRCM_SWDSGN_NSU_DRV_DEV_TYPE                         (0xB10FU)    /**< @brief #NSU_DrvDevType */
#define BRCM_SWDSGN_NSU_DRV_DEV_GLOBAL                       (0xB110U)    /**< @brief #NSU_DrvDev */
/** @} */

/**
    @trace #BRCM_SWARCH_NSU_DRV_INIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_SRC_CONFIG_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_PROC
    @trace #BRCM_SWARCH_NSU_DRV_CONTROL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_BIN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_1588_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_CAPTURE_CTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_EVENT_CAP_TS_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_GENCTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WREVENT_GEN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WR_SYSTICK_COUNTER_PROC
    @trace #BRCM_SWREQ_NSU_INIT
*/
static NSU_RDBType * const NSU_Regs[NSU_MAX_HW_ID] =
{
#if (NSU_MAX_HW_ID == 0U)
#error "NSU_MAX_HW_ID == 0U"
#endif
    (NSU_RDBType *)NSU_BASE,
#if (NSU_MAX_HW_ID > 1U)
#error "NSU_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_NSU_DRV_INIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_SRC_CONFIG_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_PROC
    @trace #BRCM_SWARCH_NSU_DRV_CONTROL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_BIN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_1588_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_CAPTURE_CTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_EVENT_CAP_TS_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_GENCTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WREVENT_GEN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WR_SYSTICK_COUNTER_PROC
    @trace #BRCM_SWREQ_NSU_INIT
*/
typedef struct sNSU_DrvDevType {
    NSU_StateType   state;            /**< State of the driver                */
} NSU_DrvDevType;

/**
    @trace #BRCM_SWREQ_NSU_INIT
    @trace #BRCM_SWARCH_NSU_DRV_INIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_SRC_CONFIG_PROC
    @trace #BRCM_SWARCH_NSU_DRV_RESET_PROC
    @trace #BRCM_SWARCH_NSU_DRV_CONTROL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_BIN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_1588_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_CAPTURE_CTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_READ_EVENT_CAP_TS_PROC
    @trace #BRCM_SWARCH_NSU_DRV_EVENT_GENCTRL_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WREVENT_GEN_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC
    @trace #BRCM_SWARCH_NSU_DRV_WR_SYSTICK_COUNTER_PROC
*/
static NSU_DrvDevType NSU_DrvDev[NSU_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/** @brief Initialize NSU

    This API initializes the given NSU hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_NSU_INIT
    @trace  #BRCM_SWARCH_NSU_DRV_INIT_PROC

    @code{.unparsed}
    Validate HW index.
    Move driver state to NSU_STATE_INITIALIZED
    @endcode

    @limitations None
*/

int32_t NSU_DrvInit(NSU_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        NSU_DrvDev[hwId].state = NSU_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-initialize NSU

    This API de-initializes NSU. Resets the given NSU controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_NSU_INIT
    @trace  #BRCM_SWARCH_NSU_DRV_DEINIT_PROC

    @code{.unparsed}
    Validate HW index.
    Move driver state to NSU_STATE_UNINIT
    @endcode

    @limitations None
*/
int32_t NSU_DrvDeInit (NSU_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        NSU_DrvDev[hwId].state = NSU_STATE_UNINIT;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief NSU reset configuration.

    This API configures reset source.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      nsuRstSrc    0 : NSU counters are reset by software only
                                 1 : NSU counters are reset by both software
                                     and hardware input (nsu_hw_rst).

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid nsuRstSrc
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET
    @trace  #BRCM_SWARCH_NSU_DRV_RESET_SRC_CONFIG_PROC

    @code{.unparsed}
    Validate HW index, Driver state and nsuRstSrc .
    Write control register with user specified nsuRstSrc.
    @endcode


    @limitations None
*/
int32_t NSU_DrvResetSrcConfig(NSU_HwIDType hwId, uint8_t nsuRstSrc)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if(nsuRstSrc > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        NSU_Regs[hwId]->control &= NSU_CONTROL_RST_SRC_MASK;
        NSU_Regs[hwId]->control |= (((uint32_t)nsuRstSrc & 1UL) << NSU_CONTROL_RST_SRC_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief NSU reset.

    This API resets NSU binary, 1588 and systick counters.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET
    @trace  #BRCM_SWARCH_NSU_DRV_RESET_PROC

    @code{.unparsed}
    Validate HW index, Driver state.
    Update contol register to enable reset.
    @endcode


    @limitations None
*/
int32_t NSU_DrvReset(NSU_HwIDType hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    }  else {
#endif
        NSU_Regs[hwId]->control |= NSU_CONTROL_RST_MASK;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief NSU enables or disables.

    This API Enables or disables NSU.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      nsuEnDis     0 : Disables Network Synchronization Unit.
                                 1 : Enables Network Synchronization Unit.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET
    @trace  #BRCM_SWARCH_NSU_DRV_CONTROL_PROC

    @code{.unparsed}
    Validate HW index, Driver state and nsuEnDis.
    Update contol register to Enable/Disable NSU based on user input.
    @endcode


    @limitations None
*/
int32_t NSU_DrvControl(NSU_HwIDType hwId, uint8_t nsuEnDis)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (nsuEnDis > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else {
#endif
        control = NSU_Regs[hwId]->control;
        control &= ~NSU_CONTROL_EN_MASK;
        control |= ((uint32_t)nsuEnDis << NSU_CONTROL_EN_SHIFT);
        NSU_Regs[hwId]->control = control;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief NSU reads binary NS counter.

    This API reads current binary nanosecond counter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[out]     lwNsCount    32-bit lower binary nanosecond counter
    @param[out]     hiNsCount    32-bit upper binary nanosecond counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwNsCount or hiNsCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_COUNTER
    @trace  #BRCM_SWARCH_NSU_DRV_READ_BIN_COUNTER_PROC

    @code{.unparsed}
    Validate HW index, Driver state, hiNsCount and lwNsCount.
    Read bin_l_nanosec_counter_rd followed by bin_u_nanosec_counter_rd.
    @endcode

    @limitations None
*/
int32_t NSU_DrvReadBinCounter(NSU_HwIDType hwId, uint32_t *lwNsCount, uint32_t *hiNsCount)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (lwNsCount == NULL || hiNsCount == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *lwNsCount = NSU_Regs[hwId]->bin_l_nanosec_counter_rd;
        *hiNsCount = NSU_Regs[hwId]->bin_u_nanosec_counter_rd;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief NSU reads 1588 NS counter.

    This API reads 1588 nanosecond counter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[out]     nsCount      32-bit 1588 nanosecond counter
    @param[out]     secCount     32-bit 1588 second counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid nsCount or secCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_COUNTER
    @trace  #BRCM_SWARCH_NSU_DRV_READ_1588_COUNTER_PROC

    @code{.unparsed}
    Validate HW index, Driver state, nsCount and secCount.
    Read m1588_nanosec_counter_rd followed by m1588_sec_counter_rd.
    @endcode

    @limitations None
*/
int32_t NSU_DrvRead1588Counter(NSU_HwIDType hwId, uint32_t *nsCount, uint32_t *secCount)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (nsCount == NULL || secCount == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *nsCount = NSU_Regs[hwId]->m1588_nanosec_counter_rd;
        *secCount = NSU_Regs[hwId]->m1588_sec_counter_rd;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Event capture Control.

    This API Enables/Disables event capture for the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]      eventCapEnDis 0 : Disable event capture
                                  1 : Enable event capture

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @trace  #BRCM_SWARCH_NSU_DRV_EVENT_CAPTURE_CTRL_PROC

    @code{.unparsed}
    Validate HW index, Driver state.
    Enable/Disable event capture based in chanMask
    @endcode

    @limitations None
*/
int32_t NSU_DrvEventCaptureCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t eventCapEnDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
       if (eventCapEnDis == 0U) {
           NSU_Regs[hwId]->evtcap_control &=  (~(uint32_t)chanMask);
       } else {
           NSU_Regs[hwId]->evtcap_control |= (uint32_t) chanMask;
       }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Event capture Timestamp counter.

    This API reads nano second binary time stamp captured on event trigger
    for specified channel mask.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      chanMask     Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[out]     lwNsCount    Base Address of an array of size eight uint32_t, 32-bit lower binary nanosecond counter
    @param[out]     upNsCount    Base Address of an array of size eight uint32_t, 32-bit upper binary nanosecond counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwNsCount or hiNsCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @trace  #BRCM_SWARCH_NSU_DRV_READ_EVENT_CAP_TS_PROC

    @code{.unparsed}
    Validate HW index, Driver state,lwNsCount and upNsCount.
    Read evtcap_ch0_timestamp_ns_u followed by evtcap_ch0_timestamp_ns_l.
    @endcode

    @limitations None
*/
int32_t NSU_DrvReadEventCapTs(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *lwNsCount, uint32_t *upNsCount)
{
    int ret = BCM_ERR_OK;
    uint8_t chanIdx = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (lwNsCount == NULL || upNsCount == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for (chanIdx =0U; chanIdx < NSUEVTCAPCHAN_SIZE; chanIdx++) {
            if ((chanMask & (1U << chanIdx)) > 0U) {
               upNsCount[chanIdx] = NSU_Regs[hwId]->nsuevtcapchan[chanIdx].evtcap_ch0_timestamp_ns_u;
               lwNsCount[chanIdx] = NSU_Regs[hwId]->nsuevtcapchan[chanIdx].evtcap_ch0_timestamp_ns_l;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Event Generation Control.

    This API Enables/Disables event generation for the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]      eventCapEnDis 0 : Disables event generation
                                  1 : Enables event generation

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @trace  #BRCM_SWARCH_NSU_DRV_EVENT_GENCTRL_PROC

    @code{.unparsed}
    Validate HW index, Driver state.
    Enable/Disable event generation based on chanMask
    @endcode

    @limitations None
*/
int32_t NSU_DrvEventGenCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t eventGenEnDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    }  else {
#endif
       if (eventGenEnDis == 0U) {
           NSU_Regs[hwId]->evtgen_control &= (~(uint32_t)chanMask);
       } else {
           NSU_Regs[hwId]->evtgen_control |= (uint32_t) chanMask;
       }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Event generation nanosecond counter.

    This API writes nanosecond binary counter at which channel x event must be generated.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId          Index of the NSU controller
    @param[in]     chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]     lwCmptimeNs   Base Address of an array of size eight uint32_t, lower 32-bit nanosecond binary counter
    @param[in]     upCmptimeNs   Base Address of an array of size eight uint32_t, upper 32-bit nanosecond binary counter
                                 [NOTE: Event generation happens when both lower and upper matches with NSU counter]

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwCmptimeNs or upCmptimeNs
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @trace  #BRCM_SWARCH_NSU_DRV_WREVENT_GEN_COUNTER_PROC

    @code{.unparsed}
    Validate HW index, Driver state, lwCmptimeNs and upCmptimeNs.
    Write evtgen_ch0_cmptime_ns_u and evtgen_ch0_cmptime_ns_l based on chanMask and user specified nanoseconds.
    @endcode

    @limitations None
*/
int32_t NSU_DrvWrEventGenCounter(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *lwCmptimeNs, uint32_t *upCmptimeNs)
{
    int ret = BCM_ERR_OK;
    uint8_t chanIdx = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (lwCmptimeNs == NULL || upCmptimeNs == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for (chanIdx =0U; chanIdx < NSUEVTGENCHAN_SIZE; chanIdx++) {
            if ((chanMask & (1U << chanIdx)) > 0U) {
               NSU_Regs[hwId]->nsuevtgenchan[chanIdx].evtgen_ch0_cmptime_ns_u = upCmptimeNs[chanIdx];
               NSU_Regs[hwId]->nsuevtgenchan[chanIdx].evtgen_ch0_cmptime_ns_l = lwCmptimeNs[chanIdx];
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief System tick Control.

    This API Enables/Disables system ticks for specified channels.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 3 represents channel 3.
    @param[in]      tickEnDis     0 : Disables system tick
                                  1 : Enables system tick

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid chanMask
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_SYSTICK
    @trace  #BRCM_SWARCH_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC

    @code{.unparsed}
    Validate HW index, Driver state and chanMask.
    Write systick_control based on chanMask and tickEnDis.
    @endcode

    @limitations None
*/
int32_t NSU_DrvSystemTickCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t tickEnDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chanMask > 0xFU) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else {
#endif
       if (tickEnDis  == 0U) {
           NSU_Regs[hwId]->systick_control &=  (~(uint32_t)chanMask);
       } else {
           NSU_Regs[hwId]->systick_control |= (uint32_t) chanMask;
       }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief System Tick counter value.

    This API writes system tick counter value for the specified channel(s).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId          Index of the NSU controller
    @param[in]     chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 3 represents channel 3.
    @param[in]     tickCnt       Base Address of an array of size four uint32_t, Systick counter reload value in 10ns multiple. The counter increments every 10ns.
                                 The value programmed should be 1 less than the target value.
                                 For example, for 10us, value programmed should be 990 or 0x3DE

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid  tickCnt
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_SYSTICK
    @trace  #BRCM_SWARCH_NSU_DRV_WR_SYSTICK_COUNTER_PROC

    @code{.unparsed}
    Validate HW index, Driver state and chanMask.
    Write systick_ch0_val based on chanMask and tickCnt.
    @endcode

    @limitations None
*/
int32_t NSU_DrvWrSysTickCounter(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *tickCnt)
{
    int ret = BCM_ERR_OK;
    uint8_t chanIdx = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= NSU_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NSU_DrvDev[hwId].state != NSU_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (tickCnt == NULL){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for (chanIdx =0U; chanIdx < NSU_SYSTICK_CH0_VAL_SIZE; chanIdx++) {
            if ((chanMask & (1U << chanIdx)) > 0U) {
               NSU_Regs[hwId]->systick_ch0_val[chanIdx] = tickCnt[chanIdx];
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @} */


