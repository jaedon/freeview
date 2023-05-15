/* $Header:   //BASE/archives/STING_T/util/cb_list.c-arc   1.0   17 Dec 2004 10:35:34   jhkim1  $ */
/********************************************************************
 * $Workfile:   cb_list.c  $
 * Project    : PINE-II ST
 * Author     : Seungbum Chung
 * Description:
 *
 *                                 Copyright (c) 2002 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/********************************************************************
 Header Files
 ********************************************************************/
#include <string.h>
#include "vkernel.h"
#include "cb_list.h"

/********************************************************************
 Macro Definition
 ********************************************************************/



/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/


/********************************************************************
 External Variable Declaration
 ********************************************************************/


/********************************************************************
 Static Function
 ********************************************************************/


/********************************************************************
 Global Function
 ********************************************************************/
/******************************
 *     Create/Delete list     *
 ******************************/
/********************************************************************
 Function   : Callback_CreateList
 Description: Create head node for list of callback function pointers
 Input      :
 Output     :
 Return     : handle of head node
 ********************************************************************/
CALLBACK_HANDLE Callback_CreateList(void)
{
	CALLBACK_HANDLE hList;

	/* allocate head node of doubly-linked list */
	hList = (CALLBACK_HANDLE)DD_MEM_Alloc(sizeof(CALLBACK_LIST));
	memset(hList, 0, sizeof(CALLBACK_LIST));

	/* prev/next pointer should point to self */
	hList->pPrev = hList;
	hList->pNext = hList;

	return hList;
}



/********************************************************************
 Function   : Callback_DeleteList
 Description: Delete entire list
 Input      : handle of head node
 Output     :
 Return     :
 ********************************************************************/
void Callback_DeleteList(CALLBACK_HANDLE hList)
{
	CALLBACK_HANDLE hNode, hNodeRemove;

	/* traverse all node in list */
	hNode = hList->pNext;
	while (hNode != hList)
	{
		/* get node which should be removed */
		hNodeRemove = hNode;

		/* advance pointer */
		hNode = hNode->pNext;

		/* pull out from linked list */
		hNodeRemove->pPrev->pNext = hNodeRemove->pNext;
		hNodeRemove->pNext->pPrev = hNodeRemove->pPrev;

		/* deallocate memory */
		DD_MEM_Free(hNodeRemove);
	}
}



/******************************
 *   Add/Remove func-pointer  *
 ******************************/
/********************************************************************
 Function   : Callback_AddFunc
 Description: Add callback function to list
 Input      : handle of head node, function pointer
 Output     :
 Return     : handle of new node allocated
              (NULL if error is occurred)
 ********************************************************************/
CALLBACK_HANDLE Callback_AddFunc(CALLBACK_HANDLE hList, void *pFunc)
{
	CALLBACK_HANDLE hNode;

	/* search given function pointer in whole linked-list */
	hNode = hList->pNext;
	while (hNode != hList)
	{
		if (hNode->pFunc == pFunc)
		{
			/* check duplication: if found, return NULL */
			return NULL;
		}

		hNode = hNode->pNext;
	}

	/* allocate node for given function pointer */
	hNode = (CALLBACK_HANDLE)DD_MEM_Alloc(sizeof(CALLBACK_LIST));

	/* set function pointer and call flag */
	hNode->pFunc 	= pFunc;
	hNode->bCall	= 0;

	/* insert to first of list */
	hNode->pPrev			= hList;
	hNode->pNext			= hList->pNext;
	hList->pNext->pPrev 	= hNode;
	hList->pNext 			= hNode;

	/* return allocated handle */
	return hNode;
}



/********************************************************************
 Function   : Callback_RemoveFunc
 Description: Search and remove if specified function exists in list
 Input      : handle of head node, function pointer
 Output     :
 Return     :
 ********************************************************************/
void Callback_RemoveFunc(CALLBACK_HANDLE hList, void *pFunc)
{
	CALLBACK_HANDLE hNode;

	/* search given function pointer in whole linked-list */
	hNode = hList->pNext;
	while (hNode != hList)
	{
		if (hNode->pFunc == pFunc)
			break;

		hNode = hNode->pNext;
	}

	/* if node of given function pointer cannot be found */
	if (hNode == hList) {
		return;
	}

	/* pull out from linked list */
	hNode->pPrev->pNext = hNode->pNext;
	hNode->pNext->pPrev = hNode->pPrev;

	/* deallocate memory */
	DD_MEM_Free(hNode);
}



/******************************
 * Get func-pointer from list *
 ******************************/
/********************************************************************
 Function   : Callback_Reset
 Description: Reset call-flag of all nodes in list
              Must be called before chain of GetNextFunc()
 Input      : handle of head node
 Output     :
 Return     :
 ********************************************************************/
void Callback_Reset(CALLBACK_HANDLE hList)
{
	CALLBACK_HANDLE hNode;

	/* traverse all node in list */
	hNode = hList->pNext;
	while (hNode != hList)
	{
		/* reset call-flags of all nodes */
		hNode->bCall = 0;

		hNode = hNode->pNext;
	}
}



/********************************************************************
 Function   : Callback_GetNextFunc
 Description: Get function pointer in list and set call-flag for that
 Input      : handle of head node
 Output     :
 Return     : function pointer found
              (NULL if call-flag of all nodes has been set)
 ********************************************************************/
void *Callback_GetNextFunc(CALLBACK_HANDLE hList)
{
	CALLBACK_HANDLE hNode;

	/* traverse all node in list */
	hNode = hList->pNext;
	while (hNode != hList)
	{
		if (hNode->bCall == 0)
		{
			hNode->bCall = 1;
			return hNode->pFunc;
		}

		hNode = hNode->pNext;
	}

	return NULL;
}


/********************************************************************
 Function   : Callback_GetNumFunc
 Description: Get number of function pointers in list
 Input      : handle of head node
 Output     :
 Return     : number of function pointers
 ********************************************************************/
unsigned char Callback_GetNumFunc(CALLBACK_HANDLE hList)
{
	CALLBACK_HANDLE hNode;
	unsigned char			nNumFunc;

	/* traverse all node in list */
	nNumFunc 	= 0;
	hNode 		= hList->pNext;
	while (hNode != hList)
	{
		/* increase number of functions */
		nNumFunc++;

		hNode = hNode->pNext;
	}

	return nNumFunc;
}




