/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Surface.h>
#include <Animation.h>
#include <Screen.h>
#include <GibbonApplication.h>
#include <OpenGLContext.h>
#include <OpenGLState.h>
#include <gibbon/config.h>
#include <assert.h>

using namespace netflix;
using namespace netflix::gibbon;

namespace netflix {
namespace gibbon {
class GLSyncEvent : public GibbonApplication::Event
{
public:
    GLSyncEvent(const Surface::SharedPtr& surface) : mSurface(surface) { }

    virtual void eventFired()
    {
        if (Surface::SharedPtr surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(mSurface)) {
            mSync.wait();
            OpenGLContext::context()->state()->bindTexture(surface->getTexture(), true);
        }
    }

private:
    Surface::WeakPtr mSurface;
    OpenGLContext::Sync mSync;
};

class GLReadEvent : public GibbonApplication::Event
{
public:
    GLReadEvent(const Surface::SharedPtr& surface, unsigned char* data)
        : mMutex(ZERO_MUTEX, "GLReadEvent mutex"), mDone(false),
          mValid(false), mSurface(surface), mData(data)
    {
    }

    virtual void eventFired()
    {
        assert(OpenGLContext::hasContext());
        mValid = OpenGLContext::context()->readPixels(mSurface, mData);

        ScopedMutex locker(mMutex);
        mDone = true;
        mCond.signal();
    }
    virtual void eventCanceled()
    {
        ScopedMutex locker(mMutex);
        mDone = true;
        mCond.signal();
    }

    void wait()
    {
        ScopedMutex locker(mMutex);
        while (!mDone) {
            mCond.wait(&mMutex);
        }
    }

    bool isValid()
    {
        ScopedMutex locker(mMutex);
        return mValid;
    }

private:
    Mutex mMutex;
    ConditionVariable mCond;

    bool mDone, mValid;
    Surface::SharedPtr mSurface;
    unsigned char* mData;
};
}} // netflix::gibbon

int Surface::getGraphicsMemoryUsed()
{
    return -1;
}

int Surface::getGraphicsMemoryCapacity()
{
    return -1;
}

int Surface::atlasPadding()
{
    return 1;
}

unsigned int Surface::calculateSize() const
{
    if (mode() == Subsurface)
        return 0;
    return estimateSize(getWidth(), getHeight(), mFlags);
}

void Surface::createTexture_internal()
{
    assert(!mTexture);
    assert(!(mFlags & Flags_Screen));
    mTexture = OpenGLContext::context()->createTexture((bytesPerPixel() == 1 ? GL_ALPHA : GL_RGBA), getWidth(), getHeight());
}

void Surface::init()
{
    mTexture = 0;
    if(mFlags & Flags_Screen) {
        mValid = true;
    } else if (!mRect.empty()) {
        if (mGroup == -1 && (GibbonConfiguration::uploadInDecodeThreads() || Animation::isAnimationThread()))
            createTexture();
        mValid = true;
    } else {
        mValid = false;
    }
}

void Surface::cleanup()
{
    if(mValid) {
        if (!GibbonConfiguration::uploadInDecodeThreads() && !Animation::isAnimationThread()) {
            OpenGLContext::addTextureToDelete(mTexture);
            mTexture = 0;
            mValid = false;
            return;
        }
        OpenGLContext::ensureContext();
        OpenGLContext::context()->state()->unbindTexture(mTexture);
        glDeleteTextures(1, &mTexture);
        mTexture = 0;
        mValid = false;
    }
}

bool Surface::isValid() const
{
    return mValid;
}

bool Surface::isAccelerated() const
{
    return true;
}

bool Surface::matches(uint8_t flags) const
{
    return bytesPerPixel(flags) == bytesPerPixel();
}

void Surface::upload(int x, int y, int width, int height, const unsigned char* data, int pad)
{
    modify();

    OpenGLContext* ctx = OpenGLContext::context();
    createTexture();
    if (pad)
        ctx->pad(shared_from_this(), x, y, width + pad, height + pad);

    ctx->state()->bindTexture(mTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, (bytesPerPixel() == 1 ? GL_ALPHA : GL_RGBA), GL_UNSIGNED_BYTE, data);
}

void Surface::updateSubsurface_sys()
{
}

bool Surface::Data::lockSurface(LockType type)
{
    if (!mSurface->isValid())
        return false;
    if (mSurface->bytesPerPixel() == 1)
        mFormat = Surface::Format_AlphaOnly;
    else
        mFormat = GibbonApplication::instance()->getScreen()->getFormat(mSurface->hasAlpha() ? Screen::SurfaceFormat_Alpha : Screen::SurfaceFormat_Opaque);
    const bool hasData = (mData != 0);
    if (!hasData)
        lock_internal();
    assert(mData);

    if (!hasData && (type & Surface::Data::ReadLock)) {
        // we need to read back
        //glReadPixels(0, 0, mWidth, mHeight, alphaOnly ? GL_ALPHA : GL_RGBA, GL_UNSIGNED_BYTE, mData);
#ifdef GIBBON_NO_THREADANIMATION
        const bool isGLThread = Application::isAppThread();
#else
        const bool isGLThread = Animation::isAnimationThread();
#endif
        if (isGLThread || GibbonConfiguration::uploadInDecodeThreads()) {
            if (!OpenGLContext::context()->readPixels(mSurface, mData)) {
                unlock_internal();
                mBytesPerLine = 0;
                return false;
            }
        } else {
            // need to post an event and wait
            shared_ptr<GLReadEvent> event(new GLReadEvent(mSurface, mData));
#ifdef GIBBON_NO_THREADANIMATION
            app()->postEvent(event);
#else
            Animation::postEvent(event);
#endif
            event->wait();
            if (!event->isValid()) {
                unlock_internal();
                mBytesPerLine = 0;
                return false;
            }
        }
    }

    mBytesPerLine = getWidth() * bytesPerPixel();
    return true;
}

void Surface::Data::unlockSurface()
{
    if (mData) {
#ifdef GIBBON_NO_THREADANIMATION
        const bool isGLThread = Application::isAppThread();
#else
        const bool isGLThread = Animation::isAnimationThread();
#endif
        if (!GibbonConfiguration::uploadInDecodeThreads() && !isGLThread) {
            assert(!OpenGLContext::hasContext());
            return;
        }
        if (mLockType == WriteLock && mSurface->mode() == Surface::Subsurface) {
            return;
        }
        if (!isGLThread)
            OpenGLContext::ensureContext();
        if (mSurface && (mLockType & WriteLock)) {
            mSurface->upload(0, 0, mWidth, mHeight, mData);
            if (!isGLThread) {
#ifdef GIBBON_NO_THREADANIMATION
                app()->postEvent(new GLSyncEvent(mSurface));
#else
                Animation::postEvent(new GLSyncEvent(mSurface));
#endif
            }
        }
        unlock_internal();
    }
}
