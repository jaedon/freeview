/*************************************************************
 * @file		_hlib_crypt.h
 * @date		2012/03/08
 * @author		ultracat
 * @brief		HUMAX common library header, related with crypto functon. (MD5 ...)

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXCRYPT_H_
#define HXCRYPT_H_

#include "htype.h"

typedef struct	_HxCryptInfo	HxCRYPT_Info_t;

extern HxCRYPT_Info_t*	HLIB_CRYPT_CreatePrivateKey(HCHAR *path);
extern HxCRYPT_Info_t*	HLIB_CRYPT_CreatePublicKey(HCHAR *path);
extern HERROR	HLIB_CRYPT_CloseKey(HxCRYPT_Info_t *handle);
extern HINT32	HLIB_CRYPT_CipherData(HxCRYPT_Info_t *handle, HUINT8 *pucData, HUINT8 **ppucOutBuffer, HINT32 nLen);
extern HINT32	HLIB_CRYPT_DeCipherData(HxCRYPT_Info_t *handle, HUINT8 *pucData, HUINT8 **ppucOutBuffer, HINT32 nLen);

#endif // #ifndef HXCRYPT_H_
