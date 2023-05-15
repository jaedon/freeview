/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "FileSystem.h"

using namespace netflix::device;
using namespace netflix;

std::vector< struct ISystem::NetworkInterface > FileSystem::getNetworkInterfaces()
{
    std::vector< struct ISystem::NetworkInterface > listnf;
    return listnf;
}
