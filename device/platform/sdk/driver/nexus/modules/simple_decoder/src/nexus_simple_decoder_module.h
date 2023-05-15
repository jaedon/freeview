/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_simple_decoder_module.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/src/nexus_simple_decoder_module.h $
 * 
 * 5   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 4   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 3   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 * 2   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_DECODER_MODULE_H__
#define NEXUS_SIMPLE_DECODER_MODULE_H__

#include "nexus_simple_decoder_thunks.h"
#include "nexus_base.h"
#include "nexus_types.h"
#include "nexus_simple_decoder_init.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_simple_audio_playback.h"
#include "nexus_simple_audio_decoder_server.h"
#include "nexus_simple_decoder_types.h"

#ifdef __cplusplus
#error
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif
#define NEXUS_MODULE_NAME simple_decoder
#define NEXUS_MODULE_SELF g_NEXUS_simpleDecoderModule

extern NEXUS_ModuleHandle g_NEXUS_simpleDecoderModule;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SimpleVideoDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SimpleAudioPlayback);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SimpleAudioDecoder);

#endif
