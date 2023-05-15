/** **********************************************************************************************************
	@file 		corsair_dsmcc_streameventlistener.h
	
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
#ifndef __CORSAIR_DSMCC_STREAMEVENTLISTENER_H__
#define __CORSAIR_DSMCC_STREAMEVENTLISTENER_H__

#include <vector>
#include <string>

/** **********************************************************************************************************
	@class	CorsairDsmccStreamEventListener
 	@brief 	this class has the information for each StreamEventListener
*********************************************************************************************************** */
class CorsairDsmccStreamEventListener {
public:
	enum{
	    StreamEventListener,
	    XMLStreamEventListener
	}ListenerType;
	
	/**
		@fn 	CorsairDsmccStreamEventListener
		@breif 	this function is the constructor
		@param	target_url	[INPUT]	the url of the page that it use this event listener.
		@param	eventname	[INPUT]	the event name to use on this listener
		@param	listener_id	[INPUT]	the Id of this Listener
		@param	lilstener	[INPUT]	the handle of this Listener
		@param	video_id	[INPUT]	the VideoId of this Listener
		@return N/A
	*/
	CorsairDsmccStreamEventListener( const char* target_url, const char* eventname, void* listener_id, void* lilstener, void* video_id, int type=StreamEventListener);

	virtual ~CorsairDsmccStreamEventListener();
	
	/**
		@fn 	getListenerType
		@breif 	get the type of this listener
		@param	N/A
		@return the reference of event name
	*/
	int getListenerType() { return m_listener_type; }
	
	/**
		@fn 	getEventName
		@breif 	get the eventname can be acceptable on this listener
		@param	N/A
		@return the reference of event name
	*/
	std::string& getEventName() { return m_event_name; }
		
	/**
		@fn 	getTargetUrl
		@breif 	get the Url can be acceptable on this listener
		@param	N/A
		@return the reference of url
	*/
	std::string& getTargetUrl() { return m_target_url; }
		
	/**
		@fn 	getListenerId
		@breif 	get the id of this listener 
		@param	N/A
		@return value of Listener Id
	*/
	void* getListenerId() { return m_listener_id; }
	
	/**
		@fn 	getListener
		@breif 	get the handle of this listener 
		@param	N/A
		@return value of Listener handle
	*/
	void* getListener() { return m_listener; }
	
	/**
		@fn 	getVideoId
		@breif 	get the VideoId can be acceptable on this listener
		@param	N/A
		@return value of VideoId
	*/
	void* getVideoId() { return m_video_id; }
	
    /**
		@fn 	onStreamEventRecieved
		@breif 	
		@param	N/A
		@return 
	*/
	virtual int onStreamEventRecieved( const char *target_url, unsigned short event_id, const char *event_name, unsigned char *data, unsigned int data_length);
	
private:
    int    			m_listener_type;
	std::string		m_event_name;
	std::string		m_target_url;
	void			*m_listener_id;
	void			*m_listener;
	void			*m_video_id;
};

#endif	// __CORSAIR_DSMCC_OBJECTCAROUSEL_H__
