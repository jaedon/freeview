/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <ondk.h>
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define MAX_CHANNELLIST_OBJECT					128

#define ONDK_CHLIST_ITEM_NUM					10
#define ONDK_CHLIST_ITEM_W						490
#if defined(CONFIG_OP_MBC)
	#define ONDK_CHLIST_LS						45
	#define ONDK_CHLIST_W						1189
	#define ONDK_CHLIST_FOCUS_ITEM_H			151
#else
	#define ONDK_CHLIST_LS						48
	#define ONDK_CHLIST_W						510
	#define ONDK_CHLIST_FOCUS_ITEM_H			120
#endif
#define ONDK_CHLIST_FOCUS_H						66
#define ONDK_CHLIST_H							((ONDK_CHLIST_LS * ONDK_CHLIST_ITEM_NUM) + ONDK_CHLIST_FOCUS_ITEM_H)
#define ONDK_CHLIST_FOCUS_ITEM_NO				3
#define ONDK_CHLIST_ITEM_LEFT_MARGIN			60
#define ONDK_CHLIST_ITEM_V_INTERVAL				10

#define ONDK_CHLIST_ITEM_C_TEXT_OFFSET_Y		11
#define ONDK_CHLIST_ITEM_C_NUMBER_X				60
#define ONDK_CHLIST_ITEM_C_NUMBER_W				85

#define ONDK_CHLIST_ITEM_C_NAME_X				171
#define ONDK_CHLIST_ITEM_C_NAME_W				304

#define ONDK_CHLIST_ITEM_C_EVENT_OFFSET_Y		(ONDK_CHLIST_FOCUS_H + 16)
#define ONDK_CHLIST_ITEM_C_EVENT_X				60
#define ONDK_CHLIST_ITEM_C_EVENT_W				370
#define ONDK_CHLIST_ITEM_C_EVENT_H				(ONDK_FONT_SIZE_24 + 8)

#define ONDK_CHLIST_ITEM_C_EVENT_TIME_OFFSET_Y	(ONDK_CHLIST_FOCUS_H + ONDK_CHLIST_ITEM_C_EVENT_H + 16 + 8)
#define ONDK_CHLIST_ITEM_C_EVENT_TIME_X			60
#define ONDK_CHLIST_ITEM_C_EVENT_TIME_W			430

#define ONDK_CHLIST_ITEM_C_ICON_OFFSET_Y		ONDK_CHLIST_ITEM_C_EVENT_OFFSET_Y
#define ONDK_CHLIST_ITEM_C_ICON_X				460
#define ONDK_CHLIST_ITEM_C_ICON_H				28
#define ONDK_CHLIST_ITEM_C_ICON_CAS_W			36
#define ONDK_CHLIST_ITEM_C_ICON_LOCK_W			22

#define ONDK_CHLIST_ITEM_NUMBER_X				60
#define ONDK_CHLIST_ITEM_NUMBER_W				68

#define ONDK_CHLIST_ITEM_NAME_X					144
#define ONDK_CHLIST_ITEM_NAME_W					346

#define ONDK_CHLIST_ITEM_TEXT_BUFFER			256
#define ONDK_CHLIST_EVENT_TEXT_BUFFER			128
#define ONDK_CHLIST_EVENT_DATE_TEXT_BUFFER		64

#define ONDK_CHLIST_ITEM_COLOR_T_GREY_02		0xffd2d2d2 	//C_T_Gong02,
#define ONDK_CHLIST_ITEM_COLOR_T_GREY_03		0xffbebebe 	//C_T_Gong03,
#define ONDK_CHLIST_ITEM_COLOR_T_WHITE_100		0xffffffff	//C_T_Whistle_100,
#define ONDK_CHLIST_ITEM_COLOR_T_BLACK_100		0xff000000 	//C_T_Bang_100,
#define ONDK_CHLIST_ITEM_COLOR_G_BLUE_01_100	0xff2eb2ff	//C_G_Base01_100,


/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

struct tagChannelListObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	void	*userdata;
	HINT32	lUserDataCount;

	HCHAR	*blankImg;
	HCHAR	*fillImg;
	HCHAR	*backgroundImg;

	ONDKImageHandle	hBlankImg;
	ONDKImageHandle	hFillImg;
	ONDKImageHandle	hBackgroundImg;

	// my kim
	HINT32	item_col_h;
	HUINT32	startTime, duration;

	HCHAR	*casImgPath;
	ONDKImageHandle	hCasImg;

	HCHAR	*szLockImgPath;
	ONDKImageHandle hLockImg;
#if defined(CONFIG_PROD_LANG_ARABIC)
	HINT32	language;
#endif
};

typedef struct
{
	HINT32	lcn;
	HCHAR 	*pszChName;
	HCHAR 	pszEventName[ONDK_CHLIST_EVENT_TEXT_BUFFER];
	HCHAR 	pszEventDate[ONDK_CHLIST_EVENT_DATE_TEXT_BUFFER];
}ONDKChListData_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static struct tagChannelListObject	s_defChannelList;


/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */
static CHANNEL_LIST_OBJECT		local_gwm_AllocNewChannelListObj(void)
{
	struct tagChannelListObject	*pObj = &s_defChannelList;

	pObj = (struct tagChannelListObject *)ONDK_Malloc(sizeof(struct tagChannelListObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defChannelList, sizeof(struct tagChannelListObject));

	return pObj;
}

static void	local_gwmChannelList_FreeObj(CHANNEL_LIST_OBJECT obj)
{
	ONDK_Assert(obj);

	if(obj->hBlankImg)	ONDK_IMG_Destroy(obj->hBlankImg);
	if(obj->hFillImg)	ONDK_IMG_Destroy(obj->hFillImg);
	if(obj->hBackgroundImg)	ONDK_IMG_Destroy(obj->hBackgroundImg);

	ONDK_Free(obj);

	return;
}

static ONDK_Result_t		local_gwm_defaultDrawChannelListMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	HERROR				hRet = ERR_FAIL;
	HINT32				i = 0, hFocusExtH = ONDK_CHLIST_FOCUS_ITEM_H, lIconX = 0, lIconY = 0;
	HINT32				lStartDrawIdx = 0, lDrawCnt = 0;
	//HINT32			eventNamePlusPos = 0;
	//HUINT32			wFont = ONDK_CHLIST_W - ONDK_CHLIST_ITEM_LEFT_MARGIN;
	HCHAR 				szBuf[ONDK_CHLIST_ITEM_TEXT_BUFFER] = {'\0',};
	ONDK_Rect_t 		drawRect, item_rect;
	ONDKFontHandle		*font = NULL, *fontNBold = NULL, *focusFont = NULL, *lcnFont = NULL, *eventNameFont = NULL;
	ONDKChListData_t	*pstChData = NULL;
	HxDATETIME_Time_t 	StartTime, EndTime;
	CHANNEL_LIST_OBJECT	channelList = (CHANNEL_LIST_OBJECT)object;


	ONDK_Assert(screen);
	ONDK_Assert(area);
	ONDK_Assert(rect);
	ONDK_Assert(object);

	// Normal Font
	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_26);
	if (font == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
		ONDK_FONT_Release(font);
		return GWM_RET_FAIL;
	}

	// Normal Bold Font
	fontNBold = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_26);
	if (fontNBold == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
		ONDK_FONT_Release(font);
		ONDK_FONT_Release(fontNBold);
		return GWM_RET_FAIL;
	}

	item_rect.x = channelList->rect.x;
	item_rect.y = channelList->rect.y;
	item_rect.w = ONDK_CHLIST_ITEM_W;
	item_rect.h = channelList->item_col_h;

	if (channelList->userdata)
	{
		pstChData = (ONDKChListData_t*)channelList->userdata;
	}
	else
	{
		pstChData = NULL;
	}

	if (channelList->lUserDataCount < ONDK_CHLIST_FOCUS_ITEM_NO)
	{
		lStartDrawIdx = (ONDK_CHLIST_FOCUS_ITEM_NO - channelList->lUserDataCount);
	}

	lDrawCnt = 0;
	for (i = 0; i < ONDK_CHLIST_ITEM_NUM; i++)
	{
		if (i < lStartDrawIdx)
		{
			item_rect.y += channelList->item_col_h;
			continue;
		}

		// 채널리스트가 리스트 아이템보다 작으면 처리안함.
		if (lDrawCnt >= channelList->lUserDataCount)
			break;

		if (i == (ONDK_CHLIST_FOCUS_ITEM_NO - 1))	// Draw Focus
		{
			lcnFont	= ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_36);
			if (lcnFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}
			focusFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_36);
			if (focusFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(focusFont);
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}
			eventNameFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_24);
			if (eventNameFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(focusFont);
				ONDK_FONT_Release(eventNameFont);
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}

			// focus item height 적용.
			item_rect.h += hFocusExtH;
			drawRect = ONDK_Rect(item_rect.x, item_rect.y, item_rect.w, ONDK_CHLIST_FOCUS_H);
			ONDK_DRAW_FillRect(screen, drawRect, ONDK_CHLIST_ITEM_COLOR_G_BLUE_01_100);

			if (pstChData != NULL)
			{
				// Focus Channel Number
				szBuf[0] = '\0';
				HxSTD_sprintf(szBuf, "%04d", pstChData[lDrawCnt].lcn);
				ONDK_FONT_DrawStringAlignAbbr(screen, lcnFont, szBuf, item_rect.x + ONDK_CHLIST_ITEM_C_NUMBER_X,
									item_rect.y + ONDK_CHLIST_ITEM_C_TEXT_OFFSET_Y, ONDK_CHLIST_ITEM_C_NUMBER_W,
									ONDK_CHLIST_ITEM_COLOR_T_BLACK_100, ONDKSTF_TOPLEFT);

				// Focus Channel Name
				szBuf[0] = '\0';
				if(pstChData[lDrawCnt].pszChName != NULL)
					HxSTD_sprintf(szBuf, "%s", pstChData[lDrawCnt].pszChName);
				else
					HxSTD_sprintf(szBuf, " ");
				ONDK_FONT_DrawStringAlignAbbr(screen, focusFont, szBuf,item_rect.x + ONDK_CHLIST_ITEM_C_NAME_X,
									item_rect.y + ONDK_CHLIST_ITEM_C_TEXT_OFFSET_Y, ONDK_CHLIST_ITEM_C_NAME_W,
									ONDK_CHLIST_ITEM_COLOR_T_BLACK_100, ONDKSTF_TOPLEFT);

				// Focus Channel Event Name
				if (pstChData[lDrawCnt].pszEventName && (HxSTD_StrLen(pstChData[lDrawCnt].pszEventName) > 0))
				{
					ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, pstChData[lDrawCnt].pszEventName, item_rect.x + ONDK_CHLIST_ITEM_C_EVENT_X,
							item_rect.y + ONDK_CHLIST_ITEM_C_EVENT_OFFSET_Y, ONDK_CHLIST_ITEM_C_EVENT_W, ONDK_CHLIST_ITEM_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
				}
				else
				{	// 없으면 clear
					HxLOG_Debug("This is Invalid Event!!!\n");
					//ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, "UNKNOWN",	item_rect.x + eventNamePlusPos, item_rect.y + 76, (wFont - eventNamePlusPos), ONDK_CHLIST_ITEM_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
				}

				lIconX = item_rect.x + ONDK_CHLIST_ITEM_C_EVENT_X + ONDK_CHLIST_ITEM_C_EVENT_W;
				lIconY = item_rect.y + ONDK_CHLIST_ITEM_C_ICON_OFFSET_Y;
				// set cas image
				if(channelList->hCasImg)
				{
					drawRect = ONDK_Rect(lIconX, lIconY, ONDK_CHLIST_ITEM_C_ICON_CAS_W, ONDK_CHLIST_ITEM_C_ICON_H);
					ONDK_IMG_StretchBlit(channelList->hCasImg, screen, drawRect);
					lIconX += ONDK_CHLIST_ITEM_C_ICON_CAS_W;
				}

				// set Lock image
				if(channelList->hLockImg)
				{
					drawRect = ONDK_Rect(lIconX, lIconY, ONDK_CHLIST_ITEM_C_ICON_LOCK_W, ONDK_CHLIST_ITEM_C_ICON_H);
					ONDK_IMG_StretchBlit(channelList->hLockImg, screen, drawRect);
					lIconX += ONDK_CHLIST_ITEM_C_ICON_CAS_W;
				}

				// Focus Channel Event Time information
				hRet = HLIB_DATETIME_IsValidUnixTime(channelList->startTime);
				if (hRet == ERR_OK)
				{
					HUINT32 ulEndTime = 0;
					HLIB_DATETIME_ConvertUnixTimeToTime(channelList->startTime, &StartTime);
					ulEndTime = channelList->startTime + channelList->duration;
					HLIB_DATETIME_ConvertUnixTimeToTime(ulEndTime, &EndTime);
#if defined(CONFIG_PROD_LANG_ARABIC)
					if (channelList->language == eLangID_Arabic)
					{
						HxSTD_sprintf(szBuf, "(%dmin) %s %02d:%02d-%02d:%02d", (channelList->duration /60), pstChData[lDrawCnt].pszEventDate,
									StartTime.ucHour, StartTime.ucMinute, EndTime.ucHour, EndTime.ucMinute);
					}
					else
					{
						HxSTD_sprintf(szBuf, "%02d:%02d-%02d:%02d %s (%dmin)", StartTime.ucHour, StartTime.ucMinute,
									EndTime.ucHour, EndTime.ucMinute, pstChData[lDrawCnt].pszEventDate, (channelList->duration /60));
					}
#else
						HxSTD_sprintf(szBuf, "%02d:%02d-%02d:%02d %s (%dmin)", StartTime.ucHour, StartTime.ucMinute,
									EndTime.ucHour, EndTime.ucMinute, pstChData[lDrawCnt].pszEventDate, (channelList->duration /60));

#endif
					ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, szBuf,	item_rect.x + ONDK_CHLIST_ITEM_C_EVENT_TIME_X,
							item_rect.y + ONDK_CHLIST_ITEM_C_EVENT_TIME_OFFSET_Y, ONDK_CHLIST_ITEM_C_EVENT_TIME_W, ONDK_CHLIST_ITEM_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
				}
				else
				{
					HxLOG_Debug("This is Invalid Time!!!\n");
				}
			}

			item_rect.y += item_rect.h;
			item_rect.h = channelList->item_col_h;

			ONDK_FONT_Release(lcnFont);
			ONDK_FONT_Release(focusFont);
			ONDK_FONT_Release(eventNameFont);
		}
		else
		{
			if (channelList->userdata)
			{
				if (pstChData != NULL)
				{
					HxLOG_Debug("pstChData : %s , x(%d), y (%d)\n", pstChData[lDrawCnt].pszChName, item_rect.x, item_rect.y);
					szBuf[0] = '\0';
					HxSTD_sprintf(szBuf, "%04d", pstChData[lDrawCnt].lcn);
					ONDK_FONT_DrawStringAlignAbbr(screen, font, szBuf, item_rect.x + ONDK_CHLIST_ITEM_NUMBER_X,
										item_rect.y, ONDK_CHLIST_ITEM_NUMBER_W, ONDK_CHLIST_ITEM_COLOR_T_GREY_03, ONDKSTF_TOPLEFT);

					szBuf[0] = '\0';
					HxSTD_sprintf(szBuf, "%s", pstChData[lDrawCnt].pszChName);
					ONDK_FONT_DrawStringAlignAbbr(screen, fontNBold, szBuf, item_rect.x + ONDK_CHLIST_ITEM_NAME_X,
										item_rect.y, ONDK_CHLIST_ITEM_NAME_W, ONDK_CHLIST_ITEM_COLOR_T_WHITE_100, ONDKSTF_TOPLEFT);
				}
				else
				{
					HxLOG_Debug("\n[%s.%d] user data is NULL \n",__FUNCTION__,__LINE__);
				}
			}
			else
			{
				HxLOG_Debug("\n[%s.%d] user data is NULL \n",__FUNCTION__,__LINE__);
			}
			item_rect.y += channelList->item_col_h;
		}

		lDrawCnt++;
	}
	ONDK_FONT_Release(font);
	ONDK_FONT_Release(fontNBold);

	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(arg);

	return GWM_RET_OK;
}

static ONDK_Result_t	local_gwm_defaultChannelListProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	CHANNEL_LIST_OBJECT	channelList;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
	HBOOL			isFrontMost = FALSE;

	ONDK_Assert(object->type == GUIOBJ_CHANNELLIST);
	channelList = (CHANNEL_LIST_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmChannelList_FreeObj(channelList);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			isFrontMost = (arg > 0) ? TRUE : FALSE;

			if (object->objDrawMethod != NULL)
			{
				object->objDrawMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (CHANNEL_LIST_OBJECT)channelList, arg);
			}
			else
			{
				local_gwm_defaultDrawChannelListMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (CHANNEL_LIST_OBJECT)channelList, arg);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(channelList->rect.x, channelList->rect.y, channelList->rect.w, channelList->rect.h);
			memcpy( &(channelList->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(channelList->rect.x, channelList->rect.y, channelList->rect.w, channelList->rect.h);

			return GWM_MESSAGE_BREAK;
		}
		case MT_CHANGE_TEXT:
		{
			return GWM_MESSAGE_PASS;
		}
		/*
		case MT_KEYDOWN:
		{
			return GWM_MESSAGE_PASS;
		}
		case MT_KEYUP:
		{
			return GWM_MESSAGE_PASS;
		}
		*/
		case MT_GET_FOCUS:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (int)channelList, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(channelList->rect.x, channelList->rect.y, channelList->rect.w, channelList->rect.h);
			break;

		case MT_LOST_FOCUS:
			channelList->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (int)channelList, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(channelList->rect.x, channelList->rect.y, channelList->rect.w, channelList->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}



/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

void			ONDK_GWM_COM_ChannelList_Init(void)
{
	struct tagChannelListObject	*pObj = &s_defChannelList;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagChannelListObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_CHANNELLIST;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwm_defaultChannelListProc;
	pObj->objDrawMethod = NULL;
}


ONDK_Result_t				ONDK_GWM_COM_ChannelList_Create(HINT32 id, HINT32 posX, HINT32 posY)
{
	ONDK_GWM_GuiObject_t		objlist;
	CHANNEL_LIST_OBJECT		new_channel_list;
	ONDK_Result_t			hresult;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_channel_list = (CHANNEL_LIST_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_channel_list != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_channel_list = local_gwm_AllocNewChannelListObj();
	if (new_channel_list == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_channel_list->ID = id;
	new_channel_list->rect.x = posX;
	new_channel_list->rect.y = posY;
	new_channel_list->rect.w = ONDK_CHLIST_W;
	new_channel_list->rect.h = ONDK_CHLIST_H;

	// my kim
	// focus item 을 제외한 나머지 item 의 height.
	new_channel_list->userdata = NULL;
	new_channel_list->item_col_h = ONDK_CHLIST_LS;

	// my kim end
	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_channel_list);
	ONDK_GWM_APP_InvalidateRelativeRect(new_channel_list->rect.x, new_channel_list->rect.y,
								new_channel_list->rect.w, new_channel_list->rect.h);

	return hresult;
}

void	*ONDK_GWM_COM_ChannelList_GetUserdata(HINT32 id)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return NULL;
	}

	return obj->userdata;
}

#if defined(CONFIG_PROD_LANG_ARABIC)
void 	ONDK_GWM_COM_ChannelList_SetUserdata(HINT32 id, void *userdata, HINT32 lUserDataCount, HINT32 lLanguage)
#else
void 	ONDK_GWM_COM_ChannelList_SetUserdata(HINT32 id, void *userdata, HINT32 lUserDataCount)
#endif
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return ;
	}

	obj->userdata = userdata;
	obj->lUserDataCount = lUserDataCount;
#if defined(CONFIG_PROD_LANG_ARABIC)
	obj->language = lLanguage;
#endif
}

HCHAR	*ONDK_GWM_COM_ChannelList_GetBackgroundImg(HINT32 id)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return NULL;
	}

	return obj->backgroundImg;
}

void ONDK_GWM_COM_ChannelList_SetBackgroundImg(HINT32 id, HCHAR	*backgroundImg)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return ;
	}

	obj->backgroundImg = backgroundImg;

	if(obj->hBackgroundImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hBackgroundImg);
		obj->hBackgroundImg = NULL;
	}

	obj->backgroundImg = backgroundImg;
	obj->hBackgroundImg = ONDK_IMG_CreateSelf((HUINT8*)backgroundImg);
}

void ONDK_GWM_COM_ChannelList_SetCasImg(HINT32 id, HCHAR *casImg)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return ;
	}

	obj->casImgPath = casImg;

	if(obj->hCasImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hCasImg);
		obj->hCasImg = NULL;
	}

	if (casImg != NULL)
	{
		obj->casImgPath = casImg;
		obj->hCasImg = ONDK_IMG_CreateSelf((HUINT8*)casImg);
	}
}


void ONDK_GWM_COM_ChannelList_SetLockImg(HINT32 id, HCHAR *szLockImg)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return ;
	}

	obj->szLockImgPath = szLockImg;

	if(obj->hLockImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hLockImg);
		obj->hLockImg = NULL;
	}

	if (szLockImg != NULL)
	{
		obj->szLockImgPath = szLockImg;
		obj->hLockImg = ONDK_IMG_CreateSelf((HUINT8*)szLockImg);
	}
}


void ONDK_GWM_COM_ChannelList_SetProgrammeTime(HINT32 id, HUINT32 startTime, HUINT32 duration)
{
	CHANNEL_LIST_OBJECT		obj;

	obj = (CHANNEL_LIST_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_CHANNELLIST)
	{
		return ;
	}

	obj->startTime	= startTime;
	obj->duration	= duration;
}


/* End of File. ---------------------------------------------------------- */


