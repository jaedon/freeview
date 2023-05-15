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

/* This example shows how to use keyladder with Cuskey+Swizzle for root key generation.
   Due to differences in HW design, the value provided in this sample only works for 
   settop chips like 740x.  For other chips, the key values may need to be changed for 
   the test to pass
*/
#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "nexus_security.h"
#include "nexus_keyladder.h"

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
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW;

	uint8_t *pSrc, *pDest, *pDest2;
	unsigned int i;
	
    uint8_t keys[16] = { 0xbe, 0xf9, 0xb0, 0x67,0x13, 0xb8, 0xbc, 0x87, 0xbc, 0xfb, 0xb2, 0x69,0x13, 0xba, 0xbe, 0x8b };
	unsigned char ucProcInForKey3[16] = { 		
		0x0f, 0x09, 0xa2, 0x06,  /*high key */  
		0x19, 0x88, 0xb6, 0x89,	    
		0x28, 0xeb, 0x90, 0x2e,   /*low key  use twice */
		0xb2, 0x36, 0x18, 0x88};

	unsigned char ucProcInKey4[16] = { 0xe4, 0x62, 0x75, 0x1b, 0x5d, 0xd4, 0xbc, 0x02, 0x27, 0x9e, 0xc9, 0x6c, 0xc8, 0x66, 0xec, 0x10 };


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
        printf("\nAllocate keyslot failed \n");
        return 1;
    }

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle2 = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle2) 
	{
        printf("\nAllocate keyslot failed \n");
        return 1;
    }

	/* Set up key for clear key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.algorithm 			= NEXUS_SecurityAlgorithm_eAes;
	NexusConfig.algorithmVar  		= NEXUS_SecurityAlgorithmVariant_eCbc;
	NexusConfig.operation 			= NEXUS_SecurityOperation_eEncrypt;
	NexusConfig.keyDestEntryType 	= NEXUS_SecurityKeyType_eOdd;

	if ( NEXUS_Security_ConfigAlgorithm(encKeyHandle, &NexusConfig)!= 0)
	{
        printf("\nConfigAlg clear keyslot failed \n");
        return 1;
    }
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
	/* Use the same value for IV in this example */
    key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption IV failed \n");
        return 1;
    }


	/* Set up key for keyladder  */
	if ( NEXUS_Security_ConfigAlgorithm(encKeyHandle2, &NexusConfig)!= 0)
	{
        printf("\nConfigAlg keyladder keyslot failed \n");
        return 1;
    }

	/* Load session key */
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 		= 0x00; 
	encryptedSessionkey.cusKeyVarH 		= 0xFF; 
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	/*encryptedSessionkey.pKeyData 		= ucProcInForKey3;*/
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(encKeyHandle2, &encryptedSessionkey) !=0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	/* Load CW */
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
    encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    encrytedCW.keySize = sizeof(ucProcInKey4); 
    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    /*encrytedCW.pKeyData = ucProcInKey4;*/
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4, encrytedCW.keySize);
    encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
    encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(encKeyHandle2, &encrytedCW))
	{
		printf("\nLoading session key failed for video ODD key\n");
		return 1;
	}

	/* Load session key for IV */
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eC; /* Use keyladder C instead */
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 		= 0x00; 
	encryptedSessionkey.cusKeyVarH 		= 0xFF; 
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	/*encryptedSessionkey.pKeyData 		= ucProcInForKey3;*/
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(encKeyHandle2, &encryptedSessionkey) !=0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	/* Load CW */
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eC;  /* Use keyladder C instead */
    encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    encrytedCW.keySize = sizeof(ucProcInKey4); 
    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eIv;
    /*encrytedCW.pKeyData = ucProcInKey4;*/
 	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4, encrytedCW.keySize);
    encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
    encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(encKeyHandle2, &encrytedCW))
	{
		printf("\nLoading session key failed for video ODD key\n");
		return 1;
	}
	/* Open DMA handle */
	dma = NEXUS_Dma_Open(0, NULL);

	NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pSrc);
	NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pDest);
	NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pDest2);

	memset(pSrc, 0, DMA_BLOCK*sizeof(unsigned char));

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


    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks 					= 1;
	jobSettings.keySlot 					= encKeyHandle2;
	jobSettings.dataFormat 					= NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr 					= pSrc;  
	blockSettings.pDestAddr 				= pDest2; 
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

	/* Make sure pSrc matches pDest 2 */
	for (i=0;i<DMA_BLOCK; i++ )
	{
		if ( pDest[i] != pDest2[i] )
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
		for (i=0;i<DMA_BLOCK; i++ )
		{
			printf ("%02x ", pDest2[i]);
		}	
	}


	NEXUS_Dma_Close(dma);


	NEXUS_Security_FreeKeySlot(encKeyHandle);
	NEXUS_Security_FreeKeySlot(encKeyHandle2);

    return 0;
}
