/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		ap_meminfo.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <sys/sysinfo.h>

#include <prism.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	HIDDEN_MEMINFO_ITEM_ID						200

#define	HIDDEN_MEMINFO_WINDOW_X					1280 - HIDDEN_MEMINFO_WINDOW_WIDTH
#define	HIDDEN_MEMINFO_WINDOW_Y					0
#define	HIDDEN_MEMINFO_WINDOW_WIDTH				430
#define	HIDDEN_MEMINFO_WINDOW_HEIGHT				372

#define	HIDDEN_MEMINFO_FRAME_ID					(HIDDEN_MEMINFO_ITEM_ID + 1)
#define	HIDDEN_MEMINFO_FRAME_X						0
#define	HIDDEN_MEMINFO_FRAME_Y						0
#define	HIDDEN_MEMINFO_FRAME_W						HIDDEN_MEMINFO_WINDOW_WIDTH
#define	HIDDEN_MEMINFO_FRAME_H						HIDDEN_MEMINFO_WINDOW_HEIGHT - 217
#define	HIDDEN_MEMINFO_FRAME_H_FULL				HIDDEN_MEMINFO_WINDOW_HEIGHT

#define	HIDDEN_MEMINFO_TITLE_ID					(HIDDEN_MEMINFO_ITEM_ID + 2)
#define	HIDDEN_MEMINFO_TITLE_X						HIDDEN_MEMINFO_FRAME_X + 122
#define	HIDDEN_MEMINFO_TITLE_Y						32
#define	HIDDEN_MEMINFO_TITLE_W						200
#define	HIDDEN_MEMINFO_TITLE_H						32

#define	HIDDEN_MEMINFO_TOTAL_CPU_ID 					(HIDDEN_MEMINFO_ITEM_ID + 3)
#define	HIDDEN_MEMINFO_TOTAL_CPU_X						65
#define	HIDDEN_MEMINFO_TOTAL_CPU_Y						75
#define	HIDDEN_MEMINFO_TOTAL_CPU_W						(HIDDEN_MEMINFO_FRAME_W - HIDDEN_MEMINFO_TOTAL_CPU_X * 2)
#define	HIDDEN_MEMINFO_TOTAL_CPU_H						35

#define	HIDDEN_MEMINFO_TOTAL_MEM_ID						(HIDDEN_MEMINFO_ITEM_ID + 4)
#define	HIDDEN_MEMINFO_TOTAL_MEM_X						HIDDEN_MEMINFO_TOTAL_CPU_X
#define	HIDDEN_MEMINFO_TOTAL_MEM_Y						102
#define	HIDDEN_MEMINFO_TOTAL_MEM_W						HIDDEN_MEMINFO_TOTAL_CPU_W
#define	HIDDEN_MEMINFO_TOTAL_MEM_H						HIDDEN_MEMINFO_TOTAL_CPU_H


#define	HIDDEN_MEMINFO_CATEGORY_LIST_ID			(HIDDEN_MEMINFO_ITEM_ID + 5)
#define	HIDDEN_MEMINFO_CATEGORY_LIST_X				HIDDEN_MEMINFO_TOTAL_CPU_X
#define	HIDDEN_MEMINFO_CATEGORY_LIST_Y				HIDDEN_MEMINFO_FRAME_Y + HIDDEN_MEMINFO_FRAME_H
#define	HIDDEN_MEMINFO_CATEGORY_LIST_W				HIDDEN_MEMINFO_TOTAL_CPU_W
#define	HIDDEN_MEMINFO_CATEGORY_LIST_H				224

#define	HIDDEN_MEMINFO_CATEGORY_TITLE_TAB			6
#define	HIDDEN_MEMINFO_CATEGORY_TITLE_X				HIDDEN_MEMINFO_TOTAL_CPU_X
#define	HIDDEN_MEMINFO_CATEGORY_TITLE_Y			(HIDDEN_MEMINFO_CATEGORY_TITLE_TAB)
#define	HIDDEN_MEMINFO_CATEGORY_TITLE_W			(HIDDEN_MEMINFO_CATEGORY_LIST_W - HIDDEN_MEMINFO_CATEGORY_TITLE_X)
#define	HIDDEN_MEMINFO_CATEGORY_TITLE_H			18

#define	HIDDEN_MEMINFO_CATEGORY_ITEM_TAB			8
#define	HIDDEN_MEMINFO_CATEGORY_ITEM_X				16
#define	HIDDEN_MEMINFO_CATEGORY_ITEM_Y				(HIDDEN_MEMINFO_CATEGORY_TITLE_Y + HIDDEN_MEMINFO_CATEGORY_TITLE_H + HIDDEN_MEMINFO_CATEGORY_ITEM_TAB)
#define	HIDDEN_MEMINFO_CATEGORY_ITEM_MG			10

#define	HIDDEN_MEMINFO_CATEGORY_NAME_W			115
#define	HIDDEN_MEMINFO_CATEGORY_MEM_W				115
#define	HIDDEN_MEMINFO_CATEGORY_CPU_W				50

#define	HIDDEN_MEMINFO_CATEGORY_VALUE_TAB			14
#define	HIDDEN_MEMINFO_CATEGORY_VALUE_LS			12
#define	HIDDEN_MEMINFO_CATEGORY_VALUE_H			20

#define	HIDDEN_MEMINFO_UPDATE_ID					(HIDDEN_MEMINFO_ITEM_ID + 6)
#define	HIDDEN_MEMINFO_UPDATE_TIME					(2 * NX_COMMON_TIME_SECOND)		/* 2 Second */

#define	MAX_MONITOR_PROCESS_NUM					5

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tag_MENUHIDDENMEMINFO_CONTENT
{
	HBOOL					bShowDetail;
	HCHAR					szTotalCPU[128];
	HCHAR					szTotalMemory[128];
	HSystemMointorInfo_t	*pstSystemMemInfo;
} MenuHiddenMemInfoContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static MenuHiddenMemInfoContent_t		s_stHiddenMemInfoContent;
static HBOOL							s_keyCountForDetail	= 0;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline MenuHiddenMemInfoContent_t	*local_memoryInfo_GetContents(void);
static HBOOL			local_memoryInfo_CheckKeySequenceToShowDetail(HUINT32 ulKeyCode);
static void			local_memoryInfo_UpdateScreenByToggle(MenuHiddenMemInfoContent_t *pstContents);
static ONDK_Result_t	local_memoryInfo_DrawMemLists(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	local_memoryInfo_SetUiObject(MenuHiddenMemInfoContent_t *pstContents);
static ONDK_Result_t	local_memoryInfo_Init(MenuHiddenMemInfoContent_t *pstContents);

static ONDK_Result_t	local_memoryInfo_MsgGwmCreate(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_memoryInfo_MsgGwmDestroy(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_memoryInfo_MsgGwmKeyDown(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
//static ONDK_Result_t	local_memoryInfo_MsgGwmTimer(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline MenuHiddenMemInfoContent_t	*local_memoryInfo_GetContents(void)
{
	return &s_stHiddenMemInfoContent;
}

static HBOOL	local_memoryInfo_CheckKeySequenceToShowDetail(HUINT32 ulKeyCode)
{
	switch (ulKeyCode)
	{
		case KEY_0:
			if(s_keyCountForDetail == 0)
			{
				s_keyCountForDetail++;
			}
			else if(s_keyCountForDetail == 1)
			{
				s_keyCountForDetail++;
			}
			else if(s_keyCountForDetail == 2)
			{
				s_keyCountForDetail++;
			}
			else if(s_keyCountForDetail == 3)
			{
				s_keyCountForDetail++;
			}
			else
			{
				s_keyCountForDetail = 0;
			}
			break;

		case KEY_5:
			if(s_keyCountForDetail == 4)
			{
				s_keyCountForDetail = 0;
				return TRUE;
			}
			else
			{
				s_keyCountForDetail = 0;
			}
			break;

		default:
			s_keyCountForDetail = 0;
			break;
	}
	return FALSE;
}

static void			local_memoryInfo_UpdateScreenByToggle(MenuHiddenMemInfoContent_t *pstContents)
{
	ONDK_Rect_t 		stRect;
	if (!pstContents->bShowDetail)
	{
		pstContents->bShowDetail = TRUE;
		ONDK_GWM_COM_Rect_ChangeSize(HIDDEN_MEMINFO_FRAME_ID,
						HIDDEN_MEMINFO_FRAME_X, HIDDEN_MEMINFO_FRAME_Y,
						HIDDEN_MEMINFO_FRAME_W, HIDDEN_MEMINFO_FRAME_H_FULL);
		stRect = ONDK_Rect(HIDDEN_MEMINFO_CATEGORY_LIST_X, HIDDEN_MEMINFO_CATEGORY_LIST_Y,
							HIDDEN_MEMINFO_CATEGORY_LIST_W, HIDDEN_MEMINFO_CATEGORY_LIST_H);
		ONDK_GWM_COM_Frame_Create(HIDDEN_MEMINFO_CATEGORY_LIST_ID, &stRect, NULL);
		ONDK_GWM_APP_SetObjectDrawMethod(HIDDEN_MEMINFO_CATEGORY_LIST_ID, local_memoryInfo_DrawMemLists);
	}
	else
	{
		pstContents->bShowDetail = FALSE;
		ONDK_GWM_Obj_Destroy(HIDDEN_MEMINFO_CATEGORY_LIST_ID);
		ONDK_GWM_COM_Rect_ChangeSize(HIDDEN_MEMINFO_FRAME_ID,
						HIDDEN_MEMINFO_FRAME_X, HIDDEN_MEMINFO_FRAME_Y,
						HIDDEN_MEMINFO_FRAME_W, HIDDEN_MEMINFO_FRAME_H);

	}
}

static	ONDK_Result_t	local_memoryInfo_DrawMemLists(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
													ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32				w = 0, lItemValueY = 0;
	HUINT32				ulFontSize = 12, i = 0;
	ONDK_Rect_t 		stRect, stRectText;
	HCHAR				szMemInfoMemValue[32] = {0,};
	HCHAR				szMemInfoCPUValue[32] = {0,};
	ONDKFontHandle		*hFontHandle = NULL;
	MenuHiddenMemInfoContent_t	*pstContents = NULL;

	pstContents = local_memoryInfo_GetContents();
	if (pstContents == NULL)
	{
		ONDK_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	if (pstContents->pstSystemMemInfo == NULL)
	{
		ONDK_Error("Error!!! (pstContents->pstSystemMemInfo == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(szMemInfoMemValue, 0x00, 32);
	szMemInfoMemValue[0] = '\0';

	HxSTD_MemSet(szMemInfoCPUValue, 0x00, 32);
	szMemInfoCPUValue[0] = '\0';

	ONDK_Debug("area x/y : [%d/%d] \n", area->x1, area->y1);
	ONDK_Debug("rect x/y/w/h : [%d/%d/%d/%d] \n", rect->x, rect->y, rect->w, rect->h);

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	// Content Category Title
	ulFontSize = 18;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		//Category - Name
		stRectText = ONDK_Rect(stRect.x, stRect.y, HIDDEN_MEMINFO_CATEGORY_NAME_W, HIDDEN_MEMINFO_CATEGORY_TITLE_H);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)"NAME", stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

		//Category - MEM
		w = stRectText.w + HIDDEN_MEMINFO_CATEGORY_ITEM_MG;
		stRectText = ONDK_Rect(stRectText.x + w, stRectText.y, HIDDEN_MEMINFO_CATEGORY_MEM_W, HIDDEN_MEMINFO_CATEGORY_TITLE_H);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)"MEM", stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

		//Category - CPU
		w = stRectText.w + HIDDEN_MEMINFO_CATEGORY_ITEM_MG;
		stRectText = ONDK_Rect(stRectText.x + w, stRectText.y, HIDDEN_MEMINFO_CATEGORY_CPU_W, HIDDEN_MEMINFO_CATEGORY_TITLE_H);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)"CPU", stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

		lItemValueY = stRectText.y + stRectText.h;
		ONDK_FONT_Release(hFontHandle);
	}

	// Content Category Value
	ulFontSize = 20;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ulFontSize);
	if (hFontHandle)
	{
		lItemValueY += HIDDEN_MEMINFO_CATEGORY_VALUE_TAB;
		for (i = 0; i < MAX_MONITOR_PROCESS_NUM; i++)
		{
			//Category - Name
			stRectText = ONDK_Rect(stRect.x, lItemValueY,
								HIDDEN_MEMINFO_CATEGORY_NAME_W, HIDDEN_MEMINFO_CATEGORY_VALUE_H);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pstContents->pstSystemMemInfo->stProcess[i].szProcessName,
											stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

			//Category - MEM
			w = stRectText.w + HIDDEN_MEMINFO_CATEGORY_ITEM_MG;
			stRectText = ONDK_Rect(stRectText.x + w, stRectText.y, HIDDEN_MEMINFO_CATEGORY_MEM_W, stRectText.h);
			HxSTD_snprintf((HCHAR*)szMemInfoMemValue, 32, (HCHAR*)"%d MB [%d%%]",
							pstContents->pstSystemMemInfo->stProcess[i].unUsedMemSizeMB, pstContents->pstSystemMemInfo->stProcess[i].unMemRate);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)szMemInfoMemValue, stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

			//Category - CPU
			w = stRectText.w + HIDDEN_MEMINFO_CATEGORY_ITEM_MG;
			stRectText = ONDK_Rect(stRectText.x + w, stRectText.y, HIDDEN_MEMINFO_CATEGORY_CPU_W, stRectText.h);
			HxSTD_snprintf((HCHAR*)szMemInfoCPUValue, 32, (HCHAR*)"%d%%",	pstContents->pstSystemMemInfo->stProcess[i].unCpuRate);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)szMemInfoCPUValue, stRectText.x, stRectText.y, stRectText.w, 0xffffffff, ONDKSTF_LEFT);

			HxSTD_MemSet(szMemInfoMemValue, 0x00, 32);
			szMemInfoMemValue[0] = '\0';

			HxSTD_MemSet(szMemInfoCPUValue, 0x00, 32);
			szMemInfoCPUValue[0] = '\0';

			lItemValueY = stRectText.y + stRectText.h + HIDDEN_MEMINFO_CATEGORY_VALUE_LS;
		}
		ONDK_FONT_Release(hFontHandle);
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	local_memoryInfo_SetUiObject(MenuHiddenMemInfoContent_t *pstContents)
{
	ONDK_Rect_t	stRect;

	// Draw - Background
	stRect = ONDK_Rect(HIDDEN_MEMINFO_FRAME_X, HIDDEN_MEMINFO_FRAME_Y,
						HIDDEN_MEMINFO_FRAME_W, HIDDEN_MEMINFO_FRAME_H);
	ONDK_GWM_COM_Rect_Create(HIDDEN_MEMINFO_FRAME_ID, &stRect, 0x804C4C4C);

	// Draw - Title
	stRect = ONDK_Rect(HIDDEN_MEMINFO_TITLE_X, HIDDEN_MEMINFO_TITLE_Y,
						HIDDEN_MEMINFO_TITLE_W, HIDDEN_MEMINFO_TITLE_H);
	ONDK_GWM_COM_Text_Create(HIDDEN_MEMINFO_TITLE_ID, &stRect, "Memory Usage");
	ONDK_GWM_COM_Text_SetFont(HIDDEN_MEMINFO_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(HIDDEN_MEMINFO_TITLE_ID, 28);
	ONDK_GWM_COM_Text_SetAlign(HIDDEN_MEMINFO_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(HIDDEN_MEMINFO_TITLE_ID, 0xffe6e6e6, 0xff000000);

	// Draw - Total Memory
	stRect = ONDK_Rect(HIDDEN_MEMINFO_TOTAL_CPU_X, HIDDEN_MEMINFO_TOTAL_CPU_Y,
						HIDDEN_MEMINFO_TOTAL_CPU_W, HIDDEN_MEMINFO_TOTAL_CPU_H);
	ONDK_GWM_COM_Text_Create(HIDDEN_MEMINFO_TOTAL_CPU_ID, &stRect, (char*)pstContents->szTotalCPU);
	ONDK_GWM_COM_Text_SetFont(HIDDEN_MEMINFO_TOTAL_CPU_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(HIDDEN_MEMINFO_TOTAL_CPU_ID, 22);
	ONDK_GWM_COM_Text_SetAlign(HIDDEN_MEMINFO_TOTAL_CPU_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(HIDDEN_MEMINFO_TOTAL_CPU_ID, 0xffffff00, 0xff000000);

	stRect = ONDK_Rect(HIDDEN_MEMINFO_TOTAL_MEM_X, HIDDEN_MEMINFO_TOTAL_MEM_Y,
						HIDDEN_MEMINFO_TOTAL_MEM_W, HIDDEN_MEMINFO_TOTAL_MEM_H);
	ONDK_GWM_COM_Text_Create(HIDDEN_MEMINFO_TOTAL_MEM_ID, &stRect, (char*)pstContents->szTotalMemory);
	ONDK_GWM_COM_Text_SetFont(HIDDEN_MEMINFO_TOTAL_MEM_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(HIDDEN_MEMINFO_TOTAL_MEM_ID, 22);
	ONDK_GWM_COM_Text_SetAlign(HIDDEN_MEMINFO_TOTAL_MEM_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(HIDDEN_MEMINFO_TOTAL_MEM_ID, 0xffffff00, 0xff000000);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	local_memoryInfo_Init(MenuHiddenMemInfoContent_t *pstContents)
{
	pstContents->bShowDetail = FALSE;
	HxSTD_MemSet(pstContents, 0x00, sizeof(MenuHiddenMemInfoContent_t));
	HxSTD_MemSet(pstContents->szTotalCPU, 0x00, 128);
	HxSTD_MemSet(pstContents->szTotalMemory, 0x00, 128);
	pstContents->szTotalCPU[0] = '\0';
	pstContents->szTotalMemory[0] = '\0';

	if (pstContents->pstSystemMemInfo == NULL)
	{
		pstContents->pstSystemMemInfo = ONDK_Calloc(sizeof(HSystemMointorInfo_t));
		if (pstContents->pstSystemMemInfo == NULL)
		{
			ONDK_Error("Error!!! ONDK_Calloc() Fail!! \n");
			return	ONDK_RET_INITFAIL;
		}
	}

	HxSTD_snprintf((HCHAR*)pstContents->szTotalCPU, 128, (HCHAR*)"CPU");
	HxSTD_snprintf((HCHAR*)pstContents->szTotalMemory, 128, (HCHAR*)"Memory");
	pstContents->szTotalCPU[128 - 1] = '\0';
	pstContents->szTotalMemory[128 - 1] = '\0';

	return	ONDK_RET_OK;
}


static void	local_memoryIfno_monitoringCallback(HSystemMointorInfo_t *pMemInfo)
{
	MenuHiddenMemInfoContent_t *pstContents = local_memoryInfo_GetContents();
	if (pMemInfo != NULL)
	{
		HxSTD_memcpy(pstContents->pstSystemMemInfo, pMemInfo, sizeof(HSystemMointorInfo_t));
		ONDK_Print("CPU Usage : %3d%%\nMemory Usage : %3d%% [Free : %d MB]\n",
						pstContents->pstSystemMemInfo->unCpuRate, pstContents->pstSystemMemInfo->unMemRate, pstContents->pstSystemMemInfo->unMemFreeSizeMB);
		HxSTD_snprintf((HCHAR*)pstContents->szTotalCPU, 128, (HCHAR*)"CPU             %d%%", pstContents->pstSystemMemInfo->unCpuRate);
		HxSTD_snprintf((HCHAR*)pstContents->szTotalMemory, 128, (HCHAR*)"Memory      %d%% [Free : %dMB]", pstContents->pstSystemMemInfo->unMemRate, pstContents->pstSystemMemInfo->unMemFreeSizeMB);
		pstContents->szTotalCPU[128 - 1] = '\0';
		pstContents->szTotalMemory[128 - 1] = '\0';
		ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_MemoryInfo_Proc));

		/*
		post ondk thread..
		*/
		ONDK_GWM_PostMessage(AP_MemoryInfo_Proc,MSG_APP_SAVE_SYSTEM_STAUS,HANDLE_NULL,0,0,0);
	}
}


static ONDK_Result_t	local_memoryInfo_MsgGwmCreate(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(HIDDEN_MEMINFO_WINDOW_X, HIDDEN_MEMINFO_WINDOW_Y,
							HIDDEN_MEMINFO_WINDOW_WIDTH, HIDDEN_MEMINFO_WINDOW_HEIGHT);

	eResult = local_memoryInfo_Init(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	local_memoryInfo_SetUiObject(pstContents);

	HAPI_SetSysMonitor(TRUE);
	HAPI_SysMonitorListener(local_memoryIfno_monitoringCallback);

	//ONDK_GWM_SetTimer(HIDDEN_MEMINFO_UPDATE_ID, HIDDEN_MEMINFO_UPDATE_TIME);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	local_memoryInfo_MsgGwmDestroy(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstSystemMemInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSystemMemInfo);
		pstContents->pstSystemMemInfo = NULL;
	}

	//ONDK_GWM_KillTimer(HIDDEN_MEMINFO_UPDATE_ID);

	HxSTD_MemSet(pstContents, 0x00, sizeof(MenuHiddenMemInfoContent_t));

	HAPI_SysMonitorListener(NULL);
	HAPI_SetSysMonitor(FALSE);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static void local_memoryInfo_GetCurrentUnixtime(HCHAR *pBuffer,HUINT32 unBufferLen)
{
	unsigned long UnixTime;
	HxDATETIME_t dateTime;

	if(pBuffer == NULL)
		return;

	HxSTD_MemSet(&dateTime,0,sizeof(HxDATETIME_t));
	UnixTime = HLIB_STD_GetSystemTime();
	HLIB_DATETIME_ConvertUnixTimeToDateTime(UnixTime, &dateTime);

	HxSTD_snprintf(pBuffer,unBufferLen,"%d-%d-%d %d:%d:%d",dateTime.stDate.usYear,dateTime.stDate.ucMonth,dateTime.stDate.ucDay,
		dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond);
}

static void local_memoryInfo_save_systemInfo(const HCHAR *pszDirPath,HCHAR *pCurUnixTime,HSystemMointorInfo_t * pstSystemInfo)
{
	HCHAR		szFilePath[HxPATH_MAX]={0,};
	HxFILE_t	hFile;
	HINT32 		nStream = 0;
	HCHAR		*pBuffer;

	HxSTD_snprintf(szFilePath,sizeof(szFilePath),"%s/%s.csv",pszDirPath,CONFIG_PRODUCT_PLATFORM_NAME);
	if(HLIB_FILE_Exist(szFilePath) == FALSE)
	{
		nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
		HLIB_RWSTREAM_Print(nStream,"TimeStamp(Uptime),TimeStamp(Unixtime),CPU(%%),MEM(%%),FREE(MB)\n");
		pBuffer = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

		hFile = HLIB_FILE_Open(szFilePath,"w+");
		if(hFile)
		{
			HLIB_FILE_Write(hFile,pBuffer,1,HxSTD_StrLen(pBuffer));
			HLIB_FILE_Close(hFile);
		}
		HLIB_MEM_Free(pBuffer);
	}

	hFile = HLIB_FILE_Open(szFilePath,"a");
	if(hFile)
	{
		struct sysinfo info;

		sysinfo(&info);

		nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
		HLIB_RWSTREAM_Print(nStream,"%d,%s,%d,%d,%d\n",info.uptime,pCurUnixTime,
			pstSystemInfo->unCpuRate,pstSystemInfo->unMemRate,pstSystemInfo->unMemFreeSizeMB);
		pBuffer = HLIB_RWSTREAM_CloseWithoutBuf(nStream);
		HLIB_FILE_Write(hFile,pBuffer,1,HxSTD_StrLen(pBuffer));
		HLIB_FILE_Close(hFile);
		HLIB_MEM_Free(pBuffer);
	}
}

static void local_memoryInfo_save_processInfo(const HCHAR *pszDirPath,HCHAR *pCurUnixTime,HSystemMointorInfo_t *pstSystemInfo)
{
	HCHAR		szFilePath[HxPATH_MAX]={0,};
	HxFILE_t	hFile;
	HINT32 		nStream = 0,i;
	HCHAR		*pBuffer;

	HxSTD_snprintf(szFilePath,sizeof(szFilePath),"%s/process.csv",pszDirPath);
	if(HLIB_FILE_Exist(szFilePath) == FALSE)
	{
		nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
		HLIB_RWSTREAM_Print(nStream,"Process,TimeStamp(Uptime),TimeStamp(Unixtime),CPU(%%),MEM(%%),MEM(MB)\n");
		pBuffer = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

		hFile = HLIB_FILE_Open(szFilePath,"w+");
		if(hFile)
		{
			HLIB_FILE_Write(hFile,pBuffer,1,HxSTD_StrLen(pBuffer));
			HLIB_FILE_Close(hFile);
		}
		HLIB_MEM_Free(pBuffer);
	}

	hFile = HLIB_FILE_Open(szFilePath,"a");
	if(hFile)
	{
		struct sysinfo info;
		sysinfo(&info);

		for(i=0; i < HAMA_MAX_MONITOR_PROCESS_NUM ; i++)
		{
			HSystemProcessInfo_t *pstProcessInfo = &pstSystemInfo->stProcess[i];

			nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
			HLIB_RWSTREAM_Print(nStream,"%s,%d,%s,%d,%d,%d\n",pstProcessInfo->szProcessName,info.uptime,pCurUnixTime,
				pstProcessInfo->unCpuRate,pstProcessInfo->unMemRate,pstProcessInfo->unUsedMemSizeMB);
			pBuffer = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

			HLIB_FILE_Write(hFile,pBuffer,1,HxSTD_StrLen(pBuffer));
			HLIB_MEM_Free(pBuffer);
		}

		HLIB_FILE_Close(hFile);
	}
}

static ONDK_Result_t	local_memoryInfo_MsgGwmSaveStatus(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				unListCount = 0;
	DxStorage_Info_t 		*pDevinfoList = NULL;
	HCHAR					szMountPath[HxPATH_MAX]={0,};
	HCHAR					szDirPath[HxPATH_MAX]={0,};
	HCHAR					szCurTime[32] = {0,};
	HSystemMointorInfo_t	*pstSystemMemInfo;
	HINT32 					i;

	if (pstContents->pstSystemMemInfo == NULL )
		return 	ONDK_RET_OK;

	pstSystemMemInfo = pstContents->pstSystemMemInfo;
	if(APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_Mem,&pDevinfoList,&unListCount) == ERR_OK)
	{
		for(i=0 ; i < unListCount; i++)
		{
			if(pDevinfoList[i].ulFlags | eDxSTORAGE_FLAG_CONNECTED && pDevinfoList[i].ulFlags | eDxSTORAGE_FLAG_MOUNTED)
			{
				HxSTD_StrNCpy(szMountPath,pDevinfoList[i].szMountPath,sizeof(szMountPath));
				break;
			}
		}
		APK_FILEMGR_STORAGE_FreeList(pDevinfoList,unListCount);
	}

	if(HxSTD_StrEmpty(szMountPath) == TRUE)
	{
		return 	ONDK_RET_OK;
	}

	HxSTD_snprintf(szDirPath,sizeof(szDirPath),"%s/%s",szMountPath,CONFIG_PRODUCT_PLATFORM_NAME);
	if(HLIB_DIR_IsExist(szDirPath) == FALSE)
	{
		HLIB_DIR_Make(szDirPath);
	}

	local_memoryInfo_GetCurrentUnixtime(szCurTime,sizeof(szCurTime));
	local_memoryInfo_save_systemInfo(szDirPath,szCurTime,pstSystemMemInfo);
	local_memoryInfo_save_processInfo(szDirPath,szCurTime,pstSystemMemInfo);

	return 	ONDK_RET_OK;
}
static ONDK_Result_t	local_memoryInfo_MsgGwmKeyDown(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		ONDK_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	if (local_memoryInfo_CheckKeySequenceToShowDetail(lKeyId) == TRUE)
	{
		local_memoryInfo_UpdateScreenByToggle(pstContents);
	}
	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			ONDK_Debug("Not consumed except KEY_STANDBY \n");
			return	ONDK_MESSAGE_PASS;
	}
}

#if 0
static ONDK_Result_t	local_memoryInfo_MsgGwmTimer(MenuHiddenMemInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HSystemMointorInfo_t	*pstMemInfo = NULL;

	if (p1 == HIDDEN_MEMINFO_UPDATE_ID)
	{
		//pstMemInfo = NX_SYSTEM_GetmemoryInfo();
		if (pstMemInfo != NULL)
		{
			HxSTD_memset(pstContents->pstSystemMemInfo, 0x00, sizeof(HSystemMointorInfo_t));
			HxSTD_memcpy(pstContents->pstSystemMemInfo, pstMemInfo, sizeof(HSystemMointorInfo_t));
			HxSTD_snprintf((HCHAR*)pstContents->szMemInfoTitle, 128, (HCHAR*)"CPU Usage : %3d%%, Memory Usage : %3d%% [Free : %d MB]",
							pstContents->pstSystemMemInfo->unCpuRate, pstContents->pstSystemMemInfo->unMemRate, pstContents->pstSystemMemInfo->unMemFreeSizeMB);
			pstContents->szMemInfoTitle[128 - 1] = '\0';

			ONDK_GWM_APP_InvalidateObject(HIDDEN_MEMINFO_CATEGORY_LIST_ID);
		}
	}

	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}
#endif

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	AP_MemoryInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	MenuHiddenMemInfoContent_t	*pstContents = NULL;

	pstContents = local_memoryInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = local_memoryInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			//eResult = local_memoryInfo_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = local_memoryInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = local_memoryInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SAVE_SYSTEM_STAUS:
			eResult = local_memoryInfo_MsgGwmSaveStatus(pstContents, p1, p2, p3);
			break;

		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
