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
#include "bchp_jtag_otp.h"

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
#include "nocs1x_csd_impl.h"
#include "nocs1x_csd.h"
#include "bdbg.h"
#include "bint.h"
#include "bhsm_otpmsp.h"

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
#else
#error "unknown chip"
#endif

#include <stdio.h>

BDBG_MODULE(NCSD);

TCsdInitParameters*  csdInitParameters[CSD_MAX_INDEX]={NULL};
bool CSD_INITIALIZED = FALSE;

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
	unsigned char *input_data;
	unsigned char *output_data;
	TCsdStatus    retCode  =  CSD_NO_ERROR;
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
            byteSwap(&(input_data[i]));
#endif

	/* Open DMA handle */
	dma = NEXUS_Dma_Open(0, NULL);

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = keyHandle;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
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

	NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

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
            byteSwap(&(output_data[i]));
#endif

	/* Copy the data to M2M output buffer. */
	BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );
 	    
	/* Done.  Close DMA. */
	NEXUS_DmaJob_Destroy(dmaJob);
	NEXUS_Dma_Close(dma);

	NEXUS_Memory_Free(output_data);
	NEXUS_Memory_Free(input_data);

   	return retCode;
	

}

unsigned long csdDecodeProdOtpBootStrap(unsigned long data)
{
    unsigned long bootStrap ;
    uint32_t	uStrapBits; 

    bootStrap = 0;

    uStrapBits = (data & CSD_BOOT_MASK0) >> 3;
    if(uStrapBits == 0x3 || uStrapBits == 0x5 || uStrapBits == 0x6 || uStrapBits == 0x7)
        bootStrap = 1;

     uStrapBits = (data & CSD_BOOT_MASK1) >> 6;
     if(uStrapBits == 0x3 || uStrapBits == 0x5 || uStrapBits == 0x6 || uStrapBits == 0x7)
        bootStrap |= 0x2;

     uStrapBits = (data & CSD_BOOT_MASK2) >> 9;
     if(uStrapBits == 0x3 || uStrapBits == 0x5 || uStrapBits == 0x6 || uStrapBits == 0x7)
        bootStrap |= 0x4;

     uStrapBits = (data & CSD_BOOT_MASK3) >> 12;
     if(uStrapBits == 0x3 || uStrapBits == 0x5 || uStrapBits == 0x6 || uStrapBits == 0x7)
        bootStrap |= 0x8;
    
    return bootStrap;
}


void csdWaitForProdOtpCmdComplete(void)
{
    uint32_t status_reg;

    /*' Wait for command to complete
    status_reg  = 0
    While (status_reg  AND &h00000001) = 0 
    	sleep 10
    	status_reg = SECTOP.JTAG_OTP.GENERAL_STATUS_1
    	'print "Polling for status done status_reg = 0x" & hex(status_reg)
    WEnd*/
    status_reg = 0;
    while((status_reg & 0x1) == 0x0)
    {
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_STATUS_1, &status_reg );
    }    

    return;
}

void csdEnableProdOtpWrite(void)
{
   /*Enabled write to OTP by writing F48D authentication value
    ' Write F
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = &h0000000f*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000f );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00200003*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003 );

    csdWaitForProdOtpCmdComplete();

    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );

    /*' Write 4
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = &h00000004*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000004 );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00200003*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003 );

    csdWaitForProdOtpCmdComplete();
    
    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );


    /*' Write 8
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = &h00000008*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000008 );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00200003*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003 );

    csdWaitForProdOtpCmdComplete();
    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );

    /*' Write D
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = &h0000000D*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000D );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00200003*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003 );

    csdWaitForProdOtpCmdComplete();    
    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );

    return;
    
}

unsigned long csdReadRefOk(unsigned long addr)
{
    unsigned long refOk;
    uint32_t general_ctrl_1;
    
    /*' Set CPU mode for OTP access
    general_ctrl_1 = SECTOP.JTAG_OTP.GENERAL_CTRL_1    */
    NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, &general_ctrl_1 );
    
    /*SECTOP.JTAG_OTP.GENERAL_CTRL_1 = general_ctrl_1 or &h00000001*/
    general_ctrl_1 |= BCHP_JTAG_OTP_GENERAL_CTRL_1_jtag_otp_cpu_mode_MASK;
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, general_ctrl_1 );

    csdEnableProdOtpWrite();

    /*' Write Address
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = bit_address*/
     NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, addr );

    /*'--Do the Read
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000*/
     NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0 );
    
    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h20A00001  'set start bit*/
     NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x20A00001 );

    csdWaitForProdOtpCmdComplete();
    
    /*print "Read Out Programming Mode = 0x" & hex(status_reg)*/

    /*read_refOK_status = SECTOP.JTAG_OTP.GENERAL_STATUS_1*/
    NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_STATUS_1, (uint32_t*)&refOk);

    return refOk;
    
}

TCsdStatus csdReadProductionOtp(unsigned long* data, unsigned long addr)
{

unsigned long refOK;

    refOK = csdReadRefOk(addr);

    /*'--Do the Read
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0);

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00A00001  'set start bit*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00A00001);

    csdWaitForProdOtpCmdComplete();
    
    /*'Store data
    read_data =SECTOP.JTAG_OTP.GENERAL_STATUS_0
    print "* GENERAL_STATUS_1= " & hex(status_reg) & "     bit_address= " & bit_address & "  Read data value =" & hex(read_data)
    read_OTP_data = read_data*/
    NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_STATUS_0, (uint32_t*)data );

    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&  */
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x0);

    return CSD_NO_ERROR;

}

TCsdStatus csdProgramProductionOtpLock(unsigned long data, unsigned long addr)
{

    uint32_t general_ctrl_1;

    /* Set CPU mode for OTP access
    general_ctrl_1 = SECTOP.JTAG_OTP.GENERAL_CTRL_1    */
    NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, &general_ctrl_1 );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_1 = general_ctrl_1 or &h00000001*/
    general_ctrl_1 |= BCHP_JTAG_OTP_GENERAL_CTRL_1_jtag_otp_cpu_mode_MASK;
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, general_ctrl_1 );

    csdEnableProdOtpWrite();

    /*print "Read Out Programming Mode = 0x" & hex(status_reg)*/

    /*' Data to Write  
    SECTOP.JTAG_OTP.GENERAL_CTRL_2 = bit_data*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_2, data );

    /*' Address to write to
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = bit_address*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, addr );

    /*' Activate Program Word
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00A00017 */
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00F0000B );

    csdWaitForProdOtpCmdComplete();

    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );
    
    return CSD_NO_ERROR;

}



TCsdStatus csdProgramProductionOtp(unsigned long data, unsigned long addr)
{

    uint32_t general_ctrl_1;

    /* Set CPU mode for OTP access
    general_ctrl_1 = SECTOP.JTAG_OTP.GENERAL_CTRL_1    */
    NEXUS_Platform_ReadRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, &general_ctrl_1 );

    /*SECTOP.JTAG_OTP.GENERAL_CTRL_1 = general_ctrl_1 or &h00000001*/
    general_ctrl_1 |= BCHP_JTAG_OTP_GENERAL_CTRL_1_jtag_otp_cpu_mode_MASK;
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_1, general_ctrl_1 );

    csdEnableProdOtpWrite();

    /*print "Read Out Programming Mode = 0x" & hex(status_reg)*/

    /*' Data to Write  
    SECTOP.JTAG_OTP.GENERAL_CTRL_2 = bit_data*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_2, data );

    /*' Address to write to
    SECTOP.JTAG_OTP.GENERAL_CTRL_3 = bit_address*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_3, addr );

    /*' Activate Program Word
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00A00017 */
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00F00017 );

    csdWaitForProdOtpCmdComplete();

    /*'Reset OTP CPU CONTROL
    SECTOP.JTAG_OTP.GENERAL_CTRL_0 = &h00000000&*/
    NEXUS_Platform_WriteRegister( BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000 );
    
    return CSD_NO_ERROR;

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
	static char SoftwareVersion[] = "NOCS-1.2.0-7358-1.0"; 

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	
	if(xSoftwareVersion==NULL)
		return CSD_ERROR;
	
	BKNI_Memcpy(xSoftwareVersion, SoftwareVersion, sizeof(SoftwareVersion));
	
	return CSD_NO_ERROR;
}
TCsdStatus csdGetChipExtension(
  TCsd20CharsString xChipExtension
)
{
	
	unsigned char	chipExtension[2]; 
       unsigned long prodOtpData;


	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(xChipExtension ==NULL)
		return CSD_ERROR;
    
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	/* Read the current boot mode. */
      csdReadProductionOtp(&prodOtpData, CSD_CHIP_EXTENSION_ADDR);

	chipExtension[1] = 0;

	chipExtension[0] = 0x40 + ((prodOtpData >> 20) & 0x3F);

	if(((prodOtpData >> 14) & 0x3F) != 0x0)		
		chipExtension[1] = 0x40 + ((prodOtpData >> 20) & 0x3F);

	xChipExtension[0] = chipExtension[0];
	xChipExtension[1] = chipExtension[1];
	
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
	TCsd4BytesVector pxNuid;
	unsigned char *r2rDataIn;
	unsigned char *crcInput;
	unsigned long uNUIDcheckNum; 
	TCsdR2RKeyPathHandle *r2rHandle;	
	TCsdSize		contKeySize = 16;
	TCsdStatus      retCode = CSD_NO_ERROR;
	const TCsdR2RCipheredProtectingKeys CONSTANT_1 = 
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};
	
	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	};

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if( xCheckNumber == NULL )
		return CSD_ERROR_INVALID_PARAMETERS;

	if(csdGetNuid( (unsigned char *)&pxNuid[0] )!=CSD_NO_ERROR) 
		return CSD_ERROR;
	
#ifdef CSD_DEBUG	
	printf("csdGetNuid()\n");
	for( i=0; i<4; i++ )
		printf("0x%02x ", pxNuid[i]);
	printf("\n\n");
#endif

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&r2rDataIn);	
	NEXUS_Memory_Allocate(sizeof(unsigned char) * 16, NULL, (void *)&crcInput);
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&r2rHandle);

	r2rHandle->initHandle = csdInitParameters[0]; 

	/* Initialize all data buffer to zero.. */
	memset(r2rDataIn, 0, sizeof(unsigned char) * 16 );

	/* Copy the NUID to data buffer. */
	BKNI_Memcpy((void *)&(r2rDataIn[12]), (void *)pxNuid, 4 );

#ifdef CSD_DEBUG
	int i;

	printf("padded r2rDataIn[16]\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", r2rDataIn[i]);
	printf("\n");	
#endif
	
	/* 3DES-ECB encrypt the padded NUID with secret content key. */
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
  		r2rDataIn,
        	crcInput,
        	16
        	) != CSD_NO_ERROR)
	{
       	retCode = CSD_ERROR;
		goto CSD_P_DONE;
	}
	
#ifdef CSD_DEBUG
	printf("crcInput\n");
	for( i=0; i<16; i++ )
		printf("0x%02x ", crcInput[i]);
	printf("\n");
#endif

	/* Perform a CRC-32-IEEE 802.3 on the resulting encrypted 16 bytes */
	genCrc32Tab();
	uNUIDcheckNum = cmpCrc32(crcInput,16) ;	

#ifdef CSD_DEBUG
	printf("NUID checkNum=0x%08x", uNUIDcheckNum);	
#endif

	xCheckNumber[3] = uNUIDcheckNum &0xff;
	xCheckNumber[2] = (uNUIDcheckNum>>8) &0xff; 
	xCheckNumber[1] = (uNUIDcheckNum>>16) &0xff; 
	xCheckNumber[0] = (uNUIDcheckNum>>24) &0xff; 

CSD_P_DONE:

	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(r2rDataIn);
	NEXUS_Memory_Free(crcInput);

	return (retCode);
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
	const TCsdR2RCipheredProtectingKeys CONSTANT_1 = {
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}
	};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	};

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
	const TCsdR2RCipheredProtectingKeys CONSTANT_1 = {
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}
	};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	};

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
	const TCsdR2RCipheredProtectingKeys CONSTANT_1= {
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}
	};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	};

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
	NEXUS_Memory_Free(r2rHandle);
	NEXUS_Memory_Free(r2rDataIn);
	NEXUS_Memory_Free(crcInput);
	
	
	return (retCode);	
}


TCsdStatus csdSetBootMode(
	TCsdBootMode xBootMode)
{
	uint32_t ubootStrapReg; 
	uint32_t unandConfigReg; 
	uint32_t unandConfigBlockSize; 
	uint32_t unandConfigDeviceSize; 
	uint32_t unandConfigPageSize; 
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;	
	ECsdBootMode strapBootMode;
	TCsdBootMode BootMode;

       unsigned long prodOtpData;
       unsigned long prodBootStrap;

    
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;
	
	if(xBootMode >= CSD_BOOT_MODE_MAX) 
		return CSD_ERROR_INVALID_PARAMETERS; 

	/* Read the current boot mode. */
      csdReadProductionOtp(&prodOtpData, CSD_BOOT_STRAP_ADDR);

      prodBootStrap = csdDecodeProdOtpBootStrap(prodOtpData);

      if((prodOtpData & CSD_BOOT_OVERRIDE_MASK) == 0x0)
        BootMode = CSD_BOOT_NOT_SET;
      else if(prodBootStrap<=0x6)
        BootMode = CSD_BOOT_FROM_NAND;
      else if(prodBootStrap<=0x9)
        BootMode = CSD_BOOT_FROM_SPI;
      else if(prodBootStrap<=0xB)
        BootMode = CSD_BOOT_FROM_NOR;
      else
        return CSD_ERROR;

	if(BootMode == CSD_BOOT_NOT_SET)
	{

		NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_STRAP_VALUE_0, &ubootStrapReg);

		ubootStrapReg &= BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK;
             ubootStrapReg = ubootStrapReg >> BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;
			  
              if(ubootStrapReg<=0x6)
                strapBootMode = CSD_BOOT_FROM_NAND;
              else if(ubootStrapReg<=0x9)
                strapBootMode = CSD_BOOT_FROM_SPI;
              else if(ubootStrapReg<=0xB)
                strapBootMode = CSD_BOOT_FROM_NOR;
              else
                return CSD_ERROR;

		if(xBootMode != strapBootMode)
			return CSD_ERROR_OPERATION_NOT_ALLOWED;
		
		if(xBootMode == CSD_BOOT_FROM_NAND)
		{
			NEXUS_Platform_ReadRegister( BCHP_NAND_CONFIG, &unandConfigReg);
			
			unandConfigBlockSize = unandConfigReg & BCHP_NAND_CONFIG_BLOCK_SIZE_MASK;
			unandConfigBlockSize >>= BCHP_NAND_CONFIG_BLOCK_SIZE_SHIFT;

			unandConfigDeviceSize = unandConfigReg & BCHP_NAND_CONFIG_DEVICE_SIZE_MASK;
			unandConfigDeviceSize >>= BCHP_NAND_CONFIG_DEVICE_SIZE_SHIFT;

			unandConfigPageSize = unandConfigReg & BCHP_NAND_CONFIG_PAGE_SIZE_MASK;
			unandConfigPageSize >>= BCHP_NAND_CONFIG_PAGE_SIZE_SHIFT;

			/* Program the Block Size into OTP. */
			progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
			progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eReserved98;                 /* NandFlashBlockSize */
			progMspIO.dataBitLen     = 3;
			progMspIO.dataBitMask[0] = 0x0;
			progMspIO.dataBitMask[1] = 0x0;
			progMspIO.dataBitMask[2] = 0x0;
			progMspIO.dataBitMask[3] = BCHP_NAND_CONFIG_BLOCK_SIZE_MASK>>BCHP_NAND_CONFIG_BLOCK_SIZE_SHIFT;
			progMspIO.mspData[0]     = 0x0;
			progMspIO.mspData[1]     = 0x0;
			progMspIO.mspData[2]     = 0x0;
			progMspIO.mspData[3]     = unandConfigBlockSize;
			
			/* Issue command to program the OTP. */
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			
			readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eReserved98;                 /* NandFlashBlockSize */
			if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
				return CSD_ERROR;
			
			/* Verify the programmed value is correct. */
			if(readMspIO.mspDataBuf[3] != unandConfigBlockSize) 
				return CSD_ERROR;

			/* Program the Device Size into OTP. */
			progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
			progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eReserved97;                    /* NandFlashDeviceSize  */
			progMspIO.dataBitLen     = 4;
			progMspIO.dataBitMask[0] = 0x0;
			progMspIO.dataBitMask[1] = 0x0;
			progMspIO.dataBitMask[2] = 0x0;
			progMspIO.dataBitMask[3] = BCHP_NAND_CONFIG_DEVICE_SIZE_MASK>>BCHP_NAND_CONFIG_DEVICE_SIZE_SHIFT;
			progMspIO.mspData[0]     = 0x0;
			progMspIO.mspData[1]     = 0x0;
			progMspIO.mspData[2]     = 0x0;
			progMspIO.mspData[3]     = unandConfigDeviceSize;
			
			/* Issue command to program the OTP. */
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			
			readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eReserved97;                    /* NandFlashDeviceSize  */
			if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
				return CSD_ERROR;

			/* Verify the programmed value is correct. */
			if(readMspIO.mspDataBuf[3] != unandConfigDeviceSize) 
				return CSD_ERROR;

			/* Program the Page Size into OTP. */
			progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
			progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eReserved99 ;                     /* NandFlashPageSize */
			progMspIO.dataBitLen     = 3;
			progMspIO.dataBitMask[0] = 0x0;
			progMspIO.dataBitMask[1] = 0x0;
			progMspIO.dataBitMask[2] = 0x0;
			progMspIO.dataBitMask[3] = BCHP_NAND_CONFIG_PAGE_SIZE_MASK>>BCHP_NAND_CONFIG_PAGE_SIZE_SHIFT;
			progMspIO.mspData[0]     = 0x0;
			progMspIO.mspData[1]     = 0x0;
			progMspIO.mspData[2]     = 0x0;
			progMspIO.mspData[3]     = unandConfigPageSize;
			
			/* Issue command to program the OTP. */
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			
			readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eReserved99 ;                     /* NandFlashPageSize */
			if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
				return CSD_ERROR;

			/* Verify the programmed value is correct. */
			if(readMspIO.mspDataBuf[3] != unandConfigPageSize) 
				return CSD_ERROR;

			/* Program the OTP to always use OTP to configure the NAND flash. */
			progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
			progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eReserved100;                          /* NandConfigViaOtpEnable */
			progMspIO.dataBitLen     = 1;
			progMspIO.dataBitMask[0] = 0x0;
			progMspIO.dataBitMask[1] = 0x0;
			progMspIO.dataBitMask[2] = 0x0;
			progMspIO.dataBitMask[3] = 0x1;
			progMspIO.mspData[0]     = 0x0;
			progMspIO.mspData[1]     = 0x0;
			progMspIO.mspData[2]     = 0x0;
			progMspIO.mspData[3]     = 0x1;
			
			/* Issue command to program the OTP. */
			if(NEXUS_Security_ProgramMSP (&progMspIO))
				return CSD_ERROR;
			
			readMspParams.readMspEnum =  NEXUS_OtpCmdMsp_eReserved100;                          /* NandConfigViaOtpEnable */
			if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
				return CSD_ERROR;

			/* Verify the programmed value is correct. */
			if(readMspIO.mspDataBuf[3] != 0x1) 
				return CSD_ERROR;
			
		}

            /* Enable Strap override. */
            prodOtpData = CSD_BOOT_OVERRIDE_MASK;
            
            if((ubootStrapReg & 0x1) == 0x1)
                prodOtpData |= CSD_BOOT_MASK0;
            if((ubootStrapReg & 0x2) == 0x2)
                prodOtpData |= CSD_BOOT_MASK1;
            if((ubootStrapReg & 0x4) == 0x4)
                prodOtpData |= CSD_BOOT_MASK2;
            if((ubootStrapReg & 0x8) == 0x8)
                prodOtpData |= CSD_BOOT_MASK3;
			
            if(csdProgramProductionOtp(prodOtpData, CSD_BOOT_STRAP_ADDR))
                return CSD_ERROR;

		/* Read the current boot mode. */
             csdReadProductionOtp(&prodOtpData, CSD_BOOT_STRAP_ADDR);

             prodBootStrap = csdDecodeProdOtpBootStrap(prodOtpData);

          if((prodOtpData & CSD_BOOT_OVERRIDE_MASK) == 0x0)
            BootMode = CSD_BOOT_NOT_SET;
          else if(prodBootStrap<=0x6)
            BootMode = CSD_BOOT_FROM_NAND;
          else if(prodBootStrap<=0x9)
            BootMode = CSD_BOOT_FROM_SPI;
          else if(prodBootStrap<=0xB)
            BootMode = CSD_BOOT_FROM_NOR;
          else
            return CSD_ERROR;
		  
            if(BootMode != xBootMode)
			return CSD_ERROR;

		/* Once the strap OTP is programmed. Lock it down. */
		
		if(csdProgramProductionOtpLock(0x1, CSD_BOOT_LOCK_ADDR_0))
                return CSD_ERROR;
		if(csdProgramProductionOtpLock(0x1, CSD_BOOT_LOCK_ADDR_1))
                return CSD_ERROR;
		if(csdProgramProductionOtpLock(0x1, CSD_BOOT_LOCK_ADDR_2))
                return CSD_ERROR;
		
             csdReadProductionOtp(&prodOtpData, CSD_BOOT_LOCK_ADDR);

		if((prodOtpData & CSD_BOOT_LOCK_MASK) != CSD_BOOT_LOCK_MASK)
                return CSD_ERROR;
			
			
	}
	else if(BootMode != xBootMode)
		return CSD_ERROR_OPERATION_NOT_ALLOWED;
		
	return CSD_NO_ERROR;
}

TCsdStatus csdGetBootMode(
	TCsdBootMode* pxBootMode)
{
       unsigned long prodOtpData;
       unsigned long prodBootStrap;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxBootMode == NULL)
		return CSD_ERROR;

	/* Read the current boot mode. */
      csdReadProductionOtp(&prodOtpData, CSD_BOOT_STRAP_ADDR);

      prodBootStrap = csdDecodeProdOtpBootStrap(prodOtpData);

      if((prodOtpData & CSD_BOOT_OVERRIDE_MASK) == 0x0)
        *pxBootMode = CSD_BOOT_NOT_SET;
      else if(prodBootStrap<=0x6)
        *pxBootMode = CSD_BOOT_FROM_NAND;
      else if(prodBootStrap<=0x9)
        *pxBootMode = CSD_BOOT_FROM_SPI;
      else if(prodBootStrap<=0xB)
        *pxBootMode = CSD_BOOT_FROM_NOR;
      else
        return CSD_ERROR;

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
		progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
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
 	TCsdScsActivationFlag* pxScsActivationFlagState)
{
	NEXUS_ReadMspParms	readMspParams;
	NEXUS_ReadMspIO readMspIO;
	
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxScsActivationFlagState ==NULL)  
		return CSD_ERROR;

	*pxScsActivationFlagState = CSD_SCS_ACTIVATION_FLAG_NOT_SET; 

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eSecureBootEnable;
	
	if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
		return CSD_ERROR;

	if(readMspIO.mspDataBuf[3]==0x1) 
		*pxScsActivationFlagState = CSD_SCS_ACTIVATION_FLAG_SET; 

	return CSD_NO_ERROR;
}

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

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;          /* UserReg */
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

		progMspIO.progMode = BCMD_Msp_CmdProgMode_eEnum;
		progMspIO.progMspEnum = NEXUS_OtpCmdMsp_eReserved104;          /* UserReg */
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
		
		readMspParams.readMspEnum    = NEXUS_OtpCmdMsp_eReserved104;          /* UserReg */
		if(NEXUS_Security_ReadMSP (&readMspParams, &readMspIO))
			return CSD_ERROR;
		
		BDBG_MSG(("STB CA Serial Number  = 0x%x%x%x%x\n", 
			readMspIO.mspDataBuf[0],
			readMspIO.mspDataBuf[1],
			readMspIO.mspDataBuf[2],
			readMspIO.mspDataBuf[3]));

		if(memcmp(readMspIO.mspDataBuf, xStbCaSn, sizeof(unsigned char) * 4) != 0)
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

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eReserved104;          /* UserReg */
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

		progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
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

		if(memcmp(readMspIO.mspDataBuf, xMarketSegmentId, sizeof(unsigned char) * 4) != 0)
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
	
	progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
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

		progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
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
		progMspIO.progMode       = BCMD_Msp_CmdProgMode_eEnum;
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
 	TCsdKeyLadderDeactivationFlag* pxKeyLadderDeactivationFlagState
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
			*pxKeyLadderDeactivationFlagState = CSD_KEY_LADDER_DEACTIVATION_FLAG_SET;
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
			*pxKeyLadderDeactivationFlagState = CSD_KEY_LADDER_DEACTIVATION_FLAG_NOT_SET;
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
	
	const TCsdR2RCipheredProtectingKeys CONSTANT_1[2] = {
	{{
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	}},
	{{
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
	}}
	};

	const TCsdUnsignedInt8 CONSTANT_2[16] = {
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a								
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

	memset(xDvrKeyR2ROutput, 0, sizeof(unsigned char) * 16 );
	memset(KeySafeR2RInput, 0, sizeof(unsigned char) * 16 );

	/* Copy the KeySafeto M2M input buffer. */
	BKNI_Memcpy((void *)KeySafeR2RInput, (void *)KeySafe, 16 );

	if( csdDecryptDataWithSecretContentKey(
		CSD_R2R_ALGORITHM_TDES_K1K2K1, 
		CSD_R2R_CRYPTO_OPERATION_MODE_ECB, 
		CONSTANT_1[0], 
		CONSTANT_2, 
		contKeySize, 
  		NULL,
  		0, 
  		FALSE,
  		r2rHandle,
  		KeySafeR2RInput,
        	xDvrKeyR2ROutput,
        	16
        	) != CSD_NO_ERROR)
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

TCsdStatus csdEncryptDataWithClearTextHostKey
(
	TCsdR2RAlgorithm xAlgorithm, 
       TCsdR2RCryptoOperationMode xMode, 
  	const TCsdUnsignedInt8* pxClearTextHostKey, 
       TCsdSize xClearTextHostKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize, 
       TCsdBoolean xRefreshIv, 
       TCsdR2RKeyPathHandle* pxR2RKeyPathHandle,
  	const TCsdUnsignedInt8* pxInputData, 
       TCsdUnsignedInt8* pxOutputData, 
       TCsdSize xDataSize
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

	/* Checks to make sure all input is valid. */
	/* Make sure clear key is 16 bytes. */
	if(xClearTextHostKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Make sure algorithm is AES-128 or 3DES-ABA. */
	if(xAlgorithm >= LAST_CSD_R2R_ALGORITHM)
		return CSD_ERROR_INVALID_PARAMETERS;

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

	/* If performing AES, make sure data size is a multiple of 16 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		( (xDataSize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		( (xDataSize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;

	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

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

	switch(xAlgorithm)
	{
		case CSD_R2R_ALGORITHM_AES_128:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			break;
		case CSD_R2R_ALGORITHM_TDES_K1K2K1:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}

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
	algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
	    retCode =  CSD_ERROR;
		goto CSD_P_DONE;
   	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    	key.keySize = xClearTextHostKeySize;
    	BKNI_Memcpy(key.keyData, pxClearTextHostKey, sizeof(TCsdUnsignedInt8)*xClearTextHostKeySize);

	/* Load clear key to key table. */
    	if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
       		BDBG_WRN(("\nLoad decryption key failed \n"));
        	retCode = CSD_ERROR;
			goto CSD_P_DONE;
    	}

	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		NEXUS_Security_GetDefaultClearKey(&key);
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

	if(csdR2RCryptoOperation(keyHandle, pxInputData, pxOutputData, xDataSize))
		retCode = CSD_ERROR;

CSD_P_DONE:
   	csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);
}

TCsdStatus csdDecryptDataWithClearTextHostKey
(
	TCsdR2RAlgorithm xAlgorithm,
       TCsdR2RCryptoOperationMode xMode,
  	const TCsdUnsignedInt8* pxClearTextHostKey,
       TCsdSize xClearTextHostKeySize, 
  	const TCsdUnsignedInt8* pxInitializationVector, 
       TCsdSize xInitializationVectorSize,
       TCsdBoolean xRefreshIv, 
       TCsdR2RKeyPathHandle* pxR2RKeyPathHandle,
  	const TCsdUnsignedInt8* pxInputData, 
       TCsdUnsignedInt8* pxOutputData, 
       TCsdSize xDataSize
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings NexusConfig;
    NEXUS_SecurityClearKey key;
	TCsdStatus             retCode = CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxR2RKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

	/* Checks to make sure all input is valid. */

	/* Make sure clear key is 16 bytes. */
	if(xClearTextHostKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Make sure algorithm is AES-128 or 3DES-ABA. */
	if(xAlgorithm >= LAST_CSD_R2R_ALGORITHM)
		return CSD_ERROR_INVALID_PARAMETERS;

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

	/* If performing AES, make sure data size is a multiple of 16 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		( (xDataSize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		( (xDataSize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;


 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	}
		
	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);

	switch(xAlgorithm)
	{
		case CSD_R2R_ALGORITHM_AES_128:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
			break;
		case CSD_R2R_ALGORITHM_TDES_K1K2K1:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}

	switch(xMode)
	{
		case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}
	
	NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;
	NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

	/* Load algorithm to key table.  Set it up to do decryption. */
	if ( NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig)!= 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
    }

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyIVType 	= NEXUS_SecurityKeyIVType_eNoIV;
    	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    	key.keySize = xClearTextHostKeySize;
    	BKNI_Memcpy(key.keyData, pxClearTextHostKey, sizeof(TCsdUnsignedInt8)*xClearTextHostKeySize);

	/* Load clear key to key table. */
    	if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
	{
       	BDBG_WRN(("\nLoad decryption key failed \n"));
        	retCode = CSD_ERROR;
			goto CSD_P_DONE;
    	}

	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		NEXUS_Security_GetDefaultClearKey(&key);
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

	if(csdR2RCryptoOperation(keyHandle, pxInputData, pxOutputData, xDataSize))
		retCode = CSD_ERROR;

CSD_P_DONE:
   csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);
}

TCsdStatus csdEncryptDataWithSecretContentKey
(
	TCsdR2RAlgorithm xAlgorithm, 
	TCsdR2RCryptoOperationMode xMode,
	const TCsdR2RCipheredProtectingKeys xR2RCipheredProtectingKeys,
	const TCsdUnsignedInt8* pxCipheredContentKey,
	TCsdSize xCipheredContentKeySize, 
	const TCsdUnsignedInt8* pxInitializationVector,
	TCsdSize xInitializationVectorSize,
	TCsdBoolean xRefreshIv,
	TCsdR2RKeyPathHandle* pxR2RKeyPathHandle,
	const TCsdUnsignedInt8* pxInputData,
	TCsdUnsignedInt8* pxOutputData,
	TCsdSize xDataSize 
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings NexusConfig;
    	NEXUS_SecurityClearKey ivkey;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	TCsdR2RCipheredProtectingKeys proc_in3;
	TCsdStatus                    retCode = CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxR2RKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

	/* Checks to make sure all input is valid. */

	/* Make sure clear key is 16 bytes. */
	if(xCipheredContentKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Make sure algorithm is AES-128 or 3DES-ABA. */
	if(xAlgorithm >= LAST_CSD_R2R_ALGORITHM)
		return CSD_ERROR_INVALID_PARAMETERS;

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

	/* If performing AES, make sure data size is a multiple of 16 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		( (xDataSize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		( (xDataSize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;

	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	}
	
	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);

	switch(xAlgorithm)
	{
		case CSD_R2R_ALGORITHM_AES_128:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
			break;
		case CSD_R2R_ALGORITHM_TDES_K1K2K1:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}

	switch(xMode)
	{
		case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}
	
	NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
	NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

    	NexusConfig.caVendorID=0x97ed;

	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_10;
	else
		NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
	
	NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.testKey2Select = 0x0;

	/* Load algorithm to key table.  Set it up to do decryption. */
	if ( NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig)!= 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
	      	retCode = CSD_ERROR;
			goto CSD_P_DONE;
    	}

	NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
	/* Load session key3 */
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
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


	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
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
	
	/* Load key4 */
	NEXUS_Security_GetDefaultControlWordSettings(&key4);
	key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    	key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    	key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;	
    	key4.operation = NEXUS_SecurityOperation_eDecrypt;     
	key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	
	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
	{
	    	key4.bRouteKey = false;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
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
		NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
		encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
	    	encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	    	encrytedCW.keySize = xCipheredContentKeySize; 
	    	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

		BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);

	    	encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
	    	encrytedCW.bRouteKey = true;
		encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
		encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
		encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;

		if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
		{
#ifdef CSD_DEBUG
			printf("\nLoading CW key failed for video ODD key\n");
#endif
			retCode = CSD_ERROR;
			goto CSD_P_DONE;
		}
	}
	
	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		NEXUS_Security_GetDefaultClearKey(&ivkey);
		ivkey.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		ivkey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		ivkey.keySize = 16;
		memset (ivkey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

		if(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1)
  			BKNI_Memcpy(&(ivkey.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(ivkey.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &ivkey) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		retCode = CSD_ERROR;
				goto CSD_P_DONE;
    		}
    	}

	if(csdR2RCryptoOperation(keyHandle, pxInputData, pxOutputData, xDataSize))
		retCode = CSD_ERROR;

CSD_P_DONE:
   csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);

}

TCsdStatus csdDecryptDataWithSecretContentKey
(
	TCsdR2RAlgorithm xAlgorithm,
       TCsdR2RCryptoOperationMode xMode,
  	const TCsdR2RCipheredProtectingKeys xR2RCipheredProtectingKeys,
  	const TCsdUnsignedInt8* pxCipheredContentKey,
       TCsdSize xCipheredContentKeySize,
  	const TCsdUnsignedInt8* pxInitializationVector,
       TCsdSize xInitializationVectorSize,
       TCsdBoolean xRefreshIv,
       TCsdR2RKeyPathHandle* pxR2RKeyPathHandle,
  	const TCsdUnsignedInt8* pxInputData,
       TCsdUnsignedInt8* pxOutputData,
       TCsdSize xDataSize
)
{
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings NexusConfig;
    	NEXUS_SecurityClearKey ivkey;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	TCsdR2RCipheredProtectingKeys proc_in3;
	TCsdStatus                    retCode = CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxR2RKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

	/* Checks to make sure all input is valid. */

	/* Make sure clear key is 16 bytes. */
	if(xCipheredContentKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Make sure algorithm is AES-128 or 3DES-ABA. */
	if(xAlgorithm >= LAST_CSD_R2R_ALGORITHM)
		return CSD_ERROR_INVALID_PARAMETERS;

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

	/* If performing AES, make sure data size is a multiple of 16 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_AES_128) &&		
		( (xDataSize % 16) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
	/* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	if((xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1) &&		
		( (xDataSize % 8) != 0 ) )
		return CSD_ERROR_INVALID_PARAMETERS;
	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	}

	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);

	switch(xAlgorithm)
	{
		case CSD_R2R_ALGORITHM_AES_128:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
			break;
		case CSD_R2R_ALGORITHM_TDES_K1K2K1:
			NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}

	switch(xMode)
	{
		case CSD_R2R_CRYPTO_OPERATION_MODE_ECB:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_R2R_CRYPTO_OPERATION_MODE_CBC:
			NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			retCode = CSD_ERROR_INVALID_PARAMETERS;
			goto CSD_P_DONE;
	}
	
	NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;
	NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    	NexusConfig.caVendorID=0x97ed;
		
	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_10;
	else
		NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
	
	NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.testKey2Select = 0x0;

	/* Load algorithm to key table.  Set it up to do decryption. */
	if ( NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig)!= 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		retCode = CSD_ERROR;
		goto CSD_P_DONE;
    	}

	/* Load session key3 */
	NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
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


	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
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
	
	/* Load key4 */
	NEXUS_Security_GetDefaultControlWordSettings(&key4);
	key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    	key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    	key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;	
    	key4.operation = NEXUS_SecurityOperation_eDecrypt;     
    	key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	key4.keyMode = NEXUS_SecurityKeyMode_eRegular;

	if(CSD_R2R_NB_OF_PROTECTING_KEYS == 2)
	{
	    	key4.bRouteKey = false;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
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
		NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
		encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
	    	encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	    	encrytedCW.keySize = xCipheredContentKeySize; 
	    	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

		BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TCsdUnsignedInt8)*xCipheredContentKeySize);

	    	encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
	    	encrytedCW.bRouteKey = true;
	    	encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
		encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5;
		encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;

		if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
		{
#ifdef CSD_DEBUG
			printf("\nLoading CW key failed for video ODD key\n");
#endif
			retCode = CSD_ERROR;
			goto CSD_P_DONE;
		}
	}
	
	/* Check if IV will be loaded. */
	if((xMode == CSD_R2R_CRYPTO_OPERATION_MODE_CBC) && 
		(xRefreshIv == TRUE))
	{
		NEXUS_Security_GetDefaultClearKey(&ivkey);
		ivkey.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		ivkey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		ivkey.keySize = 16;
		memset (ivkey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

		if(xAlgorithm == CSD_R2R_ALGORITHM_TDES_K1K2K1)
  			BKNI_Memcpy(&(ivkey.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(ivkey.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &ivkey) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
				retCode = CSD_ERROR;
				goto CSD_P_DONE;
    		}
    	}

	if(csdR2RCryptoOperation(keyHandle, pxInputData, pxOutputData, xDataSize))
		retCode = CSD_ERROR;

CSD_P_DONE:	
   csdFreeHandleNode(pxR2RKeyPathHandle->initHandle, keyHandle);
	return (retCode);
}

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
	
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxDscKeyPathHandle ==NULL) 
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

	if( (xEmi !=  CSD_Algorithm_DVB_CSA2) &&
		(xEmi !=  CSD_Algorithm_DVB_CSA3) &&
		(xEmi !=  CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) && 
		(xEmi !=  CSD_Algorithm_AES_128_ECB_CLEAR) &&
		(xEmi !=  CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(xEmi !=  CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&
		(xEmi !=  CSD_Algorithm_TDES_ABA_ECB_CLEAR))
		return CSD_ERROR_OPERATION_NOT_SUPPORTED;

	if(xEmi == CSD_Algorithm_DVB_CSA2)
	{
		if( pxClearTextDscOddHostKey != NULL && xClearTextDscOddHostKeySize == 8 )
		{
			ODDCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxClearTextDscOddHostKey == NULL && xClearTextDscOddHostKeySize == 0) )				
				return CSD_ERROR_INVALID_PARAMETERS;
		}
		
		if( pxClearTextDscEvenHostKey != NULL && xClearTextDscEvenHostKeySize == 8 )
		{
			EVENCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxClearTextDscEvenHostKey == NULL && xClearTextDscEvenHostKeySize == 0))
			{			
				return CSD_ERROR_INVALID_PARAMETERS;
			}
			else
			{			
				if( ODDCLEARKEYVALID == FALSE )					
					return CSD_ERROR_INVALID_PARAMETERS;
			}
		}
	}
	else
	{
		if( pxClearTextDscOddHostKey != NULL && xClearTextDscOddHostKeySize == 16 )
		{
			ODDCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxClearTextDscOddHostKey == NULL && xClearTextDscOddHostKeySize == 0) )				
				return CSD_ERROR_INVALID_PARAMETERS;
		}
		
		if( pxClearTextDscEvenHostKey != NULL && xClearTextDscEvenHostKeySize == 16 )
		{
			EVENCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxClearTextDscEvenHostKey == NULL && xClearTextDscEvenHostKeySize == 0))
			{
				return CSD_ERROR_INVALID_PARAMETERS;
			}
			else
			{
				if( ODDCLEARKEYVALID == FALSE )					
					return CSD_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	/* If using non-zero IV, make sure IV is valid. */
	if( (xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(pxDscKeyPathHandle->initializationVector == NULL) )
		return CSD_ERROR_INVALID_PARAMETERS;
		
	if(pxDscKeyPathHandle->initHandle->pidchannel>256)
		return CSD_ERROR;
	
	/* it's an allocated keyslot? */
	keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxDscKeyPathHandle->initHandle->pidchannel);

	if (!keyHandle)
	{	/* if not, try to allocate a new one*/

	 	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    		keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa; 
        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

		if( keyHandle == NULL) 
		{ 
			return  CSD_ERROR;  /* return if no keyslot is available*/
		}
		else
		{
			csdInsertHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
		}
		
		errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
			pxDscKeyPathHandle->initHandle->pidchannel);
		
		if (errCode != 0) 
		{
			BDBG_WRN((" NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}
	}
	
	if ( xClearTextDscOddHostKeySize > 0 ||xClearTextDscEvenHostKeySize > 0 )		
	{
	    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
		NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa; 

		switch(xEmi)
		{
			case CSD_Algorithm_DVB_CSA2:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
				break;
			case CSD_Algorithm_DVB_CSA3:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvbCsa3;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
				break;
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			default:
				break;
		}
			
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd; 
	
   		errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
	
		if (errCode != 0) 
		{
			BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}				
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
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) )
	{
		NEXUS_Security_GetDefaultClearKey(&clearKey);
		clearKey.keyIVType= NEXUS_SecurityKeyIVType_eIV; 
		clearKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		clearKey.keySize = 16;
		clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;
			
		memset (clearKey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);
			
		if(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
  			BKNI_Memcpy(&(clearKey.keyData[8]), xZeroVector, sizeof(TCsdUnsignedInt8)*8);
		else if(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR)
  			BKNI_Memcpy(clearKey.keyData, pxDscKeyPathHandle->initializationVector, sizeof(TCsdUnsignedInt8)*16);
		else
  			BKNI_Memcpy(clearKey.keyData, xZeroVector, sizeof(TCsdUnsignedInt8)*16);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &clearKey) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		goto CSD_P_DONE;
    		}
    	}

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}

/* CA key ladder */

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
    	NEXUS_SecurityClearKey ivkey;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BERR_Code errCode=CSD_NO_ERROR;
	bool	ODDCLEARKEYVALID = FALSE;
	bool EVENCLEARKEYVALID = FALSE;
	
	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	if(pxDscKeyPathHandle ==NULL) 
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
	
	if( (xEmi !=  CSD_Algorithm_DVB_CSA2) &&
		(xEmi !=  CSD_Algorithm_DVB_CSA3) &&
		(xEmi !=  CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) && 
		(xEmi !=  CSD_Algorithm_AES_128_ECB_CLEAR) &&
		(xEmi !=  CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(xEmi !=  CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&
		(xEmi !=  CSD_Algorithm_TDES_ABA_ECB_CLEAR))
		return CSD_ERROR_OPERATION_NOT_SUPPORTED;

	
	if(xEmi == CSD_Algorithm_DVB_CSA2)
	{
		if( pxCipheredDscOddContentKey != NULL && xCipheredDscOddContentKeySize == 8 )
		{
			ODDCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxCipheredDscOddContentKey == NULL && xCipheredDscOddContentKeySize == 0) )
				return CSD_ERROR_INVALID_PARAMETERS;
		}
		
		if( pxCipheredDscEvenContentKey != NULL && xCipheredDscEvenContentKeySize == 8 )
		{
			EVENCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxCipheredDscEvenContentKey == NULL && xCipheredDscEvenContentKeySize == 0))
			{
				return CSD_ERROR_INVALID_PARAMETERS;
			}
			else
			{
				if( ODDCLEARKEYVALID == FALSE )
					return CSD_ERROR_INVALID_PARAMETERS;
			}
		}
	}
	else
	{
		if( pxCipheredDscOddContentKey != NULL && xCipheredDscOddContentKeySize == 16 )
		{
			ODDCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxCipheredDscOddContentKey == NULL && xCipheredDscOddContentKeySize == 0) )
				return CSD_ERROR_INVALID_PARAMETERS;
		}
		
		if( pxCipheredDscEvenContentKey != NULL && xCipheredDscEvenContentKeySize == 16 )
		{
			EVENCLEARKEYVALID = TRUE;
		}
		else
		{
			if( !(pxCipheredDscEvenContentKey == NULL && xCipheredDscEvenContentKeySize == 0))
			{
				return CSD_ERROR_INVALID_PARAMETERS;
			}
			else
			{
				if( ODDCLEARKEYVALID == FALSE )
					return CSD_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	/* If using non-zero IV, make sure IV is valid. */
	if( (xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(pxDscKeyPathHandle->initializationVector == NULL) )
		return CSD_ERROR_INVALID_PARAMETERS;
		
	if(pxDscKeyPathHandle->initHandle->pidchannel>256)
		return CSD_ERROR;
	
	NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
	NEXUS_Security_GetDefaultControlWordSettings(&CW);
	NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
	
	/* it's an allocated keyslot? */
	keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxDscKeyPathHandle->initHandle->pidchannel);

	if (!keyHandle)
	{	/* if not, try to allocate a new one*/
	 	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    		keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa; 
			
        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

		if( keyHandle == NULL) 
		{ 
			return  CSD_ERROR;  /* return if no keyslot is available*/
		}
		else
		{
			csdInsertHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
		}
		
		errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
			pxDscKeyPathHandle->initHandle->pidchannel);
		
		if (errCode != 0) 
		{
			BDBG_WRN((" NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}
	}
	
	if ( xCipheredDscOddContentKeySize > 0 ||xCipheredDscEvenContentKeySize > 0 )		
	{
	    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
		NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa; 
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven; 



		switch(xEmi)
		{
			case CSD_Algorithm_DVB_CSA2:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
				break;
			case CSD_Algorithm_DVB_CSA3:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvbCsa3;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
				break;
			case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_AES_128_ECB_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_AES_128_CBC_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
				NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
				break;
			default:
				break;
		}

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
				NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_5;
	    			SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			    	CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			    	encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			}
			else
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
				NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_9;
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
				NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_3;
	    			SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
		   		CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL2;
			}
			else
			{
				SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
				CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
				NexusConfig.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;
	    			SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
		    		CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL3;
			}
		}

   		errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
	
		if (errCode != 0) 
		{
			BDBG_WRN(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}
		
	}
	
	if ( xCipheredDscOddContentKeySize > 0)		
	{
		SessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA; 
		SessionKey.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
		SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
	    	SessionKey.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
		SessionKey.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
		SessionKey.keyEntryType = NEXUS_SecurityKeyType_eOdd; 		
		SessionKey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
		SessionKey.operation = NEXUS_SecurityOperation_eDecrypt; 
		SessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
		SessionKey.bASKMMode = true;
		SessionKey.keyMode = NEXUS_SecurityKeyMode_eRegular;
    		SessionKey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    		SessionKey.bSwapAESKey = false;
		
		SessionKey.cusKeyL = 0;
		SessionKey.cusKeyH = 0;
		SessionKey.cusKeyVarL = 0;
		SessionKey.cusKeyVarH = 0;
		SessionKey.bRouteKey = false;
			
		BKNI_Memcpy(&(proc_in3[0][0]), &(xDscCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);		
		BKNI_Memcpy(&(proc_in3[0][8]), &(xDscCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(SessionKey.keyData,&(proc_in3[0][0]), 16);

		errCode = NEXUS_Security_GenerateSessionKey( keyHandle, &SessionKey);
		
		if (errCode != 0) 
		{
			BDBG_MSG(("NEXUS_Security_GenerateSessionKey errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}	

		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
		{
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			CW.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			CW.bASKMMode = true;
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			CW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			CW.bSwapAESKey = false;
			CW.bRouteKey = false;

			BKNI_Memcpy(&(proc_in3[1][0]), &(xDscCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);		
			BKNI_Memcpy(&(proc_in3[1][8]), &(xDscCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);
	
			BKNI_Memcpy(CW.keyData,&(proc_in3[1][0]), 16);
			CW.keySize = 16;

	       	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateControlWord (odd) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			

		}
		else
		{
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			CW.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			CW.bASKMMode = true;
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			CW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			CW.bSwapAESKey = false;
			CW.bRouteKey = true;

			BKNI_Memcpy(CW.keyData,pxCipheredDscOddContentKey, xCipheredDscOddContentKeySize);
			CW.keySize = xCipheredDscOddContentKeySize;

	       	 errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateControlWord (odd) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			
		}

		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
		{
			encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			encrytedCW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
			encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			encrytedCW.bASKMMode = true;
			encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			encrytedCW.bSwapAESKey = false;
			encrytedCW.bRouteKey = true;

			BKNI_Memcpy(encrytedCW.keyData,pxCipheredDscOddContentKey, xCipheredDscOddContentKeySize);
			encrytedCW.keySize = xCipheredDscOddContentKeySize;

	       	 errCode = NEXUS_Security_GenerateKey5(keyHandle , &encrytedCW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateKey5 (odd) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			
			
		}

	}

	if ( xCipheredDscEvenContentKeySize > 0)		
	{
		SessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA; 
		SessionKey.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
		SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
	    	SessionKey.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
		SessionKey.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
		SessionKey.keyEntryType = NEXUS_SecurityKeyType_eEven; 		
		SessionKey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
		SessionKey.operation = NEXUS_SecurityOperation_eDecrypt; 
		SessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
		SessionKey.bASKMMode = true;
		SessionKey.keyMode = NEXUS_SecurityKeyMode_eRegular;
    		SessionKey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    		SessionKey.bSwapAESKey = false;
		
		SessionKey.cusKeyL = 0;
		SessionKey.cusKeyH = 0;
		SessionKey.cusKeyVarL = 0;
		SessionKey.cusKeyVarH = 0;
		SessionKey.bRouteKey = false;

		BKNI_Memcpy(&(proc_in3[0][0]), &(xDscCipheredProtectingKeys[0][8]), sizeof(TCsdUnsignedInt8)*8);		
		BKNI_Memcpy(&(proc_in3[0][8]), &(xDscCipheredProtectingKeys[0][0]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(SessionKey.keyData,&(proc_in3[0][0]), 16);
		
		errCode = NEXUS_Security_GenerateSessionKey( keyHandle, &SessionKey);
		
		if (errCode != 0) 
		{
			BDBG_MSG(("NEXUS_Security_GenerateSessionKey errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}	

		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
		{
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			CW.keyEntryType = NEXUS_SecurityKeyType_eEven; 
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			CW.bASKMMode = true;
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			CW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			CW.bSwapAESKey = false;
			CW.bRouteKey = false;

			BKNI_Memcpy(&(proc_in3[1][0]), &(xDscCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);		
			BKNI_Memcpy(&(proc_in3[1][8]), &(xDscCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);
			BKNI_Memcpy(CW.keyData,&(proc_in3[1][0]), 16);
			CW.keySize = 16;


	       	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateControlWord (even) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			

		}
		else
		{
			CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			CW.keyEntryType = NEXUS_SecurityKeyType_eEven; 
			CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			CW.bASKMMode = true;
			CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			CW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			CW.bSwapAESKey = false;
			CW.bRouteKey = true;

			BKNI_Memcpy(CW.keyData,pxCipheredDscEvenContentKey, xCipheredDscEvenContentKeySize);
			CW.keySize = xCipheredDscEvenContentKeySize;

	       	 errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateControlWord (even) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			
		}

		if(CSD_DSC_NB_OF_PROTECTING_KEYS == 2)
		{
			encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
			encrytedCW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
			encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
		    	encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
			encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven; 
			encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
			encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
			encrytedCW.bASKMMode = true;
			encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt; 
    			encrytedCW.bSwapAESKey = false;
			encrytedCW.bRouteKey = true;

			BKNI_Memcpy(encrytedCW.keyData,pxCipheredDscEvenContentKey, xCipheredDscEvenContentKeySize);
			encrytedCW.keySize = xCipheredDscEvenContentKeySize;

	       	 errCode = NEXUS_Security_GenerateKey5(keyHandle , &encrytedCW);
			
			if (errCode != 0) 
			{
				BDBG_WRN(("NEXUS_Security_GenerateKey5 (even) errCode: %x\n", errCode ));	
        		goto CSD_P_DONE;
			}			
			
		}
	}

	/* Check if IV will be loaded. */
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) )
	{	
		NEXUS_Security_GetDefaultClearKey(&ivkey);
		ivkey.keyIVType= NEXUS_SecurityKeyIVType_eIV;
		ivkey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		ivkey.keySize = 16;
		ivkey.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		memset (ivkey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

		if(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
  			BKNI_Memcpy(&(ivkey.keyData[8]), xZeroVector, sizeof(TCsdUnsignedInt8)*8);
		else if(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR)
  			BKNI_Memcpy(ivkey.keyData, pxDscKeyPathHandle->initializationVector, sizeof(TCsdUnsignedInt8)*16);
		else
  			BKNI_Memcpy(ivkey.keyData, xZeroVector, sizeof(TCsdUnsignedInt8)*16);
		
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &ivkey) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		goto CSD_P_DONE;
    		}
    	}

	return CSD_NO_ERROR;
CSD_P_DONE: 
   csdFreeHandleNode(pxDscKeyPathHandle->initHandle, keyHandle);
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

	/* Make sure the EMI is supported. */
	if((xEmi != CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) &&
		(xEmi != CSD_Algorithm_AES_128_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(xEmi != CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&
		(xEmi != CSD_Algorithm_TDES_ABA_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_AES_128_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_0x4023) &&
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR) )
		return CSD_ERROR_INVALID_PARAMETERS;

	/* If the EMI selects TDES with CBC, then check the size of the IV=8. */
	if(((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR)) &&
		(xInitializationVectorSize != 8))
		return CSD_ERROR_INVALID_PARAMETERS;

	/* If the EMI selects AES with CBC, then check the size of the IV=16. */
	if(((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_0x4023) ||		
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))&&
		(xInitializationVectorSize != 16))
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Check to make sure the IV is 0 if the AES EMI requries it. */
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))
	{
		if(memcmp( xZeroVector, pxInitializationVector, 16 ))
			return CSD_ERROR_INVALID_PARAMETERS;
	}
	
	/* Check to make sure the IV is 0 if the 3DES EMI requries it. */
	if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
	{
		if(memcmp( xZeroVector, pxInitializationVector, 8 ))
			return CSD_ERROR_INVALID_PARAMETERS;
	}
	
	/* Check handle to make sure it is valid. */
	if(pxScrKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	}

	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
	algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;

	switch(xEmi)
	{
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;
		case CSD_Algorithm_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;
		case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_0x4023:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;			
		default:
			goto CSD_P_DONE;
	}

	algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		goto CSD_P_DONE;
    	}

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
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_0x4023) ||		
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) )
	{	
		NEXUS_Security_GetDefaultClearKey(&key);
		key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		key.keySize = 16;
		memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

		if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
  			BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		goto CSD_P_DONE;
    		}
    	}	

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

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

	/* Make sure the EMI is supported. */
	if((xEmi != CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) &&
		(xEmi != CSD_Algorithm_AES_128_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_AES_128_CBC_CLEAR) &&
		(xEmi != CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) &&
		(xEmi != CSD_Algorithm_TDES_ABA_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_0x4023) &&
		(xEmi != CSD_Algorithm_RAW_AES_128_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) &&
		(xEmi != CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR) )
		return CSD_ERROR_INVALID_PARAMETERS;

	/* If the EMI selects TDES with CBC, then check the size of the IV=8. */
	if(((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR)) &&
		(xInitializationVectorSize != 8))
		return CSD_ERROR_INVALID_PARAMETERS;

	/* If the EMI selects AES with CBC, then check the size of the IV=16. */
	if(((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_0x4023) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))&&
		(xInitializationVectorSize != 16))
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Check to make sure the IV is 0 if the AES EMI requries it. */
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR))
	{
		if(memcmp( xZeroVector, pxInitializationVector, 16 ))
			return CSD_ERROR_INVALID_PARAMETERS;
	}
	
	/* Check to make sure the IV is 0 if the 3DES EMI requries it. */
	if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
	{
		if(memcmp( xZeroVector, pxInitializationVector, 8 ))
			return CSD_ERROR_INVALID_PARAMETERS;
	}

	if(pxScrKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;
	
 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
    	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	if( keyHandle == NULL) 
	{ 
		return  CSD_ERROR;  /* return if no keyslot is available*/
	}
	else
	{
		csdInsertHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	}

	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
	algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;

	switch(xEmi)
	{
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;
		case CSD_Algorithm_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;
		case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case CSD_Algorithm_RAW_0x4023:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;			
		default:
			goto CSD_P_DONE;
	}
	
	algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg dec keyslot failed \n"));	
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
       	BDBG_WRN(("\nLoad decryption key failed \n"));
		goto CSD_P_DONE;
    	}

	/* Check if IV will be loaded. */
	/* Check if IV will be loaded. */
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(xEmi == CSD_Algorithm_RAW_0x4023) ||		
		(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) )
	{
		NEXUS_Security_GetDefaultClearKey(&key);
		key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		key.keySize = 16;
		memset (key.keyData, 0, sizeof(TCsdUnsignedInt8)*16);
			
		if((xEmi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(xEmi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
  			BKNI_Memcpy(&(key.keyData[8]), pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);
		else
  			BKNI_Memcpy(key.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		goto CSD_P_DONE;
    		}
    	}

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
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings algorithm;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	TCsdR2RCipheredProtectingKeys proc_in3;
    	NEXUS_SecurityClearKey ivkey;
	const TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BERR_Code errCode=CSD_NO_ERROR;

	if( CSD_INITIALIZED == FALSE )
		return CSD_ERROR;

	/* Checks to make sure all input is valid. */
	/* Make sure content key is 16 bytes. */
	if(xCipheredScrContentKeySize != 16)
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Make sure the EMI is supported. */
	if((xEmi != CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) &&
		(xEmi != CSD_Algorithm_AES_128_ECB_CLEAR) &&
		(xEmi != CSD_Algorithm_AES_128_CBC_CLEAR))
		return CSD_ERROR_INVALID_PARAMETERS;


	/* If the EMI selects AES with CBC, then check the size of the IV=16. */
	if(((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR))&&
		(xInitializationVectorSize != 16))
		return CSD_ERROR_INVALID_PARAMETERS;

	/* Check to make sure the IV is 0 if the AES EMI requries it. */
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52))
	{
		if(memcmp( xZeroVector, pxInitializationVector, 16 ))
			return CSD_ERROR_INVALID_PARAMETERS;
	}
	
	/* Check handle to make sure it is valid. */
	if(pxScrKeyPathHandle == NULL)
		return CSD_ERROR_INVALID_KEY_PATH_HANDLE;

	if(pxScrKeyPathHandle->initHandle->pidchannel>=256)
		return CSD_ERROR;
	
	/* it's an allocated keyslot? */
	keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxScrKeyPathHandle->initHandle->pidchannel);

	if (!keyHandle)
	{	/* if not, try to allocate a new one*/	
	 	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    		keySettings.keySlotEngine = NEXUS_SecurityEngine_eCa; 
			
        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

		if( keyHandle == NULL) 
		{ 
			return  CSD_ERROR;  /* return if no keyslot is available*/
		}
		else
		{
			csdInsertHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
		}
		
		errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
			pxScrKeyPathHandle->initHandle->pidchannel);
		
		if (errCode != 0) 
		{
			BDBG_WRN((" NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode ));	
			goto CSD_P_DONE;
		}
	}
	

	/* Set up encryption key */
    	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
	algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;

	switch(xEmi)
	{
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			break;
		case CSD_Algorithm_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;			
		case CSD_Algorithm_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;			
		default:
			goto CSD_P_DONE;
	}

	algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;

    	algorithm.caVendorID=0x97ed;

	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_9;
	else
		algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_7;
	
	algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
	algorithm.testKey2Select = 0x0;
	
	/* Load algorithm to key table.  Set it up to do encryption. */
	if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	{
		BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		goto CSD_P_DONE;
    	}

	/* Load session key3 */
	NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
    	encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eNone;
	encryptedSessionkey.keyEntryType 	= pxScrKeyPathHandle->initHandle->scramblingControlBits;
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
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
	else
		encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
	
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
	NEXUS_Security_GetDefaultControlWordSettings(&key4);
	key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
	key4.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    	key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	key4.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
    	key4.keyEntryType = pxScrKeyPathHandle->initHandle->scramblingControlBits;	
	key4.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	key4.swizzleType = NEXUS_SecuritySwizzleType_eNone;
	key4.bASKMMode = true;
	key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
    	key4.operation = NEXUS_SecurityOperation_eDecrypt;     
	key4.keyMode = NEXUS_SecurityKeyMode_eRegular;
	key4.bSwapAESKey = false;
	
	if(CSD_SCR_NB_OF_PROTECTING_KEYS == 2)
	{
	    	key4.bRouteKey = false;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
    		key4.keySize = 16; 

		/* Perform key swap for proc-in. */
		BKNI_Memcpy(&(proc_in3[1][0]), &(xScrCipheredProtectingKeys[1][8]), sizeof(TCsdUnsignedInt8)*8);
		BKNI_Memcpy(&(proc_in3[1][8]), &(xScrCipheredProtectingKeys[1][0]), sizeof(TCsdUnsignedInt8)*8);
			
		BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TCsdUnsignedInt8)*16);
	}
	else
	{
	    	key4.bRouteKey = true;
		key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4;
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
		NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
		encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
		encrytedCW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	    	encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		encrytedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
	    	encrytedCW.keyEntryType = pxScrKeyPathHandle->initHandle->scramblingControlBits;
		encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
		encrytedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
		encrytedCW.bASKMMode = true;
		encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5;
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
	if((xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xEmi == CSD_Algorithm_AES_128_CBC_CLEAR))
	{	
		NEXUS_Security_GetDefaultClearKey(&ivkey);
		ivkey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		ivkey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    		ivkey.keySize = 16;
		ivkey.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		memset (ivkey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

		BKNI_Memcpy(ivkey.keyData, pxInitializationVector, sizeof(TCsdUnsignedInt8)*xInitializationVectorSize);

		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(keyHandle, &ivkey) != 0) 
		{
        		BDBG_WRN(("\nLoad IV failed \n"));
        		goto CSD_P_DONE;
    		}
    	}

	return CSD_NO_ERROR;

CSD_P_DONE: 
   csdFreeHandleNode(pxScrKeyPathHandle->initHandle, keyHandle);
	return CSD_ERROR;

}

