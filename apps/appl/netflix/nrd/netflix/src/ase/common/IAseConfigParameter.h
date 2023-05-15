/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_ASE_CONFIG_PARAMETER_H
#define I_ASE_CONFIG_PARAMETER_H

#include <string>
#include <sstream>
#include <map>
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {
/**
 * @class IAseConfigParameter
 * @brief The interface to provide streaming parameters
 */
class IAseConfigParameter
{
public:
    typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;
    virtual ~IAseConfigParameter() {}

    virtual std::string queryParameter(const std::string& paraKey) const = 0;

    virtual void setParameter(const std::string& paraKey, const std::string& paraValue) = 0;

    template<typename T>
    T getParameterAs(const std::string& paraKey,
                     const T& minVal, const T& maxVal, const T& defaultVal);

    template<typename T>
    T getParameterAs( const std::string& paraKey, const T& def );

    template<typename T>
    void setParameter( const std::string& paraKey, const T& paraValue );

    virtual std::map<uint32_t, uint32_t>
        loadConfigMap( const std::string& paraKey, const std::string& key1, const std::string& key2, uint32_t minKeyVal = 0) = 0;

    virtual void reloadSystemParameters() = 0;
};

typedef IAseConfigParameter::IAseConfigParameterPtr IAseConfigParameterPtr;

int32_t constructConfigParameter(IAseConfigParameterPtr& pConfigParameter);

template<typename T>
T IAseConfigParameter::getParameterAs( const std::string& paraKey, const T& def )
{
    std::string const s = queryParameter( paraKey );
    if ( s.empty() )
    {
        return def;
    }

    T retVal = def;
    std::istringstream iss(s);
    return (iss >> retVal).fail() ? def : retVal;
}

template<typename T>
T IAseConfigParameter::getParameterAs(const std::string& paraKey,
                                      const T& minVal, const T& maxVal,
                                      const T& defaultVal)
{
    T retVal = getParameterAs<T>(paraKey, defaultVal);

    if ((retVal < minVal) || (retVal > maxVal))
    {
        retVal = defaultVal;
    }

    return retVal;
}

template<>
bool IAseConfigParameter::getParameterAs<bool>( const std::string& paraKey, const bool& def );

template<typename T>
void IAseConfigParameter::setParameter( const std::string& paraKey, const T& paraValue )
{
    std::ostringstream oss;
    oss << paraValue;
    setParameter( paraKey, oss.str() );
}

}}

#endif
