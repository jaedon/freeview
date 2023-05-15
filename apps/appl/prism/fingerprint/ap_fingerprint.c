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

/**************************************************************************************************/
#define _________AP_Fingerprint_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>
#include "ap_fingerprint.h"

/**************************************************************************************************/
#define _________AP_Fingerprint_Private_Macro_________________________________________________
/**************************************************************************************************/
#undef		COMMAND_NAME
#define		COMMAND_NAME				"APP_FP"


#define AP_FP_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}

// Local Config
//#define	FINGERPRINT_SUPPORT_BG									1	// CDIG-2000C CONAX PT에서 제거 요청
//#define	FINGERPRINT_SUPPORT_BG_LINE								1	// CDIG-2000C CONAX PT에서 제거 요청
#define	FINGERPRINT_SUPPORT_VK_TIMER							1
#define	FINGERPRINT_SUPPORT_OSD_REPOSITION_WIDH_DISPLAY_FORMAT	1
#define	FINGERPRINT_SUPPORT_OSD_RESCALING_720_576				1
//#define	FINGERPRINT_SUPPORT_SCHECULE							1
//#define	FINGERPRINT_SUPPORT_CENTI_TO_MILLI_SECONDS				1	// Not Used config because alreay convert to CAS

#define	FINGERPRINT_CAS_CX_IMMEDIATELY_TIME						0xFFFFFF
#define	FINGERPRINT_CAS_CX_SECONDS								1000

#define	FINGERPRINT_MSG_MAX_ITEM								10		// the maxuimum number of list items which can handle
#define	FINGERPRINT_PARSE_ITEM_MAX								32

#define	FINGERPRINT_MAX_CENTTIME_INDEX							4

#define	OSD_SCREEN_WIDTH										1280	// CONFIG_MWC_OSD_HORIZONTAL
#define	OSD_SCREEN_HEIGHT										720		// CONFIG_MWC_OSD_VERTICAL

#define	OSD_SCREEN_1280_WIDTH									1280
#define	OSD_SCREEN_720_HEIGHT									720

#define	OSD_SCREEN_1920_WIDTH									1920
#define	OSD_SCREEN_1080_HEIGHT									1080

#define	FINGERPRINT_OSD_SCREEN_WIDTH							720
#define	FINGERPRINT_OSD_SCREEN_HEIGHT							576

#define	FINGERPRINT_XPOS_OFFSET_PILLARBOX						162		// PILLABOX mode의 경우 OFFSET
#define	FINGERPRINT_YPOS_OFFSET_LETTERBOX						162		// LETTERBOX mode의 경우 OFFSET

//#define	FINGERPRINT_WIDTH									210
//#define	FINGERPRINT_HEIGHT									55

#define	FINGERPRINT_MSG_DESC_W2									684
#define	FINGERPRINT_MSG_DESC_VSPACE								8
#define	FINGERPRINT_MSG_DESC_MG									20
#define	FINGERPRINT_MSG_LINE1_H									1

#define	ONDK_FINGERPRINT_LINE_T_ID								10
#define	ONDK_FINGERPRINT_LINE_B_ID								11
#define	ONDK_FINGERPRINT_FRAME_ID								12
#define	ONDK_FINGERPRINT_STRING_ID								13

#define	ONDK_FINGERPRINT_FONT_SIZE_DEAULT_OFFSET				2
#define	ONDK_FINGERPRINT_FONT_NORMAL_SIZE						28
#define	ONDK_FINGERPRINT_FONT_COLOR								0xffffffff	// White 100%

#define	ONDK_FINGERPRINT_LINE_COLOR								0xffffff00
#define	ONDK_FINGERPRINT_FRAME_COLOR							0xc0000000	// Black 75%


/**************************************************************************************************/
#define ____AP_Fingerprint_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef enum
{
	FINGERPRINT_KEY_ID,
	FINGERPRINT_KEY_TIME,
	FINGERPRINT_KEY_DURATION,
	FINGERPRINT_KEY_X,
	FINGERPRINT_KEY_Y,
	FINGERPRINT_KEY_HEIGHT,
	FINGERPRINT_KEY_TEXT,
	FINGERPRINT_KEY_UNKNOWN
} FINGERPRINT_Key_State_t;

typedef	struct	_FingerprintData
{
	HUINT32		ulTime;			// 0xffffff = immediately
	HUINT16		usDuration;		// 단위 centi-second (1/100 초)
	HUINT16		usPosX;
	HUINT16		usPosY;
	HUINT16		usHeight;
	HUINT8		szText[FINGERPRINT_PARSE_ITEM_MAX + 1];
	HUINT8		ucCentTime[FINGERPRINT_MAX_CENTTIME_INDEX];
} FingerprintData_t;

typedef struct _Fingerprint_Contents
{
	ONDKWindowHandle	m_window;
	ONDKSurfaceHandle	m_surface;
	APK_HANDLE			m_liveMedia;
	APK_HANDLE			m_tsrPlayback;
	APK_HANDLE			m_mediaPlayback;

    HBOOL				m_bDisplayState;  // teletext 활성화되면 , display 중지 다시 비활성화되면 기존 플래그를 보고 재가동
    HBOOL				m_bLiveLock; // LIVE 가 LOCK 인상태
    HBOOL				bSetTimer;

	HUINT32				ulDisplayTimerId;

    FingerprintData_t	data;
} Fingerprint_Contents_t;


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Value_________________________________________________
/**************************************************************************************************/

static Fingerprint_Contents_t	stFingerprint_Contents;


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Prototype_________________________________________________
/**************************************************************************************************/

static Fingerprint_Contents_t* local_fingerprint_GetContents (void );

static void				local_fingerprint_cbTimeoutOccur(HUINT32 ulTimerId, void *pArg);
static HERROR			local_fingerprint_SetDuratonTimer(Fingerprint_Contents_t *pstContents);

//ui
static void 			local_fingerprint_SetUiWindowArea(Fingerprint_Contents_t *pstContents, HINT32 *lAreaW, HINT32 *lAreaH, HINT32 *plFontSize);
static void 			local_fingerprint_UpdateSetUiObject(HBOOL bHide);
static void 			local_fingerprint_SetUiObject(Fingerprint_Contents_t *pstContents);

//init
static ONDK_Result_t 	local_fingerprint_SetFingerprintData(Fingerprint_Contents_t *pstContents, HBOOL *bTimePrint, HUINT32 *ulTime, HUINT16 *usDuration);
static ONDK_Result_t	local_fingerprint_ParseJsonString(Fingerprint_Contents_t *pstContents, const HCHAR* pJsonString);
static ONDK_Result_t	local_fingerprint_Initialize(Fingerprint_Contents_t *pstContents, const HCHAR* pJsonString);

//msg run
static ONDK_Result_t 	local_fingerprint_MsgGwmCreate(Fingerprint_Contents_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_fingerprint_MsgGwmDestroy(Fingerprint_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_fingerprint_MsgGwmKeyDown(Fingerprint_Contents_t *pstContents, HINT32 p1);
static ONDK_Result_t	local_fingerprint_MsgGwmTimer(Fingerprint_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	local_fingerprint_APKMessage(Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static ONDK_Result_t 	local_fingerprint_signal_RequestStandby(Fingerprint_Contents_t *pstContents);
static ONDK_Result_t	local_fingerprint_signal_ShowFingerprint(HBOOL bShow);


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Init__________________________________________________________
/**************************************************************************************************/

static Fingerprint_Contents_t* local_fingerprint_GetContents(void)
{
	return &stFingerprint_Contents;
}


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Process__________________________________________________________
/**************************************************************************************************/

static void		local_fingerprint_cbTimeoutOccur(HUINT32 ulTimerId, void *pArg)
{
	Fingerprint_Contents_t 	*pstContents = NULL;

	pstContents = local_fingerprint_GetContents();
	if (pstContents == NULL)
	{
		HxLOG_Error("[%s:%d] pstContents is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	HxLOG_Info("pstContents->ulDisplayTimerId : [0x%x] cbTimeoutOccur : [0x%x]\n", pstContents->ulDisplayTimerId, ulTimerId);

	if (pstContents->ulDisplayTimerId != ulTimerId)
	{
		HxLOG_Error("[%s:%d] Invlaid Timer ID\n", __FUNCTION__, __LINE__);
		return;
	}

	pstContents->bSetTimer = FALSE;
	ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
	HxLOG_Debug("ONDK_GWM_APP_Destroy()\n");

	ONDK_GWM_DirectRedrawAll(ONDK_UPPER_WINDOW);
}


static HERROR		local_fingerprint_SetDuratonTimer(Fingerprint_Contents_t *pstContents)
{
	HUINT32		ulError = ERR_OK;

	HxLOG_Debug("local_fingerprint_SetDuratonTimer()\n");

//fp proc 을 create 할때, gwm create 자체가 invalidate를 타고, 이는 결국 gwm bus를 돌아야 함.
//이러한 경우 만약 fp duration이 20ms 미만일 때, duration을 지키기 위해 vk timer를 사용하고, 간혹 gwm create의 invalidate를 타기도 전에 vk timer가 올라와서 call back을 타는 경우도 발생함
//gwm timer를 사용하면 gwm bus가 꼬이는 일은 발생하지 않으나, 오차 범위가 100ms 이상 나게 된다.
//conax spec에는 duration이 20ms 인 것까지 지키게 되어 있음......
//timer 위치를 일단 다 그려진 msg를 받은 이후로 옮기고 vk timer가 불리었을 때, direct redraw 등을 이용함
//현재 20ms 에 가깝게 지켜지고 있으나, 간혹 gwm 상에서 redraw에 문제가 생겨 다른 osd 들에 영향을 줄 수 있음
//GWM redraw를 사용하도록 변경.

#if defined(FINGERPRINT_SUPPORT_VK_TIMER)
	if (pstContents->data.usDuration > 0 )
	{
		HxLOG_Info("VK_TIMER_EventAfter - Duration : [%u] \n", pstContents->data.usDuration);
		// TODO: [JHLEE] Check UTC Duration Time has centi-second (1/100) 그래서 Duration * 10을 해야 하는데 이건 UNIX Time으로들어 오네...음...
#if defined(FINGERPRINT_SUPPORT_CENTI_TO_MILLI_SECONDS)
		ulError = VK_TIMER_EventAfter(pstContents->data.usDuration * 10, local_fingerprint_cbTimeoutOccur, NULL, 0, &(pstContents->ulDisplayTimerId));
#else
		ulError = VK_TIMER_EventAfter(pstContents->data.usDuration, local_fingerprint_cbTimeoutOccur, NULL, 0, &(pstContents->ulDisplayTimerId));
#endif
		if (ulError != ERR_OK)
		{
			HxLOG_Error("VK_TIMER_EventAfter() return error\n");
		}
	}
	else
	{
		HxLOG_Error("Pls check duration value was zero!!!\n");
	}
#else
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FINGERPRINT_DURATION, pstContents->data.usDuration);
#endif

	return	ERR_OK;
}


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Proc_UI_______________________________________________________
/**************************************************************************************************/

static void 	local_fingerprint_UpdateSetUiObject(HBOOL bHide)
{
	HxLOG_Info("Update UI Object : bHide[%d]\n", bHide);

#if defined(FINGERPRINT_SUPPORT_BG)
#if defined(FINGERPRINT_SUPPORT_BG_LINE)
	ONDK_GWM_Obj_SetHide(ONDK_FINGERPRINT_LINE_T_ID, bHide);
	ONDK_GWM_Obj_SetHide(ONDK_FINGERPRINT_LINE_B_ID, bHide);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG_LINE)
	ONDK_GWM_Obj_SetHide(ONDK_FINGERPRINT_FRAME_ID, bHide);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG)
	ONDK_GWM_Obj_SetHide(ONDK_FINGERPRINT_STRING_ID, bHide);

#if defined(FINGERPRINT_SUPPORT_BG)
#if defined(FINGERPRINT_SUPPORT_BG_LINE)
	ONDK_GWM_APP_InvalidateObject(ONDK_FINGERPRINT_LINE_T_ID);
	ONDK_GWM_APP_InvalidateObject(ONDK_FINGERPRINT_LINE_B_ID);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG_LINE)
	ONDK_GWM_APP_InvalidateObject(ONDK_FINGERPRINT_FRAME_ID);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG)
	ONDK_GWM_APP_InvalidateObject(ONDK_FINGERPRINT_STRING_ID);
}


static void 	local_fingerprint_SetUiWindowArea(Fingerprint_Contents_t *pstContents, HINT32 *lAreaW, HINT32 *lAreaH, HINT32 *plFontSize)
{
	HERROR			hError = ERR_FAIL;
	HINT32 			lFontSize = ONDK_FINGERPRINT_FONT_SIZE_DEAULT_OFFSET, lLineCnt = 0, lFpFontsize = 0;
	HINT32			lBannerX = 0, lBannerY = 0, lBannerWidth = 0, lBannerHeight = 0;
	HUINT32			h = 0, ulStrLen = 0;
	ONDKFontHandle			*hFontHandle = NULL;
	//FontMultiLine_Item_t	*pstArrItem = NULL;
	HUINT32			ulViewId = 0;
	HUINT32			ulRescalingX = 0, ulRescalingY = 0;

	HxLOG_Assert(pstContents);


	// Resize for 720*576 OSD - Validation Test - 9.3.4
#if 0//defined(FINGERPRINT_SUPPORT_OSD_RESCALING_720_576)
	lFpFontsize = (HINT32)pstContents->data.usHeight;
	hError = APK_OUTPUT_VIDEO_GetResolutionSelect(&eResolutionSelect);
	if (hError == ERR_OK)
	{
		switch (eResolutionSelect)
		{
			case eDxRESOLUTION_SELECT_720P:
				lFpFontsize = (OSD_SCREEN_720_HEIGHT * lFpFontsize) / FINGERPRINT_OSD_SCREEN_HEIGHT;
				break;
			case eDxRESOLUTION_SELECT_1080I:
			case eDxRESOLUTION_SELECT_1080P:
				lFpFontsize = (OSD_SCREEN_1080_HEIGHT * lFpFontsize) / FINGERPRINT_OSD_SCREEN_HEIGHT;
				break;
			default:
				HxLOG_Info("Finger print display original scaling.\n");
				break;
		}
	}
	else
	{
		HxLOG_Info("Error!!! APK_OUTPUT_VIDEO_GetDfcSelect()\n");
	}
#endif
	lFpFontsize = (HINT32)pstContents->data.usHeight;
	lFpFontsize = (OSD_SCREEN_HEIGHT * lFpFontsize) / FINGERPRINT_OSD_SCREEN_HEIGHT;

	lFontSize += lFpFontsize;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, lFontSize);
	lLineCnt = ONDK_FONT_GetLineCount(NULL, hFontHandle, pstContents->data.usPosX, pstContents->data.usPosY,
							FINGERPRINT_MSG_DESC_W2, (HUINT8*)pstContents->data.szText, FINGERPRINT_MSG_DESC_VSPACE, (ONDKSTF_EQUALSPACE|ONDKSTF_WORDWRAP), &h);
	if (lLineCnt < 1)
		lLineCnt = 1;
/*
	pstArrItem = (FontMultiLine_Item_t *)AP_Malloc(sizeof(FontMultiLine_Item_t)*FINGERPRINT_MSG_MAX_ITEM);
	lLineCnt = ONDK_FONT_GetLineCountWithParseMultiLine(hFontHandle, pstContents->data.usPosX, pstContents->data.usPosY,
					FINGERPRINT_MSG_DESC_W2, lFontSize, (HUINT8*)pstContents->data.szText, FINGERPRINT_MSG_DESC_VSPACE,
					FINGERPRINT_MSG_MAX_ITEM, ONDKSTF_EQUALSPACE, &h, pstArrItem);
	if (pstArrItem)
	{
		AP_Free(pstArrItem);
	}
*/

	lBannerWidth = ONDK_FONT_GetStringWidth(hFontHandle, (HUINT8*)pstContents->data.szText);
	lBannerWidth += FINGERPRINT_MSG_DESC_MG;
	lBannerHeight = (lFontSize * lLineCnt);// + FINGERPRINT_MSG_DESC_VSPACE;

	if (hFontHandle)
	{
		ONDK_FONT_Release (hFontHandle);
	}

	ulStrLen = HxSTD_StrLen(pstContents->data.szText);
	if (ulStrLen == 0)
	{
		HxLOG_Debug("Error!! Invalid Text Data - fingerprint!! \n");
		return;
	}

	lBannerX = pstContents->data.usPosX;
	lBannerY = pstContents->data.usPosY;

	// TODO: PIG 또는 Mini-picute 를 지원할 경우 Fingerprint가 PIG, mini-picture 영역 안에 들어가야 한다.
	/*
 		Spec - Chipset Pairing STB Integration Kit 6.0 : 9.2.2 Reposition and rescaling for fingerpint 참조
	*/

	// Resize for 720*576 OSD - Validation Test - 9.3.4
#if defined(FINGERPRINT_SUPPORT_OSD_RESCALING_720_576)
	lBannerX = (OSD_SCREEN_WIDTH *  lBannerX) / FINGERPRINT_OSD_SCREEN_WIDTH;
	lBannerY = (OSD_SCREEN_HEIGHT * lBannerY) / FINGERPRINT_OSD_SCREEN_HEIGHT;
#endif

	hError = APK_MEDIA_PLAY_GetMainViewId( &ulViewId );
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetMainViewId err: \n");
	}
	hError = APK_MEDIA_PLAY_GetOsdDisplayPoint(ulViewId, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT, lBannerX, lBannerY, &ulRescalingX, &ulRescalingY);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetOsdDisplayPoint err: \n");
	}
	HxLOG_Warning("\tAPK_MEDIA_PLAY_GetOsdDisplayPoint(), (%d, %d) -> (%d, %d)\n", lBannerX, lBannerY, ulRescalingX, ulRescalingY);

	HxLOG_Warning("==== Set Window Area ====\n");
	HxLOG_Warning("- pstContents->data.usHeight : [%d]\n", pstContents->data.usHeight);
	HxLOG_Warning("- pstContents->data.usPosX : [%d]\n", pstContents->data.usPosX);
	HxLOG_Warning("- pstContents->data.usPosY : [%d]\n", pstContents->data.usPosY);
	HxLOG_Warning("- lFontSize : [%d]\n", lFontSize);
	HxLOG_Warning("- lLineCnt : [%d]\n", lLineCnt);
	HxLOG_Warning("- X : [%d]\n", lBannerX);
	HxLOG_Warning("- Y : [%d]\n", lBannerY);
	HxLOG_Warning("- Real OSD rescaling X : [%d]\n", ulRescalingX);
	HxLOG_Warning("- Real OSD rescaling Y : [%d]\n", ulRescalingY);
	HxLOG_Warning("- lBannerWidth : [%d]\n", lBannerWidth);
	HxLOG_Warning("- lBannerHeight : [%d]\n", lBannerHeight);
	HxLOG_Warning("=========================\n");


	//ONDK_GWM_APP_SetAppArea(lBannerX, lBannerY, lBannerWidth, lBannerHeight);
	ONDK_GWM_APP_SetAppArea(ulRescalingX, ulRescalingY, lBannerWidth, lBannerHeight);

	*lAreaW = lBannerWidth;
	*lAreaH = lBannerHeight;
	*plFontSize = lFontSize;
}


static void 	local_fingerprint_SetUiObject(Fingerprint_Contents_t *pstContents)
{
	HINT32			lAreaW = 0, lAreaH = 0, lFontSize = 0;
#if defined(FINGERPRINT_SUPPORT_BG) || defined(FINGERPRINT_SUPPORT_BG_LINE)
	HINT32			lLineH = 0;
#endif
	ONDK_Rect_t 	stRect;

	HxLOG_Assert(pstContents);

	local_fingerprint_SetUiWindowArea(pstContents, &lAreaW, &lAreaH, &lFontSize);

#if defined(FINGERPRINT_SUPPORT_BG)
#if defined(FINGERPRINT_SUPPORT_BG_LINE)
	lLineH = FINGERPRINT_MSG_LINE1_H;

	// Draw Line Top
	stRect = ONDK_Rect(0, 0, lAreaW, lLineH);
	ONDK_GWM_COM_Rect_Create(ONDK_FINGERPRINT_LINE_T_ID, &stRect, ONDK_FINGERPRINT_LINE_COLOR);

	// Draw Line Bottom
	stRect = ONDK_Rect(0, (lAreaH - lLineH), lAreaW, lLineH);
	ONDK_GWM_COM_Rect_Create(ONDK_FINGERPRINT_LINE_B_ID, &stRect, ONDK_FINGERPRINT_LINE_COLOR);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG_LINE)
	// Draw BG
	stRect = ONDK_Rect(0, lLineH, lAreaW, lAreaH - (lLineH *2));
	ONDK_GWM_COM_Rect_Create(ONDK_FINGERPRINT_FRAME_ID, &stRect, ONDK_FINGERPRINT_FRAME_COLOR);
#endif	// End of defined(FINGERPRINT_SUPPORT_BG)

	// string
	//stRect = ONDK_Rect(FINGERPRINT_MSG_DESC_MG, 2, lAreaW, lAreaH);
	stRect = ONDK_Rect(2, 2, lAreaW, lAreaH);
	ONDK_GWM_COM_Text_Create(ONDK_FINGERPRINT_STRING_ID, &stRect, pstContents->data.szText);
	ONDK_GWM_COM_Text_SetFont(ONDK_FINGERPRINT_STRING_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(ONDK_FINGERPRINT_STRING_ID, lFontSize);
	ONDK_GWM_COM_Text_SetAlign(ONDK_FINGERPRINT_STRING_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(ONDK_FINGERPRINT_STRING_ID, ONDK_FINGERPRINT_FONT_COLOR, ONDK_FINGERPRINT_FONT_COLOR);

	//local_fingerprint_signal_ShowFingerprint(TRUE);
}


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Init______________________________________________________
/**************************************************************************************************/

static ONDK_Result_t 	local_fingerprint_SetFingerprintData(Fingerprint_Contents_t *pstContents, HBOOL *bTimePrint, HUINT32 *ulTime, HUINT16 *usDuration)
{
	HCHAR			*pszCurDateTime = NULL;
#if defined(FINGERPRINT_SUPPORT_SCHECULE)
	HUINT16			usFPDuration = 0, usTmpDuration = 0;
	HUINT32			ulDiffDisplayTime = 0;
#endif
	HUINT32			ulSystemTime = 0, ulFPUnixTime = 0, ulLen = 0;
#if defined(FINGERPRINT_SUPPORT_CENTI_TO_MILLI_SECONDS)
	HUINT8			*ucFPCentiTime = NULL;
	HxDATETIME_t 	stFPDateTime, stCurrentDateTime;
#endif

	HxLOG_Assert(pstContents);

	*bTimePrint = 0;
	*ulTime = 0;
	*usDuration = 0;

#if 0
	{
		HxDATETIME_t dateTime;
		UNIXTIME	ulMakeTime = 0;

		// TEST code
		ulSystemTime = HLIB_STD_GetSystemTime();
		pszCurDateTime = HLIB_DATETIME_UnixTimeToString(ulSystemTime, NULL, 0);
		HxLOG_Info("Current Date&Time :[%s]\n", pszCurDateTime);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &dateTime);
		dateTime.stTime.ucMinute += 1;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&dateTime, &ulMakeTime);
		pszCurDateTime = HLIB_DATETIME_UnixTimeToString(ulMakeTime, NULL, 0);
		HxLOG_Info("Make Current Date&Time :[%s]\n", pszCurDateTime);

		pstContents->data.ulTime = ulMakeTime;
	}
#endif

	ulFPUnixTime = pstContents->data.ulTime;

	// Text Size
	ulLen = HxSTD_StrLen(pstContents->data.szText);
	if (ulLen == 0)
	{
		HxLOG_Debug("Error!! Invalid Text Data - fingerprint!! \n");
		return	ONDK_RET_FAIL;
	}

	/* display immediately */
	if (ulFPUnixTime == FINGERPRINT_CAS_CX_IMMEDIATELY_TIME)
	{
		*bTimePrint = FALSE;
		*usDuration = pstContents->data.usDuration;
		HxLOG_Debug("display immediately - ulTime : [0x%x][%u], usDuration : [0x%x][%u]\n",
				pstContents->data.ulTime, pstContents->data.ulTime, pstContents->data.usDuration, pstContents->data.usDuration);
		return	ONDK_RET_OK;
	}

	ulFPUnixTime = 0;

	// Get Current time
	ulSystemTime = HLIB_STD_GetSystemTime();
	pszCurDateTime = HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0);
	HxLOG_Info("Current Time :[%s]\n", pszCurDateTime);

#if defined(FINGERPRINT_SUPPORT_CENTI_TO_MILLI_SECONDS)
	ucFPCentiTime = pstContents->data.ucCentTime;
	if (ucFPCentiTime == NULL)
	{
		HxLOG_Debug("Error!! Fingerprint Centisecond is NULL!!!\n");
		return	ONDK_RET_FAIL;
	}

	/* Make centisecond to millisecond */

	// Conv Current time to date time - with out GMT Offset
	HxSTD_memset(&stCurrentDateTime, 0x00, sizeof(HxDATETIME_t));
	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulSystemTime, &stCurrentDateTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurrentDateTime, &ulSystemTime);

	HxLOG_Info(" == Current Time Year(%d), Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d)\n",
		stCurrentDateTime.stDate.usYear, stCurrentDateTime.stDate.ucMonth, stCurrentDateTime.stDate.ucDay,
		stCurrentDateTime.stTime.ucHour, stCurrentDateTime.stTime.ucMinute, stCurrentDateTime.stTime.ucSecond);

	// FP는 년월일 정보를 current로 받는다.
	HxSTD_memset(&stFPDateTime, 0x00, sizeof(HxDATETIME_t));
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stFPDateTime);
	// 시간정보는 실제 데이터로 set
	stFPDateTime.stTime.ucHour = ucFPCentiTime[0];
	stFPDateTime.stTime.ucMinute = ucFPCentiTime[1];
	stFPDateTime.stTime.ucSecond = ucFPCentiTime[2];
	stFPDateTime.stTime.usMillisecond = (HUINT16)ucFPCentiTime[3];
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stFPDateTime, &ulFPUnixTime);

	HxLOG_Info(" == Fingerprint Time Year(%d), Month(%d), Day(%d), Hour(%d), Min(%d), Sec(%d)\n",
		stFPDateTime.stDate.usYear, stFPDateTime.stDate.ucMonth, stFPDateTime.stDate.ucDay,
		stFPDateTime.stTime.ucHour, stFPDateTime.stTime.ucMinute, stFPDateTime.stTime.ucSecond);

#endif	// End of defined(FINGERPRINT_SUPPORT_CENTI_TO_MILLI_SECONDS)

#if defined(FINGERPRINT_SUPPORT_SCHECULE)
	/* Set Current Display Or After Timer */
	// Set After Timer
	if (ulSystemTime < ulFPUnixTime)
	{
		ulDiffDisplayTime = (ulFPUnixTime - ulSystemTime);
		*ulTime = (ulDiffDisplayTime * FINGERPRINT_CAS_CX_SECONDS);
		*bTimePrint = TRUE;

		HxLOG_Info("Set After Timer - Current Time : 0x%x, Finger Time : 0x%x, Diff : 0x%x\n", ulSystemTime, ulFPUnixTime, ulDiffDisplayTime);
	}
	else // Display Current Time
	{
		*bTimePrint = FALSE;
		usFPDuration = pstContents->data.usDuration;
		usTmpDuration = (HUINT16)(ulSystemTime - ulFPUnixTime)*100;
		ulDiffDisplayTime = (ulFPUnixTime - ulSystemTime);

		// 현재(즉시) 시간 display하는 경우는 다른 처리하지 않고 그냥 진행 함.
		// 과거 시간
		if (ulFPUnixTime + (usFPDuration/100) < ulSystemTime)
		{
			HxLOG_Debug("Error!! Invalid Data - Past fingerprint!! \n");
			return	ERR_FAIL;
		}

		if (usFPDuration >usTmpDuration)
		{
			usFPDuration -= usTmpDuration;
			*usDuration = usFPDuration;
		}

		HxLOG_Info("Disply current Timer - Current Time : 0x%x, Finger Time : 0x%x, Diff : 0x%x\n", ulSystemTime, ulFPUnixTime, ulDiffDisplayTime);
		HxLOG_Info("Disply current Timer - FingerPrint Duration Time 0x%x\n", usFPDuration);
	}
#endif
	//local_fingerprint_signal_ShowFingerprint(TRUE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	local_fingerprint_ParseJsonString(Fingerprint_Contents_t *pstContents, const HCHAR* pJsonString)
{
	HxJSON_t	root, uidata, attrArray, stringArray, item;

	HxLOG_Assert(pJsonString);

	root = HLIB_JSON_Decode(pJsonString);
	HxLOG_Assert(root);

	if (0 == HxSTD_StrNCmp(HLIB_JSON_Object_GetString(root, "uitype"), "textbox",  FINGERPRINT_PARSE_ITEM_MAX - 1))
	{
		HBOOL 	bFingerprint = FALSE;
		uidata = HxJSON_Object_Get(root, "uidata");
		if (uidata != NULL)
		{
			attrArray = HLIB_JSON_Object_GetArray(uidata, "attribute");
			if (attrArray)
			{
				HxJSON_ARRAY_EACH(attrArray, item,{
					if (!item || !HxJSON_IsString(item))
						continue;

					if (0 == HxSTD_StrNCmp(HxJSON_String_Get(item), "fingerprint", FINGERPRINT_PARSE_ITEM_MAX - 1))
						bFingerprint = TRUE;
				});
			}
			if (bFingerprint)
			{
				pstContents->data.usPosX = HLIB_JSON_Object_GetInteger(uidata, "posx");
				pstContents->data.usPosY = HLIB_JSON_Object_GetInteger(uidata, "posy");
				pstContents->data.usHeight = HLIB_JSON_Object_GetInteger(uidata, "height");
				pstContents->data.ulTime = HLIB_JSON_Object_GetInteger(uidata, "printtime");
				pstContents->data.usDuration = HLIB_JSON_Object_GetInteger(uidata, "timeout");
				stringArray = HLIB_JSON_Object_GetArray(uidata, "string");
				if (stringArray != NULL)
				{
					HxJSON_ARRAY_EACH(stringArray, item,{
						if (!item || !HxJSON_IsObject(item))
							continue;

						HxSTD_StrNCpy(pstContents->data.szText, HLIB_JSON_Object_GetString(item, "str"), FINGERPRINT_PARSE_ITEM_MAX - 1);
					});
				}
				HxLOG_Info("posX : [%d]\n", pstContents->data.usPosX);
				HxLOG_Info("posY : [%d]\n", pstContents->data.usPosY);
				HxLOG_Info("height : [%d]\n", pstContents->data.usHeight);
				HxLOG_Info("ulTime : [0x%x][%u]\n", pstContents->data.ulTime, pstContents->data.ulTime);
				HxLOG_Info("usDuration : [0x%x]\n", pstContents->data.usDuration);
				HxLOG_Info("text : [%s]\n", pstContents->data.szText);
				HxLOG_Info("stringlength : [%d]\n", HLIB_JSON_Object_GetInteger(uidata, "stringlength"));

				return	ONDK_RET_OK;
			}
		}
	}

	return	ONDK_RET_FAIL;
}


static ONDK_Result_t	local_fingerprint_Initialize(Fingerprint_Contents_t *pstContents, const HCHAR* pJsonString)
{
	HBOOL			bIsTimePrint = FALSE;
	HUINT16			usDuration = 0;
	HUINT32			ulTime = 0;
	ONDK_Result_t	hResult = ONDK_RET_FAIL;


	HxLOG_Assert(pstContents);

	// Finger Print MMI Data parsing
	local_fingerprint_ParseJsonString(pstContents, pJsonString);

	// Make Finger Print Data
	hResult = local_fingerprint_SetFingerprintData(pstContents, &bIsTimePrint, &ulTime, &usDuration);
	if (hResult != ONDK_RET_OK)
	{
		HxLOG_Debug("Error!!! Check Finger Print Data!! \n");
		ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
		return	ONDK_RET_INITFAIL;
	}

	pstContents->bSetTimer = (bIsTimePrint == TRUE) ? TRUE : FALSE;
	HxLOG_Info("pstContents->bSetTimer : [%d]\n", pstContents->bSetTimer);
	HxLOG_Info("Set Finger Print Data - bIsTimePrint : [%d]\n", bIsTimePrint);
	HxLOG_Info("Set Finger Print Data - ulTime : [0x%x][%u]\n", ulTime, ulTime);
	HxLOG_Info("Set Finger Print Data - usDuration : [0x%x][%u]\n", usDuration, usDuration);

	// Draw UI
	local_fingerprint_SetUiObject(pstContents);

	// Set After Show time
	if (bIsTimePrint == TRUE)
	{
		local_fingerprint_UpdateSetUiObject(TRUE);
		HxLOG_Info("Set After Show Timer - ulTime[0x%x][%u]\n", ulTime, ulTime);
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FINGERPRINT);
		ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FINGERPRINT, ulTime);
	}

	return	ONDK_RET_OK;
}


/**************************************************************************************************/
#define _____AP_Fingerprint_Private_Proc_MSG_RUNNIG______________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static ONDK_Result_t	local_fingerprint_MsgGwmCreate(Fingerprint_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Assert(pstContents);

	/* 절대 hide가 되지 않는다. */
	//ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	HxSTD_MemSet(pstContents, 0, sizeof(Fingerprint_Contents_t));

	//pstContents->m_window=	 ONDK_WINDOW_Create(ONDK_LOWER_WINDOWID,0,0, 1280,720, "ARGB8888", TRUE);
	//ONDK_WINDOW_SetShow(pstContents->m_window,TRUE);

	local_fingerprint_Initialize(pstContents, (const HCHAR*)p1);

	//ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FINGERPRINT, pstContents->data.usDuration);

	AP_FP_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


//MSG_GWM_DESTROY
static ONDK_Result_t	local_fingerprint_MsgGwmDestroy(Fingerprint_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Assert(pstContents);

	HxSTD_MemSet(pstContents, 0, sizeof(Fingerprint_Contents_t));

	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FINGERPRINT);
	//ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FINGERPRINT_DURATION);

	AP_FP_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


static ONDK_Result_t	local_fingerprint_MsgGwmKeyDown(Fingerprint_Contents_t *pstContents, HINT32 p1)
{
	HxLOG_Assert(pstContents);
    HxLOG_Print("[FingerPrint] key = [%x]\n", __FUNCTION__, __LINE__, p1);

    switch (p1)
    {
	    /******************************************************* ************************************/
	    /************************************ Teletext 동작 관련 Key ********************************/
	    /******************************************************* ************************************/
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
			return	ONDK_MESSAGE_NONCONSUMED;

        //case KEY_EXIT:
			//ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
			//return	ONDK_MESSAGE_CONSUMED;

        case KEY_CH_PLUS:
        case KEY_CH_MINUS:
			ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
			return	ONDK_MESSAGE_NONCONSUMED;

	    default:
    		return	ONDK_MESSAGE_NONCONSUMED;
    }
}


static ONDK_Result_t	local_fingerprint_MsgGwmTimer(Fingerprint_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		ulTimerID = p1;


	if (ulTimerID == GWMPOPUP_TIMER_FINGERPRINT)
	{
		HxLOG_Info("Now Show Timer - GWMPOPUP_TIMER_FINGERPRINT\n");
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FINGERPRINT);
		pstContents->bSetTimer = FALSE;
		local_fingerprint_UpdateSetUiObject(FALSE);
	}
#if !defined(FINGERPRINT_SUPPORT_VK_TIMER)
	else if (ulTimerID == GWMPOPUP_TIMER_FINGERPRINT_DURATION)
	{
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FINGERPRINT_DURATION);
		ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);
	}
#endif

	return	GWM_MESSAGE_BREAK;
}


static HCHAR *local_fingerprint_GetEventString (OxMediaPlay_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eOxMP_EVENT_TUNE_Locked)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_NoSignal)

		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaOk)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaNok)

		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcStopped)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ConflictChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_EventRelay)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_Ews)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_TSRTimecodeChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_BsCtrlStarted)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ViewStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_LockStateChanged)

		ENUM_TO_STR (eOxMP_EVENT_MEDIA_ProbeErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_StopErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferInfoChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PlayInfo)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PumpIndexChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_SeekReady)


		ENUM_TO_STR (eOxMP_EVENT_BOF)
		ENUM_TO_STR (eOxMP_EVENT_EOF)
		ENUM_TO_STR (eOxMP_EVENT_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_PLAYING)
		ENUM_TO_STR (eOxMP_EVENT_PAUSE)
		ENUM_TO_STR (eOxMP_EVENT_TRICK)
		ENUM_TO_STR (eOxMP_EVENT_RATING_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_SPEED_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_MEDIARECT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SupportedSpeedChanged)
		ENUM_TO_STR (eOxMP_EVENT_VIDEOSTREAMINFO)
		ENUM_TO_STR (eOxMP_EVENT_PinCtrlChanged)

		ENUM_TO_STR (eOxMP_EVENT_SI_PMT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_EIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_SIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_AIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_RCT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_NETWORK_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_VIDEO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_AUDIO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SELECTED)

		ENUM_TO_STR (eOxMP_EVENT_DRM_BLOCK)

		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_TAKEN)
		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_LOST)

		ENUM_TO_STR (eOxMP_EVENT_PLAYERROR)
		ENUM_TO_STR (eOxMP_EVENT_PLAYSTATE)
		ENUM_TO_STR (eOxMP_EVENT_PLAYTIME)

		ENUM_TO_STR (eOxMP_EVENT_RADIOBG_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_TTX_UPDATE)
		ENUM_TO_STR (eOxMP_EVENT_SBTL_UPDATE)

		ENUM_TO_STR (eOxMP_EVENT_TrickRestrictModeChanged)

	default:
		break;
	}

	return "Unknown";
}


static ONDK_Result_t	local_fingerprint_APKMessage(Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	// TODO:

	AP_FP_PARAM_WARRING_REMOVE(hAct, ulParam1, ulParam2, ulParam3);

	return GWM_RET_OK;
}

static ONDK_Result_t	local_fingerprint_signal_RequestStandby(Fingerprint_Contents_t *pstContents)
{
	//  reset all data when other application broadcast request standby signal
	ONDK_GWM_APP_Destroy(AP_Fingerprint_Proc);

	return GWM_RET_OK;
}


static ONDK_Result_t	local_fingerprint_signal_ShowFingerprint(HBOOL bShow)
{
	Fingerprint_Contents_t 	*pstContents = NULL;

	pstContents = local_fingerprint_GetContents();
	if (pstContents == NULL)
	{
		HxLOG_Critical("[%s;%d] pstContents is NULL\n", __FUNCTION__, __LINE__);
		return GWM_MESSAGE_BREAK;
	}

	pstContents->m_bDisplayState = bShow;
	HxLOG_Debug("m_bDisplayState(%d) m_bLiveLock(%d) \n", pstContents->m_bDisplayState ,pstContents->m_bLiveLock );

	if ((pstContents->m_bDisplayState == TRUE) && (pstContents->m_bLiveLock == FALSE))
	{
		pstContents->m_bDisplayState = TRUE;

		//1 #### BUS Overhead 문제발생을 대비하여, BUS가 누락될 경우 생성되지 않은 윈도우를 다시 얻어오도록 함.
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ################### Show Fingerprint ####################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		//ONDK_WINDOW_SetShow(pstContents->m_window,TRUE);
	}
	else
	{
		pstContents->m_bDisplayState = FALSE;
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ################### Hide Fingerprint ####################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		HxLOG_Print(HxANSI_COLOR_YELLOW("[%s] ######################################################\n"), __FUNCTION__);
		//ONDK_WINDOW_SetShow(pstContents->m_window,FALSE);
	}

	return	GWM_MESSAGE_BREAK;
}


/**************************************************************************************************/
#define _____AP_Fingerprint_Public_Proc____________________________________________________
/**************************************************************************************************/

ONDK_Result_t		AP_Fingerprint_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= GWM_RET_OK;
	Fingerprint_Contents_t     	*pstContents = local_fingerprint_GetContents();

	//ONDK_Print(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);

	switch (nMessage)
	{
		case MSG_GWM_CREATE:
			ONDK_Print(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);
			eResult = local_fingerprint_MsgGwmCreate(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
		case MSG_GWM_KEYDOWN :
			//eResult= ONDK_MESSAGE_NONCONSUMED;
			eResult = local_fingerprint_MsgGwmKeyDown(pstContents, p1);
		  	break;

		case MSG_GWM_DRAW:
			ONDK_Print(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);
			HxLOG_Debug("MSG_GWM_DRAW - Set Timer Mode :[%d]\n", pstContents->bSetTimer);
			if (pstContents->bSetTimer == FALSE)
			{
				local_fingerprint_SetDuratonTimer(pstContents);
			}
			break;

		case MSG_GWM_TIMER:
			ONDK_Print(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);
			eResult = local_fingerprint_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_SIGNAL_REQUESTSTANDBY :
			eResult =  local_fingerprint_signal_RequestStandby(pstContents);
			break;

		case MSG_GWM_APK_MESSAGE:
			eResult = local_fingerprint_APKMessage(handle,p1, p2, p3);
			break;

		case MSG_SIGNAL_FINGERPRINT:
			eResult = local_fingerprint_signal_ShowFingerprint((HBOOL)p3);
			break;

		case MSG_GWM_DESTROY:
			ONDK_Print(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);
			eResult = local_fingerprint_MsgGwmDestroy(pstContents, p1, p2, p3);
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


/**************************************************************************************************/
#define _____AP_Fingerprint_End_Of_File____________________________________________________
/**************************************************************************************************/

