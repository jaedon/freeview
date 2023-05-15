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
#ifndef _FAI_WINDOW_H_
#define _FAI_WINDOW_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "list.h"
#include "fai_widget.h"
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum {
	FAI_WINDOW_NORMAL = 0, 
	FAI_WINDOW_MSGBOX
} FAI_WINDOW_TYPE;

typedef FAI_ERR_CODE (*pfnFAI_EVENTHANDLER_T)(unsigned int unDeviceType, unsigned int unKeyCode);

typedef struct fai_window {
  FAI_WIDGET_T tWidget;

  FAI_WIDGET_T *ptFocusWidget;
  
  unsigned int unWinColor;
  LIST_T widgets;

  struct fai_window *ptParentWindow;
  struct fai_window *ptChildFocusWindow;
  
  FAI_WINDOW_TYPE eType;

  pfnFAI_EVENTHANDLER_T pfnEventHandler;
} FAI_WINDOW_T;

typedef FAI_WINDOW_T *FAI_WINDOW_H;

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
* function : FAI_WINDOW_Create
* description : 위도우를 생성한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Create(FAI_WINDOW_H *phWin, 
								char *pcName, 
								unsigned int unX, 
								unsigned int unY, 
								unsigned int unW, 
								unsigned int unH, 
								unsigned int unColor, 
								FAI_WINDOW_TYPE eType, 
								pfnFAI_EVENTHANDLER_T pfnHandler);

/******************************************************************************
* function : FAI_WINDOW_Destroy
* description : 윈도우를 제거한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Destroy(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_Draw
* description : 윈도우를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Draw(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_CreateSubWindow
* description : 서브 윈도우를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_CreateSubWindow(FAI_WINDOW_H *phSubWin, 
										FAI_WINDOW_H hParentwin, 
										char *pcName, 
										unsigned int unX, 
										unsigned int unY, 
										unsigned int unW, 
										unsigned int unH, 
										unsigned int unColor, 
										FAI_WINDOW_TYPE etype, 
										pfnFAI_EVENTHANDLER_T pfnHandler);

/******************************************************************************
* function : FAI_WINDOW_Show
* description : 서브 윈도우를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Show(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_Hide
* description : 서브 윈도우를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Hide(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_SetFocusWidget
* description : 윈도우의 디폴트 포커스를 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget);

/******************************************************************************
* function : FAI_WINDOW_GetFocusWidget
* description : 윈도우의 디폴트 포커스를 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H *phWidget);

/******************************************************************************
* function : FAI_WINDOW_Move
* description : 윈도우간 창 전환을 수행한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Move(FAI_WINDOW_H hWin1, FAI_WINDOW_H hWin2);

/******************************************************************************
* function : FAI_WINDOW_MoveWidget
* description : 현재 윈도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_MoveWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2);

/******************************************************************************
* function : FAI_WINDOW_SetCurrentWindow
* description : 현재 윈도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetCurrentWindow(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_GetCurrentWindow
* description : 현재 윈도우 핸들을 얻어 온다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetCurrentWindow(FAI_WINDOW_H *phWin);

/******************************************************************************
* function : FAI_WINDOW_SetPrevWindow
* description : 이전 윈도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetPrevWindow(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_GetPrevWindow
* description : 이전 윈도우 핸들을 얻어 온다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetPrevWindow(FAI_WINDOW_H *phWin);

#endif 
