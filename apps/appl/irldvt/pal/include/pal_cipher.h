/***************************************************************
* $Workfile:   pal_cipher.h  $
* $Modtime:
* Auther : hmkim
*
* Desc :
*        AES-x-ECB, AES-x-CBC, AES-XCBC-MAC,
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _PAL_CIPHER_H_
#define _PAL_CIPHER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include //////////////////////////////////////////////////////
***************************************************************/
#include "htype.h"

/***************************************************************
* definition and typedef ///////////////////////////////////////
***************************************************************/

/*--- AES-128 definitions for CC ---*/

#define PAL_CIPHER_BLOCK_SIZE		16	/* 128 bits (= Nb*4) for AES-x : For TS packet payload, max number of blocks is 11 for encryption with AES-128-CBC since residual blocks remain in the clear 184*8/128 is rounded to 11 */
#define PAL_CIPHER_BLOCK_SIZE_1		15


/*--- big number for RSA ---*/

// 아래 MAX_BN_LEN 값은 처리해야 하는 RSA 입력 메시지와 RSA modulus n의 바이트 수를 기준으로 설정한다 (RSA의 다른 factor들은 이보다 작은 값이다).
// bn_pow() 연산을 예외로 하면 실질적으로 bn_mul() 연산에 가장 많은 공간이 필요하다 (입력값이 각각 P, Q 바이트일 때 P + Q 바이트의 공간이 필요함).
// MAX_BN_LEN 값이 충분히 크지 않다면 buffer overflow 를 야기할 수 있으므로 입력값에 주의를 요한다.
#define MAX_BN_LEN		1024


typedef int 		bnLenType;
typedef signed int 	signedBnLenType;

typedef struct {
	bnLenType		len;				// bignum length 는 최소 1 이상이어야 한다.
	unsigned char	num[MAX_BN_LEN];	// bignum a 가 0 이라는 것은 len = 1, num[0] = 0x00 임을 의미한다 (see bn_zero).
} PalCipher_BigNumber_e;

// 기본적인 성질 : n = p x q
// valid RSA private key : for first form e x d ≡ 1 (mod λ(n)), or for second form e x dP ≡ 1 (mod (p - 1)) and e x dQ ≡ 1 (mod (q - 1))
typedef struct {
	PalCipher_BigNumber_e *n;		// RSA modulus, a positive integer, n = r1 x r2 x … x ru, u >= 2 (ri : prime factors of n, including r1 = p, r2 = q, and additional factors if any, u : number of prime factors of n, u >= 2)
	PalCipher_BigNumber_e *d;		// RSA private exponent, a positive integer
	PalCipher_BigNumber_e *p;		// first prime factor of n, a positive integer
	PalCipher_BigNumber_e *q;		// second prime factor of n, a positive integer
	PalCipher_BigNumber_e *dP; 	// p's CRT exponent, a positive integer such that e x dP ≡ 1 (mod (p - 1))
	PalCipher_BigNumber_e *dQ; 	// q's CRT exponent, a positive integer such that e x dQ ≡ 1 (mod (q - 1))
	PalCipher_BigNumber_e *qInv;	// CRT coefficient, a positive integer less than p such that q x qInv ≡ 1 (mod p)
} PalCipher_RsaPrivateKey_e;

typedef struct {
	PalCipher_BigNumber_e *n;		// RSA modulus, a positive integer, n = r1 x r2 x … x ru, u >= 2 (ri : prime factors of n, including r1 = p, r2 = q, and additional factors if any, u : number of prime factors of n, u >= 2)
	PalCipher_BigNumber_e *e;		// RSA public exponent, a positive integer between 3 and n - 1
} PalCipher_RsaPublicKey_e;

typedef enum
{
	eHashingAlg_MD2, 		/* currently not supported */
	eHashingAlg_MD5,
	eHashingAlg_SHA_1, 		/* currently not supported. see ci_cc_cipher.c */
	eHashingAlg_SHA_256, 	/* currently not supported. see ci_cc_cipher.c */
	eHashingAlg_SHA_384, 	/* currently not supported. see ci_cc_cipher.c */
	eHashingAlg_SHA_512 	/* currently not supported. see ci_cc_cipher.c */
} PalCipher_HashingAlg_e;

/***************************************************************
* global function prototypes ///////////////////////////////////
***************************************************************/

void PAL_CIPHER_EncryptAesCbc(int x, unsigned char *Key, unsigned char *PlainText, unsigned char *CipherText, unsigned short TextLen, unsigned char *IV);
void PAL_CIPHER_DecryptAesCbc(int x, unsigned char *Key, unsigned char *CipherText, unsigned char *PlainText, unsigned short TextLen, unsigned char *IV);

#define PAL_ENC_AES_CBC(K,P,C,Len,IV) 		PAL_CIPHER_EncryptAesCbc(128,K,P,C,Len,IV)
#define PAL_DEC_AES_CBC(K,C,P,Len,IV) 		PAL_CIPHER_DecryptAesCbc(128,K,C,P,Len,IV)


HERROR	PAL_CIPHER_VerifyRsaSsaPkcs1V15(PalCipher_RsaPublicKey_e *K, PalCipher_BigNumber_e *M, PalCipher_BigNumber_e *S, PalCipher_HashingAlg_e eHashAlg);
void 	PAL_CIPHER_MD5(unsigned char *message, unsigned long length, char *result);


void PAL_CIPHER_SHA_x(int x, unsigned char *Msg, unsigned long msgLen, unsigned char *digest);
#define PAL_CIPHER_SHA_1(M,Len,D)			PAL_CIPHER_SHA_x(1,M,Len,D)
#define PAL_CIPHER_SHA_256(M,Len,D)			PAL_CIPHER_SHA_x(256,M,Len,D)

HERROR PAL_CHIPER_HMAC_SHA1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_CIPHER_H_ */
