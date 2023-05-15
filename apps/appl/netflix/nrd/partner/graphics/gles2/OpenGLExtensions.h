/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLEXTENSIONS_H
#define OPENGLEXTENSIONS_H

#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
#ifndef APIENTRYP
#define APIENTRYP EGLAPIENTRYP
#define GLuint64 uint64_t
#define GLint64 int64_t
#endif
typedef void* EGLSyncKHR;
typedef EGLSyncKHR (EGLAPIENTRYP PFNEGLCREATESYNCKHRPROC) (EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLDESTROYSYNCKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync);
typedef EGLint (EGLAPIENTRYP PFNEGLCLIENTWAITSYNCKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
#endif
#ifndef APIENTRYP
#define APIENTRYP *
#endif
typedef struct __GLsync *GLsync;
typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC) (GLenum condition, GLbitfield flags);
typedef GLboolean (APIENTRYP PFNGLISSYNCPROC) (GLsync sync);
typedef void (APIENTRYP PFNGLDELETESYNCPROC) (GLsync sync);
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLGETSYNCIVPROC) (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);

#ifndef GL_ARB_sync
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#endif

#ifndef EGL_SYNC_FLUSH_COMMANDS_BIT_KHR
#define EGL_SYNC_FENCE_KHR                   0x30F9
#define EGL_SYNC_FLUSH_COMMANDS_BIT_KHR      0x0001
#define EGL_SYNC_TYPE_KHR                    0x30F7
#define EGL_SYNC_STATUS_KHR                  0x30F1
#define EGL_SYNC_CONDITION_KHR               0x30F8
#define EGL_SIGNALED_KHR                     0x30F2
#define EGL_UNSIGNALED_KHR                   0x30F3
#define EGL_SYNC_PRIOR_COMMANDS_COMPLETE_KHR 0x30F0
#define EGL_FOREVER_KHR                      0xFFFFFFFFFFFFFFFFull
#define EGL_TIMEOUT_EXPIRED_KHR              0x30F5
#define EGL_CONDITION_SATISFIED_KHR          0x30F6
#define EGL_NO_SYNC_KHR                      ((void*)0)
#endif

#endif
