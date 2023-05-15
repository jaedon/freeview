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
 * $brcm_Workfile: nexus_video_decoder_security.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/20/12 2:34p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/extensions/video_decoder/security/7422/src/nexus_video_decoder_security.c $
 * 
 * 3   6/20/12 2:34p yili
 * SW7346-555:Clean up
 * 
 * 2   4/20/11 1:46p yili
 * SW7422-404:More test cases
 * 
 * 1   3/23/11 11:20a yili
 * SW7422-404:Video decoder security
 * 
 *****************************************************************************/

#include "nexus_video_decoder_module.h"
#include "nexus_video_decoder_security.h"

#include "bxvd.h"           /* xvd interface */
#include "bxvd_image.h"
#ifdef NEXUS_ENABLE_VICH
#include "nexus_video_decoder_register_protection.h"
#endif
#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
#include "nexus_video_decoder_region_verification.h"
#endif
#ifdef NEXUS_VIDEO_DECODER_DUMP_FW_FILE
#include <stdio.h>
#endif

#if	(NEXUS_NUM_XVD_DEVICES)

BDBG_MODULE(nexus_decoder_security);
BDBG_OBJECT_ID(NEXUS_VideoDecoderSecurity);


static NEXUS_VideoDecoderSecureBootContext gBootContext[NEXUS_NUM_XVD_DEVICES];



static BERR_Code AVDBoot(void *pPrivateData, BAFL_BootInfo *pstAVDBootInfo)
{
    NEXUS_VideoDecoderSecureBootContext * pContext = (NEXUS_VideoDecoderSecureBootContext *)pPrivateData;
	BREG_Handle hReg = pContext->hReg;
#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
	BAFL_FirmwareInfo *pstArc;
#endif
	BERR_Code rc;

	BDBG_ASSERT(pPrivateData);

	BDBG_ENTER(SVP_AVDBoot_740x);
	BDBG_MSG(("\n$$$$$$$$ DeviceID is %d $$$$$$$$\n", pContext->deviceID));

#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
	BDBG_ASSERT(pstAVDBootInfo);

	pstArc = pstAVDBootInfo->pstArc;
	rc = Nexus_VideoDecoder_P_EnableRegions (pstArc, pContext->deviceID);
	if ( rc != BERR_SUCCESS) 
	{
		return BERR_TRACE(rc);
	}		
#endif

	switch(pstAVDBootInfo->eMode) 
	{
		case BAFL_BootMode_eNormal:
			BDBG_MSG(("Boot Mode: Normal (%d)", pstAVDBootInfo->eMode));
			break;

		case BAFL_BootMode_eWatchdog:
			BDBG_MSG(("Boot Mode: Watchdog (%d)", pstAVDBootInfo->eMode));
			break;

		default:
			BDBG_MSG(("Boot Mode: Unknown (%d)", pstAVDBootInfo->eMode));
	}

	if ( pContext->deviceID == 0 )
	{
#ifdef NEXUS_ENABLE_VICH
		NEXUS_VideoDecoderRegisterProtectionConfig config;
		config.index = pContext->deviceID;
		BDBG_MSG(("Enable video register protection\n"));

		rc = NEXUS_VideoDecoder_P_EnableRegisterProtection (&config);
		if ( rc != BERR_SUCCESS) 
		{
			BDBG_WRN(("AVD0 register protection might have been enabled.  \n"));
		}		
#ifndef NEXUS_ENABLE_VICH_0_NEGATIVE_HOST_WRITE
		rc = NEXUS_VideoDecoder_P_SecureStart (&config);
		if ( rc != BERR_SUCCESS) 		
#else
		BREG_Write32(hReg,
				BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,
				0);

#endif
		{
			BDBG_ERR(("Cannot Start AVD0\n"));
#endif
#ifndef NEXUS_ENABLE_VICH_0_NEGATIVE_BAD_SIGNATURE
		BREG_Write32(hReg, 
				BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,
				1);
		
		BREG_Write32(hReg, 
				(BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x18),
				0); 
		
		BREG_Write32(hReg,
				(BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x28),
				0);
#endif
#ifdef NEXUS_ENABLE_VICH
		}
#endif
	}

#if (NEXUS_NUM_XVD_DEVICES==2)
	else if ( pContext->deviceID == 1 )
	{
#ifdef NEXUS_ENABLE_VICH
		NEXUS_VideoDecoderRegisterProtectionConfig config;
		config.index = pContext->deviceID;
		BDBG_MSG(("Enable video register protection\n"));

		rc = NEXUS_VideoDecoder_P_EnableRegisterProtection (&config);
		if ( rc != BERR_SUCCESS) 
		{
			BDBG_WRN(("AVD1 register protection might have been enabled.  \n"));
		}		
#ifndef NEXUS_ENABLE_VICH_1_NEGATIVE_HOST_WRITE
		rc = NEXUS_VideoDecoder_P_SecureStart (&config);
		if ( rc != BERR_SUCCESS) 
#else
		BREG_Write32(hReg,
				BCHP_DECODE_IND_SDRAM_REGS_1_REG_CPU_DBG,
				0);
#endif
		{
			BDBG_ERR(("Cannot Start AVD1\n"));
#endif
#ifndef	NEXUS_ENABLE_VICH_1_NEGATIVE_BAD_SIGNATURE
		BREG_Write32(hReg, 
				BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,
				1);
		
		BREG_Write32(hReg, 
				(BCHP_DECODE_CPUAUX2_1_CPUAUX_REG+0x18),
				0); 
		BDBG_MSG(("Taking AVD core out of debug mode"));
		BREG_Write32(hReg,
				(BCHP_DECODE_CPUAUX2_1_CPUAUX_REG+0x28),
				0);
#endif
#ifdef NEXUS_ENABLE_VICH
		}
#endif
	}
#endif


	BDBG_LEAVE(SVP_AVDBoot_740x);
	return BERR_SUCCESS;
}


void NEXUS_VideoDecoder_P_EnableSecurity (
	BXVD_Settings * inoutp_Settings, 
    const NEXUS_VideoDecoderSecureConfig * inp_Config
)
{
	int index;

	BDBG_ASSERT ( inoutp_Settings != NULL && inp_Config != NULL );
	BDBG_ASSERT ( inp_Config->hReg != NULL  );

	index = inp_Config->index;
	BDBG_ASSERT(index==0 || index==1);


	gBootContext[index].hReg = inp_Config->hReg;
	gBootContext[index].deviceID = index;
	inoutp_Settings->pAVDBootCallback = AVDBoot;
	inoutp_Settings->pAVDBootCallbackData = &gBootContext[index];
#ifdef NEXUS_ENABLE_VICH
	inoutp_Settings->pAVDResetCallback = NEXUS_VideoDecoder_P_AVDResetCallback;
	inoutp_Settings->pAVDResetCallbackData = &gBootContext[index];
#endif

#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
	Nexus_VideoDecoder_P_DisableRegionVerification (index);
#endif

#ifdef NEXUS_VIDEO_DECODER_NO_SECURE_HEAP
	inoutp_Settings->hSecureMemory=NULL;
#endif
}

#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
void NEXUS_VideoDecoder_P_DisableFwVerification (unsigned int index)
{
    BDBG_ASSERT(index==0 || index==1);
	Nexus_VideoDecoder_P_DisableRegionVerification (index);

    BKNI_Memset (&gBootContext[index], 0, sizeof (NEXUS_VideoDecoderSecureBootContext));
    BDBG_MSG(("Done NEXUS_VideoDecoder_P_CleanupFwVerification\n"));

}

#endif

#endif
