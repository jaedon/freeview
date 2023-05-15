/********************************************************************************************
*     (c)2004-2012 Broadcom Corporation                                                     *
*                                                                                           *
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,   *
*  and may only be used, duplicated, modified or distributed pursuant to the terms and      *
*  conditions of a separate, written license agreement executed between you and Broadcom    *
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants*
*  no license (express or implied), right to use, or waiver of any kind with respect to the *
*  Software, and Broadcom expressly reserves all rights in and to the Software and all      *
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU       *
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY                    *
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.                                 *
*
*  Except as expressly set forth in the Authorized License,                                 *
*
*  1.     This program, including its structure, sequence and organization, constitutes     *
*  the valuable trade secrets of Broadcom, and you shall use all reasonable efforts to      *
*  protect the confidentiality thereof,and to use this information only in connection       *
*  with your use of Broadcom integrated circuit products.                                   *
*                                                                                           *
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"          *
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR                   *
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO            *
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES            *
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,            *
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION             *
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF              *
*  USE OR PERFORMANCE OF THE SOFTWARE.                                                      *
*                                                                                           *
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS         *
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR             *
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR               *
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF             *
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT              *
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE            *
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF              *
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_ecc_wrapper.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 9/11/12 2:52p $
 * 
 * Module Description:
 *    DTCP Ecliptic Curve Algorithm wrappers.
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_ecc_wrapper.c $
 * 
 * 11   9/11/12 2:52p leisun
 * SWSECURITY-48: Auto-generating test keys, consolidate test/production
 *  build, make common DRM default ON
 * 
 * 10   4/18/12 6:16p celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 9   12/12/11 12:18p leisun
 * SWSECURITY-89: DTCP-IP Lib CKC fix from Panasoc CSP442016 , 459589 and
 *  477670
 * 
 * 8   11/30/11 6:09p leisun
 * SWSECURITY-89: CKC fix from panasonic
 * 
 * 7   6/17/11 10:00a leisun
 * SWSECURITY-54: Fix string length for GetFirstPhaseValue() function
 * 
 * 6   5/24/10 5:14p leisun
 * SW7420-561: Fix for coverity defects report and compiler warnings
 * 
 * 5   7/31/09 3:35p leisun
 * PR 55693: removed bcrypt dependency
 * 
 * 4   7/29/09 2:07p leisun
 * PR 55693: moved DTCP constants from source to data file
 * 
 * 3   7/14/09 2:12p leisun
 * PR 55693: Fix segfault bug when switching channel using brutus
 * 
 * 2   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
 *********************************************************************************************/
/*! \file b_ecc_wrapper.c
 *  \brief wrapper for Ecliptic curve Algorithm.
 */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h"
#include "bcrypt_sha1_sw.h"

#include "openssl/bn.h"
#include "openssl/bio.h"
#include "openssl/rand.h"
#include "openssl/crypto.h"

BDBG_MODULE(b_dtcp_ip);

extern unsigned char gEccCoefficientA[];
extern unsigned char gEccCoefficientB[];
extern unsigned char gEccPrimeField[];
extern unsigned char gEccBasePointX[];
extern unsigned char gEccBasePointY[];
extern unsigned char gEccBasePointOrder[];
extern char gRngSeed[];
extern int gRngSeedSize;

/* gEccParams will be initialized by calling B_DTCP_InitEccParams */
BCRYPT_ECCParam_t gEccParams ;


unsigned char gBn160[SHA1_DIGEST_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*! \brief initialize ECC parameters and key parameters, convert from binary to string.
 *  \param[in] ghBcrypt handle to bcrypt context.
 *  \param[in,out] pEccParams pointer to ECC parameters.
 *  \retval BCRYPT_STATUS_eOk or other error code.
 */
BCRYPT_STATUS_eCode B_DTCP_InitEccParams(BCRYPT_Handle ghBcrypt, BCRYPT_ECCParam_t * pEccParams)
{
	BCRYPT_STATUS_eCode retValue;
	char * buf;
	BDBG_ASSERT(ghBcrypt);
	buf = (char *)BKNI_Malloc(6*128);
	if(buf == NULL)
	{
		BDBG_ERR(("failed to allocate memory for ecc params\n"));
		return BCRYPT_STATUS_eFAILED;
	}
	pEccParams->P = buf;
	buf += 128;
	pEccParams->A = buf;
	buf += 128;
	pEccParams->B = buf;
	buf += 128;
	pEccParams->G_x = buf;
	buf += 128;
	pEccParams->G_y = buf;
	buf += 128;
	pEccParams->N = buf;
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccPrimeField, 20, BCRYPT_String_Format_Decimal, pEccParams->P);
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccCoefficientA, 20, BCRYPT_String_Format_Decimal, pEccParams->A);
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccCoefficientB, 20, BCRYPT_String_Format_Decimal, pEccParams->B);
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccBasePointX, 20, BCRYPT_String_Format_Decimal, pEccParams->G_x);
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccBasePointY, 20, BCRYPT_String_Format_Decimal, pEccParams->G_y);
	retValue = BCrypt_ConvBinToStr(ghBcrypt, gEccBasePointOrder, 20, BCRYPT_String_Format_Decimal, pEccParams->N);
	if(retValue != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to initialize ECC params and keys \n"));
	}
	/* init RNG seed */
	RAND_seed(gRngSeed, gRngSeedSize);
	return retValue;
}
/*! \clean up ecc parameters , free resources 
 */
void B_DTCP_CleanupEccParams(BCRYPT_ECCParam_t * pEccParams)
{
	BDBG_ASSERT(pEccParams);
	BDBG_ASSERT(pEccParams->P);

	BKNI_Free(pEccParams->P);
}

/*! \brief perform modulo 2^n addtion 
 *  \param[in,out] r result.
 *  \param[in] a input
 *  \param[in] b input
 *  \param[in] m input modulo.
 *  \param[in] size size of the input/output paramaeters.
 */
BCRYPT_STATUS_eCode B_ModAdd(unsigned char * r, unsigned char * a, unsigned char * b, unsigned char * m, 
		int size_a, int size_b, int size_m)
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	BIGNUM * rb, *ab, *bb, *mb;
	BN_CTX *ctx = NULL;
	
#if 0
	/* DEBUG */
	BIO * out;
	out = BIO_new_fp(stdout, BIO_NOCLOSE);
#endif
	rb = BN_new(); ab = BN_new(); bb = BN_new(); mb = BN_new();
	BN_bin2bn(a, size_a, ab);
	BN_bin2bn(b, size_b, bb);
	BN_bin2bn(m, size_m, mb);
	/* DEBUG */
#if 0
	BN_print(out, ab); printf("\n");
	BN_print(out, bb); printf("\n");
	BN_print(out, mb); printf("\n");
#endif
	/* Can i assume this ?*/
	/*BN_bin2bn(r, size_m, rb); */
	ctx = BN_CTX_new();
	if(ctx == NULL) {
		BDBG_ERR(("NULL context\n"));
		retValue = BCRYPT_STATUS_eFAILED;
		goto out_free;
	}
	BN_mod_add(rb, ab, bb, mb, ctx);
	
	BN_bn2bin(rb, r);
	
	BN_CTX_free(ctx);
out_free:
	BN_free(rb);
	BN_free(ab);
	BN_free(bb);
	BN_free(mb);
	
	return retValue;
}
/*! brief  Utility function to compute RTT mac value(also used for OkMsg)
 *  param[in] AuthKey dtcp authentication key pointer.
 *  param[in] RttN RTT N value pointer.
 */
BCRYPT_STATUS_eCode B_DTCP_IP_ComputeRttMac( BCRYPT_Handle ghBcrypt, unsigned char * AuthKey,
		unsigned char * RttN,
		unsigned char * MacValue)
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	BCRYPT_Sha1Sw_t sha1SWIO;
	unsigned char mk[DTCP_RTT_MK_SIZE];
	unsigned char mkbuff[DTCP_RTT_MK_DATA_SIZE];

	/* Compute MK first */
	BKNI_Memcpy(mkbuff, AuthKey, DTCP_AUTH_KEY_SIZE);
	BKNI_Memcpy(&mkbuff[DTCP_AUTH_KEY_SIZE], AuthKey, DTCP_AUTH_KEY_SIZE);
	sha1SWIO.pucSrcData = mkbuff;
	sha1SWIO.ulSrcDataLenInByte = DTCP_RTT_MK_DATA_SIZE;
	sha1SWIO.binitctx = true;
	sha1SWIO.bfinal = true;
	sha1SWIO.ulctxnum = 1;
	sha1SWIO.pucDigest = &mk[0];
	if((retValue = BCrypt_Sha1Sw(ghBcrypt, &sha1SWIO)) != BCRYPT_STATUS_eOK)
	{
		return retValue;
	}
	/* Compute Mac value */
	if ((retValue = B_ModAdd(mk, mk, RttN, gBn160, DTCP_RTT_MK_SIZE, DTCP_RTT_N_SIZE, DTCP_RTT_MK_SIZE))
			!= BCRYPT_STATUS_eOK)
	{
		return retValue;
	}
	sha1SWIO.pucSrcData = &mk[0];
	sha1SWIO.ulSrcDataLenInByte = DTCP_RTT_MAC_DATA_SIZE;
	sha1SWIO.binitctx = true;
	sha1SWIO.bfinal = true;
	sha1SWIO.ulctxnum = 1;
	sha1SWIO.pucDigest = MacValue;
	if((retValue = BCrypt_Sha1Sw(ghBcrypt, &sha1SWIO)) != BCRYPT_STATUS_eOK)
	{
		return retValue;
	}

	return retValue;
}
/*! brief check two number addtion overflow
 *
 */
bool B_DTCP_IP_CheckOverFlow(unsigned char * a, unsigned char * b, int size)
{
	BIGNUM *ba, *bb, *bo;
	ba = BN_new(); bb = BN_new(); bo = BN_new();
	BN_bin2bn(a, size, ba);
	BN_bin2bn(b, size, bb);

	BN_add(bo, ba, bb);

	if (BN_num_bytes(bo) > size)
		return true;

	return false;
}
/*! brief  Utility function to compute RTT mac value for CKC
 *  param[in] AuthKey dtcp authentication key pointer.
 *  param[in] RttN RTT N value pointer.
 */
BCRYPT_STATUS_eCode B_DTCP_IP_ComputeRttMac_2( BCRYPT_Handle ghBcrypt, unsigned char * AuthKey,
		unsigned char * RttN,
		int RttN_sz,
		unsigned char * MacValue)
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	BCRYPT_Sha1Sw_t sha1SWIO;
	unsigned char mk[DTCP_RTT_MK_SIZE];
	unsigned char mkbuff[DTCP_RTT_MK_DATA_SIZE];

	/* Compute MK first */
	BKNI_Memcpy(mkbuff, AuthKey, DTCP_AUTH_KEY_SIZE);
	BKNI_Memcpy(&mkbuff[DTCP_AUTH_KEY_SIZE], AuthKey, DTCP_AUTH_KEY_SIZE);
	sha1SWIO.pucSrcData = mkbuff;
	sha1SWIO.ulSrcDataLenInByte = DTCP_RTT_MK_DATA_SIZE;
	sha1SWIO.binitctx = true;
	sha1SWIO.bfinal = true;
	sha1SWIO.ulctxnum = 1;
	sha1SWIO.pucDigest = &mk[0];
	if((retValue = BCrypt_Sha1Sw(ghBcrypt, &sha1SWIO)) != BCRYPT_STATUS_eOK)
	{
		return retValue;
	}
	/* Compute Mac value */
	if ((retValue = B_ModAdd(mk, mk, RttN, gBn160, DTCP_RTT_MK_SIZE, RttN_sz, DTCP_RTT_MK_SIZE))
			!= BCRYPT_STATUS_eOK)
	{
		return retValue;
	}
	sha1SWIO.pucSrcData = &mk[0];
	sha1SWIO.ulSrcDataLenInByte = DTCP_RTT_MAC_DATA_SIZE;
	sha1SWIO.binitctx = true;
	sha1SWIO.bfinal = true;
	sha1SWIO.ulctxnum = 1;
	sha1SWIO.pucDigest = MacValue;
	if((retValue = BCrypt_Sha1Sw(ghBcrypt, &sha1SWIO)) != BCRYPT_STATUS_eOK)
	{
		return retValue;
	}

	return retValue;
}
/*! \brief internal utility for EC_DSA
 *  extract the MSB bits of message digest that is the same number of the bits
 *  as the size of r(ecc basepoint order)
 */
static void f_msb_bits_in_r(unsigned char * f, unsigned char * r)
{

	int i = 0;
	int bits = 0, offset_bytes = 0;

	unsigned char mask = 0x80;
	for( i = 0, bits=160; i < 20; i++)
	{
		if(r[i]) {
			/* Check the "1" bits */
			while((r[i] & mask) == 0)
			{
				mask >>= 1;
				bits--;
			}break;
		}else {
			bits -= 8;
		}
	}
	offset_bytes = (160 - bits)/8;
	/* Offset bits */
	bits = 8-(bits%8);
	i = 20;
	while(i--)
	{
		f[i] = ((i >= offset_bytes)? (f[i-offset_bytes] >> bits) : 0) | 
			   ((i-1 >= offset_bytes)? (f[i-1-offset_bytes] << (8-bits)) : 0);
	}
	
}
/* Get RNG seed */
void B_GetRand(char *pRngSeed, int iRngSeedSizeInBytes)
{
	int i;

	srand((unsigned)time(NULL));

	for (i = 0; i < (iRngSeedSizeInBytes >> 2); i++)
	{
		pRngSeed[i] = rand();
	}
}
/* Generate rondom number */
BCRYPT_STATUS_eCode B_RNG(BCRYPT_Handle ghBcrypt, unsigned char * r, int len)
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	(void)ghBcrypt;
	memset(r, 0, len);
	if(RAND_bytes( r, len) != 1)
		retValue = BCRYPT_STATUS_eFAILED;
	return retValue;
}
/*
 * Generate random number in range (0, max), exclude max.
 */
BCRYPT_STATUS_eCode B_RNG_max(BCRYPT_Handle ghBcrypt, unsigned char * r, unsigned char * max, int len)
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	BIGNUM * rb, *maxb;

	(void)ghBcrypt;
	rb = BN_new(); 
	maxb = BN_new();
	BN_bin2bn(max, len, maxb);

	do {
		memset(r, 0, len);
		if(RAND_bytes(r, len ) != 1)
		{
			retValue = BCRYPT_STATUS_eFAILED;
			goto BCRYPT_P_DONE_LABEL;
		}
		BN_clear(rb);
		BN_bin2bn(r, len, rb);
	}while(BN_is_zero(rb) || BN_cmp(rb, maxb) >= 0);

BCRYPT_P_DONE_LABEL:
	BN_free(rb);
	BN_free(maxb);
	return retValue;
	
}
	
/*! \brief Get EC-DH first phase value.
 *  \param[out] pXv EC-DH first phase value.
 *  \param[out] pXk Secret information, random number.
 *  \param[in] EccParams ECC parameters.
 *  \retval BCRYPT_STATUS_eOK or other error code.
 */
BCRYPT_STATUS_eCode B_DTCP_GetFirstPhaseValue(BCRYPT_Handle ghBcrypt, unsigned char * pXv, unsigned char * pXk, BCRYPT_ECCParam_t * EccParams) 
{
	BCRYPT_ECDSASw_Multiply_t ecdsaSwIO; 
	BCRYPT_STATUS_eCode ret = BCRYPT_STATUS_eOK;
	unsigned char eccN[40];
	char pXkStr[256];
	unsigned long bytes;

	BDBG_ASSERT(ghBcrypt);
	BDBG_ASSERT(pXv);
	BDBG_ASSERT(pXk);
	BDBG_ASSERT(EccParams);
	
	BDBG_ENTER(B_DTCP_GetFirstPhaseValue);
	/*Generate a random number in [1, EccParams->N) */
#if 0
	BDBG_MSG(("ecc->n=%s\n", EccParams->N));
#endif
	if((ret = BCrypt_ConvStrToBin(ghBcrypt, EccParams->N, BCRYPT_String_Format_Decimal,
				&eccN[0], &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert str to bin for ecc->n\n"));
		return ret;
	}
				
	if((ret = B_RNG_max(ghBcrypt, pXk, eccN, DTCP_DH_FIRST_PHASE_SECRET_SIZE)) != BCRYPT_STATUS_eOK)
		return ret;
	
	memcpy(&ecdsaSwIO.eccParm, EccParams, sizeof(BCRYPT_ECCParam_t));
	if((ret = BCrypt_ConvBinToStr(ghBcrypt, pXk, DTCP_DH_FIRST_PHASE_SECRET_SIZE, 
					BCRYPT_String_Format_Decimal, pXkStr)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for pXk\n"));
		return ret;
	}
	ecdsaSwIO.scalar = pXkStr;
	ecdsaSwIO.inPoint.x = EccParams->G_x;
	ecdsaSwIO.inPoint.y = EccParams->G_y;
	
	if((ret = BCrypt_ECDSAMultiplySw(ghBcrypt, &ecdsaSwIO)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("MultiPly failed: %d\n", ret));
		goto ERR_OUT;
	}
	if((ret = BCrypt_ConvStrToBin(ghBcrypt, ecdsaSwIO.outPoint.x, BCRYPT_String_Format_Decimal,
				pXv, &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert str to bin for pXv top\n"));
		goto ERR_OUT;
	}
	if((ret = BCrypt_ConvStrToBin(ghBcrypt, ecdsaSwIO.outPoint.y, BCRYPT_String_Format_Decimal,
				pXv+DTCP_EC_DH_FIRST_PHASE_VALUE_SIZE/2, &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert str to bin for pXv bottom\n"));
		goto ERR_OUT;
	}
#if 0
	memcpy(pXv, ecdsaSwIO.outPoint.x, DTCP_DH_FIRST_PHASE_SECRET_SIZE);
	memcpy(pXv+DTCP_DH_FIRST_PHASE_SECRET_SIZE, ecdsaSwIO.outPoint.y, DTCP_DH_FIRST_PHASE_SECRET_SIZE);
#endif
	/* release buffers.*/
ERR_OUT:
	ret = BCrypt_Multiply_ECDSAClean(ghBcrypt, &ecdsaSwIO);

	BDBG_LEAVE(B_DTCP_GetFirstPhaseValue);
	return ret;

}

/*! \brief Get EC-DSA shared secret (Xk*Yv), where Yv is other device's EC-DH first phase value.
 *  \param[out] pKauth shared secret (Authentication key)
 *  \param[in]  pXk secret information.
 *  \param[in]  pYv other device's EC-DH first phase value.
 *  \param[in]  EccParams Ecc parameters.
 */
BCRYPT_STATUS_eCode B_DTCP_GetSharedSecret(BCRYPT_Handle ghBcrypt, unsigned char * pKauth, unsigned char * pXk, 
		unsigned char *pYv, BCRYPT_ECCParam_t * EccParams)
{
	unsigned long bytes;
	BCRYPT_ECDSASw_Multiply_t ecdsaSwIO; 
	BCRYPT_STATUS_eCode ret = BCRYPT_STATUS_eOK;
	char pXkStr[256];
	char pYv_xStr[256];
	char pYv_yStr[256];
	unsigned char SecretX[DTCP_DH_FIRST_PHASE_SECRET_SIZE];
	
	BDBG_ASSERT(ghBcrypt);
	BDBG_ASSERT(pKauth);
	BDBG_ASSERT(pXk);
	BDBG_ASSERT(pYv);
	BDBG_ASSERT(EccParams);

	if((ret = BCrypt_ConvBinToStr(ghBcrypt, pXk, DTCP_DH_FIRST_PHASE_SECRET_SIZE, 
					BCRYPT_String_Format_Decimal, pXkStr)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for pXk\n"));
		return ret;
	}
	if((ret = BCrypt_ConvBinToStr(ghBcrypt, pYv, DTCP_DH_FIRST_PHASE_SECRET_SIZE, 
					BCRYPT_String_Format_Decimal, pYv_xStr)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for pXk\n"));
		return ret;
	}
	if((ret = BCrypt_ConvBinToStr(ghBcrypt, pYv + DTCP_DH_FIRST_PHASE_SECRET_SIZE, 
				DTCP_DH_FIRST_PHASE_SECRET_SIZE, BCRYPT_String_Format_Decimal, pYv_yStr)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for pXk\n"));
		return ret;
	}
	memcpy(&ecdsaSwIO.eccParm, EccParams, sizeof(BCRYPT_ECCParam_t));
	ecdsaSwIO.scalar = pXkStr;
	ecdsaSwIO.inPoint.x = pYv_xStr;
	ecdsaSwIO.inPoint.y = pYv_yStr;
	if((ret = BCrypt_ECDSAMultiplySw(ghBcrypt, &ecdsaSwIO)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Multiply failed : %d \n", ret));
		goto ERR_OUT;
	}
	if((ret = BCrypt_ConvStrToBin(ghBcrypt, ecdsaSwIO.outPoint.x, BCRYPT_String_Format_Decimal,
				SecretX, &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert str to bin for pXv top\n"));
		goto ERR_OUT;
	}
	/* extract 96 bits LSB of x-coord of Xk*Yv */
	memcpy(pKauth, &SecretX[8], DTCP_AUTH_KEY_SIZE); /* Intel did this??? */
	
ERR_OUT:
	
	/* release buffers.*/
	ret = BCrypt_Multiply_ECDSAClean(ghBcrypt, &ecdsaSwIO);
	return ret;
}

/*! \brief sign data using EC-DSA algorithm.
 *  \param[out] pSignature computed signature,
 *  \param[in]  pBuffer input data to be signed.
 *  \param[in]  len input data length.
 *  \param[in]  pKey input private key.
 *  \param[in]  EccParams input ECC parameters.
 */
BCRYPT_STATUS_eCode B_DTCP_SignData(BCRYPT_Handle ghBcrypt, unsigned char * pSignature, unsigned char * pBuffer, 
		int len, char * pKey, BCRYPT_ECCParam_t * EccParams)
{
	unsigned long bytes;
	unsigned char u[DTCP_PRIVATE_KEY_SIZE];
	char ustr[256];
	unsigned char temp[20];
	unsigned char hash[ECC_BASEPOINT_ORDER_SIZE], n[40];
	BCRYPT_Sha1Sw_t Sha1Io;
	BCRYPT_ECDSASw_Sign_t ecdsaSignSwIO;
	
	BCRYPT_STATUS_eCode ret = BCRYPT_STATUS_eOK;

	BDBG_ASSERT(ghBcrypt);
	BDBG_ASSERT(pSignature);
	BDBG_ASSERT(pBuffer);
	BDBG_ASSERT(pKey);
	BDBG_ASSERT(EccParams);
#if DTCP_DEMO_MODE
	BDBG_MSG(("\n Signing data, Input Buffer:\n"));
	BDBG_BUFF(pBuffer, len );
	BDBG_MSG(("\n Signing Key:\n"));
	BDBG_BUFF((unsigned char *)pKey, DTCP_PRIVATE_KEY_SIZE );
#endif
	
	/*Generate a random number in [1, EccParams->N) */
	if((ret = B_RNG_max(ghBcrypt, &u[0], gEccBasePointOrder, DTCP_PRIVATE_KEY_SIZE)) != BCRYPT_STATUS_eOK)
		return ret;
	
	/*Compute SHA1 of input data */
	Sha1Io.pucSrcData = pBuffer;
	Sha1Io.ulSrcDataLenInByte = len;
	Sha1Io.binitctx = true;
	Sha1Io.bfinal = true;
	Sha1Io.ulctxnum = 1;
	Sha1Io.pucDigest = &hash[0];

	if((ret = BCrypt_Sha1Sw(ghBcrypt, &Sha1Io)) != BCRYPT_STATUS_eOK)
		return ret;
	if((ret = BCrypt_ConvStrToBin(ghBcrypt, EccParams->N, BCRYPT_String_Format_Decimal,
				n, &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert to bin for n\n"));
		return ret;
	}
	
	f_msb_bits_in_r(hash, n);
#if DTCP_DEMO_MODE
	BDBG_MSG(("\nHash:\n"));
	BDBG_BUFF(hash, ECC_BASEPOINT_ORDER_SIZE);
#endif
	/* initialize sign operation control structure */
	memcpy(&ecdsaSignSwIO.eccParm, EccParams, sizeof(BCRYPT_ECCParam_t));
	ecdsaSignSwIO.sig.r = NULL;
	ecdsaSignSwIO.sig.s = NULL;
	ecdsaSignSwIO.sig.format = BCRYPT_ECCSig_Format_Decimal;
	if((ret = BCrypt_ConvBinToStr(ghBcrypt, u, DTCP_PRIVATE_KEY_SIZE, BCRYPT_String_Format_Decimal, ustr))
			!= BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert binary to string for u\n"));
		return ret;
	}
	ecdsaSignSwIO.k = ustr;
	ecdsaSignSwIO.privateKey = pKey;
	ecdsaSignSwIO.hash = hash;
	ecdsaSignSwIO.hashLen = ECC_BASEPOINT_ORDER_SIZE;

	if((ret = BCrypt_ECDSASignSw(ghBcrypt, &ecdsaSignSwIO)) != BCRYPT_STATUS_eOK)
	{
		return ret;
	}
	BKNI_Memset(pSignature, 0, DTCP_SIGNATURE_SIZE);
	/* Put leading zeros , if converted bytes is less then 20, */
	BCrypt_ConvStrToBin(ghBcrypt, ecdsaSignSwIO.sig.r, BCRYPT_String_Format_Decimal, temp, &bytes);
#if DTCP_DEMO_MODE
	BDBG_MSG(("r bytes=%d \n", bytes));
#endif
	BKNI_Memcpy(pSignature + 20 - bytes, temp, bytes);
	BCrypt_ConvStrToBin(ghBcrypt, ecdsaSignSwIO.sig.s, BCRYPT_String_Format_Decimal, temp, &bytes);
#if DTCP_DEMO_MODE
	BDBG_MSG(("s bytes=%d \n", bytes));
#endif
	BKNI_Memcpy((pSignature + 20 + 20 - bytes), temp, bytes);

	ret = BCrypt_Sign_ECDSAClean(ghBcrypt, &ecdsaSignSwIO);
	
	return ret;
}
/* \brief same as above but sign data using a binary key.
 */
BCRYPT_STATUS_eCode B_DTCP_SignData_BinKey(BCRYPT_Handle ghBcrypt, unsigned char * pSignature, unsigned char * pBuffer, 
		int len, unsigned char * BinKey, BCRYPT_ECCParam_t * EccParams)
{
	BCRYPT_STATUS_eCode retValue;
	char pKeyStr[256];

	if((retValue = BCrypt_ConvBinToStr(ghBcrypt, BinKey, DTCP_PRIVATE_KEY_SIZE,
				BCRYPT_String_Format_Decimal, pKeyStr)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("failed to convert bin to str for pKeyX\n"));
		return retValue;
	}
	return B_DTCP_SignData(ghBcrypt, pSignature, pBuffer, len, pKeyStr, EccParams);
}
/*! \brief Verify data using EC-DSA algorithm.
 *  \param[out] valid 1 , signature is valid, 0 signature is invalid.
 *  \param[in] pSignature, input signature,
 *  \param[in] pBuffer input data to be verified.
 *  \param[in] len input data length.
 *  \param[in] PublicKeyX public key x component.
 *  \param[in] PublicKeyY public key y component
 *  \param[in] EccParams ECC parameters.
 */
BCRYPT_STATUS_eCode B_DTCP_VerifyData(BCRYPT_Handle ghBcrypt, int *valid, unsigned char * pSignature, unsigned char * pBuffer, 
		int len, char * PublicKeyX, char * PublicKeyY, BCRYPT_ECCParam_t * EccParams)
{
	unsigned char hash[ECC_BASEPOINT_ORDER_SIZE];
	char r[256], s[256];
	unsigned char n[256];
	unsigned long bytes;
	BCRYPT_Sha1Sw_t Sha1Io;
	BCRYPT_ECDSASw_Verify_t ecdsaVerifyIO;

	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;

	BDBG_ASSERT(ghBcrypt);
	BDBG_ASSERT(valid);
	BDBG_ASSERT(pSignature);
	BDBG_ASSERT(pBuffer);
	BDBG_ASSERT(PublicKeyX);
	BDBG_ASSERT(PublicKeyY);
	BDBG_ASSERT(EccParams);
	BDBG_ENTER(B_DTCP_VerifyData);

	memset(hash, 0, ECC_BASEPOINT_ORDER_SIZE);
	/*Compute SHA1 of input data */
	Sha1Io.pucSrcData = pBuffer;
	Sha1Io.ulSrcDataLenInByte = len;
	Sha1Io.binitctx = true;
	Sha1Io.bfinal = true;
	Sha1Io.ulctxnum = 1;
	Sha1Io.pucDigest = &hash[0];

#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("\nVerify Data input buffer\n"));
	BDBG_BUFF(pBuffer, len);
	BDBG_MSG(("\nSignature\n"));
	BDBG_BUFF(pSignature, DTCP_SIGNATURE_SIZE);
#endif
	if((retValue = BCrypt_Sha1Sw(ghBcrypt, &Sha1Io)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("BCrypt_Sha1Sw failed : %d\n", retValue));
		return retValue;
	}
	if((retValue = BCrypt_ConvStrToBin(ghBcrypt, EccParams->N, BCRYPT_String_Format_Decimal,
				n, &bytes)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert to bin for n\n"));
		return retValue;
	}
	
	f_msb_bits_in_r(hash, n);
	
	/* initialize Verify operation control structure */
	memcpy(&ecdsaVerifyIO.eccParm, EccParams, sizeof(BCRYPT_ECCParam_t));
	if((retValue = BCrypt_ConvBinToStr(ghBcrypt, pSignature, DTCP_SIGNATURE_SIZE/2,
				BCRYPT_String_Format_Decimal, r)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for r\n"));
		return retValue;
	}
	if((retValue = BCrypt_ConvBinToStr(ghBcrypt, pSignature + DTCP_SIGNATURE_SIZE/2, DTCP_SIGNATURE_SIZE/2,
				BCRYPT_String_Format_Decimal, s)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to convert bin to str for s\n"));
		return retValue;
	}
#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("r=%s\n",r));
	BDBG_MSG(("s=%s\n",s));
#endif
	ecdsaVerifyIO.sig.r = r;
	ecdsaVerifyIO.sig.s = s;
	ecdsaVerifyIO.sig.format  = BCRYPT_ECCSig_Format_Decimal;
	ecdsaVerifyIO.publicKey.x = PublicKeyX;
	ecdsaVerifyIO.publicKey.y = PublicKeyY;
	ecdsaVerifyIO.hash = hash;
	ecdsaVerifyIO.hashLen = ECC_BASEPOINT_ORDER_SIZE;

	if((retValue = BCrypt_ECDSAVerifySw(ghBcrypt, &ecdsaVerifyIO)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("ECDSA verify failed %d \n",retValue));
		return retValue;
	}
	*valid = ecdsaVerifyIO.sigIsValid;
	
	BDBG_LEAVE(B_DTCP_VerifyData);
	return retValue;
}
/* Verify data, but using a binary public key .
 */
BCRYPT_STATUS_eCode B_DTCP_VerifyData_BinKey(BCRYPT_Handle ghBcrypt, int *valid, unsigned char * pSignature, unsigned char * pBuffer, 
		int len, unsigned char * BinKey, BCRYPT_ECCParam_t * EccParams)
{
	BCRYPT_STATUS_eCode retValue;
	char pKeyX[256];
	char pKeyY[256];

	if((retValue = BCrypt_ConvBinToStr(ghBcrypt, BinKey, DTCP_PUBLIC_KEY_SIZE/2 ,
				BCRYPT_String_Format_Decimal, pKeyX)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("failed to convert bin to str for pKeyX\n"));
		return retValue;
	}
	if((retValue = BCrypt_ConvBinToStr(ghBcrypt, BinKey + DTCP_PUBLIC_KEY_SIZE/2, DTCP_PUBLIC_KEY_SIZE/2,
				BCRYPT_String_Format_Decimal, pKeyY)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("failed to convert bin to str for pKeyY\n"));
		return retValue;
	}
	return B_DTCP_VerifyData(ghBcrypt, valid, pSignature, pBuffer, len, pKeyX, pKeyY, EccParams);
	
}
/*
 * Automatically generate testing certificate.
 */
BCRYPT_STATUS_eCode B_DTCP_GenDtcpCert(BCRYPT_Handle ghBcrypt, BCRYPT_ECCParam_t *pEccParams,
		unsigned char *pDtlaPrivateKey, unsigned char *pDtlaPublicKey, unsigned char *pPrivateKey,
		unsigned char *pCert, int OverideDevID, char *devid )
{
	BCRYPT_STATUS_eCode retValue = BCRYPT_STATUS_eOK;
	unsigned char pPublicKey[DTCP_PUBLIC_KEY_SIZE];
	unsigned char msgBuf[DTCP_BASELINE_FULL_CERT_SIZE - DTCP_SIGNATURE_SIZE];
	unsigned char Signature[DTCP_SIGNATURE_SIZE];
	int valid = 0;
	
	BDBG_ASSERT(pCert);
	BDBG_ASSERT(pEccParams);
	
	BDBG_ENTER(B_DTCP_GenDtcpCert);


	/*Generate public/private key pair */
	if ((retValue = B_DTCP_GetFirstPhaseValue(ghBcrypt, pPublicKey, pPrivateKey, pEccParams))
			!= BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to generate key pair\n"));
		return retValue;
	}
	/* Fill certificate */
	pCert[0] = 0x01;	/* Baseline full */
	pCert[1] = 0x10;	/* 2nd generation SRM */
	pCert[2] = 0x00;
	pCert[2] |= 0x02;	/* AL = 1, AP = 0 */

	if (OverideDevID)
	{
		BKNI_Memcpy(pCert + 3, pPublicKey, DTCP_DEVICE_ID_SIZE);
		BKNI_Memcpy(devid, pPublicKey, DTCP_DEVICE_ID_SIZE);
	} else {
		BKNI_Memcpy(pCert + 3, devid, DTCP_DEVICE_ID_SIZE);
	}
	BKNI_Memcpy(pCert + 3 + DTCP_DEVICE_ID_SIZE,
		pPublicKey, DTCP_PUBLIC_KEY_SIZE);

	/* Sign the certificate */
	BKNI_Memcpy(msgBuf, pCert, DTCP_BASELINE_FULL_CERT_SIZE - DTCP_SIGNATURE_SIZE);
	if ((retValue = B_DTCP_SignData_BinKey(ghBcrypt, Signature, msgBuf, 
			DTCP_BASELINE_FULL_CERT_SIZE - DTCP_SIGNATURE_SIZE,
			pDtlaPrivateKey, pEccParams )) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("%s: Failed to sign device certificate\n", __FUNCTION__));
		return retValue;
	}
	if ((retValue = B_DTCP_VerifyData_BinKey(ghBcrypt, &valid, Signature,
			msgBuf, DTCP_BASELINE_FULL_CERT_SIZE - DTCP_SIGNATURE_SIZE,
			pDtlaPublicKey, pEccParams)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("%s: Failed to verify device certificate\n", __FUNCTION__));
		return retValue;
	} else if (!valid) {
		BDBG_ERR(("%s: Device certificate is invalid\n", __FUNCTION__));
		return retValue;
	}
	/* Copy signature into device certificate. */
	BKNI_Memcpy(pCert + DTCP_BASELINE_FULL_CERT_SIZE - DTCP_SIGNATURE_SIZE,
		Signature, DTCP_SIGNATURE_SIZE);
	
	BDBG_LEAVE(B_DTCP_GenDtcpCert);
	return retValue;
}
/* End of file */
