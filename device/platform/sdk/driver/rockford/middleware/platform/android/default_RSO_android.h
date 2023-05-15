/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default RSO Android platform API for EGL driver
Module   :  RSO Android platform on NEXUS

FILE DESCRIPTION
DESC
=============================================================================*/

#ifndef _RSOANPL_DEFAULT_RSOANDROID_H__
#define _RSOANPL_DEFAULT_RSOANDROID_H__

#include <EGL/egl.h>
#include <cutils/log.h>

#include "egl/android_natives.h"
#include "android_native_buffer.h"
#include <gralloc_priv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *RSOANPL_PlatformHandle;

/* Register Android with the egl platform */
extern void RSOANPL_RegisterAndroidDisplayPlatform(RSOANPL_PlatformHandle *handle, android_native_window_t *awin);

/* Unregister Android */
extern void RSOANPL_UnregisterAndroidDisplayPlatform(RSOANPL_PlatformHandle handle);

extern bool RSOANPL_BufferGetRequirements(RSOANPL_PlatformHandle handle, BEGL_PixmapInfo *bufferRequirements, BEGL_BufferSettings * bufferConstrainedRequirements);

#ifdef __cplusplus
}
#endif

#endif /* _RSOANPL_DEFAULT_RSOANDROID_H__ */
