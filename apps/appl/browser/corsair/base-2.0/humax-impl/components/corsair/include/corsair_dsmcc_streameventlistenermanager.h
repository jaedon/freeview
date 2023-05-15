/** **********************************************************************************************************
	@file 		corsair_dsmcc_streameventlistenermanager.h
	
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
#include <vector>
#include <map>
#include <string>

#include "corsair_dsmcc_streameventlistener.h"
#include "corsair_dsmcc_xmlstreameventlistener.h"
#include "corsair_dsmcc_streameventmessage.h"

/*************************************************************************************************************
	local definition
*************************************************************************************************************/
#ifndef __CORSAIR_DSMCC_STREAMEVENTLISTENERMANAGER_H__
#define __CORSAIR_DSMCC_STREAMEVENTLISTENERMANAGER_H__

using namespace std;

/** **********************************************************************************************************
	@class	CorsairDsmccStreamEventListenerManager
 	@brief 	this class is to manage several StreamEvent Listener.
*********************************************************************************************************** */
class CorsairDsmccStreamEventListenerManager {
public:
	CorsairDsmccStreamEventListenerManager();
	virtual ~CorsairDsmccStreamEventListenerManager();

	/**
		@fn 	sizeOfListener
		@breif 	get the size of listener
		@param	N/A
		@return the number of listener
	*/
	int sizeOfListener();
	
	/**
		@fn 	addListener
		@breif 	add the new object
		@param	listener	[INPUT]	the new Object to add.
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	addListener( CorsairDsmccStreamEventListener *listener );
	
	/**
		@fn 	deleteListener
		@breif 	delete the object has same EventName and ListerID
		@param	eventname	[INPUT]	the pointer of EventName
		@param	listener_id	[INPUT]	the value of listener_id
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	deleteListener( const char* target_url, const char* event_name, void* listener_id );
	
	/**
		@fn 	deleteAllListener
		@breif 	delete all element has VideoId
		@param	video_id	[INPUT]	the value of the listener'sVideoID
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	deleteAllListener( void* video_id );
	
	/**
		@fn 	findListenerMatchingId
		@breif 	find the element has VideoId and Listener ID
		@param	listener_id	[INPUT]	the value of listener_id
		@param	video_id	[INPUT]	the value of VideoId
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairDsmccStreamEventListener* findListenerMatchingId( void *listener_id );
	
	/**
		@fn 	clear
		@breif 	clear the list of this manager
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int clear( void );
	
	/**
		@fn 	onStreamEventRecieved
		@breif 	clear the list of this manager
		@param	N/A
	*/
	void onStreamEventRecieved( const char *url, unsigned short event_id, 
	                           const char *event_name, unsigned char *event_data, 
	                           unsigned int event_data_length);
	
private:
	std::vector<CorsairDsmccStreamEventListener *> m_listeners;
};

#endif	// __CORSAIR_DSMCC_STREAMEVENTLISTENERMANAGER_H__
