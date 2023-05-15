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
 * @file	  		nx_prism_spec.c
 *
 * 	Description:  	Check UI Spec									\n
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
#include <nx_common.h>
#include <nx_prism_app.h>
#include <nx_prism_util.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NAPP_UTIL_DTI_MAX						11
#define	NAPP_UTIL_VFD_LEN_16					16
#define	NAPP_UTIL_VFD_LEN_10					10
#define	NAPP_UTIL_VFD_LEN_7						7


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HUINT8	*nx_spec_GetPanelString_7Segment(NXUI_PANEL_STR_Type_t ePanelStrType);
static HUINT8	*nx_spec_GetPanelString_VFD_Len16(NXUI_PANEL_STR_Type_t ePanelStrType);
static HUINT8	*nx_spec_GetPanelString_VFD_Len10(NXUI_PANEL_STR_Type_t ePanelStrType);
static HUINT8	*nx_spec_GetPanelString_VFD_Len7(NXUI_PANEL_STR_Type_t ePanelStrType);

static HBOOL	nx_spec_GetHomeMenuUiSupportForMBC(eNxUiUtilHomeMenuSpec_t eNxHomeMenuSpec);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static HUINT8	*nx_spec_GetPanelString_7Segment(NXUI_PANEL_STR_Type_t ePanelStrType)
{
	HUINT8	*pszString = NULL;


	switch (ePanelStrType)
	{
		case eNX_PANEL_STR_Starting:	pszString = (HUINT8 *)"BOOT";			break;
		case eNX_PANEL_STR_NoChannel:	pszString = (HUINT8 *)"----";			break;
		case eNX_PANEL_STR_Mute:		pszString = (HUINT8 *)"MUTE";			break;
		case eNX_PANEL_STR_Volume:		pszString = (HUINT8 *)"V";				break;
		case eNX_PANEL_STR_DCN:			pszString = (HUINT8 *)NULL;				break;
		case eNX_PANEL_STR_Wizard:		pszString = (HUINT8 *)"INST";			break;
		case eNX_PANEL_STR_Guide:		pszString = (HUINT8 *)"EPG";			break;
		case eNX_PANEL_STR_Menu:		pszString = (HUINT8 *)"MENU";			break;
		case eNX_PANEL_STR_ChList:		pszString = (HUINT8 *)NULL;				break;
		case eNX_PANEL_STR_Settings:	pszString = (HUINT8 *)"MENU";			break;

		case eNX_PANEL_STR_Recordings:	pszString = (HUINT8 *)"REC";			break;
		case eNX_PANEL_STR_StartRec:	pszString = (HUINT8 *)"FILE";			break;

		case eNX_PANEL_STR_Media:		pszString = (HUINT8 *)"FILE";			break;
		case eNX_PANEL_STR_Video:		pszString = (HUINT8 *)"FILE";			break;
		case eNX_PANEL_STR_Music:		pszString = (HUINT8 *)"FILE";			break;
		case eNX_PANEL_STR_Photo:		pszString = (HUINT8 *)"FILE";			break;
		case eNX_PANEL_STR_MediaPlay:	pszString = (HUINT8 *)"PLAY";			break;

		case eNX_PANEL_STR_Play:		pszString = (HUINT8 *)"PLAY";			break;
		case eNX_PANEL_STR_Pause:		pszString = (HUINT8 *)"PAUS";			break;
		case eNX_PANEL_STR_FRX:			pszString = (HUINT8 *)"FBXX";			break;
		case eNX_PANEL_STR_FFX:			pszString = (HUINT8 *)"FFXX";			break;

		case eNX_PANEL_STR_Portal:		pszString = (HUINT8 *)"APPS";			break;
		case eNX_PANEL_STR_TVApps:		pszString = (HUINT8 *)"APPS";			break;

		default:						pszString = (HUINT8 *)NULL;				break;
	}

	return	pszString;
}

static HUINT8	*nx_spec_GetPanelString_VFD_Len16(NXUI_PANEL_STR_Type_t ePanelStrType)
{
	HUINT8	*pszString = NULL;


	switch (ePanelStrType)
	{
		case eNX_PANEL_STR_Starting:	pszString = (HUINT8 *)"STARTING... ";		break;
		case eNX_PANEL_STR_NoChannel:	pszString = (HUINT8 *)"-NO CHANNEL-";		break;
		case eNX_PANEL_STR_Mute:		pszString = (HUINT8 *)"-MUTE-";				break;
		case eNX_PANEL_STR_Volume:		pszString = (HUINT8 *)"VOLUME";				break;
		case eNX_PANEL_STR_DCN:			pszString = (HUINT8 *)"CH";					break;
		case eNX_PANEL_STR_Wizard:		pszString = (HUINT8 *)"-WIZARD-";			break;
		case eNX_PANEL_STR_Guide:		pszString = (HUINT8 *)"-TV GUIDE-";			break;
		case eNX_PANEL_STR_Menu:		pszString = (HUINT8 *)"-MENU-";				break;
		case eNX_PANEL_STR_ChList:		pszString = (HUINT8 *)"-CH LIST-";			break;
		case eNX_PANEL_STR_Settings:	pszString = (HUINT8 *)"-SETTINGS-";			break;

		case eNX_PANEL_STR_Recordings:	pszString = (HUINT8 *)"-RECORDINGS-";		break;
		case eNX_PANEL_STR_StartRec:	pszString = (HUINT8 *)"START RECORDING";	break;

		case eNX_PANEL_STR_Media:		pszString = (HUINT8 *)"-MEDIA-";			break;
		case eNX_PANEL_STR_Video:		pszString = (HUINT8 *)"-VIDEO-";			break;
		case eNX_PANEL_STR_Music:		pszString = (HUINT8 *)"-MUSIC-";			break;
		case eNX_PANEL_STR_Photo:		pszString = (HUINT8 *)"-PHOTO-";			break;
		case eNX_PANEL_STR_MediaPlay:	pszString = (HUINT8 *)"MEDIA PLAY";			break;

		case eNX_PANEL_STR_Play:		pszString = (HUINT8 *)"PLAY";				break;
		case eNX_PANEL_STR_Pause:		pszString = (HUINT8 *)"PAUS";				break;
		case eNX_PANEL_STR_FRX:			pszString = (HUINT8 *)"FRX";				break;
		case eNX_PANEL_STR_FFX:			pszString = (HUINT8 *)"FFX";				break;

		case eNX_PANEL_STR_Portal:		pszString = (HUINT8 *)"HUMAX TV PORTAL";	break;
		case eNX_PANEL_STR_TVApps:		pszString = (HUINT8 *)"-TV APPS-";			break;

		default:						pszString = (HUINT8 *)NULL;					break;
	}

	return	pszString;
}

static HUINT8	*nx_spec_GetPanelString_VFD_Len10(NXUI_PANEL_STR_Type_t ePanelStrType)
{
	HUINT8	*pszString = NULL;


	switch (ePanelStrType)
	{
		case eNX_PANEL_STR_Starting:	pszString = (HUINT8 *)"STARTING... ";		break;
		case eNX_PANEL_STR_NoChannel:	pszString = (HUINT8 *)"-NO CHANNEL-";	break;
		case eNX_PANEL_STR_Mute:		pszString = (HUINT8 *)"-MUTE-";			break;
		case eNX_PANEL_STR_Volume:		pszString = (HUINT8 *)"VOLUME";			break;
		case eNX_PANEL_STR_DCN:			pszString = (HUINT8 *)"CH";				break;
		case eNX_PANEL_STR_Wizard:		pszString = (HUINT8 *)"-WIZARD-";		break;
		case eNX_PANEL_STR_Guide:		pszString = (HUINT8 *)"-TV GUIDE-";		break;
		case eNX_PANEL_STR_Menu:		pszString = (HUINT8 *)"-MENU-";			break;
		case eNX_PANEL_STR_ChList:		pszString = (HUINT8 *)"-CH LIST-";		break;
		case eNX_PANEL_STR_Settings:	pszString = (HUINT8 *)"-SETTINGS-";		break;

		case eNX_PANEL_STR_Recordings:	pszString = (HUINT8 *)"RECORDING";		break;
		case eNX_PANEL_STR_StartRec:	pszString = (HUINT8 *)"RECORDING";		break;

		case eNX_PANEL_STR_Media:		pszString = (HUINT8 *)"-MEDIA-";		break;
		case eNX_PANEL_STR_Video:		pszString = (HUINT8 *)"-VIDEO-";		break;
		case eNX_PANEL_STR_Music:		pszString = (HUINT8 *)"-MUSIC-";		break;
		case eNX_PANEL_STR_Photo:		pszString = (HUINT8 *)"-PHOTO-";		break;
		case eNX_PANEL_STR_MediaPlay:	pszString = (HUINT8 *)"MEDIA PLAY";		break;

		case eNX_PANEL_STR_Play:		pszString = (HUINT8 *)"PLAY";			break;
		case eNX_PANEL_STR_Pause:		pszString = (HUINT8 *)"PAUS";			break;
		case eNX_PANEL_STR_FRX:			pszString = (HUINT8 *)"FRX";			break;
		case eNX_PANEL_STR_FFX:			pszString = (HUINT8 *)"FFX";			break;

		case eNX_PANEL_STR_Portal:		pszString = (HUINT8 *)"TV PORTAL";		break;
		case eNX_PANEL_STR_TVApps:		pszString = (HUINT8 *)"-TV APPS-";		break;

		default:						pszString = (HUINT8 *)NULL;				break;
	}

	return	pszString;
}

static HUINT8	*nx_spec_GetPanelString_VFD_Len7(NXUI_PANEL_STR_Type_t ePanelStrType)
{
	HUINT8	*pszString = NULL;


	switch (ePanelStrType)
	{
		case eNX_PANEL_STR_Starting:	pszString = (HUINT8 *)"START";		break;
		case eNX_PANEL_STR_NoChannel:	pszString = (HUINT8 *)"NO CH";			break;
		case eNX_PANEL_STR_Mute:		pszString = (HUINT8 *)"MUTE";			break;
		case eNX_PANEL_STR_Volume:		pszString = (HUINT8 *)"VOL";			break;
		case eNX_PANEL_STR_DCN:			pszString = (HUINT8 *)"CH";				break;
		case eNX_PANEL_STR_Wizard:		pszString = (HUINT8 *)"WIZARD";			break;
		case eNX_PANEL_STR_Guide:		pszString = (HUINT8 *)"GUIDE";			break;
		case eNX_PANEL_STR_Menu:		pszString = (HUINT8 *)"MENU";			break;
		case eNX_PANEL_STR_ChList:		pszString = (HUINT8 *)NULL;				break;
		case eNX_PANEL_STR_Settings:	pszString = (HUINT8 *)"SETTING";		break;

		case eNX_PANEL_STR_Recordings:	pszString = (HUINT8 *)"RECS";			break;
		case eNX_PANEL_STR_StartRec:	pszString = (HUINT8 *)"RECS";			break;

		case eNX_PANEL_STR_Media:		pszString = (HUINT8 *)"MEDIA";			break;
		case eNX_PANEL_STR_Video:		pszString = (HUINT8 *)"VIDEO";			break;
		case eNX_PANEL_STR_Music:		pszString = (HUINT8 *)"MUSIC";			break;
		case eNX_PANEL_STR_Photo:		pszString = (HUINT8 *)"PHOTO";			break;
		case eNX_PANEL_STR_MediaPlay:	pszString = (HUINT8 *)"PLAY";			break;

		case eNX_PANEL_STR_Play:		pszString = (HUINT8 *)"PLAY";			break;
		case eNX_PANEL_STR_Pause:		pszString = (HUINT8 *)"PAUS";			break;
		case eNX_PANEL_STR_FRX:			pszString = (HUINT8 *)"FBXX";			break;
		case eNX_PANEL_STR_FFX:			pszString = (HUINT8 *)"FFXX";			break;

		case eNX_PANEL_STR_Portal:		pszString = (HUINT8 *)"TV APPS";		break;
		case eNX_PANEL_STR_TVApps:		pszString = (HUINT8 *)"TV APPS";		break;

		default:						pszString = (HUINT8 *)NULL;				break;
	}

	return	pszString;
}



static HBOOL	nx_spec_GetHomeMenuUiSupportForMBC(eNxUiUtilHomeMenuSpec_t eNxHomeMenuSpec)
{
	HBOOL			bSupport = FALSE;

	switch (eNxHomeMenuSpec)
	{
		case eNxUtil_HomeMenu_LiveTv:		bSupport = TRUE;		return	bSupport;
		case eNxUtil_HomeMenu_Guide:		bSupport = TRUE;		return	bSupport;
		case eNxUtil_HomeMenu_Record:
#if defined(CONFIG_MW_MM_PVR)
			bSupport = TRUE;
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_MediaCenter:
#if defined(CONFIG_MW_MM_MEDIA)
#if defined(CONFIG_PROD_M1)
			bSupport = FALSE;
#else
			bSupport = TRUE;
#endif
#endif	// defined(CONFIG_MW_MM_MEDIA)
			return	bSupport;

		case eNxUtil_HomeMenu_Settings:		bSupport = TRUE;		return	bSupport;

		case eNxUtil_HomeMenu_TVApps:
#if defined(CONFIG_OP_SHAHID_APPS)
			bSupport = TRUE;
#else
			bSupport = FALSE;
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_Schedule:
#if defined(CONFIG_MW_MM_PVR)
			bSupport = FALSE;
#else
			bSupport = TRUE;
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_Recent:
#if defined(CONFIG_MW_MM_MEDIA) || defined(CONFIG_MW_MM_PVR)
	#if defined(CONFIG_PROD_M1)
			bSupport = FALSE;
	#else
			bSupport = TRUE;
	#endif
#else
			bSupport = FALSE;
#endif
			return bSupport;

		default:
			return	FALSE;
	}
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
void	NX_PRISM_SPEC_GetHomeMenuUiSupportNum(HUINT32 *ulCount)
{
	HBOOL		bSupport = FALSE;
	HUINT32		i = 0, ulSupportNum = 0;

	*ulCount = 0;
	for (i = eNxUtil_HomeMenu_LiveTv; i < eNxUtil_HomeMenu_Max; i++)
	{
		bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport((eNxUiUtilHomeMenuSpec_t)i);
		if (bSupport == TRUE)
		{
			ulSupportNum++;
		}
	}

	*ulCount = ulSupportNum;
}


HBOOL	NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUiUtilHomeMenuSpec_t eNxHomeMenuSpec)
{
	HBOOL			bSupport = FALSE;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	NX_SYSTEM_GetOperatorType(&eOpType);
	// TODO: To be used when it is different depending on the op

	switch (eNxHomeMenuSpec)
	{
		case eNxUtil_HomeMenu_LiveTv:
			switch (eOpType)
			{
				default:
					bSupport = TRUE;
					break;
			}
			return	bSupport;

		case eNxUtil_HomeMenu_Guide:
			switch (eOpType)
			{
				default:
					bSupport = TRUE;
					break;
			}
			return	bSupport;

		case eNxUtil_HomeMenu_Record:
#if defined(CONFIG_MW_MM_PVR)
			switch (eOpType)
			{
				default:
					bSupport = TRUE;
					break;
			}
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_MediaCenter:
#if defined(CONFIG_MW_MM_MEDIA)
			switch (eOpType)
			{
				case eDxOPERATOR_MBC:
					bSupport = nx_spec_GetHomeMenuUiSupportForMBC(eNxHomeMenuSpec);
					break;
				default:
					bSupport = TRUE;
					break;
			}
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_Settings:
			switch (eOpType)
			{
				default:
					bSupport = TRUE;
					break;
			}
			return	bSupport;

		case eNxUtil_HomeMenu_TVApps:
// TODO: [JHLEE] Check other config
#if defined(CONFIG_MWC_OSD_PROFILE_BROWSER)
			switch (eOpType)
			{
				default:
					bSupport = TRUE;
					break;
			}
#endif
			return	bSupport;

		case eNxUtil_HomeMenu_Schedule:
			// Only Support the zapper only model
			switch (eOpType)
			{
				default:
#if defined(CONFIG_MW_MM_PVR)
					bSupport = FALSE;
#else
					bSupport = TRUE;
#endif

					break;
			}
			return	bSupport;

		case eNxUtil_HomeMenu_Recent:
#if defined(CONFIG_MW_MM_MEDIA) || defined(CONFIG_MW_MM_PVR)
			switch (eOpType)
			{
				case eDxOPERATOR_MBC:
					bSupport = nx_spec_GetHomeMenuUiSupportForMBC(eNxHomeMenuSpec);
					break;

				default:
					bSupport = TRUE;
					break;
			}
#endif
			return bSupport;
		case eNxUtil_HomeMenu_Shahid:
#if defined(CONFIG_OP_SHAHID_APPS)
			return TRUE;
#else
			return FALSE;
#endif

		default:
			return	FALSE;
	}
}


HBOOL	NX_PRISM_SPEC_GetUiSupport(eNxUiUtilSpec_t eNxUiUtilSpec)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	NX_SYSTEM_GetOperatorType(&eOpType);
	// TODO: To be used when it is different depending on the op

	switch (eNxUiUtilSpec)
	{
		case eNxUtil_UI_DispSubttlTypeIcon:
			return	TRUE;

		default:
			return	FALSE;
	}
}


HBOOL	NX_PRISM_SPEC_GetUiSupportZapperOnly(void)
{
	HBOOL	bZapperOnly = TRUE;

#if defined(CONFIG_MW_MM_PVR)
	bZapperOnly = FALSE;
#elif defined(CONFIG_MW_MM_MEDIA)
	bZapperOnly = FALSE;
#else
	bZapperOnly = TRUE;
#endif

	return	bZapperOnly;
}


HBOOL	NX_PRISM_SPEC_GetUiSupportPassiveStandby(void)
{
	HERROR	hError = ERR_FAIL;
	HBOOL 	bSupportedPassive = TRUE;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	/*
		This Function was only used display menu add/Delete
	*/

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! ");
		eOpType = eDxOPERATOR_None;
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			bSupportedPassive = FALSE;
			break;

		default:
			bSupportedPassive = TRUE;
			break;
	}


#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	bSupportedPassive = FALSE;
#else
	bSupportedPassive = TRUE;
#endif

	return bSupportedPassive;
}


HBOOL	NX_PRISM_SPEC_GetGoPassiveStandby(void)
{
	HBOOL 	bGoPassive = FALSE, bSupportedPassive = FALSE;
	HERROR	hError = ERR_FAIL;
	HINT32	ePowerSavinginStandby = eAPK_POWER_STANDBYTYPE_NORMAL;


	bSupportedPassive = NX_PRISM_SPEC_GetUiSupportPassiveStandby();
	if (bSupportedPassive == FALSE)
	{
		bGoPassive = FALSE;
	}
	else
	{
		hError = NX_SYSTEM_GetStandByPowerMode(&ePowerSavinginStandby);
		if (hError == ERR_FAIL)
		{
			bGoPassive = FALSE;
		}
		else
		{
			switch (ePowerSavinginStandby)
			{
				case eAPK_POWER_STANDBYTYPE_NORMAL:
					bGoPassive = FALSE;
					break;
				case eAPK_POWER_STANDBYTYPE_ASITWAS:
					// TODO: How to used this param
					bGoPassive = FALSE;
					break;
				case eAPK_POWER_STANDBYTYPE_POWERSAVING:
					bGoPassive = TRUE;
					break;
				default:
					bGoPassive = FALSE;
					break;
			}
		}
	}

	return	bGoPassive;
}


HUINT8	*NX_PRISM_SPEC_GetPanelString(NXUI_PANEL_STR_Type_t ePanelStrType)
{
	HINT32		lVfdLen = 0;

	if (ePanelStrType >= eNX_PANEL_STR_Max)
	{
		NX_APP_Error("Error!!! Invalid PanelStrType:(%u)\n", ePanelStrType);
		return	(HUINT8 *)NULL;
	}

	lVfdLen = (HINT32)CONFIG_MW_SYS_STRING_LEN;

#if defined(CONFIG_MW_SYS_7SEG)
	return	nx_spec_GetPanelString_7Segment(ePanelStrType);
#elif (defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_OP_MBC))
	if ((lVfdLen >= NAPP_UTIL_VFD_LEN_7) && (lVfdLen < NAPP_UTIL_VFD_LEN_10))
	{
		return	nx_spec_GetPanelString_VFD_Len7(ePanelStrType);
	}
	else if ((lVfdLen >= NAPP_UTIL_VFD_LEN_10) && (lVfdLen < NAPP_UTIL_VFD_LEN_16))
	{
		return	nx_spec_GetPanelString_VFD_Len10(ePanelStrType);
	}
	else if (lVfdLen == NAPP_UTIL_VFD_LEN_16)
	{
		return	nx_spec_GetPanelString_VFD_Len16(ePanelStrType);
	}
	else
	{
		return	(HUINT8 *)NULL;
	}
#else
	// No Front Panel
	return	(HUINT8 *)NULL;
#endif

}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

