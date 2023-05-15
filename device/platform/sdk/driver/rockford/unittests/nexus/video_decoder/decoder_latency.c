/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: decoder_latency.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 5/18/12 1:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/decoder_latency.c $
 * 
 * Hydra_Software_Devel/12   5/18/12 1:21p vsilyaev
 * SW7425-2280: Added more options
 * 
 * Hydra_Software_Devel/11   5/18/12 1:03p vsilyaev
 * SW7425-2280: Print statistics per 100 frames instead of raw per-frame
 * data
 * 
 * Hydra_Software_Devel/10   4/20/12 2:25p vsilyaev
 * SW7425-2280: Fixed typo
 * 
 * Hydra_Software_Devel/9   2/14/12 4:59p vsilyaev
 * SW7425-2280: Avoid duplicate timestamps per video frame
 * 
 * Hydra_Software_Devel/8   2/14/12 3:27p vsilyaev
 * SW7425-2280: Added option to use TSM to drop late (stale) pictures
 * 
 * Hydra_Software_Devel/7   2/14/12 3:19p vsilyaev
 * SW7425-2280: Added option  for constant bitrate stream
 * 
 * Hydra_Software_Devel/6   2/14/12 1:59p vsilyaev
 * SW7425-2280: Enabled code to insert Access Unit Delimeter
 * 
 * Hydra_Software_Devel/5   2/8/12 12:24p vsilyaev
 * SW7425-2280: Improved debug output
 * 
 * Hydra_Software_Devel/4   2/8/12 12:04p vsilyaev
 * SW7425-2280: Added run-time option for zeroDelayOutputMode, improved
 * debug output
 * 
 * Hydra_Software_Devel/3   1/27/12 12:04p vsilyaev
 * SW7425-2280: Improved gating of debug output
 * 
 * Hydra_Software_Devel/2   1/27/12 11:30a vsilyaev
 * SW7425-2280: Added more debugging
 * 
 * Hydra_Software_Devel/1   1/26/12 5:23p vsilyaev
 * SW7425-2280: Utility to measure latency (delay) of video decoder
 * 
 *
 *****************************************************************************/
/* Nexus otfpvr unit test app: playback and trickmoes */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#include "blst_squeue.h"


#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(decoder_latency);

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#define MAX_READ (188*256)
struct time_entry {
    BLST_SQ_ENTRY(time_entry) link; 
    unsigned frame_no;
    struct timeval time;
};

BLST_SQ_HEAD(time_queue, time_entry);

typedef struct bdelay_statistics {
	double sum;
	double square_sum;
	int min;
	int max;
	unsigned n;
} bdelay_statistics;

void
bdelay_statistics_clear(bdelay_statistics *stats)
{
	stats->n = 0;
	stats->square_sum = 0.0;
	stats->sum = 0;
	stats->min = 100000;
	stats->max = 0;
}

void
bdelay_statistics_add(bdelay_statistics *stats, const int *data, size_t nitems)
{
	double sum = stats->sum;
	double square_sum = stats->square_sum;
	int min = stats->min;
	int max = stats->max;
	unsigned i;
	int d;

	for (i = 0; i < nitems; i++) {
		d = data[i];
		sum += (float)d;
		square_sum += (float) d * (float)d;
		if (min > d) {
			min = d;
		}
		if (max < d) {
			max = d;
		}
	}
	stats->sum = sum;
	stats->square_sum = square_sum;
	stats->min = min;
	stats->max = max;
	stats->n += nitems;

	return;
}

int
bdelay_statistics_mean(const bdelay_statistics *stats)
{
	if (stats->n == 0) {
		BDBG_ERR(("bdelay_statistics_mean: no data"));
		return 0;
	} else {
		double mean = stats->sum/stats->n;

		return (int)(floor(mean + 0.5));
	}
}

int
bdelay_statistics_deviation(const bdelay_statistics *stats)
{
	if (stats->n == 0) {
		BDBG_ERR(("bdelay_statistics_deviation: no data"));
		return 0;
	} else {
		double n = stats->n;
		double mean = stats->sum/n;
		double deviation = stats->square_sum/n - mean*mean;

		if (deviation > 0) {
			deviation = sqrt(deviation);
		} else {
			deviation = 0;
		}
		return (int)(floor(deviation + 0.5));
	}
}

int
bdelay_statistics_min(const bdelay_statistics *stats)
{
	return stats->min;
}

int
bdelay_statistics_max(const bdelay_statistics *stats)
{
	return stats->max;
}


#define MAX_FRAMES  64
struct time_info {
    struct time_queue inflight;
    struct time_queue available;
    struct time_entry data[MAX_FRAMES];
    bdelay_statistics stats;
    unsigned stats_count;
};


static void time_info_init(struct time_info *t)
{
    unsigned i;
    BLST_SQ_INIT(&t->inflight);
    BLST_SQ_INIT(&t->available);
    for(i=0;i<MAX_FRAMES;i++) {
        BLST_SQ_INSERT_TAIL(&t->available, t->data+i, link);
    }
    t->stats_count = 0;
    bdelay_statistics_clear(&t->stats);
    return;
}

static void time_info_insert(struct time_info *t, unsigned frame_no)
{
    struct time_entry *e;

    e = BLST_SQ_FIRST(&t->available);
    BDBG_ASSERT(e);
    e->frame_no = frame_no;
    BLST_SQ_REMOVE_HEAD(&t->available, link);
    gettimeofday(&e->time, NULL);
    BLST_SQ_INSERT_TAIL(&t->inflight, e, link);
    /* BDBG_LOG(("insert: %u:%u", e->time.tv_sec,  e->time.tv_usec)); */
    return;
}

static void time_diff(struct timeval *diff, const struct timeval *now, const struct timeval *then)
{
    diff->tv_sec = now->tv_sec - then->tv_sec;
    diff->tv_usec = now->tv_usec - then->tv_usec;
    if(diff->tv_usec < 0) {
        diff->tv_sec --;
        diff->tv_usec += 1000000;
    }
    return;
}


static void time_info_match(struct time_info *t, unsigned frame_no)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    for(;;) {
        struct time_entry *e = BLST_SQ_FIRST(&t->inflight);
        if(e==NULL) {
            break;
        }
        if(e->frame_no > frame_no) {
            break;
        }
        BLST_SQ_REMOVE_HEAD(&t->inflight, link);
        BLST_SQ_INSERT_HEAD(&t->available, e, link);
        /* BDBG_LOG(("match: %u:%u %u,%u", e->time.tv_sec,  e->time.tv_usec, now.tv_sec, now.tv_usec)); */
        if(e->frame_no == frame_no) {
            struct timeval diff;
            int diff_sample;
            time_diff(&diff, &now, &e->time);
            BDBG_MSG(("frame:%u delay:%u", frame_no, diff.tv_usec/1000));
            diff_sample = diff.tv_usec/1000;
            bdelay_statistics_add(&t->stats, &diff_sample, 1);
            if(t->stats_count && t->stats_count%100==0) {
                BDBG_LOG(("%u mean:%d dev:%d min:%d, max:%d\n", t->stats_count, bdelay_statistics_mean(&t->stats), bdelay_statistics_deviation(&t->stats), bdelay_statistics_min(&t->stats), bdelay_statistics_max(&t->stats)));
                bdelay_statistics_clear(&t->stats);
            }
            t->stats_count++;
            break;
        }
    }
}

static int
send_data(const uint8_t *buf, size_t bytes, NEXUS_PlaypumpHandle playpump, BKNI_EventHandle event)
{
    while(bytes>0) {
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc=NEXUS_SUCCESS;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            fprintf(stderr, "NEXUS_Playpump_GetBuffer failed");
            return -1;
        }
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            if(rc==BERR_TIMEOUT) {
                fprintf(stderr, "send_data:timeout\n");
            }
            continue;
        }
        if(buffer_size>bytes) {
            buffer_size = bytes;
        }
        /* fprintf(stderr, "buffer_size %u\n", buffer_size); */
        BKNI_Memcpy(buffer, buf, buffer_size);
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);
        bytes-=buffer_size;
        buf += buffer_size;
    }
    return 0;
}

static int
read_mpeg2_frame(FILE *fin, uint8_t *buf, size_t bufsize)
{
    uint32_t scode=0xFFFFFF;
    int i=0;

    for(;;) {
        int ch = fgetc(fin);
        if(ch<0) {
            return ch;
        }
        if((unsigned)i>=bufsize) {
            return -1;
        }
        buf[i++] = ch;
        scode = (scode << 8) | ch;
        if(scode == 0x100) { /* picture scode */
            return i;
        }
    }
}

/* low level bit manipulation */
/* 
Summary:
Returns bit 'b' from word 'w', 

Example:
    B_GET_BIT(0xDE,1)==0 
    B_GET_BIT(0xDE,7)!=0 
*/
#define B_GET_BIT(w,b)  ((w)&(1<<(b)))

/* 
Summary:
Returns bits 'e'..'b' from word 'w', 

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E 
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

/* 
Summary:
Sets bit 'b 'to value of 'v', 'name' is ignored (used as inline comment).
'v' is converted to bool (0/1) before setting bit, e.g. v with values of 1 or 1000 would generate the same result 

Example:
	B_SET_BIT("example 1",1,0)==1
	B_SET_BIT("example 2",1,2)==4
	B_SET_BIT("example 3",100,2)==4
*/

#define B_SET_BIT(name,v,b)  (((unsigned)((v)!=0)<<(b)))

/* 
Summary:
Sets bit 'b'...'e' to value of 'v', name is ignored (used as inline comment).
'v' is not truncated, e.g. it's the user responsibilty that it would feet into the given number of bits

Example:
	B_SET_BITS("example 1",1,0,0)==1
	B_SET_BITS("example 2",1,1,0)==1
	B_SET_BITS("example 3",0x55,7,1)==0xAA
*/

#define B_SET_BITS(name,v,e,b)  (((unsigned)(v))<<(b))

static int
read_avc_frame(FILE *fin, uint8_t *buf, size_t bufsize)
{
    uint32_t scode=0xFFFFFF;
    int i=0;

    for(;;) {
        int ch = fgetc(fin);
        if(ch<0) {
            return ch;
        }
        if((unsigned)i>=bufsize) {
            return -1;
        }
        buf[i++] = ch;
        scode = (scode << 8) | ch;
        if((scode&0xFFFFFF00)==0x100) { /* NAL */
            unsigned sc = scode & 0xFF;
            unsigned nal_unit_type = B_GET_BITS(sc, 4, 0);
            BDBG_MSG(("avc nal:%#x type:%u", scode, nal_unit_type));
            if(!B_GET_BIT(sc,7) && (nal_unit_type==1 || nal_unit_type==5) ) { /* valid NON-IDR or IDR slice */
#if 0
                ch = fgetc(fin); /* peek into the next byte */
                if(ch<0) {
                    return ch;
                }
                ungetc(ch, fin);
                BDBG_MSG(("nal payload %#x", ch)); 
                if(B_GET_BIT(ch, 7)==0) /* first_mb_in_slice == 0 */ 
#endif
                    /* assume one slice per picture */
                {
                    return i;
                }
            }
        }
    }
}

#define  WITH_PTS    1
#if WITH_PTS
static size_t 
pes_header_init(uint8_t *pes_header, size_t length, uint32_t pts)
{
	unsigned off;
	uint8_t ptd_dts_flag;

	pes_header[0] = 0;
	pes_header[1] = 0;
	pes_header[2] = 1;
	pes_header[3] = 0xE0;

    ptd_dts_flag = 0x2;	

	pes_header[6] = 
		B_SET_BITS( "10", 2, 7, 6 ) |
		B_SET_BITS(PES_scrambling_control, 0, 5, 4) |
		B_SET_BIT(PES_priority, 0, 3) |
		B_SET_BIT(data_alignment_indicator,/*frame_boundary*/ false, 2) |
		B_SET_BIT(copyright, 0, 1) |
		B_SET_BIT(original_or_copy, 1, 0);
	pes_header[7] = 
		B_SET_BITS(PTS_DTS_flags, ptd_dts_flag, 7, 6) |
		B_SET_BIT(ESCR_flag, 0, 5) |
		B_SET_BIT(ES_rate_flag, 0, 4) |
		B_SET_BIT(DSM_trick_mode_flag, 0, 3) |
		B_SET_BIT(additional_copy_info_flag, 0, 2) |
		B_SET_BIT(PES_CRC_flag, 0, 1) |
		B_SET_BIT(PES_extension_flag, false, 0);

    pes_header[9] = 
        B_SET_BITS("0010", ptd_dts_flag, 7, 4) |
        B_SET_BITS("PTS [32..30]", B_GET_BITS(pts,31,29), 3, 1) |
        B_SET_BIT(marker_bit, 1, 0);
    pes_header[10] = B_GET_BITS(pts,28,21); /* PTS [29..15] -> PTS [29..22] */
    pes_header[11] = 
        B_SET_BITS("PTS [29..15] -> PTS [21..15]", B_GET_BITS(pts,20,14), 7, 1) |
        B_SET_BIT(marker_bit, 1, 0);
    pes_header[12] = B_GET_BITS(pts,13,6); /* PTS [14..0] -> PTS [14..7]  */
    pes_header[13] = 
        B_SET_BITS("PTS [14..0] -> PTS [6..0]", B_GET_BITS(pts,5,0), 7, 2) |
        B_SET_BIT("PTS[0]", 0, 1) |
        B_SET_BIT(marker_bit, 1, 0);

    off = 14;


	pes_header[8] = off - 9;
	length = 0;
	pes_header[4] = B_GET_BITS(length, 15, 8);
	pes_header[5] = B_GET_BITS(length, 7, 0);

	return off;
}
#endif


int main(int argc, char **argv)  {
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/cnnticker.es";
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    uint16_t videoPid;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    struct time_info time_info;
    unsigned frame_no, decoder_frame_no;
    uint8_t buf[256*1024];
    int (*read_frame)(FILE *, uint8_t *, size_t );
    bool zeroDelay = false;
    bool insertAud = false;
    bool constantFrameRate = false;
    bool useTsmForSkip = false;
    bool useEarlyPictureDelivery = false;
    bool useHighPlayrate = false;
    unsigned frame_rate = 60;
    struct timeval first_frame_time;
    NEXUS_Error rc;

    if (argc > 1) {
        fname = argv[1];
    }
    if (argc > 3) {
        zeroDelay = atoi(argv[3]) != 0;
    }
    if (argc > 4) {
        insertAud = atoi(argv[4]) != 0;
    }
    if (argc > 5) {
        constantFrameRate = atoi(argv[5]) != 0;
    }
    if (argc > 6) {
        useTsmForSkip = atoi(argv[6]) != 0;
    }
    if (argc > 7) {
        useHighPlayrate = atoi(argv[7]) != 0;
    }
    if (argc > 8) {
        useEarlyPictureDelivery = atoi(argv[8]) != 0;
    }
    if (argc > 9) {
        frame_rate = atoi(argv[9]);
    }
   /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    if(useHighPlayrate) {
        platformSettings.transportModuleSettings.maxDataRate.playback[0] = 108 * 1000 * 1000;
    }
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.transportType = NEXUS_TransportType_eEs;
#if WITH_PTS
    playpumpSettings.transportType = NEXUS_TransportType_eMpeg2Pes;
#endif
    if(useHighPlayrate) {
        playpumpSettings.maxDataRate = platformSettings.transportModuleSettings.maxDataRate.playback[0];
    }
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings); /* take default capabilities */
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings); 
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    /* BKNI_Sleep(500); */

    videoPid = 0x1;
#if WITH_PTS
    videoPid = 0xE0;
#endif

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, videoPid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    if(argc > 2) {
        videoProgram.codec = atoi(argv[2]);
    }
    switch(videoProgram.codec) {
    case NEXUS_VideoCodec_eMpeg2: read_frame = read_mpeg2_frame; break;
    case NEXUS_VideoCodec_eH264: read_frame = read_avc_frame; break;
    default:
        fprintf(stderr, "Unsupported codec %u\n", videoProgram.codec); return -1;
    }
    videoProgram.pidChannel = videoPidChannel;
    if(useTsmForSkip) {
        NEXUS_StcChannelSettings stcSettings;
        videoProgram.stcChannel = stcChannel;
        NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
        stcSettings.mode = NEXUS_StcChannelMode_eHost;
        stcSettings.modeSettings.host.transportType = NEXUS_TransportType_eMpeg2Pes;
        rc=NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
    } else {
        videoProgram.stcChannel = NULL;
    }
    BDBG_LOG(("zeroDelayOutputMode: %s", zeroDelay?"enabled":"disabled"));
    BDBG_LOG(("insertion of Access Unit Delimeter: %s", insertAud?"enabled":"disabled"));
    BDBG_LOG(("insert frames at constant rate: %s", constantFrameRate?"enabled":"disabled"));
    BDBG_LOG(("use TSM to drop late pictures: %s", useTsmForSkip?"enabled":"disabled"));
    BDBG_LOG(("use high playback rate: %s", useHighPlayrate?"enabled":"disabled"));
    BDBG_LOG(("use early picture delivery: %s", useEarlyPictureDelivery?"enabled":"disabled"));

    if(zeroDelay || useEarlyPictureDelivery) {
        NEXUS_VideoDecoderExtendedSettings extendedSettings;
        NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &extendedSettings);
        extendedSettings.zeroDelayOutputMode = zeroDelay;
        extendedSettings.earlyPictureDeliveryMode = useEarlyPictureDelivery;
        NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &extendedSettings);
    }

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    time_info_init(&time_info);
    BKNI_Memset(buf, 0, sizeof(buf));
    gettimeofday(&first_frame_time, NULL);
    if(useTsmForSkip) {
        rc = NEXUS_StcChannel_SetStc(stcChannel, 0);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
    }
    for(frame_no=0, decoder_frame_no=0;;frame_no++) {
        NEXUS_VideoDecoderStatus vstatus;
        int rc;
        unsigned pes_offset = 0;
        unsigned i;
        unsigned frame_size;
        unsigned lastFifoDepth=0, lastCabacBinDepth=0, lastQueueDepth=0;
        unsigned frame_time = (frame_no*(uint64_t)45000) / frame_rate;
        unsigned next_frame_time = ((frame_no+1)*(uint64_t)45000) / frame_rate;
        unsigned diff_ms = 0;
        struct timeval current_time;
        struct timeval diff;
#if WITH_PTS
        pes_offset = 14;
#endif

        if(frame_no==0) {
            rc = read_frame(file, buf+pes_offset, sizeof(buf)-pes_offset); 
        } else {
            rc = read_frame(file, buf+4+pes_offset, sizeof(buf)-4-pes_offset); 
        }

        BDBG_MSG(("rc = %d", rc)); 
        if(rc<0) {
            fseek(file,0, SEEK_SET);
            break;
            continue;
        }
        if(frame_no==0) {
            BDBG_ASSERT(rc>=4);
            rc-=4;
        }
        frame_size = rc;
#if WITH_PTS
        if(frame_no==0) {
            /* send stream metadata with PTS of the first frame */
            pes_header_init(buf, 0, next_frame_time);
        } else if (frame_no==1) {
            /* don't sent PES header (and PTS) for the first frame */
            BKNI_Memset(buf, 0, pes_offset);
        } else {
            /* send PTS with all other frames */
            pes_header_init(buf, 0, frame_time);
        }
#endif
        gettimeofday(&current_time, NULL);
        time_diff(&diff, &current_time, &first_frame_time);
        diff_ms = diff.tv_sec * 1000 + diff.tv_usec/1000;
        BDBG_MSG(("frame [%u] %02x%02x%02x%02x pts:%u(%u) time:%u delayed:%u",  frame_no, buf[pes_offset], buf[pes_offset+1], buf[pes_offset+2], buf[pes_offset+3], frame_time, frame_time/45, diff_ms, diff_ms-frame_time/45));
        send_data(buf, rc+pes_offset, playpump, event); /* send previous start code */
        if(insertAud && videoProgram.codec == NEXUS_VideoCodec_eH264) {
            static const uint8_t nal_aud[188] = {0,0,1,0x9, 0,0,1,0x9}; /* need to send to AUD since one is buffered in RAVE */
            send_data(nal_aud, sizeof(nal_aud), playpump, event);  
        }
        if(1) {
            static const uint8_t trailer[2*188] = {0,0,0,0};
            send_data(trailer, sizeof(trailer), playpump, event);  /* insert trailer data  to flush transport buffers*/
        }
        BKNI_Memcpy(buf+pes_offset, buf+rc+pes_offset, 4); /* copy start code at front of the buffer */
        if(frame_no!=0) {
            /* first frame is not really an frame, so ignore it */
            time_info_insert(&time_info, frame_no);
        }
        for(i=0;i<17;i++) { 
            unsigned new_decoder_frame_no=decoder_frame_no;

            rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &vstatus);
            if(rc!=NEXUS_SUCCESS) {
                continue;
            }
#if WITH_PTS
            if(vstatus.ptsType == NEXUS_PtsType_eCoded || vstatus.ptsType == NEXUS_PtsType_eInterpolatedFromValidPTS) {
                new_decoder_frame_no = (vstatus.pts * (uint64_t)frame_rate)/45000;
            }
#else
            new_decoder_frame_no = vstatus.numDisplayed;
#endif
            if(frame_no!=0) {
                if(decoder_frame_no != new_decoder_frame_no) {
                    decoder_frame_no = new_decoder_frame_no;
                    time_info_match(&time_info, decoder_frame_no);
                }
                if(i==0 || lastFifoDepth != vstatus.fifoDepth || lastCabacBinDepth != vstatus.cabacBinDepth || lastQueueDepth != vstatus.queueDepth) {
                    BDBG_MSG(("[%u] %u:cdb:%u(%d) cabac:%u queue:%u pts:%u", frame_no, i, vstatus.fifoDepth, frame_size, vstatus.cabacBinDepth, vstatus.queueDepth, vstatus.pts)); 
                }
                lastCabacBinDepth = vstatus.cabacBinDepth;
                lastFifoDepth = vstatus.fifoDepth;
                lastQueueDepth = vstatus.queueDepth;
            }
            gettimeofday(&current_time, NULL);
            time_diff(&diff, &current_time, &first_frame_time);
            diff_ms = diff.tv_sec * 1000 + diff.tv_usec/1000;
            if(diff_ms >= next_frame_time/45) {
                if(constantFrameRate) {
                    break; /* keep constant framerate */
                }
            }
            BKNI_Sleep(1); 
        }
        BDBG_MSG(("[%u] numDisplayed:%u pts:%u(%u) cdb:%u cabac:%u queue:%u time[%u/%u/%u] %u ", frame_no, vstatus.numDisplayed, vstatus.pts, vstatus.pts/45, vstatus.fifoDepth, vstatus.cabacBinDepth, vstatus.queueDepth, diff_ms, frame_time, (frame_time - vstatus.pts)/45, i)); 
    }

    /* loops forever */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playpump_Stop(playpump);
    NEXUS_Playpump_Close(playpump); 
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    /* NEXUS_Platform_Uninit(); */
    return 0;

error:
    return 1;
}


