#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsNetworkService.h"
#include "jlabsNetworkServiceEventResult.h"

using namespace Ooif;

list<jlabsNetworkService*> jlabsNetworkService::instances;
jlabsNetworkService::jlabsNetworkService(jlabsNetworkServiceHandle i) : WrapperObject<jlabsNetworkService>(i)
{
	ENTRY;
    classType = CLASS_JLABS_NETWORK_SERVICE;
}

jlabsNetworkService::jlabsNetworkService(const jlabsNetworkService &original) : WrapperObject<jlabsNetworkService>(original.identifier)
{
	ENTRY;
    classType = CLASS_JLABS_NETWORK_SERVICE;
	if ( 0 == instances.size() )
	{
		native_jlabsNetworkServiceRegisterEventHandler( EventHandler );
	}
	instances.push_back(this);
}

jlabsNetworkService::~jlabsNetworkService()
{
    instances.remove(this);
	if ( 0 == instances.size() )
	{
		native_jlabsNetworkServiceUnRegisterEventHandler( EventHandler );
	}
}

int jlabsNetworkService::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("serviceName", native_jlabsNetworkServiceGetServiceName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("targetName", native_jlabsNetworkServiceGetTargetName(identifier, &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(get, obj, "s", JSP_GET_VALUE);
	GET_FUNCTION(set, obj, "ss", JSP_GET_VALUE);
	GET_FUNCTION(restart, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(ChangeStatus);
	return JSP_GET_NOTFOUND;
}

int jlabsNetworkService::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	
	NATIVE_SET("targetName", native_jlabsNetworkServiceSetTargetName(identifier, STRING_VALUE(value[0])));
	INTRINSIC_EVENT(ChangeStatus);
	return JSP_PUT_NOTFOUND;
}

//static 
int jlabsNetworkService::get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	
	jlabsNetworkService	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkService);
	const char			*retval;
	
	NATIVE_CALL(native_jlabsNetworkServiceGet(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));
	
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
//static 
int jlabsNetworkService::set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_STRING);
	
	jlabsNetworkService	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkService);
	const char			*retval;
	
	NATIVE_CALL(native_jlabsNetworkServiceSet(instance->getIdentifier(), STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int jlabsNetworkService::restart(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	
	jlabsNetworkService	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkService);
	OOIFNumber			retval;
	
	NATIVE_CALL(native_jlabsNetworkServiceRestart(instance->getIdentifier(), &retval));
	
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int jlabsNetworkService::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	
	jlabsNetworkService	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkService);
	OOIFNumber			retval;
	
	NATIVE_CALL(native_jlabsNetworkServiceStop(instance->getIdentifier(), &retval));
	
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
void jlabsNetworkService::releaseNative(jlabsNetworkServiceHandle id)
{
	NATIVE_WARN_NOK(native_jlabsNetworkServiceReleaseHandle(id));
}

void jlabsNetworkService::EventHandler( int nServiceIndex, int nEvent, int nParam1, int nParam2 )
{
	jlabsNetworkService	*netService = NULL;
	jlabsNetworkServiceHandle hInstanceId = NULL;
	jsplugin_obj *context  = NULL;
	Event *e = NULL;
	int nEventIfIndex = nParam1;
	OOIFNumber nInstanceSvcIndex = -1;
	OOIFNumber nInstanceTargetIndex = -1;

	OOIF_LOG_DEBUG("(+) [%s,%d] WiFiScan Event(%d).\n", __FUNCTION__, __LINE__, nEvent);
	for ( list<jlabsNetworkService *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		hInstanceId = (*it)->getIdentifier();
		native_jlabsNetworkServiceGetSvcIndex( hInstanceId, &nInstanceSvcIndex );
		if ( nInstanceSvcIndex != nServiceIndex )	
		{
			continue;
		}
		native_jlabsNetworkServiceGetTargetIndex( hInstanceId, &nInstanceTargetIndex );
		if ( nInstanceTargetIndex != nEventIfIndex )	
		{
			continue;
		}
		OOIF_LOG_DEBUG("(*) [%s,%d] instance is searched. Service(%d), Inteface(%d).\n", __FUNCTION__, __LINE__, nServiceIndex, nInstanceSvcIndex);

		netService = (*it);
		if( NULL == netService )
		{
			continue;
		}

		context = netService->getHost();
		if( NULL == context )
		{
			continue;
		}

		e = new jlabsNetworkServiceEventResult(context, nEvent);
		if(e != NULL)
		{
			netService->dispatchEvent(e);
		}
	}
	OOIF_LOG_DEBUG("(-) [%s,%d] WiFiScan Event(%d).\n", __FUNCTION__, __LINE__, nEvent);
}


