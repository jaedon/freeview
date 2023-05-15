/** **********************************************************************************************************
	@file 		corsair_cert.h
	
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

#ifndef __CORSAIR_CERT_H__
#define __CORSAIR_CERT_H__

/*************************************************************************************************************
	header
*************************************************************************************************************/
#include <string>
#include <vector>

/*************************************************************************************************************
	local definition
*************************************************************************************************************/

typedef enum {
	eCORSAIR_CERT_TYPE_CLIENT		= 0,		/** the certification is for client */
	eCORSAIR_CERT_TYPE_ROOT,					/** the certification is for root */
} eCorsairCertType;

typedef enum {
	eCORSAIR_CERT_ENCRYPT_TYPE_P12		= 0,	/** the extention of the file is *.p12 */
	eCORSAIR_CERT_ENCRYPT_TYPE_PEM				/** the extention of the file is *.pem */
} eCorsairCertEncryptType;

/*************************************************************************************************************
	local static variables
*************************************************************************************************************/

/** **********************************************************************************************************
	@class	CorsairCert
 	@brief 	this class has the information of each certification file element.
*********************************************************************************************************** */
class CorsairCert {
public:
	CorsairCert();
	virtual ~CorsairCert();
	
public:
	/**
		@fn 	getSizeOfDomain
		@breif 	the function to get size of domain this cert has.
		@param	N/A
		@return n	- the number of this cert's domain
	*/	
	int	getSizeOfDomain( void ) { return m_domain_entry.size(); }
	
	/**
		@fn 	getDomain
		@breif 	the function to get the element from the domain array.
		@param	index - the index of array.
		@return string &	the reference of the String
	*/	
	std::string &getDomain( int index ) { return m_domain_entry.at(index); }
	
	/**
		@fn 	addDomain
		@breif 	the function to add domain element on this Cert information 
		@param	domain - the String to find on the array. 
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void addDomain( const char *domain ) { std::string sDomain = domain; m_domain_entry.push_back( sDomain ); }

	/**
		@fn 	findMatchingDomain
		@breif 	the function to set each properties 
		@param	domain - the String to find on the array. 
		@return NULL - failed to get the pointer of Object.
		@return the pointer of Object
	*/
	std::string* findMatchingDomain( const char *domain );
		
	/**
		@fn 	gePath
		@breif 	the function to get the value of the property
		@param	N/A
		@return string &	the reference of the String
	*/	
	std::string &getPath( void ) { return m_path; }
		
	/**
		@fn 	setPath
		@breif 	the function to set the value on the property.
		@param	pcCertPath - the String to set
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void setPath( const char *path ) { m_path = path; }

	/**
		@fn 	getCommonName
		@breif 	the function to get the value of the property
		@param	N/A
		@return string &	the reference of the String
	*/
	std::string &getCommonName( void ) { return m_common_name; }
			
	/**
		@fn 	setCommonName
		@breif 	the function to set the value on the property.
		@param	common_name - the value to set
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void setCommonName( const char *common_name ) { m_common_name = common_name; }

	/**
		@fn 	getPassword
		@breif 	the function to get the value of the property
		@param	N/A
		@return string &	the reference of the String
	*/	
	std::string &getPassword( void ) { return m_password; }	

	/**
		@fn 	setPassword
		@breif 	the function to set the value on the property.
		@param	password - the value to set
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void setPassword( const char *password ) { m_password = password; }

	/**
		@fn 	getCertType
		@breif 	the function to get the value of the property
		@param	N/A
		@return string &	the reference of the String
	*/	
	eCorsairCertType getCertType( void ) { return m_cert_type; }	

	/**
		@fn 	setCertType
		@breif 	the function to set the value on the property.
		@param	eType - the value to set
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void setCertType( eCorsairCertType eType ) { m_cert_type = eType; }
	
	/**
		@fn 	getEncryptType
		@breif 	the function to get the value of the property
		@param	N/A
		@return string &	the reference of the String
	*/	
	eCorsairCertEncryptType getEncryptType( void ) { return m_encrypt_type; }
	
	/**
		@fn 	setEncryptType
		@breif 	the function to set the value on the property.
		@param	eType - the value to set
		@return 0 	- succeed to set
		@return 1 	- failed to set
	*/
	void setEncryptType( eCorsairCertEncryptType eType ) { m_encrypt_type = eType; }
	
	/**
		@fn 	getDefaultCert
		@breif 	the function to get the value of the property
		@param	N/A
		@return bool the value of this property
	*/	
	bool getDefaultCert( void ) { return m_default; }
	
	/**
		@fn 	setDefaultCert
		@breif 	the function to get the value of the property
		@param	default bool type value to set this property
		@return N/A
	*/	
	void setDefaultCert( bool default_cert ) { m_default = default_cert; }
	
	/**
		@fn 	printDomain
		@breif printd the Domain set to debug
		@param	N/A
		@return N/A
	*/	
	void printDomain(void);
	
private:
	/** the white list,acceptable for this cert */
	std::vector<std::string>	m_domain_entry;
	/** the path of this file */
	std::string					m_path;
	/** the common name of this cert */
	std::string					m_common_name;
	/** the password for this cert */
	std::string					m_password;
	/** the type of this cert */
	eCorsairCertType			m_cert_type;
	/** the encrypted type of this cert */
	eCorsairCertEncryptType		m_encrypt_type;
	/** the value to use this cert by certmanager for default cert */
	bool						m_default;
};

#endif

