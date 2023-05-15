/***************************************************************
* $Workfile:   pal_cipher_.c  $
* $Modtime:
* Auther : hmkim
*
* Desc :
*        AES-x-ECB, AES-x-CBC, AES-XCBC-MAC,
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/


/***************************************************************
* #ifdef statements and include ////////////////////////////////
***************************************************************/
#include "octo_common.h"
#include "pal_cipher.h"
#include "pal_cipherLocal.h"


/***************************************************************
* definition ///////////////////////////////////////////////////
***************************************************************/



#ifdef CONFIG_DEBUG
#define CHECK_RSASSA_PKCS1_V1_5_VERIFY		1
#define CHECK_EMSA_PKCS1_v1_5_ENCODE		1
#endif

#define MAX_DIGEST_SIZE			128
#define MD5_HASH_VALUE_LEN		18


#define __________HMAC__________
/***************************************************************
* HMAC /////////////////////////////////////////////////////////
***************************************************************/
#define	HMAC_PAD_SIZE		64 // for MD5 and SHA-1 (see SHA_BLOCK_SIZE)

// length of pucHmacResult should be SHA_1_DIGEST_SIZE
HERROR PAL_CHIPER_HMAC_SHA1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult)
{
	int i;

	HUINT8	ucTempKey[HMAC_PAD_SIZE];

	HUINT8	ucOkeyPad[HMAC_PAD_SIZE];
	HUINT8	ucIkeyPad[HMAC_PAD_SIZE];

	HUINT8	*pucIkeyPadPlusMsg = NULL;
	HUINT8	ucHashForIkeyPadPlusMsg[SHA_1_DIGEST_SIZE];
	HUINT8	ucOkeyPadPlusHash[HMAC_PAD_SIZE + SHA_1_DIGEST_SIZE];


	if(pucMsg == NULL)
	{
		HxLOG_Print("pucMsg NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(ucTempKey, 0x00, HMAC_PAD_SIZE);

	if(ucKeySize > HMAC_PAD_SIZE)
	{
		PAL_CIPHER_SHA_1(pucKey, ucKeySize, ucTempKey);
	}
	else
	{
		HxSTD_memcpy(ucTempKey, pucKey, ucKeySize);
	}

	HxSTD_memset(ucOkeyPad, 0x5c, HMAC_PAD_SIZE);
	HxSTD_memset(ucIkeyPad, 0x36, HMAC_PAD_SIZE);

	// i key & o key pad
	for(i = 0 ; i < HMAC_PAD_SIZE ; i++)
	{
		ucOkeyPad[i] ^= ucTempKey[i];
		ucIkeyPad[i] ^= ucTempKey[i];
	}

	pucIkeyPadPlusMsg = OxPAL_Malloc(HMAC_PAD_SIZE + ulMsgSize);
	if(pucIkeyPadPlusMsg == NULL)
	{
		HxLOG_Print("OxPAL_Malloc fail\n");
		return ERR_FAIL;
	}

	// 1pass
	HxSTD_memcpy(pucIkeyPadPlusMsg, ucIkeyPad, HMAC_PAD_SIZE);
	HxSTD_memcpy(pucIkeyPadPlusMsg + HMAC_PAD_SIZE, pucMsg, ulMsgSize);
	PAL_CIPHER_SHA_1(pucIkeyPadPlusMsg, HMAC_PAD_SIZE + ulMsgSize, ucHashForIkeyPadPlusMsg);

	// 2pass
	HxSTD_memcpy(ucOkeyPadPlusHash, ucOkeyPad, HMAC_PAD_SIZE);
	HxSTD_memcpy(ucOkeyPadPlusHash + HMAC_PAD_SIZE, ucHashForIkeyPadPlusMsg, SHA_1_DIGEST_SIZE);
	PAL_CIPHER_SHA_1(ucOkeyPadPlusHash, HMAC_PAD_SIZE + SHA_1_DIGEST_SIZE, pucHmacResult);

	OxPAL_Free(pucIkeyPadPlusMsg);

	return ERR_OK;
}




#define __________SHA__________
/***************************************************************
* SHA (SHA-1 and SHA-256) //////////////////////////////////////
***************************************************************/

#define SHA_USE_POST_PADDING			// 스펙에 따라 먼저 패딩한 후에 SHA 연산을 거치는 것이 원칙이나 여기서는 마지막에 패딩하는 방식을 사용한다.

//#define SHA_1_DIGEST_SIZE		20
//#define SHA_256_DIGEST_SIZE	32

#define SHA_1_T_NUM			80
#define SHA_256_T_NUM			64

#define SHA_BLOCK_SIZE		64		// for SHA-1, SHA-224, SHA-256 (따라서 이들 알고리즘이 처리하는 메시지 길이는 최대 2^64)
#define SHA_MSG_BUF_SIZE		0x800	// 더 작아도 상관은 없으나 퍼포먼스에 영향이 있다.

typedef struct {
	unsigned long state[8];				// for saving hash values : 5 for SHA-1 and 8 for SHA-256
	unsigned long count[2];
	unsigned char buffer[SHA_BLOCK_SIZE];
} SHA_CTX;

/* basic functions */
#define rol(value, bits)	(((value) << (bits)) | ((value) >> (32 - (bits))))	/* The rotate left (circular left shift) operation */
#define ror(value, bits)	(((value) >> (bits)) | ((value) << (32 - (bits))))	/* The rotate right (circular right shift) operation */
#define shr(value, bits)	((value) >> (bits))										/* The right shift operation */

/* SHA-x functions : For SHA-1, ft (x, y, z) = */
#define Ch(x,y,z)			(((x) & ((y) ^ (z))) ^ (z)) /* (((x) & (y)) | ((~x) & (z))) */						/* For SHA-1, ft :  0 <= t <= 19 */
#define Parity(x,y,z)		((x) ^ (y) ^ (z))																		/* For SHA-1, ft : 20 <= t <= 39 and 60 <= t <= 79 */
#define Maj(x,y,z)			(((x) & (y)) | (((x) | (y)) & (z))) /* (((x) & (y)) | ((x) & (z)) | ((y) & (z))) */	/* For SHA-1, ft : 40 <= t <= 59 */

/* SHA-224 and SHA-256 Functions */
#define SIGMA0(x)			(ror(x,2)  ^ ror(x,13) ^ ror(x,22))
#define SIGMA1(x)			(ror(x,6)  ^ ror(x,11) ^ ror(x,25))
#define sigma0(x)			(ror(x,7)  ^ ror(x,18) ^ shr(x,3))
#define sigma1(x)			(ror(x,17) ^ ror(x,19) ^ shr(x,10))

/* SHA-1 Constants */
static unsigned long SHA_1_K[4] =
{
	0x5a827999,	/*  0 <= t <= 19 */
	0x6ed9eba1,	/* 20 <= t <= 39 */
	0x8f1bbcdc,	/* 40 <= t <= 59 */
	0xca62c1d6		/* 60 <= t <= 79 */
};

/* SHA-224 and SHA-256 Constants */
static unsigned long SHA_256_K[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* for SHA-1 debugging */
#if 0
static int SHA_1_ROUND_COUNT;
#define SHA_1_CHECK_BEGIN		HxLOG_Print(" [->: %08X %08X %08X %08X %08X]\n", state[0], state[1], state[2], state[3], state[4]); SHA_1_ROUND_COUNT = 0;
#define SHA_1_CHECK			HxLOG_Print(" [%02d: %08X %08X %08X %08X %08X]\n", SHA_1_ROUND_COUNT++, a, b, c, d, e);
#define SHA_1_CHECK_END		HxLOG_Print(" [<-: %08X %08X %08X %08X %08X]\n", state[0], state[1], state[2], state[3], state[4]);
#else
#define SHA_1_CHECK_BEGIN
#define SHA_1_CHECK
#define SHA_1_CHECK_END
#endif

static void pal_cipher_SHA_1_Transform(unsigned long state[5], unsigned char buffer[SHA_BLOCK_SIZE])
{
	unsigned long a, b, c, d, e, T;
	unsigned long W[SHA_1_T_NUM];
	int t, i;

	// 1. Prepare the message schedule, Wt :
    for (t = 0, i = 0; t < SHA_BLOCK_SIZE / 4; t++, i += 4)
		W[t] = (unsigned long)buffer[i] << 24 | (unsigned long)buffer[i+1] << 16 | (unsigned long)buffer[i+2] << 8 | (unsigned long)buffer[i+3];
    for (; t < SHA_1_T_NUM; t++)
		W[t] = rol(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);

	// 2. Initialize the five working variables, a, b, c, d, and e :
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	SHA_1_CHECK_BEGIN

	// 3. For t=0 to 79 :
	for (t = 0; t < SHA_1_T_NUM; t++)
	{
		T = rol(a,5) + e + W[t];
		if (t < 20)
			T += (Ch(b,c,d) + SHA_1_K[0]);
		else if (t < 40)
			T += (Parity(b,c,d) + SHA_1_K[1]);
		else if (t < 60)
			T += (Maj(b,c,d) + SHA_1_K[2]);
		else
			T += (Parity(b,c,d) + SHA_1_K[3]);
		e = d;
		d = c;
		c = rol(b,30);
		b = a;
		a = T;

		SHA_1_CHECK
	}

	// 4. Update the hash value :
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	SHA_1_CHECK_END
}

/* for SHA-256 debugging */
#if 0
static int SHA_256_ROUND_COUNT;
#define SHA_256_CHECK_BEGIN		HxLOG_Print(" [->: %08X %08X %08X %08X %08X %08X %08X %08X]\n", state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7]); SHA_256_ROUND_COUNT = 0;
#define SHA_256_CHECK			HxLOG_Print(" [%02d: %08X %08X %08X %08X %08X %08X %08X %08X]\n", SHA_256_ROUND_COUNT++, a, b, c, d, e, f, g, h);
#define SHA_256_CHECK_END		HxLOG_Print(" [<-: %08X %08X %08X %08X %08X %08X %08X %08X]\n", state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7]);
#else
#define SHA_256_CHECK_BEGIN
#define SHA_256_CHECK
#define SHA_256_CHECK_END
#endif

static void pal_cipher_SHA_256_Transform(unsigned long state[8], unsigned char buffer[SHA_BLOCK_SIZE])
{
	unsigned long a, b, c, d, e, f, g, h, T1, T2;
	unsigned long W[SHA_256_T_NUM];
	int t, i;

	// 1. Prepare the message schedule, Wt :
    for (t = 0, i = 0; t < SHA_BLOCK_SIZE / 4; t++, i += 4)
		W[t] = (unsigned long)buffer[i] << 24 | (unsigned long)buffer[i+1] << 16 | (unsigned long)buffer[i+2] << 8 | (unsigned long)buffer[i+3];
    for (; t < SHA_256_T_NUM; t++)
		W[t] = sigma1(W[t-2]) + W[t-7] + sigma0(W[t-15]) + W[t-16];

	// 2. Initialize the eight working variables, a, b, c, d, e, f, g, and h :
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	f = state[5];
	g = state[6];
	h = state[7];

	SHA_256_CHECK_BEGIN

	// 3. For t=0 to 63 :
	for (t = 0; t < SHA_256_T_NUM; t++)
	{
		T1 = h + SIGMA1(e) + Ch(e,f,g) + SHA_256_K[t] + W[t];
		T2 = SIGMA0(a) + Maj(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		SHA_256_CHECK
	}

	// 4. Update the hash value :
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;

	SHA_256_CHECK_END
}

static void pal_cipher_SHA_Init(int x, SHA_CTX *context) /* Initialize new context */
{
	if (x == 1)
	{
		// SHA-1 initialization constants : the initial hash value, H0
		context->state[0] = 0x67452301;
		context->state[1] = 0xEFCDAB89;
		context->state[2] = 0x98BADCFE;
		context->state[3] = 0x10325476;
		context->state[4] = 0xC3D2E1F0;
	}
	else if (x == 256)
	{
		// SHA-256 initialization constants : the initial hash value, H0
		context->state[0] = 0x6a09e667;
		context->state[1] = 0xbb67ae85;
		context->state[2] = 0x3c6ef372;
		context->state[3] = 0xa54ff53a;
		context->state[4] = 0x510e527f;
		context->state[5] = 0x9b05688c;
		context->state[6] = 0x1f83d9ab;
		context->state[7] = 0x5be0cd19;
	}
	else
	{
		HxLOG_Error("[CICC]---> SHA_Init()---> Error : %d not supported !!!\n", x);
		return;
	}

	context->count[0] = context->count[1] = 0;
}

static void pal_cipher_SHA_Update(int x, SHA_CTX *context, unsigned char *data, unsigned long dataLen)
{
	void (*SHA_Transform)(unsigned long *state, unsigned char *buffer);
	unsigned long numByteDataProcessed;	// # of bytes processed so far
	unsigned long numByteInBuffMod64;		// # of bytes in the buffer mod 64

	if (x == 1)
	{
		SHA_Transform = pal_cipher_SHA_1_Transform;
	}
	else if (x == 256)
	{
		SHA_Transform = pal_cipher_SHA_256_Transform;
	}
	else
	{
		HxLOG_Error("[CICC]---> SHA_Update()---> Error : %d not supported !!!\n", x);
		return;
	}

	numByteInBuffMod64 = (context->count[0] >> 3) & 63; // numByteInBuffMod64 = (context->count[0] >> 3) % 64;

	// Adding in the number of bits of data
	if ((context->count[0] += dataLen << 3) < (dataLen << 3))
		context->count[1]++; // add in the carry bit

	context->count[1] += (dataLen >> 29);

	// If there is at least one block to be processed...
	if ((numByteInBuffMod64 + dataLen) > 63)
	{
		// Copy over 64-numByteInBuffMod64 bytes of data to the end of buffer
		HxSTD_memcpy(&context->buffer[numByteInBuffMod64], data, (numByteDataProcessed = 64 - numByteInBuffMod64));

		// Perform the transform on the buffer
		SHA_Transform(context->state, context->buffer);

		// As long as there are 64-bit blocks of data remaining, transform each one.
		for ( ; numByteDataProcessed + 63 < dataLen; numByteDataProcessed += 64)
			SHA_Transform(context->state, &data[numByteDataProcessed]);

		numByteInBuffMod64 = 0;
	}
	// Else there is not enough to process one block.
	else
	{
		numByteDataProcessed = 0;
	}

	// Copy over the remaining data into the buffer
	HxSTD_memcpy(&context->buffer[numByteInBuffMod64], &data[numByteDataProcessed], dataLen - numByteDataProcessed);
}

static void pal_cipher_SHA_Final(int x, SHA_CTX *context, unsigned char *digest) /* Add padding and return the message digest. */
{
	unsigned long digest_size, i /*, j*/;

	if (x == 1)
	{
		digest_size = SHA_1_DIGEST_SIZE;
	}
	else if (x == 256)
	{
		digest_size = SHA_256_DIGEST_SIZE;
	}
	else
	{
		HxLOG_Error("[CICC]---> SHA_Final()---> Error : %d not supported !!!\n", x);
		return;
	}

#ifdef SHA_USE_POST_PADDING
{
	unsigned char numBits[8];

	// Record the number of bits
	#if 1
	for (i = 0; i < 8; i++)
		numBits[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)] >> ((3-(i & 3)) * 8) ) & 255); // Endian independent
	#else
	for (i = 1, j = 0; j < 8; i--, j += 4)
	{
		numBits[j] = (unsigned char)((context->count[i] >> 24) & 0xff);
		numBits[j+1] = (unsigned char)((context->count[i] >> 16) & 0xff);
		numBits[j+2] = (unsigned char)((context->count[i] >> 8) & 0xff);
		numBits[j+3] = (unsigned char)(context->count[i] & 0xff);
	}
	#endif

	// Add padding
	pal_cipher_SHA_Update(x, context, (unsigned char *)"\200", 1);
	while ((context->count[0] & 504) != 448)
		pal_cipher_SHA_Update(x, context, (unsigned char *)"\0", 1);

	// Append length
	pal_cipher_SHA_Update(x, context, numBits, 8); // Should cause a SHA_1_Transform()
}
#endif

	// Store state in digest
	#if 1
    for (i = 0; i < digest_size; i++)
        digest[i] = (unsigned char)((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
	#else
	for (i = 0, j = 0; j < digest_size; i++, j += 4)
	{
		digest[j] = (unsigned char)((context->state[i] >> 24) & 0xff);
		digest[j+1] = (unsigned char)((context->state[i] >> 16) & 0xff);
		digest[j+2] = (unsigned char)((context->state[i] >> 8) & 0xff);
		digest[j+3] = (unsigned char)((context->state[i]) & 0xff);
	}
	#endif
}

void PAL_CIPHER_SHA_x(int x, unsigned char *Msg, unsigned long msgLen, unsigned char *digest) /* Digest의 공간 크기는 SHA_x_DIGEST_SIZE 이어야 한다. */
{
	SHA_CTX context;
	unsigned long i = 0, loop, remain;

	if (x != 1 && x != 256)
	{
		HxLOG_Error("[CICC]---> SHA()---> Error : x(%d)---> must be 1 or 256 !!!\n", x);
		return;
	}

	loop = msgLen / SHA_MSG_BUF_SIZE;
	remain = msgLen % SHA_MSG_BUF_SIZE;

	pal_cipher_SHA_Init(x, &context);
	for (i = 0; i < loop; i++)
		pal_cipher_SHA_Update(x, &context, &Msg[i*SHA_MSG_BUF_SIZE], SHA_MSG_BUF_SIZE);
	if (remain)
		pal_cipher_SHA_Update(x, &context, &Msg[i*SHA_MSG_BUF_SIZE], remain);
	pal_cipher_SHA_Final(x, &context, digest);
}

#define __________AES__________
/***************************************************************
* AES (AES-x-ECB and AES-x-CBC, x = 128, 192, 256) /////////////
***************************************************************/

#define Nb	4							/* The number of columns comprising a state in AES. This value 4 is a constant in AES. */ /* Nb*4 = PAL_CIPHER_BLOCK_SIZE */
static int Nk;							/* The number of 32 bit words in the key. Nk=128/32=4 for AES-128 and 6/8 for AES-192/256 */ /* Nk*4 = CI_CC_CIPHER_KEY_LEN */
static int Nr;							/* The number of rounds in AES Cipher. Nr=Nk+6=4+6=10 for AES-128 and 12/14 for AES-192/256 */
static unsigned char RoundKey[240];	/* stores the round keys used in each round to encrypt/decrypt the states. */
static unsigned char state[4][4];		/* holds the intermediate results during encryption or decryption. */
static int sbox[256] =					/* S-box: substitution values for the byte xy */
{
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
static int rsbox[256] =				/* Inverse S-box: substitution values for the byte xy */
{
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};
static int Rcon[255] =					/* The round constant word array, Rcon[i], contains the values given by x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8) */
{
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
};

#define getSBoxValue(num)		sbox[num]
#define getSBoxInvert(num)	rsbox[num]
#define xtime(x)				((x<<1) ^ (((x>>7) & 1) * 0x1b))	/* xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b} */
#define Multiply(x,y)			(((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x)))))) /* Multiply is a macro used to multiply numbers in the field GF(2^8) */

/***************************************************************************/
#define ________Local_Functions________
/***************************************************************************/
static void KeyExpansion(unsigned char *Key) /* This function produces Nb(Nr+1) round keys. */
{
	int i,j;
	unsigned char temp[4],k;

	// The first round key is the key itself.
	for(i=0;i<Nk;i++)
	{
		RoundKey[i*4]=Key[i*4];
		RoundKey[i*4+1]=Key[i*4+1];
		RoundKey[i*4+2]=Key[i*4+2];
		RoundKey[i*4+3]=Key[i*4+3];
	}

	// All other round keys are found from the previous round keys.
	while (i < (Nb * (Nr+1)))
	{
		for(j=0;j<4;j++)
		{
			temp[j]=RoundKey[(i-1) * 4 + j];
		}
		if (i % Nk == 0)
		{
			// This function rotates the 4 bytes in a word to the left once.
			// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

			// Function RotWord()
			{
				k = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = k;
			}

			// SubWord() is a function that takes a four-byte input word and
			// applies the S-box to each of the four bytes to produce an output word.

			// Function Subword()
			{
				temp[0]=getSBoxValue(temp[0]);
				temp[1]=getSBoxValue(temp[1]);
				temp[2]=getSBoxValue(temp[2]);
				temp[3]=getSBoxValue(temp[3]);
			}

			temp[0] =  temp[0] ^ Rcon[i/Nk];
		}
		else if (Nk > 6 && i % Nk == 4)
		{
			// Function Subword()
			{
				temp[0]=getSBoxValue(temp[0]);
				temp[1]=getSBoxValue(temp[1]);
				temp[2]=getSBoxValue(temp[2]);
				temp[3]=getSBoxValue(temp[3]);
			}
		}
		RoundKey[i*4+0] = RoundKey[(i-Nk)*4+0] ^ temp[0];
		RoundKey[i*4+1] = RoundKey[(i-Nk)*4+1] ^ temp[1];
		RoundKey[i*4+2] = RoundKey[(i-Nk)*4+2] ^ temp[2];
		RoundKey[i*4+3] = RoundKey[(i-Nk)*4+3] ^ temp[3];
		i++;
	}
}

static void AddRoundKey(int round) /* This function adds the round key to state. The round key is added to the state by an XOR function. */
{
	int i,j;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] ^= RoundKey[round * Nb * 4 + i * Nb + j];
		}
	}
}

static void SubBytes(void) /* This function substitutes the values in the state matrix with values in an S-box. */
{
	int i,j;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[i][j] = getSBoxValue(state[i][j]);
		}
	}
}

static void InvSubBytes(void) /* This function substitutes the values in the state matrix with values in an Inverse S-box. */
{
	int i,j;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[i][j] = getSBoxInvert(state[i][j]);
		}
	}
}

static void ShiftRows(void) /* This function shifts the rows in the state to the left. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted. */
{
	unsigned char temp;

	// Rotate first row 1 columns to left
	temp=state[1][0];
	state[1][0]=state[1][1];
	state[1][1]=state[1][2];
	state[1][2]=state[1][3];
	state[1][3]=temp;

	// Rotate second row 2 columns to left
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;

	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	// Rotate third row 3 columns to left
	temp=state[3][0];
	state[3][0]=state[3][3];
	state[3][3]=state[3][2];
	state[3][2]=state[3][1];
	state[3][1]=temp;
}

static void InvShiftRows(void) /* This function shifts the rows in the state to the right. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted. */
{
	unsigned char temp;

	// Rotate first row 1 columns to right
	temp=state[1][3];
	state[1][3]=state[1][2];
	state[1][2]=state[1][1];
	state[1][1]=state[1][0];
	state[1][0]=temp;

	// Rotate second row 2 columns to right
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;

	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	// Rotate third row 3 columns to right
	temp=state[3][0];
	state[3][0]=state[3][1];
	state[3][1]=state[3][2];
	state[3][2]=state[3][3];
	state[3][3]=temp;
}

static void MixColumns(void) /* This function mixes the columns of the state matrix. */
{
	int i;
	unsigned char Tmp,Tm,t;

	for(i=0;i<4;i++)
	{
		t=state[0][i];
		Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
		Tm = state[0][i] ^ state[1][i] ; Tm = xtime(Tm); state[0][i] ^= Tm ^ Tmp ;
		Tm = state[1][i] ^ state[2][i] ; Tm = xtime(Tm); state[1][i] ^= Tm ^ Tmp ;
		Tm = state[2][i] ^ state[3][i] ; Tm = xtime(Tm); state[2][i] ^= Tm ^ Tmp ;
		Tm = state[3][i] ^ t ; Tm = xtime(Tm); state[3][i] ^= Tm ^ Tmp ;
	}
}

static void InvMixColumns(void) /* This function mixes the columns of the state matrix. */
{
	int i;
	unsigned char a,b,c,d;

	for(i=0;i<4;i++)
	{
		a = state[0][i];
		b = state[1][i];
		c = state[2][i];
		d = state[3][i];

		state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}

static void Cipher(unsigned char *in, unsigned char *out) /* This is the main function that encrypts the PlainText. */
{
	int i,j,round=0;

	// Copy the input PlainText to state array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] = in[i*4 + j];
		}
	}

	// Add the First round key to the state before starting the rounds.
	AddRoundKey(0);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for(round=1;round<Nr;round++)
	{
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
	}

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);

	// The encryption process is over.
	// Copy the state array to output array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			out[i*4+j]=state[j][i];
		}
	}
}

static void InvCipher(unsigned char *in, unsigned char *out) /* This is the main function that decrypts the CipherText. */
{
	int i,j,round=0;

	// Copy the input CipherText to state array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] = in[i*4 + j];
		}
	}

	// Add the First round key to the state before starting the rounds.
	AddRoundKey(Nr);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for(round=Nr-1;round>0;round--)
	{
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(round);
		InvMixColumns();
	}

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);

	// The decryption process is over.
	// Copy the state array to output array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			out[i*4+j]=state[j][i];
		}
	}
}

static void pal_cipher_OS2IP(PalCipher_BigNumber_e *X, PalCipher_BigNumber_e *x) /* converts an octet string to a nonnegative integer */
{
	// if X = 0x00 0x00 0x00 0x00 0xab 0xcd 0xef, OS2IP() -> x = 11259375 (0xab 0xcd 0xef), its length = 3

	bnLenType i;

	for (i = 0; i < X->len; i++)
	{
		if (X->num[i] != 0x00)
			break;
	}

	x->len = X->len - i;
	HxSTD_memcpy(x->num, &X->num[i], x->len);
}

static int pal_cipher_I2OSP(PalCipher_BigNumber_e *x, bnLenType xLen, PalCipher_BigNumber_e *X) /* converts a nonnegative integer to an octet string of a specified length */
{
	// if x = 11259375 (0xab 0xcd 0xef) and xLen = 7, I2OSP() -> X = 0x00 0x00 0x00 0x00 0xab 0xcd 0xef

	if (x->len > xLen)
	{
		HxLOG_Error("[CICC]---> I2OSP()---> Error : integer too large !!!\n");
		return -1;
	}

	X->len = xLen;
	HxSTD_memset(X->num, 0x00, xLen - x->len);
	HxSTD_memcpy(&X->num[xLen - x->len], x->num, x->len);

	return ERR_OK;
}

static int pal_cipher_RSAVP1(PalCipher_RsaPublicKey_e *K, PalCipher_BigNumber_e *s, PalCipher_BigNumber_e *m) /* Verification primitive function */
{
	/*
		< Input >
			(n, e) : RSA public key
			s      : signature representative, an integer between 0 and n-1
		< Output >
			m      : message representative, an integer between 0 and n-1
		< Errors >
			signature representative out of range
		< Assumption >
			RSA public key (n, e) is valid
		< Steps >
			1. If the signature representative s is not between 0 and n-1, output "signature representative out of range" and stop.
			2. Let m = s^e mod n.
			3. Output m.
	*/

	#define CHECK_RSAVP1


	if (pal_cipher_bn_cmp(s, K->n) >= 0)
	{
		HxLOG_Error("[CICC]---> RSAVP1()---> Error : signature representative out of range !!!\n");
		return -1;
	}

//	pal_cipher_bn_mod_exp(s, K->e, K->n, m);

#ifdef CHECK_RSAVP1
{
	bnLenType i;
	HxLOG_Print("RSAVP1 : s = ");
	for (i = 0; i < s->len; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print("%02x ", s->num[i]);
	}
	HxLOG_Print("\nRSAVP1 : m = ");
	for (i = 0; i < m->len; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print("%02x ", m->num[i]);
	}
	HxLOG_Print("\n");
}
#endif

	return ERR_OK;
}

static int pal_cipher_GetHashValue(unsigned char *T, PalCipher_HashingAlg_e eHashAlg)
{
	unsigned char 	MD5_T[MD5_HASH_VALUE_LEN] = {0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05, 0x05, 0x00, 0x04, 0x10};
	unsigned char 	*pucSelectMD5_T;
	int				nHashValueLen;

	if(T == NULL)
	{
		return ERR_FAIL;
	}

	switch(eHashAlg)
	{
	case eHashingAlg_MD5:
	default :
		pucSelectMD5_T 	= MD5_T;
		nHashValueLen	= MD5_HASH_VALUE_LEN;
		break;
	}

	HxSTD_memcpy(T , pucSelectMD5_T, nHashValueLen);

	return ERR_OK;
}

static int pal_cipher_EMSA_PKCS1_v1_5_ENCODE(PalCipher_BigNumber_e *M, bnLenType emLen, PalCipher_BigNumber_e *EM, PalCipher_HashingAlg_e eHashAlg) /* EMSA-PKCS1-v1_5-ENCODE function based on a hash function. */
{
	/*
		< Options >
			Hash   : hash function (hLen denotes the length in octets of the hash function output)
		< Input >
			M	   : message to be encoded
			emLen  : intended length in octets of the encoded message, at least tLen + 11, where tLen is the octet length of the DER encoding T of a certain value computed during the encoding operation
		< Output >
			EM	   : encoded message, an octet string of length emLen
		< Errors >
			message too long or intended encoded message length too short
		< Steps >
			See below.
	*/

	unsigned char 	H[MAX_DIGEST_SIZE];
	int				nDigestSize;
	int				nHashValueLen;
	unsigned char 	*T = NULL;
	bnLenType 		tLen;

#if defined(CHECK_EMSA_PKCS1_v1_5_ENCODE)
	HxLOG_Print("pal_cipher_EMSA_PKCS1_v1_5_ENCODE : emLen = %d\n", emLen);
#endif

	/* 1. Apply the hash function to the message M to produce a hash value H : H = Hash (M). If the hash function outputs "message too long" output "message too long" and stop. */

	switch(eHashAlg)
	{
	case eHashingAlg_MD5 :
	default :
		nDigestSize 	= MD5_DIGEST_SIZE;
		nHashValueLen	= MD5_HASH_VALUE_LEN;
		PAL_CIPHER_MD5(M->num, M->len, H);
		break;
	}

	T = OxPAL_Malloc(nDigestSize+nHashValueLen);

	if(T == NULL)
	{
		HxLOG_Error("mem alloc error\n");
		return -1;
	}

#if defined(CHECK_EMSA_PKCS1_v1_5_ENCODE)
{
	int i;
	HxLOG_Print("pal_cipher_EMSA_PKCS1_v1_5_ENCODE : H = ");
	for (i = 0; i < 16; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print("%02x ", H[i]);
	}
	HxLOG_Print("\n");
}
#endif

	pal_cipher_GetHashValue(T ,eHashAlg);

	HxSTD_memcpy(&T[nHashValueLen], H, nDigestSize);

	/* 3. If emLen <  tLen + 11, output "intended encoded message length too short" and stop. */

	tLen = nDigestSize + nHashValueLen;

	if (emLen < tLen + 11)
	{
		if(T != NULL)
		{
			OxPAL_Free(T);
		}
		HxLOG_Error("[CICC]---> pal_cipher_EMSA_PKCS1_v1_5_ENCODE()---> Error : emLen(%d) is less than tLen(%d) + 11 !!!\n", emLen, tLen);
		return -1;
	}

	/* 4. Generate an octet string PS consisting of emLen - tLen - 3 octets with hexadecimal value 0xff. The length of PS will be at least 8 octets -> 위의 3번 step 에서 체크됨 */
	/* 5. Concatenate PS, the DER encoding T, and other padding to form the encoded message EM as EM = 0x00 || 0x01 || PS || 0x00 || T */
	/* 6. Output EM. */
	EM->len = emLen;
	EM->num[0] = 0x00;
	EM->num[1] = 0x01;
	HxSTD_memset(&EM->num[2], 0xff, emLen - tLen - 3);
	EM->num[emLen - tLen - 1] = 0x00;
	HxSTD_memcpy(&EM->num[emLen - tLen], T, tLen);

#if defined(CHECK_EMSA_PKCS1_v1_5_ENCODE)
{
	int i;
	HxLOG_Print("pal_cipher_EMSA_PKCS1_v1_5_ENCODE : EM = ");
	for (i = 0; i < emLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print("%02x ", EM->num[i]);
	}
	HxLOG_Print("\n");
}
#endif

	if(T != NULL)
	{
		OxPAL_Free(T);
	}

	return ERR_OK;
}

/***************************************************************************/
#define _______Global_Functions________
/***************************************************************************/

void PAL_CIPHER_EncryptAesCbc(int x, unsigned char *Key, unsigned char *PlainText, unsigned char *CipherText, unsigned short TextLen, unsigned char *IV)
	/* Key 길이는 Nk*4 이어야 한다. PlainText, CipherText 길이(TextLen값)는 PAL_CIPHER_BLOCK_SIZE 의 배수이어야 한다. see OneAndZeroesPadding(). */
{
	unsigned char V[PAL_CIPHER_BLOCK_SIZE];
	unsigned short i, j;

	if (TextLen == 0 || TextLen & PAL_CIPHER_BLOCK_SIZE_1) // if (TextLen == 0 || TextLen % PAL_CIPHER_BLOCK_SIZE)
	{
		HxLOG_Error("[CICC]---> E_AES_x_CBC()---> Error : TextLen(%d)---> padding required !!!\n", TextLen);
		return;
	}

	if (x != 128 && x != 192 && x != 256)
	{
		HxLOG_Error("[CICC]---> E_AES_x_CBC()---> Error : x(%d)---> must be 128 or 192 or 256 !!!\n", x);
		return;
	}

	Nk = x / 32;
	Nr = Nk + 6;

	KeyExpansion(Key);

	for (i = 0; i < PAL_CIPHER_BLOCK_SIZE; i++)
		V[i] = IV[i] ^ PlainText[i];

	Cipher(V, CipherText);

	for (i = PAL_CIPHER_BLOCK_SIZE; i < TextLen; i += PAL_CIPHER_BLOCK_SIZE)
	{
		for (j = 0; j < PAL_CIPHER_BLOCK_SIZE; j++)
			V[j] = CipherText[i-PAL_CIPHER_BLOCK_SIZE+j] ^ PlainText[i+j];

		Cipher(V, &CipherText[i]);
	}
}

void PAL_CIPHER_DecryptAesCbc(int x, unsigned char *Key, unsigned char *CipherText, unsigned char *PlainText, unsigned short TextLen, unsigned char *IV)
	/* Key 길이는 Nk*4 이어야 한다. PlainText, CipherText 길이(TextLen값)는 PAL_CIPHER_BLOCK_SIZE 의 배수이어야 한다. see OneAndZeroesPadding(). */
{
	unsigned short i, j;

	if (TextLen == 0 || TextLen & PAL_CIPHER_BLOCK_SIZE_1) // if (TextLen == 0 || TextLen % PAL_CIPHER_BLOCK_SIZE)
	{
		HxLOG_Error("[CICC]---> D_AES_x_CBC()---> Error : TextLen(%d)---> padding required !!!\n", TextLen);
		return;
	}

	if (x != 128 && x != 192 && x != 256)
	{
		HxLOG_Error("[CICC]---> D_AES_x_CBC()---> Error : x(%d)---> must be 128 or 192 or 256 !!!\n", x);
		return;
	}

	Nk = x / 32;
	Nr = Nk + 6;

	KeyExpansion(Key);

	InvCipher(CipherText, PlainText);

	for (i = 0; i < PAL_CIPHER_BLOCK_SIZE; i++)
		PlainText[i] = IV[i] ^ PlainText[i];

	for (i = PAL_CIPHER_BLOCK_SIZE; i < TextLen; i += PAL_CIPHER_BLOCK_SIZE)
	{
		InvCipher(&CipherText[i], &PlainText[i]);

		for (j = 0; j < PAL_CIPHER_BLOCK_SIZE; j++)
			PlainText[i+j] = CipherText[i-PAL_CIPHER_BLOCK_SIZE+j] ^ PlainText[i+j];
	}
}

HERROR PAL_CIPHER_VerifyRsaSsaPkcs1V15(PalCipher_RsaPublicKey_e *K, PalCipher_BigNumber_e *M, PalCipher_BigNumber_e *S, PalCipher_HashingAlg_e eHashAlg) /* RSASSA-PKCS1-V1_5-VERIFY function : return 1 if valid signature, 0 if invalid signature */
{
	/*
		< Input >
			(n, e) : signer's RSA public key
			M      : message whose signature is to be verified, an octet string
			S      : signature to be verified, an octet string of length k, where k is the length in octets of the RSA modulus n
		< Output >
			valid signature or invalid signature
		< Errors >
			message too long or RSA modulus too short
		< Steps >
			See below.
	*/

	bnLenType k;
	PalCipher_BigNumber_e s, m, EM1, EM2;

	HxSTD_memset(&m, 0, sizeof(PalCipher_BigNumber_e));

	/* 파라미터 체크 */
	if (K == NULL || K->n == NULL || M == NULL || S == NULL)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : null params !!!\n");
		return ERR_FAIL; // invalid signature
	}
	if (K->n->len < 1 || K->n->len > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : wrong length K %d !!!\n", K->n->len);
		return ERR_FAIL; // invalid signature
	}
	if (M->len < 1 || M->len > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : wrong length M %d !!!\n", M->len);
		return ERR_FAIL; // invalid signature
	}

	k = K->n->len;

	/* 1. Length checking : If the length of the signature S is not k octets, output "invalid signature" and stop. */
	if (S->len != k)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : the length of the signature (%d) is not %d octets !!!\n", S->len, k);
		return ERR_FAIL; // invalid signature
	}

	/* 2. RSA verification (step a ~ c) : */

	/* a. Convert the signature S to an integer signature representative s : s = OS2IP (S) */
	pal_cipher_OS2IP(S, &s);

	/* b. Apply the RSAVP1 verification primitive to the RSA public key (n, e) and the signature representative s to produce an integer message representative m : m = RSAVP1 ((n, e), s).
	      If RSAVP1 output "signature representative out of range" output "invalid signature" and stop. */
	if (pal_cipher_RSAVP1(K, &s, &m) < 0)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : signature representative out of range !!!\n");
		return ERR_FAIL; // invalid signature
	}

	/* c. Convert the message representative m to an encoded message EM of length k octets : EM' = I2OSP (m, k).
	      If I2OSP outputs "integer too large" output "invalid signature" and stop. */
	if (pal_cipher_I2OSP(&m, k, &EM1) < 0)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : integer too large !!!\n");
		return ERR_FAIL; // invalid signature
	}

	/* 3. EMSA-PKCS1-v1_5 encoding :
	      Apply the EMSA-PKCS1-v1_5 encoding operation to the message M to produce a second encoded message EM' of length k octets : EM' = EMSA-PKCS1-V1_5-ENCODE (M, k).
	      If the encoding operation outputs "message too long" output "message too long" and stop.
	      If the encoding operation outputs "intended encoded message length too short" output "RSA modulus too short" and stop.*/
	if (pal_cipher_EMSA_PKCS1_v1_5_ENCODE(M, k, &EM2, eHashAlg) < 0)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : EMSA_PKCS1_v1_5_ENCODE !!!\n");
		return ERR_FAIL; // invalid signature
	}


#if defined(CHECK_RSASSA_PKCS1_V1_5_VERIFY)
	{
		int i;
		HxLOG_Print("RSASSA_PKCS1_V1_5_VERIFY : EM1 = ");
		for (i = 0; i < EM1.len; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print("%02x ", EM1.num[i]);
		}
		HxLOG_Print("\n");
		HxLOG_Print("RSASSA_PKCS1_V1_5_VERIFY : EM2 = ");
		for (i = 0; i < EM2.len; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print("%02x ", EM2.num[i]);
		}
		HxLOG_Print("\n");
	}
#endif

	/* 4. Compare the encoded message EM and the second encoded message EM'. If they are the same, output "valid signature"; otherwise, output "invalid signature." */
	if (pal_cipher_bn_cmp(&EM1, &EM2) != 0)
	{
		HxLOG_Error("[CICC]---> RSASSA_PKCS1_V1_5_VERIFY()---> Error : EM1 != EM2 !!!\n");

		return ERR_FAIL; // invalid signature
	}

	return ERR_OK; // valid signature
}

void PAL_CIPHER_MD5(unsigned char *message, unsigned long length, char *result)
{
	MD5_CTX		context;
 	int			len;
	unsigned char buffer[1024], digest[MD5_DIGEST_SIZE];
 	unsigned 	read = 0;

 	pal_cipher_MD5Init(&context);
	while(1)
	{
		if( read + 1024 < length )
	  		len = 1024;
		else
		{
			len = length - read;
		}
		HxSTD_memcpy(buffer, (char *)(message+read), len);
		pal_cipher_MD5Update(&context, buffer, len);
		read += 1024;

		if( read > length )
			break;
	}
	pal_cipher_MD5Final(digest, &context);

	HxSTD_memcpy(result, digest, MD5_DIGEST_SIZE);
}

