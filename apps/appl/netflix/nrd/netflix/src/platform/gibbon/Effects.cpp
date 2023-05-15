/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Screen.h"
#include "Effects.h"
#include "GraphicsEngine.h"
#include "SurfaceCache.h"
#include "WidgetBridge.h"
#include "SurfaceDecoder.h"
#include "GibbonConfiguration.h"

#include <nrdbase/Log.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonEffect);

template<typename T> const T& range(const T &lower, const T&value, const T &upper)
{
    return std::min(std::max(value, lower), upper);
}

void Effect::addListener(const Listener::SharedPtr &listener)
{
    mListeners.insert(listener);
}

void Effect::removeListener(const Listener::SharedPtr &listener)
{
    mListeners.erase(listener);
}

void Effect::endAnimatingProperty(int property, const AnimationFinish &finish)
{
    for(ListenerContainer::iterator it = mListeners.begin(); it != mListeners.end(); ) {
        if(Listener::SharedPtr curListener = it->lock()) {
            curListener->onAnimationFinished(property, finish);
            ++it;
        } else {
            mListeners.erase(it++);
        }
    }
}

void Effect::onEffectChanged(const Variant &data)
{
    for(ListenerContainer::iterator it = mListeners.begin(); it != mListeners.end(); ) {
        if(Listener::SharedPtr curListener = it->lock()) {
            curListener->onEffectChanged(static_pointer_cast<Effect>(shared_from_this()), data);
            ++it;
        } else {
            mListeners.erase(it++);
        }
    }
}

Variant Effect::getParams() const
{
    Variant result;
    if(!mVisible)
        result["visible"] = mVisible;
    return result;
}

bool Effect::setParams(const Variant &params)
{
    setVisible(params["visible"].value<bool>(0, mVisible));
    return true;
}

bool Effect::setAnimatedParam(int property, const Variant &value)
{
    if(value.isNull()) {
        return setAnimatedProperty(property, NAN);
    } else if(value.isStringMap()) {
        const Variant animate = value["animate"];
        if(animate.isStringMap() || animate.isArray()) {
            std::vector<Variant> animates;
            if(animate.isStringMap())
                animates.push_back(animate);
            else
                animates = animate.array();
            for(Variant::ArrayIterator it = animates.begin(); it != animates.end(); ++it) {
                const Variant &animate = (*it);
                const double end = animate.mapValue<double>("end", 0, NAN);
                double start = NAN;
                const Variant start_v = animate["start"];
                if(start_v.isNull()) {
                    if(isnan(Animation::getValue(shared_from_this(), property)))
                        start = animate.mapValue<double>("previous", 0, NAN);
                } else if(start_v.isStringMap()) {
                    start = start_v["value"].value<double>(0, NAN);
                } else {
                    start = start_v.value<double>(0, NAN);
                }
                const Animation::Info info(animate);
                const bool append = animate.mapValue<bool>("append", 0, false);
                Animation::start(shared_from_this(), property, start, end, info, append);
            }
        }
    } else {
        bool ok;
        const float value_f = value.value<double>(&ok);
        if(ok)
            setAnimatedProperty(property, value_f);
    }
    return true;
}

static struct {
    const char *name;
    Effect::Type type;
} sEffectTypes[] = {
    { "blur", Effect::Type_Blur, },
    { "tint", Effect::Type_Tint, },
    { "mask", Effect::Type_Mask },
    { "diff", Effect::Type_Diff, },
    { "transform", Effect::Type_Transform },
    { "desaturate", Effect::Type_Desaturate, },
    { 0, Effect::Type_Unknown },
};

Effect::Type Effect::getType(const std::string &type)
{
    for (int i = 0; sEffectTypes[i].name; ++i) {
        if(type == sEffectTypes[i].name)
            return sEffectTypes[i].type;
    }
    return Type_Unknown;
}

std::string Effect::describeType(Type type)
{
    for (int i = 0; sEffectTypes[i].name; ++i) {
        if(sEffectTypes[i].type == type)
            return sEffectTypes[i].name;
    }
    assert("Unknown effect type" && 0);
    return "unknown";
}

Effect::SharedPtr Effect::create(int id, Type type)
{
    Effect::SharedPtr effect = Screen::createEffect(type);
    if(!effect) {
        switch(type) {
        case Type_Diff:
            effect.reset(new EffectDiff);
            break;
        case Type_Blur:
            effect.reset(new EffectBlur);
            break;
        case Type_Mask:
            effect.reset(new EffectMask);
            break;
        case Type_Tint:
            effect.reset(new EffectTint);
            break;
        case Type_Desaturate:
            effect.reset(new EffectDesaturate);
            break;
        default:
            break;
        }
    }
    if(!effect) { //no-op effect
        Log::error(TRACE_LOG, "Unable to create effect of type: %s(%d)", describeType(type).c_str(), type);
        effect.reset(new Effect(Type_Unknown));
    }
    effect->mID = id;
    return effect;
}

Surface::SharedPtr Effect::Renderer::requestSurface(Coordinate width, Coordinate height, unsigned int flags) const
{
    Surface::SharedPtr surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(width, height, flags, false);
    surface->setDescription(describe());
    return surface;
}

Rect EffectBlur::DefaultRenderer::getRect(const Rect &rect) const
{
    const int radius = mEffect->getRadius();
    if(radius <= 0)
        return rect;
    return Rect(rect.x-radius, rect.y-radius, rect.width+(radius*2), rect.height+(radius*2));
}

DisplayList::SharedPtr EffectBlur::DefaultRenderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    const int radius = mEffect->getRadius();
    if(radius <= 0)
        return dl;
    Surface::SharedPtr surface = dl->flatten();
    if(!surface)
        return dl;

    const Rect dstRect(getRect(area));
    Surface::SharedPtr result = requestSurface(dstRect.width, dstRect.height);
    {
        GraphicsEngine engine(result);
        engine.clear(result->getRect());
        engine.blit(surface, area, Rect(radius, radius, area.width, area.height));
    }

    // Stack Blur Algorithm by Mario Klingemann <mario@quasimondo.com>
    Surface::Data data(result, Surface::Data::ReadWriteLock);
    int w   = (int)data.getWidth();
    int h   = (int)data.getHeight();
    int wm  = w-1;
    int hm  = h-1;
    int wh  = w*h;
    int div = radius+radius+1;

    int *r = new int[wh];
    int *g = new int[wh];
    int *b = new int[wh];
    int *a = new int[wh];
    int rsum, gsum, bsum, asum, yp, yi;
    Color p;
    int *vmin = new int[std::max(w,h)];

    int divsum = (div+1)>>1;
    divsum *= divsum;
    int *dv = new int[256*divsum];
    for(int i=0; i < 256*divsum; ++i)
        dv[i] = (i/divsum);

    yi = 0;

    int **stack = new int*[div];
    for(int i = 0; i < div; ++i)
        stack[i] = new int[4];

    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius+1;
    int routsum, goutsum, boutsum, aoutsum;
    int rinsum, ginsum, binsum, ainsum;

    for(int y = 0; y < h; ++y){
        rinsum = ginsum = binsum = ainsum
               = routsum = goutsum = boutsum = aoutsum
               = rsum = gsum = bsum = asum = 0;
        for(int i = -radius; i <= radius; ++i) {
            p = data.getPixel(std::min(wm, std::max(i, 0)), y);
            sir = stack[i+radius];
            sir[0] = p.r;
            sir[1] = p.g;
            sir[2] = p.b;
            sir[3] = p.a;

            rbs = r1-abs(i);
            rsum += sir[0]*rbs;
            gsum += sir[1]*rbs;
            bsum += sir[2]*rbs;
            asum += sir[3]*rbs;

            if (i > 0){
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }
        }
        stackpointer = radius;

        for(int x = 0; x < w; ++x) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            a[yi] = dv[asum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if(y == 0)
                vmin[x] = std::min(x+radius+1,wm);
            p = data.getPixel(vmin[x], y);

            sir[0] = p.r;
            sir[1] = p.g;
            sir[2] = p.b;
            sir[3] = p.a;

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[(stackpointer)%div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            ++yi;
        }
    }
    for(int x = 0; x < w; ++x) {
        rinsum = ginsum = binsum = ainsum
               = routsum = goutsum = boutsum = aoutsum
               = rsum = gsum = bsum = asum = 0;

        yp =- radius * w;

        for(int i = -radius; i <= radius; ++i) {
            yi=std::max(0,yp)+x;

            sir = stack[i+radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            sir[3] = a[yi];

            rbs = r1-abs(i);

            rsum += r[yi]*rbs;
            gsum += g[yi]*rbs;
            bsum += b[yi]*rbs;
            asum += a[yi]*rbs;

            if(i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }

            if(i < hm)
                yp += w;
        }

        yi = x;
        stackpointer = radius;

        for(int y=0; y < h; ++y) {
            data.setPixel(x, y, Color(dv[rsum], dv[gsum], dv[bsum], dv[asum]));

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if(x==0)
                vmin[y] = std::min(y+r1,hm)*w;

            const int rem = x+vmin[y];
            sir[0] = r[rem];
            sir[1] = g[rem];
            sir[2] = b[rem];
            sir[3] = a[rem];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            yi += w;
        }
    }
    delete [] r;
    delete [] g;
    delete [] b;
    delete [] a;
    delete [] vmin;
    delete [] dv;
    return DisplayList::SharedPtr(new DisplayList(result));
}

Variant EffectBlur::getParams() const
{
    Variant result = Effect::getParams();
    result["radius"] = mRadius;
    return result;
}

bool EffectBlur::setParams(const Variant &params)
{
    if(!setAnimatedParam(Property_Radius, params["radius"]))
        return false;
    return Effect::setParams(params);
}

bool EffectBlur::isIntegralProperty(int property) const
{
    if(property == Property_Radius)
        return true;
    return Effect::isIntegralProperty(property);
}

std::string EffectBlur::getAnimatedPropertyName(int property) const
{
    if(property == Property_Radius)
        return "radius";
    return Effect::getAnimatedPropertyName(property);
}

int EffectBlur::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "radius")
        return Property_Radius;
    return Effect::getAnimatedPropertyID(property);
}

float EffectBlur::getAnimatedProperty(int property) const
{
    if(property == Property_Radius)
        return mRadius;
    return NAN;
}

bool EffectBlur::setAnimatedProperty(int property, float value)
{
    if(property == Property_Radius) {
        const int rvalue = isnan(value) ? 0 : static_cast<int>(value);
        if(mRadius != rvalue) {
            mRadius = rvalue;
            onEffectChanged();
        }
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}

DisplayList::SharedPtr EffectDesaturate::DefaultRenderer::render(const DisplayList::SharedPtr &dl, const Rect &) const
{
    Surface::SharedPtr surface = dl->flatten();
    Surface::Data data(surface, Surface::Data::ReadWriteLock);
    const int width = static_cast<int>(data.getWidth());
    const int height = static_cast<int>(data.getHeight());
    const float red = mEffect->getRed(), green = mEffect->getGreen(), blue = mEffect->getBlue();
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            const Color c = data.getPixel(x, y);
            if(c.a) {
                const unsigned i = (unsigned)((c.r * 0.3f) + (c.g * 0.59f) + (c.b * 0.11f));
                data.setPixel(x, y, Color(range(0, int(i * red + c.r * (1-red)), 255),
                                          range(0, int(i * green + c.g * (1-green)), 255),
                                          range(0, int(i * blue + c.b * (1-blue)), 255), c.a));
            }
        }
    }
    return DisplayList::SharedPtr(new DisplayList(surface));
}

Variant EffectDesaturate::getParams() const
{
    Variant result = Effect::getParams();
    result["r"] = mRed;
    result["g"] = mGreen;
    result["b"] = mBlue;
    return result;
}

bool EffectDesaturate::setParams(const Variant &params)
{
    if(!setAnimatedParam(Property_Red, params["r"]))
        return false;
    if(!setAnimatedParam(Property_Green, params["g"]))
        return false;
    if(!setAnimatedParam(Property_Blue, params["b"]))
        return false;
    return Effect::setParams(params);
}

std::string EffectDesaturate::getAnimatedPropertyName(int property) const
{
    if(property == Property_Red)
        return "r";
    else if(property == Property_Green)
        return "g";
    else if(property == Property_Blue)
        return "b";
    return Effect::getAnimatedPropertyName(property);
}

int EffectDesaturate::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "r")
        return Property_Red;
    else if(property == "g")
        return Property_Green;
    else if(property == "b")
        return Property_Blue;
    return Effect::getAnimatedPropertyID(property);
}

float EffectDesaturate::getAnimatedProperty(int property) const
{
    if(property == Property_Red)
        return mRed;
    else if(property == Property_Green)
        return mGreen;
    else if(property == Property_Blue)
        return mBlue;
    return Effect::getAnimatedProperty(property);
}

bool EffectDesaturate::setAnimatedProperty(int property, float value)
{
    if(property == Property_Red) {
        if(!isnan(value)) {
            value = std::max(0.0f, std::min(value, 1.0f));
            if(mRed != value) {
                mRed = value;
                onEffectChanged();
            }
        }
        return true;
    } else if(property == Property_Green) {
        if(!isnan(value)) {
            value = std::max(0.0f, std::min(value, 1.0f));
            if(mGreen != value) {
                mGreen = value;
                onEffectChanged();
            }
        }
        return true;
    } else if(property == Property_Blue) {
        if(!isnan(value)) {
            value = std::max(0.0f, std::min(value, 1.0f));
            if(mBlue != value) {
                mBlue = value;
                onEffectChanged();
            }
        }
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}

DisplayList::SharedPtr EffectTint::DefaultRenderer::render(const DisplayList::SharedPtr &dl, const Rect &) const
{
    Surface::SharedPtr surface = dl->flatten();
    {
        Surface::Data data(surface, Surface::Data::ReadWriteLock);
        const int width = static_cast<int>(data.getWidth());
        const int height = static_cast<int>(data.getHeight());
        const float red = mEffect->getRed(), green = mEffect->getGreen(), blue = mEffect->getBlue();
        for(int x = 0; x < width; ++x) {
            for(int y = 0; y < height; ++y) {
                const Color c = data.getPixel(x, y);
                if(c.a)
                    data.setPixel(x, y, Color(range(0, int(c.r * red), 255),
                                              range(0, int(c.g * green), 255),
                                              range(0, int(c.b * blue), 255), c.a));
            }
        }
    }
    surface->upload();
    return DisplayList::SharedPtr(new DisplayList(surface));
}

Variant EffectTint::getParams() const
{
    Variant result = Effect::getParams();
    result["r"] = mRed;
    result["g"] = mGreen;
    result["b"] = mBlue;
    return result;
}

bool EffectTint::setParams(const Variant &params)
{
    if(!setAnimatedParam(Property_Red, params["r"]))
        return false;
    if(!setAnimatedParam(Property_Green, params["g"]))
        return false;
    if(!setAnimatedParam(Property_Blue, params["b"]))
        return false;
    return Effect::setParams(params);
}

std::string EffectTint::getAnimatedPropertyName(int property) const
{
    if(property == Property_Red)
        return "r";
    else if(property == Property_Green)
        return "g";
    else if(property == Property_Blue)
        return "b";
    return Effect::getAnimatedPropertyName(property);
}

int EffectTint::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "r")
        return Property_Red;
    else if(property == "g")
        return Property_Green;
    else if(property == "b")
        return Property_Blue;
    return Effect::getAnimatedPropertyID(property);
}

float EffectTint::getAnimatedProperty(int property) const
{
    if(property == Property_Red)
        return mRed;
    else if(property == Property_Green)
        return mGreen;
    else if(property == Property_Blue)
        return mBlue;
    return Effect::getAnimatedProperty(property);
}

bool EffectTint::setAnimatedProperty(int property, float value)
{
    if(property == Property_Red) {
        if(mRed != value && !isnan(value)) {
            mRed = value;
            onEffectChanged();
        }
        return true;
    } else if(property == Property_Green) {
        if(mGreen != value && !isnan(value)) {
            mGreen = value;
            onEffectChanged();
        }
        return true;
    } else if(property == Property_Blue) {
        if(mBlue != value && !isnan(value)) {
            mBlue = value;
            onEffectChanged();
        }
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}

Variant EffectTransform::getParams() const
{
    Variant result = Effect::getParams();
    Variant transform;
    for(int x = 0; x < 4; ++x) {
        for(int y = 0; y < 4; ++y)
            transform[x][y] = mTransform.matrix(x, y);
    }
    result["visibleBack"] = mBackFace;
    result["transform"] = transform;
    result["distance"] = mDistance;
    return result;
}

bool EffectTransform::setParams(const Variant &params)
{
    Variant transform = params["transform"];
    if(!transform.isNull()) {
        for(int x = 0; x < 4; ++x) {
            for(int y = 0; y < 4; ++y)
                mTransform.setMatrix(x, y, transform[x].mapValue<double>(y, 0, mTransform.matrix(x, y)));
        }
    }
    mDistance = params.mapValue<double>("distance", 0, mDistance);
    mBackFace = params.mapValue<double>("visibleBack", 0, mBackFace);
    if(!setAnimatedParam(Property_TranslateX, params["translateX"]))
        return false;
    if(!setAnimatedParam(Property_TranslateY, params["translateY"]))
        return false;
    if(!setAnimatedParam(Property_ScaleX, params["scaleX"]))
        return false;
    if(!setAnimatedParam(Property_ScaleY, params["scaleY"]))
        return false;
    if(!setAnimatedParam(Property_RotateX, params["rotateX"]))
        return false;
    if(!setAnimatedParam(Property_RotateY, params["rotateY"]))
        return false;
    if(!setAnimatedParam(Property_RotateZ, params["rotateZ"]))
        return false;
    return Effect::setParams(params);
}

std::string EffectTransform::getAnimatedPropertyName(int property) const
{
    if(property == Property_TranslateX)
        return "translateX";
    else if(property == Property_TranslateY)
        return "translateY";
    else if(property == Property_ScaleX)
        return "scaleX";
    else if(property == Property_ScaleY)
        return "scaleY";
    else if(property == Property_RotateX)
        return "rotateX";
    else if(property == Property_RotateY)
        return "rotateY";
    else if(property == Property_RotateZ)
        return "rotateZ";
    return Effect::getAnimatedPropertyName(property);
}

int EffectTransform::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "translateX")
        return Property_TranslateX;
    else if(property == "translateY")
        return Property_TranslateY;
    else if(property == "scaleX")
        return Property_ScaleX;
    else if(property == "scaleY")
        return Property_ScaleY;
    else if(property == "rotateX")
        return Property_RotateX;
    else if(property == "rotateY")
        return Property_RotateY;
    else if(property == "rotateZ")
        return Property_RotateZ;
    return Effect::getAnimatedPropertyID(property);
}

float EffectTransform::getAnimatedProperty(int property) const
{
    std::map<int, float>::const_iterator it = mAnimatedProperties.find(property);
    if(it != mAnimatedProperties.end())
        return (*it).second;
    return Effect::getAnimatedProperty(property);
}

bool EffectTransform::setAnimatedProperty(int property, float value)
{
    if(property >= Property_TranslateX && property < Property_Maximum) {
        if(isnan(value)) {
            std::map<int, float>::iterator it = mAnimatedProperties.find(property);
            if(it != mAnimatedProperties.end())
                mAnimatedProperties.erase(it);
        } else {
            mAnimatedProperties[property] = value;
        }
        onEffectChanged();
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}

Transform EffectTransform::DefaultRenderer::getTransform() const
{
    if(supportsPerspective()) {
        Transform transform;
        transform.perspective(60.0f, 1.0, 0.0f, 30.0f);
        transform.translate(0, 0, mEffect->getDistance());
        transform *= mEffect->applyTransform(Rect());
        return transform;
    }
    return mEffect->applyTransform(Rect());
}

Transform EffectTransform::applyTransform(const Rect &rect) const
{
    Transform transform = getTransform();
    if(!mAnimatedProperties.empty()) {
        if(!rect.empty())
            transform.translate(rect.x+(float(rect.width)/2), rect.y+(float(rect.height)/2), 0);
        for(std::map<int, float>::const_iterator it = mAnimatedProperties.begin(); it != mAnimatedProperties.end(); ++it) {
            switch((*it).first) {
            case Property_TranslateX:
                transform.translate((*it).second, 1.0f, 0.0f);
                break;
            case Property_TranslateY:
                transform.translate(0.0f, (*it).second, 1.0f);
                break;
            case Property_ScaleX:
                transform.scale((*it).second, 1.0f, 1.0f);
                break;
            case Property_ScaleY:
                transform.scale(1.0f, (*it).second, 1.0f);
                break;
            case Property_RotateX:
                transform.rotate((*it).second, 1.0f, 0.0f, 0.0f);
                break;
            case Property_RotateY:
                transform.rotate((*it).second, 0.0f, 1.0f, 0.0f);
                break;
            case Property_RotateZ:
                transform.rotate((*it).second, 0.0f, 0.0f, 1.0f);
                break;
            }
        }
        if(!rect.empty())
            transform.translate(-(rect.x+(float(rect.width)/2)), -(rect.y+(float(rect.height)/2)), 0);
    }
    return transform;
}

Rect EffectTransform::DefaultRenderer::getRect(const Rect &rect) const
{
#if 0
    return rect;
#endif

    const Transform transform = mEffect->applyTransform(rect);
    return transform.map(rect).normalized();
}

Variant EffectMask::getParams() const
{
    Variant result = Effect::getParams();
    if(mImage)
        result["image"]["url"] = mImage->getUrl().str();
    if(mOrigin.x != CoordinateMax)
        result["sourceX"] = mOrigin.x;
    if(mOrigin.y != CoordinateMax)
        result["sourceY"] = mOrigin.y;
    return result;
}

bool EffectMask::setParams(const Variant &params)
{
    if(params.contains("image")) {
        const Variant value = params["image"];
        if(!mImage) {
            mImage.reset(new Image);
            mImage->addListener(static_pointer_cast<EffectMask>(shared_from_this()));
        }
        mImage->load(Image::createSource(value));
    }
    if(!setAnimatedParam(Property_X, params["sourceX"]))
        return false;
    if(!setAnimatedParam(Property_Y, params["sourceY"]))
        return false;
    return Effect::setParams(params);
}

std::string EffectMask::getAnimatedPropertyName(int property) const
{
    if(property == Property_X)
        return "sourceX";
    else if(property == Property_Y)
        return "sourceY";
    return Effect::getAnimatedPropertyName(property);
}

int EffectMask::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "sourceX")
        return Property_X;
    else if(property == "sourceY")
        return Property_Y;
    return Effect::getAnimatedPropertyID(property);
}

float EffectMask::getAnimatedProperty(int property) const
{
    if(property == Property_X)
        return mOrigin.x;
    else if(property == Property_Y)
        return mOrigin.y;
    return Effect::getAnimatedProperty(property);
}

bool EffectMask::setAnimatedProperty(int property, float value)
{
    if(property == Property_X) {
        if(mOrigin.x != value) {
            mOrigin.x = isnan(value) ? CoordinateMax : value;
            onEffectChanged();
        }
        return true;
    } else if(property == Property_Y) {
        if(mOrigin.y != value) {
            mOrigin.y = isnan(value) ? CoordinateMax : value;
            onEffectChanged();
        }
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}

Rect EffectMask::DefaultRenderer::getRect(const Rect &rect) const
{
    if(mEffect->hasImage()) {
        Size imageSize(mEffect->getImageSize());
        const Point origin(mEffect->getOrigin());
        if(origin.x != CoordinateMax)
            imageSize.width -= origin.x;
        if(origin.y != CoordinateMax)
            imageSize.height -= origin.y;
        return Rect(rect.x, rect.y,
                    std::max(static_cast<Coordinate>(0), std::min(rect.width, imageSize.width)),
                    std::max(static_cast<Coordinate>(0), std::min(rect.height, imageSize.height)));
    }
    return Rect();
}

Surface::SharedPtr EffectMask::getMask() const
{
    if(mImage) {
        ScopedMutex _lock(Widget::lock());
        mImage->reload();
        return mImage->getSurface();
    }
    return Surface::SharedPtr();
}

unsigned int EffectMask::purge()
{
    if(mImage)
        return mImage->purge();
    return 0;
}

std::string EffectMask::describe() const
{
    std::ostringstream stream;
    stream << Effect::describe();
    if(mImage) {
        stream << ":" << mImage.get() << ":";
        if(mImage->loadFlags() & SurfaceLoader::Load_LazyDownload)
            stream << "lazydownload:";
        if(mImage->loadFlags() & SurfaceLoader::Load_LazyDecode)
            stream << "lazydecode:";
        if(mImage->hasFailed())
            stream << "failed:";
        if(!mImage->canPurge())
            stream << "nopurge:";
        stream << mImage->getSurface().get();
    }
    return stream.str();
}

DisplayList::SharedPtr EffectMask::DefaultRenderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    Surface::SharedPtr surface = dl->flatten();
    if(Surface::SharedPtr mask = mEffect->getMask()) {
        {
            const Point origin(mEffect->getOrigin());
            const float scale = mEffect->getImageScale();
            Surface::Data in_data(mask, Surface::Data::ReadLock);
            Surface::Data out_data(surface, Surface::Data::ReadWriteLock);
            const int width = static_cast<int>(area.width);
            const int height = static_cast<int>(area.height);
            const int originX = (int)(origin.x == CoordinateMax ? 0 : origin.x),
                      originY = (int)(origin.y == CoordinateMax ? 0 : origin.y);
            for(int out_x = (int)area.x; out_x < width; ++out_x) {
                const int in_x = (int)((out_x + originX) / scale);
                for(int out_y = (int)area.y; out_y < height; ++out_y) {
                    const int in_y = (int)((out_y + originY) / scale);
                    const Color mask_c = (in_x < in_data.getWidth() && in_y < in_data.getHeight()) ? in_data.getPixel(in_x, in_y) : Color(0, 0, 0, 0);
                    Color c = out_data.getPixel(out_x, out_y);
                    if(c.a) {
                        c.a = mask_c.a;
                        out_data.setPixel(out_x, out_y, c);
                    }
                }
            }
        }
        surface->upload();
        return DisplayList::SharedPtr(new DisplayList(surface));
    }
    return DisplayList::SharedPtr();
}

void EffectMask::onImageLoaded(const Image::SharedPtr &image, bool loaded)
{
    Variant data;
    if(loaded)
        data["image"] = ImageBridge::imageLoadProperties(image);
    onEffectChanged(data);
}

std::string EffectDiff::describe() const
{
    std::ostringstream stream;
    stream << Effect::describe();
    if(mImage) {
        stream << ":" << mImage.get() << ":";
        if(mImage->loadFlags() & SurfaceLoader::Load_LazyDownload)
            stream << "lazydownload:";
        if(mImage->loadFlags() & SurfaceLoader::Load_LazyDecode)
            stream << "lazydecode:";
        if(mImage->hasFailed())
            stream << "failed:";
        if(!mImage->canPurge())
            stream << "nopurge:";
        stream << mImage->getSurface().get();
    }
    return stream.str();
}

Surface::SharedPtr EffectDiff::getImage() const
{
    if(mImage) {
        ScopedMutex _lock(Widget::lock());
        mImage->reload();
        return mImage->getSurface();
    }
    return Surface::SharedPtr();
}

Variant EffectDiff::getParams() const
{
    Variant result = Effect::getParams();
    if(mImage)
        result["image"]["url"] = mImage->getUrl().str();
    result["match"] = mMatch;
    result["r"] = mColor.r;
    result["g"] = mColor.g;
    result["b"] = mColor.b;
    result["dim"] = mDim;
    return result;
}

bool EffectDiff::setParams(const Variant &params)
{
    if(!setAnimatedParam(Property_Red, params["r"]))
        return false;
    if(!setAnimatedParam(Property_Green, params["g"]))
        return false;
    if(!setAnimatedParam(Property_Blue, params["b"]))
        return false;
    if(params.contains("image")) {
        const Variant value = params["image"];
        if(!mImage) {
            mImage.reset(new Image);
            mImage->addListener(static_pointer_cast<EffectMask>(shared_from_this()));
        }
        mImage->load(Image::createSource(value));
    }
    mMatch = params["match"].value<uint8_t>(0, mMatch);
    mDump = params["dump"].value<uint8_t>(0, mDump);
    mDim = params["dim"].value<double>(0, mDim);
    return Effect::setParams(params);
}

DisplayList::SharedPtr EffectDiff::DefaultRenderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    if(Surface::SharedPtr image = mEffect->getImage()) {
        Surface::SharedPtr surface = dl->flatten();
        if(mEffect->shouldDump()) {
            char filename[2048];
            sprintf(filename, "/tmp/surface_diff_image");
            SurfaceDecoders::instance()->encode(image, filename);
        }
        if(mEffect->shouldDump()) {
            char filename[2048];
            sprintf(filename, "/tmp/surface_diff_orig");
            SurfaceDecoders::instance()->encode(surface, filename);
        }
        {
            int diffPixels = 0;
            const double dim = mEffect->getDim();
            const Color diffColor = mEffect->getColor();
            const int match = mEffect->getMatch();
            Surface::Data in_data(image, Surface::Data::ReadLock);
            Surface::Data out_data(surface, Surface::Data::ReadWriteLock);
            const int width = static_cast<int>(std::min(area.width, in_data.getWidth()));
            const int height = static_cast<int>(std::min(area.height, in_data.getHeight()));
            for(int out_x = (int)area.x, in_x = 0; out_x < width; ++out_x, ++in_x) {
                for(int out_y = (int)area.y, in_y = 0; out_y < height; ++out_y, ++in_y) {
                    const Color image_c = (in_x < in_data.getWidth() && in_y < in_data.getHeight()) ? in_data.getPixel(in_x, in_y) : Color(0, 0, 0, 0);
                    Color c = out_data.getPixel(out_x, out_y);
                    if(abs(image_c.r-c.r) > match || abs(image_c.g-c.g) > match || abs(image_c.b-c.b) > match || abs(image_c.a-c.a) > match) {
                        ++diffPixels;
                        c = diffColor;
                    } else {
                        c.r = std::max(0, int(c.r*(1-dim)));
                        c.g = std::max(0, int(c.g*(1-dim)));
                        c.b = std::max(0, int(c.b*(1-dim)));
                        c.a = std::max(0, int(c.a*(1-dim)));
                    }
                    out_data.setPixel(out_x, out_y, c);
                }
            }
            {
                Variant data;
                data["pixels"] = diffPixels;
                mEffect->onEffectChanged(data);
            }
        }
        if(mEffect->shouldDump()) {
            char filename[2048];
            sprintf(filename, "/tmp/surface_diff");
            SurfaceDecoders::instance()->encode(surface, filename);
        }
        return DisplayList::SharedPtr(new DisplayList(surface));
    }
    return dl;
}

Rect EffectDiff::DefaultRenderer::getRect(const Rect &rect) const
{
    if(mEffect->hasImage()) {
        const Size imageSize(mEffect->getImageSize());
        return Rect(rect.x, rect.y, range(Coordinate(0), rect.width, imageSize.width), range(Coordinate(0), rect.height, imageSize.height));
    }
    return rect;
}

unsigned int EffectDiff::purge()
{
    if(mImage)
        return mImage->purge();
    return 0;
}

void EffectDiff::onImageLoaded(const Image::SharedPtr &, bool)
{
    onEffectChanged();
}

std::string EffectDiff::getAnimatedPropertyName(int property) const
{
    if(property == Property_Red)
        return "r";
    else if(property == Property_Green)
        return "g";
    else if(property == Property_Blue)
        return "b";
    return Effect::getAnimatedPropertyName(property);
}

int EffectDiff::getAnimatedPropertyID(const std::string &property) const
{
    if(property == "r")
        return Property_Red;
    else if(property == "g")
        return Property_Green;
    else if(property == "b")
        return Property_Blue;
    return Effect::getAnimatedPropertyID(property);
}

float EffectDiff::getAnimatedProperty(int property) const
{
    if(property == Property_Red)
        return mColor.r;
    else if(property == Property_Green)
        return mColor.g;
    else if(property == Property_Blue)
        return mColor.b;
    return Effect::getAnimatedProperty(property);
}

bool EffectDiff::setAnimatedProperty(int property, float value)
{
    if(property == Property_Red) {
        if(mColor.r != value && !isnan(value)) {
            mColor.r = static_cast<int>(value);
            onEffectChanged();
        }
        return true;
    } else if(property == Property_Green) {
        if(mColor.g != value && !isnan(value)) {
            mColor.g = static_cast<int>(value);
            onEffectChanged();
        }
        return true;
    } else if(property == Property_Blue) {
        if(mColor.b != value && !isnan(value)) {
            mColor.b = static_cast<int>(value);
            onEffectChanged();
        }
        return true;
    }
    return Effect::setAnimatedProperty(property, value);
}



