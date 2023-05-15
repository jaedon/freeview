/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ImageBridge.h"
#include "Widget.h"
#include "Debug.h"
#include "GibbonConfiguration.h"

#include <nrd/Request.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringUtils.h>
#include <nrdnet/IAsyncHttpLib.h>

using namespace netflix::gibbon;
using namespace netflix;

#define ImageBridgePropertyList(Property)                           \
    Property(group, NfObject::ReadWrite|DumpProperty)               \
    Property(height, NfObject::ReadOnly)                            \
    Property(horizontalAlignment, NfObject::ReadWrite|DumpProperty) \
    Property(purge, NfObject::ReadWrite|DumpProperty)               \
    Property(sendImageLoaded, NfObject::ReadWrite)                  \
    Property(sourceRect, NfObject::ReadWrite|DumpProperty)          \
    Property(src, NfObject::ReadWrite|DumpProperty)                 \
    Property(verticalAlignment, NfObject::ReadWrite|DumpProperty)   \
    Property(visible, NfObject::ReadWrite|DumpProperty)             \
    Property(width, NfObject::ReadOnly)

PropertyTable(ImageBridge, ImageBridgePropertyList, properties)

#define ImageBridgeMethodList(Method)           \
    Method(destroy)

MethodTable(ImageBridge, ImageBridgeMethodList);

static Image::Alignment convertImageAlignment(const char *string)
{
    assert(!strchr(string, ' '));
    if(!strcmp(string, "normal"))
        return Image::Align_Normal;
    else if(!strcmp(string, "top"))
        return Image::Align_Top;
    else if(!strcmp(string, "left"))
        return Image::Align_Left;
    else if(!strcmp(string, "center"))
        return Image::Align_Center;
    else if(!strcmp(string, "right"))
        return Image::Align_Right;
    else if(!strcmp(string, "bottom"))
        return Image::Align_Bottom;
    else if(!strcmp(string, "tile"))
        return Image::Align_Tile;
    else if(!strcmp(string, "stretch"))
        return Image::Align_Stretch;
    else if(!strcmp(string, "aspect"))
        return Image::Align_MaintainAspectRatio;
    return Image::Align_Normal;
}

static unsigned convertImageAlignment(const Variant &value)
{
    unsigned align = Image::Align_Normal;
    switch (value.type()) {
    case Variant::Type_Integer:
    case Variant::Type_Double:
        align = value.value<int>();
        break;
    case Variant::Type_Array: {
        const int size = value.size();
        for(int i=0; i<size; ++i)
            align |= convertImageAlignment(value[i]);
        break; }
    case Variant::Type_String: {
        std::string string = value.value<std::string>();
        if(string.find(' ') == std::string::npos) {
            align = convertImageAlignment(string.c_str());
        } else {
            char *saveptr;
            for(char *str=strtok_r(&string[0], " ", &saveptr); str; str = strtok_r(0, " ", &saveptr))
                align |= convertImageAlignment(str);
        }
        break; }
    default:
        break;
    }
    return align;
}

ImageBridge::ImageBridge(const std::string &name, const Image::SharedPtr &image)
    : GibbonBaseBridge(name, Properties::ImageBridge::properties, Properties::ImageBridge::PropertyCount,
                       Methods::ImageBridge::methods, Methods::ImageBridge::MethodCount, Flag_NoPropertySync),
      mSendImageLoaded(false), mImage(image)
{

}

ImageBridge::~ImageBridge()
{
    ScopedMutex _lock(Widget::lock());
    mImage.reset();
    clearChildren();
}

void ImageBridge::init()
{
    mImage->addListener(static_pointer_cast<ImageBridge>(shared_from_this()));
}

bool ImageBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::ImageBridge::sendImageLoaded:
        *result = mSendImageLoaded;
        break;
    case Properties::ImageBridge::height:
        *result = mImage->getHeight();
        break;
    case Properties::ImageBridge::width:
        *result = mImage->getWidth();
        break;
    case Properties::ImageBridge::horizontalAlignment:
        *result = mImage->getHorizontalAlignment();
        break;
    case Properties::ImageBridge::src:
        *result = mImage->getUrl().str();
        break;
    case Properties::ImageBridge::verticalAlignment:
        *result = mImage->getVerticalAlignment();
        break;
    case Properties::ImageBridge::visible:
        *result = mImage->getVisible();
        break;
    case Properties::ImageBridge::purge:
        *result = mImage->canPurge();
        break;
    case Properties::ImageBridge::sourceRect: {
        const Rect rect = mImage->getSourceRect();
        if(rect.x != CoordinateMax)
            (*result)["x"] = rect.x;
        if(rect.y != CoordinateMax)
            (*result)["y"] = rect.y;
        if(rect.width != CoordinateMax)
            (*result)["width"] = rect.width;
        if(rect.height != CoordinateMax)
            (*result)["height"] = rect.height;
        break; }
    case Properties::ImageBridge::group:
        *result = mImage->group();
        break;
    default:
        return false;
    }
    return true;
}

bool ImageBridge::setProperty(int index, const Variant &value)
{
    GibbonBaseBridge::setHaveProperty(index);
    if(Debug::DebugWidgetProperties)
        Log::error(TRACE_LOG, "[%s:%d]: %s(%s) (%s->%s)", __func__, __LINE__, mImage->getWidget()->describe().c_str(),
                                  name().c_str(), propertyName(index), value.toJSON().c_str());
    switch(index) {
    case Properties::ImageBridge::sendImageLoaded: {
        const bool old = mSendImageLoaded;
        mSendImageLoaded = value.value<bool>();
        if(mSendImageLoaded && !old && mImage->getSurface())
            onImageLoaded(mImage, true);
        break; }
    case Properties::ImageBridge::src: {
        SurfaceLoader::Source::SharedPtr source = Image::createSource(value);
#if 0
        source->setLoadFlags(source->loadFlags() | Image::Load_LazyDownload);
#endif
        mImage->load(source);
        break; }
    case Properties::ImageBridge::horizontalAlignment:
        mImage->setHorizontalAlignment(convertImageAlignment(value));
        break;
    case Properties::ImageBridge::visible:
        mImage->setVisible(value.value<bool>(0, true));
        break;
    case Properties::ImageBridge::verticalAlignment:
        mImage->setVerticalAlignment(convertImageAlignment(value));
        break;
    case Properties::ImageBridge::purge:
        mImage->setCanPurge(value.value<bool>(0, true));
        break;
    case Properties::ImageBridge::sourceRect: {
        Rect rect = mImage->getSourceRect();
        if(value.isArray()) {
            if(value.size() >= 1)
                rect.x = value[0].value<Coordinate>(0, rect.x);
            if(value.size() >= 2)
                rect.y = value[1].value<Coordinate>(0, rect.y);
            if(value.size() >= 3)
                rect.width = value[2].value<Coordinate>(0, CoordinateMax);
            if(value.size() >= 4)
                rect.height = value[3].value<Coordinate>(0, CoordinateMax);
        } else {
            rect.x = value.mapValue<Coordinate>("x", 0, rect.x);
            rect.y = value.mapValue<Coordinate>("y", 0, rect.y);
            if(value.contains("width"))
                rect.width = value.mapValue<Coordinate>("width", 0, CoordinateMax);
            if(value.contains("height"))
                rect.height = value.mapValue<Coordinate>("height", 0, CoordinateMax);
        }
        mImage->setSourceRect(rect.normalized());
        break; }
    case Properties::ImageBridge::group: {
        bool ok;
        const int grp = value.value<int>(&ok);
        mImage->setGroup((!ok || grp < 0) ? -1 : grp);
        break; }
    default:
        return false;
    }
    return true;
}

Variant ImageBridge::invoke(int method, const Variant &)
{
    switch (method) {
    case Methods::ImageBridge::destroy:
        setParent(shared_ptr<NfObject>());
        break;
    default:
        return false;
    }
    return Variant();
}

Variant ImageBridge::imageLoadProperties(const Image::SharedPtr &image)
{
    assert(MutexStack::locked(Widget::lock().name()));
    Variant properties;
    const int httpStatusCode = image->getHttpStatusCode();
    if(httpStatusCode != -1)
        properties["statusCode"] = httpStatusCode;
    const int errorCode = image->getErrorCode();
    if(errorCode != -1) {
        const NetError::ErrorCode result = static_cast<NetError::ErrorCode>(errorCode);
        const NetError::ErrorGroup ng = NetError::mapErrorGroup(result);
        properties["errorcode"] = result;
        properties["errorgroup"] = ng;
    }
    if(image->hasFailed()) {
        properties["success"] = false;
    } else {
        properties["success"] = true;
        properties["width"] = image->getWidth();
        properties["height"] = image->getHeight();
        if(Surface::SharedPtr surface = image->getSurface()) {
            properties["decoded"] = true;
            if(!surface->isAccelerated())
                properties["slow"] = true;
        } else {
            properties["decoded"] = false;
        }
        properties["success"] = true;
    }
    NetworkMetrics metrics = image->getNetworkMetrics();
    properties["dnsTime"] = metrics.dnsTime();
    properties["connectTime"] = metrics.connectTime();
    properties["transactionTime"] = metrics.transactionTime();
    properties["duration"] = metrics.duration();
    if (metrics.serverIpAddress().valid())
        properties["serverIp"] = metrics.serverIpAddress().getStrAddr();
    const std::string cname = metrics.cname();
    if (!cname.empty())
        properties["cname"] = cname;
    const std::string sslVersion = metrics.sslVersion();
    if (!sslVersion.empty())
        properties["sslVersion"] = sslVersion;

    properties["size"] = image->getEncodedSize();
    properties["state"] = Resource::stateToString(image->state());
    return properties;
}

void ImageBridge::onImageLoaded(const Image::SharedPtr &image, bool loaded)
{
    assert(image == mImage);
    if(mSendImageLoaded && loaded) {
        PERFORMANCE_MARKER_SCOPED("imagebridge.onimageloaded");
        sendEvent("imageLoaded", ImageBridge::imageLoadProperties(image));
    }
}
