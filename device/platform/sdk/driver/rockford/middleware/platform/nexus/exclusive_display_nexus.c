/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
The "exclusive display" is the fastest of all the display options.
It supports a SINGLE native rendering window, which corresponds to the physical
display. Only one native window can be created in this mode, with one 
eglWindowSurface attached. The framebuffers are set one at a time as the
physical display framebuffers. No copying is done.
=============================================================================*/

#include "default_nexus.h"
#include "display_nexus.h"

#include "nexus_platform.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>
#include <assert.h>

static BKNI_MutexHandle countMutex = NULL;
static uint32_t         windowCount = 0; 

bool                    blitCompositeMode = false;

typedef struct  
{
   BufferOnDisplayFunc     bufferOnDisplayFunc;
   NEXUS_DISPLAYHANDLE     display;
   NEXUS_HEAPHANDLE        heap;
   uint32_t                winWidth;
   uint32_t                winHeight;
   uint32_t                winX;
   uint32_t                winY;
   bool                    stretch;
   uint32_t                cb_arg;
   uint32_t                numPosted;     /* Counts the number of posted, pending display buffers (there is only one window) */
   NXPL_DisplayType        displayType;
   BEGL_MemoryInterface    *memInterface;
} NXPL_DisplayData;


/*****************************************************************************
 * Display driver interface
 *****************************************************************************/
static void vsync_callback(void *context, int param)
{
   NXPL_DisplayData        *data = (NXPL_DisplayData*)context;
   uint32_t                 cb_arg;

   cb_arg = data->cb_arg;

/*
   printf("VSync (cb_arg = %d)\n", cb_arg);
*/

   if (data != NULL && data->bufferOnDisplayFunc != NULL)
      data->bufferOnDisplayFunc(cb_arg);

   /* Atomic decrement */
   __sync_fetch_and_sub(&data->numPosted, 1);
}

static BEGL_Error DispBufferDisplay(void *context, BEGL_BufferDisplayState *state)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)state->buffer;
   NXPL_NativeWindowInfo *nw = (NXPL_NativeWindowInfo*)state->windowState.window;
#ifdef BLURAY_PLATFORM
   void *window_handle;
   bdvd_display_get_gfx_window_handle(data->display, &window_handle);
#endif

   /* Atomic increment */
   __sync_fetch_and_add(&data->numPosted, 1);

   if (nw->width != data->winWidth || nw->height != data->winHeight || 
       nw->x != data->winX || nw->y != data->winY ||
       nw->stretch != data->stretch)
   {
      /* Window settings have changed */
      NEXUS_GraphicsSettings graphicsSettings;
      NEXUS_Error err;

      /* setup the display & callback */
#ifndef BLURAY_PLATFORM
      NEXUS_Display_GetGraphicsSettings(data->display, &graphicsSettings);
#else
      bdvd_gfx_window_get(window_handle, &graphicsSettings);
#endif
      graphicsSettings.enabled = true;
      graphicsSettings.position.x = nw->x;
      graphicsSettings.position.y = nw->y;
      if (!nw->stretch)
      {
         graphicsSettings.position.width = nw->width;
         graphicsSettings.position.height = nw->height;
      }
      graphicsSettings.clip.width = nw->width;
      graphicsSettings.clip.height = nw->height;
      graphicsSettings.frameBufferCallback.context = context;
      graphicsSettings.frameBufferCallback.param = 0;
      graphicsSettings.frameBufferCallback.callback = vsync_callback;
#ifndef BLURAY_PLATFORM
      err = NEXUS_Display_SetGraphicsSettings(data->display, &graphicsSettings);
#else
      err = bdvd_gfx_window_set(window_handle, &graphicsSettings);
#endif
      if (err)
         printf("NEXUS_Display_SetGraphicsSettings() failed\n");

      /* update width and height, etc. */
      data->winWidth = nw->width;
      data->winHeight = nw->height;
      data->winX = nw->x;
      data->winY = nw->y;
      data->stretch = nw->stretch;
   }

/*
   printf("SetFB %x (cb_arg = %d)\n", buffer->surface, state->cb_arg);
*/

   data->cb_arg = state->cb_arg;

#if (!((BCHP_CHIP == 35230) || (BCHP_CHIP == 35233)))
   if (data->displayType == NXPL_2D)
   {
#ifndef BLURAY_PLATFORM
      NEXUS_Display_SetGraphicsFramebuffer(data->display, buffer->surface);
#else
      bdvd_gfx_window_set_framebuffer(window_handle, buffer->surface, NULL);
#endif
   }
   else
   {
#ifndef BLURAY_PLATFORM
      NEXUS_GraphicsFramebuffer3D fb3d;
      NEXUS_Graphics_GetDefaultFramebuffer3D(&fb3d);
      
      fb3d.main = buffer->surface;

      if (data->displayType == NXPL_3D_LEFT_RIGHT)
         fb3d.orientation == NEXUS_VideoOrientation_e3D_LeftRight;
      else if (data->displayType == NXPL_3D_OVER_UNDER)
         fb3d.orientation == NEXUS_VideoOrientation_e3D_OverUnder;

      NEXUS_Display_SetGraphicsFramebuffer3D(data->display, &fb3d);
#else
      /* BD doesn't handle 3d yet.  Add support here if required */
      bdvd_gfx_window_set_framebuffer(window_handle, buffer->surface, NULL);
#endif
   }
#else
   NEXUS_Display_SetGraphicsFramebuffer(data->display, buffer->surface);
#endif

   return BEGL_Success;
}

/* Flush pending displays until they are all done, then removes all buffers from display. Will block until complete. */
static BEGL_Error DispWindowUndisplay(void *context, BEGL_WindowState *windowState)
{
   NXPL_DisplayData        *data = (NXPL_DisplayData*)context;
   NXPL_NativeWindowInfo   *nw = (NXPL_NativeWindowInfo*)windowState->window;
   NEXUS_GraphicsSettings  graphicsSettings;
   NEXUS_Error             err;
   uint32_t                i, pending;
   uint32_t                escape = 0;
#ifdef BLURAY_PLATFORM
   void *window_handle;
   bdvd_display_get_gfx_window_handle(data->display, &window_handle);
#endif

   /* Wait for the last posted buffer to be called-back (with a failsafe) */
   while ((pending = __sync_fetch_and_or(&data->numPosted, 0)) != 0)
   {
      if (pending > 1)
      {
         /* This has to be a swapInterval 0 case, in which case we will never get enough vsyncs, so break now */
         break;
      }
      usleep(1);
      escape++;
      if (escape > 1000)   /* Safeguard limit */
         break;
   }

#ifndef BLURAY_PLATFORM
   NEXUS_Display_GetGraphicsSettings(data->display, &graphicsSettings);
#else
   bdvd_gfx_window_get(window_handle, &graphicsSettings);
#endif
   graphicsSettings.enabled = false;
   graphicsSettings.frameBufferCallback.callback = NULL;
#ifndef BLURAY_PLATFORM
   err = NEXUS_Display_SetGraphicsSettings(data->display, &graphicsSettings);
#else
   err = bdvd_gfx_window_set(window_handle, &graphicsSettings);
#endif
   if (err)
      BKNI_Printf("NEXUS_Display_SetGraphicsSettings() failed during platform shutdown\n");

   /* Reset last seen data, so that the next display call will set them correctly */
   data->winWidth = 0;
   data->winHeight = 0;
   data->winX = 0;
   data->winY = 0;
   data->stretch = 0;

   return BEGL_Success;
}

/* Request creation of an appropriate display buffer. Only the 3D driver knows the size and alignment constraints, so the
 * buffer create request must come from the driver. settings->totalByteSize is the size of the memory that the driver needs.
 * We could have just requested a block of memory using the memory interface, but by having the platform layer create a 'buffer'
 * it can actually create whatever type it desires directly, and then only have to deal with that type. For example, in a Nexus
 * platform layer, this function might be implemented to create a NEXUS_Surface (with the correct memory constraints of course).
 * When the buffer handle is passed out during BufferDisplay, the platform layer can simply use it as a NEXUS_Surface. It
 * doesn't have to wrap the memory each time, or perform any lookups. Since the buffer handle is opaque to the 3d driver, the
 * platform layer has complete freedom. */
static BEGL_BufferHandle DispBufferCreate(void *context, BEGL_BufferSettings *settings)
{
   NXPL_DisplayData              *data = (NXPL_DisplayData*)context;
   NXPL_BufferData               *buffer = NULL;
   NEXUS_SurfaceCreateSettings    surfSettings;
   NEXUS_MemoryAllocationSettings memSettings;
   NEXUS_MemoryStatus             memStatus;
   uint32_t                       bpp;

   buffer = (NXPL_BufferData*)malloc(sizeof(NXPL_BufferData));
   memset(buffer, 0, sizeof(NXPL_BufferData));

   NEXUS_Surface_GetDefaultCreateSettings(&surfSettings);

   switch (settings->format)
   {
   case BEGL_BufferFormat_eA8B8G8R8 :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eA8_B8_G8_R8;
      break;
   case BEGL_BufferFormat_eR8G8B8A8 :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
      break;
   case BEGL_BufferFormat_eX8B8G8R8 :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eX8_B8_G8_R8;
      break;
   case BEGL_BufferFormat_eR8G8B8X8 :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_X8;
      break;
   case BEGL_BufferFormat_eR5G6B5 :
   case BEGL_BufferFormat_eR5G6B5_Texture :
      bpp = 16; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eR5_G6_B5;
      break;
   case BEGL_BufferFormat_eYUV422_Texture :
      bpp = 16; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;
      break;
   case BEGL_BufferFormat_eVUY224_Texture :
      bpp = 16; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8;
      break;
   case BEGL_BufferFormat_eA8B8G8R8_Texture :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eA8_B8_G8_R8;
      break;
   case BEGL_BufferFormat_eA8R8G8B8_Texture :
      bpp = 32;
      surfSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
      break;
   case BEGL_BufferFormat_eR8G8B8A8_Texture :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
      break;
   case BEGL_BufferFormat_eB8G8R8A8_Texture :
      bpp = 32;
      surfSettings.pixelFormat = NEXUS_PixelFormat_eB8_G8_R8_A8;
      break;
   }

   surfSettings.width = settings->width;
   surfSettings.height = settings->height;
   surfSettings.alignment = settings->alignment;
   surfSettings.pitch = settings->pitchBytes;
   surfSettings.heap = data->heap;

   NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
   memSettings.alignment = settings->alignment;
   memSettings.heap = data->heap;

   // Surface must be created in our heap, and always with a cached address
   NEXUS_Memory_Allocate(settings->totalByteSize, &memSettings, &surfSettings.pMemory);
   if (surfSettings.pMemory != NULL)
      buffer->surface = NEXUS_Surface_Create(&surfSettings);

   // Note: these 3 lines are very deliberate, it means that FORCE_UNCACHED works correctly.
   // Be very careful if you change them. Think about uncached mode!
   NEXUS_Heap_GetStatus(data->heap, &memStatus);
   settings->physOffset = memStatus.offset + ((uintptr_t)surfSettings.pMemory - (uintptr_t)memStatus.addr);
   settings->cachedAddr = data->memInterface->ConvertPhysicalToCached(data->memInterface->context, settings->physOffset);

   buffer->settings = *settings;

/*
   printf("DispBufferCreate %x\n", buffer->surface);
*/

   return (BEGL_BufferHandle)buffer;
}

/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferDisplayState *bufferState)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)bufferState->buffer;
   
/*
   printf("DispBufferDestroy %x\n", buffer->surface);
*/

   if (buffer != NULL)
   {
      NEXUS_SurfaceCreateSettings createSettings;

      if (buffer->surface)
      {
         NEXUS_Surface_GetCreateSettings(buffer->surface, &createSettings);
         NEXUS_Surface_Destroy(buffer->surface);

         // We do this using the create settings to ensure it's a cached ptr, even in FORCE_UNCACHED
         NEXUS_Memory_Free(createSettings.pMemory);
      }
      memset(buffer, 0, sizeof(NXPL_BufferData));
      free(buffer);
   }

   return BEGL_Success;
}

/* Get information about a created window buffer */
static BEGL_Error DispBufferGetCreateSettings(void *context, BEGL_BufferHandle bufHandle, BEGL_BufferSettings *settings)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)bufHandle;

   if (buffer != NULL)
   {
      *settings = buffer->settings;
      return BEGL_Success;
   }

   return BEGL_Fail;
}

/* Called to determine current size of the window referenced by the opaque window handle.
 * This is needed by EGL in order to know the size of a native 'window'. */
static BEGL_Error DispWindowGetInfo(void *context,
                                    BEGL_WindowHandle window,
                                    BEGL_WindowInfoFlags flags,
                                    BEGL_WindowInfo *info)
{
   NXPL_NativeWindowInfo *nw = (NXPL_NativeWindowInfo*)window;

   if (nw != NULL)
   {
      info->width = nw->width;
      info->height = nw->height;
      info->swapchain_count = 0;    /* just take the default from egl at the moment */
      return BEGL_Success;
   }

   return BEGL_Fail;
}

#ifdef BLURAY_PLATFORM
static BEGL_Error DispGetNativeFormat(void *context, BEGL_BufferFormat bufferFormat, unsigned int *nativeFormat)
{
   if (nativeFormat != NULL)
   {
      switch(bufferFormat)
      {
         case BEGL_BufferFormat_eX8B8G8R8:
         case BEGL_BufferFormat_eA8B8G8R8:
            *nativeFormat = 0x21;            /* this may not be correct */
         break;
         case BEGL_BufferFormat_eR5G6B5:
            BKNI_Printf("%s: unknown X visual id value for RGB565\n", __FUNCTION__);
            *nativeFormat = 0;
         break;
         default:
            BKNI_Printf("%s: unknown BEGL_BufferFormat (%d)\n", __FUNCTION__, bufferFormat);
            return BEGL_Fail;
         break;
      }

      return BEGL_Success;
   }

   return BEGL_Fail;
}
#endif

BEGL_DisplayInterface *NXPL_CreateDisplayInterface(BEGL_MemoryInterface *memIface, 
                                                   NEXUS_DISPLAYHANDLE display, BEGL_DisplayCallbacks *displayCallbacks)
{
   NXPL_DisplayData *data;
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));

   if (disp != NULL)
   {
      BKNI_CreateMutex(&countMutex);

      data = (NXPL_DisplayData*)malloc(sizeof(NXPL_DisplayData));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));
      
      if (data != NULL)
      {
         NEXUS_MemoryStatus            memStatus;
         int topTwoBitsHeapStart;
         int topTwoBitsHeapEnd;

         memset(data, 0, sizeof(NXPL_DisplayData));

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->WindowUndisplay = DispWindowUndisplay;
         disp->WindowGetInfo = DispWindowGetInfo;
#ifdef BLURAY_PLATFORM
         disp->GetNativeFormat = DispGetNativeFormat;
#endif /* BLURAY_PLATFORM */

         data->memInterface = memIface;
         data->bufferOnDisplayFunc = displayCallbacks->BufferOnDisplay;
         data->display = display;

         /* Store the heap settings */
         data->heap = NXPL_MemHeap(memIface);
         
         NEXUS_Heap_GetStatus(data->heap, &memStatus);
         topTwoBitsHeapStart = memStatus.offset >> 30;
         topTwoBitsHeapEnd = (memStatus.offset + memStatus.size - 1) >> 30;
         if (topTwoBitsHeapStart != topTwoBitsHeapEnd)
         {
            printf("\n\nNXPL : NXPL_CreateDisplayInterface() ERROR.\nThe Heap you have selected in your platform layer straddles a 1GB boundary\n"
                   "Start %p, Size %p\n", memStatus.offset, memStatus.size);
            goto error1;
         }
      }
      else
      {
         goto error0;
      }
   }
   return disp;

error1:
   free(data);

error0:
   free(disp);
   return NULL;
}

void NXPL_SetDisplayType(NXPL_PlatformHandle handle, NXPL_DisplayType type)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL && data->displayInterface != NULL && data->displayInterface->context != NULL)
   {
      NXPL_DisplayData *dd = (NXPL_DisplayData*)data->displayInterface->context;
      dd->displayType = type;
   }
}

void NXPL_DestroyDisplayInterface(BEGL_DisplayInterface *mem)
{
   if (mem != NULL)
   {
      if (countMutex)
         BKNI_DestroyMutex(countMutex);

      if (mem->context != NULL)
         free(mem->context);

      memset(mem, 0, sizeof(BEGL_DisplayInterface));
      free(mem);
   }
}

bool NXPL_BufferGetRequirements(NXPL_PlatformHandle handle,
                                BEGL_PixmapInfo *bufferRequirements,
                                BEGL_BufferSettings * bufferConstrainedRequirements)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL && data->displayCallbacks.BufferGetRequirements != NULL)
   {
      data->displayCallbacks.BufferGetRequirements(bufferRequirements, bufferConstrainedRequirements);
      return true;
   }

   return false;
}

bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle handle, void **pixmapHandle, NEXUS_SURFACEHANDLE *surface, BEGL_PixmapInfo *info)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL && data->displayCallbacks.PixmapCreateCompatiblePixmap != NULL)
   {
      BEGL_BufferHandle buffer = data->displayCallbacks.PixmapCreateCompatiblePixmap(info);
      if (buffer != NULL)
      {
         *pixmapHandle = (void*)buffer;
         *surface = ((NXPL_BufferData*)buffer)->surface;
         return true;
      }
   }

   return false;
}

void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle handle, void *pixmapHandle)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL &&
       data->displayInterface != NULL &&
       data->displayInterface->BufferDestroy != NULL)
   {
      BEGL_BufferDisplayState bufferState;
      memset(&bufferState, 0, sizeof(BEGL_BufferDisplayState));
      bufferState.buffer = (BEGL_BufferHandle)pixmapHandle;
      bufferState.cb_arg = -1;

      data->displayInterface->BufferDestroy(data->displayInterface->context, &bufferState);
   }
}

/* Exclusive display mode only allows ONE window (unless you happen to be using the blit composite mode
   by calling NXPL_CreateCompositedDisplayInterface) */
void *NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info)
{
   if (!blitCompositeMode)
   {
   assert(countMutex != NULL);

   BKNI_AcquireMutex(countMutex);
      if (windowCount > 0)
   {
      BKNI_ReleaseMutex(countMutex);
      return NULL;
   }
   else
   {
      windowCount++;
      BKNI_ReleaseMutex(countMutex);
   }
   }

   NXPL_NativeWindowInfo *nw = (NXPL_NativeWindowInfo*)malloc(sizeof(NXPL_NativeWindowInfo));
   memset(nw, 0, sizeof(NXPL_NativeWindowInfo));
   if (nw != NULL && info != NULL)
      *nw = *info;

   return nw;
}

void NXPL_UpdateNativeWindow(void *native, const NXPL_NativeWindowInfo *info)
{
   if (info != NULL && native != NULL)
      *((NXPL_NativeWindowInfo*)native) = *info;
}

void NXPL_DestroyNativeWindow(void *nativeWin)
{
   if (nativeWin != NULL)
   {
      NXPL_NativeWindowInfo *nw = (NXPL_NativeWindowInfo*)nativeWin;
      memset(nw, 0, sizeof(NXPL_NativeWindowInfo));
      free(nw);
      
      if (!blitCompositeMode)
      {
      assert(countMutex != NULL);

      BKNI_AcquireMutex(countMutex);
      windowCount--;
      BKNI_ReleaseMutex(countMutex);
      }
   }
}


