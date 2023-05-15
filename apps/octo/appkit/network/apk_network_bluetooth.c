/**************************************************************
 *	@file		apk_network_bluetooth.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-07-15
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/
#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>
#include 	<hapi.h>

#if defined(CONFIG_SUPPORT_NIMA)

#define FuncEnter HxLOG_Debug("   (+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__)
#define FuncLeave HxLOG_Debug("   (-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__)


HERROR APK_NETWORK_BT_SetEnable(HBOOL bEnable)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if (!bEnable)
	{
		APK_OUTPUT_AUDIO_SetMasterMute(bEnable);
		HAPI_BroadcastSignal("signal:onBtStateChanged", 1, "ii", eNETWORK_STATUS_BT_AUDIO_MUTE, (HUINT32)FALSE);
	}

	eRet = NAPI_BT_SetEnable(bEnable);

	FuncLeave;
	return eRet;
}

HERROR APK_NETWORK_BT_GetEnable(HBOOL *bEnable)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	eRet = NAPI_BT_GetEnable(bEnable);
	HxLOG_Debug("   (#)[%s:%d] enable : %d \n",__FUNCTION__,__LINE__, *bEnable);

	FuncLeave;
	return eRet;
}

HERROR APK_NETWORK_BT_Scan(void)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	eRet = NAPI_BT_Scan(eBLUETOOTH_PROFILE_All);

	FuncLeave;
	return eRet;
}

HERROR APK_NETWORK_BT_Connect(HINT32 nIndex)
{
	FuncEnter;
	/* not support */
	FuncLeave;
	return ERR_OK;
}

HERROR APK_NETWORK_BT_ConnectByBdAddr(const HCHAR *szBdAddr)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if( !szBdAddr ) return ERR_FAIL;

	HxLOG_Debug("	(#)[%s:%d] bdaddr : %s \n",__FUNCTION__,__LINE__, szBdAddr);
	eRet = NAPI_BT_Connect(szBdAddr);

	FuncLeave;
	return eRet;
}


HERROR APK_NETWORK_BT_Disconnect(HINT32 nIndex)
{
	FuncEnter;
	/* not support */
	FuncLeave;
	return ERR_OK;
}

HERROR APK_NETWORK_BT_DisconnectByBdAddr(const HCHAR *szBdAddr)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if( !szBdAddr ) return ERR_FAIL;

	HxLOG_Debug("	(#)[%s:%d] bdaddr : %s \n",__FUNCTION__,__LINE__, szBdAddr);
	eRet = NAPI_BT_Disconnect(szBdAddr);

	FuncLeave;
	return eRet;
}
HERROR APK_NETWORK_BT_Remove(HINT32 nIndex)
{
	FuncEnter;
	/* not support */
	FuncLeave;
	return ERR_OK;
}

HERROR APK_NETWORK_BT_RemoveByBdAddr(const HCHAR *szBdAddr)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if( !szBdAddr ) return ERR_FAIL;

	HxLOG_Debug("	(#)[%s:%d] bdaddr : %s \n",__FUNCTION__,__LINE__, szBdAddr);
	eRet = NAPI_BT_Remove(szBdAddr);

	FuncLeave;
	return eRet;
}

HERROR APK_NETWORK_BT_SendPinCode(HUINT32 nIndex, HCHAR *pucPinCode)
{
	FuncEnter;
	/* not support */
	FuncLeave;
	return ERR_OK;
}

HERROR APK_NETWORK_BT_SendPinCodeByBdAddr(const HCHAR *szBdAddr, HCHAR *pucPinCode)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if( !szBdAddr ) return ERR_FAIL;
	if( !pucPinCode ) return ERR_FAIL;

	HxLOG_Debug("	(#)[%s:%d] bdaddr : %s , pincode : %s\n",__FUNCTION__,__LINE__, szBdAddr, pucPinCode);
	eRet = NAPI_BT_SendPinCode(szBdAddr, pucPinCode);

	FuncLeave;
	return eRet;
}

HUINT32 APK_NETWORK_BT_GetDeviceNum(void)
{
	HUINT32 dev_num = 0;
	FuncEnter;

	dev_num = NAPI_BT_GetDeviceNum();

	FuncLeave;
	return dev_num;
}

HERROR APK_NETWORK_BT_GetDeviceInfo(HUINT32 ulDevId, APKS_BT_DEVICEINFO_t *pstDevInfoList)
{
	HERROR eRet = ERR_FAIL;
	NAPIBTDevInfo_t dev_info;
	FuncEnter;

	if( !pstDevInfoList ) return ERR_FAIL;

	NAPI_BT_GetDeviceInfo(ulDevId, &dev_info);
	//pstDevInfoList->ulDevId = dev_info.dev_id;
	HxSTD_MemCopy(pstDevInfoList->szDevName, dev_info.name, APKD_BLUETOOTH_DEV_NAME_LEN);
	pstDevInfoList->eStatus = dev_info.status;
	HxSTD_MemCopy(pstDevInfoList->dev_class, dev_info.dev_class, APKD_BLUETOOTH_DEV_CLASS);
	pstDevInfoList->eSupportedProfile = dev_info.support_profile;
	HxSTD_MemCopy(pstDevInfoList->aucBdAddr, dev_info.bdaddr, APKD_BLUETOOTH_DEV_MACADDR_LEN);

	FuncLeave;
	return eRet;
}

HERROR APK_NETWORK_BT_GetMacAddrInfo( HCHAR *szMacAddr)
{
	HERROR eRet = ERR_FAIL;
	FuncEnter;

	if( !szMacAddr ) return ERR_FAIL;

	eRet = NAPI_BT_GetHostMacAddrInfo(szMacAddr);
	HxLOG_Debug("	(#)[%s:%d] bdaddr : %s \n",__FUNCTION__,__LINE__, szMacAddr);

	FuncLeave;
	return eRet;
}

HUINT32 APK_NETWORK_BT_GetMacAddrInfoSize(void)
{
	HUINT32 mac_size;
	FuncEnter;

	mac_size = NAPI_BT_GetHostMacAddrInfoSize();

	FuncLeave;
	return mac_size;
}


HUINT32 APK_NETWORK_BT_GetDevInfoList(HUINT32 ulListSize, APKS_BT_DEVICEINFO_t *pstDevInfoList)
{
	NAPIBTDevInfo_t *dev_info = NULL;
	HINT32 i = 0, dev_count = 0;
	FuncEnter;

	if( !pstDevInfoList ) return ERR_FAIL;

	HxSTD_MemSet(pstDevInfoList, 0x00, ulListSize);

	dev_count = ulListSize / sizeof(APKS_BT_DEVICEINFO_t);
	HxLOG_Debug("   (#)[%s:%d] Bluetooth Device Count : %d \n",__FUNCTION__,__LINE__, dev_count);

	dev_info = HLIB_STD_MemAlloc(sizeof(NAPIBTDevInfo_t)*dev_count);
	if( !dev_info ) return ERR_FAIL;

	dev_count = NAPI_BT_GetDevInfoList(dev_count, dev_info);

	for( i=0; i<dev_count; i++ )
	{
		pstDevInfoList[i].ulDevId = i;//dev_info[i].dev_id;
		HxSTD_MemCopy(pstDevInfoList[i].szDevName, dev_info[i].name, APKD_BLUETOOTH_DEV_NAME_LEN);
		pstDevInfoList[i].eStatus = dev_info[i].status;
		HxSTD_MemCopy(pstDevInfoList[i].dev_class, dev_info[i].dev_class, APKD_BLUETOOTH_DEV_CLASS);
		pstDevInfoList[i].eSupportedProfile = dev_info[i].support_profile;
		HxSTD_MemCopy(pstDevInfoList[i].aucBdAddr, dev_info[i].bdaddr, APKD_BLUETOOTH_DEV_MACADDR_LEN);
	}

	if( dev_info ) HLIB_STD_MemFree(dev_info);

	FuncLeave;
	return dev_count;
}


#else



HERROR APK_NETWORK_BT_SetEnable(HBOOL bEnable)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_GetEnable(HBOOL *bEnable)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_Scan(void)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_Connect(HINT32 nIndex)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_ConnectByBdAddr(const HCHAR *szBdAddr)
{
	return ERR_FAIL;
}


HERROR APK_NETWORK_BT_Disconnect(HINT32 nIndex)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_DisconnectByBdAddr(const HCHAR *szBdAddr)
{
	return ERR_FAIL;
}
HERROR APK_NETWORK_BT_Remove(HINT32 nIndex)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_RemoveByBdAddr(const HCHAR *szBdAddr)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_SendPinCode(HUINT32 nIndex, HCHAR *pucPinCode)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_SendPinCodeByBdAddr(const HCHAR *szBdAddr, HCHAR *pucPinCode)
{
	return ERR_FAIL;
}

HUINT32 APK_NETWORK_BT_GetDeviceNum(void)
{
	return 0;
}

HERROR APK_NETWORK_BT_GetDeviceInfo(HUINT32 ulDevId, APKS_BT_DEVICEINFO_t *pstDevInfoList)
{
	return ERR_FAIL;
}

HERROR APK_NETWORK_BT_GetMacAddrInfo( HCHAR *szMacAddr)
{
	return ERR_FAIL;
}

HUINT32 APK_NETWORK_BT_GetMacAddrInfoSize(void)
{
	return 0;
}

HUINT32 APK_NETWORK_BT_GetDevInfoList(HUINT32 ulListSize, APKS_BT_DEVICEINFO_t *pstDevInfoList)
{
	return 0;
}


#endif
