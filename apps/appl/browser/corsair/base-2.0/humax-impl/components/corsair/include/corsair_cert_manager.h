/** **********************************************************************************************************
	@file 		corsair_cert_manager.h
	
	@date		2012/09/07
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
#ifndef __CORSAIR_CERT_MANAGER_H__
#define __CORSAIR_CERT_MANAGER_H__
/*************************************************************************************************************
	header
*************************************************************************************************************/
#include <vector>
#include <string>
#include "gogi_opera_api.h"
#include "corsair_cert.h"
/*************************************************************************************************************
	local definition
*************************************************************************************************************/

/*************************************************************************************************************
	local static variables
*************************************************************************************************************/

/** **********************************************************************************************************
	@class	CorsairCertManager
 	@brief 	this class is to manage several certification file.
*********************************************************************************************************** */
class CorsairCertManager {
public:
	CorsairCertManager();
	virtual ~CorsairCertManager();

public:
	/**
		@fn 	loadCertFiles
		@breif 	load certification files.
		@param	N/A
		@return 0	- succeed to load the cert.conf
		@return 1	- error is happened to load the cert.conf
	*/
	int loadCertFiles( void );
	
	/**
		@fn 	findMatchingCert
		@breif 	find a cert file that is used on this domain.
		@param	domain - input - the Domain of the current page.
		@param	common_name - input - the common name of certification. 
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	CorsairCert*	findMatchingCert( const char *domain, const char *common_name );

	/**
		@fn 	getWorkingIndexOnCertArray
		@breif  get index of the working certification file in the certarray. if there is not matching certification.
		        retry to find matching certification with default common name.
		@param	domain - input - the Domain of the current page.
		@param  cert_array - input - the array of certification common name .
		@param	size_of_cert_array - input - the size of pszCertArray.
		@return n	- the index number. it's smaller than nSizeOfCertArray.
		@return -1	- there is not matching certification file.
	*/
	int getWorkingIndexOnCertArray( const char *domain, const char **cert_array, int size_of_cert_array );

	/**
		@fn 	getCommonNameOfDefaultCert
		@breif  get the common name of default certification file
		@param	N/A
		@return char pointer - the pointer of the array.
	*/
	const char* getCommonNameOfDefaultCert( void );
	
	/**
		@fn 	addDomainOnWhitelist
		@breif  add the domain on the certification has the common name.
		@param  common_name - input - the common name of the cert.
		@param	domain - input - the Domain of the current page.
		@return 0	- succeed 
		@return 1	- error 
	*/
	int addDomainOnWhitelist( const char *common_name, const char *domain );
		
	/**
		@fn 	updateCertConfiguration
		@breif  update the config file with the current management information 
		@param	N/A
		@return 0	- succeed 
		@return 1	- error 
	*/
	int updateCertConfiguration( void );

	/**
		@fn 	isAcceptableCertification
		@breif  the function to check wheather if current certification dialog is acceptable or not.
		@param	handle - data, current dialog information
		@return 0 	- denied
		@return 1 	- accepted
	*/	
	bool isAcceptableCertification(GogiOperaEventData *data);
	int CanSelectCertification(GogiOperaEventData *data);	
	
private:
	/**
		@fn 	findCertMatchingCommonName
		@breif  the function to set each properties 
		@param	common_name - the common name to find on the array.
		@return 0	- there is no element to match.
		@return 1	- there is the element to match.
	*/
	CorsairCert* findCertMatchingCommonName( const char *common_name );
	
	/**
		@fn 	addCert
		@breif  the function to set each properties 
		@param	new_cert - the pointer of new object to add in the array
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	int	addCert( CorsairCert* new_cert );
	
	/**
		@fn 	getGogiCertManagerHandle
		@breif  the function to get a handle to access Opera GogiCertManager
		@param	N/A
		@return p 	- succeed to get a pointer of handle
		@return NULL - failed to get a handle;
	*/
	GogiCertificateManager* getGogiCertManagerHandle( eCorsairCertType eType );
	
	/**
		@fn 	getGogiCertManagerHandle
		@breif  the function to free a handle for GogiCertManager
		@param	handle - input - the pointer of handle to free;
		@return 0 	- succeed
		@return 1 	- failed
	*/
	int	freeGogiCertManagerHandle( GogiCertificateManager *handle );

private:
	typedef std::vector<CorsairCert *> 	CorsairCertVector;
	CorsairCertVector					m_root_cert_entry;
	CorsairCertVector					m_client_cert_entry;
};

#endif	/* __CORSAIR_CERT_MANAGER_H__ */

