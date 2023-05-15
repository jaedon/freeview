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

/* ���� ����ü */
typedef struct fai_widget {
	LIST_NODE_T tNode;
	char szName[30];                 // ������ �̸�
	unsigned int unX;                // ������ ��ġ 
	unsigned int unY;
	unsigned int unW;                         // ������ ũ��
	unsigned int unH;

	FAI_WIDGET_STATUS eFocus;       // ������ ���� ���õǾ� �ִ���
	FAI_WIDGT_TYPE eType;           // ������ Ÿ��

	struct fai_widget *ptUp;         // ������ �׹��� ��ũ     
	struct fai_widget *ptDown;       // ������ ����Ǿ� �ִ� ���
	struct fai_widget *ptLeft;       // ����� ������ �����͸� ���� �ְ�   
	struct fai_widget *ptRight;      // �׷��� ���� ��� NULL �����͸� ���� �ִ�.

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
* description : ������ ��ũ�� ��Ų��. �������� �̵��� ���� ����� �����鸸 
* 				�̵��� �����ϴ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Link(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2, FAI_WIDGET_LINK_DIR eDir);

/******************************************************************************
* function : FAI_WIDGET_SetFocus
* description : �־��� ������ ��Ŀ�� ON/OFF�� �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_SetFocus(FAI_WIDGET_H hWidget, FAI_WIDGET_STATUS eFocus);

/******************************************************************************
* function : FAI_WIDGET_Move
* description : �����鰣 ��Ŀ�� ��ȯ�� �Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Move(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2);

/******************************************************************************
* function : FAI_WIDGET_Draw
* description : ������ ȭ�鿡 �׸���. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Draw(FAI_WIDGET_H hWidget, int bRedraw);

/******************************************************************************
* function : FAI_WIDGET_Destroy
* description : ������ �����Ѵ�. 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Destroy(FAI_WIDGET_H hWidget);

#endif

