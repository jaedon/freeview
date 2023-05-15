/*************************************************************
 * @file		hxmempool.h
 * @date		2011/06/29
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, fixed size blocks allocation memory
 * http://www.humaxdigital.com
 *************************************************************/

#define	____HxMEMPOOL_README___________________

/******************************************************************************
 [Example Code]

int main (int argc, char *argv[])
{


	return 0;
}


 [Expected Result]

 ************************************************************************************/

#ifndef __HLIB_MEMPOOL_H__
#define __HLIB_MEMPOOL_H__

#define	HxMEMPOOL

//#include <htype.h>
#include "htype.h"

/**
 * block의 개수와 사이즈가 정해져있을 때, 필요한 메모리 양
 *
 *
 */
HUINT32	HLIB_MEMPOOL_ComputeBytes (HUINT32 blocsize, HUINT32 blockcount);

/**
 * create memory pool (fixed-size-blocks allocation)
 *
 * @param	buf			memory buffer (if NULL, buf will be allocated)
 * @param	bytes		bytes of buf (or to be allocated size)
 * @param	blocksize	memory block(slice) size
 * @return	memory pool descriptor
 */
HINT32	HLIB_MEMPOOL_New (void *buf, HUINT32 bytes, HUINT32 blocksize);

/**
 * create memory pool (fixed-size-blocks allocation)
 * - if memory pool underflow, mempool use additional memory from the function (passed by param 4 & 5).
 *
 * @param	buf			memory buffer (if NULL, buf will be allocated)
 * @param	bytes		bytes of buf (or to be allocated size)
 * @param	blocksize	memory block(slice) size
 * @param	_malloc		memalloc function
 * @param	_free		mem free function
 * @return	memory pool descriptor
 */
HINT32	HLIB_MEMPOOL_NewEx (void *buf, HUINT32 bytes, HUINT32 blocksize, void * (*_malloc)(HUINT32), void (*_free)(void *));

/**
 * destroy memory pool
 *
 * @param	md		memory pool descriptor
 */
void	HLIB_MEMPOOL_Delete (HINT32 md);

/**
 * clear memory pool
 *
 * @param	md		memory pool descriptor
 */
void	HLIB_MEMPOOL_Clear (HINT32 md);

/**
 * allocate memory (blocksize) from pool
 *
 * @param	md		memory pool descriptor
 * @return	memory
 */
void *	HLIB_MEMPOOL_Alloc (HINT32 md);

/**
 * allocate memory (blocksize) from pool. the memory is set to zero.
 *
 * @param	md		memory pool descriptor
 * @return	memory
 */
void *	HLIB_MEMPOOL_Calloc (HINT32 md);

/**
 * return memory (blocksize) to pool
 *
 * @param	md		memory pool descriptor
 * @param	data	memory
 * @return	NULL (if 'data' has invalid address then the invalid address will be returned.)
 */
void *	HLIB_MEMPOOL_Free  (HINT32 md, void *data);

HBOOL	HLIB_MEMPOOL_IsAllocated (HINT32 md, const void *address);

#endif /* __HLIB_MEMPOOL_H__ */

