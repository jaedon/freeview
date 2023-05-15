/***************************************************************************
 *     (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: csd.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 4/25/11 12:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/csdlib/7125/csd.c.gpg $
 * 
 * 9   4/25/11 12:30p gracew
 * SW7125-920: Memory release for all the pointers of each API before return status
 * 
 * 8   2/7/11 2:28p dbayani
 * SWSECURITY-33: Update M2M CSD APIs to free key slot instead of relying on caller to call csdTerminate.
 * 
 * 
 **************************************************************************/

#include <string.h>

#include "bchp_sun_top_ctrl.h"
#include "bchp_nand.h"

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "nexus_core_security.h"     /* to pick up definitions for NEXUS_KeySlotHandle */
#include "nexus_security_types.h"
#include "priv/nexus_security_priv.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nocs3x_csd_impl.h"
#include "nocs3x_csd.h"
#include "bdbg.h"
#include "bint.h"
#include "bchp_jtag_otp.h"

#if (BCHP_CHIP==7403) 
#include "bsp_s_otp_7403A0.h"
#elif (BCHP_CHIP==7405) 
#include "bsp_s_otp_7405A0.h"
#elif (BCHP_CHIP==7401) 
#include "bsp_s_otp_7401B0.h"
#elif (BCHP_CHIP == 7325)
#include "bsp_s_otp_7325B0.h"
#elif (BCHP_CHIP == 7335)
#include "bsp_s_otp_7335B0.h"
#elif (BCHP_CHIP == 7125)
#include "bsp_s_otp_common.h"
#elif (BCHP_CHIP == 7358)
#include "bsp_s_otp_common.h"
#elif (BCHP_CHIP == 7231)
#include "bsp_s_otp_common.h"
#elif (BCHP_CHIP == 7425)
#include "bsp_s_otp_common.h"
#elif (BCHP_CHIP == 7552)
#include "bsp_s_otp_common.h"
#elif (BCHP_CHIP == 7429)
#include "bsp_s_otp_common.h"
#else
#error "unknown chip"
#endif

#include <stdio.h>

BDBG_MODULE(NCSD);
bool CSD_INITIALIZED = FALSE;


TCsdInitParameters*  csdInitParameters[CSD_MAX_INDEX]={NULL};

/**********************************************************************************************
						 CSD implementation specific structures
**********************************************************************************************/

/**********************************************************************************************
						 CSD implementation specific  variables
**********************************************************************************************/

static unsigned long 			gCrcTab [256];

#define BHSM_SLOT_NUM_INIT_VAL		0xFFFF

/**********************************************************************************************
						 CSD implementation specific internal functions/macros
**********************************************************************************************/

static unsigned long cmpCrc32 (unsigned char * xbufInp, int xbufLen)
{
  	unsigned long crcVal;
  	
 	 crcVal = 0xFFFFFFFF;
 	 while (xbufLen--) {
    		crcVal = ((crcVal >> 8) & 0x00FFFFFF) ^ gCrcTab[(crcVal ^ *xbufInp++) & 0xFF];
 	 }
	 return (crcVal ^ 0xFFFFFFFF);
}

static void genCrc32Tab (void)
{
  	unsigned long crcVal, poly;
  	register int i, j;

  	poly = 0xEDB88320L;
  	for (i = 0; i < 256; i++) 
	{
  	 	 crcVal = i;
  		  for (j = 8; j > 0; j--) 
		  {
		      if (crcVal & 1) 
			{
				crcVal = (crcVal >> 1) ^ poly;
      			} else {
				crcVal >>= 1;
      			}
    		}
    		gCrcTab [i] = crcVal;
  	}
}

void byteSwap(
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

static void CompleteCallback(void *pParam, int iParam)
{
	iParam=iParam;
    BKNI_SetEvent(pParam);
}


TCsdStatus csdR2RCryptoOperation(
	NEXUS_KeySlotHandle keyHandle,
  	const TCsdUnsignedInt8* pxInputData, 
       TCsdUnsignedInt8* pxOutputData, 
       TCsdSize xDataSize	
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
	TCsdStatus    retCode  =  CSD_NO_ERROR;
	BKNI_EventHandle event;
	NEXUS_Error rc;

	
#if(CFG_BIG_ENDIAN==1)
	TCsdSize i;
#endif
	BKNI_CreateEvent(&event);

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

	/* Copy the input data to M2M input buffer. */
	BKNI_Memcpy((void *)input_data, (void *)pxInputData, xDataSize );

#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
            byteSwap(&(input_data[i]));
#endif

	/* Open DMA handle */
	dma = NEXUS_Dma_Open(0, NULL);

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = keyHandle;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = CompleteCallback;
	 jobSettings.completionCallback.context = event;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = input_data;  
	blockSettings.pDestAddr = output_data; 
	blockSettings.blockSize = xDataSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

	rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	/* Wait for M2M Encryption to complete. */
	if (rc == NEXUS_DMA_QUEUED )
	{
			if (BKNI_WaitForEvent(event, BKNI_INFINITE) ) 
		{
		}
	}
		else if ( rc != NEXUS_SUCCESS )
		{
			 printf("\nDMA transfer failed \n");
	
		}


#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
            byteSwap(&(output_data[i]));
#endif

	/* Copy the data to M2M output buffer. */
	BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );

CSD_P_DONE:
		
	/* Done.  Close DMA. */
	NEXUS_DmaJob_Destroy(dmaJob);

	NEXUS_Dma_Close(dma);
	BKNI_DestroyEvent(event);

	NEXUS_Memory_Free(output_data);
	NEXUS_Memory_Free(input_data);

   	return retCode;
	

}


/**********************************************************************************************
						 CSD Public Functions
**********************************************************************************************/
TCsdStatus csdInitialize(
	TCsdInitParameters*  pxInitParameters)
{
	int csd_index;
	
	/* Make sure input parameter pointer is not NULL. */
	if(pxInitParameters==NULL) 
		return CSD_ERROR;

	/* Find an available CSD. */	
	for(csd_index=0;csd_index<CSD_MAX_INDEX;csd_index++)
	{
		if(csdInitParameters[csd_index] == NULL) 
			break;
	}

	/* If no CSD is available return error. */
	if(csd_index==CSD_MAX_INDEX)
		return CSD_ERROR_DRIVER_ALREADY_INITIALIZED;

	csdInitParameters[csd_index]=pxInitParameters;

	/* initialize linked list structure for storing key handle */
	pxInitParameters->handleNodeCount = 0;
	pxInitParameters->rootHandleNode = NULL;
	pxInitParameters->pidchannel = 256;
	pxInitParameters->scramblingControlBits = 0;

 	CSD_INITIALIZED = TRUE;
		
	return CSD_NO_ERROR;

}

TCsdStatus csdFreeHandleNode(
	TCsdInitParameters*  pxInitParameters, 
	NEXUS_KeySlotHandle handle)
{
	handleNode curNode, prevNode;

	/* always start at root */
	curNode  = pxInitParameters->rootHandleNode;
	prevNode = pxInitParameters->rootHandleNode;

	/* Search through all nodes for the key slot handle we want to free. */
	while (curNode)
	{
		if (handle == curNode->handle)
			break;
		prevNode = curNode;
		curNode  = curNode->nextNode;
	}

	
	if (curNode)
	{
		/* found the node to release */
		NEXUS_Security_FreeKeySlot(handle);

		/* remove the node from the link */
		prevNode->nextNode = curNode->nextNode;
		if (curNode == pxInitParameters->rootHandleNode)
			pxInitParameters->rootHandleNode = curNode->nextNode;

		pxInitParameters->handleNodeCount -= 1;

		/* release the node storage */
		NEXUS_Memory_Free((void *)curNode);

	}
	else
		/* The linked list does not contain the requested handle, return error. */
		return CSD_ERROR_INVALID_PARAMETERS;

	return CSD_NO_ERROR;
	
}

static void csdInsertHandleNode(
	TCsdInitParameters*  pxInitParameters, 
	NEXUS_KeySlotHandle handle)
{
	handleNode node;

	/* allocate the memory for the node storage */
	NEXUS_Memory_Allocate(sizeof(struct keyHandleNode), NULL, (void *)&node);
	node->handle = handle;

	/* always insert to the front */
	node->nextNode = pxInitParameters->rootHandleNode;
	pxInitParameters->rootHandleNode   = node;
	pxInitParameters->handleNodeCount += 1;

	return;
	
}

TCsdStatus csdTerminate(
	TCsdTerminateParameters*  pxTerminateParameters)
{
	handleNode node;
	handleNode nextNode;
	int csd_index;
	
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxTerminateParameters == NULL)
		return CSD_ERROR_INVALID_PARAMETERS;

	if(pxTerminateParameters->initParam == NULL)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Find the CSD to terminate. */
	for(csd_index=0;csd_index<CSD_MAX_INDEX;csd_index++)
	{
		if(csdInitParameters[csd_index] == pxTerminateParameters->initParam ) 
			break;
	}

	/* If CSD does not exist, then return error. */
	if(csd_index==CSD_MAX_INDEX)
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* first check to see if the request is to free a single keyslot handle */
	if (pxTerminateParameters->keyHandleToRelease)
	{
		/* Make sure there is at least 1 node. */
		if (pxTerminateParameters->initParam->handleNodeCount)
		{
			/* Remove the association between the PID channel and key slot. */
			if(pxTerminateParameters->initParam->pidchannel!=256)
				NEXUS_Security_RemovePidChannelFromKeySlot(pxTerminateParameters->keyHandleToRelease,
					pxTerminateParameters->keyHandleToRelease->pidChannel);

			/* Free the node. */
			return csdFreeHandleNode(
				pxTerminateParameters->initParam, 
				pxTerminateParameters->keyHandleToRelease); 
		}
		else
			/* No node to free. */
			return CSD_ERROR_INVALID_PARAMETERS;

		return CSD_NO_ERROR;
	}

	/* If the input doesn't specify a single node to release, then release all. */
	if (pxTerminateParameters->initParam->handleNodeCount)
	{
		node = pxTerminateParameters->initParam->rootHandleNode;
		
		while (node)
		{
			nextNode = node->nextNode;
			
			/* Remove the association between the PID channel and key slot. */
			if(pxTerminateParameters->initParam->pidchannel!=256)
				NEXUS_Security_RemovePidChannelFromKeySlot(node->handle,
					node->handle->pidChannel);

			if(csdFreeHandleNode(
				pxTerminateParameters->initParam, 
				node->handle) != CSD_NO_ERROR)
				return CSD_ERROR_INVALID_PARAMETERS;

			node = nextNode;
		}
	}

	csdInitParameters[csd_index]=NULL;

	/* After terminating CSD, check if any CSD is intiailized. */
	for(csd_index=0;csd_index<CSD_MAX_INDEX;csd_index++)
	{
		if(csdInitParameters[csd_index] != NULL ) 
			break;
	}

	/* If CSD does not exist, set CSD_INITIALIZED to false. */
	if(csd_index==CSD_MAX_INDEX)
		CSD_INITIALIZED = FALSE;	
	
	return CSD_NO_ERROR;
	
}

TCsdStatus csdGetSoftwareVersion(
	TCsd20CharsString xSoftwareVersion)
{
	static char SoftwareVersion[] = "NOCS-3.0.0-7231-1.0"; 

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	
	if(xSoftwareVersion==NULL)
		return CSD_ERROR;
	
	BKNI_Memcpy(xSoftwareVersion, SoftwareVersion, sizeof(SoftwareVersion));
	
	return CSD_NO_ERROR;
}

TCsdStatus csdGetChipRevision(
  TCsd20CharsString xChipRevision)
{
	
	unsigned char	chipVersion[2] = "A0"; 
	uint32_t uChipNum; 

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(xChipRevision ==NULL)
		return CSD_ERROR;

	
	/* "chip rev=71250020" */

	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID, &uChipNum );
	uChipNum &= 0xff;
	chipVersion[0] +=(uChipNum>>4);
	chipVersion[1] += uChipNum & 0x0f;

	xChipRevision[0] = chipVersion[0];
	xChipRevision[1] = chipVersion[1];
	
	return CSD_NO_ERROR;
	
}

TCsdStatus csdGetNuid (
	TCsd4BytesVector xNuid)
{
	BERR_Code errCode = BERR_SUCCESS;
	NEXUS_ReadOtpIO	otpIo;
	unsigned int i;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(xNuid ==NULL)  
		return CSD_ERROR;

	/* Read the ID from OTP. */
	errCode = NEXUS_Security_ReadOTP (
		NEXUS_OtpCmdReadRegister_eKeyID,
		NEXUS_OtpKeyType_eA,
		&otpIo);

	if (errCode != BERR_SUCCESS) 
		return CSD_ERROR;

	/* NUID is the second Dword */
	BKNI_Memcpy((void *)xNuid, (void *)&(otpIo.otpKeyIdBuf[4]), CSD_NUID_LENGTH/8 );
	
	for (i=0; i<(CSD_NUID_LENGTH/8); i++) 
		BDBG_MSG((" %02X ", xNuid[i])) ;
	
	return CSD_NO_ERROR;
}

TCsdStatus csdGetNUIDCheckNumber(
  TCsd4BytesVector xCheckNumber
)
{

	TCsdStatus      retCode = CSD_NO_ERROR;
   
	TCsdTerminateParameters terminateParameter;
	unsigned int i;
	NEXUS_ReadOtpIO readOtpIO;
	NEXUS_OtpCmdReadRegister opCode = NEXUS_OtpCmdReadRegister_eKeyHash;
	NEXUS_OtpKeyType keyType = NEXUS_OtpKeyType_eA ;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xCheckNumber == NULL )
		return CSD_ERROR_INVALID_PARAMETERS;

	if(NEXUS_Security_ReadOTP(opCode, keyType, &readOtpIO)){
		BDBG_ERR(("NEXUS_Security_ReadOTP error!\n"));
		return CSD_ERROR;
			   }

#if 0
		printf("read Otp IO\n");
		for( i=0; i<readOtpIO.otpKeyIdSize; i++ )
			printf("0x%02x ", readOtpIO.otpKeyIdBuf[i]);
	printf("\n");
#endif



/*
	xCheckNumber[0] = readOtpIO.otpKeyIdBuf[0] ;
	xCheckNumber[1] = readOtpIO.otpKeyIdBuf[1] ; 
	xCheckNumber[2] = readOtpIO.otpKeyIdBuf[2] ; 
	xCheckNumber[3] = readOtpIO.otpKeyIdBuf[3] ; 
*/
	BKNI_Memcpy(xCheckNumber, readOtpIO.otpKeyIdBuf, sizeof(xCheckNumber)); 
CSD_P_DONE:
	
	


	return (retCode);
}

TCsdStatus csdGetNuid64
(
  TCsd8BytesVector xNuid64
)
{
	BERR_Code errCode = BERR_SUCCESS;
	NEXUS_ReadOtpIO	otpIo;
	unsigned int i;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(xNuid64 ==NULL)  
		return CSD_ERROR;

	/* Read the ID from OTP. */
	errCode = NEXUS_Security_ReadOTP (
		NEXUS_OtpCmdReadRegister_eKeyID,
		NEXUS_OtpKeyType_eA,
		&otpIo);

	if (errCode != BERR_SUCCESS) 
		return CSD_ERROR;

	/* NUID is the second Dword */
	BKNI_Memcpy((void *)xNuid64, (void *)&(otpIo.otpKeyIdBuf[0]), CSD_NUID_LENGTH/8 );
	
	for (i=0; i<(CSD_NUID_LENGTH/8); i++) 
		BDBG_MSG((" %02X ", xNuid64[i])) ;
	
	return CSD_NO_ERROR;
}

TCsdStatus csdGetCSCDCheckNumber(
	const TCsdUnsignedInt8		xCSCData[16],
	TCsd4BytesVector xCheckNumber
)
{
	TCsdR2RKeyPathHandle *r2rHandle;	
	unsigned char *crcInput;
	unsigned char *xCSCDataInput;
	unsigned long uCSCDcheckNum;
	TCsdSize	contKeySize = 16;
	TCsdStatus  retCode = CSD_NO_ERROR;
	TCsdUnsignedInt16      xEmi = 0x4041;     
	TCsdTerminateParameters terminateParameter;
	unsigned int i;
	const TCsdUnsignedInt8 CONSTANTs[2][16] = 
		{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},
		{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},};
	
	const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};



	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	
	if( xCheckNumber == NULL )
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xCSCDataInput);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&r2rHandle);

	r2rHandle->initHandle = csdInitParameters[0]; 

	/* Copy the xCSCData to M2M input buffer. */
	BKNI_Memcpy((void *)xCSCDataInput, (void *)xCSCData, 16 );
	
#ifdef CSD_DEBUG
	printf("xCSCData[16]\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", xCSCDataInput[i]);
	printf("\n");	
#endif

r2rHandle->keyHandle = NULL;
	r2rHandle->bEncrypt = true;
	
#if 1
	if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)){
				   printf("R2R csdSetProtectedRam2RamContentKeys returns error \n");
			   }
#endif

	csdEncryptData(xEmi, r2rHandle, NULL, 0, xCSCDataInput, crcInput, 16/*inputVector.sizeDataIn*/); 

#if 0
	if(csdEncryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		CONSTANT_1, 
		CONSTANT_2, 
		contKeySize, 
  		NULL,
  		0, 
  		FALSE,
  		r2rHandle,
  		xCSCDataInput,
        	crcInput,
        	16
        	) != CSD_NO_ERROR)
	{
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
#endif
	
#ifdef CSD_DEBUG
	printf("crcInput\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", crcInput[i]);
	printf("\n");
#endif

	genCrc32Tab();
	uCSCDcheckNum= cmpCrc32(crcInput,16) ;	
	
#ifdef CSD_DEBUG
	printf("CSC Data checkNum=0x%08x", uCSCDcheckNum);	
#endif

	xCheckNumber[3] = uCSCDcheckNum&0xff;
	xCheckNumber[2] = (uCSCDcheckNum>>8) &0xff; 
	xCheckNumber[1] = (uCSCDcheckNum>>16) &0xff; 
	xCheckNumber[0] = (uCSCDcheckNum>>24) &0xff; 

CSD_P_DONE:
	terminateParameter.initParam = csdInitParameters[0];
	
	terminateParameter.keyHandleToRelease = r2rHandle->keyHandle;
	if( csdTerminate(&terminateParameter)!=CSD_NO_ERROR){
		BDBG_ERR(("Fail to terminate R2R key handle."));
		retCode = CSD_ERROR;
	}
	
	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(xCSCDataInput);
	NEXUS_Memory_Free(crcInput);

	return retCode;	
}

TCsdStatus csdGetSTBCASNCheckNumber(
	const TCsdUnsignedInt8	 xSTBCASNData[4],
	TCsd4BytesVector xSTBCASNCheckNumber
)
{
	TCsdR2RKeyPathHandle * r2rHandle;	
	unsigned char *crcInput;
	unsigned char *xSTBCASNDataInput;
	unsigned long uSTBCASNcheckNum;	
	TCsdSize	contKeySize = 16;
	TCsdStatus  retCode = CSD_NO_ERROR;
		TCsdUnsignedInt16      xEmi = 0x4041;     
	TCsdTerminateParameters terminateParameter;
	unsigned int i;
	const TCsdUnsignedInt8 CONSTANTs[2][16] = 
		{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},
		{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},};
	
	const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};


	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xSTBCASNCheckNumber == NULL )
		return CSD_ERROR_INVALID_PARAMETERS;

#ifdef CSD_DEBUG
	printf("xSTBCASNData[16]\n");
	for( i=0; i<4; i++ )
		printf("0x%02x ", xSTBCASNData[i]);
	printf("\n");	
#endif


	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xSTBCASNDataInput);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&r2rHandle);

	r2rHandle->initHandle = csdInitParameters[0]; 

	/* Initialize data buffer to zero. */
	memset(xSTBCASNDataInput, 0, sizeof(unsigned char) * 16 );

	/* Copy the xSTBCASNData to M2M input buffer. */
	BKNI_Memcpy((void *)&(xSTBCASNDataInput[12]), (void *)xSTBCASNData, 4 );
	
#ifdef CSD_EBGU
	printf("padded r2rDataIn[16]\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", xSTBCASNDataInput[i]);
	printf("\n");	
#endif
	
r2rHandle->keyHandle = NULL;
	r2rHandle->bEncrypt = true;
	
#if 1
	if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)){
				   printf("R2R csdSetProtectedRam2RamContentKeys returns error \n");
			   }
#endif

	csdEncryptData(xEmi, r2rHandle, NULL, 0, xSTBCASNDataInput, crcInput, 16/*inputVector.sizeDataIn*/); 

#if 0
	if( csdEncryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		CONSTANT_1, 
		CONSTANT_2, 
		contKeySize, 
  		NULL, 
  		0, 
  		FALSE,
  		r2rHandle,
  		xSTBCASNDataInput,
        	crcInput,
        	16
        	) != CSD_NO_ERROR)
	{
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
#endif
	
#ifdef CSD_DEBUG
	printf("crcInput\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", crcInput[i]);
	printf("\n");
#endif

	genCrc32Tab();
	uSTBCASNcheckNum= cmpCrc32(crcInput,16) ;	
	
#ifdef CSD_DEBUG
	printf("STBCASN checkNum=0x%08x", uSTBCASNcheckNum);	
#endif

	xSTBCASNCheckNumber[3] = uSTBCASNcheckNum&0xff;
	xSTBCASNCheckNumber[2] = (uSTBCASNcheckNum>>8) &0xff; 
	xSTBCASNCheckNumber[1] = (uSTBCASNcheckNum>>16) &0xff; 
	xSTBCASNCheckNumber[0] = (uSTBCASNcheckNum>>24) &0xff; 

CSD_P_DONE:
	terminateParameter.initParam = csdInitParameters[0];
	
	terminateParameter.keyHandleToRelease = r2rHandle->keyHandle;
	if( csdTerminate(&terminateParameter)!=CSD_NO_ERROR){
		BDBG_ERR(("Fail to terminate R2R key handle."));
		retCode = CSD_ERROR;
	}
	
	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(xSTBCASNDataInput);
	NEXUS_Memory_Free(crcInput);
	
	return retCode;	
}

TCsdStatus csdGetDataIntegrityCheckNumber(
	const TCsdUnsignedInt8*		pxData,
	TCsdSize						xDataSize,
	TCsd4BytesVector 				xDataIntegrityCheckNumber
)
{
	TCsdR2RKeyPathHandle * r2rHandle;	
	unsigned char *r2rDataIn;	
	unsigned char *crcInput;
	unsigned long uDataIntegrityCheckNum;
	TCsdSize	contKeySize = 16;
	TCsdStatus  retCode = CSD_NO_ERROR;
		TCsdUnsignedInt16      xEmi = 0x4041;     
	TCsdTerminateParameters terminateParameter;
	unsigned int i;
	const TCsdUnsignedInt8 CONSTANTs[2][16] = 
		{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},
		{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;	

	if( xDataSize >16 || xDataSize < 1 )
		return CSD_ERROR_INVALID_PARAMETERS;
	
	if( xDataIntegrityCheckNumber == NULL )
		return CSD_ERROR_INVALID_PARAMETERS;
	
#ifdef CSD_DEBUG
	printf("pxData\n");
	for( i=0; i<xDataSize; i++ )
		printf("0x%02x ", pxData[i]);
	printf("\n");	
#endif

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&r2rDataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&r2rHandle);

	r2rHandle->initHandle = csdInitParameters[0]; 

	/* Initialize data buffer to zero. */
	memset(r2rDataIn, 0, sizeof(unsigned char) * 16 );

	/* Copy the xDataSize to M2M input buffer. */
	BKNI_Memcpy((void *)&(r2rDataIn[16-xDataSize]), (void *)pxData, xDataSize );
	
#ifdef CSD_DEBUG
	printf("padded r2rDataIn[16]\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", r2rDataIn[i]);
	printf("\n");	
#endif
r2rHandle->keyHandle = NULL;
	r2rHandle->bEncrypt = true;
	
#if 1
	if(csdSetProtectedRam2RamContentKeys(xEmi,r2rHandle,CONSTANTs,CONSTANT_2, 16)){
				   printf("R2R csdSetProtectedRam2RamContentKeys returns error \n");
			   }
#endif

	csdEncryptData(xEmi, r2rHandle, NULL, 0, r2rDataIn, crcInput, 16/*inputVector.sizeDataIn*/); 


#if 0
	if( csdEncryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		CONSTANT_1, 
		CONSTANT_2, 
		contKeySize, 
  		NULL, 
  		0, 
  		FALSE,
  		r2rHandle,
  		r2rDataIn,
        	crcInput,
        	16
        	) != CSD_NO_ERROR)
	{
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
#endif
	
#ifdef CSD_DEBUG
	printf("crcInput\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", crcInput[i]);
	printf("\n");
#endif

	genCrc32Tab();
	uDataIntegrityCheckNum= cmpCrc32(crcInput,16) ;	

#ifdef CSD_DEBUG
	printf("Data Integrity checkNum=0x%08x", uDataIntegrityCheckNum);	
#endif
	xDataIntegrityCheckNumber[3] = uDataIntegrityCheckNum&0xff;
	xDataIntegrityCheckNumber[2] = (uDataIntegrityCheckNum>>8) &0xff; 
	xDataIntegrityCheckNumber[1] = (uDataIntegrityCheckNum>>16) &0xff; 
	xDataIntegrityCheckNumber[0] = (uDataIntegrityCheckNum>>24) &0xff; 

CSD_P_DONE:
	terminateParameter.initParam = csdInitParameters[0];
	
	terminateParameter.keyHandleToRelease = r2rHandle->keyHandle;
	if( csdTerminate(&terminateParameter)!=CSD_NO_ERROR){
		BDBG_ERR(("Fail to terminate R2R key handle."));
		retCode = CSD_ERROR;
	}
	
	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(r2rDataIn);
	NEXUS_Memory_Free(crcInput);
	
	
	return (retCode);	
}


TCsdStatus csdSetBootMode(
	TCsdBootMode xBootMode)
{
	uint32_t ubootStrapReg; 
	ECsdBootMode strapBootMode;
		
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_STRAP_VALUE_0, &ubootStrapReg);

	ubootStrapReg &= BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK;
	ubootStrapReg = ubootStrapReg>>BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

	if(ubootStrapReg <7 )
	{
		strapBootMode = CSD_BOOT_FROM_NAND;
	}
	else if(ubootStrapReg <10 )
	{
		strapBootMode = CSD_BOOT_FROM_SPI;
	}
	else if(ubootStrapReg <12 )
	{
		strapBootMode = CSD_BOOT_FROM_NOR;
	}
	else
	{
		strapBootMode = CSD_BOOT_MODE_MAX;
	}

	if(xBootMode != strapBootMode)
		return CSD_ERROR_OPERATION_NOT_ALLOWED;


	return CSD_NO_ERROR;
}


TCsdStatus csdGetBootMode(
	TCsdBootMode* pxBootMode)
{
	uint32_t ubootStrapReg; 
	ECsdBootMode strapBootMode;
		
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_STRAP_VALUE_0, &ubootStrapReg);

	ubootStrapReg &= BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK;
	ubootStrapReg = ubootStrapReg>>BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

	if(ubootStrapReg <7 )
	{
		strapBootMode = CSD_BOOT_FROM_NAND;
	}
	else if(ubootStrapReg <10 )
	{
		strapBootMode = CSD_BOOT_FROM_SPI;
	}
	else if(ubootStrapReg <12 )
	{
		strapBootMode = CSD_BOOT_FROM_NOR;
	}
	else
	{
		*pxBootMode = CSD_BOOT_MODE_MAX;
		return CSD_NO_ERROR;
	}
		
	/* Store boot mode. */
	*pxBootMode = strapBootMode;

	return CSD_NO_ERROR;
}

TCsdStatus csdEnableScs(void)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eSecureBootEnable;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	if(readMspIO.mspDataBuf[3]==0x0) 
	{
		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eSecureBootEnable;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x0;
		progMspIO.dataBitMask[1] = 0x0;
		progMspIO.dataBitMask[2] = 0x0;
		progMspIO.dataBitMask[3] = 0x01;
		progMspIO.mspData[0]     = 0x0;
		progMspIO.mspData[1]     = 0x0;
		progMspIO.mspData[2]     = 0x0;
		progMspIO.mspData[3]     = 0x1;
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;
		
		readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eSecureBootEnable;

		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;

		if(readMspIO.mspDataBuf[3] != 0x1) 
			return CSD_ERROR;
	}
	
	return CSD_NO_ERROR;

}

TCsdStatus csdGetScsActivationFlag(
 	TCsdActivationFlag* pxScsActivationFlagState)
{
	NEXUS_ReadMspParms	readMspParams;
	NEXUS_ReadMspIO readMspIO;
	
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxScsActivationFlagState ==NULL)  
		return CSD_ERROR;

	*pxScsActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET; 

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eSecureBootEnable;
	
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	if(readMspIO.mspDataBuf[3]==0x1) 
		*pxScsActivationFlagState = CSD_ACTIVATION_FLAG_SET; 

	return CSD_NO_ERROR;
}

#if 0
TCsdStatus csdSetScsTotalAreaSize(
	TCsdScsTotalAreaSize xScsTotalAreaSize
)
{
	/* in Bseck mode, size is written in flash */
	BSTD_UNUSED(xScsTotalAreaSize); 

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	
	return CSD_NO_ERROR;
}

TCsdStatus csdGetScsTotalAreaSize(
	TCsdScsTotalAreaSize* pxScsTotalAreaSize
)
{
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxScsTotalAreaSize ==	NULL)  
		return CSD_ERROR;

	*pxScsTotalAreaSize = CSD_SCS_TOTAL_AREA_SIZE_NOT_SUPPORTED; 
	
	return CSD_NO_ERROR; 
}
#endif

TCsdStatus csdSetStbCaSn(
  	const TCsd4BytesVector xStbCaSn
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;
	unsigned char zeroArray[4] = {0, 0, 0, 0};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xStbCaSn == NULL )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;
	
	BDBG_MSG(("STB CA Serial Number ID  = 0x%x%x%x%x\n", 
		readMspIO.mspDataBuf[0],
		readMspIO.mspDataBuf[1],
		readMspIO.mspDataBuf[2],
		readMspIO.mspDataBuf[3]));

	if((memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0) &&
		(memcmp(xStbCaSn, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0))
		return CSD_ERROR_OPERATION_NOT_ALLOWED;

	if(memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) == 0)
	{

		progMspIO.progMode = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved104;
		progMspIO.dataBitLen = 32;
		progMspIO.dataBitMask[0] = 0xff;
		progMspIO.dataBitMask[1] = 0xff;
		progMspIO.dataBitMask[2] = 0xff;
		progMspIO.dataBitMask[3] = 0xff;
		
		progMspIO.mspData[0]     = xStbCaSn[0];
		progMspIO.mspData[1]     = xStbCaSn[1];
		progMspIO.mspData[2]     = xStbCaSn[2];
		progMspIO.mspData[3]     = xStbCaSn[3];
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;
		
		readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eReserved104;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		BDBG_MSG(("STB CA Serial Number  = 0x%x%x%x%x\n", 
			readMspIO.mspDataBuf[0],
			readMspIO.mspDataBuf[1],
			readMspIO.mspDataBuf[2],
			readMspIO.mspDataBuf[3]));

		if(memcmp(progMspIO.mspData, xStbCaSn, sizeof(unsigned char) * 4) != 0)
			return CSD_ERROR;
	}

	return CSD_NO_ERROR; 
}


TCsdStatus csdGetStbCaSn(
	TCsd4BytesVector xStbCaSn
)
{

	NEXUS_ReadMspParms		readMspParams;
	NEXUS_ReadMspIO readMspIO;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xStbCaSn == NULL )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	BDBG_MSG(("STB CA Serial Number  = 0x%x%x%x%x\n", 
		readMspIO.mspDataBuf[0],
		readMspIO.mspDataBuf[1],
		readMspIO.mspDataBuf[2],
		readMspIO.mspDataBuf[3]));

	BKNI_Memcpy(xStbCaSn, readMspIO.mspDataBuf, sizeof(unsigned char) * 4);

	return CSD_NO_ERROR; 
}

TCsdStatus csdSetMarketSegmentId(
	const TCsd4BytesVector xMarketSegmentId
)
{
	NEXUS_ReadMspParms		readMspParams;
	NEXUS_ProgramMspIO	progMspIO;
	NEXUS_ReadMspIO readMspIO;
	unsigned char zeroArray[4] = {0, 0, 0, 0};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xMarketSegmentId == NULL )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eMarketId;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;
	
	BDBG_MSG(("Market ID  = 0x%x%x%x%x\n", 
		readMspIO.mspDataBuf[0],
		readMspIO.mspDataBuf[1],
		readMspIO.mspDataBuf[2],
		readMspIO.mspDataBuf[3]));

	if((memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0) &&
		(memcmp(xMarketSegmentId, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) != 0))
		return CSD_ERROR_OPERATION_NOT_ALLOWED;

	if(memcmp(zeroArray, readMspIO.mspDataBuf, sizeof(unsigned char) * 4) == 0)
	{

		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
		progMspIO.dataBitLen     = 32;
		progMspIO.dataBitMask[0] = 0xff;
		progMspIO.dataBitMask[1] = 0xff;
		progMspIO.dataBitMask[2] = 0xff;
		progMspIO.dataBitMask[3] = 0xff;
		
		progMspIO.mspData[0]     = xMarketSegmentId[0];
		progMspIO.mspData[1]     = xMarketSegmentId[1];
		progMspIO.mspData[2]     = xMarketSegmentId[2];
		progMspIO.mspData[3]     = xMarketSegmentId[3];
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;
		
		readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		BDBG_MSG(("Market ID  = 0x%x%x%x%x\n", 
			readMspIO.mspDataBuf[0],
			readMspIO.mspDataBuf[1],
			readMspIO.mspDataBuf[2],
			readMspIO.mspDataBuf[3]));

		if(memcmp(progMspIO.mspData, xMarketSegmentId, sizeof(unsigned char) * 4) != 0)
			return CSD_ERROR;
	}

	return CSD_NO_ERROR; 
}


TCsdStatus csdGetMarketSegmentId(
	TCsd4BytesVector xMarketSegmentId
)
{
	NEXUS_ReadMspParms		readMspParams;
	NEXUS_ReadMspIO readMspIO;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xMarketSegmentId == NULL )
		return CSD_ERROR;

	readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eMarketId;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	BDBG_MSG(("Market ID  = 0x%x%x%x%x\n", 
		readMspIO.mspDataBuf[0],
		readMspIO.mspDataBuf[1],
		readMspIO.mspDataBuf[2],
		readMspIO.mspDataBuf[3]));

	BKNI_Memcpy(xMarketSegmentId, readMspIO.mspDataBuf, sizeof(unsigned char) * 4);

	return CSD_NO_ERROR; 
}

TCsdStatus csdGetDebugInterfaceProtectionLevel(
	TCsdDebugInterfaceAccessMode* pxDebugInterfaceProtectionLevel
)
{
	NEXUS_ReadMspParms	readMspParams;
	NEXUS_ReadMspIO readMspIO;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( pxDebugInterfaceProtectionLevel == NULL )
		return CSD_ERROR;	

	readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eCrDisable; 
	
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;
	
	BDBG_MSG(( "Cr bit 1  = %x\n", readMspIO.mspDataBuf[3]));
	if(readMspIO.mspDataBuf[3]==0x0)
	{
		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCrLockEnable; 
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
	
		BDBG_MSG(("Cr bit 0  = %x\n", readMspIO.mspDataBuf[3]));
		
		if(readMspIO.mspDataBuf[3]==0x1) 
			*pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED;
		else 
			*pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN;
	}
	else
		*pxDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED;	
	
	return CSD_NO_ERROR;

}


TCsdStatus csdSelectDebugInterfaceProtectionLevel(
	TCsdDebugInterfaceAccessMode xDebugInterfaceProtectionLevel)
{

	NEXUS_ProgramMspIO	progMspIO;
	TCsdDebugInterfaceAccessMode xState;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	 
	if(csdGetDebugInterfaceProtectionLevel(&xState) != CSD_NO_ERROR) 
	{
		BDBG_ERR(("Error getting JTAG state!\n"));
		return CSD_ERROR;
	}
	
	if(xState == xDebugInterfaceProtectionLevel) 
	{
		BDBG_WRN(("Already in this mode\n"));
		return CSD_NO_ERROR;
	}
	
	else if(xState > xDebugInterfaceProtectionLevel)
	{
		BDBG_ERR(("Error: operation not allowed\n"));
		return CSD_ERROR_OPERATION_NOT_ALLOWED;
	}
	
	progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
 	progMspIO.dataBitLen     = 1;
 	progMspIO.dataBitMask[0] = 0x0;
 	progMspIO.dataBitMask[1] = 0x0;
 	progMspIO.dataBitMask[2] = 0x0;
 	progMspIO.dataBitMask[3] = 0x01;
 	progMspIO.mspData[0]     = 0x0;
 	progMspIO.mspData[1]     = 0x0;
 	progMspIO.mspData[2]     = 0x0;
 	progMspIO.mspData[3]     = 0x1;
	
	switch(xDebugInterfaceProtectionLevel)
	{
		case CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED:
		
			progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCrLockEnable;
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			/* Need to reset board to get the correct state */
			
			break;
		case CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED:
			
			progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCrDisable;
	
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			
			/* need to reset board to get the correct state
			readMspIO.readMspEnum = BCMD_Otp_CmdMsp_eCrOtpBit1; 
			BHSM_ReadMSP (hCsdInitParameters->hHsmCsd, &readMspIO);  
			if(readMspIO.aucMspData[3]) result= CSD_NO_ERROR;
			else result = CSD_ERROR_OPERATION_FAILED;
			*/
			break;

		default:
			return CSD_ERROR;
			break;

	}

	return CSD_NO_ERROR;

}

TCsdStatus csdDeactivateKeyLadder(void)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the CA key ladder disabled? */
	if(readMspIO.mspDataBuf[3] == 0)
	{

		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x00;
		progMspIO.dataBitMask[1] = 0x00;
		progMspIO.dataBitMask[2] = 0x00;
		progMspIO.dataBitMask[3] = 0x01;
		progMspIO.mspData[0]     = 0x00;
		progMspIO.mspData[1]     = 0x00;
		progMspIO.mspData[2]     = 0x00;
		progMspIO.mspData[3]     = 0x01;
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		if(progMspIO.mspData[3] != 0x1)
			return CSD_ERROR;
	}

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the CP key ladder disabled? */
	if(readMspIO.mspDataBuf[3] == 0)
	{
		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x00;
		progMspIO.dataBitMask[1] = 0x00;
		progMspIO.dataBitMask[2] = 0x00;
		progMspIO.dataBitMask[3] = 0x01;
		progMspIO.mspData[0]     = 0x00;
		progMspIO.mspData[1]     = 0x00;
		progMspIO.mspData[2]     = 0x00;
		progMspIO.mspData[3]     = 0x01;
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
				
		if(progMspIO.mspData[3] != 0x1)
			return CSD_ERROR;		
	}
	
	return CSD_NO_ERROR; 
}


TCsdStatus csdGetKeyLadderDeactivationFlag(
 	TCsdActivationFlag* pxKeyLadderDeactivationFlagState
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxKeyLadderDeactivationFlagState == NULL)
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCaKeyLadderDisable;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;
	
	BDBG_MSG(("Disable CA Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

	if(readMspIO.mspDataBuf[3] == 0x1)
	{
		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		BDBG_MSG(("Disable CP Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

		if(readMspIO.mspDataBuf[3] == 0x1)
		{
			*pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_SET;
			 return CSD_NO_ERROR;
		}
		else
			 return CSD_ERROR;
	}
	else
	{
		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eCpKeyLadderDisable;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		BDBG_MSG(("Disable CP Key ladder  = %x\n", readMspIO.mspDataBuf[3]));

		if(readMspIO.mspDataBuf[3] == 0x1)
		{
			 return CSD_ERROR;
		}
		else
		{
			*pxKeyLadderDeactivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;
			 return CSD_NO_ERROR;
		}
	}

}

TCsdStatus csdGetDvrKey(
	TCsdUnsignedInt8 xDvrKey[16]
)
{
	TCsdR2RKeyPathHandle *r2rHandle;	
	TCsdUnsignedInt8 *xDvrKeyR2ROutput;
	TCsdUnsignedInt8 *KeySafeR2RInput;
	TCsdSize	contKeySize = 16;
	TCsdStatus  retCode = CSD_NO_ERROR;
	const TCsdUnsignedInt8	 KeySafe[16] = {
		0xbf, 0x18, 0x6a, 0x73, 0xec, 0x86, 0x3f, 0x25, 
		0x9b, 0xe8, 0x03, 0x52, 0x40, 0xd7, 0x37, 0x8c
	};
	
	const TCsdR2RCipheredProtectingKeys CONSTANT_1 = 
	{
		{
			0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 
			0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E
		
		},
		{
			0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 
			0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E
		}
	};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E
	};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xDvrKey == NULL )
		return CSD_ERROR;	

#ifdef CSD_DEBUG
	printf("KeySafe[16]\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", KeySafe[i]);
	printf("\n");	
#endif


	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&xDvrKeyR2ROutput);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&KeySafeR2RInput);
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&r2rHandle);

	r2rHandle->initHandle = csdInitParameters[0]; 
	r2rHandle->keyHandle = NULL;
	r2rHandle->bEncrypt = false; 

	memset(xDvrKeyR2ROutput, 0, sizeof(unsigned char) * 16 );
	memset(KeySafeR2RInput, 0, sizeof(unsigned char) * 16 );

	/* Copy the KeySafeto M2M input buffer. */
	BKNI_Memcpy((void *)KeySafeR2RInput, (void *)KeySafe, 16 );

	if( csdSetProtectedRam2RamContentKeys(
        CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR,
        r2rHandle,
		CONSTANT_1,   
		CONSTANT_2, 
		contKeySize) != CSD_NO_ERROR)
	{
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}

	if( csdDecryptData(
        CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR,
        r2rHandle,
        NULL,
        0,
  		KeySafeR2RInput,
        xDvrKeyR2ROutput,
        16) != CSD_NO_ERROR)
	{
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}

	/* Copy the M2M output buffer to xDvrKey. */
	BKNI_Memcpy((void *)xDvrKey, (void *)xDvrKeyR2ROutput, 16 );
	

#ifdef CSD_DEBUG
	printf("DvrKey[16]\n");	
	for( i=0; i<16; i++ )
		printf("0x%02x ", xDvrKey[i]);
	printf("\n");	
#endif

CSD_P_DONE:
	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(xDvrKeyR2ROutput);
	NEXUS_Memory_Free(KeySafeR2RInput);
	

	return (retCode);
}



#define CSD_DSC_PARSE_PARAMETERS(pxClearTextDscEvenHostKey,pxClearTextDscOddHostKey,xClearTextDscEvenHostKeySize,xClearTextDscOddHostKeySize) \
	if( CSD_INITIALIZED == FALSE )\
		return CSD_ERROR;\
	if(pxDscKeyPathHandle ==NULL) \
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;\
	if( (xEmi !=  CSD_Algorithm_DVB_CSA2) &&\
		(xEmi !=  CSD_Algorithm_DVB_CSA3) && \
		(xEmi !=  CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) && \
		(xEmi !=  CSD_Algorithm_AES_128_ECB_CLEAR) &&\
		(xEmi !=  CSD_Algorithm_AES_128_CBC_CLEAR) &&\
		(xEmi !=  CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&\
		(xEmi !=  CSD_Algorithm_TDES_ABA_ECB_CLEAR))\
		return CSD_ERROR_OPERATION_NOT_SUPPORTED;\
	if(xEmi == CSD_Algorithm_DVB_CSA2)\
	{\
		if( pxClearTextDscOddHostKey != NULL && xClearTextDscOddHostKeySize == 8 )\
		{\
			ODDCLEARKEYVALID = TRUE;\
		}\
		else\
		{\
			if( !(pxClearTextDscOddHostKey == NULL && xClearTextDscOddHostKeySize == 0) )		\		
				return CSD_ERROR_INVALID_PARAMETERS;\
		}\
		if( pxClearTextDscEvenHostKey != NULL && xClearTextDscEvenHostKeySize == 8 )\
		{\
			EVENCLEARKEYVALID = TRUE;\
		}\
		else\
		{\
			if( !(pxClearTextDscEvenHostKey == NULL && xClearTextDscEvenHostKeySize == 0))\
			{\
				return CSD_ERROR_INVALID_PARAMETERS;\
			}\
			else\
			{		\	
				if( ODDCLEARKEYVALID == FALSE )		\			
					return CSD_ERROR_INVALID_PARAMETERS;\
			}\
		}\
	}\
	else\
	{\
		if( pxClearTextDscOddHostKey != NULL && xClearTextDscOddHostKeySize == 16 )\
		{\
			ODDCLEARKEYVALID = TRUE;\
		}\
		else\
		{\
			if( !(pxClearTextDscOddHostKey == NULL && xClearTextDscOddHostKeySize == 0) )	\			
				return CSD_ERROR_INVALID_PARAMETERS;\
		}\
		if( pxClearTextDscEvenHostKey != NULL && xClearTextDscEvenHostKeySize == 16 )\
		{\
			EVENCLEARKEYVALID = TRUE;\
		}\
		else\
		{\
			if( !(pxClearTextDscEvenHostKey == NULL && xClearTextDscEvenHostKeySize == 0))\
			{\
				return CSD_ERROR_INVALID_PARAMETERS;\
			}\
			else\
			{\
				if( ODDCLEARKEYVALID == FALSE )		\			
					return CSD_ERROR_INVALID_PARAMETERS;\
			}\
		}\
	}\
	if( (xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) &&\
		(pxDscKeyPathHandle->initializationVector == NULL) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if(pxDscKeyPathHandle->initHandle->pidchannel>256)\
		return CSD_ERROR;\

#define  CSD_DES_CONFIG_ALGORITHM(xClearTextDscOddHostKeySize,xClearTextDscEvenHostKeySize)\
	keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxDscKeyPathHandle->initHandle->pidchannel);\
	if (!keyHandle)\
	{	\
	 	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);\
    	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa; \
        keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);\
		if( keyHandle == NULL) \
		{ \
			return  CSD_ERROR;  \
		}\
		else\
		{\
			csdInsertHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);\
		}\
		errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, \
			pxDscKeyPathHandle->initHandle->pidchannel);\
		if (errCode != 0) \
		{\
			BDBG_WRN((" NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode ));	\
			goto CSD_P_DONE;\
		}\
	}\
	if ( xClearTextDscOddHostKeySize > 0 ||xClearTextDscEvenHostKeySize > 0 )		\
	{\
	    NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);\
		NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa; \
		switch(xEmi)\
		{\
			case CSD_Algorithm_DVB_CSA2:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;\
				break;\
			case CSD_Algorithm_DVB_CSA3:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvbCsa3;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;\
				break;\
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				break;\
			case CSD_Algorithm_AES_128_ECB_CLEAR:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				break;\
			case CSD_Algorithm_AES_128_CBC_CLEAR:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				break;\
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				break;\
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:\
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;\
				break;\
			default:\
				break;\
		}\
	}\


#define CSD_DSC_LOAD_IV_IF_REQUIRED \
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||\
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) )\
	{\
		NEXUS_Security_GetDefaultClearKey(&clearKey);\
		clearKey.keyIVType= NEXUS_SecurityKeyIVType_eIV; \
		clearKey.keyEntryType = NEXUS_SecurityKeyType_eIv;\
    	clearKey.keySize = 16;\
		clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;\
		memset (clearKey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);\
		if(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)\
  			BKNI_Memcpy(&(clearKey.keyData[8]), xZeroVector, sizeof(TCsdUnsignedInt8)*8);\
		else if(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR)\
  			BKNI_Memcpy(clearKey.keyData, pxDscKeyPathHandle->initializationVector, sizeof(TCsdUnsignedInt8)*16);\
		else\
  			BKNI_Memcpy(clearKey.keyData, xZeroVector, sizeof(TCsdUnsignedInt8)*16);\
    	if (NEXUS_Security_LoadClearKey(keyHandle, &clearKey) != 0) \
		{\
        		BDBG_WRN(("\nLoad IV failed \n"));\
        		goto CSD_P_DONE;\
    	}\
	}\


/* Clear CA */
TCsdStatus csdSetClearTextDscHostKeys
(
	TCsdUnsignedInt16            xEmi,
	const TCsdUnsignedInt8* pxClearTextDscOddHostKey,
	TCsdSize xClearTextDscOddHostKeySize,
	const TCsdUnsignedInt8* pxClearTextDscEvenHostKey,
	TCsdSize xClearTextDscEvenHostKeySize,
	TCsdDscKeyPathHandle* pxDscKeyPathHandle
)
{
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings	NexusConfig;
	NEXUS_SecurityClearKey clearKey; 
	NEXUS_SecurityKeySlotSettings keySlotSettings;	
	BERR_Code errCode=CSD_NO_ERROR;
	bool	ODDCLEARKEYVALID = FALSE;
	bool EVENCLEARKEYVALID = FALSE;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	

	/* Parse DSC paramters */
	CSD_DSC_PARSE_PARAMETERS(pxClearTextDscEvenHostKey,pxClearTextDscOddHostKey,xClearTextDscEvenHostKeySize,xClearTextDscOddHostKeySize);

	/* Config Algorithm */
	CSD_DES_CONFIG_ALGORITHM(xClearTextDscOddHostKeySize,xClearTextDscEvenHostKeySize);

	NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd; 
   	errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
	if (errCode != 0) 
	{
		BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode ));	
		goto CSD_P_DONE;
	}


	if ( xClearTextDscOddHostKeySize > 0)		
	{
		NEXUS_Security_GetDefaultClearKey(&clearKey);
		clearKey.keyIVType= NEXUS_SecurityKeyIVType_eNoIV; 
		clearKey.keySize = xClearTextDscOddHostKeySize; 
		clearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
		clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;
		BKNI_Memcpy(clearKey.keyData,  pxClearTextDscOddHostKey,clearKey.keySize) ; 
		
		errCode = NEXUS_Security_LoadClearKey(keyHandle, &clearKey);
		
		if (errCode != 0) 
		{
			BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}				
	}

	if ( xClearTextDscEvenHostKeySize > 0)		
	{
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven; 
	
   		errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
	
		if (errCode != 0) 
		{
			BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(even) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}				

		NEXUS_Security_GetDefaultClearKey(&clearKey);
		clearKey.keyIVType= NEXUS_SecurityKeyIVType_eNoIV; 
		clearKey.keySize = xClearTextDscEvenHostKeySize; 
		clearKey.keyEntryType = NEXUS_SecurityKeyType_eEven; 
		clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;
		
		BKNI_Memcpy(clearKey.keyData,  pxClearTextDscEvenHostKey,clearKey.keySize) ; 
		
		errCode = NEXUS_Security_LoadClearKey( keyHandle, &clearKey);
		
		if (errCode != 0) 
		{
			BDBG_MSG(("NEXUS_Security_LoadClearKey (even) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}		
	}

	/* Check if IV will be loaded. */
	CSD_DSC_LOAD_IV_IF_REQUIRED;

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}

/* CA key ladder */

#define CSD_DSC_GENERATE_ROUNTE_KEY(polarity, pKey, keySizeIn)\
	if ( keySizeIn > 0)		\
	{\
		SessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA; \
		SessionKey.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
		SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
	    SessionKey.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
		SessionKey.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
		SessionKey.keyEntryType = polarity; 		\
		SessionKey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
		SessionKey.operation = NEXUS_SecurityOperation_eDecrypt; \
		SessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;\
		SessionKey.bASKMMode = true;\
		SessionKey.keyMode = NEXUS_SecurityKeyMode_eRegular;\
    	SessionKey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;\
    	SessionKey.bSwapAESKey = false;\
		SessionKey.cusKeyL = 0;\
		SessionKey.cusKeyH = 0;\
		SessionKey.cusKeyVarL = 0;\
		SessionKey.cusKeyVarH = 0;\
		SessionKey.bRouteKey = false;\
		SessionKey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
		SessionKey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
		SessionKey.hwklDestAlg = NexusConfig.algorithm;\
		SessionKey.rootKeySwap	= false;\
		BKNI_Memcpy(&(proc_in3[0][0]), &(xDscCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);\		
		BKNI_Memcpy(&(proc_in3[0][8]), &(xDscCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);\
		BKNI_Memcpy(SessionKey.keyData,&(proc_in3[0][0]), 16);\
		errCode = NEXUS_Security_GenerateSessionKey( keyHandle, &SessionKey);\
		if (errCode != 0) \
		{\
			BDBG_MSG(("NEXUS_Security_GenerateSessionKey errCode: %x\n", errCode ));	\
			goto CSD_P_DONE;\
		}	\
		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)\
		{\
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			CW.keyEntryType = polarity; \
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			CW.bASKMMode = true;\
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			CW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		CW.bSwapAESKey = false;\
			CW.bRouteKey = false;\
			CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			CW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			CW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(&(proc_in3[1][0]), &(xDscCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);	\	
			BKNI_Memcpy(&(proc_in3[1][8]), &(xDscCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);\
			BKNI_Memcpy(CW.keyData,&(proc_in3[1][0]), 16);\
			CW.keySize = 16;\
	       	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);\
			if (errCode != 0) \
			{\
				BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode ));	\
        		goto CSD_P_DONE;\
			}	\		
		}\
		else\
		{\
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			CW.keyEntryType = polarity; \
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			CW.bASKMMode = true;\
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			CW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		CW.bSwapAESKey = false;\
			CW.bRouteKey = true;\
			CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			CW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			CW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(CW.keyData,pKey, keySizeIn);\
			CW.keySize = keySizeIn;\
	       	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);\
			if (errCode != 0) \
			{\
				BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode ));	\
        		goto CSD_P_DONE;\
			}			\
		}\
		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)\
		{\
			encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA; \
			encrytedCW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; \
			encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; \
		    encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; \
			encrytedCW.keyEntryType = polarity; \
			encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;\
			encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone; \
			encrytedCW.bASKMMode = true;\
			encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;\
			encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt; \
    		encrytedCW.bSwapAESKey = false;\
			encrytedCW.bRouteKey = true;\
			encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;\
			encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;\
			encrytedCW.hwklDestAlg = NexusConfig.algorithm;\
			BKNI_Memcpy(encrytedCW.keyData,pKey, keySizeIn);\
			encrytedCW.keySize = keySizeIn;\
	       	errCode = NEXUS_Security_GenerateKey5(keyHandle , &encrytedCW);\
			if (errCode != 0) \
			{\
				BDBG_WRN(("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode ));\	
        		goto CSD_P_DONE;\
			}\			
		}\
	}\

TCsdStatus csdSetProtectedDscContentKeys
(
	TCsdUnsignedInt16            xEmi,
  	const TCsdDscCipheredProtectingKeys xDscCipheredProtectingKeys,
  	const TCsdUnsignedInt8* pxCipheredDscOddContentKey,
       TCsdSize xCipheredDscOddContentKeySize,
  	const TCsdUnsignedInt8* pxCipheredDscEvenContentKey,
       TCsdSize xCipheredDscEvenContentKeySize,
       TCsdDscKeyPathHandle* pxDscKeyPathHandle
)
{
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings	NexusConfig;
	NEXUS_SecurityEncryptedSessionKey	SessionKey; 
	NEXUS_SecurityKeySlotSettings	keySlotSettings;
	NEXUS_SecurityEncryptedControlWord	CW; 
	NEXUS_SecurityEncryptedControlWord encrytedCW;
	TCsdDscCipheredProtectingKeys proc_in3;
    	NEXUS_SecurityClearKey clearKey;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BERR_Code errCode=CSD_NO_ERROR;
	bool	ODDCLEARKEYVALID = FALSE;
	bool EVENCLEARKEYVALID = FALSE;
	
	/* Parse DSC paramters */
	CSD_DSC_PARSE_PARAMETERS(pxCipheredDscEvenContentKey,pxCipheredDscOddContentKey,xCipheredDscEvenContentKeySize,xCipheredDscOddContentKeySize);

	/* Config Algorithm */
	CSD_DES_CONFIG_ALGORITHM(xCipheredDscOddContentKeySize,xCipheredDscEvenContentKeySize);

	NEXUS_Security_GetDefaultSessionKeySettings (&SessionKey);
	NEXUS_Security_GetDefaultControlWordSettings (&CW);
	NEXUS_Security_GetDefaultControlWordSettings (&encrytedCW);

 	if ( xCipheredDscOddContentKeySize > 0 ||xCipheredDscEvenContentKeySize > 0 )		
	{
   		NexusConfig.caVendorID=0x97ed;
		NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
		NexusConfig.testKey2Select = 0x0;

		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
		{
			if(xEmi == CSD_Algorithm_DVB_CSA2)
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
				encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
				/* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_5;*/
	    		SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			    CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			    encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			}
			else
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				/* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_9;*/
	    		SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
			    CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
			    encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
			}
			
		}
		else
		{
			if(xEmi == CSD_Algorithm_DVB_CSA2)
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
				/*NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_3;*/
	    		SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
		   		CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			}
			else
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
				/* NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;*/
	    		SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
		    	CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
			}
		}
	
		NexusConfig.key2Select =  NEXUS_SecurityKey2Select_eFixedKey;
		NexusConfig.askmModuleID = 0xE0;

   		errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
	
		if (errCode != 0) 
		{
			BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}
		
	}
	
	/* Route ODD key */
	CSD_DSC_GENERATE_ROUNTE_KEY(NEXUS_SecurityKeyType_eOdd, pxCipheredDscOddContentKey, xCipheredDscOddContentKeySize);

	/* Route EVEN key */
	CSD_DSC_GENERATE_ROUNTE_KEY(NEXUS_SecurityKeyType_eEven, pxCipheredDscEvenContentKey, xCipheredDscEvenContentKeySize);


	/* Check if IV will be loaded. */
	CSD_DSC_LOAD_IV_IF_REQUIRED;

	return CSD_NO_ERROR;
CSD_P_DONE: 
   csdFreeHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}

#define CSD_SCR_VALIDATE_PARAMS \
	if((xEmi != CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) &&\
		(xEmi != CSD_Algorithm_AES_128_ECB_CLEAR) &&\
		(xEmi != CSD_Algorithm_AES_128_CBC_CLEAR) &&\
		(xEmi != CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&\
		(xEmi != CSD_Algorithm_TDES_ABA_ECB_CLEAR) &&\
		(xEmi != CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) &&\
		(xEmi != CSD_Algorithm_RAW_AES_128_ECB_CLEAR) &&\
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) &&\
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR) &&\
		(xEmi != CSD_Algorithm_RAW_0x4022) &&\
		(xEmi != CSD_Algorithm_RAW_0x4023) &&\
		(xEmi != CSD_Algorithm_RAW_0x4024) )\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if(((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR)) &&\
		(xInitializationVectorSize != 8))\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if(((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||\
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))&&\
		(xInitializationVectorSize != 16))\
		return CSD_ERROR_INVALID_PARAMETERS;\
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))\
	{\
		if(memcmp( xZeroVector, pxInitializationVector, 16 ))\
			return CSD_ERROR_INVALID_PARAMETERS;\
	}\
	if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))\
	{\
		if(memcmp( xZeroVector, pxInitializationVector, 8 ))\
			return CSD_ERROR_INVALID_PARAMETERS;\
	}\
	if(pxScrKeyPathHandle == NULL)\
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;\
	pxScrKeyPathHandle->dataFormat = NEXUS_DmaDataFormat_eMpeg;\

#define CSD_SCR_PARSE_EMI_ALGORITHMS \
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);\
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; \
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);\
	if( keyHandle == NULL) \
	{ \
		return  CSD_ERROR; \
	}\
	else\
	{\
		csdInsertHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);\
	}\
    NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);\
	algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;\
	algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;\
	pxScrKeyPathHandle->ivSize =0;\
	switch(xEmi)\
	{\
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:\
		case CSD_Algorithm_RAW_0x4022:\
		case CSD_Algorithm_RAW_0x4023:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_RAW_0x4024:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCounter;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;\
			algorithm.aesCounterSize = NEXUS_SecurityAesCounterSize_e128Bits;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_AES_128_ECB_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			break;\
		case CSD_Algorithm_AES_128_CBC_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_TDES_ABA_ECB_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			break;\
		case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			pxScrKeyPathHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			pxScrKeyPathHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;\
			break;\
		case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			pxScrKeyPathHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;\
			pxScrKeyPathHandle->ivSize = xInitializationVectorSize;\
			break;\
		case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:\
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;\
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;\
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;\
			pxScrKeyPathHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;\
			break;\
		default:\
			goto CSD_P_DONE;\
	}\
	algorithm.operation = (bEnc)?NEXUS_SecurityOperation_eEncrypt:NEXUS_SecurityOperation_eDecrypt;\
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;\
CSD_Algorithm_RAW_0x4022
#define CSD_SCR_LOAD_IV_IF_REQUIRED \
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||\
		(xEmi == CSD_Algorithm_RAW_0x4022) ||\
		(xEmi == CSD_Algorithm_RAW_0x4023) ||\
		(xEmi == CSD_Algorithm_RAW_0x4024) ||\
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||\
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||\
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) )\
	{\
		NEXUS_Security_GetDefaultClearKey(&key);\
		key.keyIVType = NEXUS_SecurityKeyIVType_eIV;\
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;\
    	key.keySize = 16;\
		memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);\
		if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||\
			(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))\
  			BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);\
		else\
  			BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);\
  		BKNI_Memcpy(pxScrKeyPathHandle->ivArray, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);\
   		if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) \
		{\
        		BDBG_WRN(("\nLoad IV failed \n"));\
        		goto CSD_P_DONE;\
    		}\
    	}\


static TCsdStatus csdSetClearTextScrEncryptionDecryptionKey(
	TCsdUnsignedInt16            xEmi,
	const TCsdUnsignedInt8* pxClearTextScrHostKey,
       TCsdSize xClearTextScrHostKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdScrKeyPathHandle* pxScrKeyPathHandle,
	   bool bEnc
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	/* Checks to make sure all input is valid. */
	/* Make sure clear key is 16 bytes. */
	if(xClearTextScrHostKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	CSD_SCR_VALIDATE_PARAMS;
	
	CSD_SCR_PARSE_EMI_ALGORITHMS;

	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		goto CSD_P_DONE;
    	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
    	key.keySize = xClearTextScrHostKeySize;
	BKNI_Memcpy(key.keyData,  pxClearTextScrHostKey,sizeof(TCsdUnsignedInt8)*key.keySize) ; 

	/* Load clear key to key table. */
    	if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
	{
       	BDBG_WRN(("\nLoad encryption key failed \n"));
		goto CSD_P_DONE;
    	}

	/* Check if IV will be loaded. */
	CSD_SCR_LOAD_IV_IF_REQUIRED;

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}

TCsdStatus csdSetClearTextScrEncryptionKey(
	TCsdUnsignedInt16            xEmi,
	const TCsdUnsignedInt8* pxClearTextScrHostKey,
       TCsdSize xClearTextScrHostKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
	return csdSetClearTextScrEncryptionDecryptionKey(
											xEmi,
											pxClearTextScrHostKey,
											xClearTextScrHostKeySize,
											pxInitializationVector,
											xInitializationVectorSize,
											pxScrKeyPathHandle,
											true
											);
}

TCsdStatus csdSetClearTextScrDecryptionKey(
	TCsdUnsignedInt16            xEmi,
  	const TCsdUnsignedInt8* pxClearTextScrHostKey,
       TCsdSize xClearTextScrHostKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
	return csdSetClearTextScrEncryptionDecryptionKey(
											xEmi,
											pxClearTextScrHostKey,
											xClearTextScrHostKeySize,
											pxInitializationVector,
											xInitializationVectorSize,
											pxScrKeyPathHandle,
											false
											);

}


static TCsdStatus csdSetProtectedScrEncryptionDecryptionKey(
	TCsdUnsignedInt16 xEmi,
  	const TCsdScrCipheredProtectingKeys xScrCipheredProtectingKeys,
  	const TCsdUnsignedInt8* pxCipheredScrContentKey,
       TCsdSize xCipheredScrContentKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdScrKeyPathHandle* pxScrKeyPathHandle,
	   bool	bEnc
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings algorithm;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	TCsdR2RCipheredProtectingKeys proc_in3;
    NEXUS_SecurityClearKey key;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BERR_Code errCode=CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	/* Checks to make sure all input is valid. */
	/* Make sure content key is 16 bytes. */
	if(xCipheredScrContentKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;
	
	CSD_SCR_VALIDATE_PARAMS;
	
	CSD_SCR_PARSE_EMI_ALGORITHMS;
	
   	algorithm.caVendorID=0x97ed;

	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
		algorithm.askmModuleID = 0xE0;
	else
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;
	
	algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
	algorithm.testKey2Select = 0x0;
	algorithm.key2Select =  NEXUS_SecurityKey2Select_eFixedKey;
	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		goto CSD_P_DONE;
    }

	/* Load session key3 */
	NEXUS_Security_GetDefaultSessionKeySettings (&encryptedSessionkey);
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	encryptedSessionkey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
	encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eNone;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	encryptedSessionkey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.bASKMMode = true;
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
	encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 	= 0x00; 
	encryptedSessionkey.cusKeyVarH 	= 0xFF; 
	encryptedSessionkey.bSwapAESKey = false;
	encryptedSessionkey.bRouteKey 		= false;

	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
	else
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	
	encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

	/* Perform key swap for proc-in. */
	BKNI_Memcpy(&(proc_in3[0][0]), &(xScrCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);
	BKNI_Memcpy(&(proc_in3[0][8]), &(xScrCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);

	BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);

	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
	{
#ifdef CSD_DEBUG
		printf("\nLoading session key failed \n");
#endif
		goto CSD_P_DONE;
	}
	
	/* Load key4 */
	NEXUS_Security_GetDefaultControlWordSettings (&key4);
	key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
	key4.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	key4.hwklDestAlg = algorithm.algorithm;
    key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	/*key4.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;*/
    key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;	
	key4.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	key4.swizzleType = NEXUS_SecuritySwizzleType_eNone;
	key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	key4.hwklLen = NEXUS_SecurityHWKLLenSelect_eLen3;
	key4.bASKMMode = true;
	key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
    key4.operation = NEXUS_SecurityOperation_eDecrypt;     
	key4.keyMode = NEXUS_SecurityKeyMode_eRegular;
	key4.bSwapAESKey = false;
	
	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
	{
	    key4.bRouteKey = false;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
    	key4.keySize = 16; 

		/* Perform key swap for proc-in. */
		BKNI_Memcpy(&(proc_in3[1][0]), &(xScrCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(&(proc_in3[1][8]), &(xScrCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);
			
		BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);
	}
	else
	{
	    key4.bRouteKey = true;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
    	key4.keySize = xCipheredScrContentKeySize; 
		BKNI_Memcpy(key4.keyData,  pxCipheredScrContentKey, sizeof(TCsdUnsignedInt8)*xCipheredScrContentKeySize);
	}
	
	
	if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
	{
#ifdef CSD_DEBUG
		printf("\nLoading key4 failed for video ODD key\n");
#endif
		goto CSD_P_DONE;
	}
	
	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
	{	
		/* Load key5 */
		NEXUS_Security_GetDefaultControlWordSettings (&encrytedCW);
		encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
		encrytedCW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	    encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		/*encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;*/
	    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
		encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
		encrytedCW.hwklLen = NEXUS_SecurityHWKLLenSelect_eLen3;
		encrytedCW.hwklDestAlg = algorithm.algorithm;
		encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
		encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
		encrytedCW.bASKMMode = true;
		encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
		encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
		encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
	    encrytedCW.keySize = xCipheredScrContentKeySize; 
		BKNI_Memcpy(encrytedCW.keyData, pxCipheredScrContentKey, sizeof(TCsdUnsignedInt8)*xCipheredScrContentKeySize);
	    encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
		encrytedCW.bSwapAESKey = false;
	    encrytedCW.bRouteKey = true;
			

		if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
		{
#ifdef CSD_DEBUG
			printf("\nLoading CW key failed for video ODD key\n");
#endif
			goto CSD_P_DONE;
		}
	}
	
	/* Check if IV will be loaded. */
	CSD_SCR_LOAD_IV_IF_REQUIRED;

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}


TCsdStatus csdSetProtectedScrDecryptionKey(
	TCsdUnsignedInt16 xEmi,
  	const TCsdScrCipheredProtectingKeys xScrCipheredProtectingKeys,
  	const TCsdUnsignedInt8* pxCipheredScrContentKey,
       TCsdSize xCipheredScrContentKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdScrKeyPathHandle* pxScrKeyPathHandle
)
{
	return csdSetProtectedScrEncryptionDecryptionKey(
														xEmi,
														xScrCipheredProtectingKeys,
														pxCipheredScrContentKey,
														xCipheredScrContentKeySize,
														pxInitializationVector,
														xInitializationVectorSize,
														pxScrKeyPathHandle,
														false);

}

static unsigned long csd_P_ReadRefOKStatus(unsigned long bit_address){
	unsigned int status_reg= 0;

	
	NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1,&status_reg);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, status_reg|1); 

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000f); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
		
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000004); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
	
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000008); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
	
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000D); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
	
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, bit_address); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x20A00001); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
	
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}	
	NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);

	return status_reg; 
}

static unsigned int csd_P_ReadOTPData(void){
	unsigned int status_reg= 0;
	
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00A00001); 
	status_reg= 0;
	while(!(status_reg&0x1)){
		BKNI_Delay(10);
	
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1,&status_reg);
	}	
	NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_0,&status_reg);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0); 
	return status_reg; 
}

TCsdStatus csdGetChipExtension
(
  TCsd20CharsString xChipExtension
)
{
	unsigned status_reg;
	const unsigned int Addr_row = 413;
	const unsigned int Data_row = 0x01A00001; 
	const unsigned int expect_data = 0x0000188B;

	if(xChipExtension == NULL) return CSD_ERROR; 
	status_reg = csd_P_ReadRefOKStatus(Addr_row);
	if(status_reg != expect_data){
		BDBG_ERR(("OTP programming failed!  %x\n", status_reg));
		return CSD_ERROR;
	}
	status_reg = csd_P_ReadOTPData();

#if 0
	xChipExtension[0] = (status_reg>>19)&0xff;
	xChipExtension[1] = (status_reg>>13)&0x3f;
#else
	/* for test purpose before have a real part */
	xChipExtension[0] = 'N';
	xChipExtension[1] = 'P';
#endif
	return CSD_NO_ERROR;
}

TCsdStatus csdSetScsPv
(
  TCsdScsPvId               xScsPvParamsId,
  TCsdScsPvPathdHandle*    pxPvPathHandle, 
  TCsdUnsignedInt16         xPvLength,
  TCsdUnsignedInt8*        pxPvValue
)
{
	return CSD_NO_ERROR;
}


TCsdStatus csdGetScsPv
(
  TCsdScsPvId               xScsPvParamsId,
  TCsdScsPvPathdHandle*    pxPvPathHandle,
  TCsdUnsignedInt16         xPvLength,
  TCsdUnsignedInt8*        pxPvValue
)
{
	return CSD_NO_ERROR;
}


TCsdStatus csdSetRamUnscrambledStorageSize
(
  TCsdUnsignedInt8*        pxPvRamUnscrambledStorageSize,
  TCsdUnsignedInt16         xPvLength
)
{
	return CSD_NO_ERROR;
}

TCsdStatus csdGetRamUnscrambledStorageSize
(
  TCsdUnsignedInt8*        pxPvRamUnscrambledStorageSize,
  TCsdUnsignedInt16         xPvLength
)
{
	return CSD_NO_ERROR;
}


TCsdStatus csdEnableRamScrambling
(
  void
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the Dramscrambler enabled? */
	if(readMspIO.mspDataBuf[3] == 0)
	{

		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x00;
		progMspIO.dataBitMask[1] = 0x00;
		progMspIO.dataBitMask[2] = 0x00;
		progMspIO.dataBitMask[3] = 0x01;
		progMspIO.mspData[0]     = 0x00;
		progMspIO.mspData[1]     = 0x00;
		progMspIO.mspData[2]     = 0x00;
		progMspIO.mspData[3]     = 0x01;
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		if(progMspIO.mspData[3] != 0x1)
			return CSD_ERROR;
	}


	return CSD_NO_ERROR;
}

TCsdStatus csdGetScramblingActivationFlag
(
  TCsdActivationFlag* pxScramblingActivationFlagState
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the Dramscrambler enabled? */
	if(readMspIO.mspDataBuf[3] == 1)
	{

		*pxScramblingActivationFlagState = CSD_ACTIVATION_FLAG_SET;
	}else *pxScramblingActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;

	
	return CSD_NO_ERROR;
}

TCsdStatus csdEnableFlashProtection
(
  void
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the Dramscrambler enabled? */
	if(readMspIO.mspDataBuf[3] == 0)
	{

		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x00;
		progMspIO.dataBitMask[1] = 0x00;
		progMspIO.dataBitMask[2] = 0x00;
		progMspIO.dataBitMask[3] = 0x01;
		progMspIO.mspData[0]     = 0x00;
		progMspIO.mspData[1]     = 0x00;
		progMspIO.mspData[2]     = 0x00;
		progMspIO.mspData[3]     = 0x01;
		
		/* Issue command to program the OTP. */
		if(NEXUS_Security_ProgramMSP (&progMspIO))
			return CSD_ERROR;

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation;
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		if(progMspIO.mspData[3] != 0x1)
			return CSD_ERROR;
	}


	
	return CSD_NO_ERROR;
}


TCsdStatus csdGetFlashProtectionActivationFlag
(
  TCsdActivationFlag* pxFlashProtectionActivationFlagState
)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;

	if(CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eFlashDecryptionActivation;
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	/* Is the Dramscrambler enabled? */
	if(readMspIO.mspDataBuf[3] == 1)
	{

		*pxFlashProtectionActivationFlagState = CSD_ACTIVATION_FLAG_SET;
	}else *pxFlashProtectionActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;

	
	return CSD_NO_ERROR;
}

TCsdStatus csdEncryptData
(
        TCsdUnsignedInt16       xEmi,
        TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*      pxInitializationVector,
        TCsdSize                xInitializationVectorSize,
  const TCsdUnsignedInt8*      pxInputData,
        TCsdUnsignedInt8*      pxOutputData,
        TCsdSize                xDataSize
)
{
	 NEXUS_SecurityClearKey key;
	 
	 if( CSD_INITIALIZED == FALSE )
		 return CSD_ERROR;

	if(pxR2RKeyPathHandle->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return CSD_ERROR; 
	}

/* load IV key if needed */
	if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR)
	)
	{
		BKNI_Memset((void *)&key, 0, sizeof(key));
	NEXUS_Security_GetDefaultClearKey(&key);
		key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		key.keySize = 16; /*xInitializationVectorSize;*/
		
	if(xInitializationVectorSize == 8){
	
  		BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, xInitializationVectorSize);
		}
	else
		BKNI_Memcpy(key.keyData, pxInitializationVector, xInitializationVectorSize);
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(pxR2RKeyPathHandle->keyHandle , &key) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		return CSD_ERROR;
    		}
    	}
	if(csdR2RCryptoOperation(pxR2RKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
	return CSD_ERROR;
	return CSD_NO_ERROR;
}

TCsdStatus csdDecryptData
(
        TCsdUnsignedInt16       xEmi,
        TCsdR2RKeyPathHandle*  pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*      pxInitializationVector,
        TCsdSize                xInitializationVectorSize,
  const TCsdUnsignedInt8*      pxInputData,
        TCsdUnsignedInt8*      pxOutputData,
        TCsdSize                xDataSize
)
{
		NEXUS_SecurityClearKey key;
		
		if( CSD_INITIALIZED == FALSE )
			return CSD_ERROR;

	
		if(pxR2RKeyPathHandle->keyHandle == NULL){
			BDBG_ERR(("R2R key is not loaded!\n")); 
			return CSD_ERROR; 
		}

	/* load IV key if needed */
		if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
			(xEmi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
			(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
			(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR)
		)
		{
		BKNI_Memset((void *)&key, 0, sizeof(key));
	NEXUS_Security_GetDefaultClearKey(&key);
			key.keyIVType	= NEXUS_SecurityKeyIVType_eIV;
			key.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		key.keySize = 16; /*xInitializationVectorSize;*/
			
	if(xInitializationVectorSize == 8){
			
  		BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, xInitializationVectorSize);
		}
			else
		BKNI_Memcpy(key.keyData, pxInitializationVector, xInitializationVectorSize);
			/* Load IV. */
				if (NEXUS_Security_LoadClearKey(pxR2RKeyPathHandle->keyHandle , &key) != 0) 
			{
					BDBG_WRN(("\nLoad IV failed \n"));
	return CSD_ERROR;
}
			}
		if(csdR2RCryptoOperation(pxR2RKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
			return CSD_ERROR;
		
	return CSD_NO_ERROR;
}

TCsdStatus csdSetFlashProtRootKey
(
        TCsdFPKeyPathHandle*         pxFPKeyPathHandle
)
{

		NEXUS_SecurityKeySlotSettings keySettings;
		NEXUS_KeySlotHandle keyHandle=NULL;
		NEXUS_SecurityAlgorithmSettings algorithm;
		NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
		NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
		TCsdR2RCipheredProtectingKeys proc_in3;
		TCsdStatus				   retCode = CSD_NO_ERROR;
		const TCsdUnsignedInt8 CONSTANTs[2][16] = 
		{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},
		{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e},};
	
		const TCsdUnsignedInt8 CONSTANT_2[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};
		if( CSD_INITIALIZED == FALSE )
			return CSD_ERROR;
	
	
		if(pxFPKeyPathHandle == NULL)
			return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
		/* Checks to make sure all input is valid. */
		/* Make sure clear key is 16 bytes. */
	/*	if(pxFPKexCipheredContentKeySize != 16)
			return CSD_ERROR_INVALID_PARAMETERS;
		/* Make sure algorithm is AES-128 or 3DES-ABA. */


		NEXUS_Security_GetDefaultKeySlotSettings((const) &keySettings);
	
		keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
		keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		pxFPKeyPathHandle->keyHandle = keyHandle;
	

		if( keyHandle == NULL) 
		{ 
			return	CSD_ERROR;	/* return if no keyslot is available*/
		}
		else
		{
			csdInsertHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
			
		}
		/* Set up encryption key */
		NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);
	

		algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;


		algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
		if(pxFPKeyPathHandle->bEncrypt)
		algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
		else 
			algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	
		algorithm.caVendorID=0x97ed;
	
	if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_10;
	else
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
		algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
		algorithm.testKey2Select = 0x0;
		algorithm.key2Select =	NEXUS_SecurityKey2Select_eFixedKey;
		/* Load algorithm to key table.  Set it up to do encryption. */
		if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
		{
			BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
			retCode =  CSD_ERROR;
			goto CSD_P_DONE;
		}
		
		
		/*BKNI_Memcpy((void *)&pxFPKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 
*/
		BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));
	
		/* Load session key3 */
		encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
		
		encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
		encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
		/*encryptedSessionkey.hwklLen 		= NEXUS_SecurityHWKLLenSelect_eLen3;*/
		encryptedSessionkey.keyladderType	= NEXUS_SecurityKeyladderType_e3Des;
		encryptedSessionkey.rootKeySrc		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
		encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eNone;
		encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;
		encryptedSessionkey.keyDestIVType	= NEXUS_SecurityKeyIVType_eNoIV;
		encryptedSessionkey.operation		= NEXUS_SecurityOperation_eDecrypt;
		encryptedSessionkey.operationKey2	= NEXUS_SecurityOperation_eEncrypt;
		encryptedSessionkey.bASKMMode	= true;
		encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
		encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
		encryptedSessionkey.bSwapAESKey = false;
		
		encryptedSessionkey.cusKeyL 		= 0x00; 
		encryptedSessionkey.cusKeyH 		= 0x00; 
		encryptedSessionkey.cusKeyVarL	= 0x00; 
		encryptedSessionkey.cusKeyVarH	= 0xFF; 
		encryptedSessionkey.bRouteKey		= false;
		/*encryptedSessionkey.hwklDestAlg = algorithm.algorithm;*/
		encryptedSessionkey.rootKeySwap = false;
	
		if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
			encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
		else
			encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
		
		encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;
	
		/* Perform key swap for proc-in. */
		BKNI_Memcpy(&(proc_in3[0][0]), &(CONSTANTs[0][8]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(&(proc_in3[0][8]), &(CONSTANTs[0][0]), sizeof(TCsdUnsignedInt8)*8);
	
		BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);
	
		if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
		{
#ifdef CSD_DEBUG
			printf("\nLoading session key failed \n");
#endif
			retCode = CSD_ERROR;
			goto CSD_P_DONE;
		}
		BKNI_Memset((void *)&key4, 0, sizeof(key4));
		/* Load key4 */
		key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
			key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
			key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
		/*key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;*/
			key4.bASKMMode	= true;
			key4.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
			key4.operation = NEXUS_SecurityOperation_eDecrypt;	   
		key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
		/*key4.hwklDestAlg = algorithm.algorithm;*/
		if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
		{
				key4.bRouteKey = false;
			key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
				key4.keySize = 16; 
			/* Perform key swap for proc-in. */
			BKNI_Memcpy(&(proc_in3[1][0]), &(CONSTANTs[1][8]), sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(&(proc_in3[1][8]), &(CONSTANTs[1][0]), sizeof(TCsdUnsignedInt8)*8);
				
			BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);
			
		}
		else
		{
				key4.bRouteKey = true;
			key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
				key4.keySize = 16; 
			BKNI_Memcpy(key4.keyData,  CONSTANT_2, key4.keySize);
		}
		
		key4.keyMode = NEXUS_SecurityKeyMode_eRegular;
		
		if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
		{
#ifdef CSD_DEBUG
			printf("\nLoading key4 failed for video ODD key\n");
#endif
			retCode = CSD_ERROR;
			goto CSD_P_DONE;
		}
		
		if(CSD_FPK_NB_OF_PROTECTING_KEYS == 2)
		{
			/* Load key5 */
			BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
			encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
				encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
					encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
		/*encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;*/
			encrytedCW.bASKMMode	= true;
				encrytedCW.keySize = 16; 
				encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	
			BKNI_Memcpy(encrytedCW.keyData, CONSTANT_2, encrytedCW.keySize);
		/*	BKNI_Memcpy(&(encrytedCW.keyData[0]), &(pxCipheredContentKey[8]), sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(&(encrytedCW.keyData[8]), &(pxCipheredContentKey[0]), sizeof(TCsdUnsignedInt8)*8);
	*/
				encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;	 
				encrytedCW.bRouteKey = true;
			encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
			encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
		/*	encrytedCW.hwklDestAlg = algorithm.algorithm;*/
	
			if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
			{
#ifdef CSD_DEBUG
				printf("\nLoading CW key failed for video ODD key\n");
#endif
				retCode = CSD_ERROR;
				goto CSD_P_DONE;
			}
		}

	return CSD_NO_ERROR;
	
	CSD_P_DONE:
		csdFreeHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
		return (retCode);
	return CSD_ERROR;
}

TCsdStatus csdSetClearTextFlashProtectionKey
(
        TCsdFPKeyPathHandle*         pxFPKeyPathHandle,
  const TCsdUnsignedInt8*            pxClearTextFlashProtectionKey
)
{
	
		NEXUS_SecurityKeySlotSettings keySettings;
		NEXUS_KeySlotHandle keyHandle;
		NEXUS_SecurityAlgorithmSettings algorithm;
		NEXUS_SecurityClearKey key;
		TCsdStatus				   retCode = CSD_NO_ERROR;
	
		if( CSD_INITIALIZED == FALSE )
			return CSD_ERROR;
	
	
	
		if(pxFPKeyPathHandle == NULL)
			return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
		pxFPKeyPathHandle->initHandle = csdInitParameters;
		/* Checks to make sure all input is valid. */
		/* Make sure clear key is 16 bytes. */
		if(pxClearTextFlashProtectionKey == NULL)
			return CSD_ERROR_INVALID_PARAMETERS;

		NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
		keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
		if(pxFPKeyPathHandle->keyHandle == NULL){
			keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
			pxFPKeyPathHandle->keyHandle = keyHandle;
	
		if( keyHandle == NULL) 
		{ 
			return	CSD_ERROR;	/* return if no keyslot is available*/
		}
		else
		{
			csdInsertHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
		}
	}else keyHandle = pxFPKeyPathHandle->keyHandle ;
	
		/* Set up encryption key */
			NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);
	

				algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			
				algorithm.enableExtIv = false;


		algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
		if(pxFPKeyPathHandle->bEncrypt)
		algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
		else 
			algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
		/* Load algorithm to key table.  Set it up to do encryption. */
		if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
		{
			BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
			retCode =  CSD_ERROR;
			goto CSD_P_DONE;
		}
		
		
	
		NEXUS_Security_GetDefaultClearKey(&key);
	
		key.keyIVType	= NEXUS_SecurityKeyIVType_eNoIV;
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
			key.keySize = 16;
			
			BKNI_Memcpy(key.keyData, pxClearTextFlashProtectionKey, 16);
	
		/* Load clear key to key table. */
			if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
			{
				BDBG_WRN(("\nLoad decryption key failed \n"));
				retCode = CSD_ERROR;
				goto CSD_P_DONE;
			}

	return CSD_NO_ERROR;
	
	CSD_P_DONE:
		csdFreeHandleNode(pxFPKeyPathHandle->initHandle, keyHandle);
		return (retCode);
	
}


TCsdStatus csdEncryptDataWithClearTextFlashProtectionKey
(
        TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
  const TCsdUnsignedInt8*             pxInputData,
        TCsdUnsignedInt8*             pxOutputData,
        TCsdSize                       xDataSize
)
{

	 if( CSD_INITIALIZED == FALSE )
		 return CSD_ERROR;

	if(pxFPKeyPathHandle->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return CSD_ERROR; 
	}

	if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
		return CSD_ERROR;
	return CSD_NO_ERROR;
}

TCsdStatus csdDecryptDataWithClearTextFlashProtectionKey
(
        TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
  const TCsdUnsignedInt8*             pxInputData,
        TCsdUnsignedInt8*             pxOutputData,
        TCsdSize                       xDataSize
)
{
	if( CSD_INITIALIZED == FALSE )
		 return CSD_ERROR;

	if(pxFPKeyPathHandle->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return CSD_ERROR; 
	}

	if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
		return CSD_ERROR;
	return CSD_NO_ERROR;
}



TCsdStatus csdSetClearTextRam2RamHostKey
(
        TCsdUnsignedInt16              xEmi,
        TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
  const TCsdUnsignedInt8*             pxClearTextR2RHostKey,
        TCsdSize                       xClearTextR2RHostKeySize
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;
	TCsdStatus                 retCode = CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
	return CSD_ERROR;



	if(pxR2RKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
	pxR2RKeyPathHandle->initHandle = csdInitParameters;
	/* Checks to make sure all input is valid. */
	/* Make sure clear key is 16 bytes. */
	if(xClearTextR2RHostKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;
	/* Make sure algorithm is AES-128 or 3DES-ABA. */
	if(xEmi > CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR|| xEmi <CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
		return CSD_ERROR_INVALID_PARAMETERS;
#if 0
	/* If performing AES-CBC, and new IV will be loaded, make sure IV is 16 bytes. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		(xRefreshIv == TRUE) &&
		(xInitializationVectorSize != 16))
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES-CBC, and new IV will be loaded, make sure IV is 8bytes. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		(xRefreshIv == TRUE) &&
		(xInitializationVectorSize != 8))
		return CSD_ERROR_INVALID_PARAMETERS;
#endif
	/* If performing AES, make sure data size is a multiple of 16 bytes. */
	if(((xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) || \
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) || \
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR)) &&		
		( (xClearTextR2RHostKeySize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if(((xEmi == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) || (xEmi == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR)) &&		
		( (xClearTextR2RHostKeySize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
if(pxR2RKeyPathHandle->keyHandle == NULL){
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	pxR2RKeyPathHandle->keyHandle = keyHandle;

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	}
}else keyHandle = pxR2RKeyPathHandle->keyHandle ;

	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	switch(xEmi)
	{
		#if 0
		case CSD_Algorithm_DVB_CSA2:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eDvb;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			algorithm.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			break;

	  case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;
#endif

	  case CSD_Algorithm_AES_128_ECB_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			algorithm.enableExtIv = false;
			break;

	  case CSD_Algorithm_AES_128_CBC_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			/*algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;*/
			break;

	  case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;

			break;
	
	case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
	case CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;
			
	  case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
	  case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			/*algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;*/
			/* algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;*/
			algorithm.enableExtIv = false;
			break;

	  default:
			break;

		
	}
#if 0
	switch(xMode)
	{
		case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}
#endif	
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	if(pxR2RKeyPathHandle->bEncrypt)
	algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	else 
		algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
	    retCode =  CSD_ERROR;
		goto CSD_P_DONE;
   	}
   	
   	
	BKNI_Memcpy((void *)&pxR2RKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 

	NEXUS_Security_GetDefaultClearKey(&key);

	key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    	key.keySize = xClearTextR2RHostKeySize;
		
    	BKNI_Memcpy(key.keyData, pxClearTextR2RHostKey, sizeof(TCsdUnsignedInt8)*xClearTextR2RHostKeySize);

	/* Load clear key to key table. */
    	if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
       		BDBG_WRN(("\nLoad decryption key failed \n"));
        	retCode = CSD_ERROR;
			goto CSD_P_DONE;
    	}
#if 0
	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		key.keySize = 16;
		
		memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);
		
		if(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1)
  			BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		retCode = CSD_ERROR;
				goto CSD_P_DONE;
    		}
    	}


#endif
	return CSD_NO_ERROR;

CSD_P_DONE:
   	csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);

	
	
}



TCsdStatus csdSetProtectedRam2RamContentKeys
(
        TCsdUnsignedInt16              xEmi,
        TCsdR2RKeyPathHandle*         pxR2RKeyPathHandle,
  const TCsdR2RCipheredProtectingKeys  xR2RCipheredProtectingKeys,
  const TCsdUnsignedInt8*             pxCipheredContentKey,
        TCsdSize                       xCipheredContentKeySize
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle=NULL;
	NEXUS_SecurityAlgorithmSettings algorithm;
   	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	TCsdR2RCipheredProtectingKeys proc_in3;
	TCsdStatus                 retCode = CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
	return CSD_ERROR;


	if(pxR2RKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
	/* Checks to make sure all input is valid. */
	/* Make sure clear key is 16 bytes. */
	if(xCipheredContentKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;
	/* Make sure algorithm is AES-128 or 3DES-ABA. */

	if(xEmi > CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR|| xEmi <CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
		return CSD_ERROR_INVALID_PARAMETERS;


#if 0
	/* If performing AES-CBC, and new IV will be loaded, make sure IV is 16 bytes. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		(xRefreshIv == TRUE) &&
		(xInitializationVectorSize != 16))
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES-CBC, and new IV will be loaded, make sure IV is 8bytes. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		(xRefreshIv == TRUE) &&
		(xInitializationVectorSize != 8))
		return CSD_ERROR_INVALID_PARAMETERS;
#endif
	/* If performing AES, make sure data size is a multiple of 16 bytes. */


	if(((xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||\
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) || \
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||\
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR) ) &&		
		( (xCipheredContentKeySize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if(((xEmi == CSD_Algorithm_RAW_AES_128_ECB_CLEAR) || (xEmi == CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR)) &&		
		( (xCipheredContentKeySize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
 	NEXUS_Security_GetDefaultKeySlotSettings((const) &keySettings);
	
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	pxR2RKeyPathHandle->keyHandle = keyHandle;




	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);

	}
	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	switch(xEmi)
	{
		#if 0
		case CSD_Algorithm_DVB_CSA2:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eDvb;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			algorithm.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			break;

	  case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;
#endif

	  case CSD_Algorithm_AES_128_ECB_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

	  case CSD_Algorithm_AES_128_CBC_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

	  case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;

			break;
	
	case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
	case CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;
			
	  case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
	  case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

	  default:
			break;

		
	}
#if 0
	switch(xMode)
	{
		case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}
#endif	
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	if(pxR2RKeyPathHandle->bEncrypt)
	algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	else 
		algorithm.operation = NEXUS_SecurityOperation_eDecrypt;

	algorithm.caVendorID=0x97ed;

	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		algorithm.askmModuleID = 0xE0;
	else
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
	
	algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
	algorithm.testKey2Select = 0x0;
	algorithm.key2Select =  NEXUS_SecurityKey2Select_eFixedKey;
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
	    retCode =  CSD_ERROR;
		goto CSD_P_DONE;
   	}
   	
   	
	BKNI_Memcpy((void *)&pxR2RKeyPathHandle->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 
	BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));

	/* Load session key3 */
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	encryptedSessionkey.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
    	encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eNone;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	encryptedSessionkey.keyDestIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.bASKMMode 	= true;
    	encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
    	encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    	encryptedSessionkey.bSwapAESKey = false;

	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 	= 0x00; 
	encryptedSessionkey.cusKeyVarH 	= 0xFF; 
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
	encryptedSessionkey.rootKeySwap	= false;

	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eReserved20;
	else
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	
	encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

	/* Perform key swap for proc-in. */
	BKNI_Memcpy(&(proc_in3[0][0]), &(xR2RCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);
	BKNI_Memcpy(&(proc_in3[0][8]), &(xR2RCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);

	BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TCsdUnsignedInt8)*16);

	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
	{
#ifdef CSD_DEBUG
		printf("\nLoading session key failed \n");
#endif
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
	BKNI_Memset((void *)&key4, 0, sizeof(key4));
	/* Load key4 */
	key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    	key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
		key4.bASKMMode 	= true;
    	key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;	
    	key4.operation = NEXUS_SecurityOperation_eDecrypt;     
	key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	key4.hwklDestAlg = algorithm.algorithm;
	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
	{
	    	key4.bRouteKey = false;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
    		key4.keySize = 16; 
		/* Perform key swap for proc-in. */
		BKNI_Memcpy(&(proc_in3[1][0]), &(xR2RCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(&(proc_in3[1][8]), &(xR2RCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);
			
		BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);
		
	}
	else
	{
	    	key4.bRouteKey = true;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
    		key4.keySize = xCipheredContentKeySize; 
		BKNI_Memcpy(key4.keyData,  pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);
	}
	
	key4.keyMode = NEXUS_SecurityKeyMode_eRegular;
	
	if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
	{
#ifdef CSD_DEBUG
		printf("\nLoading key4 failed for video ODD key\n");
#endif
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
	
	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
	{
		/* Load key5 */
		BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
		encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
	    	encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
				encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
		encrytedCW.bASKMMode 	= true;
	    	encrytedCW.keySize = xCipheredContentKeySize; 
	    	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

		BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);

	    	encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
	    	encrytedCW.bRouteKey = true;
		encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
		encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
		encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
		encrytedCW.hwklDestAlg = algorithm.algorithm;

		if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
		{
#ifdef CSD_DEBUG
			printf("\nLoading CW key failed for video ODD key\n");
#endif
			retCode = CSD_ERROR;
			goto CSD_P_DONE;
		}
	}
#if 0
	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		key.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		key.keySize = 16;
		
		memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);
		
		if(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1)
  			BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		retCode = CSD_ERROR;
				goto CSD_P_DONE;
    		}
    	}


#endif
	return CSD_NO_ERROR;

CSD_P_DONE:
   	csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);


}

TCsdStatus csdSetProtectedScrEncryptionKey
(
        TCsdUnsignedInt16               xEmi,
  const TCsdScrCipheredProtectingKeys   xScrCipheredProtectingKeys,
  const TCsdUnsignedInt8*               pxCipheredScrContentKey,
        TCsdSize                        xCipheredScrContentKeySize,
  const TCsdUnsignedInt8*               pxInitializationVector,
        TCsdSize                        xInitializationVectorSize,
        TCsdScrKeyPathHandle*           pxScrKeyPathHandle
)
{
	return csdSetProtectedScrEncryptionDecryptionKey(
														xEmi,
														xScrCipheredProtectingKeys,
														pxCipheredScrContentKey,
														xCipheredScrContentKeySize,
														pxInitializationVector,
														xInitializationVectorSize,
														pxScrKeyPathHandle,
														true);
}

TCsdStatus csdEncryptDataWithFlashProtRootKey
(
        TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
  const TCsdUnsignedInt8*             pxInputData,
        TCsdUnsignedInt8*             pxOutputData,
        TCsdSize                       xDataSize
)
{
	if( CSD_INITIALIZED == FALSE )
		 return CSD_ERROR;

	if(pxFPKeyPathHandle->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return CSD_ERROR; 
	}

	if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
		return CSD_ERROR;

	return CSD_NO_ERROR;
}


TCsdStatus csdDecryptDataWithFlashProtRootKey
(
        TCsdFPKeyPathHandle*          pxFPKeyPathHandle,
  const TCsdUnsignedInt8*             pxInputData,
        TCsdUnsignedInt8*             pxOutputData,
        TCsdSize                       xDataSize
)
{
	if( CSD_INITIALIZED == FALSE )
		 return CSD_ERROR;

	if(pxFPKeyPathHandle->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return CSD_ERROR; 
	}

	if(csdR2RCryptoOperation(pxFPKeyPathHandle->keyHandle , pxInputData, pxOutputData, xDataSize))
		return CSD_ERROR;

	return CSD_NO_ERROR;
}
