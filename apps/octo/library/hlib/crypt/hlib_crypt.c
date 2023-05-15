//	사용하지 않음.
//	사용할 경우, hlib_encrypt의 dynamic loading부분 merge필요.
#if 0
/*
 * hxcrypt.c
 *
 *  Created on: 2012. 03. 08.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <_hlib_log.h>
#include <_hlib_stdlib.h>
#include <_hlib_mem.h>
#include <_hlib_rwstream.h>
#include <_hlib_crypt.h>

#undef HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"hxcrypt"

#define	HxCRYPT_PUBKEY_PATH		"/tmp/hlibpub.pem"

#define	RSA_KEY_PRIVATE			0
#define	RSA_KEY_PUBLIC			1

typedef struct _HxCryptInfo
{
	HCHAR		*path;
	HUINT32		keyType;

	EVP_PKEY	*key;
} _HxCryptInfo_t;

/**
	해당 Path에 공개 키를 생성하고..
	Return값으로 Private Key를 돌려준다
 */
HxCRYPT_Info_t*	HLIB_CRYPT_CreatePrivateKey(HCHAR *path)
{
	FILE	*fPub;
	RSA		*rsaPriv = NULL, *rsaPub = NULL;
	HUINT8	seedBuf[128], *seed = seedBuf;

	HxCRYPT_Info_t	*cryptInfo;

	*seed = HLIB_STD_GetSystemTick();
	RAND_seed(seedBuf, 8);

	rsaPriv = RSA_generate_key(512, RSA_F4, NULL, NULL);
	if (rsaPriv == NULL)
	{
		HxLOG_Critical("RSA Generate KEY error!!\n");

		return (HxCRYPT_Info_t*)NULL;
	}
	rsaPub  = RSAPublicKey_dup(rsaPriv);
	if (rsaPub == NULL)
	{
		HxLOG_Critical("RSA Public KEY copy error!!\n");
		RSA_free(rsaPriv);

		return (HxCRYPT_Info_t*)NULL;
	}

	if (path == NULL)
		path = (HCHAR*)HxCRYPT_PUBKEY_PATH;

	fPub = fopen(path, "w");
	if(fPub)
	{
		if (!PEM_write_RSAPublicKey(fPub, rsaPub))
		{
			HxLOG_Critical("Fail to make public key file : [%s]\n", path);
			RSA_free(rsaPriv);
			RSA_free(rsaPub);

			return (HxCRYPT_Info_t*)NULL;
		}
		fclose(fPub);
	}

	cryptInfo = (HxCRYPT_Info_t*)HLIB_MEM_Calloc(sizeof(_HxCryptInfo_t));
	cryptInfo->path = HLIB_MEM_StrDup_CB(path);
	cryptInfo->keyType = RSA_KEY_PRIVATE;
	cryptInfo->key       = EVP_PKEY_new();
	if (!EVP_PKEY_assign_RSA(cryptInfo->key, rsaPriv))
	{
		HxLOG_Critical("PKEY Assign Failed!!!\n");
	}

	RSA_free(rsaPub);

	return cryptInfo;
}

HxCRYPT_Info_t*	HLIB_CRYPT_CreatePublicKey(HCHAR *path)
{
	FILE	*fPub;
	RSA		*rsaPub;

	HxCRYPT_Info_t	*cryptInfo;

	if (path == NULL)
		path = HxCRYPT_PUBKEY_PATH;

	fPub = fopen(path, "r");
	if (fPub == NULL)
	{
		HxLOG_Critical("Can't find public key : [%s]\n", path);
		return NULL;
	}

	rsaPub = PEM_read_RSAPublicKey(fPub, NULL, NULL, NULL);
	if (rsaPub == NULL)
	{
		HxLOG_Critical("Can't read RSA Public Key..\n");
		return NULL;
	}

	fclose(fPub);

	cryptInfo = (HxCRYPT_Info_t*)HLIB_MEM_Malloc(sizeof(HxCRYPT_Info_t));
	cryptInfo->path = HLIB_MEM_StrDup_CB(path);
	cryptInfo->keyType = RSA_KEY_PUBLIC;
	cryptInfo->key     = EVP_PKEY_new();
	if (!EVP_PKEY_assign_RSA(cryptInfo->key, rsaPub))
	{
		HxLOG_Critical("PKEY Assign Failed!!!\n");
	}

	return cryptInfo;
}

HERROR	HLIB_CRYPT_CloseKey(HxCRYPT_Info_t *handle)
{
	HxLOG_Assert(handle);

	if (handle->keyType == RSA_KEY_PRIVATE)
		unlink(handle->path);

    EVP_PKEY_free(handle->key);
	HLIB_MEM_Free(handle->path);
	HLIB_MEM_Free(handle);

	return ERR_OK;
}

HINT32	HLIB_CRYPT_CipherData(HxCRYPT_Info_t *handle, HUINT8 *pucData, HUINT8 **ppucOutBuffer, HINT32 nLen)
{
	HINT32			inFile, outFile;

	EVP_PKEY		*pkey;
	EVP_CIPHER_CTX	ctx;
	HUINT8			buffer[4096];
	HUINT8			buffer_out[4096 + EVP_MAX_IV_LENGTH];
	HINT32			len;
	HINT32			len_out;
	HUINT8			*ek = NULL;
	HINT32			ekLen;
	HUINT32			ekLen_n;
	HUINT8			iv[EVP_MAX_IV_LENGTH];

	inFile = HLIB_RWSTREAM_Open(NULL, 0);
	outFile = HLIB_RWSTREAM_Open(NULL, 0);

	HLIB_RWSTREAM_Write(inFile, pucData, nLen);
	HLIB_RWSTREAM_Rewind(inFile);

	nLen = 0;
	pkey = handle->key;

	EVP_CIPHER_CTX_init(&ctx);
	ek = HLIB_MEM_Malloc(EVP_PKEY_size(pkey));

	if (!EVP_SealInit(&ctx, EVP_aes_128_cbc(), &ek, &ekLen, iv, &pkey, 1))
	{
		HxLOG_Critical("EVP_SealInit: failed.\n");
		goto out_free;
	}

	ekLen_n = htonl(ekLen);
	HLIB_RWSTREAM_Write(outFile, &ekLen_n, sizeof ekLen_n);
	HLIB_RWSTREAM_Write(outFile, ek, ekLen);
	HLIB_RWSTREAM_Write(outFile, iv, EVP_CIPHER_iv_length(EVP_aes_128_cbc()));

	while ((len = HLIB_RWSTREAM_Read(inFile, buffer, sizeof buffer)) > 0)
	{
		if (!EVP_SealUpdate(&ctx, buffer_out, &len_out, buffer, len))
		{
			HxLOG_Critical("EVP_SealUpdate : failed.\n");
			goto out_free;
		}

		if (HLIB_RWSTREAM_Write(outFile, buffer_out, len_out) == 0)
		{
			HxLOG_Critical("Error on output..\n");
			goto out_free;
		}
	}

	if (!EVP_SealFinal(&ctx, buffer_out, &len_out))
	{
		HxLOG_Critical("EVP_SealFinal: failed.\n");
		goto out_free;
	}

	if (HLIB_RWSTREAM_Write(outFile, buffer_out, len_out) == 0)
	{
		perror("output file");
		goto out_free;
	}

	out_free:
	HLIB_MEM_Free(ek);

	HLIB_RWSTREAM_Close(inFile);
	nLen = HLIB_RWSTREAM_GetOffset(outFile);
	*ppucOutBuffer = HLIB_RWSTREAM_CloseWithoutBuf(outFile);

	return nLen;
}

HINT32	HLIB_CRYPT_DeCipherData(HxCRYPT_Info_t *handle, HUINT8 *pucData, HUINT8 **ppucOutBuffer, HINT32 nLen)
{
	HINT32			inFile, outFile;

	EVP_PKEY		*pkey;
	EVP_CIPHER_CTX	ctx;
	HUINT8			buffer[4096];
	HUINT8			buffer_out[4096 + EVP_MAX_IV_LENGTH];
	HINT32			len;
	HINT32			len_out;
	HUINT8			*ek;
	HUINT32			eklen;
	HUINT32			eklen_n;
	HUINT8			iv[EVP_MAX_IV_LENGTH];

	inFile = HLIB_RWSTREAM_Open(NULL, 0);
	outFile = HLIB_RWSTREAM_Open(NULL, 0);

	HLIB_RWSTREAM_Write(inFile, pucData, nLen);
	HLIB_RWSTREAM_Rewind(inFile);

	nLen = 0;
	pkey = handle->key;

	EVP_CIPHER_CTX_init(&ctx);
	ek = HLIB_MEM_Malloc(EVP_PKEY_size(pkey));

	if (HLIB_RWSTREAM_Read(inFile, &eklen_n, sizeof eklen_n) == 0)
	{
		HxLOG_Critical("Input Error.\n");
		goto out_free;
	}

	eklen = ntohl(eklen_n);
	if (eklen > EVP_PKEY_size(pkey))
	{
		HxLOG_Critical("Bad encrypted key length (%u > %d)\n", eklen, EVP_PKEY_size(pkey));
		goto out_free;
	}
	if (HLIB_RWSTREAM_Read(inFile, ek, eklen) == 0)
	{
		HxLOG_Critical("input file");
		goto out_free;
	}
	if (HLIB_RWSTREAM_Read(inFile, iv, EVP_CIPHER_iv_length(EVP_aes_128_cbc())) == 0)
	{
		HxLOG_Critical("input file");
		goto out_free;
	}

	if (!EVP_OpenInit(&ctx, EVP_aes_128_cbc(), ek, eklen, iv, pkey))
	{
		HxLOG_Critical("EVP_OpenInit: failed.\n");
		goto out_free;
	}

	while ((len = HLIB_RWSTREAM_Read(inFile, buffer, sizeof buffer)) > 0)
	{
		if (!EVP_OpenUpdate(&ctx, buffer_out, &len_out, buffer, len))
		{
			HxLOG_Critical("EVP_OpenUpdate: failed.\n");
			goto out_free;
		}
		if (HLIB_RWSTREAM_Write(outFile, buffer_out, len_out) == 0)
		{
			HxLOG_Critical("output file");
			goto out_free;
		}
	}

	if (!EVP_OpenFinal(&ctx, buffer_out, &len_out))
	{
		HxLOG_Critical("EVP_SealFinal: failed.\n");
		goto out_free;
	}

	if (len_out)
	{
		if (HLIB_RWSTREAM_Write(outFile, buffer_out, len_out) == 0)
		{
			HxLOG_Critical("output file");

			goto out_free;
		}
	}

	out_free:
	HLIB_MEM_Free(ek);
	HLIB_RWSTREAM_Close(inFile);
	nLen = HLIB_RWSTREAM_GetOffset(outFile);
	*ppucOutBuffer = HLIB_RWSTREAM_CloseWithoutBuf(outFile);

	return nLen;
}

#endif

