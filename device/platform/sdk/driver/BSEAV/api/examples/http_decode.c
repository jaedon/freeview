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
 * $brcm_Workfile: http_decode.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/http_decode.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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

/* Test App to demonstrate receiving content in HTTP payload from a remote server and decodes & displays it locally using */
/* Settop API Playpump Module */
/* It supports read from standard STREAM socket type or the Broadcom's accelerated socket interface (option -a) */

BDBG_MODULE(http_decode);
typedef struct
{
	bdecode_t decode;
	bdecode_status decode_status;
	bdisplay_t display;
	bdecode_window_t window;
	bplaypump_params playpump_params;
	bplaypump_open_params playpump_open_params;
	bstream_mpeg mpeg;
	bstream_t stream;
	BKNI_EventHandle event;
    bplaypump_t pump; /* Settop API Playpump handle */
    void * buffer; /* playpump buffer */
    size_t buffer_size; /* playpump buffer size */
	int sd;	/* socket descriptor */
	int settopInit;

} playback_ip_t;

playback_ip_t playback_ip; /* Main control structure */
int gQuit = 0;
#define MAX_BUFFER_SIZE (1316 * 64 * 2)
#define	MAX_RBUF_SIZE 1600

void usage()
{
    printf("Usage: http_decode [-a] [-s<num>] [-b] [-t <num>] [-h] IP Port [filename to receive] [vpid] [vtype] [apid] [atype]\n");
    printf("options are:\n");
    printf(" -a         # use accelerated socket type, default is regular socket\n");
    printf(" -s <num>   # size of MPEG2 TS packets, default 188 bytes\n");
    printf(" -b         # run with Brutus App, default is w/o Brutus\n");
    printf(" -t <num>   # select timeout value in seconds, default is 20sec\n");
    printf(" -h         # prints netrecv usage\n");
    printf("\n");
}

void sigHandler(int signal)
{
    printf("**** Caught Signal # %d\n", signal);
	gQuit = 1;
    if (playback_ip.settopInit != 0)
    {
        printf("sighandler(): calling bsettop_uninit\n");
        playback_ip.settopInit = 0;
        close(playback_ip.sd);
        bsettop_uninit();
    }
    exit(-1);
}

void play_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int settop_init(playback_ip_t *playback_ip, int vpid, bvideo_codec vtype, int apid, baudio_format atype, int pcrpid, int tsSize) 
{
	bresult rc;
	rc = bsettop_init(BSETTOP_VERSION);
	playback_ip->decode = bdecode_open(B_ID(0));
	playback_ip->display = bdisplay_open(B_ID(0));
	playback_ip->window = bdecode_window_open(B_ID(0), playback_ip->display);
	if (rc) goto error;
	
	BKNI_CreateEvent(&playback_ip->event);

    /* increase the buffering for IP STB to accomodate adequate "pre-charge" */
    bplaypump_get_open_params(B_ID(0), &playback_ip->playpump_open_params);
    playback_ip->playpump_open_params.buffer_size *= 2;
    playback_ip->playpump_open_params.num_descriptors *= 2;
    playback_ip->pump = bplaypump_open(B_ID(0), &playback_ip->playpump_open_params);

    printf("Stream Info: vpid 0x%x, vtype 0x%x, apid 0x%x, atype 0x%x, pcr pid 0x%x, tsSize\n",
            vpid, vtype, apid, atype, pcrpid, tsSize);
	bstream_mpeg_init(&playback_ip->mpeg);
	playback_ip->mpeg.video[0].pid = vpid;
	playback_ip->mpeg.video[0].format = vtype;
	playback_ip->mpeg.audio[0].pid = apid;
	playback_ip->mpeg.audio[0].format = atype;
	playback_ip->mpeg.pcr_pid = pcrpid;

	bplaypump_params_init(&playback_ip->playpump_params, playback_ip->pump);
	playback_ip->playpump_params.read_callback = play_callback;
	playback_ip->playpump_params.callback_context = playback_ip->event;
	if (tsSize == 192)
		playback_ip->playpump_params.timestamp_enabled = true;

	playback_ip->stream = bplaypump_start(playback_ip->pump, &playback_ip->mpeg, &playback_ip->playpump_params);
	if (!playback_ip->stream) goto error;

	if (bdecode_start(playback_ip->decode, playback_ip->stream, playback_ip->window))
		goto error;

    /* Catch signals to do proper Settop API un-initialize */
    signal(SIGHUP, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGQUIT, sigHandler);
    signal(SIGSEGV, sigHandler);
    signal(SIGPIPE, sigHandler);

    return 0;

error:
	return 1;
}

/* builds HTTP get request */
void http_build_get_req(char *write_buf, char *url, char *server)
{
	sprintf(write_buf,
			"GET %s HTTP/1.1\r\n"
			"User-Agent: BRCM 97401\r\n"
			"Host: %s\r\n"
			"Connection: Keep-Alive\r\n"
			"PlaySpeed.dlna.org: speed=%d\r\n"
			"TimeSeekRange.dlna.org: npt=%.3f\r\n"
			"\r\n", 
			url, server, 1, 0.);
}

/* Read & Parse Server's HTTP Response */
int http_read_response(char **http_hdr, char **http_payload, int fd, char *rbuf)
{
	int n;
	int ret;
	char tmp;
	char *p;
	fd_set rfds;
	struct timeval 	socket_timeout;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	socket_timeout.tv_sec = 10;
	socket_timeout.tv_usec = 0;
	ret = select(fd+1, &rfds, NULL, NULL, &socket_timeout);
	if (ret == 0) {
		BDBG_WRN(("Receive timeout"));
		return -1;
	} else if (ret<0 && errno != EINTR) {
		BDBG_ERR(("select error"));
		return -1;
	}

	if (!FD_ISSET(fd, &rfds))
		/* No response from server */
		return -1;

	memset(rbuf, 0, MAX_RBUF_SIZE);
	if ((n = read(fd, rbuf, MAX_RBUF_SIZE)) <= 0) {
		perror("read\n");
		return -1;
	}
	p = strstr(rbuf, "\r\n\r\n");
	if(!p) {
		printf("No HTTP Header: increase the read size\n");
		return -1;
	}
	
	*http_hdr = rbuf;
	*http_payload = p + 4;
	rbuf[n] = 0;

	/* print HTTP response header */
	tmp = **http_payload;
	**http_payload = 0;
	BDBG_ERR(("HTTP Response:\n%s", *http_hdr));
	**http_payload = tmp;

	/* Now look for the <300 HTTP response codes */
	if (strstr(rbuf, "HTTP/1.1 4") || strstr(rbuf, "HTTP/1.0 4")) {
		BDBG_ERR(("Incorrect HTTP response: %s\n", rbuf));
		return -1;
	}

	return (n);
}

int http_set_brcm_sock_params(int sd)
{
	STRM_SockRecvParams_t sockRecvParams;
	/* now that we have read the HTTP response, increase the # of pkts per read */
    sockRecvParams.pktsPerRecv = 64;
    sockRecvParams.pktOffset = 0;
    if (setsockopt(sd, SOCK_BRCM_STREAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
    {
    	BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
		/* in case of failure (shouldn't happen), read 1 pkt at a time */
	}
	BDBG_MSG(("Modified the default pkts per recvfrom to 64\n"));
}

/* Create TCP connection to server, send HTTP Req and return the socket descriptor */
int http_send_get_req(char *server, int port, char *http_req, int doSockAccel)
{
	int sd, rc = 0;
	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;
	int rcv_buf_size;   

	printf("TCP - Connection to %s:%d ...\n",server, port);

	h = gethostbyname(server);
	if (h == NULL) {
		BDBG_ERR(("Unknown Host"));
		return -EINVAL;
	}

	servAddr.sin_family = h->h_addrtype;
	memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0],h->h_length);
	servAddr.sin_port = htons(port);

    if (!doSockAccel) 
		sd = socket(servAddr.sin_family, SOCK_STREAM, 0);
	else
		sd = socket(servAddr.sin_family, SOCK_BRCM_STREAM, 0);

	if (sd<0) {
		/* Socket Create Error */
		BDBG_ERR(("Socket Open Err"));
		return -EINVAL;
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	if (bind(sd, (struct sockaddr *)&localAddr, sizeof(localAddr))) {
		BDBG_ERR(("Socket Bind Err"));
		return -EINVAL;
	}

	/* Connect to server */
	rc = connect(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));
	if (rc<0) {
		BDBG_ERR(("Connect Error: Server busy or unavailable"));
		return -EBUSY;
	}

	if (!doSockAccel) {
		/* increase the receive window size */
		rcv_buf_size = 500000;   
		if (setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (const char*)&rcv_buf_size, sizeof(rcv_buf_size) ) < 0 ) {
			BDBG_ERR(("SO_RCVBUF Socket Error\n"));
			return -EINVAL;
		}
	}

	printf("TCP: Connection Success\n");

	/* Now send the GET request to server */
    if (doSockAccel) 
		b_cacheflush(http_req, strlen(http_req));	/* need to cache flush as Eth DMA engine is used to copy the user buffer */

	if ( write(sd, http_req, strlen(http_req)) < 0) {
		BDBG_ERR(("HTTP Write Error\n"));
		rc = -EBUSY;
		goto error;
	}

	BDBG_ERR(("Get Request: \n%s\n", http_req));
	BKNI_Sleep(50);
	
	return sd;
error:
	return rc;
}

/* Get Buffer of requested "size" from Playpump */
int bplayback_ip_get_playbump_buffer(playback_ip_t *playback_ip, unsigned int size)
{
    for(;;) {
        if (bplaypump_get_buffer(playback_ip->pump, &playback_ip->buffer, &playback_ip->buffer_size)) {
            BDBG_ERR(("Returned error from bplaypump_get_buffer()!"));
            goto error;
        }

        if (playback_ip->buffer_size == 0) {
            BDBG_ERR(("No Free Playpump Buffer, sleeping"));
            /*bplaypump_flush(playback_ip->pump); */
            BKNI_Sleep(20);
            continue;
        }
        else if(playback_ip->buffer_size >= size) {
            /* constrain the amount of data we're going to read from the socket */
            playback_ip->buffer_size = size;
        	BDBG_MSG(("Got %d size buffer from the playpump\n", size));
            break;
        }
        BDBG_MSG(("skip buffer %d\n", playback_ip->buffer_size));
        /* release this buffer, it's too small and try to get buffer again*/
        if(bplaypump_read_complete(playback_ip->pump, playback_ip->buffer_size, 0)) {
            BDBG_ERR(("Returned error from bplaypump_read_complete()!"));
            BKNI_Sleep(20);
            continue;
        }
    }
    return (playback_ip->buffer_size);
error:
    return -1;
}

/* Read data from socket */
int http_socket_read(playback_ip_t *playback_ip)
{
	int rc;
	int bytesRead;
	fd_set rfds;
	struct timeval tv;

retry_select:
	FD_ZERO(&rfds);
	FD_SET(playback_ip->sd, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	if ( (rc = select(playback_ip->sd+1, &rfds, NULL, NULL, &tv)) < 0 ) {
		if (rc == -EINTR)
			goto retry_select;
		BDBG_ERR(("ERROR: select(): exiting..."));
		goto error;
	}

	if ( rc == 0 || !FD_ISSET(playback_ip->sd, &rfds)) {
		/* No Input From Server, go back to the top of the loop */
		goto retry_select;
	}

	/* Now that we have data available on the socket, read it into the playpump buffer */
	if ( (bytesRead = read(playback_ip->sd, playback_ip->buffer, playback_ip->buffer_size)) < 0 ) {
		if (bytesRead == -EINTR)
			goto retry_select;
		perror("%s: read ERROR:");
		goto error;
	}
	else if (bytesRead == 0) {
		BDBG_ERR(("Server Closed the connection, we are done!"));
		goto error;
	}
	return bytesRead;

error:
	return -1;
}

/* Wait until AV CDB have enough space for new AV data */
int http_wait_av_decoder_buffer(playback_ip_t *playback_ip)
{
	int i;
	bstream_status stream_status;
	memset(&stream_status, 0, sizeof(stream_status));

	if (bstream_get_mpeg_parameters(playback_ip->stream, &stream_status)) {
		BDBG_ERR(("bstream_get_mpeg_parameters() returned error!"));
		return -1;
	}
	
    if(stream_status.decode == NULL) {
		BDBG_ERR(("Decoder is not ready\n"));
		return -1;
	}

	for(;;) {
		if (bdecode_get_status(stream_status.decode, &playback_ip->decode_status)) {
			BDBG_ERR(("Returned error from bdecode_get_status()!"));
			return -1;
		}

		if ((playback_ip->decode_status.video_fifo_size - playback_ip->decode_status.video_fifo_depth < (1024 * 512)) ||
			(playback_ip->decode_status.audio_fifo_size - playback_ip->decode_status.audio_fifo_depth < (1024 * 20))) {
			BDBG_MSG(("******************* CDB getting Full, video depth %lu, size %lu, audio depth %lu, size %lu\n", 
				playback_ip->decode_status.video_fifo_depth, 
				playback_ip->decode_status.video_fifo_size, 
				playback_ip->decode_status.audio_fifo_depth,
				playback_ip->decode_status.audio_fifo_size));
			BKNI_Sleep(30);
			continue;
		}

		/* decoder has sufficient space, return success */
		break;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char *rbuf = NULL;		/* temp buffer to read HTTP response */
	int rbuf_len;			/* total length of the HTTP response */
	char *http_req = NULL;	/* HTTP req buffer */
	char *http_resp_hdr = NULL;	/* Pointer to the http_resp_hdr */
	char *http_payload = NULL; /* Pointer to the http payload */
	int http_payload_len;	/* HTTP payload that is received during the HTTP response read call */
	int bytesRead;			/* Bytes read from socket */
	char *server; 			/* Server to Connect to */
	unsigned short port;	/* Server Port # */
	char *url;				/* URL to playback */
	int c;					/* temp char */
	int doSockAccel = 0; 	/* Use Accelerated Sockets */
	int timeout = 20; 		/* timoout */
	int tsSize = 188;		/* Transport payload size: not yet used */
	int vpid, apid, pcrpid; /* Video, Audio, PCR pids */
	bvideo_codec vtype;		/* Video codec type */
	baudio_format atype;	/* Audio codec type */
	STRM_SockRecvParams_t sockRecvParams; /* Structure to set Socket Recv options */
	bresult rc;				/* Setttop API return code */
	int brutus=0;			/* Set if this app is run along side brutus application */


	memset(&playback_ip, 0, sizeof(playback_ip));
    while ((c = getopt(argc, argv, "s:t:ahb")) != -1)
    {
        switch (c) 
        {
        case 's':
            tsSize = atoi(optarg);
            if (tsSize != 188 && tsSize != 192 && tsSize != 130)
            {
                printf("Not supported transport Size (%d)\n", tsSize);
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
        case 8:   /* server ip, port, url, vidpid, vtype, audio pid, atype are specified on the command line. */
        {
            server = argv[optind];
            port = atoi(argv[optind + 1]);
            url = argv[optind + 2];
            vpid = atoi(argv[optind+3]);
            vtype = atoi(argv[optind+4]);
            apid = atoi(argv[optind+5]);
            atype = atoi(argv[optind+6]);
            pcrpid = atoi(argv[optind+7]);
            break;
        }
        case 3:   /* Both server, port & url are specified on the command line. */
        {
            server = argv[optind];
            port = atoi(argv[optind + 1]);
            url = argv[optind + 2];
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
	printf("Server %s, Port %d\n", server, port);

    /* initialize Settop APIs */
    if (!brutus)
    {
        if (settop_init(&playback_ip, vpid, vtype, apid, atype, pcrpid, tsSize))
            goto error;
        printf("Settop APIs & BCM Driver is initialized\n");
        playback_ip.settopInit = 1;
    }

	/* prepare initial HTTP Get request */
	http_req = (char *) BKNI_Malloc(512);
	rbuf = (char *) BKNI_Malloc(MAX_RBUF_SIZE);
	if (!rbuf || !http_req)
		goto error;

	memset(http_req, 0, 512);
	http_build_get_req(http_req, url, server);

	/* setup the socket connection to the server & send GET request */
	if ( (playback_ip.sd = http_send_get_req(server, port, http_req, doSockAccel)) < 0) {
		printf("Failed to Send HTTP Get request to Server\n");
		goto error;
	}

	if ( (rbuf_len = http_read_response(&http_resp_hdr, &http_payload, playback_ip.sd, rbuf)) < 0 ) {
		printf("Failed to Receive correct HTTP response\n");
		goto error;
	}

	/* It is possible to receive some payload along with HTTP response */
	/* Feed this payload separately to the playpump */
	http_payload_len = rbuf_len - (http_payload - http_resp_hdr);
	if (http_payload_len) {
		if (bplayback_ip_get_playbump_buffer(&playback_ip, http_payload_len) < 0)
			goto error;
		/* now copy the initial payload into the playpump buffer */
		memcpy(playback_ip.buffer, http_payload, http_payload_len);
write_again:
        BDBG_ERR(("Feeding initial payload of len %d to Playpump\n", http_payload_len));
        if(bplaypump_read_complete(playback_ip.pump, 0, http_payload_len) ) {
            BDBG_ERR(("Returned error from bplaypump_read_complete(), retrying in 20sec!"));
            BKNI_Sleep(20);
            goto write_again;
        }
	}

	if (doSockAccel) 
		http_set_brcm_sock_params(playback_ip.sd);

	while (!gQuit) {
		/* wait until enough space is available in Audio & Video Buffers */
		if (http_wait_av_decoder_buffer(&playback_ip) < 0)
			goto error;

		/* get an adequately sized buffer from the playpump */
		if (bplayback_ip_get_playbump_buffer(&playback_ip, MAX_BUFFER_SIZE) < 0)
			goto error;

        /* wait so that data can be accumulated at socket buffer, thus fewer read calls need to be made */
        if (!doSockAccel) 
			BKNI_Sleep(30);

		/* read data from socket */
		if ( (bytesRead = http_socket_read(&playback_ip)) < 0) {
			BDBG_ERR(("Failed to receive data from Server, errno %d\n", bytesRead));
			goto error;
		}
		
		/* now feed this buffer to playpump */
		if (bplaypump_read_complete(playback_ip.pump, 0, bytesRead) != b_ok) {
            BDBG_ERR(("bplaypump_read_complete() returned error!"));
            continue;
        }
	}

error:
    close(playback_ip.sd);
    if (!brutus && playback_ip.settopInit) {
		/*bplaypump_flush(play);*/
		bsettop_uninit();
    }
    playback_ip.settopInit = 0;
	if (rbuf) BKNI_Free(rbuf);
	if (http_req) BKNI_Free(http_req);
    return 0;
}
