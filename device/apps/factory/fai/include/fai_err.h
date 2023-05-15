/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_ERR_H_
#define _FAI_ERR_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include <stdio.h>

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	// General FAI Error Code
	FAI_NO_ERR					= 0,
	FAI_ERR 					= 1,
	FAI_ERR_INVALID_PARAMETER	= 2,
	FAI_ERR_MEM_ALLOC			= 3,
	FAI_ERR_MES_CONNECT,
	FAI_ERR_MES_UPLOAD,
	FAI_ERR_MES_DOWNLOAD,
	FAI_ERR_MES_CHECK_DOWNLOAD,
	FAI_ERR_MES_OTP,
	FAI_FEATURE_NOT_SUPPORTED
} FAI_ERR_CODE;

typedef enum
{
	FAI_COMMON_TMD_ERR_FAIL					= 0,
	FAI_COMMON_TMD_ERR_INVALID_PARAMETER,
	FAI_COMMON_TMD_ERR_INVALID_ARGUMENT,	
	FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL,
	FAI_COMMON_TMD_ERR_MEMORY_ALLOC_FAIL,
	FAI_COMMON_TMD_ERR_NOT_SUPPORTED_FEATURE 
}FAI_COMMON_TMD_ERR;

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
#endif														\


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

