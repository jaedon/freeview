/*************************************************************************************************************
	File 		: HAirplay.cpp
	author 		: STB-Component@humaxdigital.com
	comment		:
	date    	: 2013/09/06
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include "HAirplay.h"
#include "hlib.h"

using namespace Ooif;

HAirplay *HAirplay::m_pAirplayInstance;
HAirplayManager *HAirplayManager::m_pAirplayMgrInstance;
list<HAirplay*> HAirplay::instances;


HAirplayManager::HAirplayManager()
{
	ENTRY;
	classType = CLASS_AIRPLAY;
	m_pAirplayMgrInstance = this;
}

HAirplayManager::~HAirplayManager()
{
	ENTRY;
}
int HAirplayManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("airplay", obj, new HAirplay(), JSP_GET_VALUE );
	return JSP_GET_NOTFOUND;
}

int HAirplayManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{

	return JSP_PUT_NOTFOUND;
}

HAirplay::HAirplay() : EventTarget()
{
	ENTRY;
	classType = CLASS_AIRPLAY;
	m_pAirplayInstance = this;
	HAirplay::instances.push_back(this);
	native_Airplay_RegEventListener(HAirplay::callbackEventHandler);
}

HAirplay::~HAirplay()
{
	ENTRY;
	m_pAirplayInstance = NULL;
	HAirplay::instances.remove(this);
	if ( 0 == HAirplay::instances.size() )
	{
		native_Airplay_UnRegEventListener();
	}
}

int HAirplay::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	/* Properties - Methodes */
	GET_FUNCTION(start, 			obj, 					"", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 				obj, 					"", 	JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("started",			native_Airplay_IsStarted(), 		JSP_GET_VALUE);
	GET_BOOLEAN("activated",		native_Airplay_IsActivated(), 		JSP_GET_VALUE);

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ReqPlay);
	INTRINSIC_EVENT_GET(ReqStop);
	INTRINSIC_EVENT_GET(ReqSetPosition);
	INTRINSIC_EVENT_GET(ReqSetSpeed);
	INTRINSIC_EVENT_GET(ReqShowPhoto);
	INTRINSIC_EVENT_GET(ReqAudioPlay);
	INTRINSIC_EVENT_GET(ReqAudioStop);
	INTRINSIC_EVENT_GET(ReqAudioFlush);
	INTRINSIC_EVENT_GET(ReqSetMetadata);
	INTRINSIC_EVENT_GET(ReqSetAlbumart);

	return JSP_GET_NOTFOUND;
}

int HAirplay::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ReqPlay);
	INTRINSIC_EVENT(ReqStop);
	INTRINSIC_EVENT(ReqSetPosition);
	INTRINSIC_EVENT(ReqSetSpeed);
	INTRINSIC_EVENT(ReqShowPhoto);
	INTRINSIC_EVENT(ReqAudioPlay);
	INTRINSIC_EVENT(ReqAudioStop);
	INTRINSIC_EVENT(ReqAudioFlush);
	INTRINSIC_EVENT(ReqSetMetadata);
	INTRINSIC_EVENT(ReqSetAlbumart);
    NATIVE_SET("PlaybackStatus", native_Airplay_SetPlaybackStatusInfo(STRING_VALUE(value[0]))); //"PAUSED","PLAYING","EOF","STOPPED","ERROR"
    NATIVE_SET("BufferedRange", native_Airplay_SetBufferedRangeInfo(NUMBER_VALUE(value[0])));
    NATIVE_SET("SeekableRange", native_Airplay_SetSeekableRangeInfo(NUMBER_VALUE(value[0])));
    NATIVE_SET("Duration", native_Airplay_SetDurationInfo(NUMBER_VALUE(value[0])));
    NATIVE_SET("Position", native_Airplay_SetPositionInfo(NUMBER_VALUE(value[0])));
    NATIVE_SET("Rate", native_Airplay_SetRateInfo(NUMBER_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

int	HAirplay::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Airplay_Start(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HAirplay::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_Airplay_Stop(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}

void HAirplay::callbackEventHandler( unsigned int event, unsigned long long decimal, char* pUrl, char* pPath, char* pTitle, char* pAlbum, char* pArtist )
{
	ENTRY;
	OOIF_LOG_DEBUG("[callbackEventHandler] received event [%d]\n", event);
	for (list<HAirplay*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		HAirplay *pAirplayInstance = (*it);

		if ( NULL == pAirplayInstance )
		{
			OOIF_LOG_DEBUG("HAirplay has Null instance !!!! \n");
			return ;
		}

		context = pAirplayInstance->getHost();
		OOIF_LOG_DEBUG("callbackEventHandler - context(%p), m_pAirplayInstance(%p), event = %d\n", context, pAirplayInstance, event);

		if(context){
			switch( event )
			{
				case eNATIVE_AIRPLAY_REQ_PLAY:
					e = new HAirplayResultEvent(context, "ReqPlay", event, decimal, pUrl);
					break;
				case eNATIVE_AIRPLAY_REQ_STOP:
					e = new HAirplayResultEvent(context, "ReqStop", event, decimal);
					break;
				case eNATIVE_AIRPLAY_REQ_SHOWPHOTO:
					e = new HAirplayResultEvent(context, "ReqShowPhoto", event, decimal, NULL, pPath);
					break;
				case eNATIVE_AIRPLAY_REQ_SETPOSITION:
					e = new HAirplayResultEvent(context, "ReqSetPosition", event, decimal);
					break;
				case eNATIVE_AIRPLAY_REQ_SETRATE:
					e = new HAirplayResultEvent(context, "ReqSetSpeed", event, decimal);
					break;
				case eNATIVE_AIRPLAY_REQ_SETVOLUME:
					e = new HAirplayResultEvent(context, "ReqSetVolume", event, decimal);
					break;
				case eNATIVE_AIRTUNES_REQ_PLAY:
					e = new HAirplayResultEvent(context, "ReqAudioPlay", event, decimal, pUrl);
					break;
				case eNATIVE_AIRTUNES_REQ_STOP:
					e = new HAirplayResultEvent(context, "ReqAudioStop", event, decimal);
					break;
				case eNATIVE_AIRTUNES_REQ_FLUSH:
					e = new HAirplayResultEvent(context, "ReqAudioFlush", event, decimal);
					break;
				case eNATIVE_AIRTUNES_REQ_SETMETADATA:
					e = new HAirplayResultEvent(context, "ReqSetMetadata", event, decimal, NULL, NULL, pTitle, pAlbum, pArtist);
					break;
				case eNATIVE_AIRTUNES_REQ_SETALBUMART:
					e = new HAirplayResultEvent(context, "ReqSetAlbumart", event, decimal, NULL, pPath);
					break;
				default :
					return ;
			}
			if( NULL != e)
			{
				OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
				pAirplayInstance->dispatchEvent(e);
			}
		}
	}
}

HAirplayResultEvent::HAirplayResultEvent( jsplugin_obj *c, const char *eventName, int event, unsigned long long decimal, char* pUrl, char* pPath, char* pTitle, char* pAlbum, char* pArtist )
															: Event( c, eventName )
{
    ENTRY;
    m_nEvent = event;
	m_decimalValue = decimal;
    m_pUrl = NULL;
    m_pPath = NULL;
    m_pTitle = NULL;
    m_pArtist = NULL;
    m_pAlbum = NULL;

    if(pUrl != NULL)
    {
        m_pUrl = HLIB_STD_StrDup(pUrl);
    }
    if(pPath != NULL)
    {
        m_pPath = HLIB_STD_StrDup(pPath);
    }
    if(pAlbum != NULL)
    {
        m_pAlbum = HLIB_STD_StrDup(pAlbum);
    }
    if(pTitle != NULL)
    {
        m_pTitle = HLIB_STD_StrDup(pTitle);
    }
    if(pArtist != NULL)
    {
        m_pArtist = HLIB_STD_StrDup(pArtist);
    }

	OOIF_LOG_DEBUG("HAirplayResultEvent name(%s), event(%d), decimal(%d), url(%p)\n", eventName, event, decimal, m_pUrl);
}

HAirplayResultEvent:: ~HAirplayResultEvent()
{
	ENTRY;
	if(m_pUrl != NULL)
	{
		HLIB_STD_MemFree(m_pUrl);
	}
	if(m_pPath != NULL)
	{
		HLIB_STD_MemFree(m_pPath);
	}
	if(m_pAlbum != NULL)
	{
		HLIB_STD_MemFree(m_pAlbum);
	}
	if(m_pTitle != NULL)
	{
		HLIB_STD_MemFree(m_pTitle);
	}
	if(m_pArtist != NULL)
	{
		HLIB_STD_MemFree(m_pArtist);
	}
}

int HAirplayResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_STRING("url", m_pUrl, JSP_GET_VALUE);
    GET_NUMBER("decimal", m_decimalValue, JSP_GET_VALUE);
    GET_NUMBER("event", m_nEvent, JSP_GET_VALUE);
    GET_STRING("path", m_pPath, JSP_GET_VALUE);
    GET_STRING("title", m_pTitle, JSP_GET_VALUE);
    GET_STRING("album", m_pAlbum, JSP_GET_VALUE);
    GET_STRING("artist", m_pArtist, JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HAirplayResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HAirplayResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;
	switch(m_nEvent)
	{
        case eNATIVE_AIRPLAY_REQ_PLAY:
			{
				RETURN_PARAM_COUNT = 2;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pUrl;
				result[1].type = JSP_TYPE_NUMBER;
				result[1].u.number = m_decimalValue;
			}
			break;
		case eNATIVE_AIRPLAY_REQ_SETPOSITION:	// m_decimalValue => position
		case eNATIVE_AIRPLAY_REQ_SETRATE:		// m_decimalValue => rate
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				result[0].type = JSP_TYPE_NUMBER;
				result[0].u.number = m_decimalValue;
			}
			break;
		case eNATIVE_AIRPLAY_REQ_STOP:			    // m_decimalValue => fail or success?
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				result[0].type = JSP_TYPE_BOOLEAN;
				result[0].u.number = m_decimalValue;
			}
			break;
		case eNATIVE_AIRPLAY_REQ_SHOWPHOTO:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pPath;
			}
			break;
		case eNATIVE_AIRTUNES_REQ_PLAY:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pUrl;
			}
			break;
		case eNATIVE_AIRTUNES_REQ_STOP:
			{
				RETURN_PARAM_COUNT = 0;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
			}
			break;
		case eNATIVE_AIRTUNES_REQ_FLUSH:
			{
				RETURN_PARAM_COUNT = 0;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
			}
			break;
		case eNATIVE_AIRTUNES_REQ_SETMETADATA:
			{
				RETURN_PARAM_COUNT = 3;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pTitle;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pAlbum;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pArtist;
			}
			break;
		case eNATIVE_AIRTUNES_REQ_SETALBUMART:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
                result[0].type = JSP_TYPE_STRING;
                result[0].u.string = m_pPath;
			}
			break;
		default :
			break;
	}
	return RETURN_PARAM_COUNT;
}

#endif // OIPF

