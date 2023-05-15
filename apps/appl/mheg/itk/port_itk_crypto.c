
/**
	@file	  port_itk_crypto.c														\n
	@brief	  itk file for Crypto porting layer							\n

	Description: ITK에서 Crypto Porting 에 사용되는 기능을 정의한 Body파일	\n
	Module: mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/asn1t.h>
#include <openssl/objects.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include "curl/curl.h"

#include "port_itk_main.h"
#include "port_itk_misc.h"
#include "port_itk_console.h"
#include "port_itk_crypto.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define HUTL_htonl(x) (itk_uint32_t)(\
	((itk_uint32_t)((x & 255)) << 24) +\
	((itk_uint32_t)((x >> 8) & 255)  << 16) +\
	((itk_uint32_t)((x >> 16) & 255) << 8) +\
	((itk_uint32_t)((x >> 24) & 255)))
#define HUTL_ntohl	HUTL_htonl

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagMhegCryptoInfo
{
	redkey_profile_t	itkProfile;
	itk_crypto_t		*pObject;
} MHEG_CRYPTO_INFO_t;

/** internal certificate storage */
typedef struct cert_t
{
	struct cert_t *next;	/**< next chanin */
	itk_int32_t id;		/**< chain identifier */
	STACK_OF(X509) *chain;	/**< open ssl chain */
	itk_date_t date;		/**< date/time of last update in UTC */
} cert_t;

/** internal storage structure for cert manager */
typedef struct certmngr_t
{
	itk_cert_mngr_t vtable;	/**< api jump table */
	itk_crypto_t *crypto;	/**< parent interface */
	cert_t *certs;			/**< list of certificates */
	itk_heap_t *heap;		/**< heap for working memory */
} certmngr_t;

/* OpenSSL ASN.1 definitions for the signature file */
typedef struct AUTH_SIGNATURE
{
	AUTHORITY_KEYID *authkeyid;
	ASN1_OBJECT *sig_alg;
	ASN1_BIT_STRING *signature;
} AUTH_SIGNATURE;

DECLARE_ASN1_FUNCTIONS(AUTH_SIGNATURE)

ASN1_SEQUENCE(AUTH_SIGNATURE) = {
	ASN1_SIMPLE(AUTH_SIGNATURE, authkeyid, AUTHORITY_KEYID),
	ASN1_SIMPLE(AUTH_SIGNATURE, sig_alg, ASN1_OBJECT),
	ASN1_SIMPLE(AUTH_SIGNATURE, signature, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(AUTH_SIGNATURE)

IMPLEMENT_ASN1_FUNCTIONS(AUTH_SIGNATURE)

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static MHEG_CRYPTO_INFO_t	s_mhegCryptoInfo;

/*
* Manufacturers should create a public-private 1024 bit RSA key pair for each receiver model.
* The resident program will use the private key of that pair. The public key will be used by
* broadcasters to verify the signatures created. (FS-13.10.9b.5 of freesat_extensions_draft_v0_7.pdf)
*/
/*
-----BEGIN RSA PRIVATE KEY-----
MIICXQIBAAKBgQC8IeRzPZ80iEcKTVW6KHYFBE5K9eM/IEjvwz3Xt1ZSCvIqbEAE
ybO6DP5uKG6DWUnMNwyIpIOi4O/Dd6MX7CXVJG3UAzAsjv5xH+AkyGj8NNBLK1BA
C9+ml/uRjK/4cnOOwoPw/wj5K2Wdd68l9fbMjCPGLuHvRuUwNpXZhsVb3wIDAQAB
AoGASG/8WGr3q/B0+gCA66a8mn3n73DxyNKTgs34N+D9uaVBTRhVJgLL0u1B4gRr
WOO+SIVBrDM8BDD74JVmFMX2YorJ+TPcivwD7Zslsfso61ykpuoywtEHjil82ZTW
vLWGyQt/rHEAIV8k5xK2W6oJpP5Bf0oEM3oDJyMG/jI+yaECQQD4wic999rq983e
ezbSTzX6fRPk9gHLRhV1NZPf+eDZEiSDtExt3jOB8hdC2pAkjYZDZiIcjxay54xq
qjU9fOMXAkEAwZvuC5at9HeD6qlGf6NV95xyg2NsnggtJ+/mPiW7menHZO3LZTu1
52TklFdz0/69u55G7Lsz+5Xnvwjy0Q3qeQJBAPY5DyiqPXlYIOD3qtLObspqpZd6
3Vm8HB8xEhafOi0CZTwr6X7Scr9LxjR5LDXSE7SA0oDcLQaWZIlrwlkDVa8CQQCv
wEDjPDFFTojtqCRbEYakc9xv8P1Up892BYxYtiAKo/FhaYvNpdqG50uobYqz0veh
LRjtJTMXGwl4nULvC1bhAkAC8VPv0DZ5jpFBT3TbHeIfd6NuaZ9LX/NI+Hu1smvH
a1NJWt5lRXCUkt+HRu7+UHGfT3qNVkBjNj/Zlt1Z9S3p
-----END RSA PRIVATE KEY-----

-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC8IeRzPZ80iEcKTVW6KHYFBE5K
9eM/IEjvwz3Xt1ZSCvIqbEAEybO6DP5uKG6DWUnMNwyIpIOi4O/Dd6MX7CXVJG3U
AzAsjv5xH+AkyGj8NNBLK1BAC9+ml/uRjK/4cnOOwoPw/wj5K2Wdd68l9fbMjCPG
LuHvRuUwNpXZhsVb3wIDAQAB
-----END PUBLIC KEY-----
*/

unsigned char	rsa_PrivateKey[] = "MIICXQIBAAKBgQC8IeRzPZ80iEcKTVW6KHYFBE5K9eM/IEjvwz3Xt1ZSCvIqbEAEybO6DP5uKG6DWUnMNwyIpIOi4O/Dd6MX7CXVJG3UAzAsjv5xH+AkyGj8NNBLK1BAC9+ml/uRjK/4cnOOwoPw/wj5K2Wdd68l9fbMjCPGLuHvRuUwNpXZhsVb3wIDAQABAoGASG/8WGr3q/B0+gCA66a8mn3n73DxyNKTgs34N+D9uaVBTRhVJgLL0u1B4gRrWOO+SIVBrDM8BDD74JVmFMX2YorJ+TPcivwD7Zslsfso61ykpuoywtEHjil82ZTWvLWGyQt/rHEAIV8k5xK2W6oJpP5Bf0oEM3oDJyMG/jI+yaECQQD4wic999rq983eezbSTzX6fRPk9gHLRhV1NZPf+eDZEiSDtExt3jOB8hdC2pAkjYZDZiIcjxay54xqqjU9fOMXAkEAwZvuC5at9HeD6qlGf6NV95xyg2NsnggtJ+/mPiW7menHZO3LZTu152TklFdz0/69u55G7Lsz+5Xnvwjy0Q3qeQJBAPY5DyiqPXlYIOD3qtLObspqpZd63Vm8HB8xEhafOi0CZTwr6X7Scr9LxjR5LDXSE7SA0oDcLQaWZIlrwlkDVa8CQQCvwEDjPDFFTojtqCRbEYakc9xv8P1Up892BYxYtiAKo/FhaYvNpdqG50uobYqz0vehLRjtJTMXGwl4nULvC1bhAkAC8VPv0DZ5jpFBT3TbHeIfd6NuaZ9LX/NI+Hu1smvHa1NJWt5lRXCUkt+HRu7+UHGfT3qNVkBjNj/Zlt1Z9S3p";
unsigned long	rsa_KeyVersion = 10005;


/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/
static cert_t *newCert(certmngr_t *certmgr);
static void remCert(certmngr_t *certmgr, cert_t *cert);
static AUTH_SIGNATURE *parseSigFile(void *data, size_t dataLength);
static void *getDirectoryName(AUTH_SIGNATURE *sig);
static itk_validation_t verifyDigestSig(X509 *cert, void *digest,  size_t digestLength, void *sig, size_t sigLength, int digest_type);


/********************************************************************/
/********************      functions  ********************/
/********************************************************************/

static itk_validation_t addCertificate(struct itk_cert_mngr_t *thiz, itk_int32_t id,
					void const *data, size_t dataLength)
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	cert_t *cert=NULL;
	const unsigned char *ptr,*p_in;
	X509 *x509;

#if	defined(CONFIG_DEBUG)
	if(thiz)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
	if(data)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(cmngr);
	HxLOG_Assert(data);
#endif

	/* check list for id */
	cert=cmngr->certs;
	while(cert)
	{
		/* already exists */
		if (cert->id==id)
		{
			break;
		}
		cert = cert->next;
	}
	/* new cert */
	if (cert==NULL)
	{
		cert = newCert(cmngr);
		if (cert==NULL)
		{
			HxLOG_Error("ERROR\n");
			return ITK_NOT_VALID;
		}
		/* store identifier */
		cert->id = id;
		/* init chain */
		cert->chain = sk_X509_new_null();
		if (cert->chain == NULL)
		{
			remCert(cmngr, cert);
			HxLOG_Error("ERROR\n");
			return ITK_NOT_VALID;
		}
	}

	/* create x509 cert from data */
	ptr=p_in=data;
	x509 = d2i_X509(NULL, &p_in, (long)dataLength);
	if (x509==NULL || p_in != (unsigned char *)ptr + dataLength)
	{
		HxLOG_Error("addCertificate: Data error\n");
		X509_free(x509);
		return ITK_NOT_VALID;
	}
	else
	{
		int i;
		// update date
		cert->date = mheg_itk_port_miscGetDate((struct itk_misc_t *)mheg_itk_port_miscGetObject());
		// check for duplicates
		for(i=0;i<sk_X509_num(cert->chain);i++)
		{
			X509 *c = sk_X509_value(cert->chain,i);
			if (X509_cmp(c,x509)==0)//great logic!
			{
				X509_free(x509);
				return ITK_VALID;//already on
			}
		}
		// new cert, add to chain
		sk_X509_push(cert->chain,x509);
		return ITK_VALID;
	}
}

static itk_validation_t verifySignature(struct itk_cert_mngr_t *thiz, itk_int32_t id,
					void *srcData, size_t srcDataLen, void *sigData, size_t sigDataLen, itk_control_t raw )
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	cert_t *certs;
	AUTH_SIGNATURE *sig;
	int i;
	itk_validation_t result=ITK_NOT_VALID;/*default, we cant match sign to cert*/

	HxLOG_Debug("dataLength(%d), sigDataLen(%d)\n", srcDataLen, sigDataLen);

#if	defined(CONFIG_DEBUG)
	if(cmngr)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
	if(srcData)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
	if(sigData)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(cmngr);
	HxLOG_Assert(srcData);
	HxLOG_Assert(sigData);
#endif

	certs = cmngr->certs;

	if( raw == ITK_ENABLE && sigDataLen == 128 )
	{
		unsigned char sha[SHA_DIGEST_LENGTH];

		// this is a raw sha-1 signature block. All we have to do is check the first certificate.

		/* get first cert from stack */
		X509 *cert = sk_X509_value(certs->chain,0);
		/* this must be the correct certificate, cannot return NOT_FOUND */
		result = ITK_NOT_VALID;
		/* calc sha */
		SHA1(srcData, srcDataLen, &sha[0]);
		/* verify digest signature was produced with this certificate */
		result = verifyDigestSig(cert, sha, SHA_DIGEST_LENGTH,
			sigData, sigDataLen, NID_sha1);

		return result;
	}

	/* parse the signature */
	sig = parseSigFile(sigData, sigDataLen);

	if(sig == NULL)
	{
		HxLOG_Error("ERROR\n");
		return ITK_NOT_VALID;
	}

	/* check certificate chains */
	while(certs)
	{
		/* check id */
		if (id == -1 || certs->id == id)
		{
			/* find certificate in chain that created the signature */
			for(i=0;i<sk_X509_num(certs->chain);i++)
			{
				/* get cert from stack */
				X509 *cert = sk_X509_value(certs->chain,i);
				/* compare cert issuer name against sig dir name and serial numbers */
				if (X509_NAME_cmp(getDirectoryName(sig), X509_get_issuer_name(cert)) == 0 &&
					ASN1_INTEGER_cmp(X509_get_serialNumber(cert),sig->authkeyid->serial) == 0)
				{
					/* this is the correct certificate, cannot return NOT_FOUND */
					result = ITK_NOT_VALID;
					/* validate digest of hash file for appropriate algorithm */
					switch (OBJ_obj2nid(sig->sig_alg))
					{
						case NID_md5:
						{
							HxLOG_Error("Warning: rejected MD5 signature, only SHA-1 supported.\n");
							break;
						}
						case NID_sha1:
						{
							unsigned char sha[SHA_DIGEST_LENGTH];
							/* calc sha */
							SHA1(srcData, srcDataLen, &sha[0]);
							/* verify digest signature was produced with this certificate */
							result = verifyDigestSig(cert, sha, SHA_DIGEST_LENGTH,
							sig->signature->data, sig->signature->length, NID_sha1);
							break;
						}
						default:
							break;
					}
					break;
				}
				/* if ok return now */
				if (result == ITK_VALID)
					return result;
			}
		}
		certs = certs->next;
	}

	if (result != ITK_VALID)
		HxLOG_Error("ERROR\n");

	AUTH_SIGNATURE_free(sig);

	return result;
}

static itk_validation_t verifyCertificate(struct itk_cert_mngr_t *thiz, void *cert, size_t certLength)
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	/**@todo assuming cert ptr is an X509_STORE_CTX for now */
	X509_STORE_CTX *ctx = (X509_STORE_CTX*)cert;
	cert_t *certs;
	int err,result=0;
	char data[256];
	UNUSED(certLength);
	/* get name for debug */
	X509_NAME_oneline(X509_get_issuer_name(ctx->cert), data, 256);
//	DBG(("Verify server cert \"%s\"\n",data));
//	DBG(("\t start date: %s", X509_get_notBefore(ctx->cert)));
//	DBG(("\t expire date: %s\n", X509_get_notAfter(ctx->cert)));

	/* verify server cert against certificate files loaded from the carousel */
	certs = cmngr->certs;
	while(certs)
	{
		/* set trusted CA certificate chain - specify which certs from the carousel to check against */
		X509_STORE_CTX_trusted_stack(ctx, certs->chain);
		/* verify the server cert against the trusted certs */
		result = X509_verify_cert(ctx);
		//DBG(("verify cert:  chain [%p] returned %d: %s\n",certs->chain,result,(result?"success":X509_verify_cert_error_string(ctx->error))));
		if( result == 1 )
		{
			// Certificate has verified on this chain, make sure any error codes
			// from previous chains are cleared, or they will cause curl to fail
			// the transaction
			X509_STORE_CTX_set_error( ctx, X509_V_OK );
			break;
		}
		/* this chain did not succeed, try the next one */
		certs = certs->next;
	}
	/* err? */
	if (result!=1)
	{
		err = X509_STORE_CTX_get_error(cert);
//		DBG(("Verify server cert \"%s\" failed: %d (%s)\n",data,err,X509_verify_cert_error_string(err)));
	}

	UNUSED(err);

	return (result==1 ? ITK_VALID : ITK_NOT_VALID);
}

static itk_validation_t checkDate(struct itk_cert_mngr_t *thiz, itk_int32_t id, itk_date_t date)
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	cert_t *cert;
	itk_validation_t validity=ITK_NOT_VALID;

#if	defined(CONFIG_DEBUG)
	if(cmngr)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(cmngr);
#endif


	cert = cmngr->certs;
	/* check certificate dates */
	while(cert)
	{
		/* check id */
		if (id == -1 || cert->id == id)
		{
			/* return invalid if date is older */
			if (date.year < cert->date.year ||
				date.month < cert->date.month ||
				date.day < cert->date.day ||
				date.hour < cert->date.hour ||
				date.minute < cert->date.minute ||
				date.second < cert->date.second)
			{
				HxLOG_Error("ERROR\n");
				return ITK_NOT_VALID;
			}
			else
				validity = ITK_VALID;//we have passed at least one
		}
		cert = cert->next;
	}
	return validity;
}

static void resetChain(struct itk_cert_mngr_t *thiz, itk_int32_t id)
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	cert_t *cert;

#if	defined(CONFIG_DEBUG)
	if(cmngr)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(cmngr);
#endif

	cert = cmngr->certs;
	/* check list for id */
	cert=cmngr->certs;
	while(cert)
	{
		if (cert->id==id || id==-1)
		{
			/* free existing cert chain */
			sk_X509_pop_free(cert->chain,X509_free);
			cert->chain = sk_X509_new_null();
			//sk_X509_zero(cert->chain);
			if (id!=-1)
				break;
		}
		cert = cert->next;
	}
}

static void release(struct itk_cert_mngr_t *thiz)
{
	certmngr_t *cmngr = (certmngr_t *)thiz;
	while(cmngr->certs)
	{
		remCert(cmngr,cmngr->certs);
	}

	cmngr->heap->free(cmngr->heap,cmngr);
}

static void sha1(itk_crypto_t * thiz, itk_present_t prefix, void *data, size_t dataLength, itk_uint8_t *digest)
{
	UNUSED(thiz);

#if	defined(CONFIG_DEBUG)
	if(digest)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(digest);
#endif

	if (prefix == ITK_NOT_PRESENT)
	{
		SHA1((const unsigned char *)data, dataLength, digest);
	}
	else
	{
		SHA_CTX shactx;
		struct {
			itk_uint32_t entry_type, length;
		} prefix_data;

		prefix_data.entry_type = HUTL_htonl(1);
		prefix_data.length = HUTL_htonl((itk_uint32_t)dataLength);
		SHA1_Init(&shactx);
		SHA1_Update(&shactx, &prefix_data, sizeof(prefix_data));
		SHA1_Update(&shactx, data, dataLength);
		SHA1_Final(digest, &shactx);
	}
}

/*
**	Base64 encoding routine.
*/
itk_errcode_t base64(struct itk_crypto_t * thiz, void *data, size_t dataLength, itk_uint8_t *result, size_t resultLength)
{
	BIO *bio = NULL, *b64 = NULL;
	BUF_MEM *bptr = NULL;
	UNUSED(thiz);

	HxLOG_Debug("+++\n");

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);//Do not add extra new lines
	bio = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bio);
	BIO_write(b64, data, (int)dataLength);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	if (bptr->length >= (int)resultLength)
	{
		BIO_free_all(b64);
		HxLOG_Error("ERROR\n");
		return ITKE_FAIL;
	}
	memcpy(result, bptr->data, bptr->length);
	result[bptr->length] = 0;

	BIO_free_all(b64);

	return ITKE_OK;
}

/*
 *					IMPORTANT
 * Manufacturer shall change this function to use their Freesat Private Key.
 * For details about the Private Key please refer freesat_extensions_draft_v0_7.pdf sec FS-13.10.9b.5.
 */
void privateKeySignature(struct itk_crypto_t * thiz, void *data, size_t dataLength, itk_uint8_t *signature, size_t sigLength)
{
	/* IMPORTANT:   Put the Private key here */
	itk_uint8_t *pri_key = rsa_PrivateKey;
	BIO* bio = NULL;
	UNUSED(thiz);

	/* BIO_new_mem_buf() creates a memory BIO using len bytes of data at buf,
	    if len is -1 then the buf is assumed to be null terminated and its length is determined by strlen. */
	if(NULL != (bio = BIO_new_mem_buf(pri_key, -1)))
	{
		RSA* rsa_key = NULL;
		if(NULL != (rsa_key = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL)))
		{
		  if ((int)sigLength >= RSA_size(rsa_key))
		  {
			  RSA_sign(NID_sha1, (unsigned char*)data, (unsigned int)dataLength, signature, (unsigned int *)&sigLength, rsa_key);
		  }
		  RSA_free(rsa_key);
		}
		BIO_free(bio);
	}
}

/*
 *					IMPORTANT
 * Manufacturer shall change this to represent their Freesat Private Key
 * Version Number. For details about the Private Key please refer
 * freesat_extensions_draft_v0_7.pdf sec FS-13.10.9b.5.
 */
itk_uint32_t privateKeyVersion(struct itk_crypto_t * thiz)
{
	UNUSED(thiz);
	return rsa_KeyVersion; /* Change this to private key version number */
}

/**Parses signature data
 */
static AUTH_SIGNATURE *parseSigFile(void *data, size_t dataLength)
{
	const unsigned char *p_in = data;
	AUTH_SIGNATURE *sig;
	HBOOL ok = TRUE;
	int hashtype;

	/* parse the file */
	sig = d2i_AUTH_SIGNATURE(NULL, &p_in, (long)dataLength);
	if (!sig)
	{
		ERR_load_crypto_strings();
		HxLOG_Error("[ERROR] Failed to parse signature file %s\n\r",ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}

	/* check the signature was read correctly */
	if (ok && p_in != (unsigned char *)data + dataLength)
	{
		HxLOG_Error("Signature file invalid\n\r");
		ok = FALSE;
	}
	if (ok && !sig->authkeyid->issuer)
	{
		HxLOG_Error("Signature issuer not found\n\r");
		ok = FALSE;
	}
	if (ok && !sig->authkeyid->serial)
	{
		HxLOG_Error("Signature serial not found\n\r");
		ok = FALSE;
	}
	hashtype = OBJ_obj2nid(sig->sig_alg);
	if (ok && hashtype != NID_md5 && hashtype != NID_sha1)
	{
		HxLOG_Error("Signature hash type unknown\n\r");
		ok = FALSE;
	}

	if (!ok)
	{
		AUTH_SIGNATURE_free(sig);
		sig = NULL;
	}
	return sig;
}

/**Returns signatures directory name field
 */
static void *getDirectoryName(AUTH_SIGNATURE *sig)
{
	int i;
	for (i=0; i<sk_GENERAL_NAME_num(sig->authkeyid->issuer); i++)
	{
		GENERAL_NAME *name = sk_GENERAL_NAME_value(sig->authkeyid->issuer, i);
		if (name->type == GEN_DIRNAME)
			return name->d.dirn;
	}
	return NULL;
}

/**Verifiies signature has been produced with certificate using the digest data
 */
static itk_validation_t verifyDigestSig(X509 *cert, void *digest,  size_t digestLength, void *sig, size_t sigLength, int digest_type)
{
	EVP_PKEY *pkey;
	RSA *rsa;
	int verified;
	itk_validation_t res;

	pkey = X509_get_pubkey(cert);
	if (!pkey)
	{
		HxLOG_Error("ERROR\n");
		return ITK_NOT_VALID;
	}
	rsa = EVP_PKEY_get1_RSA(pkey);
	if (!rsa)
	{
		HxLOG_Error("ERROR\n");
		return ITK_NOT_VALID;
	}
	/* verify */
	verified = RSA_verify(digest_type, digest, (unsigned int)digestLength, sig, (unsigned int)sigLength, rsa);
	if (verified != 1)
	{
#ifndef NDEBUG
		char errstr[256];
		ERR_error_string(ERR_get_error(), errstr);
		HxLOG_Error("[ERROR] RSA_verify failed: %s\n\r",errstr);
#endif
		res = ITK_NOT_VALID;
	}
	else
	{
		res = ITK_VALID;
	}

	// clean up
	RSA_free(rsa);
	EVP_PKEY_free(pkey);
    return res;

}

/**Create and add to list
 */
static cert_t *newCert(certmngr_t *certmgr)
{
	cert_t *cert, *certs = certmgr->certs;
	/* alloc */
	cert = certmgr->heap->zalloc(certmgr->heap, sizeof(cert_t), "cert_t");
	if (cert)
	{
		/* 1st one */
		if (certs == NULL)
			certmgr->certs = cert;
		else
		{
			/* add to end of list */
			while(certs->next)
				certs = certs->next;
			certs->next = cert;
		}
	}
	return cert;
}

/**Remove from list
 */
static void remCert(certmngr_t *certmgr, cert_t *cert)
{
	cert_t *certs = certmgr->certs,*prev=NULL;
	while(certs)
	{
		if (certs==cert)
		{
			/* assign next in list to previous */
			if (prev)
				prev->next = cert->next;
			/* next is new start of list */
			else
				certmgr->certs = cert->next;

			if (cert->chain)
				sk_X509_pop_free(cert->chain,X509_free);

			/* deallocate */
			certmgr->heap->free(certmgr->heap, cert);
			break;
		}
		prev = certs;
		certs = certs->next;
	}
}

static itk_cert_mngr_t *newCertificateManager(struct itk_crypto_t * thiz, itk_heap_t *heap)
{
	certmngr_t *cmngr = NULL;

#if	defined(CONFIG_DEBUG)
		if(heap)	{	HxLOG_Assert(1);	}
		else		{	HxLOG_Assert(0);	}
#else
		HxLOG_Assert(heap);
#endif

	/* alloc manager */
	cmngr = heap->zalloc(heap,sizeof(certmngr_t),"cert mngr");
	if (cmngr)
	{
		/* set api */
		cmngr->vtable.addCertificate = addCertificate;
		cmngr->vtable.verifySignature = verifySignature;
		cmngr->vtable.verifyCertificate = verifyCertificate;
		cmngr->vtable.checkDate = checkDate;
		cmngr->vtable.resetChain = resetChain;
		cmngr->vtable.release = release;
		cmngr->crypto = (itk_crypto_t *)thiz;
		cmngr->heap = heap;
	}

	return (itk_cert_mngr_t *)cmngr;
}
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


itk_crypto_t *mheg_itk_port_cryptoGetObject(void)
{
	itk_crypto_t	*pstCrypto = NULL;
	redkey_profile_t eItkProfile = s_mhegCryptoInfo.itkProfile;

	HxLOG_Error(C_CODE_F_WHITE_B_BLUE"Crypto Object Profile (%d) !!!"C_CODE_RESET"\n", eItkProfile);

	pstCrypto = HLIB_STD_MemAlloc(sizeof(itk_crypto_t));
	if (!pstCrypto)
	{
		HxLOG_Error("The Crypto object cannot create because not enough memory!!\r\n");
		return (itk_crypto_t*)NULL;
	}

	pstCrypto->sha1 = sha1;
	pstCrypto->base64 = base64;
	pstCrypto->privateKeySignature = privateKeySignature;
	pstCrypto->privateKeyVersion = privateKeyVersion;
	pstCrypto->newCertificateManager = newCertificateManager;
	pstCrypto->getAuthenticationCertificate = NULL; // TODO: 현재 사용되지 않음

	if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
		|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
		|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA) )
	{
		pstCrypto->base64 = NULL; // base64;
		pstCrypto->privateKeySignature = NULL; // privateKeySignature;
		pstCrypto->privateKeyVersion = NULL; // privateKeyVersion;
	}

	return (itk_crypto_t*)pstCrypto;
}

/** cleans up OpenSSL data, to be called on shutdown */
void itkCryptoCleanup(void) // not used
{
	BIO *bio_err=NULL;
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
	ERR_remove_state(0);
	EVP_cleanup();
	CRYPTO_mem_leaks(bio_err);
	if (bio_err != NULL)
	{
		BIO_free(bio_err);
		bio_err = NULL;
	}
}

#define _____PUBLIC_APIs________________________________________________________________


void PORT_ITK_Crypto_CreateObject(redkey_profile_t eItkProfile)
{
	s_mhegCryptoInfo.itkProfile = eItkProfile;
	s_mhegCryptoInfo.pObject = mheg_itk_port_cryptoGetObject();

	if (s_mhegCryptoInfo.pObject)
		ITK_registerManager(ITK_CRYPTO, s_mhegCryptoInfo.pObject);
}

#ifdef CONFIG_DEBUG
void PORT_ITK_Crypto_Debug_ShowSha1Status()
{
	unsigned char sha_1_digest[SHA_DIGEST_LENGTH];
	unsigned char sha_256_digest[SHA256_DIGEST_LENGTH];
	unsigned char a[] = "abc";
	unsigned char b[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	unsigned char c[] = "The quick brown fox jumps over the lazy dog";
	unsigned char d[] = "The quick brown fox jumps over the lazy dog.";
	unsigned char e[] = "";
	unsigned char result1[] = {0xA9,0x99,0x3E,0x36,0x47,0x06,0x81,0x6A,0xBA,0x3E,0x25,0x71,0x78,0x50,0xC2,0x6C,0x9C,0xD0,0xD8,0x9D};
	unsigned char result2[] = {0x84,0x98,0x3E,0x44,0x1C,0x3B,0xD2,0x6E,0xBA,0xAE,0x4A,0xA1,0xF9,0x51,0x29,0xE5,0xE5,0x46,0x70,0xF1};
	unsigned char result3[] = {0x2F,0xD4,0xE1,0xC6,0x7A,0x2D,0x28,0xFC,0xED,0x84,0x9E,0xE1,0xBB,0x76,0xE7,0x39,0x1B,0x93,0xEB,0x12};
	unsigned char result4[] = {0x40,0x8D,0x94,0x38,0x42,0x16,0xF8,0x90,0xFF,0x7A,0x0C,0x35,0x28,0xE8,0xBE,0xD1,0xE0,0xB0,0x16,0x21};
	unsigned char result5[] = {0xDA,0x39,0xA3,0xEE,0x5E,0x6B,0x4B,0x0D,0x32,0x55,0xBF,0xEF,0x95,0x60,0x18,0x90,0xAF,0xD8,0x07,0x09};
	unsigned char result6[] = {0xBA,0x78,0x16,0xBF,0x8F,0x01,0xCF,0xEA,0x41,0x41,0x40,0xDE,0x5D,0xAE,0x22,0x23,
		                        0xB0,0x03,0x61,0xA3,0x96,0x17,0x7A,0x9C,0xB4,0x10,0xFF,0x61,0xF2,0x00,0x15,0xAD};
	unsigned char result7[] = {0x24,0x8D,0x6A,0x61,0xD2,0x06,0x38,0xB8,0xE5,0xC0,0x26,0x93,0x0C,0x3E,0x60,0x39,
		                        0xA3,0x3C,0xE4,0x59,0x64,0xFF,0x21,0x67,0xF6,0xEC,0xED,0xD4,0x19,0xDB,0x06,0xC1};

	HxLOG_Debug("\n SHA_Test (SHA-1) Start... ");
	SHA1(a, 3, sha_1_digest);
	if (HxSTD_memcmp(sha_1_digest, result1, SHA_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	SHA1(b, 56, sha_1_digest);
	if (HxSTD_memcmp(sha_1_digest, result2, SHA_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	SHA1(c, 43, sha_1_digest);
	if (HxSTD_memcmp(sha_1_digest, result3, SHA_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	SHA1(d, 44, sha_1_digest);
	if (HxSTD_memcmp(sha_1_digest, result4, SHA_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	SHA1(e, 0, sha_1_digest);
	if (HxSTD_memcmp(sha_1_digest, result5, SHA_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	HxLOG_Debug("\n SHA_Test (SHA-1) End. \n");

	HxLOG_Debug("\n SHA_Test (SHA-256) Start... ");
	SHA256(a, 3, sha_256_digest);
	if (HxSTD_memcmp(sha_256_digest, result6, SHA256_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	SHA256(b, 56, sha_256_digest);
	if (HxSTD_memcmp(sha_256_digest, result7, SHA256_DIGEST_LENGTH) == 0)
		HxLOG_Debug(" OK"); // should be
	else
		HxLOG_Debug(" NOT OK !!!");
	HxLOG_Debug("\n SHA_Test (SHA-256) End. \n");
}


#endif /* CONFIG_DEBUG */


