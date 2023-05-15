/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLCONTEXTGLX_H
#define OPENGLCONTEXTGLX_H

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

namespace netflix {
namespace gibbon {

class OpenGLContextPrivate
{
public:
    Display* dpy;
    Colormap cmap;
    unsigned int* dpyRef;
    int screen;

    enum {
        WindowContext,
        PbContext
    } type;

    union {
        Window win;
        GLXPbuffer pb;
    };

    GLXContext ctx;
    GLXFBConfig fbc;
};

}}

#endif
