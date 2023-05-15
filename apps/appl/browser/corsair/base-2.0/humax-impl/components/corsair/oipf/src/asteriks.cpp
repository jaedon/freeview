
/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include "dsmcc_api.h"
#include "hbbtv_api.h"
#include "oipf_api.h"

#include <iostream>
#include <sstream>
#include <directfb.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#ifdef CORSAIR_SEGV_HANDLER_WITH_TRACEBACK
# include <execinfo.h>
#endif

#include "corsair.h"
#include "asteriks.h"
#include "corsair_gfx.h"
#include "corsair_input.h"
#include "corsair_utils.h"
#include "corsair_config.h"
#include "corsair_hbbtv_surface.h"
#ifdef ENABLE_OPERA_TVSTORE
#include "corsair_otvs_surface.h"
#include "corsair_otvs.h"
#endif
#include "corsair_hinput.h"
#include "corsair_tools.h"
#include "oipf_surface.h"
#include "corsair_web_surface.h"
#include "opera_action.h"
#include "oipf_action.h"
#include "corsair_oipf.h"

#include "gogi_opera_api.h"

#include "opllocalsystem.h"
#include "oplappmgr.h"
using namespace std;

extern Corsair* g_corsair;

#define OIPF_APP_TO_LOAD "--oipf="

Asteriks::Asteriks(int argc, char** argv) :
		Corsair(argc, argv) ,
		oipf_surface(NULL),
		oipf_app_to_load(NULL)
{
	char szMainAppUrl[512] = {0,};

    char *tempUsbUrl = CorsairTools::getUsbPath((HCHAR *)USB_INSTALLED_APPLICATION_PATH);

	if (NULL != tempUsbUrl)
	{
    	snprintf(szMainAppUrl, sizeof(szMainAppUrl), "file://%s", tempUsbUrl);
	}
#if 0 /* this is blocked temporary */
	else if ( access(USER_INSTALLED_APPLICATION_PATH, 0 ) == 0 )
	{
		szMainAppUrl = (char *)USER_INSTALLED_APPLICATION_URI;
	}
#endif
	else {
    	snprintf(szMainAppUrl, sizeof(szMainAppUrl), "%s", DEFAULT_APPLICATION_URI);
	}

	oipf_app_to_load = strdup(szMainAppUrl);
	LOG_DEBUG("We are going to load OIPF application from : %s\n",oipf_app_to_load);
}

Asteriks::~Asteriks()
{
	deinitHSDK();
	for (HBBTV_WindowIDVector::iterator iter = oipf_windows_id.begin(); iter != oipf_windows_id.end(); ++iter)
		opera_hbbtv_release_window(*iter);
	oipf_windows_id.clear();

	free(oipf_app_to_load);
}

bool Asteriks::init()
{
	if (!Corsair::init())
		return false;

	// init surface for oipf
	CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
	if (!corsair_gfx)
		return false;

	int screen_width = corsair_gfx->getScreenWidth();
	int screen_height = corsair_gfx->getScreenHeight();

	oipf_surface = corsair_gfx->createOipfSurface(0, 0, screen_width, screen_height);
	if (!oipf_surface)
		return false;

#ifdef USE_DIRECTFBGL
	corsair_gfx->initGogiScreen();
#endif

	CorsairInput *oipfInput = new CorsairOipfInput(eAWINDOW_OIPF);
	if ( !oipfInput )
		LOG_DEBUG("OipfInput instance creatiton is failed.\n");
	m_inputs.push_back(oipfInput);


	// init opera oipf to have one empty window at startup
	HBBTV_WindowIdentifier oipf_winid;
	if (!requestOipfWindow(&oipf_winid))
		return false;


    if( CorsairTools::getExternalMainAppURL() != NULL )
    {
        if(oipf_app_to_load)
            free(oipf_app_to_load);

        oipf_app_to_load = strdup(CorsairTools::getExternalMainAppURL());
        LOG_DEBUG("There is External Main app URL : %s\n",oipf_app_to_load);
    }

	if (oipf_app_to_load)
	{
		HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
		data->app.url = strdup(oipf_app_to_load);
		data->app.hbbtv_window = oipf_winid;
		opera_hbbtv_send_event(E_START_BROADCAST_INDEPENDENT_APP, data);
	}
	showOipfSurface();

	return true;
}

bool Asteriks::initHSDK(GogiOpera* opera)
{
	opera_oipf_init(opera);

	if (!opera_hbbtv_add_notification_listener(Asteriks::notification_listener))
		return false;

	if (!opera_oipf_add_notification_listener(Asteriks::oipf_notification_listener))
		return false;

	if (!opera_oipf_set_application_create_listener(Asteriks::oipf_application_create_listener))
		 return false;
#ifdef OIPF_WIDGETS
	if (!opera_oipf_add_notification_listener(Asteriks::oipf_widget_notification_listener))
		return false;
#endif // OIPF_WIDGETS

	return true;
}

void Asteriks::deinitHSDK()
{
	opera_oipf_shutdown();
}

bool Asteriks::attachApplication(const char *app_path)
{
	// init opera oipf to have one empty window at startup
	HBBTV_WindowIdentifier oipf_winid = opera_oipf_create_window();
	if (!oipf_winid)
		return false;

	oipf_windows_id.push_back(oipf_winid);

	HBBTV_EventData* data = (HBBTV_EventData*)malloc(sizeof(HBBTV_EventData));
	data->app.url = strdup(app_path);
	data->app.hbbtv_window = oipf_winid;
	opera_hbbtv_send_event(E_START_BROADCAST_INDEPENDENT_APP, data);
	showOipfSurface();

	return true;
}

bool Asteriks::detachApplication()
{
	HBBTV_WindowIDVector::iterator it;

	if ( oipf_windows_id.size() > 0 )
	{
		it = oipf_windows_id.begin();
		HBBTV_EventData* data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
		data->app.hbbtv_window = (*it);
		opera_hbbtv_send_event(E_TERMINATE_APP, data);
	}
	return true;
}

HBBTV_EventData* Asteriks::getUAString()
{
	HBBTV_EventData* ua_data = Corsair::getUAString();

	if (ua_data)
	{
		free(ua_data->ua_string.model_name);
		ua_data->ua_string.model_name = strdup("HbbTV/OIPF add-on");
	}

	return ua_data;
}

/**
 * Resize Gogi window. This is called by notification handler when HbbTV
 * framework has requested the oipf window (= oipf application) to
 * change its size.
 */
void Asteriks::resizeGogiWindow(GogiOperaWindow* gogi_window, GogiRect& rect)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	if (!gogi_window)
		// not oipf window, do nothing
		return;

	oipf_surface->resizeGogiWindow(gogi_window, rect);
}

bool Asteriks::requestOipfWindow(HBBTV_WindowIdentifier *winId)
{
	*winId = opera_oipf_create_window();
	if (!*winId)
		return false;

	oipf_windows_id.push_back(*winId);
	return true;
}

bool Asteriks::releaseOipfWindow(HBBTV_WindowIdentifier winId)
{
	HBBTV_WindowIDVector::iterator it;
	for (it = oipf_windows_id.begin(); it != oipf_windows_id.end(); ++it)
	{
		if ((*it) == winId)
			break;
	}

	if (it == oipf_windows_id.end())
	{
		return false;
	}

	oipf_windows_id.erase(it);

	opera_hbbtv_release_window(winId);

	return true;
}

void Asteriks::resizeOipfWindow(GogiOperaWindow *gogi_window, GogiRect& rect)
{
	g_corsair->getActionQueue()->add(new ResizeOipfWindowAction(gogi_window, this, rect));
}

void Asteriks::showOipfSurface()
{
	oipf_surface->showSurface();
	setFocusedWindow(oipf_surface->getWindow());
}

void Asteriks::showHbbTVSurface()
{
	Corsair::showHbbTVSurface();
}


void Asteriks::showOpenWebSurface()
{
	Corsair::showOpenWebSurface();
}

#ifdef ENABLE_OPERA_TVSTORE
void Asteriks::showOTVSSurface()
{
	oipf_surface->hideSurface();
	otvs_surface->showSurface();
	setFocusedWindow(otvs_surface->getFocusedWindow());
}
#endif

GogiOperaWindow* Asteriks::getFocusedWindow()
{
	if (oipf_surface && oipf_surface->isVisible() && oipf_surface->isOnScreen()
	/* The focus management is modified by Humax.
	 * Therefore, it needs to check whether the focused window is HbbTV surface or not.
	 */
		&& Corsair::getFocusedWindow() != hbbtv_surface->getWindow())
	{
		CorsairHbbTVSurface::HbbTVWindowData* winData = oipf_surface->getFocusedWindowData();
		return winData ? winData->gogi_window : NULL;
	}
	else
	{
		return Corsair::getFocusedWindow();
	}
}

#ifdef OIPF_WIDGETS
GogiScreenHandle Asteriks::getScreenHandle() const
{
	return oipf_surface->getScreenHandle();
}

bool Asteriks::registerGogiWindow(GogiOperaWindow* gogi_window, HBBTV_WindowIdentifier hbbtv_window_id)
{
	if (oipf_surface && opera_oipf_is_oipf_window(hbbtv_window_id))
	{
		oipf_surface->registerWindow(gogi_window);
		return true;
	}
	return false;
}
#endif // OIPF_WIDGETS

bool Asteriks::requestGogiWindow(GogiOperaWindow** gogi_window, gogi_bool& granted, HBBTV_WindowIdentifier hbbtv_window_id)
{
	if (opera_oipf_is_oipf_window(hbbtv_window_id))
	{
		if (!oipf_surface)
			return false;

		*gogi_window = oipf_surface->requestWindow();
		granted = *gogi_window != NULL;

		if (granted)
			LOG_DEBUG("Granting opera window to OIPF\n");
		else
			LOG_DEBUG("Opera window for OIPF not created !\n");
	}
	else
	{
		return Corsair::requestGogiWindow(gogi_window, granted, hbbtv_window_id);
	}

	return true;
}

void Asteriks::showGogiWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));
	if (!gogi_window)
		// not oipf window, do nothing
		return;

	if (opera_oipf_is_oipf_window(opera_hbbtv_get_window_identifier(gogi_window)))
	{
		if (!oipf_surface)
			return;

		oipf_surface->showWindow(gogi_window);

		if (oipf_surface->isVisible())
			setFocusedWindow(oipf_surface->getWindow());
	}
	else
	{
		Corsair::showGogiWindow(gogi_window);
	}
}

void Asteriks::releaseGogiWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	if (!gogi_window)
 		// not oipf/hbbtv window, do nothing
 		return;

	if (opera_oipf_is_oipf_window(opera_hbbtv_get_window_identifier(gogi_window)))
	{
		if (!oipf_surface)
			return;

		if (!oipf_surface->releaseWindow(gogi_window))
			return;

		if (oipf_surface->isVisible())
			setFocusedWindow(oipf_surface->getWindow());
	}
	else
	{
		Corsair::releaseGogiWindow(gogi_window);
	}
}

void Asteriks::hideGogiWindow(GogiOperaWindow* gogi_window)
{
	assert(opera_hbbtv_get_window_identifier(gogi_window));

	if (!gogi_window)
 		// not oipf/hbbtv window, do nothing
 		return;

	if (opera_oipf_is_oipf_window(opera_hbbtv_get_window_identifier(gogi_window)))
	{
		if (oipf_surface)
			oipf_surface->hideWindow(gogi_window);
	}
	else
	{
		Corsair::hideGogiWindow(gogi_window);
	}
}

/* static */
HBBTV_STATUS Asteriks::oipf_notification_listener(OIPF_NotificationType notification, OIPF_NotificationData* data)
{
	return HBBTV_OK;
}

#ifdef OIPF_WIDGETS

/* static */
HBBTV_STATUS Asteriks::handleOipfWidgetReinstallNotification(OIPF_NotificationData* data)
{
	// DAE 7.2.1.3 says we should query the user for the action, but here we want to perform the reinstall in a automatic fashion (always reinstall)
	data->oipf_widget_reinstall.callback(OipfWidgetReinstallChoice_YES,
	                                     data->oipf_widget_reinstall.installed_widget_handle,
	                                     data->oipf_widget_reinstall.newLocalFile,
	                                     data->oipf_widget_reinstall.newDownloadUrl);

	return HBBTV_OK;
}

/* static */
HBBTV_STATUS Asteriks::oipf_widget_notification_listener(OIPF_NotificationType notification, OIPF_NotificationData* data)
{
	switch (notification)
	{
		case N_CONFIRM_OIPF_WIDGET_REINSTALL:
			return handleOipfWidgetReinstallNotification(data);
	}

	return HBBTV_OK;
}

OIPF_ApplicationType Asteriks::oipf_application_create_listener(const char *url)
{
	bool http = strstr(url, "http://") == url || strstr(url, "https://") == url;
	bool file = strstr(url, "file://") == url;
	bool widget = strstr(url, "widget://") == url;
	// Overrides for testing purposes. They should be disabled in release version.
	bool hbbtv_override = strstr(url, "_hbbtv_") ||
						  strstr(url, "36a3130e828dc08ed0ef542e28839a66" ) ||
						  strstr(url, "9ef75ea99fd496dfd629ba5959032db7") ||
						  strstr(url, "prm-test");
	bool oipf_override = strstr(url, "_oipf_") ||
						 strstr(url, "humaxtvportal");
	bool otvs_override = strstr(url, "tvstore.opera.com") ||
						 strstr(url, "tvstore-test");
	bool otvb_override = strstr(url, "--tvbrowser--");

	bool oipf = oipf_override || (!hbbtv_override && (file || widget));
	bool hbbtv = hbbtv_override || (!oipf_override && http);
#ifdef ENABLE_OPERA_TVSTORE
	if (otvs_override )
	{
		HAPI_BroadcastSignal("signal:onHumaxApplicationCreate", 2, "ss", "otvs", url);
		//OplOIPFApplicationManager::setHumaxApplicationLoad(OPLHUMAXAPPLICATION_OTVS);
		return APPLICATION_TYPE_HBBTV;
	}
#endif
#ifdef ENABLE_OPERA_TVBROWSER
	if ( otvb_override )
	{
		HAPI_BroadcastSignal("signal:onHumaxApplicationCreate", 2, "ss", "otvb", "");
		//OplOIPFApplicationManager::setHumaxApplicationLoad(OPLHUMAXAPPLICATION_OTVB);
		return APPLICATION_TYPE_HBBTV;
	}
#endif
#ifdef ENABLE_OPERA_HBBTV
	if ( hbbtv_override )
	{
		HAPI_BroadcastSignal("signal:onHumaxApplicationCreate", 2, "ss", "hbbtv", (void*)url);
		//OplOIPFApplicationManager::setHumaxApplicationLoad(OPLHUMAXAPPLICATION_HBBTV);
		return APPLICATION_TYPE_HBBTV;
	}
#endif
	return APPLICATION_TYPE_OIPF;
}

#endif // OIPF_WIDGETS

IAsteriks* opera_oipf_get_asteriks()
{
	if (!g_corsair)
		assert(0);

	return static_cast<Asteriks*>(g_corsair);
}

IOipfSurface* opera_oipf_get_oipf_surface()
{
	if ( g_corsair == NULL )
	{
		assert(0);
		return NULL;
	}

	if ( (static_cast<Asteriks*>(g_corsair))->oipf_surface == NULL )
		return NULL;

	return (static_cast<Asteriks*>(g_corsair))->oipf_surface;
}
