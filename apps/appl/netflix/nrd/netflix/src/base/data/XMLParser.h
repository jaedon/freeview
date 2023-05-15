/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <nrdbase/StdInt.h>
#include <nrdbase/SaxParser.h>
#include <vector>
#include <nrdbase/tr1.h>

namespace netflix {

class XMLParser
{
public:
    XMLParser();
    virtual ~XMLParser() {}
    class Value : public enable_shared_from_this<Value>
    {
    public:
        enum Type {
            Text,
            Object
        };
        Value(Type t)
            : type(t)
        {}
        virtual ~Value()
        {}

        enum Mode {
            Replace,
            Append,
            Array
        };

        virtual void set(const char */* name */, Mode/* mode */, const shared_ptr<Value> &/* value */) {}

        const Type type;
    };

    shared_ptr<Value> parse(const char *data, int length);
    std::string error() const { return mError; }
    const char *current() { return mCurrent; }
protected:
    virtual shared_ptr<Value> createText(const char *string, int length) = 0;
    virtual shared_ptr<Value> createObject() = 0;
private:
    void startElementHandler(const char *name, const char **attributes);
    void charDataHandler(const char *s, int len);
    void endElementHandler(const char *name);

    static void startElementHandler(void *userData, const char *name, const char **attributes)
    {
        reinterpret_cast<XMLParser*>(userData)->startElementHandler(name, attributes);
    }
    static void charDataHandler(void *userData, const char *s, int len)
    {
        reinterpret_cast<XMLParser*>(userData)->charDataHandler(s, len);
    }
    static void endElementHandler(void *userData, const char *name)
    {
        reinterpret_cast<XMLParser*>(userData)->endElementHandler(name);
    }

private:
    std::vector<std::pair<shared_ptr<Value>, bool> > mStack;
    std::string mError;
    netflix::SaxParser *mSaxParser;
    const char *mCurrent;
};

}

#endif // XMLPARSER_H
