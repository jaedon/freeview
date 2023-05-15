/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_register_protection.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/27/12 2:08p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/extensions/video_decoder/security/40nm/src/nexus_video_decoder_register_protection.c $
 * 
 * 5   8/27/12 2:08p erickson
 * SW7435-235: refactor directories to remove symbolic links
 * 
 * 4   7/20/11 3:21p yili
 * SW7422-404:Code resturcture
 * 
 * 3   6/29/11 5:10p yili
 * SW7422-404: Clean up
 * 
 * 2   4/20/11 1:48p yili
 * SW7422-404:More test cases
 * 
 * 1   3/23/11 11:20a yili
 * SW7422-404:Video decoder security
 * 
 *****************************************************************************/

#include "nexus_video_decoder_module.h"
#include "nexus_video_decoder_register_protection.h"
#include "nexus_video_decoder_security.h"
#include "nexus_bsp_config.h"
#include "nexus_keyladder.h"

#if BCHP_CHIP == 7231
#include "nexus_video_decoder_register_protection_settings_7231.c"
#else
#include "nexus_video_decoder_register_protection_settings.c"
#endif
#if BCHP_CHIP == 7231
#include "nexus_video_decoder_register_protection_signatures_7231.c"
#elif BCHP_CHIP == 7346
#include "nexus_video_decoder_register_protection_signatures_7346.c"
#elif BCHP_CHIP == 7358
#include "nexus_video_decoder_register_protection_signatures_7358.c"
#else
#include "nexus_video_decoder_register_protection_signatures.c"
#endif

#if (NEXUS_NUM_XVD_DEVICES)

BDBG_MODULE(nexus_decoder_security);
BDBG_OBJECT_ID(NEXUS_VideoDecoderSecurityRegisterProtection);

BERR_Code GenerateKey3ForSignatureVerification (void)
{
	BERR_Code rc = NEXUS_SUCCESS;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	static const unsigned char ucProcInForKey3[16] = 
	{
		0x11, 0x11, 0x22, 0x22, 0x12, 0x34, 0x56, 0x78, 
		0x98, 0x76, 0x54, 0x32, 0x09, 0x23, 0x45, 0x56
	};
	NEXUS_KeySlotHandle keyHandle = NULL;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings AlgConfig;



    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	/* Allocate AV keyslots */
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !keyHandle)
	{
		rc = 1;
		goto error;
	}

    NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;

	/* ++++++++ */
	AlgConfig.terminationMode     = NEXUS_SecurityTerminationMode_eClear;
	AlgConfig.ivMode              = NEXUS_SecurityIVMode_eRegular;
	AlgConfig.solitarySelect      = NEXUS_SecuritySolitarySelect_eClear;
	AlgConfig.caVendorID          = 0x1234;
	AlgConfig.askmModuleID        = NEXUS_SecurityAskmModuleID_eModuleID_3;
	AlgConfig.otpId               = NEXUS_SecurityOtpId_eOtpVal;
	AlgConfig.testKey2Select      = 2;
	/* ++++++++ */

	if ( NEXUS_Security_ConfigAlgorithm (keyHandle, &AlgConfig) != 0 )
	{
		return 1;
	}	
	
	BKNI_Memset (&encryptedSessionkey, 0, sizeof(NEXUS_SecurityEncryptedSessionKey));
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eB;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x0a; 
	encryptedSessionkey.cusKeyH 		= 0x0a; 
	encryptedSessionkey.cusKeyVarL 		= 0x27; 
	encryptedSessionkey.cusKeyVarH 		= 0x27; 
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;

	/* ++++++++ */
	encryptedSessionkey.custSubMode        = NEXUS_SecurityCustomerSubMode_eGeneralPurpose1;
	encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL7;
	encryptedSessionkey.keyMode            = NEXUS_SecurityKeyMode_eRegular;
	/* ++++++++ */

	/*encryptedSessionkey.pKeyData 		= ucProcInForKey3;*/
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	rc=NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey);

error:
	NEXUS_Security_FreeKeySlot (keyHandle);
	return rc;
}

BERR_Code NEXUS_VideoDecoder_P_EnableRegisterProtection (
    const NEXUS_VideoDecoderRegisterProtectionConfig *pConfig
    )
{
	unsigned int index, i;
	BERR_Code rc = NEXUS_SUCCESS;
	NEXUS_SecurityAVDSRegRangeSettings	avdRegRangeSettings;
	AVDRegRegion * pRegInfo;

	BDBG_ENTER(NEXUS_VideoDecoder_P_EnableRegisterProtection);
	BDBG_ASSERT(pConfig);

	index = pConfig->index;
	BDBG_ASSERT(index<=NEXUS_NUM_XVD_DEVICES);

	if ( (rc = GenerateKey3ForSignatureVerification ()) != NEXUS_SUCCESS )
	{
		BDBG_WRN(("Cannot generate key3c"));
		return BERR_TRACE(rc);
	}

	avdRegRangeSettings.vkl = NEXUS_SecurityVirtualKeyladderID_eVKL7;
	avdRegRangeSettings.keyLayer = NEXUS_SecurityKeySource_eKey3;
	if ( (index==0) )
	{
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_SVD
		avdRegRangeSettings.VDECId = NEXUS_SecurityAVDType_eSVD;
#else
		avdRegRangeSettings.VDECId = NEXUS_SecurityAVDType_eAVD;
#endif
		avdRegRangeSettings.nRange = sizeof(gAVD0_Regions)/sizeof(AVDRegRegion);
		pRegInfo = (AVDRegRegion*)gAVD0_Regions;
	}
#if (NEXUS_NUM_XVD_DEVICES>1)
	else if (index==1)
	{
		avdRegRangeSettings.VDECId = NEXUS_SecurityAVDType_eAVD;
		avdRegRangeSettings.nRange = sizeof(gAVD1_Regions)/sizeof(AVDRegRegion);
		pRegInfo = (AVDRegRegion*)gAVD1_Regions;
	}
	else
	{
		/* We should never reach here.  Just added to get ride of compiler warning */
		return BERR_TRACE(1);
	}
#endif

	BDBG_ASSERT(avdRegRangeSettings.nRange<=NEXUS_MAX_AVDSVD_RANGE);

	for ( i=0; i< avdRegRangeSettings.nRange; i++ )
	{
		avdRegRangeSettings.loRange[i] = pRegInfo[i].startPhysicalAddress;
		avdRegRangeSettings.hiRange[i] = pRegInfo[i].endPhysicalAddress;
	}

	for (i=0;i<NEXUS_HMACSHA256_SIGNATURE_SIZE;i++)
	{
		avdRegRangeSettings.signature[i] = gAVD_SetVichSignature[index][i];
	}
	if ( (rc = NEXUS_Security_AVDSRegistersSetUp(&avdRegRangeSettings))!= NEXUS_SUCCESS )
	{
		BDBG_WRN(("Cannot SetVICH %08x\n", rc));
		return BERR_TRACE(rc);
	} 

	return rc;
}

BERR_Code NEXUS_VideoDecoder_P_AVDResetCallback (void * pPrivateData)
{
    NEXUS_VideoDecoderSecureBootContext * pContext = (NEXUS_VideoDecoderSecureBootContext *)pPrivateData;
	BREG_Handle hReg = pContext->hReg;
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ASSERT(pPrivateData);

	BDBG_ENTER(AVDResetCallback);
	BDBG_MSG(("\n$$$$$$$$ DeviceID is %d $$$$$$$$\n", pContext->deviceID));

	if ( pContext->deviceID == 0 )
	{
		NEXUS_VideoDecoderRegisterProtectionConfig config;
		config.index = pContext->deviceID;
		rc = NEXUS_VideoDecoder_P_SecureReset (&config);
		if ( rc != BERR_SUCCESS) 		
		{
			BDBG_ERR(("##### Cannot secure reset AVD0 #####\n"));
			/* Stop IL AVD ARC */
			BREG_Write32(hReg, 
						BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,
						1);
			BREG_Write32(hReg, 
						BCHP_DECODE_CPUAUX_0_CPUAUX_REG+0x28,
						1);

			/* Stop OL AVD ARC */
			BREG_Write32(hReg, 
						BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,
						1);

			BREG_Write32(hReg, 
						BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x28,
						1);

			/* Stop BL SVD ARC */
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_BLD
			BREG_Write32(hReg, 
						BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,
						1);
			BREG_Write32(hReg, 
							BCHP_BLD_DECODE_CPUAUX_0_CPUAUX_REG+0x28,
							1); 
#endif
		}

    }

#if (NEXUS_NUM_XVD_DEVICES==2)
	else if ( pContext->deviceID == 1 )
	{
		NEXUS_VideoDecoderRegisterProtectionConfig config;
		config.index = pContext->deviceID;
		rc = NEXUS_VideoDecoder_P_SecureReset (&config);
		if ( rc != BERR_SUCCESS) 
		{
			BDBG_ERR(("##### Cannot secure reset AVD1 #####\n"));
			/* Stop IL AVD ARC */
			BREG_Write32(hReg, 
						BCHP_DECODE_IND_SDRAM_REGS_1_REG_CPU_DBG,
						1);
			BREG_Write32(hReg, 
						BCHP_DECODE_CPUAUX_1_CPUAUX_REG+0x28,
						1);
		}
		/* Stop OL AVD ARC */
		BREG_Write32(hReg, 
					BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,
					1);

		BREG_Write32(hReg, 
					BCHP_DECODE_CPUAUX2_1_CPUAUX_REG+0x28,
					1);

	}
#endif


	BDBG_LEAVE(AVDResetCallback);
	return BERR_SUCCESS;
}


static BERR_Code NEXUS_VideoDecoder_P_SecureModify(
    const NEXUS_VideoDecoderRegisterProtectionConfig *pConfig, 
	bool bReset
    )
{
	unsigned int index, i;
	BERR_Code rc = NEXUS_SUCCESS;
	NEXUS_SecurityAVDSRegModifySettings avdModifySettings;
	AVDRegAddrValue * pRegAddrValue;
	unsigned char * pSignature;

	BDBG_ENTER(NEXUS_VideoDecoder_P_SecureStart);
	BDBG_ASSERT(pConfig);

	index = pConfig->index;
	BDBG_ASSERT(index<=NEXUS_NUM_XVD_DEVICES);

	if ( (rc = GenerateKey3ForSignatureVerification ()) != NEXUS_SUCCESS )
	{
		BDBG_WRN(("Cannot generate key3c"));
		return BERR_TRACE(rc);
	}

	avdModifySettings.vkl = NEXUS_SecurityVirtualKeyladderID_eVKL7;
	avdModifySettings.keyLayer = NEXUS_SecurityKeySource_eKey3;

#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_SVD
	avdModifySettings.avdID = (index==0)? NEXUS_SecurityAVDType_eSVD:NEXUS_SecurityAVDType_eAVD;
#else
	avdModifySettings.avdID = (index==0)? NEXUS_SecurityAVDType_eAVD:NEXUS_SecurityAVDType_eAVD;
#endif

	if ( bReset )
	{
		pRegAddrValue = (AVDRegAddrValue *)&gResetAVDRegAddrValues[index][0];
		pSignature = (unsigned char *)&gAVD_ResetAvdSignature[index][0];
		avdModifySettings.nAVDReg = (index==0)? NEXUS_VIDEO_REG_PROTECTION_NUM_RESET_AVD0_REGISTERS:\
						NEXUS_VIDEO_REG_PROTECTION_NUM_RESET_AVD1_REGISTERS;
	}
	else
	{
		pRegAddrValue = (AVDRegAddrValue *)&gStartAVDRegAddrValues[index][0];
		pSignature = (unsigned char *)&gAVD_StartAvdSignature[index][0];
		avdModifySettings.nAVDReg = NEXUS_VIDEO_REG_PROTECTION_NUM_START_AVD_REGISTERS;
	}

	for ( i=0; i<avdModifySettings.nAVDReg; i++ )
	{
		avdModifySettings.regAddr[i] = pRegAddrValue[i].regPhysicalAddress;
		avdModifySettings.regVal[i] = pRegAddrValue[i].regValue;
	}

	for (i=0; i<NEXUS_HMACSHA256_SIGNATURE_SIZE; i++ )
	{
		avdModifySettings.signature[i] = pSignature[i];
	}

#ifdef NEXUS_ENABLE_VICH_0_NEGATIVE_BAD_SIGNATURE
	if ( (index==0 ) && (!bReset))
		avdModifySettings.signature[0] = ~avdModifySettings.signature[0];
#endif
#ifdef NEXUS_ENABLE_VICH_1_NEGATIVE_BAD_SIGNATURE
	if ( (index==1 ) && (!bReset))
		avdModifySettings.signature[0] = ~avdModifySettings.signature[0];
#endif

	if ( (rc = NEXUS_Security_AVDSRegistersModify(&avdModifySettings))!= NEXUS_SUCCESS )
	{
		BDBG_WRN(("Cannot StartAVD"));
		return BERR_TRACE(rc);
	} 
	return rc;
}


BERR_Code NEXUS_VideoDecoder_P_SecureStart(
    const NEXUS_VideoDecoderRegisterProtectionConfig *pConfig
    )
{

	return NEXUS_VideoDecoder_P_SecureModify (pConfig, false);
}

BERR_Code NEXUS_VideoDecoder_P_SecureReset(
    const NEXUS_VideoDecoderRegisterProtectionConfig *pConfig
    )
{
	return NEXUS_VideoDecoder_P_SecureModify (pConfig, true);

}

#endif
