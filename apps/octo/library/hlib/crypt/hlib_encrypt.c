/*
 * hxcrypt.c
 *
 *  Created on: 2012. 07. 05.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <dlfcn.h>

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/des.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <_hlib_log.h>
#include <_hlib_stdlib.h>
#include <_hlib_mem.h>
#include <_hlib_rwstream.h>
#include <_hlib_encrypt.h>

typedef struct
{
	HUINT32		ulSemId;			//	Semaphore가 필요없을것 같지만 나중을 대비해서 우선 만들어 놓음.

	void		*pvCryptoSo;
	void		(*_DES_set_odd_parity)(DES_cblock *key);
	void		(*_DES_set_key_checked)(const_DES_cblock *key, DES_key_schedule *schedule);
	void		(*_DES_cfb64_encrypt)(const unsigned char *in, unsigned char *out, long length, DES_key_schedule *schedule, DES_cblock *ivec, int *num, int enc);
	void		(*_DES_ede3_cfb64_encrypt)(const unsigned char *in, unsigned char *out, long length, DES_key_schedule *ks1, DES_key_schedule *ks2, DES_key_schedule *ks3, DES_cblock *ivec, int *num, int enc);
} openssl_t;

static openssl_t		*s_pstOpenSSL = NULL;
#define	openssl(func)	s_pstOpenSSL->_##func

#if defined(CONFIG_MW_CAS_NAGRA)

#endif
#if defined(CONFIG_MW_CAS_IRDETO)

#endif
#if defined(CONFIG_MW_CAS_CONAX)

#endif
#if defined(CONFIG_MW_CAS_VIACCESS)

#endif

static HERROR	hlib_crypt_InitLibrary(void)
{
	if (s_pstOpenSSL == NULL)
	{
		s_pstOpenSSL = HLIB_MEM_Calloc(sizeof(openssl_t));

		s_pstOpenSSL->pvCryptoSo = dlopen("/usr/lib/libcrypto.so", RTLD_LAZY);
		if (s_pstOpenSSL->pvCryptoSo == NULL)
		{
			HxLOG_Critical("Can't find /usr/lib/libcrypto.so\n");
			HxLOG_Assert(0);
			return ERR_FAIL;
		}

		s_pstOpenSSL->_DES_set_odd_parity = dlsym(s_pstOpenSSL->pvCryptoSo, "DES_set_odd_parity");
		s_pstOpenSSL->_DES_set_key_checked = dlsym(s_pstOpenSSL->pvCryptoSo, "DES_set_key_checked");
		s_pstOpenSSL->_DES_cfb64_encrypt = dlsym(s_pstOpenSSL->pvCryptoSo, "DES_cfb64_encrypt");
		s_pstOpenSSL->_DES_ede3_cfb64_encrypt = dlsym(s_pstOpenSSL->pvCryptoSo, "DES_ede3_cfb64_encrypt");

		HxLOG_Assert(s_pstOpenSSL->_DES_set_odd_parity && s_pstOpenSSL->_DES_set_key_checked && s_pstOpenSSL->_DES_cfb64_encrypt && s_pstOpenSSL->_DES_ede3_cfb64_encrypt);
		HxLOG_Warning("libCrypto dynamic loading is done...\n");
	}

	return ERR_OK;
}

HERROR HLIB_CRYPT_Init (void)
{
	return hlib_crypt_InitLibrary();
}

HINT32	HLIB_CRYPT_DES_EncryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen)
{
	//static char*    Res;
	int             n=0;
	DES_cblock      Key2;
	DES_key_schedule schedule;

	hlib_crypt_InitLibrary();

	/* Prepare the key for use with DES_cfb64_encrypt */
	memcpy( Key2, Key,8);
	openssl(DES_set_odd_parity( &Key2 ));
	openssl(DES_set_key_checked( &Key2, &schedule ));

	/* Encryption occurs here */
	openssl(DES_cfb64_encrypt( ( unsigned char * ) pucData, ( unsigned char * ) pucOutBuffer,
	               	nLen, &schedule, &Key2, &n, DES_ENCRYPT ));

	return ERR_OK;
}

HINT32	HLIB_CRYPT_DES_DecryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen)
{
//	static char*    Res;
	int             n=0;

	DES_cblock      Key2;
	DES_key_schedule schedule;

	hlib_crypt_InitLibrary();

	/* Prepare the key for use with DES_cfb64_encrypt */
	memcpy( Key2, Key,8);
	openssl(DES_set_odd_parity( &Key2 ));
	openssl(DES_set_key_checked( &Key2, &schedule ));

	/* Decryption occurs here */
	openssl(DES_cfb64_encrypt( ( unsigned char * ) pucData, ( unsigned char * ) pucOutBuffer,
	                   nLen, &schedule, &Key2, &n, DES_DECRYPT ));

	return ERR_OK;
}

HINT32	HLIB_CRYPT_DES3_EncryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen)
{
	//static char*    Res;
	int             n=0;
	DES_cblock      Key1, Key2, Key3;
	DES_key_schedule schedule1, schedule2,schedule3;

	hlib_crypt_InitLibrary();

	/* Prepare the key for use with DES_cfb64_encrypt */
	memcpy( Key1, Key,8);
	memcpy( Key2, &Key[8],8);
	memcpy( Key3, &Key[16],8);

	openssl(DES_set_odd_parity( &Key1 ));
	openssl(DES_set_key_checked( &Key1, &schedule1 ));
	openssl(DES_set_odd_parity( &Key2 ));
	openssl(DES_set_key_checked( &Key2, &schedule2 ));
	openssl(DES_set_odd_parity( &Key3 ));
	openssl(DES_set_key_checked( &Key3, &schedule3 ));

	/* Encryption occurs here */
	openssl(DES_ede3_cfb64_encrypt( ( unsigned char * ) pucData, ( unsigned char * ) pucOutBuffer,
	               	nLen, &schedule1, &schedule2, &schedule3 ,&Key1, &n, DES_ENCRYPT ));

	return ERR_OK;
}

HINT32	HLIB_CRYPT_DES3_DecryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen)
{
//	static char*    Res;
	int             n=0;

	DES_cblock      Key1, Key2, Key3;
	DES_key_schedule schedule1, schedule2,schedule3;

	hlib_crypt_InitLibrary();

	/* Prepare the key for use with DES_cfb64_encrypt */
	memcpy( Key1, Key,8);
	memcpy( Key2, &Key[8],8);
	memcpy( Key3, &Key[16],8);

	openssl(DES_set_odd_parity( &Key1 ));
	openssl(DES_set_key_checked( &Key1, &schedule1 ));
	openssl(DES_set_odd_parity( &Key2 ));
	openssl(DES_set_key_checked( &Key2, &schedule2 ));
	openssl(DES_set_odd_parity( &Key3 ));
	openssl(DES_set_key_checked( &Key3, &schedule3 ));

	/* Decryption occurs here */
	openssl(DES_ede3_cfb64_encrypt( ( unsigned char * ) pucData, ( unsigned char * ) pucOutBuffer,
	                   nLen, &schedule1, &schedule2, &schedule3 ,&Key1, &n, DES_DECRYPT ));

	return ERR_OK;
}


