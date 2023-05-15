#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "jlabsCDSSearcher.h"
#include "jlabsCDSSearcheResultEvent.h"
#include "jlabsDMS.h"

typedef enum
{
	eDMP_SERVER_REMOVE = 0,
	eDMP_SERVER_ADD
} DMP_SERVER_Update_e;


#define DMP_ACTION_STOP     0
#define DMP_ACTION_START    1

using namespace Ooif;

list<jlabsCDSSearcher*> jlabsCDSSearcher::instances;
jlabsCDSSearcher *jlabsCDSSearcher::m_sDMSSearcher = NULL;
jlabsCDSSearcher::jlabsCDSSearcher() : EventTarget()
{
	ENTRY;
	OOIFNumber retval;
	m_sDMSSearcher = this;
	classType = CLASS_JLABS_CDSSEARCHER;
	OOIF_LOG_DEBUG("\ndebug  ", "(*) jlabsDMS() Class Create!!\n");
	if ( 0 == instances.size() )
	{
		native_jlabsCDSSearchRegisterEventHandler(jlabsCDSSearcher::callbackEventHandler);
		native_jlabsDMPAction( DMP_ACTION_START, &retval );
	}
	instances.push_back(this);
	//OOIF_LOG_PRINT ("(*) jlabsCDSSearcher Create() address : 0x%0x, count : %d\n",  this, instances.size() );
}

jlabsCDSSearcher::~jlabsCDSSearcher()
{
	ENTRY;
	//native_jlabsDMPAction( DMP_ACTION_STOP, &retval );
	OOIF_LOG_DEBUG("\ndebug  ", "(*) jlabsDMS() Class Destroy!!\n");

	instances.remove(this);
	if ( 0 == instances.size() )
	{
		native_jlabsCDSSearchUnRegisterEventHandler();
	}
	//OOIF_LOG_PRINT ("(*) jlabsCDSSearcher Destroy() address : 0x%0x, count : %d\n",  this, instances.size() );
}

int jlabsCDSSearcher::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;

	GET_FUNCTION(CDSSearch, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(DMSAttached);
	INTRINSIC_EVENT_GET(DMSDetached);

	return JSP_GET_NOTFOUND;
}

int jlabsCDSSearcher::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(DMSAttached);
	INTRINSIC_EVENT(DMSDetached);
	return JSP_PUT_NOTFOUND;
}

//static
int jlabsCDSSearcher::CDSSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#if 0
	/* Not yet used  - About timeout  */
	CHECK_ARGUMENT_COUNT(1);
	int nTimeout = NUMBER_VALUE(argv[0]);
#endif

	unsigned int nDmpCount = 0;
	jlabsDMSDeviceHandle *DMSHandle = NULL, handle = NULL;
	jlabsDMSCollection *collection = new jlabsDMSCollection;
	jlabsDMS *dms= NULL;

	if(native_jlabsGetDMSList(&nDmpCount, &handle) == OOIF_RETURN_OK)
	{
		DMSHandle = (jlabsDMSDeviceHandle *)handle;

		for (unsigned int i = 0; i< nDmpCount; i++ )
		{
			dms = new jlabsDMS(DMSHandle[i]);
			collection->addItem(dms);
		}
	}

	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

//static
void jlabsCDSSearcher::callbackEventHandler( int event, int size,  jlabsDMSDeviceHandle dmsList)
{
	jlabsCDSSearcher *dmsSearcher = NULL;

	jsplugin_obj *context = NULL;

	Event *e = NULL;

	for ( list<jlabsCDSSearcher *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		dmsSearcher = (*it);
		context = dmsSearcher->getHost(); 
		if ( NULL == context )
		{
			continue;
		}

		switch( event )
		{
			case eDMP_SERVER_ADD :
				e = new jlabsCDSSearcheResultEvent( context, "DMSAttached", size, dmsList);
				break;

			case eDMP_SERVER_REMOVE :
				e = new jlabsCDSSearcheResultEvent( context, "DMSDetached", size, dmsList);
				break;
			default :
				return ;
		}

		if ( e != NULL )
		{
			dmsSearcher->dispatchEvent(e);
		}
	}
}
#endif
