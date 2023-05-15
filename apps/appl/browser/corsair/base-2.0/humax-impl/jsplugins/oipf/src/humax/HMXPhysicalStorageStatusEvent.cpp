/*
 * HMXPhysicalStorageStatusEvent.cpp
 *
 *  Created on: 2012. 3. 22.
 *      Author: jhkim5
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXPhysicalStorageStatusEvent.h"
#include "HMXPhysicalStorage.h"

namespace Ooif {

HMXPhysicalStorageStatusEvent::HMXPhysicalStorageStatusEvent(jsplugin_obj *c,OOIFNumber aDevid, OOIFNumber aState) :
		Event(c, "PhysicalStorageStatus"),mDevid(aDevid),mState(aState)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageStautstEvent 0x%08x created\n", (unsigned int)this);
}

HMXPhysicalStorageStatusEvent::~HMXPhysicalStorageStatusEvent()
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageStautstEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXPhysicalStorageStatusEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageStautstEvent::getter [%s]\n", name);
	GET_NUMBER("id", mDevid, JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", mState, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMXPhysicalStorageStatusEvent::serialize(jsplugin_value** argv)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageStatusEvent::serialize\n");
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = mDevid;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = mState;
	return 2;
}

} /* namespace Ooif */
