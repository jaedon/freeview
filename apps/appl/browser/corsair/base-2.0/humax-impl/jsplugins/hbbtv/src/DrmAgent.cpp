// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "DrmAgent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Exception.h"
#include "NativeDrmAgent.h"
#include "DRMMessageResultEvent.h"
#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "DRMSystemMessageEvent.h"
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
#include "DRMSystemStatusChangeEvent.h"
#endif // OIPF

#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#if !defined(CONFIG_PROD_ICORDPRO) && !defined(CONFIG_PROD_HGS1000S)		// Apps produced by RTL don't satisfy HbbTV Security. iCordPRO model is blocked by Chang-goo Kang.
#define	CHECK_HBBTV_SECURITY		1
#endif

using namespace Ooif;
//static
list<DrmAgent*> DrmAgent::instances;

DrmAgent::DrmAgent(DrmAgentHandle i) : WrapperObject<DrmAgent>(i)
{
	ENTRY;
	classType = CLASS_DRMAGENT;
	DrmAgent::instances.push_back(this);
}

DrmAgent::~DrmAgent()
{
	ENTRY;
	DrmAgent::instances.remove(this);
}

int DrmAgent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef CHECK_HBBTV_SECURITY
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
#endif

	INTRINSIC_EVENT_GET(DRMMessageResult);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	INTRINSIC_EVENT_GET(DRMSystemMessage);
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
	INTRINSIC_EVENT_GET(DRMSystemStatusChange);
	GET_FUNCTION(DRMSystemStatus, obj, "s", JSP_GET_VALUE_CACHE);
#endif // OIPF
	GET_FUNCTION(sendDRMMessage, obj, "sss", JSP_GET_VALUE_CACHE);

	return EventTarget::getter(obj, name, result);
}

int DrmAgent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef CHECK_HBBTV_SECURITY
#ifdef HBBTV_SECURITY
	SET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
#endif

	INTRINSIC_EVENT(DRMMessageResult);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	INTRINSIC_EVENT(DRMSystemMessage);
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
	INTRINSIC_EVENT(DRMSystemStatusChange);
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

int DrmAgent::sendDRMMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef CHECK_HBBTV_SECURITY
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
#endif

	// Verify calling arguments
	CHECK_ARGUMENT_COUNT(3);

	// Call native DRM agent API
	DrmAgent *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, DrmAgent);
	const char *msgID;
	NATIVE_CALL(native_DrmAgentSendDRMMessage(instance->getIdentifier(),
	            STRING_VALUE(argv[0]),
	            STRING_VALUE(argv[1]),
	            STRING_VALUE(argv[2]),
	            &msgID));
	RETURN_STRING(msgID, JSP_CALL_VALUE);
}

#ifdef OIPF
//static
int DrmAgent::DRMSystemStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	// Verify calling arguments
	CHECK_ARGUMENT_COUNT(1);

	// Call native DRM agent API
	DrmAgent *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, DrmAgent);
	DrmSystemStatus res;
	NATIVE_CALL(native_DrmAgentDRMSystemStatus(instance->getIdentifier(),
	            STRING_VALUE(argv[0]),
	            &res));
	RETURN_NUMBER((OOIFNumber)res, JSP_CALL_VALUE);
}
#endif // OIPF

DrmAgent* DrmAgent::GetInstance(DrmAgentHandle id)
{
	for (list<DrmAgent*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		if ((*it)->getIdentifier() == id)
			return *it;
	}
	return NULL;
}

void DrmAgent::onDrmAgentEvent(DrmEvent event, DrmEventInfo *info, DrmAgentHandle id)
{
	DrmAgent* drmAgent = GetInstance(id);
	if (!drmAgent)
	{
		OOIF_LOG_WARNING("Unknown identifier received");
		return;
	}

	Event *e = NULL;
	switch (event)
	{
	case DRM_EVENT_DRM_MESSAGE_RESULT:
		OOIF_LOG_DEBUG("onDRMMessageResult event received");
		e = new DRMMessageResultEvent(drmAgent->getHost(),
		                              info->drm_message_result_data.msgID,
		                              info->drm_message_result_data.resultMsg,
		                              info->drm_message_result_data.resultCode);
		break;
#ifdef OIPF
	case DRM_EVENT_DRM_SYSTEM_STATUS_CHANGE:
		OOIF_LOG_DEBUG("onDRMSystemStatusChange");
		e = new DRMSystemStatusChangeEvent(drmAgent->getHost(),
		                                   info->drm_system_status_change.DRMSystemID);
		break;
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	case DRM_EVENT_DRM_SYSTEM_MESSAGE:
		OOIF_LOG_DEBUG("onDRMSystemMessage");
		e = new DRMSystemMessageEvent(drmAgent->getHost(),
		                              info->drm_system_message.msg,
		                              info->drm_system_message.DRMSystemID);
		break;
#endif // OIPF || HBBTV_1_2_1
	default:
		OOIF_LOG_WARNING("Unknown event received");
		return;
	}
	drmAgent->dispatchEvent(e);
}

void DrmAgent::releaseNative(DrmAgentHandle id)
{
	NATIVE_WARN_NOK(native_DrmAgentReleaseHandle(id));
}

OOIFReturnCode DrmAgent::checkListenerAccessAllowed(const char *eventName)
{
#ifdef CHECK_HBBTV_SECURITY
#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
#endif

	return native_DrmAgentOnListenerAccessed(identifier, eventName);
}
