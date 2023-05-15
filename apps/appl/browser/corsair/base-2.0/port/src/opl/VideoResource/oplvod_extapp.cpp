/*************************************************************************************************************
	File 		: oplvod_extapp.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/11/15
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/

#include "oplvod_extapp.h"
#include "opllocalsystem.h"
#include "bplcookie.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

#include <hlib.h>

using namespace std;

/*******************************************************************************
 * defines
 *******************************************************************************/
#define MAX_COOKIE_SIZE		(1024 * 4)
#define MAX_URI_SIZE 	  	MAX_COOKIE_SIZE + 2048

#define YOUTUBE_DOMAIN			"youtube.com"
#define YOUTUBE_COOKIE_PREFIX 	"/HMXCOOKIE="

#define MAXDOME_DOMAIN 			"maxdome.de"
#define MAXDOME_COOKIE_NAME 	"MAN"

#define MUSICBAEBLE_DOMAIN		"obigowebapp/baeble"
#define ITALIANFOOD_DOMAIN		"obigowebapp/italianfood"
#define ABSOLUTERADIO_DOMAIN	"obigowebapps/absoluteradio"

/*******************************************************************************
 * interfaces
 *******************************************************************************/
bool OplVodExtApp::stayYoutubeApp()
{
	return stayThisApp(YOUTUBE_DOMAIN);
}

bool OplVodExtApp::getYoutubeCookie(std::string &cookie)
{
	if ( !BplCookieManager::getCookieByName(YOUTUBE_DOMAIN, cookie) ) return false;

    cookie.append("/");

	return true;
}

bool OplVodExtApp::stayMaxdomeApp()
{
	return stayThisApp(MAXDOME_DOMAIN);
}

bool OplVodExtApp::getMaxdomeCookie(std::string &cookie)
{
//    bool result = false;
	if ( !BplCookieManager::getCookieByName(MAXDOME_COOKIE_NAME, cookie) ) return false;

    // remove the prefix "MAX=" string
    cookie = cookie.substr(4);
	return true;
}

bool OplVodExtApp::stayMusicBaebleApp()
{
	return stayThisApp(MUSICBAEBLE_DOMAIN);
}

bool OplVodExtApp::stayItalianFoodApp()
{
	return stayThisApp(ITALIANFOOD_DOMAIN);
}

bool OplVodExtApp::stayAbsoluteRadioApp()
{
	return stayThisApp(ABSOLUTERADIO_DOMAIN);
}

bool OplVodExtApp::getTVAppsCookie(std::string &cookie)
{
    bool result = false;

    unsigned int urlLen = 2048;
    char *url = (char *)malloc(sizeof(char)*urlLen);

    if ( NULL == url )
        return result;

    char *domain = NULL;
    OPL_LocalSystem_GetPortalURL(url, urlLen);
    domain = strtok(url, "//");
    domain = strtok(NULL, "//");

    if ( BplCookieManager::getCookieByDomain(domain, cookie) )
    {
        cookie.append("/");
        result = true;
    }

    free(url);

    return result;
}

/*******************************************************************************
 * static functions
 *******************************************************************************/
bool OplVodExtApp::stayThisApp(const char *keyword)
{
	string url;
	url = BplCookieManager::getUrl();

	return ( strstr(url.c_str(), keyword) ) ?  true : false;
}
