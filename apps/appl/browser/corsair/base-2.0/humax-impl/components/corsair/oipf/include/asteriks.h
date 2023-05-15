/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Daniel Blizniak
 *
 */

#ifndef ASTERIKS_H
#define ASTERIKS_H

#include "corsair.h"
#include "corsair_config.h"
#include "corsair_oipf.h"
#include "oipf_api.h"

class OipfSurface;

/** Main class for Opera and HbbTV/OIPF SDK integration.
 * 
 * Asteriks holds three surfaces;
 *	- one for HbbTV applications
 *	- one for Open Web
 *	- one for OIPF applications.
 * F9  can be used to toggle between two surfaces when running Corsair - HbbTV and OpenWeb.
 * F10 can be used to toggle between two surfaces when running Corsair - OIPF and OpenWeb.
*/
class Asteriks : public Corsair, public IAsteriks
{	
	public:
		Asteriks(int argc, char** argv);
		virtual ~Asteriks();
		
		bool init();

		bool attachApplication(const char *app_path);
		bool detachApplication();
		
#ifdef OIPF_WIDGETS
		/** Register an externally created gogi window to Asterix.
		 * This will allow Asterix to correctly focus and refresh this window according to OIPF window handling strategy.
		 *
		 * @param gogi_window, the gogi window to register
		 * @param hbbtv_window_id, OIPF window ID which is know to Asterix (was created by Asterix)
		 */
		bool registerGogiWindow(GogiOperaWindow* gogi_window, HBBTV_WindowIdentifier hbbtv_window_id);
#endif // OIPF_WIDGETS

		bool requestGogiWindow(GogiOperaWindow** gogi_window, gogi_bool& granted, HBBTV_WindowIdentifier hbbtv_window_id);
		void showGogiWindow(GogiOperaWindow* gogi_window);
		void resizeGogiWindow(GogiOperaWindow* gogi_window, GogiRect& rect);
		void releaseGogiWindow(GogiOperaWindow* gogi_window);
		void hideGogiWindow(GogiOperaWindow* gogi_window);
		
		// IAsteriks
		bool requestOipfWindow(HBBTV_WindowIdentifier *winId);
		bool releaseOipfWindow(HBBTV_WindowIdentifier winId);
		void resizeOipfWindow(GogiOperaWindow* gogi_window, GogiRect& rect);
		
		void showOipfSurface();
		void showHbbTVSurface();
		void showOpenWebSurface();
#ifdef ENABLE_OPERA_TVSTORE
		void showOTVSSurface();
#endif
		GogiOperaWindow* getFocusedWindow();

#ifdef OIPF_WIDGETS
		GogiScreenHandle getScreenHandle() const;
#endif // OIPF_WIDGETS
		OipfSurface *getOipfSurface() { return oipf_surface; }
		
		static HBBTV_STATUS oipf_notification_listener(OIPF_NotificationType notification, OIPF_NotificationData* data);
		
#ifdef OIPF_WIDGETS
		/** Handles all non GOGI notifications regarding OIPF widgets
		 * @param[in] notification the type of notification
		 * @param[in] data notification data
		 * @return HBBTV_OK on success, HBBTV_ERROR otherwise
		 */
		static HBBTV_STATUS oipf_widget_notification_listener(OIPF_NotificationType notification, OIPF_NotificationData* data);
#endif // OIPF_WIDGETS
	protected:
		bool initHSDK(GogiOpera* opera);
		void deinitHSDK();
		HBBTV_EventData* getUAString();
		
	private:
		static OIPF_ApplicationType oipf_application_create_listener(const char *url);
		void start_in_hbbtv_window(const char *url);

#ifdef OIPF_WIDGETS
		static HBBTV_STATUS handleOipfWidgetReinstallNotification(OIPF_NotificationData* data);
#endif // OIPF_WIDGETS

		friend IOipfSurface* opera_oipf_get_oipf_surface();
		OipfSurface*	oipf_surface;
		char*			oipf_app_to_load;
		
		/** OIPF window instances */
		std::vector<HBBTV_WindowIdentifier> oipf_windows_id;
};


#endif // ASTERIKS_H

