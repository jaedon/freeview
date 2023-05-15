/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_dtcp_applib_priv.c $
 * $brcm_Revision: 21 $
 * $brcm_Date: 10/4/12 11:29a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_dtcp_applib_priv.c $
 * 
 * 21   10/4/12 11:29a cdisc
 * SW7425-3979: DTCP-IP streamlining
 * 
 * SW7425-3979/1   9/28/12 2:35p cdisc
 * SW7425-3979: making common DRM default path
 * 
 * 20   9/11/12 2:52p leisun
 * SWSECURITY-48: Auto-generating test keys, consolidate test/production
 *  build, make common DRM default ON
 * 
 * 19   7/20/12 2:08p leisun
 * SW7425-2894: Fix build failure for non-drm/test key mode
 * 
 * 18   5/23/12 6:07p celysee
 * SW7425-2894: Adding common drm and dtcp-ip
 * 
 * 17   5/23/12 3:07p celysee
 * SW7425-2894: Adding common drm and dtcp-ip
 * 
 * 16   5/18/12 3:41p celysee
 * SW7425-2894: dtcp-ip suppor (private)
 * 
 * 15   5/2/12 2:49p leisun
 * SW7425-2894: remove DTCP_IP_TEST flag
 * 
 * 14   5/1/12 6:07p leisun
 * SW7425-2894: Build is broken with default mode(Testing)
 * 
 * 13   4/18/12 6:16p celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 12   11/30/11 6:08p leisun
 * SWSECURITY-89: CKC fix from panasonic
 * 
 * 11   11/16/11 1:31p leisun
 * SWSECURITY-87: DTCP-IP read wrong SRM message length
 * 
 * 10   2/11/11 5:39p leisun
 * SWSECURITY-32: Updated lib to V1SE1.3 conformance.
 * 
 * 9   1/12/11 1:20p leisun
 * SWSECURITY-29: Add RESPONSE2 to DTCP-IP lib
 * 
 * 8   10/1/10 10:44a leisun
 * SW7408-108: Fix rng seed size issue
 *
 * 7   7/14/10 3:34p leisun
 * SW7420-561: production key fix
 *
 * 6   7/2/10 2:39p leisun
 * SW7420-561: Added key encryption support
 *
 * 5   7/2/10 2:12p leisun
 * SW7420-561: Added key encryption support
 *
 * 4   7/29/09 2:06p leisun
 * PR 55693: moved DTCP constants from source to data file
 *
 * 3   7/14/09 2:11p leisun
 * PR 55693: Fix segfault bug when switching channel using brutus
 *
 * 2   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 *
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "b_os_lib.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h"
#include "b_dtcp_srm.h"
#include "b_dtcp_status_codes.h"
#include "b_ecc_wrapper.h"
#include "b_dtcp_stack.h"

#include "drm_dtcp_ip.h"
#include "drm_dtcp_ip_vendor.h"
#include "drm_types.h"
#include "drm_metadata.h"
#ifdef B_DTCP_IP_LEGACY_PRODUCTION_KEY
#include "nexus_memory.h"
#include "nexus_security.h"
#include "nexus_dma.h"
#include "nexus_keyladder.h"
#include "decrypto.h"
#endif


BDBG_MODULE(b_dtcp_ip);
/* defined in b_ecc_wrappers.c */
extern BCRYPT_ECCParam_t gEccParams;

unsigned char gEccCoefficientA[20];
unsigned char gEccCoefficientB[20];
unsigned char gEccPrimeField[20];
unsigned char gEccBasePointX[20];
unsigned char gEccBasePointY[20];
unsigned char gEccBasePointOrder[20];

/* Dtla public key in binary format */
unsigned char gDtlaPublicKey[DTCP_PUBLIC_KEY_SIZE];
unsigned char gDtlaPrivateKey[DTCP_PRIVATE_KEY_SIZE];

unsigned char gCa0[] = {0xa3, 0x80, 0x6f, 0x97, 0x16, 0x35, 0xc1, 0x79, 0x6d, 0xea, 0xed, 0x24};
unsigned char gCb1[] = {0x28, 0xb5, 0x22, 0xc7, 0xcd, 0xb2, 0x2d, 0x49, 0x0e, 0xf8, 0xe3, 0xd4};
unsigned char gCb0[] = {0x7c, 0x0d, 0x87, 0xc7, 0x6c, 0x19, 0x56, 0x62, 0xf9, 0x8c, 0x14, 0xe6};
unsigned char gCc1[] = {0xe6, 0xed, 0x3e, 0x30, 0x78, 0xff, 0x0e, 0x5b, 0x3a, 0x27, 0x3c, 0xcb};
unsigned char gCc0[] = {0x45, 0xb1, 0x46, 0xa0, 0x61, 0x1a, 0x50, 0x1a, 0xc3, 0x2b, 0xfa, 0xd4};
unsigned char gCd0[] = {0xde, 0x66, 0x5a, 0xde, 0xc0, 0x15, 0x61, 0x12, 0xcf, 0x26, 0x28, 0xeb};
char gRngSeed[20];
int  gRngSeedSize = 20;

#define DTCP_RNG_SEED_FILENAME "dtcp.rng"
#define DTCP_SRM_FILENAME "dtcp.srm"
#if 0
#define DTCP_CERT_FILENAME "dtcp.crt"
#define DTCP_PVT_KEY_FILENAME "dtcp.pvk"
#endif
#define DTCP_IDU_FILENAME "dtcp.idu"
#define DTCP_DEVICE_ID_FILENAME "/var/tmp/dtcp.devid"
#define DTCP_CONSTANT_FILENAME "dtcp.const"
#define DTCP_ENC_CONSTANT_FILENAME "dtcp_enc.const"
#define DTCP_ENC_CERT_FILENAME "dtcp_enc.crt"
#define DTCP_PUBLIC_KEY_OFFSET	8
#define DTCP_DEVICE_ID_OFFSET	3

static char gSrm[DTCP_SRM_SECOND_GEN_MAX_SIZE];
static int  gSrmSize = DTCP_SRM_SECOND_GEN_MAX_SIZE;

static char gCert[DTCP_BASELINE_FULL_CERT_SIZE];
static int  gCertSize = DTCP_BASELINE_FULL_CERT_SIZE;
static char gPrivKey[DTCP_PRIVATE_KEY_SIZE];
static int  gPrivKeySize = DTCP_PRIVATE_KEY_SIZE;

static char gIDu[DTCP_DEVICE_ID_SIZE];
static int gIDuSize = DTCP_DEVICE_ID_SIZE;
static int OverrideDevID = 0;

/*! \brief utility function, read bytes from file .
 *  param[in] aFilename name of the file to be read.
 *  param[in] aDest output buffer pointer.
 *  param[in] size of the output buffer.
 */
static int DtcpAppLib_LoadBytesFromFile(const char *aFilename, char *aDest, int *aDestSize)
{
    int returnValue = BERR_SUCCESS;
    FILE *fin = NULL;
    int bytesRead = 0;

	BDBG_ASSERT(aFilename);
	BDBG_ASSERT(aDest);
	BDBG_ASSERT(aDestSize);

	if((fin = fopen(aFilename, "rb")) != NULL)
	{
		bytesRead = (int) fread(aDest, 1, (size_t) *aDestSize, fin);
		*aDestSize = bytesRead;
		fclose(fin);
		BDBG_MSG(("Read %d bytes from %s\n", bytesRead, aFilename));
	}else {
		BDBG_ERR(("Failed to open %s for reading\n", aFilename));
		returnValue = BERR_IO_ERROR;
	}
	return returnValue;
}
/*! \brief utility write buffer to a file.
 *  param[in] aFilename output file name
 *  param[in] output buffer pointer.
 *  param[in] output buffer size.
 */
static int DtcpAppLib_SaveBytesToFile(const char *aFilename, unsigned char *aSrc, unsigned int aSrcSize)
{
    int returnValue = BERR_SUCCESS;
    FILE *fout = NULL;
    int bytesWritten = 0;

	BDBG_ASSERT(aFilename);
	BDBG_ASSERT(aSrc);

	if((fout = fopen(aFilename, "wb")) != NULL)
	{
		bytesWritten = (int)fwrite(aSrc, 1, (size_t) aSrcSize, fout);
		fclose(fout);
		BDBG_MSG(("Wrote %d bytes to %s\n", bytesWritten, aFilename));
	}else {
		returnValue = BERR_IO_ERROR;
		BDBG_ERR(("Failed to write to %s\n", aFilename));
	}

	return returnValue;
}

/*! \brief load srm from file.
 */
static int DtcpAppLib_LoadSrm(char *aSrm, int *aSrmSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_SRM_SECOND_GEN_MAX_SIZE > *aSrmSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aSrmSize = DTCP_SRM_SECOND_GEN_MAX_SIZE;
        BDBG_ERR(("%s - reading file %s\n", __FUNCTION__, DTCP_SRM_FILENAME));
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_SRM_FILENAME, aSrm, aSrmSize);
    }

    BDBG_ERR(("%s - Exiting function --------------------------------", __FUNCTION__));
    return returnValue;
}

static void DtcpAppLib_ReverseBytes( char * buf, int size)
{
	int i;
	char tmp;
	for(i = 0; i < size/2; i++)
	{
		tmp = *(buf + i);
		*(buf + i) = *(buf + size -i-1);
		*(buf + size -i-1) = tmp;
	}
}
/*! \brief load Rng seed from file.
 */
static int DtcpAppLib_LoadRngSeed(char *aRngSeed, int *aRngSeedSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_AES_BLOCK_SIZE > *aRngSeedSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aRngSeedSize = 20;
        BDBG_ERR(("%s - reading file %s\n", __FUNCTION__, DTCP_RNG_SEED_FILENAME));
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_RNG_SEED_FILENAME, aRngSeed, aRngSeedSize);
        if ( returnValue != BERR_SUCCESS || DTCP_AES_BLOCK_SIZE != *aRngSeedSize)
        {
            returnValue = BERR_IO_ERROR;
        }
    }
    return returnValue;
}
#if 0
/*! \brief load certificate from file.
 */
static int DtcpAppLib_LoadCert(char *aCert, int *aCertSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_BASELINE_FULL_CERT_SIZE > *aCertSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aCertSize = DTCP_BASELINE_FULL_CERT_SIZE;
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_CERT_FILENAME, aCert, aCertSize);
    }
    return returnValue;
}
/*! \brief load private key from file.
 */
static int DtcpAppLib_LoadPrivateKey(char *aPrivateKey, int *aPrivateKeySize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_PRIVATE_KEY_SIZE > *aPrivateKeySize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aPrivateKeySize = DTCP_PRIVATE_KEY_SIZE;
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_PVT_KEY_FILENAME, aPrivateKey, aPrivateKeySize);
    }
    return returnValue;
}
#endif
static int DtcpAppLib_LoadConstants(char *aBuffer, int *aSize)
{
    int returnValue = BERR_SUCCESS;
	int size = 6*20 + DTCP_PUBLIC_KEY_SIZE + DTCP_PRIVATE_KEY_SIZE + 6*DTCP_CONTENT_KEY_CONSTANT_SIZE;
    if ( size > *aSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aSize  = size;
        BDBG_ERR(("%s - reading file %s\n", __FUNCTION__, DTCP_CONSTANT_FILENAME));
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_CONSTANT_FILENAME, aBuffer, aSize);
		if(returnValue != BERR_SUCCESS || *aSize != size)
		{
			return returnValue;
		}
		memcpy(gEccCoefficientA, aBuffer, 20);
		aBuffer += 20;
		memcpy(gEccCoefficientB, aBuffer, 20);
		aBuffer += 20;
		memcpy(gEccPrimeField, aBuffer, 20);
		aBuffer += 20;
		memcpy(gEccBasePointX, aBuffer, 20);
		aBuffer += 20;
		memcpy(gEccBasePointY, aBuffer, 20);
		aBuffer += 20;
		memcpy(gEccBasePointOrder, aBuffer, 20);
		aBuffer += 20;
		memcpy(gDtlaPublicKey, aBuffer, DTCP_PUBLIC_KEY_SIZE);
		aBuffer += DTCP_PUBLIC_KEY_SIZE;
		memcpy(gDtlaPrivateKey, aBuffer, DTCP_PRIVATE_KEY_SIZE);
		aBuffer += DTCP_PRIVATE_KEY_SIZE;
		memcpy(gCa0, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		aBuffer += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(gCb1, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		aBuffer += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(gCb0, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		aBuffer += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(gCc1, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		aBuffer += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(gCc0, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		aBuffer += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(gCd0, aBuffer, DTCP_CONTENT_KEY_CONSTANT_SIZE);
    }
    return returnValue;
}

#ifdef COMMON_DEVICE_CERT
/*! \brief load Idu from file, only for comman certificate.
 */
static int DtcpAppLib_LoadIDu(char *aIDu, int *aIDuSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_DEVICE_ID_SIZE > *aIDuSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        *aIDuSize = DTCP_DEVICE_ID_SIZE;
        returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_IDU_FILENAME, aIDu, aIDuSize);
        if ( returnValue != BERR_SUCCESS || DTCP_DEVICE_ID_SIZE != *aIDuSize)
        {
            returnValue = BERR_IO_ERROR;
        }
    }
    return returnValue;
}
#endif
#if 0
/*! \brief write RNG seed to file.
 */
static int DtcpAppLib_UpdateRngSeed(unsigned char *aRngSeed, unsigned int aRngSeedSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_AES_BLOCK_SIZE != aRngSeedSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        returnValue = DtcpAppLib_SaveBytesToFile( DTCP_RNG_SEED_FILENAME,
                                                   aRngSeed, aRngSeedSize);
    }
    return returnValue;
}
#endif
/*! \brief write srm into file.
 */
static int DtcpAppLib_UpdateSrm(unsigned char *aSrm, unsigned int aSrmSize)
{
    int returnValue = BERR_SUCCESS;

    if ( DTCP_SRM_SECOND_GEN_MAX_SIZE < aSrmSize )
    {
        returnValue = BERR_INVALID_PARAMETER;
    }
    else
    {
        returnValue = DtcpAppLib_SaveBytesToFile( DTCP_SRM_FILENAME,
                                                   aSrm, aSrmSize);
    }
    return returnValue;
}
/*! \brief initialize device parameters, read certificate and SRM from files.
 *  TODO: How is this be safe ?
 */
static int DtcpAppLib_InitializeDeviceParams(B_DeviceParams_T *aDeviceParams,
		B_DTCP_KeyFormat_T key_format)
{
    int returnValue = BERR_SUCCESS;
	char aConstants[1024];
	int ConstantSize = 1024;
	char *buf;
    drm_dtcp_ip_data_t dtcpIpData;
	gSrmSize = DTCP_SRM_SECOND_GEN_MAX_SIZE;
	gCertSize = DTCP_BASELINE_FULL_CERT_SIZE;
	gPrivKeySize = DTCP_PRIVATE_KEY_SIZE;
    /* zero data arrays */
    memset(gRngSeed, 0, gRngSeedSize);
    memset(gSrm, 0, gSrmSize);
    memset(gCert, 0, gCertSize);
    memset(gPrivKey, 0, gPrivKeySize);
    memset(gIDu, 0, gIDuSize);

	BDBG_ENTER(DtcpAppLib_InitializeDeviceParams);
	BDBG_MSG(("%s - Entered function ......................\n", __FUNCTION__));

#ifdef COMMON_DEVICE_CERT
	/* Source device is capable of processing Idus */
	aDeviceParams->capability = 1;
	if ((returnValue = DtcpAppLib_LoadIDu(gIDu, &gIDuSize)) != BERR_SUCCESS) {
		BDBG_ERR(("Failed to load IDU file\n"));
		goto error;
	}
	aDeviceParams->CommonDeviceCert = true;
    memcpy(&aDeviceParams->IDu[0], gIDu, gIDuSize);
    aDeviceParams->IDuSize = gIDuSize;
#else
	/* Source device is NOT capable of processing Idus */
	aDeviceParams->capability = 0;
	aDeviceParams->CommonDeviceCert = false;
#endif
#ifdef BRIDGE_DEVICE
    aDeviceParams->BridgeDevice = true;
#else
    aDeviceParams->BridgeDevice = false;
#endif
	if ((returnValue = DtcpAppLib_LoadSrm(gSrm, &gSrmSize)) != BERR_SUCCESS ) {
		BDBG_ERR(("Failed to load SRM file\n"));
		goto error;
	}
	aDeviceParams->Srm = (B_Srm_T *)gSrm;
	aDeviceParams->SrmSize = gSrmSize;
#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("gSrmSize=%d\n", gSrmSize));
	BDBG_MSG(("SRM header\n"));
	BDBG_BUFF((unsigned char*)aDeviceParams->Srm, sizeof(B_Srm_Header_T));
	BDBG_MSG(("CRL entries\n"));
	BDBG_BUFF((unsigned char*)aDeviceParams->Srm->Crl, B_DTCP_GetSrmLength((unsigned char *)gSrm));
#endif

	buf = &aConstants[0];


	if (key_format == B_DTCP_KeyFormat_eCommonDRM)
	{
		/* Common DRM key format support */
		if(DRM_DtcpIp_Vendor_GetData(&dtcpIpData) != Drm_Success)
		{
			BDBG_ERR(("%s - Failed to load DTCP-IP data\n", __FUNCTION__));
			goto error;
		}
		memcpy(gDtlaPublicKey, (char *)dtcpIpData.DtlaPublicKey, DRM_DTCP_IP_DTLA_PUBLIC_KEY_SIZE_IN_BYTES);

		memset(gDtlaPrivateKey, 0, DTCP_PRIVATE_KEY_SIZE);
		DtcpAppLib_ReverseBytes((char *)dtcpIpData.PrimeGF, 20);
		memcpy(gEccPrimeField, dtcpIpData.PrimeGF, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		DtcpAppLib_ReverseBytes((char *)dtcpIpData.ACoefficientCurve, 20);
		memcpy(gEccCoefficientA, dtcpIpData.ACoefficientCurve, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		DtcpAppLib_ReverseBytes((char *)dtcpIpData.BCoefficientCurve, 20);
		memcpy(gEccCoefficientB, dtcpIpData.BCoefficientCurve, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		DtcpAppLib_ReverseBytes((char *)dtcpIpData.XBasePoint, 20);
		memcpy(gEccBasePointX, dtcpIpData.XBasePoint, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		DtcpAppLib_ReverseBytes((char *)dtcpIpData.YBasePoint, 20);
		memcpy(gEccBasePointY, dtcpIpData.YBasePoint, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		DtcpAppLib_ReverseBytes((char *)dtcpIpData.OrderBasePoint, 20);
		memcpy(gEccBasePointOrder, dtcpIpData.OrderBasePoint, DRM_DTCP_IP_ECC_PARAMS_SIZE_IN_BYTES);

		memcpy(gCert, dtcpIpData.BaselineFullCert, DRM_DTCP_IP_BASELINE_FULL_CERT_SIZE_IN_BYTES);
		memcpy(gPrivKey, dtcpIpData.PrivateKey, DRM_DTCP_IP_PRIVATE_KEY_SIZE_IN_BYTES);
		memcpy(gRngSeed, dtcpIpData.RngSeed, DRM_DTCP_IP_SEED_RNG_SIZE_IN_BYTES);
	
	} else if (key_format == B_DTCP_KeyFormat_eProduction)
	{
#ifdef B_DTCP_IP_LEGACY_PRODUCTION_KEY
		/* Legacy production key format support */
		Initialize_KeySlot();
		if((returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_ENC_CONSTANT_FILENAME, aConstants, &ConstantSize))
				!= BERR_SUCCESS) {
			BDBG_ERR(("Failed to load file %s\n", DTCP_ENC_CONSTANT_FILENAME));
    		Finalize_KeySlot();
			return returnValue;
		}else if(keyladder_crypto((uint8_t *)&aConstants[0], ConstantSize)) {
			BDBG_ERR(("Failed to decrypt %s \n", DTCP_ENC_CONSTANT_FILENAME));
    		Finalize_KeySlot();
			return returnValue;
		}
		memcpy(gDtlaPublicKey, buf, DTCP_PUBLIC_KEY_SIZE);
		buf += DTCP_PUBLIC_KEY_SIZE;
		memset(gDtlaPrivateKey, 0, DTCP_PRIVATE_KEY_SIZE);
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccPrimeField, buf, 20);
		buf += 20;
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccCoefficientA, buf, 20);
		buf += 20;
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccCoefficientB, buf, 20);
		buf += 20;
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccBasePointX, buf, 20);
		buf += 20;
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccBasePointY, buf, 20);
		buf += 20;
		DtcpAppLib_ReverseBytes(buf, 20);
		memcpy(gEccBasePointOrder, buf, 20);

		ConstantSize = 1024;
		if((returnValue = DtcpAppLib_LoadBytesFromFile(DTCP_ENC_CERT_FILENAME, aConstants, &ConstantSize))
				!= BERR_SUCCESS) {
			BDBG_ERR(("Failed to load file %s\n", DTCP_ENC_CERT_FILENAME));
    		Finalize_KeySlot();
			return returnValue;
		}else if(keyladder_crypto((uint8_t*)&aConstants[0], ConstantSize)) {
			BDBG_ERR(("Failed to decrypt %s \n", DTCP_ENC_CERT_FILENAME));
    		Finalize_KeySlot();
			return returnValue;
		}

		buf = &aConstants[0];
		memcpy(gCert, buf, 88);
		buf += 88;
		memcpy(gPrivKey, buf, 20);
		buf += 408;
		memcpy(gRngSeed, buf, 20);
#else
		BDBG_ERR(("This binary was not built with Legacy production key support\n"));
		goto error;
#endif	/* B_DTCP_IP_LEGACY_PRODUCTION_KEY */
#if 0
		/* Deump out decrypted keys for debugging */
		buf = &aConstants[0];
		memcpy(buf, gEccCoefficientA, 20);
		buf += 20;
		memcpy(buf, gEccCoefficientB, 20);
		buf += 20;
		memcpy(buf, gEccPrimeField, 20);
		buf += 20;
		memcpy(buf, gEccBasePointX, 20);
		buf += 20;
		memcpy(buf, gEccBasePointY, 20);
		buf += 20;
		memcpy(buf, gEccBasePointOrder, 20);
		buf += 20;
		memcpy(buf, gDtlaPublicKey, 40);
		buf += 40;
		memcpy(buf, gDtlaPrivateKey, 20);
		buf += 20;
		memcpy(buf, gCa0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		buf += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(buf, gCb1, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		buf += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(buf, gCb0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		buf += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(buf, gCc1, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		buf += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(buf, gCc0, DTCP_CONTENT_KEY_CONSTANT_SIZE);
		buf += DTCP_CONTENT_KEY_CONSTANT_SIZE;
		memcpy(buf, gCd0, DTCP_CONTENT_KEY_CONSTANT_SIZE);

		DtcpAppLib_SaveBytesToFile("dtcp_decrypt.const", &aConstants[0], 252);
		/* crt */
		DtcpAppLib_SaveBytesToFile("dtcp_decrypt.crt", gCert, 88);
		/* pvk */
		DtcpAppLib_SaveBytesToFile("dtcp_decrypt.pvk", gPrivKey, 20);
		/* rng */
		DtcpAppLib_SaveBytesToFile("dtcp_decrypt.rng", gRngSeed, 20);
#endif
	}
	else
	{
		/* Test key format support */
		if ((returnValue = DtcpAppLib_LoadConstants(aConstants, &ConstantSize)) != BERR_SUCCESS)
		{
			BDBG_ERR(("%s - Failed to load %s \n", __FUNCTION__, DTCP_CONSTANT_FILENAME));
			goto error;
		}
       	if ((returnValue = DtcpAppLib_LoadRngSeed(gRngSeed, &gRngSeedSize)) != BERR_SUCCESS )
		{
			BDBG_ERR(("%s - Failed to load %s\n", __FUNCTION__, DTCP_RNG_SEED_FILENAME));
			goto error;
		}
		/* gCert will be populated by GenDtcpCert() function */
	}

	aDeviceParams->Cert = gCert;
	aDeviceParams->CertSize = gCertSize;
	/* Get Device public key from certificate */
	memcpy(&aDeviceParams->PublicKey, &gCert[DTCP_PUBLIC_KEY_OFFSET],DTCP_PUBLIC_KEY_SIZE);
	/* dtlaPUblickey is in binary format, gDtlaPublicKeyX and gDtlaPublicKeyY is decimal string format */
    memcpy(&aDeviceParams->dtlaPublicKey, gDtlaPublicKey, sizeof(gDtlaPublicKey));
	/* Get device private key, from private key file */
    memcpy(&aDeviceParams->PrivateKey, gPrivKey, gPrivKeySize);

error:
	BDBG_LEAVE(DtcpAppLib_InitializeDeviceParams);
	BDBG_MSG(("%s - Exiting function........................................", __FUNCTION__));
    return returnValue;
}

/*!\brief exproted library startup function,must be called before any other AKE function call.
 * \param[in] mode device mode source/sink
 * \param[in] use_pcp_ur true if use PCP_UR flag in PCP header.
 * \param[in] key_format  DTCP-IP key format, e.g. production/test.
 * \param[in] ckc_check Enable content key confirmation procedure for sink device.
 * \param[in] dump  dump extra messages for debugging.
 */
void * DtcpAppLib_Startup(B_DeviceMode_T mode, 
		bool use_pcp_ur, 
		B_DTCP_KeyFormat_T key_format,
		bool ckc_check)
{
	BERR_Code retValue;
	B_DeviceParams_T * pDeviceParams = NULL;
	B_DTCP_StackHandle_T pStack = NULL;
	char devid[DTCP_DEVICE_ID_SIZE];

	if((pDeviceParams = BKNI_Malloc(sizeof(B_DeviceParams_T)) ) == NULL)
	{
		BDBG_ERR(("Faled to allocate memory for device params: size=%d\n", sizeof(B_DeviceParams_T)));
		return NULL;
	}
	BKNI_Memset(pDeviceParams, 0, sizeof(B_DeviceParams_T));
	if (use_pcp_ur == true)
		pDeviceParams->pcp_ur_capability = 1;
	else 
		pDeviceParams->pcp_ur_capability = 0;
	pDeviceParams->Mode = mode;

	if ((retValue = DtcpAppLib_InitializeDeviceParams(pDeviceParams, key_format)) != BERR_SUCCESS )
    {
		BDBG_ERR(("Failed to initialize device params: retValue=%d\n", retValue));
		goto error1;
	}
	if ((pStack = B_DTCP_Stack_Init(pDeviceParams, &DtcpAppLib_UpdateSrm, B_StackId_eIP)) == NULL)
	{
		BDBG_ERR(("Failed to initialize DTCP-IP stack\n"));
		goto error1;
    }
	pStack->pAkeCoreData->ckc_check = ckc_check;
	/*
	 * Initialize ECC parameters and DTLA public/private key pair.
	 */
	if((retValue = B_DTCP_InitEccParams(pStack->pAkeCoreData->hBcrypt, &gEccParams)) != BCRYPT_STATUS_eOK)
	{
		BDBG_ERR(("Failed to initialize ECC params: retValue=%d\n", retValue));
		goto error2;
	}
	memcpy(&pDeviceParams->EccParams, &gEccParams, sizeof(BCRYPT_ECCParam_t));
	/*
	 * Auto generate test key, if we are running in test key mode.
	 */
	if (key_format == B_DTCP_KeyFormat_eTest)
	{
        if (DtcpAppLib_LoadBytesFromFile(DTCP_DEVICE_ID_FILENAME, devid, &gIDuSize) 
				!= BERR_SUCCESS)
		{
			OverrideDevID = 1;
		} else {
			OverrideDevID = 0;
		}

		if ((retValue = B_DTCP_GenDtcpCert(pStack->pAkeCoreData->hBcrypt, &gEccParams, 
				gDtlaPrivateKey, gDtlaPublicKey, pDeviceParams->PrivateKey, 
				(unsigned char *)gCert, OverrideDevID, devid)) != BCRYPT_STATUS_eOK)
		{
			BDBG_ERR(("Failed to generate device certificate: retValue=%d\n", retValue));
			goto error2;
		}
		/* Get Device public key from certificate */
		memcpy(&pDeviceParams->PublicKey, &gCert[DTCP_PUBLIC_KEY_OFFSET],DTCP_PUBLIC_KEY_SIZE);
		if (OverrideDevID)
		{
        	retValue = DtcpAppLib_SaveBytesToFile(DTCP_DEVICE_ID_FILENAME,
					(unsigned char *)&gCert[DTCP_DEVICE_ID_OFFSET], DTCP_DEVICE_ID_SIZE);
			if (retValue != BERR_SUCCESS)
				BDBG_WRN(("Failed to save device id file!\n"));
		}
	}
	return (void*)pStack;

error2:
	B_DTCP_Stack_UnInit(pStack);
error1:
	BKNI_Free(pDeviceParams);
    return NULL;
}
