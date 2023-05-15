/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#define MDX_UTIL_C

#include "MdxLog.h"
#include "MdxUtil.h"

#include <nrdbase/Thread.h>
#include <algorithm>
#include <functional>
#include <stdarg.h>
#include <stdio.h>
#include <expat.h>

using namespace std;
using namespace netflix;
using namespace netflix::mdx;

namespace netflix {
namespace mdx {

HttpHeadersToVariantMap::HttpHeadersToVariantMap()
{
}

HttpHeadersToVariantMap::~HttpHeadersToVariantMap()
{
}

std::vector<std::string> splitHeaders(const std::string &str)
{
    std::vector<std::string> tokens;

    // size_t lastPos = 0;
    size_t lastPos = str.find_first_not_of(" \t\r\n", 0);
    size_t nextPos = str.find("\r\n", lastPos);

    while (nextPos != string::npos) {
        tokens.push_back(str.substr(lastPos, nextPos - lastPos));

        // lastPos = nextPos + delim.length();
        lastPos = str.find_first_not_of(" \t\r\n", nextPos);
        nextPos = str.find("\r\n", lastPos);
    }

    return tokens;
}

std::string trimWhitespace(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos) {
        return "";
    } else if (end == std::string::npos) {
        return str.substr(start);
    } else {
        return str.substr(start, (end + 1) - start);
    }
}

Variant HttpHeadersToVariantMap::format(const std::string &headers)
{
    Variant variantMap;

    std::vector<std::string> vHeaders = splitHeaders(headers);

    for (std::vector<std::string>::iterator i = vHeaders.begin(); i != vHeaders.end(); ++i) {
        std::string &header = *i;

        size_t colonPos = header.find(':');
        if (colonPos != std::string::npos) {
            std::string name = header.substr(0, colonPos);
            name = trimWhitespace(name);

            std::string value = header.substr(colonPos + 1);
            value = trimWhitespace(value);

            variantMap[name] = value;
        }
    }

    return variantMap;
}

XmlToVariantMap::XmlToVariantMap() : mParser(XML_ParserCreate(0)), mAllocated(0)
{
}

XmlToVariantMap::~XmlToVariantMap()
{
}

Variant XmlToVariantMap::format(const std::string &xml)
{
    reset();

    if (!XML_Parse(mParser, xml.c_str(), xml.size(), 1)) {
        MDXERROR("XmlToVariantMap failed to parse xml: %s", xml.c_str());
        mVariantMap.clear();
    }

    return mVariantMap;
}

void XmlToVariantMap::reset()
{
    mAllocated = 0;
    mVariantMap.clear();
    mMapInProgress.clear();
    XML_ParserReset(mParser, 0);

    XML_SetUserData(mParser, this);
    XML_SetElementHandler(mParser, startElementHandler, endElementHandler);
    XML_SetCharacterDataHandler(mParser, charDataHandler);
}

void XmlToVariantMap::startElementHandler(void *userData, const char *name, const char **attributes)
{
    (void)attributes; // unused
    XmlToVariantMap *formatter = reinterpret_cast<XmlToVariantMap *>(userData);
    const std::string str(name);
    if (formatter->mAllocated + str.size() > MaxAllocated) {
        XML_StopParser(formatter->mParser, XML_FALSE);
        return;
    }
    formatter->mAllocated += str.size();

    Variant newVariantMapLevel;
    newVariantMapLevel["name"] = str;

    formatter->mMapInProgress.push_back(newVariantMapLevel);
}

void XmlToVariantMap::endElementHandler(void *userData, const char *name)
{
    (void)name; // unused
    XmlToVariantMap *formatter = reinterpret_cast<XmlToVariantMap *>(userData);

    if (!formatter->mMapInProgress.empty()) {
        Variant completedMapLevel = formatter->mMapInProgress.back();
        formatter->mMapInProgress.pop_back();

        if (!formatter->mMapInProgress.empty()) {
            // Still have work to do
            Variant *inProgressLevel = &(formatter->mMapInProgress.back());
            const std::string key = completedMapLevel["name"].string();
            completedMapLevel.remove("name");
            (*inProgressLevel)[key] = completedMapLevel;
        } else {
            // We're all done!
            std::string key = completedMapLevel["name"].string();
            completedMapLevel.remove("name");
            formatter->mVariantMap[key] = completedMapLevel;
        }
    }
}

void XmlToVariantMap::charDataHandler(void *userData, const char *value, int length)
{
    XmlToVariantMap *formatter = reinterpret_cast<XmlToVariantMap *>(userData);

    if (!formatter->mMapInProgress.empty()) {
        Variant *inProgressLevel = &(formatter->mMapInProgress.back());

        std::string trimValue = std::string(value, length);
        trimValue.erase(trimValue.begin(),
                        std::find_if(trimValue.begin(), trimValue.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        trimValue.erase(std::find_if(trimValue.rbegin(), trimValue.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
                        trimValue.end());

        if (trimValue.length() > 0) {
            if (formatter->mAllocated + trimValue.size() > MaxAllocated) {
                XML_StopParser(formatter->mParser, XML_FALSE);
                return;
            }
            formatter->mAllocated += trimValue.size();

            if (!inProgressLevel->contains("_value")) {
                (*inProgressLevel)["_value"] = trimValue;
            } else {
                (*inProgressLevel)["_value"] = (*inProgressLevel)["_value"].string() + trimValue;
            }
        }
    }
}
}
}
