/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
This is a default implementation of a Nexus platform layer used by V3D.
This illustrates one way in which the abstract display interface might be
implemented. You can replace this with your own custom version if preferred.
=============================================================================*/

#include "default_nexus.h"
#include "display_nexus.h"

#include "nexus_platform.h"

#include <malloc.h>
#include <memory.h>

typedef struct  
{
   NXPL_CompositingData    compData;
   NEXUS_HeapHandle        heap;
   uint32_t                winWidth;
   uint32_t                winHeight;
   uint32_t                winX;
   uint32_t                winY;
   bool                    stretch;
   BufferOnDisplayFunc     bufOnDispFunc;
   BEGL_MemoryInterface    *memInterface;
} NXPL_CompDisplayData;

/* There will be one NXPL_WindowState for each window */
typedef struct
{
   uint32_t                last_posted_cb_arg;
   uint32_t                last_received_cb_arg;
} NXPL_WindowState;

typedef struct
{
   NXPL_CompDisplayData    *data;
   uint32_t                cb_arg;
   NXPL_WindowState        *window_state;
} NXPL_CompPostContext;

/*****************************************************************************
 * Display driver interface
 *****************************************************************************/
static BEGL_Error DispBufferDisplay(void *compContext, BEGL_BufferDisplayState *state)
{
   NXPL_CompDisplayData *data = (NXPL_CompDisplayData*)compContext;
   NXPL_BufferData *buffer = (NXPL_BufferData*)state->buffer;
   NXPL_WindowState *windowState = (NXPL_WindowState*)state->windowState.platformState;

   NXPL_CompPostContext *context = (NXPL_CompPostContext*)malloc(sizeof(NXPL_CompPostContext));
   memset(context, 0, sizeof(NXPL_CompPostContext));

   windowState->last_posted_cb_arg = state->cb_arg;

   context->window_state = windowState;
   context->data = data;
   context->cb_arg = state->cb_arg;

   /*printf("Display %p, win = %p, cb_arg = %d\n", buffer->surface, state->windowState.window, state->cb_arg);*/

   if (data->compData.bufferBlit)
      data->compData.bufferBlit(context, buffer->surface, state->windowState.window);

   return BEGL_Success;
}

static void BufferOnDisplayCallback(void *context)
{
   NXPL_CompPostContext *compPost = (NXPL_CompPostContext*)context;

   /*printf("BufferOnDisplayCallback %d, winstate = %p\n", compPost->cb_arg, compPost->window_state);*/

   if (compPost && compPost->data && compPost->data->bufOnDispFunc)
      compPost->data->bufOnDispFunc(compPost->cb_arg);

   compPost->window_state->last_received_cb_arg = compPost->cb_arg;

   if (compPost)
      free(compPost);
}

/* Flush pending displays until they are all done, then removes all buffers from display. Will block until complete. */
static BEGL_Error DispWindowUndisplay(void *context, BEGL_WindowState *windowState)
{
   NXPL_CompDisplayData *data = (NXPL_CompDisplayData*)context;
   NXPL_WindowState     *state = (NXPL_WindowState*)windowState->platformState;
   uint32_t             i;

   /*printf("DispWindowUndisplay win=%p\n", windowState->window);*/

   if (data->compData.windowUndisplay != NULL)
      data->compData.windowUndisplay(context, windowState->window);

   /* Wait for the last posted buffer to be called-back (with a failsafe) */
   for (i = 0; i < 1000 && state->last_posted_cb_arg != state->last_received_cb_arg; i++)
      usleep(1);

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
   NXPL_CompDisplayData          *data = (NXPL_CompDisplayData*)context;
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
   case BEGL_BufferFormat_eR8G8B8A8_Texture :
      bpp = 32; 
      surfSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
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

   return (BEGL_BufferHandle)buffer;
}

/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferDisplayState *bufferState)
{
   NXPL_CompDisplayData *data = (NXPL_CompDisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)bufferState->buffer;

   /*printf("DispBufferDestroy buffer=%p, win=%p\n", bufferState->buffer, bufferState->windowState.window);*/

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
   NXPL_CompDisplayData *data = (NXPL_CompDisplayData*)context;
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
      info->swapchain_count = 0;    /* Let GL make our swap-chain */
      return BEGL_Success;
   }

   return BEGL_Fail;
}

static void *DispWindowStateCreate(void *context, BEGL_WindowHandle window)
{
   NXPL_WindowState *windowState = (NXPL_WindowState *)malloc(sizeof(NXPL_WindowState));
   memset(windowState, 0, sizeof(NXPL_WindowState));

   return (void*)windowState;
}

static BEGL_Error DispWindowStateDestroy(void *state)
{
   NXPL_WindowState *windowState = (NXPL_WindowState *)state;

   if (windowState)
   {
      memset(windowState, 0, sizeof(NXPL_WindowState));
      free(windowState);
      return BEGL_Success;
   }

   return BEGL_Fail;
}

extern bool blitCompositeMode;

BEGL_DisplayInterface *NXPL_CreateCompositedDisplayInterface(BEGL_MemoryInterface *memIface, 
                                                             NXPL_CompositingData *compData, 
                                                             BEGL_DisplayCallbacks *displayCallbacks)
{
   NXPL_CompDisplayData *data;
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));

   compData->bufferOnDisplay = NULL;

   if (disp != NULL)
   {
      blitCompositeMode = true;

      data = (NXPL_CompDisplayData*)malloc(sizeof(NXPL_CompDisplayData));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));

      if (data != NULL)
      {
         NEXUS_MemoryStatus            memStatus;
         int topTwoBitsHeapStart;
         int topTwoBitsHeapEnd;

         memset(data, 0, sizeof(NXPL_CompDisplayData));

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->WindowUndisplay = DispWindowUndisplay;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->WindowGetInfo = DispWindowGetInfo;
         disp->WindowPlatformStateCreate = DispWindowStateCreate;
         disp->WindowPlatformStateDestroy = DispWindowStateDestroy;

         compData->bufferOnDisplay = BufferOnDisplayCallback;

         data->memInterface = memIface;
         data->compData = *compData;
         data->bufOnDispFunc = displayCallbacks->BufferOnDisplay;

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

void NXPL_DestroyCompositedDisplayInterface(BEGL_DisplayInterface *mem)
{
   if (mem != NULL)
   {
      if (mem->context != NULL)
         free(mem->context);

      memset(mem, 0, sizeof(BEGL_DisplayInterface));
      free(mem);
   }
}

