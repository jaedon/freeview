/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_dllist.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.07.20
 * File Description:	Double Linked List
 * Module:
 * Remarks:
 */

/**
 * @defgroup UTIL_DLLIST
 * @brief Double Linked List
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 22 July 2011
 */

/**
 * @brief Double Linked List 
 */


/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/**
 * @todo Mutex나 Sem 사용 해야함. (추후 내부에 Locking Algo. 추가 필요)
 */

/*@{*/

/**
 * @file util_dllist.h
 */

#ifndef __UTIL_DLLIST_H__
#define __UTIL_DLLIST_H__

#include <stdlib.h>
#include "htype.h"
#include "vkernel.h"

/// @name ErrorCode
//@{
typedef enum
{
	DLL_ERR_OK,
	DLL_ERR_FAIL
} DLL_ERRCODE;
//@}

/**
 * Double Linked List Node
 */
typedef struct _DL_ListNode_s
{
	HINT32 uuid; /**< unique id  */
	HUINT32 unWeight; /**< value  */
	void *pData; /**< data  */	
	struct _DL_ListNode_s *next; /**< next node */	
	struct _DL_ListNode_s *prev; /**< prev node */	
} UTIL_DLLNode_t;

/**
 * Double Linked List
 */
typedef struct _DL_List_s
{
	HINT32 uuid; /**< unique id */	
	HUINT32 unSize; /**< list size */	
	UTIL_DLLNode_t *pHead; /**< head node */	
	UTIL_DLLNode_t *pTail; /**< tail node */	
//	UTIL_DLLNode_t *pCur;
} UTIL_DLL_t;

/*! 
 *	\brief Data를 통해 Node 삽입 (Weight 로 정렬)
 *	\param *dllist : List Handle
 *	\param pData : Data
 *	\param unWeight : unWeight
 *	\return ErrorCode
 *	\see 
 */
DLL_ERRCODE UTIL_DLLIST_Insert (UTIL_DLL_t *dllist, void* pData, HUINT32 unWeight);

/*! 
 *	\brief Data를 통해 Node 삽입 (Tail)
 *	\param *dllist : List Handle
 *	\param pData : Data
 *	\return ErrorCode
 *	\see 
 */
DLL_ERRCODE UTIL_DLLIST_Append (UTIL_DLL_t *dllist, void* pData);

/*! 
 *	\brief Node 삽입 (Tail)
 *	\param *dllist : List Handle
 *	\param pData : Data
 *	\return ErrorCode
 *	\see 
 */
DLL_ERRCODE UTIL_DLLIST_AppendNode (UTIL_DLL_t *dllist, UTIL_DLLNode_t* pNewNode);

/*! 
 *	\brief Data를 통해 Node 삽입 (Head)
 *	\param *dllist : List Handle
 *	\param pData : Data
 *	\return ErrorCode
 *	\see 
 */
DLL_ERRCODE UTIL_DLLIST_Preppend (UTIL_DLL_t *dllist, void* pData);

/*! 
 *	\brief Node 삽입 (Head)
 *	\param *dllist : List Handle
 *	\param pData : Data
 *	\return ErrorCode
 *	\see 
 */
DLL_ERRCODE UTIL_DLLIST_PreppendNode (UTIL_DLL_t *dllist, UTIL_DLLNode_t* pNewNode);

/*! 
 *	\brief n 번 node 반환 
 *	\param *dllist : List Handle
 *	\param unNode : Node Number
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_Get_n_Node (UTIL_DLL_t *dllist, HUINT32 unNode);

/*! 
 *	\brief Head Node 반환 
 *	\param *dllist : List Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_GetHeadNode (UTIL_DLL_t *dllist);

/*! 
 *	\brief Tail Node 반환 
 *	\param *dllist : List Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_GetTailNode (UTIL_DLL_t *dllist);

/*! 
 *	\brief Next Node 반환 
 *	\param *node : Node Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_NextNode (UTIL_DLLNode_t* node);

/*! 
 *	\brief Prev Node 반환 
 *	\param *node : Node Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_PrevNode (UTIL_DLLNode_t* node);

/*! 
 *	\brief Head Node 를 추출 한다. 
 *	\param *dllist : List Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_ExtractHead(UTIL_DLL_t *dllist);

/*! 
 *	\brief Tail Node 를 추출 한다. 
 *	\param *dllist : List Handle
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_ExtractTail(UTIL_DLL_t *dllist);

/*! 
 *	\brief n Node 를 추출 한다. 
 *	\param *dllist : List Handle
 *	\param unNode : n Node Number 
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_Extract_n_Node(UTIL_DLL_t *dllist, HUINT32 unNode);

/*! 
 *	\brief Data Addr을 통해 Node 를 추출 한다. 
 *	\param *dllist : List Handle
 *	\param pData : Data Addr
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_ExtractNodeByDataAddr(UTIL_DLL_t *dllist, void* pData);

/*! 
 *	\brief Node n 부터 마지막 Node 까지 잘라서 추출해낸다. (n 포함)
 *	\param *dllist : List Handle
 *	\param unNode : n Node Number
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_Extract_toEnd(UTIL_DLL_t *dllist, HUINT32 unNode);

/*! 
 *	\brief Node n 부터 거꾸로 시작 Node 까지 잘라서 추출해낸다. (n 포함)
 *	\param *dllist : List Handle
 *	\param unNode : n Node Number
 *	\return Node Handle
 *	\see 
 */
UTIL_DLLNode_t* UTIL_DLLIST_Extract_toStart(UTIL_DLL_t *dllist, HUINT32 unNode);

/*! 
 *	\brief List Size 반환 
 *	\param *dllist : List Handle
 *	\return Size
 *	\see 
 */
HUINT32 UTIL_DLLIST_Size (UTIL_DLL_t* dllist);

/*! 
 *	\brief Nodes 제거
 *	\param *dllist : Node Handle
 *	\return ErrorCode
 *	\see 반드시 모든 Node의 Data는 NULL이 어야 한다.
 */
DLL_ERRCODE UTIL_DLLIST_DeleteNodes (UTIL_DLLNode_t* node);

/*! 
 *	\brief List 초기화
 *	\param *dllist : List Handle
 *	\return ErrorCode
 *	\see 반드시 모든 Node의 Data는 NULL이 어야 한다.
 */
DLL_ERRCODE UTIL_DLLIST_Reset (UTIL_DLL_t *dllist);

#endif /* __UTIL_DLLIST_H__ */

/*@}*/
