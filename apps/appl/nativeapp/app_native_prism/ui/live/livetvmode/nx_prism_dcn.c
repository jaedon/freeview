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
 * @file	  		nx_prism_channel_list.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"

#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_DCN_DIGIT_NUM		4

//////////////////////////////////////
//channel list
//////////////////////////////////////
#define NX_DCN_CHLIST_ID		0x00007000
#define NX_DCN_CHLIST_DIM_ID	(NX_DCN_CHLIST_ID + 1)
#define NX_DCN_CHLIST_LINE1_ID	(NX_DCN_CHLIST_ID + 2)
#define NX_DCN_CHLIST_LINE2_ID	(NX_DCN_CHLIST_ID + 3)
#define NX_DCN_CHLIST_OBJ_ID	(NX_DCN_CHLIST_ID + 4)

#define NX_DCN_FRAME_X		880
#define NX_DCN_FRAME_Y		50
#define NX_DCN_FRAME_W		340
#define NX_DCN_FRAME_H		262
// dim
#define NX_DCN_CHLIST_DIM_X			0
#define NX_DCN_CHLIST_DIM_Y			50
#define NX_DCN_CHLIST_DIM_W			340
#define NX_DCN_CHLIST_DIM_H			12// 212
// line 1
#define NX_DCN_CHLIST_LINE1_X		0
#define NX_DCN_CHLIST_LINE1_Y		50
#define NX_DCN_CHLIST_LINE1_W		340
#define NX_DCN_CHLIST_LINE1_H		1
// line 2
#define NX_DCN_CHLIST_LINE2_X		0
#define NX_DCN_CHLIST_LINE2_Y		(62-1)//NX_DCN_FRAME_H -1
#define NX_DCN_CHLIST_LINE2_W		340
#define NX_DCN_CHLIST_LINE2_H		1
// chlist
#define NX_DCN_CHLIST_OBJ_X			0
#define NX_DCN_CHLIST_OBJ_Y			50
#define NX_DCN_CHLIST_OBJ_W			340
#define NX_DCN_CHLIST_OBJ_H			200
#define NX_DCN_CHLIST_OBJ_ITEM_H	50
#define NX_DCN_CHLIST_OBJ_ITEM_NUM		4

#define NX_DCN_CHLIST_OBJ_ITEM_X	15
#define NX_DCN_CHLIST_OBJ_ITEM_Y	15
#define NX_DCN_CHLIST_OBJ_ITEM_W	55
#define NX_DCN_CHLIST_OBJ_ITEM_LS	50
#define NX_DCN_CHLIST_OBJ_ITEM_NAME_X	77
#define NX_DCN_CHLIST_OBJ_ITEM_NAME_W	248
#define NX_DCN_CHLIST_OBJ_CURSOR_Y	6





//////////////////////////////////////
//dcn input
//////////////////////////////////////
#define NX_DCN_INPUT_ID		0x00009000
#define NX_DCN_INPUT_X		110 //NX_DCN_FRAME_X+110
#define NX_DCN_INPUT_Y		0
#define NX_DCN_INPUT_W		(30*NX_DCN_DIGIT_NUM)
#define NX_DCN_INPUT_H		50


// timer
#define NX_DCN_TIMEOUT_ID			100
#define NX_DCN_TIMEOUT_DURATION		3000



/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HUINT8		aucDcnStr[NX_DCN_DIGIT_NUM + 1];
	NX_Channel_t *pstDcnChList;
	HINT32		nDcnChlistIndex;
	HINT32		nDcnChlistNum;

	Handle_t	hHandlePrev;
	NX_ChListGroup_t	eCurGroupType;
}NxUiDcnContents_t;
/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiDcnContents_t	s_UiDcnContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
STATIC ONDK_Result_t		nx_dcn_ClearDCNNumber(NxUiDcnContents_t *pstContents);
STATIC HUINT8				*nx_dcn_getDCNNumberByStr(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t		nx_dcn_ShiftSetLocNumber(NxUiDcnContents_t *pstContents, HUINT32 digit);
STATIC ONDK_Result_t		nx_dcn_createChList(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t		nx_dcn_destroyChList(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t		nx_dcn_GetStringNumberCount(HUINT8 *pszChannelStrNumber, HUINT32 *pnCount);
STATIC ONDK_Result_t		nx_dcn_ShiftClearLocNumber(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t		nx_dcn_DrawUiSimilarList(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t		nx_dcn_DestroyUiSimilarList(NxUiDcnContents_t *pstContents);


STATIC NxUiDcnContents_t*	nx_dcn_GetContents(void);
STATIC ONDK_Result_t 	nx_dcn_MsgGwmCreate(NxUiDcnContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC ONDK_Result_t	nx_dcn_Destroy(NxUiDcnContents_t *pstContents);
STATIC ONDK_Result_t	nx_dcn_MsgGwmDestroy(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC ONDK_Result_t 	nx_dcn_MsgGwmTimer(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC ONDK_Result_t 	nx_dcn_MsgGwmKeyDown(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR 			nx_dcn_CreateObjects(NxUiDcnContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

STATIC ONDK_Result_t		nx_dcn_ClearDCNNumber(NxUiDcnContents_t *pstContents)
{
	HINT32			nLoop = 0;

	/* set new digit : right to left direction */
	for (nLoop = 0; nLoop < NX_DCN_DIGIT_NUM; nLoop++)
	{
		pstContents->aucDcnStr[nLoop] = '-';
	}
	pstContents->aucDcnStr[NX_DCN_DIGIT_NUM] = '\0';

	return ERR_OK;
}

STATIC HUINT8	*nx_dcn_getDCNNumberByStr(NxUiDcnContents_t *pstContents)
{
	HINT32			nLoop = 0;

	for (nLoop = 0; nLoop < NX_DCN_DIGIT_NUM; nLoop++)
	{
		if(pstContents->aucDcnStr[nLoop] >= '1' && pstContents->aucDcnStr[nLoop] <= '9')
		{
			return &pstContents->aucDcnStr[nLoop];
		}
	}

	return NULL;
}

STATIC ONDK_Result_t		nx_dcn_ShiftSetLocNumber(NxUiDcnContents_t *pstContents, HUINT32 digit)
{
	HINT32			nLoop = 0;

	/* set new digit : right to left direction */
	for (nLoop = 0; nLoop < NX_DCN_DIGIT_NUM - 1; nLoop++)
	{
		pstContents->aucDcnStr[nLoop] = pstContents->aucDcnStr[nLoop + 1];
	}
	pstContents->aucDcnStr[NX_DCN_DIGIT_NUM - 1] = (HUINT8)('0' + (digit));

	return ERR_OK;
}

STATIC ONDK_Result_t		nx_dcn_GetStringNumberCount(HUINT8 *pszChannelStrNumber, HUINT32 *pnCount)
{
	HINT32 nLoop = 0;
	HINT32 nCount = 0;

	for(nLoop = 0; nLoop < NX_DCN_DIGIT_NUM ; nLoop++)
	{
		switch(pszChannelStrNumber[nLoop])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			nCount++;
			break;
		default:
			break;
		}
	}
	*pnCount = nCount;
	return ERR_OK;
}

STATIC ONDK_Result_t		nx_dcn_ShiftClearLocNumber(NxUiDcnContents_t *pstContents)
{
	HINT32			nLoop = 0;

	/* set new digit : right to left direction */
	for (nLoop = (NX_DCN_DIGIT_NUM-2) ; nLoop >= 0; nLoop --)
	{
		pstContents->aucDcnStr[nLoop + 1] = pstContents->aucDcnStr[nLoop];
	}
	pstContents->aucDcnStr[0] = '-' ;

	return ERR_OK;
}

STATIC ONDK_Result_t		nx_dcn_createChList(NxUiDcnContents_t *pstContents)
{
	HERROR hRes = ERR_FAIL;

	nx_dcn_destroyChList(pstContents);

#if 1
	// group only
	hRes = NX_CHANNEL_DCN_MakeGroupNxChannelListByDCN(nx_dcn_getDCNNumberByStr(pstContents), pstContents->eCurGroupType, &pstContents->pstDcnChList, &pstContents->nDcnChlistNum);
#else
	// all
	hRes = NX_CHANNEL_DCN_MakeNxChannelListByDCN(nx_dcn_getDCNNumberByStr(pstContents), &pstContents->pstDcnChList, &pstContents->nDcnChlistNum);
#endif
	if(hRes == ERR_OK && pstContents->nDcnChlistNum > 0)
	{
		nx_dcn_DrawUiSimilarList(pstContents);
	}

	return ERR_OK;
}

STATIC ONDK_Result_t		nx_dcn_destroyChList(NxUiDcnContents_t *pstContents)
{
	nx_dcn_DestroyUiSimilarList(pstContents);

	// remove dcn chlist
	pstContents->nDcnChlistNum = 0;
	if(pstContents->pstDcnChList)
	{
		NX_APP_Free(pstContents->pstDcnChList);
		pstContents->pstDcnChList = NULL;
	}

	return ERR_OK;
}

STATIC ONDK_Result_t		nx_dcn_sendResult(NxUiDcnContents_t *pstContents)
{

	if(pstContents->nDcnChlistNum > 0)
	{
		NX_Channel_t *pstChannel = &pstContents->pstDcnChList[pstContents->nDcnChlistIndex];
		ONDK_GWM_PostMessage((ONDK_GWM_Callback_t)pstContents->hHandlePrev, MSG_APP_DCN_INPUT, (Handle_t)NULL, pstChannel->lcn, pstChannel->svcUid, 0);
	}

	return ERR_OK;
}


static ONDK_Result_t	nx_dcn_DrawSimilarChlistAll(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			lNumItems = 0, lStartIdx = 0, lCurrIdx = 0, lSelectedIdx = 0, lNumOfPage = NX_DCN_CHLIST_OBJ_ITEM_NUM;
	HINT32			lStartX = 0, lEndX = 0, lStartY = 0, lEndY = 0, lItemX = 0, lItemY = 0, lItemNamex = 0;
	HINT32			lMaxWidth = 0, lCellHeight = 0, lFirstIdx = 0, lLastIdx = 0, lCount = 0, lMaxNameWidth = 0;
	HUINT8			**pszListStr = NULL;
	ONDKFontHandle	*hFontHandle = NULL, *hFontHandleName = NULL;
	ONDK_Color_t	ulFontColor;
	LIST_OBJECT		pstListObj;
	NX_Channel_t	*pChList;
	HUINT8			aucBufNum[NX_TEXT_SIZE_16];
	HUINT8			aucBufNum2[NX_TEXT_SIZE_16];
	HUINT8			aucBufName[NX_TEXT_SIZE_64];
	HINT32			nDCNNumCnt = 0;

	NOT_USED_PARAM(arg);

	nx_dcn_GetStringNumberCount(nx_dcn_GetContents()->aucDcnStr, &nDCNNumCnt);
	pChList = (NX_Channel_t*)nx_dcn_GetContents()->pstDcnChList;

	lStartX = area->x1 + rect->x;
	lStartY = area->y1 + rect->y;
	lEndX = lStartX + rect->w;
	lEndY = lStartY + rect->h;

	// Get the properties of the list
	pstListObj = (LIST_OBJECT)object;

	ONDK_GWM_List_GetProperty (pstListObj, &lNumItems, &lStartIdx, &lCurrIdx, &lSelectedIdx, &lNumOfPage);
	pszListStr = (HUINT8 **)ONDK_GWM_List_GetArrayPtr(pstListObj);
	if (pszListStr == NULL)
	{
		return	ONDK_RET_INITFAIL;
	}

	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	if (lNumItems > lNumOfPage)
	{
		// 아이템이 4개 이상이므로 스크롤바 또는 화살표 등의 처리가 필요하다.
		if ((lNumItems - lCurrIdx) <= lNumOfPage)
		{
			// current가 last page에 있다. current가 페이지 첫 아이템이 아니다.
			lFirstIdx = (lNumItems - lNumOfPage);
			lLastIdx = (lNumItems - 1);
		}
		else
		{
			// current가 페이지가 첫 아이템이다.
			lFirstIdx = lCurrIdx;
			lLastIdx = (lCurrIdx + lNumOfPage - 1);
		}
	}
	else
	{
		lFirstIdx = 0;
		lLastIdx = (lNumItems - 1);
	}

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_24);
	if (hFontHandle == NULL)
	{
		HxLOG_Error("Font Creatation Failure!!!");
	}
	hFontHandleName = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	if (hFontHandleName == NULL)
	{
		HxLOG_Error("Font Creatation Failure!!!");
	}

	lItemX = lStartX + NX_DCN_CHLIST_OBJ_ITEM_X;
	lItemY = lStartY + NX_DCN_CHLIST_OBJ_ITEM_Y;
	lMaxWidth = NX_DCN_CHLIST_OBJ_ITEM_W;
	lItemNamex =lStartX + NX_DCN_CHLIST_OBJ_ITEM_NAME_X;
	lMaxNameWidth = NX_DCN_CHLIST_OBJ_ITEM_NAME_W;

	for (lCount = 0; (lCount < NX_DCN_CHLIST_OBJ_ITEM_NUM) && ((lFirstIdx + lCount) <= lLastIdx); lCount++)
	{
		DxService_t 	*stSvcInfo = NULL;
		HINT32			nIdx = lFirstIdx + lCount;
		HINT32			nCurItemIdx = ONDK_GWM_List_GetCurrentIndex(NX_DCN_CHLIST_OBJ_ID);
		HBOOL			fFocused = FALSE;

		stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(pChList[nIdx].svcUid);
		if(stSvcInfo == NULL)
		{
			HxLOG_Error("Not found Service uid[%d]!!!", pChList[nIdx].svcUid);
			continue;
		}

		if(nCurItemIdx == nIdx)
		{
			fFocused = TRUE;
		}

		// cursor
		if(fFocused)
		{
			HINT32 nCursorY = lItemY - NX_DCN_CHLIST_OBJ_ITEM_Y + NX_DCN_CHLIST_OBJ_CURSOR_Y;
			ONDK_DRAW_FillRectElement(screen,lStartX, nCursorY, NX_DCN_CHLIST_OBJ_W, NX_DCN_CHLIST_OBJ_ITEM_H, COL(C_G_Base01_100));
		}

		// num
		HxSTD_snprintf((char*)aucBufNum, NX_TEXT_SIZE_16, (char*)"%04d", pChList[nIdx].lcn);
		HxSTD_snprintf((char*)aucBufNum2, NX_TEXT_SIZE_16, (char*)"%d", pChList[nIdx].lcn);
		if(hFontHandle)
		{
			HINT32 nNumCnt = 0;
			HINT32 nXPos = lItemX;
			HINT32 nLCNNumCnt = 0;
			nx_dcn_GetStringNumberCount(aucBufNum2, &nLCNNumCnt);

			for(nNumCnt = 0 ; nNumCnt < NX_DCN_DIGIT_NUM ; nNumCnt++)
			{
				HINT32 	nCharWidth = 0;
				HINT32	nStart = NX_DCN_DIGIT_NUM - nLCNNumCnt;
				HINT32	nEnd = NX_DCN_DIGIT_NUM - nLCNNumCnt + nDCNNumCnt;

				if(nNumCnt >= nStart && nNumCnt < nEnd)
				{
					ulFontColor = COL(C_T_Base01);
				}
				else
				{
					ulFontColor = COL(C_T_Whistle_100);
				}

				if(fFocused)
				{
					ulFontColor = COL(C_T_Bang_100);
				}

				ONDK_FONT_DrawOneCharacter(screen, hFontHandle, (HCHAR*)&aucBufNum[nNumCnt], nXPos, lItemY, ulFontColor, &nCharWidth);
				nXPos += nCharWidth;
			}

			//ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)aucBufNum, lItemX, lItemY, lMaxWidth, ulFontColor, ONDKSTF_LEFT);
		}

		// name
		ulFontColor = (fFocused) ? COL(C_T_Bang_100) : COL(C_T_Whistle_100);
		HxSTD_snprintf((char*)aucBufName, NX_TEXT_SIZE_64, (char*)"%s", stSvcInfo->name);
		if(hFontHandleName)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandleName, (char*)aucBufName, lItemNamex, lItemY, lMaxNameWidth, ulFontColor, ONDKSTF_LEFT);
		}


		lItemY += NX_DCN_CHLIST_OBJ_ITEM_LS;
	}

	if (hFontHandle)
	{
		ONDK_FONT_Release(hFontHandle);
		hFontHandle = NULL;
	}

	if (hFontHandleName)
	{
		ONDK_FONT_Release(hFontHandleName);
		hFontHandleName = NULL;
	}

	NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t		nx_dcn_DrawUiSimilarList(NxUiDcnContents_t *pstContents)
{
	ONDK_Rect_t	stRect;
	HINT32		nPagePerItem = (pstContents->nDcnChlistNum < NX_DCN_CHLIST_OBJ_ITEM_NUM) ? pstContents->nDcnChlistNum : NX_DCN_CHLIST_OBJ_ITEM_NUM;
	HINT32 		nDimH = NX_DCN_CHLIST_OBJ_ITEM_H * nPagePerItem;
	pstContents->nDcnChlistIndex = 0;

	// dim
	stRect = ONDK_Rect(NX_DCN_CHLIST_DIM_X, NX_DCN_CHLIST_DIM_Y, NX_DCN_CHLIST_DIM_W, NX_DCN_CHLIST_DIM_H + nDimH);
	ONDK_GWM_COM_Rect_Create(NX_DCN_CHLIST_DIM_ID, &stRect, COL(C_G_Bangk_75));

	// line 1
	stRect = ONDK_Rect(NX_DCN_CHLIST_LINE1_X, NX_DCN_CHLIST_LINE1_Y, NX_DCN_CHLIST_LINE1_W, NX_DCN_CHLIST_LINE1_H);
	ONDK_GWM_COM_Rect_Create(NX_DCN_CHLIST_LINE1_ID, &stRect, COL(C_G_Whistle_100));

	// line 2
	stRect = ONDK_Rect(NX_DCN_CHLIST_LINE2_X, NX_DCN_CHLIST_LINE2_Y + nDimH, NX_DCN_CHLIST_LINE2_W, NX_DCN_CHLIST_LINE2_H);
	ONDK_GWM_COM_Rect_Create(NX_DCN_CHLIST_LINE2_ID, &stRect, COL(C_G_Whistle_100));

	//ch list obj.
	stRect = ONDK_Rect(NX_DCN_CHLIST_OBJ_X, NX_DCN_CHLIST_OBJ_Y , NX_DCN_CHLIST_OBJ_W, NX_DCN_CHLIST_OBJ_H);
	ONDK_GWM_List_Create(NX_DCN_CHLIST_OBJ_ID, &stRect, "chlist by numeric key", NX_DCN_CHLIST_OBJ_ITEM_H,
		pstContents->nDcnChlistNum, nPagePerItem,(GWM_LIST_ITEMS)pstContents->pstDcnChList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_DCN_CHLIST_OBJ_ID, (GWM_ObjDraw_t)nx_dcn_DrawSimilarChlistAll);
	ONDK_GWM_List_SetCurrentIndex(NX_DCN_CHLIST_OBJ_ID, pstContents->nDcnChlistIndex);
	ONDK_GWM_List_SetCircularMove(NX_DCN_CHLIST_OBJ_ID, FALSE);
	ONDK_GWM_Obj_SetFocus(NX_DCN_CHLIST_OBJ_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t		nx_dcn_DestroyUiSimilarList(NxUiDcnContents_t *pstContents)
{
	ONDK_GWM_Obj_Destroy(NX_DCN_CHLIST_DIM_ID);
	ONDK_GWM_Obj_Destroy(NX_DCN_CHLIST_LINE1_ID);
	ONDK_GWM_Obj_Destroy(NX_DCN_CHLIST_LINE2_ID);
	ONDK_GWM_Obj_Destroy(NX_DCN_CHLIST_OBJ_ID);

	return ONDK_RET_OK;
}



STATIC NxUiDcnContents_t*	nx_dcn_GetContents(void)
{
	return &s_UiDcnContents;
}

STATIC HERROR nx_dcn_CreateObjects(NxUiDcnContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t	stRect;
	nx_dcn_ClearDCNNumber(pstContents);

	// frame
	ONDK_GWM_APP_SetAppArea(NX_DCN_FRAME_X, NX_DCN_FRAME_Y, NX_DCN_FRAME_W, NX_DCN_FRAME_H);

	stRect = ONDK_Rect(NX_DCN_INPUT_X,NX_DCN_INPUT_Y,NX_DCN_INPUT_W,NX_DCN_INPUT_H);
	ONDK_GWM_COM_Input_Create (NX_DCN_INPUT_ID, &stRect, (char *)NULL, NX_DCN_DIGIT_NUM, (HCHAR*)pstContents->aucDcnStr);
	ONDK_GWM_APP_SetObjectDrawMethod (NX_DCN_INPUT_ID, NX_PRISM_COMP_DrawDCNInputButton);
	ONDK_GWM_Obj_SetFocus (NX_DCN_INPUT_ID);

	return ERR_OK;
}


STATIC ONDK_Result_t nx_dcn_MsgGwmCreate(NxUiDcnContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_MemSet(pstContents, 0, sizeof(NxUiDcnContents_t));

	pstContents->hHandlePrev = hAct;
	pstContents->eCurGroupType = p1;

	nx_dcn_CreateObjects(pstContents, hAct, p1, p2, p3);

	return	ONDK_RET_OK;
}

STATIC ONDK_Result_t	nx_dcn_Destroy(NxUiDcnContents_t *pstContents)
{

	ONDK_GWM_KillTimer(NX_DCN_TIMEOUT_ID);

	ONDK_GWM_Obj_Destroy(NX_DCN_INPUT_ID);

	nx_dcn_destroyChList(pstContents);

	return	ONDK_RET_OK;
}

STATIC ONDK_Result_t	nx_dcn_MsgGwmDestroy(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	nx_dcn_Destroy(pstContents);

	return	ONDK_RET_OK;
}


STATIC ONDK_Result_t nx_dcn_MsgGwmKeyDown(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	ret = ONDK_RET_FAIL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32 nKeyId = p1;
	HUINT32 ulNumCnt = 0;
	HINT32	lObjId = -1;

	eResult = ONDK_GWM_SetTimer(NX_DCN_TIMEOUT_ID, NX_DCN_TIMEOUT_DURATION);
	if (eResult == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_DCN_TIMEOUT_ID);
	}

	switch (nKeyId)
	{
	case KEY_GUIDE:
	case KEY_MENU:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_PASS;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		nx_dcn_GetStringNumberCount(pstContents->aucDcnStr, &ulNumCnt);
		if(ulNumCnt >= NX_DCN_DIGIT_NUM)
		{
			nx_dcn_ClearDCNNumber(pstContents);
		}
		nx_dcn_ShiftSetLocNumber(pstContents, nKeyId - KEY_0);
		ONDK_GWM_COM_Input_SetText(NX_DCN_INPUT_ID, NX_DCN_DIGIT_NUM, (HCHAR*)pstContents->aucDcnStr);

		nx_dcn_createChList(pstContents);
		return ONDK_MESSAGE_BREAK;

	case KEY_CH_PLUS:
	case KEY_CH_MINUS:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_PASS;

	case KEY_ARROWUP:
		lObjId = GWM_GetFocusedObjectID();
		if (lObjId == NX_DCN_CHLIST_OBJ_ID)
		{
			pstContents->nDcnChlistIndex--;
			if(pstContents->nDcnChlistIndex < 0)
				pstContents->nDcnChlistIndex = pstContents->nDcnChlistNum -1;
			ONDK_GWM_List_SetCurrentIndex(NX_DCN_CHLIST_OBJ_ID, pstContents->nDcnChlistIndex);
			return	ONDK_MESSAGE_BREAK;
		}
		break;
	case KEY_ARROWDOWN:
		lObjId = GWM_GetFocusedObjectID();
		if (lObjId == NX_DCN_CHLIST_OBJ_ID)
		{
			pstContents->nDcnChlistIndex++;
			if(pstContents->nDcnChlistIndex >= pstContents->nDcnChlistNum)
				pstContents->nDcnChlistIndex = 0;
			ONDK_GWM_List_SetCurrentIndex(NX_DCN_CHLIST_OBJ_ID, pstContents->nDcnChlistIndex);
			return	ONDK_MESSAGE_BREAK;
		}
		break;

	case KEY_OK:
		nx_dcn_sendResult(pstContents);
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_BREAK;

	case KEY_ARROWLEFT:
		nx_dcn_ShiftClearLocNumber(pstContents);
		ONDK_GWM_COM_Input_SetText(NX_DCN_INPUT_ID, NX_DCN_DIGIT_NUM, (HCHAR*)pstContents->aucDcnStr);

		nx_dcn_createChList(pstContents);
		return ONDK_MESSAGE_BREAK;

	case KEY_EXIT:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_PASS;
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_BREAK;
	case KEY_STANDBY:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_PASS;

	case KEY_VFORMAT:
	case KEY_SUBTITLE:
	case KEY_AUDIO:
	case KEY_TEXT:
		NX_PRISM_NotAvailable_Show();
		return	ONDK_MESSAGE_BREAK;

	default:
		return ONDK_MESSAGE_PASS;
	}

	return ret;
}

static ONDK_Result_t	nx_dcn_MsgGwmTimer(NxUiDcnContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch(p1)
	{
		case NX_DCN_TIMEOUT_ID: // timeout exit
			ONDK_GWM_KillTimer(NX_DCN_TIMEOUT_ID);
			nx_dcn_sendResult(pstContents);
			ONDK_GWM_APP_Destroy(0);
			break;

		default:
			break;
	}

	return GWM_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_DCN_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	NxUiDcnContents_t		*pstContents = nx_dcn_GetContents();

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			nx_dcn_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			nx_dcn_MsgGwmKeyDown(pstContents, p3, 0, 0);
			break;

		case MSG_GWM_TIMER:
			lRet = nx_dcn_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			lRet = ONDK_MESSAGE_PASS;
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_dcn_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_DESTROY:
			lRet = nx_dcn_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

