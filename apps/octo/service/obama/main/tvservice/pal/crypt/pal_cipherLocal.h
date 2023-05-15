/***************************************************************
* $Workfile:   pal_cipherLocal.h  $
* $Modtime:
* Auther : 
*
* Desc :
*        AES-x-ECB, AES-x-CBC, AES-XCBC-MAC,
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _PAL_CIPHER_LOCAL_H_
#define _PAL_CIPHER_LOCAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include //////////////////////////////////////////////////////
***************************************************************/
#include "pal_cipher.h"

/***************************************************************
* definition and typedef ///////////////////////////////////////
***************************************************************/
#define PAL_CIPHER_DEACTIVATE_NOT_USED_FUNCTIONS
#define PAL_CIPHER_GIVE_TASK_SLEEP_FOR_BIG_COMPUTATION
#define PAL_CIPHER_USE_MPI_LIB

#ifdef PAL_CIPHER_INCLUDE_MPI_TEST
	#ifndef PAL_CIPHER_USE_MPI_LIB
		#define PAL_CIPHER_USE_MPI_LIB
	#endif
#endif

/*--- MD5  ---*/

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context. */
typedef struct {
	UINT4 state[4];				/* state (ABCD) */
	UINT4 count[2];				/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} MD5_CTX;


/***************************************************************
* global function prototypes ///////////////////////////////////
***************************************************************/


int 	pal_cipher_bn_cmp(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b);
void 	pal_cipher_bn_mod(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out);
void 	pal_cipher_bn_init(PalCipher_BigNumber_e *a, unsigned char *c, bnLenType len);
void 	pal_cipher_bn_add(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out);
int 	pal_cipher_bn_sub(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out);
void 	pal_cipher_bn_mul(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out);
void 	pal_cipher_bn_mod_exp(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *n, PalCipher_BigNumber_e *out);


#ifdef INCLUDE_BN_TEST
void 	pal_cipher_BN_Test(void);
#endif

void 	pal_cipher_MD5Init (MD5_CTX *context);
void	pal_cipher_MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
void 	pal_cipher_MD5Final(unsigned char digest[16], MD5_CTX *context);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_CIPHER_H_ */
