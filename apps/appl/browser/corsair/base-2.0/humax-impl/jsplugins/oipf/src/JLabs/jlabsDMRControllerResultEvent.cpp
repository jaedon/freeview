#if defined OIPF && defined JLABS 

#include "jlabsDMRControllerResultEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "opldmr.h"
#include "macros.h"

using namespace Ooif;

jlabsDMRControllerResultEvent::jlabsDMRControllerResultEvent(jsplugin_obj *c, const char *eventName, int event, int result, void *handleId ) : Event( c, eventName )
{
	ENTRY;

	m_nEvent = event;
	m_nResult = result;
	m_handleId = handleId;

	OOIF_LOG_DEBUG("jlabsDMRControllerResultEvent name(%s), event(%d), result(%d), handleId(%p)\n", eventName, event, result, handleId);
}

jlabsDMRControllerResultEvent::~jlabsDMRControllerResultEvent()
{
	ENTRY;
}

int jlabsDMRControllerResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int jlabsDMRControllerResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int jlabsDMRControllerResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;

	switch(m_nEvent)
	{
		case eNATIVE_DMR_REQ_PLAY:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];

				Opl_DLNA_DmrContent_t *content =  static_cast<Opl_DLNA_DmrContent_t *>(m_handleId);

				result = *argv;

				result[0].type = JSP_TYPE_STRING;
				result[0].u.string = content->szImportURI;
				OOIF_LOG_DEBUG("jlabsDMRControllerResultEvent <eNATIVE_DMR_REQ_PLAY> uri - %s  \n", content->szImportURI);

			}
			break;

		case eNATIVE_DMR_PLAY_STOP:             
			OOIF_LOG_DEBUG("jlabsDMRControllerResultEvent <eNATIVE_DMR_PLAY_STOP> \n");
			break;

		default :
			break;
	}

	return RETURN_PARAM_COUNT;
}
#endif

