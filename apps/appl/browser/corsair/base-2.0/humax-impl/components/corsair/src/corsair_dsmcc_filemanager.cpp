/** **********************************************************************************************************
	@file 		corsair_dsmcc_filemanager.cpp
	
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
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "corsair.h"
#include "corsair_config.h"
#include "corsair_dsmcc_file.h"
#include "corsair_dsmcc_filemanager.h"
#include "corsair_dsmcc_manager.h"

using namespace std;

/*************************************************************************************************************
	CorsairDsmccFile class body
*************************************************************************************************************/

CorsairDsmccFile::CorsairDsmccFile( const char *filename, void *identifier )
{
	LOG_DEBUG(" + %s \n",filename);
	m_filename = filename;
	m_mimetype.clear();
	m_identifier = identifier;
	m_is_ready = 0;
	m_file_length = 0;
	m_read_length = 0;
	m_buffer_entry.clear();
}

CorsairDsmccFile::~CorsairDsmccFile()
{
	vector<unsigned char *>::iterator it;
		
	m_filename.clear();
	m_mimetype.clear();

	for ( it = m_buffer_entry.begin(); it != m_buffer_entry.end( ) ; it++ )
	{
		free( (*it) );
	}
	
	m_buffer_entry.clear();
}

int CorsairDsmccFile::read( void** ppBuffer, int* piLength)
{
	ifstream		InputFile;
	unsigned int	LengthToRead = 0;
	unsigned char  	*BufferToRead = NULL;
	
	LOG_DEBUG("readfile %s \n",m_filename);
	
	InputFile.open( m_filename.c_str(), ios::binary );

	if ( InputFile.is_open() == false )
	{
		LOG_DEBUG("failed to open the file\n");
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
		LOG_DEBUG("This file is read fully\n");
		return 0;
	}
		
	BufferToRead = ( unsigned char *) malloc ( LengthToRead );
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
	
	LOG_DEBUG("READ: %s ( %d/%d/%d ) \n",m_filename.c_str(),LengthToRead,m_read_length,m_file_length);
	
	*ppBuffer = BufferToRead;
	*piLength = (int )LengthToRead;

	return 0;
}

int CorsairDsmccFile::close( void )
{
	m_read_length = 0;

    vector<unsigned char *>::iterator it;
   	for ( it = m_buffer_entry.begin(); it != m_buffer_entry.end( ) ; it++ )
   	{
   		free( (*it) );
   	}
	m_buffer_entry.clear();
	
	return 0;
}

int CorsairDsmccFile::checkFileExist( void )
{
	if ( access( m_filename.c_str(), 0 ) == 0 )
	{
		return 1;
	}
	
	return 0;
}

std::string & CorsairDsmccFile::getMimeType( void )
{
	LOG_DEBUG("+ \n");

	if ( m_filename.find(".htm") != string::npos )
	{
		m_mimetype = "application/vnd.hbbtv.xhtml+xml";
	}
	else if ( m_filename.find(".css") != string::npos )
	{
		m_mimetype = "text/css";
	}
	
	LOG_DEBUG("- %s \n",m_mimetype.c_str());
	return m_mimetype;
}

/***********************************************************************************************
	CorsairDsmccFileManager class body
***********************************************************************************************/
CorsairDsmccFileManager::CorsairDsmccFileManager( const char * pcRootPath )
{
	m_file_entry.clear();
	m_path = pcRootPath;	
}

CorsairDsmccFileManager::~CorsairDsmccFileManager()
{
}

int CorsairDsmccFileManager::addFile( CorsairDsmccFile *poFile )
{
	m_file_entry.push_back( poFile );
	return 0;
}

int CorsairDsmccFileManager::delFile( void *identifier )
{
	int	i = 0;
	
	if ( identifier == NULL )
	{
		return 1;
	}

	vector<CorsairDsmccFile *>::iterator it;	
   	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++,i++ )
   	{
   		if ( (*it)->getIdentifier( ) == identifier )
   		{
   			delete (*it);
   			m_file_entry.erase( m_file_entry.begin() + i );
   			return 0;
   		}
   	}
	
	return 1;
}

CorsairDsmccFile* CorsairDsmccFileManager::findFileMatchingIdentifier( void *identifier )
{
	if ( identifier == NULL )
	{
		return NULL;
	}
	
    vector<CorsairDsmccFile *>::iterator it;
   	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
   	{
   		if ( (*it)->getIdentifier( ) == identifier )
   		{
   			return (*it);
   		}
   	}
	
	return NULL;
}

CorsairDsmccFile* CorsairDsmccFileManager::getFile( int iIndex )
{
	vector<CorsairDsmccFile *>::iterator it;
	
	if ( iIndex < 0 )
	{
		return NULL;
	}
	
	return m_file_entry.at( iIndex );
}
	
int CorsairDsmccFileManager::getSize( void )
{
	return m_file_entry.size( );
}

int CorsairDsmccFileManager::clear( void )
{
    if ( m_file_entry.empty() == false )
    {
    	vector<CorsairDsmccFile *>::iterator it;
    	
    	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
    	{
    		delete (*it);
    	}
	
	    m_file_entry.clear();
	}
	
	return 0;
}
