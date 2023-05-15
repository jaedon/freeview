/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __EFFECTBRIDGE_H__
#define __EFFECTBRIDGE_H__

#include <Effects.h>
#include <GibbonBaseBridge.h>

#include <nrd/NfObject.h>
#include <nrd/Request.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class EffectBridge : public GibbonBaseBridge, public Effect::Listener
{
public:
    EffectBridge(const std::string &name, Effect::SharedPtr effect);
    ~EffectBridge();
    void init();

    inline Effect::SharedPtr getEffect() const { return mEffect; }
    inline int getID() const { return getEffect()->getID(); }

    virtual unsigned requestFlags(const Request &request) const
    {
        return GibbonBaseBridge::requestFlags(request) | Request::Queued;
    }
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);

    virtual ClassType classType() const { return EffectType; }
    virtual uint32_t classID() const { return getID(); }

    virtual void onEffectChanged(const Effect::SharedPtr &, const Variant &);
    virtual void onAnimationFinished(int, const AnimationFinish &);

protected:
    int mAccelerated;
    Effect::SharedPtr mEffect;
};

}} // namespace netflix::gibbon

#endif /* __EFFECTBRIDGE_H__ */
