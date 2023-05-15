/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <Surface.h>
#include <SurfaceCache.h>
#include <GraphicsEngine.h>
#include <GibbonResourceManager.h>

#include <nrdbase/Time.h>
#include <nrdbase/tr1.h>

#include <nrdnet/IAsyncHttpLib.h>

#include <string>
#include <vector>

namespace netflix {
namespace gibbon {

class Widget;
class ImageSurfaceLoader;

class Image : public enable_shared_from_this<Image>, public Surface::Controller::Listener
{
public:
    typedef shared_ptr<Image> SharedPtr;
    typedef weak_ptr<Image> WeakPtr;

    static void shutdown();

    static SurfaceLoader::Source::SharedPtr createSource(const Variant &value);

    enum Type {
        Type_None,
        Type_Background,
        Type_Foreground
    };

    Image(int id = -1, shared_ptr<Widget> widget = shared_ptr<Widget>(), Type type = Type_None);
    ~Image();

    inline int getID() const { return mID; }

    class Listener
    {
        public:
        typedef shared_ptr<Listener> SharedPtr;
        typedef weak_ptr<Listener> WeakPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onImageLoaded(const Image::SharedPtr &, bool) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);

    inline bool hasContent() const { return getVisible() && hasUrl() && !hasFailed(); }

    std::string describe() const;

    bool isLoading() const;
    inline void load(const std::string &url, unsigned int flags=0) {
        load(SurfaceLoader::Source::SharedPtr(new SurfaceLoader::Source(AseUrl(url), flags)));
    }
    void load(const SurfaceLoader::Source::SharedPtr &source);
    inline unsigned int purge(uint8_t flags=0) { return surfaceLoaded(Surface::SharedPtr(), flags); }

    inline shared_ptr<Widget> getWidget() const { return mWidget.lock(); }

    inline bool isActiveLoader(const SurfaceLoader::SharedPtr &loader) const;
    inline bool hasUrl() const { return mSource && mSource->type() != SurfaceLoader::Source::Type_None; }
    const shared_ptr<SurfaceLoader::Source> &getSource() const { return mSource; }
    inline AseUrl getUrl() const { return mSource ? mSource->url() : AseUrl(); }
    inline DataBuffer getData() const { return mSource ? mSource->data() : DataBuffer(); }
    inline uint8_t loadFlags() const { return mSource ? mSource->loadFlags() : 0; }
    DataBuffer cacheKey() const { return mSource ? mSource->cacheKey() : DataBuffer(); }
    HttpHeaders requestHeaders() const { return mSource ? mSource->headers() : HttpHeaders(); }

    void setVisible(bool v);
    inline bool getVisible() const { return mFlags & Visible; }

    inline Type getType() const { return mType; }

    bool shouldDownload() const;
    inline bool canPurge() const { return mFlags & CanPurge; }
    inline void setCanPurge(bool p)
    {
        if (p) {
            mFlags |= CanPurge;
        } else {
            mFlags &= ~CanPurge;
        }
    }

    inline bool hasFailed() const { return mFlags & Failed; }
    inline Resource::State state() const { return mState; }
    inline NetworkMetrics getNetworkMetrics() const { return mNetworkInfo.metrics; }
    inline ullong getEncodedSize() const { return mNetworkInfo.encodedSize; }
    inline int getHttpStatusCode() const { return mNetworkInfo.httpStatusCode; }
    inline int getErrorCode() const { return mNetworkInfo.errorCode; }

    inline void setGroup(int group) { mGroup = group; }
    inline int group() const { return mGroup; }

    enum Alignment
    {
        Align_Normal = 0x00,
        Align_Top = Align_Normal,
        Align_Left = Align_Normal,
        Align_Center = 0x01,
        Align_Right = 0x02,
        Align_Bottom = Align_Right,
        Align_Tile = 0x04,
        Align_Stretch = 0x08,
        Align_MaintainAspectRatio = 0x10|Align_Stretch
    };
    inline uint8_t getHorizontalAlignment() const { return mHAlign; }
    void setHorizontalAlignment(uint8_t a);
    inline uint8_t getVerticalAlignment() const { return mVAlign; }
    void setVerticalAlignment(uint8_t a);

    enum AspectFitMode
    {
        Aspect_None = 0x0,
        Aspect_FitHorizontal = 0x1,
        Aspect_FitVertical = 0x2,
        Aspect_Fit = Aspect_FitHorizontal|Aspect_FitVertical
    };

    inline AspectFitMode aspectFitMode() const
    {
        uint32_t ret = Aspect_None;
        if((mHAlign & Align_MaintainAspectRatio) == Align_MaintainAspectRatio)
            ret |= Aspect_FitHorizontal;
        if((mVAlign & Align_MaintainAspectRatio) == Align_MaintainAspectRatio)
            ret |= Aspect_FitVertical;
        return static_cast<AspectFitMode>(ret);
    }

    inline Rect getSourceRect() const { return mSourceRect; }
    void setSourceRect(const Rect &rect);

    inline Rect getRenderRect() const { return calculateRenderRect(getWidth(), getHeight()); }
    Rect getDestinationRect(const Rect &targetRect) const;

    bool hasAlpha() const;
    unsigned int calculateSize() const;
    Coordinate getWidth() const;
    Coordinate getHeight() const;

    Size sizeContent() const;
    inline Size scaledSizeContent() const { return sizeContent().scaled(mScale, mScale); }
    inline float scale() const { return mScale; }

    void upload();
    Surface::SharedPtr reload();
    Surface::SharedPtr getSurface() const;
    void render(GraphicsEngine *graphics, const Rect &globalRect);

    void start();
    void stop();
    bool isRunning() const;
    bool isAnimated() const;
    virtual void onSurfaceChanged(const Surface::SharedPtr &surface, const Rect &rect);

private:
    Resource::State mState;
    struct NetworkInfo {
        NetworkInfo() : httpStatusCode(-1), errorCode(-1), encodedSize(0) { }
        NetworkMetrics metrics;
        int httpStatusCode, errorCode;
        ullong encodedSize;
    } mNetworkInfo;

    int mID;
    Type mType;
    enum Flags {
        CanPurge = 0x01,
        Failed = 0x02,
        Running = 0x04,
        Visible = 0x08,
        Empty = 0x10
    };
    unsigned int mFlags : 5;
    shared_ptr<SurfaceLoader::Source> mSource;
    int mGroup;
    float mScale;
    shared_ptr<ImageSurfaceLoader> mLoader;
    Surface::SharedPtr mSurface;
    weak_ptr<Widget> mWidget;
    uint8_t mHAlign, mVAlign;
    Rect mSourceRect;
    Size mSize;

    Rect calculateRenderRect(Coordinate width, Coordinate height) const;
    void sendRequest();
    void loadResource(const shared_ptr<Resource> &resource,
                      Resource::State state);
    enum {
        Loaded_SendLoadedEvent = 0x01,
        Loaded_Failure         = 0x02
    };
    unsigned int surfaceLoaded(const Surface::SharedPtr &surface, uint8_t loadedFlags);
    void sendImageLoaded(bool loaded);

    typedef WeakPtrSet<Listener> ListenerContainer;
    ListenerContainer mListeners;

    friend class ImageSurfaceDecodedEvent;
    friend class ImageSurfaceDownloadedEvent;
    friend class ImageSurfaceLoader;
};

}} // namespace netflix::gibbon

#endif // IMAGE_H
