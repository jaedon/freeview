/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: decode_video.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/19/08 2:33p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/decode_video.c $
 * 
 * 6   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 * 
 * 5   6/7/08 1:39p jgarrett
 * PR 43318: Enabling display on DTV platforms without composite outputs.
 * 
 * 4   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 * 
 * 3   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 * 
 * 2   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 * 
******************************************************************************/
/* Nexus example app: single live video decode from an input band */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_input.h"
#include "nexus_video_window.h"
#include "nexus_message.h"
#include "nexus_memory.h"
#include "bstd.h"
#include "bkni.h"
#include "nocs1x_csd_impl.h"
#include "nocs1x_csd.h"
#include "nocs11_ctd.h"
#include "nocs1x_bsd_impl.h"
#include "nocs1x_bsd.h"
#include "nexus_otpmsp.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "bcm7346_Test_Vectors_1_x.h"
#include "nexus_dma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(NOCS);

#define ECM_DATA_OFFSET 		7

NEXUS_MessageHandle msg;
bool use_sw_keys_flag = true;
bool DSC_initialized = false;
unsigned int global_nuid;

void R2R_byteSwap(
    unsigned char *word
)
{
    unsigned char temp;
    
    temp = word[0];
    word[0] = word[3];
    word[3] = temp;
    
    temp = word[1];
    word[1] = word[2];
    word[2] = temp;
    
    return;
}


TCsdStatus R2R_CryptoOperation(
	NEXUS_KeySlotHandle keyHandle,
  	const TCsdUnsignedInt8* pxInputData, 
       TCsdUnsignedInt8* pxOutputData, 
       TCsdSize xDataSize,
       NEXUS_DmaDataFormat dmaFormat
	)
{
	NEXUS_DmaHandle dma;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;
	NEXUS_Error errCode;
	unsigned char *input_data;
	unsigned char *output_data;
#if(CFG_BIG_ENDIAN==1)
	TCsdSize i;
#endif

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

	/* Copy the input data to M2M input buffer. */
	BKNI_Memcpy((void *)input_data, (void *)pxInputData, xDataSize );

#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
            R2R_byteSwap(&(input_data[i]));
#endif

	/* Open DMA handle */
	dma = NEXUS_Dma_Open(0, NULL);

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = keyHandle;
	jobSettings.dataFormat = dmaFormat;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = input_data;  
	blockSettings.pDestAddr = output_data; 
	blockSettings.blockSize = xDataSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

	errCode = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	/* Wait for M2M Encryption to complete. */
	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
		   break;
		}
		BKNI_Delay(1);
	}

#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
            R2R_byteSwap(&(output_data[i]));
#endif

	/* Copy the xCSCData to M2M input buffer. */
	BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );

	/* Done.  Close DMA. */
/*	NEXUS_DmaJob_Destroy(dmaJob);*/
	
	NEXUS_Dma_Close(dma);

	NEXUS_Memory_Free(output_data);
	NEXUS_Memory_Free(input_data);

    	return CSD_NO_ERROR;
	

}


int R2R_1_HostKey_TDES_ECB(void)
{
	int i;	
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);

	pxR2RKeyPathHandle->initHandle = &csdInit;

	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, r2r1InEncTdesEcb, sizeof(TCsdUnsignedInt8)*8);
	
	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		csdEncryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r1HostKeyEncTdesEcb, 16, 
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			8
			);	

		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	
	}	
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r1OutEncTdesEcb, 8 ))
			printf("HostKey TDES ECB: Encryption Process Failed\n");
	else
		printf("HostKey TDES ECB: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r1InDecTdesEcb, sizeof(TCsdUnsignedInt8)*8);
	
	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		csdDecryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r1HostKeyDecTdesEcb, 16, 
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			8
			);	

		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	
	}	
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r1OutDecTdesEcb, 8 ))
		printf("HostKey TDES ECB: Decrytpion Process Failed\n");
	else
		printf("HostKey TDES ECB: Decrytpion Process Successful\n");
	

	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;
	
}

int R2R_2_HostKey_TDES_CBC(void)
{
	int i;	
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdUnsignedInt8 *dataTemp;
	TCsdUnsignedInt8 *currentIv;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);

	pxR2RKeyPathHandle->initHandle = &csdInit;

	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataTemp);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&currentIv);

	BKNI_Memcpy(dataIn, r2r2InEncTdesCbc, sizeof(TCsdUnsignedInt8)*8);
	
	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		if(i==0)
		{
			csdEncryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r2HostKeyEncTdesCbc, 16, 
				r2r2IvEncTdesCbc, 8, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				8
				);
			
			BKNI_Memcpy(dataIn, r2r2IvEncTdesCbc, sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*8);
			
		}
		else
		{
			csdEncryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r2HostKeyEncTdesCbc, 16, 
				currentIv, 8, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				8
				);	
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*8);
			
		}

		BKNI_Memcpy(currentIv, dataOut, sizeof(TCsdUnsignedInt8)*8);
				
	}	
	
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r2OutEncTdesCbc, 8 ))
			printf("HostKey TDES CBC: Encryption Process Failed\n");
	else
		printf("HostKey TDES CBC: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r2InDecTdesCbc, sizeof(TCsdUnsignedInt8)*8);
	
	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		if(i==0)
		{
			csdDecryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r2HostKeyDecTdesCbc, 16, 
				r2r2IvDecTdesCbc, 8, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				8
				);	
		}
		else
		{
			csdDecryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r2HostKeyDecTdesCbc, 16, 
				currentIv, 8, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				8
				);	
		}

		BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);

	}	
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r2OutDecTdesCbc, 8 ))
			printf("HostKey TDES CBC: Decryption Process Failed\n");
	else
		printf("HostKey TDES CBC: Decryption Process Successful\n");


	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(dataTemp);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;
	
}

int R2R_3_HostKey_AES_ECB(void)
{
	int i;	
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);

	pxR2RKeyPathHandle->initHandle = &csdInit;

	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, r2r3InEncAesEcb, sizeof(TCsdUnsignedInt8)*16);
	
	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<1000; i++)
	{
		csdEncryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_AES_128, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r3HostKeyEncAesEcb, 16, 
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			16
			);	
		
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r3OutEncAesEcb, 16 ))
			printf("HostKey AES ECB: Encryption Process Failed\n");
	else
		printf("HostKey AES ECB: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r3InDecAesEcb, sizeof(TCsdUnsignedInt8)*16);
	
	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<1000; i++)
	{
		csdDecryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_AES_128, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r3HostKeyDecAesEcb, 16, 
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			16
			);	
		
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r3OutDecAesEcb, 16 ))
			printf("HostKey AES ECB: Decryption Process Failed\n");
	else
		printf("HostKey AES ECB: Decryption Process Successful\n");	

	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);

	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;
	
}

int R2R_4_HostKey_AES_CBC(void)
{
	int i;	
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdUnsignedInt8 *dataTemp;
	TCsdUnsignedInt8 *currentIv;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);

	pxR2RKeyPathHandle->initHandle = &csdInit;

	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataOut);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataTemp);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&currentIv);

	BKNI_Memcpy(dataIn, r2r4InEncAesCbc, sizeof(TCsdUnsignedInt8)*16);
	
	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<1000; i++)
	{
		if(i==0)
		{
			csdEncryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r4HostKeyEncAesCbc, 16, 
				r2r4IvEncAesCbc, 16, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				16
				);	

			BKNI_Memcpy(dataIn, r2r4IvEncAesCbc, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}
		else
		{
			csdEncryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r4HostKeyEncAesCbc, 16, 
				currentIv, 16, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				16
				);	
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}
				
		BKNI_Memcpy(currentIv, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r4OutEncAesCbc, 16 ))
		printf("HostKey AES CBC: Encryption Process Failed\n");
	else
		printf("HostKey AES CBC: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r4InDecAesCbc, sizeof(TCsdUnsignedInt8)*16);
	
	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<1000; i++)
	{
		if(i==0)
		{
			csdDecryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r4HostKeyDecAesCbc, 16, 
				r2r4IvDecAesCbc, 16, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				16
				);	
			BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*16);		
			BKNI_Memcpy(dataIn, r2r4IvDecAesCbc, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}
		else
		{
			csdDecryptDataWithClearTextHostKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r4HostKeyDecAesCbc, 16, 
				currentIv, 16, TRUE,
				pxR2RKeyPathHandle,
				dataIn,
				dataOut,
				16
				);	
			BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}

	}
	
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut, r2r4OutDecAesCbc, 16 ))
		printf("HostKey AES CBC: Decryption Process Failed\n");
	else
		printf("HostKey AES CBC: Decryption Process Successful\n");
	
	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);

	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;
	
}

int R2R_5_ContentKey_TDES_ECB(void)
{
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus errCode; 
	TCsd4BytesVector ulValue;
	unsigned int intNuid;	
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	errCode = csdGetNuid(ulValue);

	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}

	intNuid = (unsigned int)(ulValue[0])<<24 | (unsigned int)(ulValue[1])<<16 |(unsigned int)(ulValue[2])<<8 |(unsigned int) ulValue[3];
	
	printf("NUID: %x \n", intNuid);
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	pxR2RKeyPathHandle->initHandle = &csdInit;
	
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, r2r5InEncTdesEcb, sizeof(TCsdUnsignedInt8)*8);

	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		csdEncryptDataWithSecretContentKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r5CipheredProtectingKeysEncTdesEcb, 
			r2r5CipheredContentKeyEncTdesEcb, 
			16, 
  			NULL, 0, FALSE,
  			pxR2RKeyPathHandle,
  			dataIn,
        		dataOut,
        		8
        		);
		
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	}
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r5OutEncTdesEcbTable[intNuid], 8 ))
		printf("ContentKey TDES ECB: Encryption Process Failed\n");
	else
		printf("ContentKey TDES ECB: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r5InDecTdesEcb, sizeof(TCsdUnsignedInt8)*8);

	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		csdDecryptDataWithSecretContentKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r5CipheredProtectingKeysDecTdesEcb, 
			r2r5CipheredContentKeyDecTdesEcb, 
			16, 
  			NULL, 0, FALSE,
  			pxR2RKeyPathHandle,
  			dataIn,
        		dataOut,
        		8
        		);

		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	}
	
	printf("dataOut3 = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r5OutDecTdesEcbTable[intNuid], 8 ))
		printf("ContentKey TDES ECB: Decryption Process Failed\n");
	else
		printf("ContentKey TDES ECB: Decryption Process Successful\n");


	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	

}

int R2R_6_ContentKey_TDES_CBC(void)
{
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdUnsignedInt8 *dataTemp;
	TCsdUnsignedInt8 *currentIv;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus errCode; 
	TCsd4BytesVector ulValue;
	unsigned int intNuid;
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	errCode = csdGetNuid(ulValue);

	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}

	intNuid = (unsigned int)(ulValue[0])<<24 | (unsigned int)(ulValue[1])<<16 |(unsigned int)(ulValue[2])<<8 |(unsigned int) ulValue[3];
	
	printf("NUID: %x \n", intNuid);
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	pxR2RKeyPathHandle->initHandle = &csdInit;
	
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataTemp);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&currentIv);
	
	BKNI_Memcpy(dataIn, r2r6InEncTdesCbc, sizeof(TCsdUnsignedInt8)*8);

	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		if(i==0)
		{
			csdEncryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r6CipheredProtectingKeysEncTdesCbc, 
				r2r6CipheredContentKeyEncTdesCbc, 
				16, 
		  		r2r6IvEncTdesCbc, 8, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		8
	        		);
			BKNI_Memcpy(dataIn, r2r6IvEncTdesCbc, sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*8);

			
		}
		else
		{
			csdEncryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r6CipheredProtectingKeysEncTdesCbc, 
				r2r6CipheredContentKeyEncTdesCbc, 
				16, 
		  		currentIv, 8, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		8
	        		);
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*8);
			
		}
		
		BKNI_Memcpy(currentIv, dataOut, sizeof(TCsdUnsignedInt8)*8);
	}
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r6OutEncTdesCbcTable[intNuid], 8 ))
		printf("ContentKey TDES CBC: Encryption Process Failed\n");
	else
		printf("ContentKey TDES CBC: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r6InDecTdesCbc, sizeof(TCsdUnsignedInt8)*8);
	
	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<10000; i++)
	{
		if(i==0)
		{
			csdDecryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r6CipheredProtectingKeysDecTdesCbc, 
				r2r6CipheredContentKeyDecTdesCbc, 
				16, 
		  		r2r6IvDecTdesCbc, 8, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		8
	        		);
		}
		else
		{
			csdDecryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_TDES_K1K2K1, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r6CipheredProtectingKeysDecTdesCbc, 
				r2r6CipheredContentKeyDecTdesCbc, 
				16, 
		  		currentIv, 8, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		8
	        		);
		}
		
		BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	}
	
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r6OutDecTdesCbcTable[intNuid], 8 ))
		printf("ContentKey TDES CBC: Decryption Process Failed\n");
	else
		printf("ContentKey TDES CBC: Decryption Process Successful\n");


	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);

	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	

}

int R2R_7_ContentKey_AES_ECB(void)
{
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus errCode; 
	TCsd4BytesVector ulValue;
	unsigned int intNuid;	
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	errCode = csdGetNuid(ulValue);

	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}

	intNuid = (unsigned int)(ulValue[0])<<24 | (unsigned int)(ulValue[1])<<16 |(unsigned int)(ulValue[2])<<8 |(unsigned int) ulValue[3];
	
	printf("NUID: %x \n", intNuid);
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	pxR2RKeyPathHandle->initHandle = &csdInit;
	
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, r2r7InEncAesEcb, sizeof(TCsdUnsignedInt8)*16);
	
	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");

	for(i=0;i<1000; i++)
	{
		csdEncryptDataWithSecretContentKey(
			CSD_R2R_ALGORITHM_AES_128, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r7CipheredProtectingKeysEncAesEcb, 
			r2r7CipheredContentKeyEncAesEcb, 
			16, 
	  		NULL, 0, FALSE,
	  		pxR2RKeyPathHandle,
	  		dataIn,
	        	dataOut,
	        	16
	        	);
		
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r7OutEncAesEcbTable[intNuid], 16 ))
		printf("ContentKey AES ECB: Encryption Process Failed\n");
	else
		printf("ContentKey AES ECB: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r7InDecAesEcb, sizeof(TCsdUnsignedInt8)*16);

	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");


	for(i=0;i<1000; i++)
	{
		csdDecryptDataWithSecretContentKey(
			CSD_R2R_ALGORITHM_AES_128, 
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
			r2r7CipheredProtectingKeysDecAesEcb, 
			r2r7CipheredContentKeyDecAesEcb, 
			16, 
	  		NULL, 0, FALSE,
	  		pxR2RKeyPathHandle,
	  		dataIn,
	        	dataOut,
	        	16
	        	);
				
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r7OutDecAesEcbTable[intNuid], 16 ))
		printf("ContentKey AES ECB: Decryption Process Failed\n");
	else
		printf("ContentKey AES ECB: Decryption Process Successful\n");	

	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	

}

int R2R_8_ContentKey_AES_CBC(void)
{
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdUnsignedInt8 *currentIv;
	TCsdUnsignedInt8 *dataTemp;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus errCode; 
	TCsd4BytesVector ulValue;
	unsigned int intNuid;
	
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	errCode = csdGetNuid(ulValue);

	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}

	intNuid = (unsigned int)(ulValue[0])<<24 | (unsigned int)(ulValue[1])<<16 |(unsigned int)(ulValue[2])<<8 |(unsigned int) ulValue[3];
	
	printf("NUID: %x \n", intNuid);
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	
	pxR2RKeyPathHandle->initHandle = &csdInit;
	
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataOut);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&currentIv);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataTemp);

	BKNI_Memcpy(dataIn, r2r8InEncAesCbc, sizeof(TCsdUnsignedInt8)*16);

	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");


	for(i=0;i<1000; i++)
	{
		if(i==0)
		{
			csdEncryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r8CipheredProtectingKeysEncAesCbc, 
				r2r8CipheredContentKeyEncAesCbc, 
				16, 
		  		r2r8IvEncAesCbc, 16, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		16
	        		);
			
			BKNI_Memcpy(dataIn, r2r8IvEncAesCbc, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
		}
		else
		{
			csdEncryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r8CipheredProtectingKeysEncAesCbc, 
				r2r8CipheredContentKeyEncAesCbc, 
				16, 
		  		currentIv, 16, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		16
	        		);
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}
		
		BKNI_Memcpy(currentIv, dataOut, sizeof(TCsdUnsignedInt8)*16);
	}
	
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r8OutEncAesCbcTable[intNuid], 16 ))
		printf("ContentKey AES CBC: Encryption Process Failed\n");
	else
		printf("ContentKey AES CBC: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r8InDecAesCbc, sizeof(TCsdUnsignedInt8)*16);

	printf("dataIn = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataIn+i));
	printf("\n");


	for(i=0;i<1000; i++)
	{
		if(i==0)
		{
			csdDecryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r8CipheredProtectingKeysDecAesCbc, 
				r2r8CipheredContentKeyDecAesCbc, 
				16, 
		  		r2r8IvDecAesCbc, 16, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		16
	        		);
			BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*16);		
			BKNI_Memcpy(dataIn, r2r8IvDecAesCbc, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}
		else
		{
			csdDecryptDataWithSecretContentKey(
				CSD_R2R_ALGORITHM_AES_128, 
				CSD_R2R_CRYPTO_OPERATION_MODE_CBC, 
				r2r8CipheredProtectingKeysDecAesCbc, 
				r2r8CipheredContentKeyDecAesCbc, 
				16, 
		  		currentIv, 16, TRUE,
	  			pxR2RKeyPathHandle,
	  			dataIn,
	        		dataOut,
	        		16
	        		);
			BKNI_Memcpy(currentIv, dataIn, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataIn, dataTemp, sizeof(TCsdUnsignedInt8)*16);
			BKNI_Memcpy(dataTemp, dataOut, sizeof(TCsdUnsignedInt8)*16);
			
		}

	}
	
	printf("dataOut = ");
	for( i=0; i<16; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  r2r8OutDecAesCbcTable[intNuid], 16 ))
		printf("ContentKey AES CBC: Decryption Process Failed\n");
	else
		printf("ContentKey AES CBC: Decryption Process Successful\n");

	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	

}

int Ram2RamCryptoEngineTest( void )
{
	char text_input[20];
	int select_test;

	while(1) 
	{	
		select_test = 0;

		printf("\n\n");
		printf("\nSelect Ram2Ram test to Test:\n");
	      	printf("0) Print menu\n");
		printf("1) [R2R_1] HostKey - TDES - ECB\n");
		printf("2) [R2R_2] HostKey - TDES - CBC\n");
		printf("3) [R2R_3] HostKey - AES - ECB\n");
		printf("4) [R2R_4] HostKey - AES - CBC\n");
		printf("5) [R2R_5] ContentKey - TDES - ECB\n");
		printf("6) [R2R_6] ContentKey - TDES - CBC\n");
		printf("7) [R2R_7] ContentKey - AES - ECB\n");
		printf("8) [R2R_8] ContentKey - AES - CBC\n");
		printf("97) Upper menu \n");
		printf("98) Root menu \n");		
		printf("99) Quit application\n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch( select_test )
		{
			case 0:  
				break;
			case 1:
				R2R_1_HostKey_TDES_ECB();
				break;
			case 2:
				R2R_2_HostKey_TDES_CBC();
				break;
			case 3:
				R2R_3_HostKey_AES_ECB();
				break;
			case 4:
				R2R_4_HostKey_AES_CBC();
				break;
			case 5:
				R2R_5_ContentKey_TDES_ECB();
				break;
			case 6:
				R2R_6_ContentKey_TDES_CBC();
				break;
			case 7:
				R2R_7_ContentKey_AES_ECB();
				break;
			case 8:
				R2R_8_ContentKey_AES_CBC();
				break;		
			case 97:
			case 98:
				goto BHSM_P_DONE_LABEL;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;	
		}
	}

	BHSM_P_DONE_LABEL:
	
	return 0;			

}


void dsc_message_callback(void *context, int param)
{
      	const uint8_t *buffer;
        size_t size;
        int  message_length;
    	uint8_t evenKey[16], oddKey[16];	
    	uint8_t header = 0;	
    	uint8_t keySize;	
	const TCsdDscCipheredProtectingKeys *dscCipheredProtectingKeysTable;

    	BSTD_UNUSED(param);

	while (1) 
	{
		/* wait 5 seconds */	
		if (NEXUS_Message_GetBuffer(msg, (const void **)&buffer, &size)) 
			break;
		
		if (!buffer)
			continue;

		switch(((SCsdDscAVPair *)context)->xEmi)
		{
			case CSD_Algorithm_DVB_CSA2:
				keySize = 8;
				dscCipheredProtectingKeysTable = &(dsc2CipheredProtectingKeysTable[global_nuid]);
				break;
			case CSD_Algorithm_DVB_CSA3:
				keySize = 16;
				dscCipheredProtectingKeysTable = &(dsc3CipheredProtectingKeysTable[global_nuid]);
				break;
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			case CSD_Algorithm_AES_128_ECB_CLEAR:
			case CSD_Algorithm_AES_128_CBC_CLEAR:
				keySize = 16;
				dscCipheredProtectingKeysTable = &(dsc4CipheredProtectingKeysTable[global_nuid]);
				break;
/*				
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
				keySize = 16;
				dscCipheredProtectingKeysTable = &(dsc5CipheredProtectingKeysTable[global_nuid]);
				break;
*/				
			default:
				keySize = 8;
				dscCipheredProtectingKeysTable = &(dsc2CipheredProtectingKeysTable[global_nuid]);
				break;
		}

		/* Set the IV if it is EMI 0x0022. */
		if((((SCsdDscAVPair *)context)->xEmi) == CSD_Algorithm_AES_128_CBC_CLEAR)
		{
			((SCsdDscAVPair *)context)->VideoChannelInfo.initializationVector = (unsigned char*)scrInitializationVectorAesCIplusPayload;
			((SCsdDscAVPair *)context)->AudioChannelInfo.initializationVector = (unsigned char*)scrInitializationVectorAesCIplusPayload;
		}			
		
        	/* We should always get whole PAT's because maxContiguousMessageSize is 4K */
        	message_length = size;

  		if(header != buffer[0]) 
		{
			header = buffer[0];
	
			if (header  == 0x81)
			{
				BKNI_Memcpy(&evenKey[0], (char *)buffer  + ECM_DATA_OFFSET, sizeof(uint8_t)*keySize);
				BKNI_Memcpy(&oddKey[0], (char *)buffer  + ECM_DATA_OFFSET+keySize, sizeof(uint8_t)*keySize);
			}
			else if (header == 0x80)
			{
				BKNI_Memcpy(&oddKey[0], (char *)buffer  +  ECM_DATA_OFFSET, sizeof(uint8_t)*keySize);
				BKNI_Memcpy(&evenKey[0], (char *)buffer  + ECM_DATA_OFFSET+keySize, sizeof(uint8_t)*keySize);
			}

			if(use_sw_keys_flag)
			{
				if(csdSetClearTextDscHostKeys(
					((SCsdDscAVPair *)context)->xEmi, 
					(TCsdUnsignedInt8*)&(oddKey[0]), 
					keySize,
					(TCsdUnsignedInt8*)&(evenKey[0]), 
					keySize,
					(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->VideoChannelInfo))!=CSD_NO_ERROR)
					printf("Error: csdSetClearTextDscHostKeys failed!\n"); 

				if(csdSetClearTextDscHostKeys(
					((SCsdDscAVPair *)context)->xEmi, 
					(const TCsdUnsignedInt8*)&(oddKey[0]), 
					keySize,
					(const TCsdUnsignedInt8*)&(evenKey[0]), 
					keySize,
					(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->AudioChannelInfo))!=CSD_NO_ERROR)
					printf("Error: csdSetClearTextDscHostKeys failed!\n"); 
			}
  			else
  			{
				/* Load Video key */
				if(csdSetProtectedDscContentKeys(
					((SCsdDscAVPair *)context)->xEmi,
		     			*dscCipheredProtectingKeysTable,
		  			(const TCsdUnsignedInt8*)&(oddKey[0]), 
		      			 keySize,
		 			(const TCsdUnsignedInt8*)&(evenKey[0]),
		        		keySize,
		       		(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->VideoChannelInfo)
					)!=CSD_NO_ERROR)
					printf("Error: csdSetProtectedDscContentKeys failed!\n"); 

				/* Load Audio key */
				if(csdSetProtectedDscContentKeys(
					((SCsdDscAVPair *)context)->xEmi,
		     			*dscCipheredProtectingKeysTable,
		  			(TCsdUnsignedInt8*)&(oddKey[0]), 
		      			 keySize,
		 			(TCsdUnsignedInt8*)&(evenKey[0]),
		        		keySize,
		       		(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->AudioChannelInfo)
					)!=CSD_NO_ERROR)
					printf("Error: csdSetProtectedDscContentKeys failed!\n"); 		  				
  			}
		}
		NEXUS_Message_ReadComplete(msg, message_length);
	}

	return;
}

TCsdStatus  DSC_1_TS_Crypto_Engine_Clear_Text(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInitVideo);
	status=csdInitialize(&csdInitAudio);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	ChannelInfo.VideoChannelInfo.initHandle = &csdInitVideo;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInitAudio;

	/* Bring up all modules for a platform in a default configuraiton for this platform */
    	NEXUS_Platform_GetConfiguration(&platformConfig);

    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    	/* Map a parser band to the streamer input band. */
    	parserBand = NEXUS_ParserBand_e0;
     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    	/* Open the audio and video pid channels */
    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, unscrambledServiceVideoPid, NULL);
    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, unscrambledServiceAudioPid, NULL);


    	/* Open the StcChannel to do lipsync with the PCR */
    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    	stcSettings.timebase = NEXUS_Timebase_e0;
    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
    		the audio outputs. */
    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    	videoProgram.pidChannel = videoPidChannel;
    	videoProgram.stcChannel = stcChannel;
    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    	audioProgram.pidChannel = audioPidChannel;
    	audioProgram.stcChannel = stcChannel;
	
    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

    	/* Bring up audio decoders and outputs */
    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
	
    	NEXUS_AudioOutput_AddInput(
    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
    	);
	
    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
       	NEXUS_AudioOutput_AddInput(
            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
            	);
    	else
        	NEXUS_AudioOutput_AddInput(
        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
            	);

    	/* bring up display */
    	NEXUS_Display_GetDefaultSettings(&displaySettings);

    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

    	display = NEXUS_Display_Open(0, &displaySettings);
    	window = NEXUS_VideoWindow_Open(display, 0);
	
    	#if NEXUS_NUM_COMPONENT_OUTPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    	#endif
    	#if NEXUS_NUM_SCART_INPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
    	#if NEXUS_NUM_SCART_INPUTS>1
    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
    	#endif
    	#endif
    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    	#endif
    
    	/* bring up decoder and connect to display */
    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    	windowSettings.visible = true;
    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    	/* Start Decoders */
    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
	
    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
        	/* Only pass through AC3 */
        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

    	/* Prepare XPT to recognize PMT. */
    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    	pidChannelSettings.requireMessageBuffer = true;
    	pidChannel = NEXUS_PidChannel_Open(parserBand, 0x1b, &pidChannelSettings);

	while (1) 
	{
		select_test = 0;
		printf("\n\n");
		printf("\nSelect to end test\n");
	      	printf("0) Print menu\n");
		printf("97) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				break;
			case 97:
				goto Exit_DSC_1_TS_Crypto_Engine_Clear_Text;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;			
		}
	}
    
Exit_DSC_1_TS_Crypto_Engine_Clear_Text:
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);
	NEXUS_VideoDecoder_Stop(vdecode);
	NEXUS_VideoDecoder_Close(vdecode);
	NEXUS_AudioDecoder_Close(pcmDecoder);
	NEXUS_AudioDecoder_Close(compressedDecoder);
	printf("No stream input! return..\n");

  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	csdTerminateHandle.initParam = &csdInitAudio; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 1;
}

TCsdStatus  DSC_2_TS_Crypto_Engine_DVB_CSA_v2(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_MessageSettings settings;
    	NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	TCsd4BytesVector ulValue;
	
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInitVideo);
	status=csdInitialize(&csdInitAudio);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	status = csdGetNuid(ulValue);

	if(status) 
	{
		printf("Error: can not get NUID \n"); 
	}

	global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

	ChannelInfo.VideoChannelInfo.initHandle = &csdInitVideo;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInitAudio;

request_Emi:	
	select_test = 0;
	printf("\n\n");
	printf("\n[DSC_2] Select the EMI? \n");
      	printf("[1] EMI 0x0000\n");
	fputs("Choice: ", stdout);   
	fflush(stdout);

	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%d", &select_test);

	switch(select_test)
	{
		case 1:
			ChannelInfo.xEmi = CSD_Algorithm_DVB_CSA2;
			break;
		default:
			printf("Invalid Choice.  Shall only be 1.\n");
			goto request_Emi;
			break;
	}

	while (1) 
	{
Start_DSC_2_TS_Crypto_Engine_DVB_CSA_v2:
		select_test = 0;
		printf("\n\n");
		printf("\nSelect TS Crypto Engine DVB-CSA2 key source:\n");
	      	printf("0) Print menu\n");
		printf("1) Clear key \n");	
		printf("2) Protected ContentKey \n");	
		printf("97) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				goto Start_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;
				break;
			case 1:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = true;
				break;
			case 2:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = false;
				break;
			case 97:
				goto Exit_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				goto Start_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;
				break;			
		}


		DSC_initialized = true;

		/* Bring up all modules for a platform in a default configuraiton for this platform */
	    	NEXUS_Platform_GetConfiguration(&platformConfig);

	    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
	    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

	    	/* Map a parser band to the streamer input band. */
	    	parserBand = NEXUS_ParserBand_e0;
	     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
	     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
	     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
	     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

	    	/* Open the audio and video pid channels */

		switch(select_test)
		{
			case 1:	
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceAudioPid, NULL);
				break;
			case 2:
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceAudioPid, NULL);
				break;
			default:
				goto Start_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;
				break;
		}
		
	    	/* Open the StcChannel to do lipsync with the PCR */
	    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	    	stcSettings.timebase = NEXUS_Timebase_e0;
	    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
	    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	    		the audio outputs. */
	    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	    	videoProgram.pidChannel = videoPidChannel;
	    	videoProgram.stcChannel = stcChannel;
	    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
	    	audioProgram.pidChannel = audioPidChannel;
	    	audioProgram.stcChannel = stcChannel;
		
	    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
	    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
	    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
	    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

	    	/* Bring up audio decoders and outputs */
	    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
		
	    	NEXUS_AudioOutput_AddInput(
	    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
	    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	    	);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	       	NEXUS_AudioOutput_AddInput(
	            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
	            	);
	    	else
	        	NEXUS_AudioOutput_AddInput(
	        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	            	);

	    	/* bring up display */
	    	NEXUS_Display_GetDefaultSettings(&displaySettings);

	    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
	    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

	    	display = NEXUS_Display_Open(0, &displaySettings);
	    	window = NEXUS_VideoWindow_Open(display, 0);
		
	    	#if NEXUS_NUM_COMPONENT_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	    	#endif
	    	#if NEXUS_NUM_SCART_INPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
	    	#if NEXUS_NUM_SCART_INPUTS>1
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
	    	#endif
	    	#endif
	    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
	    	#endif
	    
	    	/* bring up decoder and connect to display */
	    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

	    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
	    	windowSettings.visible = true;
	    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
	    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

	    	/* Start Decoders */
	    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
	    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	        	/* Only pass through AC3 */
	        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

	    	/* Prepare XPT to recognize PMT. */
	    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
	    	pidChannelSettings.requireMessageBuffer = true;


		switch(select_test)
		{
			case 1:	
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			case 2:
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			default:
				goto Start_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;
				break;
		}

	    	NEXUS_Message_GetDefaultSettings(&settings);
	    	settings.dataReady.callback = dsc_message_callback;
	     	settings.dataReady.context = &ChannelInfo;
	    	settings.maxContiguousMessageSize = 4096;
	    	msg = NEXUS_Message_Open(&settings);
	    	BDBG_ASSERT(msg);

	    	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
	    	startSettings.pidChannel = pidChannel;
		startSettings.psfCrcDisabled = true;

	    	/* use the default filter for any data */
	    	if (NEXUS_Message_Start(msg, &startSettings)) 
			goto Exit_DSC_2_TS_Crypto_Engine_DVB_CSA_v2;	

	}    
	
Exit_DSC_2_TS_Crypto_Engine_DVB_CSA_v2:

	if(DSC_initialized == true)
	{
		DSC_initialized = false;

		NEXUS_Message_Stop(msg);
		
		NEXUS_VideoWindow_Close(window);
		NEXUS_Display_Close(display);		
		NEXUS_VideoDecoder_Stop(vdecode);
		NEXUS_VideoDecoder_Close(vdecode);	
		NEXUS_AudioDecoder_Stop(pcmDecoder);	
		NEXUS_AudioDecoder_Stop(compressedDecoder);	
		NEXUS_AudioDecoder_Close(pcmDecoder);			
		NEXUS_AudioDecoder_Close(compressedDecoder);		
	}
	
  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	csdTerminateHandle.initParam = &csdInitAudio; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return 1;
}

TCsdStatus  DSC_3_TS_Crypto_Engine_DVB_CSA_v3(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_MessageSettings settings;
    	NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	TCsd4BytesVector ulValue;
	
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInitVideo);
	status=csdInitialize(&csdInitAudio);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	status = csdGetNuid(ulValue);

	if(status) 
	{
		printf("Error: can not get NUID \n"); 
	}

	global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

	ChannelInfo.VideoChannelInfo.initHandle = &csdInitVideo;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInitAudio;

request_Emi:	
	select_test = 0;
	printf("\n\n");
	printf("\n[DSC_3] Select the EMI? \n");
      	printf("[1] EMI 0x0001\n");
	fputs("Choice: ", stdout);   
	fflush(stdout);

	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%d", &select_test);

	switch(select_test)
	{
		case 1:
			ChannelInfo.xEmi = CSD_Algorithm_DVB_CSA3;
			break;
		default:
			printf("Invalid Choice.  Shall only be 1.\n");
			goto request_Emi;
			break;
	}

	while (1) 
	{
Start_DSC_3_TS_Crypto_Engine_DVB_CSA_v3:
		select_test = 0;
		printf("\n\n");
		printf("\nSelect TS Crypto Engine DVB-CSA3 key source:\n");
	      	printf("0) Print menu\n");
		printf("1) Clear key \n");	
		printf("2) Protected ContentKey \n");	
		printf("97) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				goto Start_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;
				break;
			case 1:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = true;
				break;
			case 2:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = false;
				break;
			case 97:
				goto Exit_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				goto Start_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;
				break;			
		}


		DSC_initialized = true;

		/* Bring up all modules for a platform in a default configuraiton for this platform */
	    	NEXUS_Platform_GetConfiguration(&platformConfig);

	    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
	    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

	    	/* Map a parser band to the streamer input band. */
	    	parserBand = NEXUS_ParserBand_e0;
	     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
	     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
	     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
	     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

	    	/* Open the audio and video pid channels */

		switch(select_test)
		{
			case 1:	
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceAudioPid, NULL);
				break;
			case 2:
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceAudioPid, NULL);
				break;
			default:
				goto Start_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;
				break;
		}
		
	    	/* Open the StcChannel to do lipsync with the PCR */
	    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	    	stcSettings.timebase = NEXUS_Timebase_e0;
	    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
	    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	    		the audio outputs. */
	    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	    	videoProgram.pidChannel = videoPidChannel;
	    	videoProgram.stcChannel = stcChannel;
	    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
	    	audioProgram.pidChannel = audioPidChannel;
	    	audioProgram.stcChannel = stcChannel;
		
	    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
	    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
	    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
	    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

	    	/* Bring up audio decoders and outputs */
	    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
		
	    	NEXUS_AudioOutput_AddInput(
	    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
	    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	    	);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	       	NEXUS_AudioOutput_AddInput(
	            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
	            	);
	    	else
	        	NEXUS_AudioOutput_AddInput(
	        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	            	);

	    	/* bring up display */
	    	NEXUS_Display_GetDefaultSettings(&displaySettings);

	    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
	    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

	    	display = NEXUS_Display_Open(0, &displaySettings);
	    	window = NEXUS_VideoWindow_Open(display, 0);
		
	    	#if NEXUS_NUM_COMPONENT_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	    	#endif
	    	#if NEXUS_NUM_SCART_INPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
	    	#if NEXUS_NUM_SCART_INPUTS>1
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
	    	#endif
	    	#endif
	    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
	    	#endif
	    
	    	/* bring up decoder and connect to display */
	    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

	    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
	    	windowSettings.visible = true;
	    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
	    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

	    	/* Start Decoders */
	    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
	    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	        	/* Only pass through AC3 */
	        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

	    	/* Prepare XPT to recognize PMT. */
	    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
	    	pidChannelSettings.requireMessageBuffer = true;


		switch(select_test)
		{
			case 1:	
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			case 2:
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			default:
				goto Start_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;
				break;
		}

	    	NEXUS_Message_GetDefaultSettings(&settings);
	    	settings.dataReady.callback = dsc_message_callback;
	     	settings.dataReady.context = &ChannelInfo;
	    	settings.maxContiguousMessageSize = 4096;
	    	msg = NEXUS_Message_Open(&settings);
	    	BDBG_ASSERT(msg);

	    	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
	    	startSettings.pidChannel = pidChannel;
		startSettings.psfCrcDisabled = true;

	    	/* use the default filter for any data */
	    	if (NEXUS_Message_Start(msg, &startSettings)) 
			goto Exit_DSC_3_TS_Crypto_Engine_DVB_CSA_v3;	

	}    
	
Exit_DSC_3_TS_Crypto_Engine_DVB_CSA_v3:

	if(DSC_initialized == true)
	{
		DSC_initialized = false;

		NEXUS_Message_Stop(msg);
		
		NEXUS_VideoWindow_Close(window);
		NEXUS_Display_Close(display);		
		NEXUS_VideoDecoder_Stop(vdecode);
		NEXUS_VideoDecoder_Close(vdecode);	
		NEXUS_AudioDecoder_Stop(pcmDecoder);	
		NEXUS_AudioDecoder_Stop(compressedDecoder);	
		NEXUS_AudioDecoder_Close(pcmDecoder);			
		NEXUS_AudioDecoder_Close(compressedDecoder);		
	}
	
  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	csdTerminateHandle.initParam = &csdInitAudio; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return 1;
}

TCsdStatus  DSC_4_TS_Crypto_Engine_Aes(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_MessageSettings settings;
    	NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	TCsd4BytesVector ulValue;
	
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInitVideo);
	status=csdInitialize(&csdInitAudio);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	status = csdGetNuid(ulValue);

	if(status) 
	{
		printf("Error: can not get NUID \n"); 
	}

	global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

	ChannelInfo.VideoChannelInfo.initHandle = &csdInitVideo;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInitAudio;

request_Emi:	
	select_test = 0;
	printf("\n\n");
	printf("\n[DSC_4] Select the EMI? \n");
      	printf("[1] EMI 0x0020\n");
      	printf("[2] EMI 0x0021\n");
      	printf("[3] EMI 0x0022\n");
	fputs("Choice: ", stdout);   
	fflush(stdout);

	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%d", &select_test);

	switch(select_test)
	{
		case 1:
			ChannelInfo.xEmi = CSD_Algorithm_AES_128_CBC_IV_0_SCTE52;
			break;
		case 2:
			ChannelInfo.xEmi = CSD_Algorithm_AES_128_ECB_CLEAR;
			break;
		case 3:
			ChannelInfo.xEmi = CSD_Algorithm_AES_128_CBC_CLEAR;
			break;
		default:
			printf("Invalid Choice.  Shall only be 1, 2, or 3.\n");
			goto request_Emi;
			break;
	}

	while (1) 
	{
Start_DSC_4_TS_Crypto_Engine_Aes:
		select_test = 0;
		printf("\n\n");
		printf("\nSelect TS Crypto Engine AES key source:\n");
	      	printf("0) Print menu\n");
		printf("1) Clear key \n");	
		printf("2) Protected ContentKey \n");	
		printf("97) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				goto Start_DSC_4_TS_Crypto_Engine_Aes;
				break;
			case 1:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = true;
				break;
			case 2:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = false;
				break;
			case 97:
				goto Exit_DSC_4_TS_Crypto_Engine_Aes;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				goto Start_DSC_4_TS_Crypto_Engine_Aes;
				break;			
		}


		DSC_initialized = true;

		/* Bring up all modules for a platform in a default configuraiton for this platform */
	    	NEXUS_Platform_GetConfiguration(&platformConfig);

	    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
	    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

	    	/* Map a parser band to the streamer input band. */
	    	parserBand = NEXUS_ParserBand_e0;
	     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
	     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
	     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
	     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

	    	/* Open the audio and video pid channels */

		switch(select_test)
		{
			case 1:	
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceAudioPid, NULL);			    	
				break;
			case 2:
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceAudioPid, NULL);
				break;
			default:
				goto Start_DSC_4_TS_Crypto_Engine_Aes;
				break;
		}
		
	    	/* Open the StcChannel to do lipsync with the PCR */
	    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	    	stcSettings.timebase = NEXUS_Timebase_e0;
	    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
	    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	    		the audio outputs. */
	    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	    	videoProgram.pidChannel = videoPidChannel;
	    	videoProgram.stcChannel = stcChannel;
	    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
	    	audioProgram.pidChannel = audioPidChannel;
	    	audioProgram.stcChannel = stcChannel;
		
	    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
	    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
	    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
	    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

	    	/* Bring up audio decoders and outputs */
	    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
		
	    	NEXUS_AudioOutput_AddInput(
	    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
	    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	    	);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	       	NEXUS_AudioOutput_AddInput(
	            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
	            	);
	    	else
	        	NEXUS_AudioOutput_AddInput(
	        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	            	);

	    	/* bring up display */
	    	NEXUS_Display_GetDefaultSettings(&displaySettings);

	    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
	    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

	    	display = NEXUS_Display_Open(0, &displaySettings);
	    	window = NEXUS_VideoWindow_Open(display, 0);
		
	    	#if NEXUS_NUM_COMPONENT_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	    	#endif
	    	#if NEXUS_NUM_SCART_INPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
	    	#if NEXUS_NUM_SCART_INPUTS>1
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
	    	#endif
	    	#endif
	    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
	    	#endif
	    
	    	/* bring up decoder and connect to display */
	    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

	    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
	    	windowSettings.visible = true;
	    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
	    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

	    	/* Start Decoders */
	    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
	    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	        	/* Only pass through AC3 */
	        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

	    	/* Prepare XPT to recognize PMT. */
	    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
	    	pidChannelSettings.requireMessageBuffer = true;

		switch(select_test)
		{
			case 1:	
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceECMPid, &pidChannelSettings);	    			
				break;
			case 2:
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			default:
				goto Start_DSC_4_TS_Crypto_Engine_Aes;
				break;
		}

	    	NEXUS_Message_GetDefaultSettings(&settings);
	    	settings.dataReady.callback = dsc_message_callback;
	     	settings.dataReady.context = &ChannelInfo;
	    	settings.maxContiguousMessageSize = 4096;
			
	    	msg = NEXUS_Message_Open(&settings);
	    	BDBG_ASSERT(msg);

	    	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);

	    	startSettings.pidChannel = pidChannel;
		startSettings.psfCrcDisabled = true;

	    	/* use the default filter for any data */
	    	if (NEXUS_Message_Start(msg, &startSettings)) 
			goto Exit_DSC_4_TS_Crypto_Engine_Aes;	

	}    
	
Exit_DSC_4_TS_Crypto_Engine_Aes:

	if(DSC_initialized == true)
	{
		DSC_initialized = false;

		NEXUS_Message_Stop(msg);
		
		NEXUS_VideoWindow_Close(window);
		NEXUS_Display_Close(display);		
		NEXUS_VideoDecoder_Stop(vdecode);
		NEXUS_VideoDecoder_Close(vdecode);	
		NEXUS_AudioDecoder_Stop(pcmDecoder);	
		NEXUS_AudioDecoder_Stop(compressedDecoder);	
		NEXUS_AudioDecoder_Close(pcmDecoder);	
		NEXUS_AudioDecoder_Close(compressedDecoder);
	}
	
  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	csdTerminateHandle.initParam = &csdInitAudio; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return 1;
}

TCsdStatus  DSC_5_TS_Crypto_Engine_Tdes(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_MessageSettings settings;
    	NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	TCsd4BytesVector ulValue;
	
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInitVideo);
	status=csdInitialize(&csdInitAudio);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	status = csdGetNuid(ulValue);

	if(status) 
	{
		printf("Error: can not get NUID \n"); 
	}

	global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

	ChannelInfo.VideoChannelInfo.initHandle = &csdInitVideo;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInitAudio;

request_Emi:	
	select_test = 0;
	printf("\n\n");
	printf("\n[DSC_5] Select the EMI? \n");
      	printf("[1] EMI 0x0040\n");
      	printf("[2] EMI 0x0041\n");
	fputs("Choice: ", stdout);   
	fflush(stdout);

	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%d", &select_test);

	switch(select_test)
	{
		case 1:
			ChannelInfo.xEmi = CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52;
			break;
		case 2:
			ChannelInfo.xEmi = CSD_Algorithm_TDES_ABA_ECB_CLEAR;
			break;
		default:
			printf("Invalid Choice.  Shall only be 1 or 2.\n");
			goto request_Emi;
			break;
	}

	while (1) 
	{
Start_DSC_5_TS_Crypto_Engine_Tdes:
		select_test = 0;
		printf("\n\n");
		printf("\nSelect TS Crypto Engine TDES key source:\n");
	      	printf("0) Print menu\n");
		printf("1) Clear key \n");	
		printf("2) Protected ContentKey \n");	
		printf("97) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				goto Start_DSC_5_TS_Crypto_Engine_Tdes;
				break;
			case 1:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = true;
				break;
			case 2:
				if(DSC_initialized)
				{
					NEXUS_Message_Stop(msg);
					NEXUS_VideoWindow_Close(window);
					NEXUS_Display_Close(display);		
					NEXUS_VideoDecoder_Stop(vdecode);
					NEXUS_VideoDecoder_Close(vdecode);	
					NEXUS_AudioDecoder_Stop(pcmDecoder);	
					NEXUS_AudioDecoder_Stop(compressedDecoder);	
					NEXUS_AudioDecoder_Close(pcmDecoder);	
					NEXUS_AudioDecoder_Close(compressedDecoder);
				}
				
				use_sw_keys_flag = false;
				break;
			case 97:
				goto Exit_DSC_5_TS_Crypto_Engine_Tdes;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				goto Start_DSC_5_TS_Crypto_Engine_Tdes;
				break;			
		}


		DSC_initialized = true;

		/* Bring up all modules for a platform in a default configuraiton for this platform */
	    	NEXUS_Platform_GetConfiguration(&platformConfig);

	    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
	    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

	    	/* Map a parser band to the streamer input band. */
	    	parserBand = NEXUS_ParserBand_e0;
	     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
	     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
	     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
	     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

	    	/* Open the audio and video pid channels */

		switch(select_test)
		{
			case 1:	
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceAudioPid, NULL);
				break;
			case 2:
			    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceVideoPid, NULL);
			    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceAudioPid, NULL);
				break;
			default:
				goto Start_DSC_5_TS_Crypto_Engine_Tdes;
				break;
		}
		
	    	/* Open the StcChannel to do lipsync with the PCR */
	    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	    	stcSettings.timebase = NEXUS_Timebase_e0;
	    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
	    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	    		the audio outputs. */
	    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	    	videoProgram.pidChannel = videoPidChannel;
	    	videoProgram.stcChannel = stcChannel;
	    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
	    	audioProgram.pidChannel = audioPidChannel;
	    	audioProgram.stcChannel = stcChannel;
		
	    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
	    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
	    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
	    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

	    	/* Bring up audio decoders and outputs */
	    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
		
	    	NEXUS_AudioOutput_AddInput(
	    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
	    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	    	);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	       	NEXUS_AudioOutput_AddInput(
	            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
	            	);
	    	else
	        	NEXUS_AudioOutput_AddInput(
	        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
	            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
	            	);

	    	/* bring up display */
	    	NEXUS_Display_GetDefaultSettings(&displaySettings);

	    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
	    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

	    	display = NEXUS_Display_Open(0, &displaySettings);
	    	window = NEXUS_VideoWindow_Open(display, 0);
		
	    	#if NEXUS_NUM_COMPONENT_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	    	#endif
	    	#if NEXUS_NUM_SCART_INPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
	    	#if NEXUS_NUM_SCART_INPUTS>1
	    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
	    	#endif
	    	#endif
	    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
	    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
	    	#endif
	    
	    	/* bring up decoder and connect to display */
	    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

	    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
	    	windowSettings.visible = true;
	    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
	    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

	    	/* Start Decoders */
	    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
	    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
		
	    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
	        	/* Only pass through AC3 */
	        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

	    	/* Prepare XPT to recognize PMT. */
	    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
	    	pidChannelSettings.requireMessageBuffer = true;


		switch(select_test)
		{
			case 1:	
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, hostKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			case 2:
	    			pidChannel = NEXUS_PidChannel_Open(parserBand, contentKeyScrambledServiceECMPid, &pidChannelSettings);
				break;
			default:
				goto Start_DSC_5_TS_Crypto_Engine_Tdes;
				break;
		}

	    	NEXUS_Message_GetDefaultSettings(&settings);
	    	settings.dataReady.callback = dsc_message_callback;
	     	settings.dataReady.context = &ChannelInfo;
	    	settings.maxContiguousMessageSize = 4096;
	    	msg = NEXUS_Message_Open(&settings);
	    	BDBG_ASSERT(msg);

	    	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
	    	startSettings.pidChannel = pidChannel;
		startSettings.psfCrcDisabled = true;

	    	/* use the default filter for any data */
	    	if (NEXUS_Message_Start(msg, &startSettings)) 
			goto Exit_DSC_5_TS_Crypto_Engine_Tdes;	

	}    
	
Exit_DSC_5_TS_Crypto_Engine_Tdes:

	if(DSC_initialized == true)
	{
		DSC_initialized = false;

		NEXUS_Message_Stop(msg);
		
		NEXUS_VideoWindow_Close(window);
		NEXUS_Display_Close(display);		
		NEXUS_VideoDecoder_Stop(vdecode);
		NEXUS_VideoDecoder_Close(vdecode);	
		NEXUS_AudioDecoder_Stop(pcmDecoder);	
		NEXUS_AudioDecoder_Stop(compressedDecoder);	
		NEXUS_AudioDecoder_Close(pcmDecoder);	
		NEXUS_AudioDecoder_Close(compressedDecoder);
	}
	
  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	csdTerminateHandle.initParam = &csdInitAudio; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return 1;
}

int TransportStreamCryptoEngineTest( void )
{
	char text_input[20];
	int select_test;

	while(1) 
	{	
		select_test = 0;
	
		printf("\n\n");
		printf("\nSelect Stream test to Test:\n");
	      	printf("0) Print menu\n");
		printf("1) [DSC_1] TS Crypto Engine Clear Text\n");
		printf("2) [DSC_2] TS Crypto Engine DVB-CSAv2\n");
		printf("3) [DSC_3] TS Crypto Engine DVB-CSAv3\n");
		printf("4) [DSC_4] TS Crypto Engine AES\n");
		printf("5) [DSC_5] TS Crypto Engine TDES\n");
		printf("97) Upper menu \n");
		printf("98) Root menu \n");		
		printf("99) Quit application\n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				break;
			case 1:
				DSC_1_TS_Crypto_Engine_Clear_Text();
				break;
			case 2:
				DSC_2_TS_Crypto_Engine_DVB_CSA_v2();
				break;
			case 3:
				DSC_3_TS_Crypto_Engine_DVB_CSA_v3();
				break;
			case 4:
				DSC_4_TS_Crypto_Engine_Aes();
				break;
			case 5:
				printf("TS Crypto Engine TDES is not supported\n");
/*				
				DSC_5_TS_Crypto_Engine_Tdes();
*/				
				break;
			case 97:
			case 98:
				goto BHSM_P_DONE_LABEL;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;	
		}
	}

	BHSM_P_DONE_LABEL:
	
	return 0;			

}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("overflow %s\n", (const char *)context);
}


static void scr_record_callback(void *context, int param)
{
    const void *data_buffer, *index_buffer;
    size_t data_buffer_size, index_buffer_size;
    int n;
    unsigned char * temp_buffer;

    BSTD_UNUSED(param);
#define MAX_READ (188*1024)

    NEXUS_Memory_Allocate(MAX_READ*(sizeof(unsigned char)), NULL, (void *)&temp_buffer);


    while (1) {

        if (NEXUS_Recpump_GetDataBuffer(*(NEXUS_RecpumpHandle*)(((SCsdScrAVPair *)context)->recpump), &data_buffer, &data_buffer_size))
            break;
		
        if (NEXUS_Recpump_GetIndexBuffer(*(NEXUS_RecpumpHandle*)(((SCsdScrAVPair *)context)->recpump), &index_buffer, &index_buffer_size))
            break;
		
        if (data_buffer_size == 0 && index_buffer_size == 0)
            break;

        if (data_buffer_size > MAX_READ)
            data_buffer_size = MAX_READ;

	 data_buffer_size =(data_buffer_size/(188*16))*(188*16);


        if (data_buffer_size == MAX_READ) {
	 /* Perfrom M2M DMA of data and store in buffer to be consumed by playpump. */
	 R2R_CryptoOperation(
	 	((SCsdScrAVPair *)context)->StreamInfo.initHandle->rootHandleNode->handle,
  		data_buffer, 
       	temp_buffer, 
       	data_buffer_size,
       	((SCsdScrAVPair *)context)->dmaFormat);
			
            n = fwrite(temp_buffer, 1, data_buffer_size, ((SCsdScrAVPair *)context)->file);
			
            if (n < 0) 
	         break;
            NEXUS_Recpump_DataWriteComplete(*(NEXUS_RecpumpHandle*)(((SCsdScrAVPair *)context)->recpump), n);
        }
        if (index_buffer_size) {
            n = fwrite(index_buffer, 1, index_buffer_size, ((SCsdScrAVPair *)context)->indexFile);
            if (n < 0)
	         break;
            NEXUS_Recpump_IndexWriteComplete(*(NEXUS_RecpumpHandle*)(((SCsdScrAVPair *)context)->recpump), n);
        }
		
    }

    /* Free the temp buffer. */
    NEXUS_Memory_Free(temp_buffer);
	
    return;
	
}

static void scr_play_callback(void *context, int param)
{
    void *buffer;
    size_t buffer_size;
    int n;
    unsigned char * temp_buffer;

    BSTD_UNUSED(param);
#define MAX_READ (188*1024)

    /* Allocate temporary buffer for storage of file.  This buffer will then be used as input to the
    	M2M DMA descrambler. */
    NEXUS_Memory_Allocate(MAX_READ*(sizeof(unsigned char)), NULL, (void *)&temp_buffer);

    while(1)
    {
        if(NEXUS_Playpump_GetBuffer(*(NEXUS_PlaypumpHandle*)(((SCsdScrAVPair *)context)->playpump), &buffer, &buffer_size))
	     break;

        if (buffer_size == 0)
		break;
            

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

	if( ((((SCsdScrAVPair *)context)->dma) == NULL) && ((((SCsdScrAVPair *)context)->bProtectedKey) == false) )
	{
	 /* Make sure the buffer size is a multiple of the packet size
	 	to guarantee alignment of packets. */
	 buffer_size =(buffer_size/(188*16))*(188*16);

	 /* read file and store data in temp buffer. */
        n = fread(temp_buffer, 1, buffer_size, ((SCsdScrAVPair *)context)->file);

	 /* Perfrom M2M DMA of data and store in buffer to be consumed by playpump. */
	 R2R_CryptoOperation(
	 	((SCsdScrAVPair *)context)->StreamInfo.initHandle->rootHandleNode->handle,
  		temp_buffer, 
       	buffer, 
       	buffer_size,
       	((SCsdScrAVPair *)context)->dmaFormat);
	}
	else		
        	n = fread(buffer, 1, buffer_size, ((SCsdScrAVPair *)context)->file);

        if (n < 0)
	    break;
		
        if (n == 0) 
	 {	 
            BKNI_Sleep(1000); /* This is a simplistic way of making sure that decoder buffers are empty before flushing. */
            fseek(((SCsdScrAVPair *)context)->file, 0, SEEK_SET);
			
            NEXUS_Playpump_Flush(*(NEXUS_PlaypumpHandle*)(((SCsdScrAVPair *)context)->playpump));
			
        }
        else
	 {
	 
            NEXUS_Playpump_ReadComplete(*(NEXUS_PlaypumpHandle*)(((SCsdScrAVPair *)context)->playpump), 0, n);
        }

    }

    /* Free the temp buffer. */
    NEXUS_Memory_Free(temp_buffer);
	
    return;
	
}

TCsdStatus SCR_X_1_Decrypt_Host_Key(
	int scr_test,
	ECsdAlgorithm algorithm
)
{
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    char fname[40];
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlatformConfiguration platformConfig;
    TCsdStatus status;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    TCsdInitParameters csdInit;
    TCsdTerminateParameters csdTerminateHandle; 
    SCsdScrAVPair ScrHandle;
    char text_input[20];
    int select_test;
    TCsdUnsignedInt8 *pxClearTextScrHostKey;
	
    memset (&csdInit, 0, sizeof(TCsdInitParameters));
    status=csdInitialize(&csdInit);
	
    if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
    {
        printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
	 return 1;
    }

    ScrHandle.StreamInfo.initHandle = &csdInit;

    NEXUS_Memory_Allocate(sizeof(TCsdUnsignedInt8)*16, NULL, (void *)&pxClearTextScrHostKey);

request_key_again:
    printf("\nSelect the following option? \n");   
    printf("[SCR_%x_1] [1]: Host Key1 \n", scr_test);   
    printf("[SCR_%x_1] [2]: Host Key2 \n", scr_test);   
    fputs("Choice: ", stdout);   
	
    fflush(stdout); 		
    fgets(text_input, sizeof text_input, stdin);
    sscanf(text_input, "%d", &select_test);

    switch(select_test)
    {
        case 1:	
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesPayload1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesPayload1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesRaw1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesRaw1, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			goto request_key_again;
		}
		break;
	case 2:
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesPayload2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesPayload2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesRaw2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesRaw2, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			return 0;
		}
		break;
	default:
    		printf("Invalid choice.  Shall be 1 or 2 only.\n");   
		goto request_key_again;
		break;
    	}

request_ts_again:
    printf("Which MPEG Transport Steam do you want to use? \n");   
    printf("[1]: scrEmi0x%04x.ts \n", algorithm);   
    printf("[2]: scrEmi0x%04xRecorded.ts \n", algorithm);   
    fputs("Choice: ", stdout);   
	
    fflush(stdout); 		
    fgets(text_input, sizeof text_input, stdin);
    sscanf(text_input, "%d", &select_test);

    switch(select_test)
    {
        case 1:	
		switch(algorithm)
		{
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0020.ts");		
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0021.ts");		
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
			 	strcpy(fname, "scrEmi0x0022.ts");		
				break;
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0040.ts");		
				break;
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0041.ts");		
				break;
			case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
			 	strcpy(fname, "scrEmi0x4020.ts");		
				break;
			case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x4021.ts");		
				break;
			case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
			 	strcpy(fname, "scrEmi0x4040.ts");		
				break;
			case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x4041.ts");		
				break;
			default:
		    		printf("Invalid algorithm.\n");   
				return 0;
				break;
		}
		break;
	case 2:
		switch(algorithm)
		{
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0020Recorded.ts");		
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0021Recorded.ts");		
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
			 	strcpy(fname, "scrEmi0x0022Recorded.ts");		
				break;
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0040Recorded.ts");		
				break;
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0041Recorded.ts");		
				break;
			case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
			 	strcpy(fname, "scrEmi0x4020Recorded.ts");		
				break;
			case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x4021Recorded.ts");		
				break;
			case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
			 	strcpy(fname, "scrEmi0x4040Recorded.ts");		
				break;
			case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x4041Recorded.ts");		
				break;
			default:
		    		printf("Invalid algorithm.\n");   
				return 0;
				break;
		}	
		break;
	default:
    		printf("Invalid choice.  Shall be 1 or 2 only.\n");   
		fflush(stdout); 		
		goto request_ts_again;
		break;
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);

	if((algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		NULL,
        		0,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesRaw,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorTdesRaw,
        		8,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesCbcPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}	
	else if((algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesCIplusPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}		
	else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52))
	{
    		if(csdSetClearTextScrDecryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorTdesPayload,
        		8,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}		
	else
	{
		printf("Invalid algorithm\n");
		return 0;
	}	
	
    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
	
    if (!file)
    {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto Exit_SCR_X_1_Decrypt_Host_Key_error;
    }

    ScrHandle.file=file;

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);

    ScrHandle.playpump=&playpump;
    ScrHandle.bProtectedKey=false;

    /* Do not use the nexus DMA for decryption. 
    	Manually perfrom DMA in callback to have better
    	control. */
    ScrHandle.dma=NULL;
    playpumpSettings.securityDma = NULL;
    
    if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
	 playpumpSettings.securityDmaDataFormat = NEXUS_DmaDataFormat_eBlock;
    else
        playpumpSettings.securityDmaDataFormat = NEXUS_DmaDataFormat_eMpeg;
	
    ScrHandle.dmaFormat = playpumpSettings.securityDmaDataFormat;
    playpumpSettings.dataCallback.context = &ScrHandle;
    playpumpSettings.dataCallback.callback = scr_play_callback;

    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, scrServiceVideoPid, NULL);
    audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, scrServiceAudioPid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;	

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
    );

    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
        );
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
        );
    }	

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    while (1) 
    {
        select_test = 0;
        printf("\n\n");
        printf("\nSelect to end test\n");
        printf("0) Print menu\n");
	printf("97) Upper Menu \n");
        printf("99) Quit application \n");
        
        fputs("Choice: ", stdout);   
        fflush(stdout);
        
        fgets(text_input, sizeof text_input, stdin);
        sscanf(text_input, "%d", &select_test);
        
        switch(select_test)
        {
        	case 0:  
        		break;
        	case 97:
        		goto Exit_SCR_X_1_Decrypt_Host_Key;
        		break;
        	case 99:
        		exit (0);
        	default:
        		printf("Unknown choice...\n");
        		break;			
        }

    }

Exit_SCR_X_1_Decrypt_Host_Key:

	NEXUS_AudioDecoder_Stop(pcmDecoder);	
	NEXUS_AudioDecoder_Stop(compressedDecoder);	
	NEXUS_AudioDecoder_Close(pcmDecoder);
	NEXUS_AudioDecoder_Close(compressedDecoder);
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);		
	NEXUS_VideoDecoder_Stop(videoDecoder);
	NEXUS_VideoDecoder_Close(videoDecoder);	

Exit_SCR_X_1_Decrypt_Host_Key_error:
	NEXUS_Playpump_Stop(playpump);
	NEXUS_Playpump_Close(playpump);
	NEXUS_Memory_Free(pxClearTextScrHostKey);
		
    csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);
	
    return 0;
}


TCsdStatus SCR_X_2_Encrypt_Host_Key(
	int scr_test,
	ECsdAlgorithm algorithm
)
{
    NEXUS_InputBand inputBand;
    NEXUS_RecpumpHandle recpump;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    char fname[40];
    char index[40];
    TCsdInitParameters csdInit;
    TCsdTerminateParameters csdTerminateHandle; 
    SCsdScrAVPair ScrHandle;
    TCsdStatus status;
    char unFeatureChoice1[20];
    char text_input[20];
    int select_test;
    TCsdUnsignedInt8 *pxClearTextScrHostKey;
    FILE *data_file;
    FILE *index_file;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;

    memset (&csdInit, 0, sizeof(TCsdInitParameters));
    status=csdInitialize(&csdInit);

    ScrHandle.StreamInfo.initHandle = &csdInit;

    if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
    {
        printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
        return 1;
    }

    NEXUS_Memory_Allocate(sizeof(TCsdUnsignedInt8)*16, NULL, (void *)&pxClearTextScrHostKey);

request_key_again:
    printf("\nSelect the following option? \n");   
    printf("[SCR_%x_2] [1]: Host Key1 \n", scr_test);   
    printf("[SCR_%x_2] [2]: Host Key2 \n", scr_test);   
    fputs("Choice: ", stdout);   

    fflush(stdout); 		
    fgets(text_input, sizeof text_input, stdin);
    sscanf(text_input, "%d", &select_test);

    switch(select_test)
    {
        case 1:	
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesPayload1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesPayload1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesRaw1, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesRaw1, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			goto request_key_again;
		}
		break;
	case 2:
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesPayload2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesPayload2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyAesRaw2, sizeof(TCsdUnsignedInt8)*16);
		else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
			(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
			BKNI_Memcpy(pxClearTextScrHostKey, scrClearTextHostKeyTdesRaw2, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			return 0;
		}
		break;
	default:
    		printf("Invalid choice.  Shall be 1 or 2 only.\n");   
		goto request_key_again;
		break;
    	}

	/* Set the proper name of recorded file. */
	switch(algorithm)
	{
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
		 	strcpy(fname, "scrEmi0x0020Recorded.ts");		
		       strcpy(index, "scrEmi0x0020Recorded.nav");	
			
			break;
		case CSD_Algorithm_AES_128_ECB_CLEAR:
		 	strcpy(fname, "scrEmi0x0021Recorded.ts");		
		       strcpy(index, "scrEmi0x0021Recorded.nav");	
			
			break;
		case CSD_Algorithm_AES_128_CBC_CLEAR:
		 	strcpy(fname, "scrEmi0x0022Recorded.ts");	
		       strcpy(index, "scrEmi0x0022Recorded.nav");	
			
			break;
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
		 	strcpy(fname, "scrEmi0x0040Recorded.ts");		
		       strcpy(index, "scrEmi0x0040Recorded.nav");	
			
			break;
		case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
		 	strcpy(fname, "scrEmi0x0041Recorded.ts");		
		       strcpy(index, "scrEmi0x0041Recorded.nav");	
			
			break;
		case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
		 	strcpy(fname, "scrEmi0x4020Recorded.ts");		
		       strcpy(index, "scrEmi0x4020Recorded.nav");	
			
			break;
		case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
		 	strcpy(fname, "scrEmi0x4021Recorded.ts");		
		       strcpy(index, "scrEmi0x4021Recorded.nav");	
			
			break;
		case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
		 	strcpy(fname, "scrEmi0x4040Recorded.ts");		
		       strcpy(index, "scrEmi0x4040Recorded.nav");	
			
			break;
		case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
		 	strcpy(fname, "scrEmi0x4041Recorded.ts");		
		       strcpy(index, "scrEmi0x4041Recorded.nav");	
			
			break;
		default:
	    		printf("Invalid algorithm.\n");   
			return 0;
			break;
	}	

	if((algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_TDES_ABA_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		NULL,
        		0,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesRaw,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorTdesRaw,
        		8,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}
	else if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesCbcPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}	
	else if((algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorAesCIplusPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}		
	else if((algorithm == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52))
	{
    		if(csdSetClearTextScrEncryptionKey(
        		algorithm,
  	 		pxClearTextScrHostKey,
        		16,
  	 		scrInitializationVectorTdesPayload,
        		8,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
	}		
	else
	{
		printf("Invalid algorithm\n");
		return 0;
	}	

    NEXUS_Platform_GetConfiguration(&platformConfig);


    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);

    /* Open the files for recording. */
    data_file = fopen(fname, "wb");
    if (!data_file) {
        fprintf(stderr, "can't open data file:%s\n", fname);
        goto Exit_SCR_X_3_Decrypt_Content_Key;
    }
    index_file = fopen(index, "wb");
    if (!index_file) {
        fprintf(stderr, "can't open index file:%s\n", index);
        goto Exit_SCR_X_3_Decrypt_Content_Key;
    }

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);

    ScrHandle.file=data_file;
    ScrHandle.indexFile=index_file;
    ScrHandle.recpump=&recpump;

    /* Do not use the nexus DMA for decryption. 
    	Manually perfrom DMA in callback to have better
    	control. */
    ScrHandle.dma=NULL;
    recpumpSettings.securityDma = NULL;
    recpumpSettings.securityContext = NULL;
	
    recpumpSettings.data.dataReady.callback = scr_record_callback;
    recpumpSettings.data.dataReady.context = &ScrHandle;
    recpumpSettings.index.dataReady.callback = scr_record_callback;
    recpumpSettings.index.dataReady.context = &ScrHandle;
	
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = "data";
    recpumpSettings.index.overflow.callback = overflow_callback;
    recpumpSettings.index.overflow.context = "index";
	
	if((algorithm == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(algorithm == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR))
	    recpumpSettings.securityDmaDataFormat = NEXUS_DmaDataFormat_eBlock;
	else
	    recpumpSettings.securityDmaDataFormat = NEXUS_DmaDataFormat_eMpeg;

    ScrHandle.dmaFormat = recpumpSettings.securityDmaDataFormat;

    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    NEXUS_Recpump_AddPidChannel(recpump, 
        pidChannel[0] =NEXUS_PidChannel_Open(parserBand, scrServiceVideoPid, NULL), /* open it again */
        &addPidChannelSettings);
    NEXUS_Recpump_AddPidChannel(recpump, 
        pidChannel[1]=NEXUS_PidChannel_Open(parserBand, scrServiceAudioPid, NULL), /* open it again */
        NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = pidChannel[0]; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = pidChannel[0];
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    audioProgram.pidChannel = pidChannel[1];
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

     /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));

    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    NEXUS_Recpump_Start(recpump);

    /* Nexus is now recording to disk */
    fputs("press ENTER to stop record", stdout);   
    fflush(stdout); 		
    fgets(unFeatureChoice1, sizeof unFeatureChoice1, stdin);

    NEXUS_AudioDecoder_Stop(pcmDecoder);	
    NEXUS_AudioDecoder_Stop(compressedDecoder);	
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);		
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_VideoDecoder_Close(vdecode);	
    
    NEXUS_Recpump_RemoveAllPidChannels(recpump);    
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Recpump_Close(recpump);
	
Exit_SCR_X_3_Decrypt_Content_Key:

    NEXUS_Memory_Free(pxClearTextScrHostKey);

    csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 	
    csdTerminate(&csdTerminateHandle);

    return 0;
}


TCsdStatus SCR_X_3_Decrypt_Content_Key(
	int scr_test,
	ECsdAlgorithm algorithm
)
{
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    char fname[40];
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlatformConfiguration platformConfig;
    TCsdStatus status;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    TCsdInitParameters csdInitVideo;
    TCsdInitParameters csdInitAudio;
    TCsdTerminateParameters csdTerminateHandle; 
    SCsdScrAVPair ScrHandle;
    char text_input[20];
    int select_test;
    TCsdUnsignedInt8 *pxCipheredScrContentKey;
    TCsd4BytesVector ulValue;
    NEXUS_PidChannelStatus pidStatus;
	
    memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
    status=csdInitialize(&csdInitVideo);

    memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
    status=csdInitialize(&csdInitAudio);

    if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
    {
        printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
	 return 1;
    }

    ScrHandle.StreamInfo.initHandle = &csdInitVideo;
    ScrHandle.AudioInfo.initHandle = &csdInitAudio;

    status = csdGetNuid(ulValue);
    
    if(status) 
        printf("Error: can not get NUID \n"); 
    
    global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

    NEXUS_Memory_Allocate(sizeof(TCsdUnsignedInt8)*16, NULL, (void *)&pxCipheredScrContentKey);

request_key_again:
    printf("\nSelect the following option? \n");   
    printf("[SCR_%x_3] [1]: Protecting Key & Content Key1 \n", scr_test);   
    printf("[SCR_%x_3] [2]: Protecting Key & Content Key2 \n", scr_test);   
    fputs("Choice: ", stdout);   
	
    fflush(stdout); 		
    fgets(text_input, sizeof text_input, stdin);
    sscanf(text_input, "%d", &select_test);

    switch(select_test)
    {
        case 1:	
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxCipheredScrContentKey, scrCipheredContentKeyAesPayload1, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			return 0;
		}
		break;
	case 2:
		if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
			(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) ||
			(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
			BKNI_Memcpy(pxCipheredScrContentKey, scrCipheredContentKeyAesPayload2, sizeof(TCsdUnsignedInt8)*16);
		else
		{
			printf("Invalid algorithm.\n");   
			return 0;
		}
		break;
	default:
    		printf("Invalid choice.  Shall be 1 or 2 only.\n");   
		goto request_key_again;
		break;
    	}

request_ts_again:
    printf("Which MPEG Transport Steam do you want to use? \n");   
    printf("[1]: scrEmi0x%04x.ts \n", algorithm);   
    printf("[2]: scrEmi0x%04xRecorded.ts \n", algorithm);   
    fputs("Choice: ", stdout);   
	
    fflush(stdout); 		
    fgets(text_input, sizeof text_input, stdin);
    sscanf(text_input, "%d", &select_test);

    switch(select_test)
    {
        case 1:	
		switch(algorithm)
		{
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0020.ts");				
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0021.ts");		
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
			 	strcpy(fname, "scrEmi0x0022.ts");		
				break;
			default:
		    		printf("Invalid algorithm.\n");   
				return 0;
				break;
		}
		break;
	case 2:
		switch(algorithm)
		{
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			 	strcpy(fname, "scrEmi0x0020Recorded.ts");		
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
			 	strcpy(fname, "scrEmi0x0021Recorded.ts");		
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
			 	strcpy(fname, "scrEmi0x0022Recorded.ts");		
				break;
			default:
		    		printf("Invalid algorithm.\n");   
				return 0;
				break;
		}	
		break;
	default:
    		printf("Invalid choice.  Shall be 1 or 2 only.\n");   
		fflush(stdout); 		
		goto request_ts_again;
		break;
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);
	
    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
	
    if (!file)
    {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto Exit_SCR_X_3_Decrypt_Content_Key_error;
    }

    ScrHandle.file=file;

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);

    ScrHandle.playpump=&playpump;
    ScrHandle.bProtectedKey=true;
    ScrHandle.dma=NULL;
    playpumpSettings.securityDma = NULL;
    playpumpSettings.securityDmaDataFormat = NEXUS_DmaDataFormat_eMpeg;
    ScrHandle.dmaFormat = playpumpSettings.securityDmaDataFormat;
    playpumpSettings.dataCallback.context = &ScrHandle;
    playpumpSettings.dataCallback.callback = scr_play_callback;

    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, scrServiceVideoPid, NULL);
    audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, scrServiceAudioPid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;	

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
    );

    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
        );
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
        );
    }	

	NEXUS_PidChannel_GetStatus (videoProgram.pidChannel, &pidStatus);
	ScrHandle.StreamInfo.initHandle->pidchannel= pidStatus.pidChannelIndex;

	NEXUS_PidChannel_GetStatus (audioProgram.pidChannel, &pidStatus);
	ScrHandle.AudioInfo.initHandle->pidchannel= pidStatus.pidChannelIndex;

	ScrHandle.StreamInfo.initHandle->scramblingControlBits = NEXUS_SecurityKeyType_eEven;
	ScrHandle.AudioInfo.initHandle->scramblingControlBits = NEXUS_SecurityKeyType_eEven;

	if(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR)
	{
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		NULL,
        		0,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
			
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		NULL,
        		0,
        		&(ScrHandle.AudioInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 			
	}
	else if((algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52))
	{
	
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		scrInitializationVectorAesCbcPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
			
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		scrInitializationVectorAesCbcPayload,
        		16,
        		&(ScrHandle.AudioInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
			

	}	
	else if((algorithm == CSD_Algorithm_AES_128_CBC_CLEAR))
	{
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		scrInitializationVectorAesCIplusPayload,
        		16,
        		&(ScrHandle.StreamInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
			
    		if(csdSetProtectedScrDecryptionKey(
        		algorithm,
        		scrCipheredProtectingKeysTableAesPayload[global_nuid],
  	 		pxCipheredScrContentKey,
        		16,
  	 		scrInitializationVectorAesCIplusPayload,
        		16,
        		&(ScrHandle.AudioInfo)
    			) != CSD_NO_ERROR)
        		printf("Error: csdSetClearTextScrDecryptionKey failed!\n"); 
			
	}		
	else
	{
		printf("Invalid algorithm\n");
		return 0;
	}	



    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    while (1) 
    {
        select_test = 0;
        printf("\n\n");
        printf("\nSelect to end test\n");
        printf("0) Print menu\n");
	printf("97) Upper Menu \n");
        printf("99) Quit application \n");
        
        fputs("Choice: ", stdout);   
        fflush(stdout);
        
        fgets(text_input, sizeof text_input, stdin);
        sscanf(text_input, "%d", &select_test);
        
        switch(select_test)
        {
        	case 0:  
        		break;
        	case 97:
        		goto Exit_SCR_X_3_Decrypt_Content_Key;
        		break;
        	case 99:
        		exit (0);
        	default:
        		printf("Unknown choice...\n");
        		break;			
        }

    }

Exit_SCR_X_3_Decrypt_Content_Key:

	NEXUS_AudioDecoder_Stop(pcmDecoder);	
	NEXUS_AudioDecoder_Stop(compressedDecoder);	
	NEXUS_AudioDecoder_Close(pcmDecoder);
	NEXUS_AudioDecoder_Close(compressedDecoder);
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);		
	NEXUS_VideoDecoder_Stop(videoDecoder);
	NEXUS_VideoDecoder_Close(videoDecoder);	

Exit_SCR_X_3_Decrypt_Content_Key_error:
	NEXUS_Playpump_Stop(playpump);
	NEXUS_Playpump_Close(playpump);
	NEXUS_Memory_Free(pxCipheredScrContentKey);
	
    csdTerminateHandle.initParam = &csdInitVideo; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);

    csdTerminateHandle.initParam = &csdInitAudio; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);
	
    return 0;
}



int SCR_X_Menu(int scr_test,  ECsdAlgorithm algorithm)
{
	char text_input[20];
	int select_test;
	TCsdStatus errCode;

	while(1) 
	{	
		select_test = 0;
	
		printf("\n\n");
		printf("\nSelect Stream test to Test:\n");
	      	printf("0) Print menu\n");
		printf("1) [SCR_%x_1] Decrypt - Host Key - EMI 0x%04x\n", scr_test, algorithm);
		printf("2) [SCR_%x_2] Encrypt - Host Key - EMI 0x%04x\n", scr_test, algorithm);
		printf("3) [SCR_%x_3] Decrypt - Content Key - EMI 0x%04x\n", scr_test, algorithm);
		printf("4) [SCR_%x_4] Encrypt - Content Key - EMI 0x%04x\n", scr_test, algorithm);
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application\n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch(select_test)
		{
			case 0:  
				break;
			case 1:
				errCode = SCR_X_1_Decrypt_Host_Key(scr_test, algorithm);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 2:
				errCode = SCR_X_2_Encrypt_Host_Key(scr_test, algorithm);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 3:				
				if( (algorithm == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
					(algorithm == CSD_Algorithm_AES_128_ECB_CLEAR) || 
					(algorithm == CSD_Algorithm_AES_128_CBC_CLEAR) )
				{
					errCode = SCR_X_3_Decrypt_Content_Key(scr_test, algorithm);
					if(errCode)
						goto BHSM_P_DONE_LABEL;		
				}
				else
					printf("Content Key Decryption is not supported.\n");
				break;
			case 4:
				printf("Content Key Encryption is not supported.\n");				
				break;
			case 97:	
				goto BHSM_P_DONE_LABEL;	
				break;
			case 98:
				return 1;
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;	
		}
	}

	BHSM_P_DONE_LABEL:
	
	return 0;			

}

int StreamCryptoEngineTest( void )
{
	char text_input[20];
	int select_test;
	TCsdStatus errCode;

	while(1) 
	{	
		select_test = 0;
	
		printf("\n\n");
		printf("\nSelect Stream test to Test:\n");
	      	printf("0) Print menu\n");
		printf("1) [SCR_1] EMI 0x0020\n");
		printf("2) [SCR_2] EMI 0x0021\n");
		printf("3) [SCR_3] EMI 0x0022\n");
		printf("4) [SCR_4] EMI 0x0040\n");
		printf("5) [SCR_5] EMI 0x0041\n");
		printf("6) [SCR_6] EMI 0x4020\n");
		printf("7) [SCR_7] EMI 0x4021\n");
		printf("8) [SCR_8] EMI 0x4040\n");
		printf("9) [SCR_9] EMI 0x4041\n");
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application\n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch(select_test)
		{
			case 0:  
				break;
			case 1:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_AES_128_CBC_IV_0_SCTE52);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 2:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_AES_128_ECB_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 3:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_AES_128_CBC_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 4:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 5:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_TDES_ABA_ECB_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 6:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 7:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_RAW_AES_128_ECB_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 8:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 9:
				errCode = SCR_X_Menu(select_test, CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR);
				if(errCode)
					goto BHSM_P_DONE_LABEL;			
				break;
			case 97:	
			case 98:
				goto BHSM_P_DONE_LABEL;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;	
		}
	}

	BHSM_P_DONE_LABEL:
	
	return 0;			

}


static unsigned input_number(void)
{
    unsigned number;
	
    scanf("%x", &number);
    printf("0x%x", number);
	
    return number;
}

static int OTPTest_SetBootMode(void)
{
	TCsdStatus errCode;
	char text_input[20];
	int select_test;

   	while(1)
	{
		select_test = 0;
	
		printf("\n");
		printf("0) Print menu \n");
		printf("1) Boot from SPI \n"); 
		printf("2) Boot from NAND \n"); 
		printf("3) Boot from NOR \n"); 
		printf("97) Upper menu \n");
		printf("98) Root menu  \n");
		printf("99) Quit application \n");
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
	
		switch (select_test)
		{
			case 0:
				break;
			case 1:
				errCode  = csdSetBootMode(CSD_BOOT_FROM_SPI);
				
				if(errCode == CSD_NO_ERROR)
					printf( "\nBoot mode is set to CSD_BOOT_FROM_SPI\n"); 
				else	
					printf("\nError - Boot mode is not set to CSD_BOOT_FROM_SPI\n");
				
				break;
			case 2:
				errCode  = csdSetBootMode(CSD_BOOT_FROM_NAND);
				
				if(errCode == CSD_NO_ERROR) 
					printf( "\nBoot mode is set to CSD_BOOT_FROM_NAND\n"); 
				else	
					printf("\nError - Boot mode is not set to CSD_BOOT_FROM_NAND\n");
				
				break;
			case 3:				
				errCode  = csdSetBootMode(CSD_BOOT_FROM_NOR);
				
				if(errCode == CSD_NO_ERROR) 
					printf( "\nBoot mode is set to CSD_BOOT_FROM_NOR\n"); 
				else	
					printf("\nError - Boot mode is not set to CSD_BOOT_FROM_NOR\n");
				
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}

	return 0;
}

static int OTPTest_EnableSCS(void){

	TCsdStatus errCode;
	char text_input[20];
	int select_test;

	errCode = csdEnableScs();
	
	if(errCode == CSD_NO_ERROR) 
		printf("\nSCS is enabled\n");
	else
		printf("\nError -  SCS is not enabled \n");
	
   	while(1)
	{
		select_test = 0;
		printf("\n");
		printf("0) Print menu \n");
		printf("97) Upper menu \n");
		printf("98) Root menu  \n");
		printf("99) Quit application \n");
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
	
		switch (select_test)
		{
			case 0:
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}
	return 0;
}

static int OTPTest_SetSCSTotalAreaSize(void){

	TCsdStatus errCode;
	char text_input[20];
	int select_test;
	
	errCode = csdSetScsTotalAreaSize(0);
	
	if(errCode == CSD_NO_ERROR) 
		printf("\nFunction returned successful, but SCS total size is not supported.\n");
	else
		printf("\nCould not set Scs total area size\n");
	
   	while(1)
	{
		select_test = 0;
		printf("\n");
		printf("0) Print menu \n");
		printf("97) Upper menu \n");
		printf("98) Root menu  \n");
		printf("99) Quit application \n");
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch (select_test)
		{
			case 0:
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}
	
	return 0;
}

static int OTPTest_SetMarketID(void)
{

	TCsdStatus errCode;
	char text_input[20];
	int select_test;
	unsigned int value;
	TCsd4BytesVector marketsegmentid;

  	printf("\nEnter the Market Segment ID value(Hex): \n");
	
	value = input_number();	

	marketsegmentid[3] = value &0xff;
	marketsegmentid[2] = (value>>8) &0xff; 
	marketsegmentid[1] = (value>>16) &0xff; 
	marketsegmentid[0] = (value>>24) &0xff; 
	
	errCode = csdSetMarketSegmentId(marketsegmentid);
	
	if(errCode == CSD_NO_ERROR) 
		printf("\nThe value 0x%x has been set for the Market Segment ID\n", value);
	else
		printf("\nError -  Market Segment ID is not set \n");
	
   	while(1)
	{
		select_test = 0;
		printf("\n");
		printf("0) Print menu \n");
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application \n"); 
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch (select_test)
		{
			case 0:
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}

	return 0;
}

static int OTPTest_SetStbCaSerialNumber(void)
{

	TCsdStatus errCode;
	char text_input[20];
	int select_test;
	unsigned int value;
	TCsd4BytesVector serialNumber;

  	printf("\nEnter the STB CA serial number value(Hex): \n");
	
	value = input_number();	

	serialNumber[3] = value &0xff;
	serialNumber[2] = (value>>8) &0xff; 
	serialNumber[1] = (value>>16) &0xff; 
	serialNumber[0] = (value>>24) &0xff; 
	
	errCode = csdSetStbCaSn(serialNumber);
	
	if(errCode == CSD_NO_ERROR) 
		printf("\nThe value 0x%x has been set for the STB CA Serial Number\n", value);
	else
		printf("\nError -  STB CA Serial Number is not set \n");
	
   	while(1)
	{
		select_test = 0;
		printf("\n");
		printf("0) Print menu \n");
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application \n"); 
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch (select_test)
		{
			case 0:
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}

	return 0;
}


static int  OTPTest_SetJTAGProtectionLevel(void)
{

	TCsdStatus errCode;
	char text_input[20];
	int select_test;
	
	while(1)
	{
		select_test = 0;
		printf("\n");
	 	printf("0) Print menu \n");
	 	printf("1) ACCESS_MODE_OPEN \n"); 
	 	printf("2) ACCESS_MODE_PASSWORD_PROTECTED \n"); 
	 	printf("3) ACCESS_MODE_CLOSED \n"); 
		printf("97) Upper menu \n");
	 	printf("98) Root menu  \n");
	 	printf("99) Quit application \n");	
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch (select_test)
		{
			case 0:
			 	break;
		 	case 1:
			 	errCode  = csdSelectDebugInterfaceProtectionLevel(CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN);
			 	if(errCode == CSD_NO_ERROR) 
					printf( "\nDebug Interface Protection Level is open\n"); 
			 	else	
					printf("\nError - Failed to open the Debug Interface Protection Level\n");
			 	break;
			case 2:
			 	errCode  = csdSelectDebugInterfaceProtectionLevel(CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED);
			 	if(errCode == CSD_NO_ERROR) 
			 	{
					printf( "\nDebug Interface Protection Level is password protected\n"); 
					printf( "This will NOT take effect until the chip is reset.\n"); 
			 	}
			 	else	 
					printf("\nError - Failed to password protect the Debug Interface Protection Level\n");
			 	break;
			case 3:
			 	errCode  = csdSelectDebugInterfaceProtectionLevel(CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED);
			 	if(errCode == CSD_NO_ERROR) 
			 	{
			 		printf( "\nDebug Interface Protection Level is closed\n"); 
					printf( "This will NOT take effect until the chip is reset.\n"); 
			 	}
			 	else	 
					printf("\nError - Failed to close protect the Debug Interface Protection Level\n");
			 	break;
		 	case 97:
			 	return 0; 
		 	case 98:
			 	return 1;
		 	case 99:
			 	exit(0);
		 	default:
			 	break;
		}
	}

	return 0;

}

static int OTPTest_DeactivateKeyLadder(void){

	TCsdStatus errCode;
	char text_input[20];
	int select_test;

	errCode = csdDeactivateKeyLadder();
	
	if(errCode == CSD_NO_ERROR) 
		printf("\nKey ladder is deactivated\n");
	else
		printf("\nError -  Key ladder is not deactivated \n");
	
   	while(1)
	{
		select_test = 0;
		printf("\n");
		printf("0) Print menu \n");
		printf("97) Upper menu \n");
		printf("98) Root menu  \n");
		printf("99) Quit application \n");
		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch (select_test)
		{
			case 0:
				break;
			case 97:
				return 0; 
			case 98:
				return 1;
			case 99:
				exit(0);
			default:
				break;
	   	}
   	}

	return 0;
}

static int OTPTest_GetConfigBits_Test(void)
{
	unsigned char FieldValue[4];
	unsigned int value;
	char text_input[20];
	unsigned int select_otp;

	fputs("\nEnter enum of OTP to read (hex): 0x", stdout);
	fflush(stdout);
		
 	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%x", &select_otp);

	if(ctdGetConfigurationBitField((TCtdBitFieldHandle)select_otp, 0, FieldValue) !=  CTD_NO_ERROR)
	{
		printf("\nError read OTP field value!\n");	
	}
	else
	{
		value = ((unsigned int) FieldValue[0])<<24 | 
			((unsigned int) FieldValue[1])<<16 | 
			((unsigned int) FieldValue[2])<<8 | 
			((unsigned int) FieldValue[3]);
		
		printf("\nOTP field 0x%x value is: 0x%x\n", select_otp, value);
	}
	return 0;
}

static int OTPTest_SetConfigBits_Test(void)
{
	char text_input[20];
	unsigned int select_otp;
	unsigned int  select_value;
	unsigned int  select_mask;
	unsigned int  select_length;
	unsigned char FieldValue[4];
	unsigned char FieldMask[4];
	
	fputs("\nEnter enum of OTP to program (hex): 0x", stdout);
	fflush(stdout);
		
 	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%x", &select_otp);

	fputs("\nEnter value to program to OTP (hex): 0x", stdout);
	fflush(stdout);
		
 	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%x", &select_value);	

	fputs("\nEnter mask of OTP field (hex): 0x", stdout);
	fflush(stdout);
		
 	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%x", &select_mask);		

	fputs("\nEnter bit length of OTP field (hex): 0x", stdout);
	fflush(stdout);
		
 	fgets(text_input, sizeof text_input, stdin);
	sscanf(text_input, "%x", &select_length);		

	FieldValue[0] = (unsigned char)((select_value & 0xFF000000) >> 24);
	FieldValue[1] = (unsigned char)((select_value & 0x00FF0000) >> 16);
	FieldValue[2] = (unsigned char)((select_value & 0x0000FF00) >> 8);
	FieldValue[3] = (unsigned char)(select_value & 0x000000FF);

	FieldMask[0] = (unsigned char)((select_mask & 0xFF000000) >> 24);
	FieldMask[1] = (unsigned char)((select_mask & 0x00FF0000) >> 16);
	FieldMask[2] = (unsigned char)((select_mask & 0x0000FF00) >> 8);
	FieldMask[3] = (unsigned char)(select_mask & 0x000000FF);

	ctdSetConfigurationBitField((TCtdBitFieldHandle)select_otp,
		(TCtdUnsignedInt16)select_length,
		(TCtdUnsignedInt8*)&FieldValue,
		(TCtdUnsignedInt8*)&FieldMask);

	return 0;
}

int  Test_OTP(void)
{
	char text_input[20];
	int select_test;
	unsigned int return_code; 

 	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("\nError: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
  	while(1)
	{
		select_test = 0;
		
		printf("\n\n");
		printf("\nPersistent Value Configuration Test....\n");
		printf("0) Print menu\n");
		printf("1) [PV_1] Set Boot Mode\n");
		printf("2) [PV_2] Enable SCS\n");
		printf("3) [PV_3] Set Market Segment ID\n");
		printf("4) [PV_4] Set SCS total area size \n");
		printf("5) [PV_5] Set STB CA Serial Number \n");
		printf("6) [PV_6] Select Debug Interface Protection \n"); 
		printf("7) [PV_7] Deactive Key Ladder \n");
		printf("8) [PV_8] Set Configuration Bit Field\n");
		printf("9) [PV_9] Get Configuration Bit Field\n");
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application \n"); 
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch(select_test)
		{
			case 0:
				break;
			case 1:
				return_code = OTPTest_SetBootMode();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;
			case 2:
				return_code = OTPTest_EnableSCS();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;
			case 3:
				return_code = OTPTest_SetMarketID();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;	
			case 4:
				return_code = OTPTest_SetSCSTotalAreaSize();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;	
			case 5:
				return_code = OTPTest_SetStbCaSerialNumber();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;	
			case 6:
				return_code = OTPTest_SetJTAGProtectionLevel();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;		
			case 7:
				return_code = OTPTest_DeactivateKeyLadder();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;	
			case 8:
				return_code = OTPTest_SetConfigBits_Test();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;
			case 9:
	  			return_code = OTPTest_GetConfigBits_Test();
				if(return_code== 1) 
					goto Exit_Nocs_Test;
				break;
			case 97:
			case 98:
				goto Exit_Nocs_Test;
				break;
			case 99:
				exit (0);				
			default:
				break;		
		
		}	
  	}

Exit_Nocs_Test:	
  	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);
	
   	return 0;
}

TCsdStatus KLT_1_R2R_Test(void)
{
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus errCode; 
	TCsd4BytesVector ulValue;
	unsigned int intNuid;
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
	errCode = csdGetNuid(ulValue);

	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}

	intNuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];
	
	printf("NUID: %x \n", intNuid);
	
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	
	pxR2RKeyPathHandle->initHandle = &csdInit;
	
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, klt1InEncTdesEcb, sizeof(TCsdUnsignedInt8)*8);

	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	csdEncryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		klt1CipheredProtectingKeysEncTdesEcb[0], 
		klt1CipheredContentKeyEncTdesEcb[0], 
		16, 
		NULL, 0, FALSE,
		pxR2RKeyPathHandle,
		dataIn,
    		dataOut,
    		8
    		);
		
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  klt1OutEncTdesEcbTable[intNuid][0], 8 ))
		printf("KLT R2R Symmetric Key at penultimate stage: Test Successful\n");
	else
		printf("KLT R2R Symmetric Key at penultimate stage: Test Failed\n");


	printf("dataIn = ");
	for( i=0; i<8; i++)
		printf("0x%x ", *(dataIn+i));
	printf("\n");

	csdEncryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		klt1CipheredProtectingKeysEncTdesEcb[1], 
		klt1CipheredContentKeyEncTdesEcb[1], 
		16, 
		NULL, 0, FALSE,
		pxR2RKeyPathHandle,
		dataIn,
    		dataOut,
    		8
    		);
		
	printf("dataOut = ");
	for( i=0; i<8; i++)
		printf("0x%02x ", *(dataOut+i));
	printf("\n");

	if(memcmp( dataOut,  klt1OutEncTdesEcbTable[intNuid][1], 8 ))
		printf("KLT R2R Symmetric Key at last stage: Test Successful\n");
	else
		printf("KLT R2R Symmetric Key at last stage: Test Failed\n");


	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
	
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	

}

void klt_message_callback(void *context, int param)
{
      	const uint8_t *buffer;
        size_t size;
        int  message_length;
    	uint8_t evenKey[8], oddKey[8];	
    	uint8_t header = 0;	
#if 0		
	unsigned char klt2CipheredProtectingKeysTable[16][16] = {
		{0xEF , 0xCD, 0xAB,  0x89,  0x67,  0x45,  0x23, 0x01,  
		0x01, 0xEF,  0xCD,  0xAB,  0x89,  0x67,  0x45, 0x23}
	};
#endif
    	BSTD_UNUSED(param);

		
	while (1) 
	{
		/* wait 5 seconds */
	
	
		if (NEXUS_Message_GetBuffer(msg, (const void **)&buffer, &size)) 
			break;

		if (!buffer)
			continue;

        	/* We should always get whole PAT's because maxContiguousMessageSize is 4K */
        	message_length = size;

  		if(header != buffer[0]) 
		{
			header = buffer[0];
	
			if (header  == 0x81)
			{
				BKNI_Memcpy(&evenKey[0], (char *)buffer  + ECM_DATA_OFFSET, sizeof(uint8_t)*8);
				BKNI_Memcpy(&oddKey[0], (char *)buffer  + ECM_DATA_OFFSET+8, sizeof(uint8_t)*8);
			}
			else if (header == 0x80)
			{
				BKNI_Memcpy(&oddKey[0], (char *)buffer  +  ECM_DATA_OFFSET, sizeof(uint8_t)*8);
				BKNI_Memcpy(&evenKey[0], (char *)buffer  + ECM_DATA_OFFSET+8, sizeof(uint8_t)*8);
			}


			if(use_sw_keys_flag)
			{
				if(csdSetClearTextDscHostKeys(
					CSD_Algorithm_DVB_CSA2, 
					(TCsdUnsignedInt8*)&(oddKey[0]), 
					8,
					(TCsdUnsignedInt8*)&(evenKey[0]), 
					8,
					(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->VideoChannelInfo))!=CSD_NO_ERROR)
					printf("Error: csdSetClearTextDscHostKeys failed!\n"); 

				if(csdSetClearTextDscHostKeys(
					CSD_Algorithm_DVB_CSA2, 
					(TCsdUnsignedInt8*)&(oddKey[0]), 
					8,
					(TCsdUnsignedInt8*)&(evenKey[0]), 
					8,
					(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->AudioChannelInfo))!=CSD_NO_ERROR)
					printf("Error: csdSetClearTextDscHostKeys failed!\n"); 
			}
  			else
  			{
				/* Load Video key */
				if(csdSetProtectedDscContentKeys(
					CSD_Algorithm_DVB_CSA2,
		     			klt2CipheredProtectingKeysTable[global_nuid],
		  			(TCsdUnsignedInt8*)&(oddKey[0]), 
		      			 8,
		 			(TCsdUnsignedInt8*)&(evenKey[0]),
		        		8,
		       		(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->VideoChannelInfo)
					)!=CSD_NO_ERROR)
					printf("Error: csdSetProtectedDscContentKeys failed!\n"); 

				/* Load Audio key */
				if(csdSetProtectedDscContentKeys(
					CSD_Algorithm_DVB_CSA2,
		     			klt2CipheredProtectingKeysTable[global_nuid],
		  			(TCsdUnsignedInt8*)&(oddKey[0]), 
		      			 8,
		 			(TCsdUnsignedInt8*)&(evenKey[0]),
		        		8,
		       		(TCsdDscKeyPathHandle *)&(((SCsdDscAVPair *)context)->AudioChannelInfo)
					)!=CSD_NO_ERROR)
					printf("Error: csdSetProtectedDscContentKeys failed!\n"); 		  				
  			}
		}
		NEXUS_Message_ReadComplete(msg, message_length);
	}

	return;
}

TCsdStatus  KLT_2_TS_Test(void)
{
	NEXUS_InputBand inputBand;
    	NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    	NEXUS_StcChannelHandle stcChannel;
    	NEXUS_PlatformConfiguration 	platformConfig;
    	NEXUS_VideoDecoderHandle vdecode;
    	NEXUS_VideoDecoderStartSettings videoProgram;
    	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    	NEXUS_ParserBandSettings parserBandSettings;
    	NEXUS_StcChannelSettings stcSettings;
    	NEXUS_DisplayHandle display;
    	NEXUS_VideoWindowHandle window;
    	NEXUS_VideoWindowSettings windowSettings;
    	NEXUS_DisplaySettings displaySettings;
    	NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    	NEXUS_AudioDecoderStartSettings audioProgram;
    	NEXUS_MessageSettings settings;
    	NEXUS_MessageStartSettings startSettings;
    	NEXUS_PidChannelSettings pidChannelSettings;
    	NEXUS_PidChannelHandle pidChannel;
    	NEXUS_PidChannelStatus pidStatus; 
 	TCsdInitParameters csdInit;
	SCsdDscAVPair ChannelInfo;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	char text_input[20];
	int select_test;
	TCsd4BytesVector ulValue;

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}

	status = csdGetNuid(ulValue);

	if(status) 
	{
		printf("Error: can not get NUID \n"); 
	}

	global_nuid = (long)(ulValue[0])<<24 | (long)(ulValue[1])<<16 |(long)(ulValue[2])<<8 |(long) ulValue[3];

	ChannelInfo.VideoChannelInfo.initHandle = &csdInit;
	ChannelInfo.AudioChannelInfo.initHandle = &csdInit;

	/* Bring up all modules for a platform in a default configuraiton for this platform */
    	NEXUS_Platform_GetConfiguration(&platformConfig);

    	/* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    	NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    	/* Map a parser band to the streamer input band. */
    	parserBand = NEXUS_ParserBand_e0;
     	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
     	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
     	parserBandSettings.sourceTypeSettings.inputBand = inputBand;
     	parserBandSettings.transportType = NEXUS_TransportType_eTs;
     	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    	/* Open the audio and video pid channels */
    	videoPidChannel = NEXUS_PidChannel_Open(parserBand, symmetricalContentKeyScrambledServiceVideoPid, NULL);
    	audioPidChannel = NEXUS_PidChannel_Open(parserBand, symmetricalContentKeyScrambledServiceAudioPid, NULL);

    	/* Open the StcChannel to do lipsync with the PCR */
    	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    	stcSettings.timebase = NEXUS_Timebase_e0;
    	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    	stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
    		the audio outputs. */
    	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    	videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    	videoProgram.pidChannel = videoPidChannel;
    	videoProgram.stcChannel = stcChannel;
    	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    	audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    	audioProgram.pidChannel = audioPidChannel;
    	audioProgram.stcChannel = stcChannel;
	
    	NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
    	ChannelInfo.VideoChannelInfo.initHandle->pidchannel = pidStatus.pidChannelIndex;
    	NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
    	ChannelInfo.AudioChannelInfo.initHandle->pidchannel  = pidStatus.pidChannelIndex;

    	/* Bring up audio decoders and outputs */
    	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
	
    	NEXUS_AudioOutput_AddInput(
    		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
    		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
    	);
	
    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
       	NEXUS_AudioOutput_AddInput(
            		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            		NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed)
            	);
    	else
        	NEXUS_AudioOutput_AddInput(
        		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo)
            	);

    	/* bring up display */
    	NEXUS_Display_GetDefaultSettings(&displaySettings);

    	displaySettings.displayType = NEXUS_DisplayType_eAuto;
    	displaySettings.format = NEXUS_VideoFormat_eNtsc;
    	displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;

    	display = NEXUS_Display_Open(0, &displaySettings);
    	window = NEXUS_VideoWindow_Open(display, 0);
	
    	#if NEXUS_NUM_COMPONENT_OUTPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    	#endif
    	#if NEXUS_NUM_SCART_INPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
    	#if NEXUS_NUM_SCART_INPUTS>1
    	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
    	#endif
    	#endif
    	#if NEXUS_NUM_COMPOSITE_OUTPUTS
    	NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    	#endif
    
    	/* bring up decoder and connect to display */
    	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    	windowSettings.visible = true;
    	NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    	/* Start Decoders */
    	NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    	NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
	
    	if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
        	/* Only pass through AC3 */
        	NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

    	/* Prepare XPT to recognize PMT. */
    	NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    	pidChannelSettings.requireMessageBuffer = true;
    	pidChannel = NEXUS_PidChannel_Open(parserBand, symmetricalContentKeyScrambledServiceECMPid, &pidChannelSettings);

    	NEXUS_Message_GetDefaultSettings(&settings);
    	settings.dataReady.callback = klt_message_callback;
     	settings.dataReady.context = &ChannelInfo;
    	settings.maxContiguousMessageSize = 4096;
    	msg = NEXUS_Message_Open(&settings);
    	BDBG_ASSERT(msg);

    	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
    	startSettings.pidChannel = pidChannel;

	/* Force to use protected content keys. */
	use_sw_keys_flag = false;
		
    	/* use the default filter for any data */
    	if (NEXUS_Message_Start(msg, &startSettings)) 
		goto Exit_KLT_2_TS_Test;	

	while (1) 
	{
		select_test = 0;
		printf("\n\n");
		printf("\nRequest to terminate KLT 2 TS Test :\n");
	      	printf("0) Print menu\n");
		printf("98) Upper Menu \n");
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);

		switch(select_test)
		{
			case 0:  
				break;
			case 98:
				goto Exit_KLT_2_TS_Test;			
				break;
			case 99:
				exit (0);
			default:
				printf("Unknown choice...\n");
				break;			
		}
	}

	
Exit_KLT_2_TS_Test:

	NEXUS_Message_Stop(msg);
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);		
	NEXUS_VideoDecoder_Stop(vdecode);
	NEXUS_VideoDecoder_Close(vdecode);	
	NEXUS_AudioDecoder_Close(pcmDecoder);	
	NEXUS_AudioDecoder_Close(compressedDecoder);
	printf("No stream input! return..\n");

  	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return 1;
}

int  KeyLadderTest(void)
{
	char text_input[20];
	int select_test;

 	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return 1;
	}
	
  	while(1)
	{
		select_test = 0;
		
		printf("\n\n");
		printf("\nKey Ladder Tests....\n");
		printf("0) Print menu\n");
		printf("1) [KLT_1] R2R Crypto Engine Symmetric keys injection.\n");
		printf("2) [KLT_2] TS Crypto Engine Symmetric keys injection.\n");
		printf("97) Upper Menu \n");
		printf("98) Root Menu \n");
		printf("99) Quit application \n"); 
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch(select_test)
		{
			case 0:
				break;
			case 1:
				KLT_1_R2R_Test();
				break;
			case 2:
				KLT_2_TS_Test();
				break;
			case 97:
			case 98:
				goto Exit_Nocs_Test;
				break;
			case 99:
				exit (0);				
			default:
				break;		
		
		}	
  	}

Exit_Nocs_Test:	
  	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);
	
   	return 0;
}

void PrintStatus(void)
{
	TCsdStatus errCode; 
	TCsd20CharsString stringVar;  
	TCsd4BytesVector ulValue;
	TCsdBootMode bootMode;
	TCsdScsActivationFlag scsFlag;
	TCsdScsTotalAreaSize scsAreaSize;
	TCsdDebugInterfaceAccessMode accessMode;
	TCsdKeyLadderDeactivationFlag klFlag;
	TCsdUnsignedInt8 char16Value[16];
 	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	int i;
		
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	
	if(csdInitialize(&csdInit))
		printf("Failed to init CSD\n"); 
	
	printf("\n********************************************************************\n");
	printf("Manufacturer: Broadcom Corporation \n");
	printf("Chipset: BCM7425\n");
	
	csdGetSoftwareVersion(stringVar); 
	printf("CSD version:  %s\n", stringVar);
	
	memset(stringVar, 0, sizeof(stringVar));
	csdGetChipRevision(stringVar); 
	printf("Chip revision:  %s\n", stringVar);
	
	memset(stringVar, 0, sizeof(stringVar));
	csdGetChipExtension(stringVar);
	printf("Chip Extension:  %s\n", stringVar);
	
	
	errCode = csdGetNuid(ulValue);
	if(errCode) 
	{
		printf("Error: can not get NUID \n"); 
	}
	printf("NUID: 0x%02x%02x%02x%02x \n", ulValue[0], ulValue[1],ulValue[2], ulValue[3]);
	
	errCode = csdGetNUIDCheckNumber(ulValue);
	if(errCode) 
	{
		printf("Error: can not get check number \n"); 
	}
	printf("Check number is : 0x%02x%02x%02x%02x \n",  ulValue[0], ulValue[1],ulValue[2], ulValue[3]);

	errCode = csdGetMarketSegmentId(ulValue);
	if(errCode)
	{
		printf("Error: can not get Market Segment ID \n"); 
	}
	printf("Market Segment ID: 0x%02x%02x%02x%02x \n", ulValue[0], ulValue[1],ulValue[2], ulValue[3]);

	errCode = csdGetDvrKey(char16Value);
	if(errCode)
	{
		printf("Error: can not get DVR Key Value: \n"); 
	}
	printf("DVR Key Value is: ");
	
	for( i = 0; i < 16; i++ )
		printf("%x ", char16Value[i]);
	printf("\n");
	
	csdGetBootMode(&bootMode);
	switch(bootMode)
	{
		case CSD_BOOT_NOT_SET:
			printf("Boot mode is NOT_SET\n");
			break;				
		case CSD_BOOT_FROM_SPI:
			printf("Boot mode is BOOT_FROM_SPI\n");
			break;
		case CSD_BOOT_FROM_NAND:
			printf("Boot mode is BOOT_FROM_NAND\n");
			break;
		case CSD_BOOT_FROM_NOR:
			printf("Boot mode is BOOT_FROM_NOR\n");
			break;			
		default:
			printf("Error - Unknown boot mode\n");
			break;
	}
	
	csdGetScsActivationFlag(&scsFlag);
	switch(scsFlag)
	{
		case CSD_SCS_ACTIVATION_FLAG_SET:
			printf("SCS Activation Flag is set\n");
			break;
		default:
			printf("SCS Activation Flag is not set\n");
			break;
	}
	
	csdGetScsTotalAreaSize(&scsAreaSize);
	switch(scsAreaSize)
	{
		case CSD_SCS_TOTAL_AREA_SIZE_NOT_SUPPORTED:
			printf("SCS Total Area Size is not supported\n");
			break;
		default:
			printf("Error - get SCS Total Area Size return an invalid value\\n");
			break;
	}
	
	errCode = csdGetStbCaSn(ulValue);
	if(errCode)
	{
		printf("Error: can not get STB CA serial number \n"); 
	}
	printf("STB CA SN: 0x%02x%02x%02x%02x \n", ulValue[0], ulValue[1],ulValue[2], ulValue[3]);	
	
	csdGetDebugInterfaceProtectionLevel(&accessMode);
	switch(accessMode)
	{
		case CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN:
			printf("Debug Interface is open\n");
			break;
		case CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED:
			printf("Debug Interface is password protected\n");
			break;
		case CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED:
			printf("Debug Interface is closed\n");
			break;
		default:
			printf("Debug Interfacs state is inconsistent\n");
			break;
	}
	
	csdGetKeyLadderDeactivationFlag(&klFlag);
	switch(klFlag)
	{
		case CSD_KEY_LADDER_DEACTIVATION_FLAG_SET:
			printf("Key ladder is deactivated\n");
			break;
		default:
			printf("Key ladder is NOT deactivated\n");
			break;
	}

	printf("********************************************************************\n");

  	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	return;
}


int main(void)
{    
	char text_input[20];
	int select_test;
	
	NEXUS_PlatformSettings platformSettings;

	/* Platform init */
    	NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    	platformSettings.openFrontend = true;
    	NEXUS_Platform_Init(&platformSettings);

	PrintStatus();
	
	/* Menu of NOCS 1.1 Tests. */
    	while(1) 
	{	
		select_test = 0;
		printf("\n\n");
		printf("\nSelect NOCS test to Test:\n");
	      	printf("0) Print menu\n");
	      	printf("1) R2R RAM2RAM Crypto Engine \n");
	      	printf("2) DSC TS Crypto Engine \n");
	      	printf("3) KLT Key Ladders Tests \n");
	      	printf("4) SCR Stream Crypto Engine \n");
	      	printf("5) PV Persistent Value Configuration \n");
		printf("97) Upper menu \n");
		printf("98) Root menu \n");		
		printf("99) Quit application \n");
		
   		fputs("Choice: ", stdout);   
   		fflush(stdout);
		
 		fgets(text_input, sizeof text_input, stdin);
		sscanf(text_input, "%d", &select_test);
		
		switch(select_test)
		{
			case 0:
				break;
			case 1:	
				printf("Running R2R RAM2RAM Crypto Engine Test...\n");
				Ram2RamCryptoEngineTest();
				break;
			case 2:
				printf("Running DSC TS Crypto Engine Test...\n");
				TransportStreamCryptoEngineTest();
				break;	
			case 3:
				printf("Running KLT Key Ladders Tests...\n");
				KeyLadderTest();
				break;	
			case 4:
				printf("Running SCR Stream Crypto Engine...\n");
				StreamCryptoEngineTest();
				break;				
			case 5:
				printf("Running PV Persistent Value Configuration Test...\n");
				Test_OTP();
				break;
			case 97:
			case 98:
				break;
			case 99:
				goto BHSM_P_DONE_LABEL;
				break;
			default:
				printf("Unknown choice...\n");
				break;	
		}
	}

	BHSM_P_DONE_LABEL:	
	return 0;
}

