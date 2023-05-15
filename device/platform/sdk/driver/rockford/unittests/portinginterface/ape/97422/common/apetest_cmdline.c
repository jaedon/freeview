/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: apetest_cmdline.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_cmdline.c $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 *      
 * 03/06/12 carson
 * ripped from cmdline_args.c    Thanks!
 * 
 *****************************************************************************/

#include "apetest_common.h"

#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"
#include "bmedia_cdxa.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif
#include "bfile_stdio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



static const namevalue_t g_audioCodecStrs[] = {
    {"mpegL1",          BAVC_AudioCompressionStd_eMpegL1        },
    {"mpegL2",          BAVC_AudioCompressionStd_eMpegL2        },
    {"mpegL3",          BAVC_AudioCompressionStd_eMpegL3        },
    {"aac",             BAVC_AudioCompressionStd_eAac           },
    {"aacAdts",         BAVC_AudioCompressionStd_eAacAdts       },
    {"aacLoas",         BAVC_AudioCompressionStd_eAacLoas       },
    {"aacPlus",         BAVC_AudioCompressionStd_eAacPlus       },
    {"aacPlusLoas",     BAVC_AudioCompressionStd_eAacPlusLoas   },
    {"aacPlusAdts",     BAVC_AudioCompressionStd_eAacPlusAdts   },
    {"ac3",             BAVC_AudioCompressionStd_eAc3           },
    {"ddp",             BAVC_AudioCompressionStd_eAc3Plus       },
    {"Ac3Lossless",     BAVC_AudioCompressionStd_eAc3Lossless   },
    {"Dts",             BAVC_AudioCompressionStd_eDts           },
    {"Dtshd",           BAVC_AudioCompressionStd_eDtshd         },
    {"DtsLegacy",       BAVC_AudioCompressionStd_eDtsLegacy     },
    {"WmaStd",          BAVC_AudioCompressionStd_eWmaStd        },
    {"WmaStdTs",        BAVC_AudioCompressionStd_eWmaStdTs      },
    {"WmaPro",          BAVC_AudioCompressionStd_eWmaPro        },
    {"Mlp",             BAVC_AudioCompressionStd_eMlp           },
    {"Pcm",             BAVC_AudioCompressionStd_ePcm           },
    {"PcmWav",          BAVC_AudioCompressionStd_ePcmWav        },
    {"LpcmDvd",         BAVC_AudioCompressionStd_eLpcmDvd       },
    {"LpcmHdDvd",       BAVC_AudioCompressionStd_eLpcmHdDvd     },
    {"LpcmBd",          BAVC_AudioCompressionStd_eLpcmBd        },
    {"Lpcm1394",        BAVC_AudioCompressionStd_eLpcm1394      },
    {"Amr",             BAVC_AudioCompressionStd_eAmr           },
    {"Dra",             BAVC_AudioCompressionStd_eDra           },
    {"Cook",            BAVC_AudioCompressionStd_eCook          },
    {"Adpcm",           BAVC_AudioCompressionStd_eAdpcm         },
    {"Sbc",             BAVC_AudioCompressionStd_eSbc           },
    {"Vorbis",          BAVC_AudioCompressionStd_eVorbis        },  
    {"G711",            BAVC_AudioCompressionStd_eG711          },
    {"G723_1",          BAVC_AudioCompressionStd_eG723_1        },
    {"G726",            BAVC_AudioCompressionStd_eG726          },
    {"G729",            BAVC_AudioCompressionStd_eG729          },
    {"Flac",            BAVC_AudioCompressionStd_eFlac          },
    {NULL,              0}
};


static const namevalue_t g_streamStrs[] = {
    {"TsMpeg",          BAVC_StreamType_eTsMpeg        },
    {"DssEs",           BAVC_StreamType_eDssEs         },
    {"Pes",             BAVC_StreamType_ePes           },
    {"Es",              BAVC_StreamType_eEs            },
    {"Bes",             BAVC_StreamType_eBes           },
    {"DssPes",          BAVC_StreamType_eDssPes        },
    {"PS",              BAVC_StreamType_ePS            },
    {"Cms",             BAVC_StreamType_eCms           },   
    {"TsBDRay",         BAVC_StreamType_eTsBDRay       },   
    {"Mpeg1System",     BAVC_StreamType_eMpeg1System   },   
    {"AVI",             BAVC_StreamType_eAVI           },   
    {"MPEG4",           BAVC_StreamType_eMPEG4         },   
    {"MKV",             BAVC_StreamType_eMKV           },   
    { NULL,             0                              }             
};                                                                    



static unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    char *endptr;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    value = strtol(name, &endptr, 0);
    if(!endptr || *endptr) { /* if valid, *endptr = '\0' */
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}

void APETEST_PrintList(const namevalue_t *table)
{
    unsigned i;
    const char *sep=" {";
    for (i=0;table[i].name;i++) {
        /* skip aliases */
        if (i > 0 && table[i].value == table[i-1].value) continue;
        printf("%s%s",sep,table[i].name);
        sep = ",";
    }
    printf("}");
}

void APETEST_PrintListOption(const char *name, const namevalue_t *list)
{
    printf("  -%s ", name);
    APETEST_PrintList(list);
    printf("\n");
}

void APETEST_PrintUsageCommon(void)
{
    printf(
        "  -h | --help  - this usage information\n"
        );
    APETEST_PrintListOption("audio_type", g_audioCodecStrs);
    APETEST_PrintListOption("stream_type", g_streamStrs);
    printf("  -audio PID\n");
#if 0
    printf(
        "  -probe - use media probe to discover stream format for playback (defaults on if no video or audio pid)\n"
        );
    printf(
        "  -compressed_audio - Also output compressed audio\n"
        "  -multichannel_audio - Also output multichannel audio on hdmi\n"
        "  -pcm sr,channels,bits - Play LPCM file\n"
        );
#endif
    return;
}

int APETEST_ParseCommon(int offset, int argc, const char *argv[], APETEST_CommonOpts *opts, void (*usage)(const char *) )
{
    int i;

    if (offset==0) {
        offset = 1;
        memset(opts,0,sizeof(*opts));
        opts->audioCodec = APETEST_INVALID;
        opts->streamType = APETEST_INVALID;
        opts->decodedAudio = true;
        opts->useHdmiOutput = false;
    }

    for (i=offset;i<argc;i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")
           || !strcmp(argv[i], "?") || !strcmp(argv[i], "-?")) {
            usage(argv[0]);
            return -1;
        }
        else if (!strcmp(argv[i], "-pcr") && i+1<argc) {
            opts->pcrPid = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-audio") && i+1<argc) {
            opts->audioPid = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-audio_type") && i+1<argc) {
            opts->audioCodec=lookup(g_audioCodecStrs, argv[++i]);
        }
#if 0
        else if (!strcmp(argv[i], "-hdmi") && i+1<argc) {
            opts->useHdmiOutput = strcasecmp(argv[++i], "off");
        }
        else if (!strcmp(argv[i], "-audio_cdb") && i+1<argc) {
            opts->audioCdb = strtoul(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-probe")) {
            opts->probe = true;
        }
#endif
        else if (!strcmp(argv[i], "-compressed_audio")) {
            opts->compressedAudio = true;
        }
        else if (!strcmp(argv[i], "-multichannel_audio")) {
            opts->multichannelAudio = true;
        }
        else {
            return i;
        }
    }

    return 0;
}


int APETEST_Probe(APETEST_CommonOpts *opts, const char *filename, const char **indexname)
{
    int rc = 0;

    if (opts->probe) {
        /* use media probe to set values */
        bmedia_probe_t probe = NULL;
        bmedia_probe_config probe_config;
        const bmedia_probe_stream *stream = NULL;
        const bmedia_probe_track *track = NULL;
        bfile_io_read_t fd = NULL;
        bool foundAudio = false;
        FILE *fin;
        char stream_info[512];

        probe = bmedia_probe_create();

/*        fin = fopen64(filename,"rb"); */
        fin = fopen(filename,"rb"); 
        if (!fin) {
            printf("can't open media file '%s' for probing\n", filename);
            rc = -1;
            goto done;
        }

        fd = bfile_stdio_read_attach(fin);
        bmedia_probe_default_cfg(&probe_config);
        probe_config.file_name = filename;
        probe_config.type = bstream_mpeg_type_unknown;
        stream = bmedia_probe_parse(probe, fd, &probe_config);

        if(stream && stream->type == bstream_mpeg_type_cdxa) {
            bcdxa_file_t cdxa_file;
            bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
            printf( "Media Probe:\n" "%s\n\n", stream_info);
            cdxa_file = bcdxa_file_create(fd);
            if(cdxa_file) {
                const bmedia_probe_stream *cdxa_stream;
                cdxa_stream = bmedia_probe_parse(probe, bcdxa_file_get_file_interface(cdxa_file), &probe_config);
                bcdxa_file_destroy(cdxa_file);
                if(cdxa_stream) {
                    bmedia_probe_stream_free(probe, stream);
                    stream = cdxa_stream;
                    opts->cdxaFile = true;
                }
            }
        }
        /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
        bfile_stdio_read_detach(fd);

        fclose(fin);
        if(!stream) {
            printf("media probe can't parse stream '%s'\n", filename);
            rc = -1;
            goto done;
        }

        /* if the user has specified the index, don't override */
        if (indexname && !*indexname) {
            if (stream->index == bmedia_probe_index_available || stream->index == bmedia_probe_index_required) {
                *indexname = filename;
            }
        }


        bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
        printf(
            "Media Probe:\n"
            "%s\n\n",
            stream_info);

        if (opts->streamType == APETEST_INVALID)
        {
            opts->streamType = Bstream2BavcStream(stream->type);
        }

        for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
            switch(track->type) {
                case bmedia_track_type_audio:
                    if(track->info.audio.codec != baudio_format_unknown && !foundAudio) {
                        if (opts->audioPid == 0)
                        {
                            opts->audioPid = track->number;
                        }
                        if (opts->audioCodec == APETEST_INVALID)
                        {
                            opts->audioCodec = Baudio2BavcAudio(track->info.audio.codec);
                        }
                        foundAudio = true;
                    }
                    break;

                case bmedia_track_type_pcr:
                    opts->pcrPid = track->number;
                    break;
                default:
                    break;
            }
        }

#if B_HAS_AVI && 0
        if (stream->type == bstream_mpeg_type_avi && ((bavi_probe_stream *)stream)->video_framerate && opts->videoFrameRate==0) {
            NEXUS_LookupFrameRate(((bavi_probe_stream *)stream)->video_framerate, &opts->videoFrameRate);
        }
#endif

done:
        if (probe) {
            if (stream) {
                bmedia_probe_stream_free(probe, stream);
            }
            bmedia_probe_destroy(probe);
        }
    }

    return rc;
}





/* convert common opts to AvCtxCfg params, common opts utils in here */
BERR_Code APETEST_OptsToRaveSettings( 
    BXPT_RaveCx_Handle hCtx,    
    APETEST_CommonOpts *pCommon,
    BXPT_Rave_AvSettings *pAvCtxCfg    /* [out] The AV settings. */
    )
{
	RET_ERR( BXPT_Rave_GetAvConfig( hCtx, pAvCtxCfg ) ); 
    BKNI_Memset(pAvCtxCfg->EsRanges, 0, sizeof(pAvCtxCfg->EsRanges)); /* all disabled */
    pAvCtxCfg->BandHoldEn = true;
    pAvCtxCfg->DisableContinuityCheck = false;
    pAvCtxCfg->OutputFormat = BAVC_StreamType_eEs;

    pAvCtxCfg->InputFormat = pCommon->streamType;

    /* There are 4 types of DSS A/V streams:
    DSS SD video - this is DSS ES
    DSS HD video - this is DSS PES
    DSS MPEG audio - this actually uses MPEG1 system headers, but it's very similar to PES, therefore DSS PES
    Therefore we convert DSS ES to DSS PES here.
    DSS AC3 audio - uses MPEG2 System PES, therefore DSS PES
    */
    if (pAvCtxCfg->InputFormat == BAVC_StreamType_eDssEs)
    {
        pAvCtxCfg->InputFormat = BAVC_StreamType_eDssPes;
    }

    switch (pCommon->audioCodec) {
    case BAVC_AudioCompressionStd_eMpegL2:
    case BAVC_AudioCompressionStd_eMpegL3:
        if ( pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
        }
        else
        {
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eMpegAudio;
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eMpegL1:
        if (pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
        }
        else
        {
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eMpegAudioLayer3;
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        if (pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAacAudio;
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eAacPlusLoas:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
        /* baudio_format_aac_plus_loas is also handled here as it has the same value as baudio_format_aac_plus */
        if (pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAacHe;
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eDra:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eDra;
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;
    case BAVC_AudioCompressionStd_ePcm: /* PCM audio in PES uses the same BCMA header/encapsulation */
    case BAVC_AudioCompressionStd_ePcmWav:
    case BAVC_AudioCompressionStd_eWmaStd:
    case BAVC_AudioCompressionStd_eWmaStdTs:
    case BAVC_AudioCompressionStd_eWmaPro:
    case BAVC_AudioCompressionStd_eAmr:
    case BAVC_AudioCompressionStd_eAdpcm:
    case BAVC_AudioCompressionStd_eVorbis:
    case BAVC_AudioCompressionStd_eG711:
    case BAVC_AudioCompressionStd_eG726:
    case BAVC_AudioCompressionStd_eG729:
    case BAVC_AudioCompressionStd_eG723_1:
    case BAVC_AudioCompressionStd_eFlac:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eWma;
        pAvCtxCfg->StreamIdHi = 0xEF; /* PES stream id 0xCD is set by playpump ASF->PES packetizer.
                                    but allow a greater range. */
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eCook:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAmr;
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xC0;
        break;
    case BAVC_AudioCompressionStd_eAc3:
        if (pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
#if BCHP_CHIP != 7401 && BCHP_CHIP != 7403
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAc3Plus; /* Use AC3+ ITB type for AC3 */
#else
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAc3gAudio;
#endif
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;
    case BAVC_AudioCompressionStd_eAc3Plus:
        if (pCommon->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eAc3Plus;
        }
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;
    case BAVC_AudioCompressionStd_eMlp:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eMlpAudio;
        pAvCtxCfg->StreamIdHi = 0xEF;
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;
    case BAVC_AudioCompressionStd_eLpcmDvd:
    case BAVC_AudioCompressionStd_eLpcmHdDvd:
    case BAVC_AudioCompressionStd_eLpcmBd:
    case BAVC_AudioCompressionStd_eLpcm1394:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eLpcmAudio;
        pAvCtxCfg->StreamIdHi = 0xEF; /* TBD */
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;
    case BAVC_AudioCompressionStd_eDts:
    case BAVC_AudioCompressionStd_eDtshd:
    case BAVC_AudioCompressionStd_eDtsLegacy:
        pAvCtxCfg->ItbFormat = BAVC_ItbEsType_eDtsAudio;
        pAvCtxCfg->StreamIdHi = 0xEF; /* TBD */
        pAvCtxCfg->StreamIdLo = 0xBD;
        break;

    default:
        return BERR_NOT_SUPPORTED;
    }

    return BERR_SUCCESS;
}

#if 0
BERR_Code APETEST_OpenCaptureFile(const char * filename, FILE **fin)
{
        *fin = fopen(filename,"wb"); 
        if (!*fin) {
            printf("can't open media file '%s' for capture\n", filename);
            return BERR_NOT_SUPPORTED;
        }
    return BERR_SUCCESS;
}
#endif