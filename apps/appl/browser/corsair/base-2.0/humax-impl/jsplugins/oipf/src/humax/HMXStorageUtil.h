/*
 * HMXStorageUtil.h
 *
 *  Created on: 2012. 1. 19.
 *      Author: swcho
 */

#ifndef HMXSTORAGEUTIL_H_
#define HMXSTORAGEUTIL_H_

#include "ObjectInstance.h"
#include "oplstorageutil.h"

class OplPhysicalStorageData;

namespace Ooif {

class HMXStorageUtil : public EventTarget, public OplStorageUtil::IOplStorageListener {
public:
	HMXStorageUtil();
	virtual ~HMXStorageUtil();
private: // from EventTarget
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int destructor(jsplugin_obj *obj);
	virtual void gc(jsplugin_obj *obj);
	virtual void attr_change(jsplugin_obj *obj, const char *name, const char *value);
private: // from OplStorageUtil::IOplStorageListener
	virtual bool physicalStorageAttached(OplPhysicalStorageData* aData);
	virtual bool physicalStorageDetached(OplPhysicalStorageData* aData);
	virtual bool physicalStorageCheckDisk(int nDevIdx);
	virtual bool physicalStorageStatus(unsigned int aDevId, unsigned int aStatus);
public:
	static int getPhysicalStorages(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static int supportedFormatTypes(
			jsplugin_obj *this_obj,
			jsplugin_obj *function_obj,
			int argc,
			jsplugin_value *argv,
			jsplugin_value *result );
	static const ClassType class_name = CLASS_HMX_STORAGEUTIL;
private:
	OplStorageUtil* mOplUtil;
};

}
#endif /* HMXSTORAGEUTIL_H_ */
