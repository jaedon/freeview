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
 * $brcm_Workfile: rsend.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/rplay/rsend.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/poll.h>

#include "rplay.h"

struct rbuf {
	struct rplay_header header;
	char buf[RPLAY_PAYLOAD];
};

int 
main(int argc, const char *argv[])
{
	int sock;
	int tcp=1;
	int port=1035;
	const char *addr;
	const char *fname;
	int rc;
	struct sockaddr_in saddr;
	unsigned transfered;
	int fd;
	struct hostent *host;
	char buf[RPLAY_CHUNK_SIZE];
	struct pollfd rpoll;
	unsigned seq=0;
	unsigned heart_seq=0;
	struct rbuf rbuf;
	

	if (argc<4) {
usage:		
		printf("usage: %s <tcp/udp> port file host\n", argv[0]);
		return 0;
	}
	if (!strcmp(argv[1],"tcp")) { 
		tcp = 1;
	} else if (!strcmp(argv[1],"udp")) { 
		tcp = 0;
	}  else {
		goto usage;
	}
	port = strtol(argv[2], NULL, 0);
	if (port<=0) {
		printf("invalid port %s\n", argv[3]);
		goto usage;
	}
	printf("unsing port %d\n", port);
	fname = argv[3];
	fd = open(fname, O_RDONLY);
	if (fd<0) {
		perror(fname);
	}
	printf("using file %s\n", fname);
	addr = argv[4];
	memset(&saddr, 0, sizeof saddr);
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	printf("Connecting...\n");
	host = gethostbyname(addr);
	if (!host) {
		perror(addr);
	    return 1;
	}
	saddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);
	sock = socket(PF_INET , tcp ? SOCK_STREAM:SOCK_DGRAM, tcp?IPPROTO_TCP:IPPROTO_UDP);
	if (!sock) {
		perror("socket");
		return 1;
	}
	rc = connect(sock, (struct sockaddr *)&saddr, sizeof saddr);
	if (rc!=0) {
		perror("connect");
		return 1;
	}
	printf("Serving...\n");
	for(;;) {
		rpoll.events =  POLLIN;
		rpoll.revents = 0;
		rpoll.fd = sock;
		rc = poll(&rpoll, 1, 100);
		if (rc<0) {
			perror("poll");
			return 1;
		}
		if (rc==0) {
			rbuf.header.chunk = RPLAY_CHUNK_CMD;
			rbuf.header.offset = RPLAY_CMD_HEARTBEAT;
			rbuf.header.seq = heart_seq++;
			rc = write(sock, &rbuf.header, sizeof rbuf.header);
			if (rc<0) {
				perror("write");
				return 1;
			}
			continue;
		}
		rc = read(sock, &rbuf, sizeof(rbuf));
		if (rc==0) {
			printf("player has died\n");
			return 1;
		}
		if (rc<0) {
			perror("read");
			return 1;
		}
		if (rc < (int)sizeof rbuf.header) {
			printf("invalid packet length %d\n", rc);
			return 1;
		}
		if (rbuf.header.chunk == RPLAY_CHUNK_CMD && rbuf.header.offset == RPLAY_CMD_READ) {
			struct rplay_read_cmd *read_cmd = (void *)rbuf.buf;
			off_t off;
			unsigned cur_off;
			unsigned size;
			unsigned chunk;
			struct iovec io[2];

			printf("fetching... %d %d %d\n", fd, (int)read_cmd->offset, (int)read_cmd->size);
			off = lseek(fd, (int)read_cmd->offset, SEEK_SET);
			if (off<0) {
				perror("lseek");
				return 1;
			}
			rc = read(fd, buf, read_cmd->size);
			if (rc<0) {
				perror("read");
				return 1;
			}
			if (rc==0) {
				rbuf.header.chunk = RPLAY_CHUNK_CMD;
				rbuf.header.offset = RPLAY_CMD_EOF;
				rbuf.header.seq = seq++;
				rc = write(sock, &rbuf.header, sizeof rbuf.header);
				if (rc<0) {
					perror("write");
					return 1;
				}
				continue;
			}
			size = (unsigned) rc;
			chunk = read_cmd->chunk;

			io[0].iov_base = &rbuf.header;
			io[0].iov_len = sizeof rbuf.header;

			rbuf.header.chunk = chunk;
		
			seq = 0;	
			for(cur_off=0;cur_off<size;) {
				rbuf.header.offset = cur_off;
				rbuf.header.seq = seq++;
				io[1].iov_base = buf+cur_off;
				io[1].iov_len = size-cur_off;
				if (io[1].iov_len>RPLAY_PAYLOAD) {
					io[1].iov_len=RPLAY_PAYLOAD;
				}
				rc = writev(sock, io, 2);
				/* printf("sending... %d %d\n", io[1].iov_len, rc); */
				if (rc<0) {
					perror("writev");
					return 1;
				}
				cur_off += io[1].iov_len;
			}
		}
	}

}
