/***************************************************************************
 *     Copyright (c) 2008-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: http_test_client.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 8/28/09 6:44p $
 *
 * Module Description:
 * 	Sample Code for sending HTTP Get Request and writing received data to disk.
 *  
 * 
 *************************************************************/ 
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#ifdef __mips__
#include <asm/cachectl.h>
#endif

#include "bnetaccel_info.h"

#define SERVER_PORT	5000
#define BUF_MASK	0x1ff
#define BUF_SIZE 	(188*7*64)
#define O_DIRECT 0x8000

void usage()
{
    printf("Usage: http_test_client: -d <srv-ip> -p <srv-port> -f <content-file> [-h -a -v]\n");
    printf("options are:\n");
    printf(" -d <srv-ip>   # IP address of HTTP Server (e.g. 192.168.1.110)\n");
    printf(" -p <srv-port> # Port of HTTP Server (e.g. 5000)\n");
    printf(" -f <file>     # url of stream; /data/videos prepended (e.g. portugal.mpg) \n");
    printf(" -a            # use accelerated sockets (default: STD Socket)\n");
    printf(" -m            # just leave content in memory, dont write to disk (default: write to file)\n");
	printf(" -v            # print periodic stats (default: no)\n");
    printf(" -h            # prints http_test_client usage\n");
    printf("\n");
}

double difftime1(struct timeval *start, struct timeval *stop)
{
	double dt = 1000000.*(stop->tv_sec - start->tv_sec) + (stop->tv_usec - start->tv_usec);
	return dt;
}

/* This function creates a TCP connection to server and returns the socket descriptor */
int TcpConnect(char *server, int port, int socket_type)
{
	int sd,rc;

	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;

 	printf("TCP - Connection to %s:%d ...\n",server, port);

	h = gethostbyname(server);
	if (h == NULL) {
		perror("Unknown Host");
		return (-EINVAL);
	}

	servAddr.sin_family = h->h_addrtype;
	memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0],h->h_length);
	servAddr.sin_port = htons(port);

	/* Open Socket */
	sd = socket(AF_INET, socket_type, 0);
	if (sd<0) {
		perror("Socket Open Err");
		return -EINVAL;
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	if (bind(sd,(struct sockaddr *) &localAddr, sizeof(localAddr))) {
		perror("Socket Bind Err");
		return -EINVAL;
	}

	/* Connect to server */
	rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
	if (rc<0) {
		perror("Connect Error: Server busy or unavailable:");
		return -EBUSY;
	}

	printf("TCP: Connection Success\n");
	return sd;
}

#define PKTS_PER_READ 64
#define MAX_BUFFER_SIZE (1316 * PKTS_PER_READ)
#ifdef __mips__
int sockSetRecvParams(int sd, int accel_socket) 
{
	STRM_SockRecvParams_t sockRecvParams = STRM_SOCK_RECV_PARAMS_HTTP_DEFAULT;
	socklen_t len;
	int    rcv_buf = 400000;   

	if (accel_socket) {
		/* increase the # of pkts per read */
    	sockRecvParams.pktsPerRecv = PKTS_PER_READ;
		/* return TCP payloads back to back in the recv buffer */
    	sockRecvParams.pktOffset = 0;
		/* let driver choose the hdroffset, which is basically TCP payload */
    	sockRecvParams.hdrOffset = 0;
    	if (setsockopt(sd, SOCK_BRCM_STREAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0) {
    		printf("%s: setsockopt() ERROR:", __FUNCTION__);
			/* in case of failure (shouldn't happen), read 1 pkt at a time */
		}
		printf("Modified the default pkts per recvfrom to %d\n", sockRecvParams.pktsPerRecv);
	}
	else {
		/* increase the socket receive buffer */
		if (setsockopt( sd, SOL_SOCKET, SO_RCVBUF, (const char*)&rcv_buf, sizeof(rcv_buf) ) < 0 ) {
			perror("SO_RCVBUF Socket Error:\n");
			return -EINVAL;
		}
	}

    return 0;
}
#else
int updateSockParams(int sd) 
{
	return 0;
}
#endif

#define HTTP_POST_SIZE 2048

int main(int argc, char **argv)
{
	char *post;
	char *server = NULL;
	char *fname = NULL;
	int maxrate = 19;
	int c;
	int sd;
	FILE *fp = NULL;
	char *p;
	char *rbuf;
	int  port = 0;
	ssize_t n;
	unsigned char *buf, *xbuf;
	ssize_t bytes = 0;
	unsigned long total=0;
	unsigned long count=0;
	char tok;
	ssize_t len;
	double dt;
	struct timeval start, stop;
	int accel_socket = 0;   /* Use Standard Socket */
	int dont_write_to_disk = 0;
	int verbose = 0;

    while ((c = getopt(argc, argv, "d:p:f:amvh")) != -1) {
        switch (c) {
        case 'd':
            server = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'f':
            fname = optarg;
            break;
        case 'a':
            accel_socket = 1;
            break;
        case 'm':
            dont_write_to_disk = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            usage();
            return -1;
        }
    }

	if (port == 0 || server == NULL || fname == NULL) {
		printf("Missing Args...\n");
		usage();
		exit(1);
	}

	printf("Server %s, Port %d, File %s, Maxrate %f\n",
			server, port, fname, maxrate);

	xbuf = (unsigned char *) malloc(BUF_SIZE + BUF_MASK);
	rbuf = (char *) malloc(1024);
	post = (char *) malloc(HTTP_POST_SIZE);

	if (xbuf == NULL || rbuf == NULL || post == NULL) {
		perror("malloc failure\n");
		exit(-1);
	}

	buf = (unsigned char *) (((unsigned long)xbuf + BUF_MASK) & ~BUF_MASK);

	/* Build HTTP Get Request */
	n = snprintf(post, HTTP_POST_SIZE,
			"GET /%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Rate: %d\r\n"
			"PlaySpeed.dlna.org: speed=1\r\n"
			"User-Agent: %s Test App\r\n"
			"\r\n",
			fname, server, port, maxrate, (accel_socket == 1 ? "BRCM" : ""));
	printf("Sending HTTP Request:----->[\n%s]\n", post);
#ifdef __mips__
	syscall(SYS_cacheflush, post, n, DCACHE); 
#endif

	/* open file for writing */
	if (!dont_write_to_disk) {
		fp = fopen(fname,"wb");
		if (!fp) {
			perror("File open error:\n");
			exit(-1);
		}
	}

	/*TODO: enable O_DIRECT and test, need to change BUF_SIZE*/
	/*fcntl(fileno(fp),F_SETFL,O_DIRECT);*/
	
	/* Connect to Server */
	sd = TcpConnect(server, port, (accel_socket == 1 ? SOCK_BRCM_STREAM : SOCK_STREAM));
	if (sd < 0) {
			printf("Connection to Server Failed, Exiting...\n");
			exit(-1);
	}
	/* Send HTTP Get Request */
	n = write(sd, post, strlen(post));
	if ((unsigned)n != strlen(post)) {
		printf("Failed to write HTTP Get Request: rc %d\n", n);
		perror("write(): ");
		exit(-1);
	}

	printf("Succesfully Send HTTP Get Request to to %s:%d\n", server, port);
	usleep(10000);

	/* Read HTTP Response */
	memset(rbuf, 0, 1024);
	if ( (n = read(sd, rbuf, 1024)) <= 0) {
		printf("Failed to read HTTP Response: rc = %d\n", n);
		perror("read(): ");
		exit(-1);
	}

    rbuf[n] = '\0'; 

	/* Scan for end of HTTP header */
	p = strstr(rbuf,"\r\n\r\n");
	if(!p) {
		printf("No HTTP Header\n");
		len = 0;
		p = rbuf;
		tok = 0;
	}
	else {
		p+=4;
		tok = *p;
		*p = 0;
		len = strlen(rbuf);
	}

	printf("Total response len %d, payload len %d\n", n, n-len);
	printf("HTTP Response: -----> [\n%s]\n", rbuf);
	*p = tok;

	/* write any data that was read part of the initial read */
	if (n>len) {
		if ( (total = write(fileno(fp), p, n - len)) <= 0 ) {
			printf("Failed to write initial payload bytes (%lu)\n", total);
			perror("write():\n");
			exit(-1);
		}
	}

	/* increase the recv buffer before starting to read the content */
	sockSetRecvParams(sd, accel_socket);
	sleep(1);

	gettimeofday(&start, NULL);
	/* read data from network & write to the file */
	while(1) {
		if ( (n = read(sd, buf, BUF_SIZE)) <= 0) {
			gettimeofday(&stop, NULL);
			printf("read failed: n = %d\n", n);
			perror("read(): ");
			break;
		}

		gettimeofday(&stop, NULL);
		if (dont_write_to_disk) {
            bytes = n;
			goto after_write;
        }
		if (((bytes = write(fileno(fp), buf, n)) <= 0) || bytes != n) {
			printf("Failed to write payload bytes (%d)\n", bytes);
			perror("write():\n");
			break;
		}

after_write:
		dt = difftime1(&start, &stop);
		total += bytes;
		count++;
		if (verbose && (count % 100) == 0) {
			printf("[%10lu] Received %10lu Bytes in dt = %10.2fusec at rate %2.1f\n", 
					count, total, dt, (total * 8. / dt));  
		}
	}

	dt = difftime1(&start, &stop);
	printf("Final stats: Received %10lu bytes to %s file in %10.1fusec at %2.1f rate\n", 
			total, fname, dt, (total * 8. / dt));
	fclose(fp);
	free(xbuf);
	free(rbuf);
    free(post);
	return 0;
}
