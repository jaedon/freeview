/*
 * HMXPhysicalStorage.cpp
 *
 *  Created on: 2012. 1. 27.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "oplstorageutil.h"
#include "HMXPhysicalStorage.h"
#include "HMXLogicalStorage.h"
#include "HMXStorageFormatProgressEvent.h"
#include "HMXStorageDetachProgressEvent.h"
#include "HMXStorageRecoveryProgressEvent.h"
#include "HMXStorageUsbSpeedResultEvent.h"

#include "NativeApplicationManager.h"

namespace Ooif {

HMXPhysicalStorage::HMXPhysicalStorage(OplPhysicalStorageData* aOplData) :
		WrapperObject<HMXPhysicalStorage>(aOplData),
		mOplData(aOplData)
{
	init();
}

HMXPhysicalStorage::HMXPhysicalStorage(const HMXPhysicalStorage& aHmxPhysicalStorage) :
		WrapperObject<HMXPhysicalStorage>(aHmxPhysicalStorage.mOplData)
{
	mOplData = aHmxPhysicalStorage.mOplData;
	init();
}

void HMXPhysicalStorage::init()
{
	classType = CLASS_HMX_PHYSICALSTORAGE;
	mOplData->addListener(this);
	OOIF_LOG_DEBUG("HMXPhysicalStorage 0x%08x created =========================\n", (unsigned int)this);
	OOIF_LOG_DEBUG("  id = %d\n", (int)mOplData->id);
	OOIF_LOG_DEBUG("  uuid = %s\n", mOplData->uuid);
	OOIF_LOG_DEBUG("  totalSpace = %d\n", (int)mOplData->totalSize);
}

HMXPhysicalStorage::~HMXPhysicalStorage() {
	mOplData->removeListener(this);
	OOIF_LOG_DEBUG("HMXPhysicalStorage 0x%08x destroyed ======================\n", (unsigned int)this);
}

int HMXPhysicalStorage::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	INTRINSIC_EVENT_GET(FormatProgress);
	INTRINSIC_EVENT_GET(DetachProgress);
	INTRINSIC_EVENT_GET(RecoveryProgress);
	INTRINSIC_EVENT_GET(UsbSpeedResult);

	// OplPhysicalStorageData::TInterface
	GET_NUMBER("EInterfaceUnknown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EInterfaceSATA", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EInterfaceUSB", 2, JSP_GET_VALUE_CACHE);

	// OplPhysicalStorageData::TInterfaceVersion
	GET_NUMBER("EInterfaceVersionUnknown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EInterfaceVersionUSB1", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EInterfaceVersionUSB2", 2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EInterfaceVersionUSB3", 3, JSP_GET_VALUE_CACHE);

	// OplPhysicalStorageData::TType
	GET_NUMBER("ETypeUnknown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ETypeInternal", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ETypeExternal", 2, JSP_GET_VALUE_CACHE);

	// OplPhysicalStorageData::TKind
	GET_NUMBER("EKindUnknown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EKindHDD", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EKindUSBMemory", 2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EKindSDMemory", 3, JSP_GET_VALUE_CACHE);

	// OplPhysicalStorageData::TUsage
	GET_NUMBER("EUsageUnkown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EUsagePVR", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EUsageSubPVR", 2, JSP_GET_VALUE_CACHE); // Detachable......
	GET_NUMBER("EUsageStorage", 3, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EUsageNeedFormat", 4, JSP_GET_VALUE_CACHE);	// DO NOT USE: this usage is moved to isNeedFormat property.
	GET_NUMBER("EUsageRecStorage", 5, JSP_GET_VALUE_CACHE);

	// OplPhysicalStorageData::TParing
	GET_NUMBER("EPairingUnkown", 0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EPairingOK", 1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EPairingFail", 2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EPairingNoInfo", 3, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EPairingNow", 4, JSP_GET_VALUE_CACHE);

	GET_NUMBER("id", mOplData->id, JSP_GET_VALUE_CACHE);
	GET_STRING("label", mOplData->label, JSP_GET_VALUE);
	GET_NUMBER("interfaceType", mOplData->interface, JSP_GET_VALUE_CACHE);
	GET_NUMBER("interfaceVersion", mOplData->interfaceVersion, JSP_GET_VALUE_CACHE);
	GET_NUMBER("type", mOplData->type, JSP_GET_VALUE_CACHE);
	GET_NUMBER("kind", mOplData->kind, JSP_GET_VALUE_CACHE);
	GET_NUMBER("usage", mOplData->usage, JSP_GET_VALUE_CACHE);
	GET_NUMBER("pairing", mOplData->pairing, JSP_GET_VALUE);
	GET_STRING("uuid", mOplData->uuid, JSP_GET_VALUE);
	GET_NUMBER("availableSize", mOplData->availableSize, JSP_GET_VALUE);
	GET_NUMBER("availablePvrSize", mOplData->availablePvrSize, JSP_GET_VALUE);
	GET_NUMBER("usedSize", mOplData->usedSize, JSP_GET_VALUE);
	GET_NUMBER("reservedSize", mOplData->reservedSize, JSP_GET_VALUE);
	GET_NUMBER("totalSize", mOplData->totalSize, JSP_GET_VALUE);
	GET_BOOLEAN("ismount", mOplData->ismount, JSP_GET_VALUE);
	GET_BOOLEAN("isNeedFormat", mOplData->isNeedFormat, JSP_GET_VALUE);

	GET_FUNCTION(getLogicalStorages, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(detach, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(formatAll, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getLogicalStorages, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recovery, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setStorageName, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setSwitchHddForPvr, obj, "sss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(checkUsbSpeed, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
	//return ObjectInstance::getter(obj, name, result);
}

int HMXPhysicalStorage::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::setter [%s]\n", name);
	INTRINSIC_EVENT(FormatProgress);
	INTRINSIC_EVENT(DetachProgress);
	INTRINSIC_EVENT(RecoveryProgress);
	INTRINSIC_EVENT(UsbSpeedResult);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::setter(obj, name, value);
}

void HMXPhysicalStorage::releaseNative(void* id)
{
	OplPhysicalStorageData* oplData = reinterpret_cast<OplPhysicalStorageData*>(id);
	delete oplData;
}

void HMXPhysicalStorage::formatProgress(
		const TFormatProgress& aStatus,
		int aProgress,
		int aProgressMax,
		const char* aErrMessage)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::formatProgress\n");
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXStorageFormatProgressEvent(context, aStatus, aProgress, aProgressMax, aErrMessage);
		dispatchEvent(e);
	}
}

void HMXPhysicalStorage::detachProgress(
		const TDetachProgress& aStatus,
		const char* aErrMessage)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::detachProgress\n");
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXStorageDetachProgressEvent(context, aStatus, aErrMessage);
		dispatchEvent(e);
	}
}

void HMXPhysicalStorage::recoveryProgress(
		const TRecoveryProgress& aStatus,
		int aProgress,
		int aProgressMax,
		const char* aErrMessage)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::recoveryProgress\n");
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXStorageRecoveryProgressEvent(context, aStatus, aProgress, aProgressMax, aErrMessage);
		dispatchEvent(e);
	}
}

void HMXPhysicalStorage::usbSpeedResult(
		int aReadMbps,
		int aWriteMbps)
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::usbSpeedResult\n");
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXStorageUsbSpeedResultEvent(context, aReadMbps, aWriteMbps);
		dispatchEvent(e);
	}
}

int HMXPhysicalStorage::getLogicalStorages(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	list<OplLogicalStorageData*> oplLogicalList = instance->mOplData->getLogicalStoragesDataList();
	HMXLogicalStorageCollection* logicalList = new HMXLogicalStorageCollection();
	list<OplLogicalStorageData*>::iterator i(oplLogicalList.begin());
	while (i != oplLogicalList.end()) {
		logicalList->addItem(new HMXLogicalStorage(*i++));
	}
	OOIF_LOG_DEBUG("HMXPhysicalStorage::getLogicalStorages [%d, %d]\n", oplLogicalList.size(), logicalList->getSize());
	RETURN_OBJECT(this_obj, logicalList, result, JSP_CALL_VALUE);
}

int HMXPhysicalStorage::detach(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::detach\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	instance->mOplData->detach();
	return OOIF_RETURN_OK;
}

int HMXPhysicalStorage::formatAll(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::formatAll\n");

	/* jwkim: all widgets should be removed before HDD format */

	OOIF_LOG_DEBUG("HMXPhysicalStorage::format started.\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);

	OOIF_LOG_DEBUG("HMXPhysicalStorage::formatAll id=%f, label=%s, mountpath=%s, uuid=%s, interface=%f,\
		kind=%f, type=%f\n", instance->mOplData->id, instance->mOplData->label,\
		instance->mOplData->mountpath, instance->mOplData->uuid,\
		instance->mOplData->type, instance->mOplData->kind, instance->mOplData->type);
	OOIF_LOG_DEBUG("HMXPhysicalStorage::formatAll id=%f, label=%s, mountpath=%s, uuid=%s, interface=%f,\
		kind=%f, type=%f\n", instance->mOplData->id, instance->mOplData->label,\
		instance->mOplData->mountpath, instance->mOplData->uuid,\
		instance->mOplData->type, instance->mOplData->kind, instance->mOplData->type);

	if ( instance->mOplData->id == 0 ) {	/* removes all widgets when internal HDD format */
		int count;
		WidgetDescriptorHandle *handles = NULL;
		native_AppManGetWidgets(&count, &handles);

		for( int i= 0; i< count; i++ )
		{
			if ( native_AppManUninstallWidget(handles[i]) != OOIF_RETURN_OK ) continue;
		}
		if ( handles != NULL )
			HLIB_STD_MemFree(handles);
	}

	if (argc > 1) {
		instance->mOplData->formatAll(STRING_VALUE(argv[0]));
	}
	else {
		instance->mOplData->formatAll("");
	}
	return OOIF_RETURN_OK;
}

int HMXPhysicalStorage::recovery(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::recovery\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	instance->mOplData->recovery();
	return OOIF_RETURN_OK;
}

int HMXPhysicalStorage::setStorageName(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::setStorageName\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	instance->mOplData->setStorageName(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]));
	return OOIF_RETURN_OK;
}

int HMXPhysicalStorage::setSwitchHddForPvr(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::setSwitchHddForPvr\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	instance->mOplData->setSwitchHddForPvr(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), STRING_VALUE(argv[2]));
	return OOIF_RETURN_OK;
}

int HMXPhysicalStorage::checkUsbSpeed(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	OOIF_LOG_DEBUG("HMXPhysicalStorage::checkUsbSpeed\n");
	HMXPhysicalStorage* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXPhysicalStorage);
	instance->mOplData->checkUsbSpeed(instance->mOplData->uuid);
	return OOIF_RETURN_OK;
}

} /* namespace Ooif */
