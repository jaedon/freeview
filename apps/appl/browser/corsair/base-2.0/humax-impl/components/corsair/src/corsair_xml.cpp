/** **********************************************************************************************************
	@file 		corsair_xml.cpp
	
	@date		2012/10/22
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

#include "corsair_log.h"
#include "corsair_xml.h"

#include <libxml/tree.h>
#include <string.h>

using namespace std;

/********************************************************************************************************
 * static functions
 ********************************************************************************************************/
static inline xmlNodePtr __xnode(CorsairXmlNode *node)
{
	return static_cast<xmlNodePtr>(node);
}

static inline CorsairXmlNode* __cnode(CorsairXmlNode *node)
{
	return static_cast<CorsairXmlNode*>(node);
}

static inline xmlDocPtr __doc(CorsairXmlDoc *doc)
{
	return static_cast<xmlDocPtr>(doc);
}

/********************************************************************************************************
 * CorsairXml
 ********************************************************************************************************/
CorsairXml::CorsairXml(const char *doc_name)
{
	xmlDocPtr doc = xmlParseFile(doc_name);

	m_doc = static_cast<CorsairXmlDoc *>(doc);

	if ( m_doc == NULL )
		LOG_ERR("Cannot parse this XML document.\n");
}

CorsairXml::~CorsairXml()
{
	xmlFreeDoc(__doc(m_doc));
}

CorsairXmlNode* CorsairXml::getRoot()
{
	xmlDocPtr doc = __doc(m_doc);
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
	return __cnode(root);
}

CorsairXmlNode* CorsairXml::child(CorsairXmlNode *cur)
{
	xmlNodePtr node = __xnode(cur);

	node = node->children;
	if ( node == NULL )
		return NULL;	

	string node_name = getName(node);
	if ( strcmp(node_name.c_str(), "text") == 0 )
		node = node->next;
	
	return __cnode(node);
}

CorsairXmlNode* CorsairXml::next(CorsairXmlNode *cur)
{
	xmlNodePtr node = __xnode(cur);

	if ( node == NULL )
		return NULL;

	node = node->next;
	if ( node == NULL )
		return NULL;

	string node_name = getName(node);
	if ( strcmp(node_name.c_str(), "text") == 0 )
		node = node->next;
	
	return __cnode(node);
}

CorsairXmlNode* CorsairXml::parent(CorsairXmlNode *cur)
{
	xmlNodePtr node = __xnode(cur);

	node = node->parent;
	if ( node == NULL )
		return NULL;

	string node_name = getName(node);	
	if ( strcmp(node_name.c_str(), "text") == 0 )
		node = node->next;
	
	return __cnode(node);
}

CorsairXmlNode* CorsairXml::find(CorsairXmlNode *cur, const char *name)
{
	xmlNodePtr node = __xnode(cur);

	while ( node != NULL )
	{
		string node_name = getName(node);	
		if ( strcmp(node_name.c_str(), name) != 0 )
		{
			node = __xnode( next(node) );
			return find(__cnode(node), name);
		}
		else
		{
			return __cnode(node);			
		}
	}
	
	return NULL;
}


string CorsairXml::getName(CorsairXmlNode *cur)
{
	xmlNodePtr node = __xnode(cur);
	string name((char *)node->name);
	
	return name;
}

string CorsairXml::getContent(CorsairXmlNode *cur)
{
	xmlNodePtr node = __xnode(cur);
	xmlChar* temp = xmlNodeGetContent(node);	
	string content((char *)temp);

	xmlFree(temp);
	
	return content;
}

string CorsairXml::getAttributeValue(CorsairXmlNode *cur, const char *name)
{
	xmlNodePtr node = __xnode(cur);

	if ( strcmp((char *)(node->name), name) != 0 )
		return NULL;

	string attribute((char *)node->properties->children->content);
	
	return attribute;
}

