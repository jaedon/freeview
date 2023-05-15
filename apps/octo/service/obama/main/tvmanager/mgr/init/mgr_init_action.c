/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/****************************************************************************
*
* File Name   : mgr_init_action.c
*
* Description : AP module initialization application
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/01/16		initiated					JK Baek
* 2008/02/11		renamed					JK Baek
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <otl.h>
#include <get_enum_str.h>
#include <octo_common.h>
#include <hlib.h>
#include <hapi.h>
#include <bus.h>
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
/** @brief local header file  */
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_av.h>
#include <svc_output.h>

#if defined(CONFIG_MW_MM_PVR)
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#endif

#if defined(CONFIG_MW_MM_HDD_FORMAT_JIGSW)
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#endif

#include <mgr_common.h>
#include <mgr_spec.h>
#include <mgr_action.h>
#include <mgr_pvr_action.h>
#include <mgr_live.h>
#include <mgr_storage.h>
#include <mgr_coldboot.h>
#include <mgr_time.h>
#include <mgr_3rdparty.h>
#include <mgr_swup.h>
#include <mgr_storage.h>
#include <mgr_epg.h>
#include <mgr_cas.h>
#include <mgr_setting.h>
#include <mgr_action.h>
#include <mgr_schedule.h>
#include <mgr_hbbtv.h>
#include <mgr_init.h>
#include <mgr_satip.h>
#include <pmgr_model.h>
#if defined(CONFIG_HOMMA_RC)
#include <mgr_rc.h>
#endif

#include <oapi_manager.h>

#if defined(CONFIG_MW_LOGCAPTURE)
#include <mgr_logcapture.h>
#endif

#include <tapi.h>

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */


/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */


STATIC HERROR mgr_GetSystemConfig(SvcSi_CfgSystem_t *pstSystem)
{
	HERROR				hErr;
	DxOperator_e		eOperator = eDxOPERATOR_NOT_DEFINED;
	HxCountry_e 		eCountryId = eCountryID_NONE;
	HxLANG_Id_e			eMainLangId = eLangID_MAX;

	HxSTD_MemSet(pstSystem, 0x00, sizeof(SvcSi_CfgSystem_t));

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		eCountryId = eCountryID_NONE;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperator, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
		eOperator = eDxOPERATOR_NOT_DEFINED;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMainLangId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
		eMainLangId = eLangID_MAX;
	}

	HxLOG_Debug("Country(%s)  Operator(%s) \n",HLIB_COUNTRY_IdToFullName(eCountryId), OTL_CONV_Operator2String(eOperator));

	pstSystem->eCountry		= eCountryId;
	pstSystem->eOperator	= eOperator;
	pstSystem->eOsdLang		= eMainLangId;

	return ERR_OK;
}


STATIC HERROR mgr_GetAudioOutputConfig(SvcSi_CfgAudioOutput_t *pstAudioOutput, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR					 hErr;
	HBOOL					 bMultiUser, bMultiMenu;
	DxDigitalAudioFormat_e	 eHdmiAudioOut, eSpdifAudioOut;
	DxStereoSelect_e		 eStereoSelect;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	// Get Audio Output Configuration
	eStereoSelect = eDxSTEREO_SELECT_STEREO;
	bMultiUser = bMultiMenu = FALSE;

	bMultiUser = FALSE;
	if ((TRUE == pstUserSettings->stAudio.bUserSelected) && (TRUE == pstUserSettings->stAudio.bDolby))
	{
		bMultiUser = TRUE;
	}

	eHdmiAudioOut = eSpdifAudioOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT, (HUINT32 *)&eHdmiAudioOut, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT) Error!!\n");
		eHdmiAudioOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT, (HUINT32 *)&eSpdifAudioOut, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT) Error!!\n");
		eSpdifAudioOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

#if 0
	(void)SVC_OUT_GetHdmiAudioFormat(&eHdmiAudioOut);
	(void)SVC_OUT_GetSpdifAudioFormat(&eSpdifAudioOut);
#endif
	if ((eDxDIGITALAUDIO_FORMAT_MULTICHANNEL == eHdmiAudioOut) || (eDxDIGITALAUDIO_FORMAT_MULTICHANNEL == eSpdifAudioOut))
	{
		bMultiMenu = TRUE;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT, (HUINT32 *)&eStereoSelect, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT) Error!!\n");
	}

	HxLOG_Debug("eStereoSelect(%d) bMultiUser(%s) bMultiMenu(%s) \n",
		OTL_ENUM2STR_StereoSelect(eStereoSelect), BOOL_TO_STR(bMultiUser), BOOL_TO_STR(bMultiMenu) );

	pstAudioOutput->eStereoSelect											= eStereoSelect;
	pstAudioOutput->abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_User]	= bMultiUser;
	pstAudioOutput->abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu]	= bMultiMenu;

	return ERR_OK;
}

STATIC HERROR mgr_GetAudioLanguageConfig(SvcSi_CfgAudioLang_t *pstAudioLanguage, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR			 hErr;
	HxLANG_Id_e 	 eSvcAudioLangId, eAudioLangId0, eAudioLangId1, eAudioLangIdUndefined, eAudioLangIdOriginal, eAudioLangIdDefault;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	eSvcAudioLangId = eAudioLangId0 = eAudioLangId1 = eAudioLangIdUndefined =  eAudioLangIdOriginal =  eAudioLangIdDefault = eLangID_MAX;

	if(pstUserSettings->stAudio.bUserSelected)
	{
		eSvcAudioLangId = pstUserSettings->stAudio.eLang;
		pstAudioLanguage->nPid = pstUserSettings->stAudio.nPid;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0) Error!!\n");
		eAudioLangId0 = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO1, (HUINT32 *)&eAudioLangId1, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO1) Error!!\n");
		eAudioLangId1 = eLangID_MAX;
	}


	// TODO: Get Undefined Lang, Original Lang, Default Lang from DB_PARAM !!!!!!
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED, (HUINT32 *)&eAudioLangIdUndefined, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED) Error!!\n");
		eAudioLangIdUndefined = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL, (HUINT32 *)&eAudioLangIdOriginal, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL) Error!!\n");
		eAudioLangIdOriginal = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT, (HUINT32 *)&eAudioLangIdDefault, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT) Error!!\n");
		eAudioLangIdDefault = eLangID_MAX;
	}

	if (eAudioLangId0 == eLangID_NONE)				eAudioLangId0 = eLangID_MAX;
	if (eAudioLangId1 == eLangID_NONE)				eAudioLangId1 = eLangID_MAX;
	if (eAudioLangIdUndefined == eLangID_NONE)		eAudioLangIdUndefined = eLangID_MAX;
	if (eAudioLangIdOriginal == eLangID_NONE)		eAudioLangIdOriginal = eLangID_MAX;
	if (eAudioLangIdDefault == eLangID_NONE)		eAudioLangIdDefault = eLangID_MAX;

	HxLOG_Debug("eSvcAudioLangId(%s) eAudioLangId0(%s) eAudioLangId1(%s) \n",
		HLIB_LANG_IdTo639(eSvcAudioLangId),
		HLIB_LANG_IdTo639(eAudioLangId0) ,
		HLIB_LANG_IdTo639(eAudioLangId1));
	HxLOG_Debug("eAudioLangIdUndefined(%s) eAudioLangIdOriginal(%s) eAudioLangIdDefault(%s)\n",
		HLIB_LANG_IdTo639(eAudioLangIdUndefined),
		HLIB_LANG_IdTo639(eAudioLangIdOriginal),
		HLIB_LANG_IdTo639(eAudioLangIdDefault));

	pstAudioLanguage->aeLangId[eSvcSi_CFG_AUD_LANG_User]			= eSvcAudioLangId;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_AUD_LANG_User_With_Pid]	= eSvcAudioLangId;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_AUD_LANG_0] 				= eAudioLangId0;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_AUD_LANG_1] 				= eAudioLangId1;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]	= eAudioLangIdUndefined;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]		= eAudioLangIdOriginal;
	pstAudioLanguage->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]		= eAudioLangIdDefault;

	return ERR_OK;
}

STATIC HERROR mgr_GetAudioDescConfig(SvcSi_CfgAudioDesc_t *pstAudioDesc, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR			 hErr;
	HBOOL			 bAdUser, bAdMenu;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	bAdUser = bAdMenu = FALSE;

	// Get Audio Description Configuration
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAdMenu, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION) Error!!\n");
		bAdMenu = FALSE;
	}

	HxLOG_Debug("bAdUser(%s) bAdMenu(%s) \n",BOOL_TO_STR(bAdUser), BOOL_TO_STR(bAdMenu));

	pstAudioDesc->abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_User] = bAdUser;
	pstAudioDesc->abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_Menu] = bAdMenu;

	return ERR_OK;
}


STATIC HERROR mgr_GetSubtitleLanguageConfig(SvcSi_CfgSubtitleLang_t *pstSubtitleLang, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR			 hErr;
	HxLANG_Id_e 	 eSubtitleLangIdUser, eSubtitleLangId0, eSubtitleLangId1, eSubtitleLangIdUndefined, eSubtitleLangIdOriginal, eSubtitleLangIdDefault;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	eSubtitleLangIdUser = eSubtitleLangId0 = eSubtitleLangId1 = eSubtitleLangIdUndefined = eSubtitleLangIdOriginal = eSubtitleLangIdDefault = eLangID_MAX;
	pstSubtitleLang->bUserSelectedOff = pstUserSettings->stSubtitle.bUserSelectedOff;

	if(pstUserSettings->stSubtitle.bUserSelected)
	{
		eSubtitleLangIdUser = pstUserSettings->stSubtitle.eLang;
		pstSubtitleLang->nTrack = pstUserSettings->stSubtitle.nTrack;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSubtitleLangId0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0) Error!!\n");
		eSubtitleLangId0 = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE1, (HUINT32 *)&eSubtitleLangId1, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE1) Error!!\n");
		eSubtitleLangId1 = eLangID_MAX;
	}

	// TODO: Get Undefined Lang, Original Lang, Default Lang from DB_PARAM !!!!!!
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED, (HUINT32 *)&eSubtitleLangIdUndefined, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED) Error!!\n");
		eSubtitleLangIdUndefined = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL, (HUINT32 *)&eSubtitleLangIdOriginal, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL) Error!!\n");
		eSubtitleLangIdOriginal = eLangID_MAX;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT, (HUINT32 *)&eSubtitleLangIdDefault, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT) Error!!\n");
		eSubtitleLangIdDefault = eLangID_MAX;
	}

	if (eSubtitleLangIdUser == eLangID_NONE)			eSubtitleLangIdUser = eLangID_MAX;
	if (eSubtitleLangId0 == eLangID_NONE)				eSubtitleLangId0 = eLangID_MAX;
	if (eSubtitleLangId1 == eLangID_NONE)				eSubtitleLangId1 = eLangID_MAX;
	if (eSubtitleLangIdUndefined == eLangID_NONE)		eSubtitleLangIdUndefined = eLangID_MAX;
	if (eSubtitleLangIdOriginal == eLangID_NONE)		eSubtitleLangIdOriginal = eLangID_MAX;
	if (eSubtitleLangIdDefault == eLangID_NONE)			eSubtitleLangIdDefault = eLangID_MAX;

	HxLOG_Debug("eSubtitleLangIdUser(%s) eSubtitleLangId0(%s) eSubtitleLangId1(%s)\n",
		HLIB_LANG_IdTo639(eSubtitleLangIdUser),
		HLIB_LANG_IdTo639(eSubtitleLangId0),
		HLIB_LANG_IdTo639(eSubtitleLangId1));
	HxLOG_Debug("eSubtitleLangIdUndefined(%s) eSubtitleLangIdOriginal(%s) eSubtitleLangIdDefault(%s)\n",
		HLIB_LANG_IdTo639(eSubtitleLangIdUndefined),
		HLIB_LANG_IdTo639(eSubtitleLangIdOriginal),
		HLIB_LANG_IdTo639(eSubtitleLangIdDefault));

	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_User] 			= eSubtitleLangIdUser;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_User_With_Idx] = eSubtitleLangIdUser;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]				= eSubtitleLangId0;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]				= eSubtitleLangId1;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]		= eSubtitleLangIdUndefined;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]		= eSubtitleLangIdOriginal;
	pstSubtitleLang->aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]		= eSubtitleLangIdDefault;

	return ERR_OK;
}

STATIC HERROR mgr_GetSubtitleTypeConfig(SvcSi_CfgSubtitleType_t *pstSubtitleType, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR	 hErr;
	HBOOL 	 bHohUser, bHohMenu, bDvbUser, bDvbMenu, bHdUser, bHdMenu;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	bHohUser = bHohMenu = bDvbUser = bDvbMenu = bHdUser = bHdMenu = FALSE;

	bHohUser	= pstUserSettings->stSubtitle.bHoh;
	bDvbUser	= pstUserSettings->stSubtitle.bDvb;
	bHdUser		= pstUserSettings->stSubtitle.bHd;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING, (HUINT32 *)&bHohMenu, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING) Error!!\n");
		bHohMenu = FALSE;
	}

	// TODO: Get bDvbMenu, bHdMenu from DB_PARAM !!!!!!
	bDvbMenu	= TRUE;	// Default Value
	bHdMenu		= TRUE;	// Default Value

	pstSubtitleType->abHoh[eSvcSi_CFG_SUBTTL_HOH_User]	= bHohUser;
	pstSubtitleType->abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu]	= bHohMenu;
	pstSubtitleType->abDvb[eSvcSi_CFG_SUBTTL_DVB_User]	= bDvbUser;
	pstSubtitleType->abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu]	= bDvbMenu;
	pstSubtitleType->abHD[eSvcSi_CFG_SUBTTL_HD_User]	= bHdUser;
	pstSubtitleType->abHD[eSvcSi_CFG_SUBTTL_HD_Menu]	= bHdMenu;

	return ERR_OK;
}


STATIC HERROR mgr_GetSysConfiguration(SvcSi_ConfigClass_e bwClass, SvcSi_Config_t *pstConfig, SvcSi_UserSettings_t *pstUserSettings)
{
	HERROR						 hErr;
	HBOOL						 bSys, bAudOutput, bAudLang, bAudDesc;
	HBOOL						 bSubttl, bSubttlType, bSubttlLang;

	if (NULL == pstConfig)
		return ERR_FAIL;

	bSys = bAudOutput = bAudLang = bAudDesc = bSubttl = bSubttlType = bSubttlLang = FALSE;

	bSys = (bwClass & eSvcSi_CFG_SYS_ALL) ? TRUE : FALSE;
	bAudOutput = (bwClass & eSvcSi_CFG_AUD_OUTPUT) ? TRUE : FALSE;
	bAudLang = (bwClass & eSvcSi_CFG_AUD_LANG) ? TRUE : FALSE;
	bAudDesc = (bwClass & eSvcSi_CFG_AUD_DESC) ? TRUE : FALSE;
	bSubttl = bSubttlType = bSubttlLang = (bwClass & eSvcSi_CFG_SUBTTL_ALL) ? TRUE : FALSE;

	hErr = ERR_OK;

	// Get System Configuration
	if (bSys)
	{
		hErr = mgr_GetSystemConfig(&pstConfig->stSystem);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetSystemConfig() Error!!\n");
			return hErr;
		}
	}

	// Get Audio Output Configuration
	if (bAudOutput)
	{
		hErr = mgr_GetAudioOutputConfig(&pstConfig->stAudioOutput, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetAudioOutputConfig() Error!!\n");
			return hErr;
		}
	}

	// Get Audio Language Configuration
	if (bAudLang)
	{
		hErr = mgr_GetAudioLanguageConfig(&pstConfig->stAudioLanguage, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetAudioLanguageConfig() Error!!\n");
			return hErr;
		}
	}

	// Get Audio Description Configuration
	if (bAudDesc)
	{
		hErr = mgr_GetAudioDescConfig(&pstConfig->stAudioDesc, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetAudioDescConfig() Error!!\n");
			return hErr;
		}
	}

	if (bSubttlLang)
	{
		hErr = mgr_GetSubtitleLanguageConfig(&pstConfig->stSubtitleLangauge, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetSubtitleLanguageConfig() Error!!\n");
			return hErr;
		}
	}

	if (bSubttlType)
	{
		hErr = mgr_GetSubtitleTypeConfig(&pstConfig->stSubtitleType, pstUserSettings);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_GetSubtitleTypeConfig() Error!!\n");
			return hErr;
		}
	}

	return ERR_OK;
}


BUS_Result_t	PROC_Init(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr;
	UNIXTIME			utRtcTime = 0;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
		{
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "PROC_Init");
#if	defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
			MGR_STORAGE_IntHddSetPowerState(ePS_On);
#endif

			MGR_ACTION_Init();
			MGR_ACTION_BgJobInit();

			MGR_SCHEDULE_Init(); // time init 이 되기전 sapi로 notifier가 등록되어야, sama에서 time이 available 해 진 이후에 발생하는 event를 놓치지 않는다.

			VK_CLOCK_SetDstMode (VK_CLOCK_DST_OFF);
			hErr = SVC_SYS_GetRtcTime(&utRtcTime);
			if(hErr == ERR_OK)
			{
				/* System Time에 Setting한다. */

				if (TAPI_SetTime(TxSOURCE_SET_BY_MICOM, utRtcTime) != ERR_OK)
					HxLOG_Error("[TAPI_SetTime is failed [%u]\n", utRtcTime);
			}
			else
			{
				HxLOG_Warning("\n\txxxxxxxx Error getting MICOM time xxxxxxxx\n");
			}

			hErr = SVC_SI_SetCallback (mgr_GetSysConfiguration);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\n\txxxxxxxx Error SVC_SI_SetCallback xxxxxxxx\n");
			}

			{
				SvcSi_EsPriorityTable_t 	 stTable;

				HxSTD_MemSet(&stTable, 0x00, sizeof(SvcSi_EsPriorityTable_t));
				hErr = MGR_MODEL_GetSiEsPriorityTable(&stTable);
				if (hErr == ERR_OK)
				{
					hErr = SVC_SI_SetAudioPriorityTable (stTable.astAudio, stTable.ulItemNumAudio);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("\n\txxxxxxxx Error SVC_SI_SetAudioPriorityTable() xxxxxxxx\n");
					}

					hErr = SVC_SI_SetSubtitlePriorityTable (stTable.astSubtitle, stTable.ulItemNumSubtitle);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("\n\txxxxxxxx Error SVC_SI_SetSubtitlePriorityTable() xxxxxxxx\n");
					}
				}
				else
				{
					HxLOG_Error("\n\txxxxxxxx Error MGR_MODEL_GetSiEsPriorityTable() xxxxxxxx\n");
				}
			}

			MGR_TIME_Init();
			MGR_OUTPUT_Init();
#if defined(CONFIG_MW_CAS)
			MGR_CAS_Init();
#endif
			MGR_SPEC_Init();
			// Product 별 Init가 필요
			MGR_INIT_Model();

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
			MGR_CAS_Start ();
#endif

#if defined(CONFIG_MW_MULTIMEDIA) && defined(CONFIG_MW_MM_PVR)

			MGR_RECORDER_Init();
			MGR_PLAYBACK_Init();

			MGR_PVR_Init();
#endif
			MGR_LIVE_Init();

#if defined(CONFIG_MW_MM_PVR) && defined(CONFIG_MW_MULTIMEDIA) && defined(CONFIG_STREAMING_SATIP)
			MGR_SATIP_Init ();
#endif

#if defined(CONFIG_HOMMA_RC)
			MGR_RC_Init();
			MGR_RC_Start();
#endif
#if defined(CONFIG_MW_SI_AIT)
			MGR_HBBTV_Init();
#endif
			MGR_ACTION_Start();

			OM_Init();

#if defined(CONFIG_MW_HDD_FORMAT_JIGSW)
			MGR_HddFormatJIG_Start();
#endif

			
			//Init completed.
			BUS_PostMessage(NULL, eMEVT_ACTION_NOTIFY_INIT_COMPLETE, HANDLE_NULL, 0, 0, 0);
			BUS_MGR_Destroy(0);
			break;
		}
		case eMEVT_BUS_DESTROY:
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAct, p1, p2, p3);
}

/* End of File. ---------------------------------------------------------- */


