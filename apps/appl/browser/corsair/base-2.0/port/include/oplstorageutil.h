/*
 * OplStorageUtil.h
 *
 *  Created on: 2012. 1. 19.
 *      Author: swcho
 */

#ifndef OPLSTORAGEUTIL_H_
#define OPLSTORAGEUTIL_H_

#include <list>
#include <hlib.h>
using namespace std;

//#define DEBUG_STORAGE_UTIL

#ifdef DEBUG_STORAGE_UTIL
#define STRG_DBG(a)	a
#else
#define STRG_DBG(a)
#endif

enum TFormatProgress
{
	EFormatProgressNone,
	EFormatProgressOnGoing,
	EFormatProgressFinished,
	EFormatProgressError
};

enum TDetachProgress
{
	EDetachProgressNone,
	EDetachProgressOnGoing,
	EDetachProgressFinished,
	EDetachProgressError
};

enum TRecoveryProgress
{
	ERecoveryProgressNone,
	ERecoveryProgressOnGoing,
	ERecoveryProgressFinished,
	ERecoveryProgressError
};

class OplStorageUtilPrivate;

class IOplLogicalStorageListener {
public:
	virtual void formatProgress(
			const TFormatProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage = NULL) = 0;
};

class OplLogicalStorageData
{
public:
	~OplLogicalStorageData();
	void addListener(IOplLogicalStorageListener* aListener);
	void removeListener(IOplLogicalStorageListener* aListener);
public:
	double id;
	bool available;
	bool pvr;
	double availableSize;
	double usedSize;
	double reservedSize;
	double totalSize;
	char* formatType;
	char* path;
	char* label;
private:
	OplLogicalStorageData();
	list<IOplLogicalStorageListener*> mListeners;
	OplStorageUtilPrivate* p;
	friend class OplStorageUtilPrivate;
};

class IOplPhysicalStorageListener {
public:
	virtual void formatProgress(
			const TFormatProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage = NULL) = 0;
	virtual void detachProgress(
			const TDetachProgress& aStatus,
			const char* aErrMessage = NULL) = 0;
	virtual void recoveryProgress(
			const TRecoveryProgress& aStatus,
			int aProgress,
			int aProgressMax,
			const char* aErrMessage = NULL) = 0;
	virtual void usbSpeedResult(
			int aReadMbps,
			int aWriteMbps) = 0;
};

class OplPhysicalStorageData
{
public:
	enum TInterface{
		EInterfaceUnknown,
		EInterfaceSATA,
		EInterfaceUSB,
		EInterfaceSD
	};
	enum TInterfaceVersion{
		EInterfaceVersionUnknown,
		EInterfaceVersionUSB1,
		EInterfaceVersionUSB2,
		EInterfaceVersionUSB3
	};
	enum TType {
		ETypeUnknown,
		ETypeInternal,
		ETypeExternal
	};
	enum TKind {
		EKindUnknown,
		EKindHDD,
		EKindUSBMemory,
		EKindSDMemory
	};
	enum TUsage {
		EUsageUnkown,
		EUsagePVR,
		EUsageSubPVR,
		EUsageStorage,
		EUsageNeedFormat,
		EUsageRecStorage,
	};
	enum TParing {
		EPairingUnkown,
		EPairingOK,
		EPairingFail,
		EPairingNoInfo,
		EPairingNow
	};
public:
	~OplPhysicalStorageData();
	list<OplLogicalStorageData*> getLogicalStoragesDataList();
	void addListener(IOplPhysicalStorageListener* aListener);
	void removeListener(IOplPhysicalStorageListener* aListener);
	void formatAll(const char* aLabel);
	void detach();
	void recovery();
	void setStorageName(const char* uuid, const char* aLabel);
	void checkUsbSpeed(const char* uuid);
	void setSwitchHddForPvr(const char* id, const char* aAssign, const char* disableOther);
public:
	double id;
	double pairing;
	double interface;
	double interfaceVersion;
	double type;
	double kind;
	double usage;
	char* uuid;
	char* label;
	double availableSize;
	double availablePvrSize;
	double usedSize;
	double reservedSize;
	double totalSize;
	bool   ismount;
    char* mountpath;
	bool   isNeedFormat;
private:
	OplPhysicalStorageData(OplStorageUtilPrivate* aStorageUtil);
	list<IOplPhysicalStorageListener*> mListeners;
	OplStorageUtilPrivate* p;
	friend class OplStorageUtilPrivate;
};

class OplStorageUtil {
public:
public:
	class IOplStorageListener {
	public:
		virtual bool physicalStorageAttached(OplPhysicalStorageData* aData) = 0;
		virtual bool physicalStorageDetached(OplPhysicalStorageData* aData) = 0;
		virtual bool physicalStorageCheckDisk(int nDevIdx) = 0;
		virtual bool physicalStorageStatus(unsigned int aDevId, unsigned int aStatus) = 0;
	};
public:
	OplStorageUtil(IOplStorageListener* aStorageListener);
	virtual ~OplStorageUtil();
	const list<const char*> getSupportedFileTypeList();
	list<OplPhysicalStorageData*> getPhysicalStoragesDataList();
	list<OplLogicalStorageData*> getLogicalStoragesDataList(const char* uuid);
private:
	void init();
private:
	IOplStorageListener* mStorageListener;
	OplStorageUtilPrivate* p;
};

#endif /* OPLSTORAGEUTIL_H_ */
