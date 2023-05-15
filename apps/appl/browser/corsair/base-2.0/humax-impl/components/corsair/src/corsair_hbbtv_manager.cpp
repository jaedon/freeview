/** **********************************************************************************************************
	@file 		corsair_hbbtv_manager.cpp

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

#include "corsair_hbbtv_manager.h"
#include "corsair_hbbtv_action.h"
#include "corsair_config.h"
#include "corsair_hbbtv_surface.h"
#include "corsair_hama.h"
#include "corsair_ait_manager.h"
#include "corsair_tools.h"
#include "corsair.h"
#include "oipf_api.h"

#include "oplnetwork.h"
#include "oplbroadcast.h"

#include "hapi.h"

#include <cstdio>

using namespace std;



/**************************************************************************************
 * class CorsairHbbTVManager
 **************************************************************************************/
CorsairHbbTVManager::CorsairHbbTVManager()
	: m_teletext_state(TELETEXT_STATE_LIVE)
	, m_net_connected(false)
	, m_prm_status(false)
	, m_hbbtv_from_humax(false)
{
	pthread_mutex_init(&m_mtx, NULL);

	int netstat = 0;
	OPL_Network_GetLinkStatus(&netstat);
	action(new CorsairHbbTVNetworkUpdateAction(this));

	addBIApplication();
}

CorsairHbbTVManager::~CorsairHbbTVManager()
{
	vector<CorsairHbbTVApp *>::iterator it;

	lock();
	for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
	{
		delete (*it);
	}
	m_hbbtv_apps.clear();
	unlock();
	clearActions();

	pthread_mutex_destroy(&m_mtx);
}

void CorsairHbbTVManager::addHbbTVApplication(CorsairHbbTVApp *app)
{
	lock();
	m_hbbtv_apps.push_back(app);
	unlock();
	LOG_DEBUG("CorsairHbbTVManager::New HbbTV Applciation is added\n");
}

void CorsairHbbTVManager::removeHbbTVApplication(unsigned short appid, unsigned long orgid)
{
	lock();
	vector<CorsairHbbTVApp *>::iterator it;
	for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
	{
		if ( (*it)->getAppID() == appid && (*it)->getOrgID() == orgid )
		{
			delete (*it);
			m_hbbtv_apps.erase(it);
			break;
		}
	}
	unlock();
	LOG_DEBUG("CorsairHbbTVManager::HbbTV Applciation is removed: appid=0x%x\n",appid);
}

CorsairHbbTVApp *CorsairHbbTVManager::getApplication(CorsairHbbTVApp::eHbbTVApplicationType type)
{
	CorsairHbbTVApp *app = NULL;
	vector<CorsairHbbTVApp *>::iterator it;

	lock();
	for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
	{
		app = *it;
		if ( app->getAppType() == type )
		{
			pthread_mutex_unlock(&m_mtx);
			return app;
		}
	}

	unlock();
	return NULL;
}

CorsairHbbTVApp *CorsairHbbTVManager::getRunningApplication(CorsairHbbTVApp::eHbbTVApplicationType type)
{
	return CorsairHbbTVAction::getRunningApp(type);
}


bool CorsairHbbTVManager::isRunningApplication()
{
	return CorsairHbbTVAction::sizeofapps() > 0;
}

void CorsairHbbTVManager::action(CorsairHbbTVAction *action)
{
	delete action;
}

void CorsairHbbTVManager::resetTeletext()
{
	setTeletext(TELETEXT_STATE_LIVE);
}

/*
 * private
 */
void CorsairHbbTVManager::addBIApplication()
{
	char buf[512] = {0,};
	FILE *fp = fopen(DEFAULT_PORTAL_URL_PATH, "r");
	if ( fp )
	{
		fgets(buf, sizeof(buf), fp);
		fclose(fp);
	}

	string url = ( buf[0] != '\0' ) ? buf : DEFAULT_TV_PORTAL_URI;

	CorsairHbbTVApp *app = new CorsairHbbTVBIApp();
	if ( app )
	{
		app->setUrl(url.c_str());
	}

	addHbbTVApplication(app);
}

void CorsairHbbTVManager::setTeletext(eTeletextState state)
{
	lock();
	m_teletext_state = state;
	unlock();
}

bool CorsairHbbTVManager::startDigitalTeletext()
{
	CorsairHbbTVApp *app = getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_TELETEXT);
	if ( app )
	{
		app->run();
		setTeletext(TELETEXT_STATE_DIGITAL);
		return true;
	}
	return false;
}

bool CorsairHbbTVManager::startStandardTeletext()
{
	OPL_Channel_t currentChannel = NULL;
	if( OPL_Broadcast_HasSDTeletext() == BPL_STATUS_OK )
    {
		HAPI_BroadcastSignal("signal:onTeletextState", 1, "i", 1);
		setTeletext(TELETEXT_STATE_STANDARD);
		return true;
    }
	return false;
}

void CorsairHbbTVManager::comebackLive()
{
	CorsairHbbTVApp *app = getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);
	if ( app )
	{
		app->run();
	}
	setTeletext(TELETEXT_STATE_LIVE);
}

void CorsairHbbTVManager::runTeletext()
{
    /*it's seperated to help easy understanding, not for code effective.
     this scenario is for playing any medias, vods, TSR, Chase play */
    if ( CorsairTools::isPlayingMedia() )
    {
        switch ( m_teletext_state )
        {
            case TELETEXT_STATE_LIVE:
                /* even launching DVB ttx is failed, we don't need to do anything additionally */
    		    startStandardTeletext();
    		    break;
    		    
    		case TELETEXT_STATE_STANDARD:
                setTeletext(TELETEXT_STATE_LIVE);    		    
    		    break;
    		    
    		default:
    		    break;
        }
    }
    else
    {
    	switch ( m_teletext_state )
    	{
    		case TELETEXT_STATE_LIVE:
    		{
    			if ( !startDigitalTeletext() )
    			{
                    CorsairHbbTVApp *app = getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);
                	if ( app )
                	{
                		app->stop();
                    }
                    startStandardTeletext();
                }
                break;
    		}
    		case TELETEXT_STATE_DIGITAL:
    		{
    			CorsairHbbTVApp *app = getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_TELETEXT);
    			if ( app )
    			{
    				app->stop();
    			}
    
    			if ( !startStandardTeletext() )
    				comebackLive();
                break;
    		}
    		case TELETEXT_STATE_STANDARD:
    		{
    			comebackLive();
    			break;
    		}
    		default:
    			break;
    	}
    }
}

void CorsairHbbTVManager::clearApps()
{
	vector<CorsairHbbTVApp *>::iterator it;

	lock();
	for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
	{
		delete (*it);
	}

	m_hbbtv_apps.clear();
	unlock();
}

void CorsairHbbTVManager::showApps()
{
	vector<CorsairHbbTVApp *>::iterator it;

	lock();
	for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
	{
		LOG_DEBUG("-------------------------------------------------------------->\n");
		LOG_DEBUG("URL: %s\n",(*it)->getUrl().c_str());
		LOG_DEBUG("APP TYPE: %d\n",(*it)->getAppType());
		LOG_DEBUG("APP ID: 0x%x\n",(*it)->getAppID());
		LOG_DEBUG("ORG ID: 0x%x\n",(*it)->getOrgID());
		LOG_DEBUG("<--------------------------------------------------------------\n");
	}
	unlock();
	LOG_DEBUG("DEBUG: Show HbbTV Applications[size=%d]\n",m_hbbtv_apps.size());
}

void CorsairHbbTVManager::setHbbTVApplicationFromHumax(bool load)
{
    m_hbbtv_from_humax = load;

    if ( m_hbbtv_from_humax )
    {
        HAPI_BroadcastSignal("signal:onHumaxApplicationLoaded", 1, "s", "hbbtv");
    }
    else
    {
	    HAPI_BroadcastSignal("signal:onHumaxApplicationUnloaded", 1, "s", "hbbtv");
    }
}

bool CorsairHbbTVManager::getHbbTVApplicationFromHumax()
{
    return m_hbbtv_from_humax;
}

void CorsairHbbTVManager::setNagraPRMStatus(bool enable)
{
	m_prm_status = enable;
}
bool CorsairHbbTVManager::getNagraPRMStatus(void)
{
	return m_prm_status;
}

/*  This function will be implemented only for hbbtv&mheg coexistence tests. (FVP-COEX-0200 ~ 0270)  */
/*  If there is the absence AIT of this channel, the application shall be treuted to BR. */
/*  Ref - DBOOK K.2.1.2 / BBC Interactive HTML application specification 2.3.3 */
bool CorsairHbbTVManager::isBroadcastRelatedApplicationFromXMLAIT(HBBTV_AITUpdateNotification *data)
{
    bool ret = false;
    string url = "";
    vector<CorsairHbbTVApp *>::iterator it;

    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    if ( NULL == aitManager->getCurrentAITSection()) 
        return true;

    if ( data->app_count != 1) return false;
    url = data->applications[0].urls[0];
    
    CorsairTools::UrlParser urlInXmlAit(url);
    string xmlait_url = string(urlInXmlAit.getProtocol() + "://" + urlInXmlAit.getHost());

    lock();
    for ( it=m_hbbtv_apps.begin(); it!=m_hbbtv_apps.end(); ++it )
    {
        if( (*it)->getAppType() != CorsairHbbTVApp::HBBTV_APPTYPE_BR_PRESENT) 
            continue;

        CorsairTools::UrlParser urlInAit((*it)->getUrl());
        string ait_url = string(urlInAit.getProtocol() + "://" + urlInAit.getHost());

        if( (data->applications[0].appId == (*it)->getAppID())  &&
                (data->applications[0].orgId == (*it)->getOrgID())
                 && (0 == xmlait_url.compare(ait_url)) )
        {
            ret = true;
        }
    }

	unlock();

    return ret;
}

	/* action queue */
bool CorsairHbbTVManager::addAction(CorsairHbbTVAction *action)
{
    lock();       
    m_hbbtv_actions.push_back(action);
    unlock();
    return true;
}

bool CorsairHbbTVManager::executeAllActions(void)
{
    lock();
    
    if ( 0 < m_hbbtv_actions.size() )
    {
        for ( std::vector<CorsairHbbTVAction *>::iterator it = m_hbbtv_actions.begin(); m_hbbtv_actions.end() != it; it++ )
        {
            (*it)->execute();
            delete (*it);
        }
        m_hbbtv_actions.clear();
    }
    
    unlock();
    return true;
}

bool CorsairHbbTVManager::clearActions(void)
{
    lock();
    
    if ( 0 < m_hbbtv_actions.size() )
    {
        for ( std::vector<CorsairHbbTVAction *>::iterator it = m_hbbtv_actions.begin(); m_hbbtv_actions.end() != it; it++ )
        {
            delete (*it);
        }
        m_hbbtv_actions.clear();
    }
    
    unlock();
    return true;
}    
