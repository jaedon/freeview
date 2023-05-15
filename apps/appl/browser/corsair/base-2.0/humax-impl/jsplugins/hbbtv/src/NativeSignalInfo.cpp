// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
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
OOIFReturnCode native_SignalInfoGetStrength(SignalInfoHandle identifier, OOIFNumber *retval)
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
OOIFReturnCode native_SignalInfoGetQuality(SignalInfoHandle identifier, OOIFNumber *retval)
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
OOIFReturnCode native_SignalInfoGetBer(SignalInfoHandle identifier, OOIFNumber *retval)
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
OOIFReturnCode native_SignalInfoGetSnr(SignalInfoHandle identifier, OOIFNumber *retval)
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

//ro Boolean lock
OOIFReturnCode native_SignalInfoGetLock(SignalInfoHandle identifier, OOIFBoolean *retval)
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

#endif // OIPF
