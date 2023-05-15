//**************************************************************************
//
//    Copyright 2006 Broadcom Corporation
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
//    Filename: mibPeek.c
//    Author:   Maurice Turcotte
//    Creation Date: 07 JUL 06
//
//**************************************************************************
//    Description:
//
//              A tiny app to do fixed SNMP Get operations on the eCM.
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

#include <sys/ioctl.h>	/* nhlee : for Non-block IO */
#include "di_channel_priv.h"

/* Miscellaneous definitions */
#define ASN_BIT8            (0x80)
#define MAX_SUBID   0xFFFFFFFF
#define	MAX_MSG_LEN		1500	/* max message payload length */
#define	MAC_ADDR_LEN		6	/* length of MAC address, in bytes */
#define	CM_VERSION_LEN		10	/* length of CM version, in bytes */
#define	SNMP_PORT	161	/* SNMP Port */
#define  SNMPERROR_OFFSET 23    /* offset into the message where error status is */
#define  SNMPTYPE_OFFSET 13    /* offset into the message where msg type is */
#define  SNMPREQID_OFFSET SNMPTYPE_OFFSET + 2  /* offset into the message where req ID is */
#define  SNMPPRIVREQID_OFFSET SNMPTYPE_OFFSET +1 + 2  /* offset into the private message where req ID is */

/* Miscellaneous definitions from the Euro-DOCSIS Spec */
#define	ECHO_MAX_MSG_LEN					1466	/* max echo message payload length */
#define	ECHO_MIN_MSG_LEN					18		/* max echo message payload length */
#define	EURODOCSIS_ECHO_PORT		0x2711	/* EuroDOCSIS UDP Echo Port */
#define MAGIC_NUMBER_FIRST_BYTE		0x45
#define MAGIC_NUMBER_SECOND_BYTE	0x75


extern int show_di_cm;

#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif

char *targetAddress = {"192.168.100.1"};
#if 0
static unsigned char snmpInPktsMsg[] = {0x30, 0x29, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
                                      0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1C, 0x02,
                                      0x04, 0x42, 0xDB, 0x92, 0x66, 0x02, 0x01, 0x00,
                                      0x02, 0x01, 0x00, 0x30, 0x0E, 0x30, 0x0C, 0x06,
                                      0x08, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0B, 0x01,
                                      0x00, 0x05, 0x00};

#define  SNMPINPKTSMSG_OFFSET 41

static unsigned char docsDevServerBootStateMsg[] =
                                     {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
                                      0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02,
                                      0x04, 0x74, 0xDB, 0x92, 0x67, 0x02, 0x01, 0x00,
                                      0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06,
                                      0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x45, 0x01,
                                      0x04, 0x01, 0x00, 0x05, 0x00};

#define  DOCSDEVSERVERBOOTSTATEMSG_OFFSET 43
#endif

/* docsIfBaseObject (1.3.6.1.2.1.10.127.1.1)*/
/* docsIfDownstreamChannelTable(1.3.6.1.2.1.10.127.1.1.1)*/


/* internet.mgmt.mib-2 (1.3.6.1.2.1.4.20.1.2) */
static unsigned char mgmtIpAddressMsg[] =
					{0x30, 0x2A, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
					0x75, 0x62, 0x6C, 0x69, 0x63, 0xA1, 0x1D, 0x02,
					0x04, 0x74, 0xDB, 0x92, 0x6E, 0x02, 0x01, 0x00,
					0x02, 0x01, 0x00, 0x30, 0x0F, 0x30, 0x0D, 0x06,
					0x09, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x04, 0x14,
					0x01, 0x02, 0x05, 0x00};
#define  MGMTIPADDRESSMSG_OFFSET 46
#define  MGMTIPADDRESSMSG_INDEX_OFFSET 42
#define  MGMTIPADDRESSMSG_OIDLEN 9

#define  OID_OFFSET 31
#define  SEQ_OFFSET 17

/*Error check for Public Mib*/
int asnCheckErrorStatus(char *msg, unsigned int msgLen)
{
	register  unsigned char snmpErrorStatus;

	if (SNMPERROR_OFFSET >= msgLen)
	{
		CM_DI_Print(0,("asnCheckErrorStatus: offset out of bounds\n"));
		return 0;
	}

	snmpErrorStatus = msg[SNMPERROR_OFFSET];

	if (snmpErrorStatus != 0)
	{
		CM_DI_Print(0,("asnCheckErrorStatus: error %d found\n", snmpErrorStatus));
		return 0;
	}

	/* A OK! */
	return 1;
}

/*Error check for Private MIB*/
int asnCheckErrorStatus_P(char *msg, unsigned int msgLen)
{
	register  unsigned char snmpErrorStatus;

	if (SNMPERROR_OFFSET >= msgLen)
	{
		CM_DI_Print(0,("asnCheckErrorStatus: offset out of bounds\n"));
		return 0;
	}

	snmpErrorStatus = msg[SNMPERROR_OFFSET-1];

	if (snmpErrorStatus != 0)
	{
		CM_DI_Print(0,("asnCheckErrorStatus: error %d found\n", snmpErrorStatus));
		return 0;
	}

	/* A OK! */
	return 1;
}



int asnParseLengthCheck(unsigned int msgLen, unsigned int offset, unsigned long asnLen)
{
	// first, a basic sanity check - make sure the offset is within the message
	if (offset >= msgLen)
	{
		return 0;
	}

	// the real test - if the asn length is more than the number of
	// bytes left in the message, then fail
	if (asnLen > (msgLen - offset))
	{
		return 0;
	}
	return 1;
}


int asnParseLength(unsigned char *msg, unsigned int msgLen, unsigned int *offset)
{
	register  unsigned char lengthbyte;

	/* CM_DI_Print(0,("asnParseLength: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset)); */
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseLength: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseLength: offset out of bounds\n"));
		return -1;
	}

	lengthbyte = msg[*offset];

	/* increment offset for the calling code */
	(*offset)++;

	/* check for long length format, which we should never have */
	if (lengthbyte & 0x80)
	{
		CM_DI_Print(0,( "asnParseLength: long length value not supported.\n"));
		return -1;
	}
	else  /* short asnlength */
	{
		return (unsigned int) lengthbyte;
	}
}

void dump_hex(unsigned char *buffer, unsigned int len, unsigned int offset)
{
	unsigned int i, j;
	unsigned char *phelper;
	phelper = (unsigned char *)buffer;
	if (buffer == NULL)
		return;

	for(j = 0; j < offset; j++)
		CM_DI_Print(0,(" "));

	for (i =0 ; i < len ; i++)
	{
		CM_DI_Print(0, ("%02hX ", (*phelper)));
		phelper++;
		if(((i+1)%10 == 0) && (i != 0))
		{
			CM_DI_Print(0,("\n"));
			for(j = 0; j < offset; j++)
				CM_DI_Print(0,(" "));
		}
	}
	CM_DI_Print(0,("\n"));
}



unsigned int asnParseInt(unsigned char *msg, unsigned int msgLen, unsigned int *offset)
{
	unsigned char asnType;
	unsigned long asnLength;
	register long value = 0;

	/* CM_DI_Print(0,("asnParseInt: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset)); */
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseInt: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseInt: offset out of bounds\n"));
		return -1;
	}

	/* check ASN Type */
	asnType = msg[*offset];
	if (asnType != 0x02)
	{
		CM_DI_Print(0,("asnParseInt expected asnType 0x02, but got %d so give up!!!\n", asnType));
		return -1;
	}

	(*offset)++;

	/* get ASN length */
	asnLength = asnParseLength(msg, msgLen, offset);

	if (msg[*offset] & 0x80)
		value = -1;         /* integer is negative */
	while (asnLength-- > 0)
	{
		value = (value << 8) | msg[*offset];
		(*offset)++;
	}

	return value;

}

unsigned int asnParseUnsignedInt(unsigned char *msg, unsigned int msgLen, unsigned int *offset)
{
	unsigned char asnType;
	unsigned long asnLength;
	register u_long value = 0;

	/* CM_DI_Print(0,("asnParseUnsignedInt: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset)); */
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseUnsignedInt: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseUnsignedInt: offset out of bounds\n"));
		return -1;
	}

	/* check ASN Type */
	asnType = msg[*offset];
	if (asnType == 0x41)
	;
	else if (asnType == 0x42)
	  ;
	else if (asnType == 0x43)
	  ;
	else
	{
		CM_DI_Print(0,("asnParseUnsignedInt expected asnType 0x41, 0x42, or 0x43, but got %d so give up!!!\n", asnType));
		return 0;
	}

	(*offset)++;

	/* get ASN length */
	asnLength = asnParseLength(msg, msgLen, offset);

	if (msg[*offset] & 0x80)
	value = ~value;

	while (asnLength-- > 0)
	{
		value = (value << 8) | msg[*offset];
		(*offset)++;
	}

	return value;

}


int asnParseString(unsigned char *msg, unsigned int msgLen, unsigned int *offset, unsigned char *parsedString, int *parsedStringLen)
{
	unsigned char asnType;
	unsigned long asnLength;
	//register unsigned long value = 0;

	/* CM_DI_Print(0,("asnParseString: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset); */
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseString: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseString: offset out of bounds\n"));
		return -1;
	}

	/* check ASN Type */
	asnType = msg[*offset];
	if (asnType != 0x04)
	{
		CM_DI_Print(0,("asnParseString expected asnType 0x04, but got %x so give up!!!\n", asnType));
		return -1;
	}

	(*offset)++;

	/* get ASN length */
	asnLength = asnParseLength(msg, msgLen, offset);
	//CM_DI_Print(0,("asnParseString: get length (%d)\n", (int)asnLength));
	//dump_hex(&msg[*offset], asnLength, 1);

	if (asnLength > (unsigned long)*parsedStringLen)
	{
		CM_DI_Print(0,("asnParseString expected asnLength <= %d, but got %d so give up!!!\n", *parsedStringLen, (int)asnLength));
		return -1;
	}

	VK_memset(parsedString, 0, *parsedStringLen);

	memmove(parsedString, &(msg[*offset]), asnLength);

	*parsedStringLen = asnLength;

	return asnLength;

}

// Parse and ASN.1 object id. Puts the OID into objid, the number of subids into
// objidLen, returns the number of bytes parsed to get the OID and adjusts offset.
int asnParseObjid(unsigned char *msg, unsigned int msgLen, unsigned int *offset, unsigned long *objid, int *objidLen)
{
	/*
	 * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
	 * subidentifier ::= {leadingbyte}* lastbyte
	 * leadingbyte ::= 1 7bitvalue
	 * lastbyte ::= 0 7bitvalue
	 */
	register unsigned long          *oidp = objid + 1;
	register unsigned long subidentifier, length;
	unsigned long          asnLength;
	unsigned char          asnType;
	unsigned char          *bufp;

	// CM_DI_Print(0,("asnParseObjid: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset));
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseObjid: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseObjid: offset out of bounds\n"));
		return -1;
	}

	/* check ASN Type */
	asnType = msg[*offset];
	if (asnType != 0x06)
	{
		CM_DI_Print(0,("asnParseObjid expected asnType 0x06, but got %d so give up!\n", asnType));
		return -1;
	}

	(*offset)++;

	/* get ASN length */
	asnLength = asnParseLength(msg, msgLen, offset);

	    //dump_hex(&msg[*offset], asnLength, 1);
#if 0
	if (asnLength < 0)
	{
		CM_DI_Print(0,("asnParseObjid got asnLength %d so give up!!!\n", (int)asnLength));
		return -1;
	}
#endif
	if (!(asnParseLengthCheck(msgLen, *offset, asnLength)))
	{
		CM_DI_Print(0,("asnParseObjid got failed length check %d so give up!!!\n", (int)asnLength));
		return -1;
	}

	*objidLen = asnLength;

	// Handle invalid object identifier encodings of the form 06 00 robustly
	if (asnLength == 0)
	{
		objid[0] = objid[1] = 0;
		return 0;
	}

	length = asnLength;

	bufp = &msg[*offset];

	while (length > 0)
	{
		subidentifier = 0;
		do
		{                    /* shift and add in low order 7 bits */
			subidentifier = (subidentifier << 7) + (*(u_char *) bufp & ~ASN_BIT8);
			length--;
			(*offset)++;
		} while (*(u_char *) bufp++ & ASN_BIT8);        /* last byte has high bit clear */
		/*
		 * ?? note, this test will never be true, since the largest value
		 * of subidentifier is the value of MAX_SUBID!
		 */
		if (subidentifier > (u_long) MAX_SUBID)
		{
			return -1;
		}
		*oidp++ = (long) subidentifier;
	}

	/*
	 * The first two subidentifiers are encoded into the first component
	 * with the value (X * 40) + Y, where:
	 *  X is the value of the first subidentifier.
	 *  Y is the value of the second subidentifier.
	 */
	subidentifier = (u_long) objid[1];
	if (subidentifier == 0x2B)
	{
		objid[0] = 1;
		objid[1] = 3;
	}
	else
	{
		if (subidentifier < 40)
		{
			objid[0] = 0;
			objid[1] = subidentifier;
		}
		else if (subidentifier < 80)
		{
			objid[0] = 1;
			objid[1] = subidentifier - 40;
		}
		else
		{
			objid[0] = 2;
			objid[1] = subidentifier - 80;
		}
	}

	*objidLen = (int) (oidp - objid);

	return asnLength;
}


unsigned long int asnParseIp(unsigned char *msg, unsigned int msgLen, unsigned int *offset)
{
	unsigned char asnType;
	unsigned long asnLength;
	register unsigned long value = 0;

	/* CM_DI_Print(0,("asnParseIp: msg = %p, msgLen = %d, offset = %d\n", msg, msgLen, *offset)); */
	/* first do some sanity checking */
	if (!msg)
	{
		CM_DI_Print(0,("asnParseIp: NULL pointer\n"));
		return -1;
	}

	if ((*offset > MAX_MSG_LEN) || (*offset >= msgLen))
	{
		CM_DI_Print(0,("asnParseIp: offset out of bounds\n"));
		return -1;
	}

	/* check ASN Type */
	asnType = msg[*offset];
	if (asnType != 0x40)
	{
		CM_DI_Print(0,("asnParseIp expected asnType 0x40, but got %d so give up!!!\n", asnType));
		return -1;
	}

	(*offset)++;

	/* get ASN length */
	asnLength = asnParseLength(msg, msgLen, offset);

	//dump_hex(&msg[*offset], asnLength, 1);
	if (asnLength != 4)
	{
		CM_DI_Print(0,("asnParseIp expected asnLength 4, but got %d so give up!!!\n", (int)asnLength));
		return -1;
	}

	while (asnLength-- > 0)
	{
		value = (value << 8) | msg[*offset];
		(*offset)++;
	}

	return value;

}


unsigned long int GetCMIp(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen)
{
	unsigned int iRetcode=100;
	int ifIndex = 0;   // we are looking to the IP address of interface 2 (eCM)

	int     n, sendLength, recvlineOffset;
	char	recvline[MAX_MSG_LEN + 1];
	unsigned long oid[32] = {0,};
	int oidOffset, oidLen = 0;

	VK_memcpy(recvline, mgmtIpAddressMsg, sizeof(mgmtIpAddressMsg));

	sendLength = sizeof(mgmtIpAddressMsg);

	while (ifIndex != 2)
	{

		n = sendto(sockfd, recvline, sendLength, 0, (void *) pservaddr, servlen);
		if (n <= 0)
		{
			CM_DI_Print(0,("GetCMIp sendto failed - returned %d errno %d\n", n, errno));
			return iRetcode;
		}


		n = recvfrom(sockfd, recvline, MAX_MSG_LEN, 0, NULL, NULL);
		// CM_DI_Print(0,("recvfrom returned %d errno %d\n", n, errno);

		if (n <= 0)
		{
			if(errno == EWOULDBLOCK)
				CM_DI_Print(1,("GetCMIp recvfrom timed out\n"));
			else
				CM_DI_Print(0,("GetCMIp recv from got errno %d \n", errno));
			return iRetcode;
		}

		if (VK_memcmp(&mgmtIpAddressMsg[OID_OFFSET + 2], &recvline[OID_OFFSET + 2], MGMTIPADDRESSMSG_OIDLEN))  // unexpected OID
			return iRetcode;

		if (VK_memcmp(&mgmtIpAddressMsg[SEQ_OFFSET], &recvline[SEQ_OFFSET], 4))  // unexpected sequence number
			return iRetcode;

	        // dump_hex(recvline, n, 1);

		if (asnCheckErrorStatus(recvline, n))
		{
			recvlineOffset = OID_OFFSET;
			asnParseObjid((unsigned char *)recvline, n, (unsigned int *)&recvlineOffset, oid, &oidLen);
			ifIndex = asnParseInt((unsigned char *)recvline, n, (unsigned int *)&recvlineOffset);
			// CM_DI_Print(0,("found ifIndex %d\n", ifIndex));
			if(ifIndex == -1)
			{
				iRetcode = 100;
				return iRetcode;
			}
			else if (ifIndex == 2) // We found the CM!!
			{
				iRetcode = 0;
				oidOffset = oidLen - sizeof(iRetcode);
				while (oidOffset < oidLen)
				{
					 iRetcode = (iRetcode << 8) | (unsigned char) oid[oidOffset];
					 oidOffset++;
				}
				//CM_DI_Print(0,("NHLEE : 0 got mgmtIpAddressMsg = %X\n", iRetcode));
			}
			else
			{
			        // CM_DI_Print(0,("got mgmtIpAddress = %X\n", ifIndex));
				sendLength = n;
				recvline[SNMPTYPE_OFFSET] = (char)0xa1;
				iRetcode = ifIndex;
			}
		}
		else
		{
			CM_DI_Print(0,("GetCMIp get mgmtIpAddressMsg failed.\n"));
			iRetcode = 100;
		}
	}
	return iRetcode;
}

int GetResp(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
                char *reqMsg, int reqMsgLen, char *reqMsgId, int reqMsgIdLen,
                char *respMsg, int *respMsgLen, int maxExpectedRespLen)
{
	int n;

	while (1)
	{

		n = sendto(sockfd, reqMsg, reqMsgLen, 0, (void *) pservaddr, servlen);

		if (n <= 0)
		{
			CM_DI_Print(0,("GetResp sendto failed - returned %d errno %d\n", n, errno));
			return FALSE;
		}

		n = recvfrom(sockfd, respMsg, *respMsgLen, 0, NULL, NULL);
		// CM_DI_Print(0,("recvfrom returned %d errno %d\n", n, errno));

		if (n <= 0)
		{
			if(errno == EWOULDBLOCK)
				CM_DI_Print(1,("GetResp recvfrom timed out\n"));
			else
				CM_DI_Print(0,("GetResp recv from got errno %d \n", errno));
			return FALSE;
		}

		if (n > maxExpectedRespLen)        // Got a response, but not the one expected
			continue;

		if (VK_memcmp(&respMsg[SNMPREQID_OFFSET], reqMsgId, reqMsgIdLen) == 0)
		{
			// Got the right response!!
			*respMsgLen = n;
			return TRUE;
		}
	}
}

int GetResp_P(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
                char *reqMsg, int reqMsgLen, char *reqMsgId, int reqMsgIdLen,
                char *respMsg, int *respMsgLen, int maxExpectedRespLen)
{
	int n;

	while (1)
	{

		n = sendto(sockfd, reqMsg, reqMsgLen, 0, (void *) pservaddr, servlen);

		if (n <= 0)
		{
			CM_DI_Print(0,("GetResp_P sendto failed - returned %d errno %d\n", n, errno));
			return FALSE;
		}

		n = recvfrom(sockfd, respMsg, *respMsgLen, 0, NULL, NULL);
		// CM_DI_Print(0,("recvfrom returned %d errno %d\n", n, errno);

		if (n <= 0)
		{
			if(errno == EWOULDBLOCK)
				CM_DI_Print(0,("GetResp_P recvfrom timed out\n"));
			else
				CM_DI_Print(0,("GetResp_P recv from got errno %d \n", errno));
			return FALSE;
		}

		if (n > maxExpectedRespLen)        // Got a response, but not the one expected
			continue;

		//dump_hex(respMsg, n, 1);
		//CM_DI_Print(0,("Not my ID(%02x %02x %02x %02x)\n", reqMsgId[0], reqMsgId[1], reqMsgId[2], reqMsgId[3]));

		if (VK_memcmp(&respMsg[SNMPPRIVREQID_OFFSET], reqMsgId, reqMsgIdLen) == 0)
		{
			// Got the right response!!
			*respMsgLen = n;
			return TRUE;
		}
	}
}



unsigned int GetUInt(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset)
{
	int retVal = -1;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);

	if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + sizeof(long int)))
	{
		CM_DI_Print(0, ("GetUInt GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen);

	// if (asnCheckErrorStatus(recvline, n)) {
	if (asnCheckErrorStatus(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseUnsignedInt((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset);
		// CM_DI_Print(0,("asnParseUnsignedInt got = %d\n", retVal);
	}
	else
	{
		CM_DI_Print(0,("GetUInt asnParseUnsignedInt failed.\n"));
	}

	return retVal;
}


int GetInt(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset)
{
	int retVal = -1;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);

	if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + sizeof(long int)))
	{
		CM_DI_Print(0, ("GetInt GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen);

	if (asnCheckErrorStatus(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseInt((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset);
		// CM_DI_Print(0,("asnParseInt got = %d\n", retVal);
	}
	else
	{
		CM_DI_Print(0,("GetInt asnParseInt failed.\n"));
	}

	return retVal;
}

int GetInt_P(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset)
{
	int retVal = -1;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);

	if (FALSE == GetResp_P(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPPRIVREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + sizeof(long int)))
	{
		CM_DI_Print(0, ("GetInt_P GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen);

	if (asnCheckErrorStatus_P(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseInt((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset);
		// CM_DI_Print(0,("asnParseInt got = %d\n", retVal);
	}
	else
	{
		CM_DI_Print(0,("GetInt_P asnParseInt failed.\n"));
	}

	return retVal;
}



int SetDocsisDownload(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen)
{
	int retVal = -1;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);

	if (FALSE == GetResp_P(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPPRIVREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + sizeof(long int)))
	{
		CM_DI_Print(0, ("SetDocsisDownload GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen);


	if (asnCheckErrorStatus_P(recvline, recvlineLen))
	{
		CM_DI_Print(0,("SetDocsisDownload set Download Set OK.\n"));
		retVal = 1;
	}
	else
	{
		CM_DI_Print(0,("SetDocsisDownload set Download Set failed.\n"));
		retVal = 0;
	}

	return retVal;
}


unsigned long int GetIp(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset)
{
	unsigned long int retVal = 0;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);

	if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + sizeof(long int)))
	{
		CM_DI_Print(0, ("GetIp GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

	if (asnCheckErrorStatus(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseIp((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset);
		// CM_DI_Print(0,("asnParseIp got = %d\n", retVal));
	}
	else
	{
		CM_DI_Print(0,("GetIp asnParseUnsignedInt failed.\n"));
	}
	return retVal;
}


int GetMac(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset, unsigned char mac[])
{
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	int retVal = 0;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);
	int macLen = MAC_ADDR_LEN;

	if (FALSE == GetResp_P(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPPRIVREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + MAC_ADDR_LEN))
	{
		CM_DI_Print(0, ("GetMac GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

	if (asnCheckErrorStatus_P(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseString((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset, mac, &macLen);
		// CM_DI_Print(0,("asnParseMac got = %d bytes back\n", retVal));
		if (retVal != 6)
			return 0;
		if (macLen != 6)
			return 0;
	}
	else
	{
		CM_DI_Print(0,("GetMac asnParseUnsignedInt failed.\n"));
	}
#else
	int retVal = 0;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);
	int macLen = MAC_ADDR_LEN;

	if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + MAC_ADDR_LEN))
	{
		CM_DI_Print(0, ("GetMac GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

	if (asnCheckErrorStatus(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseString((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset, mac, &macLen);
		if (retVal != 6)
			return 0;
		if (macLen != 6)
			return 0;
			// CM_DI_Print(0,("asnParseMac got = %d bytes back\n", retVal));
	}
	else
	{
		CM_DI_Print(0,("GetMac asnParseUnsignedInt failed.\n"));
	}
#endif

	return retVal;
}


#if 1
int GetCmVer(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset, unsigned char output_string[], int *output_string_len)
{
	int retVal = 0;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);
	//int out_strLen = VK_strlen(output_string);

	if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + (*output_string_len)))
	{
		CM_DI_Print(0, ("GetCmVer GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

	//CM_DI_Print(0, ("GetCmVer : (%d) \n", *output_string_len));

	if (asnCheckErrorStatus(recvline, recvlineLen))
	{
		recvlineOffset = offset;
		retVal = asnParseString((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset, output_string, output_string_len);
		if (retVal == -1)
			return 0;
		if (retVal != (*output_string_len))
			return 0;
		// CM_DI_Print(0,("asnParseMac got = %d bytes back\n", retVal));
	}
	else
	{
		CM_DI_Print(0,("GetCmVer asnCheckErrorStatus failed.\n"));
	}
	    return retVal;
}

#else
int GetCmVer(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset, unsigned char cm_ver[])
{
  int retVal = 0;
  int   recvlineOffset;
  char  recvline[MAX_MSG_LEN + 1];
  int recvlineLen = sizeof(recvline);
  int cmVerLen = CM_VERSION_LEN;

  if (FALSE == GetResp(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
    &reqMsg[SNMPREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + CM_VERSION_LEN))
  {
    CM_DI_Print(0, ("GetResp() failed \n"));
    return retVal;
  }
    // CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

    if (asnCheckErrorStatus(recvline, recvlineLen)) {
      recvlineOffset = offset;
      retVal = asnParseString(recvline, recvlineLen, &recvlineOffset, cm_ver, &cmVerLen);
      if (retVal != 6)
        return 0;
      if (cmVerLen != 6)
        return 0;
      // CM_DI_Print(0,("asnParseMac got = %d bytes back\n", retVal));
    } else {
      CM_DI_Print(0,("asnCheckErrorStatus failed.\n"));
    }
        return retVal;
}
#endif

/* Private Mib*/
int GetString_P(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen,
           char *reqMsg, int reqMsgLen, int offset, unsigned char output_string[], int *output_string_len)
{
	int retVal = 0;
	int   recvlineOffset;
	char  recvline[MAX_MSG_LEN + 1];
	int recvlineLen = sizeof(recvline);
	//int out_strLen = VK_strlen(output_string);

	if (FALSE == GetResp_P(sockfd, pservaddr, servlen, reqMsg, reqMsgLen,
		&reqMsg[SNMPPRIVREQID_OFFSET], 4, recvline, &recvlineLen, reqMsgLen + (*output_string_len)))
	{
		CM_DI_Print(0, ("GetString_P GetResp() failed \n"));
		return retVal;
	}
	// CM_DI_Print(0, ("GetResp() got %d bytes \n", recvlineLen));

	//CM_DI_Print(0, ("GetString_P : (%d) \n", *output_string_len));

	if (asnCheckErrorStatus_P(recvline, recvlineLen))  	/* Private Mib */
	{
		recvlineOffset = offset;
		retVal = asnParseString((unsigned char *)recvline, recvlineLen, (unsigned int *)&recvlineOffset, output_string, output_string_len);
		if (retVal == -1)
			return 0;
		if (retVal != (*output_string_len))
			return 0;
			// CM_DI_Print(0,("asnParseMac got = %d bytes back\n", retVal));
	}
	else
	{
		CM_DI_Print(0,("GetString_P asnCheckErrorStatus failed.\n"));
	}

	return retVal;
}

void doEcho(int sockfd, struct sockaddr_in *pcliaddr, socklen_t clilen)
{
	int				n, ret;
	socklen_t		len;
	char			mesg[ECHO_MAX_MSG_LEN];
	unsigned int 	echoLength, echoCount;
//	unsigned int	numOfEchoBytes;

	CM_DI_Print(0,("doEcho Entering receive loop\n"));

	for ( ; ; )
	{
		len = clilen;
		n = recvfrom(sockfd, mesg, ECHO_MAX_MSG_LEN, 0, pcliaddr, &len);

		if (n < 0)
		{
			CM_DI_Print(0,("doEcho recvfrom error!\n"));
		}
		else
		{
			// Display magic number.
			CM_DI_Print(0,("\nmagic #1 = 0x%02x\n", mesg[0]));
			CM_DI_Print(0,("magic #2 = 0x%02x\n", mesg[1]));

			/* check magic number */
		        if ( (mesg[0] != MAGIC_NUMBER_FIRST_BYTE) ||
					 (mesg[1] != MAGIC_NUMBER_SECOND_BYTE) )
			{
				CM_DI_Print(0,("ERROR - Invalid magic number: %02x  %02x\n", mesg[0], mesg[1]));
				continue;
			}

			/* Read echo length. */
			echoLength = mesg[3];
			echoLength <<= 8;
			echoLength += mesg[2];

			// Display echo length.
			CM_DI_Print(0,("echoLength = %d	received_length = %d\n", echoLength, n));

			// Validate.
			if ((echoLength < ECHO_MIN_MSG_LEN) || (echoLength > ECHO_MAX_MSG_LEN))
			{
				CM_DI_Print(0,("ERROR - Invalid echo length: %d (must be between 18 and 1466)\n", echoLength));
				continue;
			}

			// Validate
		        if (echoLength > (unsigned int)n)
			{
				CM_DI_Print(0,("ERROR - Echo length %d can't be bigger than the length of the received packet %d\n", echoLength, n));
				continue;
			}

			// Read echo count.
			echoCount = mesg[5];
			echoCount <<= 8;
			echoCount += mesg[4];

			// Display echo count.
			CM_DI_Print(0,("echoCount = %d\n", echoCount));

			if (echoCount == 0 )
			{
				CM_DI_Print(0,("Echo count is %d.\n", echoCount));
				continue;
			}

		        mesg[6] ^= 0xff;	/* ones complement first two payload bytes */
		        mesg[7] ^= 0xff;
#if 0
			for (numOfEchoBytes=0; numOfEchoBytes <= n; numOfEchoBytes++)
			{
				CM_DI_Print(0,(" 0x%x ", mesg[numOfEchoBytes]));
			}
#endif
			for (;echoCount > 0; echoCount--)
			{
				ret = sendto(sockfd, mesg, n, 0, pcliaddr, echoLength);

				if (ret <= 0)
				{
					CM_DI_Print(0,("doEcho : sendto errno %d\n", ret));
					break;
				}
			}
		}
	}
}

void EchoTest(void)
{
	int sockfd;
	int retVal = -1;
	struct sockaddr_in	servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		CM_DI_Print(0,(" socket error!\n"));
	}
	else
	{
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family      = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port        = htons(EURODOCSIS_ECHO_PORT);

		retVal = bind(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr));
		if (retVal < 0)
		{
			CM_DI_Print(0,(" bind error!\n"));
		}
		else
		{
			doEcho(sockfd, (struct sockaddr_in *) &cliaddr, sizeof(cliaddr));
		}
		close(sockfd);
	}
}

#endif
