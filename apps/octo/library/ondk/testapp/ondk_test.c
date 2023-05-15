/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>






/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define ONDK_TEST_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}

#define DEF_ONDK_TESTPAGE_X  0
#define DEF_ONDK_TESTPAGE_Y  0
#define DEF_ONDK_TESTPAGE_W  1280
#define DEF_ONDK_TESTPAGE_H  720

#define DEF_ONDK_TESTPAGE_BTN_W  120
#define DEF_ONDK_TESTPAGE_BTN_H  60
#define DEF_ONDK_TESTPAGE_BTN_X  10
#define DEF_ONDK_TESTPAGE_BTN_Y  10

#define DEF_ONDK_TESTPAGE_ITEM_NAME_LENGTH 16




typedef enum tagTESTPAGE_Object
{
	eTESTPAGE_Object_FRAME_Background = 0x991,
	eTESTPAGE_Object_RECT_DATA,
	eTESTPAGE_Object_RECT_DataOutLine,

	eTESTPAGE_Object_BTN_ID,

	eTESTPAGE_Object_BTN_MAX
}TESTPAGE_Object_e;



typedef struct tagTESTPAGE_Item_t
{
	HCHAR	szName[DEF_ONDK_TESTPAGE_ITEM_NAME_LENGTH];
	void 	(*callbackTest)(void);
} TESTPAGE_Item_t;

typedef struct tagTESTPAGE_Contents_t
{
	HINT32		nItemCount;
	HxList_t	*pszTestModule; /* TESTPAGE_Item_t */
} TESTPAGE_Contents_t;

STATIC TESTPAGE_Contents_t *s_pstTesgPageContents = NULL;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC TESTPAGE_Contents_t *local_testpage_GetContents(void)
{
	return s_pstTesgPageContents;
}

STATIC HxList_t *local_ondk_test_Append(HxList_t *list, HCHAR *moduleName, void (*callback)(void))
{
	TESTPAGE_Item_t *pTest = NULL;

	pTest = ONDK_Calloc(sizeof(TESTPAGE_Item_t));
	ONDK_Assert(pTest);
	HxSTD_StrNCpy(pTest->szName, moduleName, DEF_ONDK_TESTPAGE_ITEM_NAME_LENGTH);
	pTest->callbackTest = callback;

	list = HLIB_LIST_Append(list, pTest);

	return list;
}

STATIC void local_ondk_test_Find(void *userdata, void *data)
{
	TESTPAGE_Item_t *pTest = (TESTPAGE_Item_t *)data;

	if(NULL == pTest)
	{
		return;
	}

	if(NULL == userdata || 0 == HLIB_STD_StrCaseCmp(pTest->szName, userdata))
	{
		HLIB_CMD_Printf("-----------------\r\n");
		HLIB_CMD_Printf("Test module : %s\r\n", pTest->szName);
		pTest->callbackTest();
		HLIB_CMD_Printf("-----------------\r\n");
	}

}


STATIC void local_testpage_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
	ONDK_GWM_COM_Button_SetAlign(id, ONDKSTF_CENTER);
}


STATIC void local_testpage_SetUiObject(TESTPAGE_Contents_t *pstContents)
{
	ONDK_Rect_t 	rect;
	HINT32			idx = 0;
	HxList_t		*pList = NULL;
	TESTPAGE_Item_t	*pItem = NULL;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TESTPAGE_X, DEF_ONDK_TESTPAGE_Y, DEF_ONDK_TESTPAGE_W, DEF_ONDK_TESTPAGE_H);
#if 0
	rect = ONDK_Rect(DEF_ONDK_TESTPAGE_X, DEF_ONDK_TESTPAGE_Y, DEF_ONDK_TESTPAGE_W, DEF_ONDK_TESTPAGE_H);
	ONDK_GWM_COM_Rect_Create(eTESTPAGE_Object_FRAME_Background, &rect, 0xFFFFFFFF);
#endif

	rect = ONDK_Rect(DEF_ONDK_TESTPAGE_BTN_X, DEF_ONDK_TESTPAGE_BTN_Y, DEF_ONDK_TESTPAGE_BTN_W, DEF_ONDK_TESTPAGE_BTN_H);
	pList = pstContents->pszTestModule;
	while(pList)
	{
		pItem = (TESTPAGE_Item_t *)HLIB_LIST_Data(pList);
		ONDK_Assert(pItem);

		local_testpage_SetUiObject_CreateButton((eTESTPAGE_Object_BTN_ID+idx), pItem->szName, rect);
		rect.x += (DEF_ONDK_TESTPAGE_BTN_W + 10);
		if(rect.x > (DEF_ONDK_TESTPAGE_X+DEF_ONDK_TESTPAGE_W))
		{
			rect.x = DEF_ONDK_TESTPAGE_BTN_X;
			rect.y += (DEF_ONDK_TESTPAGE_BTN_H+10);
		}
		idx++;
		pList = HLIB_LIST_Next(pList);
	}
	pstContents->nItemCount = idx;

	ONDK_GWM_Obj_SetFocus (eTESTPAGE_Object_BTN_ID);
}

/**************************************************************************************************/
#define _________AP_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testpage_MsgGwmCreate(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testpage_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testpage_MsgGwmKeyDown(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	switch(nKeyId)
	{
	case KEY_EXIT:
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(NULL);
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testpage_MsgGwmClicked(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nCmdId = (HUINT32)p1;

	HLIB_LIST_FindEx(pstContents->pszTestModule, (void*)ONDK_GWM_COM_Button_GetTitle(nCmdId), (void *)local_ondk_test_Find);
	ONDK_GWM_APP_Destroy(NULL);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testpage_MsgGwmDestroy(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testpage_MsgGwmScrollRight(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus (eTESTPAGE_Object_BTN_ID);

	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testpage_MsgGwmScrollLeft(TESTPAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTPAGE_Object_BTN_ID+(pstContents->nItemCount-1)));

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TESTPAGE_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTPAGE_Contents_t 	*pstContents = local_testpage_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testpage_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testpage_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_testpage_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testpage_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_testpage_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_testpage_MsgGwmScrollLeft(pstContents, p1, p2, p3);
		break;
	default:
		break;
	}

	if(eResult != GWM_RET_OK)
	{
		return eResult;
	}
	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}


void ONDK_TEST_Init(void)
{
	TESTPAGE_Contents_t *pContent = NULL;

	if(NULL != s_pstTesgPageContents)
	{
		return;
	}
	s_pstTesgPageContents = ONDK_Calloc(sizeof(TESTPAGE_Contents_t));
	ONDK_Assert(s_pstTesgPageContents);

	pContent = s_pstTesgPageContents;


	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "OSD", 		ONDK_TEST_OSD_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "STRING",		ONDK_TEST_STRING_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "IMAGE",		ONDK_TEST_IMAGE_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "TIMER",		ONDK_TEST_TIMER_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "FONT",		ONDK_TEST_FONT_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "MSGHOOK",	ONDK_TEST_MSGHOOK_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "FPS",		ONDK_TEST_FPS_Start);
	pContent->pszTestModule = local_ondk_test_Append(pContent->pszTestModule, "O2ANI",		ONDK_TEST_O2ANI_Start);
}

void ONDK_TEST_Start(HCHAR *module)
{
	HCHAR 				*moduleName = module;
	TESTPAGE_Contents_t *pstContents = NULL;

	ONDK_TEST_Init();
	pstContents = local_testpage_GetContents();

	if(NULL == moduleName)
	{
		moduleName = "PAGE";
	}

	if(0 == HLIB_STD_StrCaseCmp(moduleName, "PAGE"))
	{
		if(NULL == ONDK_GWM_APP_Get(ONDK_TESTPAGE_Proc))
		{
			HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TESTPAGE_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TESTPAGE_Proc, 0, 0, 0, 0);
		}
		else
		{
			HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
			ONDK_GWM_APP_Destroy(ONDK_TESTPAGE_Proc);
		}
	}
	else
	{
		HLIB_LIST_FindEx(pstContents->pszTestModule, (void *)moduleName, (void *)local_ondk_test_Find);
	}
}



