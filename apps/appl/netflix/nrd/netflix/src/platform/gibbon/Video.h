/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <Rect.h>
#include <GraphicsEngine.h>

#include <nrd/NrdConfiguration.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>

namespace netflix {


namespace mediacontrol {
class IMediaSourcePlayer;
class IAdaptiveStreamingPlayer;
}

namespace gibbon {

class Widget;
class VideoUpdatePlayerCommand;

class Video : public enable_shared_from_this<Video>
{
public:
    typedef shared_ptr<Video> SharedPtr;
    typedef weak_ptr<Video> WeakPtr;

    Video(shared_ptr<Widget> widget);
    ~Video();

    inline shared_ptr<Widget> getWidget() const { return mWidget.lock(); }

#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    void flip();
#endif

    void render(GraphicsEngine *graphics, const Rect &globalRect);

    inline shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> getPlayer() const {return mMediaSourcePlayer;}
    void setPlayer(const shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> &player);

    bool setRect(const Rect &rect);
    bool bringToFront();

    inline Size sizeContent() const { return Size(GibbonConfiguration::screenWidth(), GibbonConfiguration::screenHeight()); }

    void init();

protected:
    friend class VideoUpdatePlayerCommand;
    void updatePlayer(GraphicsEngine *graphics, const Rect &globalRect);

    weak_ptr<Widget> mWidget;
    shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> mMediaSourcePlayer;

#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    Surface::SharedPtr mSurface;
#endif

    void cleanup();
};

}} // namespace netflix::gibbon

#endif /* __VIDEO_H__ */
