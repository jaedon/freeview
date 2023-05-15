/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SURFACEEFFECTSDFB_H__
#define __SURFACEEFFECTSDFB_H__

#include <Effects.h>

namespace netflix {
namespace gibbon {

class EffectMaskDFB : public EffectMask
{
public:
    EffectMaskDFB() { }

    virtual bool getAccelerated() const { return true; }
    virtual std::string describe() const { return "DFB" + EffectMask::describe(); }

    class DFBRenderer : public DefaultRenderer {
    public:
        DFBRenderer(EffectMask::SharedPtr e) : DefaultRenderer(e) {}
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DFBRenderer(static_pointer_cast<EffectMask>(shared_from_this())));
    }
};

class EffectTransformDFB : public EffectTransform
{
public:
    EffectTransformDFB() { }

    virtual bool getAccelerated() const { return true; }
    virtual std::string describe() const { return "DFB" + EffectTransform::describe(); }

    class DFBRenderer : public DefaultRenderer {
    public:
        DFBRenderer(EffectTransform::SharedPtr e) : DefaultRenderer(e) {}
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DFBRenderer(static_pointer_cast<EffectTransform>(shared_from_this())));
    }
protected:
    virtual bool supportsPerspective() const { return false; }
};

}};

#endif /* __SURFACEEFFECTSDFB_H__ */
