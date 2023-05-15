/**
	@file     mw_sys.c
	@brief    IO device 및 전원 관련 모듈을 제어하는 service

	Description: \n
	Module: MW/SYS			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <pal_panel.h>
#include <pal_power.h>
#include <pal_key.h>
#include <pal_sys.h>
#include <pal_param.h>
#include <pal_output.h>
#include <pal_crypt.h>

#include <napi_netconf.h>

#include <svc_sys.h>
#include <svc_output.h>

#include <psvc_sys.h>

#include <tapi.h>
#include <uapi.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_FP_AUTO_DISABLE)
#define	FP_DISABLE_WAITING_TIME			40		//	4sec
#define	FP_FADING_DIMMING_LEVEL			10
#endif

#define MAX_PANEL_BUFFER_LEN				256

#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD)
#define	PAN_STR_SEM_GET()				VK_SEM_Get(s_ulPanelStringSemId)
#define	PAN_STR_SEM_RELEASE()			VK_SEM_Release(s_ulPanelStringSemId)
#else
#define	PAN_STR_SEM_GET()
#define	PAN_STR_SEM_RELEASE()
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	PanelScrType_NoScr,
	PanelScrType_Forward,
	PanelScrType_Backward,
	PanelScrType_ForwardOnce,
	PanelScrType_Max

}SvcSys_PanelScrType_e;

typedef struct
{
	HUINT32		priority;
	HUINT8		panelString[MAX_PANEL_BUFFER_LEN+1];
}SvcSys_PanelPriority_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD)
STATIC HUINT32			s_ulPanelTaskId = 0;
STATIC HUINT32			s_ulPanelStringSemId = 0;
#endif

STATIC HUINT8			s_aucDisableAutoDisable;
//STATIC HUINT8 			s_szStringBuffer[MAX_PANEL_STR_PRIORITY][MAX_PANEL_BUFFER_LEN+1];
STATIC HUINT8 			s_szPanelString[MAX_PANEL_BUFFER_LEN+1];
#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD) || defined(CONFIG_MW_SYS_7SEG)
STATIC HBOOL			s_bTimeDisplay = FALSE;
#endif

STATIC HULONG			s_ulLedBlinkingTaskId = 0;
STATIC HULONG			s_ulBlinkingMsgQId = 0;
STATIC HULONG			s_ulBlinkingTimer = 0;

STATIC WakeUpSecondReason_t s_wakeUpSecondReason = eWAKEUP_FOR_UNDECIDED;

STATIC PostMsgToMgrCb_t	s_fnSvcSystemPostMsgToMgr;


#if	defined(CONFIG_FP_AUTO_DISABLE)
STATIC HBOOL			s_bPanelDimmingUpdate = FALSE;
#endif

STATIC	SvcSys_LedInform_t	s_PanelLedInform[NUM_MAX_DxLED];

STATIC	HxList_t			*s_PanelPriorityList	=	NULL;

#if defined(SUPPORT_PAL_PARAM)
STATIC HBOOL 				s_bPalParamEnabled = FALSE;
#define	USE_PAL_PARAM()		s_bPalParamEnabled
#else
#define	USE_PAL_PARAM()		FALSE
#endif


// RGB LED Related values
STATIC HUINT32					s_ulRgbBlinkTaskId = 0;
STATIC HULONG					s_ulRgbBlinkInterval = 700;

STATIC SvcSys_RgbLedInfo_t		*s_stRgbLedList = NULL;
STATIC HUINT32					s_ulRgbLedNum = 0;

typedef HERROR (*pf_rgbled_color)(DxLedId_e, HBOOL);
STATIC pf_rgbled_color	s_pfLedColor[eDxRGBLED_COLOR_MAX] =
{
	SVC_SYS_SetRGBLedNone,
	SVC_SYS_SetRGBLedRed,
	SVC_SYS_SetRGBLedGreen,
	SVC_SYS_SetRGBLedBlue,
	SVC_SYS_SetRGBLedYellow
};

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC void		svc_sys_LedBlinkingTimerCallback(unsigned long TimerId, void *params);
STATIC HERROR	svc_sys_LedStartBlinking(void);
STATIC void		svc_sys_LedBlink(DxLedId_e eLedId);
STATIC void		svc_sys_LedBlinkSetInfo(void);
STATIC HBOOL		svc_sys_LedBlinkSupported(void);
STATIC HBOOL		svc_sys_LedBlinkKeySupported(HUINT32 ulKeyCode);
STATIC HERROR	svc_sys_SetLedInform(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bUsedCus);
STATIC HERROR	svc_sys_SetLedInformByCustom(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bUsedCus);

STATIC HBOOL		svc_sys_FindPanelStringByPriorityCallback(void *pvUserData, void *pvListData);
STATIC HERROR	svc_sys_GetPanelStringByPriority(HUINT32 priority, SvcSys_PanelPriority_t **ppstObject);
STATIC HINT32 	svc_sys_SortPanelStringMapByPriority (void *pData1, void *pData2);
STATIC HERROR	svc_sys_AddPriorityPanelString(HUINT32 nPriority, HUINT8 *pszStr);
STATIC HUINT8 * svc_sys_RegisterPanelString(HUINT32 nPriority, HUINT8 *pszStr);
STATIC HERROR	svc_sys_RemoveAllPanelPriorityMap(void);
STATIC HERROR	svc_sys_InitOctoPanelPriority(void);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HINT32 svc_sys_PostMsgToMgr (HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (s_fnSvcSystemPostMsgToMgr)
	{
		return (*s_fnSvcSystemPostMsgToMgr)(msg, hAct, p1, p2, p3);
	}

	return 0;
}

STATIC void svc_sys_RegisterPostMsgToMgrCallBack (PostMsgToMgrCb_t pfncPostMsgCallback)
{

	if(pfncPostMsgCallback == NULL)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_fnSvcSystemPostMsgToMgr = pfncPostMsgCallback;
}

#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
#define DEF_STR_SIZE		(1024*5)
#define DEF_TRIGGER_STR		"DIAG_APP"

STATIC void svc_sys_setDefaultLaunchAppMode(void)
{
	HxFILE_t pxFile;
	LaunchAppMode_t eMode = eLAUNCH_APP_MAIN;	// default value(NOT NUMA mode)

	pxFile = HLIB_FILE_Open("/proc/cmdline", "r");
	if (pxFile != NULL)
	{
		HUINT32 ulSize = DEF_STR_SIZE;
		HCHAR 	*szStr = NULL;

		szStr = (HCHAR*)HLIB_STD_MemAlloc(ulSize);
		if (szStr != NULL)
		{
			HUINT8 	*pucRet = NULL;
			pucRet = HLIB_FILE_GetString(pxFile, szStr, (HINT32)ulSize);
			if (pucRet != NULL)
			{
				HxSTR_t	*pxStr;
				pxStr = HLIB_STR_New((const HUINT8*)szStr);
				if (pxStr != NULL)
				{
					if (HLIB_STR_Find(pxStr, (const HUINT8*)DEF_TRIGGER_STR) != -1)
					{
						eMode = eLAUNCH_APP_DIAGNOSTICS; // in case of NUMA mode
					}

					HLIB_STR_Delete(pxStr);
				}
			}
			HLIB_STD_MemFree(szStr);
		}

		(void)HLIB_FILE_Close(pxFile);
	}

	(void)SVC_SYS_SetLaunchAppMode(eMode);
}
#endif



#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Power_On_Off___________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void SVC_SYS_PwrInit(void)
{
	PAL_PWR_Init();
}

HERROR SVC_SYS_GetWakeUpReason(WakeUpReason_t *pWakeUpReason)
{
	return PAL_PWR_GetWakeUpReason(pWakeUpReason);
}

HERROR SVC_SYS_SetWakeUpReason(WakeUpReason_t wakeUpReason)
{
	return PAL_PWR_SetWakeUpReason(wakeUpReason);
}

HERROR SVC_SYS_GetWakeUpSecondReason(WakeUpSecondReason_t *pWakeUpSecondReason)
{
	*pWakeUpSecondReason = s_wakeUpSecondReason;
	return ERR_OK;
}

HERROR SVC_SYS_SetWakeUpSecondReason(WakeUpSecondReason_t wakeUpSecondReason)
{
	s_wakeUpSecondReason = wakeUpSecondReason;
	return ERR_OK;
}

HERROR SVC_SYS_TurnOffMainBoard(HBOOL bPowerSaveMode)
{
	return PAL_PWR_TurnOffMainBoard(bPowerSaveMode);
}

HERROR SVC_SYS_SetRtcTime(HUINT32 ulUnixTime)
{
	HERROR 	hErr;
	HINT32	offsetTime = 0;

	hErr = HLIB_DATETIME_GetTimeOffsetWithDst(&offsetTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	ulUnixTime += (offsetTime * SECONDS_PER_MIN);

	return PAL_PWR_SetRtcTime(ulUnixTime);
}

// 1980년 1월 1일 6시 30분 이전이면 무조건 잘못된 시간으로 친다.
#define MIN_RTC_TIME				315556200

// Case of NOMICOM, If Obama does not set RTCTIME by DI_RTC_SetCurrentTime(), DI_RTC_GetCurrentTime() returns Invalid time and no error.
// Before DI Issue modify, Only Use this function in mgr_init_action.c
HERROR SVC_SYS_GetRtcTime(HUINT32 *pulUnixTime)
{
	HERROR	hErr;
	HINT32	offsetTime = 0;

	hErr = PAL_PWR_GetRtcTime(pulUnixTime);
	if ((hErr != ERR_OK) || (*pulUnixTime < MIN_RTC_TIME))
	{
		HxLOG_Warning("\n\n\n");
		return ERR_FAIL;
	}


	hErr = HLIB_DATETIME_GetTimeOffsetWithDstAtSpecificLocalTime(*pulUnixTime, &offsetTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	if ( *pulUnixTime <= (offsetTime * SECONDS_PER_MIN) )
	{
		return ERR_OK;
	}


	*pulUnixTime -= (offsetTime * SECONDS_PER_MIN);


	if (TAPI_SetTime(TxSOURCE_SET_BY_MICOM, *pulUnixTime) != ERR_OK)
		HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", *pulUnixTime);

	return ERR_OK;
}

HERROR SVC_SYS_SetWakeUpTime(HUINT32 ulUnixTime)
{
	HERROR 	hErr;
	HINT32	offsetTime = 0;

	hErr = HLIB_DATETIME_GetTimeOffsetWithDst(&offsetTime);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	ulUnixTime += (offsetTime * SECONDS_PER_MIN);

	return PAL_PWR_SetWakeUpTime(ulUnixTime);
}

HERROR SVC_SYS_EnableWakeUpTime(void)
{
	return PAL_PWR_EnableWakeUpTime();
}

HERROR SVC_SYS_DisableWakeUpTime(void)
{
	return PAL_PWR_DisableWakeUpTime();
}

HERROR SVC_SYS_Deactivate_SC(void)
{
	return PAL_PWR_Deactivate_SC();
}

HERROR SVC_SYS_WIFIPower(HBOOL bOnOff)
{
	return PAL_PWR_WIFIPower(bOnOff);
}

HERROR SVC_SYS_MountNandFlash(void)
{
	return PAL_PWR_MountNandFlash();
}

HERROR SVC_SYS_UmountNandFlash(void)
{
	return PAL_PWR_UmountNandFlash();
}

HERROR SVC_SYS_Reboot(void)
// Warning : only use in mgr_action. Other modules have to use the MGR_ACTION_RebootSystem()
{
	//PAL_OUT_SetAvEnable(FALSE);
	//SVC_FS_UnMountAllDevice();

	return PAL_PWR_Reboot();
}

HERROR SVC_SYS_SetWakeupRcuKey(HUINT32 keyGroup)
{
	return PAL_PWR_SetWakeupRcuKey(keyGroup);
}

HERROR SVC_SYS_ClearWakeupRcuKey(void)
{
	return PAL_PWR_ClearWakeupRcuKey();
}

HERROR SVC_SYS_PrepareFastBootStandby(void)
{
	return PAL_PWR_PrepareFastBootStandby();
}
HERROR SVC_SYS_SetFastBootStandby(HINT32 nSleepMode,HINT32 nBootMode,HBOOL bPowerSavingStandby)
{
	return PAL_PWR_SetFastBootStandby(nSleepMode,nBootMode,bPowerSavingStandby);
}
HERROR SVC_SYS_CancelFastBootStandby(void)
{
	return PAL_PWR_CancelFastBootStandby();
}








HERROR SVC_SYS_SetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel)
{
	return PAL_PANEL_SetColdBootCondition (bDisplayVideo, bViewPanel);
}

HERROR SVC_SYS_SetWakeOnLAN(HBOOL bOnOff)
{
	HERROR hRet = ERR_OK;
#if 0//defined(CONFIG_SUPPORT_NIMA)	// Network Driver is not seperated.
	hRet = NAPI_NetConf_SetWOL(bOnOff);
#else
#if !defined(CONFIG_USES_NFSROOT)	// The case of NFSROOT, WOL will disconnet the nfs line.
		hRet = PAL_PWR_SetWakeOnLAN(bOnOff);
#endif
#endif
	return hRet;
}

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Front_Panel_LED_VFD___________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

HERROR SVC_SYS_GetPanelCapability(Panel_Capability_t *pstPanelCapa)
{
	return PAL_PANEL_GetCapability(pstPanelCapa);
}

void SVC_SYS_ClearLed(void)
{
	PAL_PANEL_ClearLed();
}

void SVC_SYS_ClearLedByCustom(void)
{
	int i = 0;
	for ( i=0; i < NUM_MAX_DxLED; i++)
	{
		s_PanelLedInform[i].bUsedByCustom	= TRUE;
	}
	SVC_SYS_ClearLed();
}

HERROR SVC_SYS_SetLedOnOff(DxLedId_e ledId, HBOOL bOnOff)
{
	if ( s_PanelLedInform[ledId].bUsedByCustom )		// octo 가 아닌 다른곳에서 컨트롤 하는 거라면...
	{
		return ERR_OK;
	}

	if ( ( eDxLED_ID_DOLBY == ledId) && ( FALSE == bOnOff ) )
	{
		// DOLBY끌때 DOLBY+도 끈다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, bOnOff);
	}
	else if ( ( eDxLED_ID_DOLBY == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY켤때 DULBY+는 끈다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, FALSE);
	}
	else if ( ( eDxLED_ID_DOLBY_PLUS == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY+킬때 DOLBY도 킨다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY, bOnOff);
	}
	if (PAL_PANEL_SetLedOnOff(ledId, bOnOff) == ERR_OK)
	{
		return svc_sys_SetLedInform(ledId, bOnOff, FALSE);			// led 상태 저장.
	}

	return ERR_OK;
}

HERROR SVC_SYS_SetLedOnOffOnStandbyMode(DxLedId_e ledId, HBOOL bOnOff)
{
	if ( s_PanelLedInform[ledId].bUsedByCustom )		// octo 가 아닌 다른곳에서 컨트롤 하는 거라면...
	{
		return ERR_OK;
	}

	if ( ( eDxLED_ID_DOLBY == ledId) && ( FALSE == bOnOff ) )
	{
		// DOLBY끌때 DOLBY+도 끈다.
		PAL_PANEL_SetLedOnOffOnStandbyMode (eDxLED_ID_DOLBY_PLUS, bOnOff);
	}
	else if ( ( eDxLED_ID_DOLBY == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY켤때 DULBY+는 끈다.
		PAL_PANEL_SetLedOnOffOnStandbyMode (eDxLED_ID_DOLBY_PLUS, FALSE);
	}
	else if ( ( eDxLED_ID_DOLBY_PLUS == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY+킬때 DOLBY도 킨다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY, bOnOff);
	}
	if (PAL_PANEL_SetLedOnOffOnStandbyMode(ledId, bOnOff) == ERR_OK)
	{
		return svc_sys_SetLedInform(ledId, bOnOff, FALSE);			// led 상태 저장.
	}

	return ERR_OK;
}


HERROR SVC_SYS_GetLedStatus(DxLedId_e eLedId, HBOOL *pbOnOff)
{
	return PAL_PANEL_GetLedStatus(eLedId, pbOnOff);ERR_OK;
}

HERROR SVC_SYS_SetLedOnOffByCustom(DxLedId_e ledId, HBOOL bOnOff)
{
	if ( ( eDxLED_ID_DOLBY == ledId) && ( FALSE == bOnOff ) )
	{
		// DOLBY끌때 DOLBY+도 끈다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, bOnOff);
	}
	else if ( ( eDxLED_ID_DOLBY == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY켤때 DULBY+는 끈다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, FALSE);
	}
	else if ( ( eDxLED_ID_DOLBY_PLUS == ledId) && ( TRUE == bOnOff ) )
	{
		// DOLBY+킬때 DOLBY도 킨다.
		PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY, bOnOff);
	}

	if (PAL_PANEL_SetLedOnOff(ledId, bOnOff) == ERR_OK)
	{
		// custom (user) 셋팅이라는 정보를 true 로 설정.
		return svc_sys_SetLedInformByCustom(ledId, bOnOff, TRUE);			// led 상태 저장.
	}

	return ERR_OK;
}

HERROR SVC_SYS_SetLedAuto(void)
{
	int i = 0;
	for ( i = 0; i < NUM_MAX_DxLED; i++)
	{
		s_PanelLedInform[i].bUsedByCustom	= FALSE;
		SVC_SYS_SetLedOnOff(i, s_PanelLedInform[i].bOnOff);
	}
	return ERR_OK;
}

HERROR SVC_SYS_SetCategoryInform(DxPanelCategory_e eCategory)
{
	int i = 0;
	for ( i = 0; i < NUM_MAX_DxLED; i++)
	{
		if ( s_PanelLedInform[i].category & eCategory )
		{
			if ( ( eDxLED_ID_DOLBY == i) && ( FALSE == s_PanelLedInform[i].bOnOff ) )
			{
				// DOLBY끌때 DOLBY+도 끈다.
				PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, s_PanelLedInform[i].bOnOff);
			}
			else if ( ( eDxLED_ID_DOLBY == i) && ( TRUE == s_PanelLedInform[i].bOnOff ) )
			{
				// DOLBY켤때 DULBY+는 끈다.
				PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY_PLUS, FALSE);
			}
			else if ( ( eDxLED_ID_DOLBY_PLUS == i) && ( TRUE == s_PanelLedInform[i].bOnOff ) )
			{
				// DOLBY+킬때 DOLBY도 킨다.
				PAL_PANEL_SetLedOnOff (eDxLED_ID_DOLBY, s_PanelLedInform[i].bOnOff);
			}

			PAL_PANEL_SetLedOnOff(i, s_PanelLedInform[i].bOnOff);

		}
	}
	return ERR_OK;
}

HERROR SVC_SYS_SetLedBlink(DxLedId_e ledId, HUINT32 delay)
{
	return PAL_PANEL_SetLedBlink(ledId, delay);
}

HERROR SVC_SYS_StartBlink(HUINT32 ulKeyCode, KEY_PressMode_e eKeyPressMode, KEY_Device_t eKeyDevice)
{
	HBOOL	bKeySupport;

#if	defined(CONFIG_FP_AUTO_DISABLE)
	PAN_STR_SEM_GET();
	s_bPanelDimmingUpdate = TRUE;
	PAN_STR_SEM_RELEASE();
#endif

	bKeySupport = svc_sys_LedBlinkKeySupported(ulKeyCode);
	if (bKeySupport == FALSE)
	{
		return FALSE;
	}

	if (eKeyPressMode == eKeyPress_DOWN || eKeyPressMode == eKeyPress_REPEAT)
	{
		if (eKeyDevice == eKeyDevice_RCU || eKeyDevice == eKeyDevice_FRONT)
		{
			return svc_sys_LedStartBlinking();
		}
	}

	return FALSE;
}

HERROR SVC_SYS_SetPowerLedUse(HBOOL bOnOff)
{
	return PAL_PANEL_SetPowerLedUse(bOnOff);
}

HERROR SVC_SYS_SetPowerLedState(PowerLedState_t ledState)
{
	return PAL_PANEL_SetPowerLedState(ledState);
}

HERROR SVC_SYS_SetPanelDim(PanelDimmingLevel_t dimLevel)
{
	return PAL_PANEL_SetDim(dimLevel);
}

HERROR SVC_SYS_GetPanelDimLevel(PanelDimmingLevel_t *peDimLevel)
{
	return PAL_PANEL_GetDimLevel(peDimLevel);
}

HERROR SVC_SYS_SetPanelDimLevel(PanelDimmingLevel_t eDimLevel)
{
	return PAL_PANEL_SetDimLevel(eDimLevel);
}

HERROR SVC_SYS_SetPanelPower(HBOOL bOnOff)
{
	return PAL_PANEL_SetPower(bOnOff);
}

HERROR SVC_SYS_SetPanelTimeDisplay(HBOOL bOnOff)
{
#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD) || defined(CONFIG_MW_SYS_7SEG)
	HERROR	hErr	= ERR_FAIL;

	PAN_STR_SEM_GET();
	s_bTimeDisplay = bOnOff;
	hErr = PAL_PANEL_SetTimeDisplay(bOnOff);
	PAN_STR_SEM_RELEASE();

	if(bOnOff == FALSE)		// SetTimeDisplay(Off) clean the panel.
	{
		PAL_PANEL_DisplayString((HUINT8 *)s_szPanelString);
	}

	return hErr;
#else
	return ERR_OK;
#endif
}

HERROR SVC_SYS_GetPanelMaxStringLen(HUINT32 *pulMaxLen)
{
	return PAL_PANEL_GetMaxStringLen(pulMaxLen);
}

HERROR	SVC_SYS_DisablePanelAutoDisableOneTime(void)
{
	s_aucDisableAutoDisable = 1;

	return ERR_OK;
}

#if 0
HERROR	MW_SYS_AddPanelStringPriority(HUINT32 nPriority, HUINT8 *pszStr)
{
	svc_sys_AddPriorityPanelString(nPriority, pszStr);
	return ERR_OK;
}
#endif

HERROR SVC_SYS_DisplayPanelString(HINT32 nPriority, HUINT8 *pszStr, DxPanelAlign_e eAlign)
{
#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD) || defined(CONFIG_MW_SYS_7SEG)
	HUINT8			szTempString[MAX_PANEL_BUFFER_LEN + 1] = {0,};
	HUINT8			*pszDisplayStr = NULL;

	PAN_STR_SEM_GET();
	if(pszStr == NULL)
	{
		HxLOG_Debug("SVC_SYS_DisplayPanelString is NULL\n");
	}
	else
	{
		HxLOG_Debug("SVC_SYS_DisplayPanelString is %s\n", pszStr);
	}

	if (pszStr != NULL && pszStr[0] != '\0')
	{
		HLIB_STD_StrUtf8NCpy(szTempString, pszStr, MAX_PANEL_BUFFER_LEN + 1);

		// Panel에 표시하려는 pszStr은 반드시 UTF-8 이어야 한다.
		// 300-468 의 기본에 따라 앞에 0x15 가 와도 되지만 그 이후로는 반드시 UTF-8 이어야 한다.

		// 이미 UTF-8 인데,, 변환이 필요한지.. 일단 7SEG 만 적용.
#if !defined(CONFIG_MW_SYS_7SEG)
		{
			HERROR			hError = ERR_FAIL;

			hError = PAL_PANEL_ConvertStringToPanelString(szTempString, eAlign);
			if(hError != ERR_OK)
			{
				HxLOG_Info("[SVC_SYS_DisplayPanelString] PAL_PANEL_ConvertStringToPanelString fail !!\n");
				goto EXIT_MWSYS_FUNC;
			}
		}
#endif
	}
	else
	{
		szTempString[0] = '\0';
	}
	pszDisplayStr = svc_sys_RegisterPanelString(nPriority, szTempString);

	if (MWC_UTIL_DvbStrcmp((char*)s_szPanelString, (char*)pszDisplayStr) == 0)
	{
		HxLOG_Info("[%s.%d] s_szPanelString same !!\n",__FUNCTION__,__LINE__);
		goto EXIT_MWSYS_FUNC;
	}
	HLIB_STD_StrUtf8NCpy((char*)s_szPanelString, (char*)pszDisplayStr, MAX_PANEL_BUFFER_LEN + 1);
#if defined(CONFIG_MW_SYS_7SEG)
	PAL_PANEL_DisplayString((HUINT8 *)s_szPanelString);
#endif

EXIT_MWSYS_FUNC :

	PAN_STR_SEM_RELEASE();
#endif
	return ERR_OK;

}

HERROR SVC_SYS_ClearPanelString(HINT32 nPriority)
{
	s_szPanelString[0] = '\0';

	return SVC_SYS_DisplayPanelString(nPriority, s_szPanelString, eDxPANEL_ALIGN_LEFT);
}
#if 0
HERROR SVC_SYS_ClearAllPanelString(void)
{
	HUINT32		i;

	PAN_STR_SEM_GET();

	for(i = 0; i < MAX_PANEL_STR_PRIORITY; i++)
	{
		s_szStringBuffer[i][0] = '\0';
	}

	VK_memset32(s_szPanelString, 0x00, MAX_PANEL_BUFFER_LEN + 1);
	s_szPanelString[0] = '\0';

#if defined(CONFIG_MW_SYS_7SEG)
	PAL_PANEL_DisplayString((HUINT8 *)s_szPanelString);
#endif

	PAN_STR_SEM_RELEASE();

	return ERR_OK;
}
#endif

HERROR SVC_SYS_ClearAllPanelString(void)
{
#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD) || defined(CONFIG_MW_SYS_7SEG)
	HINT32			i;
	HxList_t		*pList = NULL;
	SvcSys_PanelPriority_t	*pData = NULL;

	PAN_STR_SEM_GET();

	for ( i = 0; i < HLIB_LIST_Length(s_PanelPriorityList); i++)
	{
		pList = HLIB_LIST_GetListItem(s_PanelPriorityList, i);
		if(pList)
		{
			pData = (SvcSys_PanelPriority_t*)HLIB_LIST_Data(pList);
			HxSTD_MemSet(pData->panelString, '\0', MAX_PANEL_BUFFER_LEN+1);
		}
	}

	VK_memset32(s_szPanelString, 0x00, MAX_PANEL_BUFFER_LEN + 1);
	s_szPanelString[0] = '\0';

#if defined(CONFIG_MW_SYS_7SEG)
	PAL_PANEL_DisplayString((HUINT8 *)s_szPanelString);
#endif

	PAN_STR_SEM_RELEASE();
#endif

	return ERR_OK;
}

HERROR SVC_SYS_DestroyAllPanelString(void)
{
	return svc_sys_RemoveAllPanelPriorityMap();
}

HERROR SVC_SYS_SetLedVfdDimmLevel(DimmingLevel_e level)
{
#if defined(CONFIG_MW_SYS_VFD)
	return PAL_PANEL_SetDimmingLevel(level);
#else
	return ERR_FAIL;
#endif
}
HERROR SVC_SYS_SetLedDimmLevel(DimmingLevel_e level)
{
	PAL_PANEL_SetDimmingLevel(level);
	return ERR_OK;
}

#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD)

#if	defined(CONFIG_EASY_ACCESS_FPTASK)

STATIC PanelDisplayType_t	s_ePanelDispType = ePANEL_DISP_AutoDisable;

STATIC	void	local_sys_panelTask(void *pParam)
{
	HBOOL		bUpdateDisplay;
	HUINT8		szScrollString[MAX_PANEL_BUFFER_LEN + 1];
	HUINT8		szLastString[MAX_PANEL_BUFFER_LEN + 1];
	HUINT8		szTempString[MAX_PANEL_BUFFER_LEN * 2 + 1];
	HINT32		nScrollVector, nWaitDelay, nDimmingWaitingValue;
	HUINT32		ulMaxPanelStringLen;
	HUINT32		ulPanelStringLen, ulScrollPosition;
	SvcSys_PanelScrType_e	eScrollType;

	PAL_PANEL_GetMaxStringLen(&ulMaxPanelStringLen);
	eScrollType = PanelScrType_NoScr;
	szScrollString[0] = szTempString[0] = 0;

	while (1)
	{
		VK_TASK_Sleep(1);
		PAN_STR_SEM_GET();

		if (MWC_UTIL_DvbStrlenStrOnly(s_szPanelString) == 0)
		{
			szLastString[0] = 0;
			PAN_STR_SEM_RELEASE();
			continue;
		}

		bUpdateDisplay = FALSE;

		//	same string
		if (MWC_UTIL_DvbStrncmp(s_szPanelString, szLastString, MAX_PANEL_BUFFER_LEN) == 0)
		{
			//	go on scroll
			switch (eScrollType)
			{
				case	PanelScrType_ForwardOnce:
				case	PanelScrType_Forward:
					nScrollVector = 1;
					break;
				case	PanelScrType_Backward:
					nScrollVector = -1;
					break;
				case	PanelScrType_NoScr:
				default:
					nScrollVector = 0;
					break;
			}

			ulScrollPosition = (ulScrollPosition + nScrollVector) % ulPanelStringLen;
#ifdef	CONFIG_PANEL_SCROLL_BACKWARD
			if (eScrollType == PanelScrType_Forward)
			{
				if ((ulScrollPosition + ulMaxPanelStringLen) >= ulPanelStringLen)
					eScrollType = PanelScrType_Backward;
			}
			if (eScrollType == PanelScrType_Backward)
			{
				if (ulScrollPosition == 0)
					eScrollType = PanelScrType_Forward;
			}
#endif
			strcpy(szScrollString, szTempString + ulScrollPosition);
			szScrollString[ulPanelStringLen - ulScrollPosition] = 0;
			HxSTD_memset(szScrollString + ulPanelStringLen - ulScrollPosition, 0, MAX_PANEL_BUFFER_LEN - ulPanelStringLen - ulScrollPosition);

			if (eScrollType == PanelScrType_ForwardOnce)
			{
				if (ulScrollPosition == (MWC_UTIL_DvbStrlenStrOnly(s_szPanelString) + 3))
				{
					eScrollType = PanelScrType_NoScr;
				}
			}

			if (nScrollVector)
			{
//				HxLOG_Print("Update Scroll : [%s]\n", szScrollString);
				bUpdateDisplay = TRUE;
			}
		} else
		{
			eScrollType = PanelScrType_NoScr;
			nScrollVector = 0;

			if (MWC_UTIL_DvbStrlenStrOnly(s_szPanelString) > ulMaxPanelStringLen)
			{
				nScrollVector = 1;
				eScrollType = PanelScrType_Forward;
			}

			memcpy(szLastString, s_szPanelString, MAX_PANEL_BUFFER_LEN);

			memset(szTempString, 0, MAX_PANEL_BUFFER_LEN * 2);

			ulPanelStringLen = MWC_UTIL_DvbStrlenStrOnly(s_szPanelString);
			MWC_UTIL_DvbStrcpyStrOnly(szTempString, s_szPanelString);

			if (eScrollType == PanelScrType_ForwardOnce)
			{
				strcat(szTempString, "   ");
				MWC_UTIL_DvbStrcpyStrOnly(szTempString + ulPanelStringLen + 3, s_szPanelString);
				ulPanelStringLen = strlen(szTempString);
			} else
				MWC_UTIL_DvbStrcpyStrOnly(szTempString + ulPanelStringLen, s_szPanelString);

			ulScrollPosition = 0;
			HxSTD_memset(szScrollString, 0, MAX_PANEL_BUFFER_LEN + 1);
			strcpy(szScrollString, s_szPanelString);
			szScrollString[ulMaxPanelStringLen] = 0;

#if	defined(CONFIG_FP_AUTO_DISABLE)
			if (!s_aucDisableAutoDisable)
				s_bPanelDimmingUpdate = TRUE;

			s_aucDisableAutoDisable = 0;
#endif

//			HxLOG_Print("new string : [%s]\n", szScrollString);

			bUpdateDisplay = TRUE;
		}

		PAN_STR_SEM_RELEASE();

		if (bUpdateDisplay)
			PAL_PANEL_DisplayString(szScrollString);

		if (ulScrollPosition == 0)
			nWaitDelay = 20;
		else
			nWaitDelay = 6;

		while (nWaitDelay > 0)
		{
			PAN_STR_SEM_GET();

			{
				if (s_ePanelDispType == ePANEL_DISP_AutoDisable)
				{
#if	defined(CONFIG_FP_AUTO_DISABLE)
					STATIC	PanelDimmingLevel_t		eCurDimLevel, eTargetDimLevel;

					if (s_bPanelDimmingUpdate)
					{
						nDimmingWaitingValue = FP_DISABLE_WAITING_TIME;			//	10sec
						s_bPanelDimmingUpdate = FALSE;

						eCurDimLevel = ePANEL_DIM_Level100;
						PAL_PANEL_GetDimLevel(&eTargetDimLevel);
						//	KEY Repeat 동작에 무리를 줌...
						PAL_PANEL_SetDim(ePANEL_DIM_Level100);
						if (MWC_UTIL_DvbStrlenStrOnly(s_szPanelString) > ulMaxPanelStringLen)
						{
							eScrollType = PanelScrType_ForwardOnce;
							ulScrollPosition = 0;
						}
	//					HxLOG_Print("Dimming Update\n");
					} else
					if (eScrollType == PanelScrType_NoScr)						//	scroll이 없을경우...
					{
						nDimmingWaitingValue--;
						if ((nDimmingWaitingValue < 0) && (nDimmingWaitingValue >= -FP_FADING_DIMMING_LEVEL))
						{
	//						HxLOG_Print("Dimming is going : [%d]\n", nDimmingWaitingValue);
							PAL_PANEL_SetDim(eTargetDimLevel + ((eCurDimLevel - eTargetDimLevel) / FP_FADING_DIMMING_LEVEL) * (FP_FADING_DIMMING_LEVEL + nDimmingWaitingValue));
						}
					}
#endif
				}
			}

			if (strncmp(szLastString, s_szPanelString, MAX_PANEL_BUFFER_LEN))
			{
//				HxLOG_Print("New String encounter!!\n");
				PAN_STR_SEM_RELEASE();
				break;
			}
			PAN_STR_SEM_RELEASE();
			nWaitDelay--;
			VK_TASK_Sleep(100);
		}
	}
}

#else

STATIC void		local_sys_LedInformationInit(void)
{
	int i=0;
	for (i=0; i < NUM_MAX_DxLED; i++)
	{
		s_PanelLedInform[i].category	= eDxPANEL_CATEGORY_LIVE | eDxPANEL_CATEGORY_MEDIA;		// 분류의 대한 초기화는 생각해봐야 한다.
		s_PanelLedInform[i].bOnOff		= FALSE;
		s_PanelLedInform[i].bUsedByCustom= FALSE;
	}

	s_PanelLedInform[eDxLED_ID_TV].category		=	eDxPANEL_CATEGORY_LIVE | eDxPANEL_CATEGORY_MEDIA;
	s_PanelLedInform[eDxLED_ID_REC].category		=	eDxPANEL_CATEGORY_PVR;
	s_PanelLedInform[eDxLED_ID_PLAY].category		=	eDxPANEL_CATEGORY_LIVE | eDxPANEL_CATEGORY_MEDIA;
	s_PanelLedInform[eDxLED_ID_SRS].category		=	eDxPANEL_CATEGORY_LIVE | eDxPANEL_CATEGORY_MEDIA | eDxPANEL_CATEGORY_AUDIO;
	s_PanelLedInform[eDxLED_ID_DOLBY].category	=	eDxPANEL_CATEGORY_LIVE | eDxPANEL_CATEGORY_MEDIA | eDxPANEL_CATEGORY_AUDIO;
}

STATIC void		local_sys_panelTask(void *pParam)
{
	HUINT32				ulStringLen = 0;
	HUINT32				ulOnlyStringLen = 0;
	HUINT32				ulMaxScrollStringLen = 0;
	HUINT32				ulOffset = 0;
	HUINT8 				szScrollString[MAX_PANEL_BUFFER_LEN + 1] = {0, };
	HUINT8 				szTempPanelString[MAX_PANEL_BUFFER_LEN + 1] = "START SYSTEM";
	HUINT16				usStartPos = 0;
	HINT32				nScrollMax = 0, nBlankPos = 0;
	SvcSys_PanelScrType_e		eScrollType = PanelScrType_NoScr;
	HBOOL				bDisplay = FALSE;
	HBOOL				bFirstDisplay = FALSE;
	HINT32				lSleepCount = 0;

	NOT_USED_PARAM(pParam);

	PAL_PANEL_GetMaxStringLen(&ulMaxScrollStringLen);

	// led informtaion init
	local_sys_LedInformationInit();

	while (1)
	{
		VK_TASK_Sleep(1);
		PAN_STR_SEM_GET();

		// scroll or sleep
		if(memcmp(s_szPanelString, szTempPanelString, MAX_PANEL_BUFFER_LEN) == 0)
		{
			bFirstDisplay = FALSE;

#if			defined(CONFIG_PANEL_SCROLL_BACKWARD)
			switch(eScrollType)
			{
				case PanelScrType_NoScr :
					bDisplay = FALSE;
					break;

				case PanelScrType_Forward :
					ulOnlyStringLen = MWC_UTIL_DvbStrlenStrOnly(&szTempPanelString[usStartPos]);
					if(ulOnlyStringLen > MAX_PANEL_SCROLL_LEN)
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						usStartPos++;
					}
					else
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						eScrollType = PanelScrType_Backward;
					}
					bDisplay = TRUE;
					break;

				case PanelScrType_Backward :
					if(usStartPos > ulOffset)
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						usStartPos--;
					}
					else
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						eScrollType = PanelScrType_Forward;
						bFirstDisplay = TRUE;
					}
					bDisplay = TRUE;
					break;

				default :
					bDisplay = FALSE;
					break;
			}
#else
			switch(eScrollType)
			{
				case PanelScrType_NoScr :
					bDisplay = FALSE;
					break;

				case PanelScrType_Forward :
					ulOnlyStringLen = MWC_UTIL_DvbStrlenStrOnly(&szTempPanelString[usStartPos]);
					if(ulOnlyStringLen > 0)
					{
						if(usStartPos == ulOffset)
						{
							bFirstDisplay = TRUE;
#if defined(CONFIG_MW_SYS_PANEL_SCROLL_ONCE)
							eScrollType = PanelScrType_NoScr;
#endif
						}
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						usStartPos++;
					}
					else
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						usStartPos = ulOffset;
					}
					bDisplay = TRUE;
					break;

				case PanelScrType_Backward:
					if(usStartPos > 0)
					{
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						usStartPos--;
					}
					else
					{
						HxSTD_MemSet(szScrollString, ' ', MAX_PANEL_BUFFER_LEN + 1);
						HLIB_STD_StrUtf8NCpy(&szScrollString[nBlankPos], &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
						nBlankPos++;
						nScrollMax--;
					}
					if(nScrollMax < 0)
					{
						usStartPos 		= ulStringLen - (ulMaxScrollStringLen -1 );
						nScrollMax		= ulMaxScrollStringLen ;
						nBlankPos		= 0;
						HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
					}
					bDisplay = TRUE;
					break;

				default :
					bDisplay = FALSE;
					break;
			}
#endif
		}
		// string update
		else
		{
			eScrollType = PanelScrType_NoScr;
			bFirstDisplay = TRUE;
			ulOffset = 0;

			HxSTD_memcpy(szTempPanelString, s_szPanelString, MAX_PANEL_BUFFER_LEN);

			ulStringLen = MWC_UTIL_DvbStrlen(szTempPanelString);
			ulOnlyStringLen = MWC_UTIL_DvbStrlenStrOnly(szTempPanelString);
			ulOffset = ulStringLen - ulOnlyStringLen;
			usStartPos = ulOffset;

			HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);

			if(ulOnlyStringLen > ulMaxScrollStringLen)
			{
				//eScrollType = PanelScrType_Forward;
				eScrollType = PanelScrType_Forward;
				if(eScrollType == PanelScrType_Backward)
				{
					usStartPos 		= ulOnlyStringLen - (ulMaxScrollStringLen -1 );
					nScrollMax		= ulMaxScrollStringLen  ;
					nBlankPos		= 0;
					HLIB_STD_StrUtf8NCpy(szScrollString, &szTempPanelString[usStartPos], MAX_PANEL_BUFFER_LEN + 1);
				}
				else{
					usStartPos++;
				}

			}
			bDisplay = TRUE;
		}
		PAN_STR_SEM_RELEASE();

		lSleepCount = 0;
		if(bDisplay)
		{
			PAN_STR_SEM_GET();
			if (!s_bTimeDisplay)
			{
				PAL_PANEL_DisplayString(szScrollString);
			}
			PAN_STR_SEM_RELEASE();
			if(bFirstDisplay)
			{
				while(lSleepCount < 10)
				{
					VK_TASK_Sleep(200);
					lSleepCount++;

					PAN_STR_SEM_GET();
					if(memcmp(s_szPanelString, szTempPanelString, MAX_PANEL_BUFFER_LEN) != 0)
					{
						PAN_STR_SEM_RELEASE();
						break;
					}
					PAN_STR_SEM_RELEASE();
				}
			}
			else
			{
				while(lSleepCount < 3)
				{
					VK_TASK_Sleep(200);
					lSleepCount++;

					PAN_STR_SEM_GET();
					if(memcmp(s_szPanelString, szTempPanelString, MAX_PANEL_BUFFER_LEN) != 0)
					{
						PAN_STR_SEM_RELEASE();
						break;
					}
					PAN_STR_SEM_RELEASE();
				}
			}
		}
		else
		{
			while(lSleepCount < 10)
			{
				VK_TASK_Sleep(200);
				lSleepCount++;

				PAN_STR_SEM_GET();
				if(memcmp(s_szPanelString, szTempPanelString, MAX_PANEL_BUFFER_LEN) != 0)
				{
					PAN_STR_SEM_RELEASE();
					break;
				}
				PAN_STR_SEM_RELEASE();
			}
		}
	}

	return;
}
#endif
#endif

#define	LED_BLINK_TIME		300		// 300ms
#define	LED_ON_TIME			80
#define	LED_OFF_TIME		40
#define	MAX_LED_LIST		4
STATIC HUINT32	s_ulLedBlinkListNum;
STATIC DxLedId_e	s_aulLedBlinkList[MAX_LED_LIST];
#define	MAX_IGNORE_KEY_LIST		4
STATIC HUINT32		s_ulKeyListNum;
STATIC KEY_Code_t	s_aulKeyListToIgnore[MAX_IGNORE_KEY_LIST];

STATIC void svc_sys_LedBlinkingTimerCallback(unsigned long TimerId, void *params)
{
	if (s_ulBlinkingTimer == TimerId)
		s_ulBlinkingTimer = 0;
}

STATIC HERROR svc_sys_LedStartBlinking(void)
{
	HUINT32	ulMsg;
	HBOOL	bSendMsg;
	HUINT32	nResult = 0;

	bSendMsg = TRUE;
	if (s_ulBlinkingTimer)
	{
		VK_TIMER_Cancel(s_ulBlinkingTimer);
		bSendMsg = FALSE;
	}

	VK_TIMER_EventAfter((unsigned long)LED_BLINK_TIME, svc_sys_LedBlinkingTimerCallback, (void *)NULL, (int)0, (unsigned long *)&s_ulBlinkingTimer);
	if (bSendMsg)
	{
		ulMsg = 0;
		nResult = VK_MSG_SendTimeout(s_ulBlinkingMsgQId, &ulMsg, sizeof(HUINT32), 0);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			return nResult;
		}
	}

	return ERR_OK;
}

STATIC void svc_sys_LedBlink(DxLedId_e eLedId)
{
	PAL_PANEL_ForceSetLedOnOff(eLedId, TRUE);
	VK_TASK_Sleep(LED_ON_TIME);
	PAL_PANEL_ForceSetLedOnOff(eLedId, FALSE);
	VK_TASK_Sleep(LED_OFF_TIME);
}

STATIC void svc_sys_LedBlinkSetInfo(void)
{
	HERROR	hErr;

	// Led List
	s_ulLedBlinkListNum = 0;
	HxSTD_memset(s_aulLedBlinkList, 0, sizeof(DxLedId_e) * MAX_LED_LIST);

	// Keys to Ignore
	s_ulKeyListNum = 0;
	HxSTD_memset(s_aulKeyListToIgnore, 0, sizeof(KEY_Code_t) * MAX_IGNORE_KEY_LIST);

	hErr = SVC_SYS_GetBlinkingInfo(s_aulLedBlinkList, &s_ulLedBlinkListNum, s_aulKeyListToIgnore, &s_ulKeyListNum);
	if (hErr != ERR_OK)
	{
		s_ulLedBlinkListNum = 0;
		s_ulKeyListNum= 0;
	}

	if (s_ulLedBlinkListNum > MAX_LED_LIST)
	{
		s_ulLedBlinkListNum = MAX_LED_LIST;
	}

	if (s_ulKeyListNum > MAX_IGNORE_KEY_LIST)
	{
		s_ulKeyListNum = MAX_IGNORE_KEY_LIST;
	}

}

STATIC HBOOL svc_sys_LedBlinkSupported(void)
{
	return (s_ulLedBlinkListNum == 0) ? FALSE : TRUE;
}

STATIC HBOOL svc_sys_LedBlinkKeySupported(HUINT32 ulKeyCode)
{
	HUINT32		i;

	for (i=0; i<s_ulKeyListNum;i++)
	{
		if (s_aulKeyListToIgnore[i] == (KEY_Code_t)ulKeyCode)
		{
			return FALSE;
		}
	}

	return TRUE;
}

STATIC HERROR	svc_sys_SetLedInform(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bUsedCus)
{
	s_PanelLedInform[eLedId].bOnOff			=	bOnOff;
	s_PanelLedInform[eLedId].bUsedByCustom	=	bUsedCus;

	return ERR_OK;
}

STATIC HERROR	svc_sys_SetLedInformByCustom(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bUsedCus)
{
	//s_PanelLedInform[eLedId].bOnOff			=	bOnOff;
	s_PanelLedInform[eLedId].bUsedByCustom	=	bUsedCus;

	return ERR_OK;
}

STATIC void		local_sys_ledBlinkingTask(void *pParam)
{
	HUINT32	ulMsg;
	HINT32	lResult;
	HERROR	hError;
	DxLedId_e	eLedId = NUM_MAX_DxLED;
	HBOOL	bLedOnOff;
	HBOOL	bFoundLed;
	HUINT32	i;

	while(1)
	{
		lResult = VK_MSG_Receive(s_ulBlinkingMsgQId, &ulMsg, sizeof(HUINT32));
		if(lResult != VK_OK)
		{
			HxLOG_Error("[LED Blinking] VK_MSG_Receive Error\n");
			continue;
		}

		if (svc_sys_LedBlinkSupported() == FALSE)
		{
			continue;
		}

		// Find LED ID to Blink
		bFoundLed = FALSE;
		for (i=0; i<s_ulLedBlinkListNum; i++)
		{
			bLedOnOff = FALSE;
			hError = PAL_PANEL_GetLedStatus(s_aulLedBlinkList[i], &bLedOnOff);
			if (hError == ERR_OK && bLedOnOff == TRUE)
			{
				HxLOG_Print("[LED Blinking] PAL_PANEL_GetLedStatus(%d) ON\n", s_aulLedBlinkList[i]);
				eLedId = s_aulLedBlinkList[i];
				bFoundLed = TRUE;
				break;
			}
		}

		if (bFoundLed == FALSE)
		{
			HxLOG_Error("[LED Blinking] bFoundLed == FALSE\n");
			continue;
		}

		// Blinking
		while (s_ulBlinkingTimer)
		{
			bLedOnOff = TRUE;
			hError = PAL_PANEL_GetLedStatus(eLedId, &bLedOnOff);
			// Blinking 되는 중간에 TV/RADIO 전환 된 경우
			if (hError == ERR_OK && bLedOnOff == FALSE)
			{
				// Get Available LED ID
				for (i=0; i<s_ulLedBlinkListNum; i++)
				{
					hError = PAL_PANEL_GetLedStatus(s_aulLedBlinkList[i], &bLedOnOff);
					if (hError == ERR_OK && bLedOnOff == TRUE)
					{
						HxLOG_Print("[LED Blinking] PAL_PANEL_GetLedStatus(%d) ON\n", s_aulLedBlinkList[i]);
						eLedId = s_aulLedBlinkList[i];
						break;
					}
				}
			}

			svc_sys_LedBlink(eLedId);
		}

		// Restore previous LED status
		for (i=0; i<s_ulLedBlinkListNum; i++)
		{
			hError = PAL_PANEL_GetLedStatus(s_aulLedBlinkList[i], &bLedOnOff);
			if (hError == ERR_OK)
			{
				HxLOG_Print("[LED Blinking] PAL_PANEL_GetLedStatus(%d) %s\n", s_aulLedBlinkList[i], (bLedOnOff) ? "ON" : "OFF");
				PAL_PANEL_ForceSetLedOnOff(s_aulLedBlinkList[i], bLedOnOff);	// Led on/off 하고
				PAL_PANEL_SetLedOnOff(s_aulLedBlinkList[i], bLedOnOff);			// Status도 Update 하기 위해..
			}
		}
	}
}

STATIC HBOOL	svc_sys_FindPanelStringByPriorityCallback(void *pvUserData, void *pvListData)
{
	SvcSys_PanelPriority_t	*pstListInfo;
	HUINT32			priority;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (SvcSys_PanelPriority_t *)pvListData;
	priority = (HUINT32)pvUserData;

	return (pstListInfo->priority == priority) ? TRUE : FALSE;
}

STATIC HERROR	svc_sys_GetPanelStringByPriority(HUINT32 priority, SvcSys_PanelPriority_t **ppstObject)
{
	HxList_t	*pstFound		=	NULL;
	HUINT32		nPriority		=	priority;

	//HxLOG_Debug("List Num [%d]\n", HLIB_LIST_Length(s_PanelPriorityList));

	pstFound = HLIB_LIST_FindEx(s_PanelPriorityList, (const void *)nPriority, svc_sys_FindPanelStringByPriorityCallback);
	if (NULL == pstFound)
		return ERR_FAIL;

	if (ppstObject)
		*ppstObject = (SvcSys_PanelPriority_t *)HLIB_LIST_Data(pstFound);

	return ERR_OK;
}

STATIC HINT32 svc_sys_SortPanelStringMapByPriority (void *pData1, void *pData2)
{
	SvcSys_PanelPriority_t	*pstPanelData1= ( SvcSys_PanelPriority_t * )pData1;
	SvcSys_PanelPriority_t	*pstPanelData2= ( SvcSys_PanelPriority_t * )pData2;

	if ((NULL != pstPanelData1) && (NULL != pstPanelData2))
	{
		return pstPanelData1->priority - pstPanelData2->priority;
	}

	return 0;
}

STATIC HERROR	svc_sys_AddPriorityPanelString(HUINT32 nPriority, HUINT8 *pszStr)
{
	SvcSys_PanelPriority_t		*pAddData	=	HLIB_STD_MemAlloc(sizeof(SvcSys_PanelPriority_t));

	if(pAddData == NULL)
	{
		return ERR_FAIL;
	}

	pAddData->priority	=	nPriority;

	HxSTD_MemSet(pAddData->panelString, '\0', MAX_PANEL_BUFFER_LEN+1);
	if(pszStr)
	{
		HLIB_STD_StrUtf8NCpy(pAddData->panelString, pszStr, MAX_PANEL_BUFFER_LEN+1);
	}

	if( HLIB_LIST_Length(s_PanelPriorityList) <= 0)
	{
		s_PanelPriorityList = 	HLIB_LIST_Append(s_PanelPriorityList, pAddData);
	}
	else
	{
		s_PanelPriorityList = 	HLIB_LIST_AppendSortEx(s_PanelPriorityList, pAddData, svc_sys_SortPanelStringMapByPriority);
	}

	return ERR_OK;
}

STATIC HERROR	svc_sys_RemoveAllPanelPriorityMap(void)
{
	HxList_t			*pList = NULL;
	SvcSys_PanelPriority_t		*pData = NULL;
	pList = HLIB_LIST_First(s_PanelPriorityList);

	while(pList)
	{
		pData	= HLIB_LIST_Data(pList);
		HLIB_STD_MemFree(pData);
		pList = pList->next;
	}

	s_PanelPriorityList = HLIB_LIST_RemoveAll(s_PanelPriorityList);
	s_PanelPriorityList = NULL;
	return ERR_OK;
}

STATIC HERROR	svc_sys_InitOctoPanelPriority(void)
{
	HINT32		i;
	if(HLIB_LIST_Length(s_PanelPriorityList) > 0)
		svc_sys_RemoveAllPanelPriorityMap();

	for( i = ePANEL_STR_PRIORITY_BG_ACTION; i <= NUM_MAX_PanelStringPriority_e; i++)
	{
		svc_sys_AddPriorityPanelString(i, NULL);
	}
	return ERR_OK;
}

STATIC HUINT8 * svc_sys_RegisterPanelString(HUINT32 nPriority, HUINT8 *pszStr)
{
	HINT32				i;
	HxList_t			*pFoundList = NULL;

	SvcSys_PanelPriority_t		*pFoundPanelString	=	NULL;
	SvcSys_PanelPriority_t		*pDisplayPanelString=	NULL;


	svc_sys_GetPanelStringByPriority(nPriority, &pFoundPanelString);

	if (pFoundPanelString != NULL)		// 이미 등독된 Priority 를 가지는 panel string
	{
		HxSTD_MemSet(pFoundPanelString->panelString, '\0', MAX_PANEL_BUFFER_LEN+1);
		if(pszStr)
		{
			HLIB_STD_StrUtf8NCpy(pFoundPanelString->panelString, pszStr, MAX_PANEL_BUFFER_LEN+1);
		}
	}
	else								// 등록 되지 않은 priority 이므로 등록한다.
	{
		svc_sys_AddPriorityPanelString(nPriority, pszStr);
	}

	for ( i=0; i < HLIB_LIST_Length(s_PanelPriorityList); i++)
	{
		pFoundList	=	HLIB_LIST_GetListItem(s_PanelPriorityList, i);

		if (pFoundList)
		{
			pDisplayPanelString = (SvcSys_PanelPriority_t *)HLIB_LIST_Data(pFoundList);

			if (pDisplayPanelString->panelString[0] != '\0')
				return pDisplayPanelString->panelString;
		}
	}

	return "";
}

STATIC void		local_sys_RgbLedblinkingTask(void *pParam)
{
	HINT32 i = 0;

	while(TRUE)
	{
		if(s_stRgbLedList != NULL )
		{
			for(i = 0 ; i < s_ulRgbLedNum ; ++i)
			{
				if(s_stRgbLedList[i].bBlink)
				{
					SVC_SYS_SetRGBLedOff(s_stRgbLedList[i].eLedId);
				}
			}
			VK_TASK_Sleep(s_ulRgbBlinkInterval);

			for(i = 0 ; i < s_ulRgbLedNum ; ++i)
			{
				if(s_stRgbLedList[i].bBlink)
				{
					if(s_pfLedColor[s_stRgbLedList[i].stLedState.eLedColor])
					{
						s_pfLedColor[s_stRgbLedList[i].stLedState.eLedColor](s_stRgbLedList[i].eLedId, TRUE);
						SVC_SYS_SetRGBLedDimmingLevel(s_stRgbLedList[i].eLedId, s_stRgbLedList[i].stLedState.eDimmLevel);
					}
				}
			}
			VK_TASK_Sleep(s_ulRgbBlinkInterval);
		}

		VK_TASK_Sleep(200);

	}
}

HERROR	local_sys_RgbLedInit(void)
{
	HERROR	err = ERR_OK;
	HINT32	lResult = 0;

	lResult = VK_TASK_Create(	local_sys_RgbLedblinkingTask,
								MW_PANEL_TASK_PRIORITY,
								MW_PANEL_TASK_STACK_SIZE,
								(const char*)"mw_rgbled_task",
								NULL,
								(unsigned long*)&s_ulRgbBlinkTaskId,
								0);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Create fail!!\n");
		return ERR_FAIL;
	}


	lResult = VK_TASK_Start(s_ulRgbBlinkTaskId);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Start fail!!\n");
		return ERR_FAIL;

	}

	return err;
}

SvcSys_RgbLedInfo_t* local_sys_GetRgbLedInfo(DxLedId_e eLedId)
{
	HINT32 i = 0;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	if( s_stRgbLedList != NULL )
	{
		for(i = 0 ; i < s_ulRgbLedNum ; ++i)
		{
			if(s_stRgbLedList[i].eLedId == eLedId)
			{
				pRgbInfo = s_stRgbLedList+i;
				break;
			}
		}
	}

	return pRgbInfo;
}

HERROR SVC_SYS_Init (PostMsgToMgrCb_t pfncPostMsgCallback)
{

	HERROR		hError = ERR_FAIL;
	HINT32		lResult = 0;

	svc_sys_RegisterPostMsgToMgrCallBack(pfncPostMsgCallback);

	PAL_PARAM_Init(NULL);
	PAL_SYS_InitEncryptionKey();
	PAL_SYS_UpgradeInit(psvc_sys_UpgradeGetMaxUpgradeSize());

#if defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD)
	/* Panel String Scroll */
	lResult = VK_SEM_Create((unsigned long*)&s_ulPanelStringSemId, "mw_panel_sem", VK_SUSPENDTYPE_FIFO);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_SEM_Create fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}

	lResult = VK_TASK_Create(	local_sys_panelTask,
								MW_PANEL_TASK_PRIORITY,
								MW_PANEL_TASK_STACK_SIZE,
								(const char*)"mw_panel_task",
								NULL,
								(unsigned long*)&s_ulPanelTaskId,
								0);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Create fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}


	lResult = VK_TASK_Start(s_ulPanelTaskId);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Start fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}
#endif

	s_aucDisableAutoDisable = 0;

#if !defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V5)
	/* Led Blinking */
	svc_sys_LedBlinkSetInfo();
	lResult = VK_MSG_Create((unsigned long)MW_SYS_MSGQ_SIZE, (unsigned long)sizeof(HUINT32), (const char *)"LEDBLINKMsgQ", (unsigned long *)&s_ulBlinkingMsgQId, VK_SUSPENDTYPE_FIFO);
	if(lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_MSG_Create fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}

	lResult = VK_TASK_Create(	local_sys_ledBlinkingTask,
								MW_PANEL_TASK_PRIORITY,
								MW_PANEL_TASK_STACK_SIZE,
								(const char*)"mw_led_task",
								NULL,
								(unsigned long*)&s_ulLedBlinkingTaskId,
								0);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Create fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}

	lResult = VK_TASK_Start(s_ulLedBlinkingTaskId);
	if (lResult != VK_OK)
	{
		HxLOG_Error("[SVC_SYS_Init] VK_TASK_Start fail!!\n");
		goto EXIT_MWSYS_FUNC;
	}

	// RGB LED Related TASK
	hError = psvc_sys_GetRgbLedInfo(&s_stRgbLedList, &s_ulRgbLedNum);
	if(hError == ERR_OK && s_ulRgbLedNum > 0)
	{
		s_ulRgbBlinkInterval = psvc_sys_GetRgbBlinkInterval();
		local_sys_RgbLedInit();
	}

	svc_sys_InitOctoPanelPriority();
	PAL_PANEL_ClearLed();

#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
	svc_sys_setDefaultLaunchAppMode();
#endif

#endif
	hError = ERR_OK;

EXIT_MWSYS_FUNC :

	return hError;
}


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________RCU_Front_Key___________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

HERROR	SVC_SYS_RegisterKeyEvtCallback(void)
{
	return SYS_ERR_OK;
}

HERROR	SVC_SYS_RegisterKeyPreProcessingEvtCallback(KEY_NotifyCallback_t pfnNotify)
{
	HERROR ePalErr; 

	ePalErr = PAL_KEY_RegisterKeyPreProcessingCallback(pfnNotify);
	return (ePalErr == ERR_OK) ? SYS_ERR_OK : SYS_ERR_FAIL;
}

#if 0
void SVC_SYS_OnOffKeyInput(HBOOL bOnOff)
{
	if(bOnOff == TRUE)
	{
		PAL_KEY_EnableKeyInput();
	}
	else
	{
		PAL_KEY_DisableKeyInput();
	}
}
#endif

HBOOL SVC_SYS_IsWakeUpKey(HUINT32 ulKeyCode)
{
	return FALSE;
}

#if 0
HUINT8* SVC_SYS_GetKeyString(HUINT32 ulKeyCode)
{
	return PAL_KEY_GetKeyString(ulKeyCode);
}
#endif

HERROR	SVC_SYS_GetCurrentCustomCode(HUINT16 *pusCustomCode)
{
	return PAL_KEY_GetCurrentCustomCode(pusCustomCode);
}

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________System_Information___________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

HERROR SVC_SYS_GetModelName(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn)
{
	if (NULL == pszModelName)
		return ERR_FAIL;

	if (0 == ulLength)
		return ERR_FAIL;

	return psvc_sys_GetModelName(pszModelName, ulLength, bPanelSizeOn);
}

HERROR SVC_SYS_GetSystemId (HUINT32 *pulSystemId)
{
	if (NULL == pulSystemId)
		return ERR_FAIL;

	return psvc_sys_GetSystemId (pulSystemId);
}

HERROR SVC_SYS_GetDeviceId (HUINT8 *pszDeviceId, HUINT32 ulBufSize)
{
	if(pszDeviceId == NULL || ulBufSize == 0)
		return ERR_FAIL;

	return psvc_sys_GetDeviceId(pszDeviceId, ulBufSize);
}

SvcSys_ErrorCode_e SVC_SYS_GetSystemVersion (VersionType_t eSystemType, HUINT32 *pulSystemVer)
{
	HERROR		hErr;

	if (USE_PAL_PARAM())
	{
		PAL_PARAM_ERR_CODE_e		eParamErr = ePAL_PARAM_ERR_FAIL;

		switch (eSystemType)
		{
		case eVERSION_TYPE_NEXTOTA:			// OTA Loader에게 알려주기 위한 Update될 OTA Image Version
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION_NEXT, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_LOADER:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_LOADER_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_APPL:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_EEPROM:
		case eVERSION_TYPE_KERNEL:
			break;
		case eVERSION_TYPE_ROOTFS:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_ROOTFS_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_RESOURCE:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_RESOURCE_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_DB:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_DB_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
		case eVERSION_TYPE_BOOTLOADER:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_BOOTLOADER_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
#if defined(CONFIG_OTA_USE_PKG_VERSION)
		case eVERSION_TYPE_PACKAGE:
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_PACKAGE_VERSION, (void *)pulSystemVer, sizeof(HUINT32));
			break;
#endif
		/* eVERSION_TYPE_UPGRADER uses PAL_SYS_GetSystemVersion() */

		default:
			break;
		}

		if (eParamErr == ePAL_PARAM_ERR_FAIL)
			hErr = SYS_ERR_FAIL;
		else if (eParamErr == ePAL_PARAM_ERR_CRC)
			hErr = SYS_ERR_CRC;
		else
			hErr = SYS_ERR_OK;
	}
	else
	{
		PAL_SYS_ERR_CODE_t		eSysErr;
		eSysErr = PAL_SYS_GetSystemVersion (eSystemType, pulSystemVer);
		if (eSysErr == PAL_SYS_ERR_FAIL)
			hErr = SYS_ERR_FAIL;
		else if (eSysErr == PAL_SYS_ERR_CRC)
			hErr = SYS_ERR_CRC;
		else
			hErr = SYS_ERR_OK;
	}

	return hErr;
}

HERROR SVC_SYS_SetSystemVersion (VersionType_t eSystemType, HUINT32 ulSystemVer)
{
	if (USE_PAL_PARAM())
	{
		PAL_PARAM_ERR_CODE_e		eParamErr;

		eParamErr = ePAL_PARAM_ERR_FAIL;
		switch (eSystemType)
		{
		case eVERSION_TYPE_NEXTOTA:			// OTA Loader에게 알려주기 위한 Update될 OTA Image Version
			eParamErr = PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION_NEXT, (void *)ulSystemVer, sizeof(HUINT32));
			break;
		default:
			break;
		}

		return (eParamErr != ePAL_PARAM_ERR_OK) ? SYS_ERR_FAIL : SYS_ERR_OK;
	}
	else
	{
		PAL_SYS_ERR_CODE_t		palSysErr;

		palSysErr = PAL_SYS_SetSystemVersion (eSystemType, ulSystemVer);

		return (palSysErr != PAL_SYS_ERR_OK) ? SYS_ERR_FAIL : SYS_ERR_OK;
	}
}

#define	svcSys_MAX_VER_HEAD_LEN				6
#define	svcSys_MAX_APP_VER_LEN				(svcSys_MAX_VER_HEAD_LEN + 3)
#define	svcSys_MAX_LOADER_VER_LEN			(svcSys_MAX_VER_HEAD_LEN + 2)
#define	svcSys_MAX_MICOM_VER_LEN			(svcSys_MAX_VER_HEAD_LEN + 4)
#define	svcSys_MAX_VER_LEN					10
#define	svcSys_LOADER_IRDTO_VER_HEAD		(HCHAR*)"IRDETO"
#define	svcSys_MAKE_STR_LEN					32

STATIC HERROR svc_sys_MakeVerText (HUINT8 *szHeadStr, HUINT32 ulVerNum, HUINT8 *szText, HUINT32 ulVerByteNum)
{
	HUINT32			 ulVerNum1, ulVerNum2, ulVerNum3, ulTemp = 0;			// VerNum1.VerNum2.VerNum3
	HBOOL			 bHeader = FALSE;

	if (szText == NULL)
	{
		return ERR_FAIL;
	}

	ulVerNum1 = (ulVerNum >> 16) & 0xFF;
	ulVerNum2 = (ulVerNum >> 8) & 0xFF;
	ulVerNum3 = (ulVerNum) & 0xFF;

	bHeader = (szHeadStr != NULL && szHeadStr[0] != '\0') ? TRUE : FALSE;
	switch (ulVerByteNum)
	{
	case 2:
		if (bHeader)
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%s %d.%02d", szHeadStr, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%d.%02d", ulVerNum2, ulVerNum3);
		}
		break;

	case 3:	// For Irdeto Loader
		ulVerNum1 = (ulVerNum >> 8) & 0xFF;
		ulTemp = (ulVerNum) & 0xFF;
		ulVerNum2 =(ulTemp>>4) & 0x0F;
		ulVerNum3 = (ulTemp) & 0x0F;

		if (bHeader)
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%s %d.%d.%d", szHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%d.%d.%d", ulVerNum1, ulVerNum2, ulVerNum3);
		}
		break;

	case 4:
	default:
		if (bHeader)
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%s %d.%02d.%02d", szHeadStr, ulVerNum1, ulVerNum2, ulVerNum3);
		}
		else
		{
			HxSTD_PrintToStrN (szText, svcSys_MAKE_STR_LEN, "%d.%02d.%02d", ulVerNum1, ulVerNum2, ulVerNum3);
		}

		break;
	}


	return ERR_OK;
}

STATIC HERROR svc_sys_GetVersionHeader (VersionType_t eSystemType, HUINT8 *pucVersionHeader, HUINT32 ulBufMax)
{
	HERROR		 hErr;
	HUINT8		aucVer[svcSys_MAX_VER_LEN];

	if (pucVersionHeader == NULL || ulBufMax < svcSys_MAX_VER_HEAD_LEN + 1)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pucVersionHeader, 0, ulBufMax);
	switch (eSystemType)
	{
	case eVERSION_TYPE_LOADER:
		hErr = PAL_PARAM_GetField(ePAL_PARAM_LOADER_VERSION, (void *)aucVer, svcSys_MAX_LOADER_VER_LEN);
		if (hErr == ERR_OK)
		{
			HLIB_STD_StrUtf8NCpy(pucVersionHeader, aucVer, svcSys_MAX_LOADER_VER_LEN);
			return ERR_OK;
		}
		else if (hErr == PAL_SYS_ERR_CRC)
		{
			HLIB_STD_StrUtf8NCpy(pucVersionHeader, SYSTEM_APP_VER_HEAD, svcSys_MAX_LOADER_VER_LEN);
			return ERR_FAIL;
		}
		break;

	case eVERSION_TYPE_APPL:
		HLIB_STD_StrUtf8NCpy(pucVersionHeader, SYSTEM_APP_VER_HEAD, svcSys_MAX_LOADER_VER_LEN);
		return ERR_OK;

	default:
		break;
	}

	return ERR_FAIL;
}

STATIC HERROR svc_sys_MakeSystemVerString (VersionType_t eSystemType, HUINT32 ulSystemVer, HUINT8 *pszSysVerStr)
{
	if (pszSysVerStr == NULL)
	{
		return ERR_FAIL;
	}

	switch (eSystemType)
	{
	case eVERSION_TYPE_LOADER:
	{
#if defined(CONFIG_MW_CAS_IRDETO)
		svc_sys_MakeVerText (svcSys_LOADER_IRDTO_VER_HEAD, ulSystemVer, pszSysVerStr, 3);
#else
		HUINT8 ucVersionHeader[svcSys_MAX_VER_HEAD_LEN + 2];
		svc_sys_GetVersionHeader(eSystemType, ucVersionHeader, svcSys_MAX_VER_HEAD_LEN+2);
		svc_sys_MakeVerText (ucVersionHeader, ulSystemVer, pszSysVerStr, 2);		// MODEL~ X.XX
#endif
		return ERR_OK;
	}

	case eVERSION_TYPE_APPL:
		svc_sys_MakeVerText (SYSTEM_APP_VER_HEAD, ulSystemVer, pszSysVerStr, 4);
		return ERR_OK;

	case eVERSION_TYPE_NEXTOTA:
	case eVERSION_TYPE_BOOTLOADER:
	case eVERSION_TYPE_UPGRADER:
		svc_sys_MakeVerText (NULL, ulSystemVer, pszSysVerStr, 4);
		return ERR_OK;

#if defined(CONFIG_OTA_USE_PKG_VERSION)
	case eVERSION_TYPE_PACKAGE:
		//svc_sys_MakeVerText (NULL, ulSystemVer, pszSysVerStr, /* 5 */);
		return ERR_OK;
#endif

	default:
		break;
	}

	return ERR_FAIL;
}


HERROR SVC_SYS_MakeSystemVerString (VersionType_t eSystemType, HUINT32 ulSystemVer, HUINT8 *pszSysVerStr)
{
	if (USE_PAL_PARAM())
	{
		return svc_sys_MakeSystemVerString (eSystemType, ulSystemVer, pszSysVerStr);
	}
	else
	{
		return PAL_SYS_MakeSystemVerString (eSystemType, ulSystemVer, pszSysVerStr);
	}
}


STATIC HERROR svc_sys_GetTotalSystemVersionString (HUINT8 *szVerStr, HINT32 nStrLen)
{
	HUINT32	ulRootFsVersion, ulResourceVersion, ulDbVersion;

	if (szVerStr == NULL)			{ return ERR_FAIL; }
	if (nStrLen <= 0)				{ return ERR_FAIL; }

	ulRootFsVersion = ulResourceVersion = ulDbVersion = 0;

	(void)PAL_PARAM_GetField(ePAL_PARAM_ROOTFS_VERSION, (void *)&ulRootFsVersion, sizeof(HUINT32));
	(void)PAL_PARAM_GetField(ePAL_PARAM_RESOURCE_VERSION, (void *)&ulResourceVersion, sizeof(HUINT32));
	(void)PAL_PARAM_GetField(ePAL_PARAM_DB_VERSION, (void *)&ulDbVersion, sizeof(HUINT32));

	HxSTD_PrintToStrN (szVerStr, nStrLen,  "%05d.%02d-%05d.%02d-%05d.%02d",
										ulRootFsVersion/100, ulRootFsVersion%100,
										ulResourceVersion/100, ulResourceVersion%100,
										ulDbVersion/100, ulDbVersion%100);
	return ERR_OK;
}

HERROR SVC_SYS_GetTotalSystemVersionString (HUINT8 *pszVersion)
{
	if (USE_PAL_PARAM())
	{
		return svc_sys_GetTotalSystemVersionString (pszVersion, eVERSION_TOTAL_MAX_STR_LEN);
	}
	else
	{
		return PAL_SYS_GetTotalSystemVersionString (pszVersion);
	}
}

HERROR SVC_SYS_GetKernelVersionString (HUINT8 *pszVersion)
{
	if (NULL == pszVersion)
		return ERR_FAIL;

	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_KERNEL_VERSION, (void *)pszVersion, eVERSION_MAX_STR_LEN);
	}
	else
	{
		return PAL_SYS_GetKernelVersionString (pszVersion);
	}
}

HERROR SVC_SYS_GetMicomVersionString(HUINT8 *pszVersion)
{
	if (NULL == pszVersion)
		return ERR_FAIL;

	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_MICOM_STRVER, (void *)pszVersion, eVERSION_MAX_STR_LEN);
	}
	else
	{
		return PAL_SYS_GetMicomVerString(pszVersion);
	}
}

HERROR SVC_SYS_GetChipId(HUINT32 *pulChipId)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_CHIP_ID, (void *)pulChipId, sizeof(HUINT32));
	}
	else
	{
		return PAL_SYS_GetChipId(pulChipId);
	}
}

HERROR SVC_SYS_GetChipId_x64(HUINT32 *pulFrontId, HUINT32 *pulRearId)
{
	if (USE_PAL_PARAM())
	{
		// 필요시 구현하세요
		return ERR_FAIL;
	}
	else
	{
		return PAL_SYS_GetChipId_x64(pulFrontId, pulRearId);
	}
}

HERROR SVC_SYS_GetChipCheckNumber(HUINT32 *pulChipCheckNumber)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_CHIP_CHECK_NO, (void *)pulChipCheckNumber, sizeof(HUINT32));
	}
	else
	{
		return PAL_SYS_GetChipCheckNumber(pulChipCheckNumber );
	}
}

HERROR SVC_SYS_GetSysHWRevision(HUINT32 *pulHWRevision)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_HW_REVISION, (void *)pulHWRevision, sizeof(HUINT32));
	}
	else
	{
		return PAL_SYS_GetSysHWRevision(pulHWRevision );
	}
}

HERROR SVC_SYS_GetSystemSerialNumber(HUINT8 *szSerialNoStr, HUINT32 ulSize)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_SERIAL_NO, (void *)szSerialNoStr, ulSize);
	}
	else
	{
		return PAL_SYS_GetField(PAL_FLASH_FIELD_SERIAL_NO, (void *)szSerialNoStr, ulSize);
	}
}

#if defined(CONFIG_NETFLIX_APP)
HERROR SVC_SYS_SetNetflixESN(HUINT8 *szESN, HUINT32 ulSize)
{
	return PAL_PARAM_SetField(ePAL_PARAM_NETFLIX_ESN, (void *)szESN, ulSize);
}

HERROR SVC_SYS_GetNetflixESN(HUINT8 *szESN, HUINT32 ulSize)
{
	return PAL_PARAM_GetField(ePAL_PARAM_NETFLIX_ESN, (void *)szESN, ulSize);
}
#endif

HERROR SVC_SYS_GetStbID(HUINT8 *szStbIdStr, HUINT32 ulSize)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_DSTB_ID, (void *)szStbIdStr, ulSize);
	}
	else
	{
		return PAL_SYS_GetField(PAL_FLASH_FIELD_DSTB_ID, (void*)szStbIdStr, ulSize);
	}
}

HERROR SVC_SYS_GetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e *peOtaFlag)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_SWUP_FLAG1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_SWUP_FLAG2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_SWUP_FLAG3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_GetField(eItem, (void *)peOtaFlag, sizeof(SWUP_Flag_e));
		}

		return hErr;
	}
	else
	{
		return PAL_SYS_GetOtaFlag (eSlot, peOtaFlag);
	}
}

HERROR SVC_SYS_SetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e eOtaFlag)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_SWUP_FLAG1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_SWUP_FLAG2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_SWUP_FLAG3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&eOtaFlag, sizeof(SWUP_Flag_e));
		}

		return hErr;
	}
	else
	{
		return PAL_SYS_SetOtaFlag (eSlot, eOtaFlag);
	}
}

HERROR SVC_SYS_GetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e *peOtaType)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_SWUP_TYPE1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_SWUP_TYPE2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_SWUP_TYPE3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_GetField(eItem, (void *)peOtaType, sizeof(SWUP_DownloadType_e));
		}

		return hErr;
	}
	else
	{
	 	return PAL_SYS_GetOtaType (eSlot, peOtaType);
	}
}

HERROR SVC_SYS_SetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e eOtaType)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_SWUP_TYPE1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_SWUP_TYPE2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_SWUP_TYPE3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&eOtaType, sizeof(SWUP_DownloadType_e));
		}

		return hErr;
	}
	else
	{
	 	return PAL_SYS_SetOtaType (eSlot, eOtaType);
	}
}

HERROR SVC_SYS_SetChannelType(SWUP_InfoSlot_e eSlot, SWUP_ChannelType_e eChType)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_SWUP_CH_TYPE1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_SWUP_CH_TYPE2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_SWUP_CH_TYPE3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&eChType, sizeof(SWUP_ChannelType_e));
		}

		return hErr;
	}
	else
	{
	 	return PAL_SYS_SetChannelType (eSlot, eChType);
	}
}

HERROR SVC_SYS_SetOtaPid (SWUP_InfoSlot_e eSlot, HUINT16 usPid)
{
	if (USE_PAL_PARAM())
	{
		HERROR				hErr = ERR_FAIL;
		PAL_PARAM_Item_e	eItem = ePAL_PARAM_MAX;

		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_ES_PID1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_ES_PID2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_ES_PID3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&usPid, sizeof(HUINT16));
		}

		return hErr;
	}
	else
	{
		return PAL_SYS_SetOtaPid (eSlot, usPid);
	}
}

HERROR SVC_SYS_SetOtaTuningInfo (SWUP_InfoSlot_e eSlot, DxTuneParam_t *pstTuningInfo)
{
	HERROR				hErr;
	HUINT32				ulLnbFreq;
	DxSAT_LnbVoltage_e	lnbVoltage;
	HBOOL				b22kTone;

	if (eSlot >= eSWUP_INFO_SLOT_MAX)
	{
		return ERR_FAIL;
	}

	if (NULL == pstTuningInfo)
	{
		return ERR_FAIL;
	}

	hErr = ERR_FAIL;
	switch (pstTuningInfo->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		{
			ulLnbFreq		= 0;
			lnbVoltage		= eDxLNB_VOLT_STD;
			b22kTone		= FALSE;
			switch (pstTuningInfo->sat.antennaType)
			{
			case eDxANT_TYPE_LNB_ONLY:
				ulLnbFreq		= pstTuningInfo->sat.antInfo.info.lnb.ulLnbFreq;
				lnbVoltage		= pstTuningInfo->sat.antInfo.info.lnb.lnbVoltage;
			//	b22kTone		= pstTuningInfo->sat.antInfo.info.diseqc.b22kTone;
				break;
			case eDxANT_TYPE_DISEQC:
				ulLnbFreq		= pstTuningInfo->sat.antInfo.info.diseqc.ulLnbFreq;
				lnbVoltage		= pstTuningInfo->sat.antInfo.info.diseqc.lnbVoltage;
				b22kTone		= pstTuningInfo->sat.antInfo.info.diseqc.b22kTone;
				break;
			case eDxANT_TYPE_SCD:
				ulLnbFreq		= pstTuningInfo->sat.antInfo.info.scd.ulLnbFreq;
				lnbVoltage		= pstTuningInfo->sat.antInfo.info.scd.lnbVoltage;
				b22kTone		= pstTuningInfo->sat.antInfo.info.scd.b22kTone;
				break;
			case eDxANT_TYPE_SMATV:
				break;
			default:
				break;
			}

		}
		hErr = PAL_SYS_SetSwUpdateTuningInfoSat (eSlot, pstTuningInfo->sat.tuningInfo.ulFrequency,
													pstTuningInfo->sat.tuningInfo.ulSymbolRate,
													pstTuningInfo->sat.tuningInfo.eFecCodeRate,
													pstTuningInfo->sat.tuningInfo.ePolarization,
													pstTuningInfo->sat.tuningInfo.eTransSpec,
													pstTuningInfo->sat.tuningInfo.ePskMod,
													pstTuningInfo->sat.tuningInfo.ePilot,
													pstTuningInfo->sat.antennaType,
													/* LNB && Common */
													ulLnbFreq,
													lnbVoltage,
													b22kTone,
													/* Diseqc */
													pstTuningInfo->sat.antInfo.info.diseqc.diseqcVersion,
													pstTuningInfo->sat.antInfo.info.diseqc.diseqcInput,
													pstTuningInfo->sat.antInfo.info.diseqc.toneBurst,
													pstTuningInfo->sat.antInfo.info.diseqc.ucMotorPosition,
													pstTuningInfo->sat.antInfo.info.diseqc.sSatLonggitude,
													/* SCD */
													pstTuningInfo->sat.antInfo.info.scd.scdInput,
													pstTuningInfo->sat.antInfo.info.scd.ulScdUserBandNum,
													pstTuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq,
													pstTuningInfo->sat.antInfo.info.scd.ulScdUserBandNum1,
													pstTuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq1);
		break;

	case eDxDELIVERY_TYPE_TER:
		hErr = PAL_SYS_SetSwUpdateTuningInfoTer (eSlot, pstTuningInfo->ter.ulFrequency,
													pstTuningInfo->ter.eBandWidth,
													pstTuningInfo->ter.eConstellation,
													pstTuningInfo->ter.eHierachy,
													pstTuningInfo->ter.eCodeRate,
													pstTuningInfo->ter.eTransMode,
													pstTuningInfo->ter.eGuardInterval,
													pstTuningInfo->ter.eStream,
													pstTuningInfo->ter.eOffset,
													pstTuningInfo->ter.eSystem,
													/* DVB T2 */
													pstTuningInfo->ter.stT2Param.ulPlpId,
													pstTuningInfo->ter.stT2Param.preambleFormat,
													pstTuningInfo->ter.stT2Param.mixed,
													pstTuningInfo->ter.stT2Param.pilotPattern,
													pstTuningInfo->ter.stT2Param.extenedCarrier,
													pstTuningInfo->ter.stT2Param.PAPRreduction,
													pstTuningInfo->ter.stT2Param.numPlpBlocks);
		break;

	case eDxDELIVERY_TYPE_CAB:
		hErr = PAL_SYS_SetSwUpdateTuningInfoCab (eSlot, pstTuningInfo->cab.ulFrequency,
													pstTuningInfo->cab.ulSymbolRate,
													pstTuningInfo->cab.constellation,
													pstTuningInfo->cab.spectrum);
		break;
	default:
		break;
	}

	return hErr;
}

HERROR SVC_SYS_GetSystemUpdateDate( HUINT8 *pucString )
{
#if defined(USE_PAL_PARAM)
	return PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_UPDATE_DATE, (void *)pucString, 0);
#else
	return PAL_SYS_GetSystemUpdateDate( pucString );
#endif
}

HERROR SVC_SYS_GetSystemUpdatedList( HUINT8 *pucString )
{
#if defined(USE_PAL_PARAM)
	return PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_UPDATED_LIST, (void *)pucString, 0);
#else
	return PAL_SYS_GetSystemUpdatedList( pucString );
#endif
}

HERROR SVC_SYS_GetHwRevision( HUINT8 *pucString )
{
#if defined(USE_PAL_PARAM)
	return PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_HW_VERSION, (void *)pucString, 0);
#else
	return PAL_SYS_GetHwRevision( pucString );
#endif
}

HERROR	SVC_SYS_GetEeprom4LoaderDefaultItemList(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum)
{
	if (NULL == pastDefaultItem)
	{
		HxLOG_Error("NULL == pastDefaultItem \n");
		return ERR_FAIL;
	}

	if (NULL == pulItemNum)
	{
		HxLOG_Error("NULL == pulItemNum \n");
		return ERR_FAIL;
	}

	return psvc_sys_GetEeprom4LoaderDefaultItemList(pastDefaultItem, pulItemNum);
}

HERROR	SVC_SYS_SetEeprom4LoaderFactoryReset(SvcSys_FactoryDefaultItem_t *astDefaultItem, HUINT32 ulItemNum)
{
	HERROR	 hErr;
	HUINT32  i;

	if (NULL == astDefaultItem)
	{
		HxLOG_Error("NULL == astDefaultItem \n");
		return ERR_FAIL;
	}

	if (0 >= ulItemNum)
	{
		HxLOG_Error("0 >= ulItemNum \n");
		return ERR_FAIL;
	}

	hErr = ERR_OK;
	for (i=0; i<ulItemNum; i++)
	{
		if (SvcSys_PARAM_KEY_INVALID == astDefaultItem->ulKey)
			break;

		if (astDefaultItem->pvData)
		{
			hErr |= PAL_PARAM_SetField(astDefaultItem->ulKey, astDefaultItem->pvData, astDefaultItem->ulSize);
		}
	}

	if(hErr != ERR_OK)
	{
		fprintf(stderr, "assert!!, writing to shared field with loader is failed~~\n");

		return ERR_FAIL;
	}

	return ERR_OK;
//	return PAL_SYS_SetEeprom4LoaderFactoryReset();
}

HERROR SVC_SYS_SetLanguage( HUINT8 *pucLanguage )
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_LANGUAGE, (void *)pucLanguage, sizeof(HUINT8)*4);
	}
	else
	{
		return PAL_SYS_SetLanguage( pucLanguage );
	}
}

HERROR SVC_SYS_SetScartRGB(DxScartFormat_b eScartRGB)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_SCART_RGB, (void *)eScartRGB, sizeof(DxScartFormat_b));
	}
	else
	{
		return PAL_SYS_SetScartRGB(eScartRGB);
	}
}

HERROR SVC_SYS_SetResolution( DxResolutionSelect_e eResolution )
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_HD_RESOULTION, (void *)&eResolution, sizeof(DxResolutionSelect_e));
	}
	else
	{
		return PAL_SYS_SetResolution( eResolution );
	}
}

HERROR SVC_SYS_SetAspectRatio (DxAspectRatio_e eAspectRatio)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_ASPECT_RATIO, (void *)&eAspectRatio, sizeof(DxAspectRatio_e));
	}
	else
	{
		return PAL_SYS_SetAspectRatio (eAspectRatio);
	}
}

HERROR SVC_SYS_SetTunerId(SWUP_InfoSlot_e eSlot, HUINT8 ucTunerId)
{
	if (USE_PAL_PARAM())
	{
		HERROR					hErr = ERR_FAIL;
		PAL_PARAM_Item_e		eItem = ePAL_PARAM_MAX;
		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_ES_PID1;	break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_ES_PID2;	break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_ES_PID3;	break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&ucTunerId, sizeof(HUINT8));
		}

		return hErr;
	}
	else
	{
		return PAL_SYS_SetTunerId(eSlot, ucTunerId);
	}
}

HERROR SVC_SYS_SetAntennaPower(SWUP_InfoSlot_e eSlot, HUINT8 ucAntennaPower)
{
	if (USE_PAL_PARAM())
	{
		HERROR					hErr = ERR_FAIL;
		PAL_PARAM_Item_e		eItem = ePAL_PARAM_MAX;
		switch (eSlot)
		{
		case eSWUP_INFO_SLOT_0: eItem = ePAL_PARAM_ES_PID1; break;
		case eSWUP_INFO_SLOT_1: eItem = ePAL_PARAM_ES_PID2; break;
		case eSWUP_INFO_SLOT_2: eItem = ePAL_PARAM_ES_PID3; break;
		default:
			break;
		}

		if (ePAL_PARAM_MAX != eItem)
		{
			hErr = PAL_PARAM_SetField(eItem, (void *)&ucAntennaPower, sizeof(HUINT8));
		}

		return hErr;
	}
	else
	{
		return PAL_SYS_SetAntennaPower(eSlot, ucAntennaPower);
	}
}

HERROR SVC_SYS_SetFirstBoot(HBOOL bFirstBoot)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_DISPLAY_FACTORY_INFO, (void *)&bFirstBoot, sizeof(HBOOL));
	}
	else
	{
		return PAL_SYS_SetFirstBoot(bFirstBoot);
	}
}

HERROR SVC_SYS_GetFirstBoot(HBOOL *pbFirstBoot)
{
	if (NULL == pbFirstBoot)
		return ERR_FAIL;

	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_DISPLAY_FACTORY_INFO, (void *)pbFirstBoot, sizeof(HBOOL));
	}
	else
	{
		return PAL_SYS_GetFirstBoot(pbFirstBoot);
	}
}

HERROR SVC_SYS_GetLaunchAppMode(LaunchAppMode_t *pLaunchAppMode)
{
	if (NULL == pLaunchAppMode)
		return ERR_FAIL;

	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_LAUNCH_APP_MODE, (void *)pLaunchAppMode, sizeof(HUINT8));
	}
	else
	{
		return PAL_SYS_GetLaunchAppMode((HUINT8 *)pLaunchAppMode);
	}
}

HERROR SVC_SYS_SetLaunchAppMode(LaunchAppMode_t eLaunchAppMode)
{
	if (USE_PAL_PARAM())
	{
		return PAL_PARAM_SetField(ePAL_PARAM_LAUNCH_APP_MODE, (void *)eLaunchAppMode, sizeof(HUINT8));
	}
	else
	{
		return PAL_SYS_SetLaunchAppMode((HUINT8)eLaunchAppMode);
	}
}

HERROR SVC_SYS_GetFusingInfo( Sys_FusingInfo_t *pstFusingInfo )
{
	if (pstFusingInfo == NULL)
		return ERR_FAIL;

	return psvc_sys_GetFusingInfo(pstFusingInfo);
}

HERROR	SVC_SYS_SetFusingInfoJTAG(HUINT8 ucJTAG)
{
	return psvc_sys_SetFusingInfoJTAG(ucJTAG);
}

HERROR	SVC_SYS_SetFusingInfoCW(HUINT8 ucCWE)
{
	return psvc_sys_SetFusingInfoCW(ucCWE);
}

HERROR	SVC_SYS_SetFusingInfoSecureBoot(HUINT8 ucSCS)
{
	return psvc_sys_SetFusingInfoSecureBoot(ucSCS);
}


#if 0
HERROR MW_SYS_SetCountryCode(HUINT8 *pucCountryCode)
{
	return PAL_SYS_SetCountryCode(pucCountryCode);
}

HERROR MW_SYS_SetUsageId(HUINT8 ucUsageId)
{
	return PAL_SYS_SetUsageId(ucUsageId);
}

HERROR MW_SYS_GetUsageId(HUINT8 *pucUsageId)
{
	return PAL_SYS_GetUsageId(pucUsageId);
}
#endif

#if (defined(CONFIG_MW_CAS_VIACCESS) && defined(CONFIG_MW_MM_PVR))
HERROR SVC_SYS_GetHddSerialNumber(const HUINT8 *pszDevName, HUINT8 **pszSn)
{
	return UAPI_FS_GetHddSerialNumber(pszDevName, pszSn);
}
#endif

#if defined (CONFIG_MW_CI_PLUS)
HERROR SVC_SYS_GetCiPlusVersionNumber(HUINT8 *pucString)
{
	return PAL_SYS_GetCiPlusCoreVersion(pucString);
}
#endif

HERROR SVC_SYS_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS)
{
	return PAL_SYS_GetNandBadBlocks(pulCntLdr, pulCntUBI, pulCntUBIFS);
}

HERROR SVC_SYS_SetPanelOnByOTA(void)
{
	return PAL_PANEL_SetPanelOnByOTA();
}


HERROR SVC_SYS_GetCPUTemperature(HUINT32 *pulCpuTemp)
{
	return PAL_SYS_GetCPUTemperature(pulCpuTemp);
}

#if defined( CONFIG_MW_INET_SNMP )
void SVC_SYS_SetMWPrivateMIBInfo(SvcSys_SnmpInformation_t *pstInfo)
{
	PAL_SNMP_MWInformation_t *pstSnmpMWInfo = NULL;

	pstSnmpMWInfo = (PAL_SNMP_MWInformation_t *)HLIB_STD_MemAlloc(sizeof(PAL_SNMP_MWInformation_t));
	if( pstSnmpMWInfo != NULL )
	{
		memset ( pstSnmpMWInfo , 0x00 , sizeof(PAL_SNMP_MWInformation_t));

		/* 필요한 것만 넣어준다. */
		memcpy( pstSnmpMWInfo->szSystemId , pstInfo->szSystemId, 10 );
		memcpy( pstSnmpMWInfo->szApplicationVer, pstInfo->szApplicationVer, 10 );
		memcpy( pstSnmpMWInfo->szMiddleWareVer, pstInfo->szMiddleWareVer, 10);
		memcpy( pstSnmpMWInfo->szSoftBrowserVer, pstInfo->szSoftBrowserVer, 10 );
		memcpy( pstSnmpMWInfo->szStbID , pstInfo->szStbID, 6 );

		memcpy( pstSnmpMWInfo->szBcasCardId , pstInfo->szBcasCardId, 32 );
		memcpy( pstSnmpMWInfo->szCcasCardId , pstInfo->szCcasCardId, 32 );

		pstSnmpMWInfo->ulDownFrequency = pstInfo->ulDownFrequency;
		pstSnmpMWInfo->ulWatchingChannelId = pstInfo->ulWatchingChannelId;
		pstSnmpMWInfo->ulWatchingNetworkId = pstInfo->ulWatchingNetworkId;

		pstSnmpMWInfo->etWatchingChannelType = pstInfo->etWatchingChannelType;
		pstSnmpMWInfo->ulWatchingTunerGroupId = pstInfo->ulWatchingTunerGroupId;

		pstSnmpMWInfo->etPowerStatus = (PAL_SNMP_StbPowerStatus_e)pstInfo->etPowerStatus;
		pstSnmpMWInfo->etPlayStatus = (PAL_SNMP_StbPlayStatus_e)pstInfo->etPlayStatus;

		pstSnmpMWInfo->ulTuner1ChannelId = pstInfo->ulTuner1ChannelId;
		pstSnmpMWInfo->ulTuner1NetworkId = pstInfo->ulTuner1NetworkId;
		pstSnmpMWInfo->ulTuner1Frequency = pstInfo->ulTuner1Frequency;

		pstSnmpMWInfo->ulTuner2ChannelId = pstInfo->ulTuner2ChannelId;
		pstSnmpMWInfo->ulTuner2NetworkId = pstInfo->ulTuner2NetworkId;
		pstSnmpMWInfo->ulTuner2Frequency = pstInfo->ulTuner2Frequency;

		PAL_SYS_SetMWPrivateMIBInfo( pstSnmpMWInfo );

		HLIB_STD_MemFree( pstSnmpMWInfo );
	}
}
#endif

HUINT8 *SVC_SYS_GetEncryptionKey (void)
{
	return PAL_SYS_GetEncryptionKey();
}

HUINT32 SVC_SYS_GetEncryptionKeySize (void)
{
	return PAL_SYS_GetEncryptionKeySize ();
}


HUINT8*	SVC_SYS_GetIV( void )
{
#if 1
	return PAL_SYS_GetEncryptionIV();
#else
	return s_aucKey;
#endif
}

void SVC_SYS_SetPvrEncryptMode( PvrEncryptMode_t eEncType )
{
	HINT32 nEncType = 0;

	switch(eEncType)
	{
		case ePvrEncryptNone:		nEncType	=	ePAL_PVR_ENCRYPTION_TYPE_NONE;	break;
		case ePvrEncrypt3Des:		nEncType	=	ePAL_PVR_ENCRYPTION_TYPE_3DES;	break;
		case ePvrEncryptAes:		nEncType	=	ePAL_PVR_ENCRYPTION_TYPE_AES;	break;
		case ePvrEncryptUnknown:	nEncType	=	ePAL_PVR_ENCRYPTION_TYPE_CURRENT;	break;

	}

	PAL_SYS_SetPvrEncryptMode (nEncType);
}

HUINT32 SVC_SYS_GetPvrEncryptMode( void )
{
	return PAL_SYS_GetPvrEncryptMode();
}

void *SVC_SYS_CryptMemAlloc (HUINT32 ulSize)
{
	return PAL_SYS_CRYPT_MemAlloc(ulSize);
}

HERROR SVC_SYS_Encrypt(HUINT8 *pucKey, HBOOL bBlock, /* Block:TRUE or Stream:FALSE */ HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize)
{
	if (NULL == pucPlain)		return ERR_FAIL;
	if (NULL == pucEncrypted)	return ERR_FAIL;
	if (0 == ulSize)			return ERR_FAIL;

	return PAL_SYS_Encrypt(pucKey, bBlock, pucPlain, pucEncrypted, ulSize);
}

HERROR SVC_SYS_Decrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize)
{
	if (NULL == pucPlain)		return ERR_FAIL;
	if (NULL == pucDecrypted)	return ERR_FAIL;
	if (0 == ulSize)			return ERR_FAIL;

	return PAL_SYS_Decrypt(pucKey, bBlock, pucPlain, pucDecrypted, ulSize);
}


//RGB LED Related Functions
HERROR SVC_SYS_SetRGBLedNone(DxLedId_e eLedId, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eLedColor = eDxRGBLED_COLOR_NONE;
		err = PAL_PANEL_SetRgbLed(eLedId, FALSE, FALSE, FALSE);
		pRgbInfo->bBlink = bBlink;
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedRed(DxLedId_e eLedId, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eLedColor = eDxRGBLED_COLOR_RED;
		err = PAL_PANEL_SetRgbLed(eLedId, TRUE, FALSE, FALSE);
		pRgbInfo->bBlink = bBlink;
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedGreen(DxLedId_e eLedId, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eLedColor = eDxRGBLED_COLOR_GREEN;
		err = PAL_PANEL_SetRgbLed(eLedId, FALSE, TRUE, FALSE);
		pRgbInfo->bBlink = bBlink;
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedBlue(DxLedId_e eLedId, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eLedColor = eDxRGBLED_COLOR_BLUE;
		err = PAL_PANEL_SetRgbLed(eLedId, FALSE, FALSE, TRUE);
		pRgbInfo->bBlink = bBlink;
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedYellow(DxLedId_e eLedId, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eLedColor = eDxRGBLED_COLOR_YELLOW;
		err = PAL_PANEL_SetRgbLed(eLedId, TRUE, TRUE, FALSE);
		pRgbInfo->bBlink = bBlink;
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedByColor(DxLedId_e eLedId, DxRgbLedColor_e eColor, HBOOL bBlink)
{
	HERROR err = ERR_FAIL;
	if(s_pfLedColor[eColor] != NULL)
	{
		err = s_pfLedColor[eColor](eLedId, bBlink);
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedOff(DxLedId_e eLedId)
{
	HERROR err = ERR_OK;

	err = PAL_PANEL_SetRgbLed(eLedId, FALSE, FALSE, FALSE);

	return err;
}

HERROR SVC_SYS_SetRGBLedDimmingLevel(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		pRgbInfo->stLedState.eDimmLevel = eDimmLevel;
		err = PAL_PANEL_SetLedsDimmingLevel(eLedId, (HUINT8)eDimmLevel);
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedDimmingValue(DxLedId_e eLedId, HUINT8 ucDimmValue)
{
	HERROR err = ERR_FAIL;
	SvcSys_RgbLedInfo_t *pRgbInfo = NULL;

	pRgbInfo = local_sys_GetRgbLedInfo(eLedId);
	if(pRgbInfo)
	{
		DxRgbLedDimmLevel_e eDimmLevel = eDxRGBLED_DIMMLEVEL_SOLID;
		if(ucDimmValue == 100)
		{
			eDimmLevel = eDxRGBLED_DIMMLEVEL_OFF;
		}
		else if(ucDimmValue < 100 && ucDimmValue > 50)
		{
			eDimmLevel = eDxRGBLED_DIMMLEVEL_DIMMED;
		}
		else
		{
			eDimmLevel = eDxRGBLED_DIMMLEVEL_SOLID;
		}

		pRgbInfo->stLedState.eDimmLevel = eDimmLevel;
		err = PAL_PANEL_SetLedsDimmingLevel(eLedId, ucDimmValue);
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedBlinkOn(DxLedId_e eLedId)
{
	HERROR err = ERR_OK;
	HINT32 i = 0;

	if( s_stRgbLedList != NULL )
	{
		for(i = 0 ; i < s_ulRgbLedNum ; ++i)
		{
			if(s_stRgbLedList[i].eLedId == eLedId)
			{
				s_stRgbLedList[i].bBlink = TRUE;
			}
		}
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedBlinkOff(DxLedId_e eLedId)
{
	HERROR err = ERR_OK;
	HINT32 i = 0;

	if( s_stRgbLedList != NULL )
	{
		for(i = 0 ; i < s_ulRgbLedNum ; ++i)
		{
			if(s_stRgbLedList[i].eLedId == eLedId)
			{
				s_stRgbLedList[i].bBlink = FALSE;

				// Back to last state
				if(s_pfLedColor[s_stRgbLedList[i].stLedState.eLedColor])
				{
					s_pfLedColor[s_stRgbLedList[i].stLedState.eLedColor](eLedId, FALSE);
					SVC_SYS_SetRGBLedDimmingLevel(eLedId, s_stRgbLedList[i].stLedState.eDimmLevel);
				}
			}
		}
	}

	return err;
}

HERROR SVC_SYS_SetRGBLedBlinkInterval(HULONG ulTime)
{
	HERROR err = ERR_OK;

	s_ulRgbBlinkInterval = ulTime;

	return err;
}

#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
void SVC_SYS_AgingLog_UsbInit(void)
{
	PAL_SYS_AgingLog_UsbInit();
}

HERROR SVC_SYS_AgingLog_GetPath(HCHAR *pszPath)
{
	return PAL_SYS_AgingLog_GetPath(pszPath);
}
#endif
