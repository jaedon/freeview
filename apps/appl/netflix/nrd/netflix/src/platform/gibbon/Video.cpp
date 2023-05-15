/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Video.h"

#include "Debug.h"
#include "Widget.h"

#include <nrd/IMediaSourcePlayer.h>
#include <nrd/IAdaptiveStreamingPlayer.h>

using namespace netflix::gibbon;
using namespace netflix;

Video::Video(shared_ptr<Widget> widget) : mWidget(widget)
{
}

Video::~Video()
{
    cleanup();
}

void Video::setPlayer(const shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> &player)
{
    mMediaSourcePlayer = player;
    init();
}

bool Video::setRect(const netflix::gibbon::Rect &rect)
{
    const Size graphicsSize(GibbonConfiguration::screenWidth(), GibbonConfiguration::screenHeight());
    uint32_t outputWidth = (uint32_t)graphicsSize.width, outputHeight = (uint32_t)graphicsSize.height;
    if(shared_ptr<netflix::device::ISystem> system = GibbonApplication::instance()->getSystem())
        system->getVideoOutputResolution(outputWidth, outputHeight);

    netflix::Rect videoWindow;
    videoWindow.x = int(rect.x * (float(outputWidth) / graphicsSize.width));
    videoWindow.y = int(rect.y * (float(outputHeight) / graphicsSize.height));
    videoWindow.width = int(rect.width * (float(outputWidth) / graphicsSize.width));
    videoWindow.height = int(rect.height * (float(outputHeight) / graphicsSize.height));

    if(mMediaSourcePlayer) {
        mMediaSourcePlayer->setVideoWindow(videoWindow, 0, 0);
        return true;
    } else if(shared_ptr<netflix::mediacontrol::IAdaptiveStreamingPlayer> player = nrdApp()->player()) {
        player->setVideoWindow(videoWindow, 0);
        return true;
    }
    return false;
}

bool Video::bringToFront()
{
    if(mMediaSourcePlayer) {
        //mMediaSourcePlayer->bringToFront();
        return true;
    }
    return false;
}

namespace netflix {
namespace gibbon {
class VideoUpdatePlayerCommand : public DisplayList::Command
{
public:
    inline VideoUpdatePlayerCommand(const Video::SharedPtr &video, const netflix::gibbon::Rect &rect) : Command(Type_Unknown), mVideo(video), mRect(rect) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "VideoUpdatePlayer(" << mRect << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *) const {
        if(Video::SharedPtr video = mVideo.lock()) //make sure we are still valid
            video->updatePlayer(graphics, mRect);
    }
    virtual void write(DisplayList::Writer &) const { }
    virtual void read(DisplayList::Reader &) { }

private:
    Video::WeakPtr mVideo;
    const netflix::gibbon::Rect mRect;
};
}}

void Video::updatePlayer(GraphicsEngine *graphics, const netflix::gibbon::Rect &globalRect)
{
    assert(graphics);
    if(DisplayList::SharedPtr displayList = graphics->getDisplayList()) {
        displayList->appendCommand(new VideoUpdatePlayerCommand(shared_from_this(), globalRect));
    } else {
        assert(graphics->getSurface() && (graphics->getSurface()->getFlags() & Surface::Flags_Screen));
        setRect(globalRect);
        bringToFront();
    }
}

#ifndef GIBBON_VIDEO_PLATFORM
void Video::render(GraphicsEngine *graphics, const netflix::gibbon::Rect &globalRect)
{
    updatePlayer(graphics, globalRect);
}

void Video::cleanup()
{
    setRect(netflix::gibbon::Rect(0, 0, 0, 0));
}

void Video::init()
{
}
#endif
