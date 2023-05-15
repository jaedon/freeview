/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/******************************************************************************
* header and definitions
******************************************************************************/
/* global header files */
#include "string.h"

/* humax header files */
#include "fvk.h"

#include "fai_common.h"
#include "fai_window.h"
#include "fai_button.h"
#include "fai_font.h"

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


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : FAI_BUTTON_Create
* description : 버튼을 생성한다. 
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
								TEXT_ALIGN_TYPE eAlignType)
{
	FAI_BUTTON_H hBt;
	
	if( hWin == NULL || pcStr == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Create] : FAI_ERR_INVALID_PARAMETER\n"));	
		return FAI_ERR_INVALID_PARAMETER;
	}
	hBt = (FAI_BUTTON_H)FVK_MEM_Alloc(sizeof(FAI_BUTTON_T));
	if( hBt == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Create] : FVK_MEM_Alloc Error\n"));
		return FAI_ERR_MEM_ALLOC;
	}
	hBt->pcText = (char *)FVK_MEM_Alloc(strlen(pcStr)+1);
	if( hBt->pcText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Create] : FVK_MEM_Alloc Error\n"));	
		FVK_MEM_Free(hBt);
		return FAI_ERR_MEM_ALLOC;
	}
	strcpy(hBt->pcText, pcStr);	
	strcpy(hBt->tWidget.szName, pcStr);	
	hBt->tWidget.unX = hWin->tWidget.unX + unX;
	hBt->tWidget.unY = hWin->tWidget.unY + unY;
	hBt->tWidget.unW = unW;
	hBt->tWidget.unH = unH;
	hBt->tWidget.ptUp = NULL;
	hBt->tWidget.ptDown = NULL;
	hBt->tWidget.ptLeft = NULL;
	hBt->tWidget.ptRight= NULL;
	hBt->tWidget.eFocus = FAI_WIDGET_NO_FOCUS;
	hBt->tWidget.eType = FAI_WIDGET_BUTTON;
	hBt->tWidget.pvRealWidget = (FAI_BUTTON_H)hBt;
	
	hBt->ptParentWindow = hWin;
	hBt->unFocusColor = unFocusColor;
	hBt->unNoFocusColor = unNoFocusColor;
	hBt->eAlignType = eAlignType;

	LIST_InsertBack(&hWin->widgets, &hBt->tWidget.tNode);

	*phBt = hBt;
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_BUTTON_Draw
* description : 버튼을 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Draw(FAI_BUTTON_H hBt)
{
	int nAlignX, nAlignY;

	if( hBt == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Draw] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	if( hBt->tWidget.eFocus == FAI_WIDGET_FOCUS )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH, hBt->unFocusColor));
	}
	else if( hBt->tWidget.eFocus == FAI_WIDGET_NO_FOCUS )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH, hBt->unNoFocusColor));
	}

	if( hBt->eAlignType == TEXT_ALIGN_NOT_USE ) 
	{
		FAI_ERR_CHECK(FAI_FONT_DrawString(hBt->tWidget.unX, hBt->tWidget.unY, hBt->pcText, RGB_COLOR_WHITE));
	}
	else if( hBt->eAlignType == TEXT_ALIGN_MIDDLE )
	{
		nAlignX = hBt->tWidget.unW/2 - FAI_FONT_GetStringWidth(hBt->pcText)/2;
		nAlignY = hBt->tWidget.unH/2 - TEXT_HEIGHT/2;
		FAI_ERR_CHECK(FAI_FONT_DrawString(hBt->tWidget.unX+nAlignX, hBt->tWidget.unY+nAlignY, hBt->pcText, RGB_COLOR_WHITE));
	}
	
	return FAI_NO_ERR;
}


/******************************************************************************
* function : FAI_BUTTON_Redraw
* description : 버튼을 다시 그린다. 버튼간의 이동시 윈도우 전체를 다시 그릴 필요 없이
* 				버튼만 다시 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Redraw(FAI_BUTTON_H hBt)
{
	int nAlignX, nAlignY;

	if( hBt == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Redraw] : FAI_ERR_INVALID_PARAMETER\n"));			
		return FAI_ERR_INVALID_PARAMETER;
	}
	//FAI_ERR_CHECK(FAI_OSD_ClearScreen(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH));
	FDI_OSD_DrawFillRect(hBt->tWidget.unX, hBt->tWidget.unY, hBt->tWidget.unW, hBt->tWidget.unH, RGB_COLOR_BLACK);
	FDI_OSD_FlushScreen(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT);
	
	if( hBt->tWidget.eFocus == FAI_WIDGET_FOCUS )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH, hBt->unFocusColor));
	}
	else if( hBt->tWidget.eFocus == FAI_WIDGET_NO_FOCUS )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH, hBt->unNoFocusColor));
	}

	if( hBt->eAlignType == TEXT_ALIGN_NOT_USE )
	{
		FAI_ERR_CHECK(FAI_FONT_DrawString(hBt->tWidget.unX, hBt->tWidget.unY, hBt->pcText, RGB_COLOR_WHITE));
	}
	else if( hBt->eAlignType == TEXT_ALIGN_MIDDLE )
	{
		nAlignX = hBt->tWidget.unW/2 - FAI_FONT_GetStringWidth(hBt->pcText)/2;
		nAlignY = hBt->tWidget.unH/2 - TEXT_HEIGHT/2;
		FAI_ERR_CHECK(FAI_FONT_DrawString(hBt->tWidget.unX+nAlignX, hBt->tWidget.unY+nAlignY, hBt->pcText, RGB_COLOR_WHITE));
	}
	FAI_ERR_CHECK(FDI_OSD_FlushScreen(hBt->tWidget.unX , hBt->tWidget.unY , hBt->tWidget.unW, hBt->tWidget.unH));

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_BUTTON_Destroy
* description : 버튼을 제거한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_Destroy(FAI_BUTTON_H hBt)
{
	if( hBt == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_Destroy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}		
	FVK_MEM_Free(hBt->pcText);
	FVK_MEM_Free(hBt);

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_BUTTON_SetFocus
* description : 버튼의 포커스를 맞춘다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_BUTTON_SetFocus(FAI_BUTTON_H hBt, FAI_WIDGET_STATUS eFocus)
{
	if( hBt == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BUTTON_SetFocus] : FAI_ERR_INVALID_PARAMETER\n"));		
		return FAI_ERR_INVALID_PARAMETER;
	}		
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(&hBt->tWidget, eFocus));
	
	return FAI_NO_ERR;
}


