/*
 * HMXPhysicalStorageStatusEvent.h
 *
 *  Created on: 2012. 3. 22.
 *      Author: jhkim5
 */

#ifndef HMXPHYSICALSTORAGESTATUSEVENT_H_
#define HMXPHYSICALSTORAGESTATUSEVENT_H_

#include "Event.h"

class OplPhysicalStorageData;

namespace Ooif {

class HMXPhysicalStorage;
class HMXPhysicalStorageStatusEvent : public Event {
public:
	HMXPhysicalStorageStatusEvent(jsplugin_obj *c,OOIFNumber aDevid, OOIFNumber aState);
	virtual ~HMXPhysicalStorageStatusEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	OOIFNumber mDevid;
	OOIFNumber mState;
};

} /* namespace Ooif */
#endif /* HMXPHYSICALSTORAGESTATUSEVENT_H_ */
