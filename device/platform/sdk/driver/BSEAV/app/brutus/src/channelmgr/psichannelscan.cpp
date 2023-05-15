/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psichannelscan.cpp $
 * $brcm_Revision: 83 $
 * $brcm_Date: 5/8/12 3:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: X:/win_views/7241/BSEAV/app/brutus/src/channelmgr/psichannelscan.cpp $
 * 
 * 83   5/8/12 3:24p vishk
 * SW3462-12: Enable brutus support for DVBC2
 * 
 * 82   3/21/12 12:12p erickson
 * SW7425-2664: consolidate force_sds BCHP_CHIP list
 * 
 * 81   12/21/11 3:54p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 80   6/5/11 8:39p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Add support for DVBT2 OFDM standard.
 * 
 * 79   4/18/11 5:04p dlwin
 * SW7425-187: Fixed a Brutus channels.txt parser problem; multiple
 * explicit 'PROGRAM PID' always resulted in MPEG2 video codec for second
 * and greater 'PROGRAM PID' entry.
 * 
 * 78   2/15/11 3:05p nickh
 * SW7422-213: Add support for multiple video PIDs and types for SVC/MVC
 * 
 * 77   2/15/11 6:25p xhuang
 * SW7358-34: Add 7358 support
 * 
 * 76   1/27/11 11:32a mward
 * SW7125-726:  Add QAM Annex C.  Set bandwidth to 8 MHz for Annex A.
 * 
 * 75   12/28/10 3:16p hongtaoz
 * SW7425-49: add 7425 support;
 * 
 * 74   12/15/10 10:41a jrubio
 * SW7344-9: add 7344/7346 support
 * 
 * 73   11/4/10 2:16p hongtaoz
 * SW7425-49: added 7425 support;
 * 
 * 72   10/21/10 4:59p nickh
 * SW7422-80: Add 7422 support for Satellite tuners
 * 
 * 71   4/30/10 11:54a erickson
 * SW7405-4105: support single pid in PROGRAM PIDS which can set both
 * video and pcr
 *
 * 70   4/19/10 3:58p nickh
 * SW7420-709: Add support for SDS3/SDS4/SDS5 tuners that can be set as
 * default.  Add 7420 to the list of chips capable of forcing a specific
 * default tuner.
 *
 * 69   4/12/10 7:33p nitinb
 * SW7550-339: Enable "channel edit" functionality in Brutus for OFDM
 * input
 *
 * 68   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/23/09 2:53p chengli
 * SW7550-64: read mode setting for OFDM
 *
 * 67   8/19/09 10:55a jrubio
 * PR55232: add 7342/7340 support
 *
 * 66   8/10/09 1:39p jrubio
 * PR55232: add 7340/7342
 *
 * 65   7/8/09 10:19a erickson
 * PR29193: backing out this change. it was not actually doing what was
 * intended. there were two variables in PSIChannelScan::read called list
 * which probably led to the bad code. I renamed one.
 *
 * 64   7/1/09 1:38p mward
 * PR49686: In fixed order audtype is before vidtype.
 *
 * 63   6/26/09 3:37p erickson
 * PR56373: close file on error
 *
 * 62   6/24/09 7:09p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 *
 * 61   6/23/09 6:16p katrep
 * PR55806: QAM128 support
 *
 * 60   5/22/09 2:16p erickson
 * PR49686: brutus channels.txt should support list of audio/video pids
 * per program
 *
 * 59   3/10/09 11:14a agin
 * PR44451: If not forcing an SDSx, use normal tuner allocation.
 *
 * 58   2/4/09 10:30a jrubio
 * PR51629: add 7336 support
 *
 * 57   1/10/09 4:55p agin
 * PR50937: Fixed debug messages.
 *
 * 56   11/15/08 7:11a agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 55   11/12/08 10:25a agin
 * PR48907: Need to support DCII for 97325/97335.
 *
 * 54   10/31/08 12:29p jjordan
 * PR47230: Add TTS Pacing
 *
 * PR47230/1   10/28/08 10:37a jjordan
 * PR47230: Port TTS Pacing to Nexus
 *
 * 53   8/20/08 5:49p lwhite
 * PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
 *
 * 52   7/1/08 11:33a agin
 * PR44451: Add ability to force SDS0/SDS1/SDS2 tuners as the default
 * tuner.
 *
 * 51   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 50   1/28/08 11:41a jtna
 * PR29193: Rework channeleditscreen behavior
 *
 * 49   9/19/07 12:33p jrubio
 * PR34613: call tcpConnect function instead of adding tcp code here, do
 * not read ipstreamer_server.cfg file anymore.
 *
 * 48   9/18/07 6:12p jtna
 * PR29193: Disabled ChannelMap entries at the end of the list are
 * incorrectly added to GUI
 *
 * 47   9/4/07 11:14a yrajakar
 * PR34578: Adding support for RTP FEC
 *
 * 46   8/10/07 1:22p ssood
 * PR33786: Net DMA & IP Streamer refactoring: replace STREAMER_SUPPORT
 * variable by NETACCEL_SUPPORT
 *
 * 45   7/11/07 3:30p erickson
 * PR32352: print all video/audio pids in a program
 *
 * 44   7/11/07 3:09p erickson
 * PR32352: print audio/video codec names instead of numbers
 *
 * 43   6/11/07 1:04p jjordan
 * PR32034: Add support for TTS streams in IP channels
 *
 * 42   5/3/07 2:17p mward
 * PR30301: add QAM16/32 support
 *
 * 41   4/24/07 12:49p jrubio
 * PR30010: added the ability to retrieve channel info for IPStreamer
 *
 * 40   4/19/07 6:44p jrubio
 * PR30010: adding tcp channel IP_HTTP entry
 *
 * 39   2/23/07 10:32a erickson
 * PR26335: added diseqc command support
 *
 * 38   1/29/07 11:52a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 38   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 37   1/18/07 12:19p ahulse
 * PR27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 *
 * 36   1/5/07 6:52p ssood
 * PR23774: Edit Channel Map doesn't write a IP Channel back to
 * channel.txt
 *
 * 35   11/30/06 6:15p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 34   11/21/06 8:52a erickson
 * PR23575: add DEMUX_PROGRAMS feature (defaulted off) to create separate
 * channels for streams with multiple audio/video pids.
 *
 * 33   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 32   10/3/06 4:49p jjordan
 * PR24714: Add support for RTSP protocol
 *
 * 31   9/28/06 6:59p arbisman
 * PR24289: Back-out changes
 *
 * 30   9/22/06 11:32a erickson
 * PR24374: memset CHANNEL_INFO_T correctly
 *
 * 29   9/20/06 3:50p erickson
 * PR24374: don't put CHANNEL_INFO_T on the stack. it's 14K.
 *
 * 27   7/25/06 5:24p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 *
 * 26   7/6/06 4:01p erickson
 * PR21941: remove pcrpid->vidpid hack. allow pcrpid=0 to set vsync mode.
 *
 * 25   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 24   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/1   5/16/06 11:33a tokushig
 * merge from main
 *
 * 23   5/12/06 12:38p erickson
 * PR19790: added QPSK_TURBO
 *
 * 22   5/3/06 10:01a erickson
 * PR19790: added LDPC params and reworked code to remove hardcoded
 * numbers in switch statement
 *
 * 21   4/20/06 1:41p jjordan
 * PR17547: add basic RTP support to NET_IF DMA kernel-mode driver
 *
 * 20   3/15/06 10:47a jgarrett
 * PR 20004: Re-enabling QAM for the 97400.  Identified problem with soft
 * FP.
 *
 * 19   3/3/06 5:06p jgarrett
 * PR 20004: Disabling QAM tuning for now on 97400
 *
 * 18   1/17/06 11:46a erickson
 * PR18651: allow audio type names (i.e. AC3,MPEG) to be used in video's
 * audtype param
 *
 * 17   10/21/05 3:06p dma
 * PR17733: auto channel/pids scan for IP playback
 *
 * 16   10/13/05 5:25p erickson
 * PR17541: moved general purpose code to mediatypes.c
 *
 * 15   10/13/05 3:11p erickson
 * PR17541: added more audio and video codecs for 7401 and beyond
 *
 * 14   10/12/05 5:35p jjordan
 * PR17547: Add support for RTP protocols
 *
 * 13   8/24/05 6:12p arbisman
 * PR16678: Add support for WinCE
 *
 * 12   8/16/05 2:17p erickson
 * PR12978: is noScan is set, write out PROGRAM PIDS
 *
 * 11   6/29/05 9:05a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 10   6/8/05 10:22a erickson
 * PR15808: allow pids of 0x1fff to get around auto assignment of pcr
 * and/or video, but then convert back to 0 so that settop api can detect
 *
 * 9   4/21/05 1:54p erickson
 * PR14701: default pcrpid with vidpid and vice versa
 *
 * 8   4/20/05 1:46p erickson
 * PR13097: standardized printout
 *
 * 7   4/15/05 6:35p vsilyaev
 * PR 13097: Printout video/audio types for discovered programs.
 *
 * 6   4/9/05 12:20a erickson
 * PR14701: renamed parameter
 *
 * 5   4/4/05 1:30p erickson
 * PR13823: removed B_HAS_XXXX frontend #defines. use bconfig for runtime
 * check if really needed.
 *
 * 4   3/29/05 4:58p erickson
 * PR14593: 3560 work
 *
 * 3   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 2   3/1/05 12:31p erickson
 * PR13838: support audio/video type strings
 *
 * 1   2/7/05 8:00p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/26   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/25   1/25/05 6:36p bandrews
 * PR11657: Added user specified code rate support
 *
 * Irvine_BSEAVSW_Devel/24   11/9/04 7:08p vsilyaev
 * PR 13097: Added support for video stream type. Added code to parse
 * name/value pairs.
 *
 * Irvine_BSEAVSW_Devel/23   8/18/04 11:31a erickson
 * PR10832: updated default QAM symbol rate code
 *
 * Irvine_BSEAVSW_Devel/22   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/21   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/20   8/3/04 3:13p erickson
 * PR11770: fixed channel map logic
 *
 * Irvine_BSEAVSW_Devel/19   7/22/04 12:27p erickson
 * PR11682: LINEINPUT format is list[2], not 3
 *
 * Irvine_BSEAVSW_Devel/18   7/21/04 3:16p erickson
 * PR11682: convert lots of strcmps to switch
 *
 * Irvine_BSEAVSW_Devel/17   7/21/04 10:10a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 *
 * Irvine_BSEAVSW_Devel/16   5/25/04 11:49a erickson
 * PR11135: use bband_get for proxy support
 *
 * Irvine_BSEAVSW_Devel/15   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/14   3/2/04 5:26p erickson
 * PR9217: improved debug output on channel scan
 *
 * Irvine_BSEAVSW_Devel/13   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/12   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 *
 * Irvine_BSEAVSW_Devel/11   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/10   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/9   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 *************************************************************/
#include "psichannelscan.h"
extern "C" {
#include "tspsimgr.h"
#ifdef DSS_SUPPORT
#include "dsstspsimgr.h"
#endif
}
#ifdef B_HAS_IP
#include "ip.h"
#define NET_CHANMAP_PORT   5002
#endif
#include "mediatypes.h"
#include <mstringlist.h>
#include "brutus_os.h"
BDBG_MODULE(psichannelscan);

#define MEGA 1000000
#define MEGA_F 1000000.0

#if B_CHANNELMGR_FORCE_SDS
int force_sds;
#endif

PSIChannelScan::PSIChannelScan(ChannelMap *map, const Settings *settings) :
    ChannelScan(map, settings)
{
}

static void printProgram(const PROGRAM_INFO_T *program)
{
    int i;
    printf("  program: video=");
    for (i=0;i<program->num_video_pids;i++) {
        printf("%#x[%s]",
            program->video_pids[i].pid,
            b_vidtype_str((bvideo_codec)program->video_pids[i].streamType));
        if (i < program->num_video_pids-1)
            printf(",");
    }

    printf(", audio=");
    for (i=0;i<program->num_audio_pids;i++) {
        printf("%#x[%s]",
            program->audio_pids[i].pid,
            b_audtype_str((baudio_format)program->audio_pids[i].streamType));
        if (i < program->num_audio_pids-1)
            printf(",");
    }

    printf(", pcr=%#x\n", program->pcr_pid);
}

int PSIChannelScan::readProgramInfo(bband_t band, unsigned entryIndex)
{
    CHANNEL_INFO_T *info = (CHANNEL_INFO_T *)malloc(sizeof(CHANNEL_INFO_T)); /* 14K structure, use heap */
    int rc;
    bband_settings settings;

    if (!info) return -1;
    memset(info, 0, sizeof(*info));
    bband_get(band, &settings);

    if (IS_DSS(settings.mpeg_type)) {
#ifdef DSS_SUPPORT
        rc = dssTsPsi_getChannelInfo(info, band);
#else
        rc = -1;
        goto done;
#endif
    }
    else {
        rc = tsPsi_getChannelInfo(info, band, NULL);
    }
    if (rc) goto done;

    map()->clearPrograms(entryIndex);
    for (int i=0; i<info->num_programs; i++)
    {
        const PROGRAM_INFO_T *program = &info->program_info[i];
        if (includeProgram(entryIndex, program)) {
            if (_settings.demuxPrograms) {
                for (int index=0; index<program->num_audio_pids; index++) {
                    PROGRAM_INFO_T mod_program = *program;
                    mod_program.num_video_pids = 1;
                    mod_program.video_pids[0] = mod_program.video_pids[index];
                    mod_program.num_audio_pids = 1;
                    mod_program.audio_pids[0] = mod_program.audio_pids[index];
                    map()->addProgram(entryIndex, &mod_program);
                    printProgram(&mod_program);
                }
            }
            else {
                map()->addProgram(entryIndex, program);
                printProgram(program);
            }
        }
    }
done:
    free(info);
    return rc;
}

int PSIChannelScan::write(const char *filename) const
{
    int rc = 0;
    FILE *f;

    f = fopen(filename, "w+");
    if (!f)
        return -1;

// write standard header
    fputs(
"###########################\n"
"# Brutus Channel List\n"
"# See BrutusUsageGuide.doc for syntax. The following are some common examples:\n"
"#   LINEINPUT   0  NTSC\n"
"#   QPSK_DVB    0  1120    20.0   13v\n"
"#   8PSK        0  1150    21.5\n"
"#   QAM64       0  765     DEFAULT\n"
"#   ANALOG      0  63.25   NTSC\n"
"#   VSB8        0  527     DEFAULT\n"
"#   OFDM        0  527     8 (BW 5-8) off:auto(CCI) high:low(priority)\n"
"#   STREAMER    0  0\n"
"###########################\n"
    , f);

    for (int i=0;i<map()->total();i++) {
        ChannelMap::ChannelMapEntry str;
        if (map()->getEntry(i, &str) == false) {
            rc = -1;
            goto error;
        }
        if (str.disabled == 2) /* if entry has been deleted, goto next entry */
            continue;

        switch (str.getType()) {
        case btuner_vsb_stream:
                fprintf(f, "%-15s %d\t%0.2f\t%f\n",
                    btuner_vsb_str(str.vsb.mode),
                    str.disabled?1:0,
                    str.freq,
                    str.vsb.symbol_rate / MEGA_F);
                break;
        case btuner_ofdm_stream:
                    fprintf(f, "%-15s %d\t%0.2f\t%d\t%s\t%s\t%s\n",
                    btuner_ofdm_str(),
                    str.disabled?1:0,
                    str.freq,
                    str.ofdm.bandwidth,
                    str.ofdm.cci_mode==btuner_ofdm_cci_mode_none?"off":"auto",
                    str.ofdm.priority==btuner_ofdm_priority_high?"high":"low",
                        btuner_ofdm_mode_str(str.ofdm.mode));
                break;
        case btuner_streamer_stream:
                fprintf(f, "STREAMER %d\t%d\t%s\n",
                    str.disabled?1:0,
                    str.streamer.index,
                    bstream_mpeg_type_str(str.streamer.mpeg_type)
                    );
                break;
        case btuner_sds_stream:
            fprintf(f, "%-15s %d\t%0.2f\t%0.2f\t%s\t%s\n",
                btuner_sds_str(str.sds.params.mode, str.sds.mpeg_type),
                str.disabled?1:0,
                str.freq,
                str.sds.params.symbol_rate / MEGA_F,
                str.sds.params.diseqc_polarity==13?"13V":"18V",
                str.sds.params.diseqc_tone?"ToneOn":"ToneOff");
            break;
        case btuner_qam_stream:
            fprintf(f, "%-15s %d\t%0.2f\t%f\t%s\n",
                btuner_qam_str(str.qam.mode),
                str.disabled?1:0,
                str.freq,
                str.qam.symbol_rate / MEGA_F,
                btuner_qam_annex_str(str.qam.annex));
            break;
        case btuner_analog_stream:
            if (str.analog.linein >= 0) {
                fprintf(f, "LINEINPUT %d %s\n",
                    str.analog.linein,
                    btuner_analog_format_str(str.analog.params.video_format));
            }
            else {
                fprintf(f, "%-15s %d\t%0.2f\t%s\n",
                    btuner_analog_str(&str),
                    str.disabled?1:0,
                    str.freq,
                    btuner_analog_format_str(str.analog.params.video_format));
            }
            break;
        case btuner_ip_stream:
                fprintf(f, "%s\t%d\t",
                                        (str.ip.params.open_params.protocol == bsettop_ip_protocol_udp)? "IP_UDP":
                                        (str.ip.params.open_params.protocol == bsettop_ip_protocol_rtp_fec)? "IP_FEC":
                        "IP_RTP",
                    str.disabled?1:0
                    );

                                if  ((str.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp) ||
                                     (str.ip.params.open_params.protocol == bsettop_ip_protocol_http))

                                {
                        fprintf(f, "%s\n", str.ip.params.open_params.url);
                                }
                                else
                                {
                        fprintf(f, "%s\t%d\n",
                                                str.ip.params.open_params.ip_addr,
                                                str.ip.params.open_params.port
                            );
                                }
            break;
        default:
            rc = -1;
            goto error;
        }

        if (map()->isNoScanChannelMapEntry(i)) {
            for (int p=0;p<map()->totalPrograms(i);p++) {
                PROGRAM_INFO_T program;
                if (map()->getProgram(i, p, &program) == true) {
                        fprintf(f, "PROGRAM PIDS video=%#x video_type=%#x audio=%#x audio_type=%#x pcr=%#x\n",
                            program.video_pids[0].pid,
                            program.video_pids[0].streamType,
                            program.audio_pids[0].pid,
                            program.audio_pids[0].streamType,
                            program.pcr_pid);
                                }
            }
        }
    }
    fclose(f);
    b_sync_disk();
    return 0;

error:
    if (f) fclose(f);
    return rc;
}

int PSIChannelScan::read(const char *filename, const char * ipv4) {
    MStringList list;

    #if B_CHANNELMGR_FORCE_SDS
    force_sds=-1;
    #endif

    map()->clear();
    if (!readFile(filename, list))
        BDBG_WRN(("Local Channel Map not found."));

#ifdef B_HAS_IP
    MStringList server_list;
    int socket_fd,act_read;
    char chan_buf[2048];
    memset(chan_buf, 0, sizeof(chan_buf));

    if (ipv4 ==NULL)
        goto regular_channels;
    //const char *serv_addr = server_list.first();
    //serv_addr = strtok((char*)serv_addr, " ");
    socket_fd = tcpConnect(ipv4,NET_CHANMAP_PORT);
    if (socket_fd <0 )
    {
        BDBG_WRN(("No Socket!!!"));
        goto regular_channels;
    }

    act_read=  tcprecv(socket_fd, chan_buf, 2048, 0);
    chan_buf[act_read] = '\0';
    tcpClose(socket_fd);
    char *start, *next;
    start = chan_buf;
    while (1) {
        next = strchr(start, '\n');
        if (next)
            list.add(start, next-start);
        else {
            list.add(start);
            break;
        }
        start = next+1;
    }

regular_channels:
#endif

    for (const char *s = list.first(); s; s = list.next()) {
        // check for line comment
        char *comment = strchr(s, '#');
        if (comment)
            *comment = 0;

        MStringList terms;
        MStringList::split(s, " \t", terms);

        if (!terms.total())
            continue;
        else if (terms[0] == "PROGRAM") {
            int n = map()->total()-1;
            ChannelMap::ChannelMapEntry str;
            if (map()->getEntry(n, &str) == true)
                edit(terms, n, &str);
        }
        #if B_CHANNELMGR_FORCE_SDS
        else if (terms[0] == "SDS0") {
            BDBG_WRN(("SDS0"));
            force_sds=0;
        }
        else if (terms[0] == "SDS1") {
            BDBG_WRN(("SDS1"));
            force_sds=1;
        }
        else if (terms[0] == "SDS2") {
            BDBG_WRN(("SDS2"));
            force_sds=2;
        }
        else if (terms[0] == "SDS3") {
            BDBG_WRN(("SDS3"));
            force_sds=3;
        }
        else if (terms[0] == "SDS4") {
            BDBG_WRN(("SDS4"));
            force_sds=4;
        }
        else if (terms[0] == "SDS5") {
            BDBG_WRN(("SDS5"));
            force_sds=5;
        }
        else if (terms[0] == "SDS6") {
            BDBG_WRN(("SDS6"));
            force_sds=6;
        }
        else if (terms[0] == "SDS7") {
            BDBG_WRN(("SDS7"));
            force_sds=7;
        }
        #endif
        else {
            ChannelMap::ChannelMapEntry str;
            memset(&str, 0, sizeof(ChannelMap::ChannelMapEntry));
            if (process(terms, &str)) {
                map()->add(&str);
            }
        }
    }
    return 0;
}

void PSIChannelScan::edit(MStringList &list, int streamnum, ChannelMap::ChannelMapEntry *str)
{
    if (list.total() > 1 && list[1] == "PIDS") {
        PROGRAM_INFO_T program;
        bstream_mpeg mpeg;
        unsigned i;
        unsigned pos=0;

        bstream_mpeg_init(&mpeg);
        for (i=2;list[i];i++) {
            char *val = strchr(list[i], '=');
            if (val) {
                char name[16];
                unsigned n = val - list[i];
                if (n > 15) n = 15;
                strncpy(name, list[i], n);
                name[n] = 0;
                val++;

                if (!strcasecmp(name, "video")) {
                    int i = 0;
                    while (*val) {
                        mpeg.video[i++].pid = strtoul(val, (char **)&val, 0);
                        if (*val) val++; /* if not terminated, assume it's a single comma */
                    }
                }
                else if (!strcasecmp(name, "video_type")) {
                    int i = 0;
                    while (*val) {
                        char *nextval;
                        mpeg.video[i++].format = b_lookup_vidtype(val, &nextval);
                        val = nextval;
                        if (*val) val++; /* if not terminated, assume it's a single comma */
                    }
                }
                else if (!strcasecmp(name, "audio")) {
                    int i = 0;
                    while (*val) {
                        mpeg.audio[i++].pid = strtoul(val, (char **)&val, 0);
                        if (*val) val++; /* if not terminated, assume it's a single comma */
                    }
                }
                else if (!strcasecmp(name, "audio_type")) {
                    int i = 0;
                    while (*val) {
                        char *nextval;
                        mpeg.audio[i++].format = b_lookup_audtype(val, &nextval);
                        val = nextval;
                        if (*val) val++; /* if not terminated, assume it's a single comma */
                    }
                }
                else if (!strcasecmp(name, "pcr")) {
                    mpeg.pcr_pid = strtoul(val, NULL, 0);
                }
                else {
                    BDBG_WRN(("Unknown label '%s', value '%s'", name, val));
                }
            }
            else {
                /* if there's no label, take them in a fixed order */
                switch (pos++) {
                case 0: mpeg.pcr_pid = strtoul(list[i], NULL, 0); break;
                case 1: mpeg.video[0].pid = strtoul(list[i], NULL, 0); break;
                case 2: mpeg.audio[0].pid = strtoul(list[i], NULL, 0); break;
                case 3: mpeg.audio[0].format = b_lookup_audtype(list[i], NULL); break;
                case 4: mpeg.video[0].format = b_lookup_vidtype(list[i], NULL); break;
                default: break;
                }
            }
        }

        /* a helpful default */
        if (mpeg.video[0].pid && !mpeg.pcr_pid) {
            mpeg.pcr_pid = mpeg.video[0].pid;
            BDBG_WRN(("Automatically using vidpid for pcrpid in channels.txt. override with pcr=XXXX."));
        }
        if (!mpeg.video[0].pid && !mpeg.audio[0].pid && mpeg.pcr_pid) {
            mpeg.video[0].pid = mpeg.pcr_pid;
            BDBG_WRN(("Automatically using pcrpid for vidpid in channels.txt. override with video=XXXX."));
        }

        /* pcr_pid of 0 does not force vsync because of playback support. make it 0x1fff to force vsync
        because this pid is always live, never playback. */
        if (!mpeg.pcr_pid) mpeg.pcr_pid = 0x1fff;

        ChannelMap::setProgram(&program, &mpeg);
        map()->addProgram(streamnum, &program, NULL,
            false /* not scanned */);

        /* dma: if we are here, it means there is an "PROGRAM PIDS ..." entry in channels.txt.
            It means, no need to do auto PIDs.
        */
        if (str->getType() == btuner_ip_stream) {
            str->ip.hasProgramPidsLine = true;
            str->ip.gotAutoPids = false;
        }
    }
}

static struct {
    const char *name;
    btuner_stream_type type;
    int value; /* the meaning of this value depends on the btuner_stream_type */
} entry_types[] = {
    {"INVALID",     btuner_sds_stream, 0}, /* must be entry 0 in this list */
    {"INPUTBAND",   btuner_streamer_stream, 0}, /* legacy, same as STREAMER */
    {"STREAMER",    btuner_streamer_stream, 0},
    {"VSB8",        btuner_vsb_stream, 8},
    {"VSB16",       btuner_vsb_stream, 16},
    {"OFDM",        btuner_ofdm_stream, 0},
    {"QPSK_DSS",    btuner_sds_stream, 0},
    {"QPSK_DSS_PES",btuner_sds_stream, 1},
    {"QPSK_DVB",    btuner_sds_stream, 2},
    {"QPSK_LDPC",   btuner_sds_stream, 3},
    {"QPSK_TURBO",  btuner_sds_stream, 4},
    {"8PSK_TURBO",  btuner_sds_stream, 5},
    {"DCII",        btuner_sds_stream, 6},
    {"8PSK_LDPC",   btuner_sds_stream, 7},
    {"8PSK",        btuner_sds_stream, 7},
    {"LDPC",        btuner_sds_stream, 8},
    {"TURBO",       btuner_sds_stream, 9},
    {"LINEINPUT",   btuner_analog_stream, 0},
    {"ANALOG",      btuner_analog_stream, 1},
    {"QAM16",       btuner_qam_stream, 16},
    {"QAM32",       btuner_qam_stream, 32},
    {"QAM64",       btuner_qam_stream, 64},
    {"QAM128",      btuner_qam_stream, 128},
    {"QAM256",      btuner_qam_stream, 256},
    {"QAM1024",     btuner_qam_stream, 1024},
    {"IP_UDP",      btuner_ip_stream, 0},
    {"IP_RTP",      btuner_ip_stream, 1},
    {"IP_HTTP",     btuner_ip_stream, 2},
    {"IP_FEC",      btuner_ip_stream, 3},
#if B_HAS_RTSP
    {"RTSP",        btuner_rtsp_stream, 0},
#endif
    {0,             btuner_sds_stream, 0} // terminator
};

static void b_sds_coderate_parser(MString s, btuner_sds_coderate *p_coderate)
{
    MStringList list;
    MStringList::split(s, "/", list);

    if (p_coderate && list.total() > 1)
    {
        p_coderate->num = atoi(list[0]);
        p_coderate->denom = atoi(list[1]);
        if (list.total() == 3)
        {
            float bps = atof(list[2]);
            p_coderate->bits_per_symbol = (int)(bps * 1000);
        }
    }
}

static void b_ip_qualifiers_parser(MStringList &list, ChannelMap::ChannelMapEntry *str)
{
    char qualifier_str[9];
    int i;

    for (i=4; i<list.total(); i++) {
        strlcpy(qualifier_str, list[i].s(), sizeof(qualifier_str)-1);
    if(strncmp(qualifier_str, "TTS", 3)==0) {
        str->ip.timestamp_enabled = true;
            BDBG_MSG(("TTS Enabled" ));
        } else if (strncmp(qualifier_str, "SSL", 3)==0) {
                if (str->ip.params.open_params.security.security_protocol == bsettop_ip_security_none) {
                    str->ip.params.open_params.security.security_protocol = bsettop_ip_security_ssl;
                    BDBG_MSG(("Security: SSL " ));
                }
                else {
                    BDBG_ERR(("Error: Conflicting security types set"));
            }
        } else if (strncmp(qualifier_str, "DTCP_IP", 7)==0) {
                if (str->ip.params.open_params.security.security_protocol == bsettop_ip_security_none) {
                    str->ip.params.open_params.security.security_protocol = bsettop_ip_security_dtcp_ip;
                    BDBG_MSG(("Security: DTCP_IP " ));
                }
                else {
                    BDBG_ERR(("Error: Conflicting security types set"));
                }
        }
    }
/* TODO: add support for PS and AMM */
#if 0
    else if (strncmp(qualifier_str, "PS", 2)==0) {
        str->ip.mpeg_type = bstream_mpeg_type_pes;
        if (list.total() >= 6) {
            strcpy(qualifier_str, list[5].s());
            if(strncmp(qualifier_str, "AMM", 3)==0) {
                str->ip.use_audio_master_mode = true;
            }
        }
    }
    else if(strncmp(qualifier_str, "AMM", 3)==0) {
        str->ip.use_audio_master_mode = true;
    }
#endif
}

bool PSIChannelScan::process(MStringList &list, ChannelMap::ChannelMapEntry *str)
{
    if (list.total() == 0)
        return false;

    const char *entry_type = list[0];
    BDBG_MSG(("PSIChannelScan::process '%s'", entry_type));

    int type_num;
    for (type_num=0;entry_types[type_num].name;type_num++) {
        if (!strcmp(entry_types[type_num].name, entry_type))
            break;
    }
    if (!entry_types[type_num].name)
        return false;

    /* universal setting */
    if (list.total() >= 2)
        str->disabled = atoi(list[1]);
    else
        str->disabled = 1;

    if (type_num == 0) { /* "INVALID" */
        // placeholder, skipped by findNextChannel
        str->disabled = 1;
        return true;
    }

    switch (entry_types[type_num].type) {
    case btuner_streamer_stream:
        if (list.total() >= 3) {
            str->setType(btuner_streamer_stream);
            str->streamer.index = atoi(list[2]);
            str->streamer.mpeg_type = bstream_mpeg_type_ts;
            if (list.total() > 3) {
                if (list[3] == "DSS_PES")
                    str->streamer.mpeg_type = bstream_mpeg_type_dss_pes;
                else if (list[3] == "DSS_ES")
                    str->streamer.mpeg_type = bstream_mpeg_type_dss_es;
            }
        }
        break;

    case btuner_vsb_stream:
        if (list.total() >= 3)
        {
            str->setType(btuner_vsb_stream);
            if (entry_types[type_num].value == 16)
                str->vsb.mode = btuner_vsb_mode_vsb16;
            else
                str->vsb.mode = btuner_vsb_mode_vsb8;
            str->freq = atof(list[2]);
            if (list.total() > 3 && strcasecmp(list[3], "DEFAULT"))
                str->vsb.symbol_rate = (unsigned)(atof(list[3]) * MEGA);
        }
        break;

    case btuner_ofdm_stream:
        if (list.total() >= 3)
        {
            str->setType(btuner_ofdm_stream);
            str->freq = atof(list[2]);
            str->ofdm.cci_mode = btuner_ofdm_cci_mode_none;
            str->ofdm.priority = btuner_ofdm_priority_high;
            str->ofdm.bandwidth = btuner_ofdm_bandwidth_8Mhz;
            str->ofdm.mode = btuner_ofdm_mode_dvbt;

            if (list.total() >= 4)
            {
                if(!strcasecmp(list[3], "DEFAULT"))
                {
                    str->ofdm.bandwidth = btuner_ofdm_bandwidth_8Mhz;
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_auto;
                    str->ofdm.mode = btuner_ofdm_mode_dvbt;
                    break;
                }
                else
                {
                    if((atoi(list[3])< 5) || (atoi(list[3]) > 8))
                    {
                        BDBG_ERR(("Invalid bandwidth entry for ofdm using default value 8Mhz"));
                        str->ofdm.bandwidth = btuner_ofdm_bandwidth_8Mhz;
                    }
                    else
                    {
                        str->ofdm.bandwidth = (btuner_ofdm_bandwidth)atoi(list[3]);
                    }
                }
            }
            if (list.total() >= 5)
            {
                if(list[4] == "auto")
                {
                    BDBG_MSG(("ofdm cci auto"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_auto;
                }
                else if(list[4] == "off")
                {
                    BDBG_MSG(("ofdm cci off"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_none;
                }
                else if (list[4] == "low")
                {
                    BDBG_MSG(("ofdm priority low"));
                    str->ofdm.priority = btuner_ofdm_priority_low;
                }
                else if (list[4] == "high")
                {
                    BDBG_MSG(("ofdm priority high"));
                    str->ofdm.priority = btuner_ofdm_priority_high;
                }
                else if (list[4] == "dvbt")
                {
                    BDBG_MSG(("ofdm mode dvbt"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt;
                }
                else if (list[4] == "dvbt2")
                {
                    BDBG_MSG(("ofdm mode dvbt2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt2;
                }
                else if (list[4] == "dvbc2")
                {
                    BDBG_MSG(("ofdm mode dvbc2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbc2;
                }
                else if (list[4] == "isdbt")
                {
                    BDBG_MSG(("ofdm mode isdbt"));
                    str->ofdm.mode = btuner_ofdm_mode_isdbt;
                }
            }
            if (list.total() >= 6)
            {
                if(list[5] == "auto")
                {
                    BDBG_MSG(("ofdm cci auto"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_auto;
                }
                else if(list[5] == "off")
                {
                    BDBG_MSG(("ofdm cci off"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_none;
                }
                else if (list[5] == "low")
                {
                    BDBG_MSG(("ofdm priority low"));
                    str->ofdm.priority = btuner_ofdm_priority_low;
                }
                else if (list[5] == "high")
                {
                    BDBG_MSG(("ofdm priority high"));
                    str->ofdm.priority = btuner_ofdm_priority_high;
                }
                else if (list[5] == "dvbt")
                {
                    BDBG_MSG(("ofdm mode dvbt"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt;
                }
                else if (list[5] == "dvbt2")
                {
                    BDBG_MSG(("ofdm mode dvbt2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt2;
                }
                else if (list[5] == "dvbc2")
                {
                    BDBG_MSG(("ofdm mode dvbc2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbc2;
                }
                else if (list[5] == "isdbt")
                {
                    BDBG_MSG(("ofdm mode isdbt"));
                    str->ofdm.mode = btuner_ofdm_mode_isdbt;
                }
            }
            if (list.total() >= 7)
            {
                if(list[6] == "auto")
                {
                    BDBG_MSG(("ofdm cci auto"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_auto;
                }
                else if(list[6] == "off")
                {
                    BDBG_MSG(("ofdm cci off"));
                    str->ofdm.cci_mode = btuner_ofdm_cci_mode_none;
                }
                else if (list[6] == "low")
                {
                    BDBG_MSG(("ofdm priority low"));
                    str->ofdm.priority = btuner_ofdm_priority_low;
                }
                else if (list[6] == "high")
                {
                    BDBG_MSG(("ofdm priority high"));
                    str->ofdm.priority = btuner_ofdm_priority_high;
                }
                else if (list[6] == "dvbt")
                {
                    BDBG_MSG(("ofdm mode dvbt"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt;
                }
                else if (list[6] == "dvbt2")
                {
                    BDBG_MSG(("ofdm mode dvbt2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbt2;
                }
                else if (list[6] == "dvbc2")
                {
                    BDBG_MSG(("ofdm mode dvbc2"));
                    str->ofdm.mode = btuner_ofdm_mode_dvbc2;
                }
                else if (list[6] == "isdbt")
                {
                    BDBG_MSG(("ofdm mode isdbt"));
                    str->ofdm.mode = btuner_ofdm_mode_isdbt;
                }
            }
        }
        break;

    case btuner_sds_stream:
        if (list.total() >= 4)
        {
            str->setType(btuner_sds_stream);
            switch (entry_types[type_num].value) {
            case 0:
                str->sds.params.mode = btuner_sds_mode_qpsk_dss;
                str->sds.mpeg_type = bstream_mpeg_type_dss_es;
                break;
            case 1:
                str->sds.params.mode = btuner_sds_mode_qpsk_dss;
                str->sds.mpeg_type = bstream_mpeg_type_dss_pes;
                break;
            case 2:
                str->sds.params.mode = btuner_sds_mode_qpsk_dvb;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 3:
                str->sds.params.mode = btuner_sds_mode_qpsk_ldpc;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 4:
                str->sds.params.mode = btuner_sds_mode_qpsk_turbo;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 5:
                str->sds.params.mode = btuner_sds_mode_8psk_turbo;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 6:
                str->sds.params.mode = btuner_sds_mode_qpsk_dcii;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 7:
                str->sds.params.mode = btuner_sds_mode_8psk_ldpc;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 8:
                str->sds.params.mode = btuner_sds_mode_ldpc;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            case 9:
                str->sds.params.mode = btuner_sds_mode_turbo;
                str->sds.mpeg_type = bstream_mpeg_type_ts;
                break;
            }

            // common
            str->freq = atof(list[2]);
            str->sds.params.symbol_rate = (unsigned)(atof(list[3]) * MEGA);

            /* The remaining parameters are not positional. If you need to add
            more non-boolean parameters, consider NAME=VALUE formatting. */
            for (int i=4;i<list.total();i++) {
                const char *param = list[i];
                if (!strcasecmp(param, "18v"))
                    str->sds.params.diseqc_polarity = (btuner_sds_diseqc_polarity)18;
                else if (!strcasecmp(param, "13v"))
                    str->sds.params.diseqc_polarity = (btuner_sds_diseqc_polarity)13;
                else if (!strcasecmp(param, "ToneOn"))
                    str->sds.params.diseqc_tone = true;
                else if (!strcasecmp(param, "LDPC_PILOT"))
                    str->sds.params.ldpc_pilot = true;
                else if (!strcasecmp(param, "LDPC_PILOT_PLL"))
                    str->sds.params.ldpc_pilot_pll = true;
                else if (!strcasecmp(param, "Diseqc_SAT_A")) {
                    str->sds.params.diseqc_message.enabled = true;
                    str->sds.params.diseqc_message.diseqc_sat_number = 1;
                }
                else if (!strcasecmp(param, "Diseqc_SAT_B")) {
                    str->sds.params.diseqc_message.enabled = true;
                    str->sds.params.diseqc_message.diseqc_sat_number = 2;
                }
                else if (!strcasecmp(param, "Diseqc_SAT_C")) {
                    str->sds.params.diseqc_message.enabled = true;
                    str->sds.params.diseqc_message.diseqc_sat_number = 3;
                }
                else if (!strcasecmp(param, "Diseqc_SAT_D")) {
                    str->sds.params.diseqc_message.enabled = true;
                    str->sds.params.diseqc_message.diseqc_sat_number = 4;
                }
                else {
                    /* default to coderate, which is #/# format. if there's no "/" found,
                    no coderate is set. */
                    b_sds_coderate_parser(param, &str->sds.params.coderate);
                }
            }
        }
        break;

    case btuner_analog_stream:
        if (entry_types[type_num].value == 0) { /* "LINEINPUT" */
            str->setType(btuner_analog_stream);
            /* we've hijacked this field, so assert it's always enabled */
            str->disabled = 0;
            if (list.total() > 1)
                str->analog.linein = atoi(list[1]);
            else
                str->analog.linein = 0; /* default */
            if (list.total() > 2)
                str->analog.params.video_format = !strcasecmp(list[2], "PAL")?
                    bvideo_format_pal:bvideo_format_ntsc;
        }
        else { /* "ANALOG" */
            if (list.total() >= 3) {
                str->setType(btuner_analog_stream);
                str->analog.linein = -1;
                str->freq = atof(list[2]);
                if (list.total() > 3)
                    str->analog.params.video_format = !strcasecmp(list[3], "PAL")?
                        bvideo_format_pal:bvideo_format_ntsc;
            }
        }
        break;

    case btuner_qam_stream:
        if (list.total() >= 3) {
            str->setType(btuner_qam_stream);
            str->qam.mode = (btuner_qam_mode)entry_types[type_num].value;
            str->freq = atof(list[2]);

            // have to get annex before symrate
            if (list.total() > 4)
                str->qam.annex = !strcasecmp(list[4], "AnnexA")?
                    btuner_qam_annex_a:!strcasecmp(list[4], "AnnexC") ?
                    btuner_qam_annex_c:btuner_qam_annex_b;

            bool default_symrate = list.total() <= 3 || !strcasecmp(list[3], "DEFAULT");
            if (!default_symrate)
                str->qam.symbol_rate = (unsigned)(atof(list[3]) * MEGA);
            else
                str->qam.symbol_rate = b_get_default_qam_symrate(str->qam.annex, str->qam.mode);
        }
        str->qam.bandwidth = (str->qam.annex == btuner_qam_annex_a) ? btuner_qam_bandwidth_8Mhz : btuner_qam_bandwidth_6Mhz;
        break;
    case btuner_ip_stream:
        // extract IP channel params
        str->setType(btuner_ip_stream);
        str->ip.hasProgramPidsLine = false;
        str->ip.gotAutoPids = false;

        str->ip.timestamp_enabled = false;
        str->ip.params.open_params.security.security_protocol = bsettop_ip_security_none;
        switch (entry_types[type_num].value) {
        case 0:
            // plain UDP
            if (list.total() >= 4) {
                str->ip.params.open_params.protocol = bsettop_ip_protocol_udp;
                strcpy(str->ip.params.open_params.ip_addr, list[2]);
                str->ip.params.open_params.port = atoi(list[3]);
                if (list.total() >= 5) {
                    b_ip_qualifiers_parser(list, str);
                }
            }
            break;
        case 1:
            if (list.total() >= 3) {
                if(str->ip.params.open_params.url)
                    free(str->ip.params.open_params.url);
                str->ip.params.open_params.url = (char *)malloc(sizeof(char)*(strlen(list[2].s())+1));
                strcpy(str->ip.params.open_params.url, list[2].s());
                if(strncmp(str->ip.params.open_params.url, "rtsp", 4)==0)
                    // URL style tuning (RTSP)
#ifdef LIVEMEDIA_SUPPORT
                    str->ip.params.open_params.protocol = bsettop_ip_protocol_rtsp;
#else
                    BDBG_ERR(("Error: RTSP not supported (did you build with LIVEMEDIA_SUPPORT=y?)"));
#endif
                else if (list.total() >= 4) {
                    // RTP
#ifdef LIVEMEDIA_SUPPORT
                    str->ip.params.open_params.protocol = bsettop_ip_protocol_rtp;
#else
                    str->ip.params.open_params.protocol = bsettop_ip_protocol_rtp_no_rtcp;
#endif
                    strcpy(str->ip.params.open_params.ip_addr, list[2]);
                    str->ip.params.open_params.port = atoi(list[3]);
                    if (list.total() >= 5) {
                        b_ip_qualifiers_parser(list, str);
                    }
                }
            }
            else {
                BDBG_ERR(("Error: invalid channel entry"));
            }
            break;
        case 2:
                strcpy(str->ip.params.open_params.ip_addr, list[2]);
                str->ip.params.open_params.port = atoi(list[3]);

                str->ip.params.open_params.protocol = bsettop_ip_protocol_http;

                if(str->ip.params.open_params.url)
                    free(str->ip.params.open_params.url);
                str->ip.params.open_params.url = (char *)malloc(sizeof(char)*(strlen(list[4].s())+1));
                strcpy(str->ip.params.open_params.url, list[4].s());
                if (list.total() >= 5) {
                    b_ip_qualifiers_parser(list, str);
                }
                BDBG_MSG(("IP %s, port %d",str->ip.params.open_params.ip_addr, str->ip.params.open_params.port));
                break;
        case 3:
            // plain RTP FEC
            if (list.total() >= 4) {
                str->ip.params.open_params.protocol = bsettop_ip_protocol_rtp_fec;
                strcpy(str->ip.params.open_params.ip_addr, list[2]);
                str->ip.params.open_params.port = atoi(list[3]);
                if (list.total() >= 5) {
                    b_ip_qualifiers_parser(list, str);
            }
            }
            break;
        default:
            BDBG_ERR(("Got bad value in case btuner_ip_stream"));
            break;
        }
        break;
#if B_HAS_RTSP
    case btuner_rtsp_stream: {
        if (list.total() < 3) {
            return false;
        }
        str->setType(btuner_rtsp_stream);
        strcpy(str->rtsp.url, list[2]);
        PROGRAM_INFO_T program;
        map()->add(str);
        int n = map()->total()-1;
        unsigned short pcrpid=0, vidpid=1, audpid=1, audtype=0, vidtype=0;
        ChannelMap::setProgram(&program, pcrpid, vidpid, audpid, audtype, vidtype);
        map()->addProgram(n, &program, NULL,
                false /* not scanned */);
        return false; /* we've already added entry */
    }
#endif

    default:
        return false;
    }
    return true;
}

