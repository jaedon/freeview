/*************************************************************
 * @file		_hlib_aes.h
 * @date		2013/05/01
 * @author		ultracat
 * @brief		HUMAX common library header, related with crypto functon. (MD5 ...)

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXAES_H_
#define HXAES_H_

#include "htype.h"

typedef enum
{
	HLIB_CRYPT_AES_128		= 0,
	HLIB_CRYPT_AES_192,
	HLIB_CRYPT_AES_256,
	HLIB_CRYPT_MAX_NUM_AES_X,
} HLIB_CRYPT_AES_X;

extern void		HLIB_CRYPT_EncryptAesCbc(HLIB_CRYPT_AES_X eAesX, const HUINT8 *szKey, const HUINT8 *szPlainText, HUINT8 *szCipherText, HUINT16 TextLen, HUINT8 *IV);
extern void		HLIB_CRYPT_DecryptAesCbc(HLIB_CRYPT_AES_X eAesX, const HUINT8 *szKey, const HUINT8 *szCipherText, HUINT8 *szPlainText, HUINT16 TextLen, HUINT8 *IV);
extern void     HLIB_CRYPT_DecryptAesECB(HLIB_CRYPT_AES_X eAesX, const HUINT8 *key ,const HUINT8 *input, HUINT8 *output);
#define HLIB_CRYPT_ENC_AES_CBC(K,P,C,Len,IV) 		HLIB_CRYPT_EncryptAesCbc(HLIB_CRYPT_AES_128,K,P,C,Len,IV)
#define HLIB_CRYPT_DEC_AES_CBC(K,C,P,Len,IV) 		HLIB_CRYPT_DecryptAesCbc(HLIB_CRYPT_AES_128,K,C,P,Len,IV)
#define HLIB_CRYPT_DEC_AES_ECB(K,I,O) 		        HLIB_CRYPT_DecryptAesECB(HLIB_CRYPT_AES_128,K,I,O)



#endif // #ifndef HXAES_H_
