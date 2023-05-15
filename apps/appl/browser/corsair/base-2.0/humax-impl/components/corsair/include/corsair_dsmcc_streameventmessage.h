/** **********************************************************************************************************
	@file 		corsair_dsmcc_streameventmessgae.h
	
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
#ifndef __CORSAIR_DSMCC_STREAMEVENTMESSAGE_H__
#define __CORSAIR_DSMCC_STREAMEVENTMESSAGE_H__

#include <string>
#include <fstream>
#include <map>

/** **********************************************************************************************************
	@class	CorsairDsmccStreamEventMessage
 	@brief 	
*********************************************************************************************************** */
class CorsairDsmccStreamEventMessage {
public:
	
	/**
		@fn 	CorsairDsmccStreamEventListener
		@breif 	this function is the constructor
		@return N/A
	*/
	CorsairDsmccStreamEventMessage( const char* url);
    
    /**
		@fn 	CorsairDsmccStreamEventListener
		@breif 	this function is the constructor
		@return N/A
	*/
	~CorsairDsmccStreamEventMessage();
	
	/**
		@fn 	addEventIdAndName
		@breif 	
		@return N/A
	*/
	int addEventIdAndName( unsigned short event_id, const char *event_name);
	
	/**
		@fn 	onSteamEventMessageChanged
		@breif 	this function is the constructor
		@return N/A
	*/
	int onSteamEventMessageChanged();

	/**
		@fn 	getEventName
		@breif 	this function is the constructor
		@return N/A
	*/
	const char* getEventName(unsigned short event_id);
	
	/**
		@fn 	setLocalPath
		@breif 	this function is the constructor
		@return N/A
	*/
	void setLocalPath(const char *localpath) { m_localpath = localpath;}
	
    /**
		@fn 	getUrl
		@breif 	this function is the constructor
		@return N/A
	*/
	std::string& getUrl() { return m_url; }
	
private:
    std::map<unsigned short, std::string>   m_event_names_n_ids;
	std::string								m_url;
	std::string                             m_localpath;
};

#endif	// __CORSAIR_DSMCC_STREAMEVENTMESSAGE_H__
