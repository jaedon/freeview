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
 * $brcm_Workfile: m2m_aescbc.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/20/12 12:22p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/security/m2m_aescbc.c $
 * 
 * 1   2/20/12 12:22p yili
 * SW7346-555:New sample code for AES
 * 
 * 3   4/15/11 2:52p jtna
 * SW7422-420: replace deprecated NEXUS_DmaJob_Start() with
 *  NEXUS_DmaJob_ProcessBlocks()
 * 
 * 2   4/6/09 5:30p yili
 * PR53745:Minor fix
 * 
 * 1   4/6/09 11:26a yili
 * PR53745:Adding security sample code to Nexus
 * 
 *****************************************************************************/

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
    
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle                 encKeyHandle, decKeyHandle;
    NEXUS_SecurityAlgorithmSettings     NexusConfig;
    NEXUS_SecurityClearKey key;

    uint8_t *pSrc, *pDest, *pDest2;
    unsigned int i;
    uint8_t keys[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t iv[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };


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
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!decKeyHandle) 
    {
        printf("\nAllocate dec keyslot failed \n");
        return 1;
    }

    /* Set up encryption key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
    NexusConfig.algorithm           = NEXUS_SecurityAlgorithm_eAes;
    NexusConfig.algorithmVar        = NEXUS_SecurityAlgorithmVariant_eCbc;
 	NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    NexusConfig.operation           = NEXUS_SecurityOperation_eEncrypt;
    NexusConfig.keyDestEntryType    = NEXUS_SecurityKeyType_eOdd;
	NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
	NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;

    if ( NEXUS_Security_ConfigAlgorithm(encKeyHandle, &NexusConfig)!= 0)
    {
        printf("\nConfigAlg enc keyslot failed \n");
        return 1;
    }


 	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
 	key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, key.keySize);
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }

	key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, iv, key.keySize);
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption IV failed \n");
        return 1;
    }
    /* Set up decryption key */
    NexusConfig.operation           = NEXUS_SecurityOperation_eDecrypt;
    if ( NEXUS_Security_ConfigAlgorithm(decKeyHandle, &NexusConfig)!= 0)
    {
        printf("\nConfigAlg dec keyslot failed \n");
        return 1;
    }
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
  	key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, key.keySize);
    if (NEXUS_Security_LoadClearKey(decKeyHandle, &key) != 0) {
        printf("\nLoad decryption key failed \n");
        return 1;
    }

	key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, iv, key.keySize);
    if (NEXUS_Security_LoadClearKey(decKeyHandle, &key) != 0) {
        printf("\nLoad decryption IV failed \n");
        return 1;
    }

    /* Open DMA handle */
    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pSrc);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pDest);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, (void *)&pDest2);

    /*  Encryption */
    memset(pSrc, 0x55, DMA_BLOCK*sizeof(unsigned char));

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks                   = 1;
    jobSettings.keySlot                     = encKeyHandle;
    jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = NULL;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr                  = pSrc;  
    blockSettings.pDestAddr                 = pDest; 
    blockSettings.blockSize                 = DMA_BLOCK;
    blockSettings.resetCrypto               = true;
    blockSettings.scatterGatherCryptoStart  = true;
    blockSettings.scatterGatherCryptoEnd    = true;
    blockSettings.cached                    = true;

    NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
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


    /*  Decryption */
    memset(pDest2, 0xAA, DMA_BLOCK*sizeof(unsigned char)); /* Filled a fixed pattern */

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks                   = 1;
    jobSettings.keySlot                     = decKeyHandle;
    jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = NULL;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr                  = pDest;  
    blockSettings.pDestAddr                 = pDest2; 
    blockSettings.blockSize                 = DMA_BLOCK;
    blockSettings.resetCrypto               = true;
    blockSettings.scatterGatherCryptoStart  = true;
    blockSettings.scatterGatherCryptoEnd    = true;
    blockSettings.cached                    = true;

    NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
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
        if ( pSrc[i] != pDest2[i] )
        {
            printf ("\nComparison failed at location %d\n", i);
            break;
        }
    }

    if ( i==DMA_BLOCK )
            printf ("\nTest passed\n");

#if 1
	printf ("\nSouce\n");
    for (i=0;i<DMA_BLOCK; i++ )
    {
        if ( i%16==0 )
        {
            printf ("\n");
        }
		printf("%02x ", pSrc[i]);
    }
	printf ("\n");

	printf ("\nDest\n");
    for (i=0;i<DMA_BLOCK; i++ )
    {
        if ( i%16==0 )
        {
            printf ("\n");
        }
		printf("%02x ", pDest[i]);
    }
	printf ("\n");

	printf ("\nDest\n");
    for (i=0;i<DMA_BLOCK; i++ )
    {
        if ( i%16==0 )
        {
            printf ("\n");
        }
		printf("%02x ", pDest2[i]);
    }
	printf ("\n");
#endif

    NEXUS_Dma_Close(dma);


    NEXUS_Security_FreeKeySlot(encKeyHandle);
    NEXUS_Security_FreeKeySlot(decKeyHandle);

    return 0;
}
