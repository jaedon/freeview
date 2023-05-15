/*==============================================================================
 tlvlib.cpp
============================================================================== */

#include <string.h>
#include "tlvlib.h"
#include "types.h"



/*------------------------------------------------------------------------------
 Calculate TAG+LEN length
------------------------------------------------------------------------------ */
int tlvCalcTagLenLen(U32 xLen)
{
	if (xLen<0x80)
		return 2;	/* length of TAG + LEN  */
	else if (xLen<0x100)
		return 3;	/* length of TAG + LEN  */
	else 
		return 4;	/* length of TAG + LEN  */
}



/*------------------------------------------------------------------------------
 Calculate TLV length
------------------------------------------------------------------------------ */
int tlvCalcTlvLen(U32 xLen)
{
	U32 len;

	len=tlvCalcTagLenLen(xLen);
	len+=xLen;
	return len;
}



/*------------------------------------------------------------------------------
 Put TAG+LEN into buffer
 Return TAG+LEN length, or <0 in case of error
 In case of buffer size check, make sure there is room for an End Marker
------------------------------------------------------------------------------ */
int tlvPutTagLen(U8 *xpBuf,U32 xBufSize,U8 xTag,U32 xLen)
{
	U8	*p;
	U32 len;

	/*  Calculate length of TAG + LEN */
	len=tlvCalcTagLenLen(xLen);

	if ((xBufSize>0)&&((len+xLen+2)>xBufSize))
		return -1;
	
	/*  Put TAG */
	p=xpBuf;
	*p++=xTag;

	/* Put LEN */
	if (xLen<0x80)
		*p++=(U8)xLen;
	else if (xLen<0x100)
	{
		*p++=0x81;
		*p++=(U8)xLen;
	}
	else 
	{
		*p++=0x82;
		*p++=(U8)(xLen>>8);
		*p++=(U8)xLen;
	}

	return len;
}



/*------------------------------------------------------------------------------
 Put TLV into buffer
 Return TLV length, or <0 in case of error
------------------------------------------------------------------------------ */
int tlvPutTlv(U8 *xpBuf,U32 xBufSize,U8 xTag,U32 xLen,void *xpVal)
{
	U8	*p;
	int len;

	/* Put TAG + LEN  */
	len=tlvPutTagLen(xpBuf,xBufSize,xTag,xLen);
	if (len<0)
		return -1;

	if (xLen==0)
		return len;
	/* Put VAL */
	p=xpBuf+len;
	memcpy(p,xpVal,xLen);

	/* Return TLV length (not including End Marker) */
	len+=xLen;
	return len;
}



/*------------------------------------------------------------------------------
 Get TLV LEN and the length of the TLV LEN field 
------------------------------------------------------------------------------ */
int tlvGetLen(U32 *xpValLen,U32 *xpLenLen,U8 *xpBuf,U32 xBufSize)
{
	U32 len;
	U32	rest;

	*xpValLen=0;
	*xpLenLen=0;
	if (xBufSize==0)
		return -1;
	if (xBufSize>(3+0xFFFF))
		return -2;
	len=xpBuf[0];
	
	if (len<0x80)
	{
		rest=xBufSize-1;
		if (len>rest)
			return -3;
		*xpValLen=len;
		*xpLenLen=1;
		return 0;
	}
	if (len==0x81)
	{
		len=xpBuf[1];
		rest=xBufSize-2;
		if (len>rest)
			return -4;
		*xpValLen=len;
		*xpLenLen=2;
		return 0;
	}
	if (len==0x82)
	{
		len=(((U32)xpBuf[1])<<8)+xpBuf[2];
		rest=xBufSize-3;
		if (len>rest)
			return -5;
		*xpValLen=len;
		*xpLenLen=3;
		return 0;
	}
	return -6;
}



/*------------------------------------------------------------------------------
 Check TLV sequence
 Return TLV count, or <0 if error
------------------------------------------------------------------------------ */
int tlvChkSeq(U8 *xpBuf,U32 xBufLen)
{
	U8 *p;
	U32 i;
	int rest;
	U8	tag;
	int	stat;
	U32 valLen;
	U32 lenLen;
	U32 tlvLen;
	int tlvCnt;

	if (xBufLen<2)
		return -1; /* This sequence is too short to make sense */
	tlvCnt=0;
	for (i=0,p=xpBuf; i<xBufLen; i+=tlvLen,p+=tlvLen)
	{
		rest=xBufLen-i;
		if (rest<1)
			return -2; /* a TAG without LEN is illegal */
		tag=*p;
		if ((tag==0x00)||(tag==0xFF))
			return -3;
		stat=tlvGetLen(&valLen,&lenLen,p+1,rest-1);
		if (stat<0)
			return -4;
		tlvLen=1+lenLen+valLen;
		tlvCnt++;
	}
	if (i!=xBufLen)
		return -5;
	return tlvCnt;
}

/*------------------------------------------------------------------------------
 Check TLV sequence up to first end TLV marker
 Return nb of bytes in the sequence or <0 if error
 when xpTlvCnt is not NULL  contains  TLV count, 
------------------------------------------------------------------------------ */
int tlvChkFirstSeq(U8 *xpBuf,U32 xBufLen, int *xpTlvCnt)
{
	U8 *p;
	U32 i;
	int rest;
	U8	tag=0;
	int	stat;
	U32 valLen;
	U32 lenLen;
	U32 tlvLen;
	int tlvCnt;

	if (xBufLen<2)
		return -1; /* This sequence is too short to make sense */
	tlvCnt=0;
	for (i=0,p=xpBuf; i<xBufLen && tag != 0x0e; i+=tlvLen,p+=tlvLen)
	{
		rest=xBufLen-i;
		if (rest<1)
			return -2; /* a TAG without LEN is illegal */
		tag=*p;
		if ((tag==0x00)||(tag==0xFF))			
			return -3;
		stat=tlvGetLen(&valLen,&lenLen,p+1,rest-1);
		if (stat<0)
			return -4;
		tlvLen=1+lenLen+valLen;
		tlvCnt++;
	}
	
	if (xpTlvCnt != NULL)
		*xpTlvCnt = tlvCnt;
	
	if (i>xBufLen)
		return -5;
	if (tag != 0x0e)
		return -6; /* last tag was not 0x0e */
		
	return i;
}


/*------------------------------------------------------------------------------
 Get TLV's TAG, LEN and VAL
 Return TLV len if OK, otherwise <0 if error
------------------------------------------------------------------------------ */
int tlvGetTlv(U8 *xpTag,U32 *xpLen,U8 **xppVal,U8 *xpBuf,U32 xBufLen)
{
	U8	tag;
	int	stat;
	U32 valLen;
	U32 lenLen;
	int	tlvLen;

	if (xBufLen<2)
		return -1; /* a TAG without LEN is illegal */
	tag=xpBuf[0];
	if ((tag==0x00)||(tag==0xFF))
		return -2;
	stat=tlvGetLen(&valLen,&lenLen,xpBuf+1,xBufLen-1);
	if (stat<0)
		return -3;
	*xpTag=tag;
	*xpLen=valLen;
	*xppVal=xpBuf+1+lenLen;
	tlvLen=1+lenLen+valLen;
	return tlvLen;
}

/*------------------------------------------------------------------------------
 Get expected TLV value
 Return TLV len if OK, otherwise <0 if error
------------------------------------------------------------------------------ */
int tlvGetExpectedTlv(U8 xpTag,int xLen,U8 **xppVal,U8 *xpBuf,U32 xBufLen)
{
	U8 tag;
	U32 valLen;
	int stat;
	
	stat = tlvGetTlv(&tag, &valLen, xppVal, xpBuf, xBufLen);
	if (stat<0)
		return -1;
	
	if (tag != xpTag)
		return -2;
	
	if (xLen>=0)
		if (valLen != xLen)
			return -3;
	
	return stat;
}

	
/* END OF FILE */
