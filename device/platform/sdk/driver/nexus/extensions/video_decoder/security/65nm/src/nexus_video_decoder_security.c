/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_security.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/20/12 2:38p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/extensions/video_decoder/security/7125/src/nexus_video_decoder_security.c $
 * 
 * 4   6/20/12 2:38p yili
 * SW7125-136:Clean up
 * 
 * 3   1/17/12 6:15p yili
 * SW7125-136:Fixed build issue due to change in build env.
 * 
 * 2   8/5/11 12:08p yili
 * SW7125-136:include RDB files
 * 
 * 1   7/20/11 11:59a yili
 * SW7125-136:Relocate security
 * 
 * 9   9/30/10 10:40a erickson
 * SW7468-215: add BCHP_CHIP
 * 
 * 8   4/16/10 10:23a erickson
 * SW7468-215: extend BCHP_CHIP lists
 * 
 * 7   3/19/10 2:25p mphillip
 * SW7420-457: Disable guard check for video decoder heap for FW checking
 * 
 * 6   12/29/09 5:20p mphillip
 * SW7125-136: Merge additional security changes
 * 
 * 5   11/24/09 12:59p erickson
 * SW7420-457: merge security changes
 * 
 * 4   10/5/09 12:15p erickson
 * SW7405-2067: add NEXUS_VideoDecoder_P_DisableFwVerification. rename
 *  some regver structures.
 *
 * 3   5/29/09 5:48p jrubio
 * PR55232: add 7340/7342
 * 
 * 2   3/16/09 11:50a yili
 * PR52990:Add region verificaiton - Minor change to meet Nexus coding
 *  standard.
 * 
 * wince_7400/2   12/19/08 1:30p yili
 * PR42660:IPTV
 * 
 * wince_7400/1   12/16/08 5:17p ptimariu
 * firmware checking initial checkin
 * 
 *****************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_video_decoder_security.h"
#include "nexus_security_regver.h"

#include "bxvd.h"           /* xvd interface */
#include "bxvd_image.h"
#include "bchp_decode_ind_sdram_regs2_0.h"
#include "bchp_decode_cpuaux2_0.h"
#if (NEXUS_NUM_XVD_DEVICES==2)
#include "bchp_decode_ind_sdram_regs2_1.h"
#include "bchp_decode_cpuaux2_1.h"
#endif

BDBG_MODULE(nexus_decoder_security);
BDBG_OBJECT_ID(NEXUS_VideoDecoderSecurity);

#define XVD_FIRMWARE_SIZE 0x00100000
#define XVD_FIRMWARE_CODE_SIZE 0xc0000
#define TOTAL_AVD_DEVICES	(NEXUS_NUM_XVD_DEVICES)

typedef struct NEXUS_VideoDecoderSecureBootContext
{
    BREG_Handle hReg;
    unsigned int deviceID;
    void * pVirtualAddr;
    unsigned int physicalAddr;
    bool bVichRegSet;
    BMEM_Heap_Handle hFWMem;
}   NEXUS_VideoDecoderSecureBootContext;

static NEXUS_VideoDecoderSecureBootContext gBootContext[TOTAL_AVD_DEVICES];

extern void *NEXUS_Platform_P_MapMemory(
    unsigned long offset,
    unsigned long length,
    bool cached);
extern void *NEXUS_Platform_P_UnMapMemory(
    unsigned long offset,
    unsigned long length);

#define FW_IMG_SIZE 0x00100100

#define SVP_IMAGE_ContextEntry char
extern const unsigned char svp_image_avd0[];
#if (NEXUS_NUM_XVD_DEVICES==2)
extern const unsigned char svp_image_avd1[];
#endif

static const SVP_IMAGE_ContextEntry* SVP_IMAGE_FirmwareImages[BXVD_IMAGE_FirmwareID_Max] = 
{	
	NULL, /* BXVD_IMAGE_FirmwareID_eOuterELF_AVD0 */
	NULL, /* BXVD_IMAGE_FirmwareID_eInnerELF_AVD0 */
	NULL, /* BXVD_IMAGE_FirmwareID_eOuterELF_AVD1 */
	NULL, /* BXVD_IMAGE_FirmwareID_eInnerELF_AVD1 */
	(char *)svp_image_avd0,
#if (NEXUS_NUM_XVD_DEVICES==2)
	(char *)svp_image_avd1
#else
	(char *)NULL
#endif
};

void *SVP_IMAGE_Context = (void *)SVP_IMAGE_FirmwareImages;

typedef struct
{
	unsigned int offset;
	unsigned int size;
	unsigned char * pImgData;
} IMG_FILE;

typedef enum
{
	IMG_SEEK_SET,
	IMG_SEEK_END
}  IMG_SEEK;

static IMG_FILE * img_fopen(unsigned char * pData, unsigned int size)
{
	IMG_FILE * pFile;

	if ( !pData || !size )
		return NULL;

	pFile = (IMG_FILE *)BKNI_Malloc (sizeof(IMG_FILE));
	pFile->offset = 0;
	pFile->size = size;
	pFile->pImgData = pData;
	return pFile;
}

static void img_fclose (IMG_FILE * pFile)
{
	BKNI_Free (pFile);
}

static void img_fseek (IMG_FILE * pFile, unsigned int offset, IMG_SEEK seek)
{
	if ( !pFile) 	
		return;

	if ( seek == IMG_SEEK_SET )
	{
		pFile->offset = offset;
	}
	else if ( seek == IMG_SEEK_END )
	{
		pFile->offset = pFile->size - offset-1;
	}
}

static int img_fread (unsigned char * pData, unsigned int unitSize, unsigned int size, IMG_FILE * pFile)
{
	if ( !pFile || !pData || !unitSize || !size )
		return -1;

	if ( pFile->offset + size > pFile->size )
		size = pFile->size - pFile->offset;

	BKNI_Memcpy(pData, &(pFile->pImgData[pFile->offset]), unitSize*size);
	pFile->offset += size;

	return size;
	
}

typedef struct SVP_IMAGE_Container
{
	IMG_FILE *hImageFile;
	BXVD_AvdImgHdr imageHeader;
	uint8_t *pImageDataChunk;
	uint32_t uiImageDataChunkLength;
} SVP_IMAGE_Container;



static BERR_Code SVP_IMAGE_Open(void *context,
				void **image,
				unsigned image_id)
{
	IMG_FILE *hImageFile = NULL;
	SVP_IMAGE_ContextEntry *pContextEntry = NULL;
	SVP_IMAGE_Container *pImageContainer = NULL;

	BDBG_ENTER(SVP_IMAGE_Open);

	BDBG_ASSERT(context);
	BDBG_ASSERT(image);
	
	/* Validate the firmware ID range */
	BDBG_MSG(("Validating image ID range"));
	if (image_id >= BXVD_IMAGE_FirmwareID_Max)
	{
		BDBG_ERR(("Invalid image id %d", image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Validate the image referenced by firmware ID exists in the context */
	pContextEntry = ((SVP_IMAGE_ContextEntry**)context)[image_id];
	
	if (pContextEntry == NULL) {
		BDBG_ERR(("Image id %d is NULL", image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);		
	}

	/* Open the file */
	BDBG_MSG(("Opening image %s", pContextEntry));
	hImageFile = img_fopen((unsigned char *)pContextEntry, (unsigned int)FW_IMG_SIZE);
	if (hImageFile == NULL) {
		BDBG_ERR(("Error opening image %s", pContextEntry));
		return BERR_TRACE(BERR_UNKNOWN);
	}
	
	/* Allocate an image container struct */
	BDBG_MSG(("Allocating image container"));
	pImageContainer = (SVP_IMAGE_Container*) BKNI_Malloc(sizeof(SVP_IMAGE_Container));
	BKNI_Memset(pImageContainer, 0, sizeof(SVP_IMAGE_Container));

	/* Fill in the image container struct */
	pImageContainer->hImageFile = (IMG_FILE *)hImageFile;
	
	/* Read the header */
	img_fread((unsigned char *)(&(pImageContainer->imageHeader)), 1, sizeof(BXVD_AvdImgHdr), (IMG_FILE *)pImageContainer->hImageFile);

	*image = pImageContainer;

	BDBG_LEAVE(SVP_IMAGE_Open);
	return BERR_SUCCESS;
}

static BERR_Code SVP_IMAGE_Next(void *image,
				unsigned chunk,
				const void **data,
				uint16_t length)
{
	SVP_IMAGE_Container *pImageContainer = (SVP_IMAGE_Container*) image;

	BDBG_ENTER(SVP_IMAGE_Next);
	BDBG_ASSERT(image);
	BDBG_ASSERT(data);
	BSTD_UNUSED(length);

	if (chunk == 0) {
		BDBG_MSG(("Returning image chunk[%d]: %d bytes of image header", chunk, length));

		/* Validate length requested is same as our header size */
		if (length != sizeof(BXVD_AvdImgHdr)) {
			BDBG_ERR(("Incorrect image header length requested"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);			
		}
		
		/* Return a pointer to the header */
		*data = &pImageContainer->imageHeader;
	} else {	
		if (pImageContainer->pImageDataChunk == NULL) {
			/* Allocate image data chunk buffer */
			pImageContainer->pImageDataChunk = (uint8_t*) BKNI_Malloc(length);
			BKNI_Memset(pImageContainer->pImageDataChunk, 0, length);
			pImageContainer->uiImageDataChunkLength = length;
		} else if (pImageContainer->uiImageDataChunkLength != length) {
			/* Verify existing buffer is the right size */
			BDBG_ERR(("Inconsistent length detected. Specified length must be same for all chunks 1-n. You provided %d but we expected %d",
				  length,
				  pImageContainer->uiImageDataChunkLength));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		/* Seek to the requested chunk in the file. We need to
		 * make sure we offset by the header size */
		img_fseek((IMG_FILE *)pImageContainer->hImageFile, sizeof(BXVD_AvdImgHdr) + (chunk - 1)*length, IMG_SEEK_SET);
		img_fread(pImageContainer->pImageDataChunk, 1, length, (IMG_FILE *)pImageContainer->hImageFile);
		
		BDBG_MSG(("Returning image chunk[%d]: %d bytes of image data", chunk, length));
		*data = pImageContainer->pImageDataChunk;
	}

	BDBG_LEAVE(SVP_IMAGE_Next);

	return BERR_SUCCESS; 
}

static void SVP_IMAGE_Close(void *image)
{
	SVP_IMAGE_Container *pImageContainer = (SVP_IMAGE_Container*) image;
	
	/* Free the image container struct */
	BDBG_ENTER(SVP_IMAGE_Close);
	if (pImageContainer != NULL) {
		if (pImageContainer->hImageFile != NULL) {
			img_fclose((IMG_FILE *)pImageContainer->hImageFile);
		}
		if (pImageContainer->pImageDataChunk != NULL) {
			BKNI_Free((IMG_FILE *)pImageContainer->pImageDataChunk);
		}
		BKNI_Free(pImageContainer);
	}
	
	BDBG_LEAVE(SVP_IMAGE_Close);
	return;
}

static BIMG_Interface SVP_IMAGE_Interface = 
{
	SVP_IMAGE_Open,
	SVP_IMAGE_Next,
	SVP_IMAGE_Close
};

static BERR_Code SVP_AVDBoot_740x(void *pPrivateData, BXVD_AVDBootMode eAVDBootMode)
{
    NEXUS_VideoDecoderSecureBootContext * pContext = (NEXUS_VideoDecoderSecureBootContext *)pPrivateData;
	BREG_Handle hReg = pContext->hReg;
	NEXUS_SecurityCodeRegionSettings regionSetting;
	unsigned int regionID;
	BERR_Code rc;

	BDBG_ASSERT(pPrivateData);

	BDBG_ENTER(SVP_AVDBoot_740x);
	BDBG_MSG(("\n$$$$$$$$ DeviceID is %d $$$$$$$$\n", pContext->deviceID));
	BDBG_MSG(("\n$$$$$$$$ Physical address is %x $$$$$$$$\n", pContext->physicalAddr));
	BDBG_MSG(("\n$$$$$$$$ Virtual address is %x $$$$$$$$\n", pContext->pVirtualAddr));

	regionSetting.regionID = 9+pContext->deviceID;
	regionSetting.codeStartPhyAddress = pContext->physicalAddr;
	regionSetting.codeSize = XVD_FIRMWARE_CODE_SIZE;
	regionSetting.sigStartPhyAddress = pContext->physicalAddr+XVD_FIRMWARE_CODE_SIZE;
	regionSetting.sigSize = NEXUS_SecurityRegverSignatureSize_e256Bytes;
	regionSetting.codeRule = 0x200;
	regionSetting.keyID = 0x2;

	if ( (rc=NEXUS_Security_VerifyRegion (&regionSetting, &regionID)) != 0) 
	{
		BDBG_WRN(("Cannot define region for video FW"));
		return BERR_TRACE(rc);
	}

	switch(eAVDBootMode) {
	case BXVD_AVDBootMode_eNormal:
		BDBG_MSG(("Boot Mode: Normal (%d)", eAVDBootMode));
		break;

	case BXVD_AVDBootMode_eWatchdog:
		BDBG_MSG(("Boot Mode: Watchdog (%d)", eAVDBootMode));
		break;

	default:
		BDBG_MSG(("Boot Mode: Unknown (%d)", eAVDBootMode));
	}
#if (BCHP_CHIP == 7401  )
	BDBG_MSG(("Putting AVD core into debug mode"));

	BREG_Write32(hReg, 
		     BCHP_DECODE_IND_SDRAM_REGS2_REG_CPU_DBG,
		     1);
	
	BDBG_MSG(("Resetting AVD PC to 0"));
	BREG_Write32(hReg, 
		     BCHP_DECODE_CPUAUX2_CPUAUX_REG,
		     0); 
	
	BDBG_MSG(("Taking AVD core out of debug mode"));
	BREG_Write32(hReg,
		     BCHP_DECODE_IND_SDRAM_REGS2_REG_CPU_DBG,
		     0);
#elif (BCHP_CHIP == 7400 || BCHP_CHIP == 7405 || BCHP_CHIP==7335 ||BCHP_CHIP==7340 || BCHP_CHIP==7342 || BCHP_CHIP==7325 || BCHP_CHIP == 7420 || BCHP_CHIP==7125 || BCHP_CHIP == 7468 || BCHP_CHIP == 7208)
	BDBG_MSG(("Putting AVD core into debug mode"));
	if ( pContext->deviceID == 0 )
	{
#if 1/*ndef NEXUS_SECURITY_SETVICH*/
		BREG_Write32(hReg, 
				BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,
				1);
		
		BDBG_MSG(("Resetting AVD PC to 0"));
		BREG_Write32(hReg, 
				BCHP_DECODE_CPUAUX2_0_CPUAUX_REG,
				0); 
		
		BDBG_MSG(("Taking AVD core out of debug mode"));
		BREG_Write32(hReg,
				BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,
				0);
#else		
		NEXUS_Crypto_StartAVD(pContext->deviceID);
#endif
	}
#if (BCHP_CHIP != 7405 && BCHP_CHIP!= 7335 && BCHP_CHIP!= 7340 && BCHP_CHIP!= 7342 && BCHP_CHIP!= 7325 && BCHP_CHIP!= 7125 && BCHP_CHIP != 7468 && BCHP_CHIP != 7208)
	else if ( pContext->deviceID == 1 )
	{
#ifndef NEXUS_SECURITY_SETVICH
		BREG_Write32(hReg, 
				BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,
				1);
		
		BDBG_MSG(("Resetting AVD PC to 0"));
		BREG_Write32(hReg, 
				BCHP_DECODE_CPUAUX2_1_CPUAUX_REG,
				0); 
		
		BDBG_MSG(("Taking AVD core out of debug mode"));
		BREG_Write32(hReg,
				BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,
				0);
#else
		NEXUS_Crypto_StartAVD(pContext->deviceID);
#endif
	}
#endif
#endif

	BDBG_LEAVE(SVP_AVDBoot_740x);
	return BERR_SUCCESS;
}


void NEXUS_VideoDecoder_P_EnableSecurity (
	BXVD_Settings * inoutp_Settings, 
    const NEXUS_VideoDecoderSecureConfig * inp_Config,
	BMEM_Handle * hFWMem 
)
{
    unsigned long offset;
    unsigned long size;
	void * pAddr;
	BMEM_Heap_Settings mem_heap_settings;
	int index;
	BERR_Code rc;

	BDBG_ASSERT ( inoutp_Settings != NULL && inp_Config != NULL );
	BDBG_ASSERT ( inp_Config->hReg != NULL && inp_Config->hMem != NULL );

	index = inp_Config->index;
	BDBG_ASSERT(index==0 || index==1);
    NEXUS_Security_DisableRegion (9+index);
	size =    XVD_FIRMWARE_SIZE+0x100; 
	offset = XVD_PHYSICAL_ADDRESS + (XVD_FIRMWARE_SIZE + 65536)*index;
	pAddr = NEXUS_Platform_P_MapMemory(offset, size, false); /* uncached */
	BDBG_MSG (("##### Video FW memory offset is %08lx#####\n", offset));
	if ( !pAddr )
	{
		BDBG_ERR (("##### Cannot map video FW memory #####\n"));
		return;
    }

	rc = BMEM_Heap_GetDefaultSettings(&mem_heap_settings);
	if ( rc!=BERR_SUCCESS )
    {
		return ;
    }
	mem_heap_settings.uiAlignment = 0;
    mem_heap_settings.eSafetyConfig = BMEM_SafetyConfig_eFastest;
	mem_heap_settings.eBookKeeping = BMEM_BookKeeping_eSystem; /* don't put metadata into heap. Allows allocation right at offset. */
	rc = BMEM_Heap_Create(inp_Config->hMem, pAddr, offset, size, &mem_heap_settings, hFWMem);
	if ( rc!=BERR_SUCCESS )
	{
		BDBG_ERR (("##### Cannot create video FW heap #####\n"));
		return ;
	}

	inoutp_Settings->pImgInterface = &SVP_IMAGE_Interface;
	inoutp_Settings->pImgContext = SVP_IMAGE_Context;
	gBootContext[index].hReg = inp_Config->hReg;
	gBootContext[index].deviceID = index;
	gBootContext[index].pVirtualAddr = pAddr;
	gBootContext[index].physicalAddr = offset;
	gBootContext[index].bVichRegSet = false;
	gBootContext[index].hFWMem = *hFWMem;
	inoutp_Settings->pAVDBootCallbackData = &gBootContext[index];
	inoutp_Settings->pAVDBootCallback = SVP_AVDBoot_740x;

}

void NEXUS_VideoDecoder_P_DisableFwVerification (unsigned int index)
{
    BDBG_ASSERT(index==0 || index==1);
    NEXUS_Security_DisableRegion (9+index);

    BMEM_Heap_Destroy(gBootContext[index].hFWMem);
    NEXUS_Platform_P_UnmapMemory(gBootContext[index].pVirtualAddr, XVD_FIRMWARE_SIZE+0x100);
    BKNI_Memset (&gBootContext[index], 0, sizeof (NEXUS_VideoDecoderSecureBootContext));
    BDBG_MSG(("Done NEXUS_VideoDecoder_P_CleanupFwVerification\n"));

}


