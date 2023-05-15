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
* $brcm_Workfile: b_playback_ip_dtcp_ip.c $
* $brcm_Revision: 47 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: DTCP-IP module
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_dtcp_ip.c $
* 
* 47   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 46   9/11/12 3:58p leisun
* SWSECURITY-48: DTCP-IP change. Auto-generating test keys, consolidate
*  test/production build, make common DRM default ON
* 
* 45   7/13/12 8:43a ssood
* SW7425-3486: fixed a compilation error
* 
* 44   6/13/12 2:04p ssood
* SW7231-858: supressing debug logs for HLS playback sessions
* 
* 43   6/13/12 7:24a ssood
* SW7231-766: Coverity fixes for HLS player logic
* 
* 42   5/23/12 3:30p ssood
* SW7425-3042: suppressing extra debug logging
* 
* 41   5/22/12 6:15p ssood
* SW7425-2894: Ip Applib changes for supporting DTCP-IP with robustness
*  rules
* 
* 40   11/23/11 2:10p ssood
* SW7408-109: RVU Trickplay support: added FFWD & FRWD
* 
* 39   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 38   6/3/11 5:34p ssood
* SW7231-166: coverity fixes
* 
* 37   5/24/11 11:36a ssood
* SW7405-4392: Coverity fixes
* 
* 36   2/13/11 4:38p root
* SW7422-161:Add extra parameter to fix DTCP build error
* 
* 35   12/17/10 10:01a ssood
* SW7420-1217: handle the case where server sends non 16byte aligned AES
*  encrypted data
* 
* 34   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 33   10/27/10 8:37p ssood
* SW7125-353: coverity fix
* 
* 32   7/26/10 11:26a ssood
* SW7420-502: correctly validate the return code of the DTCP decrypt call
* 
* 31   7/23/10 11:10a ssood
* SW7420-502: RVU client: fixed subtle issues w/ playing DTCP encrypted
*  HTTP Chunk encoded content from HMC30 server
* 
* 30   7/2/10 10:48a ssood
* SW7420-716: fixed couple of boundary cases to enable *overnight*
*  playing DTCP encrypted streams from HMC30 RVU Server
* 
* 29   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 28   5/20/10 12:31p ssood
* SW7420-561: added compiler option for capturing the clear data a/f dtcp
*  decryption
* 
* 27   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/2   3/1/10 7:55a ssood
* SW7420-561: added support to dynamically adjust for DTCP PCP header +
*  plus fixed a bug causing periodica macroblocking when HTTP Chunk Xfer
*  Encoding was used w/ DTCP/IP Decryption
* 
* SW7420-561/1   2/16/10 10:10a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 26   12/14/09 3:24p ssood
* SW7420-454: fix coverity reported issues
* 
* 26   12/14/09 3:16p ssood
* SW7420-454: fix coverity reported issues
* 
* 25   12/14/09 12:40p ssood
* SW7420-502: fix compiler warnings
* 
* 24   12/12/09 6:34p ssood
* SW7420-502: only process current chunk header end & next chunk header
*  begining when >2 bytes have been read
* 
* 23   12/10/09 7:02p ssood
* SW7420-502: support receiving dtcp/ip encrypted content via the HTTP
*  chunk encoding
* 
* 22   11/17/09 4:04p ssood
* SW7420-340: added support to clone a dtcp/ip session (needed to
*  multiple connects from a client)
* 
* 21   10/6/09 4:58p ssood
* SW7420-340: added support for streaming DTCP/IP encrypted stream to > 1
*  clients + fixed compile errors for clear channels
* 
* 20   8/11/09 7:04p lwhite
* PR55693: Fixed software decryption
* 
* 19   8/11/09 1:13p lwhite
* PR55693: Consolidate residual cypher data in security context
* 
* 18   8/7/09 5:54p lwhite
* PR55693: PCP 14 byte fix
* 
* 17   8/6/09 2:49p lwhite
* PR55693: Fixed wrap logic for 16 byte PCP
* 
* 16   7/21/09 1:53p leisun
* PR 55693: fix closing stream
* 
* 15   7/17/09 4:48p leisun
* PR 55693: close stream in DtcpIpSessionClose()
* 
* 14   7/16/09 7:05p lwhite
* PR55693: Fixed signedness
* 
* 13   7/13/09 11:26a leisun
* PR 55693: Re-design interfaces, and other cleanups
* 
* 12   7/1/09 6:30p ssood
* PR53773: handle case where DTCP/IP library init may fail
* 
* 11   6/25/09 4:35p leisun
* PR 55693: Fix memory leak for DTCP
* 
* 10   6/25/09 3:23p ssood
* PR53773: Allow app to specify Nexus M2M handle if available
* 
* 9   6/25/09 9:55a ssood
* PR53773: turning off DTCP debug logs
* 
* 8   6/11/09 11:21a leisun
* PR 55907: Add HW M2M feature for DTCP-IP
* 
* 7   6/11/09 11:14a leisun
* PR 55907: Add HW M2M feature for DTCP-IP
* 
* 6   6/10/09 8:35p lwhite
* PR53773: Add RadEa time/byte offset
* 
* 5   6/9/09 3:52p leisun
* PR 55693: checking in DTCP-IP lib
* 
* 4   4/21/09 3:32p ssood
* PR53773: handle cases where some security protocols dont encrypt HTTP
*  response header
* 
* 3   4/10/09 3:51p lwhite
* PR53773: Added close to security struc
* 
* 2   4/3/09 6:39p ssood
* PR53773 : further changes to support security framework for SSL, DTCP-
*  IP, RAD/ES protocols
* 
* 1   4/2/09 11:36p ssood
* PR53773 : changes to support security framework for SSL, DTCP-IP,
*  RAD/ES protocols
* 
***************************************************************************/
#if defined(LINUX) || defined(__vxworks)

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_constants.h"

#if 0
#define RECORD_CLEAR_DATA
#endif
#ifdef RECORD_CLEAR_DATA
#include <stdio.h>
static FILE * fclear = NULL;
static FILE * fenc = NULL;
#endif

BDBG_MODULE(b_playback_ip_dtcp_ip);
#define DTCP_IP_ENCRYPT_BUF_SIZE (128*1024)
typedef struct B_PlaybackIpDtcpIpCtx
{
    void *akeHandle;
    void *streamHandle;
    B_PlaybackIpSessionOpenSettings openSettings;
    char initialPayload[TMP_BUF_SIZE];  /* buffer to hold any payload data that got read part of the initial HTTP header processing */
    int initialPayloadLength;
    char *encryptBuf;
    int encryptBufSize;
    /* The following members are used within _http_dtcp_ip_socket_read */
    /* They are used to store data during read and decryption for 16 byte aligment */
    char extended_decrypted_bytes[HTTP_AES_BLOCK_SIZE];  /* stores extended decrypted bytes (in clear) as caller asked for less than 16 bytes of data */
    int extended_len;       /* extended bytes read and decrypted */ 
    char residual_encrypted_bytes[HTTP_AES_BLOCK_SIZE];  /* stores left over bytes from decryption, that did not get decrypted*/
    int residual_encrypted_len;
    char partial_read_bytes[HTTP_AES_BLOCK_SIZE];  /* stores partial read bytes, non mod 16 aligned bytes */
    int partial_read_len;
} B_PlaybackIpDtcpIpCtx;

void B_PlaybackIp_DtcpIpSessionClose(void *voidHandle);
/* 
 * This function tries to read the requested amount from the socket and returns any errors or bytes read.
 * It returns: 
 *  =-1: for errors other than EINTR & EAGAIN during read call or when channel change occurs
 *  = 0: for EOF where server closed the TCP connection
 *  > 0: for success indicating number of bytes read from the socket
 */
int 
_http_dtcp_ip_socket_read(void *voidHandle, B_PlaybackIpHandle playback_ip, int sd, char *rbuf, int rbuf_len)
{
    static int firstTime = 1;
    int rc = 0;
    int bytesRead = 0;
    B_PlaybackIpDtcpIpCtx *securityCtx = (B_PlaybackIpDtcpIpCtx *)voidHandle;
    char *read_ptr;
    int read_len = rbuf_len;
    int bytesToRead = 0;
    int bytesToCopy;
    unsigned int truncated_len = 0;
    char *orig_rbuf = NULL;
    char temp_bytes[HTTP_AES_BLOCK_SIZE];  /* Used for reading upto 16 bytes */
    B_PlaybackIpState *playbackIpState = &playback_ip->playback_state;
    bool dtcp_pcp_header_found = 0;

#ifdef RECORD_CLEAR_DATA
    if(fclear == NULL)
        fclear = fopen("/data/videos/output.mpg", "wb+");
#endif
    /*
     * if using hardware decryption, single buffer is used for reading data from socket and 
     * for decryption. For sw decryption, two separate buffers (one for socket read and one for decryption ) are used.
     */
#ifdef B_DTCP_IP_HW_DECRYPTION
    BDBG_MSG(("%s: Using HW decryption for DTCP, rbuf_len %d, sd %d", __FUNCTION__, rbuf_len, sd));
#else
    BDBG_MSG(("%s: Using SW decryption for DTCP, rbuf_len %d, sd %d", __FUNCTION__, rbuf_len, sd));
#endif
    if (rbuf_len > securityCtx->encryptBufSize) {
        /* Realloc */
        B_PlaybackIp_UtilsFreeMemory(securityCtx->encryptBuf);
        if ( (securityCtx->encryptBuf = (char *)B_PlaybackIp_UtilsAllocateMemory(rbuf_len, securityCtx->openSettings.heapHandle)) == NULL) {
            BDBG_ERR(("%s:%d: memory allocation failure\n", __FUNCTION__, __LINE__));
            return -1;
        }
        BDBG_MSG(("%s:%d: reallocated encrypt buffer", __FUNCTION__, __LINE__));
        securityCtx->encryptBufSize = rbuf_len;
    }
    read_ptr = securityCtx->encryptBuf;
    if (firstTime) {
        firstTime = 0;
#ifdef B_DTCP_IP_HW_DECRYPTION
        BDBG_WRN(("%s: Using HW decryption for DTCP (w/ cleaned up mmove), encypted & decrypted buffers are %s", __FUNCTION__, (read_ptr == securityCtx->encryptBuf) ? "SAME" : "DIFFERENT"));
#else
        BDBG_WRN(("%s: Using SW decryption for DTCP, encypted & decrypted buffers are %s", __FUNCTION__, (read_ptr == securityCtx->encryptBuf) ? "SAME" : "DIFFERENT"));
#endif
    }

    /* Return extra decrypted data from previous read */
    if (securityCtx->extended_len && securityCtx->extended_len <= HTTP_AES_BLOCK_SIZE) {
        bytesToCopy = rbuf_len > securityCtx->extended_len ? securityCtx->extended_len : rbuf_len;
        memcpy(rbuf, securityCtx->extended_decrypted_bytes, bytesToCopy);
        bytesRead = bytesToCopy;
        securityCtx->extended_len -= bytesRead;
        if (securityCtx->extended_len != 0) {
            /* move the remaining bytes to the beginning */
            memmove(securityCtx->extended_decrypted_bytes, securityCtx->extended_decrypted_bytes + bytesRead, securityCtx->extended_len);
        }
        BDBG_MSG(("%s: extended bytes returned %d, remaining extended bytes %d", __FUNCTION__, bytesRead, securityCtx->extended_len));
        return bytesRead;
    }

    /* Adjust read amount to meet the AES 16 byte alignment requirement */
    if (securityCtx->openSettings.security.enableDecryption) {
        /* Truncated if greater than 16 and not mod 16 */
        if ((rbuf_len > HTTP_AES_BLOCK_SIZE) && (rbuf_len % HTTP_AES_BLOCK_SIZE)) {
            truncated_len = rbuf_len % HTTP_AES_BLOCK_SIZE;
            rbuf_len -= truncated_len;   /* Truncate length of clear data expected in return */ 
            BDBG_MSG(("%s: Adjust length for decryption block size, original %d, new %d", __FUNCTION__, rbuf_len+truncated_len, rbuf_len));
        } else if (rbuf_len < HTTP_AES_BLOCK_SIZE) {
            /* since caller is asking to read/decrypt less than AES block size, we will need to read & decrypt extra bytes for 16 byte alignment */
            /* and then save the extended bytes for subsequent read */
            BDBG_MSG(("%s: Extend reading for %d bytes\n", __FUNCTION__,  rbuf_len));
            securityCtx->extended_len = HTTP_AES_BLOCK_SIZE - rbuf_len;
            rbuf_len += securityCtx->extended_len;   /*  Add padding  */ 
            orig_rbuf = rbuf;
            read_ptr = rbuf = temp_bytes;
        }
    }

    /* Copy previously read encrypted data (which was left over from the decryption due to PCP header being present) to begining of rbuf */
    /* Read length shouuld be 16 byte aligned */
    if (securityCtx->residual_encrypted_len && securityCtx->residual_encrypted_len <= HTTP_AES_BLOCK_SIZE ) {
        BDBG_MSG(("%s: Prepend previously read but not decrypted %d bytes, bytesRead so far %d", __FUNCTION__,  securityCtx->residual_encrypted_len, bytesRead));
        bytesToCopy = rbuf_len > securityCtx->residual_encrypted_len ? securityCtx->residual_encrypted_len : rbuf_len;
        memcpy(read_ptr+bytesRead, securityCtx->residual_encrypted_bytes, bytesToCopy);
        bytesRead += bytesToCopy;
        securityCtx->residual_encrypted_len -= bytesToCopy;
        if (securityCtx->residual_encrypted_len)
            memmove(securityCtx->residual_encrypted_bytes, securityCtx->residual_encrypted_bytes + bytesRead, securityCtx->residual_encrypted_len);
    }

    /* Copy previously read encrypted data (which was not decrypted due to mod 16 requirements) to begining of rbuf */
    /* Read length shouuld be 16 byte aligned */
    if (securityCtx->partial_read_len && securityCtx->partial_read_len <= HTTP_AES_BLOCK_SIZE) {
        bytesToCopy = (rbuf_len-bytesRead) > securityCtx->partial_read_len ? securityCtx->partial_read_len : (rbuf_len-bytesRead);
        BDBG_MSG(("%s: Preprend partial read %d bytes, total partial read %d, bytesRead already read %d", __FUNCTION__, bytesToCopy, securityCtx->partial_read_len, bytesRead));
        memcpy(read_ptr+bytesRead, securityCtx->partial_read_bytes, bytesToCopy);
        bytesRead += bytesToCopy;
        securityCtx->partial_read_len -= bytesToCopy;
        if (securityCtx->partial_read_len > 0)
            /* move the remaining bytes to the beginning of the partial_read_bytes array */
            memmove(securityCtx->partial_read_bytes, securityCtx->partial_read_bytes+bytesToCopy, securityCtx->partial_read_len);
    }

    if (securityCtx->initialPayloadLength && securityCtx->initialPayloadLength <= TMP_BUF_SIZE) {
        /* Copy initially read encrypted data to begining of rbuf */
        bytesToCopy = (rbuf_len-bytesRead) > securityCtx->initialPayloadLength ? securityCtx->initialPayloadLength : (rbuf_len-bytesRead);
        BDBG_MSG(("%s: copying read %d bytes from initial buf length of %d into decryption buffer, bytesRead so far %d", __FUNCTION__,  bytesToCopy, securityCtx->initialPayloadLength, bytesRead));
        memcpy(read_ptr+bytesRead, securityCtx->initialPayload, bytesToCopy);
        bytesRead += bytesToCopy;
        securityCtx->initialPayloadLength -= bytesToCopy;
        if (securityCtx->initialPayloadLength != 0)
            /* move the remaining bytes to the beginning of the intitialPayload */
            memmove(securityCtx->initialPayload, securityCtx->initialPayload + bytesToCopy, securityCtx->initialPayloadLength);
    }

    read_len = rbuf_len - bytesRead;
    bytesToRead = read_len;
    BDBG_MSG(("%s: now read remaining %d bytes, total rbuf_len %d, bytesRead %d", __FUNCTION__,  read_len, rbuf_len, bytesRead));

    if (rbuf_len < 0 || read_len < 0 ) 
        BDBG_ASSERT(NULL);
    /* read remaining bytesToRead bytes from the socket */
    while (read_len != 0) {
        /* read_len will only be 0 when initialPayload had all the requested bytes, so we dont need to read any data from the socket */
        if ((*playbackIpState == B_PlaybackIpState_eStopping) || (*playbackIpState == B_PlaybackIpState_eStopped || (*playbackIpState == B_PlaybackIpState_eWaitingToEnterTrickMode))) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of read loop due to state (%d) change\n", __FUNCTION__, *playbackIpState));
            return -1;
        }
        rc = read(sd, read_ptr + bytesRead, read_len);
        if (rc < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                if (bytesRead) {
                    BDBG_MSG(("%s: breaking from read loop due to partial data %d bytes recvd, bytesToRead %d", __FUNCTION__, bytesRead, bytesToRead));
                    break;  /* Partial data received */
                }
                else {
                    BDBG_MSG(("%s: Read System Call interrupted or timed out (errno %d)\n", __FUNCTION__, errno));
                    return rc;  /* no data read return */
                }
            }
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: read ERROR:%d", __FUNCTION__, errno));
#endif
            return -1;
        }else if (rc == 0) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: Reached EOF, server closed the connection!\n", __FUNCTION__));
#endif
            return 0;
        } else if (rc < read_len) 
        {
            bytesRead += rc;
            read_len -= rc;
            BDBG_MSG(("%s: Partial read rc %d, bytesRead so far %d, bytes remaining: read_len %d, rbuf_len %d, continue reading", __FUNCTION__, rc, bytesRead, read_len, rbuf_len));
            
#if 0
            /* TODO: need to spend more time understanding this whole function and impact of commenting this function */
            if(securityCtx->openSettings.security.enableDecryption)
            {
                BDBG_MSG(("%s: Partial read rc %d, rbuf_len %d, bytesRead %d!\n", __FUNCTION__, rc, rbuf_len, bytesRead));
                if((bytesRead % HTTP_AES_BLOCK_SIZE != 0) && (securityCtx->residual_encrypted_len==0))
                {
                    /* read more data if it's not 16 bytes aligned for AES block size.*/
                    read_len = HTTP_AES_BLOCK_SIZE - rc % HTTP_AES_BLOCK_SIZE;
                    continue;
                }else {
                    /* Decrypt this block of data */
                    break;
                }
            }else {
                break;
            }
#endif
        } else {
            bytesRead += rc;    
            BDBG_MSG(("%s: read all requested data: rc %d, bytesRead %d, read_len %d, rbuf_len %d", __FUNCTION__, rc, bytesRead, read_len, rbuf_len));
            break;
        }
    }
    
    /* if decryption is not yet enabled or is disabled, then simply return the read data */
    if (!securityCtx->openSettings.security.enableDecryption) {
        BDBG_MSG(("decryption is off: so memcopying the result of %d bytes from src %p to dst %p buffer", bytesRead, read_ptr, rbuf));
        memcpy(rbuf, read_ptr, bytesRead);
        return bytesRead;
    }

    BDBG_MSG(("%s: Asked %d bytes Read =%d bytes\n", __FUNCTION__, rbuf_len-securityCtx->extended_len+truncated_len, bytesRead));

    /* Partial data receive.  Data is NOT aligned to 16 byte */
    if (bytesRead % HTTP_AES_BLOCK_SIZE) {
        if (bytesRead > HTTP_AES_BLOCK_SIZE) {
            truncated_len = bytesRead % HTTP_AES_BLOCK_SIZE;
            BDBG_MSG(("%s: Partial data received %d, expected %d, need to truncate %d\n", __FUNCTION__, bytesRead, rbuf_len, truncated_len));
            bytesRead -= truncated_len;   /* Truncate length */ 
            securityCtx->partial_read_len = truncated_len;
            memcpy(securityCtx->partial_read_bytes, read_ptr+bytesRead, securityCtx->partial_read_len);
        }else {
            BDBG_MSG(("%s: bytesRead %d < AES block size, so returning EAGAIN: asked (rbuf_len) %d, app asked %d, rc %d, extended %d", 
                        __FUNCTION__, bytesRead, rbuf_len, rbuf_len-securityCtx->extended_len, rc, securityCtx->extended_len));
            securityCtx->partial_read_len = bytesRead;
            memcpy(securityCtx->partial_read_bytes, read_ptr, securityCtx->partial_read_len);
            errno = EAGAIN;
            bytesRead = 0; 
            securityCtx->extended_len = 0;
            return -1;
        }
    }

#if 0
    /* dump the encrypted data into file. */
    fwrite(playback_ip->encrypt_buf, 1, bytesRead, fenc);
    fflush(fenc);
#endif
    {
        unsigned int data_processed = 0 ;           /* This refer to each call to DepacketizData */
        unsigned int clear_buff_size = rbuf_len;
        unsigned int total_clear_data = 0;
        unsigned int beginPointer = 0;
        unsigned int endPointer = bytesRead;
        size_t bytesRemaining = bytesRead;
        BERR_Code errorCode;

        while (beginPointer != endPointer) {
            errorCode = DtcpAppLib_StreamDepacketizeData(securityCtx->streamHandle,
                    securityCtx->akeHandle,
                    (unsigned char *)read_ptr + beginPointer,   /* encrypted buffer */
                    bytesRemaining, /* encrypted buffer length */
                    (unsigned char *)rbuf + total_clear_data, /* clear buffer */
                    &clear_buff_size,   /* input: clear buffer length, output: length of decrypted bytes (may be less than data processed due to DTCP lib taking out PCP header) */
                    &data_processed,    /* how many bytes are processed by DTCP lib after decryption, includes the length of PCP header */
                    &dtcp_pcp_header_found);
        
            if (errorCode != BERR_SUCCESS) {
                BDBG_ERR(("%s: DTCP_DepacketizeData returned %d\n", __FUNCTION__, bytesRead));
                break;
            }
            if (playback_ip->chunkEncoding && dtcp_pcp_header_found ) {
                playback_ip->dtcpPcpHeaderFound = true;
                BDBG_MSG(("DTCP PCP header flag is set "));
            }

            BDBG_MSG(("len: bytesRemaining (enc) %d, clear %d, processed %d\n", bytesRemaining, clear_buff_size, data_processed));
            beginPointer += data_processed;
            bytesRemaining = endPointer - beginPointer;
        
            total_clear_data += clear_buff_size;

            /* Inform the API how much room we have left on output buffer.*/
            clear_buff_size = rbuf_len - clear_buff_size;

            /* Check for any unencrypted data */
            if ((bytesRemaining < HTTP_AES_BLOCK_SIZE) && (bytesRemaining > 0)) {
                BDBG_MSG(("bytesRemaining  %d are less than AES block size, save the encrypted bytes and continue next time, read_ptr %p, begin ptr %d, end ptr %d, clear_buf_size %d", 
                            bytesRemaining, read_ptr, beginPointer, endPointer, clear_buff_size));
                securityCtx->residual_encrypted_len = bytesRemaining;
                memcpy(securityCtx->residual_encrypted_bytes, read_ptr+beginPointer, securityCtx->residual_encrypted_len);
                if (securityCtx->extended_len && total_clear_data == 0) {
                    /* so caller had asked for data < AES block size and thus we had to extend the bytes to read, but since bytes remaining are less than AES block size, return EAGAIN to caller */
                    BDBG_MSG(("caller had asked for data < AES block size and thus we had to extend the bytes to read, but since bytes remaining are less than AES block size, return EAGAIN to caller"));
                    securityCtx->extended_len = 0;
                    errno = EAGAIN;
                    return -1;
                }
                break;
            }
        }

        /* Return the total decrypted data count */
        bytesRead = total_clear_data;
        BDBG_MSG(("playback_ip: total_clear_data=%d\n", bytesRead));

#ifdef RECORD_CLEAR_DATA
        /* write data to file */
        fwrite(rbuf, 1, bytesRead , fclear); 
        fflush(fclear);
#endif
    }
    /* If padding occured; then adjust length and copy data back to original buffer */
    if (securityCtx->extended_len) {
        BDBG_MSG(("%s: extended bytes saved %d, rbuf_len %d ", __FUNCTION__, securityCtx->extended_len, rbuf_len));
        rbuf_len -= securityCtx->extended_len;
        bytesRead -= securityCtx->extended_len;

        /* Save decrypted bytes */
        memcpy(temp_bytes, rbuf, bytesRead);
        memcpy(securityCtx->extended_decrypted_bytes,rbuf+rbuf_len,securityCtx->extended_len);

        /* copy from temp_bytes back to original rbuf */
        rbuf = orig_rbuf;
        memcpy(rbuf, temp_bytes, bytesRead);
    }
#if 0
#define VERIFY_SYNC_BYTE
#endif
#ifdef VERIFY_SYNC_BYTE
#define TS_PKT_SIZE 192
#define SYNC_BYTE_OFFSET 4
    {
        static long long totalBytesRead = 0;
        static int nextSyncByteOffset = 0;
        int i=0;
        if (bytesRead < nextSyncByteOffset) {
            nextSyncByteOffset -= bytesRead;
            totalBytesRead += bytesRead;
        }
        else {
            i = nextSyncByteOffset;
            for (i=nextSyncByteOffset; i<bytesRead; i+=TS_PKT_SIZE) {
                if (rbuf[i+SYNC_BYTE_OFFSET] != 0x47) BDBG_WRN(("################ sync bytes mismatch at %lld, i %d, bytes 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", totalBytesRead+i+SYNC_BYTE_OFFSET, i+SYNC_BYTE_OFFSET, rbuf[i], rbuf[i+1], rbuf[i+2], rbuf[i+3], rbuf[i+4], rbuf[i+5], rbuf[i+6], rbuf[i+7]));
#if 0
                else BDBG_MSG(("sync byte 0x%x, i %d, total %lld", rbuf[i+SYNC_BYTE_OFFSET], i+SYNC_BYTE_OFFSET, totalBytesRead));
#endif
            }
            totalBytesRead += bytesRead;
            nextSyncByteOffset = ((TS_PKT_SIZE - (bytesRead % TS_PKT_SIZE)) + nextSyncByteOffset) % TS_PKT_SIZE;
        }
    }
#endif
    return bytesRead;
}

int  B_PlaybackIp_DtcpIpDecryptionDisable(
    void *securityHandle
    )
{
    B_PlaybackIpDtcpIpCtx *securityCtx = (B_PlaybackIpDtcpIpCtx *)securityHandle;

    if (!securityCtx) {
        BDBG_ERR(("%s: invalid securityCtx %p", __FUNCTION__, securityCtx));
        return -1;
    }
    /* set flag to disable decryption */
    BDBG_MSG(("%s: Disabling DTCP/IP Decryption\n", __FUNCTION__));
    securityCtx->openSettings.security.enableDecryption = false;

    return 0;
}

int B_PlaybackIp_DtcpIpDecryptionEnable(
    void *securityHandle, 
    char *initialPayload, 
    int initialPayloadLength)
{
    B_PlaybackIpDtcpIpCtx *securityCtx = (B_PlaybackIpDtcpIpCtx *)securityHandle;

    if (!securityCtx || (initialPayloadLength && !initialPayload)) {
        BDBG_ERR(("%s: invalid securityCtx %p or initial paylaod params (length %d, payload ptr %p)\n", __FUNCTION__, securityCtx, initialPayloadLength, initialPayload));
        return -1;
    }
    /* set flag to enable decryption */
    BDBG_MSG(("%s: Enabling DTCP/IP Decryption\n", __FUNCTION__));
    securityCtx->openSettings.security.enableDecryption = true;

    if (initialPayloadLength) {
        securityCtx->initialPayloadLength = initialPayloadLength;
        if (initialPayloadLength > TMP_BUF_SIZE) {
            BDBG_ERR(("%s: Need to increase the initialPayload buffer length from %d to %d\n", __FUNCTION__, TMP_BUF_SIZE, initialPayloadLength));
            BDBG_ASSERT(NULL);
            return -1;
        }
        memcpy(securityCtx->initialPayload, initialPayload, initialPayloadLength);
    }

    return 0;
}

/*
 * Initialize session specific context of the security module
 * Called during B_PlaybackIp_SocketOpen().
 */
int B_PlaybackIp_DtcpIpSessionOpen(
    B_PlaybackIpSessionOpenSettings *openSettings,    /* input: provides server ip, port info */
    int sd,                                                     /* input: socket descriptor */
    B_PlaybackIpSecurityOpenOutputParams *securityOpenOutputParams) /* output: security settings return params */
{
    B_PlaybackIpDtcpIpCtx *securityCtx = NULL;
    B_PlaybackIpSecurityOpenSettings *securityOpenSettings;
    BERR_Code rc;

    BSTD_UNUSED(sd);

    securityOpenOutputParams->byteRangeOffset = 0;

    if (openSettings == NULL) {
        BDBG_ERR(("%s: Invalid parameters, Open Settings %p\n", __FUNCTION__, openSettings));
        goto error;
    }
    securityOpenSettings = &openSettings->security;

    if (sd <= 0) {
        BDBG_ERR(("%s: invalid socket, sd = %d", __FUNCTION__, sd));
        goto error;
    }

    if (securityOpenSettings->securityProtocol != B_PlaybackIpSecurityProtocol_DtcpIp) {
        BDBG_ERR(("%s: invoking DTCP/IP module with incorrect security protocol %d", __FUNCTION__, securityOpenSettings->securityProtocol));
        goto error;
    }

    if (securityOpenSettings->initialSecurityContext == NULL) {
        BDBG_ERR(("%s: AKE Handle is not provided by app\n", __FUNCTION__));
        goto error;
    }

    securityCtx = BKNI_Malloc(sizeof(B_PlaybackIpDtcpIpCtx));
    if (!securityCtx) {
        BDBG_ERR(("%s:%d: memory allocation failure\n", __FUNCTION__, __LINE__));
        goto error;
    }
    memset(securityCtx, 0, sizeof(B_PlaybackIpDtcpIpCtx));
    securityCtx->akeHandle = securityOpenSettings->initialSecurityContext;
    memcpy(&securityCtx->openSettings, openSettings, sizeof(B_PlaybackIpSessionOpenSettings));

    /* allocate an encrypt buffer of a default size and then re-malloc if user is asking for larger data */
    securityCtx->encryptBuf = (char *)B_PlaybackIp_UtilsAllocateMemory(DTCP_IP_ENCRYPT_BUF_SIZE, openSettings->heapHandle);
    if (!securityCtx->encryptBuf) {
        BDBG_ERR(("%s:%d: memory allocation failure\n", __FUNCTION__, __LINE__));
        goto error;
    }
    securityCtx->encryptBufSize = DTCP_IP_ENCRYPT_BUF_SIZE;
    if((securityCtx->streamHandle = DtcpAppLib_OpenSinkStream(securityCtx->akeHandle, B_StreamTransport_eHttp)) == NULL)
    {
        BDBG_ERR(("%S: Failed to open DTCP-IP sink stream\n", __FUNCTION__));
        rc = B_ERROR_SOCKET_ERROR;
        goto error;
    }

    BDBG_MSG(("%s: Successfully Opened Sink Stream: handles AKE %p, Stream %p\n", __FUNCTION__, securityCtx->akeHandle, securityCtx->streamHandle));

    BDBG_MSG(("%s: setting up the netIo interface for socket read & write\n", __FUNCTION__));
    securityOpenOutputParams->netIoPtr->read = _http_dtcp_ip_socket_read;
    /* TODO: if data written doesn't need to be protected, then write function need not be implemented */
    /* netIo->write = _http_dtcp_ip_socket_write; */
    securityOpenOutputParams->netIoPtr->close = B_PlaybackIp_DtcpIpSessionClose;
    securityOpenOutputParams->netIoPtr->suspend = NULL;

    *securityOpenOutputParams->securityHandle = (void *)securityCtx;
    return 0;
error:
    B_PlaybackIp_DtcpIpSessionClose(securityCtx);
    return -1;
}

int B_PlaybackIp_DtcpIpCloneSessionOpen(
    int sd,                                                     /* input: socket descriptor */
    void *source,                                               /* intput: original security handle */
    void **targetSecurityHandle)                                /* output: new security handle */
{
    (void)sd;
    B_PlaybackIpDtcpIpCtx *securityCtx = NULL;
    B_PlaybackIpDtcpIpCtx *sourceSecurityHandle = (B_PlaybackIpDtcpIpCtx *)source;

    securityCtx = BKNI_Malloc(sizeof(B_PlaybackIpDtcpIpCtx));
    if (!securityCtx) {
        BDBG_ERR(("%s:%d: memory allocation failure\n", __FUNCTION__, __LINE__));
        goto error;
    }
    memset(securityCtx, 0, sizeof(B_PlaybackIpDtcpIpCtx));
    securityCtx->akeHandle = sourceSecurityHandle->akeHandle;
    memcpy(&securityCtx->openSettings, &sourceSecurityHandle->openSettings, sizeof(B_PlaybackIpSessionOpenSettings));

    /* allocate an encrypt buffer of a default size and then re-malloc if user is asking for larger data */
    securityCtx->encryptBuf = (char *)B_PlaybackIp_UtilsAllocateMemory(DTCP_IP_ENCRYPT_BUF_SIZE, securityCtx->openSettings.heapHandle);
    if (!securityCtx->encryptBuf) {
        BDBG_ERR(("%s:%d: memory allocation failure\n", __FUNCTION__, __LINE__));
        goto error;
    }
    securityCtx->encryptBufSize = DTCP_IP_ENCRYPT_BUF_SIZE;
    if((securityCtx->streamHandle = DtcpAppLib_OpenSinkStream(securityCtx->akeHandle, B_StreamTransport_eHttp)) == NULL)
    {
        BDBG_ERR(("%S: Failed to open DTCP-IP sink stream\n", __FUNCTION__));
        goto error;
    }
    securityCtx->openSettings.security.enableDecryption = false;

    BDBG_MSG(("%s: Successfully Opened Sink Stream: handles AKE %p, Stream %p\n", __FUNCTION__, securityCtx->akeHandle, securityCtx->streamHandle));

    *targetSecurityHandle = securityCtx;
    return 0;
error:
    if (securityCtx && securityCtx->encryptBuf)
        B_PlaybackIp_UtilsFreeMemory(securityCtx->encryptBuf);
    if (securityCtx)
        BKNI_Free(securityCtx);
    return -1;
}

/*
 * Un-Initialize session specific context of the security module
 * Called during B_PlaybackIp_SocketClose().
 */
void B_PlaybackIp_DtcpIpSessionClose(
    void *voidHandle)                                           /* input: security module specific handle */
{
    B_PlaybackIpDtcpIpCtx *securityCtx = (B_PlaybackIpDtcpIpCtx *)voidHandle;
    if (securityCtx)
    {
        BDBG_MSG(("%s: Closing handles AKE %p, Stream %p\n", __FUNCTION__, securityCtx->akeHandle, securityCtx->streamHandle));
        /* Free up DtcpIp Handle & any other saved contexts */
        if(securityCtx->streamHandle) {
            DtcpAppLib_CloseStream(securityCtx->streamHandle);
        }
        if(securityCtx->encryptBuf){
            B_PlaybackIp_UtilsFreeMemory(securityCtx->encryptBuf);
        }
        BKNI_Free(securityCtx);
    }
}

static int gDtcpIpInitRefCnt = 0;
/*
 * Initialize global context of the security module
 * Called during B_PlaybackIp_Open().
 */
void * B_PlaybackIp_DtcpIpInit(void *nexusDmaHandle)
{
    void * ctx = NULL;
    
#ifndef B_DTCP_IP_HW_DECRYPTION
    BSTD_UNUSED(nexusDmaHandle);
#endif

    if (!gDtcpIpInitRefCnt) {
#if defined(B_DTCP_IP_HW_DECRYPTION)
        if(DtcpInitHWSecurityParams(nexusDmaHandle) != BERR_SUCCESS)
        {
            BDBG_ERR(("Failed to init DtcpIp HW Security params\n"));
            goto error;
        }
#endif

        if((ctx = DtcpAppLib_Startup(B_DeviceMode_eSink, false, 0, false)) == NULL)
        {
            BDBG_ERR(("DtcpIp AppLib faild to start\n"));
            goto error;
        }
        BDBG_MSG(("%s: done\n", __FUNCTION__));
    }
    gDtcpIpInitRefCnt++;

    return ctx;

error:

    DtcpAppLib_Shutdown(ctx);
#if defined(B_DTCP_IP_HW_DECRYPTION) || defined(B_DTCP_IP_HW_ENCRYPTION)
    DtcpCleanupHwSecurityParams();
#endif
    return NULL;
}

/*
 * Un-Initialize global context of the security module
 * Called during B_PlaybackIp_Close().
 */
void B_PlaybackIp_DtcpIpUnInit(void * ctx)
{
    
    gDtcpIpInitRefCnt--;
    if (!gDtcpIpInitRefCnt) {
        /* Global system un-initialization for DtcpIp library */
#ifdef B_DTCP_IP_HW_DECRYPTION
        DtcpCleanupHwSecurityParams();
#endif
        DtcpAppLib_Shutdown(ctx);
        /* unload library */
        BDBG_MSG(("%s: done\n", __FUNCTION__));
    }
}

#endif /* LINUX || VxWorks */
