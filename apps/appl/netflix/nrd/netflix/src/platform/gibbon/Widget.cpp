/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Widget.h"

#include <Image.h>
#include <Debug.h>
#include <Screen.h>
#include <Surface.h>
#include <Animation.h>
#include <SurfaceCache.h>
#include <SurfaceDecoder.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>

#include <nrd/AppLog.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/tr1.h>
#include <nrdnet/ResourceManager.h>

#include <vector>
#include <algorithm>
#include <iostream>

static int sSizeContentIndent = 0, sUpdateIndent = 0;
#define DEBUG_WIDGET_CACHE(fmt, ...) if(NRDP_UNLIKELY(Debug::DebugWidgetCache)) Log::warn(TRACE_GIBBON_WIDGET, "WIDGET_CACHE:" fmt, __VA_ARGS__)
#define DEBUG_WIDGET_SIZECONTENT(fmt, ...) if(NRDP_UNLIKELY(Debug::DebugWidgetSize)) Log::warn(TRACE_GIBBON_WIDGET, "WIDGET_SIZECONTENT:" fmt, __VA_ARGS__)

#if 1
#define DEBUG_WIDGET_LAYOUT() if(debugLayout) Log::stream(TRACE_GIBBON_WIDGET, Log::Error) << "WIDGET_LAYOUT:"
#else
class NA {public: template <typename T> NA operator<<(const T &) { return *this; }};
#define DEBUG_WIDGET_LAYOUT() if (false) NA()
#endif

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonWidget);

Mutex Widget::sLock(GIBBON_WIDGET_MUTEX, "GibbonWidget");
Time Widget::sLastPurgeTime;
int Widget::RenderScope::sRenderDepth = -1;

namespace netflix {
namespace gibbon {

extern std::string gibbon_indent(int indent);

static inline void scaleRect(Rect *rect, float scale)
{
    if(!gibbon_float_compare(scale, 1.0f)) {
#ifdef GIBBON_COORDINATE_FLOAT
        rect->x *= scale;
        rect->y *= scale;
        rect->width *= scale;
        rect->height *= scale;
#else
        const float x = rect->x * scale, y = rect->y * scale;
        rect->x = (Coordinate)floor(x);
        rect->y = (Coordinate)floor(y);
        rect->width = (Coordinate)ceil((rect->width * scale)+fabs(x-rect->x));
        rect->height = (Coordinate)ceil((rect->height * scale)+fabs(y-rect->y));
#endif
    }
}

}} // netflix::applicaton::gibbon

static struct WidgetProperty
{
    const char *name;
    operator const char *() const { return name; }
    Widget::Property property;
    enum {
        Animated = 0x01,
        Integral = 0x02,
#if defined(GIBBON_COORDINATE_FLOAT)
        Coordinate = 0x04,
#else
        Coordinate = 0x04|Integral
#endif
    };
    uint8_t flags;

    inline static const WidgetProperty *findProperty(const std::string &property);
    inline static const WidgetProperty *findProperty(Widget::Property property);
} sPropertyInfo[] = {
    // these are sorted alphabetically and mapped by index in the two functions underneath
    { "backgroundColor", Widget::Property_BackgroundColor, 0 },
    { "erase", Widget::Property_Erase, 0 },
    { "height", Widget::Property_Height, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "opacity", Widget::Property_Opacity, WidgetProperty::Animated },
    { "renderHeight", Widget::Property_RenderHeight, WidgetProperty::Coordinate },
    { "renderWidth", Widget::Property_RenderWidth, WidgetProperty::Coordinate },
    { "renderX", Widget::Property_RenderX, WidgetProperty::Coordinate },
    { "renderY", Widget::Property_RenderY, WidgetProperty::Coordinate },
    { "scale", Widget::Property_Scale, WidgetProperty::Animated },
    { "scrollX", Widget::Property_ScrollX, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "scrollY", Widget::Property_ScrollY, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "text", Widget::Property_Text, 0 },
    { "width", Widget::Property_Width, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "x", Widget::Property_X, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "y", Widget::Property_Y, WidgetProperty::Animated|WidgetProperty::Coordinate },
    { "video", Widget::Property_Video, 0 },
    { 0, Widget::Property_Unknown, 0 }
};

const WidgetProperty *WidgetProperty::findProperty(const std::string &name)
{
    const int i = binarySearchString(name, sPropertyInfo, sizeof(sPropertyInfo) / sizeof(WidgetProperty));
    return i == -1 ? 0 : sPropertyInfo + i;
}
const WidgetProperty *WidgetProperty::findProperty(Widget::Property property)
{
    switch (property) {
    case Widget::Property_BackgroundColor: return sPropertyInfo + 0;
    case Widget::Property_Erase: return sPropertyInfo + 1;
    case Widget::Property_Height: return sPropertyInfo + 2;
    case Widget::Property_Opacity: return sPropertyInfo + 3;
    case Widget::Property_RenderHeight: return sPropertyInfo + 4;
    case Widget::Property_RenderWidth: return sPropertyInfo + 5;
    case Widget::Property_RenderX: return sPropertyInfo + 6;
    case Widget::Property_RenderY: return sPropertyInfo + 7;
    case Widget::Property_Scale: return sPropertyInfo + 8;
    case Widget::Property_ScrollX: return sPropertyInfo + 9;
    case Widget::Property_ScrollY: return sPropertyInfo + 10;
    case Widget::Property_Text: return sPropertyInfo + 11;
    case Widget::Property_Width: return sPropertyInfo + 12;
    case Widget::Property_X: return sPropertyInfo + 13;
    case Widget::Property_Y: return sPropertyInfo + 14;
    case Widget::Property_Video: return sPropertyInfo + 15;
    default: break;
    }
    return 0;
}

static int sID = 0xFF000000;

Widget::Widget(int id) :
    mID(id == -1 ? sID++ : id),
    mFlags(WidgetFlag_LoadImages|WidgetFlag_Visible|WidgetFlag_Clips),
    mAnimatingProperties(0),
    mContentMask(0),
    mLoadImages(LoadImages_Default),
    mBackgroundColor(0,0,0,0),
    mDrawOrder(0),
    mOpacity(1.0f),
    mLayoutSpacing(0),
    mRect(CoordinateMax, CoordinateMax, CoordinateMax, CoordinateMax),
    mMinSize(0, 0), mMaxSize(CoordinateMax, CoordinateMax),
    mLayoutRect(CoordinateMax, CoordinateMax, CoordinateMax, CoordinateMax)
{
    mChildren.needsLayout = mChildren.needsRender = mChildren.hasContent = mChildren.needsDrawOrder = 0;
    mCache.flags = Widget::RenderCache_Default;
    mLayout.dirtied = mCache.dirtied = 0;

    mLayout.stretch = 0;
    mLayout.direction = Widget::Layout_None;
    mLayout.align = Style::ALIGN_LEFT | Style::ALIGN_TOP;
    mPadding.left = mPadding.right = mPadding.bottom = mPadding.top = mPadding.layoutWrap = 0;
    NRDP_OBJECTCOUNT_REF(GibbonWidget, this);
}

Widget::~Widget()
{
    ScopedMutex _lock(Widget::lock());
    GibbonApplication::instance()->removeWidget(getID());
    for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it)
        (*it)->purge();
    mImages.clear();
    mText.reset();
    NRDP_OBJECTCOUNT_DEREF(GibbonWidget, this);
}

void Widget::init()
{
    GibbonApplication::instance()->addWidget(sharedPtr());
}

void Widget::needsLayout()
{
    if(!isLayingOut() && hasLayout() && !isLayoutDirty()) {
        if(NRDP_UNLIKELY(Debug::DebugWidgetLayout))
            Log::error(TRACE_GIBBON_WIDGET, "** NEEDSLAYOUT(%s)", describe().c_str());
        mFlags |= WidgetFlag_DirtyLayout;
        for(Widget::SharedPtr p = getParent(); p; p = p->getParent()) {
            ++p->mChildren.needsLayout;
            if(!p->getVisible())
                break;
        }
    }
}

void Widget::unsetLayoutRect()
{
    const Rect unlayoutRect(CoordinateMax, CoordinateMax, CoordinateMax, CoordinateMax);
    if(mLayoutRect != unlayoutRect) {
        DEBUG_WIDGET_SIZECONTENT("%s** UNLAID(%s) [%s]", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 mLayoutRect.toString().c_str());
        mFlags |= WidgetFlag_DirtyRect;
        mLayoutRect = unlayoutRect;
    }
}

bool Widget::setLayoutRect(const Rect &rect)
{
    assert(!isLayingOut());
    if(mLayoutRect != rect) {
        DEBUG_WIDGET_SIZECONTENT("%s** LAID(%s) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 rect.toString().c_str(), mLayoutRect.toString().c_str());
        mFlags |= WidgetFlag_DirtyRect;
        mLayoutRect = rect;
        needsLayout();
        return true;
    }
    return false;
}

void Widget::needsRect(uint8_t force)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mFlags & WidgetFlag_DirtyRect) {
        parentNeedsRect();
        return;
    }

    const bool flexible = isFlexible();
    if(!force && !flexible)
        return;

    DEBUG_WIDGET_SIZECONTENT("%s** NEEDSRECT(%s)", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str());
    mFlags |= WidgetFlag_DirtyRect;
    parentNeedsRect();
    unsetLayoutRect();
    if((!force || (force & NeedsRect_ForceLayout)) && hasLayout())
        needsLayout();
    for(std::list<Widget*>::iterator it = mClonedTo.begin(); it != mClonedTo.end(); ++it) {
        (*it)->parentNeedsRender();
        (*it)->needsRect();
    }
    if(flexible && isRoot() && getVisible() && !gibbon_float_is_zero(getOpacity()))
        needsRender(getTransformedClipRect());
}

Size Widget::sizeContent(Coordinate maxWidth, Coordinate maxHeight) const
{
    if(!isFlexible() || (maxWidth == CoordinateMax && maxHeight == CoordinateMax))
        return getContentsRect().size();
    return calculateSizeContent(maxWidth, maxHeight);
}

Size Widget::calculateSizeContent(Coordinate maxWidth, Coordinate maxHeight) const
{
    if(maxHeight == CoordinateMax)
        maxHeight = mRect.height;
    boundSize(&maxWidth, &maxHeight);
    if(maxWidth != CoordinateMax)
        maxWidth = std::max<Coordinate>(0, maxWidth - (mPadding.left + mPadding.right));
    if(maxHeight != CoordinateMax)
        maxHeight = std::max<Coordinate>(0, maxHeight - (mPadding.top + mPadding.bottom));

    DEBUG_WIDGET_SIZECONTENT("%sSIZECONTENT(%s) %s %s", gibbon_indent(sSizeContentIndent++).c_str(), describe().c_str(),
                             prettySize(maxWidth, maxHeight).c_str(), prettyRect(mRect).c_str());

    Rect unlaidChildrenRect, laidChildrenRect;
    if(hasChildren()) {
        calculateChildrenRect(Size(maxWidth, maxHeight), &unlaidChildrenRect, &laidChildrenRect);
        DEBUG_WIDGET_SIZECONTENT("%s+ SIZECONTENT(%s) CHILDREN %s %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 prettyRect(unlaidChildrenRect).c_str(), prettyRect(laidChildrenRect).c_str());
    }

    Rect contentsRect(0, 0, 0, 0);
    if(mRect.width != CoordinateMax)
        contentsRect.width = mRect.width;
    if(mRect.height != CoordinateMax)
        contentsRect.height = mRect.height;
    if(hasImage() && isFlexible()) {
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if((*it)->getType() == Image::Type_Foreground) {
                const Size imageSize = (*it)->sizeContent();
                DEBUG_WIDGET_SIZECONTENT("%s+ SIZECONTENT(%s) IMAGE(%p) %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                         (*it).get(), prettySize(imageSize).c_str());
                contentsRect |= imageSize;
            }
        }
    }
    if(mClonedFrom) {
        const Rect clonedRect(0, 0, mClonedFrom->getTransformedRenderRect().size());
        DEBUG_WIDGET_SIZECONTENT("%s+ SIZECONTENT(%s) CLONE %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 prettyRect(clonedRect).c_str());
        contentsRect |= clonedRect;
    }
    if(NRDP_UNLIKELY(hasVideo())) {
        const Size videoSize = mVideo->sizeContent();
        DEBUG_WIDGET_SIZECONTENT("%s+ SIZECONTENT(%s) VIDEO(%p) %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 mVideo.get(), prettySize(videoSize).c_str());
        contentsRect |= videoSize;
    }
    if(hasText() && isFlexible()) {
        const Size textSize = mText->sizeContent(Size(maxWidth, maxHeight));
        DEBUG_WIDGET_SIZECONTENT("%s+ SIZECONTENT(%s) TEXT %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                 prettySize(textSize).c_str());
        contentsRect |= textSize;
    }

    Size result(0, 0);

    //content
    if(laidChildrenRect.right() > result.width)
        result.width = laidChildrenRect.right();
    if(laidChildrenRect.bottom() > result.height)
        result.height = laidChildrenRect.bottom();
    if(contentsRect.width > result.width)
        result.width = contentsRect.width;
    if(contentsRect.height > result.height)
        result.height = contentsRect.height;
    if(result.width != CoordinateMax)
        result.width += mPadding.left + mPadding.right;
    if(result.height != CoordinateMax)
        result.height += mPadding.top + mPadding.bottom;

    //unlaid children
    if(unlaidChildrenRect.right() > result.width)
        result.width = unlaidChildrenRect.right();
    if(unlaidChildrenRect.bottom() > result.height)
        result.height = unlaidChildrenRect.bottom();

    //done
    boundSize(&result.width, &result.height);
#if GIBBON_COORDINATE_FLOAT
    DEBUG_WIDGET_SIZECONTENT("%s~SIZECONTENT(%s) (%d:%d:%d:%d) [%fx%f] ", gibbon_indent(--sSizeContentIndent).c_str(), describe().c_str(),
                             mPadding.left, mPadding.top, mPadding.right, mPadding.bottom, result.width, result.height);
#else
    DEBUG_WIDGET_SIZECONTENT("%s~SIZECONTENT(%s) (%d:%d:%d:%d) [%dx%d] ", gibbon_indent(--sSizeContentIndent).c_str(), describe().c_str(),
                             mPadding.left, mPadding.top, mPadding.right, mPadding.bottom, result.width, result.height);
#endif
    return result;
}

void Widget::calculateChildrenRect(const Size &maxSize, Rect *unlaidChildrenRect, Rect *laidChildrenRect) const
{
    if(hasChildren()) {
        DEBUG_WIDGET_SIZECONTENT("%sCHILDRENRECT(%s) [%s]", gibbon_indent(sSizeContentIndent++).c_str(), describe().c_str(), maxSize.toString().c_str());

        BoxLayout layout(this);
        const Layout direction = getLayout();
        for(ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
            Widget::SharedPtr wc = *it;
            if(!wc->getVisible())
                continue;
            if(wc->shouldLayout(direction)) {
                layout.addWidget(wc);
            } else {
                const Rect &wRect = wc->getTransformedClipRect();
                DEBUG_WIDGET_SIZECONTENT("%s+ UNLAID_CHILD(%s) %s %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                         wc->describe().c_str(), wRect.toString().c_str());
                *unlaidChildrenRect |= wRect;
            }
        }
        if(!layout.isEmpty()) {
            uint8_t flags = BoxLayout::Flag_SizeContent;
            if(NRDP_UNLIKELY(Debug::DebugWidgetSize))
                flags |= BoxLayout::Flag_Debug;
            const std::vector<BoxLayout::WidgetBox> &boxes = layout.layout(maxSize, flags);
            if(NRDP_UNLIKELY(Debug::DebugWidgetSize)) {
                for(size_t i = 0; i < boxes.size(); ++i) {
                    const BoxLayout::WidgetBox &box = boxes[i];
                    const Rect wRect(box.rect());
                    DEBUG_WIDGET_SIZECONTENT("%s+ LAID_CHILD(%s) %s %s", gibbon_indent(sSizeContentIndent).c_str(), describe().c_str(),
                                             box.widget->describe().c_str(), wRect.toString().c_str());
                }
            }
            Size layoutSize = layout.size();
            // if(layoutSize.width > maxSize.width)
            //     layoutSize.width = maxSize.width;
            // if(layoutSize.width > maxSize.height)
            //     layoutSize.height = maxSize.height;
            *laidChildrenRect |= layoutSize;
        }
        DEBUG_WIDGET_SIZECONTENT("%s~CHILDRENRECT(%s) %s %s", gibbon_indent(--sSizeContentIndent).c_str(), describe().c_str(),
                                 unlaidChildrenRect->toString().c_str(), layout.isEmpty() ? "[NA]" : laidChildrenRect->toString().c_str());
    }
}

void Widget::updateRect() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    assert((mFlags & WidgetFlag_DirtyRect) && getVisible());
    mFlags &= ~WidgetFlag_DirtyRect;

    const bool laid = isInLayout();
    const Rect oldRenderRect = mRenderRect;
    DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(%d) %s [%s]", gibbon_indent(sSizeContentIndent++).c_str(),
                             laid, describe().c_str(), prettyRect(mRect).c_str());

    //contents
    mContentsRect = mRect;
    if(mContentsRect.x == CoordinateMax)
        mContentsRect.x = 0;
    if(mContentsRect.y == CoordinateMax)
        mContentsRect.y = 0;
    if(isFlexible())  {
        const Size sc = calculateSizeContent(mRect.width, CoordinateMax);
        if(mRect.width == CoordinateMax)
            mContentsRect.width = sc.width;
        if(mRect.height == CoordinateMax)
            mContentsRect.height = sc.height;
        DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(content) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(),
                                 describe().c_str(), mContentsRect.toString().c_str());
    }

    //render
    if(laid) {
        mRenderRect = mLayoutRect;
        DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(layout) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(),
                                 describe().c_str(), mRenderRect.toString().c_str());
        if(!gibbon_float_compare(mTransform.scale, 1.0f)) {
            mRenderRect.width = GIBBON_ROUND(mRenderRect.width / mTransform.scale);
            mRenderRect.height = GIBBON_ROUND(mRenderRect.height / mTransform.scale);
        }
    } else {
        mRenderRect = mContentsRect;
        if(boundSize(&mRenderRect.width, &mRenderRect.height)) {
            DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(bounded) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(),
                                     describe().c_str(), mRenderRect.toString().c_str());
        }
    }

    //clip
    mClipRect = mRenderRect;
    if(!getClipEnabled() && hasChildren()) {
        Rect unlaidChildrenRect, laidChildrenRect;
        calculateChildrenRect(mRenderRect.size(), &unlaidChildrenRect, &laidChildrenRect);
        Rect childrenRect = laidChildrenRect | unlaidChildrenRect;
        if(childrenRect.width && childrenRect.height) {
            childrenRect.x += mClipRect.x;
            childrenRect.y += mClipRect.y;

            Rect enclosingRect = childrenRect;
            if(mClipRect.x < enclosingRect.x)
                enclosingRect.x = mClipRect.x;
            if(mClipRect.y < enclosingRect.y)
                enclosingRect.y = mClipRect.y;
            if(mClipRect.width <= 0)
                enclosingRect.right(childrenRect.right());
            else if(childrenRect.width <= 0)
                enclosingRect.right(mClipRect.right());
            else
                enclosingRect.right(mClipRect.right() > childrenRect.right() ? mClipRect.right() : childrenRect.right());
            if(mClipRect.height <= 0)
                enclosingRect.bottom(childrenRect.bottom());
            else if(childrenRect.height <= 0)
                enclosingRect.bottom(mClipRect.bottom());
            else
                enclosingRect.bottom(mClipRect.bottom() > childrenRect.bottom() ? mClipRect.bottom() : childrenRect.bottom());
            mClipRect = enclosingRect;
            DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(clip) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(),
                                     describe().c_str(), prettyRect(mClipRect).c_str());
        }
    }

    //cache
    if(mCache.rect != mRenderRect) {
        if(mCache.surface && (mRenderRect.width > mCache.rect.width || mRenderRect.height > mCache.rect.height))
            mCache.dirtyRect = mRenderRect;
        mCache.rect = Rect(0, 0, mRenderRect.width, mRenderRect.height);
        DEBUG_WIDGET_SIZECONTENT("%sUPDATERECT(cache) %s [%s]", gibbon_indent(sSizeContentIndent).c_str(),
                                 describe().c_str(), prettyRect(mCache.rect).c_str());
    }

    int properties = 0;
    if(mRenderRect.x != oldRenderRect.x)
        properties |= Property_RenderX;
    if(mRenderRect.y != oldRenderRect.y)
        properties |= Property_RenderY;
    if(mRenderRect.width != oldRenderRect.width)
        properties |= Property_RenderWidth;
    if(mRenderRect.height != oldRenderRect.height)
        properties |= Property_RenderHeight;
    if(properties)
        renderPropertiesChanged(properties);
    DEBUG_WIDGET_SIZECONTENT("%s~UPDATERECT(0x%x) %s [%s:%s:%s]", gibbon_indent(--sSizeContentIndent).c_str(), properties,
                             describe().c_str(), mContentsRect.toString().c_str(), mRenderRect.toString().c_str(), mClipRect.toString().c_str());
}

void Widget::setScrollX(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mScroll.origin.x != v) {
        const Coordinate scrollDelta = v - mScroll.origin.x;
        parentNeedsRender();
        mScroll.origin.x = v;
        needsRect(NeedsRect_ForceContent);
        if(mCache.surface) {
            const Rect srcRect = mCache.surface->getRect(),
                       dstRect = Rect(srcRect.x+scrollDelta, srcRect.y, srcRect.size());
            mCache.surface->offset(scrollDelta, 0);
            DEBUG_WIDGET_CACHE("** CACHE_COPY%d(%s) %s->%s %s", __LINE__, describe().c_str(),
                               srcRect.toString().c_str(), dstRect.toString().c_str(), mCache.surface->describe().c_str());
            if(mFlags & WidgetFlag_DirtyContent)
                mCache.dirtyRect.x += scrollDelta;
            if(scrollDelta < 0)
                needsRender(Rect(srcRect.width+scrollDelta, srcRect.y, -scrollDelta, srcRect.height));
            else
                needsRender(Rect(srcRect.x, srcRect.y, scrollDelta, srcRect.height));
        } else {
            needsRender();
        }
    }
}

void Widget::setScrollY(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mScroll.origin.y != v) {
        const Coordinate scrollDelta = v - mScroll.origin.y;
        parentNeedsRender();
        mScroll.origin.y = v;
        needsRect(NeedsRect_ForceContent);
        if(mCache.surface) {
            const Rect srcRect = mCache.surface->getRect(),
                       dstRect = Rect(srcRect.x, srcRect.y+scrollDelta, srcRect.size());
            mCache.surface->offset(0, scrollDelta);
            DEBUG_WIDGET_CACHE("** CACHE_COPY%d(%s) %s->%s %s", __LINE__, describe().c_str(),
                               srcRect.toString().c_str(), dstRect.toString().c_str(), mCache.surface->describe().c_str());
            if(mFlags & WidgetFlag_DirtyContent)
                mCache.dirtyRect.y += scrollDelta;
            if(scrollDelta < 0)
                needsRender(Rect(srcRect.x, srcRect.height+scrollDelta, srcRect.width, -scrollDelta));
            else
                needsRender(Rect(srcRect.x, srcRect.y, srcRect.width, scrollDelta));
        } else {
            needsRender();
        }
    }
}

void Widget::setX(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mRect.x != v) {
        parentNeedsRender();
        mRect.x = v;
        needsRect(NeedsRect_ForceContent);
        needsRender(false);
    }
}

void Widget::setY(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mRect.y != v) {
        parentNeedsRender();
        mRect.y = v;
        needsRect(NeedsRect_ForceContent);
        needsRender(false);
    }
}

void Widget::setWidth(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mRect.width != v) {
        parentNeedsRender();
        mRect.width = v;
        needsLayout();
        needsRect(NeedsRect_ForceAll);
        needsRender();
    }
}

void Widget::setHeight(Coordinate v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mRect.height != v) {
        parentNeedsRender();
        mRect.height = v;
        needsLayout();
        needsRect(NeedsRect_ForceAll);
        needsRender();
    }
}

void Widget::setRect(const Rect &r)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mRect != r) {
        unsigned int changed = 0, properties = 0;
        if(r.x != mRect.x) {
            changed |= Property_X;
            if(!(mAnimatingProperties & Property_X))
                properties |= Property_X;
        }
        if(r.y != mRect.y) {
            changed |= Property_Y;
            if(!(mAnimatingProperties & Property_Y))
                properties |= Property_Y;
        }
        if(r.width != mRect.width) {
            changed |= Property_Width;
            if(!(mAnimatingProperties & Property_Width))
                properties |= Property_Width;
        }
        if(r.height != mRect.height) {
            changed |= Property_Height;
            if(!(mAnimatingProperties & Property_Height))
                properties |= Property_Height;
        }

        parentNeedsRender();
        mRect = r;
        if(changed & (Property_Width|Property_Height)) {
            needsRect(NeedsRect_ForceAll);
            needsRender();
        } else if(properties) {
            needsRect(NeedsRect_ForceContent);
            parentNeedsRender();
        }
    }
}

void Widget::setMinSize(const Size& _s)
{
    assert(MutexStack::locked(Widget::lock().name()));
    Size s = _s;
    if(s.width < 0)
        s.width = 0;
    if(s.height < 0)
        s.height = 0;
    if(mMinSize != s) {
        parentNeedsRender();
        mMinSize = s;
        needsRect();
        needsRender();
    }
}

void Widget::setMaxSize(const Size& s)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mMaxSize != s) {
        parentNeedsRender();
        mMaxSize = s;
        needsRect();
        needsRender();
    }
}

void Widget::setBackgroundColor(const Color &color)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mBackgroundColor != color) {
        mBackgroundColor = color;
        needsRender();
    }
}

void Widget::setClone(Widget::SharedPtr clone)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mClonedFrom != clone) {
        parentNeedsRender();
        if(mClonedFrom) {
            for(std::list<Widget*>::iterator it = mClonedFrom->mClonedTo.begin();
                it != mClonedFrom->mClonedTo.end(); ++it) {
                if((*it) == this) {
                    mClonedFrom->mClonedTo.erase(it);
                    break;
                }
            }
        }
        mClonedFrom = clone;
        if(mClonedFrom)
            mClonedFrom->mClonedTo.push_back(this);
        needsRect();
        needsRender();
    }
}

void Widget::setScale(float scale)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mTransform.scale != scale) {
        parentNeedsRender();
        parentNeedsRect();
        mTransform.scale = scale;
        needsRect(NeedsRect_ForceAll);
        needsRender(false);
    }
}

void Widget::setTransformOrigin(const Point& to)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mTransform.origin != to) {
        parentNeedsRender();
        const Point oldTransformOrigin = mTransform.origin;
        mTransform.origin = to;
        needsRect();
        needsRender();
    }
}

void Widget::setRoot(bool r)
{
    {
        assert(MutexStack::locked(Widget::lock().name()));
        if(isRoot() != r) {
            parentNeedsRender();
            if(r)
                mFlags |= WidgetFlag_Root;
            else
                mFlags &= ~WidgetFlag_Root;
            needsRect();
            needsRender();
        }
    }
}

void Widget::setErase(bool v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(getErase() != v) {
        if(v)
            mFlags |= WidgetFlag_Erase;
        else
            mFlags &= ~WidgetFlag_Erase;
        needsRender();
    }
}

Video::SharedPtr Widget::createVideo()
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(!mVideo) {
        mVideo.reset(new Video(sharedPtr()));
        mVideo->init();
        needsRender();
    }
    return mVideo;
}

void Widget::destroyVideo()
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mVideo) {
        mVideo.reset();
        needsRender();
    }
}

void Widget::setSmoothScale(bool v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(getSmoothScale() != v) {
        if(v)
            mFlags |= WidgetFlag_SmoothScale;
        else
            mFlags &= ~WidgetFlag_SmoothScale;
        needsRender();
    }
}

void Widget::setVisible(bool v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(getVisible() != v) {
        if(Widget::SharedPtr parent = getParent())
            parent->updateChildrenDeltas(sharedPtr(), v);
        parentNeedsRender();
        if(v)
            mFlags |= WidgetFlag_Visible;
        else
            mFlags &= ~WidgetFlag_Visible;
        parentNeedsRect();
        needsRect(NeedsRect_ForceAll);
        needsRender();
    }
    if(!v)
        Animation::stop(sharedPtr(), 0, Animation::Stop_End);
}

void Widget::setClipEnabled(bool clipping)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(clipping != getClipEnabled()) {
        parentNeedsRender();
        if(clipping)
            mFlags |= WidgetFlag_Clips;
        else
            mFlags &= ~WidgetFlag_Clips;
        needsRect(NeedsRect_ForceContent);
        needsRender();
    }
}

void Widget::addListener(const Listener::SharedPtr &listener)
{
    mListeners.insert(listener);
}

void Widget::removeListener(const Listener::SharedPtr &listener)
{
    mListeners.erase(listener);
}

void Widget::setDrawOrder(int16_t order)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mDrawOrder != order) {
        const int16_t oldDrawOrder = mDrawOrder;
        mDrawOrder = order;
        if(Widget::SharedPtr parent = getParent()) {
            if(order && !oldDrawOrder)
                ++parent->mChildren.needsDrawOrder;
            if(!order && oldDrawOrder)
                --parent->mChildren.needsDrawOrder;
            parent->needsDrawOrder();
        }
        parentNeedsRender();
    }
}

void Widget::setOpacity(float opacity)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(!gibbon_float_compare(mOpacity, opacity)) {
        if(gibbon_float_is_zero(opacity))
            parentNeedsRender();
        mOpacity = opacity;
        if(shouldRenderCache())
            parentNeedsRender();
        else
            needsRender();
    }
}

void Widget::setOpaque(bool opaque)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(opaque != getOpaque()) {
        if(opaque)
            mFlags |= WidgetFlag_Opaque;
        else
            mFlags &= ~WidgetFlag_Opaque;
        needsRender();
    }
}

bool Widget::hasOpaqueContent() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(getOpaque() && getVisible() && gibbon_float_compare(mOpacity, 1.0f) &&
       getTransformedRenderRect() == GibbonApplication::instance()->getScreen()->getRootRect())
        return true;
    return false;
}

netflix::gibbon::Rect Widget::mapRect(const Rect &rect, Widget::SharedPtr to, bool clip) const
{
    Rect result = rect;
    for(Widget::SharedPtr p = getParent(); p; p = p->getParent()) {
        if(p == to)
            break;
        scaleRect(&result, p->mTransform.scale);
        const Rect parent_rect(p->getTransformedRenderRect());
        result.x += (p->getScrollX() + parent_rect.x);
        result.y += (p->getScrollY() + parent_rect.y);
        if(clip && p->getClipEnabled())
            result = parent_rect.intersected(result);
    }
    return result;
}

Widget::SharedPtr Widget::getRoot() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    Widget::SharedPtr result = sharedPtr();
    for(Widget::SharedPtr p = getParent(); p; p = p->getParent())
        result = p;
    return result;
}

void Widget::needsDrawOrder()
{
    if(mChildren.needsDrawOrder)
        mFlags |= WidgetFlag_DirtyDrawOrder;
    mChildren.drawOrder = mChildren.order;
}

bool Widget::drawOrderSortPredicate(const Widget::SharedPtr& w1, const Widget::SharedPtr& w2)
{
    return w1->mDrawOrder < w2->mDrawOrder;
}

void Widget::updateDrawOrder() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mFlags & WidgetFlag_DirtyDrawOrder) {
        mFlags &= ~WidgetFlag_DirtyDrawOrder;
        mChildren.drawOrder.sort(Widget::drawOrderSortPredicate);
    }
}

void Widget::widgetsAt_internal(const Rect &rect, float parentScale, SharedList *widgets) const
{
    Rect clipRect = getTransformedClipRect();
    scaleRect(&clipRect, parentScale);
    const bool intersects = clipRect.intersects(rect);
    if((getClipEnabled() && !intersects) || !getVisible() || gibbon_float_is_zero(getOpacity()))
        return;

    if(hasChildren()) {
        const Rect renderRect = getTransformedRenderRect();
        parentScale *= mTransform.scale;
        const Rect subRect(rect.x - renderRect.x - mScroll.origin.x,
                           rect.y - renderRect.y - mScroll.origin.y, rect.width, rect.height);
        for(ChildContainer::reverse_iterator it = mChildren.drawOrder.rbegin(); it != mChildren.drawOrder.rend(); ++it)
            (*it)->widgetsAt_internal(subRect, parentScale, widgets);
    }
    if(intersects)
        widgets->push_back(sharedPtr());
}

void Widget::addChild(const Widget::SharedPtr &child, int index)
{
    {
        assert(MutexStack::locked(Widget::lock().name()));
        if(Widget::SharedPtr oldParent = child->getParent())
            oldParent->removeChild(child);

        bool circular = false;
        for(Widget::SharedPtr p = sharedPtr(); p; p = p->getParent()) {
            if(p == child) {
                circular = true;
                Log::error(TRACE_GIBBON_WIDGET, "CircularAddChild detected! %s [%s]", child->describe().c_str(), describe().c_str());
                break;
            }
        }
        if(!circular) {
            parentNeedsRender();
            if(index == -1) {
                mChildren.order.push_back(child);
            } else {
                bool found = false;
                for(ChildContainer::iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
                    if((*it)->mParent.index > index) {
                        mChildren.order.insert(it, child);
                        found = true;
                        break;
                    }
                }
                if(!found)
                    mChildren.order.push_back(child);
            }
            if(child->getDrawOrder())
                ++mChildren.needsDrawOrder;
            if(child->getVisible())
                updateChildrenDeltas(child, true);
            needsDrawOrder();
            child->mParent.index = index;
            child->mParent.widget = sharedPtr();
            needsLayout();
            needsRect();
            uint8_t needsRectFlags = 0;
            if(child->mFlags & WidgetFlag_DirtyRect)
                needsRectFlags |= NeedsRect_ForceContent;
            if(child->mFlags & WidgetFlag_DirtyLayout)
                needsRectFlags |= NeedsRect_ForceLayout;
            child->mFlags &= ~(WidgetFlag_DirtyRender|WidgetFlag_DirtyRect|WidgetFlag_DirtyLayout);
            child->needsRect(needsRectFlags);
            child->needsRender();
        }
    }
}

void Widget::removeChild(const Widget::SharedPtr &child)
{
    assert(MutexStack::locked(Widget::lock().name()));

    Widget::SharedPtr that(sharedPtr());
    assert(!child->getParent() || child->getParent() == that);
    if(child->getParent() == that) {
        parentNeedsRender();
        child->needsRender();
        needsRender();
        mChildren.order.erase(std::remove(mChildren.order.begin(), mChildren.order.end(), child), mChildren.order.end());
        if(child->getDrawOrder())
            --mChildren.needsDrawOrder;
        if(child->getVisible())
            updateChildrenDeltas(child, false);
        mChildren.drawOrder.erase(std::remove(mChildren.drawOrder.begin(), mChildren.drawOrder.end(), child), mChildren.drawOrder.end());
        child->mParent.widget.reset();
        needsLayout();
        needsRect();
        needsRender();
    }
}

void Widget::updateChildrenDeltas(Widget::SharedPtr child, bool increment)
{
    int hasContentDelta = child->mChildren.hasContent;
    if(child->mContentMask)
        ++hasContentDelta;
    int needsRenderDelta = child->mChildren.needsRender;
    if(child->mFlags & WidgetFlag_DirtyRender)
        ++needsRenderDelta;
    int needsLayoutDelta = child->mChildren.needsLayout;
    if(child->mFlags & WidgetFlag_DirtyLayout)
        ++needsLayoutDelta;
    if(hasContentDelta || needsRenderDelta || needsLayoutDelta) {
        for(Widget::SharedPtr p = sharedPtr(); p; p = p->getParent()) {
            if(increment) {
                p->mChildren.hasContent += hasContentDelta;
                p->mChildren.needsRender += needsRenderDelta;
                p->mChildren.needsLayout += needsLayoutDelta;
            } else {
                p->mChildren.hasContent = std::max(p->mChildren.hasContent-hasContentDelta, 0);
                p->mChildren.needsRender = std::max(p->mChildren.needsRender-needsRenderDelta, 0);
                p->mChildren.needsLayout = std::max(p->mChildren.needsLayout-needsLayoutDelta, 0);
            }
            if(!p->getVisible())
                break;
        }
    }
}

int Widget::updateContentMask() const
{
    if(!getVisible())
        return 0;

    int contentDelta = 0;
    if(mFlags & WidgetFlag_DirtyRender) {
        const unsigned int previousContentMask = mContentMask;
        mContentMask = getContentMask();
        if(static_cast<bool>(previousContentMask) != static_cast<bool>(mContentMask))
            contentDelta = mContentMask ? 1 : -1;
    }

    if(mChildren.needsRender && hasChildren()) {
        for(ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
            const int childrenContentDelta = (*it)->updateContentMask();
            if(childrenContentDelta) {
                mChildren.hasContent += childrenContentDelta;
                const_cast<Widget*>(this)->needsLayout();
                contentDelta += childrenContentDelta;
            }
        }
    }
    return contentDelta;
}

Rect Widget::update() const
{
    assert(MutexStack::locked(Widget::lock().name()));

    Rect updated;
    if(!getVisible()) {
        mFlags &= ~(WidgetFlag_DirtyRender|WidgetFlag_DirtyContent);
        return updated;
    }

    const bool textCursor = hasText() && mText->hasTextCursor();
    if(textCursor)
        mText->updateCursor();

    const bool dirtyRender = (mFlags & WidgetFlag_DirtyRender);
    if(!dirtyRender && !mChildren.needsRender) {
        mDirtyRect.clear();
        return updated;
    }

    ++sUpdateIndent;
    const Rect renderRect = getTransformedRenderRect();
    const bool cache = shouldRenderCache();
    const bool dirtyContent = (mFlags & WidgetFlag_DirtyContent);
    if(!cache && mCache.surface) {
        DEBUG_WIDGET_CACHE("** CACHE_RESET%d(%s) %s", __LINE__, describe().c_str(), mCache.surface->describe().c_str());
        mCache.surface.reset();
    }

    if(textCursor)
        updated |= mText->getTextCursorBounds();

    if(dirtyRender || mChildren.needsRender) {
        const Rect clipRect = getTransformedClipRect();

        if(dirtyRender) {
            updated = clipRect;
            if(!mDirtyRect.empty()) {
                Rect dirty = mDirtyRect;
                dirty.x += renderRect.x;
                dirty.y += renderRect.y;
                updated &= dirty;
                mDirtyRect.clear();
            }
            mFlags &= ~WidgetFlag_DirtyRender;

            if(dirtyContent) {
                mFlags &= ~WidgetFlag_DirtyContent;
                if(cache) {
                    Rect surfaceRect = updated - renderRect.origin();
                    if(!gibbon_float_compare(mTransform.scale, 1.0f)) {
                        surfaceRect.width = GIBBON_CEIL(surfaceRect.width / mTransform.scale);
                        surfaceRect.height = GIBBON_CEIL(surfaceRect.height / mTransform.scale);
                    }
                    mCache.dirtyRect |= surfaceRect;
                }
            }
        }

        if(mChildren.needsRender) {
            Rect childrenRect;
            for (ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
                const Rect childRect = (*it)->update();
                childrenRect |= childRect;
            }
            childrenRect.x += mScroll.origin.x;
            childrenRect.y += mScroll.origin.y;
            if(cache)
                mCache.dirtyRect |= childrenRect + Point(renderRect.x-clipRect.x, renderRect.y-clipRect.y);
            scaleRect(&childrenRect, mTransform.scale);
            if(hasEffects()) { //Only account for the width/height the x/y is already accounted for in renderRect
                Rect effectedRect = childrenRect;
                transformEffectRect(&childrenRect);
                childrenRect.width = effectedRect.width;
                childrenRect.height = effectedRect.height;
            }
            childrenRect.x += renderRect.x;
            childrenRect.y += renderRect.y;
            if(getClipEnabled())
                childrenRect &= renderRect;
            updated |= childrenRect;
            mChildren.needsRender = 0;
        }
    }

    --sUpdateIndent;
    if(getVisible() && getOpacity()) {
        if(!updated.empty()) {
            mEffects.rendered.reset();
#ifdef BUILD_DEBUG
            if(NRDP_UNLIKELY(Debug::DebugScreenRender))
                Log::error(TRACE_GIBBON_WIDGET, "[%s:%d]: %s%s%s %s", __func__, __LINE__,
                                          dirtyRender ? "*" : " ", gibbon_indent(sUpdateIndent).c_str(),
                                          updated.toString().c_str(), describe().c_str());
#endif
        }
        return updated;
    }
    return Rect();
}

void Widget::parentNeedsRect()
{
    if(Widget::SharedPtr parent = getParent()) {
        if(parent->hasLayout())
            parent->needsLayout();
        if(parent->isFlexible() || !parent->getClipEnabled()) {
            parent->parentNeedsRender();
            parent->needsRect(NeedsRect_ForceContent);
        }
    }
}

void Widget::parentNeedsRender()
{
    if(isInLayout() || !(mFlags & WidgetFlag_DirtyRect)) {
        if(!getVisible() || gibbon_float_is_zero(getOpacity()))
            return;
        parentNeedsRender(getTransformedClipRect());
    }
}

void Widget::parentNeedsRender(const Rect &rect)
{
    if(!getVisible() || gibbon_float_is_zero(getOpacity()))
        return;
    if(Widget::SharedPtr parent = getParent())
        parent->needsRender(rect + parent->getScrollOrigin());
    else
        needsRender(rect);
}

void Widget::needsRender(const Rect &clipRect, bool needsContent)
{
    if(isRendering())
        return;

    assert(MutexStack::locked(Widget::lock().name()));

    const bool wasDirty = (mFlags & WidgetFlag_DirtyRender);
    mFlags |= WidgetFlag_DirtyRender;
    if(needsContent)
        mFlags |= WidgetFlag_DirtyContent;

    Rect globalRect = clipRect;
    bool empty = globalRect.empty();
    if(empty) {
        globalRect.clear();
        mDirtyRect.clear();
    } else {
        scaleRect(&globalRect, mTransform.scale);
        if(!wasDirty || !mDirtyRect.empty())
            mDirtyRect |= globalRect;
        if(!(mFlags & WidgetFlag_DirtyRect) && getVisible() && getOpacity()) {
            const Rect clipRect = getTransformedClipRect();
            const Rect &renderRect = getTransformedRenderRect();
            globalRect.x += renderRect.x;
            globalRect.y += renderRect.y;
            globalRect &= clipRect;
        } else {
            empty = true;
            globalRect.clear();
        }
    }
    if(empty && wasDirty)
        return;

    if(NRDP_UNLIKELY(Debug::DebugScreenRender))
        Log::error(TRACE_GIBBON_WIDGET, "[%s:%d]: %s %s", __func__, __LINE__, clipRect.toString().c_str(), describe().c_str());
    Widget::SharedPtr root = sharedPtr();
    for(Widget::SharedPtr parent = getParent(); parent; parent = parent->getParent()) {
        root = parent;
        if(!empty) {
            if(parent->getVisible() && parent->getOpacity() && !(parent->mFlags & WidgetFlag_DirtyRect)) {
                scaleRect(&globalRect, parent->mTransform.scale);
                const Rect parentClipRect = parent->getTransformedClipRect();
                const Rect &parentRenderRect = parent->getTransformedRenderRect();
                globalRect.x += (parent->getScrollX() + parentRenderRect.x);
                globalRect.y += (parent->getScrollY() + parentRenderRect.y);
                if(parent->getClipEnabled())
                    globalRect &= parentClipRect;
                if(globalRect.empty()) {
                    globalRect.clear();
                    empty = true;
                }
            } else {
                globalRect.clear();
                empty = true;
            }
            if(empty && wasDirty)
                break;
        }
        if(!wasDirty)
            ++parent->mChildren.needsRender;
    }
    for(std::list<Widget*>::iterator it = mClonedTo.begin(); it != mClonedTo.end(); ++it)
        (*it)->needsRender();
    if(root->isRoot() && (!empty || !wasDirty))
        GibbonApplication::instance()->getScreen()->update(globalRect, sharedPtr());
}

std::string Widget::describe(bool full) const
{
    std::ostringstream stream;
    stream << "Widget(" << getID() << ":" << this;
    if(!mName.empty())
        stream << ":" << mName;
    stream << ")";
    if(full) {
        const uint16_t oldFlags = mFlags;
        stream << ": " << "global:" << prettyRect(getGlobalRect(false)) << " ";
        stream << "rect:" << prettyRect(getRect()) << " ";
        if(!getClipEnabled()) {
            const Rect &cr = getTransformedClipRect();
            if(getTransformedRenderRect() != cr)
                stream << "clip:" << prettyRect(cr) << " ";
            else
                stream << "noclip ";
        }
        if(mAnimatingProperties) {
            stream << "anim=";
            for(int i = 0, cnt = 0; sPropertyInfo[i].name; ++i) {
                if((sPropertyInfo[i].flags & WidgetProperty::Animated) &&
                   mAnimatingProperties & sPropertyInfo[i].property) {
                    if(cnt++)
                        stream << ",";
                    stream << sPropertyInfo[i].name;
                }
            }
            stream << " ";
        }
        if(getDrawOrder())
            stream << "order:" << getDrawOrder() << " ";
        if(getScrollX() || getScrollY())
            stream << "scroll:" << getScrollX() << "," << getScrollY() << " ";
        if(getMinSize() != Size(0, 0))
            stream << "min:" << prettySize(getMinSize()) << " ";
        if(getMaxSize() != Size(CoordinateMax, CoordinateMax))
            stream << "max:" << prettySize(getMaxSize()) << " ";
        if(getTopPadding() || getLeftPadding() || getRightPadding() || getTopPadding())
            stream << "padding:" << getLeftPadding() << ":" << getTopPadding() << ":"
                   << getRightPadding() << ":" << getBottomPadding() << ":" << prettyRect(getPaddedRect()) << " ";
        if(mClonedFrom)
            stream << "cloned:" << mClonedFrom->describe() << " ";
        if(hasLayout()) {
            stream << "layout@" << mLayout.dirtied << ":";
            const Widget::Layout direction = getLayout();
            if(direction == Widget::Layout_Horizontal)
                stream << "h";
            else if(direction == Widget::Layout_Vertical)
                stream << "v";
            else if(direction == Widget::Layout_HorizontalWrapping)
                stream << "w";
            if(mLayout.align & Style::ALIGN_CENTER_VERTICAL)
                stream << ":" << "c";
            else if(mLayout.align & Style::ALIGN_BOTTOM)
                stream << ":" << "b";
            else if(mLayout.align & Style::ALIGN_RIGHT)
                stream << ":" << "r";
            if(isLayoutDirty())
                stream << "*";
            stream << " ";
        }
        if(getErase())
            stream << "erase ";
        if(hasVideo())
            stream << "video ";
        if(isInLayout()) {
            stream << "laid:" << prettyRect(mLayoutRect) << " ";
            if(getLayoutStretch())
                stream << "stretch:" << getLayoutStretch() << " ";
            if(getLayoutSpacing())
                stream << "space:" << getLayoutSpacing() << " ";
        }
        if(hasEffects()) {
            stream << "effects:" << mEffects.rendered.get();
            if(mEffects.rendered)
                stream << "[" << mEffects.rendered->getSurface().get() << "]";
            stream << ":" << mEffects.rect.toString();
            for(Effects::const_iterator it = mEffects.effects.begin(); it != mEffects.effects.end(); ++it)
                stream << ":" << (*it)->describe();
            stream << " ";
        }
        if(getSmoothScale())
            stream << "smooth ";
        if(!isOnScreen())
            stream << "offscreen" << (isRendered() ? "*" : "") << " ";
        else if(isRendered())
            stream << "rendered ";
        if(getOpaque())
            stream << "opaque ";
        if(!getVisible())
            stream << "hidden ";

        int contents = 0;
        if(mCache.surface) {
            if(getRenderCache() == RenderCache_Enabled)
                stream << "reqcached";
            else
                stream << "cached";
            stream << "@" << mCache.dirtied << ":" << mCache.rect << ":" << mCache.surface.get() ;
            if(!shouldRenderCache())
                stream << "*";
            stream << " ";
        } else if(shouldRenderCache()) {
            stream << "cachable ";
        }
        if(hasChildren()) {
            ++contents;
            stream << "kids:" << getChildren().size() << ":" << hasContent();
            if(mChildren.needsLayout)
                stream << ":l[" << mChildren.needsLayout << "]";
            if(mChildren.needsRender)
                stream << ":r[" << mChildren.needsRender << "]";
            if(mChildren.needsDrawOrder)
                stream << ":o[" << mChildren.needsDrawOrder << "]";
            stream << " ";
        }
        {
            const unsigned int cm = getContentMask();
            if(unsigned int content = getContent())
                stream << "content:0x" << std::hex << content << (cm == content ? " " : "* ") << std::dec;
            else if(cm)
                stream << "missing_content:0x" << std::hex << cm << std::dec << " ";
            if(hasBackgroundColor()) {
                ++contents;
                stream << "bg:" << int(getBackgroundColor().r) << ":" << int(getBackgroundColor().g) << ":"
                       << int(getBackgroundColor().b);
                if(getBackgroundColor().a != 255)
                    stream << ":(" << int(getBackgroundColor().a) << ")";
                stream << " ";
            }
            if(hasText()) {
                ++contents;
                stream << mText->describe();
            }
            for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                if((*it)->hasUrl()) {
                    ++contents;
                    stream << (*it)->describe();
                }
            }
        }
        if(contents) {
            stream << "content:" << contents << ":" << prettySize(getContentsRect().size());
            if(oldFlags & WidgetFlag_DirtyRect)
                stream << "*";
            stream << " ";
            if(!gibbon_float_compare(getScale(), 1.0f)) {
                stream << "scale:" << getScale();
                if(mTransform.origin.x || mTransform.origin.y)
                    stream << "@" << mTransform.origin;
                stream << " ";
            }
            if(!gibbon_float_compare(getOpacity(), 1.0f))
                stream << "opacity:" << getOpacity() << " ";
        } else {
            stream << "empty ";
        }
    }
    return stream.str();
}

Surface::SharedPtr Widget::getClonedSurface(Rect *rect) const
{
    assert(shouldRenderCache());
    const Rect widgetRect(getTransformedRenderRect());
    const Rect widgetClipRect = getTransformedClipRect();
    updateRenderCache(Point(widgetRect.x-widgetClipRect.x, widgetRect.y-widgetClipRect.y));
    *rect = mCache.rect;
    return mCache.surface;
}

void Widget::updateRenderCache(const Point &origin) const
{
    PERFORMANCE_MARKER_SCOPED("widget.updatecache");

    RenderScope _scope(this);
    if(!mCache.rect.width || !mCache.rect.height) {
        mCache.surface.reset();
        return;
    }
    if(mCache.surface && (mCache.surface->getWidth() < mCache.rect.width || mCache.surface->getHeight() < mCache.rect.height)) {
        DEBUG_WIDGET_CACHE("** CACHE_RESIZE%d(%s) %s->%s %s", __LINE__, describe().c_str(),
                           mCache.rect.size().toString().c_str(), mCache.surface->getSize().toString().c_str(), mCache.surface->describe().c_str());
        mCache.surface.reset();
    }

    const bool shouldHaveAlpha = (mBackgroundColor.a != 255 || !gibbon_float_compare(mOpacity, 1.0f)) && !getOpaque();
    if(!mCache.surface || !mCache.surface->isAccelerated() || shouldHaveAlpha != mCache.surface->hasAlpha()) {
        uint8_t flags = Surface::Flags_RenderTarget|Surface::Flags_Accelerated;
        if(shouldHaveAlpha)
            flags |= Surface::Flags_Alpha;
        for(int i = 0; i < 3; ++i) {
            if(i == 1) { //give a purge a try
                Widget::purgeAll(Widget::Purge_Offscreen);
            } else if(i == 2) { //ok, we'll settle for an unacclerated surface
                flags &= ~Surface::Flags_Accelerated;
                if(mCache.surface && mCache.surface->matches(flags))
                    break;
                Log::warn(TRACE_GIBBON_WIDGET, "** CACHE_SLOW(%s)", describe().c_str());
            }
            if(Surface::SharedPtr surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(mCache.rect.width, mCache.rect.height, flags, false)) {
                if(surface->isValid()) {
                    mCache.dirtied = 0;
                    mCache.surface = surface;
                    DEBUG_WIDGET_CACHE("** CACHE_CREATE(%s):0x%x: %s", describe().c_str(), flags, mCache.surface->describe().c_str());
                    mCache.dirtyRect = mCache.rect;
                    mCache.surface->setDescription("widgetcache: " + describe());
                    break;
                }
            }
        }
    }

    assert(MutexStack::locked(Widget::lock().name()));
    if(!mCache.dirtyRect.empty()) {
        Rect clipRect(mCache.surface->getRect());
        if(!GibbonConfiguration::screenRenderDisplayList())
            clipRect.intersect(mCache.dirtyRect);
        mCache.dirtyRect.clear();
        if(!clipRect.empty()) {
            mEffects.rendered.reset();
            ++mCache.dirtied;
            {
                GraphicsEngine graphics;
                DisplayList::SharedPtr displayList;
                if(GibbonConfiguration::screenRenderDisplayList()) {
                    displayList.reset(new DisplayList(mCache.surface->getSize()));
                    graphics.begin(displayList);
                } else {
                    graphics.begin(mCache.surface);
                }
                DEBUG_WIDGET_CACHE("** CACHE_UPDATE(%s@%d) %s :: %s", describe().c_str(), mCache.dirtied,
                                   clipRect.toString().c_str(), mCache.surface->describe().c_str());
                if(getSmoothScale())
                    graphics.setSmoothScale(true);
                graphics.setClip(clipRect);
                if(mCache.surface->hasAlpha())
                    graphics.clear(clipRect);
                else
                    graphics.fill(clipRect, Color(0, 0, 0));
                if((mContentMask & Property_ContentMask))
                    renderContent(&graphics, mCache.rect + mScroll.origin);
                if(hasChildren())
                    renderChildren(&graphics, origin + mScroll.origin, clipRect);
                if(NRDP_UNLIKELY(Debug::DrawWidgetCache)) {
                    graphics.setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
                    static const char *colors[] = { "maroon", "red", "orange", "yellow", "olive", "purple", "fuchsia", "white", "lime", 0 };
                    static int color = 0;
                    ++color;
                    if(!colors[color])
                        color = 0;
                    Color c(colors[color]);
                    c.a = 100;
                    graphics.fill(clipRect, c);
                }
                graphics.end();
                if(displayList)
                    mCache.surface->setDisplayList(displayList);
            }
        }
    }
    if(!mEffects.rendered && hasEffects()) {
        mEffects.rect = Rect(0, 0, mCache.rect.width, mCache.rect.height);
        mEffects.rendered = applyEffects(DisplayList::SharedPtr(new DisplayList(mCache.surface->duplicate(mCache.rect))), mEffects.rect);
    }
}

DisplayList::SharedPtr Widget::grab(uint8_t flags)
{
    assert(MutexStack::locked(Widget::lock().name()));
    {
        const Rect &r = getRoot()->prerender();
        if(!r.empty())
            GibbonApplication::instance()->getScreen()->update(r, sharedPtr());
    }

    const Rect r = getTransformedClipRect();
    if(r.empty())
        return DisplayList::SharedPtr();

    DisplayList::SharedPtr result(new DisplayList(r.width, r.height));
    {
        GraphicsEngine engine(result);
        const Rect widgetRect(0, 0, r.width, r.height);
        engine.clear(widgetRect);
        render(&engine, Point(r.x == CoordinateMax ? 0 : -r.x, r.y == CoordinateMax ? 0 : -r.y), widgetRect, flags);
    }
    if(hasEffects()) {
        Rect surfaceRect(result->getRect());
        result = applyEffects(result, surfaceRect);
    }
    return result;
}

Rect Widget::prerender()
{
    assert(isRoot());
    updateContentMask();
    layout(false);
    const Rect updated = update();
    return updated;
}

void Widget::render(GraphicsEngine *graphics, const Point &origin, const Rect &clipRect, uint8_t flags)
{
    //PERFORMANCE_MARKER_SCOPED(("widget.rendertree " + describe()).c_str(), false);
    PERFORMANCE_MARKER_SCOPED("widget.rendertree");

    PERFORMANCE_MARKER_START("widget.rendertree.pre");

    assert(MutexStack::locked(Widget::lock().name()));
    RenderScope _scope(this);
    if(!getVisible() || mOpacity <= 0 || !hasContent()) { //nothing to see here
        mFlags &= ~WidgetFlag_Rendered;
        PERFORMANCE_MARKER_END();
        return;
    }
    assert(!isLayoutDirty());

    const float oldScale = graphics->scale();
    const Rect transformedRenderRect(getTransformedRenderRect().scaled(oldScale, oldScale)+origin);
    const Rect graphicsClipRect(transformedRenderRect.intersected(clipRect));
    const bool hasGraphicsClipRect = !graphicsClipRect.empty();
    const bool clipEnabled = getClipEnabled();
    if(clipEnabled && !hasGraphicsClipRect) { //clipped away
        mFlags &= ~WidgetFlag_Rendered;
        if(!transformedRenderRect.empty()) {
            const Rect sr = GibbonApplication::instance()->getScreen()->getRootRect();
            if(transformedRenderRect.intersects(sr))
                mFlags |= WidgetFlag_Rendered;
        }
        PERFORMANCE_MARKER_END();
        return;
    }
    const bool scaled = !gibbon_float_compare(mTransform.scale, 1.0f);

    graphics->push(graphicsClipRect);
    if(flags & Render_Comment)
        graphics->comment("Rendering widget: " + describe() + "@" + graphicsClipRect.toString());

    if(hasGraphicsClipRect)
        graphics->setClip(graphicsClipRect);
    const bool oldSmoothScale = graphics->smoothScale();
    graphics->setSmoothScale(getSmoothScale());
    const float oldOpacity = graphics->opacity();
    if(!gibbon_float_compare(mOpacity, 1.0f))
        graphics->setOpacity(mOpacity*oldOpacity);
    if(scaled)
        graphics->setScale(mTransform.scale*oldScale);

    bool shouldRender = true;
    const bool effects = hasEffects();
    const Rect widgetRenderRect(getRenderRect());
    Rect widgetRect(widgetRenderRect + origin + mScroll.origin);
    if(scaled) {
        widgetRect.x -= GIBBON_FLOOR(mTransform.origin.x * (mTransform.scale - 1.0f));
        widgetRect.y -= GIBBON_FLOOR(mTransform.origin.y * (mTransform.scale - 1.0f));
    }
    if(effects)
        transformEffectRect(&widgetRect);

    if(!(flags & Render_NoCache)) {
        const bool cache = shouldRenderCache();
        if(mCache.surface) { //decide what to do with previously cached surfaces
            // Allow the cache surface to grow, but not shrink, saving us the allocation time at the expense of memory.
            if(!mCache.surface->isValid() ||
               gibbon_coordinate_greaterThan(mCache.rect.width, mCache.surface->getWidth()) ||
               gibbon_coordinate_greaterThan(mCache.rect.height, mCache.surface->getHeight())) {
                DEBUG_WIDGET_CACHE("** CACHE_RESIZE%d(%s) %s->%s %s)", __LINE__, describe().c_str(),
                                   mCache.rect.size().toString().c_str(), mCache.surface->getSize().toString().c_str(), mCache.surface->describe().c_str());
                mCache.surface.reset();
            } else if(!cache && !mCache.dirtyRect.empty()) {
                DEBUG_WIDGET_CACHE("** CACHE_RESET%d(%s) %s", __LINE__, describe().c_str(), mCache.surface->describe().c_str());
                mCache.surface.reset();
            }
        }

        if((mCache.surface || cache)) {
            const Rect widgetClipRect(getClipRect());
            updateRenderCache(Point(widgetRenderRect.x-widgetClipRect.x, widgetRenderRect.y-widgetClipRect.y));

            if(mContentMask & Property_Erase)
                graphics->clear(widgetRect);

            if(effects && mEffects.rendered) {
                DEBUG_WIDGET_CACHE("** EFFECT_APPLY(%s) %p %s->%s", describe().c_str(), mEffects.rendered.get(),
                                   mEffects.rect.toString().c_str(), widgetRect.toString().c_str());
                renderEffect(graphics, widgetRect);
            } else if(Surface::SharedPtr surface = mCache.surface) {
                GibbonApplication::instance()->getSurfaceCache()->touch(surface);
                if(surface->hasAlpha() || !gibbon_float_compare(graphics->opacity(), 1.0))
                    graphics->setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
                else
                    graphics->setCompositionMode(GraphicsEngine::CompositionMode_Source);
                const Rect blitRect = widgetRect - mScroll.origin;
                DEBUG_WIDGET_CACHE("** CACHE_BLIT(%s) %s->%s %s", describe().c_str(),
                                   mCache.rect.toString().c_str(), blitRect.toString().c_str(), surface->describe().c_str());
                graphics->blit(surface, mCache.rect, blitRect);
            }

            if(NRDP_UNLIKELY(Debug::DrawWidgetBlendMode)) {
                const float oldOpacity = graphics->opacity();
                bool blended;
                if((mContentMask & Property_Erase)) {
                    blended = false;
                } else {
                    blended = !gibbon_float_compare(oldOpacity, 1.0);
                    if(!blended && mCache.surface)
                        blended = mCache.surface->hasAlpha();
                }
                graphics->setOpacity(1.0f);
                graphics->setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
                if(blended) {
                    graphics->fill(widgetRect, Color(255, 0, 0, 80));
                    graphics->drawRect(widgetRect, Color(255, 0, 0, 160));
                } else {
                    graphics->fill(widgetRect, Color(0, 255, 0, 50));
                    graphics->drawRect(widgetRect, Color(0, 255, 0, 100));
                }
                graphics->setOpacity(oldOpacity);
            }

            if(NRDP_UNLIKELY(Debug::DrawWidgetCache)) {
                graphics->setCompositionMode(GraphicsEngine::CompositionMode_Source);
                graphics->drawRect(widgetRect, Color(0, 255, 0, 128));
                {
                    Style::SharedPtr style(new Style);
                    style->setFontFamily("Arial_for_Netflix");
                    style->setFontPixelSize(12);
                    style->setBackgroundColor(Color(192, 192, 192));
                    style->setColor(Color(0, 255, 0));
                    char buffer[1024];
                    sprintf(buffer, "c:%s:%p:%d", describe().c_str(), mCache.surface.get(), mCache.dirtied);
                    graphics->drawText(buffer, widgetRect, widgetRect, style);
                }
            }
            shouldRender = false;
        } else if(mCache.surface) {
            DEBUG_WIDGET_CACHE("** CACHE_RESET%d(%s) %s", __LINE__, describe().c_str(), mCache.surface->describe().c_str());
            mCache.surface.reset();
        }
    }

    PERFORMANCE_MARKER_END();

    if(shouldRender) {
        const bool shouldRenderChildren = hasGraphicsClipRect || !clipEnabled;
        if(effects) {
            if(!mEffects.rendered || (flags & Render_NoCache)) {
                Rect effectRect(0, 0, widgetRenderRect.width, widgetRenderRect.height);
                DisplayList::SharedPtr displayList(new DisplayList(effectRect.width, effectRect.height));
                GraphicsEngine displayGraphics(displayList);
                if((mContentMask & Property_ContentMask) && hasGraphicsClipRect)
                    renderContent(&displayGraphics, effectRect, flags);
                if(shouldRenderChildren && hasChildren())
                    renderChildren(&displayGraphics, Point(0, 0), effectRect, flags);
                mEffects.rendered = applyEffects(displayList, effectRect);
                mEffects.rect = effectRect;
            }
            if(mEffects.rendered)
                renderEffect(graphics, widgetRect);
        } else {
            if((mContentMask & Property_ContentMask) && hasGraphicsClipRect)
                renderContent(graphics, widgetRect, flags);
            if(shouldRenderChildren && hasChildren())
                renderChildren(graphics, widgetRect.origin(), clipEnabled ? graphicsClipRect : clipRect, flags);
        }
    }

    if(NRDP_UNLIKELY(Debug::DrawWidgetRectangles)) {
        graphics->setCompositionMode(GraphicsEngine::CompositionMode_Source);
        static const char *colors[] = { "maroon", "red", "orange", "yellow", "olive", "purple", "fuchsia", "white", "lime", 0 };
        static int color = 0;
        ++color;
        if(!colors[color])
            color = 0;
        graphics->drawRect(widgetRect, Color(colors[color]));
        {
            Style::SharedPtr style(new Style);
            style->setFontFamily("Arial_for_Netflix");
            style->setFontPixelSize(12);
            style->setBackgroundColor(Color(192, 192, 192));
            style->setColor(Color(255, 0, 0));
            std::ostringstream stream;
            stream << "w:";
            if(hasLayout())
                stream << "l:" << mLayout.dirtied;
            stream << describe();
            graphics->drawText(stream.str().c_str(), widgetRect, widgetRect, style);
        }
    }

    graphics->setScale(oldScale);
    graphics->setOpacity(oldOpacity);
    graphics->setSmoothScale(oldSmoothScale);
    if(hasGraphicsClipRect)
        graphics->clearClip();
    mFlags |= WidgetFlag_Rendered;
    graphics->pop();
}

void Widget::renderContent(GraphicsEngine *graphics, const Rect &globalRect, uint8_t flags) const
{
    PERFORMANCE_MARKER_SCOPED("widget.rendercontent");
    unsigned int contentMask = mContentMask;
    assert(contentMask & Property_ContentMask);

    const bool blend = !gibbon_float_compare(graphics->opacity(), 1.0f);
    const GraphicsEngine::CompositionMode blendMode = getOpaque() ? GraphicsEngine::CompositionMode_Source : GraphicsEngine::CompositionMode_SourceOver;

    if(NRDP_UNLIKELY(contentMask & Property_Erase)) {
        graphics->clear(globalRect);
        if(NRDP_LIKELY(!(contentMask &= ~Property_Erase)))
            goto renderContent_done;
    }

    if(NRDP_UNLIKELY(contentMask & Property_Video)) {
        if(flags & Render_Comment)
            graphics->comment("RenderVideo");
        graphics->setCompositionMode(blend ? blendMode : GraphicsEngine::CompositionMode_Source);
        mVideo->render(graphics, globalRect);
        if(NRDP_LIKELY(!(contentMask &= ~Property_Video)))
            goto renderContent_done;
    }

    if(contentMask & Property_BackgroundColor) {
        graphics->setCompositionMode(mBackgroundColor.a == 255 && !blend ? GraphicsEngine::CompositionMode_Source : blendMode);
        graphics->fill(globalRect, mBackgroundColor);
        if(NRDP_LIKELY(!(contentMask &= ~Property_BackgroundColor)))
            goto renderContent_done;
    }

    if(contentMask & Property_BackgroundImage) {
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if((*it)->getVisible() && (*it)->getType() == Image::Type_Background) {
                if(flags & Render_Comment)
                    graphics->comment("RenderBGImage: " + (*it)->getUrl().str());
                graphics->setCompositionMode(((*it)->hasAlpha() || blend) ? blendMode : GraphicsEngine::CompositionMode_Source);
                (*it)->render(graphics, globalRect);
            }
        }
        if(NRDP_LIKELY(!(contentMask &= ~Property_BackgroundImage)))
            goto renderContent_done;
    }

    {
        const Rect contentRect(globalRect.x+mPadding.left, globalRect.y+mPadding.top,
                               std::max<Coordinate>(0, globalRect.width-(mPadding.left+mPadding.right)),
                               std::max<Coordinate>(0, globalRect.height-(mPadding.top+mPadding.bottom)));
        if(!contentRect.empty()) {
            if(NRDP_UNLIKELY(contentMask & Property_Clone)) {
                assert(mClonedFrom);
                Rect clonedRect;
                if(Surface::SharedPtr clonedSurface = mClonedFrom->getClonedSurface(&clonedRect)) {
                    graphics->setCompositionMode((clonedSurface->hasAlpha() || blend) ? blendMode : GraphicsEngine::CompositionMode_Source);
                    GibbonApplication::instance()->getSurfaceCache()->touch(clonedSurface);
                    graphics->blit(clonedSurface, clonedRect, Rect(contentRect.x, contentRect.y,
                                                                   std::min(clonedRect.width, contentRect.width),
                                                                   std::min(contentRect.height, clonedRect.height)));
                }
                if(NRDP_LIKELY(!(contentMask &= ~Property_Clone)))
                    goto renderContent_done;
            }

            if(contentMask & Property_Image) {
                for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                    if((*it)->getType() == Image::Type_Foreground && (*it)->getVisible()) {
                        if(flags & Render_Comment)
                            graphics->comment("RenderFGImage: " + (*it)->describe());
                        graphics->setCompositionMode(((*it)->hasAlpha() || blend) ? blendMode : GraphicsEngine::CompositionMode_Source);
                        (*it)->render(graphics, contentRect);
                    }
                }
                if(NRDP_LIKELY(!(contentMask &= ~Property_Image)))
                    goto renderContent_done;
            }

            if(contentMask & Property_Text) {
                if(flags & Render_Comment)
                    graphics->comment("RenderText: " + mText->describe());
                mText->render(graphics, contentRect);
                if(NRDP_LIKELY(!(contentMask &= ~Property_Text)))
                    goto renderContent_done;
            }
        }
    }

renderContent_done:
    if(NRDP_UNLIKELY(Debug::DrawWidgetBlendMode)) {
        const float oldOpacity = graphics->opacity();
        bool blended;
        if((mContentMask & Property_Erase) || blendMode == GraphicsEngine::CompositionMode_Source) {
            blended = false;
        } else {
            blended = !gibbon_float_compare(oldOpacity, 1.0);
            if(!blended && (mContentMask & (Property_BackgroundImage|Property_Image))) {
                for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                    if((*it)->hasContent() && (*it)->hasAlpha()) {
                        blended = true;
                        break;
                    }
                }
            }
            if(!blended && (mContentMask & (Property_BackgroundColor)))
                blended = mBackgroundColor.a != 255;
        }
        graphics->setOpacity(1.0f);
        graphics->setClip(globalRect);
        graphics->setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
        if(blended) {
            graphics->fill(globalRect, Color(255, 0, 0, 80));
            graphics->drawRect(globalRect, Color(255, 0, 0, 160));
        } else {
            graphics->fill(globalRect, Color(0, 255, 0, 50));
            graphics->drawRect(globalRect, Color(0, 255, 0, 100));
        }
        graphics->setOpacity(oldOpacity);
    }
}

void Widget::renderEffect(GraphicsEngine *graphics, const Rect &rect)
{
    assert(MutexStack::locked(Widget::lock().name()));
    assert(mEffects.rendered);
    graphics->setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
    const Rect srcRect(0, 0, mEffects.rect.width, mEffects.rect.height),
        dstRect(rect);
    graphics->apply(mEffects.rendered, srcRect, dstRect);
}

void Widget::renderChildren(GraphicsEngine *graphics, const Point &origin, const Rect &clipRect, uint8_t flags) const
{
    if(mFlags & WidgetFlag_DirtyDrawOrder)
        updateDrawOrder();
    for(ChildContainer::iterator it = mChildren.drawOrder.begin(); it != mChildren.drawOrder.end(); ++it)
        (*it)->render(graphics, origin, clipRect, flags);
}

void Widget::onImageLoaded(const Image::SharedPtr &image, bool)
{
    //PERFORMANCE_MARKER_SCOPED(("widget.onimageloaded " + describe()).c_str(), false);
    PERFORMANCE_MARKER_SCOPED("widget.onimageloaded");
    assert(MutexStack::locked(Widget::lock().name()));
    if(!isPurging() && getVisible() && image->getVisible()) {
        if(image->getType() == Image::Type_Foreground) {
            parentNeedsRender();
            needsRect();
        }
        needsRender();
    }
}

bool Widget::isCoordinateProperty(Property property)
{
    const WidgetProperty *widgetProperty = WidgetProperty::findProperty((Widget::Property)property);
    assert(widgetProperty);
    if(widgetProperty)
        return (widgetProperty->flags & WidgetProperty::Coordinate);
    return false;
}

bool Widget::isIntegralProperty(int property) const
{
    const WidgetProperty *widgetProperty = WidgetProperty::findProperty((Widget::Property)property);
    assert(widgetProperty);
    if(widgetProperty) {
        if(Debug::TestAnimationCoordIsIntegral && (widgetProperty->flags & WidgetProperty::Coordinate))
            return true;
        return widgetProperty->flags & WidgetProperty::Integral;
    }
    return false;
}

bool Widget::isAnimatedProperty(Property property)
{
    const WidgetProperty *widgetProperty = WidgetProperty::findProperty(property);
    assert(widgetProperty);
    if(widgetProperty)
        return widgetProperty->flags & WidgetProperty::Animated;
    return false;
}

float Widget::getAnimatedProperty(int property) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    assert(isAnimatedProperty((Property)property));
    const Rect& r = getRect();
    switch(property) {
    case Property_ScrollX:
        return mScroll.origin.x;
    case Property_ScrollY:
        return mScroll.origin.y;
    case Property_X:
        return r.x != CoordinateMax ? r.x : getTransformedRenderRect().x;
    case Property_Y:
        return r.y != CoordinateMax ? r.y : getTransformedRenderRect().y;
    case Property_Width:
        return r.width != CoordinateMax ? r.width : getTransformedRenderRect().width;
    case Property_Height:
        return r.height != CoordinateMax ? r.height : getTransformedRenderRect().height;
    case Property_Scale:
        return getScale();
    case Property_Opacity:
        return getOpacity();
    default:
        break;
    }
    return 0;
}

unsigned int Widget::getContentMask() const
{
    unsigned ret = 0;
    const Rect renderRect = getTransformedRenderRect();
    if(renderRect.empty()) {
        //the image is yet to be loaded, therefore we don't know our size yet
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if((*it)->getType() == Image::Type_Foreground && (*it)->hasContent() &&
               ((*it)->loadFlags() & SurfaceLoader::Load_LazyMask) && !(*it)->getSurface()) {
                ret |= Property_Image;
                break;
            }
        }
        return ret;
    }
    if(NRDP_UNLIKELY(hasClone()))
        ret |= Property_Clone;
    if(NRDP_UNLIKELY(getErase()))
        ret |= Property_Erase;
    if(NRDP_UNLIKELY(hasVideo()))
        ret |= Property_Video;
    if(hasText() && mText->hasContent())
        ret |= Property_Text;

    Rect backgroundImageRect;
    bool hasBackgroundImage = false;
    bool hasOpaqueImage = false, hasOpaqueBackgroundImage = false;
    for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
        if((*it)->hasContent()) {
            if((*it)->getType() == Image::Type_Background) {
                hasBackgroundImage = true;
                backgroundImageRect |= (*it)->getDestinationRect(renderRect);
                if(!(*it)->hasAlpha())
                    hasOpaqueBackgroundImage = true;
            } else if((*it)->getType() == Image::Type_Foreground) {
                ret |= Property_Image;
                if(!(*it)->hasAlpha())
                    hasOpaqueImage = true;
            }
        }
    }

    bool backgroundColor = hasBackgroundColor();
    if(!hasBackgroundImage) {
        if(backgroundColor) {
            if(!hasOpaqueImage || hasPadding()) {
                ret |= Property_BackgroundColor;
            } else {
                for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
                    if((*it)->getType() == Image::Type_Foreground && (*it)->hasContent()) {
                        const Rect imageRect = (*it)->getDestinationRect(renderRect);
                        if(imageRect == renderRect) {
                            backgroundColor = false;
                            break;
                        }
                    }
                }
            }
        }
    } else if(hasOpaqueImage) {
        const Rect contentsRect(renderRect.x + mPadding.left, renderRect.y + mPadding.top,
                                renderRect.width + - (mPadding.left + mPadding.right),
                                renderRect.height + - (mPadding.top + mPadding.bottom));
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if((*it)->getType() != Image::Type_Foreground)
                continue;
            const Rect imageRect = (*it)->getDestinationRect(contentsRect);
            if(!imageRect.contains(backgroundImageRect)) {
                ret |= Property_BackgroundImage;
                if(backgroundColor && hasOpaqueBackgroundImage && backgroundImageRect != renderRect)
                    backgroundColor = false;
                break;
            } else if(backgroundColor && imageRect == renderRect) {
                backgroundColor = false;
            }
        }
    } else {
        ret |= Property_BackgroundImage;
        if(backgroundColor && hasOpaqueBackgroundImage && backgroundImageRect == renderRect)
            backgroundColor = false;
    }
    if(backgroundColor)
        ret |= Property_BackgroundColor;
    if(hasLayout())
        ret |= Property_Layout;
    return ret;
}

void Widget::startAnimatingProperty(int property, const AnimationStart &start)
{
    assert(MutexStack::locked(Widget::lock().name()));
    assert(isAnimatedProperty((Property)property));
    if(NRDP_UNLIKELY(Debug::InstrumentWidgetAnimations))
        INSTRUMENTATION_INTERVAL_SWITCHED_START(StringFormatter::sformat("gibbon.widget.animation.%d.%s", getID(), getAnimatedPropertyName(property).c_str()),
                                                Variant::StringMap("from", start.frame.startValue)("to", start.frame.endValue)("duration", start.frame.info.duration));
    if(property == Property_ScrollX)
        reload(Point(start.frame.startValue, 0));
    else if(property == Property_ScrollY)
        reload(Point(0, start.frame.endValue));
    mAnimatingProperties |= property;
}

void Widget::endAnimatingProperty(int property, const AnimationFinish &finish)
{
    assert(MutexStack::locked(Widget::lock().name()));
    assert(isAnimatedProperty((Property)property));
    if(NRDP_UNLIKELY(Debug::InstrumentWidgetAnimations))
        INSTRUMENTATION_INTERVAL_SWITCHED_END(StringFormatter::sformat("gibbon.widget.animation.%d.%s", getID(), getAnimatedPropertyName(property).c_str()),
                                                Variant::StringMap("fps", finish.fps));
    mAnimatingProperties &= ~property;
    for(Widget::ListenerContainer::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
        (*it)->onAnimationFinished((Property)property, finish);
}

std::string Widget::getAnimatedPropertyName(int property) const
{
    const WidgetProperty *widgetProperty = WidgetProperty::findProperty((Widget::Property)property);
    assert(widgetProperty);
    if(widgetProperty)
        return widgetProperty->name;
    return std::string();
}

Widget::Property Widget::convertPropertyName(const std::string &property)
{
    const WidgetProperty *widgetProperty = WidgetProperty::findProperty(property);
    assert(widgetProperty);
    if(widgetProperty)
        return widgetProperty->property;
    return Property_Unknown;
}

bool Widget::setAnimatedProperty(int property, float value)
{
    assert(isAnimatedProperty((Property)property));
    assert(MutexStack::locked(Widget::lock().name()));

    if(NRDP_UNLIKELY(Debug::InstrumentWidgetAnimations))
        INSTRUMENTATION_INTERVAL_SWITCHED_MARK(StringFormatter::sformat("gibbon.widget.animation.%d.%s", getID(), getAnimatedPropertyName(property).c_str()), "value", value);
    switch(property) {
    case Property_X:
        setX(isnan(value) ? CoordinateMax : GIBBON_ROUND(value));
        return true;
    case Property_Y:
        setY(isnan(value) ? CoordinateMax : GIBBON_ROUND(value));
        return true;
    case Property_Width:
        setWidth(isnan(value) ? CoordinateMax : GIBBON_ROUND(value));
        return true;
    case Property_Height:
        setHeight(isnan(value) ? CoordinateMax : GIBBON_ROUND(value));
        return true;
    case Property_Scale:
        setScale(isnan(value) ? 1.0 : value);
        return true;
    case Property_Opacity:
        setOpacity(isnan(value) ? 1.0 : value);
        return true;
    case Property_ScrollX:
        setScrollX(isnan(value) ? 0 : value);
        return true;
    case Property_ScrollY:
        setScrollY(isnan(value) ? 0 : value);
        return true;
    default:
        break;
    };
    return false;
}

void Widget::renderPropertiesChanged(unsigned int properties) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    properties &= ~mAnimatingProperties;
    if(properties) {
        for(Widget::ListenerContainer::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
            (*it)->onRenderPropertiesChanged(properties);
    }
}

void Widget::setLayoutSpacing(int16_t spacing)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mLayoutSpacing != spacing) {
        parentNeedsRender();
        parentNeedsRect();
        mLayoutSpacing = spacing;
        needsRect();
        needsRender();
    }
}

void Widget::setPadding(int16_t top, int16_t left, int16_t bottom, int16_t right, int16_t layoutWrap)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mPadding.top != top || mPadding.left != left || mPadding.bottom != bottom ||
       mPadding.right != right || mPadding.layoutWrap != layoutWrap) {
        parentNeedsRender();
        mPadding.top = top;
        mPadding.left = left;
        mPadding.bottom = bottom;
        mPadding.right = right;
        mPadding.layoutWrap = layoutWrap;
        needsLayout();
        needsRect();
        needsRender();
    }
}

void Widget::setLayoutAlign(uint8_t align)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mLayout.align != align) {
        mLayout.align = align;
        needsLayout();
        needsRect();
        needsRender();
    }
}

void Widget::setLayoutStretch(int16_t stretch)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mLayout.stretch != stretch) {
        parentNeedsRender();
        mLayout.stretch = stretch;
        needsLayout();
        needsRect();
        needsRender();
    }
}

void Widget::setLayout(Layout direction)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mLayout.direction != direction) {
        parentNeedsRender();
        mLayout.dirtied = 0;
        mLayout.direction = direction;
        if(direction == Layout_None) {
            for(ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it)
                (*it)->unsetLayoutRect();
        } else {
            needsLayout();
        }
        needsRect();
        needsRender();
    }
}

void Widget::setRenderCache(RenderCache flags)
{
    assert(MutexStack::locked(Widget::lock().name()));
    mCache.flags = flags;
    if(mCache.flags == RenderCache_Disabled)
        needsRender();
}

bool Widget::shouldRenderCache() const
{
    if(!getClipEnabled())
        return false;
    if(mCache.flags == RenderCache_Enabled)
        return true;
    if(!mClonedTo.empty())
        return true;
    if(!gibbon_float_compare(mTransform.scale, 1.0f) && hasContent())
        return true;
    return false;
}

unsigned int Widget::purgeSurfaces(PurgeType type)
{
    if(isRendering())
        return 0;

    FlagScope _scope(this, WidgetFlag_Purging);
    unsigned int released = 0;
    for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
        if((type == Purge_Force || (*it)->canPurge()) && (*it)->getSurface())
            released += (*it)->purge();
    }
    if(type != Purge_Preemptive) {
        if(mCache.surface) {
            DEBUG_WIDGET_CACHE("** CACHE_RESET%d(%s) %s", __LINE__, describe().c_str(), mCache.surface->describe().c_str());
            released += mCache.surface->calculateSize();
            mCache.surface.reset();
        }
        mEffects.rendered.reset();
    }
    if(mText)
        released += mText->purge();
    for(Effects::const_iterator it = mEffects.effects.begin(); it != mEffects.effects.end(); ++it)
        released += (*it)->purge();
    for(EffectRenderers::const_iterator it = mEffects.renderers.begin(); it != mEffects.renderers.end(); ++it)
        released += (*it)->purge();
    return released;
}

void Widget::unsuspend()
{
    purgeSurfaces(Purge_Force);
    needsRect();
    reloadImages();
    for(ChildContainer::iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
        Widget::SharedPtr child = (*it);
        child->unsuspend();
    }
}

Time Widget::lastPurged()
{
    ScopedMutex _lock(Widget::lock());
    return Time::mono() - sLastPurgeTime;
}

unsigned int Widget::purgeAll(PurgeType type)
{
    unsigned int result = 0;
    ScopedMutex _locker(Widget::lock());
    sLastPurgeTime = Time::mono();
    const std::list<Widget::SharedPtr> widgets = gibbonApp()->getRootWidgets();
    for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it)
        result += (*it)->purge(type);
    return result;
}

void Widget::purgeText()
{
    ScopedMutex _locker(Widget::lock());
    const std::list<Widget::SharedPtr> widgets = gibbonApp()->getRootWidgets();
    for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it)
        (*it)->purgeText_internal();
}

void Widget::purgeText_internal()
{
    if (mText)
        mText->purge();

    for(ChildContainer::iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it)
        (*it)->purgeText_internal();
}

unsigned int Widget::purge(PurgeType type)
{
    PERFORMANCE_MARKER_SCOPED("widget.purge");
    ScopedMutex _lock(Widget::lock());
    Rect globalRect;
    if(!isRoot())
        type = Purge_Force;
    else if(getClipEnabled())
        globalRect = getGlobalRect();
    else
        globalRect = gibbonApp()->getScreen()->getRootRect();
    return purge_internal(globalRect, Point(0, 0), type);
}

unsigned int Widget::purge_internal(Rect rect, Point offset, PurgeType type)
{
    if(isRendering())
        return 0;

    bool purgable = true;
    if(type != Purge_Force && !rect.empty()) {
        const Rect clipRect(getTransformedClipRect());
        const Rect globalClipRect(clipRect.x + offset.x, clipRect.y + offset.y, clipRect.width, clipRect.height);
        purgable = !getVisible() ||
                   (gibbon_float_is_zero(getOpacity()) && !(mAnimatingProperties & Property_Opacity)) ||
                   !rect.overlaps(globalClipRect);
        if(purgable) {
            rect.clear();
        } else if(hasChildren()) {
            if(getClipEnabled())
                rect = rect.intersected(globalClipRect);
            const Rect renderRect(getTransformedRenderRect());
            offset.x += renderRect.x + mScroll.origin.x;
            offset.y += renderRect.y + mScroll.origin.y;
        }
    }

    unsigned int result = 0;
    if(purgable) {
        const int purged = purgeSurfaces(type);
        result += purged;
    }
    for(ChildContainer::iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
        Widget::SharedPtr child = (*it);
        const unsigned int released = child->purge_internal(rect, offset, type);
        result += released;
    }
    return result;
}

bool Widget::isOnScreen() const
{
    for(shared_ptr<const Widget> w = sharedPtr(); w; w = w->getParent()) {
        if(!w->getVisible() || !w->getOpacity())
            return false;
    }
    return GibbonApplication::instance()->getScreen()->getRootRect().overlaps(getGlobalRect());
}

void Widget::dump(bool recursive)
{
    Variant result = dump_internal(0, recursive);
    Log::warn(TRACE_GIBBON_WIDGET, "%s:\n%s", describe().c_str(), result.toJSON(true).c_str());
}

Variant Widget::dump_internal(int i, bool recursive)
{
    Variant result;
    char buffer[1024];
    result["rect"] = mRect.toString();
    {
        snprintf(buffer, 1024, "%p", this);
        result["pointer"] = buffer;
    }
    if(!mName.empty())
        result["name"] = mName;
    {
        result["global_rect"] = getGlobalRect().toString();
        result["content"] = hasContent();
        if(hasText()) {
            Variant text;
            text["string"] = mText->getText();
            result["text"] = text;
        }
        if(getClipEnabled())
            result["clip"] = true;

        for(Effects::const_iterator it = mEffects.effects.begin(); it != mEffects.effects.end(); ++it) {
            Variant effect;
            effect["type"] = (*it)->getType();
            effect["visible"] = (*it)->getVisible();
            effect["params"] = (*it)->getParams();
            result["effects"].push_back(effect);
        }

        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if((*it)->hasUrl()) {
                Variant image;
                image["background"] = (*it)->getType() == Image::Type_Background;
                image["visible"] = (*it)->getVisible();
                image["url"] = (*it)->getUrl().str();
                snprintf(buffer, 1024, "%p", (*it)->getSurface().get());
                image["surface"] = buffer;
                result["images"].push_back(image);
            }
        }
        if(hasVideo())
            result["video"] = true;
        if(getErase())
            result["erase"] = true;
        if(getOpaque())
            result["opaque"] = true;
        if(hasBackgroundColor()) {
            snprintf(buffer, 1024, "%d,%d,%d,%d", mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
            result["background"] = buffer;
        }
        if(shouldRenderCache()) {
            snprintf(buffer, 1024, "%p", mCache.surface.get());
            result["cache"]["surface"] = buffer;
            result["cache"]["rect"] = mCache.rect.toString();
        }
    }
    if(recursive) {
        updateDrawOrder();
        for(ChildContainer::iterator it = mChildren.drawOrder.begin(); it != mChildren.drawOrder.end(); ++it) {
            Widget::SharedPtr child = (*it);
            snprintf(buffer, 1024, "%d", child->getID());
            result["children"][buffer] = child->dump_internal(i+1, false);
        }
    }
    return result;
}

const std::vector<Widget::BoxLayout::WidgetBox> &Widget::BoxLayout::layout(const Size &contentSize, uint8_t flags)
{
    const bool debugLayout = flags & Flag_Debug;
    const int debugIndent = (flags & Flag_SizeContent) ? sSizeContentIndent : 0;

    float used = mBoxesSpacing;
    Widget::Layout direction = mWidget->mLayout.direction;
    if(debugLayout) {
        const char *description = "u";
        if(direction == Widget::Layout_Horizontal)
            description = "h";
        else if(direction == Widget::Layout_Vertical)
            description = "v";
        else if(direction == Widget::Layout_HorizontalWrapping)
            description = "w";
        DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent) << "LAYOUT(" << mWidget->describe() << ") "
                              << description << "(" << direction << ") " << prettySize(contentSize);
    }

    //step one: establish suggested sizes with stretch factors
    int stretches = 0, flexibles = 0;
    for(int pass = 0; pass < 2; ++pass) {
        for(size_t i = 0; i < mBoxes.size(); ++i) {
            WidgetBox &box = mBoxes[i];
            if(box.width != CoordinateMax && box.height != CoordinateMax)
                continue;

            Widget::SharedPtr wc = box.widget;
            const Rect &wRect = wc->getRect();

            if(pass == 0 && ((direction != Layout_Vertical && mWidget->mRect.width != CoordinateMax && wRect.width == CoordinateMax) ||
                             ((direction == Layout_Vertical && mWidget->mRect.height != CoordinateMax && wRect.height == CoordinateMax)))) {
                ++flexibles;
                continue;
            } else if(wc->isFlexible()) {
                for(Images::const_iterator it = wc->mImages.begin(); it != wc->mImages.end(); ++it) {
                    Image::SharedPtr image = (*it);
                    if(image->hasContent() && !image->getSurface())
                        image->reload();
                }
                Coordinate maxWidth = wRect.width, maxHeight = wRect.height;
                if(maxWidth == CoordinateMax) { // Fill the available space
                    maxWidth = contentSize.width;
                    if(direction != Layout_Vertical && maxWidth != CoordinateMax) {
                        maxWidth -= used;
                        if(flexibles)
                            maxWidth /= flexibles;
                    }
                }
                if(maxHeight == CoordinateMax) { // Fill the available space
                    maxHeight = contentSize.height;
                    if(direction == Layout_Vertical && maxHeight != CoordinateMax) {
                        maxHeight -= used;
                        if(flexibles) {
                            const Coordinate shared = maxHeight / flexibles;
                            if(shared > contentSize.height)
                                maxHeight = shared;
                        }
                    }
                }
                const Size sc = wc->sizeContent(maxWidth, maxHeight);
                box.width = (wRect.width == CoordinateMax ? sc.width : wRect.width);
                box.height = (wRect.height == CoordinateMax ? sc.height : wRect.height);
                if(pass == 1)
                    --flexibles;
            } else {
                box.width = wRect.width;
                box.height = wRect.height;
            }
            wc->boundSize(&box.width, &box.height);
            if(!gibbon_float_compare(wc->mTransform.scale, 1.0f)) {
                box.width *= wc->mTransform.scale;
                box.height *= wc->mTransform.scale;
            }

            if(direction == Layout_Vertical) {
                if(wRect.height == CoordinateMax && direction != Layout_HorizontalWrapping &&
                   (wc->mMaxSize.height == CoordinateMax || box.height < wc->mMaxSize.height))
                    box.stretch = wc->mLayout.stretch;
                if(box.height > 0) {
                    if(box.stretch) {
                        if(wRect.height == CoordinateMax)
                            box.surplus = box.height;
                        else if(box.height > wRect.height)
                            box.surplus = box.height - wRect.height;
                    }
                    used += box.height;
                }
            } else {
                if(wRect.width == CoordinateMax && direction != Layout_HorizontalWrapping &&
                   (wc->mMaxSize.width == CoordinateMax || box.width < wc->mMaxSize.width)) {
                    box.stretch = wc->mLayout.stretch;
                }
                if(box.width > 0) {
                    if(box.stretch) {
                        if(wRect.width == CoordinateMax)
                            box.surplus = box.width;
                        else if(box.width > wRect.width)
                            box.surplus = box.width - wRect.width;
                    }
                    used += box.width;
                }
            }
            if(box.stretch)
                stretches += box.stretch;
        }
        if(!flexibles)
            break;
    }
    if(debugLayout) {
        for(size_t i = 0; i < mBoxes.size(); ++i) {
            WidgetBox &box = mBoxes[i];
            Widget::SharedPtr wc = box.widget;
            DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent)  << "  "
                                  << box.widget->describe() << " is " << box.width << "x" << box.height << " (" << box.stretch << "/" << box.surplus << "/" << wc->mLayoutSpacing << ")";
        }
    }

    //step two: apply the stretch factor and use up all available space
    float remaining = 0;
    if(!(flags & Flag_SizeContent)) {
        if(direction == Layout_Vertical) {
            if(contentSize.height != CoordinateMax)
                remaining = contentSize.height - used;
        } else {
            if(contentSize.width != CoordinateMax)
                remaining = contentSize.width - used;
        }
        DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent) << "  PROCESS STRETCHES(" << mWidget->describe() << ") " << used << "/" << remaining;
        while(remaining > 0 && stretches) {
            float u = 0;
            bool done = true;
            const float factor = float(remaining) / stretches;
            DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent) << "  STRETCHING(" << mWidget->describe() << ") " << used << "/" << remaining << " " << stretches << "(" << factor << ")";
            float grow;
            for(size_t i = 0; i < mBoxes.size(); ++i) {
                WidgetBox &box = mBoxes[i];
                grow = 0;
                if(box.stretch) {
                    if(stretches == 1) {
                        grow = remaining-u;
                        done = false;
                    } else {
                        grow = box.stretch * factor;
                        if(fabsf(grow) > 0.00001f)
                            done = false;
                        if(box.surplus) {
                            if(box.surplus > grow) {
                                box.surplus -= grow;
                                grow = 0;
                            } else {
                                grow -= box.surplus;
                                box.surplus = 0;
                            }
                        }
                    }
                    if(grow) {
                        Widget::SharedPtr wc = box.widget;
                        if(direction == Layout_Vertical) {
                            if(wc->mMaxSize.height != CoordinateMax && box.height+grow >= wc->mMaxSize.height) {
                                grow = wc->mMaxSize.height-box.height;
                                stretches -= box.stretch;
                                box.stretch = 0;
                                box.surplus = 0;
                            }
                            box.height += grow;
                        } else {
                            if(wc->mMaxSize.width != CoordinateMax && box.width+grow >= wc->mMaxSize.width) {
                                grow = wc->mMaxSize.width-box.width;
                                stretches -= box.stretch;
                                box.stretch = 0;
                                box.surplus = 0;
                            }
                            box.width += grow;
                        }
                        u += grow;
                    }
                }
                DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent)
                                      << "    STRETCHED(" << box.widget->describe() << ") " << box.width << "x" << box.height
                                      << " (" << box.stretch << "/" << box.surplus << ") (" << grow << ")";
            }
            if(done)
                break;
            used += u;
            remaining -= u;
        }
    }

    //step three: handle top left placement
    int start_off = 0, start_line = 0;
    if(remaining) {
        if(direction == Layout_Vertical) {
            if(mWidget->mLayout.align & Style::ALIGN_CENTER_VERTICAL)
                start_off = int(start_off + remaining / 2);
            else if(mWidget->mLayout.align & Style::ALIGN_BOTTOM)
                start_off = int(start_off + remaining);
        } else {
            if(direction == Layout_Horizontal) {
                if(mWidget->mLayout.align & Style::ALIGN_CENTER_HORIZONTAL)
                    start_off = int(start_off + (remaining / 2));
                else if(mWidget->mLayout.align & Style::ALIGN_RIGHT)
                    start_off += int(start_off + remaining);
            }
        }
    }
    float rounding = 0;
    int offset = start_off, line = start_line, maxLine = -1;
    float max = 0.0f;
    for(size_t i = 0; i < mBoxes.size(); ++i) {
        WidgetBox &box = mBoxes[i];
        Widget::SharedPtr wc = box.widget;
        const int spacing = (!i) ? 0 : wc->mLayoutSpacing;
        const Rect &wRect = wc->getRect();
        offset += spacing;
        if(direction == Layout_Vertical) {
            box.x = line;
            box.y = offset;
            {
                const float advance = floor(box.height);
                rounding += box.height - advance;
                offset += int(advance);
            }
            if(!(flags & Flag_SizeContent) && wRect.width == CoordinateMax && contentSize.width != CoordinateMax && box.width < contentSize.width) {
                box.width = contentSize.width;
                wc->boundSize(&box.width, (float*)0);
            }
            if(box.width > max)
                max = ceil(box.width);
        } else {
            box.x = offset;
            box.y = line;
            if(direction == Layout_HorizontalWrapping) {
                if(offset+int(box.width) > contentSize.width) { //wrap
                    if(offset-spacing > maxLine)
                        maxLine = offset-spacing;
                    line += (int)max + mWidget->mPadding.layoutWrap;
                    offset = 0;
                    if(!(flags & Flag_SizeContent) && (mWidget->mLayout.align & Style::ALIGN_RIGHT))
                        box.x = int(contentSize.width - offset);
                    else
                        box.x = offset;
                    box.y = line;
                    max = ceil(box.height);
                }
            } else if(!(flags & Flag_SizeContent) && wRect.height == CoordinateMax && contentSize.height != CoordinateMax && box.height < contentSize.height) {
                box.height = contentSize.height;
                wc->boundSize((float*)0, &box.height);
            }
            {
                const float advance = floor(box.width);
                rounding += box.width - advance;
                offset += int(advance);
            }
            if(box.height > max)
                max = ceil(box.height);
        }
    }
    if(!(flags & Flag_SizeContent) && direction != Layout_HorizontalWrapping && remaining < 1.0f && rounding) { //give away the round
        if(const int r = (int)round(rounding)) {
            offset += r;
            Widget::SharedPtr widget;
            for(size_t i = 0; i < mBoxes.size(); ++i) {
                WidgetBox &box = mBoxes[i];
                if(widget) {
                    if(direction == Layout_Vertical)
                        box.y += r;
                    else
                        box.x += r;
                } else {
                    Widget::SharedPtr wc = box.widget;
                    if(direction == Layout_Vertical) {
                        if(wc->mMaxSize.height == CoordinateMax || box.height < wc->mMaxSize.height) {
                            widget = wc;
                            box.height += r;
                        }
                    } else if(wc->mMaxSize.width == CoordinateMax || box.width < wc->mMaxSize.width) {
                        widget = wc;
                        box.width += r;
                    }
                }
            }
            DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent)
                                  << "  ROUND(" << mWidget->describe() << ") [" << (widget ? widget->describe() : std::string("N/A")) << "] (" << rounding << ")";
        }
    }
    if(direction != Layout_Vertical && maxLine < offset)
        maxLine = offset;
    if(!(flags & Flag_SizeContent) && contentSize.height != CoordinateMax)
        mSize.height = contentSize.height;
    else if(direction == Layout_Vertical)
        mSize.height = offset;
    else
        mSize.height = line + max;
    if(!(flags & Flag_SizeContent) && contentSize.width != CoordinateMax)
        mSize.width = contentSize.width;
    else if(direction == Layout_Vertical)
        mSize.width = line + max;
    else
        mSize.width = maxLine;
    DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent) << "  PLACING(" << mWidget->describe() << ") " << used << "/" << remaining << " " << prettySize(mSize);
    for(size_t i = 0; i < mBoxes.size(); ++i) {
        WidgetBox &box = mBoxes[i];
        Widget::SharedPtr wc = box.widget;
        if(direction == Layout_Vertical) {
            if(wc->getWidth() == CoordinateMax && box.width > mSize.width)
                box.width = mSize.width;
        } else {
            if(wc->getHeight() == CoordinateMax && box.height > mSize.height)
                box.height = mSize.height;
        }
        if(debugLayout) {
            const Rect wLayoutRect = box.rect();
            DEBUG_WIDGET_LAYOUT() << gibbon_indent(debugIndent)
                                  << "    PLACED(" << wc->describe() << ") " << wLayoutRect.toString() << "[" << wLayoutRect.right() << "x"
                                  << wLayoutRect.bottom() << "] (" << box.oldRenderRect.toString() << "[" << box.oldRenderRect.right() << "x"
                                  << box.oldRenderRect.bottom() << ")";
        }
    }
    return mBoxes;
}

void Widget::layout(bool force) const
{
    if(force || isLayoutDirty() || (mChildren.needsLayout && getVisible())) {
        INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.layout");
        const int updated = updateLayout(force, Size(CoordinateMax, CoordinateMax));
        INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.layout", Variant::StringMap("updated", updated));
    }
}

int Widget::updateLayout(bool force, const Size &laidSize) const
{
    assert(MutexStack::locked(Widget::lock().name()));

    const int content = hasContent();
    const bool dirtyLayout = force || isLayoutDirty();
    bool dirtyKidsLayout = force || mChildren.needsLayout;
    const bool debugLayout = Debug::DebugWidgetLayout;

    if(dirtyLayout) {
        if (isRoot())
            DEBUG_WIDGET_LAYOUT() << "***************** LAYOUT";
        mFlags &= ~WidgetFlag_DirtyLayout;
    }

    int result = 0;
    if(content) {
        Widget *that = const_cast<Widget*>(this);
        FlagScope _scope(that, WidgetFlag_LayingOut);
        if(hasLayout() && dirtyLayout) {
            const Rect oldRenderRect = mRenderRect;
            Size contentSize = laidSize;
            if(contentSize.width == CoordinateMax)
                contentSize.width = mRect.width;
            if(contentSize.height == CoordinateMax)
                contentSize.height = mRect.height;
            if(contentSize.width == CoordinateMax || contentSize.height == CoordinateMax) {
                const Size sc = sizeContent(contentSize.width);
                if(contentSize.width == CoordinateMax)
                    contentSize.width = sc.width;
                if(contentSize.height == CoordinateMax)
                    contentSize.height = sc.height;
            }

            BoxLayout layout(this);
            layout.reserve(mChildren.order.size());

            //step one: accumulate the widgets
            for(ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
                Widget::SharedPtr wc = *it;
                if(wc->shouldLayout(mLayout.direction))
                    layout.addWidget(wc);
                else //not included in my layout
                    wc->unsetLayoutRect();
            }

            //step two: apply layout
            const std::vector<BoxLayout::WidgetBox> &boxes = layout.layout(Size(contentSize.width - (mPadding.left+mPadding.right),
                                                                                contentSize.height - (mPadding.top+mPadding.bottom)),
                                                                           debugLayout ? BoxLayout::Flag_Debug : 0);

            //step three: place all the widgets according to their sizes
            int changed = 0;
            for(size_t i = 0; i < boxes.size(); ++i) {
                const BoxLayout::WidgetBox &box = boxes[i];
                Widget::SharedPtr wc = box.widget;
                Rect wLayoutRect(box.rect());
                wLayoutRect.x += mPadding.left;
                wLayoutRect.y += mPadding.top;
                if(wc->setLayoutRect(wLayoutRect))
                    dirtyKidsLayout = true;
                if(box.oldRenderRect != wLayoutRect) {
                    unsigned int wProperties = 0;
                    if(box.oldRenderRect.x != wLayoutRect.x)
                        wProperties |= Property_RenderX;
                    if(box.oldRenderRect.y != wLayoutRect.y)
                        wProperties |= Property_RenderY;
                    if(box.oldRenderRect.width != wLayoutRect.width)
                        wProperties |= Property_RenderWidth;
                    if(box.oldRenderRect.height != wLayoutRect.height)
                        wProperties |= Property_RenderHeight;
                    if(wProperties) {
                        wc->renderPropertiesChanged(wProperties);
                        {
                            const unsigned int previousContentMask = wc->mContentMask;
                            wc->mContentMask = wc->getContentMask();
                            if(static_cast<bool>(previousContentMask) != static_cast<bool>(wc->mContentMask)) {
                                const int contentDelta = wc->mContentMask ? 1 : -1;
                                for(Widget::SharedPtr p = that->sharedPtr(); p; p = p->getParent())
                                    p->mChildren.hasContent += contentDelta;
                            }
                        }
                        wc->needsRender();
                        wc->updateContentMask();
                        ++changed;
                    }
                }
            }

            //step four: size self to enclose the layout
            Rect layoutRect = mRect;
            if(layoutRect.x == CoordinateMax || layoutRect.y == CoordinateMax) {
                const Rect &r = getTransformedRenderRect();
                if(layoutRect.x == CoordinateMax)
                    layoutRect.x = r.x;
                if(layoutRect.y == CoordinateMax)
                    layoutRect.y = r.y;
            }
            if(layoutRect.width == CoordinateMax || layoutRect.height == CoordinateMax) {
                const Size layoutSize(layout.size());
                if(layoutRect.height == CoordinateMax)
                    layoutRect.height = layoutSize.height+(mPadding.top+mPadding.bottom);
                if(layoutRect.width == CoordinateMax)
                    layoutRect.width = layoutSize.width+(mPadding.left+mPadding.right);
                if(contentSize.width > layoutRect.width)
                    layoutRect.width = contentSize.width;
                if(contentSize.height > layoutRect.height)
                    layoutRect.height = contentSize.height;
                boundSize(&layoutRect.width, &layoutRect.height);
            }

            if(gibbon_float_compare(mTransform.scale, 1.0f))
                layoutRect.scale(mTransform.scale, mTransform.scale);
            DEBUG_WIDGET_LAYOUT() << "  SIZING(" << describe() << ") " << layoutRect.width << "x" << layoutRect.height << "(" << oldRenderRect << ")";

            if(layoutRect != oldRenderRect) {
                unsigned int properties = 0;
                if(mLayoutRect.x != layoutRect.x)
                    properties |= Property_RenderX;
                if(mLayoutRect.y != layoutRect.y)
                    properties |= Property_RenderY;
                if(mLayoutRect.width != layoutRect.width)
                    properties |= Property_RenderWidth;
                if(mLayoutRect.height != layoutRect.height)
                    properties |= Property_RenderHeight;
                if(properties) {
                    renderPropertiesChanged(properties);
                    that->mFlags |= WidgetFlag_DirtyRect;
                    that->mLayoutRect = layoutRect;
                }
                if(Widget::SharedPtr parent = getParent()) {
                    if(getVisible() && !gibbon_float_is_zero(getOpacity())) {
                        if(!parent->hasLayout() && !parent->isInLayout() &&
                           (parent->isFlexible() || !parent->getClipEnabled()))
                            parent->needsRect();
                        parent->needsRender(oldRenderRect);
                    }
                }
                ++changed;
            }
            if(changed)
                that->needsRender();
            ++result;
            ++mLayout.dirtied;
            mFlags &= ~WidgetFlag_DirtyLayout;
        }

        //recurse
        if(dirtyKidsLayout && (force || getVisible())) {
            that->mChildren.needsLayout = 0;
            for(ChildContainer::const_iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
                Widget::SharedPtr child = (*it);
                if(!child->getVisible()) { //not ready
                    //DEBUG_WIDGET_LAYOUT() << "  SKIPPING(" << describe() << ")";
                } else {
                    result += child->updateLayout(force, (hasLayout() && child->isInLayout()) ? child->mLayoutRect.size() : Size(CoordinateMax, CoordinateMax));
                }
            }
        }
    }
    return result;
}

void Widget::onEffectChanged(const Effect::SharedPtr &, const Variant &)
{
    if(!isPurging()) {
        parentNeedsRender();
        needsRect(NeedsRect_ForceAll);
    }
    mEffects.rendered.reset();
    needsRender(false);
}

EffectRenderers Widget::updateEffectRenderers() const
{
    mEffects.renderers = Screen::createEffectRenderers(mEffects.effects);
    return mEffects.renderers;
}

class EffectRendererCommand : public DisplayList::Command
{
public:
    inline EffectRendererCommand(const Effect::Renderer::SharedPtr &effect,
                                 const DisplayList::SharedPtr &dl,
                                 const Rect &srcRect, const Rect &dstRect) : Command(Type_Unknown), mEffect(effect),
                                                                             mSrcDL(dl), mSrcRect(srcRect), mDstRect(dstRect) { }

    virtual std::string describe() const {
        std::ostringstream s;
        s << "EffectRendererCommand(" << mEffect->describe() << "): " << mSrcRect << "->" << mDstRect;
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        if(!mDstDL)
            mDstDL = mEffect->render(mSrcDL, mSrcRect);
        graphics->apply(mDstDL, mDstRect, Rect(mDstRect+Point(state->offsetX, state->offsetY)));
    }
    virtual void write(DisplayList::Writer &) const { }
    virtual void read(DisplayList::Reader &) { }

private:
    Effect::Renderer::SharedPtr mEffect;
    mutable DisplayList::SharedPtr mDstDL;
    DisplayList::SharedPtr mSrcDL;
    Rect mSrcRect, mDstRect;
};

DisplayList::SharedPtr Widget::applyEffects(DisplayList::SharedPtr dl, Rect &rect) const
{
    if(!hasEffects() || !dl->hasCommands())
        return dl;
    PERFORMANCE_MARKER_SCOPED("widget.applyeffects");

    const std::string base_description = dl->getDescription() + ":EFFECTS";
    std::string all_description = base_description;

    const EffectRenderers effects = updateEffectRenderers();
    for(EffectRenderers::const_iterator it = effects.begin(); it != effects.end(); ++it) {
        const Rect effectRect(0, 0, (*it)->getRect(rect).size());
        if(GibbonConfiguration::screenRenderDisplayList()) {
            DisplayList::SharedPtr effect_dl(new DisplayList(effectRect.width, effectRect.height));
            effect_dl->appendCommand(new EffectRendererCommand((*it), dl, rect, effectRect));
            dl = effect_dl;
        } else {
            INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.render.effect", Variant::StringMap("effect", (*it)->describe())("rect",Variant::Array(rect.x)(rect.y)(rect.width)(rect.height))("effectRect", Variant::Array(effectRect.x)(effectRect.y)(effectRect.width)(effectRect.height)));
            dl = (*it)->render(dl, rect);
            INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.render.effect");
        }
        const std::string description  = (*it)->describe();
        if(dl)
            dl->setDescription(base_description + ":" + description);
        all_description += ":" + description;
        rect = effectRect;
    }
    if(dl)
        dl->setDescription(all_description);
    return dl;
}

void Widget::addEffect(const Effect::SharedPtr &effect)
{
    assert(MutexStack::locked(Widget::lock().name()));
    parentNeedsRender();
    removeEffect(effect);
    effect->addListener(sharedPtr());
    mEffects.effects.push_back(effect);
    needsRect();
    needsRender();
}

void Widget::removeEffect(const Effect::SharedPtr &effect)
{
    assert(MutexStack::locked(Widget::lock().name()));
    parentNeedsRender();
    effect->removeListener(sharedPtr());
    for(Effects::iterator it = mEffects.effects.begin(); it != mEffects.effects.end(); ++it) {
        if((*it) == effect) {
            mEffects.effects.erase(it);
            break;
        }
    }
    needsRect(NeedsRect_ForceContent);
    needsRender();
}

void Widget::addImage(const Image::SharedPtr &image)
{
    assert(MutexStack::locked(Widget::lock().name()));
    parentNeedsRender();
    removeImage(image);
    mImages.push_back(image);
    image->addListener(sharedPtr());
    needsRect();
    needsRender();
}

void Widget::removeImage(const Image::SharedPtr &image)
{
    assert(MutexStack::locked(Widget::lock().name()));
    parentNeedsRender();
    image->removeListener(sharedPtr());
    for(Images::iterator it = mImages.begin(); it != mImages.end(); ++it) {
        if((*it) == image) {
            mImages.erase(it);
            break;
        }
    }
    needsRect();
    needsRender();
}

void Widget::setLoadImages(LoadImages v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mLoadImages != v) {
        mLoadImages = v;
        if(v)
            reload();
    }
}

int Widget::reloadImages() const
{
    int result = 0;
    if(!mImages.empty()) {
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            if(!(*it)->getSurface() && (*it)->hasContent()) {
                ++result;
                (*it)->reload();
            }
        }
    }
    return result;
}

unsigned int Widget::reload(const Point &origin)
{
    const Rect clipRect(getTransformedClipRect()), globalRect(getGlobalRect());
    return reload_internal((getClipEnabled() ? globalRect : GibbonApplication::instance()->getScreen()->getRootRect()),
                           globalRect.origin()-clipRect.origin(), origin);
}

unsigned int Widget::reload_internal(Rect rect, Point offset, Point childOffset)
{
    bool offscreen = true;
    unsigned int result = 0;
    if(!rect.empty()) {
        const Rect clipRect(getTransformedClipRect());
        const Rect globalClipRect(clipRect.x + offset.x, clipRect.y + offset.y, clipRect.width, clipRect.height);
        offscreen = !getVisible() ||
                    (gibbon_float_is_zero(getOpacity()) && !(mAnimatingProperties & Property_Opacity)) ||
                    !rect.overlaps(globalClipRect);
        if(offscreen) {
            rect.clear();
        } else if(hasChildren()) {
            if(getClipEnabled())
                rect = rect.intersected(globalClipRect);
            const Rect renderRect(getTransformedRenderRect());
            offset.x += renderRect.x + mScroll.origin.x + childOffset.x;
            offset.y += renderRect.y + mScroll.origin.y + childOffset.y;
        }
    }
    if(!offscreen)
        result += reloadImages();
    for(ChildContainer::iterator it = mChildren.order.begin(); it != mChildren.order.end(); ++it) {
        Widget::SharedPtr child = (*it);
        const unsigned int reloaded = child->reload_internal(rect, offset, Point(0, 0));
        result += reloaded;
    }
    return result;
}

void Widget::stopImages()
{
    assert(MutexStack::locked(Widget::lock().name()));
    for(Images::iterator it = mImages.begin(); it != mImages.end(); ++it)
        (*it)->stop();
}

void Widget::exec(const Variant &variant)
{
    NRDP_UNUSED(variant);
#if 0
    if(variant["name"] == "image_optimize") {
        for(Images::const_iterator it = mImages.begin(); it != mImages.end(); ++it) {
            int first_x=INT_MAX, last_x=INT_MAX, first_y=INT_MAX, last_y=INT_MAX;
            if(Surface::SharedPtr surface = (*it)->getSurface()) {
                Surface::Data data(surface);
                for(int y = 0; y < data.getHeight(); ++y) {
                    const unsigned char *line = data.scanline(y);
                    for(int x = 0; x < data.getWidth(); ++x) {
                        for(int i = 0; i < data.bytesPerPixel(); ++i) {
                            if(*(line + x + i)) {
                                if(x < first_x)
                                    first_x = x;
                                if(x > last_x || last_x == INT_MAX)
                                    last_x = x;
                                if(y < first_y)
                                    first_y = y;
                                if(y > last_y || last_y == INT_MAX)
                                    last_y = y;
                                break;
                            }
                        }
                    }
                }
                const Rect copyRect(first_x, first_y, (last_x - first_x)+1, (last_y - first_y)+1);
                if(Surface::SharedPtr copy = surface->duplicate(copyRect)) {
                    const std::vector<unsigned char> img = SurfaceDecoders::instance()->write(copy);
                    if(!img.empty()) {
                        std::string path = StringFormatter::sformat("/tmp/widget_%d_%d.img", mID, (*it)->getID());
                        if(FILE *f = fopen(path.c_str(), "w")) {
                            Log::error(TRACE_LOG, "[%s:%d]: %s (%d,%d+%dx%d)", __func__, __LINE__, path.c_str(),
                                                      copyRect.x, copyRect.y, copyRect.width, copyRect.height);
                            const size_t w = fwrite(&img[0], sizeof(char), img.size(), f);
                            assert(w == img.size());
                            fclose(f);
                        }
                    }
                }
            }
        }
    }
#endif
}
