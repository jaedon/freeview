/***************************************************************************
*     (c)2003-2012 Broadcom Corporation
*
*  Thi program is the proprietary software of Broadcom Corporation and/or its licensors,
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
* $brcm_Workfile: b_playback_ip_mpeg_dash.c $
* $brcm_Revision: 3 $
* $brcm_Date: 9/4/12 6:13p $
*
* Description: MPEG DASH (ISO/IEC 23009) Streaming Client Implementation
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_mpeg_dash.c $
* 
* 3   9/4/12 6:13p gskerl
* SW7346-853: Added new cookie argument in call to http_parse_redirect().
* 
* 2   7/24/12 4:05p gskerl
* SW7346-853: Added some support for parsing initialization segments.
*  Also changed the calling sequence to B_PlaybackIp_MpegDashParseMPD().
* 
* 1   7/19/12 4:07p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/4   7/17/12 1:59p gskerl
* SW7346-853: Added a PRINT_XML_DEBUG macro for printing XML debug
*  messages.
* 
* SW7346-853/3   7/10/12 7:45p gskerl
* SW7346-853: Applied recent changes made to b_playback_ip_hls.c.
* 
* SW7346-853/2   7/10/12 5:21p gskerl
* SW7346-853: Added some ifdefs to allow for compile without
*  B_HAS_HTTP_AES_SUPPORT.
* 
* SW7346-853/1   7/6/12 10:50a gskerl
* SW7346-853: Initial checkin. Adapted from b_playback_ip_hls.c. Still a
*  work in progress..
* 
***************************************************************************/
#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_aes.h"
#include "b_playback_ip_utils.h"
#include "b_playback_ip_lm_helper.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <ctype.h>

BDBG_MODULE(b_playback_ip_mpeg_dash);
BDBG_FILE_MODULE(b_playback_ip_mpeg_dash_xml);

/* Overall flow 
   -Browser obtains the URI of a AV stream by parsing the html page (video tag for html5)
    -URI points to an MPD (Media Presentation Description) file which contains an ordered list of media URIs & informational tags
    -Each media URI refers to a media file which is a segment of of a single continous stream 
   -Browser passes the playlist URI to the IP library
   -IP library sends request to the server to obtain the playlist file and parses it 
*/
/* xml tags */
#define XML_TAG_MPD                         "MPD"
#define XML_TAG_PERIOD                      "Period"            /* Period is child of MPD */
#define XML_TAG_ADAPTATIONSET               "AdaptationSet"     /* AdaptationSet is child of Period */
#define XML_TAG_REPRESENTATION              "Representation"    /* Representation is child of AdaptataionSet */
#define XML_TAG_SEGMENTLIST                 "SegmentList"       /* SegmentList is child of Representation */
#define XML_TAG_SEGMENTURL                  "SegmentURL"        /* SegmentURL is child of SegmentList */
#define XML_TAG_SEGMENTBASE                 "SegmentBase"       /* SegmentURL is child of SegmentList */
#define XML_TAG_BASEURL                     "BaseURL"           /* BaseURL is child of several element types */
#define XML_TAG_INITIALIZATIONSEGMENT       "Initialization"    /* Initialization segment is child of SegmentBase or SegmentList */

/* xml attributes */
#define XML_ATT_PROFILES                     "profiles"
#define XML_ATT_ID                           "id"
#define XML_ATT_BANDWIDTH                    "bandwidth"
#define XML_ATT_MEDIA                        "media"
#define XML_ATT_TYPE                         "type"
#define XML_ATT_SOURCEURL                    "sourceURL"

bool  g_xmlDebugEnabled = false;     /* This will get set to true if the "b_playback_ip_mpeg_dash_xml" debug level is enabled. */
#define PRINT_XML_DEBUG(bdbg_args, node)  do { if (g_xmlDebugEnabled){BDBG_LOG( bdbg_args);  B_PlaybackIp_XmlElem_Print(node, 0);} } while(0);

#define MPEG_DASH_PLAYLIST_FILE_SIZE (500*1024)
#define MPEG_DASH_EVENT_TIMEOUT_MSEC 1000 /* 1 secs */
#define MPEG_DASH_READ_CHUNK_SIZE (TS_PKT_SIZE*HTTP_AES_BLOCK_SIZE*5)


extern int playback_ip_read_socket( B_PlaybackIpHandle playback_ip, void *securityHandle, int fd, char *buf, int buf_size, int timeout);
bool http_absolute_uri(char *url);
int B_PlaybackIp_SecuritySessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, int fd, void **newSecurityHandle);
NEXUS_TransportType http_get_payload_content_type(char *http_hdr);
extern int http_read_response(B_PlaybackIpHandle playback_ip, void *securityHandle, int fd, char **rbufp, int rbuf_size, char **http_hdr, char **http_payload, int *payload_len);
extern http_url_type_t http_get_url_type(char *http_hdr, char *url);

void B_PlaybackIp_MpegDashInsertPlaylistInfoEntry(MpegDashSessionState *mpegDashSession, PlaylistFileInfo *newPlaylistFileInfo);

#if 0
#define MPEG_DASH_UNIT_TEST
#endif

#ifdef MPEG_DASH_UNIT_TEST
static bool printVariantPlaylistFile(MpegDashSessionState *mpegDashSession);

static void
dump_playlist_file(MpegDashSessionState *mpegDashSession)
{
    static FILE * playlistFile = NULL;
    playlistFile = fopen("/data/videos/dash_mpd.mpd", "wb+");
    fwrite(mpegDashSession->playlistBuffer, 1, mpegDashSession->playlistBufferDepth , playlistFile); 
    fflush(playlistFile);
    fclose(playlistFile);
    BDBG_MSG(("%s: wrote downloaded MPD file into file /data/videos/dash_mpd.mpd", __FUNCTION__));
}
#endif

bool 
B_PlaybackIp_IsMpegDashSession (
    char *playlistUri, 
    char *httpResponseHeader
    )
{
    char *contentType;
    /*
     *  Either file name or HTTP Content-type will indicate it be an MPEG-DASH MPD file.
     *  Assume DASH if file name ends w/ ".mpd" and/or HTTP Content-type is "application/dash+xml" 
     */
    if (B_PlaybackIp_UtilsStristr(playlistUri, ".mpd") != NULL) {
        BDBG_MSG(("%s: MPEG-DASH MPD file URI: %s", __FUNCTION__, playlistUri));
        return true;
    }

    if ((contentType = B_PlaybackIp_UtilsStristr(httpResponseHeader, "Content-Type: ")) != NULL) {
        contentType += strlen("Content-Type: ");

        if (B_PlaybackIp_UtilsStristr(contentType, "application/dash+xml")) {
            BDBG_MSG(("%s: MPEG-DASH MPD file URI, contentType %s", __FUNCTION__, playlistUri, contentType));
            return true;
        }
    }

    /* neither uri name nor content type indicates that the session is MPEG-DASH. */
    return false;
}


char *
B_PlaybackIp_MpegDashBuildAbsoluteUri(char *server, int port, char *baseUri, char *fileName) 
{
    int uriLength;
    char portString[16];
    char *uri;
    char *tmp1, *tmp2 = NULL;
    int baseUriLength;
    char *baseUriCopy = NULL;

    BDBG_ASSERT(server);
    BDBG_ASSERT(baseUri);
    BDBG_ASSERT(fileName);

    /* determine the # of char for the port */
    memset(portString, 0, sizeof(portString));
    snprintf(portString, sizeof(portString)-1, "%d", port);

    /* check if we need to use path from the base URI */
    tmp1 = baseUri;
    while ((tmp1 = strstr(tmp1, "/")) != NULL) {
        tmp2 = tmp1; /* note location of next directory path */
        tmp1 += 1; /* move past the / char */
    }
    if (tmp2) {
        baseUriLength = tmp2 - baseUri + 1 + 1; /* one for / char, one for NULL char */
        if ((baseUriCopy = (char *)BKNI_Malloc(baseUriLength)) == NULL) {
            BDBG_ERR(("%s: ERROR: failed to allocate %d bytes of memory at %d\n", __FUNCTION__, baseUriLength, __LINE__));
            return NULL;
        }
        strncpy(baseUriCopy, baseUri, baseUriLength-1);
        baseUriCopy[baseUriLength-1] = '\0';
    }
    else {
        /* no path to use from baseUri */
        baseUriLength = 0;
    }

    /* now allocate space for holding the absolute URI */
    uriLength = strlen(server) + strlen(portString) + baseUriLength + strlen(fileName) + 11; /* extra characters for http header */
    if ((uri = (char *)malloc(uriLength)) == NULL) {
        if (baseUriCopy) 
            BKNI_Free(baseUriCopy);
        BDBG_ERR(("%s: Failed to allocate %d bytes of memory for building uri", __FUNCTION__, uriLength));
        return NULL;
    }
    uri[uriLength-1] = '\0';
    snprintf(uri, uriLength-1, "http://%s:%s%s%s", server, portString, baseUriLength ? baseUriCopy : "", fileName);
    BDBG_MSG(("%s: server %s port %s base uri %s, file %s, Absolute uri %s", __FUNCTION__, server, portString, baseUriLength ? baseUri : "", fileName, uri));
    if (baseUriCopy) 
        BKNI_Free(baseUriCopy);
    return uri;
}

/* download file from the current network socket into the specified buffer and return the amount of bytes read. */
/* read until EOF, error condition, or channel change (state change) occurs */
bool
B_PlaybackIp_MpegDashDownloadFile(B_PlaybackIpHandle playback_ip, int fd, char *buffer, int bufferSize, ssize_t *totalBytesRead, bool nullTerminate)
{
    ssize_t bytesRead = 0;
    int bytesToRead;
    bool serverClosed = false;

    BDBG_ENTER(B_PlaybackIp_MpegDashDownloadFile);

    BDBG_MSG(("%s: start downloading file, currently read %d", __FUNCTION__, *totalBytesRead));
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            BDBG_MSG(("%s: breaking file download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }

        if (playback_ip->contentLength > 0 && *totalBytesRead == (int)playback_ip->contentLength) {
            /* we have read all the bytes that server had indicated via contentLength, so instead of trying another read and waiting for server to close the connection */
            /* consider this as server closed event and break out of the read loop */
            BDBG_MSG(("%s: breaking out of read loop as we have read %d upto the content length %lld", __FUNCTION__, *totalBytesRead, playback_ip->contentLength));
            serverClosed = true;
            break;
        }
        bytesToRead = bufferSize > MPEG_DASH_READ_CHUNK_SIZE ? MPEG_DASH_READ_CHUNK_SIZE : bufferSize;
        /* make sure there is enough space in the read buffer */
        if ((*totalBytesRead + bytesToRead) > bufferSize) {
            BDBG_MSG(("%s: need bigger buffer to hold the complete downloaded file: totalBytesRead %d, size %d, returning what is read", __FUNCTION__, *totalBytesRead, bufferSize));
            break;
        }

        if ((bytesRead = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, fd, buffer+*totalBytesRead, bytesToRead, playback_ip->networkTimeout)) <= 0) {
            if (playback_ip->selectTimeout) {
                BDBG_MSG(("%s: socket error, retry read: size %d, errno :%d, state %d, select timeout %d, server closed %d",
                    __FUNCTION__, *totalBytesRead+bytesRead, errno, playback_ip->playback_state, playback_ip->selectTimeout, playback_ip->serverClosed));
                continue;
            }
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
            BDBG_ERR(("%s: Network Read Error, rc %d, playback ip state %d", __FUNCTION__, bytesRead, playback_ip->playback_state));
#endif
            serverClosed = true;
            break;
        }

        /* read some data, increament the index count and make sure there is space left in the index cache for the next read */
        *totalBytesRead += bytesRead;
    }
    BDBG_MSG(("%s: finished downloading file: errno %d, size %d, state %d, server closed %d on socket %d", __FUNCTION__, errno, *totalBytesRead, playback_ip->playback_state, serverClosed, fd));
    if (serverClosed) {
        /* close security context and socket */
        if (playback_ip->securityHandle) {
            playback_ip->netIo.close(playback_ip->securityHandle);
            playback_ip->securityHandle = NULL;
        }
        close(fd);
    }

    /* downloaded the file into the buffer, return success */
    if (*totalBytesRead) {
        if (*totalBytesRead < bufferSize) {
            if (nullTerminate)
                buffer[*totalBytesRead] = '\0'; /* null terminate the read playlist */
        }
        else {
            BDBG_ERR(("%s: increase the file buffer size from %d, it is not big enough", __FUNCTION__, bufferSize));
            BDBG_LEAVE(B_PlaybackIp_MpegDashDownloadFile);
            return false;
        }
        BDBG_LEAVE(B_PlaybackIp_MpegDashDownloadFile);
        return true;
    }
    else
    {
        return false;
        BDBG_LEAVE(B_PlaybackIp_MpegDashDownloadFile);
    }
}



/* download data from the current network socket into the specified buffer and return the amount of bytes read. */
/* read until EOF, error condition, or channel change (state change) occurs */
/* in addition, also measure the network bandwidth while downloading this data. */
/* current socket (accessed via playback_ip struct) can correspond to playlist file or actual media segment */
bool
B_PlaybackIp_MpegDashDownloadMediaSegment(
    B_PlaybackIpHandle playback_ip, 
    int fd, 
    char *buffer, 
    int bufferSize, 
    ssize_t *totalBytesRead, 
    unsigned *networkBandwidth, 
    bool *serverClosed
    )
{
    ssize_t bytesRead = 0;
    B_Time beginTime, endTime;
    unsigned int totalDownloadTime;

    *serverClosed = false;

    BDBG_ENTER(B_PlaybackIp_MpegDashDownloadMediaSegment);

    BDBG_MSG(("%s: start downloading file: contentLength %lld, uri %s", __FUNCTION__, playback_ip->contentLength, playback_ip->openSettings.socketOpenSettings.url));
    /* start a timer to note the n/w b/w */
    B_Time_Get(&beginTime);
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            BDBG_MSG(("%s: breaking file download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }

        if (playback_ip->contentLength > 0 && *totalBytesRead == (int)playback_ip->contentLength) {
            /* we have read all the bytes that server had indicated via contentLength, so instead of trying another read and waiting for server to close the connection */
            /* consider this as server closed event and break out of the read loop */
            BDBG_MSG(("%s: breaking out of read loop as we have read %d upto the content length %lld", __FUNCTION__, *totalBytesRead, playback_ip->contentLength));
            *serverClosed = true;
            break;
        }

        /* make sure there is enough space in the read buffer */
        if ((*totalBytesRead + MPEG_DASH_READ_CHUNK_SIZE) > bufferSize) {
            BDBG_MSG(("%s: need bigger buffer to hold the complete downloaded file: totalBytesRead %d, size %d, returning what is read", __FUNCTION__, *totalBytesRead, bufferSize));
            break;
        }

        if ((bytesRead = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, fd, buffer+*totalBytesRead, MPEG_DASH_READ_CHUNK_SIZE, playback_ip->networkTimeout)) <= 0) {
            if (playback_ip->selectTimeout) {
                BDBG_MSG(("%s: socket error, retry read: size %d, errno :%d, state %d, select timeout %d, server closed %d",
                    __FUNCTION__, *totalBytesRead+bytesRead, errno, playback_ip->playback_state, playback_ip->selectTimeout, playback_ip->serverClosed));
                continue;
            }
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
            BDBG_ERR(("%s: Network Read Error, rc %d, playback ip state %d", __FUNCTION__, bytesRead, playback_ip->playback_state));
#endif
            *serverClosed = true;
            break;
        }

        /* read some data, increament the index count and make sure there is space left in the index cache for the next read */
        *totalBytesRead += bytesRead;
    }

    if (*serverClosed == true) {
        /* close security context and socket */
        if (playback_ip->securityHandle) {
            playback_ip->netIo.close(playback_ip->securityHandle);
            playback_ip->securityHandle = NULL;
        }
        close(fd);
    }

    B_Time_Get(&endTime);
    totalDownloadTime = B_Time_Diff(&endTime, &beginTime);
    if (totalDownloadTime) {
        unsigned modifiedNetworkBandwidth;
        *networkBandwidth = ((int)((*totalBytesRead*8.)/(totalDownloadTime))*1000);
        modifiedNetworkBandwidth = (*networkBandwidth * 80)/100;
        /* 1000 for convering msec to sec, 80/100 for taking 80% of current n/w b/w */
        BDBG_MSG(("%s: download time (msec) %u, total read %ld, b/w %u, modified b/w %u", __FUNCTION__, totalDownloadTime, *totalBytesRead, *networkBandwidth, modifiedNetworkBandwidth));
        *networkBandwidth = modifiedNetworkBandwidth;
    }
    BDBG_MSG(("%s: finished downloading file (fd %d): n/w bandwidth %d, errno %d, size %d, state %d, select timeout %d, server closed %d",
            __FUNCTION__, fd, *networkBandwidth, errno, *totalBytesRead, playback_ip->playback_state, playback_ip->selectTimeout, playback_ip->serverClosed));

    /* downloaded the file into the buffer, now check if we need to decrypt this segment */

    BDBG_LEAVE(B_PlaybackIp_MpegDashDownloadMediaSegment);
    if (*totalBytesRead)
        return true;
    else
        return false;
}




void
B_PlaybackIp_MpegDashFreeMediaFileSegmentInfo(MediaFileSegmentInfo *mediaFileSegmentInfo)
{
    if (mediaFileSegmentInfo->absoluteUri) {
        free(mediaFileSegmentInfo->absoluteUri);
        mediaFileSegmentInfo->absoluteUri = NULL;
    }
    if (mediaFileSegmentInfo->server) {
        BKNI_Free(mediaFileSegmentInfo->server);
        mediaFileSegmentInfo->server = NULL;
    }
    BDBG_MSG(("%s: freed mediaFileSegmentInfo entry %p", __FUNCTION__, mediaFileSegmentInfo));
    BKNI_Free(mediaFileSegmentInfo);
}

void
B_PlaybackIp_MpegDashFreeMediaFileSegmentInfoAll(struct MediaFileSegmentInfoQueue *mediaFileSegmentInfoQueueHead)
{
    MediaFileSegmentInfo *mediaFileSegmentInfo, *tmpPtr;
    mediaFileSegmentInfo = BLST_Q_FIRST(mediaFileSegmentInfoQueueHead);
    while (mediaFileSegmentInfo != NULL) {
        tmpPtr = BLST_Q_NEXT(mediaFileSegmentInfo, next);
        BLST_Q_REMOVE(mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
        B_PlaybackIp_MpegDashFreeMediaFileSegmentInfo(mediaFileSegmentInfo);
        mediaFileSegmentInfo = tmpPtr;
    }

    BDBG_MSG(("%s: done", __FUNCTION__));
}

void
B_PlaybackIp_MpegDashFreePlaylistInfo(PlaylistFileInfo *playlistFileInfo)
{

    B_PlaybackIp_MpegDashFreeMediaFileSegmentInfoAll(&playlistFileInfo->mediaFileSegmentInfoQueueHead);

    if (playlistFileInfo->absoluteUri) {
        free(playlistFileInfo->absoluteUri);
        playlistFileInfo->absoluteUri = NULL;
    }
    if (playlistFileInfo->server) {
        BKNI_Free(playlistFileInfo->server);
        playlistFileInfo->server = NULL;
    }
    BDBG_MSG(("%s: freed playlistFileInfo entry %p", __FUNCTION__, playlistFileInfo));
    BKNI_Free(playlistFileInfo);
}

void
B_PlaybackIp_MpegDashFreePlaylistInfoAll(B_PlaybackIpHandle playback_ip)
{
    PlaylistFileInfo *playlistFileInfo, *tmpPtr;
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;
    
    playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
    while (playlistFileInfo != NULL) {
        tmpPtr = BLST_Q_NEXT(playlistFileInfo, next);
        BLST_Q_REMOVE(&mpegDashSession->playlistFileInfoQueueHead, playlistFileInfo, next);
        B_PlaybackIp_MpegDashFreePlaylistInfo(playlistFileInfo);
        playlistFileInfo = tmpPtr;
    }

    if (mpegDashSession->playlistBuffer) {
        free(mpegDashSession->playlistBuffer);
        mpegDashSession->playlistBuffer = NULL;
    }
    BDBG_MSG(("%s: done", __FUNCTION__));
}

bool 
mpegDash_parse_url(B_PlaybackIpProtocol *protocol, char **server, unsigned *portPtr, char **uri, char *absoluteUri)
{
    char *tmp1, *tmp2, *tmp3;

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(absoluteUri, "http://"))) {
        *protocol = B_PlaybackIpProtocol_eHttp;
        tmp1 += strlen("http://");
        *portPtr = 80;
    }
    else if ( (tmp1 = B_PlaybackIp_UtilsStristr(absoluteUri, "https://"))) {
        *protocol = B_PlaybackIpProtocol_eHttps;
        tmp1 += strlen("https://");
        *portPtr = 443;
    }
    else {
        BDBG_ERR(("%s: unsupported protocol in the given URL %s", __FUNCTION__, absoluteUri));
        return false;
    }
    /* http protocol is being used, parse it further */

    /* now take out the server string from the url */
    tmp2 = strstr(tmp1, "/");
    if (tmp2) {
        if ((*server = (char *)BKNI_Malloc(tmp2-tmp1+1)) == NULL) {
            BDBG_ERR(("%s: ERROR: failed to allocate %d bytes of memory at %d\n", __FUNCTION__, (tmp2-tmp1), __LINE__));
            return false;
        }
        strncpy(*server, tmp1, tmp2-tmp1);
        (*server)[tmp2-tmp1] = '\0';

        /* Check to see if a port value was specified */
        tmp3 = strstr(*server, ":");
        if (tmp3) {
            tmp3[0] = '\0'; /* this null terminates the server name string */
            tmp3++;
            *portPtr = strtoul(tmp3, (char **) NULL, 10);
        }

        /* now get the uri */
        *uri = tmp2;
        BDBG_MSG(("%s: server %s, port %d, protocol %d, url %s", __FUNCTION__, *server, *portPtr, *protocol, *uri));
        return true;
    }
    else {
        BDBG_ERR(("%s: Incorrect URL: Failed to find the server part in %s", __FUNCTION__, absoluteUri));
        return false;
    }
}

/* builds HTTP get request */
/* TODO: reuse the function from b_playback_ip_http.c */
static void 
mpegDash_build_get_req(B_PlaybackIpHandle playback_ip, char *write_buf, int write_buf_size, char *server, int port, char *uri)
{
    snprintf(write_buf, write_buf_size,
            "GET %s HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "User-Agent: %s\r\n"
            "Connection: Close\r\n"
            "EncEnabled: No\r\n"
            "transferMode.dlna.org: Streaming\r\n"
            "\r\n"
            ,uri, server, port,
            (playback_ip->openSettings.u.http.userAgent ? playback_ip->openSettings.u.http.userAgent : "BRCM HTTP Client/2.0")
            );
}

void
B_Playback_MpegDashDestroyPlaybackIpSession(
    B_PlaybackIpHandle playbackIp
    )
{
    BDBG_ENTER(B_Playback_MpegDashDestroyPlaybackIpSession);

    if (!playbackIp)
    {
        BDBG_LEAVE(B_Playback_MpegDashDestroyPlaybackIpSession);
        return;
    }

    if (B_PlaybackIp_SessionClose(playbackIp)) {
        BDBG_ERR(("%s: B_PlaybackIp_SessionClose() Failed", __FUNCTION__));
        BDBG_LEAVE(B_Playback_MpegDashDestroyPlaybackIpSession);
        return;
    }

    if (B_PlaybackIp_Close(playbackIp)) {
        BDBG_ERR(("%s: B_PlaybackIp_Close() Failed", __FUNCTION__));
        BDBG_LEAVE(B_Playback_MpegDashDestroyPlaybackIpSession);
        return;
    }
    BDBG_LEAVE(B_Playback_MpegDashDestroyPlaybackIpSession);
}


static B_PlaybackIpHandle 
B_Playback_MpegDashCreatePlaybackIpSession(
    B_PlaybackIpHandle playback_ip,
    char *server,
    unsigned port,
    char **uri,
    B_PlaybackIpProtocol protocol,
    void *initialSecurityContext
    )
{
    B_PlaybackIpSessionOpenSettings ipSessionOpenSettings;
    B_PlaybackIpSessionOpenStatus ipSessionOpenStatus;
    NEXUS_Error rc = NEXUS_UNKNOWN;
    B_PlaybackIpHandle playbackIp;

    BDBG_ENTER(B_Playback_MpegDashCreatePlaybackIpSession);

    BDBG_MSG (("%s: server %s, port %d, uri %s, protocol %d", __FUNCTION__, server, port, *uri, protocol));
    playbackIp = B_PlaybackIp_Open(NULL);
    if (!playbackIp) {BDBG_ERR(("%s: Failed to open a Playback Ip Session", __FUNCTION__)); return NULL;}

    memset(&ipSessionOpenSettings, 0, sizeof(ipSessionOpenSettings));
    memset(&ipSessionOpenStatus, 0, sizeof(ipSessionOpenStatus));
    if (protocol == B_PlaybackIpProtocol_eHttps) {
        if (initialSecurityContext == NULL) {
            BDBG_ERR(("%s: ERROR: App needs to call B_PlaybackIp_SslInit() and pass in the returned context in ipSessionOpenSettings.security.initialSecurityContext", __FUNCTION__));
            goto error;
        }
        ipSessionOpenSettings.security.initialSecurityContext = initialSecurityContext;
        ipSessionOpenSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
    }
    /* Set IP Address, Port, and Protocol used to receive the AV stream */
    strncpy(ipSessionOpenSettings.socketOpenSettings.ipAddr, server, sizeof(ipSessionOpenSettings.socketOpenSettings.ipAddr)-1);
    ipSessionOpenSettings.socketOpenSettings.port = port;
    ipSessionOpenSettings.socketOpenSettings.protocol = B_PlaybackIpProtocol_eHttp;
    ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
    ipSessionOpenSettings.socketOpenSettings.url = *uri;
    ipSessionOpenSettings.u.http.userAgent = playback_ip->openSettings.u.http.userAgent;
    rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
    if (rc != B_ERROR_SUCCESS) {
        BDBG_ERR(("Session Open call failed: rc %d, HTTP Status %d\n", rc, ipSessionOpenStatus.u.http.statusCode));
        goto error;
    }
    BDBG_MSG (("Session Open call succeeded, HTTP status code %d, server %s, port %d, uri %s, protocol %d", ipSessionOpenStatus.u.http.statusCode, server, port, uri, protocol));

    BDBG_LEAVE(B_Playback_MpegDashCreatePlaybackIpSession);
    return playbackIp;

error:
    B_Playback_MpegDashDestroyPlaybackIpSession(playbackIp);
    BDBG_LEAVE(B_Playback_MpegDashCreatePlaybackIpSession);
    return NULL;
}



/* setup HTTP Session, Open & Enable Security Context and get mpegDash session ready for media Segment download */
bool
B_PlaybackIp_MpegDashSetupHttpSessionToServer(
    B_PlaybackIpHandle playback_ip, 
    MediaFileSegmentInfo *mediaFileSegmentInfo,
    int *socketFd,
    char *buffer,
    int *bufferDepth
    )
{
    bool rc = false;
    char *server;
    unsigned port;
    char **uri;
    char *requestMessage, *responseMessage;
    char *http_hdr, *http_payload;
    http_url_type_t http_url_type;
    char *serverRedirect = NULL;
	char *uriRedirect = NULL;
    B_PlaybackIpProtocol protocol;
    B_PlaybackIpSessionOpenSettings openSettings;

    BDBG_ENTER(B_PlaybackIp_MpegDashSetupHttpSessionToServer);

    if (!playback_ip || !mediaFileSegmentInfo)
    {
        BDBG_LEAVE(B_PlaybackIp_MpegDashSetupHttpSessionToServer);
        return false;
    }

    if (!playback_ip->openSettings.socketOpenSettings.useProxy) {
        server = mediaFileSegmentInfo->server;
        port = mediaFileSegmentInfo->port;
    }
    else {
        /* when proxy is enabled, connection request has to be sent to the proxy server */
        server = playback_ip->openSettings.socketOpenSettings.ipAddr;
        port = playback_ip->openSettings.socketOpenSettings.port;
    }
    protocol = mediaFileSegmentInfo->protocol;
    uri = &mediaFileSegmentInfo->uri;
    BDBG_MSG(("%s: URI: http://%s:%d%s", __FUNCTION__, server, port, *uri));

    /* reset previous content length */
    playback_ip->contentLength = 0;

    /* prepare initial Get request */
    responseMessage = (char *) BKNI_Malloc(TMP_BUF_SIZE+1);
    requestMessage = (char *)BKNI_Malloc(TMP_BUF_SIZE+1);
    if (!responseMessage || !requestMessage) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        goto error;
    }
    memset(&openSettings, 0, sizeof(openSettings));
    openSettings.security.securityProtocol = mediaFileSegmentInfo->securityProtocol;
    for (;;) {
        memset(requestMessage, 0, TMP_BUF_SIZE+1);
        memset(responseMessage, 0, TMP_BUF_SIZE+1);

        mpegDash_build_get_req(playback_ip, requestMessage, TMP_BUF_SIZE, server, port, *uri);

        /* setup the socket connection to the server & send GET request */
        if (B_PlaybackIp_UtilsTcpSocketConnect(server, port, false, socketFd) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to send Socket Connect Request to Server: %s:%d\n", __FUNCTION__, server, port));
            goto error;
        }

        /* now setup security context prior to downloading the media segment */
        /* currently, supported security protocols are: HTTPS (SSL/TLS), AES128, and Clear (no encryption) */
        /* Note: security protocol can change from segment to segment, so this function is called prior to each segment download */
        openSettings.security.initialSecurityContext = playback_ip->openSettings.security.initialSecurityContext; /* points to either AES (dmaHandle) or SSL initial security context */
#ifdef NEXUS_HAS_DMA
        openSettings.security.dmaHandle = playback_ip->openSettings.security.dmaHandle;
#endif
        switch (openSettings.security.securityProtocol) {
#ifdef B_HAS_HTTP_AES_SUPPORT
            case B_PlaybackIpSecurityProtocol_Aes128:
                /* setup the new key & iv */
                openSettings.security.enableDecryption = false;
                openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Aes128;
                memcpy(openSettings.security.settings.aes128.key, mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.key, sizeof(playback_ip->openSettings.security.settings.aes128.key));
                memcpy(openSettings.security.settings.aes128.iv, mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv, sizeof(playback_ip->openSettings.security.settings.aes128.iv));
                break;
#endif
            case B_PlaybackIpSecurityProtocol_Ssl:
                openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
                openSettings.security.enableDecryption = true;
                break;
            default:
                /* Setup clear path */
                openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_None;
        }
        if (B_PlaybackIp_SecuritySessionOpen(playback_ip, &openSettings, *socketFd, &playback_ip->securityHandle) < 0) {
            BDBG_ERR(("%s: ERROR: failed to setup the security session", __FUNCTION__));
            goto error;
        }

        /* and then send the HTTP Get request */
        if (playback_ip->netIo.write(playback_ip->securityHandle, &playback_ip->playback_state, *socketFd, requestMessage, strlen(requestMessage)) < 0) {
            BDBG_ERR(("%s: ERROR: failed to send HTTP Get request to Server: %s:%d\n", __FUNCTION__, server, port));
            goto error;
        }
        BDBG_MSG(("%s: Sent HTTP Get Request (socket %d) --->:\n %s", __FUNCTION__, *socketFd, requestMessage));

        playback_ip->chunkEncoding = false;
        playback_ip->serverClosed = false;
        playback_ip->selectTimeout = false;
        *bufferDepth = 0;

        /* now read and process the HTTP Response headers */
        if (http_read_response(playback_ip, playback_ip->securityHandle, *socketFd, &responseMessage, TMP_BUF_SIZE, &http_hdr, &http_payload, &playback_ip->initial_data_len) < 0) {
            BDBG_ERR(("%s: ERROR: failed to receive valid HTTP response\n", __FUNCTION__));
            goto error;
        }
        http_url_type = http_get_url_type(http_hdr, *uri);
        BDBG_MSG(("%s: http url type %d", __FUNCTION__, http_url_type));
        if (http_url_type == HTTP_URL_IS_REDIRECT) {
            /* parse HTTP redirect and extract the new URI & server:port info */
            if ((serverRedirect = (char *)BKNI_Malloc(512)) == NULL) {
                BDBG_ERR(("%s: failed to allocate memory for redirectServer", __FUNCTION__));
                goto error;
            }
            if (http_parse_redirect(serverRedirect, &port, &protocol, &uriRedirect, &playback_ip->cookieFoundViaHttpRedirect, http_hdr) != 0) {
                BDBG_ERR(("%s: Incorrect HTTP Redirect response or parsing error", __FUNCTION__));
                goto error;
            }
            /* previous function gets the new URL & server information and we send another GET request to this server */
            close(*socketFd);
            if (playback_ip->securityHandle)
                playback_ip->netIo.close(playback_ip->securityHandle);
            playback_ip->securityHandle = NULL;
            server = serverRedirect;
            uri = &uriRedirect;
            if (protocol == B_PlaybackIpProtocol_eHttps)
                openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: HTTP redirect case, caching the redirected URL: http://%s:%d%s", __FUNCTION__, server, port, *uri));
#endif
            continue;
        }
        else {
            /* actual content URL, get the content attributes from HTTP response header */
            BDBG_MSG(("%s: GOT ACTUAL CONTENT: sock fd %d", __FUNCTION__, *socketFd));
            break;
        }
    }

    /* store the content type of the payload, this assists in providing a valuble hint to media probe and thus cuts down the probe time */
    playback_ip->http_content_type = http_get_payload_content_type(responseMessage);

#ifdef B_HAS_HTTP_AES_SUPPORT
    if (openSettings.security.securityProtocol == B_PlaybackIpSecurityProtocol_Aes128) {
        /* enable AES encryption as HTTP header processing (which is in clear) is done now */
        if (B_PlaybackIp_AesDecryptionEnable(playback_ip->securityHandle, http_payload, playback_ip->initial_data_len) < 0) {
            BDBG_ERR(("%s: ERROR: failed to enable the security decryption", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: security context is enabled for media segment %p, seq # %d, sec protocol %d, initial encrypted bytes %d", __FUNCTION__, mediaFileSegmentInfo, mediaFileSegmentInfo->mediaSequence, openSettings.security.securityProtocol, playback_ip->initial_data_len));
        playback_ip->initial_data_len = 0; /* initial payload is now given to security layer for later decryption during the read call */
    }
#endif /* B_HAS_HTTP_AES_SUPPORT */
    if (playback_ip->initial_data_len) {
        *bufferDepth = playback_ip->initial_data_len;
        memcpy(buffer, http_payload, *bufferDepth);
        BDBG_MSG(("%s: completed, %d bytes of initial playlist read", __FUNCTION__, *bufferDepth));
    }
    rc = true;

error:
    if (serverRedirect) 
        BKNI_Free(serverRedirect);
	if (uriRedirect) 
        free(uriRedirect); /* allocated via strdup of redirected URI in http_parse_redirect */
    if (responseMessage)
        BKNI_Free(responseMessage);
    if (requestMessage)
        BKNI_Free(requestMessage);
    BDBG_LEAVE(B_PlaybackIp_MpegDashSetupHttpSessionToServer);
    return rc;
}



static const char  *B_PlaybackIp_MpegDash_P_CheckBdbgEnabled(bool *enabled) {
    *enabled = true;
     return "";
}

bool 
B_PlaybackIp_MpegDashParseMPD(B_PlaybackIpHandle playback_ip, PlaylistFileInfo *playlistFileInfo)
{
    int programId = -1;

    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
    int nextSeq = 0;

    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;

    B_PlaybackIp_XmlElement   xmlElemRoot                   = NULL;
    B_PlaybackIp_XmlElement   xmlElemMpd                    = NULL;
    B_PlaybackIp_XmlElement   xmlElemPeriod                 = NULL;
    B_PlaybackIp_XmlElement   xmlElemAdaptationSet          = NULL;
    B_PlaybackIp_XmlElement   xmlElemRepresentation         = NULL;
    B_PlaybackIp_XmlElement   xmlElemSegmentList            = NULL;
    B_PlaybackIp_XmlElement   xmlElemInitializationSegment  = NULL;
    B_PlaybackIp_XmlElement   xmlElemSegmentUrl             = NULL;
    B_PlaybackIp_XmlElement   xmlElemSegmentBase            = NULL;
    B_PlaybackIp_XmlElement   xmlElemBaseUrl                = NULL;
    const char  *pAttrValueString;
    const char  *pBaseUrl = "";


    BDBG_ENTER(B_PlaybackIp_MpegDashParseMPD);

    /* To determine if our debuglevel is enabled, we'll need to make two calls to BDBG_MODULE_MSG.  The first call
     * is to make sure that the module gets registered, then the second will only evaluate the argument list
     * if the BDBG level is enabled. 
     */ 
    BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("b_playback_ip_mpeg_dash_xml is enabled"));                                                      /* Make sure that bape_mixerprint is registered with BDBG   */
    BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("Printing MPD's XML information...%s", B_PlaybackIp_MpegDash_P_CheckBdbgEnabled(&g_xmlDebugEnabled)));  /* Set dbug_enabled if b_playback_ip_mpeg_dash_xml is enabled           */


    /* Parse the XML string and create a tree of XML objects. */
    xmlElemRoot = B_PlaybackIp_Xml_Create(mpegDashSession->playlistBuffer);
    PRINT_XML_DEBUG(("XML Root Node: %p", xmlElemRoot), xmlElemRoot );
    if (!xmlElemRoot)
    {
        BDBG_ERR(("%s: XML Parser Failed to parse the MPD file", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    /* Get the pointer to the "MPD" element under the root.*/
    xmlElemMpd = B_PlaybackIp_XmlElem_FindChild(xmlElemRoot , XML_TAG_MPD);
    PRINT_XML_DEBUG(("MPD Element: %p", xmlElemMpd), xmlElemMpd );
    if (!xmlElemMpd)
    {
        BDBG_ERR(("%s: Can't find top-level MPD element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    /* Get the "profiles" attribute from the MPD. */
    /* GARYWASHERE: Skip profile checking for now. */
    pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemMpd , XML_ATT_PROFILES);
    if (g_xmlDebugEnabled) {
        BDBG_LOG(("MPD Profiles: %p (%s)", pAttrValueString, pAttrValueString ));  
    }

    /*  Look for a BaseURL element under the MPD. */
    /*GARYWASHERE: Just use the first BaseURL for now. */
    xmlElemBaseUrl = B_PlaybackIp_XmlElem_FindChild(xmlElemMpd , XML_TAG_BASEURL);
    PRINT_XML_DEBUG(("BaseURL Element: %p", xmlElemBaseUrl), xmlElemBaseUrl );
    if (xmlElemBaseUrl)
    {
        pBaseUrl = B_PlaybackIp_XmlElem_ChildData(xmlElemBaseUrl);  

        /* GARYWASHERE: need to handle other baseURL attributes (@serviceLocation and @byteRange), also. */
    }

    /*  Now get the first "Period" under the "MPD". */
    /*GARYWASHERE: Just use the first Period for now. */
    xmlElemPeriod = B_PlaybackIp_XmlElem_FindChild(xmlElemMpd , XML_TAG_PERIOD);
    PRINT_XML_DEBUG(("Period Element: %p", xmlElemPeriod), xmlElemPeriod );
    if (!xmlElemPeriod)
    {
        BDBG_ERR(("%s: Can't find any Period elements under MPD element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    /*  Now get the first "AdaptationSet" under the "Period". */
    /* GARYWASHERE: Eventually, we may need multiple AdaptationSets... e.g., one for audio, one for video. */
    xmlElemAdaptationSet = B_PlaybackIp_XmlElem_FindChild(xmlElemPeriod , XML_TAG_ADAPTATIONSET);
    PRINT_XML_DEBUG(("AdaptationSet Element: %p", xmlElemAdaptationSet), xmlElemAdaptationSet );
    if (!xmlElemAdaptationSet)
    {
        BDBG_ERR(("%s: Can't find any AdaptationSet elements under Period element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    /* Get the "type" attribute from the MPD. */
    /* GARYWASHERE: Right now we can only handle static MPDs. */
    pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemMpd , XML_ATT_TYPE);
    if (g_xmlDebugEnabled)
    {
        BDBG_LOG(("MPD Type: %p (%s)", pAttrValueString, pAttrValueString ));  
    }

    if (strcmp(pAttrValueString, "static") == 0)
    {
        playlistFileInfo->bounded = true;
    }
    else
    {
        BDBG_ERR(("%s: MPD type is non-static!  Can't handle dynamic MPDs yet.", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    {
        B_PlaybackIp_XmlElement   xmlElemThisRep;
        unsigned                  thisRepsBandwidth = 0;

        xmlElemRepresentation = NULL;

        /* Now loop through each Representation in the AdaptationSet to find the best one. */
        xmlElemThisRep = B_PlaybackIp_XmlElem_FindChild(xmlElemAdaptationSet, XML_TAG_REPRESENTATION);
        for (;;) {
            if (!xmlElemThisRep) {
                break;
            }

            PRINT_XML_DEBUG(("Checking this Representation Element: %p", xmlElemThisRep), xmlElemThisRep );

            pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemThisRep , XML_ATT_BANDWIDTH);
            if (!pAttrValueString)
            {
                BDBG_ERR(("%s: Can't find a bandwidth attribute on Representation element", __FUNCTION__));
                BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
                return NULL;
            }
            thisRepsBandwidth = strtol(pAttrValueString, NULL, 10);
            if (thisRepsBandwidth == 0) {
                BDBG_ERR(("%s: invalid bandwidth (%d) in the Representation playlist, ignoring it", __FUNCTION__, thisRepsBandwidth));

            }

            BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: Bandwidth %d maxNetworkBandwidth %d playlistFileInfo bandwidth %d", __FUNCTION__, thisRepsBandwidth, mpegDashSession->maxNetworkBandwidth, playlistFileInfo->bandwidth));

            if (thisRepsBandwidth <= mpegDashSession->maxNetworkBandwidth  && 
                thisRepsBandwidth > playlistFileInfo->bandwidth ) {

                if (xmlElemRepresentation) {
                    PRINT_XML_DEBUG(("Disregarding this Representation Element: %p", xmlElemRepresentation), xmlElemRepresentation );
                }

                PRINT_XML_DEBUG(("Found better Representation Element: %p", xmlElemThisRep), xmlElemThisRep );

                xmlElemRepresentation = xmlElemThisRep;
                playlistFileInfo->bandwidth = thisRepsBandwidth;
            }
            else
            {
                if (g_xmlDebugEnabled)
                {
                    PRINT_XML_DEBUG(("Ignoring Representation Element: %p", xmlElemThisRep), xmlElemThisRep );
                }
            }

            playlistFileInfo->bandwidth = thisRepsBandwidth;

            /*  Now get the next "Representation" in the "AdaptationSet" (if there is one). */
            xmlElemThisRep = B_PlaybackIp_XmlElem_NextChild(xmlElemAdaptationSet );
            if (xmlElemThisRep)
            {
                xmlElemThisRep = B_PlaybackIp_XmlElem_FindNextChild(xmlElemAdaptationSet, xmlElemThisRep, XML_TAG_REPRESENTATION );
            }
        }

        if (!xmlElemRepresentation)
        {
            BDBG_ERR(("%s: Can't find any Representation elements under AdaptationSet element", __FUNCTION__));
            BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
            return NULL;
        }

        PRINT_XML_DEBUG(("Final choice for Representation:  %p", xmlElemRepresentation), xmlElemRepresentation );
    }

    pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemRepresentation , XML_ATT_ID);
    if (!pAttrValueString)
    {
        BDBG_ERR(("%s: Can't find an id attribute on Representation element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    playlistFileInfo->programId = strtol(pAttrValueString, NULL, 10);
    if (programId == -1)
        programId = playlistFileInfo->programId;
    else {
        if (programId != playlistFileInfo->programId) {
            BDBG_ERR(("%s: no support for multiple programs yet: Program ID current %d, new %d", __FUNCTION__, programId, playlistFileInfo->programId));
            /* TODO: goto free up current entry and move to the next program entry */
            goto error;
        }
    }

    BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: Program ID %d", __FUNCTION__, playlistFileInfo->programId));

    pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemRepresentation , XML_ATT_BANDWIDTH);
    if (!pAttrValueString)
    {
        BDBG_ERR(("%s: Can't find a bandwidth attribute on Representation element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }
    playlistFileInfo->bandwidth = strtol(pAttrValueString, NULL, 10);
    if (playlistFileInfo->bandwidth == 0) {
        BDBG_ERR(("%s: invalid bandwidth (%d) in the Representation playlist, ignoring it", __FUNCTION__, playlistFileInfo->bandwidth));
    }

    BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: Bandwidth %d", __FUNCTION__, playlistFileInfo->bandwidth));

    /* GARYWASHERE: Need to parse remaining Representation info here (bandwidth, width, height, codecs, etc.)*/

    /* Now look for the SegmentBase under the Representation. */
    xmlElemSegmentBase = B_PlaybackIp_XmlElem_FindChild(xmlElemRepresentation , XML_TAG_SEGMENTBASE);
    PRINT_XML_DEBUG(("SegmentBase Element: %p", xmlElemSegmentBase), xmlElemSegmentBase );

    /* If we have a SegmentBase, then look for an Initialization element under it. */
    if (xmlElemSegmentBase)
    {
        xmlElemInitializationSegment = B_PlaybackIp_XmlElem_FindChild(xmlElemSegmentBase , XML_TAG_INITIALIZATIONSEGMENT);
        PRINT_XML_DEBUG(("InitializationSegment Element (from SegmentBase): %p", xmlElemInitializationSegment), xmlElemInitializationSegment );
    }

    /* Now look for the SegmentList under the Representation. */
    xmlElemSegmentList = B_PlaybackIp_XmlElem_FindChild(xmlElemRepresentation , XML_TAG_SEGMENTLIST);
    PRINT_XML_DEBUG(("SegmentList element: %p", xmlElemSegmentList), xmlElemSegmentList );
    if (!xmlElemSegmentList)
    {
        BDBG_ERR(("%s: Can't find any SegmentLists under Representation element", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
        return NULL;
    }

    {
        B_PlaybackIp_XmlElement   xmlElemInitializationSegmentFromSegmentList = NULL;

        /* Look for an Iniitalization element for the SegmentList. */
        xmlElemInitializationSegmentFromSegmentList = B_PlaybackIp_XmlElem_FindChild(xmlElemSegmentList , XML_TAG_INITIALIZATIONSEGMENT);
        PRINT_XML_DEBUG(("InitializationSegment Element (from SegmentList): %p", xmlElemInitializationSegmentFromSegmentList), xmlElemInitializationSegmentFromSegmentList );

        if (xmlElemInitializationSegmentFromSegmentList) {
            xmlElemInitializationSegment = xmlElemInitializationSegmentFromSegmentList;
        }
    }

    /* Now loop through each SegmentURL in the SegmentList.     */
    /* But start with the initializaton segment if we have one. */
    if (xmlElemInitializationSegment) {
        xmlElemSegmentUrl = xmlElemInitializationSegment;
    }
    else {
        xmlElemSegmentUrl = B_PlaybackIp_XmlElem_FindChild(xmlElemSegmentList, XML_TAG_SEGMENTURL);
    }

    for (;;) {
        if (!xmlElemSegmentUrl) {
            break;
        }

        PRINT_XML_DEBUG(("SegmentUrl element: %p", xmlElemSegmentUrl), xmlElemSegmentUrl );

        /* allocate/initialize media file segment info structure */
        if ((mediaFileSegmentInfo = (MediaFileSegmentInfo *)BKNI_Malloc(sizeof(MediaFileSegmentInfo))) == NULL) {
            BDBG_WRN(("%s: Failed to allocate %d bytes of memory for media segment info structure", __FUNCTION__, sizeof(MediaFileSegmentInfo)));
            goto error;
        }
        BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: allocated %d bytes of memory for media segment info: %p", __FUNCTION__, sizeof(MediaFileSegmentInfo), mediaFileSegmentInfo));
        memset(mediaFileSegmentInfo, 0, sizeof(MediaFileSegmentInfo));
        playlistFileInfo->numMediaSegments++;
        if (playlistFileInfo->currentMediaFileSegment == NULL) {
            /* initialize the queue */
            BLST_Q_INIT(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
            playlistFileInfo->currentMediaFileSegment = mediaFileSegmentInfo;
            mediaFileSegmentInfo->mediaSequence = playlistFileInfo->mediaSegmentBaseSequence;
            nextSeq = playlistFileInfo->mediaSegmentBaseSequence + 1;
            BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: initialized head for the media segment info queue, base seq# %d, mediaFileSegmentInfo entry %p", __FUNCTION__, playlistFileInfo->mediaSegmentBaseSequence, mediaFileSegmentInfo));
        }
        else {
            BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: seq %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence));
            mediaFileSegmentInfo->mediaSequence = nextSeq++;
        }


        if (strcmp( B_PlaybackIp_XmlElem_GetTag(xmlElemSegmentUrl), XML_TAG_INITIALIZATIONSEGMENT) == 0) {
            pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemSegmentUrl, XML_ATT_SOURCEURL);
        }
        else
        {
            pAttrValueString = B_PlaybackIp_XmlElem_FindAttrValue(xmlElemSegmentUrl, XML_ATT_MEDIA);
        }

        if (!pAttrValueString)
        {
            BDBG_ERR(("%s: Can't find a media attribute on SegmentURL element", __FUNCTION__));
            goto error;
        }

        /* we have processed all tags in-between URL tag and actual URL, so now copy the actual URL */
        if (http_absolute_uri(  /*GARYWASHERE*/(char *)     pAttrValueString)) {
            /* mediaSegment URI is absolute uri */
            BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: media segment URI is absolute", __FUNCTION__));
            if ((mediaFileSegmentInfo->absoluteUri = strdup(pAttrValueString)) == NULL) {
                BDBG_ERR(("strdup failed at %s:%d", __FUNCTION__, __LINE__));
                goto error;
            }
        }
        else if (pBaseUrl && strlen(pBaseUrl) > 0) {

             if ((mpegDash_parse_url(&mediaFileSegmentInfo->protocol, &mediaFileSegmentInfo->server, &mediaFileSegmentInfo->port, &mediaFileSegmentInfo->uri, (char *)pBaseUrl) == false) || (mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttp && mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttps)) {
                 BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
                 goto error;
             }

             BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: Using BaseUrl: %s", __FUNCTION__, pBaseUrl ));
             if ((mediaFileSegmentInfo->absoluteUri = B_PlaybackIp_MpegDashBuildAbsoluteUri(mediaFileSegmentInfo->server, mediaFileSegmentInfo->port, mediaFileSegmentInfo->uri, /*GARYWASHERE*/ (char*)     pAttrValueString)) == NULL) {
                 BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
                 goto error;
             }

             BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: Built AbsoluteUri: %s", __FUNCTION__, mediaFileSegmentInfo->absoluteUri ));
        }
        else {
            /* relative url, build complete uri using server ip address & port # */
            BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: media segment URI is not absolute URI", __FUNCTION__));
            if ((mediaFileSegmentInfo->absoluteUri = B_PlaybackIp_MpegDashBuildAbsoluteUri(playlistFileInfo->server, playlistFileInfo->port, playlistFileInfo->uri, /*GARYWASHERE*/ (char*)     pAttrValueString)) == NULL) {
                BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
                goto error;
            }
        }

        if (    (   mpegDash_parse_url(     &mediaFileSegmentInfo->protocol, 
                                            &mediaFileSegmentInfo->server, 
                                            &mediaFileSegmentInfo->port, 
                                            &mediaFileSegmentInfo->uri, 
                                            mediaFileSegmentInfo->absoluteUri) == false
                )
                || 
                (
                    mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttp && 
                    mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttps
                )
            )
        {
            BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
            goto error;
        }

        if (mediaFileSegmentInfo->securityProtocol != B_PlaybackIpSecurityProtocol_None) {
            BDBG_ERR(("%s: Encryption Method %d is not yet supported", __FUNCTION__, mediaFileSegmentInfo->securityProtocol));
            goto error;
        }
        /* now insert this entry into the queue of media file segments */
        BLST_Q_INSERT_TAIL(&playlistFileInfo->mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
        mediaFileSegmentInfo = NULL; /* as this entry is now already inserted into the list of segment URIs */

        /*  If we just added an initialization segment, then grab the first segment from the segment list. */
        /*  Otherwise, get the next "SegmentURL" from the "SegmentList" (if there is one). */
        if (strcmp( B_PlaybackIp_XmlElem_GetTag(xmlElemSegmentUrl), XML_TAG_INITIALIZATIONSEGMENT) == 0) {
            xmlElemSegmentUrl = B_PlaybackIp_XmlElem_FindChild(xmlElemSegmentList, XML_TAG_SEGMENTURL);
        }
        else
        {
            xmlElemSegmentUrl = B_PlaybackIp_XmlElem_NextChild(xmlElemSegmentList );
            if (xmlElemSegmentUrl)
            {
                xmlElemSegmentUrl = B_PlaybackIp_XmlElem_FindNextChild(xmlElemSegmentList, xmlElemSegmentUrl, XML_TAG_SEGMENTURL );
            }
        }
    }

    /* now insert this entry into the queue of playlist files */
    BKNI_AcquireMutex(mpegDashSession->lock);
    B_PlaybackIp_MpegDashInsertPlaylistInfoEntry(mpegDashSession, playlistFileInfo);
    /* GARYWASHERE: This will need to be more intelligent later. */
    if ( ! mpegDashSession->currentPlaylistFile) {
        mpegDashSession->currentPlaylistFile = playlistFileInfo;
    }
    BKNI_ReleaseMutex(mpegDashSession->lock);

    /* GARYWASHERE: Don't forget to destroy the XML tree somewhere. */
    /* Destroy the tree of XML objects to free its memory. */
    B_PlaybackIp_Xml_Destroy( xmlElemRoot );
    xmlElemRoot = NULL;

    BDBG_MODULE_MSG(b_playback_ip_mpeg_dash_xml,("%s: finished parsing MPD file", __FUNCTION__));
    BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
    return true;

error:
    BDBG_LEAVE(B_PlaybackIp_MpegDashParseMPD);
    return false;
}



#ifdef MPEG_DASH_UNIT_TEST
static bool
printPlaylistFile(PlaylistFileInfo *playlistFileInfo)
{
    /* parse the common info first */
    
    MediaFileSegmentInfo *mediaFileSegmentInfo;
    
    BDBG_MSG(("playlist file: uri %s, version %d, program id %d, b/w %d, duration (msec): total %d, max %d, cur seq %d, bounded %d", 
                playlistFileInfo->uri,
                playlistFileInfo->version,
                playlistFileInfo->programId,
                playlistFileInfo->bandwidth,
                playlistFileInfo->totalDuration,
                playlistFileInfo->maxMediaSegmentDuration,
                playlistFileInfo->mediaSegmentBaseSequence,
                playlistFileInfo->bounded
                ));
    for (mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
         mediaFileSegmentInfo; 
         mediaFileSegmentInfo = BLST_Q_NEXT(mediaFileSegmentInfo, next)) 
    {

        BDBG_MSG(("media file segment info: uri %s, duration %d msec, title %s, date %s time %s, seq %d, marked discontinuity %d", 
                    mediaFileSegmentInfo->uri,
                    mediaFileSegmentInfo->duration,
                    mediaFileSegmentInfo->title,
                    mediaFileSegmentInfo->date,
                    mediaFileSegmentInfo->time,
                    mediaFileSegmentInfo->mediaSequence,
                    mediaFileSegmentInfo->markedDiscontinuity
                    ));
    }

    BDBG_MSG(("%s: finished printing playlist file", __FUNCTION__));

    return true;
}
#endif

/* remove current playlistFileInfoEntry in the list which is sorted (ascending order) by the b/w of the playlist entries */
/* called is required to lock the playlist */
void
B_PlaybackIp_MpegDashRemovePlaylistInfoEntry(MpegDashSessionState *mpegDashSession, PlaylistFileInfo *currentPlaylistFileInfo)
{
    PlaylistFileInfo *playlistFileInfo;
    BDBG_MSG(("%s: remove playlist entry %p", __FUNCTION__, currentPlaylistFileInfo));
    for (playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
         playlistFileInfo && playlistFileInfo != currentPlaylistFileInfo;
         playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next))
    { ; }
    if (playlistFileInfo) {
        /* entry found, remove it */
        BDBG_MSG(("%s: removing playlist entry: current %p, entry %p", __FUNCTION__, currentPlaylistFileInfo, playlistFileInfo));
        BLST_Q_REMOVE(&mpegDashSession->playlistFileInfoQueueHead, playlistFileInfo, next);
    }
    else {
        BDBG_ERR(("%s: SW BUG: entry to be removed %p not found in the list, current entry %p", __FUNCTION__, currentPlaylistFileInfo, playlistFileInfo));
        BDBG_ASSERT(NULL);
    }
}

/* insert the new the playlistFileInfoEntry in the list which is sorted (ascending order) by the b/w of the playlist entries */
/* caller is required to lock the playlist */
void
B_PlaybackIp_MpegDashInsertPlaylistInfoEntry(MpegDashSessionState *mpegDashSession, PlaylistFileInfo *newPlaylistFileInfo)
{
    PlaylistFileInfo *playlistFileInfo;
    BDBG_MSG(("%s: insert playlist entry %p in the ascending order of bandwidth (%d)", __FUNCTION__, newPlaylistFileInfo, newPlaylistFileInfo->bandwidth));
    for (playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
         playlistFileInfo;
         playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next))
    {

        BDBG_MSG(("%s: playlist file: uri %s, b/w %d", __FUNCTION__, playlistFileInfo->uri, playlistFileInfo->bandwidth));
        if (newPlaylistFileInfo->bandwidth < playlistFileInfo->bandwidth) {
            BDBG_MSG(("%s: new playlistInfo entry %p has bandwidth %d smaller than previous entry's %p bandwidth %d", __FUNCTION__, newPlaylistFileInfo, newPlaylistFileInfo->bandwidth, playlistFileInfo, playlistFileInfo->bandwidth)); 
            break;
        }
    }
    if (playlistFileInfo) {
        /* new entry's b/w is smaller than this entry's b/w, so insert it before this entry */
        BDBG_MSG(("%s: inserting playlist entry %p before entry %p", __FUNCTION__, newPlaylistFileInfo, playlistFileInfo));
        BLST_Q_INSERT_BEFORE(&mpegDashSession->playlistFileInfoQueueHead, playlistFileInfo, newPlaylistFileInfo, next);
    }
    else {
        /* insert it at the tail */
        BDBG_MSG(("%s: inserting playlist entry %p at the tail", __FUNCTION__, newPlaylistFileInfo));
        BLST_Q_INSERT_TAIL(&mpegDashSession->playlistFileInfoQueueHead, newPlaylistFileInfo, next);
    }
}


/* connect to the server, download the complete playlist file and then parse it to build the list of media segments */
int
B_PlaybackIp_MpegDashConnectDownloadAndParsePlaylistFile(
    B_PlaybackIpHandle playback_ip,
    MpegDashSessionState *mpegDashSession,
    PlaylistFileInfo *playlistFileInfo
    )
{
    B_PlaybackIpHandle playbackIp; /* temp playback ip session */

    BDBG_ENTER(B_PlaybackIp_MpegDashConnectDownloadAndParsePlaylistFile);

    /* Need to create a new temporary playback IP session as playlist may be downloaded using a different security parameters */
    /* than the current playback ip session (which corresponds to top level m3u8 URL) */
    playbackIp = B_Playback_MpegDashCreatePlaybackIpSession(playback_ip, playlistFileInfo->server, playlistFileInfo->port, &playlistFileInfo->uri, playlistFileInfo->protocol, playback_ip->openSettings.security.initialSecurityContext);
    if (!playbackIp) {
        goto error;
    }
    playbackIp->openSettings.u.http.userAgent = playback_ip->openSettings.u.http.userAgent;
    /* update its state to playing so that playback ip util functions can do HTTP processing (handle redirects, Http Chunk Xfer Encoding, etc.) and provide just the payload of HTTP */
    playbackIp->playback_state = B_PlaybackIpState_ePlaying;
    playbackIp->mpegDashSessionState = mpegDashSession;

    BDBG_MSG(("%s: Connected to the server using the new URL, now download the playlist, initial data %d", __FUNCTION__, playbackIp->initial_data_len));
    if (playbackIp->initial_data_len) {
        BKNI_Memcpy(mpegDashSession->playlistBuffer, playbackIp->temp_buf, playbackIp->initial_data_len);
        mpegDashSession->playlistBufferDepth = playbackIp->initial_data_len;
        playbackIp->initial_data_len = 0;
    }
    else {
        mpegDashSession->playlistBufferDepth = 0;
    }

    mpegDashSession->playlistSocketFd = playbackIp->socketState.fd;
    if (B_PlaybackIp_MpegDashDownloadFile(playbackIp, mpegDashSession->playlistSocketFd, mpegDashSession->playlistBuffer, mpegDashSession->playlistBufferSize, &mpegDashSession->playlistBufferDepth, true) != true) {
        BDBG_ERR(("%s: failed to download the M3U playlist file", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: downloaded playlist file of %d bytes", __FUNCTION__, mpegDashSession->playlistBufferDepth));

    mpegDashSession->playlistBufferReadIndex = 0;

    if (B_PlaybackIp_MpegDashParseMPD(playbackIp, playlistFileInfo) != true) {
        BDBG_ERR(("%s: failed to parse the MPD playlist file", __FUNCTION__));
        goto error;
    }

    /* now destroy this temp playback ip session */
    playbackIp->playback_state = B_PlaybackIpState_eOpened; /* change state back to Open */
    B_Playback_MpegDashDestroyPlaybackIpSession(playbackIp);

    BDBG_LEAVE(B_PlaybackIp_MpegDashConnectDownloadAndParsePlaylistFile);
    return 0;

error:
    /* destroy this temp playback ip session */
    if (playbackIp) {
        playbackIp->playback_state = B_PlaybackIpState_eOpened; /* change state back to Open */
        B_Playback_MpegDashDestroyPlaybackIpSession(playbackIp);
    }

    BDBG_LEAVE(B_PlaybackIp_MpegDashConnectDownloadAndParsePlaylistFile);
    return -1;
}


#ifdef MPEG_DASH_UNIT_TEST
static bool
printVariantPlaylistFile(MpegDashSessionState *mpegDashSession)
{
    /* parse the common info first */
    PlaylistFileInfo *playlistFileInfo;

    BDBG_MSG(("%s: playlist file q head %p, current playlist file %p", __FUNCTION__, &mpegDashSession->playlistFileInfoQueueHead, mpegDashSession->currentPlaylistFile));
    for (playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
         playlistFileInfo; 
         playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next)) 
    {

        BDBG_MSG(("%s: playlist file entry %p: uri %s, version %d, program id %d, b/w %d, duration (msec): total %d, max %d, cur seq %d, bounded %d, playlist file downloaded %s", 
                __FUNCTION__,
                playlistFileInfo,
                playlistFileInfo->uri,
                playlistFileInfo->version,
                playlistFileInfo->programId,
                playlistFileInfo->bandwidth,
                playlistFileInfo->totalDuration,
                playlistFileInfo->maxMediaSegmentDuration,
                playlistFileInfo->mediaSegmentBaseSequence,
                playlistFileInfo->bounded,
                playlistFileInfo->currentMediaFileSegment ? "YES":"NO"
                ));
        /* verify the playlist file */
        printPlaylistFile(playlistFileInfo);
    }

    BDBG_MSG(("%s: finished printing variant playlist file", __FUNCTION__));

    return true;
}
#endif



void 
B_PlaybackIp_MpegDashSegmentBufferDestroy(
    B_PlaybackIpHandle playback_ip
    )
{
    int i;
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;

    for (i=0; i<MPEG_DASH_NUM_SEGMENT_BUFFERS && mpegDashSession->segmentBuffer[i].buffer; i++) {
        if (mpegDashSession->segmentBuffer[i].lock) {
            BKNI_DestroyMutex(mpegDashSession->segmentBuffer[i].lock);
            mpegDashSession->segmentBuffer[i].lock = NULL;
        }
    }
}


void
B_PlaybackIp_MpegDashSessionDestroy(B_PlaybackIpHandle playback_ip)
{
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;

    BDBG_ENTER(B_PlaybackIp_MpegDashSessionDestroy);

    B_PlaybackIp_MpegDashFreePlaylistInfoAll(playback_ip);

    if (!mpegDashSession)
    {
        BDBG_LEAVE(B_PlaybackIp_MpegDashSessionDestroy);
        return;
    }

    if (mpegDashSession->bufferFilledEvent) {
        BKNI_DestroyEvent(mpegDashSession->bufferFilledEvent);
        mpegDashSession->bufferFilledEvent = NULL;
    }
    if (mpegDashSession->bufferEmptiedEvent) {
        BKNI_DestroyEvent(mpegDashSession->bufferEmptiedEvent);
        mpegDashSession->bufferEmptiedEvent = NULL;
    }
    if (mpegDashSession->reDownloadPlaylistEvent) {
        BKNI_DestroyEvent(mpegDashSession->reDownloadPlaylistEvent);
        mpegDashSession->reDownloadPlaylistEvent = NULL;
    }
    if (mpegDashSession->lock) {
        BKNI_DestroyMutex(mpegDashSession->lock);
        mpegDashSession->lock = NULL;
    }
    B_PlaybackIp_MpegDashSegmentBufferDestroy(playback_ip);

    if (mpegDashSession->playlistSocketFd) {
        close(mpegDashSession->playlistSocketFd);
        mpegDashSession->playlistSocketFd = 0;
    }

    if (mpegDashSession->playlistBuffer) {
        BKNI_Free(mpegDashSession->playlistBuffer);
        mpegDashSession->playlistBuffer = NULL;
    }

    BKNI_Free(mpegDashSession);
    playback_ip->mpegDashSessionState = NULL;

    BDBG_LEAVE(B_PlaybackIp_MpegDashSessionDestroy);
}




/* function to setup an MPEG-DASH Session: */
/*  -downloads the Media Presentation Description */
/*  -connects to the server for the 1st media segment and gets playback_ip session for media probing */
int
B_PlaybackIp_MpegDashSessionSetup(B_PlaybackIpHandle playback_ip, char *http_hdr)
{
    char *pValue;
    MediaFileSegmentInfo *mediaFileSegmentInfo;
    PlaylistFileInfo *playlistFileInfo;
    MpegDashSessionState *mpegDashSession; 
    int initial_data_len = 0;

    BDBG_ENTER(B_PlaybackIp_MpegDashSessionSetup);

    if (B_PlaybackIp_IsMpegDashSession(playback_ip->openSettings.socketOpenSettings.url, http_hdr) != true) {
        BDBG_MSG(("%s: Not an MPEG-DASH Streaming Session", __FUNCTION__));
        BDBG_LEAVE(B_PlaybackIp_MpegDashSessionSetup);
        return 0;
    }

    /* Now it is an MPEG-DASH session, do further checks to validate playlist file format */
    BDBG_MSG(("%s: MPEG-DASH Session: download & parse playlist", __FUNCTION__));

    /* allocate MPEG-DASH session state */
    if ((playback_ip->mpegDashSessionState = (MpegDashSessionState *)BKNI_Malloc(sizeof(MpegDashSessionState))) == NULL) {
        BDBG_ERR(("%s: failed to allocate %d bytes for MPEG-DASH Session state", __FUNCTION__, sizeof(MpegDashSessionState)));
        goto error;
    }
    mpegDashSession = playback_ip->mpegDashSessionState;
    BKNI_Memset(mpegDashSession, 0, sizeof(MpegDashSessionState));

    /* allocate a buffer where playlist file will be completely downloaded */
    if ((mpegDashSession->playlistBuffer = (char *)BKNI_Malloc(MPEG_DASH_PLAYLIST_FILE_SIZE)) == NULL) {
        BDBG_ERR(("%s: failed to allocate %d bytes for downloading the playlist file", __FUNCTION__, MPEG_DASH_PLAYLIST_FILE_SIZE));
        goto error;
    }
    mpegDashSession->playlistBufferSize = MPEG_DASH_PLAYLIST_FILE_SIZE;

    /* copy any initial payload data (read part of the initial HTTP response) into the playlist buffer */
    initial_data_len = playback_ip->chunkPayloadLength ? playback_ip->chunkPayloadLength : playback_ip->initial_data_len;
    if (initial_data_len) {
        memcpy(mpegDashSession->playlistBuffer, playback_ip->temp_buf, initial_data_len);
        mpegDashSession->playlistBufferDepth = initial_data_len;
        playback_ip->initial_data_len = 0;
        playback_ip->chunkPayloadLength = 0;
    }
    else {
        mpegDashSession->playlistBufferDepth = 0;
    }

    pValue = getenv("max_download_bitrate");
    if (pValue) {
        mpegDashSession->maxNetworkBandwidth = strtoul(pValue, NULL, 0);
    }
#define PLAYBACK_IP_MAX_NETWORK_BANDWIDTH 5800000 /* set to 1.8Mpbs */
    if (!pValue || mpegDashSession->maxNetworkBandwidth == 0) {
        mpegDashSession->maxNetworkBandwidth = PLAYBACK_IP_MAX_NETWORK_BANDWIDTH;
    }
    BDBG_MSG(("%s: max network bandwidth set to %d", __FUNCTION__, mpegDashSession->maxNetworkBandwidth));

    /* download the MPD file */
    if (B_PlaybackIp_MpegDashDownloadFile(playback_ip, playback_ip->socketState.fd, mpegDashSession->playlistBuffer, mpegDashSession->playlistBufferSize, &mpegDashSession->playlistBufferDepth, true) != true) {
        BDBG_ERR(("%s: failed to download the MPD file", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: downloaded playlist file of %d bytes", __FUNCTION__, mpegDashSession->playlistBufferDepth));
    mpegDashSession->playlistBufferReadIndex = 0;

#ifdef MPEG_DASH_UNIT_TEST
    dump_playlist_file(mpegDashSession);
#endif

    if (BKNI_CreateMutex(&mpegDashSession->lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    /* Create a new playlistFileInfo struct here, so we can begin to populate it. */
    if ((playlistFileInfo = (PlaylistFileInfo *)BKNI_Malloc(sizeof(PlaylistFileInfo))) == NULL) {
        BDBG_ERR(("%s: Failed to allocate %d bytes of memory for playlistInfo file structure", __FUNCTION__, sizeof(PlaylistFileInfo)));
        goto error;
    }
    memset(playlistFileInfo, 0, sizeof(PlaylistFileInfo));

    /* build the uri of the playlist file */
    if ((playlistFileInfo->absoluteUri = B_PlaybackIp_MpegDashBuildAbsoluteUri(playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, ""/* empty base uri*/, playback_ip->openSettings.socketOpenSettings.url)) == NULL) {
        BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
        goto error;
    }
    if ((mpegDash_parse_url(&playlistFileInfo->protocol, &playlistFileInfo->server, &playlistFileInfo->port, &playlistFileInfo->uri, playlistFileInfo->absoluteUri) == false) || playlistFileInfo->protocol != B_PlaybackIpProtocol_eHttp) {
        BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
        goto error;
    }

    /* initialize the queue */
    BLST_Q_INIT(&mpegDashSession->playlistFileInfoQueueHead);
    /* in the case of simple playlist file, it will only have element */

    if (!B_PlaybackIp_MpegDashParseMPD(playback_ip, playlistFileInfo)) {
        BDBG_ERR(("%s: Failed to parse MPD file", __FUNCTION__));
        goto error;
    }

#ifdef MPEG_DASH_UNIT_TEST
    printVariantPlaylistFile(mpegDashSession);
#endif
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: finished parsing the MPD file, now setup for media probing", __FUNCTION__));
#endif

    /* at this time,  playlists (both variant & simple ones) are downloaded & parsed, so lets download the 1st segment and find out its media PSI info */

    /* for the chosen playlist, take the 1st media segment URL and connect to that server so that we can probe its media details */
    mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
    while (mediaFileSegmentInfo) {
        if (B_PlaybackIp_MpegDashSetupHttpSessionToServer(playback_ip, mediaFileSegmentInfo, &playback_ip->socketState.fd, playback_ip->temp_buf, &playback_ip->initial_data_len) == false) {
            BDBG_ERR(("%s: ERROR: Socket setup or HTTP request/response failed for downloading Media Segment, retrying next media segment", __FUNCTION__));
            /* remove this media segment as we met some error in downloading this segment (most likely segment URL has expired) */
            BLST_Q_REMOVE(&playlistFileInfo->mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
            B_PlaybackIp_MpegDashFreeMediaFileSegmentInfo(mediaFileSegmentInfo);
            mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
            continue;
        }
        /* successfully connected & got the GET request/response from the server, so move on */
        break;
    }
    if (mediaFileSegmentInfo == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to download any media segment of playlist file (uri %s) for media probe", __FUNCTION__, playlistFileInfo->absoluteUri));
        goto error;
    }
    
    /* reset the content length, otherwise media probe takes way too long */
    /* TODO: need to see if this is needed or psiParsingTimeLimit will do the trick */
    playback_ip->contentLength = 0;

    if (BKNI_CreateEvent(&mpegDashSession->bufferEmptiedEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&mpegDashSession->bufferFilledEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&mpegDashSession->reDownloadPlaylistEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    playback_ip->mpegDashSessionEnabled = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: Complete, now media probing will start.", __FUNCTION__));
#endif

    /* the actual media probe operation happens in the caller function of http module */
    BDBG_LEAVE(B_PlaybackIp_MpegDashSessionSetup);
    return 0;

error:
    B_PlaybackIp_MpegDashSessionDestroy(playback_ip);
    BDBG_LEAVE(B_PlaybackIp_MpegDashSessionSetup);
    return -1;
}


int 
B_PlaybackIp_MpegDashSegmentBufferSetup(
    B_PlaybackIpHandle playback_ip
    )
{
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;

    BDBG_MSG(("%s: using index & data cache for downloading the consecutive media segments", __FUNCTION__));
    /* use index & data cache for downloading the consecutive media segments */
    mpegDashSession->segmentBuffer[0].filled = false;
    mpegDashSession->segmentBuffer[0].buffer = playback_ip->indexCache;
    mpegDashSession->segmentBuffer[0].bufferSize = playback_ip->indexCacheSize;
    mpegDashSession->segmentBuffer[0].bufferDepth = 0;
    if (BKNI_CreateMutex(&mpegDashSession->segmentBuffer[0].lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    mpegDashSession->segmentBuffer[1].filled = false;
    mpegDashSession->segmentBuffer[1].buffer = playback_ip->dataCache[0].cache;
    mpegDashSession->segmentBuffer[1].bufferSize = playback_ip->dataCache[0].size;
    mpegDashSession->segmentBuffer[1].bufferDepth = 0;
    if (BKNI_CreateMutex(&mpegDashSession->segmentBuffer[1].lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    return 0;
error:
    return -1;
}

PlaylistFileInfo *
B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile(
    B_PlaybackIpHandle playback_ip,
    MpegDashSessionState *mpegDashSession, 
    PlaylistFileInfo *currentPlaylistFileInfo
    )
{
    PlaylistFileInfo *newPlaylistFileInfo = NULL;
    B_PlaybackIpProtocol protocol;

    BDBG_ENTER(B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile);

    BDBG_MSG(("%s: playlist %p, uri %p", __FUNCTION__, currentPlaylistFileInfo, currentPlaylistFileInfo->uri));
    /* allocate/initialize the new playlist file structure */
    if ((newPlaylistFileInfo = (PlaylistFileInfo *)BKNI_Malloc(sizeof(PlaylistFileInfo))) == NULL) {
        BDBG_ERR(("%s: Failed to allocate %d bytes of memory for playlistInfo file structure", __FUNCTION__, sizeof(PlaylistFileInfo)));
        goto error;
    }
    memset(newPlaylistFileInfo, 0, sizeof(PlaylistFileInfo));
    newPlaylistFileInfo->absoluteUri = strdup(currentPlaylistFileInfo->absoluteUri);
    newPlaylistFileInfo->protocol = currentPlaylistFileInfo->protocol;
    if (!newPlaylistFileInfo->absoluteUri) {
        BDBG_ERR(("%s: failed to duplicate uri %s", __FUNCTION__, currentPlaylistFileInfo->absoluteUri));
        goto error;
    }
    if ((mpegDash_parse_url(&protocol, &newPlaylistFileInfo->server, &newPlaylistFileInfo->port, &newPlaylistFileInfo->uri, newPlaylistFileInfo->absoluteUri) == false) || protocol != B_PlaybackIpProtocol_eHttp) {
        BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
        goto error;
    }
    newPlaylistFileInfo->bandwidth = currentPlaylistFileInfo->bandwidth;
    newPlaylistFileInfo->programId = currentPlaylistFileInfo->programId;

    /* reload the playlist */
    if (B_PlaybackIp_MpegDashConnectDownloadAndParsePlaylistFile(playback_ip, mpegDashSession, newPlaylistFileInfo) < 0) {
        BDBG_ERR(("%s: Failed to download & parse playlist file entry w/ uri %s", __FUNCTION__, newPlaylistFileInfo->uri));
        goto error;
    }

#ifdef MPEG_DASH_UNIT_TEST
    /* verify the playlist file */
    printPlaylistFile(newPlaylistFileInfo);
#endif
    BDBG_LEAVE(B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile);
    return newPlaylistFileInfo;

error:
    if (newPlaylistFileInfo)
        BKNI_Free(newPlaylistFileInfo);
    BDBG_LEAVE(B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile);
    return NULL;
}

void 
B_PlaybackIp_MpegDashReplacePlaylists(
    MpegDashSessionState *mpegDashSession, 
    PlaylistFileInfo *playlistFileInfo,
    PlaylistFileInfo *newPlaylistFileInfo
    )
{
    B_PlaybackIp_MpegDashRemovePlaylistInfoEntry(mpegDashSession, playlistFileInfo);
    B_PlaybackIp_MpegDashInsertPlaylistInfoEntry(mpegDashSession, newPlaylistFileInfo);
    B_PlaybackIp_MpegDashFreePlaylistInfo(playlistFileInfo);
#ifdef MPEG_DASH_UNIT_TEST
    printVariantPlaylistFile(mpegDashSession);
#endif
}



MediaFileSegmentInfo *
B_PlaybackIp_MpegDashGetNextMediaSegmentEntry(
    B_PlaybackIpHandle playback_ip,
    MpegDashSessionState *mpegDashSession, 
    int currentMediaSegmentSequenceNumber,
    unsigned networkBandwidth
    )
{
    bool useDifferentPlaylist = false;
    PlaylistFileInfo *playlistFileInfo;
    PlaylistFileInfo *prevPlaylistFileInfo = NULL;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
#if 0
#define MPEG_DASH_UNIT_TEST1
#endif
#ifdef MPEG_DASH_UNIT_TEST1
    static int lastPlaylistIdx = 0; static int i=0;
#endif

    BDBG_ENTER(B_PlaybackIp_MpegDashGetNextMediaSegmentEntry);

    if (mpegDashSession->resetPlaylist) {
        /* resetting a playlist, start from the 1st media segment */
        mpegDashSession->downloadedAllSegmentsInCurrentPlaylist = false;
        mpegDashSession->resetPlaylist = false;
        mediaFileSegmentInfo = BLST_Q_FIRST(&mpegDashSession->currentPlaylistFile->mediaFileSegmentInfoQueueHead);
        BDBG_MSG(("%s: Starting w/ 1st media segment: sequence # %d, current network bandwidth %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence, networkBandwidth));
        mpegDashSession->currentPlaylistFile->currentMediaFileSegment = mediaFileSegmentInfo;
        BDBG_LEAVE(B_PlaybackIp_MpegDashGetNextMediaSegmentEntry);
        return mediaFileSegmentInfo;
    }
    
    /* find a playlist entry whose n/w b/w matches the current n/w b/w */
    for (prevPlaylistFileInfo = NULL, playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
         playlistFileInfo;
         prevPlaylistFileInfo=playlistFileInfo, playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next)) 
    {
#ifdef MPEG_DASH_UNIT_TEST1
        {
            if (playlistFileInfo->bandwidth <= 64000)
                continue;
            if (i==lastPlaylistIdx) {
                lastPlaylistIdx = i+1;
                BDBG_WRN(("%s: UNIT TEST: using playlist w/ b/w %u > max network b/w %u", __FUNCTION__, playlistFileInfo->bandwidth, mpegDashSession->maxNetworkBandwidth));
                i = 0;
                break;
            }
            i++;
            continue;
        }
#endif
        BDBG_MSG(("%s: playlist file: uri %s, b/w %d, network Bandwidth %d", __FUNCTION__, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth));
        if (playlistFileInfo->bandwidth > mpegDashSession->maxNetworkBandwidth) {
            BDBG_WRN(("%s: using playlist w/ b/w %u > max network b/w %u", __FUNCTION__, playlistFileInfo->bandwidth, mpegDashSession->maxNetworkBandwidth));
            break;
        }
        if (playlistFileInfo->bandwidth > networkBandwidth ) {
            /* the list of playlist file entries is created in the ascending order of their bandwidth value */
            BDBG_MSG(("%s: found playlist entry whose b/w %d exceeds the network b/w %d, use previous playlist entry", __FUNCTION__, playlistFileInfo->bandwidth, networkBandwidth));
            break;
        }
    }

    if (prevPlaylistFileInfo == NULL) {
        BDBG_MSG(("%s: using 1st playlist file entry %p, uri %s, b/w %d, network b/w %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth));
    }
#ifdef MPEG_DASH_UNIT_TEST1
    else if (playlistFileInfo == NULL) {
        playlistFileInfo = BLST_Q_FIRST(&mpegDashSession->playlistFileInfoQueueHead);
        BDBG_MSG(("%s: using 1st playlist file entry %p, uri %s, b/w %d, network b/w %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth));
        i = 0;
        lastPlaylistIdx = 1;
    }
#endif
    else {
        playlistFileInfo = prevPlaylistFileInfo;
        BDBG_MSG(("%s: using playlist file entry %p, uri %s, b/w %d, network b/w %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth));
    }

    if (playlistFileInfo == NULL) {
        BDBG_ERR(("%s: SW Bug: playlist list seems to be corrupted", __FUNCTION__));
        goto error;
    }

    /* since all playlists are not downloaded upfront, anytime we change the playlist (whether for live or bounded streams), we have to download this new playlist */
    if (playlistFileInfo != mpegDashSession->currentPlaylistFile) {
        PlaylistFileInfo *newPlaylistFileInfo;
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: switch playlist due to n/w b/w change, playlist file entry %p, uri %s, b/w %d, network b/w %d, bounded %d", 
                    __FUNCTION__, playlistFileInfo, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth, playlistFileInfo->bounded));
#endif
        if ((newPlaylistFileInfo = B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile(playback_ip, mpegDashSession, playlistFileInfo)) == NULL) {
            BDBG_ERR(("%s: B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile() failed", __FUNCTION__));
            goto error;
        }
        B_PlaybackIp_MpegDashReplacePlaylists(mpegDashSession, playlistFileInfo, newPlaylistFileInfo);
        playlistFileInfo = newPlaylistFileInfo;
        mpegDashSession->currentPlaylistFile = playlistFileInfo;
        useDifferentPlaylist = true;
    }
    if (useDifferentPlaylist == true || mpegDashSession->currentPlaylistFile->playlistHasChanged == true) {
        BDBG_MSG(("%s: find next media segment entry w/ seq # > %d due to %s", 
                    __FUNCTION__, currentMediaSegmentSequenceNumber,
                    (mpegDashSession->currentPlaylistFile->playlistHasChanged? "Playlist update":"n/w b/w change")));
        mpegDashSession->currentPlaylistFile->playlistHasChanged = false;
        for (mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
             mediaFileSegmentInfo; 
             mediaFileSegmentInfo = BLST_Q_NEXT(mediaFileSegmentInfo, next)) 
        {
            if (mediaFileSegmentInfo->mediaSequence > currentMediaSegmentSequenceNumber) {
                BDBG_MSG(("%s: Found next media segment sequence order, current %d, next %d", __FUNCTION__, currentMediaSegmentSequenceNumber, mediaFileSegmentInfo->mediaSequence));
                break;
            }
        }
    }
    else {
        mediaFileSegmentInfo = BLST_Q_NEXT(playlistFileInfo->currentMediaFileSegment, next); 
        if (mediaFileSegmentInfo) {
            BDBG_MSG(("%s: using same playlist file, simply return the next entry w/ seq# %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence));
        }
        else {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
            BDBG_WRN(("%s: next entry is NULL, this may be it for bounded streams!!!", __FUNCTION__));
#endif
        }
    }
    if (mediaFileSegmentInfo)
        playlistFileInfo->currentMediaFileSegment = mediaFileSegmentInfo;

    BDBG_LEAVE(B_PlaybackIp_MpegDashGetNextMediaSegmentEntry);
    return mediaFileSegmentInfo;

error:
    BDBG_LEAVE(B_PlaybackIp_MpegDashGetNextMediaSegmentEntry);
    return NULL;
}


void
B_PlaybackIp_MpegDashMediaSegmentDownloadThread(
    void *data
    )
{
    int i;
    BERR_Code rc;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;
    MpegDashSegmentBuffer *segmentBuffer = NULL;
    unsigned networkBandwidth = 0;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
    int currentMediaSegmentSequenceNumber = -1;
    bool previousMediaSegmentCompletelyDownloaded = true;
    bool serverClosed;

    BDBG_MSG(("%s: Started", __FUNCTION__));
    /* start from the 1st segment in the current playlist */
    mpegDashSession->resetPlaylist = true;
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            BDBG_MSG(("%s: breaking out of MPEG-DASH Media Segment Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
            BDBG_WRN(("%s: TODO: yet to add trickmode logic"));
            BDBG_ASSERT(NULL);
            break;
        }

        /* determine the next buffer to use for downloading next media segment */
        segmentBuffer = NULL;
        while (!segmentBuffer) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of MPEG-DASH Download Media Segment Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            for (i=0; !segmentBuffer && i<MPEG_DASH_NUM_SEGMENT_BUFFERS; i++) {
                BKNI_AcquireMutex(mpegDashSession->segmentBuffer[i].lock);
                if (!mpegDashSession->segmentBuffer[i].filled) {
                    segmentBuffer = &mpegDashSession->segmentBuffer[i];
                    segmentBuffer->bufferDepth = 0;
                }
                BKNI_ReleaseMutex(mpegDashSession->segmentBuffer[i].lock);
            }

            if (!segmentBuffer) {
                if (mpegDashSession->mpegDashPlaybackThreadDone) {
                    BDBG_ERR(("%s: MPEG-DASH Playback thread is done, so stopping the MPEG-DASH Download thread", __FUNCTION__));
                    goto error;
                }
                /* wait on signal from playback thread to consume and free up one of the buffers */
                rc = BKNI_WaitForEvent(mpegDashSession->bufferEmptiedEvent, MPEG_DASH_EVENT_TIMEOUT_MSEC);
                if (rc == BERR_TIMEOUT) {
                    BDBG_MSG(("%s: EVENT timeout: failed to receive event from MPEG-DASH Playback thread indicating buffer availability, continue waiting", __FUNCTION__));
                    continue;
                } else if (rc!=0) {
                    BDBG_ERR(("%s: failed to wait for event indicating buffer consumption from MPEG-DASH Playback thread, rc = %d", __FUNCTION__, rc));
                    goto error;
                }
            }
        }

        if (previousMediaSegmentCompletelyDownloaded == true) {
            /* now download the next segment */
            BKNI_AcquireMutex(mpegDashSession->lock);
            /* now we have a buffer, pick the next media segment to download, it may be from a different playlist file if our n/w b/w has changed */
            if ((mediaFileSegmentInfo = B_PlaybackIp_MpegDashGetNextMediaSegmentEntry(playback_ip, mpegDashSession, mpegDashSession->resetPlaylist ? -1:currentMediaSegmentSequenceNumber, networkBandwidth)) == NULL) {
                if (mpegDashSession->currentPlaylistFile->bounded || (!mpegDashSession->currentPlaylistFile->bounded && mpegDashSession->mpegDashPlaylistReDownloadThreadDone)) {
#ifdef BDBG_DEBUG_BUILD
                    if (playback_ip->ipVerboseLog)
                        BDBG_WRN(("%s: No More Media Segment URI left in the Playlist URI %s, we are done!!", __FUNCTION__, mpegDashSession->currentPlaylistFile->uri));
#endif
                    BKNI_ReleaseMutex(mpegDashSession->lock);
                    goto error;
                }
                /* unbounded case: no more media segments for downloading, set a flag to indicate this to Playlist ReDownload thread (incase it has to do playlist switch due to seq# wrap) */
                mpegDashSession->downloadedAllSegmentsInCurrentPlaylist = true;
                BKNI_ReleaseMutex(mpegDashSession->lock);
                /* couldn't find the next media segment, may have to wait for ReDownload thread to fetch the updated playlist from the server */
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: couldn't find the next media segment, may have to wait for ReDownload thread to fetch the updated playlist from the server", __FUNCTION__));
#endif
                BKNI_Sleep(200); /* allow the playlist redownload thread to refetch the playlist and then retry */
                continue;
            }
            currentMediaSegmentSequenceNumber = mediaFileSegmentInfo->mediaSequence;
            BDBG_MSG(("%s: next media segment: uri %s, seq# %d, prev fd %d", __FUNCTION__, mediaFileSegmentInfo->absoluteUri, mediaFileSegmentInfo->mediaSequence, playback_ip->socketState.fd));
            if (B_PlaybackIp_MpegDashSetupHttpSessionToServer(playback_ip, mediaFileSegmentInfo, &playback_ip->socketState.fd, segmentBuffer->buffer, &segmentBuffer->bufferDepth) == false) {
                BDBG_ERR(("%s: ERROR: Socket setup or HTTP request/response failed for downloading next Media Segment, skip to next uri", __FUNCTION__));
                BKNI_ReleaseMutex(mpegDashSession->lock);
                continue;
            }
            BKNI_ReleaseMutex(mpegDashSession->lock);
        }
        else {
            /* previous media segment wasn't completely downloaded due to segment buffer being smaller than the content length */
            BDBG_MSG(("%s: previous media segment wasn't completely downloaded due to segment buffer being smaller than the content length", __FUNCTION__));
        }

        BDBG_MSG(("%s: now download the actual media segment, contentLength %lld", __FUNCTION__, playback_ip->contentLength));
        /* now download the actual media segment */
        serverClosed = false;
        if (B_PlaybackIp_MpegDashDownloadMediaSegment(playback_ip, playback_ip->socketState.fd, segmentBuffer->buffer, segmentBuffer->bufferSize, &segmentBuffer->bufferDepth, &networkBandwidth, &serverClosed) != true) {
            BDBG_ERR(("%s: failed to download the current media segment, skip to next media segment", __FUNCTION__));
            continue;
        }

        /* check if we downloaded the complete media segment, if not set a flag to continue downloading in the next iteration */
        if (serverClosed) {
            BDBG_MSG(("%s: downloaded complete media segment bytesDownloaded %d", __FUNCTION__, segmentBuffer->bufferDepth));
            previousMediaSegmentCompletelyDownloaded = true;
        }
        else {
            BDBG_MSG(("%s: didn't download all bytes of current media segment: bytesDownloaded %d", __FUNCTION__, segmentBuffer->bufferDepth));
            previousMediaSegmentCompletelyDownloaded = false;
        }

        /* if switching URLs, then do media probe again and determine if pids/codecs have changed. If so, AV decoders will need to be restarted */
        /* TODO: add code for this */

        /* inform MPEG-DASH playback thread that buffer is filled w/ the media segment */
        BKNI_AcquireMutex(segmentBuffer->lock);
        segmentBuffer->filled = true;
        if (mediaFileSegmentInfo->markedDiscontinuity) {
            segmentBuffer->markedDiscontinuity = true;
            mediaFileSegmentInfo->markedDiscontinuity = false;
        }
        BKNI_ReleaseMutex(segmentBuffer->lock);
        BKNI_SetEvent(mpegDashSession->bufferFilledEvent);
        BDBG_MSG(("%s: informed MPEG-DASH playback thread that buffer %p: depth %d is filled , current network bandwidth %d", __FUNCTION__, segmentBuffer, segmentBuffer->bufferDepth, networkBandwidth));
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error: 
    mpegDashSession->mpegDashSegmentDownloadThreadDone = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: MPEG-DASH Media Segment Download thread is exiting...", __FUNCTION__));
#endif
    return;
}

void 
B_PlaybackIp_MpegDashPlaylistReDownloadThread(
    void *data
    )
{
    int rc;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    MpegDashSessionState *mpegDashSession = playback_ip->mpegDashSessionState;
    PlaylistFileInfo *playlistFileInfo;
    PlaylistFileInfo *newPlaylistFileInfo;
    int reloadTimer = 30; /* start w/ a high timeout value */

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: Started", __FUNCTION__));
#endif
    while (true) {
        newPlaylistFileInfo = NULL;
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            BDBG_MSG(("%s: breaking out of MPEG-DASH Playlist Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
            BDBG_WRN(("%s: TODO: yet to add trick mode code"));
            BDBG_ASSERT(NULL);
            break;
        }
        rc = BKNI_WaitForEvent(mpegDashSession->reDownloadPlaylistEvent, reloadTimer*1000);
        if (rc!=BERR_SUCCESS && rc != BERR_TIMEOUT) {
            BDBG_ERR(("%s: failed to wait for event indicating playlist reload timer, rc = %d", __FUNCTION__, rc));
            goto error;
        }
        if (mpegDashSession->mpegDashPlaybackThreadDone) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: MPEG-DASH Playback thread is done, so stopping the MPEG-DASH Playlist ReDownload thread", __FUNCTION__));
#endif
            goto error;
        }
        if (playback_ip->mediaStartTimeNoted == false) {
            BDBG_ERR(("%s: media playback hasn't started for this long, so breaking out", __FUNCTION__));
            break;
        }
        BDBG_MSG(("%s: Redownload playlist as %s, rc %d", __FUNCTION__, rc != BERR_TIMEOUT? "we've downloaded all segments in current playlist":"reload timer expired", rc));

        BKNI_AcquireMutex(mpegDashSession->lock);
        playlistFileInfo = mpegDashSession->currentPlaylistFile; /* Segment Download Thread can change the playlist due to n/w b/w change */
        if ((newPlaylistFileInfo = B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile(playback_ip, mpegDashSession, playlistFileInfo)) == NULL) {
            static int count = 0;
            if (++count > 30) {
                BKNI_ReleaseMutex(mpegDashSession->lock);
                BDBG_ERR(("%s: B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile() failed over %d attempts", __FUNCTION__, count));
                goto error;
            }
            else {
                reloadTimer = 2;    /* retry playlist download after 2 sec */
                BKNI_ReleaseMutex(mpegDashSession->lock);
                BDBG_ERR(("%s: B_PlaybackIp_MpegDashAllocateDownloadAndParsePlaylistFile() failed, retry, # of attempts %d ", __FUNCTION__, count));
                continue;
            }
        }
        if (newPlaylistFileInfo->mediaSegmentBaseSequence != -1) {
            /* seq # of the 1st segment in this playlist is specified, makes it quite easy to determine whether server has updated the playlist or not */
            if (newPlaylistFileInfo->mediaSegmentBaseSequence == playlistFileInfo->mediaSegmentBaseSequence && newPlaylistFileInfo->numMediaSegments == playlistFileInfo->numMediaSegments) {
                /* new playlist file hasn't changed yet, restart the timer and retry it */
                B_PlaybackIp_MpegDashFreePlaylistInfo(newPlaylistFileInfo);
                newPlaylistFileInfo = NULL;
#if 1
                /* just playlist hasn't changed, try to fetch it faster next time */
                reloadTimer = playlistFileInfo->initialMinimumReloadDelay/2;
#else
                /* commenting out the spec recommended way to compute the reload timer */
                static int playlistHasNotChangedCount = 1;
                if (playlistHasNotChangedCount == 1)
                    reloadTimer = playlistFileInfo->initialMinimumReloadDelay/2;
                else if (playlistHasNotChangedCount == 2)
                    reloadTimer = (playlistFileInfo->initialMinimumReloadDelay*3)/2;
                else
                    reloadTimer = 3*playlistFileInfo->initialMinimumReloadDelay;
                playlistHasNotChangedCount++;
#define MPEG_DASH_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES 9000
                if (playlistHasNotChangedCount > MPEG_DASH_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES) {
                    BDBG_ERR(("%s: new playlist has not changed for %d sec, %d times, stopping the PlaylistReDownload thread", __FUNCTION__, MPEG_DASH_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES*reloadTimer/2, playlistHasNotChangedCount));
                    BKNI_ReleaseMutex(mpegDashSession->lock);
                    break;
                }
#endif
                BDBG_MSG(("%s: new playlist has not changed yet, restart the timer by %d sec and retry it, initial timer %d", __FUNCTION__, reloadTimer, playlistFileInfo->initialMinimumReloadDelay));
            }
            else if (newPlaylistFileInfo->mediaSegmentBaseSequence < playlistFileInfo->mediaSegmentBaseSequence && !mpegDashSession->downloadedAllSegmentsInCurrentPlaylist) {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: even though playlist base seq (new %d, current %d) has wrapped around: ignore it for now we haven't yet downloaed all segments in the current playlist", __FUNCTION__, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo->mediaSegmentBaseSequence));
#endif
                B_PlaybackIp_MpegDashFreePlaylistInfo(newPlaylistFileInfo);
                reloadTimer = 1; /* retry the playlist every second so that we have the playlist ready to go once media downthread finishes segments from previous thread */
            }
            else {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: playlist has changed (base seq: cur %d, new %d), remove %p playlist and insert %p playlist", __FUNCTION__, 
                            playlistFileInfo->mediaSegmentBaseSequence, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo, newPlaylistFileInfo));
#endif
                if (mpegDashSession->downloadedAllSegmentsInCurrentPlaylist && newPlaylistFileInfo->mediaSegmentBaseSequence < playlistFileInfo->mediaSegmentBaseSequence) {
                    /* we have downloaded all segments in the current playlist and new playlist has wrapped around (new base seq < cur base seq) */
                    /* set a flag to indicate starting from the 1st segment of this playlist */
                    BDBG_MSG(("%s: playlist base seq has wrapped around: & we've downloaed all segments in the current playlist, so resetting to use new playlist from starting seq # %d, old playlist seq# %d", __FUNCTION__, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo->mediaSegmentBaseSequence));
                    mpegDashSession->resetPlaylist = true;
                }
                B_PlaybackIp_MpegDashReplacePlaylists(mpegDashSession, playlistFileInfo, newPlaylistFileInfo);
                mpegDashSession->currentPlaylistFile = newPlaylistFileInfo;
                mpegDashSession->currentPlaylistFile->playlistHasChanged = true; 
                playlistFileInfo = newPlaylistFileInfo;
#if 0
                /* reset the count as playlist has now changed */
                playlistHasNotChangedCount = 1;
#endif
                reloadTimer = playlistFileInfo->initialMinimumReloadDelay;
                if (playlistFileInfo->bounded) {
                    BDBG_MSG(("%s: updated playlist has END TAG set, stopping the ReDownloading thread as Live Event has reached towards its end", __FUNCTION__)); 
                    BKNI_ReleaseMutex(mpegDashSession->lock);
                    break;
                }
            }
        }
        else {
            /* TODO: add this code */
            BDBG_WRN(("%s: TODO: base media sequence is not explicitly specified in the playlist, need to manually compare the uris to determine if the new playlist has changed", __FUNCTION__));
        }
        BKNI_ReleaseMutex(mpegDashSession->lock);
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error: 
    if (newPlaylistFileInfo)
        B_PlaybackIp_MpegDashFreePlaylistInfo(newPlaylistFileInfo);
    mpegDashSession->mpegDashPlaylistReDownloadThreadDone = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: MPEG-DASH Playlist ReDownload thread is exiting...", __FUNCTION__));
#endif
    return;
}

static void 
B_PlaybackIp_ReadCallback(void *context, int param)
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;
    BKNI_SetEvent(playback_ip->read_callback_event);
    BSTD_UNUSED(param);
}

extern bool B_PlaybackIp_UtilsEndOfStream( B_PlaybackIpHandle playback_ip);
#define HTTP_PLAYPUMP_BUF_SIZE (19200)
void 
B_PlaybackIp_MpegDashPlaybackThread(
    void *data
    )
{
    int i;
    int bytesToCopy;
    B_ThreadSettings settingsThread;
    B_PlaybackIpHandle playback_ip;
    size_t bytesFed;
    ssize_t rc = -1;
    off_t totalBytesFed = 0;
    MpegDashSessionState *mpegDashSession;
    MpegDashSegmentBuffer *segmentBuffer;
    NEXUS_PlaypumpSettings nSettings;
    PlaylistFileInfo *playlistFileInfo;
    static int fileNameSuffix = 0;
    char recordFileName[32];
    FILE *fclear = NULL;

    BDBG_ENTER(B_PlaybackIp_MpegDashPlaybackThread);

    playback_ip = (B_PlaybackIpHandle)data;
    mpegDashSession = playback_ip->mpegDashSessionState;
    playlistFileInfo = mpegDashSession->currentPlaylistFile;

    if (playback_ip->settings.networkTimeout) {
        if (playback_ip->settings.networkTimeout > (HTTP_SELECT_TIMEOUT/10))
            playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
        else
            playback_ip->networkTimeout = playback_ip->settings.networkTimeout;
    }
    else {
        playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
    }
    BDBG_MSG(("%s: Starting (n/w timeout %d secs)", __FUNCTION__, playback_ip->networkTimeout));

    if (B_PlaybackIp_MpegDashSegmentBufferSetup(playback_ip) < 0) {
        goto error;
    }

    /* start a thread to download the next media file segment into the read buffer */
    B_Thread_GetDefaultSettings(&settingsThread);
    mpegDashSession->mpegDashSegmentDownloadThread = B_Thread_Create("MpegDashMediaSegmentDownloadThread", B_PlaybackIp_MpegDashMediaSegmentDownloadThread, (void *)playback_ip, &settingsThread);
    if (NULL == mpegDashSession->mpegDashSegmentDownloadThread) {
        BDBG_ERR(("%s: Failed to create the %s thread for MPEG-DASH protocol\n", __FUNCTION__, "MpegDashMediaSegmentDownloadThread"));
        goto error;
    }
    BDBG_MSG(("%s: Created the %s for MPEG-DASH protocol", __FUNCTION__, "MpegDashMediaSegmentDownloadThread"));

    if (playback_ip->nexusHandles.playpump) {
        NEXUS_Playpump_GetSettings(playback_ip->nexusHandles.playpump, &nSettings);
        /* TODO: understand this */
        nSettings.dataCallback.callback = B_PlaybackIp_ReadCallback;
        nSettings.dataCallback.context = playback_ip;
        if (NEXUS_Playpump_SetSettings(playback_ip->nexusHandles.playpump, &nSettings)) {
            BDBG_ERR(("%s:%d Nexus Error: %d\n", __FUNCTION__, __LINE__, rc));
            goto error;
        }
    }
    else {
        BDBG_ERR(("%s: playback_ip->nexusHandles.playpump is NULL", __FUNCTION__));
        goto error;
    }

    if (playback_ip->enableRecording) {
        memset(recordFileName, 0, sizeof(recordFileName));
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/mpegDash_rec%d_0.ts", fileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }

    if (B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(playback_ip))
        goto error;

    if (playlistFileInfo->bounded == false) {
        /* stream is unbounded, meaning it is live stream, we need to start a thread to periodically download the updated playlist */
        mpegDashSession->mpegDashPlaylistReDownloadThread = B_Thread_Create("MpegDashPlaylistReDownloadThread", B_PlaybackIp_MpegDashPlaylistReDownloadThread, (void *)playback_ip, &settingsThread);
        if (NULL == mpegDashSession->mpegDashPlaylistReDownloadThread) {
            BDBG_ERR(("%s: Failed to create the %s thread for MPEG-DASH protocol\n", __FUNCTION__, "MpegDashPlaylistReDownloadThread"));
            goto error;
        }
        BDBG_MSG(("%s: Created %s for MPEG-DASH protocol", __FUNCTION__, "MpegDashPlaylistReDownloadThread"));

        /* if playlist total duration is less than 3 target durations. This is to avoid stalls due n/w b/w fluctuations & latency to server */
#if 0
        /* commenting this out as it is causing large initial latency while playing from wowza HLS server. This server only had 3 segments in the */
        /* initial playlist whose total duration happened to be < 3*target duration. Thus, this loop of 20sec seems to be large and wasteful */
        while (playlistFileInfo->totalDuration < 3*playlistFileInfo->maxMediaSegmentDuration) {
            /* TODO: double check this logic whether totalDuration is getting updated at the correct play a/f media seg is downloaded */
            static int count = 0;
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of MPEG-DASH Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                break;
            }
            if (count++ > 20) {
                BDBG_ERR(("%s: Unbound stream didn't update in over 20 sec, aborting: total current duration %d is less than 3 times max segment duration %d", __FUNCTION__, playlistFileInfo->totalDuration, playlistFileInfo->maxMediaSegmentDuration));
                break;
            }
            BDBG_MSG(("%s: Unbound stream: total duration %d is less than 3 times max segment duration %d, wait to cross this threshold", __FUNCTION__, playlistFileInfo->totalDuration, playlistFileInfo->maxMediaSegmentDuration));
            /* sleep and retry, hopefully reDownload thread gets a chance to fetch the new playlist w/ more than 3*seg duration */
            BKNI_Sleep(1000);
        }
#endif
    }
    
    /* main loop */
    BDBG_MSG(("%s: Read from segment buffer and feed to playpump", __FUNCTION__));
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of MPEG-DASH Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
            BDBG_MSG(("%s: pausing main thread to allow trickmode transitions", __FUNCTION__));
            BKNI_Sleep(50);
            continue;
        }

        /* determine the segment buffer to read data from */
        segmentBuffer = NULL;
        while (!segmentBuffer) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of MPEG-DASH Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            for (i=0; !segmentBuffer && i<MPEG_DASH_NUM_SEGMENT_BUFFERS; i++) {
                BKNI_AcquireMutex(mpegDashSession->segmentBuffer[i].lock);
                if (mpegDashSession->segmentBuffer[i].filled) {
                    segmentBuffer = &mpegDashSession->segmentBuffer[i];
                    BDBG_MSG(("%s: %p buffer is filled up", __FUNCTION__, segmentBuffer));
                }
                else
                    BDBG_MSG(("%s: still no filled buffer, wait for signal from download thread, i %d", __FUNCTION__, i));
                BKNI_ReleaseMutex(mpegDashSession->segmentBuffer[i].lock);
            }

            if (!segmentBuffer) {
                if (mpegDashSession->mpegDashSegmentDownloadThreadDone) {
                    BDBG_MSG(("%s: Network Read Error, wait to play out the stream", __FUNCTION__));
                    if (B_PlaybackIp_UtilsEndOfStream(playback_ip) == true) {
#ifdef BDBG_DEBUG_BUILD
                        if (playback_ip->ipVerboseLog)
                            BDBG_WRN(("%s: No more segments to feed to playback & playback is done, total fed %lld", __FUNCTION__, totalBytesFed));
#endif
                        goto error;
                    }
                    else {
#ifdef BDBG_DEBUG_BUILD
                        if (playback_ip->ipVerboseLog)
                            BDBG_WRN(("%s: Continue waiting to either playout the whole stream or re-reading from socket incase it becomes valid again by a seek or rewind trickplay", __FUNCTION__));
#endif
                        BKNI_Sleep(1000);
                        continue;
                    }
                }
                /* wait on signal from MPEG-DASH download thread to fill up one of the buffers */
                BDBG_MSG(("%s: wait %d msec for a signal from download thread", __FUNCTION__, MPEG_DASH_EVENT_TIMEOUT_MSEC));
                rc = BKNI_WaitForEvent(mpegDashSession->bufferFilledEvent, MPEG_DASH_EVENT_TIMEOUT_MSEC);
                if (rc == BERR_TIMEOUT) {
                    BDBG_MSG(("%s: EVENT timeout: failed to receive event from MPEG-DASH Download thread indicating buffer availability, continue waiting", __FUNCTION__));
                    continue;
                } else if (rc!=0) {
                    BDBG_ERR(("%s: failed to wait for event indicating buffer availability from MPEG-DASH Download thread, rc = %d", __FUNCTION__, rc));
                    goto error;
                }
                BDBG_MSG(("%s: got a signal from download thread, rc %d", __FUNCTION__, rc));
            }
        }
        BDBG_MSG(("%s: Read from segment buffer %p", __FUNCTION__, segmentBuffer));

        bytesFed = 0;
        BDBG_MSG(("%s: Feed %d bytes to Playpump", __FUNCTION__, segmentBuffer->bufferDepth));
        if (segmentBuffer->markedDiscontinuity) {
            BDBG_MSG(("%s: Flushing AV pipeline", __FUNCTION__));
            /* TODO: check this */
#if 0
            B_PlaybackIp_UtilsFlushAvPipeline(playback_ip);
#endif
            segmentBuffer->markedDiscontinuity = false;
        }
        while (segmentBuffer->bufferDepth) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of MPEG-DASH Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            BDBG_MSG_FLOW(("%s: Get playpump buffer to read data into", __FUNCTION__));
            /* get an adequately sized buffer from the playpump */
            if (B_PlaybackIp_UtilsGetPlaypumpBuffer(playback_ip, HTTP_PLAYPUMP_BUF_SIZE) < 0) {
                BDBG_MSG(("%s: Failed to get buffer from playpump, breaking out of HTTP loop", __FUNCTION__));
                break;
            }
            BDBG_MSG_FLOW(("%s: Got playpump buffer to read more data into", __FUNCTION__));

            /* determine how much data can be copied */
            bytesToCopy = segmentBuffer->bufferDepth > HTTP_PLAYPUMP_BUF_SIZE ? HTTP_PLAYPUMP_BUF_SIZE:segmentBuffer->bufferDepth;
            memcpy(playback_ip->buffer, segmentBuffer->buffer+bytesFed, bytesToCopy);

            BDBG_MSG_FLOW(("%s: copied %d bytes from segment buffer into playpump buffer", __FUNCTION__, bytesToCopy));
        
            /* write data to file */
            if (playback_ip->enableRecording && fclear) {
                fwrite(playback_ip->buffer, 1, bytesToCopy, fclear); 
            }

            /* now feed appropriate data it to the playpump */
            if (NEXUS_Playpump_ReadComplete(playback_ip->nexusHandles.playpump, 0, bytesToCopy)) {
                BDBG_ERR(("%s: NEXUS_Playpump_ReadComplete failed, continuing...", __FUNCTION__));
                continue;
            }
            bytesFed += bytesToCopy;
            totalBytesFed += bytesToCopy;
            segmentBuffer->bufferDepth -= bytesToCopy;
            BDBG_MSG_FLOW(("%s: Fed %d bytes to Playpump, total fed %d, remaining %d\n", __FUNCTION__, bytesToCopy, bytesFed, segmentBuffer->bufferDepth));
        }
        BDBG_MSG(("%s: Finished feeding %d bytes to Playpump", __FUNCTION__, bytesFed));

        /* inform MPEG-DASH Download thread that buffer is emptied and fed to the playback h/w */
        BKNI_AcquireMutex(segmentBuffer->lock);
        segmentBuffer->filled = false;
        BKNI_ReleaseMutex(segmentBuffer->lock);
        BKNI_SetEvent(mpegDashSession->bufferEmptiedEvent);
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error:
    mpegDashSession->mpegDashPlaybackThreadDone = true;
    while (!mpegDashSession->mpegDashSegmentDownloadThreadDone) {
        static int count = 0;
        if (count++ > 50) {
            BDBG_ERR(("%s: Failed to wait for MPEG-DASH Media Segment Download thread to finish for over %d attempts", __FUNCTION__, count));
            break;
        }
        BDBG_MSG(("%s: MPEG-DASH Playback thread ran into some error, wait for Download thread to finish", __FUNCTION__));
        BKNI_Sleep(100);
    }
    if (mpegDashSession->mpegDashPlaylistReDownloadThread) {
        while (!mpegDashSession->mpegDashPlaylistReDownloadThreadDone) {
        static int count = 0;
        BKNI_SetEvent(mpegDashSession->reDownloadPlaylistEvent);
        if (count++ > 50) {
            BDBG_ERR(("%s: Failed to wait for MPEG-DASH Playlist ReDownload thread to finish for over %d attempts", __FUNCTION__, count));
            break;
        }
            BDBG_MSG(("%s: MPEG-DASH Playback thread ran into some error, wait for Playlist ReDownload thread to finish", __FUNCTION__));
            BKNI_Sleep(100);
        }
    }
    if (playback_ip->enableRecording && fclear) {
        fflush(fclear);
        fclose(fclear);
    }
    if (mpegDashSession->mpegDashSegmentDownloadThread) {
        B_Thread_Destroy(mpegDashSession->mpegDashSegmentDownloadThread);
    }

    if (mpegDashSession->mpegDashPlaylistReDownloadThread) {
        B_Thread_Destroy(mpegDashSession->mpegDashPlaylistReDownloadThread);
    }

    if (playback_ip->openSettings.eventCallback && 
            playback_ip->playback_state != B_PlaybackIpState_eStopping && 
            playback_ip->playback_state != B_PlaybackIpState_eStopped) 
    {
        B_PlaybackIpEventIds eventId;
        if (playback_ip->serverClosed)
            eventId = B_PlaybackIpEvent_eServerEndofStreamReached;
        else
            eventId = B_PlaybackIpEvent_eErrorDuringStreamPlayback;
            eventId = B_PlaybackIpEvent_eServerEndofStreamReached;
        playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, eventId);
    }
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: MPEG-DASH Playback thread is exiting...", __FUNCTION__));
#endif
    BKNI_SetEvent(playback_ip->playback_halt_event);

    BDBG_LEAVE(B_PlaybackIp_MpegDashPlaybackThread);
}
