// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdlib.h>
#include <string.h>

#include "opldrmagent.h"
#include "NativeDrmAgent.h"
#include "DrmAgent.h"
#include "macros.h"

#include <fcntl.h>
#include <unistd.h>

using namespace Ooif;

#define MAX_BUF_LEN 64

static drmListener *g_DrmAgentListener = NULL;

static unsigned short __urandom(void)
{
	int fd = open("/dev/urandom", O_RDONLY);
	unsigned short r = 0;
	ssize_t ret = -1;

	if (fd < 0) {
		OOIF_LOG_ERROR("[OPL_DRMAGENT][%s] Error> could not open /dev/urandom\n", __FUNCTION__);
		return 0;
	}

	ret = read(fd, &r, sizeof (r));
	if (ret < 0) {
		OOIF_LOG_ERROR("[OPL_DRMAGENT][%s] Error> could not read /dev/urandom\n", __FUNCTION__);
	}

	if (fd >= 0) close(fd);

	return (unsigned short)r;
}

static void _drmagent_EmitEvent (DrmEvent event, DrmEventInfo *info, DrmAgentHandle id)
{
	OOIF_LOG_DEBUG("+ [%s : %d]\n",__FUNCTION__,__LINE__);
	if (g_DrmAgentListener)
	{
		g_DrmAgentListener(event, info, id);
	}
	OOIF_LOG_DEBUG("- [%s : %d]\n",__FUNCTION__,__LINE__);
}

static void _drmagent_onDRMStatusEvent(void *hDrmAgent,const char *pzDrmSystemId)
{
}
static void _drmagent_onDrmResultEvent(void *hDrmAgent, unsigned short nMsgId, const char *pzResultMsg, 	unsigned int ulResultCode)
{
	ENTRY;

	DrmAgentHandle		id = (DrmAgentHandle)hDrmAgent;
	OplDrmAgent			*drmagentObj = static_cast<OplDrmAgent *>(id);

	DrmEventInfo			info;


	if(drmagentObj == NULL)
	{
		OOIF_LOG_ERROR("drmagentObj  is NULL!!!!!!! [%s : %d]\n",__FUNCTION__,__LINE__);
		return;
	}
	memset(&info, 0x00, sizeof(DrmEventInfo));



	//snprintf(szMsgId, MAX_BUF_LEN, "drmAgent:msgId:%d", msgId);
	const int szLen = 16*4 ; // HUINT32 * char + "drmAgent:msgId:%d" + 1
	HCHAR	* szMsgId = new char[szLen];
//		snprintf(szMsgId, szLen, "drmAgent:msgId:%d", retMsgId);
	snprintf(szMsgId, szLen, "%d", nMsgId);

	info.drm_message_result_data.msgID = szMsgId;
	info.drm_message_result_data.resultCode = ulResultCode;
	info.drm_message_result_data.resultMsg = pzResultMsg;
	OOIF_LOG_DEBUG("info.drm_message_result_data.msgID %s\n",info.drm_message_result_data.msgID);
	OOIF_LOG_DEBUG("info.drm_message_result_data.resultCode %d\n",info.drm_message_result_data.resultCode);
	OOIF_LOG_DEBUG("info.drm_message_result_data.resultMsg %s\n",info.drm_message_result_data.resultMsg);

	_drmagent_EmitEvent(DRM_EVENT_DRM_MESSAGE_RESULT, &info, id);

	return;
}

OOIFReturnCode native_DrmAgentSetListener(drmListener listener)
{
	if(g_DrmAgentListener != NULL)
	{
		OOIF_LOG_WARNING("Listener has already been registered\n");
	}

	g_DrmAgentListener = listener;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DrmAgentCreate(DrmAgentHandle *id, long int window_identifier, const char * host_uri)
{
	OOIF_LOG_DEBUG(" + [%s : %d]\n",__FUNCTION__,__LINE__);

	OplDrmAgent *drmagentObj = OplDrmAgentManager::getInstance().CreateDRMAgent();

	if ( !drmagentObj )
	{
		OOIF_LOG_ERROR(" - error!!! [%s : %d]\n",__FUNCTION__,__LINE__);
		return OOIF_RETURN_VALUE_NULL;
	}

	*id = drmagentObj;
	drmagentObj->setDrmAgentResultEventListener(_drmagent_onDrmResultEvent, _drmagent_onDRMStatusEvent);
	OOIF_LOG_DEBUG(" - [%s : %d]\n",__FUNCTION__,__LINE__);
	return OOIF_RETURN_OK;


}

/*
 * This is implemented in such a way that if you "Sesame" in the msg
 * string, it will return an "That's ok with me" and 0 in
 * resultCode. Anything else will return a failed message.
 */
OOIFReturnCode native_DrmAgentSendDRMMessage(DrmAgentHandle identifier, const char *msgType, const char *msg, const char *DRMSystemID, const char **retval)
{
	HUINT16		retMsgId = 0;
	OplDrmAgent 	*drmagentObj = static_cast<OplDrmAgent *>(identifier);

	DrmEvent 	e;
	DrmEventInfo 	info;
    static HCHAR    szMsgId[MAX_BUF_LEN];
    memset(szMsgId, 0x00, MAX_BUF_LEN);

//	if(CHECK_VALID_MSG_TYPE(msgType))

	OOIF_LOG_DEBUG("msgType : %s \n",msgType);
	OOIF_LOG_DEBUG("msg : %s \n",msg);
	OOIF_LOG_DEBUG("DRMSystemID : %s \n",DRMSystemID);

	if( drmagentObj->sendDrmMessage(msgType, msg, DRMSystemID, retMsgId) == BPL_STATUS_OK)
	{
    	snprintf(szMsgId, MAX_BUF_LEN, "%d", retMsgId);
		*retval = szMsgId;
	}
	else
	{
		for (int i=0; i<7; i++)
			szMsgId[i] = 'a' + __urandom()%26;
		szMsgId[7] = 0;
		e = DRM_EVENT_DRM_MESSAGE_RESULT;
		info.drm_message_result_data.msgID = szMsgId;
		info.drm_message_result_data.resultMsg = "";
		info.drm_message_result_data.resultCode = 1; // Unknown error on OIPF spec
		DrmAgent::onDrmAgentEvent(e, &info, identifier);
		*retval = szMsgId;
	}

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DrmAgentDRMSystemStatus(DrmAgentHandle id, const char *DRMSystemID, DrmSystemStatus *retval)
{
	OOIF_LOG_DEBUG("DrmAgentDRMSystemStatus");
	OplDrmAgent 	*drmagentObj = static_cast<OplDrmAgent *>(id);

	if(drmagentObj == NULL)
		return OOIF_RETURN_VALUE_NULL;

	eOplDrmAgentDrmSystemStatus_t status = DRM_SYSTEM_STATUS_UNKNOWN;
	drmagentObj->getDrmSystemStatus(DRMSystemID,status);

	*retval = (DrmSystemStatus)status;
	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_DrmAgentReleaseHandle(DrmAgentHandle id)
{
	OplDrmAgent 	*drmagentObj = static_cast<OplDrmAgent *>(id);
	if(drmagentObj && OplDrmAgentManager::getInstance().RemoveDRMAgent(drmagentObj) == BPL_STATUS_OK)
	{
		delete drmagentObj;
		OplDrmAgentManager::releaseInstance(); // have to call all the time
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DrmAgentOnListenerAccessed(DrmAgentHandle id, const char *eventName)
{
	return OOIF_RETURN_OK;
}

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
OOIFReturnCode native_DRMControlInformationInitialize(DRMControlInformationHandle identifier, DRMControlInformationData *data)
{
	data->drmType = HLIB_STD_StrDup("Dummy DRM Type");
	data->drmContentID = HLIB_STD_StrDup("Dummy DRM Content ID");
	data->rightsIssuerURL = HLIB_STD_StrDup("dummy://issuer");
	data->silentRightsURL = HLIB_STD_StrDup("dummy://rights");
	data->previewRightsURL = HLIB_STD_StrDup("dummy://preview");
	data->drmPrivateData = HLIB_STD_StrDup("Dummy Private Data");
	data->doNotRecord = 0;
	data->doNotTimeShift = 0;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DRMControlInformationReleaseHandle(DRMControlInformationHandle id)
{
	return OOIF_RETURN_OK;
}
#endif // HBBTV_DOWNLOAD || OIPF
