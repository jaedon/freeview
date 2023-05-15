/** **********************************************************************************************************
	@file 		corsair_dsmcc_filerequestmanager.h
	
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
#ifndef __CORSAIR_DSMCC_FILEMANAGER_H__
#define __CORSAIR_DSMCC_FILEMANAGER_H__

#include <vector>
#include <string>
#include "corsair_dsmcc_filerequest.h"

using namespace std;

/** ***************************************************************************************
	@class	CorsairDsmccFileRequest
 	@brief 	this class has the information of each downloaded file element from broadcast.
****************************************************************************************** */
class CorsairDsmccFileRequestManager {
	
public:

	/**
		@fn 	CorsairDsmccFileRequestManager
		@breif 	constructure of this class
		@param	pcRootPath	[INPUT]	the root path of this manager.
		@return N/A
	*/
	CorsairDsmccFileRequestManager( const char * root_path );
	virtual ~CorsairDsmccFileRequestManager();

	/**
		@fn 	addFile
		@breif 	add new file object.
		@param	poFile	[INPUT]	the pointer of file object
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int addFile( CorsairDsmccFileRequest *file );
	
	/**
		@fn 	deleteFile
		@breif 	delete a file has this id
		@param	pvIdentifier	[INPUT]	the value of the file identifier
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int deleteFile( void *identifier );
	
	/**
		@fn 	findFileMatchingIdentifier
		@breif 	get a file has this id
		@param	pvIdentifier	[INPUT]	the value of the file identifier
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairDsmccFileRequest* findFileMatchingIdentifier( void *identifier );
	
	/**
		@fn 	clear
		@breif 	make free of current file list
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	clear( void );
	
	/**
		@fn 	getPath
		@breif 	the function to get the root path of this manager.
		@param	N/A
		@return reference of string
	*/
	std::string&		getPath( void ) { return m_path; }
	
	/**
		@fn 	onFileDownloaded
		@breif 	the function to get mimetype
		@param	N/A
		@return the value of identifier
	*/
	int onFileDownloaded( void );
	
private:
	std::string								m_path;
	std::vector<CorsairDsmccFileRequest *>			m_file_entry;
};

#endif	// __CORSAIR_DSMCC_FILEMANAGER_H__
