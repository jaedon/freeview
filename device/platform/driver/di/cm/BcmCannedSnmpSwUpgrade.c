//**************************************************************************
//
//    Copyright 2007 Broadcom Corporation
//    All Rights Reserved
//    No portions of this material may be reproduced in any form without the
//    written permission of:
//             Broadcom Corporation
//             16251 Laguna Canyon Road
//             Irvine, California  92618
//    All information contained in this document is Broadcom Corporation
//    company private, proprietary, and trade secret.
//
//
//
//**************************************************************************
//    Filename: BcmCannedSnmpSwUpgrade.c
//    Author:   Maurice Turcotte
//    Creation Date: 03 JAN 07
//
//**************************************************************************
//    Description:
//
//              A tiny app to do fixed SNMPv1 Set operations on the eCM,
//              specifically a multi-variable set to initiate a download
//              on the eCM.
//
//              In order for this to work properly the following must be
//              defined correctly:
//
//             ecmAddress - The IP address of the eCM
//
//             docsDevSwServer - The IP address, in hex, of the download server
//
//             docsDevSwFilename - The name of file to download
//
//             snmpCommunity - The SNMP write community of the eCM
//
//
//**************************************************************************
//    Revision History:
//
//**************************************************************************
#if 1 /* __HUMAX_CODE__ nhlee add */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "vkernel.h"
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */
#include	<setjmp.h>		/* for signal jump buffer */

#include	<sys/select.h>	/* for convenience */

#include	<poll.h>		/* for convenience */

#include	<strings.h>		/* for convenience */
#include "di_uart.h"

/* Miscellaneous definitions */
#define	MAX_MSG_LEN		1500	/* max message payload length */
#define	SNMP_PORT	161	/* SNMP Port */
#define  SNMPERROR_OFFSET 23    /* offset into the message where we find the error status */

/*** These are the paramters that need to be configured ***/

/* The IP address of the eCM */
char *ecmAddress = {"192.168.100.1"};

/* The IP address, in hex, of the server where we download from */
#if 1
unsigned char docsDevSwServer[] = {0xC0, 0xA8, 0x11, 0x0A}; /*192.168.17.10 is my ip address*/

#else
char docsDevSwServer[] = {0x0A, 0x0A, 0x0A, 0x0A};
#endif

/* The name of file we will download */
//char docsDevSwFilename[] = {"ecram_sto.bin.3255"};
//char docsDevSwFilename[] = {"ecm_0_9_13_rph_slim.bin"};
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
char docsDevSwFilename[] = {"/media/ecm.bin"};
#else
char docsDevSwFilename[] = {"/media/drive1/ecm.bin"};
#endif
/* The SNMP write community of the eCM */
char snmpCommunity[] = {"private"};

/*** End of paramters that need to be configured ***/


unsigned char downloadMsgBuf[MAX_MSG_LEN];

static unsigned char snmpInPktsMsg[] = {0x30, 0x29, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
                                      0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1C, 0x02,
                                      0x04, 0x42, 0xDB, 0x92, 0x65, 0x02, 0x01, 0x00,
                                      0x02, 0x01, 0x00, 0x30, 0x0E, 0x30, 0x0C, 0x06,
                                      0x08, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0B, 0x01,
                                      0x00, 0x05, 0x00};

#define  SNMPINPKTSMSG_OFFSET 41

/*
static unsigned char swDownloadMsg[] =
    {0x30, 0x54, 0x02, 0x01,   0x00, 0x04, 0x06, 0x70,
     0x75, 0x62, 0x6c, 0x69,   0x63, 0xa0, 0x47, 0x02,
     0x01, 0x06, 0x02, 0x01,   0x00, 0x02, 0x01, 0x00,
     0x30, 0x3c, 0x30, 0x11,   0x06, 0x09, 0x2b, 0x06,
     0x01, 0x02, 0x01, 0x45,   0x01, 0x03, 0x01, 0x40,
     0x04, 0x0a, 0x0b, 0x0b,   0x0b, 0x30, 0x16, 0x06,
     0x0a, 0x2b, 0x06, 0x01,   0x02, 0x01, 0x45, 0x01,
     0x03, 0x02, 0x00, 0x04,   0x08, 0x6a, 0x75, 0x6e,
     0x6b, 0x2e, 0x62, 0x69,   0x6e, 0x30, 0x0f, 0x06,
     0x0a, 0x2b, 0x06, 0x01,   0x02, 0x01, 0x45, 0x01,
     0x03, 0x03, 0x00, 0x02,   0x01, 0x01 };
*/

/*
  30 56 02 01   00 04 07 70   72 69 76 61   74 65 a3 48
  02 01 07 02   01 00 02 01   00 30 3d 30   12 06 0a 2b
  06 01 02 01   45 01 03 01   00 40 04 0a   0b 0b 0b 30
  16 06 0a 2b   06 01 02 01   45 01 03 02   00 04 08 6a
  75 6e 6b 2e   62 69 6e 30   0f 06 0a 2b   06 01 02 01
  45 01 03 03   00 02 01 01
*/


static unsigned char setMsgHdr[] = {0x30, 0xff, 0x02, 0x01, 0x00};

static unsigned char setMsgCommunityHdr[] =
    {0x04, 0xff};

#if 1	/* nhlee increase request id */
static unsigned char setMsgPduHdr[] =
    {0xa3, 0xff, 0x02, 0x02, 0x06, 0x03, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00 };
#else
static unsigned char setMsgPduHdr[] =
    {0xa3, 0xff, 0x02, 0x01, 0x06, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00 };
#endif

static unsigned char setMsgVarBindsHdr[] = { 0x30, 0xff};

static unsigned char docsDevSwServerVar[] =
    {0x30, 0xff, 0x06, 0x0a,   0x2b, 0x06, 0x01, 0x02,
     0x01, 0x45, 0x01, 0x03,   0x01, 0x00, 0x40, 0xff };

static unsigned char docsDevSwFilenameVar[] =
     {0x30, 0xff, 0x06, 0x0a,   0x2b, 0x06, 0x01, 0x02,
      0x01, 0x45, 0x01, 0x03,   0x02, 0x00, 0x04, 0xff};

static unsigned char docsDevSwAdminStatusVarBind[] =
     {0x30, 0x0f, 0x06, 0x0a,   0x2b, 0x06, 0x01, 0x02,
     0x01, 0x45, 0x01, 0x03,   0x03, 0x00, 0x02, 0x01,
     0x01};

#define MSGHDR_OFFSET 1
#define MSGHDR_SIZE 2


extern unsigned int GetUInt(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,  char *reqMsg, int reqMsgLen, int offset);
extern int SetDocsisDownload(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen);

/* Add docsDevSwServer VarBind to buffer and return the number of bytes added */
int addDocsDevSwServerVarBind(unsigned char *buf)
{
	VK_memcpy(buf, docsDevSwServerVar, sizeof(docsDevSwServerVar));
	buf[MSGHDR_OFFSET] = sizeof(docsDevSwServerVar) + sizeof(docsDevSwServer) - MSGHDR_SIZE;
	buf += sizeof(docsDevSwServerVar) - 1;
	*buf = sizeof(docsDevSwServer);
	buf++;
	VK_memcpy(buf, docsDevSwServer, sizeof(docsDevSwServer));
	DI_UART_Print ("addDocsDevSwServerVarBind returning %d\n", sizeof(docsDevSwServerVar) + sizeof(docsDevSwServer));

	return (sizeof(docsDevSwServerVar) + sizeof(docsDevSwServer));
}

/* Add docsDevSwFilename VarBind to buffer and return the number of bytes added */
int addDocsDevSwFilenameVarBind(unsigned char *buf)
{
	VK_memcpy(buf, docsDevSwFilenameVar, sizeof(docsDevSwFilenameVar));
	buf[MSGHDR_OFFSET] = sizeof(docsDevSwFilenameVar) + VK_strlen(docsDevSwFilename) - MSGHDR_SIZE;
	buf += sizeof(docsDevSwFilenameVar) - 1;
	*buf = VK_strlen(docsDevSwFilename);
	buf++;
	VK_memcpy(buf, docsDevSwFilename, VK_strlen(docsDevSwFilename));
	DI_UART_Print ("addDocsDevSwFilenameVarBind returning %d\n", sizeof(docsDevSwFilenameVar) + VK_strlen(docsDevSwFilename));

	return (sizeof(docsDevSwFilenameVar) + VK_strlen(docsDevSwFilename));
}

/* Add docsDevSwAdminStatus VarBind to buffer and return the number of bytes added */
int addDocsDevSwAdminStatusVarBind(unsigned char *buf)
{
	VK_memcpy(buf, docsDevSwAdminStatusVarBind, sizeof(docsDevSwAdminStatusVarBind));
	buf += sizeof(docsDevSwAdminStatusVarBind);
	DI_UART_Print ("addDocsDevSwAdminStatusVarBind returning %d\n", sizeof(docsDevSwAdminStatusVarBind));

	return (sizeof(docsDevSwAdminStatusVarBind));
}

/* Add VarBinds to buffer and return the number of bytes added */
int addVarBinds(unsigned char *buf)
{
	int bytesAdded = 0;
	unsigned char *hdrLen;

	VK_memcpy(buf, setMsgVarBindsHdr, sizeof(setMsgVarBindsHdr));
	hdrLen = buf + MSGHDR_OFFSET;
	buf += sizeof(setMsgVarBindsHdr);
	bytesAdded += addDocsDevSwServerVarBind(buf + bytesAdded);
	bytesAdded += addDocsDevSwFilenameVarBind(buf + bytesAdded);
	bytesAdded += addDocsDevSwAdminStatusVarBind(buf + bytesAdded);
	*hdrLen = bytesAdded;
	DI_UART_Print ("addVarBinds returning %d\n", bytesAdded + sizeof(setMsgVarBindsHdr));

	return (bytesAdded + sizeof(setMsgVarBindsHdr));
}

/* Add PDU to buffer and return the number of bytes added */
int addPDU(unsigned char *buf)
{
	int bytesAdded = 0;
	unsigned char *hdrLen;

	VK_memcpy(buf, setMsgPduHdr, sizeof(setMsgPduHdr));
	hdrLen = buf + MSGHDR_OFFSET;
	buf += sizeof(setMsgPduHdr);
	bytesAdded += addVarBinds(buf + bytesAdded);
	*hdrLen = bytesAdded + sizeof(setMsgPduHdr) - MSGHDR_SIZE;
	DI_UART_Print ("addPDU returning %d\n", bytesAdded + sizeof(setMsgPduHdr));

	return (bytesAdded + sizeof(setMsgPduHdr));
}

/* Add SNMP community to buffer and return the number of bytes added */
int addCommunity(unsigned char *buf)
{
	VK_memcpy(buf, setMsgCommunityHdr, sizeof(setMsgCommunityHdr));
	buf[MSGHDR_OFFSET] = VK_strlen(snmpCommunity);
	buf += sizeof(setMsgCommunityHdr);
	VK_memcpy(buf, snmpCommunity, VK_strlen(snmpCommunity));
	DI_UART_Print("addCommunity returning %d\n", (sizeof(setMsgCommunityHdr) + VK_strlen(snmpCommunity)));

	return (sizeof(setMsgCommunityHdr) + VK_strlen(snmpCommunity));
}

/* Add Set SNMP message to buffer and return the number of bytes added */
int addSnmpMsg(unsigned char *buf)
{
	int bytesAdded = 0;
	unsigned char *hdrLen;

	VK_memcpy(buf, setMsgHdr, sizeof(setMsgHdr));
	hdrLen = buf + MSGHDR_OFFSET;
	buf += sizeof(setMsgHdr);
	bytesAdded = addCommunity(buf);
	bytesAdded += addPDU(buf + bytesAdded);
	*hdrLen = bytesAdded + sizeof(setMsgHdr) - MSGHDR_SIZE;
	DI_UART_Print ("addSnmpMsg returning %d\n", bytesAdded + sizeof(setMsgHdr));

	return (bytesAdded + sizeof(setMsgHdr));
}

void docsSwUgradebyCannedSNMP(void)
{
	//char	recvline[MAX_MSG_LEN + 1];
	//int recvlineLen = sizeof(recvline);
	int msgSize, retVal = -1;
	int n;


	int	sockfd;
	struct sockaddr_in	servaddr;
	struct timeval tv;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, ecmAddress, &servaddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd >=0)
	{
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		n = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		n = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

		retVal = GetUInt(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr),
					(char *)snmpInPktsMsg, sizeof(snmpInPktsMsg),
					SNMPINPKTSMSG_OFFSET);

		/**** Download Set ****/

		if ((msgSize = addSnmpMsg(downloadMsgBuf)) > MAX_MSG_LEN)
		{
			DI_UART_Print("SNMP download message too big\n");
		}
		else
		{

			retVal = SetDocsisDownload(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr),
					(char *)downloadMsgBuf, msgSize);
		}
		close(sockfd);

	}
	else
	{
		DI_UART_Print("docsSwUgradebyCannedSNMP:: socket error....\n");
	}


}


#if 0
int
main()
{
int					sockfd;
struct sockaddr_in	servaddr;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SNMP_PORT);
  inet_pton(AF_INET, ecmAddress, &servaddr.sin_addr);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  mibPeek(stdin, sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr));

  exit(0);
}
#endif

#endif
