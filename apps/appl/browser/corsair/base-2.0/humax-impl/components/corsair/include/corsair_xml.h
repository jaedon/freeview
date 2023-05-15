/** **********************************************************************************************************
	@file 		corsair_xml.h
	
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

#ifndef __CORSAIR_XML_H__
#define __CORSAIR_XML_H__

#include <string>

#define MAX_BUF_SIZE 1024*100

typedef void CorsairXmlNode;
typedef void CorsairXmlDoc;

/**************************************************************************************
 * class CorsairXml
 **************************************************************************************/
class CorsairXml {
public:
	/**
		@fn 	CorsairXml
		@breif 	constructure of this class
		@param	doc_name	[INPUT]	the path of XML document file
		@return N/A
	*/
	CorsairXml(const char *doc_name);
	virtual ~CorsairXml();

	/**
		@fn 	getRoot
		@breif 	get the root element in the XML Document
		@return	handle of the node or NULL
	*/
	CorsairXmlNode *getRoot();

	/**
		@fn 	child
		@breif 	get the child element of current node
		@param	cur		[INPUT]	handle of current node
		@return	handle of the node or NULL
	*/	
	CorsairXmlNode *child(CorsairXmlNode *cur);

	/**
		@fn 	next
		@breif 	get the next element of current node
		@param	cur		[INPUT]	handle of current node		
		@return	handle of the node or NULL
	*/		
	CorsairXmlNode *next(CorsairXmlNode *cur);

	/**
		@fn 	parent
		@breif 	get the parent element of current node
		@param	cur		[INPUT]	handle of current node		
		@return	handle of the node or NULL
	*/			
	CorsairXmlNode *parent(CorsairXmlNode *cur);

	/**
		@fn 	find
		@breif 	find the equivalent node of name input
		@param	cur		[INPUT]	handle of current node		
		@param	name	[INPUT]	name of element to find				
		@return	handle of the node or NULL
	*/				
	CorsairXmlNode *find(CorsairXmlNode *cur, const char *name);

	/**
		@fn 	getName
		@breif 	get the name of current node
		@param	cur		[INPUT]	handle of current node		
		@return	string of the node's name or NULL
	*/					
	std::string getName(CorsairXmlNode *cur);

	/**
		@fn 	getContent
		@breif 	get the content of current node
		@param	cur		[INPUT]	handle of current node		
		@return	string of the node's content or NULL
	*/						
	std::string getContent(CorsairXmlNode *cur);	

	/**
		@fn 	getAttributeValue
		@breif 	get the attribute's value of current node
		@param	cur		[INPUT]	handle of current node		
		@return	string of the node's value of attribute or NULL
	*/						
	std::string getAttributeValue(CorsairXmlNode *cur, const char *name);

private:
	CorsairXmlDoc *m_doc;
};

#endif	/* __CORSAIR_XML_H__ */
