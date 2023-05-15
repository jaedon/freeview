// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  This is the object corresponding to the oipfConfiguration embedded
  object. Due to naming collision with the Configuration object
  (returned from this object) - it is named MasterConfiguration.
 */

#ifndef __HCAS_H__
#define __HCAS_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "EventTarget.h"

namespace Ooif
{
	class HCasUi : public EventTarget
	{
	private :
		static void* 	HCasUiIdentifier;

	public:
		HCasUi();
		~HCasUi();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int openSession(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int closeSession(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int sendUiResponse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int onCasUiEvent(int module, int slot, int type, int session, int eventType, void *pvData);
	};

	class HCasUiEvent : public Event
	{
	private :
		OOIFNumber module;
		OOIFNumber slot;
		OOIFNumber sType;
		OOIFNumber sSession;
		OOIFNumber sEventType;
		char* uiData;

	public :
		HCasUiEvent (jsplugin_obj *c, OOIFNumber _module, OOIFNumber _slot, OOIFNumber _sType, OOIFNumber _sSession, OOIFNumber _sEventType, const char* _uiData);
		~HCasUiEvent();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **argv);
	};

} /* namespace Ooif */

#endif // __HCAS_H__

/* EOF */

