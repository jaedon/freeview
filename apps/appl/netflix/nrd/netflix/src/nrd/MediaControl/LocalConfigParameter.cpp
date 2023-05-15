/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "LocalConfigParameter.h"

using namespace std;
using namespace netflix::mediacontrol;

LocalConfigParameter::LocalConfigParameter()
{
}

void
LocalConfigParameter::reloadSystemParameters()
{
}

string LocalConfigParameter::queryParameter(const string &key) const
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
void LocalConfigParameter::setParameter(const string &key, const string &value)
{
    mParams[key] = value;
}

//////////
//
// toString
//
//////////
string LocalConfigParameter::toString()
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


