/** **********************************************************************************************************
	@file 		mediasource.h
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		Media Source
	
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

#ifndef __MEDIA_SOURCE_H__
#define __MEDIA_SOURCE_H__

#include <string>

/*--------------------------------------------------------------------------------------------
 * class MediaSource
 *-------------------------------------------------------------------------------------------*/
class MediaSource {
public:
	enum eSourceType {
		EXTERNAL	= 0,
		MSE
	};

	MediaSource()
		: m_source_type(EXTERNAL) {}
	MediaSource(const char *mime_type)
		: m_source_type(EXTERNAL), m_mime_type(mime_type) {}
	MediaSource(const char *mime_type, const char *url)
		: m_source_type(EXTERNAL), m_mime_type(mime_type), m_url(url) {}
	virtual ~MediaSource() {}

	eSourceType getSourceType() { return m_source_type; }
	std::string &getUrl() { return m_url; }
	void setUrl(const char *url) { m_url = url; }
	std::string &getMimeType() { return m_mime_type; }
	void setMimeType(const char *mime_type) { m_mime_type = mime_type; }

protected:
	eSourceType		m_source_type;
	std::string		m_mime_type;
	std::string		m_url;
};

#endif	/* __MEDIA_SOURCE_H__ */
