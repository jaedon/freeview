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
 * @file	  		nx_prism_home.c
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
#include <sys/resource.h>
#include <nx_main.h>
#include <nx_core.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________

typedef struct
{
	HINT32 	nVer;
	HINT32	nKeyboardMode;
}NxMainContents_t;
/******************************************************************************/
#define	NATIVEAPP_SIGNAL_BUF_SIZE		0x10000


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxMainContents_t 			s_NxMainContents;

static HBOOL						s_bluetoothState	= FALSE;
static HBOOL						s_bSystemInitialized = FALSE;
static HxCBUF_t						s_pstMsgBuffer = NULL;


//extern ONDK_Result_t			AP_AudioVolume_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxMainContents_t*	nx_main_GetContents(void);
static ONDK_Result_t 	nx_main_MsgGwmCreate(NxMainContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_main_timer(NxMainContents_t *pstContents, Handle_t hAct, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static HERROR			nx_main_initOutput(void);
static void 			nx_main_initApp(void);
static HERROR			nx_main_startApp(void);
static ONDK_Result_t 	nx_main_signal_receive(NxMainContents_t *pstContents, HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3);
static void 			nx_main_deliveryPacket(HINT32 size, void *pvData);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NxMainContents_t* nx_main_GetContents(void)
{
	return &s_NxMainContents;
}


static ONDK_Result_t 	nx_main_MsgGwmCreate(NxMainContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	if(pstContents == NULL)
	{
		HxLOG_Assert(pstContents);
	}

	if(pstContents)
	{
		HxSTD_MemSet(pstContents, 0, sizeof(NxMainContents_t));
	}

	return ONDK_RET_OK;
}

#if defined(CONFIG_HUMAX_CRASHREPORT)
static void		nx_main_signal_handler(int signal, siginfo_t *siginfo, void *context)
#else
static void		nx_main_signal_handler(int signal, VK_SIG_INFO_t *siginfo, void *context)
#endif
{
	//	TODO: Add release all DFB Resource.
	ONDK_Destroy();
	NX_COMMON_NOT_USED_ARGUMENT(NULL, signal, siginfo, context);
}

static ONDK_Result_t nx_main_timer(NxMainContents_t *pstContents, Handle_t hAct, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	return ONDK_RET_OK;
}

static HERROR	nx_main_initOutput(void)
{
	HERROR					hError = ERR_FAIL;
	DxAspectRatio_e			eScreenRatio = eDxASPECTRATIO_4X3;
	DxDfcSelect_e			eDispFormat = eDxDFC_SELECT_DEFAULT;
	DxResolutionSelect_e	eResolution = eDxRESOLUTION_SELECT_1080I;

	/* Get Resolution */
	hError = NX_SYSTEM_GetResolutionSelect(&eResolution);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetResolutionSelect()\n");
	}
	else
	{
		hError = NX_SYSTEM_SetResolutionSelect(eResolution);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_GetResolutionSelect() - eResolution(%d)\n", eResolution);
		}
	}

	/* Get Screen Ratio */
	hError = NX_SYSTEM_GetTvAspectRatio(&eScreenRatio);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetTvAspectRatio()\n");
	}
	else
	{
		hError = NX_SYSTEM_SetTvAspectRatio(eScreenRatio);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetTvAspectRatio() - eScreenRatio(%d)\n", eScreenRatio);
		}
	}

	/* Get Display Format */
	hError = NX_SYSTEM_GetDisplayFormat(&eDispFormat);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetDisplayFormat()\n");
	}
	else
	{
		hError = NX_SYSTEM_SetDisplayFormat(eDispFormat);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetDisplayFormat() - eDispFormat(%d)\n", eDispFormat);
		}
	}

	return	ERR_OK;
}

static void nx_main_initApp(void)
{
	/****************************************
	* subtitle window create
	*****************************************/
	HxLOG_Warning("# Create Subtitle window\n");
	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_Subtitle_Proc) ==NULL)
		ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"NX_PRISM_Subtitle_Proc", APP_OSD_DRAW_SBTL_TTX_PRIOTITY,NX_PRISM_Subtitle_Proc, HANDLE_NULL, 0, 0,0);

	/****************************************
	* NX_CORE init.
	*****************************************/
	NX_CORE_Init();

	/****************************************
	* NX_PRISM UI init.
	*****************************************/
	// GUI Color Init
	NX_PRISM_Resource_Init();
	NX_PRISM_MENUCONFIG_Init();

	/****************************************
	* enable data streamer for subttitle
	*****************************************/
	NX_PORT_MEDIA_DATASTREAMER_InitDataStreamer(nx_main_deliveryPacket);
	NX_PORT_MEDIA_DATASTREAMER_ConnectDataStreamer("dvbsubtitle");
	NX_PORT_MEDIA_DATASTREAMER_ConnectDataStreamer("mediasubtitle");
#if defined(CONFIG_MW_TELETEXT)
	NX_PORT_MEDIA_DATASTREAMER_ConnectDataStreamer("ttxpes");
	NX_PRISM_Teletext_Init();
#endif

	/****************************************
	* NX_PRISM Output Init
	*****************************************/
	//nx_main_initOutput();

	ONDK_KEY_Active(TRUE);
}

static HERROR	nx_main_startApp(void)
{
	HxLOG_Warning("\n NX_PRISM_START_Proc start!!!!!!!!!!\n\n");

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_START_Proc", APP_UIMGR_PRIORITY, NX_PRISM_START_Proc, 0, 0, 0, 0);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrAction_Proc", APP_ACTION_PRIORITY, NX_PRISM_MgrAction_Proc, 0, 0, 0, 0);

	return	ERR_OK;
}

static ONDK_Result_t nx_main_signal_receive(NxMainContents_t *pstContents, HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HCHAR	*pszProcessName = NULL;

	if(pstContents == NULL)
	{
		HxLOG_Assert(pstContents);
	}

	switch (eMsg)
	{
		case eHMSG_BOOTUPSTATE_CHANGE:
			return ONDK_MESSAGE_BREAK;

		case eHMSG_APP_READY:
			pszProcessName = HLIB_STD_GetProcessName(p1);
#if defined(CONFIG_PROD_OS_EMULATOR)
			pszProcessName = "obama";
#endif

			HxLOG_Warning("eHSIG_AppsReady Received...[%s]\n", pszProcessName);

			if (HxSTD_StrEmpty(pszProcessName) == FALSE && HLIB_STD_StrCaseCmp(pszProcessName, "obama") == 0)
			{
				if (s_bSystemInitialized == FALSE)
				{
					HxLOG_Warning("OBAMA is just launched --> try to initialize APPKIT\n");

					nx_main_initApp();
					nx_main_startApp();

					HAPI_BroadcastSignal("signal:onMainApplicationReady", 1, "i", 0);
					s_bSystemInitialized = TRUE;
				}
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_SYSSTATE_CHANGE:
			return ONDK_MESSAGE_PASS;

		case eHMSG_MAINAPP_READY:
			return ONDK_MESSAGE_PASS;

		/***********************
		* Panel
		************************/
		case eHMSG_PANEL_PRIORITY:
			// set priority
			NX_PANEL_SetAppPriority((HINT32)p1);
			if (s_bSystemInitialized == TRUE)
			{
				// clear current priority
				NX_PANEL_SetString(NULL, eDxPANEL_ALIGN_CENTER, FALSE);
				NX_PANEL_SetLedAuto();
			}
			return ONDK_MESSAGE_BREAK;

		/***********************
		* keyboard
		************************/
		case eHMSG_OSK_MODECHANGE:
			if(pstContents)
			{
				pstContents->nKeyboardMode = p1;
			}
			return ONDK_MESSAGE_CONSUMED;
		case eHMSG_OSK_SHOW_SIMPLE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SimpleKeyboard_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SimpleKeyboard_Proc, HANDLE_NULL, 0, 0, p1);
			return ONDK_MESSAGE_CONSUMED;
		case eHMSG_OSK_SHOW:
			if(pstContents)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
							NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, pstContents->nKeyboardMode, (HINT32)p1);
			}
			return ONDK_MESSAGE_CONSUMED;
		case eHMSG_OSK_DESTROY:
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_KeyBoard_Proc))
			{
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_KeyBoard_Proc);
			}
			return ONDK_MESSAGE_CONSUMED;
		case eHMSG_REMOTEAPP_STRING:
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_KeyBoard_Proc)
				|| ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SimpleKeyboard_Proc))
			{
				ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_REMOTESTING, HANDLE_NULL, p1, 0, 0);
			}
			return ONDK_MESSAGE_CONSUMED;



		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);


	return ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_main_APKMessage(NxMainContents_t *pstContents, Handle_t hAct,HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)hAct;

	switch (eAPK_Type)
	{
		default:
			break;
	}

	return ONDK_MESSAGE_PASS;
}

/******************************************************************************/
#define _________Related_HAPI_MSG_functions____________________________________________
/******************************************************************************/

static HERROR	nx_main_postMessage(HAPIMSG_e eMsg, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_HAPI_MESSAGE, (Handle_t)eMsg, nParam1, nParam2, nParam3);

	return ERR_OK;
}

//	i : pid
static HERROR	nx_main_onApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 2);

	HxLOG_Trace();

	HxLOG_Debug("[%s:%d] pid is [%s][%d]\n", __FUNCTION__, __LINE__, HxOBJECT_STR(apArgv[0]), HxOBJECT_INT(apArgv[1]));
	nx_main_postMessage(eHMSG_APP_READY, HxOBJECT_INT(apArgv[1]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}



static HERROR	nx_main_onPanelPriority(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_PANEL_PRIORITY, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onUserActionIdle(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	nx_main_postMessage(eHMSG_USER_ACTION_IDLE_NOTIFY, 0, 0, 0);

	NX_COMMON_NOT_USED_ARGUMENT(nArgc, apArgv, nRequestId, pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onInputNotify(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	nx_main_postMessage(eHMSG_INPUT_NOTIFY, 0, 0, 0);

	NX_COMMON_NOT_USED_ARGUMENT(nArgc, apArgv, nRequestId, pvUser);

	return ERR_OK;
}

//	b : Bootup
static HERROR	nx_main_onBootupStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HSystemBootupStatus_t	*pstBootStatus;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	if (s_pstMsgBuffer == NULL)
		s_pstMsgBuffer = HLIB_CBUF_New(NATIVEAPP_SIGNAL_BUF_SIZE);
	pstBootStatus = (HSystemBootupStatus_t*)HLIB_CBUF_GetBuffer(s_pstMsgBuffer, sizeof(HSystemBootupStatus_t));
	HxSTD_MemCopy(pstBootStatus, HxOBJECT_BIN_DATA(apArgv[0]), HxOBJECT_BIN_SIZE(apArgv[0]));

	nx_main_postMessage(eHMSG_BOOTUPSTATE_CHANGE, (HUINT32)pstBootStatus, 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onOSKShow(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HUINT8 *nParam = NULL;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();

	if ( HxOBJECT_INT(apArgv[0]) )
	{
		if (s_pstMsgBuffer == NULL)
		s_pstMsgBuffer = HLIB_CBUF_New(NATIVEAPP_SIGNAL_BUF_SIZE);
		nParam = (HUINT8*)HLIB_CBUF_GetBuffer(s_pstMsgBuffer, sizeof(HUINT8));
		*nParam = HxOBJECT_INT(apArgv[0]);
	}

	nx_main_postMessage(eHMSG_OSK_SHOW, (HINT32)nParam, 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onOSKDestroy(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();

	nx_main_postMessage(eHMSG_OSK_DESTROY, 0, 0, 0);

	NX_COMMON_NOT_USED_ARGUMENT(nArgc, apArgv, nRequestId, pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onOSKModeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_OSK_MODECHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onShowSimpleKeyboard(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_OSK_SHOW_SIMPLE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	s
static HERROR	nx_main_onRemoteAppString(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*pszBuffer;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	if (s_pstMsgBuffer == NULL)
		s_pstMsgBuffer = HLIB_CBUF_New(NATIVEAPP_SIGNAL_BUF_SIZE);
	pszBuffer = HLIB_CBUF_GetBuffer(s_pstMsgBuffer, HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);
	HLIB_STD_StrNCpySafe(pszBuffer, HxOBJECT_STR(apArgv[0]), HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);

	nx_main_postMessage(eHMSG_REMOTEAPP_STRING, (HINT32)pszBuffer, 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_SYSSTATE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

static void nx_main_deliveryPacket(HINT32 size, void *pvData)
{
	HINT32		len;
	HUINT8		*p = (HUINT8*)pvData;
	HCHAR		*dataType;
	HUINT8 		*tempMem = NULL;

	len = HxMACRO_Get32Bit(p);			p += 4;
	dataType = p;						p += len;
	len = size - (len + 4);

	if (HLIB_STD_StrCaseCmp(dataType, "ttxpes") == 0) //teletext ..
	{
#if 0
		if(NX_PRISM_Teletext_GetDecoderHandle() != HANDLE_NULL)
		{
			HxLOG_Print("=========================== alloc delivery packet\n");
			tempMem = HLIB_STD_MemAlloc(size);
			if (NULL != tempMem)
			{
				HxSTD_MemCopy(tempMem, pvData, size);
				ONDK_GWM_RECV_PostDataToGWM(MSG_APP_RCV_TELETEXT_PACKET, 0, (HUINT32)0, size, (HUINT32)tempMem, (void *)tempMem, HLIB_STD_MemFree_CB);
			}
		}
#endif
		NX_PRISM_Teletext_deliveryPacket((HCHAR*)dataType, len, (HUINT8 *)p);
	}
	else  //subtitle..
	{
		tempMem = HLIB_STD_MemAlloc(size);

		if (NULL != tempMem)
		{
			HxSTD_MemCopy(tempMem, pvData, size);
			ONDK_GWM_RECV_PostDataToGWM(MSG_APP_RCV_SUBTITLE_PACKET, 0, (HUINT32)0, size, (HUINT32)tempMem, (void *)tempMem, HLIB_STD_MemFree_CB);
		}
	}
}

//	i
static HERROR	nx_main_onSubtitleState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_SUBTITLE_STATE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onTeletextState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_TELETEXT_STATE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onSubtitleTypeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_SUBTITLE_TYPE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_MAINAPP_READY, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onSystemLanguageChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_MAINLANG_CHANGED, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

//	i
static HERROR	nx_main_onFontTypeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_SUBTITLE_FONTTYPE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onOtaStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	nx_main_postMessage(eHMSG_IS_OTA_CHECK, HxOBJECT_INT(apArgv[0]), 0, 0);

	NX_COMMON_NOT_USED_PARAM(nRequestId);
	NX_COMMON_NOT_USED_PARAM(pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onMsgboxShow(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	nx_main_postMessage(eHMSG_MSGBOX_SHOW, 0, 0, 0);

	NX_COMMON_NOT_USED_ARGUMENT(nArgc, apArgv, nRequestId, pvUser);

	return ERR_OK;
}

static HERROR	nx_main_onBtStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	s_bluetoothState = HxOBJECT_INT(apArgv[1]);
	nx_main_postMessage(eHMSG_BLUETOOTH_MUTE, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, nArgc, nRequestId, pvUser);

	return ERR_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_MAIN_GWMStart_Proc(HINT32 nMessage, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxMainContents_t *pstContents = nx_main_GetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s  [%d] p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, nMessage, ONDK_GWM_GwmMsg2String(nMessage), hAct, p1, p2, p3);

	switch (nMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Warning("\n\n[%s.%d]   CREATED NATIVE NX_MAIN_GWMStart_Proc !!! \n\n",__FUNCTION__,__LINE__);
			HxLOG_Debug("[%s.%d] MSG_GWM_CREATE \n",__FUNCTION__,__LINE__);
			nx_main_MsgGwmCreate(pstContents, hAct, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			return nx_main_signal_receive(pstContents, (HAPIMSG_e)hAct, p1, p2, p3);

		case MSG_GWM_APK_MESSAGE:
			return nx_main_APKMessage(pstContents, hAct, p1, p2, p3);

		case MSG_GWM_TIMER:
			return nx_main_timer(pstContents, hAct, p1, p2, p3);

		case MSG_GWM_DESTROY:
			ONDK_Error("[%s:%d] destroy app!!\r\n", __FUNCTION__, __LINE__);
			NX_PRISM_AppNaviCntxt_DeleteAllMainMenuItem();
			return ONDK_MESSAGE_BREAK;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		NX_APP_Assert(eResult == ONDK_RET_OK);
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(nMessage, hAct, p1, p2, p3);
}

#define MEMINFO_TEST		(0)

#if (MEMINFO_TEST == 1) // MOON
static void		nx_main_PrintMemInfo(void *arg)
{
	char cmd[100];

	HxSTD_memset(cmd, 0, 100);
	HxSTD_StrCpy(cmd, "chmod 775 ./home/memstat.sh");
	VK_SYSTEM_Command((const char*)cmd);

	while(1)
	{
		if (access((const char*)"/tmp/meminfo.txt", F_OK) == 0)
		{
			HxSTD_memset(cmd, 0, 100);
			HxSTD_StrCpy(cmd, "rm /tmp/meminfo.txt");
			VK_SYSTEM_Command((const char*)cmd);
		}

		HxSTD_memset(cmd, 0, 100);
		HxSTD_StrCpy(cmd, "./home/memstat.sh >> /tmp/meminfo.txt");
		VK_SYSTEM_Command((const char*)cmd);

		VK_TASK_Sleep(30 * 1000); // 30 sec
	}
}
#endif


int NATIVE_Main(HINT32 argc, HCHAR **argv)
{
#if defined(CONFIG_DEBUG)
	HLIB_DBG_Init();
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(nx_main_signal_handler);
#else
	VK_SIG_SetSigAction(SIGSEGV, nx_main_signal_handler);
	VK_SIG_SetSigAction(SIGILL, nx_main_signal_handler);
	VK_SIG_SetSigAction(SIGFPE, nx_main_signal_handler);
	VK_SIG_SetSigAction(SIGBUS, nx_main_signal_handler);
	VK_SIG_SetSigAction(SIGQUIT, nx_main_signal_handler);
#endif

	HxLOG_Warning("\n\n\n\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("## Native App start !!\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("#####################################\n");
	HxLOG_Warning("\n\n\n\n");

	/****************************************
	* ONDK init.
	*****************************************/
	HxLOG_Warning("# ONDK init\n");
	ONDK_Init(argc, argv);
	ONDK_GWM_Init();
	ONDK_GWM_RECV_RegisterLoop(APK_EVENT_Dispatch);
	ONDK_STRINGS_Init();
	ONDK_GWM_Start(NX_MAIN_GWMStart_Proc);

	/****************************************
	* HAMA Event dispatcher init.
	*****************************************/
	HxLOG_Warning("# HAPI init\n");
	HAPI_Init();
    HAPI_SetWindowStyle(ONDK_UPPER_WINDOWID, eHWS_WindowOwnControl | eHWS_WindowInputAlways, FALSE);
	HAPI_SetWindowStyle(ONDK_LOWER_WINDOWID, eHWS_WindowOwnControl, FALSE);
	//	Buffered Signal의 동작은 Connect된 시그널의 순서로 이루어 진다.
	//	StartLogo등의 제어를 위해 AppsReady를 선 등록하고, bootup state를 등록한다.
	//	Buffered Signal의 동작은 항상 유의 해야 한다. --> 등록된 순서대로 Buffered Signal이 동작하게 된다..
	HAPI_ConnectSystemSignal("hama:onReadyProcess", "si", NULL, nx_main_onApplicationReady);
	HAPI_ConnectSystemSignal("hama:onPanelPriority","i", NULL, nx_main_onPanelPriority);
	HAPI_ConnectSystemSignal("hama:onSystemUserActionIdle", NULL, NULL, nx_main_onUserActionIdle);
	HAPI_ConnectSystemSignal("hama:onInputNotify", "ii", NULL, nx_main_onInputNotify);	// notify when key inpu

	HAPI_ConnectSignal("signal:onOSKShow",            NULL, nx_main_onOSKShow);
	HAPI_ConnectSignal("signal:onOSKDestroy",   	  NULL, nx_main_onOSKDestroy);
	HAPI_ConnectSignal("signal:onOSKModeChanged",     NULL, nx_main_onOSKModeChanged);
	HAPI_ConnectSignal("signal:onShowSimpleKeyboard", NULL, nx_main_onShowSimpleKeyboard);
	HAPI_ConnectSignal("signal:onRemoteAppString",    NULL, nx_main_onRemoteAppString);
	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, nx_main_onMainApplicationReady);

//	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, nx_main_onSystemStateChanged);
//	HAPI_ConnectSignal("signal:onSubtitleState",	  NULL, nx_main_onSubtitleState);
//	HAPI_ConnectSignal("signal:onTeletextState",	  NULL, nx_main_onTeletextState);
//	HAPI_ConnectSignal("signal:onSubtitleTypeChanged",NULL, nx_main_onSubtitleTypeChanged);
//	HAPI_ConnectSignal("signal:onSystemLanguageChanged", NULL, nx_main_onSystemLanguageChanged);
//	HAPI_ConnectSignal("signal:onFontTypeChanged", NULL, nx_main_onFontTypeChanged);
//	HAPI_ConnectSignal("signal:onOtaStateChanged", NULL, nx_main_onOtaStateChanged);
//	HAPI_ConnectSignal("signal:onMsgboxShow", NULL, nx_main_onMsgboxShow);
//	HAPI_ConnectSignal("signal:onBtStateChanged", NULL, nx_main_onBtStateChanged);
//	HAPI_ConnectSignal("signal:onBootupStateChanged", NULL, nx_main_onBootupStateChanged);

#if (MEMINFO_TEST == 1) // MOON
	{
		unsigned long tid;
		VK_TASK_Create(nx_main_PrintMemInfo, 70, SIZE_64K, "nxMainMemInfoPrinter", (void*)NULL, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}
#endif

	/****************************************
	* Secure init. - Drop Root Previleges
	*****************************************/
	HLIB_EXTRA_DropRootPrivileges("nativeapp");

	HxLOG_Warning("# Native App Initializing DONE\n");

#if defined(CONFIG_PROD_OS_EMULATOR)
	{
		// send signal to set obama ready on emulator
		HINT32 nSrzr = 0;

		nSrzr = HLIB_SERIALIZER_Open();
		HxSERIALIZER_String(nSrzr, "obama");
		HxSERIALIZER_Integer(nSrzr, 2);
	//	HLIB_SERIALIZER_Close(nSrzr);
		nx_main_onApplicationReady(2, nSrzr, 0, NULL);

		CMD_KeyTask(NULL);
	}
#else
	while (1)
	{
		VK_TASK_Sleep(100000);
	}

	ONDK_GWM_Destroy();
	ONDK_Destroy();
#endif
	return 0;
}
