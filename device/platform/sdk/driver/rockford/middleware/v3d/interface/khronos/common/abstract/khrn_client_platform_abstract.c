#include <stdlib.h>

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_int_parallel.h"
#include "interface/khronos/common/khrn_client_platform.h"
#include "interface/khronos/common/khrn_client.h"
#include "interface/khronos/egl/egl_client_config.h"
#include "middleware/khronos/egl/egl_server.h"
#include "helpers/vc_image/vc_image.h"
#include "interface/vcos/vcos.h"
#include "middleware/khronos/common/khrn_hw.h"
#ifndef V3D_LEAN
#include "middleware/khronos/dispatch/khrn_dispatch_rpc.h" /* khdispatch_send_async prototype */
#endif
#include "interface/khronos/common/khrn_int_ids.h"

#include "interface/khronos/common/khrn_client_platform.h"

#ifdef ANDROID
#include <cutils/log.h>
#endif

#ifdef KHRONOS_CLIENT_LOGGING
#include <stdio.h>
#include <stdarg.h>
FILE *xxx_vclog = NULL;

void khronos_client_log(const char *fmt, ...)
{
   va_list ap;
   va_start(ap,fmt);
   vprintf(fmt, ap);
   fflush(stdout);
   if(xxx_vclog!=NULL)
   {
      vfprintf(xxx_vclog, fmt, ap);
      fflush(xxx_vclog);
   }
   va_end(ap);
   printf("\n");
}

#endif

// Client is single-threaded on direct platform so no mutex stuff necessary

// TODO: find somewhere better to put this function?

void khdispatch_send_async(uint32_t command, uint64_t pid, uint32_t sem)
{
   if (sem != KHRN_NO_SEMAPHORE) {
      int name[3];

      name[0] = (int)pid; name[1] = (int)(pid >> 32); name[2] = (int)sem;

      if (command == ASYNC_COMMAND_DESTROY) {
         /* todo: destroy */
      } else {
         PLATFORM_SEMAPHORE_T s;
         if (khronos_platform_semaphore_create(&s, name, 1) == KHR_SUCCESS) {
            switch (command) {
            case ASYNC_COMMAND_WAIT:
               /* todo: i don't understand what ASYNC_COMMAND_WAIT is for, so this
                * might be completely wrong */
               khronos_platform_semaphore_acquire(&s);
               break;
            case ASYNC_COMMAND_POST:
               khronos_platform_semaphore_release(&s);
               break;
            default:
               UNREACHABLE();
            }
            khronos_platform_semaphore_destroy(&s);
         }
      }
   }
}

VCOS_STATUS_T khronos_platform_semaphore_create(PLATFORM_SEMAPHORE_T *sem, int name[3], int count)
{
   char buf[64];
   // should be KhanSemaphore but we are limited to 31 characters
   vcos_snprintf(buf,sizeof(buf),"KhanSem%08x%08x%08x", name[0], name[1], name[2]);
   return vcos_named_semaphore_create(sem, buf, count);
}

void khronos_platform_semaphore_destroy(PLATFORM_SEMAPHORE_T *sem)
{
   vcos_named_semaphore_delete(sem);
}

void khronos_platform_semaphore_acquire(PLATFORM_SEMAPHORE_T *sem)
{
   vcos_named_semaphore_wait(sem);
}

VCOS_STATUS_T khronos_platform_semaphore_acquire_timeout(PLATFORM_SEMAPHORE_T *sem, int timeout)
{
   return vcos_named_semaphore_wait_timeout(sem, timeout);
}

void khronos_platform_semaphore_release(PLATFORM_SEMAPHORE_T *sem)
{
   vcos_named_semaphore_post(sem);
   khrn_sync_notify_master();
}

VCOS_STATUS_T platform_tls_create(PLATFORM_TLS_T *tls)
{
   return vcos_tls_create(tls);
}

void platform_tls_destroy(PLATFORM_TLS_T tls)
{
   vcos_tls_delete(tls);
}

void platform_tls_set(PLATFORM_TLS_T tls, void *v)
{
   if (vcos_tls_set(tls, v) != VCOS_SUCCESS)
   {
#ifdef ANDROID
      LOGD("platform_tls_set() : FAILED (key %p), (v %p)\n", tls, v);
#endif
   }
}

void *platform_tls_get(PLATFORM_TLS_T tls)
{
   void * tsd = vcos_tls_get(tls);
   if (tsd == NULL)
   {
#ifdef ANDROID
      LOGD("platform_tls_get() : creating TLS block, (tid = %d) (pid = %d)\n", gettid(), getpid());
#endif
      if (client_thread_attach(tls))
         tsd = vcos_tls_get(tls);
#ifdef ANDROID
      else
         LOGD("client_thread_attach() : FAILED\n");
#endif
   }

#ifdef ANDROID
   if (tsd == NULL)
   {
      LOGD("***************************************************************************\n");
      LOGD("ERROR : TLS = NULL, (tid = %d) (pid = %d)\n", gettid(), getpid());
      LOGD("***************************************************************************\n");
   }
#endif

   return tsd;
}

void platform_tls_remove(PLATFORM_TLS_T tls)
{
   vcos_tls_set(tls, NULL);
}

/* ----------------------------------------------------------------------
 * workaround for broken platforms which don't detect threads exiting
 * -------------------------------------------------------------------- */
void platform_hint_thread_finished()
{
   client_thread_detach();
}

void *khrn_platform_malloc(size_t size, const char *desc)
{
   UNUSED(desc);

   return malloc(size);
}

void khrn_platform_free(void *v)
{
   free(v);
}

void platform_get_info(EGLNativeWindowType win, unsigned int flags, CLIENT_PLATFORM_INFO_T * info)
{
   BEGL_WindowInfo         windowInfo;
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();

   if (info)
   {
      memset(info, 0, sizeof(CLIENT_PLATFORM_INFO_T));

      if ((driverInterfaces != NULL) &&
          (driverInterfaces->displayInterface != NULL) &&
          (driverInterfaces->displayInterface->WindowGetInfo) != NULL)
      {
         BEGL_Error ret;
         BEGL_WindowInfoFlags wiFlags = 0;
         memset(&windowInfo, 0, sizeof(BEGL_WindowInfo));

         if (flags & GI_WIDTH)
            wiFlags |= BEGL_WindowInfoWidth;
         if (flags & GI_HEIGHT)
            wiFlags |= BEGL_WindowInfoHeight;
         if (flags & GI_FMT)
            wiFlags |= BEGL_WindowInfoFormat;
         if (flags & GI_SWAPCC)
            wiFlags |= BEGL_WindowInfoSwapChainCount;

         ret = driverInterfaces->displayInterface->WindowGetInfo(driverInterfaces->displayInterface->context,
            win, wiFlags, &windowInfo);
         if (ret == BEGL_Success)
         {
            info->width = windowInfo.width;
            info->height = windowInfo.height;
            info->swapchain_count = windowInfo.swapchain_count;
            info->format = windowInfo.format;
         }
      }
   }
}

bool platform_supported_format(KHRN_IMAGE_FORMAT_T format)
{
   bool res;

   /* a platform is always required to provide RSO outputs */
   if (khrn_image_is_rso(format))
      res = true;
   else
   {
      BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

      if ((driverInterfaces != NULL) &&
          (driverInterfaces->displayInterface != NULL) &&
          (driverInterfaces->displayInterface->IsSupported != NULL))
      {
         BEGL_BufferFormat bufferFormat;

         switch (format)
         {
         case ABGR_8888_TF:      bufferFormat = BEGL_BufferFormat_eA8B8G8R8_TFormat;      break;
         case XBGR_8888_TF:      bufferFormat = BEGL_BufferFormat_eX8B8G8R8_TFormat;      break;
         case RGB_565_TF:        bufferFormat = BEGL_BufferFormat_eR5G6B5_TFormat;        break;
         case ABGR_8888_LT:      bufferFormat = BEGL_BufferFormat_eA8B8G8R8_LTFormat;     break;
         case XBGR_8888_LT:      bufferFormat = BEGL_BufferFormat_eX8B8G8R8_LTFormat;     break;
         case RGB_565_LT:        bufferFormat = BEGL_BufferFormat_eR5G6B5_LTFormat;       break;
         default:                bufferFormat = BEGL_BufferFormat_INVALID;                break;
         }

         if (driverInterfaces->displayInterface->IsSupported(driverInterfaces->displayInterface->context, bufferFormat) == BEGL_Success)
            res = true;
         else
            res = false;
      }
      else
         res = false;
   }

   return res;
}

bool platform_get_buffersettings(void * opaque_buffer_handle, BEGL_BufferSettings * bufferSettings)
{
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();
   BEGL_Error ret = BEGL_Fail;

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->BufferGetCreateSettings != NULL) &&
       (bufferSettings != NULL))
   {
      ret = driverInterfaces->displayInterface->BufferGetCreateSettings(
         driverInterfaces->displayInterface->context,
         opaque_buffer_handle,
         bufferSettings);
   }

   return (ret == BEGL_Success);
}

bool platform_lock(void * opaque_buffer_handle, BEGL_WindowState *windowState)
{
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();
   BEGL_Error ret = BEGL_Fail;

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->BufferAccess != NULL))
   {
      BEGL_BufferAccessState bufferAccess;
      bufferAccess.buffer = opaque_buffer_handle;
      bufferAccess.lockType = BEGL_Lock;
      if (windowState != NULL)
      {
         bufferAccess.windowState = *windowState;
      }

      ret = driverInterfaces->displayInterface->BufferAccess(driverInterfaces->displayInterface->context,
                                                             &bufferAccess);
   }

   return (ret == BEGL_Success);
}

void platform_unlock(void * opaque_buffer_handle, BEGL_WindowState *windowState)
{
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->BufferAccess != NULL))
   {
      BEGL_BufferAccessState bufferAccess;
      bufferAccess.buffer = opaque_buffer_handle;
      bufferAccess.lockType = BEGL_Unlock;
      if (windowState != NULL)
      {
         bufferAccess.windowState = *windowState;
      }

      driverInterfaces->displayInterface->BufferAccess(driverInterfaces->displayInterface->context,
                                                       &bufferAccess);
   }
}


bool platform_get_pixmap_info(EGLNativePixmapType pixmap, KHRN_IMAGE_WRAP_T *image)
{
   MEM_HANDLE_T handle = MEM_INVALID_HANDLE;
   BEGL_BufferSettings bufferSettings;
   BEGL_DriverInterfaces *driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->BufferGetCreateSettings != NULL))
   {
      BEGL_Error ret;
      ret = driverInterfaces->displayInterface->BufferGetCreateSettings(driverInterfaces->displayInterface->context,
         (BEGL_BufferHandle)pixmap, &bufferSettings);

      if (ret == BEGL_Success)
      {
         uint32_t w, h;
         uint32_t align = 0;
         KHRN_IMAGE_FORMAT_T format;
         uint32_t flags;

         w = bufferSettings.width;
         h = bufferSettings.height;

         flags = IMAGE_FLAG_RENDER_TARGET | IMAGE_FLAG_DISPLAY;

         if (bufferSettings.format == BEGL_BufferFormat_eR5G6B5 ||
             bufferSettings.format == BEGL_BufferFormat_eR5G6B5_Texture)
         {
            format = RGB_565_RSO;
         }
         else if (bufferSettings.format == BEGL_BufferFormat_eYUV422_Texture ||
                  bufferSettings.format == BEGL_BufferFormat_eVUY224_Texture)
         {
            /* Internally, both of these are the same - any endian switching is handled by the platform layer,
               not by the driver. e.g. YUV422 and VUY224 map to different (reversed) Nexus surface formats so the
               blitter will do the swapping, since we can't do it in our h/w. */
            format = YUV_422_RSO;
            flags =  IMAGE_FLAG_TEXTURE | IMAGE_FLAG_RSO_TEXTURE; /* Not a valid render target */
         }
         else if (bufferSettings.format == BEGL_BufferFormat_eA8B8G8R8_Texture ||
                  bufferSettings.format == BEGL_BufferFormat_eR8G8B8A8_Texture)
         {
            /* Internally, both of these are the same - any endian switching is handled by the platform layer,
               not by the driver. e.g. A8B8G8R8 and R8G8B8A8 map to different (reversed) Nexus surface formats so the
               blitter will do the swapping, since we can't do it in our h/w. */
            format = ABGR_8888_RSO;
            flags =  IMAGE_FLAG_TEXTURE | IMAGE_FLAG_RSO_TEXTURE; /* Not a valid render target */
         }
         else if (bufferSettings.format == BEGL_BufferFormat_eA8R8G8B8_Texture ||
                  bufferSettings.format == BEGL_BufferFormat_eB8G8R8A8_Texture)
         {
            /* Internally, both of these are the same - any endian switching is handled by the platform layer,
               not by the driver. e.g. A8B8G8R8 and R8G8B8A8 map to different (reversed) Nexus surface formats so the
               blitter will do the swapping, since we can't do it in our h/w. */
            format = ARGB_8888_RSO;
            flags =  IMAGE_FLAG_TEXTURE | IMAGE_FLAG_RSO_TEXTURE; /* Not a valid render target */
         }
         else
         {
            format = ABGR_8888_RSO;
         }

         khrn_image_platform_fudge(&format, &w, &h, &align, (KHRN_IMAGE_CREATE_FLAG_T)(
            ((flags & IMAGE_FLAG_TEXTURE) ? IMAGE_CREATE_FLAG_TEXTURE : 0) |
            ((flags & IMAGE_FLAG_RSO_TEXTURE) ? IMAGE_CREATE_FLAG_RSO_TEXTURE : 0) |
            ((flags & IMAGE_FLAG_RENDER_TARGET) ? IMAGE_CREATE_FLAG_RENDER_TARGET : 0) |
            ((flags & IMAGE_FLAG_DISPLAY) ? IMAGE_CREATE_FLAG_DISPLAY : 0)));

         /* Check that this looks like a validly sized pixmap buffer */
         if (khrn_image_is_rso(format) &&
            (bufferSettings.pitchBytes == khrn_image_get_stride(format, w)) &&
            (bufferSettings.alignment == align))
         {
            khrn_image_interlock_wrap(image, format, bufferSettings.width, bufferSettings.height,
                            bufferSettings.pitchBytes, flags, bufferSettings.cachedAddr, NULL);
            return true;
         }
      }
   }

   /* EGL_BAD_NATIVE_PIXMAP */
   return false;
}

void platform_get_pixmap_server_handle(EGLNativePixmapType pixmap, uint32_t *handle)
{
  handle[0] = (uint32_t)(uintptr_t)pixmap;
}

bool platform_match_pixmap_api_support(EGLNativePixmapType pixmap, uint32_t api_support)
{
   UNUSED(pixmap);
   UNUSED(api_support);

   return true;
}

uint64_t khronos_platform_get_process_id(void)
{
   return 0;
}

#if EGL_BRCM_global_image && EGL_KHR_image

bool platform_use_global_image_as_egl_image(uint32_t id_0, uint32_t id_1, EGLNativePixmapType pixmap, EGLint *error)
{
   UNUSED(id_0);
   UNUSED(id_1);
   UNUSED(pixmap);
   UNUSED(error);

   vcos_verify(0);
   return false;
}

void platform_acquire_global_image(uint32_t id_0, uint32_t id_1)
{
   UNUSED(id_0);
   UNUSED(id_1);

   vcos_verify(0);
}

void platform_release_global_image(uint32_t id_0, uint32_t id_1)
{
   UNUSED(id_0);
   UNUSED(id_1);

   vcos_verify(0);
}

void platform_get_global_image_info(uint32_t id_0, uint32_t id_1,
   uint32_t *pixel_format, uint32_t *width, uint32_t *height)
{
   UNUSED(id_0);
   UNUSED(id_1);
   UNUSED(pixel_format);
   UNUSED(width);
   UNUSED(height);

   vcos_verify(0);
}

#endif
void platform_client_lock(void)
{
   platform_mutex_acquire(&client_mutex);
}

void platform_client_release(void)
{
   platform_mutex_release(&client_mutex);
}

void platform_init_rpc(struct CLIENT_THREAD_STATE *state) { UNUSED(state); }
void platform_term_rpc(struct CLIENT_THREAD_STATE *state) { UNUSED(state); }
void platform_maybe_free_process(void) { }
void platform_destroy_winhandle(void *a, uint32_t b) { UNUSED(a); UNUSED(b); }

void platform_surface_update(uint32_t handle) { UNUSED(handle); }
void egl_gce_win_change_image(void) { UNREACHABLE(); }


void platform_retrieve_pixmap_completed(EGLNativePixmapType pixmap) { UNUSED(pixmap); UNREACHABLE(); }
void platform_send_pixmap_completed(EGLNativePixmapType pixmap) { UNUSED(pixmap); UNREACHABLE(); }
uint32_t platform_memcmp(const void * aLeft, const void * aRight, size_t aLen) { return khrn_par_memcmp(aLeft, aRight, aLen); }
void platform_memcpy(void * aTrg, const void * aSrc, size_t aLength) { khrn_par_memcpy(aTrg, aSrc, aLength); }

EGLDisplay khrn_platform_set_display_id(EGLNativeDisplayType display_id)
{
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->SetDisplayID != NULL))
   {
      EGLDisplay egl_display_id;
      BEGL_Error res;

      res = driverInterfaces->displayInterface->SetDisplayID(driverInterfaces->displayInterface->context,
         (unsigned int)display_id,
         (unsigned int *)&egl_display_id);
      if (res != BEGL_Success)
         return EGL_NO_DISPLAY;
      return egl_display_id;
   }

   if (display_id == EGL_DEFAULT_DISPLAY)
      return (EGLDisplay)1;
   else
      return EGL_NO_DISPLAY;
}

static int xxx_position = 0;
uint32_t khrn_platform_get_window_position(EGLNativeWindowType win)
{
   return xxx_position;
}

uint32_t platform_get_color_format ( uint32_t format )
{
   uint32_t res = 0;
   BEGL_DriverInterfaces   *driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->GetNativeFormat != NULL))
   {
      BEGL_BufferFormat bufferFormat = BEGL_BufferFormat_INVALID;
      if ((format == ABGR_8888_TF) || (format == ABGR_8888))
         bufferFormat = BEGL_BufferFormat_eA8B8G8R8;
      else if ((format == XBGR_8888_TF) || (format == XBGR_8888))
         bufferFormat = BEGL_BufferFormat_eX8B8G8R8;
      else if ((format == RGB_565_TF) || (format == RGB_565))
         bufferFormat = BEGL_BufferFormat_eR5G6B5;

      driverInterfaces->displayInterface->GetNativeFormat(driverInterfaces->displayInterface->context,
               bufferFormat, &res);
   }

   return res;
}

void khrn_platform_release_pixmap_info(EGLNativePixmapType pixmap, KHRN_IMAGE_WRAP_T *image)
{
   UNUSED(pixmap);
   UNUSED(image);
   /* Nothing to do */
}

void khrn_platform_bind_pixmap_to_egl_image(EGLNativePixmapType pixmap, EGLImageKHR egl_image, bool send)
{
   UNUSED(pixmap);
   UNUSED(egl_image);
   UNUSED(send);
   /* todo */
}

void khrn_platform_unbind_pixmap_from_egl_image(EGLImageKHR egl_image)
{
   UNUSED(egl_image);
   /* todo */
}
