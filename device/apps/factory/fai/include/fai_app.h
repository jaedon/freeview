/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

#ifndef _FAI_APP_H_
#define _FAI_APP_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_window.h"
#include "fai_text.h"
#include "fai_button.h"
#include "fai_msgbox.h"
#include "fai_testmenu.h"
#include "fai_app_config.h"
#include "fdi_key.h"

/* model specific configuration header */
#include "factorysw_config.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_PP_SHEET_1 = 0, 
	FAI_PP_SHEET_2, 
	FAI_PP_SHEET_3
} FAI_PP_SHEET;

typedef struct  
{
	FDI_KEY_DEVICE eDev;
	unsigned int nKeyCode;
	FDI_KEY_TYPE eKeyType;
} FAI_KEY_MSG;

typedef struct fai_appinfo
{
	/* pilot production window */
	FAI_WINDOW_H hPPMainWindow;
	FAI_WINDOW_H hPPMainTitle;
	
	FAI_BUTTON_H hPPSheetButton[PP_SHEET_MAX];
	FAI_WINDOW_H hPPSheetWindow[PP_SHEET_MAX];
	FAI_TESTMENU_H hPPMenu[PP_SHEET_MAX][PP_TOTAL_MENU_NUM];
	FAI_TESTMENU_H hRcuDispMenu;
	
	FAI_BUTTON_H hPPClearButton;

	FAI_PP_SHEET ePPCurrentSheet;
} FAI_APPINFO_T;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */
FAI_ERR_CODE FAI_APP_Init();
FAI_ERR_CODE FAI_APP_ClearTestMenus();
FAI_ERR_CODE FAI_APP_GetCurrentSheet(FAI_PP_SHEET *peCurSheet);
FAI_ERR_CODE FAI_APP_SetCurrentSheet(FAI_PP_SHEET eCurSheet);
FAI_ERR_CODE FAI_APP_GetSheetWindowHandle(FAI_PP_SHEET eSheet, FAI_WINDOW_H *phWin);
FAI_ERR_CODE FAI_APP_GetTestMenuHandle(FAI_PP_SHEET eSheet, unsigned char ucPos, FAI_TESTMENU_H *phMenu);
FAI_ERR_CODE FAI_APP_ChangeEthStateText(BOOL bIsLinked);
unsigned long FAI_APP_GetCurrentRcuKey();

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

#endif

