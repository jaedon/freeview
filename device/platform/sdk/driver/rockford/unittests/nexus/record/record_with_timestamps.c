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
 * $brcm_Workfile: record_with_timestamps.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 8/31/11 2:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_with_timestamps.c $
 * 
 * Hydra_Software_Devel/6   8/31/11 2:13p jtna
 * SW7405-4989: general clean-up
 * 
 * Hydra_Software_Devel/5   7/29/10 12:07p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/4   6/17/09 4:39p jtna
 * PR55767: update
 * 
 * Hydra_Software_Devel/3   6/12/09 10:19a jtna
 * PR55767: default to using NEXUS_TransportTimestampType_eBinary
 * 
 * Hydra_Software_Devel/2   6/11/09 5:05p jtna
 * PR55767: added 2 more usage modes for playback
 * 
 * Hydra_Software_Devel/1   6/9/09 3:23p jtna
 * PR55767: added example to show record with timestamps
 * 
 *****************************************************************************/
/* Nexus example to record with timestamps. See comments above main() for options */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include <sys/types.h>
#include <sys/stat.h>

BDBG_MODULE(timestamps);

/* 

Terminology: 
    TS - transport stream, 188-byte packets
    TTS - timestamped transport stream, 192-byte packets

[Live decode]
NEXUS supports 3 cases:
1) take a TS source,  and record a TTS (with new timestamps)
2) take a TTS source, and record a TS  (strip the timestamps)
3) take a TTS source, and record a TTS (with new timestamps)

[Playback]
NEXUS supports 4 cases:
1) take a TS source,  and record a TTS (with new timestamps)   
    NOTE: for case 1) the new timestamps will reflect the rate that the packets entered the playback parser, 
          which is probably not the rate at which the stream should be played for proper decoding.
          The alternative is to route the playback output through a remux to a parser band.
          
2) take a TTS source, and record a TS  (strip the timestamps)
3) take a TTS source, and record a TTS (with same timestamps)
4) take a TTS source, and record a TTS (with new timestamps)
    NOTE: case 4) is similar to case 1)

-------------------------------------------------------------
[Live]
NEXUS_InputBandSettings.packetLength    sets    BXPT_InputBandConfig.IbPktLength
NEXUS_RecpumpSettings.timestampType     sets    BXPT_Rave_RecordSettings.TimestampMode

[Playback]
NEXUS_PlaypumpSettings.timestamp.type   sets    BXPT_Playback_ChannelSettings.TimestampMode
NEXUS_RecpumpSettings.timestampType     sets    BXPT_Rave_RecordSettings.TimestampMode

BXPT_ParserConfig.TsMode            appears to do nothing
BXPT_Playback_ParserConfig.TsMode   appears to do nothing

*/

#define LIVE_SOURCE 1  /* 1 if source is live. otherwise, source is playback */
#define TEST_CASE   1  /* corresponds to the above cases */

#define NUM_TIMESTAMPS 10 /* number of timestamps to print */

int get_timestamps(const char* fname, uint32_t *timestamps, int num, uint32_t offset, bool *isTTS);
void print_timestamps(uint32_t *timestamps, unsigned num);

int main(void) 
{
#if NEXUS_HAS_RECORD
    NEXUS_PlatformSettings platformSettings;
#if LIVE_SOURCE
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_InputBandSettings inputBandSettings;
    NEXUS_ParserBandSettings parserBandSettings;
#else
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_FilePlayHandle playfile;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaypumpSettings playpumpSettings;
#endif
    NEXUS_RecpumpSettings recpumpSettings;

    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    const char *fout = "videos/timestamps.mpg"; /* recording output */
#if (LIVE_SOURCE)
#if (TEST_CASE==1)
    /* live TS source */
    const unsigned vpid = 0x11;
    const unsigned apid = 0x14;
#else
    /* live TTS source */
    const unsigned vpid = 0x11;
    const unsigned apid = 0x14;
#endif

#else /* playback */
#if (TEST_CASE>1)
    /* playback TTS source */
    const char *fin = "videos/Joe_Kid_1min.tts"; 
    const unsigned vpid = 0x31;
    const unsigned apid = 0x34;
#else
    /* playback TS source */
    const char *fin = "videos/cnnticker.mpg"; 
    const unsigned vpid = 0x21;
    const unsigned apid = 0x22;
#endif
#endif

    uint32_t offset, timestamps[NUM_TIMESTAMPS];
    bool isTTS;
    unsigned rc;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

#if LIVE_SOURCE
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    NEXUS_InputBand_GetSettings(inputBand, &inputBandSettings);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);    
#else
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);
#endif

    NEXUS_Recpump_GetDefaultSettings(&recpumpSettings);

    /* set the timestamp settings */
#if LIVE_SOURCE
#if (TEST_CASE==1)   /* Live case 1) take a TS source,  and record a TTS (with new timestamps) */
    inputBandSettings.packetLength = 188;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eBinary;
#elif (TEST_CASE==2) /* Live case 2) take a TTS source, and record a TS  (strip the timestamps) */
    inputBandSettings.packetLength = 192;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eNone;
#else                /* Live case 3) take a TTS source, and record a TTS (with new timestamps) */
    inputBandSettings.packetLength = 192;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eBinary;
#endif
    BDBG_WRN(("Record from live: source=%s, output=%s", 
        inputBandSettings.packetLength==188 ? "TS":"TTS",
        recpumpSettings.timestampType==NEXUS_TransportTimestampType_eNone ? "TS":"TTS"));

#else /* playback */
#if (TEST_CASE==1)   /* Playback case 1) take a TS source,  and record a TTS (with new timestamps) */
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eNone;
    playpumpSettings.timestamp.forceRestamping = true;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eBinary;
#elif (TEST_CASE==2) /* Playback case 2) take a TTS source, and record a TS  (strip the timestamps) */
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eBinary;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eNone;
#elif (TEST_CASE==3) /* Playback case 3) take a TTS source, and record a TTS (with same timestamps) */
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eBinary;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eBinary;
#else                /* Playback case 4) take a TTS source, and record a TTS (with new timestamps) */
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eBinary;
    playpumpSettings.timestamp.forceRestamping = true;
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eBinary;
#endif
    BDBG_WRN(("Record from playback: source=%s, output=%s", 
        playpumpSettings.timestamp.type==NEXUS_TransportTimestampType_eNone ? "TS":"TTS",
        recpumpSettings.timestampType==NEXUS_TransportTimestampType_eNone ? "TS":"TTS"));
#endif

    /* apply the settings */
#if LIVE_SOURCE
    NEXUS_InputBand_SetSettings(inputBand, &inputBandSettings);
#else
    playfile = NEXUS_FilePlay_OpenPosix(fin, NULL);
    if (!playfile) {
        BDBG_ERR(("Cannot open file:%s", fin));
        return -1;
    }

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings = playpumpSettings;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);
#endif

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    recordSettings.recpumpSettings = recpumpSettings;
    NEXUS_Record_SetSettings(record, &recordSettings);

    file = NEXUS_FileRecord_OpenPosix(fout, NULL);

    /* video pid indexing is not relevant for this example; record without indexing */
#if LIVE_SOURCE
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, vpid, NULL);
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, apid, NULL);
#else
    pidChannel[0] = NEXUS_Playback_OpenPidChannel(playback, vpid, NULL);
    pidChannel[1] = NEXUS_Playback_OpenPidChannel(playback, apid, NULL);    
#endif
    NEXUS_Record_AddPidChannel(record, pidChannel[0], NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    NEXUS_Record_Start(record, file);
    /* Nexus is now recording to disk */

#if (!LIVE_SOURCE)
    NEXUS_Playback_Start(playback, playfile, NULL);
#endif
    BKNI_Sleep(10*1000);
    
#if (!LIVE_SOURCE)
    NEXUS_Playback_Stop(playback);
#endif
    NEXUS_Record_Stop(record);

    /* print timestamps of recording output */
    rc = get_timestamps(fout, timestamps, NUM_TIMESTAMPS, 0, &isTTS);
    BDBG_ASSERT(!rc);
    if (isTTS) {
        print_timestamps(timestamps, NUM_TIMESTAMPS);
    }
    offset = timestamps[0];

#if (!LIVE_SOURCE)
    if (playpumpSettings.timestamp.type!=NEXUS_TransportTimestampType_eNone) {
        /* print timestamps of source for comparison */
        rc = get_timestamps(fin, timestamps, NUM_TIMESTAMPS, offset, &isTTS);
        BDBG_ASSERT(!rc);
        print_timestamps(timestamps, NUM_TIMESTAMPS);
    }
#endif

    NEXUS_Record_Stop(record);
#if (!LIVE_SOURCE)
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(playfile);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
#endif
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_RemoveAllPidChannels(record);
#if LIVE_SOURCE
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
#endif
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Platform_Uninit();
#endif
    return 0;
}


/* extract timestamps from a TTS */
int get_timestamps(
    const char* fname, 
    uint32_t *timestamps, /* [out] array of timestamps */
    int num,              /* size of timestamps[] */
    uint32_t offset,      /* first element of timestamps[] will be greater than this value */
    bool *isTTS)
{
    FILE *f;
    int i, c, buf[5], count;
    bool done;
    uint32_t timestamp;
    struct stat sbuf;

    BKNI_Memset(timestamps, 0, sizeof(*timestamps)*num);

    if (stat(fname, &sbuf)) {
        BDBG_ERR(("%s does not exist", fname));
        return 1;
    }
    else {
        if (sbuf.st_size<num*192) {
            BDBG_ERR(("Filesize smaller than number of requested timestamps"));
            return 1;
        }
    }

    f = fopen(fname, "rb");
    BDBG_ASSERT(f);

    c = fgetc(f);
    if (c==0x47) { /* start by assuming TS, eg. 188byte/packet */
        count = 0;
        while (count++<num) {
            fseek(f, 187, SEEK_CUR);
            c = fgetc(f);
            if (c!=0x47) {
                BDBG_ERR(("Expected TS, but no sync at %ld", ftell(f)));
                return 1;
            }
        }
        *isTTS = false;
        BDBG_WRN(("%s: TS stream", fname));
        return 0;
    }
    else {
        *isTTS = true;
        fseek(f, 0, SEEK_SET);
    }
    
    /* assume TTS and try getting the timestamps */
    done = false;
    count = 0;
    do {
        for (i=0, c=0; i<5; i++) { /* read 5 bytes and store in buf */
            c=fgetc(f);
            if (c!=EOF) {
                buf[i]=c;
            }
            else {
                done = true;
                break;
            }
        }

        if (!done) {
            if (buf[4]==0x47) {
                /* we should have a timestamp */
                timestamp = buf[3] | buf[2]<<8 | buf[1]<<16 | buf[0]<<24;
                if (timestamp>=offset) {
                    timestamps[count++] = timestamp;
                }
                if (fseek(f, 187, SEEK_CUR) || feof(f) || count>=num) {
                    break;
                }
            }
            else {
                BDBG_ERR(("Out of sync: Expected sync byte at pos %ld. Resyncing...", ftell(f)));
                done = true;
                while ((c=fgetc(f))!=EOF) {
                    if (c==0x47) {
                        /* simplistic resyncing */
                        fseek(f, -5, SEEK_CUR);
                        done = false;
                        break;
                    }
                }
            }
        }
    } while (!done);

    BDBG_WRN(("%s: TTS stream", fname));
    fclose (f);
    return 0;
}

void print_timestamps(uint32_t *timestamps, unsigned num)
{
    unsigned i;
    printf("Timestamps:\n");
    for (i=0; i<num; i++) {
        printf("%#x ", timestamps[i]);
        if ((i+1)%10==0) printf("\n");
    }
    printf("\n");
}

