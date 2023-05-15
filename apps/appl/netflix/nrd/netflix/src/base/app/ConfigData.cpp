/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <nrdbase/ConfigData.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Log.h>
#include <nrdbase/SaxParser.h>

#include <iostream>
#include <fstream>
#include <assert.h>
#include <string.h>

using namespace netflix;

ConfigData::ConfigData()
{
}

ConfigData::~ConfigData()
{
}

bool ConfigData::init(const std::string &filename)
{
    const DataBuffer data = Configuration::resourceContent(filename);
    return init(data, filename);
}

bool ConfigData::init(const DataBuffer &data, const std::string &sourceDescription)
{
    if (data.isEmpty())
        return false;
    SaxParser expat;
    std::stack<Variant*> stack;
    stack.push(&mValues);

    expat.setUserData(&stack);
    expat.setCharDataHandler(charDataHandler);
    expat.setElementHandler(startElementHandler, endElementHandler);

    bool result = expat.parse(data.c_str(), data.size());
    if (!result) {
        Log::error(TRACE_CONFIGDATA, "Error (%s) parsing %s", expat.errorString().c_str(),
                   sourceDescription.empty() ? "*data*" : sourceDescription.c_str());
    }
    return result;
}

void ConfigData::startElementHandler(void *userData, const char *name,
                                     const char **/*attributes*/)
{
    if (!strcmp(name, "config_root") || !strcmp(name, "config_data"))
        return;
    std::stack<Variant*> &stack = *reinterpret_cast<std::stack<Variant*> *>(userData);
    switch (stack.top()->type()) {
    case Variant::Type_StringMap:
    case Variant::Type_Null: {
        Variant &v = (*stack.top())[name];
        if (!v.isStringMap())
            v.clear();
        stack.push(&v);
        break; }
    default:
        break;
    }
}

void ConfigData::endElementHandler(void *userData, const char *name)
{
    if (!strcmp(name, "config_root") || !strcmp(name, "config_data"))
        return;
    std::stack<Variant*> &stack = *reinterpret_cast<std::stack<Variant*> *>(userData);
    if (!strcmp(name, "include")) {
        ConfigData cfg;
        std::string includeFile = stack.top()->value<std::string>();
        stack.pop();
        if (includeFile[0] == '$') {
            size_t slash = includeFile.find('/');
            std::string env = includeFile.substr(1, slash - 1);
            char *val = getenv(env.c_str());
            if (val)
                includeFile.replace(0, slash, val);
        }
        if (cfg.init(includeFile)) {
            const Variant &included = cfg.mValues;
            if (!included.isNull())
                stack.top()->merge(included);
        }
        stack.top()->remove("include");
    } else {
        stack.pop();
    }
}

void ConfigData::charDataHandler(void *userData, const char *s, int len)
{
    std::stack<Variant*> &stack = *reinterpret_cast<std::stack<Variant*> *>(userData);
    bool isValid = false;
    for (int i=0; i<len; ++i) {
        if (!std::isspace((unsigned char)s[i])) {
            isValid = true;
            break;
        }
    }

    if (isValid) {
        const std::string value(s, len);
        Variant &top = *stack.top();
        if (top.isString()) {
            top = top.string() + value;
        } else {
            top = value;
        }
    }
}
