/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_aes_sw_ifc Interface
    @ingroup grp_aes_sw

    @addtogroup grp_aes_sw_ifc
    @{
    @file aes.h
    @brief AES SW routines API header
    This file contains the interface function declarations for AES
    SW routines

    @version 0.1 Initial Version
*/
#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <bcm_err.h>
#include <crypto_common.h>

/**
    @name AES Architecture IDs
    @{
    @brief Architecture IDs for the AES API's
*/
#define BRCM_SWARCH_AES_GCM_OP_MODE_TYPE                           (0x8201U) /**< @brief #AES_GCMOpModeType                        */
#define BRCM_SWARCH_AES_GCM_CONTEXT_STATE_TYPE                     (0x8202U) /**< @brief #AES_GCMContextStateType                  */
#define BRCM_SWARCH_AES_GCM_CONTEXT_SIZE_MACRO                     (0x8203U) /**< @brief #AES_GCM_CONTEXT_SIZE                     */
#define BRCM_SWARCH_AES_CMAC_CONTEXT_SIZE_MACRO                    (0x8204U) /**< @brief #AES_CMAC_CONTEXT_SIZE                    */
#define BRCM_SWARCH_AES_GCM_CONTEXT_TYPE                           (0x8205U) /**< @brief #AES_GCMContextType                       */
#define BRCM_SWARCH_AES_CMAC_CONTEXT_TYPE                          (0x8206U) /**< @brief #AES_CMACContextType                      */
#define BRCM_SWARCH_AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN_MACRO (0x8207U) /**< @brief #AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN */
#define BRCM_SWARCH_AES_ECB_ENCRYPT_BLOCK_PROC                     (0x8208U) /**< @brief #AES_ECBEncryptBlock                      */
#define BRCM_SWARCH_AES_ECB_DECRYPT_BLOCK_PROC                     (0x8209U) /**< @brief #AES_ECBDecryptBlock                      */
#define BRCM_SWARCH_AES_GCM_INIT_CONTEXT_PROC                      (0x820AU) /**< @brief #AES_GCMInitContext                       */
#define BRCM_SWARCH_AES_GCM_AAD_UPDATE_PROC                        (0x820BU) /**< @brief #AES_GCMAADUpdate                         */
#define BRCM_SWARCH_AES_GCM_AAD_FINISH_PROC                        (0x820CU) /**< @brief #AES_GCMAADFinish                         */
#define BRCM_SWARCH_AES_GCM_CRYPT_UPDATE_PROC                      (0x820DU) /**< @brief #AES_GCMCryptUpdate                       */
#define BRCM_SWARCH_AES_GCM_FINISH_AND_TAG_PROC                    (0x820EU) /**< @brief #AES_GCMFinishAndTag                      */
#define BRCM_SWARCH_AES_GCM_FINISH_AND_AUTH_PROC                   (0x820FU) /**< @brief #AES_GCMFinishAndAuth                     */
#define BRCM_SWARCH_AES_CMAC_INIT_CONTEXT_PROC                     (0x8210U) /**< @brief #AES_CMACInitContext                      */
#define BRCM_SWARCH_AES_CMAC_UPDATE_PROC                           (0x8211U) /**< @brief #AES_CMACUpdate                           */
#define BRCM_SWARCH_AES_CMAC_FINISH_AND_TAG_PROC                   (0x8212U) /**< @brief #AES_CMACFinishAndTag                     */
#define BRCM_SWARCH_AES_KBKDF_CTRMODE_CMACPRF_PROC                 (0x8213U) /**< @brief #AES_KBKDFCtrModeCmacPrf                  */
#define BRCM_SWARCH_AES_KEY_WRAP_PROC                              (0x8214U) /**< @brief #AES_KeyWrap                              */
#define BRCM_SWARCH_AES_KEY_UNWRAP_PROC                            (0x8215U) /**< @brief #AES_KeyUnwrap                            */
/** @} */

/** @{
    @brief AES GCM operation modes

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
typedef uint32_t AES_GCMOpModeType;
#define AES_GCM_OP_MODE_ENCRYPT_AND_TAG         (1UL)
#define AES_GCM_OP_MODE_DECRYPT_AND_AUTH        (2UL)
/** @} */

/** @{
    @brief AES GCM context states

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
typedef uint32_t AES_GCMContextStateType;
#define AES_GCM_CONTEXT_STATE_AAD_PROCESSING    (1UL)
#define AES_GCM_CONTEXT_STATE_AAD_DONE          (2UL)
#define AES_GCM_CONTEXT_STATE_CRYPT_PROCESSING  (3UL)
#define AES_GCM_CONTEXT_STATE_DONE              (4UL)
/** @} */

/** @brief AES GCM context size

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
#define AES_GCM_CONTEXT_SIZE                    (768UL)

/** @brief AES CMAC context size

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
#define AES_CMAC_CONTEXT_SIZE                   (384UL)

/**
    @brief Maximum length of the strings supported by the #AES_KBKDFCtrModeCmacPrf function

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
#define AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN (2048UL)

/** @brief AES GCM context

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
typedef struct sAES_GCMContextType {
    uint64_t privData[AES_GCM_CONTEXT_SIZE / 8UL];   /**< @brief Implementation private data */
} AES_GCMContextType;

/** @brief AES CMAC context

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
typedef struct sAES_CMACContextType {
    uint64_t privData[AES_CMAC_CONTEXT_SIZE / 8UL];  /**< @brief Implementation private data */
} AES_CMACContextType;

/** @brief AES-ECB Encrypt one block (128 bits) of plain-text

    @behavior Sync, reentrant

    @pre None

    @param[in]      aKeyType                    AES key type
    @param[in]      aCipherKey                  Pointer to the cipher key
    @param[in]      aPlainText                  Pointer to the plain-text to encrypt
    @param[out]     aCipherText                 Pointer to the memory where the cipher-text
                                                is to be written

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        Invalid keyType,
                                                aCipherKey is NULL,
                                                aPlainText is NULL,
                                                aCipherText is NULL
    @retval         BCM_ERR_OK                  Encryption complete

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_ECBEncryptBlock(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey, const uint8_t *const aPlainText, uint8_t *const aCipherText);

/** @brief AES-ECB Decrypt one block (128 bits) of cipher-text

    @behavior Sync, reentrant

    @pre None

    @param[in]      aKeyType                    AES key type
    @param[in]      aCipherKey                  Pointer to the cipher key
    @param[in]      aCipherText                 Pointer to the cipher-text to decrypt
    @param[out]     aPlainText                  Pointer to the memory where the plain-text
                                                is to be written

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        Invalid keyType,
                                                aCipherKey is NULL,
                                                aPlainText is NULL,
                                                aCipherText is NULL
    @retval         BCM_ERR_OK                  Encryption complete

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_ECBDecryptBlock(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey, const uint8_t *const aCipherText, uint8_t *const aPlainText);

/** @brief Initialize an AES-GCM context

    @behavior Sync, reentrant

    @pre None

    @param[in]      aKeyType                    AES key type
    @param[in]      aOpMode                     GCM operation mode
    @param[in]      aCipherKey                  Pointer to the cipher key
    @param[in]      aIV                         Initialization vector. Expected to be 12 bytes long
    @param[out]     aCtx                        COntext structure to initialize

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        Invalid keyType,
                                                Invalid aOpMode,
                                                aCipherKey is NULL,
                                                aIV is NULL,
                                                aCtx is NULL
    @retval         BCM_ERR_OK                  Context init sucessful

    @post The context is initialized and is in #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMInitContext(const CRYPTO_KeyType aKeyType, const AES_GCMOpModeType aOpMode, const uint8_t *const aCipherKey, const uint8_t *const aIV, AES_GCMContextType *const aCtx);

/** @brief Update the digest computation of the additional authentication data

    @behavior Sync, reentrant

    @pre The context is in #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state

    @param[inout]   aCtx                        GCM context
    @param[in]      aAADLen                     Length in bytes of the AAD to digest.
                                                Must be a non-zero multiple of 16.
                                                If the client has no furthur AAD to process,
                                                AES_GCMCryptUpdate() or AES_GCMFinish*() should
                                                be directly called without invoking this API
    @param[in]      aAAD                        AAD to digest

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        aCtx NULL or invalid,
                                                aAADLen is 0 or a non-multiple of 16
                                                aAAD is NULL
    @retval         BCM_ERR_INVAL_STATE         aCtx is not in #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state
                                                Cannot digest aAADLen more bytes
    @retval         BCM_ERR_OK                  Data pointed by aAAD digested

    @post The context is still at #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state upon returning
        from this API call

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMAADUpdate(AES_GCMContextType *const aCtx, const uint32_t aAADLen, const uint8_t *const aAAD);

/** @brief Finish the digest computation of the additional authentication data

    @behavior Sync, reentrant

    @pre The context is in #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state

    @param[inout]   aCtx                        GCM context
    @param[in]      aAADLen                     Length in bytes of the AAD to digest.
                                                Must be non-zero.
                                                If the client has no furthur AAD to process,
                                                AES_GCMCryptUpdate() or AES_GCMFinish*() should
                                                be directly called without invoking this API
    @param[in]      aAAD                        AAD to digest

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        aCtx NULL or invalid,
                                                aAADLen is 0
                                                aAAD is NULL
    @retval         BCM_ERR_INVAL_STATE         aCtx is not in #AES_GCM_CONTEXT_STATE_AAD_PROCESSING state
                                                Cannot digest aAADLen more bytes
    @retval         BCM_ERR_OK                  Data pointed by aAAD digested

    @post The context is transitioned to #AES_GCM_CONTEXT_STATE_AAD_DONE state upon a sucessful
        return from this API call

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMAADFinish(AES_GCMContextType *const aCtx, const uint32_t aAADLen, const uint8_t *const aAAD);

/** @brief AES-GCM Encrypt/Decrypt the data input while updating the internal digest computation for authentication

    This API also supports the GMAC only operation. To use this mode, the aDataOut should
    be set to NULL. The implementation then assumes that aDataIn contains the ciphertext
    and computes the GMAC over it.

    @behavior Sync, reentrant

    @pre The context should be in one of the following states
        -# AES_GCM_CONTEXT_STATE_AAD_PROCESSING
        -# AES_GCM_CONTEXT_STATE_AAD_DONE
        -# AES_GCM_CONTEXT_STATE_CRYPT_PROCESSING

    @param[inout]   aCtx                        GCM context
    @param[in]      aDataLen                    Length of the input data in bytes to process.
                                                Must be a non-zero multiple of 16.
    @param[in]      aDataIn                     Pointer to the crypt data to process.
                                                If aDataOut parameter is not NULL, this data
                                                is processed by both the crypt and the GMAC engines.
                                                If aDataOut is NULL, this pointer is assumed to
                                                be pointing to the ciphertext and only the GMAC computation
                                                is updated. Encryption/decryption operations are not
                                                performed.
    @param[out]     aDataOut                    Pointer to the memory to which the crypt output
                                                is to be written to.
                                                Can be NULL (for GMAC only operation).
                                                aDataLen bytes will be written to this buffer

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        aCtx NULL or invalid,
                                                aDataLen is 0 or a non-multiple of 16
                                                aDataIn is NULL.
    @retval         BCM_ERR_INVAL_STATE         aCtx is not in the state specified in the preconditions,
                                                Cannot digest aDataLen more bytes
    @retval         BCM_ERR_OK                  Crypt output is written to the buffer pointed by aDataOut.
                                                Internal GCM digest computation is updated

    @post The context is transitioned to #AES_GCM_CONTEXT_STATE_CRYPT_PROCESSING state upon a sucessful
        return from this API call

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMCryptUpdate(AES_GCMContextType *const aCtx, const uint32_t aCryptInputLen, const uint8_t *const aCryptInput, uint8_t *const aCryptOutput);

/** @brief AES-GCM Encrypt the data input, finish the digest computation and generate an auth tag

    This API also supports the GMAC only operation. To use this mode, the aDataOut should
    be set to NULL. The implementation then assumes that aDataIn contains the ciphertext
    and computes the GMAC over it.

    @behavior Sync, reentrant

    @pre The context should be inited with opMode: #AES_GCM_OP_MODE_ENCRYPT_AND_TAG and should be in one of the following states
        -# AES_GCM_CONTEXT_STATE_AAD_PROCESSING
        -# AES_GCM_CONTEXT_STATE_AAD_DONE
        -# AES_GCM_CONTEXT_STATE_CRYPT_PROCESSING

    @param[inout]   aCtx                        GCM context
    @param[in]      aDataLen                    Length of the crypt data in bytes to process.
                                                Can be zero.
    @param[in]      aDataIn                     Pointer to the crypt data to process.
                                                Must be NULL when aDataLen is 0.
                                                If aDataOut parameter is not NULL, this data
                                                is processed by both the crypt and the GMAC engines.
                                                If aDataOut is NULL, this pointer is assumed to
                                                be pointing to the ciphertext and only the GMAC computation
                                                is updated. Encryption/decryption operations are not
                                                performed.
    @param[out]     aDataOut                    Pointer to the memory where the encrypted data is to be written.
                                                Must be NULL when aDataLen is 0.
                                                Can be NULL when aDataLen is not 0 (for GMAC only operation).
                                                aDataLen bytes will be written to this buffer.
    @param[out]     aAuthTag                    16 byte authentication tag to autrhenticate all the processed data

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        aCtx NULL or invalid,
                                                aDataLen is 0 but aDataIn and aDataOut are not NULL
                                                aDataLen is not 0 but aDataIn is NULL
                                                aAuthTag is NULL
    @retval         BCM_ERR_INVAL_STATE         aCtx was not created with #AES_GCM_OP_MODE_ENCRYPT_AND_TAG opMode
                                                aCtx is not in the state specified in the preconditions
                                                Cannot digest aDataLen more bytes
    @retval         BCM_ERR_OK                  Crypt output is written to the buffer pointed by aDataOut (if aDataLen > 0).
                                                The 16 byte AES-GCM authentication tag is written to the buffer pointed by aAuthTag

    @post The context is transitioned to #AES_GCM_CONTEXT_STATE_DONE state upon a sucessful
        return from this API call

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMFinishAndTag(AES_GCMContextType *const aCtx, const uint32_t aCryptInputLen, const uint8_t *const aCryptInput, uint8_t *const aCryptOutput, uint8_t *const aAuthTag);

/** @brief AES-GCM Decrypt the data input, finish the digest computation and validate the authentication tag

    This API also supports the GMAC only operation. To use this mode, the aDataOut should
    be set to NULL. The implementation then assumes that aDataIn contains the ciphertext
    and computes the GMAC over it.

    @behavior Sync, reentrant

    @pre The context should be inited with opMode: #AES_GCM_OP_MODE_DECRYPT_AND_AUTH and should be in one of the following states
        -# AES_GCM_CONTEXT_STATE_AAD_PROCESSING
        -# AES_GCM_CONTEXT_STATE_AAD_DONE
        -# AES_GCM_CONTEXT_STATE_CRYPT_PROCESSING

    @param[inout]   aCtx                        GCM context
    @param[in]      aDataLen                    Length of the crypt data in bytes to process.
                                                Can be zero.
    @param[in]      aDataIn                     Pointer to the crypt data to process.
                                                Must be NULL when aDataLen is 0.
                                                If aDataOut parameter is not NULL, this data
                                                is processed by both the crypt and the GMAC engines.
                                                If aDataOut is NULL, this pointer is assumed to
                                                be pointing to the ciphertext and only the GMAC computation
                                                is updated. Encryption/decryption operations are not
                                                performed.
    @param[out]     aDataOut                    Pointer to the memory where the encrypted data is to be written.
                                                Must be NULL when aDataLen is 0.
                                                Can be NULL when aDataLen is not 0 (for GMAC only operation).
                                                aDataLen bytes will be written to this buffer.
    @param[in]      aAuthTag                    16 byte authentication tag to validate

    Return values are documented in reverse-chronological order
    @retval         BCM_ERR_INVAL_PARAMS        aCtx NULL or invalid,
                                                aDataLen is 0 but aDataIn and aDataOut are not NULL
                                                aDataLen is not 0 but aDataIn is NULL
                                                aAuthTag is NULL
    @retval         BCM_ERR_INVAL_STATE         aCtx was not created with #AES_GCM_OP_MODE_ENCRYPT_AND_TAG opMode
                                                aCtx is not in the state specified in the preconditions
                                                Cannot digest aDataLen more bytes
    @retval         BCM_ERR_AUTH_FAILED         Authentication failed
    @retval         BCM_ERR_OK                  Crypt output is written to the buffer pointed by aDataOut (if aDataLen > 0).

    @post The context is transitioned to #AES_GCM_CONTEXT_STATE_DONE state when BCM_ERR_AUTH_FAILED
        or BCM_ERR_OK is the return value of this API call

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_GCMFinishAndAuth(AES_GCMContextType *const aCtx, const uint32_t aCryptInputLen, const uint8_t *const aCryptInput, uint8_t *const aCryptOutput, const uint8_t *const aAuthTag);

/**
    @brief Initialize an AES-CMAC context.

    AES-CMAC is specified in NIST SP 800-38B and RFC 4493

    @behaviour Sync, Non-Reentrant

    @pre None

    @param[in]      aKeyType                    The AES keytype
    @param[in]      aCipherKey                  The AES cipher key to be used to compute the MAC
    @param[inout]   aCtx                        The AES-CMAC context to initialize


    @retval         #BCM_ERR_OK                 CMAC context initialized successfully
    @retval         #BCM_ERR_INVAL_PARAMS       aKeyType is invalid.
                                                aCipherKey is NULL.
                                                aCtx is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_CMACInitContext(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey, AES_CMACContextType *const aCtx);

/**
    @brief Update the AES-CMAC computation by processing a new data chunk

    @behaviour Sync, Non-Reentrant

    @pre None

    @param[inout]   aCtx                        The AES-CMAC context
    @param[in]      aDataLen                    Length of the data to process, in bytes.
                                                Should be a non-zero multiple of 16.
    @param[in]      aDataIn                     Buffer holding the input data to process.


    @retval         #BCM_ERR_OK                 Input data digested successfully.
    @retval         #BCM_ERR_INVAL_PARAMS       aCtx is NULL or is invalid.
                                                aDataLen is 0 or is not a multiple of 16.
                                                aDataIn is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_CMACUpdate(AES_CMACContextType *const aCtx, const uint32_t aDataLen, const uint8_t *const aDataIn);

/**
    @brief Update and finalize the AES-CMAC computation

    @behaviour Sync, Non-Reentrant

    @pre None

    @param[inout]   aCtx                        The AES-CMAC context
    @param[in]      aDataLen                    Length of the data to process, in bytes.
                                                Should be non-zero.
    @param[in]      aDataIn                     Buffer holding the input data to process.
    @param[out]     aAuthTag                    Buffer to which the computed 16-byte tag is to be written to.


    @retval         #BCM_ERR_OK                 AES-CMAC computation complete. Tag has been written into the
                                                buffer provided.
    @retval         #BCM_ERR_INVAL_PARAMS       aCtx is NULL or is invalid.
                                                aDataLen is 0.
                                                aDataIn is NULL.
                                                aAuthTag is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_CMACFinishAndTag(AES_CMACContextType *const aCtx, const uint32_t aDataLen, const uint8_t *const aDataIn, uint8_t *const aAuthTag);

/**
    @brief Key based KDF specified in NIST SP800-108, with AES-CMAC PRF and counter mode of operation

    The algorithm, specified in the section 4.1 of the said publication has a parameter 'r'. This
    implementation fixes this parameter to 8. Also, 'L' is encoded with 16 bits.

    @behaviour Sync, Non-Reentrant

    @pre None

    @param[in]      aKeyType                    The AES keytype
    @param[in]      aCipherKey                  The AES cipherkey input to the KDF construction
    @param[in]      aLabelLen                   KDF label input string length in bytes
    @param[in]      aLabel                      Pointer to the KDF label input
    @param[in]      aContextLen                 KDF context input string length in bytes
    @param[in]      aContext                    Pointer to the KDF context input
    @param[in]      aDkmLen                     Required length in bytes of the output derived key material string
    @param[out]     aDkm                        The buffer into which the output string is to be written into


    @retval         #BCM_ERR_OK                 DKM populated into the provided buffer
    @retval         #BCM_ERR_INVAL_PARAMS       aKeyType is invalid.
                                                aCipherKey is NULL.
                                                aLabelLen is greater than #AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN.
                                                aLabel is NULL while aLabelLen is non-zero.
                                                aContextLen is greater than #AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN.
                                                aContext is NULL while aContextLen is non-zero.
                                                aDkmLen is 0 or is greater than #AES_KBKDF_CTRMODE_CMACPRF_MAX_STRING_LEN.
                                                aDkm is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_KBKDFCtrModeCmacPrf(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey,
                                const uint32_t aLabelLen, const uint8_t *const aLabel,
                                const uint32_t aContextLen, const uint8_t *const aContext,
                                const uint32_t aDkmLen, uint8_t *const aDkm);

/**
    @brief RFC 3394, AES key wrapping function

    @brief Sync, Non-Reentrant

    @pre None

    @param[in]      aKeyType                    The AES keytype
    @param[in]      aCipherKey                  The AES cipherkey to be used as a KEK
    @param[in]      aPlainTextLen               Length of the key material to wrap in bytes.
                                                Must be a non-zero multiple of 8.
    @param[in]      aPlainText                  Pointer to the key material to wrap
    @param[out]     aCipherText                 Buffer to write the wrapped key material into.
                                                (aPlainTextLen + 8) bytes will be written to this buffer.


    @retval         #BCM_ERR_OK                 Key wrapped successfully.
    @retval         #BCM_ERR_INVAL_PARAMS       aKeyType is invalid.
                                                aCipherKey is NULL.
                                                aPlainTextLen is 0 or is not a multiple of 8.
                                                aPlainText is NULL.
                                                aCipherText is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_KeyWrap(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey,
                    const uint32_t aPlainTextLen, const uint8_t *const aPlainText,
                    uint8_t *const aCipherText);

/**
    @brief RFC 3394, AES key unwrapping function

    @brief Sync, Non-Reentrant

    @pre None

    @param[in]      aKeyType                    The AES keytype
    @param[in]      aCipherKey                  The AES cipherkey to be used as a KDK
    @param[in]      aCipherTextLen              Length of the key material to wrap in bytes.
                                                Must be a non-zero multiple of 8 and greater than 8.
    @param[in]      aCipherText                 Pointer to the key material to unwrap
    @param[out]     aPlainText                  Buffer to write the unwrapped key material into.
                                                (aCipherTextLen - 8) bytes will be written to this buffer.


    @retval         #BCM_ERR_OK                 Key wrapped successfully.
    @retval         #BCM_ERR_AUTH_FAILED        Key authentication failed.
    @retval         #BCM_ERR_INVAL_PARAMS       aKeyType is invalid.
                                                aCipherKey is NULL.
                                                aCipherTextLen is 0 or 8 or is not a multiple of 8.
                                                aCipherText is NULL.
                                                aPlainText is NULL.

    @post None

    @trace #BRCM_SWREQ_AES_SW_ROUTINES
*/
int32_t AES_KeyUnwrap(const CRYPTO_KeyType aKeyType, const uint8_t *const aCipherKey,
                      const uint32_t aCipherTextLen, const uint8_t *const aCipherText,
                      uint8_t *const aPlainText);

#endif
/** @} */
