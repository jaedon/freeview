/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_MSGBOX_H_
#define _FAI_MSGBOX_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_window.h"
#include "fai_text.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define FAI_MSG_STR_MAX					80
#define FAI_MSG_LINE_MAX 				20

typedef struct fai_msgbox {
	FAI_WINDOW_H ptMsgBoxWin;
	unsigned long ulLineCnt;
	FAI_TEXT_H	ptLine[FAI_MSG_LINE_MAX];
} FAI_MSGBOX_T;

typedef FAI_MSGBOX_T *FAI_MSGBOX_H;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */
extern FAI_MSGBOX_H g_ptMsgBox;

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : FAI_MSGBOX_Create
* description : OK메시지 박스를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_Create(FAI_MSGBOX_H *phMsgBox, 
								char *pcTitleMsg, 
								char ppcMsg[][FAI_MSG_STR_MAX],
								unsigned int unLineCnt, 
								pfnFAI_EVENTHANDLER_T pfnHandler);

/******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_ChangeMsg(FAI_MSGBOX_H hMsgBox, 
									unsigned char *pucMsg, 
									unsigned long ulLineNum, 
									unsigned long ulColor);

/******************************************************************************
* function : FAI_MSGBOX_Destoy
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_Destoy(FAI_MSGBOX_H hMsgBox);

/******************************************************************************
* function : FAI_MSGBOX_SetCurrentMsgBox
* description : 현재 위도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_SetCurrentMsgBox(FAI_MSGBOX_H hMsgBox);

/******************************************************************************
* function : FAI_MSGBOX_GetCurrentMsgBox
* description : 현재 위도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_GetCurrentMsgBox(FAI_MSGBOX_H *hMsgBox);

#endif
