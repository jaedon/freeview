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
#include "corsair_dsmcc_xmlstreameventlistener.h"
#include "corsair_dsmcc_uri.h"

using namespace std;

/*************************************************************************************************************
	CorsairDsmccXMLStreamEventListener class body
*************************************************************************************************************/
CorsairDsmccXMLStreamEventListener::CorsairDsmccXMLStreamEventListener( const char* pcTargetURL, const char* pcEventName, unsigned short usEventId, unsigned short usServiceId, 
																        unsigned short usComponentTag, void* pListenerId, void* pListener, void* pVideoId)
    :CorsairDsmccStreamEventListener(pcTargetURL, pcEventName, pListenerId, pListener, pVideoId, XMLStreamEventListener )
    ,m_event_id(usEventId)
    ,m_service_id(usServiceId)
    ,m_component_tag(usComponentTag)
{
	LOG_DEBUG(" (+)\n" );
}

CorsairDsmccXMLStreamEventListener::~CorsairDsmccXMLStreamEventListener()
{
	LOG_DEBUG(" (+)\n" );
}

int CorsairDsmccXMLStreamEventListener::onStreamEventRecieved( const char *target_url, unsigned short event_id, const char *event_name, unsigned char *data, unsigned int data_length)
{
    HBBTV_EventData *hbbtv_event = NULL;

    LOG_DEBUG(" (+) target_url %s event_id %x event_name %s data %s\n", target_url, event_id, event_name, data );
    
	hbbtv_event = (HBBTV_EventData *)malloc(sizeof(HBBTV_EventData));
	if ( !hbbtv_event )
	{
		LOG_DEBUG(" (-) Failed to get allocation1\n" );
		return -1;
	}

	memset(hbbtv_event, 0, sizeof(HBBTV_EventData));

	if( getEventName().size() > 0 )
	{
		hbbtv_event->dsmcc.name = (char *)malloc( getEventName().size() + 1 );
		if( hbbtv_event->dsmcc.name == NULL )
		    goto free_event;
		snprintf( hbbtv_event->dsmcc.name, getEventName().size() + 1, "%s", getEventName().c_str() );
	}
	else if ( event_name != NULL )
	{
	    hbbtv_event->dsmcc.name = (char *)malloc( strlen( (char *)event_name ) + 1 );
	    if( hbbtv_event->dsmcc.name == NULL )
		    goto free_event;
		snprintf( hbbtv_event->dsmcc.name, strlen( (char *)event_name ) + 1, "%s", event_name );
	}

    if( data != NULL )
	{
	    hbbtv_event->dsmcc.data = (char *)malloc( data_length * 2 + 1 );
	    if( hbbtv_event->dsmcc.data == NULL )
		    goto free_event;
	    {
			/* TS 102796 1.2.1, clause 8.2.1.2 "Data of the DSM-CC StreamEvent's event encoded in hexadecimal."
			so, we should make out the Object info to hexadecimal value on hbbtv_event->dsmcc.data */
			int data_i;
			for( data_i = 0; data_i < data_length; data_i++ )
			{
				snprintf( &(hbbtv_event->dsmcc.data[data_i *2]), 3, "%02x", data[data_i]);
			}
			hbbtv_event->dsmcc.data[data_i * 2] = '\0';
		}
		   
	    hbbtv_event->dsmcc.text = (char *)malloc( data_length + 1 );
	    if( hbbtv_event->dsmcc.text == NULL )
		    goto free_event;
		snprintf( hbbtv_event->dsmcc.text, data_length + 1, "%s", data );	    	
	}
	
	{
	    CorsairDsmccUri target_uri(getTargetUrl().c_str()); 
	    string url;
	    if ( target_url )
	        url = target_url;
	        
	    /**
	     @ ToDo this url.append(".xml") is blocked. we need to check Ligada TC.
	     */
    	if ( event_name == NULL ||
    	     getEventName().compare(event_name) != 0 ||
    	     target_url == NULL ||
    	     target_uri.compareFilename(url.c_str()) != 0 )
    	{
    	    hbbtv_event->dsmcc.status = (char *)malloc( strlen( (char *)"error" ) + 1 );
    	    if( hbbtv_event->dsmcc.status == NULL )
    		    goto free_event;
    	    snprintf( hbbtv_event->dsmcc.status, strlen( (char *)"error" ) + 1, "error" );
    	}
    	else
    	{
    	    hbbtv_event->dsmcc.status = (char *)malloc( strlen( (char *)"trigger" ) + 1 );
    	    if( hbbtv_event->dsmcc.status == NULL )
    		    goto free_event;
    	    snprintf( hbbtv_event->dsmcc.status, strlen( (char *)"trigger" ) + 1, "trigger" );
    	}
    }
	
//	hbbtv_event->dsmcc.identifier;
	hbbtv_event->dsmcc.listener_id = getListenerId();
	hbbtv_event->dsmcc.event_listener = getListener();
	hbbtv_event->dsmcc.video_broadcast_handle = getVideoId();

	LOG_DEBUG(" \t identifier             : 0x%08x\n", hbbtv_event->dsmcc.identifier );
	LOG_DEBUG(" \t name                   : %s\n", ( hbbtv_event->dsmcc.name ? hbbtv_event->dsmcc.name : "Null") );
	LOG_DEBUG(" \t data                   : %s\n", ( hbbtv_event->dsmcc.data ? hbbtv_event->dsmcc.data : "Null") );
	LOG_DEBUG(" \t text                   : %s\n", ( hbbtv_event->dsmcc.text ? hbbtv_event->dsmcc.text : "Null") );
	LOG_DEBUG(" \t status                 : %s\n", hbbtv_event->dsmcc.status );
	LOG_DEBUG(" \t listener_id            : 0x%08x\n", hbbtv_event->dsmcc.listener_id );
	LOG_DEBUG(" \t event_listener         : 0x%08x\n", hbbtv_event->dsmcc.event_listener );
	LOG_DEBUG(" \t video_broadcast_handle : 0x%08x\n", hbbtv_event->dsmcc.video_broadcast_handle );

	LOG_DEBUG(" (-) ok\n" );
	
	opera_hbbtv_send_event(E_DSMCC_STREAM_EVENT, hbbtv_event);
	
	return 0;

free_event:
	if (hbbtv_event)
	{
		if ( hbbtv_event->dsmcc.text )	free( hbbtv_event->dsmcc.text );
		if ( hbbtv_event->dsmcc.data )	free( hbbtv_event->dsmcc.data );
		if ( hbbtv_event->dsmcc.name )	free( hbbtv_event->dsmcc.name );
		if ( hbbtv_event->dsmcc.status )	free( hbbtv_event->dsmcc.status );
		free( hbbtv_event );
	}
    LOG_DEBUG(" (-) failed\n" );
    
	return -1;
}

