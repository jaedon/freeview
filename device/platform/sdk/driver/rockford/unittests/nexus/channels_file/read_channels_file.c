
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
 * $brcm_Workfile: read_channels_file.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/11/12 3:02p $
 *
 * Module Description:
 * Simplified parsing for QAM entries in a channels.txt file.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/channels_file/read_channels_file.c $
 * 
 * Hydra_Software_Devel/2   1/11/12 3:02p mward
 * SW7125-1205:  BDBG_MSG both opened and not-opened file.
 * 
 * Hydra_Software_Devel/1   1/10/12 5:47p mward
 * SW7125-1205:  Add channels file support.
 * 
 * Hydra_Software_Devel/1   1/6/12 5:31p mward
 * SW7125-1205:  Parse a channels file for QAM and program info.
 * 
 ******************************************************************************/
#include "nexus_platform.h"
#include "read_channels_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LIST_MAX 10 /* maximum sub-string items in a line */
#define MEGA   1000000

BDBG_MODULE(read_channels_file);

static struct {
    const char *name;
    NEXUS_FrontendQamMode mode;
	ReadChannelType type;
} entry_types[] = {
    {"INVALID",     NEXUS_FrontendQamMode_eMax,	ReadChannelType_eMax}, /* must be entry 0 in this list */
	{"STREAMER",    NEXUS_FrontendQamMode_eMax, ReadChannelType_eStreamer},
    {"QAM16",       NEXUS_FrontendQamMode_e16,	ReadChannelType_eQam},
    {"QAM32",       NEXUS_FrontendQamMode_e32,	ReadChannelType_eQam},
    {"QAM64",       NEXUS_FrontendQamMode_e64,	ReadChannelType_eQam},
    {"QAM128",      NEXUS_FrontendQamMode_e128,	ReadChannelType_eQam},
    {"QAM256",      NEXUS_FrontendQamMode_e256,	ReadChannelType_eQam},
    {"QAM1024",     NEXUS_FrontendQamMode_e1024,	ReadChannelType_eQam},
	{"IP_UDP",      NEXUS_FrontendQamMode_e256,	ReadChannelType_eIp},
    {0,             NEXUS_FrontendQamMode_eMax,	ReadChannelType_eMax} /* terminator */
};

/* adapted from bsettop_namevalue_pairs.c */
static struct {
    const char *name;
    NEXUS_AudioCodec codec;
} audio_codecs[] = {
    {"MPEG", NEXUS_AudioCodec_eMpeg},
    {"MPEG1", NEXUS_AudioCodec_eMpeg},
    {"MPEG1L2", NEXUS_AudioCodec_eMpeg},
    {"MP3", NEXUS_AudioCodec_eMp3},
    {"MPEG1L3", NEXUS_AudioCodec_eMp3},
    {"AC3", NEXUS_AudioCodec_eAc3},
    {"AAC", NEXUS_AudioCodec_eAac},
    {"AAC+", NEXUS_AudioCodec_eAacPlus}, /* loas is same as aac plus */
    {"AAC+ADTS", NEXUS_AudioCodec_eAacPlusAdts}, 
    {"DTS", NEXUS_AudioCodec_eDts},
    {"AC3+", NEXUS_AudioCodec_eAc3Plus},
    {"DDP", NEXUS_AudioCodec_eAc3Plus},
    {"DTSHD", NEXUS_AudioCodec_eDtsHd},
    {"WMA", NEXUS_AudioCodec_eWmaStd},
    {"WMASTD", NEXUS_AudioCodec_eWmaStd},
    {"WMAPRO", NEXUS_AudioCodec_eWmaPro},
    {"LPCM_DVD", NEXUS_AudioCodec_eLpcmDvd},
    {"LPCM_HDDVD", NEXUS_AudioCodec_eLpcmHdDvd},
    {"LPCM_BLURAY", NEXUS_AudioCodec_eLpcmBluRay},
    {"AVS", NEXUS_AudioCodec_eAvs},
    {"PCM", NEXUS_AudioCodec_ePcm},
    {"DRA", NEXUS_AudioCodec_eDra},
    {"COOK", NEXUS_AudioCodec_eCook},
    {"VORBIS", NEXUS_AudioCodec_eVorbis},
    {NULL, 0}
};

/* adapted from bsettop_namevalue_pairs.c */
static struct {
    const char *name;
    NEXUS_VideoCodec codec;
}  video_codecs[] = {
    {"MPEG", NEXUS_VideoCodec_eMpeg2},
    {"MPEG2", NEXUS_VideoCodec_eMpeg2},
    {"MPEG1", NEXUS_VideoCodec_eMpeg1},
    {"AVC", NEXUS_VideoCodec_eH264},
    {"H.264", NEXUS_VideoCodec_eH264},
    {"H264", NEXUS_VideoCodec_eH264},
    {"SVC", NEXUS_VideoCodec_eH264_Svc},
    {"MVC", NEXUS_VideoCodec_eH264_Mvc},
    {"H263", NEXUS_VideoCodec_eH263},
    {"H.263",NEXUS_VideoCodec_eH263},
    {"VC1", NEXUS_VideoCodec_eVc1},
    {"VC-1", NEXUS_VideoCodec_eVc1},
    {"VC1-SM", NEXUS_VideoCodec_eVc1SimpleMain},
    {"VC1SM", NEXUS_VideoCodec_eVc1SimpleMain},
    {"VC-1SM", NEXUS_VideoCodec_eVc1SimpleMain},
    {"VC-1-SM", NEXUS_VideoCodec_eVc1SimpleMain},
    {"DivX3", NEXUS_VideoCodec_eDivx311},
    {"DivX-3", NEXUS_VideoCodec_eDivx311},
    {"MPEG4", NEXUS_VideoCodec_eMpeg4Part2},
    {"MPEG4-2", NEXUS_VideoCodec_eMpeg4Part2},
    {"AVS", NEXUS_VideoCodec_eAvs},
    {NULL, 0}
};

int split(char *str, const char *delim, const char **list, const int list_max);
bool ReadChannelsGetChanParams(char* line, ReadChannelEntry *pChEntry);
bool ReadChannelsGetProgramParams(char* line, ReadChannelEntry *pChEntry);

int split(char *str, const char *delim, const char **list, const int list_max) {
    char *next;
    int count = 0;
    while ((str && *str) && (count<list_max)) {
		list[count++] = str;
        next = strpbrk(str, delim);
		if (next) {
			str = strspn(next, delim) + next;
			*(next) = '\0';
		}
		else {
			break;
		}
    }
    return count;
}


int ReadChannelsFile(const char *filename, ReadChannelEntry *entries, const int n_entries) {
    char buf[256];
	int i = 0;
	int len;
	bool qamLine = true;
	char *comment;

    FILE *f = fopen(filename, "r");
    if (!f) {
		BDBG_MSG(("%s(%s) file NOT opened.",__FUNCTION__,filename));
		return 0;
	}
	BDBG_MSG(("%s(%s) opened file.",__FUNCTION__,filename));

    while (!feof(f) && (i<n_entries)) {
        if (!fgets(buf, 256, f))
            break;
        
        comment = strchr(buf, '#');	/* check for line comment */
        if (comment)
            *comment = 0;

        len = strlen(buf);
        if (buf[len-1] == '\n')
            buf[--len] = 0;
        if (buf[len-1] == '\r')
            buf[--len] = 0;

		if (qamLine) {
			if (ReadChannelsGetChanParams(buf,&entries[i])) {
				qamLine = false;
			}
		}
		else {
			if (ReadChannelsGetProgramParams(buf,&entries[i])) {
				qamLine = true;
				i++;
			}
		}
    }
    fclose(f);
	BDBG_MSG(("%s() read %d channels",__FUNCTION__,i));
    return i;
}

bool ReadChannelsGetChanParams(char* line, ReadChannelEntry *pChEntry)
{
	const char *list[LIST_MAX];
	int total = split(line, " \t\0", list, LIST_MAX);
	const char *entry_type;
	int disabled; /* non-zero indicates a disabled channel entry */
	int type_num;
	bool default_symrate;

	if (total == 0)
        return false;

    entry_type = list[0]; /* QAM64, QAM256, etc. */
    BDBG_MSG(("%s() '%s'",__FUNCTION__,entry_type));

    for (type_num=0;entry_types[type_num].name;type_num++) {
        if (!strcmp(entry_types[type_num].name, entry_type))
            break;
    }
    if (!entry_types[type_num].name)
        return false;

	pChEntry->channelType = entry_types[type_num].type;

    if (total >= 2)
        disabled = atoi(list[1]); /* "disabled" flag, normally 0 */
    else
        disabled = 1;

    if (type_num == 0) { /* "INVALID" */
        disabled = 1;
    }
	if (disabled) {
		return false;
	}
	switch (pChEntry->channelType) {
	case ReadChannelType_eStreamer: 
		pChEntry->settings.streamerSettings.index = (total >= 3) ? atoi(list[2]) : 0;
		BDBG_MSG(("%s() streamer = %d",__FUNCTION__,pChEntry->settings.streamerSettings.index));
		return true;
	case ReadChannelType_eQam:
		{
			NEXUS_FrontendQamSettings *pSettings = &pChEntry->settings.qamSettings;
			if (total >= 3) {
				pSettings->mode = entry_types[type_num].mode;
				pSettings->frequency = MEGA*atof(list[2]);
				BDBG_MSG(("%s() mode = %d",__FUNCTION__,pSettings->mode));
				BDBG_MSG(("%s() frequency = %u",__FUNCTION__,pSettings->frequency));
	
				/* have to get annex before symrate */
				if (total > 4)
					pSettings->annex = !strcasecmp(list[4], "AnnexA")?
						NEXUS_FrontendQamAnnex_eA:!strcasecmp(list[4], "AnnexC") ?
						NEXUS_FrontendQamAnnex_eC:NEXUS_FrontendQamAnnex_eB;
	
				default_symrate = total <= 3 || !strcasecmp(list[3], "DEFAULT");
				pSettings->symbolRate = default_symrate ? 0 : (unsigned)(atof(list[3]) * MEGA);
			}
			pSettings->bandwidth = (pSettings->annex == NEXUS_FrontendQamAnnex_eA) ? 8*MEGA : 6*MEGA;
		}
		return true;
	case ReadChannelType_eIp:
		if (total >= 4) {
			strncpy(pChEntry->settings.ipSettings.ip_addr, list[2], sizeof(pChEntry->settings.ipSettings.ip_addr));
			pChEntry->settings.ipSettings.port = atoi(list[3]);
			BDBG_MSG(("%s() ip_addr = %s port = %d",__FUNCTION__,pChEntry->settings.ipSettings.ip_addr,
					  pChEntry->settings.ipSettings.port));
			return true;
		}
		else {
			BDBG_ERR(("IP_UDP line incomplete."));
			return false;
		}
	default:
		BDBG_ERR(("Channel Type not supported."));
		return false;
	}
}

bool ReadChannelsGetProgramParams(char* line, ReadChannelEntry *pChEntry)
{
	const char *list[LIST_MAX];
	int total = split(line, " \t\0", list, LIST_MAX);

    if (total > 1 && !strcasecmp(list[1], "PIDS")) {
        int i,j;
        unsigned pos=0;

        for (i=2;i<total;i++) {
            char *val = strchr(list[i], '=');
            if (val) {
                char name[16];
                unsigned n = val - list[i];
                if (n > 15) n = 15;
                strncpy(name, list[i], n);
                name[n] = 0;
                val++;

                if (!strcasecmp(name, "video")) {
					pChEntry->video_pid = strtoul(val, (char **)&val, 0);
                }
                else if (!strcasecmp(name, "video_type")) {
					for (j=0;video_codecs[j].name;j++) {
						if (!strcasecmp(video_codecs[j].name, val))
							break;
					}
					if (video_codecs[j].name) {
						pChEntry->video_codec = video_codecs[j].codec;
					}
					else {
						BDBG_ERR(("%s() video_type %s NOT VALID",__FUNCTION__,val));
						return false;
					}
                }
                else if (!strcasecmp(name, "audio")) {
					pChEntry->audio_pid = strtoul(val, (char **)&val, 0);
                }
                else if (!strcasecmp(name, "audio_type")) {
					for (j=0;audio_codecs[j].name;j++) {
						if (!strcasecmp(audio_codecs[j].name, val))
							break;
					}
					if (audio_codecs[j].name) {
						pChEntry->audio_codec = audio_codecs[j].codec;
					}
					else {
						return false;
					}
                }
                else if (!strcasecmp(name, "pcr")) {
                    pChEntry->pcr_pid = strtoul(val, NULL, 0);
                }
                else {
                    BDBG_WRN(("Unknown label '%s', value '%s'", name, val));
                }
            }
            else {
                /* if there's no label, take them in a fixed order */
                switch (pos++) {
                case 0: pChEntry->pcr_pid = strtoul(list[i], NULL, 0); break;
                case 1: pChEntry->video_pid = strtoul(list[i], NULL, 0); break;
                case 2: pChEntry->audio_pid = strtoul(list[i], NULL, 0); break;
                case 3: pChEntry->audio_codec = strtoul(list[i], NULL, 0); break;
                case 4: pChEntry->video_codec = strtoul(list[i], NULL, 0); break;
                default: break;
                }
            }
        }
		BDBG_MSG(("%s() pcr_pid 0x%02x",__FUNCTION__,pChEntry->pcr_pid));
		BDBG_MSG(("%s() video_pid 0x%02x",__FUNCTION__,pChEntry->video_pid));
		BDBG_MSG(("%s() audio_pid 0x%02x",__FUNCTION__,pChEntry->audio_pid));
		BDBG_MSG(("%s() audio_codec 0x%02x",__FUNCTION__,pChEntry->audio_codec));
		BDBG_MSG(("%s() video_codec 0x%02x",__FUNCTION__,pChEntry->video_codec));

        /* a helpful default */
        if (pChEntry->video_pid && !pChEntry->pcr_pid) {
            pChEntry->pcr_pid = pChEntry->video_pid;
            BDBG_WRN(("Automatically using vidpid for pcrpid in channels.txt. override with pcr=XXXX."));
        }
        if (!pChEntry->video_pid && !pChEntry->audio_pid && pChEntry->pcr_pid) {
            pChEntry->video_pid = pChEntry->pcr_pid;
            BDBG_WRN(("Automatically using pcrpid for vidpid in channels.txt. override with video=XXXX."));
        }

		return true;
    }
	return false;
}


