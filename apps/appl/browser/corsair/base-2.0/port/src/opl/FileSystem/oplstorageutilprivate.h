/*
 * oplstorageutilprivate.h
 *
 *  Created on: 2012. 1. 26.
 *      Author: swcho
 */

#ifndef OPLSTORAGEUTILPRIVATE_H_
#define OPLSTORAGEUTILPRIVATE_H_

#include <list>
#include <set>
#include "oplstorageutil.h"
#include "htype.h"
#include "hlib.h"
#include "dlib.h"
#include "apk.h"

class OplStorageUtilPrivate {
public:
	static OplStorageUtilPrivate* getInstance();
	virtual ~OplStorageUtilPrivate();
public: // Utilities
	void addStorageListener(OplStorageUtil::IOplStorageListener* aStorageListener);
	void removeStorageListener(OplStorageUtil::IOplStorageListener* aStorageListener);
	void addPhysicalStorageListener(OplPhysicalStorageData* aPhysicalStorageData);
	void removePhysicalStorageListener(OplPhysicalStorageData* aPhysicalStorageData);
	void addLogicalStorageListener(OplLogicalStorageData* aLogicalStorageData);
	void removeLogicalStorageListener(OplLogicalStorageData* aLogicalStorageData);
private:
	OplStorageUtilPrivate();
	void init();
	void printDeviceInfo(DxStorage_Info_t *info);
	const list<const char*> getSupportedFileTypeList();
	list<OplPhysicalStorageData*> getPhysicalStoragesDataList();
	list<OplLogicalStorageData*> getLogicalStoragesDataList(const char* mountpath);
	void formatAll(HUINT32 aDeviceId, const char* aLabel);
	void detach(HUINT32 aDeviceId);
	void recovery(HUINT32 aDeviceId);
	void setStorageName(const HCHAR* uuid, const HCHAR* label);
	void setSwitchHddForPvr(const HCHAR* ucStorageId, const HCHAR* assign, const HCHAR* disableOther);
	void checkUsbSpeed(const HCHAR* uuid);
	void updateDeviceProgress(DxStorage_Progress_t* pstProgress);
	OplPhysicalStorageData* createPhysicalStorageData(const DxStorage_Info_t * aDevInfo);
	OplLogicalStorageData* createLogicalStorageData(const DxStorage_PartitionInfo_t * aDevInfo, HUINT32 ulIdx, HUINT64 reservedPvrSizeKb);

	void notifyDeviceAttach(OplPhysicalStorageData* aOplPhysicalStorageData);
	void notifyDeviceDetach(OplPhysicalStorageData* aOplPhysicalStorageData);
	void notifyDeviceStatus(HUINT32 aDeviceId, HUINT32 aStatus);
	void notifyFormatProgress(
			HUINT32 aDeviceId,
			const TFormatProgress& aStatus,
			int aProgress,
			int aProgressMax);
	void notifyDetachProgress(
			HUINT32 aDeviceId,
			const TDetachProgress& aStatus);
	void notifyRecoveryProgress(
			HUINT32 aDeviceId,
			const TRecoveryProgress& aStatus,
			int aProgress,
			int aProgressMax);
	void notifyUsbSpeedResult(
			HUINT32 aDeviceId,
			int aReadMbps,
			int aWriteMbps);
public: // Callback function for Appkit layer

	static void DeviceInfoNotifier(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3);

private:
	list<OplStorageUtil::IOplStorageListener*> mListeners;
	list<OplPhysicalStorageData*> mPhysicalStorageDataForNotify;
	list<OplLogicalStorageData*> mLogicalStorageDataForNotify;
	set<int> mFormatEventTriggerdId;
	set<int> mDstEventTriggerdId;
	friend class OplStorageUtil;
	friend class OplPhysicalStorageData;
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	HBOOL	bFtpServerStateBeforeUnpairing;
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	HBOOL	bSambaServerStateBeforeUnpairing;
#endif
};

#endif /* OPLSTORAGEUTILPRIVATE_H_ */
