#ifdef OIPF

#include "jlabsCDSSearcheResultEvent.h"
#include "jlabsDMS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "macros.h"

using namespace Ooif;

jlabsCDSSearcheResultEvent::jlabsCDSSearcheResultEvent(jsplugin_obj *c, const char *eventName, int size, jlabsDMSDeviceHandle handleId ) : Event( c, eventName ), m_handleId( handleId ), m_nHandleCount( size )
{
	ENTRY;
}

jlabsCDSSearcheResultEvent::~jlabsCDSSearcheResultEvent()
{
	ENTRY;
}

int jlabsCDSSearcheResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int jlabsCDSSearcheResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsCDSSearcheResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	OOIF_LOG_DEBUG("debug  ", "(*) jlabsCDSSearcheResultEvent() Class serialize \n");

	int count = m_nHandleCount;

	jlabsDMSDeviceHandle *DMSHandle = (jlabsDMSDeviceHandle *)m_handleId;

	jlabsDMSCollection *collection = new jlabsDMSCollection;
	jlabsDMS *dms= NULL;

	for(int i=0; i<count; i++)
	{
		dms = new jlabsDMS(DMSHandle[i]);
		collection->addItem( dms );
	}

	SET_SERIALIZED_OBJECT( collection, 0 );

	//
	if( m_handleId != NULL)
	{
		HLIB_STD_MemFree(m_handleId);
		m_handleId = NULL;
	}
	//
	return 1;
}
#endif
