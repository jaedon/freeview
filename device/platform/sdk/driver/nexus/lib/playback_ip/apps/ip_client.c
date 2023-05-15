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
 * $brcm_Workfile: ip_client.c $
 * $brcm_Revision: 94 $
 * $brcm_Date: 10/30/12 11:16a $
 * 
 * Module Description:
 *  Example program to demonstrate receiving live or playback content over IP Channels (UDP/RTP/RTSP/HTTP based)
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_client.c $
 * 
 * 94   10/30/12 11:16a ssood
 * SW7231-858: disabled audioMaster mode, lowered the IpGetStatus calls to
 *  per 100msec interval
 * 
 * 93   10/22/12 11:54a ssood
 * SW7231-858: cleaned up the app shutdown sequence
 * 
 * 92   10/19/12 7:36a ssood
 * SW7425-4093: added test to simulate channel change
 * 
 * 91   10/16/12 5:36p ssood
 * SW7231-858: dont loop around by default + check if the URL played till
 *  the end
 * 
 * 90   9/18/12 5:56p ssood
 * SW7435-361: increase the default session setup time as it is not enough
 *  for playing encoded sessions
 * 
 * 89   9/15/12 3:08p ssood
 * SW7231-1010: increased the timeout value for allowing runtime buffering
 *  to complete
 * 
 * 88   9/11/12 3:57p leisun
 * SWSECURITY-48: DTCP-IP change. Auto-generating test keys, consolidate
 *  test/production build, make common DRM default ON
 * 
 * 87   8/20/12 8:55a ssood
 * SW7425-3541: Coverity fixes
 * 
 * 86   8/13/12 2:47p ssood
 * SW7346-911: initialize the Session Setup timeout to setupTimeout env
 *  variable value + specify the index type of self-indexing for nexus
 *  playback + added some more auto test cases + dont set gEof flag for
 *  error callback, instead wait for end of stream callback to finish
 *  playing the full stream
 * 
 * 85   8/3/12 4:44p ssood
 * SW7425-3678: increase the structure to hold the number of PSI entries
 * 
 * 84   7/19/12 4:07p gskerl
 * SW7346-853: Merged from branch SW7346-853 to main.
 * 
 * SW7346-853/2   7/10/12 3:27p gskerl
 * SW7346-853: Updated branch SW7346-853 from main/LATEST.
 * 
 * SW7346-853/1   7/9/12 1:41p gskerl
 * SW7346-853: Added support for MPEG-DASH. Added some additional BDBG_MSG
 *  prints. Commented out code that was enabling zeroDelayOutputMode for
 *  the video decoder (because it was causing jerkiness). Cleaned up some
 *  of the stcChannel setting logic.
 * 
 * 83   6/25/12 6:08p ssood
 * SW7435-250: coverity fixes exposed by new coverity version
 * 
 * 82   6/13/12 6:00p mward
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 81   6/5/12 6:49p ssood
 * SW7231-766: add support for downloading encrypted HLS keys
 * 
 * 80   5/25/12 2:58p ssood
 * SW7425-3042: added env variables for modifying userAgent & proxy fields
 * 
 * 79   5/23/12 3:13p ssood
 * SW7425-2894: suppressing extra debug log from ip client modules
 * 
 * 78   5/22/12 8:57a ssood
 * SW7425-3042: URI parsing was overriding the port specified with the
 *  default port
 * 
 * 77   5/17/12 6:36p ssood
 * SW7425-3042: simplified build flags & code by using nexus multi process
 *  capabilities
 * 
 * 76   5/14/12 4:33p ssood
 * SW7425-3042: refactor ip_streamer into multiple sub files
 * 
 * 75   5/4/12 7:11p ssood
 * SW7346-805: change the transport pacing timestamp type to 32bit binary
 * 
 * 74   4/23/12 5:22p ssood
 * SW7425-2860: added option to set session setup time via env variable
 * 
 * 73   4/18/12 6:16p celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 72   4/12/12 4:43p mananp
 * SW7425-2860:Adding limited time duration feature during live decode
 * 
 * 71   4/11/12 6:05p ssood
 * SW7425-2860: adding parsing support for HTTPS protocol
 * 
 * 70   4/11/12 4:03p ssood
 * SW7425-2860: adding parsing support for HTTPS protocol
 * 
 * 69   4/10/12 3:54p mananp
 * SW7425-2860:ip_client new features addition: limited time duration
 *  option for live decode, specify the whole URI from command line, set
 *  additionalHeader at runtime, return value of the code as per error
 *  status
 * 
 * 68   3/2/12 10:36a ssood
 * SW7231-692: fix coverity
 * 
 * 67   2/28/12 5:54p ssood
 * SW7231-692: update hdmi logic in ip_client
 * 
 * 66   2/9/12 3:31p ssood
 * SW7425-2319: Add support for IP Tuner Lock/Unlock mechanism
 * 
 * 65   1/17/12 5:40p ssood
 * SW7231-603: limit media probe time
 * 
 * 64   11/30/11 4:40p mward
 * SW7429-1:  Allow build with NEXUS_NUM_AUDIO_DACS 0 and/or
 *  NEXUS_NUM_SPDIF_OUTPUTS 0
 * 
 * 63   10/21/11 2:03p ssood
 * SW7231-405: fix scaler before capture for HLS sessions inorder to avoid
 *  black frames during bitrate/resolution transitions
 * 
 * 62   9/1/11 2:38p hongtaoz
 * SW7425-504: fixed quit errors;
 * 
 * 61   8/29/11 7:04p hongtaoz
 * SW7425-504: configure sync channel properly if audio is not present to
 *  avoid 3 seconds mute at decode;
 * 
 * 60   8/29/11 2:28p hongtaoz
 * SW7425-504: fixed PCR channel failure with video only test;
 * 
 * 59   8/25/11 8:11a jrubio
 * SW7125-1088: fix coverity issue
 * 
 * 58   8/25/11 8:01a jrubio
 * SW7125-1088: fix coverity issue
 * 
 * 57   8/23/11 4:42p jrubio
 * SW7344-181: coverity fix CID 35300
 * 
 * 56   8/23/11 4:35p jrubio
 * SW7344-181: coverity fix
 * 
 * 55   8/17/11 10:36a ssood
 * SW7425-1040: coverity fix
 * 
 * 54   8/7/11 7:00p ssood
 * SW7425-1040: more changes to support FCC over IP sessions
 * 
 * 53   8/4/11 4:45p ssood
 * SW7425-1040: add fast channel change support to IP Sessions
 * 
 * 52   7/14/11 11:09a ssood
 * SW7420-1978: get ip_client to work in the slave mode
 * 
 * 51   7/11/11 12:09p ssood
 * SW7231-268: add nexus sync channel support in ip_client for better AV
 *  lipsync
 * 
 * 50   6/21/11 9:43p qsong
 * SWDTV-7634: Make it build against latest 233 nexus.
 * 
 * 49   6/6/11 1:28a ssood
 * SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
 *  eventCallback support for end of stream playback
 * 
 * 48   6/3/11 5:31p ssood
 * SW7231-166: fixed api call sequence for the case where a format doesn't
 *  have any index info
 * 
 * 47   5/31/11 10:12a ssood
 * SW7231-166: require apps to set contentLengthHint for allowing
 *  trickmodes on streaming content from DLNA Servers using HTTP Chunk
 *  Xfer Encoding
 * 
 * 46   5/26/11 11:31p ssood
 * SW7231-166: dont create Nexus File Handle for cases where Nexus
 *  playpump will get used
 * 
 * 45   5/25/11 3:35p ssood
 * SW7231-166: fixed some bugs during failing trickplay cases
 * 
 * 44   5/24/11 11:35a ssood
 * SW7405-4392: Coverity fixes
 * 
 * 43   5/23/11 3:44p ssood
 * SW7231-166: Extend Playback IP trickmodes to be compatible with latest
 *  DLNA Guidelines
 * 
 * 42   5/19/11 4:28p ssood
 * SW7420-1774: Add support to decode SVC/MVC videos
 * 
 * 41   3/28/11 11:03a ssood
 * SW7420-502: reset the wait event
 * 
 * 40   3/21/11 5:39p ssood
 * SW7420-502: rvu support: enable timestamp mode if server sends 192byte
 *  TS packets
 * 
 * 39   3/8/11 4:00p ssood
 * SWDTV-5698: increase the timeout for session callback event
 * 
 * 38   2/24/11 1:42p ssood
 * SW7420-502: clock recovery support for RVU client
 * 
 * 37   2/13/11 4:38p root
 * SW7422-161:Add extra parameter to fix DTCP build error
 * 
 * 36   1/7/11 3:30p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 35   12/15/10 10:47a mward
 * SW7420-1217: Include nexus_dma.h to fix warning.
 * 
 * 34   12/14/10 2:09p ssood
 * SW7420-1217: support case where app doesn't a-priori know that it needs
 *  to play a HLS URI
 * 
 * 33   12/14/10 12:08p ssood
 * SW7420-1217: add support for encryption key parsing, downloading and
 *  receiving encrypted content
 * 
 * 32   12/7/10 12:10a ssood
 * SW7420-1257: fixed warning
 * 
 * 31   12/7/10 12:00a ssood
 * SW7420-1257: support for playing H264 encoded video conference stream
 * 
 * 30   11/24/10 2:58p ssood
 * SW7420-1257: Video Conferencing Demo: add support for receiving input
 *  camera input and stream it out
 * 
 * 29   11/15/10 5:10p ssood
 * SW7420-1217: limited file playback duration check to HLS protocol only
 * 
 * 28   10/29/10 9:07a ssood
 * SW7420-1217: basic support to play both variant & non-variant bounded
 *  playlists containing clear content
 * 
 * 27   10/8/10 6:24p ssood
 * SW7420-1088: extended ip_client to auto test trickmode cases
 * 
 * 26   10/8/10 10:41a mward
 * SWGERRARD-547: unused variable 'playbackIpStatus'.
 * 
 * 25   10/6/10 6:25p ssood
 * SWGERRARD-547: added flag for LPCM playback & auto testing
 * 
 * 24   9/24/10 5:49p ssood
 * SWGERRARD-547: added support for seeks on ES files (MP3, WMA w/o index)
 * 
 * 23   9/17/10 8:21a ssood
 * SWGERRARD-547: support to expose multiple multiple programs and tracks
 *  within a multi-program stream
 * 
 * 22   8/27/10 3:59p ssood
 * SW7420-502: added partial support to play RVU sessions in live mode
 * 
 * 21   4/16/10 5:02p ssood
 * SW7420-561: Added support for Additional Philips RTSP features:
 *  configurable keepalive timer, allow apps to send custom headers to
 *  servers, generate EOF events to app upon RTSP heartbeat failures
 * 
 * 20   4/2/10 5:57p ssood
 * SW7420-561: add support to handle large URLs (HTML5 URLs can be over
 *  256bytes) + Coverity fixes
 * 
 * 19   3/15/10 4:40p ssood
 * SW7125-233: Coverity fixes
 * 
 * 18   3/10/10 12:39p ssood
 * SW7420-561: removed nexus dependency for simple file streaming (no
 *  DTCP/IP or Live streaming support)
 * 
 * 17   3/4/10 12:17p ssood
 * SW7420-561: merge from branch to mainline
 * 
 * SW7420-561/3   3/4/10 11:53a ssood
 * SW7420-561: added comments
 * 
 * SW7420-561/2   2/16/10 10:42p ssood
 * SW7420-561: Changes to support basic RTSP w/o trickmodes
 * 
 * SW7420-561/1   2/16/10 10:13a ssood
 * SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
 *  Philips RTSP requirements
 * 
 * 15   1/23/10 10:17a ssood
 * SW7420-454: further changes to condense the ip playback log
 * 
 * 14   1/20/10 5:30p ssood
 * SW7420-454: added option to compile out auto PSI detection via
 *  AUTO_PSI_SUPPORT=n
 * 
 * 13   12/22/09 11:11a ssood
 * SW7420-454: switch pause method to connection stalling to demonstrate
 *  pause on live channels
 * 
 * 12   12/16/09 3:28p ssood
 * SW7420-454: extend ip_client app to support fast rewind
 * 
 * 11   11/19/09 9:23p ssood
 * SW7420-454: no need to set playback override flag to av decoders, stc
 *  channel ePcr mode is enough to indicate live IP playback
 * 
 * 10   11/17/09 12:44p ssood
 * SW7420-454: fixed compilation warning
 * 
 * 9   11/17/09 12:41p ssood
 * SW7420-454: add an option for dtcp ake port
 * 
 * 8   10/28/09 9:48a ssood
 * SW7405-2681: add nexus error callback
 * 
 * 7   10/6/09 12:18p ssood
 * SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
 * 
 * 6   9/25/09 3:43p ismailk
 * SW7405-3080: Include "b_dtcp_applib.h" under #ifdef B_HAS_DTCP_IP
 * 
 * 5   9/21/09 12:09p ssood
 * SW7405-2559: Adding LPCM playback support
 * 
 * 4   9/16/09 4:20p ssood
 * SW7420-340: print debug stats
 *
 *
 * 
 ******************************************************************************/
#include "ip_includes.h"
#ifdef B_HAS_DTCP_IP
#include "b_dtcp_applib.h"
#endif
#include "b_os_lib.h"

#include "b_psip_lib.h"
#include "ip_psi.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder_primer.h"
#include "nexus_video_decoder_extra.h"
#include "bmedia_probe.h"
#include "nexus_dma.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif

BDBG_MODULE(ip_client);
#define CA_CERT "./host.cert"

/* Test App to demonstrate the Live IP tuning using Nexus IP Applib */
#define IP_NETWORK_JITTER 300
#define TOTAL_PRIMERS 1

/*TODO - For now use these three Nexus as globals.  Should ideally be placed */
/*       within context varible of HDMI callback                           */
NEXUS_AudioDecoderStartSettings audioProgram;
NEXUS_AudioDecoderHandle pcmDecoder = NULL, compressedDecoder = NULL;

bool audioStarted = false;

typedef struct {
    int iphVersion;
    char host[256];     /* IP Address of Receiving Host */
    int port;     /* Port # */
    char *url;      /* URL */
    B_PlaybackIpProtocol protocol; /* Protocol: UDP/RTP */
    bool useLiveIpMode;
    bool decoderStats;
    B_PlaybackIpSecurityProtocol security;  /* which security protocol to use */
    unsigned int preChargeTime;  /* how many seconds to precharge the buffers before display begings */
    int initialSeekTime; /* how many seconds to seek into the media before starting the initial playback */
    unsigned decodeTimeDuration; /*For how many seconds the live decode/playback will persist  */
    bool loop; /* loop around at end */
    bool playMp3; /* play MP3 */
    bool playLpcm; /* play LPCM */
    int secondsToJump;
    int dtcpAkePort;
	int dtcpKeyFormat;
	bool ckc_check;
    bool slaveMode;
    bool runUnitTests;
    bool playMultipleStreams;
    bool skipPsiParsing;
    bool fastChannelChange;
}ExampleIpCfg;

int feedToPlayback = 0;

void usage(void)
{
    printf("Usage: ip_client [options] [<URI>]\n");
    printf("Usage: ip_client -d <ip> -p <port> [-t <num>] [-u <url>] [-v <num>] [-S <num>] [-b <num>] [-i <num>] [-l <num>] [-T <num>] -j <num> [-c] [-y] [-k] [-f] [-h] \n");
    printf("options are:\n");
    printf(" <uri>          # Complete URI specifying protocol, server, port number and url\n");
    printf("                # e.g. nexus ip_client -S 2 http://192.168.1.109:5000/AbcMpeg2HD.mpg\n");
    printf("                # e.g. nexus ip_client https://192.168.1.109/AbcMpeg2HD.mpg\n");
    printf("                # e.g. nexus ip_client udp://192.168.1.109:1234\n");
    printf("                # e.g. nexus ip_client rtp://192.168.1.109:1234\n");
    printf(" -d <ip>        # IP address of Live IP Channel (e.g. 192.168.1.110)\n");
    printf(" -p <port>      # Port of Live IP Channel (e.g. 1234)\n");
    printf(" -t <0|1|2|3|4> # protocol: UDP(0)|RTP(1)|RTSP(2)|HTTP(3)|RTP w/o RTCP(4), default is UDP\n");
    printf(" -u <url>       # URL for RTSP & HTTP protocols\n");
    printf(" -v             # IP version to use: IPv4: 4, IPv6: 6, default is IPv4\n");
    printf(" -s             # print decoder stats\n");
    printf(" -S <0|1|2|3|4> # Security: none(0) | SSL(1) | DTCP-IP(2) | RAD-EA(3) | AES128(4), default is none\n");
    printf(" -b <time>      # pre-charge the buffers for <time> seconds, default is 0\n");
    printf(" -i <time>      # initial seek time in seconds, default is 0\n");
    printf(" -j <num>       # jump forward/backward by these many seconds when j is pressed\n");
    printf(" -l <num>       # loop around after end, default is false\n");
    printf(" -T <num>       # number of seconds the live decode/playback will continue for\n");
    printf(" -n <ake-port>  # DTCP/IP AKE Port, default 8000\n");
	printf(" -K <0|1|2>		# DTCP/IP key format: test(0) | common DRM(1) | legacy production(2), default is test\n");
	printf(" -C             # DTCP/IP: Enable Content Key Confirmation procedure for Sink device\n");
    printf(" -m             # play MP3 (skips media probing)\n");
    printf(" -z             # run in slave mode\n");
    printf(" -y             # run basic unit tests on the stream (seek, pause, ff, fr, etc.)\n");
    printf(" -c             # play lpcm file\n");
    printf(" -k             # skip psi parsing (PSI info is harded in ip_client.c) \n");
    printf(" -f             # use fast channel \n");
    printf(" -x             # play multiple files back to back (channel change)\n");
    printf(" -h             # prints this usage\n");
    printf(" env variables:\n");
    printf(" additionalHttpHeader=<Browser Cookie String>      # set it if URL is obtained via a Browser session requiring a HTTP Cookie\n");
    printf(" setupTimeout=<SessionSetupTimeout in msec>        # set this to high value in sec (say 20sec) if receiving HTTP session from a Internet sever \n");
    printf(" userAgent=<custom userAgent string>               # userAgent string\n");
    printf(" useProxy=<1 | 0>                                  # set to 1 if all outgoing HTTP requests must flow thru a proxy (whose address is specified via the -d option) \n");
    printf("\n");
}

char  *_getEnvVariableValue(char *pName, char *defaultValue)
{
    char *value;
    int valueLen;
    char *pValue;
    
    pValue = getenv(pName);

    if (pValue != NULL) {
        value = pValue;
        if (strncmp(pName, "additionalHeader", strlen("additionalHeader")) == 0) {
            valueLen = strlen(pValue)+2;
            if ((value = malloc(valueLen)) != NULL) {
                strncpy(value, pValue, strlen(pValue));
                strncat(value, "\r\n", valueLen-1);
            }
            else {
                return NULL;
            }
        }
    }
    else{
        value = defaultValue;
    }
    
    BDBG_MSG(("%s: %s = %s", __FUNCTION__, pName, value));
    return value;
}


static void set_default_ip_cfg(ExampleIpCfg *ipCfg)
{
    memset(ipCfg, 0, sizeof(ExampleIpCfg));

    ipCfg->protocol = B_PlaybackIpProtocol_eUdp;
    ipCfg->iphVersion = 4;
    ipCfg->url = NULL;
    ipCfg->decoderStats = false;
    ipCfg->decodeTimeDuration = 0;
    ipCfg->preChargeTime = 0;
    ipCfg->security = B_PlaybackIpSecurityProtocol_None;
    ipCfg->initialSeekTime = 0;
    ipCfg->loop = false;
    ipCfg->playMp3 = false;
    ipCfg->dtcpAkePort = 8000;
    ipCfg->playLpcm = false;
    ipCfg->secondsToJump = 5;
    ipCfg->slaveMode = false;
    ipCfg->runUnitTests = false;
    ipCfg->playMultipleStreams = false;
    ipCfg->fastChannelChange = false;
}

extern char *B_PlaybackIp_UtilsStristr(char *str, char *subStr);
int parse_url(char *urlstr, ExampleIpCfg *cfg)
{
    char *tmpPtr, *tmpPtr1, *tmpPtr2;
    
    /* valid exampled of urlstr: */
    /* "http://player.vimeo.com:80/play_redirect?quality=hd&codecs=h264&clip_id=638324" */
    /* "/play_redirect?quality=hd&codecs=h264&clip_id=638324" */

    tmpPtr = strstr(urlstr, "\"");
    if (tmpPtr) {
        urlstr += 1;
        urlstr[strlen(urlstr)-1] = '\0';
    }
    tmpPtr = strstr(urlstr, ":");
    if (tmpPtr) {
        *tmpPtr = '\0';
        if (B_PlaybackIp_UtilsStristr(urlstr,"udp")) {
            cfg->protocol = B_PlaybackIpProtocol_eUdp;
        }
        else if (B_PlaybackIp_UtilsStristr(urlstr,"rtp")) {
            cfg->protocol = B_PlaybackIpProtocol_eRtp;
        }
        else if (B_PlaybackIp_UtilsStristr(urlstr,"rtsp")) {
            cfg->protocol = B_PlaybackIpProtocol_eRtsp;
        }
        else if (B_PlaybackIp_UtilsStristr(urlstr,"https")) {
            cfg->protocol = B_PlaybackIpProtocol_eHttp;
            if (!cfg->port)
                cfg->port = 443;
            cfg->security = B_PlaybackIpSecurityProtocol_Ssl;
        }
        else if (B_PlaybackIp_UtilsStristr(urlstr,"http")) {
            cfg->protocol = B_PlaybackIpProtocol_eHttp;
            if (!cfg->port)
                cfg->port = 80;
        }
        else {
            BDBG_ERR(("Incorrect or not yet supported Protocol Option (%d)", cfg->protocol));
            return -1;
        }
    }
    else {
        /* not a complete absolute url */
        cfg->url = urlstr;
        return 0;
    }

    /* restore the original string */
    *tmpPtr = ':';
    tmpPtr +=3; /* now tmpPtr points to the start of the server name */
    tmpPtr1 = strstr(tmpPtr, "/");
    /* parse for host name & port number */
    if (tmpPtr1)
        *tmpPtr1 = '\0';
    tmpPtr2 = strstr(tmpPtr, ":");
    if (tmpPtr2) {
        *tmpPtr2 = '\0';
        cfg->port = atoi(tmpPtr2+1);
    }
    memset(cfg->host, 0, sizeof(cfg->host));
    strncpy(cfg->host, tmpPtr, sizeof(cfg->host)-1);
    if (tmpPtr1) 
        *tmpPtr1 = '/';
    /* rest of the string is the URL */
    cfg->url = tmpPtr1;
    return 0;
}

static int parse_options(int argc, char *argv[], ExampleIpCfg *ipCfg)
{
    int ch;
    while ( (ch = getopt(argc, argv, "d:p:v:t:T:u:sS:b:i:l:j:n:K:zmcxykfhC")) != -1) {
        switch (ch) {
        case 'd':
            strncpy(ipCfg->host, optarg, sizeof(ipCfg->host)-1);
            break;
        case 'p':
            ipCfg->port = atoi(optarg);
            break;
        case 'n':
            ipCfg->dtcpAkePort = atoi(optarg);
            break;
		case 'K':
			ipCfg->dtcpKeyFormat = atoi(optarg);
			break;
		case 'C':
			ipCfg->ckc_check = true;
			break;
        case 't':
            ipCfg->protocol = (B_PlaybackIpProtocol)atoi(optarg);
            if (ipCfg->protocol >= B_PlaybackIpProtocol_eMax) {
                BDBG_ERR(("Incorrect Protocol Option (%d)\n", ipCfg->protocol));
                goto usage;
            }
            break;
        case 'u':
            ipCfg->url = optarg;
            BDBG_MSG(("uri: %s", optarg));
            break;
        case 'v':
            ipCfg->iphVersion = atoi(optarg);
            if (ipCfg->iphVersion != 4 && ipCfg->iphVersion != 6) {
                BDBG_ERR(("Incorrect IP Version (%d)\n", ipCfg->iphVersion));
                goto usage;
            }
            break;
        case 'f':
            ipCfg->fastChannelChange = true;
            break;
        case 's':
            ipCfg->decoderStats = true;
            break;
        case 'S':
            ipCfg->security = (B_PlaybackIpSecurityProtocol)atoi(optarg);
            if (ipCfg->security > B_PlaybackIpSecurityProtocol_Max) {
                BDBG_ERR(("Incorrect Security Option (%d)\n", ipCfg->security));
                goto usage;
            }
            break;
        case 'b':
            ipCfg->preChargeTime = atoi(optarg);
            break;
        case 'T':
            ipCfg->decodeTimeDuration = atoi(optarg);
            break;
        case 'i':
            ipCfg->initialSeekTime = atoi(optarg);
            if (ipCfg->initialSeekTime < 0) {
                BDBG_ERR(("Incorrect initial seek time (%d)\n", ipCfg->initialSeekTime));
                goto usage;
            }
            break;
        case 'j':
            ipCfg->secondsToJump = atoi(optarg);
            break;
        case 'l':
            ipCfg->loop = atoi(optarg);
            break;
        case 'm':
            ipCfg->playMp3 = true;
            break;
        case 'z':
            ipCfg->slaveMode = true;
            break;
        case 'c':
            ipCfg->playLpcm = true;
            break;
        case 'y':
            ipCfg->runUnitTests = true;
            break;
        case 'x':
            ipCfg->playMultipleStreams = true;
            break;
        case 'k':
            ipCfg->skipPsiParsing = true;
            break;
        case 'h':
        default:
usage:
            usage();
            exit(1);
        }
    }

    if (ipCfg->url == NULL) {
        if (parse_url(argv[argc-1],ipCfg) < 0) {
            BDBG_ERR(("Incorrect uri: %s", optarg));
            goto usage;
        }
        BDBG_MSG (("URL is %s:%d%s", ipCfg->host, ipCfg->port, ipCfg->url));
    }


    if ((ipCfg->protocol == B_PlaybackIpProtocol_eRtsp || ipCfg->protocol == B_PlaybackIpProtocol_eHttp) && (ipCfg->url == NULL)) {
        BDBG_ERR(("ERROR: RTSP & HTTP protocols require URL option\n"));
        usage();
        return -1;
    }
    BDBG_WRN(("Options are: \n\tHost: %s\n\tPort %d\n\tProtocol %s\n\tUrl %s\n\tIpVersion %d\n",
            ipCfg->host, ipCfg->port, 
            ipCfg->protocol == B_PlaybackIpProtocol_eUdp ? "UDP" : 
            ipCfg->protocol == B_PlaybackIpProtocol_eRtp ? "RTP" : 
            ipCfg->protocol == B_PlaybackIpProtocol_eRtpNoRtcp ? "RTP no RTCP" : 
            ipCfg->protocol == B_PlaybackIpProtocol_eRtsp ? "RTSP" : "HTTP",
            ipCfg->url,
            ipCfg->iphVersion));
    return 0;
}



ExampleIpCfg ipCfg;
int ip_client_setup(int argc, char *argv[])
{

    /* set the IP socket related defults for program */
    set_default_ip_cfg(&ipCfg);

    /* parse command line options */
    if (parse_options(argc, argv, &ipCfg)) {
        BDBG_WRN(("ERROR: Incorrect Options\n"));
        goto error;
    }
    
    return 0;

error:
    return 1;
}

void setDefaultPsiSettings(B_PlaybackIpPsiInfo *psi)
{
    memset(psi, 0, sizeof(*psi));
    psi->psiValid = true;
    psi->videoCodec = NEXUS_VideoCodec_eMpeg2;
    psi->videoPid = 0x11;
    psi->audioCodec = NEXUS_AudioCodec_eAc3;
    psi->audioPid = 0x14;
    psi->pcrPid = 0x11;
    psi->mpegType = NEXUS_TransportType_eTs;
    BDBG_MSG (("Setting Default PSI Settings: vpid %d, vcodec %d, apid %d, acodec %d, container %d", psi->videoPid, psi->videoCodec, psi->audioPid, psi->audioCodec, psi->mpegType));
}

bool gEof = false;
static void errorCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    BDBG_ERR((" #### Error callback, let the playback finish via the endOfStreamCallback #### \n"));
}

static void endOfStreamCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    gEof = true;
    BDBG_ERR((" #### End of stream reached #### \n"));
}

/* returns 0 for successful buffering or -1 otherwise */
int preChargeNetworkBuffer(B_PlaybackIpHandle playbackIp, unsigned int preChargeTime /* in secs */)
{
    NEXUS_Error rc;
    B_PlaybackIpSettings playbackIpSettings;
    B_PlaybackIpStatus playbackIpStatus;
    NEXUS_PlaybackPosition prevBufferDuration = 0;
    int noChangeInBufferDepth = 0;

    /* check for EOF condition */
    if (gEof) {
        BDBG_WRN(("Buffering Aborted due to EOF\n"));
        return -1;
    }

    /* check how much data is currently buffered: this is from the last seek point */
    BKNI_Sleep(100);    /* sleep little bit to get IP thread finish off any buffering */
    rc = B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus);
    if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}
    if (playbackIpStatus.curBufferDuration >= (1000*preChargeTime)) {
        BDBG_WRN(("Already buffered %lu milli-sec of data, required %u milli-sec of buffer...\n", playbackIpStatus.curBufferDuration, preChargeTime*1000));
        return 0;
    }

    /* underflowing in network buffer, tell IP Applib to start buffering */
    BDBG_WRN(("Start pre-charging n/w buffer: currently buffered %lu milli-sec of data, required %u milli-sec of buffer...\n", playbackIpStatus.curBufferDuration, preChargeTime*1000));
    memset(&playbackIpSettings, 0, sizeof(playbackIpSettings));
    playbackIpSettings.preChargeBuffer = true;
    rc = B_PlaybackIp_SetSettings(playbackIp, &playbackIpSettings);
    if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}

    /* now monitor the buffer depth until it reaches the desired preChargeTime value */
    do {
        BKNI_Sleep(100);
        rc = B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}
        BDBG_WRN(("currently buffered %lu milli-sec of data, max duration %d, required %u milli-sec of buffer...\n", playbackIpStatus.curBufferDuration, playbackIpStatus.maxBufferDuration, preChargeTime*1000));
        if (playbackIpStatus.curBufferDuration >= playbackIpStatus.maxBufferDuration) {
            BDBG_WRN(("currently buffered %lu max duration %d milli-sec worth data, so done buffering \n", playbackIpStatus.curBufferDuration, playbackIpStatus.maxBufferDuration));
            break;
        }
        if (!prevBufferDuration)
            prevBufferDuration = playbackIpStatus.curBufferDuration;
        else {
            if (prevBufferDuration == playbackIpStatus.curBufferDuration)
                noChangeInBufferDepth++;
            else  {
                noChangeInBufferDepth = 0;
                prevBufferDuration = playbackIpStatus.curBufferDuration;
            }
        }
        if (noChangeInBufferDepth >= 1000) {
            BDBG_WRN(("Warning: can't buffer upto the required buffer depth, currently buffered %lu max duration %d milli-sec worth data, so done buffering \n", playbackIpStatus.curBufferDuration, playbackIpStatus.maxBufferDuration));
            break;
        }
        /* keep buffering until we have buffered upto the high water mark or eof/server close events happen */
    } while (playbackIpStatus.curBufferDuration < (1000*preChargeTime) && !gEof && !playbackIpStatus.serverClosed);  

    /* tell IP Applib to stop buffering */
    playbackIpSettings.preChargeBuffer = false;
    rc = B_PlaybackIp_SetSettings(playbackIp, &playbackIpSettings);
    if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}

    if (gEof) {
        BDBG_WRN(("Buffering Aborted due to EOF, buffered %lu milli-sec worth of data...\n", playbackIpStatus.curBufferDuration));
        return -1;
    }
    else if (playbackIpStatus.serverClosed) {
        BDBG_WRN(("Can't Buffer anymore due to server closed connection, buffered %lu milli-sec worth of data...\n", playbackIpStatus.curBufferDuration));
        /* Note: this is not an error case as server may have closed connection, but we may not have played all this data, so let playback finish */
        return 0;
    }
    else {
        BDBG_WRN(("Buffering Complete (buffered %lu milli-sec worth of data), serverClosed %d...\n", playbackIpStatus.curBufferDuration, playbackIpStatus.serverClosed));
        return 0;
    }
}

#define EVENT_WAIT_TIME_IN_MSEC 30000
B_PlaybackIpEventIds gEventId = B_PlaybackIpEvent_eMax;
BKNI_EventHandle waitEvent = NULL;
void playbackIpEventCallback(void *appCtx, B_PlaybackIpEventIds eventId)
{
#if 0
    /* App can choose to either directly call the Ip API here or send an event to the app thread */
    /* which can actually make the Ip API call. It can use the eventId to determine which call to make */
    NEXUS_Error rc;
    rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSsessionSetupSettings, &ipSessionSetupStatus);
#endif
    gEventId = eventId;
    if (eventId == B_PlaybackIpEvent_eServerEndofStreamReached)
        gEof = 1;
    else {
        BKNI_SetEvent(waitEvent);
    }
    BDBG_MSG (("%s: Got EventId %d from IP library, appCtx %p", __FUNCTION__, eventId, appCtx));
}

int 
runPlaybackIpUnitTests(B_PlaybackIpPsiInfo *psi, B_PlaybackIpHandle playbackIp)
{
    B_PlaybackIpTrickModesSettings ipTrickModeSettings;
    int rc = -1;
    B_PlaybackIpStatus playbackIpStatus;
    B_PlaybackIpStatus playbackIpStatusNew;

	/* ip_client is also used for auto testing media streaming over IP */
	
	/* run tests after a stream is played for sometime */
	sleep(10);

    /* Pause */
    B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
    ipTrickModeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseConnectionStalling;
    if (B_PlaybackIp_Pause(playbackIp, &ipTrickModeSettings)) {BDBG_ERR(("ERROR: Failed to pause Ip playback\n")); goto out;}
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (playbackIpStatus.ipState != B_PlaybackIpState_ePaused) {BDBG_WRN(("Pause failed: state is %d", playbackIpStatus.ipState)); goto out;}
    sleep(1);
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatusNew)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (playbackIpStatus.position != playbackIpStatusNew.position) {BDBG_WRN(("Pause failed: positions are %d, %d", playbackIpStatus.position, playbackIpStatusNew.position)); goto out;}
    BDBG_WRN(("*********** Successfully Paused Ip playback*********** \n"));
	sleep(5);

    /* Play */
    if (B_PlaybackIp_Play(playbackIp)) {BDBG_ERR(("ERROR: Failed to pause-resume Ip playback\n")); goto out;}
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (playbackIpStatus.ipState != B_PlaybackIpState_ePlaying) {BDBG_WRN(("Pause-Resume failed: state is %d", playbackIpStatus.ipState)); goto out;}
    sleep(2);
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatusNew)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (playbackIpStatus.position == playbackIpStatusNew.position || playbackIpStatusNew.position < playbackIpStatus.position) {BDBG_WRN(("Pause-Resume failed: positions are %d, %d", playbackIpStatus.position, playbackIpStatusNew.position)); goto out;}
    BDBG_WRN(("*********** Successfully Pause-Resumed Ip playback*********** \n"));
	sleep(5);

	/* seek 110sec fwd */
    B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
    ipTrickModeSettings.seekPosition = 110*1000; /* in msec */
    if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) {BDBG_ERR(("ERROR: Failed to seek Ip playback\n")); goto out;}
    sleep(1);
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatusNew)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (105000 > playbackIpStatusNew.position) {BDBG_WRN(("Seek to 110sec position failed: current position %d", playbackIpStatusNew.position)); goto out;}
    BDBG_WRN(("*********** %s: seeked successfully to 110sec *********** ", __FUNCTION__));
    sleep(10);

	/* seek 60sec fwd */
    B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
    ipTrickModeSettings.seekPosition = 60*1000; /* in msec */
    if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) {BDBG_ERR(("ERROR: Failed to seek Ip playback\n")); goto out;}
    sleep(1);
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatusNew)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (55000 > playbackIpStatusNew.position) {BDBG_WRN(("Seek to 60sec position failed: current position %d", playbackIpStatusNew.position)); goto out;}
    BDBG_WRN(("*********** %s: seeked successfully to 60sec *********** ", __FUNCTION__));
    sleep(10);

	/* case: seek relative 30sec backward */
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
    ipTrickModeSettings.seekPositionIsRelative = true;
    ipTrickModeSettings.seekBackward = true;
    ipTrickModeSettings.seekPosition = 30*1000; /* in msec */
    if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) {BDBG_ERR(("ERROR: Failed to seek Ip playback\n")); goto out;}
    sleep(2);
    if (B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatusNew)) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); goto out;}
    if (playbackIpStatus.position-31000 > playbackIpStatusNew.position) {BDBG_WRN(("Seek relative back by 30sec position failed: positions is %d, %d", playbackIpStatus.position, playbackIpStatusNew.position)); goto out;}
    BDBG_WRN(("*********** %s: successfully seeked backward to 30sec *********** ", __FUNCTION__));
	sleep(10);
	
	/* skip trickplay tests for audio files */
    if (psi->videoPid == 0) {
        rc = 0;
        goto out;
    }

	/* trickplay ff */
    B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
    if (psi->numPlaySpeedEntries > 1) {
        ipTrickModeSettings.method = B_PlaybackIpTrickModesMethod_UsePlaySpeed;
        ipTrickModeSettings.rate = psi->playSpeed[8];
        if (ipTrickModeSettings.rate >= psi->httpMinIFrameSpeed)
            ipTrickModeSettings.frameRepeat = psi->httpFrameRepeat;
        else
            ipTrickModeSettings.frameRepeat = 1;
        if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {BDBG_WRN(("Failed to set the trick play speed")); goto out;}
        BDBG_WRN(("****************** FF Started ***************************"));
	    sleep(10);
        if (B_PlaybackIp_Play(playbackIp)) {BDBG_ERR(("ERROR: Failed to resume from trickplay \n")); goto out;}
	    sleep(15);

        /* rewind test */
        ipTrickModeSettings.rate = psi->playSpeed[2];
        if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {BDBG_WRN(("Failed to set the trick play speed")); goto out;}
        BDBG_WRN(("****************** FR Started ***************************"));
	    sleep(10);
        if (B_PlaybackIp_Play(playbackIp)) {BDBG_ERR(("ERROR: Failed to resume from trickplay \n")); goto out;}
    }
    else {
        ipTrickModeSettings.method = B_PlaybackIpTrickModesMethod_UseByteRange;
        ipTrickModeSettings.rate = 5;
        if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {BDBG_WRN(("Failed to do client side trickmodes ")); goto out;}
        BDBG_WRN(("****************** FF Started ***************************"));
	    sleep(10);
        if (B_PlaybackIp_Play(playbackIp)) {BDBG_ERR(("ERROR: Failed to resume from trickplay \n")); goto out;}
	    sleep(15);

        /* fast rewind */
        ipTrickModeSettings.method = B_PlaybackIpTrickModesMethod_UseByteRange;
        ipTrickModeSettings.rate = -5;
        if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {BDBG_WRN(("Failed to do client side trickmodes ")); goto out;}
        BDBG_WRN(("****************** FR Started ***************************"));
	    sleep(10);
        if (B_PlaybackIp_Play(playbackIp)) {BDBG_ERR(("ERROR: Failed to resume from trickplay \n")); goto out;}
    }
	sleep(15);
    rc = 0;
	
out:
    BDBG_WRN(("****************** AUTO Test Completed ***************************"));
    return rc;
}

/* logic to determine whether IP library should use Nexus Playpump to feed network stream or would it get pulled via Nexus Playback module */
static bool
useNexusPlaypumpForIpPlayback(
    B_PlaybackIpPsiInfo *psi,
    B_PlaybackIpProtocol protocol
    )
{
    /* For RTP, UDP, RTSP based protocols, use Nexus Playpump as data is being pushed by the server and thus can't use the Nexus playback as it pulls data from IP library */
    if (protocol != B_PlaybackIpProtocol_eHttp && protocol != B_PlaybackIpProtocol_eHttps) {
        return true;
    }

    /* For HTTP/S, we can use both Nexus Playback or Playpump as it is a pull based protocol */

    /* For following cases & protocols, we will use Nexus Playpump */
    /* - Server side trickmodes are supported (either Playspeed or TimeSeekRange ) as Nexus playback can't maintain the stream position during trickplays */
    /* - HLS protocol: stream segments are downloaded from server first and then feed into playpump */
    /* - RVU protocol: is like Server supporting server side trickmodes, Nexus Playback can't track stream position during trickplays) */ 
    if (psi->numPlaySpeedEntries > 1 /* DLNA App should use DLNA PS flags as well */ || psi->hlsSessionEnabled || psi->mpegDashSessionEnabled || psi->liveChannel) {
        return true;
    }

    /* By default use the Nexus Playback for all protocols */
    return false;
}

/* logic to determine which clock recorvery mechanism should be programmed for IP Playback Channels */
static bool
useLiveModeForIpPlayback(
    B_PlaybackIpPsiInfo *psi,
    B_PlaybackIpProtocol protocol,
    B_PlaybackIpClockRecoveryMode *clockRecoveryModeForPlaybackIp
    )
{
    bool useLiveIpMode;

    /* psi discovery should be complete at this point, else we can't truly determine the clock mode and we default to pull mode */
    if (!psi->psiValid) {
        *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePull;
        useLiveIpMode = false;
    }

    if (psi->mpegType != NEXUS_TransportType_eTs) {
        /* for all non mpeg2 transport streams (ES, ASF, MP4, etc. formats), we are in the non-live mode */
        /* as there is *no* good mechanism for locking to the sender's clock (TTS or PCRs are missing) */
        /* this is true irrespective of the network protocol (HTTP vs. RTP/UDP) */
        *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePull;
        useLiveIpMode = false;
    }
    /* MPEG2 TS content */
    else if (protocol != B_PlaybackIpProtocol_eHttp && protocol != B_PlaybackIpProtocol_eHttps) {
        /* UDP, RTP, RTSP protocols carrying MPEG2 TS content are always put in live mode */
        /* TODO: add call into IP library to determine if incoming stream is a TTS stream or not */
        /* for now, use SyncSlipMode */
        /* For DLNA Apps, media profile will indicate if it is a TTS or TS stream */
        *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
        useLiveIpMode = true;
    }
    /* MPEG2 TS content in HTTP/HTTPS protocols only */
#if 0
    /* RVU check */
    else if (appCtx->rvuFlag) {
        /* RVU protocol: channels are always in live mode whether live or files from disk are being streamed & have 4 byte Timestamps */
        *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip;
    }
#endif
    /* Non RVU MPEG2 TS content in HTTP/HTTPS protocols only */
    else if (psi->liveChannel) {
        /* special check to auto detect if this channel is in live mode (true for Broadcom servers as they send this flag) */
        if (psi->transportTimeStampEnabled) {
            /* live channel & stream has 4 byte Timestamps, use TTS based pacing mode */
            *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip;
            useLiveIpMode = true;
        }
        else {
#if 1
            /* live channel & stream doesn't has 4 byte Timestamps, so use PCRs as timestamps for pacing */
            /* however, this PCR based basing is not yet fully tested, so default to pull mode for now */
            *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePushWithPcrNoSyncSlip;
            *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
            useLiveIpMode = true;
#else
            *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePull;
            useLiveIpMode = false;
#endif
        }
    }
    /* Non RVU MPEG2 TS content in HTTP/HTTPS protocols from non-Broadcom servers */
    else {
        /* App has to somehow to know whether this channel needs to be in Live or playback mode */
        /* this could be based on either pre-defined channel map or from a DLNA level flag */
        /* For DLNA apps, there is a VideoItem.VideoBroadcastItem flag indicates live mode */
        /* then if the media profile is TTS, we can be in TTS mode. Else, when PCR pacing */
        /* is supported, we can use PCR pacing mode. */
        /* and default would be pull mode even though it is a live stream */

        /* we default to pull mode */
        *clockRecoveryModeForPlaybackIp = B_PlaybackIpClockRecoveryMode_ePull;
        useLiveIpMode = false;
    }

    return useLiveIpMode;
}

static void sourceChangeCallback(void *context, int param)
{
    BSTD_UNUSED(context);
    BDBG_MSG (("Got Source (%s) change callback", param ==1 ? "video" : "audio"));
}

static NEXUS_PlatformSettings platformSettings;
int main(int argc, char *argv[])
{
    int i,errorFlag=0;
    NEXUS_Error rc = NEXUS_UNKNOWN;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_StcChannelSettings stcChannelSettings;
    NEXUS_PidChannelHandle videoPidChannel = NULL, audioPidChannel = NULL, pcrPidChannel = NULL, extraVideoPidChannel = NULL;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump = NULL;
    NEXUS_PlaypumpOpenPidChannelSettings pidChannelSettings;
    NEXUS_DisplayHandle display = NULL;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window = NULL;
    NEXUS_VideoDecoderHandle videoDecoder = NULL;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoDecoderSettings  videoDecoderSettings;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
    NEXUS_AudioDecoderSettings audioDecoderSettings;
    NEXUS_AudioOutputSettings audioOutputSettings;
    NEXUS_AudioOutput output;
    B_PlaybackIpOpenSettings playbackIpOpenSettings;
    B_PlaybackIpSessionStartSettings ipSessionStartSettings;
    B_PlaybackIpSessionStartStatus ipSessionStartStatus;
    B_PlaybackIpSessionOpenSettings ipSessionOpenSettings;
    B_PlaybackIpSessionOpenStatus ipSessionOpenStatus;
    B_PlaybackIpSessionSetupSettings ipSessionSetupSettings;
    B_PlaybackIpSessionSetupStatus ipSessionSetupStatus;
    B_PlaybackIpHandle playbackIp = NULL;
    NEXUS_PlaybackPidChannelSettings playbackPidChannelSettings;
    NEXUS_VideoDecoderPrimerHandle primer[TOTAL_PRIMERS];

    B_PlaybackIpTrickModesSettings ipTrickModeSettings;
    B_PlaybackIpStatus playbackIpStatus;
    B_PlaybackIpPsiInfo psi;
    NEXUS_PlaybackHandle playback = NULL;
    NEXUS_PlaybackSettings playbackSettings;
    B_PlaybackIpSettings playbackIpSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;
    char buf[16];
    int numbytes;
    psiCollectionDataType  collectionData;
    int numPrograms;
    /* coverity[stack_use_local_overflow] */
    B_PlaybackIpPsiInfo psiList[MAX_PROGRAMS_PER_FREQUENCY];
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;
    NEXUS_PlaybackStatus pbStatus;
    NEXUS_PlaypumpStatus ppStatus;
    uint32_t stc;
#ifdef B_HAS_DTCP_IP
    void * AkeHandle = NULL;
    void * dtcpCtx = NULL;
#endif
	NEXUS_DmaHandle dmaHandle;
    const bmedia_probe_track *track;
    const bmedia_probe_stream *stream;
    
#ifdef B_HAS_SSL
    void *ssl_ctx;
    B_PlaybackIpSslInitSettings sslInitSettings;
#endif
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
    NEXUS_SyncChannelHandle syncChannel;
#endif
    B_PlaybackIpError rrc = 0; 
    NEXUS_VideoWindowScalerSettings scalerSettings;

    B_Time prevTime,curTime;

    rrc = B_PlaybackIp_GetDefaultSettings(&playbackIpSettings);
    if (rrc !=B_ERROR_SUCCESS ) {
        BDBG_ERR((" Failed to initialize Playback IP Settings"));
        exit(1);
    }

    if (ip_client_setup(argc, argv)) {
        exit(1);
    }

    /* Dependencies: App must initialize OS lib */
    B_Os_Init();

    if (!ipCfg.slaveMode) {
        /* Bring up all modules for a platform in a default configuration for this platform */
        NEXUS_Platform_GetDefaultSettings(&platformSettings);
        platformSettings.openFrontend = false;
#if NEXUS_DTV_PLATFORM
        platformSettings.displayModuleSettings.panel.lvds.lvdsMappingRule = NEXUS_PanelOutputMappingRule_eOpenLdiRule; 
        platformSettings.displayModuleSettings.panel.lvds.customLinkSettings.lvdsLinkMap1 = NEXUS_LvdsLinkMap_eInternalLink1;
        platformSettings.displayModuleSettings.panel.lvds.customLinkSettings.lvdsLinkMap2 = NEXUS_LvdsLinkMap_eInternalLink2;
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[0].maskLinkHSync = 0;
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[0].maskLinkVSync = 0;
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[0].maskLinkDE    = 0;        
        
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[1].maskLinkHSync = 0;
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[1].maskLinkVSync = 0;
        platformSettings.displayModuleSettings.panel.lvds.linkChannel[1].maskLinkDE    = 0;
    
        if (strcmp("B552", getenv("panel_type")) == 0) {
            platformSettings.displayModuleSettings.panel.lvds.dvoLinkMode = NEXUS_PanelOutputLinkMode_eDualChannel1;
            platformSettings.displayModuleSettings.panel.lvds.lvdsColorMode = NEXUS_LvdsColorMode_e8Bit ;
            platformSettings.displayModuleSettings.panel.lvds.useNegativeEdge = false ;
    
            if (getenv("pixel_swap") != NULL && strcasecmp("yes", getenv("pixel_swap")) == 0) {
                platformSettings.displayModuleSettings.panel.lvds.dvoLinkMode = NEXUS_PanelOutputLinkMode_eCustom;
            }
        }
#endif
        rc = NEXUS_Platform_Init(&platformSettings);
    }
    else {
        rc = NEXUS_Platform_Join();
    }
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (BKNI_CreateEvent(&waitEvent)) { BDBG_ERR(("Failed to create an event at %d", __LINE__)); exit(1); }

#if NEXUS_HAS_SYNC_CHANNEL
    if (!ipCfg.fastChannelChange) {
        /* create a sync channel */
        NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
        syncChannelSettings.enablePrecisionLipsync = true;
        syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
        if (syncChannel == NULL) {BDBG_ERR(("NEXUS Error at %d, Exiting...\n", __LINE__)); exit(1);}
        BDBG_MSG (("Using Nexus STC channel for lipsync"));
    }
#endif

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Open the StcChannel */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcChannelSettings);
    stcChannelSettings.timebase = NEXUS_Timebase_e0;
    stcChannelSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcChannelSettings);
    if (!stcChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Open Video decoder */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    /* Increase the CDB size as it is used as the dejitter buffer */ 
    videoDecoderOpenSettings.fifoSize = 10 * 1024 * 1024;
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    if (!videoDecoder) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Open & setup audio decoders and connect outputs */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
    BDBG_MSG(("fifo size %d, type %d\n", audioDecoderOpenSettings.fifoSize, audioDecoderOpenSettings.type));
    audioDecoderOpenSettings.fifoSize = 2*512 * 1024;
    audioDecoderOpenSettings.type = NEXUS_AudioDecoderType_eDecode;
    /*TODO: pcmDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);*/
    /* */pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    if (!pcmDecoder) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}

    /*compressedDecoder = NEXUS_AudioDecoder_Open(1, &audioDecoderOpenSettings);*/
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    if (!compressedDecoder) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}

    /* ----------------------------------------------------------------------------------------------------------------*/


    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    /* Use default */
#else
    /* Note: change to display type back for panel output */
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    BDBG_MSG (("Display format %d", displaySettings.format));
#endif
    display = NEXUS_Display_Open(0, &displaySettings);
    if (!display) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    /* ----------------------------------------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------------------------------------*/
    window = NEXUS_VideoWindow_Open(display, 0);
    if (!window) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
#if 0
    if (ipCfg.skipPsiParsing) {
        NEXUS_VideoWindowSettings windowSettings;
        /* is app is asking to skip psi parsing, we are lowering the window size */
        /* this is mainly done for the video conferencing demo case as we know the PSI info in advance and want to use smaller display window */
        NEXUS_VideoWindow_GetSettings(window, &windowSettings);
        windowSettings.position.x = 50 + (70 * 2);
        windowSettings.position.y = 50 + (30 * 2);
        windowSettings.position.width = 640/2;
        windowSettings.position.height = 480/2;
        NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    }
#endif
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
#endif
#if 0
    /* Note: enable for composite output */
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if ( platformConfig.outputs.composite[0] ) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    }
#endif
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    /* Install hotplug callback -- video only for now */
    rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
#endif
#endif
    BDBG_MSG(("Display Manager is Opened\n"));

    /* connect video decoder to display */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* ----------------------------------------------------------------------------------------------------------------*/
#ifdef B_HAS_SSL
    if (ipCfg.security == B_PlaybackIpSecurityProtocol_Ssl){
        memset(&sslInitSettings, 0, sizeof(B_PlaybackIpSslInitSettings)); 
        sslInitSettings.rootCaCertPath=CA_CERT;
        sslInitSettings.clientAuth=false;
        sslInitSettings.ourCertPath=NULL;
        sslInitSettings.privKeyPath=NULL;
        sslInitSettings.password=NULL;
        ssl_ctx = B_PlaybackIp_SslInit(&sslInitSettings);
        if (!ssl_ctx) {
            BDBG_ERR(("SSL Security initialization failed\n"));
            exit(1);
        }
    }
#endif

	if ((dmaHandle = NEXUS_Dma_Open(0, NULL)) == NULL) {
        BDBG_ERR(("ERROR: NEXUS_Dma_Open failed"));
        return (-1);
    }
#ifdef B_HAS_DTCP_IP
    if (ipCfg.security == B_PlaybackIpSecurityProtocol_DtcpIp) {
        struct timeval t_start, t_finish;

#ifdef B_DTCP_IP_HW_DECRYPTION
        BDBG_MSG(("Initializing HW security params\n"));
        /* TODO: for VMS, we may need to pass in the nexus DMA handle here if it is already opened */
        if(DtcpInitHWSecurityParams(dmaHandle) != BERR_SUCCESS)
        {
            BDBG_ERR(("ERROR: Failed to init DtcpHW Security parmas\n"));
            return (-1);
        }
#endif

        /* initialize DtcpIp library */
        /* if App hasn't already opened the Nexus M2M DMA handle, then pass-in initial arg as NULL and let DTCP/IP library open the handle */
        if ((dtcpCtx = DtcpAppLib_Startup(B_DeviceMode_eSink, false, ipCfg.dtcpKeyFormat, ipCfg.ckc_check)) == NULL) {
            BDBG_ERR(("ERROR: DtcpAppLib_Startup failed\n"));
            return (-1);
        }
        if(dtcpCtx == NULL)
        {
            BDBG_ERR(("%s: Failed to Initialize Dtcp/Ip Library: DTCP/IP encryption/decryption won't work\n", __FUNCTION__));
            exit(rc);
        }

        /* Perform AKE for DTCP/IP */
        gettimeofday(&t_start, 0);
        BDBG_MSG(("host is %s, strlen %d\n", ipCfg.host, strlen(ipCfg.host)));
        if((rc = DtcpAppLib_DoAke(dtcpCtx, ipCfg.host, ipCfg.dtcpAkePort, &AkeHandle)) != BERR_SUCCESS) {
            BDBG_ERR(("DTCP AKE Failed!!!\n"));
            exit(rc);
        }
        gettimeofday(&t_finish, 0);
        BDBG_MSG (("-------------AKE took %d secs and %d msecs\n", 
            (t_finish.tv_usec < t_start.tv_usec? t_finish.tv_sec - t_start.tv_sec - 1: t_finish.tv_sec - t_start.tv_sec),
            (t_finish.tv_usec < t_start.tv_usec? t_finish.tv_usec - t_start.tv_usec + 1000000: t_finish.tv_usec - t_start.tv_usec) ));
    }
#endif

#ifdef B_HAS_RAD_EA
    if (ipCfg.security == B_PlaybackIpSecurityProtocol_RadEa){
        B_PlaybackIp_RadEaInit(0);
    }
#endif

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Open Nexus Playpump, For Live Modes, IP Applib directly feeds the network data to the Nexus Playpump */
    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
    BDBG_MSG (("## fifo size %d, desc %d", playpumpOpenSettings.fifoSize, playpumpOpenSettings.numDescriptors));
    playpumpOpenSettings.fifoSize *= 2;
    playpumpOpenSettings.numDescriptors = 200;
    playpump = NEXUS_Playpump_Open(0, &playpumpOpenSettings);
    if (!playpump) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    /* IP Applib currently uses the FIFO mode to feed data, so app needs to set that mode */
    playpumpSettings.mode = NEXUS_PlaypumpMode_eFifo;
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__));errorFlag=1; goto error;}
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Setup Nexus Playback, For Playback Modes (e.g. HTTP), IP Applib uses Nexus Playback to feed data */
    playback = NEXUS_Playback_Create();
    if (!playback) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Open IP Applib Handle */
    BDBG_MSG(("Initializing IP Applib...\n"));
    playbackIp = B_PlaybackIp_Open(&playbackIpOpenSettings);
    if (!playbackIp) {BDBG_WRN(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}

newSession:
    gEof = 0;
    gEventId = B_PlaybackIpEvent_eMax;
    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Setup the security & socket setting structure used in the IP Session Open */
    memset(&ipSessionOpenSettings, 0, sizeof(ipSessionOpenSettings));
    memset(&ipSessionOpenStatus, 0, sizeof(ipSessionOpenStatus));
    /* Security */
    if (ipCfg.security > B_PlaybackIpSecurityProtocol_None) {
        switch (ipCfg.security ) {
#ifdef B_HAS_SSL
        case B_PlaybackIpSecurityProtocol_Ssl: 
            ipSessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
            ipSessionOpenSettings.security.initialSecurityContext = ssl_ctx;
            ipSessionOpenSettings.security.dmaHandle = dmaHandle;
            break;
#endif
#ifdef B_HAS_DTCP_IP
        /* DTCP-IP */
        case B_PlaybackIpSecurityProtocol_DtcpIp:
            ipSessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_DtcpIp;
            ipSessionOpenSettings.security.initialSecurityContext = AkeHandle;
            break;
#endif
        case B_PlaybackIpSecurityProtocol_Aes128:
            BDBG_MSG (("Setting the dma handle for HLS encryption"));
            ipSessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Aes128;
            ipSessionOpenSettings.security.initialSecurityContext = dmaHandle;
            break;
        /* TODO RAD-EA */
        default:
            BDBG_ERR(("TODO: Security options\n"));
            break;
        }
    }
    else
        /* just pass the dma handle to IP library in case it needs it */
       ipSessionOpenSettings.security.initialSecurityContext = dmaHandle;

    /* Set IP Address, Port, and Protocol used to receive the AV stream */
    strncpy(ipSessionOpenSettings.socketOpenSettings.ipAddr, ipCfg.host, sizeof(ipSessionOpenSettings.socketOpenSettings.ipAddr)-1);
    ipSessionOpenSettings.socketOpenSettings.port = ipCfg.port;
    ipSessionOpenSettings.socketOpenSettings.protocol = ipCfg.protocol;
    memset(&psi, 0, sizeof(psi));
    switch (ipCfg.protocol) {
    case B_PlaybackIpProtocol_eUdp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
    case B_PlaybackIpProtocol_eRtp:
        ipSessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
        ipSessionOpenSettings.maxNetworkJitter = 300;
        ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
        ipSessionOpenSettings.eventCallback = playbackIpEventCallback;
        ipSessionOpenSettings.appCtx = playbackIp; /* this should be app handle */
        rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
        if (rc) {
            BDBG_ERR(("Session Open call failed: rc %d", rc));
            errorFlag=1;
            goto errorClose;
        }
        BDBG_MSG (("Session Open call succeeded"));
        break;
    case B_PlaybackIpProtocol_eRtsp:
        ipSessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
        ipSessionOpenSettings.maxNetworkJitter = 300;
        ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
        ipSessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eRtsp;
        ipSessionOpenSettings.socketOpenSettings.url = ipCfg.url;
        /* App can set this flag to enable non-blocking mode of IP library APIs */
        /* ipSessionOpenSettings.nonBlockingMode = true;*/
        ipSessionOpenSettings.nonBlockingMode = true;
        ipSessionOpenSettings.eventCallback = playbackIpEventCallback;
        ipSessionOpenSettings.appCtx = playbackIp; /* this should be app handle */
        /* apps can request IP library to return the RTSP Response Message incase it needs to extract any custom headers */
        ipSessionOpenSettings.u.rtsp.copyResponseHeaders = true;
        rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
        if (rc == B_ERROR_IN_PROGRESS) {
            /* app can do some useful work while SessionOpen is in progress and resume when callback sends the completion event */
            BDBG_MSG (("Session Open call in progress, sleeping for now..."));
            while (gEventId != B_PlaybackIpEvent_eSessionOpenDone)
                /* Note: instead of while loop, app can just wait on an event which is signalled by the callback function */
                BKNI_Sleep(100);
            /* So we got the event, call SessionOpen again to retrieve the session open status */
            rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
        }
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("Socket Open call failed: rc %d, HTTP Status %d\n", rc, ipSessionOpenStatus.u.rtsp.statusCode));
            errorFlag=1;
            goto errorClose;
        }
        BDBG_MSG (("Session Open call succeeded, RTSP status code %d", ipSessionOpenStatus.u.rtsp.statusCode));
        memset(&ipSessionSetupSettings, 0, sizeof(ipSessionSetupSettings));
        ipSessionSetupSettings.u.rtsp.userAgent = "Broadcom's LiveMedia based test RTSP Client";
        /* apps can request IP library to return the RTSP Response Message incase it needs to extract any custom headers */
        ipSessionSetupSettings.u.rtsp.copyResponseHeaders = true;
        /* Note: can optionally set the additionalHeaders field */
        rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSessionSetupSettings, &ipSessionSetupStatus);
        BKNI_ResetEvent(waitEvent);
        if (rc == B_ERROR_IN_PROGRESS) {
            BERR_Code err;
            /* app can do some useful work while SessionSetup is in progress and resume when callback sends the completion event */
            BDBG_MSG (("Session Setup call in progress, sleeping for now..."));
            err = BKNI_WaitForEvent(waitEvent, EVENT_WAIT_TIME_IN_MSEC);
            if (err == BERR_TIMEOUT || err != 0) {
                BDBG_WRN(("Session Setup call timed out or got error (err %d)!!!", err));
                errorFlag=1;
                goto error;
            }
            /* So we got the event, call SessionSetup again to retrieve the session setup status (psi & file handle) */
            rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSessionSetupSettings, &ipSessionSetupStatus);
        }
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("Session Setup call failed: rc %d\n", rc));
            errorFlag=1;
            goto error;
        }
        for (i=0; i<ipSessionSetupStatus.u.rtsp.scaleListEntries; i++)
            BDBG_MSG(("scale list[%d] = %0.1f", ipSessionSetupStatus.u.rtsp.scaleList[i]));
        break;
    case B_PlaybackIpProtocol_eHttp:
        ipSessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eHttp;
        ipSessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePull;
        ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
#if 0
        /* set this flag is App already knows the PSI info of the stream and it wants to use Nexus Playpump for feeding IP session data */
        ipSessionOpenSettings.useNexusPlaypump = 1;
#endif
        ipSessionOpenSettings.u.http.networkBufferSize = (30*3*1024*1024)/8; /* data cache size: 30sec worth for a max bitrate of 3Mbps */
        ipSessionOpenSettings.socketOpenSettings.url = ipCfg.url;
        ipSessionOpenSettings.socketOpenSettings.useProxy = strtol(_getEnvVariableValue("useProxy", "0"), (char **)NULL, 10);
#if 0
        /* If app needs to set any addition HTTP header fields that need to be sent in the outgoing HTTP Get request */
        /* they should be set in additionalHttpHeaderFields strings. E.g. for DLNA requests, app can specify the desired transfer mode */
        ipSessionOpenSettings.u.http.additionalHeaders = _getEnvVariableValue("additionalHeader", "Cookie: VISITOR_INFO1_LIVE=mRUZ7HDi4Yc;\r\n");
#endif
        ipSessionOpenSettings.u.http.additionalHeaders = _getEnvVariableValue("additionalHeader", "transferMode.dlna.org: Streaming\r\n");   /* Streaming mode for AV content */
        /* apps can override the default user agent string in the outgoing HTTP Get Request */

        ipSessionOpenSettings.u.http.userAgent = _getEnvVariableValue("userAgent", "BRCM IP Applib Test App/2.0");   /* Streaming mode for AV content */
        /* apps can request IP library to return the HTTP Response Message incase it needs to extract any custom headers */
        ipSessionOpenSettings.u.http.copyResponseHeaders = true;
        /* setup a callback for receiving various events from IP library */
        ipSessionOpenSettings.eventCallback = playbackIpEventCallback;
        ipSessionOpenSettings.appCtx = playbackIp; /* this should be app handle */
        /* App can set this flag to enable non-blocking mode of IP library APIs */
        /* ipSessionOpenSettings.nonBlockingMode = true;*/
        ipSessionOpenSettings.nonBlockingMode = true;
        rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
        if (rc == B_ERROR_IN_PROGRESS) {
            /* app can do some useful work while SessionOpen is in progress and resume when callback sends the completion event */
            BDBG_MSG (("Session Open call in progress, sleeping for now..."));
            while (gEventId != B_PlaybackIpEvent_eSessionOpenDone)
                /* Note: instead of while loop, app can just wait on an event which is signalled by the callback function */
                BKNI_Sleep(100);
            /* So we got the event, call SessionOpen again to retrieve the session open status */
            rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
        }
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("Session Open call failed: rc %d, HTTP Status %d\n", rc, ipSessionOpenStatus.u.http.statusCode));
            errorFlag=1;
            goto errorClose;
        }
        BDBG_MSG (("Session Open call succeeded, HTTP status code %d", ipSessionOpenStatus.u.http.statusCode));
        if (ipSessionOpenStatus.u.http.responseHeaders) {
            BDBG_MSG(("Response Header is... \n%s", ipSessionOpenStatus.u.http.responseHeaders));
            /* Note: App can extract any customer Response Headers here: useful to extract DLNA flags from getContentFeatures Header */

            /* now free the responseHeader */
            free(ipSessionOpenStatus.u.http.responseHeaders);
        }
        memset(&ipSessionSetupSettings, 0, sizeof(ipSessionSetupSettings));
        /* If app knows the media container type, then it can provide the hint to IP Applib */
        /*ipSessionSetupSettings.contentTypeHint = NEXUS_TransportType_eEs; */
        if (ipCfg.playMp3) {
            BDBG_MSG (("Playing Mp3, so tell IP Applib to skip auto PSI probing\n"));
            ipSessionSetupSettings.u.http.skipPsiParsing = false;
            ipSessionSetupSettings.u.http.avgBitRate = 192000;
            ipSessionSetupSettings.u.http.contentLengthHint = 2182835;
        }
        else if (ipCfg.playLpcm) {
            BDBG_MSG (("Playing LPCM, so tell IP Applib to skip auto PSI probing & Convert LPCM to WAV by inserting WAV header\n"));
            ipSessionSetupSettings.u.http.skipPsiParsing = true;
            ipSessionSetupSettings.u.http.avgBitRate = 192000; /* please set it if known */
            ipSessionSetupSettings.u.http.convertLpcmToWave = true;
            ipSessionSetupSettings.u.http.bitsPerSample = 16;
            ipSessionSetupSettings.u.http.sampleRate = 48000;
            ipSessionSetupSettings.u.http.numChannels = 2;
        }
#if 0
        /* If app figures out from getContentFeatures that the server doesn't support HTTP Range Headers, then it can set a flag to tell IP Applib to not include one in outgoing Get Req */
        ipSessionSetupSettings.u.http.disableRangeHeader = true;
        /* since Range header is not supported by server, then we can't seek to it and thus tell IP library to disable trick modes */
        ipSessionSetupSettings.u.http.dontUseIndex = true;
#endif

        /* if app needs to play multiple formats (such as a DLNA DMP/DMR) (e.g. TS, VOB/PES), then set this option to do deep payload inspection */
        ipSessionSetupSettings.u.http.enablePayloadScanning = true;

        /* set a limit on how long the psi parsing should continue before returning */
        ipSessionSetupSettings.u.http.psiParsingTimeLimit = strtol(_getEnvVariableValue("setupTimeout", "30000"), (char **)NULL, 10);

        rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSessionSetupSettings, &ipSessionSetupStatus);
        if (rc == B_ERROR_IN_PROGRESS) {
            BERR_Code err;
            /* app can do some useful work while SessionSetup is in progress and resume when callback sends the completion event */
            BDBG_MSG (("Session Setup call in progress, sleeping for now..."));
            err = BKNI_WaitForEvent(waitEvent, ipSessionSetupSettings.u.http.psiParsingTimeLimit);
            if (err == BERR_TIMEOUT || err != 0) {
                BDBG_WRN(("Session Setup call timed out or got error (err %d)!!!", err));
                errorFlag=1;
                goto errorClose;
            }
            /* So we got the event, call SessionSetup again to retrieve the session setup status (psi & file handle) */
            rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSessionSetupSettings, &ipSessionSetupStatus);
        }
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("Session Setup call failed: rc %d\n", rc));
            errorFlag=1;
            goto error;
        }
        psi = ipSessionSetupStatus.u.http.psi;

        BDBG_MSG (("psi.psiValid: %u ", psi.psiValid));
        BDBG_MSG (("psi.audioCodec: %u ",           psi.audioCodec));
        BDBG_MSG (("psi.audioPid: %u ",             psi.audioPid));
        BDBG_MSG (("psi.avgBitRate: %u ",           psi.avgBitRate));
        BDBG_MSG (("psi.extraVideoPid: %u ",        psi.extraVideoPid));
        BDBG_MSG (("psi.hlsSessionEnabled: %u ",    psi.hlsSessionEnabled));
        BDBG_MSG (("psi.liveChannel: %u ",          psi.liveChannel));
        BDBG_MSG (("psi.mpegDashSessionEnabled: %u ", psi.mpegDashSessionEnabled));
        BDBG_MSG (("psi.mpegType: %u ",             psi.mpegType));
        BDBG_MSG (("psi.pcrPid: %u ",               psi.pcrPid));
        BDBG_MSG (("psi.transportTimeStampEnabled: %u ", psi.transportTimeStampEnabled));
        BDBG_MSG (("psi.videoFrameRate: %u ",       psi.videoFrameRate));
        BDBG_MSG (("psi.videoHeight: %u ",          psi.videoHeight));
        BDBG_MSG (("psi.videoPid: %u ",             psi.videoPid));
        BDBG_MSG (("psi.videoWidth: %u ",           psi.videoWidth));

        BDBG_MSG (("Session Setup call succeeded, file handle %p", ipSessionSetupStatus.u.http.file));
        if (ipSessionSetupSettings.u.http.skipPsiParsing) 
            goto skipTrackInfo;
        stream = (bmedia_probe_stream *)(ipSessionSetupStatus.u.http.stream);
        if (!stream)
            goto skipTrackInfo;
        for (track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
            if (track->type==bmedia_track_type_video) {
                BDBG_MSG(("video track %u codec:%u\n", track->number, track->info.video.codec));
            }
            if (track->type==bmedia_track_type_pcr) {
                BDBG_MSG(("pcr pid %u\n", track->number));
            }
            else if(track->type==bmedia_track_type_audio) {
                BDBG_MSG(("audio track %u codec:%u\n", track->number, track->info.audio.codec));
            }
        }
skipTrackInfo:
        break;
    default:
        BDBG_WRN(("Protocol %d not supported", ipCfg.protocol));
        errorFlag=1;
        goto error;
    }

    /* ----------------------------------------------------------------------------------------------------------------*/
    BDBG_MSG(("IP SessionOpen Complete"));

    if (ipCfg.playMp3) {
        psi.psiValid = true;
        psi.audioCodec = NEXUS_AudioCodec_eMp3;
        psi.audioPid = 0x1;
        psi.mpegType = NEXUS_TransportType_eEs;
    }
    else if (ipCfg.playLpcm) {
        psi.psiValid = true;
        psi.audioCodec = NEXUS_AudioCodec_ePcmWav;
        psi.audioPid = 0x1;
        psi.mpegType = NEXUS_TransportType_eWav;
    }
    /* ----------------------------------------------------------------------------------------------------------------*/
    else if (ipCfg.skipPsiParsing) {
        /* hardcode the codec info */
        /* current one is for Video Conferencing Demo case where camera output is being streamed to us via RTP */
        memset(&psi, 0, sizeof(psi));
        psi.psiValid = true;
        psi.videoCodec = NEXUS_VideoCodec_eMpeg4Part2;
        psi.videoCodec = NEXUS_VideoCodec_eH264;
        psi.videoPid = 0x1;
        psi.audioPid = 0;
        psi.pcrPid = 0x1;
        psi.mpegType = NEXUS_TransportType_eEs;
    }
    else if (psi.psiValid == false) {
        /* For non-HTTP protocols, apps will need to use psip library to do the psi discovery */
        memset(&collectionData, 0, sizeof(psiCollectionDataType));
        collectionData.playpump = playpump;
        collectionData.live = true;
        collectionData.playbackIp = playbackIp;
        /*collectionData.srcType = ipStreamerCfg->srcType;*/
        BDBG_MSG (("Acquiring PSI info..."));
        acquirePsiInfo(&collectionData,  psiList, &numPrograms);
        /* just choose the 1st program */
        psi = psiList[0];
    }

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* If IP Applib is not able to find the PSI info, then set them to defaults */
    if (psi.psiValid == false) setDefaultPsiSettings(&psi);
    
    BDBG_MSG (("Video Pid %d, Video Codec %d, Audio Pid %d, Audio Codec %d, Pcr Pid %d, Transport Type %d", 
                psi.videoPid, psi.videoCodec, psi.audioPid, psi.audioCodec, psi.pcrPid, psi.mpegType));

    if (psi.hlsSessionEnabled || psi.mpegDashSessionEnabled) {
        NEXUS_VideoWindow_GetScalerSettings(window, &scalerSettings);
        scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
        scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
        rc = NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);
        BDBG_ASSERT(!rc);
        BDBG_MSG (("HLS or MPEG-DASH Session: Updated Scaler settings to fix Scaler before Capture"));
    }

    /* ----------------------------------------------------------------------------------------------------------------*/

    if (ipCfg.fastChannelChange) {
        for (i=0;i<TOTAL_PRIMERS;i++) {
            primer[i] = NEXUS_VideoDecoderPrimer_Open(videoDecoder);
        }
    }

    B_PlaybackIp_GetSettings(playbackIp, &playbackIpSettings);
    ipCfg.useLiveIpMode = useLiveModeForIpPlayback(&psi, ipCfg.protocol, &playbackIpSettings.ipMode);
    playbackIpSettings.useNexusPlaypump = useNexusPlaypumpForIpPlayback(&psi, ipCfg.protocol);
    if (playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
            playbackIpSettings.ttsParams.autoDetect = false;
            playbackIpSettings.ttsParams.pacingMaxError = 2636;
            playbackIpSettings.ttsParams.throttleParams.initBufDepth = 625000;
            playbackIpSettings.ttsParams.throttleParams.maxBufDepth = 2250000;
            playbackIpSettings.ttsParams.throttleParams.minBufDepth = 125000;
            playbackIpSettings.ttsParams.throttleParams.maxClockMismatch = 60;
        }
    rc = B_PlaybackIp_SetSettings(playbackIp, &playbackIpSettings);
    if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}
    BDBG_WRN(("IP Clock Recovery Mode set to %s Mode (%d), using Nexus %s to feed stream over IP", 
                playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePull? "non-Live":"Live", playbackIpSettings.ipMode,
                playbackIpSettings.useNexusPlaypump? "Playpump" : "Playback"));

    /* do the initial configuration of Nexus Playpump/Playback modules: set modes & open pid channels */
    if (playbackIpSettings.useNexusPlaypump) {
        NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
        playpumpSettings.transportType = psi.mpegType;
        if (playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
            playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_e32_Binary;
            playpumpSettings.timestamp.timebase = NEXUS_Timebase_e1;
            playpumpSettings.timestamp.pacingMaxError = playbackIpSettings.ttsParams.pacingMaxError;
            playpumpSettings.timestamp.pacing = true;
            playpumpSettings.timestamp.pacingOffsetAdjustDisable = true;
            playpumpSettings.timestamp.parityCheckDisable = true;
            playpumpSettings.timestamp.resetPacing = true;
            BDBG_MSG (("$$$$$$$$$$$$$$$$$ updated TTS related params for playpump ...."));
        }
        rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); errorFlag=1; goto error;}

        /* Open the video pid channel */
        if (psi.videoPid) {
            NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
            pidChannelSettings.pidType = NEXUS_PidType_eVideo;
            videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.videoPid, &pidChannelSettings);
            if (!videoPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("%s: Opened video pid channel %u for video pid %u ", __FUNCTION__, videoPidChannel, psi.videoPid));
        }

        /* Open the extra video pid channel if present in the stream */
        if (psi.extraVideoCodec != NEXUS_VideoCodec_eNone && psi.extraVideoPid != 0) {
            NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
            pidChannelSettings.pidType = NEXUS_PidType_eVideo;
            extraVideoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.extraVideoPid, &pidChannelSettings);
            if (!extraVideoPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("%s: extra video pid %d, codec %d is present, pid channel created", __FUNCTION__, psi.extraVideoPid, psi.extraVideoCodec));
        }

        if (psi.audioPid) {
            /* Open the audio pid channel */
            NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
            pidChannelSettings.pidType = NEXUS_PidType_eAudio;
            pidChannelSettings.pidTypeSettings.audio.codec = psi.audioCodec;
            audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.audioPid, &pidChannelSettings);
            if (!audioPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("%s: Opened audio pid channel %u for audio pid %u ", __FUNCTION__, audioPidChannel, psi.audioPid));
        }

        if (psi.pcrPid != psi.audioPid && psi.pcrPid != psi.videoPid) {
            /* Open the pcr pid channel */
            NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
            pidChannelSettings.pidType = NEXUS_PidType_eUnknown;
            pcrPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, psi.pcrPid, &pidChannelSettings);
            if (!pcrPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("%s: Opened pcr pid channel %u for pcr pid %u ", __FUNCTION__, pcrPidChannel, psi.pcrPid));
        }
        else 
        {
            pcrPidChannel = videoPidChannel;
            BDBG_MSG (("%s: Using video pid channel %u for pcr pid channel ", __FUNCTION__, videoPidChannel));
        }
    }
    else {
        /* using Nexus Playback module */
        NEXUS_Playback_GetSettings(playback, &playbackSettings);
        playbackSettings.playpump = playpump;
        if (psi.videoPid) 
            playbackSettings.stcChannel = stcChannel;
        if (ipCfg.initialSeekTime || ipCfg.preChargeTime)
            playbackSettings.startPaused = true;
        if (!ipCfg.loop) {
            playbackSettings.endOfStreamCallback.callback = endOfStreamCallback;
            playbackSettings.endOfStreamCallback.context = NULL;
            playbackSettings.errorCallback.callback = errorCallback;
            playbackSettings.errorCallback.context = NULL;
            playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
        }
        else {
            playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
        }
        if (ipCfg.playMp3 || ipCfg.playLpcm || psi.audioCodec == NEXUS_AudioCodec_eMp3) {
            BDBG_MSG (("Enabling streamProcessing flag for audio seeking capability"));
            playbackSettings.enableStreamProcessing = true; /* needed for seek capability */
        }
        playbackSettings.playpumpSettings.transportType = psi.mpegType;
        playbackSettings.playpumpSettings.mode = NEXUS_PlaypumpMode_eFifo;
        if (psi.transportTimeStampEnabled) {
            playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
            playbackSettings.playpumpSettings.timestamp.pacing = false;
            BDBG_MSG (("Setting timestamp flag"));
        }
        rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        /* TODO: type setting following:
            playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
        */

        if (psi.videoPid) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
            playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
            playbackPidChannelSettings.pidTypeSettings.video.codec = psi.videoCodec;
            playbackPidChannelSettings.pidTypeSettings.video.decoder = videoDecoder;        /* Decode will set this later */
            playbackPidChannelSettings.pidTypeSettings.video.index = true;
            videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, psi.videoPid, &playbackPidChannelSettings);
            if (!videoPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        }

        /* Open the extra video pid channel if present in the stream */
        if (psi.extraVideoCodec != NEXUS_VideoCodec_eNone && psi.extraVideoPid != 0) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
            playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
            playbackPidChannelSettings.pidTypeSettings.video.codec = psi.extraVideoCodec;
            playbackPidChannelSettings.pidTypeSettings.video.decoder = videoDecoder;        /* Decode will set this later */
            playbackPidChannelSettings.pidTypeSettings.video.index = true;
            extraVideoPidChannel = NEXUS_Playback_OpenPidChannel(playback, psi.extraVideoPid, &playbackPidChannelSettings);
            if (!extraVideoPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("%s: extra video pid %d, codec %d is present, pid channel created", __FUNCTION__, psi.extraVideoPid, psi.extraVideoCodec));
        }

        if (psi.audioPid) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidChannelSettings);
            playbackPidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
            playbackPidChannelSettings.pidSettings.pidTypeSettings.audio.codec = psi.audioCodec;
            playbackPidChannelSettings.pidTypeSettings.audio.primary = pcmDecoder;
            audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, psi.audioPid, &playbackPidChannelSettings);
            if (!audioPidChannel) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG(("Playback Audio Pid channel is opened\n"));
        }
    }
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Stc Channel configuration */
    NEXUS_StcChannel_GetSettings(stcChannel, &stcChannelSettings);
    switch (playbackIpSettings.ipMode) {
    case B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip:
        /* Following Nexus configuration is done for absorbing high IP network jitter 
         -DPCR & PCR Offset blocks are programmed w/ increased thresholds for errors 
         -Both display & audio outputs are decoupled from input time base & run from a free clock
         -AV CDBs sizes are increased to absorb the network jitter
         -AV decodes are delayed by the amount of jitter buffer depth
        */
        if (psi.pcrPid) {
            /* program the timebase 0: increase its track range & max pcr errors */
            NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
            timebaseSettings.freeze = false;
            timebaseSettings.sourceSettings.pcr.pidChannel = pcrPidChannel;
            timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 183/2;    /* in milliseconds: based on 90Khz clock */
            timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
            rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("Configured Timebase %d with jitter %d", NEXUS_Timebase_e0, IP_NETWORK_JITTER));

            /* Update STC Channel Settings to accomodate Network Jitter */
            /* configure the StcChannel to do lipsync with the PCR */
            stcChannelSettings.timebase = NEXUS_Timebase_e0;     /* timebase */
            stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr; /* Live Mode */
            /* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
            stcChannelSettings.modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183; 
            /* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
            stcChannelSettings.modeSettings.pcr.maxPcrError =  IP_NETWORK_JITTER * 183;
            stcChannelSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
            /*  PCR Offset "Jitter Adjustment" is not suitable for use with IP channels Channels, so disable it */
            stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = true;
            /* Disable Auto Timestamp correction for PCR Jitter */
            stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = true;
            /* We just configured the Timebase, so turn off auto timebase config */
            stcChannelSettings.autoConfigTimebase = false;
            BDBG_MSG (("Configured stc channel: timebase %d, jitter %d", NEXUS_Timebase_e0, IP_NETWORK_JITTER));
        }

        /* Setup timebase 1 to free run */
        NEXUS_Timebase_GetSettings(NEXUS_Timebase_e1, &timebaseSettings);
        timebaseSettings.freeze = true;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e1, &timebaseSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Configured Timebase %d to freerun \n", NEXUS_Timebase_e1));
        break;

    case B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip:
    case B_PlaybackIpClockRecoveryMode_ePushWithPcrNoSyncSlip:
        if (psi.pcrPid) {
            /* program the timebase 0: increase its track range & max pcr errors */
            NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
            timebaseSettings.freeze = false;
            timebaseSettings.sourceSettings.pcr.pidChannel = pcrPidChannel;
            timebaseSettings.sourceSettings.pcr.maxPcrError = 255;
            timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG(("Configured Timebase %d with increased tracking range & maxPcrError \n", NEXUS_Timebase_e0));

        }
        else {
            /* program the timebase 0: increase its track range & max pcr errors */
            NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            timebaseSettings.freeze = true;
            timebaseSettings.sourceSettings.freeRun.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            timebaseSettings.sourceSettings.freeRun.centerFrequency = 0x400000;
            rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG(("Configured Timebase %d with increased tracking range & maxPcrError \n", NEXUS_Timebase_e0));
        }
        /* program the timebase 1: for monitoring the playpump buffer */
        NEXUS_Timebase_GetSettings(NEXUS_Timebase_e1, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        timebaseSettings.freeze = true;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e122ppm;
        rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e1, &timebaseSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Configured Timebase %d with increased tracking range & maxPcrError \n", NEXUS_Timebase_e1));

        /* Update STC Channel Settings */
        /* configure the StcChannel to do lipsync with the PCR */
        stcChannelSettings.timebase = NEXUS_Timebase_e0;     /* timebase */
        stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr; /* Live Mode */
        /* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcChannelSettings.modeSettings.pcr.offsetThreshold = 8;
        /* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcChannelSettings.modeSettings.pcr.maxPcrError = 255;
        stcChannelSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
        /*  PCR Offset "Jitter Adjustment" is not suitable for use with IP channels Channels, so disable it */
        stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = true;
        /* Disable Auto Timestamp correction for PCR Jitter */
        stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = true;
        /* We just configured the Timebase, so turn off auto timebase config */
        stcChannelSettings.autoConfigTimebase = false;
        break;

    case B_PlaybackIpClockRecoveryMode_ePull:
        /* PVR/Pull mode */ 
        if (psi.videoPid) {
            stcChannelSettings.timebase = NEXUS_Timebase_e0;     /* timebase */
            stcChannelSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR (Pull) Mode */
            stcChannelSettings.modeSettings.Auto.transportType = psi.mpegType;
        }
        break;
    }
#if 0
    /* check if helpful */
    stcChannelSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
    stcChannelSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
#endif
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcChannelSettings);
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    /* ----------------------------------------------------------------------------------------------------------------*/


    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Setup up video decoder */
    BDBG_MSG(("Video PID %d, Video Codec %d, Audio PID %d, Audio Codec %d, PCR PID %d, transport type %d, duration %d\n",
            psi.videoPid, psi.videoCodec, psi.audioPid, psi.audioCodec, psi.pcrPid, psi.mpegType, psi.duration));
    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    if (psi.videoPid) {
        videoProgram.codec = psi.videoCodec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;
        if (extraVideoPidChannel) {
            videoProgram.enhancementPidChannel = extraVideoPidChannel;
            videoProgram.codec = psi.extraVideoCodec;
            BDBG_MSG (("%s: extra video pid channel programmed", __FUNCTION__, extraVideoPidChannel));
        }
    }

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = psi.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    if (psi.audioPid) {
        audioProgram.stcChannel = stcChannel;
    }

#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        /* Only pass through AC3 */
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0] ),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
#endif
    /* configure output rate managers */
    switch (playbackIpSettings.ipMode) {
    case B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip:
        /* Both display & audio outputs are decoupled from input time base & run from a free clock */
        #if NEXUS_NUM_AUDIO_DACS
        output = NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]);
        NEXUS_AudioOutput_GetSettings(output, &audioOutputSettings);
        audioOutputSettings.timebase = NEXUS_Timebase_e1;
        rc = NEXUS_AudioOutput_SetSettings(output, &audioOutputSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        #endif
        #if NEXUS_NUM_SPDIF_OUTPUTS
        output = NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]);
        NEXUS_AudioOutput_GetSettings(output, &audioOutputSettings);
        audioOutputSettings.timebase = NEXUS_Timebase_e1;
        rc = NEXUS_AudioOutput_SetSettings(output, &audioOutputSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        #endif
#if NEXUS_NUM_HDMI_OUTPUTS
        output = NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0] );
        NEXUS_AudioOutput_GetSettings(output, &audioOutputSettings);
        audioOutputSettings.timebase = NEXUS_Timebase_e1;
        rc = NEXUS_AudioOutput_SetSettings(output, &audioOutputSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
#endif
        /* TODO: Modify timebase of other audio outputs as well (e.g. i2s, etc) */

        /* Decouple the Display from input timebase and run it on free running timebase */
        NEXUS_Display_GetSettings(display, &displaySettings);
        displaySettings.timebase = NEXUS_Timebase_e1;
        rc = NEXUS_Display_SetSettings(display, &displaySettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        break;
    case B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip:
    case B_PlaybackIpClockRecoveryMode_ePushWithPcrNoSyncSlip:
    case B_PlaybackIpClockRecoveryMode_ePull:
        break;
    }
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* ----------------------------------------------------------------------------------------------------------------*/
    /* Two additional AV decoder seetings are needed for Live IP Mode */
    /*  1. Delay the start of decoder as per the network jitter by increasing the PTS Offset */
    /*  2. Tell the decoder that it is in the live mode */
    if (psi.videoPid) {
	    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
        if (playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip) {
            videoDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
        }

        videoDecoderSettings.sourceChanged.callback = sourceChangeCallback;
        videoDecoderSettings.sourceChanged.context = videoDecoder;
        videoDecoderSettings.sourceChanged.param = 1;
        rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Video Decoder settings are modified for IP \n"));
        if (!psi.audioPid) {
            NEXUS_VideoDecoderExtendedSettings pSettings;
            NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &pSettings);
            /* pSettings.zeroDelayOutputMode = true;   Commenting out because it causes jerkiness during playback of normal streams. */
            pSettings.ignoreDpbOutputDelaySyntax = true;
            rc = NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &pSettings);
            if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            BDBG_MSG (("Video Decoder extended settings are modified Ip Mode"));
        }
    }

    if (psi.audioPid) {
        NEXUS_AudioDecoder_GetSettings(pcmDecoder, &audioDecoderSettings);		
        if (playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip) {
            audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
        }

        audioDecoderSettings.sourceChanged.callback = sourceChangeCallback;
        audioDecoderSettings.sourceChanged.context = pcmDecoder;
        audioDecoderSettings.sourceChanged.param = 0;
        rc = NEXUS_AudioDecoder_SetSettings(pcmDecoder, &audioDecoderSettings);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("PCM Audio Decoder settings are modified for IP \n"));

        NEXUS_AudioDecoder_GetSettings(compressedDecoder, &audioDecoderSettings);
        audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
        rc = NEXUS_AudioDecoder_SetSettings(compressedDecoder, &audioDecoderSettings);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Compressed Audio Decoder settings are modified for IP \n"));
    }
    /* ----------------------------------------------------------------------------------------------------------------*/

#if NEXUS_HAS_SYNC_CHANNEL
    if (!ipCfg.fastChannelChange) {
        /* connect sync channel */
        NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
        syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
        if (psi.audioPid) {
            syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
#if 0
            syncChannelSettings.audioInput[1] = NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed);
#endif
        }
        NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);
    }
#endif

#if 0
    /* Needed to comment out starting the compressed decoder, otherwise client based trickmodes for TS content were not working */
    /* as this decoder was not getting flushed during trick play transition. Need to look into this at some point */
    if (psi.audioPid && audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        rc = NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__); exit(1);}
    }
#endif

    if (ipCfg.fastChannelChange) {
        NEXUS_VideoDecoderPrimerSettings primerSettings;
        for (i=0;i<TOTAL_PRIMERS;i++) {
            NEXUS_VideoDecoderPrimer_GetSettings(primer[i], &primerSettings);
            primerSettings.pastTolerance = 1500; /* amount of time willing to race: for IP, start decode from the previous GOP */
            primerSettings.futureTolerance = 0;
            rc = NEXUS_VideoDecoderPrimer_SetSettings(primer[i], &primerSettings);

            /* now start the primer */
            NEXUS_VideoDecoderPrimer_Start(primer[i], &videoProgram);
        }
    }

    /* Start Playing Media */
    if (!playbackIpSettings.useNexusPlaypump) {
        BDBG_MSG(("Start Nexus Playback \n"));
        NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
        playbackStartSettings.mpeg2TsIndexType = NEXUS_PlaybackMpeg2TsIndexType_eSelf;
        rc = NEXUS_Playback_Start(playback, ipSessionSetupStatus.u.http.file, &playbackStartSettings);
        if (rc) {
            BDBG_ERR(("NEXUS Error (%d) at %d, retrying w/o index..\n", rc, __LINE__));
            rc = B_PlaybackIp_SessionStop(playbackIp);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            rc = B_PlaybackIp_SessionClose(playbackIp);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}

            BDBG_WRN(("Starting IP Applib: w/o index this time..."));
            ipSessionOpenSettings.nonBlockingMode = false;
            rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
            ipSessionSetupSettings.u.http.dontUseIndex = true;
            rc = B_PlaybackIp_SessionSetup(playbackIp, &ipSessionSetupSettings, &ipSessionSetupStatus);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}

            rc = NEXUS_Playback_Start(playback, ipSessionSetupStatus.u.http.file, NULL);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        }
        BDBG_MSG(("Nexus Playback is started\n"));
    }
    else {
        /* playpump is used for all Live modes including Http Live Streaming */
        rc = NEXUS_Playpump_Start(playpump);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Nexus Playpump is started\n"));
    }
    audioStarted = true;
    /* ----------------------------------------------------------------------------------------------------------------*/

    /* let IP Applib go ... */
    memset(&ipSessionStartSettings, 0, sizeof(ipSessionStartSettings));
    if (ipCfg.protocol == B_PlaybackIpProtocol_eHttp) {
        ipSessionStartSettings.u.http.preChargeBuffer = false;    /* precharging done via the preChargeNetworkBuffer() call below */
    }
    else if (ipCfg.protocol == B_PlaybackIpProtocol_eRtsp) {
        ipSessionStartSettings.u.rtsp.mediaTransportProtocol = B_PlaybackIpProtocol_eRtp;  /* protocol used to carry actual media */
        ipSessionStartSettings.u.rtsp.keepAliveInterval = 10;  /* send rtsp heart beats (keepalive) every 10sec */
    }
    /* set Nexus handles */
    if (playpump)
        ipSessionStartSettings.nexusHandles.playpump = playpump;
    if (playback)
        ipSessionStartSettings.nexusHandles.playback = playback;
    if (psi.videoPid) {
        ipSessionStartSettings.nexusHandles.videoDecoder = videoDecoder;
    }
    if (stcChannel) 
        ipSessionStartSettings.nexusHandles.stcChannel = stcChannel;
    if (psi.audioPid && pcmDecoder) {
        ipSessionStartSettings.nexusHandles.primaryAudioDecoder = pcmDecoder;
    }
#if 0
    if (compressedDecoder) 
        ipSessionStartSettings.nexusHandles.secondaryAudioDecoder = compressedDecoder;
#endif
    ipSessionStartSettings.nexusHandlesValid = true;
    ipSessionStartSettings.mpegType = psi.mpegType;
    rc = B_PlaybackIp_SessionStart(playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);
    while (rc == B_ERROR_IN_PROGRESS) {
        /* app can do some useful work while SessionSetup is in progress and resume when callback sends the completion event */
        BDBG_MSG (("Session Start call in progress, sleeping for now..."));
        BKNI_Sleep(100);
        rc = B_PlaybackIp_SessionStart(playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);
    }
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); errorFlag=1; goto error;}

    if (ipCfg.fastChannelChange && psi.videoPid) {
        BDBG_WRN(("Press ENTER to switch programs\n"));
        getchar();
        BDBG_WRN(("Starting Decoder w/ Primer ...\n"));
        rc = NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(primer[0],videoDecoder);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); errorFlag=1;  goto errorClose;}
        BDBG_MSG(("Video Decoder Primer is Started\n"));
    }
    else {
        /* Start Decoders */
        if (psi.videoPid) {
            rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
            if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); errorFlag=1;  goto errorClose;}
            BDBG_MSG(("Video Decoder is Started\n"));
        }
    }

    if (psi.audioPid) {
        rc = NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_MSG(("Audio Decoder is Started\n"));
    }

    /* TODO: wait for PreChangeEvent */
    /* Now pre-charge n/w buffer if configured */
    if (ipCfg.preChargeTime && !ipCfg.initialSeekTime) {
        /* do initial pre-charging only if we are not doing any initial seeks */
        if (preChargeNetworkBuffer(playbackIp, ipCfg.preChargeTime)) {
            BDBG_ERR((" #### Initial pre-charge of Network buffer of %d sec failed\n", ipCfg.preChargeTime));
            exit(1);
        }
        BDBG_MSG ((" #### Initial Network pre-charge of %d sec is complete", ipCfg.preChargeTime));
        rc = NEXUS_Playback_Play(playback);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    }

    /* ----------------------------------------------------------------------------------------------------------------*/


    BDBG_MSG (("Nexus & Platform Setup complete"));
    if (ipCfg.initialSeekTime) {
        /* user wants to skip to specified time in seconds */
        if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
            BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
            exit(1);
        }
#if 0
        BDBG_WRN(("Seeking Playback by %d sec", ipCfg.initialSeekTime));
        rc = NEXUS_Playback_Seek(playback, ipCfg.initialSeekTime * 1000);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        BDBG_WRN(("Seeked Playback by %d sec\n", ipCfg.initialSeekTime));

        if (preChargeNetworkBuffer(playbackIp, ipCfg.preChargeTime)) {
            BDBG_ERR((" #### Initial pre-charge of Network buffer of %d sec failed\n", ipCfg.preChargeTime));
            exit(1);
        }
        BDBG_WRN((" #### initial Seek: Network pre-charge of %d sec is complete", ipCfg.preChargeTime));

        rc = NEXUS_Playback_Play(playback);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
#else
        B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings);
        ipTrickModeSettings.seekPositionIsRelative = false;
        ipTrickModeSettings.seekBackward = false;
        ipTrickModeSettings.seekPosition = ipCfg.initialSeekTime*1000; /* in msec */
        if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) { 
            BDBG_ERR(("ERROR: Failed to seek Ip playback\n"));
            errorFlag=1;
            goto error;
        }
#endif
        BDBG_MSG (("Initial Seek successful, playback is started at correct time offset"));
    }

    /* Set stdin to non-blocking */
    if (fcntl(0, F_SETFL, fcntl(0, F_GETFL) |O_NONBLOCK) < 0)
        BDBG_WRN(("ERROR setting non-blocking mode on STDIN\n"));
    
    if (psi.videoPid & !psi.audioPid) {
        NEXUS_VideoDecoderTrickState videoDecoderTrickSettings;
        NEXUS_VideoDecoder_GetTrickState(videoDecoder, &videoDecoderTrickSettings);
        videoDecoderTrickSettings.tsmEnabled = false;
        if (NEXUS_VideoDecoder_SetTrickState(videoDecoder, &videoDecoderTrickSettings) != NEXUS_SUCCESS) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1); }
        BDBG_MSG (("Disabling TSM...."));
    }
    B_Time_Get(&prevTime);
    while (!gEof) {
        static int state = 1;
        int curBufferDuration; 
        static unsigned prevPts = 0;
        static int rate = 1;
        static unsigned playSpeedIndex;
        static bool usingServerBasedTrickModes = false;
        /* Sleep for a second before we check any status */
        gEof = false;
        BKNI_Sleep(100);

        /* Print various status while decoding */

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        rc = NEXUS_Playback_GetStatus(playback, &pbStatus);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        rc = NEXUS_Playpump_GetStatus(playpump, &ppStatus);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
        rc = B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}
        if (psi.videoPid & psi.audioPid) 
            NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);


        if (ipCfg.decoderStats) {
            BDBG_WRN(("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo size %d, depth %d, fullness %d%%\n",
            videoStatus.source.width, videoStatus.source.height, videoStatus.pts, stc, videoStatus.ptsStcDifference, videoStatus.fifoSize, videoStatus.fifoDepth, videoStatus.fifoSize?(videoStatus.fifoDepth*100)/videoStatus.fifoSize:0));
            BDBG_WRN(("audio0            pts %#x, stc %#x (diff %d) fifo size %d, depth %d, fullness %d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize, audioStatus.fifoDepth, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0));
#if 0
            NEXUS_AudioDecoder_GetStatus(compressedDecoder, &audioStatus);
#endif
            if ( audioStatus.started ) {
                BDBG_WRN(("audio1            pts %#x, stc %#x (diff %d) fifo size %d, depth %d, fullness %d%%\n",
                audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize, audioStatus.fifoDepth, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0));
            }
            BDBG_WRN(("q depth %d, decoded %d, displayed %d, decode errs %d, display errs %d, decode drops %d, display drops %d, display underflow %d, received %d, pts errs %d\n",
                    videoStatus.queueDepth, videoStatus.numDecoded, videoStatus.numDisplayed, videoStatus.numDecodeErrors,
                    videoStatus.numDisplayErrors, videoStatus.numDecodeDrops, videoStatus.numDisplayDrops, videoStatus.numDisplayUnderflows, videoStatus.numPicturesReceived, videoStatus.ptsErrorCount));

            BDBG_WRN(("playback: ip pos %lu, pb pos %lu, fed %lu, first %lu, last %lu, PB buffer depth %d, size %d, fullness %d%%, played bytes %lld\n", 
                    playbackIpStatus.position, pbStatus.position, pbStatus.readPosition, pbStatus.first, pbStatus.last, ppStatus.fifoDepth, ppStatus.fifoSize, 
                    (ppStatus.fifoDepth*100)/ppStatus.fifoSize, ppStatus.bytesPlayed));
        }
        B_Time_Get(&curTime);
        /*Limited time decode for liveChannel if its specified using -T option*/
        if ((ipCfg.decodeTimeDuration > 0) && (unsigned)(B_Time_Diff(&curTime,&prevTime)/1000) > ipCfg.decodeTimeDuration) {
            BDBG_WRN(("*********** stopping ip playback session, total playback time (%d) defined by user exceeded ***********", ipCfg.decodeTimeDuration));
            break;
        }

        if (playbackIpStatus.ipState == B_PlaybackIpState_eTrickMode && gEof) {
            if (B_PlaybackIp_Play(playbackIp)) {
                BDBG_ERR(("ERROR: Failed to pause Ip playback\n"));
                errorFlag=1;
                goto error;
            }
            state = 1;
            rate = 1;
        }
        else if (playbackIpStatus.ipState == B_PlaybackIpState_ePaused || playbackIpStatus.ipState == B_PlaybackIpState_eTrickMode)
            goto skip_runtime_buffering_check;
#define RUNTIME_BUFFERING_CODE
#ifdef RUNTIME_BUFFERING_CODE
        if (psi.mpegType == NEXUS_TransportType_eMp4) {
            curBufferDuration = pbStatus.readPosition - pbStatus.position;
            BDBG_MSG(("buffered %d mill-seconds worth of MP4 content\n", curBufferDuration));
        }
        else if (psi.mpegType == NEXUS_TransportType_eAsf) {
            curBufferDuration = (ppStatus.mediaPts - (psi.videoPid?videoStatus.pts:audioStatus.pts))/45;
            BDBG_MSG(("buffered %d milli-seconds worth of ASF content\n", curBufferDuration));
        }
        else {
            /* we need to use alternate means to determine the amount of buffering in system since such formats are not processed in sw and thus we dont know the curBufferDepth */
            /* instead, we can detect the underflow condition by monitoring the last pts displayed, once it doesn't change, we are definitely underflowing and thus can precharge */
            /* by default, set curBufferDuration to a higher number to avoid precharging */
            curBufferDuration = 99999; /* set to some large value */
            if (prevPts) {
                if (prevPts == (psi.videoPid?videoStatus.pts:audioStatus.pts)) {
                    /* pts hasn't changed, so we are underflowing, set flag to precharge */
                    BDBG_MSG(("pts hasn't changed, so we are underflowing, prev pts, %u, cur pts %u, pre charge time %d\n", prevPts, psi.videoPid?videoStatus.pts:audioStatus.pts, ipCfg.preChargeTime));
                    curBufferDuration = 0;
                }
            }
            prevPts = psi.videoPid?videoStatus.pts:audioStatus.pts;
            BDBG_MSG(("prev pts, %u, cur pts %u\n", prevPts, psi.videoPid?videoStatus.pts:audioStatus.pts));
        }
        if (ipCfg.preChargeTime && (curBufferDuration < 200)) {
            /* we are precharging & current buffer level is below the low water mark, so start pre-charging */
            /* however, sleep quickly to see if underflow is due to EOF. Otherwise, we will Pause Playback too quickly */
            /* before we get the EOF callback. Sleep gives Nexus Playback a chance to invoke the eof callback. */
            BKNI_Sleep(500);
            if (gEof) {
                BDBG_WRN(("Underflow is due to EOF, breaking out...\n"));
                break;
            }
            BDBG_WRN(("Underflowing, so pausing the playback until enough buffering is done..."));
            if (NEXUS_Playback_Pause(playback)) {
                BDBG_WRN(("ERROR: Failed to pause Nexus playback\n"));
                break;
            }
            BDBG_MSG(("Paused Nexus Playback...\n"));

            /* Now pre-charge n/w buffer */
            if (preChargeNetworkBuffer(playbackIp, ipCfg.preChargeTime)) {
                BDBG_ERR((" #### runtime pre-charge of Network buffer of %d sec failed\n", ipCfg.preChargeTime));
                break;
            }
            BDBG_MSG(("Resuming Playback\n"));
            if (NEXUS_Playback_Play(playback)) {
                BDBG_WRN(("ERROR: Failed to play Nexus playback from pause\n"));
                break;
            }
        }
skip_runtime_buffering_check:
#endif

        /* Exit loop if character is entered */
        memset(buf, 0, sizeof(buf));
        numbytes = read(0,buf,sizeof(buf));
        BDBG_MSG(("User interrupted loop. STDIN received data, numbytes %d, errno %d\n", numbytes, errno));
        if (numbytes > 0) {
            if (buf[0] == 'p' && state == 1) {
                state = 0;
                if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
                    exit(1);
                }
                ipTrickModeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseConnectionStalling;
                ipTrickModeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
                BDBG_WRN(("Pausing Playback using method %d", ipTrickModeSettings.pauseMethod));
                if (B_PlaybackIp_Pause(playbackIp, &ipTrickModeSettings)) {
                    BDBG_ERR(("ERROR: Failed to pause Ip playback\n"));
                    errorFlag=1;
                    goto error;
                }
            }
            else if (buf[0] == 'p' && state == 0) {
                BDBG_WRN(("Resuming Playback"));
                state = 1;
                rate = 1;
                if (B_PlaybackIp_Play(playbackIp)) {
                    BDBG_ERR(("ERROR: Failed to pause Ip playback\n"));
                    errorFlag=1;
                    goto error;
                }
                BDBG_WRN(("After Resuming Playback"));
            }
            else if (strncmp(buf, "fs", 2) == 0 || strncmp(buf, "rs", 2) == 0 ) {
                int dir = (strncmp(buf, "fs", 2) == 0) ? 1 : -1;
                int rate;
                buf[5] = buf[6] = '\0';
                rate = atoi(&buf[2]);
                if (rate == 0 || rate > NEXUS_NORMAL_PLAY_SPEED)
                    rate = NEXUS_NORMAL_PLAY_SPEED;
                state = 0;  /* goto to play when when p is entered */
                BDBG_WRN(("set the trick play slow %s command", dir == 1?"fwd": "rwd"));
                if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
                    exit(1);
                }
                ipTrickModeSettings.absoluteRateDefined = true;
                if (dir > 0)
                    ipTrickModeSettings.absoluteRate = rate;
                else
                    ipTrickModeSettings.absoluteRate = -rate;
                if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_WRN(("Failed to set the trick play speed %d, index %d\n", psi.playSpeed[playSpeedIndex], playSpeedIndex));
                    break;
                }
                BDBG_WRN(("Successfully set the trick play slow %s command", dir == 1?"fwd": "rwd"));
            }
            else if (buf[0] == 's') {
                int seekTime = 0;
                buf[6] = '\0';
                if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
                    exit(1);
                }
                if (buf[1] == 'r') {
                    /* seek relative */
                    seekTime = atoi(&buf[2]);
                    ipTrickModeSettings.seekPositionIsRelative = true;
                }
                else {
                    seekTime = atoi(&buf[1]);
                    ipTrickModeSettings.seekPositionIsRelative = false;
                }

                if (seekTime < 0) {
                    ipTrickModeSettings.seekPositionIsRelative = true;
                    ipTrickModeSettings.seekBackward = true;
                    seekTime = ~seekTime + 1;
                }
                else
                    ipTrickModeSettings.seekBackward = false;
                /* user wants to skip to specified time in seconds */
                ipTrickModeSettings.seekPosition = seekTime*1000; /* in msec */
                BDBG_WRN(("IP Playback seeking to %d sec ", seekTime));
                if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) { 
                    BDBG_ERR(("ERROR: Failed to seek Ip playback\n"));
                    errorFlag=1;
                    goto error;
                }
                state = 1;
                BDBG_WRN(("IP Playback is started at %d time pos %u", seekTime, ipTrickModeSettings.seekPosition));
            }
            else if (buf[0] == 'j') {
                /* jump forward by a fixed time (defaults to 5 sec) */
                int seekTime = 0;
                if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
                    exit(1);
                }
                /* seek relative */
                seekTime = ipCfg.secondsToJump;
                ipTrickModeSettings.seekPositionIsRelative = true;
                if (seekTime < 0) {
                    ipTrickModeSettings.seekBackward = true;
                    seekTime = ~seekTime + 1;
                }
                else
                    ipTrickModeSettings.seekBackward = false;
                /* user wants to skip to specified time in seconds */
                ipTrickModeSettings.seekPosition = seekTime*1000; /* in msec */
                BDBG_WRN(("IP Playback jumping to %d sec ", seekTime));
                if (B_PlaybackIp_Seek(playbackIp, &ipTrickModeSettings)) { 
                    BDBG_ERR(("ERROR: Failed to seek Ip playback\n"));
                    errorFlag=1;
                    goto error;
                }
                state = 1;
                BDBG_WRN(("IP Playback is started at %d time pos %u", seekTime, ipTrickModeSettings.seekPosition));
            }
            else if (strncmp(buf, "ff", 2) == 0 || strncmp(buf, "fr", 2) == 0 ) {
                int dir = (strncmp(buf, "ff", 2) == 0) ? 1 : -1;
                state = 0;  /* goto to play when when p is entered */
                if (B_PlaybackIp_GetTrickModeSettings(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); 
                    exit(1);
                }
                if (psi.numPlaySpeedEntries <= 1) {
                    /* client side trickmodes */
                    if (dir > 0)
                        rate++;
                    else {
                        if (rate == 1)
                            rate = -1;
                        else
                            rate--;
                    }
                    ipTrickModeSettings.method = B_PlaybackIpTrickModesMethod_UseByteRange;
                    ipTrickModeSettings.rate = rate;
#ifdef USE_ABSOLUTE_RATE
                    if (dir > 0) {
                        ipTrickModeSettings.absoluteRateDefined = true;
                        ipTrickModeSettings.absoluteRate = NEXUS_NORMAL_PLAY_SPEED/2;
                    } 
                    else {
                        ipTrickModeSettings.absoluteRateDefined = true;
                        ipTrickModeSettings.absoluteRate = -NEXUS_NORMAL_PLAY_SPEED/2;
                    } 
#endif
                    ipTrickModeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
                    if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                            BDBG_WRN(("Failed to set the trick play rate %d for client side trickmodes\n", rate));
                            break;
                    }
                    BDBG_WRN(("Successfuly set the trick play rate %d for client side trickmodes", rate));
                    usingServerBasedTrickModes = false;
                }
                else {
                    /* server side trickmodes */
                    BDBG_WRN(("trick play rate %d, index %d, totol entries %d", rate, playSpeedIndex, psi.numPlaySpeedEntries));
                    unsigned i;
                    /* server has provided various supported speeds during trick mode */
                    ipTrickModeSettings.method = B_PlaybackIpTrickModesMethod_UsePlaySpeed;
                    ipTrickModeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
                    /* now pick the correct speed from the playSpeed Arrary */
                    if (rate == 1 && dir> 0) {
                        /* we are now switching from normal to FFWD trick play, find the 1st positive speed */
                        for (i=0; i< psi.numPlaySpeedEntries; i++) {
                            if (psi.playSpeed[i] > 0) {
                                /* 1st positive speed */
                                playSpeedIndex = i;
                                break;
                            }
                        }
                    }
                    else if (rate == 1 && dir < 0) {
                        /* we are now switching from normal to FREW trick play, find the 1st -ve speed */
                        for (i=0; i< psi.numPlaySpeedEntries; i++) {
                            if (psi.playSpeed[i] > 0) {
                                /* 1st positive speed */
                                playSpeedIndex = i-1;
                                break;
                            }
                        }
                    }
                    else {
                        /* we are already in the fwd trick play state, update the index */
                        if (dir>0)
                            playSpeedIndex++;
                        else
                            playSpeedIndex--;
                    }
                    if (playSpeedIndex >= psi.numPlaySpeedEntries) {
                        playSpeedIndex = psi.numPlaySpeedEntries - 1;
                    }
                    ipTrickModeSettings.rate = psi.playSpeed[playSpeedIndex];
                    if (ipTrickModeSettings.rate >= psi.httpMinIFrameSpeed || dir < 0)
                        ipTrickModeSettings.frameRepeat = psi.httpFrameRepeat;
                    else
                        ipTrickModeSettings.frameRepeat = 1;
                    if (B_PlaybackIp_TrickMode(playbackIp, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
                        BDBG_WRN(("Failed to set the trick play speed %d, index %d\n", psi.playSpeed[playSpeedIndex], playSpeedIndex));
                        break;
                    }
                    BDBG_WRN(("Successfully set the trick play speed %d, index %d, slow motion %d", psi.playSpeed[playSpeedIndex], playSpeedIndex, ipTrickModeSettings.frameRepeat));
                    usingServerBasedTrickModes = true;
                    rate++;
                }
            }
            else if (strncmp(buf, "q", 1) == 0) {
                BDBG_WRN(("breaking from loop as user entered %s", buf));
                break;
            }
            else if (strncmp(buf, "h", 1) == 0 || strncmp(buf, "?", 1) == 0) {
                BDBG_WRN(("runtime options are--->\n"));
                printf("\ts[-+]<num>: seek to this absolute position in sec. e.g. s30 to seek to 30sec in movie\n");
                printf("\tsr[-+]<num>: seek relative in backward or forward direction by sec. e.g. sr-30 to seek to 30sec backwards from current point\n");
                printf("\tj<: jump forward or backward by fixed time (defaults to 5sec, modified via -j startup option.\n");
                printf("\tp: toggle between pause/play playback\n");
                printf("\tff: fast forward to next +ve speed \n");
                printf("\tfr: fast rewind to previous -ve speed\n");
            }
            else {
                BDBG_WRN(("Continuing loop: user entered %s", buf));
            }
        } /* else assume EAGAIN */

        if (ipCfg.runUnitTests) {
            if (runPlaybackIpUnitTests(&psi, playbackIp) < 0) {
                BDBG_ERR(("Auto tests failed"));
                errorFlag = 1;
            }
            else 
                BDBG_ERR(("Auto tests worked"));
            if (!ipCfg.loop) {
                BDBG_WRN(("Breaking main loop"));
                break;
            }
        }
    }

error:
    if (!ipCfg.runUnitTests && !errorFlag && psi.duration && ipCfg.protocol == B_PlaybackIpProtocol_eHttp) {
        rc = B_PlaybackIp_GetStatus(playbackIp, &playbackIpStatus);
        if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return (-1);}
        if ((!ipCfg.decodeTimeDuration && playbackIpStatus.position/1000 < (psi.duration/1000) -1) || (ipCfg.decodeTimeDuration > psi.duration/1000 && (unsigned)B_Time_Diff(&curTime,&prevTime)/1000 < psi.duration/1000)) {
            BDBG_WRN(("Didn't play the whole file: duration %d, last position %u", psi.duration, playbackIpStatus.position));
            errorFlag = 1;
        }
        else
            BDBG_WRN(("Played the whole file: duration %d, last position %u, errorFlag %d", psi.duration, playbackIpStatus.position, errorFlag));
    }
    if (playbackIp)
        B_PlaybackIp_SessionStop(playbackIp);
#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS	
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]));
#endif
    
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    if (audioPidChannel) NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_VideoDecoder_Stop(videoDecoder);

    /* Do cleanup necessary for changing channels */
    if (!playbackIpSettings.useNexusPlaypump) {
        BDBG_MSG (("Stopping Nexus Playback module"));
        if(playback)
        {
            NEXUS_Playback_CloseAllPidChannels(playback);
            NEXUS_Playback_Stop(playback);
        }
    }
    else {
        BDBG_MSG (("Stopping Nexus Playpump module"));
        NEXUS_Playpump_Stop(playpump);
        /* NEXUS_Playpump_CloseAllPidChannels(playpump);*/
        NEXUS_Playpump_ClosePidChannel(playpump, videoPidChannel);
        if (audioPidChannel) NEXUS_Playpump_ClosePidChannel(playpump, audioPidChannel);
        if (pcrPidChannel != videoPidChannel)
            NEXUS_Playpump_ClosePidChannel(playpump, pcrPidChannel);
    }

errorClose:
#if NEXUS_HAS_SYNC_CHANNEL
    if (!ipCfg.fastChannelChange) {
        /* disconnect sync channel */
        NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
        syncChannelSettings.videoInput = NULL;
        syncChannelSettings.audioInput[0] = NULL;
        syncChannelSettings.audioInput[1] = NULL;
        NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);
    }
#endif

    /* Close Socket related resources */
    if (playbackIp != NULL) {
        /* coverity[freed_arg=FALSE] */
        /* coverity[double_free=FALSE] */
        rc = B_PlaybackIp_SessionClose(playbackIp);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, Exiting...\n", rc, __LINE__)); exit(1);}
    }

    if (ipCfg.playMultipleStreams) {
        BDBG_WRN((" ############## replaying stream ################ "));
        goto newSession;
    }
#ifdef B_HAS_DTCP_IP
    /* close the per session handle */
    if (ipCfg.security == B_PlaybackIpSecurityProtocol_DtcpIp) {
        if (DtcpAppLib_CloseAke(dtcpCtx, AkeHandle) != BERR_SUCCESS) {
            BDBG_WRN(("%s: failed to close the DTCP AKE session", __FUNCTION__));
        }
        B_PlaybackIp_DtcpIpUnInit(dtcpCtx);
#ifdef B_DTCP_IP_HW_DECRYPTION
        DtcpCleanupHwSecurityParams();
#endif
    }
#endif

#ifdef B_HAS_SSL
    if (ipCfg.security == B_PlaybackIpSecurityProtocol_Ssl){
        B_PlaybackIp_SslUnInit(ssl_ctx);
    }
#endif

    if (ipCfg.fastChannelChange) {
        for (i=0;i<TOTAL_PRIMERS;i++) {
            NEXUS_VideoDecoderPrimer_Close(primer[i]);
        }
    }

    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    /* Do cleanup necessary for quitting application */
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    B_PlaybackIp_Close(playbackIp);
    NEXUS_SyncChannel_Destroy(syncChannel);
    if (dmaHandle != NULL) {
        NEXUS_Dma_Close(dmaHandle);
    }
    if (waitEvent) 
        BKNI_DestroyEvent(waitEvent);
    NEXUS_Platform_Uninit();
    /* For debugging any memory leaks, use this */
    BKNI_Uninit();
    if(errorFlag == 1) {  /* Checking if the run completes successfully or with the errors */
        return -1;
    }
    else{
        return 0;
    }
    
}
