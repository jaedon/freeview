/**************************************************************************************/
/**
 * @file NativeJlabsDTTChannel.cpp
 *
 * jlabsDTTChanne module
 *
 * @author  Seo-Lim Park(slpark@humaxdigital.com)
 * @date    2012/03/15
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>

#include "macros.h"
#include "NativeJlabsChannel.h"
#include "NativeJlabsMiscSetting.h"
#include "oplchannel.h"
#include "opllocalsystem.h"
#include "opljlabslocalsystem.h"
#include "oplsignalinfo.h"
#include "opltuner.h"


#define	MAX_BUF_LEN 				32
#define	MAX_URL_BUF_LEN			257
#define	MAX_DMS_FRIENDLY_NAME_BUF_LEN 	128
#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

#define	__GET_MISC_STRING_PROPERTY(func, buffer, length) \
	do{\
		if (!func(buffer, length))\
		{\
			OOIF_LOG_ERROR("[%s:%d] ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);\
			return OOIF_RETURN_ERROR;\
		}\
		if(length>MAX_BUF_LEN)\
		{	bUrl = TRUE;	}\
		eRetType = eReturnType_String;\
	}while(0)
#define	__GET_OUTPUT_STRING_PROPERTY(func) \
	do{\
		if ( !func(0, szGet))\
		{\
			OOIF_LOG_ERROR("[%s:%d] ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);\
			return OOIF_RETURN_ERROR;\
		}\
		eRetType = eReturnType_String;\
	}while(0)
#define	__GET_MISC_NUMBER_PROPERTY(func, get, rettype) \
	do{\
		if (!func(get))\
		{\
			OOIF_LOG_ERROR("[%s:%d] ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);\
			return OOIF_RETURN_ERROR;\
		}\
		eRetType = rettype;\
	}while(0)

#define	__SET_MISC_NUMBER_PROPERTY(func) \
	do{\
		ulSet = HLIB_STD_StrToUINT32(value);\
		setValue = (OOIFNumber)ulSet;\
		if((ret = func(setValue) )!= OOIF_RETURN_OK)\
		{\
			return ret;\
		}\
	}while(0)

#define	__SET_MISC_BOOLEAN_PROPERTY(func)	\
	do{\
		if((!HxSTD_StrCmp(value,(const char *)"false")) || (!HxSTD_StrCmp(value,(const char *)"FALSE")))\
		{setBoolean = FALSE;} \
		else if((!HxSTD_StrCmp(value,(const char *)"true")) || (!HxSTD_StrCmp(value,(const char *)"TRUE")))\
		{setBoolean = TRUE;}\
		else\
			return OOIF_RETURN_NOT_SUPPORTED;\
		if((ret = func(setBoolean) )!= OOIF_RETURN_OK)\
		{\
			return ret;\
		}\
	}while(0)


typedef enum{
	eReturnType_Number,
	eReturnType_Boolean,
	eReturnType_String,
	eReturnType_Max,
}eOOIF_ReturnType;

/* GETTER */
OOIFReturnCode native_jlabsMiscSettingGetPostCode(const char **retval)
{
	static HCHAR szPostcode[MAX_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetPostCode");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szPostcode,0,MAX_BUF_LEN);

	if (!OplJlabsLocalSystem::getPostcode(szPostcode, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getPostcde ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szPostcode);
	*retval = szPostcode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetLocalArea(const char **retval)
{
	static HCHAR szLocalArea[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLocalArea");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	HxSTD_MemSet(szLocalArea,0,MAX_BUF_LEN);

	if (!OplJlabsLocalSystem::getLocalArea(szLocalArea, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getLocalArea ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szLocalArea);

	*retval = szLocalArea;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetTvAspectRatio(const char **retval)
{
	static HCHAR szTVAspectRatio[MAX_BUF_LEN];

	OOIF_LOG_INFO("native_jlabsMiscSettingGetTvAspectRatio");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szTVAspectRatio,0,MAX_BUF_LEN);

	if ( !OplHmxOutput::getTvAspectRatio(0, szTVAspectRatio) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurTvAspectRatio ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] TVAspectRatio:%s\n",__PRETTY_FUNCTION__,__LINE__,szTVAspectRatio);

	*retval = szTVAspectRatio;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSoundMode(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	char szSoundMode[MAX_BUF_LEN];

	HxSTD_MemSet(szSoundMode,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getSoundMode(0, szSoundMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSoundMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	if(HxSTD_StrCmp(szSoundMode,"Left")==0)
		*retval = "MAIN";
	else if(HxSTD_StrCmp(szSoundMode,"Right")==0)
		*retval = "SUB";
	else if(HxSTD_StrCmp(szSoundMode,"Stereo")==0)
		*retval = "BOTH";
	else
		*retval = NULL;

	OOIF_LOG_INFO("[%s:%d] Sound Mode:%s\n",__PRETTY_FUNCTION__,__LINE__,szSoundMode);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSpdifMode(const char **retval)
{
	HCHAR szDigitalOutputMode[MAX_BUF_LEN];

	OOIF_LOG_INFO("native_jlabsMiscSettingGetSpdifMode");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szDigitalOutputMode,0,MAX_BUF_LEN);

	/* set은 한번에 spdif , hdmi 모두 해주고 , get은 spdif 기준으로 가져 간다.
	한개의 필드로 두가지 세팅 */
	if ( !OplHmxAudio::getSpdifAudioOutputMode(0, szDigitalOutputMode) )
	{
		OOIF_LOG_ERROR("[%s:%d] getSpdifOutputMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	if(HxSTD_StrCmp(szDigitalOutputMode,"PCM")==0)
		*retval = "PCM";
	else if(HxSTD_StrCmp(szDigitalOutputMode,"Bypass AAC")==0)
		*retval = "AUTO";
	else
		*retval = "PCM";

	OOIF_LOG_INFO("[%s:%d] Digital Audio Output Mode:%s\n",__PRETTY_FUNCTION__,__LINE__,szDigitalOutputMode);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSubtitleMode(OOIFNumber *retval)
{
	HINT32 subTitleMode = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetSubtitleMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getSubtitleMode(&subTitleMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getSubtitleMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,subTitleMode);

	*retval = subTitleMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSuperImposeMode(OOIFNumber *retval)
{
	HINT32 superImposeMode = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetSuperImposeMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getSuperImposeMode(&superImposeMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getSuperImposeMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,superImposeMode);

	*retval = superImposeMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSetupConfigurationLaunchMode(OOIFNumber *retval)
{
	HUINT32 launchMode = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetSetupConfigurationLaunchMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getSetupConfigureLaunchMode(&launchMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getSetupConfigureLaunchMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,launchMode);

	*retval = launchMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetNumberOfTuners(OOIFNumber *retval)
{
	HUINT32 numOfTuners = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetNumberOfTuners");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	OplTuner::getNumOfDev(&numOfTuners);
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,numOfTuners);
	*retval = numOfTuners;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetDisplayMode3D(OOIFNumber *retval)
{
	HINT32 dispModeFor3D = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetDisplayMode3D");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getDisplayMode3D(&dispModeFor3D))
	{
		OOIF_LOG_ERROR("[%s:%d] getDisplayMode3D ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,dispModeFor3D);

	*retval = dispModeFor3D;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetThreeDMode(OOIFBoolean *retval)
{
	HBOOL bThreeDMode = FALSE;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetThreeDMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getThreeDMode(&bThreeDMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getThreeDMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bThreeDMode);

	*retval = bThreeDMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetAutoSoftwareDownloadEnable(OOIFBoolean*retval)
{
	HBOOL swDownloadEnale = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetAutoSoftwareDownloadEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getAutoSWDownloadEnable(&swDownloadEnale))
	{
		OOIF_LOG_ERROR("[%s:%d] getAutoSWDownloadEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,swDownloadEnale);

	*retval = swDownloadEnale;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetChannelMaskEnable(OOIFBoolean *retval)
{
	HBOOL chMaskEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetChannelMaskEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getChannelMaskEnable(&chMaskEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getChannelMaskEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,chMaskEnable);

	*retval = chMaskEnable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetSeamlessEnable(OOIFBoolean *retval)
{
	HBOOL seamlessEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetSeamlessEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getSeamlessEnable(&seamlessEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getSeamlessEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,seamlessEnable);

	*retval = seamlessEnable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetAutoPPVPurchaseEnable(OOIFBoolean *retval)
{
	HBOOL autoPpvPurchaseEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetAutoPPVPurchaseEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getAutoPPVPurchaseEnable(&autoPpvPurchaseEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getAutoPPVPurchaseEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,autoPpvPurchaseEnable);

	*retval = autoPpvPurchaseEnable;
	return OOIF_RETURN_OK;
}

/* read only */
OOIFReturnCode native_jlabsMiscSettingGetLastChannelNetworkTypeName(OOIFNumber *retval)
{
	OPL_NetworkType_e		eNetType = eOPL_NETWORK_END;
	OPL_ChannelManager_t	 	chManager;
	OPL_Channel_t			 hChannel = NULL;
	HCHAR					 szCcid[MAX_BUF_LEN] = { 0, };

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLastChannelNetworkTypeName");

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return OOIF_RETURN_VALUE_NULL;
	}
	if(OplJlabsLocalSystem::getLastChCCID(szCcid,MAX_BUF_LEN))
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		hChannel = OPL_Channel_FindChannelAt (chManager, 0);
		if (NULL == hChannel)
		{
			return OOIF_RETURN_VALUE_NULL;
		}
	}
	eNetType = OPL_Channel_GetNetTypeByOnID(OPL_Channel_GetOrgNetworkID(hChannel));
	switch(eNetType)
	{
		case eOPL_DTT_NETWORK:
			*retval = JLABS_DTT_NETWORK;
			break;
		case eOPL_BS_NETWORK:
			*retval = JLABS_BS_NETWORK;
			break;
		case eOPL_CATV_NETWORK:
			*retval = JLABS_CATV_NETWORK;
			break;
		case eOPL_CS1_NETWORK:
			*retval = JLABS_CS1_NETWORK;
			break;
		case eOPL_CS2_NETWORK:
			*retval = JLABS_CS2_NETWORK;
			break;
		case eOPL_JCHITS_NETWORK:
			*retval = JLABS_JCHITS_NETWORK;
			break;
		default:
			return OOIF_RETURN_VALUE_NULL;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetLastChannelThreeDigitNumber(OOIFNumber *retval)
{
	OPL_ChannelManager_t	 	chManager;
	OPL_Channel_t			 hChannel = NULL;
	HCHAR					 szCcid[MAX_BUF_LEN] = { 0, };
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLastChannelThreeDigitNumber");

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	if(OplJlabsLocalSystem::getLastChCCID(szCcid,MAX_BUF_LEN))
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		hChannel = OPL_Channel_FindChannelAt (chManager, 0);
		if (NULL == hChannel)
		{
			return OOIF_RETURN_VALUE_UNDEFINED;
		}
	}

	*retval = OPL_Channel_GetMajorChannel(hChannel);
	return OOIF_RETURN_OK;
}

#if defined(JLABS_JCOM)
OOIFReturnCode native_jlabsMiscSettingGetLimitPerPPV(const char **retval)
{
	static HCHAR szLimitPrice[MAX_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLimitPerPPV");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szLimitPrice,0,MAX_BUF_LEN);

	if (!OplJlabsLocalSystem::getLimitPerPPV(szLimitPrice, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getLimitPerPPV ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szLimitPrice);

	*retval = szLimitPrice;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetLimitPerPPM(const char **retval)
{
	static HCHAR szLimitPrice[MAX_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLimitPerPPM");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szLimitPrice,0,MAX_BUF_LEN);

	if (!OplJlabsLocalSystem::getLimitPerPPM(szLimitPrice, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getLimitPerPPM ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szLimitPrice);

	*retval = szLimitPrice;
	return OOIF_RETURN_OK;
}

/*삭제예정*/
OOIFReturnCode native_jlabsMiscSettingGetHdmiAudioMode(const char **retval)
{
/*
	HCHAR szPostcode[MAX_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetHdmiAudioMode");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szPostcode,0,MAX_BUF_LEN);

	if (!OplJlabsLocalSystem::getPostcode(szPostcode, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getPostcde ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szPostcode);

	*retval = szPostcode;
*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetFriendlyName(const char **retval)
{
	static HCHAR szFriendlyName[MAX_DMS_FRIENDLY_NAME_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetFriendlyName");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szFriendlyName, 0, MAX_DMS_FRIENDLY_NAME_BUF_LEN);

	if (!OplJlabsLocalSystem::getFriendlyName(szFriendlyName, MAX_DMS_FRIENDLY_NAME_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getFriendlyName ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szFriendlyName);

	*retval = szFriendlyName;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetNaviUrl(const char **retval)
{
	static HCHAR szUrl[MAX_URL_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetNaviUrl");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szUrl,0,MAX_URL_BUF_LEN);

	if (!OplJlabsLocalSystem::getNaviURL(szUrl, MAX_URL_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getNaviURL ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szUrl);

	*retval = szUrl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetInteracTVURL(const char **retval)
{
	static HCHAR szUrl[MAX_URL_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetInteracTVURL");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szUrl,0,MAX_URL_BUF_LEN);

	if (!OplJlabsLocalSystem::getInteracTVURL(szUrl, MAX_URL_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getNaviURL ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szUrl);

	*retval = szUrl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetInternetBrowserURL(const char **retval)
{
	static HCHAR szUrl[MAX_URL_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetInternetBrowserURL");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szUrl,0,MAX_URL_BUF_LEN);

	if (!OplJlabsLocalSystem::getInternetBrowserURL(szUrl, MAX_URL_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getInternetBrowserURL ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szUrl);

	*retval = szUrl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetVodHomeURL(const char **retval)
{
	static HCHAR szUrl[MAX_URL_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetVodHomeURL");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szUrl,0,MAX_URL_BUF_LEN);

	if (!OplJlabsLocalSystem::getVodHomeURL(szUrl, MAX_URL_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getNaviURL ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szUrl);

	*retval = szUrl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetAcsUrl(const char **retval)
{
	static HCHAR szUrl[MAX_URL_BUF_LEN];

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetNaviUrl");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szUrl,0,MAX_URL_BUF_LEN);

	if (!OplJlabsLocalSystem::getACSURL(szUrl, MAX_URL_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getNaviURL ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szUrl);

	*retval = szUrl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetPinUnlockExpiration(OOIFNumber *retval)
{
	HINT32 pinUnlocExpire = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetPinUnlockExpiration");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getPinUnlockExpireOption(&pinUnlocExpire))
	{
		OOIF_LOG_ERROR("[%s:%d] getPinUnlockExpireOption ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,pinUnlocExpire);

	*retval = pinUnlocExpire;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetTuningRange(OOIFNumber *retval)
{
	HINT32 tuningRange = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetTuningRange");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getTuningRange(&tuningRange))
	{
		OOIF_LOG_ERROR("[%s:%d] getTuningRange ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,tuningRange);

	*retval = tuningRange;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetTimebarDisplayMode(OOIFNumber *retval)
{
	HINT32 timebarDispMode = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetTimebarDisplayMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getTimebarDisplayMode(&timebarDispMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getTimebarDisplayMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,timebarDispMode);

	*retval = timebarDispMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetHdmiCecMode(OOIFNumber*retval)
{
	HINT32 HdmiCecMode = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetHdmiCecMode");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplLocalSystem::getHdmiCecMode(&HdmiCecMode))
	{
		OOIF_LOG_ERROR("[%s:%d] getHdmiCecMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,HdmiCecMode);

	*retval = HdmiCecMode;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetAMPermissionLevel(OOIFNumber *retval)
{
	HINT32 level = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetAMPermissionLevel");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getAMPermissionLevel(&level))
	{
		OOIF_LOG_ERROR("[%s:%d] getAMPermissionLevel ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,level);

	*retval = level;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetHdmiResolutionMode(OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	static char szResolution[(MAX_BUF_LEN+1)];

	HxSTD_MemSet(szResolution,0,(MAX_BUF_LEN+1));

	if ( !OplHmxOutput::getCurResolution(0/* 상관없음 */, szResolution, MAX_BUF_LEN) )
	{
		OOIF_LOG_ERROR("[%s:%d] getCurResolution ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] pszResolution:%s\n",__PRETTY_FUNCTION__,__LINE__,szResolution);

	// TODO: sbkim, spec 정해지며 다시 수정 해야함.
	if( !HxSTD_StrCmp( szResolution , "Original" ) )
	{
		*retval = 1; /* 번조 추종 */
	}
	else if( !HxSTD_StrCmp( szResolution, "Native") )
	{
		*retval = 0; /* 고정 -  EDID MAX */
	}
	else
	{
		/* error case */
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetBmlInfoDisplayEnable(OOIFBoolean*retval)
{
	HBOOL bBmlInfoDispEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetBmlInfoDisplayEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getBmlInfoDisplayEnable(&bBmlInfoDispEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getBmlInfoDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bBmlInfoDispEnable);

	*retval = bBmlInfoDispEnable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetIPlateDisplayEnable(OOIFBoolean*retval)
{
	HBOOL bIplateDispEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetIPlateDisplayEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getIPlateDisplayEnable(&bIplateDispEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getIPlateDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIplateDispEnable);

	*retval = bIplateDispEnable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetFrontDisplayEnable(OOIFBoolean*retval)
{
	HBOOL bFrontDispEnable = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetFrontDisplayEnable");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getFrontDisplayEnable(&bFrontDispEnable))
	{
		OOIF_LOG_ERROR("[%s:%d] getFrontDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bFrontDispEnable);

	*retval = bFrontDispEnable;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetIsDMSPublic(OOIFBoolean*retval)
{
	HBOOL bIsDMSPublic = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetIsDMSPublic");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getIsDMSPublic(&bIsDMSPublic))
	{
		OOIF_LOG_ERROR("[%s:%d] getIsDMSPublic ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsDMSPublic);

	*retval = bIsDMSPublic;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetIsDMSDeliver(OOIFBoolean*retval)
{
	HBOOL bIsDMSDeliver = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetIsDMSDeliver");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getIsDMSDeliver(&bIsDMSDeliver))
	{
		OOIF_LOG_ERROR("[%s:%d] getIsDMSDeliver ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsDMSDeliver);

	*retval = bIsDMSDeliver;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetIsIPDubbing(OOIFBoolean*retval)
{
	HBOOL bIsIPDubbing = 0;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetIsIPDubbing");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getIsIPDubbing(&bIsIPDubbing))
	{
		OOIF_LOG_ERROR("[%s:%d] bIsIPDubbing ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsIPDubbing);

	*retval = bIsIPDubbing;
	return OOIF_RETURN_OK;
}

static OOIFReturnCode native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(HUINT32 networkType, OOIFNumber *retvalMajor, OOIFNumber *retvalBranch)
{
	OPL_ChannelManager_t	 	chManager;
	OPL_Channel_t			 hChannel = NULL;
	HCHAR					 szCcid[MAX_BUF_LEN] = { 0, };
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLastDTTChannelThreeDigitNumber");
	if((NULL == retvalMajor) && (NULL == retvalBranch))
	{
		return OOIF_RETURN_ERROR;
	}
	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		if(retvalMajor != NULL)
			*retvalMajor = 0;
		if(retvalBranch != NULL)
			*retvalBranch = -1;
		return OOIF_RETURN_OK;
	}
	if(OplJlabsLocalSystem::getLastChCCIDByNetwork(networkType, szCcid, MAX_BUF_LEN))
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		if(retvalMajor != NULL)
			*retvalMajor = 0;
		if(retvalBranch != NULL)
			*retvalBranch = -1;
		return OOIF_RETURN_OK;
	}

	if(retvalMajor != NULL)
		*retvalMajor = OPL_Channel_GetMajorChannel(hChannel);
	if(retvalBranch != NULL)
	{
		if(networkType == eOPL_DTT_NETWORK)
			*retvalBranch = OPL_Channel_GetBranchNum(hChannel);
		else
			*retvalBranch = -1;
	}
	return OOIF_RETURN_OK;
}

/* read only */
OOIFReturnCode native_jlabsMiscSettingGetLastDTTChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_DTT_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastDttChannelBranchNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_DTT_NETWORK, NULL, retval);
}

OOIFReturnCode native_jlabsMiscSettingGetLastBSChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_BS_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastCATVChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_CATV_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastCS1ChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_CS1_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastCS2ChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_CS2_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastJCHITSChannelThreeDigitNumber(OOIFNumber *retval)
{
	return native_jlabsMiscSettingGetLastChannelThreeDigitNumberByNetwork(eOPL_JCHITS_NETWORK, retval, NULL);
}

OOIFReturnCode native_jlabsMiscSettingGetLastChannelBranchNumber(OOIFNumber *retval)
{
	OPL_ChannelManager_t	 	chManager;
	OPL_Channel_t			 hChannel = NULL;
	HCHAR					 szCcid[MAX_BUF_LEN] = { 0, };
	OPL_NetworkType_e		eNetType = eOPL_NETWORK_END;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetLastChannelBranchNumber");

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	if(OplJlabsLocalSystem::getLastChCCID(szCcid,MAX_BUF_LEN))
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		hChannel = OPL_Channel_FindChannelAt (chManager, 0);
		if (NULL == hChannel)
		{
			return OOIF_RETURN_VALUE_UNDEFINED;
		}
	}
	eNetType = OPL_Channel_GetNetTypeByOnID(OPL_Channel_GetOrgNetworkID(hChannel));
	if(eNetType == eOPL_DTT_NETWORK)
	{
		*retval = OPL_Channel_GetBranchNum(hChannel);
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetChannelMaskPattern(OOIFNumber *retval)
{
	HUINT32 pattern;

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetChannelMaskPattern");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::getChannelMaskPattern(&pattern))
	{
		OOIF_LOG_ERROR("[%s:%d] getChannelMaskPattern ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	if((pattern != 1) && (pattern != 2))
		pattern = 0;

	OOIF_LOG_INFO("[%s:%d] value:%d\n", __PRETTY_FUNCTION__, __LINE__, pattern);

	*retval = pattern;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetStandbyMode(OOIFNumber *retval)
{
	HINT32 mode;

	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetStandbyMode");

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsLocalSystem::getStandbyMode(&mode))
	{
		OOIF_LOG_ERROR("[%s:%d] getStandbyMode ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n", __PRETTY_FUNCTION__, __LINE__, mode);

	*retval = static_cast<OOIFNumber>(mode);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingGetIsOutputHD(OOIFBoolean*retval)
{
	HBOOL bIsOutputHD = TRUE;
	OOIF_LOG_DUMMY("native_jlabsMiscSettingGetIsOutputHD");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	OOIF_LOG_INFO("[%s:%d] isOutputHD is not implemented yet\n",__PRETTY_FUNCTION__,__LINE__);
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsOutputHD);
	*retval = bIsOutputHD;
	return OOIF_RETURN_OK;
}

#endif

/* SETTER */
OOIFReturnCode native_jlabsMiscSettingSetPostCode(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.postCode to : %s", value);

	if ( !OplJlabsLocalSystem::setPostcode((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setPostcode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetLocalArea(const char *value)
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("Setting MiscSetting.localArea to : %s", value);
	if ( !OplJlabsLocalSystem::setLocalArea((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] localArea ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetTvAspectRatio(const char *value)
{
	OOIF_LOG_INFO("Setting MiscSetting.tvAspectRatio to : %s", value);

	if ( !OplHmxOutput::setTvAspectRatio(0, value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setTvAspectRatio ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSoundMode(const char *value)
{
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if(HxSTD_StrCmp(value,"MAIN")==0)
		value = "Left";
	else if(HxSTD_StrCmp(value,"SUB")==0)
		value = "Right";
	else if(HxSTD_StrCmp(value,"BOTH")==0)
		value = "Stereo";
	else
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplHmxAudio::setSoundMode(0, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSoundMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSpdifMode(const char *value)
{
	HBOOL	bIsAuto = FALSE;
	OOIF_LOG_INFO("Setting MiscSetting.spdifMode to : %s", value);
	if (NULL == value)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if(HxSTD_StrCmp(value, "AUTO") == 0)
		bIsAuto = TRUE;

	if( bIsAuto == TRUE )
		value = "Bypass AAC";
	else
		value = "PCM";

	/* set은 한번에 spdif , hdmi 모두 해주고 , get은 spdif 기준으로 가져 간다.
	한개의 필드로 두가지 세팅 */

	/* 1. spdif */
	if (!OplHmxAudio::setSpdifAudioOutputMode(0, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSpdifOutputMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	/* hdmi */
	if (!OplHmxAudio::setHdmiAudioOutputMode(0, value))
	{
		OOIF_LOG_ERROR("[%s:%d] setHdmiOutputMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSubtitleMode(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.subtitleMode to : %f", value);

	if (!OplJlabsLocalSystem::setSubtitleMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSubtitleMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSuperImposeMode(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.superImposeMode to : %f", value);

	if (!OplJlabsLocalSystem::setSuperImposeMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSuperImposeMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSetupConfigurationLaunchMode(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.configurationLaunchMode to : %f", value);

	if (!OplJlabsLocalSystem::setSetupConfigureLaunchMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSetupConfigureLaunchMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetDisplyMode3D(OOIFNumber value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.displyMode3D to : %f", value);

	if (!OplJlabsLocalSystem::setDisplayMode3D((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setDisplayMode3D ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetNumberOfTuners(OOIFNumber value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.numberOfTuners to : %f", value);

	if (!OplJlabsLocalSystem::setNumOfTuners((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setNumOfTuners ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetThreeDMode(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.ThreeDMode to : %f", value);

	if (!OplJlabsLocalSystem::setThreeDMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setThreeDMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetAutoSoftwareDownloadEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.autoSoftsareDownloadEnable to : %d", value);
	if (!OplJlabsLocalSystem::setAutoSWDownloadEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setAutoSWDownloadEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetChannelMaskEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.ChannelMaskEnable to : %d", value);
	if (!OplJlabsLocalSystem::setChannelMaskEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setChannelMaskEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetSeamlessEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.SeamlessEnable to : %d", value);
	if (!OplJlabsLocalSystem::setSeamlessEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setSeamlessEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetAutoPPVPurchaseEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.AutoPPVPurchaseEnable to : %d", value);
	if (!OplJlabsLocalSystem::setAutoPPVPurchaseEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setAutoPPVPurchaseEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

// String get(String key)
OOIFReturnCode native_jlabsMiscSettingGet(const char *name, const char **retval)
{
	static HCHAR 		szGet[MAX_BUF_LEN];
	static HCHAR		szUrlBuf[MAX_URL_BUF_LEN];
	HCHAR			*pcGet = NULL;
	HINT32			nGet = 0;
	HBOOL			bGet = FALSE, bUrl = FALSE;
	OOIFNumber		getValue = 0;
	OOIFBoolean		bGetValue = 0;
	eOOIF_ReturnType	eRetType = eReturnType_Max;
	OOIFReturnCode 	ret = OOIF_RETURN_ERROR;

	if(name == NULL || retval == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szGet, 0x00, MAX_BUF_LEN);
	if (!HxSTD_StrCmp(name,(const char *)"postCode"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getPostcode, szGet, MAX_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"localArea"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getLocalArea, szGet, MAX_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"tvAspectRatio"))
	{
		__GET_OUTPUT_STRING_PROPERTY(OplHmxOutput::getTvAspectRatio);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"dualMono"))
	{
		if((ret = native_jlabsMiscSettingGetSoundMode((const char **)&pcGet) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		HxSTD_StrNCpy(szGet, pcGet, MAX_BUF_LEN-1);
		eRetType = eReturnType_String;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"spdifMode"))
	{
		if((ret = native_jlabsMiscSettingGetSpdifMode((const char **)&pcGet) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		HxSTD_StrNCpy(szGet, pcGet, MAX_BUF_LEN-1);
		eRetType = eReturnType_String;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"subtitleMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getSubtitleMode, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"superimposeMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getSuperImposeMode, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"setupConfigurationLaunchMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getSetupConfigureLaunchMode, (HUINT32*)&nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"numberOfTuners"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplTuner::getNumOfDev, (HUINT32*)&nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"displayMode3D"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getDisplayMode3D, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"ThreeDMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getThreeDMode, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"autoSoftwareDownloadEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getAutoSWDownloadEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"channelMaskEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getChannelMaskEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"seamlessEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getSeamlessEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"autoPPVPurchaseEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getAutoPPVPurchaseEnable, &bGet, eReturnType_Boolean);
	}
#if defined(JLABS_JCOM)
	else if(!HxSTD_StrCmp(name,(const char *)"limitPerPPV"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getLimitPerPPV, szGet, MAX_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"limitPayPerMonth"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getLimitPerPPM, szGet, MAX_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"friendlyName"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getFriendlyName, szGet, MAX_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"naviURL"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getNaviURL, szUrlBuf, MAX_URL_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"interacTVURL"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getInteracTVURL, szUrlBuf, MAX_URL_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"internetBrowserURL"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getInteracTVURL, szUrlBuf, MAX_URL_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"vodHomeURL"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getVodHomeURL, szUrlBuf, MAX_URL_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"acsURL"))
	{
		__GET_MISC_STRING_PROPERTY(OplJlabsLocalSystem::getACSURL, szUrlBuf, MAX_URL_BUF_LEN);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiAudioMode"))
	{
		//시나리오에서 삭제됨.
	}
	else if(!HxSTD_StrCmp(name,(const char *)"pinUnlockExpiration"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getPinUnlockExpireOption, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"instantReplay"))
	{
		//시나리오에서 삭제됨.
	}
	else if(!HxSTD_StrCmp(name,(const char *)"instantSkip"))
	{
		//시나리오에서 삭제됨.
	}
	else if(!HxSTD_StrCmp(name,(const char *)"tuningRange"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getTuningRange, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"timebarDisplayMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getTimebarDisplayMode, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiCECMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplLocalSystem::getHdmiCecMode, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"amPermissionLevel"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getAMPermissionLevel, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiResolutionMode"))
	{
		if((ret = native_jlabsMiscSettingGetHdmiResolutionMode(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"bmlInfoDisplayEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getBmlInfoDisplayEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"iPlateDisplayEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getIPlateDisplayEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"frontDisplayEnable"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getFrontDisplayEnable, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"isDMSPublic"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getIsDMSPublic, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"isDMSDeliver"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getIsDMSDeliver, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"isIPDubbing"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getIsIPDubbing, &bGet, eReturnType_Boolean);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastDttChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastDTTChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastDttChannelBranchNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastDttChannelBranchNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastBsChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastBSChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastCatvChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastCATVChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastCs1ChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastCS1ChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastCs2ChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastCS2ChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastJchitsChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastJCHITSChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastChannelBranchNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastChannelBranchNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"channelMaskPattern"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getChannelMaskPattern, (HUINT32*)&nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"standbyMode"))
	{
		__GET_MISC_NUMBER_PROPERTY(OplJlabsLocalSystem::getStandbyMode, &nGet, eReturnType_Number);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"isOutputHD"))
	{
		if((ret = native_jlabsMiscSettingGetIsOutputHD(&bGetValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		bGet = (HINT32)bGetValue;
		eRetType = eReturnType_Boolean;
	}

#endif
	else if(!HxSTD_StrCmp(name,(const char *)"lastChannelNetworkTypeName"))
	{
		if((ret = native_jlabsMiscSettingGetLastChannelNetworkTypeName(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}
	else if(!HxSTD_StrCmp(name,(const char *)"lastChannelThreeDigitNumber"))
	{
		if((ret = native_jlabsMiscSettingGetLastChannelThreeDigitNumber(&getValue) )!= OOIF_RETURN_OK)
		{
			return ret;
		}
		nGet = (HINT32)getValue;
		eRetType = eReturnType_Number;
	}

	if(eRetType == eReturnType_Number)
	{
		HxSTD_snprintf(szGet, MAX_BUF_LEN-1, "%d", nGet);
		szGet[MAX_BUF_LEN-1] = '\0';
		*retval  = szGet;
	}
	else if(eRetType == eReturnType_Boolean)
	{
		if(bGet == FALSE)
		{
			HxSTD_StrNCpy(szGet, "false", MAX_BUF_LEN-1);
		}
		else if(bGet == TRUE)
		{
			HxSTD_StrNCpy(szGet, "true", MAX_BUF_LEN-1);
		}
		else
		{
			*retval = "unDefined";
			return OOIF_RETURN_VALUE_UNDEFINED;
		}
		*retval  = szGet;
	}
	else if(eRetType == eReturnType_String)
	{
		if(bUrl == FALSE)
		{
			*retval  = szGet;
		}
		else
		{
			*retval  = szUrlBuf;
		}
	}
	else
	{
		*retval = "unDefined";
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

// void set(String key, String value);
OOIFReturnCode native_jlabsMiscSettingSet(const char *name, const char *value)
{
	HUINT32			ulSet = 0;
	OOIFNumber		setValue = 0;
	OOIFBoolean		setBoolean = FALSE;
	OOIFReturnCode 	ret = OOIF_RETURN_ERROR;

	if(name == NULL || value == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!HxSTD_StrCmp(name,(const char *)"postCode"))
	{
		return native_jlabsMiscSettingSetPostCode(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"localArea"))
	{
		return native_jlabsMiscSettingSetLocalArea(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"tvAspectRatio"))
	{
		return native_jlabsMiscSettingSetTvAspectRatio(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"dualMono"))
	{
		return native_jlabsMiscSettingSetSoundMode(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"spdifMode"))
	{
		return native_jlabsMiscSettingSetSpdifMode(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"subtitleMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetSubtitleMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"superimposeMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetSuperImposeMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"setupConfigurationLaunchMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetSetupConfigurationLaunchMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"numberOfTuners"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetNumberOfTuners);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"displayMode3D"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetDisplyMode3D);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"threeDMode"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetThreeDMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"autoSoftwareDownloadEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetAutoSoftwareDownloadEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"channelMaskEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetChannelMaskEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"seamlessEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetSeamlessEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"autoPPVPurchaseEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetAutoPPVPurchaseEnable);
	}
#if defined(JLABS_JCOM)
	else if(!HxSTD_StrCmp(name,(const char *)"limitPerPPV"))
	{
		return native_jlabsMiscSettingSetLimitPerPPV(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"limitPayPerMonth"))
	{
		return native_jlabsMiscSettingSetLimitPerPPM(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"friendlyName"))
	{
		return native_jlabsMiscSettingSetFriendlyName(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"naviURL"))
	{
		return native_jlabsMiscSettingSetNaviUrl(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"interacTVURL"))
	{
		return native_jlabsMiscSettingSetInteracTVUrl(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"internetBrowserURL"))
	{
		return native_jlabsMiscSettingSetInternetBrowserUrl(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"vodHomeURL"))
	{
		return native_jlabsMiscSettingSetVodHomeUrl(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"acsURL"))
	{
		return native_jlabsMiscSettingSetAcsUrl(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiAudioMode"))
	{
		return native_jlabsMiscSettingSetHdmiAudioMode(value);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"pinUnlockExpiration"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetPinUnlockExpiration);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"tuningRange"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetTuningRange);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"timebarDisplayMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetTimebarDisplayMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiCECMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetHdmiCecMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"amPermissionLevel"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetAmPermissionLevel);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"bmlInfoDisplayEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetBmlInfoDisplayEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"iPlateDisplayEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetIPlateDisplayEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"frontDisplayEnable"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetFrontDisplayEnable);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"isDMSPublic"))
	{
		__SET_MISC_BOOLEAN_PROPERTY(native_jlabsMiscSettingSetIsDMSPublic);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"hdmiResolutionMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetHdmiResolutionMode);
	}
	else if(!HxSTD_StrCmp(name,(const char *)"standbyMode"))
	{
		__SET_MISC_NUMBER_PROPERTY(native_jlabsMiscSettingSetStandbyMode);
	}
#endif
	else
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

#if defined(JLABS_JCOM)
OOIFReturnCode native_jlabsMiscSettingSetLimitPerPPV(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.limitPerPPV to : %s", value);

	if ( !OplJlabsLocalSystem::setLimitPerPPV((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setLimitPerPPV ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetLimitPerPPM(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.limitPerPPM to : %s", value);

	if ( !OplJlabsLocalSystem::setLimitPerPPM((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setLimitPerPPM ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetHdmiAudioMode(const char *value)
{
	/*
	OOIF_LOG_DUMMY("Setting MiscSetting.postCode to : %s", value);

	if ( !OplJlabsLocalSystem::setPostcode((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setPostcode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);
	*/
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetFriendlyName(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.friendlyName to : %s", value);

	if ( !OplJlabsLocalSystem::setFriendlyName((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] friendlyName ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetNaviUrl(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.naviUrl to : %s", value);

	if ( !OplJlabsLocalSystem::setNaviUrl((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setNaviUrl ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetInteracTVUrl(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.interacTvUrl to : %s", value);

	if ( !OplJlabsLocalSystem::setInteracTVUrl((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setInteracTVUrl ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetInternetBrowserUrl(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.internetBrowserUrl to : %s", value);

	if ( !OplJlabsLocalSystem::setInternetBrowserUrl((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setInternetBrowserUrl ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetVodHomeUrl(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.vodHomeUrl to : %s", value);

	if ( !OplJlabsLocalSystem::setVodHomeUrl((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setVodHomeUrl ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetAcsUrl(const char *value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.acsUrl to : %s", value);

	if ( !OplJlabsLocalSystem::setAcsUrl((HCHAR*)value) )
	{
		OOIF_LOG_ERROR("[%s:%d] setAcsUrl ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetPinUnlockExpiration(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.pinUnlockExpiration to : %f", value);

	if (!OplJlabsLocalSystem::setPinUnlockExpireOption((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setPinUnlockExpireOption ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetTuningRange(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.tuningRange to : %f", value);

	if (!OplJlabsLocalSystem::setTuningRange((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setTuningRange ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetTimebarDisplayMode(OOIFNumber value)
{
	OOIF_LOG_INFO("Setting MiscSetting.timebarDispMode to : %f", value);

	if (!OplJlabsLocalSystem::setTimebarDisplayMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setTimebarDisplayMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetHdmiResolutionMode(OOIFNumber value)
{
	static char szSetResolution[(MAX_BUF_LEN+1)];

	// TODO: sbkim, spec 정해지며 다시 수정 해야함.
	switch( (int)value )
	{
		case 0:
			HxSTD_snprintf(szSetResolution, MAX_BUF_LEN-1, "Native");
			break;
		case 1:
			HxSTD_snprintf(szSetResolution, MAX_BUF_LEN-1, "Original");
			break;
		default:
			OOIF_LOG_ERROR("[%s:%d] setCurResolution( val : %f ) ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__, value);
			return OOIF_RETURN_ERROR;
			break;
	}

	if ( !OplHmxOutput::setCurResolution((unsigned int)0/* 상관없음 */, szSetResolution) )
	{
		OOIF_LOG_ERROR("[%s:%d] setCurResolution ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szSetResolution);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetStandbyMode(OOIFNumber value)
{
	HINT32 mode = (HINT32)value;
	OOIF_LOG_DUMMY("Setting MiscSetting.standbyMode to : %d\n", (int)value);

	if(!OplJlabsLocalSystem::setStandbyMode(mode))
	{
		OOIF_LOG_ERROR("[%s:%d] setStandbyMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] mode:%d\n",__PRETTY_FUNCTION__,__LINE__,mode);

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsMiscSettingSetHdmiCecMode(OOIFNumber value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.hdmiCecMode to : %d\n", (int)value);
	if (!OplLocalSystem::setHdmiCecMode((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setHdmiCecMode ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetAmPermissionLevel(OOIFNumber value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.amPermissionLevel to : %d", (int)value);
	if (!OplJlabsLocalSystem::setAmPermissionLevel((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setAmPermissionLevel ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetBmlInfoDisplayEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.bmlInfoDispEnable to : %d", value);
	if (!OplJlabsLocalSystem::setBmlInfoDisplayEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setBmlInfoDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetIPlateDisplayEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.iPlateDispEnable to : %d", value);
	if (!OplJlabsLocalSystem::setIPlateDisplayEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setIPlateDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetFrontDisplayEnable(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.frontDispEnable to : %d", value);
	if (!OplJlabsLocalSystem::setFrontDisplayEnable((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setFrontDisplayEnable ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMiscSettingSetIsDMSPublic(OOIFBoolean value)
{
	OOIF_LOG_DUMMY("Setting MiscSetting.isDMSPublic to : %d", value);
	if (!OplJlabsLocalSystem::setIsDMSPublic((int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] setIsDMSPublic ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

#endif
#endif // #if defined OIPF && defined JLABS

