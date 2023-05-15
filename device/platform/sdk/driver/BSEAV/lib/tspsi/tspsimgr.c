/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tspsimgr.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 2/15/11 2:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/tspsi/tspsimgr.c $
 * 
 * 22   2/15/11 2:25p nickh
 * SW7422-213: Add SVC/MVC types for PMT parsing
 * 
 * 21   4/26/10 1:20p erickson
 * SW7405-3942: linux 2.6.31 BKNI_Sleep(1) appears to take only 1
 * millisecond (as advertised). 400 BKNI_Sleep(1)'s is therefore not 4000
 * msec. So, we need to change to BKNI_Sleep(10).
 *
 * 20   3/3/10 12:01p vsilyaev
 * SW7405-3990: Added stream_type for DTS audio
 *
 * 19   11/23/09 5:51p katrep
 * SW7405-3457:Add DRA support
 *
 * 18   8/14/09 3:51p erickson
 * PR55994: fix #include for bsettop.h
 *
 * 17   1/20/08 10:59p katrep
 * PR38591: Added support for AVS streams.
 *
 * 16   10/15/07 4:04p vishk
 * PR 35263: IB-Converity (CID 420): OVERRUN_STATIC,
 *
 * 15   10/12/07 3:41p vishk
 * PR 35263: IB-Converity (CID 420): OVERRUN_STATIC,
 *
 * 14   7/10/07 3:01p erickson
 * PR31916: update tspsimgr for wider range of specs and codecs.
 * consolidated duplicate code.
 *
 * 13   1/18/07 12:08p ahulse
 * PR27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 *
 * 12   12/14/06 11:59a jjordan
 * PR26473: add get timeout capability
 *
 * 11   11/16/06 4:02p erickson
 * PR25993: remove bad/broken B_HAS_RAVE code. this is now defaulted in
 * bmessage_stream_params_init.
 *
 * 10   7/28/06 12:39p erickson
 * PR15309: must use filtergroups for RAVE platforms because multiple pid
 * channels for the same band and pid are not allowed.
 *
 * 9   6/13/06 2:40p erickson
 * PR18488: temporarily increasing timeout for 740x platforms
 *
 * 8   2/22/06 5:12p arbisman
 * PR19269: Add support for ISO 14496-3 audio type
 *
 * 7   8/25/05 4:25p erickson
 * PR16866: increase PAT and PMT default timeouts
 *
 * 6   8/12/05 3:28p erickson
 * PR16659: added more debug
 *
 * 5   8/10/05 5:00p erickson
 * PR16295: cleanup warning
 *
 * 4   8/10/05 11:38a erickson
 * PR16295: insert programs into CHANNEL_INFO_T using program_number for
 * consistency of scan order
 *
 * 3   3/15/05 2:52p erickson
 * PR13415: changed bmessage_format parameter, added some debug
 *
 * 2   2/17/05 5:13p erickson
 * PR14180: cleanup warning
 *
 * 1   2/7/05 11:34p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/11   2/7/05 4:58p erickson
 * PR13908: fix warning
 *
 * Irvine_BSEAVSW_Devel/10   2/7/05 4:35p erickson
 * PR13908: fix warnings
 *
 * Irvine_BSEAVSW_Devel/9   12/29/04 7:24p marcusk
 * PR13701: Updated with support for H.264 Stream Type (0xB1)
 *
 * Irvine_BSEAVSW_Devel/8   12/20/04 1:56p erickson
 * PR13595: removed error printout when you can't find the PAT
 *
 * Irvine_BSEAVSW_Devel/7   11/4/04 1:02p erickson
 * PR13141: added pmt pid to PMT callback
 *
 * Irvine_BSEAVSW_Devel/6   7/29/04 7:39a erickson
 * PR11682: no memcpy needed for parsing PMT information. Just call
 * bmessage_stop after processing data. Transport message overflows
 * aren't important and it will be quick anyway.
 *
 * Irvine_BSEAVSW_Devel/5   7/23/04 9:25a erickson
 * PR11682: debug cleanup
 *
 * Irvine_BSEAVSW_Devel/4   7/21/04 12:07p erickson
 * PR11682: refactored so that finer-grain control is available, without
 * breaking existing api
 *
 * Irvine_BSEAVSW_Devel/3   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/2   5/4/04 1:52p erickson
 * PR10938: added bounds checking and PMT and PAT validation in order to
 * prevent segfaults on bad data
 *
 * Irvine_BSEAVSW_Devel/12   2/5/04 11:54a erickson
 * PR9497: usleep is not portable, so use the magnum kernel interface
 *
 * Irvine_BSEAVSW_Devel/11   12/19/03 3:47p erickson
 * PR8731: need semicolon after BDBG_MODULE statement
 *
 * Irvine_BSEAVSW_Devel/10   12/8/03 4:16p marcusk
 * PR 8895: Properly pack filter bytes properly (filter should always be
 * passed in as byte ordered array)
 *
 * Irvine_BSEAVSW_Devel/9   11/24/03 10:17a erickson
 * PR8714: program numbers can be 16 bit and we were only filtering on the
 * lower 8 bits. This problem was introduced when converting to settop
 * api.
 *
 * Irvine_BSEAVSW_Devel/8   10/31/03 1:28p erickson
 * converted TODO into warning.
 *
 * Irvine_BSEAVSW_Devel/7   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/6   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/5   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/4   9/19/03 2:31p erickson
 * PR8074 - uninitialized memory
 *
 * Irvine_BSEAVSW_Devel/3   9/10/03 5:36p marcusk
 * Updated to use new path
 *
 * Irvine_BSEAVSW_Devel/2   9/10/03 5:12p erickson
 * various fixes, now works on 7328
 *
 * Irvine_BSEAVSW_Devel/1   9/10/03 4:33p marcusk
 * initial version
 *
 ****************************************************************/
#include "bstd.h"
#include "bsettop.h"
#include "tspsimgr.h"
#include "bsettop_message.h"
#include "ts_psi.h"
#include "ts_pat.h"
#include "ts_pmt.h"
BDBG_MODULE(tspsimgr);

#include "bkni.h"
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */

#define PSI_BFR_SIZE    TS_PSI_MAX_PSI_TABLE_SIZE

/* Note that the following timeouts are not 400 milliseconds.
It is 400 BKNI_Sleep(10)'s, which will be about 4000 ms (4 seconds) on linux.
Also, the original default of 200 was found to be too short for some customer streams. The downside
of having too large a timeout value is usually app delays when scanning non-existent streamers
or streams with no PSI information. Not critical. */
static int tsPsi_patTimeout = 400;

/* PR 18488 - temporarily increasing timeout for 740x platforms. */
static int tsPsi_pmtTimeout = 600;

static void tsPsi_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, PROGRAM_INFO_T *progInfo );
static void tsPsi_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EPID *ePidData );
static bresult tsPsi_getProgramMaps( bband_t band, const void *p_patBfr, unsigned pat_bfrSize,
    CHANNEL_INFO_T *p_chanInfo, bstream_t stream);
static void tsPsi_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize);

#if BDBG_DEBUG_BUILD
#define B_ERROR(ERR) (BDBG_ERR(("%s at line %d", #ERR, __LINE__)), (ERR))
#else
#define B_ERROR(ERR) (ERR)
#endif

int tsPsi_getPAT(bband_t band, void *p_bfr, unsigned bfrSize, bstream_t stream)
{
    bmessage_stream_t patStream;
    bmessage_stream_params params;
    int timeout;
    bresult result;
    unsigned patSize = 0;

    patStream = bmessage_open(bmessage_format_psi);
    if (!patStream) return -1;

    bmessage_stream_params_init(&params, patStream);
    params.band = band;
    params.stream = stream;
    params.pid = 0x0;

    result = bmessage_start(patStream, &params);
    if (result) return -1;

    BDBG_MSG(("Looking for PAT"));
    for( timeout = tsPsi_patTimeout; timeout > 0 && patSize == 0; timeout-- )
    {
        const void *buffer;
        if (bmessage_get_buffer(patStream, &buffer, &patSize))
            return -1;
        if (patSize) {
            if (patSize > bfrSize)
                patSize = bfrSize;
            BKNI_Memcpy(p_bfr, buffer, patSize);
            /* don't call bmessage_read_complete because we're stopping anyway */
        }
        else {
            BKNI_Sleep(10);
        }
    }
    bmessage_close(patStream);

    /* validate before returning it */
    if (patSize && !TS_PAT_validate(p_bfr, patSize)) {
        tsPsi_p_dumpBuffer(p_bfr, patSize);
        BDBG_WRN(("Invalid PAT data detected"));
        return -1;
    }

    return patSize;
}

bresult tsPsi_getChannelInfo(CHANNEL_INFO_T *p_chanInfo, bband_t band, bstream_t stream )
{
    uint8_t     pat[PSI_BFR_SIZE];
    size_t      patSize;
    TS_PSI_header header;

    /* Blocking call to get PAT */
    patSize = tsPsi_getPAT(band, pat, PSI_BFR_SIZE, stream);
    /* If there's no PAT, return but don't print an error because this can happen
    normally. */
    if (patSize <= 0)
        return berr_external_error;

    TS_PSI_getSectionHeader( pat, &header );
    p_chanInfo->version                 = header.version_number;
    p_chanInfo->transport_stream_id     = header.table_id_extension;
    p_chanInfo->num_programs            = 0;

    BDBG_MSG(("Parsing PAT"));
    return tsPsi_getProgramMaps( band, pat, patSize, p_chanInfo, stream );
}

#define ADD_VIDEO_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_video_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    BDBG_MSG(("  vpid[%d] 0x%x, type 0x%x", (P_INFO)->num_video_pids, (PID), (TYPE))); \
    (P_INFO)->video_pids[(P_INFO)->num_video_pids].pid = (PID); \
    (P_INFO)->video_pids[(P_INFO)->num_video_pids].streamType = (TYPE); \
    tsPsi_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->video_pids[(P_INFO)->num_video_pids] ); \
    (P_INFO)->num_video_pids++; \
    } \
    } while (0)
#define ADD_AUDIO_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_audio_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    BDBG_MSG(("  apid[%d] 0x%x, type 0x%x", (P_INFO)->num_audio_pids, (PID), (TYPE))); \
    (P_INFO)->audio_pids[(P_INFO)->num_audio_pids].pid = (PID); \
    (P_INFO)->audio_pids[(P_INFO)->num_audio_pids].streamType = (TYPE); \
    tsPsi_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->audio_pids[(P_INFO)->num_audio_pids] ); \
    (P_INFO)->num_audio_pids++; \
    } \
    } while (0)
#define ADD_OTHER_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_other_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    BDBG_MSG(("  opid[%d] 0x%x, type 0x%x", (P_INFO)->num_audio_pids, (PID), (TYPE))); \
    (P_INFO)->other_pids[(P_INFO)->num_other_pids].pid = (PID); \
    (P_INFO)->other_pids[(P_INFO)->num_other_pids].streamType = (TYPE); \
    tsPsi_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->other_pids[(P_INFO)->num_other_pids] ); \
    (P_INFO)->num_other_pids++; \
    } \
    } while (0)

void tsPsi_parsePMT( const void *pmt, unsigned pmtSize, PROGRAM_INFO_T *p_programInfo)
{
    int i;
    TS_PMT_stream pmt_stream;
    TS_PSI_header header;

    TS_PSI_getSectionHeader(pmt, &header );

    /* Store the main information about the program */
    p_programInfo->program_number   = header.table_id_extension;
    p_programInfo->version          = header.version_number;
    p_programInfo->pcr_pid          = TS_PMT_getPcrPid(pmt, pmtSize);

    /* find and process Program descriptors */
    tsPsi_procProgDescriptors(pmt, pmtSize, p_programInfo );

    /* Find the video and audio pids... */
    p_programInfo->num_video_pids   = 0;
    p_programInfo->num_audio_pids   = 0;
    p_programInfo->num_other_pids   = 0;

    for( i = 0; i < TS_PMT_getNumStreams(pmt, pmtSize); i++ )
    {
        int descIdx = 0;
        if (TS_PMT_getStream(pmt, pmtSize, i, &pmt_stream )) {
            BDBG_WRN(("Invalid PMT data detected"));
            continue;
        }

        switch( pmt_stream.stream_type )
        {
        /* video formats */
        case TS_PSI_ST_11172_2_Video:  /* MPEG-1 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg1, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_Video:   /* ATSC MPEG-2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_13818_2_Video: /* MPEG-2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_2_Video: /* MPEG-4 Part 2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg4_part2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_10_Video: /* H.264/AVC */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_10_AnnexG_Video: /* H.264/SVC */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264_svc, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_10_AnnexH_Video: /* H.264/MVC */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264_mvc, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_AVS_Video: /* AVS */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_avs, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_SMPTE_VC1:      /* VC-1 */
            /* need to parse descriptor and then subdescriptor to determine profile */
            for (;;) {
                TS_PSI_descriptor desc = TS_PMT_getStreamDescriptor(pmt, pmtSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                case TS_PSI_DT_Registration:
                    /* calculate and check format_identifier */
                    {
                    uint32_t format_identifier = (desc[2] << 24) + (desc[3] << 16) + (desc[4] << 8) + desc[5];
                    if (format_identifier == 0x56432D31) {
                        /* check that proper sub-descriptor exists */
                        int subdescriptor_tag = desc[6];
                        if (subdescriptor_tag == 0x01) {
                            int profile_level = desc[7];
                            if (profile_level >= 0x90)  /* Advanced Profile ES */
                                ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_vc1, pmt, pmtSize, i);
                            else /* Simple/Main Profile ES */
                                ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_vc1_sm, pmt, pmtSize, i);
                        }
                    }
                    }
                    break;
                default:
                    break;
                }
            }
            break;

        /* audio formats */
        case TS_PSI_ST_11172_3_Audio: /* MPEG-1 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case TS_PSI_ST_13818_3_Audio: /* MPEG-2 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case TS_PSI_ST_13818_7_AAC:  /* MPEG-2 AAC */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac, pmt, pmtSize, i);    /* Note baudio_format_aac = MPEG-2 AAC */
            break;
        case TS_PSI_ST_14496_3_Audio: /* MPEG-4 AAC */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i);   /* Note baudio_format_aac_plus = MPEG-4 AAC */
            break;
        case TS_PSI_ST_ATSC_AC3:      /* ATSC AC-3 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_EAC3:     /* ATSC Enhanced AC-3 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3_plus, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_DTS:     /* ASTC ??? DTS audio */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dts, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_AVS_Audio:     /* AVS */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_avs, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_DRA_Audio:     /* DRA */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dra, pmt, pmtSize, i);
            break;


        /* video or audio */
        case TS_PSI_ST_13818_1_PrivatePES:  /* examine descriptors to handle private data */
            for (;;) {
                TS_PSI_descriptor desc = TS_PMT_getStreamDescriptor(pmt, pmtSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                /* video formats */
                case TS_PSI_DT_VideoStream:
                    /* MPEG_1_only_flag is bit 2 of desc[2], this determines MPEG-1/2 */
                    if ((desc[2] & 0x04) == 1)
                        ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg1, pmt, pmtSize, i);
                    else
                        ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_MPEG4_Video:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg4_part2, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_AVC:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_AVS_Video:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_avs, pmt, pmtSize, i);
                    break;

                /* audio formats */
                case TS_PSI_DT_AudioStream:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);   /* Same baudio_format for MPEG-1 or MPEG-2 audio */
                    break;
                case TS_PSI_DT_MPEG2_AAC:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac, pmt, pmtSize, i);   /* Note baudio_format_aac = MPEG-2 AAC */
                    break;
                case TS_PSI_DT_MPEG4_Audio:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case TS_PSI_DT_DVB_AAC:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case TS_PSI_DT_DVB_AC3:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_EnhancedAC3:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3_plus, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_DTS:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dts, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_DRA:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dra, pmt, pmtSize, i);
                    break;
                default:
                    BDBG_MSG(("Unsupported private descriptor 0x%x",desc[0]));
                    break;
                }
            }
            break;
        default:
            if( p_programInfo->num_other_pids < MAX_PROGRAM_MAP_PIDS )
            {
                ADD_OTHER_PID(p_programInfo, pmt_stream.elementary_PID, pmt_stream.stream_type, pmt, pmtSize, i);
            }
            break;
        }
        /* If we get any data our status is complete! */
    } /* EFOR Program map loop */
}

static void tsPsi_p_addChanInfo(void *context, uint16_t pmt_pid, const void *pmt, unsigned size)
{
    CHANNEL_INFO_T *p_chanInfo = (CHANNEL_INFO_T*)context;
    PROGRAM_INFO_T programInfo;
    int i;

    BKNI_Memset(&programInfo, 0, sizeof(programInfo));

    /* If this isn't true, then the logic at the top of tsPsi_getPMTs has failed,
    or we haven't stopped each msgstream after reading one and only one item. */
    BDBG_ASSERT(p_chanInfo->num_programs < MAX_PROGRAMS_PER_CHANNEL);

    /* The "if" comparision below is present to silence the Coverity from choosing the false path in the above "BDBG_ASSERT" line of code. */
    if(p_chanInfo->num_programs < MAX_PROGRAMS_PER_CHANNEL){
        tsPsi_parsePMT(pmt, size, &programInfo);
        programInfo.map_pid = pmt_pid;

        /* now that we know the program_number, insert it into the array */
        for (i=0;i<p_chanInfo->num_programs;i++) {
            if (programInfo.program_number < p_chanInfo->program_info[i].program_number)
                break;
        }
        /* make room for an insertion */
        if (i < p_chanInfo->num_programs) {
            unsigned char *ptr = (unsigned char *)&p_chanInfo->program_info[i];
            BKNI_Memmove(ptr + sizeof(PROGRAM_INFO_T), ptr, sizeof(PROGRAM_INFO_T) * (p_chanInfo->num_programs - i));
        }
        /* now copy into place */
        BKNI_Memcpy(&p_chanInfo->program_info[i], &programInfo, sizeof(PROGRAM_INFO_T));
        p_chanInfo->num_programs++;
    }
}

static bresult  tsPsi_getProgramMaps( bband_t band, const void *p_patBfr, unsigned pat_bfrSize,
    CHANNEL_INFO_T *p_chanInfo, bstream_t stream )
{
    return tsPsi_getPMTs(band, p_patBfr, pat_bfrSize, tsPsi_p_addChanInfo, p_chanInfo, stream);
}

bresult tsPsi_getPMTs(bband_t band, const void *p_patBfr, unsigned pat_bfrSize,
    tsPsi_PMT_callback callback, void *context, bstream_t stream)
{
    bresult         result = b_ok;
    int             i;
    size_t          bfrSize;
    int             num_programs;
    int             num_programs_to_get;
    int             num_messages_received;
    bool            message_received;
    int             timeout;
    bmessage_stream_t   *pmtStreamArray;
    uint16_t        *pmt_pidArray;
    TS_PAT_program  program;
    int             curProgramNum = 0;

    num_programs = TS_PAT_getNumPrograms(p_patBfr);
    BDBG_MSG(("num_programs %d", num_programs));
    if( num_programs > MAX_PROGRAMS_PER_CHANNEL )
    {
        BDBG_WRN(("Maximum number of programs exceeded in tspsimgr: %d > %d",
            num_programs, MAX_PROGRAMS_PER_CHANNEL));
        num_programs = MAX_PROGRAMS_PER_CHANNEL;
    }

    pmtStreamArray = (bmessage_stream_t *)malloc( sizeof(bmessage_stream_t) * num_programs );
    memset(pmtStreamArray, 0, sizeof(bmessage_stream_t) * num_programs);

    pmt_pidArray = (uint16_t *)malloc( sizeof(uint16_t) * num_programs );
    memset(pmt_pidArray, 0, sizeof(uint16_t) * num_programs);

    result = TS_PAT_getProgram( p_patBfr, pat_bfrSize, curProgramNum, &program );
    curProgramNum++;

    if( result == b_ok )
    {
        while( num_programs )
        {
            /* Always try to read the max number of pids at the same time */
            num_programs_to_get = num_programs;
            num_messages_received = 0;

            for( i = 0; i < num_programs_to_get; i++ )
            {
                if( program.program_number == 0 )
                {
                    /* This is the network PID, so ignore it */
                    num_messages_received++;
                }
                else
                {
                    bmessage_stream_params params;

                    pmtStreamArray[i] = bmessage_open(bmessage_format_psi);
                    if( pmtStreamArray[i] == NULL )
                    {
                        /* Decrease the number of programs to get in this loop due to transport resources */
                        num_programs_to_get = i-1;

                        if( num_programs_to_get == 0 )
                        {
                            /* Abort due to not being able to enable messages */
/*                          BRCM_DBG_MSG(("Unable to enable any messages!")); */
                            num_programs = 0;
                        }
                        break;
                    }

                    bmessage_stream_params_init(&params, pmtStreamArray[i]);
                    params.band = band;
                    params.stream = stream;
                    params.pid = program.PID;
                    /* these fields must match */
                    params.filter.mask[0] = 0x00;
                    params.filter.mask[3] = 0x00;
                    params.filter.mask[4] = 0x00;

                    /* they must match these values */
                    params.filter.coef[0] = 0x02;
                    params.filter.coef[3] = (program.program_number & 0xFF00) >> 8;
                    params.filter.coef[4] = program.program_number & 0xFF;

                    BDBG_MSG(("filter pid %#x, program %#x", params.pid, program.program_number));

                    pmt_pidArray[i] = program.PID;
                    if (bmessage_start(pmtStreamArray[i], &params)) {
                        bmessage_close(pmtStreamArray[i]);
                        pmtStreamArray[i] = NULL;
                        pmt_pidArray[i] = 0;
                    }
                }

                /* We are finished with this program association so go to the next */

                /* TODO: Check for error */
                TS_PAT_getProgram( p_patBfr, pat_bfrSize, curProgramNum, &program );
                curProgramNum++;
            }

            /* Now we have enabled our pid channels, so wait for each one to get some data */
            timeout = tsPsi_pmtTimeout;
            while( num_messages_received != num_programs_to_get && timeout != 0 )
            {
                message_received = 0;
                /* Check each of the pid channels we are waiting for */
                for( i = 0; i < num_programs_to_get; i++ )
                {
                    const void *buffer;
                    if (pmtStreamArray[i] &&
                        !bmessage_get_buffer(pmtStreamArray[i], &buffer, &bfrSize ) &&
                        bfrSize)
                    {
                        /* don't call bmessage_read_complete because we're stopping anyway */

                        message_received = true;
                        num_messages_received++;

                        BDBG_MSG(("PMT: %d %d (%02x %02x %02x)", i, bfrSize,
                            ((unsigned char *)buffer)[0],((unsigned char *)buffer)[1],((unsigned char *)buffer)[2]));

                        if (!TS_PMT_validate(buffer, bfrSize)) {
                            BDBG_WRN(("Invalid PMT data detected: ch %d, bfrSize 0x%x", i, bfrSize));
                            tsPsi_p_dumpBuffer(buffer, bfrSize);
                        }
                        else {
                            /* Give the PMT to the callback */
                            (*callback)(context, pmt_pidArray[i], buffer, bfrSize);
                        }

                        /* cannot stop until after the data has been parsed because
                        we're not copying the data into a local buffer */
                        bmessage_stop(pmtStreamArray[i]);
                    }
                }
                if( !message_received )
                {
                    BKNI_Sleep(10);
                    timeout--;
                }
            }

            /* Now disable our pid channels */
            for( i = 0; i < num_programs_to_get; i++ )
            {
                if (pmtStreamArray[i])
                    bmessage_close(pmtStreamArray[i]);
            }
            num_programs -= num_programs_to_get;
        }
    }

    free( pmtStreamArray );
    free(pmt_pidArray);

    return result;
}

void tsPsi_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, PROGRAM_INFO_T *progInfo )
{
    int i;
    TS_PSI_descriptor descriptor;

    for( i = 0, descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i ) )
    {
        switch (descriptor[0])
        {
        case TS_PSI_DT_CA:
            progInfo->ca_pid = ((descriptor[4] & 0x1F) << 8) + descriptor[5];
            break;

        default:
            break;
        }
    }
}

void tsPsi_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EPID *ePidData )
{
    int i;
    TS_PSI_descriptor descriptor;

    for( i = 0, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i ) )
    {
        switch (descriptor[0])
        {
        case TS_PSI_DT_CA:
            ePidData->ca_pid = ((descriptor[4] & 0x1F) << 8) + descriptor[5];
            break;

        default:
            break;
        }
    }
}

void tsPsi_setTimeout( int patTimeout, int pmtTimeout )
{
    tsPsi_patTimeout = patTimeout;
    tsPsi_pmtTimeout = pmtTimeout;
}

void tsPsi_getTimeout( int *patTimeout, int *pmtTimeout )
{
    *patTimeout = tsPsi_patTimeout;
    *pmtTimeout = tsPsi_pmtTimeout;
}

static void tsPsi_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize)
{
    unsigned i;
    for (i=0;i<bfrSize;i++)
        BKNI_Printf("%02X", p_bfr[i]);
    BKNI_Printf("\n");
}
