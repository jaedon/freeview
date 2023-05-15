// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_LCN_UPDATE
#define __H_LCN_UPDATE

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"
#include "humax/HDataTypes.h"

#define ____HMX_LcnUpdate_functions____


typedef void (*hLcnUpdateEventListener)(void *pstLcnInform);

OOIFReturnCode native_HLcnUpdate_start(hLcnUpdateEventListener listener, HUINT32 searchMode);
OOIFReturnCode native_HLcnUpdate_stop(hLcnUpdateEventListener listener);
OOIFReturnCode native_HLcnUpdate_save(hLcnUpdateEventListener listener, HUINT32 searchMode, HUINT32 index);
OOIFReturnCode native_HLcnUpdate_getinfo(hLcnUpdateEventListener listener);
OOIFReturnCode native_HLcnUpdate_removeInfo(OOIFBoolean sync);
OOIFReturnCode native_HLcnUpdateSetListener(hLcnUpdateEventListener listener);

void native_HLcnUpdateEventListener(void *pstLcnInform);


namespace Ooif
{
	class HLcnUpdate : public EventTarget
	{
	private:
		static list<HLcnUpdate*> 	instances; // for hadling events.
		static void onLcnUpdateEventOPL(void *pEventData);

	public:
		HLcnUpdate();
		~HLcnUpdate();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int startLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int saveLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getLcnUpdateInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int removeLcnUpdateInfo(jsplugin_obj * this_obj,jsplugin_obj * function_obj,int argc,jsplugin_value * argv,jsplugin_value * result);
		static void onLcnUpdateEvent(humax::LcnUpdateEvent event, humax::LcnUpdateEventInfo *info);

		static const ClassType class_name = CLASS_LCN_UPDATE;

	};

	class CLcnUpdateSvcList : public ObjectInstance
	{
		private:
			int								m_numOfSvcList;
			humax::LcnUpdateSvcList_Data	*svcList;

		public:
			CLcnUpdateSvcList(int _numOfSvcList, humax::LcnUpdateSvcList_Data *_svcList);
			~CLcnUpdateSvcList();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static int	getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static const ClassType class_name = CLASS_LCNUPDATESVCLIST;
	};

	class CLcnUpdateSvc : public ObjectInstance
	{
		private:
			int								m_numOfSvc;
			humax::LcnUpdateSvc_Data		*svc;

		public:
			CLcnUpdateSvc(int _numOfSvc, humax::LcnUpdateSvc_Data *_svc);
			~CLcnUpdateSvc();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static int	getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static const ClassType class_name = CLASS_LCNUPDATESVC;
	};
} /* namespace Ooif */

#endif // __H_LCN_UPDATE
#endif // OIPF


