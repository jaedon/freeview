/** **********************************************************************************************************
	@file 		corsair_dsmcc_manager.h
	
	@date		2015/06/07
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
#include <cstdlib>

#include "corsair_dsmcc_uri.h"
#include "corsair_log.h"

/** **********************************************************************************************************
	@class	CorsairDsmccUri
 	@brief 	this class has the information of each downloaded file element from broadcast.
*********************************************************************************************************** */

CorsairDsmccUri::CorsairDsmccUri(const char* uri)
{
    std::string local_uri = uri;
    std::size_t host_idx = local_uri.rfind("/");
    if( host_idx != std::string::npos )
    {
        m_uri = local_uri;
    }
    else
    {
        m_uri.clear();
        m_uri.append("/");
        m_uri.append(uri);
    }
}

CorsairDsmccUri::~CorsairDsmccUri()
{
}

CorsairDsmccUri& CorsairDsmccUri::operator=(const char *str)
{
    if ( str )
    {
        this->m_uri = str;
    }
    return *this;
}

const char* CorsairDsmccUri::getUri()
{
    return m_uri.c_str();
}

const char* CorsairDsmccUri::getProtocol()
{
    if( haveProtocol() )
    {
        std::string protocol_div = "://";
        std::size_t protocol_idx = m_uri.find(protocol_div);
        if( protocol_idx != std::string::npos )
            return m_uri.substr(0,protocol_idx - 1).c_str();
    }
    return NULL;
}
    
const char* CorsairDsmccUri::getHost()
{
    if (haveProtocol())
    {
        std::string protocol_div = "://";
        std::size_t protocol_idx = m_uri.find(protocol_div);
        std::size_t host_idx;
        if( protocol_idx != std::string::npos )
        {
            protocol_idx +=3;
            host_idx = m_uri.find("/", protocol_idx );
            if( host_idx != std::string::npos )
            {
                return m_uri.substr(protocol_idx, host_idx).c_str();
            }
        }
    }
    return NULL;
}
    
unsigned short CorsairDsmccUri::getOriginalNetworkId()
{
    return (unsigned short)getId(0);
}

unsigned short CorsairDsmccUri::getTransportId()
{
    return (unsigned short)getId(1);
}

unsigned short CorsairDsmccUri::getServiceId()
{
    return (unsigned short)getId(2);
}

unsigned char CorsairDsmccUri::getComponentTag()
{
    return (unsigned short)getId(3);
}
	
const char* CorsairDsmccUri::getAbsolutePath()
{
#if 1
    if (haveProtocol())
    {
        std::string protocol_div = "://";
        std::size_t protocol_idx = m_uri.find(protocol_div);
        std::size_t host_idx;
        if( protocol_idx != std::string::npos )
        {
            host_idx = m_uri.find("/", protocol_idx + 3 );
            if( host_idx != std::string::npos )
            {
                return m_uri.substr(host_idx).c_str();
            }
        }
    }
    else 
    {
        std::size_t host_idx = m_uri.find("/");
        if( host_idx != std::string::npos )
        {
            return m_uri.substr(host_idx).c_str();
        }
    }
#else
    std::size_t host_idx = m_uri.rfind("/");
    if( host_idx != std::string::npos )
    {
        return m_uri.substr(host_idx).c_str();
    }
#endif
    return m_uri.c_str();   
}

const char* CorsairDsmccUri::getFilename()
{
    m_filename = "";
    
    /* find last '/', from it filename will be started */
    std::size_t host_idx = m_uri.rfind("/");
    if( host_idx != std::string::npos )
    {
        std::string _parsed_string;
        _parsed_string = m_filename = m_uri.substr(host_idx);
        
        /* remove query and fragment string from uri */
        std::size_t query_and_fragment_idx = m_filename.find_last_of("?#");
        while( query_and_fragment_idx != std::string::npos )
        {
            _parsed_string = _parsed_string.substr(0,query_and_fragment_idx-1);
            query_and_fragment_idx = _parsed_string.find_last_of("?#");
        }
        
        /* remove file ext from filename */
        std::size_t ext_dot_idx = _parsed_string.find_last_of(".");
        if( ext_dot_idx != std::string::npos )
        {
            _parsed_string = _parsed_string.substr(0,ext_dot_idx);
        }
        m_filename = _parsed_string;
    }
    
    return m_filename.c_str(); 
}

/* this function only compare the filename without ext. */
int CorsairDsmccUri::compareFilename(const char* path)
{
    if(path == NULL)
    {
        return -1;
    }
    
    std::string cmp1 = getFilename();
    CorsairDsmccUri cmp2_uri(path);
    std::string cmp2 = cmp2_uri.getFilename();
    
    return cmp1.compare(cmp2);
}

bool CorsairDsmccUri::haveProtocol()
{
    if ( m_uri.compare(0,6,"dvb://") != 0 &&
         m_uri.compare(0,7,"http://") != 0 &&
         m_uri.compare(0,8,"https://") != 0 )
    {
        return false;
    }
    return true;
}

unsigned int CorsairDsmccUri::getId(int idx)
{
 	char szTemp[32] = {0,};
	szTemp[0] = '0';
	szTemp[1] = 'x';
	
    std::string protocol_div = "://";
    std::size_t protocol_idx = m_uri.find(protocol_div);
        
    if ( protocol_idx != std::string::npos )
    {
        protocol_idx += 3;
    }
    else
    {
        protocol_idx = 0;
    }
    
    std::size_t dvb_start_idx = protocol_idx/* length of "://" */;
    std::size_t dvb_end_idx = protocol_idx;
    
    /*         ¡ý dvb_end_idx
                 ¡ý dvb_start_idx
        dvb://1.2.3.4/index.html  */
    for( int i = 0; i <= idx; i++ )
    {
        if ( i != 0 )
            dvb_end_idx = dvb_start_idx + 1;
            
        if ( i == 3 )
            dvb_start_idx = m_uri.find("/", dvb_end_idx );
        else
            dvb_start_idx = m_uri.find(".", dvb_end_idx );
        
        if( dvb_start_idx == std::string::npos )
        {
            return 0;
        }
        
    }

    /*  by any reason, below is happened. we shall ignore 
                 ¡ý dvb_end_idx
                         ¡ý dvb_start_idx
                   ¡ý host_idx
        dvb://1.3.4/index.html  */
    std::size_t host_idx = m_uri.find("/", protocol_idx );
    if( dvb_start_idx < protocol_idx || dvb_start_idx > host_idx )
    {
        return 0;
    }
    
    /*          ¡ý¡ý  if the gap is 1, the value is empty
        dvb://1...0/index.html  */
    if ( dvb_start_idx == dvb_end_idx )
    {
        return 0;
    }
    snprintf(&szTemp[2], sizeof(szTemp)-2, "%s", m_uri.substr(dvb_end_idx, (dvb_start_idx-(dvb_end_idx))).c_str());
    
    return strtol(szTemp, NULL, 16);
}
