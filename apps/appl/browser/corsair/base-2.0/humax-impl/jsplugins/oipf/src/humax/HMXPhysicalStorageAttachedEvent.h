/*
 * HMXPhysicalStorageAttachedEvent.h
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#ifndef HMXPHYSICALSTORAGEATTACHEDEVENT_H_
#define HMXPHYSICALSTORAGEATTACHEDEVENT_H_

#include "Event.h"

class OplPhysicalStorageData;

namespace Ooif {

class HMXPhysicalStorage;
class HMXPhysicalStorageAttachedEvent : public Event {
public:
	HMXPhysicalStorageAttachedEvent(jsplugin_obj *c, OplPhysicalStorageData* aOplData);
	virtual ~HMXPhysicalStorageAttachedEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	HMXPhysicalStorage* mPhysicalStorage;
};

} /* namespace Ooif */
#endif /* HMXPHYSICALSTORAGEATTACHEDEVENT_H_ */
