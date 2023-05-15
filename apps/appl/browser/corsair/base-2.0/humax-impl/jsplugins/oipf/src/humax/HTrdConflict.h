// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_TRD_CONFLICT
#define __H_TRD_CONFLICT

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"
#include "humax/HDataTypes.h"


#define ____HMX_TrdConflict_functions____


typedef void (*hTrdConflictEventListener)(void *pstTrdConflict);

OOIFReturnCode native_HTrdConflict_GetTrdConflict(hTrdConflictEventListener listener,unsigned int depth);
OOIFReturnCode native_HTrdConflict_SetTrdConflict(unsigned int depth, unsigned int index);
OOIFReturnCode native_HTrdConflictSetListener(hTrdConflictEventListener listener);

void native_HTrdConflictEventListener(void *pstTrdConflict);


namespace Ooif
{
	class HTrdConflict : public EventTarget
	{
	private:
		static list<HTrdConflict*> 	instances; // for hadling events.
		static void onTrdConflictEventOPL(void *pEventData);

	public:
		HTrdConflict();
		~HTrdConflict();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int getTrdConflict(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setTrdConflict(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static void onTrdConflictEvent(humax::TrdConflictEventType_e event,unsigned int trdconflictCount, humax::TrdConflictData_t *info);
		

		

		static const ClassType class_name = CLASS_TRDCONFLICT;

	};
} /* namespace Ooif */

#endif // __H_CHANNEL_IMPORT_EXPORT
#endif // OIPF


