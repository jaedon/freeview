/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLCONTEXTEGL_H
#define OPENGLCONTEXTEGL_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace netflix {
namespace gibbon {

class OpenGLContextPrivate
{
public:
    enum {
        WindowContext,
        PbContext
    } type;

    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    GLuint videoTexture;
    EGLImageKHR videoImage;
};

}}

#endif
