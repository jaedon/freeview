/***************************************************************************
*     (c)2003-2012 Broadcom Corporation
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
* $brcm_Workfile: b_playback_ip_rtsp.c $
* $brcm_Revision: 5 $
* $brcm_Date: 8/22/12 1:56p $
*
* Description: This file implements the RTSP Wrapper to the Live Media RTSP library.
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_rtsp.c $
* 
* 5   8/22/12 1:56p ssood
* SW7552-326: merging to mainline: reset event before its usage
* 
* SW7552-326/1   8/21/12 4:25p rzheng
*  SW7552-326: Video is not out for IP playback with RTSP mode
* 
* 4   4/16/10 5:02p ssood
* SW7420-561: Added support for Additional Philips RTSP features:
*  configurable keepalive timer, allow apps to send custom headers to
*  servers, generate EOF events to app upon RTSP heartbeat failures
* 
* 3   4/2/10 5:56p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 2   2/16/10 10:42p ssood
* SW7420-561: Changes to support basic RTSP w/o trickmodes
* 
* 1   2/16/10 12:11p ssood
* SW7420-561: Adding file for RTSP wrapper layer
* 
* 
***************************************************************************/
#if defined(LINUX) || defined(__vxworks)

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include "b_playback_ip_lm_helper.h"
#include <sys/ioctl.h>
#include <net/if.h>

BDBG_MODULE(b_playback_ip_rtsp);

extern B_PlaybackIpError B_PlaybackIp_RtpSessionOpen(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, B_PlaybackIpSessionOpenStatus *openStatus);
extern void B_PlaybackIp_RtpSessionClose(B_PlaybackIpHandle playback_ip);
extern B_PlaybackIpError B_PlaybackIp_RtpSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);
extern B_PlaybackIpError B_PlaybackIp_UdpSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);

static void 
rtsp_cmd_completion(void *appCtx, B_PlaybackIpEventIds eventId) 
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)appCtx;
    
    BDBG_ASSERT(playback_ip);
    BKNI_SetEvent(playback_ip->liveMediaSyncEvent);
    BDBG_MSG(("%s: sent LiveMediaSyncEvent for eventId %d\n", __FUNCTION__, eventId));
}

#define LIVEMEDIA_LIB_API_WAIT_TIME 2000
/* 
   Function does following:
   -sdf
   -sdf
*/
B_PlaybackIpError 
B_PlaybackIp_RtspSessionOpen(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionOpenSettings *openSettings,
	B_PlaybackIpSessionOpenStatus *openStatus /* [out] */
    )
{
    B_PlaybackIpSocketState *socketState;
    B_PlaybackIpError errorCode = B_ERROR_PROTO;

    if (!playback_ip || !openSettings || !openStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, openSettings %p, openStatus %p\n", __FUNCTION__, playback_ip, openSettings, openStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* if SessionSetup is in progress, return INCOMPLETE */
    if (playback_ip->apiInProgress) {
        return B_ERROR_IN_PROGRESS;
    }

    /* if SessionSetup is completed, return results to app */
    if (playback_ip->apiCompleted) {
        BDBG_WRN(("%s: previously started session setup operation completed, playback_ip %p\n", __FUNCTION__, playback_ip));
        goto done;
    }

    /* Neither SessionSetup is in progress nor it is completed, verify input params and then start work */
    if (openSettings->u.rtsp.additionalHeaders && !strstr(openSettings->u.rtsp.additionalHeaders, "\r\n")) {
        BDBG_ERR(("%s: additional RTSP header is NOT properly terminated (missing \\r\\n), header is %s\n", __FUNCTION__, openSettings->u.rtsp.additionalHeaders));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* input parameters are good, so update the api progress state */
    playback_ip->apiInProgress = true;
    playback_ip->apiCompleted = false;
    socketState = &openStatus->socketState;
    
    if (!openSettings->nonBlockingMode) {
        /* App wants to do a blocking call, so we will need to define our internal callback to wait for LiveMedia Session Completion */
        /* All RTSP related Live Media calls are non-blocking */
        playback_ip->openSettings.eventCallback = rtsp_cmd_completion;
        playback_ip->openSettings.appCtx = playback_ip;
    }

    /* We pass IP Channel info (IP, Port, RTSP URL) to the Live Media library */
    /* Library is responsible for setting up the initial socket information  & returns the socket fd back */
    BKNI_ResetEvent(playback_ip->liveMediaSyncEvent);
    errorCode = B_PlaybackIp_liveMediaSessionOpen(playback_ip, &playback_ip->openSettings, &playback_ip->lm_context, &socketState->fd);
    if (errorCode == B_ERROR_IN_PROGRESS) {
        BERR_Code rc;
        if (openSettings->nonBlockingMode)
            return B_ERROR_IN_PROGRESS;

        /* app is in the blocking mode, so wait for command completion */
        BDBG_MSG(("%s: waiting for for Live Media Session Open Command Completion event", __FUNCTION__));
        rc = BKNI_WaitForEvent(playback_ip->liveMediaSyncEvent, LIVEMEDIA_LIB_API_WAIT_TIME  /* msec */);
        if (rc == BERR_TIMEOUT) {
            BDBG_WRN(("%s: timed out for Live Media Session Open Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        } else if (rc!=0) {
            BDBG_WRN(("%s: Got error while waiting for Live Media Session Open Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        }

        /* session open completed successfully, continue below at done label */
        BDBG_MSG(("%s: Session Open Command completed: socket %d", __FUNCTION__, socketState->fd));
    }
    else if (errorCode != B_ERROR_SUCCESS) {
        goto error;
    }

done:
    /* RTSP command completed, verify response */
    openStatus->u.rtsp = playback_ip->openStatus.u.rtsp;    /* filled in by the liveMediaSessionOpen call */
    if (openStatus->u.rtsp.responseHeaders == NULL) {
        errorCode = B_ERROR_PROTO;
        goto error;
    }
    BDBG_MSG(("%s: successfully received the RTSP Response", __FUNCTION__));
    playback_ip->playback_state = B_PlaybackIpState_eOpened;

    /* Since we dont yet know the media transport protocol, as assume it to be RTP and do the RTP session setup */
    /* RTP session setup is the super set of both RTP & UDP cases, so we should be fine */
    errorCode = B_PlaybackIp_RtpSessionOpen(playback_ip, openSettings, openStatus);

error:
    playback_ip->apiInProgress = false;
    playback_ip->apiCompleted = false;
    if (errorCode != B_ERROR_SUCCESS) {
        B_PlaybackIp_RtpSessionClose(playback_ip);
    }
    return errorCode;
}

/* 
   Function does following:
   -sdf
   -sdf
*/
B_PlaybackIpError 
B_PlaybackIp_RtspSessionSetup(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionSetupSettings *setupSettings,
	B_PlaybackIpSessionSetupStatus *setupStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;

    if (!playback_ip || !setupSettings || !setupStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, setupSettings %p, setupStatus %p\n", __FUNCTION__, playback_ip, setupSettings, setupStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    if (setupSettings->u.rtsp.additionalHeaders && !strstr(setupSettings->u.rtsp.additionalHeaders, "\r\n")) {
        BDBG_ERR(("%s: additional RTSP header is NOT properly terminated (missing \\r\\n), header is: %s\n", __FUNCTION__, setupSettings->u.rtsp.additionalHeaders));
        errorCode = B_ERROR_INVALID_PARAMETER;
        goto error;
    }


    /* if SessionSetup is in progress, return INCOMPLETE */
    if (playback_ip->apiInProgress)
        return B_ERROR_IN_PROGRESS;

    /* if SessionSetup is completed, return results to app */
    if (playback_ip->apiCompleted) {
        BDBG_WRN(("%s: previously started session setup operation completed, playback_ip %p\n", __FUNCTION__, playback_ip));
        /* Note: since this api was run in a separate thread, we defer thread cleanup until the Ip_Start */
        /* as this call to read up the session status may be invoked in the context of this thread via the callback */
        goto done;
    }

    /* Neither SessionSetup is in progress nor it is completed, so start setup */
    playback_ip->apiInProgress = true;
    playback_ip->apiCompleted = false;
    memset(&playback_ip->setupStatus, 0, sizeof(playback_ip->setupStatus));

#ifdef TEST_NW_TIMEOUT
    BDBG_WRN((" ***** pull cable, sleeping for 5 sec ****"));
    sleep(5);
    BDBG_WRN((" ***** after sleeping for 5 sec ****"));
#endif

    BKNI_ResetEvent(playback_ip->liveMediaSyncEvent);
    errorCode = B_PlaybackIp_liveMediaSessionSetup(playback_ip->lm_context);
    if (errorCode == B_ERROR_IN_PROGRESS) {
        BERR_Code rc;
        if (playback_ip->openSettings.nonBlockingMode)
            return B_ERROR_IN_PROGRESS;

        /* app is in the blocking mode, so wait for command completion */
        rc = BKNI_WaitForEvent(playback_ip->liveMediaSyncEvent, LIVEMEDIA_LIB_API_WAIT_TIME /* msec */);
        if (rc == BERR_TIMEOUT) {
            BDBG_WRN(("%s: timed out for Live Media Session Setup Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        } else if (rc!=0) {
            BDBG_WRN(("%s: Got error while waiting for Live Media Session Setup Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        }

        /* session setup completed */
        BDBG_MSG(("%s: Live Media Session Setup Command Response\n %s", __FUNCTION__, playback_ip->setupStatus.u.rtsp.responseHeaders));
    }
    else if (errorCode != B_ERROR_SUCCESS) {
        goto error;
    }

done:
    setupStatus->u.rtsp = playback_ip->setupStatus.u.rtsp;
    if (setupStatus->u.rtsp.responseHeaders != NULL) {
        /* success */
        errorCode = B_ERROR_SUCCESS;
        /* get the optional scale list if provided by server */
        B_PlaybackIp_liveMediaGetScaleList(playback_ip->lm_context, &setupStatus->u.rtsp.scaleListEntries, &setupStatus->u.rtsp.scaleList[0]);
    }
    else {
        errorCode = B_ERROR_PROTO;
    }

error:
    playback_ip->apiInProgress = false;
    playback_ip->apiCompleted = false;
    return errorCode;
}

B_PlaybackIpError 
B_PlaybackIp_RtspSessionStart(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionStartSettings *startSettings,
	B_PlaybackIpSessionStartStatus *startStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;

    if (!playback_ip || !startSettings || !startStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, startSettings %p, startStatus %p\n", __FUNCTION__, playback_ip, startSettings, startStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* if SessionStart is in progress, return INCOMPLETE */
    if (playback_ip->apiInProgress)
        return B_ERROR_IN_PROGRESS;

    /* if SessionStart is completed, return results to app */
    if (playback_ip->apiCompleted) {
        BDBG_WRN(("%s: previously started session start operation completed, playback_ip %p\n", __FUNCTION__, playback_ip));
        /* Note: since this api was run in a separate thread, we defer thread cleanup until the Ip_Start */
        /* as this call to read up the session status may be invoked in the context of this thread via the callback */
        goto done;
    }

    BDBG_MSG(("%s: RTSP Media Transport Protocol: %s, position start %d, end %d, keepAliveInterval %d", 
                __FUNCTION__, startSettings->u.rtsp.mediaTransportProtocol == B_PlaybackIpProtocol_eUdp ? "UDP" : "RTP", 
                startSettings->u.rtsp.start,
                startSettings->u.rtsp.end,
                startSettings->u.rtsp.keepAliveInterval
                ));
    /* Neither SessionStart is in progress nor it is completed, so start session */
    playback_ip->apiInProgress = true;
    playback_ip->apiCompleted = false;
    memset(&playback_ip->startStatus, 0, sizeof(playback_ip->startStatus));

    BKNI_ResetEvent(playback_ip->liveMediaSyncEvent);
    errorCode = B_PlaybackIp_liveMediaSessionStart(playback_ip->lm_context);
    if (errorCode == B_ERROR_IN_PROGRESS) {
        BERR_Code rc;
        if (playback_ip->openSettings.nonBlockingMode)
            return B_ERROR_IN_PROGRESS;

        /* app is in the blocking mode, so wait for command completion */
        rc = BKNI_WaitForEvent(playback_ip->liveMediaSyncEvent, LIVEMEDIA_LIB_API_WAIT_TIME /* msec */);
        if (rc == BERR_TIMEOUT) {
            BDBG_WRN(("%s: timed out for Live Media Session Start Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        } else if (rc!=0) {
            BDBG_WRN(("%s: Got error while waiting for Live Media Session Start Command Completion event", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        }

        /* session Start completed */
        BDBG_MSG(("%s: Live Media Session Start Command Response\n %s", __FUNCTION__, playback_ip->startStatus.u.rtsp.responseHeaders));
    }
    else {
        if (errorCode != B_ERROR_SUCCESS)
            goto error;
    }

done:
    startStatus->u.rtsp = playback_ip->startStatus.u.rtsp;
    if (startStatus->u.rtsp.responseHeaders == NULL) {
        errorCode = B_ERROR_PROTO;
        goto error;
    }

    /* Now setup the media transport protocol for RTSP: either UDP or RTP */
    if (playback_ip->mediaTransportProtocol == B_PlaybackIpProtocol_eUdp)
        errorCode = B_PlaybackIp_UdpSessionStart(playback_ip, startSettings, startStatus);
    else
        errorCode = B_PlaybackIp_RtpSessionStart(playback_ip, startSettings, startStatus);

error:
    playback_ip->apiInProgress = false;
    playback_ip->apiCompleted = false;
    if (errorCode != B_ERROR_SUCCESS) {
        B_PlaybackIp_liveMediaSessionStop(playback_ip);
    }
    return errorCode;
}

#endif /* LINUX || VxWorks */
