
#include "interface/vcos/vcos.h"

#include "middleware/khronos/egl/egl_platform.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "helpers/vc_image/vc_image.h"
#include "interface/khronos/common/khrn_client_platform.h"

#include <stdio.h>
#include <assert.h>

#ifdef ANDROID
#include <cutils/log.h>
#endif


static EGL_SERVER_RETURN_CALLBACK_T s_eglCallback;

void vsync_callback(unsigned int cb_arg)
{
   s_eglCallback(cb_arg);
}

extern BEGL_DriverInterfaces * BEGL_GetDriverInterfaces(void);

EGLAPI void EGLAPIENTRY BEGLint_BufferGetRequirements(BEGL_PixmapInfo *bufferRequirements, BEGL_BufferSettings * bufferConstrainedRequirements)
{
   KHRN_IMAGE_CREATE_FLAG_T flags;
   KHRN_IMAGE_FORMAT_T format;
   uint32_t align, wd, ht;
   uint32_t overrun;
   /* needs to be max of CPU cache line and the GCACHE on the v3d */
   uint32_t cacheline_size = mem_cacheline_size();
   cacheline_size = (cacheline_size > BCG_GCACHE_LINE_SIZE) ? cacheline_size : BCG_GCACHE_LINE_SIZE;

   memset(bufferConstrainedRequirements, 0, sizeof(BEGL_BufferSettings));

   switch (bufferRequirements->format)
   {
   case BEGL_BufferFormat_eA8B8G8R8:
   case BEGL_BufferFormat_eX8B8G8R8:
      format = ABGR_8888_RSO;
      flags = IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eR8G8B8A8:
   case BEGL_BufferFormat_eR8G8B8X8:
      format = RGBA_8888_RSO;
      flags = IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eR5G6B5:
      format = RGB_565_RSO;
      flags = IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eA8B8G8R8_TFormat:
      format = ABGR_8888_TF;
      flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eX8B8G8R8_TFormat:
      format = XBGR_8888_TF;
      flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eR5G6B5_TFormat:
      format = RGB_565_TF;
      flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;
      break;
   case BEGL_BufferFormat_eR5G5B5A1_TFormat:
      format = RGBA_5551_TF;
      flags = IMAGE_CREATE_FLAG_TEXTURE;
      break;
   case BEGL_BufferFormat_eR4G4B4A4_TFormat:
      format = RGBA_4444_TF;
      flags = IMAGE_CREATE_FLAG_TEXTURE;
      break;
   default:
      assert(0);
      format = IMAGE_FORMAT_INVALID;
   }

   wd = bufferRequirements->width;
   ht = bufferRequirements->height;
   align = 0;

   khrn_image_platform_fudge(&format, &wd, &ht, &align, flags);

   if (khrn_image_is_lineartile(format))
   {
      switch (format)
      {
      case ABGR_8888_LT:
         bufferConstrainedRequirements->format = BEGL_BufferFormat_eA8B8G8R8_LTFormat;
         break;
      case XBGR_8888_LT:
         bufferConstrainedRequirements->format = BEGL_BufferFormat_eX8B8G8R8_LTFormat;
         break;
      case RGB_565_LT:
         bufferConstrainedRequirements->format = BEGL_BufferFormat_eR5G6B5_LTFormat;
         break;
      case RGBA_5551_LT:
         bufferConstrainedRequirements->format = BEGL_BufferFormat_eR5G5B5A1_LTFormat;
         break;
      case RGBA_4444_LT:
         bufferConstrainedRequirements->format = BEGL_BufferFormat_eR4G4B4A4_LTFormat;
         break;
      }
   }
   else
   {
      bufferConstrainedRequirements->format = bufferRequirements->format;
   }

   bufferConstrainedRequirements->width = wd;
   bufferConstrainedRequirements->height = ht;
   bufferConstrainedRequirements->alignment = (align > cacheline_size) ? align : cacheline_size;
   bufferConstrainedRequirements->pitchBytes = khrn_image_get_stride(format, wd);
   bufferConstrainedRequirements->totalByteSize = khrn_image_get_size(format, wd, ht);
   /* make sure the last line can always complete a tile line (with the enables set) */
   overrun = (KHRN_HW_TILE_WIDTH - (wd % KHRN_HW_TILE_WIDTH)) * (khrn_image_get_bpp(format) / 8);
   bufferConstrainedRequirements->totalByteSize += overrun;
   /* align to a cache line */
   bufferConstrainedRequirements->totalByteSize = (bufferConstrainedRequirements->totalByteSize + (cacheline_size - 1)) & ~(cacheline_size - 1);
}

void egl_server_platform_display(uint32_t win, KHRN_IMAGE_T *image, uint32_t cb_arg)
{
   BEGL_BufferDisplayState bufferDisplayState;

#ifdef NULL_DRIVER
   s_eglCallback(cb_arg);
#else
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   memset(&bufferDisplayState, 0, sizeof(BEGL_BufferDisplayState));

   bufferDisplayState.buffer = (BEGL_BufferHandle)image->opaque_buffer_handle;
   bufferDisplayState.cb_arg = cb_arg;
   vcos_assert(image->window_state != NULL);
   bufferDisplayState.windowState = *image->window_state;

   /* post the unlock */
   platform_unlock(image->opaque_buffer_handle, image->window_state);

   /* post display */
   if ((driverInterfaces != NULL) &&
         (driverInterfaces->displayInterface != NULL) &&
         (driverInterfaces->displayInterface->BufferDisplay != NULL))
   {
      driverInterfaces->displayInterface->BufferDisplay(driverInterfaces->displayInterface->context, &bufferDisplayState);
   }

#endif
}

void egl_server_platform_display_nothing_sync(uint32_t win, BEGL_WindowState *windowState)
{
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
       (driverInterfaces->displayInterface != NULL) &&
       (driverInterfaces->displayInterface->WindowUndisplay != NULL))
   {
      driverInterfaces->displayInterface->WindowUndisplay(driverInterfaces->displayInterface->context, windowState);
   }
}

MEM_HANDLE_T egl_server_platform_create_pixmap_info(uint32_t pixmap, EGLint *error)
{
   MEM_HANDLE_T handle = MEM_INVALID_HANDLE;
   BEGL_BufferSettings bufferSettings;
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   memset(&bufferSettings, 0, sizeof(BEGL_BufferSettings));

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
         MEM_HANDLE_T wrapHandle;
         KHRN_IMAGE_FORMAT_T format = IMAGE_FORMAT_INVALID;
         KHRN_IMAGE_CREATE_FLAG_T flags;

         w = bufferSettings.width;
         h = bufferSettings.height;

         flags = IMAGE_CREATE_FLAG_RENDER_TARGET | IMAGE_CREATE_FLAG_DISPLAY;

         switch (bufferSettings.format)
         {
         case BEGL_BufferFormat_eR5G6B5:
            format = RGB_565_RSO;
            break;
         case BEGL_BufferFormat_eR5G6B5_Texture:
            format = RGB_565_RSO;
            flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RSO_TEXTURE;  /* Not a valid render target */
            break;
         case BEGL_BufferFormat_eYUV422_Texture:
         case BEGL_BufferFormat_eVUY224_Texture:
            if (!khrn_workarounds.HW2898 || is_power_of_2(w))
            {
               format = YUV_422_RSO;
               flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RSO_TEXTURE;  /* Not a valid render target */
            }
            break;
         case BEGL_BufferFormat_eA8B8G8R8_Texture:
         case BEGL_BufferFormat_eR8G8B8A8_Texture:
            {
               format = ABGR_8888_RSO;
               flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RSO_TEXTURE;  /* Not a valid render target */
            }
            break;
         case BEGL_BufferFormat_eA8R8G8B8_Texture:
         case BEGL_BufferFormat_eB8G8R8A8_Texture:
            {
               format = ARGB_8888_RSO;
               flags = IMAGE_CREATE_FLAG_TEXTURE | IMAGE_CREATE_FLAG_RSO_TEXTURE;  /* Not a valid render target */
            }
            break;
         case BEGL_BufferFormat_eA8B8G8R8:
            format = ABGR_8888_RSO;
            break;
         case BEGL_BufferFormat_eR8G8B8A8:
            format = RGBA_8888_RSO;
            break;
         case BEGL_BufferFormat_eX8B8G8R8:
            format = XBGR_8888_RSO;
            break;
         case BEGL_BufferFormat_eR8G8B8X8:
            format = RGBX_8888_RSO;
            break;
         case BEGL_BufferFormat_eA8B8G8R8_TFormat:
            format = ABGR_8888_TF;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eX8B8G8R8_TFormat:
            format = XBGR_8888_TF;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR5G6B5_TFormat:
            format = RGB_565_TF;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR5G5B5A1_TFormat:
            format = RGBA_5551_TF;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR4G4B4A4_TFormat:
            format = RGBA_4444_TF;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eA8B8G8R8_LTFormat:
            format = RGBA_8888_LT;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eX8B8G8R8_LTFormat:
            format = XBGR_8888_LT;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR5G6B5_LTFormat:
            format = RGB_565_LT;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR5G5B5A1_LTFormat:
            format = RGBA_5551_LT;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         case BEGL_BufferFormat_eR4G4B4A4_LTFormat:
            format = RGBA_4444_LT;
            flags |= IMAGE_CREATE_FLAG_TEXTURE;
            break;
         default:
            /* can come here if format is not supported */
            break;
         }

         /* buffer must be 4kb aligned or it will cause HW errors */
         if ((((uint32_t)bufferSettings.cachedAddr & ~0xFFF) == (uint32_t)bufferSettings.cachedAddr) &&
             (((uint32_t)bufferSettings.physOffset & ~0xFFF) == (uint32_t)bufferSettings.physOffset) &&
             (format != IMAGE_FORMAT_INVALID))
         {
            khrn_image_platform_fudge(&format, &w, &h, &align, flags);

            wrapHandle = mem_wrap((void *)bufferSettings.cachedAddr, bufferSettings.pitchBytes * h, align, MEM_FLAG_DIRECT, "wrapped pixmap");

#ifdef ANDROID
            LOGD("%s[%d] Bs.width[w]:%x[%x] BsHeight[h]:%x[%x] BspitchBytes:%x[%x] format:%x ",
            __FUNCTION__,__LINE__,
            bufferSettings.width,w,
            bufferSettings.height,h,
            bufferSettings.pitchBytes,
            khrn_image_get_stride(format, w),format);
#endif
            handle = khrn_image_create_from_storage(format, bufferSettings.width, bufferSettings.height, bufferSettings.pitchBytes,
                                                   MEM_INVALID_HANDLE, wrapHandle, 0, 
                                                   flags);

            /* Note: Don't store the opaque buffer handle here, as we may not always want to destroy it.
               * We'll store it explicitly for our swap chain created buffers. Remember, this is used for pixmaps too.*/

            mem_release(wrapHandle);
            if (handle == MEM_INVALID_HANDLE)
               *error = EGL_BAD_ALLOC;
            else
               *error = EGL_SUCCESS;
         }
         else
         {
            *error = EGL_BAD_NATIVE_PIXMAP;
         }
      }
   }

   return handle;
}

void egl_server_platform_init(EGL_SERVER_RETURN_CALLBACK_T return_callback)
{
   s_eglCallback = return_callback;
}

void egl_server_platform_set_position(uint32_t handle, uint32_t position, uint32_t width, uint32_t height)
{
   UNUSED(handle);
   UNUSED(position);
   UNUSED(width);
   UNUSED(height);
   /* todo, resize swap chain */
}

uint32_t egl_server_platform_get_window_position(EGLNativeWindowType win)
{
   UNUSED(win);
   return 0;
}

void BRCM_GetDefaultNativeWindowSettings(EGLNativeWindowType nwt)
{
   UNUSED(nwt);
   /* todo - remove me */
}

EGLDisplay egl_server_platform_default_display(void)
{
   /* return the first display */
   return (EGLDisplay)1;
}

bool egl_server_platform_validate_display(EGLDisplay display)
{
   return (display == (EGLDisplay)1);
}

void egl_server_platform_set_display(EGLDisplay dpy, EGLNativeWindowType win)
{
   UNUSED(dpy);
   UNUSED(win);
}

BEGL_WindowState *egl_server_platform_create_window_state(uint32_t window)
{
   BEGL_WindowState *platformState = NULL;
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   platformState = (BEGL_WindowState *)malloc(sizeof(BEGL_WindowState));
   memset(platformState, 0, sizeof(BEGL_WindowState));
   platformState->window = (BEGL_WindowHandle)window;

   if ((driverInterfaces != NULL) &&
      (driverInterfaces->displayInterface != NULL) &&
      (driverInterfaces->displayInterface->WindowPlatformStateCreate != NULL))
   {
      if (platformState)
      {
         platformState->platformState = driverInterfaces->displayInterface->WindowPlatformStateCreate(
                                                driverInterfaces->displayInterface->context, (BEGL_WindowHandle)window);
      }
   }

   return platformState;
}




uint32_t egl_server_platform_create_buffer(
   KHRN_IMAGE_FORMAT_T format,
   uint32_t width, uint32_t height,
   KHRN_IMAGE_CREATE_FLAG_T flags,
   BEGL_WindowState *platformState,
   BEGL_BufferUsage usage)
{
   BEGL_BufferHandle bufHandle = 0;
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
      (driverInterfaces->displayInterface != NULL) &&
      (driverInterfaces->displayInterface->BufferCreate != NULL))
   {
      uint32_t padded_width = width;
      uint32_t padded_height = height;
      uint32_t align = 64;
      uint32_t overrun;
      BEGL_BufferSettings bufferSettings;
      bool rso_image;
      /* needs to be max of CPU cache line and the GCACHE on the v3d */
      uint32_t cacheline_size = mem_cacheline_size();
      cacheline_size = (cacheline_size > BCG_GCACHE_LINE_SIZE) ? cacheline_size : BCG_GCACHE_LINE_SIZE;

      vcos_assert(format != IMAGE_FORMAT_INVALID);

      khrn_image_platform_fudge(&format, &padded_width, &padded_height, &align, flags);
      rso_image = khrn_image_is_rso(format);

      memset(&bufferSettings, 0, sizeof(BEGL_BufferSettings));

      bufferSettings.usage = usage;
      
      if (platformState)
         bufferSettings.windowState = *((BEGL_WindowState *)platformState);

      bufferSettings.alignment = (align > cacheline_size) ? align : cacheline_size;
      bufferSettings.width = width;
      bufferSettings.height = height;
      bufferSettings.pitchBytes = khrn_image_get_stride(format, padded_width);
      bufferSettings.totalByteSize = khrn_image_get_size(format, padded_width, padded_height);
      /* make sure the last line can always complete a tile line (with the enables set) */
      overrun = (KHRN_HW_TILE_WIDTH - (padded_width % KHRN_HW_TILE_WIDTH)) * (khrn_image_get_bpp(format) / 8);
      bufferSettings.totalByteSize += overrun;
      /* align to a cache line */
      bufferSettings.totalByteSize = (bufferSettings.totalByteSize + (cacheline_size - 1)) & ~(cacheline_size - 1);
#if defined(BIG_ENDIAN_CPU)
      bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR8G8B8A8 : BEGL_BufferFormat_eA8B8G8R8_TFormat;
#else
      bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eA8B8G8R8 : BEGL_BufferFormat_eA8B8G8R8_TFormat;
#endif

      if (khrn_image_get_bpp(format) == 16)
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR5G6B5 : BEGL_BufferFormat_eR5G6B5_TFormat;
      else if (khrn_image_get_alpha_size(format) == 0)
      {
#if defined(BIG_ENDIAN_CPU)
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR8G8B8X8 : BEGL_BufferFormat_eX8B8G8R8_TFormat;
#else
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eX8B8G8R8 : BEGL_BufferFormat_eX8B8G8R8_TFormat;
#endif
      }

      bufHandle = driverInterfaces->displayInterface->BufferCreate(driverInterfaces->displayInterface->context,
                                                                   &bufferSettings);
   }

   return (uint32_t)bufHandle;
}



uint32_t egl_server_platform_get_buffer(
   EGLNativeWindowType win,
   KHRN_IMAGE_FORMAT_T format,
   uint32_t width, uint32_t height,
   KHRN_IMAGE_CREATE_FLAG_T flags,
   BEGL_WindowState *platformState,
   BEGL_BufferUsage usage)
{
   BEGL_BufferHandle bufHandle = 0;
   BEGL_DriverInterfaces * driverInterfaces = BEGL_GetDriverInterfaces();

   if ((driverInterfaces != NULL) &&
      (driverInterfaces->displayInterface != NULL) &&
      (driverInterfaces->displayInterface->BufferGet != NULL))
   {
      uint32_t padded_width = width;
      uint32_t padded_height = height;
      uint32_t align = 64;
      uint32_t overrun;
      bool rso_image;
      BEGL_BufferSettings bufferSettings;
      /* needs to be max of CPU cache line and the GCACHE on the v3d */
      uint32_t cacheline_size = mem_cacheline_size();
      cacheline_size = (cacheline_size > BCG_GCACHE_LINE_SIZE) ? cacheline_size : BCG_GCACHE_LINE_SIZE;

      khrn_image_platform_fudge(&format, &padded_width, &padded_height, &align, flags);
      rso_image = khrn_image_is_rso(format);

      memset(&bufferSettings, 0, sizeof(BEGL_BufferSettings));
      bufferSettings.usage = usage;
      if (platformState)
         bufferSettings.windowState = *platformState;

      bufferSettings.alignment = (align > cacheline_size) ? align : cacheline_size;
      bufferSettings.width = width;
      bufferSettings.height = height;
      bufferSettings.pitchBytes = khrn_image_get_stride(format, padded_width);
      bufferSettings.totalByteSize = khrn_image_get_size(format, padded_width, padded_height);
      /* make sure the last line can always complete a tile line (with the enables set) */
      overrun = (KHRN_HW_TILE_WIDTH - (padded_width % KHRN_HW_TILE_WIDTH)) * (khrn_image_get_bpp(format) / 8);
      bufferSettings.totalByteSize += overrun;
      /* align to a cache line */
      bufferSettings.totalByteSize = (bufferSettings.totalByteSize + (cacheline_size - 1)) & ~(cacheline_size - 1);
#if defined(BIG_ENDIAN_CPU)
      bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR8G8B8A8 : BEGL_BufferFormat_eA8B8G8R8_TFormat;
#else
      bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eA8B8G8R8 : BEGL_BufferFormat_eA8B8G8R8_TFormat;
#endif

      if (khrn_image_get_bpp(format) == 16)
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR5G6B5 : BEGL_BufferFormat_eR5G6B5_TFormat;
      else if (khrn_image_get_alpha_size(format) == 0)
      {
#if defined(BIG_ENDIAN_CPU)
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eR8G8B8X8 : BEGL_BufferFormat_eX8B8G8R8_TFormat;
#else
         bufferSettings.format = (rso_image) ? BEGL_BufferFormat_eX8B8G8R8 : BEGL_BufferFormat_eX8B8G8R8_TFormat;
#endif
      }

      bufHandle = driverInterfaces->displayInterface->BufferGet(driverInterfaces->displayInterface->context,
                                                                &bufferSettings);

      /*
      printf("cachedAddr     = %p\n"
             "physOffset     = %p\n"
             "width          = %d\n"
             "height         = %d\n"
             "pitchBytes     = %d\n"
             "totalByteSize  = %d\n"
             "alignment      = %d\n"
             "format         = %d\n"
             "usage          = %d\n",
             bufferSettings.cachedAddr,
             bufferSettings.physOffset,
             bufferSettings.width,
             bufferSettings.height,
             bufferSettings.pitchBytes,
             bufferSettings.totalByteSize,
             bufferSettings.alignment,
             bufferSettings.format,
             bufferSettings.usage);
      */
   }

   return (uint32_t)bufHandle;
}


void egl_server_platform_destroy_buffer(uint32_t bufHandle, BEGL_WindowState *windowState)
{
   if (bufHandle != 0)
   {
      /* Destroy the abstract buffer */
      BEGL_DriverInterfaces *driverInterfaces = BEGL_GetDriverInterfaces();
      if (driverInterfaces != NULL && 
          driverInterfaces->displayInterface != NULL && 
          driverInterfaces->displayInterface->BufferDestroy != NULL)
      {
         BEGL_BufferDisplayState state;
         memset(&state, 0, sizeof(BEGL_BufferDisplayState));
         state.buffer = (BEGL_BufferHandle)bufHandle;
         state.cb_arg = -1;
         if (windowState != NULL)
         {
            state.windowState = *windowState;
         }

         driverInterfaces->displayInterface->BufferDestroy(driverInterfaces->displayInterface->context, &state);
      }
   }
}
