// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_SWUPDATE
#define __H_SWUPDATE

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "oplswupdate.h"

#define ____HMX_SoftwareUpdate_functions____

#define	SWUPDATE_STRING_MAX		128

typedef void *	hSwupdateHandle;
typedef void (*hSwupdateEventListener)(DxSwUpdate_Event_e event, DxSwUpdate_EventData_t* eventData);

OOIFReturnCode native_HSoftwareUpdateInit(hSwupdateEventListener listener);
OOIFReturnCode native_HSoftwareUpdate_StartDetect(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle);
OOIFReturnCode native_HSoftwareUpdate_StartDownload(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle);
OOIFReturnCode native_HSoftwareUpdate_StartInstall(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle);
void native_HSwupdateEventListener(DxSwUpdate_EventData_t *pstEventData);
OOIFReturnCode native_HSwUpdateSetListener(hSwupdateEventListener listener);
OOIFReturnCode native_HSwUpdateGetVerBootupHumaxOta(const char **retval);
OOIFReturnCode native_HSwUpdateGetNetworkId(OOIFNumber *retval);
OOIFReturnCode native_HSwUpdateSetNetworkId(OOIFNumber value);


namespace Ooif
{
	class HSoftwareUpdate : public EventTarget
	{
	private:
		static list<HSoftwareUpdate*> 	instances;
		static _OPL_SWUPDATE_Info_t		s_stBplSwupdateInfo;

		static	DxSwUpdate_DetectResultTuning_t	s_stTunningInfo;

	public:
		HSoftwareUpdate();
		~HSoftwareUpdate();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void hapi_broadcastIsOtaSignal(bool isOta);

		static int swupdateInit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startDetectWithType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startInstall(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelInstall(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int startManualDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelManualDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void onSwupdateEvent(DxSwUpdate_Event_e eEvent, DxSwUpdate_EventData_t *pstEventData);
		static int restartSystem(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int startUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int startSwupdateState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_SOFTWARE_UPDATE;

	};

	class CSwUpdateInfo : public ObjectInstance
	{
		private:
			DxSwUpdate_EventData_t		*m_EventInfo;

		public:
			CSwUpdateInfo(DxSwUpdate_EventData_t *pstEventInfo);
			~CSwUpdateInfo();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static int	getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static const ClassType class_name = CLASS_SOFTWARE_UPDATEINFO;
	};
} /* namespace Ooif */

#endif // __H_SWUPDATE
#endif // OIPF


