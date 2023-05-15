/** **********************************************************************************************************
	@file 		corsair_gfx_egl.cpp
	
	@date		2013/11/20
	@author		STB-Compoment(tstbcomp@humaxdigital.com)
	@breif		
	
	(c) 2011-2013 Humax Co., Ltd. 
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

#include "corsair_gfx_egl.h"
#include "corsair_gfx.h"
#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* declare external interface */
typedef void (*BufferOnDisplayFunc)(uint32_t);

BEGL_MemoryInterface  *NXPL_CreateMemInterface(BEGL_HWInterface *hwIface);
BEGL_HWInterface      *NXPL_CreateHWInterface(void);
BEGL_DisplayInterface *DBPL_CreateDisplayInterface(IDirectFB *dfb,
		BEGL_MemoryInterface *memIface,
		BEGL_DisplayCallbacks *displayCallbacks);

void NXPL_DestroyMemInterface(BEGL_MemoryInterface *iface);
void NXPL_DestroyHWInterface(BEGL_HWInterface *iface);
void DBPL_DestroyDisplayInterface(BEGL_DisplayInterface * disp);

#ifdef __cplusplus
}
#endif

EGLDisplay CorsairGfxEgl::m_display;
EGLConfig CorsairGfxEgl::m_config;
EGLContext CorsairGfxEgl::m_opera_context;
DBPL_PlatformHandle CorsairGfxEgl::m_dbpl_handle;	

CorsairGfxEgl::CorsairGfxEgl()
{
	m_dbpl_handle = 0;
}

CorsairGfxEgl::~CorsairGfxEgl()
{

}

CorsairGfxEgl* CorsairGfxEgl::getInstance()
{
	static CorsairGfxEgl* instance = NULL;
	if (!instance)
		instance = new CorsairGfxEgl();
	return instance;
}

bool CorsairGfxEgl::init(void)
{
	DBPL_RegisterDirectFBDisplayPlatform();

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if ( m_display == EGL_NO_DISPLAY )
	{
		LOG_ERR("Failed to get EGL display\n");
		return false;
	}

	EGLint major = 0, minor = 0;
	if ( !eglInitialize(m_display, &major, &minor) )
	{
		LOG_ERR("Failed to initialize EGL\n");
		return false;
	}

	int configs = 0;
	if ( !eglGetConfigs(m_display, NULL, 0, &configs) )
	{
		LOG_ERR("Failed to get EGL configurations\n");
		return false;
	}

	EGLint config_attrs[] = {
		EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,   EGL_OPENGL_ES2_BIT,
		EGL_DEPTH_SIZE,        24,
		EGL_RED_SIZE,          8,
		EGL_GREEN_SIZE,        8,
		EGL_BLUE_SIZE,         8,
		EGL_ALPHA_SIZE,        8,
		EGL_STENCIL_SIZE,      0,
		EGL_BUFFER_SIZE,       EGL_DONT_CARE,
		EGL_NONE
	};

	if ( !eglChooseConfig(m_display, config_attrs, &m_config, configs, &configs) || (configs == 0) )
	{
		LOG_ERR("Failed to choose EGL configuration\n");
		return false;
	}

    EGLint contextAttribs[] = { 
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };  

    m_opera_context = eglCreateContext(m_display, m_config, NULL, contextAttribs);
	if ( eglSwapInterval(m_display, 0) != EGL_TRUE )
	{	
		LOG_ERR("Failed to create opera context.\n");
		return false;
	}

	return true;
}

void CorsairGfxEgl::deinit()
{
	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_display, m_opera_context);
	eglTerminate(m_display);
	m_opera_context = EGL_NO_CONTEXT;
	m_display = EGL_NO_DISPLAY;

	DBPL_UnregisterDirectFBDisplayPlatform();
}

void CorsairGfxEgl::DBPL_RegisterDirectFBDisplayPlatform(void)
{
	IDirectFB *dfb = CorsairGfx::getInstance()->getDfbInstance();

	BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));
	memset(data, 0, sizeof(BEGL_DriverInterfaces));

	if (data != NULL)
	{
		BEGL_GetDefaultDriverInterfaces(data);

		data->hwInterface = NXPL_CreateHWInterface();
		data->memInterface = NXPL_CreateMemInterface(data->hwInterface);
		data->displayInterface = DBPL_CreateDisplayInterface(dfb,
				data->memInterface,
				&data->displayCallbacks);

		m_dbpl_handle = (DBPL_PlatformHandle)data;

		BEGL_RegisterDriverInterfaces(data);
	}
}

void CorsairGfxEgl::DBPL_UnregisterDirectFBDisplayPlatform(void)
{
	BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)m_dbpl_handle;

	if (data != NULL)
	{
		/* Clear out all of our interface pointers and register the empty ones.
		 * Do this before destroying the interfaces in case the driver needs to use them
		 * to shutdown correctly */
		BEGL_DriverInterfaces nulliface;
		memset(&nulliface, 0, sizeof(BEGL_DriverInterfaces));
		BEGL_GetDefaultDriverInterfaces(&nulliface);
		BEGL_RegisterDriverInterfaces(&nulliface);

		NXPL_DestroyMemInterface(data->memInterface);
		NXPL_DestroyHWInterface(data->hwInterface);
		DBPL_DestroyDisplayInterface(data->displayInterface);

		memset(data, 0, sizeof(BEGL_DriverInterfaces));
		free(data);
	}
}
