/*
 * HMXLogicalStorage.h
 *
 *  Created on: 2012. 1. 27.
 *      Author: swcho
 */

#ifndef HMXLOGICALSTORAGE_H_
#define HMXLOGICALSTORAGE_H_

#include "EventTarget.h"
#include "WrapperObject.h"
#include "oplstorageutil.h"
#include "NativeHmxStorageUtil.h"

namespace Ooif {

class HMXLogicalStorage :
	public EventTarget,
	public WrapperObject<HMXLogicalStorage>,
	public IOplLogicalStorageListener {
public:
	HMXLogicalStorage(OplLogicalStorageData* aOplData);
	HMXLogicalStorage(const HMXLogicalStorage& aHmxLogicalStorage);
	virtual ~HMXLogicalStorage();
private:
	void init();
private: // from EventTarget
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public: // for DataObject
	static void releaseNative(void* id);
private: // IOplLogicalStorageListener
	virtual void formatProgress(
			const TFormatProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage);
public:
	static int format(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static const ClassType class_name = CLASS_HMX_LOGICALSTORAGE;
private:
	OplLogicalStorageData* mOplData;
};

} /* namespace Ooif */
#endif /* HMXLOGICALSTORAGE_H_ */
