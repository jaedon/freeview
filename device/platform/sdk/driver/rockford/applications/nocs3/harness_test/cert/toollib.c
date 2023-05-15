
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"


/* -------------------------------------------------------------------------- */
/* Display buffer                                                                */
/* -------------------------------------------------------------------------- */
int display_buffer(U8 *xpBuf, U32 xBufLen) 
{
	const U32 lwidth = 80;
	char strBuf[lwidth+3];
	char *p;
	U32 i;
	int ret;
	if (xBufLen < (lwidth/2)) {
		for(i=0, p=strBuf; i<xBufLen; i++, p+=ret) 
			ret = sprintf(p, "%02x", xpBuf[i]);
		*p = '\n';
		p++;
		*p = 0; /* end string with null char */
		
	} else {
		for(i=0, p=strBuf; i<(lwidth/4)-1; i++, p+=ret) 
			ret = sprintf(p, "%02x", xpBuf[i]);
		ret = sprintf(p, " ... ");
		p+=ret;
		for(i=xBufLen-(lwidth/4)+1; i<xBufLen; i++, p+=ret) 
			ret = sprintf(p, "%02x", xpBuf[i]);
		*p = '\n';
		p++;
		*p = 0; /* end string with null char */
	}
	syslog(LOG_DEBUG, strBuf);
	return 0;
}

int display_msg_and_buf(int priority, const char *msg, U8 *xpBuf, U32 xBufLen)
{
	const U32 lwidth = 80;
	char strBuf[2*lwidth]; /* size doubled for safety against buffer overflows */
	char *p;
	U32 i;
	int ret;
		
	/* print message */
	p=strBuf;
	ret = sprintf(p, "%s ", msg);
	p+=ret;
		
	if (xpBuf == NULL) {
		ret = sprintf(p, "*NULL*\n");
		p+=ret;
	} else if (xBufLen < ((lwidth/2)-10)) {
		for(i=0; i<xBufLen; i++, p+=ret) 
			ret = sprintf(p,  "%02x", xpBuf[i]);
		ret=sprintf(p," (%dB)\n",xBufLen);
		p+=ret;
		*p = 0; /* end string with null char */
			
	} else {
		for(i=0 ; i<(lwidth/4)-1; i++, p+=ret) 
			ret = sprintf(p, "%02x", xpBuf[i]);
		ret = sprintf(p, " ... ");
		p+=ret;
		for(i=xBufLen-(lwidth/4)+1; i<xBufLen; i++, p+=ret) 
			ret = sprintf(p, "%02x", xpBuf[i]);
		ret=sprintf(p," (%dB)\n",xBufLen);
		p+=ret;
		*p = 0; /* end string with null char */
	}
	if(priority > LOG_DEBUG) 
	{
		syslog(LOG_DEBUG, strBuf);
		fprintf(stderr, "%s", strBuf);
	}
	else
		syslog(priority, strBuf);
	return 0;
}


/*----------------------------------------------------------------------------*\
|* read null terminated hex string from xStr and stores bytes in xpOut 
|* 
|* return number of bytes written (max xOutLen)
|*        <0 on error
\*----------------------------------------------------------------------------*/
int readHexStr(const char *xStr, U8 *xpOut, int xOutLen)
{
	int i,bout,len;
	U32 val;
	len=strlen(xStr);
	for(i=0,bout=0; i<len && bout<xOutLen; i+=2, bout++) {
		if( sscanf(xStr+i, "%02x", &val) != 1 ) /* should exactly match 2 characters */
			return -i-100;	
		*(xpOut+bout) = val & 0xff;
    }
    return bout;
}
