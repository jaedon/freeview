/** **********************************************************************************************************
	@file 		corsair_cert_manager.cpp
	
	@date		2012/09/07
	@author		IPM-SW1T(tipmsw1@humaxdigital.com)
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
#include <vector>
#include <iostream>
#include "gogi_opera_api.h"

#include "corsair.h"
#include "corsair_cert.h"
#include "corsair_cert_manager.h"
#include "corsair_xml.h"

#define XML_NODE_INITNODE			"certificates"
#define XML_NODE_CERT				"cert"
#define XML_ATTRIB_FILENAME			"file-name"
#define XML_ATTRIB_TYPE				"type"
#define XML_ATTRIB_ENCRYPT			"encrypt"
#define XML_ATTRIB_COMMONNAME		"common-name"
#define XML_ATTRIB_PASSWORD			"password"
#define XML_ATTRIB_DOMAIN			"domains"
#define XML_ATTRIB_DOMAIN_URL		"url"
#define XML_ATTRIB_DEFAULT			"default"

using namespace std;
/*************************************************************************************************************
	CorsairCert class body
*************************************************************************************************************/
CorsairCert::CorsairCert( void )
{
	m_domain_entry.clear();
	m_path.clear();
	m_common_name.clear();
	m_password.clear();
	m_cert_type = eCORSAIR_CERT_TYPE_CLIENT;
	m_encrypt_type = eCORSAIR_CERT_ENCRYPT_TYPE_P12;	
	m_default = false;
}

CorsairCert::~CorsairCert( )
{
	vector<string>::iterator it;
		
	for ( it = m_domain_entry.begin(); it != m_domain_entry.end( ) ; it++ )
	{
		(*it).clear();
	}	
	
	m_path.clear();
	m_common_name.clear();
	m_password.clear();
}

string* CorsairCert::findMatchingDomain( const char *domain )
{
	if ( domain == NULL )
	{
		return NULL;
	}

	string	sCurrentDomain, sDomain = domain;
	vector<string>::iterator it;
	for ( it = m_domain_entry.begin(); it != m_domain_entry.end( ) ; it++ )
	{
		sCurrentDomain = (*it);
		
		if ( sDomain.find( sCurrentDomain ) != string::npos )
		{
			return &(*it);
		}
	}
	
	return NULL;
}

void CorsairCert::printDomain(void)
{
	vector<string>::iterator it;
	
	LOG_DEBUG("\tDomains----\n");
	
	for ( it = m_domain_entry.begin(); it != m_domain_entry.end( ) ; it++ )
	{
		LOG_DEBUG("\t\t %s\n",(*it).c_str());
	}
	
	LOG_DEBUG(" \t-----------\n");
}
/*************************************************************************************************************
	CorsairCertManager class body
*************************************************************************************************************/
CorsairCertManager::CorsairCertManager()
{
	m_root_cert_entry.clear();
	m_client_cert_entry.clear();
}

CorsairCertManager::~CorsairCertManager()
{
	m_root_cert_entry.clear();
	m_client_cert_entry.clear();
}

int	CorsairCertManager::loadCertFiles(void)
{
	char 							acBuffer[1024] = {0,};
	string							sConfFilePath = DEFAULT_CERT_CONFIG_FILE_PATH;
	string							sCertFilePath = DEFAULT_CERT_FILE_PATH;
	string							sFileStream;
	ifstream						ConfFileStream;
	string 							sDomains, sCommonName, sPath, sFullPath, sPassword, sTmp, sRet;
    eCorsairCertType                eCertType = eCORSAIR_CERT_TYPE_CLIENT;
    eCorsairCertEncryptType         eEncryptType = eCORSAIR_CERT_ENCRYPT_TYPE_P12;    
    CorsairCert                     *pNewCorsairCert = NULL;
    CorsairCertVector               *pTargetCertEntry = NULL;
	vector<CorsairCert *>::iterator	it;

	CorsairXml *xml = new CorsairXml((const char *)sConfFilePath.c_str());

	CorsairXmlNode *root = xml->getRoot();
	CorsairXmlNode *cur, *cert, *attrib, *domains, *url;

	cur = xml->find(root, XML_NODE_INITNODE);
	if ( !cur )
	{
		delete xml;
		return -1;
	}
	cur = xml->child(cur);
	
	while ( cur != NULL )
	{
		/* the one loop will make one certification info */
		/* add new cert */
		pNewCorsairCert = new CorsairCert( );
		
		cert = xml->child(cur);
		/* fill each attribute */
		// type (required)
		attrib = xml->find(cert, XML_ATTRIB_TYPE);
		if ( attrib )
		{
			sTmp = xml->getContent(attrib);
			eCertType = ( sTmp == "client" ) ? eCORSAIR_CERT_TYPE_CLIENT : eCORSAIR_CERT_TYPE_ROOT;
			pNewCorsairCert->setCertType( eCertType );
		}
		
		// encrypt (required)
		attrib = xml->find(cert, XML_ATTRIB_ENCRYPT);
		if ( attrib )
		{
			sTmp = xml->getContent(attrib);
			eEncryptType = ( sTmp == "P12" ) ? eCORSAIR_CERT_ENCRYPT_TYPE_P12 : eCORSAIR_CERT_ENCRYPT_TYPE_PEM;
			pNewCorsairCert->setEncryptType( eEncryptType );
		}

		// common-name (required)
		attrib = xml->find(cert, XML_ATTRIB_COMMONNAME);
		if ( attrib )
		{
			sCommonName = xml->getContent(attrib);
			pNewCorsairCert->setCommonName( sCommonName.c_str() );
		}

		// file-name (required)
		attrib = xml->find(cert, XML_ATTRIB_FILENAME);
		if ( attrib )
		{
			sPath = xml->getContent(attrib);
			sFullPath = sCertFilePath;
			if ( eCertType == eCORSAIR_CERT_TYPE_CLIENT )
			{
				sFullPath.append("/client/");
			}
			else
			{
				sFullPath.append("/root/");
			}
			sFullPath.append( sPath.c_str() );
			pNewCorsairCert->setPath( sFullPath.c_str() );
		}
		
		// common-name (required)
		attrib = xml->find(cert, XML_ATTRIB_PASSWORD);
		if ( attrib )
		{
			sPassword = xml->getContent(attrib);
			pNewCorsairCert->setPassword( sPassword.c_str() );
		}
		
		// domains (optional) that includes <url> URLs
		attrib = xml->find(cert, XML_ATTRIB_DOMAIN);
		if ( attrib )
		{
			domains = xml->child(attrib);
			while( domains != NULL )
			{
				// common-name (required)
				url = xml->find(domains, XML_ATTRIB_DOMAIN_URL);
				if ( url )
				{
					sDomains = xml->getContent(url);
					if ( ! pNewCorsairCert->findMatchingDomain( sDomains.c_str() ) )
					{
						pNewCorsairCert->addDomain( sDomains.c_str() );
					}
					domains  = xml->next( url );
				}
			}
		}
		
		// default cert (optional)
		attrib = xml->find(cert, XML_ATTRIB_DEFAULT);
		if ( attrib )
		{
			bool default_cert = false;
			sTmp = xml->getContent(attrib);
			default_cert = ( sTmp == "yes" ) ? true : false;
			pNewCorsairCert->setDefaultCert( default_cert );
		}
		
		addCert( pNewCorsairCert );

		// next cert		
		cur = xml->next(cur);
	}

	delete xml;
	
	return 0;
}

CorsairCert* CorsairCertManager::findMatchingCert( const char *domain, const char *common_name )
{
	vector<CorsairCert *>::iterator	it;
	
	if ( domain == NULL || common_name == NULL )
	{
		return NULL;	
	}
	
	for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			if ( (*it)->findMatchingDomain( domain ) )
			{
				return (*it);
			}
		}
	}
	
	for ( it = m_root_cert_entry.begin(); it < m_root_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			if ( (*it)->findMatchingDomain( domain ) )
			{
				return (*it);
			}
		}
	}
	
	return NULL;
}

int CorsairCertManager::getWorkingIndexOnCertArray( const char *domain, const char **cert_array, int size_of_cert_array )
{
	vector<CorsairCert *>::iterator	it;
		
	if ( domain == NULL || cert_array == NULL )
	{
		return -1;
	}

	for ( int i = 0; i < size_of_cert_array; i++ )
	{
		for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
		{
			if ( (*it)->getCommonName().compare( cert_array[i] ) == 0 )
			{
				if ( (*it)->findMatchingDomain( domain ) )
				{
					return i;
				}
			}
		}
		
		for ( it = m_root_cert_entry.begin(); it < m_root_cert_entry.end(); it++ )
		{
			if ( (*it)->getCommonName().compare( cert_array[i] ) == 0 )
			{
				if ( (*it)->findMatchingDomain( domain ) )
				{
					return i;
				}
			}
		}	
	}
	
	for ( int i = 0; i < size_of_cert_array; i++ )
	{
		if ((*it)->getDefaultCert())
		{
			if ((*it)->getCommonName().compare( cert_array[i] ) == 0 )
		    {
			    return i;
		    }
	    }
	}
	
	return -1;
}

const char* CorsairCertManager::getCommonNameOfDefaultCert( void )
{
	vector<CorsairCert *>::iterator	it;
	for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
	{
		if ( (*it)->getDefaultCert() )
		{
			return (*it)->getCommonName().c_str();
		}
	}
	
	for ( it = m_root_cert_entry.begin(); it != m_root_cert_entry.end(); it++ )
	{
		if ( (*it)->getDefaultCert() )
		{
			return (*it)->getCommonName().c_str();
		}
	}

	return NULL;
}
	
int CorsairCertManager::addDomainOnWhitelist( const char *common_name, const char *domain )
{
	vector<CorsairCert *>::iterator	it;
	for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			(*it)->addDomain(domain);
			return 0;
		}
	}
	
	for ( it = m_root_cert_entry.begin(); it != m_root_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			(*it)->addDomain(domain);
			return 0;
		}
	}
	
	return 1;
}

static void local_print_cert( fstream* out_file, CorsairCert *cert )
{
	int j;
	unsigned found;
	string out_string;
	
	(*out_file) << "\t<"<<XML_NODE_CERT<<">" << endl;
	
	found = cert->getPath().find_last_of("/\\");
  	(*out_file) << "\t\t" << "<" << XML_ATTRIB_FILENAME << ">" << cert->getPath().substr(found+1) << "</" << XML_ATTRIB_FILENAME << ">" << endl;
  	
	cert->getCertType() == eCORSAIR_CERT_TYPE_CLIENT ? out_string = "client" : out_string = "root";
	(*out_file) << "\t\t" << "<" << XML_ATTRIB_TYPE << ">" << out_string << "</" << XML_ATTRIB_TYPE << ">" << endl;
	
	cert->getEncryptType() == eCORSAIR_CERT_ENCRYPT_TYPE_P12 ? out_string = "P12" : out_string = "PEM";
	(*out_file) << "\t\t" << "<" << XML_ATTRIB_ENCRYPT << ">" << out_string << "</" << XML_ATTRIB_ENCRYPT << ">" << endl;
	
	(*out_file) << "\t\t" << "<" << XML_ATTRIB_COMMONNAME << ">" << cert->getCommonName() << "</" << XML_ATTRIB_COMMONNAME << ">" << endl;
	if (cert->getSizeOfDomain() > 0 )
	{
		(*out_file) << "\t\t" << "<" << XML_ATTRIB_DOMAIN << ">" << endl;
		for( j = 0; j < cert->getSizeOfDomain(); j++ )
		{
			(*out_file) << "\t\t\t" << "<" << XML_ATTRIB_DOMAIN_URL << ">" << cert->getDomain(j) << "</" << XML_ATTRIB_DOMAIN_URL << ">" << endl;
		}
		(*out_file) << "\t\t" << "</" << XML_ATTRIB_DOMAIN << ">" << endl;
	}
	
	if (cert->getPassword().length() > 0 )
		(*out_file) << "\t\t" << "<" << XML_ATTRIB_PASSWORD << ">" << cert->getPassword() << "</" << XML_ATTRIB_PASSWORD << ">" << endl;
		
	if (cert->getDefaultCert() == true)
		(*out_file) << "\t\t" << "<" << XML_ATTRIB_DEFAULT << ">" << "yes" << "</" << XML_ATTRIB_DEFAULT << ">" << endl;
		
	(*out_file) << "\t</"<<XML_NODE_CERT<<">" << endl;
}

int CorsairCertManager::updateCertConfiguration( void )
{
	vector<CorsairCert *>::iterator	it;
	fstream config_file;

	config_file.open(DEFAULT_CERT_CONFIG_FILE_PATH, std::fstream::out );
	if(!config_file.is_open())
	{
		LOG_ERR("failed to open config file\n");
		return 1;
	}
	
	config_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	config_file << "<"<<XML_NODE_INITNODE<<">" << endl;
	
	for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
	{
		local_print_cert( &config_file, (*it) );
	}
	
	for ( it = m_root_cert_entry.begin(); it < m_root_cert_entry.end(); it++ )
	{
		local_print_cert( &config_file, (*it) );
	}
	config_file << "</"<<XML_NODE_INITNODE<<">" << endl;
	config_file.close();
	
	return 0;
}
	
CorsairCert* CorsairCertManager::findCertMatchingCommonName( const char *common_name )
{
	vector<CorsairCert *>::iterator	it;
	for ( it = m_client_cert_entry.begin(); it != m_client_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			return (*it);
		}
	}
	
	for ( it = m_root_cert_entry.begin(); it != m_root_cert_entry.end(); it++ )
	{
		if ( (*it)->getCommonName().compare( common_name ) == 0 )
		{
			return (*it);
		}
	}
	
	return NULL;
}

int	CorsairCertManager::addCert( CorsairCert* new_cert )
{
	GogiCertificateManager *pCertManagerHandle = getGogiCertManagerHandle( new_cert->getCertType() );

#if 0	
	LOG_DEBUG("addCert CN :" << new_cert->getCommonName() << " Path : " << new_cert->getPath();
	new_cert->printDomain(); 
#endif

	if ( pCertManagerHandle )
	{
		int r = op_import_certificate_noninteractive( pCertManagerHandle, 
													  new_cert->getPath().c_str(), 
													  new_cert->getPassword().compare( "" ) == 0 ? NULL: new_cert->getPassword().c_str(),
													  false, true );
		freeGogiCertManagerHandle( pCertManagerHandle );
	}

	if ( new_cert->getCertType( ) == eCORSAIR_CERT_TYPE_CLIENT )
	{
		m_client_cert_entry.push_back( new_cert );	
	}
	else
	{
		m_root_cert_entry.push_back( new_cert );
	}	
	
	return 0;
}

GogiCertificateManager* CorsairCertManager::getGogiCertManagerHandle( eCorsairCertType eType )
{
	GogiCertificateManager		*pCertManagerHandle = NULL;
	GOGI_OPERA_CERT_CATEGORY	category = GOGI_OPERA_INTERMEDIATE_CERTIFICATE_AUTHORITIES;
	
	if ( getCorsair() == NULL || getCorsair()->getOpera() == NULL )
	{
		return NULL;
	}
	
	if ( eType == eCORSAIR_CERT_TYPE_CLIENT )
	{
		category = GOGI_OPERA_PERSONAL_CERTIFICATES;
	}
	else
	{
		category = GOGI_OPERA_CERTIFICATE_AUTHORITIES;
	}
		
	if ( op_create_certificate_manager( getCorsair()->getOpera(), &pCertManagerHandle, category) != GOGI_STATUS_OK )
	{
		return NULL;
	}
	
	return pCertManagerHandle;
}

int	CorsairCertManager::freeGogiCertManagerHandle( GogiCertificateManager *handle )
{
	if ( getCorsair() == NULL || getCorsair()->getOpera() )
	{
		return 1;
	}
	
	op_close_certificate_manager( getCorsair()->getOpera(), handle, true );
	return 0;
}
int CorsairCertManager::CanSelectCertification(GogiOperaEventData *data)
{
	gogi_dialog_callback_t callback = data->dialog.callback;
	void* id = data->dialog.id;
	int 	result = 0, i;
#if 0
	LOG_DEBUG("check certs\n");
	for( i = 0; i < data->dialog.certificate.numcerts; i++ )
	{
		LOG_DEBUG(" id		   : %d \n",data->dialog.certificate.cert[i].id);
		LOG_DEBUG(" filename	   : %s \n",data->dialog.certificate.cert[i].filename);
		LOG_DEBUG(" shortName    : %s \n",data->dialog.certificate.cert[i].shortName);
		LOG_DEBUG(" fullName	   : %s \n",data->dialog.certificate.cert[i].fullName);
		LOG_DEBUG(" issuer	   : %s \n",data->dialog.certificate.cert[i].issuer);
		LOG_DEBUG(" validFrom    : %s \n",data->dialog.certificate.cert[i].validFrom);
		LOG_DEBUG(" validTo	   : %s \n",data->dialog.certificate.cert[i].validTo);
		LOG_DEBUG(" extraData    : %s \n",data->dialog.certificate.cert[i].extraData);
		LOG_DEBUG(" allowConnect : %d \n",data->dialog.certificate.cert[i].allowConnect);
		LOG_DEBUG(" warn		   : %d \n",data->dialog.certificate.cert[i].warn);
	}
	LOG_DEBUG("check certcomments\n");
	for( i = 0; i < data->dialog.certificate.num_certcomments; i++ )
	{
		LOG_DEBUG(" certcomments : %s \n",data->dialog.certificate.certcomments[i]);
	}
#endif
	if ( 0 >= data->dialog.certificate.numcerts )
	{
		LOG_DEBUG("data->dialog.certificate.numcerts <= 0.\n");
		return -1;
	}
	
	if ( NULL != data->dialog.origin )
	{
		LOG_DEBUG( "GOGI_DIALOG_TYPE_CLIENT_CERT_SELECT. %s \n",data->dialog.origin);
		for ( int i=0; i<data->dialog.certificate.numcerts; ++i )
		{
		if ( findMatchingCert(data->dialog.origin, data->dialog.certificate.cert[i].shortName) )
			{
				LOG_DEBUG("GOGI_DIALOG_TYPE_CLIENT_CERT_SELECT. => %s \n",data->dialog.certificate.cert[i].shortName);
				return i;
			}
		}
	}
	if ( getCommonNameOfDefaultCert() != NULL )
	{
		string default_cert_common_name = getCommonNameOfDefaultCert();
		for ( int i=0; i<data->dialog.certificate.numcerts; ++i )
		{
			/* to find the default cert */
			if ( default_cert_common_name.compare(data->dialog.certificate.cert[i].shortName ) == 0 )
			{
				/* if cert.conf file does not have cert file for current url, we select RTL cert */
				LOG_ERR("GOGI_DIALOG_TYPE_CLIENT_CERT_SELECT. non-registered=> %s\n",data->dialog.certificate.cert[i].shortName);
				return i;
			}
		}
	}
	LOG_ERR("GOGI_DIALOG_TYPE_CLIENT_CERT_SELECT. => GOGI_DIALOG_BUTTON_CANCEL\n");
	return -1;
}

bool CorsairCertManager::isAcceptableCertification(GogiOperaEventData *data)
{
	gogi_dialog_callback_t callback = data->dialog.callback;
	void* id = data->dialog.id;

	switch(data->dialog.dialog_context)
	{
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_NO_CHAIN:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_WRONG_NAME:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_EXPIRED: // if you need to pass currents, make a whitelist.
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_DISABLED_CIPHER:
			return false;
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_USAGE_WARNING:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_ANONYMOUS_CONNECTION:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_AUTHENTICATED_ONLY:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_LOW_ENCRYPTION:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_PASSWORD_IMPORT_CERTIFICATE:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_PASSWORD_EXPORT_CERTIFICATE:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_PASSWORD_UNLOCK_CERTIFICATE:
		case GOGI_DialogData::GOGI_DIALOG_CONTEXT_PASSWORD_WAND_ENCRYPTION:		
			break;
	}
	
	return true;
}

