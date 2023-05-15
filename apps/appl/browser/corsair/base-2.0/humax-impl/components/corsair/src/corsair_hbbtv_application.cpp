/** **********************************************************************************************************
	@file 		corsair_hbbtv_application.cpp

	@date		2014/11/29
	@author
	@breif

	(c) 2014 Humax Co., Ltd.
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

#include "corsair_hbbtv_application.h"
#include "corsair_tools.h"
#include "corsair_hbbtv_surface.h"
#include "corsair.h"

#include "oplvod.h"

#include "hapi.h"

#include <cstdlib>
#include <iostream>

using namespace std;

/**************************************************************************************
 * class CorsairHbbTVApp
 **************************************************************************************/
CorsairHbbTVApp::CorsairHbbTVApp()
	: m_appid(0), m_orgid(0), m_broadcast_related(false), m_apptype(HBBTV_APPTYPE_NONE), m_is_running(false), m_is_started_mheg(false)
{
	m_url = "";
}

CorsairHbbTVApp::~CorsairHbbTVApp()
{
}

bool CorsairHbbTVApp::show()
{
	CorsairHbbTVSurface *hbbtv_surface = getCorsair()->getHbbTVSurface();
	if ( !hbbtv_surface ) return false;

	hbbtv_surface->showWindow(hbbtv_surface->getWindow());
	return true;
}

bool CorsairHbbTVApp::hide()
{
	CorsairHbbTVSurface *hbbtv_surface = getCorsair()->getHbbTVSurface();
	if ( !hbbtv_surface ) return false;

	hbbtv_surface->hideWindow(hbbtv_surface->getWindow());
	return true;
}

bool CorsairHbbTVApp::stop_by_system()
{
	CorsairTools::clearPanelString();

	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
	opera_hbbtv_send_event(E_TERMINATE_APP, data);

	m_is_running = false;

	return true;
}

/**************************************************************************************
 * class CorsairHbbTVBRApp
 **************************************************************************************/
CorsairHbbTVBRApp::CorsairHbbTVBRApp()
{
	m_broadcast_related = true;
}

CorsairHbbTVBRApp::CorsairHbbTVBRApp(unsigned short appid, unsigned long orgid, const char *url)
{
    m_appid = appid;
    m_orgid = orgid;
    if ( url ) m_url = url;
    m_broadcast_related = true;
}

CorsairHbbTVBRApp::~CorsairHbbTVBRApp()
{
}

bool CorsairHbbTVBRApp::run()
{
	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.appId = m_appid;
	data->app.orgId = m_orgid;
	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
	opera_hbbtv_send_event(E_START_APP, data);

	return true;
}

bool CorsairHbbTVBRApp::stop()
{
	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
	opera_hbbtv_send_event(E_TERMINATE_APP, data);

	return true;
}

/**************************************************************************************
 * class CorsairHbbTVBRAutoApp
 **************************************************************************************/
CorsairHbbTVBRAutoApp::CorsairHbbTVBRAutoApp()
{
	m_apptype = HBBTV_APPTYPE_BR_AUTOSTART;
}

CorsairHbbTVBRAutoApp::CorsairHbbTVBRAutoApp(unsigned short appid, unsigned long orgid, const char *url)
	: CorsairHbbTVBRApp(appid, orgid, url)
{
	m_apptype = HBBTV_APPTYPE_BR_AUTOSTART;
}

CorsairHbbTVBRAutoApp::~CorsairHbbTVBRAutoApp()
{
}

/**************************************************************************************
 * class CorsairHbbTVBRTeletextApp
 **************************************************************************************/
CorsairHbbTVBRTeletextApp::CorsairHbbTVBRTeletextApp()
{
	m_apptype = HBBTV_APPTYPE_BR_TELETEXT;
}

CorsairHbbTVBRTeletextApp::CorsairHbbTVBRTeletextApp(unsigned short appid, unsigned long orgid, const char *url)
	: CorsairHbbTVBRApp(appid, orgid, url)
{
	m_apptype = HBBTV_APPTYPE_BR_TELETEXT;
}

CorsairHbbTVBRTeletextApp::~CorsairHbbTVBRTeletextApp()
{
}

/**************************************************************************************
 * class CorsairHbbTVBIApp
 **************************************************************************************/
CorsairHbbTVBIApp::CorsairHbbTVBIApp()
{
	m_broadcast_related = false;
	m_apptype = HBBTV_APPTYPE_BI;
}

CorsairHbbTVBIApp::CorsairHbbTVBIApp(const char *url)
{
	m_broadcast_related = false;
	m_apptype = HBBTV_APPTYPE_BI;
	if ( url ) m_url = url;
}

CorsairHbbTVBIApp::~CorsairHbbTVBIApp()
{
}

bool CorsairHbbTVBIApp::run()
{
	CorsairTools::subtitleOff();
	CorsairTools::setPreferenceLanguage();;
	CorsairTools::stopLiveTV();
	CorsairTools::setPanelString("tv portal");
	OplVod::setAutoMode(false);

	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.url = strdup(m_url.c_str());
	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
	opera_hbbtv_send_event(E_START_BROADCAST_INDEPENDENT_APP, data);

	m_is_running = true;

	return true;
}

bool CorsairHbbTVBIApp::run_without_liveoff()
{
	CorsairTools::subtitleOff();
	CorsairTools::setPreferenceLanguage();;
	CorsairTools::setPanelString("tv portal");
	OplVod::setAutoMode(false);

	char buf[1024] = {0,};
	if ( CorsairTools::getUrlfromUsb(buf, sizeof(buf)) )
		m_url = string(buf);

	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.url = strdup(m_url.c_str());
	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();

	opera_hbbtv_send_event(E_START_BROADCAST_INDEPENDENT_APP, data);

	m_is_started_mheg = true;
	m_is_running = true;

	return true;
}

bool CorsairHbbTVBIApp::stop()
{
	CorsairTools::startLiveTV();
	CorsairTools::subtitleOn();
	CorsairTools::clearPanelString();
	OplVod::setAutoMode(true);

	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	if ( !data ) return false;

	data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
	opera_hbbtv_send_event(E_TERMINATE_APP, data);
	m_is_running = false;

	return true;
}

#if defined(CONFIG_PROD_FVP4000T)
/**************************************************************************************
 * class CorsairHbbTVBRXmlAIT
 **************************************************************************************/
CorsairHbbTVBRXmlAIT::CorsairHbbTVBRXmlAIT()
{
	m_apptype = HBBTV_APPTYPE_BR_XMLAIT;
}

CorsairHbbTVBRXmlAIT::CorsairHbbTVBRXmlAIT(unsigned short appid, unsigned long orgid, const char *url)
	: CorsairHbbTVBRApp(appid, orgid, url)
{
	m_apptype = HBBTV_APPTYPE_BR_XMLAIT;
}

CorsairHbbTVBRXmlAIT::~CorsairHbbTVBRXmlAIT()
{
}

/**************************************************************************************
 * class CorsairHbbTVBRPresentApp
 **************************************************************************************/
CorsairHbbTVBRPresentApp::CorsairHbbTVBRPresentApp()
{
	m_apptype = HBBTV_APPTYPE_BR_PRESENT;
}

CorsairHbbTVBRPresentApp::CorsairHbbTVBRPresentApp(unsigned short appid, unsigned long orgid, const char *url)
	: CorsairHbbTVBRApp(appid, orgid, url)
{
	m_apptype = HBBTV_APPTYPE_BR_PRESENT;
}

CorsairHbbTVBRPresentApp::~CorsairHbbTVBRPresentApp()
{
}

#endif
