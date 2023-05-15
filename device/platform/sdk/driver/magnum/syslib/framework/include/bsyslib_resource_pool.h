/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsyslib_resource_pool.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* NOTE: This module does not follow the magnum convention for BERR_Code
* as the return type of every function that can error out.  The reason
* for this is that it is much more useful to return the values selected
* in this code.
*
* $brcm_Log: /magnum/syslib/framework/noarch/bsyslib_resource_pool.h $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   1/10/08 5:21p bandrews
* PR36149: Added resource pool
***************************************************************************/

#include "bsyslib_list.h"

#ifndef BSYSLIB_RESOURCE_POOL_H__
#define BSYSLIB_RESOURCE_POOL_H__

/*
Summary:
*/
typedef struct
{
	BSYSlib_List_Handle hResources;
	BSYSlib_List_Handle hFree;
} BSYSlib_ResourcePool;

/*
Summary:
*/
BSYSlib_ResourcePool * BSYSlib_ResourcePool_Create(void);

/*
Summary:
*/
void BSYSlib_ResourcePool_Destroy(BSYSlib_ResourcePool * psPool);

/*
Summary:
*/
void BSYSlib_ResourcePool_Add(BSYSlib_ResourcePool * psPool, void * pvResource);

/*
Summary:
*/
void BSYSlib_ResourcePool_Remove(BSYSlib_ResourcePool * psPool, void * pvResource);

/*
Summary:
*/
void * BSYSlib_ResourcePool_Acquire(BSYSlib_ResourcePool * psPool);

/*
Summary:
*/
void BSYSlib_ResourcePool_Release(BSYSlib_ResourcePool * psPool, void * pvResource);

#endif /* BSYSLIB_RESOURCE_POOL_H__ */

