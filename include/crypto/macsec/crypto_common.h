/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_crypto_common_ifc Common Types
    @ingroup grp_macsecdrv

    @addtogroup grp_crypto_common_ifc
    @{

    @file crypto_common.h
    @brief Crypto Common Types and Macros
    This header file contains common types and macros for cryptography

    @version 0.1 Initial Version
*/

#ifndef CRYPTO_COMMON_H
#define CRYPTO_COMMON_H

#include <stdint.h>
#include <bcm_utils.h>
#include <crypto_types.h>

/**
    @name Component Architecture IDs
    @{
    @brief Architecture IDs for Component
*/
#define BRCM_SWARCH_CRYPTO_KEY_MACRO                                (0x8100U)   /**< @brief #CRYPTO_KEY_FAMILY_TYPE_MASK        */
#define BRCM_SWARCH_CRYPTO_KEY_FAMILY_TYPE                          (0x8101U)   /**< @brief #CRYPTO_KeyFamilyType               */
#define BRCM_SWARCH_CRYPTO_ALGMODE_TYPE                             (0x8105U)   /**< @brief #CRYPTO_AlgModeType                 */
#define BRCM_SWARCH_CRYPTO_KEY_AUTH_STRING_MAX_SIZE_MACRO           (0x8107U)   /**< @brief #CRYPTO_KEY_AUTH_STRING_MAX_SIZE    */
#define BRCM_SWARCH_CRYPTO_SHA256_DIGEST_STORAGE_TYPE               (0x8108U)   /**< @brief #CRYPTO_SHA256DigestStorageType     */
#define BRCM_SWARCH_CRYPTO_SHA384_DIGEST_STORAGE_TYPE               (0x8109U)   /**< @brief #CRYPTO_SHA384DigestStorageType     */
#define BRCM_SWARCH_CRYPTO_SHA512_DIGEST_STORAGE_TYPE               (0x810AU)   /**< @brief #CRYPTO_SHA512DigestStorageType     */
#define BRCM_SWARCH_CRYPTO_HMAC_SHA256_TAG_STORAGE_TYPE             (0x810BU)   /**< @brief #CRYPTO_HMACSHA256TagStorageType    */
#define BRCM_SWARCH_CRYPTO_HMAC_SHA384_TAG_STORAGE_TYPE             (0x810CU)   /**< @brief #CRYPTO_HMACSHA384TagStorageType    */
#define BRCM_SWARCH_CRYPTO_HMAC_SHA512_TAG_STORAGE_TYPE             (0x810DU)   /**< @brief #CRYPTO_HMACSHA512TagStorageType    */
#define BRCM_SWARCH_CRYPTO_AES_CMAC_TAG_STORAGE_TYPE                (0x810EU)   /**< @brief #CRYPTO_AESCMACTagStorageType       */
#define BRCM_SWARCH_CRYPTO_RSA2K_SIGNATURE_STORAGE_TYPE             (0x810FU)   /**< @brief #CRYPTO_RSA2KSignatureStorageType   */
#define BRCM_SWARCH_CRYPTO_RSA3K_SIGNATURE_STORAGE_TYPE             (0x8110U)   /**< @brief #CRYPTO_RSA3KSignatureStorageType   */
#define BRCM_SWARCH_CRYPTO_ECDSA_SECP256R1_SIGNATURE_STORAGE_TYPE   (0x8111U)   /**< @brief #CRYPTO_ECDSASecp256R1SignatureStorageType  */
#define BRCM_SWARCH_CRYPTO_ECDSA_SECP384R1_SIGNATURE_STORAGE_TYPE   (0x8112U)   /**< @brief #CRYPTO_ECDSASecp384R1SignatureStorageType  */
#define BRCM_SWARCH_CRYPTO_SIGNATURE_STORAGE_TYPE                   (0x8113U)   /**< @brief #CRYPTO_SignatureStorageType        */
#define BRCM_SWARCH_CRYPTO_AES128_PRIVKEY_TYPE                      (0x8114U)   /**< @brief #CRYPTO_AES128PrivKeyType           */
#define BRCM_SWARCH_CRYPTO_AES192_PRIVKEY_TYPE                      (0x8115U)   /**< @brief #CRYPTO_AES192PrivKeyType           */
#define BRCM_SWARCH_CRYPTO_AES256_PRIVKEY_TYPE                      (0x8116U)   /**< @brief #CRYPTO_AES256PrivKeyType           */
#define BRCM_SWARCH_CRYPTO_SECRET96_PRIVKEY_TYPE                    (0x8117U)   /**< @brief #CRYPTO_Secret96BitPrivKeyType      */
#define BRCM_SWARCH_CRYPTO_SECRET128_PRIVKEY_TYPE                   (0x8118U)   /**< @brief #CRYPTO_Secret128BitPrivKeyType     */
#define BRCM_SWARCH_CRYPTO_SECRET192_PRIVKEY_TYPE                   (0x8119U)   /**< @brief #CRYPTO_Secret192BitPrivKeyType     */
#define BRCM_SWARCH_CRYPTO_SECRET256_PRIVKEY_TYPE                   (0x811AU)   /**< @brief #CRYPTO_Secret256BitPrivKeyType     */
#define BRCM_SWARCH_CRYPTO_SECRET384_PRIVKEY_TYPE                   (0x811BU)   /**< @brief #CRYPTO_Secret384BitPrivKeyType     */
#define BRCM_SWARCH_CRYPTO_SECRET512_PRIVKEY_TYPE                   (0x811CU)   /**< @brief #CRYPTO_Secret512BitPrivKeyType     */
#define BRCM_SWARCH_CRYPTO_RSA2K_PRIVKEY_TYPE                       (0x811DU)   /**< @brief #CRYPTO_RSA2KPrivKeyType            */
#define BRCM_SWARCH_CRYPTO_RSA2K_PUBKEY_TYPE                        (0x811EU)   /**< @brief #CRYPTO_RSA2KPubKeyType             */
#define BRCM_SWARCH_CRYPTO_RSA3K_PRIVKEY_TYPE                       (0x811FU)   /**< @brief #CRYPTO_RSA3KPrivKeyType            */
#define BRCM_SWARCH_CRYPTO_RSA3K_PUBKEY_TYPE                        (0x8120U)   /**< @brief #CRYPTO_RSA3KPubKeyType             */
#define BRCM_SWARCH_CRYPTO_ECC_SECP256R1_PRIVKEY_TYPE               (0x8121U)   /**< @brief #CRYPTO_ECCSECP256R1PrivKeyType     */
#define BRCM_SWARCH_CRYPTO_ECC_SECP256R1_PUBKEY_TYPE                (0x8122U)   /**< @brief #CRYPTO_ECCSECP256R1PubKeyType      */
#define BRCM_SWARCH_CRYPTO_ECC_SECP384R1_PRIVKEY_TYPE               (0x8123U)   /**< @brief #CRYPTO_ECCSECP384R1PrivKeyType     */
#define BRCM_SWARCH_CRYPTO_ECC_SECP384R1_PUBKEY_TYPE                (0x8124U)   /**< @brief #CRYPTO_ECCSECP384R1PubKeyType      */
#define BRCM_SWARCH_CRYPTO_PRIVKEY_STORAGE_TYPE                     (0x8125U)   /**< @brief #CRYPTO_PrivKeyStorageType          */
#define BRCM_SWARCH_CRYPTO_PUBKEY_STORAGE_TYPE                      (0x8126U)   /**< @brief #CRYPTO_PubKeyStorageType           */
#define BRCM_SWARCH_CRYPTO_PRIVKEY_CONTAINER_TYPE                   (0x8127U)   /**< @brief #CRYPTO_PrivKeyContainerType        */
#define BRCM_SWARCH_CRYPTO_PUBKEY_CONTAINER_TYPE                    (0x8128U)   /**< @brief #CRYPTO_PubKeyContainerType         */
#define BRCM_SWARCH_CRYPTO_KEY_CONTAINER_TYPE                       (0x8129U)   /**< @brief #CRYPTO_KeyContainerType            */
#define BRCM_SWARCH_CRYPTO_AES_GCM_TAG_STORAGE_TYPE                 (0x812BU)   /**< @brief #CRYPTO_AESGCMTagStorageType        */
#define BRCM_SWARCH_CRYPTO_SIGNATURE_SHORT_STORAGE_TYPE             (0x812AU)   /**< @brief #CRYPTO_SignatureShortStorageType   */
#define BRCM_SWARCH_CRYPTO_PRIVKEY_SHORT_STORAGE_TYPE               (0x812CU)   /**< @brief #CRYPTO_PrivKeyShortStorageType     */
#define BRCM_SWARCH_CRYPTO_PUBKEY_SHORT_STORAGE_TYPE                (0x812DU)   /**< @brief #CRYPTO_PubKeyShortStorageType      */
#define BRCM_SWARCH_CRYPTO_PRIVKEY_SHORT_CONTAINER_TYPE             (0x812FU)   /**< @brief #CRYPTO_PrivKeyShortContainerType   */
#define BRCM_SWARCH_CRYPTO_PUBKEY_SHORT_CONTAINER_TYPE              (0x8130U)   /**< @brief #CRYPTO_PubKeyShortContainerType    */
#define BRCM_SWARCH_CRYPTO_PLAIN_DATA_SHORT_CONTAINER_TYPE          (0x8131U)   /**< @brief #CRYPTO_PlainDataShortContainerType */
#define BRCM_SWARCH_CRYPTO_KEY_SHORT_CONTAINER_TYPE                 (0x8132U)   /**< @brief #CRYPTO_KeyShortContainerType       */
/** @} */

/**
    @brief Cryptography common macros

    @trace #BRCM_SWREQ_CRYPTO
*/
#define CRYPTO_KEY_FAMILY_TYPE_MASK     (0xFFFFFE00UL)  /**< @brief Family type Mask                */
#define CRYPTO_KEY_BW_ENCODE_FMT_MASK   (0x00000100UL)  /**< @brief Bit width encoding format mask  */
#define CRYPTO_KEY_BW_MASK              (0x000000FFUL)  /**< @brief Bit width Mask                  */
#define CRYPTO_KEY_HMAC_SHA256          (0x484D0108UL)  /**< @brief HMAC SHA256 key, 256 bits       */
#define CRYPTO_KEY_HMAC_SHA384          (0x484D010CUL)  /**< @brief HMAC SHA384 key, 384 bits       */
#define CRYPTO_KEY_HMAC_SHA512          (0x484D0110UL)  /**< @brief HMAC SHA512 key, 512 bits       */

/**
    @brief Crypto key family type

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_KeyFamilyType;
#define CRYPTO_KEY_FAMILY_AES           (0x41450000UL)  /**< @brief AES key family                  */
#define CRYPTO_KEY_FAMILY_ECC           (0x45431000UL)  /**< @brief ECC SECP* key family            */
#define CRYPTO_KEY_FAMILY_RSA           (0x52531000UL)  /**< @brief RSA key family                  */
#define CRYPTO_KEY_FAMILY_HMAC          (0x484D0000UL)  /**< @brief HMAC key family                 */
#define CRYPTO_KEY_FAMILY_SECRET        (0x53450000UL)  /**< @brief Secret bits                     */
#define CRYPTO_KEY_FAMILY_PLAIN_DATA    (0x55440000UL)  /**< @brief Plain data. The lower 4 nibbles
                                                                may be used to encode the data length
                                                                in bytes                            */

/**
    @{
    @brief Operating mode of a crypt algorithm

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_AlgModeType;
#define CRYPTO_ALGMODE_NONE             (0x00000000UL)  /**< @brief Operating mode none                             */
#define CRYPTO_ALGMODE_RSA_PKCS1_V15    (0x504A2100UL)  /**< @brief RFC 8017, PKCS1-v1_5 encoding                   */
#define CRYPTO_ALGMODE_RSA_PSS          (0x504B2100UL)  /**< @brief RFC 8017, EMSA-PSS encoding                     */
#define CRYPTO_ALGMODE_AES_CMAC_TAG     (0x41434D54UL)  /**< @brief NIST SP 800-38B, AES-CMAC tag generation        */
#define CRYPTO_ALGMODE_AES_CMAC_AUTH    (0x41434D41UL)  /**< @brief NIST SP 800-38B, AES-CMAC tag authentication    */
#define CRYPTO_ALGMODE_CMAC_KDF_CTR     (0x434D5452UL)  /**< @brief NIST 800-108 CTR mode, AES-CMAC PRF             */
#define CRYPTO_ALGMODE_HKDF_EXTRACT     (0x484B4558UL)  /**< @brief RFC 5869, HKDF extract algorithm                */
#define CRYPTO_ALGMODE_HKDF_EXPAND      (0x484B4550UL)  /**< @brief RFC 5869, HKDF expand algorithm                 */
#define CRYPTO_ALGMODE_TXRX_AES_KEYWRAP (0x41454B57UL)  /**< @brief RFC 3394, AES keywrap algorithm                 */
#define CRYPTO_ALGMODE_TXRX_FLASH       (0x494D4757UL)  /**< @brief Import/export keys to and from BRCM flash       */
#define CRYPTO_ALGMODE_CBC_ENC          (0x43424345UL)  /**< @brief NIST SP800-38A, CBC encryption mode             */
#define CRYPTO_ALGMODE_CBC_DEC          (0x43424344UL)  /**< @brief NIST SP800-38A, CBC decryption mode             */
#define CRYPTO_ALGMODE_CTR_ENC          (0x43545245UL)  /**< @brief NIST SP800-38A, CTR encryption mode             */
#define CRYPTO_ALGMODE_CTR_DEC          (0x43545244UL)  /**< @brief NIST SP800-38A, CTR decryption mode             */
#define CRYPTO_ALGMODE_HMAC_TAG         (0x484D4354UL)  /**< @brief FIPS 198-1, HMAC Tag generation                 */
#define CRYPTO_ALGMODE_HMAC_AUTH        (0x484D4341UL)  /**< @brief FIPS 198-1, HMAC Tag authentication             */
#define CRYPTO_ALGMODE_GCM_ENC_AND_TAG  (0x47434D45UL)  /**< @brief NIST SP800-38D, GCM encrypt and tag             */
#define CRYPTO_ALGMODE_GCM_DEC_AND_AUTH (0x47434D41UL)  /**< @brief NIST SP800-38D, GCM decrypt and authenticate    */
#define CRYPTO_ALGMODE_CCM_ENC_AND_TAG  (0x43434D45UL)  /**< @brief NIST SP800-38C, CCM encrypt and tag             */
#define CRYPTO_ALGMODE_CCM_DEC_AND_AUTH (0x43434D41UL)  /**< @brief NIST SP800-38C, CCM decrypt and authenticate    */
/** @} */

/**
    @brief Maximum length of the key authorisation binary string

    @trace #BRCM_SWREQ_CRYPTO
*/
#define CRYPTO_KEY_AUTH_STRING_MAX_SIZE (32UL)  /**< @brief Crypto Key Authorization max size */

/**
    @brief SHA256 digest

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_SHA256DigestStorageType {
    uint32_t digest[32UL / 4UL];
} CRYPTO_SHA256DigestStorageType;

/**
    @brief SHA384 digest

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_SHA384DigestStorageType {
    uint32_t digest[48UL / 4UL];
} CRYPTO_SHA384DigestStorageType;

/**
    @brief SHA512 digest

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_SHA512DigestStorageType {
    uint32_t digest[64UL / 4UL];
} CRYPTO_SHA512DigestStorageType;

/**
    @brief HMAC-SHA256 authentication tag

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_HMACSHA256TagStorageType {
    uint32_t tag[32UL / 4UL];
} CRYPTO_HMACSHA256TagStorageType;

/**
    @brief HMAC-SHA384 authentication tag

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_HMACSHA384TagStorageType {
    uint32_t tag[48UL / 4UL];
} CRYPTO_HMACSHA384TagStorageType;

/**
    @brief HMAC-SHA512 authentication tag

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_HMACSHA512TagStorageType {
    uint32_t tag[64UL / 4UL];
} CRYPTO_HMACSHA512TagStorageType;

/**
    @brief AES CMAC tag storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_AESCMACTagStorageType {
    uint32_t tag[16UL / 4UL];               /**< @brief AES CMAC tag storage                */
} CRYPTO_AESCMACTagStorageType;

/**
    @brief AES GCM tag storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_AESGCMTagStorageType {
    uint32_t tag[16UL / 4UL];               /**< @brief AES GCM tag storage                */
} CRYPTO_AESGCMTagStorageType;

/**
    @brief RSA2K signature storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA2KSignatureStorageType {
    uint32_t signature[256UL / 4UL];        /**< @brief RSA2K signature storage             */
} CRYPTO_RSA2KSignatureStorageType;

/**
    @brief RSA3K signature storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA3KSignatureStorageType {
    uint32_t signature[384UL / 4UL];        /**< @brief RSA3K signature storage             */
} CRYPTO_RSA3KSignatureStorageType;

/**
    @brief ECDSA SECP256R1 signature storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECDSASecp256R1SignatureStorageType {
    uint32_t R[32UL / 4UL];                 /**< @brief ECDSA SECP256R1 signature storage R */
    uint32_t S[32UL / 4UL];                 /**< @brief ECDSA SECP256R1 signature storage S */
} CRYPTO_ECDSASecp256R1SignatureStorageType;

/**
    @brief ECDSA SECP384R1 signature storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECDSASecp384R1SignatureStorageType {
    uint32_t R[48UL / 4UL];                 /**< @brief ECDSA SECP384R1 signature storage R */
    uint32_t S[48UL / 4UL];                 /**< @brief ECDSA SECP384R1 signature storage S */
} CRYPTO_ECDSASecp384R1SignatureStorageType;

/**
    @brief Signature storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_SignatureStorageType {
    uint8_t                                     raw[384UL];     /**< @brief Raw bits                            */
    CRYPTO_SHA256DigestStorageType              sha256;         /**< @brief SHA256 digest storage               */
    CRYPTO_SHA384DigestStorageType              sha384;         /**< @brief SHA384 digest storage               */
    CRYPTO_SHA512DigestStorageType              sha512;         /**< @brief SHA512 digest storage               */
    CRYPTO_HMACSHA256TagStorageType             hmacSha256;     /**< @brief HMAC-SHA256 tag storage             */
    CRYPTO_HMACSHA384TagStorageType             hmacSha384;     /**< @brief HMAC-SHA384 tag storage             */
    CRYPTO_HMACSHA512TagStorageType             hmacSha512;     /**< @brief HMAC-SHA512 tag storage             */
    CRYPTO_AESCMACTagStorageType                aesCmac;        /**< @brief AES CMAC tag storage                */
    CRYPTO_AESGCMTagStorageType                 aesGcm;         /**< @brief AES GCM tag storage                 */
    CRYPTO_ECDSASecp256R1SignatureStorageType   ecdsaSecp256R1; /**< @brief ECDSA SECP256R1 signature storage   */
    CRYPTO_ECDSASecp384R1SignatureStorageType   ecdsaSecp384R1; /**< @brief ECDSA SECP384R1 signature storage   */
    CRYPTO_RSA2KSignatureStorageType            rsa2K;          /**< @brief RSA2K signature storage             */
    CRYPTO_RSA3KSignatureStorageType            rsa3K;          /**< @brief RSA3K signature storage             */
} CRYPTO_SignatureStorageType;

/**
    @brief Signature storage for use-cases with short signatures

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_SignatureShortStorageType {
    uint8_t                                     raw[96UL];      /**< @brief Raw bits                            */
    CRYPTO_SHA256DigestStorageType              sha256;         /**< @brief SHA256 digest storage               */
    CRYPTO_SHA384DigestStorageType              sha384;         /**< @brief SHA384 digest storage               */
    CRYPTO_SHA512DigestStorageType              sha512;         /**< @brief SHA512 digest storage               */
    CRYPTO_HMACSHA256TagStorageType             hmacSha256;     /**< @brief HMAC-SHA256 tag storage             */
    CRYPTO_HMACSHA384TagStorageType             hmacSha384;     /**< @brief HMAC-SHA384 tag storage             */
    CRYPTO_HMACSHA512TagStorageType             hmacSha512;     /**< @brief HMAC-SHA512 tag storage             */
    CRYPTO_AESCMACTagStorageType                aesCmac;        /**< @brief AES CMAC tag storage                */
    CRYPTO_AESGCMTagStorageType                 aesGcm;         /**< @brief AES GCM tag storage                 */
    CRYPTO_ECDSASecp256R1SignatureStorageType   ecdsaSecp256R1; /**< @brief ECDSA SECP256R1 signature storage   */
    CRYPTO_ECDSASecp384R1SignatureStorageType   ecdsaSecp384R1; /**< @brief ECDSA SECP384R1 signature storage   */
} CRYPTO_SignatureShortStorageType;

/**
    @brief AES 128 bit private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_AES128PrivKeyType {
    uint32_t keyBits[16UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_AES128PrivKeyType;

/**
    @brief AES 192 bit private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_AES192PrivKeyType {
    uint32_t keyBits[24UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_AES192PrivKeyType;

/**
    @brief AES 256 bit private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_AES256PrivKeyType {
    uint32_t keyBits[32UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_AES256PrivKeyType;

/**
    @brief 96 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret96BitPrivKeyType {
    uint32_t keyBits[12UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret96BitPrivKeyType;

/**
    @brief 128 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret128BitPrivKeyType {
    uint32_t keyBits[16UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret128BitPrivKeyType;

/**
    @brief 192 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret192BitPrivKeyType {
    uint32_t keyBits[24UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret192BitPrivKeyType;

/**
    @brief 256 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret256BitPrivKeyType {
    uint32_t keyBits[32UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret256BitPrivKeyType;

/**
    @brief 384 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret384BitPrivKeyType {
    uint32_t keyBits[48UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret384BitPrivKeyType;

/**
    @brief 512 bit secret

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_Secret512BitPrivKeyType {
    uint32_t keyBits[64UL / 4UL];           /**< @brief Key bits       */
} CRYPTO_Secret512BitPrivKeyType;

/**
    @brief RSA 2048 bit private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA2KPrivKeyType {
    uint32_t d[256UL / 4UL];                /**< @brief Key bits       */
} CRYPTO_RSA2KPrivKeyType;

/**
    @brief RSA 2048 bit public key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA2KPubKeyType {
    uint32_t n[256UL / 4UL];                /**< @brief RSA modulus    */
} CRYPTO_RSA2KPubKeyType;

/**
    @brief RSA 3072 bit private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA3KPrivKeyType {
    uint32_t d[384UL / 4UL];                /**< @brief Key bits       */
} CRYPTO_RSA3KPrivKeyType;

/**
    @brief RSA 3072 bit public key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_RSA3KPubKeyType {
    uint32_t n[384UL / 4UL];                /**< @brief RSA modulus    */
} CRYPTO_RSA3KPubKeyType;

/**
    @brief ECC SECP256R1 private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECCSECP256R1PrivKeyType {
    uint32_t d[32UL / 4UL];                 /**< @brief SECP256R1 private key   */
} CRYPTO_ECCSECP256R1PrivKeyType;

/**
    @brief ECC SECP256R1 public key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECCSECP256R1PubKeyType {
    uint32_t qx[32UL / 4UL];                /**< @brief SECP256R1 Pubkey X      */
    uint32_t qy[32UL / 4UL];                /**< @brief SECP256R1 Pubkey Y      */
} CRYPTO_ECCSECP256R1PubKeyType;

/**
    @brief ECC SECP384R1 private key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECCSECP384R1PrivKeyType {
    uint32_t d[48UL / 4UL];                 /**< @brief SECP384R1 private key   */
} CRYPTO_ECCSECP384R1PrivKeyType;

/**
    @brief ECC SECP384R1 public key

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_ECCSECP384R1PubKeyType {
    uint32_t qx[48UL / 4UL];                /**< @brief SECP384R1 Pubkey X      */
    uint32_t qy[48UL / 4UL];                /**< @brief SECP384R1 Pubkey Y      */
} CRYPTO_ECCSECP384R1PubKeyType;

/**
    @brief Private key storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_PrivKeyStorageType {
    uint8_t                         raw[384UL];     /**< @brief Raw bits                            */
    CRYPTO_AES128PrivKeyType        aes128;         /**< @brief AES128 key storage                  */
    CRYPTO_AES128PrivKeyType        aes192;         /**< @brief AES192 key storage                  */
    CRYPTO_AES256PrivKeyType        aes256;         /**< @brief AES256 key storage                  */
    CRYPTO_Secret96BitPrivKeyType   secret96Bit;    /**< @brief  96 bit secret                      */
    CRYPTO_Secret128BitPrivKeyType  secret128Bit;   /**< @brief 128 bit secret                      */
    CRYPTO_Secret192BitPrivKeyType  secret192Bit;   /**< @brief 192 bit secret                      */
    CRYPTO_Secret256BitPrivKeyType  secret256Bit;   /**< @brief 256 bit secret                      */
    CRYPTO_Secret384BitPrivKeyType  secret384Bit;   /**< @brief 384 bit secret                      */
    CRYPTO_Secret512BitPrivKeyType  secret512Bit;   /**< @brief 512 bit secret                      */
    CRYPTO_ECCSECP256R1PrivKeyType  eccSecp256R1;   /**< @brief ECC SECP256R1 private key storage   */
    CRYPTO_ECCSECP384R1PrivKeyType  eccSecp384R1;   /**< @brief ECC SECP384R1 private key storage   */
    CRYPTO_RSA2KPrivKeyType         rsa2K;          /**< @brief RSA2K private key storage           */
    CRYPTO_RSA3KPrivKeyType         rsa3K;          /**< @brief RSA3K private key storage           */
} CRYPTO_PrivKeyStorageType;

/**
    @brief Private key storage for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_PrivKeyShortStorageType {
    uint8_t                         raw[64UL];      /**< @brief Raw bits                            */
    CRYPTO_AES128PrivKeyType        aes128;         /**< @brief AES128 key storage                  */
    CRYPTO_AES128PrivKeyType        aes192;         /**< @brief AES192 key storage                  */
    CRYPTO_AES256PrivKeyType        aes256;         /**< @brief AES256 key storage                  */
    CRYPTO_Secret96BitPrivKeyType   secret96Bit;    /**< @brief  96 bit secret                      */
    CRYPTO_Secret128BitPrivKeyType  secret128Bit;   /**< @brief 128 bit secret                      */
    CRYPTO_Secret192BitPrivKeyType  secret192Bit;   /**< @brief 192 bit secret                      */
    CRYPTO_Secret256BitPrivKeyType  secret256Bit;   /**< @brief 256 bit secret                      */
    CRYPTO_Secret384BitPrivKeyType  secret384Bit;   /**< @brief 384 bit secret                      */
    CRYPTO_Secret512BitPrivKeyType  secret512Bit;   /**< @brief 512 bit secret                      */
    CRYPTO_ECCSECP256R1PrivKeyType  eccSecp256R1;   /**< @brief ECC SECP256R1 private key storage   */
    CRYPTO_ECCSECP384R1PrivKeyType  eccSecp384R1;   /**< @brief ECC SECP384R1 private key storage   */
} CRYPTO_PrivKeyShortStorageType;

/**
    @brief Public key storage data structure

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_PubKeyStorageType {
    uint8_t                         raw[384UL];     /**< @brief Raw bits                            */
    CRYPTO_ECCSECP256R1PubKeyType   eccSecp256R1;   /**< @brief ECC SECP256R1 public key storage    */
    CRYPTO_ECCSECP384R1PubKeyType   eccSecp384R1;   /**< @brief ECC SECP384R1 public key storage    */
    CRYPTO_RSA2KPubKeyType          rsa2K;          /**< @brief RSA2K public key storage            */
    CRYPTO_RSA3KPubKeyType          rsa3K;          /**< @brief RSA3K public key storage            */
} CRYPTO_PubKeyStorageType;

/**
    @brief Public key storage for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef union uCRYPTO_PubKeyShortStorageType {
    uint8_t                         raw[96UL];      /**< @brief Raw bits                            */
    CRYPTO_ECCSECP256R1PubKeyType   eccSecp256R1;   /**< @brief ECC SECP256R1 public key storage    */
    CRYPTO_ECCSECP384R1PubKeyType   eccSecp384R1;   /**< @brief ECC SECP384R1 public key storage    */
} CRYPTO_PubKeyShortStorageType;

/**
    @brief Private key container

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_PrivKeyContainerType {
    CRYPTO_KeyType                  keyType;        /**< @brief Private key type                    */
    CRYPTO_PrivKeyStorageType       keyStorage;     /**< @brief Private key storage                 */
    uint32_t                        reserved;       /**< @brief Reserved                            */
} CRYPTO_PrivKeyContainerType;

/**
    @brief Public key container

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_PubKeyContainerType {
    CRYPTO_KeyType              keyType;    /**< @brief Public key type                     */
    CRYPTO_PubKeyStorageType    keyStorage; /**< @brief Public key storage                  */
    uint32_t                    reserved;   /**< @brief Reserved                            */
} CRYPTO_PubKeyContainerType;

/**
    @brief Crypto key container

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_KeyContainerType {
    CRYPTO_PubKeyContainerType  pubKey;     /**< @brief Public key container                */
    CRYPTO_PrivKeyContainerType privKey;    /**< @brief Private key container               */
} CRYPTO_KeyContainerType;

/**
    @brief Private key container for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_PrivKeyShortContainerType {
    CRYPTO_KeyType                  keyType;        /**< @brief Private key type                    */
    CRYPTO_PrivKeyShortStorageType  keyStorage;     /**< @brief Private key storage                 */
    uint32_t                        reserved;       /**< @brief Reserved                            */
} CRYPTO_PrivKeyShortContainerType;

/**
    @brief Public key container for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_PubKeyShortContainerType {
    CRYPTO_KeyType                keyType;    /**< @brief Public key type                     */
    CRYPTO_PubKeyShortStorageType keyStorage; /**< @brief Public key storage                  */
    uint32_t                      reserved;   /**< @brief Reserved                            */
} CRYPTO_PubKeyShortContainerType;

/**
    @brief Plain data container for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_PlainDataShortContainerType {
    CRYPTO_KeyType                pdType;     /**< @brief Plain data magic and length         */
    uint8_t                       pd[100UL];  /**< @brief Plain data                          */
} CRYPTO_PlainDataShortContainerType;

/**
    @brief Crypto key container for use-cases with short keys

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef struct sCRYPTO_KeyShortContainerType {
    CRYPTO_PubKeyShortContainerType  pubKey;     /**< @brief Public key container                */
    CRYPTO_PrivKeyShortContainerType privKey;    /**< @brief Private key container               */
} CRYPTO_KeyShortContainerType;

#endif /* CRYPTO_COMMON_H */

/** @} */
