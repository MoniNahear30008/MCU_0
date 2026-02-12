/*****************************************************************************
 Copyright 2014 Broadcom Corporation.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_transceiverdrv_impl Transceiver Driver Design
    @ingroup grp_transceiverdrv

    @addtogroup grp_transceiverdrv_impl
    @{

    @file drivers/ethernet/transceiver/ethxcvr_drv.c
    @brief Transceiver Driver Design

    @version 0.1 Initial version
*/

#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>
#include <ethxcvr.h>
#include "./inc/xcvr_functbl.h"

/**
    @name Transceiver driver Design IDs
    @{
    @brief Design IDs for Transceiver driver
*/
#define BRCM_SWDSGN_ETHXCVR_INVALID_STATE_CHECK_MACRO          (0xBC00U)    /**< @brief #ETHXCVR_INVALID_STATE_CHECK      */
#define BRCM_SWDSGN_ETHXCVR_DRVINIT_PROC                       (0xBC01U)    /**< @brief #ETHXCVR_DrvInit                  */
#define BRCM_SWDSGN_ETHXCVR_DRVGETSPEED_PROC                   (0xBC02U)    /**< @brief #ETHXCVR_DrvGetSpeed              */
#define BRCM_SWDSGN_ETHXCVR_DRVSETMODE_PROC                    (0xBC03U)    /**< @brief #ETHXCVR_DrvSetMode               */
#define BRCM_SWDSGN_ETHXCVR_DRVGETMODE_PROC                    (0xBC04U)    /**< @brief #ETHXCVR_DrvGetMode               */
#define BRCM_SWDSGN_ETHXCVR_DRVGETLINKSTATE_PROC               (0xBC05U)    /**< @brief #ETHXCVR_DrvGetLinkState          */
#define BRCM_SWDSGN_ETHXCVR_DRVGETDUPLEXMODE_PROC              (0xBC06U)    /**< @brief #ETHXCVR_DrvGetDuplexMode         */
#define BRCM_SWDSGN_ETHXCVR_DRVRWDATA_TYPE                     (0xBC0AU)    /**< @brief #ETHXCVR_DrvRWDataType            */
#define BRCM_SWDSGN_ETHXCVR_DRVRWDATA_GLOBAL                   (0xBC0BU)    /**< @brief #ETHXCVR_DrvRWData                */
#define BRCM_SWDSGN_ETHXCVR_DRVDEINIT_PROC                     (0xBC0DU)    /**< @brief #ETHXCVR_DrvDeInit                */
#define BRCM_SWDSGN_ETHXCVR_DRVSETSPEED_PROC                   (0xBC0EU)    /**< @brief #ETHXCVR_DrvSetSpeed              */
#define BRCM_SWDSGN_ETHXCVR_DRVSETMASTERMODE_PROC              (0xBC0FU)    /**< @brief #ETHXCVR_DrvSetMasterMode         */
#define BRCM_SWDSGN_ETHXCVR_DRVGETMASTERMODE_PROC              (0xBC10U)    /**< @brief #ETHXCVR_DrvGetMasterMode         */
#define BRCM_SWDSGN_ETHXCVR_DRVSETLOOPBACKMODE_PROC            (0xBC11U)    /**< @brief #ETHXCVR_DrvSetLoopbackMode       */
#define BRCM_SWDSGN_ETHXCVR_DRVGETLOOPBACKMODE_PROC            (0xBC12U)    /**< @brief #ETHXCVR_DrvGetLoopbackMode       */
#define BRCM_SWDSGN_ETHXCVR_DRVSETJUMBOMODE_PROC               (0xBC13U)    /**< @brief #ETHXCVR_DrvSetJumboMode          */
#define BRCM_SWDSGN_ETHXCVR_DRVRESTARTAUTONEG_PROC             (0xBC14U)    /**< @brief #ETHXCVR_DrvRestartAutoNeg        */
#define BRCM_SWDSGN_ETHXCVR_DRVGETAUTONEGSTATUS_PROC           (0xBC15U)    /**< @brief #ETHXCVR_DrvGetAutoNegStatus      */
#define BRCM_SWDSGN_ETHXCVR_DRVGETJUMBOMODE_PROC               (0xBC17U)    /**< @brief #ETHXCVR_DrvGetJumboMode          */
#define BRCM_SWDSGN_ETHXCVR_DRVSETDUPLEXMODE_PROC              (0xBC18U)    /**< @brief #ETHXCVR_DrvSetDuplexMode         */
#define BRCM_SWDSGN_ETHXCVR_DRVSETFLOWCONTROL_PROC             (0xBC19U)    /**< @brief #ETHXCVR_DrvSetFlowControl        */
#define BRCM_SWDSGN_ETHXCVR_DRVGETFLOWCONTROL_PROC             (0xBC1AU)    /**< @brief #ETHXCVR_DrvGetFlowControl        */
#define BRCM_SWDSGN_ETHXCVR_DRVGETPORTCONFIG_PROC              (0xBC1DU)    /**< @brief #ETHXCVR_DrvGetPortConfig         */
#define BRCM_SWDSGN_ETHXCVR_DRVSETAUTONEGMODE_PROC             (0xBC1EU)    /**< @brief #ETHXCVR_DrvSetAutoNegMode        */

#define BRCM_SWDSGN_ETHXCVR_DRV_GETPHYID_PROC                  (0xBC28U)    /**< @brief #ETHXCVR_DrvGetPhyID              */
#define BRCM_SWDSGN_ETHXCVR_DRV_GET_CABLE_DIAG_RESULT_PROC     (0xBC31U)    /**< @brief #ETHXCVR_DrvGetCableDiagResult    */
#define BRCM_SWDSGN_ETHXCVR_DRV_RESET_PROC                     (0xBC33U)    /**< @brief #ETHXCVR_DrvReset                 */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_INIT_PROC                 (0xBC34U)    /**< @brief #ETHXCVR_DrvTC10Init              */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_MODE_PROC  (0xBC35U)    /**< @brief #ETHXCVR_DrvTC10EnableProgramMode */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_DONE_PROC  (0xBC37U)    /**< @brief #ETHXCVR_DrvTC10EnableProgramDone */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_WAKEUP_REQ_PROC           (0xBC38U)    /**< @brief #ETHXCVR_DrvTC10WakeUpReq         */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_SLEEP_REQ_PROC            (0xBC39U)    /**< @brief #ETHXCVR_DrvTC10SleepReq          */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_FORCE_SLEEP_REQ_PROC      (0xBC3AU)    /**< @brief #ETHXCVR_DrvTC10ForceSleepReq     */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_ALLOW_SLEEP_REQ_PROC      (0xBC3BU)    /**< @brief #ETHXCVR_DrvTC10AllowSleepReq     */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_ALLOW_MDI_WAKE_PROC       (0xBC3CU)    /**< @brief #ETHXCVR_DrvTC10AllowMdiWake      */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_SLEEP_ABORT_REQ_PROC      (0xBC3DU)    /**< @brief #ETHXCVR_DrvTC10SleepAbortReq     */
#define BRCM_SWDSGN_ETHXCVR_TC10PORTCNTXT_GLOBAL               (0xBC3EU)    /**< @brief #ETHXCVR_TC10PortCntxt            */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_ENABLE_EVENTS_PROC        (0xBC40U)    /**< @brief #ETHXCVR_DrvTC10EnableEvents      */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_DISABLE_EVENTS_PROC       (0xBC41U)    /**< @brief #ETHXCVR_DrvTC10DisableEvents     */
#define BRCM_SWDSGN_ETHXCVR_DRV_TC10_DISABLE_PROC              (0xBC42U)    /**< @brief #ETHXCVR_DrvTC10Disable           */
/** @} */

/**
    @brief ETHXCVR Invalid state check

    @trace #BRCM_SWARCH_ETHXCVR_DRV_INIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_INVALID_STATE_CHECK     \
    if((ETHXCVR_DrvRWData[aID].initId != aID) || (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState)) { \
        retVal = BCM_ERR_INVAL_STATE;   \
    }

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETLINKSTATE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETSPEED_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_DrvRWDataType {
    ETHXCVR_IDType         initId;
    ETHXCVR_StateType      xcvrState;
    ETHXCVR_PortConfigType xcvrCfg;
    ETHXCVR_ModeType       mode;
} ETHXCVR_DrvRWDataType;

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETMODE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETLINKSTATE_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
static ETHXCVR_DrvRWDataType ETHXCVR_DrvRWData[2UL] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_INIT_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_WAKEUP_REQ_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_REQ_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_FORCE_SLEEP_REQ_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_ABORT_REQ_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_SLEEP_REQ_PROC
    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_MDI_WAKE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
static ETHXCVR_TC10PortContextType ETHXCVR_TC10PortCntxt[2UL] COMP_SECTION(".bss.drivers");

/** @brief Initialize transceiver

    This API initializes the transceiver with the given configuration.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID            Index of the Transceiver
    @param[in]      aConfig        Pointer to transceiver configuration

    @return     void

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_INIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
int32_t ETHXCVR_DrvInit(ETHXCVR_IDType aID,
                         const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc  = NULL;
    ETHXCVR_CntxtType            *phyCntxt = NULL;
    retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->init(aConfig->phy.hwID, phyCntxt, aConfig);
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrState = ETHXCVR_STATE_INIT;
            if (ETHXCVR_BOOLEAN_TRUE == aConfig->tc10Enable) {
                ETHXCVR_DrvRWData[aID].mode = ETHXCVR_MODE_DOWN;
            } else {
                ETHXCVR_DrvRWData[aID].mode = ETHXCVR_MODE_RESET;
            }
            /* Cache the config */
            BCM_MemCpy(&ETHXCVR_DrvRWData[aID].xcvrCfg, aConfig, sizeof(ETHXCVR_PortConfigType));
            ETHXCVR_DrvRWData[aID].initId = aID;
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief De-initialize transceiver

    This API de-initializes the transceiver driver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
int32_t ETHXCVR_DrvDeInit(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->deinit(config->phy.hwID, phyCntxt, config);
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrState = ETHXCVR_STATE_UNINIT;
            BCM_MemSet(&ETHXCVR_DrvRWData[aID].xcvrCfg, 0U, sizeof(ETHXCVR_PortConfigType));
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}
/** @brief Reset transceiver

    This API resets the transceiver driver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_TIME_OUT       Timed out while resetting the transceiver

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_RESET_PROC
    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
int32_t ETHXCVR_DrvReset(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->reset(config->phy.hwID, phyCntxt, config);
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].mode = ETHXCVR_MODE_RESET;
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Set transceiver mode

    This API sets mode of the transceiver. If aMode is #ETHXCVR_MODE_DOWN, transceiver is disabled by driver
    (both RX/TX disabled). If aMode is #ETHXCVR_MODE_ACTIVE, transceiver is enabled by the driver (both RX/TX active)
    When #ETHXCVR_MODE_ACTIVE is used, it will decide whether to perform full power up (MII Ctrl Power bit) or perform
    local and remote wake up depending on whether the wakeup feature is disabled or not


    @behavior ASync, Non Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_EAGAIN         Transceiver mode can not be set at this
                                        moment. Try again.
    @retval     #BCM_ERR_NOSUPPORT      No Support for the Transceiver mode

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setMode(config->phy.hwID, phyCntxt, config, aMode);
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].mode = aMode;
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Get transceiver mode

    This API retrieves the current mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMode       Pointer to Mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType *const aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_ModeType             phyMode = ETHXCVR_MODE_DOWN;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getMode(config->phy.hwID, phyCntxt, config, &phyMode);
        if (BCM_ERR_OK == retVal) {
            *aMode = phyMode;
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Set transceiver master/slave mode

    This API sets master/slave mode of the transceiver. If aMode is
    master, transceiver is configured in master
    mode. If aMode is master disable, transceiver is
    configured by driver in slave mode.

    @note Transceiver master/slave mode might not be applicable to all
    type of transceiver. If the mode is not applicable, this
    API shall return #BCM_ERR_NOSUPPORT


    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMasterMode Master/slave mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Master/slave mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting master/slave mode is not supported
    @retval     #BCM_ERR_EAGAIN         Master mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType aMasterMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setMasterMode(config->phy.hwID, phyCntxt, config, aMasterMode);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.phy.masterMode = aMasterMode;
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Get transceiver master/slave mode

    This API retrieves the current master/slave mode of the Ethernet transceiver.

    @note Transceiver master/slave mode might not be applicable to all
    types of transceivers. If the mode is not applicable, this
    API shall return #BCM_ERR_NOSUPPORT

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMasterMode Pointer to retrieve master/slave mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Master/slave mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Master/slave mode is not supported

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETMASTERMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getMasterMode(config->phy.hwID, phyCntxt, config, aMasterMode);
        /* Update the configuration */
        if (BCM_ERR_OK != retVal) {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Get transceiver link state

    This API retrieves the state of the Ethernet transceiver link.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aLinkState  Pointer to link state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved transceiver link state
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aLinkState is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETLINKSTATE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetLinkState(ETHXCVR_IDType aID,
                                    ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    ETHXCVR_LinkStateType        phyLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getLinkState(config->phy.hwID, phyCntxt, config, &phyLinkState);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            if ((ETHXCVR_LINKSTATE_DOWN == phyLinkState)) {
                *aLinkState = ETHXCVR_LINKSTATE_DOWN;
            } else {
                *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
            }
        } else {
            retVal =  BCM_ERR_INVAL_STATE;
        }
    }
    return retVal;
}

/** @brief Set transceiver speed

    This API set speed of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aSpeed      Speed of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver speed
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting speed is not supported
    @retval     #BCM_ERR_EAGAIN         Speed can not be changed at this
                                        moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType aSpeed)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setSpeed(config->phy.hwID, phyCntxt, config, aSpeed);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.speed = aSpeed;
        }
    }
    return retVal;
}

/** @brief Get transceiver speed

    This API retrieves the current speed of the Ethernet transceiver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aSpeed      Speed of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved transceiver speed
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETSPEED_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType *const aSpeed)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getSpeed(config->phy.hwID, phyCntxt, config, aSpeed);
    }
    return retVal;
}

/** @brief Set transceiver duplex mode

    This API set duplex mode of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aDuplexMode Duplex mode of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver duplex mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aDuplexMode is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting duplex mode is not supported
    @retval     #BCM_ERR_EAGAIN         Duplex mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setDuplexMode(config->phy.hwID, phyCntxt, config, aDuplexMode);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.duplex = aDuplexMode;
        }
    }
    return retVal;
}

/** @brief Get duplex mode

    This API retrieves the current duplex mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aDuplexMode Duplex Mode of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved current duplex mode of the transceiver
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aDuplexMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETDUPLEXMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;
    ETHXCVR_DuplexModeType       phyDuplex = ETHXCVR_DUPLEXMODE_FULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getDuplexMode(config->phy.hwID, phyCntxt, config, aDuplexMode);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            *aDuplexMode = phyDuplex;
        }
    }
    return retVal;
}

/** @brief Get flow control

    This API retrieves the current flow control of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID          Index of the transceiver
    @param[out]     aFlowControl Flow control of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved current flow control of the transceiver
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aFlowControl is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetFlowControl(ETHXCVR_IDType aID,
                                     ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t                      retVal = BCM_ERR_OK;
     const ETHXCVR_FuncTblType    *phyFunc = NULL;
     const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
     ETHXCVR_FlowControlType      phyFlowControl = ETHXCVR_FLOWCONTROL_NONE;
     ETHXCVR_CntxtType            *phyCntxt = NULL;

     ETHXCVR_INVALID_STATE_CHECK
     if(BCM_ERR_INVAL_STATE != retVal) {
         retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
     }
     if(BCM_ERR_OK == retVal) {
         retVal = phyFunc->getFlowControl(config->phy.hwID, phyCntxt, config, aFlowControl);
         /* Update the configuration */
         if (BCM_ERR_OK == retVal) {
             *aFlowControl = phyFlowControl;
         }
     }
     return retVal;
}
/** @brief Set transceiver flow control

    This API sets flow control of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID          Index of the transceiver
    @param[in]      aFlowControl Flow control of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver flow control
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aFlowControl  is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting flow control is not supported
    @retval     #BCM_ERR_EAGAIN         Flow control mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETFLOWCONTROL_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetFlowControl(ETHXCVR_IDType aID,
                                      ETHXCVR_FlowControlType aFlowControl)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setFlowControl(config->phy.hwID, phyCntxt, config, aFlowControl);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.flowControl = aFlowControl;
        }
    }
    return retVal;
}

/** @brief Set transceiver loopback mode

    This API sets loopback mode of the transceiver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Loopback mode to be configured.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Loopback mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid loopback mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting loopback mode is not supported
    @retval     #BCM_ERR_EAGAIN         Loopback mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_LoopbackModeType aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setLoopbackMode(config->phy.hwID, phyCntxt, config, aMode);
    }
    return retVal;
}

/** @brief Get transceiver loopback mode

    This API retrieves the current loopback mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMode       Pointer to retrieve loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Loopback mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Loopback mode is not supported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETLOOPBACKMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_LoopbackModeType *const aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getLoopbackMode(config->phy.hwID, phyCntxt, config, aMode);
    }
    return retVal;
}

/** @brief Set transceiver jumbo mode

    This API sets jumbo mode of the transceiver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Jumbo mode to be configured.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Jumbo mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid jumbo mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting jumbo mode is not supported
    @retval     #BCM_ERR_EAGAIN         Jumbo mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setJumboMode(config->phy.hwID, phyCntxt, config, aMode);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.jumbo = aMode;
        }
    }
    return retVal;
}

/** @brief Get transceiver jumbo mode

    This API retrieves the current jumbo packet mode enabled/disabled for
    the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aJumboMode  Pointer to retrieve jumbo mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Jumbo mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aJumboMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Jumbo packet mode is not supported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETJUMBOMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType *const aJumboMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_BooleanType          phyJumbo = ETHXCVR_BOOLEAN_FALSE;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getJumboMode(config->phy.hwID, phyCntxt, config, &phyJumbo);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            *aJumboMode = phyJumbo;
        }
    }
    return retVal;
}

/** @brief Set auto-negotiation mode

    This API sets the auto-negotiation mode for the transceiver. In case
    aMode is autonegotiation enable then it also performs auto-negotiation.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Auto-negotiation enable/disable mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Auto-negotiation mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid auto-negotiation mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting auto-negotiation mode is not supported
    @retval     #BCM_ERR_EAGAIN         Auto-negotiation mode can not be set
                                        at this moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_SETAUTONEGMODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvSetAutoNegMode(ETHXCVR_IDType aID,
                                      ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->setAutoNegMode(config->phy.hwID, phyCntxt, config, aMode);
        /* Update the configuration */
        if (BCM_ERR_OK == retVal) {
            ETHXCVR_DrvRWData[aID].xcvrCfg.autoNeg = aMode;
        }
    }
    return retVal;
}
/** @brief Get transceiver auto-negotiation status

    This API retrieves auto-negotiation status of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Index of the transceiver
    @param[out]     aStatus         Pointer to retrieve status of auto-negotiation

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver auto-negotiation status
                                        retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aStatus is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Auto-negotiation status is not supported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETAUTONEGSTATUS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetAutoNegStatus(ETHXCVR_IDType aID,
                                        ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_AutoNegStatusType autoNegStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->getAutoNegStatus(config->phy.hwID, phyCntxt, config, &autoNegStatus);
        /* Report down if any one of the status is down. Do not report error */
        if (BCM_ERR_OK == retVal) {
            /* If Bus And PHY COMPLETE OR PHY Complete and Bus Not Present */
            if (ETHXCVR_AUTONEGSTATUS_COMPLETE == autoNegStatus ) {
                *aStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
            } else if (ETHXCVR_AUTONEGSTATUS_NO_ABILITY == autoNegStatus){
                *aStatus = ETHXCVR_AUTONEGSTATUS_NO_ABILITY;
            } else { /* If all cases fails */
                *aStatus = ETHXCVR_AUTONEGSTATUS_INCOMPLETE;
            }
        }
    }
    return retVal;
}

/** @brief Restart auto-negotiation

    This API restarts auto-negotiation for the transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Auto-negotiation restarted successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_STATE    Auto-negotiation is not enabled
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Restarting auto-negotiation is not supported

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWARCH_ETHXCVR_DRV_RESTARTAUTONEG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
int32_t ETHXCVR_DrvRestartAutoNeg(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        if (ETHXCVR_BOOLEAN_TRUE == config->autoNeg) {
            retVal = phyFunc->restartAutoNeg(config->phy.hwID, phyCntxt, config);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief Get port config

    This API getches the port config

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aConfig     Pointer to copy the port config

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Config retrived successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETPORTCONFIG_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetPortConfig(ETHXCVR_IDType aID, ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        BCM_MemCpy(aConfig, &ETHXCVR_DrvRWData[aID].xcvrCfg, sizeof(ETHXCVR_PortConfigType));
    }
    return retVal;
}

/** @brief Get transceiver identifier

    This API obtains the PHY identifier.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[out]     aPhyID      PHY identifier

    @retval     #BCM_ERR_OK             successful
    @retval     #BCM_ERR_INVAL_PARAMS   invalid transceiver id
    @retval     #BCM_ERR_INVAL_PARAMS   aPhyID is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GETPHYID_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
int32_t ETHXCVR_DrvGetPhyID(ETHXCVR_IDType aID, ETHXCVR_PhyIDType *const aPhyID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;

    if((ETHXCVR_DrvRWData[aID].initId != aID) || (ETHXCVR_STATE_UNINIT == ETHXCVR_DrvRWData[aID].xcvrState)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
        if(BCM_ERR_OK == retVal) {
            retVal = phyFunc->getID(aID, aPhyID);
            /* Update the configuration */
            if (BCM_ERR_OK != retVal) {
                retVal =  BCM_ERR_INVAL_STATE;
            }
        }
    }
    return retVal;
}

/** @brief Get cable diagnostic result

    This API obtains the cable diagnostics result.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver.
    @param[out]     aResult     Pointer to result structure memory.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             success (aResult contains valid result).
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_BUSY          Cable diagnostic is running.

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_GET_CABLE_DIAG_RESULT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/

int32_t ETHXCVR_DrvGetCableDiagResult(ETHXCVR_IDType aID,
                                          const ETHXCVR_AcdParamsType aParams,
                                          ETHXCVR_CableDiagResultType *const aResult)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_FuncTblType    *phyFunc = NULL;
    const ETHXCVR_PortConfigType *config  = &ETHXCVR_DrvRWData[aID].xcvrCfg;
    ETHXCVR_CntxtType            *phyCntxt = NULL;

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetFnTblsCntxt(aID, &phyFunc);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = phyFunc->runCableDiag(config->phy.hwID, phyCntxt, config, aParams, aResult);
    }
    return retVal;
}

/** @brief TC10 Init

    This API initialises TC10 hardware logic

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[in]     aConfig      Port configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Initialised successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_INIT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10Init(ETHXCVR_IDType aID,
                                  const ETHXCVR_TC10PortConfigType *const aConfig)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->init != NULL) {
            retVal = tc10Tbl->init(aID, tc10Ctxt, aConfig);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Disable

    Disable TC10

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_DISABLE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10Disable(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType  *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->enable != NULL) {
            retVal = tc10Tbl->enable(aID, tc10Ctxt, ETHXCVR_BOOLEAN_FALSE);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Enable Program Start

    TC10 Enable Program Start

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[in]     aMode        Enable/Disable control

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_MODE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10EnableProgramMode(ETHXCVR_IDType aID, ETHXCVR_BooleanType aMode)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType  *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
    retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->enPgmState != NULL) {
            retVal = tc10Tbl->enPgmState(aID, tc10Ctxt, aMode);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Enable Program End

    TC10 Enable Program End

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[in]     wakeReason   Wake reason to handle standby mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_DONE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10EnableProgramDone(ETHXCVR_IDType aID, ETHXCVR_WakeupReasonType wakeReason)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType  *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->exitHostMode != NULL) {
            retVal = tc10Tbl->exitHostMode(aID, tc10Ctxt, wakeReason);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}
/** @brief TC10 Wakeup Req

    This API requests a TC10 wake-up

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully requested a TC10 wake-up
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_WAKEUP_REQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10WakeUpReq(ETHXCVR_IDType aID)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->wakeUpReq != NULL) {
            retVal = tc10Tbl->wakeUpReq(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Sleep Request

    This API requests a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully requested a TC10 sleep
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_REQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10SleepReq(ETHXCVR_IDType aID)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->sleepReq != NULL) {
            retVal = tc10Tbl->sleepReq(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Force Sleep Req

    This API forces a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully forced the phy into TC10 sleep state
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_FORCE_SLEEP_REQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10ForceSleepReq(ETHXCVR_IDType aID)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->forceSleepReq != NULL) {
            retVal = tc10Tbl->forceSleepReq(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Sleep Abort Req

    This API aborts an ongoing TC10 sleep handshake

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully aborted an ongoing sleep handshake
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_ABORT_REQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR
*/
int32_t ETHXCVR_DrvTC10SleepAbortReq(ETHXCVR_IDType aID)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->sleepAbortReq != NULL) {
            retVal = tc10Tbl->sleepAbortReq(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief Allow Sleep request

    Sleep requests from remote partner will be aborted by default during boot-up.
    This API enables/disables accepting sleep requests from remote.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[in]     aMode        Boolean to indicate if sleep requests have to be allowed or not

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_SLEEP_REQ_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_DrvTC10AllowSleepReq(ETHXCVR_IDType aID,
                                               ETHXCVR_BooleanType aMode)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->allowSleepReq != NULL) {
            retVal = tc10Tbl->allowSleepReq(aID, tc10Ctxt, aMode);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief Allow wakeup through MDI

    This API enables/disables wakeup of phy wake requests from link partner.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[in]     aMode        TRUE = Enable wakeup on WUP, FALSE = Disable wakeup on WUP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_MDI_WAKE_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10AllowMdiWake(ETHXCVR_IDType aID,
                                              ETHXCVR_BooleanType aMode)

{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->allowMdiWake != NULL) {
            retVal = tc10Tbl->allowMdiWake(aID, tc10Ctxt, aMode);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}


/** @brief TC10 Enable Events

    This API enables events specific to TC10.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID            Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events enabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_EVENTS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10EnableEvents(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->enableEvents != NULL) {
            retVal = tc10Tbl->enableEvents(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief TC10 Disable Events

    This API disables events specific to TC10.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID            Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events disabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWARCH_ETHXCVR_DRV_TC10_DISABLE_EVENTS_PROC
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10DisableEvents(ETHXCVR_IDType aID)
{
    int32_t                      retVal = BCM_ERR_OK;
    const ETHXCVR_ExtPhyTC10FuncTblType *tc10Tbl = NULL;
    ETHXCVR_TC10PortContextType *tc10Ctxt = &ETHXCVR_TC10PortCntxt[aID];

    ETHXCVR_INVALID_STATE_CHECK
    if(BCM_ERR_INVAL_STATE != retVal) {
        retVal = ETHXCVR_PlatformGetTc10FnTblsCntxt(aID, &tc10Tbl);
    }
    if(BCM_ERR_OK == retVal) {
        if(tc10Tbl->disableEvents != NULL) {
            retVal = tc10Tbl->disableEvents(aID, tc10Ctxt);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}
/** @} */

