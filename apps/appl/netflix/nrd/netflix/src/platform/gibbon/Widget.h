/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <Text.h>
#include <Rect.h>
#include <Point.h>
#include <Color.h>
#include <Image.h>
#include <Style.h>
#include <Video.h>
#include <Effects.h>
#include <Surface.h>
#include <DisplayList.h>
#include <Animation.h>
#include <GraphicsEngine.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/Noncopyable.h>

#include <vector>

namespace netflix {

namespace gibbon {



inline std::string prettySize(Coordinate width, Coordinate height)
{
    std::ostringstream stream;
    if(width == CoordinateMax)
        stream << "*";
    else
        stream << width;
    stream << "x";
    if(height == CoordinateMax)
        stream << "*";
    else
        stream << height;
    return stream.str();
}

inline std::string prettySize(const Size &size)
{
    return prettySize(size.width, size.height);
}

inline std::string prettyRect(const Rect &rect)
{
    std::ostringstream stream;
    stream << "[";
    if(rect.x == CoordinateMax)
        stream << "*";
    else
        stream << rect.x;
    stream << ",";
    if(rect.y == CoordinateMax)
        stream << "*";
    else
        stream << rect.y;
    stream << "+" << prettySize(rect.width, rect.height) << "]";
    return stream.str();
}

class Widget : public Effect::Listener, public AnimationObject, public Noncopyable, public Image::Listener
{
public:
    typedef std::list<Image::SharedPtr> Images;

    Widget(int id=-1);
    virtual ~Widget();
    virtual void init();

    typedef shared_ptr<Widget> SharedPtr;
    typedef std::list<Widget::SharedPtr> SharedList;
    typedef weak_ptr<Widget> WeakPtr;
    typedef std::list<Widget::WeakPtr> WeakList;

    typedef SharedList ChildContainer;

    struct Transformation {
        inline Transformation(float s=1.0, const Point &o=Point(0, 0)) : scale(s), origin(o) { }
        float scale;
        Point origin;
    };

    enum RenderCache {
        RenderCache_Default,
        RenderCache_Enabled,
        RenderCache_Disabled
    };

    enum LoadImages {
        LoadImages_Default,
        LoadImages_Enabled,
        LoadImages_Disabled
    };

    enum Layout {
        Layout_None,
        Layout_Horizontal,
        Layout_HorizontalWrapping,
        Layout_Vertical
    };

    enum Property
    {
        //Animated
        Property_X                        = 0x00000001,
        Property_Y                        = 0x00000002,
        Property_Width                    = 0x00000004,
        Property_Height                   = 0x00000008,
        Property_Scale                    = 0x00000010,
        Property_Opacity                  = 0x00000020,
        Property_ScrollX                  = 0x00000040,
        Property_ScrollY                  = 0x00000080,

        //Render output
        Property_RenderX                  = 0x00001000,
        Property_RenderY                  = 0x00002000,
        Property_RenderWidth              = 0x00004000,
        Property_RenderHeight             = 0x00008000,

        //Content properties
        Property_Clone                    = 0x00100000,
        Property_Text                     = 0x00200000,
        Property_Image                    = 0x00400000,
        Property_BackgroundImage          = 0x00800000,
        Property_BackgroundColor          = 0x01000000,
        Property_Erase                    = 0x02000000,
        Property_Layout                   = 0x04000000,
        Property_Video                    = 0x08000000,
        Property_ContentMask              = Property_Clone | Property_Text | Property_Image | Property_Video |
                                            Property_BackgroundImage | Property_BackgroundColor | Property_Erase,

        Property_Unknown                  = 0x00000000
    };

    virtual void startAnimatingProperty(int property, const AnimationStart &start);
    virtual void endAnimatingProperty(int property, const AnimationFinish &finish);
    virtual std::string getAnimatedPropertyName(int property) const;
    virtual int getAnimatedPropertyID(const std::string &property) const { return Widget::convertPropertyName(property); }
    virtual bool isIntegralProperty(int property) const;
    virtual float getAnimatedProperty(int property) const;
    virtual bool setAnimatedProperty(int property, float value);

    static Mutex &lock() { return sLock; }

    static bool isCoordinateProperty(Property property);
    static Property convertPropertyName(const std::string &property);
    static bool isAnimatedProperty(Property property);
    unsigned int getContentMask() const;

    inline unsigned int getAnimatingProperties() const { return mAnimatingProperties; }
    inline bool isAnimating() const { return getAnimatingProperties(); }

    class Listener
    {
    public:
        typedef shared_ptr<Listener> SharedPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onAnimationFinished(Property, const AnimationFinish &) { }
        virtual void onRenderPropertiesChanged(unsigned int) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);

    void dump(bool recursive);
    std::string describe(bool full) const;
    virtual std::string describe() const { return describe(false); }

    static Time lastPurged();
    enum PurgeType {
        Purge_Force,
        Purge_Preemptive,
        Purge_Offscreen
    };
    unsigned int purge(PurgeType type);
    static unsigned int purgeAll(PurgeType type);
    static void purgeText();

    void unsuspend();

    inline int getID() const { return mID; }

    inline std::string getName() const { return mName; }
    inline void setName(const std::string &name) { mName = name; }

    Widget::SharedPtr getRoot() const;
    inline bool isRoot() const { return (mFlags & WidgetFlag_Root); }
    void setRoot(bool r);

    inline SharedList widgetsAt(const Point &pos) const { return widgetsAt(Rect(pos, 1, 1)); }
    inline SharedList widgetsAt(const Rect &rect) const {
        assert(MutexStack::locked(Widget::lock().name()));
        updateContentMask();
        layout(false);
        SharedList result;
        widgetsAt_internal(rect, 1.0f, &result);
        return result;
    }


    inline Widget::SharedPtr getParent() const
    {
        assert(MutexStack::locked(Widget::lock().name()));
        return isRoot() ? Widget::SharedPtr() : mParent.widget.lock();
    }
    inline void setParent(const Widget::SharedPtr &p)
    {
        assert(MutexStack::locked(Widget::lock().name()));
        Widget::SharedPtr that = static_pointer_cast<Widget>(shared_from_this());
        if(Widget::SharedPtr parent = getParent()) {
            if(p == parent)
                return;
            parent->removeChild(that);
        }
        if(p)
            p->addChild(that);
    }

    inline Widget::SharedPtr sharedPtr() { return static_pointer_cast<Widget>(shared_from_this()); }
    inline Widget::SharedPtr sharedPtr() const { return const_cast<Widget*>(this)->sharedPtr(); }

    void addChild(const Widget::SharedPtr &child, int index=-1);
    void removeChild(const Widget::SharedPtr &child);
    inline bool hasChildren() const { return !mChildren.order.empty(); }
    inline int hasChildrenContent() const { return mChildren.hasContent; }
    inline const ChildContainer &getChildren(bool drawOrder=false) const {
        if(drawOrder) {
            updateDrawOrder();
            return mChildren.drawOrder;
        }
        return mChildren.order;
    }

    void setClone(Widget::SharedPtr clone);
    inline bool hasClone() const { return (bool)mClonedFrom; }
    inline Widget::SharedPtr getClone() const { return mClonedFrom; }

    void setLayoutStretch(int16_t stretch);
    inline int getLayoutStretch() const { return mLayout.stretch; }
    void setLayoutAlign(uint8_t align);
    inline uint8_t getLayoutAlign() const { return mLayout.align; }
    void setPadding(int16_t top, int16_t left, int16_t bottom, int16_t right, int16_t wrap=0);
    inline int16_t getTopPadding() const { return mPadding.top; }
    inline int16_t getLeftPadding() const { return mPadding.left; }
    inline int16_t getBottomPadding() const { return mPadding.bottom; }
    inline int16_t getRightPadding() const { return mPadding.right; }
    inline int16_t getLayoutWrapPadding() const { return mPadding.layoutWrap; }
    void setLayoutSpacing(int16_t spacing);
    inline int16_t getLayoutSpacing() const { return mLayoutSpacing; }

    void layout(bool force) const;
    void setLayout(Layout direction);
    inline Layout getLayout() const { return mLayout.direction; }
    inline bool isLayoutDirty() const { return (mFlags & WidgetFlag_DirtyLayout); }
    inline bool shouldLayout(Layout direction) {
        if(direction == Layout_None || !getVisible())
            return false;
        return mRect.x == CoordinateMax && mRect.y == CoordinateMax;
    }
    inline bool isInLayout() const { return mLayoutRect.x != CoordinateMax && mLayoutRect.y != CoordinateMax; }
    inline bool hasLayout() const { return mLayout.direction != Layout_None && hasChildren(); }

    bool shouldRenderCache() const;
    void setRenderCache(RenderCache c);
    inline RenderCache getRenderCache() const { return mCache.flags; }
    inline Surface::SharedPtr getCachedSurface() const { return mCache.surface; }

    bool isOnScreen() const;
    inline bool isRendered() const {
        assert(MutexStack::locked(Widget::lock().name()));
        if(!(mFlags & WidgetFlag_Rendered) || hasDirtyRect())
            return false;
        for(Widget::SharedPtr p = getParent(); p; p = p->getParent()) {
            if(!(p->mFlags & WidgetFlag_Rendered) || p->hasDirtyRect())
                return false;
        }
        return true;
    }
    inline bool isRendering() const { return (mFlags & WidgetFlag_Rendering); }
    inline bool isLayingOut() const { return (mFlags & WidgetFlag_LayingOut); }
    inline bool isPurging() const { return (mFlags & WidgetFlag_Purging); }
    inline static int getRenderDepth() { return RenderScope::getDepth(); }

    inline unsigned int getContent() const { return mContentMask; }
    inline int hasContent() const { return hasChildrenContent() + (mContentMask ? 1 : 0); }
    inline bool hasBackgroundImage() const {
        if(!mImages.empty()) {
            for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                if((*it)->getType() == Image::Type_Background && (*it)->hasContent())
                    return true;
            }
        }
        return false;
    }
    inline bool hasBackgroundColor() const { return mBackgroundColor.a; }
    inline bool hasImage() const {
        if(!mImages.empty()) {
            for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                if((*it)->getType() == Image::Type_Foreground && (*it)->hasContent())
                    return true;
            }
        }
        return false;
    }
    inline bool hasText() const { return mText && mText->hasText(); }
    inline bool hasPadding() const { return mPadding.left > 0 || mPadding.right > 0 || mPadding.top > 0 || mPadding.bottom > 0; }
    inline bool hasEffects() const { return !mEffects.effects.empty(); }
    inline DisplayList::SharedPtr getEffectDisplayList() const { return mEffects.rendered; }

    void setBackgroundColor(const Color& color);
    Color getBackgroundColor() const { return mBackgroundColor; }

    void setX(Coordinate v);
    inline Coordinate getX() const { return mRect.x; }
    void setY(Coordinate v);
    inline Coordinate getY() const { return mRect.y; }
    void setWidth(Coordinate v);
    void setHeight(Coordinate v);

    void setMinSize(const Size& s);
    inline const Size &getMinSize() const { return mMinSize; }
    void setMaxSize(const Size& s);
    inline const Size &getMaxSize() const { return mMaxSize; }

    Size getSize() const { return mRect.size(); }
    inline Coordinate getWidth() const { return mRect.width; }
    inline Coordinate getHeight() const { return mRect.height; }
    Size calculateSizeContent(Coordinate maxWidth=CoordinateMax, Coordinate maxHeight=CoordinateMax) const;

    void setRect(const Rect &r);
    inline bool hasDirtyRect() const { return mFlags & WidgetFlag_DirtyRect; }
    inline bool isFlexible() const { return mRect.width == CoordinateMax || mRect.height == CoordinateMax; }
    inline Rect getGlobalRect(bool clip=true) const { return mapRect(getTransformedClipRect(), Widget::SharedPtr(), clip); }
    inline const Rect &getRect() const { return mRect; }
    inline const Rect &getContentsRect() const
    {
        if((mFlags & (WidgetFlag_DirtyRect|WidgetFlag_Visible)) == (WidgetFlag_DirtyRect|WidgetFlag_Visible))
            updateRect();
        return mContentsRect;
    }
    inline Rect getTransformedRenderRect() const
    {
        if((mFlags & (WidgetFlag_DirtyRect|WidgetFlag_Visible)) == (WidgetFlag_DirtyRect|WidgetFlag_Visible))
            updateRect();
        Rect result = mRenderRect;
        transformRect(&result);
        return result;
    }
    inline const Rect &getRenderRect() const
    {
        if((mFlags & (WidgetFlag_DirtyRect|WidgetFlag_Visible)) == (WidgetFlag_DirtyRect|WidgetFlag_Visible))
            updateRect();
        return mRenderRect;
    }
    inline Rect getTransformedClipRect() const
    {
        if((mFlags & (WidgetFlag_DirtyRect|WidgetFlag_Visible)) == (WidgetFlag_DirtyRect|WidgetFlag_Visible))
            updateRect();
        Rect result = mClipRect;
        transformRect(&result);
        return result;
    }
    inline const Rect &getClipRect() const
    {
        if((mFlags & (WidgetFlag_DirtyRect|WidgetFlag_Visible)) == (WidgetFlag_DirtyRect|WidgetFlag_Visible))
            updateRect();
        return mClipRect;
    }
    inline Rect getPaddedRect() const
    {
        Rect result = getContentsRect();
        if(mPadding.left) {
            result.x += mPadding.left;
            result.width = std::max<Coordinate>(0, result.width-mPadding.left);
        }
        if(mPadding.top) {
            result.y += mPadding.top;
            result.height = std::max<Coordinate>(0, result.height-mPadding.top);
        }
        if(mPadding.right) {
            result.width = std::max<Coordinate>(0, result.width-mPadding.right);
        }
        if(mPadding.bottom) {
            result.height = std::max<Coordinate>(0, result.height-mPadding.bottom);
        }
        return result;
    }

    unsigned int reload(const Point &origin=Point(0, 0));
    void setLoadImages(LoadImages v);
    inline LoadImages getLoadImages() const { return mLoadImages; }
    inline bool shouldLoadImages() const {
        return mLoadImages != LoadImages_Disabled;
    }

    inline bool hasVideo() const { return (bool)getVideo(); }
    inline Video::SharedPtr getVideo() const { return mVideo; }
    Video::SharedPtr createVideo();
    void destroyVideo();

    void setErase(bool v);
    inline bool getErase() const { return (mFlags & WidgetFlag_Erase); }

    void setVisible(bool v);
    inline bool getVisible() const { return (mFlags & WidgetFlag_Visible); }

    void setSmoothScale(bool v);
    inline bool getSmoothScale() const { return GraphicsEngine::defaultSmoothScale() || (mFlags & WidgetFlag_SmoothScale); }

    void setScale(float scale);
    inline float getScale() const { return mTransform.scale; }

    void setScrollX(Coordinate v);
    void setScrollY(Coordinate v);
    inline Coordinate getScrollX() const { return getScrollOrigin().x; }
    inline Coordinate getScrollY() const { return getScrollOrigin().y; }
    inline Point getScrollOrigin() const { return mScroll.origin; }

    void setTransformOrigin(const Point& to);
    inline Point getTransformOrigin() const { return mTransform.origin; }

    void setClipEnabled(bool clipping);
    inline bool getClipEnabled() const { return (mFlags & WidgetFlag_Clips); }

    void setDrawOrder(int16_t order);
    inline int16_t getDrawOrder() const { return mDrawOrder; }

    void setOpacity(float opacity);
    inline float getOpacity() const { return mOpacity; }

    void setOpaque(bool opaque);
    inline bool getOpaque() const { return (mFlags & WidgetFlag_Opaque);  }
    bool hasOpaqueContent() const;

    void needsDrawOrder();
    void parentNeedsRender();
    void parentNeedsRender(const Rect &rect);
    void needsRender(bool needsCache) { needsRender(Rect(), needsCache); }
    void needsRender(const Rect &rect=Rect(), bool needsContent=true); //empty means next calculated area
    enum {
        NeedsRect_ForceContent = 0x01,
        NeedsRect_ForceLayout  = 0x02,
        NeedsRect_ForceAll = NeedsRect_ForceLayout|NeedsRect_ForceContent
    };
    void needsRect(uint8_t flags=0);
    void parentNeedsRect();
    void needsLayout();

    void exec(const Variant &);

    void stopImages();
    int reloadImages() const;
    void onImageLoaded(const Image::SharedPtr &, bool);
    void addImage(const Image::SharedPtr &image);
    void removeImage(const Image::SharedPtr &image);
    inline Images getImages() const { return mImages; }

    inline void setText(const shared_ptr<Text> &text) { mText = text; }
    inline shared_ptr<Text> getText() const { return mText; }

    void onEffectChanged(const Effect::SharedPtr &, const Variant &);
    void addEffect(const Effect::SharedPtr &effect);
    void removeEffect(const Effect::SharedPtr &effect);

    enum {
        Render_NoCache          = 0x01,
        Render_Comment          = 0x02
    };

    Rect prerender();
    DisplayList::SharedPtr grab(uint8_t flags=0);
    void render(GraphicsEngine *engine, const Rect& clipRect);
    void render(GraphicsEngine *engine, const Point& origin, const Rect& clipRect, uint8_t flags=0);

private:
    unsigned int purgeSurfaces(PurgeType type);
    unsigned int purge_internal(Rect rect, Point offset, PurgeType type);
    void purgeText_internal();

    Variant dump_internal(int i, bool recursive);
    unsigned int reload_internal(Rect rect, Point offset, Point childOffset);
    void renderPropertiesChanged(unsigned int) const;
    void widgetsAt_internal(const Rect &rect, float parentScale, SharedList *widgets) const;

    int mID;
    enum {
        WidgetFlag_Visible        = 0x0001,
        WidgetFlag_Clips          = 0x0002,
        WidgetFlag_Opaque         = 0x0004,
        WidgetFlag_Rendered       = 0x0008,
        WidgetFlag_Root           = 0x0010,
        WidgetFlag_Erase          = 0x0020,
        WidgetFlag_SmoothScale    = 0x0040,
        WidgetFlag_LoadImages     = 0x0080,
        WidgetFlag_DirtyRect      = 0x0100,
        WidgetFlag_DirtyLayout    = 0x0200,
        WidgetFlag_DirtyDrawOrder = 0x0400,
        WidgetFlag_DirtyRender    = 0x0800,
        WidgetFlag_DirtyContent   = 0x1000,
        WidgetFlag_Rendering      = 0x2000,
        WidgetFlag_LayingOut      = 0x4000,
        WidgetFlag_Purging        = 0x8000
    };
    mutable uint16_t mFlags;
    unsigned int mAnimatingProperties;
    mutable unsigned int mContentMask;

    mutable Rect mDirtyRect;
    Images mImages;
    Video::SharedPtr mVideo;
    Text::SharedPtr mText;
    LoadImages mLoadImages;
    Transformation mTransform;
    Color mBackgroundColor;
    int16_t mDrawOrder;
    float mOpacity;
    std::string mName;

    class FlagScope
    {
    public:
        inline FlagScope(const Widget *widget, uint16_t flags) : mWidget(widget) {
            const uint16_t existingFlags = mWidget->mFlags & flags;
            if(existingFlags)
                mFlags = flags & (~existingFlags);
            else
                mFlags = flags;
            mWidget->mFlags |= mFlags;
        }
        virtual ~FlagScope() {
            if(mFlags)
                mWidget->mFlags &= ~mFlags;
        }
    private:
        uint16_t mFlags;
        const Widget *mWidget;
    };

    class RenderScope : public FlagScope
    {
    public:
        inline RenderScope(const Widget *widget) : FlagScope(widget, WidgetFlag_Rendering) { ++sRenderDepth; }
        inline ~RenderScope() { --sRenderDepth; }
        inline static int getDepth() { return sRenderDepth; }
    private:
        static int sRenderDepth;
    };

    mutable struct {
        Point origin;
    } mScroll;

    mutable struct {
        unsigned short dirtied;
        RenderCache flags;
        Surface::SharedPtr surface;
        Rect dirtyRect, rect;
    } mCache;

    mutable struct {
        Rect rect;
        Effects effects;
        EffectRenderers renderers;
        DisplayList::SharedPtr rendered;
    } mEffects;

    mutable struct {
        unsigned short dirtied;
        Layout direction;
        int16_t stretch;
        uint8_t align;
    } mLayout;
    int16_t mLayoutSpacing;

    static Mutex sLock;
    static Time sLastPurgeTime;

    Rect mRect;
    Size mMinSize, mMaxSize;
    mutable Rect mContentsRect, mRenderRect, mClipRect, mLayoutRect;
    mutable struct { int16_t left, right, top, bottom, layoutWrap; } mPadding;

    Widget::SharedPtr mClonedFrom;
    std::list<Widget*> mClonedTo;

    struct {
        ChildContainer order;
        int16_t needsDrawOrder;
        mutable int16_t hasContent;
        mutable int16_t needsRender;
        mutable int16_t needsLayout;
        mutable ChildContainer drawOrder;
    } mChildren;
    struct {
        int index;
        Widget::WeakPtr widget;
    } mParent;

    class BoxLayout {
    public:
        BoxLayout(const Widget *widget) : mWidget(widget), mSize(CoordinateMax, CoordinateMax), mBoxesSpacing(0) { assert(mWidget); }

        struct WidgetBox {
            WidgetBox(Widget::SharedPtr wc) : x(0), y(0), width(CoordinateMax), height(CoordinateMax), stretch(0), surplus(0), widget(wc) {
                assert(widget);
                oldRenderRect = widget->mRenderRect;
            }
            int x, y;
            float width, height;
            inline Rect rect() const { return Rect(Coordinate(x), Coordinate(y), floorf(width), floorf(height)); }
            int stretch;
            float surplus;
            Rect oldRenderRect;
            Widget::SharedPtr widget;
        };

        inline void reserve(int cap) { mBoxes.reserve(cap); }
        inline bool isEmpty() const { return mBoxes.empty(); }
        inline void addWidget(Widget::SharedPtr wc) { addWidget(WidgetBox(wc)); }
        inline void addWidget(const WidgetBox &box) {
            if(!mBoxes.empty())
                mBoxesSpacing += box.widget->mLayoutSpacing;
            mBoxes.push_back(box);
        }

        inline Size size() const { return mSize; }
        enum {
            Flag_Debug       = 0x01,
            Flag_SizeContent = 0x02
        };
        const std::vector<WidgetBox> &layout(const Size &contentSize, uint8_t flags);

    private:
        const Widget *mWidget;
        Size mSize;
        int mBoxesSpacing;
        std::vector<WidgetBox> mBoxes;
    };

    typedef std::set<Listener::SharedPtr> ListenerContainer;
    ListenerContainer mListeners;

    void updateDrawOrder() const;
    void updateRect() const;
    Surface::SharedPtr getClonedSurface(Rect *rect) const;
    void updateRenderCache(const Point &origin) const;
    EffectRenderers updateEffectRenderers() const;
    int updateContentMask() const;
    int updateLayout(bool force, const Size &laidSize) const;
    void updateChildrenDeltas(Widget::SharedPtr child, bool increment);
    void unsetLayoutRect();
    bool setLayoutRect(const Rect &rect);
    Rect update() const;

    DisplayList::SharedPtr applyEffects(DisplayList::SharedPtr dl, Rect &rect) const;
    Size sizeContent(Coordinate width=CoordinateMax, Coordinate height=CoordinateMax) const;
    void loadImages() const;

    void renderEffect(GraphicsEngine *graphics, const Rect &rect);
    void renderContent(GraphicsEngine *graphics, const Rect& rect, uint8_t flags=0) const;
    void renderChildren(GraphicsEngine *graphics, const Point &origin, const Rect &clipRect, uint8_t flags=0) const;

    Rect mapRect(const Rect &rect, Widget::SharedPtr to, bool clip) const;
    void calculateChildrenRect(const Size &maxSize, Rect *unlaidChildrenRect, Rect *laidChildrenRect) const;
    inline void transformRect(Rect *rect) const {
        if(NRDP_UNLIKELY(!gibbon_float_compare(mTransform.scale, 1.0f))) {
            rect->x -= GIBBON_FLOOR(mTransform.origin.x * (mTransform.scale - 1.0f));
            rect->y -= GIBBON_FLOOR(mTransform.origin.y * (mTransform.scale - 1.0f));
            const float width = rect->width * mTransform.scale;
            rect->width = GIBBON_CEIL(width);
            const float height = rect->height * mTransform.scale;
            rect->height = GIBBON_CEIL(height);
        }
        transformEffectRect(rect);
    }
    inline void transformEffectRect(Rect *rect) const {
        if(NRDP_UNLIKELY(hasEffects())) {
            const EffectRenderers effects = updateEffectRenderers();
            for(EffectRenderers::const_iterator it = effects.begin(); it != effects.end(); ++it)
                *rect = (*it)->getRect(*rect);
        }
    }

    template <typename T> int boundSize(T *width, T *height) const {
        int result = 0;
        if(width) {
            if(mMinSize.width != CoordinateMax && *width < mMinSize.width) {
                ++result;
                *width = mMinSize.width;
            } else if(mMaxSize.width != CoordinateMax && *width > mMaxSize.width) {
                ++result;
                *width = mMaxSize.width;
            }
        }
        if(height) {
            if(mMinSize.height != CoordinateMax && *height < mMinSize.height) {
                ++result;
                *height = mMinSize.height;
            } else if(mMaxSize.height != CoordinateMax && *height > mMaxSize.height) {
                ++result;
                *height = mMaxSize.height;
            }
        }
        return result;
    }

    static bool drawOrderSortPredicate(const Widget::SharedPtr& w1, const Widget::SharedPtr& w2);
};


}} // namespace netflix::gibbon

#endif // WIDGET_H
