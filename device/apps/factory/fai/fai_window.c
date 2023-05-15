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
#include <string.h>

#include "fvk.h"

#include "fai_common.h"
#include "fai_window.h"
#include "fai_widget.h"
#include "fai_text.h"
#include "fai_button.h"
#include "fai_testmenu.h"

#include "fdi_osd.h"

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
static FAI_WINDOW_H s_tpCurrentWindow, s_tpPrevWindow;

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
								pfnFAI_EVENTHANDLER_T pfnHandler)

{
	FAI_WINDOW_H hWin;
	
	hWin = (FAI_WINDOW_H)FVK_MEM_Alloc(sizeof(FAI_WINDOW_T));
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_Create] : FAI_ERR_MEM_ALLOC\n"));
		return FAI_ERR_MEM_ALLOC;
	}
	strcpy(hWin->tWidget.szName, pcName);
	hWin->tWidget.unX = unX;
	hWin->tWidget.unY = unY;
	hWin->tWidget.unW = unW;
	hWin->tWidget.unH = unH;
	hWin->tWidget.pvRealWidget = hWin;
	hWin->tWidget.eType = FAI_WIDGET_WINDOW;
	hWin->tWidget.eFocus = FAI_WIDGET_FOCUS;
		
	hWin->unWinColor = unColor;
	hWin->ptParentWindow = NULL;
	hWin->eType = eType;
	hWin->pfnEventHandler = pfnHandler;
	
	LIST_Init(&hWin->widgets);
	
	*phWin = hWin;
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_Destroy
* description : 윈도우를 제거한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Destroy(FAI_WINDOW_H hWin)
{
	LIST_NODE_T *ptNode;
	FAI_WIDGET_T *ptWidget;

	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_Destroy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	while( !LIST_IsEmpty(&hWin->widgets) )
	{
		ptNode = LIST_DeleteFront(&hWin->widgets);
	    ptWidget = LIST_Entry(ptNode, FAI_WIDGET_T, tNode);
		FAI_WIDGET_Destroy(ptWidget);
	}
	LIST_Delete(&hWin->tWidget.tNode);
	FVK_MEM_Free(hWin);

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_Draw
* description : 윈도우를 그린다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Draw(FAI_WINDOW_H hWin)
{
	LIST_NODE_T *ptNode;
	FAI_WIDGET_H ptWidget;

	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_DrawWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	/* If window has no focus, skip drawing */
	if( hWin->tWidget.eFocus == FAI_WIDGET_NO_FOCUS )
	{
		FAI_PRINT(FAI_PRT_WARNING, ("[FAI_DrawWindow] : FAI_WIDGET_NO_FOCUS\n"));
		return FAI_NO_ERR;
	}

	/* 1. Clear screen */
	//FAI_ERR_CHECK(FAI_OSD_ClearScreen(hWin->tWidget.unX , hWin->tWidget.unY, hWin->tWidget.unW, hWin->tWidget.unH));
	FDI_OSD_DrawFillRect(hWin->tWidget.unX, hWin->tWidget.unY, hWin->tWidget.unW, hWin->tWidget.unH, RGB_COLOR_BLACK);
	FDI_OSD_FlushScreen(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT);

	/* 2. Draw main window background */
	FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hWin->tWidget.unX , hWin->tWidget.unY, hWin->tWidget.unW, hWin->tWidget.unH, hWin->unWinColor));

	/* 3. Draw widgets that belong to window */
	for( ptNode = LIST_Begin(&hWin->widgets); ptNode != LIST_End(&hWin->widgets); ptNode = LIST_Next(ptNode) )
	{
		ptWidget = LIST_Entry(ptNode, FAI_WIDGET_T, tNode);
		FAI_ERR_CHECK(FAI_WIDGET_Draw(ptWidget, 0));
	}

	/* 4. flush screen */
	FAI_ERR_CHECK(FDI_OSD_FlushScreen(hWin->tWidget.unX , hWin->tWidget.unY , hWin->tWidget.unW, hWin->tWidget.unH));
	
	return FAI_NO_ERR;
}

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
										pfnFAI_EVENTHANDLER_T pfnHandler)
{
	FAI_ERR_CODE err = FAI_NO_ERR;

	if( phSubWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOWD_CreateSubWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	FAI_ERR_CHECK(FAI_WINDOW_Create(phSubWin, pcName, hParentwin->tWidget.unX+unX, hParentwin->tWidget.unY+unY, unW, unH, unColor, etype, pfnHandler));

	(*phSubWin)->ptParentWindow = hParentwin;

	LIST_InsertBack(&hParentwin->widgets, &(*phSubWin)->tWidget.tNode);
	
	return err;
}


/******************************************************************************
* function : FAI_WINDOW_Show
* description : 서브 윈도우를 생성한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Show(FAI_WINDOW_H hWin)
{
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_Show] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	hWin->tWidget.eFocus = FAI_WIDGET_FOCUS;
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_Hide
* description : 서브 윈도우를 생성한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Hide(FAI_WINDOW_H hWin)
{
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HideWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	
	hWin->tWidget.eFocus = FAI_WIDGET_NO_FOCUS;
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_SetFocusWidget
* description : 윈도우의 디폴트 포커스를 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget)
{
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_SetFocusWidget] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	hWin->ptFocusWidget = hWidget;
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(hWidget, FAI_WIDGET_FOCUS));
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_GetFocusWidget
* description : 윈도우의 디폴트 포커스를 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetFocusWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H *phWidget)
{
	if( hWin == NULL || phWidget == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_GetFocusWidget] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	*phWidget = hWin->ptFocusWidget;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_Move
* description : 윈도우간 창 전환을 수행한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_Move(FAI_WINDOW_H hWin1, FAI_WINDOW_H hWin2)
{
	if( hWin1 == NULL || hWin2 == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDDOW_Move] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	FDI_OSD_DrawFillRect(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT, RGB_COLOR_BLACK);
	FDI_OSD_FlushScreen(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT);
	
	FAI_ERR_CHECK(FAI_WINDOW_SetCurrentWindow(hWin2));
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(hWin1->ptFocusWidget, FAI_WIDGET_NO_FOCUS));
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(hWin2->ptFocusWidget, FAI_WIDGET_FOCUS));
	FAI_ERR_CHECK(FAI_WINDOW_Draw(hWin2));

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_MoveWidget
* description : 현재 위도우로 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_MoveWidget(FAI_WINDOW_H hWin, FAI_WIDGET_H hWidget1, FAI_WIDGET_H hWidget2)
{
	if( hWin == NULL || hWidget1 == NULL || hWidget2 == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_MoveWidget] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	FAI_ERR_CHECK(FAI_WIDGET_Move(hWidget1, hWidget2));
	FAI_ERR_CHECK(FAI_WINDOW_SetFocusWidget(hWin, hWidget2));

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_SetCurrentWindow
* description : 현재 위도우로 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetCurrentWindow(FAI_WINDOW_H hWin)
{
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_SetCurrentWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	s_tpCurrentWindow = hWin;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_GetCurrentWindow
* description : 현재 위도우로 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetCurrentWindow(FAI_WINDOW_H *phWin)
{
	if( phWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_GetCurrentWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	*phWin = s_tpCurrentWindow;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_SetPrevWindow
* description : 현재 위도우로 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_SetPrevWindow(FAI_WINDOW_H hWin)
{
	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_SetPrevWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	s_tpPrevWindow = hWin;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_WINDOW_GetPrevWindow
* description : 현재 위도우로 설정한다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_WINDOW_GetPrevWindow(FAI_WINDOW_H *phWin)
{
	if( phWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WINDOW_GetPrevWindow] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	*phWin = s_tpPrevWindow;

	return FAI_NO_ERR;
}

