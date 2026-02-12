/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_crypto_secy_types_ifc SecY Types
    @ingroup grp_macsecdrv

    @addtogroup grp_crypto_secy_types_ifc
    @{

    @file crypto_secy_types.h
    @brief Crypto MAC Security Entity (MACSEC) Types
    This header file contains the Types for Crypto MAC Security Entity (MACSEC)

    @version 1.0 Initial Version
*/

#ifndef CRYPTO_SECY_TYPES_H
#define CRYPTO_SECY_TYPES_H

#include <crypto_common.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <bcm_utils.h>

/**
    @name Crypto SecY Architecture IDs
    @{
    @brief Architecture IDs for Crypto SecY
*/

#define BRCM_SWARCH_SECY_MACRO                       (0x8941U) /**< @brief #SECY_XPN_SALT_LEN          */
#define BRCM_SWARCH_SECY_HW_CFG_TYPE                 (0x8942U) /**< @brief #SECY_HwCfgType             */
#define BRCM_SWARCH_SECY_VALIDATE_FRAME_TYPE         (0x8943U) /**< @brief #SECY_ValidateFrameType     */
#define BRCM_SWARCH_SECY_RULE_OPT0_TYPE              (0x8944U) /**< @brief #SECY_RuleOpt0Type          */
#define BRCM_SWARCH_SECY_RULE_OPT1_TYPE              (0x8945U) /**< @brief #SECY_RuleOpt1Type          */
#define BRCM_SWARCH_SECY_CFG_TYPE                    (0x8946U) /**< @brief #SECY_CfgType               */
#define BRCM_SWARCH_SECY_CIPHER_TYPE                 (0x8947U) /**< @brief #SECY_CipherType            */
#define BRCM_SWARCH_SECY_CIPHER_CAP_TYPE             (0x8948U) /**< @brief #SECY_CipherCapType         */
#define BRCM_SWARCH_SECY_CONF_OFFSET_TYPE            (0x8949U) /**< @brief #SECY_ConfOffsetType        */
#define BRCM_SWARCH_SECY_RULE_MATCH_TYPE             (0x894AU) /**< @brief #SECY_RuleMatchType         */
#define BRCM_SWARCH_SECY_RULE_DIR_TYPE               (0x894BU) /**< @brief #SECY_RuleDirType           */
#define BRCM_SWARCH_SECY_SCI_TYPE                    (0x894CU) /**< @brief #SECY_SCIType               */
#define BRCM_SWARCH_SECY_KEY_TYPE                    (0x894DU) /**< @brief #SECY_KeyType               */
#define BRCM_SWARCH_SECY_SA_STATUS_TYPE              (0x894EU) /**< @brief #SECY_SAStatusType          */
#define BRCM_SWARCH_SECY_STATS_TYPE                  (0x894FU) /**< @brief #SECY_StatsType             */
#define BRCM_SWARCH_SECY_TXSCSTATS_TYPE              (0x8950U) /**< @brief #SECY_TxSCStatsType         */
#define BRCM_SWARCH_SECY_RXSCSTATS_TYPE              (0x8951U) /**< @brief #SECY_RxSCStatsType         */

#define BRCM_SWARCH_SECY_HANDLE_TYPE                 (0x8952U) /**< @brief #SECY_HandleType            */
#define BRCM_SWARCH_SECY_BUS_TYPE                    (0x8953U) /**< @brief #SECY_BusType               */

#define BRCM_SWARCH_SECY_HW_STATUS_TYPE              (0x8956U) /**< @brief #SECY_HwStatusType          */
#define BRCM_SWARCH_SECY_PORT_STATUS_TYPE            (0x8957U) /**< @brief #SECY_PortStatusType        */


/** @} */

/**
    @name CRYPTO SECY Macros
    @{
    @brief Crypto SECY Macros

    @trace #BRCM_SWREQ_SECY
**/
#define SECY_XPN_SALT_LEN    (12UL)
#define SECY_KEY_LEN         (32UL)
#define SECY_KEYID_LEN       (16UL)
#define SECY_MSG_MAX_SIZE    (448UL)
/** @} */

/**
    @brief CRYPTO configuration type

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_HwCfgType {
    uint32_t    checkKaY;   /**< @brief Enable detection of KaY frame
                                (E bit = 1 and C bit = 0 in SecTAG).
                                If set to TRUE, these frames are marked as control
                                frame and shall be handled by KaY.
                                If set to FALSE, these frames will be marked as
                                invalid CRYPTO Tagged frame and will be dropped */
    uint32_t    enableQtagParse; /**< @brief Enable detection of 802.1Q tag
                                (802.1Q tag in clear) */
} SECY_HwCfgType;

/**
    @name SECY_ValidateFrameType
    @{
    @brief Ingress tagged frame validation options.

    These values are used for 802.1AE compliant processing Frame when no MACsec secure channel is found.
    validation options as defined in section 10.6 of IEEE Std 802.1AE-2018: Media Access Control (MAC) Security

    These options help in staged deployement of macsec in the network system
    Setting frame validation option to NULL also disables frame protection of the SecY.

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_ValidateFrameType;
#define SECY_VALIDATE_FRAME_DISABLED   (0UL) /**< @brief no validation (remove SecTAG and ICV if
                                                            present in the received frame and don't
                                                            discard the frame) */
#define SECY_VALIDATE_FRAME_CHECK      (1UL) /**< @brief check (enable check but don't discard
                                                            the invalid frames) */
#define SECY_VALIDATE_FRAME_STRICT     (2UL) /**< @brief strict check (enable validation
                                                      and discard invalid frames) */
#define SECY_VALIDATE_FRAME_NULL       (3UL) /**< @brief no processing, do not remove SecTAG or
                                                            ICV from frame */
/** @} */

/**
    @name SECY_CipherType
    @{
    @brief Crypto EUI-64 cipher id
    64-bit EUI Cipher IDs as defined by the 802.1AE specification

    @trace #BRCM_SWREQ_SECY
*/
typedef uint64_t SECY_CipherType;
#define SECY_CIPHER_GCM_AES_128         (0x0080C20001000001ULL) /**< @brief GCM AES 128 bit */
#define SECY_CIPHER_GCM_AES_256         (0x0080C20001000002ULL) /**< @brief GCM AES 256 bit */
#define SECY_CIPHER_GCM_AES_XPN_128     (0x0080C20001000003ULL) /**< @brief GCM AES XPN 128 */
#define SECY_CIPHER_GCM_AES_XPN_256     (0x0080C20001000004ULL) /**< @brief GCM AES XPN 256 */
/** @} */

/**
    @name SECY_ConfOffsetType
    @{
    @brief Confidentiality offset
    Confidentiality offsets supported by the cipher suite (bit mask values)

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_ConfOffsetType;
#define SECY_CONF_OFFSET_0             (0U)   /**< @brief confidentiality offset 0 */
#define SECY_CONF_OFFSET_30            (1U)   /**< @brief confidentiality offset 30 */
#define SECY_CONF_OFFSET_50            (2U)   /**< @brief confidentiality offset 50 */
/** @} */

/**
    @name SECY_CipherCapType
    @{
    @brief Cipher capabilites types

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_CipherCapType;
#define SECY_CIPHER_CAP_INTEGRITY          (1UL) /**< @brief integrity protection without confidentiality */
#define SECY_CIPHER_CAP_CONFIDENTIALITY    (2UL) /**< @brief integrity with confidentiality */
#define SECY_CIPHER_CAP_CONF_OFFSET        (3UL) /**< @brief integrity with confidentiality offset */
/** @} */

/**
    @name SECY_RuleDirType
    @{
    @brief SECY_RuleDirType
    Control packet rule direction (egress/ingress)

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_RuleDirType;
#define SECY_RULE_DIR_EGRESS       (1U)     /**< @brief Add the rule only for egress direction */
#define SECY_RULE_DIR_INGRESS      (2U)     /**< @brief Add the rule only for ingress direction */
#define SECY_RULE_DIR_BOTH         (3U)     /**< @brief Apply the rule in both the directions */
/** @} */

/**
    @name SECY_RuleMatchType
    @{
    @brief SECY_RuleMatchType
    Packet classification rule match mask. Based on the match type,
    respective fields in the MACSEC_RuleOpt0 fields are used

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_RuleMatchType;
#define SECY_RULE_MATCH_DA_ONLY             (1UL)   /**< @brief Match only DA value
                                                            (#SECY_RuleOpt0Type.macDAStart) */
#define SECY_RULE_MATCH_ETHTYPE_ONLY        (2UL)   /**< @brief match only ethtype in
                                                            (#SECY_RuleOpt0Type.ethType) */
#define SECY_RULE_MATCH_DA_ETHTYPE          (3UL)   /**< @brief Matching both DA and EthType maynot be supported
                                                                on all platform. Same functionality can be achieved
                                                                through the CPRuleOpt1 (refer #sSECY_RuleOpt1Type) */
#define SECY_RULE_MATCH_DA_RANGE_ONLY       (4UL)   /**< @brief match DA range value
                                                             (match is when macDAStart <= MAC_DA <=
                                                              macDAEnd) */
#define SECY_RULE_MATCH_DA_RANGE_ETHTYPE    (5UL)   /**< @brief match DA range and ethType
                                                            (match is when macDAStart <= MAC_DA <=
                                                            macDAEnd & ethertype) */
#define SECY_RULE_MATCH_DA_CONST            (6UL)  /**< @brief match DA const value in
                                                                #SECY_RuleOpt0Type.macDAStart.
                                                                Match happens on 48-bits of the DA. */
/** @} */

/**
    @name SECY_HandleMaskType
    @{
    @brief Handle masks and shifts
    @trace #BRCM_SWREQ_SECY
*/
typedef uint64_t SECY_HandleType;
#define SECY_HANDLE_HASH_SHIFT        (48U)
#define SECY_HANDLE_HASH_MASK         (0xFFFFULL << SECY_HANDLE_HASH_SHIFT)
#define SECY_HANDLE_SEED_SHIFT        (42U)
#define SECY_HANDLE_SEED_MASK         (0x1FULL << SECY_HANDLE_SEED_SHIFT)
#define SECY_HANDLE_HWID_SHIFT        (37U)
#define SECY_HANDLE_HWID_MASK         (0x1FULL << SECY_HANDLE_HWID_SHIFT)
#define SECY_HANDLE_SECYID_SHIFT      (32U)
#define SECY_HANDLE_SECYID_MASK       (0x1FULL << SECY_HANDLE_SECYID_SHIFT)
#define SECY_HANDLE_AN_SHIFT          (26U)
#define SECY_HANDLE_AN_MASK           (0x3ULL << SECY_HANDLE_AN_SHIFT)
#define SECY_HANDLE_SCID_SHIFT        (21U)
#define SECY_HANDLE_SCID_MASK         (0x1FULL << SECY_HANDLE_SCID_SHIFT)
#define SECY_HANDLE_SAID_SHIFT        (16U)
#define SECY_HANDLE_SAID_MASK         (0x1FULL << SECY_HANDLE_SAID_SHIFT)
#define SECY_HANDLE_RULEID_SHIFT      (8U)
#define SECY_HANDLE_RULEID_MASK       (0xFFULL << SECY_HANDLE_RULEID_SHIFT)
#define SECY_HANDLE_RULEID_EGRESS_SHIFT   (8U)
#define SECY_HANDLE_RULEID_EGRESS_MASK    (0xFULL << SECY_HANDLE_RULEID_EGRESS_SHIFT)
#define SECY_HANDLE_RULEID_INGRESS_SHIFT  (12U)
#define SECY_HANDLE_RULEID_INGRESS_MASK   (0xFULL << SECY_HANDLE_RULEID_INGRESS_SHIFT)
#define SECY_HANDLE_RULEMATCH_SHIFT   (4U)
#define SECY_HANDLE_RULEMATCH_MASK    (0xFULL << SECY_HANDLE_RULEMATCH_SHIFT)
#define SECY_HANDLE_RULEMATCH_MASK8   (0xFU   << SECY_HANDLE_RULEMATCH_SHIFT)
#define SECY_HANDLE_RULE_TYPE_SHIFT   (2U)
#define SECY_HANDLE_RULE_TYPE_MASK    (0x3ULL << SECY_HANDLE_RULE_TYPE_SHIFT)
#define SECY_HANDLE_RULE_TYPE_MASK8   (0x3U   << SECY_HANDLE_RULE_TYPE_SHIFT)
#define SECY_HANDLE_DIR_SHIFT         (0U)
#define SECY_HANDLE_DIR_MASK          (0x3ULL << SECY_HANDLE_DIR_SHIFT)
#define SECY_HANDLE_DIR_MASK8         (0x3U   << SECY_HANDLE_DIR_SHIFT)
#define SECY_HANDLE_CRC16_POLY        (0x8005U)
#define SECY_HANDLE_TO_PORT(aHandle)  ((aHandle & SECY_HANDLE_HWID_MASK) >> SECY_HANDLE_HWID_SHIFT)
/** @} */

/**
    @name SECY_BusType
    @{
    @brief SECY Bus access Type
    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_BusType;
#define SECY_BUS_MODE_UNDEFINED     (0UL)
#define SECY_BUS_MODE_AXI           (1UL)
#define SECY_BUS_MODE_MDIO          (2UL)
/** @} */

/**
    @brief SCI type

    CRYPTO Secure channel identifier consist of 6-bytes of the
    source mac address and 16-bit of port identifier.

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_SCIType {
    uint8_t macAddr[6UL];
    uint16_t portNum;
} SECY_SCIType;

/**
    @brief CRYPTO key type

    MACSec key provided by client during the SA creation. If SecY entity
    is using extended packet number (#SECY_CfgType.xpnEnable is set to true),
    client shall fill 16-bytes of the salt value.
    keyId field is 12-bytes which is associated with specific key and  returned
    while querying the current status of the SA (#SECY_SAStatusType).

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_KeyType {
    uint8_t         sak[SECY_KEY_LEN];        /**< @brief CRYPTO key buffer */
    uint8_t         salt[SECY_XPN_SALT_LEN];  /**< @brief Salt for XPN key
                                                          If XPN cipher is not used
                                                          this parameter shall be set to 0 */
    uint8_t         keyId[SECY_KEYID_LEN];    /**< @brief key identified as defined by 802.1X */
} SECY_KeyType;

/**
    @brief CRYPTO rule option 0 type

    This structure defines the control packet rule.
    Based on the ruleMask (#SECY_RuleMatchType), respective fields
    of this structures are matched to classify a packet as control packet.
    When a packet is classified as control packet, it will bypass the encryption/decryption
    engine.

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_RuleOpt0Type {
    SECY_RuleMatchType    ruleMask;       /**< @brief rule match mask */
    uint8_t               macDAStart[6U]; /**< @brief MAC DA to start to match */
    uint8_t               macDAEnd[6U];   /**< @brief MAC DA end to match */
    SECY_RuleDirType      dir;            /**< @brief direction in which rule shall be applied */
    uint16_t              ethType;        /**< @brief Ethertype to match */
    uint8_t               rsvd[2U];
} SECY_RuleOpt0Type;

/**
    @brief CRYPTO rule option 1 type

    This structure defines the rule for the packet classification.
    Lookup rule consist of key, data and corresponding key mask and data masks.
    Mask fields can be used to mask out the bits which doesn't need to be compared in the
    key/data fields. For example, if matchMacDAMask is set to {0xFF, 0xFF, 0, 0, 0, 0}, then
    macDA[0] and macDA[1] fields are usedfor comparison.

    frameData field shall be in below format; consider the below frame:
    data = {
    0x84, 0xC5, 0xD5, 0x13, 0xD2, 0xAA,
    0xF6, 0xE5, 0xBB, 0xD2, 0x72, 0x77,
    0x08, 0x00, 0x0F, 0x10, 0x11, 0x12 ... }

    To match only first 32-bit of data:
    frameData[0] = data[12] | data[13] << 8 | data[14] << 16 | data[15] << 24
    matchFrameDataMask[0] = 0xFFFFFFFF

    To match next 24-bit of data also:
    frameData[1] = data[16] | data[17] << 8 | data[18) << 16
    matchFrameDataMask[1] = 0xFFFFFF

    If macSecTagged is set to TRUE, engine will check if the packet has SecTag (along with other
    rule parameters). Note that setting this to TRUE during the control frame rule addition
    will cause MACSec tagged frame to bypass the encryption/decryption (and it is not recommended!!).
    This flag can be set to TRUE during SecY lookup rule addition.

    If vlanTagged is set to TRUE, packets will be checked if they are vlan tagged (for this
    feature to work, Vlan tag parsing must be enabled in the hardware:
    #SECY_HwCfgType.enableQTagParse shall be set to TRUE). As hardware already parsed the VLAN
    etherType (0x8100) so we can skip passing this.
    Frame field shall be in below format:

    VLAN Tagged data = {
    0x84, 0xC5, 0xD5, 0x13, 0xD2, 0xAA,
    0xF6, 0xE5, 0xBB, 0xD2, 0x72, 0x77,
    0x81, 0x00, 0x0F, 0x10, 0x11, 0x12
    0x13, 0x14  ... }

    To match only first 32-bit of data:
    frameData[0] = data[14] | data[15] << 8 | data[16] << 16 | data[17] << 24
    matchFrameDataMask[0] = 0xFFFFFFFF

    To match next 24-bit of data also:
    frameData[1] = data[18] | data[19] << 8 | data[20) << 16
    matchFrameDataMask[1] = 0xFFFFFF

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_RuleOpt1Type {
    uint8_t                 macDA[6U];          /**< @brief MAC DA to match (Only bits which are set
                                                    in matchDAMask are matched) */
    uint8_t                 macSA[6U];          /**< @brief MAC SA to match (only bits which are set
                                                    in matchSAMask are matched) */
    uint32_t                frameData[2U];      /**< @brief frame data after SA to match (only 56
                                                    bits out of 64-bit are compared:
                                                    frameData[31:0], followed by frameData[23:0].
                                                    Only bits which are set in the
                                                    matchFrameDataMask are matched) */
    uint32_t                macSecTagged;       /**< @brief TRUE: check SecTAG detection */
    uint32_t                vlanTagged;         /**< @brief TRUE: detect 802.1Q tagged packet  */
    uint8_t                 matchMacDAMask[6U]; /**< @brief MAC DA mask bits */
    uint8_t                 matchMacSAMask[6U]; /**< @brief MAC SA mask bits */
    uint32_t                matchFrameDataMask[2U]; /**< @brief Frame data mask bits */
    SECY_RuleDirType  dir;                    /**< @brief Rule direction */
} SECY_RuleOpt1Type;

/**
    @brief SecY configuration
    This structure defines the SecY configuration.

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_CfgType {
    uint32_t        xpnEnable;          /**< @brief TRUE: Enable XPN support for this SecY */
    uint32_t        includeSCI;         /**< @brief TRUE: include explicit SCI in transmit frame
                                                    FALSE: don't encode SCI in the SecTAG in
                                                    transmit frames.
                                                    if this field is set to FALSE, it's assumed that
                                                    its point-to-point connection with peer SecY
                                                    (this SecY will can receive from at most one
                                                    peer SecY and only one receive secure channel
                                                    can be created for this SecY). */
    uint8_t         preSecTagAuthStart; /**< @brief Pre-SecTAG Authentication Start.
                                                    Specifies number of bytes from the start of
                                                    the frame to be bypassed without
                                                    CRYPTO protection. */
    uint8_t         preSecTagAuthLen; /**< @brief Pre-SecTAG Authentication Length.
                                                  Specifies number of bytes to be authenticated in
                                                  the pre-SecTAG area. For 802.1AE compliance,
                                                  this value must be set to 12 Bytes, representing
                                                  the length of the MAC SA/DA fields. */
    uint8_t         secTagOffset;     /**< @brief Offset of the SecTAG (location relative to start
                                                  of the frame). For normal operations, this shall be
                                                  set to 12. In case of VLAN in clear (802.1Q tag
                                                  in clear text, this field shall be programmed with
                                                  16 (6 bytes MAC DA + 6 bytes MAC SA + 4 bytes for
                                                  802.1Q tag)) */
    uint8_t         rsvd;
} SECY_CfgType;

/**
    @brief  SA status type

    SA status type

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_SAStatusType {
    uint32_t        inUse;              /**< @brief true: SA is use, false: SA not in use */
    uint32_t        ssci;               /**< @brief SSCI value for this SA */
    uint32_t        createdTime;        /**< @brief Time when this SA was created */
    uint32_t        startedTime;        /**< @brief Time when this SA became active */
    uint32_t        stoppedTime;        /**< @brief Time when this SA was stopped */
    uint8_t         keyID[16UL];        /**< @brief SA Key identifier */
} SECY_SAStatusType;

/**
    @brief SECY_TxSCStatsType

    Transmit Secure channel statistics type

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_TxSCStatsType {
    uint64_t outPktsProtected;      /**< @brief number of packets protected */
    uint64_t outPktsEncrypted;      /**< @brief number of packets encrypted */
} SECY_TxSCStatsType;

/**
    @brief SECY_RxSCStatsType

    Receive secure channel statistics

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_RxSCStatsType {
    uint64_t inPktsLate;           /**< @brief replayProtect is enabled and recv_frame(pn) <
                                               sa->nextPN */
    uint64_t inPktsNotValid;       /**< @brief recv frame is not valid and validateFrames is Strict
                                               or C-bit is set in SecTAG */
    uint64_t inPktsInvalid;        /**< @brief recv frame is not valid and validaFrame is Check */
    uint64_t inPktsDelayed;        /**< @brief recv_frame(pn) < sa->nextPn */
    uint64_t inPktsUnchecked;      /**< @brief recv frame is not valid and
                                               validateFrame == Disabled */
    uint64_t inPktsOK;             /**< @brief packet is valid and recv_frame(pn) > sa->nextPN */
} SECY_RxSCStatsType;

/**
    @brief SecY statistics

    Statistics counters for SecY entity

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_StatsType {
    uint64_t outPktsUntagged;      /**< @brief number of packet which are not protected
                                               (protectFrames == False) */
    uint64_t outPktsTooLong;       /**< @brief number of protected frames (protectFrame == True) whose
                                               length if more than maximum allowed frame length on
                                               interface */
    uint64_t inPktsUntagged;       /**< @brief received frame has no SecTAG and
                                               (validateFrame != Strict) */
    uint64_t inPktsNoTag;          /**< @brief received frame has no SecTAG and
                                               (validateFrame == Strict) */
    uint64_t inPktsBadTag;         /**< @brief received frame has invalid tag or invalid ICV */
    uint64_t inPktsOverrun;        /**< @brief received frame queued for validation but exceeds
                                               selected cipher suite capabilities */
    uint64_t inPktsNoSA;           /**< @brief no active SA is found for this frame and
                                               (validateFrame != Strict) */
    uint64_t inPktsNoSAError;      /**< @brief no active SA is found for this frame and
                                               (validateFrame == Strict) or C-bit == 1 in SecTAG */
    uint64_t outOctetsProtected;   /**< @brief number of octets of user data in tx frame which were
                                               integrity protected but not encrypted */
    uint64_t outOctetsEncrypted;   /**< @brief number of octets of user data in tx frame which were
                                               integrity protected and encrypted */
    uint64_t inOctetsValidated;     /**< @brief Number of octets of User Data recovered from
                                                received frames that were integrity protected but
                                                not encrypted */
    uint64_t inOctetsDecrypted;     /**< @brief Number of octets of User Data recovered from
                                                received frames that were both integrity protected
                                                and encrypted both */
} SECY_StatsType;

/**
    @brief SecY Get Port Status

    @trace #BRCM_SWREQ_SECY
*/
typedef uint32_t SECY_HwStatusType;
#define SECY_HW_STATUS_SECURED_SHIFT          (0x0UL)
#define SECY_HW_STATUS_SECURED_MASK           (0x1UL << SECY_HW_STATUS_SECURED_SHIFT)
#define SECY_HW_STATUS_CONFIDENTIALITY_SHIFT  (0x1UL)
#define SECY_HW_STATUS_CONFIDENTIALITY_MASK   (0x1UL << SECY_HW_STATUS_CONFIDENTIALITY_SHIFT)
#define SECY_HW_STATUS_OPERATIONAL_SHIFT      (0x2UL)
#define SECY_HW_STATUS_OPERATIONAL_MASK       (0x1UL << SECY_HW_STATUS_OPERATIONAL_SHIFT)

/**
    @brief SecY Get Port Status

    @trace #BRCM_SWREQ_SECY
*/
typedef struct sSECY_PortStatusType {
    SECY_CipherType    cipher;               /**< @brief Cipher Suit Info */
    SECY_HwStatusType  hwStatus;             /**< @brief Secy Hardware status
                                                         SECURED : Allowed only Tag Packet
                                                         CONFIDENTIALITY : Data Encription is Enabled
                                                         OPERATIONAL : Control Port is Enabled */
    uint32_t           rsvd[5];
} SECY_PortStatusType;

#endif /* CRYPTO_SECY_TYPES_H */

/** @} */

