/** **********************************************************************************************************
	@file 		corsair_product_env.cpp

	@date		2013/12/31
	@author		Inseon Son (isson@humaxdigital.com)
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
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "corsair.h"
#include "corsair_product_env.h"
#include "corsair_xml.h"

#define DEFAULT_PRODUCT_FILE_PATH			"/usr/browser/opera_home/product.xml"

#define XML_PRODUCT							"product"

#define XML_INFO							"information"
#define XML_MEM								"memory"
#define XML_DIR								"directory"
#define XML_FONT							"font"
#define XML_ENABLED							"enabled"
#define XML_HBBTV							"hbbtv"

#define XML_INFO_PRODUCT_NAME				"product-name"
#define XML_INFO_MODEL_NAME					"model-name"
#define XML_INFO_WIDGET_EXIST				"widget"

#define XML_MEM_HEAP_LIMIT_SIZE				"heap-limit-size"
#define XML_MEM_ALLOCATED_LIMIT_SIZE		"allocated-limit-size"

#define XML_DIR_OPERA_WIDGET_INSTALL_DIR	"opera-widget-install-dir"
#define XML_DIR_OPERA_HOME_TARGET_DIR		"opera-home-target-dir"
#define XML_DIR_OPERA_HOME_SOURCE_DIR		"opera-home-source-dir"
#define XML_DIR_OPERA_DIR					"opera-dir"
#define XML_DIR_OPERA_FONT_DIR				"opera-font-dir"
#define XML_DIR_EXTERNAL_APP_URL            "external-app-url"

#define XML_FONT_DEFAULT					"default"
#define XML_FONT_CURSIVE					"cursive"
#define XML_FONT_FANTASY					"fantasy"
#define XML_FONT_MONOSPACE					"monospace"
#define XML_FONT_SANS_SERIF					"sans-serif"
#define XML_FONT_SERIF_FONT					"serif"

#define XML_ENABLED_DRAGONFLY				"dragonfly"
#define XML_ENABLED_URL						"url"
#define XML_ENABLED_MAINAPP					"mainapp"

#define XML_HBBTV_TRUSTAPP					"trusted-application"
#define XML_HBBTV_URL						"url"

using namespace std;

/*************************************************************************************************************
	Static member variables
*************************************************************************************************************/
CorsairXml *CorsairProductEnv::m_xml;
Information_t CorsairProductEnv::m_information;
Memory_t CorsairProductEnv::m_memory;
Directory_t CorsairProductEnv::m_directory;
Font_t CorsairProductEnv::m_font;
Enabled_t CorsairProductEnv::m_enabled;
vector<string> CorsairProductEnv::m_trust_applist;

/*************************************************************************************************************
	CorsairProductEnv class body
*************************************************************************************************************/
CorsairProductEnv::CorsairProductEnv()
{
}
CorsairProductEnv::~CorsairProductEnv()
{
}

bool CorsairProductEnv::initProductEnv()
{
	string product_file_path = DEFAULT_PRODUCT_FILE_PATH;
	CorsairXmlNode *root, *cur, *found;

	m_xml = new CorsairXml((const char *)product_file_path.c_str());

	root = m_xml->getRoot();
    if ( m_xml->getName(root) != XML_PRODUCT )
    {
		LOG_ERR("## cannot find root node/n ##\n");
		delete m_xml;
		return false;
	}

	cur = m_xml->child(root);

	// Information
	found = m_xml->find(cur, XML_INFO);
	if ( found )
		setInformation(found);

	// Memory
	found = m_xml->find(cur, XML_MEM);
	if ( found )
		setMemory(found);

 	// Directory
	found = m_xml->find(cur, XML_DIR);
	if ( found )
		setDirectory(found);

	// Font
	found = m_xml->find(cur, XML_FONT);
	if ( found )
		setFont(found);

	// Enabled
	found = m_xml->find(cur, XML_ENABLED);
	if ( found )
		setEnabled(found);

	found = m_xml->find(cur, XML_HBBTV);
	if ( found )
		setHbbTV(found);

	return true;
}

const char* CorsairProductEnv::getHbbTVTrustedApp(int idx)
{
	if( idx >= m_trust_applist.size())
		return NULL;
	return m_trust_applist.at(idx).c_str();
}

void CorsairProductEnv::printElements()
{

}

/*************************************************************************************************************
 * private functions
 *************************************************************************************************************/
void CorsairProductEnv::setInformation(CorsairXmlNode *node)
{
	CorsairXmlNode *info, *cur;

	cur = m_xml->child(node);

	info = m_xml->find(cur, XML_INFO_PRODUCT_NAME);
	if ( info  )
		m_information.product_name = m_xml->getContent(info);

	info = m_xml->find(cur, XML_INFO_MODEL_NAME);
	if ( info )
		m_information.model_name = m_xml->getContent(info);

	info = m_xml->find(cur, XML_INFO_WIDGET_EXIST);
	if ( info )
		m_information.widget = __to_boolean( m_xml->getContent(info) );
}

void CorsairProductEnv::setMemory(CorsairXmlNode *node)
{
	CorsairXmlNode *mem, *cur;

	cur = m_xml->child(node);

	mem = m_xml->find(cur, XML_MEM_HEAP_LIMIT_SIZE);
	if ( mem )
		m_memory.heap_limit_size = __to_number( m_xml->getContent(mem) );

	mem = m_xml->find(cur, XML_MEM_ALLOCATED_LIMIT_SIZE);
	if ( mem )
		m_memory.allocated_limit_size = __to_number( m_xml->getContent(mem) );
}

void CorsairProductEnv::setDirectory(CorsairXmlNode *node)
{
	CorsairXmlNode *dir, *cur;

	cur = m_xml->child(node);

	dir = m_xml->find(cur, XML_DIR_OPERA_WIDGET_INSTALL_DIR);
	if ( dir )
		m_directory.opera_widget_install_dir = m_xml->getContent(dir);

	dir = m_xml->find(cur, XML_DIR_OPERA_HOME_TARGET_DIR);
	if ( dir )
		m_directory.opera_home_target_dir = m_xml->getContent(dir);

	dir = m_xml->find(cur, XML_DIR_OPERA_HOME_SOURCE_DIR);
	if ( dir )
		m_directory.opera_home_source_dir = m_xml->getContent(dir);

	dir = m_xml->find(cur, XML_DIR_OPERA_DIR);
	if ( dir )
		m_directory.opera_dir = m_xml->getContent(dir);

	dir = m_xml->find(cur, XML_DIR_OPERA_FONT_DIR);
	if ( dir )
		m_directory.opera_font_dir = m_xml->getContent(dir);

	dir = m_xml->find(cur, XML_DIR_EXTERNAL_APP_URL);
	if ( dir )
	{
		m_directory.external_app_url = m_xml->getContent(dir);
	}

}

void CorsairProductEnv::setFont(CorsairXmlNode *node)
{
	CorsairXmlNode *font, *cur;

	cur = m_xml->child(node);

	font = m_xml->find(cur, XML_FONT_DEFAULT);
	if ( font )
		m_font.default_font = m_xml->getContent(font);

	font = m_xml->find(cur, XML_FONT_CURSIVE);
	if ( font )
		m_font.cursive_font = m_xml->getContent(font);

	font = m_xml->find(cur, XML_FONT_FANTASY);
	if ( font )
		m_font.fantasy_font = m_xml->getContent(font);

	font = m_xml->find(cur, XML_FONT_MONOSPACE);
	if ( font )
		m_font.monospace_font = m_xml->getContent(font);

	font = m_xml->find(cur, XML_FONT_SANS_SERIF);
	if ( font )
		m_font.sans_serif_font= m_xml->getContent(font);

	font = m_xml->find(cur, XML_FONT_SERIF_FONT);
	if ( font )
		m_font.serif_font = m_xml->getContent(font);
}

void CorsairProductEnv::setEnabled(CorsairXmlNode *node)
{
	CorsairXmlNode *enabled, *cur;

	cur = m_xml->child(node);

	enabled = m_xml->find(cur, XML_ENABLED_DRAGONFLY);
	if ( enabled )
		m_enabled.dragonfly = __to_boolean( m_xml->getContent(enabled) );

	enabled = m_xml->find(cur, XML_ENABLED_URL);
	if ( enabled )
		m_enabled.url = __to_boolean( m_xml->getContent(enabled) );

	enabled = m_xml->find(cur, XML_ENABLED_MAINAPP);
	if ( enabled )
		m_enabled.mainapp = __to_boolean( m_xml->getContent(enabled) );
}

void CorsairProductEnv::setHbbTV(CorsairXmlNode *node)
{
	CorsairXmlNode *trust_app, *cur, *url;

	cur = m_xml->child(node);
	trust_app = m_xml->find(cur, XML_HBBTV_TRUSTAPP);
	if ( trust_app )
	{
		trust_app = m_xml->child(trust_app);
		if ( trust_app != NULL )
		{
			do{
				url = m_xml->find(trust_app, XML_HBBTV_URL);
				if ( url )
				{
					m_trust_applist.push_back(m_xml->getContent(url));
				}
				trust_app = m_xml->next(url);
			}while( trust_app != NULL );
		}
	}
}

unsigned int CorsairProductEnv::__to_number(const std::string str)
{
	return atoi(str.c_str());
}

bool CorsairProductEnv::__to_boolean(const std::string str)
{
	if ( atoi(str.c_str()) == 1 )
		return true;

	return false;
}
