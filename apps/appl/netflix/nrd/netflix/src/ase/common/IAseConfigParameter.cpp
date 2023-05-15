/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/IAseConfigParameter.h>

#include <algorithm>
#include <functional>
#include <cctype>

using namespace netflix;
using namespace netflix::ase;

template<>
bool IAseConfigParameter::getParameterAs<bool>( const std::string& paraKey, const bool& def )
{
    std::string s = queryParameter( paraKey );

    std::transform( s.begin(), s.end(), s.begin(), std::ptr_fun<int,int>(std::tolower) );

    return ( s == "1" || s == "true" ) ? true
                : ( s == "0" || s == "false" ) ? false
                : def;
}



