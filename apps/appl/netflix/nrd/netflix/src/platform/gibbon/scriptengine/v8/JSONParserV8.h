/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JSONParserJSC_h
#define JSONParserJSC_h

#include <nrdbase/JSONParser.h>
#include <v8.h>
#include <vector>
#include <string.h>
#include <stdlib.h>

namespace netflix {
namespace gibbon {

class JSONParserV8 : public JSONParser
{
public:
    JSONParserV8() { }

    class V8Value : public Value
    {
    public:
        V8Value(Type type, const v8::Handle<v8::Value>& val)
            : Value(type), value(val)
        {
        }

        v8::Handle<v8::Value> value;
    };

    class V8String : public String
    {
    public:
        V8String() : length(0) { }
        virtual void addData(const char *str, int len)
        {
            pieces.push_back(Piece(str, len, true));
            length += len;
        }
        virtual void addRange(const char *str, int len)
        {
            pieces.push_back(Piece(str, len, false));
            length += len;
        }
        virtual void finalize(const char *str, int len)
        {
            length += len;
            if (pieces.empty()) {
                string = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                                 str, v8::String::kNormalString,
                                                 len);
            } else {
                char staticBuf[1024];
                char *buf;
                if (length >= sizeof(buf)) {
                    buf = new char[length + 1];
                } else {
                    buf = staticBuf;
                }

                int pos = 0;

                for (std::vector<Piece>::const_iterator it = pieces.begin(); it != pieces.end(); ++it) {
                    if (it->owned) {
                        memcpy(buf + pos, it->owned, it->len);
                        free(it->owned);
                    } else {
                        memcpy(buf + pos, it->str, it->len);
                    }
                    pos += it->len;
                }
                if (len)
                    memcpy(buf + pos, str, len);
                buf[length] = '\0';
                string = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                                 buf, v8::String::kNormalString,
                                                 length);
                if (buf != staticBuf)
                    delete[] buf;
            }
        }

        struct Piece {
            Piece(const char *s, int l, bool o);

            const char *str;
            char *owned;
            int len;
        };

        std::vector<Piece> pieces;
        unsigned length;
        v8::Handle<v8::String> string;
    };

    class V8Object : public Object
    {
    public:
        V8Object()
            : object(v8::Object::New(v8::Isolate::GetCurrent()))
        {
        }
        virtual bool setValue(String *key, Value *value);

        v8::Handle<v8::Object> object;
    };

    class V8Array : public Array
    {
    public:
        V8Array() { }
        virtual void append(Value *value);
        virtual void finalize();

        std::vector<v8::Handle<v8::Value> > values;
        v8::Handle<v8::Array> array;
    };

    static inline v8::Handle<v8::Value> cast(const JSONParser::Value* value)
    {
        if (value) {
            // we can't cast directly to a Base * because of multiple inheritance
            switch (value->type) {
            case JSONParser::Value::Type_Object:
                return static_cast<const JSONParserV8::V8Object*>(value)->object;
            case JSONParser::Value::Type_Array:
                return static_cast<const JSONParserV8::V8Array*>(value)->array;
            case JSONParser::Value::Type_Null:
            case JSONParser::Value::Type_Integer:
            case JSONParser::Value::Type_Double:
            case JSONParser::Value::Type_Boolean:
                return static_cast<const JSONParserV8::V8Value*>(value)->value;
            case JSONParser::Value::Type_String:
                return static_cast<const JSONParserV8::V8String*>(value)->string;
            }
        }
        return v8::Handle<v8::Value>();
    }

    virtual Object *createObject();
    virtual Array *createArray();
    virtual String *createStringKey();
    virtual String *createStringValue();
    virtual Value *create(double val);
    virtual Value *create(bool val);
    virtual Value *create(llong val);
    virtual Value *createNull();

    v8::Handle<v8::Value> parse(const char *data, int size) // overridden
    {
        const Value* val = JSONParser::parse(data, size);
        const v8::Handle<v8::Value> ret = cast(val);
        delete val;
        return ret;
    }
};

inline JSONParserV8::V8String::Piece::Piece(const char *s, int l, bool o)
    : str(o ? 0 : s), owned(o ? strndup(s, l) : 0), len(l)
{
}

inline bool JSONParserV8::V8Object::setValue(String *key, Value *value)
{
    const v8::Handle<v8::String>& str = static_cast<V8String*>(key)->string;
    const v8::Handle<v8::Value>& val = JSONParserV8::cast(value);
    object->Set(str, val);
    return true;
}

inline void JSONParserV8::V8Array::append(Value *value)
{
    values.push_back(JSONParserV8::cast(value));
}

inline void JSONParserV8::V8Array::finalize()
{
    const size_t sz = values.size();
    array = v8::Array::New(v8::Isolate::GetCurrent(), sz);
    for (size_t idx = 0; idx < sz; ++idx) {
        array->Set(idx, values.at(idx));
    }
}

}}

#endif
