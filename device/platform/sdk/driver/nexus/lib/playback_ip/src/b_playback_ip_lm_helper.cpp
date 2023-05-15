/***************************************************************************
*     (c)2008-2010 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: b_playback_ip_lm_helper.cpp $
* $brcm_Revision: 15 $
* $brcm_Date: 12/14/10 12:07p $
*
* Module Description:
*  Helper file for Live Media library 
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_lm_helper.cpp $
* 
* 15   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 14   4/16/10 6:05p ssood
* SW7420-561: dont send keepalives for non-RTSP sessions
* 
* 13   4/16/10 5:02p ssood
* SW7420-561: Added support for Additional Philips RTSP features:
*  configurable keepalive timer, allow apps to send custom headers to
*  servers, generate EOF events to app upon RTSP heartbeat failures
* 
* 12   4/2/10 5:55p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 11   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/3   2/18/10 12:43a ssood
* SW7420-561: added checks to validate IP state during IP API calls
* 
* SW7420-561/2   2/16/10 10:42p ssood
* SW7420-561: Changes to support basic RTSP w/o trickmodes
* 
* SW7420-561/1   2/16/10 10:11a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 10   3/20/09 3:11p lwhite
* PR53472: Keep-alive uses GET_PARAMETER
* 
* 9   12/2/08 12:58p lwhite
* PR49862: Fixed rtsp request
* 
* 8   10/22/08 6:10p lwhite
* PR42739: Corrected RTP stats size
* 
* 7   10/3/08 3:29p lwhite
* PR42739: Added RTSP timeline support
* 
* 6   8/18/08 11:05a ssood
* PR45431: Fixed RTP to work standard sockets
* 
* 5   8/14/08 6:11p lwhite
* PR42739: fixed DEBUG=n warnings
* 
* 4   8/1/08 3:40p lwhite
* PR42739: RTSP trickmode fix. Uncomment RTP hdr acquistion
* 
* 3   7/28/08 3:27p ssood
* PR42739: further changes for supporting example app for IP Applib
* 
* 2   7/22/08 10:13a ssood
* PR42739: fixing warnings
* 
* 1   7/21/08 4:17p ssood
* PR42739: Moving LiveMedia wrapper functionality part of IP Applib
* 
****************************************************************************/
/* liveMedia helper functions */
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>

/* Live Media header files */
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "RTCP.hh"
#include "liveMedia.hh"

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_lm_helper.h"
#include "blive_ext.h"
#include "b_playback_ip_lm_rtp_src.h"

BDBG_MODULE(b_playback_ip_lm_helper);

// open livemedia: sets up the scheduler to serialize access to Live Media library
void *
B_PlaybackIp_liveMediaOpen(void) 
{
    UsageEnvironment* lm_env;
#if BDBG_DEBUG_BUILD
    char const* const libName = "LIVE555 Streaming Media v";
    char const* const libVersionStr = LIVEMEDIA_LIBRARY_VERSION_STRING;
#endif

    if ((lm_env = blive_scheduler_open()) == NULL) {
            BDBG_ERR(("ERROR: failed to Open the Live Media Scheduler"));
            return NULL;
    }
    BDBG_MSG(("%s%s: lm_env %p", libName, libVersionStr, lm_env));
    return lm_env;
}

// close livemedia
void 
B_PlaybackIp_liveMediaClose(UsageEnvironment* lm_env)
{
    if (lm_env) {
        blive_scheduler_close();
        BDBG_MSG(("lm_env %p", lm_env));
    }
}

/////////////////////// livemedia session /////////////////////////////

#define B_LM_MAX_CNAME_LEN          100
#define B_LM_ESTIMATED_RTCP_BW      160         // in kbps; for RTCP b/w share
#define B_BITBAND_TIMESTAMP_FREQ    1000        // Bitband RTP timestamp frequency (1KHz)
#define B_MP2T_TIMESTAMP_FREQ       90000       // MP2T RTP timestamp frequency (90KHz)
#define B_LM_KEEPALIVE_USEC         15000000    // time between RTSP keep-alive messages
#define B_LM_PERIODIC_TASK_USEC     1000000     // time between execution of periodic task
#define B_LM_MAX_RTP_HEADERS        4000

// the session state
typedef enum lmSessionState {
    lmSessionStateNone,
    lmSessionStateOpened,
    lmSessionStateStarted,
    lmSessionStateClosing
} lmSessionState;

// the session
typedef struct B_PlaybackIp_liveMediaSession {
    RTPSource* source;
    Groupsock *rtpGroupsock;
    Groupsock *rtcpGroupsock;
    RTCPInstance* rtcpInstance;
    RTSPClient* rtspClient;
    MediaSession* rtspMediaSession;
    unsigned rtpHeaderDataCount;
    B_PlaybackIp_RtpStatsHeader *rtpHeaderData;
    B_PlaybackIpHandle playback_ip;
    B_PlaybackIpSessionOpenSettings openSettings;
    B_PlaybackIpError errorCode;
    char url[256];
#if 0
    B_PlaybackIpProtocol protocol;
#endif
    lmSessionState state;
    unsigned prevSeqNum;
    unsigned periodicTaskPendingCount;
    unsigned periodicTaskSchedCount;
    unsigned timestampFreq;
    bool gotFirstPacket;
    struct timeval presentationStartTime; 
    struct timeval presentationTime; 
    struct timeval NPT; 
    struct timeval endTime;
    float scaleList[MAX_SCALE_LIST_ENTRIES];
    int scaleListEntries;
    int keepAliveInterval; /* in microsec */
} B_PlaybackIp_liveMediaSession;

// the session context
typedef struct B_PlaybackIp_liveMediaSessionContext {       // pass into blive_scheduler_queue_delayed_task(), new/delete this with each unique session
    UsageEnvironment* lm_env;
    B_PlaybackIp_liveMediaSession *session;                 // pointer to the session
    void *params;                               // new/delete params on each scheduled "delayed task", or set to NULL
    blive_scheduler_task_wait_ctx_t waitCtx;    // used to wait/signal scheduler "delayed task" completion
} B_PlaybackIp_liveMediaSessionContext;

static unsigned char CNAME[B_LM_MAX_CNAME_LEN+1];

static void B_PlaybackIp_doSessionInit(void *context);
static void B_PlaybackIp_doSessionOpen(void *context);
static void B_PlaybackIp_doSessionSetup(void *context);
static void B_PlaybackIp_doSessionStart(void *context);
static void B_PlaybackIp_doSessionRTSPControl(void *context);
static void B_PlaybackIp_doSessionClose(void *context);
static void B_PlaybackIp_doPeriodicTask(void *context);

static void B_PlaybackIp_processReceptionStats(B_PlaybackIp_liveMediaSession *session);
static void sendKeepAlive(B_PlaybackIp_liveMediaSession *session);
static bool hasBitbandRSTPServer(B_PlaybackIp_liveMediaSession *session);

// open a livemedia RTP or RTSP session
B_PlaybackIpError 
B_PlaybackIp_liveMediaSessionOpen(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionOpenSettings *openSettings,
    void **context, 
    int *fd
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    B_PlaybackIp_liveMediaSessionContext *lmContext = NULL;

    lmContext = new B_PlaybackIp_liveMediaSessionContext;
    if (!lmContext) {
        BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
        errorCode = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }

    lmContext->lm_env = (UsageEnvironment *)B_PlaybackIp_liveMediaOpen();

    lmContext->session = new B_PlaybackIp_liveMediaSession;
    if (!lmContext->session) {
        BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
        errorCode = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    memset(lmContext->session, 0, sizeof(B_PlaybackIp_liveMediaSession));
    lmContext->session->playback_ip = playback_ip;
    lmContext->session->openSettings = *openSettings;
    BDBG_MSG(("created Live Media wrapper session ctx: lmContext %p", lmContext));

    // schedule initialization of session: create Groupsock for RTP, object for RTSP Client
    lmContext->params = NULL;
    blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionInit, lmContext);
    if (lmContext->session->errorCode != B_ERROR_SUCCESS) {
        errorCode = lmContext->session->errorCode; 
        goto error;
    }

    // wait for init to complete
    blive_scheduler_wait(&lmContext->waitCtx);

    // return the context and socket back to the caller
    *context = lmContext;
    *fd = lmContext->session->rtpGroupsock->socketNum();
#ifdef TODO
    /* this check is only valid for RTP as for RTSP socket is not setup during Open call */
    BDBG_ASSERT(*fd);
#endif

    if (openSettings->socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        // schedule open of session (don't wait for completion)
        /* For RTSP, we have sent OPTION command to the server and its result is not yet processed */
        blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionOpen, lmContext);
        return B_ERROR_IN_PROGRESS;
    }
    else {
        return B_ERROR_SUCCESS;
    }

error:
    *context = NULL;
    if (lmContext->session)
        delete lmContext->session;
    if (lmContext)
        delete lmContext;
    return errorCode;
}

// setup a livemedia RTP or RTSP session
B_PlaybackIpError 
B_PlaybackIp_liveMediaSessionSetup(
    void *context
    )
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;

    // schedule open of session (don't wait for completion)
    blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionSetup, lmContext);
    if (session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp)
        return B_ERROR_IN_PROGRESS;
    else
        return B_ERROR_SUCCESS;
}

// start a livemedia RTP or RTSP session
B_PlaybackIpError 
B_PlaybackIp_liveMediaSessionStart(
    void *context 
    ) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;

    // schedule start
    blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionStart, lmContext);

    if (session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp)
        return B_ERROR_IN_PROGRESS;
    else
        return B_ERROR_SUCCESS;
}

// control an RTSP session (eg FFW, REW, etc)
void B_PlaybackIp_liveMediaSessionRTSPControl(
    void *context, 
    B_PlaybackIpRtspCtrlCmd *cmd
    ) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    #if BDBG_DEBUG_BUILD
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    #endif

    BDBG_ASSERT(session);

    // create a copy of the params and manage its lifecycle
    lmContext->params = new B_PlaybackIpRtspCtrlCmd;
    memcpy(lmContext->params, cmd, sizeof(B_PlaybackIpRtspCtrlCmd));

    // schedule
    blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionRTSPControl, lmContext);
}

// close a livemedia RTP or RTSP session
void B_PlaybackIp_liveMediaSessionClose(
    void *context
    ) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    #if BDBG_DEBUG_BUILD
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    #endif

    BDBG_ASSERT(session);

    // shedule close
    blive_scheduler_queue_delayed_task(0, (TaskFunc*)B_PlaybackIp_doSessionClose, lmContext);

    // wait for close to complete
    blive_scheduler_wait(&lmContext->waitCtx);

    B_PlaybackIp_liveMediaClose(lmContext->lm_env);
}

#if 0
// get data from an RTP or RTSP session
void B_PlaybackIp_liveMediaSessionGetData(void *context, void *buffer, unsigned int *buffer_size) {
    // stub for future use
}
#endif

// stop a livemedia RTP or RTSP session
void B_PlaybackIp_liveMediaSessionStop(void *context) 
{
    BSTD_UNUSED(context);
}

void B_PlaybackIp_liveMediaSessionRange(void *context, unsigned long *current , unsigned long *end ) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;

    *current  = session->NPT.tv_sec*1000 + session->NPT.tv_usec/1000; /*milliseconds*/
    *end      = session->endTime.tv_sec*1000 + session->endTime.tv_usec/1000; /*milliseconds*/
}

static void 
B_PlaybackIp_doSessionInit(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpSessionOpenSettings *openSettings = &session->openSettings;
    struct in_addr sessionAddress;
    const unsigned char ttl = 1; // low, in case routers don't admin scope
    bool verbose = false;

    /* Defer this to a/f SETUP time when we know the protocol Media is being streamed on (Raw UDP or RTP) */
    // setup buffer for reception stats
    BDBG_MSG(("rtp header data size %d, %d\n", sizeof(B_PlaybackIp_RtpHeader) , B_MAX_RTP_HEADERS));
    session->rtpHeaderData = (B_PlaybackIp_RtpStatsHeader *)malloc(sizeof(B_PlaybackIp_RtpStatsHeader) * B_MAX_RTP_HEADERS);
    if (!session->rtpHeaderData) {
        BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
        session->errorCode = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    memset(session->rtpHeaderData, 0, sizeof(B_PlaybackIp_RtpHeader) * B_MAX_RTP_HEADERS);
    session->timestampFreq = B_MP2T_TIMESTAMP_FREQ;

    if (openSettings->socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        portNumBits portNum;
        Port srcPort(0);
        Groupsock *prevGroupsock = NULL;

        snprintf(session->url, sizeof(session->url), 
                "rtsp://"
                "%s:%d%s",
                openSettings->socketOpenSettings.ipAddr, openSettings->socketOpenSettings.port, openSettings->socketOpenSettings.url);
        BDBG_MSG(("Got URL: %s\n", session->url));

        if(getenv("rtsp_verbose")) {
            if((strcasecmp(getenv("rtsp_verbose"), "y"))==0)
                verbose = true;
        }

        // create RTSP Client object
        session->rtspClient = RTSPClient::createNew(*lmContext->lm_env, verbose, NULL, 0);
        if (!session->rtspClient) {
            BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
            session->errorCode = B_ERROR_OUT_OF_MEMORY;
            goto error;
        }
        if (openSettings->u.rtsp.userAgent)
            session->rtspClient->setUserAgentString(openSettings->u.rtsp.userAgent);
        else
            session->rtspClient->setUserAgentString("RTSP Client(C) 2001-2011, Broadcom Corporation");

        /* TODO: need to defer this GroupSock initialization work to after SessionSetup (a/f SETUP response is processed) */
        /* main reason is that we dont know in advance whether server is going to use a Raw UDP or RTP protocol for sending Media */
        for(;;) {
            // open RTP socket
            session->rtpGroupsock = new Groupsock(*lmContext->lm_env, sessionAddress, 0, ttl);
            if (!session->rtpGroupsock) {
                BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
                session->errorCode = B_ERROR_OUT_OF_MEMORY;
                goto error;
            }
            getSourcePort(*lmContext->lm_env, session->rtpGroupsock->socketNum(), srcPort);
            if(prevGroupsock !=  NULL) {
                delete prevGroupsock;
                prevGroupsock = NULL;
            }
            portNum = ntohs(srcPort.num());
            // use only even numbered ephemeral port
            if ((portNum&1) == 0) {
                break;
            }
            else {
                BDBG_WRN(("Rejecting non-even port number: %d", portNum));
                prevGroupsock = session->rtpGroupsock;  // need to keep this until we new another one
            }
        }
        // open RTCP socket
        session->rtcpGroupsock = new Groupsock(*lmContext->lm_env, sessionAddress, portNum+1, ttl);
        if (!session->rtcpGroupsock) {
            BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
            session->errorCode = B_ERROR_OUT_OF_MEMORY;
            goto error;
        }
    }
    else {
        // open RTP/RTCP sockets
        BDBG_MSG(("Got IP address: %s, port number: %d, protocol: %d", openSettings->socketOpenSettings.ipAddr, openSettings->socketOpenSettings.port, openSettings->socketOpenSettings.protocol));
        sessionAddress.s_addr = our_inet_addr(openSettings->socketOpenSettings.ipAddr);
        session->rtpGroupsock = new Groupsock(*lmContext->lm_env, sessionAddress, openSettings->socketOpenSettings.port, ttl);
        if (!session->rtpGroupsock) {
            BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
            session->errorCode = B_ERROR_OUT_OF_MEMORY;
            goto error;
        }
        session->rtcpGroupsock = new Groupsock(*lmContext->lm_env, sessionAddress, openSettings->socketOpenSettings.port+1, ttl);
        if (!session->rtcpGroupsock) {
            BDBG_ERR(("%s: Memory allocation failure\n", __FUNCTION__));
            session->errorCode = B_ERROR_OUT_OF_MEMORY;
            goto error;
        }
    }

    // signal caller that init is complete
    blive_scheduler_signal(&lmContext->waitCtx);
    session->errorCode = B_ERROR_SUCCESS;
    return;

error:
    if (session->rtspClient)
        Medium::close(session->rtspClient);
    if (session->rtpGroupsock)
        delete session->rtpGroupsock;
    if (session->rtcpGroupsock)
        delete session->rtcpGroupsock;
    return;
}

// open a livemedia RTP or RTSP session
static void B_PlaybackIp_doSessionOpen(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpHandle playback_ip = session->playback_ip;
    char *tmpPtr;

    if (session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        // do OPTIONS
        BDBG_MSG(("%s: OPTIONS", __FUNCTION__));
        char* optionsResponse = session->rtspClient->sendOptionsCmd(session->url, /*username*/ NULL, /*password*/ NULL);
        if(optionsResponse == NULL) {
            BDBG_ERR(("Error: Failed to get OPTIONS response"));
            playback_ip->openStatus.u.rtsp.responseHeaders = NULL;
        }
        else {
            BDBG_MSG(("OPTIONS response (Public Header) :\n %s", optionsResponse));
            if(session->rtspClient->responseMessage() != NULL) {
                char *responseHeaders;
                responseHeaders = playback_ip->openStatus.u.rtsp.responseHeaders = session->rtspClient->responseMessage();
                BDBG_MSG(("Complete response header:\n %s", responseHeaders));
                /* TODO: Accept-Scale: -24,-16,-8,-4,-2,1,2,4,8,16,24 */
                if ( (tmpPtr = strcasestr(responseHeaders, "Accept-Scale:")) ) {
                    char *tmpPtr2, *tmpPtr3;
                    char tmpChar;
                    session->scaleListEntries = 0;
                    tmpPtr += strlen("Accept-Scale:");
                    if ((tmpPtr2 = strstr(tmpPtr, "\r\n")) == NULL && (tmpPtr2 = strstr(tmpPtr, "\n")) == NULL) {
                        BDBG_WRN(("%s: Accept-Scale record is not correctly terminated (missing CR NL or NL)", __FUNCTION__));
                        session->scaleListEntries = 0;
                    }
                    else {
                        char *scaleList = tmpPtr;
                        tmpChar = *tmpPtr2;
                        *tmpPtr2 = '\0';
                        session->scaleListEntries = 0;
                        while ((tmpPtr3 = strstr(scaleList, ",")) != NULL) {
                            *tmpPtr3 = '\0';
                            session->scaleList[session->scaleListEntries++] = strtof(scaleList, (char **)NULL);
                            *tmpPtr3 = ',';
                            scaleList = tmpPtr3+1;
                            if (session->scaleListEntries >= MAX_SCALE_LIST_ENTRIES) {
                                BDBG_WRN(("%s: RTSP server sent more scale values that the scale list entries %d", __FUNCTION__, MAX_SCALE_LIST_ENTRIES));
                                break;
                            }
                            BDBG_MSG(("scale list[%d] = %0.1f", session->scaleListEntries-1, session->scaleList[session->scaleListEntries-1]));
                        }
                        if (scaleList != '\0') {
                            /* last entry not followed by the separator ',' */
                            session->scaleList[session->scaleListEntries++] = strtof(scaleList, (char **)NULL);
                            BDBG_MSG(("scale list[%d] = %0.1f", session->scaleListEntries-1, session->scaleList[session->scaleListEntries-1]));
                        }
                        *tmpPtr2 = tmpChar;
                    }
                }
                else {
                    session->scaleListEntries = 0;
                }
            }
            delete[] optionsResponse;
        }
        playback_ip->apiCompleted = true;
        playback_ip->apiInProgress = false;
        if (playback_ip->openSettings.eventCallback)
            playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eSessionOpenDone);
    }
}

void
B_PlaybackIp_liveMediaGetScaleList(void *context, int *scaleListEntries, float *scaleList)
{
    int i;
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;

    for (i=0; i<session->scaleListEntries; i++)
        scaleList[i] = session->scaleList[i];
    *scaleListEntries = session->scaleListEntries;
}

// open a livemedia RTP or RTSP session
static void B_PlaybackIp_doSessionSetup(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpHandle playback_ip = session->playback_ip;
    char *tmpPtr;

#if 0
    printf("@@@ Entered B_PlaybackIp_doSessionSetup() @ 0x%x, session: 0x%x\n", B_PlaybackIp_doSessionSetup, (unsigned)session);
#endif

    if(session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        // do OPTIONS
        BDBG_MSG(("OPTIONS"));
        char* optionsResponse = session->rtspClient->sendOptionsCmd(session->url, /*username*/ NULL, /*password*/ NULL);
        if(optionsResponse == NULL) {
            BDBG_ERR(("Error: Failed to get OPTIONS response"));
            goto error;
        }
        else {
            BDBG_MSG(("OPTIONS response:\n %s", optionsResponse));
            delete[] optionsResponse;
        }
        // do DESCRIBE
        BDBG_MSG(("DESCRIBE"));
        char* sdpDescription = session->rtspClient->describeURL(session->url);
        if (sdpDescription == NULL) {
            BDBG_ERR(("Error: Failed to get a SDP description from URL"));
            goto error;
        }
    
        BDBG_MSG(("Opened URL: %s", session->url));
        BDBG_MSG(("Got SDP description:"));
        BDBG_MSG(("%s", sdpDescription));
    
        /* determine the media transport: UDP or RTP */
        tmpPtr = strstr(sdpDescription, "m=");
        if (tmpPtr) {
            char *tmpPtr2;
            if ( (tmpPtr2 = strstr(tmpPtr, "\r\n")) == NULL && (tmpPtr2 = strstr(tmpPtr, "\n")) == NULL) {
                BDBG_WRN(("%s: SDP m record is not correctly terminated (missing CR NL or NL, assuming RTP as transport protocol", __FUNCTION__));
                playback_ip->mediaTransportProtocol = B_PlaybackIpProtocol_eRtp;
            }
            else {
                char tmp;
                tmp = *tmpPtr2;
                *tmpPtr2 = '\0';
                /* now look for RTP keyword, if found, transport is RTP else UDP */
                if (strstr(tmpPtr, "RTP")) {
                    playback_ip->mediaTransportProtocol = B_PlaybackIpProtocol_eRtp;
                    BDBG_MSG(("%s: media transport protocol is RTP", __FUNCTION__));
                }
                else if (strstr(tmpPtr, "UDP")) {
                    playback_ip->mediaTransportProtocol = B_PlaybackIpProtocol_eUdp;
                    BDBG_MSG(("%s: media transport protocol is UDP", __FUNCTION__));
                }
                else {
                    BDBG_ERR(("%s: media transport defined in the m= description is not supported, m record is %s", 
                                __FUNCTION__, tmpPtr));
                    goto error;
                }
                *tmpPtr2 = tmp;
            }
        }
        else {
            /* media transport not defined, assume default */
            BDBG_MSG(("%s: media transport record m= not defined, assume media transport default of RTP", __FUNCTION__));
            playback_ip->mediaTransportProtocol = B_PlaybackIpProtocol_eRtp;
        }

        // Create a media session object from the SDP description
        BDBG_MSG(("Creating the media session object from the SDP description..."));
        session->rtspMediaSession = MediaSession::createNew(*lmContext->lm_env, sdpDescription);
        delete[] sdpDescription;
        if (session->rtspMediaSession == NULL) {
            BDBG_ERR(("Error: Failed to create a MediaSession object from the SDP description"));
            goto error;
        } else if (!session->rtspMediaSession->hasSubsessions()) {
            BDBG_ERR(("Error: This RTSP session has no media subsessions"));
            goto error;
        }
    
        // do SETUP
        BDBG_MSG(("SETUP"));
        MediaSubsessionIterator iter(*session->rtspMediaSession);
        MediaSubsession *subsession = iter.next();

        // set ephemeral port number
        Port srcPort(0);
        portNumBits portNum = 0;
        getSourcePort(*lmContext->lm_env, session->rtpGroupsock->socketNum(), srcPort);
        if (srcPort.num() != 0) 
            portNum = ntohs(srcPort.num());
        else
            BDBG_ERR(("Error: error getting ephemeral port number"));

        subsession->setClientPortNum(portNum);
        if (!session->rtspClient->setupMediaSubsession(*subsession)) {
            BDBG_ERR(("Error: Failed to  setup sub-session"));
        } else {
            BDBG_MSG(("Successfully setup %s, %s, on port: %d",
                      subsession->mediumName(),
                      subsession->codecName(),
                      subsession->clientPortNum()));
            if (session->rtspClient->responseMessage() != NULL) {
                playback_ip->setupStatus.u.rtsp.responseHeaders = session->rtspClient->responseMessage();
                BDBG_MSG(("Complete SETUP response header:\n %s", playback_ip->setupStatus.u.rtsp.responseHeaders));
                playback_ip->apiCompleted = true;
                playback_ip->apiInProgress = false;
                /* TODO: get responseHeaders & statusCode from LM library */
                if (playback_ip->openSettings.eventCallback)
                    playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eSessionSetupDone);
            }
        }

        // setup destination for RTCP messeages
        Port destPort(subsession->serverPortNum+1);
        unsigned destAddress = subsession->connectionEndpointAddress();
        struct in_addr destAddr; destAddr.s_addr = destAddress;
        BDBG_MSG(("connectionEndpointAddress: %s", inet_ntoa(destAddr)));
        session->rtcpGroupsock->changeDestinationParameters(destAddr, destPort, /*destTTL*/ 1);

        // bitband server has proprietary use of timestamp
        if(hasBitbandRSTPServer(session)) session->timestampFreq = B_BITBAND_TIMESTAMP_FREQ;

        /* Get End Time and convert to timeval structure */
        session->endTime.tv_sec = session->rtspMediaSession->playEndTime();
        session->endTime.tv_usec = session->rtspMediaSession->playEndTime() *1000;
        session->endTime.tv_usec %=1000;
        session->endTime.tv_usec *=1000;
    }

    // create the data source: a "MPEG Video RTP source"
    session->source = MPEG2TransportStreamRTPSource::createNew(*lmContext->lm_env, session->rtpGroupsock, session->timestampFreq);

    // create (and start) a 'RTCP instance' for the RTP source:
    gethostname((char*)CNAME, B_LM_MAX_CNAME_LEN);
    CNAME[B_LM_MAX_CNAME_LEN] = '\0'; // just in case

    // Note: This starts RTCP running automatically
    session->rtcpInstance = RTCPInstance::createNew(*lmContext->lm_env, session->rtcpGroupsock,
                                                    B_LM_ESTIMATED_RTCP_BW, CNAME,
                                                    NULL /* we're a client */, 
                                                    session->source);

    session->state = lmSessionStateOpened;
    return;
error:
    playback_ip->setupStatus.u.rtsp.responseHeaders = NULL;
    playback_ip->apiCompleted = true;
    playback_ip->apiInProgress = false;
    if (playback_ip->openSettings.eventCallback) {
        playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eSessionSetupDone);
    }
    /* TODO: may need to also send some event to server to indicate session setup failure */

}

static void 
B_PlaybackIp_doSessionStart(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpHandle playback_ip = session->playback_ip;

#if 0
    printf("@@@ Entered B_PlaybackIp_doSessionStart() @ 0x%x, session: 0x%x\n", B_PlaybackIp_doSessionStart, (unsigned)session);
#endif

    if(session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        if (playback_ip->setupSettings.u.rtsp.additionalHeaders)
            session->rtspClient->setAdditionalHeaders(playback_ip->setupSettings.u.rtsp.additionalHeaders);
        // do PLAY
        BDBG_MSG(("PLAY"));
        if(!session->rtspClient->playMediaSession(*session->rtspMediaSession)) {
            // this can happen in response to incompatible server response, which is not a failure mode
            BDBG_MSG(("RTSP PLAY request returned bad value"));
        }
        else {
            BDBG_MSG(("RTSP PLAY request succeeded!!"));
        }
        if (playback_ip->startSettings.u.rtsp.keepAliveInterval)
            session->keepAliveInterval = playback_ip->startSettings.u.rtsp.keepAliveInterval * 1000000;
        else
            session->keepAliveInterval = B_LM_KEEPALIVE_USEC;
        BDBG_MSG(("keep alive interval %d", session->keepAliveInterval));
        if (session->rtspClient->responseMessage() != NULL) {
            playback_ip->startStatus.u.rtsp.responseHeaders = session->rtspClient->responseMessage();
            BDBG_MSG(("Play response header:\n %s", playback_ip->startStatus.u.rtsp.responseHeaders));
            playback_ip->apiCompleted = true;
            playback_ip->apiInProgress = false;
            /* TODO: get statusCode from LM library */
            if (playback_ip->openSettings.eventCallback)
                playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eSessionStartDone);
        }
    }

    if(session->state == lmSessionStateOpened) {
        session->periodicTaskPendingCount++;
        BDBG_MSG(("queuing B_PlaybackIp_doPeriodicTask() in B_PlaybackIp_doSessionStart()\n"));
        blive_scheduler_queue_delayed_task(B_LM_PERIODIC_TASK_USEC, (TaskFunc*)B_PlaybackIp_doPeriodicTask, lmContext);
        session->state = lmSessionStateStarted;
    }
}

static void B_PlaybackIp_doSessionRTSPControl(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpRtspCtrlCmd *cmd = (B_PlaybackIpRtspCtrlCmd *)lmContext->params;

#if 0
    printf("@@@ Entered B_PlaybackIp_doSessionRTSPControl() @ 0x%x, session: 0x%x\n", B_PlaybackIp_doSessionRTSPControl, (unsigned)session);
    printf("cmd type: %d, params.scale: %d, params.start: %d, params.end: %d\n", 
           cmd->type, 
           cmd->params.scale, 
           cmd->params.start, 
           cmd->params.end);
#endif

    switch(cmd->type) {
    case B_PlaybackIp_RtspCtrlCmdType_Play:
        // do PLAY
        BDBG_MSG(("PLAY (trick-mode)"));
        if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
                                                  (1.0*session->NPT.tv_sec)+(1.0*session->NPT.tv_usec/1000000.0),
                                                  cmd->params.end,
                                                  cmd->params.scale)) 
        {
            BDBG_ERR(("Error: Failed to PLAY (trick-mode)"));
        }
        break;
    case B_PlaybackIp_RtspCtrlCmdType_Pause:
        // do PAUSE
        BDBG_MSG(("PAUSE"));
        if(!session->rtspClient->pauseMediaSession(*session->rtspMediaSession)) {
            BDBG_ERR(("Error: Failed to PAUSE"));
        }
        break;
    case B_PlaybackIp_RtspCtrlCmdType_Resume:
        // do PLAY
        BDBG_MSG(("PLAY (resume)"));
        if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
                                                  /*start*/ -1,
                                                  cmd->params.end,
                                                  cmd->params.scale)) {
            BDBG_ERR(("Error: Failed to PLAY (trick-mode)"));
        }
        break;
    default:
        BDBG_ERR(("Error: unknown RTSP control command"));
        break;
    }

    // delete the command params, we're managing its lifecycle in this module
    delete cmd;
}

static void B_PlaybackIp_doSessionClose(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;

#if 0
    printf("@@@ Entered B_PlaybackIp_doSessionClose() @ 0x%x, session: 0x%x\n", B_PlaybackIp_doSessionClose, (unsigned)session);
#endif

    if(session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        // do TEARDOWN
        if(!session->rtspClient->teardownMediaSession(*session->rtspMediaSession)) {
            BDBG_ERR(("Error: Failed to TEARDOWN media session"));
        }
        Medium::close(session->rtspMediaSession);
        Medium::close(session->rtspClient);
        session->rtspClient = NULL;
    }

    Medium::close(session->rtcpInstance); // Note: Sends a RTCP BYE
    Medium::close(session->source);

    delete session->rtpGroupsock;
    delete session->rtcpGroupsock;

    free(session->rtpHeaderData);

    // defer session deletion to B_PlaybackIp_doPeriodicTask()
    session->state = lmSessionStateClosing;

    // signal caller that close is complete
    blive_scheduler_signal(&lmContext->waitCtx);
}

// execute/reschedule periodic tasks
static void B_PlaybackIp_doPeriodicTask(void *context) 
{
    B_PlaybackIp_liveMediaSessionContext *lmContext = (B_PlaybackIp_liveMediaSessionContext *)context;
    B_PlaybackIp_liveMediaSession *session = lmContext->session;
    B_PlaybackIpHandle playback_ip = session->playback_ip;

    BDBG_ASSERT(session);

    BDBG_MSG(("Entered B_PlaybackIp_doPeriodicTask() @ 0x%x, session: 0x%x, session->state: %d, sched_count: %d", 
              B_PlaybackIp_doPeriodicTask, (unsigned)session, session->state, session->periodicTaskSchedCount));

    session->periodicTaskSchedCount++;
    if(session->periodicTaskPendingCount > 0) session->periodicTaskPendingCount--;

    if (session->state == lmSessionStateStarted && playback_ip->playback_state >= B_PlaybackIpState_ePlaying) {
        BDBG_ASSERT(session->source);
        if (session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp && ((session->periodicTaskSchedCount%(session->keepAliveInterval/B_LM_PERIODIC_TASK_USEC))==0))
            sendKeepAlive(session);
        B_PlaybackIp_processReceptionStats(session);

        // avoid reschedule if scheduler falls behind
        if(session->periodicTaskPendingCount == 0) {
            session->periodicTaskPendingCount++;
            // reschedule this function
            blive_scheduler_queue_delayed_task(B_LM_PERIODIC_TASK_USEC, (TaskFunc*)B_PlaybackIp_doPeriodicTask, lmContext);
        }
    }
    else if(session->state == lmSessionStateClosing) {
        // we're done
        delete session;
        delete lmContext;
    }
}

// process RTP reception statistics
static void B_PlaybackIp_processReceptionStats(
    B_PlaybackIp_liveMediaSession *session
    ) 
{
    Boolean hasBeenSyncedUsingRTCP = false;

    session->rtpHeaderDataCount= 0;

    B_PlaybackIp_GetRtpHeaderData(
            session->playback_ip, 
            B_MAX_RTP_HEADERS * sizeof(B_PlaybackIp_RtpStatsHeader), 
            session->rtpHeaderData, 
            &session->rtpHeaderDataCount);

    if(session->rtpHeaderDataCount!=0) {
        BDBG_MSG(("entry_cnt: %d, first entry seq_num: %u, ssrc:%u, timestamp: %u", 
                  session->rtpHeaderDataCount, session->rtpHeaderData[0].sequence_num, session->rtpHeaderData[0].ssrc, 
                  session->rtpHeaderData[0].timestamp));

        // process each header via the reception stats database
        for(unsigned i=0; i<session->rtpHeaderDataCount; i++) {
            if(session->prevSeqNum+1 != session->rtpHeaderData[i].sequence_num && session->prevSeqNum != 65535) {
                BDBG_WRN(("break in RTP sequence number, prevSeqNum: %d, seq_num: %d", 
                          session->prevSeqNum,
                          session->rtpHeaderData[i].sequence_num));
            }
            session->prevSeqNum = session->rtpHeaderData[i].sequence_num;
            session->source->receptionStatsDB().noteIncomingPacket(session->rtpHeaderData[i].ssrc, 
                                                                   session->rtpHeaderData[i].sequence_num, 
                                                                   session->rtpHeaderData[i].timestamp,
                                                                   session->timestampFreq,
                                                                   /*True*/ False,
                                                                   session->presentationTime,
                                                                   hasBeenSyncedUsingRTCP,
                                                                   RTP_FIXED_HEADER_SIZE+RTP_PAYLOAD_SIZE);
            if(!session->gotFirstPacket && i==0) {
                session->gotFirstPacket = true;
                session->presentationStartTime = session->presentationTime;
            }
        }
    }

    if(session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        if(session->rtpHeaderDataCount) {
            // update our Normal Play Time
            if(hasBitbandRSTPServer(session)) {
                session->NPT.tv_sec = session->rtpHeaderData[session->rtpHeaderDataCount-1].timestamp/session->timestampFreq;
                session->NPT.tv_usec = session->rtpHeaderData[session->rtpHeaderDataCount-1].timestamp%session->timestampFreq;
            }
            else {
                timersub(&session->presentationTime, &session->presentationStartTime, &session->NPT);
            }
        }
        BDBG_MSG(("NPT: %ld.%ld, hasBeenSyncedUsingRTCP: %d", session->NPT.tv_sec, session->NPT.tv_usec, hasBeenSyncedUsingRTCP));
    }
}

// send keep-alive message to RTSP server
static void sendKeepAlive(B_PlaybackIp_liveMediaSession *session) 
{
    char *unusedParameterValueBufferPtr = NULL;

    if(session->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
        if(hasBitbandRSTPServer(session)) {
            // bitband specific keep-alive (needed for pre bitband ver. 5.1 builds)
            session->rtspClient->setMediaSessionParameter(*session->rtspMediaSession, /*parameter*/"Speed", /*value*/"1.0");
        }
        else {
#if 0
            // a widely supported form of keep-alive
            if(!session->rtspClient->playMediaSession(*session->rtspMediaSession, 
                                                      /*start*/ -1,
                                                      /*end*/ -1,
                                                      1.0)) {
                BDBG_ERR(("Error: Failed to PLAY (keep-alive)"));
            }
#else
            // Keep session alive with GET_PARAMETER
            if(!session->rtspClient->getMediaSessionParameter(*session->rtspMediaSession, 
                                                     NULL, 
                                                      unusedParameterValueBufferPtr)) {
                BDBG_ERR(("Error: Failed to GET_PARAMETER (keep-alive)"));
                if (session->playback_ip->openSettings.eventCallback)
                    session->playback_ip->openSettings.eventCallback(session->playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eServerEndofStreamReached);
                }
#endif
        }
    }
}

// determine if the session is using a Bitband RTSP server
static bool hasBitbandRSTPServer(B_PlaybackIp_liveMediaSession *session) 
{
    if(session->rtspClient->serverType() != NULL) {
        if(strcmp(session->rtspClient->serverType(), "BitBand-Vision")==0 ||
           // TODO: remove "VideoServer" workaround type in preference of acquiring timestamp frequency from SDP
           strcmp(session->rtspClient->serverType(), "VideoServer")==0) {
            return true;
        }
    }
    return false;
}

#if 0
#define PAYLOAD_TYPE_MP2T   33
MPEG2TransportStreamRTPSource*
MPEG2TransportStreamRTPSource::createNew(UsageEnvironment& env, 
                                         Groupsock* RTPgs,
                                         unsigned rtpTimestampFrequency) {
    return new MPEG2TransportStreamRTPSource(env, RTPgs, rtpTimestampFrequency);
}

MPEG2TransportStreamRTPSource
::MPEG2TransportStreamRTPSource(UsageEnvironment& env, 
                                Groupsock* RTPgs,
                                unsigned rtpTimestampFrequency)
    : RTPSource(env, RTPgs, PAYLOAD_TYPE_MP2T, rtpTimestampFrequency) {
}

MPEG2TransportStreamRTPSource::~MPEG2TransportStreamRTPSource() {
    fRTPInterface.stopNetworkReading();
}

char const* MPEG2TransportStreamRTPSource::MIMEtype() const {
    return "video/MP2T";
}
#endif

