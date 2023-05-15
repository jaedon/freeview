/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/******************************************************************************
* header and definitions
******************************************************************************/
/* humax header files */
#include "vkernel.h"
#include "fvk.h"

/******************************************************************************
* Debug level
******************************************************************************/

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef struct CB_FUNC_LIST_T
{
	FVK_TIMER_MODE eMode;
	unsigned long ulTimerId;
	pfnFVK_TIMER_EVT_CALLBACK pfnCallback;
	struct CB_FUNC_LIST_T *ptNext;
}CB_FUNC_LIST;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */


static CB_FUNC_LIST s_tCBFuncListHeader;

/******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE INTRN_FVK_TIMER_AddCallBackFuncList (CB_FUNC_LIST *ptCBFuncList)
{
	CB_FUNC_LIST *ptCurList, *ptNewList;

	ptNewList = FVK_MEM_Alloc(sizeof(CB_FUNC_LIST));
	if(ptNewList == NULL)
	{
		return FVK_ERR;
	}

	FVK_MEM_Copy(ptNewList, ptCBFuncList, sizeof(CB_FUNC_LIST));
	
	ptCurList = &s_tCBFuncListHeader;
	while(ptCurList->ptNext != NULL)
	{
		ptCurList = ptCurList->ptNext;
	}
	ptCurList->ptNext = ptNewList;
	return FVK_NO_ERR;
}

/******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
CB_FUNC_LIST *INTRN_FVK_TIMER_GetCallBackFuncList (unsigned long ulTimerId)
{
	CB_FUNC_LIST *ptCurList;

	ptCurList = s_tCBFuncListHeader.ptNext;
	while(ptCurList)
	{
		if(ptCurList->ulTimerId == ulTimerId)
		{
			return ptCurList;
		}

		ptCurList = ptCurList->ptNext;
	}
	return (CB_FUNC_LIST *)NULL;
}

/******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE INTRN_FVK_TIMER_DelelteCallBackFuncList (unsigned long ulTimerId)
{
	CB_FUNC_LIST *ptPreList, *ptCurList;

	ptPreList = &s_tCBFuncListHeader;
	ptCurList = s_tCBFuncListHeader.ptNext;
	while(ptCurList)
	{
		if(ptCurList->ulTimerId == ulTimerId)
		{
			ptPreList->ptNext = ptCurList->ptNext;
			
			FVK_MEM_Free(ptCurList);
			return FVK_NO_ERR;
		}
		
		ptPreList = ptCurList;	
		ptCurList = ptCurList->ptNext;
	}	
	return FVK_ERR;
}


/******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void INTRN_FVK_TIMER_EvntCallback(unsigned long ulTimerId, void *pvParams)
{
	CB_FUNC_LIST *ptCBFuncList;

	ptCBFuncList = INTRN_FVK_TIMER_GetCallBackFuncList(ulTimerId);
	if(ptCBFuncList == NULL)
	{
		return;
	}

	/* Call CallbackFunction */
	ptCBFuncList->pfnCallback(ulTimerId, pvParams);

	if(ptCBFuncList->eMode == FVK_TIMER_ONCE)
	{
		/* Call CallbackFunction */
		if(INTRN_FVK_TIMER_DelelteCallBackFuncList(ulTimerId) != FVK_NO_ERR)
		{
			return;
		}
	}
}


/******************************************************************************
* function : FVK_TIMER_CreateStart
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TIMER_CreateStart(unsigned long ulTimeOut, pfnFVK_TIMER_EVT_CALLBACK pfnCallback, void *pArgs, unsigned long ulArgsSize, FVK_TIMER_MODE eMode, unsigned long *pulTimerId)
{   
	VK_ERR_CODE eVkRet;
	CB_FUNC_LIST tCBFuncList;

	if(eMode == FVK_TIMER_ONCE)
	{
		eVkRet = VK_TIMER_EventAfter(ulTimeOut, INTRN_FVK_TIMER_EvntCallback, pArgs, ulArgsSize, pulTimerId);
		if(eVkRet != VK_OK)
		{
			return FVK_ERR; 
		}
	}
	else if(eMode == FVK_TIMER_REPEAT)
	{
		eVkRet = VK_TIMER_EventEvery(ulTimeOut, INTRN_FVK_TIMER_EvntCallback, pArgs, ulArgsSize, pulTimerId);
		if(eVkRet != VK_OK)
		{
			return FVK_ERR;
		}		

	}

	/* add list */
	tCBFuncList.eMode = eMode;
	tCBFuncList.ulTimerId = *pulTimerId;
	tCBFuncList.pfnCallback = pfnCallback;
	tCBFuncList.ptNext = NULL;
	if(INTRN_FVK_TIMER_AddCallBackFuncList(&tCBFuncList) != FVK_NO_ERR)
	{
		return FVK_ERR;
	}

	return FVK_NO_ERR;
}



/******************************************************************************
* function : FVK_TIMER_Destory
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TIMER_Destory(unsigned long ulTimerId)
{
	VK_ERR_CODE eVkRet;

	/* delete list */
	if(INTRN_FVK_TIMER_DelelteCallBackFuncList(ulTimerId) != FVK_NO_ERR)
	{
		return FVK_ERR;
	}
	
	eVkRet = VK_TIMER_Cancel(ulTimerId);
	if(eVkRet != VK_OK)
	{
		return FVK_ERR;
	}
	
	return FVK_NO_ERR;
}


/* end of file */

