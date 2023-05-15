/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: netrecv.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/netrecv.c $
 * 
 * 8   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <asm/cachectl.h>

#include "bsettop.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bnetaccel_info.h"

/* Test App to demonstrate the accelerated socket interface */
/* it supports mode to read from standard DGRAM socket */
/* as well as Broadcom's accelerated socket type */
#define ALIGN_4096   (4096 - 1)
#define MEM_SIZE 4*1024*1024
#define UDP_HDR_SIZE 8
#define RTP_HDR_SIZE 12

typedef enum {
    eProtoUDP,
    eProtoRTP,
    eProtoTCP,
    eProtoHTTP,
    eProtoMAX
} eProtos;

unsigned char *xbuf, *buf, *buftmp;
unsigned long total = 0;

#define DEBUG
#ifdef DEBUG
struct udphdr {
    unsigned short   source;
    unsigned short   dest;
    unsigned short   len;
    unsigned short   checksum;
};

typedef struct rtphdr {
    uint16_t word0;
#define VERSION_MASK 0xc000
#define VERSION_SHIFT 14
#define PADDING_MASK 0x2000
#define PADDING_SHIFT 13
#define EXTENSION_MASK 0x1000
#define EXTENSION_SHIFT 12
#define CSRC_MASK 0x0f00
#define CSRC_SHIFT 8
#define MARKER_MASK 0x0080
#define MARKER_SHIFT 7
#define TYPE_MASK 0x007f
#define TYPE_SHIFT 0
    uint16_t sequenceNum;
    uint32_t timestamp;
    uint32_t ssrc;
} rtphdr;

void printRtp(rtphdr *rtp)
{
	uint16_t word0;
	word0 = ntohs(rtp->word0);
	printf("RTP Hdr: Version %d Pad %d Ext %d CRSC count %d Marker %d Type %d ",
			(word0 & VERSION_MASK) >> VERSION_SHIFT,
			(word0 & PADDING_MASK) >> PADDING_SHIFT,
			(word0 & EXTENSION_MASK) >> EXTENSION_SHIFT,
			(word0 & CSRC_MASK) >> CSRC_SHIFT,
			(word0 & MARKER_MASK) >> MARKER_SHIFT,
			(word0 & TYPE_MASK) >> TYPE_SHIFT
		  );
    printf("Seq %d, TS %u, SSRC %u\n",
            ntohs(rtp->sequenceNum),
            ntohl(rtp->timestamp),
            ntohl(rtp->ssrc));
}

void printTS(unsigned char *ptr)
{
    printf("Transport Header Fields are:\n");
    printf("[0] 0x%x, [1] 0x%x, [2] 0x%x, [3] 0x%x\n", ptr[0], ptr[1], ptr[2], ptr[3]);
}

#endif

void usage()
{
    printf("Usage: netrecv [-a] [-q] [-s<num>] [-v<num>] [-b] [-p<num>] [-n<num>] [-t <num>] [-w] [-h] IP Port [filename to write or receive] [vpid] [vtype] [apid] [atype]\n");
    printf("options are:\n");
    printf(" -a <num>   # use accelerated socket type, default is regular socket\n");
    printf(" -q         # queue to playback HW for decode & display\n");
    printf(" -s <num>   # size of MPEG2 TS packets, default 188 bytes\n");
    printf(" -v         # IP version to use: IPv4: 4, IPv6: 6, default is IPv4\n");
    printf(" -b         # run with Brutus App, default is w/o Brutus\n");
    printf(" -p <0|1|2> # protocol: UDP|RTP|HTTP, default is RTP\n");
    printf(" -n <num>   # pkts to receive per read/recvfrom call, default is 64\n");
    printf(" -z <num>   # Per Datagram size, default hdr sz + 7 *TS size\n");
    printf(" -t <num>   # select timeout value in seconds, default is 20sec\n");
    printf(" -w         # write To Disk, default is no write\n");
    printf(" -h         # prints netrecv usage\n");
    printf("\n");
}

static int settopInit = 0;
int sfd, wfd, memfd;
	bresult rc;
	bdecode_t decode;
	bdisplay_t display;
	bdecode_window_t window;
	bplaypump_params playpump_params;
    bplaypump_open_params playpump_open_params;
	FILE *file;
	bplaypump_t play;
	bstream_mpeg mpeg;
	bstream_t stream;
	BKNI_EventHandle event;


void sigHandler(int signal)
{
    printf("**** Caught Signal # %d\n", signal);
    if (settopInit != 0)
    {
        printf("sighandler(): calling bsettop_uninit\n");
        settopInit = 0;
        close(sfd);
        bsettop_uninit();
    }
    exit(-1);
}

void play_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int settop_init(int vpid, bvideo_codec vtype, int apid, baudio_format atype, int pcrpid, int tsSize) 
{
	rc = bsettop_init(BSETTOP_VERSION);
	decode = bdecode_open(B_ID(0));
	display = bdisplay_open(B_ID(0));
	window = bdecode_window_open(B_ID(0), display);
	if (rc) goto error;
	
	BKNI_CreateEvent(&event);

    /* increase the buffering for IP STB to accomodate adequate "pre-charge" */
    bplaypump_get_open_params(B_ID(0), &playpump_open_params);
    playpump_open_params.buffer_size *= 2;
    playpump_open_params.num_descriptors *= 2;
    play = bplaypump_open(B_ID(0), &playpump_open_params);

    printf("Stream Info: vpid 0x%x, vtype 0x%x, apid 0x%x, atype 0x%x, pcr pid 0x%x, tsSize\n",
            vpid, vtype, apid, atype, pcrpid, tsSize);
	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = vpid;
	mpeg.video[0].format = vtype;
	mpeg.audio[0].pid = apid;
	mpeg.audio[0].format = atype;
	mpeg.pcr_pid = pcrpid;

	bplaypump_params_init(&playpump_params, play);
	playpump_params.read_callback = play_callback;
	playpump_params.callback_context = event;
        if (tsSize == 192)
	        playpump_params.timestamp_enabled = true;

	stream = bplaypump_start(play, &mpeg, &playpump_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;
    return 0;

error:
	return 1;
}

int feedPlaypump(struct iovec *vector, int pktCnt, int len)
{
    unsigned char *playbackBuf;
    size_t playbackBufSize = 0;
    int bytesToCopy, bytesToFeed = 0;
    int i, feedNow = 0;

    /*printf("*** need to feed %d bytes from %d pkts\n", len, pktCnt);*/
    i=0;
    while (i < pktCnt)
    {
        if (playbackBufSize <=0)
        {
            if (bplaypump_get_buffer(play, (void **)&playbackBuf, &playbackBufSize))
            {
                printf("$$$$$$ ERROR: failed to get buffer from playpump\n");
                return -1;
            }
            /*printf("#### [%d] allocated buffer 0x%x of %d bytes, need to feed %d bytes\n", 
                i, playbackBuf, playbackBufSize, len);*/
        }
        if (playbackBufSize == 0)
        {
            /*printf("$$$$$ playback buffer size %d, needed %d bytes, sleeping\n",
                playbackBufSize, len);*/
            BKNI_Sleep(10);
            continue;
        }
        /* now copy data into the playback buffer */
        if (vector[i].iov_len < playbackBufSize)
        {
            /* feed the complete packet */
            memcpy(playbackBuf, vector[i].iov_base, vector[i].iov_len);
            playbackBufSize -= vector[i].iov_len;
            bytesToFeed += vector[i].iov_len;
            playbackBuf += vector[i].iov_len;
            i++;
            if (i == pktCnt)
            {
                feedNow = 1;
            }
        }
        else
        {
            /* can only feed the partial packet */
            /*printf("^^^^^ [%d] feeding partial buffer, need %d, have %d, base %x\n", 
                    i, vector[i].iov_len, playbackBufSize, vector[i].iov_base);*/
            memcpy(playbackBuf, vector[i].iov_base, playbackBufSize);
            bytesToFeed += playbackBufSize;
            vector[i].iov_base = (unsigned char *)vector[i].iov_base + playbackBufSize;
            vector[i].iov_len -= playbackBufSize;
            playbackBufSize = 0;

            /* reached end of playback buffer, so feed it now */
            feedNow = 1;
        }
        if (feedNow)
        {
            /* feed the data to the playpump */
		    if (bplaypump_read_complete(play, 0, bytesToFeed) != b_ok)
            {
                printf("$$$$$ ERROR: bplaypump_read_complete() failed\n");
                return -1;
            }
            len -= bytesToFeed;
            /* printf("#####[%d] Fed %d bytes (total %d), remaining %d \n", i, bytesToFeed, total, len);*/
            bytesToFeed = 0;
            feedNow = 0;
        }
    }
}

int pktsPerRead = 100;
char write_buf[1024];
char read_buf[1024];
unsigned char *rbuf;
bresult rc;
int ret;
int i, c;
int bufSize, hdrSize = 0, pktSize =0, num = 0, brutus=0, iphVersion = 4;
int pktCnt = 0, sockType, writeToDisk = 0, doSockAccel = 0, timeout = 20, tsSize = 188;
eProtos proto = eProtoRTP;
struct iovec *vector;
int bytesRecv =0, nwritten;
unsigned char *ptr;
unsigned short seq =0, expectedSeq = 0;
unsigned short port;
unsigned short udpLen = 0;
struct udphdr *udp;
struct rtphdr *rtp;
unsigned char sockaddr_storage[NI_MAXHOST];
struct sockaddr *saddr;
socklen_t saddrLen;
struct ip_mreq mreq;
struct ipv6_mreq ipv6mreq;
fd_set rfds;
struct timeval tv;
char *host, *service, *filename = "./rec0.mpg";
struct addrinfo  hints;
struct addrinfo *addrInfo;
char hostbuf[NI_MAXHOST];
char serverbuf[NI_MAXSERV];
int firstTime = 1;
int feedToPlayback = 0;
int bytesToFeed = 0;
int vpid, apid, pcrpid;
bvideo_codec vtype;
baudio_format atype;
STRM_SockRecvParams_t sockRecvParams;

int sendHttpReq(int sfd, struct addrinfo *addrInfo, char *filename)
{
    int total = 0, len;

    if (connect(sfd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0)
    {
        perror("connect():");
        goto exit;
    }
    printf("Socket connect done\n");

    /* build HTTP request */
    sprintf(write_buf,
            "GET %s HTTP/1.1\r\n"
            "User-Agent: BRCM 97400\r\n"
            "Delay: 1 seconds\r\n"
            "PlaySpeed.dlna.org: speed=%d\r\n"
            "TimeSeekRange.dlna.org: npt=%.3f\r\n"
            "\r\n", 
            filename, 1, 0.);

    /* need to flush the cache to allow data to be written to memory */
	cacheflush(write_buf, strlen(write_buf), DCACHE);

    /* send the request */
    if (write(sfd, write_buf, strlen(write_buf)) < 0)
    {
        perror("write():");
        goto exit;
    }
    printf("HTTP Req (%d bytes) Sent: \n%s\t\t\n", strlen(write_buf), write_buf);

    /* read the response back */
    rbuf = buf;
    total = 0;
    while (true)
    {
        printf("before read\n");
        len = read(sfd, rbuf, 1023 - total);
        if (len <= 0)
        {
            perror("#### ERROR in http response read");
            total = 0;
            return -1;
        }

        rbuf += len;
        total += len;
        *rbuf = 0;
        printf("HTTP Response (%d bytes): %s\n", len, buf);
        /* skip to the end of the HTTP header */
        if (!strncmp("\r\n", (char *)buf + total-2, 2))
            break;
    }

    /* now that we have read the HTTP response, increase the # of pkts per read */
	if (doSockAccel) {
    	sockRecvParams.pktsPerRecv = pktsPerRead;
    	sockRecvParams.pktOffset = 0;
    	sockRecvParams.hdrOffset = 0;	/* receive UDP header also */
    	if (setsockopt(sfd, SOCK_BRCM_STREAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
    	{
        	perror("setsockopt():recv options:");
        	goto exit;
    	}
    	printf("Modified the default pkts per recvfrom to %d\n", pktsPerRead);
	}
    return 0;

exit:
    return -1;
}

int main(int argc, char *argv[])
{

    while ( (c = getopt(argc, argv, "z:qs:v:p:wn:t:ahb")) != -1)
    {
        switch (c) 
        {
        case 'v':
            iphVersion = atoi(optarg);
            if (iphVersion != 4 && iphVersion != 6)
            {
                printf("Incorrect IP Version (%d)\n", iphVersion);
                goto usage;
            }
            break;
        case 'p':
            proto = atoi(optarg);
            if (proto > eProtoMAX)
            {
                printf("Incorrect Protocol Option (%d)\n", proto);
                goto usage;
            }
            break;
        case 's':
            tsSize = atoi(optarg);
            if (tsSize != 188 && tsSize != 192 && tsSize != 130)
            {
                printf("Not supported transport Size (%d)\n", tsSize);
                goto usage;
            }
            break;
        case 'q':
            feedToPlayback = 1;
            break;
        case 'w':
            writeToDisk = 1;
            break;
        case 'n':
            pktsPerRead = atoi(optarg);
            if (pktsPerRead > 128)
            {
                printf("Incorrect Pkts Per Read (%d)\n", pktsPerRead);
                goto usage;
            }
            break;
        case 'z':
            pktSize = atoi(optarg);
            if (pktSize > 2000)
            {
                printf("Per UDP Datagram size %d is too large?\n", pktSize);
                goto usage;
            }
            break;
        case 't':
            timeout = atoi(optarg);
            break;
        case 'a':
            doSockAccel = 1;
            break;
        case 'b':
            brutus = 1;
            break;
        case 'h':
        default:
usage:
            usage();
            return -1;
        }
    }

    /* Get Server's IP Address & Port Info */
    switch ( argc - optind )
    {
        case 7:   /* host ip, port, vidpid, vtype, audio pid, atype are specified on the command line. */
        {
            host = argv[optind];
            service = argv[optind + 1];
            vpid = atoi(argv[optind+2]);
            vtype = atoi(argv[optind+3]);
            apid = atoi(argv[optind+4]);
            atype = atoi(argv[optind+5]);
            pcrpid = atoi(argv[optind+6]);
            break;
        }
        case 3:   /* host, port & file name are specified on the command line. */
        {
            host = argv[optind];
            service = argv[optind + 1];
            filename = argv[optind + 2];
            vpid = 0x31;    /* defaults */
	        vtype = bvideo_codec_mpeg2;
            apid = 0x34;
	        atype = baudio_format_ac3;
            pcrpid = vpid;
            break;
        }
        case 2:   /* Both host & port are specified on the command line. */
        {
            host = argv[optind];
            service = argv[optind + 1];
            vpid = 0x31;    /* defaults */
	        vtype = bvideo_codec_mpeg2;
            apid = 0x34;
	        atype = baudio_format_ac3;
            pcrpid = vpid;
            break;
        }
        default:
        {
            usage();
            return -1;
        }
    }

    /* initialize Settop APIs: this also initializes the kernel driver */
    if (!brutus || feedToPlayback)
    {
        if (settop_init(vpid, vtype, apid, atype, pcrpid, tsSize))
            goto exit;
        printf("Settop APIs & BCM Driver is initialized\n");
        settopInit = 1;
    }

    /* Catch signals to do proper Settop API un-initialize */
    if (!brutus)
    {
        signal(SIGHUP, sigHandler);
        signal(SIGINT, sigHandler);
        signal(SIGTERM, sigHandler);
        signal(SIGQUIT, sigHandler);
        signal(SIGSEGV, sigHandler);
        signal(SIGPIPE, sigHandler);
    }

    if (proto == eProtoTCP)
        goto pktSize;
    /* Note: on why UDP & RTP headers can be optionally copied along with the payloads */
    /* For UDP & RTP Protocols, packet headers can also be copied along with the payload */
    /* This allows applications do any RTP speoific processing (e.g FEC). */
    /* In some instances, the payload size can vary from packet to packet (upto 1-7 MPEG2 TS packets)*/
    /* For this reason, UDP header is also copied along w/ RTP header. This allows apps to determine */
    /* per packet payload length (udp length field */
    /* Add RTP Header Size for RTP Protocol */
    if (doSockAccel)
        (proto == eProtoUDP) ? (hdrSize = UDP_HDR_SIZE) : (hdrSize = UDP_HDR_SIZE + RTP_HDR_SIZE);
    else
        /* standard linux network stack doesn't provide access to UDP header */
        (proto == eProtoUDP) ? (hdrSize = 0) : (hdrSize = RTP_HDR_SIZE);
pktSize:
    if (!pktSize)
        pktSize = hdrSize + (7*tsSize);
    bufSize = pktSize * pktsPerRead;
    printf("Options are --> proto %d, hdrSize = %d, bufSize %d, writeToDisk %d, pktsPerRead %d, sockAccel %d\n", 
            proto, hdrSize, bufSize, writeToDisk, pktsPerRead, doSockAccel);

    /* allocate receive buffer */
    if ( (xbuf = malloc(bufSize + ALIGN_4096)) == NULL )
    {
        perror("malloc failure():");
        goto exit;
    }
    buf = (unsigned char *)(((unsigned long)xbuf + ALIGN_4096) & ~ALIGN_4096);
    memset(xbuf, (bufSize + ALIGN_4096), 0);
    printf("Successfully malloced (addr = 0x%x)\n", buf);

    /* setup the vector arrary: each vector corresponds to a pkt */
    if ( (vector = malloc(sizeof(struct iovec) * pktsPerRead)) == NULL )
    {
        perror("malloc failure():");
        goto exit;
    }
    for (i=0; i < pktsPerRead; i++) 
    {
        vector[i].iov_base = buf + i*pktSize;
        vector[i].iov_len = pktSize;
    }
    
    {
    /* prep up fields for the getaddrinfo() */
    memset(&hints, 0, sizeof(hints));
    /* set socket family */
    (iphVersion == 4) ? (hints.ai_family = AF_INET) : (hints.ai_family = AF_INET6);
    (proto == eProtoTCP) ?  (hints.ai_protocol = IPPROTO_TCP) : (hints.ai_protocol = IPPROTO_UDP);

    /* set socket type */
    /* set it to standard socktype as getaddrinfo doesn't understand our new sock type */
    /* after getaddrinfo() call, we change the type to new Broadcom accelerated socket type */
    (proto == eProtoTCP) ?  (hints.ai_socktype = SOCK_STREAM):(hints.ai_socktype = SOCK_DGRAM);
    if ((ret = getaddrinfo(host, service, &hints, &addrInfo)) != 0)
    {
        printf("getaddrinfo() Failed: error code = %d\n", ret);
        goto exit;
    }

    /* now update the sock type to our new type */
    /* this is a hack until we update the uclibc to understand the new socket type */
    if (doSockAccel)
    {
        (proto == eProtoTCP) ? 
            (addrInfo->ai_socktype = SOCK_BRCM_STREAM) :
            (addrInfo->ai_socktype = SOCK_BRCM_DGRAM);
    }
    saddrLen = NI_MAXHOST;
    saddr = (struct sockaddr *)sockaddr_storage;

    if ( iphVersion == 6 && ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id == 0)
    {
        printf("IPv6 scope-ID is not set\n");
		((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id = 1;
    }

    printf("IP Address Info:\n"
            "   family       = %2d (PF_INET = %d, PF_INET6 = %d)\n"
            "   socktype     = %2d (SOCK_DRAM = %d, SOCK_BRCM_DGRAM %d, SOCK_STREAM = %d, SOCK_BRCM_STREAM %d)\n"
            "   protocol     = %2d (IPPROTO_UDP = %d, IPPROTO_TCP %d)\n"
            "   addrlen      = %2d\n"
            "   flags        = %d\n",
            addrInfo->ai_family,
            PF_INET, PF_INET6,
            addrInfo->ai_socktype,
            SOCK_DGRAM, SOCK_BRCM_DGRAM,
            SOCK_STREAM, SOCK_BRCM_STREAM,
            addrInfo->ai_protocol,
            IPPROTO_UDP, IPPROTO_TCP,
            addrInfo->ai_addrlen,
            addrInfo->ai_flags);

    getnameinfo(addrInfo->ai_addr, addrInfo->ai_addrlen, hostbuf, sizeof(hostbuf),
                serverbuf, sizeof(serverbuf), NI_NUMERICHOST | NI_NUMERICSERV);

    printf("   IP Addr      = %s\n"
           "   Port         = %s\n",
           hostbuf, serverbuf);
    
    /* Create Datagram socket */
    if ( (sfd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol)) < 0 )
    {
        perror("socket():");
        goto exit;
    }
    printf("Socket Created\n");
    }

    if (!doSockAccel)
    {
        /* increase the receive buffer size for UDP socket, network 
           stack will drop pkts at >14Mbps rate */
        int size = 250000;
        if (setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &size, 4) != 0)
        {
            perror("setsockopt():");
        	goto exit;
        }
        printf("Increased the Socket RCVBUF size to %d bytes\n", size);
    }

    /* Bind to the Receiving IP Address & Port */
    if (proto != eProtoTCP && (bind(sfd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0)) 
    {
        perror("bind():");
        goto exit;
    }
    printf("Socket bind done\n");

    /* Note: */
    /* Apps can choose to either read "n" bytes of data or "m" IP packets at a time */
    /* by default, i/f is designed to wait for "n" bytes and only then return */
    /* To instead read only "m" packets at a time, Apps can set per this number */
    /* via a socket option. read/recvfrom calls can block until these conditions are met */

    /* by default, read/recvfrom i/f returns lower of (bytes to be read or bytes in 1 pkt) */
    /* this is needed to allow app only read the essential protocol headers in the begining. */
    /* E.g. HTTP & DTCP/IP protocols headers can be read by app by reading 1 pkt at a time */
    /* once headers are read, apps can increase the # of pkts (thus read more payload data) */
    /* by setting a socket option */

    /* set the number of pkts to wait before returning from recvfrom */
    if (doSockAccel && proto != eProtoTCP) 
    {
        sockRecvParams.pktsPerRecv = pktsPerRead;
        sockRecvParams.pktOffset = pktSize;
        if (setsockopt(sfd, SOCK_BRCM_DGRAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
        {
            perror("setsockopt():recv options:");
            goto exit;
        }
        printf("Modified the default pkts per recvfrom to %d, pkt offset to %d\n", pktsPerRead, pktSize);
    }

    /* Note on each packet payload offset */
    /* read/recvfrom api also provides flexibility to copy eash packet at a fixed offset */
    /* instead of copying payloads of multiple packets back to back. */
    /* this is mainly useful for RTP processing where individual pkts can be of variable lengths */
    /* and applications wants each packet to begin at a fixed offset */
    /* set the per pkt size, kernel then copies each pkt at fixed offset */
    /* Join the Multicast Group */

	if (iphVersion == 4)
	{
		if (IN_MULTICAST(ntohl(((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr)))
    	{
        	printf("Joining IPv4 Multicast Address Group\n");
    		mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr;
    		mreq.imr_interface.s_addr = INADDR_ANY;
        	if (setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) 
        	{
            	perror("setsockopt():");
        		goto exit;
        	}
    	}
	}
	else	/* IPv6 case */
	{
		if ( IN6_IS_ADDR_MULTICAST(((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr.s6_addr))
		{
        	printf("Joining IPv6 Multicast Address Group\n");
			ipv6mreq.ipv6mr_multiaddr = ((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr;
			ipv6mreq.ipv6mr_interface = 0;	/* chooses default interface */
        	if (setsockopt(sfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &ipv6mreq, sizeof(ipv6mreq)) < 0) 
        	{
            	perror("setsockopt():");
        		goto exit;
        	}
		}
	}
    
    /* TCP is connection oriented protocols, setup connection with the streaming server */
    /* and send HTTP Request to retrieve the content */
    if (proto == eProtoTCP)
    {
        if (sendHttpReq(sfd, addrInfo, filename) < 0)
            goto exit;
    }
    /* Open the temp file where received content is written to: for testing purpose */
/* #define O_DIRECT	0x8000 */
    /* if ((wfd = open("./rec0.mpg", O_RDWR|O_CREAT|O_TRUNC|O_DIRECT)) < 0) */
    if (writeToDisk && (wfd = open(filename, O_RDWR|O_CREAT|O_TRUNC)) < 0) 
    {
        perror("open():");
        goto exit;
    }
    printf("Ready to Receive Data...\n");

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(sfd, &rfds);
        tv.tv_sec = timeout;
        tv.tv_usec = 0;

        if ( (ret = select(sfd +1, &rfds, NULL, NULL, &tv)) < 0 )
        {
            perror("select(): exiting...");
        	goto exit;
        }

        if (!FD_ISSET(sfd, &rfds))
        {
            printf("**** No Input From Server, EXITING.... \n");
        	goto exit;
        }

        /* Use recvfrom() or read() to receive the RTP header (if present) + payload */
        /* if ( (bytesRecv = read(sfd, buf, bufSize)) < 0 ) */
        if (proto == eProtoTCP)
        {
            if ( (bytesRecv = read(sfd, buf, bufSize)) < 0)
            {
                perror("recvfrom():");
        	    goto exit;
            } 
            /* reset the iov_base pointers */
            for (i=0; i < pktsPerRead; i++)
            {
                vector[i].iov_base = buf + i*pktSize;
                vector[i].iov_len = pktSize;
            }
            bytesToFeed = bytesRecv;
            goto feed;
        }
        if (doSockAccel)
        {
            if ( (bytesRecv = recvfrom(sfd, buf, bufSize, 0, saddr, &saddrLen)) < 0)
            {
                perror("recvfrom():");
        	    goto exit;
            }
        }
        else
        {
            /* need to make multiple recvfrom calls to read multiple packets */
            ptr = buf;
            for (i=0; i < pktsPerRead; i++) 
            {
                if ( (bytesRecv = recvfrom(sfd, ptr, bufSize, 0, saddr, &saddrLen)) < 0)
                {
                    perror("recvfrom():");
        	        goto exit;
                }
                vector[i].iov_base = ptr;
                vector[i].iov_len = bytesRecv;
                ptr += pktSize;
                /*printf("read %d in 0x%x, ", bytesRecv, vector[i].iov_base);*/
            }
            /*printf("\n");*/
        }

        /* Adjust payload pointers & optionally verify RTP Sequence */
        ptr = buf;
        bytesToFeed = 0;
        for (i=0; i< pktsPerRead; i++)
        {
            num++;
            expectedSeq++;
            if (doSockAccel)
            {
                udp = (struct udphdr *)ptr;
                udpLen = ntohs(udp->len);
                rtp = (rtphdr *) (ptr + sizeof(struct udphdr));
                seq = ntohs(rtp->sequenceNum);

                /* adjust the payload pointer */
                vector[i].iov_base = ptr + hdrSize;
                vector[i].iov_len = udpLen - hdrSize;
                bytesToFeed+= udpLen - hdrSize;
            }
            else
            {
                /* standard socket recvfrom: kernel returns only one pkt starting at RTP header */
                udpLen = vector[i].iov_len;
                rtp = (rtphdr *) ptr;
                seq = ntohs(rtp->sequenceNum);

                /* adjust the payload pointer */
                vector[i].iov_base = ptr + hdrSize;
                vector[i].iov_len = udpLen - hdrSize;
                bytesToFeed += udpLen - hdrSize;
            }
            if (proto == eProtoRTP)
            {
                if (!firstTime)
                {
                    if ( seq != expectedSeq )
                    {
                        printf("********* Break in RTP Seq (expected seq %u, current seq %u, pktnum %d ***********\n", 
                                expectedSeq-1, seq, num);
                        printRtp(rtp);
                        expectedSeq = seq;
        				/* goto exit; */
                    }
                }
                else
                {
                    expectedSeq = ntohs(rtp->sequenceNum);
                    firstTime = 0;
                    printf("########## Initial RTP Seq %d, pktsize %d, pktnum %d ###########\n", 
                            expectedSeq, udpLen, num);
                    printRtp(rtp);
                }
            } /* if RTP */
            /* next pkt */
            ptr += pktSize;
        }

        /* now write it to disk either using write or writev*/
        if (writeToDisk)
        {
            if (proto == eProtoUDP && !doSockAccel)
            {
                /* Plain UDP, do a block write */
                if ( (nwritten = write(wfd, buf, bytesRecv)) < 0 )
                {
                    perror("write():");
        			goto exit;
                }
                if (nwritten != bytesRecv)
                {
                    printf("Write Error: only wrote %u our of %u received bytes\n", nwritten, bytesRecv);
        			goto exit;
                }
            }
            else
            {
                /* RTP + UDP, use vectored write to skip the RTP header */
                if ( (nwritten = writev(wfd, vector, pktsPerRead)) < 0 ) 
                {
                    perror("write():");
        			goto exit;
                }
            }
        }

        /* Now feed the received buffers to playpump */
feed:
        if (feedToPlayback && feedPlaypump(vector, pktsPerRead, bytesToFeed) )
            goto exit;

        bytesToFeed = 0;
        total += bytesRecv;
        pktCnt += pktsPerRead;
        if (pktCnt % (pktsPerRead * 100) == 0) 
        {
            if (saddr->sa_family == AF_INET)
            {
                inet_ntop(saddr->sa_family, &((struct sockaddr_in *)saddr)->sin_addr, hostbuf, NI_MAXHOST);
                port = ntohs(((struct sockaddr_in *)saddr)->sin_port);
            }
            else
            {
                inet_ntop(saddr->sa_family, &((struct sockaddr_in6 *)saddr)->sin6_addr, hostbuf, NI_MAXHOST);
                port = ntohs(((struct sockaddr_in6 *)saddr)->sin6_port);
            }
            printf("Rcvd %u bytes in %u pkts from %s:%d, total bytes rcvd %u\n", 
                bytesRecv, pktCnt, hostbuf, port, total);
        }
    } /* while (1) */

exit:
    close(sfd);
    if (!brutus && settopInit) {
		bsettop_uninit();
    }
    settopInit = 0;
    return 0;
}
