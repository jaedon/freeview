/*=============================================================================
Copyright (c) 2011 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  External interface

FILE DESCRIPTION
API re-entrancy protection (for multi-threading) and logging over 
a remote interface.
=============================================================================*/

#ifndef KHRN_API_INTERPOSER_H
#define KHRN_API_INTERPOSER_H

#include "interface/khronos/include/GLES/gl.h"
#include "interface/khronos/include/GLES2/gl2.h"
#include "interface/khronos/include/GLES/glext.h"
#include "interface/khronos/include/GLES2/gl2ext.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"
#include "interface/khronos/include/VG/openvg.h"
#include "interface/khronos/include/VG/vgext.h"
#include "interface/khronos/include/VG/vgu.h"

/* Type of event */
typedef enum {
   eEVENT_INVALID_TYPE = 0,
   eEVENT_WAITING,
   eEVENT_START,
   eEVENT_END,
   eEVENT_ONE_SHOT
} eEventType;

/* Recognised event codes */
typedef enum {
   eEVENT_INVALID_CODE = 0,
   eEVENT_DRIVER,
   eEVENT_API,
   eEVENT_BIN,
   eEVENT_RENDER,
   eEVENT_TFCONVERT,
   eEVENT_EGLIMAGE_APP_LOCK,
   eEVENT_MISC
} eEventCode;

/* Indicates which row of the event viewer to use - nominal hardware unit */
typedef enum {
   eEVENT_BINNER     = 0,
   eEVENT_RENDERER,
   eEVENT_CPU,
   eEVENT_CPU2,
   eEVENT_CPU3,
   eEVENT_CPU4,
   eEVENT_LAST_ROW
} eEventRow;

typedef struct
{
   uint32_t eventType;
   uint32_t eventCode;
   uint32_t eventRow;
   uint32_t eventData;
   char     *desc;
} EventData;

extern eEventRow khrn_event_cpu_row();

typedef struct { 
   GLboolean (*real_glIsBuffer)(GLuint buffer);
   GLboolean (*real_glIsEnabled)(GLenum cap);
   GLboolean (*real_glIsFramebuffer)(GLuint framebuffer);
   GLboolean (*real_glIsProgram)(GLuint program);
   GLboolean (*real_glIsRenderbuffer)(GLuint renderbuffer);
   GLboolean (*real_glIsShader)(GLuint shader);
   GLboolean (*real_glIsTexture)(GLuint texture);
   GLenum (*real_glCheckFramebufferStatus)(GLenum target);
   GLenum (*real_glGetError)(void);
   GLuint (*real_glCreateProgram)(void);
   GLuint (*real_glCreateShader)(GLenum type);
   const GLubyte* (*real_glGetString)(GLenum name);
   int (*real_glGetAttribLocation)(GLuint program, const GLchar* name);
   int (*real_glGetUniformLocation)(GLuint program, const GLchar* name);
   void (*real_glActiveTexture)(GLenum texture);
   void (*real_glAlphaFunc)(GLenum func, GLclampf ref);
   void (*real_glAlphaFuncx)(GLenum func, GLclampx ref);
   void (*real_glAttachShader)(GLuint program, GLuint shader);
   void (*real_glBindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
   void (*real_glBindBuffer)(GLenum target, GLuint buffer);
   void (*real_glBindFramebuffer)(GLenum target, GLuint framebuffer);
   void (*real_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
   void (*real_glBindTexture)(GLenum target, GLuint texture);
   void (*real_glBlendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
   void (*real_glBlendEquation)( GLenum mode );
   void (*real_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
   void (*real_glBlendFunc)(GLenum sfactor, GLenum dfactor);
   void (*real_glBlendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
   void (*real_glBufferData)(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
   void (*real_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
   void (*real_glClear)(GLbitfield mask);
   void (*real_glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
   void (*real_glClearColorx)(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
   void (*real_glClearDepthf)(GLclampf depth);
   void (*real_glClearDepthx)(GLclampx depth);
   void (*real_glClearStencil)(GLint s);
   void (*real_glClientActiveTexture)(GLenum texture);
   void (*real_glClipPlanef)(GLenum plane, const GLfloat *equation);
   void (*real_glClipPlanex)(GLenum plane, const GLfixed *equation);
   void (*real_glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
   void (*real_glColor4ub)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
   void (*real_glColor4x)(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
   void (*real_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
   void (*real_glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glCompileShader)(GLuint shader);
   void (*real_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
   void (*real_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
   void (*real_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
   void (*real_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
   void (*real_glCullFace)(GLenum mode);
   void (*real_glDeleteBuffers)(GLsizei n, const GLuint *buffers);
   void (*real_glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
   void (*real_glDeleteProgram)(GLuint program);
   void (*real_glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
   void (*real_glDeleteShader)(GLuint shader);
   void (*real_glDeleteTextures)(GLsizei n, const GLuint* textures);
   void (*real_glDepthFunc)(GLenum func);
   void (*real_glDepthMask)(GLboolean flag);
   void (*real_glDepthRangef)(GLclampf zNear, GLclampf zFar);
   void (*real_glDepthRangex)(GLclampx zNear, GLclampx zFar);
   void (*real_glDetachShader)(GLuint program, GLuint shader);
   void (*real_glDisable)(GLenum cap);
   void (*real_glDisableClientState)(GLenum array);
   void (*real_glDisableVertexAttribArray)(GLuint index);
   void (*real_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
   void (*real_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
   void (*real_glEnable)(GLenum cap);
   void (*real_glEnableClientState)(GLenum array);
   void (*real_glEnableVertexAttribArray)(GLuint index);
   void (*real_glFinish)(void);
   void (*real_glFlush)(void);
   void (*real_glFogf)(GLenum pname, GLfloat param);
   void (*real_glFogfv)(GLenum pname, const GLfloat *params);
   void (*real_glFogx)(GLenum pname, GLfixed param);
   void (*real_glFogxv)(GLenum pname, const GLfixed *params);
   void (*real_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
   void (*real_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
   void (*real_glFrontFace)(GLenum mode);
   void (*real_glFrustumf)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
   void (*real_glFrustumx)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
   void (*real_glGenBuffers)(GLsizei n, GLuint* buffers);
   void (*real_glGenFramebuffers)(GLsizei n, GLuint* framebuffers);
   void (*real_glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
   void (*real_glGenTextures)(GLsizei n, GLuint *textures);
   void (*real_glGenerateMipmap)(GLenum target);
   void (*real_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
   void (*real_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
   void (*real_glGetAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
   void (*real_glGetBooleanv)(GLenum pname, GLboolean* params);
   void (*real_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
   void (*real_glGetClipPlanef)(GLenum pname, GLfloat eqn[4]);
   void (*real_glGetClipPlanex)(GLenum pname, GLfixed eqn[4]);
   void (*real_glGetFixedv)(GLenum pname, GLfixed *params);
   void (*real_glGetFloatv)(GLenum pname, GLfloat *params);
   void (*real_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
   void (*real_glGetIntegerv)(GLenum pname, GLint *params);
   void (*real_glGetLightfv)(GLenum light, GLenum pname, GLfloat *params);
   void (*real_glGetLightxv)(GLenum light, GLenum pname, GLfixed *params);
   void (*real_glGetMaterialfv)(GLenum face, GLenum pname, GLfloat *params);
   void (*real_glGetMaterialxv)(GLenum face, GLenum pname, GLfixed *params);
   void (*real_glGetPointerv)(GLenum pname, GLvoid **params);
   void (*real_glGetProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
   void (*real_glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
   void (*real_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
   void (*real_glGetShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
   void (*real_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
   void (*real_glGetShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
   void (*real_glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
   void (*real_glGetTexEnvfv)(GLenum env, GLenum pname, GLfloat *params);
   void (*real_glGetTexEnviv)(GLenum env, GLenum pname, GLint *params);
   void (*real_glGetTexEnvxv)(GLenum env, GLenum pname, GLfixed *params);
   void (*real_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
   void (*real_glGetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
   void (*real_glGetTexParameterxv)(GLenum target, GLenum pname, GLfixed *params);
   void (*real_glGetUniformfv)(GLuint program, GLint location, GLfloat* params);
   void (*real_glGetUniformiv)(GLuint program, GLint location, GLint* params);
   void (*real_glGetVertexAttribPointerv)(GLuint index, GLenum pname, GLvoid** pointer);
   void (*real_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
   void (*real_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
   void (*real_glHint)(GLenum target, GLenum mode);
   void (*real_glLightModelf)(GLenum pname, GLfloat param);
   void (*real_glLightModelfv)(GLenum pname, const GLfloat *params);
   void (*real_glLightModelx)(GLenum pname, GLfixed param);
   void (*real_glLightModelxv)(GLenum pname, const GLfixed *params);
   void (*real_glLightf)(GLenum light, GLenum pname, GLfloat param);
   void (*real_glLightfv)(GLenum light, GLenum pname, const GLfloat *params);
   void (*real_glLightx)(GLenum light, GLenum pname, GLfixed param);
   void (*real_glLightxv)(GLenum light, GLenum pname, const GLfixed *params);
   void (*real_glLineWidth)(GLfloat width);
   void (*real_glLineWidthx)(GLfixed width);
   void (*real_glLinkProgram)(GLuint program);
   void (*real_glLoadIdentity)(void);
   void (*real_glLoadMatrixf)(const GLfloat *m);
   void (*real_glLoadMatrixx)(const GLfixed *m);
   void (*real_glLogicOp)(GLenum opcode);
   void (*real_glMaterialf)(GLenum face, GLenum pname, GLfloat param);
   void (*real_glMaterialfv)(GLenum face, GLenum pname, const GLfloat *params);
   void (*real_glMaterialx)(GLenum face, GLenum pname, GLfixed param);
   void (*real_glMaterialxv)(GLenum face, GLenum pname, const GLfixed *params);
   void (*real_glMatrixMode)(GLenum mode);
   void (*real_glMultMatrixf)(const GLfloat *m);
   void (*real_glMultMatrixx)(const GLfixed *m);
   void (*real_glMultiTexCoord4f)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
   void (*real_glMultiTexCoord4x)(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
   void (*real_glNormal3f)(GLfloat nx, GLfloat ny, GLfloat nz);
   void (*real_glNormal3x)(GLfixed nx, GLfixed ny, GLfixed nz);
   void (*real_glNormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glOrthof)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
   void (*real_glOrthox)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
   void (*real_glPixelStorei)(GLenum pname, GLint param);
   void (*real_glPointParameterf)(GLenum pname, GLfloat param);
   void (*real_glPointParameterfv)(GLenum pname, const GLfloat *params);
   void (*real_glPointParameterx)(GLenum pname, GLfixed param);
   void (*real_glPointParameterxv)(GLenum pname, const GLfixed *params);
   void (*real_glPointSize)(GLfloat size);
   void (*real_glPointSizex)(GLfixed size);
   void (*real_glPolygonOffset)(GLfloat factor, GLfloat units);
   void (*real_glPolygonOffsetx)(GLfixed factor, GLfixed units);
   void (*real_glPopMatrix)(void);
   void (*real_glPushMatrix)(void);
   void (*real_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
   void (*real_glReleaseShaderCompiler)(void);
   void (*real_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
   void (*real_glRotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
   void (*real_glRotatex)(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
   void (*real_glSampleCoverage)(GLclampf value, GLboolean invert);
   void (*real_glSampleCoveragex)(GLclampx value, GLboolean invert);
   void (*real_glScalef)(GLfloat x, GLfloat y, GLfloat z);
   void (*real_glScalex)(GLfixed x, GLfixed y, GLfixed z);
   void (*real_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
   void (*real_glShadeModel)(GLenum mode);
   void (*real_glShaderBinary)(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
   void (*real_glShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
   void (*real_glStencilFunc)(GLenum func, GLint ref, GLuint mask);
   void (*real_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
   void (*real_glStencilMask)(GLuint mask);
   void (*real_glStencilMaskSeparate)(GLenum face, GLuint mask);
   void (*real_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
   void (*real_glStencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
   void (*real_glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glTexEnvf)(GLenum target, GLenum pname, GLfloat param);
   void (*real_glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
   void (*real_glTexEnvi)(GLenum target, GLenum pname, GLint param);
   void (*real_glTexEnviv)(GLenum target, GLenum pname, const GLint *params);
   void (*real_glTexEnvx)(GLenum target, GLenum pname, GLfixed param);
   void (*real_glTexEnvxv)(GLenum target, GLenum pname, const GLfixed *params);
   void (*real_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
   void (*real_glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
   void (*real_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
   void (*real_glTexParameteri)(GLenum target, GLenum pname, GLint param);
   void (*real_glTexParameteriv)(GLenum target, GLenum pname, const GLint *params);
   void (*real_glTexParameterx)(GLenum target, GLenum pname, GLfixed param);
   void (*real_glTexParameterxv)(GLenum target, GLenum pname, const GLfixed *params);
   void (*real_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
   void (*real_glTranslatef)(GLfloat x, GLfloat y, GLfloat z);
   void (*real_glTranslatex)(GLfixed x, GLfixed y, GLfixed z);
   void (*real_glUniform1f)(GLint location, GLfloat x);
   void (*real_glUniform1fv)(GLint location, GLsizei count, const GLfloat* v);
   void (*real_glUniform1i)(GLint location, GLint x);
   void (*real_glUniform1iv)(GLint location, GLsizei count, const GLint* v);
   void (*real_glUniform2f)(GLint location, GLfloat x, GLfloat y);
   void (*real_glUniform2fv)(GLint location, GLsizei count, const GLfloat* v);
   void (*real_glUniform2i)(GLint location, GLint x, GLint y);
   void (*real_glUniform2iv)(GLint location, GLsizei count, const GLint* v);
   void (*real_glUniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);
   void (*real_glUniform3fv)(GLint location, GLsizei count, const GLfloat* v);
   void (*real_glUniform3i)(GLint location, GLint x, GLint y, GLint z);
   void (*real_glUniform3iv)(GLint location, GLsizei count, const GLint* v);
   void (*real_glUniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
   void (*real_glUniform4fv)(GLint location, GLsizei count, const GLfloat* v);
   void (*real_glUniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);
   void (*real_glUniform4iv)(GLint location, GLsizei count, const GLint* v);
   void (*real_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
   void (*real_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
   void (*real_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
   void (*real_glUseProgram)(GLuint program);
   void (*real_glValidateProgram)(GLuint program);
   void (*real_glVertexAttrib1f)(GLuint indx, GLfloat x);
   void (*real_glVertexAttrib1fv)(GLuint indx, const GLfloat* values);
   void (*real_glVertexAttrib2f)(GLuint indx, GLfloat x, GLfloat y);
   void (*real_glVertexAttrib2fv)(GLuint indx, const GLfloat* values);
   void (*real_glVertexAttrib3f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
   void (*real_glVertexAttrib3fv)(GLuint indx, const GLfloat* values);
   void (*real_glVertexAttrib4f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
   void (*real_glVertexAttrib4fv)(GLuint indx, const GLfloat* values);
   void (*real_glVertexAttribPointer)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
   void (*real_glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
   void (*real_glEGLImageTargetTexture2DOES)(GLenum target, GLeglImageOES image);
   void (*real_glEGLImageTargetRenderbufferStorageOES)(GLenum target, GLeglImageOES image);
   void (*real_glDrawTexsOES)(GLshort x, GLshort y, GLshort z, GLshort width, GLshort height);
   void (*real_glDrawTexiOES)(GLint x, GLint y, GLint z, GLint width, GLint height);
   void (*real_glDrawTexxOES)(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height);
   void (*real_glDrawTexsvOES)(const GLshort *coords);
   void (*real_glDrawTexivOES)(const GLint *coords);
   void (*real_glDrawTexxvOES)(const GLfixed *coords);
   void (*real_glDrawTexfOES)(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
   void (*real_glDrawTexfvOES)(const GLfloat *coords);
   GLboolean (*real_glIsRenderbufferOES)(GLuint renderbuffer);
   void (*real_glBindRenderbufferOES)(GLenum target, GLuint renderbuffer);
   void (*real_glDeleteRenderbuffersOES)(GLsizei n, const GLuint* renderbuffers);
   void (*real_glGenRenderbuffersOES)(GLsizei n, GLuint* renderbuffers);
   void (*real_glRenderbufferStorageOES)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
   void (*real_glGetRenderbufferParameterivOES)(GLenum target, GLenum pname, GLint* params);
   GLboolean (*real_glIsFramebufferOES)(GLuint framebuffer);
   void (*real_glBindFramebufferOES)(GLenum target, GLuint framebuffer);
   void (*real_glDeleteFramebuffersOES)(GLsizei n, const GLuint* framebuffers);
   void (*real_glGenFramebuffersOES)(GLsizei n, GLuint* framebuffers);
   GLenum (*real_glCheckFramebufferStatusOES)(GLenum target);
   void (*real_glFramebufferRenderbufferOES)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
   void (*real_glFramebufferTexture2DOES)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
   void (*real_glGetFramebufferAttachmentParameterivOES)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
   void (*real_glGenerateMipmapOES)(GLenum target);
   GLbitfield (*real_glQueryMatrixxOES)(GLfixed mantissa[16], GLint exponent[16]);
   void (*real_glPointSizePointerOES)(GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glDiscardFramebufferEXT)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
   
   void (*real_glInsertEventMarkerEXT)(GLsizei length, const GLchar *marker);
   void (*real_glPushGroupMarkerEXT)(GLsizei length, const GLchar *marker);
   void (*real_glPopGroupMarkerEXT)(void);

   void (*real_glCurrentPaletteMatrixOES)(GLuint matrixpaletteindex);
   void (*real_glLoadPaletteFromModelViewMatrixOES) (void);
   void (*real_glMatrixIndexPointerOES) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
   void (*real_glWeightPointerOES) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

   EGLint (*real_eglGetError)(void);
   EGLDisplay (*real_eglGetDisplay)(EGLNativeDisplayType display_id);
   EGLBoolean (*real_eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor);
   EGLBoolean (*real_eglTerminate)(EGLDisplay dpy);
   const char *(*real_eglQueryString)(EGLDisplay dpy, EGLint name);
   EGLBoolean (*real_eglGetConfigs)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);
   EGLBoolean (*real_eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
   EGLBoolean (*real_eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
   EGLSurface (*real_eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
   EGLSurface (*real_eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list);
   EGLSurface (*real_eglCreatePixmapSurface)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
   EGLBoolean (*real_eglDestroySurface)(EGLDisplay dpy, EGLSurface surface);
   EGLBoolean (*real_eglQuerySurface)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
   EGLBoolean (*real_eglBindAPI)(EGLenum api);
   EGLenum    (*real_eglQueryAPI)(void);
   EGLBoolean (*real_eglWaitClient)(void);
   EGLBoolean (*real_eglReleaseThread)(void);
   EGLSurface (*real_eglCreatePbufferFromClientBuffer)(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
   EGLBoolean (*real_eglSurfaceAttrib)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
   EGLBoolean (*real_eglBindTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
   EGLBoolean (*real_eglReleaseTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
   EGLBoolean (*real_eglSwapInterval)(EGLDisplay dpy, EGLint interval);
   EGLContext (*real_eglCreateContext)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
   EGLBoolean (*real_eglDestroyContext)(EGLDisplay dpy, EGLContext ctx);
   EGLBoolean (*real_eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
   EGLContext (*real_eglGetCurrentContext)(void);
   EGLSurface (*real_eglGetCurrentSurface)(EGLint readdraw);
   EGLDisplay (*real_eglGetCurrentDisplay)(void);
   EGLBoolean (*real_eglQueryContext)(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);
   EGLBoolean (*real_eglWaitGL)(void);
   EGLBoolean (*real_eglWaitNative)(EGLint engine);
   EGLBoolean (*real_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
   EGLBoolean (*real_eglCopyBuffers)(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
   EGLBoolean (*real_eglLockSurfaceKHR)(EGLDisplay display, EGLSurface surface, const EGLint *attrib_list);
   EGLBoolean (*real_eglUnlockSurfaceKHR)(EGLDisplay display, EGLSurface surface);
   EGLImageKHR(*real_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
   EGLBoolean (*real_eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image);
   EGLSyncKHR (*real_eglCreateSyncKHR)(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
   EGLBoolean (*real_eglDestroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
   EGLint     (*real_eglClientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
   EGLBoolean (*real_eglSignalSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
   EGLBoolean (*real_eglGetSyncAttribKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);
   EGLBoolean (*real_eglImageUpdateParameterivBRCM)(EGLDisplay dpy, EGLImageKHR image, EGLenum pname, const EGLint *params);
   EGLBoolean (*real_eglImageUpdateParameteriBRCM)(EGLDisplay dpy, EGLImageKHR image, EGLenum pname, EGLint param);

   __eglMustCastToProperFunctionPointerType (*real_eglGetProcAddress)(const char *procname);

} REAL_GL_API_TABLE;

extern void khrn_init_api_interposer(void);
extern void khrn_remote_event_log(EventData *data);

#endif
