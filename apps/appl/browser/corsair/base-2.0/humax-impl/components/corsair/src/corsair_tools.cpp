/**************************************************************************************/
/**
 * @file corsair_tools.cpp
 *
 * Corsair Tools
 *
 * @author  IPM-SW1T
 * @date    2012/09/24
 * @attention   (c) 2011-2012 Humax Co., Ltd.

 *	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
 *	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 *	non-assignable, non-transferable and non-exclusive license to use this Software.
 *	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 *	you agree to the responsibility to take all reasonable efforts to protect the any information
 *	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 *	reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 *	If you have no authorized license, discontinue using this Software immediately.

 *	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 *	IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 *	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 *	 CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 *	This License is effective until terminated. You may terminate this License at any time by destroying all copies
 *	of the Software including all documentation. This License will terminate immediately without notice from Humax
 *	to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 *	of the Software and all documentation.

 *	The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 *	All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 *	in the Republic of Korea.

 *
 **************************************************************************************
 **/

#include "corsair_tools.h"
#include "corsair.h"
#include "asteriks.h"
#include "corsair_config.h"
#include "corsair_product_env.h"

#include "cvideoresourcemanager.h"
#include "oplvideobroadcasthandle.h"
#include "opllocalsystem.h"
#include "oplavoutput.h"
#include "opllocalsystem.h"

#include "bplcookie.h"
#include "bplglobal.h"

#include "hapi.h"

#include "gogi_opera_api.h"
#include "NativeApplicationManager.h"

#include <map>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <uapi.h>
#include<string>
#include<algorithm>
#include<cctype>
#include<functional>


using namespace std;
static bool subtitle_enabled = false;

CorsairTools::CorsairTools()
{
}

CorsairTools::~CorsairTools()
{
}

unsigned long CorsairTools::getTicks()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
}

bool CorsairTools::startLiveTV()
{
    return CVideoResourceManager::startLiveTV();
}

bool CorsairTools::stopLiveTV()
{
	return CVideoResourceManager::stopLiveTV();
}

bool CorsairTools::recoverLiveTV()
{        
	return CVideoResourceManager::recoverLiveTVIfPossible();
}

bool CorsairTools::isPlayingMedia() 
{
    return CVideoResourceManager::isPlayingMedia();
}

bool CorsairTools::subtitleOn()
{
	bool enabled;

	enabled = OplHmxOutput::IsSubtitleEnabled(0);
	if ( enabled )
	{
		if( subtitle_enabled )
		{
			subtitle_enabled = false;
		    HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 1);
		}
		return true;
	}

	return false;
}

bool CorsairTools::subtitleOff()
{
	bool enabled;

	enabled = OplHmxOutput::IsSubtitleEnabled(0);
	if ( enabled )
	{
		subtitle_enabled = true;
		HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 0);
		return true;
	}

	return false;
}

bool CorsairTools::mhegOn()
{
	// start the MHEG application
	HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 1, "i", TRUE);
	return true;
}

bool CorsairTools::mhegOff()
{
	// stop the MHEG application: enum ITK_SERVICE_TUNE_STOP_HOST (1)
	HAPI_BroadcastSignal("signal:ChangeMhegStatusByOtherAppl", 2, "ii", FALSE, 1);
	return true;
}

bool CorsairTools::mhegBlock()
{
	HAPI_BroadcastSignal("signal:RequestMhegBlock", 1, "i", TRUE);
	return true;
}

bool CorsairTools::mhegUnBlock()
{
	HAPI_BroadcastSignal("signal:RequestMhegBlock", 1, "i", FALSE);
	return true;
}

bool CorsairTools::oipfAppBlock()
{
	HAPI_BroadcastSignal("signal:RequestOipfAppBlock", 1, "i", TRUE);
	return true;
}

bool CorsairTools::oipfAppUnBlock()
{
	HAPI_BroadcastSignal("signal:RequestOipfAppBlock", 1, "i", FALSE);
	return true;
}

bool CorsairTools::setPanelString(const char* string)
{
	OPL_HFrontPanel_SetStringAsTopPriority(string, 0, 100, false);

	return true;
}

bool CorsairTools::clearPanelString()
{
	OPL_HFrontPanel_SetStringAsTopPriority("", 0, 0, false);

	return true;
}

bool CorsairTools::setPreferenceLanguage()
{
	if ( getCorsair() == NULL ) return false;

	static std::map<const char *, const char *> language_map;
	language_map["eng"] = "en-US";
	language_map["ger"] = "de-DE";
	language_map["deu"] = "de-DE";
	language_map["tur"] = "tr-TR";
	language_map["ara"] = "ar-EG";
	language_map["per"] = "fa-IR";
	language_map["ces"] = "cs-CZ";
	language_map["rus"] = "ru-RU";

	language_map["fre"] = "fr";

	language_map["ita"] = "it";

	language_map["spa"] = "es";

	language_map["gre"] = "el";

	char lang[32] = {0,};
	OPL_LocalSystem_GetPreferredMenuLanuage(lang);

	map<const char *, const char *>::iterator it;
	for ( it=language_map.begin(); it!=language_map.end(); ++it )
	{
		if ( strncmp(it->first, lang, strlen(lang)) == 0 )
		{
			op_set_pref(getCorsair()->getOpera(), "Network", "HTTP Accept Language", it->second);
			return true;
		}
	}

	return false;
}

bool CorsairTools::getUrlfromUsb(char *buf, int bufsize)
{
#ifndef CONFIG_DEBUG
	if ( !CorsairProductEnv::getEnabledUrl() ) return false;
#endif
	char *pTempUrl = NULL;
	pTempUrl = (char *)UAPI_FM_GetDefaultURL(NULL, (HCHAR *)DEFAULT_USB_PORTAL_PATH);
	if(pTempUrl == NULL)
		return false;

	if(pTempUrl) HxSTD_StrNCpy(buf, pTempUrl, bufsize);
	buf[bufsize-1] = '\0';

	return true;
}

bool CorsairTools::getUrlfromUsb(const string path, string &url)
{
	char *tmp = (char *)UAPI_FM_GetDefaultURL(NULL, (HCHAR *)path.c_str());
    if ( tmp == NULL ) 
        return false;

    url = tmp;
    return true;
}

char* CorsairTools::getUsbPath(char *file)
{
    return UAPI_FM_GetFILEPath(file);
}


/*
 * Usage
 *
 * 1. In the USB, you have to make /opera folder
 * 2. And then you have to create /opera/dragonfly.txt file.
 * 3. you have to put your PC's ip address at first line.
 *    Port number in second line...
 * 4. This information only loaded in STB's booting time.
 * 5. Good Luck
 *
 */
bool CorsairTools::startDragonFlyWithNfsroot()
{
	int i = 0;
	char *p = NULL;
	char temp[32] = {0,};
	char szIP[32] = {0,};
	char szPort[32] = {0,};
	// NFSROOT. dragongly.txt. ..... dragon fly. ..... .. ....

	FILE *fp = fopen(DEFAULT_DRAGONFLY_NFSROOT_PATH, "r");
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

		return true;
	}

	return false;
}


bool CorsairTools::startDragonFlyWithUsb()
{
	int i = 0;
	char *p = NULL;
	char temp[32] = {0,};
	char szIP[32] = {0,};
	char szPort[32] = {0,};

    char *psFilePath = CorsairTools::getUsbPath((HCHAR *)DEFAULT_DRAGONFLY_USB_PATH);

    if (psFilePath == NULL)
    {
        return false;
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

		return true;
	}

	return false;
}

bool CorsairTools::startDragonFly(const char *ipAddr, unsigned short port)
{
	if ( op_dragonfly_enable() != GOGI_STATUS_OK
		|| op_dragonfly_connect(ipAddr, port) != GOGI_STATUS_OK )
	{
		return false;
	}

	return true;
}

bool CorsairTools::stopDragonFly()
{
	gogi_bool bconnected = 0;
	op_dragonfly_is_connected(&bconnected);
	if ( bconnected )
	{
		op_dragonfly_disable();
		return true;
	}

	return false;
}

void CorsairTools::syncCookie()
{
	unsigned int cookie_count;
	GogiCookie *gogi_cookie = NULL;

	if ( getCorsair() == NULL ) return;

	if ( op_get_cookie_list(getCorsair()->getOpera(), &gogi_cookie, &cookie_count) != GOGI_STATUS_OK )
		return;

	BplCookieManager::delAll();

	for ( int i=0; i<cookie_count; ++i )
	{
		BplCookie *cookie = new BplCookie();
		if ( cookie )
		{
			cookie->name = strdup(gogi_cookie[i].name);
			cookie->domain = strdup(gogi_cookie[i].domain);
			cookie->path = strdup(gogi_cookie[i].path);
			cookie->value = strdup(gogi_cookie[i].value);
			cookie->expires = gogi_cookie[i].expires;
			cookie->secure = gogi_cookie[i].secure;

			BplCookieManager::add(cookie);
		}
	}

	op_free_cookie_list(getCorsair()->getOpera());

	GogiOperaWindow* focused_window = getCorsair()->getFocusedWindow();
	if ( focused_window )
	{
		char *curr_url = NULL;
		op_get_loading_url(focused_window, &curr_url, false);
		BplCookieManager::setUrl(curr_url);
	}
	else
	{
		BplCookieManager::setUrl("about:blank");
	}
	
}

void CorsairTools::saveCookie()
{
	op_request_saving_cookies();
}

bool CorsairTools::changeDaeApp()
{
	const char *__dae_path = "/media/drive1/dae/index.html";
	if ( access(__dae_path, R_OK) != 0 ) return false;

	Asteriks *ast = static_cast<Asteriks *>(getCorsair());
	if ( !ast ) return false;

	ast->detachApplication();

	char dae_url[512] = {0,};
	snprintf(dae_url, sizeof(dae_url), "file://%s", __dae_path);

	return ast->attachApplication(dae_url);
}

bool CorsairTools::ReloadMainAppFromUSB()
{
    char *__dae_path = CorsairTools::getUsbPath((HCHAR *)USB_INSTALLED_APPLICATION_PATH);

    if (__dae_path == NULL)
    {
        return false;
    }

	Asteriks *ast = static_cast<Asteriks *>(getCorsair());
	if ( !ast ) return false;

	ast->detachApplication();

	char dae_url[512] = {0,};
	snprintf(dae_url, sizeof(dae_url), "file://%s", __dae_path);

	return ast->attachApplication(dae_url);
}

bool CorsairTools::doFormatWidgetInformation()
{
	int count;
	WidgetDescriptorHandle *handles = NULL;
	native_AppManGetWidgets(&count, &handles);

	for( int i= 0; i< count; i++)
	{
		if( native_AppManUninstallWidget(handles[i]) != 0)
		{
			LOG_DEBUG("doFormatWidgetInformation() failed at native_AppManUninstallWidget : %d \n",i);
			free(handles);
			return false;
		}
	}

	free(handles);
	return true;
}

const char * CorsairTools::getExternalMainAppURL()
{
    unsigned int status = 0;
    if(OplLocalSystem::getBoxLaunchAppMode(&status) == true)
    {
        if(status == 1)
        {
            Directory_t *dir = CorsairProductEnv::getDirectory();
            if(dir && (dir->external_app_url.length() > 0))
            {
                const char* url = dir->external_app_url.c_str();
                if( strstr(url, "file://") == url ) // file:///url/xxx/index.html
                {
                    if( HLIB_DIR_IsExist((const HCHAR *)(url + strlen("file://"))) == TRUE )
                    {
                        return url;
                    }
                }
                else if( (strstr(url, "http://") == url) || (strstr(url, "https://") == url ))    //http:// | https
                {
                    return url;
                }

            }
        }
    }
    LOG_DEBUG(" There is not external Main App\n");
    return NULL;
}

void CorsairTools::doAbort()
{
    LOG_ERR("@@@@@@@    Terminate Process using abort() to recover the system state !!  @@@@@@@ \n");
    abort();
}


/*  UrlParser class  */
void CorsairTools::UrlParser::parse(const string& url_s)
{
    const string prot_end("://");
    string::const_iterator prot_i = search(url_s.begin(), url_s.end(), \
                                           prot_end.begin(), prot_end.end());
    m_protocol.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i, \
              back_inserter(m_protocol), \
              ptr_fun<int,int>(tolower)); // protocol is icase
    if( prot_i == url_s.end() )
        return;

    advance(prot_i, prot_end.length());
    string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    m_host.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i, \
              back_inserter(m_host), \
              ptr_fun<int,int>(tolower)); // host is icase

    string::const_iterator query_i = find(path_i, url_s.end(), '?');
    m_path.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    m_query.assign(query_i, url_s.end());
}
