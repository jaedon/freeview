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
 * $brcm_Workfile: parse_channels.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/7/12 6:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/common/parse_channels.h $
 * 
 * Hydra_Software_Devel/1   6/7/12 6:17p mphillip
 * SW7425-2907: Add a channels.txt parser for unittests
 *
 *
 *
******************************************************************************/

/*
 * parse_channels.h
 *
 * The purpose behind this set of utility functions is to provide a library which can parse
 * a Brutus-like channels.txt, so that the frontend unit tests can tune to specific channels
 * without hardcoding the information.
 *
 * (This may also be appropriate for integration tests, e.g. "tune a channel and record".)
 *
 * The basic structure is an array of channel_entries, which contains the parsed information.
 *
 * A return of NULL from parse_channels() means that either the file contained no entries, or
 * it could not be opened.
 *
 * parse_channels is expected to be used in conjunction with ../common/parse_commandline.h's
 * channels_file_name to get a default filename (or override via the command-line).
 *
 * comment_has_token(comment, token) is provided to allow trailing comments to contain special
 * values, meaningful to a test.  So, if there are lines like:
 *
 *     QPSK_DVB     0  1120.0  20.0     13v ToneOn # {weak}
 *     8PSK_TURBO   0  1150.0  21.5 2/3 13v ToneOn # {strong}
 *
 * the program could use comment_has_token(entries[i].comment,"weak") to check if the given channel
 * has {weak}, or "strong" to check if the channel is {strong}.  The meaning of these tokens is
 * unittest-defined.  (The functionality was originally required for a signal-strength test.)
 *
 * Trailing comments may contain more than one attribute.  Including trailing comments on a
 * PROGRAM PIDS line currently overwrites the associated channel line's trailing comment.
 *
 * (Note: this header can only be included once, and it includes code -- this allows it to
 * be dropped into existing unittests without modifying makefiles to link libraries or add
 * other source files.)
 */

#ifndef __PARSE_CHANNELS_H__
#define __PARSE_CHANNELS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstd.h"
#include "bkni.h"

#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#include "nexus_frontend_satellite.h"
#include "nexus_frontend_qam.h"
#include "nexus_frontend_ofdm.h"
#endif

#define is_str(x,y) (!strcasecmp(x,y))

#define CHANNELS_MAX_PROGRAMS 16

typedef
struct program_entries {
    unsigned pcr;
    unsigned video;
    unsigned audio;
    NEXUS_VideoCodec video_codec;
    NEXUS_AudioCodec audio_codec;
} program_entries;

typedef enum {
    channel_sds,
    channel_qam,
    channel_streamer,
    channel_analog,
    channel_vsb,
    channel_ofdm,
    channel_ip,
    channel_total_stream_types
} channel_type;

typedef
struct channel_entries {
    channel_type type;
    int sub_type;
#if NEXUS_HAS_FRONTEND
    struct {
        struct {
            NEXUS_FrontendSatelliteSettings satellite;
            NEXUS_FrontendDiseqcSettings diseqc;
        } satellite;
        NEXUS_FrontendQamSettings qam;
        NEXUS_FrontendOfdmSettings ofdm;
        unsigned streamer_index;
    } fe_settings;
#endif
    bool is_dss;
    int num_programs;
    program_entries programs[CHANNELS_MAX_PROGRAMS];
    char *trailing_comment;
} channel_entries;

static struct {
    const char *name;
    channel_type type;
    int value; /* the meaning of this value depends on the channel_stream_type */
} entry_types[] = {
    {"INVALID",     channel_sds, 0}, /* must be entry 0 in this list */
    {"INPUTBAND",   channel_streamer, 0}, /* legacy, same as STREAMER */
    {"STREAMER",    channel_streamer, 0},
    {"VSB8",        channel_vsb, 8},
    {"VSB16",       channel_vsb, 16},
    {"OFDM",        channel_ofdm, 0},
    {"QPSK_DSS",    channel_sds, NEXUS_FrontendSatelliteMode_eDss},
    {"QPSK_DSS_PES",channel_sds, NEXUS_FrontendSatelliteMode_eDss},
    {"QPSK_DVB",    channel_sds, NEXUS_FrontendSatelliteMode_eDvb},
    {"QPSK_LDPC",   channel_sds, NEXUS_FrontendSatelliteMode_eQpskLdpc},
    {"QPSK_TURBO",  channel_sds, NEXUS_FrontendSatelliteMode_eQpskTurbo},
    {"8PSK_TURBO",  channel_sds, NEXUS_FrontendSatelliteMode_e8pskTurbo},
    {"DCII",        channel_sds, NEXUS_FrontendSatelliteMode_eDcii},
    {"8PSK_LDPC",   channel_sds, 7},
    {"8PSK",        channel_sds, 7},
    {"LDPC",        channel_sds, 8},
    {"TURBO",       channel_sds, 9},
    {"LINEINPUT",   channel_analog, 0},
    {"ANALOG",      channel_analog, 1},
    {"QAM16",       channel_qam, NEXUS_FrontendQamMode_e16},
    {"QAM32",       channel_qam, NEXUS_FrontendQamMode_e32},
    {"QAM64",       channel_qam, NEXUS_FrontendQamMode_e64},
    {"QAM128",      channel_qam, NEXUS_FrontendQamMode_e128},
    {"QAM256",      channel_qam, NEXUS_FrontendQamMode_e256},
    {"QAM1024",     channel_qam, NEXUS_FrontendQamMode_e1024},
    {"IP_UDP",      channel_ip, 0},
    {"IP_RTP",      channel_ip, 1},
    {"IP_HTTP",     channel_ip, 2},
    {"IP_FEC",      channel_ip, 3},
    {0,             channel_sds, 0}
};

void default_entry(channel_entries *entry) {
    memset(entry,0,sizeof(*entry));
#if !SKIP_GET_DEFAULT
    NEXUS_Frontend_GetDefaultSatelliteSettings(&entry->fe_settings.satellite.satellite);
    NEXUS_Frontend_GetDefaultQamSettings(&entry->fe_settings.qam);
    NEXUS_Frontend_GetDefaultOfdmSettings(&entry->fe_settings.ofdm);
#endif
}

static struct {
    char *name;
    NEXUS_AudioCodec codec;
} audio_codec_pairs[] = {
    {"MPEG", NEXUS_AudioCodec_eMpeg},
    {"MPEG1", NEXUS_AudioCodec_eMpeg},
    {"MPEG1L2", NEXUS_AudioCodec_eMpeg},
    {"MP3", NEXUS_AudioCodec_eMp3},
    {"MPEG1L3", NEXUS_AudioCodec_eMp3},
    {"AC3", NEXUS_AudioCodec_eAc3},
    {"AAC", NEXUS_AudioCodec_eAac},
    {"AAC+", NEXUS_AudioCodec_eAacPlus},
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

NEXUS_AudioCodec parse_audio_codec(char *s) {
    NEXUS_AudioCodec rv = 0;

    int type_num;
    for (type_num=0;audio_codec_pairs[type_num].name;type_num++) {
        if (is_str(audio_codec_pairs[type_num].name, s)) {
            rv = audio_codec_pairs[type_num].codec;
            break;
        }
    }

    return rv;
}

static struct {
    char *name;
    NEXUS_VideoCodec codec;
} video_codec_pairs[] = {
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

NEXUS_VideoCodec parse_video_codec(char *s) {
    NEXUS_VideoCodec rv = 0;

    int type_num;
    for (type_num=0;video_codec_pairs[type_num].name;type_num++) {
        if (is_str(video_codec_pairs[type_num].name, s)) {
            rv = video_codec_pairs[type_num].codec;
            break;
        }
    }

    return rv;
}

void parse_program(program_entries *entry, char *token) {
    char *s = strdup(token);
    char *v = strchr(s, '='); /* find the = */

    if (v) {
        *v = 0;
        v++;
        if (v && *v) {
            if (is_str(s, "pcr")) {
                entry->pcr = strtoul(v, (char **)&v, 0);
            } else if (is_str(s, "video")) {
                entry->video = strtoul(v, (char **)&v, 0);
            } else if (is_str(s, "video_type")) {
                entry->video_codec = parse_video_codec(v);
            } else if (is_str(s, "audio")) {
                entry->audio = strtoul(v, (char **)&v, 0);
            } else if (is_str(s, "audio_type")) {
                entry->audio_codec = parse_audio_codec(v);
            }
        }
    }

    free(s);
}

void parse_coderate(NEXUS_FrontendSatelliteCodeRate *cr, char *token)
{
    char *s = strdup(token);
    char *v = strchr(s, '/'); /* find the / */

    if (v) {
        *v = 0;
        v++;
        if (v && *v) {
            cr->numerator = atoi(s);
            cr->denominator = atoi(v);
            v = strchr(v, '/');
            if (v) {
                *v = 0;
                v++;
                if (v && *v) {
                    float bps = atof(v);
                    cr->bitsPerSymbol = (int)(bps * 1000);
                }
            } else {
                cr->bitsPerSymbol = 0;
            }
        }
    }

    free(s);
}


bool parse_entry(channel_entries *entry, char *line) {
    static channel_entries *previous_entry = NULL;

    bool rv = false;
    bool parsing_pids = false;
    bool default_symrate = false;
    int token_count = 0;

    char *delim = " \t";
    char *s = strdup(line);
    char *t;

    default_entry(entry);

    t = strtok(s, delim);
    while (t && *t) {
        token_count++;
        if (*t == '#') {
            /* found a comment... */
            if (rv) {
                char *cpy;
                cpy = strdup(strpbrk(line, "#"));
                entry->trailing_comment = cpy;
            }
            break;
        } else if (is_str("PROGRAM",t)) {
            if (previous_entry) {
                parsing_pids = true;
            }
        } else if (parsing_pids) {
            if (token_count == 2) {
                if (!is_str("PIDS",t)) {
                    /* error, format is "PROGRAM PIDS" */
                    printf("Badly formed PROGRAM PIDS line! '%s'\n", line);
                    rv = false;
                    break;
                } else {
                    previous_entry->num_programs += 1;
                }
            } else {
                parse_program(&previous_entry->programs[previous_entry->num_programs - 1], t);
            }
        } else {
            /* neither a comment nor pids */
            previous_entry = entry;
            if (!rv) { /* no valid entry found yet */
                int type_num;
                for (type_num=0;entry_types[type_num].name;type_num++) {
                    if (is_str(entry_types[type_num].name, t)) {
                        entry->type = entry_types[type_num].type;
                        entry->sub_type = entry_types[type_num].value;
                        rv = true;
                        break;
                    }
                }
            } else { /* we have an existing channel type, parse the new token */
                switch(entry->type) {
                case channel_sds:
                    switch (token_count) {
                    case 2:
                        /* This token is actually ignored */
                        entry->fe_settings.satellite.satellite.mode = (NEXUS_FrontendQamMode)entry->sub_type;
                        break;
                    case 3:
                        entry->fe_settings.satellite.satellite.frequency = (unsigned)(atof(t) * 1000000);
                        break;
                    case 4:
                        entry->fe_settings.satellite.satellite.symbolRate = (unsigned)(atof(t) * 1000000);
                        break;
                    default:
                        if (is_str(t, "18v"))
                            entry->fe_settings.satellite.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
                        else if (is_str(t, "13v"))
                            entry->fe_settings.satellite.diseqc.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
                        else if (is_str(t, "ToneOn"))
                            entry->fe_settings.satellite.diseqc.toneEnabled = true;
                        else if (is_str(t, "LDPC_PILOT"))
                            entry->fe_settings.satellite.satellite.ldpcPilot = true;
                        else if (is_str(t, "LDPC_PILOT_PLL"))
                            entry->fe_settings.satellite.satellite.ldpcPilot = true;
#if 0
                        else if (is_str(t, "Diseqc_SAT_A")) {
                            str->sds.params.diseqc_message.enabled = true;
                            str->sds.params.diseqc_message.diseqc_sat_number = 1;
                        }
                        else if (is_str(t, "Diseqc_SAT_B")) {
                            str->sds.params.diseqc_message.enabled = true;
                            str->sds.params.diseqc_message.diseqc_sat_number = 2;
                        }
                        else if (is_str(t, "Diseqc_SAT_C")) {
                            str->sds.params.diseqc_message.enabled = true;
                            str->sds.params.diseqc_message.diseqc_sat_number = 3;
                        }
                        else if (is_str(t, "Diseqc_SAT_D")) {
                            str->sds.params.diseqc_message.enabled = true;
                            str->sds.params.diseqc_message.diseqc_sat_number = 4;
                        }
#endif
                        else {
                            /* default to coderate, which is #/# format. if there's no "/" found, no coderate is set. */
                            parse_coderate(&entry->fe_settings.satellite.satellite.codeRate, t);
                        }
                    }
                    break;
                case channel_qam:
                    switch (token_count) {
                    case 2:
                        /* This token is actually ignored, but we have some bookkeeping */
                        entry->fe_settings.qam.mode = (NEXUS_FrontendQamMode)entry->sub_type;
                        break;
                    case 3:
                        entry->fe_settings.qam.frequency = (unsigned)(atof(t) * 1000000);
                        break;
                    case 4:
                        if (is_str("DEFAULT",t)) {
                            default_symrate = true; /* Must know annex to determine default symbol rate */
                        } else {
                            entry->fe_settings.qam.symbolRate = (unsigned)(atof(t) * 1000000);
                        }
                    case 5:
                    default:
                        if (is_str("AnnexA",t)) {
                            entry->fe_settings.qam.annex = NEXUS_FrontendQamAnnex_eA;
                            if (default_symrate) {
                                entry->fe_settings.qam.symbolRate = 6952000;
                            }
                        } else if (is_str("AnnexB",t)) {
                            entry->fe_settings.qam.annex = NEXUS_FrontendQamAnnex_eB;
                            if (default_symrate) {
                                entry->fe_settings.qam.symbolRate = (entry->fe_settings.qam.mode == NEXUS_FrontendQamMode_e64) ? 5056941 : 536053;
                            }
                        } else if (is_str("AnnexC",t)) {
                            entry->fe_settings.qam.annex = NEXUS_FrontendQamAnnex_eC;
                            if (default_symrate) {
                                entry->fe_settings.qam.symbolRate = 5274000;
                            }
                        }
                        break;
                    }
                    break;
                case channel_streamer:
                    if (token_count == 3) {
                        entry->fe_settings.streamer_index = atoi(t);
                    }
                    if (token_count == 4) {
                        if (is_str("DSS_PES",t)) {
                            entry->is_dss = true;
                        }
                        if (is_str("DSS_ES",t)) {
                            entry->is_dss = true;
                        }
                    }
                    break;
                case channel_analog:
                    printf("Analog is unsupported\n");
                    break;
                case channel_vsb:
                    printf("VSB is unsupported\n");
                    break;
                case channel_ofdm:
                    printf("OFDM is unsupported\n");
                    break;
                case channel_ip:
                    printf("IP is unsupported\n");
                default:
                    break;
                }
            }
        }
        t = strtok(NULL, delim);
    }

    if (s) free(s);

    return rv;
}

channel_entries *parse_channels(const char *filename, int *num_entries) {
    int num_allocated = 4;
    channel_entries *entries = NULL;
    FILE *f = fopen(filename, "r");

    *num_entries = 0;

    while (f && !feof(f)) {
        char line[256];
        int len;
        channel_entries *scratch = NULL;
        if (!fgets(line, 256, f)) {
            break;
        }
        len = strlen(line);
        while (line[len-1] == '\n' || line[len-1] == '\r') {
            line[--len] = 0;
        }
        if (entries == NULL) {
            entries = (channel_entries *)malloc(num_allocated * sizeof(channel_entries));
        } else if (*num_entries == num_allocated) {
            int i;
            channel_entries *new_entries = (channel_entries *)malloc(2 * num_allocated * sizeof(channel_entries));
            for (i=0; i < *num_entries; i++) {
                memcpy(&new_entries[i],&entries[i],sizeof(channel_entries));
            }
            num_allocated *= 2;
            free(entries);
            entries = new_entries;
        }
        scratch = &entries[*num_entries];
        if (parse_entry(scratch, line)) {
            *num_entries += 1;
        }
    }

    return entries;
}

/* Searches comment for {"token"}.  Intended to be used to look for specific values
 * in the trailing comment on a channel line, e.g.:
 *   QPSK 0 1234 20.0 7/8 # {weak} (this is a weak signal channel)
 *
 * Tokens are only recognized if within curly-braces, {token} and are ignored otherwise.
 */
bool comment_has_token(const char *comment, const char *token) {
    bool rv = false;

    char *start = "{";
    char *end = "}";
    char *s = strdup(comment);
    char *b = strpbrk(s, start);
    while (b) {
        char *e = strpbrk(b, end);
        if (e) {
            b++;
            *e = 0;
            e++;
            if (is_str(token,b)) {
                rv = true;
                b = NULL;
            }
            b = strpbrk(e, start);
        } else { /* no trailing "}" */
            b = NULL;
        }
    }

    return rv;
}

#undef is_str

#endif
