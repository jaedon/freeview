/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "OpenGLContext.h"
#include "Screen.h"
#include <GibbonConfiguration.h>
#include <nrd/NrdConfiguration.h>
#include <nrd/AppLog.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include "OpenGLContextGLX.h"

#include <string.h>
#include <string>


using namespace netflix;
using namespace netflix::gibbon;

static Mutex sGLXDpyMutex(GIBBON_GL_PRIVATE_MUTEX, "GLX dpy mutex");

bool OpenGLContext::init(OpenGLContext* share)
{
    const bool useDirectContext = GibbonConfiguration::useDirectContext();

    ScopedMutex locker(sGLXDpyMutex);

    mPrivate = new OpenGLContextPrivate;
    memset(mPrivate, '\0', sizeof(OpenGLContextPrivate));
    if (!share) {
        mWidth = GibbonConfiguration::screenWidth();
        mHeight = GibbonConfiguration::screenHeight();

        // create an X display and everything
        XInitThreads();
        Display* dpy = mPrivate->dpy = XOpenDisplay(0);
        if (!dpy) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Couldn't create X display");
            return false;
        }
        mPrivate->type = OpenGLContextPrivate::WindowContext;

        static int visualAttribs[] = {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            None
        };

        int glxMajor, glxMinor;
        // FBConfigs were added in GLX version 1.3
        if (!glXQueryVersion(dpy, &glxMajor, &glxMinor) ||
            ((glxMajor == 1) && (glxMinor < 3)) || (glxMajor < 1)) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Need GLX 1.3 or higher");
            return false;
        }

        int fbCount;
        GLXFBConfig* fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visualAttribs, &fbCount);
        if (!fbc) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Couldn't get framebuffer config");
            return false;
        }

        const int targetSamples = Screen::getConfiguration().targetSamples;
        // Pick the FB config/visual with the most samples (up to and including targetSamples) per pixel
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
        for (int i = 0; i < fbCount; ++i) {
            XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, fbc[i]);
            if (vi) {
                int samp_buf, samples;
                glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLES, &samples);

                if (best_fbc < 0 || ((samp_buf && samples > best_num_samp) && (samples <= targetSamples))) {
                    best_fbc = i;
                    best_num_samp = samples;
                }
                if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
                    worst_fbc = i;
                    worst_num_samp = samples;
                }
            }
            XFree(vi);
        }

        NTRACE(TRACE_GIBBON_GRAPHICS, "Chose fbc %x with %d samples", best_fbc, best_num_samp);
        GLXFBConfig bestFbc = mPrivate->fbc = fbc[best_fbc];

        XFree(fbc);

        XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, bestFbc);

        mPrivate->screen = vi->screen;

        XSetWindowAttributes swa;
        swa.colormap = mPrivate->cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
        swa.background_pixmap = None;
        swa.border_pixel = 0;
        swa.event_mask = (StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask |
                          PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

        mPrivate->win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, mWidth, mHeight, 0, vi->depth,
                                      InputOutput, vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
        if (!mPrivate->win) {
            XFree(vi);
            Log::error(TRACE_GIBBON_GRAPHICS, "Unable to create X11 window");
            return false;
        }

        XFree(vi);

        {
            XStoreName(dpy, mPrivate->win, "Gibbon GLX");
            XClassHint* hint = XAllocClassHint();
            hint->res_name = strdup("netflix");
            hint->res_class = strdup("Netflix");
            XSetClassHint(dpy, mPrivate->win, hint);
            free(hint->res_name);
            free(hint->res_class);
            XFree(hint);
        }
        {
            XSizeHints hints;
            hints.flags = PSize | PMinSize | PMaxSize;
            hints.width = hints.max_width = hints.min_width = mWidth;
            hints.height = hints.max_height = hints.min_height = mHeight;
            XSetWMNormalHints(dpy, mPrivate->win, &hints);
        }

        XMapWindow(dpy, mPrivate->win);

        mPrivate->ctx = glXCreateNewContext(dpy, bestFbc, GLX_RGBA_TYPE, 0, useDirectContext ? True : False);
        glXMakeCurrent(dpy, mPrivate->win, mPrivate->ctx);

        mPrivate->dpyRef = new unsigned int(1);
    } else {
        mPrivate->type = OpenGLContextPrivate::PbContext;
        const GLXContext shareCtx = share->mPrivate->ctx;
        Display* dpy = mPrivate->dpy = share->mPrivate->dpy;
        mPrivate->fbc = share->mPrivate->fbc;
        mPrivate->dpyRef = share->mPrivate->dpyRef;
        mWidth = share->mWidth;
        mHeight = share->mHeight;
        ++(*mPrivate->dpyRef);
        static int pbAttribs[] = {
            GLX_PBUFFER_WIDTH, static_cast<int>(mWidth),
            GLX_PBUFFER_HEIGHT, static_cast<int>(mHeight),
            GLX_LARGEST_PBUFFER, False,
            GLX_PRESERVED_CONTENTS, False,
            None
        };
        mPrivate->pb = glXCreatePbuffer(dpy, mPrivate->fbc, pbAttribs);
        if (!mPrivate->pb) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Unable to create pixel buffer");
            return false;
        }
        mPrivate->ctx = glXCreateNewContext(dpy, mPrivate->fbc, GLX_RGBA_TYPE, shareCtx, useDirectContext ? True : False);
        glXMakeCurrent(dpy, mPrivate->pb, mPrivate->ctx);
    }
    return true;
}

void OpenGLContext::cleanup()
{
    ScopedMutex locker(sGLXDpyMutex);

    if (!mPrivate->dpy)
        return;
    if (mPrivate->ctx) {
        glXMakeCurrent(mPrivate->dpy, None, 0);
        glXDestroyContext(mPrivate->dpy, mPrivate->ctx);
        switch (mPrivate->type) {
        case OpenGLContextPrivate::WindowContext:
            XDestroyWindow(mPrivate->dpy, mPrivate->win);
            XFreeColormap(mPrivate->dpy, mPrivate->cmap);
            break;
        case OpenGLContextPrivate::PbContext:
            glXDestroyPbuffer(mPrivate->dpy, mPrivate->pb);
            break;
        }
    }

    if (!--(*mPrivate->dpyRef)) {
        XCloseDisplay(mPrivate->dpy);
        delete mPrivate->dpyRef;
    }

    delete mPrivate;
    mPrivate = 0;
}

void OpenGLContext::flip()
{
    // ### is this right?
    assert(mPrivate->type == OpenGLContextPrivate::WindowContext);
    glXSwapBuffers(mPrivate->dpy, mPrivate->win);
}

void* OpenGLContext::display()
{
    return mPrivate->dpy;
}

bool OpenGLContext::renderVideo()
{
    return false;
}

void *OpenGLContext::createEGLVideoImage(unsigned /*width*/, unsigned /*height*/)
{
    return 0;
}

void* OpenGLContext::procAddr(const std::string& proc)
{
    return reinterpret_cast<void*>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(proc.c_str())));
}

static pthread_once_t sPlatformExtensionOnce = PTHREAD_ONCE_INIT;
static std::set<std::string> sPlatformExtensions;

static void initPlatformExtensions()
{
    Display* dpy = static_cast<Display*>(OpenGLContext::context()->display());
    const std::string all = glXQueryExtensionsString(dpy, DefaultScreen(dpy));
    size_t prev = 0;
    for (size_t pos = all.find(' '); pos != std::string::npos; pos = all.find(' ', pos + 1)) {
        sPlatformExtensions.insert(all.substr(prev, pos - prev));
        prev = pos + 1;
    }
    if (prev < all.size()) {
        sPlatformExtensions.insert(all.substr(prev));
    }
}

bool OpenGLContext::supportsPlatformExtension(const std::string& ext)
{
    pthread_once(&sPlatformExtensionOnce, initPlatformExtensions);
    return (sPlatformExtensions.find(ext) != sPlatformExtensions.end());
}
