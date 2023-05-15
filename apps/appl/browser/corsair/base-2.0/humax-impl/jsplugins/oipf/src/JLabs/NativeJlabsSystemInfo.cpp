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
#include "NativeJlabsSystemInfo.h"
#include "opljlabssysteminfo.h"

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

/* getter read only */
#define	MAX_BUF_LEN 32

#define	__GET_SYS_STRING_PROPERTY(func, buffer, length) \
	do{\
		if (!func(buffer, length))\
		{\
			OOIF_LOG_ERROR("[%s:%d] ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);\
			return OOIF_RETURN_ERROR;\
		}\
		eRetType = eReturnType_String;\
	}while(0)

#define	__GET_SYS_NUMBER_PROPERTY(func, get, rettype) \
	do{\
		if (!func(get))\
		{\
			OOIF_LOG_ERROR("[%s:%d] ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);\
			return OOIF_RETURN_ERROR;\
		}\
		eRetType = rettype;\
	}while(0)

typedef enum{
	eReturnType_Number,
	eReturnType_Boolean,
	eReturnType_String,
	eReturnType_Max,
}eOOIF_ReturnType;

OOIFReturnCode native_jlabsSystemInfoGetDocsisModemStatus(OOIFNumber *retval)
{
	HUINT32	ulDocsisModemStatus = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisModemStatus(&ulDocsisModemStatus))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisModemStatus ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisModemStatus);
	*retval = ulDocsisModemStatus;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisDownFrequency(OOIFNumber *retval)
{
	HUINT32	ulDocsisDownFrequency = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisDownFrequency(&ulDocsisDownFrequency))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisDownFrequency ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisDownFrequency);
	*retval = ulDocsisDownFrequency;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisQamType(const char **retval)
{
	static HCHAR szDocsisQamType[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getDocsisQamType(szDocsisQamType, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisQamType ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = szDocsisQamType;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisDownSignalLevel(OOIFNumber *retval)
{
	HUINT32	ulDocsisDownSignalLevel = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisDownSignalLevel(&ulDocsisDownSignalLevel))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisDownSignalLevel ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisDownSignalLevel);
	*retval = ulDocsisDownSignalLevel;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetdDocsisDownBer(OOIFNumber *retval)
{
	HUINT32	ulDocsisDownBer = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisDownBer(&ulDocsisDownBer))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisDownBer ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisDownBer);
	*retval = ulDocsisDownBer;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisDownSnr(OOIFNumber *retval)
{
	HUINT32	ulDocsisDownSnr = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisDownSnr(&ulDocsisDownSnr))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisDownSnr ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisDownSnr);
	*retval = ulDocsisDownSnr;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisUpFrequency(OOIFNumber *retval)
{
	HUINT32	ulDocsisUpFrequency = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisUpFrequency(&ulDocsisUpFrequency))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisUpFrequency ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisUpFrequency);
	*retval = ulDocsisUpFrequency;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisUpChannelId(OOIFNumber *retval)
{
	HUINT32	ulDocsisUpChannelId = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisDocsisUpChannelId(&ulDocsisUpChannelId))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisDocsisUpChannelId ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisUpChannelId);
	*retval = ulDocsisUpChannelId;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisUpBandWidth(OOIFNumber *retval)
{
	HUINT32	ulDocsisUpBandWidth = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisUpBandWidth(&ulDocsisUpBandWidth))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisUpBandWidth ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisUpBandWidth);
	*retval = ulDocsisUpBandWidth;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDocsisUpSignalLevel(OOIFNumber *retval)
{
	HUINT32	ulDocsisUpSignalLevel = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDocsisUpSignalLevel(&ulDocsisUpSignalLevel))
	{
		OOIF_LOG_ERROR("[%s:%d] getDocsisUpSignalLevel ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulDocsisUpSignalLevel);
	*retval = ulDocsisUpSignalLevel;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoSetDocsisDownFrequency(OOIFNumber downFrequency, OOIFBoolean *pbSuccess)
{
	HUINT32	ulDocsisDownFrequency = 0;
	HBOOL	bSuccess = FALSE;
	if(pbSuccess == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;
	ulDocsisDownFrequency = (HUINT32)downFrequency;
	OOIF_LOG_DUMMY("[%s] DownFrequency : %d\n",__FUNCTION__, ulDocsisDownFrequency);
	if(!OplJlabsSystemInfo::setDocsisDownFrequency(ulDocsisDownFrequency, &bSuccess))
	{
		OOIF_LOG_DUMMY("[%s] setDocsisDownFrequency returned error!! bSuccess %d\n",__FUNCTION__,bSuccess);
		*pbSuccess = FALSE;
	}
	else
	{
		OOIF_LOG_DUMMY("[%s] setDocsisDownFrequency returned ok!! bSuccess %d\n",__FUNCTION__,bSuccess);
		*pbSuccess = (OOIFBoolean)bSuccess;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetCardIdBCAS(const char **retval)
{
	static HCHAR szCardId[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetCardIdBCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getCardIdBCAS(szCardId, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getCardIdBCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szCardId);

	*retval = szCardId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDiscernBCAS(const char **retval)
{
	static HCHAR szDiscernBCAS[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetDiscernBCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getDiscernBCAS(szDiscernBCAS, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDiscernBCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szDiscernBCAS);

	*retval = szDiscernBCAS;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetGroupIdBCAS(int *pNumOfGroupId, char ***retval)
{
	static HCHAR aszGroupId[OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID][MAX_BUF_LEN];
	static HCHAR *acGroupId[OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID];
	HUINT32 numOfId = 0, i =0;

	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetGroupIdBCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	for(i = 0; i<OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID; i++)
	{
		HxSTD_MemSet(aszGroupId[i], 0x00, MAX_BUF_LEN);
		acGroupId[i] = aszGroupId[i];
	}
	if (!OplJlabsSystemInfo::getGroupIdBCAS(acGroupId, &numOfId, MAX_BUF_LEN, OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID))
	{
		OOIF_LOG_ERROR("[%s:%d] getGroupIdBCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (char**)acGroupId;
	*pNumOfGroupId = (int)numOfId;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsSystemInfoGetCardIdCCAS(const char **retval)
{
	static HCHAR szCardId[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetCardIdCCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getCardIdCCAS(szCardId, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getCardIdCCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szCardId);

	*retval = szCardId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDiscernCCAS(const char **retval)
{
	static HCHAR szDiscernCCAS[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetDiscernCCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getDiscernCCAS(szDiscernCCAS, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDiscernCCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szDiscernCCAS);

	*retval = szDiscernCCAS;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetGroupIdCCAS(int *pNumOfGroupId, char ***retval)
{
	static HCHAR aszGroupId[OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID][MAX_BUF_LEN];
	static HCHAR *acGroupId[OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID];
	HUINT32 numOfId = 0, i =0;

	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetGroupIdCCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	for(i = 0; i<OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID; i++)
	{
		HxSTD_MemSet(aszGroupId[i], 0x00, MAX_BUF_LEN);
		acGroupId[i] = aszGroupId[i];
	}
	if (!OplJlabsSystemInfo::getGroupIdCCAS(acGroupId, &numOfId, MAX_BUF_LEN, OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID))
	{
		OOIF_LOG_ERROR("[%s:%d] getGroupIdCCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (char**)acGroupId;
	*pNumOfGroupId = (int)numOfId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDeviceId(const char **retval)
{
	static HCHAR szDeviceId[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsSystemInfo::getDeviceId(szDeviceId, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDeviceId ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szDeviceId);

	*retval = szDeviceId;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetDeviceModel(const char **retval)
{
	static HCHAR szModelName[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDeviceModel(szModelName, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDeviceModel ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szModelName);
	*retval = szModelName;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsSystemInfoGetDeviceSerialNumber(const char **retval)
{
	static HCHAR szSerialNum[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDeviceSerialNumber(szSerialNum, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDeviceSerialNumber ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szSerialNum);

	*retval = szSerialNum;

	return OOIF_RETURN_OK;
}

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceSoftwareVersion(const char **retval)
{
	static HCHAR szSWVersion[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDeviceSoftwareVersion(szSWVersion, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDeviceSoftwareVersion ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szSWVersion);

	*retval = szSWVersion;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsSystemInfoGetDeviceHardwareVersion(const char **retval)
{
	static HCHAR szHWVersion[MAX_BUF_LEN];

	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getDeviceHardwareVersion(szHWVersion, MAX_BUF_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getDeviceHardwareVersion ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%s\n",__PRETTY_FUNCTION__,__LINE__,szHWVersion);
	*retval = szHWVersion;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetSystemUpdateDate(OOIFNumber *retval)
{
	HUINT32	ulUpdateDate = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	if (!OplJlabsSystemInfo::getSystemUpdateDate(&ulUpdateDate))
	{
		OOIF_LOG_ERROR("[%s:%d] getSystemUpdateDate ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,ulUpdateDate);
	*retval = ulUpdateDate;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetCardStatusBCAS(OOIFBoolean*retval)
{
	HBOOL	bIsBCasOK = FALSE;
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetCardStatusBCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getCardStatusBCAS(&bIsBCasOK))
	{
		OOIF_LOG_ERROR("[%s:%d] getCardStatusBCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsBCasOK);

	*retval = bIsBCasOK;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoGetCardStatusCCAS(OOIFBoolean *retval)
{
	HBOOL	bIsCCasOK = FALSE;
	OOIF_LOG_DUMMY("native_jlabsSystemInfoGetCardStatusCCAS");
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;
	if (!OplJlabsSystemInfo::getCardStatusCCAS(&bIsCCasOK))
	{
		OOIF_LOG_ERROR("[%s:%d] getCardStatusCCAS ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	OOIF_LOG_INFO("[%s:%d] value:%d\n",__PRETTY_FUNCTION__,__LINE__,bIsCCasOK);

	*retval = bIsCCasOK;
	return OOIF_RETURN_OK;
}

// String get(String key)
OOIFReturnCode native_jlabsSystemInfoGet(const char *name, const char **retval)
{
	static HCHAR 		szGet[MAX_BUF_LEN];
	static HCHAR 		szGroupId[(OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID*OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID) + 1];
	HUINT32			ulGet = 0;
	HBOOL			bGet = FALSE;
	eOOIF_ReturnType	eRetType = eReturnType_Max;
	OOIFReturnCode 	ret = OOIF_RETURN_ERROR;
	HCHAR 			**acGroupId = NULL;
	HINT32 			numOfId = 0;
	HINT32 			i = 0;

	if(name == NULL || retval == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szGet, 0x00, MAX_BUF_LEN);
	if (!HxSTD_StrCmp(name,(const char *)"docsisModemStatus"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisModemStatus, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisDownFrequency"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisDownFrequency, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisQamType"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDocsisQamType, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisDownSignalLevel"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisDownSignalLevel, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisDownBer"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisDownBer, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisDownSnr"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisDownSnr, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisUpFrequency"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisUpFrequency, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisUpChannelId"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisDocsisUpChannelId, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisUpBandWidth"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisUpBandWidth, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"docsisUpSignalLevel"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getDocsisUpSignalLevel, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"cardIdBCAS"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getCardIdBCAS, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"discernBcas"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDiscernBCAS, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"groupIdBcas"))
	{
		if((ret = native_jlabsSystemInfoGetGroupIdBCAS(&numOfId, &acGroupId)) != OOIF_RETURN_OK)
		{
			return ret;
		}
		HxSTD_MemSet(szGroupId, 0x00, (OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID*OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID) + 1);
		if((numOfId >0) && (numOfId <=OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID))
		{
			for(i =0 ; i < numOfId ; i++)
			{
				HxSTD_StrNCat(szGroupId, acGroupId[i], (OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID));
			}
		}
		*retval  = szGroupId;
		return OOIF_RETURN_OK;
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"cardIdCCAS"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getCardIdCCAS, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"discernCcas"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDiscernCCAS, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"groupIdCcas"))
	{
		if((ret = native_jlabsSystemInfoGetGroupIdCCAS(&numOfId, &acGroupId)) != OOIF_RETURN_OK)
		{
			return ret;
		}
		HxSTD_MemSet(szGroupId, 0x00, (OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID*OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID) + 1);
		if((numOfId >0) && (numOfId <=OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID))
		{
			for(i =0 ; i < numOfId ; i++)
			{
				HxSTD_StrNCat(szGroupId, acGroupId[i], (OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID));
			}
		}
		*retval  = szGroupId;
		return OOIF_RETURN_OK;
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"deviceId"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDeviceId, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"deviceModel"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDeviceModel, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"deviceSerialNumber"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDeviceSerialNumber, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"deviceSoftwareVersion"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDeviceSoftwareVersion, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"deviceHardwareVersion"))
	{
		__GET_SYS_STRING_PROPERTY(OplJlabsSystemInfo::getDeviceHardwareVersion, szGet, MAX_BUF_LEN);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"systemUpdateDate"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getSystemUpdateDate, &ulGet, eReturnType_Number);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"cardStatusBCAS"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getCardStatusBCAS, &bGet, eReturnType_Boolean);
	}
	else	if (!HxSTD_StrCmp(name,(const char *)"cardStatusCCAS"))
	{
		__GET_SYS_NUMBER_PROPERTY(OplJlabsSystemInfo::getCardStatusCCAS, &bGet, eReturnType_Boolean);
	}

	if(eRetType == eReturnType_Number)
	{
		HxSTD_snprintf(szGet, MAX_BUF_LEN-1, "%d", ulGet);
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
		*retval  = szGet;
	}
	else
	{
		*retval = "unDefined";
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsSystemInfoStartSettingInfo(OOIFNumber settingInfoOpt, OOIFBoolean *pbSuccess)
{
	HUINT32	ulSettingInfoOpt = 0;
	HBOOL	bSuccess = FALSE;
	OOIF_LOG_DUMMY("native_jlabsSystemInfoStartSettingInfo");
	if(pbSuccess == NULL)
		return OOIF_RETURN_VALUE_UNDEFINED;
	ulSettingInfoOpt = (HUINT32)settingInfoOpt;
	OOIF_LOG_DUMMY("[%s] settingInfoOpt %d\n",__FUNCTION__,settingInfoOpt);
	if(!OplJlabsSystemInfo::startSettingInfo(ulSettingInfoOpt, &bSuccess))
	{
		OOIF_LOG_DUMMY("[%s] startSettingInfo returned error!! bSuccess %d\n",__FUNCTION__,bSuccess);
		*pbSuccess = FALSE;
	}
	else
	{
		OOIF_LOG_DUMMY("[%s] startSettingInfo returned ok!! bSuccess %d\n",__FUNCTION__,bSuccess);
		*pbSuccess = (OOIFBoolean)bSuccess;
	}
	return OOIF_RETURN_OK;
}


#endif // #if defined OIPF && defined JLABS
