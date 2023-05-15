/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "OpenGLContext.h"
#include "OpenGLState.h"
#include "OpenGLContextEGL.h"
#include <Screen.h>
#include <gibbon/config.h>
#include <nrd/AppLog.h>
#include <string.h>

using namespace netflix;
using namespace netflix::gibbon;

#ifndef GIBBON_EGL_NO_SWAP
static inline void gibbonEGLSwap(EGLDisplay display, EGLSurface surface)
{
    eglSwapBuffers(display, surface);
}
#else
extern void gibbonEGLSwap(EGLDisplay display, EGLSurface surface);
#endif

bool OpenGLContext::init(OpenGLContext* share)
{
    EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    mPrivate = new OpenGLContextPrivate;
    memset(mPrivate, '\0', sizeof(OpenGLContextPrivate));
    if (!share) {
        mPrivate->type = OpenGLContextPrivate::WindowContext;

        EGLint numConfigs = 0;
        EGLint majorVersion;
        EGLint minorVersion;

        mPrivate->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        assert(mPrivate->display != EGL_NO_DISPLAY);

        EGLint result;
        NRDP_UNUSED(result);

        result = eglInitialize(mPrivate->display, &majorVersion, &minorVersion);
        assert(result != EGL_FALSE);

        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL INITIALIZED " << majorVersion << "." << minorVersion;

        result = eglGetConfigs(mPrivate->display, 0, 0, &numConfigs);
        assert(result != EGL_FALSE);
        assert(numConfigs > 0);

        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL CONFIGS ARE " << numConfigs;

        const int targetSamples = Screen::getConfiguration().targetSamples;
        int trySamples = targetSamples;

        const EGLint configAttributes[] = {
            EGL_RED_SIZE,        1,
            EGL_GREEN_SIZE,      1,
            EGL_BLUE_SIZE,       1,
            EGL_ALPHA_SIZE,      1,
            EGL_DEPTH_SIZE,      0,
            EGL_STENCIL_SIZE,    0,
            EGL_BUFFER_SIZE,     EGL_DONT_CARE,

            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,

            EGL_SAMPLE_BUFFERS,  trySamples ? 1 : EGL_DONT_CARE,
            EGL_SAMPLES,         trySamples ? trySamples : EGL_DONT_CARE,

            EGL_NONE
        };

        for (;;) {
            int chooseConfigs = numConfigs;
            result = eglChooseConfig(mPrivate->display, configAttributes, &mPrivate->config, 1, &chooseConfigs);
            if (result == EGL_TRUE && chooseConfigs) {
                numConfigs = chooseConfigs;
                break;
            }

            if (!trySamples)
                break;
            trySamples /= 2;
        }

        assert(result != EGL_FALSE);
        assert(numConfigs > 0);

        EGLint bufferSize, redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize;

        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_BUFFER_SIZE, &bufferSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_RED_SIZE, &redSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_GREEN_SIZE, &greenSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_BLUE_SIZE, &blueSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_ALPHA_SIZE, &alphaSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_DEPTH_SIZE, &depthSize);
        eglGetConfigAttrib(mPrivate->display, mPrivate->config, EGL_STENCIL_SIZE, &stencilSize);

        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL CONFIG HAS"
                                         << " U=" << bufferSize
                                         << " R=" << redSize
                                         << " G=" << greenSize
                                         << " B=" << blueSize
                                         << " A=" << alphaSize
                                         << " D=" << depthSize
                                         << " S=" << stencilSize;

        mPrivate->surface = eglCreateWindowSurface(mPrivate->display, mPrivate->config, 0, 0);

        assert(EGL_NO_SURFACE != mPrivate->surface);

        int width;
        int height;

        eglQuerySurface(mPrivate->display, mPrivate->surface, EGL_WIDTH, &width);
        eglQuerySurface(mPrivate->display, mPrivate->surface, EGL_HEIGHT, &height);

        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL SURFACE IS " << width << "x" << height;

        mWidth = width;
        mHeight = height;

        mPrivate->context = eglCreateContext(mPrivate->display, mPrivate->config, EGL_NO_CONTEXT, contextAttributes);
        assert(EGL_NO_CONTEXT != mPrivate->context);

        result = eglMakeCurrent(mPrivate->display, mPrivate->surface, mPrivate->surface, mPrivate->context);
        assert(EGL_FALSE != result);

        result = eglSwapInterval(mPrivate->display, 1);
        assert(EGL_FALSE != result);

        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL VENDOR     : " << eglQueryString(mPrivate->display, EGL_VENDOR);
        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL VERSION    : " << eglQueryString(mPrivate->display, EGL_VERSION);
        std::string ext(reinterpret_cast< const char * >(eglQueryString(mPrivate->display, EGL_EXTENSIONS)));
        std::replace(ext.begin(), ext.end(), ' ', '\n' );
        Log::info(TRACE_GIBBON_GRAPHICS) << "EGL EXTENSIONS : " << "\n" << ext;
    } else {
        mPrivate->type = OpenGLContextPrivate::PbContext;
        mPrivate->display = share->mPrivate->display;
        mPrivate->config = share->mPrivate->config;
        mWidth = share->mWidth;
        mHeight = share->mHeight;

        static EGLint pbAttribs[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE
        };

        EGLint result;
        NRDP_UNUSED(result);

        mPrivate->surface = eglCreatePbufferSurface(mPrivate->display, mPrivate->config, pbAttribs);
        assert(EGL_NO_SURFACE != mPrivate->surface);
        mPrivate->context = eglCreateContext(mPrivate->display, mPrivate->config, share->mPrivate->context, contextAttributes);
        assert(EGL_NO_CONTEXT != mPrivate->context);

        result = eglMakeCurrent(mPrivate->display, mPrivate->surface, mPrivate->surface, mPrivate->context);
        assert(EGL_FALSE != result);

        result = eglSwapInterval(mPrivate->display, 1);
        assert(EGL_FALSE != result);
}

    return true;
}

void OpenGLContext::cleanup()
{
    eglMakeCurrent(mPrivate->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if(mPrivate->videoImage) {
        eglDestroyImageKHR(mPrivate->display, mPrivate->videoImage);
        glDeleteTextures(1 , &mPrivate->videoTexture );
        mPrivate->videoImage = 0;
    }
    eglDestroySurface(mPrivate->display, mPrivate->surface);
    eglDestroyContext(mPrivate->display, mPrivate->context);
    if (mPrivate->type == OpenGLContextPrivate::WindowContext)
        eglTerminate(mPrivate->display);

    delete mPrivate;
    mPrivate = 0;
}

void OpenGLContext::flip()
{
    gibbonEGLSwap(mPrivate->display, mPrivate->surface);
}

void* OpenGLContext::display()
{
    return mPrivate->display;
}

void *OpenGLContext::createEGLVideoImage(unsigned width, unsigned height)
{
    GL_CHECK;
    OpenGLContext::ensureContext();
    if(mPrivate->videoImage) {
        eglDestroyImageKHR(mPrivate->display, mPrivate->videoImage);
        glDeleteTextures(1, &mPrivate->videoTexture);
    }
    glGenTextures(1, &mPrivate->videoTexture);
    glBindTexture(GL_TEXTURE_2D, mPrivate->videoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK;
    mPrivate->videoImage = eglCreateImageKHR(mPrivate->display, mPrivate->context, EGL_GL_TEXTURE_2D_KHR, (EGLClientBuffer)mPrivate->videoTexture, 0);
    GL_CHECK;
    glFinish();
    return mPrivate->videoImage;
}

bool OpenGLContext::renderVideo()
{
    if(int texture = mPrivate->videoTexture) {
        GraphicsEngine* graphics = engine();
        Rect clip;
        const bool clipped = graphics->getState().hasClip;
        if(clipped) {
            clip = graphics->getState().clip;
            mState->disableCap(GL_SCISSOR_TEST);
            graphics->clearClip();
        }

        Transform transform;
        transform.scale(1, -1, 1);
        static GLfloat texTransform[4] = { 0, 0, 1, 1 };
        graphics->blit(texture, transform, texTransform);

        if(clipped) {
            mState->enableCap(GL_SCISSOR_TEST);
            graphics->setClip(clip);
        }
        return true;
    }
    return false;
}

void* OpenGLContext::procAddr(const std::string& proc)
{
    return reinterpret_cast<void*>(eglGetProcAddress(proc.c_str()));
}

static pthread_once_t sPlatformExtensionOnce = PTHREAD_ONCE_INIT;
static std::set<std::string> sPlatformExtensions;

static void initPlatformExtensions()
{
    EGLDisplay dpy = static_cast<EGLDisplay>(OpenGLContext::context()->display());
    const std::string all = reinterpret_cast<const char*>(eglQueryString(dpy, EGL_EXTENSIONS));
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
    return (std::find(sPlatformExtensions.begin(), sPlatformExtensions.end(), ext) != sPlatformExtensions.end());
}
