/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mempool.h
	@brief	  mempool & �޸� Ǯ

	Description:                             		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MEMPOOL_H__
#define	__MEMPOOL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <htype.h>
#include <stdarg.h>


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct	tagMemoryPoolElem
{
	struct tagMemoryPoolElem	*pNext;
	HUINT8						ucDataField;
}MemPoolElem_t;


/**
 * Memory Pool
 *
 * @struct tagMemoryPool
 */
typedef struct tagMemoryPool
{
	void			*buf;			///< Memory buffer

	HUINT32			ulLength;		///< Buffer length in bytes
	HUINT32			ulNum;			///< Number of elements
	HUINT32			ulSize;			///< Size of an element

	HUINT32			ulRemained;
	HUINT8			*pucSlice;
	MemPoolElem_t	*pFree;			///< Free list
	HUINT32			ulAllocated;	///< Number of allocated slot


	/**
	 * Constructor of element
	 * ���� �޸� Ǯ�κ��� �޸𸮸� �Ҵ���� ���� ���, elem ���� NULL�� �Ѿ��.
	 *
 	 * @param mempool	this
	 * @param elem		An element
	 * @param va		var arguments
	 * @return	allocated element
	 */
	void* (* ctor)	(struct tagMemoryPool *mempool, void *elem, va_list va);

	/**
	 * Destructor of element
	 * ���� elem�� �޸� Ǯ ������ ����� ��� mempool�� NULL�� �Ѿ��.
	 *
 	 * @param mempool	this
	 * @param elem		An element
	 * @param va		var arguments
	 */
	void (*	dtor)	(struct tagMemoryPool *mempool, void *elem, va_list va);
}MemPool_t;


typedef void *	(* MemPoolCtor_t)(MemPool_t *, void *, va_list);
typedef void 	(* MemPoolDtor_t)(MemPool_t *, void *, va_list);



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
 * create memory pool
 *
 * @param	buf			memory buffer
 * @param	ulLength	length of memory buffer
 * @param	ulSize		size of an element
 * @param	pfnCtor		constructor
 * @param	pfnDtor		destructor
 * @return	instance of 'MemPool_t' or NULL
 */
MemPool_t *	MEMPOOL_Create (
					void *			buf,
					const HUINT32	ulLength,
					const HUINT32	ulSize,
					MemPoolCtor_t	pfnCtor,
					MemPoolDtor_t	pfnDtor
					);

/**
 * Destroy mempool
 *
 * @param 	pMemPool	instance of 'MemPool_t'
 */
 void	MEMPOOL_Destroy (
					MemPool_t *pMemPool
					);


/**
 * Clear mempool
 *
 * @param 	pMemPool	instance of 'MemPool_t'
 */
void	MEMPOOL_Clear	(
					MemPool_t *pMemPool
					);

/**
 * Allocate new element from memory pool.
 *
 * @param	pMemPool	instance of 'MemPool_t'
 * @param	...			pass '...' to constructor
 * @return	An instance or NULL
 */
void *	MEMPOOL_New (
					MemPool_t *const pMemPool,
					...
					);


/**
 * Free
 *
 * @param 	pMemPool	instance of 'MemPool_t'
 * @param 	pvElem		An element will be deleted.
 * @param	...			pass '...' to destructor
 */
void	MEMPOOL_Delete (
					MemPool_t *const pMemPool,
					void *			pvElem,
					...
					);


#ifdef CONFIG_DEBUG
/**
 * Print Mempool status for DEBUG
 *
 * @param 	pMemPool	instance of 'MemPool_t'
 * @param	ulDbgLevel	Debug level (e.g. HxLOG_Print;
 */
void 	MEMPOOL_Print (
					const MemPool_t *const pMemPool,
					HUINT32 		ulDbgLevel
					);

#else

#define MEMPOOL_Print	// Do nothing...

#endif //CONFIG_DEBUG

#endif /* !__MEMPOOL_H__ */


