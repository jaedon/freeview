// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#include "NativeTuner.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>

#include "NativeHConfiguration.h"


#include "opllocalsystem.h"
#include "oplnetwork.h"
#include "oplsignalinfo.h"

// Currently Not Used.
// TODO: We have to conclude, it use or not.
OOIFReturnCode native_TunerInitialize(TunerHandle identifier, TunerData *data)
{
	OOIF_LOG_DEBUG("Initializing Tuner with handle='%ld'", reinterpret_cast<long>(identifier));

	data->id = 1;
	data->name = HLIB_STD_StrDup("Tuner Name");

	return OOIF_RETURN_OK;
}

// Currently Not Used.
// TODO: We have to conclude, it use or not.
OOIFReturnCode native_TunerReleaseHandle(TunerHandle id)
{
	OOIF_LOG_DEBUG("TunerReleaseHandle");
	return OOIF_RETURN_OK;
}

// Boolean enableTuner
OOIFReturnCode native_TunerGetEnableTuner(TunerHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplTuner::isTunerEnabled((unsigned int)identifier);

	return OOIF_RETURN_OK;
}

// Boolean powerOnExternal
OOIFReturnCode native_TunerGetPowerOnExternal(TunerHandle identifier, OOIFBoolean *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	*retval = OplTuner::isExternalPower((unsigned int)identifier);

	return OOIF_RETURN_OK;
}

//ro SignalInfo signalInfo
OOIFReturnCode native_TunerGetSignalInfo(TunerHandle identifier, SignalInfoHandle *retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	int		tuner_id;

	*retval = 0;

	if (!OplTuner::getTunerId((unsigned int)identifier, &tuner_id))
	{
		OOIF_LOG_ERROR("[%s:%d] getTunerId ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = (SignalInfoHandle)tuner_id;

	return OOIF_RETURN_OK;
}

//ro StringCollection idTypes
OOIFReturnCode native_TunerGetIdTypes(TunerHandle identifier, int *count, char ***types)
{
	if (NULL == strings)
		return OOIF_RETURN_VALUE_UNDEFINED;

	// static buffer used in getIdTypes() for strings.
	if (!OplTuner::getIdTypes((unsigned int)identifier, count, strings))
	{
		OOIF_LOG_ERROR("[%s:%d] getIdTypes ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_TunerSetEnableTuner(TunerHandle identifier, OOIFBoolean value)
{
	OOIF_LOG_DEBUG("Tuner set enableTuner: %d", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_TunerSetPowerOnExternal(TunerHandle identifier, OOIFBoolean value)
{
	OOIF_LOG_DEBUG("Tuner set powerOnExternal: %d ", value);
	return OOIF_RETURN_OK;
}
#endif // OIPF