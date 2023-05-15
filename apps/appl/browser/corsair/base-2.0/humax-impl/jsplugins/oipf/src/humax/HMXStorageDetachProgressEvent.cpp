/*
 * HMXStorageDetachProgressEvent.cpp
 *
 *  Created on: 2012. 2. 10.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "HMXStorageDetachProgressEvent.h"

namespace Ooif {

HMXStorageDetachProgressEvent::HMXStorageDetachProgressEvent(
		jsplugin_obj *c,
		OOIFNumber aState,
		const char* aErrMessage) :
		Event(c, "DetachProgress"),
		mState(aState),
		mErrMessage(NULL)
{
	mErrMessage = HLIB_STD_StrDup(aErrMessage);
}

HMXStorageDetachProgressEvent::~HMXStorageDetachProgressEvent()
{
	if (mErrMessage) HLIB_MEM_Free(mErrMessage);
}

int HMXStorageDetachProgressEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("state", mState, JSP_GET_VALUE_CACHE);
	GET_STRING("errMessage", mErrMessage, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMXStorageDetachProgressEvent::serialize(jsplugin_value** argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = mState;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = mErrMessage;
	return 2;
}

} /* namespace Ooif */
