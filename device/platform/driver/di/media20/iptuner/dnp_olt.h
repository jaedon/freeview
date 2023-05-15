/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   dnp_olt.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Fri Apr 17 03:08:21 KST 2009 $
 * File Description:	     Download Offset Lookup Table Manager 
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DNP_OLT_H__
#define __DNP_OLT_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "vfio20.h"
#include "di_iptuner_config.h"
/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	DNP_OLT_SEEK_FOR_READ=0,
	DNP_OLT_SEEK_FOR_WRITE
}DNP_OLT_SEEK_TYPE;

typedef struct 
{
	HINT8 	*pTable;
	HINT8	*pSaveFileName;
	HINT32	nReadFd;
	HINT32	nWriteFd;
	HULONG	ulOLTWriteSema;
	
	HINT32	nOLTChunkExponent;		/* the base 2 and the exponent value : ex 3 ==> (2^3) : KBytes*/
	HINT32	nOLTChunkBytes;			/* Bytes */
	HINT32	nOLTShiftCount;			/* number */
	HINT32	nOLTSize;				/* Bytes */

	HINT64	llWritingByteOffset;	/* Bytes */
	HINT64	llJumpRequestOffset;
	HINT64	llReadingByteOffset;	/* Bytes */
	HINT64	llTotalDataBytes;		/* Bytes */
	HINT64	llSumOfWriting;			/* Bytes */
	HINT64	llFragmented;			/* fragmented number */

	HUINT64	ulEndOffset;			/* Bytes */

	HUINT64 ullDataReadOffset;
	HUINT64 ullIndexReadOffset;

	/* speed Parameters to calculate the buffering speed */
	HBOOL	bSpeedGun;				/* flag of speed calculation */
	HULONG	ulStartClock;			/* milliseconds */
	HULONG  ulDelayedStartClock;    /* milliseconds */
	HULONG	ulOldClock;				/* milliseconds */
	HINT32	nWritingSpeedPerSecond;	/* Bytes */
	HINT32	nWritingSpeedPerTwoSecond;	/* Bytes */
	HINT64  llOldSum;

	HBOOL	bRequestJump;
	HBOOL	bComplete;
} DNP_OLT_t ;
/* End typedef */

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
DNP_OLT_t *DNP_OLT_Create (HINT32 nChunkBase, HINT64 llContentLength, HINT8 *pSavePath);
HBOOL	DNP_OLT_Destroy (DNP_OLT_t *pOLT);
HBOOL	DNP_OLT_CheckForJumping (DNP_OLT_t *pOLT);
HINT32	DNP_OLT_GetOneChunkSize (DNP_OLT_t *pOLT);
HBOOL	DNP_OLT_IsFilled (DNP_OLT_t *pOLT, HINT64 llOffset, HINT64 llSize);
void 	DNP_OLT_PrintInformation (DNP_OLT_t *pOLT);
HBOOL	DNP_OLT_SetBufferingPosition(DNP_OLT_t *pOLT, HINT64 llOffset);
HBOOL	DNP_OLT_UpdateBufferingPosition (DNP_OLT_t *pOLT, HINT64 llOffset, HINT64 llSize);
HBOOL	DNP_OLT_UpdateReadingPosition (DNP_OLT_t *pOLT, HINT64 llOffset);
HINT64	DNP_OLT_GetBufferingPosition (DNP_OLT_t *pOLT);
HINT32	DNP_OLT_GetBufferingRate (DNP_OLT_t *pOLT);
HBOOL	DNP_OLT_Write (DNP_OLT_t *pOLT, HINT8 *pBuf, HINT64 llSize);
HINT64 DNP_OLT_Seek(DNP_OLT_t *pOLT, HINT64 offset, HINT32 whence, DNP_OLT_SEEK_TYPE type);
HINT64	DNP_OLT_Read (DNP_OLT_t *pOLT, HINT8 *pBuf, HINT64 llSize);
HBOOL	DNP_OLT_CheckReachedEOF (DNP_OLT_t *pOLT);
HINT64	DNP_OLT_GetNextEmptyPosition (DNP_OLT_t *pOLT, HINT64 llOffset);
HINT64	DNP_OLT_GetBufferedBytes (DNP_OLT_t *pOLT);
HINT32	DNP_OLT_BufferFlush (DNP_OLT_t *pOLT);

/* not implemented yet */
HBOOL	DNP_OLT_SetEndOffset (DNP_OLT_t *pOLT, HUINT64 ulEndOffset);
HBOOL	DNP_OLT_GetFirstOffsetOfEntry (DNP_OLT_t *pOLT, HINT64 llOffset);

HINT32	DNP_OLT_GetEntryByOffset (DNP_OLT_t *pOLT, HINT64 llOffset);

/* speed calculation */
HBOOL   DNP_OLT_InitSpeedGun (DNP_OLT_t *pOLT);
HBOOL   DNP_OLT_UpdateSpeedGun (DNP_OLT_t *pOLT);
HBOOL	DNP_OLT_UninitSpeedGun (DNP_OLT_t *pOLT);
HINT64 DNP_OLT_GetBufferedDataSize(DNP_OLT_t *pOLT);
HINT64 DNP_OLT_GetDataReadOffset(DNP_OLT_t *pOLT);
HBOOL DNP_OLT_SetDataReadOffset(DNP_OLT_t *pOLT, HUINT64 ullOffset);
HINT64	DNP_OLT_GetJumpRequestPosition(DNP_OLT_t *pOLT);
void	DNP_OLT_SetJumpRequestPosition(DNP_OLT_t *pOLT, HINT64 offset);
void DNP_OLT_RegisterStartTimeMsec (DNP_OLT_t *pOLT);
void DNP_OLT_UnRegisterStartTimeMsec (DNP_OLT_t *pOLT);
HUINT64 DNP_OLT_GetDownloadedBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset);
HUINT64 DNP_OLT_GetEmptyBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset);

#ifdef  __cplusplus
}
#endif

/* End global function prototypes */
#endif /* ! __DNP_OLT_H__ */
