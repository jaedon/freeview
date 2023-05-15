/*
 * HMXStorageUsbSpeedResultEvent.cpp
 *
 *  Created on: 07-MAR-2014
 *      Author: yhkim2
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXStorageUsbSpeedResultEvent.h"

namespace Ooif {

HMXStorageUsbSpeedResultEvent::HMXStorageUsbSpeedResultEvent(
		jsplugin_obj *c,
		OOIFNumber aReadMbps,
		OOIFNumber aWriteMbps) :
		Event(c, "UsbSpeedResult"),
		mReadMbps(aReadMbps),
		mWriteMbps(aWriteMbps)
{
	OOIF_LOG_DEBUG("HMXStorageUsbSpeedResultEvent 0x%08x created start\n", (unsigned int)this);
	OOIF_LOG_DEBUG("HMXStorageUsbSpeedResultEvent 0x%08x created end\n", (unsigned int)this);
}

HMXStorageUsbSpeedResultEvent::~HMXStorageUsbSpeedResultEvent()
{
	OOIF_LOG_DEBUG("HMXStorageUsbSpeedResultEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXStorageUsbSpeedResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("ReadMbps", mReadMbps, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WriteMbps", mWriteMbps, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMXStorageUsbSpeedResultEvent::serialize(jsplugin_value** argv)
{
	OOIF_LOG_DEBUG("HMXStorageUsbSpeedResultEvent::serialize start\n");
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = mReadMbps;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = mWriteMbps;
	OOIF_LOG_DEBUG("HMXStorageUsbSpeedResultEvent::serialize end\n");
	return 2;
}

} /* namespace Ooif */
