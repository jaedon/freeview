/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SystemBridge.h"
#include <nrd/NrdApplication.h>
#include <nrd/NrdConfiguration.h>

#include <nrdbase/Base64.h>
#include <nrdbase/StringCompressor.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Variant.h>
#include <Request.h>
#include <nrd/config.h>

using namespace netflix;

#define SystemBridgeDevicePropertyList(Property)    \
    Property(bootUrl, NfObject::ReadOnly)           \
    Property(bootUrlTime, NfObject::ReadOnly)       \
    Property(uiQueryString, NfObject::ReadWrite)

PropertyTable(SystemBridge, SystemBridgeDevicePropertyList, properties)

SystemBridge::SystemBridge()
    : NfObject("system", Properties::SystemBridge::properties, Properties::SystemBridge::PropertyCount),
      mUiQueryString(NrdConfiguration::queryString())
{
}

bool SystemBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::SystemBridge::bootUrl:
        *result = NrdConfiguration::bootRequest().value("url");
        break;
    case Properties::SystemBridge::bootUrlTime:
        *result = nrdApp()->monoFirstLocationSet().ms();
        break;
    case Properties::SystemBridge::uiQueryString:
        *result = mUiQueryString;
        break;
    default:
        return false;
    }
    return true;
}

bool SystemBridge::setProperty(int index, const Variant &value)
{
    if (index == Properties::SystemBridge::uiQueryString) {
        mUiQueryString = value.value<std::string>();
        propertyUpdated(Properties::SystemBridge::uiQueryString);
        return true;
    }
    return false;
}
