/*
 * HMXPhysicalStorageDetachedEvent.cpp
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXPhysicalStorageDetachedEvent.h"
#include "HMXPhysicalStorage.h"

namespace Ooif {

HMXPhysicalStorageDetachedEvent::HMXPhysicalStorageDetachedEvent(jsplugin_obj *c, OplPhysicalStorageData* aOplData) :
		Event(c, "PhysicalStorageDetached")
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageDetachedEvent 0x%08x created\n", (unsigned int)this);
	mPhysicalStorage = new HMXPhysicalStorage(aOplData);
}

HMXPhysicalStorageDetachedEvent::~HMXPhysicalStorageDetachedEvent()
{
	delete mPhysicalStorage;
	OOIF_LOG_DEBUG("HMXPhysicalStorageDetachedEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXPhysicalStorageDetachedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageDetachedEvent::getter [%s]\n", name);
	GET_OBJECT("physicalStorage", obj, new HMXPhysicalStorage(*mPhysicalStorage), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMXPhysicalStorageDetachedEvent::serialize(jsplugin_value** argv)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorageDetachedEvent::serialize\n");
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	SET_SERIALIZED_OBJECT( new HMXPhysicalStorage(*mPhysicalStorage), 0);
	return 1;
}

} /* namespace Ooif */
