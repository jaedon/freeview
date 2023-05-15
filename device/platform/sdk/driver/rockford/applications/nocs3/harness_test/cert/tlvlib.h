/*==============================================================================
 tlvlib.h
============================================================================== */
#include "types.h"

int tlvCalcTagLenLen(U32 xLen);
int tlvCalcTlvLen(U32 xLen);
int tlvPutTagLen(U8 *xpBuf,U32 xBufSize,U8 xTag,U32 xLen);
int tlvPutTlv(U8 *xpBuf,U32 xBufSize,U8 xTag,U32 xLen,void *xpVal);
int tlvGetLen(U32 *xpValLen,U32 *xpLenLen,U8 *xpBuf,U32 xBufSize);
int tlvChkSeq(U8 *xpBuf,U32 xBufLen);
int tlvChkFirstSeq(U8 *xpBuf,U32 xBufLen, int *xpTlvCnt);
int tlvGetTlv(U8 *xpTag,U32 *xpLen,U8 **xppVal,U8 *xpBuf,U32 xBufLen);
int tlvGetExpectedTlv(U8 xpTag,int xLen,U8 **xppVal,U8 *xpBuf,U32 xBufLen);

/*** end of file ***/

