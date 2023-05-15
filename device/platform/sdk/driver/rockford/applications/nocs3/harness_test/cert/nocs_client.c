/******************************************************************************
 *    (c)2008 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nocs_client.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/20/12 2:06p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/applications/nocs3/harness_test/cert/nocs_client.c $
 * 
 * Hydra_Software_Devel/2   7/20/12 2:06p yili
 * SW7346-555:NOCS3 integration
 * 
 * 6   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 * 
 * 5   6/7/08 1:39p jgarrett
 * PR 43318: Enabling display on DTV platforms without composite outputs.
 * 
 * 4   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 * 
 * 3   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 * 
 * 2   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 * 
******************************************************************************/
/* Nexus example app: single live video decode from an input band */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_input.h"
#include "nexus_video_window.h"
#include "nexus_message.h"
#include "nexus_memory.h"
#include "bstd.h"
#include "bkni.h"
#include "nexus_otpmsp.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "nexus_dma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <netdb.h> 



#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> /* or <netinet/in.h> */
#include <fcntl.h>
#include <sys/stat.h>

void usage(char *argv[]);

int gServerSock; 
int g_infd; /* input file descriptor */
int g_outfd; /* output file descriptor */
int g_numchld; /* number of childre spawn in netmode */



#define INBUFSIZE 4096
#define OUTBUFSIZE 4096
#define CERT_BRIDGE_VERSION 0x20110727




void usage(char *argv[]) {
	printf(
			"CERT Bridge v %x\n"
			"Usage:\n"
			"  %s -f [-i filein] [-o fileout] [-l]\n"
			"or\n"
			"  %s -n [-p bindport] [-b bindaddr] [-l]\n"
			"\n"
			"where\n", CERT_BRIDGE_VERSION, argv[0], argv[0]
			);
	printf(
			"   -f   use file mode\n"
			"   -n   use network mode (default)\n"
			" Global options:\n"
			"   -l   enable logging.\n"
			" File mode options:\n"
			"   -i   set input filename. Defaults reading from stdin.\n"
			"   -o   set output filename. Defaults writing to stdout.\n"
			" Network mode options:\n"
			"   -p   set bind port. root permission are required for ports < 1024. (default 10101)\n"
			"   -b   set bind address. The address should be in the standard numbers-and-dots notation.\n"			
			);
}

int main(int argc, char* argv[])
{    
	char text_input[20];
	int select_test;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
int numread;
FILE *stream;
	
	NEXUS_PlatformSettings platformSettings;

	/* Platform init */
    	NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    	platformSettings.openFrontend = false;
    	NEXUS_Platform_Init(&platformSettings);

   	/* Check if the command line is valid. */
    if (argc < 3) {
       printf("usage %s hostname port\n", argv[0]);
       return 0;
    }
    portno = atoi(argv[2]);
    
    /* create a socket to communicate over. */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("ERROR opening socket\n");
        
    /* Get the name of the host */
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("ERROR, no such host\n");
        return 0;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
    /* Connect to the server. */
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");


if( (stream = fopen("inttest_AES.tlv", "r")) == NULL)
	printf("The file is not opened\n");

numread = fread(buffer, sizeof(char), 256, stream);

printf("file size is %x\n", numread);
#if 0
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
#endif

printf("buffer len = %x\n", strlen(buffer));
#if 0	
    n = write(sockfd,buffer,strlen(buffer));
#endif
    n = write(sockfd,buffer,numread);

    if (n < 0) 
         printf("ERROR writing to socket\n");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         printf("ERROR reading from socket\n");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;

	BHSM_P_DONE_LABEL:	
	return 0;
}

