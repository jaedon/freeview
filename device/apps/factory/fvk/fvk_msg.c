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

/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : FVK_MSG_Create
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_Create (unsigned long ulMsgQueueCount, unsigned long ulMsgSize, char szMsgQueueName[], unsigned long *pulMsgQueueID)
{
	VK_ERR_CODE eVkRet = FVK_OK;

	eVkRet = VK_MSG_Create(ulMsgQueueCount, ulMsgSize, szMsgQueueName, pulMsgQueueID, VK_SUSPENDTYPE_FIFO);

	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}
 
	return eVkRet;
}


/******************************************************************************
* function : FVK_MSG_Send
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_Send (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize)
{
	VK_ERR_CODE eVkRet = FVK_OK;
	
	eVkRet = VK_MSG_Send(ulMsgQueueID, pvMsg, ulMsgSize);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}

	return eVkRet;
}


/******************************************************************************
* function : FVK_MSG_SendTimeout
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_SendTimeout (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize, unsigned long ulTimeOut)
{
	VK_ERR_CODE eVkRet = FVK_OK;
	
	eVkRet = VK_MSG_SendTimeout(ulMsgQueueID, pvMsg, ulMsgSize, ulTimeOut);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}

	return eVkRet;
}


/******************************************************************************
* function : FVK_MSG_Receive
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_Receive (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize)
{
	VK_ERR_CODE eVkRet = FVK_OK;
	
	eVkRet = VK_MSG_Receive(ulMsgQueueID, pvMsg, ulMsgSize);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}
	
	return eVkRet;
}


/******************************************************************************
* function : FVK_MSG_ReceiveTimeout
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_ReceiveTimeout (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize, unsigned long ulTimeOut)
{
	VK_ERR_CODE eVkRet = FVK_OK;

	eVkRet = VK_MSG_ReceiveTimeout(ulMsgQueueID, pvMsg, ulMsgSize, ulTimeOut);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}

	return eVkRet;
}


/* end of file */

