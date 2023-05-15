/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WidgetBridge.h"
#include "GibbonApplication.h"
#include "SurfaceDecoder.h"
#include "Shadow.h"
#include "Screen.h"
#include "TextTruncation.h"
#include "TextCursor.h"
#include "EdgeEffect.h"
#include "Debug.h"

#include <nrd/AppLog.h>
#include <nrdbase/Base64.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/tr1.h>
#include <nrdnet/NetError.h>

#include <iterator>

using namespace netflix::gibbon;
using namespace netflix;

static const unsigned int sRenderProperties = Widget::Property_RenderX|Widget::Property_RenderY|
                                              Widget::Property_RenderWidth|Widget::Property_RenderHeight;


enum Round
{
    Round_Floor,
    Round_Ceil,
    Round_Round, //baby, right round
};
static inline Coordinate scaleCoordinate(Coordinate coordinate, Round round=Round_Round)
{
    if(Debug::TestScreenCoordinateScale) {
        const float result = float(coordinate) * GibbonApplication::instance()->getScreen()->getCoordinateScale();
        if(round == Round_Round)
            return roundf(result);
        else if(round == Round_Ceil)
            return ceilf(result);
        return floorf(result);
    }
    return coordinate;
}
static inline Coordinate unscaleCoordinate(Coordinate coordinate, Round round=Round_Round)
{
    if(Debug::TestScreenCoordinateScale) {
        const float result = float(coordinate) / GibbonApplication::instance()->getScreen()->getCoordinateScale();
        if(round == Round_Round)
            return roundf(result);
        else if(round == Round_Ceil)
            return ceilf(result);
        return floorf(result);
    }
    return coordinate;
}

#define WidgetBridgePropertyList(Property)                          \
    Property(animations, NfObject::ReadWrite|DumpProperty)          \
    Property(backgroundColor, NfObject::ReadWrite|DumpProperty)     \
    Property(cache, NfObject::ReadWrite|DumpProperty)               \
    Property(clip, NfObject::ReadWrite|DumpProperty)                \
    Property(clone, NfObject::ReadWrite|DumpProperty)               \
    Property(drawOrder, NfObject::ReadWrite|DumpProperty)           \
    Property(erase, NfObject::ReadWrite|DumpProperty)               \
    Property(height, NfObject::ReadWrite|DumpProperty)              \
    Property(layout, NfObject::ReadWrite|DumpProperty)              \
    Property(layoutSpacing, NfObject::ReadWrite|DumpProperty)       \
    Property(layoutStretch, NfObject::ReadWrite|DumpProperty)       \
    Property(loadImages, NfObject::ReadWrite|DumpProperty)          \
    Property(maxHeight, NfObject::ReadWrite|DumpProperty)           \
    Property(maxWidth, NfObject::ReadWrite|DumpProperty)            \
    Property(minHeight, NfObject::ReadWrite|DumpProperty)           \
    Property(minWidth, NfObject::ReadWrite|DumpProperty)            \
    Property(name, NfObject::ReadWrite|DumpProperty)                \
    Property(opacity, NfObject::ReadWrite|DumpProperty)             \
    Property(opaque, NfObject::ReadWrite|DumpProperty)              \
    Property(padding, NfObject::ReadWrite|DumpProperty)             \
    Property(parent, NfObject::ReadWrite)                           \
    Property(renderHeight, NfObject::ReadOnly)                      \
    Property(renderWidth, NfObject::ReadOnly)                       \
    Property(renderX, NfObject::ReadOnly)                           \
    Property(renderY, NfObject::ReadOnly)                           \
    Property(scale, NfObject::ReadWrite|DumpProperty)               \
    Property(scrollX, NfObject::ReadWrite|DumpProperty)             \
    Property(scrollY, NfObject::ReadWrite|DumpProperty)             \
    Property(sendAnimationFinished, NfObject::ReadWrite)            \
    Property(sendRenderProperties, NfObject::ReadWrite)             \
    Property(smoothScale, NfObject::ReadWrite|DumpProperty)         \
    Property(transformOriginX, NfObject::ReadWrite|DumpProperty)    \
    Property(transformOriginY, NfObject::ReadWrite|DumpProperty)    \
    Property(video, NfObject::ReadWrite|DumpProperty)               \
    Property(visible, NfObject::ReadWrite|DumpProperty)             \
    Property(width, NfObject::ReadWrite|DumpProperty)               \
    Property(x, NfObject::ReadWrite|DumpProperty)                   \
    Property(y, NfObject::ReadWrite|DumpProperty)

PropertyTable(WidgetBridge, WidgetBridgePropertyList, properties)

#define WidgetBridgeMethodList(Method)          \
    Method(dump)                                \
    Method(exec)                                \
    Method(grab)                                \
    Method(removeEffect)                        \
    Method(removeImage)                         \
    Method(stopAnimation)

MethodTable(WidgetBridge, WidgetBridgeMethodList);

static Effect::Type convertEffect(const Variant &value)
{
    Effect::Type type = Effect::Type_Unknown;
    if(value.isNumber())
        type = (Effect::Type)value.value<int>();
    else
        type = Effect::getType(value.value<std::string>());
    return type;
}

static Widget::Property convertAnimatedProperty(const Variant &value)
{
    const std::string v = value.value<std::string>();
    const Widget::Property property = Widget::convertPropertyName(v);
    if(Widget::isAnimatedProperty(property))
        return property;
    return Widget::Property_Unknown;
}

static Widget::Layout convertLayout(const Variant &value)
{
    Widget::Layout layout = Widget::Layout_None;
    const std::string v = value.value<std::string>();
    if(v == "horizontal" || v == "h")
        layout = Widget::Layout_Horizontal;
    else if(v == "vertical" || v == "v")
        layout = Widget::Layout_Vertical;
    else if(v == "wrapping")
        layout = Widget::Layout_HorizontalWrapping;
    return layout;
}

static Variant convertLayout(Widget::Layout layout)
{
    Variant value;
    if(layout == Widget::Layout_Horizontal)
        value = "horizontal";
    else if(layout == Widget::Layout_Vertical)
        value = "vertical";
    else if(layout == Widget::Layout_HorizontalWrapping)
        value = "wrapping";
    return value;
}

WidgetBridge::WidgetBridge(const std::string &name, Widget::SharedPtr widget)
    : GibbonBaseBridge(name, Properties::WidgetBridge::properties, Properties::WidgetBridge::PropertyCount,
                       Methods::WidgetBridge::methods, Methods::WidgetBridge::MethodCount, Flag_NoPropertySync),
      mAnimate(true), mWidget(widget), mSendRenderProperties(false), mSendAnimationFinished(false)
{
    if(Debug::DebugWidgetSendRenderProperties)
        mSendRenderProperties = true;
}

WidgetBridge::~WidgetBridge()
{
    ScopedMutex _lock(Widget::lock());
    mWidget.reset();
    clearChildren();
}

void WidgetBridge::init()
{
    mWidget->addListener(Widget::Listener::SharedPtr(new WidgetListener(static_pointer_cast<WidgetBridge>(shared_from_this()))));
}

void WidgetBridge::syncPendingProperties(unsigned int properties)
{
    std::vector<int> updated;
    if(properties & Widget::Property_RenderX)
        updated.push_back(Properties::WidgetBridge::renderX);
    if(properties & Widget::Property_RenderY)
        updated.push_back(Properties::WidgetBridge::renderY);
    if(properties & Widget::Property_RenderWidth)
        updated.push_back(Properties::WidgetBridge::renderWidth);
    if(properties & Widget::Property_RenderHeight)
        updated.push_back(Properties::WidgetBridge::renderHeight);
    // if(properties & Widget::Property_RenderTextLines)
    //     updated.push_back(Properties::WidgetBridge::renderTextLines);
    // if(properties & Widget::Property_RenderTextCursorPosition)
    //     updated.push_back(Properties::WidgetBridge::renderTextCursorPosition);
    // if(properties & Widget::Property_RenderTextFirstLine)
    //     updated.push_back(Properties::WidgetBridge::renderTextFirstLine);
    if(updated.size())
        propertiesUpdated(&updated[0], updated.size());
}

void WidgetBridge::handleAnimationFinished(Widget::Property property, const AnimationFinish &finish)
{
    assert(finish.state != AnimationFinish::State_None);
    GibbonBaseBridge::syncPendingProperties(); //send everything
    if(mSendAnimationFinished || (finish.state != AnimationFinish::State_Restarting && !(finish.frame.flags & Animation::KeyFrame::NoOp))) {
        Variant data;
        if(finish.frame.info.id != INT_MAX)
            data["id"] = finish.frame.info.id;
        if(finish.state == AnimationFinish::State_Aborted)
            data["aborted"] = true;
        if(!isnan(finish.fps))
            data["fps"] = finish.fps;
        if(!isnan(finish.value)) {
            if(Widget::isCoordinateProperty(property))
                data["value"] = unscaleCoordinate(finish.value);
            else
                data["value"] = finish.value;
        }
        switch(property) {
        case Widget::Property_ScrollX:
            data["propertyID"] = Properties::WidgetBridge::scrollX;
            break;
        case Widget::Property_ScrollY:
            data["propertyID"] = Properties::WidgetBridge::scrollY;
            break;
        case Widget::Property_X:
            data["propertyID"] = Properties::WidgetBridge::x;
            break;
        case Widget::Property_Y:
            data["propertyID"] = Properties::WidgetBridge::y;
            break;
        case Widget::Property_Width:
            data["propertyID"] = Properties::WidgetBridge::width;
            break;
        case Widget::Property_Height:
            data["propertyID"] = Properties::WidgetBridge::height;
            break;
        case Widget::Property_Opacity:
            data["propertyID"] = Properties::WidgetBridge::opacity;
            break;
        case Widget::Property_Scale:
            data["propertyID"] = Properties::WidgetBridge::scale;
            break;
        default:
            assert(false);
        }
        sendEvent("animationFinished", data);
    }
}

void WidgetBridge::handleRenderPropertiesChanged(unsigned int properties)
{
    if(!mSendRenderProperties)
        return;
    const unsigned int renderProperties = properties & sRenderProperties;
    assert(properties && renderProperties == properties);
    propertiesChanged(renderProperties, !mWidget->getAnimatingProperties());
}

bool WidgetBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::WidgetBridge::sendRenderProperties:
        *result = mSendRenderProperties;
        break;
    case Properties::WidgetBridge::sendAnimationFinished:
        *result = mSendAnimationFinished;
        break;
    case Properties::WidgetBridge::backgroundColor:
        *result = convertColor(mWidget->getBackgroundColor());
        break;
    case Properties::WidgetBridge::cache: {
        const Widget::RenderCache cache = mWidget->getRenderCache();
        if(cache == Widget::RenderCache_Enabled)
            *result = true;
        else if(cache == Widget::RenderCache_Disabled)
            *result = false;
        break; }
    case Properties::WidgetBridge::clip:
        *result = mWidget->getClipEnabled();
        break;
    case Properties::WidgetBridge::renderHeight: {
        const Coordinate val = mWidget->getTransformedRenderRect().height;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = unscaleCoordinate(val);
        break; }
    case Properties::WidgetBridge::renderWidth: {
        const Coordinate val =mWidget->getTransformedRenderRect().width;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = unscaleCoordinate(val);
        break; }
    case Properties::WidgetBridge::renderX: {
        const Coordinate val = mWidget->getTransformedRenderRect().x;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = unscaleCoordinate(val);
        break; }
    case Properties::WidgetBridge::renderY: {
        const Coordinate val = mWidget->getTransformedRenderRect().y;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = unscaleCoordinate(val);
        break; }
    case Properties::WidgetBridge::minHeight: {
        const Coordinate val = mWidget->getMinSize().height;
        if(val == 0)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::minWidth: {
        const Coordinate val = mWidget->getMinSize().width;
        if(val == 0)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::maxHeight: {
        const Coordinate val = mWidget->getMaxSize().height;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::maxWidth: {
        const Coordinate val = mWidget->getMaxSize().width;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::drawOrder:
        *result = mWidget->getDrawOrder();
        break;
    case Properties::WidgetBridge::erase:
        *result = mWidget->getErase();
        break;
    case Properties::WidgetBridge::video:
        *result = (bool)mWidget->getVideo();
        break;
    case Properties::WidgetBridge::height: {
        const Coordinate val = mWidget->getRect().height;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::smoothScale:
        *result = mWidget->getSmoothScale();
        break;
    case Properties::WidgetBridge::visible:
        *result = mWidget->getVisible();
        break;
    case Properties::WidgetBridge::layout:
        if(mWidget->hasLayout()) {
            (*result)["layout"] = convertLayout(mWidget->getLayout());
            (*result)["align"] = convertAlignment(mWidget->getLayoutAlign());
        }
        break;
    case Properties::WidgetBridge::layoutStretch:
        *result = mWidget->getLayoutStretch();
        break;
    case Properties::WidgetBridge::loadImages:
        *result = mWidget->getLoadImages();
        break;
    case Properties::WidgetBridge::layoutSpacing:
        *result = mWidget->getLayoutSpacing();
        break;
    case Properties::WidgetBridge::name:
        *result = mWidget->getName();
        break;
    case Properties::WidgetBridge::opacity:
        *result = mWidget->getOpacity();
        break;
    case Properties::WidgetBridge::opaque:
        *result = mWidget->getOpaque();
        break;
    case Properties::WidgetBridge::padding:
        (*result)["top"] = mWidget->getTopPadding();
        (*result)["left"] = mWidget->getLeftPadding();
        (*result)["bottom"] = mWidget->getBottomPadding();
        (*result)["right"] = mWidget->getRightPadding();
        break;
    case Properties::WidgetBridge::parent:
        if(Widget::SharedPtr p = mWidget->getParent())
            *result = p->getID();
        else
            *result = Variant();
        break;
    case Properties::WidgetBridge::clone:
        if(Widget::SharedPtr c = mWidget->getClone())
            *result = c->getID();
        else
            *result = Variant();
        break;
    case Properties::WidgetBridge::scale:
        *result = mWidget->getScale();
        break;
    case Properties::WidgetBridge::transformOriginX:
        *result = mWidget->getTransformOrigin().x;
        break;
    case Properties::WidgetBridge::transformOriginY:
        *result = mWidget->getTransformOrigin().y;
        break;
    case Properties::WidgetBridge::width: {
        const Coordinate val = mWidget->getRect().width;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::scrollX: {
        *result = mWidget->getScrollX();
        break; }
    case Properties::WidgetBridge::scrollY: {
        *result = mWidget->getScrollY();
        break; }
    case Properties::WidgetBridge::x: {
        const Coordinate val = mWidget->getRect().x;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::WidgetBridge::y: {
        const Coordinate val = mWidget->getRect().y;
        if(val == CoordinateMax)
            *result = Variant();
        else
            *result = val;
        break; }
    default:
        return false;
    }
    return true;
}

void WidgetBridge::setProperties(int index, const Variant &value)
{
    switch(index) {
    case Properties::WidgetBridge::x:
    case Properties::WidgetBridge::y:
    case Properties::WidgetBridge::scale:
    case Properties::WidgetBridge::width:
    case Properties::WidgetBridge::height:
    case Properties::WidgetBridge::opacity:
    case Properties::WidgetBridge::scrollX:
    case Properties::WidgetBridge::scrollY: {
        assert(value.isArray());
        const int size = value.size();
        for(int i = 0; i < size; ++i) {
            const Variant &v = value[i];
            if(v.isNull() && i != size-1) {
                Animation::stop(mWidget, index, Animation::Stop_Current);
            } else {
                setProperty(index, v);
            }
        }
        break; }
    default:
        GibbonBaseBridge::setProperties(index, value);
        break;
    }
}


bool WidgetBridge::setProperty(int index, const Variant &value)
{
    GibbonBaseBridge::setHaveProperty(index);
    if(Debug::DebugWidgetProperties)
        Log::error(TRACE_LOG, "[%s:%d]: %s (%s->%s)", __func__, __LINE__, mWidget->describe().c_str(),
                                  propertyName(index), value.toJSON().c_str());

    switch(index) {
    case Properties::WidgetBridge::sendRenderProperties: {
        const bool old = mSendRenderProperties;
        mSendRenderProperties = value.value<bool>();
        if(mSendRenderProperties && !old)
            handleRenderPropertiesChanged(sRenderProperties);
        break; }
    case Properties::WidgetBridge::sendAnimationFinished: {
        mSendAnimationFinished = value.value<bool>();
        break; }
    case Properties::WidgetBridge::backgroundColor:
        mWidget->setBackgroundColor(convertColor(value));
        break;
    case Properties::WidgetBridge::cache:
        if(value.isNull())
            mWidget->setRenderCache(Widget::RenderCache_Default);
        else if(value.value<bool>())
            mWidget->setRenderCache(Widget::RenderCache_Enabled);
        else
            mWidget->setRenderCache(Widget::RenderCache_Disabled);
        break;
    case Properties::WidgetBridge::clip:
        mWidget->setClipEnabled(value.value<bool>(0, mWidget->getClipEnabled()));
        break;
    case Properties::WidgetBridge::drawOrder:
        mWidget->setDrawOrder(value.value<int>());
        break;
    case Properties::WidgetBridge::erase:
        mWidget->setErase(value.value<bool>(0, mWidget->getErase()));
        break;
    case Properties::WidgetBridge::video:
        if(value.isBoolean()) {
            if(value.value<bool>())
                mWidget->createVideo();
            else
                mWidget->destroyVideo();
        } else if(value.isString()) {
            const std::string name = value.value<std::string>();
            if(shared_ptr<netflix::mediacontrol::IMediaSourcePlayer> player = nrdApp()->player(name))
                mWidget->createVideo()->setPlayer(player);
            else
                mWidget->destroyVideo();
        }
        break;
    case Properties::WidgetBridge::smoothScale:
        mWidget->setSmoothScale(value.value<bool>(0, mWidget->getSmoothScale()));
        break;
    case Properties::WidgetBridge::visible:
        mWidget->setVisible(value.value<bool>(0, mWidget->getVisible()));
        break;
    case Properties::WidgetBridge::layout:
        if(value.isNull()) {
            mWidget->setLayout(Widget::Layout_None);
        } else if(value.isStringMap()) {
            if(value.contains("layout"))
                mWidget->setLayout(convertLayout(value["layout"]));
            if(value.contains("align"))
                mWidget->setLayoutAlign(convertAlignment(value["align"]));
        } else {
            mWidget->setLayout(convertLayout(value));
        }
        break;
    case Properties::WidgetBridge::loadImages:
        if(value.isNull())
            mWidget->setLoadImages(Widget::LoadImages_Default);
        else if(value.isNumber())
            mWidget->setLoadImages((Widget::LoadImages)value.value<int>());
        else if(value.value<bool>(0, mWidget->getLoadImages()))
            mWidget->setLoadImages(Widget::LoadImages_Enabled);
        else
            mWidget->setLoadImages(Widget::LoadImages_Disabled);
        break;
    case Properties::WidgetBridge::layoutStretch:
        mWidget->setLayoutStretch(value.isNull() ? 0 : value.value<int>());
        break;
    case Properties::WidgetBridge::layoutSpacing:
        mWidget->setLayoutSpacing(value.isNull() ? 0 : (int16_t)scaleCoordinate(value.value<int>()));
        break;
    case Properties::WidgetBridge::name:
        mWidget->setName(value.value<std::string>());
        break;
    case Properties::WidgetBridge::opaque:
        mWidget->setOpaque(value.value<bool>(0, mWidget->getOpaque()));
        break;
    case Properties::WidgetBridge::opacity: {
        setAnimatedProperty(Widget::Property_Opacity, value);
        break; }
    case Properties::WidgetBridge::scale: {
        setAnimatedProperty(Widget::Property_Scale, value);
        break; }
    case Properties::WidgetBridge::padding:
        if(value.isNumber()) {
            const int pad = (int)scaleCoordinate(value.value<int>());
            mWidget->setPadding(pad, pad, pad, pad, pad);
        } else if(value.isString()) {
            std::vector<std::string> pads;
            StringTokenizer::tokenize(value.value<std::string>(), pads, " ");
            pads.push_back("0"); pads.push_back("0");
            pads.push_back("0"); pads.push_back("0");
            mWidget->setPadding((int16_t)scaleCoordinate(atoi(pads[0].c_str())), (int16_t)scaleCoordinate(atoi(pads[1].c_str())), (int16_t)scaleCoordinate(atoi(pads[2].c_str())),
                                (int16_t)scaleCoordinate(atoi(pads[3].c_str())), (int16_t)scaleCoordinate(atoi(pads[4].c_str())));
        } else if(value.isArray()) {
            mWidget->setPadding((int16_t)scaleCoordinate(value.size() >= 1 ? value[0].value<int>(0, 0) : 0),
                                (int16_t)scaleCoordinate(value.size() >= 2 ? value[1].value<int>(0, 0) : 0),
                                (int16_t)scaleCoordinate(value.size() >= 3 ? value[2].value<int>(0, 0) : 0),
                                (int16_t)scaleCoordinate(value.size() >= 4 ? value[3].value<int>(0, 0) : 0),
                                (int16_t)scaleCoordinate(value.size() >= 5 ? value[4].value<int>(0, 0) : 0));
        } else {
            mWidget->setPadding((int16_t)scaleCoordinate(value.mapValue<int>("top", 0, 0)),
                                (int16_t)scaleCoordinate(value.mapValue<int>("left", 0, 0)),
                                (int16_t)scaleCoordinate(value.mapValue<int>("bottom", 0, 0)),
                                (int16_t)scaleCoordinate(value.mapValue<int>("right", 0, 0)),
                                (int16_t)scaleCoordinate(value.mapValue<int>("wrap", 0, 0)));
        }
        break;
    case Properties::WidgetBridge::clone:
        if(value.isNull()) {
            mWidget->setClone(Widget::SharedPtr());
        } else {
            const int clone = value.value<int>();
            shared_ptr<WidgetBridge> cloneWidget = GibbonApplication::instance()->getBridge()->createWidget(clone);
            mWidget->setClone(cloneWidget->getWidget());
        }
        break;
    case Properties::WidgetBridge::parent:
        if(value.isNull()) {
            if(Widget::SharedPtr p = mWidget->getParent())
                p->removeChild(mWidget);
        } else {
            int parent = -1, index = -1;
            if(value.isArray()) {
                if(value.size() >= 1)
                    parent = value[0].value<int>(0, -1);
                if(value.size() >= 2)
                    index = value[1].value<int>(0, -1);
            } else if(value.isStringMap()) {
                parent = value.mapValue<int>("id", 0, -1);
                index = value.mapValue<int>("index", 0, -1);
            } else {
                parent = value.value<int>();
            }
            shared_ptr<WidgetBridge> parentWidget = GibbonApplication::instance()->getBridge()->createWidget(parent);
            assert(parentWidget);
            parentWidget->getWidget()->addChild(mWidget, index);
        }
        break;
    case Properties::WidgetBridge::transformOriginX: {
        Point to = mWidget->getTransformOrigin();
        to.x = scaleCoordinate(value.value<Coordinate>());
        mWidget->setTransformOrigin(to);
        break; }
    case Properties::WidgetBridge::transformOriginY: {
        Point to = mWidget->getTransformOrigin();
        to.y = scaleCoordinate(value.value<Coordinate>());
        mWidget->setTransformOrigin(to);
        break; }
    case Properties::WidgetBridge::minHeight: {
        Size m = mWidget->getMinSize();
        if(value.isNull())
            m.height = 0;
        else
            m.height = scaleCoordinate(value.value<Coordinate>());
        mWidget->setMinSize(m);
        break; }
    case Properties::WidgetBridge::minWidth: {
        Size m = mWidget->getMinSize();
        if(value.isNull())
            m.width = 0;
        else
            m.width = scaleCoordinate(value.value<Coordinate>());
        mWidget->setMinSize(m);
        break; }
    case Properties::WidgetBridge::maxHeight: {
        Size m = mWidget->getMaxSize();
        if(value.isNull())
            m.height = CoordinateMax;
        else
            m.height = scaleCoordinate(value.value<Coordinate>());
        mWidget->setMaxSize(m);
        break; }
    case Properties::WidgetBridge::maxWidth: {
        Size m = mWidget->getMaxSize();
        if(value.isNull())
            m.width = CoordinateMax;
        else
            m.width = scaleCoordinate(value.value<Coordinate>());
        mWidget->setMaxSize(m);
        break; }
    case Properties::WidgetBridge::scrollX: {
        setAnimatedProperty(Widget::Property_ScrollX, value);
        break; }
    case Properties::WidgetBridge::scrollY: {
        setAnimatedProperty(Widget::Property_ScrollY, value);
        break; }
    case Properties::WidgetBridge::x: {
        setAnimatedProperty(Widget::Property_X, value);
        break; }
    case Properties::WidgetBridge::y: {
        setAnimatedProperty(Widget::Property_Y, value);
        break; }
    case Properties::WidgetBridge::width: {
        setAnimatedProperty(Widget::Property_Width, value);
        break; }
    case Properties::WidgetBridge::height: {
        setAnimatedProperty(Widget::Property_Height, value);
        break; }
    default:
        return false;
    }
    return true;
}

shared_ptr<TextBridge> WidgetBridge::createText()
{
    if(!mTextBridge) {
        Text::SharedPtr text(new Text(mWidget));
        if(Debug::DebugWidgetProperties)
            Log::error(TRACE_LOG, "[%s:%d]: %s (CREATE_TEXT)", __func__, __LINE__, mWidget->describe().c_str());
        mWidget->setText(text);

        mTextBridge.reset(new TextBridge(text));
        mTextBridge->init();
        mTextBridge->setParent(shared_from_this());
    }
    assert(mTextBridge->getText());
    return mTextBridge;
}

shared_ptr<EffectBridge> WidgetBridge::createEffect(int id, const Variant &type)
{
    return createEffect(id, convertEffect(type));
}

shared_ptr<EffectBridge> WidgetBridge::createEffect(int id, Effect::Type type)
{
    shared_ptr<EffectBridge> bridge = getEffect(id);
    if(!bridge) {
        Effect::SharedPtr effect = Effect::create(id, type);
        if(Debug::DebugWidgetProperties)
            Log::error(TRACE_LOG, "[%s:%d]: %s (CREATE_EFFECT %d)", __func__, __LINE__, mWidget->describe().c_str(), id);
        assert(effect);
        mWidget->addEffect(effect);

        std::ostringstream stream;
        stream << "effect" << id;
        bridge.reset(new EffectBridge(stream.str(), effect));
        bridge->init();
        {
            if(!mEffectsBridge) {
                mEffectsBridge.reset(new NfObject("effects"));
                mEffectsBridge->setParent(shared_from_this());
            }
            bridge->setParent(mEffectsBridge);
        }
    }
    assert(bridge->getEffect() && (type == Effect::Type_Unknown || bridge->getEffect()->getType() == type));
    return bridge;
}

shared_ptr<ImageBridge> WidgetBridge::createImage(int id, Image::Type type)
{
    shared_ptr<ImageBridge> bridge = getImage(id);
    if(!bridge) {
        Image::SharedPtr image(new Image(id, mWidget, type));
        if(Debug::DebugWidgetProperties)
            Log::error(TRACE_LOG, "[%s:%d]: %s (CREATE_IMAGE %d)", __func__, __LINE__, mWidget->describe().c_str(), id);
        mWidget->addImage(image);

        std::ostringstream stream;
        stream << "image" << id;
        bridge.reset(new ImageBridge(stream.str(), image));
        bridge->init();
        {
            if(!mImagesBridge) {
                mImagesBridge.reset(new NfObject("images"));
                mImagesBridge->setParent(shared_from_this());
            }
            bridge->setParent(mImagesBridge);
        }
    }
    assert(bridge->getImage() && (type == Image::Type_None || bridge->getImage()->getType() == type));
    return bridge;
}

void WidgetBridge::destroy()
{
    if (shared_ptr<DebuggerBridge> debugger = GibbonApplication::instance()->getDebugger()) {
        if (debugger->isEnabled()) {
            debugger->destroyWidget(getID());
        }
    }

    ScopedMutex _widgetLock(Widget::lock());
    if(Widget::SharedPtr parent = mWidget->getParent())
        parent->removeChild(mWidget);
    destroy_internal();
}

void WidgetBridge::destroy_internal()
{
    Widget::SharedPtr root = GibbonApplication::instance()->getScreen()->root();
    if(root != mWidget) {
        const Widget::ChildContainer &children = mWidget->getChildren();
        for(Widget::ChildContainer::const_iterator it = children.begin(); it != children.end(); ++it) {
            if(shared_ptr<WidgetBridge> child = GibbonApplication::instance()->getBridge()->getWidget((*it)->getID()))
                child->destroy_internal();
        }
        GibbonApplication::instance()->getBridge()->removeWidget(mWidget->getID());
        setParent(shared_ptr<NfObject>());
    }
}

Variant WidgetBridge::invoke(int method, const Variant &variant)
{
    bool ok;
    switch(method) {
    case Methods::WidgetBridge::dump: {
        uint8_t flags = 0;
        if (variant.mapValue<bool>("recurse", 0, true))
            flags |= Dump_Recursive;
        Variant result = dump(flags);

        const int id = variant.mapValue<int>("id", &ok);
        if(ok) {
            Variant data;
            data["id"] = id;
            data["data"] = result;
            sendEvent("dump", data);
        }
        break; }
    case Methods::WidgetBridge::removeEffect: {
        const int id = variant.mapValue<int>("effect", &ok);
        if(!ok) {
            invalidArgumentError(method, "effect");
            return false;
        }
        removeEffect(getEffect(id));
        break; }
    case Methods::WidgetBridge::removeImage: {
        const int id = variant.mapValue<int>("image", &ok);
        if(!ok) {
            invalidArgumentError(method, "image");
            return false;
        }
        removeImage(getImage(id));
        break; }
    case Methods::WidgetBridge::stopAnimation: {
        int property = 0;
        {
            const Variant &property_v = variant["property"];
            if(!property_v.isNull()) {
                property = convertAnimatedProperty(property_v);
                if(property == Widget::Property_Unknown) {
                    invalidArgumentError(method, "property");
                    return false;
                }
            }
        }
        Animation::Stop stopType = Animation::Stop_End;
        {
            const Variant &end_v = variant["end"];
            if(end_v.isBoolean() && !end_v.boolean()) {
                stopType = Animation::Stop_Current;
            } else if(end_v.isString()) {
                if(end_v.string() == "end")
                    stopType = Animation::Stop_End;
                else if(end_v.string() == "pending")
                    stopType = Animation::Stop_Pending;
                else if(end_v.string() == "current")
                    stopType = Animation::Stop_Current;
            }
        }
#if defined(DEBUG_ANIMATION_FPS) && (DEBUG_ANIMATION_FPS > 2)
        Log::info(TRACE_GIBBON_ANIMATION, "Animation(%p): %d explicitly stopped[%d]", mWidget.get(), property, stopType);
#endif
        {
            ScopedMutex _lock(Widget::lock());
            Animation::stop(mWidget, property, stopType);
        }
        break; }
    case Methods::WidgetBridge::exec:
        mWidget->exec(variant);
        break;
    case Methods::WidgetBridge::grab: {
        const int id = variant.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["id"] = id;
        if(DisplayList::SharedPtr grab = mWidget->grab(Widget::Render_NoCache)) {
            if(Surface::SharedPtr surface = grab->flatten()) {
                const DataBuffer grabBuffer = SurfaceDecoders::instance()->encode(surface);
                if(grabBuffer.empty())
                    data["data"] = Variant();
                else
                    data["data"] = "data:image/png;base64," + grabBuffer.toBase64String();
            }
        }
        sendEvent("grab", data);
        break; }
    }
    return Variant();
}

void WidgetBridge::setAnimatedProperty(Widget::Property property, const Variant &value)
{
    if(value.isStringMap()) {
        const Variant animate = value["animate"];
        if(animate.isStringMap()) {
            const double end = animate.mapValue<double>("end", 0, NAN);
            double start = NAN;
            const Variant start_v = animate["start"];
            if(start_v.isNull()) {
                if(isnan(Animation::getValue(mWidget, property)))
                    start = animate.mapValue<double>("previous", 0, NAN);
            } else if(start_v.isStringMap()) {
                start = start_v["value"].value<double>(0, NAN);
                if(start_v.contains("relative")) {
                    if(isnan(start))
                        start = 0;
                    const int relative_id = start_v.mapValue<int>("relative", 0, 0);
                    if(Widget::SharedPtr relative = GibbonApplication::instance()->getWidget(relative_id)) {
                        const Variant::StringMapIterator rel_property_v = start_v.find("relativeProperty");
                        Widget::Property rel_property = convertAnimatedProperty((*rel_property_v).second);
                        if(rel_property == Widget::Property_Unknown)
                            rel_property = property;
                        double rel_value = Animation::getValue(relative, rel_property);
                        if(isnan(rel_value)) //animation knows nothing about it so go directly to the widget
                            rel_value = relative->getAnimatedProperty(rel_property);
                        start += rel_value;
                    }
                }
            } else {
                start = start_v.value<double>(0, NAN);
            }
            const Animation::Info info(animate);
            const bool append = animate.mapValue<bool>("append", 0, false);
            setAnimatedProperty(property, end, start, &info, append);
        } else {
            const double end = value.mapValue<double>("value", 0, NAN);
            setAnimatedProperty(property, end, NAN, 0);
        }
    } else {
        setAnimatedProperty(property, value.isNull() ? NAN : value.value<double>(), NAN, 0);
    }
}

void WidgetBridge::setAnimatedProperty(Widget::Property property, float value, float startValue, const Animation::Info *animation, bool append)
{
    ScopedMutex _lock(Widget::lock());
    const bool isCoordinateProperty = Widget::isCoordinateProperty(property);
    if(isCoordinateProperty && !isnan(value)) {
#ifdef BUILD_DEBUG
        if(!gibbon_float_compare(roundf(value), value))
            Log::error(TRACE_GIBBON_WIDGET, "[%s] Property(%s): Set to non-integral value %f",
                       mWidget->describe().c_str(), mWidget->getAnimatedPropertyName(property).c_str(), value);
#endif
        if(Debug::TestScreenCoordinateScale) {
            Round round = Round_Round;
            // if(property == Widget::Property_X || property == Widget::Property_Y)
            //     round = Round_Floor;
            // else if(property == Widget::Property_Width || property == Widget::Property_Height)
            //     round = Round_Ceil;
            value = scaleCoordinate(value, round);
        }
        value = roundf(value);
    }
    if(animation && mAnimate) {
        if(isCoordinateProperty && !isnan(startValue)) {
#ifdef BUILD_DEBUG
            if(!gibbon_float_compare(roundf(startValue), startValue))
                Log::error(TRACE_GIBBON_WIDGET, "[%s] Property(%s): Set to non-integral startValue %f",
                           mWidget->describe().c_str(), mWidget->getAnimatedPropertyName(property).c_str(), startValue);
#endif
            if(Debug::TestScreenCoordinateScale)
                startValue = scaleCoordinate(startValue);
            startValue = roundf(startValue);
        }
        Animation::start(mWidget, property, startValue, value, *animation, append);
        return;
    }
    //jump there and cancel existing animation
    Animation::stop(mWidget, property, Animation::Stop_Current);
    mWidget->setAnimatedProperty(property, value);
}

Variant WidgetBridge::dump(uint8_t flags) const
{
    Variant result;
    if(mWidget) {
        if (!(flags & Dump_NoProperties)) {
            if(const PropertyInfo *p = properties()) {
                for(int i = 0; i < propertyCount(); ++i) {
                    if(((p+i)->flags & DumpProperty) && ((flags & Dump_All) || (haveProperty(i)))) {
                        Variant property;
                        if(getProperty(i, &property))
                            result[p[i].name] = property;
                    }
                }
            }

            if(mTextBridge) {
                if(const PropertyInfo *p = mTextBridge->properties()) {
                    Variant text;
                    for(int i = 0; i < mTextBridge->propertyCount(); ++i) {
                        if(((p+i)->flags & DumpProperty) && ((flags & Dump_All) || (mTextBridge->haveProperty(i)))) {
                            Variant property;
                            if(mTextBridge->getProperty(i, &property))
                                text[p[i].name] = property;
                        }
                    }
                    if(!text.isNull())
                        result["text"] = text;
                }
            }

            if(mEffectsBridge) {
                const NfObject::ObjectMap c = mEffectsBridge->children();
                for (NfObject::ObjectMap::const_iterator it = c.begin(); it != c.end(); ++it) {
                    shared_ptr<EffectBridge> effect = static_pointer_cast<EffectBridge>(it->second);
                    if(const PropertyInfo *p = effect->properties()) {
                        Variant effectProperties;
                        for(int i = 0; i < effect->propertyCount(); ++i) {
                            if(((p+i)->flags & DumpProperty) && ((flags & Dump_All) || (effect->haveProperty(i)))) {
                                Variant property;
                                if(effect->getProperty(i, &property))
                                    effectProperties[p[i].name] = property;
                            }
                        }
                        if(!effectProperties.isNull()) {
                            effectProperties["type"] = effect->getEffect()->getType();
                            result["effects"].push_back(effectProperties);
                        }
                    }
                }
            }

            if(mImagesBridge) {
                const NfObject::ObjectMap c = mImagesBridge->children();
                for (NfObject::ObjectMap::const_iterator it = c.begin(); it != c.end(); ++it) {
                    shared_ptr<ImageBridge> image = static_pointer_cast<ImageBridge>(it->second);
                    if(const PropertyInfo *p = image->properties()) {
                        Variant imageProperties;
                        for(int i = 0; i < image->propertyCount(); ++i) {
                            if(((p+i)->flags & DumpProperty) && ((flags & Dump_All) || (image->haveProperty(i)))) {
                                Variant property;
                                if(image->getProperty(i, &property))
                                    imageProperties[p[i].name] = property;
                            }
                        }
                        if(!imageProperties.isNull()) {
                            imageProperties["background"] = image->getImage()->getType() == Image::Type_Background;
                            result["images"].push_back(imageProperties);
                        }
                    }
                }
            }
        }

        if((flags & Dump_All))
            result["id"] = mWidget->getID();
        if(flags & Dump_Recursive) {
            const Widget::ChildContainer &children = mWidget->getChildren();
            for(Widget::ChildContainer::const_iterator it = children.begin(); it != children.end(); ++it) {
                if(shared_ptr<WidgetBridge> child = GibbonApplication::instance()->getBridge()->getWidget((*it)->getID()))
                    result["children"].push_back(child->dump(flags));
            }
        }
    }
    return result;
}

void WidgetBridge::removeEffect(const shared_ptr<EffectBridge> &effect)
{
    if(effect)
        mWidget->removeEffect(effect->getEffect());
}

shared_ptr<EffectBridge> WidgetBridge::getEffect(int id)
{
    if(mEffectsBridge) {
        const NfObject::ObjectMap c = mEffectsBridge->children();
        for (NfObject::ObjectMap::const_iterator it = c.begin(); it != c.end(); ++it) {
            shared_ptr<EffectBridge> effect = static_pointer_cast<EffectBridge>(it->second);
            if(effect->getEffect() && effect->getEffect()->getID() == id)
                return effect;
        }
    }
    return shared_ptr<EffectBridge>();
}

void WidgetBridge::removeImage(const shared_ptr<ImageBridge> &image)
{
    if(image)
        mWidget->removeImage(image->getImage());
}

shared_ptr<ImageBridge> WidgetBridge::getImage(int id)
{
    if(mImagesBridge) {
        const NfObject::ObjectMap c = mImagesBridge->children();
        for (NfObject::ObjectMap::const_iterator it = c.begin(); it != c.end(); ++it) {
            shared_ptr<ImageBridge> image = static_pointer_cast<ImageBridge>(it->second);
            if(image->getImage() && image->getImage()->getID() == id)
                return image;
        }
    }
    return shared_ptr<ImageBridge>();
}

