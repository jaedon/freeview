
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
 * $brcm_Workfile: read_channels_file.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/10/12 5:48p $
 *
 * Module Description:
 * Simplified parsing for QAM entries in a channels.txt file.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/channels_file/read_channels_file.h $
 * 
 * Hydra_Software_Devel/1   1/10/12 5:48p mward
 * SW7125-1205:  Add channels file support.
 * 
 * Hydra_Software_Devel/1   1/6/12 5:32p mward
 * SW7125-1205:  Parse a channels file for QAM and program info.
 * 
 ******************************************************************************/
#include "nexus_frontend.h"

#define READ_CHANNELS_ENTRIES_MAX 8

typedef enum ReadChannelType
{
    ReadChannelType_eStreamer,
    ReadChannelType_eQam,
    ReadChannelType_eIp,
    ReadChannelType_eMax
} ReadChannelType;

typedef struct ReadChannelIpSettings {
	char 			ip_addr[64]; 
	unsigned int 	port;
}ReadChannelIpSettings;

typedef struct ReadChannelStreamerSettings {
	int index;
} ReadChannelStreamerSettings;

typedef struct ReadChannelEntry {
	ReadChannelType channelType;
	union {
		ReadChannelStreamerSettings streamerSettings;
		NEXUS_FrontendQamSettings qamSettings;
		ReadChannelIpSettings ipSettings;
	} settings;
    unsigned short video_pid;
	NEXUS_VideoCodec video_codec;
    unsigned short audio_pid;
	NEXUS_AudioCodec audio_codec;
	unsigned short pcr_pid;
} ReadChannelEntry;

/* reads channel entries from a Brutus channels.txt style file. Expects an explict PROGRAM PIDS line after 
each QAM line.  Returns the number of valid entries, not to exceed num_entries. */
int ReadChannelsFile(const char *filename, ReadChannelEntry *entries, const int num_entries);

