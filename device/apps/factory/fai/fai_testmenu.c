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
#include "fai_testmenu.h"
#include "fai_app.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define TEST_MENU_TEXT_COLOR 			RGB_COLOR_WHITE
#define TEST_MENU_RCU_COLOR 			RGB_COLOR_BLUE
#define TEST_MENU_DEFAULT_COLOR 		RGB_COLOR_BLUE

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */
FAI_TESTMENU_H g_hCurrentTestMenu;

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */
extern BOOL g_bIsMsgBoxPopUp;

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : FAI_TESTMENU_Create
* description : 테스트 메뉴를 생성한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Create(FAI_TESTMENU_H *phMenu, 
								FAI_WINDOW_H hWin, 
								char *pcTestName, 
								char *pcInputRcuText, 
								unsigned int unX, 
								unsigned int unY, 
								unsigned int unW, 
								unsigned int unH, 
								TEST_MENU_TYPE eType)

{
	FAI_ERR_CODE err = FAI_NO_ERR;
	FAI_TESTMENU_H hMenu;
	char *str;
	int i, j, k;
	char pcTempStr[20];

	if( hWin == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_Create] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	hMenu = (FAI_TESTMENU_H)FVK_MEM_Alloc(sizeof(FAI_TESTMENU_T));
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_Create] : FAI_ERR_MEM_ALLOC\n"));
		return FAI_ERR_MEM_ALLOC;
	}

	hMenu->tWidget.eType = FAI_WIDGET_TESTMENU;
	hMenu->tWidget.pvRealWidget = (FAI_TESTMENU_H)hMenu;
	hMenu->tWidget.ptUp = NULL;
	hMenu->tWidget.ptDown = NULL;
	hMenu->tWidget.ptLeft = NULL;
	hMenu->tWidget.ptRight= NULL;
	hMenu->ptParentWindow = hWin;
	hMenu->eType = eType;
	hMenu->unStep = 0;
	
	strcpy(hMenu->acTestName[0], "");
	strcpy(hMenu->acTestName[1], "");
	strcpy(hMenu->acTestName[2], "");
	strcpy(hMenu->acInputRcuName, "");

#if 0
	str = strtok(pcTestName, "\n");
	while( str != NULL )
	{
		strcpy(hMenu->acTestName[i++], str);
		str = strtok(NULL, "\n");		
	}
#else /* if strtok is err */
	i = j = k = 0;
	if( pcTestName != NULL )
	{
		while( pcTestName[i] != '\0' )
		{
			if( pcTestName[i] == '\n' )
			{
				pcTempStr[j] = '\0';
				strcpy(hMenu->acTestName[k++], pcTempStr);
				strcpy(pcTempStr, "");				
				j = 0;
			}
			else
			{
				pcTempStr[j++] = pcTestName[i];
			}
			i++;
		}
		pcTempStr[j] = '\0';
		strcpy(hMenu->acTestName[k], pcTempStr);
	}
#endif
	if( pcInputRcuText != NULL )
	{
		strcpy(hMenu->acInputRcuName, pcInputRcuText);
	}
#if 0 /* enable, if you want to print test menu position */
	FAI_PRINT(PRT_DBGINFO, ("TestMenu Position : %d %d %d %d\n", hWin->tWidget.unX+unX, 
												hWin->tWidget.unY+unY, 
												unW, 
												unH))
#endif
	if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
	{
		if( pcTestName == NULL || pcInputRcuText == NULL )
		{
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine1, hWin, unX, unY, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine2, hWin, unX, unY+(unH-24)/4*1, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine3, hWin, unX, unY+(unH-24)/4*2, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));	
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine4, hWin, unX, unY+(unH-24)/4*3, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptInputRCUText, hWin, unX, unY+(unH-24)/4*4+4, unW, 20, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));			 
		}
		else  
		{
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine1, hWin, unX, unY, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, TEST_MENU_DEFAULT_COLOR, "", TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine2, hWin, unX, unY+(unH-24)/4*1, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, TEST_MENU_DEFAULT_COLOR, hMenu->acTestName[0], TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine3, hWin, unX, unY+(unH-24)/4*2, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, TEST_MENU_DEFAULT_COLOR, hMenu->acTestName[1], TEXT_ALIGN_MIDDLE));	
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptTestMenuLine4, hWin, unX, unY+(unH-24)/4*3, unW, (unH-24)/4, TEST_MENU_TEXT_COLOR, TEST_MENU_DEFAULT_COLOR, hMenu->acTestName[2], TEXT_ALIGN_MIDDLE));
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptInputRCUText, hWin, unX, unY+(unH-24)/4*4+4, unW, 20, TEST_MENU_TEXT_COLOR, TEST_MENU_RCU_COLOR, pcInputRcuText, TEXT_ALIGN_MIDDLE));
		}
		hMenu->eType = TEST_MENU_TYPE_NORMAL;
	}
	else if( hMenu->eType == TEST_MENU_TYPE_KEY )
	{
		if( pcTestName == NULL )
		{
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptInputRCUText, hWin, unX, unY, unW, unH, TEST_MENU_TEXT_COLOR, RGB_COLOR_BLACK, "", TEXT_ALIGN_MIDDLE));
		}
		else
		{
			FAI_ERR_CHECK(FAI_TEXT_Create(&hMenu->ptInputRCUText, hWin, unX, unY, unW, unH, TEST_MENU_TEXT_COLOR, TEST_MENU_DEFAULT_COLOR, pcTestName, TEXT_ALIGN_MIDDLE));
		}
		hMenu->eType = TEST_MENU_TYPE_KEY;
	}

	LIST_InsertBack(&hWin->widgets, &hMenu->tWidget.tNode);
	*phMenu = hMenu;

	return err;
}

/******************************************************************************
* function : FAI_TESTMENU_Draw
* description : 테스트 메뉴를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Draw(FAI_TESTMENU_H hMenu)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_Draw] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}
	
	if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
	{
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptTestMenuLine1));
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptTestMenuLine2));
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptTestMenuLine3));
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptTestMenuLine4));
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptInputRCUText));
	}
	else if( hMenu->eType == TEST_MENU_TYPE_KEY )
	{
		FAI_ERR_CHECK(FAI_TEXT_Draw(hMenu->ptInputRCUText));
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_Draw
* description : 테스트 메뉴를 그린다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenuText(FAI_TESTMENU_H hMenu, TESTMENU_LINE eLine, char *pcText, unsigned int unTextColor, unsigned int unBgColor, BOOL bReDraw)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_ChageState] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	if( bReDraw == TRUE )
	{
		if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
		{
			switch(eLine)
			{
				case TESTMENU_LINE_1:
					FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine1, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_2:
					FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine2, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_3:
					FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine3, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_4:
					FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine4, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_RCU:
					FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, unTextColor, unBgColor, hMenu->ptInputRCUText->pcText));
					break;
				default:
					break;
			}
		}
		else
		{
			FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, unTextColor, unBgColor, pcText));
		}
	}
	else
	{
		if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
		{
			switch(eLine)
			{
				case TESTMENU_LINE_1:
					FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine1, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_2:
					FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine2, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_3:
					FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine3, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_4:
					FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine4, unTextColor, unBgColor, pcText));
					break;
				case TESTMENU_LINE_RCU:
					FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, unTextColor, unBgColor, hMenu->ptInputRCUText->pcText));
					break;
				default:
					break;
			}
		}
		else
		{
			FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, unTextColor, unBgColor, pcText));
		}

	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE eLine, char *pcText, unsigned int unBgColor)
{
	FAI_TESTMENU_H hMenu;
	
	FAI_ERR_CHECK(FAI_TESTMENU_GetCurrentMenu(&hMenu));
	if( unBgColor == RGB_COLOR_YELLOW )
	{
		FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_BLACK, unBgColor, TRUE));
	}
	else
	{
		FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_WHITE, unBgColor, TRUE));
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentMenuColor(unsigned int unBgColor)
{
	FAI_TESTMENU_H hMenu;

	FAI_ERR_CHECK(FAI_TESTMENU_GetCurrentMenu(&hMenu));	
	FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine1, TEST_MENU_TEXT_COLOR, unBgColor, ""));
	FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine2, TEST_MENU_TEXT_COLOR, unBgColor, ""));
	FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine3, TEST_MENU_TEXT_COLOR, unBgColor, ""));
	FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine4, TEST_MENU_TEXT_COLOR, unBgColor, ""));

	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeCurrentRCUColor(unsigned int unBgColor)
{
	FAI_TESTMENU_H hMenu;
	unsigned char aucRcuStr[10];
	
	FAI_ERR_CHECK(FAI_TESTMENU_GetCurrentMenu(&hMenu));	
	strcpy(aucRcuStr, hMenu->ptInputRCUText->pcText);
	if( unBgColor == RGB_COLOR_YELLOW )
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, RGB_COLOR_BLACK, unBgColor, aucRcuStr));
	}
	else
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, RGB_COLOR_WHITE, unBgColor, aucRcuStr));		
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenu(FAI_TESTMENU_H hMenu, TESTMENU_LINE eLine, char *pcText, unsigned int unBgColor)
{
	FAI_PP_SHEET eCurrentSheet;
	FAI_WINDOW_H hCurrentSheetWin;
	
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_ChangeMenu] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	while( g_bIsMsgBoxPopUp )
	{
		FVK_TASK_Sleep(1000);
	}

	FAI_APP_GetCurrentSheet(&eCurrentSheet);
	FAI_APP_GetSheetWindowHandle(eCurrentSheet, &hCurrentSheetWin);

	if( hCurrentSheetWin == hMenu->ptParentWindow )
	{
		if( unBgColor == RGB_COLOR_YELLOW )
		{
			FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_BLACK, unBgColor, TRUE));
		}
		else
		{
			FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_WHITE, unBgColor, TRUE));
		}
	}
	else
	{
		if( unBgColor == RGB_COLOR_YELLOW )
		{
			FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_BLACK, unBgColor, FALSE));
		}
		else
		{
			FAI_ERR_CHECK(FAI_TESTMENU_ChangeMenuText(hMenu, eLine, pcText, RGB_COLOR_WHITE, unBgColor, FALSE));
		}
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeMenuColor(FAI_TESTMENU_H hMenu, unsigned int unBgColor)
{
	FAI_PP_SHEET eCurrentSheet;
	FAI_WINDOW_H hCurrentSheetWin;
	
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_ChangeMenuColor] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	while( g_bIsMsgBoxPopUp )
	{
		FVK_TASK_Sleep(1000);
	}

	FAI_APP_GetCurrentSheet(&eCurrentSheet);
	FAI_APP_GetSheetWindowHandle(eCurrentSheet, &hCurrentSheetWin);

	if( hCurrentSheetWin == hMenu->ptParentWindow )
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine1, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine2, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine3, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptTestMenuLine4, TEST_MENU_TEXT_COLOR, unBgColor, ""));
	}
	else
	{
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine1, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine2, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine3, TEST_MENU_TEXT_COLOR, unBgColor, ""));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine4, TEST_MENU_TEXT_COLOR, unBgColor, ""));
	}
	return FAI_NO_ERR; 
}

/******************************************************************************
* function : FAI_TESTMENU_ChangeCurrentMenuText
* description : 테스트 메뉴의 상태를 변경한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_ChangeRCUColor(FAI_TESTMENU_H hMenu, unsigned int unBgColor)
{
	FAI_PP_SHEET eCurrentSheet;
	FAI_WINDOW_H hCurrentSheetWin;
	
	unsigned char aucRcuStr[10];
	
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_ChangeRCUColor] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;
	}

	while( g_bIsMsgBoxPopUp )
	{
		FVK_TASK_Sleep(1000);
	}

	FAI_APP_GetCurrentSheet(&eCurrentSheet);
	FAI_APP_GetSheetWindowHandle(eCurrentSheet, &hCurrentSheetWin);
	
	strcpy(aucRcuStr, hMenu->ptInputRCUText->pcText);

	if( hCurrentSheetWin == hMenu->ptParentWindow )
	{
		if( unBgColor == RGB_COLOR_YELLOW )
		{
			FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, RGB_COLOR_BLACK, unBgColor, aucRcuStr));
		}
		else
		{
			FAI_ERR_CHECK(FAI_TEXT_Change(hMenu->ptInputRCUText, RGB_COLOR_WHITE, unBgColor, aucRcuStr));		
		}
	}
	else
	{
		if( unBgColor == RGB_COLOR_YELLOW )
		{
			FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, RGB_COLOR_BLACK, unBgColor, aucRcuStr));
		}
		else
		{
			FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, RGB_COLOR_WHITE, unBgColor, aucRcuStr));		
		}
	}
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_Destroy
* description : 테스트 메뉴를 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Destroy(FAI_TESTMENU_H hMenu)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_Destroy] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}

	if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
	{
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptTestMenuLine1));
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptTestMenuLine2));
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptTestMenuLine3));
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptTestMenuLine4));		
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptInputRCUText));		
	}
	else if( hMenu->eType == TEST_MENU_TYPE_KEY )
	{
		FAI_ERR_CHECK(FAI_TEXT_Destroy(hMenu->ptInputRCUText));		
	}
	FVK_MEM_Free(hMenu);
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenu
* description : 테스트 메뉴를 삭제한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetCurrentMenu(FAI_TESTMENU_H hMenu)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_SetCurrentMenu] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	g_hCurrentTestMenu = hMenu;
	return FAI_NO_ERR;	
}

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenu
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_GetCurrentMenu(FAI_TESTMENU_H *phMenu)
{
	*phMenu = g_hCurrentTestMenu;
	return FAI_NO_ERR;		
}

/******************************************************************************
* function : FAI_TESTMENU_Reset
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_Reset(FAI_TESTMENU_H hMenu)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_Reset] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}

	hMenu->unStep = 0;
	if( hMenu->eType == TEST_MENU_TYPE_NORMAL )
	{
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine1, RGB_COLOR_WHITE, RGB_COLOR_BLUE, ""));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine2, RGB_COLOR_WHITE, RGB_COLOR_BLUE, hMenu->acTestName[0]));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine3, RGB_COLOR_WHITE, RGB_COLOR_BLUE, hMenu->acTestName[1]));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptTestMenuLine4, RGB_COLOR_WHITE, RGB_COLOR_BLUE, hMenu->acTestName[2]));
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, RGB_COLOR_WHITE, RGB_COLOR_BLUE, hMenu->acInputRcuName));
	} else if( hMenu->eType == TEST_MENU_TYPE_KEY )
	{
		FAI_ERR_CHECK(FAI_TEXT_ChangeNoDraw(hMenu->ptInputRCUText, RGB_COLOR_WHITE, RGB_COLOR_BLUE, hMenu->acInputRcuName));
	}
	
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_SetStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetStep(FAI_TESTMENU_H hMenu, unsigned int unStep) // remove
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_SetStep] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}

	hMenu->unStep = unStep;
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_GetStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned int FAI_TESTMENU_GetStep(FAI_TESTMENU_H hMenu)
{
	if( hMenu == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TESTMENU_GetStep] : FAI_ERR_INVALID_PARAMETER\n"));
		return FAI_ERR_INVALID_PARAMETER;		
	}
	return hMenu->unStep;
}

/******************************************************************************
* function : FAI_TESTMENU_SetCurrentMenuStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_TESTMENU_SetCurrentMenuStep(unsigned int unStep)
{
	FAI_TESTMENU_H hMenu;

	FAI_ERR_CHECK(FAI_TESTMENU_GetCurrentMenu(&hMenu));	
	hMenu->unStep = unStep;
	return FAI_NO_ERR;
}

/******************************************************************************
* function : FAI_TESTMENU_GetCurrentMenuStep
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned int FAI_TESTMENU_GetCurrentMenuStep()
{
	FAI_TESTMENU_H hMenu;

	FAI_ERR_CHECK(FAI_TESTMENU_GetCurrentMenu(&hMenu));	
	
	return hMenu->unStep;
}

