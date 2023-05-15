/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JSONParser_h
#define JSONParser_h

#include <nrdbase/StdInt.h>

namespace netflix {

class JSONParser
{
public:
    JSONParser() : mData(0), mEnd(0), mCurrent(0), mError(false) {}
    virtual ~JSONParser() {}

    class Value
    {
    public:
        enum Type {
            Type_String,
            Type_Object,
            Type_Array,
            Type_Null,
            Type_Boolean,
            Type_Integer,
            Type_Double
        };
        Value(Type t) : type(t) {}

        const Type type;
        virtual ~Value() {}
    };
    class String : public Value
    {
    public:
        String() : Value(Type_String) {}
        virtual ~String() {}
        virtual void addData(const char *str, int len) = 0;
        virtual void addRange(const char *str, int len) = 0;
        virtual void finalize(const char *str = "", int len = 0) = 0;
        void addChar(char ch) { addData(&ch, 1); }
        void addChar(unsigned char ch) { addChar(static_cast<char>(ch)); }
    };

    class Object : public Value
    {
    public:
        Object() : Value(Type_Object) {}
        virtual ~Object() {}
        virtual bool setValue(String *string, Value *value) = 0;
    };

    class Array : public Value
    {
    public:
        Array() : Value(Type_Array) {}
        virtual ~Array() {}
        virtual void append(Value *value) = 0;
        virtual void finalize() {}
    };


    Value *parse(const char *data, int size);
    const char *current() const { return mCurrent; }
    bool atEnd() const { return (mCurrent == mEnd); }
    bool hasError() const { return mError; }

protected:
    virtual Object *createObject() = 0;
    virtual Array *createArray() = 0;
    virtual String *createStringKey() = 0;
    virtual String *createStringValue() = 0;
    virtual Value *create(double val) = 0;
    virtual Value *create(bool val) = 0;
    virtual Value *create(llong val) = 0;
    virtual Value *createNull() = 0;
private:
    static void putUtf(unsigned int utf, String *string);
    static void putEscapedUtf(unsigned int utf, String *string);

    Value *parseValue();
    bool parseString(String *string);
    bool parseObject(Object *object);
    bool parseArray(Array *array);
    bool eatWhiteSpace();

    const char *mData;
    const char *mEnd;
    const char *mCurrent;
    bool mError;
};

}

#endif
