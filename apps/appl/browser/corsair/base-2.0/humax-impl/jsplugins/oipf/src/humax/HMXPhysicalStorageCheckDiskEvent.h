/*
 * HMXPhysicalStorageCheckDiskEvent.h
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#ifndef HMXPHYSICALSTORAGECHECKDISKEVENT_H_
#define HMXPHYSICALSTORAGECHECKDISKEVENT_H_

#include "Event.h"

class OplPhysicalStorageData;

namespace Ooif {

class HMXPhysicalStorage;
class HMXPhysicalStorageCheckDiskEvent : public Event {
public:
	HMXPhysicalStorageCheckDiskEvent(jsplugin_obj *c, int nDevidx);
	virtual ~HMXPhysicalStorageCheckDiskEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	int mDevIdx;
};

} /* namespace Ooif */
#endif /* HMXPHYSICALSTORAGECHECKDISKEVENT_H_ */
