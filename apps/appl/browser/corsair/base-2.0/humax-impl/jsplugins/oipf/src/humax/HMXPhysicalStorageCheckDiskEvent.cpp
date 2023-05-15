/*
 * HMXPhysicalStorageCheckDiskEvent.cpp
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXPhysicalStorageCheckDiskEvent.h"
#include "HMXPhysicalStorage.h"

namespace Ooif {

HMXPhysicalStorageCheckDiskEvent::HMXPhysicalStorageCheckDiskEvent(jsplugin_obj *c, int nDevidx) :
		Event(c, "PhysicalStorageCheckDisk")
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageCheckDiskEvent 0x%08x created\n", (unsigned int)this);
	mDevIdx = nDevidx;
}

HMXPhysicalStorageCheckDiskEvent::~HMXPhysicalStorageCheckDiskEvent()
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageCheckDiskEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXPhysicalStorageCheckDiskEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageCheckDiskEvent::getter [%s]\n", name);
	GET_NUMBER("DeviceIndex", mDevIdx, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMXPhysicalStorageCheckDiskEvent::serialize(jsplugin_value** argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = mDevIdx;

	return 1;
}

} /* namespace Ooif */
