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
#include "fai_font.h"
#include "fai_window.h"
#include "fai_widget.h"
#include "fai_text.h"
#include "fai_msgbox.h"
#include "fai_app_config.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define MSG_BOX_WINDOW_X 			100
#define MSG_BOX_WINDOW_Y 			200
#define MSG_BOX_WINDOW_W 			400
#define MSG_BOX_WINDOW_H 			100
#define MSG_BOX_TITLE_HEIGHT 		20
#define MSG_BOX_ADD_UP_Y 			10
#define MSG_BOX_ADD_HEIGHT 			20

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
static FAI_MSGBOX_H s_ptCurrentMsgBox;
static unsigned char *s_pucMsgBuffer;
BOOL g_bIsMsgBoxPopUp;

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
								pfnFAI_EVENTHANDLER_T pfnHandler)
{
	int i;
	FAI_WINDOW_H hCurWin, hMsgBoxWin;
	FAI_MSGBOX_H hMsgBox;
	FAI_TEXT_H hMsgText, hTitleText;
	FDI_OSD_PIXEL_FORMAT_e ePixelFormat;
	unsigned long ulPixelSize;

	if( unLineCnt <= 0 || unLineCnt > FAI_MSG_LINE_MAX)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MSGBOX_Create] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	hMsgBox = FVK_MEM_Alloc(sizeof(FAI_MSGBOX_T));
	if( hMsgBox == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CreateInfoMsgBox] : FAI_ERR_MEM_ALLOC\n"));
		return FAI_ERR_MEM_ALLOC;
	} 
	FVK_MEM_Set(hMsgBox, 0x00, sizeof(FAI_MSGBOX_T));
	
	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hCurWin));
	FAI_ERR_CHECK(FAI_WINDOW_SetPrevWindow(hCurWin));

	FAI_ERR_CHECK(FAI_WINDOW_CreateSubWindow(&hMsgBox->ptMsgBoxWin, 
			 							hCurWin, pcTitleMsg, 
										MSG_BOX_WINDOW_X, 
										MSG_BOX_WINDOW_Y -(unLineCnt-1)*MSG_BOX_ADD_UP_Y, 
										MSG_BOX_WINDOW_W, 
										MSG_BOX_TITLE_HEIGHT+15 + (unLineCnt)*MSG_BOX_ADD_HEIGHT, 
										MSG_BOX_TITLE_COLOR, 
										FAI_WINDOW_MSGBOX, 
										pfnHandler));
	FAI_ERR_CHECK(FAI_TEXT_Create(&hTitleText, 
									hMsgBox->ptMsgBoxWin, 
									5, 
									5, 
									MSG_BOX_TITLE_HEIGHT, 
									MSG_BOX_WINDOW_W, 
									RGB_COLOR_WHITE, 
									RGB_COLOR_NONE, 
									pcTitleMsg, 
									TEXT_ALIGN_NOT_USE));

	FAI_ERR_CHECK(FAI_WINDOW_CreateSubWindow(&hMsgBoxWin, 
										hMsgBox->ptMsgBoxWin, 
										"titlebar", 
										5, 
										MSG_BOX_TITLE_HEIGHT+5, 
										MSG_BOX_WINDOW_W-10, 
										(MSG_BOX_WINDOW_H-50)+(unLineCnt-1)*MSG_BOX_ADD_HEIGHT-25, 
										RGB_COLOR_WHITE, 
										FAI_WINDOW_NORMAL, 
										NULL));
	hMsgBox->ulLineCnt = unLineCnt;
	for( i = 0; i < unLineCnt; i++ )
	{
		FAI_ERR_CHECK(FAI_TEXT_Create(&hMsgBox->ptLine[i], 
										hMsgBoxWin, 
										5, 
										5+i*MSG_BOX_ADD_HEIGHT, 
										MSG_BOX_WINDOW_W-20, 										
										MSG_BOX_ADD_HEIGHT, 
										RGB_COLOR_BLACK, 
										RGB_COLOR_NONE, 
										(ppcMsg!=NULL&&ppcMsg[i]!=NULL)?ppcMsg[i]:"",
										TEXT_ALIGN_NOT_USE));
	}
	FAI_ERR_CHECK(FAI_WINDOW_SetCurrentWindow(hMsgBox->ptMsgBoxWin));

	FDI_OSD_GetPixelInfo(&ePixelFormat, &ulPixelSize);
	s_pucMsgBuffer = FVK_MEM_Alloc(hMsgBox->ptMsgBoxWin->tWidget.unW*4*hMsgBox->ptMsgBoxWin->tWidget.unH);
	if( s_pucMsgBuffer == NULL )
	{
		FVK_MEM_Free(hMsgBox);
		return FAI_ERR;
	}
	FDI_OSD_GetOsdRect(hMsgBox->ptMsgBoxWin->tWidget.unX, 
						hMsgBox->ptMsgBoxWin->tWidget.unY, 
						hMsgBox->ptMsgBoxWin->tWidget.unW,
						hMsgBox->ptMsgBoxWin->tWidget.unH, 
						s_pucMsgBuffer);
	
	FAI_ERR_CHECK(FAI_WINDOW_Draw(hMsgBox->ptMsgBoxWin));

	FAI_ERR_CHECK(FAI_MSGBOX_SetCurrentMsgBox(hMsgBox));
	FAI_ERR_CHECK(FAI_WINDOW_SetCurrentWindow(hMsgBox->ptMsgBoxWin)); 
	*phMsgBox = hMsgBox;	
	g_bIsMsgBoxPopUp = TRUE;

	return FAI_NO_ERR;
}

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
									unsigned long ulColor)
{
	if( hMsgBox == NULL || pucMsg == NULL || ulLineNum <= 0 || ulLineNum > FAI_MSG_LINE_MAX)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MSGBOX_ChangeMsg] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	if( hMsgBox->ptLine[ulLineNum-1] != NULL )
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(hMsgBox->ptLine[ulLineNum-1], ulColor, RGB_COLOR_WHITE, pucMsg));
	}

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_MSGBOX_Destoy
* description : 메시지 박스를 제거한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_Destoy(FAI_MSGBOX_H hMsgBox)
{
	FAI_WINDOW_H prevWin;
	
	if( hMsgBox == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MSGBOX_Destoy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	FAI_ERR_CHECK(FAI_WINDOW_GetPrevWindow(&prevWin));
	FAI_ERR_CHECK(FAI_WINDOW_SetCurrentWindow(prevWin));		

	FDI_OSD_SetOsdRect(hMsgBox->ptMsgBoxWin->tWidget.unX, 
						hMsgBox->ptMsgBoxWin->tWidget.unY, 
						hMsgBox->ptMsgBoxWin->tWidget.unW,
						hMsgBox->ptMsgBoxWin->tWidget.unH, 
						s_pucMsgBuffer);
	FVK_MEM_Free(s_pucMsgBuffer);
	
	FAI_ERR_CHECK(FDI_OSD_FlushScreen(hMsgBox->ptMsgBoxWin->tWidget.unX, hMsgBox->ptMsgBoxWin->tWidget.unY, hMsgBox->ptMsgBoxWin->tWidget.unW, hMsgBox->ptMsgBoxWin->tWidget.unH));
	
	FAI_ERR_CHECK(FAI_WINDOW_Destroy(hMsgBox->ptMsgBoxWin));
	g_bIsMsgBoxPopUp = FALSE;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_MSGBOX_SetCurrentMsgBox
* description : 현재 위도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_SetCurrentMsgBox(FAI_MSGBOX_H hMsgBox)
{
	if( hMsgBox == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MSGBOX_SetCurrentMsgBox] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	s_ptCurrentMsgBox = hMsgBox;

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_MSGBOX_GetCurrentMsgBox
* description : 현재 위도우로 설정한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_MSGBOX_GetCurrentMsgBox(FAI_MSGBOX_H *hMsgBox)
{
	if( hMsgBox == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MSGBOX_GetCurrentMsgBox] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	*hMsgBox = s_ptCurrentMsgBox;

	return FAI_NO_ERR;
}

