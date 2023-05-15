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
#ifndef _FAI_TESTMENU_H_
#define _FAI_TESTMENU_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_window.h"
#include "fai_text.h"
#include "fai_err.h"
#include "fai_font.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum 
{ 
	TEST_MENU_TYPE_NORMAL = 0, 
	TEST_MENU_TYPE_KEY
} TEST_MENU_TYPE;

typedef enum
{
	TESTMENU_LINE_1 = 1, 
	TESTMENU_LINE_2, 
	TESTMENU_LINE_3, 
	TESTMENU_LINE_4,
	TESTMENU_LINE_RCU
} TESTMENU_LINE;

typedef struct fai_testmenu {
  FAI_WIDGET_T tWidget;
  char acTestName[3][20];
  char acInputRcuName[20];
  
  FAI_TEXT_H ptTestMenuLine1;
  FAI_TEXT_H ptTestMenuLine2;
  FAI_TEXT_H ptTestMenuLine3;
  FAI_TEXT_H ptTestMenuLine4;  
  FAI_TEXT_H ptInputRCUText;
  TEST_MENU_TYPE eType;
  FAI_WINDOW_H ptParentWindow;

  unsigned int unStep;
} FAI_TESTMENU_T;

typedef FAI_TESTMENU_T *FAI_TESTMENU_H;

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
* function : FAI_TESTMENU_Create
* description : 테스트 메뉴를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Create(FAI_TESTMENU_H *phMenu, 
								FAI_WINDOW_H hWin, 
								char *pcTestName, 
								char *pcInputRcuText, 
								unsigned int unX, 
								unsigned int unY, 
								unsigned int unW, 
								unsigned int unH, 
								TEST_MENU_TYPE eType);

/******************************************************************************
* function : FAI_TESTMENU_Draw
* description : 테스트 메뉴를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Draw(FAI_TESTMENU_H hMenu);

/******************************************************************************
* function : FAI_TESTMENU_Draw
* description : 테스트 메뉴를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenuText(FAI_TESTMENU_H hMenu, 
											TESTMENU_LINE eLine, 
											char *pcText, 
											unsigned int unTextColor, 
											unsigned int unBgColor, 
											BOOL bReDraw);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE eLine, char *pcText, unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentMenuColor(unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentRCUColor(unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenu(FAI_TESTMENU_H hMenu, TESTMENU_LINE eLine, char *pcText, unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenuColor(FAI_TESTMENU_H hMenu, unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeRCUColor(FAI_TESTMENU_H hMenu, unsigned int unBgColor);

/******************************************************************************
* function : FAI_TESTMENU_Destroy
* description : 테스트 메뉴를 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Destroy(FAI_TESTMENU_H hMenu);

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenu
* description : 테스트 메뉴를 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetCurrentMenu(FAI_TESTMENU_H hMenu);

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenu
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_GetCurrentMenu(FAI_TESTMENU_H *phMenu);

/******************************************************************************
* function : FAI_TESTMENU_Reset
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Reset(FAI_TESTMENU_H hMenu);

/******************************************************************************
* function : FAI_TESTMENU_SetStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetStep(FAI_TESTMENU_H hMenu, unsigned int unStep); // remove

/******************************************************************************
* function : FAI_TESTMENU_GetStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned int FAI_TESTMENU_GetStep(FAI_TESTMENU_H hMenu);

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenuStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetCurrentMenuStep(unsigned int unStep);

/******************************************************************************
* function : FAI_TESTMENU_GetCurrentMenuStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned int FAI_TESTMENU_GetCurrentMenuStep();

#endif 

