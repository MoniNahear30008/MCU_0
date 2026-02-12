/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvrdrv_ifc Transceiver Interface
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvrdrv_ifc
    @{
    @section sec_xcvrdrv_overview Overview
    Transcever driver provides interface to configure and control ethernet PHY.
    It supports following transceiver components:
      -# Internal copper PHY (100/1000BASE-T)
      -# On chip Serdes PHY(2500BASE-X) to connect external PHY
      transceiver. The path is selected based on Transceiver ID Type.
    | XCVR ID     | XCVR Instance     |
    |:-----------:|:-----------------:|
    | 0           |   Internal CU PHY |
    | 1           |   SERDES PHY      |
    Only one transceiver ID can be selected at a time either Internal PHY or Serdes PHY
    (External PHY path) depending on use case. Both internal PHY and external PHY can not be active at
    the same time.
    @section api_list List of Transceiver APIs
    | API Name | Brief Description   |
    |:-----------:|:-------------:|
    | @ref ETHER_CommonInit                  | Initialize Ethernet common       |
    | @ref ETHXCVR_DrvInit                   | Initialize transceiver driver    |
    | @ref ETHXCVR_DrvDeInit                 | De-Initialize transceiver driver |
    | @ref ETHXCVR_DrvSetMode                | Select PHY mode                  |
    | @ref ETHXCVR_DrvGetMode                | Get PHY mode                     |
    | @ref ETHXCVR_DrvSetMasterMode          | Set SGMII master mode            |
    | @ref ETHXCVR_DrvGetMasterMode          | Get SGMII master mode            |
    | @ref ETHXCVR_DrvGetLinkState           | Get link status information      |
    | @ref ETHXCVR_DrvGetSpeed               | Get PHY speed                    |
    | @ref ETHXCVR_DrvSetSpeed               | Set PHY speed                    |
    | @ref ETHXCVR_DrvGetDuplexMode          | Get duplex mode                  |
    | @ref ETHXCVR_DrvSetDuplexMode          | Set duplex mode                  |
    | @ref ETHXCVR_DrvGetFlowControl         | Get flow control                 |
    | @ref ETHXCVR_DrvSetFlowControl         | Set flow control                 |
    | @ref ETHXCVR_DrvSetLoopbackMode        | Set loopback mode                |
    | @ref ETHXCVR_DrvGetLoopbackMode        | Get loopback mode                |
    | @ref ETHXCVR_DrvSetJumboMode           | Set Jumbo mode                   |
    | @ref ETHXCVR_DrvGetJumboMode           | Get Jumbo mode                   |
    | @ref ETHXCVR_DrvSetAutoNegMode         | Set auto-negotiation mode        |
    | @ref ETHXCVR_DrvGetAutoNegStatus       | Get auto-negotiation status      |
    | @ref ETHXCVR_DrvRestartAutoNeg         | Restart auto-negotiation         |
    | @ref ETHXCVR_DrvGetPortConfig          | Get port configuration           |
    | @ref ETHXCVR_DrvGetPhyID               | Get PHY ID                       |
    | @ref ETHXCVR_DrvGetCableDiagResult     | Get cable diagnostics result     |
    | @ref ETHXCVR_DrvReset                  | PHY reset function               |
    | @ref ETHXCVR_DrvTC10Init               | Initialize TC10 HW               |
    | @ref ETHXCVR_DrvTC10EnableProgramMode  | TC10 Enable program mode         |
    | @ref ETHXCVR_DrvTC10EnableProgramDone  | TC10 Enable program done         |
    | @ref ETHXCVR_DrvTC10WakeUpReq          | TC10 Wakeup request              |
    | @ref ETHXCVR_DrvTC10SleepReq           | TC10 sleep request               |
    | @ref ETHXCVR_DrvTC10ForceSleepReq      | TC10 Force sleep request         |
    | @ref ETHXCVR_DrvTC10AllowSleepReq      | TC10 allow sleep request         |
    | @ref ETHXCVR_DrvTC10AllowMdiWake       | TC10 Allow wakeup through MDI    |
    | @ref ETHXCVR_DrvTC10SleepAbortReq      | TC10 sleep abort request         |
    | @ref ETHXCVR_DrvTC10EnableEvents       | TC10 enable events               |
    | @ref ETHXCVR_DrvTC10DisableEvents      | TC10 disable events              |
    | @ref ETHXCVR_DrvTC10Disable            | TC10 disable                     |

    @note
      To enable cable diagnostics, BRPHY1GV2_ACD_SUPPORT flag needs to be uncommented in Makefile
    @file ethxcvr.h
    @brief Ethernet Transceiver interface
    This header file contains the interface functions for Ethernet Transceiver

    @version 1.0 Imported from docx
*/

#ifndef ETHXCVR_H
#define ETHXCVR_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <xcvr_common.h>
#include <ethernet_common.h>

/**
    @name Ethernet Transceiver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_SWARCH_ETHXCVR_DRV_INIT_PROC                      (0x8516U)    /**< @brief #ETHXCVR_DrvInit                   */
#define BRCM_SWARCH_ETHXCVR_DRV_DEINIT_PROC                    (0x8517U)    /**< @brief #ETHXCVR_DrvDeInit                 */
#define BRCM_SWARCH_ETHXCVR_DRV_SETMODE_PROC                   (0x8518U)    /**< @brief #ETHXCVR_DrvSetMode                */
#define BRCM_SWARCH_ETHXCVR_DRV_GETMODE_PROC                   (0x8519U)    /**< @brief #ETHXCVR_DrvGetMode                */
#define BRCM_SWARCH_ETHXCVR_DRV_SETMASTERMODE_PROC             (0x851AU)    /**< @brief #ETHXCVR_DrvSetMasterMode          */
#define BRCM_SWARCH_ETHXCVR_DRV_GETMASTERMODE_PROC             (0x851BU)    /**< @brief #ETHXCVR_DrvGetMasterMode          */
#define BRCM_SWARCH_ETHXCVR_DRV_GETLINKSTATE_PROC              (0x851CU)    /**< @brief #ETHXCVR_DrvGetLinkState           */
#define BRCM_SWARCH_ETHXCVR_DRV_GETSPEED_PROC                  (0x851DU)    /**< @brief #ETHXCVR_DrvGetSpeed               */
#define BRCM_SWARCH_ETHXCVR_DRV_SETSPEED_PROC                  (0x851EU)    /**< @brief #ETHXCVR_DrvSetSpeed               */
#define BRCM_SWARCH_ETHXCVR_DRV_GETDUPLEXMODE_PROC             (0x851FU)    /**< @brief #ETHXCVR_DrvGetDuplexMode          */
#define BRCM_SWARCH_ETHXCVR_DRV_SETDUPLEXMODE_PROC             (0x8520U)    /**< @brief #ETHXCVR_DrvSetDuplexMode          */
#define BRCM_SWARCH_ETHXCVR_DRV_GETFLOWCONTROL_PROC            (0x8521U)    /**< @brief #ETHXCVR_DrvGetFlowControl         */
#define BRCM_SWARCH_ETHXCVR_DRV_SETFLOWCONTROL_PROC            (0x8522U)    /**< @brief #ETHXCVR_DrvSetFlowControl         */

#define BRCM_SWARCH_ETHXCVR_DRV_SETLOOPBACKMODE_PROC           (0x8523U)    /**< @brief #ETHXCVR_DrvSetLoopbackMode        */
#define BRCM_SWARCH_ETHXCVR_DRV_GETLOOPBACKMODE_PROC           (0x8524U)    /**< @brief #ETHXCVR_DrvGetLoopbackMode        */
#define BRCM_SWARCH_ETHXCVR_DRV_SETJUMBOMODE_PROC              (0x8525U)    /**< @brief #ETHXCVR_DrvSetJumboMode           */
#define BRCM_SWARCH_ETHXCVR_DRV_GETJUMBOMODE_PROC              (0x8526U)    /**< @brief #ETHXCVR_DrvGetJumboMode           */
#define BRCM_SWARCH_ETHXCVR_DRV_SETAUTONEGMODE_PROC            (0x8527U)    /**< @brief #ETHXCVR_DrvSetAutoNegMode         */
#define BRCM_SWARCH_ETHXCVR_DRV_GETAUTONEGSTATUS_PROC          (0x8528U)    /**< @brief #ETHXCVR_DrvGetAutoNegStatus       */
#define BRCM_SWARCH_ETHXCVR_DRV_RESTARTAUTONEG_PROC            (0x8529U)    /**< @brief #ETHXCVR_DrvRestartAutoNeg         */

#define BRCM_SWARCH_ETHXCVR_DRV_GETPORTCONFIG_PROC             (0x852FU)    /**< @brief #ETHXCVR_DrvGetPortConfig          */

#define BRCM_SWARCH_ETHXCVR_DRV_GETPHYID_PROC                  (0x8535U)    /**< @brief #ETHXCVR_DrvGetPhyID               */
#define BRCM_SWARCH_ETHXCVR_DRV_GET_CABLE_DIAG_RESULT_PROC     (0x8538U)    /**< @brief #ETHXCVR_DrvGetCableDiagResult     */
#define BRCM_SWARCH_ETHXCVR_DRV_RESET_PROC                     (0x853AU)    /**< @brief #ETHXCVR_DrvReset                  */

#define BRCM_SWARCH_ETHXCVR_DRV_TC10_INIT_PROC                 (0x853BU)    /**< @brief #ETHXCVR_DrvTC10Init               */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_MODE_PROC  (0x853CU)    /**< @brief #ETHXCVR_DrvTC10EnableProgramMode  */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_PROGRAM_DONE_PROC  (0x853EU)    /**< @brief #ETHXCVR_DrvTC10EnableProgramDone  */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_WAKEUP_REQ_PROC           (0x853FU)    /**< @brief #ETHXCVR_DrvTC10WakeUpReq          */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_REQ_PROC            (0x8540U)    /**< @brief #ETHXCVR_DrvTC10SleepReq           */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_FORCE_SLEEP_REQ_PROC      (0x8541U)    /**< @brief #ETHXCVR_DrvTC10ForceSleepReq      */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_SLEEP_REQ_PROC      (0x8542U)    /**< @brief #ETHXCVR_DrvTC10AllowSleepReq      */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_ALLOW_MDI_WAKE_PROC       (0x8543U)    /**< @brief #ETHXCVR_DrvTC10AllowMdiWake       */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_SLEEP_ABORT_REQ_PROC      (0x8544U)    /**< @brief #ETHXCVR_DrvTC10SleepAbortReq      */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_ENABLE_EVENTS_PROC        (0x8546U)    /**< @brief #ETHXCVR_DrvTC10EnableEvents       */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_DISABLE_EVENTS_PROC       (0x8547U)    /**< @brief #ETHXCVR_DrvTC10DisableEvents      */
#define BRCM_SWARCH_ETHXCVR_DRV_TC10_DISABLE_PROC              (0x8548U)    /**< @brief #ETHXCVR_DrvTC10Disable            */

/** @} */

/** @brief Initialize transceiver

    This API initializes the transceiver with the given configuration.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID            Index of the Transceiver
    @param[in]      aConfig        Pointer to transceiver configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
extern int32_t ETHXCVR_DrvInit(ETHXCVR_IDType aID,
                         const ETHXCVR_PortConfigType *const aConfig);

/** @brief De-initialize transceiver

    This API de-initializes the transceiver driver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID

    @post

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
extern int32_t ETHXCVR_DrvDeInit(ETHXCVR_IDType aID);

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

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
extern int32_t ETHXCVR_DrvReset(ETHXCVR_IDType aID);

/** @brief Set transceiver mode

    This API sets mode of the transceiver. If aMode is #ETHXCVR_MODE_DOWN, transceiver is disabled by driver
    (both RX/TX disabled). If aMode is #ETHXCVR_MODE_ACTIVE, transceiver is enabled by the driver (both RX/TX active)
    When #ETHXCVR_MODE_ACTIVE is used, it will decide whether to perform full power up (MII Ctrl Power bit) or perform
    local and remote wake up depending on whether the wakeup feature is disabled or not.


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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType aMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType *const aMode);

/** @brief Set transceiver master/slave mode

    This API sets master/slave mode of the transceiver. If aMode is
    master enable, transceiver is configured in master
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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType aMasterMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType *const aMasterMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetLinkState(ETHXCVR_IDType aID,
                                    ETHXCVR_LinkStateType *const aLinkState);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType *const aSpeed);

/** @brief Set transceiver speed

    This API sets the speed of Ethernet transceiver.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType aSpeed);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType *const aDuplexMode);

/** @brief Set transceiver duplex mode

    This API sets duplex mode of the Ethernet transceiver.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType aDuplexMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetFlowControl(ETHXCVR_IDType aID,
                                     ETHXCVR_FlowControlType *const aFlowControl);

/** @brief Set transceiver flow control

    This API sets flow control of the Ethernet transceiver.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetFlowControl(ETHXCVR_IDType aID,
                                      ETHXCVR_FlowControlType aFlowControl);

/** @brief Set transceiver loopback mode

    This API sets loopback mode of the transceiver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode     Loopback mode to be configured.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_LoopbackModeType aMode);

/** @brief Get transceiver loopback mode

    This API retrieves the current loopback mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMode    Pointer to retrieve loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Loopback mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Loopback mode is not supported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_LoopbackModeType *const aMode);

/** @brief Set transceiver jumbo mode

    This API sets jumbo packet mode of the transceiver.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType aMode);

/** @brief Get transceiver jumbo packet mode

    This API retrieves the current jumbo packet mode (enabled/disabled) for
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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType *const aJumboMode);


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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvSetAutoNegMode(ETHXCVR_IDType aID,
                                      ETHXCVR_BooleanType aMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetAutoNegStatus(ETHXCVR_IDType aID,
                                        ETHXCVR_AutoNegStatusType *const aStatus);

/** @brief Restart auto-negotiation

    This API restarts auto-negotiation for the transceiver.

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_DrvRestartAutoNeg(ETHXCVR_IDType aID);

/** @brief Get port configuration

    This API fetches the port configuration.

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetPortConfig(ETHXCVR_IDType aID, ETHXCVR_PortConfigType *const aConfig);

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

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_DrvGetPhyID(ETHXCVR_IDType aID, ETHXCVR_PhyIDType *const aPhyID);

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

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/

extern int32_t ETHXCVR_DrvGetCableDiagResult(ETHXCVR_IDType aID,
                                          const ETHXCVR_AcdParamsType aParams,
                                          ETHXCVR_CableDiagResultType *const aResult);

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

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10Init(ETHXCVR_IDType aID,
                                              const ETHXCVR_TC10PortConfigType *const aConfig);

/** @brief TC10 Wakeup Req

    This API requests a TC10 wake-up

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully requested a TC10 wake-up
    @retval     #BCM_ERR_INVAL_PARAMS   aConnHdl is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10WakeUpReq(ETHXCVR_IDType aID);

/** @brief TC10 Sleep Request

    This API requests a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully requested a TC10 sleep
    @retval     #BCM_ERR_INVAL_PARAMS   aConnHdl is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10SleepReq(ETHXCVR_IDType aID);

/** @brief TC10 Force Sleep Req

    This API forces a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully forced the phy into TC10 sleep state
    @retval     #BCM_ERR_INVAL_PARAMS   aConnHdl is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10ForceSleepReq(ETHXCVR_IDType aID);

/** @brief TC10 Sleep Abort Req

    This API aborts an ongoing TC10 sleep handshake

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID                 Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully aborted an ongoing sleep handshake
    @retval     #BCM_ERR_INVAL_PARAMS   aConnHdl is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Unsupported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR
*/
extern int32_t ETHXCVR_DrvTC10SleepAbortReq(ETHXCVR_IDType aID);

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

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10AllowSleepReq(ETHXCVR_IDType aID,
                                               ETHXCVR_BooleanType aMode);

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

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10AllowMdiWake(ETHXCVR_IDType aID,
                                              ETHXCVR_BooleanType aMode);


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

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10EnableEvents(ETHXCVR_IDType aID);

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

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10DisableEvents(ETHXCVR_IDType aID);

/** @brief TC10 Enable program mode

    This API start enable/disable TC10 proram state

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID            Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events disabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10EnableProgramMode(ETHXCVR_IDType aID, ETHXCVR_BooleanType aMode);


/** @brief TC10 Enable program done

    This API indicate to exit Host mode state

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID            Index of the transceiver
    @param[in]     wakeReason     WakeReason to handle standby mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events disabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10EnableProgramDone(ETHXCVR_IDType aID, ETHXCVR_WakeupReasonType wakeReason);


/** @brief TC10 Disable

    This API disables TC10.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID            Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK       TC10 disabled successfully

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern int32_t ETHXCVR_DrvTC10Disable(ETHXCVR_IDType aID);

#endif /* ETH_XCVR_H */

/** @} */
