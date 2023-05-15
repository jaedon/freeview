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
* description : �����츦 �����Ѵ�.
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
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
* description : �����츦 �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Destroy(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_Draw
* description : �����츦 �׸���. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Draw(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_CreateSubWindow
* description : ���� �����츦 �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
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
* description : ���� �����츦 �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Show(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_Hide
* description : ���� �����츦 �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Hide(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_SetFocusWidget
* description : �������� ����Ʈ ��Ŀ���� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget);

/******************************************************************************
* function : FAI_WINDOW_GetFocusWidget
* description : �������� ����Ʈ ��Ŀ���� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H *phWidget);

/******************************************************************************
* function : FAI_WINDOW_Move
* description : �����찣 â ��ȯ�� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Move(FAI_WINDOW_H hWin1, FAI_WINDOW_H hWin2);

/******************************************************************************
* function : FAI_WINDOW_MoveWidget
* description : ���� ������� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_MoveWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2);

/******************************************************************************
* function : FAI_WINDOW_SetCurrentWindow
* description : ���� ������� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetCurrentWindow(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_GetCurrentWindow
* description : ���� ������ �ڵ��� ��� �´�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetCurrentWindow(FAI_WINDOW_H *phWin);

/******************************************************************************
* function : FAI_WINDOW_SetPrevWindow
* description : ���� ������� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetPrevWindow(FAI_WINDOW_H hWin);

/******************************************************************************
* function : FAI_WINDOW_GetPrevWindow
* description : ���� ������ �ڵ��� ��� �´�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetPrevWindow(FAI_WINDOW_H *phWin);

#endif 
