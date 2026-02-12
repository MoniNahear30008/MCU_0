/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_crypto_types Common Types
    @ingroup grp_crypto_abstract

    @addtogroup grp_crypto_types
    @{

    @file crypto_types.h
    @brief Cryptography common types

    @version 0.1.0 Initial version
*/

#ifndef CRYPTO_TYPES_H
#define CRYPTO_TYPES_H

#include <stdint.h>
#include <bcm_utils.h>
#include <bcm_comp.h>

/**
    @name Cryptography common types architecture IDs
    @{
    @brief Architecture IDs for Cryptography common types

*/
#define BRCM_SWARCH_CRYPTO_MACRO                (0x8000U)   /**< @brief #CRYPTO_MAGIC               */
#define BRCM_SWARCH_CRYPTO_KEY_TYPE             (0x8001U)   /**< @brief #CRYPTO_KeyType             */
#define BRCM_SWARCH_CRYPTO_PAD_SCHEME_TYPE      (0x8002U)   /**< @brief #CRYPTO_PadSchemeType       */
#define BRCM_SWARCH_CRYPTO_ALGO_MODE_TYPE       (0x8003U)   /**< @brief #CRYPTO_AlgoModeType        */
#define BRCM_SWARCH_CRYPTO_KDF_ALGO_TYPE        (0x8004U)   /**< @brief #CRYPTO_KdfAlgoType         */
#define BRCM_SWARCH_CRYPTO_KEY_USAGE_TYPE       (0x8005U)   /**< @brief #CRYPTO_KeyUsageType        */
#define BRCM_SWARCH_CRYPTO_KEY_CLASS_ID_TYPE    (0x8006U)   /**< @brief #CRYPTO_KeyClassIdType      */
#define BRCM_SWARCH_CRYPTO_KEY_BLOB_HDR_TYPE    (0x8007U)   /**< @brief #CRYPTO_KeyBlobHeaderType   */
#define BRCM_SWARCH_CRYPTO_MEM_HDL_TYPE         (0x8008U)   /**< @brief #CRYPTO_MemHdlType          */
#define BRCM_SWARCH_CRYPTO_MEM_IV_TYPE          (0x8009U)   /**< @brief #CRYPTO_MemIvType           */
#define BRCM_SWARCH_CRYPTO_KEY_INFO_TYPE        (0x800AU)   /**< @brief #CRYPTO_KeyInfoType         */
#define BRCM_SWARCH_CRYPTO_KEY_BLOB_256_TYPE    (0x800BU)   /**< @brief #CRYPTO_KeyBlob256Type      */
#define BRCM_SWARCH_CRYPTO_HASH_FUNC_TYPE       (0x800CU)   /**< @brief #CRYPTO_HashFuncType        */
#define BRCM_SWARCH_CRYPTO_KEY_BLOB_512_TYPE    (0x800DU)   /**< @brief #CRYPTO_KeyBlob512Type      */
#define BRCM_SWARCH_CRYPTO_KEY_BLOB_1024_TYPE   (0x800EU)   /**< @brief #CRYPTO_KeyBlob1024Type     */
#define BRCM_SWARCH_CRYPTO_KEY_BLOB_2048_TYPE   (0x800FU)   /**< @brief #CRYPTO_KeyBlob2048Type     */

/** @} */

/**
    @brief Crypto related macros

    @trace #BRCM_SWREQ_CRYPTO
*/
#define CRYPTO_MAGIC                    (0x43525054UL)
#define CRYPTO_COMMON_MAX_PAYLOAD_SIZE  (392UL)
#define CRYPTO_KEY_BLOB_256_DATA_SIZE   (256UL - 32UL)
#define CRYPTO_KEY_BLOB_512_DATA_SIZE   (512UL - 32UL)
#define CRYPTO_KEY_BLOB_1024_DATA_SIZE  (1024UL - 32UL)
#define CRYPTO_KEY_BLOB_2048_DATA_SIZE  (2048UL - 32UL)
#define CRYPTO_MAX_USER_DATA_SIZE       (256UL)
#define CRYPTO_KBH8_MAGIC               (0x4B424838UL)
#define CRYPTO_KBH9_MAGIC               (0x4B424839UL)
#define CRYPTO_KBHA_MAGIC               (0x4B424841UL)
#define CRYPTO_KBHB_MAGIC               (0x4B424842UL)

/**
    @name Crypto Key type macros
    @{
    @brief Crypto key type macros

    These macros are encoded as follows:

    | Bits (31:9)   | Bit-8                     | Bits (7:0)           |
    |:-------------:|:-------------------------:|:--------------------:|
    | Key fam type  | Bit-width encoding mode   | Bit-width encoding   |

    When bit-8 is set, the the bits in (7:0) represent the bit width of the key
    in the multiples of 32. Otherwise, they represent the bit width in the
    multiples of 128.

    This type maps to algorithm_identifier in EVITA 3.2. Key derivation algorithms
    are outside this type for better segregation for static configuration selection.
    For key derivation algorithms, refer to #CRYPTO_KdfAlgoType

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_KeyType;
#define CRYPTO_KEY_INVALID              (0x00000000UL)  /**< @brief Invalid key type                */
#define CRYPTO_KEY_RSA_2K               (0x52531010UL)  /**< @brief RSA2K bit key (RSA modulus)     */
#define CRYPTO_KEY_RSA_3K               (0x52531018UL)  /**< @brief RSA3K bit key (RSA modulus)     */
#define CRYPTO_KEY_ECC_SECP256R1        (0x45431002UL)  /**< @brief ECC SECP256R1 Key               */
#define CRYPTO_KEY_ECC_SECP384R1        (0x45431003UL)  /**< @brief ECC SECP384R1 Key               */
#define CRYPTO_KEY_AES128               (0x41450104UL)  /**< @brief AES 128 bit key                 */
#define CRYPTO_KEY_AES192               (0x41450106UL)  /**< @brief AES 192 bit key                 */
#define CRYPTO_KEY_AES256               (0x41450108UL)  /**< @brief AES 256 bit key                 */
#define CRYPTO_KEY_SECRET_96            (0x53450103UL)  /**< @brief  96 bit secret                  */
#define CRYPTO_KEY_SECRET_128           (0x53450104UL)  /**< @brief 128 but secret                  */
#define CRYPTO_KEY_SECRET_192           (0x53450106UL)  /**< @brief 192 bit secret                  */
#define CRYPTO_KEY_SECRET_256           (0x53450108UL)  /**< @brief 256 bit secret                  */
#define CRYPTO_KEY_SECRET_384           (0x5345010CUL)  /**< @brief 384 bit secret                  */
#define CRYPTO_KEY_SECRET_512           (0x53450110UL)  /**< @brief 512 bit secret                  */
/** @} */

/**
    @name Padding algorithms
    @{
    @brief Padding algorithms

    This type maps to padding_scheme in EVITA 3.2

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_PadSchemeType;
#define CRYPTO_PAD_SCHEME_NONE          (0x00000000UL)  /**< @brief Operating mode none             */
#define CRYPTO_PAD_SCHEME_PKCS1_V15     (0x504A2100UL)  /**< @brief RFC 8017, PKCS1-v1_5 encoding   */
#define CRYPTO_PAD_SCHEME_PSS           (0x504B2100UL)  /**< @brief RFC 8017, EMSA-PSS encoding     */
/** @} */

/**
    @name Symmetric algorithms
    @{
    @brief Symmetric key based algorithms

    This type maps to operation_mode in EVITA 3.2.

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_AlgoModeType;
#define CRYPTO_ALGO_MODE_NONE          (0x00000000UL)  /**< @brief No Algorithm                    */
#define CRYPTO_ALGO_MODE_HMAC          (0x484D4310UL)  /**< @brief FIPS 198-1, HMAC SHA256  "HMC"  */
#define CRYPTO_ALGO_MODE_CMAC          (0x434D4300UL)  /**< @brief NIST SP800-38B, AES-CMAC "CMC" */
#define CRYPTO_ALGO_MODE_CBC           (0x43424300UL)  /**< @brief NIST SP800-38A, AES-CBC  "CBC" */
#define CRYPTO_ALGO_MODE_CTR           (0x43545200UL)  /**< @brief NIST SP800-38A, AES-CTR  "CTR" */
#define CRYPTO_ALGO_MODE_CCM           (0x43434D00UL)  /**< @brief NIST SP800-38C, AES-CCM  "CCM" */
#define CRYPTO_ALGO_MODE_GCM           (0x47434D00UL)  /**< @brief NIST SP800-38D, AES-GCM  "GCM" */
/** @} */

/**
    @brief Hash functions

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_HashFuncType;
#define CRYPTO_HASH_FUNC_NONE           (0x00000000UL)  /**< @brief Hash function none      */
#define CRYPTO_HASH_FUNC_SHA256         (0x53482002UL)  /**< @brief SHA2-256                */
#define CRYPTO_HASH_FUNC_SHA384         (0x53482003UL)  /**< @brief SHA2-384                */
#define CRYPTO_HASH_FUNC_SHA512         (0x53482004UL)  /**< @brief SHA2-512                */
/** @} */

/**
    @name Derivation algorithms
    @{
    @brief Key and IV derivation algorithms

    This type maps to algorithm_identifier in EVITA 3.2, limited to Key derivation algorithms.
    For others, refer to #CRYPTO_KeyType

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_KdfAlgoType;
#define CRYPTO_KDF_ALGO_NONE            (0x00000000UL)  /**< brief No Algorithm (use as is) */
#define CRYPTO_KDF_ALGO_HKDF_SHA256     (0x484B5300UL)  /**< brief HKDF with SHA256   "HKS" */
/** @} */

/**
    @name Crypto memory usage macros
    @{
    @brief Crypto memory usage macros

    Represented by 32-bit mask

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_KeyUsageType;
#define CRYPTO_KEY_USAGE_DERIV_MASK         (0x1UL)        /**< @brief Usable for key-derivation               */
#define CRYPTO_KEY_USAGE_DERIV_SHIFT        (0UL)          /**< @brief Usable for key-derivation               */
#define CRYPTO_KEY_USAGE_SIGN_MASK          (0x2UL)        /**< @brief Usable for cryptographic signing        */
#define CRYPTO_KEY_USAGE_SIGN_SHIFT         (1UL)          /**< @brief Usable for key-derivation               */
#define CRYPTO_KEY_USAGE_VERIFY_MASK        (0x4UL)        /**< @brief Usable for signature verification       */
#define CRYPTO_KEY_USAGE_VERIFY_SHIFT       (2UL)          /**< @brief Usable for signature verification       */
#define CRYPTO_KEY_USAGE_DATA_ENCRYPT_MASK  (0x8UL)        /**< @brief Usable for bulk encryption or decryption*/
#define CRYPTO_KEY_USAGE_DATA_ENCRYPT_SHIFT (3UL)          /**< @brief Usable for bulk encryption or decryption*/
#define CRYPTO_KEY_USAGE_KEY_ENCRYPT_MASK   (0x20UL)       /**< @brief Usable for key encryption or decryption */
#define CRYPTO_KEY_USAGE_KEY_ENCRYPT_SHIFT  (5UL)          /**< @brief Usable for key encryption or decryption */
#define CRYPTO_KEY_USAGE_EXPORT_MASK        (0x80UL)       /**< @brief Usable for exporting                    */
#define CRYPTO_KEY_USAGE_EXPORT_SHIFT       (7UL)          /**< @brief Usable for exporting                    */
#define CRYPTO_KEY_USAGE_EXPORT_RAW_MASK    (0x100UL)      /**< @brief Usable for raw exporting                */
#define CRYPTO_KEY_USAGE_EXPORT_RAW_SHIFT   (8UL)          /**< @brief Usable for raw exporting                */
#define CRYPTO_KEY_USAGE_PERSISTENT_MASK    (0x200UL)      /**< @brief Non volatile key                        */
#define CRYPTO_KEY_USAGE_PERSISTENT_SHIFT   (9UL)          /**< @brief Non volatile key                        */
#define CRYPTO_KEY_USAGE_IV_MASK            (0x80000000UL) /**< @brief Usable for IV                           */
#define CRYPTO_KEY_USAGE_IV_SHIFT           (31UL)         /**< @brief Usable for IV                           */
/** @} */

/**
    @brief Key class ID

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef uint16_t CRYPTO_KeyClassIdType;

/**
    @name Crypto memory handle macros
    @{
    @brief Crypto memory handle macros

    Represented by 32-bits of CRC, 16-bits of random seed, and 16-bits of index.
    Special handles are as described below.

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint64_t CRYPTO_MemHdlType;
#define CRYPTO_MEM_HDL_INVALID           (0x00ULL)                 /**< @brief Invalid Handle                                            */
#define CRYPTO_MEM_HDL_MVK               (0x01ULL)                 /**< @brief Module Manufacturer Verification Key (Public Key in OTP)  */
#define CRYPTO_MEM_HDL_DUK               (0x02ULL)                 /**< @brief Device unique key for IMGL/Update modules cryptography    */
#define CRYPTO_MEM_HDL_DHK               (0x03ULL)                 /**< @brief Diffie Hellman Key for secure sessions */
#define CRYPTO_MEM_HDL_COMP_ID_MASK      (0xFFFF000000000000ULL)   /**< @brief Component ID shift value */
#define CRYPTO_MEM_HDL_COMP_ID_SHIFT     (48UL)                    /**< @brief Component ID mask value */
#define CRYPTO_MEM_HDL_CLASS_ID_MASK     (0xFFFF00000000ULL)       /**< @brief Key class ID shift value */
#define CRYPTO_MEM_HDL_CLASS_ID_SHIFT    (32UL)                    /**< @brief Key class ID mask value */
#define CRYPTO_MEM_HDL_GET_COMP_ID(aHdl) ((BCM_CompIDType)(((aHdl) & CRYPTO_MEM_HDL_COMP_ID_MASK) >> CRYPTO_MEM_HDL_COMP_ID_SHIFT))
                                                                    /**< @brief Retrieve component ID from handle */
#define CRYPTO_MEM_HDL_GET_KEY_CLASS_ID(aHdl) ((uint16_t)(((aHdl) & CRYPTO_MEM_HDL_CLASS_ID_MASK) >> CRYPTO_MEM_HDL_CLASS_ID_SHIFT))
                                                                    /**< @brief Retrieve key class ID from handle */
/** @} */

/**
    @brief Crypto initial vector type

    @trace #BRCM_SWREQ_CRYPTO
*/
typedef uint32_t CRYPTO_MemIvType;
#define CRYPTO_MEM_IV_HOST    (0UL)
#define CRYPTO_MEM_IV_DEVICE  (1UL)

/** @brief Key info type

    @trace #BRCM_SWREQ_CRYPTO

    @note If the user data is more than #CRYPTO_MAX_USER_DATA_SIZE, userDataSize shall be updated to
            actual size while userData shall be filled till #CRYPTO_MAX_USER_DATA_SIZE
*/
typedef struct sCRYPTO_KeyInfoType {
    CRYPTO_KeyType          type;           /**< @brief Output: Key type                */
    CRYPTO_KeyUsageType     usage;          /**< @brief Output: Allowed key usage       */
    BCM_CompIDType          comp;           /**< @brief Output: Associated component ID */
    CRYPTO_KeyClassIdType   keyClass;       /**< @brief Output: Key class ID            */
    uint16_t                keySize;        /**< @brief Output: key data size in bytes  */
    uint16_t                userDataSize;   /**< @brief Output: User data size in bytes */
    uint8_t                 userData[CRYPTO_MAX_USER_DATA_SIZE];
                                            /**< @brief Output: User data               */
} CRYPTO_KeyInfoType;

/**
    @brief Key blob header type

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef struct sCRYPTO_KeyBlobHeaderType {
    uint32_t                magic;      /**< @brief 32-bit magic ‘KBH8’
                                        @xsd constant:CRYPTO_KBH8_MAGIC             */
    CRYPTO_KeyType          type;       /**< @brief Key type  @xsd default:INVALID  */
    CRYPTO_HashFuncType     hashFunc;   /**< @brief Hash function @xsd default:NONE */
    CRYPTO_AlgoModeType     algoMode;   /**< @brief Algorithm mode @xsd default:NONE */
    CRYPTO_KeyUsageType     usage;      /**< @brief Allowed key usage               */
    uint16_t                comp;       /**< @brief Associated component ID
                                        @xsd default:0                              */
    CRYPTO_KeyClassIdType   keyClass;   /**< @brief Key class ID
                                        @xsd default:0                              */
    uint16_t                adSize;     /**< @brief Associated data size
                                        @xsd default:0                              */
    uint16_t                pubSize;    /**< @brief Public key size
                                        @xsd default:0                              */
    uint16_t                privSize;   /**< @brief Private key size
                                        @xsd default:0                              */
    uint16_t                icvSize;    /**< @brief ICV/Signature size
                                        (signed by MVK, DUK or session key)
                                        @xsd default:0                              */
} CRYPTO_KeyBlobHeaderType;

/**
    @brief Key store short blob type

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef struct sCRYPTO_KeyBlob256Type {
    CRYPTO_KeyBlobHeaderType hdr;        /**< @brief Blob header               */
    uint8_t                  data[CRYPTO_KEY_BLOB_256_DATA_SIZE]; /**< @brief Key material
                                        including associated data, public key,
                                        private key, and integrity check value in
                                        that order @xsd hexdec;default:0x00*/
} CRYPTO_KeyBlob256Type;

/**
    @brief Key store 512 blob type

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef struct sCRYPTO_KeyBlob512Type {
    CRYPTO_KeyBlobHeaderType hdr;        /**< @brief Blob header               */
    uint8_t                  data[CRYPTO_KEY_BLOB_512_DATA_SIZE]; /**< @brief Key material
                                        including associated data, public key,
                                        private key, and integrity check value in
                                        that order @xsd hexdec;default:0x00*/
} CRYPTO_KeyBlob512Type;

/**
    @brief Key store 1024 blob type

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef struct sCRYPTO_KeyBlob1024Type {
    CRYPTO_KeyBlobHeaderType hdr;        /**< @brief Blob header               */
    uint8_t                  data[CRYPTO_KEY_BLOB_1024_DATA_SIZE]; /**< @brief Key material
                                        including associated data, public key,
                                        private key, and integrity check value in
                                        that order @xsd hexdec;default:0x00*/
} CRYPTO_KeyBlob1024Type;

/**
    @brief Key store 2048 blob type

    @trace #BRCM_SWREQ_CRYPTO
 */
typedef struct sCRYPTO_KeyBlob2048Type {
    CRYPTO_KeyBlobHeaderType hdr;        /**< @brief Blob header               */
    uint8_t                  data[CRYPTO_KEY_BLOB_2048_DATA_SIZE]; /**< @brief Key material
                                        including associated data, public key,
                                        private key, and integrity check value in
                                        that order @xsd hexdec;default:0x00*/
} CRYPTO_KeyBlob2048Type;

#endif /* CRYPTO_TYPES_H */

/** @} */
