/*
 * hxaes.c
 *
 *  Created on: 2012. 03. 08.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#if 0
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#endif

#include <_hlib_log.h>
#include <_hlib_stdlib.h>
#include <_hlib_mem.h>
#include <_hlib_rwstream.h>
#include <_hlib_aes.h>

#undef HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"hxaes"


#define CIPHER_BLOCK_SIZE		16	/* 128 bits (= Nb*4) for AES-x : For TS packet payload, max number of blocks is 11 for encryption with AES-128-CBC since residual blocks remain in the clear 184*8/128 is rounded to 11 */
#define CIPHER_BLOCK_SIZE_1		15


/***************************************************************
* AES (AES-x-ECB and AES-x-CBC, x = 128, 192, 256) /////////////
***************************************************************/

#define Nb	4							/* The number of columns comprising a state in AES. This value 4 is a constant in AES. */ /* Nb*4 = CIPHER_BLOCK_SIZE */
STATIC int Nk;							/* The number of 32 bit words in the key. Nk=128/32=4 for AES-128 and 6/8 for AES-192/256 */ /* Nk*4 = CI_CC_CIPHER_KEY_LEN */
STATIC int Nr;							/* The number of rounds in AES Cipher. Nr=Nk+6=4+6=10 for AES-128 and 12/14 for AES-192/256 */
STATIC unsigned char RoundKey[240];	/* stores the round keys used in each round to encrypt/decrypt the states. */
STATIC unsigned char state[4][4];		/* holds the intermediate results during encryption or decryption. */
STATIC int sbox[256] =					/* S-box: substitution values for the byte xy */
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
STATIC int rsbox[256] =				/* Inverse S-box: substitution values for the byte xy */
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
STATIC int Rcon[255] =					/* The round constant word array, Rcon[i], contains the values given by x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8) */
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
STATIC void hlib_crypt_aes_KeyExpansion(const unsigned char *pszKey) /* This function produces Nb(Nr+1) round keys. */
{
	int i,j;
	unsigned char temp[4],k;

	// The first round key is the key itself.
	for(i=0;i<Nk;i++)
	{
		RoundKey[i*4]=pszKey[i*4];
		RoundKey[i*4+1]=pszKey[i*4+1];
		RoundKey[i*4+2]=pszKey[i*4+2];
		RoundKey[i*4+3]=pszKey[i*4+3];
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

STATIC void hlib_crypt_aes_AddRoundKey(int round) /* This function adds the round key to state. The round key is added to the state by an XOR function. */
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

STATIC void hlib_crypt_aes_SubBytes(void) /* This function substitutes the values in the state matrix with values in an S-box. */
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

STATIC void hlib_crypt_aes_InvSubBytes(void) /* This function substitutes the values in the state matrix with values in an Inverse S-box. */
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

STATIC void hlib_crypt_aes_ShiftRows(void) /* This function shifts the rows in the state to the left. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted. */
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

STATIC void hlib_crypt_aes_InvShiftRows(void) /* This function shifts the rows in the state to the right. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted. */
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

STATIC void hlib_crypt_aes_MixColumns(void) /* This function mixes the columns of the state matrix. */
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

STATIC void hlib_crypt_aes_InvMixColumns(void) /* This function mixes the columns of the state matrix. */
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

STATIC void hlib_crypt_aes_Cipher(unsigned char *in, unsigned char *out) /* This is the main function that encrypts the PlainText. */
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
	hlib_crypt_aes_AddRoundKey(0);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for(round=1;round<Nr;round++)
	{
		hlib_crypt_aes_SubBytes();
		hlib_crypt_aes_ShiftRows();
		hlib_crypt_aes_MixColumns();
		hlib_crypt_aes_AddRoundKey(round);
	}

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	hlib_crypt_aes_SubBytes();
	hlib_crypt_aes_ShiftRows();
	hlib_crypt_aes_AddRoundKey(Nr);

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

STATIC void hlib_crypt_aes_InvCipher(const unsigned char *in, unsigned char *out) /* This is the main function that decrypts the CipherText. */
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
	hlib_crypt_aes_AddRoundKey(Nr);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for(round=Nr-1;round>0;round--)
	{
		hlib_crypt_aes_InvShiftRows();
		hlib_crypt_aes_InvSubBytes();
		hlib_crypt_aes_AddRoundKey(round);
		hlib_crypt_aes_InvMixColumns();
	}

	// The last round is given below.
	// The MixColumns function is not here in the last round.

	hlib_crypt_aes_InvShiftRows();
	hlib_crypt_aes_InvSubBytes();
	hlib_crypt_aes_AddRoundKey(0);

	// The decryption process is over.
	// Copy the state array to output array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
		    if(state[j][i] == 0x04)//check EOT
            {
            	out[i*4+j]=0;
            }
            else
            {
    			out[i*4+j]=state[j][i];
            }

		}
	}
}

STATIC HUINT32	hlib_crypt_aes_GetKeyLengthAes(HLIB_CRYPT_AES_X eAesX)
{
	switch (eAesX)
	{
	case HLIB_CRYPT_AES_128:		return 128;
	case HLIB_CRYPT_AES_192:		return 192;
	case HLIB_CRYPT_AES_256:		return 256;
	default:					break;
	}

	return 0;
}

static void BlockCopy(HUINT8* output, HUINT8* input)
{
  HUINT8 i;
  for (i=0;i<16;++i)
  {
    output[i] = input[i];
  }
}


/***************************************************************************/
#define _______Global_Functions________
/***************************************************************************/


void HLIB_CRYPT_EncryptAesCbc(HLIB_CRYPT_AES_X eAesX, const HUINT8 *pszKey, const HUINT8 *szPlainText, HUINT8 *szCipherText, HUINT16 TextLen, HUINT8 *IV)
	/* Key ���̴� Nk*4 �̾�� �Ѵ�. PlainText, CipherText ����(TextLen��)�� CIPHER_BLOCK_SIZE �� ����̾�� �Ѵ�. see OneAndZeroesPadding(). */
{
	unsigned char V[CIPHER_BLOCK_SIZE];
	unsigned short i, j;
	HUINT32 ulKeyLength;

	if (TextLen == 0 || TextLen & CIPHER_BLOCK_SIZE_1) // if (TextLen == 0 || TextLen % CIPHER_BLOCK_SIZE)
	{
		HxLOG_Error("[CICC]---> E_AES_x_CBC()---> Error : TextLen(%d)---> padding required !!!\n", TextLen);
		return;
	}

	ulKeyLength = hlib_crypt_aes_GetKeyLengthAes(eAesX);
	if (ulKeyLength != 128 && ulKeyLength != 192 && ulKeyLength != 256)
	{
		HxLOG_Error("[CICC]---> E_AES_x_CBC()---> Error : x(%d)---> must be 128 or 192 or 256 !!!\n", hlib_crypt_aes_GetKeyLengthAes(eAesX));
		return;
	}

	Nk = ulKeyLength / 32;
	Nr = Nk + 6;

	hlib_crypt_aes_KeyExpansion(pszKey);

	for (i = 0; i < CIPHER_BLOCK_SIZE; i++)
		V[i] = IV[i] ^ szPlainText[i];

	hlib_crypt_aes_Cipher(V, szCipherText);

	for (i = CIPHER_BLOCK_SIZE; i < TextLen; i += CIPHER_BLOCK_SIZE)
	{
		for (j = 0; j < CIPHER_BLOCK_SIZE; j++)
			V[j] = szCipherText[i-CIPHER_BLOCK_SIZE+j] ^ szPlainText[i+j];

		hlib_crypt_aes_Cipher(V, &szCipherText[i]);
	}
}

void HLIB_CRYPT_DecryptAesCbc(HLIB_CRYPT_AES_X eAesX, const HUINT8 *pszKey, const HUINT8 *szCipherText, HUINT8 *szPlainText, HUINT16 TextLen, HUINT8 *IV)
	/* Key ���̴� Nk*4 �̾�� �Ѵ�. PlainText, CipherText ����(TextLen��)�� CIPHER_BLOCK_SIZE �� ����̾�� �Ѵ�. see OneAndZeroesPadding(). */
{
	unsigned short i, j;
	HUINT32 ulKeyLength;

	if (TextLen == 0 || TextLen & CIPHER_BLOCK_SIZE_1) // if (TextLen == 0 || TextLen % CIPHER_BLOCK_SIZE)
	{
		HxLOG_Error("[CICC]---> D_AES_x_CBC()---> Error : TextLen(%d)---> padding required !!!\n", TextLen);
		return;
	}

	ulKeyLength = hlib_crypt_aes_GetKeyLengthAes(eAesX);
	if (ulKeyLength != 128 && ulKeyLength != 192 && ulKeyLength != 256)
	{
		HxLOG_Error("[CICC]---> D_AES_x_CBC()---> Error : x(%d)---> must be 128 or 192 or 256 !!!\n", hlib_crypt_aes_GetKeyLengthAes(eAesX));
		return;
	}

	Nk = ulKeyLength / 32;
	Nr = Nk + 6;

	hlib_crypt_aes_KeyExpansion(pszKey);

	hlib_crypt_aes_InvCipher(szCipherText, szPlainText);

	for (i = 0; i < CIPHER_BLOCK_SIZE; i++)
		szPlainText[i] = IV[i] ^ szPlainText[i];

	for (i = CIPHER_BLOCK_SIZE; i < TextLen; i += CIPHER_BLOCK_SIZE)
	{
		hlib_crypt_aes_InvCipher(&szCipherText[i], &szPlainText[i]);

		for (j = 0; j < CIPHER_BLOCK_SIZE; j++)
			szPlainText[i+j] = szCipherText[i-CIPHER_BLOCK_SIZE+j] ^ szPlainText[i+j];
	}
}

void HLIB_CRYPT_DecryptAesECB(HLIB_CRYPT_AES_X eAesX, const HUINT8 *key, const HUINT8 *input, HUINT8 *output)
{
	HUINT32 ulKeyLength;

	ulKeyLength = hlib_crypt_aes_GetKeyLengthAes(eAesX);
	if (ulKeyLength != 128 && ulKeyLength != 192 && ulKeyLength != 256)
	{
		HxLOG_Error("[CICC]---> D_AES_x_ECB()---> Error : x(%d)---> must be 128 or 192 or 256 !!!\n", hlib_crypt_aes_GetKeyLengthAes(eAesX));
		return;
	}

    Nk = ulKeyLength / 32;
	Nr = Nk + 6;

    HxLOG_Error("HLIB_CRYPT_DecryptAesECB Nk : %d, Nr : %d \n", Nk, Nr);

	hlib_crypt_aes_KeyExpansion(key);

	hlib_crypt_aes_InvCipher(input, output);
}
