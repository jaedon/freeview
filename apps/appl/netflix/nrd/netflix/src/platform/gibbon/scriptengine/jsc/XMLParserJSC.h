/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef XMLParserJSC_h
#define XMLParserJSC_h

#include "JSC.h"
#include <nrdbase/XMLParser.h>

namespace netflix {
namespace gibbon {
namespace JSC {
class XMLParserJSC : public XMLParser
{
public:
    XMLParserJSC(JSContextRef context);
    class JSCText : public Value
    {
    public:
        JSCText(JSContextRef ctx, const char *str, int length);

        JSContextRef context;
        ScopedString text;
    };

    class JSCObject : public Value
    {
    public:
        JSCObject(XMLParserJSC *p, JSContextRef ctx);
        virtual void set(const char *name, Mode mode, const shared_ptr<Value> &value);

        JSContextRef context;
        JSObjectRef object;
        XMLParserJSC *parser;
    };
    virtual shared_ptr<Value> createText(const char *string, int length);
    virtual shared_ptr<Value> createObject();
private:
    JSContextRef mContext;
    ScopedString mLength;
    friend class JSCObject;
};
}
}
}

#endif
