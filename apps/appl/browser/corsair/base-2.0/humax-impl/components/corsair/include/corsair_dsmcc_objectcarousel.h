/** **********************************************************************************************************
	@file 		corsair_dsmcc_objectcarousel.h
	
	@date		2012/10/03
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
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
#ifndef __CORSAIR_DSMCC_OBJECTCAROUSEL_H__
#define __CORSAIR_DSMCC_OBJECTCAROUSEL_H__

#include <vector>
#include <string>

#include "corsair_dsmcc_filerequestmanager.h"
#include "corsair_dsmcc_streameventmessage.h"

using namespace std;

/** **********************************************************************************************************
	@class	CorsairDsmccObjectCarousel
 	@brief 	this class has the information of each Object Carousel
*********************************************************************************************************** */
class CorsairDsmccObjectCarousel : public CorsairDsmccFileRequestManager{
public:
    
    enum{
        eDownloadTriggered,
        eDownloadSuccessed,
        eDownloadFailed
    };
        
	CorsairDsmccObjectCarousel( unsigned int originalnetwork_id, unsigned int transport_id, unsigned int service_id, unsigned int carsouel_id, const char *szCachePath );
	virtual ~CorsairDsmccObjectCarousel();

	/**
		@fn 	getOriginalNetworkId
		@breif 	to get the CarouselId of this Object.
		@param	N/A
		@return CarouselId
	*/
	unsigned int getOriginalNetworkId( void ) { return m_originalnetwork_id ;}
	
	/**
		@fn 	getTransportId
		@breif 	to get the TransportId of this Object.
		@param	N/A
		@return CarouselId
	*/
	unsigned int getTransportId( void ) { return m_transport_id ;}

	/**
		@fn 	getServiceId
		@breif 	to get the CarouselId of this Object.
		@param	N/A
		@return CarouselId
	*/
	unsigned int getServiceId( void ) { return m_service_id ;}
	
	/**
		@fn 	getCarouselId
		@breif 	to get the CarouselId of this Object.
		@param	N/A
		@return CarouselId
	*/
	unsigned int getCarouselId( void ) { return m_carousel_id ;}
	
	/**
		@fn 	onCarouselDownloaded
		@breif 	notify the result of download trigger
		@param	N/A
	*/
	void onCarouselDownloaded( bool success );
	
	/**
		@fn 	downloaded
		@breif 	get m_download_completed
		@param	N/A
		@return CarouselId
	*/
	bool downloaded( );
	
	/**
		@fn 	setDownloaded
		@breif 	set m_download_completed to param.
		@param	N/A
		@return CarouselId
	*/
	void setDownloaded( int downloaded );
	
	/**
		@fn 	startDownload
		@breif 	ask download to platform
		@param	N/A
		@return CarouselId
	*/
	int startDownload(void);
	    
	/**
		@fn 	onStreamEventMessageRecieved
		@breif 	clear the list of this manager
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onStreamEventMessageRecieved( const char *url, unsigned short event_id, const char *event_name );
	
	/**
		@fn 	findEventMessageMatchingEventId
		@breif 	find 
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	CorsairDsmccStreamEventMessage* findEventMessageMatchingEventId(unsigned short event_id);
	
private:
	unsigned int                                    m_originalnetwork_id;
	unsigned int                                    m_transport_id;
	unsigned int                                    m_service_id;
	unsigned int				                    m_carousel_id;
	int                                             m_download_completed;
	std::vector<CorsairDsmccStreamEventMessage *>   m_event_message;
};

#endif	// __CORSAIR_DSMCC_OBJECTCAROUSEL_H__
