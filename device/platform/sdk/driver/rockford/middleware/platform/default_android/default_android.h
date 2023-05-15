/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Android platform API for EGL driver
Module   :  Android platform on SurfaceFlinger

FILE DESCRIPTION
DESC
=============================================================================*/

#ifndef _ANPL_DEFAULT_ANDROID_H__
#define _ANPL_DEFAULT_ANDROID_H__

#include <EGL/egl.h>

#include "egl/android_natives.h"
#include "android_native_buffer.h"
#include <gralloc_priv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *ANPL_PlatformHandle;

/* Register ANPL with the egl platform */
extern void ANPL_RegisterAndroidDisplayPlatform(ANPL_PlatformHandle *handle, android_native_window_t *awin);

/* Unregister ANPL */
extern void ANPL_UnregisterAndroidDisplayPlatform(ANPL_PlatformHandle handle);

extern bool ANPL_BufferGetRequirements(ANPL_PlatformHandle handle, BEGL_PixmapInfo *bufferRequirements, BEGL_BufferSettings * bufferConstrainedRequirements);

#ifdef __cplusplus
}
#endif

#endif /* _ANPL_DEFAULT_ANDROID_H__ */
