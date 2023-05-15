/********************************************************************
 *
 * memchain.h - Utility for associating allocated memory
 *
 * Copyright 2003-2004 Zoran Corporation
 * All Rights Reserved
 *
 * memChainCreate:     Creates a memChain object.
 *   (O) instance:     returns the memChain instance.
 *   (I) memLimit:     Optionally limits the memory allocated with a
 *                     memChain.  Set to 0xFFFFFFFF for no limit.
 *
 * memChainDestroy:    Destroys a memChain object.
 *   (I) instance:     The memChain instance.
 *
 * memChainAlloc:      Allocates a memory block and associates it with
 *                     the specified memChain object.
 *   (I) instance:     The memChain instance.
 *   (I) size:         Size of memory (in bytes) requested.
 *
 * $Id: memchain.h,v 2.0 2003/12/23 01:52:22 tvogt Exp $
 ********************************************************************/

#ifndef _MEMCHAIN_H_
#define _MEMCHAIN_H_

#include "htype.h"
#include "vkernel.h"

#ifdef HUMAX_MEMORY_DEBUG
/* user-defined malloc/free */
typedef void* (*myMalloc_f) (unsigned long size, char* file, int line, unsigned short group);
typedef void (*myFree_f) (void *p, char* file, int line, unsigned short group);

#else

/* user-defined malloc/free */
typedef void* (*myMalloc_f) (HUINT32 size);
typedef void (*myFree_f) (void *block);
#endif

typedef	enum {
	ERR_MC_NoErr	= 0,
	ERR_MC_OutOfMem,
	ERR_MC_NullPtr
} MC_Errot_t;


/* memChain Setup Info */
typedef struct memChainSetup {
	HUINT32		memLimit;
	myMalloc_f		Malloc;
	myFree_f		Free;
} memChainSetup_t, *memChainSetupPtr_t;

typedef struct memElement
{
	void*			userData;
	struct memElement	*next;
} memElement_t, *memElementPtr_t;

typedef struct memChainHead {
	HUINT32			magic;
	HUINT32			memLimit;
	myMalloc_f		Malloc;
	myFree_f			Free;
	HUINT32			totalElements;
	HUINT32			totalBytes;
	HUINT32			totalUserBytes;
	memElementPtr_t	element;
} memChainHead_t, *memChainHeadPtr_t;


typedef struct memChainHead* memId_t;

/* API */
MC_Errot_t UTIL_MEMCHAIN_Create (memId_t *memId, memChainSetupPtr_t setupPtr);
MC_Errot_t UTIL_MEMCHAIN_Destroy (memId_t memId);
void* UTIL_MEMCHAIN_Alloc (memId_t memId, HUINT32 size);

#endif /* _MEMCHAIN_H_ */
/********************************************************************
 * $Log: memchain.h,v $
 * Revision 2.0  2003/12/23 01:52:22  tvogt
 * first checkin of g10 bsp
 *
 * Revision 1.1  2002/06/10 17:20:08  tvogt
 * First checkin of TL9xx Transport Driver
 *
 *
 * TL8xx Derived Revision 1.6.12.1  2002/01/12 11:09:12  neelakan
 ********************************************************************/
