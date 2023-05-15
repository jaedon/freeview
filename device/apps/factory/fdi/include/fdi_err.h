/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_ERR_H_
#define _FDI_ERR_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include <stdio.h>

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	// General FDI Error Code
	FDI_NO_ERR					= 0,
	FDI_ERR 					= 1,
	FDI_ERR_INVALID_PARAMETER	= 2,
	FDI_ERR_MEM_ALLOC			= 3,
	FDI_ERR_WAIT				= 4, 
	FDI_ERR_RESOLUTION, 	
	FDI_ERR_ASPECT,
	FDI_FEATURE_NOT_SUPPORTED,

} FDI_ERR_CODE;
	
#if !defined(CONFIG_PLATFORM_ALI)
#define FAI_ERR_CHECK(err)									\
{															\
	if( err != FAI_NO_ERR ) 								\
	{														\
		fprintf(stderr, "%s <%d>:\n", __FILE__, __LINE__);	\
		return FAI_ERR;										\
	}														\
}
#else
#define FAI_ERR_CHECK(err)									\
{															\
	if( err != FAI_NO_ERR ) 								\
	{														\
		printf("%s <%d>:\n", __FILE__, __LINE__);	\
		return FAI_ERR;										\
	}														\
}
#endif															\


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

#endif

