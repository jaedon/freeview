/*************************************************************
 * @file		_hlib_encrypt.h
 * @date		2012/03/08
 * @author		ultracat
 * @brief		HUMAX common library header, related with crypto functon. (MD5 ...)

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXENCRYPT_H_
#define HXENCRYPT_H_

#include "htype.h"

typedef enum
{
	HxCRYPT_DES,
	HxCRYPT_3DES,
	HxCRYPT_AES		// NOT IMPLEMENTED YET
} HxCryptType_e;

typedef struct
{
	HxCryptType_e	type;
	union {
		HUINT8		DES[8];
		HUINT8		DES3[24];
		HUINT8		AES[16];
	} u;
} HxCryptKey_t;

extern  HERROR  HLIB_CRYPT_Init (void);
extern	HINT32	HLIB_CRYPT_DES_EncryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen);
extern	HINT32	HLIB_CRYPT_DES_DecryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen);
extern	HINT32	HLIB_CRYPT_DES3_EncryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen);
extern	HINT32	HLIB_CRYPT_DES3_DecryptData(char *Key, HUINT8 *pucData, HUINT8 *pucOutBuffer, HINT32 nLen);

#endif // #ifndef HXENCRYPT_H_
