/*
 * HMXPhysicalStorageDetachedEvent.h
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#ifndef HMXPHYSICALSTORAGEDETACHEDEVENT_H_
#define HMXPHYSICALSTORAGEDETACHEDEVENT_H_

#include "Event.h"

class OplPhysicalStorageData;

namespace Ooif {

class HMXPhysicalStorage;
class HMXPhysicalStorageDetachedEvent : public Event {
public:
	HMXPhysicalStorageDetachedEvent(jsplugin_obj *c, OplPhysicalStorageData* aOplData);
	virtual ~HMXPhysicalStorageDetachedEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value**);
private:
	HMXPhysicalStorage* mPhysicalStorage;
};

} /* namespace Ooif */
#endif /* HMXPHYSICALSTORAGEDETACHEDEVENT_H_ */
