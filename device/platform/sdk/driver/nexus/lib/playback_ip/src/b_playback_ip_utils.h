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
* $brcm_Workfile: b_playback_ip_utils.h $
* $brcm_Revision: 24 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description:
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_utils.h $
* 
* 24   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 23   10/16/12 5:20p ssood
* SW7231-858: converted socket connect to non-blocking operation with a
*  timeout
* 
* 22   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 21   7/17/12 4:42p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/1   6/20/12 6:28p gskerl
* SW7346-853: Fixed compile warning... removed comma at end of enum list.
* 
* 20   9/19/11 1:50p ssood
* SW7425-1040: added logic to better handle network congestion
* 
* 19   3/17/11 4:36p ssood
* SW7420-1616: initial check-in for pvr encryption support
* 
* 18   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 17   11/23/10 11:55a ssood
* SW7420-1257: initial check-in for rtp/udp streaming support in
*  ip_streamer for Video Conferencing Demo
* 
* 16   8/5/10 2:47p jjordan
* SW7405-4205: add socket flush to facilitate accurate targetting of TTS
*  buffer fullness at start of decode
* 
* 15   7/23/10 11:10a ssood
* SW7420-502: RVU client: fixed subtle issues w/ playing DTCP encrypted
*  HTTP Chunk encoded content from HMC30 server
* 
* 14   3/16/10 3:09p ssood
* SW7420-559: carry forward the EROUTER related changes for WAN unicast
*  support
* 
* 13   11/17/09 3:48p ssood
* SW7420-454: Enhance IP library to stream files from local disk
* 
* 12   10/22/09 12:55p ssood
* SW7420-340: converted streaming socket to non-blocking to avoid getting
*  blocked when client silently goes away
* 
* 11   9/21/09 12:09p ssood
* SW7405-2559: Adding LPCM playback support
* 
* 10   8/6/09 3:38p ssood
* PR48146: add HTTP redirect support
* 
* 9   6/18/09 10:41a lwhite
* PR55417: Add http parsing result enum
* 
* 8   5/22/09 1:57p lwhite
* PR55410: Add util to wait for playpump
* 
* 7   5/13/09 10:50a ssood
* PR53773: added comments for the HTTP utility function
* 
* 6   5/13/09 10:39a ssood
* PR53773: created utitlity functions for HTTP parsing so that RAD/EA
*  module can use it as well
* 
* 5   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
* 
* 4   10/27/08 11:50a ssood
* PR45431: adding MP4 & ASF playback support over HTTP
* 
* 3   10/13/08 5:31p ssood
* PR47521: code cleanup due to api changes
* 
* 2   8/5/08 1:12p ssood
* PR42739: splitting up UDP & RTP Processing code into separate sub-
*  modules
* 
* 1   6/11/08 6:21p ssood
* PR42739: adding IP Applib files
* 
***************************************************************************/

void B_PlaybackIp_UtilsTuneNetworkStack(int fd);
int B_PlaybackIp_UtilsWaitForSocketData(B_PlaybackIpHandle playback_ip, bool *recvTimeout);
int 
B_PlaybackIp_UtilsWaitForSocketWriteReady(int fd, int timeout /* in usec */);
int B_PlaybackIp_UtilsGetPlaypumpBuffer(B_PlaybackIpHandle playback_ip, unsigned int size);
int B_PlaybackIp_UtilsFlushAvPipeline(B_PlaybackIpHandle playback_ip);
int B_PlaybackIp_UtilsFlushSocket(B_PlaybackIpHandle playback_ip);
B_PlaybackIpError B_PlaybackIp_UtilsReadUdpPayloadChunk(
    B_PlaybackIpHandle playback_ip,
    void *buf,       /* buffer big enough to hold (bufSize * iterations) bytes */
    int bufSize,     /* how much to read per iteration */
    int iterations, /* number of iterations (i.e. recv calls) */
    int *bytesRecv,    /* pointer to list of bytesRecv for each recvfrom call */
    int *totalBytesRecv, /* total bytes received */
    bool *recvTimeout   /* set if timeout occurs while waiting to receive a IP packet: helps detect unmarked discontinuity */
    );
#include "bsettop_types.h"
bstream_mpeg_type B_PlaybackIp_UtilsNexus2Mpegtype(NEXUS_TransportType nexus_value);
NEXUS_VideoCodec B_PlaybackIp_UtilsVideoCodec2Nexus(bvideo_codec settop_value);
NEXUS_AudioCodec B_PlaybackIp_UtilsAudioCodec2Nexus(baudio_format settop_value);
NEXUS_TransportType B_PlaybackIp_UtilsMpegtype2ToNexus(bstream_mpeg_type settop_value);
B_PlaybackIpError B_PlaybackIp_UtilsTcpSocketConnect(volatile B_PlaybackIpState *playbackIpState, char *server, int port, bool nonBlockingSocket, int *psd);
int B_PlaybackIp_UtilsTcpSocketWrite(volatile B_PlaybackIpState *playbackIpState, int sd, char *wbuf, int wbuf_len);
char *B_PlaybackIp_UtilsStristr(char *str, char *subStr);
int B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(B_PlaybackIpHandle playback_ip);

#define HTTP_HDR_TERMINATOR_STRING "\r\n\r\n"
typedef enum B_PlaybackIpHttpHdrParsingResult {
    B_PlaybackIpHttpHdrParsingResult_eSuccess,          /* Header is successfully parsed */
    B_PlaybackIpHttpHdrParsingResult_eIncompleteHdr,    /* Header is not terminated w/ CRNL sequence */
    B_PlaybackIpHttpHdrParsingResult_eIncorrectHdr,     /* Some key header fields such as CR/NL may be missing */
    B_PlaybackIpHttpHdrParsingResult_eReadNextHdr,      /* Need to Do another Read to get the Next HTTP Header Response */
    B_PlaybackIpHttpHdrParsingResult_eStatusRedirect,   /* Received HTTP Redirect */
    B_PlaybackIpHttpHdrParsingResult_eStatusBadRequest, /* Request contains bad syntax or can not be fullfilled by the server */
    B_PlaybackIpHttpHdrParsingResult_eStatusServerError,/* Server failed to fullfill a valid request due to some error */
    B_PlaybackIpHttpHdrParsingResult_eStatusNotSupported /* Currently, this HTTP return status is not supported by HTTP client */
}B_PlaybackIpHttpHdrParsingResult; 

typedef struct B_PlaybackIpHttpMsgFields {
    B_PlaybackIpHttpHdrParsingResult parsingResult; /* Defined above */
    int statusCode;          /* HTTP response status code value, may be set even in ERROR case */
    /* rest of the fields are only set if B_ERROR_SUCCESS is returned */
    off_t contentLength;      /* length of the content being played via HTTP */
    bool chunkEncoding;       /* if true, server is sending content using HTTP Chunk Transfer Encoding */
    char *httpHdr;            /* pointer to the HTTP hdr if present */
    char *httpPayload;        /* pointer to the HTTP payload if present */
    int httpPayloadLength;    /* HTTP payload length, 0 if not present */
} B_PlaybackIpHttpMsgFields;

/* Function to parse HTTP Response Header. Returns B_ERROR_SUCCESS if header is successfully parsed */
B_PlaybackIpError B_PlaybackIp_UtilsHttpResponseParse(
    char *rbuf,                             /* read buffer pointing to start of HTTP Response Header */
    unsigned int bytesRead,                 /* # of bytes present in the read buffer */
    B_PlaybackIpHttpMsgFields *httpFields   /* output structure containing various fields from successfully parsed header */
    );

/* Function to byte-swap a buffer */
void
B_PlaybackIp_UtilsByteSwap(
    char *buf, 
    int bufSize
    );

/* Function to build a Wav Header */
int
B_PlaybackIp_UtilsBuildWavHeader(
    uint8_t *buf, 
    size_t bufSize, 
    unsigned bitsPerSample, 
    unsigned sampleRate, 
    unsigned numChannels
    );

/* Functions to stream live or pre-recorded content to a network client */
B_PlaybackIpError B_PlaybackIp_UtilsStreamingCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data);
void B_PlaybackIp_UtilsStreamingCtxClose(struct bfile_io_write_net *data);
ssize_t B_PlaybackIp_UtilsStreamingCtxWrite(bfile_io_write_t self, const void *buf, size_t length);
ssize_t B_PlaybackIp_UtilsStreamingCtxWriteAll(bfile_io_write_t self, const void *buf, size_t bufSize);
extern B_PlaybackIpError B_PlaybackIp_UtilsRtpUdpStreamingCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data);
extern void B_PlaybackIp_UtilsRtpUdpStreamingCtxClose(struct bfile_io_write_net *data);
void B_PlaybackIp_UtilsSetRtpPayloadType( NEXUS_TransportType mpegType, int *rtpPayloadType);
B_PlaybackIpError B_PlaybackIp_UtilsPvrDecryptionCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data);
void B_PlaybackIp_UtilsPvrDecryptionCtxClose(struct bfile_io_write_net *data);
B_PlaybackIpError B_PlaybackIp_UtilsPvrDecryptBuffer(struct bfile_io_write_net *data, unsigned char *buf, unsigned char *clearBuf, int bufSize, int *outBufSize);
#ifdef EROUTER_SUPPORT
B_PlaybackIpError B_PlaybackIp_UtilsSetErouterFilter(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings);
void B_PlaybackIp_UtilsUnsetErouterFilter(B_PlaybackIpHandle playback_ip);
#endif
void B_PlaybackIp_UtilsFreeMemory(void *buffer);
void *B_PlaybackIp_UtilsAllocateMemory(int size, NEXUS_HeapHandle heapHandle);
