#ifdef OIPF
#include "HCasActionIrEmmForcedOta.h"
#include "HCasActionIrEmmForcedOtaEvent.h"


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


list<HCasActionIrEmmForcedOta*> HCasActionIrEmmForcedOta::instances;


HCasActionIrEmmForcedOta::HCasActionIrEmmForcedOta() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HCasActionIrEmmForcedOta::instances.push_back(this);
}


HCasActionIrEmmForcedOta::~HCasActionIrEmmForcedOta()
{
	ENTRY;
	HCasActionIrEmmForcedOta::instances.remove(this);

}

int HCasActionIrEmmForcedOta::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(setListener, obj, "", JSP_GET_VALUE_CACHE); 
	GET_FUNCTION(releaseListener, obj, "", JSP_GET_VALUE_CACHE); 


	INTRINSIC_EVENT_GET(CasActionIrEmmForcedOta);
	return JSP_GET_NOTFOUND;
}

int HCasActionIrEmmForcedOta::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	
	INTRINSIC_EVENT(CasActionIrEmmForcedOta);

	return JSP_PUT_NOTFOUND;
}

int HCasActionIrEmmForcedOta::setListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIF_LOG_DEBUG(" HCasActionIrEmmForcedOta::setListener !!! ");

	NATIVE_CALL(native_HCasActionIrEmmForcedOta_setListener(onCasActionIrEmmForcedOtaEvent));	
	return 0;
}


int HCasActionIrEmmForcedOta::releaseListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIF_LOG_DEBUG(" HCasActionIrEmmForcedOta::releaseListener !!! ");

	NATIVE_CALL(native_HCasActionIrEmmForcedOta_releaseListener(onCasActionIrEmmForcedOtaEvent));
	return 0;
}

void HCasActionIrEmmForcedOta::onCasActionIrEmmForcedOtaEvent(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	OOIF_LOG_DEBUG(" onCasActionIrEmmForcedOta is called \n ");

	for (list<HCasActionIrEmmForcedOta*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		e = new HCasActionIrEmmForcedOtaEvent(context,
											  ulParam1,
											  ulParam2,
											  ulParam3);
		(*it)->dispatchEvent(e);
	}
}

#endif // OIPF


#define ____HMX_CasActionIrEmmForcedOta_functions____

hCasActionIrEmmForcedOtaEventListener global_hCasActionIrEmmForcedOtaEventListener = NULL;	// global listener variable for Cas Action Irdeto Emm Forced Ota

OOIFReturnCode native_HCasActionIrEmmForcedOta_setListener(hCasActionIrEmmForcedOtaEventListener listener)
{
	native_HCasActionIrEmmForcedOtaEventSetListener(listener);
	OPL_HMXCasActionIrEmmForcedOta_RegisterNotifier(native_HCasActionIrEmmForcedOtaEventListener);	
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HCasActionIrEmmForcedOta_releaseListener(hCasActionIrEmmForcedOtaEventListener listener)
{
	native_HCasActionIrEmmForcedOtaEventSetListener(NULL);
	OPL_HMXCasActionIrEmmForcedOta_UnregisterNotifier(native_HCasActionIrEmmForcedOtaEventListener);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HCasActionIrEmmForcedOtaEventSetListener(hCasActionIrEmmForcedOtaEventListener listener)
{
	global_hCasActionIrEmmForcedOtaEventListener = listener;
	return OOIF_RETURN_OK;
}


void native_HCasActionIrEmmForcedOtaEventListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (global_hCasActionIrEmmForcedOtaEventListener)
	{
		global_hCasActionIrEmmForcedOtaEventListener(ulParam1, ulParam2, ulParam3);
	}
}

