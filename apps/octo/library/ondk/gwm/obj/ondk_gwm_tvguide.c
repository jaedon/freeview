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



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include <ondk_gwm_appobj.h>
#include <ondk_gwm_component.h>


#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_GUIDE_OBJECT						128

#define	TVGUIDE_VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02

#define	ONDK_GUIDE_EPG_PF_NUM					2
#define	ONDK_GUIDE_EPG_NAME_LEN					128
#define	ONDK_GUIDE_QUERY_NUM					8

#define	ONDK_GUIDE_ALL_CELL_ITEM_NUM			8
#define	ONDK_GUIDE_ALL_CELL_FRAME_H				(73*ONDK_GUIDE_ALL_CELL_ITEM_NUM)

#define	ONDK_GUIDE_CHLIST_START_X				100
#define	ONDK_GUIDE_CHLIST_START_Y				83

//  width per 1 hour is 480 ( 1 min -> 8 pixel)
#define	ONDK_GUIDE_CELL_START_X					283
#define	ONDK_GUIDE_CELL_START_Y					82
#define	ONDK_GUIDE_CELL_START_H					73
#define	ONDK_GUIDE_CELL_START_W					7

#define	ONDK_GUIDE_CELL_DIM_L_X					ONDK_GUIDE_CELL_START_X
#define	ONDK_GUIDE_CELL_DIM_L_Y					ONDK_GUIDE_CELL_START_Y
#define	ONDK_GUIDE_CELL_DIM_L_W					60
#define	ONDK_GUIDE_CELL_DIM_L_H					ONDK_GUIDE_ALL_CELL_FRAME_H

#define	ONDK_GUIDE_CELL_DIM_R_X					1200
#define	ONDK_GUIDE_CELL_DIM_R_Y					ONDK_GUIDE_CELL_START_Y
#define	ONDK_GUIDE_CELL_DIM_R_W					60
#define	ONDK_GUIDE_CELL_DIM_R_H					ONDK_GUIDE_ALL_CELL_FRAME_H

#define	ONDK_GUIDE_1HOUR_TO_MILLISECOND			3600
#define	ONDK_W_PER_MIN							8
#define	ONDK_W_PER_HOUR							480

// margin
#define	ONDK_EVENT_NAME_MARGIN					5
#define	ONDK_CELL_MARGIN						2
#define	ONDK_COLUMN_MARGIN						2
#define	ONDK_GUIDE_UI_CURSOR_INNER_LINE			1
#define	ONDK_GUIDE_UI_CURSOR_DURATION_LINE		4

#define	ONDK_GUIDE_UI_EVENT_TIME_START_X		284
#define	ONDK_GUIDE_UI_EVENT_TIME_START_Y		(66 - ONDK_FONT_SIZE_24)
#define	ONDK_GUIDE_UI_EVENT_TIME_START_W		90
#define	ONDK_GUIDE_UI_EVENT_TIME_START_H		(ONDK_FONT_SIZE_24 + 8)

#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_X		ONDK_GUIDE_CHLIST_START_X
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_Y		(66 - ONDK_FONT_SIZE_36)
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_W		300
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_H		(ONDK_FONT_SIZE_36 + 8)

#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_X		(ONDK_GUIDE_CELL_START_X + (90 * ONDK_W_PER_MIN))
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_Y		(66 - ONDK_FONT_SIZE_36)
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_W		300
#define	ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_H		(ONDK_FONT_SIZE_36 + 8)

#define	ONDK_GUIDE_UI_TIME_LINE_SHADOW_W		1
#define	ONDK_GUIDE_UI_TIME_LINE_W				2

#define	ONDK_GUIDE_UI_CHLIST_BG_X				100
#define	ONDK_GUIDE_UI_CHLIST_BG_Y				83
#define	ONDK_GUIDE_UI_CHLIST_BG_W				180
#define	ONDK_GUIDE_UI_CHLIST_BG_H				71

#define	ONDK_GUIDE_UI_CHLIST_NUMBER_X			10
#define	ONDK_GUIDE_UI_CHLIST_NUMBER_Y			(26 - ONDK_FONT_SIZE_20)
#define	ONDK_GUIDE_UI_CHLIST_NUMBER_W			47
#define	ONDK_GUIDE_UI_CHLIST_NUMBER_H			(ONDK_FONT_SIZE_20 + 8)

#define	ONDK_GUIDE_UI_CHLIST_NAME_X				62
#define	ONDK_GUIDE_UI_CHLIST_NAME_Y				(26 - ONDK_FONT_SIZE_20)
#define	ONDK_GUIDE_UI_CHLIST_NAME_W				110
#define	ONDK_GUIDE_UI_CHLIST_NAME_H				(ONDK_FONT_SIZE_20 + 8)

#define	ONDK_GUIDE_UI_EVENT_NAME_X				13
#define	ONDK_GUIDE_UI_EVENT_NAME_Y				(30 - ONDK_FONT_SIZE_22)
#define	ONDK_GUIDE_UI_EVENT_NAME_H				(ONDK_FONT_SIZE_22 + 8)

#define	ONDK_GUIDE_UI_EVENT_TIME_X				13
#define	ONDK_GUIDE_UI_EVENT_TIME_Y				(58 - ONDK_FONT_SIZE_18)
#define	ONDK_GUIDE_UI_EVENT_TIME_H				(ONDK_FONT_SIZE_18 + 8)

#define	ONDK_GUIDE_UI_TEXT_CHNAME_BUFFER		5
#define	ONDK_GUIDE_UI_TEXT_BUFFER				128

#define	ONDK_GUIDE_COLOR_G_BLACK_90				0xe6000000		//C_G_Bang_90
#define	ONDK_GUIDE_COLOR_G_BLACK_80				0xcc000000		//C_G_Bang_80
#define	ONDK_GUIDE_COLOR_G_BLACK_50				0x80000000		//C_G_Bang_50
#define	ONDK_GUIDE_COLOR_G_WHITE_100			0xffffffff		//C_G_Whistle_100
#define	ONDK_GUIDE_COLOR_G_WHITE_10				0x1affffff		//C_G_Whistle_10
#define	ONDK_GUIDE_COLOR_G_BLUE_01_20			0x332eb2ff		//C_G_Base01_20
#define	ONDK_GUIDE_COLOR_G_BLUE_01_100			0xff2eb2ff		//C_G_Base01_100
#define	ONDK_GUIDE_COLOR_G_BLUE_02_100			0xff135c86		//C_G_Base02_100
#define	ONDK_GUIDE_COLOR_G_YELLOW_02			0xfff8ff44		//C_G_Yoyo02
#define	ONDK_GUIDE_COLOR_G_ORANGE_01			0xfff0640f		//C_G_Orgel01

#define	ONDK_GUIDE_COLOR_T_WHITE_100			0xffffffff		//C_T_Whistle_100
#define	ONDK_GUIDE_COLOR_T_WHITE_05				0x0dffffff 		//C_T_Whistle_05
#define	ONDK_GUIDE_COLOR_T_BLACK_100			0xff000000		//C_T_Bang_100
#define	ONDK_GUIDE_COLOR_T_GREY_07				0xff646464		//C_T_Gong07
#define	ONDK_GUIDE_COLOR_T_GREY_06				0xff828282		//C_T_Gong06
#define	ONDK_GUIDE_COLOR_T_GREY_04				0xffaaaaaa		//C_T_Gong04
#define	ONDK_GUIDE_COLOR_T_GREY_03				0xffbebebe		//C_T_Gong03
#define	ONDK_GUIDE_COLOR_T_GREY_02				0xffd2d2d2 		//C_T_Gong02
#define	ONDK_GUIDE_COLOR_T_BLUE_01				0xff2eb2ff		//C_T_Base01
#define	ONDK_GUIDE_COLOR_T_YELLOW_02			0xfff8ff44		//C_T_Yoyo02


/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

typedef struct
{
	HCHAR	pszEventName[ONDK_GUIDE_EPG_NAME_LEN];
	HUINT32	startTime;
	HUINT32 duration;
	HINT32	eventId;
	HINT32	svcUid;
	HCHAR	*pszDuration[32];
	HBOOL	bFocus;
	HBOOL	bNoProgramme;
}ONDK_GuideData_t;


// It was same struct with Nx_GuideChannel_t;
typedef struct tagGuideChannel {
	HUINT32	svcUid;
	HINT32	lcn;
	HINT32	programmeNum;
	HCHAR	*pszChannelName;
	ONDK_GuideData_t *pstGuideData;

	HBOOL	bPfFocus;
} ONDK_GuideChannel_t;


struct tagTvGuideObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.

	void	*userdata;

	HCHAR	*blankImg;
	HCHAR	*fillImg;
	HCHAR	*backgroundImg;

	ONDKImageHandle	hBlankImg;
	ONDKImageHandle	hFillImg;
	ONDKImageHandle	hBackgroundImg;

	// mykim
	HBOOL	bPfProgramme;
	HUINT32	currentTime;
	HUINT32	epgDisStartTime;		// 기준시간.

	HCHAR	*grid1LImg;
	HCHAR	*grid2CImg;
	HCHAR	*grid3RImg;
	HCHAR	*gridDimLImg;
	HCHAR	*gridDimRImg;

	ONDKImageHandle	hGrid1LImg;
	ONDKImageHandle hGrid2CImg;
	ONDKImageHandle hGrid3RImg;
	ONDKImageHandle hGridDimLImg;
	ONDKImageHandle hGridDimRImg;

	HCHAR	*pszNoProgram;
	HCHAR	*pszOnAir;
	HCHAR	*pszNext;

	HUINT32		ulSvcUid;
	// my kim
};

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagTvGuideObject	s_defTvGuide;

static TV_GUIDE_OBJECT		local_gwm_AllocNewTvGuideObj(void)
{
	struct tagTvGuideObject	*pObj = &s_defTvGuide;

	pObj = (struct tagTvGuideObject *)ONDK_Malloc(sizeof(struct tagTvGuideObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defTvGuide, sizeof(struct tagTvGuideObject));

	return pObj;
}

static ONDK_Result_t 	local_gwm_DrawEpgSearchTime(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																			ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	TV_GUIDE_OBJECT		tvGuideObj = (TV_GUIDE_OBJECT)object;
	HxDATETIME_t 		DateTime;
	ONDKFontHandle		*datetimeFont = NULL;
	HCHAR 				tmpTime[32] = {'\0',};
	ONDK_Rect_t 		drawRect;

	/* On-Air View */
	if (tvGuideObj->bPfProgramme)
	{
		drawRect = ONDK_Rect(ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_X, ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_Y,
								ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_W, ONDK_GUIDE_UI_EPG_MODE_TEXT_ONAIR_H);
		datetimeFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_36);
		if (tvGuideObj->pszOnAir)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, datetimeFont, tvGuideObj->pszOnAir, drawRect.x, drawRect.y, drawRect.w,
											ONDK_GUIDE_COLOR_T_YELLOW_02, ONDKSTF_TOPLEFT);
		}

		drawRect = ONDK_Rect(ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_X, ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_Y,
								ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_W, ONDK_GUIDE_UI_EPG_MODE_TEXT_NEXT_H);
		datetimeFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_36);
		if (tvGuideObj->pszNext)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, datetimeFont, tvGuideObj->pszNext, drawRect.x, drawRect.y, drawRect.w,
											ONDK_GUIDE_COLOR_T_WHITE_100, ONDKSTF_TOPLEFT);
		}
		return ONDK_RET_OK;
	}

	/* NEXT View */
	// 이젠 안해도 된다.. nx_prism_guide 에서 구해 내려온다.
	HLIB_DATETIME_ConvertUnixTimeToDateTime(tvGuideObj->epgDisStartTime, &DateTime);
	drawRect = ONDK_Rect(ONDK_GUIDE_UI_EVENT_TIME_START_X, ONDK_GUIDE_UI_EVENT_TIME_START_Y,
							ONDK_GUIDE_UI_EVENT_TIME_START_W, ONDK_GUIDE_UI_EVENT_TIME_START_H);
	datetimeFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_24);
	if (datetimeFont)
	{
		HxSTD_sprintf(tmpTime, "%02d:%02d", DateTime.stTime.ucHour, DateTime.stTime.ucMinute);
		ONDK_FONT_DrawStringAlignAbbr(screen, datetimeFont, tmpTime, drawRect.x, drawRect.y, drawRect.w, ONDK_GUIDE_COLOR_T_BLUE_01, ONDKSTF_TOPLEFT);

		HLIB_DATETIME_ConvertUnixTimeToDateTime((tvGuideObj->epgDisStartTime + ONDK_GUIDE_1HOUR_TO_MILLISECOND), &DateTime);
		HxSTD_sprintf(tmpTime, "%02d:%02d", DateTime.stTime.ucHour, DateTime.stTime.ucMinute);
		ONDK_FONT_DrawStringAlignAbbr(screen, datetimeFont, tmpTime, drawRect.x + ONDK_W_PER_HOUR, drawRect.y, drawRect.w, ONDK_GUIDE_COLOR_T_BLUE_01, ONDKSTF_TOPLEFT);

		ONDK_FONT_Release(datetimeFont);
	}

	NOT_USED_PARAM(area);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(rect);
	NOT_USED_PARAM(param1);

	return ONDK_RET_OK;
}


//#define CONFIG_IMAGE_CURSOR

static void		local_gwm_FillRectEventCell(ONDKSurfaceHandle *screen, ONDK_Rect_t drawRect, TV_GUIDE_OBJECT tvGuideObj)
{
	if (drawRect.w < ONDK_GUIDE_CELL_START_W)
	{
		ONDK_IMG_StretchBlit(tvGuideObj->hGrid2CImg, screen, drawRect);
	}
	else
	{
		ONDK_GUICOM_HBox(screen, drawRect, tvGuideObj->grid1LImg, tvGuideObj->grid2CImg, tvGuideObj->grid3RImg);
	}
}

#if defined(CONFIG_IMAGE_CURSOR)
static ONDKSurfaceHandle s_tvGuideFocusCursor = NULL;
static ONDKSurfaceHandle s_tvGuideTemp = NULL;
#endif

static void		local_gwm_DrawFillRect(ONDKSurfaceHandle *screen, ONDK_Rect_t drawRect, ONDK_Color_t ulColor)
{
#if defined(CONFIG_IMAGE_CURSOR)
	if(NULL == s_tvGuideTemp)
	{
		s_tvGuideTemp = ONDK_SURFACE_Create(1, 1, "ARGB8888");
	}

	ONDK_DRAW_FillCopyRect(s_tvGuideTemp, ONDK_Rect(0,0,1,1), ulColor);
	ONDK_SURFACE_StretchBlit(s_tvGuideTemp, screen, ONDK_Rect(0,0,1,1), drawRect);
#else
	ONDK_DRAW_FillRect(screen, drawRect, ulColor);
#endif
}



static void		local_gwm_FillRectFocus(ONDKSurfaceHandle *screen, ONDK_Rect_t drawRect, HBOOL bNoProgram, HUINT32 ulStartTime, HUINT32 ulDuration, TV_GUIDE_OBJECT tvGuideObj)
{
	HINT32			lDuration_h = 0, lProgressW = 0;
	HUINT32			ulCurTime = 0, ulDiff = 0, ulDiffRef = 0, ulProgress = 0;
	ONDK_Color_t	innerColor = ONDK_GUIDE_COLOR_G_BLACK_50;
	ONDK_Color_t	item_color = ONDK_GUIDE_COLOR_G_BLUE_01_100;
	ONDK_Rect_t		stRect;

	lDuration_h = (bNoProgram == TRUE) ? 0 : ONDK_GUIDE_UI_CURSOR_DURATION_LINE;
#if defined(CONFIG_IMAGE_CURSOR)
	if( NULL == s_tvGuideFocusCursor)
	{
		s_tvGuideFocusCursor =  ONDK_SURFACE_Create(200, ONDK_GUIDE_ALL_CELL_FRAME_H, "ARGB8888");
		// Top Line
		stRect = ONDK_Rect(0, 0, 200, (ONDK_GUIDE_UI_CURSOR_INNER_LINE + lDuration_h));

		// Left Line
		stRect = ONDK_Rect(0, 0, ONDK_GUIDE_UI_CURSOR_INNER_LINE, ONDK_GUIDE_ALL_CELL_FRAME_H);
		ONDK_DRAW_FillCopyRect(s_tvGuideFocusCursor, stRect, innerColor);

		// Right Line
		stRect = ONDK_Rect((200-1), 0, ONDK_GUIDE_UI_CURSOR_INNER_LINE, ONDK_GUIDE_ALL_CELL_FRAME_H);
		ONDK_DRAW_FillCopyRect(s_tvGuideFocusCursor, stRect, innerColor);

		// Bottom Line
		stRect = ONDK_Rect(0, (ONDK_GUIDE_ALL_CELL_FRAME_H-1), ONDK_GUIDE_ALL_CELL_FRAME_H, ONDK_GUIDE_UI_CURSOR_INNER_LINE);
		ONDK_DRAW_FillCopyRect(s_tvGuideFocusCursor, stRect, innerColor);


		stRect = ONDK_Rect(ONDK_GUIDE_UI_CURSOR_INNER_LINE,
							ONDK_GUIDE_UI_CURSOR_INNER_LINE+lDuration_h,
							(200-ONDK_GUIDE_UI_CURSOR_INNER_LINE),
							(ONDK_GUIDE_ALL_CELL_FRAME_H-ONDK_GUIDE_UI_CURSOR_INNER_LINE));
		ONDK_DRAW_FillCopyRect(s_tvGuideFocusCursor, stRect, item_color);

	}
	ONDK_SURFACE_StretchBlit(s_tvGuideFocusCursor, screen, ONDK_Rect(0,0,200,ONDK_GUIDE_ALL_CELL_FRAME_H), drawRect);
#else



	// Top Line
	stRect = ONDK_Rect(drawRect.x, drawRect.y, drawRect.w, (ONDK_GUIDE_UI_CURSOR_INNER_LINE + lDuration_h));
	ONDK_DRAW_FillRect(screen, stRect, innerColor);

	// Left Line
	stRect = ONDK_Rect(drawRect.x, drawRect.y, ONDK_GUIDE_UI_CURSOR_INNER_LINE, drawRect.h);
	ONDK_DRAW_FillRect(screen, stRect, innerColor);

	// Right Line
	stRect = ONDK_Rect(drawRect.x, drawRect.y, (drawRect.w - ONDK_GUIDE_UI_CURSOR_INNER_LINE), drawRect.h);
	ONDK_DRAW_FillRect(screen, stRect, innerColor);

	// Bottom Line
	stRect = ONDK_Rect(drawRect.x, (drawRect.y + drawRect.h - ONDK_GUIDE_UI_CURSOR_INNER_LINE), drawRect.w, ONDK_GUIDE_UI_CURSOR_INNER_LINE);
	ONDK_DRAW_FillRect(screen, stRect, innerColor);

	// Fill color
	stRect = ONDK_Rect((drawRect.x + ONDK_GUIDE_UI_CURSOR_INNER_LINE), (drawRect.y + ONDK_GUIDE_UI_CURSOR_INNER_LINE + lDuration_h),
		(drawRect.w - (ONDK_GUIDE_UI_CURSOR_INNER_LINE*2)), (drawRect.h - (ONDK_GUIDE_UI_CURSOR_INNER_LINE*2) - lDuration_h));
	ONDK_DRAW_FillRect(screen, stRect, item_color);



#endif
	// Fill color


	// Draw Event Progress
	if (bNoProgram == FALSE)
	{
		ulCurTime = tvGuideObj->currentTime;
		ulDiffRef = ulDuration;
		ulDiff = ulCurTime - ulStartTime;
		if(ulDiff > ulDiffRef)
			ulDiff = ulDiffRef;
		ulProgress = (HUINT32)(ulDiff * 100 / ulDiffRef);
		lProgressW = ((drawRect.w - 2) * ulProgress) / 100;
		stRect = ONDK_Rect((drawRect.x + ONDK_GUIDE_UI_CURSOR_INNER_LINE), drawRect.y, lProgressW, ONDK_GUIDE_UI_CURSOR_DURATION_LINE);
		local_gwm_DrawFillRect(screen, stRect, ONDK_GUIDE_COLOR_G_ORANGE_01);
	}

}

static ONDK_Result_t	local_gwm_DrawChannelInform(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object,
											int itemPosY, ONDK_GuideChannel_t *pstGuideChannel)
{
	HCHAR				szChannel[ONDK_GUIDE_UI_TEXT_BUFFER];
	HCHAR				szChNumber[ONDK_GUIDE_UI_TEXT_CHNAME_BUFFER];
	ONDK_Rect_t 		drawRect;
	ONDK_Color_t		fontColor = 0;
	ONDKFontHandle		*hChannelFont = NULL;
	TV_GUIDE_OBJECT		tvGuideObj = (TV_GUIDE_OBJECT)object;

	drawRect = ONDK_Rect(ONDK_GUIDE_UI_CHLIST_BG_X, itemPosY, ONDK_GUIDE_UI_CHLIST_BG_W, ONDK_GUIDE_UI_CHLIST_BG_H);
	//ONDK_IMG_StretchBlit(tvGuideObj->hGrid2CImg, screen, drawRect);
	ONDK_DRAW_FillRectElement(screen, drawRect.x, drawRect.y, drawRect.w, drawRect.h, ONDK_GUIDE_COLOR_G_WHITE_10);

	if (pstGuideChannel != NULL)
	{
		// Current Service
		if (pstGuideChannel->svcUid == tvGuideObj->ulSvcUid)
		{
			ONDK_DRAW_FillRectElement(screen, drawRect.x, drawRect.y, drawRect.w, drawRect.h, ONDK_GUIDE_COLOR_G_BLUE_01_20);
		}

		fontColor = ONDK_GUIDE_COLOR_T_GREY_03;
		hChannelFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_20);
		if (hChannelFont)
		{
			sprintf(szChNumber, "%04d", pstGuideChannel->lcn);
			ONDK_FONT_DrawStringAlignAbbr(screen, hChannelFont, szChNumber, (drawRect.x + ONDK_GUIDE_UI_CHLIST_NUMBER_X),
							(drawRect.y + ONDK_GUIDE_UI_CHLIST_NUMBER_Y), ONDK_GUIDE_UI_CHLIST_NUMBER_W, fontColor, ONDKSTF_TOPLEFT);

			sprintf(szChannel, "%s", pstGuideChannel->pszChannelName);
			ONDK_FONT_DrawStringAlignAbbr(screen, hChannelFont, szChannel, (drawRect.x + ONDK_GUIDE_UI_CHLIST_NAME_X),
							(drawRect.y + (ONDK_GUIDE_UI_CHLIST_BG_H/2) - (ONDK_GUIDE_UI_CHLIST_NAME_H/2)), ONDK_GUIDE_UI_CHLIST_NAME_W, fontColor, ONDKSTF_TOPLEFT);
			ONDK_FONT_Release(hChannelFont);
		}
	}

	NOT_USED_PARAM(rect);
	NOT_USED_PARAM(area);

	return ONDK_RET_OK;
}


static ONDK_Result_t	local_gwm_DrawColumn(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object,
											int itemPosY, ONDK_GuideChannel_t *pstGuideChannel)
{
	HINT32				i = 0;
	HUINT32				cellW = 0, setStartDuration = 0, maxTime = 0;
	ONDK_Rect_t 		drawRect, prevDrawRect;
	ONDK_Color_t		fontColor = 0;
	ONDK_GuideChannel_t *guideChannel = pstGuideChannel;
	ONDKFontHandle		*eventNameFont = NULL, *timeFont = NULL;
	TV_GUIDE_OBJECT		tvGuideObj = (TV_GUIDE_OBJECT)object;

	// 여기서 return 안하는 이유는 ,, 데이터가 하나도 없어도 기본 UI 는 그려주기 위해.
	//if (guideChannel == NULL)
	//	return ONDK_RET_FAIL;

	// draw channel inform
	local_gwm_DrawChannelInform(screen, area, rect, object, itemPosY, pstGuideChannel);

	eventNameFont 	= ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_22);
	timeFont	 	= ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_18);

	prevDrawRect 	= ONDK_Rect(ONDK_GUIDE_CELL_START_X, 0, 0, 0);		// 처음 시작은. 기준시간의 위치.
	maxTime = tvGuideObj->epgDisStartTime + (60 * 60 * 2);

#if 0//defined(CONFIG_DEBUG)
	{
		HxDATETIME_t datetime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(tvGuideObj->epgDisStartTime, &datetime);
		ONDK_Debug("\nepg Display Standard time (%02d:%02d)\n",	datetime.stTime.ucHour, datetime.stTime.ucMinute);
	}
#endif

	if (guideChannel != NULL && guideChannel->programmeNum > 0)
	{
		for (i = 0; i < guideChannel->programmeNum; i++)
		{
			ONDK_GuideData_t *guideData = &guideChannel->pstGuideData[i];
			if (guideData != NULL)
			{
				if (guideData->startTime > maxTime)
				{
					ONDK_Debug("\nPass This Programme data.\n");
					continue;
				}
				else
				if ((guideData->startTime + guideData->duration) < tvGuideObj->epgDisStartTime)
				{
					continue;
				}
				else
				if (guideData->startTime <= tvGuideObj->epgDisStartTime)
				{
					setStartDuration = (guideData->startTime + guideData->duration) - tvGuideObj->epgDisStartTime;
					cellW = (setStartDuration / 60 ) * ONDK_W_PER_MIN;
					if (cellW > 0)
					{
						drawRect = ONDK_Rect(prevDrawRect.x + prevDrawRect.w, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
						local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

						if (guideData->bFocus)
							local_gwm_FillRectFocus(screen, drawRect, guideData->bNoProgramme, guideData->startTime, guideData->duration, tvGuideObj);

						if (guideData->bNoProgramme != TRUE)
						{
							if (guideData->bFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
							else						fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

							ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, guideData->pszEventName, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
															(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);

							if (guideData->bFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
							else						fontColor = ONDK_GUIDE_COLOR_T_GREY_04;

							ONDK_FONT_DrawStringAlignAbbr(screen, timeFont, (HCHAR*)guideData->pszDuration, (drawRect.x + ONDK_GUIDE_UI_EVENT_TIME_X),
															(drawRect.y + ONDK_GUIDE_UI_EVENT_TIME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
						}
						else
						{
							if (guideData->bFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
							else						fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

							ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, guideData->pszEventName, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
															(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
						}

						prevDrawRect = drawRect;
						prevDrawRect.x += ONDK_CELL_MARGIN;
					}
				}
				else // guideData->startTime > tvGuideObj->epgDisStartTime
				{
					if (i == 0)	// if first item...
					{
						// no programme inform 처리를 먼저 한후 데이터를 그림.
						setStartDuration = guideData->startTime - tvGuideObj->epgDisStartTime;
						cellW = (setStartDuration / 60 ) * ONDK_W_PER_MIN;
						if (cellW > 0)
						{
							prevDrawRect.x = ONDK_GUIDE_CELL_START_X;
							drawRect = ONDK_Rect(prevDrawRect.x, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
							if (cellW > 0)
							{
								local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);
								if (tvGuideObj->pszNoProgram)
								{
									ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
																(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, ONDK_GUIDE_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
								}
								//if (guideData->bFocus)
								//	local_gwm_FillRectFocus(screen, drawRect);
							}
							prevDrawRect = drawRect;
							prevDrawRect.x += ONDK_CELL_MARGIN;
						}
					}
					cellW = (guideData->duration / 60 ) * ONDK_W_PER_MIN;
					if (cellW > 0)
					{
						drawRect = ONDK_Rect(prevDrawRect.x + prevDrawRect.w, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
						local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

						if (guideData->bFocus)
							local_gwm_FillRectFocus(screen, drawRect, guideData->bNoProgramme, guideData->startTime, guideData->duration, tvGuideObj);

						if (guideData->bFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
						else						fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

						ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, guideData->pszEventName, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
														(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);

						if (guideData->bFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
						else						fontColor = ONDK_GUIDE_COLOR_T_GREY_04;

						ONDK_FONT_DrawStringAlignAbbr(screen, timeFont, (HCHAR*)guideData->pszDuration, (drawRect.x + ONDK_GUIDE_UI_EVENT_TIME_X),
														(drawRect.y + ONDK_GUIDE_UI_EVENT_TIME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
						prevDrawRect = drawRect;
						prevDrawRect.x += ONDK_CELL_MARGIN;
					}
				}

			}
		}// end for
		if ((prevDrawRect.x + prevDrawRect.w) < 1280)		// 빈공간 채우기.
		{
			drawRect = ONDK_Rect(prevDrawRect.x + prevDrawRect.w, itemPosY, (1280 - (prevDrawRect.x + prevDrawRect.w)), ONDK_GUIDE_CELL_START_H);
			local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);
			if (tvGuideObj->pszNoProgram)
			{
				ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
											(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, ONDK_GUIDE_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
			}
		}
	}
	else	// no programe 도 item 으로 처리되므로 여기로 들어올일이 없다...
	{
		cellW = 120 * ONDK_W_PER_MIN;
		drawRect = ONDK_Rect(ONDK_GUIDE_CELL_START_X, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
		local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);
		if (tvGuideObj->pszNoProgram)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
										(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, ONDK_GUIDE_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
		}
	}

	ONDK_FONT_Release(eventNameFont);
	ONDK_FONT_Release(timeFont);

	return ONDK_RET_OK;
}

static ONDK_Result_t	local_gwm_DrawPFColumn(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object,
											int itemPosY, ONDK_GuideChannel_t *pstGuideChannel)
{
	HINT32				i = 0;
	HUINT32				cellW = 0, maxTime = 0;
	ONDK_Rect_t 		drawRect, prevDrawRect;
	ONDK_Color_t		fontColor = ONDK_GUIDE_COLOR_T_GREY_02;
	TV_GUIDE_OBJECT		tvGuideObj = (TV_GUIDE_OBJECT)object;
	ONDK_GuideChannel_t *guideChannel = pstGuideChannel;
	ONDKFontHandle		*eventNameFont = NULL, *timeFont = NULL;
	//HUINT32 setStartDuration = 0;

//	HUINT32 startPosX = 0, drawDuration =0;


	// 여기서 return 안하는 이유는 ,, 데이터가 하나도 없어도 기본 UI 는 그려주기 위해.
	//if (guideChannel == NULL)
	//	return ONDK_RET_FAIL;

	// draw channel inform
	local_gwm_DrawChannelInform(screen, area, rect, object, itemPosY, pstGuideChannel);

	eventNameFont 	= ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_22);
	timeFont	 	= ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_18);

	prevDrawRect 	= ONDK_Rect(ONDK_GUIDE_CELL_START_X, 0, 0, 0);		// 처음 시작은. 기준시간의 위치.
	maxTime = tvGuideObj->epgDisStartTime + (60 * 60 * 2);

	if (guideChannel != NULL && guideChannel->programmeNum > 0)
	{
		for (i = 0; i < ONDK_GUIDE_EPG_PF_NUM; i++)	// PF 만
		{
			ONDK_GuideData_t *guideData = &guideChannel->pstGuideData[i];
			fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

			if (guideData != NULL)
			{
				if (i == 0)
				{
					if (guideChannel->bPfFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
					else							fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

					cellW = 90 * ONDK_W_PER_MIN;
					drawRect = ONDK_Rect(ONDK_GUIDE_CELL_START_X, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
				}
				else if (i == 1)
				{
					cellW = 30 * ONDK_W_PER_MIN;
					drawRect = ONDK_Rect((ONDK_GUIDE_CELL_START_X + (90 * ONDK_W_PER_MIN) + ONDK_CELL_MARGIN), itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
				}

				local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

				if ( i == 0 && guideChannel->bPfFocus == TRUE)
					local_gwm_FillRectFocus(screen, drawRect, guideData->bNoProgramme, guideData->startTime, guideData->duration, tvGuideObj);

				if (guideData->bNoProgramme != TRUE)
				{
					ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, guideData->pszEventName, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
													(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);

					ONDK_FONT_DrawStringAlignAbbr(screen, timeFont, (HCHAR*)guideData->pszDuration, (drawRect.x + ONDK_GUIDE_UI_EVENT_TIME_X),
													(drawRect.y + ONDK_GUIDE_UI_EVENT_TIME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
				}
				else
				{
					ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, guideData->pszEventName, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
													(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
				}
			}
			else
			{

				if (i == 0)
				{

					if (guideChannel->bPfFocus)		fontColor = ONDK_GUIDE_COLOR_T_BLACK_100;
					else							fontColor = ONDK_GUIDE_COLOR_T_GREY_02;

					cellW = 90 * ONDK_W_PER_MIN;
					drawRect = ONDK_Rect(ONDK_GUIDE_CELL_START_X, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
					local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

					if ( guideChannel->bPfFocus == TRUE)
						local_gwm_FillRectFocus(screen, drawRect, TRUE, 0, 0, tvGuideObj);

					if (tvGuideObj->pszNoProgram)
					{
						ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
														(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
					}
				}
				else if ( i == 1)
				{
					cellW = 30 * ONDK_W_PER_MIN;
					drawRect = ONDK_Rect((ONDK_GUIDE_CELL_START_X + (90 * ONDK_W_PER_MIN) + ONDK_CELL_MARGIN), itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
					local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

					if (tvGuideObj->pszNoProgram)
					{
						ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_GUIDE_UI_EVENT_NAME_X),
														(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW - ONDK_GUIDE_UI_EVENT_NAME_X, fontColor, ONDKSTF_TOPLEFT);
					}
				}
			}
		}// end for
	}
	else		// no programme 도 아이템으로 처리되므로.. 여기로 들어올일이 없다.
	{
		cellW = 90 * ONDK_W_PER_MIN;
		drawRect = ONDK_Rect(ONDK_GUIDE_CELL_START_X, itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
		local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

		if ((guideChannel != NULL) && (guideChannel->bPfFocus == TRUE))
		{
			local_gwm_FillRectFocus(screen, drawRect, TRUE, 0, 0, tvGuideObj);
		}

		if (tvGuideObj->pszNoProgram)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram, (drawRect.x + ONDK_EVENT_NAME_MARGIN),
											(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW, ONDK_GUIDE_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
		}

		cellW = 30 * ONDK_W_PER_MIN;
		drawRect = ONDK_Rect((ONDK_GUIDE_CELL_START_X + (90 * ONDK_W_PER_MIN) + ONDK_CELL_MARGIN), itemPosY, cellW, ONDK_GUIDE_CELL_START_H);
		local_gwm_FillRectEventCell(screen, drawRect, tvGuideObj);

		if (tvGuideObj->pszNoProgram)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, tvGuideObj->pszNoProgram,	(drawRect.x + ONDK_EVENT_NAME_MARGIN),
											(drawRect.y + ONDK_GUIDE_UI_EVENT_NAME_Y), cellW, ONDK_GUIDE_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
		}
	}

	ONDK_FONT_Release(eventNameFont);
	ONDK_FONT_Release(timeFont);

	return ONDK_RET_OK;
}

static ONDK_Result_t	local_gwm_DrawCurrentVerticalLine(ONDKSurfaceHandle *screen, ONDK_GWM_GuiObject_t object)
{
	HUINT32				current = 0, cellW = 0;
	ONDK_Rect_t 		drawRect;
	ONDK_Color_t		item_color = 0, item_Shadowcolor = 0;
	TV_GUIDE_OBJECT		tvGuideObj = (TV_GUIDE_OBJECT)object;

	if (tvGuideObj->currentTime < TVGUIDE_VALID_UNIXTIME)
	{
		ONDK_Debug("time is Invalid!!!\n\n");
		return ONDK_RET_FAIL;
	}
	else if (tvGuideObj->currentTime < (tvGuideObj->epgDisStartTime + (60 * 60 * 2)))
	{
		current = abs(tvGuideObj->currentTime - tvGuideObj->epgDisStartTime);
		cellW = (current / 60) * ONDK_W_PER_MIN;

		// Draw Time Line
		item_color = ONDK_GUIDE_COLOR_G_YELLOW_02;
		drawRect = ONDK_Rect(ONDK_GUIDE_CELL_START_X + cellW, ONDK_GUIDE_CELL_START_Y, ONDK_GUIDE_UI_TIME_LINE_W, ONDK_GUIDE_ALL_CELL_FRAME_H);
		local_gwm_DrawFillRect(screen, drawRect, item_color);

		// Draw Time Line shadow
		item_Shadowcolor = ONDK_GUIDE_COLOR_G_BLACK_50;
		drawRect = ONDK_Rect(drawRect.x + drawRect.w, drawRect.y, ONDK_GUIDE_UI_TIME_LINE_SHADOW_W, drawRect.h);
		local_gwm_DrawFillRect(screen, drawRect, item_Shadowcolor);
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	local_gwm_defaultDrawTvGuideMethod(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																			ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32			i = 0, yPos = ONDK_GUIDE_CELL_START_Y;
	ONDK_Rect_t 	drawRect;
	ONDK_GuideChannel_t	*pstChannelDataList = NULL;
	TV_GUIDE_OBJECT	tvGuideObj = (TV_GUIDE_OBJECT)object;

	// draw search time
	local_gwm_DrawEpgSearchTime(screen, area, rect, object, arg, param1);

	if (tvGuideObj->userdata != NULL)
	{
		pstChannelDataList = (ONDK_GuideChannel_t*)tvGuideObj->userdata;

		if (tvGuideObj->bPfProgramme)
		{
			ONDK_Debug("\n this is pf programme! \n");

			for(i = 0; i < ONDK_GUIDE_QUERY_NUM; i++)
			{
				local_gwm_DrawPFColumn(screen, area, rect, object, yPos, &pstChannelDataList[i]);
				yPos += ONDK_GUIDE_CELL_START_H;
			}
			// Draw Dim R
			if (tvGuideObj->hGridDimRImg)
			{
				drawRect = ONDK_Rect(ONDK_GUIDE_CELL_DIM_R_X, ONDK_GUIDE_CELL_DIM_R_Y,
										ONDK_GUIDE_CELL_DIM_R_W, ONDK_GUIDE_CELL_DIM_R_H);
				ONDK_IMG_StretchBlit(tvGuideObj->hGridDimRImg, screen, drawRect);
			}
		}
		else
		{
			ONDK_Debug("\n this is NEXT programme! \n");

			for(i = 0; i < ONDK_GUIDE_QUERY_NUM; i++)
			{
				local_gwm_DrawColumn(screen, area, rect, object, yPos, &pstChannelDataList[i]);
				yPos += ONDK_GUIDE_CELL_START_H;
			}
			// Draw Dim L
			if (tvGuideObj->hGridDimLImg)
			{
				drawRect = ONDK_Rect(ONDK_GUIDE_CELL_DIM_L_X, ONDK_GUIDE_CELL_DIM_L_Y,
										ONDK_GUIDE_CELL_DIM_L_W, ONDK_GUIDE_CELL_DIM_L_H);
				ONDK_IMG_StretchBlit(tvGuideObj->hGridDimLImg, screen, drawRect);
			}

			local_gwm_DrawCurrentVerticalLine(screen, object);
		}
	}
	else
	{
		for(i = 0; i < ONDK_GUIDE_QUERY_NUM; i++)
		{
			local_gwm_DrawColumn(screen, area, rect, object, yPos, NULL);
			yPos += ONDK_GUIDE_CELL_START_H;
		}
		// Draw Dim L
		if (tvGuideObj->hGridDimLImg)
		{
			drawRect = ONDK_Rect(ONDK_GUIDE_CELL_DIM_L_X, ONDK_GUIDE_CELL_DIM_L_Y,
									ONDK_GUIDE_CELL_DIM_L_W, ONDK_GUIDE_CELL_DIM_L_H);
			ONDK_IMG_StretchBlit(tvGuideObj->hGridDimLImg, screen, drawRect);
		}
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	local_gwm_defaultTvGuideProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	TV_GUIDE_OBJECT	tvGuideObj;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
	HBOOL			isFrontMost = FALSE;

	ONDK_Assert(object->type == GUIOBJ_TVGUIDE);
	tvGuideObj = (TV_GUIDE_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			//local_gwmChannelList_FreeObj(tvGuideObj);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			isFrontMost = (arg > 0) ? TRUE : FALSE;

			if (object->objDrawMethod != NULL)
			{
				object->objDrawMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (TV_GUIDE_OBJECT)tvGuideObj, arg);
			}
			else
			{
				local_gwm_defaultDrawTvGuideMethod(ONDK_GWM_GetSurface(ONDK_GWM_APP_GetPosition(g_pstCurrentApp)), &area, &rect, object, (TV_GUIDE_OBJECT)tvGuideObj, arg);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*new_rect;

			new_rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(tvGuideObj->rect.x, tvGuideObj->rect.y, tvGuideObj->rect.w, tvGuideObj->rect.h);
			memcpy( &(tvGuideObj->rect), new_rect, sizeof(ONDK_Region_t));
			ONDK_GWM_APP_InvalidateRelativeRect(tvGuideObj->rect.x, tvGuideObj->rect.y, tvGuideObj->rect.w, tvGuideObj->rect.h);

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
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_GET, (Handle_t)NULL, object->ID, arg, (int)tvGuideObj, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(tvGuideObj->rect.x, tvGuideObj->rect.y, tvGuideObj->rect.w, tvGuideObj->rect.h);
			break;

		case MT_LOST_FOCUS:
			tvGuideObj->focus &= ~GUIOBJ_PUSHED;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, (int)tvGuideObj, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(tvGuideObj->rect.x, tvGuideObj->rect.y, tvGuideObj->rect.w, tvGuideObj->rect.h);
			break;
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}

void	ONDK_GWM_COM_TvGuide_Init(void)
{
	struct tagTvGuideObject	*pObj = &s_defTvGuide;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagTvGuideObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_TVGUIDE;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwm_defaultTvGuideProc;
	pObj->objDrawMethod = NULL;

}

ONDK_Result_t	ONDK_GWM_COM_TvGuide_Create(HINT32 id, HINT32 posX, HINT32 posY)
{
	ONDK_GWM_GuiObject_t	objlist;
	TV_GUIDE_OBJECT			new_tv_guide;
	ONDK_Result_t			hresult;

	ONDK_Assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_tv_guide = (TV_GUIDE_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_tv_guide != NULL)
	{
		printf("\n\n\n\n[%s.%d] ----------- TVGUID CONFLICT ------------\n\n\n",__FUNCTION__,__LINE__);
		return GWM_RET_LINK_CONFLICT;
	}

	new_tv_guide = local_gwm_AllocNewTvGuideObj();
	if (new_tv_guide == NULL)
	{
		ONDK_Error("[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		printf("\n\n\n\n\n[%s:%d] obj GWM_RET_OUT_OF_MEMORY\n", __FUNCTION__, __LINE__);
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_tv_guide->ID = id;
	new_tv_guide->rect.x = 0;
	new_tv_guide->rect.y = 0;
	new_tv_guide->rect.w = 1280;
	new_tv_guide->rect.h = 720;

	// my kim
	// focus item 을 제외한 나머지 item 의 height.
	new_tv_guide->userdata = NULL;

	new_tv_guide->grid1LImg = NULL;
	new_tv_guide->grid2CImg = NULL;
	new_tv_guide->grid3RImg = NULL;
	new_tv_guide->gridDimLImg = NULL;
	new_tv_guide->gridDimRImg = NULL;

	new_tv_guide->hGrid1LImg = NULL;
	new_tv_guide->hGrid2CImg = NULL;
	new_tv_guide->hGrid3RImg = NULL;
	new_tv_guide->hGridDimLImg = NULL;
	new_tv_guide->hGridDimRImg = NULL;

	new_tv_guide->pszNoProgram = NULL;
	new_tv_guide->pszOnAir = NULL;
	new_tv_guide->pszNext = NULL;

	new_tv_guide->ulSvcUid = 0;
	// my kim end

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_tv_guide);
	ONDK_GWM_APP_InvalidateRelativeRect(new_tv_guide->rect.x, new_tv_guide->rect.y,
								new_tv_guide->rect.w, new_tv_guide->rect.h);

	NOT_USED_PARAM(posX);
	NOT_USED_PARAM(posY);

	return hresult;
}


void	ONDK_GWM_COM_TvGuide_SetBackgroundImg(HINT32 id, HCHAR	*backgroundImg)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
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

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

void	ONDK_GWM_COM_TvGuide_SetPF(HINT32 id, HBOOL bPf)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->bPfProgramme = bPf;
}

void	ONDK_GWM_COM_TvGuide_SetCurrentTime(HINT32 id, HUINT32 currentTime)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->currentTime = currentTime;
}

void	ONDK_GWM_COM_TvGuide_SetStartUnitTime(HINT32 id, HUINT32 unitTime)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->epgDisStartTime = unitTime;
}

void	ONDK_GWM_COM_TvGuide_SetGrid1LImg(HINT32 id, HCHAR	*img)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->grid1LImg = img;

	if(obj->hGrid1LImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hGrid1LImg);
		obj->hGrid1LImg = NULL;
	}

	obj->grid1LImg = img;
	obj->hGrid1LImg = ONDK_IMG_CreateSelf((HUINT8*)img);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

void	ONDK_GWM_COM_TvGuide_SetGrid2CImg(HINT32 id, HCHAR	*img)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->grid2CImg = img;

	if(obj->hGrid2CImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hGrid2CImg);
		obj->hGrid2CImg = NULL;
	}

	obj->grid2CImg = img;
	obj->hGrid2CImg = ONDK_IMG_CreateSelf((HUINT8*)img);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}

void	ONDK_GWM_COM_TvGuide_SetGrid3RImg(HINT32 id, HCHAR	*img)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->grid3RImg = img;

	if(obj->hGrid3RImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hGrid3RImg);
		obj->hGrid3RImg = NULL;
	}

	obj->grid3RImg = img;
	obj->hGrid3RImg = ONDK_IMG_CreateSelf((HUINT8*)img);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}


void	ONDK_GWM_COM_TvGuide_SetGridDimLImg(HINT32 id, HCHAR *img)
{
	TV_GUIDE_OBJECT 	obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_TVGUIDE))
	{
		return;
	}

	if (obj->hGridDimLImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hGridDimLImg);
		obj->hGridDimLImg = NULL;
	}

	obj->gridDimLImg = img;
	obj->hGridDimLImg = ONDK_IMG_CreateSelf((HUINT8*)img);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}


void	ONDK_GWM_COM_TvGuide_SetGridDimRImg(HINT32 id, HCHAR *img)
{
	TV_GUIDE_OBJECT 	obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_TVGUIDE))
	{
		return;
	}

	if (obj->hGridDimRImg != NULL)
	{
		ONDK_IMG_Destroy(obj->hGridDimRImg);
		obj->hGridDimRImg = NULL;
	}

	obj->gridDimRImg = img;
	obj->hGridDimRImg = ONDK_IMG_CreateSelf((HUINT8*)img);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}


void	ONDK_GWM_COM_TvGuide_SetEpgData(HINT32 id, void *userdata)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (obj == NULL || obj->type != GUIOBJ_TVGUIDE)
	{
		return ;
	}

	obj->userdata = userdata;
}


void	ONDK_GWM_COM_TvGuide_SetEpgStringData(HINT32 id, HCHAR *pszNoProgram, HCHAR *pszOnAir, HCHAR *pszNext)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_TVGUIDE))
	{
		return ;
	}

	obj->pszNoProgram = pszNoProgram;
	obj->pszOnAir = pszOnAir;
	obj->pszNext = pszNext;

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}


void	ONDK_GWM_COM_TvGuide_SetEpgCurrentChannel(HINT32 id, HUINT32 ulSvcUid)
{
	TV_GUIDE_OBJECT		obj;

	obj = (TV_GUIDE_OBJECT)ONDK_GWM_APP_GetObject(id);
	if ((obj == NULL) || (obj->type != GUIOBJ_TVGUIDE))
	{
		return;
	}

	obj->ulSvcUid = ulSvcUid;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&obj->rect);
}



