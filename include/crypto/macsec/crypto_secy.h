/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_crypto_secy_ifc SecY Interface
    @ingroup grp_macsecdrv

    @addtogroup grp_crypto_secy_ifc
    @{
     @section sec_secy_overview Overview
     From interface perspective, SecY operations can be grouped into below groups:
         -# Initialization of the SecY driver and hardware (#MACSEC_DrvInit).
         -# SecY Management:
             -# Creating a SecY entity(#MACSEC_DrvCreateSecY).
             -# Creating a rule on the hardware to map specific traffic to specific
                SecY entity (#MACSEC_DrvAddSecYLookupRule).
             -# Getting the underlying supported cipher suites and their capabilities and choosing
                 one of the them to use for the given SecY (#MACSEC_DrvGetCipherCap,#MACSEC_DrvSetCipherSuite).
             -# Creating transmit/receive secure channel for a SecY (#MACSEC_DrvCreateTxSC,
                #MACSEC_DrvCreateRxSC)
             -# Creating transmit/receive secure associations (#MACSEC_DrvCreateTxSA,
                 #MACSEC_DrvCreateRxSA).
             -# Enabling of the Transmit and receive SAs (#MACSEC_DrvSAEnable).
             -# Disabling the transmit and receive SAs (#MACSEC_DrvSADisable).
             -# Get next packet number information for transmit and receive SA
                 (#MACSEC_DrvSAGetNextPN).
             -# Set nextPN value for the receive SA (#MACSEC_DrvSASetNextPN).
             -# Get transmit and receive secure channel and secure associations statistics (#MACSEC_DrvGetTxSCStats,
                #MACSEC_DrvGetRxSCStats).
             -# Get SecY statistics counters (#MACSEC_DrvGetSecYStats).
             -# Delete SAs (#MACSEC_DrvDeleteSA).
             -# Delete Secure channels (#MACSEC_DrvDeleteTxSC, #MACSEC_DrvDeleteRxSC).
             -# Delete SecY Lookup rule (#MACSEC_DrvRemoveSecYLookupRule).
             -# Delete SecY entity(#MACSEC_DrvDeleteSecY).
         -# APIs to add rules for the packets which need to bypass the encryption/decryption.
            These packets are classified as control packet in the driver
            (refer to #MACSEC_DrvAddCPRuleOpt0, #MACSEC_DrvAddCPRuleOpt1).
         -# De-Initialization of the SecY driver and hardware (#MACSEC_DrvDeInit)

    @limitations None
    @section api_list List of MACsec APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref MACSEC_DrvInit         | Initialize MACsec driver |
    | @ref MACSEC_DrvDeInit       | De-Initialize MACsec driver |
    | @ref MACSEC_DrvAddCPRuleOpt0    | Add Control packet rule 0   |
    | @ref MACSEC_DrvAddCPRuleOpt1      | Add Control packet rule 1 |
    | @ref MACSEC_DrvRemoveCPRule | Remove CP Rule |
    | @ref MACSEC_DrvCreateSecY | Creates a SecY instance |
    | @ref MACSEC_DrvGetCipherCap  | Get Cipher capability |
    | @ref MACSEC_DrvSetCipherSuite     | Set Cipher capability |
    | @ref MACSEC_DrvAddSecYLookupRule     | Add SECY lookup rule |
    | @ref MACSEC_DrvRemoveSecYLookupRule  | Remove SecY lookup rule |
    | @ref MACSEC_DrvGetSecYStats | Get SECY statistics |
    | @ref MACSEC_DrvSetProtectFrame | Set protect frame |
    | @ref MACSEC_DrvSetReplayProtect | Set replay protect |
    | @ref MACSEC_DrvSetFrameValidate | Set frame validation |
    | @ref MACSEC_DrvDeleteSecY | Delete SECY instance |
    | @ref MACSEC_DrvCreateTxSC | Create TX Secure Channel |
    | @ref MACSEC_DrvCreateRxSC    | Create RX Secure Channel |
    | @ref MACSEC_DrvGetTxSCStats    | Get TX SC statistics |
    | @ref MACSEC_DrvGetRxSCStats  | Get RX SC statistics |
    | @ref MACSEC_DrvDeleteTxSC | Delete Tx SC |
    | @ref MACSEC_DrvDeleteRxSC   | Delete Rx SC |
    | @ref MACSEC_DrvCreateTxSA        | Create Tx Secure Association |
    | @ref MACSEC_DrvCreateRxSA        | Create Rx Secure Association |
    | @ref MACSEC_DrvSAEnable  | Enable SA |
    | @ref MACSEC_DrvSADisable | Disable SA |
    | @ref MACSEC_DrvSAGetStatus       | Get SA status |
    | @ref MACSEC_DrvSetPNThreshold     | Set packet threshold |
    | @ref MACSEC_DrvSAGetNextPN      | Get next packet number |
    | @ref MACSEC_DrvSASetNextPN       | Set next packet number |
    | @ref MACSEC_DrvDeleteSA      | Delete SA |
    | @ref MACSEC_DrvCtrlPortSet  | Set controlled port enable or disable |

    @file crypto_secy.h
    @brief Crypto MAC Security Entity (MACSEC) Interface
    This header file contains the interface functions for Crypto MAC Security Entity (MACSEC)

    @version 1.0 Initial Version
*/

#ifndef CRYPTO_SECY_H
#define CRYPTO_SECY_H

#include <crypto_common.h>
#include <crypto_secy_types.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <bcm_utils.h>

/**
    @name Crypto SecY Architecture IDs
    @{
    @brief Architecture IDs for Crypto SecY
*/
#define BRCM_SWARCH_MACSEC_HWIDTYPE_TYPE                   (0x8301U) /**< @brief #MACSEC_HwIDType            */
#define BRCM_SWARCH_MACSEC_SECYHDLTYPE_TYPE                (0x8302U) /**< @brief #MACSEC_SecYHdlType         */
#define BRCM_SWARCH_MACSEC_SAHDLTYPE_TYPE                  (0x8303U) /**< @brief #MACSEC_SAHdlType           */
#define BRCM_SWARCH_MACSEC_RULEHDLTYPE_TYPE                (0x8304U) /**< @brief #MACSEC_RuleHdlType         */
#define BRCM_SWARCH_MACSEC_AN_TYPE                         (0x8305U) /**< @brief #MACSEC_ANType              */
#define BRCM_SWARCH_MACSEC_MACSPEED_TYPE                   (0x8306U) /**< @brief #MACSEC_MacSpeedType        */
#define BRCM_SWARCH_MACSEC_DRV_INIT_PROC                   (0x8307U) /**< @brief #MACSEC_DrvInit                  */
#define BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC          (0x8308U) /**< @brief #MACSEC_DrvAddCPRuleOpt0         */
#define BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC          (0x8309U) /**< @brief #MACSEC_DrvAddCPRuleOpt1         */
#define BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC           (0x830AU) /**< @brief #MACSEC_DrvRemoveCPRule          */
#define BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC                 (0x830BU) /**< @brief #MACSEC_DrvDeInit                */
#define BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC             (0x830CU) /**< @brief #MACSEC_DrvCreateSecY            */
#define BRCM_SWARCH_MACSEC_DRV_GETCIPHERCAP_PROC           (0x830DU) /**< @brief #MACSEC_DrvGetCipherCap          */
#define BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC         (0x830EU) /**< @brief #MACSEC_DrvSetCipherSuite        */
#define BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC      (0x830FU) /**< @brief #MACSEC_DrvAddSecYLookupRule     */
#define BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC   (0x8310U) /**< @brief #MACSEC_DrvRemoveSecYLookupRule  */
#define BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC               (0x8311U) /**< @brief #MACSEC_DrvGetSecYStats              */
#define BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC        (0x8312U) /**< @brief #MACSEC_DrvSetProtectFrame       */
#define BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC       (0x8313U) /**< @brief #MACSEC_DrvSetReplayProtect      */
#define BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC       (0x8314U) /**< @brief #MACSEC_DrvSetFrameValidate      */
#define BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC             (0x8315U) /**< @brief #MACSEC_DrvDeleteSecY            */
#define BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC           (0x8316U) /**< @brief #MACSEC_DrvCreateTxSC            */
#define BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC           (0x8317U) /**< @brief #MACSEC_DrvCreateRxSC            */
#define BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC           (0x8318U) /**< @brief #MACSEC_DrvGetTxSCStats          */
#define BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC           (0x8319U) /**< @brief #MACSEC_DrvGetRxSCStats          */
#define BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC             (0x831AU) /**< @brief #MACSEC_DrvDeleteTxSC            */
#define BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC             (0x831BU) /**< @brief #MACSEC_DrvDeleteRxSC            */
#define BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC             (0x831CU) /**< @brief #MACSEC_DrvCreateTxSA            */
#define BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC             (0x831DU) /**< @brief #MACSEC_DrvCreateRxSA            */
#define BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC               (0x831EU) /**< @brief #MACSEC_DrvSAEnable              */
#define BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC              (0x831FU) /**< @brief #MACSEC_DrvSADisable             */
#define BRCM_SWARCH_MACSEC_DRV_GETSASTATUS_PROC            (0x8320U) /**< @brief #MACSEC_DrvSAGetStatus           */
#define BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC         (0x8321U) /**< @brief #MACSEC_DrvSetPNThreshold        */
#define BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC            (0x8322U) /**< @brief #MACSEC_DrvSAGetNextPN           */
#define BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC            (0x8323U) /**< @brief #MACSEC_DrvSASetNextPN           */
#define BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC               (0x8324U) /**< @brief #MACSEC_DrvDeleteSA              */
#define BRCM_SWARCH_MACSEC_DRV_CTRL_PORT_SET_PROC           (0x8325U) /**< @brief #MACSEC_DrvCtrlPortSet  */


/** @} */
/**
    @name MACSEC_HwIDType;
    @{
    @brief MACsec hardware index type

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef uint32_t MACSEC_HwIDType;       /**< @brief  MACsec hardware index type */
/** @} */

/**
    @name SecY Handle type
    @{
    @brief MACsec SecY handle returned by the driver when SecY is created.

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef BCM_HandleType MACSEC_SecYHdlType;       /**< @brief SecY Handle type */
/** @} */

/**
    @name MACSEC_SAHandleType
    @{
    @brief Secure association handle type
    Handle returned by the driver when SA is created

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef BCM_HandleType MACSEC_SAHdlType;

/** @} */

/**
    @name MACSEC_RuleHdlType
    @{
    @brief rule Handle type
    Handle returned by the driver when a rule is created.

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef BCM_HandleType MACSEC_RuleHdlType;
/** @} */

/**
    @name MACSEC_ANType
    @{
    @brief Association number type
    2-bit Association number mask and max values

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef uint8_t MACSEC_ANType;
#define MACSEC_AN_MASK      (3U)
#define MACSEC_AN_MAX       (3U)
/** @} */

/**
    @name MACSEC_MacSpeedType
    @{
    @brief MAC speed types
    MAC Speed configuration values

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef uint32_t MACSEC_MacSpeedType;
#define MACSEC_MACSPEED_100MBPS     (1UL)   /**< @brief 100 Mbps speed */
#define MACSEC_MACSPEED_1000MBPS    (2UL)   /**< @brief 1 Gbps speed */
#define MACSEC_MACSPEED_2500MBPS    (3UL)   /**< @brief 2.5 Gbps speed */
/** @} */

/**
    @brief Initialize SecY

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aID             MACsec controller index
    @param[in]      aCfg            Configuration


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                  Success
    @return     #BCM_ERR_INVAL_PARAMS        aCfg is NULL or invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvInit(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, const SECY_HwCfgType *const aConfig);

/**
    @brief Add Control packet rule 0

    Add a control packet rule in the hardware and return the handle for the rule.
    Multiple control packet rules (option 0) can be added on a given port (limited by underlying
    hardware resource. Refer to TRM for the details about how many rules are allowed).

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aID             MACsec controller index
    @param[in]      aRuleOpt           Filtering Rule
    @param[out]     aRuleHdl           Rule Handle


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is NULL or
                                         aRuleOpt is NULL or Invalid
    @return     #BCM_ERR_NOMEM           No more space to add new rule

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvAddCPRuleOpt0(BCM_HandleType aConnHdl, MACSEC_HwIDType aHwID, const SECY_RuleOpt0Type *const aRuleOpt,
                                MACSEC_RuleHdlType *const aRuleHdl);


/**
    @brief Add Control packet rule 1

    Add a control packet rule in the hardware and return the handle for the rule.
    Multiple control packet rules (option 1) can be added on a given port (limited by underlying
    hardware resource. Refer to TRM for the details about how many rules are allowed).

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aID             MACsec controller index
    @param[in]      aRuleOpt     Filtering Rule
    @param[out]     aRuleHdl     Rule Handle



    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is NULL or
                                         aRuleOpt1 is NULL or Invalid
    @return     #BCM_ERR_NOMEM           No more space to add new rule

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvAddCPRuleOpt1(BCM_HandleType aConnHdl, MACSEC_HwIDType aHwID, const SECY_RuleOpt1Type *const aRuleOpt,
                                MACSEC_RuleHdlType *const aRuleHdl);

/**
    @brief Remove Control packet Rule

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aRuleHdl           Rule Handle

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvRemoveCPRule(BCM_HandleType aConnHdl, MACSEC_RuleHdlType aRuleHdl);

/**
    @brief SecY HW De-Init

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aID             MACsec controller index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     BCM_ERR_INVAL_PARAMS     Invalid Port

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvDeInit(BCM_HandleType aConnHdl, MACSEC_HwIDType aID);

/**
    @brief Creates a SecY instance and returns the handle to user in aSecYHdl parameter

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]     aConnHdl    Connection handle (for remote access)
    @param[in]     aID          MACsec controller index
    @param[in]     aCfg                     SecY Config
    @param[out]     aSecYHdl                 SecY Handle

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is NULL or
                                         aCfg is NULL
    @return     #BCM_ERR_NOMEM           Can not create more SecY instances

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCreateSecY(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, const SECY_CfgType *const aCfg,
                             MACSEC_SecYHdlType *const aSecYHdl);

/**
    @brief Provides the supported cipher suites and its capabilities

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]  aID          MACsec controller index
    @param[in]      aCipherID          Cipher ID
    @param[out]     aCipherCap         Cipher Cap

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aCipherCap is NULL
    @return     #BCM_ERR_NOSUPPORT       Given cipher is not supported by hardware

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvGetCipherCap(MACSEC_HwIDType aID, SECY_CipherType aCipherID,
                                      SECY_CipherCapType *const aCap);

/**
    @brief Set the given cipher suite for SecY

    @note Cipher suite must be supported by the system (SECY_GetCipherCap() API has returned
          BCM_ERR_OK for given cipher suite)

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aSecYHdl         SecY Handle
    @param[in]      aCipherID        Cipher ID
    @param[in]      aConOffset       Confidentiality Offset

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_NOSUPPORT       Given cipher is not supported by hardware

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSetCipherSuite(MACSEC_SecYHdlType aSecYHdl, SECY_CipherType aCipherID,
                                        SECY_ConfOffsetType aOffset);


/**
    @brief Add a rule for SecY lookup and returns rule handle

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aSecYHdl                 SecY Handle
    @param[in]      aRuleOpt                 Filtering Rule
    @param[out]     aRuleHdl                 Rule Handle


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is NULL or
                                         aSecYHdl is invalid
                                         aRuleOpt is NULL or Invalid

    @return     BCM_ERR_NOMEM           No more space to add new rule

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvAddSecYLookupRule(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                                               const SECY_RuleOpt1Type *const aRule,
                                                                    MACSEC_RuleHdlType *const aRuleHandle);

/**
    @brief Remove Lookup Rule

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]     aRuleHdl           Rule Handle
    @param[in]     aForceRemove  If TRUE force remove TCAM rule otherwise it checks validity


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
int32_t MACSEC_DrvRemoveSecYLookupRule(BCM_HandleType aConnHdl, MACSEC_RuleHdlType aRuleHdl, BCM_BoolType aForceRemove);

/**
    @brief Stats query

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aSecYHdl        SecY Handle
    @param[out]     aStats          stats

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aStats is NULL or
                                         aSecYHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvGetSecYStats(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                               SECY_StatsType *const aStats);

/**
    @brief Set Frame Protect

    Enable/Disable frame protection on this SecY.
    If frame protection is disabled, SecY sends the packet without
    any modification (without SecTAG and encryption).

    @note This API is provided only for debug purpose.
    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aSecYHdl        SecY Handle
    @param[in]      aEnable         Enable Protect


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSetProtectFrame(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                                                        BCM_BoolType aEnable);

/**
    @brief Set Replay Protect

    Enable/Disable replay protection on SecY.
    Setting the replay protection option shall be done before the creation of the SA (best
    practice is to set this debug option right after creating the SecY). Setting this option
    has no effect on the existing SAs.
    If replay protection is required to be applied on the SecY which is already receiving,
    SAs must be deleted, then this API shall be called to set the required option and then new SA
    shall be created again.

    @note This API is provided only for debug purpose. Ideally, replay protection shall
           always be enabled in the system.

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl    Connection handle (for remote access)
    @param[in]      aSecYHdl        SecY Handle
    @param[in]      aEnable         Enable Replay Protect
    @param[in]      aWindow         Replay Protect Window

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSetReplayProtect(BCM_HandleType aHandle, MACSEC_SecYHdlType aSecYHdl,
                                                         uint32_t aEnable , uint32_t aWindow);

/**
    @brief Set Ingress frame validation

    Set the frame validate option for the ingress.
    Setting the frame validation option shall be done before the creation of the SA (best
    practice is to set this debug option right after creating the SecY). Setting frame
    validation option has no effect on the existing SAs.
    If validation option is required to be applied on the SecY which is already receiving,
    SAs must be deleted, then this API shall be called to set
    the required validation option and then new SA shall be created again.

    @note This API is provided only for debug purpose. Bydefault, when SecY is initialized
    validate frame option is set to STRICT.

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl      Connection handle (for remote access)
    @param[in]      aSecYHdl         SecY Handle
    @param[in]      aValidate        Enable Replay Protect


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is invalid or
                                         aValidate is invalid
    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSetFrameValidate(BCM_HandleType aHandle, MACSEC_SecYHdlType aSecYHdl,
                                                       SECY_ValidateFrameType aValidateFrame);

/**
    @brief Delete SecY

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl       Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvDeleteSecY(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl);

/**
    @brief Create secure transmit channel on given SecY

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl         Connection handle (for remote access)
    @param[in]      aSecYHdl          SecY Handle
    @param[in]      aSci                 SCI

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or
                                         aSecYHdl is Invalid
    @return     #BCM_ERR_BUSY            Secure channel already created

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCreateTxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI);

/**
    @brief Create receive secure channel on given SecY

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle
    @param[in]      aSci                  SCI


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or
                                         aSecYHdl is Invalid
    @return     #BCM_ERR_NOMEM           Can not create more RX channel on this SecY

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCreateRxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI);

/**
    @brief Get TxSC Stats

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle
    @param[in]      aSCI               SCI
    @param[out]     aTxStats           TX SCI Stat

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is Invalid or
                                         aTxStats is NULL or
                                         aSci is Invalid


    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvGetTxSCStats(BCM_HandleType aConnHdl , MACSEC_SecYHdlType aSecYHdl,
                                      SECY_SCIType *aSCI, SECY_TxSCStatsType *const aStats);

/**
    @brief Get RxSC Stats

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle
    @param[in]      aSCI               SCI
    @param[out]     aRxStats           RX SCI Stats


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSecYHdl is Invalid or
                                         aRxStats is NULL or
                                         aSci is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvGetRxSCStats(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                      SECY_SCIType *aSCI, SECY_RxSCStatsType *const aStats);

/**
    @brief Delete TxSC

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle
    @param[in]      aSCI               SCI

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or
                                         aSecYHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvDeleteTxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI);

/**
    @brief Delete RxSC

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSecYHdl           SecY Handle
    @param[in]      aSCI               SCI

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or
                                         aSecYHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvDeleteRxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI);

/**
    @brief Create TXSA

    Create a transmit secure association on given SecY and return the handle of the newly
    created SA.
    When SA is created, it is not enabled. Client needs to enable the SA by calling #MACSEC_DrvSAEnable
    function.

    Example:
    If XPN Cipher Suit is enabled and
    the Salt is 0xE630E81A48DE86A21C66FA6D
    and SSCI is 0x7A30C118
    then user should pass input to API in below format:

    aKey.salt[] = {E6,30,E8,1A,48,DE,86,A2,1C,66,FA,6D}
    aSsci       = 0x18C1307A (In reverse order)

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl            Connection handle (for remote access)
    @param[in]      aSecYHdl            SecY Handle.
    @param[in]      aAN                 Association number.
    @param[in]      aNextPN             Next PN number incremented by 1 to be used for this SA.
                                        (for example, if first packet has to be send with PN = 1,
                                        this shall be set to 0).
    @param[in]      aSci                SCI for which SA has to be created
    @param[in]      aSsci               32-bit SSCI value for this SCI if XPN cipher is used.
                                        If XPN cipher is not used for this SecY,
                                        this parameter shall be set to 0.
    @param[in]      aConfidentiality    TRUE: Enable confidentiality protection on this SA
                                        (payload encrypted and ICV is appended)
                                        FALSE: Disable confidentiality protection on this SA
                                        (data flows in plain, ICV is appended for the authentication)
    @param[in]      aKey                key
    @param[out]     aSAHdl              A non-zero SA handle (value is valid only when API
                                        returns BCM_ERR_OK)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or Invalid or
                                         aKey is NULL or Invalid or
                                         aSAHdl is NULL or
                                         aAN is > 3 or
                                         aNextPN is 0 or > 0xFFFFFFFF or
                                         Duplicate SSCI value for the same SCI or
                                         aSecYHdl is Invalid
    @return     #BCM_ERR_BUSY            Trying to update the active SA

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCreateTxSA(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, MACSEC_ANType aAN,
                                     uint64_t aNextPN, SECY_SCIType *aSCI, uint32_t aSSCI,
                                     uint32_t aConfidentiality, const SECY_KeyType *const aKey,
                                     MACSEC_SAHdlType  *const aSA);

/**
    @brief Create RXSA

    Create a receive secure association on given SecY and returns the handle of the newly
    created SA.

    Example:
    If XPN Cipher Suit is enabled and
    the Salt is 0xE630E81A48DE86A21C66FA6D
    and SSCI is 0x7A30C118
    then user should pass input to API in below format:

    aKey.salt[] = {E6,30,E8,1A,48,DE,86,A2,1C,66,FA,6D}
    aSsci       = 0x18C1307A (In reverse order)

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl            Connection handle (for remote access)
    @param[in]      aSecYHdl            SecY Handle.
    @param[in]      aAN                 Association number.
    @param[in]      aLowestPN           Lowest acceptable PN number for receive SA (must be > 0 and
                                        < 0xFFFFFFFFF for 32-bit PN and < 0xFFFFFFFFFFFFFFFF for
                                        XPN).
    @param[in]      aSci                SCI for which SA has to be created
    @param[in]      aSsci               32-bit SSCI value for this SCI if XPN cipher is used.
                                        If XPN cipher is not used for this SecY,
                                        this parameter shall be set to 0.
    @param[in]      aKey                key
    @param[out]     aSAHdl              A non-zero SA handle (value is valid only when API
                                        returns BCM_ERR_OK)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSci is NULL or Invalid or
                                         aKey is NULL or Invalid or
                                         aSAHdl is NULL or
                                         aAN is > 3 or
                                         aLowestPN is 0 or > 0xFFFFFFFF or
                                         Duplicate SSCI value for the same SCI or
                                         aSecYHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCreateRxSA(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, MACSEC_ANType aAN,
                                    uint64_t aLowestPN, SECY_SCIType *aSCI, uint32_t aSSCI,
                                    const SECY_KeyType *const aKey, MACSEC_SAHdlType *const aSAHdl);

/**
    @brief Enable SecTag Control Port

    Enable/Disable the controlled port of the SecY (Set MAC_Operational to TRUE for the controlled port of the SecY).

    @behavior sync, Non-Rentrant

    @pre SA is created using MACSEC_SACreate() API

    @param[in]      aConnHdl      Connection handle (for remote access)
    @param[in]      aSecYHdl       SecY Handle.
    @param[in]      aEnable         Enable or disable MACsec controlled port

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    SecY is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvCtrlPortSet(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, uint32_t aEnable);

/**
    @brief Enable SA

    Enable an inactive SA.
    For transmit, if this is not currently active SA (it's not encoding SA), it will become
    active after API returns (will become encoding SA). If SecY Tx SC was not transmitting, it will
    become ready for transmitting.  Old SA is not automatically deleted and
    Client needs to call #MACSEC_DrvSADisable() followed by #MACSEC_DrvDeleteSA() to free-up the resources.
    For receive, if this SA was not already enabled, it will become active. If none of the SAs
    for this SC was active, SC will become active and ready to receive.

    @behavior Sync, Non-Rentrant

    @pre SA is created using MACSEC_SACreate() API

    @param[in]      aConnHdl       Connection handle (for remote access)
    @param[in]      aSAHdl         SA Handle

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSAHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSAEnable(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSA);

/**
    @brief Disables an active SA

    @note None

    @behavior Sync, Non-Rentrant

    @pre SA is created using MACSEC_SACreate() API

    @param[in]      aConnHdl       Connection handle (for remote access)
    @param[in]      aSAHdl         SA handle

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSAHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSADisable(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl);

/**
    @brief Get SA status

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl       Connection handle (for remote access)
    @param[in]      aSAHdl           SA Handle
    @param[out]     aStatus          SA Status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aStatus is NULL or
                                         aSAHdl is Invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSAGetStatus(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl,
                                             SECY_SAStatusType *const aStatus);

/**
    @brief Set PN Threshold

    Set packet number threshold for the transmit SA. When transmit sequence number
    (nextPN value) for SA crosses this threshold, an IRQ event is generated which
    could be directly processed by the host (if supported by the board/hardware design).

    This API is provided to signal host software when PN threshold crosses configured
    value so that  MKA/Client can switch to new SA well before expiry.

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSAHdl             SA Handle
    @param[in]      aPNThrld           PN Threshold


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSAHdl is invalid or
                                         aPN is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSetPNThreshold(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t aPNThrld);

/**
    @brief Get nextPN value

    Reads the nextPN value of the active SA.
    For transmit SA, this returns the PN value of last
    successfully transmitted frame plus 1 (for example, when this
    API was called, last frame which was send had PN value 0x3,
    this API returns 0x4).
    For receive SA, this API return the lowest acceptable PN
    value (for example, when this API was called, SA has successfully
    received a frame with PN value of 0x3, this API returns 0x4).

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]        aConnHdl       Connection handle (for remote access)
    @param[in]      aSAHdl           SA Handle
    @param[out]     aPN              Next PN


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSAHdl is invalid or
                                         aPN is NULL

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSAGetNextPN(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t *aPN);

/**
    @brief Set nextPN value

    Set the next pn for the receive SA (lowest acceptable PN)

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl         Connection handle (for remote access)
    @param[in]      aSAHdl           SA Handle
    @param[in]      aPN              Next PN Threshold

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aSAHdl is invalid
                                         aPN value is greater than 32-bit and XPN is not enabled
    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvSASetNextPN(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t aPN);

/**
    @brief Delete SA

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aConnHdl           Connection handle (for remote access)
    @param[in]      aSAHdl             SA Handle

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Success
    @return     #BCM_ERR_INVAL_PARAMS    aRuleHdl is invalid

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
extern int32_t MACSEC_DrvDeleteSA(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl);

#endif /* SECY_H */

/** @} */
