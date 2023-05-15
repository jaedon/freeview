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
 * $brcm_Workfile: rplay.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/rplay/rplay.c $
 * 
 * 3   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <pthread.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

#ifdef __vxworks
#include <sockLib.h>
#endif

#include "rplay.h"

BDBG_MODULE(rplay);


struct context {
	bplaypump_t play;
	BKNI_EventHandle event;
	int port;
	int tcp;
	int sock;
	off_t offset;
	unsigned chunk; 
	unsigned seq;
	struct sockaddr_in server;
} g_context0, g_context1;

static void read_callback(void *context);
static void *play_func(void *context);

static void prompt(const char *msg) 
{
	printf("\n%s\n", msg);
	getchar();
}

int main(void) 
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display);
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display);
	bplaypump_params playpump_params;
	bstream_mpeg mpeg;
	bstream_t stream0, stream1;
	pthread_t thread0, thread1;

	if (rc) goto error;

	BKNI_CreateEvent(&g_context0.event);

	g_context0.play = bplaypump_open(B_ID(0), NULL);
	g_context0.port = 1035;
	g_context0.tcp = 0;
	g_context0.offset = 0;

	BKNI_CreateEvent(&g_context1.event);
	g_context1.play = bplaypump_open(B_ID(1), NULL);
	g_context1.port = 1036;
	g_context1.tcp = 0;
	g_context1.offset = 0;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x31;
	mpeg.audio[0].pid = 0x34;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x31;

	bplaypump_params_init(&playpump_params, g_context0.play);
	playpump_params.read_callback = read_callback;
	playpump_params.callback_context = g_context0.event;

	stream0 = bplaypump_start(g_context0.play, &mpeg, &playpump_params);
	if (!stream0) goto error;

	if (bdecode_start(decode0, stream0, window0))
		goto error;

	if (pthread_create(&thread0, NULL, play_func, &g_context0))
		goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	bplaypump_params_init(&playpump_params, g_context1.play);
	playpump_params.read_callback = read_callback;
	playpump_params.callback_context = g_context1.event;

	stream1 = bplaypump_start(g_context1.play, &mpeg, &playpump_params);
	if (!stream1) goto error;

	if (bdecode_start(decode1, stream1, window1))
		goto error;

	if (pthread_create(&thread1, NULL, play_func, &g_context1))
		goto error;

	prompt("press enter to enter slow motion");
	bplaypump_set_decoder_rate(g_context0.play, BPLAYPUMP_NORMALPLAY_SPEED/2);
	prompt("press enter to pause");
	bplaypump_set_decoder_rate(g_context0.play, 0);
	prompt("press enter to resume normal play");
	bplaypump_set_decoder_rate(g_context0.play, BPLAYPUMP_NORMALPLAY_SPEED);
	prompt("press enter to stop");

	bplaypump_stop(g_context0.play);
	BKNI_SetEvent(g_context0.event);
	pthread_join(thread0, NULL);

	bplaypump_stop(g_context1.play);
	BKNI_SetEvent(g_context1.event);
	pthread_join(thread1, NULL);

	return 0;
error:
	return 1;
}

void read_callback(void *context)
{
	BDBG_MSG(("calbback"));
	/* We can't do the read and read_complete in the callback, so notify the thread */
	BKNI_SetEvent((BKNI_EventHandle)context);
}

struct rbuf {
	struct rplay_header header;
	uint8_t buf[RPLAY_PAYLOAD];
};

void *play_func(void *context_)
{
	struct context *t = (struct context *)context_;
	void *buffer;
	size_t buffer_size;
	struct sockaddr_in saddr;
	struct iovec io[2];
	int rc;
	unsigned cur_off;
	uint8_t *ptr;
	struct rbuf buf;
	struct rplay_read_cmd *read_cmd;
	int on=1;
	socklen_t socklen;
	unsigned nheart;
	int eof;

	t->offset = 0;
	t->chunk = 1;

	t->sock = socket(PF_INET , t->tcp ? SOCK_STREAM:SOCK_DGRAM, t->tcp?IPPROTO_TCP:IPPROTO_UDP);
	if (t->sock<0) {
		perror("socket");
		exit(1);
	}
	memset(&saddr, 0, sizeof saddr);
	saddr.sin_port = htons(t->port);
	saddr.sin_family = AF_INET;
	BDBG_MSG(("connecting..."));
	saddr.sin_addr.s_addr = INADDR_ANY ;
	rc = bind(t->sock, (struct sockaddr *)&saddr, sizeof saddr);
	if (rc!=0) {
		perror("bind");
		exit(1);
	}
	io[0].iov_base = &buf.header;
	io[0].iov_len = sizeof(buf.header);
	rc = setsockopt(t->sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	if (rc!=0) {
		perror("setsockopt");
		exit(1);
	}
	if (t->tcp) {
		rc = listen(t->sock, 2);
		if (rc!=0) {
			perror("listen");
			exit(1);
		}
		t->sock = accept(t->sock, NULL, 0);
		if (t->sock<0) {
			perror("accept");
			exit(1);
		}
	} else {
		socklen = sizeof t->server;
		BDBG_MSG(("recvfrom+"));
      	rc = recvfrom(t->sock, (caddr_t)&buf, sizeof(buf), 0, (struct sockaddr *)&t->server, &socklen);
		if (rc<0) {
			perror("recvfrom");
			exit(1);
		}
		BDBG_MSG(("recvfrom-"));
#if 0
		send_sock = connect(t->sock, (struct sockaddr *)&saddr, socklen);
		if (send_sock<0) {
			perror("connect");
			exit(1);
		}
#endif
	}
	BDBG_WRN(("Playing..."));

	while (1) {
		if (bplaypump_get_buffer(t->play, &buffer, &buffer_size)) {
			BDBG_ERR(("internal error"));
			break;
		}
		if (!buffer_size) {
			BDBG_MSG(("waiting"));
			BKNI_WaitForEvent(t->event, BKNI_INFINITE);
			continue;
		}

		if (buffer_size > RPLAY_CHUNK_SIZE) {
			buffer_size = RPLAY_CHUNK_SIZE;
		}
		buf.header.chunk = RPLAY_CHUNK_CMD;
		buf.header.offset = RPLAY_CMD_READ;
		read_cmd = (void *)buf.buf;
		read_cmd->chunk = t->chunk;
		read_cmd->offset = t->offset;
		read_cmd->size = buffer_size;
		BDBG_MSG(("sending request"));
		rc = sendto(t->sock, (caddr_t)&buf, sizeof(buf.header) + sizeof(*read_cmd), 0, (struct sockaddr *)&t->server, sizeof t->server);
		if (rc<0) {
			perror("write");
			exit(1);
		}
		t->seq = 0;
		

		for(eof=0,nheart=0,ptr=buffer,cur_off=0;cur_off < buffer_size;) {
next_packet:
			if ((buffer_size-cur_off)>= RPLAY_PAYLOAD) {
				io[1].iov_base = ptr+cur_off;
				io[1].iov_len = RPLAY_PAYLOAD;
			} else {
				io[1].iov_base = buf.buf;
				io[1].iov_len = sizeof buf.buf;
			}
			rc = readv(t->sock, io, 2);
			if (rc==0) {
				BDBG_ERR(("server has died"));
				exit(1);
			}
			if (rc<0) {
				perror("readv");
				exit(1);
			}
			if (rc<(int)(sizeof buf.header)) {
				BDBG_ERR(("invalid packet len=%d\n", rc));
				exit(1);
			}
			if (buf.header.chunk==RPLAY_CHUNK_CMD) {
				switch(buf.header.offset) {
				case RPLAY_CMD_HEARTBEAT:
					nheart++;
					BDBG_MSG(("recv: hearbeat %d %d", buf.header.seq, nheart));
					if (nheart>3) {
						BDBG_WRN(("recv: hearbeat %d %d", buf.header.seq, nheart));
						goto done_recv;
					}
					break;
				case RPLAY_CMD_EOF:
					BDBG_WRN(("recv: eof"));
					eof=1;
					t->offset = 0;
					goto done_recv;
				default:
					BDBG_ERR(("recv: unknown cmd %d",  buf.header.offset));
				}
			} else if (buf.header.chunk == t->chunk) {
				nheart = 0;
				rc -= sizeof buf.header;
				if (io[1].iov_base != ptr+buf.header.offset) {
					memcpy(ptr+buf.header.offset, io[1].iov_base, rc);
				}
				if (buf.header.offset == cur_off) {
					cur_off += rc;
				} else if (buf.header.seq > t->seq) {
					BDBG_WRN(("missed packet %d %d", buf.header.offset, cur_off));
					cur_off += buf.header.offset + rc;
				} else {
					BDBG_WRN(("old sequence %d %d", buf.header.seq, t->seq));
					goto next_packet;
				}
				t->seq = buf.header.seq+1;
		   } else {
				BDBG_WRN(("unknown chunk %d %d", buf.header.chunk, t->chunk));
		   }
		}
done_recv:
		t->offset += cur_off;
		t->chunk ++;
		if (t->chunk > RPLAY_MAX_CHUNK) {
			t->chunk = 1;
		}
		if (cur_off == 0) {
			if (eof) {
				BDBG_MSG(("flushing"));
				BKNI_Sleep(200); /* This is a simplistic way of making sure the MVD buffer
					is empty before flushing. */
				bplaypump_flush(t->play);
			}
}
		else {
			if (bplaypump_read_complete(t->play, 0, cur_off))
				break;
		}
	}
	return NULL;
}

