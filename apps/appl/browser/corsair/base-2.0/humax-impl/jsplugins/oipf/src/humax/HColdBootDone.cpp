#ifdef OIPF
#include "HColdBootDone.h"
#include "HColdBootDoneEvent.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>

#include "jsplugin.h"
#include "macros.h"
#include "NativeHConfiguration.h"
#include "opllocalsystem.h"


using namespace Ooif;


list<HColdBootDone*> HColdBootDone::instances;


HColdBootDone::HColdBootDone() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HColdBootDone::instances.push_back(this);
}


HColdBootDone::~HColdBootDone()
{
	ENTRY;
	HColdBootDone::instances.remove(this);

}

int HColdBootDone::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(getColdBootDone, obj, "", JSP_GET_VALUE_CACHE); 
	GET_FUNCTION(releaseColdBootDoneListener, obj, "", JSP_GET_VALUE_CACHE); 


	INTRINSIC_EVENT_GET(ColdBootDone);
	return JSP_GET_NOTFOUND;
}

int HColdBootDone::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	
	INTRINSIC_EVENT(ColdBootDone);

	return JSP_PUT_NOTFOUND;
}

int HColdBootDone::getColdBootDone(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval; 	
	NATIVE_CALL(native_HColdBootDone_getColdBootDone(&retval, onColdBootDoneEvent));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);	
	return 0;
}


int HColdBootDone::releaseColdBootDoneListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	
	OOIF_LOG_DEBUG(" releaseColdBootDoneListener !!! ");
	native_HColdBootDone_releaseListener();
	
	return 0;
}

void HColdBootDone::onColdBootDoneEvent(void)
{
	OOIF_LOG_DEBUG(" onColdBootDoneEvent is called \n ");

	for (list<HColdBootDone*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		e = new HColdBootDoneEvent(context);
		(*it)->dispatchEvent(e);
	}
}

#endif // OIPF


#define ____HMX_ColdBootDone_functions____

hColdBootDoneEventListener global_hColdBootDoneEventListener = NULL;	// global listener variable for channel import export


OOIFReturnCode native_HColdBootDone_getColdBootDone(OOIFBoolean *retval, hColdBootDoneEventListener listener)
{
	BPL_BOOL bFlag;
	native_HColdBootDoneSetListener(listener);
	OPL_HMXColdBoot_GetColdBootDone(&bFlag, native_HColdBootDoneEventListener);	
	*retval = bFlag;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HColdBootDone_releaseListener(void)
{
	native_HColdBootDoneSetListener(NULL);
	OPL_HMXColdBoot_UnregisterNotifier();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HColdBootDoneSetListener(hColdBootDoneEventListener listener)
{
	global_hColdBootDoneEventListener = listener;
	return OOIF_RETURN_OK;
}


void native_HColdBootDoneEventListener(void)
{

	if (global_hColdBootDoneEventListener)
	{
		global_hColdBootDoneEventListener();
	}
}



