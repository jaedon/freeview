/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_BUTTON_H_
#define _FAI_BUTTON_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_widget.h"
#include "fai_text.h"
#include "fai_window.h"
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef struct fai_button 
{
  FAI_WIDGET_T tWidget;

  unsigned int unFocusColor;
  unsigned int unNoFocusColor;
  char *pcText;   

  TEXT_ALIGN_TYPE eAlignType;
  FAI_WINDOW_H ptParentWindow;
} FAI_BUTTON_T;

typedef FAI_BUTTON_T *FAI_BUTTON_H;

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
* function : FAI_CreateButton
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Create(FAI_BUTTON_H *phBt, 
								FAI_WINDOW_H hWin, 
								unsigned int unX, 
								unsigned int unY, 
								unsigned int unW, 
								unsigned int unH, 
								unsigned int unFocusColor, 
								unsigned int unNoFocusColor, 
								char *pcStr, 
								TEXT_ALIGN_TYPE eAlignType);

/******************************************************************************
* function : FAI_DestroyButton
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Destroy(FAI_BUTTON_H hBt);

/******************************************************************************
* function : FAI_DrawButton
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Draw(FAI_BUTTON_H hBt);

/******************************************************************************
* function : FAI_RedrawButton
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Redraw(FAI_BUTTON_H hBt);

/******************************************************************************
* function : FAI_SetFocusButton
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_SetFocus(FAI_BUTTON_H hBt, FAI_WIDGET_STATUS eFocus);

#endif 

