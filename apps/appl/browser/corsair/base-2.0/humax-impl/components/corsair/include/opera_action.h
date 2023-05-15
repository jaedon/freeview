/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef __OPERA_ACTION_H_
#define __OPERA_ACTION_H_

#define MAX_QUEUE_SIZE 200

#include <iostream>
#include <string>
#include <malloc.h>
#include "string.h"
#include "stdarg.h"
#include "stdlib.h"

#include "gogi_opera_api.h"
#include "corsair.h"
#include "corsair_otvs.h"


// ----------------------------------------------
// OperaAction
//
// Abstract parent class implemented by actions which are to be
// performed asynchronously.
// ----------------------------------------------


class OperaAction
{
	public:
		virtual ~OperaAction() {}
		virtual void execute() = 0;
};


// ----------------------------------------------
// OperaActionQueue
//
// Queue to place Opera actions in. This is generally used to avoid
// calling Opera API functions while inside an Opera callback, or from
// different threads. See the SDK documentation on reentrancy and
// thread safety for more information.
// ----------------------------------------------

class OperaActionQueue
{
	public:
		OperaActionQueue() : m_length(0) {}
		~OperaActionQueue() { clean(); }

		bool add(OperaAction* action)
		{
			if (m_length >= MAX_QUEUE_SIZE)
			{
				LOG_DEBUG("Action queue full, dropping action");
				return false;
			}

			m_queue[m_length++] = action;

			return true;
		}

		void executeAll()
		{
			// Execute all pending actions.
			for (int i = 0; i < m_length; ++i)
			{
				m_queue[i]->execute();
			}
			// Clean the queue
			clean();
		}

	private:
		void clean()
		{
			for (int i = 0; i < m_length; ++i)
			{
				delete m_queue[i];
				m_queue[i] = NULL;
			}
			m_length = 0;
		}

		int m_length;
		OperaAction* m_queue[MAX_QUEUE_SIZE];
};

// ----------------------------------------------
// NotificationAction
//
// This class is used to perform asynchronous notification handling in
// order to be able to call GOGI API functions.  The actual
// implementation resides in the opera_handle_deferred_notification
// function.  ----------------------------------------------
class NotificationAction : public OperaAction
{
	public:
		NotificationAction(GogiOpera* _opera,
		GogiOperaWindow* _notificationWindow,
		GOGI_OPERA_EVENT _event,
		GogiOperaEventData* _data)
			: opera(_opera),
			  notificationWindow(_notificationWindow),
			  event(_event),
			  data(NULL)
		{
			if (!_data) return;

			data = new GogiOperaEventData;
			*data = *_data;

			// Make deep copy of the fields we are using.
			switch(event)
			{
				case GOGI_OPERA_EVT_ASYNC_DIALOG:
					data->dialog.title = strdup(_data->dialog.origin ? _data->dialog.origin : "");
					data->dialog.origin = strdup(_data->dialog.origin ? _data->dialog.origin : "");
					data->dialog.extensions = NULL;
					data->dialog.message = strdup(_data->dialog.message ? _data->dialog.message : "");
					break;

				case GOGI_OPERA_EVT_LOADING_PROGRESS:
					data->loading.message = strdup(_data->loading.message ? _data->loading.message : "");
					break;

				case GOGI_OPERA_EVT_URL_CHANGE:
					data->url.url = strdup(_data->url.url ? _data->url.url : "");
					break;

				case GOGI_OPERA_EVT_RSS_FEED:
					data->url.url = strdup(_data->url.url ? _data->url.url : "");
					break;

				case GOGI_OPERA_EVT_PROTOCOL_LOADING_START:
					data->protocol.name = strdup(_data->protocol.name ? _data->protocol.name : "");
					data->protocol.url = strdup(_data->protocol.url ? _data->protocol.url : "");
					break;

				case GOGI_OPERA_EVT_PLATFORM_MESSAGE:
					data->platform_message.message = strdup(_data->platform_message.message ? _data->platform_message.message : "");
					break;

				case GOGI_OPERA_EVT_LOADING_START:
				case GOGI_OPERA_EVT_LOADING_FINISHED:
				case GOGI_OPERA_EVT_DOCUMENT_ICON:
				case GOGI_OPERA_EVT_EXISTS_HISTORY:
				case GOGI_OPERA_EVT_LEAVE:
				case GOGI_OPERA_EVT_FEED_SUBSCRIBED:
				case GOGI_OPERA_EVT_LINK_NAVIGATED:
				case GOGI_OPERA_EVT_WINDOW_RESIZE:
				case GOGI_OPERA_EVT_WINDOW_CLOSE:
				case GOGI_OPERA_EVT_TV_WINDOW_RECT:
				case GOGI_OPERA_EVT_SCREEN_INVALID:
				case GOGI_OPERA_EVT_ERROR:
					break;

				default:
					LOG_DEBUG("Unknown event to NotificationAction constructor.");
					break;
			}
		}

		virtual ~NotificationAction()
		{
			switch(event)
			{
				case GOGI_OPERA_EVT_ASYNC_DIALOG:
					free(const_cast<char*>(data->dialog.title));
					free(const_cast<char*>(data->dialog.origin));
					free(const_cast<char*>(data->dialog.message));
					break;

				case GOGI_OPERA_EVT_LOADING_PROGRESS:
					free(const_cast<char*>(data->loading.message));
					break;

				case GOGI_OPERA_EVT_URL_CHANGE:
					free(const_cast<char*>(data->url.url));
					break;

				case GOGI_OPERA_EVT_RSS_FEED:
					free(const_cast<char*>(data->url.url));
					break;

				case GOGI_OPERA_EVT_PROTOCOL_LOADING_START:
					free(const_cast<char*>(data->protocol.name));
					free(const_cast<char*>(data->protocol.url));
					break;

				case GOGI_OPERA_EVT_PLATFORM_MESSAGE:
					free(const_cast<char*>(data->platform_message.message));
					break;

				case GOGI_OPERA_EVT_LOADING_START:
				case GOGI_OPERA_EVT_LOADING_FINISHED:
				case GOGI_OPERA_EVT_DOCUMENT_ICON:
				case GOGI_OPERA_EVT_EXISTS_HISTORY:
				case GOGI_OPERA_EVT_LEAVE:
				case GOGI_OPERA_EVT_FEED_SUBSCRIBED:
				case GOGI_OPERA_EVT_LINK_NAVIGATED:
				case GOGI_OPERA_EVT_WINDOW_RESIZE:
				case GOGI_OPERA_EVT_WINDOW_CLOSE:
				case GOGI_OPERA_EVT_SCREEN_INVALID:
				case GOGI_OPERA_EVT_ERROR:
					break;

				default:
					LOG_DEBUG("Unknown event to NotificationAction destructor.");
					break;
			}

			delete data;
		}

		virtual void execute()
		{
			Corsair::opera_handle_deferred_notification(opera, notificationWindow, event, data);
		}

	private:
		GogiOpera* opera;
		GogiOperaWindow* notificationWindow;
		GOGI_OPERA_EVENT event;
		GogiOperaEventData* data;
};


class ShowHbbTVWindowAction : public OperaAction
{
	public:
		ShowHbbTVWindowAction(GogiOperaWindow* _window, Corsair* _corsair) : window(_window), corsair(_corsair) { }
		void execute()
		{
			if (window)
				corsair->showGogiWindow(window);
		}
	private:
		GogiOperaWindow* window;
		Corsair* corsair;
};

class HideHbbTVWindowAction : public OperaAction
{
	public:
		HideHbbTVWindowAction(GogiOperaWindow* _window, Corsair* _corsair) : window(_window), corsair(_corsair) { }
		void execute()
		{
			if (window)
				corsair->hideGogiWindow(window);
		}
	private:
		GogiOperaWindow* window;
		Corsair* corsair;
};

class OtvsNotificationAction : public OperaAction
{
	public:
		OtvsNotificationAction(GogiOpera* _opera,
		GogiOperaWindow* _notificationWindow,
		GOGI_OPERA_EVENT _event,
		GogiOperaEventData* _data)
			: opera(_opera),
			  notificationWindow(_notificationWindow),
			  event(_event),
			  data(NULL)
		{
			if (!_data) return;

			data = new GogiOperaEventData;
			*data = *_data;

			// Make deep copy of the fields we are using.
			switch(event)
			{
				case GOGI_OPERA_EVT_LOAD_TV_APP:
					data->tv_application.load.name = strdup(_data->tv_application.load.name ? _data->tv_application.load.name : "");
					data->tv_application.load.application = strdup(_data->tv_application.load.application ? _data->tv_application.load.application : "");
					data->tv_application.load.app_mode = _data->tv_application.load.app_mode;
					data->tv_application.load.icon_url = strdup(_data->tv_application.load.icon_url ? _data->tv_application.load.icon_url : "");
					break;

				case GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL:
					data->tv_application.forbidden.url = strdup(_data->tv_application.forbidden.url ? _data->tv_application.forbidden.url : "");
					break;
			
				case GOGI_OPERA_EVT_LOADING_FAILED:
					data->loading.message = strdup(_data->loading.message);
					break;

				case GOGI_OPERA_EVT_WINDOW_CLOSE:
					break;
					
				default:
					LOG_DEBUG("Unknown event to OtvsNotificationAction constructor.");
					break;
			}
		}

		virtual ~OtvsNotificationAction()
		{
			switch(event)
			{
				case GOGI_OPERA_EVT_LOAD_TV_APP:
					free(const_cast<char*>(data->tv_application.load.name));
					free(const_cast<char*>(data->tv_application.load.application));
					free(const_cast<char*>(data->tv_application.load.icon_url));
					break;

				case GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL:
					free(const_cast<char*>(data->tv_application.forbidden.url));
					break;
					
				case GOGI_OPERA_EVT_LOADING_FAILED:
					free(const_cast<char*>(data->loading.message));
					break;

				case GOGI_OPERA_EVT_WINDOW_CLOSE:
					break;
					
				default:
					LOG_DEBUG("Unknown event to OtvsNotificationAction destructor.");
					break;
			}

			delete data;
		}
		
		void execute()
		{
			CorsairOTVS::otvs_handle_deferred_notification(opera, notificationWindow, event, data);
		}
		
	private:
		GogiOpera* opera;
		GogiOperaWindow* notificationWindow;
		GOGI_OPERA_EVENT event;
		GogiOperaEventData* data;
};

#endif //__OPERA_ACTION_H_
