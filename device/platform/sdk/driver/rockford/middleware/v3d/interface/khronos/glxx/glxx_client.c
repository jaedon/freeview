/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
OpenGL client-side function declarations. Dispatches GL calls to either 1.1 or 2.0
handlers via RPC or direct call.
=============================================================================*/

#include "interface/khronos/common/khrn_client_mangle.h"

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_options.h"

#include "interface/khronos/glxx/glxx_client.h"
#include "interface/khronos/glxx/gl11_int_config.h"
#include "interface/khronos/include/GLES/glext.h"
#include "interface/khronos/include/GLES2/gl2ext.h"

#include "interface/khronos/glxx/gl11_int_impl.h"
#include "interface/khronos/glxx/gl20_int_impl.h"
#include "interface/khronos/glxx/glxx_int_impl.h"
#include "interface/khronos/common/khrn_int_misc_impl.h"

#include "interface/khronos/common/khrn_client_rpc.h"
#include "interface/khronos/common/khrn_int_util.h"
//#include "../khronos.h"

#ifdef RPC_DELAYED_USE_OF_POINTERS
#include "middleware/khronos/common/khrn_hw.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef __HIGHC__
#pragma warning( disable : 4100 4127 4204)             // unreferenced formal parameter, constant conditional expression, non-constant initializer
#endif

#ifdef GL11_CLIENT_SINGLE
GLXX_CLIENT_STATE_T gl11_client_state;
#endif

#ifdef GL20_CLIENT_SINGLE
GLXX_CLIENT_STATE_T gl20_client_state;
#endif

#define SERIALIZE_ATTRIB(b, x, k) RPC_INT((x).size), \
                              RPC_ENUM((x).type), \
                              RPC_BOOLEAN((x).normalized), \
                              RPC_SIZEI((x).stride), \
                              RPC_UINT((b ? (uint32_t)(k + offsetof(CACHE_ENTRY_T, data)) : (uint32_t)(uintptr_t)(x).pointer)), \
                              RPC_UINT((x).buffer)

#define SERIALIZE_ATTRIB_VALUE(x) RPC_FLOAT((x).value[0]), \
                                  RPC_FLOAT((x).value[1]), \
                                  RPC_FLOAT((x).value[2]), \
                                  RPC_FLOAT((x).value[3])

#ifdef DISABLE_OPTION_PARSING
static void set_error(GLXX_CLIENT_STATE_T *state, GLenum error)
{
   if (state->error == GL_NO_ERROR)
      state->error = error;
}
#else
static void set_error_ex(GLXX_CLIENT_STATE_T *state, GLenum error, const char *func, unsigned int line)
{
   khrn_error_assist(error, func, line);

   if (state->error == GL_NO_ERROR)
      state->error = error;
}
#define set_error(a, b) set_error_ex(a, b, __func__, __LINE__)
#endif

GL_API void GL_APIENTRY glActiveTexture (GLenum texture)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (texture >= GL_TEXTURE0 && texture < GL_TEXTURE0 + GL11_CONFIG_MAX_TEXTURE_UNITS)
         state->active_texture.server = texture;

      RPC_CALL1(glActiveTexture_impl,
                thread,
                GLACTIVETEXTURE_ID,
                RPC_ENUM(texture));
   }

   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glActiveTexture_impl,
                thread,
                GLACTIVETEXTURE_ID,
                RPC_ENUM(texture));
   }
}

GL_API void GL_APIENTRY glAlphaFunc (GLenum func, GLclampf ref)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glAlphaFunc_impl_11,
                thread,
                GLALPHAFUNC_ID_11,
                RPC_ENUM(func),
                RPC_FLOAT(ref));
   }
}

GL_API void GL_APIENTRY glAlphaFuncx (GLenum func, GLclampx ref)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glAlphaFuncx_impl_11,
                thread,
                GLALPHAFUNCX_ID_11,
                RPC_ENUM(func),
                RPC_FIXED(ref));
   }
}

GL_API void GL_APIENTRY glAttachShader (GLuint program, GLuint shader)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glAttachShader_impl_20,
                thread,
                GLATTACHSHADER_ID_20,
                RPC_UINT(program),
                RPC_UINT(shader));
   }
}

GL_API void GL_APIENTRY glBindAttribLocation (GLuint program, GLuint index, const char *name)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3_IN_BULK(glBindAttribLocation_impl_20,
                        thread,
                        GLBINDATTRIBLOCATION_ID_20,
                        RPC_UINT(program),
                        RPC_UINT(index),
                        name,
                        strlen(name) + 1);
   }
}

GL_API void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   if(IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      vcos_assert(state != NULL);

      switch (target) {
      case GL_ARRAY_BUFFER:
         state->bound_buffer.array = buffer;
         break;
      case GL_ELEMENT_ARRAY_BUFFER:
         state->bound_buffer.element_array = buffer;
         break;
      default:
         // do nothing, server will signal error
         break;
      }

      RPC_CALL2(glBindBuffer_impl,
                thread,
                GLBINDBUFFER_ID,
                RPC_ENUM(target),
                RPC_UINT(buffer));
   }

}

GL_API void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glBindTexture_impl,
                thread,
                GLBINDTEXTURE_ID,
                RPC_ENUM(target),
                RPC_UINT(texture));
   }
}

GL_API void GL_APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL4(glBlendColor_impl_20,
                thread,
                GLBLENDCOLOR_ID_20,
                RPC_FLOAT(red),
                RPC_FLOAT(green),
                RPC_FLOAT(blue),
                RPC_FLOAT(alpha));
   }
}

GL_API void GL_APIENTRY glBlendEquation( GLenum mode ) // S
{
   glBlendEquationSeparate(mode, mode);
}

GL_API void GL_APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glBlendEquationSeparate_impl_20,
                thread,
                GLBLENDEQUATIONSEPARATE_ID_20,
                RPC_ENUM(modeRGB),
                RPC_ENUM(modeAlpha));
   }
}

static void set_blend_func (CLIENT_THREAD_STATE_T *thread, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
   RPC_CALL4(glBlendFuncSeparate_impl,
             thread,
             GLBLENDFUNCSEPARATE_ID,
             RPC_ENUM(srcRGB),
             RPC_ENUM(dstRGB),
             RPC_ENUM(srcAlpha),
             RPC_ENUM(dstAlpha));
}

GL_API void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) set_blend_func(thread, sfactor, dfactor, sfactor, dfactor);
}

GL_API void GL_APIENTRY glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) set_blend_func(thread, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

GL_API void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4_IN_BULK(glBufferData_impl,
                        thread,
                        GLBUFFERDATA_ID,
                        RPC_ENUM(target),
                        RPC_SIZEIPTR(size),
                        RPC_ENUM(usage),
                        NULL,
                        0);

      if (data) {
         int offset = 0;

         while (size > 0) {
            int32_t batch = _min(KHDISPATCH_WORKSPACE_SIZE, (int32_t) size);

            RPC_CALL4_IN_BULK(glBufferSubData_impl,
                              thread,
                              GLBUFFERSUBDATA_ID,
                              RPC_ENUM(target),
                              RPC_INTPTR(offset),
                              RPC_SIZEIPTR(batch),
                              (char *)data + offset,
                              (size_t) batch);

            offset += batch;
            size -= batch;
         }
      }
   }
}

GL_API void GL_APIENTRY glBufferSubData (GLenum target, GLintptr base, GLsizeiptr size, const GLvoid *data)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      if (data) {
         int offset = 0;

         while (size > 0) {
            int32_t batch = _min(KHDISPATCH_WORKSPACE_SIZE, (int32_t)size);

            RPC_CALL4_IN_BULK(glBufferSubData_impl,
                              thread,
                              GLBUFFERSUBDATA_ID,
                              RPC_ENUM(target),
                              RPC_INTPTR(base+offset),
                              RPC_SIZEIPTR(batch),
                              (char *)data + offset,
                              (size_t) batch);

            offset += batch;
            size -= batch;
         }
      }
   }
}

GL_API void GL_APIENTRY glClear (GLbitfield mask)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      //TODO: pixmap behaviour can be better optimized to handle clears
      if (state->render_callback)
         state->render_callback();

      RPC_CALL1(glClear_impl,
                thread,
                GLCLEAR_ID,
                RPC_BITFIELD(mask));
   }
}

GL_API void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glClearColor_impl,
                thread,
                GLCLEARCOLOR_ID,
                RPC_FLOAT(red),
                RPC_FLOAT(green),
                RPC_FLOAT(blue),
                RPC_FLOAT(alpha));
   }
}

GL_API void GL_APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL4(glClearColorx_impl_11,
                thread,
                GLCLEARCOLORX_ID_11,
                RPC_FIXED(red),
                RPC_FIXED(green),
                RPC_FIXED(blue),
                RPC_FIXED(alpha));
   }
}

GL_API void GL_APIENTRY glClearDepthf (GLclampf depth)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glClearDepthf_impl,
                thread,
                GLCLEARDEPTHF_ID,
                RPC_FLOAT(depth));
   }
}

GL_API void GL_APIENTRY glClearDepthx (GLclampx depth)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1(glClearDepthx_impl_11,
                thread,
                GLCLEARDEPTHX_ID_11,
                RPC_FIXED(depth));
   }
}

GL_API void GL_APIENTRY glClearStencil (GLint s)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glClearStencil_impl,
                thread,
                GLCLEARSTENCIL_ID,
                RPC_INT(s));
   }
}

GL_API void GL_APIENTRY glClientActiveTexture (GLenum texture)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (texture >= GL_TEXTURE0 && texture < GL_TEXTURE0 + GL11_CONFIG_MAX_TEXTURE_UNITS)
         state->active_texture.client = texture;
      else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glClipPlanef (GLenum plane, const GLfloat *equation)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2_IN_CTRL(glClipPlanef_impl_11,
                        thread,
                        GLCLIPPLANEF_ID_11,
                        RPC_ENUM(plane),
                        equation,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glClipPlanex (GLenum plane, const GLfixed *equation)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2_IN_CTRL(glClipPlanex_impl_11,
                        thread,
                        GLCLIPPLANEX_ID_11,
                        RPC_ENUM(plane),
                        equation,
                        4 * sizeof(GLfixed));
   }
}

static void color_internal(CLIENT_THREAD_STATE_T *thread, float red, float green, float blue, float alpha)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   state->attrib[GL11_IX_COLOR].value[0] = red;
   state->attrib[GL11_IX_COLOR].value[1] = green;
   state->attrib[GL11_IX_COLOR].value[2] = blue;
   state->attrib[GL11_IX_COLOR].value[3] = alpha;

   //TODO: it is irritating that we have to do it this way
   RPC_CALL4(glintColor_impl_11,
      thread,
      GLINTCOLOR_ID_11,
      RPC_FLOAT(red),
      RPC_FLOAT(green),
      RPC_FLOAT(blue),
      RPC_FLOAT(alpha));
}

GL_API void GL_APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      color_internal(
         thread,
         clampf(red, 0.0f, 1.0f),
         clampf(green, 0.0f, 1.0f),
         clampf(blue, 0.0f, 1.0f),
         clampf(alpha, 0.0f, 1.0f));
   }
}

GL_API void GL_APIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      color_internal(
         thread,
         (float)red / 255.0f,
         (float)green / 255.0f,
         (float)blue / 255.0f,
         (float)alpha / 255.0f);
   }
}

GL_API void GL_APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      color_internal(
         thread,
         clampf(fixed_to_float(red), 0.0f, 1.0f),
         clampf(fixed_to_float(green), 0.0f, 1.0f),
         clampf(fixed_to_float(blue), 0.0f, 1.0f),
         clampf(fixed_to_float(alpha), 0.0f, 1.0f));
   }
}

GL_API void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glColorMask_impl,
                thread,
                GLCOLORMASK_ID,
                RPC_BOOLEAN(red),
                RPC_BOOLEAN(green),
                RPC_BOOLEAN(blue),
                RPC_BOOLEAN(alpha));
   }
}

static GLboolean is_color_size(GLint size)
{
   return size == 4;
}

static GLboolean is_color_type(GLenum type)
{
   return type == GL_UNSIGNED_BYTE ||
          type == GL_FIXED ||
          type == GL_FLOAT;
}

static GLboolean is_aligned( GLenum type, size_t value)
{
   switch (type) {
   case GL_BYTE:
   case GL_UNSIGNED_BYTE:
      return GL_TRUE;
   case GL_SHORT:
   case GL_UNSIGNED_SHORT:
      return (value & 1) == 0;
   case GL_FIXED:
   case GL_FLOAT:
      return (value & 3) == 0;
   default:
      UNREACHABLE();
      return GL_FALSE;
   }
}

GL_API void GL_APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (is_color_type(type)) {
         if (is_color_size(size) && is_aligned(type, (size_t)pointer) && is_aligned(type, (size_t)stride) && stride >= 0) {

            state->attrib[GL11_IX_COLOR].size = size;
            state->attrib[GL11_IX_COLOR].type = type;
            state->attrib[GL11_IX_COLOR].stride = stride;

            state->attrib[GL11_IX_COLOR].pointer = pointer;

            state->attrib[GL11_IX_COLOR].buffer = state->bound_buffer.array;

            RPC_CALL0(glColorPointer_impl_11,
                      thread,
                      GLCOLORPOINTER_ID_11);
         } else
            set_error(state, GL_INVALID_VALUE);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

static uint32_t get_palette_size(GLenum internalformat)
{
   switch (internalformat)
   {
   case GL_PALETTE4_RGB8_OES: return 16 * 3;
   case GL_PALETTE4_RGBA8_OES: return 16 * 4;
   case GL_PALETTE4_R5_G6_B5_OES: return 16 * 2;
   case GL_PALETTE4_RGBA4_OES: return 16 * 2;
   case GL_PALETTE4_RGB5_A1_OES: return 16 * 2;
   case GL_PALETTE8_RGB8_OES: return 256 * 3;
   case GL_PALETTE8_RGBA8_OES: return 256 * 4;
   case GL_PALETTE8_R5_G6_B5_OES: return 256 * 2;
   case GL_PALETTE8_RGBA4_OES: return 256 * 2;
   case GL_PALETTE8_RGB5_A1_OES: return 256 * 2;
   default:
      UNREACHABLE();
      return 0;
   }
}

GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   GLboolean res;
   if (IS_OPENGLES_11_OR_20(thread)) {
      switch (internalformat)
      {
         case GL_ETC1_RGB8_OES:
         {
            uint32_t pitch = 2 * ((width + 3) / 4);
            uint32_t lines = pitch ? (uint32_t)(KHDISPATCH_WORKSPACE_SIZE / pitch) : (uint32_t)height;

            res = RPC_BOOLEAN_RES(RPC_CALL8_IN_BULK_RES(glCompressedTexImage2D_impl,
                              thread,
                              GLCOMPRESSEDTEXIMAGE2D_ID,
                              RPC_ENUM  (target),
                              RPC_INT   (level),
                              RPC_ENUM  (internalformat),
                              RPC_SIZEI (width),
                              RPC_SIZEI (height),
                              RPC_INT   (border),
                              RPC_SIZEI (imageSize),
                              NULL,
                              0));

            if (res && data && lines && width && height) {
               int offset = 0;

               while (height > 0) {
                  int32_t batch = (_min(lines, (int32_t)height) + 3) & ~3;

                  RPC_CALL9_IN_BULK(glCompressedTexSubImage2D_impl,
                                    thread,
                                    GLCOMPRESSEDTEXSUBIMAGE2D_ID,
                                    RPC_ENUM(target),
                                    RPC_INT(level),
                                    RPC_INT(0),
                                    RPC_INT(offset),
                                    RPC_SIZEI(width),
                                    RPC_SIZEI(batch),
                                    RPC_ENUM(internalformat),
                                    batch * pitch,
                                    (char *)data + offset * pitch,
                                    batch * pitch);

                  offset += batch;
                  height -= batch;
               }
            }
            break;
         }
         case GL_PALETTE4_RGB8_OES:
         case GL_PALETTE4_RGBA8_OES:
         case GL_PALETTE4_R5_G6_B5_OES:
         case GL_PALETTE4_RGBA4_OES:
         case GL_PALETTE4_RGB5_A1_OES:
         case GL_PALETTE8_RGB8_OES:
         case GL_PALETTE8_RGBA8_OES:
         case GL_PALETTE8_R5_G6_B5_OES:
         case GL_PALETTE8_RGBA4_OES:
         case GL_PALETTE8_RGB5_A1_OES:
         {
            int palette_size = get_palette_size(internalformat);

            level = -level;
            res = RPC_BOOLEAN_RES(RPC_CALL8_IN_BULK_RES(glCompressedTexImage2D_impl,
                              thread,
                              GLCOMPRESSEDTEXIMAGE2D_ID,
                              RPC_ENUM  (target),
                              RPC_INT   (level),
                              RPC_ENUM  (internalformat),
                              RPC_SIZEI (width),
                              RPC_SIZEI (height),
                              RPC_INT   (border),
                              RPC_SIZEI (imageSize),
                              data,
                              palette_size));

            if (res && data && width && height) {
               int offset = palette_size;
               while (offset < imageSize) {
                  int32_t batch = _min(KHDISPATCH_WORKSPACE_SIZE, imageSize - offset);

                  RPC_CALL9_IN_BULK(glCompressedTexSubImage2D_impl,
                                    thread,
                                    GLCOMPRESSEDTEXSUBIMAGE2D_ID,
                                    RPC_ENUM(target),
                                    RPC_INT(level),
                                    RPC_INT(offset - palette_size),
                                    RPC_INT(0),
                                    RPC_SIZEI(width),
                                    RPC_SIZEI(height),
                                    RPC_ENUM(internalformat),
                                    batch,
                                    (char *)data + offset,
                                    batch);

                  offset += batch;
               }
            }
            break;
         }
      }
   }
}

GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   UNUSED(target);
   UNUSED(level);
   UNUSED(xoffset);
   UNUSED(yoffset);
   UNUSED(width);
   UNUSED(height);
   UNUSED(imageSize);
   UNUSED(data);

   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      switch (format)
      {
      case GL_ETC1_RGB8_OES:
         // Cannot specify subimages of ETC1 textures
         set_error(state, GL_INVALID_OPERATION);
         break;
      case GL_PALETTE4_RGB8_OES:
      case GL_PALETTE4_RGBA8_OES:
      case GL_PALETTE4_R5_G6_B5_OES:
      case GL_PALETTE4_RGBA4_OES:
      case GL_PALETTE4_RGB5_A1_OES:
      case GL_PALETTE8_RGB8_OES:
      case GL_PALETTE8_RGBA8_OES:
      case GL_PALETTE8_R5_G6_B5_OES:
      case GL_PALETTE8_RGBA4_OES:
      case GL_PALETTE8_RGB5_A1_OES:
         // Cannot specify subimages of paletted textures
         set_error(state, GL_INVALID_OPERATION);
         break;
      default:
         // Some format we don't recognise
         set_error(state, GL_INVALID_VALUE);
         break;
      }
   }
}

GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL8(glCopyTexImage2D_impl,
                thread,
                GLCOPYTEXIMAGE2D_ID,
                RPC_ENUM(target),
                RPC_INT(level),
                RPC_ENUM(internalformat),
                RPC_INT(x),
                RPC_INT(y),
                RPC_SIZEI(width),
                RPC_SIZEI(height),
                RPC_INT(border));
   }
}

GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL8(glCopyTexSubImage2D_impl,
                thread,
                GLCOPYTEXSUBIMAGE2D_ID,
                RPC_ENUM(target),
                RPC_INT(level),
                RPC_INT(xoffset),
                RPC_INT(yoffset),
                RPC_INT(x),
                RPC_INT(y),
                RPC_SIZEI(width),
                RPC_SIZEI(height));
   }
}

GL_API GLuint GL_APIENTRY glCreateProgram (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_UINT_RES(RPC_CALL0_RES(glCreateProgram_impl_20,
                                        thread,
                                        GLCREATEPROGRAM_ID_20));
   }

   return 0;
}

GL_API GLuint GL_APIENTRY glCreateShader (GLenum type)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_UINT_RES(RPC_CALL1_RES(glCreateShader_impl_20,
                                        thread,
                                        GLCREATESHADER_ID_20,
                                        RPC_ENUM(type)));
   }

   return 0;
}

GL_API void GL_APIENTRY glCullFace (GLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glCullFace_impl,
                thread,
                GLCULLFACE_ID,
                RPC_ENUM(mode));
   }
}

GL_API void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      int i, j;

      for (i = 0; i < n; i++) {
         GLuint buffer = buffers[i];

         if (state->bound_buffer.array == buffer)
            state->bound_buffer.array = 0;
         if (state->bound_buffer.element_array == buffer)
            state->bound_buffer.element_array = 0;

         for (j = 0; j < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; j++)
            if (state->attrib[j].buffer == buffer)
               state->attrib[j].buffer = 0;
      }
   }

   if (IS_OPENGLES_11_OR_20(thread)) {
      do {
         int32_t items = (int32_t)( KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint) );
         int32_t batch = _min(items, (int32_t) n);

         RPC_CALL2_IN_BULK(glDeleteBuffers_impl,
                           thread,
                           GLDELETEBUFFERS_ID,
                           RPC_SIZEI(batch),
                           buffers + offset,
                           batch > 0 ? batch * sizeof(GLuint) : 0);

         offset += batch;
         n -= batch;
      } while (n > 0);
   }
}

GL_API void GL_APIENTRY glDeleteProgram (GLuint program)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glDeleteProgram_impl_20,
                thread,
                GLDELETEPROGRAM_ID_20,
                RPC_UINT(program));
   }
}

GL_API void GL_APIENTRY glDeleteShader (GLuint shader)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glDeleteShader_impl_20,
                thread,
                GLDELETESHADER_ID_20,
                RPC_UINT(shader));
   }
}

GL_API void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;

   if (IS_OPENGLES_11_OR_20(thread)) {
      do {
         int32_t items = (int32_t)(KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
         int32_t batch = _min(items, (int32_t)n);

         RPC_CALL2_IN_BULK(glDeleteTextures_impl,
                           thread,
                           GLDELETETEXTURES_ID,
                           RPC_SIZEI(batch),
                           textures + offset,
                           batch > 0 ? batch * sizeof(GLuint) : 0);

         offset += batch;
         n -= batch;
      } while (n > 0);
   }
}

GL_API void GL_APIENTRY glDepthFunc (GLenum func)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glDepthFunc_impl,
                thread,
                GLDEPTHFUNC_ID,
                RPC_ENUM(func));
   }
}

GL_API void GL_APIENTRY glDepthMask (GLboolean flag)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glDepthMask_impl,
                thread, 
                GLDEPTHMASK_ID,
                RPC_BOOLEAN(flag));
   }
}

GL_API void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glDepthRangef_impl,
                thread,
                GLDEPTHRANGEF_ID,
                RPC_FLOAT(zNear),
                RPC_FLOAT(zFar));
   }
}

GL_API void GL_APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glDepthRangex_impl_11,
                thread,
                GLDEPTHRANGEX_ID_11,
                RPC_FIXED(zNear),
                RPC_FIXED(zFar));
   }
}

GL_API void GL_APIENTRY glDetachShader (GLuint program, GLuint shader)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glDetachShader_impl_20,
                thread,
                GLDETACHSHADER_ID_20,
                RPC_UINT(program),
                RPC_UINT(shader));
   }
}

GL_API void GL_APIENTRY glDisable (GLenum cap)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glDisable_impl,
               thread,
                GLDISABLE_ID,
                RPC_ENUM(cap));
   }
}

static void set_enabled_11(CLIENT_THREAD_STATE_T *thread, GLenum array, GLboolean enabled)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   vcos_assert(state != NULL);

   switch (array) {
   case GL_VERTEX_ARRAY:
      state->attrib[GL11_IX_VERTEX].enabled = enabled;
      break;
   case GL_NORMAL_ARRAY:
      state->attrib[GL11_IX_NORMAL].enabled = enabled;
      break;
   case GL_COLOR_ARRAY:
      state->attrib[GL11_IX_COLOR].enabled = enabled;
      break;
   case GL_POINT_SIZE_ARRAY_OES:
      state->attrib[GL11_IX_POINT_SIZE].enabled = enabled;
      break;
   case GL_TEXTURE_COORD_ARRAY:
      state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].enabled = enabled;
      break;
   default:
      set_error(state, GL_INVALID_ENUM);
      break;
   }
}

GL_API void GL_APIENTRY glDisableClientState (GLenum array)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      set_enabled_11(thread, array, GL_FALSE);
   }
}

GL_APICALL void GL_APIENTRY glDisableVertexAttribArray (GLuint index)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (index < GLXX_CONFIG_MAX_VERTEX_ATTRIBS)
         state->attrib[index].enabled = GL_FALSE;
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

static int align_length(int length)
{
   return (length + 15) & ~15;
}

static int calc_length(int max, int size, GLenum type, int stride)
{
   if (max >= 0) {
      int type_size = khrn_get_type_size( (int)type);

      return align_length(size * type_size + max * (stride ? stride : size * type_size));
   } else
      return 0;
}

typedef struct MERGE_INFO {
   bool send;

   const char *start;
   const char *end;
   int key;

   struct MERGE_INFO *next;
} MERGE_INFO_T;

static int merge_attribs(CLIENT_THREAD_STATE_T *thread, KHRN_CACHE_T *cache, MERGE_INFO_T *merge, int *keys, int send, bool is_opengles_11)
{
   int i, j;

   for (i = 1; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++)
      if (merge[i].send)
         for (j = 0; j < i; j++)
            if (merge[j].send && !merge[j].next) {
               const char *start = merge[i].start < merge[j].start ? merge[i].start : merge[j].start;
               const char *end = merge[i].end > merge[j].end ? merge[i].end : merge[j].end;

               if ((uint32_t)(end - start) < (uint32_t)((merge[i].end - merge[i].start) + (merge[j].end - merge[j].start))) {
                  MERGE_INFO_T *curr;

                  if (merge[i].start < merge[j].start) {
                     curr = &merge[i];
                     while (curr->next)
                        curr = curr->next;
                     curr->end = end;

                     merge[j].key = merge[j].start - merge[i].start;
                     merge[j].next = &merge[i];
                  } else {
                     curr = &merge[j];
                     while (curr->next)
                        curr = curr->next;
                     curr->end = end;

                     merge[i].key = merge[i].start - merge[j].start;
                     merge[i].next = &merge[j];
                  }
                }
            }

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++)
      if (merge[i].send && !merge[i].next) {
         int key = khrn_cache_lookup(thread, cache, merge[i].start, merge[i].end - merge[i].start, CACHE_SIG_ATTRIB_0 + i, is_opengles_11);

         merge[i].key = key;
         keys[send++] = key;
      }

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++) {
      MERGE_INFO_T *curr;

      for (curr = merge[i].next; curr; curr = curr->next)
         merge[i].key += curr->key;

      merge[i].next = NULL;
   }

   return send;
}

static GLboolean is_index_type(GLenum type)
{
   return type == GL_UNSIGNED_BYTE ||
          type == GL_UNSIGNED_SHORT;
}

static void draw_arrays_or_elements(CLIENT_THREAD_STATE_T *thread, GLXX_CLIENT_STATE_T *state, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
   const void *indices_pointer = 0;
   GLuint indices_buffer;
   bool send_indices;
   int max = 0;
   int indices_length = 0;
   int indices_key = 0;
   int first = 0;
   bool send_any;
   int i;
   MERGE_INFO_T attrib_merge[GLXX_CONFIG_MAX_VERTEX_ATTRIBS];
   const GLvoid *saved_blah[GLXX_CONFIG_MAX_VERTEX_ATTRIBS];
   int keys[GLXX_CONFIG_MAX_VERTEX_ATTRIBS+1];
   int send = 0;

   vcos_assert(state != NULL);

   if (state->render_callback && (IS_OPENGLES_11(thread) || state->default_framebuffer))
      state->render_callback();

   if(count<0)
   {
      set_error(state, GL_INVALID_VALUE);
      return;
   }

   send_any = false;
   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++) {
      send_any |= state->attrib[i].enabled && state->attrib[i].pointer != NULL && state->attrib[i].buffer == 0;

      /* TODO: what should we do if people give us null pointers? */
      if (state->attrib[i].enabled && state->attrib[i].pointer == NULL && state->attrib[i].buffer == 0)
         return;
   }

   if(type==0) {
      first = (int)indices;
      indices_pointer = (void *)first;
      indices_buffer = 0;
      send_indices = 0;
      indices_key = 0;
   } else {
      send_indices = count >= 0 && state->bound_buffer.element_array == 0;
      indices_buffer = state->bound_buffer.element_array;
   }

{

   if(type==0) {
      max = first + count - 1;
   } else {
      max = send_indices ? find_max(count, khrn_get_type_size( (int)type ), indices) : (send_any ? RPC_INT_RES(RPC_CALL3_RES(glintFindMax_impl, thread, no_id, count, type, indices)) : -1);
      indices_length = align_length(count * khrn_get_type_size( (int) type ));
      indices_key = send_indices ? khrn_cache_lookup(thread, &state->cache, indices, indices_length, CACHE_SIG_INDEX, IS_OPENGLES_11(thread)) : 0;
      indices_pointer = !send_indices ? indices : (void *)(indices_key + offsetof(CACHE_ENTRY_T, data));
   }

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++) {
      attrib_merge[i].send = state->attrib[i].enabled && state->attrib[i].buffer == 0;
      attrib_merge[i].start = state->attrib[i].pointer;
      attrib_merge[i].end = (const char *)state->attrib[i].pointer + calc_length(max, state->attrib[i].size, state->attrib[i].type, state->attrib[i].stride);
      attrib_merge[i].key = 0;
      attrib_merge[i].next = NULL;
   }

   keys[0] = indices_key;
   send = merge_attribs(thread, &state->cache, attrib_merge, keys, send_indices, IS_OPENGLES_11(thread));
   if (khrn_cache_get_entries(&state->cache) < send){
      set_error(state, GL_OUT_OF_MEMORY);
      return;
   }

   /* if any of the keys have run out of space in the cache, return without doing the draw */
   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++)
   {
      if (attrib_merge[i].send && attrib_merge[i].key == -1)
      {
         set_error(state, GL_OUT_OF_MEMORY);
         return;
      }
   }

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++)
   {
      if (attrib_merge[i].send)
      {
         saved_blah[i] = state->attrib[i].pointer;
         state->attrib[i].pointer = (GLvoid *)(attrib_merge[i].key + offsetof(CACHE_ENTRY_T, data));
      }
   }

   RPC_CALL8(glDrawElements_impl, thread, no_id, mode, count, type,
                                   (void *)indices_pointer, indices_buffer,
                                   state->attrib, keys, send);

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++)
   {
      if (attrib_merge[i].send)
         state->attrib[i].pointer = saved_blah[i];
   }
}

}

GL_API void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      draw_arrays_or_elements(thread, state, mode, count, 0, (void *)first);
   }
}


GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      if (!is_index_type(type)) {
         set_error(state, GL_INVALID_ENUM);
         return;
      }
      if (!is_aligned(type, (size_t)indices)) {
         set_error(state, GL_INVALID_VALUE);
         return;
      }
      draw_arrays_or_elements(thread, state, mode, count, type, indices);
   }
}

GL_API void GL_APIENTRY glEnable (GLenum cap)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glEnable_impl,
                thread,
                GLENABLE_ID,
                RPC_ENUM(cap));
   }
}

GL_API void GL_APIENTRY glEnableClientState (GLenum array)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      set_enabled_11(thread, array, GL_TRUE);
   }
}

GL_APICALL void GL_APIENTRY glEnableVertexAttribArray (GLuint index)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (index < GLXX_CONFIG_MAX_VERTEX_ATTRIBS)
         state->attrib[index].enabled = GL_TRUE;
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

GL_API void GL_APIENTRY glFinish (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {

      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (thread->is_current_pixmap || thread->has_rendered_to_pixmap) {
         (void) RPC_UINT_RES(RPC_CALL0_RES(glFinish_impl,
                                           thread,
                                           GLFINISH_ID));  // Return ignored - read performed to force blocking
      }

      /* Clear the has_rendered_to_pixmap if we don't currently have a pixmap bound */
      if (!thread->is_current_pixmap)
         thread->has_rendered_to_pixmap = false;

      if (state->flush_callback)
         state->flush_callback(true);
   }
}

GL_API void GL_APIENTRY glFlush (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {

      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (thread->is_current_pixmap || thread->has_rendered_to_pixmap) {
         RPC_CALL0(glFlush_impl,
                   thread,
                   GLFLUSH_ID);
      }

      /* Note: we don't clear the has_rendered_to_pixmap flag here, as we do in glFinish, since someone might call glFinish()
         immediately after this, which would then be a no-op and break the rules. */

      if (state->flush_callback)
         state->flush_callback(false);
   }

   //TODO: where exactly should we put RPC_FLUSH? Are there any other functions
   //which need it? (e.g. eglSwapBuffers)
   RPC_FLUSH(thread);
}

GL_API void GL_APIENTRY glFogf (GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glFogf_impl_11,
                thread,
                GLFOGF_ID_11,
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glFogfv (GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported fog params are

         FOG_MODE (1)
         FOG_DENSITY (1)
         FOG_START (1)
         FOG_END (1)
         FOG_COLOR (4)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glFogfv_impl_11,
                        thread,
                        GLFOGFV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glFogx (GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glFogx_impl_11,
               thread,
               GLFOGX_ID_11,
               RPC_ENUM(pname),
               RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glFogxv (GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported fog params are

         FOG_MODE (1)
         FOG_DENSITY (1)
         FOG_START (1)
         FOG_END (1)
         FOG_COLOR (4)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glFogxv_impl_11,
                        thread,
                        GLFOGXV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glFrontFace (GLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glFrontFace_impl,
                thread,
                GLFRONTFACE_ID,
                RPC_ENUM(mode));
   }
}

GL_API void GL_APIENTRY glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL6(glFrustumf_impl_11,
                thread,
                GLFRUSTUMF_ID_11,
                RPC_FLOAT(left),
                RPC_FLOAT(right),
                RPC_FLOAT(bottom),
                RPC_FLOAT(top),
                RPC_FLOAT(zNear),
                RPC_FLOAT(zFar));
   }
}

GL_API void GL_APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL6(glFrustumx_impl_11,
                thread,
                GLFRUSTUMX_ID_11,
                RPC_FIXED(left),
                RPC_FIXED(right),
                RPC_FIXED(bottom),
                RPC_FIXED(top),
                RPC_FIXED(zNear),
                RPC_FIXED(zFar));
   }
}

GL_API void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      int offset = 0;

      do {
         int32_t items = (int32_t) (KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
         int32_t batch = _min(items, (int32_t) n);

         RPC_CALL2_OUT_BULK(glGenBuffers_impl,
                            thread,
                            GLGENBUFFERS_ID,
                            RPC_SIZEI(batch),
                            (GLuint*)(buffers + offset));

         offset += batch;
         n -= batch;
      } while (n > 0);
   }
}

GL_API void GL_APIENTRY glGenTextures (GLsizei n, GLuint *textures)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      int offset = 0;

      do {
         int32_t items = (int32_t) (KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
         int32_t batch = _min(items, (int32_t)n);

         RPC_CALL2_OUT_BULK(glGenTextures_impl,
                            thread,
                            GLGENTEXTURES_ID,
                            RPC_SIZEI(batch),
                            textures + offset);

         offset += batch;
         n -= batch;
      } while (n > 0);
   }
}

GL_APICALL void GL_APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetActiveAttrib_impl_20(program, index, bufsize, length, size, type, name);
   }
}

GL_APICALL void GL_APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetActiveUniform_impl_20(program, index, bufsize, length, size, type, name);
   }
}

GL_APICALL void GL_APIENTRY glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetAttachedShaders_impl_20(program, maxcount, count, shaders);
   }
}

GL_APICALL int GL_APIENTRY glGetAttribLocation (GLuint program, const char *name)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_INT_RES(RPC_CALL2_IN_BULK_RES(glGetAttribLocation_impl_20,
                                               thread,
                                               GLGETATTRIBLOCATION_ID_20,
                                               RPC_UINT(program),
                                               name,
                                               strlen(name) + 1));
   }

   return 0;
}

/*
   native client-side boolean variables

   VERTEX ARRAY IsEnabled
   NORMAL ARRAY IsEnabled
   COLOR ARRAY IsEnabled
   TEXTURE COORD ARRAY IsEnabled
   POINT SIZE ARRAY OES IsEnabled
*/

static int get_boolean_internal_11(CLIENT_THREAD_STATE_T *thread, GLenum pname, GLboolean *params)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   vcos_assert(state != NULL);

   switch (pname) {
   case GL_VERTEX_ARRAY:
      params[0] = state->attrib[GL11_IX_VERTEX].enabled;
      return 1;
   case GL_NORMAL_ARRAY:
      params[0] = state->attrib[GL11_IX_NORMAL].enabled;
      return 1;
   case GL_COLOR_ARRAY:
      params[0] = state->attrib[GL11_IX_COLOR].enabled;
      return 1;
   case GL_TEXTURE_COORD_ARRAY:
      params[0] = state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].enabled;
      return 1;
   case GL_POINT_SIZE_ARRAY_OES:
      params[0] = state->attrib[GL11_IX_POINT_SIZE].enabled;
      return 1;
   default:
      UNREACHABLE();
      break;
   }

   return 0;
}

/*
   native client-side floating-point state variables

   CURRENT_COLOR
   CURRENT_TEXTURE_COORDS
   CURRENT_NORMAL
   POINT_SIZE
*/

static int get_float_internal_11(CLIENT_THREAD_STATE_T *thread, GLenum pname, GLfloat *params)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   int i;

   switch (pname) {
   case GL_CURRENT_TEXTURE_COORDS:
   {
      /*
         apparently we need the current texture coordinates for the _server_ active texture unit
      */

      for (i = 0; i < 4; i++)
         params[i] = state->attrib[state->active_texture.server - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[i];
      return 4;
   }
   case GL_CURRENT_COLOR:
   {
      for (i = 0; i < 4; i++)
         params[i] = state->attrib[GL11_IX_COLOR].value[i];
      return 4;
   }
   case GL_CURRENT_NORMAL:
   {
      for (i = 0; i < 3; i++)
         params[i] = state->attrib[GL11_IX_NORMAL].value[i];
      return 3;
   }
   case GL_POINT_SIZE:
      params[0] = state->attrib[GL11_IX_POINT_SIZE].value[0];
      return 1;
   default:
      UNREACHABLE();
      break;
   }

   return 0;
}

/*
   native client-side integer state variables

   CLIENT ACTIVE TEXTURE GetIntegerv
   VERTEX ARRAY SIZE GetIntegerv
   VERTEX ARRAY TYPE GetIntegerv
   VERTEX ARRAY STRIDE GetIntegerv
   NORMAL ARRAY TYPE GetIntegerv
   NORMAL ARRAY STRIDE GetIntegerv
   COLOR ARRAY SIZE GetIntegerv
   COLOR ARRAY TYPE GetIntegerv
   COLOR ARRAY STRIDE GetIntegerv
   TEXTURE COORD ARRAY SIZE GetIntegerv
   TEXTURE COORD ARRAY TYPE GetIntegerv
   TEXTURE COORD ARRAY STRIDE GetIntegerv
   POINT SIZE ARRAY TYPE OES GetIntegerv
   POINT SIZE ARRAY STRIDE OES GetIntegerv

   VERTEX ARRAY BUFFER BINDING GetIntegerv
   NORMAL ARRAY BUFFER BINDING GetIntegerv
   COLOR ARRAY BUFFER BINDING GetIntegerv
   TEXTURE COORD ARRAY BUFFER BINDING GetIntegerv
   POINT SIZE ARRAY BUFFER BINDING OES GetIntegerv

   UNPACK ALIGNMENT GetIntegerv
   PACK ALIGNMENT GetIntegerv
*/

static int get_integer_internal_11(CLIENT_THREAD_STATE_T *thread, GLenum pname, GLint *params)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   vcos_assert(state != NULL);

   switch (pname) {
   case GL_CLIENT_ACTIVE_TEXTURE:
      params[0] = (GLint) state->active_texture.client;
      return 1;
   case GL_VERTEX_ARRAY_SIZE:
      params[0] = (GLint) state->attrib[GL11_IX_VERTEX].size;
      return 1;
   case GL_VERTEX_ARRAY_TYPE:
      params[0] = (GLint) state->attrib[GL11_IX_VERTEX].type;
      return 1;
   case GL_VERTEX_ARRAY_STRIDE:
      params[0] = (GLint) state->attrib[GL11_IX_VERTEX].stride;
      return 1;
   case GL_NORMAL_ARRAY_TYPE:
      params[0] = (GLint) state->attrib[GL11_IX_NORMAL].type;
      return 1;
   case GL_NORMAL_ARRAY_STRIDE:
      params[0] = (GLint) state->attrib[GL11_IX_NORMAL].stride;
      return 1;
   case GL_COLOR_ARRAY_SIZE:
      params[0] = (GLint) state->attrib[GL11_IX_COLOR].size;
      return 1;
   case GL_COLOR_ARRAY_TYPE:
      params[0] = (GLint) state->attrib[GL11_IX_COLOR].type;
      return 1;
   case GL_COLOR_ARRAY_STRIDE:
      params[0] = (GLint) state->attrib[GL11_IX_COLOR].stride;
      return 1;
   case GL_TEXTURE_COORD_ARRAY_SIZE:
      params[0] = (GLint) state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].size;
      return 1;
   case GL_TEXTURE_COORD_ARRAY_TYPE:
      params[0] = (GLint) state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].type;
      return 1;
   case GL_TEXTURE_COORD_ARRAY_STRIDE:
      params[0] = (GLint) state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].stride;
      return 1;
   case GL_POINT_SIZE_ARRAY_TYPE_OES:
      params[0] = (GLint) state->attrib[GL11_IX_POINT_SIZE].type;
      return 1;
   case GL_POINT_SIZE_ARRAY_STRIDE_OES:
      params[0] = (GLint) state->attrib[GL11_IX_POINT_SIZE].stride;
      return 1;
   case GL_ARRAY_BUFFER_BINDING:
      params[0] = (GLint) state->bound_buffer.array;
      return 1;
   case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      params[0] = (GLint) state->bound_buffer.element_array;
      return 1;
   case GL_VERTEX_ARRAY_BUFFER_BINDING:
      params[0] = (GLint) state->attrib[GL11_IX_VERTEX].buffer;
      return 1;
   case GL_NORMAL_ARRAY_BUFFER_BINDING:
      params[0] = (GLint) state->attrib[GL11_IX_NORMAL].buffer;
      return 1;
   case GL_COLOR_ARRAY_BUFFER_BINDING:
      params[0] = (GLint) state->attrib[GL11_IX_COLOR].buffer;
      return 1;
   case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
      /*
      TODO is this right?
      Most texture state variables are qualified by the value of ACTIVE TEXTURE
to determine which server texture state vector is queried. Client texture
state variables such as texture coordinate array pointers are qualified by the
value of CLIENT ACTIVE TEXTURE. Tables 6.3, 6.4, 6.7, 6.13, 6.15, and 6.21
indicate those state variables which are qualified by ACTIVE TEXTURE or
CLIENT ACTIVE TEXTURE during state queries
      */
      params[0] = (GLint) state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].buffer;
      return 1;
   case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
      params[0] = (GLint) state->attrib[GL11_IX_POINT_SIZE].buffer;
      return 1;

   case GL_UNPACK_ALIGNMENT:
      params[0] = (GLint) state->alignment.unpack;
      return 1;
   case GL_PACK_ALIGNMENT:
      params[0] = (GLint) state->alignment.pack;
      return 1;

   //TODO: these are horrible and don't make any sense
   //Is this a sensible thing to return?
   case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      params[0] = (GLint) GL_UNSIGNED_BYTE;
      return 1;
   case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
      params[0] = (GLint) GL_RGBA;
      return 1;
   //end TODO

   default:
      UNREACHABLE();
      break;
   }

   return 0;
}

static int get_integer_internal_20(CLIENT_THREAD_STATE_T *thread, GLenum pname, GLint *params)
{
   GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

   vcos_assert(state != NULL);

   switch (pname) {
   case GL_UNPACK_ALIGNMENT:
      params[0] = state->alignment.unpack;
      return 1;
   case GL_PACK_ALIGNMENT:
      params[0] = state->alignment.pack;
      return 1;
   default:
      UNREACHABLE();
      break;
   }

   return 0;
}

GL_API void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      switch (pname) {
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_POINT_SIZE_ARRAY_OES:
         get_boolean_internal_11(thread, pname, params);
         break;
      case GL_CLIENT_ACTIVE_TEXTURE:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_POINT_SIZE_ARRAY_TYPE_OES:
      case GL_POINT_SIZE_ARRAY_STRIDE_OES:
      case GL_ARRAY_BUFFER_BINDING:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      case GL_VERTEX_ARRAY_BUFFER_BINDING:
      case GL_NORMAL_ARRAY_BUFFER_BINDING:
      case GL_COLOR_ARRAY_BUFFER_BINDING:
      case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
      case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
      {
         GLint temp;

         get_integer_internal_11(thread, pname, &temp);

         params[0] = temp != 0;
         break;
      }
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_CURRENT_COLOR:
      case GL_CURRENT_NORMAL:
      case GL_POINT_SIZE:
      {
         GLfloat temp[4];
         GLuint count = (GLuint) get_float_internal_11(thread, pname, temp);
         GLuint i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = temp[i] != 0.0f;

         break;
      }
      default:
         RPC_CALL2_OUT_CTRL(glGetBooleanv_impl,
                            thread, 
                            GLGETBOOLEANV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }

   if (IS_OPENGLES_20(thread)) {
      switch (pname) {
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      {
         GLint temp = 0;

         get_integer_internal_20(thread, pname, &temp);

         params[0] = temp != 0;
         break;
      }
      default:
         RPC_CALL2_OUT_CTRL(glGetBooleanv_impl,
                            thread, 
                            GLGETBOOLEANV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }

   }
}

GL_API void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetBufferParameteriv_impl,
                         thread, 
                         GLGETBUFFERPARAMETERIV_ID,
                         RPC_ENUM(target),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetClipPlanef (GLenum pname, GLfloat eqn[4])
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2_OUT_CTRL(glGetClipPlanef_impl_11,
                         thread, 
                         GLGETCLIPPLANEF_ID_11,
                         RPC_ENUM(pname),
                         eqn);
   }
}

GL_API void GL_APIENTRY glGetClipPlanex (GLenum pname, GLfixed eqn[4])
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2_OUT_CTRL(glGetClipPlanex_impl_11,
                         thread, 
                         GLGETCLIPPLANEX_ID_11,
                         RPC_ENUM(pname),
                         eqn);
   }
}

GL_API GLenum GL_APIENTRY glGetError (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      GLenum result = state->error;

      if (result == GL_NO_ERROR) {
         result = RPC_ENUM_RES(RPC_CALL0_RES(glGetError_impl,
                                             thread,
                                             GLGETERROR_ID));
      } else
         state->error = GL_NO_ERROR;

      return result;
   }

   return 0;
}

GL_API void GL_APIENTRY glGetFixedv (GLenum pname, GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      switch (pname) {
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_POINT_SIZE_ARRAY_OES:
      {
         GLboolean temp[4];

         int count = get_boolean_internal_11(thread, pname, temp);
         int i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = temp[i] ? (GLfixed)float_to_fixed(1.0f) : (GLfixed)float_to_fixed(0.0f);

         break;
      }
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_CURRENT_COLOR:
      case GL_CURRENT_NORMAL:
      case GL_POINT_SIZE:
      {
         GLfloat temp[4];

         int count = get_float_internal_11(thread, pname, temp);
         int i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = (GLfixed) float_to_fixed(temp[i]);

         break;
      }
      case GL_CLIENT_ACTIVE_TEXTURE:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_POINT_SIZE_ARRAY_TYPE_OES:
      case GL_POINT_SIZE_ARRAY_STRIDE_OES:
      case GL_ARRAY_BUFFER_BINDING:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      case GL_VERTEX_ARRAY_BUFFER_BINDING:
      case GL_NORMAL_ARRAY_BUFFER_BINDING:
      case GL_COLOR_ARRAY_BUFFER_BINDING:
      case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
      case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
      {
         GLint temp;

         get_integer_internal_11(thread, pname, &temp);

         params[0] = (GLfixed) float_to_fixed((GLfloat)temp);
         break;
      }
      default:
         RPC_CALL2_OUT_CTRL(glGetFixedv_impl_11,
                            thread, 
                            GLGETFIXEDV_ID_11,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }
}

GL_API void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      switch (pname) {
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_POINT_SIZE_ARRAY_OES:
      {
         GLboolean temp[4];
         GLuint count = (GLuint) get_boolean_internal_11(thread, pname, temp);
         GLuint i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = temp[i] ? 1.0f : 0.0f;

         break;
      }
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_CURRENT_COLOR:
      case GL_CURRENT_NORMAL:
      case GL_POINT_SIZE:
         get_float_internal_11(thread, pname, params);

         break;
      case GL_CLIENT_ACTIVE_TEXTURE:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_POINT_SIZE_ARRAY_TYPE_OES:
      case GL_POINT_SIZE_ARRAY_STRIDE_OES:
      case GL_ARRAY_BUFFER_BINDING:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      case GL_VERTEX_ARRAY_BUFFER_BINDING:
      case GL_NORMAL_ARRAY_BUFFER_BINDING:
      case GL_COLOR_ARRAY_BUFFER_BINDING:
      case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
      case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
      {
         GLint temp;

         get_integer_internal_11(thread, pname, &temp);

         params[0] = (GLfloat)temp;
         break;
      }
      default:
         RPC_CALL2_OUT_CTRL(glGetFloatv_impl,
                            thread, 
                            GLGETFLOATV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }
   else if (IS_OPENGLES_20(thread)) {
      switch (pname) {
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      {
         GLint temp = 0;

         get_integer_internal_20(thread, pname, &temp);

         params[0] = (GLfloat)temp;
         break;
      }
      default:
         RPC_CALL2_OUT_CTRL(glGetFloatv_impl,
                            thread, 
                            GLGETFLOATV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }
}

GL_API void GL_APIENTRY glGetIntegerv (GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      switch (pname) {
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_POINT_SIZE_ARRAY_OES:
      {
         GLboolean temp[4];
         GLuint count = (GLuint) get_boolean_internal_11(thread, pname, temp);
         GLuint i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = temp[i] ? 1 : 0;

         break;
      }
      case GL_CURRENT_COLOR:
      case GL_CURRENT_NORMAL:
      {
         GLfloat temp[4];
         GLuint count = (GLuint) get_float_internal_11(thread, pname, temp);
         GLuint i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = (GLint)floor((4294967295.0f * temp[i] - 1.0f) / 2.0f + 0.5f);

         //TODO: that the above is correct wrt table 2.7 in the GL spec

         break;
      }
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_POINT_SIZE:
      {
         GLfloat temp[4];
         GLuint count = (GLuint) get_float_internal_11(thread, pname, temp);
         GLuint i;

         vcos_assert(count <= 4);

         for (i = 0; i < count; i++)
            params[i] = (GLint) float_to_int(temp[i]);

         break;
      }
      case GL_CLIENT_ACTIVE_TEXTURE:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_POINT_SIZE_ARRAY_TYPE_OES:
      case GL_POINT_SIZE_ARRAY_STRIDE_OES:
      case GL_ARRAY_BUFFER_BINDING:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      case GL_VERTEX_ARRAY_BUFFER_BINDING:
      case GL_NORMAL_ARRAY_BUFFER_BINDING:
      case GL_COLOR_ARRAY_BUFFER_BINDING:
      case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
      case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
         get_integer_internal_11(thread, pname, params);
         break;
      default:
         RPC_CALL2_OUT_CTRL(glGetIntegerv_impl,
                            thread, 
                            GLGETINTEGERV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }
   else if (IS_OPENGLES_20(thread)) {
      switch (pname) {
      case GL_UNPACK_ALIGNMENT:
      case GL_PACK_ALIGNMENT:
         get_integer_internal_20(thread, pname, params);
         break;
      default:
         RPC_CALL2_OUT_CTRL(glGetIntegerv_impl,
                            thread, 
                            GLGETINTEGERV_ID,
                            RPC_ENUM(pname),
                            params);
         break;
      }
   }
}

GL_API void GL_APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetLightfv_impl_11,
                         thread, 
                         GLGETLIGHTFV_ID_11,
                         RPC_ENUM(light),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetLightxv (GLenum light, GLenum pname, GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetLightxv_impl_11,
                         thread, 
                         GLGETLIGHTXV_ID_11,
                         RPC_ENUM(light),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetMaterialfv_impl_11,
                         thread, 
                         GLGETMATERIALFV_ID_11,
                         RPC_ENUM(face),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetMaterialxv_impl_11,
                         thread, 
                         GLGETMATERIALXV_ID_11,
                         RPC_ENUM(face),
                         RPC_ENUM(pname),
                         params);
   }
}

/*
   VERTEX ARRAY POINTER GetPointerv
   NORMAL ARRAY POINTER GetPointerv
   COLOR ARRAY POINTER GetPointerv
   TEXTURE COORD ARRAY POINTER GetPointerv
   POINT SIZE ARRAY POINTER OES GetPointerv
*/

GL_API void GL_APIENTRY glGetPointerv (GLenum pname, GLvoid **params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      /*
         unhappy about casting away constness here
      */

      switch (pname) {
      case GL_VERTEX_ARRAY_POINTER:
         params[0] = (void *)state->attrib[GL11_IX_VERTEX].pointer;
         break;
      case GL_NORMAL_ARRAY_POINTER:
         params[0] = (void *)state->attrib[GL11_IX_NORMAL].pointer;
         break;
      case GL_COLOR_ARRAY_POINTER:
         params[0] = (void *)state->attrib[GL11_IX_COLOR].pointer;
         break;
      case GL_TEXTURE_COORD_ARRAY_POINTER:
         params[0] = (void *)state->attrib[state->active_texture.client - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].pointer;
         break;
      case GL_POINT_SIZE_ARRAY_POINTER_OES:
         params[0] = (void *)state->attrib[GL11_IX_POINT_SIZE].pointer;
         break;
      default:
         set_error(state, GL_INVALID_ENUM);
         break;
      }
   }
   else if (IS_OPENGLES_20(thread)) {
      /*
         no valid queries for glGetPointerv in OpenGL ES 2.0
      */

      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      set_error(state, GL_INVALID_ENUM);
   }
}

GL_APICALL void GL_APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetProgramiv_impl_20,
                         thread, 
                         GLGETPROGRAMIV_ID_20,
                         RPC_UINT(program),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei *length, char *infolog)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetProgramInfoLog_impl_20(program, bufsize, length, infolog);
   }
}

#define MAX_RENDERER_STRLEN 64
#define MAX_RENDERERVER_STRLEN 8

GL_API const GLubyte * GL_APIENTRY glGetString (GLenum name)
{
   static GLubyte rendererString[MAX_RENDERER_STRLEN];

   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      switch (name) {
      case GL_VENDOR:
#ifndef NDEBUG
         return (const GLubyte *)"Broadcom DEBUG";
#else
         return (const GLubyte *)"Broadcom";
#endif
      case GL_RENDERER:
         {
            GLubyte versionString[MAX_RENDERERVER_STRLEN];
            glintGetCoreRevision_impl(MAX_RENDERERVER_STRLEN, versionString);
            sprintf(rendererString, "VideoCore IV HW (V3D-%s)", versionString); 
            return (const GLubyte *)rendererString;
         }
      case GL_VERSION:
         return (const GLubyte *)"OpenGL ES-CM 1.1";
      case GL_EXTENSIONS:
         return (const GLubyte *)"GL_OES_compressed_ETC1_RGB8_texture GL_OES_compressed_paletted_texture GL_OES_texture_npot GL_OES_EGL_image GL_EXT_discard_framebuffer GL_OES_query_matrix GL_OES_framebuffer_object GL_OES_rgb8_rgba8 GL_OES_depth24 GL_OES_depth32 GL_OES_stencil8 GL_OES_draw_texture GL_OES_packed_depth_stencil GL_EXT_debug_marker GL_EXT_texture_format_BGRA8888 GL_EXT_read_format_bgra";  /*TODO is npot right? I can't find it in glext.h */
      default:
         set_error(state, GL_INVALID_ENUM);
         return NULL;
      }
   }
   else if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      switch (name) {
      case GL_VENDOR:
#ifndef NDEBUG
         return (const GLubyte *)"Broadcom DEBUG";
#else
         return (const GLubyte *)"Broadcom";
#endif
      case GL_RENDERER:
         {
            GLubyte versionString[MAX_RENDERERVER_STRLEN];
            glintGetCoreRevision_impl(MAX_RENDERERVER_STRLEN, versionString);
            sprintf(rendererString, "VideoCore IV HW (V3D-%s)", versionString); 
            return (const GLubyte *)rendererString;
         }
      case GL_VERSION:
         return (const GLubyte *)"OpenGL ES 2.0";
      case GL_SHADING_LANGUAGE_VERSION:
         return (const GLubyte *)"OpenGL ES GLSL ES 1.00";
      case GL_EXTENSIONS:
         return (const GLubyte *)"GL_OES_compressed_ETC1_RGB8_texture GL_OES_compressed_paletted_texture GL_OES_texture_npot GL_OES_depth24 GL_OES_vertex_half_float GL_OES_EGL_image GL_EXT_discard_framebuffer GL_OES_rgb8_rgba8 GL_OES_depth32 GL_APPLE_rgb_422 GL_OES_packed_depth_stencil GL_EXT_debug_marker GL_EXT_texture_format_BGRA8888 GL_EXT_read_format_bgra";
      default:
         set_error(state, GL_INVALID_ENUM);
         return NULL;
      }
   }

   return NULL;
}

GL_API void GL_APIENTRY glGetTexEnviv (GLenum env, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexEnviv_impl_11,
                         thread, 
                         GLGETTEXENVIV_ID_11,
                         RPC_ENUM(env),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexEnvfv_impl_11,
                         thread, 
                         GLGETTEXENVFV_ID_11,
                         RPC_ENUM(env),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexEnvxv_impl_11,
                         thread, 
                         GLGETTEXENVXV_ID_11,
                         RPC_ENUM(env),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexParameteriv_impl,
                         thread, 
                         GLGETTEXPARAMETERIV_ID,
                         RPC_ENUM(target),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexParameterfv_impl,
                         thread, 
                         GLGETTEXPARAMETERFV_ID,
                         RPC_ENUM(target),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_API void GL_APIENTRY glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3_OUT_CTRL(glGetTexParameterxv_impl_11,
                         thread, 
                         GLGETTEXPARAMETERXV_ID_11,
                         RPC_ENUM(target),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetUniformfv (GLuint program, GLint location, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetUniformfv_impl_20,
                         thread, 
                         GLGETUNIFORMFV_ID_20,
                         RPC_UINT(program),
                         RPC_INT(location),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetUniformiv (GLuint program, GLint location, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetUniformiv_impl_20,
                         thread, 
                         GLGETUNIFORMIV_ID_20,
                         RPC_UINT(program),
                         RPC_INT(location),
                         params);
   }
}

GL_APICALL int GL_APIENTRY glGetUniformLocation (GLuint program, const char *name)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_INT_RES(RPC_CALL2_IN_BULK_RES(glGetUniformLocation_impl_20,
                                               thread, 
                                               GLGETUNIFORMLOCATION_ID_20,
                                               RPC_UINT(program),
                                               name,
                                               strlen(name) + 1));
   }

   return 0;
}

/*
   GetVertexAttrib

   VERTEX ATTRIB ARRAY ENABLED False GetVertexAttrib
   VERTEX ATTRIB ARRAY SIZE 4 GetVertexAttrib
   VERTEX ATTRIB ARRAY STRIDE 0 GetVertexAttrib
   VERTEX ATTRIB ARRAY TYPE FLOAT GetVertexAttrib
   VERTEX ATTRIB ARRAY NORMALIZED False GetVertexAttrib
   VERTEX ATTRIB ARRAY BUFFER BINDING 0 GetVertexAttrib

   CURRENT VERTEX ATTRIB 0,0,0,1 GetVertexAttributes
*/

GL_APICALL void GL_APIENTRY glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (index < GLXX_CONFIG_MAX_VERTEX_ATTRIBS)
         switch (pname) {
         case GL_CURRENT_VERTEX_ATTRIB:
            params[0] = state->attrib[index].value[0];
            params[1] = state->attrib[index].value[1];
            params[2] = state->attrib[index].value[2];
            params[3] = state->attrib[index].value[3];
            break;

         //TODO: is this the best way to handle conversions? We duplicate
         //the entire switch statement.
         case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
            params[0] = state->attrib[index].enabled ? 1.0f : 0.0f;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_SIZE:
            params[0] = (GLfloat)state->attrib[index].size;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
            params[0] = (GLfloat)state->attrib[index].stride;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_TYPE:
            params[0] = (GLfloat)state->attrib[index].type;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
            params[0] = state->attrib[index].normalized ? 1.0f : 0.0f;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
            params[0] = (GLfloat)state->attrib[index].buffer;
            break;



         default:
            set_error(state, GL_INVALID_ENUM);
            break;
         }
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

GL_APICALL void GL_APIENTRY glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (index < GLXX_CONFIG_MAX_VERTEX_ATTRIBS)
         switch (pname) {
         case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
            params[0] = (GLint) state->attrib[index].enabled ? GL_TRUE : GL_FALSE;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_SIZE:
            params[0] = (GLint) state->attrib[index].size;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
            params[0] = (GLint) state->attrib[index].stride;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_TYPE:
            params[0] = (GLint) state->attrib[index].type;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
            params[0] = (GLint) state->attrib[index].normalized ? GL_TRUE : GL_FALSE;
            break;
         case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
            params[0] = (GLint) state->attrib[index].buffer;
            break;


         //TODO: is this the best way to handle conversions? We duplicate
         //the entire switch statement.
         case GL_CURRENT_VERTEX_ATTRIB:
            params[0] = (GLint)state->attrib[index].value[0];
            params[1] = (GLint)state->attrib[index].value[1];
            params[2] = (GLint)state->attrib[index].value[2];
            params[3] = (GLint)state->attrib[index].value[3];
            break;

         default:
            set_error(state, GL_INVALID_ENUM);
            break;
         }
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

/*
   GetVertexAttribPointer

   VERTEX ATTRIB ARRAY POINTER NULL GetVertexAttribPointer
*/

GL_APICALL void GL_APIENTRY glGetVertexAttribPointerv (GLuint index, GLenum pname, void **pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (index < GLXX_CONFIG_MAX_VERTEX_ATTRIBS)
         switch (pname) {
         case GL_VERTEX_ATTRIB_ARRAY_POINTER:
            pointer[0] = (void *)state->attrib[index].pointer;
            break;
         default:
            set_error(state, GL_INVALID_ENUM);
            break;
         }
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

GL_API void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glHint_impl,
                thread, 
                GLHINT_ID,
                RPC_ENUM(target),
                RPC_ENUM(mode));
   }
}

GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsBuffer_impl,
                                           thread, 
                                           GLISBUFFER_ID,
                                           RPC_UINT(buffer)));
   }

   return 0;
}

GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      switch (cap) {
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_POINT_SIZE_ARRAY_OES:
      case GL_TEXTURE_COORD_ARRAY:
      {
         GLboolean temp = 0;
         GLuint count = (GLuint) get_boolean_internal_11(thread, cap, &temp);
         UNUSED_NDEBUG(count);
         vcos_assert(count == 1);

         return temp;
      }
      default:
         return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsEnabled_impl,
                                              thread, 
                                              GLISENABLED_ID,
                                              RPC_ENUM(cap)));
      }
   }
   else if (IS_OPENGLES_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsEnabled_impl,
                                           thread, 
                                           GLISENABLED_ID,
                                           RPC_ENUM(cap)));
   }

   return 0;
}

GL_API GLboolean GL_APIENTRY glIsProgram (GLuint program)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsProgram_impl_20,
                                           thread, 
                                           GLISPROGRAM_ID_20,
                                           RPC_UINT(program)));
   }

   return 0;
}

GL_API GLboolean GL_APIENTRY glIsShader (GLuint shader)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsShader_impl_20,
                                           thread, 
                                           GLISSHADER_ID_20,
                                           RPC_UINT(shader)));
   }

   return 0;
}

GL_API GLboolean GL_APIENTRY glIsTexture (GLuint texture)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsTexture_impl,
                                           thread, 
                                           GLISTEXTURE_ID,
                                           RPC_UINT(texture)));
   }
   return 0;
}

GL_API void GL_APIENTRY glLightModelf (GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glLightModelf_impl_11,
                thread, 
                GLLIGHTMODELF_ID_11,
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported lighting model params are

         LIGHT_MODEL_AMBIENT (4)
         LIGHT_MODEL_TWO_SIDE (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glLightModelfv_impl_11,
                        thread, 
                        GLLIGHTMODELFV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glLightModelx (GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glLightModelx_impl_11,
                thread, 
                GLLIGHTMODELX_ID_11,
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glLightModelxv (GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported lighting model params are

         LIGHT_MODEL_AMBIENT (4)
         LIGHT_MODEL_TWO_SIDE (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glLightModelxv_impl_11,
                        thread, 
                        GLLIGHTMODELXV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glLightf_impl_11,
                thread, 
                GLLIGHTF_ID_11,
                RPC_ENUM(light),
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported light params are

         AMBIENT (4)
         DIFFUSE (4)
         SPECULAR (4)
         POSITION (4)
         SPOT_DIRECTION (3)
         SPOT_EXPONENT (1)
         SPOT_CUTOFF (1)
         CONSTANT_ATTENUATION (1)
         LINEAR_ATTENUATION (1)
         QUADRATIC_ATTENUATION (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glLightfv_impl_11,
                        thread, 
                        GLLIGHTFV_ID_11,
                        RPC_ENUM(light),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glLightx_impl_11,
                thread, 
                GLLIGHTX_ID_11,
                RPC_ENUM(light),
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported light params are

         AMBIENT (4)
         DIFFUSE (4)
         SPECULAR (4)
         POSITION (4)
         SPOT_DIRECTION (3)
         SPOT_EXPONENT (1)
         SPOT_CUTOFF (1)
         CONSTANT_ATTENUATION (1)
         LINEAR_ATTENUATION (1)
         QUADRATIC_ATTENUATION (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glLightxv_impl_11,
                        thread, 
                        GLLIGHTXV_ID_11,
                        RPC_ENUM(light),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glLineWidth (GLfloat width)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glLineWidth_impl,
                thread, 
                GLLINEWIDTH_ID,
                RPC_FLOAT(width));
   }
}

GL_API void GL_APIENTRY glLineWidthx (GLfixed width)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1(glLineWidthx_impl_11,
                thread, 
                GLLINEWIDTHX_ID_11,
                RPC_FIXED(width));
   }
}

GL_API void GL_APIENTRY glLinkProgram (GLuint program)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glLinkProgram_impl_20,
               thread, 
               GLLINKPROGRAM_ID_20,
               RPC_UINT(program));
   }
}

GL_API void GL_APIENTRY glLoadIdentity (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL0(glLoadIdentity_impl_11,
                thread, 
                GLLOADIDENTITY_ID_11);
   }
}

GL_API void GL_APIENTRY glLoadMatrixf (const GLfloat *m)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1_IN_CTRL(glLoadMatrixf_impl_11,
                       thread, 
                       GLLOADMATRIXF_ID_11,
                       m,
                       16 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glLoadMatrixx (const GLfixed *m)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1_IN_CTRL(glLoadMatrixx_impl_11,
                        thread, 
                        GLLOADMATRIXX_ID_11,
                        m,
                        16 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glLogicOp (GLenum opcode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1(glLogicOp_impl_11,
                thread, 
                GLLOGICOP_ID_11,
                RPC_ENUM(opcode));
   }
}

GL_API void GL_APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glMaterialf_impl_11,
                thread, 
                GLMATERIALF_ID_11,
                RPC_ENUM(face),
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported material params are

         AMBIENT (4)
         DIFFUSE (4)
         SPECULAR (4)
         EMISSION (4)
         SHININESS (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glMaterialfv_impl_11,
                        thread, 
                        GLMATERIALFV_ID_11,
                        RPC_ENUM(face),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glMaterialx_impl_11,
                thread, 
                GLMATERIALX_ID_11,
                RPC_ENUM(face),
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported material params are

         AMBIENT (4)
         DIFFUSE (4)
         SPECULAR (4)
         EMISSION (4)
         SHININESS (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glMaterialxv_impl_11,
                        thread, 
                        GLMATERIALXV_ID_11,
                        RPC_ENUM(face),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glMatrixMode (GLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1(glMatrixMode_impl_11,
                thread, 
                GLMATRIXMODE_ID_11,
                RPC_ENUM(mode));
   }
}

GL_API void GL_APIENTRY glMultMatrixf (const GLfloat *m)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1_IN_CTRL(glMultMatrixf_impl_11,
                        thread, 
                        GLMULTMATRIXF_ID_11,
                        m,
                        16 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glMultMatrixx (const GLfixed *m)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1_IN_CTRL(glMultMatrixx_impl_11,
                        thread, 
                        GLMULTMATRIXX_ID_11,
                        m,
                        16 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (target >= GL_TEXTURE0 && target < GL_TEXTURE0 + GL11_CONFIG_MAX_TEXTURE_UNITS) {
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[0] = clean_float(s);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[1] = clean_float(t);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[2] = clean_float(r);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[3] = clean_float(q);

      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (target >= GL_TEXTURE0 && target < GL_TEXTURE0 + GL11_CONFIG_MAX_TEXTURE_UNITS) {
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[0] = fixed_to_float(s);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[1] = fixed_to_float(t);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[2] = fixed_to_float(r);
         state->attrib[target - GL_TEXTURE0 + GL11_IX_TEXTURE_COORD].value[3] = fixed_to_float(q);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      state->attrib[GL11_IX_NORMAL].value[0] = clean_float(nx);
      state->attrib[GL11_IX_NORMAL].value[1] = clean_float(ny);
      state->attrib[GL11_IX_NORMAL].value[2] = clean_float(nz);
   }
}

GL_API void GL_APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      state->attrib[GL11_IX_NORMAL].value[0] = fixed_to_float(nx);
      state->attrib[GL11_IX_NORMAL].value[1] = fixed_to_float(ny);
      state->attrib[GL11_IX_NORMAL].value[2] = fixed_to_float(nz);

   }
}

static GLboolean is_normal_type(GLenum type)
{
   return type == GL_BYTE ||
          type == GL_SHORT ||
          type == GL_FIXED ||
          type == GL_FLOAT;
}

GL_API void GL_APIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (is_normal_type(type)) {
         if (is_aligned(type, (size_t)pointer) && is_aligned(type, (size_t)stride) && stride >= 0) {

            state->attrib[GL11_IX_NORMAL].type = type;
            state->attrib[GL11_IX_NORMAL].stride = stride;
            state->attrib[GL11_IX_NORMAL].pointer = pointer;
            state->attrib[GL11_IX_NORMAL].buffer = state->bound_buffer.array;

            RPC_CALL0(glNormalPointer_impl_11,
                      thread, 
                      GLNORMALPOINTER_ID_11);
         } else
            set_error(state, GL_INVALID_VALUE);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL6(glOrthof_impl_11,
                thread, 
                GLORTHOF_ID_11,
                RPC_FLOAT(left),
                RPC_FLOAT(right),
                RPC_FLOAT(bottom),
                RPC_FLOAT(top),
                RPC_FLOAT(zNear),
                RPC_FLOAT(zFar));
   }
}

GL_API void GL_APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL6(glOrthox_impl_11,
                thread, 
                GLORTHOX_ID_11,
                RPC_FIXED(left),
                RPC_FIXED(right),
                RPC_FIXED(bottom),
                RPC_FIXED(top),
                RPC_FIXED(zNear),
                RPC_FIXED(zFar));
   }
}

static GLboolean is_alignment(GLint param)
{
   return param == 1 ||
          param == 2 ||
          param == 4 ||
          param == 8;
}

GL_API void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      if (is_alignment(param)) {
         GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

         vcos_assert(state != NULL);

         switch (pname) {
         case GL_PACK_ALIGNMENT:
            state->alignment.pack = param;
            break;
         case GL_UNPACK_ALIGNMENT:
            state->alignment.unpack = param;
            break;
         }
      }
   }
}

GL_API void GL_APIENTRY glPointParameterf (GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glPointParameterf_impl_11,
                thread, 
                GLPOINTPARAMETERF_ID_11,
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported material params are

         POINT_SIZE_MIN (1)
         POINT_SIZE_MAX (1)
         POINT_FADE_THRESHOLD_SIZE (1)
         POINT_DISTANCE_ATTENUATION (3)

         so we need to transmit 3 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glPointParameterfv_impl_11,
                        thread, 
                        GLPOINTPARAMETERFV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        3 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glPointParameterx (GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glPointParameterx_impl_11,
                thread, 
                GLPOINTPARAMETERX_ID_11,
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glPointParameterxv (GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported material params are

         POINT_SIZE_MIN (1)
         POINT_SIZE_MAX (1)
         POINT_FADE_THRESHOLD_SIZE (1)
         POINT_DISTANCE_ATTENUATION (3)

         so we need to transmit 3 words of parameter data
      */

      RPC_CALL2_IN_CTRL(glPointParameterxv_impl_11,
                        thread, 
                        GLPOINTPARAMETERXV_ID_11,
                        RPC_ENUM(pname),
                        params,
                        3 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glPointSize (GLfloat size)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      size = clean_float(size);

      if (size > 0.0f)
         state->attrib[GL11_IX_POINT_SIZE].value[0] = size;
      else
         set_error(state, GL_INVALID_VALUE);
   }
   else if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glPointSize_impl_20,
                thread, 
                GLPOINTSIZE_ID_20,
                RPC_FLOAT(size));
   }
}

GL_API void GL_APIENTRY glPointSizex (GLfixed size)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (size > 0)
         state->attrib[GL11_IX_POINT_SIZE].value[0] = fixed_to_float(size);
      else
         set_error(state, GL_INVALID_VALUE);
   }
}

GL_API void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glPolygonOffset_impl,
                thread, 
                GLPOLYGONOFFSET_ID,
                RPC_FLOAT(factor),
                RPC_FLOAT(units));
   }
}

GL_API void GL_APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glPolygonOffsetx_impl_11,
                thread, 
                GLPOLYGONOFFSETX_ID_11,
                RPC_FIXED(factor),
                RPC_FIXED(units));
   }
}

GL_API void GL_APIENTRY glPopMatrix (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL0(glPopMatrix_impl_11,
                thread, 
                GLPOPMATRIX_ID_11);
   }
}

GL_API void GL_APIENTRY glPushMatrix (void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL0(glPushMatrix_impl_11,
                thread, 
                GLPUSHMATRIX_ID_11);
   }
}

/*
   we need to calculate on the client side how much data to transfer to the
   server on a call to glTexImage2D()

   from section 3.6 of the OpenGL ES 1.1 spec

   the first element of the Nth row is indicated by

   p + Nk

   where N is the row number (counting from zero) and k is defined as

   k = nl                  if s >= a
     = a/s * ceil(snl/a)   otherwise

   where n is the number of elements in a group, l is the number of groups in
   the row, a is the value of UNPACK ALIGNMENT, and s is the size, in units of GL
   ubytes, of an element.

   this code is
*/

static uint32_t get_pitch(uint32_t w, GLenum format, GLenum type, uint32_t a)
{
   uint32_t n = 0;
   uint32_t s = 0;
   uint32_t k = 0;

   switch (format) {
   case GL_RGBA:
#if GL_EXT_texture_format_BGRA8888
   case GL_BGRA_EXT:
#endif
      switch (type) {
      case GL_UNSIGNED_BYTE:
         n = 4;
         s = 1;
         break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_5_5_5_1:
         n = 1;
         s = 2;
         break;
      }
      break;
   case GL_RGB:
      switch (type) {
      case GL_UNSIGNED_BYTE:
         n = 3;
         s = 1;
         break;
      case GL_UNSIGNED_SHORT_5_6_5:
         n = 1;
         s = 2;
         break;
      }
      break;
   case GL_LUMINANCE_ALPHA:
      n = 2;
      s = 1;
      break;
   case GL_LUMINANCE:
   case GL_ALPHA:
      n = 1;
      s = 1;
      break;
#if GL_APPLE_rgb_422
   case GL_RGB_422_APPLE:
      n = 1;
      s = 2;
      break;
#endif
   }

   if (s != 0) {   /* Avoid division by zero errors on invalid formats */
      if (s < a)
         k = (a / s) * ((s * n * w + a - 1) / a);
      else
         k = n * w;
   }

   switch (format) {
   case GL_RGBA:
#if GL_EXT_texture_format_BGRA8888
   case GL_BGRA_EXT:
#endif
      switch (type) {
      case GL_UNSIGNED_BYTE:
         return k;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_5_5_5_1:
         return k * 2;
      }
      break;
   case GL_RGB:
      switch (type) {
      case GL_UNSIGNED_BYTE:
         return k;
      case GL_UNSIGNED_SHORT_5_6_5:
         return k * 2;
      }
      break;
   case GL_LUMINANCE_ALPHA:
   case GL_LUMINANCE:
   case GL_ALPHA:
      return k;
      break;
#if GL_APPLE_rgb_422
   case GL_RGB_422_APPLE:
      return k * 2;
      break;
#endif
   }

   return 0;      // transfer no data, format will be rejected by server
}

GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      uint32_t pitch = get_pitch( (uint32_t)width, format, type, (uint32_t) state->alignment.pack);
      uint32_t lines = pitch ? (uint32_t) (KHDISPATCH_WORKSPACE_SIZE / pitch) : (uint32_t)height;

      if (pixels && lines) {
         int offset = 0;

         while (height > 0) {
            int32_t batch = _min(lines, (int32_t) height);

            RPC_CALL8_OUT_BULK(glReadPixels_impl,
                               thread, 
                               GLREADPIXELS_ID,
                               RPC_INT(x),
                               RPC_INT(y + offset),
                               RPC_SIZEI(width),
                               RPC_SIZEI(batch),
                               RPC_ENUM(format),
                               RPC_ENUM(type),
                               RPC_INT(state->alignment.pack),
                               (char *)pixels + offset * pitch);

            offset += batch;
            height -= batch;
         }
      }

      // We do not call flush_callback as the spec does not imply a full flush
      // at this point (I think).
   }
}

GL_API void GL_APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL4(glRotatef_impl_11,
                thread, 
                GLROTATEF_ID_11,
                RPC_FLOAT(angle),
                RPC_FLOAT(x),
                RPC_FLOAT(y),
                RPC_FLOAT(z));
   }
}

GL_API void GL_APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL4(glRotatex_impl_11,
                thread, 
                GLROTATEX_ID_11,
                RPC_FIXED(angle),
                RPC_FIXED(x),
                RPC_FIXED(y),
                RPC_FIXED(z));
   }
}

GL_API void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glSampleCoverage_impl,
                thread, 
                GLSAMPLECOVERAGE_ID,
                RPC_FLOAT(value),
                RPC_BOOLEAN(invert));
   }
}

GL_API void GL_APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL2(glSampleCoveragex_impl_11,
                thread, 
                GLSAMPLECOVERAGEX_ID_11,
                RPC_FIXED(value),
                RPC_BOOLEAN(invert));
   }
}

GL_API void GL_APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glScalef_impl_11,
                thread, 
                GLSCALEF_ID_11,
                RPC_FLOAT(x),
                RPC_FLOAT(y),
                RPC_FLOAT(z));
   }
}

GL_API void GL_APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glScalex_impl_11,
                thread, 
                GLSCALEX_ID_11,
                RPC_FIXED(x),
                RPC_FIXED(y),
                RPC_FIXED(z));
   }
}

GL_API void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glScissor_impl,
                thread, 
                GLSCISSOR_ID,
                RPC_INT(x),
                RPC_INT(y),
                RPC_SIZEI(width),
                RPC_SIZEI(height));
   }
}

GL_API void GL_APIENTRY glShadeModel (GLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL1(glShadeModel_impl_11,
                thread, 
                GLSHADEMODEL_ID_11,
                RPC_ENUM(mode));
   }
}

static void set_stencil_func(CLIENT_THREAD_STATE_T *thread, GLenum face, GLenum func, GLint ref, GLuint mask) {
   RPC_CALL4(glStencilFuncSeparate_impl,
             thread, 
             GLSTENCILFUNCSEPARATE_ID,
             RPC_ENUM(face),
             RPC_ENUM(func),
             RPC_INT(ref),
             RPC_UINT(mask));
}

GL_API void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) set_stencil_func(thread, GL_FRONT_AND_BACK, func, ref, mask);
}

GL_API void GL_APIENTRY glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) set_stencil_func(thread, face, func, ref, mask);
}

static void set_stencil_mask(CLIENT_THREAD_STATE_T *thread, GLenum face, GLuint mask) {
   RPC_CALL2(glStencilMaskSeparate_impl,
             thread, 
             GLSTENCILMASKSEPARATE_ID,
             RPC_ENUM(face),
             RPC_UINT(mask));
}

GL_API void GL_APIENTRY glStencilMask (GLuint mask)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) set_stencil_mask(thread, GL_FRONT_AND_BACK, mask);
}

GL_API void GL_APIENTRY glStencilMaskSeparate (GLenum face, GLuint mask) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) set_stencil_mask(thread, face, mask);
}

static void set_stencil_op(CLIENT_THREAD_STATE_T *thread, GLenum face, GLenum fail, GLenum zfail, GLenum zpass) {
   RPC_CALL4(glStencilOpSeparate_impl,
             thread, 
             GLSTENCILOPSEPARATE_ID,
             RPC_ENUM(face),
             RPC_ENUM(fail),
             RPC_ENUM(zfail),
             RPC_ENUM(zpass));
}

GL_API void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) set_stencil_op(thread, GL_FRONT_AND_BACK, fail, zfail, zpass);
}

GL_API void GL_APIENTRY glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) set_stencil_op(thread, face, fail, zfail, zpass);
}

static GLboolean is_texture_coord_size(GLint size)
{
   return size == 2 ||
          size == 3 ||
          size == 4;
}

static GLboolean is_texture_coord_type(GLenum type)
{
   return type == GL_BYTE ||
          type == GL_SHORT ||
          type == GL_FIXED ||
          type == GL_FLOAT;
}

GL_API void GL_APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (is_texture_coord_type(type)) {
         if (is_texture_coord_size(size) && is_aligned(type, (size_t)pointer) && is_aligned(type, (size_t)stride) && stride >= 0) {

            int indx = GL11_IX_TEXTURE_COORD + state->active_texture.client - GL_TEXTURE0;

            state->attrib[indx].size = size;
            state->attrib[indx].type = type;
            state->attrib[indx].stride = stride;
            state->attrib[indx].pointer = pointer;
            state->attrib[indx].buffer = state->bound_buffer.array;

            RPC_CALL1(glTexCoordPointer_impl_11,
                      thread, 
                      GLTEXCOORDPOINTER_ID_11,
                      RPC_ENUM(state->active_texture.client));
         } else
            set_error(state, GL_INVALID_VALUE);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTexEnvi_impl_11,
                thread, 
                GLTEXENVI_ID_11,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_INT(param));
   }
}

GL_API void GL_APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported texture environment params are

         COORD_REPLACE_OES (1)
         TEXTURE_ENV_MODE (1)
         TEXTURE_ENV_COLOR (4)
         COMBINE_RGB (1)
         RGB_SCALE (1)
         SRC0_RGB (1)
         SRC1_RGB (1)
         SRC2_RGB (1)
         OPERAND0_RGB (1)
         OPERAND1_RGB (1)
         OPERAND2_RGB (1)
         COMBINE_ALPHA (1)
         ALPHA_SCALE (1)
         SRC0_ALPHA (1)
         SRC1_ALPHA (1)
         SRC2_ALPHA (1)
         OPERAND0_ALPHA (1)
         OPERAND1_ALPHA (1)
         OPERAND2_ALPHA (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glTexEnviv_impl_11,
                        thread, 
                        GLTEXENVIV_ID_11,
                        RPC_ENUM(target),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLint));
   }
}

GL_API void GL_APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTexEnvf_impl_11,
                thread, 
                GLTEXENVF_ID_11,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported texture environment params are

         COORD_REPLACE_OES (1)
         TEXTURE_ENV_MODE (1)
         TEXTURE_ENV_COLOR (4)
         COMBINE_RGB (1)
         RGB_SCALE (1)
         SRC0_RGB (1)
         SRC1_RGB (1)
         SRC2_RGB (1)
         OPERAND0_RGB (1)
         OPERAND1_RGB (1)
         OPERAND2_RGB (1)
         COMBINE_ALPHA (1)
         ALPHA_SCALE (1)
         SRC0_ALPHA (1)
         SRC1_ALPHA (1)
         SRC2_ALPHA (1)
         OPERAND0_ALPHA (1)
         OPERAND1_ALPHA (1)
         OPERAND2_ALPHA (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glTexEnvfv_impl_11,
                        thread, 
                        GLTEXENVFV_ID_11,
                        RPC_ENUM(target),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfloat));
   }
}

GL_API void GL_APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTexEnvx_impl_11,
                thread, 
                GLTEXENVX_ID_11,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      /*
         the only supported texture environment params are

         COORD_REPLACE_OES (1)
         TEXTURE_ENV_MODE (1)
         TEXTURE_ENV_COLOR (4)
         COMBINE_RGB (1)
         RGB_SCALE (1)
         SRC0_RGB (1)
         SRC1_RGB (1)
         SRC2_RGB (1)
         OPERAND0_RGB (1)
         OPERAND1_RGB (1)
         OPERAND2_RGB (1)
         COMBINE_ALPHA (1)
         ALPHA_SCALE (1)
         SRC0_ALPHA (1)
         SRC1_ALPHA (1)
         SRC2_ALPHA (1)
         OPERAND0_ALPHA (1)
         OPERAND1_ALPHA (1)
         OPERAND2_ALPHA (1)

         so we need to transmit 4 words of parameter data
      */

      RPC_CALL3_IN_CTRL(glTexEnvxv_impl_11,
                        thread, 
                        GLTEXENVXV_ID_11,
                        RPC_ENUM(target),
                        RPC_ENUM(pname),
                        params,
                        4 * sizeof(GLfixed));
   }
}

GL_API void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   GLboolean res;
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      uint32_t pitch = get_pitch( (uint32_t)width, format, type, (uint32_t)state->alignment.unpack);
      uint32_t lines = pitch ? (uint32_t)(KHDISPATCH_WORKSPACE_SIZE / pitch) : (uint32_t)height;

      res = RPC_BOOLEAN_RES(RPC_CALL10_IN_BULK_RES(glTexImage2D_impl,
                        thread, 
                        GLTEXIMAGE2D_ID,
                        RPC_ENUM(target),
                        RPC_INT(level),
                        RPC_ENUM(internalformat),
                        RPC_SIZEI(width),
                        RPC_SIZEI(height),
                        RPC_INT(border),
                        RPC_ENUM(format),
                        RPC_ENUM(type),
                        RPC_INT(state->alignment.unpack),
                        NULL,
                        0));

      if (res && pixels && lines) {
         int offset = 0;

         while (height > 0) {
            int32_t batch = _min(lines, (int32_t)height);

            RPC_CALL10_IN_BULK(glTexSubImage2D_impl,
                              thread, 
                              GLTEXSUBIMAGE2D_ID,
                              RPC_ENUM(target),
                              RPC_INT(level),
                              RPC_INT(0),
                              RPC_INT(offset),
                              RPC_SIZEI(width),
                              RPC_SIZEI(batch),
                              RPC_ENUM(format),
                              RPC_ENUM(type),
                              RPC_INT(state->alignment.unpack),
                              (char *)pixels + offset * pitch,
                              batch * pitch);

            offset += batch;
            height -= batch;
         }
      }
   }
}

GL_API void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3(glTexParameteri_impl,
                thread, 
                GLTEXPARAMETERI_ID,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_INT(param));
   }
}

GL_API void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3(glTexParameterf_impl,
                thread, 
                GLTEXPARAMETERF_ID,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_FLOAT(param));
   }
}

GL_API void GL_APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTexParameterx_impl_11,
                thread, 
                GLTEXPARAMETERX_ID_11,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                RPC_FIXED(param));
   }
}

GL_API void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   /*
      the only supported texture params are

      TEXTURE_MIN_FILTER
      TEXTURE_MAG_FILTER
      TEXTURE_WRAP_S
      TEXTURE_WRAP_T

      each of which takes a single argument
      
      and for 1.1
      TEXTURE_CROP_RECT_OES 
      which takes 4 ints
   */

   if (IS_OPENGLES_11(thread)) {
      if(pname != GL_TEXTURE_CROP_RECT_OES) {
         glTexParameteri(target, pname, params[0]);
      }
      else {
         RPC_CALL3_IN_CTRL(glTexParameteriv_impl,
                thread, 
                GLTEXPARAMETERIV_ID,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                params,
                4 * sizeof(GLint));
      }
   }
   else if(IS_OPENGLES_20(thread))
      glTexParameteri(target, pname, params[0]); 
}

GL_API void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   /*
      the only supported texture params are

      TEXTURE_MIN_FILTER
      TEXTURE_MAG_FILTER
      TEXTURE_WRAP_S
      TEXTURE_WRAP_T

      each of which takes a single argument
      
      and for 1.1
      TEXTURE_CROP_RECT_OES 
      which takes 4 ints
   */

   if (IS_OPENGLES_11(thread)) {
      if(pname != GL_TEXTURE_CROP_RECT_OES) {
         glTexParameterf(target, pname, params[0]);
      }
      else {
         RPC_CALL3_IN_CTRL(glTexParameterfv_impl,
                thread, 
                GLTEXPARAMETERFV_ID,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                params,
                4 * sizeof(GLfloat));
      }
   }
   else if(IS_OPENGLES_20(thread))
      glTexParameterf(target, pname, params[0]); 
}

GL_API void GL_APIENTRY glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   /*
      the only supported texture params are

      TEXTURE_MIN_FILTER
      TEXTURE_MAG_FILTER
      TEXTURE_WRAP_S
      TEXTURE_WRAP_T

      each of which takes a single argument
      
      and for 1.1
      TEXTURE_CROP_RECT_OES 
      which takes 4 ints
   */

   if (IS_OPENGLES_11(thread)) {
      if(pname != GL_TEXTURE_CROP_RECT_OES) {
         glTexParameterx(target, pname, params[0]);
      }
      else {
         RPC_CALL3_IN_CTRL(glTexParameterxv_impl_11,
                thread, 
                GLTEXPARAMETERXV_ID_11,
                RPC_ENUM(target),
                RPC_ENUM(pname),
                params,
                4 * sizeof(GLfixed));
      }
   }
   else if(IS_OPENGLES_20(thread))
      glTexParameterx(target, pname, params[0]); 
}

GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      uint32_t pitch = get_pitch( (uint32_t)width, format, type, (uint32_t)state->alignment.unpack);
      uint32_t lines = pitch ? (uint32_t)(KHDISPATCH_WORKSPACE_SIZE / pitch) : (uint32_t)height;

      if (pixels && lines) {
         int offset = 0;

         while (height > 0) {
            int32_t batch = _min(lines, (int32_t)height);

            RPC_CALL10_IN_BULK(glTexSubImage2D_impl,
                              thread, 
                              GLTEXSUBIMAGE2D_ID,
                              RPC_ENUM(target),
                              RPC_INT(level),
                              RPC_INT(xoffset),
                              RPC_INT(yoffset+offset),
                              RPC_SIZEI(width),
                              RPC_SIZEI(batch),
                              RPC_ENUM(format),
                              RPC_ENUM(type),
                              RPC_INT(state->alignment.unpack),
                              (char *)pixels + offset * pitch,
                              batch * pitch);

            offset += batch;
            height -= batch;
         }
      }
   }
}

GL_API void GL_APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTranslatef_impl_11,
                thread, 
                GLTRANSLATEF_ID_11,
                RPC_FLOAT(x),
                RPC_FLOAT(y),
                RPC_FLOAT(z));
   }
}

GL_API void GL_APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      RPC_CALL3(glTranslatex_impl_11,
                thread, 
                GLTRANSLATEX_ID_11,
                RPC_FIXED(x),
                RPC_FIXED(y),
                RPC_FIXED(z));
   }
}

GL_API void GL_APIENTRY glUniform1i (GLint location, GLint x)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glUniform1i_impl_20,
                thread, 
                GLUNIFORM1I_ID_20,
                RPC_INT(location),
                RPC_INT(x));
   }
}

GL_API void GL_APIENTRY glUniform2i (GLint location, GLint x, GLint y)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3(glUniform2i_impl_20,
                thread, 
                GLUNIFORM2I_ID_20,
                RPC_INT(location),
                RPC_INT(x),
                RPC_INT(y));
   }
}

GL_API void GL_APIENTRY glUniform3i (GLint location, GLint x, GLint y, GLint z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL4(glUniform3i_impl_20,
                thread, 
                GLUNIFORM3I_ID_20,
                RPC_INT(location),
                RPC_INT(x),
                RPC_INT(y),
                RPC_INT(z));
   }
}

GL_API void GL_APIENTRY glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL5(glUniform4i_impl_20,
                thread, 
                GLUNIFORM4I_ID_20,
                RPC_INT(location),
                RPC_INT(x),
                RPC_INT(y),
                RPC_INT(z),
                RPC_INT(w));
   }
}

GL_API void GL_APIENTRY glUniform1f (GLint location, GLfloat x)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL2(glUniform1f_impl_20,
                thread, 
                GLUNIFORM1F_ID_20,
                RPC_INT(location),
                RPC_FLOAT(x));
   }
}

GL_API void GL_APIENTRY glUniform2f (GLint location, GLfloat x, GLfloat y)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3(glUniform2f_impl_20,
                thread, 
                GLUNIFORM2F_ID_20,
                RPC_INT(location),
                RPC_FLOAT(x),
                RPC_FLOAT(y));
   }
}

GL_API void GL_APIENTRY glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL4(glUniform3f_impl_20,
                thread, 
                GLUNIFORM3F_ID_20,
                RPC_INT(location),
                RPC_FLOAT(x),
                RPC_FLOAT(y),
                RPC_FLOAT(z));
   }
}

GL_API void GL_APIENTRY glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL5(glUniform4f_impl_20,
                thread, 
                GLUNIFORM4F_ID_20,
                RPC_INT(location),
                RPC_FLOAT(x),
                RPC_FLOAT(y),
                RPC_FLOAT(z),
                RPC_FLOAT(w));
   }
}

/*
   clamp the size of uniform data to the maximum conceivable value (128 vec4s)
*/

#define MAX_UNIFORM_SIZE 2048

static INLINE int clamp_uniform_size(int size)
{
   return (int) _min( (int32_t)size, MAX_UNIFORM_SIZE);
}

GL_API void GL_APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 1 * sizeof(GLint)));

      RPC_CALL4_IN_CTRL(glUniform1iv_impl_20,
                        thread, 
                        GLUNIFORM1IV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 2 * sizeof(GLint)));

      RPC_CALL4_IN_CTRL(glUniform2iv_impl_20,
                        thread, 
                        GLUNIFORM2IV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 3 * sizeof(GLint)));

      RPC_CALL4_IN_CTRL(glUniform3iv_impl_20,
                        thread, 
                        GLUNIFORM3IV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 4 * sizeof(GLint)));

      RPC_CALL4_IN_CTRL(glUniform4iv_impl_20,
                        thread, 
                        GLUNIFORM4IV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 1 * sizeof(GLfloat)));

      RPC_CALL4_IN_CTRL(glUniform1fv_impl_20,
                        thread, 
                        GLUNIFORM1FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 2 * sizeof(GLfloat)));

      RPC_CALL4_IN_CTRL(glUniform2fv_impl_20,
                        thread, 
                        GLUNIFORM2FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 3 * sizeof(GLfloat)));

      RPC_CALL4_IN_CTRL(glUniform3fv_impl_20,
                        thread, 
                        GLUNIFORM3FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat *v)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 4 * sizeof(GLfloat)));

      RPC_CALL4_IN_CTRL(glUniform4fv_impl_20,
                        thread, 
                        GLUNIFORM4FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_INT(size),
                        v,
                        (size_t)size);
   }
}

/*
   If transpose is GL_FALSE, each matrix is assumed to be supplied in column major order.
   If transpose is GL_TRUE, each matrix is assumed to be supplied in row major order.
*/

GL_API void GL_APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 2 * 2 * sizeof(GLfloat)));

      RPC_CALL5_IN_CTRL(glUniformMatrix2fv_impl_20,
                        thread, 
                        GLUNIFORMMATRIX2FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_BOOLEAN(transpose),
                        RPC_INT(size),
                        value,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 3 * 3 * sizeof(GLfloat)));

      RPC_CALL5_IN_CTRL(glUniformMatrix3fv_impl_20,
                        thread, 
                        GLUNIFORMMATRIX3FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_BOOLEAN(transpose),
                        RPC_INT(size),
                        value,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      int size = clamp_uniform_size( (int)(count * 4 * 4 * sizeof(GLfloat)));

      RPC_CALL5_IN_CTRL(glUniformMatrix4fv_impl_20,
                        thread,
                        GLUNIFORMMATRIX4FV_ID_20,
                        RPC_INT(location),
                        RPC_SIZEI(count),
                        RPC_BOOLEAN(transpose),
                        RPC_INT(size),
                        value,
                        (size_t)size);
   }
}

GL_API void GL_APIENTRY glUseProgram (GLuint program) // S
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glUseProgram_impl_20,
                thread, 
                GLUSEPROGRAM_ID_20,
                RPC_UINT(program));
   }
}

GL_API void GL_APIENTRY glValidateProgram (GLuint program)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glValidateProgram_impl_20,
                thread, 
                GLVALIDATEPROGRAM_ID_20,
                RPC_UINT(program));
   }
}

GL_APICALL void GL_APIENTRY glVertexAttrib1f (GLuint indx, GLfloat x)
{
   glVertexAttrib4f(indx, x, 0.0f, 0.0f, 1.0f);
}

GL_APICALL void GL_APIENTRY glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y)
{
   glVertexAttrib4f(indx, x, y, 0.0f, 1.0f);
}

GL_APICALL void GL_APIENTRY glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
   glVertexAttrib4f(indx, x, y, z, 1.0f);
}

GL_APICALL void GL_APIENTRY glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (indx < GLXX_CONFIG_MAX_VERTEX_ATTRIBS) {
         state->attrib[indx].value[0] = clean_float(x);
         state->attrib[indx].value[1] = clean_float(y);
         state->attrib[indx].value[2] = clean_float(z);
         state->attrib[indx].value[3] = clean_float(w);
      } else
         set_error(state, GL_INVALID_VALUE);
   }
}

GL_APICALL void GL_APIENTRY glVertexAttrib1fv (GLuint indx, const GLfloat *values)
{
   glVertexAttrib1f(indx, values[0]);
}

GL_APICALL void GL_APIENTRY glVertexAttrib2fv (GLuint indx, const GLfloat *values)
{
   glVertexAttrib2f(indx, values[0], values[1]);
}

GL_APICALL void GL_APIENTRY glVertexAttrib3fv (GLuint indx, const GLfloat *values)
{
   glVertexAttrib3f(indx, values[0], values[1], values[2]);
}

GL_APICALL void GL_APIENTRY glVertexAttrib4fv (GLuint indx, const GLfloat *values)
{
   glVertexAttrib4f(indx, values[0], values[1], values[2], values[3]);
}

static GLboolean is_vertex_attrib_size(GLint size)
{
   return size >= 1 && size <= 4;
}

static GLboolean is_vertex_attrib_type(GLenum type)
{
   return type == GL_BYTE ||
          type == GL_UNSIGNED_BYTE ||
          type == GL_SHORT ||
          type == GL_UNSIGNED_SHORT ||
          type == GL_FLOAT ||
          type == GL_FIXED;
}

GL_APICALL void GL_APIENTRY glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *ptr)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   if(IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);


      vcos_assert(state != NULL);

      if (indx < GLXX_CONFIG_MAX_VERTEX_ATTRIBS && is_vertex_attrib_size(size) && stride >= 0) {
         if (is_vertex_attrib_type(type) || type == GL_HALF_FLOAT_OES) {
            state->attrib[indx].size = size;
            state->attrib[indx].type = type;
            state->attrib[indx].normalized = normalized;
            state->attrib[indx].stride = stride;
            state->attrib[indx].pointer = ptr;
            state->attrib[indx].buffer = state->bound_buffer.array;

            RPC_CALL1(glVertexAttribPointer_impl_20,
                      thread,
                      GLVERTEXATTRIBPOINTER_ID_20,
                      RPC_UINT(indx));
         } else
            set_error(state, GL_INVALID_ENUM);
      } else
         set_error(state, GL_INVALID_VALUE);
   }
}

static GLboolean is_vertex_size(GLint size)
{
   return size == 2 ||
          size == 3 ||
          size == 4;
}

static GLboolean is_vertex_type(GLenum type)
{
   return type == GL_BYTE ||
          type == GL_SHORT ||
          type == GL_FIXED ||
          type == GL_FLOAT;
}

GL_API void GL_APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      vcos_assert(state != NULL);

      if (is_vertex_type(type)) {
         if (is_vertex_size(size) && is_aligned(type, (size_t)pointer) && is_aligned(type, (size_t)stride) && stride >= 0) {

            state->attrib[GL11_IX_VERTEX].size = size;
            state->attrib[GL11_IX_VERTEX].type = type;
            state->attrib[GL11_IX_VERTEX].stride = stride;
            state->attrib[GL11_IX_VERTEX].pointer = pointer;
            state->attrib[GL11_IX_VERTEX].buffer = state->bound_buffer.array;

            RPC_CALL0(glVertexPointer_impl_11,
                      thread, 
                      GLVERTEXPOINTER_ID_11);
         } else
            set_error(state, GL_INVALID_VALUE);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

GL_API void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glViewport_impl,
                thread, 
                GLVIEWPORT_ID,
                RPC_INT(x),
                RPC_INT(y),
                RPC_SIZEI(width),
                RPC_SIZEI(height));
   }
}
/*****************************************************************************************/
/*                             OES extension functions                           */
/*****************************************************************************************/

static GLboolean is_point_size_type(GLenum type)
{
   return type == GL_FIXED ||
          type == GL_FLOAT;
}

GL_API void GL_APIENTRY glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);

      if (is_point_size_type(type)) {
         if (is_aligned(type, (size_t)pointer) && is_aligned(type, (size_t)stride) && stride >= 0) {

            state->attrib[GL11_IX_POINT_SIZE].type = type;
            state->attrib[GL11_IX_POINT_SIZE].stride = stride;
            state->attrib[GL11_IX_POINT_SIZE].pointer = pointer;
            state->attrib[GL11_IX_POINT_SIZE].buffer = state->bound_buffer.array;

            RPC_CALL0(glPointSizePointerOES_impl_11,
                      thread, 
                      GLPOINTSIZEPOINTEROES_ID_11);
         } else
            set_error(state, GL_INVALID_VALUE);
      } else
         set_error(state, GL_INVALID_ENUM);
   }
}

/* OES_shader_source */
GL_APICALL void GL_APIENTRY glCompileShader (GLuint shader)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL1(glCompileShader_impl_20,
                thread, 
                GLCOMPILESHADER_ID_20,
                RPC_UINT(shader));
   }
}

GL_APICALL void GL_APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetShaderiv_impl_20,
                         thread, 
                         GLGETSHADERIV_ID_20,
                         RPC_ENUM(shader),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei *length, char *infolog)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetShaderInfoLog_impl_20(shader, bufsize, length, infolog);
   }
}

GL_APICALL void GL_APIENTRY glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei *length, char *source)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      glGetShaderSource_impl_20(shader, bufsize, length, source);
   }
}

GL_APICALL void GL_APIENTRY glReleaseShaderCompiler(void)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
   }
}

GL_APICALL void GL_APIENTRY glShaderBinary (GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   UNUSED(n);
   UNUSED(shaders);
   UNUSED(binaryformat);
   UNUSED(binary);
   UNUSED(length);

   if (IS_OPENGLES_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      set_error(state, GL_INVALID_ENUM);
   }
}

GL_APICALL void GL_APIENTRY glShaderSource(GLuint shader, GLsizei count, const char **string, const GLint *length)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      RPC_CALL4(glShaderSource_impl_20, thread, no_id, shader,
                             count,
                             string,
                             length);
   }
}

/* OES_framebuffer_object */

GLboolean glxx_client_IsRenderbuffer(GLuint renderbuffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsRenderbuffer_impl,
                                           thread, 
                                           GLISRENDERBUFFER_ID,
                                           RPC_UINT(renderbuffer)));
   }

   return 0;
}


GL_APICALL GLboolean GL_APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
   return glxx_client_IsRenderbuffer(renderbuffer);
}

void glxx_client_BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL2(glBindRenderbuffer_impl,
                thread, 
                GLBINDRENDERBUFFER_ID,
                RPC_ENUM(target),
                RPC_UINT(renderbuffer));
   }
}

GL_APICALL void GL_APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
   glxx_client_BindRenderbuffer(target, renderbuffer);
}


void glxx_client_DeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;

   do {
      int32_t items = (int32_t) (KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
      int32_t batch = _min(items, (int32_t)n);

      if (IS_OPENGLES_11_OR_20(thread)) {
         RPC_CALL2_IN_BULK(glDeleteRenderbuffers_impl,
                           thread, 
                           GLDELETERENDERBUFFERS_ID,
                           RPC_SIZEI(batch),
                           renderbuffers + offset,
                           batch > 0 ? batch * sizeof(GLuint) : 0);
      }

      offset += batch;
      n -= batch;
   } while (n > 0);
}

GL_APICALL void GL_APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
   glxx_client_DeleteRenderbuffers(n, renderbuffers);
}

void glxx_client_GenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;

   do {
      int32_t items = (int32_t)(KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
      int32_t batch = _min(items, (int32_t)n);

      if (IS_OPENGLES_11_OR_20(thread)) {
         RPC_CALL2_OUT_BULK(glGenRenderbuffers_impl,
                            thread, 
                            GLGENRENDERBUFFERS_ID,
                            RPC_SIZEI(batch),
                            renderbuffers + offset);
      }

      offset += batch;
      n -= batch;
   } while (n > 0);
}

GL_APICALL void GL_APIENTRY glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
   glxx_client_GenRenderbuffers(n, renderbuffers);
}

void glxx_client_RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glRenderbufferStorage_impl,
                thread, 
                GLRENDERBUFFERSTORAGE_ID,
                RPC_ENUM(target),
                RPC_ENUM(internalformat),
                RPC_SIZEI(width),
                RPC_SIZEI(height));
   }
}

GL_APICALL void GL_APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
   glxx_client_RenderbufferStorage(target, internalformat, width, height);
}

void glxx_client_GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL3_OUT_CTRL(glGetRenderbufferParameteriv_impl,
                         thread, 
                         GLGETRENDERBUFFERPARAMETERIV_ID,
                         RPC_ENUM(target),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
   glxx_client_GetRenderbufferParameteriv(target, pname, params);
}

GLboolean glxx_client_IsFramebuffer(GLuint framebuffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      return RPC_BOOLEAN_RES(RPC_CALL1_RES(glIsFramebuffer_impl,
                                           thread, 
                                           GLISFRAMEBUFFER_ID,
                                           RPC_UINT(framebuffer)));
   }

   return 0;
}

GL_APICALL GLboolean GL_APIENTRY glIsFramebuffer(GLuint framebuffer)
{
   return glxx_client_IsFramebuffer(framebuffer);
}

/*
   Spec deviation:
      eglMakeCurrent(gles2.0 context, pixmap surface)
      glBindFramebuffer(invalid framebuffer id)
      glDrawSomeStuff()
      glFinish()
      Pixmap will not have been updated, as client assumes that rendering is
      taking place outside of the default framebuffer
*/

void glxx_client_BindFramebuffer(GLenum target, GLuint framebuffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      GLXX_CLIENT_STATE_T *state = GLXX_GET_CLIENT_STATE(thread);
      RPC_CALL2(glBindFramebuffer_impl,
                thread, 
                GLBINDFRAMEBUFFER_ID,
                RPC_ENUM(target),
                RPC_UINT(framebuffer));

      //TODO: this may be set incorrectly if there's an error
      state->default_framebuffer = (framebuffer == 0);
   }
}

GL_APICALL void GL_APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
   glxx_client_BindFramebuffer(target, framebuffer);
}

void glxx_client_DeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;

   do {
      int32_t items = (int32_t)(KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
      int32_t batch = _min(items, (int32_t)n);

      if (IS_OPENGLES_11_OR_20(thread)) {
         RPC_CALL2_IN_BULK(glDeleteFramebuffers_impl,
                           thread, 
                           GLDELETEFRAMEBUFFERS_ID,
                           RPC_SIZEI(batch),
                           framebuffers + offset,
                           batch > 0 ? batch * sizeof(GLuint) : 0);
      }

      offset += batch;
      n -= batch;
   } while (n > 0);
}

GL_APICALL void GL_APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
   glxx_client_DeleteFramebuffers(n, framebuffers);
}

void glxx_client_GenFramebuffers(GLsizei n, GLuint *framebuffers)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   int offset = 0;

   do {
      int32_t items = (int32_t)(KHDISPATCH_WORKSPACE_SIZE / sizeof(GLuint));
      int32_t batch = _min(items, (int32_t)n);

      if (IS_OPENGLES_11_OR_20(thread)) {
         RPC_CALL2_OUT_BULK(glGenFramebuffers_impl,
                            thread, 
                            GLGENFRAMEBUFFERS_ID,
                            RPC_SIZEI(batch),
                            framebuffers + offset);
      }

      offset += batch;
      n -= batch;
   } while (n > 0);
}

GL_APICALL void GL_APIENTRY glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
   glxx_client_GenFramebuffers(n, framebuffers);
}

GLenum glxx_client_CheckFramebufferStatus(GLenum target)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      return RPC_ENUM_RES(RPC_CALL1_RES(glCheckFramebufferStatus_impl,
                           thread, 
                           GLCHECKFRAMEBUFFERSTATUS_ID,
                           RPC_ENUM(target)));
   }

   return GL_NONE;
}

GL_APICALL GLenum GL_APIENTRY glCheckFramebufferStatus(GLenum target)
{
   return glxx_client_CheckFramebufferStatus(target);
}

void glxx_client_FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL5(glFramebufferTexture2D_impl,
                thread, 
                GLFRAMEBUFFERTEXTURE2D_ID,
                RPC_ENUM(target),
                RPC_ENUM(attachment),
                RPC_ENUM(textarget),
                RPC_UINT(texture),
                RPC_INT(level));
   }
}

GL_APICALL void GL_APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
   glxx_client_FramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glxx_client_FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4(glFramebufferRenderbuffer_impl,
                thread, 
                GLFRAMEBUFFERRENDERBUFFER_ID,
                RPC_ENUM(target),
                RPC_ENUM(attachment),
                RPC_ENUM(renderbuffertarget),
                RPC_UINT(renderbuffer));
   }
}

GL_APICALL void GL_APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
   glxx_client_FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void glxx_client_GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL4_OUT_CTRL(glGetFramebufferAttachmentParameteriv_impl,
                         thread, 
                         GLGETFRAMEBUFFERATTACHMENTPARAMETERIV_ID,
                         RPC_ENUM(target),
                         RPC_ENUM(attachment),
                         RPC_ENUM(pname),
                         params);
   }
}

GL_APICALL void GL_APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
   glxx_client_GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

void glxx_client_GenerateMipmap(GLenum target)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) {
      RPC_CALL1(glGenerateMipmap_impl,
                thread, 
                GLGENERATEMIPMAP_ID,
                RPC_ENUM(target));
   }
}

GL_APICALL void GL_APIENTRY glGenerateMipmap(GLenum target)
{
   glxx_client_GenerateMipmap(target);
}

/* OES_shader_source + OES_shader_binary */
GL_APICALL void GL_APIENTRY glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_20(thread)) {
      GLint result[3];

      RPC_CALL3_OUT_CTRL(glGetShaderPrecisionFormat_impl_20,
                         thread, 
                         GLGETSHADERPRECISIONFORMAT_ID_20,
                         RPC_ENUM(shadertype),
                         RPC_ENUM(precisiontype),
                         result);

      if (range) {
         range[0] = result[0];
         range[1] = result[1];
      }
      if (precision)
         *precision = result[2];
   }
}

GL_APICALL void GL_APIENTRY glDiscardFramebufferEXT(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   if (IS_OPENGLES_11_OR_20(thread)) 
   {
      RPC_CALL3_IN_CTRL(glDiscardFramebufferEXT_impl,
                thread, 
                GLDISCARDFRAMEBUFFEREXT_ID,
                RPC_ENUM(target),
                RPC_SIZEI(numAttachments),
                attachments,
                numAttachments * sizeof(GLenum));
   }
}

static void glxx_client_state_init(GLXX_CLIENT_STATE_T *state)
{
   int i;

   state->error = GL_NO_ERROR;

   state->alignment.pack = 4;
   state->alignment.unpack = 4;

   state->bound_buffer.array = 0;
   state->bound_buffer.element_array = 0;

   for (i = 0; i < GLXX_CONFIG_MAX_VERTEX_ATTRIBS; i++) {
      state->attrib[i].enabled = GL_FALSE;
      state->attrib[i].size = 4;
      state->attrib[i].type = GL_FLOAT;
      state->attrib[i].normalized = GL_FALSE;
      state->attrib[i].stride = 0;
      state->attrib[i].pointer = NULL;
      state->attrib[i].buffer = 0;
      state->attrib[i].value[0] = 0.0f;
      state->attrib[i].value[1] = 0.0f;
      state->attrib[i].value[2] = 0.0f;
      state->attrib[i].value[3] = 1.0f;
   }

   state->render_callback = NULL;
   state->flush_callback = NULL;
}

int gl11_client_state_init(GLXX_CLIENT_STATE_T *state)
{
   int i;
   int indx;

   state->type = OPENGL_ES_11;

   //perform common initialisation
   glxx_client_state_init(state);
   //gl2.0 specific

   state->active_texture.client = GL_TEXTURE0;
   state->active_texture.server = GL_TEXTURE0;

   //color
   state->attrib[GL11_IX_COLOR].size = 4;
   state->attrib[GL11_IX_COLOR].normalized = GL_TRUE;
   state->attrib[GL11_IX_COLOR].value[0] = 1.0f;
   state->attrib[GL11_IX_COLOR].value[1] = 1.0f;
   state->attrib[GL11_IX_COLOR].value[2] = 1.0f;
   state->attrib[GL11_IX_COLOR].value[3] = 1.0f;

   //normal
   state->attrib[GL11_IX_NORMAL].size = 3;
   state->attrib[GL11_IX_NORMAL].normalized = GL_TRUE;
   state->attrib[GL11_IX_NORMAL].value[0] = 0.0f;
   state->attrib[GL11_IX_NORMAL].value[1] = 0.0f;
   state->attrib[GL11_IX_NORMAL].value[2] = 1.0f;

   //vertex
   state->attrib[GL11_IX_VERTEX].size = 4;
   state->attrib[GL11_IX_VERTEX].normalized = GL_FALSE;
   state->attrib[GL11_IX_VERTEX].value[0] = 0.0f;
   state->attrib[GL11_IX_VERTEX].value[1] = 0.0f;
   state->attrib[GL11_IX_VERTEX].value[2] = 0.0f;
   state->attrib[GL11_IX_VERTEX].value[3] = 1.0f;

   for (i = 0; i < GL11_CONFIG_MAX_TEXTURE_UNITS; i++) {
      indx = GL11_IX_TEXTURE_COORD + i;
      state->attrib[indx].size = 4;
      state->attrib[indx].normalized = GL_FALSE;
      state->attrib[indx].value[0] = 0.0f;
      state->attrib[indx].value[1] = 0.0f;
      state->attrib[indx].value[2] = 0.0f;
      state->attrib[indx].value[3] = 1.0f;
   }

   //point size
   state->attrib[GL11_IX_POINT_SIZE].size = 1;
   state->attrib[GL11_IX_POINT_SIZE].normalized = GL_FALSE;
   state->attrib[GL11_IX_POINT_SIZE].value[0] = 1.0f;

   return khrn_cache_init(&state->cache);
}

int gl20_client_state_init(GLXX_CLIENT_STATE_T *state)
{
   state->type = OPENGL_ES_20;

   //perform common initialisation
   glxx_client_state_init(state);
   //gl2.0 specific

   state->default_framebuffer = true;

   return khrn_cache_init(&state->cache);
}

void glxx_client_state_free(GLXX_CLIENT_STATE_T *state)
{
   khrn_cache_term(&state->cache);
   khrn_platform_free(state);
}




//TODO we need these to get the conformance test to build
#ifdef __cplusplus
extern "C" {
#endif

GL_API void GL_APIENTRY glCurrentPaletteMatrixOES (GLuint matrixpaletteindex) { UNUSED(matrixpaletteindex); }
GL_API void GL_APIENTRY glLoadPaletteFromModelViewMatrixOES (void) {}
GL_API void GL_APIENTRY glMatrixIndexPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { UNUSED(size); UNUSED(type); UNUSED(stride); UNUSED(pointer); }
GL_API void GL_APIENTRY glWeightPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { UNUSED(size); UNUSED(type); UNUSED(stride); UNUSED(pointer); }
GL_API void GL_APIENTRY glTexImage3DOES (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels) { UNUSED(target); UNUSED(level); UNUSED(internalformat); UNUSED(width); UNUSED(height); UNUSED(depth); UNUSED(border); UNUSED(format); UNUSED(type); UNUSED(pixels); }
GL_API void GL_APIENTRY glTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels) { UNUSED(target); UNUSED(level); UNUSED(xoffset); UNUSED(yoffset); UNUSED(zoffset); UNUSED(width); UNUSED(height); UNUSED(depth); UNUSED(format); UNUSED(type); UNUSED(pixels); }
GL_API void* GL_APIENTRY glMapBufferOES (GLenum target, GLenum access) {UNUSED(target); UNUSED(access); return NULL; };
GL_API GLboolean GL_APIENTRY glUnmapBufferOES (GLenum target) {UNUSED(target); return GL_FALSE; };

#ifdef __cplusplus
}
#endif
