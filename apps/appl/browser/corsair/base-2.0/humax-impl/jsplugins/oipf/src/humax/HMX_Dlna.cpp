// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "HMX_Dlna.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "macros.h"

//#include "NativeHmxDLNA.h"

using namespace Ooif;

typedef enum
{
	eDMP_SERVER_REMOVE = 0,
	eDMP_SERVER_ADD
} DMP_SERVER_Update_e;

typedef enum
{
	eDMP_CONTENT_END = 0,
	eDMP_CONTENT_ADD
} DMP_CONTENT_Update_e;

/***********************************************************************************
 *  HMX_DMSSearcher Class
 ***********************************************************************************
 */
HMX_DMSSearcher *HMX_DMSSearcher::m_sDMSSearcher = NULL;
DMSDeviceHandle HMX_DMSSearcher::m_sCurrentDMSDevice = NULL;
list<HMX_DMSSearcher *> HMX_DMSSearcher::instances;
HMX_DMSSearcher::HMX_DMSSearcher() : EventTarget()
{
	ENTRY;
	m_sDMSSearcher = this;
	classType = CLASS_DMSSEARCHER;
	native_DMSSearchRegisterEventHandler(HMX_DMSSearcher::callbackEventHandler);

	HMX_DMSSearcher::instances.push_back(this);
	OOIF_LOG_DEBUG("\n\t(+) HMX_DMSSearcher() Class Create!!\n");
}

HMX_DMSSearcher::~HMX_DMSSearcher()
{
	ENTRY;
	OOIF_LOG_DEBUG("\n\t(-) HMX_DMSSearcher() Class Destroy!!\n");
	HMX_DMSSearcher::instances.remove(this);

	if ( 0 == HMX_DMSSearcher::instances.size() )
	{
		native_DMSSearchUnRegisterEventHandler();
	}
}

int HMX_DMSSearcher::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;

	GET_FUNCTION(search, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAvailableDMSList, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getCurrentDMS, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(DMSAttached);
	INTRINSIC_EVENT_GET(DMSDetached);
	return JSP_GET_NOTFOUND;
}

int HMX_DMSSearcher::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(DMSAttached);
	INTRINSIC_EVENT(DMSDetached);
	return JSP_PUT_NOTFOUND;
}

//static
int	HMX_DMSSearcher::search(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval = TRUE;
	int actionType = NUMBER_VALUE(argv[0]);
	native_DMPAction( actionType, &retval );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int	HMX_DMSSearcher::getAvailableDMSList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	//TEMP
	//Obj -> Collecton
	RETURN_OBJECT(this_obj, new HMX_DMS(0) ,result, JSP_GET_VALUE);
}

//static
int	HMX_DMSSearcher::getCurrentDMS(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	DMSDeviceHandle identifier = NULL;
	native_DMSSearchGetCurrentDMS( &identifier );
	HMX_DMS *dms = NULL;
	if ( NULL != identifier )
	{
		m_sCurrentDMSDevice = identifier;
		dms = new HMX_DMS(identifier);
		RETURN_OBJECT(this_obj, dms ,result, JSP_GET_VALUE);
	}

	return JSP_GET_VALUE;
}
//static
void HMX_DMSSearcher::callbackEventHandler( int event, int size, DMSDeviceHandle dmsList )
{
	HMX_DMSSearcher *dmsSearcher = m_sDMSSearcher;
	if ( NULL == dmsSearcher )
	{
		return ;
	}

	jsplugin_obj *context = dmsSearcher->getHost();
	Event *e = NULL;
	switch( event )
	{
		case eDMP_SERVER_ADD :
			e = new HMX_DMSSearcherResultEvent( context, "DMSAttached", size, dmsList );
			break;
		case eDMP_SERVER_REMOVE :
			e = new HMX_DMSSearcherResultEvent( context, "DMSDetached", size, dmsList );
			break;
		default :
			return ;
	}

	dmsSearcher->dispatchEvent(e);
}

/***********************************************************************************
 *  HMX_DMS Class
 ***********************************************************************************
 */

//static
HMX_DMS * HMX_DMS::m_sDMSDevice = NULL;
HMX_DMS::HMX_DMS(DMSDeviceHandle id) : EventTarget(), WrapperObject<HMX_DMS>(id)
{
	ENTRY;
	classType = CLASS_DMSDEVICE;
}
HMX_DMS::~HMX_DMS()
{
	ENTRY;
}

int HMX_DMS::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("udn", native_DMSDeviceGetUDN(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("friendlyName", native_DMSDeviceGetFriendlyName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("ipAddress", native_DMSDeviceGetIpAddress(identifier, &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(browseDirectChildren, obj, "sn", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(ContentSearch);
	return JSP_GET_NOTFOUND;
}

int HMX_DMS::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	if ( !strcmp(name, "onContentSearch") )
	{
		native_DMSDeviceRegisterEventHandler( HMX_DMS::callbackEventHandler );
	}
	INTRINSIC_EVENT(ContentSearch);
	return JSP_PUT_NOTFOUND;
}

//static
 void HMX_DMS::releaseNative( DMSDeviceHandle id)
{
	if( HMX_DMSSearcher::GetCurrentDMSDevice()  != id )
	{
		NATIVE_WARN_NOK(native_DMSDeviceReleaseHandle(id));
	}
}

//static
int	HMX_DMS::browseDirectChildren(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	HMX_DMS	*dmsDevice = EXTRACT_OBJECT_OR_THROW( dmsDevice, this_obj, HMX_DMS );
	OOIFNumber retval = TRUE;
	char cid[512];
	char pageNum = 0;

	strncpy( cid, STRING_VALUE(argv[0]), sizeof(cid)-1);
	pageNum = NUMBER_VALUE(argv[1]);

	m_sDMSDevice = dmsDevice;

	OOIF_LOG_DEBUG("cid : %s, pageNum : %d\n", cid, pageNum );
	native_DMSDeviceBrowseDirectChildren( dmsDevice->identifier, cid, pageNum , &retval );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
void HMX_DMS::callbackEventHandler( int event, char *udn, int reqId, int size, DLNAContentHandle *contentList, int totla )
{
	HMX_DMS * dms = NULL;
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
			break;

		default :
			return ;
	}

	e = new HMX_DLNAContentSearchResultEvent( context, event, size, contentList );

	dms->dispatchEvent(e);
}

/***********************************************************************************
 *  HMX_DLNAContent Class
 ***********************************************************************************
 */
HMX_DLNAContent::HMX_DLNAContent( DLNAContentHandle id, DLNAContentMetaHandle metaId ) : WrapperObject<HMX_DLNAContent>(id)
{
	ENTRY;
	m_metaDataHandle = metaId;
}

HMX_DLNAContent::HMX_DLNAContent(const HMX_DLNAContent &original ) : WrapperObject<HMX_DLNAContent>(original.identifier), m_metaDataHandle(original.m_metaDataHandle)
{
	ENTRY;
}

HMX_DLNAContent::~HMX_DLNAContent()
{
	OOIF_LOG_DEBUG("\n(*) HMX_DLNAContent() Class Destroy!!\n");
	ENTRY;
}

int HMX_DLNAContent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("objectID", native_DLNAContentGetObjectID(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("parentID", native_DLNAContentGetParentID(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("class", native_DLNAContentGetClass(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("restricted", native_DLNAContentGetRestricted(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("objectType", native_DLNAContentGetObjectType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("importURI", native_DLNAContentGetImportURI(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbnail", native_DLNAContentGetThumbnailURI(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("title", native_DLNAContentGetTitle(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("date", native_DLNAContentGetDate(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("duration", native_DLNAContentGetDuration(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("resolution", native_DLNAContentGetResolution(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("digitalCopyControl", native_DLNAContentGetDigitalCopyControl(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("genre", native_DLNAContentGetGenre(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("childCount", native_DLNAContentGetChildCount(identifier, &RETVAL), JSP_GET_VALUE);
	// TEMP
	GET_OBJECT("metaData", obj, new HMX_DLNAContentMetaData(m_metaDataHandle), JSP_GET_VALUE );
	return JSP_GET_NOTFOUND;
}

int HMX_DLNAContent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//static
 void HMX_DLNAContent::releaseNative( DLNAContentHandle id)
{
	NATIVE_WARN_NOK(native_DLNAContentReleaseHandle(id));
}

/***********************************************************************************
 *  HMX_DLNAContentMetaData Class
 ***********************************************************************************
 */
HMX_DLNAContentMetaData::HMX_DLNAContentMetaData( DLNAContentMetaHandle id ) : WrapperObject<HMX_DLNAContentMetaData>(id)
{
	ENTRY;
}

HMX_DLNAContentMetaData::~HMX_DLNAContentMetaData()
{
	ENTRY;
}

int HMX_DLNAContentMetaData::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("album", native_DLNAContentMetaGetAlbum(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("artist", native_DLNAContentMetaGetArtist(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMX_DLNAContentMetaData::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}
//static
 void HMX_DLNAContentMetaData::releaseNative( DLNAContentMetaHandle id)
{
	ENTRY;
	/*DLNAContent ?¸ìŠ¤?´ìŠ¤???¬í•¨?˜ì—ˆ?? ?´ì œ ê¸ˆì?*/
}

/***********************************************************************************
 *  HMX_DMSSearcherResultEvent Class
 ***********************************************************************************
 */
HMX_DMSSearcherResultEvent::HMX_DMSSearcherResultEvent( jsplugin_obj *c, const char *eventName, int size, DMSDeviceHandle handleId ) : Event( c, eventName ), m_handleId( handleId ), m_nHandleCount( size)
{
	ENTRY;
}

HMX_DMSSearcherResultEvent::~HMX_DMSSearcherResultEvent()
{
	ENTRY;
}

int HMX_DMSSearcherResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int HMX_DMSSearcherResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int HMX_DMSSearcherResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT( new HMX_DMS(m_handleId), 0 );

	return 1;
}

/***********************************************************************************
 *  HMX_DMSSearcherResultEvent Class
 ***********************************************************************************
 */
HMX_DLNAContentSearchResultEvent::HMX_DLNAContentSearchResultEvent( jsplugin_obj *c, int event, int size, DLNAContentHandle *handleId ) : Event( c, "ContentSearch"), m_handleId( handleId),  m_nState( event ), m_nHandleCount( size )
{
	ENTRY;
}

HMX_DLNAContentSearchResultEvent::~HMX_DLNAContentSearchResultEvent()
{
	ENTRY;
}

int HMX_DLNAContentSearchResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int HMX_DLNAContentSearchResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int HMX_DLNAContentSearchResultEvent::serialize(jsplugin_value **argv)
{
	int count = m_nHandleCount;
	ENTRY;
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	if ( eDMP_CONTENT_END == m_nState )
	{
		return 1;
	}

	DLNAContentMetaHandle	metaHandle =  NULL;
	HMX_DLNAContentCollection *collection = new HMX_DLNAContentCollection;
	HMX_DLNAContent *content = NULL;
	for ( int i = 0; i< count; i++ )
	{
		metaHandle =  native_DLNAContentGetMetaData( m_handleId[i] );

		content = new HMX_DLNAContent( m_handleId[i], metaHandle);
		collection->addItem( content  );
	}

	//SET_SERIALIZED_OBJECT( new HMX_DLNAContent(m_handleId, m_metaHandleId), 0 );
	SET_SERIALIZED_OBJECT( collection, 0 );
	if ( NULL != m_handleId )
	{
		HLIB_STD_MemFree( m_handleId );
		m_handleId = NULL;
	}
	return 1;
}
/*
HMX_DLNAContentCollection * HMX_DLNAContentSearchResultEvent::getContentCollection( int count, DLNAContentHandle * hContents, DLNAContentMetaHandle *hMetaHandles )
{

	return NULL;
}
*/


#endif // OIPF
