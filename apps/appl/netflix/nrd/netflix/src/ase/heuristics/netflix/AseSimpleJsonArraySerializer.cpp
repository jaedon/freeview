/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AseSimpleJsonArraySerializer.h"

#include <string>
#include <vector>
#include <sstream>
#include <locale>
#include <nrdbase/StdInt.h>

using namespace netflix::ase;
using namespace std;

string AseSimpleJsonArraySerializer::toJsonString(const vector< vector<SimpleObj> >& dataArray)
{
    // check empty vector
    if ( dataArray.empty() ) {
        return "";
    }

    ostringstream jsonFormatMessage;
    jsonFormatMessage.imbue(locale::classic());
    jsonFormatMessage << "[";

    for ( uint32_t i = 0; i < dataArray.size(); ++i )
    {
        const vector<SimpleObj>& objArray = dataArray[i];
        jsonFormatMessage << "{";
        for ( uint32_t j = 0; j < objArray.size(); ++j )
        {
            SimpleObj currentObj = objArray[j];
            jsonFormatMessage << "\"" << currentObj.name << "\":";

            if (currentObj.isString)
            {
                string value = currentObj.strValue;
                if (value[0] == '[')
                {
                    jsonFormatMessage << value;
                }
                else
                {
                    jsonFormatMessage << "\"" << value << "\"";
                }
            }
            else
            {
                jsonFormatMessage << currentObj.intValue;
            }
            if ( j < objArray.size() - 1) {
                jsonFormatMessage << ",";
            }
        }
        jsonFormatMessage << "}";
        if ( i < dataArray.size() - 1 ) {
            jsonFormatMessage << ",";
        }
    }
    jsonFormatMessage << "]";
    return jsonFormatMessage.str();
}
