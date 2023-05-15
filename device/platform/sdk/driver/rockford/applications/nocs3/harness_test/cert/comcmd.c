/*============================================================================*\
 * File        : comcmd.c
 * Description : Reference code for CERT test harness application.
 * Warning     : This code assumes a little endian 32-bit architecture.
 * 
 * Classification level: Confidential
 * 
 * Copyright (c) 2011 - 2012 Nagravision SA. All rights reserved.  
 * NAGRA and NAGRAVISION are registered trademarks of Kudelski SA, 1033 Cheseaux, Switzerland.                              
 * Confidential and privileged information.
 \*============================================================================*/

/* Add the define below to make popen and pclose functions available */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <arpa/inet.h> /* for htonl and ntohl */

/*#include "global.h"*/
#include "tlv.h"
#include "tlvlib.h"

#include "toollib.h"


#include "cert_bridge.h"



/*----------------------------------------------------------------------------*\
|* Process error
|* Writes error TLV in xpOutBuf 
|* Returns the number of bytes written to xpOutBuf if succeeded else <0 if not
\* ---------------------------------------------------------------------------*/
int comErr(U8 xErr, U8 *xpOutBuf, U32 xBufLen, char *xFile, int xLineNb)
{
	int stat;
	syslog(LOG_ERR, "Error 0x%02x while processing buffer @ %s:%d.", xErr, xFile, xLineNb);
	stat = tlvPutTlv(xpOutBuf, xBufLen, TAG_STATUS, 1, &xErr);
	return stat;
}



/*----------------------------------------------------------------------------*\
|* Execute a system command 
|* Returns the number of bytes written to xpOutBuf.
\* ---------------------------------------------------------------------------*/
int comSystemCmd(U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen)
{
	U8 	 status; 	/*  overall command status  */
	int  stat; 		/*  local functions status */
	int  bo; 		/*  nb bytes written in xpOutBuf */
	U32  bi; 		/*  nb bytes read from xpArgs */
	int  m,n,p,q;
	char *pDin;     /* command to execute */
	int  cmdLen;
	char cmd[0x10000];
	int  outLen;
	char outBuf[0x10000];  /*  max pkt length for the moment is 65535 */
	char str[1000]; 
	FILE *fp;
	char *r,*s;
	int  t;
	
	/* Arguments processing */
	/* Check tag but accept any length */
	stat = tlvGetExpectedTlv(TAG_IN,-1,(U8 **)&pDin, xpArgs, xArgLen);
	if (stat < 0) 
	{
		printf("tlvGetExpectedTlv status=%d\n",stat);			
		return comErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	}
	bi = stat;
	
	if ( bi != xArgLen ) 
		return comErr(ERR_PARAM_NUMBER, xpOutBuf, xBufLen, __FILE__, __LINE__);

	/* Find command length, and protect against overflow */
	if (bi>(sizeof(cmd)-1))
		cmdLen=(sizeof(cmd)-1);
	else
		cmdLen=bi;
	memcpy(cmd,pDin,cmdLen); /* copy command */
	cmd[cmdLen]=0; /* ensure null termination */

#if (0)	
	printf("cmdLen=%u\n",cmdLen);
	printf("cmd=");
	for (n=0; n<cmdLen; n++)
		printf("%02X ",cmd[n]);
	printf("\n");
	printf("cmd=%s\n",cmd);	
#endif
	
	fp=popen(cmd,"r");
	if (fp==NULL)
	{
		printf("ERROR: Could not execute command %s\n",pDin);
		return comErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__); 
	}
	r=outBuf;
	n=sizeof(outBuf)-1; /* max number of characters to accept */
	for (; n>0; n-=m,r+=m)
	{
		s=fgets(str,sizeof(str)-1,fp);
		if (s==NULL)
			break;
		m=strlen(str);
		/* make sure we don't append too much */
		if (m>n)
			m=n;
		memcpy(r,str,m); /* append sub-string */
	}
	*r=0;	/* null terminate string */
	pclose(fp);
	outLen=(int)(r-outBuf);
	outLen++; /* include null termination in response */
	status=0;
	
#if (0)	
	if (outLen>1)
		printf("Command response: %s\n",outBuf);
	else
		printf("No command response\n");
	printf("Response length including null termination=%d\n",outLen);
	printf("outBuf=");
	for (n=0; n<outLen; n++)
		printf("%02X ",outBuf[n]);
	printf("\n");
#endif
	
	/*  Prepare response */
	m=tlvCalcTlvLen(sizeof(U8));	/*  length of status TLV (here: m==3) */
	p=tlvCalcTlvLen(outLen);		/*  length of data out TLV */
	q=m+p;
	n=tlvCalcTagLenLen(q);			/*  length of TAG_SYSTEM_CMD tag+len */
	bo=n+q;
	
	stat=tlvPutTagLen(xpOutBuf,xBufLen,TAG_SYSTEM_CMD,q);
	if (stat<0) 
		return comErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_STATUS,sizeof(U8),&status);
	if (stat<0) 
		return comErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);
	t+=stat;
	stat=tlvPutTlv(xpOutBuf+t,xBufLen-t,TAG_OUT,outLen,outBuf);
	if (stat<0) 
		return comErr(ERR_PARAM_TYPE, xpOutBuf, xBufLen, __FILE__, __LINE__);

#if (0)
	printf("Response length=%02X\n",bo);
	printf("xpOutBuf=");
	for (n=0; n<(bo+5); n++)
		printf("%02X ",xpOutBuf[n]);
	printf("\n");
#endif
	
	return bo;
}



/*** end of file ***/
