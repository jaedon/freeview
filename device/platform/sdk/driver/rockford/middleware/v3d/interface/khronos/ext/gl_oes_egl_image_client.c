/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Client-side implementation of the GL_OES_EGL_image extension.
=============================================================================*/

#define GL_GLEXT_PROTOTYPES /* we want the prototypes so the compiler will check that the signatures match */

#include "interface/khronos/common/khrn_client_mangle.h"

#include "interface/khronos/common/khrn_int_common.h"

#include "interface/khronos/glxx/glxx_client.h"
#include "interface/khronos/common/khrn_client_rpc.h"

#include "interface/khronos/glxx/glxx_int_impl.h"
#include "interface/khronos/glxx/gl20_int_impl.h"

#include "interface/khronos/include/GLES2/gl2.h"
#include "interface/khronos/include/GLES2/gl2ext.h"

GL_API void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
#if EGL_BRCM_global_image
   /* todo: if ((uintptr_t)image & (1 << 31)) */
#endif

   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glEGLImageTargetTexture2DOES_impl,
                thread,
                GLEGLIMAGETARGETTEXTURE2DOES_ID,
                RPC_ENUM(target),
                RPC_EGLID(image));
   }
}

GL_API void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
#if EGL_BRCM_global_image
   /* todo: if ((uintptr_t)image & (1 << 31)) */
#endif

   if (IS_OPENGLES_11(thread)) {
      /* OES_framebuffer_object not supported for GLES1.1 */
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      if (state->error == GL_NO_ERROR)
         state->error = GL_INVALID_OPERATION;
   }
   else if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glEGLImageTargetRenderbufferStorageOES_impl_20,
                thread,
                GLEGLIMAGETARGETRENDERBUFFERSTORAGEOES_ID_20,
                RPC_ENUM(target),
                RPC_EGLID(image));
   }
}
