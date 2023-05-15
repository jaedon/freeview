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
#if !defined(CONFIG_OS_RTOS)
#include <memory.h>
#endif
#include <string.h>
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
* function : FVK_MEM_Alloc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void* FVK_MEM_Alloc(unsigned long ulSize)
{
	void* pvBuf;

	pvBuf = VK_MEM_Alloc(ulSize);

	return pvBuf;
}

/******************************************************************************
* function : FVK_MEM_AllocUncached
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void* FVK_MEM_AllocUncached(unsigned long ulSize, unsigned long alignsize)
{
	void* pvBuf = NULL;
	
	return pvBuf;
}



/******************************************************************************
* function : FVK_MEM_Free
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MEM_Free(void *p)
{
	VK_MEM_Free(p);

	return FVK_OK;
}



/******************************************************************************
* function : FVK_MEM_FreeUncached
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MEM_FreeUncached(void *p)
{
	return FVK_OK;
}



/******************************************************************************
* function : FVK_MEM_Copy
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void FVK_MEM_Copy(void *pDest, void *pSrc, unsigned int unSize)
{
	VK_memcpy(pDest, pSrc, unSize);
}


/******************************************************************************
* function : FVK_MEM_Set
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void FVK_MEM_Set(void *pPtr, int nValue, unsigned int unSize)
{
	VK_memset(pPtr, nValue, unSize);
}

/******************************************************************************
* function : FVK_MEM_Compare
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int FVK_MEM_Compare(void *pPtr1, void *pPtr2, unsigned int unSize)
{
	return VK_memcmp(pPtr1, pPtr2, unSize);
}

/* end of file */

