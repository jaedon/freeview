/*
 * Copyright (c) 2008 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_CRYPTO_H__
#define ITK_CRYPTO_H__
#include "itk_os.h"
#include "itk_types.h"
#include "itk_misc.h"

/**
 * @file 
 * Interface to cryptographic routines.
 *
 */

/**Max possible size of SHA1 hash calculation.
 */
#define ITK_CRYTO_MAX_SHA1_LENGTH 20



/**
 * @ingroup host
 * @defgroup itkcrypto Cryptographic interface
 * @{
 *
 *
 */

/**Certificate manager.
 * The certificate manager maintains a cache of certificate chains and 
 * verifications.  The IC creates a certificate manager and adds certificates
 * to it to create an number of certificate chains.  Each certificate chain is 
 * uniquely identified by an id and certificates are added in order from leaf
 * to root. As each new certificate is added the manager should update the chains
 * date with the current itk_date_t.
 * When a chain or chains of certificates have been added the manager can verify
 * signatures and certificates.  Verifications can be cached to improve performance
 * and the cache invalidated when a chain is reset.  Should a duplicate signature
 * or certificate verification be requested the certificate manager may return 
 * the cached result.
 */

typedef struct itk_cert_mngr_t
{
    /**Adds a certificate to the chain identified by id and updates the chains 
     * creation date. Certificates are added in order from leaf to root to create
     * the chain. Certificate data is encoded as defined by ITU-T X.509 and as 
     * profiled in section12.5 of ETSI TS 102 812. Further, the rules for 
     * encoding the organizationName and organizationalUnitName in section 12.5.6
     * shall be ignored. Certificate data does not persist after the call.
     * @param[in]   thiz        A pointer to the certificate manager.
     * @param[in]   id          Identifier for certificate chain.
     * @param[in]   data        A pointer to the certificate data.
     * @param[in]   dataLength  The length of the certificate data.
     */
    itk_validation_t (*addCertificate)(struct itk_cert_mngr_t *thiz, itk_int32_t id,
                    void const *data, size_t dataLength); 

    /**
     * Verifies if any certificate in the chain identified by id was used to create the
     * signature in buffer sigData using the data in the srcData buffer.
     * To improve performance a cache of signatures verified against a certificate 
     * chain can be stored and re-used.  When #resetChain is called the cache 
     * must be invalidated.
     * @param[in]   thiz        A pointer to the certificate manager.
     * @param[in]   id          Cert chain to verify against or -1  all chains.
     * @param[in]   srcData     A pointer to the hash data.
     * @param[in]   srcDataLen  Length of hash data.
     * @param[in]   sigData     A pointer to the signature data.
     * @param[in]   sigDataLen  Length of signature data.
     * @param[in]   rawSignature  Always ITK_DISABLE for public profiles. Should be ignored.
     */
    itk_validation_t  (*verifySignature)(struct itk_cert_mngr_t *thiz, itk_int32_t id, 
                    void *srcData, size_t srcDataLen, void *sigData, size_t sigDataLen,
                    itk_control_t rawSignature );
    
    /**
     * Verifies a certificate against in all chains in the certificate manager.
     * To improve performance a cache of certificate verifications can be stored 
     * and re-used.  When #resetChain is called the cache must be invalidated.
     * @param[in]   thiz        A pointer to the certificate manager.
     * @param[in]   cert        A pointer to the certificate.
     * @param[in]   certLength  The size of the servers tls certificate in bytes.
     */
    itk_validation_t (*verifyCertificate)(struct itk_cert_mngr_t *thiz, void *cert, size_t certLength );

    /**Checks if a certificate chain is older than the given date.
     * @param[in]   thiz        A pointer to the certificate manager.
     * @param[in]   id          Cert chain to check against or -1 for all chains.
     * @param[in]   date        The date expressed as UTC to check against.
     */
    itk_validation_t (*checkDate)(struct itk_cert_mngr_t *thiz, itk_int32_t id, itk_date_t date);
        
    /**Removes all certificates and resets all cached verifications associated with this id. 
     * @param[in]   thiz        A pointer to the certificate manager.
     * @param[in]   id          Cert chain to reset or -1 for all chains.
     */
    void (*resetChain)(struct itk_cert_mngr_t *thiz, itk_int32_t id);

    /**Releases the certificate manager and associated resources.
     * @param[in]   thiz        A pointer to the certificate manager.
     */
    void (*release)(struct itk_cert_mngr_t *thiz);

} itk_cert_mngr_t;

typedef struct itk_crypto_t
{
    /**Generates an SHA1 digest from the data provided.
     * The function shall generate a value with SHA-1 whose computation rules 
     * are as defined in FIPS-180-1.  The prefix param indicates if the data
     * should include a prefix.  The prefix is made of the entry_type of 1 
     * encoded as a 32 bit uimsbf and the file length in bytes encoded as a 
     * 32 bit uimsbf.  If the prefix parameter is ITK_PRESENT the function 
     * should prepend the prefix when generating the digest value.  The result
     * is stored in the digest buffer provided which will be at least 
     * ITK_CRYTO_MAX_SHA1_LENGTH bytes.
     * @param[in]   thiz        A pointer to the crypto interface.
     * @param[in]   prefix      Indicating if prefix is to be included in the digest calculation.
     * @param[in]   data        A pointer to the data buffer.
     * @param[in]   dataLength  The size of the data in bytes.
     * @param[out]  digest      The buffer to store the digest value, min size ITK_CRYTO_MAX_SHA1_LENGTH.
     */
    void (*sha1)(struct itk_crypto_t * thiz, itk_present_t prefix, void *data, size_t dataLength, itk_uint8_t *digest);

    /**Do the base64 encoding of the data buffer and copies the output to the result 
     * @param[in]   thiz            A pointer to the crypto interface.
     * @param[in]   data            A pointer to the data buffer.
     * @param[in]   dataLength      The size of the data in bytes.
     * @param[out]  result          The buffer to store the encoded data.
	 * @param[in]   resultLength    The size of the result in bytes.
     * @return      ITKE_OK         If encoding was successful and encoded data stored in result.
     * @return      ITKE_FAIL       If encoding fails.
     */
    itk_errcode_t (*base64)(struct itk_crypto_t * thiz, void *data, size_t dataLength, itk_uint8_t *result, size_t resultLength);

    /**Freesat Specific function
	 * Do the RSA encrypting of the data using the private key of the receiver 
	 * manufacturer. This shall be the private key corresponding to the public key 
	 * manufacturer shared with broadcasters. For details about the Private Key please refer 
     * freesat_extensions_draft_v0_7.pdf sec FS-13.10.9b.5
     * @param[in]   thiz        A pointer to the crypto interface.
     * @param[in]   data        A pointer to the data buffer.
     * @param[in]   dataLength  The size of the data in bytes. Will be ITK_CRYTO_MAX_SHA1_LENGTH
     * @param[in]   signature   The buffer to store the signature.
	 * @param[in]   sigLength   The size of the signature in bytes. For Freesat profile it will be 128 bytes.
     */
    void (*privateKeySignature)(struct itk_crypto_t * thiz, void *data, size_t dataLength, itk_uint8_t *signature, size_t sigLength);

    /**Returns the version number of the Manufaturer's Private key.
	 * For details about the Private Key please refer freesat_extensions_draft_v0_7.pdf sec FS-13.10.9b.5
     * @param[in]   thiz        A pointer to the crypto interface.
     */
    itk_uint32_t (*privateKeyVersion)(struct itk_crypto_t * thiz);

    /**Creates a certificate manager.
     * @param[in]   thiz        A pointer to the crypto interface.
     * @param[in]   heap        Heap to be used for internal allocations.
     */
    itk_cert_mngr_t *(*newCertificateManager)(struct itk_crypto_t * thiz, itk_heap_t *heap);

    /**
	 * This function is reserved for private profiles.
     * 
     * Retrieves the host's authentication certificate, for verifying application
     * signatures.
     *
     * @param[in] thiz          The host.
     * @param[in] buffer    Pointer to a buffer into which the certificate should be copied.
     * @param[in] length    The length of the buffer.
     * @return The length of the certificate data copied, or 0.
     */
    itk_uint32_t (*getAuthenticationCertificate)(struct itk_crypto_t *thiz, char *buffer, itk_uint32_t length);

} itk_crypto_t;

/** @} */
#endif // ITK_CRYPTO_H__
