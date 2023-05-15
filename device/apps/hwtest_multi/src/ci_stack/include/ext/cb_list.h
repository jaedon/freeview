/* $Header:   //BASE/archives/STING_T/include/cb_list.h-arc   1.0   17 Dec 2004 10:31:46   jhkim1  $ */
/********************************************************************
 * $Workfile:   cb_list.h  $
 * Project    : PINE-II ST
 * Author     : Seungbum Chung
 * Description:
 *
 *                              Copyright (c) 2002 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _CB_LIST_H_
#define _CB_LIST_H_

/********************************************************************
 Header Files
 ********************************************************************/


/********************************************************************
 Constant/Macro Definition
 ********************************************************************/



/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef struct tCALLBACK_LIST {
	/* pointer to callback function */
	void 	*pFunc;

	/* call flag showing whether this func has been called */
	unsigned char 	bCall;

	/* for doubly-linked list */
	struct	tCALLBACK_LIST *pPrev;
	struct	tCALLBACK_LIST *pNext;
} CALLBACK_LIST, *CALLBACK_HANDLE;




/********************************************************************
 Global Function Declaration
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
CALLBACK_HANDLE Callback_CreateList(void);

/********************************************************************
 Function   : Callback_DeleteList
 Description: Delete entire list
 Input      : handle of head node
 Output     :
 Return     :
 ********************************************************************/
void Callback_DeleteList(CALLBACK_HANDLE hList);



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
CALLBACK_HANDLE Callback_AddFunc(CALLBACK_HANDLE hList, void *pFunc);

/********************************************************************
 Function   : Callback_RemoveFunc
 Description: Search and remove if specified function exists in list
 Input      : handle of head node, function pointer
 Output     :
 Return     :
 ********************************************************************/
void Callback_RemoveFunc(CALLBACK_HANDLE hList, void *pFunc);



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
void Callback_Reset(CALLBACK_HANDLE hList);

/********************************************************************
 Function   : Callback_GetNextFunc
 Description: Get function pointer in list and set call-flag for that
 Input      : handle of head node
 Output     :
 Return     : function pointer found
              (NULL if call-flag of all nodes has been set)
 ********************************************************************/
void *Callback_GetNextFunc(CALLBACK_HANDLE hList);

/********************************************************************
 Function   : Callback_GetNumFunc
 Description: Get number of function pointers in list
 Input      : handle of head node
 Output     :
 Return     : number of function pointers
 ********************************************************************/
unsigned char Callback_GetNumFunc(CALLBACK_HANDLE hList);

#endif



