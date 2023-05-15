#ifdef OIPF
#include "HClockRecovery.h"
#include "HClockRecoveryEvent.h"


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


list<HClockRecovery*> HClockRecovery::instances;


HClockRecovery::HClockRecovery() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HClockRecovery::instances.push_back(this);
}


HClockRecovery::~HClockRecovery()
{
	ENTRY;
	HClockRecovery::instances.remove(this);

}

int HClockRecovery::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(startClockRecovery, obj, "", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ClockRecovery);
	return JSP_GET_NOTFOUND;
}

int HClockRecovery::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(ClockRecovery);

	return JSP_PUT_NOTFOUND;
}

int HClockRecovery::startClockRecovery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Start Clock Recovery !!! ");
	native_HClockRecovery_startClockRecovery(onClockRecoveryEvent);

	return 0;
}


void HClockRecovery::onClockRecoveryEvent(HUINT32 batchType, HUINT32 batchProcType, HUINT32 err)
{
	OOIF_LOG_DEBUG(" Received Clock Recovery : %d, %d, %d \n ", batchType, batchProcType, err);

	for (list<HClockRecovery*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		e = new HClockRecoveryEvent(context,
				batchType,
				batchProcType,
				err);
		(*it)->dispatchEvent(e);
	}
}

#endif // OIPF


#define ____HMX_ChannelImportExport_functions____

hClockRecoveryEventListener global_HClockRecoveryListener = NULL;	// global listener variable for channel import export


OOIFReturnCode native_HClockRecovery_startClockRecovery(hClockRecoveryEventListener listener)
{
	native_HClockRecoverySetListener(listener);
	OPL_HMXClockRecovery(native_HClockRecoveryEventListener);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HClockRecoverySetListener(hClockRecoveryEventListener listener)
{
	global_HClockRecoveryListener = listener;
	return OOIF_RETURN_OK;
}


void native_HClockRecoveryEventListener(HUINT32 batchType, HUINT32 batchProcType, HUINT32 err)
{
	if (global_HClockRecoveryListener)
	{
		global_HClockRecoveryListener(batchType, batchProcType, err);
	}
}


