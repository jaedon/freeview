// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "DRMControlInformation.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

using namespace Ooif;

void DRMControlInformationDataWrapper::init(void *id)
{
	memset(&data, 0, sizeof(DRMControlInformationData));
	NATIVE_WARN_NOK(native_DRMControlInformationInitialize(id, &data));
}

DRMControlInformationDataWrapper::~DRMControlInformationDataWrapper()
{
	DATA_FREE_STRING(drmType);
	DATA_FREE_STRING(drmContentID);
	DATA_FREE_STRING(rightsIssuerURL);
	DATA_FREE_STRING(silentRightsURL);
	DATA_FREE_STRING(previewRightsURL);
	DATA_FREE_STRING(drmPrivateData);
}

void DRMControlInformation::init()
{
}

DRMControlInformation::DRMControlInformation(DRMControlInformationHandle i)
	: DataObject<DRMControlInformation, DRMControlInformationDataWrapper>(i)
{
	init();
}

DRMControlInformation::DRMControlInformation(const DRMControlInformation &original)
	: DataObject<DRMControlInformation, DRMControlInformationDataWrapper>(original)
{
	init();
}

DRMControlInformation::~DRMControlInformation()
{
}

int DRMControlInformation::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	DATA_GET_STRING(drmType);
	DATA_GET_STRING(drmContentID);
	DATA_GET_STRING(rightsIssuerURL);
	DATA_GET_STRING(silentRightsURL);
	DATA_GET_STRING(drmContentID);
	DATA_GET_STRING(previewRightsURL);
	DATA_GET_STRING(drmPrivateData);

	DATA_GET_BOOLEAN(doNotRecord);
	DATA_GET_BOOLEAN(doNotTimeShift);

	return JSP_GET_NOTFOUND;
}

void DRMControlInformation::releaseNative(DRMControlInformationHandle id)
{
	NATIVE_WARN_NOK(native_DRMControlInformationReleaseHandle(id));
}

#endif // HBBTV_DOWNLOAD || OIPF
