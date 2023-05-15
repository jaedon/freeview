/**
	@file     hxcbuf.c
	@brief   libpng interface function file

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>
#include <assert.h>

#include "_hlib_cbuf.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32		nBufSize;
	HINT32		nCurPos;

	HUINT8		*pstData;
} _HxCBUF_t;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HxCBUF_t		HLIB_CBUF_New(HINT32 nSize)
{
	_HxCBUF_t	*pstBuf;

	HxLOG_Assert(nSize);

	pstBuf = (_HxCBUF_t*)HLIB_MEM_Malloc(sizeof(_HxCBUF_t));

	pstBuf->nBufSize = nSize;
	pstBuf->nCurPos  = 0;
	pstBuf->pstData  = (HUINT8*)HLIB_MEM_Calloc(nSize);

	return (HxCBUF_t)pstBuf;
}

HERROR		HLIB_CBUF_Delete(HxCBUF_t hBufHandle)
{
	_HxCBUF_t	*bufInfo = (_HxCBUF_t*)hBufHandle;

	HxLOG_Assert(hBufHandle);

	if (bufInfo->pstData)
		HLIB_MEM_Free(bufInfo->pstData);

	HLIB_MEM_Free(bufInfo);

	return ERR_OK;
}

HCHAR*		HLIB_CBUF_GetBuffer(HxCBUF_t hBufHandle, HINT32 nSize)
{
	HCHAR		*p;
	_HxCBUF_t	*bufInfo = (_HxCBUF_t*)hBufHandle;

	HxLOG_Assert(bufInfo && bufInfo->pstData && (bufInfo->nBufSize > nSize));

	if (bufInfo->nCurPos + nSize < bufInfo->nBufSize)
	{
		p = (HCHAR*)&bufInfo->pstData[bufInfo->nCurPos];
		bufInfo->nCurPos += nSize;
	} else
	{
		p = (HCHAR*)&bufInfo->pstData[0];
		bufInfo->nCurPos = nSize;
	}

	return p;
}

/*******************************************************************************************
 * Static
 ******************************************************************************************/


