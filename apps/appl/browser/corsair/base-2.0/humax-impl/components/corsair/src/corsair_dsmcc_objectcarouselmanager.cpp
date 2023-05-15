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

#include "corsair_log.h"
#include "corsair_dsmcc_objectcarouselmanager.h"
#include "corsair_dsmcc_objectcarousel_appid.h"

#include "bpldsmcc.h"

using namespace std;


/*************************************************************************************************************
	CorsairDsmccObjectCarouselManager class body
*************************************************************************************************************/
int CorsairDsmccObjectCarouselManager::addObjectCarousel( CorsairDsmccObjectCarousel *pObjectCarousel  )
{
    LOG_DEBUG(" (+)\n" );
    
	if ( !pObjectCarousel )
	{
		return -1;
	}

	m_carousels.push_back( pObjectCarousel );

	return 0;
}

int CorsairDsmccObjectCarouselManager::deleteObjectCarousel(unsigned int originalnetwork_id, unsigned int transport_id, unsigned int service_id, unsigned int carousel_id)
{
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;

    LOG_DEBUG(" (+) url dvb://%x.%x.%x.%x/\n", originalnetwork_id, transport_id, service_id, carousel_id );

	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
	    LOG_DEBUG(" (+) check dvb://%x.%x.%x.%x/\n", (*it)->getOriginalNetworkId(), (*it)->getTransportId(), (*it)->getServiceId(), (*it)->getCarouselId() );
		 if ( (*it)->getTransportId() == transport_id &&
	         (*it)->getOriginalNetworkId() == originalnetwork_id &&
	         (*it)->getServiceId() == service_id &&
	         (*it)->getCarouselId() == carousel_id )
		{
			LOG_DEBUG(" (-) Delete\n" );
			delete (*it);
			m_carousels.erase(it);
			return 0;
		}
	}

	LOG_DEBUG(" (-)\n" );
	return -1;
}

int CorsairDsmccObjectCarouselManager::deleteObjectCarousel(unsigned long organisation_id, unsigned short application_id)
{
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;

    LOG_DEBUG(" (+) url org %x app %x\n", organisation_id, application_id );

	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
	    CorsairDsmccObjectCarouselAppId* carousel = static_cast<CorsairDsmccObjectCarouselAppId*>((*it));
	    LOG_DEBUG(" (+) check org %x app %x carousel id %x\n", carousel->getOrganisationId(), carousel->getApplicationId(), carousel->getCarouselId() );
		 if ( carousel->getOrganisationId() == organisation_id &&
	         carousel->getApplicationId() == application_id )
		{
			LOG_DEBUG(" (-) Delete\n" );
			delete carousel;
			m_carousels.erase(it);
			return 0;
		}
	}

	LOG_DEBUG(" (-)\n" );
	return -1;
}

CorsairDsmccObjectCarousel* CorsairDsmccObjectCarouselManager::findObjectCarousel(unsigned int originalnetwork_id, unsigned int transport_id, unsigned int service_id, unsigned int carousel_id)
{
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;

    LOG_DEBUG(" (+) url dvb://%x.%x.%x.%x/ \n", originalnetwork_id, transport_id, service_id, carousel_id );
	
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
	    LOG_DEBUG(" (+) check dvb://%x.%x.%x.%x/ \n", (*it)->getOriginalNetworkId(), (*it)->getTransportId(), (*it)->getServiceId(), (*it)->getCarouselId() );
	    if ( (*it)->getTransportId() == transport_id &&
	         (*it)->getOriginalNetworkId() == originalnetwork_id &&
	         (*it)->getServiceId() == service_id &&
	         (*it)->getCarouselId() == carousel_id )
		{
			LOG_DEBUG(" (-) Got\n" );
			return (*it);
		}
	}

	LOG_DEBUG(" (-)\n" );
	return NULL;
}

CorsairDsmccObjectCarousel* CorsairDsmccObjectCarouselManager::findObjectCarousel(unsigned long organisation_id, unsigned short application_id, unsigned int carousel_id)
{
    std::vector<CorsairDsmccObjectCarousel*>::iterator it;

    LOG_DEBUG(" (+) url org %x app %x carousel %x\n", organisation_id, application_id, carousel_id );

	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
	    CorsairDsmccObjectCarouselAppId* carousel = static_cast<CorsairDsmccObjectCarouselAppId*>((*it));
	    LOG_DEBUG(" (+) check org %x app %x carousel id %x\n", carousel->getOrganisationId(), carousel->getApplicationId(), carousel->getCarouselId() );
		if ( carousel->getOrganisationId() == organisation_id &&
	         carousel->getApplicationId() == application_id &&
	         carousel->getCarouselId() == carousel_id )
		{
			LOG_DEBUG(" (-) Got\n" );
			return (*it);
		}
	}

	LOG_DEBUG(" (-)\n" );
	return NULL;
}
   
int CorsairDsmccObjectCarouselManager::clear( void )
{
	LOG_DEBUG(" (+) \n" );

    std::vector<CorsairDsmccObjectCarousel*>::iterator it;
   	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
   	{
   	    LOG_DEBUG(" Delete dvb://%x.%x.%x.%x/\n", (*it)->getOriginalNetworkId(), (*it)->getTransportId(), (*it)->getServiceId(), (*it)->getCarouselId() );
	    delete (*it);
    }
   	m_carousels.clear();
    
	LOG_DEBUG(" (-)\n" );
	return 0;
}


CorsairDsmccObjectCarousel* CorsairDsmccObjectCarouselManager::getObjectCarouselHasFile( void *identifier )
{
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;

	LOG_DEBUG(" (+) identifier  %p\n", identifier );
	
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
		if ( (*it)->findFileMatchingIdentifier( identifier ) )
		{
			LOG_DEBUG(" (-) Found\n" );
			return (*it);
		}
	}

	LOG_DEBUG(" (-)\n" );
	return NULL;
}

CorsairDsmccObjectCarousel* CorsairDsmccObjectCarouselManager::getObjectCarouselHasEventId( unsigned short event_id )
{
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;

	LOG_DEBUG(" (+) event_id  %x\n", event_id );
	
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
		if ( (*it)->findEventMessageMatchingEventId( event_id ) )
		{
			LOG_DEBUG(" (-) Found\n" );
			return (*it);
		}
	}

	LOG_DEBUG(" (-)\n" );
	return NULL;
}

int CorsairDsmccObjectCarouselManager::onCarouselDownloaded( unsigned int carousel_id, bool success )
{
	LOG_DEBUG(" (+) carousel_id %x\n", carousel_id );
	
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
		if ( (*it)->getCarouselId() == carousel_id )
		{
			(*it)->onCarouselDownloaded( success );
		}
	}

	LOG_DEBUG(" (-)\n" );
	return 0;
}

int CorsairDsmccObjectCarouselManager::onStreamEventMessageRecieved( unsigned int carousel_id, const char *url, unsigned short event_id, const char *event_name )
{
    LOG_DEBUG(" (+) carousel_id %x\n", carousel_id );
	
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
		if ( (*it)->getCarouselId() == carousel_id )
		{
			(*it)->onStreamEventMessageRecieved( url, event_id, event_name );
		}
	}

	LOG_DEBUG(" (-)\n" );
	return 0;
}

int CorsairDsmccObjectCarouselManager::restartDownloadAll()
{
    LOG_DEBUG(" (+)\n" );
	
	std::vector<CorsairDsmccObjectCarousel*>::iterator it;
	for( it = m_carousels.begin(); it != m_carousels.end(); it++ )
	{
		(*it)->startDownload();
	}

	LOG_DEBUG(" (-)\n" );
	return 0;
}
