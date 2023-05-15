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
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELYn
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
 * $brcm_Workfile: ip_streamer.c $
 * $brcm_Revision: 200 $
 * $brcm_Date: 10/30/12 11:16a $
 *
 * Module Description: 
 *  main test app for ip_streamer
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_streamer.c $
 * 
 * 200   10/30/12 11:16a ssood
 * SW7231-858: correct a getopt typo which was causing RTP streaming
 *  regression
 * 
 * 199   9/13/12 2:23p aginesh
 * SW7435-353: remove tabs
 * 
 * 198   9/13/12 11:18a aginesh
 * SW7435-353: changes to get ip_streamer to work with trellis
 * 
 * 197   9/11/12 3:57p leisun
 * SWSECURITY-48: DTCP-IP change. Auto-generating test keys, consolidate
 *  test/production build, make common DRM default ON
 * 
 * 196   8/3/12 2:52p ssood
 * SW7425-3678: extend ip lib & ip streamer to support streaming out live,
 *  file, encoded content over RTP/UDP
 * 
 * 195   6/25/12 6:08p ssood
 * SW7435-250: coverity fixes exposed by new coverity version
 * 
 * 194   5/22/12 8:56a ssood
 * SW7425-3042: increased the number of threads to 8
 * 
 * 193   5/14/12 4:33p ssood
 * SW7425-3042: refactor ip_streamer into multiple sub files
 * 
 *
 ******************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#include "b_playback_ip_lib.h"
#ifndef DMS_CROSS_PLATFORMS
#include "b_psip_lib.h"
#endif /* DMS_CROSS_PLATFORMS */
#include "ip_streamer_lib.h"
#include "ip_streamer.h"
#include "ip_http.h"
#include "b_os_lib.h"

BDBG_MODULE(ip_streamer);
#define CA_CERT "host.cert"
#define DEFAULT_CONTENT_DIR "/data/videos"
#define DEFAULT_TIMESHIFT_DIR "/data/timeshift"
#define DEFAULT_TIMESHIFT_INTERVAL (60*30)  /* 30 minutes */

#if 0
#define BDBG_MSG_FLOW(x)  BDBG_WRN( x );
#else
#define BDBG_MSG_FLOW(x)  
#endif

/* Globals */
int gExitThread = 0;
pthread_t gThreadId[IP_STREAMER_MAX_THREADS], gChannelMapThreadId;
IpStreamerCtx * _ipStreamerCtx = NULL;
bool gGotSigInt = 0;

void 
signalHandler(int signal)
{
    BDBG_MSG(("Got SIGINT (%d): Cleaning up!!!", signal));
    gGotSigInt = 1;
}

static void
usage(void)
{
    printf("Usage: ip_streamer [-v] [-d] [-s] [-i <interface name>] [-n #threads] [-m media directory path] [-f] [-h] \n");
    printf("options are:\n");
    printf(" -p             # server port (default: 5000)\n");
    printf(" -d             # dtcp-ip server port (default: 8000)\n");
    printf(" -K <0|1|2>     # DTCP/IP key format: test(0) | common DRM(1) | legacy production(2), default is test\n");
    printf(" -C             # DTCP/IP: Enable content key confirmation procedure for sink device\n");
    printf(" -v             # print stats\n");
    printf(" -s             # run in slave mode (for VMS)\n");
    printf(" -i <interface name> # name of the interface to send multicast join on (default: eth0)\n");
    printf(" -m <dir name > # full path of content directory (default: /data/videos)\n");
    printf(" -n <# threads> # number of streaming threads (default: 3)\n");
    printf(" -f             # dont use frontends for live streaming (default live streaming is on)\n");
    printf(" -u <uri>       # uri for RTP/UDP streaming (e.g. rtp://192.168.1.2:8080/data/videos/AbsMpeg2HD.mpg\n");
    printf(" -h             # prints this usage\n");
    printf("\n");
}

static void
setDefaultGlobalConfig(
    IpStreamerGlobalCfg *ipStreamerGlobalCfg
    )
{
    memset(ipStreamerGlobalCfg, 0, sizeof(IpStreamerGlobalCfg));

    ipStreamerGlobalCfg->printStats = false;
    ipStreamerGlobalCfg->listeningPort = IP_STREAMER_DEFAULT_SERVER_PORT;
    ipStreamerGlobalCfg->dtcpAkePort = IP_STREAMER_DEFAULT_DTCP_IP_PORT;
    ipStreamerGlobalCfg->dtcpKeyFormat = 0;
    ipStreamerGlobalCfg->ckc_check = true;
    ipStreamerGlobalCfg->accelSocket = false;
    ipStreamerGlobalCfg->slaveMode = false;
    strncpy(ipStreamerGlobalCfg->rootDir, DEFAULT_CONTENT_DIR, sizeof(ipStreamerGlobalCfg->rootDir));
    strncpy(ipStreamerGlobalCfg->timeshiftDirPath, DEFAULT_TIMESHIFT_DIR, sizeof(ipStreamerGlobalCfg->timeshiftDirPath));
    ipStreamerGlobalCfg->timeshiftBufferInterval = DEFAULT_TIMESHIFT_INTERVAL;
    ipStreamerGlobalCfg->maxBitRate = 20*1024*1024; /* 20 Mpbs */
    strncpy(ipStreamerGlobalCfg->interfaceName, "eth0", sizeof(ipStreamerGlobalCfg->interfaceName));
    ipStreamerGlobalCfg->numStreamingSessions = 8;
    ipStreamerGlobalCfg->disableFrontend = false;
    ipStreamerGlobalCfg->streamingCfg.streamingProtocol = B_PlaybackIpProtocol_eHttp;
}

extern char *B_PlaybackIp_UtilsStristr(char *str, char *subStr);
static bool
parseUri(char *uri, IpStreamerStreamingOutCfg *streamingCfg)
{
    char *tmp1, *tmp2, *tmp3;

    memset(streamingCfg, 0, sizeof(IpStreamerStreamingOutCfg));

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(uri, "rtp:")) != NULL) {
        /* rtp protocol is being used, parse uri further */
        tmp1 += strlen("rtp://");
        streamingCfg->streamingProtocol = B_PlaybackIpProtocol_eRtp;
    }
    else if ( (tmp1 = B_PlaybackIp_UtilsStristr(uri, "udp:")) != NULL) {
        /* udp protocol is being used, parse uri further */
        tmp1 += strlen("udp://");
        streamingCfg->streamingProtocol = B_PlaybackIpProtocol_eUdp;
    }
    else {
        BDBG_ERR(("incorrect usage of -u option (uri %s), use it to specify either RTP or UDP streaming", uri));
        return false;
    }

    /* now parse rest of the URL string: take out the server string from the url */
    tmp2 = strstr(tmp1, "/");
    if (tmp2) {
        streamingCfg->streamingIpAddress = strndup(tmp1, (tmp2 - tmp1));

        /* Check to see if a port value was specified */
        tmp3 = strstr(streamingCfg->streamingIpAddress, ":");
        if (tmp3) {
            tmp3[0] = '\0'; /* this null terminates the server name string */
            tmp3++;
            streamingCfg->streamingPort = strtoul(tmp3, (char **) NULL, 10);
            if (streamingCfg->streamingPort == 0) {
                BDBG_ERR(("incorrect usage of -u option (uri %s), need to specify port # for RTP or UDP streaming", uri));
                goto error;
            }
        }
        else  {
            BDBG_ERR(("incorrect usage of -u option (uri %s), need to specify port # for RTP or UDP streaming", uri));
            goto error;
        }

        /* now get the uri */
        streamingCfg->url = strdup(tmp2);
        BDBG_WRN(("streaming protocol: %s, server:port %s:%d url %s", streamingCfg->streamingProtocol == B_PlaybackIpProtocol_eRtp ? "RTP":"UDP", streamingCfg->streamingIpAddress, streamingCfg->streamingPort, streamingCfg->url));

        return true;
    }
    else {
        BDBG_ERR(("incorrect usage of -u option (uri %s), need to specify file name for RTP or UDP streaming", uri));
    }
error:
    if (streamingCfg->streamingIpAddress)
        free(streamingCfg->streamingIpAddress);
    if (streamingCfg->url)
        free(streamingCfg->url);
    memset(streamingCfg, 0, sizeof(IpStreamerStreamingOutCfg));
    return false;
}

static int
parserIpStreamerOptions(
    int argc,
    char *argv[],
    IpStreamerGlobalCfg *ipStreamerGlobalCfg
    )
{
    int ch;
    char *streamingUri = NULL;

    /* set default global configuration */
    setDefaultGlobalConfig(ipStreamerGlobalCfg);

    /* # of streaming threads, cpu affinity, */
    while ( (ch = getopt(argc, argv, "p:sd:K:vi:m:n:fu:Ch")) != -1) {
        switch (ch) {
        case 'p':
            ipStreamerGlobalCfg->listeningPort = atoi(optarg);
            break;
        case 'd':
            ipStreamerGlobalCfg->dtcpAkePort = atoi(optarg);
            break;
        case 'K':
            ipStreamerGlobalCfg->dtcpKeyFormat = atoi(optarg);
            break;
        case 'C':
            ipStreamerGlobalCfg->ckc_check = true;
            break;
        case 'v':
            ipStreamerGlobalCfg->printStats = true;
            break;
        case 's':
            ipStreamerGlobalCfg->slaveMode = true;
            break;
        case 'i':
            memcpy(ipStreamerGlobalCfg->interfaceName, optarg, sizeof(ipStreamerGlobalCfg->interfaceName));
            break;
        case 'm':
            memcpy(ipStreamerGlobalCfg->rootDir, optarg, sizeof(ipStreamerGlobalCfg->rootDir));
            break;
        case 'n':
            ipStreamerGlobalCfg->numStreamingSessions = atoi(optarg);
            if (ipStreamerGlobalCfg->numStreamingSessions > IP_STREAMER_MAX_THREADS) {
                BDBG_ERR(("ERROR: Max Streaming Sessions supported %d, asked %d", IP_STREAMER_MAX_THREADS, ipStreamerGlobalCfg->numStreamingSessions));
                goto error;
            }
            break;
        case 'f':
            ipStreamerGlobalCfg->disableFrontend = true;
            break;
        case 'u':
            streamingUri = optarg;
            break;
        case 'h':
        default:
            goto error;
        }
    }

    if (streamingUri) {
        if (parseUri(streamingUri, &ipStreamerGlobalCfg->streamingCfg) != true) {
            goto error;
        }
    }

    return 0;

error:
     usage();
     exit(1);
}

void *
channelMapThread(
    void *data
    )
{
    IpStreamerGlobalCfg *ipStreamerGlobalCfg = (IpStreamerGlobalCfg *)data;
    int listeningFd=-1, sockFd;
    struct sockaddr_in localAddr;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char name[264];
    FILE *channelFileFp = NULL;
    char *buffer = NULL, *buf;
    int bytes, bytesLeft;
    int reuse_flag = 1;

    BDBG_MSG(("Starting Channel Map Thread "));
    if ((listeningFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        /* Socket Create Error */
        perror("Socket Open Err");
        goto error;
    }
    if (setsockopt(listeningFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof(reuse_flag) ) < 0 ) {
        BDBG_ERR(("REUSE Socket Error"));
        goto error;
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(ipStreamerGlobalCfg->listeningPort+2);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listeningFd, (struct sockaddr *) &localAddr, sizeof(localAddr))) {
        perror("Socket Bind Err to bind to actual i/f");
        goto error;
    }
    if (listen(listeningFd, 8)) {
        perror("Socket listen Err");
        goto error;
    }
    /* Set stdin to non-blocking */
    if (fcntl(listeningFd, F_SETFL, fcntl(listeningFd, F_GETFL)|O_NONBLOCK) < 0)
        BDBG_WRN(("Failed to set non-blocking mode on listening socket"));

    snprintf(name, sizeof(name)-1, "%s%s%s", ipStreamerGlobalCfg->rootDir, "/", "httpChannelMap.txt");
    if ((channelFileFp = fopen(name, "r" )) == NULL) {
        BDBG_ERR(("%s: Unable to open HTTP channel map file file %s", __FUNCTION__, name));
        goto error;
    }
    if ((buffer = BKNI_Malloc(8192)) == NULL) { BDBG_ERR(("BKMI_Malloc Failure at %d", __LINE__)); goto error;}

    while (!gExitThread) {
        waitForNetworkEvent(listeningFd);
        /* accept connection */
        if ((sockFd = accept(listeningFd, NULL, NULL)) < 0) {
            perror("ERROR: accept():");
            goto error;
        }

        if (getsockname(sockFd, (struct sockaddr *)&addr, &addrlen)) {
            perror("getsockname():");
            close(sockFd);
            goto error;
        }

        /* open the channel map file which contains all the streamable media file names */
        rewind(channelFileFp);
        bytesLeft = 8192;
        buf = buffer;
        while (fgets(name, sizeof(name)-1, channelFileFp)) {
            bytes = snprintf(buf, bytesLeft, "IP_HTTP 0 %s %d %s%s", inet_ntoa(addr.sin_addr), ipStreamerGlobalCfg->listeningPort, "/", name);
            buf += bytes;
            bytesLeft -= bytes;
        }
        bytes = buf - buffer;
        if (send(sockFd, (void*)buffer, bytes, MSG_NOSIGNAL) < 0) {
            BDBG_MSG(("Failed to send %d bytes of channel map data, errno %d", bytes, errno));
        }
        BDBG_WRN(("%s: send %d bytes of channel map data", __FUNCTION__, bytes));
        close(sockFd);
    }

error:
    BDBG_MSG(("Exiting Channel Map Thread"));
    if (listeningFd >= 0)
        close(listeningFd);
    if (channelFileFp) fclose(channelFileFp);
    if (buffer) BKNI_Free(buffer);
    pthread_exit(NULL);
}

bool 
createInfoAndNavFiles(
    IpStreamerGlobalCfg *ipStreamerGlobalCfg,
    IpStreamerGlobalCtx *ipStreamerGlobalCtx
    )
{
    int rc;
    struct dirent **namelist=NULL;
    int numDirEntries, i;
    struct stat fileStats;
    char fullFilePath[256];
    char url[264], *name;
    IpStreamerCtx *ipStreamerCtx;
    IpStreamerOpenSettings openSettings;
    B_PlaybackIpPsiInfo psi;
    FILE *channelFileFp = NULL;

    if ( (numDirEntries = scandir(ipStreamerGlobalCfg->rootDir, &namelist, NULL, NULL)) < 0) {
        BDBG_ERR(("Failed to sacn media directory %s, errno %d", ipStreamerGlobalCfg->rootDir));
        return false;
    }

    name = url; /* use url storage */
    snprintf(name, sizeof(url)-1, "%s%s%s", ipStreamerGlobalCfg->rootDir, "/", "httpChannelMap.txt");
    if ((channelFileFp = fopen(name, "w+" )) == NULL) {
        BDBG_ERR(("%s: Unable to open HTTP channel map file file %s", __FUNCTION__, name));
        goto error;
    }

    for (i=0; i < numDirEntries; i++) {
        snprintf(fullFilePath, sizeof(fullFilePath)-1, "%s%s%s",
                ipStreamerGlobalCfg->rootDir, "/", namelist[i]->d_name);
        if (stat(fullFilePath, &fileStats) < 0) {
            BDBG_WRN(("File stat() failed on %s, ignoring it", fullFilePath));
            continue;
        }
        if (S_ISDIR(fileStats.st_mode)) {
            BDBG_MSG(("Ignoring sub-directories (%s) from media search", fullFilePath));
            continue;
        }
        if (strstr(fullFilePath, ".m3u8") || strstr(fullFilePath, ".info") || strstr(fullFilePath, ".nav") || strstr(fullFilePath, ".txt")) {
            BDBG_MSG(("Ignoring nav/info files (%s) from media search for now", fullFilePath));
            continue;
        }

        /* !dir, !info, !nav, so try to get probe info on this file */

        memset(&openSettings, 0, sizeof(openSettings));
        openSettings.streamingFd = 0; /* nop */
        snprintf(url, sizeof(url)-1, "/File=%s;", fullFilePath);
        openSettings.requestUri = url;
        openSettings.mediaProbeOnly = true;
        ipStreamerCtx = (IpStreamerCtx *)B_IpStreamer_SessionOpen(ipStreamerGlobalCtx, &openSettings);
        if (!ipStreamerCtx) {BDBG_ERR(("ERROR: Failed to Open IP Streaming Context")); continue;}

        rc = B_IpStreamer_SessionAcquirePsiInfo(ipStreamerCtx, &psi);
        if (rc) {BDBG_ERR(("Failed to acquire PSI Info ")); /* TODO: dont add it to channel map */}
        B_IpStreamer_SessionClose(ipStreamerCtx);
        BDBG_MSG(("File name %s, psi valid %d", fullFilePath, psi.psiValid));
        fwrite(namelist[i]->d_name, 1, strlen(namelist[i]->d_name), channelFileFp);
        fwrite("\n", 1, 1, channelFileFp);
        free(namelist[i]);
    }
    BDBG_WRN(("Built Media Info and Nav files for media"));
error:
    if (channelFileFp) fclose(channelFileFp);
    if (namelist) free(namelist);
    return true;
}

static void
closeSocket(int streamingFd)
{
    close(streamingFd);
}

extern void B_PlaybackIp_HttpParseRespHeaderForPsi(B_PlaybackIpHandle playback_ip, NEXUS_TransportType http_content_type, char *http_hdr, B_PlaybackIpPsiInfoHandle psi);
void *
ipStreamerHttpStreamingThread(
    void *data
    )
{
    int err;
    NEXUS_Error rc = NEXUS_UNKNOWN;
    B_PlaybackIpPsiInfo psi;
    IpStreamerCtx *ipStreamerCtx;
    IpStreamerGlobalCtx *ipStreamerGlobalCtx = (IpStreamerGlobalCtx *)data;
    int streamingFd;
    char *requestUri = NULL;
    char requestUrl[512];
    int requestUrlLen = 512;
    IpStreamerOpenSettings openSettings;

    BDBG_WRN(("Starting HTTP Streaming Thread"));

    /*
     * Main flow (in a while loop):
     * Wait on new request from client (client can be remote (streaming request)
     * Accept HTTP Request from client and parse the request type:
     *  -determine src type: IP, QAM, Disk, etc.
     *  -determine dst type: IP, and/or Record
     * Call SessionOpen() to:
     *  -select a free src of requested srcType
     * Use SessionAcquitePsiInfo to determine AV PIDS & Codecs (internally starts frontends so that it can receive PSI tables & then stops them)
     * Send HTTP Response back to the client
     * Call SessionStart() to:
     *  -select free dst: IP and/or Record
     *  -start dst(s)
     *  -start src
     * Call SessionStatus() in a loop to check client connection status or signal from local app to exit
     * Call SessionStop() to
     *  -stop dst(s)
     *  -stop dst(s)
     * Call SessionClose() to:
     *  -close dst(s)
     *  -close src
     * go back & wait on new request from client
     */
    memset(requestUrl, 0, sizeof(requestUrl));
    while (!gExitThread) {
        /* wait on new client request */
        /* once client sends HTTP Get request, incoming request should give us info on what type of src & dst to use */
        memset(&openSettings, 0, sizeof(openSettings));
        requestUri = acceptNewHttpRequest(ipStreamerGlobalCtx->listeningFd, requestUrl, requestUrlLen, &streamingFd);
        if (requestUri == NULL) {BDBG_ERR(("Accept failed, go back to receiving new client request")); continue;}
        BDBG_MSG(("socket fd to stream content on %d", streamingFd));
#ifndef DMS_CROSS_PLATFORMS
        B_PlaybackIp_HttpParseRespHeaderForPsi(NULL, NEXUS_TransportType_eTs, requestUrl, &psi);
        if (psi.psiValid) {
            openSettings.skipPsiAcquisition = true;
        }
#endif

        /* Open the streaming session */
        openSettings.streamingFd = streamingFd;
        openSettings.requestUri = requestUri;
        openSettings.mediaProbeOnly = false;
        openSettings.autoRewind = getEnvVariableValue("autoRewind", 1);
        ipStreamerCtx = (IpStreamerCtx *)B_IpStreamer_SessionOpen(ipStreamerGlobalCtx, &openSettings);
        if (!ipStreamerCtx) {BDBG_ERR(("ERROR: Failed to Open IP Streaming Context")); goto errorCloseHttp;}

        if (!psi.psiValid) {
            /* psi info wasn't sent from the client, so obtain PSI info from requested source */
            rc = B_IpStreamer_SessionAcquirePsiInfo(ipStreamerCtx, &psi);
            if (rc) {BDBG_ERR(("Failed to acquire PSI Info ")); goto errorCloseSrc;}
        }

        /* send response to client for Live streaming request, we include the PSI info in the response */
        BDBG_MSG(("CTX %p: vpid %d, apid %d", ipStreamerCtx, psi.videoPid, psi.audioPid));
        rc = sendHttpResponse(&ipStreamerCtx->cfg, streamingFd, &psi);
        if (rc) {BDBG_ERR(("Failed to send HTTP response (%d)... ", rc)); goto errorCloseSrc;}
        BDBG_MSG(("Sent HTTP Response to server on socket %d", streamingFd));

        /* now start streaming session */
        err = B_IpStreamer_SessionStart((void *)ipStreamerCtx, &psi);
        if (err < 0) {BDBG_ERR(("ERROR: Failed to start IP Streamer Session")); goto errorCloseSrc;}

        while (!gExitThread) {
            static bool firstTime = false;
            /* loop until thread is stopped, client stops the sessions or incoming live stream underflows */
            IpStreamerSessionStatus status;
#ifdef NEXUS_HAS_VIDEO_ENCODER
            if (ipStreamerCtx->transcoderDst) {
                if (B_IpStreamer_InsertPatPmtTables(ipStreamerCtx) == false)
                    break;
            }
            else
#endif /* NEXUS_HAS_VIDEOENCODER */
                BKNI_Sleep(50);

            if (firstTime)
            {
                firstTime = false;
                BDBG_WRN(("Sleeping for 10 sec"));
                BKNI_Sleep(10000);
                IpStreamerSettings settings;
                memset(&settings, 0, sizeof(settings));
                settings.streamingEnabled = true;
                settings.streamingFd = ipStreamerCtx->cfg.streamingFd;
                if (B_IpStreamer_SessionSetSettings( (void *)ipStreamerCtx, &settings) < 0) {
                    BDBG_ERR(("%s: ERROR: B_IpStreamer_SessionSetSettings()", __FUNCTION__));
                    break;
                }
                BDBG_ERR(("%s: started File Streaming handle", __FUNCTION__));
            }
            BKNI_Sleep(1000);
            B_IpStreamer_SessionStatus(ipStreamerCtx, &status);
            if (!status.active) {
                BDBG_MSG(("Session is not longer active, breaking out..."));
                break;
            }
        }

        B_IpStreamer_SessionStop(ipStreamerCtx);
errorCloseSrc:
        B_IpStreamer_SessionClose(ipStreamerCtx);
errorCloseHttp:
        closeSocket(streamingFd);
        if (gExitThread)
            break;
        BDBG_WRN(("CTX %p: Current ip streaming / local recording session is closed, go back to listening for new requests", ipStreamerCtx));
    } /* while (!gExitThread) */

    BDBG_WRN(("Exiting Streamer Thread"));
    pthread_exit(NULL);

    return NULL;
}

void *
ipStreamerRtpUdpStreamingThread(
    void *data
    )
{
    int err;
    NEXUS_Error rc = NEXUS_UNKNOWN;
    B_PlaybackIpPsiInfo psi;
    IpStreamerCtx *ipStreamerCtx;
    IpStreamerGlobalCtx *ipStreamerGlobalCtx = (IpStreamerGlobalCtx *)data;
    char requestUrl[512];
    IpStreamerOpenSettings openSettings;
    IpStreamerStreamingOutCfg *streamingCfg = &ipStreamerGlobalCtx->globalCfg.streamingCfg;

    BDBG_WRN(("%s: Starting Streaming Thread", __FUNCTION__));

    /*
     * Main flow:
     * Open the streaming socket
     * Call SessionOpen() to:
     *  -select a free src of requested srcType
     * Use SessionAcquitePsiInfo to determine AV PIDS & Codecs (internally starts frontends so that it can receive PSI tables & then stops them)
     * Call SessionStart() to:
     *  -select free dst: IP and/or Record
     *  -start dst(s)
     *  -start src
     * Call SessionStatus() in a loop to check client connection status or signal from local app to exit
     * Call SessionStop() to
     *  -stop dst(s)
     *  -stop dst(s)
     * Call SessionClose() to:
     *  -close dst(s)
     *  -close src
     * exit
     */
    if (!gExitThread) {
        int i;
        memset(&openSettings, 0, sizeof(openSettings));
        openSettings.streamingFdLocal = -1; /* indicates no local streaming need */
        /* Open the streaming session */
        openSettings.streamingFd = -1;
        openSettings.mediaProbeOnly = false;
        if (strstr(streamingCfg->url, "LiveChannel;") == NULL) {
            i = snprintf(requestUrl, sizeof(requestUrl)-1, "/File=%s;Protocol=%s;Address=%s;Port=%d;",
                    streamingCfg->url, streamingCfg->streamingProtocol == B_PlaybackIpProtocol_eRtp ? "RTP":"UDP",
                    streamingCfg->streamingIpAddress, streamingCfg->streamingPort);
        }
        else {
            i = snprintf(requestUrl, sizeof(requestUrl)-1, streamingCfg->url);
        }
        requestUrl[i] = '\0';
        BDBG_MSG(("%s: RTP/UDP source url %s", __FUNCTION__, requestUrl));

        openSettings.requestUri = requestUrl;
        ipStreamerCtx = (IpStreamerCtx *)B_IpStreamer_SessionOpen(ipStreamerGlobalCtx, &openSettings);
        if (!ipStreamerCtx) {BDBG_ERR(("ERROR: Failed to Open IP Streaming Context")); goto errorCloseHttp;}

        ipStreamerCtx->cfg.streamingCfg = *streamingCfg;
        /* obtain PSI info from requested source */
        rc = B_IpStreamer_SessionAcquirePsiInfo(ipStreamerCtx, &psi);
        if (rc) {BDBG_ERR(("Failed to acquire PSI Info ")); goto errorCloseSrc;}

        BDBG_MSG(("CTX %p: vpid %d, apid %d", ipStreamerCtx, psi.videoPid, psi.audioPid));

        /* now start streaming session */
        err = B_IpStreamer_SessionStart((void *)ipStreamerCtx, &psi);
        if (err < 0) {BDBG_ERR(("ERROR: Failed to start IP Streamer Session")); goto errorCloseSrc;}

        while (!gExitThread) {
            IpStreamerSessionStatus status;
            /* loop until thread is stopped, client stops the sessions or incoming live stream underflows */
            BKNI_Sleep(1000);
            B_IpStreamer_SessionStatus(ipStreamerCtx, &status);
            if (!status.active) {
                BDBG_MSG(("Session is not longer active, breaking out..."));
                break;
            }
        }

        B_IpStreamer_SessionStop(ipStreamerCtx);
errorCloseSrc:
        B_IpStreamer_SessionClose(ipStreamerCtx);
errorCloseHttp:
        BDBG_WRN(("CTX %p: Current ip streaming / local recording session is done, exiting..", ipStreamerCtx));
    } /* while (!gExitThread) */

    BDBG_WRN(("Exiting Streamer Thread"));
    gExitThread = 1;
    pthread_exit(NULL);

    return NULL;
}

int main(int argc, char *argv[])
{
    int i;
    IpStreamerGlobalCfg ipStreamerGlobalCfg;
    IpStreamerGlobalCtx *ipStreamerGlobalCtx = NULL;
    void *(*threadFunc)(void*);
    struct ifreq ifr;

    /* parse command line options */
    if (parserIpStreamerOptions(argc, argv, &ipStreamerGlobalCfg)) {
        BDBG_ERR(("ERROR: Incorrect Command line Options"));
        return -1;
    }
    signal(SIGINT, signalHandler);

    /* ignore SIGPIPE otherwise abnormal termination on client can cause server to crash */
    signal(SIGPIPE, SIG_IGN);

    ipStreamerGlobalCfg.multiProcessEnv = false;

    /* initialize the IP Streamer */
    ipStreamerGlobalCtx = (IpStreamerGlobalCtx *)B_IpStreamer_Init(&ipStreamerGlobalCfg);
    if (!ipStreamerGlobalCtx) {BDBG_ERR(("B_IpStreamer_Init() failed at %d, Exiting...", __LINE__)); goto error;}

    /* generate info & nav files for AV content if not already there */
    createInfoAndNavFiles(&ipStreamerGlobalCfg, ipStreamerGlobalCtx);

    if (ipStreamerGlobalCfg.streamingCfg.streamingProtocol == B_PlaybackIpProtocol_eRtp ||
        ipStreamerGlobalCfg.streamingCfg.streamingProtocol == B_PlaybackIpProtocol_eUdp) {
        ipStreamerGlobalCfg.numStreamingSessions = 1; /* one streaming session per invocation */
        threadFunc = ipStreamerRtpUdpStreamingThread;
    }
    else {
        /* setup HTTP listening Server for receiving live streaming and/or local recording requests from clients */
        ipStreamerGlobalCtx->listeningFd = initTcpServer(&ipStreamerGlobalCfg);
        if (ipStreamerGlobalCtx->listeningFd < 0) {BDBG_ERR(("Failed to open lisenting socket (%d) at %d, Exiting...", ipStreamerGlobalCtx->listeningFd, __LINE__)); goto error;}
        threadFunc = ipStreamerHttpStreamingThread;
    }

    /* start worker threads */
    for (i=0; i<ipStreamerGlobalCfg.numStreamingSessions; i++) {
        if (pthread_create(&gThreadId[i], NULL, threadFunc, (void *)ipStreamerGlobalCtx)) {
            BDBG_ERR(("Failed to create pthread, errno %d", errno));
            return -1;
        }
    }
    if (pthread_create(&gChannelMapThreadId, NULL, channelMapThread, (void *)&ipStreamerGlobalCfg)) {
        BDBG_ERR(("Failed to create channel map pthread, errno %d", errno));
        return -1;
    }

    strncpy(ifr.ifr_name, ipStreamerGlobalCfg.interfaceName, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = '\0';
    BDBG_MSG(("%s: interface name %s", __FUNCTION__, ifr.ifr_name));

    /* now retrieve the IP address associated with the media */
    if (ipStreamerGlobalCfg.streamingCfg.streamingProtocol == B_PlaybackIpProtocol_eHttp) {
        if (ioctl(ipStreamerGlobalCtx->listeningFd, SIOCGIFADDR, &ifr) != 0) {
            BDBG_WRN(("%s: Failed to get Interface Address Information for %s", __FUNCTION__, ifr.ifr_name));
            return -1;
        }
        BDBG_WRN(("############## Server Listening on IP Address:Port %s:%d ###############", inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr), ipStreamerGlobalCfg.listeningPort));
    }

    while (!gExitThread) {
        /* main thread waits until app is asked to stop */
        if (gGotSigInt) {
            BDBG_WRN(("Exiting from main thread, requesting children threads to exit as well"));
            gExitThread = 1;
            break;
        }
        BKNI_Sleep(1000);
    }
    /* app is being shutdown, do cleanup */

    /* wait for childres thread to complete */
    for (i=0; i<ipStreamerGlobalCfg.numStreamingSessions; i++) {
        if (pthread_join(gThreadId[i], NULL)) {
            BDBG_ERR(("Failed to join pthread, errno %d", errno));
            return -1;
        }
    }
    pthread_join(gChannelMapThreadId, NULL);

    if (ipStreamerGlobalCfg.streamingCfg.streamingProtocol == B_PlaybackIpProtocol_eHttp)
        unInitTcpServer(ipStreamerGlobalCtx->listeningFd);
error:
    B_IpStreamer_UnInit(ipStreamerGlobalCtx);
    return 0;
}
