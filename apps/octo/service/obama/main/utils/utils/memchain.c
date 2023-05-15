/**
	@file     memchain.c
	@brief   Utility for associating allocated memory

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "octo_common.h"
#include "vkernel.h"
#include "memchain.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MEMCHAIN_MAGIC		0x45CBA9D1






/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define	______GLOBAL_MEMCHAIN_FUNC_START______

void *	memchain_malloc (unsigned long size)
{
	return HLIB_STD_MemAlloc(size);
}
void	memchain_free (void *d)
{
	HLIB_STD_MemFree(d);
}
/*=========================================================================
int UTIL_MEMCHAIN_Create (memId_t *memId, memChainSetupPtr_t setupPtr)

	*memId:		memId of the newly created memChain.
	setupPtr:		Pointer to the memChain setup information.

Creates a memChain object which links related memory into a 'chain'.  All
memory associated with this chain can then be easily freed.
=========================================================================*/
MC_Errot_t UTIL_MEMCHAIN_Create (memId_t *memId, memChainSetupPtr_t setupPtr)
{
	myMalloc_f			Malloc;
	memChainHeadPtr_t	memChainHeadPtr;
	MC_Errot_t			err = ERR_MC_NoErr;

/*	OSA_Print("memChainCreate()\n"); */
	if ((memId == NULL) || (setupPtr == NULL))
	{
		err = ERR_MC_NullPtr;
		goto memChainCreateEnd;
	}

	if (setupPtr->Malloc == NULL)
	{
		#ifdef HUMAX_MEMORY_DEBUG
		Malloc = (myMalloc_f)VK_DBGMEM_Alloc;
		#else
		Malloc = (myMalloc_f)memchain_malloc;
		#endif
	}
	else
	{
		Malloc = setupPtr->Malloc;
	}
	#ifdef HUMAX_MEMORY_DEBUG
	memChainHeadPtr = (memChainHeadPtr_t)Malloc(sizeof(memChainHead_t), __FILE__, __LINE__, MEMDBG_GROUP_SI);
	#else
	memChainHeadPtr = (memChainHeadPtr_t)Malloc(sizeof(memChainHead_t));
	#endif
	if (memChainHeadPtr == NULL) {
		err = ERR_MC_OutOfMem;
		goto memChainCreateEnd;
	}
	memset(memChainHeadPtr,0x00,sizeof(memChainHead_t));

	/* initialize */
	memChainHeadPtr->memLimit = setupPtr->memLimit;
	memChainHeadPtr->Malloc = Malloc;
	if (setupPtr->Free == NULL)
	{
		#ifdef HUMAX_MEMORY_DEBUG
		memChainHeadPtr->Free = (myFree_f)VK_DBGMEM_Free;
		#else
		memChainHeadPtr->Free = (myFree_f)memchain_free;
		#endif
	}
	else
	{
		memChainHeadPtr->Free = setupPtr->Free;
	}
	memChainHeadPtr->magic = MEMCHAIN_MAGIC;
	memChainHeadPtr->totalElements = 0;
	memChainHeadPtr->totalBytes = sizeof(memChainHead_t);
	memChainHeadPtr->totalUserBytes = 0;
	memChainHeadPtr->element = NULL;

	/* set instance */
	*memId = memChainHeadPtr;

memChainCreateEnd:
	return (err);
}


/*=========================================================================
int UTIL_MEMCHAIN_Destroy (memId_t memId)

	memId:		memId of the memChain.

Destroys the memChain, freeing all associated memory.
=========================================================================*/
MC_Errot_t UTIL_MEMCHAIN_Destroy (memId_t memId)
{
	memChainHeadPtr_t	memChainHeadPtr = (memChainHeadPtr_t)memId;
	memElementPtr_t		currElem, prevElem;

/*
	OSA_Print("memChainDestroy(%d)\n", instance);
	OSA_Print("totalElements=%d\n", memChainHeadPtr->totalElements);
*/
	memChainHeadPtr->magic = 0;

	/* free the memElements */
	currElem = memChainHeadPtr->element;
	while (currElem != NULL)
	{
		prevElem = currElem;
		currElem = currElem->next;
/*
		OSA_Print("free:0x%08X\n",(EG_S32)prevElem);
*/
		/* double pointer(Elite G9)를 single pointer(OSA)로 수정 by jgkim */
		#ifdef HUMAX_MEMORY_DEBUG
		memChainHeadPtr->Free(prevElem, __FILE__, __LINE__, MEMDBG_GROUP_MW);
		#else
		memChainHeadPtr->Free(prevElem);
		#endif
	}

	/* double pointer(Elite G9)를 single pointer(OSA)로 수정 by jgkim */
	/* lastly, free the memChainHead */
	#ifdef HUMAX_MEMORY_DEBUG
	memChainHeadPtr->Free(memChainHeadPtr, __FILE__, __LINE__, MEMDBG_GROUP_MW);
	#else
	memChainHeadPtr->Free(memChainHeadPtr);
	#endif
/*	OSA_Print("free:0x%08X\n",memChainHeadPtr); */

	return (ERR_MC_NoErr);
}


/*=========================================================================
void* UTIL_MEMCHAIN_Alloc (memId_t memId, unsigned int size)

	memId:		memId of the memChain.
	size:			The size of the requested memory chunk.

Allocates a memory block of the requested size and associates it with the
specified memChain.  Returns NULL if the OS_Calloc() fails or if memLimit
will be exceeded.
=========================================================================*/
void* UTIL_MEMCHAIN_Alloc (memId_t memId, HUINT32 size)
{
	memChainHeadPtr_t	memChainHeadPtr = (memChainHeadPtr_t)memId;
	memElementPtr_t		memElementPtr;
	HUINT32			allocSize;
	void				*rptr, *p;

	/*OSA_Print((memChainHeadPtr->magic == MEMCHAIN_MAGIC), "Not valid memChain.\n");*/

/*
	OSA_Print("memChainAlloc(%d), size:%d\n",instance,size);
*/
	/* check the memLimit (memLimit = 0 means no limit) */
	if (memChainHeadPtr->memLimit > 0)
	{
		if (memChainHeadPtr->totalUserBytes + size > memChainHeadPtr->memLimit)
		{
			HxLOG_Print("memLimit exceeded.  totalUserBytes=%d\n", memChainHeadPtr->totalUserBytes);
			rptr = NULL;
			goto memChainAllocEnd;
		}
	}

	/* allocate the memory - add 8 bytes to guarantee 64-bit alignment of user data */
	allocSize = sizeof(memElement_t) + size + 8;

	#ifdef HUMAX_MEMORY_DEBUG
	memElementPtr = (memElementPtr_t)memChainHeadPtr->Malloc(allocSize, __FILE__, __LINE__, MEMDBG_GROUP_MW);
	#else
	memElementPtr = (memElementPtr_t)memChainHeadPtr->Malloc(allocSize);
	#endif
	if (memElementPtr == NULL)
	{
		HxLOG_Print("memChainAlloc : alloc is fail..!!");
		rptr = NULL;
		goto memChainAllocEnd;
	}
	memset(memElementPtr,0x00,allocSize);

	p = (void *)(((HUINT8 *)(memElementPtr) + sizeof(memElement_t)));

	/* align on 64-bit boundary */
	memElementPtr->userData = (void *)((HUINT32)(((HUINT8 *)p)+8) & ((~0) << 3));
	rptr = memElementPtr->userData;

	/* insert element in list */
	memElementPtr->next = memChainHeadPtr->element;
	memChainHeadPtr->element = memElementPtr;

	/* update tracking statistics */
	memChainHeadPtr->totalElements++;
	memChainHeadPtr->totalBytes += allocSize;
	memChainHeadPtr->totalUserBytes += size;

/*
	OSA_Print("\ttotalElements:%d,",memChainHeadPtr->totalElements);
	OSA_Print("totalBytes:%d,",memChainHeadPtr->totalBytes);
	OSA_Print("totalUserBytes:%d\n",memChainHeadPtr->totalUserBytes);
	OSA_Print("memElementPtr:0x%08X\n", (EG_S32)memElementPtr);
	OSA_Print("rptr:0x%08X\n",(EG_S32)rptr);
*/

memChainAllocEnd:
	return (rptr);
}


/********************************************************************
 * $Log: memchain.c,v $
 * Revision 2.2  2004/11/02 01:44:44  kagrawal
 * Fixed a compilation error due to wrong typecasting
 *
 * Revision 2.1  2004/10/30 03:28:43  dcrom
 * Merges from the latest TL9 branch sources
 *
 * Revision 2.0  2003/12/24 00:46:38  tvogt
 * first checkin for g10
 *
 * Revision 1.1  2002/06/10 17:20:08  tvogt
 * First checkin of TL9xx Transport Driver
 *
 *
 * TL8xx Derived Revision 1.15.12.1  2002/01/12 11:09:12  neelakan
 ********************************************************************/
