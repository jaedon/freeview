/** **********************************************************************************************************
	@file 		corsair_dsmcc_streameventmessgae.cpp
	
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

#include "corsair_log.h"
#include "corsair_dsmcc_streameventmessage.h"

/** **********************************************************************************************************
	@class	CorsairDsmccStreamEventMessage
 	@brief 	
*********************************************************************************************************** */
CorsairDsmccStreamEventMessage::CorsairDsmccStreamEventMessage( const char* url)
    :m_url(url)
{
	LOG_DEBUG(" (+)\n" );
}
    
CorsairDsmccStreamEventMessage::~CorsairDsmccStreamEventMessage()
{
    LOG_DEBUG(" (+)\n" );
    m_event_names_n_ids.clear();
}

int CorsairDsmccStreamEventMessage::addEventIdAndName( unsigned short event_id, const char *event_name)
{
    LOG_DEBUG(" (+)\n" );
    std::map<unsigned short, std::string>::iterator it;
    it = m_event_names_n_ids.find(event_id);
    if ( it != m_event_names_n_ids.end() )
    {
        return 1;
    }
    m_event_names_n_ids.insert(std::pair<unsigned short, std::string>(event_id, event_name));
    onSteamEventMessageChanged();
    LOG_DEBUG(" (-) added\n" );
    return 0;
}

int CorsairDsmccStreamEventMessage::onSteamEventMessageChanged()
{
    LOG_DEBUG(" (+)\n" );
    
    std::string path = m_localpath;
    path.append(m_url);
    std::fstream stream_message;
    
    stream_message.open(path.c_str(), std::ios_base::out);
    if ( stream_message.is_open())
    {
        LOG_DEBUG(" (+) success to store\n" );
        std::map<unsigned short, std::string>::iterator it = m_event_names_n_ids.begin();
        while( it != m_event_names_n_ids.end() )
        {
            stream_message.write( it->second.c_str(), it->second.size() );
            it++;
            if ( it != m_event_names_n_ids.end() )
                stream_message.write( ",", 1);
        }
        stream_message.close();
    }
    else
    {
        LOG_DEBUG(" (+) failed to store\n" );
    }
    return 0;
}
	
const char* CorsairDsmccStreamEventMessage::getEventName(unsigned short event_id)
{
    //LOG_DEBUG(" (+)\n" );
    std::map<unsigned short, std::string>::iterator it;
    it = m_event_names_n_ids.find(event_id);
    if ( it == m_event_names_n_ids.end() )
        return NULL;
    
    return it->second.c_str();
}

