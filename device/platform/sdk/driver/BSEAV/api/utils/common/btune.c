/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btune.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/30/09 12:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/btune.c $
 * 
 * 7   7/30/09 12:15p vsilyaev
 * PR 57223: Fixed bad use of pointers
 * 
 * 6   5/15/08 1:44p erickson
 * PR42708: coverity fix
 *
 * 5   11/21/07 11:47a jtna
 * PR35939: Coverity Defect ID:482 SECURE_CODING - mark as intentional
 *
 * 4   10/25/07 2:24p jtna
 * PR35939: Coverity Defect ID:482 SECURE_CODING
 *
 * 2   2/2/06 5:04p vsilyaev
 * PR 19233:Use common function for tune. Added diseqc_tone option
 *
 * 1   11/30/05 5:02p vsilyaev
 * PR 18183: Common tune functions
 *
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "bsettop.h"
#include "bargs.h"
#include "btune.h"

void
btune_mpeg_init(bstream_mpeg *mpeg)
{
    /* setup mpeg parameters */
    bstream_mpeg_init(mpeg);

    if (bmpeg_arg.mpeg_type) {
        mpeg->mpeg_type = *bmpeg_arg.mpeg_type;
    }
    if (bmpeg_arg.mpeg_str) {
        unsigned video_pid = mpeg->video[0].pid;
        unsigned pcr_pid = mpeg->pcr_pid;
        unsigned audio_pid = mpeg->audio[0].pid;
        unsigned audio_format = mpeg->audio[0].format;

        /* coverity[secure_coding] */
        int n = sscanf(bmpeg_arg.mpeg_str,"0x%x,0x%x,0x%x,0x%x",
            &video_pid,
            &pcr_pid,
            &audio_pid,
            &audio_format);
            mpeg->video[0].pid = video_pid;
            mpeg->pcr_pid = pcr_pid;
            mpeg->audio[0].pid = audio_pid;
            mpeg->audio[0].format = audio_format;
            if (n < 2) {
                mpeg->pcr_pid = mpeg->video[0].pid;
            }
    } else {
        if (bmpeg_arg.pcr_pid) {
            mpeg->pcr_pid = *bmpeg_arg.pcr_pid;
        } else if (bmpeg_arg.video_pid) {
            /* use video pid as PCR pid */
            mpeg->pcr_pid = *bmpeg_arg.video_pid;
        }
        if (bmpeg_arg.video_pid) {
            mpeg->video[0].pid = *bmpeg_arg.video_pid;
            if (bmpeg_arg.video_type) {
                mpeg->video[0].format = *bmpeg_arg.video_type;
            }
        }
        if (bmpeg_arg.audio_pid) {
            mpeg->audio[0].pid = *bmpeg_arg.audio_pid;
            if (bmpeg_arg.audio_type) {
                mpeg->audio[0].format = *bmpeg_arg.audio_type;
            }
        }
    }
    if (bmpeg_arg.mpeg_type) {
        mpeg->mpeg_type = *bmpeg_arg.mpeg_type;
    }
    if (bmpeg_arg.encryption) {
        mpeg->encryption = *bmpeg_arg.encryption;
    }
    return;
}

bstream_t
btune_tune(unsigned channel, bstream_mpeg *mpeg, bool encode_analog, btune_state *state)
{
    bstream_t stream=NULL;
    btuner_t tuner=NULL;
    bool digital = false;
    int tuner_index;

    state->tuner = NULL;
    state->stream = NULL;

    digital = mpeg->video[0].pid || mpeg->audio[0].pid || bmpeg_arg.mpeg_type;

    if (btuner_arg.channel) {
        tuner_index = *btuner_arg.channel;
    } else {
        tuner_index = channel;
    }

    /* Streamer */
    if (bstreamer_arg.channel) {
        bband_t band;
        band = bstreamer_attach( B_ID((int)(*bstreamer_arg.channel)), bmpeg_arg.mpeg_type ? *bmpeg_arg.mpeg_type : bstream_mpeg_type_ts );
        if (band) {
            stream = bstream_open(band, mpeg);
        }
    }
    else if (b_arg.frequency && digital && bqam_arg.mode) {
        bband_t band;
        btuner_qam_params qam;
        int freq = 765000000;

        tuner = btuner_open(B_ID(tuner_index)); /* open tuner */
        if (!tuner) {
            fprintf(stderr, "Source is not avaliable\n");
            return NULL;
        }

        /* open components */
        fprintf(stderr, "Record from a QAM\n");
        /* acquire signal */
        btuner_qam_params_init(&qam, tuner);
        if (b_arg.symbol_rate) qam.symbol_rate=*b_arg.symbol_rate;
        if (b_arg.frequency) freq=*b_arg.frequency;
        if (bqam_arg.mode) qam.mode = *bqam_arg.mode;

        band = btuner_tune_qam(tuner, freq, &qam);
        if (!band) {
            fprintf(stderr, "Can't tune\n");
            return NULL;
        }

        stream = bstream_open(band, mpeg);
        if (!stream) {
            fprintf(stderr, "Can't acquire a signal\n");
            return NULL;
        }
    }
    /**
    * SDS - Satellite
    **/
    else if (b_arg.frequency && digital && bsds_arg.mode) {
        btuner_sds_params sds;
        bband_t band;
        int freq = 1120000000;

        tuner = btuner_open(B_ID(tuner_index)); /* open tuner */
        if (!tuner) {
            fprintf(stderr, "Source is not avaliable\n");
            return NULL;
        }
        fprintf(stderr, "Record from a SDS\n");

        /* acquire signal */
        btuner_sds_params_init(&sds, tuner);
        if (b_arg.symbol_rate) sds.symbol_rate=*b_arg.symbol_rate;
        if (b_arg.frequency) freq=*b_arg.frequency;
        if (bsds_arg.mode) sds.mode = *bsds_arg.mode;
        if (bsds_arg.diseqc_polarity) sds.diseqc_polarity = *bsds_arg.diseqc_polarity;
        if (bsds_arg.diseqc_tone) sds.diseqc_tone = *bsds_arg.diseqc_tone;


        band = btuner_tune_sds(tuner, freq, &sds);
        if (!band) {
            fprintf(stderr, "Can't tune\n");
            return NULL;
        }

        if (sds.mode == btuner_sds_mode_qpsk_dss) {
            mpeg->mpeg_type =  bstream_mpeg_type_dss_es;
        }

        stream = bstream_open(band, mpeg);
    }
    else if (b_arg.frequency && digital && bvsb_arg.mode) {
        btuner_vsb_params vsb;
        bband_t band;
        int freq = 1120000000;

        tuner = btuner_open(B_ID(tuner_index)); /* open tuner */
        if (!tuner) {
            fprintf(stderr, "Source is not avaliable\n");
            return NULL;
        }
        fprintf(stderr, "Decode from VSB\n");
        /* acquire signal */
        btuner_vsb_params_init(&vsb, tuner);
        if (b_arg.symbol_rate) vsb.symbol_rate=*b_arg.symbol_rate;
        if (b_arg.frequency) freq=*b_arg.frequency;
        if (bvsb_arg.mode) vsb.mode = *bvsb_arg.mode;

        band = btuner_tune_vsb(tuner, freq, &vsb);
        if (!band) {
            fprintf(stderr, "Can't tune\n");
            return NULL;
        }

        stream = bstream_open(band, mpeg);
    }
    /**
    * Analog
    **/
    else if (!digital && (b_arg.frequency || banalog_arg.linein)) {
        btuner_analog_params analog;
        bencode_settings settings;
        bencode_t encode;

        tuner = btuner_open(B_ID(tuner_index)); /* open tuner */
        if (!tuner) {
            fprintf(stderr, "Source is not avaliable\n");
            return NULL;
        }

        fprintf(stderr, "Decode analog\n");

        btuner_analog_params_init(&analog, tuner);
        if (banalog_arg.format) analog.video_format = *banalog_arg.format;
        if (banalog_arg.linein) {
            btuner_linein_t linein = btuner_linein_open(B_ID(*banalog_arg.linein));
            stream = btuner_tune_linein(tuner, linein, &analog);
        } else if (b_arg.frequency) {
            stream = btuner_tune_rf(tuner, *b_arg.frequency, &analog);
        }
        if (!stream) {
            fprintf(stderr, "Can't acquire a signal\n");
            return NULL;
        }
        if (encode_analog) {
            encode = bencode_open(B_ID(banalog_arg.encode_channel ? *banalog_arg.encode_channel:0));
            if (!encode) {
                fprintf(stderr, "Can't initialize encoder\n");
                return NULL;
            }
            bencode_settings_init(&settings, bencode_quality_best, encode);
            if (banalog_arg.encode_audio_type) {
                settings.audio_type = *banalog_arg.encode_audio_type;
            }
            stream = bencode_start(encode, stream, &settings);
            if (!stream) {
                fprintf(stderr, "Can't encode stream\n");
                return NULL;
            }
        }
    } else {
        fprintf(stderr, "Unsupported source\n");
        fprintf(stderr, "Supported sources are:\n"
                    "QAM: - frequency, qam mode, symbol rate and video pid are required\n"
                    "SDS - frequency, sds mode, symbol rate and video pid are required\n"
                    "VSB - frequency, vsb mode, symbol rate and video pid are required\n"
                    "Streamer - stream number and video pid are required\n"
                    "Analog-RF: - frequency is required\n"
                    "Analog-LineIn: - 'linein' is required\n");
        return NULL;
    }
    state->tuner = tuner;
    state->stream = stream;
    return stream;
}


void
btune_teardown(btune_state *state)
{
    if(state->tuner) {
        btuner_close(state->tuner);
    }
    state->tuner = NULL;
    state->stream = NULL;
    return;
}
