// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeCapabilities.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OOIFReturnCode native_CapabilityHasCapability(const char *capability, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Request for capability: %s", capability);

	if (!strcmp("+PVR", capability))
		*retval = 1;
	else
		*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_CapabilityGetXmlCapabilities(const char **retval)
{
	*retval = "<?xml version=\"1.0\"?> <profilelist> <ui_profile name=\"OITF_SDUS_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_SD60_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_SDEU_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_HD_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_FULL_HD_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> </profilelist>";

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_CapabilityGetExtraSDVideoDecodes(OOIFNumber *retval)
{
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_CapabilityGetExtraHDVideoDecodes(OOIFNumber *retval)
{
	*retval = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ObjectFactoryIsObjectSupported(const char *mimeType, OOIFBoolean *retval)
{
	if (strcmp("application/oipfApplicationManager", mimeType) == 0 ||
	        strcmp("application/oipfConfiguration", mimeType) == 0 ||
	        strcmp("application/oipfCapabilities", mimeType) == 0 ||
	        strcmp("application/oipfDrmAgent", mimeType) == 0 ||
	        strcmp("application/oipfParentalControlManager", mimeType) == 0 ||
	        strcmp("application/oipfDownloadTrigger", mimeType) == 0 ||
	        strcmp("application/oipfDownloadManager", mimeType) == 0 ||
	        strcmp("application/oipfRecordingScheduler", mimeType) == 0 ||
	        strcmp("application/oipfSearchManager", mimeType) == 0 ||
	        strcmp("video/broadcast", mimeType) == 0 ||
	        strcmp("video/mpeg", mimeType) == 0 ||
	        strcmp("video/mp4", mimeType) == 0 ||
	        strcmp("video/mpeg4", mimeType) == 0 ||
	        strcmp("video/x-ms-wmv", mimeType) == 0 ||
	        strcmp("video/x-ms-asf", mimeType) == 0 ||
#if defined(HMX_WEBUI)
	        strcmp("application/mediaSearchManager", mimeType) == 0 ||
	        strcmp("application/humaxConfiguration", mimeType) == 0 ||
	        strcmp("application/dlnaManager", mimeType) == 0 ||
	        strcmp("application/woonManager", mimeType) == 0 ||
	        strcmp("application/sambaManager",mimeType) == 0 ||
	        strcmp("application/contentManager", mimeType) == 0 ||
	        strcmp("application/networkServerConfigManager", mimeType) == 0 ||
	        strcmp("application/airplayManager", mimeType) == 0 ||
#ifdef USE_VIDEOHANDLE
			strcmp("video/vnd.humax.broadcast", mimeType) == 0 ||
			strcmp("video/vnd.humax.mpeg", mimeType) == 0 ||
#endif
#endif
#ifdef JLABS
			strcmp("application/jlabsEmmMessageManager", mimeType) == 0 ||
			strcmp("application/jlabsCDSSearcher", mimeType) == 0 ||
#endif
#ifdef RFVOD
	        strcmp("rfvideo/mpeg", mimeType) ||
#endif
	        strcmp("video/H264", mimeType) == 0
	   )
		*retval = TRUE;
	else
		*retval = FALSE;
	return OOIF_RETURN_OK;
}
