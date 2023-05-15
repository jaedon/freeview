#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Collection.h"
#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "EmptyEvent.h"

#include "jlabsDMS.h"
#include "jlabsContentSearch.h"
#include "jlabsRecordDestination.h"
#include "jlabsCDSRecordingSearchResultEvent.h"
#include "jlabsRecordDestinationResultEvent.h"


typedef enum
{
	eDMP_CONTENT_END = 0,
	eDMP_CONTENT_ADD,

	eDMP_RECORD_DESTINATIONS,
	eDMP_RECORD_DISKINFO,
} DMP_CONTENT_Update_e;


using namespace Ooif;

jlabsDMS * jlabsDMS::m_sDMSDevice = NULL;

jlabsDMS::jlabsDMS(jlabsDMSDeviceHandle id) : EventTarget(), WrapperObject<jlabsDMS>(id)
{
	ENTRY;
	classType = CLASS_JLABS_DMSDEVICE;
}

jlabsDMS::jlabsDMS(const jlabsDMS &original ) : WrapperObject<jlabsDMS>(original.identifier)
{
	ENTRY;
	classType = CLASS_JLABS_DMSDEVICE;
}

jlabsDMS::~jlabsDMS()
{
	ENTRY;
}

int jlabsDMS::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("deviceHandle", native_jlabsDMSDeviceGetDeviceHandle(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("UDN", native_jlabsDMSDeviceGetUDN(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("friendlyName", native_jlabsDMSDeviceGetFriendlyName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("ipAddress", native_jlabsDMSDeviceGetIpAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("MACAddress", native_jlabsDMSDeviceMACAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("totalStorageSize", native_jlabsDMSDeviceGetTotalStorageSize(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("sortCAP", obj, native_jlabsDMSDeviceGetSupportedSortCAP(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("searchCAP", obj, native_jlabsDMSDeviceGetSupportedSearchCAP(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("dlnaCAP", obj, native_jlabsDMSDeviceGetSupportedDlnaCAP(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("jlabsCAP", obj, native_jlabsDMSDeviceGetSupportedJlabsCAP(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("contentSearch", obj, native_jlabsDMSDeviceContentSearch(identifier, &RETVAL),
			new jlabsContentSearch(RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(browseDirectChildren, obj, "ssnn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createContentSearch, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAvailableStorageSize, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getRecordDestinations, obj, "", JSP_GET_VALUE_CACHE);

	GET_FUNCTION(addEventListener,      obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener,   obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ContentSearch);
	INTRINSIC_EVENT_GET(CDSBrowseResult);
	INTRINSIC_EVENT_GET(RecordDestinationsResult);
	INTRINSIC_EVENT_GET(AvailableStorageResult);

	return EventTarget::getter(obj, name, result);
}

int jlabsDMS::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	native_jlabsDMSDeviceRegisterEventHandler( jlabsDMS::callbackEventHandler );

	if ( !strcmp(name, "onCDSBrowseResult") )
	{
		INTRINSIC_EVENT(CDSBrowseResult);
	}

	if ( !strcmp(name, "onContentSearch") )
	{
		INTRINSIC_EVENT(ContentSearch);
	}

	if ( !strcmp(name, "onRecordDestinationsResult") )
	{
		INTRINSIC_EVENT(RecordDestinationsResult);
	}

	if ( !strcmp(name, "onAvailableStorageResult") )
	{
		INTRINSIC_EVENT(AvailableStorageResult);
	}

	return JSP_PUT_NOTFOUND;
}

void jlabsDMS::releaseNative(jlabsDMSDeviceHandle id)
{
	NATIVE_WARN_NOK(native_jlabsDMSDeviceReleaseHandle(id));
}


int jlabsDMS::addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 3);
	if (argc != 2 && argc != 3)
	{
		OOIF_LOG_WARNING("Invalid argument count to addEventListener() %d "
				"instead of 2 or 3 - aborting", argc);
		return JSP_CALL_ERROR;
	}
	if (argv[0].type == JSP_TYPE_STRING && argv[1].type == JSP_TYPE_NATIVE_OBJECT)
	{
		jlabsDMS    *dmsDevice = EXTRACT_OBJECT_OR_THROW( dmsDevice, this_obj, jlabsDMS );
		NATIVE_CALL(dmsDevice->internalAddEventListener(argv[0].u.string, argv[1].u.object, TRUE));
		return JSP_CALL_NO_VALUE;
	}
	else
	{
		OOIF_LOG_WARNING("Invalid argument passed to addEventListener() - aborting");
		return JSP_CALL_ERROR;
	}
}

int jlabsDMS::browseDirectChildren(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	jlabsDMS    *dmsDevice = EXTRACT_OBJECT_OR_THROW( dmsDevice, this_obj, jlabsDMS );
	OOIFBoolean retval = TRUE;

	char cid[512];
	char sort[128];
	int index = 0;
	int requestedCount = 0;

	strncpy( cid, STRING_VALUE(argv[0]), sizeof(cid));
	strncpy( sort, STRING_VALUE(argv[1]), sizeof(sort));
	index = NUMBER_VALUE(argv[2]);
	requestedCount = NUMBER_VALUE(argv[3]);

	m_sDMSDevice = dmsDevice;

	OOIF_LOG_DEBUG("debug ", "cid : %s, sort : %s, index : %d, req : %d\n", cid, sort, index, requestedCount);
	native_jlabsDMSDeviceBrowseDirectChildren( dmsDevice->identifier, cid, sort, index, requestedCount, &retval );

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsDMS::createContentSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIF_LOG_DUMMY("createContentSearch");

	RETURN_NULL(0);
	//RETURN_BOOLEAN(FALSE, JSP_CALL_VALUE);
}


int jlabsDMS::getAvailableStorageSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval;
	jlabsDMS    *dmsDevice = EXTRACT_OBJECT_OR_THROW( dmsDevice, this_obj, jlabsDMS );

	
	char dstId[256];

	memset(dstId, 0x00, sizeof(dstId));
	strncpy( dstId, STRING_VALUE(argv[0]), sizeof(dstId));

	m_sDMSDevice = dmsDevice;

	native_jlabsDMSgetAvailableStorageSize( dmsDevice->identifier, dstId, &retval );

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsDMS::getRecordDestinations(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;
	CHECK_ARGUMENT_COUNT(0);
	jlabsDMS    *dmsDevice = EXTRACT_OBJECT_OR_THROW( dmsDevice, this_obj, jlabsDMS );

	m_sDMSDevice = dmsDevice;

	native_jlabsDMSDeviceGetRecordDestinations(dmsDevice->identifier, &retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void jlabsDMS::callbackEventHandler( int event, int size, void **content )
{
	jlabsDMS * dms = NULL;
	jsplugin_obj *context = NULL;

	dms = m_sDMSDevice;
	if ( NULL == dms )
	{
		return ;
	}

	context = dms->getHost();
	Event *e = NULL;

	switch( event )
	{
		case eDMP_CONTENT_END :
		case eDMP_CONTENT_ADD :
				e = new jlabsCDSRecordingSearchResultEvent( context, event, size, (jlabsCDSRecordingHandle *)content);
				dms->dispatchEvent(e);
				break;

		case eDMP_RECORD_DESTINATIONS:
				e = new jlabsRecordDestinationResultEvent( context, event, size, (jlabsRecordDestinationHandle*)content);
				dms->dispatchEvent(e);
				break;
				
		case eDMP_RECORD_DISKINFO:
				{
					double total  = 0, available = 0;
					char *dst = NULL;
					if( _native_jlabsDMSgetAvailableStorageSize( (void *)content, &total, &available, &dst) == OOIF_RETURN_OK)
					{
						e = new jlabsRecordDestinationInfoResultEvent( context, total, available, dst);
						if( e != NULL)
						{
							dms->dispatchEvent(e);
						}
					}
				}
				break;

		default :
			return ;
	}
}

#endif
