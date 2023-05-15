/*
 * OplStorageUtil.cpp
 *
 *  Created on: 2012. 1. 19.
 *      Author: swcho
 */

#include "oplstorageutil.h"
#include "oplstorageutilprivate.h"
#include <hlib.h>

STRG_DBG(
	static int gObjCountPhysicalStorageData = 0;
	static int gObjCountLogicalStorageData = 0;
	static int gObjCountOplStorageUtil = 0;
);

OplLogicalStorageData::OplLogicalStorageData() :
	id(0),
	available(false),
	pvr(false),
	availableSize(0),
	usedSize(0),
	reservedSize(0),
	totalSize(0),
	formatType(NULL),
	path(NULL),
	label(NULL),
	p(NULL)
{
	STRG_DBG(
		HxLOG_Print("OplLogicalStorageData 0x%08x created\n", (unsigned int)this);
		gObjCountLogicalStorageData++;
	);
}

OplLogicalStorageData::~OplLogicalStorageData()
{
	if (formatType) HLIB_STD_MemFree(formatType);
	if (path) HLIB_STD_MemFree(path);
	if (label) HLIB_STD_MemFree(label);
	STRG_DBG(
		HxLOG_Print("OplLogicalStorageData 0x%08x destroyed\n", (unsigned int)this);
		gObjCountLogicalStorageData--;
		HxLOG_Print("Instances %d, %d, %d\n", gObjCountOplStorageUtil, gObjCountLogicalStorageData, gObjCountPhysicalStorageData);
		if (gObjCountOplStorageUtil == 0 && gObjCountLogicalStorageData == 0 && gObjCountPhysicalStorageData == 0) {
			HxLOG_Print("=================== ALL INSTANCES ARE REMOVED!!!! ========================\n");
		}
	);
}

void OplLogicalStorageData::addListener(IOplLogicalStorageListener* aListener)
{
	mListeners.push_back(aListener);
}

void OplLogicalStorageData::removeListener(IOplLogicalStorageListener* aListener)
{
	mListeners.remove(aListener);
}

OplPhysicalStorageData::OplPhysicalStorageData(OplStorageUtilPrivate* aStorageUtil) :
	id(-1),
	pairing(0),
	interface(0),
	interfaceVersion(0),
	type(0),
	kind(0),
	usage(0),
	uuid(NULL),
	availableSize(0),
	availablePvrSize(0),
	usedSize(0),
	reservedSize(0),
	totalSize(0),
	p(aStorageUtil)
{
	STRG_DBG(
		gObjCountPhysicalStorageData++;
		HxLOG_Print("OplPhysicalStorageData 0x%08x created, %d\n", (unsigned int)this, gObjCountPhysicalStorageData);
	);
}

OplPhysicalStorageData::~OplPhysicalStorageData()
{
	p->removePhysicalStorageListener(this);

	if (mountpath) HLIB_STD_MemFree(mountpath);
	if (label) HLIB_STD_MemFree(label);
	if (uuid) HLIB_STD_MemFree(uuid);
	STRG_DBG(
		gObjCountPhysicalStorageData--;
		HxLOG_Print("OplPhysicalStorageData 0x%08x destroyed\n", (unsigned int)this);
		HxLOG_Print("Instances %d, %d, %d\n", gObjCountOplStorageUtil, gObjCountLogicalStorageData, gObjCountPhysicalStorageData);
		if (gObjCountOplStorageUtil == 0 && gObjCountLogicalStorageData == 0 && gObjCountPhysicalStorageData == 0) {
			HxLOG_Print("=================== ALL INSTANCES ARE REMOVED!!!! ========================\n");
		}
	);
}

list<OplLogicalStorageData*> OplPhysicalStorageData::getLogicalStoragesDataList()
{
	return p->getLogicalStoragesDataList(mountpath);
}

void OplPhysicalStorageData::addListener(IOplPhysicalStorageListener* aListener)
{
	if (mListeners.size()==0) {
		p->addPhysicalStorageListener(this);
	}
	mListeners.push_back(aListener);
}

void OplPhysicalStorageData::removeListener(IOplPhysicalStorageListener* aListener)
{
	mListeners.remove(aListener);
	if (mListeners.size()==0) {
		p->removePhysicalStorageListener(this);
	}
}

void OplPhysicalStorageData::formatAll(const char* aLabel)
{
	p->formatAll(id, aLabel);
}

void OplPhysicalStorageData::detach()
{
	p->detach(id);
}

void OplPhysicalStorageData::setStorageName(const char* id, const char* aLabel)
{
	p->setStorageName(id, aLabel);
}

void OplPhysicalStorageData::setSwitchHddForPvr(const char* id, const char* aAssign, const char* disableOther)
{
	p->setSwitchHddForPvr(id, aAssign, disableOther);
}

void OplPhysicalStorageData::checkUsbSpeed(const char* uuid)
{
	p->checkUsbSpeed(uuid);
}

void OplPhysicalStorageData::recovery()
{
	p->recovery(id);
}

OplStorageUtil::OplStorageUtil(IOplStorageListener* aStorageListener) :
	mStorageListener(aStorageListener),
	p(NULL)
{
	STRG_DBG(
		HxLOG_Print("OplStorageUtil::OplStorageUtil\n");
		gObjCountOplStorageUtil++;
	);
	p = OplStorageUtilPrivate::getInstance();
	p->addStorageListener(mStorageListener);
}

OplStorageUtil::~OplStorageUtil()
{
	p->removeStorageListener(mStorageListener);
	p = NULL;
	STRG_DBG(
		gObjCountOplStorageUtil--;
		HxLOG_Print("OplStorageUtil::~OplStorageUtil %d, %d, %d\n", gObjCountOplStorageUtil, gObjCountLogicalStorageData, gObjCountPhysicalStorageData);
		if (gObjCountOplStorageUtil == 0 && gObjCountLogicalStorageData == 0 && gObjCountPhysicalStorageData == 0) {
			HxLOG_Print("=================== ALL INSTANCES ARE REMOVED!!!! ========================\n");
		}
	);
}

const list<const char*> OplStorageUtil::getSupportedFileTypeList()
{
	return p->getSupportedFileTypeList();
}

list<OplPhysicalStorageData*> OplStorageUtil::getPhysicalStoragesDataList()
{
	return p->getPhysicalStoragesDataList();
}

list<OplLogicalStorageData*> OplStorageUtil::getLogicalStoragesDataList(const char* uuid)
{
	return p->getLogicalStoragesDataList(uuid);
}
