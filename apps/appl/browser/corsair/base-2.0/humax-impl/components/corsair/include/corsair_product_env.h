/** **********************************************************************************************************
	@file 		corsair_product_env.h

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

#ifndef __CORSAIR_PRODUCT_ENV_H__
#define __CORSAIR_PRODUCT_ENV_H__

#include <string>
#include <vector>
#include "corsair_xml.h"

using namespace std;

typedef	struct
{
	std::string product_name;
	std::string model_name;
	bool widget;
} Information_t;

typedef	struct
{
	unsigned int heap_limit_size;
	unsigned int allocated_limit_size;
} Memory_t;

typedef	struct
{
	std::string opera_widget_install_dir;
	std::string opera_home_target_dir;
	std::string opera_home_source_dir;
	std::string opera_dir;
	std::string opera_font_dir;
    std::string external_app_url;
} Directory_t;

typedef	struct
{
	std::string default_font;
	std::string cursive_font;
	std::string fantasy_font;
	std::string monospace_font;
	std::string sans_serif_font;
	std::string serif_font;
} Font_t;

typedef	struct
{
	bool dragonfly;
	bool url;
	bool mainapp;
} Enabled_t;

/**************************************************************************************
 * class CorsairProductEnv
 **************************************************************************************/
class CorsairProductEnv {
public:
	/**
		@fn 	CorsairProductEnv
		@breif 	constructure of this class
		@return N/A
	*/
	CorsairProductEnv();
	virtual ~CorsairProductEnv();

public:
	/**
		@fn 	initProductEnv
		@breif 	the function to load product.xml, parse all elements and save to each variables.
		@param	N/A
		@return true or false
	*/
	static bool initProductEnv();

	/**
		@fn 	getInformation
		@breif 	the function get children of  the element <information>
		@param	N/A
		@return address of struct Information_t
	*/
	static Information_t *getInformation() { return &m_information; }

	/**
		@fn 	getExistedWidget
		@breif 	the function get the element <widget>
		@param	N/A
		@return member variable(widget) of struct Information_t
	*/
	static bool getExistedWidget() { return m_information.widget; }

	/**
		@fn 	getMemory
		@breif 	the function get children of  the element <memory>
		@param	N/A
		@return address of struct Memory_t
	*/
	static Memory_t *getMemory() { return &m_memory; }

	/**
		@fn 	getDirectory
		@breif 	the function get children of  the element <directory>
		@param	N/A
		@return address of struct Directory_t
	*/
	static Directory_t *getDirectory() { return &m_directory; }

	/**
		@fn 	getFont
		@breif 	the function get children of  the element <font>
		@param	N/A
		@return address of struct Font_t
	*/
	static Font_t *getFont() { return &m_font; }

	/**
		@fn 	getEnabled
		@breif 	the function get children of  the element <enabled>
		@param	N/A
		@return address of struct Enabled_t
	*/
	static Enabled_t *getEnabled() { return &m_enabled; }

	/**
		@fn 	getEnabledDragonfly
		@breif 	the function get the element <dragonfly>
		@param	N/A
		@return dragonfly of Enabled_t
	*/
	static bool getEnabledDragonfly() { return m_enabled.dragonfly; }

	/**
		@fn 	getEnabled
		@breif 	the function get children of  the element <enabled>
		@param	N/A
		@return address of struct Enabled_t
	*/
	static bool getEnabledUrl() { return m_enabled.url; }

	/**
		@fn 	getEnabled
		@breif 	the function get children of  the element <enabled>
		@param	N/A
		@return address of struct Enabled_t
	*/
	static bool getEnabledMainapp() { return m_enabled.mainapp; }

	/**
		@fn 	getNumberOfHbbTVTrustedApp
		@breif
		@param	N/A
		@return address of struct Enabled_t
	*/
	static int getNumberOfHbbTVTrustedApp() { return m_trust_applist.size(); }

	/**
		@fn 	getEnabled
		@breif 	the function get children of  the element <enabled>
		@param	N/A
		@return address of struct Enabled_t
	*/
	static const char* getHbbTVTrustedApp(int idx);

	/**
		@fn 	printElements
		@breif print all elements parsed in product.xml
		@param	N/A
		@return N/A
	*/
	static void printElements();

private:
	/**
		@fn 	setProduct
		@breif 	the function set children of  the element <information>
		@param	address of current node
		@return N/A
	*/
	static void setInformation(CorsairXmlNode *node);

	/**
		@fn 	setMemory
		@breif 	the function set children of  the element <memory>
		@param	address of current node
		@return N/A
	*/
	static void setMemory(CorsairXmlNode *node);

	/**
		@fn 	setDirectory
		@breif 	the function set children of  the element <directory>
		@param	address of current node
		@return N/A
	*/
	static void setDirectory(CorsairXmlNode *node);

	/**
		@fn 	setFont
		@breif 	the function set children of  the element <font>
		@param	address of current node
		@return N/A
	*/
	static void setFont(CorsairXmlNode *node);

	/**
		@fn 	setEnabled
		@breif 	the function set children of  the element <enabled>
		@param	address of current node
		@return N/A
	*/
	static void setEnabled(CorsairXmlNode *node);

	/**
		@fn 	setHbbTV
		@breif 	the function set children of  the element <hbbtv>
		@param	address of current node
		@return N/A
	*/
	static void setHbbTV(CorsairXmlNode *node);

	static unsigned int __to_number(const std::string str);

	static bool __to_boolean(const std::string str);

private:
	/** variables to parse related to XML */
	static CorsairXml *m_xml;

	/** children of element <information> in product.xml*/
	static Information_t m_information;

	/** children of element <memory> in product.xml*/
	static Memory_t m_memory;

	/** children of element <directory> in product.xml*/
	static Directory_t m_directory;

	/** children of element <fonts> in product.xml*/
	static Font_t m_font;

	/** children of element <release-enabled> in product.xml*/
	static Enabled_t m_enabled;

	static vector<string> m_trust_applist;
};

#endif	/* __CORSAIR_PRODUCT_ENV_H__ */
