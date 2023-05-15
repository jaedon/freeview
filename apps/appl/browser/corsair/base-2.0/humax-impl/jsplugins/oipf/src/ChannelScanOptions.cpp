// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include "ChannelScanOptions.h"

#include "jsplugin.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined JLABS
#include "NativeJlabsLocalSystem.h"
#include "NativeJlabsScanning.h"
#endif
using namespace Ooif;

ChannelScanOptions::ChannelScanOptions()
{
	ENTRY;
	classType = class_name;
	memset(&m_options, 0, sizeof(ScanOptions));
}

#if defined JLABS
ChannelScanOptions::ChannelScanOptions(jlabsScanningHandle handle)
{
	ENTRY;

	stNativeJlabsLocalSystem_ScanSetting_t 	*pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)handle;
	m_options.channelType = pstSetting->nChannelType;
	m_options.replaceExisting = pstSetting->bReplaceExisting;
	classType = class_name;
}
#endif

int ChannelScanOptions::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("channelType", m_options.channelType, JSP_GET_VALUE);
	GET_BOOLEAN("replaceExisting", m_options.replaceExisting, JSP_GET_VALUE);
#if defined(HMX_WEBUI)
	GET_NUMBER ("casType",				m_options.casType,				JSP_GET_VALUE);
	GET_BOOLEAN("enableNetworkScan",	m_options.enableNetworkScan,		JSP_GET_VALUE);
#endif
	return JSP_GET_NOTFOUND;
}

int ChannelScanOptions::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	if(!strcmp(name, "channelType"))
	{
		 m_options.channelType = NUMBER_VALUE(value[0]);
		 return JSP_PUT_OK;
	}
	if(!strcmp(name, "replaceExisting"))
	{
		m_options.replaceExisting = BOOLEAN_VALUE(value[0]);
		return JSP_PUT_OK;
	}
#if defined(HMX_WEBUI)
	if(!strcmp(name, "casType"))
	{
		m_options.casType = NUMBER_VALUE(value[0]);
		return JSP_PUT_OK;
	}
	if(!strcmp(name, "enableNetworkScan"))
	{
		m_options.enableNetworkScan = BOOLEAN_VALUE(value[0]);
		return JSP_PUT_OK;
	}
#endif
	return JSP_PUT_NOTFOUND;
}

const ScanOptions* ChannelScanOptions::getOpts()
{
	return &m_options;
}


#endif // OIPF
