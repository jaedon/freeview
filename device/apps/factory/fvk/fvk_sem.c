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
/* global variables and structures */

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
* function : FVK_SEM_Create(binary semaphore)
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_Create(char *szSemName, unsigned long *pulSemID)
{
	VK_ERR_CODE eVkRet = FVK_OK;
	unsigned long ulInitCount=1;
#if defined(LIME_DDI) || defined(NEO_DDI)
	eVkRet = VK_SEM_CreateFifoWithCount(szSemName, pulSemID, ulInitCount);
#endif
	VK_SEM_Create(pulSemID, szSemName, VK_SUSPENDTYPE_FIFO);

	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}
	
	return eVkRet;
}


/******************************************************************************
* function : FVK_SEM_CreateCount(Count semaphore)
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_CreateCount(char *szSemName, unsigned long ulIntialCount, unsigned long *pulSemID)
{
	VK_ERR_CODE eVkRet = FVK_OK;

#if defined(LIME_DDI) || defined(NEO_DDI)
	eVkRet = VK_SEM_CreateFifoWithCount(szSemName, pulSemID, ulIntialCount);
#endif
	VK_SEM_CreateWithCount(pulSemID, ulIntialCount, szSemName, VK_SUSPENDTYPE_FIFO);

	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}
 	
	return eVkRet;

}


/******************************************************************************
* function : FVK_SEM_Get
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_Get(unsigned long ulSemID)
{
	VK_ERR_CODE eVkRet = FVK_OK;

 	eVkRet = VK_SEM_Get(ulSemID);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}

 	return eVkRet;
}


/******************************************************************************
* function : FVK_SEM_GetTimeout
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_GetTimeout(unsigned long ulSemID, unsigned long ulTimeoutMsec)
{
	VK_ERR_CODE eVkRet = FVK_OK;
#if defined(LIME_DDI) || defined(NEO_DDI)
	eVkRet = VK_SEM_GetWithTimeout(ulSemID, ulTimeoutMsec);
#endif
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}
 	
	return eVkRet;
}



/******************************************************************************
* function : FVK_SEM_Release
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_Release(unsigned long ulSemID)
{
	VK_ERR_CODE eVkRet = FVK_OK;

 	eVkRet = VK_SEM_Release(ulSemID);
	if(eVkRet != VK_OK)
	{
		eVkRet = FVK_ERR;
	}

 	return eVkRet;
}



/******************************************************************************
* function : FVK_SEM_GetCountvalue
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_GetCountvalue(unsigned long ulSemID, unsigned long* pulCountVal)
{
/*

	VK_ERR_CODE eVkRet = FVK_OK;

	eVkRet = VK_SEM_GetCountValue(ulSemID, pulCountVal);
	
		VK_ERR_CODE VK_SEM_GetCountValue(ulSemID, pulCountVal)
		{
			semaphore_t *sem;
			
			sem = (semaphore_t *)ulSemID;

			*pulCountVal = sem->Count;
		}

	if(eVkRet != VK_NO_ERR)
	{
		eVkRet = FVK_ERR;
	}
	return eVkRet;

*/	
	return FVK_ERR;
}




/* end of file */

