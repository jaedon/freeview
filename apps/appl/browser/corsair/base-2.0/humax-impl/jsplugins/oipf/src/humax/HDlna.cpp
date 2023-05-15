/*************************************************************************************************************
	File 		: HDlna.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (징째Humax징짹) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/


#ifdef OIPF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HDlna.h"
#include "hlib.h"

using namespace Ooif;

#define RELEASE_HANDLE(x)		\
		if(NULL != x){			\
			HLIB_STD_MemFree(x);	\
			x = NULL;			\
		}						\

/*******************************************************************/
/********************      		HDlna      *************************/
/*******************************************************************/
HDlna *HDlna::m_pDlnaInstance = NULL;

HDlna::HDlna()
{
	ENTRY;
	classType = CLASS_DLNA;
	m_pDlnaInstance = this;
}

HDlna::~HDlna()
{
	ENTRY;
}
int HDlna::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("dmp", obj, new HDlnaDmp(), JSP_GET_VALUE );
	GET_OBJECT("dms", obj, new HDlnaDms(), JSP_GET_VALUE );
	GET_OBJECT("dmr", obj, new HDlnaDmr(), JSP_GET_VALUE );

	return JSP_GET_NOTFOUND;
}

int HDlna::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{

	return JSP_PUT_NOTFOUND;
}

/*******************************************************************/
/********************      		HDlnaDms   *************************/
/*******************************************************************/
HDlnaDms *HDlnaDms::m_pHDlnaDmsInstance;

HDlnaDms::HDlnaDms() : EventTarget()
{
	ENTRY;
	classType = CLASS_DLNADMS;
	m_pHDlnaDmsInstance = this;
}

HDlnaDms::~HDlnaDms()
{
	ENTRY;
	m_pHDlnaDmsInstance = NULL;
}

int HDlnaDms::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(	start, 				obj, "", 						JSP_GET_VALUE_CACHE);
	GET_FUNCTION(	stop, 				obj, "", 						JSP_GET_VALUE_CACHE);
	GET_BOOLEAN(	"started",			native_Dms_IsActivated(), 		JSP_GET_VALUE);
	GET_STRING(		"friendlyName",		native_Dms_GetFriendlyName(),	JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HDlnaDms::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("friendlyName", native_Dms_SetFriendlyName(STRING_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}
int	HDlnaDms::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Dms_Start(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDlnaDms::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Dms_Stop(&retval);

	RETURN_BOOLEAN(!retval, JSP_CALL_VALUE);
}
/*******************************************************************/
/********************      		HDlnaDmr   *************************/
/*******************************************************************/
HDlnaDmr *HDlnaDmr::m_pHDlnaDmrInstance;
list<HDlnaDmr*> HDlnaDmr::instances;

HDlnaDmr::HDlnaDmr() : EventTarget()
{
	ENTRY;
	classType = CLASS_DLNADMR;
	m_pHDlnaDmrInstance = this;
	HDlnaDmr::instances.push_back(this);
	native_Dmr_RegEventListener(HDlnaDmr::callbackEventHandler);
}

HDlnaDmr::~HDlnaDmr()
{
	ENTRY;
	m_pHDlnaDmrInstance = NULL;
	HDlnaDmr::instances.remove(this);
	if ( 0 == HDlnaDmr::instances.size() )
	{
		native_Dmr_UnRegEventListener();
	}
}

int HDlnaDmr::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	/* Properties - Methodes */
	GET_FUNCTION(start, 			obj, 					"", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 				obj, 					"", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setMediaChange, 	obj, 					"s", 	JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("started",			native_Dmr_IsStarted(), 		JSP_GET_VALUE);
	GET_BOOLEAN("activated",		native_Dmr_IsActivated(), 		JSP_GET_VALUE);
	GET_STRING(	"friendlyName",		native_Dmr_GetFriendlyName(),	JSP_GET_VALUE);	/* dmr's friendlyname? or dms's friendlyname?*/

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ReqPlay);
	INTRINSIC_EVENT_GET(ReqStop);
	INTRINSIC_EVENT_GET(ReqPause);
	INTRINSIC_EVENT_GET(ReqSeek);
	INTRINSIC_EVENT_GET(ReqSetVolume);
	INTRINSIC_EVENT_GET(ReqMute);

	return JSP_GET_NOTFOUND;
}

int HDlnaDmr::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ReqPlay);
	INTRINSIC_EVENT(ReqStop);
	INTRINSIC_EVENT(ReqPause);
	INTRINSIC_EVENT(ReqSeek);
	INTRINSIC_EVENT(ReqSetVolume);
	INTRINSIC_EVENT(ReqMute);
	NATIVE_SET("friendlyName", native_Dmr_SetFriendlyName(STRING_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

int	HDlnaDmr::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Dmr_Start(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDlnaDmr::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Dmr_Stop(&retval);

	RETURN_BOOLEAN(!retval, JSP_CALL_VALUE);

}
int HDlnaDmr::setMediaChange(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;

	char szURI[NATIVE_MAX_URI]			= {0};
	memset(szURI, '\0', sizeof(szURI));
	strncpy( szURI, 	STRING_VALUE(argv[0]), sizeof(szURI)-1);

	native_Dmr_SetMediaChange(&retval, szURI);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void HDlnaDmr::callbackEventHandler( unsigned int event, unsigned int result, CdsContentHandle cdsItem )
{
	ENTRY;
	OOIF_LOG_DEBUG("[callbackEventHandler] received event [%d]\n", event);
	for (list<HDlnaDmr*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		HDlnaDmr *pDmrInstance = (*it);

		if ( NULL == pDmrInstance )
		{
			OOIF_LOG_DEBUG("HDlnaDmr has Null instance !!!! \n");
			return ;
		}

		context = pDmrInstance->getHost();
		OOIF_LOG_DEBUG("callbackEventHandler - context(%p), m_pHDlnaDmrInstance(%p), cdsHandle = %p\n", context, pDmrInstance, cdsItem);

		if(context){
			switch( event )
			{
				case eNATIVE_DMR_REQ_PLAY:
					e = new HDlnaDmrResultEvent(context, "ReqPlay", 		event, result, cdsItem);
					break;
				case eNATIVE_DMR_REQ_STOP:
					e = new HDlnaDmrResultEvent(context, "ReqStop", 		event, result);
					break;
				case eNATIVE_DMR_REQ_PAUSE:
					e = new HDlnaDmrResultEvent(context, "ReqPause", 		event, result);
					break;
				case eNATIVE_DMR_REQ_SEEK:
					e = new HDlnaDmrResultEvent(context, "ReqSeek", 		event, result);
					break;
				case eNATIVE_DMR_REQ_SETVOLUME:
					e = new HDlnaDmrResultEvent(context, "ReqSetVolume", 	event, result);
					break;
				case eNATIVE_DMR_REQ_SETMUTE:
					e = new HDlnaDmrResultEvent(context, "ReqMute", 		event, result);
					break;
				case eNATIVE_DMR_REQ_BRINGIN:
					e = new HDlnaDmrResultEvent(context, "ReqBringIn", 		event, result);
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
	}
}


/*******************************************************************/
/************    		HDlnaDmrResultEvent   *********************/
/*******************************************************************/
HDlnaDmrResultEvent::HDlnaDmrResultEvent( jsplugin_obj *c, const char *eventName, int event, int result, CdsDeviceHandle handleId )
															: Event( c, eventName )
{
	m_nEvent = event;
	m_nResult = result;
	m_handleId = handleId;
	ENTRY;
	OOIF_LOG_DEBUG("HDlnaDmpScanResultEvent name(%s), event(%d), result(%d), handleId(%p)\n", eventName, event, result, handleId);
}

HDlnaDmrResultEvent:: ~HDlnaDmrResultEvent()
{
	ENTRY;

}

int HDlnaDmrResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("item", obj, new HDlnaCdsContent(m_handleId), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HDlnaDmrResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HDlnaDmrResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;
	switch(m_nEvent)
	{
		case eNATIVE_DMR_REQ_PLAY:
			{
				RETURN_PARAM_COUNT = 2;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				SET_SERIALIZED_OBJECT( new HDlnaCdsContent(m_handleId), 0 );
				result[1].type = JSP_TYPE_NUMBER;
				result[1].u.number = m_nResult;
			}
			break;
		case eNATIVE_DMR_REQ_SEEK:				// m_nResult => position
		case eNATIVE_DMR_REQ_SETVOLUME:		// m_nResult => volume
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				result[0].type = JSP_TYPE_NUMBER;
				result[0].u.number = m_nResult;
			}
			break;
		case eNATIVE_DMR_REQ_STOP:				// m_nResult => fail or success?
		case eNATIVE_DMR_REQ_PAUSE:			// m_nResult => fail or success?
		case eNATIVE_DMR_REQ_SETMUTE:			// m_nResult => fail or success?
		case eNATIVE_DMR_REQ_BRINGIN:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				result[0].type = JSP_TYPE_BOOLEAN;
				result[0].u.number = m_nResult;
			}
			break;
		default :
			break;
	}
	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/********************      		HDlnaDmp   *************************/
/*******************************************************************/
HDlnaDmp *HDlnaDmp::m_pHDlnaDmpInstance;
list<HDlnaDmp*> HDlnaDmp::instances;
int HDlnaDmp::m_nState = 0;

HDlnaDmp::HDlnaDmp() : EventTarget()
{
	ENTRY;
	classType = CLASS_DLNADMP;
	m_pHDlnaDmpInstance = this;
	HDlnaDmp::instances.push_back(this);
	native_Dmp_CdsScanRegisterEventHandler(HDlnaDmp::callbackEventHandler);
}

HDlnaDmp::~HDlnaDmp()
{
	ENTRY;
	m_pHDlnaDmpInstance = NULL;
	HDlnaDmp::instances.remove(this);
	if ( 0 == HDlnaDmp::instances.size() )
	{
		native_Dmp_CdsScanUnRegisterEventHandler();
	}
}

int HDlnaDmp::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	/* Properties - Methodes */
	GET_FUNCTION(startScan, 			obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, 				obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(refreshDms, 			obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAvailableCdsList, 	obj, "", 	JSP_GET_VALUE_CACHE);

	if ( 0 == strcmp("state", name) )
	{
		RETURN_NUMBER( HDlnaDmp::m_nState, JSP_GET_VALUE );
	}

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(CDSScan);
	INTRINSIC_EVENT_GET(CDSSearch);

	return JSP_GET_NOTFOUND;
}

int HDlnaDmp::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(CDSScan);
	INTRINSIC_EVENT(CDSSearch);

	return JSP_PUT_NOTFOUND;
}

int	HDlnaDmp::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Dmp_Start(&retval);
	HDlnaDmp::m_nState = 1;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


int	HDlnaDmp::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	native_Dmp_Stop(&retval);
	HDlnaDmp::m_nState= 0;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDlnaDmp::refreshDms(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	char szUDN[NATIVE_MAX_UDN]			= {0, };
	OOIFBoolean retval = TRUE;
	memset(szUDN, '\0', sizeof(szUDN));
	strncpy( szUDN, 	STRING_VALUE(argv[0]), sizeof(szUDN)-1);
	native_Dmp_RefreshDms(&retval, szUDN);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}

int	HDlnaDmp::getAvailableCdsList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFNumber retval = TRUE;
	unsigned int nCdsCount = 0;
	CdsDeviceHandle *pDMSHandle, pCdsDevice = NULL;

	native_Dmp_GetAvailableCdsList(&retval, &nCdsCount, &pCdsDevice);
	pDMSHandle = (CdsDeviceHandle*) pCdsDevice;

	HDlnaCdsCollection *collection = new HDlnaCdsCollection;

	if(pDMSHandle)
	{
		for (unsigned int i=0; i<nCdsCount; i++) {
			collection->addItem(new HDlnaCds((CdsDeviceHandle)pDMSHandle[i]));
		}
		HLIB_STD_MemFree(pDMSHandle);
		pDMSHandle = NULL;
	}
	RETURN_OBJECT(this_obj, collection ,result, JSP_CALL_VALUE);
}

//static
void HDlnaDmp::callbackEventHandler( int event, int size, CdsDeviceHandle cdsItem )
{
	ENTRY;
	for (list<HDlnaDmp*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		HDlnaDmp *pDmpInstance = (*it);

		if ( NULL == pDmpInstance )
		{
			OOIF_LOG_DEBUG("HDlnaDmp has Null instance !!!! \n");
			return ;
		}

		context = pDmpInstance->getHost();
		OOIF_LOG_DEBUG("callbackEventHandler - context(%p), m_pHDlnaDmpInstance(%p), cdsItem = %p\n", context, m_pHDlnaDmpInstance, cdsItem);

		if(context){
			switch( event )
			{	// To Debug
				case eDMS_SCAN_STARTED:
				case eDMS_SCAN_COMPLETE:
				case eDMS_SCAN_ADD:
				case eDMS_SCAN_REMOVE:
				case eDMS_SCAN_FAILED:
					e = new HDlnaDmpScanResultEvent(context, "CDSScan", event, size, cdsItem);
					break;
				default :
					return ;
			}
			if( NULL != e)
			{
				OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
				pDmpInstance->dispatchEvent(e);
			}
		}
	}
}


/*******************************************************************/
/************    		HDlnaDmpScanResultEvent   *********************/
/*******************************************************************/
HDlnaDmpScanResultEvent::HDlnaDmpScanResultEvent( jsplugin_obj *c, const char *eventName, int state, int size, CdsDeviceHandle handleId )
															: Event( c, eventName )
{
	m_nHandleCount = size;
	m_nState = state;
	m_handleId = handleId;

	ENTRY;
	OOIF_LOG_DEBUG("HDlnaDmpScanResultEvent name(%s), state(%d), size(%d), handleId(%p)\n", eventName, state, size, handleId);
}

HDlnaDmpScanResultEvent:: ~HDlnaDmpScanResultEvent()
{
	ENTRY;

}

int HDlnaDmpScanResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("cds", 	obj, new HDlnaCds(m_handleId), JSP_GET_VALUE);
    GET_NUMBER("state", m_nState, JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HDlnaDmpScanResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HDlnaDmpScanResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 2;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT( new HDlnaCds(m_handleId), 0 );

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nState;

	return RETURN_PARAM_COUNT;
}


/*******************************************************************/
/*********************    		HDlnaCds   **************************/
/*******************************************************************/
list<HDlnaCds*> HDlnaCds::instances;
HDlnaCds *HDlnaCds::m_pHDlnaCdsInstance = NULL;

HDlnaCds::HDlnaCds(CdsDeviceHandle id) : EventTarget(), WrapperObject<HDlnaCds>(id)
{
	ENTRY;
	classType = CLASS_DLNACDS;

	m_nLastReqId = 0;

	if ( 0 == HDlnaCds::instances.size() )
	{
		native_Cds_RegisterEventHandler((DlnaCdsContentEventListener)HDlnaCds::callbackEventHandler);
	}
	HDlnaCds::instances.push_back(this);
	OOIF_LOG_DEBUG("[%s] HDlnaCds push_back : %p (total length : %d)\n", __FUNCTION__, this, HDlnaCds::instances.size());

}

HDlnaCds::HDlnaCds(const HDlnaCds &original ) : WrapperObject<HDlnaCds>(original.identifier)
{
	ENTRY;
	classType = CLASS_DLNACDS;

	//m_nLastReqId = original.m_nLastReqId;

	if ( 0 == HDlnaCds::instances.size() )
	{
		native_Cds_RegisterEventHandler((DlnaCdsContentEventListener)HDlnaCds::callbackEventHandler);
	}
	HDlnaCds::instances.push_back(this);
	OOIF_LOG_DEBUG("[%s] HDlnaCds(clone) push_back : %p (total length : %d)\n", __FUNCTION__, this, HDlnaCds::instances.size());
}

HDlnaCds::~HDlnaCds()
{
	ENTRY;
	HDlnaCds::instances.remove(this);
	OOIF_LOG_DEBUG("[%s] HDlnaCds remove : %p (total length : %d)\n", __FUNCTION__, this, HDlnaCds::instances.size());

	if ( 0 == HDlnaCds::instances.size() )
	{
	native_Cds_UnRegisterEventHandler();
}
}

int HDlnaCds::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("udn", 			native_Cds_GetUDN(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING("friendlyName", 	native_Cds_GetFriendlyName(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING("ipAddress", 		native_Cds_GetIpAddress(identifier, &RETVAL), 		JSP_GET_VALUE);

	GET_FUNCTION(addEventListener, 		obj, "sob", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(browseContent, 		obj, "ssnn", 	JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(ContentBrowse);

	GET_FUNCTION(searchContent, 		obj, "ssssnn", 	JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(ContentSearch);

	return JSP_GET_NOTFOUND;
}

int HDlnaCds::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ContentBrowse);
	INTRINSIC_EVENT(ContentSearch);
	return JSP_PUT_NOTFOUND;
}

void HDlnaCds::releaseNative( CdsDeviceHandle id)
{
	ENTRY;
	if(NULL != id)
	{
		NATIVE_WARN_NOK(native_Cds_ReleaseHandle(id));
	}
}

int	HDlnaCds::browseContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	HDlnaCds	*cdsDevice = EXTRACT_OBJECT_OR_THROW( cdsDevice, this_obj, HDlnaCds );
	m_pHDlnaCdsInstance = cdsDevice;

	OOIFNumber retval 						= TRUE;
	char szCid[NATIVE_MAX_CID_LEN]			= {0, };
	char szSort[NATIVE_MAX_SORT_LEN]		= {0, };
	int nStartIdx							= 0;
	int nReqItemCount 						= 0;

	memset(szCid, '\0', sizeof(szCid));
	memset(szSort, '\0', sizeof(szSort));

	strncpy( szCid, 	STRING_VALUE(argv[0]), sizeof(szCid)-1);
	strncpy( szSort, 	STRING_VALUE(argv[1]), sizeof(szSort)-1);

	nStartIdx		=	NUMBER_VALUE(argv[2]);
	nReqItemCount 	= 	NUMBER_VALUE(argv[3]);

	if(NULL != cdsDevice)
	{
		OOIF_LOG_DEBUG("cdsDevice : %p ,    cid : %s, sort : %s, startIdx : %d,  pageNum : %d\n", cdsDevice, szCid, szSort, nStartIdx, nReqItemCount );
		native_Cds_BrowseContent(cdsDevice->identifier, szCid, szSort, nStartIdx, nReqItemCount, &retval);
	}

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

#if CDS_SEARCH
int	HDlnaCds::searchContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(6);
	HDlnaCds	*cdsDevice = EXTRACT_OBJECT_OR_THROW( cdsDevice, this_obj, HDlnaCds );

	OOIFNumber retval 						= 0;
	const char *pCid = NULL;
	const char *pCategory = NULL;
	const char *pKeyword = NULL;
	const char *pSort = NULL;
	int nStartIdx							= 0;
	int nReqItemCount 						= 0;

	pCid = STRING_VALUE(argv[0]);
	pCategory = STRING_VALUE(argv[1]);
	pKeyword = STRING_VALUE(argv[2]);
	pSort = STRING_VALUE(argv[3]);

	nStartIdx		=	NUMBER_VALUE(argv[4]);
	nReqItemCount 	= 	NUMBER_VALUE(argv[5]);

	if(NULL != cdsDevice)
	{
		OOIF_LOG_DEBUG("cdsDevice : %p ,  category : %s, keyword : %s, sort : %s, startIdx : %d,  pageNum : %d\n", cdsDevice, pCategory, pKeyword, pSort, nStartIdx, nReqItemCount );
		native_Cds_SearchContent(cdsDevice->identifier, pCid, pCategory, pKeyword, pSort, nStartIdx, nReqItemCount, &retval);
		cdsDevice->m_nLastReqId = retval;
	}

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#endif

void HDlnaCds::callbackEventHandler( int event, int reqId, int size, CdsContentHandle *contentList, char *parentId, int total )
{
	ENTRY;

	HBOOL bEventSend = FALSE;

	for (list<HDlnaCds*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		HDlnaCds *pCdsDevice = (*it);
		jsplugin_obj *context = NULL;

		if ( NULL == pCdsDevice )
		{
			OOIF_LOG_DEBUG("HDlnaCds has Null instance !!!! \n");
			return ;
		}

		context = pCdsDevice->getHost();
		if ( NULL == context )
		{
			continue;
		}

		if ( eCDS_SEARCH_DONE == event || eCDS_SEARCH_ERROR == event )
		{
			if ( reqId != pCdsDevice->m_nLastReqId )
		{
				continue;
			}

			e = new HDlnaCdsSearchResultEvent( context, "ContentSearch", event, reqId, size, contentList, parentId, total );
			if( NULL != e)
		{
				bEventSend = TRUE;
				OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pCdsDevice, e);
				pCdsDevice->dispatchEvent(e);
			}
		}
		else if(m_pHDlnaCdsInstance == pCdsDevice)
			{
				switch( event )
				{
					case eCDS_BROWSE_STARTED:
					case eCDS_CONTENT_ADD 	:
					case eCDS_CONTENT_END 	:
					case eCDS_BROWSE_ERROR	:
						e = new HDlnaCdsScanResultEvent( context, "ContentBrowse", event, size, contentList, parentId );
						break;
				case eCDS_SEARCH_DONE :
				case eCDS_SEARCH_ERROR :
					e = new HDlnaCdsSearchResultEvent( context, "ContentSearch", event, reqId, size, contentList, parentId, total );
					break;
					default :
						return ;
				}
				if( NULL != e)
				{
				bEventSend = TRUE;
				OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)\n", pCdsDevice, e);
					pCdsDevice->dispatchEvent(e);
				}
			}

	}

	/* 하나의 Instacne 라도 dispatchEvent를 수행 했으며, JavaScript에서 사용후
	 * 메모리 해제가 되지만, 그렇지 않을 경우 계속 메모리가 남아 있는 경우 존재 */
	if ( (TRUE != bEventSend) && contentList )
	{
		HINT32 i = 0;
		CdsContentHandle hContentId = NULL;
		OOIF_LOG_DEBUG("\nHDlnaCds does not send Event to JS Instance!! contentList(%p), size(%d)\n", contentList, size );
		for( i = 0; i < size; i++ )
		{
			hContentId = contentList[i];
			native_CdsContent_ReleaseHandle(hContentId);
			hContentId = NULL;
		}
		HLIB_STD_MemFree( contentList );
		contentList = NULL;
	}
}

/*******************************************************************/
/************    		HDlnaCdsScanResultEvent   *********************/
/*******************************************************************/
#define SAFE_DELETE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}
HDlnaCdsScanResultEvent::HDlnaCdsScanResultEvent( jsplugin_obj *c, const char *eventName, int state, int size, CdsContentHandle *handleId, char *parentId )
															: Event( c, eventName )
{
	m_pszParentId = NULL;
	m_nHandleCount = size;
	m_handleId = handleId;
	m_nState = state;

	if(eCDS_BROWSE_STARTED == state)
	{
		m_pszParentId = HLIB_STD_StrDup(parentId);
	}
	else if(eCDS_CONTENT_ADD == state && 0 != size)
	{
		char *temp = NULL;
		native_CdsContent_GetParentID(handleId[0], (const char**)&temp);
		m_pszParentId = HLIB_STD_StrDup(temp);
		OOIF_LOG_DEBUG("[%s] state : %d, size : %d,  parentID : %s, %p \n", __FUNCTION__, state, size, m_pszParentId, m_pszParentId);
	}
	else
	{
		m_pszParentId = HLIB_STD_StrDup("");
	}

#if 0 // to debug
	if(eCDS_CONTENT_ADD == state)
	{
		for(int i=0; i<size; i++)
		{
			char *pszTemp;
			native_CdsContent_GetObjectID(handleId[i], (const char**)&pszTemp);
			OOIF_LOG_DEBUG("[%s] %d.current ObjId : %s \n", __FUNCTION__, i, pszTemp);
		}
	}
#endif
	OOIF_LOG_DEBUG("HDlnaCdsScanResultEvent - event[%s] state[%d] size[%d] handleid[%p]", eventName, state, size, handleId);
}
HDlnaCdsScanResultEvent:: ~HDlnaCdsScanResultEvent()
{
	ENTRY;
	SAFE_DELETE(m_pszParentId);
}

int HDlnaCdsScanResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("contents", obj, GetContentCollection(), JSP_GET_VALUE);
    GET_NUMBER("state", m_nState, JSP_GET_VALUE);
	GET_STRING("parentID", m_pszParentId, JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HDlnaCdsScanResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

HDlnaCdsContentCollection* HDlnaCdsScanResultEvent::GetContentCollection()
{
	ENTRY;
	HDlnaCdsContentCollection *collection = new HDlnaCdsContentCollection;
	HDlnaCdsContent *content = NULL;
	for ( int i = 0; i< m_nHandleCount; i++ )
	{
		content = new HDlnaCdsContent(m_handleId[i]);
		collection->addItem( content  );
	}
	return collection;
}

int HDlnaCdsScanResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 3;

	int nCdsContentCount = m_nHandleCount;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	HDlnaCdsContentCollection *collection = new HDlnaCdsContentCollection;
	HDlnaCdsContent *content = NULL;

	for ( int i = 0; i< nCdsContentCount; i++ )
	{
		content = new HDlnaCdsContent(m_handleId[i]);
		collection->addItem( content  );
	}
	SET_SERIALIZED_OBJECT( collection, 0 );

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nState;

	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = m_pszParentId;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/************    		HDlnaCdsSearchResultEvent   *********************/
/*******************************************************************/
//#define SAFE_DELETE(p)			{if(p){ HxSTD_MemFree(p); (p)=NULL;}}
HDlnaCdsSearchResultEvent::HDlnaCdsSearchResultEvent( jsplugin_obj *c, const char *eventName, int state, int reqId, int size, CdsContentHandle *handleId, char *parentId, int total )
															: Event( c, eventName )
{
	m_nHandleCount = size;
	m_handleId = handleId;
	switch ( state )
	{
		case eCDS_SEARCH_DONE :
			m_nState = 0;
			break;
		case eCDS_SEARCH_ERROR :
		default :
			m_nState = 1;
			break;
	}

	m_nTotalCount = total;
	m_nRequestId = reqId;

	OOIF_LOG_DEBUG("HDlnaCdsSearchResultEvent - event[%s] state[%d] size[%d] handleid[%p]", eventName, state, size, handleId);
}
HDlnaCdsSearchResultEvent:: ~HDlnaCdsSearchResultEvent()
{
	ENTRY;
}

int HDlnaCdsSearchResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("contents", obj, GetContentCollection(), JSP_GET_VALUE);
    GET_NUMBER("state", m_nState, JSP_GET_VALUE);
    GET_NUMBER("searchId", m_nRequestId, JSP_GET_VALUE);
    GET_NUMBER("totalCount", m_nTotalCount, JSP_GET_VALUE);
	//GET_STRING("parentID", m_pszParentId, JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HDlnaCdsSearchResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

HDlnaCdsContentCollection* HDlnaCdsSearchResultEvent::GetContentCollection()
{
	ENTRY;
	HDlnaCdsContentCollection *collection = new HDlnaCdsContentCollection;
	HDlnaCdsContent *content = NULL;
	for ( int i = 0; i< m_nHandleCount; i++ )
	{
		content = new HDlnaCdsContent(m_handleId[i]);
		collection->addItem( content  );
	}
	return collection;
}

int HDlnaCdsSearchResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 4;

	int nCdsContentCount = m_nHandleCount;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	HDlnaCdsContentCollection *collection = new HDlnaCdsContentCollection;
	HDlnaCdsContent *content = NULL;


	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nRequestId;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nTotalCount;

	for ( int i = 0; i< nCdsContentCount; i++ )
	{
		content = new HDlnaCdsContent(m_handleId[i]);
		collection->addItem( content  );
	}
	SET_SERIALIZED_OBJECT( collection, 2 );

	result[3].type = JSP_TYPE_NUMBER;
	result[3].u.number = m_nState;

	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/******************    		HDlnaCdsContent   **********************/
/*******************************************************************/
HDlnaCdsContent::HDlnaCdsContent( CdsContentHandle id) : WrapperObject<HDlnaCdsContent>(id)
{
	ENTRY;
	classType = CLASS_DLNACDS_CONTENT;

}
HDlnaCdsContent::HDlnaCdsContent(const HDlnaCdsContent &original) : WrapperObject<HDlnaCdsContent>(original.identifier)
{
	ENTRY;
	classType = CLASS_DLNACDS_CONTENT;

}

HDlnaCdsContent::~HDlnaCdsContent()
{
	ENTRY;

}

int HDlnaCdsContent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING( "objectID", 				native_CdsContent_GetObjectID(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "parentID", 				native_CdsContent_GetParentID(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "class", 				native_CdsContent_GetClass(identifier, &RETVAL), 				JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("restricted", 			native_CdsContent_GetRestricted(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "objectType", 			native_CdsContent_GetObjectType(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "importURI", 			native_CdsContent_GetImportURI(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "thumbnailUrl", 			native_CdsContent_GetThumbnailURI(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER( "byteSeekAble", 			native_CdsContent_GetByteSeekAble(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_NUMBER( "timeSeekAble", 			native_CdsContent_GetTimeSeekAble(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "title", 				native_CdsContent_GetTitle(identifier, &RETVAL), 				JSP_GET_VALUE);
	NATIVE_GET_STRING( "date", 					native_CdsContent_GetDate(identifier, &RETVAL), 				JSP_GET_VALUE);
	NATIVE_GET_STRING( "duration",				native_CdsContent_GetDuration(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "resolution", 			native_CdsContent_GetResolution(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "digitalCopyControl", 	native_CdsContent_GetDigitalCopyControl(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "genre", 				native_CdsContent_GetGenre(identifier, &RETVAL), 				JSP_GET_VALUE);
	NATIVE_GET_NUMBER( "childCount", 			native_CdsContent_GetChildCount(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "album", 				native_CdsContent_GetAlbum(identifier, &RETVAL), 				JSP_GET_VALUE);
	NATIVE_GET_STRING( "artist", 				native_CdsContent_GetArtist(identifier, &RETVAL), 				JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}
int HDlnaCdsContent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

void HDlnaCdsContent::releaseNative( CdsContentHandle id)
{
	ENTRY;
	if(NULL != id)
	{
		NATIVE_WARN_NOK(native_CdsContent_ReleaseHandle(id));
	}
}

#endif // OIPF

