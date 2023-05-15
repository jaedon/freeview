/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef CONFIGDATA_H_
#define CONFIGDATA_H_

#include <nrdbase/StdInt.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <stack>
#include <vector>

#include <nrdbase/Variant.h>
#include <nrdbase/DataBuffer.h>

namespace netflix {

class ConfigData
{
public:
    ConfigData();
    virtual ~ConfigData();
    bool init(const std::string &file);
    bool init(const DataBuffer &data, const std::string &sourceDescription = "");

    const Variant &values() const { return mValues; }
    std::string operator[](const std::string &key) const { return mValues.mapValue<std::string>(key); }
    bool hasValue(const std::string &key) const { return mValues.contains(key); }
    std::string value(const std::string &key) const { return mValues.mapValue<std::string>(key); }
private:
    static void startElementHandler(void *userData, const char *name, const char **attributes);
    static void endElementHandler(void *userData, const char *name);
    static void charDataHandler(void *user, const char *s, int len);

    Variant mValues;
};

}

#endif // CONFIGDATA_H_
