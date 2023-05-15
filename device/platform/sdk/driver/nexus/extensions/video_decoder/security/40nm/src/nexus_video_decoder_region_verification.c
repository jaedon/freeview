/******************************************************************************
 *    (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_region_verification.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/1/11 2:35p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/extensions/video_decoder/security/7422/src/nexus_video_decoder_region_verification.c $
 * 
 * 7   7/1/11 2:35p yili
 * SW7231-272:Clean up
 * 
 * 6   6/29/11 5:18p yili
 * SW7422-404:Clean up
 * 
 * 5   6/29/11 5:10p yili
 * SW7422-404: Clean up
 * 
 * 4   4/20/11 6:28p yili
 * SW7422-404:More tests
 * 
 * 3   4/20/11 1:49p yili
 * SW7422-404:More test cases
 * 
 * 2   3/28/11 11:57a yili
 * SW7422-404:Bug fix
 * 
 * 1   3/23/11 11:21a yili
 * SW7422-404:Video decoder security
 * 
 *****************************************************************************/

#include "bxvd.h"           /* xvd interface */
#include "bxvd_image.h"
#include "bxvd_image_priv.h"
#include "nexus_video_decoder_region_verification.h"
#include "nexus_video_decoder_region_signatures.c"
#include "nexus_memory.h"
#ifdef NEXUS_VIDEO_DECODER_DUMP_FW_FILE
#include <stdio.h>
#endif


#if	(NEXUS_NUM_XVD_DEVICES)

BDBG_MODULE(nexus_decoder_security);
BDBG_OBJECT_ID(NEXUS_VideoDecoderSecurityRegionVerification);

/* AVD Region IDs */
#define NEXUS_VIDEO_FW_REGION_AVD_IL   0x0A /* AVD Inner Loop Arc region */
#define NEXUS_VIDEO_FW_REGION_AVD_OL   0x0B /* AVD Outer Loop Arc region */

/* SVD Region IDs */
#define NEXUS_VIDEO_FW_REGION_SVD_IL   0x0C /* SVD Inner Loop Arc region */
#define NEXUS_VIDEO_FW_REGION_SVD_OL   0x0D /* SVD Outer Loop Arc region */
#define NEXUS_VIDEO_FW_REGION_SVD_BL   0x0E /* SVD Base Layer Arc region */

 


typedef struct 
{
	bool bDefined;
	unsigned int regionID;
	NEXUS_SecurityRegverCPUType cpuType;
	unsigned int codeRule;
	unsigned int physicalAddr;
	unsigned int codeSize;
	unsigned char * pSignature;
}  NEXUS_VideoDecoder_CodeRegion_Info;

static NEXUS_VideoDecoder_CodeRegion_Info gRegionInfos [NEXUS_NUM_XVD_DEVICES][NEXUS_VIDEO_DECODER_SECURITY_MAX_ARCHS_PER_AVD] =
{
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_SVD
	/* SVD */
	{
		/* Outer Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_SVD_OL,
			NEXUS_SecurityRegverCPUType_eSvd,
			0x00000400,
			0,
			0,
			NULL
		},

		/* Inner Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_SVD_IL,
			NEXUS_SecurityRegverCPUType_eSvd,
			0x00000400,
			0,
			0,
			NULL
		},

		/* Base Layer */
		{
			false,
			NEXUS_VIDEO_FW_REGION_SVD_BL,
			NEXUS_SecurityRegverCPUType_eSvd,
			0x00000400,
			0,
			0,
			NULL
		},
	}
#else
	/* AVD */
	{
		/* Outer Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_AVD_OL,
			NEXUS_SecurityRegverCPUType_eAvd,
			0x00000200,
			0,
			0,
			NULL
		},

		/* Inner Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_AVD_IL,
			NEXUS_SecurityRegverCPUType_eAvd,
			0x00000200,
			0,
			0,
			NULL
		},

		/* Dummy */
		{
			false,
			0xFFFFFFFF,
			NEXUS_SecurityRegverCPUType_eAvd,
			0,
			0,
			0,
			NULL
		},
	}
#endif
#if (NEXUS_NUM_XVD_DEVICES>1)
	,

	/* AVD */
	{
		/* Outer Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_AVD_OL,
			NEXUS_SecurityRegverCPUType_eAvd,
			0x00000200,
			0,
			0,
			NULL
		},

		/* Inner Loop */
		{
			false,
			NEXUS_VIDEO_FW_REGION_AVD_IL,
			NEXUS_SecurityRegverCPUType_eAvd,
			0x00000200,
			0,
			0,
			NULL
		},

		/* Dummy */
		{
			false,
			0xFFFFFFFF,
			NEXUS_SecurityRegverCPUType_eAvd,
			0,
			0,
			0,
			NULL
		},
	}
#endif
};

NEXUS_Error Nexus_VideoDecoder_P_EnableRegionVerification (
    NEXUS_VideoDecoder_CodeRegion_Settings * pSetting
	)
{
	NEXUS_SecurityCodeRegionSettings regionSetting;
	NEXUS_VideoDecoder_CodeRegion_Info * pRegion = &gRegionInfos[pSetting->deviceID][pSetting->archID];
    NEXUS_MemoryAllocationSettings memSetting;
	BERR_Code rc = NEXUS_SUCCESS;

	if ( pRegion->pSignature==NULL )
	{
		NEXUS_Memory_GetDefaultAllocationSettings(&memSetting);
		memSetting.alignment = 32;
		if ( ( NEXUS_Memory_Allocate(256, &memSetting, (void **)&(pRegion->pSignature))!=0 ) || ((pRegion->pSignature) == NULL) )
		{
			BDBG_ERR(("Cannot define region for rave FW"));
			return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
		}
	}
		
	BKNI_Memcpy (pRegion->pSignature, &gAvdFirmwareSignatures[pSetting->deviceID][pSetting->archID][0], 256*sizeof(unsigned char));
#ifdef NEXUS_SECURITY_SVD_OL_BAD_SIGNATGURE_TEST
	if ( pRegion->regionID == NEXUS_VIDEO_FW_REGION_SVD_OL )
	{
		pRegion->pSignature[0] = ~pRegion->pSignature[0];
	}
#endif
#ifdef NEXUS_SECURITY_SVD_IL_BAD_SIGNATGURE_TEST
	if ( pRegion->regionID == NEXUS_VIDEO_FW_REGION_SVD_IL )
	{
		pRegion->pSignature[0] = ~pRegion->pSignature[0];
	}
#endif
#ifdef NEXUS_SECURITY_SVD_BL_BAD_SIGNATGURE_TEST
	if ( pRegion->regionID == NEXUS_VIDEO_FW_REGION_SVD_BL )
	{
		pRegion->pSignature[0] = ~pRegion->pSignature[0];
	}
#endif
#ifdef NEXUS_SECURITY_AVD_OL_BAD_SIGNATGURE_TEST
	if ( pRegion->regionID == NEXUS_VIDEO_FW_REGION_AVD_OL )
	{
		pRegion->pSignature[0] = ~pRegion->pSignature[0];
	}
#endif
#ifdef NEXUS_SECURITY_AVD_IL_BAD_SIGNATGURE_TEST
	if ( pRegion->regionID == NEXUS_VIDEO_FW_REGION_AVD_IL )
	{
		pRegion->pSignature[0] = ~pRegion->pSignature[0];
	}
#endif

	NEXUS_FlushCache((const void*)pRegion->pSignature, 256);

    NEXUS_Security_GetDefaultRegionSettings(&regionSetting);
	regionSetting.regionID = pRegion->regionID;
	regionSetting.codeStartPhyAddress = pSetting->physicalAddr;
	regionSetting.codeSize = pSetting->codeSize;
	regionSetting.sigStartPhyAddress = NEXUS_AddrToOffset((void *)(pRegion->pSignature));;
	regionSetting.sigSize = NEXUS_SecurityRegverSignatureSize_e256Bytes;
	regionSetting.codeRule = pRegion->codeRule;
	regionSetting.cpuType = pRegion->cpuType;
	regionSetting.keyID = NEXUS_SecurityRegverSigningKeyID_e2;
	if ( (rc=NEXUS_Security_VerifyRegion (&regionSetting, &(pRegion->regionID))) != 0) 
	{
		BDBG_ERR(("Cannot define region for video FW region %d", regionSetting.regionID));
		NEXUS_Memory_Free(pRegion->pSignature);
#if ( defined(NEXUS_SECURITY_SVD_OL_BAD_SIGNATGURE_TEST)||\
	defined (NEXUS_SECURITY_SVD_IL_BAD_SIGNATGURE_TEST) ||\
	defined (NEXUS_SECURITY_SVD_BL_BAD_SIGNATGURE_TEST) ||\
	defined (NEXUS_SECURITY_AVD_OL_BAD_SIGNATGURE_TEST) ||\
	defined (NEXUS_SECURITY_AVD_IL_BAD_SIGNATGURE_TEST) )
		BDBG_ERR(("Ignore verification error for video FW region %d", regionSetting.regionID));
		return NEXUS_SUCCESS;
#else
		return BERR_TRACE(rc);
#endif

	}

	pRegion->bDefined = true;

	return NEXUS_SUCCESS;
}

NEXUS_Error Nexus_VideoDecoder_P_EnableRegions (
    BAFL_FirmwareInfo *pstArc, 
    unsigned int deviceID
	)
{
	BERR_Code rc;

#ifdef NEXUS_SECURITY_VIDEO_NO_SVD_REGION_TEST
	if (deviceID==0)
	{
		return BERR_SUCCESS;
	}
#endif
#ifdef NEXUS_SECURITY_VIDEO_NO_AVD_REGION_TEST
	if (deviceID==1)
	{
		return BERR_SUCCESS;
	}
#endif

	while (pstArc)
	{
		NEXUS_VideoDecoder_CodeRegion_Settings regionSetting;

		regionSetting.deviceID = deviceID;
		regionSetting.archID =  pstArc->uiArcInstance;
		regionSetting.physicalAddr = NEXUS_AddrToOffset (pstArc->stCode.pStartAddress);
		regionSetting.codeSize = pstArc->stCode.uiSize;
#ifdef NEXUS_VIDEO_DECODER_DUMP_FW_FILE
		if (regionSetting.codeSize)
		{
			FILE * pFile;
			char fileName[128];
#if defined (_WIN32_WCE)
			sprintf( fileName, "\\release\\AVD_%d_%d.bin", regionSetting.deviceID, regionSetting.archID);
#else
			sprintf( fileName, "AVD_%d_%d.bin", regionSetting.deviceID, regionSetting.archID);
#endif
			pFile = fopen (fileName, "wb");
			fwrite (pstArc->stCode.pStartAddress, 1, regionSetting.codeSize, pFile);
			fclose (pFile);
		}
#endif

		BDBG_MSG(("\n$$$$$$$$ ARC is %x $$$$$$$$\n", regionSetting.archID));
		BDBG_MSG(("\n$$$$$$$$ physical address is %x $$$$$$$$\n", regionSetting.physicalAddr));
		BDBG_MSG(("\n$$$$$$$$ Code size is %x $$$$$$$$\n", regionSetting.codeSize));

		if ( regionSetting.codeSize )
		{
			NEXUS_FlushCache((const void*)(pstArc->stCode.pStartAddress), regionSetting.codeSize);
			rc = Nexus_VideoDecoder_P_EnableRegionVerification (&regionSetting);
			if ( rc != BERR_SUCCESS) 
			{
				BDBG_WRN(("AVD FW checking failed for device %d and arch %d.  \n", regionSetting.deviceID, regionSetting.archID));
				return (rc);
			}		
#ifdef NEXUS_SECURITY_VIDEO_DECODER_SVD_WRCH_TEST
			if ( deviceID==0 )
			{
				BKNI_Memset((pstArc->stCode.pStartAddress), 0, sizeof (regionSetting.codeSize));
				NEXUS_FlushCache((const void*)(pstArc->stCode.pStartAddress), regionSetting.codeSize);
			}
#endif
#ifdef NEXUS_SECURITY_VIDEO_DECODER_AVD_WRCH_TEST
			if ( deviceID==1 )
			{
				BKNI_Memset((pstArc->stCode.pStartAddress), 0, sizeof (regionSetting.codeSize));
				NEXUS_FlushCache((const void*)(pstArc->stCode.pStartAddress), regionSetting.codeSize);
			}
#endif
		}

		pstArc = pstArc->pNext;
	}

	return BERR_SUCCESS;
}


void Nexus_VideoDecoder_P_DisableRegionVerification (
    unsigned int deviceID
	)
{
	int i;

	for ( i=0; i<NEXUS_VIDEO_DECODER_SECURITY_MAX_ARCHS_PER_AVD; i++ )
	{
		NEXUS_VideoDecoder_CodeRegion_Info * pRegion = &gRegionInfos[deviceID][i];
		NEXUS_Security_DisableRegion (pRegion->regionID);
		if ( pRegion->bDefined )
		{
			if (pRegion->pSignature)
			{
				NEXUS_Memory_Free(pRegion->pSignature);
				pRegion->pSignature=NULL;
				pRegion->bDefined = false;
				pRegion->physicalAddr = 0;
				pRegion->codeSize = 0;
			}
		}
	}

}

#endif