/*
 * HMXStorageDetachProgressEvent.h
 *
 *  Created on: 2012. 2. 10.
 *      Author: swcho
 */

#ifndef HMXSTORAGEDETACHPROGRESSEVENT_H_
#define HMXSTORAGEDETACHPROGRESSEVENT_H_

#include "Event.h"

namespace Ooif {

class HMXStorageDetachProgressEvent : public Event{
public:
	HMXStorageDetachProgressEvent(
			jsplugin_obj *c,
			OOIFNumber aState,
			const char* aErrMessage);
	virtual ~HMXStorageDetachProgressEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	OOIFNumber mState;
	char* mErrMessage;
};

} /* namespace Ooif */
#endif /* HMXSTORAGEDETACHPROGRESSEVENT_H_ */
