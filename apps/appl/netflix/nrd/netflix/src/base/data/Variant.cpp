/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Variant.h"

#include <nrdbase/Variant.h>

#include <string.h>

#include <nrdbase/JSONFormatter.h>
#include <nrdbase/XMLParser.h>
#include <nrdbase/Log.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/JSONParser.h>

using namespace netflix;

const std::map<std::string, Variant> Variant::sNullStringMap;
const std::vector<Variant> Variant::sNullArray;

Variant Variant::fromQueryString(const std::string &queryString, bool decode)
{
    const int length = queryString.length();
    if (!length)
        return Variant();

    // Decoded query string contents don't overlap and are
    // always smaller than their encoded counterparts, so
    // we can utilize a single split-up buffer for all
    // keys and values.

    Variant variantMap;
    const char *front = queryString.c_str();
    while (front) {
        std::string key, val;

        // Tokenize by ampersand, and then by equals sign
        char *ampersandLoc = const_cast<char*>(strchr(front, '&'));
        char *equalsLoc = const_cast<char*>(strchr(front, '='));
        if (equalsLoc && (!ampersandLoc || equalsLoc < ampersandLoc))
            key = std::string(front, equalsLoc-front);
        else if (ampersandLoc)
            key = std::string(front, ampersandLoc-front);
        else
            key = std::string(front);
        front = ampersandLoc ? ampersandLoc+1 : 0;

        if (equalsLoc) {
            if(!ampersandLoc)
                val = std::string(equalsLoc+1);
            else if(equalsLoc < ampersandLoc)
                val = std::string(equalsLoc+1, ampersandLoc-equalsLoc-1);
        }
        if(decode) {
            key = UrlEncoder::decode(key);
            val = UrlEncoder::decode(val);
        }
        // Don't use Variant::fromString here as it will misconvert "01234" to integer
        // 1234. Leave the variant as a string variant and have the invoke() methods
        // convert the values as they know the final type
        variantMap[key] = Variant(val);
    }
    return variantMap;
}

std::string Variant::toJSON(bool pretty) const
{
    uint32_t flags = JSONFormatter::None;
    if (pretty)
        flags |= JSONFormatter::Pretty;
    JSONFormatter formatter(flags);
    return formatter.format(*this);
}

class XMLParserVariant : public XMLParser
{
public:
    class VariantText : public Value
    {
    public:
        VariantText(const char *str, int length)
            : Value(Text), text(str, length)
        {}
        virtual void set(const char *, Mode, const shared_ptr<Value> &)
        {
            assert(0 && "Impossible impossibility");
        }
        std::string text;
    };
    class VariantObject : public Value
    {
    public:
        VariantObject()
            : Value(Object)
        {}

        virtual void set(const char *name, Mode mode, const shared_ptr<Value> &value)
        {
            Variant &cur = object[name];
            switch (mode) {
            case Replace:
                if (value->type == Text) {
                    cur = reinterpret_cast<VariantText*>(value.get())->text;
                } else {
                    cur = reinterpret_cast<VariantObject*>(value.get())->object;
                }
                break;
            case Append: {
                assert(value->type == Text);
                const std::string &text = reinterpret_cast<VariantText*>(value.get())->text;
                if (cur.isNull()) {
                    cur = text;
                } else {
                    assert(cur.isString());
                    cur = cur.string() + text;
                }
                break; }
            case Array:
                if (value->type == Text) {
                    cur.push_back(reinterpret_cast<VariantText*>(value.get())->text);
                } else {
                    cur.push_back(reinterpret_cast<VariantObject*>(value.get())->object);
                }
                break;
            }
        }
        Variant object;
    };
    virtual shared_ptr<Value> createText(const char *string, int length)
    {
        return shared_ptr<Value>(new VariantText(string, length));
    }
    virtual shared_ptr<Value> createObject()
    {
        return shared_ptr<Value>(new VariantObject);
    }
};

Variant Variant::fromXML(const char *data, int length, bool *ok, int *used)
{
    if (used)
        *used = 0;
    XMLParserVariant parser;
    Variant ret;
    shared_ptr<XMLParser::Value> val = parser.parse(data, length);
    if (val) {
        if (ok)
            *ok = true;
        assert(val->type == XMLParser::Value::Object);
        ret = reinterpret_cast<XMLParserVariant::VariantObject*>(val.get())->object;
        if (used)
            *used = parser.current() - data;
    } else if (ok) {
        *ok = false;
    }
    return ret;
}

class JSONParserVariant : public JSONParser
{
public:
    class VariantValue : public Value
    {
    public:
        VariantValue(Type type) : Value(type) {}
        Variant value;
    };
    class VariantString : public String
    {
    public:
        virtual void addData(const char *str, int len) { string.append(str, len); }
        virtual void addRange(const char *str, int len) { string.append(str, len); }
        virtual void finalize(const char *str, int len) { string.append(str, len); }

        std::string string;
    };

    class VariantObject : public Object
    {
    public:
        VariantObject() : object(Variant::Type_StringMap) {}
        virtual bool setValue(String *key, Value *value)
        {
            const std::string &str = static_cast<VariantString*>(key)->string;
            if (object.find(str) == object.stringMapEnd()) {
                object[str] = JSONParserVariant::value(value);
                return true;
            }
            return false;
        }

        Variant object;
    };

    class VariantArray : public Array
    {
    public:
        VariantArray() : array(Variant::Type_Array) {}
        virtual void append(Value *value)
        {
            array.push_back(JSONParserVariant::value(value));
        }
        Variant array;
    };

    virtual Object *createObject() { return new VariantObject; }
    virtual Array *createArray() { return new VariantArray; }
    virtual String *createStringKey() { return new VariantString; }
    virtual String *createStringValue() { return new VariantString; }
    virtual Value *create(double val) { VariantValue *v = new VariantValue(Value::Type_Double); v->value = val; return v; }
    virtual Value *create(bool val) { VariantValue *v = new VariantValue(Value::Type_Boolean); v->value = val; return v; }
    virtual Value *create(llong val) { VariantValue *v = new VariantValue(Value::Type_Integer); v->value = val; return v; }
    virtual Value *createNull() { return new VariantValue(Value::Type_Null); }

    static Variant value(const Value *value)
    {
        switch (value->type) {
        case Value::Type_Object:
            return static_cast<const VariantObject*>(value)->object;
        case Value::Type_Array:
            return static_cast<const VariantArray*>(value)->array;
        case Value::Type_Null:
        case Value::Type_Double:
        case Value::Type_Integer:
        case Value::Type_Boolean:
            return static_cast<const VariantValue*>(value)->value;
        case Value::Type_String:
            return static_cast<const VariantString*>(value)->string;
        }
        return Variant();
    }
};

Variant Variant::fromJSON(const char *data, int length, bool *ok, int *used)
{
    JSONParserVariant parser;
    const JSONParser::Value *value = parser.parse(data, length);
    if (!value) {
        if (used)
            *used = 0;
        if (ok)
            *ok = false;
        return Variant();
    }
    if (used)
        *used = parser.current() - data;
    if (ok)
        *ok = true;
    const Variant ret = JSONParserVariant::value(value);
    delete value;
    return ret;
}

