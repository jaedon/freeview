/*
 * HMXPhysicalStorage.h
 *
 *  Created on: 2012. 1. 27.
 *      Author: swcho
 */

#ifndef HMXPHYSICALSTORAGE_H_
#define HMXPHYSICALSTORAGE_H_

#include <list>
#include "EventTarget.h"
#include "WrapperObject.h"
#include "oplstorageutil.h"
#include "Collection.h"
#include "NativeHmxStorageUtil.h"

using namespace std;

class OplPhysicalStorageData;

namespace Ooif {

class HMXPhysicalStorage :
	public EventTarget,
	public WrapperObject<HMXPhysicalStorage>,
	public IOplPhysicalStorageListener {
public:
	HMXPhysicalStorage(OplPhysicalStorageData* aOplData);
	HMXPhysicalStorage(const HMXPhysicalStorage& aHmxPhysicalStorage);
	virtual ~HMXPhysicalStorage();
private:
	void init();
private: // from EventTarget
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public: // for DataObject
	static void releaseNative(void* id);
private: // from OplPhysicalStorageListener
	virtual void formatProgress(
			const TFormatProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage);
	virtual void detachProgress(
			const TDetachProgress& aStatus,
			const char* aErrMessage);
	virtual void recoveryProgress(
			const TRecoveryProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage);
	virtual void usbSpeedResult(
			int aReadMbps,
			int aWriteMbps);
public:
	static int getLogicalStorages(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int detach(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int formatAll(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int recovery(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int setStorageName(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int setSwitchHddForPvr(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int checkUsbSpeed(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static const ClassType class_name = CLASS_HMX_PHYSICALSTORAGE;
private:
	OplPhysicalStorageData* mOplData;
};

} /* namespace Ooif */
#endif /* HMXPHYSICALSTORAGE_H_ */
