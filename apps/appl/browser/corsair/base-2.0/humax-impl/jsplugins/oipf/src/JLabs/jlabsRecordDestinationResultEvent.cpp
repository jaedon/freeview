#ifdef OIPF

#include "jlabsRecordDestinationResultEvent.h"
#include "jlabsDMS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "macros.h"

typedef enum
{
	eDMP_CONTENT_END = 0,
	eDMP_CONTENT_ADD
} DMP_CONTENT_Update_e;

using namespace Ooif;

jlabsRecordDestinationResultEvent::jlabsRecordDestinationResultEvent( jsplugin_obj *c, int event, int size, jlabsRecordDestinationHandle *handleId) : Event( c, "RecordDestinationsResult"), m_handleId( handleId), m_nState( event ), m_nHandleCount( size )
{
	ENTRY;
}

jlabsRecordDestinationResultEvent::~jlabsRecordDestinationResultEvent()
{
	ENTRY;
}

int jlabsRecordDestinationResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int jlabsRecordDestinationResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsRecordDestinationResultEvent::serialize(jsplugin_value **argv)
{
	int count = m_nHandleCount;
	ENTRY;

	OOIF_LOG_DEBUG("debug  ", "(*) jlabsRecordDestinationResultEvent() Class serialize\n");

	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	jlabsRecordDestinationCollection *collection = new jlabsRecordDestinationCollection;
	jlabsRecordDestination *content = NULL;

	for ( int i = 0; i< count; i++ )
	{
		content = new jlabsRecordDestination(m_handleId[i]);
		collection->addItem( content  );
	}

	SET_SERIALIZED_OBJECT( collection, 0 );

	if ( NULL != m_handleId )
	{
		HLIB_STD_MemFree( m_handleId );
		m_handleId = NULL;
	}

	return 1;
}


jlabsRecordDestinationInfoResultEvent::jlabsRecordDestinationInfoResultEvent( jsplugin_obj *c, double total, double available, char *dst)
	: Event( c, "AvailableStorageResult"), m_nTotalSize(total), m_nAvailSize(available)
{
	m_dst = HLIB_STD_StrDup(dst);

	ENTRY;
}

jlabsRecordDestinationInfoResultEvent::~jlabsRecordDestinationInfoResultEvent()
{
	ENTRY;
}

int jlabsRecordDestinationInfoResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsRecordDestinationInfoResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsRecordDestinationInfoResultEvent::serialize(jsplugin_value **argv)
{
	OOIF_LOG_DEBUG("debug  ", "(*) jlabsRecordDestinationInfoResultEvent () Class serialize\n");

	*argv = new jsplugin_value[3];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = m_dst;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nTotalSize;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = m_nAvailSize;

	return 3;
}

#endif

