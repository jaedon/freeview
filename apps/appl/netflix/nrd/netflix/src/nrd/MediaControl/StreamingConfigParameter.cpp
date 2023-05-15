/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StreamingConfigParameter.h"
#include "NrdApplication.h"

#include <nrdbase/Log.h>
#include <nrdbase/Variant.h>

using namespace std;
using namespace netflix::mediacontrol;

//////////
//
// StreamingConfigParameter
//
//////////
StreamingConfigParameter::StreamingConfigParameter()
{
    reloadSystemParameters();
}

void StreamingConfigParameter::reloadSystemParameters()
{
    // use the system config parameters as a starting point
    std::map<std::string, std::string> const& sysparams =
        nrdApp()->serviceParameters("streaming");

    for ( map<string, string>::const_iterator iter = sysparams.begin();
            iter != sysparams.end();
            ++iter )
    {
        string key = iter->first;
        mParams[key] = iter->second;
    }

    NTRACE(TRACE_MEDIACONTROL, "StreamingConfigParameter after reloading: %s",
                toString().c_str());
}

//////////
//
// queryParameter
//
//////////
string StreamingConfigParameter::queryParameter(const string &key) const
{
    map<string, string>::const_iterator it = mParams.find(key);
    if (it == mParams.end())
    {
        // not found
        return "";
    }

    return (*it).second;
}

//////////
//
// setParameter
//
//////////
void StreamingConfigParameter::setParameter(const string &key, const string &value)
{
    mParams[key] = value;
}

//////////
//
// toString
//
//////////
string StreamingConfigParameter::toString()
{
    ostringstream strm;
    strm.imbue(std::locale::classic());

    map<string, string>::const_iterator it;
    for (it = mParams.begin(); it != mParams.end(); ++it)
    {
        strm << (*it).first << ":" << (*it).second << ";";
    }

    return strm.str();
}

map<uint32_t, uint32_t>
StreamingConfigParameter::loadConfigMap(string const& paraKey, std::string const& key1, std::string const& key2, uint32_t minKeyVal)
{
    Variant variant = queryParameter(paraKey);
    map<uint32_t, uint32_t> configMap;

    if ( !variant.isNull() )
    {
        if (variant.isString())
        {
            variant = Variant::fromJSON(variant.string());
        }

        uint32_t value1, value2;
        bool haveValue1, haveValue2;
        for (Variant::ArrayIterator iter = variant.arrayBegin();
             iter != variant.arrayEnd();
             ++iter)
        {
            value1 = 0;
            value2 = 0;
            haveValue1 = false;
            haveValue2 = false;
            if ( (*iter).isStringMap() )
            {
                for (Variant::StringMapIterator mapIter = (*iter).stringMapBegin();
                     mapIter != (*iter).stringMapEnd();
                     ++mapIter)
                {
                    string key = mapIter->first;
                    if ( key == key1 )
                    {
                        value1 = mapIter->second.value<uint32_t>();
                        haveValue1 = true;
                    }
                    else if ( key == key2 )
                    {
                        value2 = mapIter->second.value<uint32_t>();
                        haveValue2 = true;
                    }
                }

            }
            if ( haveValue1 && haveValue2 && (value2 >= minKeyVal) )
            {
                configMap[value1] = value2;
            }

        }

    }

    return configMap;

}
