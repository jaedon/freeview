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
	*retval = "<?xml version=\"1.0\"?> <profilelist> <ui_profile name=\"DVB_T_FVC_P1\"> <ext> <drm DRMSystemID=\"urn:dvb:casystemid:19219\">TS_PF</drm><recording manageRecordings=\"samedomain\">true</recording> </ext> </ui_profile> </profilelist>";

//	*retval = "<?xml version='1.0' ?><root><level1><option>Option1</option><option>Option2</option><option>Option3</option></level1></root>";
//	*retval = "<?xml version=\"1.0\"?> <profilelist> <ui_profile name=\"OITF_SDUS_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_SD60_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_SDEU_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_HD_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> <ui_profile name=\"OITF_FULL_HD_UIPROF+PVR\"> <ext> <recording>true</recording> </ext> </ui_profile> </profilelist>";
return OOIF_RETURN_OK;
}

OOIFReturnCode native_CapabilityGetExtraSDVideoDecodes(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_CapabilityGetExtraHDVideoDecodes(OOIFNumber *retval)
{
	*retval = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ObjectFactoryIsObjectSupported(const char *mimeType, OOIFBoolean *retval)
{
	if (strcmp("application/oipfApplicationManager", mimeType) == 0 ||
	        strcmp("application/oipfConfiguration", mimeType) == 0 ||
	        strcmp("application/oipfCapabilities", mimeType) == 0 ||
	        strcmp("application/oipfParentalControlManager", mimeType) == 0 ||
	        strcmp("application/oipfDrmAgent", mimeType) == 0 ||
	        strcmp("application/oipfParentalControlManager", mimeType) == 0 ||
#ifndef CONFIG_PROD_FVP4000T
	        strcmp("application/oipfDownloadTrigger", mimeType) == 0 ||
	        strcmp("application/oipfDownloadManager", mimeType) == 0 ||
#endif
	        strcmp("application/oipfRecordingScheduler", mimeType) == 0 ||
	        strcmp("application/oipfSearchManager", mimeType) == 0 ||
	        strcmp("video/broadcast", mimeType) == 0 ||
	        strcmp("video/mpeg", mimeType) == 0 ||
	        strcmp("video/mp4", mimeType) == 0 ||
	        strcmp("video/mpeg4", mimeType) == 0 ||
	        strcmp("video/x-ms-wmv", mimeType) == 0 ||
	        strcmp("video/x-ms-asf", mimeType) == 0 ||
	        strcmp("audio/mpeg", mimeType) == 0 ||
	        strcmp("audio/mp4", mimeType) == 0 ||

#ifdef USE_VIDEOHANDLE
			strcmp("video/vnd.humax.broadcast", mimeType) == 0 ||
			strcmp("video/vnd.humax.mpeg", mimeType) == 0 ||

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
