/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ScreenBridge.h"
#include "WidgetBridge.h"

#include "Screen.h"
#include "SurfaceDecoder.h"
#include "GibbonApplication.h"

#include <nrdbase/Base64.h>

#include <iterator>

using namespace netflix::gibbon;
using namespace netflix;

#define ScreenBridgePropertyList(Property)      \
    Property(height, NfObject::ReadOnly)        \
    Property(overlay, NfObject::ReadWrite)      \
    Property(root, NfObject::ReadWrite)         \
    Property(scale, NfObject::ReadOnly)         \
    Property(suspend, NfObject::ReadWrite)      \
    Property(width, NfObject::ReadOnly)

PropertyTable(ScreenBridge, ScreenBridgePropertyList, properties)

#define ScreenBridgeMethodList(Method)          \
    Method(grab)                                \
    Method(update)

MethodTable(ScreenBridge, ScreenBridgeMethodList);

ScreenBridge::ScreenBridge()
    : GibbonBaseBridge("scene", Properties::ScreenBridge::properties,
                       Properties::ScreenBridge::PropertyCount,
                       Methods::ScreenBridge::methods,
                       Methods::ScreenBridge::MethodCount)
{
}

ScreenBridge::~ScreenBridge()
{
    ScopedMutex _lock(Widget::lock());
    clearScene();
    mOverlay.reset();
    clearChildren();
}

void ScreenBridge::clearScene()
{
    assert(MutexStack::locked(Widget::lock().name()));
    mRoot.reset();
}

bool ScreenBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::ScreenBridge::scale: {
        *result = GibbonApplication::instance()->getScreen()->getCoordinateScale();
        break; }
    case Properties::ScreenBridge::suspend: {
        *result = GibbonApplication::instance()->isSuspended();
        break; }
    case Properties::ScreenBridge::height: {
        *result = GibbonConfiguration::uiHeight();
        break; }
    case Properties::ScreenBridge::root: {
        *result = mRoot ? Variant(mRoot->getID()) : Variant();
        break; }
    case Properties::ScreenBridge::overlay: {
        *result = mOverlay ? Variant(mOverlay->getID()) : Variant();
        break; }
    case Properties::ScreenBridge::width: {
        *result = GibbonConfiguration::uiWidth();
        break; }
    default:
        return false;
    }
    return true;
}

bool ScreenBridge::setProperty(int index, const Variant &value)
{
    switch(index) {
    case Properties::ScreenBridge::suspend: {
        NTRACE(TRACE_SUSPEND, "gibbon.suspended = %s", value.toJSON().c_str());
        shared_ptr<GibbonSetSuspendedEvent> suspendEvent(new GibbonSetSuspendedEvent(value.value<bool>(), "UI_SCRIPT"));
#ifdef GIBBON_NO_THREADANIMATION
        app()->postEvent(suspendEvent);
#else
        Animation::postEvent(suspendEvent);
#endif
        break; }
    case Properties::ScreenBridge::overlay: {
        ScopedMutex _lock(Widget::lock());
        Widget::SharedPtr overlay;
        if(!value.isNull()) {
            shared_ptr<WidgetBridge> bridge = GibbonApplication::instance()->getBridge()->createWidget(value.value<int>());
            overlay = bridge->getWidget();
        }
        if(mOverlay != overlay) {
            if(mOverlay) {
                mOverlay->setParent(Widget::SharedPtr());
                mOverlay->purge(Widget::Purge_Force);
            }
            mOverlay = overlay;
            GibbonApplication::instance()->getScreen()->addOverlay(mOverlay);
        }
        break; }
    case Properties::ScreenBridge::root: {
        ScopedMutex _lock(Widget::lock());
        Widget::SharedPtr root;
        if(!value.isNull()) {
            shared_ptr<WidgetBridge> bridge = GibbonApplication::instance()->getBridge()->createWidget(value.value<int>());
            root = bridge->getWidget();
        }
        if(mRoot != root) {
            mRoot = root;
            GibbonApplication::instance()->getScreen()->setRoot(mRoot);
        }
        break; }
    default:
        return false;
    }
    return true;
}

Variant ScreenBridge::invoke(int method, const Variant &args)
{
    bool ok;
    switch(method) {
    case Methods::ScreenBridge::update: {
        const int id = args.mapValue<int>("id", &ok);
        if(ok) {
            ScopedMutex _lock(Widget::lock());
            if(Widget::SharedPtr w = GibbonApplication::instance()->getWidget(id))
                GibbonApplication::instance()->getScreen()->update(w->getGlobalRect());
        } else {
            GibbonApplication::instance()->getScreen()->update();
        }
        break; }
    case Methods::ScreenBridge::grab: {
        const int id = args.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["id"] = id;
        if(Surface::SharedPtr surface = GibbonApplication::instance()->getScreen()->grab()) {
            const DataBuffer grabBuffer = SurfaceDecoders::instance()->encode(surface);
            data["data"] = "data:image/png;base64," + grabBuffer.toBase64String();
        }
        sendEvent("grab", data);
        break; }
    default:
        return false;
    }
    return Variant();
}

unsigned ScreenBridge::requestFlags(const Request &request) const
{
    unsigned ret = GibbonBaseBridge::requestFlags(request);

    switch(request.type()) {
    case Request::Method:
        if(request.method() == Methods::ScreenBridge::grab)
            ret |= Request::Queued;
        break;
    case Request::SetProperty:
        ret |= Request::Queued;
        break;
    default:
        break;
    }
    return ret;
}
