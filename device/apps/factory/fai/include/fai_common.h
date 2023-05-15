/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_COMMON_H_
#define _FAI_COMMON_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "ftype.h"
#include "fai_err.h"
#include "fdi_print.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/

typedef enum
{
	FAI_PRT_ERROR = 0, 
	FAI_PRT_WARNING, 
	FAI_PRT_DBGINFO
} FAI_PRT_TYPE;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */
extern int g_nFaiDebugLevel;

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

#define FAI_DEBUG_ENABLE
#ifdef FAI_DEBUG_ENABLE
	#define	FAI_FUNC_ENTER	if( FAI_PRT_DBGINFO <= g_nFaiDebugLevel ) FDI_Print("[%s] ++++ \n", __func__)
	#define FAI_FUNC_EXIT	if( FAI_PRT_DBGINFO <= g_nFaiDebugLevel ) FDI_Print("[%s] ---- \n", __func__)
	#define FAI_PRINT(x, y) if( x <= g_nFaiDebugLevel ) FDI_Print y
#else
	#define	FAI_FUNC_ENTER
	#define FAI_FUNC_EXIT
	#define FAI_PRINT(x, y)
#endif

#define GET_STEP() FAI_TESTMENU_GetCurrentMenuStep()
#define SET_STEP(x) FAI_TESTMENU_SetCurrentMenuStep(x)

#endif

