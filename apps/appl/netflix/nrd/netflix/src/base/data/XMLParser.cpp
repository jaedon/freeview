/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "XMLParser.h"
#include <algorithm>
#include <assert.h>
#include <string.h>

using namespace netflix;

#define ATTR_NAME "$attributes"
#define CHILDREN_NAME "$children"
#define NAME_NAME "$name"
#define TEXT_NAME "$text"

XMLParser::XMLParser()
    : mSaxParser(0), mCurrent(0)
{
}

shared_ptr<XMLParser::Value> XMLParser::parse(const char *data, int length)
{
    netflix::SaxParser expat('\1');
    expat.setUserData(this);
    expat.setElementHandler(startElementHandler, endElementHandler);
    expat.setCharDataHandler(charDataHandler);
    assert(mError.empty());
    mSaxParser = &expat;
    const bool ok = expat.parse(data, length);
    mSaxParser = 0;
    mCurrent = data + expat.curByteOffset();
    if (!ok) {
        mError = expat.errorString();
        return shared_ptr<Value>();
    } else {
        mError.clear();
    }
    assert(mStack.size() == 1);
    shared_ptr<Value> ret = mStack[0].first;
    mStack.clear();
    return ret;
}

static const char *fixNamespace(const char *n)
{
    const char *ns = strchr(n, '\1');
    return ns ? ns + 1 : n;
}

void XMLParser::startElementHandler(const char *name, const char **attributes)
{
    shared_ptr<Value> val = createObject();
    assert(val);
    {
        const char *n = fixNamespace(name);
        val->set(NAME_NAME, Value::Replace, createText(n, strlen(n)));
    }

    uint32_t numAttributes = mSaxParser->getAttributeCount();
    bool preserveSpace = mStack.empty() ? false : mStack.back().second;

    if (numAttributes) {
        shared_ptr<Value> attributesValue = createObject();
        assert(attributesValue);

        for (uint32_t i = 0; i < numAttributes; i += 2) {
            const char *attribute = fixNamespace(attributes[i]);
            attributesValue->set(attribute, Value::Replace, createText(attributes[i + 1], strlen(attributes[i + 1])));
            if (!strcmp(attribute, "space")) {
                if (!strcmp(attributes[i + 1], "default")) {
                    preserveSpace = false;
                } else if (!strcmp(attributes[i + 1], "preserve")) {
                    preserveSpace = true;
                }
            }
        }

        val->set(ATTR_NAME, Value::Replace, attributesValue);
    }

    // create our own node data
    mStack.push_back(std::make_pair(val, preserveSpace));
}

void XMLParser::charDataHandler(const char *value, int length)
{
    assert(!mStack.empty());
    if (!mStack.empty()) {
        if (!mStack.back().second) {
            while (length && isspace(static_cast<unsigned char>(*value))) {
                --length;
                ++value;
            }

            const char *tail = value + length - 1;
            while (tail > value && isspace(static_cast<unsigned char>(*tail))) {
                --tail;
                --length;
            }
        } else {
            int i;
            for (i=0; i<length; ++i) {
                if (!isspace(static_cast<unsigned char>(value[i])))
                    break;
            }
            if (i == length)
                return;
        }
        if (length) {
            shared_ptr<Value> cur = mStack.back().first;
            shared_ptr<Value> text = createText(value, length);
            cur->set(TEXT_NAME, Value::Append, text);
            cur->set(CHILDREN_NAME, Value::Array, text);
        }
    }
}

void XMLParser::endElementHandler(const char */*name*/)
{
    assert(!mStack.empty());
    if (mStack.size() > 1) {
        shared_ptr<Value> node = mStack.back().first;
        mStack.pop_back();
        mStack.back().first->set(CHILDREN_NAME, Value::Array, node);
    }
}
