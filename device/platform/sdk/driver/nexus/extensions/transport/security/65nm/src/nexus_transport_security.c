/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_transport_security.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/29/11 3:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/transport/security/7125/src/nexus_transport_security.c $
 * 
 * 2   7/29/11 3:28p yili
 * SW7125-136:Add FW dump
 * 
 * 1   7/20/11 11:46a yili
 * SW7125-136:Relocate security
 * 
 * 5   10/28/09 10:14a erickson
 * SW7340-42: add BCHP_CHIP, cleanup
 *
 * 4   3/20/09 11:10a yili
 * PR42660:IPTV integration
 *
 * 3   3/17/09 11:21a yili
 * PR52990:Add region verification
 *
 * 2   3/16/09 11:47a yili
 * PR52990:Add region verificaiton - Minor change to meet Nexus coding
 *  standard.
 *
 * 1   3/9/09 4:44p yili
 * PR52990:Add region verification
 *
 * wince_7400/3   12/19/08 1:34p yili
 * PR42660:IPTV
 *
 * wince_7400/2   12/16/08 12:35p yili
 * PR42660:IPTV Integration
 *
 * wince_7400/1   12/15/08 6:04p yili
 * PR42660:IPTV Integration
 *
 *****************************************************************************/
#include "nexus_transport_module.h"
#include "nexus_transport_security.h"
#include "nexus_security_regver.h"
#include "nexus_memory.h"
#include "bxpt_rave.h"
#include "nexus_security_rawcommand.h"
#include "nexus_key2_array.c"
#ifdef NEXUS_TRANSPORT_SECURITY_DUMP_RAVE_FILE
#include "bxpt_rave_ihex.h"
#include <stdio.h>
#endif

BDBG_MODULE(nexus_transport_module);


#define RAVE_REGION_ID 11

#define RAVE_FW_SIZE 8192

static BERR_Code rave_verification(void);

#ifdef NEXUS_TRANSPORT_SECURITY_DUMP_RAVE_FILE
static void Dump_RaveFW (void)
{
	FILE * pOut;
	unsigned char dIn [4];
	int i;
	int total = 188*1024*256;

	pOut = fopen ("rave.bin", "wb");
	for ( i=0; i<BxptRaveInitDataSize; i++ )
	{
		unsigned int tmp = BxptRaveInitData [i];
		/* big endian */
		dIn[0] = 0;/*tmp >> 24;*/
		dIn[1] = (tmp>>16) & 0x3F;
		dIn[2] = (tmp>>8) & 0xFF;
		dIn[3] = (tmp) & 0xFF;

		fwrite (dIn, 1, 4, pOut);
	}
	fclose (pOut);	
}
#endif

void NEXUS_Rave_P_EnableFwVerification (BXPT_Rave_ChannelSettings * pRaveSettings)
{
    if (pRaveSettings)
    {
        pRaveSettings->chanOpenCB = rave_verification;
    }
#ifdef NEXUS_TRANSPORT_SECURITY_DUMP_RAVE_FILE
	Dump_RaveFW();
#endif
    NEXUS_Security_DisableRegion(RAVE_REGION_ID);
}

static BERR_Code load_key2(void);
static BERR_Code rave_verification(void)
{
    NEXUS_Error errCode;
    NEXUS_SecurityCodeRegionSettings RegionSetting;
    unsigned int RegionID = RAVE_REGION_ID;
    unsigned char *pAddr = NULL;
    NEXUS_MemoryAllocationSettings memSetting;

	load_key2();
    NEXUS_Memory_GetDefaultAllocationSettings(&memSetting);
    memSetting.alignment = 32;
    if ( ( NEXUS_Memory_Allocate(256, &memSetting, (void **)&pAddr)!=0 ) || (pAddr == NULL) )
    {
        BDBG_ERR(("Cannot define region for rave FW"));
        return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
    }

    BKNI_Memcpy(pAddr, gRaveFirmwareSignature, 256);

    NEXUS_FlushCache((const void*)pAddr, 256);

    NEXUS_Security_GetDefaultRegionSettings(&RegionSetting);
    RegionSetting.regionID = RegionID;
    RegionSetting.codeRule = 0x00000300;
    RegionSetting.codeSize = RAVE_FW_SIZE;
    RegionSetting.keyID = 2;
    RegionSetting.sigSize = NEXUS_SecurityRegverSignatureSize_e256Bytes;
    RegionSetting.codeStartPhyAddress = 0;
    RegionSetting.sigStartPhyAddress = NEXUS_AddrToOffset((void *)pAddr);
    errCode = NEXUS_Security_VerifyRegion(&RegionSetting, &RegionID);
    NEXUS_Memory_Free(pAddr);
    if (errCode) {
        BDBG_ERR(("NEXUS_Security_VerifyRegion failed"));
        return BERR_TRACE(errCode);
    }

    return errCode;
}

static BERR_Code load_key2(void)
{
    NEXUS_Error errCode;
    unsigned char *pAddr = NULL;
    NEXUS_MemoryAllocationSettings memSetting;
	unsigned int inBuffer[16], outBuffer[16], actualOutSize;

    NEXUS_Memory_GetDefaultAllocationSettings(&memSetting);
    memSetting.alignment = 32;
    if ( ( NEXUS_Memory_Allocate(sizeof(gKey2), &memSetting, (void **)&pAddr)!=0 ) || (pAddr == NULL) )
    {
        BDBG_ERR(("Cannot define region for rave FW"));
        return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
    }

    BKNI_Memcpy(pAddr, gKey2, sizeof(gKey2));

    NEXUS_FlushCache((const void*)pAddr, sizeof(gKey2));

	inBuffer[0] = 0x00000010;
	inBuffer[1] = 0x00000022; 
	inBuffer[2] = 0xabcdef00; 
	inBuffer[3] = 0xc955aa36; 
	inBuffer[4] = 0x789a000c; 
	inBuffer[5] = 0x00000002; 
	inBuffer[6] = 0x00000000; 
	inBuffer[7] = NEXUS_AddrToOffset((void *)pAddr); 


    errCode = NEXUS_Security_SubmitRawCommand (
			inBuffer,    /* attr{nelem=sizeInBuffer;reserved=90} */
			8,        /* size of pInBuffer in integers */
			outBuffer,         /* [out] attr{nelem=sizeOutBuffer;reserved=90} */
			6,        /* size of pOutBuffer in integers */
			&actualOutSize      /* [out] number of integers written into pOutBuffer */
    );
    NEXUS_Memory_Free(pAddr);
    if ( errCode || ((outBuffer[5]!=0) && (outBuffer[5]!=0xE9))) {
        BDBG_ERR(("Key2 loading failed %08x", outBuffer[5]));
        return BERR_TRACE(errCode);
    }

    return errCode;
}
