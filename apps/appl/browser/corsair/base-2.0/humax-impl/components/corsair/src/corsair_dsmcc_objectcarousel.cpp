/** **********************************************************************************************************
	@file 		corsair_dsmcc_objectcarouselmanager.cpp
	
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

#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "corsair_log.h"
#include "corsair_dsmcc_objectcarousel.h"

#include "bpldsmcc.h"

using namespace std;

/*************************************************************************************************************
	CorsairDsmccObjectCarousel class body
*************************************************************************************************************/
CorsairDsmccObjectCarousel::CorsairDsmccObjectCarousel( unsigned int originalnetwork_id, unsigned int transport_id, unsigned int service_id, unsigned int carsouel_id, const char *szCachePath )
    :CorsairDsmccFileRequestManager(szCachePath)
    ,m_originalnetwork_id(originalnetwork_id)
    ,m_transport_id(transport_id)
    ,m_service_id(service_id)
    ,m_carousel_id(carsouel_id)
    ,m_download_completed(eDownloadTriggered)
{
	LOG_DEBUG(" (+) url dvb://%x.%x.%x.%x/ to %s\n", m_originalnetwork_id, m_transport_id, m_service_id, m_carousel_id, szCachePath);
	if ( access( szCachePath, 0 ) == 0 )
	{
		m_download_completed = eDownloadSuccessed;
	}
}

CorsairDsmccObjectCarousel::~CorsairDsmccObjectCarousel( void )
{
	LOG_DEBUG(" (+) url dvb://%x.%x.%x.%x/ to %s\n", m_originalnetwork_id, m_transport_id, m_service_id, m_carousel_id, getPath().c_str());
	
	std::vector<CorsairDsmccStreamEventMessage *>::iterator it;
	for( it = m_event_message.begin(); it != m_event_message.end(); it++ )
    {
        delete(*it);
    }
	m_event_message.clear();

	if ( BPL_Dsmcc_Cancel(m_carousel_id) != BPL_STATUS_OK )
	{
	    LOG_DEBUG(" (-) Failed to stop dsmcc downloading\n");
	}
	
	/* the code to remove local path */
	{
	    string cmd;
        cmd.clear();
        cmd.append("rm -rf ");
        cmd.append(getPath());
        LOG_DEBUG(" (+) cmd %s\n", cmd.c_str() );
        system( cmd.c_str());
    }
    
	LOG_DEBUG(" (-) \n");
}

void CorsairDsmccObjectCarousel::onCarouselDownloaded( bool success )
{
    LOG_DEBUG(" (+) \n");
    if ( success == true )
        setDownloaded(eDownloadSuccessed);
    else
        setDownloaded(eDownloadFailed);
        
    CorsairDsmccFileRequestManager::onFileDownloaded();
}

bool CorsairDsmccObjectCarousel::downloaded(void)
{
    return m_download_completed;
}

void CorsairDsmccObjectCarousel::setDownloaded( int downloaded )
{
    m_download_completed = downloaded;   
}

int CorsairDsmccObjectCarousel::onStreamEventMessageRecieved( const char *url, unsigned short event_id, const char *event_name )
{
    LOG_DEBUG(" (+) url %s event_id %x event_name %s\n", url, event_id, event_name );
    
    std::vector<CorsairDsmccStreamEventMessage *>::iterator it;
    for( it = m_event_message.begin(); it != m_event_message.end(); it++ )
    {
        if( (*it)->getUrl().compare(url) == 0 )
        {
            (*it)->addEventIdAndName(event_id, event_name);
            LOG_DEBUG(" (-) added to stream event message(%s)\n", (*it)->getUrl().c_str() );
            return 0;
        }
    }
    
    if( it == m_event_message.end() )
    {
        CorsairDsmccStreamEventMessage *new_message = new CorsairDsmccStreamEventMessage(url);
        if( new_message != NULL )
        {
            new_message->setLocalPath(getPath().c_str());
            new_message->addEventIdAndName(event_id, event_name);
            m_event_message.push_back(new_message);
            LOG_DEBUG(" (-) add new stream event message(%s)\n", new_message->getUrl().c_str() );
        }
    }
    
	return 0;
}

CorsairDsmccStreamEventMessage* CorsairDsmccObjectCarousel::findEventMessageMatchingEventId(unsigned short event_id)
{
	std::vector<CorsairDsmccStreamEventMessage *>::iterator it;
	
	for ( it = m_event_message.begin(); it != m_event_message.end( ) ; it++ )
	{
		if ( (*it)->getEventName(event_id) != NULL )
		{
			return (*it);
		}
	}
	
	return NULL;
}

int CorsairDsmccObjectCarousel::startDownload(void)
{
    LOG_DEBUG(" (+)\n");
      
    if ( getPath().size() > 0 )
    {    
        if ( BPL_Dsmcc_Down( getCarouselId(), (char*)getPath().c_str() ) != BPL_STATUS_OK )
        {
        	LOG_DEBUG(" (-) Failed to Download Carousel\n" );
        	return -1;
        }
    }
    else
    {
        LOG_DEBUG(" (-) local path did not set\n" );
    }
    
	LOG_DEBUG(" (-) ok\n" );
	return 0;
}
