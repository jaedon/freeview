/*
 * HMXStorageRecoveryProgressEvent.h
 *
 *  Created on: 2012. 2. 27.
 *      Author: swcho
 */

#ifndef HMXSTORAGERECOVERYPROGRESSEVENT_H_
#define HMXSTORAGERECOVERYPROGRESSEVENT_H_

#include "Event.h"

namespace Ooif {

class HMXStorageRecoveryProgressEvent : public Event {
public:
	HMXStorageRecoveryProgressEvent(
			jsplugin_obj *c,
			OOIFNumber aState,
			OOIFNumber aProgress,
			OOIFNumber aProgressMax,
			const char* aErrMessage);
	virtual ~HMXStorageRecoveryProgressEvent();
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
#endif /* HMXSTORAGERECOVERYPROGRESSEVENT_H_ */
