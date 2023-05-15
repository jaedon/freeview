/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

//#ifdef CORSAIR_DSMCC_DUMMY_IMPLEMENTATION
#include <iostream>

#include "dsmcc_api.h"
#include "corsair.h"
#include "corsair_dsmcc_manager.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_hbbtv_action.h"
#include "corsair_tools.h"

#include "bplglobal.h"

void dsmcc_start()
{
	LOG_DEBUG(" (+)\n");
	CorsairDsmccManager::getInstance()->start( );
	LOG_DEBUG(" (-)\n");
};

void dsmcc_stop()
{
	LOG_DEBUG(" (+)\n");
	CorsairDsmccManager::getInstance()->stop();
	LOG_DEBUG(" (-)\n");
};

DSMCC_STATUS dsmcc_get_file(const char* filename, void* identifier)
{
	LOG_DEBUG(" (+)\n");

	int ret = CorsairDsmccManager::getInstance()->getFile( filename, identifier );
    
	if( 0 < ret  )
    {
        LOG_DEBUG(" (-) DSMCC_WAIT\n");
		return DSMCC_WAIT;
	}
	else if( ret == -1 )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		CorsairTools::mhegOn();
    	CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    	return DSMCC_ERROR;
    }
    
    LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_get_mime_type( void* identifier, char** mime_type)
{
	LOG_DEBUG(" (+)\n");

	if( CorsairDsmccManager::getInstance()->getMimeType( identifier, mime_type ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_read(void* identifier, void** data, int* length)
{
	LOG_DEBUG(" (+)\n");

	if( CorsairDsmccManager::getInstance()->readFile( identifier, data, length ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_close_file(void* identifier)
{
	LOG_DEBUG(" (+)\n");

	if( CorsairDsmccManager::getInstance()->closeFile( identifier ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_clear_cache(void)
{
	LOG_DEBUG(" (+)\n");

	CorsairDsmccManager::getInstance()->clearCache();

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_stream_event_subscribe(const char* targetURL, const char* eventName, void* listenerId, void* listener, void* videoId)
{
	LOG_DEBUG(" (+)\n");

	LOG_DEBUG("\n\ntargetUrl [%s], EventName [%s], ListenerId [0x%08x], listener [0x%08x], videoId [0x%08x]\n\n", targetURL, eventName, listenerId, listener, videoId );

	if( CorsairDsmccManager::getInstance()->subscribeStreamEventListener( targetURL, eventName, listenerId, listener, videoId ) < 0 )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_xml_stream_event_subscribe(const char* targetURL, const char* eventName, unsigned short eventId, unsigned short serviceId, unsigned short componentTag,  void* listenerId, void* listener, void* videoId)
{
	LOG_DEBUG(" (+)\n");

	LOG_DEBUG("\n\ntargetUrl [%s], EventName [%s], eventId [0x%04x], serviceId [0x%04x],\ncomponentTag [0x%04x], listenerId [0x%08x], listener[0x%08x], videoId[0x%08x]\n\n",
			targetURL, eventName, eventId, serviceId, componentTag, listenerId, listener, videoId );

	if( CorsairDsmccManager::getInstance()->subscribeXmlStreamEventListener( targetURL, eventName, eventId, serviceId, componentTag, listenerId, listener, videoId ) < 0 )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_stream_event_unsubscribe(const char* targetURL, const char* eventName, void* listenerId)
{
	LOG_DEBUG(" (+)\n");

	if( CorsairDsmccManager::getInstance()->deleteStreamEventListener( targetURL, eventName, listenerId ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_stream_event_unsubscribe_all(void* videoId)
{
	LOG_DEBUG(" (+)\n");

	if( CorsairDsmccManager::getInstance()->deleteStreamEventListenerAll( videoId ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

DSMCC_STATUS dsmcc_same_carousel(const char* url1, const char* url2, int* match)
{
	LOG_DEBUG(" (+)\n");

	if( !CorsairDsmccManager::getInstance()->isSameCarousel( url1, url2, match ) )
	{
		LOG_DEBUG(" (-) DSMCC_ERROR\n");
		return DSMCC_ERROR;
	}

	LOG_DEBUG(" (-) DSMCC_OK\n");
	return DSMCC_OK;
}

//#endif // CORSAIR_DSMCC_DUMMY_IMPLEMENTATION

