
#include <memory.h>
#include <assert.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#include "default_android.h"

#include "nexus_surface.h"
#include "nexus_memory.h"
#include "nexus_platform.h"

#include "message_queue.h"

#define AN_BUFFERS (3)

#define ABANDONEDSCRATCH_WIDTH 64
#define ABANDONEDSCRATCH_HEIGHT 64
#define ABANDONEDSCRATCH_BPP 4

typedef void (*BufferOnDisplayFunc)(uint32_t);

typedef struct
{
   BEGL_MemoryInterface          *memInterface;
   BEGL_HWInterface              *hwInterface;
   BufferOnDisplayFunc           bufferOnDisplayFunc;
   pthread_mutex_t               mutex;

   void *                        abandonedScratch;
   uint32_t                      abandonedScratchPhysical;
   pthread_t                     threadId;
   void *                        displayQueue;
} ANPL_Display;

typedef struct
{
   ANativeWindow                 *awin;
   BEGL_BufferSettings           settings;
   android_native_buffer_t       *surface;
   NEXUS_SurfaceHandle           internalSurface; /* only used for pixmaps, not main windows */

   /* keep everything in sync */
   pthread_cond_t                cond;
   bool                          inUse;
   bool                          abandoned;
} ANPL_BufferData;

extern NEXUS_HeapHandle NXPL_MemHeap(BEGL_MemoryInterface *mem);

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

typedef struct
{
   ANPL_Display    *data;
   ANPL_BufferData *buffer;
} displayQueueMessage_t;

static volatile uint32_t threadTerminated = 0;

void DispBufferPostThread(void * displayQueue)
{
   while(!threadTerminated)
   {
      ANPL_Display    *data;
      ANPL_BufferData *buffer;
      displayQueueMessage_t *message;

      message = receive_message(displayQueue, 0xFFFFFFFF);

      /* unpick the queue */
      data = message->data;
      buffer = message->buffer;

      assert(state->windowState->window == (BEGL_WindowHandle)buffer->awin);

      is_android_native_window(buffer->awin);

      buffer->awin->queueBuffer(buffer->awin, buffer->surface);

      /* LOGD("%s: %d IRQ: queue surface (%p) (%p) %p\n", __FUNCTION__, __LINE__, buffer->awin, buffer, buffer->surface); */
      buffer->surface->common.decRef(&buffer->surface->common);

      /* signal anything waiting on this that it can go */
      pthread_mutex_lock(&data->mutex);
      pthread_cond_broadcast(&buffer->cond);
      buffer->inUse = false;
      pthread_mutex_unlock(&data->mutex);

      release_message(displayQueue, message);
   }
}

static BEGL_Error DispBufferDisplay(void *context, BEGL_BufferDisplayState *state)
{
   ANPL_Display    *data = (ANPL_Display*)context;
   ANPL_BufferData *buffer = (ANPL_BufferData*)state->buffer;

   if (data)
   {
      /* queue()    -> cause a flip */
      if (buffer == NULL || buffer->awin == NULL || buffer->surface == NULL || buffer->abandoned)
      {
         if (!buffer->abandoned)
         {
            LOGD("%s: %d IRQ Called when nothing is installed (tid = %0x8.8x) (buffer = %p) (buffer->awin = %p) (buffer->surface = %p)\n", __FUNCTION__, __LINE__,
               gettid(),
               buffer,
               (buffer != NULL) ? buffer->awin : NULL,
               (buffer != NULL) ? buffer->surface : NULL);
         }
         else
         {
            LOGD("DispBufferDisplay() : skip abandoned buffer\n");
         }
      }
      else
      {
         displayQueueMessage_t *message = get_message(data->displayQueue, 0xFFFFFFFF);
         message->data = data;
         message->buffer = buffer;
         send_message(data->displayQueue, message);
      }

      if (state->cb_arg)
         data->bufferOnDisplayFunc(state->cb_arg);

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

static BEGL_Error DispIsSupported(void *context, BEGL_BufferFormat bufferFormat)
{
   if ((bufferFormat == BEGL_BufferFormat_eA8B8G8R8_TFormat) ||
       (bufferFormat == BEGL_BufferFormat_eX8B8G8R8_TFormat) ||
       (bufferFormat == BEGL_BufferFormat_eR5G6B5_TFormat) ||
       (bufferFormat == BEGL_BufferFormat_eA8B8G8R8_LTFormat) ||
       (bufferFormat == BEGL_BufferFormat_eX8B8G8R8_LTFormat) ||
       (bufferFormat == BEGL_BufferFormat_eR5G6B5_LTFormat))
   {
      return BEGL_Success;
   }
   else
   {
      return BEGL_Fail;
   }
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
   ANPL_Display                  *data = (ANPL_Display*)context;
   ANPL_BufferData               *buffer = NULL;
   NEXUS_SurfaceCreateSettings    surfSettings;
   NEXUS_MemoryAllocationSettings memSettings;
   NEXUS_MemoryStatus             memStatus;
   uint32_t                       bpp;

   LOGD("==========================================================================\n");
   LOGD("DispBufferCreate called\n");
   LOGD("==========================================================================\n");

   buffer = (ANPL_BufferData*)malloc(sizeof(ANPL_BufferData));
   memset(buffer, 0, sizeof(ANPL_BufferData));

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
   default:
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
      buffer->internalSurface = NEXUS_Surface_Create(&surfSettings);

   NEXUS_Heap_GetStatus(memSettings.heap, &memStatus);
   settings->physOffset = memStatus.offset + ((uintptr_t)surfSettings.pMemory - (uintptr_t)memStatus.addr);
   settings->cachedAddr = surfSettings.pMemory;

   buffer->settings = *settings;

   return (BEGL_BufferHandle)buffer;
}

/* Android supplies buffers back/front, whereas the abstract platform layer wants front/back..  they need to be switched */
static BEGL_BufferHandle DispBufferGet(void *context, BEGL_BufferSettings *settings)
{
   ANPL_Display    *data = (ANPL_Display*)context;
   ANPL_BufferData *buffer = NULL;
   int              format;

   buffer = (ANPL_BufferData*)malloc(sizeof(ANPL_BufferData));
   if (buffer)
   {
      memset(buffer, 0, sizeof(ANPL_BufferData));

      buffer->awin = (ANativeWindow*)settings->windowState.window;

      LOGD("DispBufferGet() : buffer->awin %p\n", buffer->awin);

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
         unsigned int mub = 1;

         if (buffer->awin->query(buffer->awin, NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, (int *)&mub) != 0)
         {
            LOGD("Unable to query NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS\n");
         }
         else
         {
            LOGD("==========================================================================\n");
            LOGD("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS = %d\n", mub);
            LOGD("==========================================================================\n");
         }

         /* we are triple buffered, allow compositor to hold one and have one in flight */
         /* sometimes the system has to keep 2, so we need effectivly 4 */
         if (native_window_set_buffer_count(buffer->awin, (AN_BUFFERS + (mub - 1))) != 0)
         {
            LOGD("DispBufferGet(): Unable to set the number of buffers to %d\n", (AN_BUFFERS + (mub - 1)));
         }

         /* surface texture - force to 1 */
         buffer->awin->setSwapInterval(buffer->awin, 1);

         /* android surface 0 */
         buffer->awin->common.incRef(&buffer->awin->common);

         /* LOGD("%s: BEGL_BufferUsage_eSwapChain0 created (%p)\n", __FUNCTION__, buffer); */
      }

      /* structure copy -> grab the elements above (inputs) to the buffer structure */
      buffer->settings = *settings;
   }

   return (BEGL_BufferHandle)buffer;
}


/* Destroy a buffer previously created with BufferCreate */
static BEGL_Error DispBufferDestroy(void *context, BEGL_BufferDisplayState *bufferState)
{
   ANPL_Display    *data = (ANPL_Display *)context;
   ANPL_BufferData *buffer = (ANPL_BufferData*)bufferState->buffer;

   if (buffer != NULL)
   {
      assert(bufferState->windowState->window == (BEGL_WindowHandle)buffer->awin);

      if (buffer->settings.usage == BEGL_BufferUsage_ePixmap)
      {
         /* for pixmap surfaces, the surface is always created via DispBufferCreate() */
         /* in this case we stashed the nexus handle in internalSurface */
         if (buffer->internalSurface && buffer->settings.cachedAddr)
         {
            NEXUS_Surface_Destroy(buffer->internalSurface);
            NEXUS_Memory_Free(buffer->settings.cachedAddr);
         }
         else
         {
            LOGD("DispBufferDestroy() BEGL_BufferUsage_ePixmap was invalid\n");
         }
      }
      else
      {
         LOGD("DispBufferDestroy : (buffer = %p)\n", buffer->surface);

         /* remove any locks on a buffer which may have been claimed prior to rendering */
         if (buffer->surface != NULL)
         {
            LOGD("%s: Cancel buffer %p\n", __FUNCTION__, buffer->surface);
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

      memset(buffer, 0, sizeof(ANPL_BufferData));
      free(buffer);
   }
   return BEGL_Success;
}



/* Get information about a created window buffer */
static BEGL_Error DispBufferGetCreateSettings(void *context, BEGL_BufferHandle bufHandle, BEGL_BufferSettings *settings)
{
   ANPL_BufferData *buffer = (ANPL_BufferData*)bufHandle;

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
   ANPL_Display   *data = (ANPL_Display*)context;
   ANativeWindow  *dst = (ANativeWindow*)window;
   int fmt;

   /* get the size from the android_native_window_t */
   if (flags & BEGL_WindowInfoWidth)
   {
      int res = dst->query(dst, NATIVE_WINDOW_WIDTH, (int *)&info->width);
      if (res == -ENODEV)
      {
         /* surface texture has been abandoned */
         info->width = ABANDONEDSCRATCH_WIDTH;
      }
   }

   if (flags & BEGL_WindowInfoHeight)
   {
      int res = dst->query(dst, NATIVE_WINDOW_HEIGHT, (int *)&info->height);
      if (res == -ENODEV)
      {
         info->height = ABANDONEDSCRATCH_HEIGHT;
      }
   }

   if (flags & BEGL_WindowInfoFormat)
   {
      int res = dst->query(dst, NATIVE_WINDOW_FORMAT, (int *)&fmt);

      if ((res == -ENODEV) || (fmt == HAL_PIXEL_FORMAT_RGBA_8888))
         info->format = BEGL_BufferFormat_eA8B8G8R8;
      else if (fmt == HAL_PIXEL_FORMAT_RGBX_8888)
         info->format = BEGL_BufferFormat_eX8B8G8R8;
      else if (fmt == HAL_PIXEL_FORMAT_RGB_565)
         info->format = BEGL_BufferFormat_eR5G6B5;
      else
         info->format = BEGL_BufferFormat_INVALID;
   }

   /* make sure we initialize enough structures for the buffers we have */
   if (flags & BEGL_WindowInfoSwapChainCount)
      info->swapchain_count = AN_BUFFERS;

   return BEGL_Success;
}

static BEGL_Error DispBufferAccess(void *context, BEGL_BufferAccessState *bufferAccess)
{
   ANPL_Display *data = (ANPL_Display*)context;

   if (data)
   {
      /* unlock is implicit in dequeue so we don't need a case for it */
      if (bufferAccess->lockType == BEGL_Lock)
      {
         int res;
         int fmt;
         android_native_buffer_t *surface = NULL;
         ANPL_BufferData   * buffer = bufferAccess->buffer;

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

         buffer->abandoned = false;
         do
         {
            res = buffer->awin->dequeueBuffer(buffer->awin, &surface);
            if (res != 0)
            {
               /* android can legitimately get a EBUSY, retry again */
#ifdef DEBUG
               LOGD("DispBufferAccess(): buffer->awin %p\n", buffer->awin);
               LOGD("DispBufferAccess(): dequeueBuffer error %d\n", res);
#endif
               usleep(1);
            }
         } while (res == -EBUSY);

         if (res != 0)
         {
            LOGD("DispBufferAccess(): surface has been probably been abandoned\n");
            buffer->abandoned = true;

            /* update BEGL_BufferSettings with new information */
            buffer->settings.physOffset = data->abandonedScratchPhysical;
            buffer->settings.pitchBytes = ABANDONEDSCRATCH_WIDTH * ABANDONEDSCRATCH_BPP;
            buffer->settings.cachedAddr = data->abandonedScratch;

            /* get the size from the android_native_buffer_t */
            buffer->settings.width = ABANDONEDSCRATCH_WIDTH;
            buffer->settings.height = ABANDONEDSCRATCH_HEIGHT;
            buffer->settings.format = BEGL_BufferFormat_eA8B8G8R8;
            buffer->surface = NULL;
         }
         else
         {
            /* LOGD("%s: %d Dequeue Done Surface: (%p) (%p) %p\n", __FUNCTION__, __LINE__, buffer->awin, buffer, surface); */
            surface->common.incRef(&surface->common);
            /* LOGD("%s: BEFORE LOCK %p\n", __FUNCTION__, surface); */
            buffer->awin->lockBuffer(buffer->awin, surface);
            /* LOGD("%s: AFTER LOCK %p\n", __FUNCTION__, surface); */

            struct private_handle_t const* hnd =  (struct private_handle_t const*) (surface->handle);

            /* update BEGL_BufferSettings with new information */
            buffer->settings.physOffset = hnd->oglPhysicalAddress;
            buffer->settings.pitchBytes = hnd->oglStride;

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
      }
      return BEGL_Success;
   }

   return BEGL_Fail;
}

BEGL_DisplayInterface *ANPL_CreateDisplayInterface(BEGL_MemoryInterface *mem,
                                                   BEGL_HWInterface *hw,
                                                   BEGL_DisplayCallbacks *displayCallbacks)
{
   ANPL_Display          *data;
   BEGL_DisplayInterface *disp = (BEGL_DisplayInterface*)malloc(sizeof(BEGL_DisplayInterface));

   if (disp != NULL)
   {
      data = (ANPL_Display *)malloc(sizeof(ANPL_Display));
      memset(disp, 0, sizeof(BEGL_DisplayInterface));

      if (data != NULL)
      {
         NEXUS_MemoryStatus             memStatus;
         NEXUS_MemoryAllocationSettings memSettings;

         memset(data, 0, sizeof(ANPL_Display));

         pthread_mutex_init(&data->mutex, NULL);

         disp->context = (void*)data;
         disp->BufferDisplay = DispBufferDisplay;
         disp->BufferCreate = DispBufferCreate;
         disp->BufferGet = DispBufferGet;
         disp->BufferDestroy = DispBufferDestroy;
         disp->BufferGetCreateSettings = DispBufferGetCreateSettings;
         disp->BufferAccess = DispBufferAccess;
         disp->WindowUndisplay = DispWindowUndisplay;
         disp->WindowGetInfo = DispWindowGetInfo;
         disp->IsSupported = DispIsSupported;

         data->bufferOnDisplayFunc = displayCallbacks->BufferOnDisplay;

         /* Store the memory interface */
         data->memInterface = mem;
         /* Store the hw interface */
         data->hwInterface = hw;

         /* allocate 64x64x4 (basically a tile for abandoned surfaces) */
         NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
         memSettings.alignment = 4096;
         memSettings.heap = NXPL_MemHeap(mem);

         NEXUS_Memory_Allocate(ABANDONEDSCRATCH_WIDTH *
                               ABANDONEDSCRATCH_HEIGHT *
                               ABANDONEDSCRATCH_BPP, &memSettings, &data->abandonedScratch);

         NEXUS_Heap_GetStatus(memSettings.heap, &memStatus);
         data->abandonedScratchPhysical = memStatus.offset + ((uintptr_t)(data->abandonedScratch) - (uintptr_t)memStatus.addr);

         /* most swap chains have 3 buffers, 32 should be more than enough */
         data->displayQueue = create_queue(32, sizeof(displayQueueMessage_t));
         pthread_create(&data->threadId, NULL, DispBufferPostThread, data->displayQueue);
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

void ANPL_DestroyDisplayInterface(BEGL_DisplayInterface * disp)
{
   if (disp)
   {
      ANPL_Display *data = (ANPL_Display *)disp->context;
      if (data)
      {
         NEXUS_Memory_Free(data->abandonedScratch);
         pthread_mutex_destroy(&data->mutex);

         threadTerminated = 1;
         pthread_join(data->threadId, NULL);
         delete_queue(data->displayQueue);

         free(data);
      }
      free(disp);
   }
}

bool ANPL_BufferGetRequirements(ANPL_PlatformHandle handle,
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
