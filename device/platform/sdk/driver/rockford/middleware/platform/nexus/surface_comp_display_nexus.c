/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_nexus.h"
#include "display_nexus.h"

#include "nexus_platform.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>

#define NUM_SWAP_BUFFERS 3

/* There will be one NXPL_WindowState for each NXPL_SurfCompNativeWindow */
typedef struct
{
   uint32_t                   numPosted;     /* Counts the number of posted, pending display buffers for this swap chain */
   NEXUS_SurfaceClientHandle  surfaceClient;
   NXPL_BufferData            *buffers[NUM_SWAP_BUFFERS];
   bool                       hasBeenDisplayed;
} NXPL_WindowState;

typedef struct
{
   uint32_t                   width;
   uint32_t                   height;
   uint32_t                   x;
   uint32_t                   y;
   bool                       stretch;
   uint32_t                   clientID;
   NEXUS_SurfaceClientHandle  surfaceClient;
} NXPL_SurfCompNativeWindow;

/* Context data for the entire display */
typedef struct
{
   BEGL_DisplayCallbacks      displayCallbacks;
   NEXUS_HEAPHANDLE           heap;
   NXPL_DisplayType           displayType;
   NXPL_DisplayType           lastDisplayType;
   BEGL_MemoryInterface       *memInterface;
   BEGL_DisplayInterface      *dispInterface;
   pthread_mutex_t            mutex;
} NXPL_DisplayData;


/*****************************************************************************
 * Display driver interface
 *****************************************************************************/
static void RecycleCallback(void *context, int param)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_WindowState *windowState = (NXPL_WindowState*)param;
   unsigned          n = 0, i, j;

   /*printf("RecycleCallback\n");*/

   do
   {
      NEXUS_SurfaceHandle surface_list[NUM_SWAP_BUFFERS];
      int rc = NEXUS_SurfaceClient_RecycleSurface(windowState->surfaceClient, surface_list, NUM_SWAP_BUFFERS, &n);
      if (rc)
         return;

      for (j = 0; j < n; j++)
      {
         int recycledIndex = -1;
         /*printf("RecycleCallback %p, surfaceClient = %p\n", surface_list[0], windowState->surfaceClient);*/

         /* Find this surface in our swap chain */
         for (i = 0; i < NUM_SWAP_BUFFERS; i++)
         {
            if (windowState->buffers[i] && windowState->buffers[i]->surface == surface_list[j])
            {
               recycledIndex = i;
               break;
            }
         }
         assert(recycledIndex >= 0 && recycledIndex < NUM_SWAP_BUFFERS);

         /*printf("unlock %p\n", windowState->buffers[recycledIndex]->surface);*/

         /* Atomic decrement */
         __sync_fetch_and_sub(&windowState->numPosted, 1);

         pthread_mutex_lock(&data->mutex);
         pthread_cond_broadcast(&windowState->buffers[recycledIndex]->cond);
         windowState->buffers[recycledIndex]->inUse = false;
         pthread_mutex_unlock(&data->mutex);
      }
   }
   while (n >= NUM_SWAP_BUFFERS);   /* Loop again if we got all our buffers recycled, in case more are waiting */
}

static BEGL_Error DispBufferDisplay(void *context, BEGL_BufferDisplayState *state)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)state->buffer;
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)state->windowState.window;
   NEXUS_Error err;

   NXPL_WindowState *windowState = (NXPL_WindowState*)state->windowState.platformState;

   /*printf("DispBufferDisplay\n");*/

   /* Atomic increment */
   __sync_fetch_and_add(&windowState->numPosted, 1);

   if (!windowState->hasBeenDisplayed || 
       data->displayType != data->lastDisplayType)
   {
      /* Window settings have changed */
      NEXUS_SurfaceClientSettings clientSettings;

      windowState->hasBeenDisplayed = true;

      /* setup the display & callback */
      NEXUS_SurfaceClient_GetSettings(windowState->surfaceClient, &clientSettings);
      /* client does not control position */
      clientSettings.recycled.context = context;
      clientSettings.recycled.param = (int)windowState;
      clientSettings.recycled.callback = RecycleCallback;

      switch (data->displayType)
      {
      default :
      case NXPL_2D :             clientSettings.orientation = NEXUS_VideoOrientation_e2D; break;
      case NXPL_3D_LEFT_RIGHT :  clientSettings.orientation = NEXUS_VideoOrientation_e3D_LeftRight; break;
      case NXPL_3D_OVER_UNDER :  clientSettings.orientation = NEXUS_VideoOrientation_e3D_OverUnder; break;
      }
      /*printf("Setting NEXUS_SurfaceClient_SetSettings for %p\n", windowState->surfaceClient);*/
      err = NEXUS_SurfaceClient_SetSettings(windowState->surfaceClient, &clientSettings);
      if (err)
        printf("NEXUS_SurfaceClient_SetSettings() failed\n");

      data->lastDisplayType = data->displayType;
   }

   /*printf("push %p, surfaceClient = %p\n", buffer->surface, windowState->surfaceClient);*/

   err = NEXUS_SurfaceClient_PushSurface(windowState->surfaceClient, buffer->surface, NULL, false);
   if (err)
      printf("NEXUS_SurfaceClient_PushSurface() failed\n");

   /* Call the driver callback immediately, next one will be pushed as soon as a buffer can be locked and rendered */
   data->displayCallbacks.BufferOnDisplay(state->cb_arg);

   return BEGL_Success;
}

/* Flush pending displays until they are all done, then removes all buffers from display. Will block until complete. */
static BEGL_Error DispWindowUndisplay(void *context, BEGL_WindowState *windowState)
{
   NXPL_DisplayData        *data = (NXPL_DisplayData*)context;
   NXPL_WindowState        *state = (NXPL_WindowState*)windowState->platformState;
   NEXUS_Error             err;
   uint32_t                i;

   if (state && state->surfaceClient)
   {
      /*printf("NEXUS_SurfaceClient_Release %p\n", state->surfaceClient);*/

      NEXUS_SurfaceClient_Clear(state->surfaceClient);
      NEXUS_SurfaceClient_Release(state->surfaceClient);
   }

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

   /* Surface must be created in our heap, and always with a cached address */
   NEXUS_Memory_Allocate(settings->totalByteSize, &memSettings, &surfSettings.pMemory);
   if (surfSettings.pMemory != NULL)
      buffer->surface = NEXUS_Surface_Create(&surfSettings);

   /* Note: these 3 lines are very deliberate, it means that FORCE_UNCACHED works correctly. */
   /* Be very careful if you change them. Think about uncached mode! */
   NEXUS_Heap_GetStatus(data->heap, &memStatus);
   settings->physOffset = memStatus.offset + ((uintptr_t)surfSettings.pMemory - (uintptr_t)memStatus.addr);
   settings->cachedAddr = data->memInterface->ConvertPhysicalToCached(data->memInterface->context, settings->physOffset);

   buffer->settings = *settings;

   /*printf("DispBufferCreate %x\n", buffer->surface);*/

   return (BEGL_BufferHandle)buffer;
}

/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferDisplayState *bufferState)
{
   NXPL_DisplayData *data = (NXPL_DisplayData*)context;
   NXPL_BufferData *buffer = (NXPL_BufferData*)bufferState->buffer;

   /*printf("DispBufferDestroy %x\n", buffer->surface);*/

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

      /* delete the condition */
      pthread_cond_destroy(&buffer->cond);

      memset(buffer, 0, sizeof(NXPL_BufferData));
      free(buffer);
   }

   return BEGL_Success;
}

/* Wraps our swap chain surfaces */
static BEGL_BufferHandle DispBufferGet(void *context, BEGL_BufferSettings *settings)
{
   NXPL_DisplayData              *data = (NXPL_DisplayData*)context;
   NXPL_BufferData               *buffer = NULL;
   NXPL_SurfCompNativeWindow     *nw = (NXPL_SurfCompNativeWindow*)settings->windowState.window;
   NXPL_WindowState              *windowState = (NXPL_WindowState *)settings->windowState.platformState;

   buffer = (NXPL_BufferData*)DispBufferCreate(context, settings);
   if (buffer)
   {
      assert(windowState->buffers[settings->usage - BEGL_BufferUsage_eSwapChain0] == NULL);
      windowState->buffers[settings->usage - BEGL_BufferUsage_eSwapChain0] = buffer;
      nw->surfaceClient = windowState->surfaceClient;

      /*printf("DispBufferGet creating %p, windowState = %p\n", buffer->surface, windowState);*/

      /* create the conditional for the buffer */
      pthread_cond_init(&buffer->cond, NULL);
   }

   return (BEGL_BufferHandle)buffer;
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
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)window;

   if (nw != NULL)
   {
      NEXUS_SurfaceClientStatus clientStatus;

      if (flags & BEGL_WindowInfoWidth || flags & BEGL_WindowInfoHeight)
      {
         if (nw->surfaceClient != NULL)
         {
            /* THIS IS CALLED ONCE AT START OF FRAME, MAKE SURE YOU DON'T CALL
               NEXUS_SurfaceClient_GetStatus() AGAIN.  NEXUS CHANGES THIS ASYNCHRONOUSLY! */
            NEXUS_SurfaceClient_GetStatus(nw->surfaceClient, &clientStatus);
            nw->width = info->width = clientStatus.position.width;
            nw->height = info->height = clientStatus.position.height;
         }
         else
         {
            info->width = nw->width;
            info->height = nw->height;
         }
      }
      info->swapchain_count = NUM_SWAP_BUFFERS; /* We implement triple buffering for the surface compositor, in this very file  */

      return BEGL_Success;
   }

   return BEGL_Fail;
}

static BEGL_Error DispBufferAccess(void *context, BEGL_BufferAccessState *bufferAccess)
{
   NXPL_DisplayData  *data = (NXPL_DisplayData*)context;

   /*printf("DispBufferAccess\n");*/

   if (data)
   {
      /* Not interested in actual lock/unlock functionality, but we will use lock to wait for the next available buffer */
      if (bufferAccess->lockType == BEGL_Lock)
      {
         NXPL_BufferData             *buffer = bufferAccess->buffer;
         NXPL_SurfCompNativeWindow        *nw = (NXPL_SurfCompNativeWindow*)bufferAccess->windowState.window;
         NXPL_WindowState                 *windowState = (NXPL_WindowState *)bufferAccess->windowState.platformState;
         NEXUS_SurfaceCreateSettings createSettings;
         NEXUS_SurfaceClientStatus        clientStatus;
         unsigned int                res;
         BEGL_BufferDisplayState          bufferState;
         BEGL_BufferSettings              bufferSettings;
         BEGL_PixmapInfo                  info;
         NEXUS_MemoryAllocationSettings   memSettings;
         NEXUS_MemoryStatus               memStatus;

         /*printf("locking %p\n", buffer->surface);*/

         /* Get the requested buffer */
         pthread_mutex_lock(&data->mutex);
         if (buffer->inUse == true)
         {
            /* this releases the mutex and waits on the condition */
            pthread_cond_wait(&buffer->cond, &data->mutex);
            /* if we get here, we've satisfied the condition and got the mutex back */
         }
         buffer->inUse = true;
         pthread_mutex_unlock(&data->mutex);

         /*printf("locked %p\n", buffer->surface);*/

         /* The buffer is now locked for our use - check that surface compositor hasn't resized its window.
            If it has, we need to resize the underlying surface. */
         if (windowState != NULL && windowState->surfaceClient != NULL)
         {
            bufferSettings = buffer->settings;

            if (nw->width != bufferSettings.width || nw->height != bufferSettings.height)
            {
               bufferSettings.width = nw->width;
               bufferSettings.height = nw->height;

               /* Delete the old buffer surface */
               if (buffer->surface)
               {
                  NEXUS_Surface_GetCreateSettings(buffer->surface, &createSettings);
                  NEXUS_Surface_Destroy(buffer->surface);
                  buffer->surface = NULL;
                  /* We do this using the create settings to ensure it's a cached ptr, even in FORCE_UNCACHED */
                  NEXUS_Memory_Free(createSettings.pMemory);
               }

               info.format = bufferSettings.format;
               info.width = bufferSettings.width;
               info.height = bufferSettings.height;
               data->displayCallbacks.BufferGetRequirements(&info, &bufferSettings);

               /* Make a new surface */
               createSettings.width = bufferSettings.width;
               createSettings.height = bufferSettings.height;
               createSettings.pitch = bufferSettings.pitchBytes;
               createSettings.alignment = bufferSettings.alignment;
               createSettings.heap = data->heap;

               NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
               memSettings.alignment = bufferSettings.alignment;
               memSettings.heap = data->heap;

               /* Surface must be created in our heap, and always with a cached address */
               NEXUS_Memory_Allocate(bufferSettings.totalByteSize, &memSettings, &createSettings.pMemory);
               if (createSettings.pMemory != NULL)
                  buffer->surface = NEXUS_Surface_Create(&createSettings);

               /* Note: these 3 lines are very deliberate, it means that FORCE_UNCACHED works correctly. */
               /* Be very careful if you change them. Think about uncached mode! */
               NEXUS_Heap_GetStatus(data->heap, &memStatus);
               bufferSettings.physOffset = memStatus.offset + ((uintptr_t)createSettings.pMemory - (uintptr_t)memStatus.addr);
               bufferSettings.cachedAddr = data->memInterface->ConvertPhysicalToCached(data->memInterface->context, bufferSettings.physOffset);

               buffer->settings = bufferSettings;
            }
         }

         /* update BEGL_BufferSettings with new information */
         NEXUS_Surface_GetCreateSettings(buffer->surface, &createSettings);

         buffer->settings.cachedAddr = createSettings.pMemory;
         buffer->settings.physOffset = data->memInterface->ConvertCachedToPhysical(data->memInterface->context, buffer->settings.cachedAddr);

         if (nw != NULL)
         {
            buffer->settings.width = nw->width;
            buffer->settings.height = nw->height;
         }
      }
      else if (bufferAccess->lockType == BEGL_Unlock)
      {
         /*printf("BEGL_Unlock %p\n", ((NXPL_BufferData*)bufferAccess->buffer)->surface);*/
      }

      return BEGL_Success;
   }

   return BEGL_Fail;
}

static void *DispWindowStateCreate(void *context, BEGL_WindowHandle window)
{
   NXPL_WindowState *windowState = (NXPL_WindowState *)malloc(sizeof(NXPL_WindowState));
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)window;
   memset(windowState, 0, sizeof(NXPL_WindowState));
   /*printf("DispWindowStateCreate = %p\n", windowState);*/

   /* Attach the surface client for this swapChain. There is one swapChain per native window, so
      we will have one client per native window. */
   nw->surfaceClient =
      windowState->surfaceClient = NEXUS_SurfaceClient_Acquire(nw->clientID);

   /*printf("NEXUS_SurfaceClient_Acquire %d = %p\n", nw->clientID, windowState->surfaceClient);*/

   if (!windowState->surfaceClient)
   {
      printf("Failed to acquire compositing client surface for client id %d", nw->clientID);
      return NULL;
   }

   return (void*)windowState;
}

static BEGL_Error DispWindowStateDestroy(void *swapChainCtx)
{
   NXPL_WindowState *windowState = (NXPL_WindowState *)swapChainCtx;

   if (windowState)
   {
      /*printf("DispWindowStateDestroy = %p\n", windowState);*/

      memset(windowState, 0, sizeof(NXPL_WindowState));
      free(windowState);
      return BEGL_Success;
   }

   return BEGL_Fail;
}

BEGL_DisplayInterface *NXPL_CreateDisplayInterface(BEGL_MemoryInterface *memIface, 
                                                   BEGL_DisplayCallbacks *displayCallbacks)
{
   NXPL_DisplayData *data;
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));

   if (disp != NULL)
   {
      data = (NXPL_DisplayData*)malloc(sizeof(NXPL_DisplayData));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));

      if (data != NULL)
      {
         NEXUS_MemoryStatus            memStatus;
         int topTwoBitsHeapStart;
         int topTwoBitsHeapEnd;

         memset(data, 0, sizeof(NXPL_DisplayData));

         /* create the mutex to stop the buffer being locked twice */
         pthread_mutex_init(&data->mutex, NULL);

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferGet = DispBufferGet;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->BufferAccess = DispBufferAccess;
         disp->WindowGetInfo = DispWindowGetInfo;
         disp->WindowPlatformStateCreate = DispWindowStateCreate;
         disp->WindowPlatformStateDestroy = DispWindowStateDestroy;
         disp->WindowUndisplay = DispWindowUndisplay;

         data->memInterface = memIface;
         data->dispInterface = disp;
         data->displayCallbacks = *displayCallbacks;

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

NEXUS_SurfaceClientHandle NXPL_CreateVideoWindowClient(void *native, unsigned windowId)
{
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)native;
   NEXUS_SurfaceClientHandle ret = NULL;

   if (nw && nw->surfaceClient)
      ret = NEXUS_SurfaceClient_AcquireVideoWindow(nw->surfaceClient, windowId);

   return ret;
}

void NXPL_ReleaseVideoWindowClient(NEXUS_SurfaceClientHandle handle)
{
   NEXUS_SurfaceClient_ReleaseVideoWindow(handle);
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
      if (mem->context != NULL)
      {
         NXPL_DisplayData *data = (NXPL_DisplayData*)mem->context;
         pthread_mutex_destroy(&data->mutex);
         free(mem->context);
      }

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

   if (data != NULL && data->displayInterface != NULL)
   {
      BEGL_BufferDisplayState bufferState;
      memset(&bufferState, 0, sizeof(BEGL_BufferDisplayState));
      bufferState.buffer = (BEGL_BufferHandle)pixmapHandle;

      data->displayInterface->BufferDestroy(data->displayInterface->context, &bufferState);
   }
}

void *NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info)
{
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)malloc(sizeof(NXPL_SurfCompNativeWindow));
   memset(nw, 0, sizeof(NXPL_SurfCompNativeWindow));

   if (nw != NULL && info != NULL)
   {
      nw->x = info->x;
      nw->y = info->y;
      nw->width = info->width;
      nw->height = info->height;
      nw->clientID = info->clientID;
   }

   return nw;
}

void NXPL_UpdateNativeWindow(void *native, const NXPL_NativeWindowInfo *info)
{
   NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)native;

   if (info != NULL && nw != NULL)
   {
      nw->x = info->x;
      nw->y = info->y;
      nw->width = info->width;
      nw->height = info->height;
      nw->clientID = info->clientID;
   }
}

void NXPL_DestroyNativeWindow(void *nativeWin)
{
   if (nativeWin != NULL)
   {
      NXPL_SurfCompNativeWindow *nw = (NXPL_SurfCompNativeWindow*)nativeWin;

      if (nw)
      {
         memset(nw, 0, sizeof(NXPL_SurfCompNativeWindow));
         free(nw);
      }
   }
}
