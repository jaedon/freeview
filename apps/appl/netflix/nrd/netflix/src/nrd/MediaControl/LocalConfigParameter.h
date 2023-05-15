/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __LOCAL_CONFIG_PARAMETER_H__
#define __LOCAL_CONFIG_PARAMETER_H__

#include <map>
#include <string>

#include <nrdase/IAseConfigParameter.h>

namespace netflix {

namespace mediacontrol {

/**
 * @class LocalConfigParameter
 * @brief implementation of IAseConfigParameter for local config parameters
 */
class LocalConfigParameter : public ase::IAseConfigParameter
{
public:
    LocalConfigParameter();

    virtual std::string queryParameter(const std::string &key) const;
    virtual void setParameter(const std::string &key, const std::string &value);

    virtual std::map<uint32_t, uint32_t>
        loadConfigMap( const std::string& /** paraKey */, const std::string& /** key1 */, const std::string& /** key2 */, uint32_t /** minKeyVal */)
        {
            std::map<uint32_t, uint32_t> configMap;
            return configMap;
        }

    /**
     * output all configured parameters in a string
     */
    std::string toString();

    /** reload the parameters from the system store
     * These parameters are actually from web api
     */
    virtual void reloadSystemParameters();
protected:
    std::map<std::string, std::string> mParams;
};

}}

#endif // __STREAMING_CONFIG_PARAMETER_H__

