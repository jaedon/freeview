/***************************************************************
* $Workfile:   ci_cc_cipher.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Content Control (CI+) Addon for
*        SHA-1, SHA-256,
*        RSASSA-PSS, RSASSA-PKCS1-V1_5,
*        AES-x-ECB, AES-x-CBC, AES-XCBC-MAC,
*        AES Based PRNG
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CC_CIPHER_H_
#define _CI_CC_CIPHER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include //////////////////////////////////////////////////////
***************************************************************/

#include "ci_plus.h"

/***************************************************************
* for self-test ////////////////////////////////////////////////
***************************************************************/

//#define SHA_TEST
//#define RSA_TEST
//	#define INCLUDE_BN_TEST
//	#define INCLUDE_MPI_TEST
//	#define INCLUDE_1ST_FORM_TEST
//	#define INCLUDE_10XX_BITS_TEST
//	#define INCLUDE_2048_BITS_TEST
//#define AES_TEST
//#define PRNG_TEST

/***************************************************************
* definition and typedef ///////////////////////////////////////
***************************************************************/

/*--- compile definitions ---*/

#define DEACTIVATE_NOT_USED_FUNCTIONS
#define GIVE_TASK_SLEEP_FOR_BIG_COMPUTATION
#define USE_MPI_LIB
#ifdef INCLUDE_MPI_TEST
	#ifndef USE_MPI_LIB
		#define USE_MPI_LIB
	#endif
#endif

/*--- big number for RSA ---*/

// 아래 MAX_BN_LEN 값은 처리해야 하는 RSA 입력 메시지와 RSA modulus n의 바이트 수를 기준으로 설정한다 (RSA의 다른 factor들은 이보다 작은 값이다).
// bn_pow() 연산을 예외로 하면 실질적으로 bn_mul() 연산에 가장 많은 공간이 필요하다 (입력값이 각각 P, Q 바이트일 때 P + Q 바이트의 공간이 필요함).
// MAX_BN_LEN 값이 충분히 크지 않다면 buffer overflow 를 야기할 수 있으므로 입력값에 주의를 요한다.
#define MAX_BN_LEN		1024

typedef int bn_len_type;
typedef signed int signed_bn_len_type;

typedef struct {
	bn_len_type		len;				// bignum length 는 최소 1 이상이어야 한다.
	unsigned char	num[MAX_BN_LEN];	// bignum a 가 0 이라는 것은 len = 1, num[0] = 0x00 임을 의미한다 (see bn_zero).
} bignum;								// 여기 구현된 bignum 라이브러리는 양수만을 취급함에 유의할 것 (see bn_sub()).

/*--- RSA key pair ---*/

// 기본적인 성질 : n = p x q
// valid RSA private key : for first form e x d ≡ 1 (mod λ(n)), or for second form e x dP ≡ 1 (mod (p - 1)) and e x dQ ≡ 1 (mod (q - 1))
typedef struct {
	bignum *n;		// RSA modulus, a positive integer, n = r1 x r2 x … x ru, u >= 2 (ri : prime factors of n, including r1 = p, r2 = q, and additional factors if any, u : number of prime factors of n, u >= 2)
	bignum *d;		// RSA private exponent, a positive integer
	bignum *p;		// first prime factor of n, a positive integer
	bignum *q;		// second prime factor of n, a positive integer
	bignum *dP;		// p's CRT exponent, a positive integer such that e x dP ≡ 1 (mod (p - 1))
	bignum *dQ;		// q's CRT exponent, a positive integer such that e x dQ ≡ 1 (mod (q - 1))
	bignum *qInv;	// CRT coefficient, a positive integer less than p such that q x qInv ≡ 1 (mod p)
} RSA_private_key;

typedef struct {
	bignum *n;		// RSA modulus, a positive integer, n = r1 x r2 x … x ru, u >= 2 (ri : prime factors of n, including r1 = p, r2 = q, and additional factors if any, u : number of prime factors of n, u >= 2)
	bignum *e;		// RSA public exponent, a positive integer between 3 and n - 1
} RSA_public_key;

/*--- AES-128 definitions for CC ---*/

#define CI_CC_CIPHER_BLOCK_SIZE		16	/* 128 bits (= Nb*4) for AES-x : For TS packet payload, max number of blocks is 11 for encryption with AES-128-CBC since residual blocks remain in the clear 184*8/128 is rounded to 11 */
#define CI_CC_CIPHER_BLOCK_SIZE_1	15
#define CI_CC_CIPHER_KEY_LEN			16	/* 128 bits (= Nk*4) for AES-128 */

/***************************************************************
* Pre-defined big numbers //////////////////////////////////////
***************************************************************/

extern bignum bn_zero;
extern bignum bn_one;
extern bignum bn_x10;
extern bignum bn_e;

/***************************************************************
* global function prototypes ///////////////////////////////////
***************************************************************/

// SHA-1 and SHA-256
void SHA_x(int x, unsigned char *Msg, unsigned long msgLen, unsigned char *digest);

// big number functions for RSA
void bn_init(bignum *a, unsigned char *c, bn_len_type len);
int bn_cmp(bignum *a, bignum *b);
void bn_add(bignum *a, bignum *b, bignum *out);
int bn_sub(bignum *a, bignum *b, bignum *out);
void bn_mul(bignum *a, bignum *b, bignum *out);
void bn_mod(bignum *a, bignum *b, bignum *out);
void bn_mod_exp(bignum *a, bignum *b, bignum *n, bignum *out);

// RSA (RSASSA-PSS + SHA-1 and RSASSA-PKCS1-V1_5 + SHA-256)
int RSASSA_PSS_SIGN(RSA_private_key *K, bignum *M, bignum *salt, bignum *S);
int RSASSA_PSS_VERIFY(RSA_public_key *K, bignum *M, bignum *S);
int RSASSA_PKCS1_V1_5_SIGN(RSA_private_key *K, bignum *M, bignum *S);
int RSASSA_PKCS1_V1_5_VERIFY(RSA_public_key *K, bignum *M, bignum *S);

// padding functions for AES
unsigned short GetAlignedLen(unsigned short usOrgLen);
void OneAndZeroesPadding(unsigned char *pucData, unsigned short usOrgLen);

// AES (AES-x-ECB and AES-x-CBC, x = 128, 192, 256)
void E_AES_x(int x, unsigned char *Key, unsigned char *PlainText, unsigned char *CipherText);
void D_AES_x(int x, unsigned char *Key, unsigned char *CipherText, unsigned char *PlainText);
void E_AES_x_CBC(int x, unsigned char *Key, unsigned char *PlainText, unsigned char *CipherText, unsigned short TextLen, unsigned char *IV);
void D_AES_x_CBC(int x, unsigned char *Key, unsigned char *CipherText, unsigned char *PlainText, unsigned short TextLen, unsigned char *IV);
void AES_XCBC_MAC(unsigned char Key[16], unsigned char *M, unsigned short Len, unsigned char MAC[16]);

// PRNG (AES Based)
void PRNG(unsigned char *S, unsigned char *R);

// adaptation definitions for CC
#define SHA_1(M,Len,D)						SHA_x(1,M,Len,D)
#define SHA_256(M,Len,D)					SHA_x(256,M,Len,D)
#define A_RSA_PSS(K,M,salt,S)				RSASSA_PSS_SIGN(K,M,salt,S)
#define V_RSA_PSS(K,M,S)					RSASSA_PSS_VERIFY(K,M,S)
#define A_RSA_PKCS1_V1_5(K,M,S)			RSASSA_PKCS1_V1_5_SIGN(K,M,S)
#define V_RSA_PKCS1_V1_5(K,M,S)			RSASSA_PKCS1_V1_5_VERIFY(K,M,S)
#define E_AES_ECB(K,P,C)					E_AES_x(128,K,P,C)
#define D_AES_ECB(K,C,P)					D_AES_x(128,K,C,P)
#define E_AES_CBC(K,P,C,Len,IV) 			E_AES_x_CBC(128,K,P,C,Len,IV)
#define D_AES_CBC(K,C,P,Len,IV) 			D_AES_x_CBC(128,K,C,P,Len,IV)
#define MAC_AES_XCBC(K,M,Len,MAC)		AES_XCBC_MAC(K,M,Len,MAC)


#ifdef __cplusplus
}
#endif

#endif /* _CI_CC_CIPHER_H_ */
