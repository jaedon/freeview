/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#if !defined(CONFIG_OS_RTOS)
#include <sys/mount.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_usb.h"
#include "fai_tmd.h"
#include "fdi_usb.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"


/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_USB_MAX_NUM			2
#define USB_TEST_RETRY_NUM		3

#define USB_FILE_SYSTEM 		"vfat"
#define USB_FILE_PATH 			"/media/"

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static BOOL s_bUsbInserted[FAI_USB_MAX_NUM];
unsigned long g_ulNumOfUsb;

FAI_ERR_CODE INTRN_FAI_USB_NotifyFunc(FDI_USB_EVENT_e eEventType, FDI_USB_Info_t *ptUsbInfo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_USB_Init(void)
{
	int i;
	FAI_ERR_CODE eRet;

	eRet = FDI_USB_GetCapability(&g_ulNumOfUsb);

	for( i = 0; i < g_ulNumOfUsb; i++ )
	{
		s_bUsbInserted[i] = FALSE;
	}
	
	eRet |= FDI_USB_RegisterCallback((pfnFDI_USB_NotifyFunc)INTRN_FAI_USB_NotifyFunc);
	eRet |= FDI_USB_HOST_CTRL_SetPower(TRUE);
	
	return eRet;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (USB_DEVICE_DECTION_ENABLE==YES)
FAI_ERR_CODE FAI_USB_Detection(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
    unsigned char ucUsbID;
    FAI_TMD_RESULT_T tTMDResult;
    FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	unsigned char ucTmdErrCode[2];

    FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
    tTMDResult.ucCategory=CATEGORY_PERIPHERAL;
    tTMDResult.ucModule=MODULE_PERIPHERAL_USB;       
    tTMDResult.ucAction=0x01; /* detection */

    if( ucArgc != 1 ) //ERR - FAI_TMD_ERR_FAIL
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[FAI_USB_Detection] : Argument Count Error=%d\n", ucArgc));
        eTmdErr= FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
    }

    FAI_TMD_GetParam(&ucUsbID, pucArgList, 0);

    
    if( ucUsbID >= g_ulNumOfUsb ) //ERR - FAI_TMD_ERR_FAIL
    {
        eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
		goto END;
    }

    if( s_bUsbInserted[ucUsbID] != TRUE ) //ERR - NOT INSERTED USB
    {
        eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_USB;
		ucTmdErrCode[1] = FAI_USB_TMD_ERR_DISCONNECT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
		goto END;
    }

END:    
    tTMDResult.eResult = eTmdErr;
    FAI_TMD_SetResult(&tTMDResult);

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
#if (USB_UI_ENABLE==YES)
FAI_ERR_CODE FAI_USB_Test_UI(void)
{
	/* Display usb test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	if( g_ulNumOfUsb == 1 )
	{
		if( s_bUsbInserted[0] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_STR);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB_DETECT_FAIL_STR);
		}
	}
	else if( g_ulNumOfUsb == 2 )
	{
		if( s_bUsbInserted[0] == TRUE && s_bUsbInserted[1] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB 1 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB 2 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_1_2_STR);
		}
		else if( s_bUsbInserted[0] == TRUE && s_bUsbInserted[1] == FALSE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB 1 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB 2 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_1_STR);
		}
		else if( s_bUsbInserted[0] == FALSE && s_bUsbInserted[1] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB 1 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB 2 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_2_STR);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB 1 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB 2 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB_DETECT_FAIL_1_2_STR);
		}

	}
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
#if (USB_EXT_UI_ENABLE==YES)
FAI_ERR_CODE FAI_USB_EXT_Test_UI(void)
{
	/* Display usb test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB3 Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	if( g_ulNumOfUsb == 3 )
	{
		if( s_bUsbInserted[2] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB3 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB3_DETECT_PASS_STR);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "USB3 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB3_DETECT_FAIL_STR);
		}
	}
	else if( g_ulNumOfUsb == 4 )
	{
		if( s_bUsbInserted[2] == TRUE && s_bUsbInserted[3] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB3 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB4 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_1_2_STR);
		}
		else if( s_bUsbInserted[2] == TRUE && s_bUsbInserted[3] == FALSE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB3 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB4 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_1_STR);
		}
		else if( s_bUsbInserted[2] == FALSE && s_bUsbInserted[3] == TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB3 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB4 Test", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);
			FDI_PANEL_Display(FAI_USB_DETECT_PASS_2_STR);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "USB3 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "USB4 Test", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_USB_DETECT_FAIL_1_2_STR);
		}

	}
	
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
FAI_ERR_CODE INTRN_FAI_USB_NotifyFunc(FDI_USB_EVENT_e eEventType, FDI_USB_Info_t *ptUsbInfo)
{

	char szUsbFileName[10];
	
	if( eEventType == FDI_USB_EVENT_INSERT )
	{
#if !defined(CONFIG_PLATFORM_ALI)		
		sprintf(szUsbFileName, "/dev/%s1", ptUsbInfo->szUsbDevName);
		if( mount(szUsbFileName, USB_FILE_PATH, USB_FILE_SYSTEM, 0, NULL) < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_USB_NotifyFunc] : mount Error!!\n"));
		}
#endif		
		
		if( ptUsbInfo->ulUsbId == 0 )
		{
			s_bUsbInserted[0] = TRUE;
		}
		else if( ptUsbInfo->ulUsbId == 1 )
		{
			s_bUsbInserted[1] = TRUE;
		}
		else if( ptUsbInfo->ulUsbId == 2 )
		{
			s_bUsbInserted[2] = TRUE;
		}
	}
	else if( eEventType == FDI_USB_EVENT_EXTRACT )
	{
#if !defined(CONFIG_PLATFORM_ALI)		
		umount(USB_FILE_PATH);
#endif		
		if( ptUsbInfo->ulUsbId == 0 )
		{
			s_bUsbInserted[0] = FALSE;
		}
		else if( ptUsbInfo->ulUsbId == 1 )
		{
			s_bUsbInserted[1] = FALSE;
		}
		else if( ptUsbInfo->ulUsbId == 2 )
		{
			s_bUsbInserted[2] = FALSE;
		}
		
		FVK_MEM_Set(ptUsbInfo->szUsbDevName, NULL, 20);
	}

	return FDI_NO_ERR;
}

