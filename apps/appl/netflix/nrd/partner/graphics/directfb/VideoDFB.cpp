/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Video.h"
#include "Widget.h"

using namespace netflix;
using namespace netflix::gibbon;

class VideoDFBFlipEvent;
static Surface::SharedPtr sFlipSurface;
static Mutex sFlipMutex(ZERO_MUTEX, "VideoDFB::Flip");
static shared_ptr<VideoDFBFlipEvent> sFlipEvent;
class VideoDFBFlipEvent : public GibbonApplication::Event
{
public:
    VideoDFBFlipEvent(const Video::SharedPtr video) : mVideo(video) { }
    std::string describe() const { return std::string("VideoDFBFlipEvent"); }
    void eventFired()
    {
        if(Video::SharedPtr video = mVideo.lock()) {
            ScopedMutex _widget_lock(Widget::lock());
            video->flip();
        }
    }
    void eventCanceled()
    {
        ScopedMutex _lock(sFlipMutex);
        sFlipEvent.reset();
    }
private:
    Video::WeakPtr mVideo;
};

typedef bool (*DFBFlipFunction)(void *data, IDirectFBSurface *buffer, int srcWidth, int srcHeight, int dstWidth, int dstHeight);
extern void setVideoRendererDFBFlipFunction(DFBFlipFunction func, void *data);
static bool VideoRendererFlipFunction(void *data, IDirectFBSurface *buffer, int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
    assert(data);
    bool flipped = false;
    Video::WeakPtr *ptr = (Video::WeakPtr*)data;
    if(Video::SharedPtr video = ptr->lock()) {
        shared_ptr<VideoDFBFlipEvent> flipEvent;
        {
            ScopedMutex _lock(sFlipMutex);
            if(sFlipSurface) {
                flipped = true;
                if(!sFlipEvent) {
                    sFlipEvent.reset(new VideoDFBFlipEvent(video));
                    flipEvent = sFlipEvent;
                }
                DFBRectangle src, target;
                src.x = src.y = target.x = target.y = 0;
                src.w = srcWidth;
                src.h = srcHeight;
                target.w = dstWidth;
                target.h = dstHeight;
                {
                    IDirectFBSurface *dfb = sFlipSurface->getDFBSurface();
                    assert(dfb);
                    dfb->Clear(dfb, 0, 0, 0, 0);
                    dfb->SetBlittingFlags(dfb, DSBLIT_NOFX);
                    dfb->StretchBlit(dfb, buffer, &src, &target);
                }
            }
        }
        if(flipped) {
            if(flipEvent)
                app()->postEvent(flipEvent);
            return true;
        }
    }
    setVideoRendererDFBFlipFunction(0, 0);
    delete ptr;
    return true;
}

void Video::flip()
{
    {
        ScopedMutex _lock(sFlipMutex);
        std::swap(sFlipSurface, mSurface);
        sFlipEvent.reset();
    }
    if(Widget::SharedPtr widget = getWidget())
        widget->needsRender();
}

void Video::render(GraphicsEngine *graphics, const Rect &globalRect)
{
    if(mSurface)
        graphics->blit(mSurface, mSurface->getRect(), globalRect);
    else
        updatePlayer(graphics, globalRect);
}

void Video::cleanup()
{
    if(mSurface) {
        Surface::SharedPtr flipSurface;
        {
            ScopedMutex _lock(sFlipMutex);
            std::swap(sFlipSurface, flipSurface);
        }
        mSurface.reset();
    } else {
        setRect(netflix::gibbon::Rect(0, 0, 0, 0));
    }
}

void Video::init()
{
    if(GibbonConfiguration::useDFBVideo()) {
        const Size size = sizeContent();
        mSurface.reset(new Surface(size.width, size.height, Surface::Flags_Alpha));
        sFlipSurface.reset(new Surface(size.width, size.height, Surface::Flags_Alpha));
        setVideoRendererDFBFlipFunction(VideoRendererFlipFunction, new Video::WeakPtr(shared_from_this()));
    }
}

