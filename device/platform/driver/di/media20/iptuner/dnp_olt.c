/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dnp_olt.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     IP tuner Main API
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2010 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>

#include "di_iptuner_config.h"
#include "di_uart.h"
#include "htype.h"
#include "vkernel.h"

#include "di_iptuner20.h"
#include "dnp_olt.h"
#include "util_clock.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#define OLT_DEBUGGING 0
//#define DNP_OLT_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DNP_OLT_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif

#define UNUSED_PARAM(x)  ((void)x)

#if 0 /* <errno.h> */
extern int errno;
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
HBOOL P_DNP_OLT_IsFilled (DNP_OLT_t *pOLT, HINT32 nEntryNumber)
{
	HINT8 *pEntryByte=NULL;

	/* get OLT offset */
	pEntryByte = pOLT->pTable + (nEntryNumber>>3);

	/* masking requested oltbit and olt-entrybit */
	return (*pEntryByte & (1 << (nEntryNumber & 7)));
}

HBOOL P_DNP_OLT_Marking (DNP_OLT_t *pOLT , HINT64 llOffset)
{
	HINT8	*pEntryByte=NULL;
	HINT32	nEntryNumber=0;

	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
	if (pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table pointer. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* get Entry Byte */
	nEntryNumber = (HINT32) (llOffset >> pOLT->nOLTShiftCount);
	pEntryByte = pOLT->pTable + (nEntryNumber >> 3);

#if 0 //OLT_DEBUGGING
	PrintDebug ("%s(%d) Debug> Marking Entry Byte Number: %d Hex:0x%x Bit %d \n"
	,__FUNCTION__
	,__LINE__
	, nEntryNumber
	, pEntryByte[0]
	,(nEntryNumber & 0x7));

	if ( (*pEntryByte) & (1 << (nEntryNumber & 0x7)) )
	{
		PrintError ("%s(%d) Error> Duplicated !! Need to more DEBUGGING (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
#endif

	/* Okay!! update entry bit */
	*pEntryByte |= (1 << (nEntryNumber & 0x7) ) ;

	PrintDebug("pOLT->nOLTSize: %d bytes and marking %d bit in %dth byte\n", pOLT->nOLTSize, nEntryNumber & 0x7,(nEntryNumber >> pOLT->nOLTChunkExponent));

	return TRUE;
}

/* ret == -1 : error
 * ret >=  0 : searched entry
 * ret == -2 : fullfilled buffer
 */
HINT32 P_DNP_OLT_SearchNextEmptyEntry (DNP_OLT_t *pOLT, HINT64 llOffset)
{
	HINT32 i=0;
	HINT32	nEntryCurNumber=0;
	HINT32	nEntryEndNumber=0;

	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/* get OLT table index : bit offset*/
	nEntryCurNumber =  llOffset >> pOLT->nOLTShiftCount ;
	nEntryEndNumber =  ((pOLT->llTotalDataBytes-1) >> pOLT->nOLTShiftCount) ;

	PrintDebug ("%s(%d) Debug> Search nEntryCurNumber (%d) ~ nEntryEndNumber (%d)\n"
	,__FUNCTION__
	,__LINE__
	,nEntryCurNumber
	,nEntryEndNumber);

	/* searching from cur to endofstream */
	for (i=nEntryCurNumber; i<=nEntryEndNumber ; i++)
	{
		if ( FALSE == P_DNP_OLT_IsFilled (pOLT,i))
		{
#if 0 /* test */
			PrintError ("%s(%d) Debug> entry (%d) isn't filled.!!!\n",__FUNCTION__,__LINE__,i);
#endif
			return i;
		}
	}

#if 0 /* searching from begin of stream */
	/* already filled by endofstream */
	/* TODO : again from first entry ???? */
	for (i=0;i<nEntryCurNumber;i++)
	{
		if ( FALSE == P_DNP_OLT_IsFilled (pOLT,i))
		{
			return i;
		}
	}
#endif

	PrintError ("%s(%d) Info> The content was full filled from PP to EndOffset in OLT !!\n",__FUNCTION__,__LINE__);
	return -2;
}

HUINT64 P_DNP_OLT_GetDownloadedBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset)
{
	HUINT32 uiIndex=0;
	HUINT32 uiFirstChunk=0;
	HUINT32 uiMaxChunk=0;
	HUINT32 uiDownloadedChunks=0;
	HUINT64 ullDownloadedBytes=0;

	uiFirstChunk = ullOffset >> pOLT->nOLTShiftCount;
	uiMaxChunk = pOLT->llTotalDataBytes >> pOLT->nOLTShiftCount;
	for(uiIndex=uiFirstChunk; uiIndex<uiMaxChunk; uiIndex++)
	{
		if(pOLT->pTable[uiIndex/8] & (1<<uiIndex%8))
		{
			uiDownloadedChunks++;
		}
	}
	ullDownloadedBytes = (HUINT64)uiDownloadedChunks<<pOLT->nOLTShiftCount;
	PrintDebug("ullDownloadedBytes: %llu Bytes\n",ullDownloadedBytes);

	return ullDownloadedBytes;
}

HUINT64 P_DNP_OLT_GetEmptyBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset)
{
	HUINT32 uiIndex=0;
	HUINT32 uiFirstChunk=0;
	HUINT32 uiMaxChunk=0;
	HUINT32 uiEmptyChunks=0;
	HUINT64 ullEmptyBytes=0;

	uiFirstChunk = ullOffset >> pOLT->nOLTShiftCount;
	uiMaxChunk = pOLT->llTotalDataBytes >> pOLT->nOLTShiftCount;
	for(uiIndex=uiFirstChunk; uiIndex<uiMaxChunk; uiIndex++)
	{
		if(!(pOLT->pTable[uiIndex/8] & (1<<uiIndex%8)))
		{
			uiEmptyChunks++;
		}
	}
	ullEmptyBytes = (HUINT64)uiEmptyChunks<<pOLT->nOLTShiftCount;
	PrintDebug("ullEmptyBytes: %llu Bytes\n",ullEmptyBytes);

	return ullEmptyBytes;
}

/* Warning !
 * 	If you use the XFS as streaming partition,
 * 	should not add the O_SYNC flag in save-file when you try to open to store.
 * 	Because it makes overhead to allocate each chunk in XFS.
 *
 * 	If you know more detail description about this issue,
 * 	please refer to below link (Optional Realtime Allocator)
 * 		http://oss.sgi.com/projects/xfs/
 */

#if __________BASE__________
#endif
DNP_OLT_t * DNP_OLT_Create (HINT32 nChunkExponent, HINT64 nContentLength, HINT8 *pSavePath)
{
	HINT32  nVKRet=0;
	DNP_OLT_t *pOLT=NULL;
	HINT32	nLen=0;
	HINT32 flags = O_CREAT|O_TRUNC|O_RDWR;

	PrintEnter;

	PrintDebug ("%s(%d) Debug> nChunkExponent %d, nContentLength %"PRId64", Path [%s]\n"
		,__FUNCTION__
		,__LINE__
		,nChunkExponent
		,nContentLength
		,pSavePath);

	if(pSavePath == NULL)
	{
		PrintError("%s(%d) Error> invalid file path \n",__FUNCTION__,__LINE__);
		return NULL;
	}

	/* we need only file path which can know in posix function.*/
	if((NULL == VK_strstr((char*)pSavePath,"dvr://"))
		&& (NULL == VK_strstr((char*)pSavePath, "file://"))
		&& (NULL != VK_strstr((char*)pSavePath,"://")) )
	{
		PrintError("%s(%d) Error> invalid file path \n",__FUNCTION__,__LINE__);
		return NULL;
	}

	if( ( (1<<nChunkExponent) > nContentLength) || (nContentLength == 0))
	{
		PrintError ("%s(%d) Error> invalid input parameter \n",__FUNCTION__,__LINE__);
		return NULL;
	}

	pOLT = (DNP_OLT_t*) VK_MEM_Alloc (sizeof(DNP_OLT_t));
	if(pOLT==NULL)
	{
		PrintError ("%s(%d) Error > Offset Table VK_malloc fail !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	VK_MEM_Memset (pOLT,0x0,sizeof(DNP_OLT_t));

	pOLT->pSaveFileName = (HINT8 *)strndup ((char *)pSavePath , (size_t)VK_strlen(pSavePath)+1);

	/* get system path */
	if ( NULL != VK_strstr((char*)pSavePath,"dvr://") )
	{
		nLen = VK_strlen("dvr://");
	}
	else if (NULL != VK_strstr((char*)pSavePath, "file://") )
	{
		nLen = VK_strlen("file://");
	}
	else if (NULL == VK_strstr((char*)pSavePath, "://") )
	{
		nLen = 0;
	}
	else
	{
		/* already checked a SavePath has valid prefix. */
		PrintError("%s(%d) Error> (%s) is invalid file path \n",__FUNCTION__,__LINE__,pSavePath);
		VK_free(pOLT->pSaveFileName);
		VK_MEM_Free(pOLT);
		return NULL;
	}

	pOLT->nReadFd = open ((char*)&pOLT->pSaveFileName[nLen], flags, 0666);
	if (pOLT->nReadFd < 0)
	{
		PrintError ("%s(%d) Error> open faile for DNP Filename[%s]!!\n",__FUNCTION__,__LINE__,pSavePath);
		VK_free(pOLT->pSaveFileName);
		VK_MEM_Free(pOLT);
		return NULL;
	}

	pOLT->nWriteFd = open ((char*)&pOLT->pSaveFileName[nLen], flags, 0666);
	if (pOLT->nWriteFd < 0)
	{
		PrintError ("%s(%d) Error> open faile for DNP Filename[%s]!!\n",__FUNCTION__,__LINE__,pSavePath);
		VK_free(pOLT->pSaveFileName);
		VK_MEM_Free(pOLT);
		return NULL;
	}

	pOLT->nOLTChunkExponent  = nChunkExponent;
	pOLT->nOLTChunkBytes     = 1<<(nChunkExponent+10);
	pOLT->llTotalDataBytes   = nContentLength;
	pOLT->nOLTShiftCount     = 10 + nChunkExponent;
/*************************
*	One bit per every 8K bytes data,
*	so, OLTsize = contents length / 64K
*************************/
#if 0	/* Jongwon: I am sorry */
	pOLT->nOLTSize           = (nContentLength >> pOLT->nOLTShiftCount) + 1;
#else
	pOLT->nOLTSize           = (nContentLength >> (pOLT->nOLTShiftCount+3)) + 1;
#endif
	pOLT->llWritingByteOffset  = 0;	/* writing cursor */
	pOLT->llJumpRequestOffset = 0;
	pOLT->llReadingByteOffset  = 0;	/* reading cursor */
	pOLT->llSumOfWriting     = 0;
	pOLT->llFragmented       = 0;
	pOLT->ulEndOffset        = 0;
	pOLT->nWritingSpeedPerSecond = 0;
	pOLT->llOldSum           =0;
	pOLT->bComplete = FALSE;

	pOLT->pTable = (HINT8*) DD_MEM_Alloc (pOLT->nOLTSize);
	if(pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error > Offset Table VK_malloc fail !!\n",__FUNCTION__,__LINE__);
		VK_free(pOLT->pSaveFileName);
		VK_MEM_Free (pOLT);
		return NULL;
	}
	VK_memset (pOLT->pTable, 0x0, pOLT->nOLTSize);

/*************************
*	Semaphores for Write
*************************/
	nVKRet = VK_SEM_Create(&pOLT->ulOLTWriteSema, "OLT_WriteSema", VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s(%d) Error> OLT sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		VK_free(pOLT->pSaveFileName);
		VK_MEM_Free (pOLT);
		return NULL;
	}

	PrintExit;
	return pOLT;

}

HBOOL DNP_OLT_Destroy (DNP_OLT_t *pOLT)
{
	PrintEnter;

	if(pOLT==NULL)
	{
		return FALSE;
	}

	(void) VK_SEM_Destroy(pOLT->ulOLTWriteSema);

	if(pOLT->pSaveFileName != NULL)
	{
		VK_free(pOLT->pSaveFileName);
	}

	if(pOLT->pTable != NULL)
	{
		DD_MEM_Free(pOLT->pTable);
	}

	(void) close (pOLT->nReadFd);
	(void) close (pOLT->nWriteFd);

	if(pOLT!=NULL)
	{
		VK_MEM_Free(pOLT);
	}

	pOLT = NULL;

	PrintExit;

	return TRUE;
}

HBOOL	DNP_OLT_Write (DNP_OLT_t *pOLT, HINT8 *pBuf, HINT64 llSize)
{
	ssize_t nWriteSize=0;
	size_t nWriteSum=0;

	if (pOLT == NULL || pBuf == NULL)
	{
		PrintError ("%s(%d) Error> invalid NULL input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
//	VK_printf("[OLT_Write] offset: 0x%llx size: 0x%llx\n", lseek ((int)pOLT->nWriteFd, 0, SEEK_CUR), llSize);

/*************************
*	Write into the WriteFd
*************************/
	(void) VK_SEM_Get(pOLT->ulOLTWriteSema);
	do
	{
		nWriteSize = write ((int)pOLT->nWriteFd, pBuf+nWriteSum, llSize-nWriteSum);
		if(nWriteSize < 0)
		{
			break;
		}
		nWriteSum += nWriteSize;
	} while (nWriteSum < llSize);

	if (nWriteSum != llSize)
	{
		PrintError ("%s(%d) Error> write error (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
		return FALSE;
	}

/*************************
*	mark up the downloaded chunk
*************************/
	if ( FALSE == P_DNP_OLT_Marking (pOLT, pOLT->llWritingByteOffset) )
	{
		PrintError ("%s(%d) Error> can't mark Entry Bit. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
		return FALSE;
	}

/*************************
*	update write offset
*************************/
	pOLT->llWritingByteOffset += nWriteSum;
	pOLT->llSumOfWriting += nWriteSum;

	(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
	return TRUE;
}

HINT64	DNP_OLT_Read (DNP_OLT_t *pOLT, HINT8 *pBuf, HINT64 llSize)
{
	ssize_t nReadSize=0;
	ssize_t nReadSum=0;

	if (pOLT == NULL || pBuf == NULL)
	{
		PrintError ("%s(%d) Error> invalid NULL input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return -1;
	}
//	VK_printf("[OLT_Read] offset: 0x%llx size: 0x%llx\n", lseek ((int)pOLT->nReadFd, 0, SEEK_CUR), llSize);

/*************************
*	Read from the ReadFd
*************************/
	do
	{
		nReadSize = 0;
		nReadSize = read ((int)pOLT->nReadFd, pBuf+nReadSum, llSize-nReadSum);
		if(nReadSize < 0)
		{
			PrintError ("%s(%d) Error> read failed errno: %d\n", __FUNCTION__,__LINE__,errno);
			break;
		}
		else
		{
			nReadSum += nReadSize;
		    if (nReadSize == 0)
				break;
		}
	} while (nReadSum < llSize);

/*************************
*	update read offset
*************************/
	pOLT->llReadingByteOffset += nReadSum;

	return nReadSum;

}

HINT64 DNP_OLT_Seek(DNP_OLT_t *pOLT, HINT64 offset, HINT32 whence, DNP_OLT_SEEK_TYPE type)
{
	HINT64 llResult=0;

	if(pOLT == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return llResult;
	}
//VK_printf("[OLT_Seek] offset: 0x%llx\n", offset);

/*************************
*	choose the target file descriptor
*************************/
	if(type == DNP_OLT_SEEK_FOR_READ)
	{
		llResult = lseek(pOLT->nReadFd, offset, whence);
	}
	else if(type == DNP_OLT_SEEK_FOR_WRITE)
	{
		(void) VK_SEM_Get(pOLT->ulOLTWriteSema);
		llResult = lseek(pOLT->nWriteFd, offset, whence);
		(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
	}
	else
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return llResult;
	}

	return llResult;
}

#if __________OFFSET__________
#endif

HINT32 DNP_OLT_GetOneChunkSize (DNP_OLT_t *pOLT)
{
	return pOLT->nOLTChunkBytes;
}


/** For debugging */
void DNP_OLT_PrintInformation (DNP_OLT_t *pOLT)
{
	PrintDebug ("==================================================\n");
	if(pOLT==NULL)
	{
		PrintError ("pOLT pointer is NULL !!! \n");
	}
	else
	{
		PrintDebug ("pOLT pointer           (%u)\n"             ,(HUINT32)pOLT );
		PrintDebug ("pSaveFileName          (%s)\n"             ,pOLT->pSaveFileName );
		PrintDebug ("nReadFd                    (%d)\n"         ,pOLT->nReadFd );
		PrintDebug ("nWriteFd                    (%d)\n"        ,pOLT->nWriteFd );
		PrintDebug ("nOLTChunkExponent      (%d)\n"             ,pOLT->nOLTChunkExponent );
		PrintDebug ("nOLTChunkBytes         (%d)bytes\n"        ,pOLT->nOLTChunkBytes);
		PrintDebug ("nOLTShiftCount         (%d)\n"             ,pOLT->nOLTShiftCount );
		PrintDebug ("nOLTSize               (%d)bytes\n"        ,pOLT->nOLTSize );

		PrintDebug ("llWritingByteOffset      (%"PRId64")bytes\n" ,pOLT->llWritingByteOffset );
		PrintDebug ("llReadingByteOffset      (%"PRId64")bytes\n" ,pOLT->llReadingByteOffset );
		PrintDebug ("llTotalDataBytes       (%"PRId64")bytes\n" ,pOLT->llTotalDataBytes );
		PrintDebug ("llSumOfWriting         (%"PRId64")bytes\n" ,pOLT->llSumOfWriting );
		PrintDebug ("llFragmented           (%"PRId64") pices\n",pOLT->llFragmented );

		PrintDebug ("ulEndOffset            (%llu) ms\n"        ,pOLT->ulEndOffset );
		PrintDebug ("nWritingSpeedPerSecond (%d)bps\n"          ,pOLT->nWritingSpeedPerSecond );
		PrintDebug ("bComplete              (%s)\n"             ,pOLT->bComplete? "TRUE":"FALSE" );
	}
	PrintDebug ("==================================================\n");

	return;
}

HBOOL	DNP_OLT_IsFilled (DNP_OLT_t *pOLT, HINT64 llOffset, HINT64 llSize)
{
	HINT32 i=0;
	HINT32	nEntryCurNumber=0;
	HINT32	nEntryEndNumber=0;

	if ( pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if (llOffset+llSize >= pOLT->llTotalDataBytes)
	{
		llSize = pOLT->llTotalDataBytes - llOffset; /* llOffset is made from zero based offset */
	}

	/* get OLT table index : bit offset*/
	nEntryCurNumber =  (HINT32) (llOffset >> pOLT->nOLTShiftCount);
	nEntryEndNumber =  (HINT32) ((llOffset+llSize-1) >> pOLT->nOLTShiftCount);

#if OLT_DEBUGGING
	PrintError ("%s(%d) Debug> Searching %d to %d in OLT Table size(%"PRId64")bytes\n"
	,__FUNCTION__
	,__LINE__
	,nEntryCurNumber
	,nEntryEndNumber
	,llSize);
#endif

	/* searching from cur to endofstream */
	for (i=nEntryCurNumber; i<=nEntryEndNumber ; i++)
	{
		if ( FALSE == P_DNP_OLT_IsFilled (pOLT,i))
		{
#if OLT_DEBUGGING
			PrintDebug ("NOT FILLED!!\n");
#endif
			return FALSE;
		}
	}
#if OLT_DEBUGGING
	PrintDebug ("FILLED!!\n");
#endif

	return TRUE;
}


HBOOL DNP_OLT_CheckForJumping (DNP_OLT_t *pOLT)
{
//	HBOOL bFlag=FALSE;
//	HINT32 nNextEntryNumber=0;
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if(pOLT->bRequestJump == TRUE)
	{
		/* turn off flag immediately.*/
		pOLT->bComplete = FALSE;
		pOLT->bRequestJump = FALSE;
		return TRUE;
	}

	return FALSE;
}

HINT64	DNP_OLT_GetBufferingPosition (DNP_OLT_t *pOLT)
{
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return -1;
	}
	return pOLT->llWritingByteOffset;
}

HBOOL DNP_OLT_SetBufferingPosition(DNP_OLT_t *pOLT, HINT64 llOffset)
{
	HINT32 iFirstEmptySlot=0;

	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	(void) VK_SEM_Get(pOLT->ulOLTWriteSema);

	iFirstEmptySlot = P_DNP_OLT_SearchNextEmptyEntry ( pOLT , llOffset );
	if(iFirstEmptySlot == -1)
	{
		PrintError("Invalid parameter!\n");
		(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
		return FALSE;
	}
	else if(iFirstEmptySlot == -2)
	{
		PrintError("File download is over!\n");
		pOLT->llWritingByteOffset = pOLT->llTotalDataBytes;
		pOLT->bComplete = TRUE;
	}
	else
	{
		pOLT->llWritingByteOffset = (HINT64)iFirstEmptySlot << pOLT->nOLTShiftCount;
	}

	(void) VK_SEM_Release (pOLT->ulOLTWriteSema);
	return TRUE;
}

HINT64 DNP_OLT_GetNextEmptyPosition (DNP_OLT_t *pOLT, HINT64 llOffset)
{
	HINT32 nLastEntryNumber=0;

	if(pOLT == NULL )
	{
		PrintError ("%s(%d) Error> invalid OLT Pointer \n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/* searching Region 의 마지막 pointer 로 */
	nLastEntryNumber = P_DNP_OLT_SearchNextEmptyEntry (pOLT, llOffset);
	if (nLastEntryNumber == (-1))
	{
		return -1;
	}
	else if (nLastEntryNumber == (-2))
	{
		return -2;
	}

	/* The Position which isn't downloaded yet from server. */
	return ((HINT64)nLastEntryNumber << pOLT->nOLTShiftCount);	/* zero based offset */
}

HBOOL	DNP_OLT_UpdateReadingPosition (DNP_OLT_t *pOLT, HINT64 llOffset)
{
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Table \n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	pOLT->llReadingByteOffset = llOffset;
	return TRUE;
}

HBOOL	DNP_OLT_GetFirstOffsetOfEntry (DNP_OLT_t *pOLT, HINT64 llOffset)
{
	UNUSED_PARAM(pOLT); /* unused parameter */
	UNUSED_PARAM(llOffset);

	PrintError ("%s(%d) Dummy : not implemented yet \n",__FUNCTION__,__LINE__);
	return TRUE;
}

HINT32	DNP_OLT_GetEntryByOffset (DNP_OLT_t *pOLT, HINT64 llOffset)
{
	UNUSED_PARAM(pOLT); /* unused parameter */
	UNUSED_PARAM(llOffset);

	PrintError ("%s(%d) Dummy : not implemented yet \n",__FUNCTION__,__LINE__);
	return TRUE;
}

HBOOL  DNP_OLT_SetEndOffset (DNP_OLT_t *pOLT, HUINT64 ulEndOffset)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter.!!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if(ulEndOffset)
		pOLT->ulEndOffset = ulEndOffset;
	else
		pOLT->ulEndOffset = pOLT->llTotalDataBytes;

	return TRUE;
}

HBOOL	DNP_OLT_CheckReachedEOF (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if((HUINT64)pOLT->llWritingByteOffset >= (pOLT->ulEndOffset))
	{
		PrintDebug ("%s(%d) : BP: %lld, endbyte :%llu\n", __FUNCTION__,__LINE__,pOLT->llWritingByteOffset, pOLT->ulEndOffset);
		return TRUE;
	}
	return FALSE;
}

HINT32 DNP_OLT_BufferFlush (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	sync();
	return 1; //fsync (pOLT->nFd);
}

/* speed calculation */
HBOOL   DNP_OLT_InitSpeedGun (DNP_OLT_t *pOLT)
{
	HULONG ulStartClock = 0;

	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	ulStartClock = UTIL_GetClockMsec ();
#if 0
	if ( ulStartClock < 0 )
	{
		PrintError ("%s(%d) Warning!!> can't get time clock. This task can't support to investigate performance.\n"
				,__FUNCTION__
				,__LINE__);
		pOLT->bSpeedGun = FALSE;
		return FALSE;
	}
	else
#endif
	{
		PrintDebug ("%s(%d) Info> Performance Investigation mode : download started at %lu\n"
				,__FUNCTION__
				,__LINE__
				,ulStartClock);
		pOLT->bSpeedGun = TRUE;

		pOLT->ulStartClock = ulStartClock;
		pOLT->ulOldClock = 0;
	}

	return TRUE;
}

HINT32   DNP_OLT_GetBufferingRate (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	return pOLT->nWritingSpeedPerSecond;
}

/* return PlayingPosition ~ BufferingPosition as byte-offset */
HINT64 DNP_OLT_GetBufferedBytes (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(pOLT->llWritingByteOffset > pOLT->llReadingByteOffset)
		return pOLT->llWritingByteOffset - pOLT->llReadingByteOffset;
	else
		return 0;
}

/*************************
*	return the data size from the playing position to the last downloaded data slot
*************************/
HINT64 DNP_OLT_GetBufferedDataSize(DNP_OLT_t *pOLT)
{
	HBOOL bFilled=FALSE;
	HINT32 uiIndex=0;
	HINT32 iEntryNumber=0;
	HINT32 iEndNumber=0;
	HINT32 uiLastDataSlot=0;
	HINT64 llBufferedDataSize=0;

	if (pOLT == NULL)
	{
		PrintError ("pOLT is NULL!\n");
		return -1;
	}

	iEntryNumber = (HINT32)(pOLT->ullDataReadOffset >> pOLT->nOLTShiftCount);
	iEndNumber = (HINT32)((pOLT->llTotalDataBytes-1) >> pOLT->nOLTShiftCount);

/*************************
*	1. Find the last filled slot
*************************/
	for(uiIndex = iEntryNumber; uiIndex<=iEndNumber; uiIndex++)
	{
		bFilled = P_DNP_OLT_IsFilled(pOLT, uiIndex);
		if(bFilled == FALSE)
		{
			break;
		}
	}

/*************************
*	2. Get the buffered data size
*************************/
	if((HINT32)uiIndex == iEndNumber)
	{
		llBufferedDataSize = pOLT->llTotalDataBytes - pOLT->ullDataReadOffset;
	}
	else
	{
		uiLastDataSlot = uiIndex-1;
		if(uiLastDataSlot < iEntryNumber)
		{
			llBufferedDataSize = 0;
		}
		else
		{
			llBufferedDataSize = (uiLastDataSlot - iEntryNumber)<<pOLT->nOLTShiftCount;
		}
	}

	return llBufferedDataSize;
}

HBOOL DNP_OLT_UpdateSpeedGun(DNP_OLT_t *pOLT)
{
	HUINT64 ullDownloadedBytes=0;
	HULONG	ulElapsedSec = 0;

	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	if (pOLT->bSpeedGun == FALSE)
	{
		return FALSE;
	}

/*************************
*	Get the amount of the elapsed clock
*************************/
	ulElapsedSec = UTIL_GetClockMsec();
	ulElapsedSec -= pOLT->ulStartClock;
	ulElapsedSec /= 1000;

/*************************
*	Get the amount of the downloaded data
*************************/
	ullDownloadedBytes = P_DNP_OLT_GetDownloadedBytes(pOLT, 0);

/*************************
*	Update the current bandwidth
*************************/
	if(ulElapsedSec != 0)
	{
		pOLT->nWritingSpeedPerSecond = ullDownloadedBytes / ulElapsedSec;
	}
	PrintDebug("pOLT->nWritingSpeedPerSecond: %d Bps\n", pOLT->nWritingSpeedPerSecond);

#if 0
	ulCurrentClock = (HULONG) UTIL_GetClockMsec ();
	if ( ulCurrentClock > 0 )
	{
		if(pOLT->ulOldClock != ulCurrentClock)    /* to divide a time unit as one second */
		{
			if(ulCurrentClock > pOLT->ulStartClock)
			{
				if(pOLT->llOldSum != 0)
				{
					if(pOLT->llSumOfWriting == pOLT->llOldSum)
					{
						/*
						 * If a value of sum is not changed for a few milliseconds.
						 * The value of current Speed(BPS) should be reduced as amount of elapsed time.
						 * Below "minus" means the increase by the past speed.
						 */
						HINT32 minus = 0;
						minus = (HINT32) pOLT->nWritingSpeedPerSecond * (ulCurrentClock-(pOLT->ulOldClock))/1000;
						if(minus>pOLT->nWritingSpeedPerSecond) pOLT->nWritingSpeedPerSecond = 0;
						else pOLT->nWritingSpeedPerSecond -= minus;
					}
					else
					{
						pOLT->nWritingSpeedPerSecond = (HINT32) ((pOLT->llSumOfWriting - pOLT->llOldSum) * 1000 / (ulCurrentClock - pOLT->ulOldClock)) ;
					}
				}
				else /* firstTime Enterd */
				{
					pOLT->nWritingSpeedPerSecond = (HINT32) (pOLT->llSumOfWriting*1000 / (ulCurrentClock - pOLT->ulStartClock));
				}

				pOLT->llOldSum = pOLT->llSumOfWriting;

				PrintDebug ( "%s(%d) Rate %lu Bps (SUM : %"PRId64" bytes , Clock : %lu) \n"
						, __FUNCTION__
						, __LINE__
						, pOLT->nWritingSpeedPerSecond
						, pOLT->llSumOfWriting
						, ulCurrentClock );
				pOLT->ulOldClock = ulCurrentClock;
			}
			else if ( ulCurrentClock < pOLT->ulStartClock)
			{
				PrintError ("%s(%d) Error > UTIL Timer was reset !!! Please check about this (CRITICAL) \n",__FUNCTION__,__LINE__);
			}
			/* else continue ; ulCurrentClock == pOLT->ulStartClock */
		}
	}
#endif
	return TRUE;
}
void DNP_OLT_RegisterStartTimeMsec (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid NULL input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return;
	}

	pOLT->ulDelayedStartClock = (HULONG) UTIL_GetClockMsec ();
	PrintError ("%s(%d) Info> Start Clock (%lu)msecs. !!\n",__FUNCTION__,__LINE__,pOLT->ulDelayedStartClock);
	return;
}

void DNP_OLT_UnRegisterStartTimeMsec (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid NULL input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return;
	}

	pOLT->ulDelayedStartClock = 0;
	PrintError ("%s(%d) Info> Start Clock is cleared. !!\n",__FUNCTION__,__LINE__);
	return;
}


HBOOL	DNP_OLT_UninitSpeedGun (DNP_OLT_t *pOLT)
{
	if (pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid NULL input parameter. (CRITICAL) !!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* reset speed-gun parameters */
	pOLT->ulStartClock = 0;
	pOLT->ulOldClock = 0;
	pOLT->bSpeedGun = FALSE;

	return TRUE;
}

HINT64 DNP_OLT_GetDataReadOffset(DNP_OLT_t *pOLT)
{
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError("Invalid parameters\n");
		return -1;
	}
	return pOLT->ullDataReadOffset;
}

HBOOL DNP_OLT_SetDataReadOffset(DNP_OLT_t *pOLT, HUINT64 ullOffset)
{
	if(pOLT == NULL)
	{
		PrintError("pOLT is NULL!\n");
		return FALSE;
	}
	pOLT->ullDataReadOffset = ullOffset;
	return TRUE;
}

HINT64 DNP_OLT_GetJumpRequestPosition(DNP_OLT_t *pOLT)
{
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError("Invalid parameters\n");
		return -1;
	}
	return pOLT->llJumpRequestOffset;
}

void DNP_OLT_SetJumpRequestPosition(DNP_OLT_t *pOLT, HINT64 offset)
{
	if(pOLT == NULL || pOLT->pTable == NULL)
	{
		PrintError("Invalid parameters\n");
		return;
	}
	pOLT->llJumpRequestOffset = offset;
	pOLT->bRequestJump = TRUE;
	return;
}

HUINT64 DNP_OLT_GetDownloadedBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset)
{
	HUINT64 ullDownloadedBytes=0;

	if(pOLT == NULL)
	{
		PrintError("pOLT is NULL!!\n");
		return 0;
	}
	ullDownloadedBytes = P_DNP_OLT_GetDownloadedBytes(pOLT, ullOffset);
	return ullDownloadedBytes;
}
HUINT64 DNP_OLT_GetEmptyBytes(DNP_OLT_t* pOLT, HUINT64 ullOffset)
{
	HUINT64 ullEmptyBytes=0;

	if(pOLT == NULL)
	{
		PrintError("pOLT is NULL!!\n");
		return 0;
	}
	ullEmptyBytes = P_DNP_OLT_GetEmptyBytes(pOLT, ullOffset);
	return ullEmptyBytes;
}
