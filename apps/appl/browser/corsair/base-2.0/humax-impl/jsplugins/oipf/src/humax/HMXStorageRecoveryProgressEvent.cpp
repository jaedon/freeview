/*
 * HMXStorageRecoveryProgressEvent.cpp
 *
 *  Created on: 2012. 2. 27.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXStorageRecoveryProgressEvent.h"

namespace Ooif {

HMXStorageRecoveryProgressEvent::HMXStorageRecoveryProgressEvent(
		jsplugin_obj *c,
		OOIFNumber aState,
		OOIFNumber aProgress,
		OOIFNumber aProgressMax,
		const char* aErrMessage) :
		Event(c, "RecoveryProgress"),
		mState(aState),
		mProgress(aProgress),
		mProgressMax(aProgressMax),
		mErrMessage(NULL)
{
	OOIF_LOG_DEBUG("HMXStorageRecoveryProgressEvent 0x%08x created start\n", (unsigned int)this);
	mErrMessage = HLIB_STD_StrDup(aErrMessage);
	OOIF_LOG_DEBUG("HMXStorageRecoveryProgressEvent 0x%08x created end\n", (unsigned int)this);
}

HMXStorageRecoveryProgressEvent::~HMXStorageRecoveryProgressEvent()
{
	if (mErrMessage) HLIB_MEM_Free(mErrMessage);
	OOIF_LOG_DEBUG("HMXStorageRecoveryProgressEvent 0x%08x destroyed\n", (unsigned int)this);
}

int HMXStorageRecoveryProgressEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("state", mState, JSP_GET_VALUE_CACHE);
	GET_NUMBER("progress", mProgress, JSP_GET_VALUE_CACHE);
	GET_NUMBER("progressMax", mProgressMax, JSP_GET_VALUE_CACHE);
	GET_STRING("errMessage", mErrMessage, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMXStorageRecoveryProgressEvent::serialize(jsplugin_value** argv)
{
	OOIF_LOG_DEBUG("HMXStorageRecoveryProgressEvent::serialize start\n");
	*argv = new jsplugin_value[4];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = mState;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = mProgress;
	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = mProgressMax;
	result[3].type = JSP_TYPE_STRING;
	result[3].u.string = mErrMessage;
	OOIF_LOG_DEBUG("HMXStorageRecoveryProgressEvent::serialize end\n");
	return 4;
}

} /* namespace Ooif */
