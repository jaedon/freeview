/** **********************************************************************************************************
	File 		: corsair_ime_jlabs.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/11/13
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


#include "corsair.h"
#include "corsair_config.h"
#include "corsair_ime_jlabs.h"
#include "corsair_hama.h"

#include "hapi.h"

#include "gogi_opera_api.h"

#include <cstdio>
#include <cstdlib>
#include <iostream> // just for debug cout

#define prefix_modeset 	"_@modeset:"
#define prefix_cancel 	"_@cancel:;"

using namespace std;

//same value with SMS_INPUT_AVAILABLE in kbd_keitai_jap.h
typedef	enum
{	KBD_NUMERIC_AVAILABLE = 0x01,
	KBD_ALPHABET_AVAILABLE = 0x02,
	KBD_KATAKANA_AVAILABLE = 0x04,
	KBD_KANJI_AVAILABLE = 0x08,
} INPUT_AVAILABLE;

#define	KBD_ALL_AVAILABLE 	( KBD_NUMERIC_AVAILABLE | KBD_ALPHABET_AVAILABLE | KBD_KATAKANA_AVAILABLE | KBD_KANJI_AVAILABLE )

/**************************************************************************************
 * CorsairImeJlabs
 ***************************************************************************************/
CorsairImeJlabs::CorsairImeJlabs()
	: m_use_prefix(false)
{

}

CorsairImeJlabs::~CorsairImeJlabs()
{

}

bool CorsairImeJlabs::activate(void *id, bool password, int position, const char *format,
		string default_str, int max_length)
{
	int prefix_length = 0;
	int charactermode = 0x00;
	m_id = id;
	m_use_prefix = parsePrefix((const char *) default_str.c_str(), &prefix_length,&charactermode);
	HAPI_BroadcastSignal("signal:onOSKModeChanged", 0, "i", password);
	HAPI_BroadcastSignal("signal:onOSKDefaultString", 0, "s", const_cast<char *>(default_str.c_str()+ prefix_length));
	HAPI_BroadcastSignal("signal:onOSKCharModeChanged", 0, "i", charactermode);
	if(max_length > 0)
		HAPI_BroadcastSignal("signal:onOSKMaxLengthChanged", 0, "i", max_length);

	HAPI_ConnectSignal("signal:onOSKResult",  NULL, CorsairHama::onOSKResult);
	HAPI_ConnectSignal("signal:onOSKCancel",  NULL, CorsairHama::onOSKCancel);
	HAPI_BroadcastSignal("signal:onOSKShow", 0, "i", 1);
	return true;
}

bool CorsairImeJlabs::deactivate()
{
	m_id = NULL;
	LOG_DEBUG(" CorsairImeJlabs::deactivate() \n");
	HAPI_DisconnectSignal("signal:onOSKResult",  CorsairHama::onOSKResult);
	HAPI_DisconnectSignal("signal:onOSKCancel",  CorsairHama::onOSKCancel);
	HAPI_BroadcastSignal("signal:onOSKDestroy", 0, "i", 0);
	HAPI_BroadcastSignal("signal:onOSKShow", 0, "i", 0);
	return true;
}

bool CorsairImeJlabs::commit(const char *str)
{
	const char * commit_str = NULL; //op_commit_ime with NULL means restore input.
	if(strncmp(str, prefix_cancel, strlen(prefix_cancel)) == 0 && !m_use_prefix)
		commit_str = NULL;
	else
		commit_str = str;

	HAPI_DisconnectSignal("signal:onOSKResult",  CorsairHama::onOSKResult);
	HAPI_DisconnectSignal("signal:onOSKCancel",  CorsairHama::onOSKCancel);
	HAPI_BroadcastSignal("signal:onOSKShow", 0, "i", 0);
	if(op_commit_ime(m_id, (const char *)commit_str) != GOGI_STATUS_OK)
	{
		m_id = NULL;
		LOG_DEBUG("ime is not activated, or op_commit_ime() failed.\n");
		return false;
	}
	return true;
}

bool CorsairImeJlabs::commit_ext(const char *str)
{
	string ime_str;

	if ( !str )
		ime_str = prefix_cancel;
	else
		ime_str = str;
	
	CorsairImeInputData *data = new CorsairImeInputData();
	if ( !data ) return false;

	data->type = CORSAIR_IME_TYPE_COMMIT;
	data->input_str = ime_str;

	m_imeQueue.push(data);

	return true;
}

void CorsairImeJlabs::imeLoop()
{
	if ( m_imeQueue.empty() ) return;

	CorsairImeInputData *data = m_imeQueue.front();

	switch ( data->type )
	{
	case CORSAIR_IME_TYPE_COMMIT:
		CorsairImeJlabs::commit(data->input_str.c_str());
		break;

	default:
		return;
	}

	delete data;

	m_imeQueue.pop();
}

bool CorsairImeJlabs::parsePrefix(const char * prefixStr,int * prefix_length, int * mode)
{
	int input_mode = 0x00;
	int pos = strlen(prefix_modeset);
	
	if(strlen(prefixStr) < pos || strncmp(prefixStr, prefix_modeset, pos)!= 0 )
	{
		*prefix_length = 0;
		*mode = KBD_ALL_AVAILABLE; // if no prefix default to all input mode. 
		return false;	// do not use prefix 
	}
	
	if(prefixStr[pos] == ';') // all input mode
	{
		*prefix_length = pos+1;
		*mode = KBD_ALL_AVAILABLE;  // _@modeset:; means all inputmode.
		return true;
	}
	
	while(prefixStr[pos] != ';')
	{
		if(strncmp(&prefixStr[pos], "KAT", 3) == 0)
		{
			input_mode = input_mode | KBD_KATAKANA_AVAILABLE; // set katakana mode
			pos +=3;
		}else if(strncmp(&prefixStr[pos], "HIR", 3) == 0)
		{
			input_mode = input_mode | KBD_KANJI_AVAILABLE; // set hiragana mode
			pos +=3;
		}else if(strncmp(&prefixStr[pos], "ENG", 3) == 0)
		{
			input_mode = input_mode | KBD_ALPHABET_AVAILABLE; // set english mode
			pos +=3;
		}else if(strncmp(&prefixStr[pos], "NUM", 3) == 0)
		{
			input_mode = input_mode | KBD_NUMERIC_AVAILABLE; // set numeric mode
			pos +=3;
		}else if(prefixStr[pos] == '+')
		{
			pos +=1;
		}else
			break;
	}

	if(prefixStr[pos] == ';')
	{	
		*prefix_length = pos+1;
		*mode = input_mode;
		LOG_DEBUG("calculated input mode is : %d \n",input_mode);
		return true;
	}

	*mode = KBD_ALL_AVAILABLE;
	*prefix_length = 0;
	return false;	// invalid prefix 
}
