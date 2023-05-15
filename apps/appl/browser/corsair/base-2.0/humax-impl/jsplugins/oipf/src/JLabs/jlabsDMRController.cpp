#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Collection.h"
#include "macros.h"
#include "Exception.h"
#include "EmptyEvent.h"

#include "NativeJlabsDMRController.h"
#include "jlabsDMRController.h"
#include "jlabsDMRControllerResultEvent.h"

using namespace Ooif;

jlabsDMRController *jlabsDMRController::m_pDmrInstance= NULL;

jlabsDMRController::jlabsDMRController() : EventTarget()
{
	ENTRY;
	OOIFBoolean retval;

	OOIF_LOG_DEBUG("jlabsDMRController Class Create !!\n");

	m_pDmrInstance = this;
	native_jlabsDMRControllerEventListener(jlabsDMRController::callbackEventHandler);
	native_jlabsDMRControllerStart(&retval);
}

jlabsDMRController::~jlabsDMRController()
{
	ENTRY;
	OOIFBoolean retval;

	OOIF_LOG_DEBUG("jlabsDMRController Class Destroy !!\n");

	native_jlabsDMRControllerStop(&retval);
}

int jlabsDMRController::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;

	/* DMR Play 여부 */
	GET_FUNCTION(isPlay, obj, "-", JSP_GET_VALUE_CACHE);
	/* DMR Play Stop 요청 */
	GET_FUNCTION(stop, obj, "-", JSP_GET_VALUE_CACHE);

	/* DMR Play Start Event */
	INTRINSIC_EVENT_GET(DMRPlay);
	/* DMR Play Stop Event */
	INTRINSIC_EVENT_GET(DMRStop);

	return EventTarget::getter(obj, name, result);
}

int jlabsDMRController::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	if ( !strcmp(name, "onDMRPlay") )
	{
		INTRINSIC_EVENT(DMRPlay);
	}

	if ( !strcmp(name, "onDMRStop") )
	{
		INTRINSIC_EVENT(DMRStop);
	}

	return JSP_PUT_NOTFOUND;
}


int jlabsDMRController::isPlay(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = FALSE;

	native_jlabsDMRControllerIsPlay(&retval); 

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


int jlabsDMRController::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = FALSE;

	native_jlabsDMRControllerPlayingStop(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void jlabsDMRController::callbackEventHandler(unsigned int event, unsigned int result, void *cdsItem)
{
	ENTRY;

	OOIF_LOG_DEBUG("[callbackEventHandler] received event [%d]\n", event);

	Event *e = NULL;
	jsplugin_obj *context = NULL;

	jlabsDMRController *pDmrInstance = m_pDmrInstance;

	if ( NULL == pDmrInstance )
	{
		OOIF_LOG_DEBUG("Dmr has Null instance !!!! \n");
		return ;
	}

	context = pDmrInstance->getHost();

	OOIF_LOG_DEBUG("callbackEventHandler - context(%p), m_pHDlnaDmrInstance(%p), cdsHandle = %p\n", context, pDmrInstance, cdsItem);

	switch( event )
	{
		case eNATIVE_DMR_REQ_PLAY:
			e = new jlabsDMRControllerResultEvent(context, "DMRPlay", event, result, cdsItem);
			break;

		case eNATIVE_DMR_PLAY_STOP:
			e = new jlabsDMRControllerResultEvent(context, "DMRStop", event, result, NULL);
			break;

		default :
			return ;
	}

	if( NULL != e)
	{
		OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
		pDmrInstance->dispatchEvent(e);
	}
}
#endif
