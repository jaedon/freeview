/** **********************************************************************************************************
	@file 		corsair_dsmcc_streameventlistenermanager.h
	
	@date		2012/10/03
	@author		tipmsw1@humaxdigital.com
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*********************************************************************************************************** */

/*************************************************************************************************************
	header
*************************************************************************************************************/
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "hbbtv_api.h"

#include "corsair_log.h"
#include "corsair_dsmcc_streameventlistenermanager.h"

using namespace std;

/*************************************************************************************************************
	CorsairDsmccStreamEventMessage class body
*************************************************************************************************************/
CorsairDsmccStreamEventListenerManager::CorsairDsmccStreamEventListenerManager()
{
	LOG_DEBUG(" (+)\n" );
}

CorsairDsmccStreamEventListenerManager::~CorsairDsmccStreamEventListenerManager()
{
	LOG_DEBUG(" (+)\n" );
	clear();
}

int CorsairDsmccStreamEventListenerManager::sizeOfListener()
{
	return m_listeners.size();
}

int CorsairDsmccStreamEventListenerManager::addListener( CorsairDsmccStreamEventListener *pListener )
{
	std::vector<CorsairDsmccStreamEventListener*>::iterator it;
	
	if ( !pListener )
	{
		return -1;
	}
	
	for ( it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		if ( (*it)->getEventName().compare( pListener->getEventName() ) == 0 && (*it)->getListenerId() == pListener->getListenerId() )
		{
			cout << __FUNCTION__ << "- this listener is already registered" << endl;
			return -1;
		}
	}
	
	m_listeners.push_back( pListener );
	
	return 0;	
}

int CorsairDsmccStreamEventListenerManager::deleteListener( const char* target_url, const char* event_name, void* listener_id )
{
	std::vector<CorsairDsmccStreamEventListener*>::iterator it;

    LOG_DEBUG(" (+) event_name %s listener %p\n", event_name, listener_id );
	
	for ( it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		if ( (*it)->getTargetUrl().compare(target_url) == 0 && 
		     (*it)->getEventName().compare(event_name) == 0 && 
		     (*it)->getListenerId() == listener_id )
		{
			LOG_DEBUG(" (-) Delete\n" );
			delete (*it);
			m_listeners.erase(it);
			return 0;
		}
	}
		
	LOG_DEBUG(" (-)\n" );
	return 1;
}

int CorsairDsmccStreamEventListenerManager::deleteAllListener( void* video_id )
{
	std::vector<CorsairDsmccStreamEventListener*>::iterator it;
	
	LOG_DEBUG(" (+) videoId %p\n", video_id );

	for ( it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		if ( (*it)->getVideoId() == video_id )
		{
			delete (*it);
		}
	}
	
	m_listeners.clear();
	return 0;
}

CorsairDsmccStreamEventListener* CorsairDsmccStreamEventListenerManager::findListenerMatchingId( void *listener_id )
{
	std::vector<CorsairDsmccStreamEventListener*>::iterator it;
	
	LOG_DEBUG(" (+) listener_id %p\n", listener_id );
	
	for ( it = m_listeners.begin(); it != m_listeners.end(); it++ )
	{
		if ( (*it)->getListenerId() == listener_id )
		{
			LOG_DEBUG(" (-) Found\n" ); 
			return (*it);
		}
	}
	
	LOG_DEBUG(" (-)\n" );
	
	return NULL;
}

int CorsairDsmccStreamEventListenerManager::clear( void )
{
	LOG_DEBUG(" (+)\n" );
	
	{
    	std::vector<CorsairDsmccStreamEventListener*>::iterator it;
    	for ( it = m_listeners.begin(); it != m_listeners.end(); ++it)
    	{
    		delete (*it);
    	}
    	m_listeners.clear();
    }
		
	LOG_DEBUG(" (-)\n" );
	
	return 0;
}

void CorsairDsmccStreamEventListenerManager::onStreamEventRecieved( const char *url, unsigned short event_id, const char *event_name, unsigned char *event_data, unsigned int event_data_length)
{
    //LOG_DEBUG(" (+) url %s event_id %x event_name %s event_data %s\n", url, event_id, event_name, (event_data ? (char*)event_data :"null") );
    LOG_DEBUG(" (+)\n");
	std::vector<CorsairDsmccStreamEventListener *>::iterator it_listener;
    for( it_listener = m_listeners.begin(); it_listener != m_listeners.end(); it_listener++ )
    {
        (*it_listener)->onStreamEventRecieved(url, event_id, event_name, event_data, event_data_length);
    }
    
    LOG_DEBUG(" (-)\n" );
}
