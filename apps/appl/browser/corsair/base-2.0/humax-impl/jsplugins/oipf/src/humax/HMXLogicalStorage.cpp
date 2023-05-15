/*
 * HMXLogicalStorage.cpp
 *
 *  Created on: 2012. 1. 27.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXLogicalStorage.h"
#include "HMXStorageFormatProgressEvent.h"

namespace Ooif {

HMXLogicalStorage::HMXLogicalStorage(OplLogicalStorageData* aOplData) :
		WrapperObject<HMXLogicalStorage>(aOplData),
		mOplData(aOplData)
{
	init();
	OOIF_LOG_DEBUG("HMXLogicalStorage 0x%08x created\n", (unsigned int)this);
	OOIF_LOG_DEBUG("  id = %d\n", (int)mOplData->id);
	OOIF_LOG_DEBUG("  label = %s\n", mOplData->label);
	OOIF_LOG_DEBUG("  totalSpace = %d\n", (int)mOplData->totalSize);
}

HMXLogicalStorage::HMXLogicalStorage(const HMXLogicalStorage& aHmxLogicalStorage) :
		WrapperObject<HMXLogicalStorage>(aHmxLogicalStorage.mOplData)
{
	mOplData = aHmxLogicalStorage.mOplData;
	init();
	OOIF_LOG_DEBUG("HMXLogicalStorage 0x%08x created\n", (unsigned int)this);
}

void HMXLogicalStorage::init()
{
	classType = CLASS_HMX_LOGICALSTORAGE;
	mOplData->addListener(this);
}

HMXLogicalStorage::~HMXLogicalStorage() {
	mOplData->removeListener(this);
	OOIF_LOG_DEBUG("HMXLogicalStorage 0x%08x destroyed\n", (unsigned int)this);
}

int HMXLogicalStorage::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	INTRINSIC_EVENT_GET(FormatProgress);

	GET_NUMBER("id", mOplData->id, JSP_GET_VALUE);
	GET_NUMBER("available", mOplData->available, JSP_GET_VALUE);
	GET_NUMBER("pvr", mOplData->pvr, JSP_GET_VALUE);
	GET_NUMBER("availableSize", mOplData->availableSize, JSP_GET_VALUE);
	GET_NUMBER("usedSize", mOplData->usedSize, JSP_GET_VALUE);
	GET_NUMBER("reservedSize", mOplData->reservedSize, JSP_GET_VALUE);
	GET_NUMBER("totalSpace", mOplData->totalSize, JSP_GET_VALUE);
	GET_STRING("formatType", mOplData->formatType, JSP_GET_VALUE);
	GET_BOOLEAN("available", mOplData->available, JSP_GET_VALUE);
	GET_STRING("path", mOplData->path, JSP_GET_VALUE);
	GET_STRING("label", mOplData->label, JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::getter(obj, name, result);
}

int HMXLogicalStorage::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	OOIF_LOG_DEBUG("HMXLogicalStorage::setter [%s]\n", name);
	INTRINSIC_EVENT(FormatProgress);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::setter(obj, name, value);
}

void HMXLogicalStorage::releaseNative(void* aHandle)
{
	OplLogicalStorageData* oplData = reinterpret_cast<OplLogicalStorageData*>(aHandle);
	delete oplData;
}

void HMXLogicalStorage::formatProgress(
		const TFormatProgress& aStatus,
		int aProgress,
		int aProgressMax,
		const char* aErrMessage)
{
	OOIF_LOG_DEBUG("HMXLogicalStorage::formatProgress\n");
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXStorageFormatProgressEvent(context, aStatus, aProgress, aProgressMax, aErrMessage);
		dispatchEvent(e);
	}
}

int HMXLogicalStorage::format(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HmxLogicalStorage::format");
	return OOIF_RETURN_OK;
}

} /* namespace Ooif */
