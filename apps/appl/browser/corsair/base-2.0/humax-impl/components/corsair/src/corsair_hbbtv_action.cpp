/** **********************************************************************************************************
	@file 		corsair_hbbtv_action.cpp

	@date		2014/12/04
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

#include "corsair_hbbtv_action.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_dsmcc_manager.h"
#include "corsair_ait_manager.h"
#include "corsair_tools.h"
#include "corsair_log.h"
#include "oipf_api.h"

#include "oplvod.h"
#include "opllocalsystem.h"
#include "oplappmgr.h"
#include "bplglobal.h"

#include <cstdio>
#include <cstring>

using namespace std;



/**************************************************************************************
 * class CorsairHbbTVAction
 **************************************************************************************/
vector<CorsairHbbTVApp *> CorsairHbbTVAction::s_hbbtv_active_apps;
bool CorsairHbbTVAction::s_is_started_mheg = false;
CorsairHbbTVAction::CorsairHbbTVAction()
{
}

CorsairHbbTVAction::~CorsairHbbTVAction()
{
}

void CorsairHbbTVAction::showActiveApps()
{
	LOG_DEBUG("DEBUG: Show Active HbbTV Applications[size=%d] \n",s_hbbtv_active_apps.size());
	vector<CorsairHbbTVApp *>::iterator it;
	for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
	{
		LOG_DEBUG("URL: %s\n",(*it)->getUrl().c_str());
		LOG_DEBUG("APP TYPE: %d\n",(*it)->getAppType());
		LOG_DEBUG("APP ID: 0x%x\n",(*it)->getAppID());
		LOG_DEBUG("ORG ID: 0x%x\n",(*it)->getOrgID());
	}
}

CorsairHbbTVApp *CorsairHbbTVAction::getRunningApp(CorsairHbbTVApp::eHbbTVApplicationType type)
{
	LOG_DEBUG("DEBUG: Show Active HbbTV Applications[size=%d]\n",s_hbbtv_active_apps.size());
	CorsairHbbTVApp *running_app = NULL;
	vector<CorsairHbbTVApp *>::iterator it;
	for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
	{
		running_app = (*it);
		LOG_DEBUG("DEBUG: Show Active HbbTV Applications[running url=%s]\n",running_app->getUrl().c_str());
		if( running_app->getAppType() == type )
			return running_app;
	}

	return NULL;
}


/*
 * class CorsairHbbtvStartAction
 */
CorsairHbbTVStartAction::CorsairHbbTVStartAction(CorsairHbbTVApp::eHbbTVApplicationType type, CorsairHbbTVManager *hm)
{
	CorsairHbbTVApp *app = NULL;
    app = hm->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);

    if ( NULL == app )
    {
    	app = hm->getApplication(type);
    	if ( app )
    		app->run();
    }
    else
    {
		LOG_DEBUG("CorsairHbbTVStartAction BR App is running.\n");
    }
}

CorsairHbbTVStartAction::~CorsairHbbTVStartAction()
{
}

/*
 * class CorsairHbbTVStopAction
 */
CorsairHbbTVStopAction::CorsairHbbTVStopAction(CorsairHbbTVApp::eHbbTVApplicationType type, CorsairHbbTVManager *hm)
{
	CorsairHbbTVApp *app = NULL;

	if( type == CorsairHbbTVApp::HBBTV_APPTYPE_BI )
	{
		app = hm->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
	}
	else
	{
		app = hm->getRunningApplication(type);
	}

	if ( app )
	{
		hm->resetTeletext();
		app->stop();
	}
}

CorsairHbbTVStopAction::~CorsairHbbTVStopAction()
{
}

/*
 * class CorsairHbbTVStartedAction
 */
CorsairHbbTVStartedAction::CorsairHbbTVStartedAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm)
{
    if( data )
    {
        if ( !opera_oipf_is_oipf_window(data->hbbtv_window) )
        {
            LOG_DEBUG("CorsairHbbTVStartedAction (data->broadcastRelated)=%d\n",data->broadcastRelated);
#if defined(DSMCC_MANAGING_WITH_APPID)
            CorsairDsmccManager::getInstance()->startApplication(data->orgId, data->appId);
#endif
            CorsairHbbTVApp *app = (data->broadcastRelated) ? \
                                   dynamic_cast<CorsairHbbTVApp *>(new CorsairHbbTVBRApp(data->appId, data->orgId, data->url)) : \
                                   dynamic_cast<CorsairHbbTVApp *>(new CorsairHbbTVBIApp(data->url));
            if ( app )
            {
#if defined(CONFIG_PROD_FVP4000T)
                if( s_is_started_mheg )
                {
                    app->m_is_started_mheg = true;
                    s_is_started_mheg = false;

                    CorsairTools::mhegOff();
                    CorsairTools::mhegBlock();
                }
#endif

                s_hbbtv_active_apps.push_back(app);

                if(data->broadcastRelated)
                {
                    app->setAppType(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);
                }

                if( app->getAppType() == CorsairHbbTVApp::HBBTV_APPTYPE_BI )
                {
                    CorsairHbbTVApp *running_bi_app = NULL;
                    running_bi_app = hm->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
#if defined(CONFIG_PROD_FVP4000T)
                    if( !((running_bi_app != NULL) && (running_bi_app->m_is_started_mheg)) )
#else
                        if( !(running_bi_app != NULL) )
#endif
                        {
                            CorsairTools::setPreferenceLanguage();
							CorsairTools::stopLiveTV();
                        }
                }

                if( s_hbbtv_active_apps.size() )
                    CorsairTools::oipfAppBlock();
            }

            if( s_hbbtv_active_apps.size() > 0 )
            {
				BplGlobalConfig::setIsRunningHbbTVApp(true);
            }
			

			
			/* hbbtv security check in humax side */
			if( data->broadcastRelated )
			{
				BplGlobalConfig::setStartedHbbTVAppType(BplGlobalConfig::HBBTV_APPLICATION_BR);
			}
			else
			{
				BplGlobalConfig::setStartedHbbTVAppType(BplGlobalConfig::HBBTV_APPLICATION_BI);
			}
        }
    }
}

CorsairHbbTVStartedAction::~CorsairHbbTVStartedAction()
{
}

/*
 * class CorsairHbbTVStoppedAction
 */
CorsairHbbTVStoppedAction::CorsairHbbTVStoppedAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm)
{
    if (data)
    {
    	if ( !opera_oipf_is_oipf_window(data->hbbtv_window) )
    	{
    		LOG_DEBUG("CorsairHbbTVStoppedAction (data->broadcastRelated)=%d\n",data->broadcastRelated);

#if defined(DSMCC_MANAGING_WITH_APPID)
    		CorsairDsmccManager::getInstance()->stopApplication(data->orgId, data->appId);
#else
    		CorsairDsmccManager::getInstance()->stopApplication(data->url);
#endif

    		vector<CorsairHbbTVApp *>::iterator it;
    		for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
    		{
    			/*
    			if ( data->broadcastRelated )
    			{
    				if ( (*it)->getAppID() == data->appId && (*it)->getOrgID() == data->orgId
    						&& strcmp((*it)->getUrl().c_str(), data->url) == 0 )
    				{
    					delete (*it);
    					s_hbbtv_active_apps.erase(it);
    					break;
    				}
    			}
    			else
    			{
    				if ( strcmp((*it)->getUrl().c_str(), data->url) == 0 )
    				{
    					delete (*it);
    					s_hbbtv_active_apps.erase(it);
    					break;
    				}
    			}

    			*/
    			if ( strcmp((*it)->getUrl().c_str(), data->url) == 0 )
    			{
    				delete (*it);
    				s_hbbtv_active_apps.erase(it);
    				break;
    			}
    	    }

    		if ( data->broadcastRelated && s_hbbtv_active_apps.size() == 0 && BplGlobalConfig::getReactivateAit() )
    		{
            	LOG_DEBUG("-- reactivate_ait = true!! \n");
                *(data->reactivate_ait) = BplGlobalConfig::getReactivateAit();
    		}
    		else
    		{
				LOG_DEBUG("-- reactivate_ait = false!! \n");
				*(data->reactivate_ait) = false;
    		}

			BplGlobalConfig::setReactivateAit(false);

    			// Workaround: CRB TEST PACK #4
    		if ( s_hbbtv_active_apps.size() == 0 )
    		{
                if ( hm->getHbbTVApplicationFromHumax() )
                {
                    hm->setHbbTVApplicationFromHumax(false);
                }
                CorsairTools::startLiveTV();
                CorsairTools::subtitleOn();
#if defined(CONFIG_PROD_FVP4000T)
                CorsairTools::mhegUnBlock();
                CorsairTools::mhegOn();
#endif                
            	CorsairTools::oipfAppUnBlock();
            	BplGlobalConfig::setIsRunningHbbTVApp(false);
            }

			/* hbbtv security check in humax side */
			BplGlobalConfig::setStoppedApp();

    	}
    }
}

CorsairHbbTVStoppedAction::~CorsairHbbTVStoppedAction()
{
}

/*
 * class CorsairHbbTVUpdateAction
 */
CorsairHbbTVUpdateAction::CorsairHbbTVUpdateAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm)
{
	if ( !opera_oipf_is_oipf_window(data->hbbtv_window) )
	{
		vector<CorsairHbbTVApp *>::iterator it;
		for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
		{
			if ( (*it)->getAppID() == data->appId && (*it)->getOrgID() == data->orgId
					&& strcmp((*it)->getUrl().c_str(), data->url) == 0 )
			{
				(*it)->setBroadcastRelated(data->broadcastRelated);
				if( data->broadcastRelated ) 
					(*it)->setAppType(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART);
				else
					(*it)->setAppType(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
				break;
			}
		}

		/* hbbtv security check in humax side */
		if( data->broadcastRelated )
		{
			BplGlobalConfig::setChangedHbbTVAppType(BplGlobalConfig::HBBTV_APPLICATION_BR);
		}
		else
		{
			BplGlobalConfig::setChangedHbbTVAppType(BplGlobalConfig::HBBTV_APPLICATION_BI);
		}

	}
}

CorsairHbbTVUpdateAction::~CorsairHbbTVUpdateAction()
{
}

/*
 * class CorsairHbbTVNewAITAction
 */
CorsairHbbTVNewAITAction::CorsairHbbTVNewAITAction(HBBTV_AITUpdateNotification *data, CorsairHbbTVManager *hm)
{
	for (int i = 0; i < data->app_count; i++)
	{
		int y = data->applications[i].url_count;
		if ( y > 0 )
		{
			while ( y-- > 0 )
			{
				if ( data->applications[i].urls[y] == NULL ) continue;
#if defined(CONFIG_PROD_FVP4000T)
				if( data->xml_ait )
				{
					CorsairHbbTVApp *app = new CorsairHbbTVBRXmlAIT(
							data->applications[i].appId,
							data->applications[i].orgId,
							data->applications[i].urls[y]);
					if ( app )
					{
						hm->addHbbTVApplication(app);
					}
				}
                else if ( data->applications[i].control_code == PRESENT )
                {
					CorsairHbbTVApp *app = new CorsairHbbTVBRPresentApp(
							data->applications[i].appId,
							data->applications[i].orgId,
							data->applications[i].urls[y]);
					if ( app )
					{
						hm->addHbbTVApplication(app);
					}
                }
				else if ( data->applications[i].control_code == AUTOSTART )
#else
				if ( data->applications[i].control_code == AUTOSTART )
#endif
				{
					CorsairHbbTVApp *app = new CorsairHbbTVBRAutoApp(
							data->applications[i].appId,
							data->applications[i].orgId,
							data->applications[i].urls[y]);
					if ( app )
					{
						hm->addHbbTVApplication(app);
					}

				}
				else if ( data->applications[i].usage_type == USAGE_DIGITAL_TELETEXT_APP )
				{
					CorsairHbbTVApp *app = new CorsairHbbTVBRTeletextApp(
							data->applications[i].appId,
							data->applications[i].orgId,
							data->applications[i].urls[y]);
					if ( app )
					{
						hm->addHbbTVApplication(app);
					}

				}
			}
		}
	}
}

CorsairHbbTVNewAITAction::~CorsairHbbTVNewAITAction()
{
}

/*
 * class CorsairHbbTVShowAction
 */
CorsairHbbTVShowAction::CorsairHbbTVShowAction()
{
}

CorsairHbbTVShowAction::CorsairHbbTVShowAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm)
{
}

CorsairHbbTVShowAction::~CorsairHbbTVShowAction()
{
}

/*
 * class CorsairHbbTVHideAction
 */
CorsairHbbTVHideAction::CorsairHbbTVHideAction()
{
}

CorsairHbbTVHideAction::CorsairHbbTVHideAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm)
{
}

CorsairHbbTVHideAction::~CorsairHbbTVHideAction()
{
}

void CorsairHbbTVHideAction::execute()
{
    CorsairTools::startLiveTV();
}

/*
 * class CorsairHbbTVVodStopAction
	*/
CorsairHbbTVVodStopAction::CorsairHbbTVVodStopAction()
{
}

CorsairHbbTVVodStopAction::~CorsairHbbTVVodStopAction()
{
}

void CorsairHbbTVVodStopAction::execute()
{
    CorsairTools::recoverLiveTV();
}

/*
 * class CorsairHbbTVExitAction
 */
CorsairHbbTVExitAction::CorsairHbbTVExitAction(CorsairHbbTVManager *hm)
{
    showActiveApps();
    if ( s_hbbtv_active_apps.size() > 0 )
    {
    	CorsairHbbTVApp *app = s_hbbtv_active_apps.back();

        if ( app->isBroadcastRelated() )
        {
            LOG_DEBUG("CorsairHbbTVInput:: Broadcast-Related application will be killed.\n");
        }
        else
        {
            LOG_DEBUG("CorsairHbbTVInput:: Broadcast-Independent application will be killed.\n");
        }
        
    	hm->resetTeletext();
    	app->stop();
    }
}

CorsairHbbTVExitAction::~CorsairHbbTVExitAction()
{
}

/*
 * class CorsairHbbTVBRExitAction
 */
CorsairHbbTVBRExitAction::CorsairHbbTVBRExitAction()
{
	showActiveApps();
	if ( s_hbbtv_active_apps.size() > 0 )
	{
		CorsairHbbTVApp *app = s_hbbtv_active_apps.back();

		if( app )
		{
			if( app->isBroadcastRelated() == 1 )
				app->stop();

		}
	}
}

CorsairHbbTVBRExitAction::~CorsairHbbTVBRExitAction()
{
}

/*
 * class CorsairHbbTVBIExitAction
 */
CorsairHbbTVBIExitAction::CorsairHbbTVBIExitAction()
{
	showActiveApps();
	if ( s_hbbtv_active_apps.size() > 0 )
	{
		CorsairHbbTVApp *app = s_hbbtv_active_apps.back();

		if( app )
		{
			if( app->isBroadcastRelated() == 0 )
				app->stop();

		}
	}
}

CorsairHbbTVBIExitAction::~CorsairHbbTVBIExitAction()
{
}


/*
 * class CorsairHbbTVChannelChangedAction
 */
CorsairHbbTVChannelChangedAction::CorsairHbbTVChannelChangedAction(CorsairHbbTVManager *hm)
{
	hm->clearApps();
	/* default HbbTV independent applicaiton has to be added */
	hm->addBIApplication();
	hm->resetTeletext();
}

CorsairHbbTVChannelChangedAction::~CorsairHbbTVChannelChangedAction()
{
}

/*
 * class CorsairHbbTVClearAppListAction
 */
CorsairHbbTVClearAppListAction::CorsairHbbTVClearAppListAction(CorsairHbbTVManager *hm)
{
	hm->clearApps();
}

CorsairHbbTVClearAppListAction::~CorsairHbbTVClearAppListAction()
{
}

/*
 * class CorsairHbbTVNetworkUpdateAction
 */
CorsairHbbTVNetworkUpdateAction::CorsairHbbTVNetworkUpdateAction(CorsairHbbTVManager *hm)
{
    int connected = 0;
    
    OPL_Network_GetInternetConnection(&connected);
    ( connected ) ? opera_hbbtv_send_event(E_CONTROL_BROADBAND_OPERATIONAL, NULL)
    	: opera_hbbtv_send_event(E_CONTROL_BROADBAND_NOT_OPERATIONAL, NULL);
    hm->m_net_connected = connected;
}

CorsairHbbTVNetworkUpdateAction::~CorsairHbbTVNetworkUpdateAction()
{
}

/*
 * class CorsairHbbTVTeletextAction
 */
CorsairHbbTVTeletextAction::CorsairHbbTVTeletextAction(CorsairHbbTVManager *hm)
{
	hm->runTeletext();
}

CorsairHbbTVTeletextAction::~CorsairHbbTVTeletextAction()
{
}

/*
 * class CorsairHbbTVErrorAction
 */
CorsairHbbTVErrorAction::CorsairHbbTVErrorAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm)
{
	/* Nothing: it requires UX scenario */
}

CorsairHbbTVErrorAction::~CorsairHbbTVErrorAction()
{
}

/*
 * class CorsairHbbTVBIAppStartAction
 */
CorsairHbbTVBIAppStartAction::CorsairHbbTVBIAppStartAction(const char *url, CorsairHbbTVManager *hm)
	: m_flag(HBBTV_BIAPP_NONE)
{
	CorsairHbbTVBIApp *app = new CorsairHbbTVBIApp(url);
	if ( app )
	{
        hm->setHbbTVApplicationFromHumax(true);
		app->run();
        delete app;
	}
}

CorsairHbbTVBIAppStartAction::CorsairHbbTVBIAppStartAction(const char *url, CorsairHbbTVManager *hm, eHbbTVBIAppFlag flag)
	: m_flag(flag)
{
	CorsairHbbTVBIApp *app = new CorsairHbbTVBIApp(url);

	if ( app )
	{
		if( m_flag == HBBTV_BIAPP_START_NOTOFFLIVE )
			app->run_without_liveoff();
#if defined(CONFIG_PROD_FVP4000T)
		s_is_started_mheg = true;
#endif

		delete app;
	}
}

CorsairHbbTVBIAppStartAction::~CorsairHbbTVBIAppStartAction()
{
}

/*
 * class CorsairHbbTVDoNotReactivateAction
 */
CorsairHbbTVDoNotReactivateAction::CorsairHbbTVDoNotReactivateAction()
{
	vector<CorsairHbbTVApp *>::iterator it;
	for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
	{
		if ( (*it)->isBroadcastRelated() )
		{
			BplGlobalConfig::setReactivateAit(false);
			break;
		}
	}
}

CorsairHbbTVDoNotReactivateAction::~CorsairHbbTVDoNotReactivateAction()
{
}

/*
 * class CorsairHbbTVDoReactivateAction
 */
CorsairHbbTVDoReactivateAction::CorsairHbbTVDoReactivateAction(CorsairHbbTVManager *hm)
{
    if ( !hm ) return;

	vector<CorsairHbbTVApp *>::iterator it;
	for ( it=s_hbbtv_active_apps.begin(); it!=s_hbbtv_active_apps.end(); ++it )
	{
		if ( (*it)->isBroadcastRelated() )
		{
			BplGlobalConfig::setReactivateAit(true);
			break;
		}
	}
}

CorsairHbbTVDoReactivateAction::~CorsairHbbTVDoReactivateAction()
{
}

