/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JSONParserJSC_h
#define JSONParserJSC_h

#include "JSC.h"
#include <nrdbase/JSONParser.h>

namespace netflix {
namespace gibbon {
namespace JSC {

class JSONParserJSC : public JSONParser
{
public:
    JSONParserJSC(JSContextRef context);

    struct Base
    {
        Base(JSValueRef val, JSContextRef ctx);
        virtual ~Base();

        JSValueRef value;
        bool unprotect;
        JSContextRef context;
    };

    class JSCValue : public Value, public Base
    {
    public:
        JSCValue(Type type, JSValueRef val, JSContextRef ctx);
    };
    class JSCString : public String, public Base
    {
    public:
        enum Mode {
            Mode_Key,
            Mode_Value
        };
        JSCString(Mode mode, JSContextRef ctx);
        virtual void addData(const char *str, int len);
        virtual void addRange(const char *str, int len);
        virtual void finalize(const char *str, int len);

        struct Piece {
            Piece(const char *s, int l, bool o);

            const char *str;
            char *owned;
            int len;
        };

        std::vector<Piece> pieces;
        unsigned length;
        ScopedString string;
        const Mode mode;
    };

    class JSCObject : public Object, public Base
    {
    public:
        JSCObject(JSObjectRef obj, JSContextRef ctx);
        virtual bool setValue(String *key, Value *value);
    };

    class JSCArray : public Array, public Base
    {
    public:
        JSCArray(JSContextRef ctx);
        virtual void append(Value *value);
        virtual void finalize();

        std::vector<JSValueRef> values;
    };

    virtual Object *createObject();
    virtual Array *createArray();
    virtual String *createStringKey();
    virtual String *createStringValue();
    virtual Value *create(double val);
    virtual Value *create(bool val);
    virtual Value *create(llong val);
    virtual Value *createNull();

    JSValueRef parse(const char *data, int size); // overridden
private:
    JSContextRef mContext;
};
}
}
}

#endif
