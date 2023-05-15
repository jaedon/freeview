/**
	@file     mempool.c
	@brief

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

//#include <octo_common.h>
#include <hlib.h>
#include <nx_common.h>
#include <mempool.h>


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

MemPool_t *	MEMPOOL_Create (
					void *			buf,
					const HUINT32	ulLength,
					const HUINT32	ulSize,
					MemPoolCtor_t	pfnCtor,
					MemPoolDtor_t	pfnDtor
					)
{
	MemPool_t	*pMemPool	= NULL;

	if (buf == NULL || ulLength == 0 || ulSize == 0)
	{
		return NULL;
	}

	pMemPool	= (MemPool_t *)NX_APP_Calloc(sizeof(MemPool_t));
	if (pMemPool == NULL)
	{
		return NULL;
	}

	pMemPool->buf			= buf;
	pMemPool->ulLength		= ulLength;
	pMemPool->ulNum			= ulLength / (ulSize + sizeof(MemPoolElem_t *));
	pMemPool->ulSize		= ulSize;
	pMemPool->pucSlice		= (HUINT8 *)buf;
	pMemPool->ulRemained	= ulLength;
	pMemPool->ctor			= pfnCtor;
	pMemPool->dtor			= pfnDtor;

	return pMemPool;
}


void	MEMPOOL_Destroy	(MemPool_t *pMemPool)
{
	if (pMemPool)
	{
		NX_APP_Free(pMemPool);
	}
}


void	MEMPOOL_Clear	(MemPool_t *pMemPool)
{
	if (pMemPool == NULL)
	{
		return;
	}

	pMemPool->pucSlice		= (HUINT8 *)pMemPool->buf;
	pMemPool->ulRemained	= pMemPool->ulLength;
	pMemPool->ulAllocated	= 0;
	pMemPool->pFree			= NULL;
}



void *	MEMPOOL_New		(MemPool_t *const pMemPool, ...)
{
	va_list	va;
	HUINT8	*pucRet	= NULL;

	if (pMemPool == NULL)
	{
		return NULL;
	}

	if (pMemPool->pucSlice != NULL)
	{
		HUINT32	ulOffset	= pMemPool->ulSize + sizeof(MemPoolElem_t *);

		if (pMemPool->ulRemained >= ulOffset)
		{
			pucRet	= pMemPool->pucSlice + sizeof(MemPoolElem_t *);
			pMemPool->pucSlice		+= ulOffset;
			pMemPool->ulRemained	-= ulOffset;
		}
		else
		{
			pucRet	= NULL;
			pMemPool->pucSlice		= NULL;
			pMemPool->ulRemained	= 0;
		}
	}

	if (pucRet == NULL && pMemPool->pFree != NULL)
	{
		MemPoolElem_t	*pElem	= pMemPool->pFree;
		pMemPool->pFree	= pMemPool->pFree->pNext;
		pucRet	= &pElem->ucDataField;
	}

	if (pucRet != NULL)
	{
		pMemPool->ulAllocated++;
	}

	if (pMemPool->ctor)
	{
		va_start(va, pMemPool);
		pucRet = pMemPool->ctor(pMemPool, pucRet, va);
		va_end(va);
	}

	return pucRet;
}


void	MEMPOOL_Delete	(MemPool_t *const pMemPool, void *pvElem, ...)
{
	HUINT32 ulElem;
	HUINT32	ulMemStart;
	HUINT32	ulMemEnd;
	va_list	va;

	if (pMemPool == NULL || pvElem == NULL || pMemPool->ulAllocated == 0)
	{
		return;
	}

	ulElem		= (HUINT32)pvElem;
	ulMemStart	= (HUINT32)pMemPool->buf;
	ulMemEnd	= ulMemStart + pMemPool->ulLength;

	if (ulMemStart <= ulElem && ulElem < ulMemEnd)
	{
		MemPoolElem_t	*pElem	= (MemPoolElem_t *)((HUINT8 *)pvElem - sizeof(MemPoolElem_t *));

		pElem->pNext	= pMemPool->pFree;
		pMemPool->pFree	= pElem;
		pMemPool->ulAllocated--;

		if (pMemPool->dtor)
		{
			va_start(va, pvElem);
			pMemPool->dtor(pMemPool, pvElem, va);
			va_end(va);
		}
	}
	else	// Invalid address!!
	{
		if (pMemPool->dtor)
		{
			va_start(va, pvElem);
			pMemPool->dtor(NULL, pvElem, va);
			va_end(va);
		}
	}
}



#ifdef CONFIG_DEBUG

void 	MEMPOOL_Print	(const MemPool_t *const pMemPool, HUINT32 ulDbgLevel)
{
	HUINT32	ulFirstAddr	= 0;
	HUINT32	ulLastAddr	= 0;

	if (pMemPool == NULL || pMemPool->buf == NULL)
	{
		return;
	}

	ulFirstAddr	= (HUINT32)pMemPool->buf;
	ulLastAddr	= ulFirstAddr + pMemPool->ulLength - 1;

	HxLOG_Print("[UTIL:MEMPOOL]\n"
				"           Memory pool: %02X%02X%02X%02X / %02X%02X%02X%02X\n"
				"   Total size in bytes: %u\n"
				"    Size of an element: %u\n"
				"        Allocated slot: %u\n",
				(HUINT8)((ulFirstAddr >> 24) & 0xFF), (HUINT8)((ulFirstAddr >> 16) & 0xFF),
				(HUINT8)((ulFirstAddr >>  8) & 0xFF), (HUINT8)((ulFirstAddr >>  0) & 0xFF),
				(HUINT8)((ulLastAddr  >> 24) & 0xFF), (HUINT8)((ulLastAddr  >> 16) & 0xFF),
				(HUINT8)((ulLastAddr  >>  8) & 0xFF), (HUINT8)((ulLastAddr  >>  0) & 0xFF),
				(unsigned int)pMemPool->ulLength,
				(unsigned int)pMemPool->ulSize,
				(unsigned int)pMemPool->ulAllocated
	);
}

#endif //CONFIG_DEBUG

/* end of file */

