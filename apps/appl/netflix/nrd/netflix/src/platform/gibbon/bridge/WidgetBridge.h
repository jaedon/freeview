/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WIDGETBRIDGE_H__
#define __WIDGETBRIDGE_H__

#include <Widget.h>
#include <Animation.h>
#include <TextBridge.h>
#include <ImageBridge.h>
#include <EffectBridge.h>
#include <GibbonBaseBridge.h>

#include <nrdbase/tr1.h>

#include <nrdbase/Variant.h>
#include <nrd/Request.h>
#include <nrd/NfObject.h>

namespace netflix {
namespace gibbon {

inline static Variant convertColor(const Color &color)
{
    Variant value;
    value["r"] = color.r;
    value["g"] = color.g;
    value["b"] = color.b;
    value["a"] = color.a;
    return value;
}

inline static Color convertColor(const Variant &value)
{
    Color color;
    if(value.isNumber()) {
        const int rgba = value.value<int>();
        color.r = (rgba >> 16) & 0xFF;
        color.g = (rgba >>  8) & 0xFF;
        color.b = (rgba >>  0) & 0xFF;
        color.a = (rgba >> 24) & 0xFF;
    } else if(value.isStringMap() || value.isNull()) {
        color.r = value["r"].value<int>(0, 0);
        color.g = value["g"].value<int>(0, 0);
        color.b = value["b"].value<int>(0, 0);
        color.a = value["a"].value<int>(0, 255);
    } else {
        color = Color(value.value<std::string>().c_str());
    }
    return color;
}

inline static unsigned int convertAlignment(const char *string)
{
    assert(!strchr(string, ' '));
    if(!strcmp(string, "right"))
        return Style::ALIGN_RIGHT;
    else if(!strcmp(string, "left"))
        return Style::ALIGN_LEFT;
    else if(!strcmp(string, "top"))
        return Style::ALIGN_TOP;
    else if(!strcmp(string, "bottom"))
        return Style::ALIGN_BOTTOM;
    else if(!strcmp(string, "center-horizontal"))
        return Style::ALIGN_CENTER_HORIZONTAL;
    else if(!strcmp(string, "center-vertical"))
        return Style::ALIGN_CENTER_VERTICAL;
    else if(!strcmp(string, "center") || !strcmp(string, "center-both"))
        return Style::ALIGN_CENTER_HORIZONTAL|Style::ALIGN_CENTER_VERTICAL;
    return Style::ALIGN_LEFT;
}

inline static unsigned int convertAlignment(const Variant &value)
{
    unsigned long align = 0;
    switch (value.type()) {
    case Variant::Type_Integer:
    case Variant::Type_Double:
        align = value.value<int>();
        break;
    case Variant::Type_Array: {
        const int size = value.size();
        for(int i=0; i<size; ++i)
            align |= convertAlignment(value[i]);
        break; }
    case Variant::Type_String: {
        std::string string = value.value<std::string>();
        if(string.find(' ') == std::string::npos) {
            align = convertAlignment(string.c_str());
        } else {
            char *saveptr;
            for(char *str=strtok_r(&string[0], " ", &saveptr); str; str = strtok_r(0, " ", &saveptr))
                align |= convertAlignment(str);
        }
        break; }
    default:
        break;
    }
    return align;
}

inline static Variant convertAlignment(unsigned long align)
{
    Variant value;
    if(align & Style::ALIGN_RIGHT)
        value.push_back("right");
    if(align & Style::ALIGN_LEFT)
        value.push_back("left");
    if(align & Style::ALIGN_TOP)
        value.push_back("top");
    if(align & Style::ALIGN_BOTTOM)
        value.push_back("bottom");
    if(align & Style::ALIGN_CENTER_HORIZONTAL)
        value.push_back("center-horizontal");
    if(align & Style::ALIGN_CENTER_VERTICAL)
        value.push_back("center-vertical");
    return value;
}

class WidgetBridge : public GibbonBaseBridge
{
public:
    WidgetBridge(const std::string &name, Widget::SharedPtr widget);
    ~WidgetBridge();
    void init();

    class WidgetListener : public Widget::Listener
    {
    public:
        WidgetListener(shared_ptr<WidgetBridge> bridge) : mBridge(bridge) {}
        virtual ~WidgetListener() {}
        virtual void onAnimationFinished(Widget::Property property, const AnimationFinish &finish)
        {
            if(shared_ptr<WidgetBridge> locked = mBridge.lock())
                locked->handleAnimationFinished(property, finish);
        }
        virtual void onRenderPropertiesChanged(unsigned int property)
        {
            if(shared_ptr<WidgetBridge> locked = mBridge.lock())
                locked->handleRenderPropertiesChanged(property);
        }
    protected:
        weak_ptr<WidgetBridge> mBridge;
    };
    void setAnimated(bool a) { mAnimate = a; }
    void handleRenderPropertiesChanged(unsigned int properties);
    void handleAnimationFinished(Widget::Property property, const AnimationFinish &finish);
    void destroy();

    enum {
        Dump_Recursive    = 0x01,
        Dump_NoProperties = 0x02,
        Dump_All          = 0x04
    };
    Variant dump(uint8_t flags) const;

    shared_ptr<EffectBridge> getEffect(int id);
    shared_ptr<ImageBridge> getImage(int id);
    inline shared_ptr<TextBridge> getText() const { return mTextBridge; }
    inline Widget::SharedPtr getWidget() const { return mWidget; }

    inline int getID() const { return getWidget()->getID(); }

    virtual std::string describe() const { return mWidget->describe(); }
    virtual unsigned requestFlags(const Request &request) const
    {
        return GibbonBaseBridge::requestFlags(request) | Request::Queued;
    }
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual void setProperties(int index, const Variant &value);

    virtual Variant invoke(int index, const Variant &args);

    shared_ptr<ImageBridge> createImage(int id, Image::Type type);
    inline shared_ptr<ImageBridge> createImage(int id, bool background) {
        return createImage(id, background ? Image::Type_Background : Image::Type_Foreground);
    }
    void removeImage(const shared_ptr<ImageBridge> &image);

    virtual ClassType classType() const { return WidgetType; }
    virtual uint32_t classID() const { return getID(); }

    shared_ptr<EffectBridge> createEffect(int id, Effect::Type type);
    shared_ptr<EffectBridge> createEffect(int id, const Variant &type);
    void removeEffect(const shared_ptr<EffectBridge> &effect);

    shared_ptr<TextBridge> createText();

protected:
    bool mAnimate;
    Widget::SharedPtr mWidget;
    bool mSendRenderProperties;
    bool mSendAnimationFinished;
    shared_ptr<NfObject> mEffectsBridge;
    shared_ptr<NfObject> mImagesBridge;
    shared_ptr<TextBridge> mTextBridge;

    void destroy_internal();
    virtual void syncPendingProperties(unsigned int properties);
    void setAnimatedProperty(Widget::Property property, const Variant &value);
    void setAnimatedProperty(Widget::Property property, float value, float startValue, const Animation::Info *animate, bool append=false);
};

}} // namespace netflix::gibbon

#endif /* __WIDGETBRIDGE_H__ */
