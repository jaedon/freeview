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
* $brcm_Workfile: b_playback_ip_udp.c $
* $brcm_Revision: 23 $
* $brcm_Date: 2/9/12 3:31p $
*
* Description: IP Applib Implementation for IP_UDP Protocols in which 
*              UDP payload contains MPEG2 TS packets.
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_udp.c $
* 
* 23   2/9/12 3:31p ssood
* SW7425-2319: Add support for IP Tuner Lock/Unlock mechanism
* 
* 22   5/4/11 10:49p ssood
* SW7425-513: get basic RTP recepition to work w/o LIVEMEDIA_SUPPORT
* 
* 21   8/6/10 11:09a mward
* SW7125-253:  Silence a strict-aliasing warning.
* 
* 7408_phase1.0/1   6/28/10 3:31p gmohile
* SW7408-1 : Fix warnings for phase 1.0 release
* 
* 20   8/5/10 8:00p spothana
* SW7420-929: Fixing a compilation error.
* 
* 19   8/5/10 2:48p jjordan
* SW7405-4205: add socket flush to facilitate accurate targetting of TTS
*  buffer fullness at start of decode
* 
* 18   3/16/10 3:09p ssood
* SW7420-559: carry forward the EROUTER related changes for WAN unicast
*  support
* 
* 17   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/1   2/16/10 10:10a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 16   1/20/10 4:13p ssood
* SW7420-454: more changes to compress the verbose output
* 
* 15   1/19/10 4:55p ssood
* SW7420-454: convert verbose output to be available via msg modules
*  flags
* 
* 14   10/8/09 1:21p ssood
* SW7420-340: added fd in the debug print
* 
* 13   6/1/09 9:48a ssood
* PR48500: use cpu to copy data from kernel skbs to user buffer in
*  read/recvmsg to avoid M2M synchronization issues w/ bcrypt encryption
*  module
* 
* 13   6/1/09 9:44a ssood
* PR48500: use cpu to copy data from kernel skbs to user buffer in
*  read/recvmsg to avoid M2M synchronization issues w/ bcrypt encryption
*  module
* 
* 12   5/22/09 1:56p lwhite
* PR55410: Playpump/decoders checked before processing
* 
* 11   5/8/09 4:36p jjordan
* PR54886: fix TTS initial buffer depth
* 
* 10   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
* 
* 9   2/3/09 3:16p jjordan
* PR49937: add TTS throttle timebase setting
* 
* 8   12/4/08 10:13a jjordan
* PR49937: Add support for RTP over TTS
* 
* 7   10/31/08 12:30p jjordan
* PR47230: Add TTS Pacing
* 
* 6   10/24/08 3:28p lwhite
* PR47685: Coverity Fix, initializer
* 
* 5   10/23/08 4:39p lwhite
* PR47685: Coverity fix
* 
* 4   10/13/08 5:31p ssood
* PR47521: code cleanup due to api changes
* 
* 3   9/3/08 12:14p ssood
* PR45431: fixed compilation errors in non-debug mode
* 
* 2   8/27/08 11:16a ssood
* PR46200: comment out the debug message to report variable UDP payload
*  size
* 
* 
***************************************************************************/

 /**
 * Note this file is currently only compiled for LINUX platforms. Even though it
 * has VxWorks & WinCE related hash defines, the code needs more porting 
 * work for these OS platforms. 
 * If the feature isn't compiled in, then the only functions in this .c file
 * should be the public stubs. 
 **/
#if defined(LINUX) || defined(__vxworks)

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include <sys/ioctl.h>
#include <net/if.h>

BDBG_MODULE(b_playback_ip_udp);

/**
* Plain UDP Processing module of Playback IP Applib:
* Since UDP payload is the AV TS data, we read PKTS_PER_CHUNK from socket directly 
*   into the playpump buffer and feed it to PB. 
*/                       
void B_PlaybackIp_UdpProcessing(
    void * data
    )
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    B_PlaybackIpError rc;
    bool recvTimeout = false;
    bool flushPipeline = false;
    int totalBytesRecv;
#ifdef B_HAS_NETACCEL
    STRM_SockRecvParams_t sockRecvParams = STRM_SOCK_RECV_PARAMS_UDP_DEFAULT;
#endif

    BDBG_MSG(("Entered %s(): playback_ip %p, socket fd %d\n", __FUNCTION__, playback_ip, playback_ip->socketState.fd));

    /* check if playpump & decoder are setup */
    if (B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(playback_ip))
        goto error;

#ifdef B_HAS_NETACCEL
    sockRecvParams.pktOffset = 0;
    sockRecvParams.hdrOffset = sizeof(struct udp_hdr);
    /* PR48500: Sumitomo would need to encrypt the live IP stream while recording. */ 
    /* This use of M2M DMA engine for encryption purpose requires synchronization of DMA engine */
    /* also being used by the netaccel driver to user copies purposes. However, this synchronization */
    /* is really hard to achieve if the encryption module (PI) is in user space & netaccel driver in the kernel. */
    /* Alternative solution is to not use M2M engine for user copies in netaccel. This doesn't impose any significant */
    /* overhead on MIPS as BMIPS4380 onward, cpus have enough horse power to carry out the copy function. */
    sockRecvParams.useCpuCopy = true;
    if(playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
        sockRecvParams.pktsPerRecv = 200;
        sockRecvParams.recvTimeout = 10;
    }
    else {
        sockRecvParams.recvTimeout = 35;
        sockRecvParams.pktsPerRecv = PKTS_PER_READ;
    }
    if (setsockopt(playback_ip->socketState.fd, SOCK_BRCM_DGRAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
    {
        BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        /* in case of failure (shouldn't happen), read 1 pkt at a time */
    }
    BDBG_ERR(("%s: Modified the default pkts per recvfrom to %d, payload offset %d\n", 
                __FUNCTION__, PKTS_PER_READ, sockRecvParams.pktOffset));
#endif

    if(playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
        B_PlaybackIp_TtsThrottle_Params throttleParams;
        NEXUS_PlaypumpSettings playpumpSettings;

        playback_ip->ttsThrottle = B_PlaybackIp_TtsThrottle_Open();

        B_PlaybackIp_TtsThrottle_GetSettings(playback_ip->ttsThrottle, &throttleParams);
        BKNI_Memcpy(&throttleParams, &playback_ip->settings.ttsParams.throttleParams, sizeof(B_PlaybackIp_TtsThrottle_Params));
        throttleParams.playPump = playback_ip->nexusHandles.playpump;
        NEXUS_Playpump_GetSettings(playback_ip->nexusHandles.playpump, &playpumpSettings);
        BDBG_WRN(("Setting throttleParams.timebase: %d", playpumpSettings.timestamp.timebase));
        throttleParams.timebase = playpumpSettings.timestamp.timebase;
        B_PlaybackIp_TtsThrottle_SetSettings(playback_ip->ttsThrottle, &throttleParams);
        B_PlaybackIp_TtsThrottle_Start(playback_ip->ttsThrottle);

        B_PlaybackIp_UtilsFlushSocket(playback_ip);

#ifdef B_HAS_NETACCEL
        sockRecvParams.pktsPerRecv = PKTS_PER_READ;
        if (setsockopt(playback_ip->socketState.fd, SOCK_BRCM_DGRAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
        {
            BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        }
#endif
    }

    /* main loop */
    while (playback_ip->playback_state != B_PlaybackIpState_eStopping) {
        /* get an adequately sized buffer from the playpump */
        if (B_PlaybackIp_UtilsGetPlaypumpBuffer(playback_ip, MAX_BUFFER_SIZE) < 0)
            goto error;

        do {
            /* read next chunk of data from socket */
            rc = B_PlaybackIp_UtilsReadUdpPayloadChunk(
                playback_ip, 
                playback_ip->buffer, 
                PKTS_PER_READ*IP_MAX_PKT_SIZE,  /* how much to read on each recvfrom call: 1pkt worth for std socket */
                PKTS_PER_CHUNK/PKTS_PER_READ,   /* # of iterations of recvfrom calls: 1 for accel socket */
                playback_ip->bytesRecvPerPkt,
                &totalBytesRecv,
                &recvTimeout
                );
            if (rc != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Network Read Error", __FUNCTION__));
                goto error;
            }
            BDBG_MSG_FLOW(("%s: read %d bytes from server\n", __FUNCTION__, totalBytesRecv));
            if (recvTimeout) {
                /* network packet loss event, this constitutues a unmarked discontinuity */
                /* current video decoder & display manager is not able to gracefully recover from it */
                /* and we get a lag of couple of sec between Audio & Video (PR47521).*/
                /* Current solution is to flush the complete pipeline & wait until this n/w pkt loss condition clears */
                flushPipeline = true;
                /* go back & try to read again */
                /* let app know about this timeout using the ipTunerUnlockEvent */
                if (!playback_ip->ipTunerUnLockedEventSent && playback_ip->openSettings.eventCallback) {
                    playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eIpTunerUnLocked);
                    playback_ip->ipTunerUnLockedEventSent = true;
                    playback_ip->ipTunerLockedEventSent = false; /* set the locked event flag so that it can be sent again once data starts coming in */
                }
                continue;
            }
            else {
                /* recvTimeout is not set, we just came out of packet loss condition*/ 
                playback_ip->ipTunerUnLockedEventSent = false; /* reset the TunerUnLockedEvent flag */
                /* flush the AV pipeline, drop this chunk of data, and then resume normal processing */
                if (flushPipeline) {
                    if (B_PlaybackIp_UtilsFlushAvPipeline(playback_ip) < 0)
                        goto error;
                    flushPipeline = false;
                    /* drop this chunk and start fresh from the next chunk */
                    continue;
                }
            }
            /* printf("after read chunk: %d \n", bytesRead); */
        } while (recvTimeout);
        
        /* invoke ipTunerLocked event for after 1st time or after a network timeout exceeding the defined network jitter */
        if (!playback_ip->ipTunerLockedEventSent && playback_ip->openSettings.eventCallback) {
            playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eIpTunerLocked);
            playback_ip->ipTunerLockedEventSent = true;
        }
        /* now feed appropriate data it to the playpump */
        if (NEXUS_Playpump_ReadComplete(playback_ip->nexusHandles.playpump, 0, totalBytesRecv )) {
            BDBG_ERR(("Returned error from bplaypump_read_complete()!"));
            continue;
        }
        /* printf("after read complete \n"); */
        BDBG_MSG_FLOW(("%s: Fed %d bytes to Playpump\n", __FUNCTION__, totalBytesRecv));

    } /* while */

error:
    BDBG_ERR(("%s: done for fd %d", __FUNCTION__, playback_ip->socketState.fd));
    if(playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
        B_PlaybackIp_TtsThrottle_Stop(playback_ip->ttsThrottle);
        B_PlaybackIp_TtsThrottle_Close(playback_ip->ttsThrottle);
    }

    BKNI_SetEvent(playback_ip->playback_halt_event);
    return;
}

void 
B_PlaybackIp_UdpSessionClose(
    B_PlaybackIpHandle playback_ip
    )
{
    if (playback_ip->bytesRecvPerPkt)
        BKNI_Free(playback_ip->bytesRecvPerPkt);

    if (playback_ip->discard_buf) {
        BKNI_Free(playback_ip->discard_buf);
        playback_ip->discard_buf = NULL;
    }
#ifdef EROUTER_SUPPORT
    B_PlaybackIp_UtilsUnsetErouterFilter(playback_ip);
#endif
}

B_PlaybackIpError 
B_PlaybackIp_UdpSetupSimpleSocket(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionOpenSettings *openSettings,
    B_PlaybackIpSocketState *socketState,
    struct addrinfo *addrInfo
    )
{
#if !defined(B_HAS_NETACCEL)
    int reuse_flag = 1;
#endif
    bool ipAddressIsUnicast = false;
    B_PlaybackIpError rc = B_ERROR_PROTO;
    struct ipv6_mreq stMreqIpv6;
    struct ip_mreq stMreq;

#ifndef EROUTER_SUPPORT
    BSTD_UNUSED(playback_ip);
#endif
    if ( (socketState->fd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol)) < 0) {
        BDBG_ERR(("%s: ERROR: Failed to create socket, errno %d\n", __FUNCTION__, errno));
        perror("socket");
        rc = B_ERROR_SOCKET_ERROR;
        goto error;
    }
#if !defined(B_HAS_NETACCEL)
    /* Make socket reusable for PiP and record channels */
    if (setsockopt(socketState->fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_flag, sizeof(reuse_flag) ) < 0 )
        BDBG_WRN(("Set REUSEADDR failed, PiP may not work"));
#endif

    if (bind(socketState->fd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0) {
        BDBG_ERR(("%s: Failed to bind to socket", __FUNCTION__));
        perror("bind");
        rc = B_ERROR_SOCKET_ERROR;
        goto error;
    }
    if (fcntl(socketState->fd, F_SETFL, O_NONBLOCK)) {
        BDBG_WRN(("can't set O_NONBLOCK mode"));
    }

    if (addrInfo->ai_family == AF_INET) {
        if (IN_MULTICAST(ntohl(((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr))) {
            BDBG_WRN(("%s : given address is multicast, joining group", openSettings->socketOpenSettings.ipAddr));
            stMreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr;
            if (openSettings->socketOpenSettings.interfaceName) {
                struct ifreq ifr;
                struct sockaddr_in *ifaddr;
                strncpy(ifr.ifr_name, openSettings->socketOpenSettings.interfaceName, sizeof(ifr.ifr_name)-1);
                if (ioctl(socketState->fd, SIOCGIFADDR, &ifr) != 0) {
                    BDBG_ERR(("ERROR: Failed to get IP Address Information for Interface %s, errno %d", openSettings->socketOpenSettings.interfaceName, errno));
                    rc = B_ERROR_SOCKET_ERROR;
                    goto error;
                }
                ifaddr = (struct sockaddr_in *)&ifr.ifr_addr;
                stMreq.imr_interface = ifaddr->sin_addr;
                BDBG_WRN(("IP Address Information for Interface %s is %s", openSettings->socketOpenSettings.interfaceName, inet_ntoa(stMreq.imr_interface)));
            }
            else {
                stMreq.imr_interface.s_addr = INADDR_ANY;
            }
            if (setsockopt(socketState->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMreq, sizeof(stMreq)) < 0) {
                BDBG_WRN(("%s: ERROR: Set ADD_MEMBERSHIP failed, errno %d", __FUNCTION__, errno));
                perror("setsockopt");
                rc = B_ERROR_SOCKET_ERROR;
                goto error;
            }
        }
        else {
            BDBG_MSG(("%s : given address is NOT multicast address\n", openSettings->socketOpenSettings.ipAddr));
            ipAddressIsUnicast = true;
        }
    }
    else if (addrInfo->ai_family == AF_INET6) {
        if (IN6_IS_ADDR_MULTICAST(((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr.s6_addr)) {
            BDBG_WRN(("%s : given address is IPv6 multicast, joining group", openSettings->socketOpenSettings.ipAddr));
            stMreqIpv6.ipv6mr_multiaddr = ((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr;
            stMreqIpv6.ipv6mr_interface = 0;    /* chooses default interface */
            if (setsockopt(socketState->fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *)&stMreqIpv6, sizeof(stMreqIpv6)) < 0) {
                BDBG_WRN(("%s: ERROR: Set ADD_MEMBERSHIP for IPv6 failed, errno %d\n", __FUNCTION__, errno));
                perror("setsockopt");
                rc = B_ERROR_SOCKET_ERROR;
                goto error;
            }
        }
        else {
            BDBG_MSG(("%s : given address is NOT multicast address\n", openSettings->socketOpenSettings.ipAddr));
            ipAddressIsUnicast = true;
        }
    }
#ifdef EROUTER_SUPPORT
    if (ipAddressIsUnicast) {
        if (B_PlaybackIp_UtilsSetErouterFilter(playback_ip, openSettings) != B_ERROR_SUCCESS)
            goto error;
    }
#endif
    rc = B_ERROR_SUCCESS;
error:
    return rc;
}

B_PlaybackIpError 
B_PlaybackIp_UdpSessionOpen(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionOpenSettings *openSettings,
    B_PlaybackIpSessionOpenStatus *openStatus /* [out] */
    )
{
    B_PlaybackIpError rc = B_ERROR_PROTO;
    struct addrinfo hints;
    struct addrinfo *addrInfo;
    char portString[16];
    B_PlaybackIpSocketState *socketState;

    if (!playback_ip || !openSettings || !openStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, openSettings %p, openStatus %p\n", __FUNCTION__, playback_ip, openSettings, openStatus));
        rc = B_ERROR_INVALID_PARAMETER;
        goto error;
    }

    socketState = &openStatus->socketState;

    /* use getaddinfo() to determine whether user specified ipAddr is a v4 or v6 address */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    memset(portString, 0, sizeof(portString));  /* getaddrinfo() requires port # in the string form */
    snprintf(portString, sizeof(portString), "%d", openSettings->socketOpenSettings.port);
    if (getaddrinfo(openSettings->socketOpenSettings.ipAddr, portString, &hints, &addrInfo) != 0) {
        BDBG_ERR(("%s: ERROR: getaddrinfo failed: %s, errno %d", __FUNCTION__, openSettings->socketOpenSettings.ipAddr, errno));
        perror("getaddrinfo");
        rc = B_ERROR_INVALID_PARAMETER;
        goto error;
    }
    if (addrInfo->ai_family != AF_INET && addrInfo->ai_family != AF_INET6) {
        BDBG_WRN(("%s: ERROR: Unsupported Address family %d\n", __FUNCTION__, addrInfo->ai_family));
        rc = B_ERROR_SOCKET_ERROR;
        goto error;
    }

    BDBG_WRN(("Channel Info: socktype %d, proto = %s, port %d, addr %s", 
            addrInfo->ai_socktype, 
            (openSettings->socketOpenSettings.protocol == B_PlaybackIpProtocol_eUdp) ? "Raw UDP":"RtpNoRtcp",
            ntohs(((struct sockaddr_in *)addrInfo->ai_addr)->sin_port),
            inet_ntoa(((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr)));

    /* we had to set the socktype to standard socktype as getaddrinfo doesn't understand our new sock type */
    /* after getaddrinfo() call, we change the type to new Broadcom accelerated socket type */
#if defined(B_HAS_NETACCEL)
    addrInfo->ai_socktype = SOCK_BRCM_DGRAM;
#else
    addrInfo->ai_socktype = SOCK_DGRAM;
#endif
    if (addrInfo->ai_family == AF_INET6 && ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id == 0) {
        BDBG_WRN(("%s: IPv6 Scope ID is not set, defaulting to 1", __FUNCTION__));
        ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id = 1;
    }


    rc = B_PlaybackIp_UdpSetupSimpleSocket(playback_ip, openSettings, socketState, addrInfo);
    if (rc != B_ERROR_SUCCESS) {
        goto error;
    }
    playback_ip->bytesRecvPerPkt = (int *)BKNI_Malloc(PKTS_PER_CHUNK * sizeof(int));

    playback_ip->discard_buf = (char *)BKNI_Malloc(DISCARD_BUFFER_SIZE * sizeof(char));

    if (playback_ip->bytesRecvPerPkt == NULL || playback_ip->discard_buf == NULL) {
        BDBG_ERR(("%s: BKNI_Malloc failed: recv buff %p, discard_buf %p\n", __FUNCTION__, playback_ip->bytesRecvPerPkt, playback_ip->discard_buf));
        rc = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }

    rc = B_ERROR_SUCCESS;
error:
    return rc;
}

/*
Summary:
    Read callback from playpump
Description:
    We can't do the read and read_complete in the callback, so notify
    the playback_func task
*/

static void 
B_PlaybackIp_ReadCallback(void *context, int param)
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;
    BKNI_SetEvent(playback_ip->read_callback_event);
    BSTD_UNUSED(param);
}

B_PlaybackIpError 
B_PlaybackIp_UdpSessionStart(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionStartSettings *startSettings,
    B_PlaybackIpSessionStartStatus *startStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    B_ThreadSettings settingsThread;
    char *threadName;
    int transportPacketSize;
    NEXUS_Error rc = B_ERROR_UNKNOWN;
    NEXUS_PlaypumpSettings nSettings;

    if (!playback_ip || !startSettings || !startStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, startSettings %p, startStatus %p\n", __FUNCTION__, playback_ip, startSettings, startStatus));
        errorCode = B_ERROR_INVALID_PARAMETER;
        goto error;
    }

    if (startSettings && startSettings->nexusHandlesValid && startSettings->nexusHandles.playpump) {
        NEXUS_Playpump_GetSettings(playback_ip->nexusHandles.playpump, &nSettings);
        nSettings.dataCallback.callback = B_PlaybackIp_ReadCallback;
        nSettings.dataCallback.context = playback_ip;
        rc = NEXUS_Playpump_SetSettings(playback_ip->nexusHandles.playpump, &nSettings);
        if (rc) {
            BDBG_ERR(("%s:%d Nexus Error: %d\n", __FUNCTION__, __LINE__, rc));
            goto error;
        }
        playback_ip->pumpparams = nSettings;
    }

    if (playback_ip->pumpparams.timestamp.type != NEXUS_TransportTimestampType_eNone) {
        BDBG_WRN(("%s: Timestamp mode is enabled", __FUNCTION__));
        transportPacketSize = TS_PKT_SIZE + 4; /* 4 byte timestamp per Transport packet */
    }
    else {
        transportPacketSize = TS_PKT_SIZE;
    }

    BDBG_WRN(("Starting Live IP/UDP Session"));
    playback_ip->payload_size = TS_PKTS_PER_UDP_PKT * transportPacketSize;
    threadName = "PlaybackIpUdp";
    /* create thread to process incoming IP packets & feed them to PB hw */
    B_Thread_GetDefaultSettings(&settingsThread);
    playback_ip->playbackIpThread = B_Thread_Create(threadName, (B_ThreadFunc)B_PlaybackIp_UdpProcessing, (void *)playback_ip, &settingsThread);
    if (NULL == playback_ip->playbackIpThread) {
        BDBG_ERR(("%s: Failed to create the %s thread for RTP media transport protocol\n", __FUNCTION__, threadName));
        goto error;
    }

    errorCode = B_ERROR_SUCCESS;

error:
    return errorCode;
}

#endif /* LINUX || VxWorks */
