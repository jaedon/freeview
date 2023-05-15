/*
 * HMXStorageUsbSpeedResultEvent.h
 *
 *  Created on: 07-MAR-2014
 *      Author: yhkim2
 */

#ifndef HMXSTORAGEUSBSPEEDRESULTEVENT_H_
#define HMXSTORAGEUSBSPEEDRESULTEVENT_H_

#include "Event.h"

namespace Ooif {

class HMXStorageUsbSpeedResultEvent : public Event {
public:
	HMXStorageUsbSpeedResultEvent(
		jsplugin_obj *c,
		OOIFNumber aReadMbps,
		OOIFNumber aWriteMbps);
	virtual ~HMXStorageUsbSpeedResultEvent();
private:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	OOIFNumber mReadMbps;
	OOIFNumber mWriteMbps;
};

} /* namespace Ooif */
#endif /* HMXSTORAGEUSBSPEEDRESULTEVENT_H_ */
