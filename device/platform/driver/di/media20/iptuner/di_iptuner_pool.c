/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_pool.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:
 * File Description:	     IP Tuner Pool API
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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "di_iptuner_config.h"
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_iptuner20.h"
#include "di_iptuner_pool.h"
#include "util_url.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif


#define UNUSED_PARAM(x)  ((void)x)
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_IPTUNER_POOL_t stIPTunerPool;
static unsigned long s_ulIPTunerPoolSemId;

/* Start iptuner result set static function prototypes */
static void P_IPTUNER_RESULT_SET_Init (DI_IPTUNER_SET_t *this);
static void P_IPTUNER_RESULT_SET_Push (DI_IPTUNER_SET_t *this, DI_IPTUNER_t *hdl);
static DI_IPTUNER_t* P_IPTUNER_RESULT_SET_Next (DI_IPTUNER_SET_t *this);
static DI_IPTUNER_t* P_IPTUNER_RESULT_SET_Prev (DI_IPTUNER_SET_t *this);
static void P_IPTUNER_RESULT_SET_Free (DI_IPTUNER_SET_t *this);
/* End iptuner result set static function prototypes */

/* Start local(static) function */
static DI_ERR_CODE P_IPTUNER_POOL_Preppend (DI_IPTUNER_t *newIPTuner);
static DI_IPTUNER_t* P_IPTUNER_POOL_GetAvailable (void);
/* End local(static) function */

#if __________p__________
#endif

#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

DI_ERR_CODE DI_IPTUNER_POOL_Init (void)
{
	// TODO: We have to check whether all nodes are deleted before delete all nodes before
    DI_IPTUNER_NODE_t *pTemp = stIPTunerPool.tail;
    DI_IPTUNER_NODE_t *pFree = NULL;
	HINT32 nRet = 0;

    PrintEnter;

    while(pTemp != NULL)
    {
       pFree = pTemp;
       pTemp = pTemp->prev;
       VK_free(pFree);
       pFree = NULL;
    }

	nRet = VK_SEM_Create(&s_ulIPTunerPoolSemId, "IP_TUNER_POOL_SEM", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		PrintExit;
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(&stIPTunerPool,0x00,sizeof(DI_IPTUNER_POOL_t));

	PrintExit;
	return DI_ERR_OK;
}

/* TODO: We have to implement this function by ykkim ^^. */
DI_ERR_CODE DI_IPTUNER_POOL_Uninit (void)
{
	/* TODO : will be implemented by ykkim */
    DI_IPTUNER_NODE_t *pTemp = stIPTunerPool.tail;
    DI_IPTUNER_NODE_t *pFree = NULL;
    PrintEnter;

    while(pTemp != NULL)
    {
       pFree = pTemp;
       pTemp = pTemp->prev;
       DI_IPTUNER_POOL_FreeTuner(pFree->pIPTunerHdl);
       VK_free(pFree);
       pFree = NULL;
    }
	PrintExit;
	return DI_ERR_OK;
}


DI_IPTUNER_t* DI_IPTUNER_POOL_AllocTuner (void)
{
	DI_IPTUNER_t *pIPTunerHdl = NULL;
	PrintEnter;
	/* 1. First, Searching for available tuner in POOL */
	pIPTunerHdl = P_IPTUNER_POOL_GetAvailable();

	if(pIPTunerHdl != NULL)
	{
		PrintExit;
		return pIPTunerHdl;
	}

	PrintDebug ("%s(%d) %d bytes \n",__FUNCTION__,__LINE__,sizeof(DI_IPTUNER_t));

	/* 2. If not exists, a tuner handle has to be created. */
	pIPTunerHdl = (DI_IPTUNER_t *)VK_MEM_Alloc(sizeof(DI_IPTUNER_t));
	if(pIPTunerHdl == NULL)
	{
		PrintExit;
		return NULL;
	}

	VK_memset(pIPTunerHdl,0x00,sizeof(DI_IPTUNER_t));
	pIPTunerHdl->isAvailable = false;

	/* 3. Insert a new tuner into the POOL */
	if(P_IPTUNER_POOL_Preppend(pIPTunerHdl) != DI_ERR_OK)
	{
		if(pIPTunerHdl != NULL)
		{
			VK_free(pIPTunerHdl);
			pIPTunerHdl = NULL;
		}
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pIPTunerHdl;
}

DI_ERR_CODE DI_IPTUNER_POOL_FreeTuner (DI_IPTUNER_t *iptuner)
{
	DI_IPTUNER_NODE_t *pTemp;
	PrintEnter;
	VK_SEM_Get(s_ulIPTunerPoolSemId);
	if(stIPTunerPool.tail == NULL)
	{
		VK_SEM_Release(s_ulIPTunerPoolSemId);
		return DI_ERR_OK; // Error ?
	}
	pTemp = stIPTunerPool.tail;

	while(pTemp != NULL)
	{
		if(pTemp->pIPTunerHdl != NULL && pTemp->pIPTunerHdl == iptuner)
		{
			/* TODO : we have to release a heap region in url,filename */
			if( pTemp->pIPTunerHdl->pUrl != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pUrl);
				pTemp->pIPTunerHdl->pUrl = NULL;
			}

			if (pTemp->pIPTunerHdl->pSaveFileName != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pSaveFileName);
				pTemp->pIPTunerHdl->pSaveFileName = NULL;
			}

			if (pTemp->pIPTunerHdl->pIndexTimeLineUrl != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pIndexTimeLineUrl);
				pTemp->pIPTunerHdl->pIndexTimeLineUrl = NULL;
			}

			if (pTemp->pIPTunerHdl->pUserAgentString != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pUserAgentString);
				pTemp->pIPTunerHdl->pUserAgentString = NULL;
			}

			if (pTemp->pIPTunerHdl->pIndexRoom != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pIndexRoom);
				pTemp->pIPTunerHdl->pIndexRoom = NULL;
			}

			if (pTemp->pIPTunerHdl->pHStream != NULL)
			{
				VK_MEM_Free (pTemp->pIPTunerHdl->pHStream);
				pTemp->pIPTunerHdl->pHStream = NULL;
			}

			if(pTemp->pIPTunerHdl->pVfioHandle !=NULL)
			{
				VK_MEM_Free(pTemp->pIPTunerHdl->pVfioHandle);
				pTemp->pIPTunerHdl->pVfioHandle = NULL;
			}

			VK_memset(pTemp->pIPTunerHdl,0x00,sizeof(DI_IPTUNER_t));
			pTemp->pIPTunerHdl->isAvailable = true;

			if(pTemp == stIPTunerPool.tail && pTemp == stIPTunerPool.head )
			{
				stIPTunerPool.tail = NULL;
				stIPTunerPool.head = NULL;
				//pTemp->prev->next = NULL;
			}
			else if(pTemp == stIPTunerPool.tail)
			{
				stIPTunerPool.tail = pTemp->prev;
				pTemp->prev->next = NULL;
			}
			else if(pTemp == stIPTunerPool.head)
			{
				stIPTunerPool.head = pTemp->next;
				pTemp->next->prev = NULL;
			}
			else
			{
				pTemp->next->prev = pTemp->prev;
				pTemp->prev->next = pTemp->next;
			}
			VK_MEM_Free(pTemp->pIPTunerHdl);
			pTemp->pIPTunerHdl = NULL;

			VK_MEM_Free(pTemp);
			pTemp = NULL;

			stIPTunerPool.size--;
			VK_SEM_Release(s_ulIPTunerPoolSemId);
			PrintExit;
			return DI_ERR_OK;
		}
		else
		{
			pTemp = pTemp->prev;
		}
	}
	VK_SEM_Release(s_ulIPTunerPoolSemId);
	PrintExit;
	return DI_ERR_OK; // Error ?
}

static DI_ERR_CODE P_IPTUNER_POOL_Preppend (DI_IPTUNER_t *newIPTuner)
{
	DI_IPTUNER_NODE_t *pTemp;
	DI_IPTUNER_NODE_t *pNewNode;
	PrintEnter;
	if(newIPTuner == NULL)
	{
		PrintExit;
		return DI_ERR_NO_ALLOC;
	}
	VK_SEM_Get(s_ulIPTunerPoolSemId);
	pNewNode = (DI_IPTUNER_NODE_t *)VK_MEM_Alloc(sizeof(DI_IPTUNER_NODE_t));
	VK_memset(pNewNode,0x00,sizeof(DI_IPTUNER_NODE_t));

	pNewNode->pIPTunerHdl = newIPTuner;
	pTemp = stIPTunerPool.head;

	stIPTunerPool.head = pNewNode;
	pNewNode->next = pTemp;
	stIPTunerPool.size++;
	stIPTunerPool.capacity++;
	if(pTemp != NULL)
	{
		pTemp->prev = pNewNode;
	}
	else
	{
		stIPTunerPool.tail = pNewNode;
	}
	VK_SEM_Release(s_ulIPTunerPoolSemId);
	PrintExit;
	return DI_ERR_OK;
}

static DI_IPTUNER_t* P_IPTUNER_POOL_GetAvailable (void)
{
	DI_IPTUNER_NODE_t *pTemp=NULL;

	PrintEnter;

	VK_SEM_Get(s_ulIPTunerPoolSemId);
	if(stIPTunerPool.tail == NULL)
	{
		VK_SEM_Release(s_ulIPTunerPoolSemId);
		PrintExit;
		return NULL;
	}

	pTemp = stIPTunerPool.tail;

	while(pTemp != NULL)
	{
		if(pTemp->pIPTunerHdl->isAvailable == true)
		{
			pTemp->pIPTunerHdl->isAvailable = false;
			stIPTunerPool.size++;
			VK_SEM_Release(s_ulIPTunerPoolSemId);
			PrintExit;
			return pTemp->pIPTunerHdl;
		}
		else
		{
			pTemp = pTemp->prev;
		}
	}
	VK_SEM_Release(s_ulIPTunerPoolSemId);
	PrintExit;
	return NULL;
}


HINT32 DI_IPTUNER_POOL_Size (void)
{
	PrintEnter;
	PrintExit;
	return stIPTunerPool.size;
}

HINT32 DI_IPTUNER_POOL_Capacity (void)
{
	PrintEnter;
	PrintExit;
	return stIPTunerPool.capacity;
}

DI_ERR_CODE DI_IPTUNER_POOL_FindByType (DI_IPTUNER_SET_t* resultSet, DI_IPTUNER_ProtocolType_e type)
{
    DI_IPTUNER_NODE_t *pTemp=NULL;
	PrintEnter;
    P_IPTUNER_RESULT_SET_Init(resultSet);

    if(stIPTunerPool.tail == NULL)
    {
	    PrintExit;
        return DI_ERR_OK;
    }

    pTemp = stIPTunerPool.tail;

    while(pTemp != NULL)
    {
        if(pTemp->pIPTunerHdl->eProtocolType == type && pTemp->pIPTunerHdl->isAvailable == 0)
        {
           resultSet->push(resultSet,pTemp->pIPTunerHdl);
        }
        pTemp = pTemp->prev;
    }
    PrintExit;
    return DI_ERR_OK;
}



HBOOL DI_IPTUNER_POOL_IsValidHandle (const DI_IPTUNER_t* pHTuner)
{
    DI_IPTUNER_NODE_t *pTemp=NULL;
	PrintEnter;

    if(stIPTunerPool.tail == NULL)
    {
		PrintError ("%s(%d) Error> IPTUNER Pool has no members.\n",__FUNCTION__,__LINE__);
		PrintExit;
        return FALSE;
    }

    pTemp = stIPTunerPool.tail;

    while (pTemp != NULL)
    {
        if((HUINT32)pTemp->pIPTunerHdl == (HUINT32)pHTuner)
		{
			PrintExit;
			return TRUE;
		}
       	pTemp = pTemp->prev;
    }
	PrintExit;
    return FALSE;
}

DI_IPTUNER_t* DI_IPTUNER_POOL_FindById (HUINT32 Id)
{
    DI_IPTUNER_NODE_t *pTemp;
	PrintEnter;
    if(stIPTunerPool.tail == NULL)
    {
	    PrintExit;
        return NULL;
    }

    pTemp = stIPTunerPool.tail;

    while(pTemp != NULL)
    {
        if((HUINT32)pTemp->pIPTunerHdl == Id)
        {
	        PrintExit;
            return pTemp->pIPTunerHdl;
        }
        pTemp = pTemp->prev;
    }
    PrintExit;
    return NULL;
}

DI_IPTUNER_t* DI_IPTUNER_POOL_FindByVfioHandle (void *pVfio)
{
	DI_IPTUNER_NODE_t *pTemp;
	PrintEnter;
    if(stIPTunerPool.tail == NULL)
    {
	    PrintExit;
        return NULL;
    }
    pTemp = stIPTunerPool.tail;
	if(pVfio == NULL)
	{
    	PrintError("%s(%d) pVfio is NULL\n",__FUNCTION__,__LINE__);
	}

    while(pTemp != NULL)
    {
    	PrintError("%s(%d) Node : %d, Handle : %d\n",__FUNCTION__,__LINE__,(HUINT32)pTemp->pIPTunerHdl->pVfioHandle,(HUINT32)pVfio);
        if((HUINT32)pTemp->pIPTunerHdl->pVfioHandle == (HUINT32)pVfio)
        {
	        PrintExit;
            return pTemp->pIPTunerHdl;
        }
        pTemp = pTemp->prev;
    }
    PrintExit;
    return NULL;
}

DI_IPTUNER_t* DI_IPTUNER_POOL_FindBySaveFileName (HINT8* pName)
{
    DI_IPTUNER_NODE_t *pTemp;
	PrintEnter;
    if(stIPTunerPool.tail == NULL || pName == NULL)
    {
	    PrintExit;
        return NULL;
    }
    pTemp = stIPTunerPool.tail;
	PrintDebug("%s(%d) FindBySaveFileName : %s, len: %d\n",__FUNCTION__,__LINE__,pName,VK_strlen((char*)pName));
    while(pTemp != NULL)
    {
    	if(pTemp->pIPTunerHdl != NULL && pTemp->pIPTunerHdl->pSaveFileName != NULL)
   		{
	     	if ( !VK_strcmp(pName,pTemp->pIPTunerHdl->pSaveFileName))
    	    {
	    	    PrintExit;
            	return pTemp->pIPTunerHdl;
	        }
   		}
    	else
   		{
    		PrintDebug("%s(%d) Handle or pSaveFileName is NULL\n",__FUNCTION__,__LINE__);
   		}
        pTemp = pTemp->prev;
    }
    PrintExit;
    return NULL;
}

DI_IPTUNER_t* DI_IPTUNER_POOL_FindByUrl (const HINT8* pName)
{
    DI_IPTUNER_NODE_t *pTemp;
	PrintEnter;
    if(stIPTunerPool.tail == NULL || pName == NULL)
    {
	    PrintExit;
        return NULL;
    }
    pTemp = stIPTunerPool.tail;
	PrintDebug("%s(%d) Url Name : %s, len: %d\n",__FUNCTION__,__LINE__,pName,VK_strlen((char *)pName));
    while(pTemp != NULL)
    {
    	if( (pTemp->pIPTunerHdl != NULL)
			&& (pTemp->pIPTunerHdl->pUrl != NULL)
#if 0
			&& (pTemp->pIPTunerHdl->pSaveFileName == NULL) /* DRAGON TEST for Probing */
#endif
			)
   		{
	     	if ( !VK_strcmp(pName,pTemp->pIPTunerHdl->pUrl))
    	    {
	    	    PrintExit;
            	return pTemp->pIPTunerHdl;
	        }
   		}
    	else
   		{
    		PrintDebug("%s(%d) Handle or pSaveFileName is NULL\n",__FUNCTION__,__LINE__);
   		}
        pTemp = pTemp->prev;
    }
    PrintExit;
    return NULL;
}


static void P_IPTUNER_RESULT_SET_Init (DI_IPTUNER_SET_t *this)
{
	PrintEnter;
	this->push = P_IPTUNER_RESULT_SET_Push;
	this->next = P_IPTUNER_RESULT_SET_Next;
    this->prev = P_IPTUNER_RESULT_SET_Prev;
    this->VK_free = P_IPTUNER_RESULT_SET_Free;
    this->VK_free(this);
    PrintExit;
}

static void P_IPTUNER_RESULT_SET_Push (DI_IPTUNER_SET_t *this, DI_IPTUNER_t *hdl)
{
	DI_IPTUNER_NODE_t *pTemp;
	DI_IPTUNER_NODE_t *pNewNode;
	PrintEnter;
	pNewNode = (DI_IPTUNER_NODE_t *)VK_MEM_Alloc(sizeof(DI_IPTUNER_NODE_t));
	VK_memset(pNewNode,0x00,sizeof(DI_IPTUNER_NODE_t));

	pNewNode->pIPTunerHdl = hdl;
	pTemp = this->list.head;
	this->list.head = pNewNode;
	pNewNode->next = pTemp;
	this->size++;
	if(pTemp != NULL)
	{
		pTemp->prev = pNewNode;
	}
	else
	{
		this->current = this->list.head;
		this->list.tail = pNewNode;
	}
	PrintExit;
}

static DI_IPTUNER_t* P_IPTUNER_RESULT_SET_Next (DI_IPTUNER_SET_t *this)
{
	DI_IPTUNER_NODE_t *pTemp = this->current;
	PrintEnter;
	if(this->eStatus == DI_IPTUNER_SET_STATUS_TAIL)
	{
		PrintExit;
		return NULL;
	}
	if(this->list.tail == pTemp)
	{
		this->eStatus = DI_IPTUNER_SET_STATUS_TAIL;
		PrintExit;
		return pTemp->pIPTunerHdl;
	}
	if(pTemp != NULL)
	{
		this->current = pTemp->next;
		this->eStatus = DI_IPTUNER_SET_STATUS_SET;
	}
	else
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pTemp->pIPTunerHdl;
}

static DI_IPTUNER_t* P_IPTUNER_RESULT_SET_Prev (DI_IPTUNER_SET_t *this)
{
	DI_IPTUNER_NODE_t *pTemp = this->current;
	PrintEnter;
	if(this->eStatus == DI_IPTUNER_SET_STATUS_HEAD)
	{
		PrintExit;
		return NULL;
	}
	if(this->list.head == pTemp)
	{
		this->eStatus = DI_IPTUNER_SET_STATUS_HEAD;
		PrintExit;
		return pTemp->pIPTunerHdl;
	}
	if(pTemp != NULL)
	{
		this->current = pTemp->prev;
		this->eStatus = DI_IPTUNER_SET_STATUS_SET;
	}
	else
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pTemp->pIPTunerHdl;
}

static void P_IPTUNER_RESULT_SET_Free (DI_IPTUNER_SET_t *this)
{
	DI_IPTUNER_NODE_t *pTemp = this->list.tail;
	DI_IPTUNER_NODE_t *pFree = NULL;
	PrintEnter;
	this->size = 0;
	this->current = NULL;
	this->eStatus = DI_IPTUNER_SET_STATUS_SET;

	while (pTemp != NULL)
	{
		pFree = pTemp;
		pTemp = pTemp->prev;
		VK_MEM_Free (pFree);
		pFree = NULL;
	}
	PrintExit;
}

