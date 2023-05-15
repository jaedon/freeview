/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Erik Lindström
 *
 */

#ifndef CORSAIR_H
#define CORSAIR_H

#include <string>
#include <signal.h>
#include <vector>

#include "sys/time.h"
#include "hbbtv_api.h"
#include "gogi_opera_api.h"

#ifdef USE_DIRECTFBGL
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <directfb.h>
#endif

#include "corsair_config.h"
#include "corsair_log.h"

class OperaActionQueue;
class CorsairHbbTVSurface;
class CorsairWebSurface;
#ifdef ENABLE_OPERA_TVSTORE
class CorsairOTVSSurface;
class CorsairOTVS;
#endif
class OipfSurface;
class CorsairCertManager;
class CorsairHbbTVManager;
class CorsairInput;
class CorsairIme;
class CorsairPostman;
class CorsairSecurityManager;
#ifdef ENABLE_FPS_DISPLAYER
class CorsairFPSDisplayer;
#endif

/** Main class for Opera and HbbTV SDK integration.

	Corsair holds two surfaces; one for HbbTV applications and one for
	Open Web. F9 and F10 can be used to toggel between these two
	surfaces when running Corsair.
*/
class Corsair
{
	protected:
		typedef std::vector<HBBTV_WindowIdentifier> HBBTV_WindowIDVector;

	public:
		Corsair(int argc, char** argv);
		virtual ~Corsair();

		virtual bool init();
		void run();
		bool isRunning() { return do_run; }
		GogiOpera* getOpera() { return opera; }
		OperaActionQueue* getActionQueue() { return action_queue; }
		HBBTV_WindowIdentifier getHbbTVWidnowId() { return hbbtv_window_id; }
		CorsairSecurityManager *getSecurityManager() { return m_security_manager; }
		CorsairHbbTVSurface *getHbbTVSurface() { return hbbtv_surface; }
		CorsairWebSurface *getWebSurface() { return web_surface; }
		virtual OipfSurface *getOipfSurface() = 0;

		CorsairHbbTVManager *getHbbTVManager() { return m_hbbtv_manager; }
#ifdef ENABLE_OPERA_TVSTORE
		CorsairOTVS *getOTVSHandle() { return m_otvs; }
#endif
		virtual bool requestGogiWindow(GogiOperaWindow** gogi_window, gogi_bool& granted, HBBTV_WindowIdentifier hbbtv_window_id);
		virtual void releaseGogiWindow(GogiOperaWindow* gogi_window);
		virtual void showGogiWindow(GogiOperaWindow* gogi_window);
		virtual void hideGogiWindow(GogiOperaWindow* gogi_window);
		virtual void showHbbTVSurface();
		virtual void showOpenWebSurface();
		virtual void showOipfSurface();
#ifdef ENABLE_OPERA_TVSTORE
		virtual void showOTVSSurface();
#endif
		virtual GogiOperaWindow* getFocusedWindow();

		static void corsair_flush_buffer(GogiRect* rects,	int num_rects, GogiScreenHandle screen);
		static HBBTV_STATUS notification_listener(HBBTV_NotificationType notification, HBBTV_NotificationData* data);
		static int opera_handle_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data);
		static void opera_handle_deferred_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data);
		static void close_signal(int signal, siginfo_t *info, void *context);
		static void abort_signal(int signal, siginfo_t *info, void *context);
		static void segv_signal(int signal, siginfo_t *info, void *context);
		static void pipe_signal(int signal, siginfo_t *info, void *context);
		static void childexit_signal(int signal);
		static void stop();
		static void saveCookie(void);
		void initUserAgent();

		CorsairIme *getIme() { return m_Ime; }
		CorsairPostman *getPostman() { return m_post_man; }

		struct timeval next_run_slice;
		static bool uses_vega;
		static bool do_run;
		static int m_frame_count;
		static bool m_count_frames;

	protected:
		virtual bool initHSDK(GogiOpera* opera);
		virtual void deinitHSDK();
		virtual HBBTV_EventData* getUAString();
		/** keep track on what gogi window is focused */
		void setFocusedWindow(GogiOperaWindow* window);

		CorsairHbbTVSurface* hbbtv_surface;
#ifdef ENABLE_OPERA_TVSTORE
		CorsairOTVSSurface* otvs_surface;
#endif
		CorsairWebSurface* web_surface;

		/** HbbTV window instance */
		HBBTV_WindowIdentifier hbbtv_window_id;
		std::vector<CorsairInput *> m_inputs;

	private:
		void initOpera();
		void scanFonts(const char* path);
		void setupSignals();
		void operaLoop();

	private:
		bool initProductEnv();
		bool initEnv();
		bool initAIT();
		bool initInput();
		bool deinitInput();
		bool readInput(int delay);
		bool createInternalObjects();
		bool destroyInternalObjects();

		GogiOperaWindow* focused_window;
		GogiOpera* opera;

		OperaActionQueue* action_queue;

		CorsairSecurityManager 	*m_security_manager;

		CorsairHbbTVManager		*m_hbbtv_manager;
		CorsairIme *m_Ime;
		CorsairPostman *m_post_man;
#ifdef ENABLE_OPERA_TVSTORE
		CorsairOTVS *m_otvs;
#endif
	public:
#ifdef ENABLE_FPS_DISPLAYER
		CorsairFPSDisplayer *m_fps_displayer;
#endif
		static Corsair *s_theApp;
};

static inline Corsair *getCorsair() { return Corsair::s_theApp; }

#ifdef __cplusplus
extern "C" {
#endif

void corsair_start(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif // CORSAIR_H

