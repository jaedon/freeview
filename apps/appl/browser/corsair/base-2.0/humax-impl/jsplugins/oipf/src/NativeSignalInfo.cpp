// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#include "NativeSignalInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include "NativeHConfiguration.h"

#include "macros.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"
#include "oplsignalinfo.h"

// Currently Not Used.
// TODO: We have to conclude, it use or not.
OOIFReturnCode native_SignalInfoReleaseHandle(SignalInfoHandle id)
{
	//Dummy Implementation TODO
	OOIF_LOG_DEBUG("SignalInfoReleaseHandle");
	return OOIF_RETURN_OK;
}

//ro Number strength
#ifdef HMX_WEBUI
OOIFReturnCode native_HSignalInfoGetStrength(SignalInfoHandle identifier, OOIFNumber *retval)
#else
OOIFReturnCode native_SignalInfoGetStrength(SignalInfoHandle identifier, OOIFNumber *retval)
#endif
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int		strength;

	if (!OplSignalInfo::getSignalInfoStrength((unsigned int)identifier, &strength))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoStrength ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = strength;

	return OOIF_RETURN_OK;
}

//ro Integer quality
#ifdef HMX_WEBUI
OOIFReturnCode	native_HSignalInfoGetQuality(SignalInfoHandle identifier, OOIFNumber *retval)
#else
OOIFReturnCode native_SignalInfoGetQuality(SignalInfoHandle identifier, OOIFNumber *retval)
#endif
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int		quality;

	if (!OplSignalInfo::getSignalInfoQuality((unsigned int)identifier, &quality))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoQuality ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = quality;

	return OOIF_RETURN_OK;
}

//ro Integer ber
#ifdef HMX_WEBUI
OOIFReturnCode	native_HSignalInfoGetBer(SignalInfoHandle identifier, OOIFNumber *retval)
#else
OOIFReturnCode native_SignalInfoGetBer(SignalInfoHandle identifier, OOIFNumber *retval)
#endif
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	float		ber;

	if (!OplSignalInfo::getSignalInfoBer((unsigned int)identifier, &ber))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoBer ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = ber;

	return OOIF_RETURN_OK;
}

//ro Number snr
#ifdef HMX_WEBUI
OOIFReturnCode	native_HSignalInfoGetSnr(SignalInfoHandle identifier, OOIFNumber *retval)
#else
OOIFReturnCode native_SignalInfoGetSnr(SignalInfoHandle identifier, OOIFNumber *retval)
#endif
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	float		snr;

	if (!OplSignalInfo::getSignalInfoSnr((unsigned int)identifier, &snr))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoSnr ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = snr;

	return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode	native_HSignalInfoGetAgc(SignalInfoHandle identifier, OOIFNumber *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	unsigned int	agc;

	if (!OplSignalInfo::getSignalInfoAgc((unsigned int)identifier, &agc))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoAgc ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}
	*retval = static_cast<OOIFNumber>(agc);

	return OOIF_RETURN_OK;
}
#endif

//ro Boolean lock
#ifdef HMX_WEBUI
OOIFReturnCode	native_HSignalInfoGetLock(SignalInfoHandle identifier, OOIFBoolean *retval)
#else
OOIFReturnCode native_SignalInfoGetLock(SignalInfoHandle identifier, OOIFBoolean *retval)
#endif
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	bool		locked;

	if (!OplSignalInfo::getSignalInfoLocked((unsigned int)identifier, &locked))
	{
		OOIF_LOG_ERROR("[%s:%d] getSignalInfoLocked ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = locked;

	return OOIF_RETURN_OK;
}

#if defined(HMX_WEBUI)
//ro String
#define MAX_TUNEDPARAM_LEN	256

OOIFReturnCode	native_HSignalInfoGetTunedParamStr(SignalInfoHandle identifier, const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szTunedParam[MAX_TUNEDPARAM_LEN];

	HxSTD_MemSet(szTunedParam,0,MAX_TUNEDPARAM_LEN);

	if (!OplSignalInfo::getTunedParamStr((unsigned int)identifier,szTunedParam,MAX_TUNEDPARAM_LEN))
	{
		OOIF_LOG_ERROR("[%s:%d] getTunedParamStr ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = szTunedParam;

	return OOIF_RETURN_OK;
}
#endif // HMX_WEBUI

#endif // OIPF
