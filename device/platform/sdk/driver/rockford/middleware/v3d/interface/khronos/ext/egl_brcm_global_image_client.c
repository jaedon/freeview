/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  EGL Broadcom global image extension

FILE DESCRIPTION
Client-side implementation of EGL_BRCM_global_image.
=============================================================================*/

#define EGL_EGLEXT_PROTOTYPES /* we want the prototypes so the compiler will check that the signatures match */

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_client.h"
#include "interface/khronos/common/khrn_client_rpc.h"
#include "interface/khronos/egl/egl_client_config.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"

#if EGL_BRCM_global_image

static EGLint get_bytes_per_pixel(EGLint pixel_format) /* returns 0 for invalid pixel formats */
{
   switch (pixel_format & ~EGL_PIXEL_FORMAT_USAGE_MASK_BRCM) {
   case EGL_PIXEL_FORMAT_ARGB_8888_PRE_BRCM: return 4;
   case EGL_PIXEL_FORMAT_ARGB_8888_BRCM:     return 4;
   case EGL_PIXEL_FORMAT_XRGB_8888_BRCM:     return 4;
   case EGL_PIXEL_FORMAT_RGB_565_BRCM:       return 2;
   case EGL_PIXEL_FORMAT_A_8_BRCM:           return 1;
   default:                                  return 0; /* invalid */
   }
}

/*
   failure is indicated by (!id[0] && !id[1]). call eglGetError for the error code

   possible failures:
   - width/height <= 0 or > EGL_CONFIG_MAX_WIDTH/EGL_CONFIG_MAX_HEIGHT (EGL_BAD_PARAMETER)
   - pixel_format invalid (EGL_BAD_PARAMETER)
   - insufficient resources (EGL_BAD_ALLOC)

   data may be NULL (in which case the image contents are undefined)
*/

EGLAPI void EGLAPIENTRY eglCreateGlobalImageBRCM(EGLint width, EGLint height, EGLint pixel_format, const void *data, EGLint data_stride, EGLint *id)
{
   EGLint bytes_per_pixel;

   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   /*
      check params
   */

   bytes_per_pixel = get_bytes_per_pixel(pixel_format);
   if ((width <= 0) || (width > EGL_CONFIG_MAX_WIDTH) ||
      (height <= 0) || (height > EGL_CONFIG_MAX_HEIGHT) ||
      (bytes_per_pixel == 0)) {
      thread->error = EGL_BAD_PARAMETER;
      id[0] = 0; id[1] = 0;
      return;
   }

   /*
      create the image
   */

   RPC_CALL4_OUT_CTRL(eglCreateGlobalImageBRCM_impl,
                      thread,
                      EGLCREATEGLOBALIMAGEBRCM_ID,
                      RPC_INT(width),
                      RPC_INT(height),
                      RPC_INT(pixel_format),
                      id);
   if (!id[0] && !id[1]) {
      thread->error = EGL_BAD_ALLOC;
      return;
   }

   /*
      fill the image in if necessary (this can't fail)
   */

   if (data) {
         eglFillGlobalImageBRCM_impl(id[0], id[1], 0, height, data, data_stride, pixel_format);
   }
}

/*
   failure is indicated by (!id[0] && !id[1]). call eglGetError for the error code

   possible failures:
   - src_id invalid (EGL_BAD_PARAMETER)
   - insufficient resources (EGL_BAD_ALLOC)
*/

EGLAPI void EGLAPIENTRY eglCreateCopyGlobalImageBRCM(const EGLint *src_id, EGLint *id)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   /*
      create the image
   */

   RPC_CALL3_OUT_CTRL(eglCreateCopyGlobalImageBRCM_impl,
                      thread,
                      EGLCREATECOPYGLOBALIMAGEBRCM_ID,
                      RPC_INT(src_id[0]),
                      RPC_INT(src_id[1]),
                      id);
   if (!id[0] && !id[1]) { /* not enough memory */
      thread->error = EGL_BAD_ALLOC;
   }
   if ((id[0] == -1) && (id[1] == -1)) { /* src_id was invalid */
      thread->error = EGL_BAD_PARAMETER;
      id[0] = 0; id[1] = 0;
   }
}

/*
   failure is indicated by returning EGL_FALSE. call eglGetError for the error code

   possible failures:
   - id invalid (EGL_BAD_PARAMETER)
*/

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyGlobalImageBRCM(const EGLint *id)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   /*
      destroy the image
   */

   if (!RPC_BOOLEAN_RES(RPC_CALL2_RES(eglDestroyGlobalImageBRCM_impl,
                                      thread,
                                      EGLDESTROYGLOBALIMAGEBRCM_ID,
                                      RPC_INT(id[0]),
                                      RPC_INT(id[1])))) {
      thread->error = EGL_BAD_PARAMETER;
      return EGL_FALSE;
   }

   return EGL_TRUE;
}

/*
   failure is indicated by returning EGL_FALSE. call eglGetError for the error code

   possible failures:
   - id invalid (EGL_BAD_PARAMETER)
*/

EGLAPI EGLBoolean EGLAPIENTRY eglQueryGlobalImageBRCM(const EGLint *id, EGLint *width_height_pixel_format)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   if (!RPC_BOOLEAN_RES(RPC_CALL3_OUT_CTRL_RES(eglQueryGlobalImageBRCM_impl,
                                               thread,
                                               EGLQUERYGLOBALIMAGEBRCM_ID,
                                               RPC_INT(id[0]),
                                               RPC_INT(id[1]),
                                               width_height_pixel_format))) {
      thread->error = EGL_BAD_PARAMETER;
      return EGL_FALSE;
   }

   return EGL_TRUE;
}

#endif
