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
 * $brcm_Workfile: nexus_video_decoder_security.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 3/23/11 11:20a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/video_decoder/security/7422/src/nexus_video_decoder_security.h $
 * 
 * 1   3/23/11 11:20a yili
 * SW7422-404:Video decoder security
 * 
 * 2   10/5/09 12:15p erickson
 * SW7405-2067: add NEXUS_VideoDecoder_P_DisableFwVerification. rename
 *  some regver structures.
 *
 * 1   3/16/09 11:31a yili
 * PR52990:Add region verification
 *
 * wince_7400/2   12/19/08 1:28p yili
 * PR42660:IPTV
 *
 * wince_7400/1   12/16/08 5:48p ptimariu
 * initial checkin video decoder firmware checking
 *
 *****************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_SECURITY_H__
#define NEXUS_VIDEO_DECODER_SECURITY_H__

#include "nexus_video_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_VideoDecoderSecureConfig
{
    BREG_Handle  hReg;
    int index;
}   NEXUS_VideoDecoderSecureConfig;

typedef struct NEXUS_VideoDecoderSecureBootContext
{
    BREG_Handle hReg;
    unsigned int deviceID;
    bool bVichRegSet;
}   NEXUS_VideoDecoderSecureBootContext;

void NEXUS_VideoDecoder_P_EnableSecurity (
    BXVD_Settings *pSettings,
    const NEXUS_VideoDecoderSecureConfig *pConfig
    );

void NEXUS_VideoDecoder_P_DisableFwVerification(
    unsigned int index
    );

#ifdef __cplusplus
}
#endif

#endif
