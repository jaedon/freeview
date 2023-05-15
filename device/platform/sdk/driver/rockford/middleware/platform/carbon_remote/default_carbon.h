/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Win32 platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#ifndef _CARPL_DEFAULT_NEXUS_H__
#define _CARPL_DEFAULT_NEXUS_H__

#include "nexus_display.h"
#include "nexus_surface.h"

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*BufferOnDisplayFunc)(BEGL_BufferDisplayState *state);

typedef void *NXPL_PlatformHandle;

typedef struct
{
   uint32_t    width;
   uint32_t    height;
   uint32_t    x;
   uint32_t    y;
   bool        stretch;
} NXPL_NativeWindowInfo;

/* Register a display for exclusive use. The client application should not use the display until 
 * calling NXPL_UnregisterNexusDisplayPlatform. 
 * It will register its own memory, h/w and display APIs suitable for basic exclusive mode rendering on 
 * a Nexus display.
 * Fills in the NXPL platform handle, which must be passed back into the unregister function.
 */
extern void CARPL_RegisterDisplayPlatform(NXPL_PlatformHandle *handle, NEXUS_DisplayHandle display);

/* Unregister a display for exclusive use. The client application can the use the Nexus display again. */
extern void CARPL_UnregisterDisplayPlatform(NXPL_PlatformHandle handle);

/* Create a 'native window' of the given size. This is really just a small structure that holds the size
 * of the window that EGL will write into. */
extern void *NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info);

/* Update the 'native window' with new settings */
extern void NXPL_UpdateNativeWindow(void *native, const NXPL_NativeWindowInfo *info);

/* Destroy a 'native window' */
extern void NXPL_DestroyNativeWindow(void *nativeWin);

extern bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle handle, void **pixmapHandle, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfo *info);

extern void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle handle, void *pixmapHandle);

#ifdef __cplusplus
}
#endif

#endif /* _CARPL_DEFAULT_NEXUS_H__ */

