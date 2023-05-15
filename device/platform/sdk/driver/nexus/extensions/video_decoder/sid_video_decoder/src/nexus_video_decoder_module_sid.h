/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  .  Except as set forth in an Authorized License, Broadcom grants
 *  no license , right to use, or waiver of any kind with respect to the
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
 *  LICENSORS BE LIABLE FOR  CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR  ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_module_sid.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/video_decoder/sid_video_decoder/src/nexus_video_decoder_module_sid.h $
 * 
 * 2   9/7/12 11:55a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_MODULE_SID_H__
#define NEXUS_VIDEO_DECODER_MODULE_SID_H__

#include "priv/nexus_picture_decoder_sid_video_decoder.h"



NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_Open_Sid( unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings);
NEXUS_Error NEXUS_VideoDecoderModule_P_Init_Sid(const NEXUS_VideoDecoderModuleSettings *pSettings);
void NEXUS_VideoDecoderModule_P_Uninit_Sid(void);

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoDecoder_Sid);

typedef struct NEXUS_VideoDecoder_Sid {
    BDBG_OBJECT(NEXUS_VideoDecoder_Sid)
    NEXUS_SidVideoDecoderHandle decoder;
    bool poweredUp;
} NEXUS_VideoDecoder_Sid;


#define LOCK_PICTURE_DECODER()    NEXUS_Module_Lock(g_NEXUS_videoDecoderModuleSettings.pictureDecoder)
#define UNLOCK_PICTURE_DECODER()  NEXUS_Module_Unlock(g_NEXUS_videoDecoderModuleSettings.pictureDecoder)


extern const NEXUS_VideoDecoder_P_Interface NEXUS_VideoDecoder_P_Interface_Sid;

#endif /* NEXUS_VIDEO_DECODER_MODULE_SID_H__ */

