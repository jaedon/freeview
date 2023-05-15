/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Client-side implementation of the EGLImage extensions for EGL:
   EGL_KHR_image
   EGL_KHR_vg_parent_image
   EGL_KHR_gl_texture_2D_image
   EGL_KHR_gl_texture_cubemap_image
=============================================================================*/

#define EGL_EGLEXT_PROTOTYPES /* we want the prototypes so the compiler will check that the signatures match */

#include "interface/khronos/common/khrn_client_mangle.h"

#include "interface/khronos/common/khrn_int_common.h"

#include "interface/khronos/common/khrn_client.h"
#include "interface/khronos/common/khrn_client_rpc.h"

#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"
#include "interface/khronos/include/EGL/eglext_brcm.h"
#include "interface/khronos/include/GLES/gl.h"

#if defined(ANDROID)
#include <android_native_buffer.h>
#include "gralloc_priv.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#endif

#ifdef KHRONOS_CLIENT_LOGGING
#include <stdio.h>
extern FILE *xxx_vclog;
#endif

EGLAPI EGLImageKHR EGLAPIENTRY eglCreateImageKHR (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attr_list)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGLImageKHR result = EGL_NO_IMAGE_KHR;

   CLIENT_LOCK();

   KHRONOS_CLIENT_LOG("eglCreateImageKHR !!!?\n");
   
   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (process) {
         EGL_CONTEXT_T *context;
         bool ctx_error;
         if (target == EGL_NATIVE_PIXMAP_KHR
#ifdef EGL_BRCM_image_wrap
            || target == EGL_IMAGE_WRAP_BRCM
#endif
#ifdef EGL_BRCM_image_wrap_bcg
            || target == EGL_IMAGE_WRAP_BRCM_BCG
#endif
#ifdef ANDROID
            || target == EGL_NATIVE_BUFFER_ANDROID
#endif
            ) {
            context = NULL;
            ctx_error = ctx != EGL_NO_CONTEXT;
         } else {
            context = client_egl_get_context(thread, process, ctx);
            ctx_error = !context;
         }
         if (ctx_error)
            thread->error = EGL_BAD_PARAMETER;
         else {
            uint32_t buf[2];
            KHRN_IMAGE_FORMAT_T buffer_format = IMAGE_FORMAT_INVALID;
            uint32_t buffer_width = 0;
            uint32_t buffer_height = 0;
            uint32_t buffer_stride = 0;
            bool buf_error = false;
            if (target == EGL_NATIVE_PIXMAP_KHR) {
               buf[0] = 0; buf[1] = (uint32_t)(-1);
               platform_get_pixmap_server_handle((EGLNativePixmapType)buffer, buf);
#if EGL_BRCM_global_image
               if ((buf[0] == 0) && (buf[1] == (uint32_t)(-1))) { /* allow either regular or global image server-side pixmaps */
#else
               if ((buf[0] == 0) || (buf[1] != (uint32_t)(-1))) { /* only allow regular server-side pixmaps */
#endif
                  /* This is a client-side pixmap! TODO: implement these properly */
                  KHRN_IMAGE_WRAP_T image;
                  if (platform_get_pixmap_info((EGLNativePixmapType)buffer, &image))
                  {
//meego hack          
                     if(image.aux!=0)
                     {
                        //image.aux refers to a server side EGL surface 
                        //that already contains the data we're interested in
                        buf[0] = (uint32_t)image.aux;
                        target = EGL_IMAGE_FROM_SURFACE_BRCM;
                        khrn_platform_release_pixmap_info((EGLNativePixmapType)buffer, &image);                        
                     }
//                                         
                     else
                     {
                        buf[0] = image.width | image.height << 16;
                        target = EGL_NATIVE_PIXMAP_CLIENT_SIDE_BRCM;
                        khrn_platform_release_pixmap_info((EGLNativePixmapType)buffer, &image);
                     }

                  }
                  else
                  {
                     buf_error = true;
                  }
               }
#if EGL_BRCM_image_wrap
            } else if (target == EGL_IMAGE_WRAP_BRCM) {
               KHRN_IMAGE_WRAP_T *wrap_buffer = (KHRN_IMAGE_WRAP_T *)buffer;

               buf[0] = (uint32_t)wrap_buffer->storage;
               buffer_format = wrap_buffer->format;
               buffer_width = wrap_buffer->width;
               buffer_height = wrap_buffer->height;
               buffer_stride = wrap_buffer->stride;
#endif
#if EGL_BRCM_image_wrap_bcg
            } else if (target == EGL_IMAGE_WRAP_BRCM_BCG) {
               EGL_IMAGE_WRAP_BRCM_BCG_IMAGE_T *wrap_buffer = (EGL_IMAGE_WRAP_BRCM_BCG_IMAGE_T *)buffer;

               buf[0] = (uint32_t)wrap_buffer->storage;
               buffer_width = wrap_buffer->width;
               buffer_height = wrap_buffer->height;
               buffer_stride = wrap_buffer->stride;

               switch(wrap_buffer->format)
               {
                  case BEGL_BufferFormat_eA8B8G8R8_TFormat:          buffer_format = ABGR_8888_TF;          break;
                  case BEGL_BufferFormat_eX8B8G8R8_TFormat:          buffer_format = XBGR_8888_TF;          break;
                  case BEGL_BufferFormat_eR5G6B5_TFormat:            buffer_format = RGB_565_TF;            break;
                  case BEGL_BufferFormat_eR5G5B5A1_TFormat:          buffer_format = RGBA_5551_TF;          break;
                  case BEGL_BufferFormat_eR4G4B4A4_TFormat:          buffer_format = RGBA_4444_TF;          break;

                  case BEGL_BufferFormat_eA8B8G8R8_LTFormat:         buffer_format = ABGR_8888_LT;          break;
                  case BEGL_BufferFormat_eX8B8G8R8_LTFormat:         buffer_format = XBGR_8888_LT;          break;
                  case BEGL_BufferFormat_eR5G6B5_LTFormat:           buffer_format = RGB_565_LT;            break;
                  case BEGL_BufferFormat_eR5G5B5A1_LTFormat:         buffer_format = RGBA_5551_LT;          break;
                  case BEGL_BufferFormat_eR4G4B4A4_LTFormat:         buffer_format = RGBA_4444_LT;          break;

                  default:
                     buf_error = true;
                     break;
               }
#endif
#ifdef ANDROID
            } else if (target == EGL_NATIVE_BUFFER_ANDROID) {
               android_native_buffer_t *android_buffer = (android_native_buffer_t *)buffer;
               vcos_assert(ANDROID_NATIVE_BUFFER_MAGIC == android_buffer->common.magic);
               /* TODO check that handle is a valid gralloc handle */
               /* These are shadow width/height and format, not to be confused with the
                  underlying formats configuration */

               buf[0] = (uint32_t)khrn_hw_unaddr(((struct private_handle_t *)android_buffer->handle)->oglPhysicalAddress);
               /* this needs to be kept to dec the reference count on the image */
               buf[1] = buffer;

               buffer_format = ABGR_8888_TF; /* default to something */
               buffer_width = android_buffer->width;
               buffer_height = android_buffer->height;
               buffer_stride = ((struct private_handle_t *)android_buffer->handle)->oglStride;

               switch (((struct private_handle_t *)android_buffer->handle)->oglFormat)
               {
                  case BEGL_BufferFormat_eA8B8G8R8_TFormat:         buffer_format = ABGR_8888_TF;       break;
                  case BEGL_BufferFormat_eX8B8G8R8_TFormat:         buffer_format = XBGR_8888_TF;       break;
                  case BEGL_BufferFormat_eR5G6B5_TFormat:           buffer_format = RGB_565_TF;         break;
                  case BEGL_BufferFormat_eR5G5B5A1_TFormat:         buffer_format = RGBA_5551_TF;       break;
                  case BEGL_BufferFormat_eR4G4B4A4_TFormat:         buffer_format = RGBA_4444_TF;       break;

                  case BEGL_BufferFormat_eA8B8G8R8_LTFormat:        buffer_format = ABGR_8888_LT;       break;
                  case BEGL_BufferFormat_eX8B8G8R8_LTFormat:        buffer_format = XBGR_8888_LT;       break;
                  case BEGL_BufferFormat_eR5G6B5_LTFormat:          buffer_format = RGB_565_LT;         break;
                  case BEGL_BufferFormat_eR5G5B5A1_LTFormat:        buffer_format = RGBA_5551_LT;       break;
                  case  BEGL_BufferFormat_eR4G4B4A4_LTFormat:       buffer_format = RGBA_4444_LT;       break;

                  default:
                     buf_error = true;
                     break;
               }
               android_buffer->common.incRef(&android_buffer->common);
#endif
            } else {
               buf[0] = (uint32_t)buffer;
            }
            if (buf_error)
               thread->error = EGL_BAD_PARAMETER;
            else {
               EGLint texture_level = 0;
               bool attr_error = false;
               if (attr_list) {
                  while (!attr_error && *attr_list != EGL_NONE) {
                     switch (*attr_list++) {
                     case EGL_GL_TEXTURE_LEVEL_KHR:
                        texture_level = *attr_list++;
                        break;
                     case EGL_IMAGE_PRESERVED_KHR:
                     {
                        EGLint preserved = *attr_list++;
                        if ((preserved != EGL_FALSE) && (preserved != EGL_TRUE)) {
                           attr_error = true;
                        } /* else: ignore the actual value -- we always preserve */
                        break;
                     }
                     default:
                        attr_error = true;
                     }
                  }
               }
               if (attr_error)
                  thread->error = EGL_BAD_PARAMETER;
               else {
#if EGL_BRCM_global_image
                  if ((target == EGL_NATIVE_PIXMAP_KHR) && (buf[1] != (uint32_t)-1)) {
                     if (platform_use_global_image_as_egl_image(buf[0], buf[1], (EGLNativePixmapType)buffer, &thread->error)) {
                        if (!khrn_global_image_map_insert(&process->global_image_egl_images,
                           process->next_global_image_egl_image,
                           buf[0] | ((uint64_t)buf[1] << 32))) {
                           thread->error = EGL_BAD_ALLOC;
                        } else {
                           result = (EGLImageKHR)(uintptr_t)process->next_global_image_egl_image;
                           thread->error = EGL_SUCCESS;
                           do {
                              process->next_global_image_egl_image = (1 << 31) |
                                 (process->next_global_image_egl_image + 1);
                           } while (khrn_global_image_map_lookup(&process->global_image_egl_images,
                              process->next_global_image_egl_image));
                        }
                     }
                  } else
#endif
                  {
                     EGLint results[2];
                     RPC_CALL11_OUT_CTRL(eglCreateImageKHR_impl,
                        thread,
                        EGLCREATEIMAGEKHR_ID,
                        RPC_UINT(context ? (context->type == OPENGL_ES_20 ? 2 : 1) : 0),
                        RPC_UINT(context ? context->servercontext : 0),
                        RPC_ENUM(target),
                        RPC_UINT(buf[0]),
                        RPC_UINT(buf[1]),
                        RPC_UINT(buffer_format),
                        RPC_UINT(buffer_width),
                        RPC_UINT(buffer_height),
                        RPC_UINT(buffer_stride),
                        RPC_INT(texture_level),
                        results);

                     result = (EGLImageKHR)(intptr_t)results[0];
                     thread->error = results[1];

                     if (target == EGL_NATIVE_PIXMAP_CLIENT_SIDE_BRCM || target == EGL_IMAGE_FROM_SURFACE_BRCM)
                     {
                        khrn_platform_bind_pixmap_to_egl_image((EGLNativePixmapType)buffer, result, target == EGL_NATIVE_PIXMAP_CLIENT_SIDE_BRCM);
                     }
                  }
               }
            }
         }
      }
   }

   CLIENT_UNLOCK();

   return result;
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyImageKHR (EGLDisplay dpy, EGLImageKHR image)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGLBoolean result;

   KHRONOS_CLIENT_LOG("eglDestroyImageKHR image=%d.\n", (int)image);
   
   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (!process)
         result = EGL_FALSE;
      else {
         khrn_platform_unbind_pixmap_from_egl_image(image);
#if EGL_BRCM_global_image
         if ((uintptr_t)image & (1 << 31)) {
            result = khrn_global_image_map_delete(&process->global_image_egl_images, (uint32_t)(uintptr_t)image) ?
               EGL_TRUE : EGL_FALSE;
         } else
#endif
         {
#ifdef ANDROID
            android_native_buffer_t *android_buffer;
#endif
            uint32_t abstract_buffer;
            result = RPC_BOOLEAN_RES(RPC_CALL2_OUT_CTRL_RES(eglDestroyImageKHR_impl,
               thread,
               EGLDESTROYIMAGEKHR_ID,
               RPC_EGLID(image),
               &abstract_buffer));
#ifdef ANDROID
            android_buffer = (android_native_buffer_t *)abstract_buffer;
            /* if android creates any type of window other than EGL_NATIVE_BUFFER_ANDROID then it could be
               NULL.  The refsw implementation doesnt do this, so its probably not absolutely necessary */
            if (android_buffer != NULL)
            {
               vcos_assert(ANDROID_NATIVE_BUFFER_MAGIC == android_buffer->common.magic);
               android_buffer->common.decRef(&android_buffer->common);
            }
#endif
         }

         if (!result) {
            thread->error = EGL_BAD_PARAMETER;
         }
      }
   }

   CLIENT_UNLOCK();

   return result;
}
