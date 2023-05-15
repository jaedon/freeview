/**************************************************************
 *	@file		apk_network_netconfig.c
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

#if defined(CONFIG_SUPPORT_NIMA)

#define	APKD_NETWORK_AP_ESSID		48
#define	APKD_NETWORK_AP_KEY			64
#define	APKD_NETWORK_WPS_PIN		64


#define FuncEnter HxLOG_Debug("   (+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__)
#define FuncLeave HxLOG_Debug("   (-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__)

HERROR		APK_NETWORK_WIFI_StartScan(HINT32 nIndex)
{
	FuncEnter;

	NAPI_Wifi_ScanStart(nIndex);


#if defined(CONFIG_OP_TDC_NETWORK_LED)
	APK_NETWORK_LED_Init();

	APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_GREEN, TRUE);
#endif

	FuncLeave;

	return ERR_OK;
}

HERROR		APK_NETWORK_WIFI_ConnectAP( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey )
{

	NAPIWifiAPInfo_t ap_info;

	FuncEnter;

	if( !pstApInfo )
		return ERR_FAIL;

#if defined(CONFIG_OP_TDC_NETWORK_LED)
	APK_NETWORK_LED_Init();

	APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_YELLOW, TRUE);
#endif	

	HxSTD_MemCopy(ap_info.essid, pstApInfo->szESSID, APKD_NETWORK_AP_ESSID);
	HxSTD_MemCopy(ap_info.apkey, szKey, APKD_NETWORK_AP_KEY);
	HxSTD_MemCopy(ap_info.macaddr, pstApInfo->szMacAddr, APKD_NETWORK_MAX_MACADDR);
	ap_info.wave_strength = pstApInfo->nWaveStrength;
	ap_info.connect_speed = pstApInfo->nConnectSpeed;
	ap_info.op_mode = pstApInfo->nOpMode;
	ap_info.secure_type = pstApInfo->nSecureType;
	ap_info.authen_type = pstApInfo->nAuthenType;
	ap_info.encrypt_type = pstApInfo->nEncryptType;
	ap_info.bIsConnected = pstApInfo->bIsConnected;

	NAPI_Wifi_Connect(nIndex, &ap_info);

	FuncLeave;
	return ERR_OK;
}

HERROR		APK_NETWORK_WIFI_ConnectAPByWPS( HINT32 nIndex, APKE_NETAP_WPS_Type_e eWPSType, HCHAR *szPinCode )
{
	/* empty */
	FuncEnter;

#if defined(CONFIG_OP_TDC_NETWORK_LED)
	APK_NETWORK_LED_Init();

	APK_OUTPUT_PANEL_SetRgbLed(eDxLED_ID_RGB_NET, eDxRGBLED_COLOR_YELLOW, TRUE);
#endif
	NAPI_Wifi_ConnectWPS(nIndex, eWPSType, szPinCode);

	FuncLeave;

	return ERR_OK;
}

HERROR		APK_NETWORK_WIFI_DisconnectAP( HINT32 nIndex )
{
	FuncEnter;

	NAPI_Wifi_Disconnect(nIndex);

	FuncLeave;
	return ERR_OK;
}

HERROR		APK_NETWORK_WIFI_GetPincodeByWPS( HINT32 nIndex, HCHAR *szPincode )
{
	HCHAR tempPincode[APKD_WPS_PIN_LEN_MAX] = {0,};
	/* empty */
	FuncEnter;

	NAPI_Wifi_GetPincodeByWPS(nIndex, tempPincode);

	if(tempPincode != NULL)
	{
		HxSTD_MemCopy(szPincode, tempPincode, APKD_WPS_PIN_LEN_MAX);
	}

	FuncLeave;

	return ERR_OK;
}

HERROR		APK_NETWORK_WIFI_GetConnectedInfo( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *pszKey, HINT32 nBufSize )
{
	NAPIWifiAPInfo_t ap_info;
	NAPI_Wifi_GetConnectedAPInfo(nIndex, &ap_info);

	FuncEnter;

	if( !pstApInfo )
		return ERR_FAIL;

	HxSTD_MemCopy(pstApInfo->szESSID, ap_info.essid, NAPI_AP_ESSID);
	HxSTD_MemCopy(pstApInfo->szMacAddr, ap_info.macaddr, NAPI_MAX_MACADDR);
	HxSTD_StrNCpy(pszKey, ap_info.apkey, nBufSize -1 );
	pstApInfo->nOpMode = ap_info.op_mode;
	pstApInfo->nSecureType = ap_info.secure_type;
	pstApInfo->nAuthenType = ap_info.authen_type;
	pstApInfo->nEncryptType = ap_info.encrypt_type;
	pstApInfo->nWaveStrength = ap_info.wave_strength;
	pstApInfo->bIsConnected = ap_info.bIsConnected;

	FuncLeave;
	return ERR_OK;
}

/**
* This synchonous function is used to configure WoWLAN feature
* @param[in] lIndex: index of wifi network interface
* @param[in] etMode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR	APK_NETWORK_WIFI_SetWakeupOnWlan( HINT32 lIndex, APKS_NETWoWlan_Mode_t etMode)
{
	return NAPI_Wifi_SetWakeupOnWLAN(lIndex, (eNapiWifiWoWlan)etMode);
}

#else


HERROR		APK_NETWORK_WIFI_StartScan(HINT32 nIndex)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_WIFI_ConnectAP( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_WIFI_ConnectAPByWPS( HINT32 nIndex, APKE_NETAP_WPS_Type_e eWPSType, HCHAR *szPinCode )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_WIFI_DisconnectAP( HINT32 nIndex )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_WIFI_GetConnectedInfo( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *pszKey, HINT32 nBufSize )
{
	return ERR_FAIL;
}

HERROR	APK_NETWORK_WIFI_SetWakeupOnWlan( HINT32 lIndex, APKS_NETWoWlan_Mode_t etMode)
{
	return ERR_FAIL;
}

#endif
