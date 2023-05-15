/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#ifndef _NXPL_DEFAULT_NEXUS_H__
#define _NXPL_DEFAULT_NEXUS_H__

#ifdef NULL_DISPLAY

#define NEXUS_SURFACEHANDLE void *
#define NEXUS_DISPLAYHANDLE void *
#define NEXUS_HEAPHANDLE    void *

#else

#ifdef SINGLE_PROCESS

#ifndef BLURAY_PLATFORM
#include "nexus_display.h"
#else
#include "bdvd.h"
#endif
#include "nexus_surface.h"
#include "nexus_memory.h"
#define NEXUS_SURFACEHANDLE NEXUS_SurfaceHandle
#ifndef BLURAY_PLATFORM
#define NEXUS_DISPLAYHANDLE NEXUS_DisplayHandle
#else
#define NEXUS_DISPLAYHANDLE bdvd_display_h
#endif
#define NEXUS_HEAPHANDLE    NEXUS_HeapHandle

#else /* Multi-process client */

#include "nexus_platform_client.h"
#include "nexus_surface_client.h"
#include "nexus_surface.h"
#include "nexus_memory.h"
#define NEXUS_SURFACEHANDLE NEXUS_SurfaceHandle
#define NEXUS_DISPLAYHANDLE void *
#define NEXUS_HEAPHANDLE    NEXUS_HeapHandle

#endif

#endif

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *NXPL_PlatformHandle;

typedef struct
{
   uint32_t    width;
   uint32_t    height;
   uint32_t    x;
   uint32_t    y;
   bool        stretch;
   uint32_t    clientID;
} NXPL_NativeWindowInfo;

/* Compositing mode types */
typedef void (*CompBufferBlitFunc)(void *context, NEXUS_SURFACEHANDLE surface, void *window);
typedef void (*CompBufferOnDisplayFunc)(void *context);
typedef void (*CompWindowUndisplayFunc)(void *context, void *window);

typedef struct
{
   CompBufferBlitFunc      bufferBlit;
   CompBufferOnDisplayFunc bufferOnDisplay;
   CompWindowUndisplayFunc windowUndisplay;
} NXPL_CompositingData;
/* End compositing mode types */


/* Register a display for exclusive use. The client application should not use the display until 
 * calling NXPL_UnregisterNexusDisplayPlatform. 
 * It will register its own memory, h/w and display APIs suitable for basic exclusive mode rendering on 
 * a Nexus display.
 * Fills in the NXPL platform handle, which must be passed back into the unregister function.
 */
extern void NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle *handle, NEXUS_DISPLAYHANDLE display);

/* Unregister a display for exclusive use. The client application can the use the Nexus display again. */
extern void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle handle);

/* Register for composited display use.
 * You must fill compData->bufferBlit with your function pointer to be called when a buffer is ready for display.
 * The compData->bufferOnDisplay function pointer will be filled in for you to use to signal when presented
 * buffers reach the display. 
 * Fills in the NXPL platform handle, which must be passed back into the unregister function.
 */
extern void NXPL_RegisterNexusCompositedDisplayPlatform(NXPL_PlatformHandle *handle, NXPL_CompositingData *compData);


extern void NXPL_UnregisterNexusCompositedDisplayPlatform(NXPL_PlatformHandle handle);

extern bool NXPL_BufferGetRequirements(NXPL_PlatformHandle handle, BEGL_PixmapInfo *bufferRequirements, BEGL_BufferSettings * bufferConstrainedRequirements);

/* Create a 'native window' of the given size. This is really just a small structure that holds the size
 * of the window that EGL will write into. */
extern void *NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info);

/* Update the 'native window' with new settings */
extern void NXPL_UpdateNativeWindow(void *native, const NXPL_NativeWindowInfo *info);

/* Destroy a 'native window' */
extern void NXPL_DestroyNativeWindow(void *nativeWin);

extern bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle handle, void **pixmapHandle, NEXUS_SURFACEHANDLE *surface, BEGL_PixmapInfo *info);

extern void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle handle, void *pixmapHandle);

#ifndef SINGLE_PROCESS
/* only valid for surface compositor */
/* gets the associated video pip window from the graphics window. */
NEXUS_SurfaceClientHandle NXPL_CreateVideoWindowClient(void *native, unsigned windowId);

/* releases the pip back */
void NXPL_ReleaseVideoWindowClient(NEXUS_SurfaceClientHandle handle);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NXPL_DEFAULT_NEXUS_H__ */
