#ifdef OIPF
#include "HChannelImportExport.h"
#include "HChannelImportExportEvent.h"


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


list<HChannelImportExport*> HChannelImportExport::instances;


HChannelImportExport::HChannelImportExport() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HChannelImportExport::instances.push_back(this);
}


HChannelImportExport::~HChannelImportExport()
{
	ENTRY;
	HChannelImportExport::instances.remove(this);

}

int HChannelImportExport::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(startChannelExport,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startChannelImport,	obj, "", JSP_GET_VALUE_CACHE);	

	INTRINSIC_EVENT_GET(ChannelImportExport);
	return JSP_GET_NOTFOUND;
}

int HChannelImportExport::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	
	INTRINSIC_EVENT(ChannelImportExport);

	return JSP_PUT_NOTFOUND;
}


int HChannelImportExport::startChannelExport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Start channel export!!! ");
	native_HChannelImportExport_startChannelExport(onChannelImportExportEvent);
	
	return 0;
}


int HChannelImportExport::startChannelImport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Start channel import!!! ");
	native_HChannelImportExport_startChannelImport(onChannelImportExportEvent);
	
	return 0;
}


void HChannelImportExport::onChannelImportExportEvent(HUINT32 type, HUINT32 result)
{
	OOIF_LOG_DEBUG(" Received Channel Import Export Message : %d, %d \n ", type, result);

	for (list<HChannelImportExport*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

				e = new HChannelImportExportEvent(context,
						type,
						result);
		(*it)->dispatchEvent(e);
	}

}

#endif // OIPF


#define ____HMX_ChannelImportExport_functions____

hChannelImportExportEventListener global_hChannelImportExportListener = NULL;	// global listener variable for channel import export


OOIFReturnCode native_HChannelImportExport_startChannelExport(hChannelImportExportEventListener listener)
{
	native_HChannelImportExportSetListener(listener);
	OPL_HMXIrdetoExportDBToUsb(native_HChannelImportExportEventListener, TRUE);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HChannelImportExport_startChannelImport(hChannelImportExportEventListener listener)
{
	native_HChannelImportExportSetListener(listener);
	OPL_HMXIrdetoImportDBToUsb(native_HChannelImportExportEventListener, TRUE);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HChannelImportExportSetListener(hChannelImportExportEventListener listener)
{
	global_hChannelImportExportListener = listener;
	return OOIF_RETURN_OK;
}


void native_HChannelImportExportEventListener(HUINT32 type, HUINT32 result)
{

	if (global_hChannelImportExportListener)
	{
		global_hChannelImportExportListener(type, result);
	}
}


