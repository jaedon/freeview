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
#ifndef _FAI_TEXT_H_
#define _FAI_TEXT_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_widget.h"
#include "fai_window.h"
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum {
	TEXT_ALIGN_NOT_USE = 0, 
	TEXT_ALIGN_LEFT, 
	TEXT_ALIGN_MIDDLE, 
	TEXT_ALIGN_RIGHT
} TEXT_ALIGN_TYPE;
	
typedef struct fai_text {
  FAI_WIDGET_T tWidget;

  unsigned int unTextColor;
  unsigned int unBgColor;
  char *pcText; 
  TEXT_ALIGN_TYPE eAlignType;

  FAI_WINDOW_H ptParentWindow;
} FAI_TEXT_T;

typedef FAI_TEXT_T *FAI_TEXT_H;

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
* function : FAI_TEXT_Create
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Create(FAI_TEXT_H *phText, 
							FAI_WINDOW_H hWin, 
							unsigned int unX, 
							unsigned int unY, 
							unsigned int unW, 
							unsigned int unH, 
							unsigned int unTextColor, 
							unsigned int unBgColor, 
							char *pcStr, 
							TEXT_ALIGN_TYPE eAlignType);

/******************************************************************************
* function : FAI_TEXT_Destroy
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Destroy(FAI_TEXT_H hText);

/******************************************************************************
* function : FAI_TEXT_Draw
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Draw(FAI_TEXT_H hText);

/******************************************************************************
* function : FAI_TEXT_Redraw
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Redraw(FAI_TEXT_H hText);

/******************************************************************************
* function : FAI_TEXT_Change
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Change(FAI_TEXT_H hText, 
							unsigned int unTextColor, 
							unsigned int unBgColor, 
							char *pcStr);

/******************************************************************************
* function : FAI_TEXT_ChangeNoDraw
* description : 텍스트 내용값을 교체한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_ChangeNoDraw(FAI_TEXT_H hText, 
								unsigned int unTextColor, 
								unsigned int unBgColor, 
								char *pcStr);

#endif 

