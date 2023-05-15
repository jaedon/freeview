/*
 * HMXStorageFormatProgressEvent.h
 *
 *  Created on: 2012. 1. 31.
 *      Author: swcho
 */

#ifndef HMXSTORAGEFORMATPROGRESSEVENT_H_
#define HMXSTORAGEFORMATPROGRESSEVENT_H_

#include "Event.h"

namespace Ooif {

class HMXStorageFormatProgressEvent : public Event {
public:
	HMXStorageFormatProgressEvent(
			jsplugin_obj *c,
			OOIFNumber aState,
			OOIFNumber aProgress,
			OOIFNumber aProgressMax,
			const char* aErrMessage);
	virtual ~HMXStorageFormatProgressEvent();
private: // from Event
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value** argv);
private:
	OOIFNumber mState;
	OOIFNumber mProgress;
	OOIFNumber mProgressMax;
	char* mErrMessage;
};

} /* namespace Ooif */
#endif /* HMXSTORAGEFORMATPROGRESSEVENT_H_ */
