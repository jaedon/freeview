/*
 * oplstorageutilprivate.cpp
 *
 *  Created on: 2012. 1. 26.
 *      Author: swcho
 */

#include "oplstorageutilprivate.h"
#include <hlib.h>

#include <apk.h>
#include "bpldb.h"

#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER) || defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
#define OPL_MAX_PASSWORD_LEN	128
#endif

#define	OPL_STORAGE_5GBPS	((HUINT64)5ULL*1024ULL*1024ULL*1024ULL)

typedef enum
{
	eOPL_STORAGE_EVENT_FormatUnknown	= 0,
	eOPL_STORAGE_EVENT_NeedFormat,			// 이 값을 webapp로 그대로 올려주고 있음 - 순서 절대로  바꾸지 말 것
	eOPL_STORAGE_EVENT_Formatting,
	eOPL_STORAGE_EVENT_FormatDone,
	eOPL_STORAGE_EVENT_FormatFail,
	eOPL_STORAGE_EVENT_PairingDone,			// 5
	eOPL_STORAGE_EVENT_UnPairingDone,
	eOPL_STORAGE_EVENT_PairingFail,
	eOPL_STORAGE_EVENT_UnPairingFail,
	eOPL_STORAGE_EVENT_SetNameFail,
	eOPL_STORAGE_EVENT_DisableHddFail,		// 10
	eOPL_STORAGE_EVENT_SafeRemoveFail,
	eOPL_STORAGE_EVENT_CheckDiskStarted,
	eOPL_STORAGE_EVENT_CheckUsbSpeedDone,
	eOPL_STORAGE_EVENT_SetNameDone,
	eOPL_STORAGE_EVENT_UnmountFail,
	eOPL_STORAGE_EVENT_OverCurrent,			//16
	eOPL_STORAGE_EVENT_OverCurrentDone,
	eOPL_STORAGE_EVENT_Max					// 18
} OPL_STORAGE_EVENT_e;

const HCHAR* KFileSystemNameEXT2 = "EXT2";
const HCHAR* KFileSystemNameEXT3 = "EXT3";
const HCHAR* KFileSystemNameXFS = "XFS";
const HCHAR* KFileSystemNameFAT16 = "FAT16";
const HCHAR* KFileSystemNameFAT32 = "FAT32";
const HCHAR* KFileSystemNameNTFS = "NTFS";
const HCHAR* KFileSystemNameDOS = "DOS";

const HCHAR* const gSupportedFileSystems[] = {
		KFileSystemNameEXT2,
		KFileSystemNameEXT3,
		KFileSystemNameXFS,
		KFileSystemNameFAT16,
		KFileSystemNameFAT32,
		KFileSystemNameNTFS,
		KFileSystemNameDOS
};


static OplPhysicalStorageData::TInterfaceVersion ConvertDeviceInterfaceVer(const DxStorage_Info_t * aDevInfo)
{
	OplPhysicalStorageData::TInterfaceVersion ret(OplPhysicalStorageData::EInterfaceVersionUnknown);


	switch (aDevInfo->eDevType) {

	case eDxSTORAGE_DEVTYPE_USBHDD:
	case eDxSTORAGE_DEVTYPE_USBMEM:
		if (aDevInfo->ullDeviceWriteSpeedBps >= OPL_STORAGE_5GBPS)
		{
			ret = OplPhysicalStorageData::EInterfaceVersionUSB3;
		}
		else
		{
			ret = OplPhysicalStorageData::EInterfaceVersionUSB2;
		}
		break;
	case eDxSTORAGE_DEVTYPE_INTHDD:
	case eDxSTORAGE_DEVTYPE_ESATAHDD:
	case eDxSTORAGE_DEVTYPE_DETACHABLE:
	case eDxSTORAGE_DEVTYPE_MMC:
	case eDxSTORAGE_DEVTYPE_NONE:
	default:
		break;
	}
	return ret;
}

static OplPhysicalStorageData::TInterface ConvertDeviceInterface(const DxStorage_DevType_e eInterface)
{
	OplPhysicalStorageData::TInterface ret(OplPhysicalStorageData::EInterfaceUnknown);

	switch (eInterface) {
	case eDxSTORAGE_DEVTYPE_INTHDD:
	case eDxSTORAGE_DEVTYPE_ESATAHDD:
	case eDxSTORAGE_DEVTYPE_DETACHABLE:
		ret = OplPhysicalStorageData::EInterfaceSATA;
		break;
	case eDxSTORAGE_DEVTYPE_USBHDD:
	case eDxSTORAGE_DEVTYPE_USBMEM:
		ret = OplPhysicalStorageData::EInterfaceUSB;
		break;
	case eDxSTORAGE_DEVTYPE_MMC:
		ret = OplPhysicalStorageData::EInterfaceSD;
		break;
	case eDxSTORAGE_DEVTYPE_NONE:
	default:
		break;
	}
	return ret;
}

static OplPhysicalStorageData::TType ConvertDeviceType(const DxStorage_DevType_e eType)
{
	OplPhysicalStorageData::TType ret(OplPhysicalStorageData::ETypeUnknown);
	switch (eType) {
	case eDxSTORAGE_DEVTYPE_INTHDD:
		ret = OplPhysicalStorageData::ETypeInternal;
		break;
	case eDxSTORAGE_DEVTYPE_USBHDD:
	case eDxSTORAGE_DEVTYPE_ESATAHDD:
	case eDxSTORAGE_DEVTYPE_DETACHABLE:
		ret = OplPhysicalStorageData::ETypeExternal;
		break;
	case eDxSTORAGE_DEVTYPE_NONE:
	default:
		break;
	}
	return ret;
}

static OplPhysicalStorageData::TKind ConvertDeviceKind(const DxStorage_DevType_e eKind)
{
	OplPhysicalStorageData::TKind ret(OplPhysicalStorageData::EKindUnknown);
	switch (eKind) {
	case eDxSTORAGE_DEVTYPE_INTHDD:
	case eDxSTORAGE_DEVTYPE_USBHDD:
	case eDxSTORAGE_DEVTYPE_DETACHABLE:
	case eDxSTORAGE_DEVTYPE_ESATAHDD:
		ret = OplPhysicalStorageData::EKindHDD;
		break;
	case eDxSTORAGE_DEVTYPE_USBMEM:
		ret = OplPhysicalStorageData::EKindUSBMemory;
		break;
	case eDxSTORAGE_DEVTYPE_MMC:
		ret = OplPhysicalStorageData::EKindSDMemory;
		break;
	case eDxSTORAGE_DEVTYPE_NONE:
	default:
		break;
	}
	return ret;
}

static OplPhysicalStorageData::TUsage ConvertDeviceUsage(const HUINT32 ulFlag)
{
	OplPhysicalStorageData::TUsage ret(OplPhysicalStorageData::EUsageUnkown);

	if (ulFlag & eDxSTORAGE_FLAG_SUBPVRDEVICE)
	{
		ret = OplPhysicalStorageData::EUsageSubPVR;
	}
	else if (ulFlag & eDxSTORAGE_FLAG_PVRDEVICE)
	{
		ret = OplPhysicalStorageData::EUsagePVR;
	}
	else if (ulFlag & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
	{
		ret = OplPhysicalStorageData::EUsageRecStorage;
	}
	else if(ulFlag & eDxSTORAGE_FLAG_MOUNTED)
	{
		ret = OplPhysicalStorageData::EUsageStorage;
	}
	else if(ulFlag & eDxSTORAGE_FLAG_NEEDFORMAT)
	{
		ret = OplPhysicalStorageData::EUsageNeedFormat;
	}
	return ret;
}

static OplPhysicalStorageData::TParing ConvertDevicePairing(const HUINT32 ulFlag)
{
	OplPhysicalStorageData::TParing ret(OplPhysicalStorageData::EPairingUnkown);

	if (ulFlag & eDxSTORAGE_FLAG_PAIRED)
	{
		if (ulFlag & eDxSTORAGE_FLAG_NEW2NDHDD)
		{
			ret = OplPhysicalStorageData::EPairingNow;
		}
		else
		{
			ret = OplPhysicalStorageData::EPairingOK;
		}
	}
	else
	{
		ret = OplPhysicalStorageData::EPairingFail;
	}

	return ret;
#if 0

	eDEV_STATUS_PAIRING_OK,
	eDEV_STATUS_PAIRING_FAIL,
	eDEV_STATUS_PAIRING_NO_INFO,
	}



	OplPhysicalStorageData::TParing ret(OplPhysicalStorageData::EPairingUnkown);
	switch (aPairing) {
	case eMS_DEV_STATUS_PairingUnknown:
		ret = OplPhysicalStorageData::EPairingUnkown;
		break;
	case eMS_DEV_STATUS_PairingOk:
		ret = OplPhysicalStorageData::EPairingOK;
		break;
	case eMS_DEV_STATUS_PairingFail:
		ret = OplPhysicalStorageData::EPairingFail;
		break;
	case eMS_DEV_STATUS_PairingNoInfo:
		ret = OplPhysicalStorageData::EPairingNoInfo;
		break;
	case eMS_DEV_STATUS_PairingNow:
		ret = OplPhysicalStorageData::EPairingNow;
		break;
	default:
		break;
	}
	return ret;
#endif
}


static OplStorageUtilPrivate* gInstance = NULL;

OplStorageUtilPrivate* OplStorageUtilPrivate::getInstance()
{
	if (gInstance == NULL) {
		gInstance = new OplStorageUtilPrivate();
		gInstance->init();
	}
	return gInstance;
}

OplStorageUtilPrivate::OplStorageUtilPrivate()
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::OplStorageUtilPrivate\n"));
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	bFtpServerStateBeforeUnpairing = FALSE;
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	bSambaServerStateBeforeUnpairing = FALSE;
#endif
}

OplStorageUtilPrivate::~OplStorageUtilPrivate()
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::~OplStorageUtilPrivate\n"));
}

void OplStorageUtilPrivate::init()
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::init\n"));
	APK_FILEMGR_STORAGE_RegisterNotifier(OplStorageUtilPrivate::DeviceInfoNotifier);
}

void OplStorageUtilPrivate::printDeviceInfo(DxStorage_Info_t *info)
{
	HINT32	i;

	HxLOG_Print("================ Storage Info ================ \n");
	HxLOG_Print("ulDeviceId : [%d]\n", info->ulDeviceId);
	HxLOG_Print("DevType : [%s]\n", APK_FILEMGR_STORAGE_GetDevTypeString(info->eDevType));
	HxLOG_Print("szMountPath : [%s]\n", info->szMountPath);
	HxLOG_Print("szDevName : [%s] \n", info->szDevName);
	HxLOG_Print("szLabel : [%s]\n", info->szLabel);
	HxLOG_Print("szUUID : [%s]\n", info->szUUID);
	HxLOG_Print("ulFlags : [0x%x]\n", info->ulFlags);
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_SECURED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_PAIRED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));

	HxLOG_Print("\n--------Partition Info--------\n");
	for (i=0; i < info->nPartitionNum ; i++)
	{
		HxLOG_Print("\n   [%d] \n",i);
		HxLOG_Print("   bAvailable : [%d]\n", info->astPartitionInfos[i].bAvailable);
		HxLOG_Print("   FsType: [%s]\n",APK_FILEMGR_STORAGE_GetFsString(info->astPartitionInfos[i].eFsType));
		HxLOG_Print("   nPvrIdx : [%d]\n", info->astPartitionInfos[i].nPvrIdx);
		HxLOG_Print("   nPlaybackIdx : [%d]\n", info->astPartitionInfos[i].nPlaybackIdx);
		HxLOG_Print("   szMountPath : [%s]\n", info->astPartitionInfos[i].szMountPath);
		HxLOG_Print("   szTsrPath : [%s]\n", info->astPartitionInfos[i].szTsrPath);
		HxLOG_Print("   szVolumeName : [%s]\n", info->astPartitionInfos[i].szVolumeName);
		HxLOG_Print("   szRecordPath : [%s]\n", info->astPartitionInfos[i].szRecordPath);
		HxLOG_Print("   szMediaVideo : [%s]\n", info->astPartitionInfos[i].szMediaVideo);
		HxLOG_Print("   szMediaAudio : [%s]\n", info->astPartitionInfos[i].szMediaAudio);
		HxLOG_Print("   szMediaPic : [%s]\n", info->astPartitionInfos[i].szMediaPic);
		HxLOG_Print("   ulStartUnit : [%d], ulEndUnit : [%d]\n", info->astPartitionInfos[i].ulStartUnit, info->astPartitionInfos[i].ulEndUnit);
		HxLOG_Print("   ulFlags : [0x%x]\n", info->astPartitionInfos[i].ulFlags);
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE));

	}
	HxLOG_Print("===========================================\n");
}

void OplStorageUtilPrivate::DeviceInfoNotifier(DxStorage_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	ApkFileMgr_FTPS_INFO_t stFtpsInfo;
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	ApkFileMgr_SMBS_INFO_t stSambaInfo;
#endif

	HxLOG_Trace();
	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		HBOOL	bConnect = (HBOOL)p1;
		DxStorage_Info_t	*pstInfo = (DxStorage_Info_t	*)p2;

		HxLOG_Warning("[%s] [%d] ulDeviceId : %d, bConnect : %s \n",__FUNCTION__,__LINE__, pstInfo->ulDeviceId, (bConnect)?"TRUE":"FALSE");

		if(pstInfo->eDevType != eDxSTORAGE_DEVTYPE_INTHDD)
		{
			if (TRUE == bConnect)
			{
				gInstance->notifyDeviceAttach(gInstance->createPhysicalStorageData(pstInfo));
			}
			else
			{
				gInstance->notifyDeviceDetach(gInstance->createPhysicalStorageData(pstInfo));
			}
		}

		if(bConnect == FALSE && (pstInfo->eDevType == eDxSTORAGE_DEVTYPE_INTHDD  || pstInfo->eDevType == eDxSTORAGE_DEVTYPE_DETACHABLE))
		{
			HxLOG_Warning("[%s:%d] ID[%d] Detach event!! APK_META_RECORD_RemoveByStorageID() \n",__FUNCTION__,__LINE__,pstInfo->ulDeviceId);
			APK_META_RECORD_RemoveByStorageID(pstInfo->szUUID);
		}

#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER) || defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
		if (pstInfo->eDevType >= eDxSTORAGE_DEVTYPE_INTHDD  && pstInfo->eDevType <= eDxSTORAGE_DEVTYPE_ESATAHDD)
		{
			//TODO: Need to check HDD is using for FTP/Samba or not
			HBOOL	bOn = FALSE;
			HCHAR szPassword[OPL_MAX_PASSWORD_LEN] = {'\0',};
			HERROR hErr = ERR_FAIL;
			HINT32 nCount;
			HCHAR *pszTmpStr = NULL;
			if (bConnect == TRUE)
			{
			//Start FTP/Samba Server
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
			{
				HxSTD_memset(&stFtpsInfo, 0x00, sizeof(ApkFileMgr_FTPS_INFO_t));
				hErr = APK_FILEMGR_FTPSERVER_GetServerInfo( &stFtpsInfo );
				hErr |= APK_DB_GetInt(USERDB_FTPSERVER_ACTIVATION, (HINT32*)&bOn);
				if (hErr == ERR_OK)
				{
					// Check status after mounting HDD
					if ((FALSE == stFtpsInfo.bStarted) && (TRUE == bOn))
					{
						APK_FILEMGR_FTPSERVER_Start();
						// Restore FTP Server password
						hErr = APK_DB_GetStr(USERDB_FTPSERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
						if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
						{
							APK_FILEMGR_FTPSERVER_SetPassword("admin","", (const HCHAR*)szPassword);
							APK_FILEMGR_FTPSERVER_SetPublicAccess("admin", FALSE);
						}
					}
				}
			}
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
			{
				bOn = FALSE;
				HxSTD_StrCpy((HCHAR*)szPassword,"");
				HxSTD_memset(&stSambaInfo, 0x00, sizeof(ApkFileMgr_SMBS_INFO_t));
				hErr = APK_FILEMGR_SAMBASERVER_GetServerInfo( &stSambaInfo );
				hErr |= APK_DB_GetInt(USERDB_SAMBASERVER_ACTIVATION, (HINT32*)&bOn);
				if (hErr == ERR_OK)
				{
					// Check status after mounting HDD
					if ((FALSE == stSambaInfo.bStarted) && (TRUE == bOn))
					{
						APK_FILEMGR_SAMBASERVER_Start();
						// Restore FTP Server password
						hErr = APK_DB_GetStr(USERDB_SAMBASERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
						if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
						{
							APK_FILEMGR_SAMBASERVER_SetPassword("admin","", (const HCHAR*)szPassword);
							APK_FILEMGR_SAMBASERVER_SetPublicAccess("admin", FALSE);
						}
					}
				}
			}
#endif
			}
			else
			{
				//Stop FTP/Samba Server
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
			{
				ApkFileMgr_FTPS_SHARED_t *pstFtpsList = NULL;
				HxLOG_Debug("Called APK_FILEMGR_FTPSERVER_GetSharedList\n");
				if(APK_FILEMGR_FTPSERVER_GetSharedList("admin", &nCount, &pstFtpsList) == ERR_OK)
				{
					// /media/driver1/.... -> remove /media, start from driver1
					if(pstFtpsList)
					{
						pszTmpStr = HxSTD_StrStr((HCHAR*)&pstFtpsList->szPath[7], (const HCHAR*)&pstInfo->szMountPath[7]);
						if (NULL != pszTmpStr)
						{
							HxSTD_memset(&stFtpsInfo, 0x00, sizeof(ApkFileMgr_FTPS_INFO_t));
							APK_FILEMGR_FTPSERVER_GetServerInfo(&stFtpsInfo);
							if (stFtpsInfo.bStarted)
							{
								APK_FILEMGR_FTPSERVER_Stop();
							}
						}
						HLIB_MEM_Free(pstFtpsList);
					}
				}
			}
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
			{
				nCount = 0;
				pszTmpStr = NULL;
				ApkFileMgr_SMBS_SHARED_t *pstSmbsList = NULL;
				HxLOG_Debug("Called APK_FILEMGR_SAMBASERVER_GetSharedList\n");
				if(APK_FILEMGR_SAMBASERVER_GetSharedList("admin", &nCount, &pstSmbsList) == ERR_OK)
				{
					if(pstSmbsList)
					{
						// /media/driver1/.... -> remove /media, start from driver1
						pszTmpStr = HxSTD_StrStr((HCHAR*)&pstSmbsList->szPath[7], (const HCHAR*)&pstInfo->szMountPath[7]);
						if (NULL != pszTmpStr)
						{
							HxSTD_memset(&stSambaInfo, 0x00, sizeof(ApkFileMgr_SMBS_INFO_t));
							APK_FILEMGR_SAMBASERVER_GetServerInfo(&stSambaInfo);
							if(stSambaInfo.bStarted)
							{
								APK_FILEMGR_SAMBASERVER_Stop();
							}
						}
						HLIB_MEM_Free(pstSmbsList);
					}
				}
			}
#endif
			}
		}
#endif

		gInstance->printDeviceInfo(pstInfo);
	}

	else if (eDxSTORAGE_NOTI_NeedFormat == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p1;
		HxLOG_Warning("[%s] [%d]Need format.... deviceId  : %d \n",__FUNCTION__,__LINE__, p1);
		gInstance->notifyDeviceStatus(ulDeviceId, eOPL_STORAGE_EVENT_NeedFormat);
	}
	else if (eDxSTORAGE_NOTI_FormatState == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p2;
		HxLOG_Warning("[%s] [%d] Format state : %d \n",__FUNCTION__,__LINE__, p1);
		switch (p1)
		{
			case eDxSTORAGE_DEV_STATUS_Formatting:
			{
				DxStorage_Progress_t	*pstProgress = (DxStorage_Progress_t *)p2;
				if (pstProgress)
				{
					gInstance->updateDeviceProgress(pstProgress);
				}
				break;
			}
			case eDxSTORAGE_DEV_STATUS_FormatFail:
				if(gInstance->mFormatEventTriggerdId.size() ==0)
					gInstance->mFormatEventTriggerdId.insert(ulDeviceId);
			case eDxSTORAGE_DEV_STATUS_FormatDone:
			{
				set<int>::iterator result = gInstance->mFormatEventTriggerdId.find(ulDeviceId);
				if (result != gInstance->mFormatEventTriggerdId.end())
				{
					gInstance->notifyFormatProgress(
							ulDeviceId,
							(eDxSTORAGE_DEV_STATUS_FormatDone == p1) ? EFormatProgressFinished : EFormatProgressError,
							100,
							100);
					gInstance->mFormatEventTriggerdId.erase(ulDeviceId);
				}
				break;
			}
			default:
				break;

		}

	}
	else if (eDxSTORAGE_NOTI_DstState == eType)
	{
		HxLOG_Warning("[%s] [%d] DST State : %d\n",__FUNCTION__,__LINE__,p1);
		switch (p1)
		{
		case eDxSTORAGE_DEV_STATUS_DstResultOk:
			{
				HUINT32 	ulDeviceId = (HUINT32)p2;
				set<int>::iterator result = gInstance->mDstEventTriggerdId.find(ulDeviceId);
				if (result != gInstance->mDstEventTriggerdId.end())
				{
					gInstance->notifyRecoveryProgress(
							ulDeviceId,
							ERecoveryProgressFinished,
							100,
							100);
					gInstance->mDstEventTriggerdId.erase(ulDeviceId);
				}
			}
			break;

		case eDxSTORAGE_DEV_STATUS_DstResultOnGoing:
			{
				DxStorage_Progress_t	*pstProgress = (DxStorage_Progress_t *)p2;
				if (pstProgress)
				{
					gInstance->updateDeviceProgress(pstProgress);
				}
			}
			break;

		case eDxSTORAGE_DEV_STATUS_DstResultFailNoHDD:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain:
		case eDxSTORAGE_DEV_STATUS_DstResultFailFatalError:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTestElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailElectricalElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailServoElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailReadElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailHadlingDamange:
		case eDxSTORAGE_DEV_STATUS_DstResultFailUnknown:
			{
				HUINT32 	ulDeviceId = (HUINT32)p2;
				if(gInstance->mDstEventTriggerdId.size() ==0)
					gInstance->mDstEventTriggerdId.insert(ulDeviceId);

				set<int>::iterator result = gInstance->mDstEventTriggerdId.find(ulDeviceId);
				if (result != gInstance->mDstEventTriggerdId.end())
				{
					gInstance->notifyRecoveryProgress(
							ulDeviceId,
							ERecoveryProgressError,
							100,
							100);
					gInstance->mDstEventTriggerdId.erase(ulDeviceId);
				}
			}
			break;
		default:
			break;
		}

	}

	else if (eDxSTORAGE_NOTI_OverCurrent == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p1;
		HBOOL 		bOccur = p2 ? TRUE : FALSE;
		OPL_STORAGE_EVENT_e eStorageEvt;

		HxLOG_Warning("[%s] [%d] eOPL_STORAGE_EVENT_OverCurrent!! deviceId(%d), Occurred(%d)\n",__FUNCTION__,__LINE__, p1, bOccur);
		// Add event if it is necessary.
		if ( bOccur )
		eStorageEvt = eOPL_STORAGE_EVENT_OverCurrent;
		else
			eStorageEvt = eOPL_STORAGE_EVENT_OverCurrentDone;
		gInstance->notifyDeviceStatus(ulDeviceId, eStorageEvt);
	}
	else if (eDxSTORAGE_NOTI_UnmountFail == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p1;
		OPL_STORAGE_EVENT_e eStorageEvt;

		// p1 = ulDeviceId
		HxLOG_Warning("[%s] [%d] Unmount Fail!! deviceId : %d \n",__FUNCTION__,__LINE__, p1);

		// Add event if it is necessary.
		eStorageEvt = eOPL_STORAGE_EVENT_UnmountFail;
		gInstance->notifyDeviceStatus(ulDeviceId, eStorageEvt);
	}
	else if (eDxSTORAGE_NOTI_CheckDiskStarted == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p1;
		OPL_STORAGE_EVENT_e eStorageEvt;

		// p1 = ulDeviceId
		HxLOG_Warning("[%s] [%d] Check disk startedil!! deviceId : %d \n",__FUNCTION__,__LINE__, ulDeviceId);

		eStorageEvt = eOPL_STORAGE_EVENT_CheckDiskStarted;
		gInstance->notifyDeviceStatus(ulDeviceId, eStorageEvt);
	}
	else if (eDxSTORAGE_NOTI_CheckUsbSpeed == eType)
	{
		HUINT32 	ulDeviceId = (HUINT32)p1;
		HUINT32 	ulReadMbps = (HUINT32)p2;
		HUINT32 	ulWriteMbps = (HUINT32)p3;

		OPL_STORAGE_EVENT_e eStorageEvt;

		// p1 = ulDeviceId
		HxLOG_Warning("[%s] [%d] Check usb speed done!! deviceId : %d, Read : %dMbps, Write : %dMbps\n",__FUNCTION__,__LINE__, ulDeviceId, ulReadMbps, ulWriteMbps);

		eStorageEvt = eOPL_STORAGE_EVENT_CheckUsbSpeedDone;

		//2 Add notifier
		gInstance->notifyUsbSpeedResult(ulDeviceId, ulReadMbps, ulWriteMbps);

	}
	else if (eDxSTORAGE_NOTI_PvrReady == eType)
	{
		// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
		HBOOL	bPvr = (HBOOL)p1;	//TRUE : PVR storage, FALSE : Not a PVR storage (disable or unpaired)
		HUINT32 	ulDeviceId = (HUINT32)p2;
		//OPL_STORAGE_EVENT_e eStorageEvt;

		HxLOG_Warning("[%s] [%d] PvrReady : %s, deviceId : %d \n",__FUNCTION__,__LINE__, (bPvr)?"T":"F", ulDeviceId);
		// Add event if it is necessary.
		/*
		if (bPvr == TRUE)
		{
			eStorageEvt = eOPL_STORAGE_EVENT_PairingDone;
		}
		else
		{
			eStorageEvt = eOPL_STORAGE_EVENT_UnPairingDone;
		}
		gInstance->notifyDeviceStatus(ulDeviceId, eStorageEvt);
		*/
	}
	else if (eDxSTORAGE_NOTI_Result == eType)
	{
		// API Call에 대한 결과를 리턴
		DxStorage_ResultType_e eResultType = (DxStorage_ResultType_e)p1;
		HUINT32 	ulDeviceId = (HUINT32)p2;
		HERROR	hErr = (HERROR)p3;				// API 결과 (ERR_OK:0, ERR_FAIL:1)
		OPL_STORAGE_EVENT_e eStorageEvt;

		HxLOG_Warning("[%s] [%d] eResultType : %d, ulDeviceId : %d, Result : %d\n",__FUNCTION__,__LINE__, eResultType, ulDeviceId, hErr);
		switch(eResultType)
		{
			case	eDxSTORAGE_RESULT_Format:
				if (hErr != ERR_OK)
				{
					eStorageEvt = eOPL_STORAGE_EVENT_FormatFail;
				}
				else
				{
				eStorageEvt = eOPL_STORAGE_EVENT_Max;
				}
				break;

			case	eDxSTORAGE_RESULT_Dst:
				// Add event if it is necessary.
				eStorageEvt = eOPL_STORAGE_EVENT_Max;
				break;

			case	eDxSTORAGE_RESULT_SetDefaultHdd:
				// Add event if it is necessary.
				eStorageEvt = eOPL_STORAGE_EVENT_Max;
				break;

			case	eDxSTORAGE_RESULT_SetName:
				if (hErr != ERR_OK)
				{
					eStorageEvt = eOPL_STORAGE_EVENT_SetNameFail;
				}
				else
				{
					eStorageEvt = eOPL_STORAGE_EVENT_SetNameDone;
				}
				break;

			case	eDxSTORAGE_RESULT_DisableHdd:
				// Add event if it is necessary.
				eStorageEvt = eOPL_STORAGE_EVENT_Max;
				break;

			case	eDxSTORAGE_RESULT_Pairing:
				if (hErr != ERR_OK)
				{
					eStorageEvt = eOPL_STORAGE_EVENT_PairingFail;
				}
				else
				{
					eStorageEvt = eOPL_STORAGE_EVENT_PairingDone;
				}
				break;

			case	eDxSTORAGE_RESULT_UnPairing:
				if (hErr != ERR_OK)
				{
					eStorageEvt = eOPL_STORAGE_EVENT_UnPairingFail;
				}
				else
				{
					eStorageEvt = eOPL_STORAGE_EVENT_UnPairingDone;
				}
				break;

			case	eDxSTORAGE_RESULT_SafeRemove:
				if (hErr != ERR_OK)
				{
					eStorageEvt = eOPL_STORAGE_EVENT_SafeRemoveFail;
				}
				else
				{
					eStorageEvt = eOPL_STORAGE_EVENT_Max;
				}
				break;

			default:
				eStorageEvt = eOPL_STORAGE_EVENT_Max;
				break;
		}

		if (eStorageEvt != eOPL_STORAGE_EVENT_Max)
		{
			gInstance->notifyDeviceStatus(ulDeviceId, eStorageEvt);
		}
	}
	else
	{
		HxLOG_Error("[%s] [%d] Unknown Type : %d \n",__FUNCTION__,__LINE__, eType);
	}
}

void OplStorageUtilPrivate::addStorageListener(OplStorageUtil::IOplStorageListener* aStorageListener)
{
	mListeners.push_back(aStorageListener);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::addStorageListener list count = %d\n", mListeners.size()));
}

void OplStorageUtilPrivate::removeStorageListener(OplStorageUtil::IOplStorageListener* aStorageListener)
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removeStorageListener list count = %d start\n", mListeners.size()));
	mListeners.remove(aStorageListener);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removeStorageListener list count = %d end\n", mListeners.size()));
}

void OplStorageUtilPrivate::addPhysicalStorageListener(OplPhysicalStorageData* aPhysicalStorageData)
{
	mPhysicalStorageDataForNotify.push_back(aPhysicalStorageData);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::addPhysicalStorageListener list count = %d\n", mPhysicalStorageDataForNotify.size()));
}

void OplStorageUtilPrivate::removePhysicalStorageListener(OplPhysicalStorageData* aPhysicalStorageData)
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removePhysicalStorageListener list count = %d start\n", mPhysicalStorageDataForNotify.size()));
	mPhysicalStorageDataForNotify.remove(aPhysicalStorageData);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removePhysicalStorageListener list count = %d end\n", mPhysicalStorageDataForNotify.size()));
}

void OplStorageUtilPrivate::addLogicalStorageListener(OplLogicalStorageData* aLogicalStorageData)
{
	mLogicalStorageDataForNotify.push_back(aLogicalStorageData);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::addLogicalStorageListener list count = %d\n", mLogicalStorageDataForNotify.size()));
}

void OplStorageUtilPrivate::removeLogicalStorageListener(OplLogicalStorageData* aLogicalStorageData)
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removeLogicalStorageListener list count = %d start\n", mLogicalStorageDataForNotify.size()));
	mLogicalStorageDataForNotify.remove(aLogicalStorageData);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::removeLogicalStorageListener list count = %d end\n", mLogicalStorageDataForNotify.size()));
}

void OplStorageUtilPrivate::updateDeviceProgress(DxStorage_Progress_t* pstProgress)
{
	STRG_DBG(HxLOG_Print("[OplStorageUtilPrivate::updateDeviceProgress] eFormat [eMS_DEV_STATUS_Formatting]\n"));
	if (pstProgress)
	{
		if (eDxSTORAGE_PROGRESS_Format == pstProgress->eType)
		{
			notifyFormatProgress(
					pstProgress->ulDeviceId,
					EFormatProgressOnGoing,
					pstProgress->ulCurrent,
					pstProgress->ulTotal);
			mFormatEventTriggerdId.insert(pstProgress->ulDeviceId);
		}
		else if (eDxSTORAGE_PROGRESS_Dst == pstProgress->eType)
		{
			notifyRecoveryProgress(
					pstProgress->ulDeviceId,
					ERecoveryProgressOnGoing,
					pstProgress->ulCurrent,
					pstProgress->ulTotal);
			mDstEventTriggerdId.insert(pstProgress->ulDeviceId);
		}
	}
}

const list<const char*> OplStorageUtilPrivate::getSupportedFileTypeList()
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::getSupportedFileTypeList()\n"));
	list<const char*> ret;
	for (HUINT32 i=0; i<sizeof(gSupportedFileSystems)/sizeof(const HCHAR*); i++) {
		ret.push_back(gSupportedFileSystems[i]);
	}
	return ret;
}

list<OplPhysicalStorageData*> OplStorageUtilPrivate::getPhysicalStoragesDataList()
{
	list<OplPhysicalStorageData*> ret;
	DxStorage_Info_t* deviceInfoList(NULL);
	HUINT32 deviceInfoListCount(0);
	if (APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_All, &deviceInfoList, &deviceInfoListCount) != ERR_OK) {
		STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::init FAILED to get APK_FILEMGR_STORAGE_INFO_GetList\n"));
	}
	for (HUINT32 i=0; i<deviceInfoListCount; i++) {
		ret.push_back(createPhysicalStorageData(&deviceInfoList[i]));
	}

	APK_FILEMGR_STORAGE_FreeList(deviceInfoList, deviceInfoListCount);

	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::getPhysicalStoragesDataList: %d\n", ret.size()));
	return ret;
}
list<OplLogicalStorageData*> OplStorageUtilPrivate::getLogicalStoragesDataList(const char* mountpath)
{
	list<OplLogicalStorageData*> ret;
	DxStorage_Info_t* deviceInfoList(NULL);
	HUINT32 deviceInfoListCount(0);
	HUINT64 reservedPvrSizeKb;

	if (APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_TypeAll, &deviceInfoList, &deviceInfoListCount) != ERR_OK) {
		STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::init FAILED to get APK_FILEMGR_STORAGE_INFO_GetList\n"));
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::getLogicalStoragesDataList [%s]\n", mountpath));
	for (HUINT32 i=0; i<deviceInfoListCount; i++) {
		const DxStorage_Info_t& devInfo(deviceInfoList[i]);
		if (HLIB_STD_StrCaseCmp(mountpath, (const HCHAR*)devInfo.szMountPath) == 0)
		{
			APK_FILEMGR_STORAGE_GetPvrReservedSize(devInfo.ulDeviceId, &reservedPvrSizeKb);
			STRG_DBG(HxLOG_Print("reservedPvrSizeKb : %lld\n", reservedPvrSizeKb));

			for (HINT32 j=0; j<devInfo.nPartitionNum; j++)
			{
				if(HLIB_DIR_IsMountedPath(devInfo.astPartitionInfos[j].szMountPath) == TRUE)
				{
					ret.push_back(createLogicalStorageData(&devInfo.astPartitionInfos[j], j, reservedPvrSizeKb));
				}
			}
		}
	}
	APK_FILEMGR_STORAGE_FreeList(deviceInfoList, deviceInfoListCount);
	return ret;
}

void OplStorageUtilPrivate::formatAll(HUINT32 aDeviceId, const char* aLabel)
{
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::formatAll [%d, %s]\n", aDeviceId, aLabel));
	if (aLabel == NULL) {
		APK_FILEMGR_STORAGE_Format(aDeviceId, "");
	}
	else {
		APK_FILEMGR_STORAGE_Format(aDeviceId, aLabel);
	}
}

void OplStorageUtilPrivate::detach(HUINT32 aDeviceId)
{
	APK_FILEMGR_STORAGE_SafeRemove(aDeviceId, false);
}

void OplStorageUtilPrivate::recovery(HUINT32 aDeviceId)
{
	APK_FILEMGR_STORAGE_StartDST(aDeviceId);
}

void OplStorageUtilPrivate::setStorageName(const HCHAR* uuid, const HCHAR* label)
{
	HCHAR *szUuid = NULL;
	HCHAR *szLabel = NULL;
	szUuid = HLIB_STD_StrDup(uuid);
	szLabel = HLIB_STD_StrDup(label);

	APK_FILEMGR_STORAGE_SetName(szUuid, szLabel);

	HLIB_STD_MemFree((void *)szUuid);
	HLIB_STD_MemFree((void *)szLabel);
}

void OplStorageUtilPrivate::checkUsbSpeed(const HCHAR* uuid)
{
	HCHAR *szUuid = NULL;
	szUuid = HLIB_STD_StrDup(uuid);

	APK_FILEMGR_STORAGE_CheckUsbSpeed(szUuid);

	HLIB_STD_MemFree((void *)szUuid);
}

void OplStorageUtilPrivate::setSwitchHddForPvr(const HCHAR* ucStorageId, const HCHAR* assign, const HCHAR* disableOther)
{
	HCHAR *pszStorageId = NULL;
	HCHAR *pszOtherStorageId = NULL;
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	ApkFileMgr_FTPS_INFO_t stFtpsInfo;
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	ApkFileMgr_SMBS_INFO_t stSambaInfo;
#endif

	pszStorageId = HLIB_STD_StrDup(ucStorageId);
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::setSwitchHddForPvr ucStorageId : [%s], assign : [%s]\n", ucStorageId, assign));

	if (strcmp(assign, "true") == 0){
		if (strcmp(disableOther, "false") != 0)
		{
			pszOtherStorageId = HLIB_STD_StrDup(disableOther);
			STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::setSwitchHddForPvr ohter ucStorageId : [%s]\n", pszOtherStorageId));
			APK_FILEMGR_STORAGE_DisablePVRHdd(pszOtherStorageId);
		}
	}

	if (strcmp(assign, "true") == 0)
	{
		APK_FILEMGR_STORAGE_PairingHdd(pszStorageId);
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
		if(bFtpServerStateBeforeUnpairing)
		{
			APK_FILEMGR_FTPSERVER_Start();
			bFtpServerStateBeforeUnpairing = FALSE;
		}
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
		if(bSambaServerStateBeforeUnpairing)
		{
			APK_FILEMGR_SAMBASERVER_Start();
			bSambaServerStateBeforeUnpairing = FALSE;
		}
#endif
	}
	else
	{
		APK_FILEMGR_STORAGE_UnPairingHdd(pszStorageId);
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
		HxSTD_memset(&stFtpsInfo, 0x00, sizeof(ApkFileMgr_FTPS_INFO_t));
		if (APK_FILEMGR_FTPSERVER_GetServerInfo(&stFtpsInfo) == 0)
			bFtpServerStateBeforeUnpairing = stFtpsInfo.bStarted;
		if (stFtpsInfo.bStarted)
			APK_FILEMGR_FTPSERVER_Stop();
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
		HxSTD_memset(&stSambaInfo, 0x00, sizeof(ApkFileMgr_SMBS_INFO_t));
		if (APK_FILEMGR_SAMBASERVER_GetServerInfo(&stSambaInfo) == 0)
			bSambaServerStateBeforeUnpairing = stSambaInfo.bStarted;
		if(stSambaInfo.bStarted)
			APK_FILEMGR_SAMBASERVER_Stop();
#endif
	}

	if (pszStorageId) {
		HLIB_STD_MemFree((void *)pszStorageId);
		pszStorageId = NULL;
	}

	if (pszOtherStorageId) {
		HLIB_STD_MemFree((void *)pszOtherStorageId);
		pszOtherStorageId = NULL;
	}

}

OplPhysicalStorageData* OplStorageUtilPrivate::createPhysicalStorageData(const DxStorage_Info_t *aDevInfo)
{
	HUINT64 ullUsedSize = 0;
	HUINT64 ullTotalSize = 0;
	HUINT64 ullReservedSize = 0;
	HUINT64 ullAvailSize = 0;
	HUINT64 ullAvailablePvrSize = 0;
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER) || defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	HBOOL	bOn = FALSE;
	HCHAR szPassword[OPL_MAX_PASSWORD_LEN];
	HERROR hErr = ERR_FAIL;
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
	ApkFileMgr_FTPS_INFO_t stFtpsInfo;
#endif
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	ApkFileMgr_SMBS_INFO_t stSmbsInfo;
#endif
#endif

	OplPhysicalStorageData* data = new OplPhysicalStorageData(this);

	data->id = aDevInfo->ulDeviceId;
	data->pairing = ConvertDevicePairing(aDevInfo->ulFlags);
	data->interface = ConvertDeviceInterface(aDevInfo->eDevType);
	data->interfaceVersion = ConvertDeviceInterfaceVer(aDevInfo);
	data->type = ConvertDeviceType(aDevInfo->eDevType);
	data->kind = ConvertDeviceKind(aDevInfo->eDevType);
	data->usage = ConvertDeviceUsage(aDevInfo->ulFlags);
	data->uuid = HLIB_STD_StrDup((const HCHAR*)aDevInfo->szUUID);
	data->label = HLIB_STD_StrDup((const HCHAR*)aDevInfo->szLabel);
	data->totalSize = data->availableSize = data->usedSize = data->reservedSize = 0LL;
	data->mountpath = HLIB_STD_StrDup((const HCHAR*)aDevInfo->szMountPath);

	APK_FILEMGR_STORAGE_GetSize(aDevInfo->ulDeviceId, &ullTotalSize, &ullAvailSize, &ullUsedSize, &ullReservedSize, &ullAvailablePvrSize);

	data->totalSize = static_cast<double>(ullTotalSize);
	data->availableSize = static_cast<double>(ullAvailSize);
	data->availablePvrSize = static_cast<double>(ullAvailablePvrSize);
	data->usedSize = static_cast<double>(ullUsedSize);
	data->reservedSize = static_cast<double>(ullReservedSize);
	data->ismount	 = false;
	if(aDevInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED)
	{
		data->ismount	 = true;
	}
	data->isNeedFormat = false;
	if (aDevInfo->ulFlags & eDxSTORAGE_FLAG_NEEDFORMAT)
	{
		data->isNeedFormat = true;
	}

	HxLOG_Print("[%s:%04d] id     : [%d]\n", __FUNCTION__, __LINE__, data->id);
	HxLOG_Print("[%s:%04d] eDevType     : [%d]\n", __FUNCTION__, __LINE__, aDevInfo->eDevType);
	HxLOG_Print("[%s:%04d] ulFlags     : [0x%x]\n", __FUNCTION__, __LINE__, aDevInfo->ulFlags);
	HxLOG_Print("[%s:%04d] Total     : [%f Kb]\n", __FUNCTION__, __LINE__, data->totalSize);
	HxLOG_Print("[%s:%04d] Available : [%f Kb]\n", __FUNCTION__, __LINE__, data->availableSize);
	HxLOG_Print("[%s:%04d] AvailablePvr  : [%f Kb]\n", __FUNCTION__, __LINE__, data->availablePvrSize);
	HxLOG_Print("[%s:%04d] Used      : [%f Kb]\n", __FUNCTION__, __LINE__, data->usedSize);
	HxLOG_Print("[%s:%04d] Reserved  : [%f Kb]\n", __FUNCTION__, __LINE__, data->reservedSize);

#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER) || defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	if (aDevInfo->eDevType >= eDxSTORAGE_DEVTYPE_INTHDD && aDevInfo->eDevType <= eDxSTORAGE_DEVTYPE_ESATAHDD)
	{
		HERROR	hErr = ERR_FAIL;
		//Start FTP Server after reboot
#if defined(CONFIG_SUPPORT_UMMA_FTP_SERVER)
		hErr = APK_FILEMGR_FTPSERVER_GetServerInfo( &stFtpsInfo );
		hErr |= APK_DB_GetInt(USERDB_FTPSERVER_ACTIVATION, (HINT32*)&bOn);
		if (hErr == ERR_OK)
		{
			// Check status after rebooting STB
			if ((FALSE == stFtpsInfo.bStarted) && (TRUE == bOn) && (bFtpServerStateBeforeUnpairing == FALSE))
			{
				APK_FILEMGR_FTPSERVER_Start();
				// Restore FTP Server password
				hErr = APK_DB_GetStr(USERDB_FTPSERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
				if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
				{
					APK_FILEMGR_FTPSERVER_SetPassword("admin","", (const HCHAR*)szPassword);
					APK_FILEMGR_FTPSERVER_SetPublicAccess("admin", FALSE);
				}
			}
		}
#endif

		//Start Samba Server after reboot
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
		bOn = FALSE;
		HxSTD_StrCpy((HCHAR*)szPassword,"");
		hErr = APK_FILEMGR_SAMBASERVER_GetServerInfo( &stSmbsInfo );
		hErr |= APK_DB_GetInt(USERDB_SAMBASERVER_ACTIVATION, (HINT32*)&bOn);
		if (hErr == ERR_OK)
		{
			// Check status after rebooting STB
			if ((FALSE == stSmbsInfo.bStarted) && (TRUE == bOn) && (bSambaServerStateBeforeUnpairing == FALSE))
			{
				APK_FILEMGR_SAMBASERVER_Start();
				// Restore FTP Server password
				hErr = APK_DB_GetStr(USERDB_SAMBASERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
				if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
				{
					APK_FILEMGR_SAMBASERVER_SetPassword("admin","", (const HCHAR*)szPassword);
					APK_FILEMGR_SAMBASERVER_SetPublicAccess("admin", FALSE);
				}
			}
		}

#endif
	}
#endif
	return data;
}

OplLogicalStorageData* OplStorageUtilPrivate::createLogicalStorageData(const DxStorage_PartitionInfo_t *aPartition, HUINT32 ulIdx, HUINT64 reservedPvrSizeKb)
{
	HERROR	hErr = ERR_FAIL;
	HUINT64 ullTotalSize = 0;
	HUINT64 ullUsedSize = 0;
	HUINT64 ullTSRUsedSize = 0;
	OplLogicalStorageData* data = new OplLogicalStorageData();

	data->id = ulIdx;
	data->available = aPartition->bAvailable;

	if(aPartition->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE)
		data->pvr = true;
	else if(aPartition->ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)
		data->pvr = true;
	else if(aPartition->ulFlags & eDxSTORAGE_FLAG_RECSTORAGEPVRDEVICE)
		data->pvr = true;

	data->reservedSize = data->usedSize	 = data->totalSize = data->availableSize = 0LL;

	data->path = HLIB_STD_StrDup((const HCHAR*)aPartition->szMountPath);
	hErr = HLIB_STORAGE_GetPartitionSize((HCHAR *)data->path, &ullTotalSize, &ullUsedSize, NULL);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!! Can not Get Partition Size : HLIB_STORAGE_GetPartitionSize()\n");
	}

	data->availableSize = static_cast<double>(ullTotalSize - ullUsedSize);
	data->usedSize		= static_cast<double>(ullUsedSize);
	data->reservedSize	= static_cast<double>(0);
	data->totalSize 	= static_cast<double>(ullTotalSize);

	if (data->pvr)
	{
		hErr = HLIB_STORAGE_GetPathSize((HCHAR *)aPartition->szTsrPath, &ullTSRUsedSize);
		if ((hErr == ERR_OK) && (ullTSRUsedSize > 0))
		{
			ullTSRUsedSize /= SIZE_1K;
			// TSR은 reserved 영역이므로 도출된 used size에서 빼준다.
			if(ullUsedSize >= ullTSRUsedSize)
				ullUsedSize -=	ullTSRUsedSize;
		}
		data->usedSize		= static_cast<double>(ullUsedSize);
		if((ullTotalSize - ullUsedSize) > reservedPvrSizeKb)
			data->availableSize = static_cast<double>((ullTotalSize - ullUsedSize) - reservedPvrSizeKb);
		else
			data->availableSize = static_cast<double>(0);
		data->reservedSize	= static_cast<double>(reservedPvrSizeKb);
	}

	data->formatType = HLIB_STD_StrDup(APK_FILEMGR_STORAGE_GetFsString(aPartition->eFsType));
	data->label = HLIB_STD_StrDup((const HCHAR*)aPartition->szVolumeName);

	return data;
}

void OplStorageUtilPrivate::notifyDeviceAttach(OplPhysicalStorageData* aOplPhysicalStorageData)
{
	bool notified(false);

	list<OplStorageUtil::IOplStorageListener*>::iterator i(mListeners.begin());
	while (i != mListeners.end()) {
		(*i++)->physicalStorageAttached(aOplPhysicalStorageData);
		notified = true;
	}
	if (!notified) {
		delete aOplPhysicalStorageData;
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyDeviceAttach %d notified=%d\n", (int)aOplPhysicalStorageData->id, notified));
}

void OplStorageUtilPrivate::notifyDeviceDetach(OplPhysicalStorageData* aOplPhysicalStorageData)
{
	bool notified(false);

	list<OplStorageUtil::IOplStorageListener*>::iterator i(mListeners.begin());
	while (i != mListeners.end()) {
		(*i++)->physicalStorageDetached(aOplPhysicalStorageData);
		notified = true;
	}
	if (!notified) {
		delete aOplPhysicalStorageData;
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyDeviceDetach %d notified=%d\n", (int)aOplPhysicalStorageData->id, notified));
}

void OplStorageUtilPrivate::notifyFormatProgress(
		HUINT32 aDeviceId,
		const TFormatProgress& aStatus,
		int aProgress,
		int aProgressMax)
{
	bool notified(false);
	list<OplPhysicalStorageData*>::iterator i(mPhysicalStorageDataForNotify.begin());
	while (i != mPhysicalStorageDataForNotify.end()) {
		OplPhysicalStorageData* data(*i++);
		if (data->id == aDeviceId) {
			list<IOplPhysicalStorageListener*>::iterator j(data->mListeners.begin());
			while (j != data->mListeners.end()) {
				(*j++)->formatProgress(aStatus, aProgress, aProgressMax, ""); // TODO: Proper error message if required
				notified = true;
			}
		}
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyFormatProgress %d, %d, %d, %d: notified=%d\n", aDeviceId, aStatus, aProgress, aProgressMax, notified));
}

void OplStorageUtilPrivate::notifyDetachProgress(
		HUINT32 aDeviceId,
		const TDetachProgress& aStatus)
{
	bool notified(false);
	list<OplPhysicalStorageData*>::iterator i(mPhysicalStorageDataForNotify.begin());
	while (i != mPhysicalStorageDataForNotify.end()) {
		OplPhysicalStorageData* data(*i++);
		if (data->id == aDeviceId) {
			list<IOplPhysicalStorageListener*>::iterator j(data->mListeners.begin());
			while (j != data->mListeners.end()) {
				(*j++)->detachProgress(aStatus, ""); // TODO: Proper error message if required
				notified = true;
			}
		}
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyDeviceDetach %d: notified=%d\n", aDeviceId, notified));
}

void OplStorageUtilPrivate::notifyRecoveryProgress(
		HUINT32 aDeviceId,
		const TRecoveryProgress& aStatus,
		int aProgress,
		int aProgressMax)
{
	bool notified(false);
	list<OplPhysicalStorageData*>::iterator i(mPhysicalStorageDataForNotify.begin());
	while (i != mPhysicalStorageDataForNotify.end()) {
		OplPhysicalStorageData* data(*i++);
		if (data->id == aDeviceId) {
			list<IOplPhysicalStorageListener*>::iterator j(data->mListeners.begin());
			while (j != data->mListeners.end()) {
				(*j++)->recoveryProgress(aStatus, aProgress, aProgressMax, ""); // TODO: Proper error message if required
				notified = true;
			}
		}
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyRecoveryProgress %d: notified=%d\n", aDeviceId, notified));
}

void OplStorageUtilPrivate::notifyUsbSpeedResult(
		HUINT32 aDeviceId,
		int aReadMbps,
		int aWriteMbps)
{
	bool notified(false);
	list<OplPhysicalStorageData*>::iterator i(mPhysicalStorageDataForNotify.begin());
	while (i != mPhysicalStorageDataForNotify.end()) {
		OplPhysicalStorageData* data(*i++);
		if (data->id == aDeviceId) {
			list<IOplPhysicalStorageListener*>::iterator j(data->mListeners.begin());
			while (j != data->mListeners.end()) {
				(*j++)->usbSpeedResult(aReadMbps, aWriteMbps); // TODO: Proper error message if required
				notified = true;
			}
		}
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyUsbSpeedResult %d: notified=%d\n", aDeviceId, notified));
}

void OplStorageUtilPrivate::notifyDeviceStatus(HUINT32 aDeviceId, HUINT32 aStatus)
{
	bool notified(false);

	list<OplStorageUtil::IOplStorageListener*>::iterator i(mListeners.begin());
	while (i != mListeners.end()) {
		(*i++)->physicalStorageStatus(aDeviceId,aStatus);
		notified = true;
	}
	STRG_DBG(HxLOG_Print("OplStorageUtilPrivate::notifyDeviceStatus id:%d status:%d : noti :%d \n",aDeviceId, aStatus,notified));
}


