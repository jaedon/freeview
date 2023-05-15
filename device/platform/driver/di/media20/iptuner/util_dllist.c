/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_dllist.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		$
 * Current Author:	$Author: $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	Double linked list
 * Module: 			          
 * Remarks:		
 */

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "util_dllist.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define UTIL_DLLIST_DEBUG
//#define STREAM_HTTP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef UTIL_DLLIST_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		
#define PrintExit		
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef UTIL_DLLIST_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */

/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/

DLL_ERRCODE UTIL_DLLIST_Preppend (UTIL_DLL_t *dllist, void* pData)
{
	UTIL_DLLNode_t *pTemp;
	UTIL_DLLNode_t *pNewNode;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId); 
	pNewNode = (UTIL_DLLNode_t *)VK_MEM_Alloc(sizeof(UTIL_DLLNode_t));
	VK_memset(pNewNode,0x00,sizeof(UTIL_DLLNode_t));
	pNewNode->pData = pData;
	pTemp = dllist->pHead;	
	dllist->pHead = pNewNode;
	pNewNode->next = pTemp;
	pNewNode->prev = NULL;
	dllist->unSize++;
	if(pTemp != NULL)
	{
		pTemp->prev = pNewNode;		
	}
	else
	{
		dllist->pTail = pNewNode;
	}	
//	VK_SEM_Release(s_ulIPTunerPoolSemId);	
	PrintExit;
	return DLL_ERR_OK;
}

DLL_ERRCODE UTIL_DLLIST_PreppendNode (UTIL_DLL_t *dllist, UTIL_DLLNode_t* pNewNode)
{
	UTIL_DLLNode_t *pTemp;
	HUINT32 unNode = 0;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
	if(pNewNode == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId); 
	pTemp = pNewNode;
	while(pTemp != NULL)
	{
		unNode++;
		if(pTemp->next == NULL)
		{
			pNewNode = pTemp;
		}
		pTemp = pTemp->next;
	}
	dllist->unSize+=unNode;
	
	pTemp = dllist->pHead;	
	if(pTemp != NULL)
	{
		pTemp->prev = pNewNode;
		pNewNode->next = pTemp;
		dllist->pHead = pNewNode;
//		pNewNode->next = NULL;
	}
	else
	{
		dllist->pHead = pNewNode;
		dllist->pTail = pNewNode;
	}
	
//	VK_SEM_Release(s_ulIPTunerPoolSemId);	
	PrintExit;
	return DLL_ERR_OK;
}



DLL_ERRCODE UTIL_DLLIST_Insert (UTIL_DLL_t *dllist, void* pData, HUINT32 unWeight)
{
	UTIL_DLLNode_t *pTemp;
	UTIL_DLLNode_t *pNewNode;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId); 
	pNewNode = (UTIL_DLLNode_t *)VK_MEM_Alloc(sizeof(UTIL_DLLNode_t));
	VK_memset(pNewNode,0x00,sizeof(UTIL_DLLNode_t));
	pNewNode->pData = pData;
	pNewNode->unWeight = unWeight;
	pTemp = dllist->pHead;
	if(pTemp != NULL)
	{
		while(pTemp != NULL)
		{
			if(pTemp->unWeight > pNewNode->unWeight)
			{
				if(pTemp == dllist->pHead)
				{
					pTemp->prev = pNewNode;
					dllist->pHead = pNewNode;
					pNewNode->next = pTemp;
				}
				else
				{
					pTemp->prev->next = pNewNode;
					pNewNode->prev = pTemp->prev;
					pTemp->prev = pNewNode;
					pNewNode->next = pTemp;
				}
				dllist->unSize++;
				PrintExit;
				return DLL_ERR_OK;
			}
			pTemp = pTemp->next;
		}
		pNewNode->prev = dllist->pTail;
		dllist->pTail->next = pNewNode;
		dllist->pTail = pNewNode;
		dllist->unSize++;
		PrintExit;
		return DLL_ERR_OK;
	}
	else
	{
		dllist->pHead = pNewNode;
		dllist->pTail = pNewNode;
		dllist->unSize++;
		PrintExit;
		return DLL_ERR_OK;
	}
}


DLL_ERRCODE UTIL_DLLIST_Append (UTIL_DLL_t *dllist, void* pData)
{
	UTIL_DLLNode_t *pTemp;
	UTIL_DLLNode_t *pNewNode;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId); 
	pNewNode = (UTIL_DLLNode_t *)VK_MEM_Alloc(sizeof(UTIL_DLLNode_t));
	if(pNewNode == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
	VK_memset(pNewNode,0x00,sizeof(UTIL_DLLNode_t));
	pNewNode->pData = pData;
	pTemp = dllist->pTail;	
	if(pTemp != NULL)
	{
		pTemp->next = pNewNode;	
		dllist->pTail = pNewNode;
		pNewNode->prev = pTemp;
		pNewNode->next = NULL;
	}
	else
	{
		dllist->pHead = pNewNode;
		dllist->pTail = pNewNode;
	}	
	dllist->unSize++;
//	VK_SEM_Release(s_ulIPTunerPoolSemId);	
	PrintExit;
	return DLL_ERR_OK;
}

DLL_ERRCODE UTIL_DLLIST_AppendNode (UTIL_DLL_t *dllist, UTIL_DLLNode_t* pNewNode)
{
	UTIL_DLLNode_t *pTemp;
	HUINT32 unNode = 0;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
	if(pNewNode == NULL)
	{
		PrintExit;
		return DLL_ERR_FAIL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId); 
	pTemp = dllist->pTail;	
	if(pTemp != NULL)
	{
		pTemp->next = pNewNode;
		pNewNode->prev = pTemp;
//		pNewNode->next = NULL;
	}
	else
	{
		dllist->pHead = pNewNode;
		dllist->pTail = pNewNode;
	}
	pTemp = pNewNode;
	while(pTemp != NULL)
	{
		unNode++;
		if(pTemp->next == NULL)
		{
			dllist->pTail = pTemp;
		}
		pTemp = pTemp->next;
	}
	dllist->unSize+=unNode;
//	VK_SEM_Release(s_ulIPTunerPoolSemId);	
	PrintExit;
	return DLL_ERR_OK;
}

UTIL_DLLNode_t* UTIL_DLLIST_Extract_n_Node(UTIL_DLL_t *dllist, HUINT32 unNode)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	HUINT32 unIdx = 0;	
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pTemp = dllist->pHead;
	while(pTemp != NULL)
	{
		if(unNode == unIdx++)
		{
			if(pTemp->prev != NULL)
			{
				pTemp->prev->next = pTemp->next;
				if(pTemp->next != NULL)
				{
					pTemp->next->prev = pTemp->prev;
				}
				else
				{
					pTemp->prev->next = NULL;
					dllist->pTail = pTemp->prev;
				}
			}
			else
			{
				if(pTemp->next != NULL)
				{
					pTemp->next->prev = NULL;
					dllist->pHead = pTemp->next;								
				}
				else
				{
					dllist->pHead = NULL;
					dllist->pTail = NULL;
				}		
			}
				dllist->unSize--;
			pTemp->prev = NULL;
			pTemp->next = NULL;
			pExtractNode = pTemp;
			break;
		}
		pTemp = pTemp->next;
	}
	if(pTemp == NULL)
	{
		pExtractNode = NULL;
	}
	PrintExit;
	return pExtractNode;
}

UTIL_DLLNode_t* UTIL_DLLIST_ExtractNodeByDataAddr(UTIL_DLL_t *dllist, void* pData)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	if(pData == NULL)
	{
		PrintExit;
		return NULL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pTemp = dllist->pHead;
	while(pTemp != NULL)
	{
		if(pTemp->pData != NULL && pTemp->pData == pData)
		{
			if(pTemp->prev != NULL)
			{
				pTemp->prev->next = pTemp->next;
				if(pTemp->next != NULL)
				{
					pTemp->next->prev = pTemp->prev;
				}
				else
				{
					pTemp->prev->next = NULL;
					dllist->pTail = pTemp->prev;
				}
			}
			else
			{
				if(pTemp->next != NULL)
				{
					pTemp->next->prev = NULL;
					dllist->pHead = pTemp->next;								
			}
			else
			{
				dllist->pHead = NULL;
				dllist->pTail = NULL;
			}
			}
			dllist->unSize--;
			pTemp->prev = NULL;
			pTemp->next = NULL;
			pExtractNode = pTemp;
			break;
		}
		pTemp = pTemp->next;
	}
	if(pTemp == NULL)
	{
		pExtractNode = NULL;
	}
	PrintExit;
	return pExtractNode;
}

UTIL_DLLNode_t* UTIL_DLLIST_ExtractHead(UTIL_DLL_t *dllist)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pTemp = dllist->pHead;
	if(pTemp != NULL)
	{
		if(pTemp->next != NULL)
		{
			pTemp->next->prev = NULL;
			dllist->pHead = pTemp->next;
			pTemp->next = NULL;	
			dllist->unSize--;
		}
		else
		{
			dllist->pHead = NULL;
			dllist->pTail = NULL;
			dllist->unSize = 0;
		}
		pTemp->prev = NULL;
		pExtractNode = pTemp;
	}
	PrintExit;
	return pExtractNode;
}

UTIL_DLLNode_t* UTIL_DLLIST_ExtractTail(UTIL_DLL_t *dllist)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pTemp = dllist->pTail;
	if(pTemp != NULL)
	{
		if(pTemp->prev != NULL)
		{
			pTemp->prev->next = NULL;
			dllist->pTail = pTemp->prev;
			pTemp->prev = NULL;	
			dllist->unSize--;
		}
		else
		{
			dllist->pHead = NULL;
			dllist->pTail = NULL;
			dllist->unSize = 0;
		}
		pTemp->next = NULL;
		pExtractNode = pTemp;
	}
	PrintExit;
	return pExtractNode;
}



UTIL_DLLNode_t* UTIL_DLLIST_Extract_toEnd(UTIL_DLL_t *dllist, HUINT32 unNode)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	HUINT32 unIdx = 0;
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
//	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pTemp = dllist->pHead;
	while(pTemp != NULL)
	{
		if(unNode == unIdx++)
		{
			if(pTemp->prev != NULL)
			{
				pTemp->prev->next = NULL;
				dllist->pTail = pTemp->prev;
				dllist->unSize = unNode;
			}
			else
			{
				dllist->pHead = NULL;
				dllist->pTail = NULL;
				dllist->unSize = 0;
			}
			pTemp->prev = NULL;
			pExtractNode = pTemp;
			break;
		}
		pTemp = pTemp->next;
	}
	if(pTemp == NULL)
	{
		pExtractNode = NULL;
	}
	PrintExit;
	return pExtractNode;
}

UTIL_DLLNode_t* UTIL_DLLIST_Extract_toStart(UTIL_DLL_t *dllist, HUINT32 unNode)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pExtractNode = NULL;
	HUINT32 unIdx = 0;
	PrintEnter; 
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	pTemp = dllist->pHead;
	pExtractNode = dllist->pHead;
	while(pTemp != NULL)
	{
		if(unNode == unIdx++)
		{
			if(pTemp->next != NULL)
			{
				pTemp->next->prev = NULL;
				dllist->pHead = pTemp->next;
				dllist->unSize -= (unNode + 1);
			}
			else
			{
				dllist->pHead = NULL;
				dllist->pTail = NULL;
				dllist->unSize = 0;
			}
			pTemp->next = NULL;
			break;
		}
		pTemp = pTemp->next;
	}
	if(pTemp == NULL)
	{
		pExtractNode = NULL;
	}
	PrintExit;
	return pExtractNode;
}

#if 0
UTIL_DLLNode_t* UTIL_DLLIST_Split (UTIL_DLL_t *dllist, UTIL_DLLNode_t* pTargetNode)
{
	UTIL_DLLNode_t *pTemp = NULL;
	UTIL_DLLNode_t *pSplittedNode = NULL;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	pTemp = dllist->pHead;
	while(pTemp != NULL)
	{
		if(pTargetNode == pTemp)
		{
			if(pTemp->prev != NULL)
			{
				pTemp->prev->next = NULL;
				dllist->pTail = pTemp->prev;
			}
			else
			{
				dllist->pHead = NULL;
				dllist->pTail = NULL;
			}
			pTemp->prev = NULL;
			pSplittedNode = pTemp;
			break;
		}
		pTemp = pTemp->next;
	}
	PrintExit;
	return pSplittedNode;
}
#endif

UTIL_DLLNode_t* UTIL_DLLIST_GetHeadNode (UTIL_DLL_t *dllist)
{
	PrintEnter;
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return dllist->pHead;
}
UTIL_DLLNode_t* UTIL_DLLIST_GetTailNode (UTIL_DLL_t *dllist)
{
	PrintEnter;
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return dllist->pTail;
}

UTIL_DLLNode_t* UTIL_DLLIST_Get_n_Node (UTIL_DLL_t *dllist, HUINT32 unNode)
{
	HUINT32 unIdx = 0;
	UTIL_DLLNode_t *pTemp;
	PrintEnter;	
	if(dllist == NULL)
	{
		PrintExit;
		return NULL;
	}
	pTemp = dllist->pHead;
	if(pTemp == NULL)
	{
		PrintExit;
		return NULL;
	}
	do
	{
		if(unNode == unIdx++)
		{
			break;
		}
	} while((pTemp = pTemp->next) != NULL)
	PrintExit;
	return pTemp;
}


UTIL_DLLNode_t* UTIL_DLLIST_NextNode (UTIL_DLLNode_t* node)
{
	PrintEnter;
	if(node == NULL)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return node->next;
}
UTIL_DLLNode_t* UTIL_DLLIST_PrevNode (UTIL_DLLNode_t* node)
{
	PrintEnter;
	if(node == NULL)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return node->prev;
}

DLL_ERRCODE UTIL_DLLIST_Reset (UTIL_DLL_t *dllist)
{
	UTIL_DLLNode_t* node =NULL;
	PrintEnter;
	node = dllist->pTail;
	if(node == NULL)
	{
		// PrintDebug ("%s(%d) Node is NULL \n",__FUNCTION__,__LINE__);
	}
	while(node != NULL)
	{
		if(node->pData != NULL)
		{
			PrintError("%s(%d) Data is exist \n",__FUNCTION__,__LINE__);
			PrintExit;
			return DLL_ERR_FAIL;
		}			
		dllist->pTail = node->prev;
		if(node != NULL)
		{
			DD_MEM_Free(node);
		}
		if(dllist->pTail != NULL)
		{
			dllist->pTail->next = NULL;
		}
		node = dllist->pTail;
	}
	PrintExit;
	return DLL_ERR_OK;
}

DLL_ERRCODE UTIL_DLLIST_DeleteNodes (UTIL_DLLNode_t* node)
{
	UTIL_DLLNode_t* pTempNode = NULL;
	UTIL_DLLNode_t* pTargetNode = NULL;
	PrintEnter;
	pTempNode = node;
	while(pTempNode != NULL)
	{
		pTargetNode = pTempNode;
		if(pTempNode->pData != NULL)
		{
			PrintError("%s(%d) Data is exist \n",__FUNCTION__,__LINE__);
			PrintExit;
			return DLL_ERR_FAIL;
		}			
		pTargetNode = pTempNode;
		pTempNode = pTempNode->next;
		if(pTargetNode != NULL)
		{
			DD_MEM_Free(pTargetNode);
		}
	}
	PrintExit;
	return DLL_ERR_OK;
}

HUINT32 UTIL_DLLIST_Size (UTIL_DLL_t* dllist)
{
	PrintEnter;
	if(dllist == NULL)
	{
		PrintExit;
		return 0;
	}
	PrintExit;
	return dllist->unSize;
}


