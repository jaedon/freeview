/** **********************************************************************************************************
	@file 		corsair_hbbtv_app.cpp
	
	@date		2012/09/13
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

#include "corsair_hbbtv_app.h"

#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

/**********************************************************************************
 * static variables
 **********************************************************************************/


/********************************************************************************************************
 * CorsairHbbTVApplication
 ********************************************************************************************************/
CorsairHbbTVApplication::CorsairHbbTVApplication(eAppType type, unsigned short app_id, unsigned long org_id, string url)
{
	m_app_type = type;
	m_app_id = app_id;
	m_org_id = org_id;
	m_url = url;
}

CorsairHbbTVApplication::~CorsairHbbTVApplication()
{
}

eAppType CorsairHbbTVApplication::getApplicationType()
{
	return m_app_type;
}

unsigned short CorsairHbbTVApplication::getApplicationID()
{
	return m_app_id;
}

unsigned long CorsairHbbTVApplication::getOrganizationID()
{
	return m_org_id;
}

std::string &CorsairHbbTVApplication::getUrl()
{
	return m_url;
}

void CorsairHbbTVApplication::setUrl(std::string url)
{
	m_url = url;
}
