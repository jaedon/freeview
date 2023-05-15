/** **********************************************************************************************************
	@file 		corsair_otvs.h
	
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

#ifndef __CORSAIR_OTVS_H__
#define __CORSAIR_OTVS_H__

#include "corsair_otvs_surface.h"

/**********************************************************************************************
 * class CorsairOTVS
 **********************************************************************************************/
class CorsairOTVS {

public:
	typedef enum{
		ERRORPAGE_TYPE_NETWORK_CONNECTED,
		ERRORPAGE_TYPE_NETWORK_NETROKERROR,
		ERRORPAGE_TYPE_FORBIDDEN_URL,
		ERRORPAGE_TYPE_OUT_OF_MEMORY,
	}ERRORPAGE_TYPE_t;
	
	CorsairOTVS(CorsairOTVSSurface *surface);
	CorsairOTVS();
	virtual ~CorsairOTVS();

	/**
		@fn 	otvs_handle_deferred_notification
		@breif 	this function is to handle OTVS action
		@param	opera - input - 
		@param	opera_window - input - 
		@param	event - input - 
		@param	data - input - 
		@return N/A
	*/
	static void otvs_handle_deferred_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data);

	/**
		@fn 	otvs_handle_notification
		@breif 	this function is to handle GOGI Event for OTVS
		@param	opera - input - 
		@param	opera_window - input - 
		@param	event - input - 
		@param	data - input - 
		@return true - this event is handed by OTVS
		@return false - this event is not handed by OTVS
	*/
	bool otvs_handle_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data);
	
	/**
		@fn 	isOTVSWindow
		@breif 	this function is to check the window is on OTVS Surface.
		@param	window - input - the window to check
		@return 1 - yes
		@return 0 - No
	*/
	bool isOTVSWindow( GogiOperaWindow* window);
			
	/**
		@fn 	startOTVSApplication
		@breif 	this function is to start OTVS application. 
		@param	window_id - input - the window id which application_url is started from
		@param	application_url - input - the url of OTVS application to run 
		@param	app_mode - input - application window mode
		@param	sbs_support - input - sbs mode support or not
		@param	broadcast_url - input - the url of VB.
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int startOTVSApplication( void* window_id, const char* application_url, int app_mode, int sbs_support, const char* broadcast_url );

	/**
		@fn 	stopOTVSApplication
		@breif 	this function is to stop OTVS application. 
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int stopOTVSApplication(void );
	
	/**
		@fn 	getOTVSApplicationWindow
		@breif 	this function is to get the window for OTVS.
		@param	N/A
		@return pinter	- the pointer for the window handle;
	*/
	GogiOperaWindow* getOTVSApplicationWindow(void );
	
	/**
		@fn 	startErrorPage
		@breif 	this function is to start displaying error page
		@param	msg - input - the additional string to display. it should be encoded by UTF-8
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int startErrorPage( ERRORPAGE_TYPE_t type, void* msg );
	
	/**
		@fn 	stopErrorPage
		@breif 	this function is to stop displaying error page.
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int stopErrorPage(void);
	
	/**
		@fn 	getErrorPageWindow
		@breif 	this function is to get the window for Error page.
		@param	N/A
		@return pinter	- the pointer for the window handle;
	*/
	GogiOperaWindow* getErrorPageWindow(void );
	
	/**
		@fn 	startTestPage
		@breif 	this function is to start displaying test url
		@param	msg - input - the additional string to display. it should be encoded by UTF-8
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int startOTVSPage( const char* url );
	
	/**
		@fn 	stopTestPage
		@breif 	this function is to stop displaying error page.
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int stopOTVSPage(void);
	
	/**
		@fn 	getTestPageWindow
		@breif 	this function is to get the window for Error page.
		@param	N/A
		@return pinter	- the pointer for the window handle;
	*/
	GogiOperaWindow* getOTVSPageWindow(void );
	
	/**
		@fn 	showOTVS
		@breif 	this function is to display the OTVS window
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int showOTVS(void );

	/**
		@fn 	hideOTVS
		@breif 	this function is to hide the OTVS window
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int hideOTVS(void );

	/**
		@fn 	getFocusedWindow
		@breif 	this function is to hide the OTVS window
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	GogiOperaWindow* getFocusedWindow(void);
	
	/**
		@fn 	getOTVSTempWindow
		@breif 	this function is to hide the OTVS window
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	GogiOperaWindow* getOTVSTempWindow(void);
	
		/**
		@fn 	setOTVSTempWindow
		@breif 	this function is to hide the OTVS window
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	void setOTVSTempWindow(GogiOperaWindow* window);
		
private:
	CorsairOTVSSurface *m_surface;
	bool m_surface_own;
	bool m_grap_oipf_window;
};
#endif	//__CORSAIR_OTVS_H__
