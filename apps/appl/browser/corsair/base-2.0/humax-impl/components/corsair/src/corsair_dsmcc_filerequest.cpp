/** **********************************************************************************************************
	@file 		corsair_dsmcc_file.cpp
	
	@date		2012/09/07
	@author		ssyoo@humaxdigital.com
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
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "hbbtv_api.h"

#include "corsair_log.h"
#include "corsair_dsmcc_filerequest.h"

#include "bplglobal.h"

using namespace std;

/*************************************************************************************************************
	CorsairDsmccFileRequest class body
*************************************************************************************************************/

CorsairDsmccFileRequest::CorsairDsmccFileRequest( const char *uri, void *identifier )
    :m_uri(uri)
    ,m_localpath("")
    ,m_mimetype("")
    ,m_identifier(identifier)
    ,m_is_ready(0)
    ,m_file_length(0)
    ,m_read_length(0)
{
    LOG_DEBUG(" (+) file : %s\n", m_uri.c_str());
	m_buffer_entry.clear();
}

CorsairDsmccFileRequest::~CorsairDsmccFileRequest()
{
	vector<unsigned char *>::iterator it;
	for ( it = m_buffer_entry.begin(); it != m_buffer_entry.end( ) ; it++ )
	{
		delete[] (*it);
	}
	m_buffer_entry.clear();
}

int CorsairDsmccFileRequest::setLocalPath( const char *localpath )
{
    LOG_DEBUG(" (+) %s\n", localpath );
    m_localpath = localpath; 
    return 0;
}

int CorsairDsmccFileRequest::read( void** ppBuffer, int* piLength)
{
	ifstream		InputFile;
	unsigned int	LengthToRead = 0;
	unsigned char  	*BufferToRead = NULL;
	
	LOG_DEBUG(" (+) file %s\n", m_localpath.c_str() );
	
	struct stat file_stat;
	if (stat(m_localpath.c_str(), &file_stat) != 0 )
	{
	    LOG_DEBUG(" (-) unknow error is happened during getting the state of the file\n");
        *ppBuffer = NULL;
    	*piLength = 0;
	    return 1;
	}
	
	if (S_ISREG(file_stat.st_mode))
	{
	    LOG_DEBUG(" (-) This is a file\n");
	    
    	InputFile.open( m_localpath.c_str(), ios::binary );
    
    	if ( InputFile.is_open() == false )
    	{
    		LOG_DEBUG(" (-) failed to open the file\n");
    		return 1;	
    	}
    	
    	if ( m_is_ready )
    	{
    		LengthToRead = m_file_length - m_read_length;
    	}
    	else
    	{
    		InputFile.seekg (0, ios::end);
    	  	m_file_length = ( unsigned int )InputFile.tellg();
    		InputFile.seekg (0, ios::beg);
    		m_read_length = 0;
    		LengthToRead = m_file_length;
    		m_is_ready = true;
    	}
    	
    	if ( LengthToRead == 0 )
    	{
    		*ppBuffer = NULL;
    		*piLength = 0;
    		LOG_DEBUG(" (-) This file is read fully\n");
    		return 0;
    	}
    		
    	BufferToRead = new unsigned char[LengthToRead];
    	if ( BufferToRead == NULL )
    	{
    		*ppBuffer = NULL;
    		*piLength = 0;
    		return 1;
    	}
    	
    	InputFile.seekg(m_read_length, ios::beg);	
    	InputFile.read((char *)BufferToRead, LengthToRead );
    	InputFile.close();
    	
    	m_read_length += LengthToRead;
    	m_buffer_entry.push_back( BufferToRead );
    	
    	LOG_DEBUG(" (-) file %s %d byte(%d/%d)\n", m_localpath.c_str() ,LengthToRead, m_read_length, m_file_length);
    	
    	*ppBuffer = BufferToRead;
    	*piLength = (int )LengthToRead;
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        LOG_DEBUG(" (-) This is a directory\n");
        
        if (m_is_ready == true)
        {
    		LengthToRead = m_file_length - m_read_length;
    	}
    	else
    	{
    	    LengthToRead = 1;
    	}
        
        if ( LengthToRead == 0 )
    	{
    		*ppBuffer = NULL;
    		*piLength = 0;
    		LOG_DEBUG(" (-) This file is read fully\n");
    		return 0;
    	}
    	
        DIR *dp;
        string buffer;
        struct dirent *dirp;
        
        if((dp  = opendir(m_localpath.c_str())) == NULL) {
            LOG_DEBUG(" (-) filed to open dir struct\n");
            return 1;
        }

        string d_name;
        buffer.clear();
        while ((dirp = readdir(dp)) != NULL) {
            d_name = dirp->d_name;
            if ( d_name.compare(".") != 0 &&
                 d_name.compare("..") != 0 )
            {
                LOG_DEBUG("find %s\n", dirp->d_name);
                buffer.append(dirp->d_name);
                buffer.append(",");
            }
        }
        closedir(dp);
        buffer.resize(buffer.size() - 1);
        LengthToRead = buffer.size();
        BufferToRead = new unsigned char[LengthToRead];
    	if ( BufferToRead == NULL )
    	{
    		*ppBuffer = NULL;
    		*piLength = 0;
    		return 1;
    	}
    	
    	memcpy(BufferToRead, buffer.c_str(), LengthToRead );
    	*piLength = LengthToRead;
        *ppBuffer = BufferToRead;
        m_is_ready = true;
        m_file_length = m_read_length = LengthToRead;
        
        LOG_DEBUG(" (-) buffer %s(%d)\n", buffer.c_str(), LengthToRead );
    }
	return 0;
}

int CorsairDsmccFileRequest::checkFileExist( void )
{
    /* m_localpath is not set, it means that this file request is not handled correctly 
       */
	if ( m_localpath.size() != 0 && access( m_localpath.c_str(), 0 ) == 0 )
	{
		return 1;
	}
	
	return 0;
}

std::string & CorsairDsmccFileRequest::getMimeType( void )
{
	if ( m_localpath.find(".htm") != string::npos )
	{
		m_mimetype = "application/vnd.hbbtv.xhtml+xml";
	}
	else if ( m_localpath.find(".css") != string::npos )
	{
		m_mimetype = "text/css";
	}
	else if ( m_localpath.find(".xml") != string::npos )
	{
		m_mimetype = "text/xml";
	}
	else if ( m_localpath.find(".js") != string::npos )
	{
	    m_mimetype = "text/javascript";
	}
	
	LOG_DEBUG(" (-) mimetype %s\n", m_mimetype.c_str());
	
	return m_mimetype;
}

int CorsairDsmccFileRequest::onFileDownloaded(void)
{
    LOG_DEBUG(" (+) file %s\n", m_localpath.c_str() );
    
    HBBTV_EventData *event_data = (HBBTV_EventData *)malloc(sizeof(HBBTV_EventData));
	if ( !event_data )
	{
		LOG_DEBUG(" (-) Failed to get allocation\n" );
		return -1;
	}
	memset(event_data,0x00, sizeof(HBBTV_EventData));
	event_data->dsmcc.identifier = m_identifier;
	
	if ( !checkFileExist() )
	{	
	    LOG_DEBUG(" (-) E_DSMCC_FILE_ERROR \n" );
	    opera_hbbtv_send_event(E_DSMCC_FILE_ERROR, event_data);
	}
	else
	{
	    LOG_DEBUG(" (-) E_DSMCC_FILE_READY\n" );
	    opera_hbbtv_send_event(E_DSMCC_FILE_READY, event_data);
	}
	
	return 0;
}
