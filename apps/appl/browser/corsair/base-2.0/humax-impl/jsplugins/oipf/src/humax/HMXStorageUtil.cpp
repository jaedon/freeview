/*
 * HMXStorageUtil.cpp
 *
 *  Created on: 2012. 1. 19.
 *      Author: swcho
 */

#include <string.h>

#include "macros.h"
#include "OOIFUtils.h"
#include "HMXStorageUtil.h"
#include "HMXPhysicalStorage.h"
#include "HMXLogicalStorage.h"
#include "HMXPhysicalStorageAttachedEvent.h"
#include "HMXPhysicalStorageDetachedEvent.h"
#include "HMXPhysicalStorageCheckDiskEvent.h"
#include "HMXPhysicalStorageStatusEvent.h"

// OPL
using namespace Ooif;

HMXStorageUtil::HMXStorageUtil() :
		mOplUtil(NULL)
{
	ENTRY;
	classType = CLASS_HMX_STORAGEUTIL;
	mOplUtil = new OplStorageUtil(this);
}

HMXStorageUtil::~HMXStorageUtil()
{
	ENTRY;
	if (mOplUtil) delete mOplUtil;
}

int HMXStorageUtil::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	INTRINSIC_EVENT_GET(PhysicalStorageAttached);
	INTRINSIC_EVENT_GET(PhysicalStorageDetached);
	INTRINSIC_EVENT_GET(PhysicalStorageStatus);
	GET_FUNCTION(getPhysicalStorages, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(supportedFormatTypes, obj, "", JSP_GET_VALUE_CACHE);

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
	//return ObjectInstance::getter(obj, name, result);
}

int HMXStorageUtil::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(PhysicalStorageAttached);
	INTRINSIC_EVENT(PhysicalStorageDetached);
	INTRINSIC_EVENT(PhysicalStorageStatus);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::setter(obj, name, value);
}

int HMXStorageUtil::destructor(jsplugin_obj *obj)
{
	ENTRY;
	return ObjectInstance::destructor(obj);
}

void HMXStorageUtil::gc(jsplugin_obj *obj)
{
//	OOIF_LOG_DEBUG ("HMXStorageUtil::gc\n");
	return ObjectInstance::gc(obj);
}

void HMXStorageUtil::attr_change(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
	return ObjectInstance::attr_change(obj, name, value);
}

bool HMXStorageUtil::physicalStorageAttached(OplPhysicalStorageData* aData)
{
	ENTRY;
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXPhysicalStorageAttachedEvent(context, aData);
		dispatchEvent(e);
	}
	return true;
}

bool HMXStorageUtil::physicalStorageDetached(OplPhysicalStorageData* aData)
{
	ENTRY;
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXPhysicalStorageDetachedEvent(context, aData);
		dispatchEvent(e);
	}
	return true;
}

bool HMXStorageUtil::physicalStorageCheckDisk(int nDevIdx)
{
	ENTRY;
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXPhysicalStorageCheckDiskEvent(context, nDevIdx);
		dispatchEvent(e);
	}
	return true;
}

bool HMXStorageUtil::physicalStorageStatus(unsigned int aDevId, unsigned int aStatus)
{
	ENTRY;
	Event* e(NULL);
	jsplugin_obj *context = getHost();
	if (context) {
		e = new HMXPhysicalStorageStatusEvent(context,aDevId,aStatus);
		dispatchEvent(e);
	}
	return true;
}


int HMXStorageUtil::getPhysicalStorages(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	ENTRY;
	
	HMXStorageUtil* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXStorageUtil);
	list<OplPhysicalStorageData*> oplPhysicalList = instance->mOplUtil->getPhysicalStoragesDataList();
	HMXPhysicalStorageCollection* physicalList = new HMXPhysicalStorageCollection();
	list<OplPhysicalStorageData*>::iterator i(oplPhysicalList.begin());
	while (i != oplPhysicalList.end()) {
		physicalList->addItem(new HMXPhysicalStorage(*i++));
	}

	RETURN_OBJECT(this_obj, physicalList, result, JSP_CALL_VALUE);
	return JSP_CALL_ERROR;
}

int HMXStorageUtil::supportedFormatTypes(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result )
{
	ENTRY;
	HMXStorageUtil* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HMXStorageUtil);
	const list<const char*> formatTypeList = instance->mOplUtil->getSupportedFileTypeList();
	list<const char*>::const_iterator i(formatTypeList.begin());
	StringCollection *collection = new StringCollection;
	while (i != formatTypeList.end()) {
		collection->addItem(HLIB_STD_StrDup(*i++));
	}
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
	return JSP_CALL_ERROR;
}
