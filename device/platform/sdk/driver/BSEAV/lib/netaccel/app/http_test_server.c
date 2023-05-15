/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: http_test_server.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 2/20/09 4:13p $
 *
 * Module Description: simple test app to stream a content file over 
 *					   standard or accelerated sockets using HTTP transport
 *  
 * Revision History:
 *    
 * $brcm_Log: /BSEAV/lib/netaccel/app/http_test_server.c $
 * 
 * 10   2/20/09 4:13p lwhite
 * PR47685: Coverity fixes
 * 
 * 9   12/2/08 4:45p lwhite
 * PR47685: Coverity fixes
 * 
 * 8   11/12/08 4:06p lwhite
 * PR47685: Coverity fixes
 * 
 * 7   10/28/08 5:22p lwhite
 * PR47685: Coverity fixes
 * 
 * 6   5/19/08 12:23p ssood
 * PR42242: updated HTTP Get string w/ 2.6.18 kernel version
 * 
 * 5   5/15/08 4:58p ssood
 * PR42242: code cleanup for http based streaming
 * 
 * 4   5/12/08 7:50a ssood
 * PR42242: adding poll support
 * 
 * 3   5/11/08 9:02a ssood
 * PR42242: added example to show usage of poll (instead of select)
 * interface
 * 
 * 2   4/14/08 4:47p ssood
 * PR36422: added BE support
 * 
 * 1   3/14/08 9:40p ssood
 * PR39645: adding new test programs for testing HTTP client & server
 * functionality
 * 
 *************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#ifdef __mips__
#include <asm/cachectl.h>
#endif

#include "bnetaccel_info.h"

#define DEFAULT_CONTENT_DIR "/data/videos"
#define ALIGN_4096   (4096 - 1)
#define BUF_SIZE 	(188*7*128)
#define NUM_BUF_DESC 	24
#define O_DIRECT	0x8000
#define FILENAME_MAX_LEN 256

char url[256];			/* content name to stream out */
char url_root[256];		/* Root directory where content is */
char recvbuf[1024];
char sendbuf[2048];
char response[2048];
off_t filesize;
int accel_socket = 0;	/* Use Standard Socket */

void usage()
{
    printf("Usage: http_test_server [-p <port> -a -r <rate> -c <cpu affinity> -m -f <content-directory> -l -k -h]\n");
    printf("options are:\n");
    printf(" -p <port>              # port to listen on (default: 5000)\n");
    printf(" -r <rate>              # rate (default: 20Mpbs)\n");
	printf(" -c <cpu affinity>      # affinity (1 || 2 || 3, default: CPU0 = 1; CPU1 = 2; ANY = 3)\n");
	printf(" -a                     # use accelerated sockets (default: STD Socket)\n");
    printf(" -m                     # send test pattern from memory (default: stream file from disk)\n");
    printf(" -f <content-directory  # (default: /data/videos) \n");
    printf(" -l                     # keep resending the file (default: stream only once)\n");
    printf(" -v                     # print periodic stats (default: no)\n");
    printf(" -k                     # disable TCP checksum computation (default: enabled)\n");
    printf(" -h                     # prints http_test_server usage\n");
    printf("\n");
}

double difftime1(struct timeval *start, struct timeval *stop) 
{
	double dt = 1000000.*(stop->tv_sec - start->tv_sec) + (stop->tv_usec - start->tv_usec);
	return dt;
}

int sockWait(int sd, int socket_type, unsigned int size)
{
    STRM_Status_t status;
    unsigned int len = sizeof(status);

    if(getsockopt(sd, socket_type, STRM_STATUS, &status, &len) != 0)
        return -1;

    while( (status.free_desc_count <= 2) || 
			((status.buffer_locked_depth + size ) > (size * NUM_BUF_DESC))) {
        usleep(10000);
        if (getsockopt(sd,  SOCK_BRCM_DGRAM, STRM_STATUS, &status, &len) != 0)
            return -1;
    }

    return 0;
}

static void setThreadAffinity(int cpu_affinity_mask)
{
#ifdef __mips__
	unsigned long new_mask;

    syscall(SYS_sched_setaffinity, getpid(), 4, &cpu_affinity_mask);
    syscall(SYS_sched_getaffinity, getpid(), 4, &new_mask);

	printf("Updated CPU Affinity mask %lu\n", new_mask);
	/* uncomment this line to verify the cpu affinity */
	/* while (1); */
#endif
}

/* set the number of buffer descriptors in the driver */
int sockSetDesc(int sd, int socket_type, int num_desc)
{
    STRM_SetBuffer_t bi;
    unsigned int len = sizeof(bi);
    bi.num_desc = num_desc;

    if (setsockopt(sd, socket_type, STRM_SETDESC, &bi, len) != 0)
        return -1;
    return 0;
}

int sockSetMode(int sd, int socket_type, int disable_checksum)
{
    STRM_Ctrl_t ctrl;
    memset(&ctrl,0,sizeof(ctrl));

	printf("TCP Checksum computation %s\n", (disable_checksum == 1) ? "OFF" : "ON");
    ctrl.disable_checksum = disable_checksum;
    if(setsockopt(sd, socket_type, STRM_CTRL, &ctrl, sizeof(ctrl)) != 0)
            printf("setsockopt: %s Failed to disable checksum\n", "STRM_CTRL");
    return 0;
}

/* Open the listener service on port 5000 */
int tcpServerSetup(int port, int socket_type)
{
    struct sockaddr_in localAddr;
    int sd;
    int reuse_flag = 1;

    if ( (sd = socket(AF_INET, socket_type, 0)) < 0) {
        /* Socket Create Error */
        perror("Socket Open Err");
        return -EINVAL;
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);

    if(setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof(reuse_flag) ) < 0 ) {
        printf("REUSE Socket Error\n");
        return -EINVAL;
    }

    if (bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr))) {
        perror("Socket Bind Err");
        return -EINVAL;
    }

    if (listen(sd, 4)) {
        perror("Socket listen Err");
        return -EINVAL;
    }

    return sd;
}

/* Parses the input for pattern begin; followed by pattern end */
int parseToken(char *input, char *output, int output_len, char *begin, char *end)
{
	char *p_begin = strstr(input, begin);
	char *p_end;
	char temp;

	if (p_begin)
	{
		p_begin += strlen(begin);
		p_end = strstr(p_begin,end);
		if(!p_end)
			return -1;
		temp = *p_end;
		*p_end = 0;
		printf("TokenFound = [%s]\n",p_begin);
		strlcpy(output,p_begin, output_len-1);
		*p_end = temp;
		return 0;
	}
	return -1; /* Not found */
}

int openUrlFile(char *url, char *filename)
{
	struct stat file_stat;
	char *p;
	FILE *fp;

	filename[0] = 0;
	snprintf(filename, FILENAME_MAX_LEN, "%s/%s", url_root, url);
	while(stat(filename, &file_stat)) {
		if ( (p = strstr(filename,".mpeg"))) {
			printf("Original %s, ",filename);
			strlcpy(p,".mpg", FILENAME_MAX_LEN-6);
			printf("Trying [%s] ",filename);
			if(stat(filename, &file_stat)) {
				printf("Failed\n");
				return -1; 
			}
			printf("OK\n");
			break;
		}
		if ( (p = strstr(filename,".mpg"))) {
			printf("Original %s, ",filename);
			strlcpy(p,".mpeg", FILENAME_MAX_LEN-5);
			printf("Trying [%s] ",filename);
			if(stat(filename, &file_stat)) {
				printf("Failed\n");
				return -1; 
			}
			printf("OK\n");
			break;
		}
		return 0;
	}

	/* File found */
	filesize = file_stat.st_size;
	return 0;
}

#ifdef USE_POLL
#include <sys/poll.h>
void waitForNetworkEvent(int sd)
{
	struct pollfd pfd;
	int rc;
	pfd.fd = sd;
	pfd.events = POLLIN | POLLPRI;
	pfd.revents = 0;

	while (1) {
	if ( (rc = poll(&pfd, 1, -1)) < 0) {
		perror("ERROR: select(): exiting...");
		exit(1);
	}
	else if (rc == 0) 
		continue;
	else if (pfd.revents == POLLIN)
		break;
	}
    
    return;

}

#else /* Use select */
void waitForNetworkEvent(int sd)
{
	fd_set rfds;
	struct timeval tv;

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(sd, &rfds);
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		if ( select(sd +1, &rfds, NULL, NULL, &tv) < 0 ) {
			perror("ERROR: select(): exiting...");
			break;
		}

		if (!FD_ISSET(sd, &rfds))
			/* No request from Client yet, go back to select loop */
			continue;
		break;
	}

    return;
}
#endif

/* waits for incoming HTTP requests, sends out HTTP response and returns new socket descriptor */
int handleHttpRequest(int sd, int socket_type, char *filename)
{
	fd_set rfds;
	struct timeval tv;
	int nsd = -1;
	struct sockaddr_in remoteAddr;
	int addrLen = sizeof(remoteAddr);
	int nbytes;

	while (1) {

		waitForNetworkEvent(sd);

		/* accept connection */
        if ( (nsd = accept(sd, (struct sockaddr *)&remoteAddr, (socklen_t *)&addrLen)) < 0 ) {
			perror("ERROR: accept(): exiting...");
			break;
		}
        printf("Accepted Connection from %lx:%d \n", remoteAddr.sin_addr.s_addr, ntohs(remoteAddr.sin_port));

		waitForNetworkEvent(nsd);

		/* Read HTTP request */
        if ((nbytes = read(nsd, recvbuf, 1024)) <= 0) {
			perror("read failed to read the HTTP Get request\n");
			continue;
		}
        recvbuf[nbytes] = 0;
        printf("Read HTTP Req ( %d bytes)[\n%s]\n", nbytes, recvbuf);
        
        /* Parse HTTP request & open the content file */
        parseToken(recvbuf, url, sizeof(url), "GET /", " ");   
        if( openUrlFile(url, filename) ) { 
            printf("File Not found, go back to listening\n");
            continue;
        }
        printf("Stream file = %s size=%lld\n", filename, filesize);

		/* Build HTTP response */
		strlcpy(response, 
			"HTTP/1.1 200 OK\r\n"
			"Content-Length: %lld\r\n"
			/*"Content-Type: video/mpeg\r\n"*/
			"Connection: Keep-Alive\r\n"
			"Accept-Ranges: bytes\r\n"
			"Connection: close\r\n"
			"Content-Range: bytes 0-%lld/%lld\r\n"
			"Server: Linux/2.6.18, UPnP/1.0, Broadcom UPnP SDK/1.0\r\n"
			"\r\n",
            sizeof(response)-1);
		nbytes = snprintf(sendbuf, sizeof(sendbuf), response, filesize, filesize-1, filesize);
#ifdef __mips__
		syscall(SYS_cacheflush, sendbuf, nbytes, DCACHE); 
#endif
		printf("HTTP Response [\n%s]", sendbuf);

		/* send out HTTP response */
    	if (write(nsd, sendbuf, nbytes) != nbytes) {
			printf("Failed to write HTTP Response of %d bytes\n", nbytes);
			perror("write(): \n");
			break;
		}

		return nsd;
	}
	return -1;
}

int main(int argc, char *argv[])
{
    int port = 5000;		/* Server Port */
	unsigned long cpu_affinity_mask = 1;	/* CPU0 = 1, CPU1 = 2; CPU0 || CPU1 = 3 */
	unsigned long new_mask;
	double rate = 19.4;		/* Default rate to stream content at */
    int loop = 0;
    int send_from_memory = 0;
	struct timeval start, stop;
	int verbose = 0;
	int disable_checksum = 0;
	int sd;					/* socket descriptor */
    unsigned long count = 0;
    unsigned long total = 0;
    int buflen = BUF_SIZE; 
    struct sockaddr_in dest;
    int c, i, j;
    int fd = -1;
    unsigned char *xbuf, *buf;
    int len; 
	loff_t bytes= 0;
	double dt=0, maxrate = 19.4;
    struct iovec iov[NUM_BUF_DESC];
    int nextBuf;
	int socket_type;
	char filename[FILENAME_MAX_LEN];

	url[0] = '\0';
	url_root[0] = '\0';
    while ((c = getopt(argc, argv, "ap:c:f:r:lmkvh")) != -1)
    {
        switch (c) 
        {
		case 'a':
            accel_socket = 1;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'c':
            cpu_affinity_mask = atoi(optarg);
			if (cpu_affinity_mask == 0 || cpu_affinity_mask > 3) {
				printf("Incorrect CPU Affinity %d, valid values are 1, 2, or 3\n", cpu_affinity_mask);
				usage();
				exit(-1);
			}
            break;
        case 'f':
            strlcpy(url_root, optarg, sizeof(url_root)-1);
            break;
        case 'r':
            maxrate = atof(optarg);
            break;
        case 'l':
            loop = 1;
            break;
        case 'm':
            send_from_memory = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'k':
            disable_checksum = 1;
            break;
        case 'h':
        default:
            usage();
            return -1;
        }
    }

	/* print usage anyway */
	usage();

	if (url_root[0] == '\0')
		strlcpy(url_root, DEFAULT_CONTENT_DIR, sizeof(url_root)-1);

	printf("http_test_server: port %d, accel_socket %s, cpu affinity %s, content directory %s, rate %f, loop around %d, send_from_memory %d, verbose %d, disable TCP checksum %d\n",
			port, 
			(accel_socket == 1 ? "YES":"NO"), 
			(cpu_affinity_mask == 1 ? "CPU0" : "CPU1"), 
			url_root, maxrate, loop, send_from_memory, verbose, disable_checksum);


#ifdef __mips__
	/* optionally set CPU affinity */
	setThreadAffinity(cpu_affinity_mask);
	/* uncomment this line to verify the cpu affinity */
	/* while (1); */
#endif

	/* allocate FIFO where data is read from disk & then sent out on the network */
    for (i=0; i<NUM_BUF_DESC; i++) {
        if ( (xbuf = malloc(BUF_SIZE + ALIGN_4096)) == NULL ) {
            perror("posix_memalign failure():");
            goto exit;
        }
        iov[i].iov_base = (unsigned char *)(((unsigned long)xbuf + ALIGN_4096) & ~ALIGN_4096);
        iov[i].iov_len = BUF_SIZE;
    }
	printf("Allocated %d-bytes for buffering, # of DESC=%d\n",NUM_BUF_DESC*(BUF_SIZE + ALIGN_4096),NUM_BUF_DESC);

	if (send_from_memory) {
    	for (i=0; i<NUM_BUF_DESC; i++) 
			memset(iov[i].iov_base, 0xff, BUF_SIZE);
	} 

	/* setup HTTP Server */
	socket_type = (accel_socket == 1) ? SOCK_BRCM_STREAM : SOCK_STREAM;
	if ( (sd = tcpServerSetup(port, socket_type)) < 0) {
		printf("Failed to Setup TCP Server, Exiting...\n");
		exit(-1);
	}

	/* wait for HTTP request & send HTTP reply */
	sd = handleHttpRequest(sd, socket_type, filename);

    if (sd<0) {
		printf("Failed handling HttpRequest, Exiting...\n");
        goto exit;
    }

	if (accel_socket) {
		/* optionally increase the netaccel driver buffer descriptor */
		sockSetDesc(sd, socket_type, NUM_BUF_DESC);
		/* optionally disable checksum if disable_checksum is 1, default is 0 */
		sockSetMode(sd, socket_type, disable_checksum);
	}

	/* get file ready for streaming */
    if (!send_from_memory) {
    	if ((fd = open(filename, O_RDONLY)) <= 0) {
        	perror("open():");
        	goto exit;
    	}
    	printf("Content File opened\n");
	}

	/* enable Direct IO */
    if (accel_socket)
		fcntl(fd, F_SETFL, O_DIRECT);

	gettimeofday(&start, NULL);
    nextBuf = 0;
	while(1) {
		/* make sure driver has finished sending all the queued up data */
		if (accel_socket) 
			sockWait(sd, socket_type, BUF_SIZE);

		/* get next buffer to read into */
        buf = (unsigned char *)iov[nextBuf++].iov_base;
        nextBuf %= NUM_BUF_DESC;

		if (send_from_memory) 
			goto send_data;
		/* read next data chunk */
		if ( (buflen  = read(fd, buf, BUF_SIZE)) < 0 ) {
            perror("read():");
            goto exit;
        }
        else if (buflen == 0) {
            printf("**** Reached EOF *******\n");
			gettimeofday(&stop, NULL);
            if (loop) {
                lseek(fd, 0, SEEK_SET); 
                continue;
            }
            break;
        }
        
send_data:
        /* send the data */
        if ( (bytes = write(sd, buf, buflen)) != buflen) {
			printf("Write failed to write %10lu bytes, instead wrote %d bytes\n", buflen, bytes);
            perror("write():");
            goto exit;
        }

        /* pacing logic */ 
		count++;
		total += bytes;
		gettimeofday(&stop, NULL);
		dt = difftime1(&start, &stop);
		rate = 8.*total/dt;
		while(rate > maxrate) {
			usleep(10000);
			gettimeofday(&stop, NULL);
			dt = difftime1(&start,&stop);
			rate = 8.*total/dt;
		} 

		if(verbose && (count % 100) == 0) {
			printf("[%6lu] Wrote %10lu Bytes in dt = %12.2fusec at rate=%2.1f/%2.1f\n", 
					count, total, dt, rate, maxrate);  
		}
	}

exit:
    for (i=0; i<NUM_BUF_DESC; i++)
	    free(iov[i].iov_base);

	printf("Final stats: Streamed %lu Bytes of %s file in dt = %10.2fusec at rate=%2.1f/%2.1f\n", 
			total, filename, dt, rate, maxrate);  

	/* needed for all data to get drained from the socket */
	sleep(2);
    return 0;
}
