/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playpump_ip.c $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description: simple test app to demonstrate UDP/RTP receiving using Normal & Accelerated Sockets
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 *
 *************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_sync_channel.h"
#include "di_demux.h"
#include "drv_pvr.h"
#include "drv_demux.h"
#include "di_video.h"
#include "di_audio.h"
#include "drv_video.h"
#include "drv_audio.h"
#include "vkernel.h"
#include "taskdef.h"
#include "htype.h"


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

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
#include <asm/cachectl.h>

/* Simple test App to demonstrate IP tuning using Nexus Playpump Interface */
/* It supports mode to read from standard DGRAM socket and Broadcom's accelerated socket type */

/* **************************************************************************** */
/*       !!!!!!!!!!!!!!!!!!!!              NOTE         !!!!!!!!!!!!!!!!!!!!!!! */
/* **************************************************************************** */
/* This example **doesn't** provide an example of a Nexus Configuration for supporting high n/w jitter */
/* which is typically present in the IP Networks. */

/* For a complete IP Application, please use the NEXUS IP Applib. There is an example app, */
/* ip_applib_test.c, in rockford/unittests/applibs/playback_ip to demonstrate its usage. */
/* This example app should be used to write customer application. */

typedef int BOOL;


#define ALIGN_4096   (4096 - 1)
#define MEM_SIZE 4*1024*1024
#define UDP_HDR_SIZE 8
#define RTP_HDR_SIZE 12
#define NEXUS_NUM_AUDIO_DECODERS 3  /* On the 7405, this is possible but may exhaust DSP bandwidth with transcoding */
#define FAI_AV_AUDIO_DEVICE_ID		0

#define DISPLA_SD	0
#define DISPLAY_HD 	1
#define DISPLAY_COLOR_BAR 	2

typedef enum {
    eProtoUDP,
    eProtoRTP,
    eProtoMAX
} eProtos;

typedef struct {
    int pktsPerRead;
    eProtos proto;
    int writeToDisk;
    int timeout;
    int tsSize;
	int audiopid;
	int videopid;
	int audioCodec;
	int videoCodec;
    int iphVersion;
    short port;
    char *host;
    char *service;
    char *filename;
    int pktSize;
    int decode;
}ExampleIpCfg;

typedef struct {
    int sfd;
    int wfd;
    int bufSize;
    int hdrSize;
    struct iovec *vector;
    uint8_t *buf;
    int bytesRecv;
    int bytesToFeed;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    unsigned char sockaddr_storage[NI_MAXHOST];
    bool bIsPlay;
}ExampleIpCtx;


int feedToPlayback = 0;

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

unsigned int	ulTaskId = 0;
unsigned long	s_ulStreamTaskId = 0;

NEXUS_PlaypumpSettings playpumpSettings;
NEXUS_PlaypumpHandle playpump = NULL;
NEXUS_StcChannelHandle stcChannel = NULL;
NEXUS_StcChannelSettings stcSettings;
NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
NEXUS_AudioDecoderHandle hAudioCoder[NEXUS_NUM_AUDIO_DECODERS] ;
NEXUS_AudioDecoderStartSettings audioProgram;
NEXUS_SyncChannelHandle nSyncChannelHandle = NULL;
NEXUS_SyncChannelSettings nSyncChannelSettings;
NEXUS_VideoDecoderSettings decoderSettings;
NEXUS_VideoDecoderHandle videoDecoder = NULL;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_StcChannelStatus status;
unsigned long streamId,stChangeId;
ExampleIpCtx ipCtxVal;
ExampleIpCfg ipCfgVal;

static void printRtp(rtphdr *rtp)
{
    uint16_t word0;
    word0 = ntohs(rtp->word0);
    VK_printf("RTP Hdr: Version %d Pad %d Ext %d CRSC count %d Marker %d Type %d ",
            (word0 & VERSION_MASK) >> VERSION_SHIFT,
            (word0 & PADDING_MASK) >> PADDING_SHIFT,
            (word0 & EXTENSION_MASK) >> EXTENSION_SHIFT,
            (word0 & CSRC_MASK) >> CSRC_SHIFT,
            (word0 & MARKER_MASK) >> MARKER_SHIFT,
            (word0 & TYPE_MASK) >> TYPE_SHIFT
          );
    VK_printf("Seq %d, TS %u, SSRC %u\n",
            ntohs(rtp->sequenceNum),
            ntohl(rtp->timestamp),
            ntohl(rtp->ssrc));
}

static void printTS(unsigned char *ptr)
{
    VK_printf("Transport Header Fields are:\n");
    VK_printf("[0] 0x%x, [1] 0x%x, [2] 0x%x, [3] 0x%x\n", ptr[0], ptr[1], ptr[2], ptr[3]);
}

#endif /* DEBUG */


static void update_stats(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    static int pktCnt = 0;
    static int total;
    int port;
    char hostbuf[NI_MAXHOST] = {0};
    struct sockaddr *saddr;

    saddr = (struct sockaddr *)ipCtx->sockaddr_storage;
    total += ipCtx->bytesRecv;
    pktCnt += ipCfg->pktsPerRead;
    if (pktCnt % (ipCfg->pktsPerRead * 100) == 0) {
        if (saddr->sa_family == AF_INET) {
            inet_ntop(saddr->sa_family, &((struct sockaddr_in *)saddr)->sin_addr, hostbuf, NI_MAXHOST);
            port = ntohs(((struct sockaddr_in *)saddr)->sin_port);
        }
        else {
            inet_ntop(saddr->sa_family, &((struct sockaddr_in6 *)saddr)->sin6_addr, hostbuf, NI_MAXHOST);
            port = ntohs(((struct sockaddr_in6 *)saddr)->sin6_port);
        }
        VK_printf("Rcvd %u bytes in %u pkts from %s:%d, total bytes rcvd %u\n",
            ipCtx->bytesRecv, pktCnt, hostbuf, port, total);
    }
}

static void set_default_ip_cfg(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    VK_memset(ipCfg, 0, sizeof(ExampleIpCfg));
    VK_memset(ipCtx, 0, sizeof(ExampleIpCtx));

    ipCfg->pktsPerRead = 32;
#if (CERTI_AGING_TEST)
	ipCfg->proto = eProtoUDP;
#else
	ipCfg->proto = eProtoRTP;
#endif
    ipCfg->writeToDisk = 0;
    ipCfg->timeout = 2;
    ipCfg->tsSize = 188;
    ipCfg->iphVersion = 4;
    ipCfg->filename = "./rec0.mpg";
    ipCfg->decode = 1;
}

static int allocate_buffers(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    int i;
    uint8_t *xbuf;

    /*
     * Note: on NetAccel copies UDP & RTP headers along with the each video payload
     * This allows applications do any RTP speoific processing (e.g FEC).
     * In some instances, the payload size can vary from packet to packet (upto 1-7 MPEG2 TS packets)
     * For this reason, UDP header is also copied along w/ RTP header. This allows apps to determine
     * per packet payload length (udp length field
     */

    /* Calculate the protocol header length: Add RTP Header Size for RTP Protocol */
    /* standard linux network stack doesn't provide access to UDP header */
    (ipCfg->proto == eProtoUDP) ? (ipCtx->hdrSize = 0) : (ipCtx->hdrSize = RTP_HDR_SIZE);

    /* if user didn't specify per packet size, calculate it */
    if (!ipCfg->pktSize)
        ipCfg->pktSize = ipCtx->hdrSize + (7*ipCfg->tsSize);

    /* and use that to determine buffer needed to hold pktsPerRead worth of data */
    ipCtx->bufSize = ipCfg->pktSize * ipCfg->pktsPerRead;
    VK_printf("Options used are --> proto %d, hdrSize = %d, bufSize %d, writeToDisk %d, pktsPerRead %d\n",
            ipCfg->proto, ipCtx->hdrSize, ipCtx->bufSize, ipCfg->writeToDisk, ipCfg->pktsPerRead);

    /* allocate receive buffer */
    if ( (xbuf = VK_malloc(ipCtx->bufSize + ALIGN_4096)) == NULL ) {
        perror("VK_malloc failure():");
        goto exit;
    }
    ipCtx->buf = (uint8_t *)(((unsigned long)xbuf + ALIGN_4096) & ~ALIGN_4096);
    VK_memset(xbuf, (ipCtx->bufSize + ALIGN_4096), 0);
    VK_printf("Successfully malloced (addr = %p)\n", ipCtx->buf);

    /* setup the vector arrary: each vector corresponds to a pkt */
    if ( (ipCtx->vector = VK_malloc(sizeof(struct iovec) * ipCfg->pktsPerRead)) == NULL ) {
        perror("VK_malloc failure():");
        goto exit;
    }
    for (i=0; i < ipCfg->pktsPerRead; i++) {
        ipCtx->vector[i].iov_base = ipCtx->buf + i*ipCfg->pktSize;
        ipCtx->vector[i].iov_len = ipCfg->pktSize;
    }
    return 0;

exit:
    return -1;
}

static int setup_ip_socket(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    int sfd;
    int ret;
    struct addrinfo  hints;
    struct addrinfo *addrInfo;
    char hostbuf[NI_MAXHOST];
    char serverbuf[NI_MAXSERV];
    struct ip_mreq mreq;
    struct ipv6_mreq ipv6mreq;

    /* prep up fields for the getaddrinfo() */
    VK_memset(&hints, 0, sizeof(hints));

    /* set socket family */
    (ipCfg->iphVersion == 4) ? (hints.ai_family = AF_INET) : (hints.ai_family = AF_INET6);
    hints.ai_protocol = IPPROTO_UDP;

    /* set socket type */
    /* set it to standard socktype as getaddrinfo doesn't understand our new sock type */
    /* after getaddrinfo() call, we change the type to new Broadcom accelerated socket type */
    hints.ai_socktype = SOCK_DGRAM;
    if ((ret = getaddrinfo(ipCfg->host, ipCfg->service, &hints, &addrInfo)) != 0) {
        VK_printf("getaddrinfo() Failed: error code = %d\n", ret);
        goto exit;
    }

    /* now update the sock type to our new type */
    /* this is a hack until we update the uclibc to understand the new socket type */
    if ( ipCfg->iphVersion == 6 && ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id == 0) {
        VK_printf("IPv6 scope-ID is not set\n");
        ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id = 1;
    }

    VK_printf("IP Address Info:\n"
            "   family       = %2d (PF_INET = %d, PF_INET6 = %d)\n"
            "   socktype     = %2d (SOCK_DRAM = %d, SOCK_STREAM = %d)\n"
            "   protocol     = %2d (IPPROTO_UDP = %d, IPPROTO_TCP %d)\n"
            "   addrlen      = %2d\n"
            "   flags        = %d\n",
            addrInfo->ai_family,
            PF_INET, PF_INET6,
            addrInfo->ai_socktype,
            SOCK_DGRAM,
            SOCK_STREAM,
            addrInfo->ai_protocol,
            IPPROTO_UDP, IPPROTO_TCP,
            addrInfo->ai_addrlen,
            addrInfo->ai_flags);

    getnameinfo(addrInfo->ai_addr, addrInfo->ai_addrlen, hostbuf, sizeof(hostbuf),
                serverbuf, sizeof(serverbuf), NI_NUMERICHOST | NI_NUMERICSERV);

    VK_printf("   IP Addr      = %s\n" "   Port         = %s\n", hostbuf, serverbuf);

    /* Create Datagram socket */
    if ( (sfd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol)) < 0 ) {
        perror("socket():");
		close(sfd);
        goto exit;
    }
    VK_printf("Socket Created: %d\n",sfd);
    ipCtx->sfd = sfd;


    /* Bind to the Receiving IP Address & Port */
    if (bind(sfd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0) {
        perror("bind():");
		close(sfd);
        goto exit;
    }
    VK_printf("Socket bind done\n");
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
    /* increase the receive buffer size for UDP socket, network
           stack will drop pkts at >14Mbps rate */
    {
         int size = 250000;
        if (setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &size, 4) != 0) {
            perror("setsockopt():");
			close(sfd);
            goto exit;
        }
        VK_printf("Increased the Socket RCVBUF size to %d bytes\n", size);
    }
    /* Note on each packet payload offset */
    /* read/recvfrom api also provides flexibility to copy eash packet at a fixed offset */
    /* instead of copying payloads of multiple packets back to back. */
    /* this is mainly useful for RTP processing where individual pkts can be of variable lengths */
    /* and applications wants each packet to begin at a fixed offset */
    /* set the per pkt size, kernel then copies each pkt at fixed offset */

    /* Join the Multicast Group */
    if (ipCfg->iphVersion == 4) {
        if (IN_MULTICAST(ntohl(((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr))) {
            VK_printf("Joining IPv4 Multicast Address Group\n");
            mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr;
            mreq.imr_interface.s_addr = INADDR_ANY;
            if (setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
                perror("setsockopt():");
				close(sfd);
                goto exit;
            }
        }
    }
    else {
        /* IPv6 case */
        if ( IN6_IS_ADDR_MULTICAST(((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr.s6_addr)) {
            VK_printf("Joining IPv6 Multicast Address Group\n");
            ipv6mreq.ipv6mr_multiaddr = ((struct sockaddr_in6 *)(addrInfo->ai_addr))->sin6_addr;
            ipv6mreq.ipv6mr_interface = 0;  /* chooses default interface */
            if (setsockopt(sfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &ipv6mreq, sizeof(ipv6mreq)) < 0) {
                perror("setsockopt():");
				close(sfd);
                goto exit;
            }
        }
    }
    return 0;

exit:
    return -1;
}

static int checkRecvData(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
	int i;
	int ret;
	fd_set rfds;
	struct timeval tv;
	int sfd = ipCtx->sfd;
	FD_ZERO(&rfds);
	FD_SET(sfd, &rfds);
	tv.tv_sec = ipCfg->timeout;
	tv.tv_usec = 0;

	if ( (ret = select(sfd +1, &rfds, NULL, NULL, &tv)) < 0 ) {
	   perror("select(): exiting...");
	   goto exit;
	}

	if (!FD_ISSET(sfd, &rfds)) {
	   VK_printf("**** No Input From Server, EXITING.... \n");
   	   ipCtx->bIsPlay=false;
	   goto exit;
	}

   	ipCtx->bIsPlay=true;
	return 0;

	exit:
	return -1;
}

static int recv_av_data_from_socket(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    int i;
    int ret;
    uint8_t *ptr;
    fd_set rfds;
    struct timeval tv;
    socklen_t saddrLen;
    struct sockaddr *saddr;
    int sfd = ipCtx->sfd;

    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);
    tv.tv_sec = ipCfg->timeout;
    tv.tv_usec = 0;

    if ( (ret = select(sfd +1, &rfds, NULL, NULL, &tv)) < 0 ) {
        perror("select(): exiting...");
        goto exit;
    }

    if (!FD_ISSET(sfd, &rfds)) {
        VK_printf("**** No Input From Server, EXITING.... \n");
   	ipCtx->bIsPlay=false;
        goto exit;
    }

    /* Use recvfrom() to receive the RTP header (if present) + payload */
    saddrLen = NI_MAXHOST;
    saddr = (struct sockaddr *)ipCtx->sockaddr_storage;

    /* need to make multiple recvfrom calls to read multiple packets */
    ptr = ipCtx->buf;
    for (i=0; i < ipCfg->pktsPerRead; i++) {
        if ( (ipCtx->bytesRecv = recvfrom(sfd, ptr, ipCtx->bufSize, 0, saddr, &saddrLen)) < 0) {
            perror("recvfrom():");
            goto exit;
        }
        ipCtx->vector[i].iov_base = ptr;
        ipCtx->vector[i].iov_len = ipCtx->bytesRecv;
        ptr += ipCfg->pktSize;
            /* VK_printf("read %d in 0x%x, ", ipCtx->bytesRecv, vector[i].iov_base);*/
    }
        /*VK_printf("\n");*/

    ipCtx->bIsPlay=true;
    return 0;

exit:
    return -1;
}

static void process_ip_pkts(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    int i;
    int num = 0;
    uint8_t *ptr;
    uint16_t seq =0;
    static uint16_t expectedSeq = 0;
    uint16_t udpLen = 0;
    struct udphdr *udp;
    struct rtphdr *rtp;
    static int firstTime = 1;

    /* Adjust payload pointers & optionally verify RTP Sequence */
    ptr = ipCtx->buf;
    ipCtx->bytesToFeed = 0;
    for (i=0; i< ipCfg->pktsPerRead; i++) {
        num++;
        expectedSeq++;
        /* standard socket recvfrom: kernel returns only one pkt starting at RTP header */
        udpLen = ipCtx->vector[i].iov_len;
        rtp = (rtphdr *) ptr;
        seq = ntohs(rtp->sequenceNum);

        /* adjust the payload pointer */
        ipCtx->vector[i].iov_base = ptr + ipCtx->hdrSize;
        ipCtx->vector[i].iov_len = udpLen - ipCtx->hdrSize;
        ipCtx->bytesToFeed += udpLen - ipCtx->hdrSize;
        if (ipCfg->proto == eProtoRTP) {
            if (!firstTime) {
                if ( seq != expectedSeq ) {
                    VK_printf("********* Break in RTP Seq (expected seq %u, current seq %u, pktnum %d ***********\n", expectedSeq-1, seq, num);
					//printRtp(rtp);
                    expectedSeq = seq;
                    /* goto exit; */
                }
            }
            else {
                expectedSeq = ntohs(rtp->sequenceNum);
                firstTime = 0;
                VK_printf("########## Initial RTP Seq %d, pktsize %d, pktnum %d ###########\n", expectedSeq, udpLen, num);
                //printRtp(rtp);
            }
        } /* if RTP */

        /* next pkt */
        /*TODO ptr += udpLen;*/
        /* */ptr += ipCfg->pktSize;
    }

 	  update_stats(ipCtx, ipCfg);
}

static int record_av_data(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    int nwritten;

    if (ipCfg->proto == eProtoUDP) {
        /* Plain UDP, do a block write */
        if ( (nwritten = write(ipCtx->wfd, ipCtx->buf, ipCfg->pktsPerRead * ipCtx->bytesRecv)) < 0 ) {
            perror("write():");
            goto exit;
        }
        if (nwritten != ipCtx->bytesRecv * ipCfg->pktsPerRead) {
            VK_printf("Write Error: only wrote %u our of %u received bytes\n", nwritten, ipCtx->bytesRecv);
            goto exit;
        }
    }
    else {
        /* RTP + UDP, use vectored write to skip the RTP header */
        if ( (nwritten = writev(ipCtx->wfd, ipCtx->vector, ipCfg->pktsPerRead)) < 0 ) {
            perror("write():");
            goto exit;
        }
    }
    return 0;
exit:
    return -1;
}

static int feed_to_playpump(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg)
{
    unsigned char *playbackBuf;
    size_t playbackBufSize = 0;
    int bytesToFeed = 0;
    int i, feedNow = 0;
    int len;
    static int total =0;

    /*VK_printf("*** need to feed %d bytes from %d pkts\n", ipCtx->bytesToFeed, ipCfg->pktsPerRead);*/
    i=0;
    len = ipCtx->bytesToFeed;
    while (i < ipCfg->pktsPerRead) {
        if (playbackBufSize <=0) {
            if (NEXUS_Playpump_GetBuffer(ipCtx->playpump, (void **)&playbackBuf, &playbackBufSize)) {
                VK_printf("ERROR: failed to get buffer from playpump\n");
                return -1;
            }
            if (playbackBufSize == 0) {
                BKNI_WaitForEvent(ipCtx->event, BKNI_INFINITE);

		VK_printf("ERROR:wait!!!\n");
		//VK_TASK_Sleep(500);

		NEXUS_Playpump_Flush(playpump);
		break;

                //continue;
            }
            /*VK_printf("#### [%d] allocated buffer %p of %d bytes, need to feed %d bytes\n",
                i, playbackBuf, playbackBufSize, len);*/
        }

        /* now copy data into the playback buffer */
        if (ipCtx->vector[i].iov_len < playbackBufSize) {
            /* feed the complete packet */
            VK_memcpy(playbackBuf, ipCtx->vector[i].iov_base, ipCtx->vector[i].iov_len);
            playbackBufSize -= ipCtx->vector[i].iov_len;
            bytesToFeed += ipCtx->vector[i].iov_len;
            playbackBuf += ipCtx->vector[i].iov_len;
            i++;
            if (i == ipCfg->pktsPerRead) {
                feedNow = 1;
            }
        }
        else {
            /* can only feed the partial packet */
            /*VK_printf("^^^^^ [%d] feeding partial buffer, need %d, have %d, base %x\n",
                    i, vector[i].iov_len, playbackBufSize, vector[i].iov_base);*/
            VK_memcpy(playbackBuf, ipCtx->vector[i].iov_base, playbackBufSize);
            bytesToFeed += playbackBufSize;
            ipCtx->vector[i].iov_base = (unsigned char *)ipCtx->vector[i].iov_base + playbackBufSize;
            ipCtx->vector[i].iov_len -= playbackBufSize;
            playbackBufSize = 0;

            /* reached end of playback buffer, so feed it now */
            feedNow = 1;
        }
        if (feedNow) {
            /* feed the data to the playpump */
            if (NEXUS_Playpump_ReadComplete(ipCtx->playpump, 0, bytesToFeed)) {
                VK_printf("ERROR: bplaypump_read_complete() failed\n");
                return -1;
            }
            len -= bytesToFeed;
            total += bytesToFeed;
            /* VK_printf("#####[%d] Fed %d bytes (total %d), remaining %d \n", i, bytesToFeed, total, len);*/
            bytesToFeed = 0;
            feedNow = 0;
        }
    }
    return 0;
}


static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}



static void P_MULTICAST_Task(void *param)
{
	while (1)
	{
        	/* read av data from the socket */
		if (recv_av_data_from_socket(&ipCtxVal, &ipCfgVal))
			break;

        	/* process IP packets: verify RTP Header (optionally decrypt AV payload) */
        	process_ip_pkts(&ipCtxVal, &ipCfgVal);

        	/* now write it to disk either using write or writev*/
        	if (ipCfgVal.writeToDisk)
		{
            		if (record_av_data(&ipCtxVal, &ipCfgVal))
                		break;
        	}

       		if (ipCfgVal.decode)
		{
        		if (feed_to_playpump(&ipCtxVal, &ipCfgVal))
 	          		break;
        	}
    	}

}


static void P_MULTICAST_PlayIpAvStream(void)
{
	NEXUS_Error err;

	DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

	if(ipCfgVal.audioCodec == NEXUS_AudioCodec_eMpeg)
		DRV_AUDIO_AddHdmiOutputPath(0, DI_AUDFORMAT_MPEG);
	else
		DRV_AUDIO_AddHdmiOutputPath(0, DI_AUDFORMAT_AC3);

	err = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
	if(err != NEXUS_SUCCESS)
	{
		VK_printf("video decoding fail \n");
	}

	err = NEXUS_AudioDecoder_Start(hAudioCoder[DI_AUDIO_DECODER_MAIN], &audioProgram);
	if(err != NEXUS_SUCCESS)
	{
		VK_printf("audio decoding fail \n");
	}

	DI_VIDEO_Show(0, TRUE);

}

static void P_MuteOff_AVstream(unsigned long ulTimerId, void *params)
{
	DI_AUDIO_SetMute(DI_AUDIO_DECODER_MAIN, FALSE);
}

static int P_MULTICAST_PlayPumpIp(ExampleIpCtx *ipCtx, ExampleIpCfg *ipCfg,HUINT32 mode)
{
	HUINT32 time;

	DRV_PVR_GetPlaypumpHandle(&playpump);
	BKNI_CreateEvent(&(ipCtx->event));

	NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	stcChannel = DRV_DEMUX_GetStcChannelHandle();

	videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, ipCfg->videopid, NULL);

	NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
	playpumpSettings.dataCallback.callback = play_callback;
	playpumpSettings.dataCallback.context = ipCtx->event;
	NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

	NEXUS_Playpump_Start(playpump);

	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &hAudioCoder[DI_AUDIO_DECODER_MAIN]);

	audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, ipCfg->audiopid, NULL);

	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
	audioProgram.codec = ipCfg->audioCodec;
	audioProgram.pidChannel = audioPidChannel;
	audioProgram.stcChannel = stcChannel;

	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec =  ipCfg->videoCodec;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = stcChannel;

	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
	NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);

	/* Bring up video decoder and display. No audio to keep the example simple. */
	DRV_VIDEO_GetVideoHandle(0,&videoDecoder);

	VK_printf("Playpump is ready to Receive Data...\n");

	/* allocate buffer to receive UDP/RTP Headers + Payload from Network */
	if (allocate_buffers(ipCtx,ipCfg)) {
		VK_printf("ERROR: Failed to Allocate Memory\n");
		goto error;
	}

	/* create the socket to receive AV Data over UDP socket */
	if (setup_ip_socket(ipCtx,ipCfg) != 0) {	//socket error일 경우 -1로 리턴.
	//return값이 -1인경우는 처리 못함. 함수내부에서 error인 경우 전부 close시켜버림.
		VK_printf("ERROR: Failed to Setup the socket\n");
		goto error;
	}

	/* Open the temp file where received content is written to */
	if (ipCfg->writeToDisk && (ipCtx->wfd = open(ipCfg->filename, O_RDWR|O_CREAT|O_TRUNC)) < 0) {
		perror("open():");
		goto error;
	}

	if(checkRecvData(ipCtx, ipCfg))
	{
		if(ipCfgVal.audioCodec == NEXUS_AudioCodec_eMpeg)
		{
			DRV_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_MPEG);
			DRV_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_MPEG);
		}
		else
		{
			DRV_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_AC3);
			DRV_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_AC3);
		}
	}

	if(mode == DISPLAY_HD)
	{
		DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_1080I_50HZ);
	}

	ipCtx->playpump = playpump;
	VK_printf("Socket is ready to Receive Data...\n");

	P_MULTICAST_PlayIpAvStream();

	/* loops forever */
	VK_TASK_Create( P_MULTICAST_Task, USER_PRIORITY6,
							  EXTENDED_TASK_STACK_SIZE,
							  "P_MULTICAST_Task",
							  (void *)ulTaskId,
							  &s_ulStreamTaskId,
							  FALSE);

	VK_TASK_Start(s_ulStreamTaskId);
	return 0;

error:
	close(ipCtx->sfd);
	ipCtx->sfd = -1;
	return -1;
}

DRV_Error DRV_MULTICAST_GetStatus(void)
{
	if(ipCtxVal.bIsPlay == true)
		return DRV_OK;
	else
		return DRV_ERR;
}

void DRV_MULTICAST_Stop(void)
{
	if(nSyncChannelHandle != NULL)
	{
		nSyncChannelSettings.videoInput = NULL;
		nSyncChannelSettings.audioInput[0] = NULL;
		nSyncChannelSettings.audioInput[1] = NULL;
		NEXUS_SyncChannel_SetSettings(nSyncChannelHandle, &nSyncChannelSettings);
	}

	if(hAudioCoder[DI_AUDIO_DECODER_MAIN] != NULL)
	{
		NEXUS_AudioDecoder_Flush(hAudioCoder[DI_AUDIO_DECODER_MAIN]);
		NEXUS_AudioDecoder_Stop(hAudioCoder[DI_AUDIO_DECODER_MAIN]);
		NEXUS_VideoDecoder_Flush(videoDecoder);
		NEXUS_VideoDecoder_Stop(videoDecoder);
	}

	if(s_ulStreamTaskId !=0)
		VK_TASK_Destroy(s_ulStreamTaskId);

	if(playpump != NULL)
	{
		NEXUS_Playpump_Flush(playpump);
		NEXUS_Playpump_Stop(playpump);
		DRV_PVR_ReleasePlaypumpHandle(playpump);
	}
	if(ipCtxVal.sfd >= 0)
	{
		close(ipCtxVal.sfd);
		ipCtxVal.sfd = -1;
	}

	if(ipCfgVal.audioCodec == NEXUS_AudioCodec_eMpeg)
	{
		DRV_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_MPEG);
		DRV_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_MPEG);
	}
	else
	{
		DRV_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_AC3);
		DRV_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_AC3);
	}

        ipCtxVal.bIsPlay=false;
}


DRV_Error DRV_MULTICAST_Start(HUINT32 type, HUINT32 vPid, HUINT32 aPid, DI_VIDEO_FORMAT eVideoCodec, DI_AUDIO_FORMAT eAudioCodec)
{
	int ret;
	DI_ERR_CODE diResult;
	NEXUS_VideoCodec vCodec;
	NEXUS_AudioCodec aCodec;

	vCodec = DRV_VIDEO_GetCodecType(0, eVideoCodec);
	aCodec = DRV_AUDIO_ConvertCodecToNexus(eAudioCodec);

	diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
	if(diResult != DI_ERR_OK)
	{
		VK_printf("DI_AUDIO_SetSpdifOutput failed!\n");
	}

	diResult = DRV_AUDIO_AddSpdifOutputPath(DI_AUDIO_DECODER_MAIN, eAudioCodec);
	if(diResult != DI_ERR_OK)
	{
		VK_printf("DRV_AUDIO_AddSpdifOutputPath fail !!\n");
	}

	set_default_ip_cfg(&ipCtxVal, &ipCfgVal);

	ipCfgVal.videopid = vPid;
	ipCfgVal.audiopid = aPid;
	ipCfgVal.videoCodec = (int)vCodec;
	ipCfgVal.audioCodec = (int)aCodec;
	ipCfgVal.service = "1234";

#if defined(CERTI_AGING_TEST)
	ipCfgVal.service = "11111";
	if(type == DISPLA_SD)	// SD stream A/V play
	{
		ipCfgVal.videopid = 163;
		ipCfgVal.audiopid = 92;
		ipCfgVal.videoCodec = (int)NEXUS_VideoCodec_eMpeg2;
		ipCfgVal.audioCodec = (int)NEXUS_AudioCodec_eMpeg;

		ipCfgVal.host = "239.1.1.44";
	}
	else if(type == DISPLAY_HD) // HD stream A/V play:
	{
		ipCfgVal.videopid = 1535;
		ipCfgVal.audiopid = 1539;
		ipCfgVal.videoCodec = (int)NEXUS_VideoCodec_eH264;
		ipCfgVal.audioCodec = (int)NEXUS_AudioCodec_eAc3;

		ipCfgVal.host = "239.1.0.10";
	}
#else
	if(type == DISPLA_SD) 	// SD stream A/V play
	{
		// service: Satellite(1084M)&Cable(506M)-ColorBar.ts
		ipCfgVal.host = "239.0.0.3";
	}
	else if(type == DISPLAY_HD) // HD stream A/V play:
	{
		// service: Satellite(1170MHz)_Cable(490MHz)_HD-PremiereHD.ts
		ipCfgVal.host = "239.0.0.4";
	}
	else if(type == DISPLAY_COLOR_BAR)
	{
		ipCfgVal.host = "239.0.0.5";
	}
#endif
	ipCtxVal.sfd = -1;
        ipCtxVal.bIsPlay=false;

	ret = P_MULTICAST_PlayPumpIp(&ipCtxVal, &ipCfgVal,type);

	return ret;

}




