/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_playback_ip.c $
 * $brcm_Revision: 60 $
 * $brcm_Date: 6/27/12 1:59p $
 *
 * Module Description:
 *
 * File implements the Settop API Shim layer for Playback IP Interface.
 * It contains the interface definition of the legacy Settop API implementation
 * (BSEAV/api/src/pvr/bsettop_playback_ip.c) and makes calls into Nexus IP APPlib.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_playback_ip.c $
 * 
 * 60   6/27/12 1:59p ssood
 * SW7435-250: more coverity fixes exposed by new coverity version
 * 
 * 59   6/6/11 1:28a ssood
 * SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
 * eventCallback support for end of stream playback
 * 
 * 58   5/26/11 11:31p ssood
 * SW7231-166: dont create Nexus File Handle for cases where Nexus
 * playpump will get used
 * 
 * 57   5/24/11 2:34p ssood
 * SW7405-4392: Coverity fixes
 * 
 * 56   5/23/11 3:44p ssood
 * SW7231-166: Extend Playback IP trickmodes to be compatible with latest
 * DLNA Guidelines
 * 
 * 55   4/16/10 6:09p ssood
 * SW7420-561: Coverity fix
 * 
 * 54   4/2/10 5:54p ssood
 * SW7420-561: add support to handle large URLs (HTML5 URLs can be over
 * 256bytes) + RTSP fixes
 * 
 * 53   3/4/10 3:18p ssood
 * SW7420-561: merge to mainline
 * 
 * SW7420-561/1   2/16/10 10:11a ssood
 * SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
 * Philips RTSP requirements
 * 
 * 51   12/23/09 3:08p erickson
 * SW7420-516: convert non-error BDBG_ERR to BDBG_WRN
 *
 * 50   10/1/09 11:00a jjordan
 * SW7405-3107: add IP recv timeout status to Settop API
 *
 * 49   9/1/09 6:24p lwhite
 * SW7405-2176: Configure default SslInit field
 *
 * 48   8/24/09 3:23p ssood
 * SW7405-1689: adapting server based trickmodes to Nexus playback changes
 * due to OTF support
 *
 * 47   8/14/09 7:35a ssood
 * PR43746: Coverity fix
 *
 * 46   7/29/09 6:19p ssood
 * PR43746: extend IP trick modes to support Disconnect & Seek based pause
 * operations
 *
 * 46   7/29/09 6:14p ssood
 * PR43746: extend IP trick modes to support Disconnect And Seek based
 * paused operations (DLNA requirement)
 *
 * 46   7/29/09 6:14p ssood
 * PR43746: extend IP trick modes to support Disconnect And Seek based
 * paused operations (DLNA requirement)
 *
 * 45   7/28/09 3:23p lwhite
 * PR55693: Fixed DTCP/IP close
 *
 * 44   7/28/09 11:52a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 43   7/22/09 4:27p ssood
 * PR43746: return current timestamp & position info to allow playbackbar
 * to display correct position during server based trick modes
 *
 * 42   7/21/09 4:18p ssood
 * PR43746: Further work for Server based HTTP trick mode support
 *
 * 41   7/16/09 1:04p ssood
 * PR43746: Add Server based HTTP trick mode support
 *
 * 41   7/16/09 1:03p ssood
 * PR43746: Add Server based HTTP trick mode support
 *
 * 41   7/16/09 12:59p ssood
 * PR43746: Add Server based HTTP trick mode support
 *
 * 41   7/16/09 12:59p ssood
 * PR43746: Add Server based HTTP trick mode support
 *
 * 40   7/14/09 3:28p leisun
 * PR 55693: fix segfault when server is not running and channel is tuned
 * to DTCP
 *
 * 39   7/14/09 2:17p leisun
 * PR 55693: Moved DtcpIpInit() to socketOpen
 *
 * 38   7/13/09 11:50a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 *
 * 37   7/1/09 6:30p ssood
 * PR53773: handle case where DTCP/IP library init may fail
 *
 * 36   6/29/09 2:10p lwhite
 * PR53773: Fixed SSL initialization
 *
 * 35   6/26/09 1:46p mward
 * PR 56277: Coverity Defect ID:16457 UNINIT
 *
 * 34   6/26/09 9:29a lwhite
 * PR53773: Reset SSL context during close
 *
 * 33   6/25/09 7:05p ssood
 * PR53773: Passing in M2M DMA handle for DTCP/IP encryption/decryption
 * purposes
 *
 * 32   6/25/09 10:43a ssood
 * PR53773: Adding support for setting up DTCP/IP Encrypted HTTP session
 *
 * 31   6/25/09 12:58a ssood
 * PR53773: fixing a compilation error
 *
 * 30   6/24/09 6:56p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 *
 * 29   5/26/09 11:50a ssood
 * PR53773: added debug code to track a crash during quit
 *
 * 28   5/25/09 11:33p ssood
 * PR55325: Add support for pre-charging network buffer for HTTP playback
 *
 * 27   3/10/09 3:08p ssood
 * PR50311: Adding support for pause & loop back
 *
 * 26   3/9/09 6:31p lwhite
 * PR50311: added support for VOB playback
 *
 * 25   3/2/09 6:06p ssood
 * PR50311: cleaned up some left out debug code
 *
 * 24   3/2/09 3:19p ssood
 * PR50311: using proper define for updating the Nexus playpump settings
 *
 * 23   3/2/09 2:48p ssood
 * PR50311: another brutus quit crash when we quit from a HTTP channel
 * where connect attempt had failed
 *
 * 22   3/2/09 9:23a ssood
 * PR50311: brutus quit crashes when we quit from a HTTP channel where
 * connect attempt had failed
 *
 * 21   3/1/09 10:38a ssood
 * PR50311: added support for HTTP playback of content w/ extra 4 byte
 * DLNA timestamps
 *
 * 20   11/13/08 5:52p lwhite
 * PR47685: Coverity fixes
 *
 * 19   11/7/08 5:35p ssood
 * PR48146: fixing another quit related crash
 *
 * 18   11/5/08 7:46p ssood
 * PR48146: fixed a core dump during failure path
 *
 * 17   11/5/08 3:27p ssood
 * PR48146: added support needed for MP4/ASF playback
 *
 * 16   10/31/08 12:30p jjordan
 * PR47230: Add TTS Pacing
 *
 * 15   10/15/08 9:22a erickson
 * PR42739: remove ERR from bplayback_ip_open if IP not on. this is a
 * normal call to discover IP capabilities.
 *
 * 14   10/13/08 5:48p ssood
 * PR47521: need audio decoder handles for flushing AV pipeline during a
 * discontinuity due to packet loss
 *
 * 13   10/3/08 3:09p lwhite
 * PR42739: Added RTSP timeline support
 *
 * 12   9/19/08 5:05p vishk
 * PR 47150: Coverity Issues.
 *
 * 11   8/11/08 12:49p ssood
 * PR45431: added cleanup code for ASF playback in error path
 *
 * 10   8/11/08 10:13a ssood
 * PR45431: Add HTTP playback support for ASF content
 *
 * 9   8/5/08 12:54p erickson
 * PR45361: fix c89 builds
 *
 * 8   7/28/08 3:30p ssood
 * PR42739: further changes for supporting example app for IP Applib
 *
 * 7   7/21/08 4:03p ssood
 * PR42739: code changes to simplify ip app lib interface
 *
 * 6   7/10/08 3:46p ssood
 * PR42739: added support for RTSP trickmodes
 *
 * 5   6/24/08 4:10p erickson
 * PR44107: rename to TrickMode
 *
 * 4   6/18/08 7:40a ssood
 * PR43744: Channel Change script fails on IP channels after 5-10min
 *
 * 3   6/15/08 7:01p ssood
 * PR42739: Fixed a core dump due to derefercing the decoder pointer when
 * it may not be set in the bstream
 *
 * 2   6/13/08 7:23p ssood
 * PR42739: Enabling HTTP & RTSP support
 *
 * 1   6/11/08 7:28p ssood
 * PR42739: Settop API Shim layer implementation of Playback IP Interface
 *
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include <bstd.h>
#include "bsettop_os.h"

 /*
    Note this file is currently only compiled for LINUX platforms. Even though it
    has VxWorks & WinCE related hash defines, the code needs to more porting
    work for these OS platforms.

    If the feature isn't compiled in, then the only functions in this .c file
    should be the public stubs.
  */

BDBG_MODULE(playback_ip);
#if defined(B_HAS_IP)
#include <bkni.h>
#include <bkni_multi.h>
#include <fcntl.h>

#ifndef _WIN32_WCE

#ifdef __vxworks
  #include <sysLib.h>
  #include <string.h>
  #include <stdlib.h>
  #include <logLib.h>
  #include <sys/times.h>
  #include <selectLib.h>
  #include <taskLib.h>
  #include <semLib.h>
  #include <sys/ioctl.h>
  #include <hostLib.h>
#else
  #include <memory.h>
  #include <pthread.h>
  #include <sys/time.h>
#endif

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#else /* _WIN32_WCE */
#include <windows.h>
    typedef HANDLE pthread_t;
    #define close(x) closesocket(x)
    unsigned long errno = 0;
    #define EINTR -1
#endif /* _WIN32_WCE */

#include "bsettop_playpump.h"
#include "bsettop_playback_ip.h"
/* SSOOD #include "bsettop_pvr_priv.h"*/
#include "bpool.h"
#include "barena.h"
#include "bioatom.h"
#include "blst_queue.h"
#include "brtp_spf.h"
#include "b_playback_ip_lib.h"
#ifdef B_HAS_DTCP_IP
#include "b_dtcp_applib.h"
#endif

#ifdef B_HAS_NETACCEL
#include "bnetaccel_info.h"
#endif

#define IP_MAX_ITEMS                    128         /* max number of packets that can be pending in the filter */
#define IP_MAX_PKT_SIZE                 1500
#define RTP_PAYLOAD_TYPE_MP2T           33          /* only support PT=33 */
#define MAX_READ                        160
#define TEST_BUFFER_SIZE                65536
#define IP_MAX_PWM_VALUE                0x7FFF
#define IP_MIN_PWM_VALUE                (-IP_MAX_PWM_VALUE)
#define IP_POSTCHARGE_PERIOD_MSEC       5000
#define IP_MAX_DECODE_POLLS             10
#define IP_UNDERFLOW_PERIOD_MSEC        100

/* slew 270 Hz (10 ppm) @ 100 Hz/sec,
 * in steps of 10Hz
 */
#define IP_PWM_STEP_PERIOD              100         /* 0.1 second */
#define IP_PWM_STEP_SIZE                75          /* 10Hz (Hz/bit = 4386/0x7fff = 0.13385 Hz/bit, ie. 0.00494 ppm/bit) */
#define IP_PWM_STEPS_PER_VIOLATION      27

#if defined (LINUX)
  #define STATUS_REPORTING_CONSTANT     1
  #define NEW_RMEM_MAX                  (1024*200)
#else
  #define STATUS_REPORTING_CONSTANT     0
#endif

#if 0
#define BDBG_MSG_FLOW(x)  printf x; printf("\n");
#else
#define BDBG_MSG_FLOW(x)
#endif



#ifdef __vxworks
  #if !defined (USE_PTHREAD)
    typedef int pthread_t;
  #endif
#endif

/**
Summary:
    State of bplayback_ip
**/
typedef enum bplayback_ip_state {
    bplayback_ip_state_stopped,
    bplayback_ip_state_stopping,
    bplayback_ip_state_playing,
    bplayback_ip_state_paused,
    bplayback_ip_state_trickmode
} bplayback_ip_state;

typedef enum bplayback_ip_buffer_state {
    bplayback_ip_buffer_precharging,
    bplayback_ip_buffer_postcharging,
    bplayback_ip_buffer_playing,
    bplayback_ip_buffer_overflowing,
    bplayback_ip_buffer_underflowing
} bplayback_ip_buffer_state;

struct b_playback_ip_item;
BLST_Q_HEAD(b_playback_ip_item_queue, b_playback_ip_item);

struct udp_hdr {
    unsigned short   source;
    unsigned short   dest;
    unsigned short   len;
    unsigned short   checksum;
};

#if !defined(B_HAS_NETACCEL) || defined(B_HAS_PLAYPUMP_IP)
#define PKTS_PER_READ 1
#else
#define PKTS_PER_READ 32
#endif
struct b_playback_ip_item {
    BLST_Q_ENTRY(b_playback_ip_item) item_queue;
    BLST_Q_ENTRY(b_playback_ip_item) active_item_queue;
#if defined(B_HAS_PLAYPUMP_IP) || !defined(B_HAS_NETACCEL)
    uint8_t data[IP_MAX_PKT_SIZE];  /* max UDP packet size (assuming no fragmentation & 1500 byte MTU) */
#else
    uint8_t *data;                  /* max UDP packet size (assuming no fragmentation & 1500 byte MTU) */
#endif
    bplayback_ip_t playback_ip;
#if !defined(B_HAS_PLAYPUMP_IP) && defined(B_HAS_NETACCEL)
    int item_index;
    bool block_end;
#endif
};

/* the playback structure */
struct bplayback_ip {
    /* this must be the 1st field in this structure */
    struct bplayback            playback;
    bplayback_ip_state          playback_state;
    bplaypump_t                 pump;
    bplaypump_params            pumpparams;
    bplayback_ip_buffer_scheme  ip_buffer_scheme;
    bsettop_ip_protocol         protocol;
    B_PlaybackIpHandle          playback_ip_handle;
    bplayback_socket_params     socket_params; /* cached socket params */
    void*                       security_ctx;     /* security context - currently only used by SSL*/
    bool                        transportTimeStampEnabled;
    bool                        handlesSet;     /* set when nexus handles are passed to IP Applib */
    void*                       dtcp_ctx;
    void*                       akeHandle;
    int                         scaleListEntries; /* number of entries in the scale list provided by the server, 0 if none */
    float                       scaleList[MAX_SCALE_LIST_ENTRIES]; /* scale list provided by the server, 0 if none */
    int                         firstPositiveScaleIndex;
    bool                        useNexusPlaypump; /* if set use Nexus Playpump otherwise Nexus Playback is used to feed IP streams */
};

static void bplayback_ip_params_init(bplayback_params *params, bplayback_t playback);
static bresult bplayback_ip_get_status(bplayback_t playback, bplayback_status *status);
static bresult bplayback_ip_get_trickmode_by_rate(bplayback_t playback, int rate, bplayback_trickmode_params *trickmode);
void bplayback_ip_socket_close(bplayback_t playback);
#define abs(x) (x>0 ? x : -x)
#define MAX_BUFFER_SIZE (1316 * PKTS_PER_READ)

/*
Summary:
    playback IP init.
*/
void bplayback_ip_p_init(void)
{
#if defined(B_DTCP_IP_HW_ENCRYPTION) || defined(B_DTCP_IP_HW_DECRYPTION)
    if(DtcpInitHWSecurityParams((void *)g_dma.hDma) != BERR_SUCCESS)
    {
        BDBG_ERR(("%s: Failed to Initialize Dtcp/Ip HW Secruity\n", __FUNCTION__));
    }
#endif
}

/*
Summary:
    playback IP uninit.
*/
void bplayback_ip_p_uninit(void)
{
#if defined(B_DTCP_IP_HW_ENCRYPTION) || defined(B_DTCP_IP_HW_DECRYPTION)
    DtcpCleanupHwSecurityParams();
#endif
}

/*
Summary:
    Open an IP playback channel.
*/
bplayback_t bplayback_ip_p_open(void)
{
    bplayback_ip_t playback_ip;
    B_PlaybackIpOpenSettings pSettings;

    playback_ip = BKNI_Malloc(sizeof(struct bplayback_ip));
    if (!playback_ip)
        return NULL;
    BKNI_Memset(playback_ip, 0, sizeof(struct bplayback_ip));
    BDBG_MSG(("%s: Allocated %d bytes\n", __FUNCTION__, sizeof(struct bplayback_ip)));

    /* initialize interface */
    bplayback_iface_init(&playback_ip->playback.iface);

    playback_ip->playback.iface.params_init = bplayback_ip_params_init;
    playback_ip->playback.iface.start = bplayback_ip_start;

    playback_ip->playback.iface.pause = bplayback_ip_pause;
    playback_ip->playback.iface.play = bplayback_ip_play;
    playback_ip->playback.iface.trickmode = bplayback_ip_trickmode;
    playback_ip->playback.iface.get_trickmode_by_rate = bplayback_ip_get_trickmode_by_rate;

    playback_ip->playback.iface.stop = bplayback_ip_stop;
    playback_ip->playback.iface.get_status = bplayback_ip_get_status;
    playback_ip->playback.iface.close = bplayback_ip_close;
    playback_ip->playback.iface.goto_index = bplayback_ip_goto_index;

    memset(&pSettings, 0, sizeof(pSettings));
    playback_ip->playback_ip_handle = B_PlaybackIp_Open(NULL);
    if (playback_ip->playback_ip_handle == NULL) {
        BDBG_ERR(("Failed to Setup the IP Playback Channel\n"));
    }
    playback_ip->playback_state = bplayback_ip_state_stopped;
    playback_ip->handlesSet = false;
    return (bplayback_t)&playback_ip->playback.iface;

}

/*
Summary:
    Close a playback channel.
*/
void bplayback_ip_p_close(
    bplayback_t playback /* Handle returned by bplayback_ip_open */
    )
{
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;

    BDBG_MSG(("%s: \n", __FUNCTION__));
    if(playback_ip->playback_state != bplayback_ip_state_stopped)
        bplayback_ip_stop(playback);

    if (playback_ip->playback.source) {
        bplayback_ip_socket_close(playback);
        playback_ip->playback.source = NULL;
    }

    B_PlaybackIp_Close(playback_ip->playback_ip_handle);
    playback_ip->playback_ip_handle = NULL;

    BKNI_Free(playback_ip);
}

void bplayback_ip_params_init(bplayback_params *params, bplayback_t playback)
{
    BSTD_UNUSED(playback);
    params->ip_params.buffer_scheme = bplayback_ip_buffer_scheme_precharge;
    params->ip_params.use_live_playback_mode = true;
    params->timestamp_enabled =  false;
    params->tts_params.auto_detect =  true;
    params->tts_params.pacing_max_error = 527;
    params->tts_params.init_buf_depth = 600000;
    params->tts_params.min_buf_depth = 0;
    params->tts_params.max_buf_depth = 1000000;
    params->tts_params.max_clock_mismatch = 100;
}

/*
Summary:
  Configures IP playback to be started.

Description:
    If bplayback_ip_start succeeds, it returns a digital stream handle which can
    be decoded or recorded.
*/
bstream_t bplayback_ip_start(
    bplayback_t             playback,
    bplaypump_t             playpump,
    const bstream_mpeg      *mpeg,
    bplayback_file_t        source,
    const bplayback_params  *params)
{
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    NEXUS_Error rc;
    B_PlaybackIpSettings playback_ip_settings;
    B_PlaybackIpSessionStartSettings ipStartSettings;
    B_PlaybackIpSessionStartStatus ipStartStatus;
    bool is_tts;

    BDBG_MSG(("%s: entering: playback %p\n", __FUNCTION__, (void *)playback));

    playback_ip->pump = playpump;
    playback_ip->playback.source = source;
    playback_ip->ip_buffer_scheme = params->ip_params.buffer_scheme;


    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    bplaypump_params_init(&playback_ip->pumpparams, playback_ip->pump);

    if (playback_ip->protocol == bsettop_ip_protocol_http) {
        playback_ip->pumpparams.is_playback_ip = true;
        if (playback_ip->transportTimeStampEnabled) {
            playback_ip->pumpparams.timestamp_enabled = true;
        }
        if (playback_ip->useNexusPlaypump) {
            /* Start Playpump, this in turn starts the Nexus Playpump */
            playback_ip->playback.stream = bplaypump_start(playback_ip->pump, mpeg, &playback_ip->pumpparams);
            if (!playback_ip->playback.stream) {
                BDBG_ERR(("%s: Failed to start Nexus Playpump", __FUNCTION__));
                return NULL;
            }
        }
        else {
            NEXUS_PlaybackSettings playbackSettings;

            BDBG_MSG(("%s: Starting Nexus Playback..., nexus playback %p, playpump %p \n",
                        __FUNCTION__, (void *)playback_ip->playback.nPlayback, (void *)playpump->nPlaypump));
            NEXUS_Playback_GetSettings(playback_ip->playback.nPlayback, &playbackSettings);
            playbackSettings.playpump = playpump->nPlaypump;
            playbackSettings.playpumpSettings.transportType = b_mpegtype2nexus(mpeg->mpeg_type);
            playbackSettings.playpumpSettings.mode = NEXUS_PlaypumpMode_eFifo;
            playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop; /* loop content */
            if (playback_ip->transportTimeStampEnabled) {
                playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
                playbackSettings.playpumpSettings.timestamp.pacing = false;
            }
            rc = NEXUS_Playback_SetSettings(playback_ip->playback.nPlayback, &playbackSettings);
            if (rc) {
                BDBG_ERR(("%s: Failed to update the Nexus Playback Settings, rc = %d\n", __FUNCTION__, rc));
                return NULL;
            }

            /* Now open the bstream */
            playback_ip->playback.stream = bstream_p_open(NULL, NULL, playback, 0, mpeg);
            if (!playback_ip->playback.stream) {
                BDBG_ERR(("%s: Failed to create the stream\n", __FUNCTION__));
                return NULL;
            }
            playback_ip->pump->stream = playback_ip->playback.stream;

            /* Note: Nexus playback is started by the bdecode_start -> bplayback_p_start() */
        }
    }
    else {
        playback_ip->pumpparams.is_playback_ip = true;
        playback_ip->pumpparams.use_live_playback_mode = params->ip_params.use_live_playback_mode;
        if(params->timestamp_active && params->timestamp_enabled) {
            /* a TTS */
            playback_ip->pumpparams.timestamp_active = true;
            playback_ip->pumpparams.timestamp_enabled = true;
        }
        else if(params->tts_params.auto_detect) {
            if(B_PlaybackIp_DetectTts(playback_ip->playback_ip_handle, &is_tts)!=B_ERROR_SUCCESS) {
                BDBG_ERR(("Failed to detect TTS/non-TTS"));
                return NULL;
            }
            if(is_tts) {
                /* a TTS */
                playback_ip->pumpparams.timestamp_active = true;
                playback_ip->pumpparams.timestamp_enabled = true;
                BDBG_WRN(("B_PlaybackIp_DetectTts() returned is_tts=true"));
            }
            else {
                /* not a TTS */
                playback_ip->pumpparams.timestamp_active = params->timestamp_active;
                playback_ip->pumpparams.timestamp_enabled = params->timestamp_enabled;
                BDBG_WRN(("B_PlaybackIp_DetectTts() returned is_tts=false"));
            }
        }
        else {
            /* not a TTS */
            playback_ip->pumpparams.timestamp_active = params->timestamp_active;
        playback_ip->pumpparams.timestamp_enabled = params->timestamp_enabled;
        }

        B_PlaybackIp_GetSettings(playback_ip->playback_ip_handle, &playback_ip_settings);

        if(playback_ip->pumpparams.timestamp_enabled) {
            playback_ip_settings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip;
            playback_ip->pumpparams.pacing_max_error = params->tts_params.pacing_max_error;
            BDBG_WRN(("Setup for TTS stream"));
        }
        else {
            /* TODO: support PCR pacing */
#if 0
            playback_ip->init_buf_depth = params->tts_params.init_buf_depth;
            playback_ip->pcr_pid = mpeg->pcr_pid;
#endif
            if(playback_ip->pumpparams.timestamp_active) {
                playback_ip_settings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip;
                BDBG_WRN(("Setup for non-TTS stream (PCR-pacing version)"));
            }
            else {
                playback_ip_settings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
                BDBG_WRN(("Setup for non-TTS stream (non-PCR-pacing version)"));
            }
        }

        if(playback_ip_settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
            playback_ip_settings.ttsParams.autoDetect = params->tts_params.auto_detect;
            playback_ip_settings.ttsParams.pacingMaxError = params->tts_params.pacing_max_error;
            playback_ip_settings.ttsParams.throttleParams.initBufDepth = params->tts_params.init_buf_depth;
            playback_ip_settings.ttsParams.throttleParams.maxBufDepth = params->tts_params.max_buf_depth;
            playback_ip_settings.ttsParams.throttleParams.minBufDepth = params->tts_params.min_buf_depth;
            playback_ip_settings.ttsParams.throttleParams.maxClockMismatch = params->tts_params.max_clock_mismatch;
            BDBG_WRN(("auto_detect: %d, init_buf_depth: %d, max_buf_depth: %d, min_buf_depth: %d, pacing_max_error: 0x%x, max_clock_mismatch: %d",
                      params->tts_params.auto_detect,
                      params->tts_params.init_buf_depth,
                      params->tts_params.max_buf_depth,
                      params->tts_params.min_buf_depth,
                      params->tts_params.pacing_max_error,
                      params->tts_params.max_clock_mismatch));
        }

        if (B_PlaybackIp_SetSettings(playback_ip->playback_ip_handle, &playback_ip_settings) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Failed to update IP Settings", __FUNCTION__));
            return NULL;
        }

        /* Start Playpump, this in turn starts the Nexus Playpump */
        playback_ip->playback.stream = bplaypump_start(playback_ip->pump, mpeg, &playback_ip->pumpparams);
        if (!playback_ip->playback.stream) {
            BDBG_ERR(("%s: Failed to create the stream", __FUNCTION__));
            return NULL;
        }
    }

    memset(&ipStartSettings, 0, sizeof(ipStartSettings));
    ipStartSettings.nexusHandles.playpump = playpump->nPlaypump; /* we dont yet know the rest of the handles, they get set in the GetStatus call */
    if (playback_ip->playback.nPlayback) {
        ipStartSettings.nexusHandles.playback = playback_ip->playback.nPlayback;
    }
    ipStartSettings.nexusHandlesValid = true;
    if (playback_ip->protocol == bsettop_ip_protocol_http)
        ipStartSettings.u.http.preChargeBuffer = false;
    else
        ipStartSettings.u.rtsp.mediaTransportProtocol = B_PlaybackIpProtocol_eRtp;
    rc = B_PlaybackIp_SessionStart(playback_ip->playback_ip_handle, &ipStartSettings, &ipStartStatus);
    if (rc) {
        BDBG_ERR(("%s: IP Session Start failed: %d\n", __FUNCTION__, rc));
        B_PlaybackIp_SessionClose(playback_ip->playback_ip_handle);
        goto error;
    }

    BDBG_WRN(("%s: IP Playback is Ready to Receive Data:", __FUNCTION__));

    playback_ip->playback_state = bplayback_ip_state_playing;
    return playback_ip->playback.stream;

error:
    bplayback_ip_stop((bplayback_t)playback_ip);
    return NULL;
}

/*
Summary:
    Stop playback.
Description:
    If you are decoding the stream, the decode will be stopped as well.
*/
bresult bplayback_ip_stop(bplayback_t playback)
{
    B_PlaybackIpError err;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;

    BDBG_WRN(("%s:", __FUNCTION__));

    playback_ip->playback_state = bplayback_ip_state_stopping;

    if ( (err = B_PlaybackIp_SessionStop(playback_ip->playback_ip_handle)) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s:%d IP Applib Error: %d\n", __FUNCTION__, __LINE__, err));
        return BSETTOP_ERROR(berr_external_error);
    }

    BKNI_Sleep(100);

    if (playback_ip->protocol == bsettop_ip_protocol_http) {
        if (playback_ip->useNexusPlaypump) 
            bplaypump_stop(playback_ip->pump);
        else {
            bstream_p_close(playback_ip->pump->stream);
            playback_ip->pump->stream = NULL;
            NEXUS_Playback_Stop(playback_ip->playback.nPlayback);
        }
    }
    else {
        bplaypump_stop(playback_ip->pump);
    }

    playback_ip->playback_state = bplayback_ip_state_stopped;

    return b_ok;
}

/*
Summary:
    Open a IP playback socket to pass to bplayback_ip_start().
Description:
    The bplayback_ip_source_t can be used for only one IP playback at a time.
*/
bplayback_file_t bplayback_ip_socket_open(
    bplayback_t playback, /* Handle returned by bplayback_ip_open */
    bplayback_socket_params *params
    )
{
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    B_PlaybackIpError rc;
    bplayback_file_t ip_source;
    B_PlaybackIpPsiInfo psi;
    B_PlaybackIpSocketState socketState;
    B_PlaybackIpSessionOpenSettings sessionOpenSettings;
    B_PlaybackIpSessionOpenStatus sessionOpenStatus;
    B_PlaybackIpSessionSetupSettings sessionSetupSettings;
    B_PlaybackIpSessionSetupStatus sessionSetupStatus;
#ifdef B_HAS_SSL
    B_PlaybackIpSslInitSettings sslInitSettings;
#endif
#ifdef B_HAS_DTCP_IP
    struct timeval t_start, t_finish;
    NEXUS_Error err;
    int dtcp_server_port;
#endif
    int i;
    B_PlaybackIpSettings playbackIpSettings;

    ip_source = BKNI_Malloc(sizeof(*ip_source));
    if (!ip_source) {
        rc = BSETTOP_ERROR(berr_out_of_memory);
        goto error;
    }
    BKNI_Memset(ip_source, 0, sizeof(*ip_source));
    BKNI_Memset(&socketState, 0, sizeof(socketState));
    BKNI_Memset(&sessionOpenSettings, 0, sizeof(sessionOpenSettings));
    BKNI_Memset(&sessionOpenStatus, 0, sizeof(sessionOpenStatus));
    BKNI_Memset(&psi, 0, sizeof(psi));
    playback_ip->playback.source = ip_source;

    /* Setup the socket setting structure using the input socket params */
    memcpy(sessionOpenSettings.socketOpenSettings.ipAddr, params->open_params.ip_addr, sizeof(params->open_params.ip_addr));
    memset(&sessionSetupSettings, 0, sizeof(sessionSetupSettings));
    sessionOpenSettings.socketOpenSettings.port = params->open_params.port;
    sessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
    sessionOpenSettings.maxNetworkJitter = 300;
    sessionOpenSettings.networkTimeout = 1;

    switch (params->open_params.protocol) {
    case bsettop_ip_protocol_udp:
        sessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eUdp;
        break;
    case bsettop_ip_protocol_rtp:
        sessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eRtp;
        break;
    case bsettop_ip_protocol_rtp_no_rtcp:
        sessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eRtpNoRtcp;
        break;
    case bsettop_ip_protocol_rtsp:
        {
        char *tmp1, *tmp2;
        sessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eRtsp;
        /* For RTSP, brutus layer doesn't initialize ipAddr & port fields, rather they are included part of the url */
        /* extract ipAddr & port information from URL and specify them as required by IP Applib */
        if ( (tmp1 = strstr(params->open_params.url, "rtsp://")) != NULL ) {
            tmp1 += strlen("rtsp://"); /* tmp1 points to ip address */
            if ( (tmp2 = strstr(tmp1, ":")) != NULL ) {
                strncpy(sessionOpenSettings.socketOpenSettings.ipAddr, tmp1, (tmp2 - tmp1));
                sessionOpenSettings.socketOpenSettings.ipAddr[tmp2-tmp1] = 0;

                /* now get the port info */
                tmp1 = tmp2 + 1; /* get past the : and now it points to the port */
                if ( (tmp2 = strstr(tmp1, "/")) != NULL ) {
                    char tmp3[16];
                    strncpy(tmp3, tmp1, (tmp2 - tmp1));
                    tmp3[tmp2-tmp1] = 0;
                    sessionOpenSettings.socketOpenSettings.port = strtol(tmp3, NULL, 10);

                    /* now get the url, tmp2 already points to start of the URL */
                    sessionOpenSettings.socketOpenSettings.url = tmp2;
                }
            }
        }
        else {
            rc = BSETTOP_ERROR(berr_invalid_parameter);
            goto error;
        }
        break;
        }
    case bsettop_ip_protocol_http:
        sessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eHttp;
        sessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePull;
        sessionOpenSettings.u.http.networkBufferSize = (20*3*1024*1024)/8; /* data cache size: 20sec worth for a max bitrate of 3Mbps */
        sessionOpenSettings.socketOpenSettings.url = params->open_params.url;
        sessionSetupSettings.u.http.enablePayloadScanning = true;
        break;
    default:
        rc = BSETTOP_ERROR(berr_invalid_parameter);
        goto error;
    }

    /* Initialize security */

    switch (params->open_params.security.security_protocol) 
    {
    case bsettop_ip_security_ssl:
#ifdef B_HAS_SSL
        sessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
        sslInitSettings.rootCaCertPath=params->open_params.security.security_info.ssl.ca_file;
        sslInitSettings.sslLibInitDone = false;
        if (strlen(params->open_params.security.security_info.ssl.client_cert_file)) {
            sslInitSettings.clientAuth=true;
            sslInitSettings.ourCertPath=params->open_params.security.security_info.ssl.client_cert_file;
            if (strlen(params->open_params.security.security_info.ssl.client_cert_key))
                sslInitSettings.privKeyPath= params->open_params.security.security_info.ssl.client_cert_key;
            if (strlen(params->open_params.security.security_info.ssl.password))
                sslInitSettings.password= params->open_params.security.security_info.ssl.password;
        } else
            sslInitSettings.clientAuth=false;
        playback_ip->security_ctx = B_PlaybackIp_SslInit(&sslInitSettings);
        if (!playback_ip->security_ctx) {
            BDBG_ERR(("%s: SSL Security initialization failed \n", __FUNCTION__ ));
            goto error;
        }
        sessionOpenSettings.security.initialSecurityContext = playback_ip->security_ctx;
#else
        BDBG_ERR(("%s: Need to compile with B_HAS_SSL defined\n", __FUNCTION__ ));
        goto error;
#endif
        break;
    case bsettop_ip_security_dtcp_ip:
#ifdef B_HAS_DTCP_IP
        sessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_DtcpIp;

        playback_ip->dtcp_ctx = params->open_params.security.security_info.dtcp_ip.dtcp_ip_ctx;
        if(playback_ip->dtcp_ctx == NULL) {
            BDBG_ERR(("%s: Failed to Initialize Dtcp/Ip Library: DTCP/IP encryption/decryption won't work\n", __FUNCTION__));
            goto error;
        }
        /* Perform AKE for DTCP/IP */
        if (params->open_params.security.security_info.dtcp_ip.ake_server_port == 0)
            dtcp_server_port = 8000;
        else
            dtcp_server_port = params->open_params.security.security_info.dtcp_ip.ake_server_port;
        BDBG_WRN(("%s: setting up DTCP/IP AKE session with %s:%d\n", __FUNCTION__, sessionOpenSettings.socketOpenSettings.ipAddr, dtcp_server_port));
        gettimeofday(&t_start, 0);
        if((err = DtcpAppLib_DoAke(playback_ip->dtcp_ctx, sessionOpenSettings.socketOpenSettings.ipAddr,
                        dtcp_server_port, &playback_ip->akeHandle)) != BERR_SUCCESS) {
            BDBG_ERR(("DTCP AKE Failed!!!\n"));
            goto error;
        }
        gettimeofday(&t_finish, 0);
        BDBG_WRN(("%s: DTCP/IP AKE Successful (time consumed %d secs and %d msecs)\n", __FUNCTION__,
            (t_finish.tv_usec < t_start.tv_usec? t_finish.tv_sec - t_start.tv_sec - 1: t_finish.tv_sec - t_start.tv_sec),
            (t_finish.tv_usec < t_start.tv_usec? t_finish.tv_usec - t_start.tv_usec + 1000000: t_finish.tv_usec - t_start.tv_usec) ));
        sessionOpenSettings.security.initialSecurityContext = playback_ip->akeHandle;
#else
        BDBG_ERR(("%s: Need to compile with B_HAS_DTCP_IP defined\n" ));
        goto error;
#endif
        break;
    case bsettop_ip_security_radea:
#ifdef B_HAS_RAD_EA
        sessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_RadEa;
        B_PlaybackIp_RadEaInit();
#else
        BDBG_ERR(("%s: Need to compile with B_HAS_RAD_EA defined\n" ));
        goto error;
#endif
        break;
    default:
        sessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_None;
    }
    BDBG_MSG(("%s: Security %d \n", __FUNCTION__, sessionOpenSettings.security.securityProtocol));

    rc = B_PlaybackIp_SessionOpen(playback_ip->playback_ip_handle, &sessionOpenSettings, &sessionOpenStatus);
    if (rc != B_ERROR_SUCCESS) {
        BDBG_ERR(("B_PlaybackIp_SessionOpen failed"));
        goto error;
    }
    params->open_params.fd = sessionOpenStatus.socketState.fd;
    socketState = sessionOpenStatus.socketState;

    rc = B_PlaybackIp_SessionSetup(playback_ip->playback_ip_handle, &sessionSetupSettings, &sessionSetupStatus);
    if (rc != B_ERROR_SUCCESS) {
        BDBG_ERR(("B_PlaybackIp_SessionSetup failed"));
        B_PlaybackIp_SessionClose(playback_ip->playback_ip_handle);
        goto error;
    }

    B_PlaybackIp_GetSettings(playback_ip->playback_ip_handle, &playbackIpSettings);
    switch (params->open_params.protocol) {
        case bsettop_ip_protocol_http:
            ip_source->nFile = sessionSetupStatus.u.http.file;
            /* now copy the PSI information into the open_params */
            /* For HTTP protocol, PSI info can be returned by the server in the response header */
            psi = sessionSetupStatus.u.http.psi;
            if (psi.psiValid == true) {
                BDBG_MSG(("%s: PSI Info is Retrieved from HTTP Response Headers\n", __FUNCTION__));
                bstream_mpeg_init(&(params->open_params.psi));
                params->open_params.psi.video[0].pid = psi.videoPid;
                params->open_params.psi.video[0].format = b_nexus2videocodec(psi.videoCodec);
#if 0
                switch (psi.videoCodec) {
                    case NEXUS_VideoCodec_eMpeg1:
                        params->open_params.psi.video[0].format = bvideo_codec_mpeg1;
                        break;
                    case NEXUS_VideoCodec_eMpeg2:
                        params->open_params.psi.video[0].format = bvideo_codec_mpeg2;
                        break;
                    case NEXUS_VideoCodec_eH264:
                        params->open_params.psi.video[0].format = bvideo_codec_h264;
                        break;
                    case NEXUS_VideoCodec_eVc1SimpleMain:
                        params->open_params.psi.video[0].format = bvideo_codec_vc1_sm;
                        break;
                    case NEXUS_VideoCodec_eDivx311:
                        params->open_params.psi.video[0].format = bvideo_codec_divx_311;
                        break;
                    case NEXUS_VideoCodec_eVc1:
                        params->open_params.psi.video[0].format = bvideo_codec_vc1;
                        break;
                    default:
                        BDBG_ERR(("%s: Video Codec %d is not yet mapped to Settop API\n", __FUNCTION__, psi.videoCodec));
                }
#endif
                params->open_params.psi.audio[0].pid = psi.audioPid;
                params->open_params.psi.audio[0].format = b_nexus2audiocodec(psi.audioCodec);

#if 0
                switch (psi.audioCodec) {
                    case NEXUS_AudioCodec_eAc3:
                        params->open_params.psi.audio[0].format = baudio_format_ac3;
                        break;
                    case NEXUS_AudioCodec_eAc3Plus:
                        params->open_params.psi.audio[0].format = baudio_format_ac3_plus;
                        break;
                    case NEXUS_AudioCodec_eWmaStd:
                        params->open_params.psi.audio[0].format = baudio_format_wma_std;
                        break;
                    case NEXUS_AudioCodec_eWmaPro:
                        params->open_params.psi.audio[0].format = baudio_format_wma_pro;
                        break;
                    case NEXUS_AudioCodec_eMpeg:
                        params->open_params.psi.audio[0].format = baudio_format_mpeg;
                        break;
                    case NEXUS_AudioCodec_eAac:
                        params->open_params.psi.audio[0].format = baudio_format_aac;
                        break;
                    default:
                        BDBG_ERR(("%s: Audio Codec %d is not yet mapped to Settop API\n", __FUNCTION__, psi.audioCodec));
                }
#endif
                params->open_params.psi.pcr_pid = psi.pcrPid;
                params->open_params.psi.mpeg_type = psi.mpegType;

                /* TODO: add more conversions as they are supported */
                if (psi.mpegType == NEXUS_TransportType_eTs) {
                    params->open_params.psi.mpeg_type = bstream_mpeg_type_ts;
                    playback_ip->transportTimeStampEnabled = psi.transportTimeStampEnabled;
                }
                else if (psi.mpegType == NEXUS_TransportType_eAsf)
                    params->open_params.psi.mpeg_type = bstream_mpeg_type_asf;
                else if (psi.mpegType == NEXUS_TransportType_eMp4)
                    params->open_params.psi.mpeg_type = bstream_mpeg_type_mp4;
                else if (psi.mpegType == NEXUS_TransportType_eVob)
                    params->open_params.psi.mpeg_type = bstream_mpeg_type_vob;
                else
                    BDBG_ERR(("%s: Transport type %d is not mapped to Settop API\n", __FUNCTION__, psi.mpegType));


                params->open_params.num_play_speed_entries = psi.numPlaySpeedEntries;
                params->open_params.first_pts = psi.firstPts;
                params->open_params.duration = psi.duration;
                params->open_params.http_min_iframe_speed = psi.httpMinIFrameSpeed;
                params->open_params.http_frame_repeat = psi.httpFrameRepeat;
                memcpy(params->open_params.play_speed, psi.playSpeed, IP_PVR_PLAYSPEED_MAX_COUNT);
            }
            else {
                BDBG_ERR(("%s: ERROR: Failed to obtain PSI Info for HTTP Playback", __FUNCTION__));
                params->open_params.psi.mpeg_type = bstream_mpeg_type_unknown;
                goto error;
            }
            if (psi.numPlaySpeedEntries > 1 /* server side trickmodes using playspeed */ || psi.hlsSessionEnabled || psi.liveChannel) {
                playbackIpSettings.useNexusPlaypump = true;
            }
            else {
                /* otherwise, we use Nexus Playback to pull streams coming over IP */
                playbackIpSettings.useNexusPlaypump = false;
                /* For HTTP, we use Nexus Playback to feeding IP data */
                if ( (playback_ip->playback.nPlayback = NEXUS_Playback_Create()) == NULL ) {
                    BDBG_ERR(("%s: Failed to create Nexus Playback Handle\n", __FUNCTION__));
                    goto error;
                }
            }
            playbackIpSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePull;
            break;
        case bsettop_ip_protocol_rtsp:
            playback_ip->firstPositiveScaleIndex = 0;
            playback_ip->scaleListEntries = sessionSetupStatus.u.rtsp.scaleListEntries;
            for (i=0; i<playback_ip->scaleListEntries; i++) {
                playback_ip->scaleList[i] = sessionSetupStatus.u.rtsp.scaleList[i];
                if (playback_ip->scaleList[i] > 0 && !playback_ip->firstPositiveScaleIndex)
                    playback_ip->firstPositiveScaleIndex = i;
                BDBG_MSG(("scaleList[%d] %f, +ve Speed idx %d", playback_ip->scaleListEntries, playback_ip->scaleList[i], playback_ip->firstPositiveScaleIndex));
            }
            /*#define TEST_CODE*/
#ifdef TEST_CODE
            playback_ip->firstPositiveScaleIndex = 5;
            playback_ip->scaleListEntries = 11;
            playback_ip->scaleList[0] = -24;
            playback_ip->scaleList[1] = -16;
            playback_ip->scaleList[2] = -8;
            playback_ip->scaleList[3] = -4;
            playback_ip->scaleList[4] = -2;
            playback_ip->scaleList[5] = 1;
            playback_ip->scaleList[6] = 2;
            playback_ip->scaleList[7] = 4;
            playback_ip->scaleList[8] = 8;
            playback_ip->scaleList[9] = 16;
            playback_ip->scaleList[10] = 24;
#endif
            /* fall below */
        default:
            /* for non-HTTP protocols, we always use playpump to feed data */
            playbackIpSettings.useNexusPlaypump = true;
            /* default clock recovery mode */
            playbackIpSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
    }
    playback_ip->useNexusPlaypump = playbackIpSettings.useNexusPlaypump;
    if (B_PlaybackIp_SetSettings(playback_ip->playback_ip_handle, &playbackIpSettings) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: Failed to update IP Settings", __FUNCTION__));
        return NULL;
    }
    BDBG_WRN(("IP Clock Recovery Mode set to %s Mode, using Nexus %s", 
                playbackIpSettings.ipMode == B_PlaybackIpClockRecoveryMode_ePull? "non-Live":"Live", 
                playbackIpSettings.useNexusPlaypump? "Playpump" : "Playback"));

    memcpy(&ip_source->socket, &socketState, sizeof(B_PlaybackIpSocketState));
    playback_ip->protocol = params->open_params.protocol;
    playback_ip->socket_params = *params;
    return (ip_source);

error:
    if (ip_source) {
        playback_ip->playback.source = NULL;
        BKNI_Free(ip_source);
    }

    if (playback_ip->playback.nPlayback) {
        NEXUS_Playback_Destroy(playback_ip->playback.nPlayback);
        playback_ip->playback.nPlayback = NULL;
    }
    return NULL;
}

/*
Summary:
    Close a IP playback socket.
Description:
    Close a IP playback socket opened by bplayback_ip_socket_open(). Playback
    must be stopped BEFORE socket is closed
*/
void bplayback_ip_socket_close(
    bplayback_t playback /* Handle returned by bplayback_ip_open */
    )
{
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;

    BDBG_MSG(("%s: \n", __FUNCTION__));
    /* closing the socket while running is not supported */
    if(playback_ip->playback_state != bplayback_ip_state_stopped)
        goto error;

    B_PlaybackIp_SessionClose(playback_ip->playback_ip_handle);
#ifdef B_HAS_DTCP_IP
    if (playback_ip->akeHandle) {
        if (DtcpAppLib_CloseAke(playback_ip->dtcp_ctx, playback_ip->akeHandle) != BERR_SUCCESS) {
            BDBG_WRN(("%s: failed to close the DTCP AKE session\n", __FUNCTION__));
        }
        playback_ip->akeHandle = NULL;
    }
#if 0
    if (playback_ip->dtcp_ctx) {
        B_PlaybackIp_DtcpIpUnInit(playback_ip->dtcp_ctx);
        playback_ip->dtcp_ctx = NULL;
    }
#endif
#endif
    if (playback_ip->playback.source) {
        BKNI_Free(playback_ip->playback.source);
        playback_ip->playback.source = NULL;     /* PR27770 */
    }
    if (playback_ip->protocol == bsettop_ip_protocol_http && playback_ip->playback.nPlayback) {
        NEXUS_Playback_Destroy(playback_ip->playback.nPlayback);
        playback_ip->playback.nPlayback = NULL;
    }
    playback_ip->handlesSet = false;
    playback_ip->useNexusPlaypump = false;

#ifdef B_HAS_SSL
    if (playback_ip->security_ctx) {
        B_PlaybackIp_SslUnInit(playback_ip->security_ctx);
        playback_ip->security_ctx = NULL;
    }
#endif


    BKNI_Sleep(500);

    return;

error:
    BDBG_ERR(("bplayback_ip_socket_close error"));
    BDBG_ASSERT(NULL);
    return;
}

/*
Summary:
    Required to initialize the bplayback_ip_source_params structure.
*/
void
bplayback_ip_source_params_init(
    bplayback_socket_params *params,    /* [out] */
    bplayback_t             playback /* required for possible resource-dependent defaults */
    )
{
    BSTD_UNUSED(playback);
    strncpy(params->open_params.ip_addr, "127.0.0.1", sizeof("127.0.0.1"));         /* default to loopback address */
    params->open_params.port = 5001;                            /* default port number */
    params->open_params.protocol = bsettop_ip_protocol_udp;     /* default to non-RTP stream */
}

static bresult 
set_nexus_handles(bplayback_ip_t playback_ip)
{
    B_PlaybackIpError err;

    if (!playback_ip->handlesSet) {
        B_PlaybackIpSettings pSettings;
        BDBG_MSG(("%s: updating nexus handles...", __FUNCTION__));
        playback_ip->handlesSet = true;
        memset((char *)&pSettings, 0, sizeof(pSettings));
        err = B_PlaybackIp_GetSettings(playback_ip->playback_ip_handle, &pSettings);

        pSettings.nexusHandles.videoDecoder = playback_ip->playback.stream->consumers.decode->video_decode->nVideoDecoder;
        pSettings.nexusHandles.primaryAudioDecoder = playback_ip->playback.stream->consumers.decode->audio_decode->nAudioDecoder;
        if (playback_ip->playback.stream->consumers.decode->audio_decode->secondary_audio_decode)
            pSettings.nexusHandles.secondaryAudioDecoder =
                playback_ip->playback.stream->consumers.decode->audio_decode->secondary_audio_decode->nAudioDecoder;
        pSettings.nexusHandles.stcChannel = playback_ip->playback.stream->consumers.decode ? playback_ip->playback.stream->consumers.decode->stcChannel : NULL;
        pSettings.nexusHandlesValid = true;
        err = B_PlaybackIp_SetSettings(playback_ip->playback_ip_handle, &pSettings);
        if (err != B_ERROR_SUCCESS)
            return berr_external_error;
    }
    return b_ok;
}

bresult
bplayback_ip_get_status(bplayback_t playback, bplayback_status *status)
{
    bresult rc = b_ok;
    B_PlaybackIpError err;
    NEXUS_PlaybackStatus playbackStatus;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    B_PlaybackIpStatus playbackIpStatus;

    BKNI_Memset(status, 0, sizeof(*status));
    BKNI_Memset(&playbackStatus, 0, sizeof(playbackStatus));

    if (!playback_ip)
        return berr_external_error;

    if (playback_ip->playback_state == bplayback_ip_state_stopped) {
        return berr_invalid_state;
    }
    if (!playback_ip->playback.stream || !playback_ip->playback.stream->consumers.decode ||
            !playback_ip->playback.stream->consumers.decode->video_decode ||
            !playback_ip->playback.stream->consumers.decode->video_decode->nVideoDecoder ||
            !playback_ip->playback.stream->consumers.decode->audio_decode ||
            !playback_ip->playback.stream->consumers.decode->audio_decode->nAudioDecoder
            ) {
        return berr_external_error;
    }

    if (set_nexus_handles(playback_ip) != b_ok) {
        BDBG_ERR(("Failed to set the nexus handles w/ IP library \n"));
        return berr_external_error;
    }

    err = B_PlaybackIp_GetStatus(playback_ip->playback_ip_handle, &playbackIpStatus);
    if (err != B_ERROR_SUCCESS)
        return berr_external_error;
    if (playback_ip->playback.nPlayback)
    {
        NEXUS_Error nerr = NEXUS_Playback_GetStatus(playback_ip->playback.nPlayback, &playbackStatus);
        if (nerr)
            return berr_external_error;
    }

    /* TODO: trickmodes: needed this to get pause to work, otherwise reading state from IP Applib was causing issues */
    if (playback_ip->playback_state == bplayback_ip_state_playing) {
        status->state = bplayback_state_playing;
    }
    else if (playback_ip->playback_state == bplayback_ip_state_paused) {
        status->state = bplayback_state_paused;
    }
    else {
        BDBG_ERR(("%s: Warning: Not supported playback ip state %d, s/w bug\n", __FUNCTION__, playback_ip->playback_state));
    }

    switch (playbackIpStatus.ipState)
    {
    case B_PlaybackIpState_eStopped:
        /* we igore the stopped state of the IP Applib. Otherwise, upon channel change */
        /* brutus wont call bsettop_playback_ip_stop to tear down */
    case B_PlaybackIpState_ePlaying:
        status->state = bplayback_state_playing;
        break;
    case B_PlaybackIpState_ePaused:
        status->state = bplayback_state_paused;
        break;
    case B_PlaybackIpState_eTrickMode:
        status->state = bplayback_state_trickmode;
        break;
    default:
        BDBG_WRN(("invalid state"));
        break;
    }

    status->num_read_errors = playbackIpStatus.numRecvTimeouts;

    status->fifo_depth = playbackStatus.fifoDepth;
    status->fifo_size = playbackStatus.fifoSize;
    status->bytes_played = playbackStatus.bytesPlayed;

    status->has_index = false;
    status->position.has_index = status->has_index = false;
    status->position.timestamp = playbackStatus.position;
    status->first_timestamp = playbackStatus.first;
    status->last_timestamp = playbackStatus.last;
    status->position.index_offset = playbackStatus.position;
    status->trickmode_params = playback_ip->playback.trick_mode_params;
    BDBG_MSG_FLOW(("playback position: first %u, last %u, current %u\n", playbackStatus.first, playbackStatus.last, playbackStatus.position));

#define SCALE_FACTOR 5000
    if (playback_ip->protocol == bsettop_ip_protocol_http) {
        if (playback_ip->socket_params.open_params.first_pts) {
            /* server has provided the PTS info, we use PTS to determine the current playback position */
            unsigned int first_pts, last_pts, current_pts;
            NEXUS_VideoDecoderStatus videoDecoderStatus;
            first_pts = playback_ip->socket_params.open_params.first_pts;
            last_pts = playback_ip->socket_params.open_params.first_pts + (playback_ip->socket_params.open_params.duration * 45);
            if (NEXUS_VideoDecoder_GetStatus(playback_ip->playback.stream->consumers.decode->video_decode->nVideoDecoder, &videoDecoderStatus) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_VideoDecoder_GetStatus() Failed\n", __FUNCTION__));
                return berr_external_error;
            }
            current_pts = videoDecoderStatus.pts;
            status->position.timestamp = ((current_pts - first_pts) / (last_pts - first_pts + 0.005)) * SCALE_FACTOR;
            BDBG_MSG_FLOW(("decoder pts position: first %x, last %x, current %x\n", first_pts, last_pts, current_pts));
        }
        else {
            /* we use the position provided by Nexus Playback */
            status->position.timestamp = (((playbackStatus.position - playbackStatus.first) /
                                        (float)(playbackStatus.last - playbackStatus.first + 0)) * SCALE_FACTOR);
        }
        status->last_timestamp = SCALE_FACTOR;
        status->first_timestamp = 0;
        status->position.has_index = status->has_index = true;

    } else if((playback_ip->protocol == bsettop_ip_protocol_rtp ) || (playback_ip->protocol == bsettop_ip_protocol_rtsp )) {
        if (playbackStatus.last)
            status->position.has_index = status->has_index = true;
    }

    return rc;
}

/*
Summary:
    Pause playback.
*/
bresult bplayback_ip_pause(
    bplayback_t playback
    )
{
#if defined(B_HAS_IP)
    B_PlaybackIpTrickModesSettings ipTrickmodeSettings;
    bresult rc = b_ok;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    BDBG_MSG(("Entered %s()", __FUNCTION__));

    if (set_nexus_handles(playback_ip) != b_ok) {
        BDBG_ERR(("Failed to set the nexus handles w/ IP library \n"));
        return berr_external_error;
    }

    memset(&ipTrickmodeSettings, 0, sizeof(ipTrickmodeSettings));
    if (playback_ip->playback_state == bplayback_ip_state_playing) {
        ipTrickmodeSettings.pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
        if (B_PlaybackIp_Pause(playback_ip->playback_ip_handle, &ipTrickmodeSettings) != B_ERROR_SUCCESS)
            return BSETTOP_ERROR(berr_external_error);
        playback_ip->playback_state = bplayback_ip_state_paused;
    }
    else {
        if (B_PlaybackIp_Play(playback_ip->playback_ip_handle) != B_ERROR_SUCCESS)
            return BSETTOP_ERROR(berr_external_error);
        playback_ip->playback_state = bplayback_ip_state_playing;
    }
    return rc;
#else
    BSTD_UNUSED(playback);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

/*
Summary:
    Play/Resume playback.
*/
bresult bplayback_ip_play(
    bplayback_t playback
    )
{
#if defined(B_HAS_IP)
    bresult rc = b_ok;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    BDBG_MSG(("Entered %s()", __FUNCTION__));

    if (set_nexus_handles(playback_ip) != b_ok) {
        BDBG_ERR(("Failed to set the nexus handles w/ IP library \n"));
        return berr_external_error;
    }

    if (B_PlaybackIp_Play(playback_ip->playback_ip_handle) != B_ERROR_SUCCESS)
        return BSETTOP_ERROR(berr_external_error);
    playback_ip->playback_state = bplayback_ip_state_playing;
    return rc;
#else
    BSTD_UNUSED(playback);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

/*
Summary:
    Trickmode playback.
*/
bresult bplayback_ip_trickmode(
    bplayback_t playback,
    const bplayback_trickmode_params *trickmode
    )
{
#if defined(B_HAS_IP)
    bresult rc = b_ok;
    B_PlaybackIpError err;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    B_PlaybackIpTrickModesSettings ipTrickModeSettings;
    BDBG_MSG(("Entered %s()", __FUNCTION__));


    if (set_nexus_handles(playback_ip) != b_ok) {
        BDBG_ERR(("Failed to set the nexus handles w/ IP library \n"));
        return berr_external_error;
    }

    if (B_PlaybackIp_GetTrickModeSettings(playback_ip->playback_ip_handle, &ipTrickModeSettings) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: Failed to get default IP TrickMode settings\n", __FUNCTION__));
        return BSETTOP_ERROR(berr_external_error);
    }
    playback_ip->playback.trick_mode_params = *trickmode;
    if (playback_ip->protocol == bsettop_ip_protocol_rtsp) {
        /* convert Broadcom trickmode speed to RTSP scale value */
        int scale_idx;
        int scale;
        scale_idx = trickmode->mode_modifier/100;
        if (scale_idx == 0) {
            BDBG_ERR(("Dont support slow-motion rtsp yet, rate %d", trickmode->mode_modifier));
            return b_ok;
        }
        
        if (scale_idx > 0) {
            scale_idx = playback_ip->firstPositiveScaleIndex + scale_idx -1;
            if (scale_idx >= playback_ip->scaleListEntries)
                scale_idx = playback_ip->scaleListEntries-1;
        }
        else {
            /* -ve idx */
            scale_idx = playback_ip->firstPositiveScaleIndex + scale_idx;
            if (scale_idx < 0)
                scale_idx = 0;
        }
        scale = (int)playback_ip->scaleList[scale_idx];
        ipTrickModeSettings.rate = scale;
        BDBG_MSG(("scale passed to Playback IP: %d, idx %d, scale %0.1f", scale, scale_idx, playback_ip->scaleList[scale_idx]));
    }
    else {
        ipTrickModeSettings.rate = trickmode->mode_modifier;
    }
    ipTrickModeSettings.frameRepeat = trickmode->slow_motion_rate;
    switch (trickmode->decode_mode) {
    case bplaypump_decode_mode_i:
        ipTrickModeSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
        break;
    case bplaypump_decode_mode_ip:
        ipTrickModeSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eIP;
        break;
    case bplaypump_decode_mode_all:
    default:
        ipTrickModeSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
        break;
    }
    err = B_PlaybackIp_TrickMode(playback_ip->playback_ip_handle, &ipTrickModeSettings);
    if (err != B_ERROR_SUCCESS)
        return BSETTOP_ERROR(berr_external_error);

    return rc;
#else
    BSTD_UNUSED(playback);
    BSTD_UNUSED(trickmode);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bplayback_ip_get_trickmode_by_rate(
    bplayback_t playback,
    int rate,
    bplayback_trickmode_params *trickmode
    )
{
#if defined(B_HAS_IP)
    bresult rc = b_ok;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    BDBG_MSG(("Entered %s: rate %d", __FUNCTION__, rate));

#if 0
    B_PlaybackIpError err;
    /* For now, commenting the GetTrickModeByRate call to IP library's until it is implemented */
    err = B_PlaybackIp_GetTrickModeByRate(playback_ip->playback_ip_handle, rate, (B_PlaybackIpTrickModesSettings *)trickmode);
    if (err != B_ERROR_SUCCESS)
        return BSETTOP_ERROR(berr_external_error);

    return rc;
#else
    /* legacy implementation */
    if ( playback_ip->protocol != bsettop_ip_protocol_http )
    {
        if (rate == BPLAYBACK_NORMALPLAY_SPEED || rate == 0) {
            /* normal play isn't a trick mode */
            return BSETTOP_ERROR(berr_invalid_parameter);
        }
        else {
            trickmode->mode = bplayback_player_mode_i; /* don't care */
            trickmode->mode_modifier = rate;
            trickmode->slow_motion_rate = 0; /* don't care */
            trickmode->decode_mode = bplaypump_decode_mode_all;
        }
        return rc;
    }
    bplayback_trickmode_params_init(trickmode);
    return rc;
#endif
#else
    BSTD_UNUSED(playback);
    BSTD_UNUSED(rate);
    BSTD_UNUSED(trickmode);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

long bplayback_ip_goto_index (
    bplayback_t playback, /* Handle returned by bplayback_ip_open */
    long offset,
    int whence
    )
{
#if defined(B_HAS_IP)
    bresult rc = b_ok;
    bplayback_ip_t playback_ip = (bplayback_ip_t) playback;
    BDBG_MSG(("Entered %s()", __FUNCTION__));
    BSTD_UNUSED(offset);
    BSTD_UNUSED(whence);
    BSTD_UNUSED(playback_ip);
    return rc;
#else
    BSTD_UNUSED(playback);
    BSTD_UNUSED(offset);
    BSTD_UNUSED(whence);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}
#endif /* LINUX  || vxworks */

/*
Summary:
    Open an IP playback channel.
*/
bplayback_t bplayback_ip_open(void)
{
#if defined(B_HAS_IP)
    return bplayback_ip_p_open();
#else
    /* Fail silently. This function is used to query if IP support is present. */
    return NULL;
#endif
}

/*
Summary:
    Close an IP playback channel.
*/
void bplayback_ip_close(bplayback_t playback)
{
#if defined(B_HAS_IP)
    BDBG_MSG(("%s: \n", __FUNCTION__));
    bplayback_ip_p_close(playback);
        return;
#else
    BSTD_UNUSED(playback);
    BSETTOP_ERROR(berr_not_supported);
    return;
#endif
}

/*
Summary:
    Required to initialize the bplayback_socket_params structure.
*/
void
bplayback_socket_params_init(
    bplayback_socket_params *params,
    bplayback_t playback
    )
{
#if defined(B_HAS_IP)
    bplayback_ip_source_params_init(params, playback);
#else
    BSTD_UNUSED(params);
    BSTD_UNUSED(playback);
#endif
}

/*
Summary:
    Open a IP playback socket to pass to bplayback_start().
Description:
    The bplayback_socket_t can be used for only one IP playback at a time.
*/
bplayback_file_t bplayback_socket_open(
    bplayback_t playback, /* Handle returned by bplayback_ip_open */
    const bplayback_socket_params *params
    )
{
#if defined(B_HAS_IP)
    return bplayback_ip_socket_open(playback, (bplayback_socket_params *) params);
#else
    BSTD_UNUSED(params);
    BSTD_UNUSED(playback);
    return NULL;
#endif
}

/*
Summary:
    Close a IP playback socket.
Description:
    Close a IP playback socket opened by bplayback_socket_open().
*/
void bplayback_socket_close(
    bplayback_t playback /* Handle returned by bplayback_ip_open */
    )
{
#if defined(B_HAS_IP)
    BDBG_MSG(("%s: \n", __FUNCTION__));
    bplayback_ip_socket_close(playback);
#else
    BSTD_UNUSED(playback);
#endif
}

