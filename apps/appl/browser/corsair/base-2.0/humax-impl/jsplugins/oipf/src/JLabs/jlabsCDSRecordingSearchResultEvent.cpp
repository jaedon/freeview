#ifdef OIPF

#include "jlabsCDSRecordingSearchResultEvent.h"

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

jlabsCDSRecordingSearchResultEvent::jlabsCDSRecordingSearchResultEvent( jsplugin_obj *c, int event, int size, jlabsCDSRecordingHandle *handleId) : Event( c, "CDSBrowseResult"), m_handleId( handleId), m_nState( event ), m_nHandleCount( size )
{
	ENTRY;

	jlabsCDSMetaHandle metaHandle =  NULL;
	m_collection = new jlabsCDSRecordingCollection;
	jlabsCDSRecording *content = NULL;
	int total_matches = 0;

	for ( int i = 0; i< size; i++ )
	{
		metaHandle = native_jlabsCDSRecordingGetMetaData( m_handleId[i] );
		content = new jlabsCDSRecording(m_handleId[i], metaHandle);
		m_collection->addItem( content  );
		native_jlabsCDSRecordingGetTotalMatchs(m_handleId[i], &total_matches);
		m_nTotal_matches = total_matches;
	}
}

jlabsCDSRecordingSearchResultEvent::~jlabsCDSRecordingSearchResultEvent()
{
	delete m_collection;
	ENTRY;
}

int jlabsCDSRecordingSearchResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (strcmp(name, "reclist") == 0)
	{
		RETURN_OBJECT(obj, m_collection, result, JSP_GET_VALUE_CACHE);
	}

	GET_NUMBER("totalMatches", m_nHandleCount, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsCDSRecordingSearchResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsCDSRecordingSearchResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;

	int count = m_nHandleCount;
	int total_matchs = 0;

	OOIF_LOG_DEBUG("debug  ", "(*) jlabsCDSRecordingSearchResultEvent() Class serialize\n");

	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	result[1].type = JSP_TYPE_NUMBER;

#if 0
	if ( eDMP_CONTENT_END == m_nState )
	{
		return 0;
	}
#endif

	jlabsCDSMetaHandle metaHandle =  NULL;
	jlabsCDSRecordingCollection *collection = new jlabsCDSRecordingCollection;
	jlabsCDSRecording *content = NULL;

	for ( int i = 0; i< count; i++ )
	{
		metaHandle = native_jlabsCDSRecordingGetMetaData( m_handleId[i] );
		content = new jlabsCDSRecording(m_handleId[i], metaHandle);
		collection->addItem( content  );
		native_jlabsCDSRecordingGetTotalMatchs(m_handleId[i], &total_matchs);
	}

	result[1].u.number = total_matchs;

	SET_SERIALIZED_OBJECT( collection, 0 );

	if ( NULL != m_handleId )
	{
		HLIB_STD_MemFree( m_handleId );
		m_handleId = NULL;
	}

	return 2;
}
#endif

