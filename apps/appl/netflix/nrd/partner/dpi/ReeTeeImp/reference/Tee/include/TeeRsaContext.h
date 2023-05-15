/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEERSACONTEXT_H_
#define TEERSACONTEXT_H_

#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include "CryptoTypes.h"

typedef enum ShaAlgo
{
    NRDP_SHA1,
    NRDP_SHA224,
    NRDP_SHA256,
    NRDP_SHA384,
    NRDP_SHA512
}NfShaAlgo;

typedef enum Padding
{
    NONE,
    PKCS1,
    PKCS1_OAEP
}Padding;

// Holds the OpenSSL RSA context
struct rsa_context_st {
    RSA* pOsslRsa_;
    int hasPrivateKey_;
    int hasPublicKey_;
};

typedef struct rsa_context_st nfRsaContext_t;

/*********************************************************************
 * @function teeRsaCreateCtxt
 *
 * @brief import and load RSA Key into openssl data structure
 *
 * @param[in] nPtr - pointer to "n" parameter (b64)
 * @param[in] ePtr - pointer to "e" parameter (b64)
 * @param[in] dPtr - pointer to "d" parameter (b64)
 * @param[in] pPtr - pointer to "p" parameter (b64)
 * @param[in] qPtr - pointer to "q" parameter (b64)
 * @param[in] dpPtr - pointer to "dp" parameter (b64)
 * @param[in] dqPtr - pointer to "dq" parameter (b64)
 * @param[in] qiPtr - pointer to "qi" parameter (b64)
 * @param[out] rsaContextPtr - pointer to where you want RSA key context stored
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeRsaCreateCtxt(char  		*nPtr,
									char 			*ePtr,
									char			*dPtr,
									char			*pPtr,
									char			*qPtr,
									char			*dpPtr,
									char			*dqPtr,
									char			*qiPtr,
									nfRsaContext_t	*rsaContextPtr );

/*********************************************************************
 * @function teePrivateDecrypt
 *
 * @brief decrypt using the private RSA key
 *
 * @param[in]  rsaContextPtr 	- pointer to rsa context
 * @param[in]  encStr 			- encrypted string (to be decrypted)
 * @param[in]  encStrLen		- length of encrypted string
 * @param[in]  padding 			- padding
 * @paran[out] decStrLen		- pointer to length of decrypted string
 * @param[out] decStrPtr 		- pointer to decrypted string
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t  teePrivateDecrypt(nfRsaContext_t		*rsaContextPtr,
									  const unsigned char	*encStr,
									  uint32_t				encStrLen,
									  Padding 				padding,
									  uint32_t				*decStrLen,
									  unsigned char			**decStrPtr);

/*********************************************************************
 * @function teeRsaGetPublicRaw
 *
 * @brief get base-64 encoded values of public modulys (n) and public exponent (e)
 *
 * @param[in]  rsaContextPtr 	- pointer to rsa context
 * @param[out] encPubMod 		- pointer to base-64 encoded public modulus string (n)
 * @param[out] modLen			- pointer to length of n
 * @param[out] encPubExp 		- pointer to base-64 encoded public exponent string (e)
 * @param[out] expLen 			- pointer to length of n
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeRsaGetPublicRaw(nfRsaContext_t	*rsaContextPtr,
									 unsigned char	**encPubMod,
									 uint32_t		*modLen,
									 unsigned char	**encPubExp,
									 uint32_t 		*expLen);

/*********************************************************************
 * @function teeRsaSha256Verify
 *
 * @brief RSA SHA256 verify using public key
 *
 * @param[in]  rsaContextPtr 	- pointer to rsa context
 * @param[in] dataLen 			- length of data
 * @param[in] data				- pointer to data (on which SHA will be computed)
 * @param[in] shaLen 			- length of sha
 * @param[in] sha 				- pointer to sha to compare against
 *
 * @returns nfTCR_OKAY if it verifies ok, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeRsaSha256Verify(nfRsaContext_t		*rsaContextPtr,
									   uint32_t 			dataLen,
									   const unsigned char  *data,
									   uint32_t 			shaLen,
									   unsigned char		*sha);

#if 0
nfTeeCryptoResult_t getRsaContext(nfRsaContext_t context);

void freeRsaContext(rsa_context** context);

nfTeeCryptoResult_t generate(rsa_context* context, unsigned int nBits, unsigned long long int publicExponent);

nfTeeCryptoResult_t  setRaw(rsa_context* context, const unsigned char* pubMod,
                            const unsigned char* pubExp, const unsigned char* privMod);

nfTeeCryptoResult_t  setPublicPkcs1(rsa_context* context, const unsigned char** pubKeyPkcs1Der);
nfTeeCryptoResult_t getPublicPkcs1(rsa_context* context, /*out*/unsigned char** pubKeyPkcs1Der);

nfTeeCryptoResult_t  setPublicSpki(rsa_context* context, const unsigned char** pubKeySpkiDer);
nfTeeCryptoResult_t  getPublicSpki(rsa_context* context, /*out*/unsigned char** pubKeySpkiDer);


nfTeeCryptoResult_t  setPrivatePkcs1(rsa_context* context, const unsigned char** privKeyPkcs1Der);
nfTeeCryptoResult_t  getPrivatePkcs1(rsa_context* context, /*out*/unsigned char** privKeyPkcs1Der);

nfTeeCryptoResult_t  setPrivatePkcs8(rsa_context* context, const unsigned char* pkcs8);
nfTeeCryptoResult_t  getPrivatePkcs8(rsa_context* context, /*out*/unsigned char** pkcs8);

nfTeeCryptoResult_t  publicEncrypt(rsa_context* context, const unsigned char* in,
                                   /*out*/unsigned char** out, Padding padding);

nfTeeCryptoResult_t  privateSign(rsa_context* context, const unsigned char* data, NfShaAlgo shaAlgo,
                                 /*out*/unsigned char** sig);

nfTeeCryptoResult_t  publicVerify(rsa_context* context, const unsigned char* data, NfShaAlgo shaAlgo,
                                  unsigned char** sig);

#endif

#endif /* TEERSACONTEXT_H_ */
