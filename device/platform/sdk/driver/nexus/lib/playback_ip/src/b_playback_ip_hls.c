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
* $brcm_Workfile: b_playback_ip_hls.c $
* $brcm_Revision: 33 $
* $brcm_Date: 11/12/12 8:46a $
*
* Description: HTTP Live Streaming (HLS) Client Implementation
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_hls.c $
* 
* 33   11/12/12 8:46a ssood
* SW7231-858: limit initial http payload length to temp_buf length
* 
* 32   11/1/12 4:38p ssood
* SW7231-858: coverity fixes
* 
* 31   10/30/12 11:11a ssood
* SW7231-858: fixed couple of memory leaks + use cookie in hls http get
*  req + dont check for EOF condition for live sessions
* 
* 30   10/16/12 5:26p ssood
* SW7231-858: a.changed HLS_EVENT_TIMEOUT_MSEC to 2 sec
* 
* 29   10/3/12 2:06p ssood
* SW7425-3673: handle cases where URL is just relative to the root
*  (server name & port) + fix for seeking to the segment w/ duration <
*  2nd segment
* 
* 28   9/6/12 4:17p ssood
* SW7425-3673: added support for seek/pause/play of HLS sessions
* 
* 27   7/31/12 4:18p ssood
* SW7346-347: code to cache & re-use HTTP cookie + fixes for Http Xfer
*  encoding + debug related additions
* 
* 26   6/20/12 4:55p ssood
* SW7231-858: support SW based AES decryption and HTTS based key &
*  playlist downloads for HLS Player
* 
* 25   6/15/12 6:26p ssood
* SW7231-858: more hls related improvements
* 
* 24   6/13/12 7:26p ssood
* SW7231-858: more coverity fixes
* 
* 23   6/13/12 1:54p ssood
* SW7231-858: supressing debug logs for HLS playback sessions
* 
* 22   6/13/12 1:08p ssood
* SW7231-858: dont generate event for the playlist download thread when
*  media segment thread has reached the live point
* 
* 21   6/13/12 7:24a ssood
* SW7231-766: Coverity fixes for HLS player logic
* 
* 20   6/5/12 6:46p ssood
* SW7231-766: add support for downloading encrypted HLS keys + support
*  for HLS spec version 3
* 
* 19   5/18/12 4:59p ssood
* SW7231-766: determine port # based on the protocol: 80 vs. 443
* 
* 18   4/25/12 2:05p ssood
* SW7231-766: fix a crash during invalid playlist processing
* 
* 17   4/25/12 9:44a ssood
* SW7231-766: enable HTTP Chunk Xfer Encoding processing for HLS sessions
* 
* 16   4/11/12 3:06p ssood
* SW7231-766: add support for HTTP redirects for HLS sessions
* 
* 15   4/3/12 12:31p ssood
* SW7425-2783: added support to handle HTTP redirects during HLS sessions
* 
* 14   2/9/12 7:03p ssood
* SW7231-405: add support for eof check
* 
* 13   1/18/12 3:16p ssood
* SW7231-603: diable use of netaccel for HTTP protocol (due to lack of
*  any performance gains)
* 
* 12   12/27/11 5:29p ssood
* SW7231-405: handle case where some URLs have additional CR char before
*  NL char
* 
* 11   10/21/11 1:54p ssood
* SW7231-405: pause support for HLS protocol
* 
* 10   5/25/11 3:35p ssood
* SW7231-166: fixed some bugs during failing trickplay cases
* 
* 9   5/24/11 11:36a ssood
* SW7405-4392: Coverity fixes
* 
* 8   12/14/10 4:12p ssood
* SW7420-1217: dont treat playlist entry w/ 0 b/w as hard error +
*  continue playing even if unbouded playlist only has 1 entry after
*  initial 10sec wait
* 
* 7   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 6   12/11/10 7:46a ssood
* SW7420-1217: support playing ES streams over HLS + make playlist
*  parsing more error resilient
* 
* 5   12/8/10 3:57p ssood
* SW7420-1217: refined support for playing unbounded streams
* 
* 4   11/12/10 6:39p ssood
* SW7420-1217: added logic to handle playlists containing retired media
*  segments
* 
* 3   11/11/10 10:34a ssood
* SW7420-1217: added support for playing unbounded (live) streams
* 
* 2   10/29/10 9:05a ssood
* SW7420-1217: basic support to play both variant & non-variant bounded
*  playlists containing clear content
* 
* 1   10/27/10 8:28p ssood
* SW7420-1217: initial check-in for HLS support
***************************************************************************/
#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_aes.h"
#include "b_playback_ip_utils.h"
#include "b_playback_ip_lm_helper.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <ctype.h>

BDBG_MODULE(b_playback_ip_hls);

/* Overall flow 
   -Browser obtains the URI of a AV stream by parsing the html page (video tag for html5)
    -URI points to a M3U8 format playlist file which contains an ordered list of media URIs & informational tags
    -Each media URI refers to a media file which is a segment of of a single continous stream 
   -Browser passes the playlist URI to the IP library
   -IP library sends request to the server to obtain the playlist file and parses it 
*/
/* Mandatory M3U8 tags */
#define HLS_M3U8_BEGIN_TAG                      "#EXTM3U"
#define HLS_M3U8_URL_TAG                        "#EXTINF:"    /* preceeds each media file URI entry, contains media file duration in seconds */
#define HLS_M3U8_MAX_MEDIA_SEGMENT_DURATION_TAG "#EXT-X-TARGETDURATION:"     /* maximum duation in seconds of any media file in a playlist */
#define HLS_M3U8_END_TAG                        "#EXT-X-ENDLIST"             /* indicated no more media files will be added to a playlist file */

/* Optional M3U8 tags */
#define HLS_M3U8_VERSION_TAG                    "#EXT-X-VERSION:"           /* version of the playlist file, if missing, assumed 1 */
#define HLS_M3U8_MEDIA_SEGMENT_SEQUENCE_TAG     "#EXT-X-MEDIA-SEQUENCE:"     /* sequence number of the 1st URI in a playlist, if missing, then 1st URI has seq # of 0 */
#define HLS_M3U8_STREAM_INFO_TAG                "#EXT-X-STREAM-INF:"         /* preceeds URI of a playlist file */
#define HLS_M3U8_DISCONTINUITY_TAG              "#EXT-X-DISCONTINUITY"      /* indicates discontinuity between the media file that follows this tag to the one before it */
#define HLS_M3U8_ENCRYPTION_KEY_TAG             "#EXT-X-KEY:"               /* indicates following segments are encrypted using this key & IV */
#define HLS_M3U8_PROGRAM_DATE_TIME_TAG          "#EXT-X-PROGRAM-DATE-TIME:"  /* preceeds a URI tag */
#define HLS_M3U8_PROGRAM_RECORD_TAG             "#EXT-X-ALLOW-CACHE:"       /* if YES, client can optionally record the media files */

#define PLAYLIST_FILE_COMPATIBILITY_VERSION 3

#define HLS_M3U8_TAG_ATTRIBUTE_PROGRAM_ID   "PROGRAM-ID"
#define HLS_M3U8_TAG_ATTRIBUTE_BANDWIDTH   "BANDWIDTH"
#define HLS_M3U8_TAG_ATTRIBUTE_CODECS       "CODECS"
#define HLS_M3U8_TAG_ATTRIBUTE_RESOLUTION   "RESOLUTION"
#define HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD   "METHOD"
#define HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD_NONE   "NONE"
#define HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD_AES128   "AES-128"
#define HLS_M3U8_TAG_ATTRIBUTE_ENC_URI      "URI"
#define HLS_M3U8_TAG_ATTRIBUTE_ENC_IV       "IV"
#define HLS_PLAYLIST_FILE_SIZE (500*1024)
#define HLS_EVENT_TIMEOUT_MSEC 2000 /* 2 secs */
#define HLS_READ_CHUNK_SIZE (TS_PKT_SIZE*HTTP_AES_BLOCK_SIZE*5)

/* extern declarations */
extern int playback_ip_read_socket( B_PlaybackIpHandle playback_ip, void *securityHandle, int fd, char *buf, int buf_size, int timeout);
ssize_t B_PlaybackIp_HttpNetRangeReq( B_PlaybackIpHandle playback_ip, void *buf, size_t length, off_t byteRangeStart, off_t byteRangeEnd, int prevFd, int *newFd);
bool http_absolute_uri(char *url);
int B_PlaybackIp_UtilsGetLine(char **linePtr, int *linePtrLength, char *buffer, int bufferLength);
NEXUS_TransportType http_get_payload_content_type(char *http_hdr);
int B_PlaybackIp_SecuritySessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, int fd, void **newSecurityHandle);
extern int http_read_response(B_PlaybackIpHandle playback_ip, void *securityHandle, int fd, char **rbufp, int rbuf_size, char **http_hdr, char **http_payload, int *payload_len);
extern http_url_type_t http_get_url_type(char *http_hdr, char *url);

#if 0
#define HLS_UNIT_TEST
#endif

#ifdef HLS_UNIT_TEST
static void
dump_playlist_file(HlsSessionState *hlsSession)
{
    static FILE * playlistFile = NULL;
    playlistFile = fopen("/data/videos/playlist.m3u8", "ab+");
    fwrite(hlsSession->playlistBuffer, 1, hlsSession->playlistBufferDepth , playlistFile); 
    fflush(playlistFile);
    fclose(playlistFile);
    BDBG_MSG(("%s: wrote downloaded M3U playlist file into file /data/videos/playlist.m3u8", __FUNCTION__));
}
#endif

int 
hls_asciiHexToDec(char ch, unsigned char *num)
{
    if (isdigit(ch)) 
        *num = ch - '0';
    else if (isxdigit(ch) && ch >= 'a')
        *num = 9 + ch - 'a' + 1;
    else if (isxdigit(ch) && ch >= 'A')
        *num = 9 + ch - 'A' + 1;
    else 
        return false;
    return true;
}

bool 
B_PlaybackIp_IsHlsSession (
    char *playlistUri, 
    char *httpResponseHeader
    )
{
    char *contentType;
    /*
       Playlist file can be either UTF-8 US-ASCII encoded. Either file name or HTTP Content-type will indicate it be a playlist file
       a.   UTF-8: if file name ends w/ .m3u8 and/or HTTP Content-type is "application/vnd.apple.mpegurl" 
       b.   US-ASCII: if file name ends w/ .m3u and/or HTTP Content-type is either "application/x-mpegURL" or "audio/mpegurl"
       c.   Note: need to understand difference in parsing between UTF-8 & ASCII

    */
    if (B_PlaybackIp_UtilsStristr(playlistUri, "m3u") != NULL) {
        BDBG_MSG(("%s: M3U encoded URI: %s", __FUNCTION__, playlistUri));
        return true;
    }

    if ((contentType = B_PlaybackIp_UtilsStristr(httpResponseHeader, "Content-Type: ")) != NULL) {
        contentType += strlen("Content-Type: ");

        if (B_PlaybackIp_UtilsStristr(contentType, "application/vnd.apple.mpegurl") ||
            B_PlaybackIp_UtilsStristr(contentType, "application/x-mpegurl") ||
            B_PlaybackIp_UtilsStristr(contentType, "audio/x-mpegurl") ||
            B_PlaybackIp_UtilsStristr(contentType, "audio/mpegurl")
            ) {
            BDBG_MSG(("%s: M3U encoded URI: %s, contentType %s", __FUNCTION__, playlistUri, contentType));
            return true;
        }
    }

    /* neither uri name nor content type indicates that the session is HTTP Live Streaming session */
    return false;
}

char *
B_PlaybackIp_HlsBuildAbsoluteUri(char *server, int port, char *baseUri, char *fileName) 
{
    int uriLength;
    char portString[16];
    char *uri;
    char *tmp1, *tmp2 = NULL;
    int baseUriLength = 0;
    char *baseUriCopy = NULL;

    /* determine the # of char for the port */
    memset(portString, 0, sizeof(portString));
    snprintf(portString, sizeof(portString)-1, "%d", port);

    if (fileName[0] != '/') {
        /* relative uri fileName doesn't start w/ /, so we need to use path from the base URI */
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
            memset(baseUriCopy, 0, baseUriLength);
            strncpy(baseUriCopy, baseUri, baseUriLength-1);
            baseUriCopy[baseUriLength-1] = '\0';
        }
    }
    else {
        baseUriLength = 0; /* nothing to use from the baseUri as relative URI is relative to just the root server name & port # */
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

/* download file (either playlist or decryption key) from the current network socket into the specified buffer and return the amount of bytes read. */
/* read until EOF, error condition, or channel change (state change) occurs */
bool
B_PlaybackIp_HlsDownloadFile(B_PlaybackIpHandle playback_ip, int fd, char *buffer, int bufferSize, ssize_t *totalBytesRead, bool nullTerminate)
{
    ssize_t bytesRead = 0;
    int bytesToRead;
    bool serverClosed = false;

    BDBG_MSG(("%s: start downloading a playlist, currently read %d", __FUNCTION__, *totalBytesRead));
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping) {
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
        bytesToRead = bufferSize > HLS_READ_CHUNK_SIZE ? HLS_READ_CHUNK_SIZE : bufferSize;
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
    BDBG_MSG(("%s: finished downloading playlist: errno %d, size %d, state %d, server closed %d on socket %d", __FUNCTION__, errno, *totalBytesRead, playback_ip->playback_state, serverClosed, fd));
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
            BDBG_ERR(("%s: increase the playlist buffer size from %d, it is not big enough", __FUNCTION__, bufferSize));
            return false;
        }
        return true;
    }
    else
        return false;
}

/* download data from the current network socket into the specified buffer and return the amount of bytes read. */
/* read until EOF, error condition, or channel change (state change) occurs */
/* in addition, also measure the network bandwidth while downloading this data. */
/* current socket (accessed via playback_ip struct) can correspond to playlist file or actual media segment */
bool
B_PlaybackIp_HlsDownloadMediaSegment(
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

    BDBG_MSG(("%s: start downloading file: contentLength %lld, uri %s", __FUNCTION__, playback_ip->contentLength, playback_ip->openSettings.socketOpenSettings.url));
    /* start a timer to note the n/w b/w */
    B_Time_Get(&beginTime);
    while (true) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
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
        if ((*totalBytesRead + HLS_READ_CHUNK_SIZE) > bufferSize) {
            BDBG_MSG(("%s: need bigger buffer to hold the complete downloaded file: totalBytesRead %d, size %d, returning what is read", __FUNCTION__, *totalBytesRead, bufferSize));
            break;
        }

        if ((bytesRead = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, fd, buffer+*totalBytesRead, HLS_READ_CHUNK_SIZE, playback_ip->networkTimeout)) <= 0) {
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
    if (*totalBytesRead)
        return true;
    else
        return false;
}

bool
B_PlaybackIp_HlsBoundedStream(B_PlaybackIpHandle playback_ip)
{
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;

    if (hlsSession && hlsSession->currentPlaylistFile && hlsSession->currentPlaylistFile->bounded)
        return true;
    else 
        return false;
}

bool 
B_PlaybackIp_IsPlaylistInExtendedM3uFormat(B_PlaybackIpHandle playback_ip)
{
    int rc;
    char *nextLine = NULL;
    int nextLineLength = 0;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;

    /* based on the url extension or content type, we had assumed that we have a M3U playlist file in the index cache */
    /* HTTP Live Streaming protocol requires this file to be in the Extended M3U format */

    /* 1st line must be HLS_M3U8_BEGIN_TAG */
    if ((rc =B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer, hlsSession->playlistBufferDepth)) > 0) {
        if (B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_BEGIN_TAG)) {
            BDBG_MSG(("%s: playlist is Extended M3U compliant, nextline %s, len %d, rc %d", __FUNCTION__, nextLine, nextLineLength, rc));
            hlsSession->playlistBufferReadIndex += rc;
            free(nextLine);
            return true;
        }
    }
    BDBG_ERR(("%s: playlist is not Extended M3U compliant, missing %s tag, nextline %s, len %d", __FUNCTION__, HLS_M3U8_BEGIN_TAG, nextLine, nextLineLength));

    if (nextLine) 
        free(nextLine);
    return false;
}

void
B_PlaybackIp_FreeMediaFileSegmentInfo(MediaFileSegmentInfo *mediaFileSegmentInfo)
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
B_PlaybackIp_FreeMediaFileSegmentInfoAll(struct MediaFileSegmentInfoQueue *mediaFileSegmentInfoQueueHead)
{
    MediaFileSegmentInfo *mediaFileSegmentInfo, *tmpPtr;
    mediaFileSegmentInfo = BLST_Q_FIRST(mediaFileSegmentInfoQueueHead);
    while (mediaFileSegmentInfo != NULL) {
        tmpPtr = BLST_Q_NEXT(mediaFileSegmentInfo, next);
        BLST_Q_REMOVE(mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
        B_PlaybackIp_FreeMediaFileSegmentInfo(mediaFileSegmentInfo);
        mediaFileSegmentInfo = tmpPtr;
    }

    BDBG_MSG(("%s: done", __FUNCTION__));
}

void
B_PlaybackIp_FreePlaylistInfo(PlaylistFileInfo *playlistFileInfo)
{

    B_PlaybackIp_FreeMediaFileSegmentInfoAll(&playlistFileInfo->mediaFileSegmentInfoQueueHead);

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
B_PlaybackIp_FreePlaylistInfoAll(B_PlaybackIpHandle playback_ip)
{
    PlaylistFileInfo *playlistFileInfo, *tmpPtr;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    
    playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
    while (playlistFileInfo != NULL) {
        tmpPtr = BLST_Q_NEXT(playlistFileInfo, next);
        BLST_Q_REMOVE(&hlsSession->playlistFileInfoQueueHead, playlistFileInfo, next);
        B_PlaybackIp_FreePlaylistInfo(playlistFileInfo);
        playlistFileInfo = tmpPtr;
    }

    if (hlsSession->playlistBuffer) {
        BKNI_Free(hlsSession->playlistBuffer);
        hlsSession->playlistBuffer = NULL;
    }
    BDBG_MSG(("%s: done", __FUNCTION__));
}

bool 
hls_parse_url(B_PlaybackIpProtocol *protocol, char **server, unsigned *portPtr, char **uri, char *absoluteUri)
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
hls_build_get_req(B_PlaybackIpHandle playback_ip, char *write_buf, int write_buf_size, char *server, int port, char *uri)
{
    char *header = write_buf;
    unsigned int bytesLeft = write_buf_size;;
    int bytesWrote = 0;

    memset(header, 0, bytesLeft);
    bytesWrote = snprintf(header, bytesLeft,
            "GET %s HTTP/1.1\r\n"
            "User-Agent: %s\r\n"
            "Connection: Close\r\n"
            "%s"
            ,
            uri,
            (playback_ip->openSettings.u.http.userAgent ? playback_ip->openSettings.u.http.userAgent : "BRCM HTTP Client/2.0"),
            (playback_ip->openSettings.u.http.additionalHeaders ? playback_ip->openSettings.u.http.additionalHeaders : "")
            );
    bytesLeft -= bytesWrote;
    header += bytesWrote;

    if (port == HTTP_DEFAULT_PORT) {
        bytesWrote = snprintf(header, bytesLeft, "Host: %s\r\n", server);
    }
    else {
        /* not using the default HTTP port, so Host Header needs to include the port # */
        bytesWrote = snprintf(header, bytesLeft, "Host: %s:%d\r\n", server, port);
    }
    bytesLeft -= bytesWrote;
    header += bytesWrote;

    if (playback_ip->cookieFoundViaHttpRedirect) {
        /* add the cookie header */
        bytesWrote = snprintf(header, bytesLeft, "Cookie: %s\r\n", playback_ip->cookieFoundViaHttpRedirect);
        bytesLeft -= bytesWrote;
        header += bytesWrote;
    }

    bytesWrote = snprintf(header, bytesLeft, "\r\n");
    bytesLeft -= bytesWrote;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog) {
        BDBG_WRN(("Complete HTTP Get request:"));
        fprintf(stdout, "%s", write_buf);
        fflush(stdout);
    }
#endif
}

void
B_Playback_HlsDestroyPlaybackIpSession(
    B_PlaybackIpHandle playbackIp
    )
{
    if (!playbackIp)
        return;

    playbackIp->playback_state = B_PlaybackIpState_eSessionOpened; /* change state back to Open */
    playbackIp->hlsSessionEnabled = false;
    playbackIp->openSettings.u.http.userAgent = NULL;

    if (B_PlaybackIp_SessionClose(playbackIp)) {
        BDBG_ERR(("%s: B_PlaybackIp_SessionClose() Failed", __FUNCTION__));
        return;
    }

    if (B_PlaybackIp_Close(playbackIp)) {
        BDBG_ERR(("%s: B_PlaybackIp_Close() Failed", __FUNCTION__));
        return;
    }
}

static B_PlaybackIpHandle 
B_Playback_HlsCreatePlaybackIpSession(
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

    return playbackIp;

error:
    if (B_PlaybackIp_Close(playbackIp)) {
        BDBG_ERR(("%s: B_PlaybackIp_Close() Failed", __FUNCTION__));
    }
    return NULL;
}

/* setup HTTP Session, Open & Enable Security Context and get hls session ready for media Segment download */
bool
B_PlaybackIp_HlsSetupHttpSessionToServer(
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
    int requestMessageSize, responseMessageSize;
    char *http_hdr, *http_payload;
    http_url_type_t http_url_type;
    char *serverRedirect = NULL;
	char *uriRedirect = NULL;
    B_PlaybackIpProtocol protocol;
    B_PlaybackIpSessionOpenSettings openSettings;
	socklen_t addrLen;
    struct sockaddr_in local_addr;

    if (!playback_ip || !mediaFileSegmentInfo)
        return false;

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
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: URI: http://%s:%d%s", __FUNCTION__, server, port, *uri));
#endif

    /* reset previous content length */
    playback_ip->contentLength = 0;

    /* prepare initial Get request */
    responseMessageSize = TMP_BUF_SIZE;
    requestMessageSize = 4*TMP_BUF_SIZE;
    responseMessage = (char *) BKNI_Malloc(responseMessageSize);
    requestMessage = (char *)BKNI_Malloc(requestMessageSize);
    if (!responseMessage || !requestMessage) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        goto error;
    }
    memset(&openSettings, 0, sizeof(openSettings));
    openSettings.security.securityProtocol = mediaFileSegmentInfo->securityProtocol;
    for (;;) {
        memset(requestMessage, 0, requestMessageSize);
        memset(responseMessage, 0, responseMessageSize);

        hls_build_get_req(playback_ip, requestMessage, requestMessageSize, server, port, *uri);

        /* setup the socket connection to the server & send GET request */
        if (B_PlaybackIp_UtilsTcpSocketConnect(&playback_ip->playback_state, server, port, false, socketFd) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to send Socket Connect Request to Server: %s:%d\n", __FUNCTION__, server, port));
            goto error;
        }

        addrLen = sizeof(local_addr);
        if (getsockname(*socketFd, (struct sockaddr *)&local_addr, (socklen_t *)&addrLen) != 0) {
            BDBG_ERR(("ERROR: Failed to obtain connection socket address, errno: %d \n", errno));
            perror("getsockname");
            goto error;
        }

        BDBG_MSG(("%s: successfully connected to server (fd %d, local ip:port = %s:%d)", 
                    __FUNCTION__, *socketFd, inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port)));
        /* now setup security context prior to downloading the media segment */
        /* currently, supported security protocols are: HTTPS (SSL/TLS), AES128, and Clear (no encryption) */
        /* Note: security protocol can change from segment to segment, so this function is called prior to each segment download */
        openSettings.security.initialSecurityContext = playback_ip->openSettings.security.initialSecurityContext; /* points to either AES (dmaHandle) or SSL initial security context */
#ifdef NEXUS_HAS_DMA
        openSettings.security.dmaHandle = playback_ip->openSettings.security.dmaHandle;
#endif
        switch (openSettings.security.securityProtocol) {
            case B_PlaybackIpSecurityProtocol_Aes128:
                /* setup the new key & iv */
                openSettings.security.enableDecryption = false;
                openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_Aes128;
                memcpy(openSettings.security.settings.aes128.key, mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.key, sizeof(playback_ip->openSettings.security.settings.aes128.key));
                memcpy(openSettings.security.settings.aes128.iv, mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv, sizeof(playback_ip->openSettings.security.settings.aes128.iv));
                break;
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
        if (http_read_response(playback_ip, playback_ip->securityHandle, *socketFd, &responseMessage, responseMessageSize, &http_hdr, &http_payload, &playback_ip->initial_data_len) < 0) {
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
            /* free up the previous cookie */
            if (playback_ip->cookieFoundViaHttpRedirect)
                BKNI_Free(playback_ip->cookieFoundViaHttpRedirect);
            playback_ip->cookieFoundViaHttpRedirect = NULL;
            if (http_parse_redirect(serverRedirect, &port, &protocol, &uriRedirect, &(playback_ip->cookieFoundViaHttpRedirect), http_hdr) != 0) {
                BDBG_ERR(("%s: Incorrect HTTP Redirect response or parsing error", __FUNCTION__));
                goto error;
            }
            BDBG_MSG(("%s: allocated cookie %x, its addr %p", __FUNCTION__, playback_ip->cookieFoundViaHttpRedirect, playback_ip->cookieFoundViaHttpRedirect));
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

    if (openSettings.security.securityProtocol == B_PlaybackIpSecurityProtocol_Aes128) {
        /* enable AES encryption as HTTP header processing (which is in clear) is done now */
        if (B_PlaybackIp_AesDecryptionEnable(playback_ip->securityHandle, http_payload, playback_ip->initial_data_len) < 0) {
            BDBG_ERR(("%s: ERROR: failed to enable the security decryption", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: security context is enabled for media segment %p, seq # %d, sec protocol %d, initial encrypted bytes %d", __FUNCTION__, mediaFileSegmentInfo, mediaFileSegmentInfo->mediaSequence, openSettings.security.securityProtocol, playback_ip->initial_data_len));
        playback_ip->initial_data_len = 0; /* initial payload is now given to security layer for later decryption during the read call */
    }
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
    if (!rc && *socketFd)
        close(*socketFd);
    return rc;
}

bool 
hls_downloadEncryptionKey(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpSecurityProtocol securityProtocol,
    char *encryptionKeyUri, 
    EncryptionInfo *encryptionInfo
    )
{
    char *server = NULL;
    unsigned port;
    char *uri;
    B_PlaybackIpProtocol protocol;
    int i;
    int encryptionKeyBufferSize = 4096;
    int encryptionKeyBufferDepth;
    unsigned char *encryptionKeyBuffer = NULL;
    bool status = false;
    B_PlaybackIpHandle playbackIp = NULL;

    if (hls_parse_url(&protocol, &server, &port, &uri, encryptionKeyUri) == false) {
        BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
        goto error;
    }
    if (playback_ip->openSettings.socketOpenSettings.useProxy) {
        /* when proxy is enabled, connection request has to be sent to the proxy server */
        server = playback_ip->openSettings.socketOpenSettings.ipAddr;
        port = playback_ip->openSettings.socketOpenSettings.port;
    }
    encryptionKeyBuffer = (unsigned char *)BKNI_Malloc(encryptionKeyBufferSize);
    if (!encryptionKeyBuffer) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        goto error;
    }

    if (protocol == B_PlaybackIpProtocol_eHttp || protocol == B_PlaybackIpProtocol_eHttps) {
        playbackIp = B_Playback_HlsCreatePlaybackIpSession(playback_ip, server, port, &uri, protocol, playback_ip->openSettings.security.initialSecurityContext);
        if (!playbackIp) {
            goto error;
        }
        /* restore some state from the main playback_ip session state */
        playbackIp->openSettings.u.http.userAgent = playback_ip->openSettings.u.http.userAgent;
        playbackIp->hlsSessionState = playback_ip->hlsSessionState;

        /* set state to be playing so that we can use the read/write calls w/o having to start the IpSessions */
        playbackIp->playback_state = B_PlaybackIpState_ePlaying;

        BDBG_MSG(("%s: Connected to the server for downloading the encryption key file", __FUNCTION__));
        if (playbackIp->initial_data_len) {
            BKNI_Memcpy(encryptionKeyBuffer, playbackIp->temp_buf, playbackIp->initial_data_len);
            encryptionKeyBufferDepth = playbackIp->initial_data_len;
            playbackIp->initial_data_len = 0;
        }
        else {
            encryptionKeyBufferDepth = 0;
        }

        /* now download the key */
        if (B_PlaybackIp_HlsDownloadFile(playbackIp, playbackIp->socketState.fd, (char *)encryptionKeyBuffer, encryptionKeyBufferSize, &encryptionKeyBufferDepth, false) != true) {
            BDBG_ERR(("%s: failed to download the encryption key file", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: downloaded encryption key file of %d bytes", __FUNCTION__, encryptionKeyBufferDepth));
        /* now destroy this temp playback ip session */
        B_Playback_HlsDestroyPlaybackIpSession(playbackIp);
    }
    else {
        BDBG_ERR(("%s: not supported key file download protocol (%d) specified", __FUNCTION__, protocol));
        goto error;
    }

    /* now store the key */
    if (securityProtocol == B_PlaybackIpSecurityProtocol_Aes128) {
        if (encryptionKeyBufferDepth > 16) {
            BDBG_ERR(("%s: Encryption Key length %d > 16", __FUNCTION__, encryptionKeyBufferDepth));
            goto error;
        }
        memcpy(encryptionInfo->encryptionMethod.aes128.key, encryptionKeyBuffer, sizeof(encryptionInfo->encryptionMethod.aes128.key));
        for (i=0; i<16;i++) {
            BDBG_MSG(("enc key[%d] %02x", i, encryptionInfo->encryptionMethod.aes128.key[i]));
        }
        BDBG_MSG(("%s: successfully downloaded the encryption key of %d length", __FUNCTION__, encryptionKeyBufferDepth));
    }
    BDBG_MSG(("%s: URI for Media Segment Encryption key: server %s, port %d, uri %s", __FUNCTION__, server, port, uri));
    status = true;

error:
    if (encryptionKeyBuffer)
        BKNI_Free(encryptionKeyBuffer);
    if (!playback_ip->openSettings.socketOpenSettings.useProxy && server)
        /* server is only allocated when we are not using proxy server, otherwise it points to the original proxy server buffer from OpenSettings */
        BKNI_Free(server);
    return status;
}

bool
B_PlaybackIp_ParsePlaylistFile(
    B_PlaybackIpHandle playback_ip, 
    PlaylistFileInfo *playlistFileInfo
    )
{
    /* parse the common info first */
    char *nextLine, *tag;
    int nextLineLength = 0;;
    int rc;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
    int nextSeq = 0;
    bool seqTagFound = false;
    bool urlTagFound = false;
    bool versionTagFound = false;
    bool markedDiscontinuity = false;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    char *encryptionKeyUri = NULL;
    EncryptionInfo encryptionInfo;
    bool ivSpecified = false;
    char *tmp, *tmp1, *attribute, *value;
    B_PlaybackIpSecurityProtocol securityProtocol = B_PlaybackIpSecurityProtocol_None;

    BDBG_ASSERT(playlistFileInfo);

    playlistFileInfo->bounded = false;
    playlistFileInfo->mediaSegmentBaseSequence = -1;
    memset(&encryptionInfo, 0, sizeof(encryptionInfo));

    for (
            (nextLine = NULL);
            ((rc = B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer+hlsSession->playlistBufferReadIndex, hlsSession->playlistBufferDepth-hlsSession->playlistBufferReadIndex)) > 0);
            (hlsSession->playlistBufferReadIndex += rc, free(nextLine), nextLine = NULL)
        )
    {
        /* now parse this line */
        BDBG_MSG(("%s: next line is line size %d, index %d, %s", __FUNCTION__, rc, hlsSession->playlistBufferReadIndex, nextLine));
        if ((tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_VERSION_TAG)) != NULL) {
            short version;
            if (versionTagFound == true) {
                BDBG_ERR(("%s: Invalid Playlist file as it contains > 1 %s tag", __FUNCTION__, HLS_M3U8_VERSION_TAG));
                goto error;
            }
            tag += strlen(HLS_M3U8_VERSION_TAG);
            version = strtol(tag, NULL, 10);
            if (version > PLAYLIST_FILE_COMPATIBILITY_VERSION) {
                BDBG_ERR(("%s: Incompatible Playlist Version %d, currently supported %d", __FUNCTION__, version, PLAYLIST_FILE_COMPATIBILITY_VERSION));
                goto error;
            }
            versionTagFound = true;
            playlistFileInfo->version = version;
            BDBG_MSG(("%s: Playlist VERSION %d", __FUNCTION__, playlistFileInfo->version));
        }
        else if ((tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_MAX_MEDIA_SEGMENT_DURATION_TAG)) != NULL) {
            tag += strlen(HLS_M3U8_MAX_MEDIA_SEGMENT_DURATION_TAG);
            playlistFileInfo->maxMediaSegmentDuration = 1000 * strtol(tag, NULL, 10);
            BDBG_MSG(("%s: Max Media Segment Duration %d msec", __FUNCTION__, playlistFileInfo->maxMediaSegmentDuration));
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_MEDIA_SEGMENT_SEQUENCE_TAG)) != NULL) {
            if (seqTagFound) {
                BDBG_ERR(("%s: Invalid Playlist file as it contains > 1 %s tag", __FUNCTION__, HLS_M3U8_MEDIA_SEGMENT_SEQUENCE_TAG));
                goto error;
            }
            tag += strlen(HLS_M3U8_MEDIA_SEGMENT_SEQUENCE_TAG);
            playlistFileInfo->mediaSegmentBaseSequence = strtol(tag, NULL, 10);
            BDBG_MSG(("%s: Media Segment Base Seq# %d", __FUNCTION__, playlistFileInfo->mediaSegmentBaseSequence));
            seqTagFound = true;
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_PROGRAM_RECORD_TAG)) != NULL) {
            tag += strlen(HLS_M3U8_PROGRAM_RECORD_TAG);
            if (B_PlaybackIp_UtilsStristr(tag, "YES") != NULL) {
                playlistFileInfo->allowCaching = true;
                BDBG_MSG(("%s: Media Stream Caching (recording) is allowed (TODO: handle this value!!!!)", __FUNCTION__, tag));
            }
            else {
                playlistFileInfo->allowCaching = false;
                BDBG_MSG(("%s: Media Stream Caching (recording) is NOT allowed (TODO: handle this value!!!!)", __FUNCTION__, tag));
            }
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_URL_TAG)) != NULL) {
            /* URL tag: allocate another media segment info entry and fill in duration & uri */
            tag += strlen(HLS_M3U8_URL_TAG);

            /* allocate/initialize media file segment info structure */
            if ((mediaFileSegmentInfo = (MediaFileSegmentInfo *)BKNI_Malloc(sizeof(MediaFileSegmentInfo))) == NULL) {
                BDBG_ERR(("%s: Failed to allocate %d bytes of memory for media segment info structure", __FUNCTION__, sizeof(MediaFileSegmentInfo)));
                goto error;
            }
            BDBG_MSG(("%s: allocated %d bytes of memory for media segment info: %p", __FUNCTION__, sizeof(MediaFileSegmentInfo), mediaFileSegmentInfo));
            memset(mediaFileSegmentInfo, 0, sizeof(MediaFileSegmentInfo));
            playlistFileInfo->numMediaSegments++;
            if (playlistFileInfo->currentMediaFileSegment == NULL) {
                /* initialize the queue */
                BLST_Q_INIT(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
                playlistFileInfo->currentMediaFileSegment = mediaFileSegmentInfo;
                mediaFileSegmentInfo->mediaSequence = playlistFileInfo->mediaSegmentBaseSequence;
                nextSeq = playlistFileInfo->mediaSegmentBaseSequence + 1;
                BDBG_MSG(("%s: initialized head for the media segment info queue, base seq# %d, mediaFileSegmentInfo entry %p", __FUNCTION__, playlistFileInfo->mediaSegmentBaseSequence, mediaFileSegmentInfo));
            }
            else {
                BDBG_MSG(("%s: seq %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence));
                mediaFileSegmentInfo->mediaSequence = nextSeq++;
            }
            if (markedDiscontinuity == true) {
                mediaFileSegmentInfo->markedDiscontinuity = true;
                markedDiscontinuity = false;
            }
            /* security protocol applies for all subsequent segments for AES protocol */
            mediaFileSegmentInfo->securityProtocol = securityProtocol;

            if ((tmp = strstr(tag, ",")) != NULL) {
                char *tmp1;
                *tmp = '\0';
                /* duration can be either decimal or floating point, convert it into msec */
                if ((tmp1 = strstr(tag, ".")) == NULL) {
                    /* straight integer, convert it into msec */
                    mediaFileSegmentInfo->duration = 1000 * strtol(tag, NULL, 10);
                }
                else {
                    /* floating point */
                    int msec;
                    *tmp1 = '\0';
                    mediaFileSegmentInfo->duration = 1000 * strtol(tag, NULL, 10);
                    tmp1++;
                    msec = 1000 * strtol(tmp1, NULL, 10);
                    if (msec >= 1000)
                        msec /= 1000;
                    mediaFileSegmentInfo->duration += msec;
                }
                BDBG_MSG(("%s: Media Segment Duration %d", __FUNCTION__, mediaFileSegmentInfo->duration));
                if (mediaFileSegmentInfo->duration > playlistFileInfo->maxMediaSegmentDuration) {
                    BDBG_MSG(("%s: Invalid Media Segment Duration %d, Max %d, ignoring it", __FUNCTION__, mediaFileSegmentInfo->duration, playlistFileInfo->maxMediaSegmentDuration));
                }
                playlistFileInfo->totalDuration += mediaFileSegmentInfo->duration;
                playlistFileInfo->initialMinimumReloadDelay = mediaFileSegmentInfo->duration/1000; /* incase this happens to be the last URI */
            }
            else {
                BDBG_ERR(("%s: Invalid Media Segment Duration tag: missing field separate , ", __FUNCTION__, tag));
                goto error;
            }
            urlTagFound = true;
        }
        else if (B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_DISCONTINUITY_TAG) != NULL) {
            markedDiscontinuity = true;
            BDBG_MSG(("%s: Marked discontinuity event set", __FUNCTION__));
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_PROGRAM_DATE_TIME_TAG)) != NULL) {
            tag += strlen(HLS_M3U8_PROGRAM_DATE_TIME_TAG);
            BDBG_MSG(("%s: program date & time tag set: %s (TODO: handle this value!!!!)", __FUNCTION__, tag));
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_ENCRYPTION_KEY_TAG)) != NULL) {
            memset(&encryptionInfo, 0, sizeof(encryptionInfo));
            /* parse the attributes of this tag, its format is:
                EXT-X-KEY:[attribute=value][,attribute=value]* : e.g.
                EXT-X-KEY:METHOD=<method>[,URI="<URI>"][,IV=<IV>]
             */
            for (
                    (tmp = attribute = tag + strlen(HLS_M3U8_ENCRYPTION_KEY_TAG));    /* points to 1st attribute */
                    (tmp && ((tmp = strstr(attribute, "=")) != NULL));
                    (attribute = tmp+1)
                )
            {
                *tmp = '\0'; /* null terminate attribute to convert it into a string */
                value = tmp+1;
                /* multiple attributes are separate by , */
                if ((tmp = strstr(value, ",")) != NULL) {
                    /* another attribute is present following this attribute=value pair */
                    *tmp = '\0';    /* null terminate value to convert it into a string */
                }
                if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD) != NULL) {
                    if (B_PlaybackIp_UtilsStristr(value, HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD_NONE) != NULL) {
                        BDBG_MSG(("%s: Subsequent Media Segments are not encrypted", __FUNCTION__));
                        securityProtocol = B_PlaybackIpSecurityProtocol_None;
                    }
                    else if (B_PlaybackIp_UtilsStristr(value, HLS_M3U8_TAG_ATTRIBUTE_ENC_METHOD_AES128) != NULL) {
                        BDBG_MSG(("%s: Subsequent Media Segments are AES128 encrypted", __FUNCTION__));
                        securityProtocol = B_PlaybackIpSecurityProtocol_Aes128;
                    }
                    else {
                        BDBG_ERR(("%s: ERROR: invalid KEY Tag: Encryption Method attribute value %s is not supported", __FUNCTION__, value));
                        goto error;
                    }
                }
                else if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_ENC_URI) != NULL) {
                    if (securityProtocol != B_PlaybackIpSecurityProtocol_Aes128) {
                        BDBG_ERR(("%s: ERROR: invalid KEY Tag: Encryption URI specified for non-AES security protocols", __FUNCTION__));
                        goto error;
                    }
                    /* uri is encapsulated in "", skip them */
                    value += 1; /* skip the 1st " char */
                    if ((tmp1 = strstr(value, "\"")) != NULL) {
                        *tmp1 = '\0'; /* leave out the last " char */
                    }
                    if (http_absolute_uri(value)) {
                        /* mediaSegment URI is absolute uri     */
                        BDBG_MSG(("%s: URI (%s) for media segment encryption key is absolute", __FUNCTION__, value));
                        if ((encryptionKeyUri = strdup(value)) == NULL) {
                            BDBG_ERR(("strdup failed at %s:%d", __FUNCTION__, __LINE__));
                            goto error;
                        }
                    }
                    else {
                        /* relative url, build complete uri using server ip address & port # */
                        BDBG_MSG(("%s: URI (%s) for media segment encryption key not is absolute", __FUNCTION__, value));
                        if ((encryptionKeyUri = B_PlaybackIp_HlsBuildAbsoluteUri(playlistFileInfo->server, playlistFileInfo->port, playlistFileInfo->uri, value)) == NULL) {
                            BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
                            goto error;
                        }
                    }
                    /* got the URI for the key file, now download & store the key file */
                    if (hls_downloadEncryptionKey(playback_ip, securityProtocol, encryptionKeyUri, &encryptionInfo) == false) {
                        BDBG_ERR(("Failed to download the encryption key at %s:%d", __FUNCTION__, __LINE__));
                        goto error;
                    }
                }
                else if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_ENC_IV) != NULL) {
                    int ivLength, i;
                    /* copy the IV */
                    if (securityProtocol == B_PlaybackIpSecurityProtocol_Aes128) {
                        if (value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
                            BDBG_ERR(("%s: IV must begin w/ 0x or 0X, it is %c%c", __FUNCTION__, value[0], value[1]));
                            goto error;
                        }
                        value +=2; /* skips 0x */
                        ivLength = strlen(value);
                        if (ivLength/2 > 16) {
                            BDBG_ERR(("%s: IV length %d > 16", __FUNCTION__, ivLength/2));
                            goto error;
                        }
                        for (i=0; i<ivLength; i+=2) {
                            unsigned char hi, lo;
                            if (hls_asciiHexToDec(value[i], &hi) != true || hls_asciiHexToDec(value[i+1], &lo) != true) {
                                BDBG_ERR(("%s: Failed to convert char %c, %c", __FUNCTION__, value[i], value[i+1]));
                                goto error;
                            }
                            encryptionInfo.encryptionMethod.aes128.iv[i/2] = (hi<<4)|lo;
                        }
                        for (i=0; i<16;i++) {
                            BDBG_MSG(("iv[%d] %02x", i, encryptionInfo.encryptionMethod.aes128.iv[i]));
                        }
                        ivSpecified = true;
                    }
                    else {
                        BDBG_ERR(("%s: IV is not yet supported for this encryption method %d", __FUNCTION__, securityProtocol));
                        goto error;
                    }
                }
                else {
                    BDBG_ERR(("%s: ERROR: undefined Attribute: %s", __FUNCTION__, attribute));
                    goto error;
                }
            }    /* end of for loop for parsing all attributes */
            if (!ivSpecified) {
                /* IV is not explicitly provided, HLS spec requires media segment sequence number to be used as the IV, set a flag to indicate that */
                encryptionInfo.useMediaSegSeqAsIv = true;
            }
            else {
                encryptionInfo.useMediaSegSeqAsIv = false;
            }
            BDBG_MSG(("%s: encryption key tag is successfully parsed", __FUNCTION__));
        }
        else if (urlTagFound == true) {
            /* we have processed all tags in-between URL tag and actual URL, so now copy the actual URL */
            if (http_absolute_uri(nextLine)) {
                /* mediaSegment URI is absolute uri */
                BDBG_MSG(("%s: media segment URI is absolute", __FUNCTION__));
                if ((mediaFileSegmentInfo->absoluteUri = strdup(nextLine)) == NULL) {
                    BDBG_ERR(("strdup failed at %s:%d", __FUNCTION__, __LINE__));
                    goto error;
                }
            }
            else {
                /* relative url, build complete uri using server ip address & port # */
                BDBG_MSG(("%s: media segment URI is not absolute URI", __FUNCTION__));
                if ((mediaFileSegmentInfo->absoluteUri = B_PlaybackIp_HlsBuildAbsoluteUri(playlistFileInfo->server, playlistFileInfo->port, playlistFileInfo->uri, nextLine)) == NULL) {
                    BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
                    goto error;
                }
            }
            if ((hls_parse_url(&mediaFileSegmentInfo->protocol, &mediaFileSegmentInfo->server, &mediaFileSegmentInfo->port, &mediaFileSegmentInfo->uri, mediaFileSegmentInfo->absoluteUri) == false) || (mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttp && mediaFileSegmentInfo->protocol != B_PlaybackIpProtocol_eHttps)) {
                BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
                goto error;
            }
            mediaFileSegmentInfo->encryptionInfo = encryptionInfo;
            if (mediaFileSegmentInfo->protocol == B_PlaybackIpProtocol_eHttps) {
                mediaFileSegmentInfo->securityProtocol = B_PlaybackIpSecurityProtocol_Ssl;
                /* we reset the securityProtocol as for HTTPs, it is determined by the URL and thus it should be reset */
                securityProtocol = B_PlaybackIpSecurityProtocol_None;
                BDBG_MSG(("%s: HLS Session is using HTTPS Encryption Method", __FUNCTION__));
            }
            else if (mediaFileSegmentInfo->securityProtocol == B_PlaybackIpSecurityProtocol_Aes128) {
                if (encryptionInfo.useMediaSegSeqAsIv == true) {
                    /* use seq # as the IV */
                        mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[0] = (unsigned char)(mediaFileSegmentInfo->mediaSequence >> 24);
                        mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[1] = (unsigned char)(mediaFileSegmentInfo->mediaSequence >> 16);
                        mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[2] = (unsigned char)(mediaFileSegmentInfo->mediaSequence >> 8);
                        mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[3] = (unsigned char)(mediaFileSegmentInfo->mediaSequence >> 0);
                    BDBG_MSG(("%s: explicit IV not set, using seq # %d as iv: iv[0] %02x, iv[1] %02x, iv[2] %02x, iv[3] %02x", __FUNCTION__, mediaFileSegmentInfo->mediaSequence,
                                mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[0], mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[1], mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[2], mediaFileSegmentInfo->encryptionInfo.encryptionMethod.aes128.iv[3]));  
                }
            }
            else if (mediaFileSegmentInfo->securityProtocol != B_PlaybackIpSecurityProtocol_None) {
                BDBG_ERR(("%s: Encryption Method %d is not yet supported", __FUNCTION__, mediaFileSegmentInfo->securityProtocol));
                goto error;
            }
            /* now insert this entry into the queue of media file segments */
            BLST_Q_INSERT_TAIL(&playlistFileInfo->mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
            urlTagFound = false;
            mediaFileSegmentInfo = NULL; /* as this entry is now already inserted into the list of segment URIs */
        }
        else if (B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_END_TAG) != NULL) {
            playlistFileInfo->bounded = true;
            BDBG_MSG(("%s: Playlist is bounded and thus not live stream", __FUNCTION__));
            break;
        }
        else if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, "#EXT")) != NULL) {
            BDBG_MSG(("%s: !!!! TODO: Yet to parse this tag %s!!!! ", __FUNCTION__, tag));
        }
        else {
            BDBG_MSG(("%s: Comment line %s, ignore it", __FUNCTION__, tag));
        }
    }

    if (playlistFileInfo->numMediaSegments == 0) {
        BDBG_ERR(("%s: Invalid Playlist file as it doesn't contains any media segment URLs", __FUNCTION__));
        goto error;
    }

    mediaFileSegmentInfo = NULL;
    /* TODO: if the EXT-X-MEDIA-SEQUENCE is not set, then we may have to make sure that the updated playlist still contains the current uri (which is being played) */
    /* HLS_M3U8_MAX_MEDIA_SEGMENT_DURATION_TAG must be present only once */
    if (playlistFileInfo->maxMediaSegmentDuration == 0) {
        BDBG_MSG(("%s: Playlist file doesn't contains %s tag", __FUNCTION__, HLS_M3U8_MEDIA_SEGMENT_SEQUENCE_TAG));
    }

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: finished parsing playlist file w/ uri %s, total duration %d, bounded %s, numMediaSegments %d", 
                __FUNCTION__, playlistFileInfo->absoluteUri, playlistFileInfo->totalDuration, playlistFileInfo->bounded ? "YES" : "NO", playlistFileInfo->numMediaSegments));
#endif

    if (encryptionKeyUri)
        free(encryptionKeyUri);
    return true;

error:
    /* free up resources */
    if (mediaFileSegmentInfo) {
        BDBG_MSG(("%s: freeing mediaFileSegmentInfo entry %p", __FUNCTION__, mediaFileSegmentInfo));
        if (mediaFileSegmentInfo->uri)
            free(mediaFileSegmentInfo->uri);
        free(mediaFileSegmentInfo);
    }
    if (encryptionKeyUri)
        free(encryptionKeyUri);
    return false;
}

#ifdef HLS_UNIT_TEST
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
B_PlaybackIp_HlsRemovePlaylistInfoEntry(HlsSessionState *hlsSession, PlaylistFileInfo *currentPlaylistFileInfo)
{
    PlaylistFileInfo *playlistFileInfo;
    BDBG_MSG(("%s: remove playlist entry %p", __FUNCTION__, currentPlaylistFileInfo));
    for (playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
         playlistFileInfo && playlistFileInfo != currentPlaylistFileInfo;
         playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next))
    { ; }
    if (playlistFileInfo) {
        /* entry found, remove it */
        BDBG_MSG(("%s: removing playlist entry: current %p, entry %p", __FUNCTION__, currentPlaylistFileInfo, playlistFileInfo));
        BLST_Q_REMOVE(&hlsSession->playlistFileInfoQueueHead, playlistFileInfo, next);
    }
    else {
        BDBG_ERR(("%s: SW BUG: entry to be removed %p not found in the list, current entry %p", __FUNCTION__, currentPlaylistFileInfo, playlistFileInfo));
        BDBG_ASSERT(NULL);
    }
}

/* insert the new the playlistFileInfoEntry in the list which is sorted (ascending order) by the b/w of the playlist entries */
/* caller is required to lock the playlist */
void
B_PlaybackIp_HlsInsertPlaylistInfoEntry(HlsSessionState *hlsSession, PlaylistFileInfo *newPlaylistFileInfo)
{
    PlaylistFileInfo *playlistFileInfo;
    BDBG_MSG(("%s: insert playlist entry %p in the ascending order of bandwidth (%d)", __FUNCTION__, newPlaylistFileInfo, newPlaylistFileInfo->bandwidth));
    for (playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
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
        BLST_Q_INSERT_BEFORE(&hlsSession->playlistFileInfoQueueHead, playlistFileInfo, newPlaylistFileInfo, next);
    }
    else {
        /* insert it at the tail */
        BDBG_MSG(("%s: inserting playlist entry %p at the tail", __FUNCTION__, newPlaylistFileInfo));
        BLST_Q_INSERT_TAIL(&hlsSession->playlistFileInfoQueueHead, newPlaylistFileInfo, next);
    }
}

/* connect to the server, download the complete playlist file and then parse it to build the list of media segments */
int
B_PlaybackIp_HlsConnectDownloadAndParsePlaylistFile(
    B_PlaybackIpHandle playback_ip,
    HlsSessionState *hlsSession,
    PlaylistFileInfo *playlistFileInfo
    )
{
    B_PlaybackIpHandle playbackIp; /* temp playback ip session */

    /* Need to create a new temporary playback IP session as playlist may be downloaded using a different security parameters */
    /* than the current playback ip session (which corresponds to top level m3u8 URL) */
    playbackIp = B_Playback_HlsCreatePlaybackIpSession(playback_ip, playlistFileInfo->server, playlistFileInfo->port, &playlistFileInfo->uri, playlistFileInfo->protocol, playback_ip->openSettings.security.initialSecurityContext);
    if (!playbackIp) {
        goto error;
    }
    playbackIp->openSettings.u.http.userAgent = playback_ip->openSettings.u.http.userAgent;
    /* update its state to playing so that playback ip util functions can do HTTP processing (handle redirects, Http Chunk Xfer Encoding, etc.) and provide just the payload of HTTP */
    playbackIp->playback_state = B_PlaybackIpState_ePlaying;
    playbackIp->hlsSessionState = hlsSession;

    BDBG_MSG(("%s: Connected to the server using the new URL, now download the playlist, initial data %d", __FUNCTION__, playbackIp->initial_data_len));
    if (playbackIp->initial_data_len) {
        BKNI_Memcpy(hlsSession->playlistBuffer, playbackIp->temp_buf, playbackIp->initial_data_len);
        hlsSession->playlistBufferDepth = playbackIp->initial_data_len;
        playbackIp->initial_data_len = 0;
    }
    else {
        hlsSession->playlistBufferDepth = 0;
    }

    hlsSession->playlistSocketFd = playbackIp->socketState.fd;
    if (B_PlaybackIp_HlsDownloadFile(playbackIp, hlsSession->playlistSocketFd, hlsSession->playlistBuffer, hlsSession->playlistBufferSize, &hlsSession->playlistBufferDepth, true) != true) {
        BDBG_ERR(("%s: failed to download the M3U playlist file", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: downloaded playlist file of %d bytes", __FUNCTION__, hlsSession->playlistBufferDepth));

    hlsSession->playlistBufferReadIndex = 0;

    /* verify that playlist file is in the required Extended M3U8 format */
    if (B_PlaybackIp_IsPlaylistInExtendedM3uFormat(playbackIp) != true) {
        goto error;
    }

    if (B_PlaybackIp_ParsePlaylistFile(playbackIp, playlistFileInfo) != true) {
        BDBG_ERR(("%s: failed to parse the M3U playlist file", __FUNCTION__));
        goto error;
    }

    /* now destroy this temp playback ip session */
    B_Playback_HlsDestroyPlaybackIpSession(playbackIp);
    return 0;

error:
    /* destroy this temp playback ip session */
    if (playbackIp) {
        B_Playback_HlsDestroyPlaybackIpSession(playbackIp);
    }
    return -1;
}

#ifdef HLS_UNIT_TEST
static bool
printVariantPlaylistFile(HlsSessionState *hlsSession)
{
    /* parse the common info first */
    PlaylistFileInfo *playlistFileInfo;
    
    BDBG_MSG(("%s: playlist file q head %p, current playlist file %p", __FUNCTION__, &hlsSession->playlistFileInfoQueueHead, hlsSession->currentPlaylistFile));
    for (playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
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

/* parses the downloaded file to see if it a simple or variant playlist */
/* for simple playlist, it just creates the playlistFileInfo structure and returns that */
/* for variant playlist, it parses the variant playlist file and builds the list of playlists */
PlaylistFileInfo *
B_PlaybackIp_ParsePlaylistVariantFile(B_PlaybackIpHandle playback_ip, bool *parsingSuccess)
{
    /* parse the common info first */
    char *nextLine = NULL, *tag;
    int nextLineLength = 0;
    int rc;
    int programId = -1;
    unsigned smallestBandwidth = 0;
    PlaylistFileInfo *playlistFileInfo = NULL;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    bool versionTagFound = false; 
    short version = PLAYLIST_FILE_COMPATIBILITY_VERSION;

    *parsingSuccess = false;

    /* make sure we ignore any initial comments in the playlist file */
    while (true) {
        if ((rc =B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer+hlsSession->playlistBufferReadIndex, hlsSession->playlistBufferDepth-hlsSession->playlistBufferReadIndex)) < 0) {
            BDBG_ERR(("%s: Failed to get the next line from the uri file", __FUNCTION__));
            return NULL;
        }
        BDBG_MSG(("%s: next line %s", __FUNCTION__, nextLine));
        /* any lines that dont start w/ #EXT are comments, so we need to ignore them */
        if ((B_PlaybackIp_UtilsStristr(nextLine, "#EXT")) != NULL) {
            /* line begins w/ #EXT tag, so we are good */
            break;
        }
        /* comment line, ignore it */
        hlsSession->playlistBufferReadIndex += rc;
    }

    BDBG_MSG(("%s: next line %s", __FUNCTION__, nextLine));
    if ((tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_VERSION_TAG)) != NULL) {
        if (versionTagFound == true) {
            BDBG_ERR(("%s: Invalid Playlist file as it contains > 1 %s tag", __FUNCTION__, HLS_M3U8_VERSION_TAG));
            goto error;
        }
        tag += strlen(HLS_M3U8_VERSION_TAG);
        version = strtol(tag, NULL, 10);
        if (version > PLAYLIST_FILE_COMPATIBILITY_VERSION) {
            BDBG_ERR(("%s: Incompatible Playlist Version %d, currently supported %d", __FUNCTION__, version, PLAYLIST_FILE_COMPATIBILITY_VERSION));
            goto error;
        }
        versionTagFound = true;
        BDBG_MSG(("%s: Playlist VERSION %d", __FUNCTION__, version));
        hlsSession->playlistBufferReadIndex += rc;
        if ((rc =B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer+hlsSession->playlistBufferReadIndex, hlsSession->playlistBufferDepth-hlsSession->playlistBufferReadIndex)) < 0) {
            BDBG_ERR(("%s: Failed to get the next line from the uri file", __FUNCTION__));
            return NULL;
        }

        BDBG_MSG(("%s: next line %s", __FUNCTION__, nextLine));
    }
    /* Check to see if media url points to a variant playlist file */
    if (B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_STREAM_INFO_TAG) == NULL) {
        /* variant playinfo tag is missing, so uri contains the actual playlist */ 
        if (hlsSession->currentPlaylistFile != NULL) {
            BDBG_MSG(("%s: current file is not a variant playlist file and variant playlist is already created, return", __FUNCTION__));
            return NULL;
        }

        /* allocate/initialize playlist file structure and return */
        if ( (playlistFileInfo = (PlaylistFileInfo *)BKNI_Malloc(sizeof(PlaylistFileInfo))) == NULL) {
            BDBG_ERR(("%s: Failed to allocate %d bytes of memory for playlistInfo file structure", __FUNCTION__, sizeof(PlaylistFileInfo)));
            return NULL;
        }
        memset(playlistFileInfo, 0, sizeof(PlaylistFileInfo));
        /* build the uri of the playlist file */
        if ((playlistFileInfo->absoluteUri = B_PlaybackIp_HlsBuildAbsoluteUri(playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, ""/* empty base uri*/, playback_ip->openSettings.socketOpenSettings.url)) == NULL) {
            BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
            goto error;
        }
        if ((hls_parse_url(&playlistFileInfo->protocol, &playlistFileInfo->server, &playlistFileInfo->port, &playlistFileInfo->uri, playlistFileInfo->absoluteUri) == false) || (playlistFileInfo->protocol != B_PlaybackIpProtocol_eHttp && playlistFileInfo->protocol != B_PlaybackIpProtocol_eHttps)) {
            BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
            goto error;
        }
        if (versionTagFound)
            playlistFileInfo->version = version;
        else
            playlistFileInfo->version = PLAYLIST_FILE_COMPATIBILITY_VERSION;
        /* rest of fields are 0 */

        /* initialize the queue */
        BLST_Q_INIT(&hlsSession->playlistFileInfoQueueHead);
        /* in the case of simple playlist file, it will only have element */
        BLST_Q_INSERT_TAIL(&hlsSession->playlistFileInfoQueueHead, playlistFileInfo, next);
        hlsSession->currentPlaylistFile = playlistFileInfo;
        BDBG_MSG(("%s: given uri (%s) is not a variant playlist file (%p)", __FUNCTION__, playlistFileInfo->absoluteUri, playlistFileInfo));
        free(nextLine);
        *parsingSuccess = true;
        return playlistFileInfo;
    }

    /* parse the variant playlist file */
    for (
            (nextLine = NULL);
            ((rc = B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer+hlsSession->playlistBufferReadIndex, hlsSession->playlistBufferDepth-hlsSession->playlistBufferReadIndex)) > 0);
            (hlsSession->playlistBufferReadIndex += rc, free(nextLine), nextLine = NULL)
        )
    {
        /* now parse this line */
        BDBG_MSG(("%s: next line is line size %d, index %d, %s", __FUNCTION__, rc, hlsSession->playlistBufferReadIndex, nextLine));
        if ( (tag = B_PlaybackIp_UtilsStristr(nextLine, HLS_M3U8_STREAM_INFO_TAG)) != NULL) {
            char *tmp, *attribute;
            /* variant stream info tag: allocate & initialize another playlistInfo entry */
            if ((playlistFileInfo = (PlaylistFileInfo *)BKNI_Malloc(sizeof(PlaylistFileInfo))) == NULL) {
                BDBG_ERR(("%s: Failed to allocate %d bytes of memory for playlistInfo file structure", __FUNCTION__, sizeof(PlaylistFileInfo)));
                return NULL;
            }
            memset(playlistFileInfo, 0, sizeof(PlaylistFileInfo));
            playlistFileInfo->version = PLAYLIST_FILE_COMPATIBILITY_VERSION;
            /* parse the attributes of this tag, its format is:
                EXT-X-STREAM-INF:[attribute=value][,attribute=value]* 
                URI 
             */
            for (
                    (tmp = attribute = tag + strlen(HLS_M3U8_STREAM_INFO_TAG));    /* points to 1st attribute */
                    (tmp && ((tmp = strstr(attribute, "=")) != NULL));
                    (attribute = tmp+1)
                )
            {
                char *value;
                *tmp = '\0'; /* null terminate attribute to convert it into a string */
                value = tmp+1;
                /* multiple attributes are separate by , */
                if ((tmp = strstr(value, ",")) != NULL) {
                    /* another attribute is present following this attribute=value pair */
                    *tmp = '\0';    /* null terminate value to convert it into a string */
                }
                if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_PROGRAM_ID) != NULL) {
                    playlistFileInfo->programId = strtol(value, NULL, 10);
                    if (programId == -1)
                        programId = playlistFileInfo->programId;
                    else {
                        if (programId != playlistFileInfo->programId) {
                            BDBG_ERR(("%s: no support for multiple programs yet: Program ID current %d, new %d", __FUNCTION__, programId, playlistFileInfo->programId));
                            /* TODO: goto free up current entry and move to the next program entry */
                            goto error;
                        }
                    }
                    BDBG_MSG(("%s: Program ID %d", __FUNCTION__, playlistFileInfo->programId));
                }
                else if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_BANDWIDTH) != NULL) {
                    playlistFileInfo->bandwidth = strtol(value, NULL, 10);
                    if (playlistFileInfo->bandwidth == 0) {
                        BDBG_ERR(("%s: invalid bandwidth (%d) in the variant playlist, ignoring it", __FUNCTION__, playlistFileInfo->bandwidth));
                    }
                    BDBG_MSG(("%s: Bandwidth %d", __FUNCTION__, playlistFileInfo->bandwidth));
                }
                else if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_CODECS) != NULL) {
                    if (tmp)
                        /* codec attribute can have 1 or more values, each value is separated by a comma, replace this comma is present w/ \0 , and then look for ending " */
                        *tmp = ',';
                    BDBG_MSG(("%s: TODO: need to parse attribute %s=%s", __FUNCTION__, HLS_M3U8_TAG_ATTRIBUTE_CODECS, value));
                    /* The syntax for CODECS attribute is: CODECS="[format][,format]" */
                    /* so we need to skip the initial " char & then jump to the next " char */
                    value += 1; /* skips starting " char */
                    if ((tmp = strstr(value, "\"")) != NULL) {
                        *tmp = '\0';
                        BDBG_MSG(("Codec string: %s", value));
                    }
                    else {
                        /* missing ending " char, so it an error as CODECS attribute is enclosed in the " " */
                        BDBG_ERR(("%s: missing ending \" char, so it an error as CODECS attribute is enclosed in the \" \", attribute: %s", __FUNCTION__, value));
                        goto error;
                    }
                }
                else if (B_PlaybackIp_UtilsStristr(attribute, HLS_M3U8_TAG_ATTRIBUTE_RESOLUTION) != NULL) {
                    BDBG_MSG(("%s: TODO: need to parse attribute %s=%s", __FUNCTION__, HLS_M3U8_TAG_ATTRIBUTE_RESOLUTION, value));
                }
                else {
                    BDBG_ERR(("%s: unsupported/unknown attribute %s=%s, ignoring it for now!!", __FUNCTION__, attribute, value));
                }
                BDBG_MSG(("%s: tmp %p, attribute %p, %s", __FUNCTION__, tmp, attribute, attribute));
            }

            /* finished parsing the STREAM_INFO TAG & its attributes, next line is the uri of the playlist file, parse that */
            hlsSession->playlistBufferReadIndex += rc;
            free(nextLine);
            nextLine = NULL;
            if ((rc =B_PlaybackIp_UtilsGetLine(&nextLine, &nextLineLength, hlsSession->playlistBuffer+hlsSession->playlistBufferReadIndex, hlsSession->playlistBufferDepth-hlsSession->playlistBufferReadIndex)) > 0) {
                if (http_absolute_uri(nextLine)) {
                    /* playlist contains the absolute uri, copy it */
                    BDBG_MSG(("%s: absolute URI: %s", __FUNCTION__, nextLine));
                    if ((playlistFileInfo->absoluteUri = strdup(nextLine)) == NULL) {
                        BDBG_ERR(("strdup failed at %s:%d", __FUNCTION__, __LINE__));
                        goto error;
                    }
                }
                else {
                    /* relative url, build complete uri using server ip address & port # */
                    BDBG_MSG(("%s: %s is not absolute URI", __FUNCTION__, nextLine));
                    if ((playlistFileInfo->absoluteUri = B_PlaybackIp_HlsBuildAbsoluteUri(playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, playback_ip->openSettings.socketOpenSettings.url, nextLine)) == NULL) {
                        BDBG_ERR(("Failed to build URI at %s:%d", __FUNCTION__, __LINE__));
                        goto error;
                    }
                }
                if (hls_parse_url(&playlistFileInfo->protocol, &playlistFileInfo->server, &playlistFileInfo->port, &playlistFileInfo->uri, playlistFileInfo->absoluteUri) == false) {
                    BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
                    goto error;
                }
                BDBG_MSG(("%s: Variant Playlist: server %s, port %d, protocol %d, uri %s, b/w %d, program id %d", __FUNCTION__, playlistFileInfo->server, playlistFileInfo->port, playlistFileInfo->protocol, playlistFileInfo->uri, playlistFileInfo->bandwidth, playlistFileInfo->programId));
            }
            else {
                BDBG_ERR(("Failed to get next line at %s:%d", __FUNCTION__, __LINE__));
                goto error;
            }

            /* initialize the queue */
            if (hlsSession->currentPlaylistFile == NULL) {
                BLST_Q_INIT(&hlsSession->playlistFileInfoQueueHead);
                hlsSession->currentPlaylistFile = playlistFileInfo;
                smallestBandwidth = playlistFileInfo->bandwidth;
                BDBG_MSG(("%s: initialized head for the playlist info queue, playlistInfo entry %p, bandwidth %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->bandwidth));
            }
            else {
                /* bitrate check here */
                if (smallestBandwidth > playlistFileInfo->bandwidth) {
                    BDBG_MSG(("%s: playlistInfo entry %p has bandwidth %d smaller than previous one %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->bandwidth, smallestBandwidth));
                    smallestBandwidth = playlistFileInfo->bandwidth;
                    hlsSession->currentPlaylistFile = playlistFileInfo;
                }
            }

            /* now insert this entry into the queue of playlist files */
            BKNI_AcquireMutex(hlsSession->lock);
            B_PlaybackIp_HlsInsertPlaylistInfoEntry(hlsSession, playlistFileInfo);
            BKNI_ReleaseMutex(hlsSession->lock);
            playlistFileInfo = NULL; /* reset pointer as this playlist is successfully inserted into the hlsSession */
        }
        else {
            BDBG_MSG(("%s: Comment line %s, ignore it", __FUNCTION__, tag));
        }
    }
    BDBG_MSG(("%s: finished parsing variant playlist file", __FUNCTION__));
    *parsingSuccess = true;
    return NULL;

error:
    B_PlaybackIp_FreePlaylistInfoAll(playback_ip);
    if (playlistFileInfo)
        B_PlaybackIp_FreePlaylistInfo(playlistFileInfo);
    *parsingSuccess = false;
    return NULL;
}

/* select a playlist, download & parse it, playlist is selected matching closely with current n/w b/w */
PlaylistFileInfo *
B_PlaybackIp_HlsSelectDownloadAndParsePlaylist(
    B_PlaybackIpHandle playback_ip,
    HlsSessionState *hlsSession
    )
{
    int networkBandwidth = 0;
    PlaylistFileInfo *playlistFileInfo = NULL;
    PlaylistFileInfo *prevPlaylistFileInfo = NULL;

    /* NOTE: we are no longer downloading all playlists in one go as it has two disadvantages: */
    /* 1) increases latency when n/w b/w is slow to a server, */ 
    /* 2) we anyway need to redownload the playlist when we later switch */
    /* only advantage is for bounded playlists where we dont need to redownload the playlist, but that can be ignored for the initial latency improval */

    /* just pick the playlist w/ the highest bitrate (capped by the max download speed) and download its playlist */
    /* we use the playlist w/ highest bitrate to determine the n/w b/w (note: playlist is created in the ascending order */
    for (prevPlaylistFileInfo = NULL, playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
         playlistFileInfo;
         prevPlaylistFileInfo=playlistFileInfo, playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next)) 
    {
        if (playlistFileInfo->bandwidth >= hlsSession->maxNetworkBandwidth) {
            break;
        }
    }
    if (playlistFileInfo == NULL)
        playlistFileInfo = prevPlaylistFileInfo;

    /* download and parse the playlist */
    if (B_PlaybackIp_HlsConnectDownloadAndParsePlaylistFile(playback_ip, hlsSession, playlistFileInfo) < 0) {
        BDBG_ERR(("%s: Failed to download & parse playlist file entry w/ urip %p, uri %s", __FUNCTION__, playlistFileInfo->uri, playlistFileInfo->uri));
        goto error;
    }

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: using playlist file entry %p, uri %s, b/w %d, network b/w %d", __FUNCTION__, playlistFileInfo, playlistFileInfo->absoluteUri, playlistFileInfo->bandwidth, networkBandwidth));
#endif
    hlsSession->currentPlaylistFile = playlistFileInfo;
    return playlistFileInfo;
error:
    return NULL;
}

void 
B_PlaybackIp_HlsSegmentBufferDestroy(
    B_PlaybackIpHandle playback_ip
    )
{
    int i;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;

    for (i=0; i<HLS_NUM_SEGMENT_BUFFERS && hlsSession->segmentBuffer[i].buffer; i++) {
        if (hlsSession->segmentBuffer[i].lock) {
            BKNI_DestroyMutex(hlsSession->segmentBuffer[i].lock);
            hlsSession->segmentBuffer[i].lock = NULL;
        }
    }
}

void
B_PlaybackIp_HlsSessionDestroy(B_PlaybackIpHandle playback_ip)
{
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    B_PlaybackIp_FreePlaylistInfoAll(playback_ip);

    if (!hlsSession)
        return;

    if (hlsSession->bufferFilledEvent) {
        BKNI_DestroyEvent(hlsSession->bufferFilledEvent);
        hlsSession->bufferFilledEvent = NULL;
    }
    if (hlsSession->bufferEmptiedEvent) {
        BKNI_DestroyEvent(hlsSession->bufferEmptiedEvent);
        hlsSession->bufferEmptiedEvent = NULL;
    }
    if (hlsSession->reDownloadPlaylistEvent) {
        BKNI_DestroyEvent(hlsSession->reDownloadPlaylistEvent);
        hlsSession->reDownloadPlaylistEvent = NULL;
    }
    if (hlsSession->segDownloadThreadPausedEvent) {
        BKNI_DestroyEvent(hlsSession->segDownloadThreadPausedEvent);
        hlsSession->segDownloadThreadPausedEvent = NULL;
    }
    if (hlsSession->playbackThreadPausedEvent) {
        BKNI_DestroyEvent(hlsSession->playbackThreadPausedEvent);
        hlsSession->playbackThreadPausedEvent = NULL;
    }
    if (hlsSession->segDownloadThreadPauseDoneEvent) {
        BKNI_DestroyEvent(hlsSession->segDownloadThreadPauseDoneEvent);
        hlsSession->segDownloadThreadPauseDoneEvent = NULL;
    }
    if (hlsSession->playbackThreadPauseDoneEvent) {
        BKNI_DestroyEvent(hlsSession->playbackThreadPauseDoneEvent);
        hlsSession->playbackThreadPauseDoneEvent = NULL;
    }
    if (hlsSession->lock) {
        BKNI_DestroyMutex(hlsSession->lock);
        hlsSession->lock = NULL;
    }
    B_PlaybackIp_HlsSegmentBufferDestroy(playback_ip);

    if (hlsSession->playlistSocketFd) {
        close(hlsSession->playlistSocketFd);
        hlsSession->playlistSocketFd = 0;
    }

    if (hlsSession->playlistBuffer) {
        BKNI_Free(hlsSession->playlistBuffer);
        hlsSession->playlistBuffer = NULL;
    }

    BKNI_Free(hlsSession);
    playback_ip->hlsSessionState = NULL;
}

/* function to setup a HLS Session: */
/*  -downloads variant and simple playlists */
/*  -connects to the server for the 1st media segment and gets playback_ip session for media probing */
int
B_PlaybackIp_HlsSessionSetup(B_PlaybackIpHandle playback_ip, char *http_hdr)
{
    char *pValue;
    MediaFileSegmentInfo *mediaFileSegmentInfo;
    PlaylistFileInfo *playlistFileInfo;
    HlsSessionState *hlsSession; 
    bool parsingSuccess = false;
    int initial_data_len = 0;

    if (B_PlaybackIp_IsHlsSession(playback_ip->openSettings.socketOpenSettings.url, http_hdr) != true) {
        BDBG_MSG(("%s: Not a HTTP Live Streaming (HLS) Session", __FUNCTION__));
        return 0;
    }

    /* Now it is a HLS session, do further checks to validate playlist file format */
    BDBG_MSG(("%s: HTTP Live Streaming (HLS) Session: download & parse playlist", __FUNCTION__));

    /* allocate hls session state */
    if ((playback_ip->hlsSessionState = (HlsSessionState *)BKNI_Malloc(sizeof(HlsSessionState))) == NULL) {
        BDBG_ERR(("%s: failed to allocate %d bytes for HLS Session state", __FUNCTION__, sizeof(HlsSessionState)));
        goto error;
    }
    hlsSession = playback_ip->hlsSessionState;
    BKNI_Memset(hlsSession, 0, sizeof(HlsSessionState));

    /* allocate a buffer where playlist file will be completely downloaded */
    if ((hlsSession->playlistBuffer = (char *)BKNI_Malloc(HLS_PLAYLIST_FILE_SIZE)) == NULL) {
        BDBG_ERR(("%s: failed to allocate %d bytes for downloading the playlist file", __FUNCTION__, HLS_PLAYLIST_FILE_SIZE));
        goto error;
    }
    hlsSession->playlistBufferSize = HLS_PLAYLIST_FILE_SIZE;

    /* copy any initial payload data (read part of the initial HTTP response) into the playlist buffer */
    initial_data_len = playback_ip->chunkPayloadLength ? playback_ip->chunkPayloadLength : playback_ip->initial_data_len;
    if (initial_data_len) {
        memcpy(hlsSession->playlistBuffer, playback_ip->temp_buf, initial_data_len);
        hlsSession->playlistBufferDepth = initial_data_len;
        playback_ip->initial_data_len = 0;
        playback_ip->chunkPayloadLength = 0;
    }
    else {
        hlsSession->playlistBufferDepth = 0;
    }

    pValue = getenv("max_download_bitrate");
    if (pValue) {
        hlsSession->maxNetworkBandwidth = strtoul(pValue, NULL, 0);
    }
#define PLAYBACK_IP_MAX_NETWORK_BANDWIDTH 5800000 /* set to 1.8Mpbs */
    if (!pValue || hlsSession->maxNetworkBandwidth == 0) {
        hlsSession->maxNetworkBandwidth = PLAYBACK_IP_MAX_NETWORK_BANDWIDTH;
    }
    BDBG_MSG(("%s: max network bandwidth set to %d", __FUNCTION__, hlsSession->maxNetworkBandwidth));

    /* download the playlist file: we dont know whether it is a simple playlist file or a variant playlist file */
    if (B_PlaybackIp_HlsDownloadFile(playback_ip, playback_ip->socketState.fd, hlsSession->playlistBuffer, hlsSession->playlistBufferSize, &hlsSession->playlistBufferDepth, true) != true) {
        BDBG_ERR(("%s: failed to download the M3U playlist file", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: downloaded playlist file of %d bytes", __FUNCTION__, hlsSession->playlistBufferDepth));
    hlsSession->playlistBufferReadIndex = 0;

#ifdef HLS_UNIT_TEST
    dump_playlist_file(hlsSession);
#endif

    /* verify that playlist file is in the required Extended M3U8 format */
    if (B_PlaybackIp_IsPlaylistInExtendedM3uFormat(playback_ip) != true) {
        goto error;
    }

    if (BKNI_CreateMutex(&hlsSession->lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    /* check if the playlist is a variant playlist and if so, download the suitable simple playlist file contained in the variant playlist */
    /* suitable playlist will be the one with bitrate matching the current n/w bitrate */
    if ((playlistFileInfo = B_PlaybackIp_ParsePlaylistVariantFile(playback_ip, &parsingSuccess)) == NULL) {
        /* variant playlist case as playlistFileInfo is NULL (i.e. initial playlist is not a simple playlist but a variant one */
        if (parsingSuccess == false) {
            BDBG_ERR(("%s: Failed to parse variant playlist file", __FUNCTION__));
            goto error;
        }
        /* variant playlist is downloaded & parsed, now download a playlist w/ b/w closest to current n/w b/w */
        if ((playlistFileInfo = B_PlaybackIp_HlsSelectDownloadAndParsePlaylist(playback_ip, hlsSession)) == NULL) {
            BDBG_ERR(("%s: Failed to download & parse playlist file entry", __FUNCTION__));
            goto error;
        }
#ifdef HLS_UNIT_TEST
        /* print simple playlist */
        dump_playlist_file(hlsSession);
#endif
    }
    else {
        /* top level uri is not a variant playlist but a simple playlist, since we have already downloaded it, now lets parse it */
        if (B_PlaybackIp_ParsePlaylistFile(playback_ip, playlistFileInfo) != true) {
            BDBG_ERR(("%s: failed to parse the M3U playlist file", __FUNCTION__));
            goto error;
        }
    }
#ifdef HLS_UNIT_TEST
    printVariantPlaylistFile(hlsSession);
#endif
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: finished parsing the M3U playlist file, now setup for media probing", __FUNCTION__));
#endif

    /* at this time,  playlists (both variant & simple ones) are downloaded & parsed, so lets download the 1st segment and find out its media PSI info */

    /* for the chosen playlist, take the 1st media segment URL and connect to that server so that we can probe its media details */
    mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
    while (mediaFileSegmentInfo) {
        if (B_PlaybackIp_HlsSetupHttpSessionToServer(playback_ip, mediaFileSegmentInfo, &playback_ip->socketState.fd, playback_ip->temp_buf, &playback_ip->initial_data_len) == false) {
            BDBG_ERR(("%s: ERROR: Socket setup or HTTP request/response failed for downloading Media Segment, retrying next media segment", __FUNCTION__));
            /* remove this media segment as we met some error in downloading this segment (most likely segment URL has expired) */
            BLST_Q_REMOVE(&playlistFileInfo->mediaFileSegmentInfoQueueHead, mediaFileSegmentInfo, next);
            B_PlaybackIp_FreeMediaFileSegmentInfo(mediaFileSegmentInfo);
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

    if (BKNI_CreateEvent(&hlsSession->bufferEmptiedEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->bufferFilledEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->reDownloadPlaylistEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->segDownloadThreadPausedEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->playbackThreadPausedEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->segDownloadThreadPauseDoneEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    if (BKNI_CreateEvent(&hlsSession->playbackThreadPauseDoneEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

    playback_ip->hlsSessionEnabled = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: Complete, now media probing will start.", __FUNCTION__));
#endif

    /* the actual media probe operation happens in the caller function of http module */
    return 0;

error:
    B_PlaybackIp_HlsSessionDestroy(playback_ip);
    return -1;
}

int 
B_PlaybackIp_HlsSegmentBufferSetup(
    B_PlaybackIpHandle playback_ip
    )
{
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;

    BDBG_MSG(("%s: using index & data cache for downloading the consecutive media segments", __FUNCTION__));
    /* use index & data cache for downloading the consecutive media segments */
    /* coverity[missing_lock] */
    hlsSession->segmentBuffer[0].filled = false;
    hlsSession->segmentBuffer[0].buffer = playback_ip->indexCache;
    hlsSession->segmentBuffer[0].bufferSize = playback_ip->indexCacheSize;
    hlsSession->segmentBuffer[0].bufferDepth = 0;
    if (BKNI_CreateMutex(&hlsSession->segmentBuffer[0].lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    /* coverity[missing_lock] */
    hlsSession->segmentBuffer[1].filled = false;
    hlsSession->segmentBuffer[1].buffer = playback_ip->dataCache[0].cache;
    hlsSession->segmentBuffer[1].bufferSize = playback_ip->dataCache[0].size;
    hlsSession->segmentBuffer[1].bufferDepth = 0;
    if (BKNI_CreateMutex(&hlsSession->segmentBuffer[1].lock) != 0) {
        BDBG_ERR(("%s: Failed to create BKNI mutex at %d", __FUNCTION__, __LINE__));
        goto error;
    }

    return 0;
error:
    return -1;
}

PlaylistFileInfo *
B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile(
    B_PlaybackIpHandle playback_ip,
    HlsSessionState *hlsSession, 
    PlaylistFileInfo *currentPlaylistFileInfo
    )
{
    PlaylistFileInfo *newPlaylistFileInfo = NULL;
    B_PlaybackIpProtocol protocol;

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
    if ((hls_parse_url(&protocol, &newPlaylistFileInfo->server, &newPlaylistFileInfo->port, &newPlaylistFileInfo->uri, newPlaylistFileInfo->absoluteUri) == false) || (protocol != B_PlaybackIpProtocol_eHttp && protocol != B_PlaybackIpProtocol_eHttps) ) {
        BDBG_ERR(("Failed to parse URI at %s:%d", __FUNCTION__, __LINE__));
        goto error;
    }
    newPlaylistFileInfo->bandwidth = currentPlaylistFileInfo->bandwidth;
    newPlaylistFileInfo->programId = currentPlaylistFileInfo->programId;

    /* reload the playlist */
    if (B_PlaybackIp_HlsConnectDownloadAndParsePlaylistFile(playback_ip, hlsSession, newPlaylistFileInfo) < 0) {
        BDBG_ERR(("%s: Failed to download & parse playlist file entry w/ uri %s", __FUNCTION__, newPlaylistFileInfo->uri));
        goto error;
    }
#ifdef HLS_UNIT_TEST
    /* verify the playlist file */
    printPlaylistFile(newPlaylistFileInfo);
#endif
    return newPlaylistFileInfo;

error:
    if (newPlaylistFileInfo)
        BKNI_Free(newPlaylistFileInfo);
    return NULL;
}

void 
B_PlaybackIp_HlsReplacePlaylists(
    HlsSessionState *hlsSession, 
    PlaylistFileInfo *playlistFileInfo,
    PlaylistFileInfo *newPlaylistFileInfo
    )
{
    B_PlaybackIp_HlsRemovePlaylistInfoEntry(hlsSession, playlistFileInfo);
    B_PlaybackIp_HlsInsertPlaylistInfoEntry(hlsSession, newPlaylistFileInfo);
    B_PlaybackIp_FreePlaylistInfo(playlistFileInfo);
#ifdef HLS_UNIT_TEST
    printVariantPlaylistFile(hlsSession);
#endif
}

MediaFileSegmentInfo *
B_PlaybackIp_HlsGetNextMediaSegmentEntry(
    B_PlaybackIpHandle playback_ip,
    HlsSessionState *hlsSession, 
    int currentMediaSegmentSequenceNumber,
    unsigned networkBandwidth
    )
{
    bool useDifferentPlaylist = false;
    PlaylistFileInfo *playlistFileInfo;
    PlaylistFileInfo *prevPlaylistFileInfo = NULL;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
#if 0
#define HLS_UNIT_TEST1
#endif
#ifdef HLS_UNIT_TEST1
    static int lastPlaylistIdx = 0; static int i=0;
#endif
    if (hlsSession->resetPlaylist) {
        /* resetting a playlist, start from the 1st media segment */
        hlsSession->downloadedAllSegmentsInCurrentPlaylist = false;
        hlsSession->resetPlaylist = false;
        mediaFileSegmentInfo = BLST_Q_FIRST(&hlsSession->currentPlaylistFile->mediaFileSegmentInfoQueueHead);
        BDBG_MSG(("%s: Starting w/ 1st media segment: sequence # %d, current network bandwidth %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence, networkBandwidth));
        hlsSession->currentPlaylistFile->currentMediaFileSegment = mediaFileSegmentInfo;
        return mediaFileSegmentInfo;
    }
    
    /* find a playlist entry whose n/w b/w matches the current n/w b/w */
    for (prevPlaylistFileInfo = NULL, playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
         playlistFileInfo;
         prevPlaylistFileInfo=playlistFileInfo, playlistFileInfo = BLST_Q_NEXT(playlistFileInfo, next)) 
    {
#ifdef HLS_UNIT_TEST1
        {
            if (playlistFileInfo->bandwidth <= 64000)
                continue;
            if (i==lastPlaylistIdx) {
                lastPlaylistIdx = i+1;
                BDBG_WRN(("%s: UNIT TEST: using playlist w/ b/w %u > max network b/w %u", __FUNCTION__, playlistFileInfo->bandwidth, hlsSession->maxNetworkBandwidth));
                i = 0;
                break;
            }
            i++;
            continue;
        }
#endif
        BDBG_MSG(("%s: playlist file: uri %s, b/w %d, network Bandwidth %d", __FUNCTION__, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth));
        if (playlistFileInfo->bandwidth > hlsSession->maxNetworkBandwidth) {
            BDBG_MSG(("%s: using playlist w/ b/w %u > max network b/w %u", __FUNCTION__, playlistFileInfo->bandwidth, hlsSession->maxNetworkBandwidth));
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
#ifdef HLS_UNIT_TEST1
    else if (playlistFileInfo == NULL) {
        playlistFileInfo = BLST_Q_FIRST(&hlsSession->playlistFileInfoQueueHead);
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
    if (playlistFileInfo != hlsSession->currentPlaylistFile) {
        PlaylistFileInfo *newPlaylistFileInfo;
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("%s: switch playlist due to n/w b/w change, playlist file entry %p, uri %s, b/w %d, network b/w %d, bounded %d", 
                    __FUNCTION__, playlistFileInfo, playlistFileInfo->uri, playlistFileInfo->bandwidth, networkBandwidth, playlistFileInfo->bounded));
#endif
        if ((newPlaylistFileInfo = B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile(playback_ip, hlsSession, playlistFileInfo)) == NULL) {
            BDBG_ERR(("%s: B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile() failed", __FUNCTION__));
            goto error;
        }
        B_PlaybackIp_HlsReplacePlaylists(hlsSession, playlistFileInfo, newPlaylistFileInfo);
        playlistFileInfo = newPlaylistFileInfo;
        hlsSession->currentPlaylistFile = playlistFileInfo;
        useDifferentPlaylist = true;
    }
    if (useDifferentPlaylist == true || hlsSession->currentPlaylistFile->playlistHasChanged == true) {
        BDBG_MSG(("%s: find next media segment entry w/ seq # > %d due to %s", 
                    __FUNCTION__, currentMediaSegmentSequenceNumber,
                    (hlsSession->currentPlaylistFile->playlistHasChanged? "Playlist update":"n/w b/w change")));
        hlsSession->currentPlaylistFile->playlistHasChanged = false;
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

    return mediaFileSegmentInfo;

error:
    return NULL;
}

B_PlaybackIpError 
B_PlaybackIp_PauseHls(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    BSTD_UNUSED(ipTrickModeSettings);
    if (!playback_ip->hlsSessionState) {
        BDBG_ERR(("%s: ERROR: HlsSessionState is NULL", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }

    if (B_PlaybackIp_HlsBoundedStream(playback_ip) == false) {
        BDBG_ERR(("%s: ERROR: pause trickplay not supported for unbounded HLS streams", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    /* in HLS pause, we dont disconnect the server connection and instead keep that going */
    /* We instead pause by setting STC rate to 0, this way download and playback threads still continue until all buffers get full */
    if (NEXUS_StcChannel_SetRate(playback_ip->nexusHandles.stcChannel, 0, 0 ) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to pause by setting stc rate to 0", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_PlayHls(
    B_PlaybackIpHandle playback_ip
    ) 
{
    if (B_PlaybackIp_HlsBoundedStream(playback_ip) == false) {
        BDBG_ERR(("%s: ERROR: resume trickplay not supported for unbounded HLS streams", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    /* just resume the STC rate and we should be good */
    if (NEXUS_StcChannel_SetRate(playback_ip->nexusHandles.stcChannel, 1, 0 ) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to un-pause by setting stc rate to 0", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_SeekHls(
    B_PlaybackIpHandle playback_ip,
    NEXUS_PlaybackPosition seekPosition
    )
{
    PlaylistFileInfo *playlistFileInfo = playback_ip->hlsSessionState->currentPlaylistFile;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
    NEXUS_PlaybackPosition currentDuration = 0;
    B_PlaybackIpState currentState;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    BERR_Code rc;
    int i;

    BDBG_MSG(("%s: seekPosition = %d", __FUNCTION__, seekPosition));

    if (seekPosition >= hlsSession->currentPlaylistFile->totalDuration) {
        BDBG_ERR(("%s: Incorrect seekPosition %d, total media duration %d", __FUNCTION__, seekPosition, hlsSession->currentPlaylistFile->totalDuration));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* save the current state */
    currentState = playback_ip->playback_state;
    /* now synchronize with the media segment download and playpump feeder threads */
    /* by changing the playback_state to WaitingToEnterTrickMode, these threads will pause their work, */
    /* signal this back to this thread and then wait for seek work to finish. Then, this thread should */
    /* signal these worker threads to resume downloading media segments and feeding to playpump. */
    BKNI_ResetEvent(hlsSession->segDownloadThreadPausedEvent);
    BKNI_ResetEvent(hlsSession->playbackThreadPausedEvent);
    hlsSession->seekOperationStarted = true;
    playback_ip->playback_state = B_PlaybackIpState_eWaitingToEnterTrickMode;
    rc = BKNI_WaitForEvent(hlsSession->segDownloadThreadPausedEvent, 2*HLS_EVENT_TIMEOUT_MSEC);
    if (rc == BERR_TIMEOUT) {
        BDBG_ERR(("%s: EVENT timeout: failed to receive event from HLS Segment download thread indicating its paused", __FUNCTION__));
        return B_ERROR_TIMEOUT;
    } else if (rc!=0) {
        BDBG_ERR(("%s: failed to wait for event from HLS Segment download thread indicating its paused, rc = %d", __FUNCTION__, rc));
        return B_ERROR_UNKNOWN;
    }
    BDBG_WRN(("%s: segment download thread is paused", __FUNCTION__));
    /* now pause the playpump feeder thread */
    rc = BKNI_WaitForEvent(hlsSession->playbackThreadPausedEvent, 2*HLS_EVENT_TIMEOUT_MSEC);
    if (rc == BERR_TIMEOUT) {
        BDBG_ERR(("%s: EVENT timeout: failed to receive event from HLS Playpump feeder thread indicating its paused", __FUNCTION__));
        return B_ERROR_TIMEOUT;
    } else if (rc!=0) {
        BDBG_ERR(("%s: failed to wait for event from HLS Playpump feeder thread indicating its paused, rc = %d", __FUNCTION__, rc));
        return B_ERROR_UNKNOWN;
    }
    BDBG_WRN(("%s: now both segment download and playpump feeder threads are paused, so do the seek related work", __FUNCTION__));

    /* determine which mediaSegment lines up with the new seekPosition for the currentPlaylist being played out */
    /* TODO: improve this: instead of starting from start of list, figure a good location to start from */
    /* it can be 0) nearby the current position, 1) near front, 2) near end */
    for (mediaFileSegmentInfo = BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead);
            mediaFileSegmentInfo;
            mediaFileSegmentInfo = BLST_Q_NEXT(mediaFileSegmentInfo, next))
    {
        currentDuration += mediaFileSegmentInfo->duration;
        if (currentDuration > seekPosition) {
            BDBG_MSG(("%s: Found next media segment (# %d) with correct seekPosition %d", __FUNCTION__, mediaFileSegmentInfo->mediaSequence, seekPosition));
            break;
        }
    }
    if (!mediaFileSegmentInfo) {
        BDBG_ERR(("%s: Failed to find a segment matching with seekPosition %d, currentDuration %d", __FUNCTION__, seekPosition, currentDuration));
        return B_ERROR_UNKNOWN;
    }
    currentDuration -= mediaFileSegmentInfo->duration; /* also take out the duration of this next segment */
    /* we set the currentMediaFileSegment to 1 entry before the one we want to seek to. */
    /* this is because download thread will start from the next segment of the current one */
    if (mediaFileSegmentInfo != BLST_Q_FIRST(&playlistFileInfo->mediaFileSegmentInfoQueueHead))
        playlistFileInfo->currentMediaFileSegment = BLST_Q_PREV(mediaFileSegmentInfo, next);
    else {
        /* we are pointing to the 1st segment, so just reset the playlist to start from here */
        playback_ip->hlsSessionState->resetPlaylist = true;
        playlistFileInfo->currentMediaFileSegment =  mediaFileSegmentInfo;
    }
    hlsSession->downloadedAllSegmentsInCurrentPlaylist = false;

    BDBG_MSG(("%s: jumping to URI %s at position %u, seeked position %u", __FUNCTION__, mediaFileSegmentInfo->uri, currentDuration, seekPosition));

    /* flush the current pipeline so we resume from the new seek location */
    if (B_PlaybackIp_UtilsFlushAvPipeline(playback_ip)) {
        BDBG_ERR(("%s: ERROR: Failed to flush the AV pipeline\n", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }

    for (i=0; i<HLS_NUM_SEGMENT_BUFFERS; i++) {
        BKNI_AcquireMutex(playback_ip->hlsSessionState->segmentBuffer[i].lock);
        playback_ip->hlsSessionState->segmentBuffer[i].filled = false;
        playback_ip->hlsSessionState->segmentBuffer[i].bufferDepth = 0;
        BKNI_ReleaseMutex(playback_ip->hlsSessionState->segmentBuffer[i].lock);
    }

    playback_ip->lastSeekPosition = currentDuration;
    playback_ip->playback_state = currentState;
    hlsSession->seekOperationStarted = false;
    BKNI_SetEvent(hlsSession->segDownloadThreadPauseDoneEvent);
    BKNI_SetEvent(hlsSession->playbackThreadPauseDoneEvent);
    BDBG_WRN(("%s: B_PlaybackIp_SeekHls successful to %d msec", __FUNCTION__, currentDuration));
    return B_ERROR_SUCCESS;
}

void
B_PlaybackIp_HlsMediaSegmentDownloadThread(
    void *data
    )
{
    int i;
    BERR_Code rc;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
    HlsSegmentBuffer *segmentBuffer = NULL;
    unsigned networkBandwidth = 0;
    MediaFileSegmentInfo *mediaFileSegmentInfo = NULL;
    int currentMediaSegmentSequenceNumber = -1;
    bool previousMediaSegmentCompletelyDownloaded = true;
    bool serverClosed;
    bool gotoTopOfLoop = false;

    BDBG_MSG(("%s: Started", __FUNCTION__));
    /* start from the 1st segment in the current playlist */
    hlsSession->resetPlaylist = true;
    while (true) {
        gotoTopOfLoop = false;
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            BDBG_MSG(("%s: breaking out of HLS Media Segment Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (hlsSession->hlsPlaybackThreadDone) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: HLS Playback thread is done, so stopping the HLS Segment Download thread", __FUNCTION__));
#endif
            goto error;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode && hlsSession->seekOperationStarted) {
            BDBG_WRN(("%s: App is trying for a trickmode command, pause this thread", __FUNCTION__));
            BKNI_ResetEvent(hlsSession->segDownloadThreadPauseDoneEvent);
            /* signal the control thread that we have paused and thus it can continue w/ setting up the trickmode operation */
            BKNI_SetEvent(hlsSession->segDownloadThreadPausedEvent);

            /* wait on signal from the control thread to indicate trickmode work is done */
            rc = BKNI_WaitForEvent(hlsSession->segDownloadThreadPauseDoneEvent, HLS_EVENT_TIMEOUT_MSEC*2);
            if (rc == BERR_TIMEOUT || rc!=0) {
                BDBG_ERR(("%s: EVENT %s: failed to receive event from control thread indicating trickmode completion", __FUNCTION__, rc==BERR_TIMEOUT?"Timeout":"Error"));
                goto error;
            }
            /* set this flag so that we dont download anything remaining from the previous segment */
            previousMediaSegmentCompletelyDownloaded = true;
            /* since seek operation will change the next sequence # to download, update the current media sequence # to it */
            currentMediaSegmentSequenceNumber = hlsSession->currentPlaylistFile->currentMediaFileSegment->mediaSequence;
            if (!serverClosed) {
                /* close security context and socket if they were closed because we didn't yet get close event from the server */
                if (playback_ip->securityHandle) {
                    playback_ip->netIo.close(playback_ip->securityHandle);
                    playback_ip->securityHandle = NULL;
                }
                close(playback_ip->socketState.fd);
            }
            BDBG_WRN(("%s: resuming seg download thread after trickmode command", __FUNCTION__));
        }

        /* determine the next buffer to use for downloading next media segment */
        segmentBuffer = NULL;
        while (!segmentBuffer) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of HLS Download Media Segment Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
                gotoTopOfLoop = true;
                break;
            }
            for (i=0; !segmentBuffer && i<HLS_NUM_SEGMENT_BUFFERS; i++) {
                BKNI_AcquireMutex(hlsSession->segmentBuffer[i].lock);
                if (!hlsSession->segmentBuffer[i].filled) {
                    segmentBuffer = &hlsSession->segmentBuffer[i];
                    segmentBuffer->bufferDepth = 0;
                }
                BKNI_ReleaseMutex(hlsSession->segmentBuffer[i].lock);
            }

            if (!segmentBuffer) {
                if (hlsSession->hlsPlaybackThreadDone) {
                    BDBG_ERR(("%s: HLS Playback thread is done, so stopping the HLS Download thread", __FUNCTION__));
                    goto error;
                }
                /* wait on signal from playback thread to consume and free up one of the buffers */
                rc = BKNI_WaitForEvent(hlsSession->bufferEmptiedEvent, HLS_EVENT_TIMEOUT_MSEC);
                if (rc == BERR_TIMEOUT) {
                    BDBG_MSG(("%s: EVENT timeout: failed to receive event from HLS Playback thread indicating buffer availability, continue waiting", __FUNCTION__));
                    continue;
                } else if (rc!=0) {
                    BDBG_ERR(("%s: failed to wait for event indicating buffer consumption from HLS Playback thread, rc = %d", __FUNCTION__, rc));
                    goto error;
                }
            }
        }
        if (gotoTopOfLoop)
            continue;

        if (previousMediaSegmentCompletelyDownloaded == true) {
            /* now download the next segment */
            BKNI_AcquireMutex(hlsSession->lock);
            /* now we have a buffer, pick the next media segment to download, it may be from a different playlist file if our n/w b/w has changed */
            if ((mediaFileSegmentInfo = B_PlaybackIp_HlsGetNextMediaSegmentEntry(playback_ip, hlsSession, hlsSession->resetPlaylist ? -1:currentMediaSegmentSequenceNumber, networkBandwidth)) == NULL) {
                /* no more segments to download, we keep looping in this thread until playback thread is done feeding and playing all data */
                /* this is done so that if the user wants to re-seek to a previously played position, we can still support it */
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: No More Media Segment URI left in the Playlist URI %s, we keep looping !!", __FUNCTION__, hlsSession->currentPlaylistFile->uri));
#endif
                hlsSession->downloadedAllSegmentsInCurrentPlaylist = true;
                BKNI_ReleaseMutex(hlsSession->lock);
                BKNI_Sleep(200); /* allow the playlist redownload thread to refetch the playlist and then retry or playback thread to finish playing */
                continue;
            }
            currentMediaSegmentSequenceNumber = mediaFileSegmentInfo->mediaSequence;
            BDBG_MSG(("%s: next media segment: uri %s, seq# %d, prev fd %d", __FUNCTION__, mediaFileSegmentInfo->absoluteUri, mediaFileSegmentInfo->mediaSequence, playback_ip->socketState.fd));
            if (B_PlaybackIp_HlsSetupHttpSessionToServer(playback_ip, mediaFileSegmentInfo, &playback_ip->socketState.fd, segmentBuffer->buffer, &segmentBuffer->bufferDepth) == false) {
                BDBG_ERR(("%s: ERROR: Socket setup or HTTP request/response failed for downloading next Media Segment, skip to next uri", __FUNCTION__));
                BKNI_ReleaseMutex(hlsSession->lock);
                continue;
            }
            BKNI_ReleaseMutex(hlsSession->lock);
        }
        else {
            /* previous media segment wasn't completely downloaded due to segment buffer being smaller than the content length */
            BDBG_MSG(("%s: previous media segment wasn't completely downloaded due to segment buffer being smaller than the content length", __FUNCTION__));
        }

        BDBG_MSG(("%s: now download the actual media segment, contentLength %lld", __FUNCTION__, playback_ip->contentLength));
        /* now download the actual media segment */
        serverClosed = false;
        if (B_PlaybackIp_HlsDownloadMediaSegment(playback_ip, playback_ip->socketState.fd, segmentBuffer->buffer, segmentBuffer->bufferSize, &segmentBuffer->bufferDepth, &networkBandwidth, &serverClosed) != true) {
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

        /* inform HLS playback thread that buffer is filled w/ the media segment */
        BKNI_AcquireMutex(segmentBuffer->lock);
        segmentBuffer->filled = true;
        if (mediaFileSegmentInfo->markedDiscontinuity) {
            segmentBuffer->markedDiscontinuity = true;
            mediaFileSegmentInfo->markedDiscontinuity = false;
        }
        else
            segmentBuffer->markedDiscontinuity = false;
        BKNI_ReleaseMutex(segmentBuffer->lock);
        BKNI_SetEvent(hlsSession->bufferFilledEvent);
        BDBG_MSG(("%s: informed HLS playback thread that buffer %p: depth %d is filled , current network bandwidth %d", __FUNCTION__, segmentBuffer, segmentBuffer->bufferDepth, networkBandwidth));
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error: 
    hlsSession->hlsSegmentDownloadThreadDone = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: HLS Media Segment Download thread is exiting...", __FUNCTION__));
#endif
    return;
}

void
B_PlaybackIp_HlsPrintAvStats(B_PlaybackIpHandle playback_ip)
{
    NEXUS_PlaypumpStatus ppStatus;
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;
    NEXUS_Error rc;
    rc = NEXUS_Playpump_GetStatus(playback_ip->nexusHandles.playpump, &ppStatus);
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return;}
    BDBG_WRN(("%s: before playpump fifo size %d, depth %d, bytesPlayed %d", __FUNCTION__, ppStatus.fifoSize, ppStatus.fifoDepth, ppStatus.bytesPlayed));
    if (playback_ip->nexusHandles.videoDecoder) {
        rc = NEXUS_VideoDecoder_GetStatus(playback_ip->nexusHandles.videoDecoder, &videoStatus);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning...\n", rc, __LINE__)); return;}
        BDBG_WRN(("before decode %.4dx%.4d, pts %#x, fifo size %d, depth %d, fullness %d%%\n",
                    videoStatus.source.width, videoStatus.source.height, videoStatus.pts, videoStatus.fifoSize, videoStatus.fifoDepth, videoStatus.fifoSize?(videoStatus.fifoDepth*100)/videoStatus.fifoSize:0));
    }
    if (playback_ip->nexusHandles.primaryAudioDecoder) {
        rc = NEXUS_AudioDecoder_GetStatus(playback_ip->nexusHandles.primaryAudioDecoder, &audioStatus);
        BDBG_WRN(("before audio0            pts %#x, fifo size %d, depth %d, fullness %d%%\n",
                    audioStatus.pts, audioStatus.fifoSize, audioStatus.fifoDepth, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0));
    }
}

void 
B_PlaybackIp_HlsPlaylistReDownloadThread(
    void *data
    )
{
    int rc;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    HlsSessionState *hlsSession = playback_ip->hlsSessionState;
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
            BDBG_MSG(("%s: breaking out of HLS Playlist Download loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
            BDBG_WRN(("%s: TODO: yet to add trick mode code"));
            BDBG_ASSERT(NULL);
            break;
        }
        rc = BKNI_WaitForEvent(hlsSession->reDownloadPlaylistEvent, reloadTimer*1000);
        if (rc!=BERR_SUCCESS && rc != BERR_TIMEOUT) {
            BDBG_ERR(("%s: failed to wait for event indicating playlist reload timer, rc = %d", __FUNCTION__, rc));
            goto error;
        }
        if (hlsSession->hlsPlaybackThreadDone) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: HLS Playback thread is done, so stopping the HLS Playlist ReDownload thread", __FUNCTION__));
#endif
            goto error;
        }
        if (playback_ip->mediaStartTimeNoted == false) {
            BDBG_ERR(("%s: media playback hasn't started for this long, so breaking out", __FUNCTION__));
            break;
        }
        BDBG_MSG(("%s: Redownload playlist as %s, rc %d", __FUNCTION__, rc != BERR_TIMEOUT? "we've downloaded all segments in current playlist":"reload timer expired", rc));

        BKNI_AcquireMutex(hlsSession->lock);
        playlistFileInfo = hlsSession->currentPlaylistFile; /* Segment Download Thread can change the playlist due to n/w b/w change */
        if ((newPlaylistFileInfo = B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile(playback_ip, hlsSession, playlistFileInfo)) == NULL) {
            int count = 0;
            if (++count > 30) {
                BKNI_ReleaseMutex(hlsSession->lock);
                BDBG_ERR(("%s: B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile() failed over %d attempts", __FUNCTION__, count));
                goto error;
            }
            else {
                reloadTimer = 2;    /* retry playlist download after 2 sec */
                BKNI_ReleaseMutex(hlsSession->lock);
                BDBG_ERR(("%s: B_PlaybackIp_HlsAllocateDownloadAndParsePlaylistFile() failed, retry, # of attempts %d ", __FUNCTION__, count));
                continue;
            }
        }
        if (newPlaylistFileInfo->mediaSegmentBaseSequence != -1) {
            /* seq # of the 1st segment in this playlist is specified, makes it quite easy to determine whether server has updated the playlist or not */
            if (newPlaylistFileInfo->mediaSegmentBaseSequence == playlistFileInfo->mediaSegmentBaseSequence && newPlaylistFileInfo->numMediaSegments == playlistFileInfo->numMediaSegments) {
                /* new playlist file hasn't changed yet, restart the timer and retry it */
                B_PlaybackIp_FreePlaylistInfo(newPlaylistFileInfo);
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
#define HLS_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES 9000
                if (playlistHasNotChangedCount > HLS_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES) {
                    BDBG_ERR(("%s: new playlist has not changed for %d sec, %d times, stopping the PlaylistReDownload thread", __FUNCTION__, HLS_PLAYLIST_RE_DOWNLOAD_MAX_RETRIES*reloadTimer/2, playlistHasNotChangedCount));
                    BKNI_ReleaseMutex(hlsSession->lock);
                    break;
                }
#endif
                BDBG_MSG(("%s: new playlist has not changed yet, restart the timer by %d sec and retry it, initial timer %d", __FUNCTION__, reloadTimer, playlistFileInfo->initialMinimumReloadDelay));
            }
            else if (newPlaylistFileInfo->mediaSegmentBaseSequence < playlistFileInfo->mediaSegmentBaseSequence && !hlsSession->downloadedAllSegmentsInCurrentPlaylist) {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: even though playlist base seq (new %d, current %d) has wrapped around: ignore it for now we haven't yet downloaed all segments in the current playlist", __FUNCTION__, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo->mediaSegmentBaseSequence));
#endif
                B_PlaybackIp_FreePlaylistInfo(newPlaylistFileInfo);
                reloadTimer = 1; /* retry the playlist every second so that we have the playlist ready to go once media downthread finishes segments from previous thread */
            }
            else {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: playlist has changed (base seq: cur %d, new %d), remove %p playlist and insert %p playlist", __FUNCTION__, 
                                playlistFileInfo->mediaSegmentBaseSequence, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo, newPlaylistFileInfo));
#endif
                if (hlsSession->downloadedAllSegmentsInCurrentPlaylist && newPlaylistFileInfo->mediaSegmentBaseSequence < playlistFileInfo->mediaSegmentBaseSequence) {
                    /* we have downloaded all segments in the current playlist and new playlist has wrapped around (new base seq < cur base seq) */
                    /* set a flag to indicate starting from the 1st segment of this playlist */
                    BDBG_MSG(("%s: playlist base seq has wrapped around: & we've downloaed all segments in the current playlist, so resetting to use new playlist from starting seq # %d, old playlist seq# %d", __FUNCTION__, newPlaylistFileInfo->mediaSegmentBaseSequence, playlistFileInfo->mediaSegmentBaseSequence));
                    hlsSession->resetPlaylist = true;
                }
                B_PlaybackIp_HlsReplacePlaylists(hlsSession, playlistFileInfo, newPlaylistFileInfo);
                hlsSession->currentPlaylistFile = newPlaylistFileInfo;
                hlsSession->currentPlaylistFile->playlistHasChanged = true; 
                playlistFileInfo = newPlaylistFileInfo;
#if 0
                /* reset the count as playlist has now changed */
                playlistHasNotChangedCount = 1;
#endif
                reloadTimer = playlistFileInfo->initialMinimumReloadDelay;
                if (playlistFileInfo->bounded) {
                    BDBG_MSG(("%s: updated playlist has END TAG set, stopping the ReDownloading thread as Live Event has reached towards its end", __FUNCTION__)); 
                    BKNI_ReleaseMutex(hlsSession->lock);
                    break;
                }
            }
        }
        else {
            /* TODO: add this code */
            BDBG_WRN(("%s: TODO: base media sequence is not explicitly specified in the playlist, need to manually compare the uris to determine if the new playlist has changed", __FUNCTION__));
        }
        BKNI_ReleaseMutex(hlsSession->lock);
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error: 
    if (newPlaylistFileInfo)
        B_PlaybackIp_FreePlaylistInfo(newPlaylistFileInfo);
    hlsSession->hlsPlaylistReDownloadThreadDone = true;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: HLS Playlist ReDownload thread is exiting...", __FUNCTION__));
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
B_PlaybackIp_HlsPlaybackThread(
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
    HlsSessionState *hlsSession;
    HlsSegmentBuffer *segmentBuffer;
    NEXUS_PlaypumpSettings nSettings;
    PlaylistFileInfo *playlistFileInfo;
    static int fileNameSuffix = 0;
    char recordFileName[32];
    FILE *fclear = NULL;
    bool gotoTopOfLoop = false;

    playback_ip = (B_PlaybackIpHandle)data;
    hlsSession = playback_ip->hlsSessionState;
    playlistFileInfo = hlsSession->currentPlaylistFile;

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

    /* close the previous security handle and socket used for downloading the 1st media segment during media probe */
    if (playback_ip->securityHandle) {
        playback_ip->netIo.close(playback_ip->securityHandle);
        playback_ip->securityHandle = NULL;
    }
    close(playback_ip->socketState.fd);

    if (B_PlaybackIp_HlsSegmentBufferSetup(playback_ip) < 0) {
        goto error;
    }

    /* start a thread to download the next media file segment into the read buffer */
    B_Thread_GetDefaultSettings(&settingsThread);
    hlsSession->hlsSegmentDownloadThread = B_Thread_Create("HlsMediaSegmentDownloadThread", B_PlaybackIp_HlsMediaSegmentDownloadThread, (void *)playback_ip, &settingsThread);
    if (NULL == hlsSession->hlsSegmentDownloadThread) {
        BDBG_ERR(("%s: Failed to create the %s thread for HLS protocol\n", __FUNCTION__, "HlsMediaSegmentDownloadThread"));
        goto error;
    }
    BDBG_MSG(("%s: Created the %s for HLS protocol", __FUNCTION__, "HlsMediaSegmentDownloadThread"));

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
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/hls_rec%d_0.ts", fileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }

    if (B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(playback_ip))
        goto error;

    if (playlistFileInfo->bounded == false) {
        /* stream is unbounded, meaning it is live stream, we need to start a thread to periodically download the updated playlist */
        hlsSession->hlsPlaylistReDownloadThread = B_Thread_Create("HlsPlaylistReDownloadThread", B_PlaybackIp_HlsPlaylistReDownloadThread, (void *)playback_ip, &settingsThread);
        if (NULL == hlsSession->hlsPlaylistReDownloadThread) {
            BDBG_ERR(("%s: Failed to create the %s thread for HLS protocol\n", __FUNCTION__, "HlsPlaylistReDownloadThread"));
            goto error;
        }
        BDBG_MSG(("%s: Created %s for HLS protocol", __FUNCTION__, "HlsPlaylistReDownloadThread"));

        /* for unbounded streams, HLS spec recommends that client should not start the normal playback */
        /* if playlist total duration is less than 3 target durations. This is to avoid stalls due n/w b/w fluctuations & latency to server */
#if 0
        /* commenting this out as it is causing large initial latency while playing from wowza HLS server. This server only had 3 segments in the */
        /* initial playlist whose total duration happened to be < 3*target duration. Thus, this loop of 20sec seems to be large and wasteful */
        while (playlistFileInfo->totalDuration < 3*playlistFileInfo->maxMediaSegmentDuration) {
            /* TODO: double check this logic whether totalDuration is getting updated at the correct play a/f media seg is downloaded */
            int count = 0;
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of HLS Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
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
            BDBG_MSG(("%s: breaking out of HLS Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        gotoTopOfLoop = false;
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode && hlsSession->seekOperationStarted) {
            BDBG_WRN(("%s: App is trying for a trickmode command, pause this thread", __FUNCTION__));
            BKNI_ResetEvent(hlsSession->playbackThreadPauseDoneEvent);
            /* signal the control thread that we have paused and thus it can continue w/ setting up the trickmode operation */
            BKNI_SetEvent(hlsSession->playbackThreadPausedEvent);
            /* wait on signal from the control thread to indicate trickmode work is done */
            rc = BKNI_WaitForEvent(hlsSession->playbackThreadPauseDoneEvent, HLS_EVENT_TIMEOUT_MSEC*2);
            if (rc == BERR_TIMEOUT || rc!=0) {
                BDBG_ERR(("%s: EVENT %s: failed to receive event from control thread indicating trickmode completion", __FUNCTION__, rc==BERR_TIMEOUT?"Timeout":"Error"));
                goto error;
            }
            BDBG_WRN(("%s: resuming HLS Playback loop after trickmode command", __FUNCTION__));
        }

        /* determine the segment buffer to read data from */
        segmentBuffer = NULL;
        while (!segmentBuffer) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of HLS Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
                gotoTopOfLoop = true;
                break;
            }
            for (i=0; !segmentBuffer && i<HLS_NUM_SEGMENT_BUFFERS; i++) {
                BKNI_AcquireMutex(hlsSession->segmentBuffer[i].lock);
                if (hlsSession->segmentBuffer[i].filled) {
                    segmentBuffer = &hlsSession->segmentBuffer[i];
                    BDBG_MSG(("%s: %p buffer is filled up", __FUNCTION__, segmentBuffer));
                }
                else
                    BDBG_MSG(("%s: still no filled buffer, wait for signal from download thread, i %d", __FUNCTION__, i));
                BKNI_ReleaseMutex(hlsSession->segmentBuffer[i].lock);
            }

            if (!segmentBuffer) {
                if (hlsSession->downloadedAllSegmentsInCurrentPlaylist) {
                    BDBG_MSG(("%s: Network Read Error, wait to play out the stream", __FUNCTION__));
                    if (B_PlaybackIp_HlsBoundedStream(playback_ip) && B_PlaybackIp_UtilsEndOfStream(playback_ip) == true) {
#ifdef BDBG_DEBUG_BUILD
                        if (playback_ip->ipVerboseLog)
                            BDBG_WRN(("%s: No more segments to feed to playback & playback is done, total fed %lld", __FUNCTION__, totalBytesFed));
#endif
                        goto error;
                    }
                    else {
#ifdef BDBG_DEBUG_BUILD
                        if (playback_ip->ipVerboseLog)
                            BDBG_WRN(("%s: Continue waiting to re-reading from socket incase it becomes valid again by a seek or rewind trickplay", __FUNCTION__));
#endif
                        BKNI_Sleep(200);
                        continue;
                    }
                }
                /* wait on signal from HLS download thread to fill up one of the buffers */
                BDBG_MSG(("%s: wait %d msec for a signal from download thread", __FUNCTION__, HLS_EVENT_TIMEOUT_MSEC));
                rc = BKNI_WaitForEvent(hlsSession->bufferFilledEvent, HLS_EVENT_TIMEOUT_MSEC);
                if (rc == BERR_TIMEOUT) {
                    BDBG_MSG(("%s: EVENT timeout: failed to receive event from HLS Download thread indicating buffer availability, continue waiting", __FUNCTION__));
                    continue;
                } else if (rc!=0) {
                    BDBG_ERR(("%s: failed to wait for event indicating buffer availability from HLS Download thread, rc = %d", __FUNCTION__, rc));
                    goto error;
                }
                BDBG_MSG(("%s: got a signal from download thread, rc %d", __FUNCTION__, rc));
            }
        }
        if (gotoTopOfLoop)
            continue;
        BDBG_MSG(("%s: Read from segment buffer %p", __FUNCTION__, segmentBuffer));

        bytesFed = 0;
        BDBG_MSG(("%s: Feed %d bytes to Playpump, discontinuity flag %d", __FUNCTION__, segmentBuffer->bufferDepth, segmentBuffer->markedDiscontinuity));
        
        if (segmentBuffer->markedDiscontinuity) {
            BDBG_ERR(("%s: TODO: this segment has marked discontinuity: need to check what can change & accordingly modify the av pipeline!!", __FUNCTION__));
        }
        while (segmentBuffer->bufferDepth) {
            if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of HLS Playback loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
                goto error;
            }
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
                gotoTopOfLoop = true;
                break;
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
        if (gotoTopOfLoop)
            continue;
        BDBG_MSG(("%s: Finished feeding %d bytes to Playpump", __FUNCTION__, bytesFed));

        /* inform HLS Download thread that buffer is emptied and fed to the playback h/w */
        BKNI_AcquireMutex(segmentBuffer->lock);
        segmentBuffer->filled = false;
        BKNI_ReleaseMutex(segmentBuffer->lock);
        BKNI_SetEvent(hlsSession->bufferEmptiedEvent);
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));

error:
    hlsSession->hlsPlaybackThreadDone = true;
    while (!hlsSession->hlsSegmentDownloadThreadDone) {
        int count = 0;
        if (count++ > 50) {
            BDBG_ERR(("%s: Failed to wait for HLS Media Segment Download thread to finish for over %d attempts", __FUNCTION__, count));
            break;
        }
        BDBG_MSG(("%s: HLS Playback thread ran into some error, wait for Download thread to finish", __FUNCTION__));
        BKNI_Sleep(100);
    }
    if (hlsSession->hlsPlaylistReDownloadThread) {
        while (!hlsSession->hlsPlaylistReDownloadThreadDone) {
        int count = 0;
        BKNI_SetEvent(hlsSession->reDownloadPlaylistEvent);
        if (count++ > 50) {
            BDBG_ERR(("%s: Failed to wait for HLS Playlist ReDownload thread to finish for over %d attempts", __FUNCTION__, count));
            break;
        }
            BDBG_MSG(("%s: HLS Playback thread ran into some error, wait for Playlist ReDownload thread to finish", __FUNCTION__));
            BKNI_Sleep(100);
        }
    }
    if (playback_ip->enableRecording && fclear) {
        fflush(fclear);
        fclose(fclear);
    }
    if (hlsSession->hlsSegmentDownloadThread) {
        B_Thread_Destroy(hlsSession->hlsSegmentDownloadThread);
    }

    if (hlsSession->hlsPlaylistReDownloadThread) {
        B_Thread_Destroy(hlsSession->hlsPlaylistReDownloadThread);
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
        BDBG_WRN(("%s: HLS Playback thread is exiting...", __FUNCTION__));
#endif
    BKNI_SetEvent(playback_ip->playback_halt_event);
}
