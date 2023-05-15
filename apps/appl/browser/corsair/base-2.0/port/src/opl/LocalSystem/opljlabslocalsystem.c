/**************************************************************************************/
/**
 * @file opljlabslocalsystem.c
 *
 * System information
 *
 * @author  Sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/06/19
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "opljlabslocalsystem.h"
#include "oplchannel.h"

#include "oapi.h"
#include "hapi.h"
#include <apk.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>


/******************************************************************
	Define
******************************************************************/
#define	CHECK_BPL_PARAM_PTR(a)		{if (a == NULL) {HxLOG_Error("Input argument is NULL\n"); return BPL_STATUS_ERROR;}}

/******************************************************************
	Static variables
******************************************************************/

/*static	APKS_Pair_t _astSpdifModePairList[] =
{
	{	eSdifMode_Auto,		"AUTO"							},
	{	eSdifMode_PCM,		"PCM"						},
};*/
/******************************************************************
	Static functions
******************************************************************/
static HCHAR* OPL_JabsLocalSystem_GetLastChannelFieldByNetwork(HUINT32 netType)
{
	switch(netType)
	{
		case eOPL_DTT_NETWORK:
			return USERDB_JLABS_DTT_LAST_CHANNEL;
		case eOPL_BS_NETWORK:
			return USERDB_JLABS_BS_LAST_CHANNEL;
		case eOPL_CATV_NETWORK:
			return USERDB_JLABS_CATV_LAST_CHANNEL;
		case eOPL_CS1_NETWORK:
			return USERDB_JLABS_CS1_LAST_CHANNEL;
		case eOPL_CS2_NETWORK:
			return USERDB_JLABS_CS2_LAST_CHANNEL;
		case eOPL_JCHITS_NETWORK:
			return USERDB_JLABS_JCHITS_LAST_CHANNEL;
		default:
			return NULL;

	}
	return NULL;
}


#define	___MiscSetting_Related_Functions____


BPLStatus OPL_JlabsLocalSystem_GetLocalArea(HCHAR *pszLocalArea, HUINT32 ulLen)
{
	APKE_JLABS_LocalArea_t	eAreaCode = eHokaidoSaporo;
	CHECK_BPL_PARAM_PTR(pszLocalArea);
	HxSTD_MemSet(pszLocalArea, 0, sizeof(HCHAR)*ulLen);
	APK_SQCSCENARIOJAPAN_GetLocalArea(&eAreaCode);
	HxSTD_snprintf(pszLocalArea, ulLen, "%d",eAreaCode);
	pszLocalArea[ulLen -1] ='\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetAutoSWDownloadEnable( HBOOL *pbAutoDownEnable)
{
	HBOOL bAutoSwDown = FALSE;
	CHECK_BPL_PARAM_PTR(pbAutoDownEnable);
	APK_SQCSCENARIOJAPAN_GetAutoSWDownloadEnable(&bAutoSwDown);
	*pbAutoDownEnable =bAutoSwDown;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetPostcode( HCHAR *pszPostcode, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszPostcode);
	HxSTD_MemSet(pszPostcode, 0, sizeof(HCHAR)*ulLen);
	APK_SQCSCENARIOJAPAN_GetPostcode(pszPostcode, ulLen);
	pszPostcode[ulLen -1] ='\0';
	return BPL_STATUS_OK;
}


BPLStatus OPL_JlabsLocalSystem_GetChannelMaskEnable(HBOOL *pbChMaskEnable)
{
	HBOOL bChMasking = FALSE;
	CHECK_BPL_PARAM_PTR(pbChMaskEnable);
	APK_SQCSCENARIOJAPAN_GetChannelMaskEnable(&bChMasking);
	*pbChMaskEnable =bChMasking;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetSubtitleMode(HINT32 *pSubtitleMode)
{
	APKE_JLABS_SubtitleMode_t eSbtlMode = eSubtitleMode_NotUse;
	CHECK_BPL_PARAM_PTR(pSubtitleMode);
	if(APK_SQCSCENARIOJAPAN_GetSbtlMode(&eSbtlMode)!=ERR_OK)
	{
		HxLOG_Error("Getting the SubtitleMode is failed..\n");
	}
	*pSubtitleMode = eSbtlMode;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetSuperImposeMode(HINT32 *pSuperImposeMode)
{
	APKE_JLABS_SuperImposeMode_t eSuperMode = eSuperImposeMode_NotUse;
	CHECK_BPL_PARAM_PTR(pSuperImposeMode);
	if(APK_SQCSCENARIOJAPAN_GetSuperimposeMode(&eSuperMode)!=ERR_OK)
	{
		HxLOG_Error("Getting the SuperimposeMode is failed..\n");
	}
	*pSuperImposeMode = eSuperMode;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetLastChCCID(HCHAR *pszLastChCCID, HUINT32 ulLen)
{
	HERROR		 hErr;
	HCHAR		szCcid[OPL_JLABS_MAX_UDB_LEN];
	CHECK_BPL_PARAM_PTR(pszLastChCCID);
	HxSTD_MemSet(szCcid, 0, OPL_JLABS_MAX_UDB_LEN);
	HxSTD_MemSet (pszLastChCCID, 0, ulLen);
	hErr = APK_DB_GetStr((HUINT8 *)USERDB_SVC_LAST_CHANNEL, szCcid, OPL_JLABS_MAX_UDB_LEN);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	HxSTD_StrNCpy(pszLastChCCID, szCcid, ulLen);
	pszLastChCCID[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetSeamlessEnable( HBOOL *pbSeamlessEnabled)
{
	HBOOL bSeamless = FALSE;
	CHECK_BPL_PARAM_PTR(pbSeamlessEnabled);
	APK_SQCSCENARIOJAPAN_GetSeamlessEnable(&bSeamless);
	*pbSeamlessEnabled =bSeamless;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetSetupConfigureLaunchMode(HUINT32 *pLaunchMode)
{
	CHECK_BPL_PARAM_PTR(pLaunchMode);
	APK_SQCSCENARIOJAPAN_GetSetupLaunchMode(pLaunchMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetAutoPPVPurchaseEnable(HBOOL *pbEnable)
{
	HBOOL bAutoPurchasePpv = FALSE;
	CHECK_BPL_PARAM_PTR(pbEnable);
	APK_SQCSCENARIOJAPAN_GetAutoPPVPurchaseEnable(&bAutoPurchasePpv);
	*pbEnable =bAutoPurchasePpv;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetDisplayMode3D(HINT32 *pDisplayMode3D)
{
	APKE_JLABS_DisplayMode_3D_t eDisplayMode3D = eDisplayMode3D_Only3DContent;
	CHECK_BPL_PARAM_PTR(pDisplayMode3D);
	APK_SQCSCENARIOJAPAN_GetDisplayMode3D(&eDisplayMode3D);
	*pDisplayMode3D = (HINT32)eDisplayMode3D;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetThreeDMode(HBOOL *pbMode)
{
	HBOOL bThreeDMode = FALSE;
	CHECK_BPL_PARAM_PTR(pbMode);
	APK_SQCSCENARIOJAPAN_GetThreeDMode(&bThreeDMode);
	*pbMode = bThreeDMode;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetLimitPerPPV( HCHAR *pszLimitPrice, HUINT32 ulLen)
{
	HERROR		 hErr;
	HUINT32		ulLimitPerPpv = 0;
	CHECK_BPL_PARAM_PTR(pszLimitPrice);
	HxSTD_MemSet(pszLimitPrice, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetLimitPerPPV(&ulLimitPerPpv);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	if(ulLimitPerPpv == 0)
	{
		HxSTD_StrNCpy(pszLimitPrice, OPL_JLABS_LOCALSYSTEM_UNLIMITED_PRICE_STR, ulLen);
	}
	else
	{
		HxSTD_snprintf(pszLimitPrice, ulLen, "%d", ulLimitPerPpv);
	}
	pszLimitPrice[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetLimitPerPPM( HCHAR *pszLimitPrice, HUINT32 ulLen)
{
	HERROR		 hErr;
	HUINT32		ulLimitPerPpm = 0;
	CHECK_BPL_PARAM_PTR(pszLimitPrice);
	HxSTD_MemSet(pszLimitPrice, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetLimitPerPPM(&ulLimitPerPpm);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	if(ulLimitPerPpm == 0)
	{
		HxSTD_StrNCpy(pszLimitPrice, OPL_JLABS_LOCALSYSTEM_UNLIMITED_PRICE_STR, ulLen);
	}
	else
	{
		HxSTD_snprintf(pszLimitPrice, ulLen, "%d", ulLimitPerPpm);
	}
	pszLimitPrice[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetFriendlyName( HCHAR *pszFriendlyName, HUINT32 ulLen)
{
	HERROR		 hErr;

	//hErr = APK_SQCDMS_GetFriendName(pszFriendlyName);
	hErr = APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, pszFriendlyName, ulLen );
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszFriendlyName[ulLen - 1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetNaviURL( HCHAR *pszUrl, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszUrl);
	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetNaviURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszUrl[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetInteracTVURL( HCHAR *pszUrl, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszUrl);
	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetInteracTVURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszUrl[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetInternetBrowserURL( HCHAR *pszUrl, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszUrl);
	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetInternetBrowserURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszUrl[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetVodHomeURL( HCHAR *pszUrl, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszUrl);
	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetVodHomeURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszUrl[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetACSURL( HCHAR *pszUrl, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszUrl);
	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetAcsURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszUrl[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetTuningRange(HINT32 *pTuninRange)
{
	CHECK_BPL_PARAM_PTR(pTuninRange);
	APK_DB_GetInt(USERDB_JLABS_TUNING_RANGE, pTuninRange);

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetTimebarDispMode(HINT32 *pTimebarDispMode)
{
	CHECK_BPL_PARAM_PTR(pTimebarDispMode);
	APK_DB_GetInt(USERDB_JLABS_TIMEBAR_DISP_MODE, pTimebarDispMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetPinUnlockExpireOption( HINT32 *pOption)
{
	CHECK_BPL_PARAM_PTR(pOption);
	APK_SQCSCENARIOJAPAN_GetPinUnlockExpireOption((APKE_JLABS_PinUnlockExpirationOption_t*)pOption);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetAMPermissionLevel( HINT32 *pLevel)
{
	CHECK_BPL_PARAM_PTR(pLevel);
	APK_SQCSCENARIOJAPAN_GetAmPermissionLevel((APKE_JLABS_AMPermissionLevel_t*)pLevel);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetBmlInfoDispEnable( HBOOL *pbBmlInfoDispEnable)
{
	HBOOL bBmlInfoDispEnable = FALSE;
	CHECK_BPL_PARAM_PTR(pbBmlInfoDispEnable);
	APK_SQCSCENARIOJAPAN_GetBmlInfoDispEnable(&bBmlInfoDispEnable);
	*pbBmlInfoDispEnable =bBmlInfoDispEnable;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetIplateInfoDispEnable( HBOOL *pbIplateInfoDispEnable)
{
	CHECK_BPL_PARAM_PTR(pbIplateInfoDispEnable);
	APK_DB_GetInt(USERDB_JLABS_I_PLATE_DISP_ENABLE, (HINT32*)pbIplateInfoDispEnable);

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetFrontDispEnable( HBOOL *pbFrontDispEnable)
{
	HBOOL bFrontDispEnable = FALSE;
	CHECK_BPL_PARAM_PTR(pbFrontDispEnable);
	APK_SQCSCENARIOJAPAN_GetFrontDispEnable(&bFrontDispEnable);
	*pbFrontDispEnable =bFrontDispEnable;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetIsDMSPublic( HBOOL *pbIsDMSPublic)
{
	HBOOL bIsDMSPublic = FALSE;
	CHECK_BPL_PARAM_PTR(pbIsDMSPublic);
	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32*)pbIsDMSPublic);

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetIsDMSDeliver( HBOOL *pbIsDMSDeliver)
{
	HERROR	hErr;
	HBOOL 	bIsDMSDeliver = FALSE;
	CHECK_BPL_PARAM_PTR(pbIsDMSDeliver);
 	hErr = APK_SQCSCENARIOJAPAN_GetIsDMSDeliver(&bIsDMSDeliver);
	*pbIsDMSDeliver = bIsDMSDeliver;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetIsIPDubbing( HBOOL *pbIsIPDubbing)
{
	HERROR	hErr;
	HBOOL 	bIsIPDubbing = FALSE;

	hErr = APK_DLNADMP_GetIsIPDubbing(&bIsIPDubbing);
	if (ERR_OK != hErr)
	{
		*pbIsIPDubbing = FALSE;
		return BPL_STATUS_OK;
	}
	*pbIsIPDubbing = bIsIPDubbing;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetLastChCCIDByNetwork(HUINT32 netType, HCHAR *pszLastChCCID, HUINT32 ulLen)
{
	HERROR		 hErr;
	HCHAR		szCcid[OPL_JLABS_MAX_UDB_LEN];
	HCHAR		*pcFieldName = NULL;
	CHECK_BPL_PARAM_PTR(pszLastChCCID);
	HxSTD_MemSet(szCcid, 0, OPL_JLABS_MAX_UDB_LEN);
	HxSTD_MemSet (pszLastChCCID, 0, ulLen);
	pcFieldName = OPL_JabsLocalSystem_GetLastChannelFieldByNetwork(netType);
	if(pcFieldName == NULL)
	{
		return BPL_STATUS_ERROR;
	}
	hErr = APK_DB_GetStr((HUINT8 *)pcFieldName, szCcid, OPL_JLABS_MAX_UDB_LEN);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	HxSTD_StrNCpy(pszLastChCCID, szCcid, ulLen);
	pszLastChCCID[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetChannelMaskPattern(HUINT32 *pChannelMaskPattern)
{
	CHECK_BPL_PARAM_PTR(pChannelMaskPattern);
	if(APK_SQCSCENARIOJAPAN_GetChannelMaskPattern(pChannelMaskPattern) != ERR_OK)
	{
		HxLOG_Error("Getting the ChannelMaskPattern is failed..\n");
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetStandbyMode(HINT32 *pnMode)
{
	APKE_JLABS_StandbyMode_t eStandbyMode = eStandbyMode_None;
	CHECK_BPL_PARAM_PTR(pnMode);
	if(APK_SQCSCENARIOJAPAN_GetStandbyMode(&eStandbyMode) != ERR_OK)
	{
		HxLOG_Error("Getting the standbyMode is failed..\n");
		eStandbyMode = eStandbyMode_PowerOFF;
	}
	switch(eStandbyMode)
	{
		case eStandbyMode_PowerON:
			*pnMode = OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_ON;
			break;
		case eStandbyMode_PowerOFF:
			*pnMode = OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_OFF;
			break;
		default:
			HxLOG_Error("[%s:%d] getStandbyMode( val : %d ) ret Error!!!\n",__FUNCTION__,__LINE__, eStandbyMode);
			*pnMode = OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_OFF;
			break;
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_JlabsLocalSystem_GetSoftwareDownloadState(HUINT32 *pulSWDownloadState)
{
	CHECK_BPL_PARAM_PTR(pulSWDownloadState);
	if(APK_SQCSCENARIOJAPAN_GetSoftwareDownloadState(pulSWDownloadState) != ERR_OK)
	{
		HxLOG_Error("Getting the SWDownloadState is failed..\n");
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_JlabsLocalSystem_GetDefaultFactoryForDap(HUINT32 *pnMode)
{
	HUINT32 nMode = 0;
	CHECK_BPL_PARAM_PTR(pnMode);

	if (APK_SQCSCENARIOJAPAN_GetDefaultFactoryForDap(&nMode) != ERR_OK)
	{
		HxLOG_Error("Getting the DefaultFactoryForDap is failed..\n");
	}
	*pnMode = nMode;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetDefaultDiscForDap(HCHAR *pszDiscId, HUINT32 ulLen)
{
	HERROR		 hErr;
	CHECK_BPL_PARAM_PTR(pszDiscId);
	HxSTD_MemSet(pszDiscId, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetDefaultDiscForDap(pszDiscId, ulLen);
	if (ERR_OK != hErr)
	{
		return BPL_STATUS_ERROR;
	}
	pszDiscId[ulLen -1] = '\0';
	return BPL_STATUS_OK;
}


BPLStatus OPL_JlabsLocalSystem_SetLocalArea( HCHAR *pszLocalArea)
{
	APKE_JLABS_LocalArea_t	eAreaCode = eHokaidoSaporo;
	CHECK_BPL_PARAM_PTR(pszLocalArea);
	eAreaCode = HLIB_STD_StrToUINT32(pszLocalArea);
	if(APK_SQCSCENARIOJAPAN_SetLocalArea(eAreaCode)!= ERR_OK)
	{
		HxLOG_Error("Setting the LocalArea is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetAutoSWDownloadEnable( HBOOL bAutoDownEnable)
{
	APK_SQCSCENARIOJAPAN_SetAutoSWDownloadEnable(bAutoDownEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetPostcode( HCHAR *pszPostcode)
{
	CHECK_BPL_PARAM_PTR(pszPostcode);
	if(APK_SQCSCENARIOJAPAN_SetPostcode(pszPostcode) != ERR_OK)
	{
		HxLOG_Error("Setting the  postcode is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetChannelMaskEnable(HBOOL bChMaskEnable)
{
	APK_SQCSCENARIOJAPAN_SetChannelMaskEnable(bChMaskEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetSubtitleMode(HINT32 subtitleMode)
{
	if(APK_SQCSCENARIOJAPAN_SetSbtlMode(subtitleMode) != ERR_OK)
	{
		HxLOG_Error("Setting the  SbtlMode is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetSuperImposeMode(HINT32 superImposeMode)
{
	if(APK_SQCSCENARIOJAPAN_SetSuperimposeMode(superImposeMode) != ERR_OK)
	{
		HxLOG_Error("Setting the  superImposeMode is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetSeamlessEnable( HBOOL bSeamlessEnabled)
{
	APK_SQCSCENARIOJAPAN_SetSeamlessEnable(bSeamlessEnabled);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetSetupConfigureLaunchMode(HINT32 launchMode)
{
	APK_SQCSCENARIOJAPAN_SetSetupLaunchMode(launchMode);
	return BPL_STATUS_OK;
}
BPLStatus OPL_JlabsLocalSystem_SetAutoPPVPurchaseEnable(HBOOL bEnable)
{
	APK_SQCSCENARIOJAPAN_SetAutoPPVPurchaseEnable(bEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetDisplayMode3D(HINT32 displayMode3D)
{
	APKE_JLABS_DisplayMode_3D_t eDisplayMode3D = (APKE_JLABS_DisplayMode_3D_t)displayMode3D;
	APK_SQCSCENARIOJAPAN_SetDisplayMode3D(eDisplayMode3D);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetThreeDMode(HBOOL bThreeDMode)
{
	APK_SQCSCENARIOJAPAN_SetThreeDMode(bThreeDMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetNumOfTuners(HINT32 numOfTuner)
{
	// TODO:
	numOfTuner = numOfTuner;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetLimitPerPPV( HCHAR *pszLimitOPrice)
{
	HUINT32 limitPerPpv = 0;
	CHECK_BPL_PARAM_PTR(pszLimitOPrice);
	if(HxSTD_StrCmp(pszLimitOPrice, OPL_JLABS_LOCALSYSTEM_UNLIMITED_PRICE_STR) == 0)
	{
		limitPerPpv = 0;
	}
	else
	{
		limitPerPpv = HLIB_STD_StrToUINT32(pszLimitOPrice);
	}
	APK_SQCSCENARIOJAPAN_SetLimitPerPPV(limitPerPpv);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetLimitPerPPM( HCHAR *pszLimitOPrice)
{
	HUINT32 limitPerPpm = 0;
	CHECK_BPL_PARAM_PTR(pszLimitOPrice);
	if(HxSTD_StrCmp(pszLimitOPrice, OPL_JLABS_LOCALSYSTEM_UNLIMITED_PRICE_STR) == 0)
	{
		limitPerPpm = 0;
	}
	else
	{
		limitPerPpm = HLIB_STD_StrToUINT32(pszLimitOPrice);
	}
	APK_SQCSCENARIOJAPAN_SetLimitPerPPM(limitPerPpm);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetFriendlyName( HCHAR *pszFriendlyName)
{
	APK_DLNADMS_SetFriendlyName(pszFriendlyName);
	APK_DB_SetStr( USERDB_DLNA_DMS_FRIENDLYNAME, pszFriendlyName );

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetNaviUrl( HCHAR *pszUrl)
{
	CHECK_BPL_PARAM_PTR(pszUrl);
	if(APK_SQCSCENARIOJAPAN_SetNaviURL(pszUrl) != ERR_OK)
	{
		HxLOG_Error("Setting the  naviUrl is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetInteracTVUrl( HCHAR *pszUrl)
{
	CHECK_BPL_PARAM_PTR(pszUrl);
	if(APK_SQCSCENARIOJAPAN_SetInteracTVURL(pszUrl) != ERR_OK)
	{
		HxLOG_Error("Setting the  InteracTVURL is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetInternetBrowserUrl( HCHAR *pszUrl)
{
	CHECK_BPL_PARAM_PTR(pszUrl);
	if(APK_SQCSCENARIOJAPAN_SetInternetBrowserURL(pszUrl) != ERR_OK)
	{
		HxLOG_Error("Setting the  InternetBrowserURL is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetVodHomeUrl( HCHAR *pszUrl)
{
	CHECK_BPL_PARAM_PTR(pszUrl);
	if(APK_SQCSCENARIOJAPAN_SetVodHomeURL(pszUrl) != ERR_OK)
	{
		HxLOG_Error("Setting the  VodHomeUrl is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetAcsUrl( HCHAR *pszUrl)
{
	CHECK_BPL_PARAM_PTR(pszUrl);
	if(APK_SQCSCENARIOJAPAN_SetAcsURL(pszUrl) != ERR_OK)
	{
		HxLOG_Error("Setting the  AcsUrl is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetTuningRange(HINT32 TuningRange)
{
	APK_DB_SetInt(USERDB_JLABS_TUNING_RANGE, TuningRange);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetTimebarDispMode(HINT32 mode)
{
	APK_DB_SetInt(USERDB_JLABS_TIMEBAR_DISP_MODE, mode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetPinUnlockExpireOption( HINT32 option)
{
	APK_SQCSCENARIOJAPAN_SetPinUnlockExpireOption(option);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetAmPermissionLevel( HINT32 level)
{
	APK_SQCSCENARIOJAPAN_SetAmPermissionLevel(level);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetStandbyMode(HINT32 mode)
{
	APKE_JLABS_StandbyMode_t eStandbyMode = eStandbyMode_None;
	switch(mode)
	{
		case OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_ON:
			eStandbyMode = eStandbyMode_PowerON;
			break;
		case OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_OFF:
			eStandbyMode = eStandbyMode_PowerOFF;
			break;
		default:
			HxLOG_Error("[%s:%d] mode( val : %d ) ret Error!!!\n",__FUNCTION__,__LINE__, mode);
			return BPL_STATUS_ERROR;
	}
	APK_SQCSCENARIOJAPAN_SetStandbyMode(eStandbyMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetBmlInfoDisoEnable( HBOOL bBmlInfoDispEnabled)
{
	APK_SQCSCENARIOJAPAN_SetBmlInfoDispEnable(bBmlInfoDispEnabled);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetIplateDispEnable( HBOOL bIplateDispEnable)
{
	APK_DB_SetInt(USERDB_JLABS_I_PLATE_DISP_ENABLE, bIplateDispEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetFrontDispEnable( HBOOL bFrontDispEnable)
{
	APK_SQCSCENARIOJAPAN_SetFrontDispEnable(bFrontDispEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetIsDMSPublic( HBOOL bIsDMSPublic)
{
	HCHAR szFriendlyName[128];
	HERROR hErr;
	if(bIsDMSPublic)
	{
		HxSTD_MemSet(szFriendlyName, 0, 128);
		hErr = APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szFriendlyName, 128 );
		APK_DLNADMS_Start(szFriendlyName, FALSE);
		APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)TRUE );
	}
	else
	{
		APK_DLNADMS_Stop();
		APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)FALSE );
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetDefaultFactoryForDap(HUINT32 mode)
{
	APK_SQCSCENARIOJAPAN_SetDefaultFactoryForDap(mode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_SetDefaultDiscForDap(HCHAR *pszDiscId)
{
	CHECK_BPL_PARAM_PTR(pszDiscId);
	if (APK_SQCSCENARIOJAPAN_SetDefaultDiscForDap(pszDiscId) != ERR_OK)
	{
		HxLOG_Error("Setting the  DefaultDiscForDap is failed..\n");
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_ResetUserInfo(void)
{
	HBOOL bRet = FALSE;
	APK_SQCSCENARIOJAPAN_UserReset(&bRet);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_GetChannelMaskResult(HBOOL *pChannelMaskResult)
{
	HUINT32  maskResult;
	HBOOL	bResult=FALSE;

	CHECK_BPL_PARAM_PTR(pChannelMaskResult);

	APK_SQCSCENARIOJAPAN_DoChannelMask(&maskResult);

	//printf("OPL_JlabsLocalSystem_GetChannelMaskResult : result = %d\n\n\n", maskResult);

	if(maskResult == eChMaskResult_StartOk)
		bResult = TRUE;
	else if(maskResult == eChMaskResult_StartFail)
		bResult = FALSE;

	*pChannelMaskResult = (BPL_BOOL)bResult;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsLocalSystem_InsertSetupInfo(HINT32 *pnInsertResult)
{
	HUINT32  ulInsertResult = 0;

	CHECK_BPL_PARAM_PTR(pnInsertResult);

	APK_SQCSCENARIOJAPAN_InsertSetupInfo(&ulInsertResult);

	HxLOG_Print("OPL_JlabsLocalSystem_InsertSetupInfo : result = %d\n", ulInsertResult);

	if(ulInsertResult == eInsertSetupInfoResult_OK)
		*pnInsertResult = 0;
	else
		*pnInsertResult = -1;

	return BPL_STATUS_OK;
}

/* end of file */

