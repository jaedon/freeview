#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_storage.h>

char const * const cApkFilemgrStorage::BUSNAME_APK_FILEMGR_STORAGE = "Octopus.Appkit.Filemgr.Storage";
char const * const cApkFilemgrStorage::BUSPATH_APK_FILEMGR_STORAGE = "/Octopus/Appkit/Filemgr/Storage";

cApkFilemgrStorage * cApkFilemgrStorage::pMainThis	=	NULL;

void cApkFilemgrStorage::callback_event (DxStorage_NotiType_e eType, HINT32	nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t type=0, param1=0, param2=0, param3=0;

	type = (uint32_t)eType;
	param1 = (uint32_t)nParam1;
	param2 = (uint32_t)nParam2;
	param3 = (uint32_t)nParam3;

	pMainThis->event (type, "",param1, param2, param3);
}

cApkFilemgrStorage::cApkFilemgrStorage(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_STORAGE)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit filemgr storage */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_STORAGE_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_STORAGE_Init Error : %d", hErr);
		}			

		APK_FILEMGR_STORAGE_RegisterNotifier (cApkFilemgrStorage::callback_event);
    }
}

void cApkFilemgrStorage::Format(const uint32_t& deviceId, const std::string& volume)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_Format ((HUINT32)deviceId, (const HCHAR *)&volume[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_Format Error : %d", hErr);
	}			
	
	return ;
}

void cApkFilemgrStorage::StartDST(const uint32_t& deviceId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_StartDST ((HUINT32)deviceId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_StartDST Error : %d", hErr);
	}			

	return ;
}

void cApkFilemgrStorage::SetDefaultHdd(const std::string& storageId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_SetDefaultHdd ((const HCHAR *)&storageId[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_SetDefaultHdd Error : %d", hErr);
	}			

	return ;
}

void cApkFilemgrStorage::DisablePVRHdd(const std::string& storageId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_DisablePVRHdd ((const HCHAR *)&storageId[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_DisablePVRHdd Error : %d", hErr);
	}			

	return ;
}

void cApkFilemgrStorage::PairingHdd(const std::string& storageId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_PairingHdd ((const HCHAR *)&storageId[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_PairingHdd Error : %d", hErr);
	}			

}

void cApkFilemgrStorage::UnPairingHdd(const std::string& storageId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_UnPairingHdd ((const HCHAR *)&storageId[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_UnPairingHdd Error : %d", hErr);
	}			

	return ;
}

void cApkFilemgrStorage::SetName(const std::string& storageId, const std::string& name)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_SetName ((const HCHAR *)&storageId[0], (const HCHAR *)&name[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_SetName Error : %d", hErr);
	}			

	return ;
}

void cApkFilemgrStorage::SafeRemove(const uint32_t& deviceId, const bool& forced)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_SafeRemove ((HUINT32)deviceId, (HBOOL)forced);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_SafeRemove Error : %d", hErr);
	}			

	return ;
}

std::vector< ::DBus::Struct< uint32_t > > cApkFilemgrStorage::GetList(const uint32_t& filter)
{
	std::vector< ::DBus::Struct< uint32_t > > reval;

	HERROR hErr = ERR_FAIL;
	DxStorage_Info_t* pstDeviceInfoList = NULL;
	HUINT32 nDevNum = 0;
	HINT32 i;
	
	hErr = APK_FILEMGR_STORAGE_GetList ((HUINT32)filter, &pstDeviceInfoList, &nDevNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_GetList Error : %d", hErr);
	}			

	{
		for (i=0 ; i<nDevNum ; i++)
		{
			::DBus::Struct< uint32_t > item;
			item._1 = pstDeviceInfoList[i].ulDeviceId;

			reval.push_back (item);
		}
	}

	hErr = APK_FILEMGR_STORAGE_FreeList (pstDeviceInfoList, nDevNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_FreeList Error : %d", hErr);
	}			

	return reval;
}

::DBus::Struct< uint64_t, uint64_t, uint64_t, uint64_t, uint64_t > cApkFilemgrStorage::GetSize(const uint32_t& deviceId)
{
	::DBus::Struct< uint64_t, uint64_t, uint64_t, uint64_t, uint64_t > reval;

	HERROR hErr = ERR_FAIL;

	HUINT64 ullTotalKbSize;
	HUINT64 ullAvailableKbSize;
	HUINT64 ullUsedKbSize;
	HUINT64 ullReservedKbSize;
	HUINT64 ullAvailablePvrKbSize;
		
	hErr = APK_FILEMGR_STORAGE_GetSize ((HUINT32)deviceId, &ullTotalKbSize, &ullAvailableKbSize, &ullUsedKbSize, &ullReservedKbSize, &ullAvailablePvrKbSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_GetSize Error : %d", hErr);
	}	

	{
		reval._1	=	(uint64_t)ullTotalKbSize;
		reval._2	=	(uint64_t)ullAvailableKbSize;
		reval._3	=	(uint64_t)ullUsedKbSize;
		reval._4	=	(uint64_t)ullReservedKbSize;
		reval._5	=	(uint64_t)ullAvailablePvrKbSize;
	}

	return reval;	
}

uint64_t cApkFilemgrStorage::GetPvrReservedSize(const uint32_t& deviceId)
{
	HERROR hErr = ERR_FAIL;	
	HUINT64 ullReservedKbSize;
	
	hErr = APK_FILEMGR_STORAGE_GetPvrReservedSize ((HUINT32)deviceId, &ullReservedKbSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_GetPvrReservedSize Error : %d", hErr);
	}			

	return (uint64_t)ullReservedKbSize;
}

void cApkFilemgrStorage::CheckUsbSpeed(const std::string& storageId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_STORAGE_CheckUsbSpeed ((const HCHAR *)&storageId[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_STORAGE_CheckUsbSpeed Error : %d", hErr);
	}			
	
	return ;
}

std::string cApkFilemgrStorage::GetDevTypeString(const uint32_t& deviceType)
{
	const HCHAR* pstrDevType = APK_FILEMGR_STORAGE_GetDevTypeString ((const DxStorage_DevType_e)deviceType);

	return std::string(pstrDevType);
}

std::string cApkFilemgrStorage::GetFsString(const uint32_t& fsType)
{
	const HCHAR* pstrFsType = APK_FILEMGR_STORAGE_GetFsString ((const DxStorage_FsType_e)fsType);
	
	return std::string(pstrFsType);
}
