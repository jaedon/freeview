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
/* global header files */
#include "string.h"

/* humax header files */
#include "fvk.h"

#include "fai_common.h"
#include "fai_window.h"
#include "fai_text.h"
#include "fai_font.h"

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
* function : FAI_TEXT_Create
* description : 텍스트를 생성한다. 
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
							TEXT_ALIGN_TYPE eAlignType)
{
	FAI_TEXT_H hText;
	
	if( hWin == NULL || pcStr == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Create] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	hText = (FAI_TEXT_H)FVK_MEM_Alloc(sizeof(FAI_TEXT_T));
	if( hText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Create] : FAI_ERR_MEM_ALLOC\n"));
		return FAI_ERR_MEM_ALLOC;
	}
	hText->pcText = FVK_MEM_Alloc(strlen(pcStr)+1);
	if( hText->pcText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Create] : FAI_ERR_MEM_ALLOC\n"));
		FVK_MEM_Free(hText);
		return FAI_ERR_MEM_ALLOC;
	}
	strcpy(hText->pcText, pcStr);		
	hText->tWidget.unX = hWin->tWidget.unX + unX;
	hText->tWidget.unY = hWin->tWidget.unY + unY;
	hText->tWidget.unW = unW;
	hText->tWidget.unH = unH;
	hText->tWidget.ptUp = NULL;
	hText->tWidget.ptDown = NULL;
	hText->tWidget.ptLeft = NULL;
	hText->tWidget.ptRight= NULL;		
	hText->tWidget.eType = FAI_WIDGET_TEXT;
	hText->tWidget.eFocus = FAI_WIDGET_NO_FOCUS;
	hText->tWidget.pvRealWidget = (FAI_TEXT_H)hText;

	hText->ptParentWindow = hWin;
	hText->unTextColor = unTextColor;
	hText->unBgColor = unBgColor;
	hText->eAlignType = eAlignType;

	LIST_InsertBack(&hWin->widgets, &hText->tWidget.tNode);

	*phText = hText;
	
	return FAI_NO_ERR;	
}

/******************************************************************************
* function : FAI_TEXT_Draw
* description : 텍스트를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Draw(FAI_TEXT_H hText)
{
	int nAlignX, nAlignY;

	if( hText == NULL ) 
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Draw] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	if( hText->unBgColor != RGB_COLOR_NONE )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hText->tWidget.unX , hText->tWidget.unY , hText->tWidget.unW, hText->tWidget.unH, hText->unBgColor));
	}
	
	if( hText->eAlignType == TEXT_ALIGN_NOT_USE )
	{
		FAI_ERR_CHECK(FAI_FONT_DrawString(hText->tWidget.unX, hText->tWidget.unY, hText->pcText, hText->unTextColor));
	}
	else if( hText->eAlignType == TEXT_ALIGN_MIDDLE )
	{
		nAlignX = hText->tWidget.unW/2 - FAI_FONT_GetStringWidth(hText->pcText)/2;
		nAlignY = hText->tWidget.unH/2 - TEXT_HEIGHT/2;  // 10 => half of character's height
		FAI_ERR_CHECK(FAI_FONT_DrawString(hText->tWidget.unX+nAlignX, hText->tWidget.unY+nAlignY, hText->pcText, hText->unTextColor));
	}
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TEXT_Redraw
* description : 텍스트를 다시 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Redraw(FAI_TEXT_H hText)
{
	int nAlignX, nAlignY;

	if( hText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Redraw] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	if( hText->unBgColor != RGB_COLOR_NONE )
	{
		FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hText->tWidget.unX , hText->tWidget.unY , hText->tWidget.unW, hText->tWidget.unH, hText->unBgColor));
	}
	
	if( hText->eAlignType == TEXT_ALIGN_NOT_USE )
	{
		FAI_ERR_CHECK(FAI_FONT_DrawString(hText->tWidget.unX, hText->tWidget.unY, hText->pcText, hText->unTextColor));
	}
	else if( hText->eAlignType == TEXT_ALIGN_MIDDLE )
	{
		nAlignX = hText->tWidget.unW/2 - FAI_FONT_GetStringWidth(hText->pcText)/2;
		nAlignY = hText->tWidget.unH/2 - TEXT_HEIGHT/2;  // 10 => half of character's height
		FAI_ERR_CHECK(FAI_FONT_DrawString(hText->tWidget.unX+nAlignX, hText->tWidget.unY+nAlignY, hText->pcText, hText->unTextColor));
	}
	
	FAI_ERR_CHECK(FDI_OSD_FlushScreen(hText->tWidget.unX, hText->tWidget.unY, hText->tWidget.unW, hText->tWidget.unH));

	return FAI_NO_ERR;	
}

/******************************************************************************
* function : FAI_TEXT_Clear
* description : 텍스트를 클리어한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Clear(FAI_TEXT_H hText)
{
	FAI_ERR_CHECK(FDI_OSD_DrawFillRect(hText->tWidget.unX , hText->tWidget.unY , hText->tWidget.unW, hText->tWidget.unH, RGB_COLOR_BLACK));

	return FAI_NO_ERR;	
}

/******************************************************************************
* function : FAI_TEXT_Destroy
* description : 텍스트를 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Destroy(FAI_TEXT_H hText)
{
	if( hText == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Destroy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	FVK_MEM_Free(hText->pcText);
	FVK_MEM_Free(hText);

	return FAI_NO_ERR;	
}

/******************************************************************************
* function : FAI_TEXT_Change
* description : 텍스트 내용값을 교체한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TEXT_Change(FAI_TEXT_H hText, 
							unsigned int unTextColor, 
							unsigned int unBgColor, 
							char *pcStr)
{	
	if( hText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Change] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	
	if( hText->pcText != NULL )
	{
		FVK_MEM_Free(hText->pcText);
	}
	
	hText->pcText = FVK_MEM_Alloc(strlen(pcStr)+1);
	if( hText->pcText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_Change] : FVK_MEM_Alloc fail!!\n"));
		return FAI_ERR_MEM_ALLOC;
	}
	
	strcpy(hText->pcText, pcStr);
	
	hText->unTextColor = unTextColor;
	hText->unBgColor = unBgColor;

	// 부모 윈도우와 현재 윈도우와 비교하여 결정
	FAI_ERR_CHECK(FAI_TEXT_Clear(hText));
	FAI_ERR_CHECK(FAI_TEXT_Redraw(hText));

	return FAI_NO_ERR;	
}

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
									char *pcStr)
{
	if( hText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_ChangeNoDraw] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}

	if( hText->pcText != NULL )
	{
		FVK_MEM_Free(hText->pcText);
	}
	
	hText->pcText = FVK_MEM_Alloc(strlen(pcStr)+1);
	if( hText->pcText == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TEXT_ChangeNoDraw] : FVK_MEM_Alloc fail!!\n"));
		return FAI_ERR_MEM_ALLOC;
	}
	strcpy(hText->pcText, pcStr);
	hText->unTextColor = unTextColor;
	hText->unBgColor = unBgColor;

	return FAI_NO_ERR;	
}

