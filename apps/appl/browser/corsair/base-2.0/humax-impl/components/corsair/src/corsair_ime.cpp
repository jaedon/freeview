/** **********************************************************************************************************
	File 		: corsair_ime.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/09/27
	attention 	: 
	
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


#include "corsair_ime.h"
#include "corsair.h"
#include "corsair_config.h"
#include "corsair_hama.h"

#include "hapi.h"

#include "gogi_opera_api.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

#define CORSAIR_IME_FORMAT_SIMPLE			"simple"
#define CORSAIR_IME_FORMAT_FULL				"full"
#define CORSAIR_IME_FORMAT_NONE				"none"

/**************************************************************************************
 * CorsairIme
 ***************************************************************************************/
CorsairIme::CorsairIme()
	: m_id(NULL)
{

}

CorsairIme::~CorsairIme()
{

}

bool CorsairIme::activate(void *id, bool password, int position, const char *format,
		string default_str, int max_length)
{
	/* Don't start the virtual keyboard */
	if ( format && strncmp(format, CORSAIR_IME_FORMAT_NONE,
			strlen(CORSAIR_IME_FORMAT_NONE)) == 0 ) return true;

	HAPI_BroadcastSignal("signal:onOSKModeChanged", 0, "i", password);

	if ( format == NULL )
	{
		HAPI_BroadcastSignal("signal:onOSKShow", 0, "s", const_cast<char *>(default_str.c_str()));
	}
	else if ( strncmp(format, CORSAIR_IME_FORMAT_SIMPLE,
			strlen(CORSAIR_IME_FORMAT_SIMPLE)) == 0 )
	{
		HAPI_BroadcastSignal("signal:onShowSimpleKeyboard", 0, "s", const_cast<char *>(default_str.c_str()));
	}
	else
	{
		HAPI_BroadcastSignal("signal:onOSKShow", 0, "s", const_cast<char *>(default_str.c_str()));
	}

	HAPI_ConnectSignal("signal:onOSKResult",  NULL, CorsairHama::onOSKResult);
	HAPI_ConnectSignal("signal:onSimpleKeyboardStr", NULL, CorsairHama::onSimpleKeyboardStr);
	HAPI_ConnectSignal("signal:onKeyboardStr", NULL, CorsairHama::onKeyboardStr);

	m_id = id;

	return true;
}

bool CorsairIme::deactivate()
{
	if ( m_id )
	{
		HAPI_DisconnectSignal("signal:onOSKResult", CorsairHama::onOSKResult);
		m_id = NULL;
	}

	return true;
}

bool CorsairIme::update(const char *str)
{
	if ( !str ) return false;

	if ( op_update_ime(m_id, (const char *)str, strlen(str), 0, 0) != GOGI_STATUS_OK )
	{
		m_id = NULL;
		LOG_DEBUG("ime is not activated, or op_update_ime() failed.\n");
		return false;
	}
	return true;
}

bool CorsairIme::update_ext(const char *str)
{
	if ( !str ) return false;

	CorsairImeInputData *data = new CorsairImeInputData();
	if ( !data ) return false;

	data->type = CORSAIR_IME_TYPE_UPDATE;
	data->input_str = str;

	m_imeQueue.push(data);

	return true;
}

bool CorsairIme::commit(const char *str)
{
	if ( !str ) return false;

	HAPI_DisconnectSignal("signal:onOSKResult", CorsairHama::onOSKResult);
	if(op_commit_ime(m_id, (const char *)str) != GOGI_STATUS_OK)
	{
		m_id = NULL;
		LOG_DEBUG("ime is not activated, or op_commit_ime() failed.\n");
		return false;
	}
	return true;
}

bool CorsairIme::commit_ext(const char *str)
{
	if ( !str ) return false;

	CorsairImeInputData *data = new CorsairImeInputData();
	if ( !data ) return false;

	data->type = CORSAIR_IME_TYPE_COMMIT;
	data->input_str = str;

	m_imeQueue.push(data);

	return true;
}

void CorsairIme::imeLoop()
{
	if ( m_imeQueue.empty() ) return;

	CorsairImeInputData *data = m_imeQueue.front();

	switch ( data->type )
	{
	case CORSAIR_IME_TYPE_UPDATE:
		update(data->input_str.c_str());
		break;

	case CORSAIR_IME_TYPE_COMMIT:
		commit(data->input_str.c_str());
		break;

	default:
		return;
	}

	delete data;

	m_imeQueue.pop();
}
