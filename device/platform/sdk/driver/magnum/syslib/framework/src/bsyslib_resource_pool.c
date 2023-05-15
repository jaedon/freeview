/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsyslib_resource_pool.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/framework/noarch/bsyslib_resource_pool.c $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   1/10/08 5:21p bandrews
* PR36149: Added resource pool
***************************************************************************/

#include "bstd.h"
#include "bsyslib_list.h"
#include "bsyslib_resource_pool.h"

BDBG_MODULE(syslib_resource_pool);

BSYSlib_ResourcePool * BSYSlib_ResourcePool_Create(void)
{
	BSYSlib_ResourcePool * psPool;

	BDBG_ENTER(BSYSlib_ResourcePool_Create);

	psPool = (BSYSlib_ResourcePool *)BKNI_Malloc(sizeof(BSYSlib_ResourcePool));

	if (psPool)
	{
		psPool->hResources = BSYSlib_List_Create();
		psPool->hFree = BSYSlib_List_Create();
	}

	BDBG_LEAVE(BSYSlib_ResourcePool_Create);
	return psPool;
}

void BSYSlib_ResourcePool_Destroy(BSYSlib_ResourcePool * psPool)
{
	BDBG_ENTER(BSYSlib_ResourcePool_Destroy);

	BDBG_ASSERT(psPool);

	BSYSlib_List_Destroy(psPool->hResources);
	BSYSlib_List_Destroy(psPool->hFree);

	BKNI_Free(psPool);

	BDBG_LEAVE(BSYSlib_ResourcePool_Destroy);
}

void BSYSlib_ResourcePool_Add(BSYSlib_ResourcePool * psPool, void * pvResource)
{
	BDBG_ENTER(BSYSlib_ResourcePool_Add);

	BDBG_ASSERT(psPool);
	BDBG_ASSERT(pvResource);

	BSYSlib_List_AddElement(psPool->hResources, pvResource);
	BSYSlib_List_AddElement(psPool->hFree, pvResource);

	BDBG_LEAVE(BSYSlib_ResourcePool_Add);
}

void BSYSlib_ResourcePool_Remove(BSYSlib_ResourcePool * psPool, void * pvResource)
{
	BDBG_ENTER(BSYSlib_ResourcePool_Remove);

	BDBG_ASSERT(psPool);
	BDBG_ASSERT(pvResource);

	BSYSlib_List_RemoveElement(psPool->hResources, pvResource);
	BSYSlib_List_RemoveElement(psPool->hFree, pvResource);

	BDBG_LEAVE(BSYSlib_ResourcePool_Remove);
}

void * BSYSlib_ResourcePool_Acquire(BSYSlib_ResourcePool * psPool)
{
	void * pvResource = NULL;
	
	BDBG_ENTER(BSYSlib_ResourcePool_Acquire);

	BDBG_ASSERT(psPool);

	if (!BSYSlib_List_IsEmpty(psPool->hFree))
	{
		pvResource = BSYSlib_List_GetByIndex(psPool->hFree, 0);
		BSYSlib_List_RemoveElement(psPool->hFree, pvResource);
	}

	BDBG_LEAVE(BSYSlib_ResourcePool_Acquire);
	return pvResource;
}

void BSYSlib_ResourcePool_Release(BSYSlib_ResourcePool * psPool, void * pvResource)
{
	BDBG_ENTER(BSYSlib_ResourcePool_Release);

	BDBG_ASSERT(psPool);
	BDBG_ASSERT(pvResource);

	BSYSlib_List_AddElement(psPool->hFree, pvResource);

	BDBG_LEAVE(BSYSlib_ResourcePool_Release);
}

