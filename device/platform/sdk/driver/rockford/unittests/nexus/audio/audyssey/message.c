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
 * $brcm_Workfile: message.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/10 1:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/audio/audyssey/message.c $
 * 
 * Hydra_Software_Devel/1   6/22/10 1:52p nvu
 * SW3548-2991: submit Audyssey certification application.
 * 
 * 4   12/8/09 3:00p gmohile
 * SW7408-1 : Add support for platforms with no frontend
 * 
 * 3   9/18/09 4:30p erickson
 * SWDEPRECATED-3717: use NEXUS_MessageSettings.bufferSize
 *
 * 2   5/4/09 4:25p erickson
 * PR54807: add basic psip support
 *
 * 1   5/4/09 2:38p erickson
 * PR54807: added message util
 *
 * 7   4/13/09 3:31p erickson
 * PR54058: remove NEXUS_PidChannelSettings.requireMessageBuffer
 *
 * 6   3/16/09 3:01p erickson
 * PR35457: use app-allocated buffer as recommended in Nexus_Usage.pdf
 *
 * 5   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 * 4   4/23/08 12:18p erickson
 * PR35457: parse the PAT to make it a more complete example
 *
 * 3   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 *
 * 2   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"
#include "nexus_playback.h"
#include "nexus_file.h"

#include "bstd.h"
#include "bkni.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(message);

void message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

NEXUS_PlaybackHandle playback;
NEXUS_ParserBand parserBand;
BKNI_EventHandle event;

#define MAX_MESSAGE_FILTERS 100
struct message_t {
    unsigned short pid;
    NEXUS_MessageHandle message;
    NEXUS_PidChannelHandle pidChannel;
    bool done;
} g_message[MAX_MESSAGE_FILTERS];

struct message_opts_t {
    unsigned short pid;
    unsigned buffer_size;
    const char *filename;
    bool hex;
    bool ascii;
    bool psi;
    bool psip;
    NEXUS_TransportType transportType;
    NEXUS_TransportTimestampType tsTimestampType;
    NEXUS_PlaybackLoopMode endOfStreamAction;
};

void cmdline_default_message_opts(struct message_opts_t *opts)
{
    BKNI_Memset(opts, 0, sizeof(*opts));
    opts->buffer_size = 4096;
    opts->transportType = NEXUS_TransportType_eTs;
    opts->tsTimestampType = NEXUS_TransportTimestampType_eNone;
    opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
}

int add_message_filter(const struct message_opts_t *opts, unsigned short pid)
{
    unsigned i;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_Error rc;
    struct message_t *msg = NULL;

    for (i=0;i<MAX_MESSAGE_FILTERS;i++) {
        if (!g_message[i].message) {
            msg = &g_message[i];
        }
    }
    if (!msg) {
        return -1;
    }

    if (playback) {
        msg->pidChannel = NEXUS_Playback_OpenPidChannel(playback, pid, NULL);
    }
    else {
        msg->pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
    }
    BDBG_ASSERT(msg->pidChannel);

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = message_callback;
    settings.dataReady.context = event;
    settings.maxContiguousMessageSize = 4096;
    settings.bufferSize = opts->buffer_size;
    msg->message = NEXUS_Message_Open(&settings);

    NEXUS_Message_GetDefaultStartSettings(msg->message, &startSettings);
    startSettings.pidChannel = msg->pidChannel;
    rc = NEXUS_Message_Start(msg->message, &startSettings);
    BDBG_ASSERT(!rc);

    return 0;
}

void print_usage(void)
{
    printf(
    "Usage: nexus message [options] [pvrfile]\n"
    "-buffer_size X   message capture buffer size in bytes\n"
    "-pid X           PID to filter (default is 0)\n"
    "-hex             print contents of all messages in hex\n"
    "-ascii           print contents of all messages in ascii (useful for PSIP)\n"
    "-psi             parse PSI tables\n"
    "-psip            parse PSIP tables\n"
    "\n"
    "If pvrfile not given, streamer input will be parsed\n"
    );
}

int cmdline_parse_message_opts(int argc, char **argv, struct message_opts_t *opts)
{
    int curarg = 1;

    while (curarg < argc) {
        if (!strcmp("--help", argv[curarg]) ||
            !strcmp("-h", argv[curarg]) ||
            !strcmp("-?", argv[curarg])) {
            print_usage();
            return 1;
        }
        else if (!strcmp("-buffer_size", argv[curarg]) && curarg+1 < argc) {
            opts->buffer_size = atoi(argv[++curarg]);
        }
        else if (!strcmp("-pid", argv[curarg]) && curarg+1 < argc) {
            opts->pid = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp("-hex", argv[curarg])) {
            opts->hex = true;
        }
        else if (!strcmp("-ascii", argv[curarg])) {
            opts->ascii = true;
        }
        else if (!strcmp("-psi", argv[curarg])) {
            opts->psi = true;
            opts->pid = 0; /* PID for PAT */
        }
        else if (!strcmp("-psip", argv[curarg])) {
            opts->psip = true;
            opts->pid = 0x1ffb; /* PID for MGT */
        }
        else if (!opts->filename) {
            opts->filename = argv[curarg];
        }
        else {
            printf("Unknown param: %s\n", argv[curarg]);
            return -1;
        }
        curarg++;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct message_opts_t opts;
    NEXUS_Error rc;
    unsigned count;

    cmdline_default_message_opts(&opts);
    if (cmdline_parse_message_opts(argc, argv, &opts)) {
        return 0;
    }

    NEXUS_Platform_Init(NULL);

    if (opts.filename) {
        NEXUS_PlaybackSettings playbackSettings;
        NEXUS_PlaypumpHandle playpump;
        NEXUS_FilePlayHandle file;

        playpump = NEXUS_Playpump_Open(0, NULL);
        BDBG_ASSERT(playpump);
        playback = NEXUS_Playback_Create();
        BDBG_ASSERT(playback);

        file = NEXUS_FilePlay_OpenPosix(opts.filename, NULL);
        if (!file) {
            fprintf(stderr, "can't open file:%s\n", opts.filename);
            return -1;
        }

        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.playpump = playpump;
        playbackSettings.playpumpSettings.transportType = opts.transportType;
        playbackSettings.playpumpSettings.timestamp.type = opts.tsTimestampType;
        playbackSettings.endOfStreamAction = opts.endOfStreamAction;
        rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Playback_Start(playback, file, NULL);
        BDBG_ASSERT(!rc);
    }
#if NEXUS_NUM_PARSER_BANDS
    else {
        NEXUS_InputBand inputBand;
        NEXUS_ParserBandSettings parserBandSettings;

        parserBand = NEXUS_ParserBand_e0;
        NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

        NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = inputBand;
        rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
        BDBG_ASSERT(!rc);
    }
#endif

    BKNI_CreateEvent(&event);

    add_message_filter(&opts, opts.pid);

    /* Read the PAT a few times */
    for (count=0;;count++) {
        const uint8_t *buffer;
        size_t size;
        unsigned programNum, message_length;

        if (count == MAX_MESSAGE_FILTERS) {
            count = 0;
        }

        if (!g_message[count].message || g_message[count].done) {
            continue;
        }

        rc = NEXUS_Message_GetBuffer(g_message[count].message, (const void **)&buffer, &size);
        BDBG_ASSERT(!rc);

        if (!size) {
#if 0
            BERR_Code rc = BKNI_WaitForEvent(event, 5 * 1000); /* wait 5 seconds */
            if (rc) {BDBG_ERR(("test failed")); rc = -1; break;}
#endif
            continue;
        }

#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)

        /* We should always get whole PAT's because maxContiguousMessageSize is 4K */
        message_length = TS_PSI_GET_SECTION_LENGTH(buffer) + 3;
        BDBG_ASSERT(size >= (size_t)message_length);

        if (opts.psi) {
            if (buffer[0] == 0) {

                /* Program Association Table */
                printf("PAT: size=%d\n", message_length);
                for (programNum=0;programNum<(unsigned)(TS_PSI_GET_SECTION_LENGTH(buffer)-7)/4;programNum++) {
                    unsigned byteOffset = 8 + programNum*4;
                    unsigned program = TS_READ_16( &buffer[byteOffset] );
                    unsigned short pid = (uint16_t)(TS_READ_16( &buffer[byteOffset+2] ) & 0x1FFF);
                    printf("  program %d: pid 0x%x\n", program, pid);
                    add_message_filter(&opts, pid);
                }
                g_message[count].done = true; /* don't parse the PAT more */
            }
            else if (buffer[0] == 1) {
                /* Conditional Access Table */
                printf("CAT: size=%d\n", message_length);
            }
            else if (buffer[0] == 2) {
                unsigned i, byteOffset;

                /* Program Table */
                printf("PMT: size=%d\n", message_length);

#define TS_PSI_LAST_SECTION_NUMBER_OFFSET   7
#define TS_PSI_MAX_BYTE_OFFSET( buf )               (TS_PSI_GET_SECTION_LENGTH(buf) - 1)
#define PROGRAM_INFO_LENGTH_OFFSET (TS_PSI_LAST_SECTION_NUMBER_OFFSET+3)
#define PROGRAM_INFO_LENGTH(buf) (TS_READ_16(&buf[PROGRAM_INFO_LENGTH_OFFSET])&0xFFF)
#define DESCRIPTOR_BASE(buf) (&buf[TS_PSI_LAST_SECTION_NUMBER_OFFSET+5])
#define STREAM_BASE(buf) (TS_PSI_LAST_SECTION_NUMBER_OFFSET + 5 + PROGRAM_INFO_LENGTH(buf))

                byteOffset = STREAM_BASE(buffer);

                while (byteOffset < (unsigned)TS_PSI_MAX_BYTE_OFFSET(buffer) && byteOffset < message_length)
                {
                    printf("  stream_type 0x%02x, pid 0x%x\n",
                        buffer[byteOffset], (uint16_t)(TS_READ_16( &buffer[byteOffset+1] ) & 0x1FFF));

                    byteOffset += 5 + (TS_READ_16( &buffer[byteOffset+3] ) & 0xFFF);
                    i++;
                }
                g_message[count].done = true; /* don't parse this PMT more */
            }
        }
        else if (opts.psip) {
            if (buffer[0] == 0xC7) {
                printf("Master Guide Table (MGT)\n");

                /* TODO: parse and filter PID's for EIT and ETT */
            }
            else if (buffer[0] == 0xC8) {
                printf("Terrestrial Virtual Channel Table (VCT)\n");
            }
            else if (buffer[0] == 0xC9) {
                printf("Cable Virtual Channel Table (CVCT)\n");
            }
            else if (buffer[0] == 0xCA) {
                printf("Rating Region Table (RRT)\n");
            }
            else if (buffer[0] == 0xCB) {
                printf("Event Information Table (EIT)\n");
            }
            else if (buffer[0] == 0xCC) {
                printf("Extended Text Table (ETT)\n");
            }
            else if (buffer[0] == 0xCD) {
                printf("System Time Table (STT)\n");
            }
        }
        else {
            printf("Found message: id=%d size=%d\n", buffer[0], message_length);
        }

        if (opts.hex) {
            unsigned i;
            for (i=0;i<message_length;i++) {
                printf("%02x", buffer[i]);
            }
            printf("\n");
        }
        if (opts.ascii) {
            unsigned i;
            for (i=0;i<message_length;i++) {
                printf("%c", buffer[i] < 32 ? '.':buffer[i]);
            }
            printf("\n");
        }

        /* only complete one message */
        if (message_length % 4) {
            message_length += 4 - (message_length % 4);
        }
        NEXUS_Message_ReadComplete(g_message[count].message, message_length);
    }

    NEXUS_Platform_Uninit();
    return 0;
}
