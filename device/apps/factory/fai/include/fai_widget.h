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
#ifndef _FAI_WIDGET_H_
#define _FAI_WIDGET_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "list.h"
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum {
	FAI_WIDGET_WINDOW = 0, 
	FAI_WIDGET_MSGBOX, 
	FAI_WIDGET_BUTTON, 
	FAI_WIDGET_TEXT, 
	FAI_WIDGET_SELECTOR, 
	FAI_WIDGET_TEXTEDIT, 
	FAI_WIDGET_TESTMENU
} FAI_WIDGT_TYPE;

typedef enum {
	FAI_WIDGET_FOCUS = 0, 
	FAI_WIDGET_NO_FOCUS
} FAI_WIDGET_STATUS;

typedef enum {
	FAI_WIDGET_LINK_UP = 0, 
	FAI_WIDGET_LINK_DOWN, 
	FAI_WIDGET_LINK_LEFT, 
	FAI_WIDGET_LINK_RIGHT
} FAI_WIDGET_LINK_DIR;

/* 위젯 구조체 */
typedef struct fai_widget {
	LIST_NODE_T tNode;
	char szName[30];                 // 위젯의 이름
	unsigned int unX;                // 위젯의 위치 
	unsigned int unY;
	unsigned int unW;                         // 위젯의 크기
	unsigned int unH;

	FAI_WIDGET_STATUS eFocus;       // 위젯이 현재 션택되어 있는지
	FAI_WIDGT_TYPE eType;           // 위젯의 타입

	struct fai_widget *ptUp;         // 위젯의 네방향 링크     
	struct fai_widget *ptDown;       // 위젯에 연결되어 있는 경우
	struct fai_widget *ptLeft;       // 연결된 위젯의 포인터를 갖고 있고   
	struct fai_widget *ptRight;      // 그렇지 않은 경우 NULL 포인터를 갖고 있다.

	void *pvRealWidget;
} FAI_WIDGET_T;

typedef FAI_WIDGET_T *FAI_WIDGET_H;

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
* function : FAI_WIDGET_Link
* description : 위젯간 링크를 시킨다. 위젯간의 이동시 서로 연결된 위젯들만 
* 				이동이 가능하다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Link(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2, FAI_WIDGET_LINK_DIR eDir);

/******************************************************************************
* function : FAI_WIDGET_SetFocus
* description : 주어진 위젯의 포커스 ON/OFF를 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_SetFocus(FAI_WIDGET_H hWidget, FAI_WIDGET_STATUS eFocus);

/******************************************************************************
* function : FAI_WIDGET_Move
* description : 위젯들간 포커스 전환을 한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Move(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2);

/******************************************************************************
* function : FAI_WIDGET_Draw
* description : 위젯을 화면에 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Draw(FAI_WIDGET_H hWidget, int bRedraw);

/******************************************************************************
* function : FAI_WIDGET_Destroy
* description : 위젯을 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Destroy(FAI_WIDGET_H hWidget);

#endif

