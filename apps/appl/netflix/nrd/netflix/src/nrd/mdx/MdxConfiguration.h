/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NRDP_MDXCONFIGURATION_H
#define NRDP_MDXCONFIGURATION_H

#include <string>
#include <nrdbase/Configuration.h>

namespace netflix {
namespace mdx {

class Configuration
{
public:
    ~Configuration();

    static ConfigurationOptions getMdxLibOptions();
    static std::string &getInterface() { return sInterface; }
    static std::string &getManufacturer() {     return sManufacturer; }
    static std::string &getModelName()    {     return sModelName; }

private:
    Configuration();

private:
    static std::string sInterface;
    static std::string sManufacturer;
    static std::string sModelName;
};

}} //netflix::mdx
#endif
