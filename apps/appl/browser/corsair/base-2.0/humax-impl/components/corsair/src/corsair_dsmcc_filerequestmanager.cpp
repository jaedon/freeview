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
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "corsair_dsmcc_filerequestmanager.h"
#include "corsair_log.h"

using namespace std;

/***********************************************************************************************
	CorsairDsmccFileRequestManager class body
***********************************************************************************************/
CorsairDsmccFileRequestManager::CorsairDsmccFileRequestManager( const char * root_path )
    :m_path(root_path)
{
	clear();
}

CorsairDsmccFileRequestManager::~CorsairDsmccFileRequestManager()
{
    clear();
}

int CorsairDsmccFileRequestManager::addFile( CorsairDsmccFileRequest *file )
{
	m_file_entry.push_back( file );
	return 0;
}

int CorsairDsmccFileRequestManager::deleteFile( void *identifier )
{
	if ( identifier == NULL )
	{
		return 1;
	}

    vector<CorsairDsmccFileRequest *>::iterator it;
   	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
   	{
   		if ( (*it)->getIdentifier( ) == identifier )
   		{
   			delete (*it);
   			m_file_entry.erase( it );
   			return 0;
   		}
   	}
	
	return 1;
}

CorsairDsmccFileRequest* CorsairDsmccFileRequestManager::findFileMatchingIdentifier( void *identifier )
{
	vector<CorsairDsmccFileRequest *>::iterator it;
	
	if ( identifier == NULL )
	{
		return NULL;
	}
	
	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
	{
		if ( (*it)->getIdentifier( ) == identifier )
		{
			return (*it);
		}
	}
	
	return NULL;
}

int CorsairDsmccFileRequestManager::clear( void )
{
	vector<CorsairDsmccFileRequest *>::iterator it;
	
	for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
	{
		delete (*it);
	}
	
	m_file_entry.clear();
	
	return 0;
}

int CorsairDsmccFileRequestManager::onFileDownloaded(void)
{
    vector<CorsairDsmccFileRequest *>::iterator it;
    for ( it = m_file_entry.begin(); it != m_file_entry.end( ) ; it++ )
    {
        (*it)->onFileDownloaded();
    }
    
	return 0;
}
