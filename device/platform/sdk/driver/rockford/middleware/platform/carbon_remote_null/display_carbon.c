/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_carbon.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>

typedef struct  
{
   BufferOnDisplayFunc     bufferOnDisplayFunc;
   uint32_t                winWidth;
   uint32_t                winHeight;
   uint32_t                winX;
   uint32_t                winY;
   bool                    stretch;
} CARPL_DisplayData;

typedef struct
{
   BEGL_BufferSettings     settings;
   void                    *memory;
} CARPL_BufferData;

static BEGL_Error DispBufferDisplay(void *context, BEGL_BufferDisplayState *state)
{
   CARPL_DisplayData *data = (CARPL_DisplayData*)context;
   CARPL_NativeWindowInfo *nw = (CARPL_NativeWindowInfo*)state->window;

   if (nw->width != data->winWidth || nw->height != data->winHeight || 
       nw->x != data->winX || nw->y != data->winY ||
       nw->stretch != data->stretch)
   {
      /* Window settings have changed */

      /* update width and height, etc. */
      data->winWidth = nw->width;
      data->winHeight = nw->height;
      data->winX = nw->x;
      data->winY = nw->y;
      data->stretch = nw->stretch;
   }

   data->bufferOnDisplayFunc(state->cb_arg);

   return BEGL_Success;
}

/* Flush pending displays until they are all done, then removes all buffers from display. Will block until complete. */
static BEGL_Error DispBufferDisplayFinish(void *context)
{
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
   CARPL_DisplayData              *data = (CARPL_DisplayData*)context;
   CARPL_BufferData               *buffer = NULL;
   uint32_t                       bpp;
   uint32_t                       numBytes, align;

   buffer = (CARPL_BufferData*)malloc(sizeof(CARPL_BufferData));
   memset(buffer, 0, sizeof(CARPL_BufferData));

   switch (settings->format)
   {
   case BEGL_BufferFormat_eA8B8G8R8 : 
      bpp = 32; 
      break;
   case BEGL_BufferFormat_eX8B8G8R8 : 
      bpp = 32; 
      break;
   case BEGL_BufferFormat_eR5G6B5 :   
      bpp = 16; 
      break;
   }

   numBytes = settings->totalByteSize;
   align = settings->alignment;

   /* make alignment on 4k boundary */
   if (align < 4096) 
      align = 4096;
   numBytes = (numBytes + (4096 - 1)) & ~(4096 -1);

   buffer->memory = memalign(align, numBytes);

   settings->cachedAddr = buffer->memory;
   settings->physOffset = (uint32_t)buffer->memory;

   buffer->settings = *settings;

   return (BEGL_BufferHandle)buffer;
}

/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferHandle bufHandle)
{
   CARPL_DisplayData *data = (CARPL_DisplayData*)context;
   CARPL_BufferData *buffer = (CARPL_BufferData*)bufHandle;

   if (buffer != NULL)
   {
      if (buffer->memory)
         free(buffer->memory);

      memset(buffer, 0, sizeof(CARPL_BufferData));
      free(buffer);
   }

   return BEGL_Success;
}

/* Get information about a created window buffer */
static BEGL_Error DispBufferGetCreateSettings(void *context, BEGL_BufferHandle bufHandle, BEGL_BufferSettings *settings)
{
   CARPL_DisplayData *data = (CARPL_DisplayData*)context;
   CARPL_BufferData *buffer = (CARPL_BufferData*)bufHandle;

   if (buffer != NULL)
   {
      *settings = buffer->settings;
      return BEGL_Success;
   }

   return BEGL_Fail;
}

/* Called to determine current size of the window referenced by the opaque window handle.
 * This is needed by EGL in order to know the size of a native 'window'. */
static BEGL_Error DispWindowGetInfo(void *context, BEGL_WindowHandle window, BEGL_WindowInfo *info)
{
   CARPL_NativeWindowInfo *nw = (CARPL_NativeWindowInfo*)window;

   if (nw != NULL)
   {
      info->width = nw->width;
      info->height = nw->height;
      return BEGL_Success;
   }

   return BEGL_Fail;
}

BEGL_DisplayInterface *CARPL_CreateDisplayInterface(BEGL_DisplayCallbacks *displayCallbacks)
{
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));
   if (disp != NULL)
   {
      CARPL_DisplayData *data = (CARPL_DisplayData*)malloc(sizeof(CARPL_DisplayData));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));

      if (data != NULL)
      {
         memset(data, 0, sizeof(CARPL_DisplayData));

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->BufferDisplayFinish = DispBufferDisplayFinish;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->WindowGetInfo = DispWindowGetInfo;

         data->bufferOnDisplayFunc = displayCallbacks->BufferOnDisplay;
      }
   }
   return disp;
}

void CARPL_DestroyDisplayInterface(BEGL_DisplayInterface *mem)
{
   if (mem != NULL)
   {
      if (mem->context != NULL)
         free(mem->context);

      memset(mem, 0, sizeof(BEGL_DisplayInterface));
      free(mem);
   }
}

/*
bool CARPL_CreateCompatiblePixmap(CARPL_PlatformHandle handle, void **pixmapHandle, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfo *info)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL && data->displayCallbacks.PixmapCreateCompatiblePixmap != NULL)
   {
      BEGL_BufferHandle buffer = data->displayCallbacks.PixmapCreateCompatiblePixmap(info);
      if (buffer != NULL)
      {
         *pixmapHandle = (void*)buffer;
         *surface = ((CARPL_BufferData*)buffer)->surface;
         return true;
      }
   }

   return false;
}

void CARPL_DestroyCompatiblePixmap(CARPL_PlatformHandle handle, void *pixmapHandle)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL && data->displayInterface != NULL)
      data->displayInterface->BufferDestroy(data->displayInterface->context, 
      (BEGL_BufferHandle)pixmapHandle);
}
*/

void *CARPL_CreateNativeWindow(const CARPL_NativeWindowInfo *info)
{
   CARPL_NativeWindowInfo *nw = (CARPL_NativeWindowInfo*)malloc(sizeof(CARPL_NativeWindowInfo));
   memset(nw, 0, sizeof(CARPL_NativeWindowInfo));
   if (nw != NULL && info != NULL)
      *nw = *info;

   return nw;
}

void CARPL_UpdateNativeWindow(void *native, const CARPL_NativeWindowInfo *info)
{
   if (info != NULL && native != NULL)
      *((CARPL_NativeWindowInfo*)native) = *info;
}

void CARPL_DestroyNativeWindow(void *nativeWin)
{
   if (nativeWin != NULL)
   {
      CARPL_NativeWindowInfo *nw = (CARPL_NativeWindowInfo*)nativeWin;
      memset(nw, 0, sizeof(CARPL_NativeWindowInfo));
      free(nw);
   }
}


