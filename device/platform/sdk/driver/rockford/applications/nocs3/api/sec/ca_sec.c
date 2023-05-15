/***************************************************************************
*     (c)2008-2013 Broadcom Corporation
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
* $brcm_Workfile: ca_sec.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 4/3/13 4:52p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/applications/nocs3/api/sec/ca_sec.c $
* 
* Hydra_Software_Devel/8   4/3/13 4:52p qxie
* SW7346-555:Nocs3 integration
* 
* Hydra_Software_Devel/7   3/28/13 2:40p qxie
* SW7346-555:Nocs3 integration
* 
* Hydra_Software_Devel/6   1/2/13 12:57p qxie
* SW7346-555:Nocs3 integration(merged latest ECDH changed from Nagra)
* 
* Hydra_Software_Devel/5   12/13/12 2:31p qxie
* SW7346-555:Nocs3 integration(Fixed ECDSA/ECDH curve parameters)
* 
* Hydra_Software_Devel/4   12/12/12 5:09p qxie
* SW7346-555:Nocs3 integration
* 
* Hydra_Software_Devel/3   12/3/12 2:08p qxie
* SW7346-555:Nocs3 integration
* 
* Hydra_Software_Devel/2   8/3/12 1:58p qxie
* SW7346-555:Nocs3 integration
* 
* Hydra_Software_Devel/1   7/17/12 10:44p yili
* SW7346-555:NOCS3.0 Integration
* 
* 9   4/25/11 12:30p gracew
* SW7125-920: Memory release for all the pointers of each API before return status
* 
* 8   2/7/11 2:28p dbayani
* SWSECURITY-33: Update M2M SEC APIs to free key slot instead of relying on caller to call csdTerminate.
* 
* 
**************************************************************************/

#include <string.h>

#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_ctrl.h"
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
#include "ca_sec.h"
#include "ca_sec_impl.h"
#include "ca_defs.h"
#include "bdbg.h"
#include "bint.h"
#include "bchp_jtag_otp.h"

#include "bsp_s_otp_common.h"
#include "nexus_random_number.h"

#include <openssl/bn.h>

#include <openssl/rsa.h>
#include <openssl/dh.h>
#include <openssl/sha.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/objects.h>
#include <openssl/hmac.h>


#include <stdio.h>

BDBG_MODULE(NSEC);
bool SEC_INITIALIZED = FALSE;

#define SEC_MAX_INDEX 2

TSecInitParameters*  secInitParameters[SEC_MAX_INDEX]={NULL};

/**********************************************************************************************
SEC implementation specific structures
**********************************************************************************************/



/**********************************************************************************************
SEC implementation specific  variables
**********************************************************************************************/



#define BHSM_SLOT_NUM_INIT_VAL		0xFFFF


/**********************************************************************************************
SEC implementation specific internal functions/macros
**********************************************************************************************/
const unsigned char Curve_Table160[6][21]={
	{0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF},
	{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF4,0xC8,0xF9,0x27,0xAE,0xD3,0xCA,0x75,0x22,0x57},
	{0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFC},
	{0x00, 0x1C, 0x97, 0xBE, 0xFC, 0x54, 0xBD, 0x7A, 0x8B, 0x65, 0xAC, 0xF8, 0x9F, 0x81, 0xD4, 0xD4, 0xAD, 0xC5, 0x65, 0xFA, 0x45},
	{0x00, 0x4A, 0x96, 0xB5, 0x68, 0x8E, 0xF5, 0x73, 0x28, 0x46, 0x64, 0x69, 0x89, 0x68, 0xC3, 0x8B, 0xB9, 0x13, 0xCB, 0xFC, 0x82},
	{0x00, 0x23, 0xA6, 0x28, 0x55, 0x31, 0x68, 0x94, 0x7D, 0x59, 0xDC, 0xC9, 0x12, 0x04, 0x23, 0x51, 0x37, 0x7A, 0xC5, 0xFB, 0x32},
};


const unsigned char Curve_Table224[6][28]={
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16,0xA2,0xE0,0xB8,0xF0,0x3E,0x13,0xDD,0x29,0x45,0x5C,0x5C,0x2A,0x3D},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE},
	{0xb4,0x05,0x0a,0x85,0x0c,0x04,0xb3,0xab,0xf5,0x41,0x32,0x56,0x50,0x44,0xb0,0xb7,0xd7,0xbf,0xd8,0xba,0x27,0x0b,0x39,0x43,0x23,0x55,0xff,0xb4},
	{0xb7,0x0e,0x0c,0xbd,0x6b,0xb4,0xbf,0x7f,0x32,0x13,0x90,0xb9,0x4a,0x03,0xc1,0xd3,0x56,0xc2,0x11,0x22,0x34,0x32,0x80,0xd6,0x11,0x5c,0x1d,0x21},
	{0xbd,0x37,0x63,0x88,0xb5,0xf7,0x23,0xfb,0x4c,0x22,0xdf,0xe6,0xcd,0x43,0x75,0xa0,0x5a,0x07,0x47,0x64,0x44,0xd5,0x81,0x99,0x85,0x00,0x7e,0x34},
};

const unsigned char Curve_Table256[6][32]=	{
	{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, 
	{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBC,0xE6,0xFA,0xAD,0xA7,0x17,0x9E,0x84,0xF3,0xB9,0xCA,0xC2,0xFC,0x63,0x25,0x51},
	{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC},
	{0x5a,0xc6,0x35,0xd8,0xaa,0x3a,0x93,0xe7,0xb3,0xeb,0xbd,0x55,0x76,0x98,0x86,0xbc,0x65,0x1d,0x06,0xb0,0xcc,0x53,0xb0,0xf6,0x3b,0xce,0x3c,0x3e,0x27,0xd2,0x60,0x4b},
	{0x6b,0x17,0xd1,0xf2,0xe1,0x2c,0x42,0x47,0xf8,0xbc,0xe6,0xe5,0x63,0xa4,0x40,0xf2,0x77,0x03,0x7d,0x81,0x2d,0xeb,0x33,0xa0,0xf4,0xa1,0x39,0x45,0xd8,0x98,0xc2,0x96},
	{0x4f,0xe3,0x42,0xe2,0xfe,0x1a,0x7f,0x9b,0x8e,0xe7,0xeb,0x4a,0x7c,0x0f,0x9e,0x16,0x2b,0xce,0x33,0x57,0x6b,0x31,0x5e,0xce,0xcb,0xb6,0x40,0x68,0x37,0xbf,0x51,0xf5},
};


const unsigned char Curve_Table384[6][48]=	{
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC7,0x63,0x4D,0x81,0xF4,0x37,0x2D,0xDF,0x58,0x1A,0x0D,0xB2,0x48,0xB0,0xA7,0x7A,0xEC,0xEC,0x19,0x6A,0xCC,0xC5,0x29,0x73},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFC},
	{0xb3,0x31,0x2f,0xa7,0xe2,0x3e,0xe7,0xe4,0x98,0x8e,0x05,0x6b,0xe3,0xf8,0x2d,0x19,0x18,0x1d,0x9c,0x6e,0xfe,0x81,0x41,0x12,0x03,0x14,0x08,0x8f,0x50,0x13,0x87,0x5a,0xc6,0x56,0x39,0x8d,0x8a,0x2e,0xd1,0x9d,0x2a,0x85,0xc8,0xed,0xd3,0xec,0x2a,0xef},
	{0xaa,0x87,0xca,0x22,0xbe,0x8b,0x05,0x37,0x8e,0xb1,0xc7,0x1e,0xf3,0x20,0xad,0x74,0x6e,0x1d,0x3b,0x62,0x8b,0xa7,0x9b,0x98,0x59,0xf7,0x41,0xe0,0x82,0x54,0x2a,0x38,0x55,0x02,0xf2,0x5d,0xbf,0x55,0x29,0x6c,0x3a,0x54,0x5e,0x38,0x72,0x76,0x0a,0xb7},
	{0x36,0x17,0xde,0x4a,0x96,0x26,0x2c,0x6f,0x5d,0x9e,0x98,0xbf,0x92,0x92,0xdc,0x29,0xf8,0xf4,0x1d,0xbd,0x28,0x9a,0x14,0x7c,0xe9,0xda,0x31,0x13,0xb5,0xf0,0xb8,0xc0,0x0a,0x60,0xb1,0xce,0x1d,0x7e,0x81,0x9d,0x7a,0x43,0x1d,0x7c,0x90,0xea,0x0e,0x5f}
};

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


TSecStatus secR2RCryptoOperation(
								 NEXUS_KeySlotHandle keyHandle,
								 const TUnsignedInt8* pxInputData, 
								 TUnsignedInt8* pxOutputData, 
								 TSize xDataSize	
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
	TSecStatus    retCode  =  SEC_NO_ERROR;
	BKNI_EventHandle event;
	NEXUS_Error rc;


#if(CFG_BIG_ENDIAN==1)
	size_t i;
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

SEC_P_DONE:

	/* Done.  Close DMA. */
	NEXUS_DmaJob_Destroy(dmaJob);

	NEXUS_Dma_Close(dma);
	BKNI_DestroyEvent(event);

	NEXUS_Memory_Free(output_data);
	NEXUS_Memory_Free(input_data);

	return retCode;


}


/**********************************************************************************************
SEC Public Functions
**********************************************************************************************/




TSecStatus secGetNuid
(
 TSecNuid* pxNuid
 )
{
	BERR_Code errCode = BERR_SUCCESS;
	NEXUS_ReadOtpIO	otpIo;
	unsigned int i;
	/*
	if( SEC_INITIALIZED == FALSE )
	return SEC_ERROR;
	*/
	if(pxNuid ==NULL)  
		return SEC_ERROR;

	/* Read the ID from OTP. */
	errCode = NEXUS_Security_ReadOTP (
		NEXUS_OtpCmdReadRegister_eKeyID,
		NEXUS_OtpKeyType_eA,
		&otpIo);

	if (errCode != BERR_SUCCESS) 
		return SEC_ERROR;

	/* NUID is the second Dword */
	BKNI_Memcpy((void *)pxNuid, (void *)&(otpIo.otpKeyIdBuf[0+SEC_NUID_LENGTH_32/8]), SEC_NUID_LENGTH_32/8 );

	for (i=0; i<(SEC_NUID_LENGTH_32/8); i++) 
		BDBG_MSG((" %02X ", pxNuid[i])) ;

	return SEC_NO_ERROR;
}

TSecStatus secGetNuid64
(
 TSecNuid64* pxNuid
 )
{
	BERR_Code errCode = BERR_SUCCESS;
	NEXUS_ReadOtpIO	otpIo;
	unsigned int i;
	/*
	if( SEC_INITIALIZED == FALSE )
	return SEC_ERROR;
	*/
	if(pxNuid ==NULL)  
		return SEC_ERROR;

	/* Read the ID from OTP. */
	errCode = NEXUS_Security_ReadOTP (
		NEXUS_OtpCmdReadRegister_eKeyID,
		NEXUS_OtpKeyType_eA,
		&otpIo);

	if (errCode != BERR_SUCCESS) 
		return SEC_ERROR;
	/* NUID is the second Dword */
	BKNI_Memcpy((void *)pxNuid, (void *)&(otpIo.otpKeyIdBuf[0]), SEC_NUID_LENGTH/8 );
	for (i=0; i<(SEC_NUID_LENGTH/8); i++) 
		BDBG_MSG((" %02X ", pxNuid[i])) ;

	return SEC_NO_ERROR;
}

TSecStatus secGetChipsetRevision
(
 const TChar** ppxChipsetRevision
 ){
	 unsigned char	chipVersion[2] = "A0"; 
	 uint32_t uChipNum; 


	 if(ppxChipsetRevision ==NULL)
		 return SEC_ERROR;


	 /* "chip rev=72310020" */

	 NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID, &uChipNum );
	 uChipNum &= 0xff;
	 chipVersion[0] +=(uChipNum>>4);
	 chipVersion[1] += uChipNum & 0x0f;

	 BKNI_Memcpy(ppxChipsetRevision, chipVersion, sizeof(chipVersion));
	 return SEC_NO_ERROR;

}


/*
'****************************************************************************************
'	read a word from OTP map
'****************************************************************************************
Function read_OTP_data(bit_address)
*/
TSecStatus sec_P_ReadOtpData(
	unsigned int  row,
	unsigned int *pData
)
{
	unsigned int  regVal;

	NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, &regVal);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, regVal | 0x00000001); 

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000f); 
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 


	regVal = 0 ;
	while (( regVal & 0x1 ) == 0 )	{
		BKNI_Sleep(10);
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal); 
		BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
	}
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000004); 

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 


	
	regVal = 0 ;
	while (( regVal & 0x1 ) == 0 )	{
		BKNI_Sleep(10);
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal); 
		BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
	}

	
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000008); 

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	regVal = 0 ;
	while (( regVal & 0x1 ) == 0 )	{
		BKNI_Sleep(10);
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal); 
		BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
	}

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000D); 
	
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003); 
	regVal = 0 ;
	while (( regVal & 0x1 ) == 0 )	{
		BKNI_Sleep(10);
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal); 
		BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
	}


	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, row*32);

	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000);
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00A00001); 
	regVal = 0 ;
	while (( regVal & 0x1 ) == 0 )	{
		BKNI_Sleep(10);
		NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal); 
		BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
}
	NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_0, &regVal); 
	*pData = regVal;
	NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);

	return SEC_NO_ERROR;
}

TSecStatus secGetChipsetExtension
(
 const TChar** ppxChipsetExtension
 ){
	 unsigned status_reg;
	 const unsigned int Addr_row = 413;
	 const unsigned int Data_row = 0x01A00001; 
	 const unsigned int expect_data = 0x0000188B;

	 unsigned char chipext[2] = {0};
	 if(ppxChipsetExtension == NULL) return SEC_ERROR; 

	 sec_P_ReadOtpData(13, &status_reg);

#if 1
	 chipext[0] = (unsigned char)((status_reg>>20)&0x3f|0x40);
	chipext[1] = (unsigned char)((status_reg>>14)&0x3f|0x40);
#else
	 /* for test purpose before have a real part */
	 chipext[0] = 'N';
	 chipext[1] = 'P';
#endif
	 BKNI_Memcpy(ppxChipsetExtension, chipext, sizeof(chipext));

	 return SEC_NO_ERROR;
}

TSecStatus secOpenRam2RamEncryptSession
(
 TSecCipherSession*     pxSession
 ){
	 NEXUS_SecurityKeySlotSettings keySettings;
	 NEXUS_KeySlotHandle keyHandle;

	if(pxSession == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	}
	 (*pxSession)->bEncrypt = true;
	 NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	 keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
	 if((*pxSession)->keyHandle == NULL){
		 keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		 (*pxSession)->keyHandle  = keyHandle;

		 if( keyHandle == NULL) 
		 { 
			 return  SEC_ERROR;  /* return if no keyslot is available*/
		 }

	 }
	 return SEC_NO_ERROR;

}

TSecStatus secOpenRam2RamDecryptSession
(
 TSecCipherSession*     pxSession
 ){
	 NEXUS_SecurityKeySlotSettings keySettings;
	 NEXUS_KeySlotHandle keyHandle;

	 if(pxSession == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	}
	 (*pxSession)->bEncrypt = false;
	 NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	 keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
	 if((*pxSession)->keyHandle == NULL){
		 keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		 (*pxSession)->keyHandle  = keyHandle;

		 if( keyHandle == NULL) 
		 { 
			 return  SEC_ERROR;  /* return if no keyslot is available*/
		 }

	 }

	 return SEC_NO_ERROR;

}
TSecStatus secCloseSession
(
 TSecCipherSession      xSession
 ){
	if(xSession == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if(xSession->keyHandle == NULL){  /* not a valid ram2ram session */
		return SEC_ERROR_BAD_PARAMETER; 
	}
	 NEXUS_Security_FreeKeySlot(xSession->keyHandle);
	 /*
	 BKNI_Memset(&(xSession->algorithmConfig), 0, sizeof(NEXUS_SecurityAlgorithmSettings) );
	 xSession->bEncrypt = false;
	 */
	 return SEC_NO_ERROR;
}

TSecStatus secSetClearTextKey
(
 TSecCipherSession   xSession,
 TUnsignedInt16      xEmi,
 TSecKeyParity       xParity,
 size_t              xClearTextKeySize,
 const TUnsignedInt8*     pxClearTextKey
 ){


	 NEXUS_KeySlotHandle keyHandle;
	 NEXUS_SecurityAlgorithmSettings algorithm;
	 NEXUS_SecurityClearKey key;
	 TSecStatus				   retCode = SEC_NO_ERROR;

	 if(xSession== NULL)
		 return SEC_ERROR_BAD_PARAMETER;
	 /* Checks to make sure all input is valid. */
	
	 /* Make sure algorithm is AES-128 or 3DES-ABA. */

	 if(xEmi > SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR|| xEmi <SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
		 return SEC_ERROR_BAD_EMI;

	 /* If performing AES, make sure data size is a multiple of 16 bytes. */
	 if(((xEmi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) || \
		 (xEmi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) || \
		 (xEmi == SEC_Algorithm_RAW_AES_128_CTR) || \
		 (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		 (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR)) &&		
		 ( (xClearTextKeySize % 16) != 0 ) )
		 return SEC_ERROR_BAD_PARAMETER;
	 /* If performing 3DES, make sure data size is a multiple of 8 bytes. */

	 if(((xEmi == SEC_Algorithm_RAW_AES_128_ECB_CLEAR) || (xEmi == SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR)) && 	
		 ( (xClearTextKeySize % 8) != 0 ) )
		 return SEC_ERROR_BAD_PARAMETER;
	 if(xClearTextKeySize == 0) return SEC_ERROR_BAD_PARAMETER;
	 if(pxClearTextKey == NULL)  return SEC_ERROR_BAD_PARAMETER;
	 if(xParity != SEC_KEY_PARITY_UNDEFINED ) return SEC_ERROR_BAD_PARAMETER;
	 keyHandle = xSession->keyHandle ;
	 xSession->xEmi = xEmi;
	 /* Set up encryption key */
	 NEXUS_Security_GetDefaultAlgorithmSettings(&algorithm);

	 switch(xEmi)
	 {


	 case SEC_Algorithm_AES_128_ECB_CLEAR:
	 case SEC_Algorithm_RAW_AES_128_ECB_CLEAR:
		 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
		 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
		 algorithm.enableExtIv = false;
		 break;

	 case SEC_Algorithm_RAW_0x4023:
	 case SEC_Algorithm_AES_128_CBC_CLEAR:
	 case SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
	 case SEC_Algorithm_RAW_AES_128_CBC_CLEAR:
		 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
		 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		 /*algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;*/
		 break;

	case SEC_Algorithm_RAW_0x4024:
	case SEC_Algorithm_RAW_AES_128_CTR:

		 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
		 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCounter;
		 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		 algorithm.aesCounterSize = NEXUS_SecurityAesCounterSize_e32Bits;
		 break;

	 case SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
		 algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
		 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;

		 break;

	 case SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
	 case SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
		 algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
		 break;

	 case SEC_Algorithm_TDES_ABA_ECB_CLEAR:
	 case SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
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

	 algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	 if(xSession->bEncrypt)
		 algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	 else 
		 algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	 /* Load algorithm to key table.  Set it up to do encryption. */
	 if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	 {
		 BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		 retCode =  SEC_ERROR;
		 goto SEC_P_DONE;
	 }

	 /*
	 BKNI_Memcpy((void *)&(xSession->algorithmConfig), (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 
	 */
	 NEXUS_Security_GetDefaultClearKey(&key);

	 key.keyIVType	= NEXUS_SecurityKeyIVType_eNoIV;
	 key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	 key.keySize = xClearTextKeySize;

	 BKNI_Memcpy(key.keyData, pxClearTextKey, sizeof(TUnsignedInt8)*xClearTextKeySize);

	 /* Load clear key to key table. */
	 if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) 
	 {
		 BDBG_WRN(("\nLoad decryption key failed \n"));
		 retCode = SEC_ERROR;
		 goto SEC_P_DONE;
	 }

	 return SEC_NO_ERROR;

SEC_P_DONE:
	 return (retCode);
}

TSecStatus secSet2LevelProtectedKey
(
 TSecCipherSession   xSession,
 TUnsignedInt16      xEmi,
 TSecKeyParity       xParity,
 size_t              xCipheredContentKeySize,
 const TUnsignedInt8*     pxCipheredContentKey,
 size_t              xCipheredProtectingKeySize,
 const TUnsignedInt8*     pxL1CipheredProtectingKey,
 const TUnsignedInt8*     pxL2CipheredProtectingKey
 ){
	 NEXUS_SecurityKeySlotSettings keySettings;
	 NEXUS_KeySlotHandle keyHandle=NULL;
	 NEXUS_SecurityAlgorithmSettings algorithm;
	 NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	 NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
	 TUnsignedInt8 proc_in3[2][16];
	 TSecStatus				   retCode = SEC_NO_ERROR;


	 if(xSession== NULL)
		 return SEC_ERROR_BAD_PARAMETER;
	 /* Checks to make sure all input is valid. */
	 /* Make sure clear key is 16 bytes. */
	 if(xCipheredContentKeySize != 16)
		 return SEC_ERROR_BAD_PARAMETER;
	 if(pxL1CipheredProtectingKey == NULL || pxL2CipheredProtectingKey == NULL) return SEC_ERROR_BAD_PARAMETER;
	 /* Make sure algorithm is AES-128 or 3DES-ABA. */
	 if(xEmi > SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR|| xEmi <SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
		 return SEC_ERROR_BAD_EMI;



	 /* If performing AES, make sure data size is a multiple of 16 bytes. */


	 if(((xEmi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||\
		 (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||\
		 (xEmi==SEC_Algorithm_RAW_AES_128_CTR)||\
		 (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR) ) && 	
		 ( (xCipheredContentKeySize % 16) != 0 ) )
		 return SEC_ERROR_BAD_PARAMETER;
	 /* If performing 3DES, make sure data size is a multiple of 8 bytes. */
	 if(((xEmi == SEC_Algorithm_RAW_AES_128_ECB_CLEAR) || (xEmi == SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR)) && 	
		 ( (xCipheredContentKeySize % 8) != 0 ) )
		 return SEC_ERROR_BAD_PARAMETER;
	 if(xParity != SEC_KEY_PARITY_UNDEFINED ) return SEC_ERROR_BAD_PARAMETER;


	 keyHandle = xSession->keyHandle;
	 xSession->xEmi = xEmi;

	 if( keyHandle == NULL) 
	 { 
		 return	SEC_ERROR_BAD_PARAMETER;	/* return if no keyslot is available*/
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

		case SEC_Algorithm_AES_128_ECB_CLEAR:
		case SEC_Algorithm_RAW_AES_128_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;
        case SEC_Algorithm_RAW_0x4023:
		case SEC_Algorithm_AES_128_CBC_CLEAR:
		case SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
		case SEC_Algorithm_RAW_AES_128_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

		case SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;

			break;
		case SEC_Algorithm_RAW_0x4024:
		case SEC_Algorithm_RAW_AES_128_CTR:
			 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCounter;
			 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			  algorithm.aesCounterSize = NEXUS_SecurityAesCounterSize_e128Bits;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			 break;

		case SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
		case SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

		case SEC_Algorithm_TDES_ABA_ECB_CLEAR:
		case SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
			algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
			break;

		default:
			break;


	 }

	 algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
	 if(xSession->bEncrypt)
		 algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
	 else 
		 algorithm.operation = NEXUS_SecurityOperation_eDecrypt;

	 algorithm.caVendorID=0x97ed;

	 if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
		 algorithm.askmModuleID = 0xE0;
	 else
		 algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;

	 algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
	 algorithm.testKey2Select = 0x0;
	 algorithm.key2Select =	NEXUS_SecurityKey2Select_eFixedKey;
	 /* Load algorithm to key table.  Set it up to do encryption. */
	 if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
	 {
		 BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	
		 retCode =  SEC_ERROR;
		 goto SEC_P_DONE;
	 }

	 /*
	 BKNI_Memcpy((void *)&xSession->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 
	 */
	 BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));

	 /* Load session key3 */
	 encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;

	 encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	 encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	 encryptedSessionkey.hwklLen 		= NEXUS_SecurityHWKLLenSelect_eLen3;
	 encryptedSessionkey.keyladderType	= NEXUS_SecurityKeyladderType_e3Des;
	 encryptedSessionkey.rootKeySrc		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	 encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eNone;
	 encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;
	 encryptedSessionkey.keyDestIVType	= NEXUS_SecurityKeyIVType_eNoIV;
	 encryptedSessionkey.operation		= NEXUS_SecurityOperation_eDecrypt;
	 encryptedSessionkey.operationKey2	= NEXUS_SecurityOperation_eEncrypt;
	 encryptedSessionkey.bASKMMode	= true;
	 encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	 encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
	 encryptedSessionkey.bSwapAESKey = false;

	 encryptedSessionkey.cusKeyL 		= 0x00; 
	 encryptedSessionkey.cusKeyH 		= 0x00; 
	 encryptedSessionkey.cusKeyVarL	= 0x00; 
	 encryptedSessionkey.cusKeyVarH	= 0xFF; 
	 encryptedSessionkey.bRouteKey		= false;
	 encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
	 encryptedSessionkey.rootKeySwap = false;

	 if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
		 encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eReserved20;
	 else
		 encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;

	 encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

	 /* Perform key swap for proc-in. */
	 BKNI_Memcpy(&(proc_in3[0][0]), &(pxL2CipheredProtectingKey[8]), sizeof(TUnsignedInt8)*8);
	 BKNI_Memcpy(&(proc_in3[0][8]), &(pxL2CipheredProtectingKey[0]), sizeof(TUnsignedInt8)*8);

	 BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TUnsignedInt8)*16);

	 if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
	 {
#ifdef CSD_DEBUG
		 printf("\nLoading session key failed \n");
#endif
		 retCode = SEC_ERROR;
		 goto SEC_P_DONE;
	 }
	 BKNI_Memset((void *)&key4, 0, sizeof(key4));
	 /* Load key4 */
	 key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
	 key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	 key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
	 key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
	 key4.bASKMMode	= true;
	 key4.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
	 key4.operation = NEXUS_SecurityOperation_eDecrypt;	   
	 key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	 key4.hwklDestAlg = algorithm.algorithm;
	 if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
	 {
		 key4.bRouteKey = false;
		 key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
		 key4.keySize = 16; 
		 /* Perform key swap for proc-in. */
		 BKNI_Memcpy(&(proc_in3[1][0]), &(pxL1CipheredProtectingKey[8]), sizeof(TUnsignedInt8)*8);
		 BKNI_Memcpy(&(proc_in3[1][8]), &(pxL1CipheredProtectingKey[0]), sizeof(TUnsignedInt8)*8);

		 BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TUnsignedInt8)*16);

	 }
	 else
	 {
		 key4.bRouteKey = true;
		 key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
		 key4.keySize = xCipheredContentKeySize; 
		 BKNI_Memcpy(key4.keyData,  pxCipheredContentKey, sizeof(TUnsignedInt8)*xCipheredContentKeySize);
	 }

	 key4.keyMode = NEXUS_SecurityKeyMode_eRegular;

	 if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
	 {
#ifdef SEC_DEBUG
		 printf("\nLoading key4 failed for video ODD key\n");
#endif
		 retCode = SEC_ERROR;
		 goto SEC_P_DONE;
	 }

	 if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
	 {
		 /* Load key5 */
		 BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
		 encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
		 encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		 encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
		 encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
		 encrytedCW.bASKMMode	= true;
		 encrytedCW.keySize = xCipheredContentKeySize; 
		 encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

		 BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TUnsignedInt8)*xCipheredContentKeySize);

		 encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;	 
		 encrytedCW.bRouteKey = true;
		 encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
		 encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
		 encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
		 encrytedCW.hwklDestAlg = algorithm.algorithm;

		 if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
		 {
#ifdef SEC_DEBUG
			 printf("\nLoading CW key failed for video ODD key\n");
#endif
			 retCode = SEC_ERROR;
			 goto SEC_P_DONE;
		 }
	 }

	 return SEC_NO_ERROR;

SEC_P_DONE:
	 return (retCode);


}

 TSecStatus secUseCertKey
(
        TSecCipherSession   xSession,
        TUnsignedInt16      xEmi,
        TSecKeyParity       xParity
){
		
		  NEXUS_KeySlotHandle keyHandle=NULL;
		  NEXUS_SecurityAlgorithmSettings algorithm;
		
		  NEXUS_SecurityEncryptedControlWord encrytedCW;
		  TUnsignedInt8 proc_in3[2][16];
		  TSecStatus				retCode = SEC_NO_ERROR;
	 
	  NEXUS_Security_PklInit();

		  if(xParity != SEC_KEY_PARITY_UNDEFINED ) return SEC_ERROR_BAD_PARAMETER;
		  if(xSession== NULL)
			  return SEC_ERROR_BAD_PARAMETER;

		  /* Make sure algorithm is AES-128 or 3DES-ABA. */
		  if(xEmi > SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR|| xEmi <SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52)
			  return SEC_ERROR_BAD_EMI;
	  
		  keyHandle = xSession->keyHandle;
		  xSession->xEmi = xEmi;
	 
		  if( keyHandle == NULL) 
		  { 
			  return SEC_ERROR_BAD_PARAMETER;	 /* return if no keyslot is available*/
		  }
#if 0  
	 	  if(xSession->bEncrypt) { /* wrong usage, should be decrypt */
			 return SEC_ERROR_BAD_USAGE;
		  }

#endif
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
	 
			 case SEC_Algorithm_AES_128_ECB_CLEAR:
			 case SEC_Algorithm_RAW_AES_128_ECB_CLEAR:
				 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
				 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
				 break;
			 case SEC_Algorithm_RAW_0x4023:
			 case SEC_Algorithm_AES_128_CBC_CLEAR:
			 case SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
			 case SEC_Algorithm_RAW_AES_128_CBC_CLEAR:
				 algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
				 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
				 break;
	 
			 case SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
				 algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
				 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
	 
				 break;
			 case SEC_Algorithm_RAW_0x4024:
			 case SEC_Algorithm_RAW_AES_128_CTR:
				  algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
				  algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCounter;
				  algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				  algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				   algorithm.aesCounterSize = NEXUS_SecurityAesCounterSize_e128Bits;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
				  break;
	 
			 case SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
			 case SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
				 algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
				 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
				 break;
	 
			 case SEC_Algorithm_TDES_ABA_ECB_CLEAR:
			 case SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
				 algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
				 algorithm.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				 algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				 algorithm.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
				 algorithm.ivMode = NEXUS_SecurityIVMode_eRegular;
				 break;
	 
			 default:
				 break;
	 
	 
		  }
	 
		  algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
		  if(xSession->bEncrypt)
			  algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
		  else 
			  algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
	 
		  algorithm.caVendorID=0x97ed;
	 #if 0
		  if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
			  algorithm.askmModuleID = 0xE0;
		  else
			  algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;
	 #endif
		  algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
		  algorithm.testKey2Select = 0x0;
		  algorithm.key2Select = NEXUS_SecurityKey2Select_eFixedKey;
		  /* Load algorithm to key table.  Set it up to do encryption. */
		  if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
		  {
			  BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));	 
			  retCode =  SEC_ERROR;
			  goto SEC_P_DONE;
		  }
	 
		  /*
		  BKNI_Memcpy((void *)&xSession->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings)); 
		  */
		  BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
	 
		  /* Load session key3 */
		  encrytedCW.keyladderID	 = NEXUS_SecurityKeyladderID_eA;
	 
		  encrytedCW.keyGenCmdID	 = NEXUS_SecurityKeyGenCmdID_eKeyGen;
		  encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_ePKL;
		 
		  encrytedCW.keyladderType  = NEXUS_SecurityKeyladderType_e3Des;
		  encrytedCW.rootKeySrc	 = NEXUS_SecurityRootKeySrc_eOtpKeyA;
		  encrytedCW.swizzleType	 = NEXUS_SecuritySwizzleType_eNone;

		  	encrytedCW.keyEntryType	 = NEXUS_SecurityKeyType_eOdd;

	
		  encrytedCW.keyDestIVType  = NEXUS_SecurityKeyIVType_eNoIV;
		  encrytedCW.operation 	 = NEXUS_SecurityOperation_eDecrypt;
		  encrytedCW.bkeyGenBlocked=true;
		  encrytedCW.bASKMMode  = true;
		  encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL0;
		  encrytedCW.bSwapAESKey = false;
		  encrytedCW.bRouteKey 	 = true;
		  encrytedCW.rootKeySwap = false;
	      encrytedCW.custSubMode =NEXUS_SecurityCustomerSubMode_eReserved21;
	
	 
		  encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
		  encrytedCW.sc01Polarity[NEXUS_SecurityPacketType_eGlobal] = 0x0;
		  encrytedCW.sc01Polarity[NEXUS_SecurityPacketType_eRestricted] = 0x0;
	 
            encrytedCW.keySize = 16;
	 
		  memset(encrytedCW.keyData, 0, sizeof(TUnsignedInt8)*16);
	 
	 
		  if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
		  {
#ifdef SEC_DEBUG
			  printf("\nLoading key4 failed for video ODD key\n");
#endif
			  retCode = SEC_ERROR;
			  goto SEC_P_DONE;
		  }
	 
	
		return SEC_NO_ERROR;
	
SEC_P_DONE:

	return SEC_NO_ERROR; 
 }

#define AON_OFFSET 0x354
TSecStatus secUseFlashProtKey
(
        TSecCipherSession   xSession,
        TUnsignedInt16      xEmi)
{
	unsigned char fekKey[16] = {0};
	unsigned int i;
	unsigned int regVal;
	TSecStatus errCode;

	if(xSession == NULL) {
			return SEC_ERROR_BAD_PARAMETER; 
	}
	if(xSession->keyHandle == NULL){  /* not a valid ram2ram session */
		return SEC_ERROR_BAD_PARAMETER; 
	}

	regVal= 0;
	for(i=0; i<16; i+=4){
		NEXUS_Platform_ReadRegister(BCHP_AON_CTRL_RESET_CTRL+0x354+i,&regVal);
		fekKey[i] = (regVal>>24)&0xff;
		fekKey[i+1] = (regVal>>16)&0xff;
		fekKey[i+2] = (regVal>>8)&0xff;
		fekKey[i+3] = (regVal)&0xff;
		
	}
	/*for(i=0; i<16; i++){printf("fek[%d]=%x\n", i, fekKey[i]); }*/
	errCode=  secSetClearTextKey(xSession,xEmi,SEC_KEY_PARITY_UNDEFINED,16,fekKey);

	return errCode; 
}


 TSecStatus secEncryptFlashProtKey
(
  const TUnsignedInt8*     pxInput,
        TUnsignedInt8*     pxOutput,
        size_t              xSize
){
	 NEXUS_Error errCode;
	 unsigned int inBuffer[16], outBuffer[16], actualOutSize;
	 NEXUS_SecurityKeySlotInfo keyInfo;
	 NEXUS_SecurityKeySlotSettings keySettings;
	 NEXUS_KeySlotHandle keyHandle=NULL;

	 NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	 keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m; 
	 keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	 
	 
	 if( keyHandle == NULL) 
	 { 
		 return  SEC_ERROR;  /* return if no keyslot is available*/
	 }
	
	 if((pxInput == NULL)||(pxOutput == NULL) || (xSize!=16)){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 NEXUS_Security_GetKeySlotInfo(keyHandle, &keyInfo);

	 inBuffer[0] = 0x00000010;
	 inBuffer[1] = 0x00000022; 
	 inBuffer[2] = 0xabcdef00; 
	 inBuffer[3] = 0xde55aa21; 
	 inBuffer[4] = 0x789a0008; 
	 inBuffer[5] = keyInfo.keySlotNumber; 
	 inBuffer[6] = 1; /* encrypt */
	 
	 
	 errCode = NEXUS_Security_SubmitRawCommand (
			 inBuffer,	  /* attr{nelem=sizeInBuffer;reserved=90} */
			 7, 	   /* size of pInBuffer in integers */
			 outBuffer, 		/* [out] attr{nelem=sizeOutBuffer;reserved=90} */
			 6, 	   /* size of pOutBuffer in integers */
			 &actualOutSize 	 /* [out] number of integers written into pOutBuffer */
	 );
	 
	 if ( errCode || (outBuffer[5]!=0 )) 
	 {
		 return SEC_ERROR;
	 }
	errCode = secR2RCryptoOperation(keyHandle , pxInput, pxOutput, xSize);

	NEXUS_Security_FreeKeySlot(keyHandle);

	 
	return errCode;
 }

TSecStatus sec_P_SessionCcryptoOperation
(
 TSecCipherSession   xSession,
 const TUnsignedInt8*     pxInput,
 TUnsignedInt8*     pxOutput,
 size_t              xMessageSize,
 const TUnsignedInt8*     pxInitVector,
 size_t              xInitVectorSize)
{

	NEXUS_SecurityClearKey key;
	unsigned int xEmi = xSession->xEmi;
	int i;
	if(xSession == NULL){
		return SEC_ERROR_BAD_PARAMETER;
	}
	if(xSession->keyHandle == NULL){
		BDBG_ERR(("R2R key is not loaded!\n")); 
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if(pxInput == NULL || pxOutput == NULL ){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	/* load IV key if needed */
	
	if((xEmi == SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xEmi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(xEmi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		(xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR)||
		(xEmi == SEC_Algorithm_RAW_0x4023)||(xEmi == SEC_Algorithm_RAW_0x4024)||
		(xEmi == SEC_Algorithm_RAW_AES_128_CTR)
		)
	{
		if( pxInitVector == NULL){
				return SEC_ERROR_BAD_PARAMETER; 
		}
		if((xEmi == SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		    (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		    (xEmi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR)){
			if(xInitVectorSize != 8)
			return SEC_ERROR_BAD_PARAMETER; 
		}else{
			if(xInitVectorSize != 16)
			return SEC_ERROR_BAD_PARAMETER; 
		}
		BKNI_Memset((void *)&key, 0, sizeof(key));
		NEXUS_Security_GetDefaultClearKey(&key);
		key.keyIVType	= NEXUS_SecurityKeyIVType_eIV;
		key.keyEntryType = NEXUS_SecurityKeyType_eIv;
		key.keySize = 16; /*xInitializationVectorSize;*/

		if(xInitVectorSize== 8){

			BKNI_Memcpy(&(key.keyData[8]), pxInitVector, xInitVectorSize);
		}
		else
			BKNI_Memcpy(key.keyData, pxInitVector, xInitVectorSize);
		/* Load IV. */
		#if 0
		for(i=0; i<xInitVectorSize; i++) printf("%x, " pxInitVector[i]);
		printf("\n");
		#endif
		if (NEXUS_Security_LoadClearKey(xSession->keyHandle , &key) != 0) 
		{
			BDBG_WRN(("\nLoad IV failed \n"));
			return SEC_ERROR;
		}
	}
	if(secR2RCryptoOperation(xSession->keyHandle , pxInput, pxOutput, xMessageSize))
		return SEC_ERROR;
	return SEC_NO_ERROR;
}

TSecStatus secSessionEncrypt
(
 TSecCipherSession   xSession,
 const TUnsignedInt8*     pxInput,
 TUnsignedInt8*     pxOutput,
 size_t              xMessageSize,
 const TUnsignedInt8*     pxInitVector,
 size_t              xInitVectorSize)
{
	if(xSession == NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if(!xSession->bEncrypt){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	return sec_P_SessionCcryptoOperation(xSession,pxInput,pxOutput,xMessageSize,pxInitVector, xInitVectorSize);
}

TSecStatus secSessionDecrypt
(
 TSecCipherSession   xSession,
 const TUnsignedInt8*     pxInput,
 TUnsignedInt8*     pxOutput,
 size_t              xMessageSize,
 const TUnsignedInt8*     pxInitVector,
 size_t              xInitVectorSize
 ){

	if(xSession == NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if(xSession->bEncrypt){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	return sec_P_SessionCcryptoOperation(xSession,pxInput,pxOutput,xMessageSize,pxInitVector, xInitVectorSize);
}

TSecStatus secEncryptData
(
 TUnsignedInt8*  pxOutput,
 const TUnsignedInt8*  pxInput,
 TSize           xDataSize
 ){

	 TSecCipherSession r2rSession;
	 const TUnsignedInt8 CONSTANT[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};
	 TUnsignedInt16      xEmi = SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR;
	 TSecStatus errcode = SEC_NO_ERROR; 

	 if(pxInput == NULL || pxOutput == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 r2rSession = malloc(sizeof(TSecCipherSession));
	 memset (r2rSession, 0, sizeof(TSecCipherSession));
	 if(secOpenRam2RamEncryptSession(&r2rSession))
		 return SEC_ERROR; ;

	 if(secSet2LevelProtectedKey(r2rSession, xEmi,SEC_KEY_PARITY_UNDEFINED,sizeof(CONSTANT), CONSTANT,
		 sizeof(CONSTANT), CONSTANT,CONSTANT )!=SEC_NO_ERROR){
			 return SEC_ERROR;

	 }
	 errcode = secSessionEncrypt(r2rSession, pxInput, pxOutput, xDataSize, NULL, 0); 
	 if(errcode) return errcode; 

	 errcode = secCloseSession(r2rSession);
	 if(errcode) return errcode;
	 free(r2rSession);
	 return SEC_NO_ERROR;
}

TSecStatus secDecryptData
(
 TUnsignedInt8*  pxOutput,
 const TUnsignedInt8*  pxInput,
 TSize           xDataSize
 ){
	 TSecCipherSession r2rSession;
	 const TUnsignedInt8 CONSTANT[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
		 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e};
	 TUnsignedInt16      xEmi = SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR;
	 
	 TSecStatus errcode = SEC_NO_ERROR; 
	 int i;

	 if(pxInput == NULL || pxOutput == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 r2rSession = malloc(sizeof(TSecCipherSession));

	 memset (r2rSession, 0, sizeof(TSecCipherSession));
	 if(secOpenRam2RamDecryptSession(&r2rSession))
		 return SEC_ERROR; ;

	 if(secSet2LevelProtectedKey(r2rSession, xEmi,SEC_KEY_PARITY_UNDEFINED, sizeof(CONSTANT),CONSTANT,
		 sizeof(CONSTANT), CONSTANT,CONSTANT )!=SEC_NO_ERROR){
			 return SEC_ERROR;

	 }
	 errcode = secSessionDecrypt(r2rSession, pxInput, pxOutput, xDataSize, NULL, 0); 
	  if(errcode) return errcode; 
#ifdef SEC_DEBUG
	 for(i=0; i<16; i++) printf("%x, ", pxInput[i]);
	 printf("\n");
	 for(i=0; i<16; i++) printf("%x, ", pxOutput[i]);
#endif
	 errcode = secCloseSession(r2rSession);
	 if(errcode) return errcode; 

	 free(r2rSession);
	 return SEC_NO_ERROR;
}

TSecStatus secSha1Init
(
 TSecHashContext*    pxContext
 ){
	 TSecHashContext shaCtx;

	 if(pxContext == NULL ) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 shaCtx = malloc(sizeof(SSecHashContext));
	if(shaCtx == NULL) {
		BDBG_ERR(("Not enough memory!\n"));
		return SEC_ERROR; 
	}
#if 0
	 if(SHA1_Init(&shaCtx->c)!=1) return SEC_ERROR;
	 *pxContext = shaCtx;
#else

	 NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
	 shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha160;
	 shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
	 shaCtx->hmacSha1Settings.byteSwap= true;
#endif

	 *pxContext = shaCtx;
	 return SEC_NO_ERROR;
}

TSecStatus secSha1Update
(
 TSecHashContext  xContext,
 const TUnsignedInt8*  pxMessageChunk,
 size_t           xChunkSize
 ){
 	if(xContext == NULL || pxMessageChunk == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }

#if 0
	 if(SHA1_Update(&xContext->c, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else

	 xContext->hmacSha1Settings.dataAddress = pxMessageChunk;
	 xContext->hmacSha1Settings.dataSize = xChunkSize;
	 
	 xContext->hmacSha1Settings.dataMode = xContext->datamode;
		
#endif
	 return SEC_NO_ERROR; 

}

TSecStatus secSha1Final
(
 TSecHashContext  xContext,
 TUnsignedInt8*  pxMessageDigest
 ){
 	
 	if(xContext == NULL || pxMessageDigest == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
#if 0
	 if(SHA1_Final(pxMessageDigest, &xContext->c)!=1) return SEC_ERROR;
#else

	 NEXUS_HMACSHA_DigestOutput hmacOut;

	 if(NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)){
		return SEC_ERROR; 
	 };
	 if(xContext->datamode != SEC_SHA_DATA_MODE_eMore) BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);
#endif
	 return SEC_NO_ERROR;
}

TSecStatus secSha256Init
(
 TSecHashContext*    pxContext
 ){
	 TSecHashContext shaCtx;

	 if(pxContext == NULL ) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 shaCtx = malloc(sizeof(SSecHashContext));
	 	if(shaCtx == NULL) {
		BDBG_ERR(("Not enough memory!\n"));
		return SEC_ERROR; 
	}
#if 0	 
	 if(SHA256_Init(&shaCtx->c256)!=1) return SEC_ERROR;
#else

	NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
	 shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
	 shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
	 shaCtx->hmacSha1Settings.byteSwap= true;
#endif
	 *pxContext = shaCtx;

	 return SEC_NO_ERROR;
}

TSecStatus secSha256Update
(
 TSecHashContext  xContext,
 const TUnsignedInt8*  pxMessageChunk,
 size_t           xChunkSize
 ){
 	if(xContext == NULL || pxMessageChunk == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
 #if 0
	 if(SHA256_Update(&xContext->c256, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else
	xContext->hmacSha1Settings.dataAddress = pxMessageChunk;
	xContext->hmacSha1Settings.dataSize = xChunkSize;
	xContext->hmacSha1Settings.dataMode = xContext->datamode;

		
#endif
	 return SEC_NO_ERROR; 

}

TSecStatus secSha256Final
(
 TSecHashContext  xContext,
 TUnsignedInt8*  pxMessageDigest
 ){

 	if(xContext == NULL || pxMessageDigest == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
#if 0
	 if(SHA256_Final(pxMessageDigest, &xContext->c256)!=1) return SEC_ERROR;
#else

	 NEXUS_HMACSHA_DigestOutput hmacOut;
	 if(NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)){
		return SEC_ERROR; 
	 };
	 if(xContext->datamode != SEC_SHA_DATA_MODE_eMore)BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);
#endif
	 return SEC_NO_ERROR;
}

TSecStatus secSha384Init
(
 TSecHashContext*    pxContext
 ){
	 TSecHashContext shaCtx;
	 hash_state *md;

	 if(pxContext == NULL ) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 shaCtx = malloc(sizeof(SSecHashContext));
	 md = malloc(sizeof(hash_state)); 
#if 1 
	#if 0   /* openssl */
	 if(SHA384_Init(&shaCtx->c384)!=1) return SEC_ERROR;
	#else     /* libtm */
	
	shaCtx->md = md; 
	register_hash(&sha384_desc); 
	if(sha384_init(shaCtx->md)) return SEC_ERROR;
	#endif
#else

	NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
	 shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
	 shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
	 shaCtx->hmacSha1Settings.byteSwap= true;
#endif
	 *pxContext = shaCtx;

	 return SEC_NO_ERROR;
}

TSecStatus secSha384Update
(
 TSecHashContext  xContext,
 const TUnsignedInt8*  pxMessageChunk,
 size_t           xChunkSize
 ){
 	if(xContext == NULL || pxMessageChunk == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
 #if 0
 
	 if(SHA384_Update(&xContext->c384, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else
  if(sha384_process(xContext->md, pxMessageChunk, xChunkSize)) return SEC_ERROR;; 
#endif
	 return SEC_NO_ERROR; 

}

TSecStatus secSha384Final
(
 TSecHashContext  xContext,
 TUnsignedInt8*  pxMessageDigest
 ){

 	if(xContext == NULL || pxMessageDigest == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
#if 0
	 if(SHA384_Final(pxMessageDigest, &xContext->c384)!=1) return SEC_ERROR;
#else
	if(sha384_done(xContext->md, pxMessageDigest))return SEC_ERROR;
#endif
	 return SEC_NO_ERROR;
}


TSecStatus secHmacSha256Init
(
 const TUnsignedInt8*      pxKey,
 size_t               xKeySize,
 TSecHashContext*    pxContext
 ){
	 TSecHashContext shaCtx;

	 if(pxContext == NULL ) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 if(pxKey == NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 shaCtx = malloc(sizeof(SSecHashContext));
#if 1
	 NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
	 BKNI_Memcpy(shaCtx->hmacSha1Settings.keyData, pxKey, xKeySize);
	 shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
	 shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eHMAC;
	 shaCtx->hmacSha1Settings.byteSwap= true;

	shaCtx->hmacSha1Settings.keyLength= xKeySize;
	 *pxContext = shaCtx;
#else
ENGINE_load_builtin_engines();
  ENGINE_register_all_complete();

HMAC_CTX_init(&shaCtx->hmac);

if(HMAC_Init_ex(&shaCtx->hmac, pxKey, xKeySize, EVP_sha256(), NULL)!=1) return SEC_ERROR;
*pxContext = shaCtx;

#endif
	 return SEC_NO_ERROR;
}
TSecStatus secHmacSha256Update
(
 TSecHashContext  xContext,
 const TUnsignedInt8*  pxMessageChunk,
 size_t           xChunkSize
 ){
 	if(xContext == NULL || pxMessageChunk == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
#if 1
	 xContext->hmacSha1Settings.dataAddress = pxMessageChunk;
	 xContext->hmacSha1Settings.dataSize = xChunkSize;
	 
	 xContext->hmacSha1Settings.dataMode = xContext->datamode;
	 #if 0
	 printf("size is %d, data mode is %d\n", xChunkSize, xContext->bMoreData);
	 if(xChunkSize < SEC_SHA_MAX_DATA_SIZE && !xContext->bMoreData)xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eAllIn;
	 else if(xChunkSize < SEC_SHA_MAX_DATA_SIZE && xContext->bMoreData){
			 xContext->bMoreData = false;	 
			 xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eLast;
			 }
	else {
		#if 1
		xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eMore;
		#endif
		xContext->bMoreData = true;
		}
	#endif
#else
if(HMAC_Update(&xContext->hmac, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;

#endif
	 return SEC_NO_ERROR; 
}

TSecStatus secHmacSha256Final
(
 TSecHashContext  xContext,
 TUnsignedInt8*  pxMessageDigest
 ){

 	if(xContext == NULL || pxMessageDigest == NULL) {
		return SEC_ERROR_BAD_PARAMETER; 
	 }
#if 1
	 NEXUS_HMACSHA_DigestOutput hmacOut;
	if( NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)){
		return SEC_ERROR; 
	};
	 if(xContext->datamode != SEC_SHA_DATA_MODE_eMore){
	 	BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);
	 }
#else
unsigned int len;
if(HMAC_Final(&xContext->hmac, pxMessageDigest, &len)!=1) return SEC_ERROR;


#endif

	 return SEC_NO_ERROR; 

}

TSecStatus secRsaGenerateKey(
		size_t                xKeySize,
  		TUnsignedInt32        xE,
  		TUnsignedInt8*       pxN,
  		TUnsignedInt8*       pxP,
  		TUnsignedInt8*       pxQ,
  		TUnsignedInt8*       pxDP,
  		TUnsignedInt8*       pxDQ,
  		TUnsignedInt8*       pxQInv

 ){
		unsigned int i;	
		int ret = 0;
		unsigned long out_size;
		rsa_key rsaKey;
		const RAND_METHOD *rng;
		int prng_idx ; 
		if((pxN == NULL) || (pxP == NULL) || (pxQ == NULL)||
			(pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)){
			return SEC_ERROR_BAD_PARAMETER;
		}
		if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}
		ltc_mp = ltm_desc;

		rng = RAND_get_rand_method();
		register_prng(&sprng_desc);
		prng_idx= find_prng("sprng");

		ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
		if(ret) goto err;
		out_size = xKeySize/2;

		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.qP),pxQInv,&out_size);
		if(ret) goto err;
		if(out_size != xKeySize/2){
			memmove(&pxQInv[xKeySize/2-out_size],&pxQInv[0],out_size);
			memset(&pxQInv[0],0,xKeySize/2-out_size);		
		}

		out_size = xKeySize;

		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.N),pxN,&out_size);
		if(ret) goto err;
		if(out_size != xKeySize){
			memmove(&pxN[xKeySize-out_size],&pxN[0],out_size);
			memset(&pxN[0],0,xKeySize-out_size);
		}
 
		out_size = xKeySize/2;

		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.p),pxP,&out_size);
		if(ret) goto err;
		if(out_size != xKeySize/2){
			memmove(&pxP[xKeySize/2-out_size],&pxP[0],out_size);
			memset(&pxP[0],0,xKeySize/2-out_size);
		}
		out_size = xKeySize/2;
		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.q),pxQ,&out_size);
		if(ret) goto err;
		if(out_size != xKeySize/2){
			memmove(&pxQ[xKeySize/2-out_size],&pxQ[0],out_size);
			memset(&pxQ[0],0,xKeySize/2-out_size);
		}
	 
		out_size = xKeySize/2;

		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.dP),pxDP,&out_size);
		if(ret) goto err;
		if(out_size != xKeySize/2){
			memmove(&pxDP[xKeySize/2-out_size],&pxDP[0],out_size);
			memset(&pxDP[0],0,xKeySize/2-out_size);
		}
		
		out_size = xKeySize/2;
		ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.dQ),pxDQ,&out_size);
		if(ret) goto err; 
		if(out_size != xKeySize/2){
			memmove(&pxDQ[xKeySize/2-out_size],&pxDQ[0],out_size);
			memset(&pxDQ[0],0,xKeySize/2-out_size);
		}
		rsa_free(&rsaKey);
		return SEC_NO_ERROR;
		
err:
	return SEC_ERROR; 

}


TSecStatus secRsaComputeCrtParams
(
        size_t                xKeySize,
        TUnsignedInt32        xE,
  const TUnsignedInt8*       pxP,
  const TUnsignedInt8*       pxQ,
        TUnsignedInt8*       pxDP,
        TUnsignedInt8*       pxDQ,
        TUnsignedInt8*       pxQInv
){
	
	 RSA *rsaKey = RSA_new(); 
	 int padding; 
	 int ret = 0;
	 BIGNUM *pr0, *p;
	 BIGNUM *r0=NULL,*r1=NULL,*r2=NULL;
	 BIGNUM local_r0;
	 BIGNUM *local_dmp=NULL, *local_dmq=NULL, *local_p=NULL; 
	 int i;
	 unsigned char d[128]={0};
	  BIGNUM *bne=BN_new(); 
	  unsigned long e = xE; 

	  if((pxP == NULL) || (pxQ == NULL)||
			  (pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)){
			  return SEC_ERROR_BAD_PARAMETER;
		  }
		if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}

	  BN_set_word(bne, e); 
 	  rsaKey->e = bne;
 
	  rsaKey->p = BN_bin2bn(pxP, xKeySize/2, rsaKey->p); 
	  rsaKey->q = BN_bin2bn(pxQ, xKeySize/2, rsaKey->q); 
	  BN_CTX *ctx=NULL;
	 
	 ctx=BN_CTX_new();
	 if (ctx == NULL) goto err;
	 BN_CTX_start(ctx);
	 r0 = BN_CTX_get(ctx);
	 r1 = BN_CTX_get(ctx);
	 r2 = BN_CTX_get(ctx);
	 local_p = BN_CTX_get(ctx);
	 local_dmp = BN_CTX_get(ctx);
	   local_dmq = BN_CTX_get(ctx);
	 if (!BN_sub(r1,rsaKey->p,BN_value_one())) goto err;	   /* p-1 */
	 if (!BN_sub(r2,rsaKey->q,BN_value_one())) goto err;	   /* q-1 */

	 if (!BN_mul(r0,r1,r2,ctx)) goto err;    /* (p-1)(q-1) */

	 if (!(rsaKey->flags & RSA_FLAG_NO_CONSTTIME))
	{
			 pr0 = &local_r0;
			 BN_with_flags(pr0, r0, BN_FLG_CONSTTIME);
		   }
	 else{
		 pr0 = r0;
		 }
	   if (!BN_mod_inverse(r0,rsaKey->e,pr0,ctx)) goto err;   /* d */
	 rsaKey->d =r0;
	if (!BN_mod(local_dmp, rsaKey->d,r1,ctx)) goto err;
		/* calculate d mod (q-1) */
	if (!BN_mod(local_dmq, rsaKey->d,r2,ctx)) goto err;
			/* calculate inverse of q mod p */

		p = rsaKey->p;
	if (!BN_mod_inverse(local_p,rsaKey->q,rsaKey->p,ctx)) goto err;	
	
	ret = BN_bn2bin(local_dmp, pxDP);
	if(ret != xKeySize/2){
		memmove(&pxDP[xKeySize/2-ret],&pxDP[0],ret);
		memset(&pxDP[0],0,xKeySize/2-ret);
	}
	ret = BN_bn2bin(local_dmq, pxDQ);
	if(ret != xKeySize/2){
		memmove(&pxQ[xKeySize/2-ret],&pxQ[0],ret);
		memset(&pxQ[0],0,xKeySize/2-ret);
	}
	ret = BN_bn2bin(local_p, pxQInv);
	if(ret != xKeySize/2){
		memmove(&pxQInv[xKeySize/2-ret],&pxQInv[0],ret);
		memset(&pxQInv[0],0,xKeySize/2-ret);
	}

	return SEC_NO_ERROR;
err:
	return SEC_ERROR;
}

TSecStatus secRsaPublicEncrypt
(
 const TUnsignedInt8*       pxInput,
 size_t                xInputSize,
 TUnsignedInt8*       pxOutput,
 TUnsignedInt32        xE,
 const TUnsignedInt8*       pxN,
 size_t                xKeySize,
 TSecRsaPadding        xPadding
 ){
	int ret = 0;
	unsigned char *tmpbuff; 
	unsigned long tmplen;
	unsigned long mudulusbitlen = xKeySize*8;
	tmpbuff = malloc(xKeySize);
	rsa_key rsaKey;
	int prng_idx; 
	int hash_idx;
	unsigned long x,y;

	if(pxInput == NULL || pxOutput == NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if(pxN==NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}

	ltc_mp = ltm_desc;

	register_prng(&sprng_desc);
	
	prng_idx= find_prng("sprng");
	hash_idx = find_hash("sha1");


	ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
	if(ret) goto err;
	rsaKey.type = PK_PUBLIC;
	ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
	if(ret) goto err;
	mudulusbitlen = mp_count_bits(rsaKey.N); 
	tmplen = xKeySize;
	switch(xPadding)
	{
	case SEC_RSA_PKCS1_V1_5_PADDING : 
		ret = pkcs_1_v1_5_encode(pxInput,xInputSize,LTC_LTC_PKCS_1_EME,mudulusbitlen,NULL,prng_idx,tmpbuff,&tmplen); 
		if(ret) goto err; 
		break;
	case SEC_RSA_OAEP_SHA_1_PADDING :
		register_hash(&sha1_desc);
		hash_idx = find_hash("sha1");
		ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		if(ret) goto err;
		unregister_hash(&sha1_desc);
		break;
	case SEC_RSA_OAEP_SHA_256_PADDING:
		register_hash(&sha256_desc);
		hash_idx = find_hash("sha256");
		ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		if(ret) goto err;
		unregister_hash(&sha256_desc);
		break;
	case SEC_RSA_NO_PADDING:
		memcpy(tmpbuff,pxInput,xInputSize);
		break;
	case SEC_RSA_OAEP_SHA_384_PADDING:
		register_hash(&sha384_desc);
		hash_idx = find_hash("sha384");
		ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		if(ret) goto err;
		unregister_hash(&sha384_desc);
		break;
	case LAST_SEC_RSA_PADDING:
		return SEC_ERROR_BAD_PADDING; 
		break;
	} 
	if(ret!=CRYPT_OK) {
		printf("Padding error!\n"); 
		goto err;
	}
	
	ret = ltc_mp.rsa_me(tmpbuff, xKeySize, pxOutput, &tmplen, PK_PUBLIC, &rsaKey);
	if(ret != CRYPT_OK) goto err; 

	free(tmpbuff);

	 return SEC_NO_ERROR;
err:
	return SEC_ERROR; 
}


TSecStatus secRsaPrivateEncrypt
(
 const TUnsignedInt8*       pxInput,
 size_t                xInputSize,
 TUnsignedInt8*       pxOutput,
  TUnsignedInt32        xE,
 const TUnsignedInt8*       pxN,
  const TUnsignedInt8*       pxP,
  const TUnsignedInt8*       pxQ,
  const TUnsignedInt8*       pxDP,
  const TUnsignedInt8*       pxDQ,
  const TUnsignedInt8*       pxQInv,
 size_t                xKeySize,
 TSecRsaPadding        xPadding
 ){
	 int ret = 0;
	 unsigned char *tmpbuff; 
	 unsigned long tmplen;
	 unsigned long mudulusbitlen = xKeySize*8;
	 tmpbuff = malloc(xKeySize);
	 rsa_key rsaKey;
	 int prng_idx;
	 int hash_idx;

	if(pxInput == NULL || pxOutput == NULL){
		return SEC_ERROR_BAD_PARAMETER; 
	}
	if((pxP == NULL) || (pxQ == NULL)||
	  (pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)){
	  return SEC_ERROR_BAD_PARAMETER;
	}
	if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}

	 ltc_mp = ltm_desc;
	 
	 register_prng(&sprng_desc);
	 
	 prng_idx= find_prng("sprng");
	 hash_idx = find_hash("sha1");
	
	ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
	if(ret) goto err;	 
	 
	ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
	if(ret) goto err;	 

	ret = mp_read_unsigned_bin((mp_int *)rsaKey.p,pxP,xKeySize/2);
	if(ret) goto err;	 

	ret = mp_read_unsigned_bin((mp_int *)rsaKey.q,pxQ,xKeySize/2);
	if(ret) goto err;	 

	ret = mp_read_unsigned_bin((mp_int *)rsaKey.dP,pxDP,xKeySize/2);
	if(ret) goto err;	 
		 
	ret = mp_read_unsigned_bin((mp_int *)rsaKey.dQ,pxDQ,xKeySize/2);
	if(ret) goto err;	 
	 
	ret = mp_read_unsigned_bin((mp_int *)rsaKey.qP,pxQInv,xKeySize/2);
	if(ret) goto err;	 

	 tmplen = xKeySize;
	 switch(xPadding)
	 {
	 case SEC_RSA_PKCS1_V1_5_PADDING : 
		 ret = pkcs_1_v1_5_encode(pxInput,xInputSize,LTC_LTC_PKCS_1_EME,mudulusbitlen,NULL,prng_idx,tmpbuff,&tmplen); 
		 if(ret) goto err;	  

		 break;
	 case SEC_RSA_OAEP_SHA_1_PADDING :
		 register_hash(&sha1_desc);
		 hash_idx = find_hash("sha1");
		 ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		 if(ret) goto err;	  

		 unregister_hash(&sha1_desc);
		 break;
	 case SEC_RSA_OAEP_SHA_256_PADDING:
		 register_hash(&sha256_desc);
		 hash_idx = find_hash("sha256");
		 ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		 if(ret) goto err;	  

		 unregister_hash(&sha256_desc);
		 break;
	 case SEC_RSA_NO_PADDING:
		 memcpy(tmpbuff,pxInput,xInputSize);
		 break;
	 case SEC_RSA_OAEP_SHA_384_PADDING:
		 register_hash(&sha384_desc);
		 hash_idx = find_hash("sha384");
		 ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
		 if(ret) goto err;	 
		 unregister_hash(&sha384_desc);
		 break;
	 case LAST_SEC_RSA_PADDING:
		 return SEC_ERROR_BAD_PADDING; 
		 break;
	 } 
	 if(ret!=CRYPT_OK) {
		 printf("Padding error!\n"); 
		 goto err;
	 }
	 ret = ltc_mp.rsa_me(tmpbuff, xKeySize, pxOutput, &tmplen, PK_PRIVATE, &rsaKey);
	 if(ret != CRYPT_OK) goto err; 
	 
	 free(tmpbuff);

	 return SEC_NO_ERROR;
err:
	return SEC_ERROR; 
}

TSecStatus secRsaPublicDecrypt
(
 const TUnsignedInt8*       pxInput,
 TUnsignedInt8*       pxOutput,
 size_t*              pxOutputSize,
 TUnsignedInt32        xE,
 const TUnsignedInt8*       pxN,
 size_t                xKeySize,
 TSecRsaPadding        xPadding
 ){
		 int ret = 0;
		 unsigned char *tmpbuff; 
		 unsigned long tmplen;
		 unsigned long mudulusbitlen = xKeySize*8;
		 int prng_idx;
	    int hash_idx;
		 int sign_valid = 0;
		 tmpbuff = malloc(xKeySize);

		 if(pxInput == NULL || pxOutput == NULL){
			 return SEC_ERROR_BAD_PARAMETER; 
		 }
		 if(pxN==NULL){
			 return SEC_ERROR_BAD_PARAMETER; 
		 }
		if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}

		 rsa_key rsaKey;
	
		 ltc_mp = ltm_desc;
	 
		 register_prng(&sprng_desc);
		 
		prng_idx= find_prng("sprng");
		hash_idx = find_hash("sha1");
	 
		ret =  rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
		if(ret) goto err;	 
		rsaKey.type = PK_PUBLIC;
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
		 if(ret) goto err;	 
		 ret = ltc_mp.rsa_me(pxInput, xKeySize, tmpbuff, &tmplen, PK_PUBLIC, &rsaKey);
		 if(ret) goto err;	 
		 mudulusbitlen = mp_count_bits(rsaKey.N); 
		 tmplen = xKeySize;
		 switch(xPadding)
		 {
		case SEC_RSA_PKCS1_V1_5_PADDING : 
			ret =	pkcs_1_v1_5_decode(tmpbuff,xKeySize,LTC_LTC_PKCS_1_EME,mudulusbitlen,pxOutput,&tmplen,&sign_valid);
			break;
		case SEC_RSA_OAEP_SHA_1_PADDING :
			register_hash(&sha1_desc);
			hash_idx = find_hash("sha1");
			ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);

			unregister_hash(&sha1_desc);
			break;
		case SEC_RSA_OAEP_SHA_256_PADDING:
			register_hash(&sha256_desc);
			hash_idx = find_hash("sha256");
			ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);

			unregister_hash(&sha256_desc);
			break;
		case SEC_RSA_NO_PADDING:
			memcpy(pxOutput,tmpbuff,xKeySize);
			break;
		case SEC_RSA_OAEP_SHA_384_PADDING:
			register_hash(&sha384_desc);
			hash_idx = find_hash("sha384");
			ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);

			unregister_hash(&sha384_desc);
			break;
		case LAST_SEC_RSA_PADDING:
			return SEC_ERROR_BAD_PADDING ; 
			break;
		 } 
		 *pxOutputSize = tmplen;
		 if(ret!=CRYPT_OK) {
			 printf("Padding error!\n"); 
			 return SEC_ERROR;
		 }
	 
		 free(tmpbuff);

	 return SEC_NO_ERROR;
err:
	return SEC_ERROR; 
}

TSecStatus secRsaPrivateDecrypt
(
	const TUnsignedInt8*       pxInput,
 TUnsignedInt8*       pxOutput,
 size_t*              pxOutputSize,
   TUnsignedInt32        xE,
  const TUnsignedInt8*       pxN,
  const TUnsignedInt8*       pxP,
  const TUnsignedInt8*       pxQ,
  const TUnsignedInt8*       pxDP,
  const TUnsignedInt8*       pxDQ,
  const TUnsignedInt8*       pxQInv,
 size_t                xKeySize,
 TSecRsaPadding        xPadding
 ){
 
		int ret = 0;
		unsigned char *tmpbuff; 
		unsigned long tmplen;
		unsigned long mudulusbitlen = xKeySize*8;
		tmpbuff = malloc(xKeySize);
		int sign_valid = 0;
		int prng_idx; 
		int hash_idx;
		rsa_key rsaKey;
		
		if(pxInput == NULL || pxOutput == NULL){
			return SEC_ERROR_BAD_PARAMETER; 
		}
		if((pxP == NULL) || (pxQ == NULL)||
 		  (pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)){
 		  return SEC_ERROR_BAD_PARAMETER;
		  }
		if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}
		ltc_mp = ltm_desc;
	
		register_prng(&sprng_desc);
		
		prng_idx= find_prng("sprng");
		hash_idx = find_hash("sha1");
	
		ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
		if(ret) goto err;	 

		ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
		if(ret) goto err;	 
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.p,pxP,xKeySize/2);
		if(ret) goto err;	 
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.q,pxQ,xKeySize/2);
		if(ret) goto err;	 
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.dP,pxDP,xKeySize/2);
		if(ret) goto err;	 
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.dQ,pxDQ,xKeySize/2);
		if(ret) goto err;	 
		ret = mp_read_unsigned_bin((mp_int *)rsaKey.qP,pxQInv,xKeySize/2);
		if(ret) goto err;	 

		ret = ltc_mp.rsa_me(pxInput, xKeySize, tmpbuff, &tmplen, PK_PRIVATE, &rsaKey);
		if(ret != CRYPT_OK) return SEC_ERROR; 
		mudulusbitlen = mp_count_bits(rsaKey.N); 
		tmplen = xKeySize;
		switch(xPadding)
		{
	   case SEC_RSA_PKCS1_V1_5_PADDING : 
		   ret =   pkcs_1_v1_5_decode(tmpbuff,xKeySize,LTC_LTC_PKCS_1_EME,mudulusbitlen,pxOutput,&tmplen,&sign_valid);
		   
		   break;
	   case SEC_RSA_OAEP_SHA_1_PADDING :
		   register_hash(&sha1_desc);
		   hash_idx = find_hash("sha1");
		   ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
		   unregister_hash(&sha1_desc);
		   break;
	   case SEC_RSA_OAEP_SHA_256_PADDING:
		   register_hash(&sha256_desc);
		   hash_idx = find_hash("sha256");
		   ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
		   unregister_hash(&sha256_desc);
		   break;
	   case SEC_RSA_NO_PADDING:
		   memcpy(pxOutput,tmpbuff,xKeySize);
		   break;
	   case SEC_RSA_OAEP_SHA_384_PADDING:
		   register_hash(&sha384_desc);
		   hash_idx = find_hash("sha384");
		   ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
		   unregister_hash(&sha384_desc);
		   break;
	   case LAST_SEC_RSA_PADDING:
	       return SEC_ERROR_BAD_PADDING; 
		   break;
		} 
		*pxOutputSize = tmplen;
		if(ret!=CRYPT_OK) {
			printf("Padding error!\n"); 
			return SEC_ERROR;
		}
		free(tmpbuff);

	 return SEC_NO_ERROR;
err:
	return SEC_ERROR; 
}

TSecStatus secDhGenerateKey
(
 const TUnsignedInt8*   pxG,
 const TUnsignedInt8*   pxP,
 const TUnsignedInt8*   pxInputPrivKey,
 TUnsignedInt8*   pxOutputPrivKey,
 TUnsignedInt8*   pxPubKey,
 size_t            xKeySize
 ){
	 DH *dhKey = DH_new();
	 int ret=0;

	 if((pxP == NULL) || (pxG == NULL)||
		(pxOutputPrivKey== NULL) || (pxPubKey == NULL)){
			  return SEC_ERROR_BAD_PARAMETER;
		  }
	if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			printf("Invalid key size %d\n", xKeySize);
			return SEC_ERROR_BAD_PARAMETER;
		}
	 dhKey->g=BN_bin2bn(pxG, xKeySize, dhKey->g); 
	 dhKey->p=BN_bin2bn(pxP, xKeySize, dhKey->p); 
	 if(pxInputPrivKey)
		 dhKey->priv_key=BN_bin2bn(pxInputPrivKey, xKeySize, dhKey->priv_key); 

	 ret = DH_generate_key(dhKey);
	 if(ret!=1){
		 return SEC_ERROR; 
	 }
	 ret = BN_bn2bin(dhKey->priv_key, pxOutputPrivKey); 
	 if(ret != xKeySize){
		memmove(&pxOutputPrivKey[xKeySize-ret],&pxOutputPrivKey[0],ret);
		memset(&pxOutputPrivKey[0],0,xKeySize-ret);
	}
	ret =  BN_bn2bin(dhKey->pub_key, pxPubKey);
	if(ret != xKeySize){
		memmove(&pxPubKey[xKeySize-ret],&pxPubKey[0],ret);
		memset(&pxPubKey[0],0,xKeySize-ret);
	}


	 DH_free(dhKey);
	 return SEC_NO_ERROR; 
}


TSecStatus secDhComputeKey
(
 const TUnsignedInt8*   pxP,
 const TUnsignedInt8*   pxPrivKey,
 const TUnsignedInt8*   pxOtherPubKey,
 TUnsignedInt8*   pxSharedSecret,
 size_t            xKeySize
 ){
	 DH *dhKey = DH_new();
	 int ret=0;
	 BIGNUM *pub_key = BN_new(); 

	 if((pxP == NULL) ||
	 (pxPrivKey== NULL) ||(pxOtherPubKey== NULL) || (pxSharedSecret == NULL)){
			return SEC_ERROR_BAD_PARAMETER;
	 }
	 if((xKeySize%64)||(xKeySize<64)||(xKeySize>512)){
			 printf("Invalid key size %d\n", xKeySize);
			 return SEC_ERROR_BAD_PARAMETER;
		 }

	 dhKey->p=BN_bin2bn(pxP, xKeySize, dhKey->p); 
	 dhKey->priv_key = BN_bin2bn(pxPrivKey, xKeySize, dhKey->priv_key); 
	 pub_key = BN_bin2bn(pxOtherPubKey, xKeySize, pub_key);

	 ret = DH_compute_key(pxSharedSecret,pub_key, dhKey);
	 if(ret==0) return SEC_ERROR; 
	 DH_free(dhKey);
	 return SEC_NO_ERROR; 
}


unsigned int sec_P_getNidnumber(unsigned int keySize, unsigned char *q, unsigned char *a, unsigned char *b, unsigned char *GX, unsigned char *GY, unsigned char *n, unsigned char h){

	unsigned int nid = 0;

	if((q==NULL)||(a==NULL)||(n==NULL) ||(b==NULL)||(GX==NULL)||(GY==NULL) ){
		return nid; 
	}
	switch(keySize){
		case 21:
			if(h==1)
				if(!memcmp(q, Curve_Table160[0], keySize))
					if(!memcmp(n, Curve_Table160[1], keySize))
						if(!memcmp(a, Curve_Table160[2], keySize))
							if(!memcmp(b, Curve_Table160[3], keySize))
								if(!memcmp(GX, Curve_Table160[4], keySize))
									if(!memcmp(GY, Curve_Table160[5], keySize))nid = NID_secp160r1;

			break;
		case 28:
			if(h==1)
				if(!memcmp(q, Curve_Table224[0], keySize))
					if(!memcmp(n, Curve_Table224[1], keySize))
						if(!memcmp(a, Curve_Table224[2], keySize))
							if(!memcmp(b, Curve_Table224[3], keySize))
								if(!memcmp(GX, Curve_Table224[4], keySize))
									if(!memcmp(GY, Curve_Table224[5], keySize))nid = NID_secp224r1;

			break;
		case 32:
			if(h==1)
				if(!memcmp(q, Curve_Table256[0], keySize))
					if(!memcmp(n, Curve_Table256[1], keySize))
						if(!memcmp(a, Curve_Table256[2], keySize))
							if(!memcmp(b, Curve_Table256[3], keySize))
								if(!memcmp(GX, Curve_Table256[4], keySize))
									if(!memcmp(GY, Curve_Table256[5], keySize))nid = NID_X9_62_prime256v1;

			break;
		case 48:
			if(h==1)
				if(!memcmp(q, Curve_Table384[0], keySize))
					if(!memcmp(n, Curve_Table384[1], keySize))
						if(!memcmp(a, Curve_Table384[2], keySize))
							if(!memcmp(b, Curve_Table384[3], keySize))
								if(!memcmp(GX, Curve_Table384[4], keySize))
									if(!memcmp(GY, Curve_Table384[5], keySize))nid = NID_secp384r1;

			break;
		default :
			nid=0;
			break;

	}
	return nid;
}

TSecStatus secEcdhGenerateKey
(
 TSecEcParams     xParams,
 const TUnsignedInt8*  pxInputPrivKey,
 TUnsignedInt8*  pxOutputPrivKey,
 TUnsignedInt8*  pxPubKeyX,
 TUnsignedInt8*  pxPubKeyY
 ){
	 EC_GROUP *ec_group =NULL;
	 EC_KEY   *eckey = NULL;
	 EC_POINT *Pt;

	 BIGNUM *pvtkey = BN_new(); 
	 BIGNUM *opvtkey = BN_new(); 
	 BIGNUM *pubKeyX = BN_new(); 
	 BIGNUM *pubKeyY = BN_new();
	int i;
	 int ret;
	 unsigned int nid = 0;
	 if((pxOutputPrivKey == NULL) ||
	 	(pxPubKeyX == NULL) ||(pxPubKeyY == NULL)){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[0]);
#ifdef SEC_DEBUG
	 printf("nid = %d\n", nid); 
#endif
	if(!nid) return SEC_ERROR_BAD_PARAMETER; 
	 eckey = EC_KEY_new_by_curve_name(nid);
#if 1
	 ret= EC_KEY_generate_key(eckey); 
	 if(ret <0){
		 BDBG_ERR(("Error call EC_KEY_generate_key\n"));
		 return SEC_ERROR;
	 }
#endif

	 if(pxInputPrivKey) {
		 pvtkey=BN_bin2bn(pxInputPrivKey, xParams.keySize, pvtkey); 
		 ret = EC_KEY_set_private_key(eckey, pvtkey); 
		 if(ret <0){
			 BDBG_ERR(("Error call EC_KEY_set_private_key\n"));
			 return SEC_ERROR;
		 }
	 }

	 opvtkey = EC_KEY_get0_private_key(eckey);
	 ret = BN_bn2bin(opvtkey, pxOutputPrivKey); 
	 if(ret != xParams.keySize){
		 memmove(&pxOutputPrivKey[xParams.keySize-ret],&pxOutputPrivKey[0],ret);
		memset(&pxOutputPrivKey[0],0,xParams.keySize-ret);
	 }
	 ec_group = EC_KEY_get0_group(eckey);
	 Pt = EC_POINT_new(ec_group);
	 Pt = EC_KEY_get0_public_key(eckey);


	 if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
		 NID_X9_62_prime_field) {
			 if (!EC_POINT_get_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY,
				 NULL)) return SEC_ERROR; 

	 }else {
		 if (!EC_POINT_get_affine_coordinates_GF2m(ec_group, Pt, pubKeyX, pubKeyY,
			 NULL)) return SEC_ERROR;

	 } 
	
	 ret = BN_bn2bin(pubKeyX, pxPubKeyX);
	  if(ret != xParams.keySize){
		memmove(&pxPubKeyX[xParams.keySize-ret],&pxPubKeyX[0],ret);
		memset(&pxPubKeyX[0],0,xParams.keySize-ret);
	 }
	 ret = BN_bn2bin(pubKeyY, pxPubKeyY);
	 if(ret != xParams.keySize){
			memmove(&pxPubKeyY[xParams.keySize-ret],&pxPubKeyY[0],ret);
			memset(&pxPubKeyY[0],0,xParams.keySize-ret);
		 }

	 BN_free(pvtkey);
	 BN_free(opvtkey);
	 BN_free(pubKeyX);
	 BN_free(pubKeyY); 

	 EC_POINT_free(Pt); 

	 EC_GROUP_free(ec_group); 
	 return SEC_NO_ERROR;

}

TSecStatus secEcdhComputeKey
(

	TSecEcParams	   xParams,
	const TUnsignedInt8*  pxPrivKey,
	const TUnsignedInt8*  pxOtherPubKeyX,
	const TUnsignedInt8*  pxOtherPubKeyY,
	TUnsignedInt8*  pxSharedSecret

 ){
	 EC_GROUP *ec_group =NULL;
	 EC_KEY	 *eckey = NULL;
	 EC_POINT *Pt;
	 EC_POINT *shared_point; 

	 BIGNUM *pvtkey = BN_new(); 
	 BIGNUM *pubKeyX = BN_new(); 
	 BIGNUM *pubKeyY = BN_new();
	 BIGNUM *shared_point_x = BN_new();;
	 BIGNUM *shared_point_y = BN_new();
	 unsigned int nid;
	 int i;
	 int ret;

	 if((pxPrivKey == NULL) ||(pxSharedSecret == NULL) ||
	 	(pxOtherPubKeyX == NULL) ||(pxOtherPubKeyY == NULL)){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[0]);
#ifdef SEC_DEBUG
	 printf("nid = %d\n", nid); 
#endif
	if(!nid) return SEC_ERROR_BAD_PARAMETER;
	 eckey = EC_KEY_new_by_curve_name(nid);
	 printf("Privkey %x, %d, %x\n", pxPrivKey, xParams.keySize, eckey);
	 for(i=0;i<xParams.keySize; i++) printf("%02x ", pxPrivKey[i]);

	 pvtkey=BN_bin2bn(pxPrivKey, xParams.keySize, pvtkey); 
	 ret = EC_KEY_set_private_key(eckey, pvtkey); 
#if 1

	 if(ret <0){
		 BDBG_ERR(("Error call EC_KEY_set_private_key\n"));
		 return SEC_ERROR;
	 }
#ifdef SC_DEBUG
	 printf("\nPubkeyX\n");
	 for(i=0;i<xParams.keySize; i++) printf("%02x ", pxOtherPubKeyX[i]);
	 printf("\nPubkeyY\n");
	 for(i=0;i<xParams.keySize; i++) printf("%02x ", pxOtherPubKeyY[i]);
#endif
	 ec_group = EC_KEY_get0_group(eckey);
	 Pt = EC_POINT_new(ec_group);
	 pubKeyX = BN_bin2bn(pxOtherPubKeyX,xParams.keySize,pubKeyX);
	 pubKeyY= BN_bin2bn(pxOtherPubKeyY,xParams.keySize,pubKeyY);
	 if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
		 NID_X9_62_prime_field) {	
			 ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
			 if(ret <0){
				 BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
				 return SEC_ERROR;
			 }
	 }else{
		 ret = EC_POINT_set_affine_coordinates_GF2m(ec_group, Pt, pubKeyX, pubKeyY, NULL);
		 if(ret <0){
			 BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
			 return SEC_ERROR;
		 }
	 }
	 EC_KEY_set_public_key(eckey,Pt);

	 ret = EC_KEY_check_key(eckey);
	 /*if(ret){

	 BDBG_ERR(("Error call EC_KEY_check_key\n"));
	 return SEC_ERROR;
	 }
	 */
	 shared_point = EC_POINT_new(ec_group);

	 ret =EC_POINT_mul(ec_group,shared_point,NULL,Pt,pvtkey,NULL);
	 if(ret!=1){
		 printf("EC_POINT_mul falied\n");
		 return SEC_ERROR;
	 }	

	 #if 1
	 if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
		 NID_X9_62_prime_field) {
			 ret = EC_POINT_get_affine_coordinates_GFp(ec_group,shared_point,shared_point_x,shared_point_y,NULL);
	 }else{

		 ret = EC_POINT_get_affine_coordinates_GF2m(ec_group, shared_point, shared_point_x, shared_point_y, NULL);
	 }

	 if(ret <0){
		 BDBG_ERR(("Error call EC_POINT_get_affine_coordinates_GFp\n"));
		 return SEC_ERROR;
	 }
	 ret = BN_bn2bin(shared_point_x,pxSharedSecret);
	 if(ret != xParams.keySize){
			memmove(&pxSharedSecret[xParams.keySize-ret],&pxSharedSecret[0],ret);
			memset(&pxSharedSecret[0],0,xParams.keySize-ret);
		 }
	

	#else
	EC_POINT_point2bn(ec_group, shared_point,EC_GROUP_get_point_conversion_form(ec_group) ,shared_point_x, NULL); 
	BN_bn2bin(shared_point_x,pxSharedSecret);
	#endif
	 BN_free(pubKeyX); 

	 /*BN_free(pubKeyY); */

	 BN_free(pvtkey); 
	 BN_free(shared_point_x); 
	 BN_free(shared_point_y); 
	 EC_POINT_free(shared_point);
	 EC_POINT_free(Pt); 
	 EC_GROUP_free(ec_group); 
	 #endif
	 return SEC_NO_ERROR;


}
TSecStatus secEcdsaGenerateKey
(
 TSecEcParams     xParams,
 const TUnsignedInt8*  pxInputPrivKey,
 TUnsignedInt8*  pxOutputPrivKey,
 TUnsignedInt8*  pxPubKeyX,
 TUnsignedInt8*  pxPubKeyY
 ){

	 return secEcdhGenerateKey(xParams, pxInputPrivKey, pxOutputPrivKey, pxPubKeyX, pxPubKeyY); 
}

TSecStatus secEcdsaSign
(
	   TSecEcParams 	xParams,
	   TSecHashType 	xHashType,
 const TUnsignedInt8*  pxPrivKey,
 const TUnsignedInt8*  pxMessage,
	   size_t			xMessageSize,
	   TUnsignedInt8*  pxSigR,
	   TUnsignedInt8*  pxSigS

 ){
	 EC_KEY	 *eckey = EC_KEY_new();
	 BIGNUM *pvtkey = BN_new(); 
	 unsigned int nid;
	 unsigned char *digest;
	 unsigned int dgst_len;
	 unsigned int sig_len;
	 ECDSA_SIG *signature = NULL;
	 EVP_MD_CTX md_ctx;
	 const EC_GROUP *group;
	 int ret;
	int i;

     if((pxPrivKey == NULL) ||(pxMessage == NULL) ||(pxSigR == NULL) ||(pxSigS == NULL)){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[0]);
#ifdef SEC_DEBUG
	 printf("nid = %d\n", nid); 
#endif
	if(!nid) return SEC_ERROR_BAD_PARAMETER;
	 eckey = EC_KEY_new_by_curve_name(nid);
	 pvtkey=BN_bin2bn(pxPrivKey, xParams.keySize, pvtkey); 
	 group = EC_KEY_get0_group(eckey);
	 ret = EC_KEY_set_private_key(eckey, pvtkey); 
	 if(ret <0){
		 BDBG_ERR(("Error call EC_KEY_set_private_key\n"));
		 return SEC_ERROR;
	 }

	 ret =EC_KEY_check_key(eckey);

	 EVP_MD_CTX_init(&md_ctx);
	 if(xHashType == SEC_HASH_SHA1){

		 EVP_DigestInit(&md_ctx, EVP_sha1());

		 digest = malloc(SHA_DIGEST_LENGTH*sizeof(unsigned char));
	 }
	 else if(xHashType ==SEC_HASH_SHA256){

		 EVP_DigestInit(&md_ctx, EVP_sha256());
		 digest = malloc(SHA256_DIGEST_LENGTH*sizeof(unsigned char));
	 }
	 else{
		 return SEC_ERROR_BAD_PARAMETER;
	 }

	 EVP_DigestUpdate(&md_ctx, (const void*)pxMessage, xMessageSize);

	 EVP_DigestFinal(&md_ctx, digest, &dgst_len);

#if 0	
	 ret = ECDSA_sign(0, pxMessage, xMessageSize, 
		 pxSignature,&sig_len, eckey);
	 if(ret!=1){
		 BDBG_ERR(("Error calling ECDSA_sign()\n"));
		 return SEC_ERROR;
	 }
#endif
	 signature = ECDSA_do_sign(digest, dgst_len, eckey);	

	 ret = BN_bn2bin(signature->r,pxSigR);
	 if(ret != xParams.keySize){
		memmove(&pxSigR[xParams.keySize-ret],&pxSigR[0],ret);
		memset(&pxSigR[0],0,xParams.keySize-ret);
	}
	ret =  BN_bn2bin(signature->s,pxSigS);
	if(ret != xParams.keySize){
		memmove(&pxSigS[xParams.keySize-ret],&pxSigS[0],ret);
		memset(&pxSigS[0],0,xParams.keySize-ret);
	}
#if 0
	 p = EC_POINT_new(group);	

	 EC_POINT_set_affine_coordinates_GFp(group,p,signature->r,signature->s,NULL); 
	 EC_POINT_point2bn(group,p,EC_GROUP_get_point_conversion_form(group),bn_p,NULL); 
	 sig_len = BN_bn2bin(bn_p,pxSignature); 	

	 EC_POINT_free(p); 
#endif
	 EC_GROUP_free(group); 

	 return SEC_NO_ERROR; 
}

TSecStatus secEcdsaVerify
(
 	TSecEcParams	   xParams,
	TSecHashType	   xHashType,
	const TUnsignedInt8*  pxPubKeyX,
	const TUnsignedInt8*  pxPubKeyY,
	const TUnsignedInt8*  pxMessage,
	size_t		   xMessageSize,
	const TUnsignedInt8*  pxSigR,
	const TUnsignedInt8*  pxSigS

 ){
	 EC_GROUP *ec_group =NULL;
	 EC_KEY	 *eckey = NULL;
	 EC_POINT *Pt;
	 BIGNUM *pubKeyX= BN_new(); 
	 BIGNUM *pubKeyY= BN_new(); 
	 ECDSA_SIG *signature = NULL;
	 unsigned int nid;
	 unsigned char *digest;
	 unsigned int dgst_len;
	 EVP_MD_CTX md_ctx;
	 int ret;

	 if((pxPubKeyX == NULL) ||(pxPubKeyY == NULL) ||(pxMessage == NULL) ||
	 	(pxSigR == NULL) ||(pxSigS == NULL)){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 	
	 nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[0]);
#ifdef SEC_DEBUG
	 printf("nid = %d\n", nid); 
#endif
	if(!nid) return SEC_ERROR_BAD_PARAMETER; 
	
	
	 eckey = EC_KEY_new_by_curve_name(nid);
	 ec_group = EC_KEY_get0_group(eckey);
	 Pt = EC_POINT_new(ec_group);
	 pubKeyX=BN_bin2bn(pxPubKeyX, xParams.keySize, pubKeyX); 
	 pubKeyY=BN_bin2bn(pxPubKeyY, xParams.keySize, pubKeyY); 
	 if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
		 NID_X9_62_prime_field) {	
			 ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
			 if(ret <0){
				 BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
				 goto err;
			 }
	 }else{
		 ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
		 if(ret <0){
			 BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
			 goto err;
		 }
	 }
	 ret = EC_KEY_set_public_key(eckey,Pt);
	 if(ret==0) goto err;
	 ret = EC_KEY_check_key(eckey);
	if(ret==0) goto err;

	 EVP_MD_CTX_init(&md_ctx);
	 /* get the message digest */
	 if(xHashType == SEC_HASH_SHA1){
		 EVP_DigestInit(&md_ctx, EVP_sha1());
		 digest = malloc(SHA_DIGEST_LENGTH*sizeof(unsigned char));
	 }
	 else if(xHashType ==SEC_HASH_SHA256){
		 EVP_DigestInit(&md_ctx, EVP_sha256());
		 digest = malloc(SHA256_DIGEST_LENGTH*sizeof(unsigned char));
	 }
	 else{
		 return SEC_ERROR_BAD_PARAMETER;
	 }
	 
	 ret = EVP_DigestUpdate(&md_ctx, (const void*)pxMessage, xMessageSize);
	 if(ret==0) goto err;
	 ret = EVP_DigestFinal(&md_ctx, digest, &dgst_len);
     if(ret==0) goto err;

	 signature = ECDSA_SIG_new();

	 
	 signature->r = BN_bin2bn(pxSigR,xParams.keySize,signature->r); 
	 signature->s = BN_bin2bn(pxSigS,xParams.keySize,signature->s ); 

	 ret =ECDSA_do_verify(digest,dgst_len,signature,eckey);
	 if(ret !=1){
		 printf("ECDSA SIGN FAIL\n");

		 return SEC_ERROR_BAD_SIGNATURE;
	 }
	 printf("ECDSA SIGN OK\n");
	return SEC_NO_ERROR; 
err:
	return SEC_ERROR;
}

TSecStatus secGenerateRandomBytes
(
 size_t           xNumOfBytes,
 TUnsignedInt8*  pxRandomBytes
 ){
	 NEXUS_RandomNumberGenerateSettings settings;
	 NEXUS_RandomNumberOutput output;

	 if(pxRandomBytes == NULL){
		return SEC_ERROR_BAD_PARAMETER;
	 }
	 if(xNumOfBytes >352){
		return SEC_ERROR_BAD_PARAMETER; 
	 }
	 NEXUS_RandomNumber_GetDefaultGenerateSettings(&settings); 
	 settings.randomNumberSize =( xNumOfBytes/4)*4;
	 if(xNumOfBytes%4)settings.randomNumberSize +=4;
	 if(NEXUS_RandomNumber_Generate(&settings, &output)){
		 return SEC_ERROR;
	 }
	 BKNI_Memcpy(pxRandomBytes, output.buffer, xNumOfBytes); 
	 return SEC_NO_ERROR;
}
TUnsignedInt32		secApiVersion(){
	return SECAPI_VERSION_INT;
}

TSecFunctionTable* secGetFunctionTable
(
 void
 ){
	 TSecFunctionTable *functionTable;

	 NEXUS_Memory_Allocate(sizeof(TSecFunctionTable), NULL, (void *)&functionTable);

	 if(functionTable == NULL){
		return SEC_ERROR;
	 }
	 BKNI_Memset(functionTable, 0, sizeof(TSecFunctionTable));
	 functionTable->secApiVersion = secApiVersion;
	 functionTable->secGetNuid64 = secGetNuid64;
	 functionTable->secGetNuid = secGetNuid;
	 functionTable->secGetChipsetRevision = secGetChipsetRevision;
	 functionTable->secGetChipsetExtension = secGetChipsetExtension;
	 functionTable->secOpenRam2RamDecryptSession = secOpenRam2RamDecryptSession;
	 functionTable->secOpenRam2RamEncryptSession = secOpenRam2RamEncryptSession;
	 functionTable->secCloseSession				= secCloseSession;
	 functionTable->secSet2LevelProtectedKey		= secSet2LevelProtectedKey;
	 functionTable->secSetClearTextKey			= secSetClearTextKey;
	 functionTable->secEncryptData 				= secEncryptData;
	 functionTable->secDecryptData				= secDecryptData;
	 functionTable->secSessionDecrypt 			= secSessionDecrypt;
	 functionTable->secSessionEncrypt			= secSessionEncrypt;
	 functionTable->secRsaGenerateKey = secRsaGenerateKey; 
	 functionTable->secRsaComputeCrtParams = secRsaComputeCrtParams;
	 functionTable->secRsaPrivateDecrypt = secRsaPrivateDecrypt;
	 functionTable->secRsaPrivateEncrypt = secRsaPrivateEncrypt;
	 functionTable->secRsaPublicDecrypt = secRsaPublicDecrypt;
	 functionTable->secRsaPublicEncrypt = secRsaPublicEncrypt;
	 functionTable->secDhGenerateKey = secDhGenerateKey;
	 functionTable->secDhComputeKey = secDhComputeKey;
	 functionTable->secEcdhGenerateKey = secEcdhGenerateKey;
	 functionTable->secEcdhComputeKey  = secEcdhComputeKey;
	 functionTable->secEcdsaGenerateKey = secEcdsaGenerateKey;
	 functionTable->secEcdsaSign = secEcdsaSign;
	 functionTable->secEcdsaVerify = secEcdsaVerify;
	 functionTable->secGenerateRandomBytes = secGenerateRandomBytes;
	 functionTable->secSha1Init		= secSha1Init;
	 functionTable->secSha1Update    = secSha1Update;
	 functionTable->secSha1Final 	= secSha1Final; 
	 functionTable->secSha256Init		= secSha256Init;
	 functionTable->secSha256Update    = secSha256Update;
	 functionTable->secSha256Final 	= secSha256Final;
	 functionTable->secSha384Init		= secSha384Init;
	 functionTable->secSha384Update    = secSha384Update;
	 functionTable->secSha384Final 	= secSha384Final;
	 functionTable->secHmacSha256Final = secHmacSha256Final;
	 functionTable->secHmacSha256Init = secHmacSha256Init;
	 functionTable->secHmacSha256Update = secHmacSha256Update;
     functionTable->secUseCertKey = secUseCertKey;
	 functionTable->secUseFlashProtKey = secUseFlashProtKey;
	 functionTable->secEncryptFlashProtKey = secEncryptFlashProtKey;
/*
	
*/
	 return functionTable;
}


