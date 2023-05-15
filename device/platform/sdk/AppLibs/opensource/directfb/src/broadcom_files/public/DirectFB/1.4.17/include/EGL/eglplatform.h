/*
 * Copyright (c) 2008  The DirectFB Organization (directfb.org)
 *
 * All rights reserved.
 *
 * Written by Claudio Ciccani <klan@directfb.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library in the file COPYING;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __EGLPLATFORM_H__
#define __EGLPLATFORM_H__

/* Windows calling convention boilerplate */
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
# define WIN32_LEAN_AND_MEAN 1
# include <windows.h>
#endif

#include <sys/types.h>
#include <stdint.h>

/* Macros used in EGL function prototype declarations.
 *
 * EGLAPI return-type EGLAPIENTRY eglFunction(arguments);
 * typedef return-type (EXPAPIENTRYP PFNEGLFUNCTIONPROC) (arguments);
 *
 * On Windows, EGLAPIENTRY can be defined like APIENTRY.
 * On most other platforms, it should be empty.
 */

#ifndef EGLAPIENTRY
# define EGLAPIENTRY
#endif
#ifndef EGLAPIENTRYP
# define EGLAPIENTRYP EGLAPIENTRY *
#endif
#ifndef EGLAPI
# define EGLAPI extern
#endif

#ifdef AC_EGL

typedef void* EGLNativeDisplayType;
typedef void* EGLNativePixmapType;
typedef void* EGLNativeWindowType;

#else

#include <directfb.h>

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef IDirectFB*        EGLNativeDisplayType;
typedef IDirectFBSurface* EGLNativePixmapType;
typedef IDirectFBWindow*  EGLNativeWindowType;

#endif

#endif /* __eglplatform_h */

