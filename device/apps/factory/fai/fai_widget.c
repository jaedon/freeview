/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_common.h"
#include "fai_widget.h"
#include "fai_window.h"
#include "fai_text.h"
#include "fai_button.h"
#include "fai_testmenu.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/

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
FAI_ERR_CODE FAI_WIDGET_Link(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2, FAI_WIDGET_LINK_DIR eDir)
{
	if( hWidget1 == NULL || hWidget2 == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIDGET_Link] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	switch(eDir)
	{
		case FAI_WIDGET_LINK_UP:
			hWidget1->ptUp = hWidget2;
			hWidget2->ptDown = hWidget1;
			break;
		case FAI_WIDGET_LINK_DOWN:
			hWidget1->ptDown = hWidget2;
			hWidget2->ptUp = hWidget1;
			break;
		case FAI_WIDGET_LINK_LEFT:
			hWidget1->ptLeft = hWidget2;
			hWidget2->ptRight = hWidget1;
			break;
		case FAI_WIDGET_LINK_RIGHT:
			hWidget1->ptRight = hWidget2;
			hWidget2->ptLeft = hWidget1;
			break;
		default:
			break;
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WIDGET_SetFocus
* description : 주어진 위젯의 포커스 ON/OFF를 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_SetFocus(FAI_WIDGET_H hWidget1, FAI_WIDGET_STATUS eFocus)
{
	if( hWidget1 == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIDGET_SetFocus] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	switch(eFocus)
	{
		case FAI_WIDGET_FOCUS:
			hWidget1->eFocus = FAI_WIDGET_FOCUS;
			break;
		case FAI_WIDGET_NO_FOCUS:
			hWidget1->eFocus = FAI_WIDGET_NO_FOCUS;
			break;
		default:
			break;
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WIDGET_Move
* description : 위젯들간 포커스 전환을 한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Move(FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2)
{
	if( hWidget1 == NULL || hWidget2 == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIDGET_Move] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(hWidget2, FAI_WIDGET_FOCUS));
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(hWidget1, FAI_WIDGET_NO_FOCUS));
	FAI_ERR_CHECK(FAI_WIDGET_Draw(hWidget2, 1));
	FAI_ERR_CHECK(FAI_WIDGET_Draw(hWidget1, 1));
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WIDGET_Draw
* description : 위젯을 화면에 그린다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Draw(FAI_WIDGET_H hWidget, int bRedraw)
{
	if( hWidget == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIDGET_Draw] : FAI_ERR_INVALID_PARAMETER\n")); 
		return FAI_ERR_INVALID_PARAMETER;
	}
	switch(hWidget->eType)
	{
		case FAI_WIDGET_WINDOW:
			FAI_ERR_CHECK(FAI_WINDOW_Draw((FAI_WINDOW_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_TEXT:
			if( bRedraw == 0 ) 	FAI_ERR_CHECK(FAI_TEXT_Draw((FAI_TEXT_T *)hWidget->pvRealWidget))
			else 			FAI_ERR_CHECK(FAI_TEXT_Redraw((FAI_TEXT_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_BUTTON:
			if( bRedraw == 0 )	FAI_ERR_CHECK(FAI_BUTTON_Draw((FAI_BUTTON_T *)hWidget->pvRealWidget))
			else 			FAI_ERR_CHECK(FAI_BUTTON_Redraw((FAI_BUTTON_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_TESTMENU:
			FAI_ERR_CHECK(FAI_TESTMENU_Draw((FAI_TESTMENU_T *)hWidget->pvRealWidget));
			break;
		default:
			FAI_PRINT(FAI_PRT_WARNING, ("[FAI_WIDGET_Draw] : Unknown Widget\n"));
			return FAI_ERR;
			break;
	}

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WIDGET_Destroy
* description : 위젯을 삭제한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WIDGET_Destroy(FAI_WIDGET_H hWidget)
{
	if( hWidget == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIDGET_Destroy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	switch(hWidget->eType)
	{
		case FAI_WIDGET_WINDOW:
			FAI_ERR_CHECK(FAI_WINDOW_Destroy((FAI_WINDOW_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_TEXT:
			FAI_ERR_CHECK(FAI_TEXT_Destroy((FAI_TEXT_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_BUTTON:
			FAI_ERR_CHECK(FAI_BUTTON_Destroy((FAI_BUTTON_T *)hWidget->pvRealWidget));
			break;
		case FAI_WIDGET_TESTMENU:
			FAI_ERR_CHECK(FAI_TESTMENU_Destroy((FAI_TESTMENU_T *)hWidget->pvRealWidget));
		default:
			break;
	}
	return FAI_NO_ERR;
}

