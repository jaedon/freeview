/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsyslib_list_priv.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 12/16/10 5:52p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/framework/noarch/bsyslib_list_priv.h $
* 
* Hydra_Software_Devel/2   12/16/10 5:52p bandrews
* SW7420-1319: fix iterator lists
* 
* Hydra_Software_Devel/1   3/24/08 3:08p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/1   1/3/08 6:05p bandrews
* PR36149: Added list
***************************************************************************/

#include "blst_queue.h"
#include "bsyslib.h"

#ifndef BSYSLIB_LIST_PRIV_H__
#define BSYSLIB_LIST_PRIV_H__

typedef struct BSYSlib_List_Entry
{
	BLST_Q_ENTRY(BSYSlib_List_Entry) sLink;
	void * pvData;
} BSYSlib_List_Entry;

struct BSYSlib_List_IteratorImpl
{
	BLST_Q_ENTRY(BSYSlib_List_IteratorImpl) sUsedLink;
	BLST_Q_ENTRY(BSYSlib_List_IteratorImpl) sFreeLink;
	BSYSlib_List_Handle hParent;
	BSYSlib_List_Entry * psCurrent;
};

struct BSYSlib_List_Impl
{
	BLST_Q_HEAD(BSYSlib_List_Head, BSYSlib_List_Entry) sEntries;
	BLST_Q_HEAD(BSYSlib_List_IteratorHead, BSYSlib_List_IteratorImpl) sUsedIterators;
	BLST_Q_HEAD(BSYSlib_List_FreeIteratorHead, BSYSlib_List_IteratorImpl) sFreeIterators;
	BSYSlib_List_IteratorHandle hIterator_isr;
};

BSYSlib_List_IteratorHandle BSYSlib_List_P_CreateIterator(
	BSYSlib_List_Handle hList
);

void BSYSlib_List_P_DestroyIterator(
	BSYSlib_List_IteratorHandle hIterator
);

#endif /* BSYSLIB_LIST_PRIV_H__ */

