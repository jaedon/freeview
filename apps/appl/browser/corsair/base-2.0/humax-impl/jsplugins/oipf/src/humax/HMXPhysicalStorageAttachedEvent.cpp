/*
 * HMXPhysicalStorageAttachedEvent.cpp
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXPhysicalStorageAttachedEvent.h"
#include "HMXPhysicalStorage.h"

namespace Ooif {

HMXPhysicalStorageAttachedEvent::HMXPhysicalStorageAttachedEvent(jsplugin_obj *c, OplPhysicalStorageData* aOplData) :
		Event(c, "PhysicalStorageAttached")
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageAttachedEvent 0x%08x created\n", (unsigned int)this);
	mPhysicalStorage = new HMXPhysicalStorage(aOplData);
}

HMXPhysicalStorageAttachedEvent::~HMXPhysicalStorageAttachedEvent()
{
	delete mPhysicalStorage;
	OOIF_LOG_DEBUG("HMXPhysicalStorageAttachedEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXPhysicalStorageAttachedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageAttachedEvent::getter [%s]\n", name);
	GET_OBJECT("physicalStorage", obj, new HMXPhysicalStorage(*mPhysicalStorage), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMXPhysicalStorageAttachedEvent::serialize(jsplugin_value** argv)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageAttachedEvent::serialize\n");
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	SET_SERIALIZED_OBJECT( new HMXPhysicalStorage(*mPhysicalStorage), 0);
	return 1;
}

} /* namespace Ooif */
