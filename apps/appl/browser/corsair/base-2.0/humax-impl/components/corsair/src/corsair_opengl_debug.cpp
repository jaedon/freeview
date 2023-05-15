/** **********************************************************************************************************
	@file 		corsair_opengl_debug.h
	
	@date		2013/11/28
	@author		STB-Component(tstbcomp@humaxdigital.com)
	@breif		
	
	(c) 2011-2014 Humax Co., Ltd. 
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

#include "corsair_opengl_debug.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

bool opengl_gl_test(const char *pszLocation)
{
	while (1)
	{
		GLint err = glGetError();
		if ( err == GL_NO_ERROR ) return true;

		LOG_ERR("OpenGL GL ERROR: %s\n",pszLocation);
		switch ( err )
		{
			case GL_INVALID_ENUM:
				LOG_ERR("GL_INVALID_ENUM \n");
				break;
			case GL_INVALID_VALUE:
				LOG_ERR("GL_INVALID_VALUE \n");
				break;
			case GL_INVALID_OPERATION:
				LOG_ERR("GL_INVALID_OPERATION \n");
				break;
			case GL_OUT_OF_MEMORY:
				LOG_ERR("GL_OUT_OF_MEMORY \n");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				LOG_ERR("GL_INVALID_FRAMEBUFFER_OPERATION \n");
				break;
			default:
				LOG_ERR("GL_UNKNOWN_ERROR \n");
				break;
		}
	}
	return false;
}

bool opengl_egl_test(const char *pszLocation)
{
    EGLint err = eglGetError();
    if ( err == EGL_SUCCESS) return true;

	LOG_ERR("OpenGL EGL ERROR: %s\n",pszLocation);

	switch ( err )
	{
	case EGL_NOT_INITIALIZED:
		LOG_ERR("EGL_NOT_INITIALIZED \n");
		break;
	case EGL_BAD_ACCESS:
		LOG_ERR("EGL_BAD_ACCESS \n");
		break;
	case EGL_BAD_ALLOC:
		LOG_ERR("EGL_BAD_ALLOC \n");
		break;
	case EGL_BAD_ATTRIBUTE:
		LOG_ERR("EGL_BAD_ATTRIBUTE \n");
		break;
	case EGL_BAD_CONFIG:
		LOG_ERR("EGL_BAD_CONFIG \n");
		break;
	case EGL_BAD_CONTEXT:
		LOG_ERR("EGL_BAD_CONTEXT \n");
		break;
	case EGL_BAD_CURRENT_SURFACE:
		LOG_ERR("EGL_BAD_CURRENT_SURFACE \n");
		break;
	case EGL_BAD_DISPLAY:
		LOG_ERR("EGL_BAD_DISPLAY \n");
		break;
	case EGL_BAD_MATCH:
		LOG_ERR("EGL_BAD_MATCH \n");
		break;
	case EGL_BAD_NATIVE_PIXMAP:
		LOG_ERR("EGL_BAD_NATIVE_PIXMAP \n");
		break;
	case EGL_BAD_NATIVE_WINDOW:
		LOG_ERR("EGL_BAD_NATIVE_WINDOW \n");
		break;
	case EGL_BAD_PARAMETER:
		LOG_ERR("EGL_BAD_PARAMETER \n");
		break;
	case EGL_BAD_SURFACE:
		LOG_ERR("EGL_BAD_SURFACE \n");
		break;
	case EGL_CONTEXT_LOST:
		LOG_ERR("EGL_CONTEXT_LOST \n");
		break;
	default:
		LOG_ERR("EGL_UNKNOWN_ERROR \n");
	}   

	return false;
}
