/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "MdxConfiguration.h"

using namespace std;
using namespace netflix;

string netflix::mdx::Configuration::sInterface = "default";
string netflix::mdx::Configuration::sManufacturer= "";
string netflix::mdx::Configuration::sModelName = "";

ConfigurationOptions netflix::mdx::Configuration::getMdxLibOptions()
{
    ConfigurationOptions options;
    options.push_back("MDX Options");
    options.push_back(ConfigurationOption(0, "mdx-interface", ConfigurationOption::STRING_ARGUMENT, &netflix::mdx::Configuration::sInterface, "Network interface to use for UPnP"));
    options.push_back(ConfigurationOption(0, "mdx-manufacturer", ConfigurationOption::STRING_ARGUMENT, &netflix::mdx::Configuration::sManufacturer, "Device Manufacturer returned in dd.xml"));
    options.push_back(ConfigurationOption(0, "mdx-model-name", ConfigurationOption::STRING_ARGUMENT, &netflix::mdx::Configuration::sModelName, "Model name returned in dd.xml"));
    return options;
}

