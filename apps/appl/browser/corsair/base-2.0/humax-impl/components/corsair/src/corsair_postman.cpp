/** **********************************************************************************************************
	@file 		corsair_postman.cpp

	@date		2013/03/21
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2012-2013 Humax Co., Ltd.
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
#include "asteriks.h"
#include "corsair_postman.h"
#include "corsair_ait_manager.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_otvs.h"
#include "corsair_tools.h"
#include "corsair.h"
#include "bplglobal.h"

#include "NativeApplicationManager.h"

#include <apk.h>
#include <hapi.h>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;


/** **************************************************************************************
	@class	CorsairPostman
****************************************************************************************** */
CorsairPostman::CorsairPostman()
{

}

CorsairPostman::~CorsairPostman()
{
	while ( !m_mail_box.empty() )
	{
		CorsairMail *mail = m_mail_box.front();
		delete mail;
		m_mail_box.pop();
	}
}

bool CorsairPostman::post(CorsairMail *mail)
{
	if ( !mail ) return false;

	m_mail_box.push(mail);

	return true;
}

void CorsairPostman::run()
{
	if ( m_mail_box.empty() ) return;

	CorsairMail *mail = m_mail_box.front();
	mail->execute();
    
	delete mail;
	m_mail_box.pop();
}

void CorsairMailOpenUrl::execute(void)
{
#ifdef OIPF
	if ( !m_url.empty() )
	{
		Asteriks *ast = static_cast<Asteriks *>(getCorsair());
		if ( !ast )
			return;

		ast->detachApplication();
		ast->attachApplication((const char*)m_url.c_str());
	}
#endif
	return;
}

void CorsairMailConnectDragonfly::execute(void)
{
	if ( !getCorsair() ) return;

	gogi_bool bconnected = 0;
	op_dragonfly_is_connected(&bconnected);

	if ( bconnected ) return;

	int i = 0;
	char *p = NULL;
	char temp[32] = {0,};
	char szIP[32] = {0,};
	char szPort[32] = {0,};

    char *psFilePath = CorsairTools::getUsbPath((HCHAR *)DEFAULT_DRAGONFLY_USB_PATH);

    if (psFilePath == NULL)
    {
        return;
    }

	FILE *fp = fopen(psFilePath, "r");
	if ( fp )
	{
		fgets(temp, sizeof(temp), fp);

		i = 0;
		p = temp;
		while ( (*p >= '0' && *p <= '9') || *p == '.' )
		{
			szIP[i++] = *p++;
		}

		fgets(temp, sizeof(temp), fp);
		i = 0;
		p = temp;
		while ( (*p >= '0' && *p <= '9') || *p == '.' )
		{
			szPort[i++] = *p++;
		}

		fclose(fp);

		op_dragonfly_enable();
		op_dragonfly_connect(szIP, atoi(szPort));
	}
}

void CorsairMailEnableHbbTV::execute(void)
{
	if ( data )
		CorsairAITManager::resume();
	else
		CorsairAITManager::suspend();
}

void CorsairMailCreateHumaxApplication::execute(void)
{
	LOG_DEBUG("CreateHumaxApplication\n");

#ifdef ENABLE_OPERA_TVSTORE
	if ( m_name.compare( "otvs" ) == 0 )
	{
		LOG_DEBUG(" Opera TV Store\n");
		if ( getCorsair() == NULL || getCorsair()->getOTVSHandle() == NULL )
			return;

		getCorsair()->getOTVSHandle()->startOTVSPage(m_url.c_str());
	}
#endif

#ifdef ENABLE_OPERA_TVBROWSER
	if ( m_name.compare( "otvb" ) == 0 )
	{
		LOG_DEBUG(" Opera TV Browser\n");
		pid_t pid;
		pid = fork();
		if (pid == (-1))
		{
			LOG_DEBUG("fork error\n");
			return;
		}
		else if (pid == 0)
		{
			char *argv[3];
			argv[0] = (char *)"surfer";
			argv[1] = (char *)m_url.c_str();
			argv[2] = NULL;

			static std::map<const char *, const char *> language_map;
			language_map["eng"] = "en";
			language_map["ger"] = "de";
			language_map["deu"] = "de";
			language_map["tur"] = "tr";
			language_map["ara"] = "ar";
			language_map["per"] = "fa";
			language_map["ces"] = "cs";
			language_map["rus"] = "ru";

			char lang[32] = {0,};
			APK_SCENARIO_GetMainLanguage(lang);
			lang[3] = '\0';

			map<const char *, const char *>::iterator it;
			for ( it=language_map.begin(); it!=language_map.end(); ++it )
			{
				if ( strncmp(it->first, lang, strlen(lang)) == 0 )
				{
					setenv("LANG", it->second, 1);
				}
			}
		    HAPI_BroadcastSignal("signal:onHumaxApplicationLoaded", 1, "s", "otvb");
			execv("/home/surfer/surfer", (char **)argv);
		}
	}
#endif

#ifdef ENABLE_OPERA_HBBTV
	if ( m_name.compare( "hbbtv" ) == 0 )
	{
		if ( !getCorsair() ) return;

		LOG_DEBUG(" Opera HbbTV\n");

    	CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    	if ( hbbtv_manager )
    		hbbtv_manager->action(new CorsairHbbTVBIAppStartAction(m_url.c_str(), hbbtv_manager));
	}
#endif
#if defined(CONFIG_PROD_FVP4000T)
	if ( m_name.compare( "mheg" ) == 0 )
	{
		LOG_DEBUG("Mheg app on\n");

		HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 1, "i", TRUE);
	}
#endif
}

void CorsairMailDestroyHumaxApplication::execute(void)
{
	LOG_DEBUG("DestroyHumaxApplication\n");

#ifdef ENABLE_OPERA_TVSTORE
	if ( m_name.compare( "otvs" ) == 0 )
	{
		LOG_DEBUG(" Opera TV Store\n");
		if ( getCorsair() == NULL || getCorsair()->getOTVSHandle() == NULL )
			return;

		if ( getCorsair()->getOTVSHandle()->getOTVSApplicationWindow() != NULL )
		{
			getCorsair()->getOTVSHandle()->stopOTVSApplication();
		}

		if ( getCorsair()->getOTVSHandle()->getErrorPageWindow() != NULL )
		{
			getCorsair()->getOTVSHandle()->stopErrorPage();
		}

		if ( getCorsair()->getOTVSHandle()->getOTVSPageWindow() != NULL )
		{
			getCorsair()->getOTVSHandle()->stopOTVSPage();
		}
	}
#endif

#ifdef ENABLE_OPERA_TVBROWSER
	if ( m_name.compare( "otvb" ) == 0 )
	{
		LOG_DEBUG(" Opera TV Browser\n");
		system("killall surfer");
		HAPI_BroadcastSignal("signal:onHumaxApplicationUnloaded", 1, "s", "otvb");
	}
#endif

#ifdef ENABLE_OPERA_HBBTV
	if ( m_name.compare( "hbbtv" ) == 0 )
	{
		LOG_DEBUG(" Opera HbbTV BI app\n");
		CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
		if ( hbbtv_manager )
		{
			CorsairHbbTVApp *biapp = hbbtv_manager->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
			if ( biapp )
			{
				biapp->stop();
			}
#if defined(CONFIG_PROD_FVP4000T)
			CorsairHbbTVApp *brapp = hbbtv_manager->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);
			if ( brapp )
			{
				brapp->stop();
			}
#endif
		}
	}
#endif
#if defined(CONFIG_PROD_FVP4000T)
	if ( m_name.compare( "mheg" ) == 0 )
	{
		LOG_DEBUG("Mheg app off\n");

		HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 2, "ii", FALSE, 1);
	}
#endif
}

CorsairMailSetSystemLanguage::CorsairMailSetSystemLanguage(const char* lang_code)
{
    std::map<const char *, const char *> language_map;
	language_map["eng"] = "en-US";
	language_map["ger"] = "de-DE";
	language_map["deu"] = "de-DE";
	language_map["tur"] = "tr-TR";
	language_map["ara"] = "ar-EG";
	language_map["per"] = "fa-IR";
	language_map["ces"] = "cs-CZ";
	language_map["rus"] = "ru-RU";
	language_map["fre"] = "fr-FR";
	language_map["ita"] = "it-IT";
	language_map["spa"] = "es-ES";
	language_map["gre"] = "el-EL";

	map<const char *, const char *>::iterator it;
	for ( it=language_map.begin(); it!=language_map.end(); ++it )
	{
		if ( strncmp(it->first, lang_code, strlen(lang_code)) == 0 )
		{
			m_language_code = it->second;
			break;
		}
	}
    
    if ( it==language_map.end() )
        m_language_code = "en-US";
}

void CorsairMailSetSystemLanguage::execute(void)
{
	if ( !getCorsair() ) return;

	op_set_pref(getCorsair()->getOpera(), "Network", "HTTP Accept Language", m_language_code.c_str());
}

void CorsairMailResendAIT::execute()
{
    CorsairAITManager::resendAIT();
    return;
}

CorsairMailSetMhegStatus::CorsairMailSetMhegStatus(bool status)
    : m_status(status)
{
    
}

void CorsairMailSetMhegStatus::execute()
{
    BplGlobalConfig::s_mheg_status = m_status;
    return;
}

/** **************************************************************************************
	private interfaces
****************************************************************************************** */

