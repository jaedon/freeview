/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __TEXTBRIDGE_H__
#define __TEXTBRIDGE_H__

#include <Text.h>
#include <Widget.h>
#include <GibbonBaseBridge.h>

#include <nrd/NfObject.h>
#include <nrd/Request.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class TextBridge : public GibbonBaseBridge
{
public:
    TextBridge(const Text::SharedPtr &text);
    ~TextBridge();
    void init();

    class TextListener : public Text::Listener
    {
    public:
        TextListener(shared_ptr<TextBridge> bridge) : mBridge(bridge) {}
        virtual ~TextListener() {}
        virtual void onPropertiesChanged(unsigned int property)
        {
            if(shared_ptr<TextBridge> locked = mBridge.lock())
                locked->handlePropertiesChanged(property);
        }
    protected:
        weak_ptr<TextBridge> mBridge;
    };

    void handlePropertiesChanged(unsigned int properties);

    inline Text::SharedPtr getText() const { return mText; }

    virtual ClassType classType() const { return TextType; }
    virtual uint32_t classID() const {
        if(Widget::SharedPtr w = mText->getWidget())
            return w->getID();
        return 0;
    }

    virtual unsigned requestFlags(const Request &request) const
    {
        return GibbonBaseBridge::requestFlags(request) | Request::Queued;
    }
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);
protected:
    virtual void syncPendingProperties(unsigned int properties);

    bool mSendRenderProperties;
    bool mSendTextLoaded;
    Text::SharedPtr mText;
};

}} // namespace netflix::gibbon

#endif /* __TEXTBRIDGE_H__ */
