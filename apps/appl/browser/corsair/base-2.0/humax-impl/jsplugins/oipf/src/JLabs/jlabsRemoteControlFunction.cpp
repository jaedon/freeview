#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsRemoteControlFunction.h"

using namespace Ooif;

list<jlabsRemoteControlFunction*> jlabsRemoteControlFunction::instances;
jlabsRemoteControlFunction::jlabsRemoteControlFunction(jlabsRemoteControlFunctionHandle i) : WrapperObject<jlabsRemoteControlFunction>(i)

{
	ENTRY;
    classType = CLASS_JLABS_REMOTECONTROL_FUNCTION;
	if ( 0 == instances.size() )
	{
		native_jlabsRemoteControlFunctionRegisterEventHandler( EventHandler );
	}
	instances.push_back(this);
}

jlabsRemoteControlFunction::~jlabsRemoteControlFunction()
{
    instances.remove(this);
	if ( 0 == instances.size() )
	{
		native_jlabsRemoteControlFunctionUnRegisterEventHandler( EventHandler );
	}
}

int jlabsRemoteControlFunction::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("currentRemoteDeviceHandle", native_jlabsRemoteControlFunctionGetDeviceHandle(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("currentRemoteDeviceUA", native_jlabsRemoteControlFunctionGetDeviceUA(identifier, &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(useServerSideXMLUIListing, obj, "sb", JSP_GET_VALUE);
	GET_FUNCTION(sendRemoteMessage, obj, "nnss", JSP_GET_VALUE);
	GET_FUNCTION(sendMulticastNotif, obj, "nnsss", JSP_GET_VALUE);
	GET_FUNCTION(sendInternalServerError, obj, "nn", JSP_GET_VALUE);
	GET_FUNCTION(dropConnection, obj, "n", JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(ReceiveRemoteMessage);
	INTRINSIC_EVENT_GET(ResultMuticastNotif);
	return JSP_GET_NOTFOUND;
}

int jlabsRemoteControlFunction::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	
	INTRINSIC_EVENT(ReceiveRemoteMessage);
	INTRINSIC_EVENT(ResultMuticastNotif);
	return JSP_PUT_NOTFOUND;
}

//static 
int jlabsRemoteControlFunction::useServerSideXMLUIListing(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_BOOLEAN);
	
	jlabsRemoteControlFunction	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsRemoteControlFunction);
	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsRemoteControlFunctionServerSideXMLUIListing(instance->getIdentifier(), STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static 
int jlabsRemoteControlFunction::sendRemoteMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(4);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(2, JSP_TYPE_STRING);
	CHECK_ARGUMENT(3, JSP_TYPE_STRING);
	
	jlabsRemoteControlFunction	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsRemoteControlFunction);
	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsRemoteControlFunctionSendRemoteMessage(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), STRING_VALUE(argv[2]), STRING_VALUE(argv[3]),&retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
//static 
int jlabsRemoteControlFunction::sendMulticastNotif(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(5);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(2, JSP_TYPE_STRING);
	CHECK_ARGUMENT(3, JSP_TYPE_STRING);
	CHECK_ARGUMENT(4, JSP_TYPE_STRING);
	
	jlabsRemoteControlFunction	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsRemoteControlFunction);
	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsRemoteControlFunctionSendMulticatNotify(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), STRING_VALUE(argv[2]), STRING_VALUE(argv[3]), STRING_VALUE(argv[4]), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static 
int jlabsRemoteControlFunction::sendInternalServerError(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	CHECK_ARGUMENT(1, JSP_TYPE_NUMBER);
	
	jlabsRemoteControlFunction	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsRemoteControlFunction);
	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsRemoteControlFunctionSendInternalServerError(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static 
int jlabsRemoteControlFunction::dropConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);
	
	jlabsRemoteControlFunction	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsRemoteControlFunction);
	OOIFBoolean retval;
	NATIVE_CALL(native_jlabsRemoteControlFunctionDropConnection(instance->getIdentifier(), NUMBER_VALUE(argv[0]), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void jlabsRemoteControlFunction::releaseNative(jlabsRemoteControlFunctionHandle id)
{
	NATIVE_WARN_NOK(native_jlabsRemoteControlFunctionReleaseHandle(id));
}

void jlabsRemoteControlFunction::EventHandler( int nServiceIndex, int nEvent, int nParam1, int nParam2 )
{
	jlabsRemoteControlFunction	*netService = NULL;
	jlabsRemoteControlFunctionHandle hInstanceId = NULL;
	jsplugin_obj *context  = NULL;
	Event *e = NULL;
	int nEventIfIndex = nParam1;
	(void )(netService);
	(void )(hInstanceId);
	(void )(e);
	(void )(nEventIfIndex);
	(void )(context);

	OOIF_LOG_DEBUG("(+) [%s,%d] WiFiScan Event(%d).\n", __FUNCTION__, __LINE__, nEvent);
	for ( list<jlabsRemoteControlFunction *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		
	}
	OOIF_LOG_DEBUG("(-) [%s,%d] WiFiScan Event(%d).\n", __FUNCTION__, __LINE__, nEvent);
}


