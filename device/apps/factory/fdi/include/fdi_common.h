/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_COMMON_H_
#define _FDI_COMMON_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "factorysw_def.h"
#include "BOX_factorysw.h"
#include "BOX_factorysw_platform.h"
#include "ftype.h"
#include "fdi_err.h"
#include "fdi_print.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
//#define LIME_DDI
//#define NEO_DDI

typedef enum
{
	FDI_PRT_ERROR = 0, 
	FDI_PRT_WARNING, 
	FDI_PRT_DBGINFO
} FDI_PRT_TYPE;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */
extern int g_nFdiDebugLevel;

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

#define FDI_DEBUG_ENABLE
#ifdef FDI_DEBUG_ENABLE
	#define	FDI_FUNC_ENTER	if( FDI_PRT_DBGINFO <= g_nFdiDebugLevel ) FDI_Print("[%s] ++++ \n", __func__)
	#define FDI_FUNC_EXIT	if( FDI_PRT_DBGINFO <= g_nFdiDebugLevel ) FDI_Print("[%s] ---- \n", __func__)
	#define FDI_PRINT(x, y) if( x <= g_nFdiDebugLevel ) FDI_Print y
#else
	#define	FDI_FUNC_ENTER
	#define FDI_FUNC_EXIT
	#define FDI_PRINT(x, y)
#endif
// fdi print ±¸Çö

#define OCTO_DI

#endif


