/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application Initializer
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <string.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_font.h"
#include "fai_window.h"
#include "fai_text.h"
#include "fai_button.h"
#include "fai_msgbox.h"
#include "fai_testmenu.h"
#include "fai_app.h"
#include "fai_app_config.h"
#include "fai_testparams.h"
#include "fdi_key.h"
#include "fdi_led.h"
#include "fdi_panel.h"
#include "fdi_osd.h"
#include "fdi_system.h"
#if defined(FACTORY_USE_ETHERNET)
#include "fdi_ethernet.h"
#endif

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if defined(GUI_CAPTURE_UI_ENABLE)
#define BI_RGB       0

typedef struct BITMAP_FILE_HEADER
{
  unsigned short usType;
  unsigned long usFILESize;
  unsigned short usReserved1;
  unsigned short usReserved2;
  unsigned long ulOffset;
} __attribute__ ((__packed__)) BITMAP_FILE_HEADER_t;

typedef struct BITMAP_INFO_HEADER
{
	unsigned long ulInfoHeaderSize;
	unsigned long ulWidth;
	unsigned long ulHeight;
  	unsigned short ulPlanes;
	unsigned short ulBitCount;
	unsigned long ulCompression;
	unsigned long ulSizeImage;
	unsigned long ulXPelsPerMeter;
	unsigned long ulYPelsPerMeter;
	unsigned long ulClrUsed;
	unsigned long ulClrImportant;
} BITMAP_INFO_HEADER_t;
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Static variables and structures
*******************************************************************************/
/* static variables and structures */
static unsigned long s_ulKeyMsgID;
static unsigned long s_ulExeMenuNum;
static FAI_APPINFO_T g_tAppInfo;
static BOOL g_bExecTestFunc;

FAI_TEXT_H g_hEthStateText;
BOOL g_bEthLinked;
unsigned long g_ulEthTaskId;
unsigned long g_ulCurrentRcuKey;
#if(WIFI_CONDUCT_ENABLE==YES)
static unsigned long s_ulLedMsgID;
#endif

/*******************************************************************************
* Static function prototypes
*******************************************************************************/
/* static function prototypes */
static void INTRN_FAI_APP_RCUEventTask(void *data);
static void INTRN_FAI_APP_KeyCallback(FDI_KEY_DEVICE eDev, unsigned int nKeyCode, FDI_KEY_TYPE eKeyType);

#if defined(FACTORY_USE_ETHERNET)
void INTRN_FAI_APP_CheckEthStateTask(void *pData);
#endif
#if (CLEAR_MENU_UI_ENABLE==YES)
FAI_ERR_CODE FAI_APP_ClearTestMenus(void);
#endif
#if (GUI_CAPTURE_UI_ENABLE==YES)
FAI_ERR_CODE FAI_APP_GUICapture(void);
#endif
static FAI_ERR_CODE INTRN_FAI_APP_InitFactoryWindow();
static FAI_ERR_CODE INTRN_FAI_APP_FactoryWindowEventHandler(unsigned int unDeviceType, unsigned int unKeyCode);
#if(WIFI_CONDUCT_ENABLE==YES)
void INTRN_FAI_APP_BlinkLEDTask(void *data)
{
	int i, j, retVal;
	FVK_ERR_CODE eVkRet;
	FAI_KEY_MSG keyMsg;
	FAI_WINDOW_H hCurWin;
	FDI_ERR_CODE eFdiErr;
	unsigned int *ulState=0;
	
	/* Process Loop LED Blink Task */
	while(1)
	{
		FVK_TASK_Sleep(300);
		INTRN_FAI_PANEL_3Color2StatusLEDTest(0); //red
		FVK_TASK_Sleep(300);
		INTRN_FAI_PANEL_3Color2StatusLEDTest(1);//green
	}
}
#endif
/*******************************************************************************
* function : FAI_APP_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_Init()
{
	FVK_ERR_CODE eFvkErr;
	unsigned long ulRcuTaskID;
	unsigned long ulLEDTaskID;
	
#if(LAUNCHER_OTA_MODE_DISABLE==YES)
	FDI_ERR_CODE eFdi_Err=FDI_NO_ERR;

	FDI_SYSTEM_DOWNLOAD_TYPE_e eOtaDnType = FDI_SYSTEM_DOWNLOAD_NONE;
	eFdi_Err = FDI_SYSTEM_SetOtaDnType(&eOtaDnType);
	if(eFdi_Err!=FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FDI_SYSTEM_SetOtaDnType Disable fail \n"));
	}
#endif

#if(WIFI_CONDUCT_ENABLE==YES)
	/* added for WIFI Conducted Test */
	VK_SYSTEM_Command("ifconfig eth0:1 down");
	VK_SYSTEM_Command("ifconfig eth1:1 down");
	VK_SYSTEM_Command("ifconfig eth1:2 down");
	VK_SYSTEM_Command("brctl addbr br0; brctl addif br0 eth0; brctl addif br0 eth1; ifconfig br0 192.168.1.50 up");

	eFvkErr = FVK_TASK_Create(INTRN_FAI_APP_BlinkLEDTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_LARGE_STACK_SIZE, "BlinkLed", NULL, &ulLEDTaskID);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_TASK_Create \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(ulLEDTaskID);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_TASK_Start \n"));
		return FVK_ERR;
	}


#endif			

	FDI_OSD_DrawFillRect(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT, RGB_COLOR_BLACK);
	FDI_OSD_FlushScreen(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT);

	INTRN_FAI_APP_InitFactoryWindow();
	//FAI_ERR_CHECK(INTRN_FAI_APP_InitFactoryWindow());

	/* flush all */
	FDI_OSD_FlushScreen(0, 0, FDI_OSD_MAX_WIDTH, FDI_OSD_MAX_HEIGHT);

	/* osd show */
	FDI_OSD_Show();

	FDI_KEY_RegisterEventCallback(INTRN_FAI_APP_KeyCallback);

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_KEY_MSG), (char*)"KEY_MSG", &s_ulKeyMsgID);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	
	eFvkErr = FVK_TASK_Create(INTRN_FAI_APP_RCUEventTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_LARGE_STACK_SIZE, "TestSW", NULL, &ulRcuTaskID);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_TASK_Create \n"));
		return FVK_ERR;
	}
#if defined(FACTORY_USE_ETHERNET)
	eFvkErr = FVK_TASK_Create(INTRN_FAI_APP_CheckEthStateTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Eth Task", NULL, &g_ulEthTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
#endif
	eFvkErr = FVK_TASK_Start(ulRcuTaskID);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_TASK_Start \n"));
		return FVK_ERR;
	}
#if defined(FACTORY_USE_ETHERNET)
	eFvkErr = FVK_TASK_Start(g_ulEthTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
#endif

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FrontKeyCallBack
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_APP_KeyCallback(FDI_KEY_DEVICE eDev, unsigned int nKeyCode, FDI_KEY_TYPE eKeyType)
{
	FVK_ERR_CODE eVkRet;	
	FAI_KEY_MSG keyMsg;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_APP_KeyCallback] : dev 0x%x, key code 0x%x, key type 0x%x\n", 
																	eDev, nKeyCode,eKeyType));
	
	keyMsg.eDev = eDev;
	keyMsg.nKeyCode = nKeyCode;
	keyMsg.eKeyType = eKeyType;
	
	if( !g_bExecTestFunc )
	{
		eVkRet = FVK_MSG_Send(s_ulKeyMsgID, &keyMsg, sizeof(FAI_KEY_MSG));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("Error FVK_MSG_Send() : %d\n", eVkRet));
			return;
		}
	}
	
	if(keyMsg.eDev == TEST_INPUT_TYPE_FKEY)
	{
		if(keyMsg.eKeyType == FDI_KEY_PRESSED)
		{
			FAI_FKEY_CALLBACK(keyMsg.nKeyCode);
		}
	}
	if(keyMsg.eDev == TEST_INPUT_TYPE_RCU)
	{
		if(keyMsg.eKeyType == FDI_KEY_PRESSED)
		{
			FAI_RCU_CALLBACK(keyMsg.nKeyCode);
		}
	}
} 

/*******************************************************************************
* function : FAI_APP_EventTask
* description : 메인 이벤트 루프이다. RCU 입력이 들어오면 해당 키를 현재 윈도우의 
* 				핸들러로 넘긴다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_APP_RCUEventTask(void *data)
{
	int i, j, retVal;
	FVK_ERR_CODE eVkRet;
	FAI_KEY_MSG keyMsg;
	FAI_WINDOW_H hCurWin;
	FAI_ERR_CODE eFaiErr;
	
	/* Process input RCU key */
	while(1)
	{
		eVkRet = FVK_MSG_Receive(s_ulKeyMsgID, &keyMsg, sizeof(FAI_KEY_MSG));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("Error FVK_MSG_Receive() : %d\n", eVkRet));
			continue;
		}

		eFaiErr = FAI_WINDOW_GetCurrentWindow(&hCurWin);
		if( eFaiErr != FAI_NO_ERR )
		{
			return;
		}

		if( keyMsg.eKeyType == FDI_KEY_PRESSED )
		{
			if( keyMsg.eDev == FDI_RCU_KEY )
			{
				g_ulCurrentRcuKey = keyMsg.nKeyCode;
			}
			hCurWin->pfnEventHandler(keyMsg.eDev, keyMsg.nKeyCode);
		}
	}
}

/******************************************************************************
* function : FAI_APP_InitMainWindow 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned long FAI_APP_GetCurrentRcuKey()
{
	return g_ulCurrentRcuKey;
}

/******************************************************************************
* function : INTRN_FAI_APP_InitFactoryWindow
* description :
* input :
* output : 
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE INTRN_FAI_APP_InitFactoryWindow()
{
	int i, j;
	FAI_TEXT_H hText;
	FAI_TESTMENU_H hTestMenu;
	char szText[100], szVersion[30];
	unsigned long ulSystemId;
	char *pcSheetName[] = { "Sheet1", "Sheet2", "Sheet3" };
	char *pcSubWindowName[] = { "SubWin1", "SubWin2", "SubWin3" };
	
	/* Create PP window */		
	FAI_ERR_CHECK(FAI_WINDOW_Create(&g_tAppInfo.hPPMainWindow, 
									"PPMainWindow", 
									APP_OSD_START_X, 
									APP_OSD_START_Y, 
									APP_MAIN_WINDOW_WIDTH, 
									APP_MAIN_WINDOW_HEIGHT, 
									MAIN_WINDOW_COLOR,
									FAI_WINDOW_NORMAL, 
									INTRN_FAI_APP_FactoryWindowEventHandler));

	/* Create title's model name text */
	memset(szText,0,sizeof(szText));
	sprintf(szText, "[%s]", MODEL_NAME);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								0, 
								5, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));

	/* Create title's system id text */
	memset(szText,0,sizeof(szText));
	FDI_SYSTEM_GetSystemId(&ulSystemId);
	sprintf(szText, "Sys_ID:0x%08X", ulSystemId);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								APP_MAIN_WINDOW_WIDTH/3, 
								5, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));

	/* Create title's factorysw version infomation */	
	memset(szText,0,sizeof(szText));
	sprintf(szText, "F_Ver:%s %s.%s", MODEL_STRING, FACTORYSW_APP_VERSION, RELEASE_VERSION);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								APP_MAIN_WINDOW_WIDTH/3*2-25, 
								5, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));
	

	/* Create title's sw version infomation */
	memset(szVersion,0,sizeof(szVersion));
	memset(szText,0,sizeof(szText));
	FDI_SYSTEM_GetAppVersion(szVersion);
	sprintf(szText, "A_Ver:%s", szVersion);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								0, 
								APP_TEXT_HEIGHT+APP_TAB, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));
	/* Create title's loader version infomation */	
	memset(szVersion,0,sizeof(szVersion));
	memset(szText,0,sizeof(szText));
	FDI_SYSTEM_GetLoaderVersion(szVersion);
#if 0//(PRODUCT_YSR2000==YES) || (PRODUCT_YS1000==YES)
	szVersion[0]='D';
	szVersion[1]='K';
	szVersion[2]='C';
	szVersion[3]='X';
	szVersion[4]='A';
	szVersion[5]='B';
	szVersion[6]=' ';
	szVersion[7]='0';
	szVersion[8]='.';
	szVersion[9]='0';
	szVersion[10]='1';
	szVersion[11]=0x0; 
#endif
	sprintf(szText, "L_Ver:%s", szVersion);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT+APP_TAB, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));
	
	/* display micom version */
#if !defined(CONFIG_MICOM_NONE)
#if 1// defined(FACTORY_USE_MICOM)
	memset(szVersion,0,sizeof(szVersion));
	memset(szText,0,sizeof(szText));
	FDI_SYSTEM_GetMicomVersion(szVersion);
	sprintf(szText, "M_Ver:%s", szVersion);
	FAI_ERR_CHECK(FAI_TEXT_Create(&hText, 
								g_tAppInfo.hPPMainWindow, 
								APP_MAIN_WINDOW_WIDTH/3*2-25, 
								APP_TEXT_HEIGHT+APP_TAB, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));
#endif
#endif
#if defined(FACTORY_USE_ETHERNET)
	/* Create title's ethernet state text */
	g_bEthLinked = FDI_ETHERNET_IsLinked(FDI_ETHERNET_FACTORY_DEV);
	memset(szText,0,sizeof(szText));
	g_bEthLinked ? sprintf(szText, "Ethernet:On") : sprintf(szText, "Ethernet:Off");
	FAI_ERR_CHECK(FAI_TEXT_Create(&g_hEthStateText, 
								g_tAppInfo.hPPMainWindow, 
								APP_MAIN_WINDOW_WIDTH/3*2-25, 
								APP_TEXT_HEIGHT*2+APP_TAB+5, 
								APP_MAIN_WINDOW_WIDTH/3, 
								APP_TEXT_HEIGHT, 
								RGB_COLOR_WHITE, 
								RGB_COLOR_NONE, 
								szText, 
								TEXT_ALIGN_NOT_USE));
#endif

#if (TOTAL_SHEET_NUMBER)
	/* Create sheet buttons and sub-windows */
	for( i = 0; i < TOTAL_SHEET_NUMBER; i++ )
	{ 
		FAI_ERR_CHECK(FAI_BUTTON_Create(&g_tAppInfo.hPPSheetButton[i], 
		 			 					g_tAppInfo.hPPMainWindow,
										APP_TAB+APP_SIDE_BLANK+(PP_TEST_MENU_WIDTH+APP_TAB)*i, 
										APP_TITLE_HEIGHT+APP_SIDE_BLANK, 
										PP_TEST_MENU_WIDTH, 
										20, 
										RGB_COLOR_BLUE,
										APP_BUTTON_NO_SELECTED_COLOR, 
										pcSheetName[i], 
										TEXT_ALIGN_MIDDLE));
		/* Create sheet's sub window */
		FAI_ERR_CHECK(FAI_WINDOW_CreateSubWindow(&g_tAppInfo.hPPSheetWindow[i], 
											g_tAppInfo.hPPMainWindow, 
											pcSubWindowName[i], 
											APP_SIDE_BLANK, 
											APP_TITLE_HEIGHT+APP_SIDE_BLANK+20, 
											APP_MAIN_WINDOW_WIDTH-APP_TAB, 
											APP_MAIN_WINDOW_HEIGHT-(APP_TITLE_HEIGHT+APP_SIDE_BLANK+20)-APP_SIDE_BLANK, 
											PP_SHEET_COLOR,
											FAI_WINDOW_NORMAL, 
											NULL));
		if( i == 0 )
		{
			//FAI_ERR_CHECK(FAI_WIDGET_SetFocus(&g_tAppInfo.hPPSheetButton[0]->tWidget, FAI_WIDGET_FOCUS));
		}
		else
		{
			FAI_ERR_CHECK(FAI_WIDGET_Link(&g_tAppInfo.hPPSheetButton[i-1]->tWidget, &g_tAppInfo.hPPSheetButton[i]->tWidget, FAI_WIDGET_LINK_RIGHT));
			FAI_ERR_CHECK(FAI_WIDGET_Link(&g_tAppInfo.hPPSheetWindow[i-1]->tWidget, &g_tAppInfo.hPPSheetWindow[i]->tWidget, FAI_WIDGET_LINK_RIGHT));
			FAI_ERR_CHECK(FAI_WINDOW_Hide(g_tAppInfo.hPPSheetWindow[i])); 
		}
	}	
	FAI_ERR_CHECK(FAI_WINDOW_SetFocusWidget(g_tAppInfo.hPPMainWindow, &g_tAppInfo.hPPSheetButton[0]->tWidget));
	FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_1));

	/* Create Empty Test Menus */
	for( i = 0; i < TOTAL_SHEET_NUMBER; i++ )
	{
		for( j = 0; j < PP_TOTAL_MENU_NUM; j++ )
		{
			if( j < PP_TEST_MENU_COLUMN*PP_TEST_MENU_LOW )
			{
				FAI_TESTMENU_Create(&hTestMenu,
												g_tAppInfo.hPPSheetWindow[i],
												NULL, 
												NULL, 
												g_atTestMenuPositionFor5x4[j].nX, 
												g_atTestMenuPositionFor5x4[j].nY, 
												PP_TEST_MENU_WIDTH, 
												PP_TEST_MENU_HEIGHT, 
												TEST_MENU_TYPE_NORMAL);
			}
			else
			{
				FAI_TESTMENU_Create(&hTestMenu,
												g_tAppInfo.hPPSheetWindow[i],
												NULL, 
												NULL, 
												g_atTestMenuPositionFor5x4[j].nX, 
												g_atTestMenuPositionFor5x4[j].nY, 
												PP_KEY_MENU_WIDTH, 
												PP_KEY_MENU_HEIGHT, 
												TEST_MENU_TYPE_KEY);
			}
		}
		//g_atExeTestParamTable[i][j] = NULL;
	}

	/* Create Test Menus */ 
	for( i = 0; i < TOTAL_OSD_TEST_PARAM; i++ )
	{
		if( g_atAllTestParamTable[i].pcTestName != NULL )
		{
			if( g_atAllTestParamTable[i].unSheet != SHEET_NUMBER_NONE && g_atAllTestParamTable[i].unPosition != CELL_NUMBER_NONE )
			{
				if( g_atAllTestParamTable[i].eInputType == TEST_INPUT_TYPE_RCU )
				{
					FAI_TESTMENU_Create(&g_tAppInfo.hPPMenu[g_atAllTestParamTable[i].unSheet][g_atAllTestParamTable[i].unPosition],
													g_tAppInfo.hPPSheetWindow[g_atAllTestParamTable[i].unSheet],
													g_atAllTestParamTable[i].pcTestName, 
													RCUCode2Str(g_atAllTestParamTable[i].unRcuKey), 
													g_atTestMenuPositionFor5x4[g_atAllTestParamTable[i].unPosition].nX, 
													g_atTestMenuPositionFor5x4[g_atAllTestParamTable[i].unPosition].nY, 
														PP_TEST_MENU_WIDTH, 
														PP_TEST_MENU_HEIGHT, 
														TEST_MENU_TYPE_NORMAL);
				}
				else if( g_atAllTestParamTable[i].eInputType == TEST_INPUT_TYPE_FKEY )
				{ 
					FAI_TESTMENU_Create(&g_tAppInfo.hPPMenu[g_atAllTestParamTable[i].unSheet][g_atAllTestParamTable[i].unPosition],
													g_tAppInfo.hPPSheetWindow[g_atAllTestParamTable[i].unSheet], 
													g_atAllTestParamTable[i].pcTestName, 
													FKeyCode2Str(g_atAllTestParamTable[i].unRcuKey), 
													g_atTestMenuPositionFor5x4[g_atAllTestParamTable[i].unPosition].nX, 
													g_atTestMenuPositionFor5x4[g_atAllTestParamTable[i].unPosition].nY, 
													PP_KEY_MENU_WIDTH, 
													PP_KEY_MENU_HEIGHT, 
													TEST_MENU_TYPE_KEY);
				}
			}
			g_atExeTestParamTable[s_ulExeMenuNum++] = &g_atAllTestParamTable[i];
		}
	}
	FAI_ERR_CHECK(FAI_WIDGET_SetFocus(&g_tAppInfo.hPPSheetButton[0]->tWidget, FAI_WIDGET_FOCUS));
#endif

	FAI_ERR_CHECK(FAI_WINDOW_SetCurrentWindow(g_tAppInfo.hPPMainWindow));

	
	FAI_ERR_CHECK(FAI_WINDOW_Draw(g_tAppInfo.hPPMainWindow));

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_APP_FactoryWindowEventHandler
* description : Factory Window 이벤트 핸들러 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_APP_FactoryWindowEventHandler(unsigned int unDeviceType, unsigned int unKeyCode)
{
	int i, j;
	FAI_TEST_RESULT eResult;
	FAI_WINDOW_H hCurWin;
	FAI_WIDGET_H hCurWidget;
	FAI_PP_SHEET eCurSheet;
	FAI_TESTMENU_T *hMenu;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_APP_PPWindowEventHandler] : Key code 0x%x\n", unKeyCode));

#if (CLEAR_MENU_UI_ENABLE==YES)
	if( unDeviceType == TEST_INPUT_TYPE_RCU && unKeyCode == CLEAR_MENU_INPUT )
	{
		FAI_APP_ClearTestMenus();
		return FAI_NO_ERR;
	}
#endif	

#if (GUI_CAPTURE_UI_ENABLE==YES)
	if( unDeviceType == TEST_INPUT_TYPE_RCU && unKeyCode == GUI_CAPTURE_INPUT )
	{
		FAI_APP_GUICapture();
		return FAI_NO_ERR;
	}
#endif

	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hCurWin));	
	FAI_ERR_CHECK(FAI_APP_GetCurrentSheet(&eCurSheet));
	FAI_ERR_CHECK(FAI_WINDOW_GetFocusWidget(hCurWin, &hCurWidget));

	if( unKeyCode == RCU_KEY_UP && unDeviceType == TEST_INPUT_TYPE_RCU )
	{
		if( hCurWidget->ptLeft != NULL )
		{
			FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptLeft));
			if( eCurSheet == FAI_PP_SHEET_2 )
			{
				FAI_ERR_CHECK(FAI_WIDGET_Move(&g_tAppInfo.hPPSheetWindow[1]->tWidget, &g_tAppInfo.hPPSheetWindow[0]->tWidget));				
				FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_1));
			}
			else if( eCurSheet == FAI_PP_SHEET_3 )
			{
				FAI_ERR_CHECK(FAI_WIDGET_Move(&g_tAppInfo.hPPSheetWindow[2]->tWidget, &g_tAppInfo.hPPSheetWindow[1]->tWidget));
				FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_2));	
			}
		}
		return FAI_NO_ERR;
	}
	else if( unKeyCode == RCU_KEY_DOWN && unDeviceType == TEST_INPUT_TYPE_RCU )
	{
		if( hCurWidget->ptRight != NULL )
		{
			FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptRight));
			FAI_ERR_CHECK(FAI_APP_GetCurrentSheet(&eCurSheet));			
			if( eCurSheet == FAI_PP_SHEET_1 )
			{
				FAI_ERR_CHECK(FAI_WIDGET_Move(&g_tAppInfo.hPPSheetWindow[0]->tWidget, &g_tAppInfo.hPPSheetWindow[1]->tWidget));
				FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_2));					
			}
			else if( eCurSheet == FAI_PP_SHEET_2 )
			{
				FAI_ERR_CHECK(FAI_WIDGET_Move(&g_tAppInfo.hPPSheetWindow[1]->tWidget, &g_tAppInfo.hPPSheetWindow[2]->tWidget));
				FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_3));					
			}
		}
		return FAI_NO_ERR;
	}
	
	for( i = 0; i < s_ulExeMenuNum; i++ )
	{
		if( g_atExeTestParamTable[i] == NULL )
		{			
			continue;
		}
		
		if( g_atExeTestParamTable[i]->unSheet == eCurSheet && g_atExeTestParamTable[i]->unRcuKey == unKeyCode &&
			g_atExeTestParamTable[i]->eInputType == unDeviceType &&	g_atExeTestParamTable[i]->pfnTestFunc != NULL )
		{
			/* set selected menu to current menu */
			FAI_ERR_CHECK(FAI_TESTMENU_SetCurrentMenu(g_tAppInfo.hPPMenu[g_atExeTestParamTable[i]->unSheet][g_atExeTestParamTable[i]->unPosition]));
			
			/* execute test function */
			FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_APP_PPWindowEventHandler] : Execute FAI_UI Test Function\n"));
			g_bExecTestFunc = TRUE;
			eResult = g_atExeTestParamTable[i]->pfnTestFunc();
			g_bExecTestFunc = FALSE;
		}
	}
	return FAI_NO_ERR;	
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_GetCurrentSheet(FAI_PP_SHEET *peCurSheet)
{
	*peCurSheet = g_tAppInfo.ePPCurrentSheet;

	return FAI_NO_ERR;	
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_SetCurrentSheet(FAI_PP_SHEET eCurSheet)
{
	g_tAppInfo.ePPCurrentSheet = eCurSheet;

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_GetSheetWindowHandle(FAI_PP_SHEET eSheet, FAI_WINDOW_H *phWin)
{
	if( eSheet == FAI_PP_SHEET_1 )
	{
		*phWin = g_tAppInfo.hPPSheetWindow[0];
	}
	else if( eSheet == FAI_PP_SHEET_2 )
	{
		*phWin = g_tAppInfo.hPPSheetWindow[1];
	}
	else if( eSheet == FAI_PP_SHEET_3 )
	{
		*phWin = g_tAppInfo.hPPSheetWindow[2];
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_GetTestMenuHandle(FAI_PP_SHEET eSheet, unsigned char ucPos, FAI_TESTMENU_H *phMenu)
{
	if( eSheet >= TOTAL_SHEET_NUMBER || ucPos >= PP_TOTAL_MENU_NUM || phMenu == NULL )
	{
		return FAI_ERR_INVALID_PARAMETER;
	}
	*phMenu = g_tAppInfo.hPPMenu[eSheet][ucPos];
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_APP_ChangeEthStateText(BOOL bIsLinked)
{
	if( bIsLinked )
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(g_hEthStateText, RGB_COLOR_WHITE, MAIN_WINDOW_COLOR, "Ethernet:On"));
	}
	else
	{
		FAI_ERR_CHECK(FAI_TEXT_Change(g_hEthStateText, RGB_COLOR_WHITE, MAIN_WINDOW_COLOR, "Ethernet:Off"));	
	}
	return FAI_NO_ERR;
}


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(FACTORY_USE_ETHERNET)
void INTRN_FAI_APP_CheckEthStateTask(void *pData)
{
	BOOL bEthState;
	
	while(1)
	{
		FVK_TASK_Sleep(1000);
		bEthState = FDI_ETHERNET_IsLinked(FDI_ETHERNET_FACTORY_DEV);
		if( bEthState != g_bEthLinked )
		{
			g_bEthLinked = bEthState;
			FAI_APP_ChangeEthStateText(bEthState);
		}
	}
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CLEAR_MENU_UI_ENABLE==YES)
FAI_ERR_CODE FAI_APP_ClearTestMenus() /* Todo: need to update test status */
{
	int i, j;
	FAI_WINDOW_H hWin;
	
	FDI_PANEL_Display("    ");
	
	for( i = 0; i < TOTAL_SHEET_NUMBER; i++ )
	{
		for( j = 0; j < PP_TOTAL_MENU_NUM; j++ )
		{
			if(g_tAppInfo.hPPMenu[i][j] != NULL)
			{
				if( FAI_TESTMENU_Reset(g_tAppInfo.hPPMenu[i][j]) != FAI_NO_ERR )
				{
					continue;
				}
			}
		}
	}
	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hWin));
	FAI_ERR_CHECK(FAI_WINDOW_Draw(hWin));
	
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (GUI_CAPTURE_UI_ENABLE==YES)
FAI_ERR_CODE  FAI_APP_SaveBMP(BITMAP_INFO_HEADER_t *pInfoHeader, void *pBuffer)
{
	FVK_ERR_CODE eError;
	BITMAP_FILE_HEADER_t tFileHeader;
	unsigned long ulFileHeaderSize;
	FILE *pHandle;

	if((pInfoHeader->ulSizeImage == 0) || (pInfoHeader->ulCompression != BI_RGB))
	{
		return FVK_ERR;
	}

	pHandle = fopen("/var/GUICapture.bmp", "a");
	if(pHandle == NULL)
	{
		return FVK_ERR;
	}
	
	ulFileHeaderSize = sizeof(BITMAP_FILE_HEADER_t);
	tFileHeader.usType = 0x4D42; // 'MB'
	tFileHeader.ulOffset = ulFileHeaderSize + pInfoHeader->ulInfoHeaderSize;
	tFileHeader.usFILESize = tFileHeader.ulOffset + pInfoHeader->ulSizeImage;
	tFileHeader.usReserved1 = 0;
	tFileHeader.usReserved2 = 0;

	fwrite(&tFileHeader, ulFileHeaderSize, 1, pHandle);
	fwrite(pInfoHeader, pInfoHeader->ulInfoHeaderSize, 1, pHandle);
	fwrite(pBuffer, pInfoHeader->ulSizeImage, 1, pHandle);
	fclose(pHandle);

    return FVK_NO_ERR;
}

FAI_ERR_CODE FAI_APP_GUICapture(void)
{
	int temp_i;
	int temp_k;
	FVK_ERR_CODE eError;
	unsigned long *pFB_Addr;
	unsigned long *pBuffer;
	BITMAP_INFO_HEADER_t tInfoHeader;
	
	tInfoHeader.ulInfoHeaderSize = sizeof(BITMAP_INFO_HEADER_t);
	tInfoHeader.ulWidth = FDI_OSD_MAX_WIDTH;
	tInfoHeader.ulHeight = FDI_OSD_MAX_HEIGHT;
	tInfoHeader.ulPlanes = 1;
	tInfoHeader.ulBitCount = 32;
	tInfoHeader.ulCompression = BI_RGB;
	tInfoHeader.ulSizeImage = FDI_OSD_MAX_WIDTH * FDI_OSD_MAX_HEIGHT * 4;
	tInfoHeader.ulXPelsPerMeter = 0;
	tInfoHeader.ulYPelsPerMeter = 0;
	tInfoHeader.ulClrUsed = 0;
	tInfoHeader.ulClrImportant = tInfoHeader.ulClrUsed;

	FDI_OSD_GetFrameBuffer((void**)&pFB_Addr);
	pBuffer = FVK_MEM_Alloc(tInfoHeader.ulSizeImage);
	if(pBuffer == NULL)
	{
		return FVK_ERR;
	}

	for(temp_i = 0; temp_i < FDI_OSD_MAX_HEIGHT; temp_i++)
	{
		for(temp_k = 0; temp_k < FDI_OSD_MAX_WIDTH; temp_k++)
		{
//			pBuffer[(FDI_OSD_MAX_HEIGHT - 1 - temp_i) * FDI_OSD_MAX_WIDTH + (FDI_OSD_MAX_WIDTH - 1 - temp_k)] = pFB_Addr[temp_i * FDI_OSD_MAX_WIDTH + temp_k];
			pBuffer[(FDI_OSD_MAX_HEIGHT - 1 - temp_i) * FDI_OSD_MAX_WIDTH + temp_k] = pFB_Addr[temp_i * FDI_OSD_MAX_WIDTH + temp_k];
		}
	}

	eError = FAI_APP_SaveBMP(&tInfoHeader, pBuffer);
	if(eError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_GUICapture] : FAI_APP_SaveBMP Error!!\n"));
	}
	FVK_MEM_Free(pBuffer);

	return eError;
}
#endif

