/** **********************************************************************************************************
	@file 		corsair_otvs_surface.cpp
	
	@date		2012/09/25
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
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

#ifndef __CORSAIR_OTVS_SURFACE_H
#define __CORSAIR_OTVS_SURFACE_H

#include <directfb.h>
#include <vector>
#include "corsair_config.h"
#include "corsair_surface.h"
#include "gogi_opera_api.h"

/**
   Surface class for windows running HbbTV applications.

   This class allow (gogi) windows to be requested for use in the hbbtv scope.

   See \ref Corsair for usage of the class.
*/
class CorsairOTVSSurface : public CorsairSurface
{
public:
	CorsairOTVSSurface(int x, int y, int w, int h);
	CorsairOTVSSurface(int x, int y, int w, int h, int appId);
	virtual ~CorsairOTVSSurface();

	/** Gets the window that is currently focused in the surface */
	int setFocusedWindow(GogiOperaWindow* focused_window);
	GogiOperaWindow* getFocusedWindow();
	GogiOperaWindow* getOTVSApplicationWindow() { return m_gogi_otvsapplication_window; }
	GogiOperaWindow* getOTVSErrorPageWindow() { return m_gogi_errorpage_window; }
	GogiOperaWindow* getOTVSPageWindow() { return m_gogi_otvspage_window; }
	GogiOperaWindow* getOTVSTempWindow() { return m_gogi_otvstemp_window; }
	void setOTVSTempWindow(GogiOperaWindow* window) { m_gogi_otvstemp_window = window; }

	/** Set surface visability */
	virtual void showSurface();
	virtual void hideSurface();

	/**
	 * Request a gogi window.
	 *
	 * @return the created gogi window
	 */
	virtual GogiOperaWindow* requestWindow( GogiOperaWindow* &window );
	GogiOperaWindow* requestOTVSApplicationWindow();
	GogiOperaWindow* requestOTVSErrorPageWindow();
	GogiOperaWindow* requestOTVSPageWindow();
		
	/**
	 * This is invoked from the HbbTV library to release a previously requested window.
	 * @return true if window was released, false if window was not found.
	 */
	virtual bool releaseWindow( GogiOperaWindow* &window );
	bool releaseOTVSApplicationWindow();
	bool releaseOTVSErrorPageWindow();
	bool releaseOTVSPageWindow();
	
protected:
    /** member functions */
	GogiOperaWindow* createGogiWindow();
	void destroyGogiWindow(GogiOperaWindow* window);
	
private:
	/** member properties */
	GogiOperaWindow* m_gogi_otvsapplication_window;
	GogiOperaWindow* m_gogi_errorpage_window;
	GogiOperaWindow* m_gogi_otvspage_window;
	GogiOperaWindow* m_gogi_otvstemp_window;
	GogiOperaWindow* m_last_focused_window;
	
};


#endif //__CORSAIR_OTVS_SURFACE_H
