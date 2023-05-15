/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_power.c
	@brief	  Octo pal power module

	Description: ���� on/off/active standby � ���õ� ����� �����ϴ� ���.\n
	Module: PAL/PWR			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_power.h>
#include <pal_scart.h>
#if defined(CONFIG_MW_CAS)
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 0)
#include <pal_sc.h>
#endif
#endif
#include <di_power.h>
#include <di_rtc.h>
#include <di_flash.h>
#include <di_key.h>

#include <napi.h>
#if defined(CONFIG_MW_INET_WOL)
#include <di_network.h>
#endif
#if defined(CONFIG_MW_INET_WLAN)
#include <di_wifi.h>
#endif
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT_WITHOUT_MICOM) // loopthru for HDR-3000T
#include <pal_channel.h>
#endif

/* VA-5SD �� �̽� �ӽ� ���� �ڵ� :
     ����Ʈī�� ���Žó� AC off �ÿʹ� �޸� ���Ĺ��� ���� �Ǵ� ���丮 ����Ʈ�ÿ��� SC CLK �� ����� ������ �־� ���Ĺ��� �Ǳ� ���� deactivate �� ���ִ� �ڵ带 �ӽ� �߰���.
     ���� Viaccess ���� VHDR-3000S �� ��� �̷� ó�� ���� ISO-7816 �� �����ϰ� �ٸ� CAS �𵨿����� ���̵带 ���� �� �־� �ϴ� VA-5SD �� ���ؼ��� �ӽ������� ó���ϵ��� ��.
     ���� DI ���� �߰� ������ �Ŀ� �� �ڵ�� ���ŵ� ������. */

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC WakeUpReason_t	s_WakeUpReason;
STATIC HUINT32			s_WakeUpTime;

#define ______________________________________________________________________

/*******************************************************************/
/*******************		Static Function  ***********************/
/*******************************************************************/

#define ______________________________________________________________________

/*******************************************************************/
/******************		Global API Function  ***********************/
/*******************************************************************/

void PAL_PWR_Init(void)
{
	DI_ERR_CODE			diErr;
	DI_POWER_REASON		wakeupReason;

	HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

	/* Cold boot �ÿ��� ���������κ��� wake up reason�� �о� ������ ���� �Ŀ��� normal <-> standby ���� �󿡼� �ٲ� �� �ִ�. */
	diErr = DI_POWER_GetWakeupReason(&wakeupReason);

	if(diErr != DI_ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	switch(wakeupReason)
	{
		case FRONT_KEY:
			s_WakeUpReason = eWAKEUP_BY_KEY;
			HxLOG_Print("\t\t\tMain board waked up by FRONT KEY\n");
			break;

		case IR_NEC_TYPE :
			s_WakeUpReason = eWAKEUP_BY_KEY;
			HxLOG_Print("\t\t\tMain board waked up by RCU KEY\n");
			break;

		case IR_JVC_TYPE :
			s_WakeUpReason = eWAKEUP_BY_KEY;
			HxLOG_Print("\t\t\tMain board waked up by RCU KEY\n");
			break;

		case SERIAL_COMMAND :
			s_WakeUpReason = eWAKEUP_BY_KEY;
			HxLOG_Print("\t\t\tMain board waked up by SERIAL COMMAND\n");
			break;

		case REAL_TIME_CLOCK:
			s_WakeUpReason = eWAKEUP_BY_TIMER;
			HxLOG_Print("\t\t\tMain board waked up by TIMER\n");
			break;

		case AC_ON:
			s_WakeUpReason = eWAKEUP_BY_ACPOWER;
			HxLOG_Print("\t\t\tMain board waked up by AC POWER\n");
			break;

		case WATCH_DOG:
			s_WakeUpReason = eWAKEUP_BY_ACPOWER;
			HxLOG_Print("\t\t\tMain board waked up by WATCH DOG\n");
			break;

		case HDMI_CEC:
			s_WakeUpReason = eWAKEUP_BY_HDMICEC;
			HxLOG_Print("\t\t\tMain board waked up by HDMI CEC\n");
			break;

		case SYSTEM_REBOOT:
			s_WakeUpReason = eWAKEUP_BY_REBOOT;
			HxLOG_Print("\t\t\tMain board waked up by SYSTEM_REBOOT\n");
			break;

		default:
			s_WakeUpReason = eWAKEUP_BY_KEY;
			HxLOG_Print("\t\t\tMain board waked up by ** UNKNOWN(%d) **. Mapping to KEY\n", wakeupReason);
			break;
	}

	HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
}


HERROR PAL_PWR_GetWakeUpReason(WakeUpReason_t *pWakeUpReason)
{
	*pWakeUpReason = s_WakeUpReason;
	return ERR_OK;
}

HERROR PAL_PWR_SetWakeUpReason(WakeUpReason_t wakeUpReason)
{
	s_WakeUpReason = wakeUpReason;
	return ERR_OK;
}

HERROR PAL_PWR_TurnOffMainBoard(HBOOL bPowerSaveMode)
{
	/* Power Saving ���� Off �� ��� DI�� ��� �غ� �۾� ���� ȣ��.. */
	if(bPowerSaveMode == TRUE)
	{
		HxLOG_Error("DI_POWER_OFFMODE_POWERSAVING_READY)\n");
		DI_POWER_SetPowerOff(DI_POWER_OFFMODE_POWERSAVING_READY);
	}

	/* Auto startup ��� ���� ����. */
	/* PAL_SCART_SetPath(eDxSCART_PATH_HWBYPASS); */

	if(bPowerSaveMode == TRUE)
	{
		HxLOG_Error("DI_POWER_OFFMODE_STANDBY_POWERSAVING\n");
		return DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_POWERSAVING);
	}
	else
	{
		HxLOG_Error("DI_POWER_OFFMODE_STANDBY_NORMAL\n");
		return DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_NORMAL);
	}
}

HERROR PAL_PWR_SetRtcTime(HUINT32 ulUnixTime)
{
	return DI_RTC_SetCurrentTime(ulUnixTime);
}

HERROR PAL_PWR_GetRtcTime(HUINT32 *pulUnixTime)
{
	return DI_RTC_GetCurrentTime(pulUnixTime);
}

HERROR PAL_PWR_SetWakeUpTime(HUINT32 ulUnixTime)
{
	s_WakeUpTime = ulUnixTime;
	return DI_RTC_SetOnTime(ulUnixTime);
}

HERROR PAL_PWR_EnableWakeUpTime(void)
{
	s_WakeUpTime = 0;
	return DI_RTC_OnTimerOnOff(TRUE);
}

HERROR PAL_PWR_DisableWakeUpTime(void)
{
	s_WakeUpTime = 0;
	return DI_RTC_OnTimerOnOff(FALSE);
}

HERROR PAL_PWR_Deactivate_SC(void)
{
#if defined(CONFIG_MW_CAS)
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 0)
	HERROR			hErr;
	PalSc_CardStatus_e	eStatus = ePAL_SC_CARDSTATUS_NOT_PRESENT;
	HUINT32 		ulNumScSlot = PAL_SC_GetNumberOfModule();
	HUINT32 		i;

	for (i = 0; i < ulNumScSlot; i++)
	{
		hErr = PAL_SC_GetCardStatus(i, &eStatus);
		if (hErr == ERR_OK && eStatus == ePAL_SC_CARDSTATUS_PRESENT)
		{
			HxLOG_Print("%s : calling PAL_SC_Deactivate(%d) ...\n", __FUNCTION__, i);
			PAL_SC_Deactivate(i);
		}
	}
#endif
#endif
	return ERR_OK;
}

HERROR PAL_PWR_WIFIPower(HBOOL bOnOff)
{
#if defined(CONFIG_MW_INET_WLAN)
	HUINT32					ulDevCnt, i;
	HERROR					eRet;
	NAPINetConfInfo_t		Info;

	ulDevCnt = NAPI_NetConf_GetCount();

	for(i=0; i<ulDevCnt; i++)
	{
		HxSTD_MemSet(&Info, 0x00, sizeof(NAPINetConfInfo_t));

		eRet = NAPI_NetConf_GetInfo(i, &Info);
		if ( eRet == ERR_OK	&& Info.type == eNAPI_NETCONF_WIFI)
		{
			HxLOG_Debug("Set Wifi-power [%d]\n",bOnOff);
			if(bOnOff == TRUE)
			{
				DI_WIFI_On(i);
			}
			else
			{
				DI_WIFI_Off(i);
			}
		}
	}
#endif
	return ERR_OK;
}

HERROR PAL_PWR_MountNandFlash(void)
{
	return DI_NANDFLASH_MountPartitions();
}

HERROR PAL_PWR_UmountNandFlash(void)
{
	return DI_NANDFLASH_UmountPartitions();
}

HERROR PAL_PWR_Reboot(void)
{
	return DI_POWER_SetMainChipReset();
}

HERROR PAL_PWR_SetWakeupRcuKey(HUINT32 keyGroup)
{
	HUINT32 keyGroupIndex = 1;

	HxLOG_Warning("keyGroup = %d\n", keyGroup);
	while(keyGroupIndex != eWAKEUP_RCUKEY_MAX)
	{
		DI_KEY_WAKEUP_KEY_TYPE	nKeyType	=	DI_KEY_WAKEUP_KEY_TYPE_MAX;
		DI_KEY_REMOCON		nRcuKey =	DI_KEY_REMOCON_UNKNOWN;

		switch(keyGroupIndex)
		{
			case eWAKEUP_RCUKEY_GROUP_NUMBER:
				nKeyType	=	DI_KEY_WAKEUP_KEY_TYPE_NUMBER;
				break;

			case eWAKEUP_RCUKEY_GROUP_CH_UP_DOWN:
				nKeyType	=	DI_KEY_WAKEUP_KEY_TYPE_CH_UP_DOWN;
				break;

			case eWAKEUP_RCUKEY_GROUP_ARROW:
				nKeyType	=	DI_KEY_WAKEUP_KEY_TYPE_ARROW;
				break;

			case eWAKEUP_RCUKEY_GROUP_SINGLE:
				nKeyType	=	DI_KEY_WAKEUP_KEY_TYPE_SINGLE;
				// nRcuKey �� single group �ƴϰ��� DI ���� �ƹ��� �ǹ� ���� ó���Ǵ���.
				// obama������ default �� power Ű�� set ������
				nRcuKey =	DI_KEY_REMOCON_STANDBY;
				break;

			default:
				break;
		}
		if((keyGroupIndex & keyGroup) != 0)
		{
			HxLOG_Warning("DI_KEY_SetWakeupRcuKey index[0x%x]\n", keyGroupIndex);
			if (DI_KEY_SetWakeupRcuKey(nKeyType, nRcuKey) != DI_ERR_OK)
			{
				HxLOG_Error("DI_KEY_SetWakeupRcuKey() failed.. nKeyType(%d) nRcuKey(0x%X) \n", nKeyType, nRcuKey);
			}
		}

		keyGroupIndex = keyGroupIndex << 1;
	}
	return ERR_OK;
}

HERROR	PAL_PWR_ClearWakeupRcuKey(void)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	HxLOG_Print(HxANSI_COLOR_YELLOW("called DI_KEY_ClearWakeupRcuKey\n"));
	return DI_KEY_ClearWakeupRcuKey(); /* DI_KEY_ClearWakeupRcuKey�� S3 Mode ��� �ÿ��� ����� */
#else
	return ERR_OK;
#endif
}

HERROR	PAL_PWR_SetWakeOnLAN(HBOOL bOn_Off)
{
#if defined(CONFIG_MW_INET_WOL)
	HUINT32					ulDevCnt, i;
	HERROR					eRet;
	NAPINetConfInfo_t		Info;

	ulDevCnt = NAPI_NetConf_GetCount();

	for(i=0; i<ulDevCnt; i++)
	{
		HxSTD_MemSet(&Info, 0x00, sizeof(NAPINetConfInfo_t));

		eRet = NAPI_NetConf_GetInfo(i, &Info);
		if ( (eRet == ERR_OK)
			&& (Info.type == eNAPI_NETCONF_LAN) )
		{
			HxLOG_Debug("[%s,%d] set WOL .  devID : 0x%X , name : %s\n",__FUNCTION__,__LINE__,
				i, Info.name);
			DI_NETWORK_SetWakeOnLAN(i, bOn_Off);
		}
	}
	return ERR_OK;
#else
	return ERR_FAIL;
#endif

}

HERROR PAL_PWR_PrepareFastBootStandby(void)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	if(DI_POWER_PrepareStandby(DI_PM_POWERSTATE_DEEP_SLEEP) == DI_ERR_OK)
	{
		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}

HERROR PAL_PWR_SetFastBootStandby(HINT32 nSleepMode,HINT32 nBootMode,HBOOL bPowerSavingStandby)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	DI_PM_WAKEUP_SRC stSrc;
	UNIXTIME		ulUnixTime = HLIB_STD_GetSystemTime();

	stSrc.bIR = TRUE;
	stSrc.bFrontKey = TRUE;
#if defined(CONFIG_MW_INET_WOL)
#if defined(CONFIG_PROD_HGS1000S)
 /*HGS-1000S Senario : if power saving mode is off, Wake On Lan is on.*/
	stSrc.bWOL = !bPowerSavingStandby;
#else
	stSrc.bWOL = TRUE;
#endif
#else
	stSrc.bWOL = FALSE;
#endif

	//s_WakeUpTime = ulUnixTime + 60;	// For testing
	if(s_WakeUpTime == 0)
	{
		stSrc.bTimer = FALSE;
		HxLOG_Print("Timer Off\n");
	}
	else
	{
		HERROR 	hErr;
		HINT32	offsetTime = 0;

		hErr = HLIB_DATETIME_GetTimeOffsetWithDst(&offsetTime);

		HxLOG_Print("s_WakeUpTime : %x ulUnixTime : %x offset : %x\n", s_WakeUpTime, ulUnixTime, offsetTime * SECONDS_PER_MIN);
		if((ERR_OK == hErr)
			&& (s_WakeUpTime > ulUnixTime + (offsetTime * SECONDS_PER_MIN))
			)
		{
			stSrc.stInfo.stTimer.ulInterval = s_WakeUpTime - ulUnixTime - (offsetTime * SECONDS_PER_MIN);
			stSrc.bTimer = TRUE;
		}
		else
		{
			stSrc.bTimer = FALSE;
		}
		HxLOG_Print("Timer %s After %d\n",stSrc.bTimer?"On":"Off", stSrc.stInfo.stTimer.ulInterval);
	}
	HxLOG_Print("IR	%s\n", stSrc.bIR?"On":"Off");
	HxLOG_Print("FrontKey %s\n", stSrc.bFrontKey?"On":"Off");
	HxLOG_Print("WOL %s\n", stSrc.bWOL?"On":"Off");

#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT_WITHOUT_MICOM)
#if defined(CONFIG_MW_CH_LOOP_THROUGH)
	if (bPowerSavingStandby == TRUE)
	{
		PAL_CH_SetLoopthroughGPIO(FALSE);
	}
	else
	{
		PAL_CH_SetLoopthroughGPIO(TRUE);
	}
#endif
#endif

	if(DI_POWER_SetStandby(nSleepMode, stSrc, nBootMode) == DI_ERR_OK)
	{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT_WITHOUT_MICOM)
#if defined(CONFIG_MW_CH_LOOP_THROUGH)
		PAL_CH_SetLoopthroughGPIO(TRUE);
#endif
#endif
		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}

HERROR PAL_PWR_CancelFastBootStandby(void)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	return DI_POWER_CancelStandby ();
#endif
	return ERR_OK;
}


