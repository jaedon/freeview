
#include <memory.h>
#include <assert.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <EGL/egl.h>

#include "default_RSO_android.h"
#include "nexus_surface.h"
#include "nexus_memory.h"
#include "nexus_platform.h"

#define ANPL_TAG "RSOANPL"

#define AN_BUFFERS (2)

NEXUS_HeapHandle NXPL_MemHeap(BEGL_MemoryInterface *mem);

typedef void (*BufferOnDisplayFunc)(uint32_t);

typedef struct
{
   BEGL_MemoryInterface          *memInterface;
   BEGL_HWInterface              *hwInterface;
   BufferOnDisplayFunc           bufferOnDisplayFunc;
   pthread_mutex_t               mutex;
} RSOAN_Display;

typedef struct
{
   ANativeWindow                 *awin;
   BEGL_BufferSettings           settings;
   android_native_buffer_t       *surface;

   /* keep everything in sync */
   pthread_cond_t                cond;
   bool                          inUse;
} RSOAN_BufferData;


static int is_android_native_window(ANativeWindow * win)
{
   int res = (win && (win->common.magic == ANDROID_NATIVE_WINDOW_MAGIC)) ? 1 :0;
   if(!res)
   {
      LOGD("==========================================================================\n");
      LOGD("INVALID WINDOW INVALID WINDOW INVALID WINDOW INVALID WINDOW INVALID WINDOW\n");
      LOGD("==========================================================================\n");
   }
   return res;
}

/*****************************************************************************
 * Display driver interface
 *****************************************************************************/

static BEGL_Error DispBufferDisplay(void *context, BEGL_BufferDisplayState *state)
{
   RSOAN_Display    *data = (RSOAN_Display*)context;
   RSOAN_BufferData *buffer = (RSOAN_BufferData*)state->buffer;

   if (data)
   {
      /* queue()    -> cause a flip */
      if (buffer == NULL || buffer->awin == NULL || buffer->surface == NULL)
      {
          LOGD("%s: %d IRQ Called when nothing is installed (tid = %p) (buffer = %p) (buffer->awin = %p) (buffer->surface = %p)\n", __FUNCTION__, __LINE__,
             gettid(),
             buffer,
             (buffer != NULL) ? buffer->awin : NULL,
             (buffer != NULL) ? buffer->surface : NULL);
      }
      else
      {
         /* LOGD("%s: unlocking and post (%p)\n", __FUNCTION__, buffer); */

         assert(state->windowState->window == (BEGL_WindowHandle)buffer->awin);

         is_android_native_window(buffer->awin);
         /* LOGD("%s: %d IRQ: queue surface %p\n", __FUNCTION__, __LINE__, buffer->surface); */
         buffer->awin->queueBuffer(buffer->awin, buffer->surface);
         buffer->surface->common.decRef(&buffer->surface->common);

         /* signal anything waiting on this that it can go */
         pthread_mutex_lock(&data->mutex);
         pthread_cond_broadcast(&buffer->cond);
         buffer->inUse = false;
         pthread_mutex_unlock(&data->mutex);

         data->bufferOnDisplayFunc(state->cb_arg);
      }

      return BEGL_Success;
   }

   return BEGL_Fail;
}

/* Flush pending displays until they are all done, then removes all buffers from display. Will block until complete. */
static BEGL_Error DispWindowUndisplay(void *context, BEGL_WindowState *windowState)
{
   LOGD("==========================================================================\n");
   LOGD("DispWindowUndisplay called\n");
   LOGD("==========================================================================\n");
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
   RSOAN_Display                  *data = (RSOAN_Display*)context;
   RSOAN_BufferData               *buffer = NULL;
   NEXUS_SurfaceCreateSettings    surfSettings;
   NEXUS_MemoryAllocationSettings memSettings;
   NEXUS_MemoryStatus             memStatus;
   uint32_t                       bpp;

   LOGD("==========================================================================\n");
   LOGD("DispBufferCreate called\n");
   LOGD("==========================================================================\n");
   
   buffer = (RSOAN_BufferData*)malloc(sizeof(RSOAN_BufferData));
   memset(buffer, 0, sizeof(RSOAN_BufferData));

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
   default :
      LOGD("==========================================================================\n");
      LOGD("unsupported pixel format, aborting\n");
      LOGD("==========================================================================\n");
      assert(0);
      break;
   }

   surfSettings.width = settings->width;
   surfSettings.height = settings->height;
   surfSettings.alignment = settings->alignment;
   surfSettings.pitch = settings->pitchBytes;
   surfSettings.heap = NXPL_MemHeap(data->memInterface);

   NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
   memSettings.alignment = settings->alignment;

   memSettings.heap = surfSettings.heap;

   /* Surface must be created in our heap, and always with a cached address */
   NEXUS_Memory_Allocate(settings->totalByteSize, &memSettings, &surfSettings.pMemory);
   if (surfSettings.pMemory != NULL)
      buffer->surface = (android_native_buffer_t *)NEXUS_Surface_Create(&surfSettings);

   /* Note: these 3 lines are very deliberate, it means that FORCE_UNCACHED works correctly.
    * Be very careful if you change them. Think about uncached mode! */
   NEXUS_Heap_GetStatus(memSettings.heap, &memStatus);
   settings->physOffset = memStatus.offset + ((uintptr_t)surfSettings.pMemory - (uintptr_t)memStatus.addr);
   settings->cachedAddr = data->memInterface->ConvertPhysicalToCached(data->memInterface->context, settings->physOffset);

   buffer->settings = *settings;

   return (BEGL_BufferHandle)buffer;
}

/* does nothing more than create the platform abstraction */
static BEGL_BufferHandle DispBufferGet(void *context, BEGL_BufferSettings *settings)
{
   RSOAN_Display    *data = (RSOAN_Display*)context;
   RSOAN_BufferData *buffer = NULL;
   int              format;

   buffer = (RSOAN_BufferData*)malloc(sizeof(RSOAN_BufferData));
   if (buffer)
   {
      memset(buffer, 0, sizeof(RSOAN_BufferData));

      buffer->awin = (ANativeWindow*)settings->windowState.window;

      is_android_native_window(buffer->awin);

      /* query the surface type is one of the 3d cores renderable types */
      buffer->awin->query(buffer->awin, NATIVE_WINDOW_FORMAT, &format);

      if ((format != HAL_PIXEL_FORMAT_RGBA_8888) &&
          (format != HAL_PIXEL_FORMAT_RGBX_8888) &&
          (format != HAL_PIXEL_FORMAT_RGB_565))
      {
         LOGD("==========================================================================\n");
         LOGD("Provided android_native_window_t is not a renderable type, aborting\n");
         LOGD("==========================================================================\n");
         assert(0);
      }

      /* create the conditional for the buffer */
      pthread_cond_init(&buffer->cond, NULL);

      if (settings->usage == BEGL_BufferUsage_eSwapChain0)
      {
         /* android surface 0 */
         buffer->awin->common.incRef(&buffer->awin->common);
         LOGD("%s: BEGL_BufferUsage_eSwapChain0 created (%p)\n", __FUNCTION__, buffer);
      }
      else
      {
         LOGD("%s: Dummy (%p)\n", __FUNCTION__, buffer);
      }

      /* structure copy -> grab the elements above (inputs) to the buffer structure */
      buffer->settings = *settings;
   }

   return (BEGL_BufferHandle)buffer;
}

/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferDisplayState *bufferState)
{
   RSOAN_Display    *data = (RSOAN_Display*)context;
   RSOAN_BufferData *buffer = (RSOAN_BufferData*)bufferState->buffer;

   assert(bufferState->windowState.window == (BEGL_WindowHandle)buffer->awin);

   is_android_native_window(buffer->awin);
   if (buffer != NULL)
   {
      if (buffer->settings.usage == BEGL_BufferUsage_ePixmap)
      {
         /* for pixmap surfaces, the surface is always created via DispBufferCreate() */
         NEXUS_Memory_Free(buffer->settings.cachedAddr);
      }
      else
      {
         /* remove any locks on a buffer which may have been claimed prior to rendering */
         if (buffer->surface != NULL)
         {
            buffer->awin->cancelBuffer(buffer->awin, buffer->surface);
            buffer->surface->common.decRef(&buffer->surface->common);
         }

         /* delete the condition */
         pthread_cond_destroy(&buffer->cond);

         if(buffer->settings.usage == BEGL_BufferUsage_eSwapChain0)
         {
            buffer->awin->common.decRef(&buffer->awin->common);
         }
      }

      memset(buffer, 0, sizeof(RSOAN_BufferData));
      free(buffer);
   }

   return BEGL_Success;
}



/* Get information about a created window buffer */
static BEGL_Error DispBufferGetCreateSettings(void *context, BEGL_BufferHandle bufHandle, BEGL_BufferSettings *settings)
{
   RSOAN_Display    *data = (RSOAN_Display*)context;
   RSOAN_BufferData *buffer = (RSOAN_BufferData*)bufHandle;

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
   RSOAN_Display  *data = (RSOAN_Display*)context;
   ANativeWindow  *dst = (ANativeWindow*)window;
   int            fmt;

   is_android_native_window(dst);

   /* get the size from the android_native_window_t */
   if (flags & BEGL_WindowInfoWidth)
   {
      int res;
      res = dst->query(dst, NATIVE_WINDOW_WIDTH, &info->width);
      if (res < 0)
      {
         LOGD("Unable to get 'dst->query(dst, NATIVE_WINDOW_WIDTH, &info->width)'\n");
         info->width = 0;
      }
   }

   if (flags & BEGL_WindowInfoHeight)
   {
      int res;
      res = dst->query(dst, NATIVE_WINDOW_HEIGHT, &info->height);
      if (res < 0)
      {
         LOGD("Unable to get 'dst->query(dst, NATIVE_WINDOW_HEIGHT, &info->height)'\n");
         info->height = 0;
      }
   }

   if (flags & BEGL_WindowInfoFormat)
   {
      dst->query(dst, NATIVE_WINDOW_FORMAT, &fmt);

      if (fmt == HAL_PIXEL_FORMAT_RGBA_8888)
         info->format = BEGL_BufferFormat_eA8B8G8R8;
      else if (fmt == HAL_PIXEL_FORMAT_RGBX_8888)
         info->format = BEGL_BufferFormat_eX8B8G8R8;
      else if (fmt == HAL_PIXEL_FORMAT_RGB_565)
         info->format = BEGL_BufferFormat_eR5G6B5;
      else
         info->format = BEGL_BufferFormat_INVALID;
   }

   /* framebuffer only supports double buffering */
   if (flags & BEGL_WindowInfoSwapChainCount)
      info->swapchain_count = AN_BUFFERS;

   return BEGL_Success;
}

static BEGL_Error DispBufferAccess(void *context, BEGL_BufferAccessState *bufferAccess)
{
   RSOAN_Display *data = (RSOAN_Display*)context;

   if (data)
   {
      /* unlock is implicit in dequeue so we don't need a case for it */
      if (bufferAccess->lockType == BEGL_Lock)
      {
         unsigned int res;
         int fmt;
         android_native_buffer_t *surface = NULL;
         RSOAN_BufferData   * buffer = bufferAccess->buffer;

         assert(bufferAccess->windowState->window == (BEGL_WindowHandle)buffer->awin);

         is_android_native_window(buffer->awin);

         pthread_mutex_lock(&data->mutex);
         if (buffer->inUse == true)
         {
            /* this releases the mutex and waits on the condition */
            pthread_cond_wait(&buffer->cond, &data->mutex);
            /* if we get here, we've satisfied the condition and got the mutex back */
         }
         buffer->inUse = true;
         pthread_mutex_unlock(&data->mutex);

         buffer->awin->dequeueBuffer(buffer->awin, &surface);
         /* LOGD("%s: %d Dequeue Done Surface:%p\n", __FUNCTION__, __LINE__, surface); */
         surface->common.incRef(&surface->common);
         buffer->awin->lockBuffer(buffer->awin, surface);

         struct private_handle_t const* hnd =  (struct private_handle_t const*) (surface->handle);

         /* update BEGL_BufferSettings with new information */
         buffer->settings.physOffset = hnd->nxSurfacePhysicalAddress;
         /* buffer->settings.pitchBytes = hnd->nxSurfaceStride; */

         /* convert physical to cached */
         buffer->settings.cachedAddr = data->memInterface->ConvertPhysicalToCached(data->memInterface->context, buffer->settings.physOffset);

#if 0
         if (buffer->settings.cachedAddr == 0)
         {
            LOGD("***********************************************************\n");
            LOGD("FAILURE, CACHED ADDRESS == 0");
            LOGD("***********************************************************\n");
         }
#endif

         /* get the size from the android_native_buffer_t */
         buffer->settings.width = surface->width;
         buffer->settings.height = surface->height;

         if (surface->format == HAL_PIXEL_FORMAT_RGBA_8888)
            buffer->settings.format = BEGL_BufferFormat_eA8B8G8R8;
         else if (surface->format == HAL_PIXEL_FORMAT_RGBX_8888)
            buffer->settings.format = BEGL_BufferFormat_eX8B8G8R8;
         else if (surface->format == HAL_PIXEL_FORMAT_RGB_565)
            buffer->settings.format = BEGL_BufferFormat_eR5G6B5;
         else
            buffer->settings.format = BEGL_BufferFormat_INVALID;

         buffer->surface = surface;
      }

      return BEGL_Success;
   }

   return BEGL_Fail;
}

BEGL_DisplayInterface *RSOANPL_CreateDisplayInterface(BEGL_MemoryInterface *mem,
                                                      BEGL_HWInterface *hw,
                                                      BEGL_DisplayCallbacks *displayCallbacks)
{
   RSOAN_Display         *data;
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));

   if (disp != NULL)
   {
      data = (RSOAN_Display *)malloc(sizeof(RSOAN_Display));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));

      if (data != NULL)
      {
         memset(data, 0, sizeof(RSOAN_Display));

         pthread_mutex_init(&data->mutex, NULL);

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->WindowUndisplay = DispWindowUndisplay;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferGet = DispBufferGet;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->BufferAccess = DispBufferAccess;
         disp->WindowGetInfo = DispWindowGetInfo;

         data->bufferOnDisplayFunc = displayCallbacks->BufferOnDisplay;

         /* Store the memory interface */
         data->memInterface = mem;
         /* Store the hw interface */
         data->hwInterface = hw;
      }
      else
      {
         goto error0;
      }
   }

   return disp;

error0:
   free(disp);

   return NULL;
}

void RSOANPL_DestroyDisplayInterface(BEGL_DisplayInterface * disp)
{
   if (disp)
   {
      RSOAN_Display *data = (RSOAN_Display *)disp->context;
      if (data)
      {
         pthread_mutex_destroy(&data->mutex);
         free(data);
      }
      free(disp);
   }
}

bool RSOANPL_BufferGetRequirements(RSOANPL_PlatformHandle handle,
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

