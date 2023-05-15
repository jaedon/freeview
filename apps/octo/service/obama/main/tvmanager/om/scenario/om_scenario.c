/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_scenario.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <hapi.h>
#include <obama_rpcprotocol.h>

#include <oapi.h>

#include <octo_common.h>

#include <db_param.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_pg.h>
#include <mgr_time.h>

#include <svc_si.h>
#include <svc_sys.h>
#include <svc_output.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef HERROR	(*om_scenario_doCB_t)(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[]);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC void			*s_pstScenarioObject;
STATIC HxQueue_t		*s_pstScenarioJob;
STATIC HxHASH_t		*s_pstScenarioFuncTable;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
//	argv[0] : s
STATIC HERROR	om_scenario_SetMainLanguage(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e	eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32)eId, 0);
	SVC_SYS_SetLanguage(HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

//	argv[0] : i
//	argv[1] : s
STATIC HERROR	om_scenario_SetAudioLanguage(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HINT32			nIndex;
	HxLANG_Id_e		eId;
	STATIC const DbParam_Item_e s_aeDbItem[] = {
		eDB_PARAM_ITEM_LANG_AUDIO0,
		eDB_PARAM_ITEM_LANG_AUDIO1,
		eDB_PARAM_ITEM_LANG_AUDIO2,
		eDB_PARAM_ITEM_LANG_AUDIO3,
		eDB_PARAM_ITEM_LANG_AUDIO4,
		eDB_PARAM_ITEM_LANG_AUDIO5,
		eDB_PARAM_ITEM_LANG_AUDIO6,
		eDB_PARAM_ITEM_LANG_AUDIO7,
	};

	nIndex = HxOBJECT_INT(apArgv[0]);

	if (0 > nIndex || sizeof(s_aeDbItem)/sizeof(DbParam_Item_e) <= nIndex)
		nIndex = 0;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[1]));

	DB_PARAM_SetItem(s_aeDbItem[nIndex], (HUINT32)eId, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetAudioLanguageUndefined(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED, eId, 0);

	return ERR_OK;
}

//	argv[0] : s
STATIC HERROR	om_scenario_SetAudioLanguageOriginal(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL, eId, 0);

	return ERR_OK;
}

//	argv[0] : s
STATIC HERROR	om_scenario_SetAudioLanguageDefault(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT, eId, 0);

	return ERR_OK;
}

//	argv[0] : i
//	argv[1] : s
STATIC HERROR	om_scenario_SetSubtitleLanguage(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HINT32			nIndex;
	HxLANG_Id_e		eId;
	STATIC const DbParam_Item_e s_aeDbItem[] = {
		eDB_PARAM_ITEM_LANG_SUBTITLE0,
		eDB_PARAM_ITEM_LANG_SUBTITLE1,
		eDB_PARAM_ITEM_LANG_SUBTITLE2,
		eDB_PARAM_ITEM_LANG_SUBTITLE3,
		eDB_PARAM_ITEM_LANG_SUBTITLE4,
		eDB_PARAM_ITEM_LANG_SUBTITLE5,
		eDB_PARAM_ITEM_LANG_SUBTITLE6,
		eDB_PARAM_ITEM_LANG_SUBTITLE7,
	};

	nIndex = HxOBJECT_INT(apArgv[0]);
	if (0 > nIndex || sizeof(s_aeDbItem)/sizeof(DbParam_Item_e) <= nIndex)
		nIndex = 0;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[1]));

	DB_PARAM_SetItem(s_aeDbItem[nIndex], (HUINT32)eId, 0);

	return ERR_OK;
}

//	argv[0] : s
STATIC HERROR	om_scenario_SetSubtitleLanguageUndefined(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED, eId, 0);

	return ERR_OK;
}

//	argv[0] : s
STATIC HERROR	om_scenario_SetSubtitleLanguageOriginal(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL, eId, 0);

	return ERR_OK;
}

//	argv[0] : s
STATIC HERROR	om_scenario_SetSubtitleLanguageDefault(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLANG_Id_e		eId;

	eId = HLIB_LANG_639ToId(HxOBJECT_STR(apArgv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT, eId, 0);

	return ERR_OK;
}

//	argv[0] : i
STATIC HERROR	om_scenario_SetHardOfHearing(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_HARDOFHEARING, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

//	argv[0] : i
STATIC HERROR	om_scenario_SetMaturityRating(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);
	BUS_PostMessage(NULL, eOEVT_PG_CHECK_RATING, (int)NULL, 0, 0, 0);

	return ERR_OK;
}

//	argv[0] : i (HxCountry_e)
STATIC HERROR	om_scenario_SetCountryCode(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxCountry_e	eCountryId;

	eCountryId = HxOBJECT_INT(apArgv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32)eCountryId, 0);

	return ERR_OK;
}

//	argv[0] : i
STATIC HERROR	om_scenario_SetStartPadding(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_PADDINGTIME_START, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	return ERR_OK;
}

//	argv[0] : i
STATIC HERROR	om_scenario_SetEndPadding(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	return ERR_OK;
}

//	argv[0] : i
STATIC HERROR	om_scenario_SetAutoDelete(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_AUTO_DELETE, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	return ERR_OK;
}

//	argv[0] : i (DxAntennaConnection_e)
STATIC HERROR	om_scenario_SetAntennaConnection(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DxAntennaConnection_e	eAntennaConnection;

	eAntennaConnection = (DxAntennaConnection_e)HxOBJECT_INT(apArgv[0]);

	switch (eAntennaConnection)
	{
		case eDxANTCONN_TWOSAME:
			SVC_PIPE_SetRfInputAbility(0, TRUE);
			SVC_PIPE_SetRfInputAbility(1, TRUE);
			break;
		case eDxANTCONN_ONECABLE:
		default:
			SVC_PIPE_SetRfInputAbility(0, TRUE);
			SVC_PIPE_SetRfInputAbility(1, FALSE);
			break;
	}

	DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eAntennaConnection, 0);

	return ERR_OK;
}

//	apArgv[0] : i
STATIC HERROR	om_scenario_SetGMTOffset(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HINT32	nGmtOffset ;

	nGmtOffset = HxOBJECT_INT(apArgv[0]);

	return MGR_TIME_SetGmtOffset(nGmtOffset, TRUE);
}

//	apArgc[0] : i
STATIC HERROR	om_scenario_SetChannelSetupPIN(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_CHANNEL_SETUP_PIN, (HUINT32)HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE, 0);

	return ERR_OK;
}

//	apArgc[0] : DxGuidancePolicy_e
STATIC HERROR	om_scenario_SetGuidancePolicy(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DxGuidancePolicy_e	eGPolicy;

	eGPolicy = HxOBJECT_INT(apArgv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_GUIDANCE_POLICY, (HUINT32)eGPolicy, 0);

	return ERR_OK;
}

//	apArgc[0] : i
STATIC HERROR	om_scenario_SetSubtitleSetting(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HBOOL	bSubtitleEnable ;

	bSubtitleEnable = (HBOOL) ((HUINT32)HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE) ;

	DB_PARAM_SetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32)bSubtitleEnable, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetDayLightSaving(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_DAYLIGHT_SAVING, (HUINT32)HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetTimeUpdate(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP, (HUINT32)HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetOperatorType(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DxOperator_e eOpType;

	eOpType = HxOBJECT_INT(apArgv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32)eOpType, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetPostcode(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_POSTCODE, (HUINT32)HxOBJECT_STR(apArgv[0]), 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetSIMode(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DxSIMode_e	eSiMode;

	eSiMode = HxOBJECT_INT(apArgv[0]);

	HAPI_SetRegistryEnum(pszKey, "DxSIMode_e", eSiMode);
	if (eSiMode != eDxSIMODE_UNKNOWN)
		SVC_SI_SetSiMode(eSiMode);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetAdultChannel(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DxAdultChannelType_e	eAdultChannel;

	eAdultChannel = HxOBJECT_INT(apArgv[0]);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_ADULTCHANNELS, (HUINT32)eAdultChannel, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetSatRecReceiverID(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HUINT32		ulReceiverId;

	ulReceiverId = HxOBJECT_INT(apArgv[0]);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_RECEIVERID, (HUINT32)ulReceiverId, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetSatRecUpdateInterval(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HINT32	nUpdateInterval, nTime;

	nUpdateInterval = HxOBJECT_INT(apArgv[0]);

	if(nUpdateInterval == eDxSATREC_INTERVAL_MANUAL)
	{
		nTime = HxOBJECT_INT(apArgv[1]);
		nUpdateInterval |= (nTime * 60 *60);
	}

	DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL, (HUINT32)nUpdateInterval, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetTVTVEnable(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_TVTV_ENABLE, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetTVTVEPGDownload_StandbyEnable(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_TVTV_DOWNTYPE, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetFavChListNumberingMode(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_FAV_CHNUM_ORDER, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetDescrambleOnOff(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_DESCRAMBLEONOFF, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetBatUpdateOnOff(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLOG_Trace();
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BATUPDATEONOFF, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetNitLcnUpdateOnOff(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLOG_Trace();
	DB_PARAM_SetItem(eDB_PARAM_ITEM_NITLCNUPDATEONOFF, HxOBJECT_INT(apArgv[0]) ? (HUINT32)TRUE : (HUINT32)FALSE, 0);

	return ERR_OK;
}


STATIC HERROR	om_scenario_SetLiveStreamPriority(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, HxOBJECT_INT(apArgv[0]), 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetAutomatic_Powerdown(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN, HxOBJECT_INT(apArgv[0]), 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_SetPriority_HDMIOutput(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HBOOL Enable;
	Enable =  HxOBJECT_INT(apArgv[0]);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT, Enable, 0);
	HxLOG_Debug("Enable[%d]\n",Enable);
	SVC_OUT_SetHighPriorityHD(Enable);
	return ERR_OK;
}

STATIC HERROR	om_scenario_SetSwUpdateVersion(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)HxOBJECT_STR(apArgv[0]), 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_NotSupportedSet(const HCHAR *pszKey, HINT32 nArgc, const HxObject_t *apArgv[])
{
	HxLOG_Warning("the value should be changed by internal!!!\n");
	return ERR_OK;
}

//	apArgv[0] : pszKey
//	apArgv[1] : binary
STATIC HERROR om_scenario_Set(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32		nParam;
	HERROR		err = ERR_FAIL;
	HxObject_t	*pstObject;
	HxObject_t	**ppArgv;

	const HCHAR		*pszKey;
	om_scenario_doCB_t	pfnCallback;

	HxLOG_Assert(nArgc == 2);

	pszKey = (const HCHAR*)HxOBJECT_STR(apArgv[0]);
	pstObject = HLIB_SERIALIZER_UnpackWithCache(s_pstScenarioObject, HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1]), FALSE);
	if (pstObject == NULL)
	{
		HxLOG_Error("object is not valid!\n");
		return ERR_FAIL;
	}

	nParam = HxOBJECT_ARR_LEN(pstObject);
	ppArgv = pstObject->u.array.values;

	pfnCallback = (om_scenario_doCB_t)HLIB_HASH_Lookup(s_pstScenarioFuncTable, (void*)pszKey);
	if (pfnCallback)
		err = pfnCallback(pszKey, nParam, (void*)ppArgv);

	HLIB_SERIALIZER_FreeObjectWithCache(s_pstScenarioObject, pstObject);

	return err;
}

HERROR	OM_SCENARIO_Init(void)
{
	HERROR	err;
	HINT32	nRpcHandle = 0;
	s_pstScenarioObject    = HLIB_SERIALIZER_OpenUnpackCache();
	s_pstScenarioJob       = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HLIB_STD_MemFree_CB);
	s_pstScenarioFuncTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, HLIB_STD_MemFree_CB);

	//	add function table for each scenario set.
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_MAINLANGUAGE,     (void*)om_scenario_SetMainLanguage);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUDIOLANGUAGE,    (void*)om_scenario_SetAudioLanguage);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED,	(void*)om_scenario_SetAudioLanguageUndefined);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL,	(void*)om_scenario_SetAudioLanguageOriginal);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUDIOLANGUAGE_DEFAULT,		(void*)om_scenario_SetAudioLanguageDefault);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SUBTITLELANGUAGE, (void*)om_scenario_SetSubtitleLanguage);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED,	(void*)om_scenario_SetSubtitleLanguageUndefined);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL,	(void*)om_scenario_SetSubtitleLanguageOriginal);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT,		(void*)om_scenario_SetSubtitleLanguageDefault);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_HARDOFHEARING,    	(void*)om_scenario_SetHardOfHearing);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_MATURITYRATING,   	(void*)om_scenario_SetMaturityRating);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_COUNTRYCODE,      	(void*)om_scenario_SetCountryCode);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_OPERATORTYPE,     	(void*)om_scenario_SetOperatorType);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_STARTPADDING,     	(void*)om_scenario_SetStartPadding);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_ENDPADDING,       	(void*)om_scenario_SetEndPadding);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUTODELETE,       	(void*)om_scenario_SetAutoDelete);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_ANTENNACONNECTION,	(void*)om_scenario_SetAntennaConnection);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_GMTOFFSET,        	(void*)om_scenario_SetGMTOffset);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_CHANNELSETUPPIN,  	(void*)om_scenario_SetChannelSetupPIN);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_GUIDANCEPOLICY,   	(void*)om_scenario_SetGuidancePolicy);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SUBTITLESETTING,  	(void*)om_scenario_SetSubtitleSetting);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_DAYLIGHTSAVING,   	(void*)om_scenario_SetDayLightSaving);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_TIMEUPDATE,       	(void*)om_scenario_SetTimeUpdate);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_POSTCODE,         	(void*)om_scenario_SetPostcode);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SIMODE,           	(void*)om_scenario_SetSIMode);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_ADULTCHANNEL,     	(void*)om_scenario_SetAdultChannel);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SATREC_RECEIVERID,	(void*)om_scenario_SetSatRecReceiverID);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SATREC_INTERVAL,		(void*)om_scenario_SetSatRecUpdateInterval);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_TVTV_ENABLE,			(void*)om_scenario_SetTVTVEnable);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_TVTV_DOWNTYPE,		(void*)om_scenario_SetTVTVEPGDownload_StandbyEnable);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_FAVCHLISTNUMBERING,	(void*)om_scenario_SetFavChListNumberingMode);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_DESCRAMBLEONOFF,		(void*)om_scenario_SetDescrambleOnOff);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_LIVESTREAM_PRIORITY,	(void*)om_scenario_SetLiveStreamPriority);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_AUTOMATIC_POWERDOWN,	(void*)om_scenario_SetAutomatic_Powerdown);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_PRIORITY_HDMIOUTPUT,	(void*)om_scenario_SetPriority_HDMIOutput);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_SWUPDATEVERSION,		(void*)om_scenario_SetSwUpdateVersion);

	//	data is generated internally.
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_REGIONCODE,       (void*)om_scenario_NotSupportedSet);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_BOUQUETNAME,      (void*)om_scenario_NotSupportedSet);
	HLIB_HASH_Insert(s_pstScenarioFuncTable, (void*)OAPI_SCENARIO_BOUQUETID,        (void*)om_scenario_NotSupportedSet);
	err = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{
		err = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SCENARIO_SET, "sb", (HxRPC_Func_t)om_scenario_Set, NULL,
								"Set Scenario (const HCHAR *pszKey, ...)");
	}


	return err;
}
#if 0
#define	OAPI_SCENARIO_MAINLANGUAGE					"mainlang"
#define	OAPI_SCENARIO_AUDIOLANGUAGE					"audiolang"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE				"subtitlelang"
#define	OAPI_SCENARIO_SUBTITLESETTING				"subtitlesetting"
#define	OAPI_SCENARIO_HARDOFHEARING					"hardofhearing"
#define OAPI_SCENARIO_MATURITYRATING				"maturiyrating"
#define	OAPI_SCENARIO_COUNTRYCODE					"countrycode"
#define	OAPI_SCENARIO_OPERATORTYPE					"operatortype"
#define	OAPI_SCENARIO_ANTENNACONNECTION				"antennaconnection"
#define	OAPI_SCENARIO_STARTPADDING					"startpadding"
#define	OAPI_SCENARIO_ENDPADDING					"endpadding"
#define	OAPI_SCENARIO_AUTODELETE					"autodelete"
#define	OAPI_SCENARIO_POSTCODE						"postcode"
#define	OAPI_SCENARIO_GMTOFFSET						"gmtoffset"
#define	OAPI_SCENARIO_CHANNELSETUPPIN				"channelsetuppin"
#define	OAPI_SCENARIO_GUIDANCEPOLICY				"guidancepolicy"
#define	OAPI_SCENARIO_DAYLIGHTSAVING				"daylightsaving"
#define	OAPI_SCENARIO_TIMEOFFSET					"timeoffset"
#define	OAPI_SCENARIO_TIMEUPDATE					"timeupdate"
#define	OAPI_SCENARIO_SWUPDATEVERSION				"swupdateversion"
#define	OAPI_SCENARIO_BOUQUETNAME					"bouquetname"
#define	OAPI_SCENARIO_SIMODE						"simode"
#define	OAPI_SCENARIO_BOUQUETID						"bouquetid"
#define	OAPI_SCENARIO_REGIONCODE					"regioncode"
#define	OAPI_SCENARIO_ADULTCHANNEL					"adultchannel"
#endif
