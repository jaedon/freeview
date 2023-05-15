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
 * $brcm_Workfile: ip_streamer_subst.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/10/12 5:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/ip_streamer/ip_streamer_subst.h $
 * 
 * Hydra_Software_Devel/6   1/10/12 5:31p mward
 * SW7125-1205:  Add ip_addr and port to start settings, for use with
 * channel file parsing.
 * 
 * Hydra_Software_Devel/5   11/30/11 4:12p jtna
 * SW7405-4653: update comments
 * 
 * Hydra_Software_Devel/4   5/24/11 6:21p jtna
 * SW7405-5336: add check for IP streamer input
 * 
 * Hydra_Software_Devel/3   7/26/10 4:44p calvinho
 * SW7405-4653: Updated nomenclature
 * 
 * Hydra_Software_Devel/2   7/26/10 12:55p calvinho
 * SW7405-4653: Updated nomenclature
 * 
 * Hydra_Software_Devel/1   7/23/10 10:53a calvinho
 * SW7405-4653: Added IP Streamer input substitution
 *
 *****************************************************************************/
#ifndef IP_STREAMER_SUBST_H__
#define IP_STREAMER_SUBST_H__

#include "nexus_platform.h"
#include "nexus_stc_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_audio_dac.h"
#include "nexus_parser_band.h"
#include "nexus_playpump.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Ips *IpsHandle; 

typedef struct IpsOpenSettings
{
    NEXUS_ParserBand parserBand; /* parserband that ips will feed the remux into */
    unsigned playpumpIndex; /* playpump index that ips will use */
    unsigned remuxIndex; /* remux index that ips will use */
} IpsOpenSettings;

typedef struct IpsTimebaseSettings
{
    NEXUS_Timebase timebaseStc; /* timebase used by stc and playpump */
    NEXUS_Timebase timebaseDecouple; /* timebase used by audio output & display */
    NEXUS_AudioDacHandle audioDac;     
    NEXUS_DisplayHandle display;
} IpsTimebaseSettings;

typedef struct IpsStartSettings
{
    NEXUS_VideoDecoderHandle vDecoder;
    NEXUS_AudioDecoderHandle aDecoder;
    NEXUS_StcChannelHandle stcChannel;
	char ipAddr[256];                   /* IP Address (in dotted decimal or char name) string */
	unsigned port;                      /* port number of IP Channel (in host byte order) */
} IpsStartSettings;

IpsHandle IpsOpen(IpsOpenSettings *pSettings);
unsigned IpsAddParserBandSettings(IpsHandle ips, NEXUS_ParserBandSettings *parserBandSettings);

unsigned IpsAddStcSettings(IpsHandle ips, NEXUS_StcChannelSettings *stcSettings);
unsigned IpsAddTimebaseSettings(IpsHandle ips, NEXUS_TimebaseSettings *timebaseSettings);
unsigned IpsConfigureTimebase(IpsHandle ips, IpsTimebaseSettings *pSettings);

unsigned IpsAddVideoDecoderOpenSettings(IpsHandle ips, NEXUS_VideoDecoderOpenSettings *videoDecoderOpenSettings);
unsigned IpsAddAudioDecoderOpenSettings(IpsHandle ips, NEXUS_AudioDecoderOpenSettings *audioDecoderOpenSettings);

unsigned IpsStart(IpsHandle ips, const IpsStartSettings *pSettings);
void IpsClose(IpsHandle ips);

#ifdef __cplusplus
}
#endif

#endif
