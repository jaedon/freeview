/** **********************************************************************************************************
	@file 		corsair_dsmcc_objectcarouselmanager.h
	
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
#ifndef __CORSAIR_DSMCC_OBJECTCAROUSELMANAGER_H__
#define __CORSAIR_DSMCC_OBJECTCAROUSELMANAGER_H__

#include <vector>
#include <string>

#include "corsair_dsmcc_objectcarousel.h"

using namespace std;

/** **********************************************************************************************************
	@class	CorsairDsmccObjectCarouselManager
 	@brief 	this class is to manage several Object Carousel.
*********************************************************************************************************** */
class CorsairDsmccObjectCarouselManager {
public:
	CorsairDsmccObjectCarouselManager() {};
	virtual ~CorsairDsmccObjectCarouselManager() {};

	/**
		@fn 	addObjectCarousel
		@breif 	add new ObjectCarousel on this manager
		@param	objectcarousel	[INPUT]	the new Object to add.
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int addObjectCarousel( CorsairDsmccObjectCarousel *objectcarousel );
	
	/**
		@fn 	deleteObjectCarousel
		@breif 	delete the ObjectCarousel has url
		@param	url	[INPUT]	the Url to delete
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int deleteObjectCarousel(unsigned int originalnetwork_id, unsigned int transport_id, unsigned int service_id, unsigned int carousel_id);
	
	/**
		@fn 	deleteObjectCarousel
		@breif 	delete the ObjectCarousel has url
		@param	url	[INPUT]	the Url to delete
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int deleteObjectCarousel(unsigned long org_id, unsigned short app_id);
	
	/**
		@fn 	findObjectCarousel
		@breif 	get the ObjectCarousel has carousel_id
		@param	carousel_id	[INPUT]	the CarouselId to find
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairDsmccObjectCarousel*	findObjectCarousel(unsigned int transport_id, unsigned int originalnetwork_id, unsigned int service_id, unsigned int carousel_id);
	CorsairDsmccObjectCarousel*	findObjectCarousel(unsigned long org_id, unsigned short app_id, unsigned int carousel_id);
		
	/**
		@fn 	getObjectCarouselHasFile
		@breif 	get the ObjectCarousel handle has the file
		@param	identifier	[INPUT]	the Id of file to find
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairDsmccObjectCarousel*	getObjectCarouselHasFile( void *identifier );
	
	/**
		@fn 	getObjectCarouselHasFile
		@breif 	get the ObjectCarousel handle has the file
		@param	identifier	[INPUT]	the Id of file to find
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairDsmccObjectCarousel*	getObjectCarouselHasEventId( unsigned short event_id );

	/**
		@fn 	clear
		@breif 	clear all data this manager has.
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	clear( void );
	
	/**
		@fn 	onCarouselDownloaded
		@breif 	notify the result of downloading
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onCarouselDownloaded( unsigned int, bool success );
		
	/**
		@fn 	onStreamEventMessageRecieved
		@breif 	clear the list of this manager
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onStreamEventMessageRecieved( unsigned int carousel_id, const char *url, unsigned short event_id, const char *event_name );
	
	/**
		@fn 	restartDownloadAll
		@breif 	clear the list of this manager
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int restartDownloadAll();
private:
	std::vector<CorsairDsmccObjectCarousel *>  m_carousels;
	    
};

#endif	// __CORSAIR_DSMCC_OBJECTCAROUSELMANAGER_H__
