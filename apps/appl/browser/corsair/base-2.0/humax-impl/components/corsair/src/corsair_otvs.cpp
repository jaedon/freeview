/** **********************************************************************************************************
	@file 		corsair_otvs.cpp
	
	@date		2012/09/13
	@author		IPM-SW1T(tipmsw1@humaxdigital.com)
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
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
#include "opera_action.h"
#include "corsair.h"
#include "corsair_otvs.h"
#include "corsair_gfx.h"
#include "corsair_hinput.h"
#include "oplappmgr.h"

using namespace std;

/********************************************************************************************
 * CorsairOTVS
 ********************************************************************************************/
CorsairOTVS::CorsairOTVS(CorsairOTVSSurface *surface):
	m_surface(surface),
	m_surface_own(false),
	m_grap_oipf_window(false)
{
	
}

CorsairOTVS::CorsairOTVS():
	m_surface(NULL),
	m_surface_own(true),
	m_grap_oipf_window(false)
{
	if (!CorsairGfx::getInstance())
	{
		LOG_ERR("Corsair GFX is not ready \n");
		return;
	}
#ifdef ENABLE_OPERA_TVSTORE	
	m_surface = CorsairGfx::getInstance()->createOTVSSurface(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, eAWINDOW_OTVS);
#endif
	if (!m_surface)
	{
		LOG_ERR(" : failed to allocate new surface  \n");
		return;
	}
	
	if(!m_surface->init(GOGI_FORMAT_VEGA_BGRA32, Corsair::corsair_flush_buffer))
	{
		LOG_ERR(" : failed to initialize new surface  \n");
		return;
	}
}

CorsairOTVS::~CorsairOTVS()
{
	if(m_surface_own && m_surface)
		delete m_surface;
}

void CorsairOTVS::otvs_handle_deferred_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data)
{
#ifdef ENABLE_OPERA_TVSTORE
	bool ret = false;
	Corsair* hCorsiar = NULL;
	CorsairOTVS* hOTVS = NULL;
	
	if ( getCorsair() == NULL )
		return;
	
	if ( getCorsair()->getOTVSHandle() == NULL )
		return;
		
	hOTVS = getCorsair()->getOTVSHandle();
	
	switch (event)
	{
		case GOGI_OPERA_EVT_LOAD_TV_APP:
			LOG_DEBUG("GOGI_OPERA_EVT_LOAD_TV_APP \n");
			if ( hOTVS->stopOTVSApplication())
        	{
				LOG_ERR("failed to finaliz the old resource for OTVS\n");
       	 	}
#if defined(NOTIFICATION_DEBUG)
       	 	LOG_DEBUG("	Application Info\n");
       	 	LOG_DEBUG("	name			: %s\n",data->tv_application.load.name);
       	 	LOG_DEBUG("	application		: %s\n",data->tv_application.load.application);
       	 	LOG_DEBUG("	app_mode		: %d\n",data->tv_application.load.app_mode);
       	 	LOG_DEBUG("	icon_url		: %s\n",data->tv_application.load.icon_url);
       	 	LOG_DEBUG("	pip_permission	: %d\n",data->tv_application.load.pip_permission);
       	 	LOG_DEBUG("	sbs_support		: %d\n",data->tv_application.load.sbs_support);
       	 	LOG_DEBUG("	broadcast_url	: %s\n",data->tv_application.load.broadcast_url);
#endif
			if ( hOTVS->startOTVSApplication( opera_window,
																	  data->tv_application.load.application,
																	  data->tv_application.load.app_mode,
																	  data->tv_application.load.sbs_support,
																	  data->tv_application.load.broadcast_url ) )
	        {
				LOG_ERR(" faled to start OTVS application\n");
			}
        	break;

        case GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL:
    		LOG_DEBUG("GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL\n");
    		LOG_DEBUG("	Forbidden Info\n");
       		LOG_DEBUG("	url			: %s\n",data->tv_application.forbidden.url);
       		hOTVS->startErrorPage( CorsairOTVS::ERRORPAGE_TYPE_FORBIDDEN_URL, data->tv_application.forbidden.url );
		   	break;

		case GOGI_OPERA_EVT_WINDOW_CLOSE:
			LOG_DEBUG("GOGI_OPERA_EVT_WINDOW_CLOSE  0x%x   0x%x  %d \n",opera_window,hOTVS->getOTVSTempWindow(),hOTVS->isOTVSWindow( opera_window ));
			if ( hOTVS->isOTVSWindow( opera_window ) == true || hOTVS->getOTVSTempWindow() == opera_window )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_WINDOW_CLOSE  0x%x \n",opera_window);
				if ( hOTVS->getOTVSApplicationWindow() == opera_window )
				{
					LOG_DEBUG("==== OTVS Application Window is closed\n");
					/* when the url of window 'A' is changed to others, if cosair get GOGI_OPERA_EVT_ERROR event,
					corsair make also the GOGI_OPERA_EVT_WINDOW_CLOSE event for window 'A' and corsair shall open error page for OTVS.
					so when corsair get GOGI_OPERA_EVT_WINDOW_CLOSE event to close OTVS pase,
					cosair shall check error page is opened to keep the original page.
					*/
					if ( !hOTVS->getErrorPageWindow() )
					{
						if ( hOTVS->stopOTVSApplication() )
						{
							LOG_ERR("failed to finaliz the old resource for OTVS\n");
						}
					}
	       	 	}
	       	 	else if ( hOTVS->getErrorPageWindow() == opera_window )
				{
					LOG_DEBUG("==== Error Page Window is closed\n");
					if ( hOTVS->stopErrorPage() )
					{
						LOG_ERR("failed to finaliz the old resource for ErrorPage\n");
					}
				}
				else if ( hOTVS->getOTVSPageWindow() == opera_window )
				{
					LOG_DEBUG("==== The first page of OTVS is closed\n");
					/* when the url of window 'A' is changed to others, if cosair get GOGI_OPERA_EVT_ERROR event,
					corsair make also the GOGI_OPERA_EVT_WINDOW_CLOSE event for window 'A' and corsair shall open error page for OTVS.
					so when corsair get GOGI_OPERA_EVT_WINDOW_CLOSE event to close OTVS pase,
					cosair shall check error page is opened to keep the original page.
					*/
					if ( !hOTVS->getErrorPageWindow() )
					{
						if ( hOTVS->stopOTVSPage() )
						{
							LOG_ERR("failed to finaliz the old resource for TestPage\n");
						}
					}
				}
				else if( hOTVS->getOTVSTempWindow() == opera_window )
				{
					hOTVS->setOTVSTempWindow(NULL);
					op_close_window( opera, opera_window );
				}
				
				if ( hOTVS->getFocusedWindow() )
				{
					/* check OTVS has some windows, if yes , focus is changed to the window */
					hOTVS->showOTVS();
				}
				else
				{
					/* if not, oipf will have the focus */
					getCorsair()->showOipfSurface();
				}
			}
			break;
		
		case GOGI_OPERA_EVT_LOADING_FAILED:
			LOG_DEBUG("GOGI_OPERA_EVT_LOADING_FAILED 0x%x \n",opera_window);
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				op_replace_url(opera_window, "file:///usr/browser/opera_dir/error/network_error.html");
			}
			break;
			
		case GOGI_OPERA_EVT_OUT_OF_MEMORY:
			LOG_DEBUG("GOGI_OPERA_EVT_OUT_OF_MEMORY \n");
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				hOTVS->startErrorPage( CorsairOTVS::ERRORPAGE_TYPE_OUT_OF_MEMORY, NULL );
			}
			break;
			
		default:
			LOG_DEBUG("==== Unknown deferred notification\n");
			break;
	}
#endif
	return;
}

bool CorsairOTVS::otvs_handle_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data)
{
#ifdef ENABLE_OPERA_TVSTORE
	bool ret = false;
	Corsair* hCorsiar = NULL;
	CorsairOTVS* hOTVS = NULL;
	
	if ( getCorsair() == NULL )
		return false;
	
	if ( getCorsair()->getOTVSHandle() == NULL )
		return false;
		
	hOTVS = getCorsair()->getOTVSHandle();
	
	switch (event)
	{
		case GOGI_OPERA_EVT_ERROR:
			LOG_DEBUG("GOGI_OPERA_EVT_ERROR\n");
#if defined(NOTIFICATION_DEBUG)
			LOG_DEBUG("    document_error      : %d \n",data->error.document_erro);
			LOG_DEBUG("    file_error          : %d \n",data->error.file_erro);
			LOG_DEBUG("    http_error          : %d \n",data->error.http_erro);
			LOG_DEBUG("    dsmcc_error         : %d \n",data->error.dsmcc_erro);
			LOG_DEBUG("    tls_error           : %d \n",data->error.tls_erro);
			LOG_DEBUG("    network_error       : %d \n",data->error.network_erro);
			LOG_DEBUG("    boundary_error      : %d \n",data->error.boundary_erro);
			LOG_DEBUG("    redirection_error   : %d \n",data->error.redirection_erro);
#endif
			/* the error popup should be displayed when TV Portal running */
			break;
			
		case GOGI_OPERA_EVT_LOAD_TV_APP:
			LOG_DEBUG("GOGI_OPERA_EVT_LOAD_TV_APP\n");
			getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, event, data));
			return true;

		case GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL:
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_TV_APP_FORBIDDEN_URL\n");
				getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, event, data));
			}
			return true;

		case GOGI_OPERA_EVT_WINDOW_CLOSE:
			if ( hOTVS->isOTVSWindow( opera_window ) == true || hOTVS->getOTVSTempWindow() == opera_window )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_WINDOW_CLOSE 0x%x \n",opera_window);
				getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, event, data));
			}
			return true;
		
		case GOGI_OPERA_EVT_WINDOW_REQUEST:
			LOG_DEBUG("GOGI_OPERA_EVT_WINDOW_REQUEST 0x%x \n",opera_window);
#if defined(NOTIFICATION_DEBUG)
			LOG_DEBUG("data->window.url : %s \n",data->window.url);
			LOG_DEBUG("data->window.width : %d \n",data->window.width);
			LOG_DEBUG("data->window.height : %d \n",data->window.height);
			LOG_DEBUG("data->window.scrollbars : %d \n",data->window.scrollbars);
			LOG_DEBUG("data->window.toolbars : %d \n",data->window.toolbars);
			LOG_DEBUG("data->window.focus_document : %d \n",data->window.focus_document);
			LOG_DEBUG("data->window.open_background : %d \n",data->window.open_background);
			LOG_DEBUG("data->window.transparent : %d \n",data->window.transparent);
			LOG_DEBUG("data->window.script_invoked : %d \n",data->window.script_invoked);
			LOG_DEBUG("data->window.user_initiated : %d \n",data->window.user_initiated);
			LOG_DEBUG("data->window.screen_handle : %x \n",data->window.screen_handle);
			LOG_DEBUG("data->window.opener : %x \n",data->window.opener);
			LOG_DEBUG("data->window.private_data : %x \n",data->window.private_data);
			LOG_DEBUG("data->window.widget : %x \n",data->window.widget);
			LOG_DEBUG("data->window.window_type : %d \n",data->window.window_type);
#endif
			return true;
			
		case GOGI_OPERA_EVT_WINDOW_NEW:
			LOG_DEBUG("GOGI_OPERA_EVT_WINDOW_NEW 0x%x \n",opera_window);
#if defined(NOTIFICATION_DEBUG)
			LOG_DEBUG("data->window.url : %s \n",data->window.url);
			LOG_DEBUG("data->window.width : %d \n",data->window.width);
			LOG_DEBUG("data->window.height : %d \n",data->window.height);
			LOG_DEBUG("data->window.scrollbars : %d \n",data->window.scrollbars);
			LOG_DEBUG("data->window.toolbars : %d \n",data->window.toolbars);
			LOG_DEBUG("data->window.focus_document : %d \n",data->window.focus_document);
			LOG_DEBUG("data->window.open_background : %d \n",data->window.open_background);
			LOG_DEBUG("data->window.transparent : %d \n",data->window.transparent);
			LOG_DEBUG("data->window.script_invoked : %d \n",data->window.script_invoked);
			LOG_DEBUG("data->window.user_initiated : %d \n",data->window.user_initiated);
			LOG_DEBUG("data->window.screen_handle : %x \n",data->window.screen_handle);
			LOG_DEBUG("data->window.opener : %x \n",data->window.opener);
			LOG_DEBUG("data->window.private_data : %x \n",data->window.private_data);
			LOG_DEBUG("data->window.widget : %x \n",data->window.widget);
			LOG_DEBUG("data->window.window_type : %d \n",data->window.window_type);
#endif
			hOTVS->setOTVSTempWindow( opera_window );
			return true;

		case GOGI_OPERA_EVT_LOADING_FAILED:
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_LOADING_FAILED\n");
#if defined(NOTIFICATION_DEBUG)
				LOG_DEBUG("    image_count            : %d \n",data->loading.image_count);
				LOG_DEBUG("    loaded_image_count     : %d \n",data->loading.loaded_image_count);
				LOG_DEBUG("    byte_count             : %d \n",data->loading.byte_count);
				LOG_DEBUG("    loaded_byte_count      : %d \n",data->loading.loaded_byte_count);
				LOG_DEBUG("    document_bytes_total   : %d \n",data->loading.document_bytes_total);
				LOG_DEBUG("    document_bytes_loaded  : %d \n",data->loading.document_bytes_loaded);
				LOG_DEBUG("    has_requested_document : %d \n",data->loading.has_requested_document);
				LOG_DEBUG("    message                : %s \n",data->loading.message);
				LOG_DEBUG("    msg_id                 : %d \n",data->loading.msg_id);
				LOG_DEBUG("    uploading              : %d \n",data->loading.uploading);
				LOG_DEBUG("    state                  : %d \n",data->loading.state);
				LOG_DEBUG("    http_response_code     : %d \n",data->loading.http_response_code);
				LOG_DEBUG("    current_percentage     : %d \n",data->loading.current_percentage);
				LOG_DEBUG("    content_type           : %s \n",data->loading.content_type);
#endif
				LOG_DEBUG("    opera_window   : 0x%x \n",opera_window);
				LOG_DEBUG("    ApplicationWindow   : 0x%x \n",hOTVS->getOTVSApplicationWindow());
				LOG_DEBUG("    PageWindow   : 0x%x \n",hOTVS->getOTVSPageWindow());
				getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, GOGI_OPERA_EVT_LOADING_FAILED, data));
			}
			return true;
			
		case GOGI_OPERA_EVT_EXTERNAL_ACTION:
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_EXTERNAL_ACTION\n");
				if ( data->external_action.action )
					LOG_DEBUG("GOGI_OPERA_EVT_EXTERNAL_ACTION Action: %s \n",data->external_action.action);
				LOG_DEBUG(" -- Parameter: %s \n",data->external_action.parameter);
				LOG_DEBUG(" -- Poll: %s \n",data->external_action.poll);
				if ( data->external_action.action && strcmp( "Exit", data->external_action.action ) == 0 )
				{
					LOG_DEBUG(" Exit - External Action is happened\n");
					getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, GOGI_OPERA_EVT_WINDOW_CLOSE, data));
				}
			}
			return true;
			
		case GOGI_OPERA_EVT_OUT_OF_MEMORY:
			if ( hOTVS->isOTVSWindow( opera_window ) == true )
			{
				LOG_DEBUG("GOGI_OPERA_EVT_OUT_OF_MEMORY \n");
				getCorsair()->getActionQueue()->add(new OtvsNotificationAction(opera, opera_window, GOGI_OPERA_EVT_OUT_OF_MEMORY, data));
			}
			return true;
			
		default:
			break;
			
	}
#endif
	return false;
}

bool CorsairOTVS::isOTVSWindow( GogiOperaWindow* window)
{
	if( window == getOTVSApplicationWindow() ||
		window == getOTVSPageWindow() ||
		window == getErrorPageWindow() )
	{
		return true;
	}
	return false;
}

int CorsairOTVS::startOTVSApplication( void* window_id, const char* application_url, int app_mode, int sbs_support, const char* broadcast_url )
{
	char *url;
	string cmp_url;
	
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if (!m_surface->getOTVSApplicationWindow())
	{
		m_surface->requestOTVSApplicationWindow();
		if (!m_surface->getOTVSApplicationWindow())
		{
			LOG_ERR(" : failed to create new window\n");
			return 1;
		}
	}
	
	if ( op_open_tv_app((GogiOperaWindow*)window_id, m_surface->getOTVSApplicationWindow(), application_url, (GOGI_TV_APP_MODE)app_mode, sbs_support, broadcast_url ) != GOGI_STATUS_OK )
	{
		LOG_ERR(" : failed to start OTVS Application\n");
		return 1;
	}

	m_surface->setFocusedWindow(m_surface->getOTVSApplicationWindow());
		
	/* TV apps -> start favorite OTVS app directly, in this case, OTVSPageWindow should be NULL. So onLoaded event shall be made. */
	if ( m_surface->getOTVSPageWindow() == NULL )
		HAPI_BroadcastSignal("signal:onHumaxApplicationLoaded", 1, "s", "otvs");
	
	return 0;
}

int CorsairOTVS::stopOTVSApplication(void)
{
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if ( m_surface->getOTVSApplicationWindow() )
	{
		hideOTVS();
		m_surface->releaseOTVSApplicationWindow();
		LOG_DEBUG("==== check release 0x%x \n",m_surface->getOTVSApplicationWindow());

		//if ( getFocusedWindow() == NULL )
		//	HAPI_BroadcastSignal("signal:onHumaxApplicationUnloaded", 1, "s", "otvs");
	}
			
	return 0;
}

GogiOperaWindow* CorsairOTVS::getOTVSApplicationWindow(void)
{
	return m_surface->getOTVSApplicationWindow();
}


int CorsairOTVS::startErrorPage( ERRORPAGE_TYPE_t type, void* msg )
{
	string url = "file:///usr/browser/opera_dir/error/otvs_error/otvs_error_application.html";
	char type_tmp[4] = {0,};
	
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if (!m_surface->getOTVSErrorPageWindow())
	{
		m_surface->requestOTVSErrorPageWindow();
		if (!m_surface->getOTVSErrorPageWindow())
		{
			LOG_ERR(" : failed to create new window\n");
			return 1;
		}
	}
	
	snprintf( type_tmp, sizeof(type_tmp), "%d", type );
	url.append("?type=");
	url.append(type_tmp);
		
	switch( type )
	{
		case ERRORPAGE_TYPE_FORBIDDEN_URL:
			url.append("&");
			url.append( (char*)msg);
			break;
			
		case ERRORPAGE_TYPE_OUT_OF_MEMORY:
			break;
			
		/* at this time, only forbidden error is handled on this function */
		default:
			return 1;
			break;
	}
		
	op_open_url( m_surface->getOTVSErrorPageWindow(), url.c_str());
	
	m_surface->setFocusedWindow(m_surface->getOTVSErrorPageWindow());

	return 0;
}

int CorsairOTVS::stopErrorPage(void)
{
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if (!m_surface->getOTVSErrorPageWindow())
	{
		LOG_ERR(" : there is not the OTVS window(app is not running)\n");
		return 0;
	}
	
	hideOTVS();
	
	m_surface->releaseOTVSErrorPageWindow();
	LOG_DEBUG("==== check release 0x%x \n",m_surface->getOTVSErrorPageWindow());
	return 0;
}

GogiOperaWindow* CorsairOTVS::getErrorPageWindow(void )
{
	return m_surface->getOTVSErrorPageWindow();
}

int CorsairOTVS::startOTVSPage( const char* url  )
{
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if (!m_surface->getOTVSPageWindow())
	{
		m_surface->requestOTVSPageWindow();
		if (!m_surface->getOTVSPageWindow())
		{
			LOG_ERR(" : failed to create new window\n");
			return 1;
		}
	}
	
	op_open_url( m_surface->getOTVSPageWindow(), url );
	m_surface->setFocusedWindow(m_surface->getOTVSPageWindow());
	
	HAPI_BroadcastSignal("signal:onHumaxApplicationLoaded", 1, "s", "otvs");
				
	return 0;
}

int CorsairOTVS::stopOTVSPage(void)
{
	if (!m_surface )
	{
		LOG_ERR(" : window is not ready\n");
		return 1;
	}
	
	if (!m_surface->getOTVSPageWindow())
	{
		LOG_ERR(" : there is not the OTVS window(app is not running)\n");
		return 0;
	}
	
	hideOTVS();
	
	m_surface->releaseOTVSPageWindow();
		
	//HAPI_BroadcastSignal("signal:onHumaxApplicationUnloaded", 1, "s", "otvs");
	
	LOG_DEBUG("==== check release 0x%x \n",m_surface->getOTVSPageWindow());
	return 0;
}

GogiOperaWindow* CorsairOTVS::getOTVSPageWindow(void )
{
	return m_surface->getOTVSPageWindow();
}

int CorsairOTVS::showOTVS(void)
{
	m_surface->showSurface(); 
	return 0;
}
	
int CorsairOTVS::hideOTVS(void)
{
	m_surface->hideSurface(); 
	return 0;
}

GogiOperaWindow* CorsairOTVS::getFocusedWindow(void)
{
	return 	m_surface->getFocusedWindow();
}

GogiOperaWindow* CorsairOTVS::getOTVSTempWindow(void)
{
	return 	m_surface->getOTVSTempWindow();
}	

void CorsairOTVS::setOTVSTempWindow(GogiOperaWindow* window)
{
	return 	m_surface->setOTVSTempWindow(window);
}
	
