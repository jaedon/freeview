/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder.h $
 * $brcm_Revision: 92 $
 * $brcm_Date: 9/6/12 2:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/include/nexus_video_decoder.h $
 * 
 **************************************************************************/

/*  This example shows how to use external key and IV to speed up 
    crypto operation.  NOTE that external key and IV must be enabled 
    by OTP */

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "nexus_security.h"

#define DMA_BLOCK   32

#define DMA_JOBS    1
int main(void)
{
    NEXUS_PlatformSettings platformSettings;

	NEXUS_DmaHandle dma;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;
	NEXUS_Error rc;
    
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle					encKeyHandle, encKeyHandle2;
	NEXUS_SecurityAlgorithmSettings 	NexusConfig;
    NEXUS_SecurityClearKey key;

	uint8_t *pSrc, *pDest, *pDest2;
	unsigned int i;
    uint8_t keys[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t iv[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };


	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);


	/* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle) 
	{
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle2 = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle2) 
	{
        printf("\nAllocate dec keyslot failed \n");
        return 1;
    }

	/* Set up encryption key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.algorithm 			= NEXUS_SecurityAlgorithm_eAes;
	NexusConfig.algorithmVar  		= NEXUS_SecurityAlgorithmVariant_eCbc;
	NexusConfig.operation 			= NEXUS_SecurityOperation_eEncrypt;
	NexusConfig.keyDestEntryType 	= NEXUS_SecurityKeyType_eEven;

	if ( NEXUS_Security_ConfigAlgorithm(encKeyHandle, &NexusConfig)!= 0)
	{
        printf("\nConfigAlg enc keyslot failed \n");
        return 1;
    }
	/* Load key */
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, key.keySize);
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
	/* Load IV */
    key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    key.keySize = sizeof(iv);
    BKNI_Memcpy(key.keyData, iv, key.keySize);
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }

	/* Set up the second encryption key */
	/* This example enables BOTh external ket and IV. since external
       key and IV are enabled, there is no need to laod key and IV any more.
       Note that external key and IV can be enabled separately */

	NexusConfig.enableExtKey 			= true;
	NexusConfig.enableExtIv 			= true;
	if ( NEXUS_Security_ConfigAlgorithm(encKeyHandle2, &NexusConfig)!= 0)
	{
        printf("\nConfigAlg 2nd keyslot failed \n");
        return 1;
    }

	/* Open DMA handle */
	dma = NEXUS_Dma_Open(0, NULL);

	NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pSrc);
	NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pDest);
	/* Allocate 32 extra bytes for external key and IV */
	NEXUS_Memory_Allocate(DMA_BLOCK+sizeof(keys)+sizeof(iv), NULL, (void *)&pDest2); 

	memset(pSrc, 0, DMA_BLOCK*sizeof(unsigned char));
	memset(pDest, 1, DMA_BLOCK*sizeof(unsigned char));
	memset(pDest2, 0, (DMA_BLOCK+sizeof(keys)+sizeof(iv))*sizeof(unsigned char));

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks 					= 1;
	jobSettings.keySlot 					= encKeyHandle;
	jobSettings.dataFormat 					= NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr 					= pSrc;  
	blockSettings.pDestAddr 				= pDest; 
	blockSettings.blockSize 				= DMA_BLOCK;
	blockSettings.resetCrypto 				= true;
	blockSettings.scatterGatherCryptoStart 	= true;
	blockSettings.scatterGatherCryptoEnd 	= true;
	blockSettings.cached 					= true;

	rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
    

	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
		   break;
		}
		BKNI_Delay(1);
	}

	NEXUS_DmaJob_Destroy(dmaJob);


	/*  Encryption using external key and IV.  Scatter and gather must be used for 
	    external key and IV to work */

	BKNI_Memcpy(pDest2, keys+8, 8);	/* Copy key.  H and L need to be swapped */
	BKNI_Memcpy(pDest2+8, keys, 8);	
	BKNI_Memcpy(pDest2+sizeof(keys), iv+8, 8);  /* Copy IV.  H and L need to be swapped */
	BKNI_Memcpy(pDest2+sizeof(keys)+8, iv, 8);  /* Copy IV */

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks 					= 1;
	jobSettings.keySlot 					= encKeyHandle2;
	jobSettings.dataFormat 					= NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr 					= pDest2;  
	blockSettings.pDestAddr 				= pDest2; 
	blockSettings.blockSize 				= DMA_BLOCK+sizeof(keys)+sizeof(iv); /* Again, 32 extra bytes for external key and IV */
	blockSettings.resetCrypto 				= true;
	blockSettings.scatterGatherCryptoStart 	= true;
	blockSettings.scatterGatherCryptoEnd 	= true;
	blockSettings.cached 					= true;

	rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
		   break;
		}
		BKNI_Delay(1);
	}

	NEXUS_DmaJob_Destroy(dmaJob);

	/* Make sure pSrc matches pDest 2 */
	for (i=0;i<DMA_BLOCK; i++ )
	{
		if ( pDest[i] != pDest2[i+sizeof(keys)+sizeof(iv)] )
		{
			printf ("\nComparison failed at location %d\n", i);
			break;
		}
	}

	if ( i==DMA_BLOCK )
			printf ("\nTest passed\n");
	else
	{
		printf("\nSource buffer1 is: \n");
		for (i=0;i<DMA_BLOCK; i++ )
		{
			printf ("%02x ", pDest[i]);
		}	
		printf("\nDestination buffer 2 is: \n");
		for (i=0;i<(DMA_BLOCK+sizeof(keys)+sizeof(iv)); i++ )
		{
			printf ("%02x ", pDest2[i]);
		}	
	}


	NEXUS_Dma_Close(dma);


	NEXUS_Security_FreeKeySlot(encKeyHandle);
	NEXUS_Security_FreeKeySlot(encKeyHandle2);

    return 0;
}
