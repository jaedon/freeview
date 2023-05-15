/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASE_SIMPLE_JSON_ARRAY_SERIALIZER_H
#define ASE_SIMPLE_JSON_ARRAY_SERIALIZER_H

#include <ctype.h>
#include <string>
#include <vector>

namespace netflix {
namespace ase {
/**
 * @class AseSimpleJsonArraySerializer
 * @brief a class to convert a vector of <string, value> pair into json format
 */
class AseSimpleJsonArraySerializer
{
public:
    class SimpleObj
    {
    public:
        SimpleObj(const std::string &nm, const std::string &val)
        {
            name = nm;
            strValue = val;
            isString = true;
        }

        SimpleObj(const std::string &nm, int val)
        {
            name = nm;
            intValue = val;
            isString = false;
        }

        std::string name;
        std::string strValue;
        int intValue;

        bool isString;
    };

    /**
     * Serialize the specified vector into a json representation
     */
    static std::string toJsonString(const std::vector< std::vector<SimpleObj> >& dataArray);
private:
    /** private constructor */
    AseSimpleJsonArraySerializer();
};

}} // namespace netflix::ase


#endif
