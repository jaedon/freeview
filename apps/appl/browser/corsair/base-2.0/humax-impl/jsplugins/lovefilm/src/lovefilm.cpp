// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  This is the Lovefilm plugin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NJSEUtils.h"

#include "src/HDevice.h"

namespace Lovefilm
{
	static jsplugin_cap cap;

	static const char* global_names[] = { "Device", NULL };
	static const char* default_object_types[] = { "amazon/lovefilm", NULL };

	/** global variable for jsplugin module callbacks */
	static int default_object_types_count = sizeof(default_object_types) / sizeof(default_object_types[0]);
	const char** handled_object_types = NULL;

	jsplugin_callbacks *g_opera_callbacks;
	std::list<jsplugin_obj*> g_proteges;

	void firstInitializeLOVEFILM();

	/** flag set when listeners are initialized */
	bool initialized = false;

	//static
	int debug(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
	{
		CHECK_ARGUMENT_COUNT(1);
#if defined(HMX_WEBUI)
		NATIVE_CALL(native_Debug(STRING_VALUE(argv[0])));
#else
		NJSE_LOG_DEBUG("%s", STRING_VALUE(argv[0]));
#endif
		return JSP_CALL_NO_VALUE;
	}

	static void videoVisible(jsplugin_obj *obj, int visibility, int unloading)
	{
		VisualObject * o = EXTRACT_OBJECT<VisualObject>(obj);
		if (o)
		{
			o->videoVisible(visibility, unloading);
		}
		else
		{
			NJSE_LOG_WARNING("add_tv_visual videoVisible called with unknown object");
		}
	}

	static void videoPosition(jsplugin_obj *obj, int x, int y, int w, int h)
	{
		VisualObject * o = EXTRACT_OBJECT<VisualObject>(obj);
		if (o)
		{
			o->videoPosition(x, y, w, h);
		}
		else
		{
			NJSE_LOG_DEBUG("WARNING: add_tv_visual VideoPosition called with unknown object");
		}
	}

	static int njse_global_getter(jsplugin_obj *ctx, const char *name, jsplugin_value *result)
	{
		ENTRY;

		GET_OBJECT("Device", ctx, new HDevice(), JSP_GET_VALUE_CACHE);
		
		return JSP_GET_NOTFOUND;
	}

	static void njse_global_gc_trace(jsplugin_obj *this_obj)
	{
		ENTRY;
		for (list<jsplugin_obj*>::iterator it=g_proteges.begin(); it!=g_proteges.end(); it++)
		{
			jsplugin_obj *obj = *it;
			g_opera_callbacks->gcmark(obj);
		}
	}

	int handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
	                  jsplugin_getter **getter, jsplugin_setter **setter, jsplugin_destructor **destructor,
	                  jsplugin_gc_trace **trace, jsplugin_notify **inserted, jsplugin_notify **removed)
	{
		ENTRY;
		const char *type = NULL;
		for(int i=0; i<attrs_count; i++)
		{
			if(!strcmp(attrs[i].name, "type"))
			{
				type = attrs[i].value;
			}
		}

		if(!type) return JSP_OBJECT_ERROR;

		HANDLE_OBJECT("amazon/lovefilm", new HDevice(), JSP_OBJECT_OK);
		
		return JSP_OBJECT_ERROR;
	}

	void lovefilm_init(jsplugin_obj *global_context)
	{
		ENTRY;
		if (!initialized)
		{
			firstInitializeLOVEFILM();
			initialized = true;
		}
		NJSEUtils::registerGlobalContext(global_context);
	}

	void lovefilm_destroy(jsplugin_obj *global_context)
	{
		ENTRY;
		NJSEUtils::unregisterGlobalContext(global_context);
		NJSEUtils::freeTmpBuffer();
	}

	int lovefilm_allow_access(const char *protocol, const char *hostname, int port, const char *url)
	{
		NJSEBoolean retval = FALSE;
		ENTRY;

		return retval;
	}

	void firstInitializeLOVEFILM()
	{
		ENTRY;
	}

} /* namespace Lovefilm */

using namespace Lovefilm;

/* Someone may want to compile and link under Windows */
# if defined (WIN32) || defined (WINCE)	// {
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

extern "C" {

	static void final_free_object_types()
	{
		if (handled_object_types)
		{
			free(handled_object_types);
			handled_object_types = NULL;
		}
	}

	/* the global plugin hook. */
	DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
	{
	    NJSE_LOG_FPRINT("====== NJSE plugin loaded ======\n");
        memset(&cap, 0, sizeof(jsplugin_cap));
        
		/* Begin of Temporary */
        handled_object_types = (const char**) malloc((default_object_types_count) * sizeof(const char*));
        atexit(final_free_object_types);
        memcpy(handled_object_types, default_object_types, default_object_types_count * sizeof(const char*));
		/* End of Temporary */
		
        cap.global_names = global_names;
        cap.object_types = handled_object_types;

        cap.global_getter = njse_global_getter;
        cap.global_setter = NULL;
        cap.init = lovefilm_init;
        cap.destroy = lovefilm_destroy;
        cap.handle_object = handle_object;
        cap.allow_access = lovefilm_allow_access;
        cap.gc_trace = njse_global_gc_trace;

        *result = &cap;

        c->add_tv_visual(videoVisible, videoPosition);

        g_opera_callbacks = c;
		return 0;
	}

} // extern "C"
