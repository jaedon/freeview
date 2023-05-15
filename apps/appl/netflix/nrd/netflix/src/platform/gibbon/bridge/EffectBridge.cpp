/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EffectBridge.h"
#include "Widget.h"
#include "Debug.h"

#include <nrd/Request.h>
#include <nrdbase/StringUtils.h>

using namespace netflix::gibbon;
using namespace netflix;

#define EffectBridgePropertyList(Property)              \
    Property(accelerated, NfObject::ReadOnly)           \
    Property(params, NfObject::ReadWrite|DumpProperty)  \
    Property(visible, NfObject::ReadWrite|DumpProperty)

PropertyTable(EffectBridge, EffectBridgePropertyList, properties)

#define EffectBridgeMethodList(Method)          \
    Method(destroy)                             \
    Method(stopAnimation)

MethodTable(EffectBridge, EffectBridgeMethodList);

EffectBridge::EffectBridge(const std::string &name, Effect::SharedPtr effect)
    : GibbonBaseBridge(name, Properties::EffectBridge::properties, Properties::EffectBridge::PropertyCount,
                       Methods::EffectBridge::methods, Methods::EffectBridge::MethodCount, Flag_NoPropertySync),
      mAccelerated(-1), mEffect(effect)
{
}

EffectBridge::~EffectBridge()
{

}

void EffectBridge::init()
{
    mEffect->addListener(static_pointer_cast<EffectBridge>(shared_from_this()));
}

bool EffectBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::EffectBridge::accelerated:
        if(mEffect->getType() != Effect::Type_Unknown && mAccelerated != -1)
            *result = mAccelerated == 1;
        break;
    case Properties::EffectBridge::params:
        *result = mEffect->getParams();
        break;
    case Properties::EffectBridge::visible:
        *result = mEffect->getVisible();
        break;
    default:
        return false;
    }
    return true;
}

bool EffectBridge::setProperty(int index, const Variant &value)
{
    if(Debug::DebugWidgetProperties)
        Log::error(TRACE_LOG, "[%s:%d]: %s (%s->%s)", __func__, __LINE__, mEffect->describe().c_str(),
                                  propertyName(index), value.toJSON().c_str());
    switch(index) {
    case Properties::EffectBridge::params:
        if(!mEffect->setParams(value)) {
            NTRACE(TRACE_LOG, "[%s:%d]: Unable to apply effect(%s): %s", __func__, __LINE__,
                   mEffect->describe().c_str(), value.toJSON().c_str());
        }
        break;
    case Properties::EffectBridge::visible:
        mEffect->setVisible(value.value<bool>(0, mEffect->getVisible()));
        break;
    default:
        return false;
    }
    return true;
}

Variant EffectBridge::invoke(int method, const Variant &variant)
{
    switch (method) {
    case Methods::EffectBridge::stopAnimation: {
        const int property = mEffect->getAnimatedPropertyID(variant.mapValue<std::string>("property", 0));
        if(!property) {
            invalidArgumentError(method, "property");
            return false;
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
        Log::info(TRACE_GIBBON_ANIMATION, "Animation(%p): %d explicitly stopped[%d]", mEffect.get(), property, stopType);
#endif
        {
            ScopedMutex _lock(Widget::lock());
            Animation::stop(mEffect, property, stopType);
        }
        break; }
    case Methods::EffectBridge::destroy: {
        setParent(shared_ptr<NfObject>());
        break;
    }
    default:
        return false;
    }
    return Variant();
}

void EffectBridge::onEffectChanged(const Effect::SharedPtr &effect, const Variant &data)
{
    assert(effect == mEffect);
    bool sendEvent = !data.isNull();
    const bool accelerated = effect->getAccelerated();
    if((int)accelerated != mAccelerated) {
        sendEvent = true;
        mAccelerated = accelerated;
        propertyUpdated(Properties::EffectBridge::accelerated);
    }
    if(sendEvent)
        this->sendEvent("effectChanged", data);
}

void EffectBridge::onAnimationFinished(int property, const AnimationFinish &finish)
{
    Variant data;
    if(finish.frame.info.id != INT_MAX)
        data["id"] = finish.frame.info.id;
    if(finish.state == AnimationFinish::State_Aborted)
        data["aborted"] = true;
    if(!isnan(finish.fps))
        data["fps"] = finish.fps;
    data["value"] = finish.value;
    data["property"] = mEffect->getAnimatedPropertyName(property);
    sendEvent("animationFinished", data);
}
