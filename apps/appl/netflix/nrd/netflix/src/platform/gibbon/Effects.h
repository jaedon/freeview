/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <list>
#include <vector>

#include <Rect.h>
#include <Image.h>
#include <Surface.h>
#include <Transform.h>
#include <Animation.h>
#include <DisplayList.h>
#include <GraphicsEngine.h>

#include <nrdbase/Variant.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/WeakPtrSet.h>

NRDP_DECLARE_OBJECTCOUNT(GibbonEffect);

namespace netflix {
namespace gibbon {

class Effect : public AnimationObject
{
public:
    enum Type {
        Type_Blur,
        Type_Tint,
        Type_Mask,
        Type_Diff,
        Type_Transform,
        Type_Desaturate,
        Type_Unknown = 128
    };

    typedef shared_ptr<Effect> SharedPtr;
    typedef weak_ptr<Effect> WeakPtr;
    inline Effect(Type type, int id=-1) : mID(id), mType(type), mVisible(true) { NRDP_OBJECTCOUNT_REF(GibbonEffect, this); }
    virtual ~Effect() { NRDP_OBJECTCOUNT_DEREF(GibbonEffect, this); }

    static Type getType(const std::string &type);
    static std::string describeType(Type type);

    static Effect::SharedPtr create(int id, const Type type);
    static Effect::SharedPtr create(int id, const std::string &type)  { return create(id, getType(type)); }

    inline int getID() const { return mID; }

    inline void setVisible(bool v) { mVisible = v; onEffectChanged(); }
    inline bool getVisible() const { return mVisible; }

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class Listener
    {
    public:
        typedef shared_ptr<Listener> SharedPtr;
        typedef weak_ptr<Listener> WeakPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onEffectChanged(const Effect::SharedPtr &, const Variant &) { }
        virtual void onAnimationFinished(int, const AnimationFinish &) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);
    void onEffectChanged(const Variant &data=Variant());

    class Renderer {
    public:
        typedef shared_ptr<Renderer> SharedPtr;
        typedef weak_ptr<Renderer> WeakPtr;
        Renderer() {}
        virtual ~Renderer() {}

        virtual std::string describe() const { return std::string(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &) const { return dl; }
        virtual Rect getRect(const Rect &rect) const { return rect; }
        virtual bool getAccelerated() const { return false; }
        virtual unsigned int purge() { return 0; }
    protected:
        Surface::SharedPtr requestSurface(Coordinate width, Coordinate height, unsigned int flags=Surface::Flags_Alpha|Surface::Flags_RenderTarget) const;
        inline Surface::SharedPtr requestSurface(const Size &size, unsigned int flags=Surface::Flags_Alpha|Surface::Flags_RenderTarget) const
        { return requestSurface(size.width, size.height, flags); }
    };
    virtual Renderer::SharedPtr getDefaultRenderer() { return Renderer::SharedPtr(new Renderer()); }
    virtual bool getAccelerated() const { return false; }
    virtual unsigned int purge() { return 0; }

    enum {
        Property_Maximum = 1
    };
    virtual std::string describe() const {
        std::ostringstream str;
        str << "Effect(" << describeType(getType()) << ":" << getVisible() << "):" << getParams().toJSON();
        return str.str();
    }
    virtual std::string getAnimatedPropertyName(int) const { return std::string(); }
    virtual int getAnimatedPropertyID(const std::string &) const { return 0; }
    virtual bool isIntegralProperty(int) const { return false; }
    virtual void endAnimatingProperty(int, const AnimationFinish &finish);

    virtual float getAnimatedProperty(int) const { return NAN; }
    virtual bool setAnimatedProperty(int, float) { return false; }

    inline Type getType() const { return mType; }

protected:
    bool setAnimatedParam(int key, const Variant &value);

private:
    int mID;
    Type mType;
    bool mVisible;

    typedef WeakPtrSet<Listener> ListenerContainer;
    mutable ListenerContainer mListeners;
};

typedef std::list<shared_ptr<Effect> > Effects;
typedef std::list<shared_ptr<Effect::Renderer> > EffectRenderers;

class EffectBlur : public Effect
{
public:
    typedef shared_ptr<EffectBlur> SharedPtr;
    typedef weak_ptr<EffectBlur> WeakPtr;
    inline EffectBlur() : Effect(Type_Blur), mRadius(0) { }

    inline void setRadius(int radius) { mRadius = radius; onEffectChanged(); }
    inline int getRadius() const { return mRadius; }

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectBlur::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
        virtual Rect getRect(const Rect &rect) const;
    protected:
        EffectBlur::SharedPtr mEffect;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectBlur>(shared_from_this())));
    }

    enum {
        Property_Radius = Effect::Property_Maximum,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual bool isIntegralProperty(int) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

protected:
    int mRadius;
};

class EffectMask : public Effect, public Image::Listener
{
public:
    typedef shared_ptr<EffectMask> SharedPtr;
    typedef weak_ptr<EffectMask> WeakPtr;
    inline EffectMask() : Effect(Type_Mask) { }
    virtual std::string describe() const;

    inline bool hasImage() const { return (bool)mImage; }
    inline float getImageScale() const { return hasImage() ? mImage->scale() : 1.0f; }
    inline Size getImageSize() const { return hasImage() ? mImage->scaledSizeContent() : Size(0, 0); }

    inline Point getOrigin() const { return mOrigin; }
    Surface::SharedPtr getMask() const;
    virtual unsigned int purge();

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectMask::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
        virtual Rect getRect(const Rect &rect) const;
    protected:
        EffectMask::SharedPtr mEffect;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectMask>(shared_from_this())));
    }

    enum {
        Property_X = Effect::Property_Maximum,
        Property_Y,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

protected:
    virtual void onImageLoaded(const Image::SharedPtr &, bool);
    Image::SharedPtr mImage;
    Point mOrigin;
};

class EffectDiff : public Effect, public Image::Listener
{
public:
    typedef shared_ptr<EffectDiff> SharedPtr;
    typedef weak_ptr<EffectDiff> WeakPtr;
    inline EffectDiff() : Effect(Type_Diff), mMatch(0), mColor(255, 0, 0), mDim(1), mDump(false) { }
    virtual std::string describe() const;

    inline double getDim() const { return mDim; }
    inline uint8_t getMatch() const { return mMatch; }
    inline bool shouldDump() const { return mDump; }
    inline Color getColor() const { return mColor; }

    inline bool hasImage() const { return (bool)mImage; }
    inline float getImageScale() const { return hasImage() ? mImage->scale() : 1.0f; }
    inline Size getImageSize() const { return hasImage() ? mImage->scaledSizeContent() : Size(0, 0); }

    Surface::SharedPtr getImage() const;
    virtual unsigned int purge();

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectDiff::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
        virtual Rect getRect(const Rect &rect) const;
    protected:
        EffectDiff::SharedPtr mEffect;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectDiff>(shared_from_this())));
    }

    enum {
        Property_Red = Effect::Property_Maximum,
        Property_Green,
        Property_Blue,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

protected:
    virtual void onImageLoaded(const Image::SharedPtr &, bool);
    Image::SharedPtr mImage;
    uint8_t mMatch;
    Color mColor;
    double mDim;
    bool mDump;
};

class EffectTransform : public Effect
{
public:
    typedef shared_ptr<EffectTransform> SharedPtr;
    typedef weak_ptr<EffectTransform> WeakPtr;
    inline EffectTransform() : Effect(Type_Transform), mBackFace(true), mDistance(-1.74f) { }

    inline float getShowBackFace() const { return mBackFace; }
    inline float getDistance() const { return mDistance; }

    inline void setTransform(const Transform &transform) { mTransform = transform; onEffectChanged(); }
    inline Transform getTransform() const { return mTransform; }
    Transform applyTransform(const Rect &rect) const;

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectTransform::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual Rect getRect(const Rect &rect) const;
    protected:
        EffectTransform::SharedPtr mEffect;
        virtual bool supportsPerspective() const { return true; }
        Transform getTransform() const;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectTransform>(shared_from_this())));
    }

    enum {
        Property_TranslateX = Effect::Property_Maximum,
        Property_TranslateY,
        Property_ScaleX,
        Property_ScaleY,
        Property_RotateX,
        Property_RotateY,
        Property_RotateZ,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

private:
    bool mBackFace;
    float mDistance;
    Transform mTransform;
    std::map<int, float> mAnimatedProperties;
};

class EffectTint : public Effect
{
public:
    typedef shared_ptr<EffectTint> SharedPtr;
    typedef weak_ptr<EffectTint> WeakPtr;
    inline EffectTint() : Effect(Type_Tint), mRed(0.0f), mGreen(0.0f), mBlue(0.0f) { }

    inline float getRed() const { return mRed; }
    inline float getGreen() const { return mGreen; }
    inline float getBlue() const { return mBlue; }

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectTint::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
    protected:
        EffectTint::SharedPtr mEffect;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectTint>(shared_from_this())));
    }

    enum {
        Property_Red = Effect::Property_Maximum,
        Property_Green,
        Property_Blue,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

private:
    float mRed, mGreen, mBlue;
};

class EffectDesaturate : public Effect
{
public:
    typedef shared_ptr<EffectDesaturate> SharedPtr;
    typedef weak_ptr<EffectDesaturate> WeakPtr;
    inline EffectDesaturate() : Effect(Type_Desaturate), mRed(1.0f), mGreen(1.0f), mBlue(1.0f) { }

    inline float getRed() const { return mRed; }
    inline float getGreen() const { return mGreen; }
    inline float getBlue() const { return mBlue; }

    virtual Variant getParams() const;
    virtual bool setParams(const Variant &params);

    class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer(const EffectDesaturate::SharedPtr &e) : mEffect(e) {}
        virtual std::string describe() const { return mEffect->describe(); }
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
    protected:
        EffectDesaturate::SharedPtr mEffect;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new DefaultRenderer(static_pointer_cast<EffectDesaturate>(shared_from_this())));
    }

    enum {
        Property_Red = Effect::Property_Maximum,
        Property_Green,
        Property_Blue,
        Property_Maximum
    };
    virtual std::string getAnimatedPropertyName(int) const;
    virtual int getAnimatedPropertyID(const std::string &) const;
    virtual float getAnimatedProperty(int) const;
    virtual bool setAnimatedProperty(int, float);

private:
    float mRed, mGreen, mBlue;
};

}} // namespace netflix::gibbon

#endif /* __EFFECTS_H__ */
