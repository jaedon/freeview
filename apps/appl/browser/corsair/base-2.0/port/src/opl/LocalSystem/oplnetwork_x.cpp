/**************************************************************************************/
/**
 * @file oplnetwork.cpp
 *
 * OplNetworkInterface
 *
 * @author  Mun-ho Park(mhpark3@humaxdigital.com)
 * @date    2011/10/24
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "oplnetwork.h"
#include "oplnetservice.h"

#include "oapi.h"
#include "hapi.h"
#include <hlib.h>
#include "apk.h"

static OPL_NETWORK_EventCallback *s_pfn_netmanager_callback = NULL;
static OPL_NETWORK_EventCallback *s_pfn_netwifi_callback = NULL;
static OPL_NETWORK_EventCallback *s_pfn_pppoe_callback = NULL;
static bool s_internet_stataus = false;


void _OPL_NETWORK_MANAGER_Notifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
    OplNetLinkStatus_e eStatus;
    HINT32	nDevIndex = ulParam1 ;

    if ( NULL == s_pfn_netmanager_callback )
    	return ;

    OPL_Network_SetInternetConnection(eEvent);

    switch ( eEvent )
    {
    	case eNETWORK_STATUS_CON_SUCCESS :
    		eStatus = eNET_CONNECT_TRY_SUCCESS;
    		break;
    	case eNETWORK_STATUS_CON_FAIL :
    		eStatus = eNET_CONNECT_TRY_FAIL;
    		break;
    	case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
    		eStatus = eNET_CABLE_LINK_CONNECTD;
    		break;
    	case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
    		eStatus = eNET_CABLE_LINK_DICONNECTED;
    		break;
    	case eNETWORK_STATUS_WIFI_USB_INSERTED :
    		eStatus = eNET_WIFI_USB_INSERTED;
    		break;
    	case eNETWORK_STATUS_WIFI_USB_EXTRACTED :
    		eStatus = eNET_WIFI_USB_EXTRACTED;
    		break;
    	default :
    		return ;
    }

    s_pfn_netmanager_callback( eStatus, 0, (HUINT32)nDevIndex );

}

void _OPL_NETWORK_WIFI_Notifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	OplNetworkAccessPoint 	*pstApInfo = NULL;
	OplNetWifiStatus_e eStatus;
	HINT32	nDevIndex = ulParam1;

	if ( NULL == s_pfn_netwifi_callback )
		return ;

	switch (eEvent)
	{
		case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS :
			eStatus = eNET_WIFI_CONNECT_SUCCESS;
			break;
		case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL :
			eStatus = eNET_WIFI_CONNECT_FAIL;
			break;

		case eNETWORK_STATUS_WIFI_SCANNING_END :
			{
				HINT32 nApCount = ulParam3;
				HINT32 i = 0;
				APKS_NETAP_Info_t *apList = (APKS_NETAP_Info_t *)ulParam2;
				for( i = 0; i < nApCount; i++ )
				{
					pstApInfo = (OplNetworkAccessPoint *)HLIB_STD_MemAlloc( sizeof(OplNetworkAccessPoint) );
					HxSTD_MemCopy( pstApInfo, (void *)&apList[i], sizeof(OplNetworkAccessPoint) );
					s_pfn_netwifi_callback( eNET_WIFI_SCANNING_OUTPUT, (OplNetApHandle_t)pstApInfo, (HUINT32)nDevIndex );

				}
			}
			eStatus = eNET_WIFI_SCANNING_END;
			break;
		case eNETWORK_STATUS_WIFI_SCANNING_FAIL :
			eStatus = eNET_WIFI_SCANNING_FAIL;
			break;
		case eNETWORK_STATUS_WIFI_KEY_INVALID :
			eStatus = eNET_WIFI_KEY_INVALID;
			break;
		case eNETWORK_STATUS_WIFI_WPS_SUCCESS :
			eStatus = eNET_WIFI_WPS_SUCCESS;
			break;
		case eNETWORK_STATUS_WIFI_WPS_FAIL :
			eStatus = eNET_WIFI_WPS_FAIL;
			break;
		default :
			return ;
	}


	s_pfn_netwifi_callback( eStatus, (OplNetApHandle_t)pstApInfo, (HUINT32)nDevIndex );

}


void _OPL_NETWORK_PPPoE_Notifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	OplNetPPPoEStatus_e eStatus;
	HINT32	nDevIndex = ulParam1;

	if ( NULL == s_pfn_pppoe_callback )
		return;

	switch (eEvent)
	{
		case eNETWORK_STATUS_PPPOE_CONNECTED :
			eStatus = eNET_PPPOE_CONNECTED;
			break;
		case eNETWORK_STATUS_PPPOE_CONNECT_FAILED :
			eStatus = eNET_PPPOE_CONNECT_FAILED;
			break;
		case eNETWORK_STATUS_PPPOE_DISCONNECTED :
			eStatus = eNET_PPPOE_DISCONNECTED;
			break;
		case eNETWORK_STATUS_PPPOE_IN_PROGRESS :
			eStatus = eNET_PPPOE_IN_PROGRESS;
			break;
		default :
			return;
	}

	s_pfn_pppoe_callback( eStatus, 0, (HUINT32)nDevIndex );

}

BPLStatus OPL_Network_GetDefaultDevIndex( int *pnIndex )
{
	APK_NETWORK_CONF_GetDefaultDev( pnIndex );
	return  BPL_STATUS_OK;
}

BPLStatus OPL_Network_GetDhcpEnabled( int *pnEnabled )
{
    int nIndex;
    HERROR err = ERR_OK;
    APKS_NETIFADDR_t stNetAddr;

	err = APK_NETWORK_CONF_GetDefaultDev( &nIndex );
	if (err != ERR_OK)
	{
		HxLOG_Error("APK_NETWORK_CONF_GetDefaultDev failed(Err:%d)\n", err);
		return BPL_STATUS_ERROR;
	}

	err = APK_NETWORK_CONF_GetAddrInfo( nIndex, &stNetAddr);
	if (err != ERR_OK)
	{
		HxLOG_Error("APK_NETWORK_CONF_GetAddrInfo(index:%d,..) failed(Err:%d)\n", nIndex, err);
		return BPL_STATUS_ERROR;
	}

    if ( TRUE == stNetAddr.bDhcpOn )
        *pnEnabled = 1;
    else
        *pnEnabled = 0;

	return  BPL_STATUS_OK;
}

BPLStatus OPL_Network_GetLinkStatus( int *pnStatus )
{
	int nIndex = 0;
	APKS_NETDEVINFO_t stDevInfo;
	HERROR eRet = ERR_OK;

	APK_NETWORK_CONF_GetDefaultDev( &nIndex );
	eRet = APK_NETWORK_CONF_GetDevInfo( nIndex, &stDevInfo );
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", nIndex);
	}

	if( FALSE == stDevInfo.bConnected )
	{
		*pnStatus = 0;
	}
	else
	{
		if ( eINTERFACE_TYPE_LAN == stDevInfo.eType )
			*pnStatus = 1;
		else
			*pnStatus = 2;
	}

	if( TRUE == stDevInfo.bNetActive )
		*pnStatus = 3;

	return  BPL_STATUS_OK;
}

BPLStatus	OPL_NetworkManager_RegisterListener( OPL_NETWORK_EventCallback callback )
{
	s_pfn_netmanager_callback = callback;
	APK_NETWORK_RegisterNotifier( _OPL_NETWORK_MANAGER_Notifier );

	return BPL_STATUS_OK;
}

BPLStatus	OPL_NetworkManager_UnRegisterListener( void )
{
	s_pfn_netmanager_callback = NULL;
	APK_NETWORK_UnRegisterNotifier( _OPL_NETWORK_MANAGER_Notifier );
	return BPL_STATUS_OK;
}

BPLStatus OPL_NetworkWiFi_ApScanning( int nDevIndex )
{
	APK_NETWORK_WIFI_StartScan( nDevIndex );
	return BPL_STATUS_OK;
}

void	OPL_NetworkWiFi_RegisterListener( OPL_NETWORK_EventCallback callback )
{
	s_pfn_netwifi_callback = callback;
	APK_NETWORK_RegisterNotifier( _OPL_NETWORK_WIFI_Notifier );
}

void	OPL_NetworkWiFi_UnRegisterListener( void )
{
	s_pfn_netwifi_callback = NULL;
	APK_NETWORK_UnRegisterNotifier( _OPL_NETWORK_WIFI_Notifier );
}

BPLStatus	OPL_NetworkWiFi_ConnectToAP( int nDevIndex, OplNetworkAccessPoint * ap, const char *szKey )
{
	APKS_NETAP_Info_t apInfo;
	char apKey[64] = { 0 };

	memcpy( &apInfo, ap, sizeof(APKS_NETAP_Info_t) );
	strncpy( apKey, szKey, sizeof(apKey) -1 );

	APK_NETWORK_WIFI_ConnectAP( nDevIndex ,&apInfo, apKey );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_NetworkWiFi_ConnectToAPbyWPS( int nDevIndex, int nType, const char *szPincode )
{
	BPLStatus result = BPL_STATUS_ERROR;
	char pincode[64] = { 0 };
	APKE_NETAP_WPS_Type_e eType = eNETAP_WPS_PIN;

	if( szPincode == NULL )
		return BPL_STATUS_ERROR;

	if ( strcmp( "auto", szPincode ) == 0 )
	{
		HCHAR szMac[32] = { 0 };
		HINT32 p[8] = { 0 };
		HINT32 checkSum = 0;
		HINT32 accum = 0;
		HINT32 i = 0;
		HCHAR tmp[3] = { 0 };

		APK_NETWORK_CONF_GetMacAddr( 0, szMac, sizeof(szMac) );

		HxSTD_StrNCpy(tmp, &szMac[0], 2 );
		p[0] = HLIB_STD_HexToInt(tmp) % 10;
		HxSTD_StrNCpy(tmp, &szMac[3], 2 );
		p[1] = HLIB_STD_HexToInt(tmp) % 10;
		HxSTD_StrNCpy(tmp, &szMac[6], 2 );
		p[2] = HLIB_STD_HexToInt(tmp) % 10;
		HxSTD_StrNCpy(tmp, &szMac[9], 2 );
		p[3] = HLIB_STD_HexToInt(tmp) % 10;
		HxSTD_StrNCpy(tmp, &szMac[12], 2 );
		p[4] = HLIB_STD_HexToInt(tmp) % 10;
		HxSTD_StrNCpy(tmp, &szMac[15], 2 );
		p[5] = HLIB_STD_HexToInt(tmp) % 10;
		p[6] = p[0] * p[5] % 10;

		for ( i = 0; i < 7 ; i++ )
		{
			if ( (i+1) % 2 )
				accum += p[i] * 3;
			else
				accum += p[i] * 1;
		}

		checkSum = 10 - ( accum  % 10 );
		p[7] = (10 == checkSum) ? 0 : checkSum ;

		HxSTD_MemSet( pincode, 0x00, sizeof(pincode) );

		HxSTD_PrintToStrN( pincode, sizeof(pincode), "%d%d%d%d%d%d%d%d", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
	}
	else
	{
		strncpy( pincode, szPincode, sizeof(pincode) - 1 );
	}

	HxLOG_Print("(*) WPS - Excute. ConnectionType(%d), PinCode(%s)\n", eType, szPincode );
	if ( ( 0 > eType ) < ( eNETAP_WPS_MAX < eType ) )
	{
		return BPL_STATUS_ERROR;
	}

	eType = (APKE_NETAP_WPS_Type_e)nType;

	result = (BPLStatus)APK_NETWORK_WIFI_ConnectAPByWPS( nDevIndex, eType, pincode );
	if ( BPL_STATUS_OK != result )
	{
		HxLOG_Print("(*)WPS - Excute Error(%d)\n", result );
		return BPL_STATUS_ERROR;
	}
	return result;
}

BPLStatus	OPL_NetworkWiFi_GetPincodeByWPS( int nDevIndex, HCHAR *szPincode )
{
	BPLStatus result = BPL_STATUS_ERROR;
	HCHAR tmpPincode[APKD_WPS_PIN_LEN_MAX] = {0, };

	result = (BPLStatus)APK_NETWORK_WIFI_GetPincodeByWPS(nDevIndex, tmpPincode);
	if ( BPL_STATUS_OK != result )
	{
		HxLOG_Print("(*)WPS - GetWpsPincode Error(%d)\n", result );
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemCopy(szPincode, tmpPincode, APKD_WPS_PIN_LEN_MAX);

	return result;
}

BPLStatus	OPL_NetworkWiFi_GetConnectedInfo( int nDevIndex, OplNetworkAccessPoint * ap )
{
	HERROR eRet = ERR_FAIL;
	APKS_NETAP_Info_t apInfo;
	HCHAR szKey[128] = { 0 };

	if ( NULL == ap )
	{
		HxLOG_Error("(*) OPL_NetworkWiFi_GetConnectedInfo() Err. ap == NULL.\n");
		return BPL_STATUS_ERROR;
	}
	HxSTD_MemSet( &apInfo, 0x00, sizeof(APKS_NETAP_Info_t) );

	eRet = APK_NETWORK_WIFI_GetConnectedInfo( nDevIndex, &apInfo, szKey, sizeof(szKey) );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Getting Connected AP Information is failed.\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_StrNCpy( ap->szESSID, apInfo.szESSID, sizeof(ap->szESSID) - 1 );
	HxSTD_StrNCpy( ap->szMacAddr, apInfo.szMacAddr, sizeof(ap->szMacAddr) - 1 );
	ap->nOpMode 		= apInfo.nOpMode;
	ap->nSecureType		= apInfo.nSecureType;
	ap->nAuthenType		= apInfo.nAuthenType;
	ap->nEncryptType	= apInfo.nEncryptType;
	ap->nWaveStrength	= apInfo.nWaveStrength;
	ap->nConnectSpeed	= apInfo.nConnectSpeed;
	ap->bIsConnected	= apInfo.bIsConnected;

	return BPL_STATUS_OK;
}

BPLStatus OPL_NetworkWiFi_GetFWVersion(int nDevIndex, char * wifiFWVersion, unsigned int max_len)
{
	APKS_NETDEVINFO_t stDevInfo;
	HERROR eRet = ERR_OK;

	eRet = APK_NETWORK_CONF_GetDevInfo( nDevIndex, &stDevInfo );
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", nDevIndex);
		return BPL_STATUS_ERROR;
	}
	else
	{
		HLIB_STD_StrNCpySafe(wifiFWVersion, stDevInfo.szFwVer, max_len);
		HxLOG_Info("Getting the firmware version of Network Device(%d) is successful.\n", nDevIndex);
		return BPL_STATUS_OK;
	}
}

BPLStatus   OPL_NetworkWiFi_SetWakeupOnWlan(int nDevIndex, OplNetWoWlan_Mode_t etMode)
{
	HERROR eRet = ERR_OK;

	eRet = APK_NETWORK_WIFI_SetWakeupOnWlan(nDevIndex, (APKS_NETWoWlan_Mode_t)etMode);
	if ( eRet != ERR_OK )
	{
		return BPL_STATUS_ERROR;
	}
	else
	{
		return BPL_STATUS_OK;
	}
}

#if defined(CONFIG_OP_JAPAN)
HINT32 OPL_NetworkWiFi_ConvertToSecurityInt( int nSecureType )
{
	HINT32 retInt = 0;
	switch ( nSecureType )
	{
		case eInet_Wifi_Sec_Open :
			retInt = 0;
			break;
		case eInet_Wifi_Sec_WEP :
			retInt = 1;
			break;
		case eInet_Wifi_Sec_WPA :
			retInt = 2;
			break;
		case eInet_Wifi_Sec_WPA2 :
			retInt = 3;
			break;
		case eInet_Wifi_Sec_WPA_WPA2 :
			retInt = 4;
			break;
		default :
			break;
	}
	return retInt;
}

HINT32 OPL_NetworkWiFi_ConvertToAuthenInt( int nAuthenType )
{
	HINT32 retInt = 0;
	switch ( nAuthenType )
	{
		case eInet_Wifi_Auth_Open :
			retInt = 0;
			break;
		case eInet_Wifi_Auth_WPAPSK :
			retInt = 1;
			break;
		case eInet_Wifi_Auth_802_1x :
			retInt = 2;
			break;
		default :
			break;
	}
	return retInt;
}

HINT32  OPL_NetworkWiFi_ConvertToEncryptInt( int nEncryptType )
{
	HINT32 retInt = 0;
	switch ( nEncryptType )
	{
		case eINet_Wifi_Encrypt_None :
			retInt = 0;
			break;
		case eINet_Wifi_Encrypt_WEP_64 :
			retInt = 1;
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP :
			retInt = 2;
			break;
		case eINet_Wifi_Encrypt_WPA_AES :
			retInt = 3;
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP_AES :
			retInt = 4;
			break;
		case eINet_Wifi_Encrypt_WEP_64_Hex :
			retInt = 5;
			break;
		case eINet_Wifi_Encrypt_WEP_64_Ascii :
			retInt = 6;
			break;
		case eInet_Wifi_Encrypt_WEP_128_Hex :
			retInt = 7;
			break;
		case eInet_Wifi_Encrypt_WEP_128_Ascii :
			retInt = 8;
			break;
		default :
			break;
	}
	return retInt;
}
#else
HINT32	OPL_NetworkWiFi_ConvertToSecurityInt( int nSecureType )
{
	APKE_INET_Wifi_Security_Type_e eSecure = eInet_Wifi_Sec_Open;

	switch ( nSecureType )
	{
		case 0 : eSecure = eInet_Wifi_Sec_Open; break;
		case 1 : eSecure = eInet_Wifi_Sec_WEP; break;
		case 2 : eSecure = eInet_Wifi_Sec_WPA; break;
		case 3 : eSecure = eInet_Wifi_Sec_WPA2; break;
		default :
			break;
	}
	return eSecure;
}

HINT32	OPL_NetworkWiFi_ConvertToAuthenInt( int nAuthenType )
{
	APKE_INET_Wifi_Authen_Type_e eAuthen = eInet_Wifi_Auth_Open;

	switch ( nAuthenType )
	{
		case 0 :
			eAuthen = eInet_Wifi_Auth_Open; break;
		case 1 :
			eAuthen = eInet_Wifi_Auth_WPAPSK; break;
		default :
			break;
	}
	return eAuthen;
}

HINT32	OPL_NetworkWiFi_ConvertToEncryptInt( int nEncryptType )
{
	APKE_INet_Wifi_Encypt_e eEncrypt = eINet_Wifi_Encrypt_None;
	switch ( nEncryptType )
	{
		case 0 :
			eEncrypt = eINet_Wifi_Encrypt_None; break;
		case 1 :
			eEncrypt = eINet_Wifi_Encrypt_WEP_64; break;
		case 2 :
			eEncrypt = eINet_Wifi_Encrypt_WEP_64_Hex; break;
		case 3 :
			eEncrypt = eINet_Wifi_Encrypt_WEP_128; break;
		case 4 :
			eEncrypt = eInet_Wifi_Encrypt_WEP_128_Hex ; break;
		case 5 :
			eEncrypt = eINet_Wifi_Encrypt_WPA_TKIP ; break;
		case 6 :
			eEncrypt = eINet_Wifi_Encrypt_WPA_AES ; break;
		case 7 :
			eEncrypt = eINet_Wifi_Encrypt_WPA_TKIP_AES ; break;
		default :
			break;
	}
	return eEncrypt;
}
#endif

HINT32 OPL_NetworkWiFi_ConvertToModeInt( int nModeType )
{
	HINT32 retInt = 0;
	switch ( nModeType )
	{
		case eInet_Wlan_Dev_Auto :
		case eInet_Wlan_Dev_Infra :
		case eInet_Wlan_Dev_Master :
			retInt = 0;
			break;
		case eInet_Wlan_Dev_Adhoc :
			retInt = 1;
			break;
		default :
			retInt = 2;
			break;
	}
	return retInt;
}

const char * OPL_NetworkWiFi_ConvertToSecuritySTR( int nSecureType )
{
	char *retStr = (char *)"";
	switch ( nSecureType )
	{
		case eInet_Wifi_Sec_Open :
			retStr = (char *)"OPEN";
			break;
		case eInet_Wifi_Sec_WEP :
			retStr = (char *)"WEP";
			break;
		case eInet_Wifi_Sec_WPA :
			retStr = (char *)"WPA";
			break;
		case eInet_Wifi_Sec_WPA2 :
			retStr = (char *)"WPA2";
			break;
		case eInet_Wifi_Sec_WPA_WPA2 :
			retStr = (char *)"WPA/WPA2";
			break;
		default :
			break;
	}
	return retStr;
}

const char * OPL_NetworkWiFi_ConvertToAuthenSTR( int nAuthenType )
{
	char *retStr = (char *)"";
	switch ( nAuthenType )
	{
		case eInet_Wifi_Auth_Open :
			retStr = (char *)"NONE";
			break;
		case eInet_Wifi_Auth_WPAPSK :
			retStr = (char *)"PSK";
			break;
		case eInet_Wifi_Auth_802_1x :
			retStr = (char *)"802.1x";
			break;
		default :
			break;
	}
	return retStr;
}

const char * OPL_NetworkWiFi_ConvertToEncryptSTR( int nEncryptType )
{
	char *retStr = (char *)"";
	switch ( nEncryptType )
	{
		case eINet_Wifi_Encrypt_None :
			retStr = (char *)"NONE";
			break;
		case eINet_Wifi_Encrypt_WEP_64 :
		case eINet_Wifi_Encrypt_WEP_64_Hex :
			retStr = (char *)"WEP64_HEX";
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP :
			retStr = (char *)"TKIP";
			break;
		case eINet_Wifi_Encrypt_WPA_AES :
			retStr = (char *)"AES";
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP_AES :
			retStr = (char *)"TKIP/AES";
			break;
		case eINet_Wifi_Encrypt_WEP_64_Ascii :
			retStr = (char *)"WEP64_ASCII";
			break;
		case eInet_Wifi_Encrypt_WEP_128_Hex :
			retStr = (char *)"WEP128_HEX";
			break;
		case eInet_Wifi_Encrypt_WEP_128_Ascii :
			retStr = (char *)"WEP128_ASCII";
			break;
		default :
			break;
	}
	return retStr;
}

void	OPL_NetworkPPPoE_RegisterListener( OPL_NETWORK_EventCallback callback )
{
	s_pfn_pppoe_callback = callback;
	APK_NETWORK_RegisterNotifier( _OPL_NETWORK_PPPoE_Notifier );
}

void	OPL_NetworkPPPoE_UnRegisterListener( void )
{
	s_pfn_pppoe_callback = NULL;
	APK_NETWORK_UnRegisterNotifier( _OPL_NETWORK_PPPoE_Notifier );
}

BPLStatus OPL_NetworkPPPoE_GetInfo( int nDevIndex, char **szId, char **szPassword )
{
	HERROR eRet = ERR_FAIL;
	APKS_PPPOE_Info_t stPPPOEInfo;

	if ( szId == NULL || szPassword == NULL )
	{
		HxLOG_Error("Getting PPPoE Information is failed.\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet( &stPPPOEInfo, 0x00, sizeof(APKS_PPPOE_Info_t) );
	stPPPOEInfo.dev_id = nDevIndex;

	eRet = APK_NETWORK_PPPOE_GetInfo( &stPPPOEInfo );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Getting PPPoE Information is failed.\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_StrNCpy( *szId, stPPPOEInfo.szUserID, sizeof(stPPPOEInfo.szUserID) - 1 );
	HxSTD_StrNCpy( *szPassword, stPPPOEInfo.szUserPW, sizeof(stPPPOEInfo.szUserPW) - 1 );

	return  BPL_STATUS_OK;
}

BPLStatus OPL_NetworkPPPoE_SetInfo( int nDevIndex, const char *szId, const char *szPassword )
{
	HERROR eRet = ERR_FAIL;
	APKS_PPPOE_Info_t stPPPOEInfo;

	if ( szId == NULL || szPassword == NULL )
	{
		HxLOG_Error("Setting PPPoE Information is failed.\n");
		return BPL_STATUS_ERROR;
	}

	if( szId[0] == '\0' || szPassword[0] == '\0' )
	{
		HxLOG_Error("Setting PPPoE Information is failed. (id/pw)\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet( &stPPPOEInfo, 0x00, sizeof(APKS_PPPOE_Info_t) );
	stPPPOEInfo.dev_id = nDevIndex;
	HxSTD_StrNCpy( stPPPOEInfo.szUserID, szId, sizeof(stPPPOEInfo.szUserID) - 1 );
	HxSTD_StrNCpy( stPPPOEInfo.szUserPW, szPassword, sizeof(stPPPOEInfo.szUserID) - 1 );

	eRet = APK_NETWORK_PPPOE_SetInfo( &stPPPOEInfo );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Setting PPPoE Information is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return  BPL_STATUS_OK;
}

BPLStatus OPL_NetworkPPPoE_GetOnOff( int nDevIndex, BPL_BOOL *bOn )
{
	HERROR eRet = ERR_FAIL;

	if ( bOn == NULL )
	{
		HxLOG_Error("Getting PPPoE on/off status is failed.\n");
		return BPL_STATUS_ERROR;
	}

	eRet = APK_NETWORK_PPPOE_GetOnOff( (HINT32) nDevIndex, (HBOOL*)bOn );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Getting PPPoE on/off status is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return  BPL_STATUS_OK;
}

BPLStatus OPL_NetworkPPPoE_SetOnOff( int nDevIndex, BPL_BOOL bOn )
{
	HERROR eRet = ERR_FAIL;

	eRet = APK_NETWORK_PPPOE_SetOnOff( (HINT32) nDevIndex, (HBOOL)bOn );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Setting PPPoE on/off status is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return  BPL_STATUS_OK;
}

BPLStatus OPL_NetworkPPPoE_Connect( int nDevIndex, const char *szId, const char *szPassword )
{
	HERROR eRet = ERR_FAIL;
	APKS_PPPOE_Info_t stPPPOEInfo;

	if ( szId == NULL || szPassword == NULL )
	{
		HxLOG_Error("Connecting PPPoE is failed.\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet( &stPPPOEInfo, 0x00, sizeof(APKS_PPPOE_Info_t) );
	stPPPOEInfo.dev_id = nDevIndex;
	HxSTD_StrNCpy( stPPPOEInfo.szUserID, szId, sizeof(stPPPOEInfo.szUserID) - 1 );
	HxSTD_StrNCpy( stPPPOEInfo.szUserPW, szPassword, sizeof(stPPPOEInfo.szUserID) - 1 );

	eRet = APK_NETWORK_PPPOE_Connect( &stPPPOEInfo );
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Connecting PPPoE is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_NetworkPPPoE_Disconnect( int nDevIndex )
{
	HERROR eRet = ERR_FAIL;

	eRet = APK_NETWORK_PPPOE_Disconnect(nDevIndex);
	if ( ERR_OK != eRet )
	{
		HxLOG_Error("Disconnecting PPPoE is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Network_GetFTPStatus( int *pnStatus)
{
//	APK_EXE_FTP_GetStatus(pnStatus);
	return  BPL_STATUS_OK;
}

BPLStatus OPL_Network_SetFTPStatus( int nStatus)
{
	return  BPL_STATUS_OK;

#if 0
	if (nStatus)
		APK_EXE_FTP_Start();

	APK_EXE_FTP_Stop();
	return  BPL_STATUS_OK;
#endif
}

BPLStatus OPL_Network_SetInternetConnection( HINT32 eEvent )
{
    int nIndex = 0;
	APKS_NETDEVINFO_t stDevInfo;
	HERROR eRet = ERR_OK;

    switch ( eEvent )
    {
    	case eNETWORK_STATUS_CON_SUCCESS :
        case eNETWORK_STATUS_CON_FAIL :
        case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
        case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
        case eNETWORK_STATUS_WIFI_USB_INSERTED :
        case eNETWORK_STATUS_WIFI_USB_EXTRACTED :
            APK_NETWORK_CONF_GetDefaultDev( &nIndex );
            eRet = APK_NETWORK_CONF_GetDevInfo( nIndex, &stDevInfo );
            if ( eRet == ERR_OK && TRUE == stDevInfo.bNetActive )
            {
                s_internet_stataus = true;
                HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 1);
            }
            else
            {
                s_internet_stataus = false;
                HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 0);
            }
    		break;

        case eNETWORK_STATUS_INTERNET_CONNECTED :
            s_internet_stataus = true;
            HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 1);
            break;

        case eNETWORK_STATUS_INTERNET_DISCONNECTED :
            s_internet_stataus = false;
            HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 0);
            break;
    	default :
    		break;
    }
	return BPL_STATUS_OK;
}

BPLStatus OPL_Network_GetInternetConnection(int *status )
{
    *status = s_internet_stataus;
    return BPL_STATUS_OK;
}



/********************************************************************************
 *	OplNetworkInterface class
 ********************************************************************************
 */

std::list<OplNetworkInterface *> OplNetworkInterface::m_sInstanceList;
OplNetworkInterface::OplNetworkInterface( int nIndex ) : m_nIndex( nIndex )
{
	setupByIndex( nIndex );
	m_sInstanceList.push_back(this);

    if(m_sInstanceList.size() == 1)
        APK_NETWORK_RegisterNotifier( OplNetworkInterface::onEventNotifier );

#if defined(CONFIG_OP_JAPAN)
	strncpy(Cmds[0].szCmd ,  "ipv4Address", strlen("ipv4Address"));
	Cmds[0].ValAddr 	= (HUINT32) &m_szIpAddr;
	Cmds[0].eType		= eNET_Property_Type_String;
	Cmds[0].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[1].szCmd ,  "subnetMask", strlen("subnetMask"));
	Cmds[1].ValAddr 	= (HUINT32) &m_szNetmask;
	Cmds[1].eType 		= eNET_Property_Type_String;
	Cmds[1].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[2].szCmd ,  "defaultGateway", strlen("defaultGateway"));
	Cmds[2].ValAddr 	= (HUINT32) &m_szGateway;
	Cmds[2].eType 		= eNET_Property_Type_String;
	Cmds[2].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[3].szCmd ,  "primaryDNSServerAddress", strlen("primaryDNSServerAddress"));
	Cmds[3].ValAddr 	= (HUINT32) &m_szDns1;
	Cmds[3].eType 		= eNET_Property_Type_String;
	Cmds[3].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[4].szCmd ,  "secondaryDNSServerAddress", strlen("secondaryDNSServerAddress"));
	Cmds[4].ValAddr 	= (HUINT32) &m_szDns2;
	Cmds[4].eType 		= eNET_Property_Type_String;
	Cmds[4].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[5].szCmd ,  "macAddress", strlen("macAddress"));
	Cmds[5].ValAddr 	= (HUINT32) &m_szMacAddress;
	Cmds[5].eType 		= eNET_Property_Type_String;
	Cmds[5].ulValSize 	= OPLNET_MAX_MACADDR_LEN;

	strncpy(Cmds[6].szCmd ,  "dhcpEnable", strlen("dhcpEnable"));
	Cmds[6].ValAddr 	= (HUINT32) &m_bDhcpOn;
	Cmds[6].eType 		= eNET_Property_Type_Bool;
	Cmds[6].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[7].szCmd ,  "dnsAuto", strlen("dnsAuto"));
	Cmds[7].ValAddr 	= (HUINT32) &m_bDnsAuto;
	Cmds[7].eType 		= eNET_Property_Type_Bool;
	Cmds[7].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[8].szCmd ,  "ipv6Address", strlen("ipv6Address"));
	Cmds[8].ValAddr 	= (HUINT32) &m_szIpv6Addr;
	Cmds[8].eType 		= eNET_Property_Type_String;
	Cmds[8].ulValSize 	= OPLNET_MAX_IPV6ADDR_LEN;

	strncpy(Cmds[9].szCmd ,  "prefixLength", strlen("prefixLength"));
	Cmds[9].ValAddr 	= (HUINT32) &m_nIpv6Prefix;
	Cmds[9].eType 		= eNET_Property_Type_Interger;
	Cmds[9].ulValSize 	= sizeof(int);

	strncpy(Cmds[10].szCmd ,  "ipv6Gateway", strlen("ipv6Gateway"));
	Cmds[10].ValAddr 	= (HUINT32) &m_szIpv6Gateway;
	Cmds[10].eType 		= eNET_Property_Type_String;
	Cmds[10].ulValSize 	= OPLNET_MAX_IPV6ADDR_LEN;

	strncpy(Cmds[11].szCmd ,  "ipv6dns1st", strlen("ipv6dns1st"));
	Cmds[11].ValAddr 	= (HUINT32) &m_szIpv6Dns1;
	Cmds[11].eType 		= eNET_Property_Type_String;
	Cmds[11].ulValSize 	= OPLNET_MAX_IPV6ADDR_LEN;

	strncpy(Cmds[12].szCmd ,  "ipv6dns2nd", strlen("ipv6dns2nd"));
	Cmds[12].ValAddr 	= (HUINT32) &m_szIpv6Dns2;
	Cmds[12].eType 		= eNET_Property_Type_String;
	Cmds[12].ulValSize 	= OPLNET_MAX_IPV6ADDR_LEN;

	strncpy(Cmds[13].szCmd ,  "tunnelStartIpv4Address", strlen("tunnelStartIpv4Address"));
	Cmds[13].ValAddr 	= (HUINT32) &m_szIpv6TnlStr;
	Cmds[13].eType 		= eNET_Property_Type_String;
	Cmds[13].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[14].szCmd ,  "tunnelEndIpv4Address", strlen("tunnelEndIpv4Address"));
	Cmds[14].ValAddr 	= (HUINT32) &m_szIpv6TnlEnd;
	Cmds[14].eType 		= eNET_Property_Type_String;
	Cmds[14].ulValSize 	= OPLNET_MAX_IPADDR_LEN;

	strncpy(Cmds[15].szCmd ,  "ipv6Type", strlen("ipv6Type"));
	Cmds[15].ValAddr 	= (HUINT32) &m_bIpv6Type;
	Cmds[15].eType 		= eNET_Property_Type_Bool;
	Cmds[15].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[16].szCmd ,  "ipv6dhcpOn", strlen("ipv6dhcpOn"));
	Cmds[16].ValAddr 	= (HUINT32) &m_bIpv6DhcpOn;
	Cmds[16].eType 		= eNET_Property_Type_Bool;
	Cmds[16].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[17].szCmd ,  "ipv6dnsAuto", strlen("ipv6dnsAuto"));
	Cmds[17].ValAddr 	= (HUINT32) &m_bIpv6DnsAuto;
	Cmds[17].eType 		= eNET_Property_Type_Bool;
	Cmds[17].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[18].szCmd ,  "connected", strlen("connected"));
	Cmds[18].ValAddr 	= (HUINT32) &m_bConnected;
	Cmds[18].eType 		= eNET_Property_Type_Bool;
	Cmds[18].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[19].szCmd ,  "enable", strlen("enable"));
	Cmds[19].ValAddr 	= (HUINT32) &m_bEnabled;
	Cmds[19].eType 		= eNET_Property_Type_Bool;
	Cmds[19].ulValSize 	= sizeof(BPL_BOOL);

	strncpy(Cmds[20].szCmd ,  "interfaceType", strlen("interfaceType"));
	Cmds[20].ValAddr 	= (HUINT32) &m_eNetType;
	Cmds[20].eType 		= eNET_Property_Type_Bool;
	Cmds[20].ulValSize 	= sizeof(BPL_BOOL);
#endif
}

OplNetworkInterface::~OplNetworkInterface()
{
	m_sInstanceList.remove(this);
    
    if(m_sInstanceList.size() == 0)
        APK_NETWORK_UnRegisterNotifier( OplNetworkInterface::onEventNotifier );
}

BPL_BOOL OplNetworkInterface::getConnected()
{
	HERROR eRet = ERR_OK;
	APKS_NETDEVINFO_t stDevInfo;
	eRet = APK_NETWORK_CONF_GetDevInfo( m_nIndex, &stDevInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", m_nIndex);
	}
	m_bConnected = stDevInfo.bConnected;
	return m_bConnected;
}
BPL_BOOL OplNetworkInterface::getEnabled()
{
	HERROR eRet = ERR_OK;

#if defined(CONFIG_OP_JAPAN)
	APKS_NETIFADDR_t stNetAddr;
	eRet = APK_NETWORK_CONF_GetAddrInfo( m_nIndex, &stNetAddr);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", m_nIndex);
	}
	m_bEnabled = stNetAddr.bEnable;
	return m_bEnabled;
#else
	APKS_NETDEVINFO_t stDevInfo;
	eRet = APK_NETWORK_CONF_GetDevInfo( m_nIndex, &stDevInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", m_nIndex);
	}
	m_bIpSetup = stDevInfo.bNetActive;
	return m_bIpSetup;
#endif
}
BPL_BOOL OplNetworkInterface::setEnabled( BPL_BOOL bOn )
{
#if defined(CONFIG_OP_JAPAN)
	m_bEnabled = bOn;
	APK_NETWORK_CONF_SaveEnableStatus( m_nIndex, bOn );
#else
	m_bIpSetup = bOn;
#endif

	return TRUE;
}

#if defined(CONFIG_OP_JAPAN)
BPL_BOOL OplNetworkInterface::get(char *szCmd, char **szRetVal)
{
	int i;
	OplNetworkCmd_t tmp;
	memset(&tmp, 0x00, sizeof(OplNetworkCmd_t));

	for(i=0; i<OPL_NETWORK_COMMAND_MAX; i++)
	{
#if 0
		if(strcmp(szCmd, Cmds[i].szCmd) == 0)
			memcpy(&tmp, Cmds[i], sizeof(OplNetworkCmd_t));
#endif
	}

	if(tmp.ValAddr == 0)
		return FALSE;

	if(tmp.eType == eNET_Property_Type_String)
	{
		HxLOG_Print("Type is String...\n");
		*szRetVal  = (char *)tmp.ValAddr ;
	}
	else if(tmp.eType == eNET_Property_Type_Bool)
	{
		HxLOG_Print("Type is Bool...\n");
		if((HBOOL *)tmp.ValAddr == 0)
			*szRetVal = (char *)"false";
		else
			*szRetVal = (char *)"true";
	}
	else if(tmp.eType == eNET_Property_Type_Interger)
	{
		HxLOG_Print("Type is Interger...\n");

	}
	else
		return FALSE;

	return TRUE;
}

BPL_BOOL OplNetworkInterface::set(char *szCmd, char *szParam)
{
	int i;
	OplNetworkCmd_t tmp;
	memset(&tmp, 0x00, sizeof(OplNetworkCmd_t));
	HxLOG_Print("[%s:%d] Network Set (Cmd:%s , Param(%s) \n", __FUNCTION__, __LINE__, szCmd, szParam);

	for(i=0; i<OPL_NETWORK_COMMAND_MAX; i++)
	{
#if 0
		if(strcmp(szCmd, Cmds[i].szCmd) == 0)
			memcpy(&tmp, Cmds[i], sizeof(OplNetworkCmd_t));
#endif
	}

	if(tmp.ValAddr == 0)
	{
		HxLOG_Print("not existed command \n");
		return ERR_FAIL;
	}

	if(tmp.eType == eNET_Property_Type_String)
	{
		HxLOG_Print("Type is String...(%s:%d)\n", szParam, strlen(szParam));
		if(strlen(szParam) > tmp.ulValSize)
		{
			HxLOG_Print("String Size Error \n");
			return ERR_FAIL;
		}

		memcpy((void *)tmp.ValAddr, szParam, tmp.ulValSize);
		HxLOG_Print("=========> %s \n", m_szIpv6Addr);
		return ERR_OK;
	}
	else if(tmp.eType == eNET_Property_Type_Bool)
	{
		HxLOG_Print("Type is Bool...\n");
		HBOOL *val = (HBOOL *)tmp.ValAddr;
		if(strcmp("true", szParam) == 0)
		{
			*val = 1;
			return ERR_OK;
		}
		else if(strcmp("flase", szParam) == 0)
		{
			*val = 0;
			return ERR_OK;
		}
		else
			return ERR_FAIL;
	}
	else if(tmp.eType == eNET_Property_Type_Interger)
	{
		HxLOG_Print("Type is Interger...\n");
		HUINT32 *val = (HUINT32 *)tmp.ValAddr;
		*val = atoi(szParam);
		return ERR_OK;
	}
	else
		return ERR_FAIL;

	HxLOG_Print("[%s:%d] \n", __FUNCTION__, __LINE__);
	return ERR_FAIL;
}
#endif


BPL_BOOL	 OplNetworkInterface::setupByIndex( int nIndex )
{
	HERROR eRet = ERR_OK;
	m_nIndex = nIndex;
	APKS_NETIFADDR_t ptNetAddr = {0, };

	eRet = APK_NETWORK_CONF_GetAddrInfo( nIndex, &ptNetAddr);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		return ERR_FAIL;
	}

	m_bDhcpOn	= ptNetAddr.bDhcpOn;
	m_bDnsAuto	= ptNetAddr.bDnsAuto;

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, m_szIpAddr, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the IP Address is failed.\n");
		return ERR_FAIL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulNetmask, m_szNetmask, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Netmask is failed.\n");
		return ERR_FAIL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulGateway, m_szGateway, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Gateway is failed.\n");
		return ERR_FAIL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns1, m_szDns1, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS1 is failed.\n");
		return ERR_FAIL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns2, m_szDns2, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS2 is failed.\n");
		return ERR_FAIL;
	}

//	HBOOL   bIpv6;                  /* whether using ipv6 address or not */
//	HBOOL   bEnable;                /* temp */

	HxSTD_StrNCpy( m_szIpv6Addr,		ptNetAddr.szIp6_addr, OPLNET_MAX_IPV6ADDR_LEN -1);
	HxSTD_StrNCpy( m_szIpv6Gateway,	ptNetAddr.szIp6_gateway, OPLNET_MAX_IPV6ADDR_LEN -1);
	HxSTD_StrNCpy( m_szIpv6Dns1, 		ptNetAddr.szIp6_dns_addr_1, OPLNET_MAX_IPV6ADDR_LEN -1);
	HxSTD_StrNCpy( m_szIpv6Dns2, 		ptNetAddr.szIp6_dns_addr_2, OPLNET_MAX_IPV6ADDR_LEN -1);

	m_bIpv6Type		= ptNetAddr.bIpv6;
	m_nIpv6Prefix	= ptNetAddr.nPrefix;

	switch(ptNetAddr.bIpv6Mode)
	{
		case 0: m_bIpv6DhcpOn   =  0;   m_bIpv6StatelessMode = 0; break;
		case 1: m_bIpv6DhcpOn   =  1;   m_bIpv6StatelessMode = 0; break;
		case 2: m_bIpv6DhcpOn   =  0;   m_bIpv6StatelessMode = 1; break;
		default :  m_bIpv6DhcpOn   =  0;   m_bIpv6StatelessMode = 0; break;
	}

	m_bIpv6DnsAuto 	= ptNetAddr.bDns6;

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulTunnelStartAddr, m_szIpv6TnlStr, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS2 is failed.\n");
		return ERR_FAIL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulTunnelEndAddr, m_szIpv6TnlEnd, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS2 is failed.\n");
		return ERR_FAIL;
	}

	APKS_NETDEVINFO_t ptDevInfo;
	eRet = APK_NETWORK_CONF_GetDevInfo( nIndex, &ptDevInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device is failed.\n");
		return ERR_FAIL;
	}

	m_bEnabled = ptNetAddr.bEnable;
	m_bConnected = ptDevInfo.bConnected;
	m_bIpSetup = ptDevInfo.bNetActive;
	strncpy( m_szMacAddress, ptDevInfo.szMacAddr, (OPLNET_MAX_MACADDR_LEN-1));
	strncpy( m_szInterfaceName, ptDevInfo.szDev_name, (OPLNET_MAX_NAME_LEN-1));

	switch ( ptDevInfo.eType )
	{
		case eINTERFACE_TYPE_LAN :
			m_eNetType = OPLNET_TYPE_LAN;
			break;
		case eINTERFACE_TYPE_WLAN :
			m_eNetType = OPLNET_TYPE_WLAN;
			break;
		case eINTERFACE_TYPE_PPPOE :
			m_eNetType = OPLNET_TYPE_PPPOE;
			break;
		default:
			m_eNetType = OPLNET_TYPE_NONE;
			break;
	}

	return ERR_OK;
}

BPL_BOOL	OplNetworkInterface::refreshData()
{
	return setupByIndex( m_nIndex );
}

BPL_BOOL	OplNetworkInterface::setupByAddressType( ADDRESS_Type_e eAddrType, const char *address )
{
	char * targetAddr = NULL;

	if ( NULL == address )
		return FALSE;

	switch ( eAddrType )
	{
		case eADDR_IPv4 :
			targetAddr = m_szIpAddr;
			break;
		case eADDR_NETMASK :
			targetAddr = m_szNetmask;
			break;
		case eADDR_GATEWAY :
			targetAddr = m_szGateway;
			break;
		case eADDR_DNS1 :
			targetAddr = m_szDns1;
			break;
		case eADDR_DNS2 :
			targetAddr = m_szDns2;
			break;

		case eADDR_IPV6:
			targetAddr = m_szIpv6Addr;
			break;
		case eAddr_IPV6_GATEWAY:
			targetAddr = m_szIpv6Gateway;
			break;
        case eADDR_IPV6_DNS1:
			targetAddr = m_szIpv6Dns1;
			break;
		case eADDR_IPV6_DNS2:
			targetAddr = m_szIpv6Dns2;
			break;

		case eADDR_IPV6_TUNNEL_START:
			targetAddr = m_szIpv6TnlStr;
			break;
		case eADDR_IPV6_TUNNEL_END:
			targetAddr = m_szIpv6TnlEnd;
			break;


		default :
			return FALSE;
	}

	if ( NULL == targetAddr )
		return FALSE;

	if(eAddrType < eADDR_IPV6)
		strncpy( targetAddr, address, OPLNET_MAX_IPADDR_LEN -1 );
	else
		strncpy( targetAddr, address, OPLNET_MAX_IPV6ADDR_LEN -1 );

	return TRUE;
}

BPL_BOOL    OplNetworkInterface::setInterfaceName( const char * pszName)
{
	if(strlen(pszName) > (OPLNET_MAX_NAME_LEN -1) ) {
		return FALSE;
	} else {
		strncpy(m_szInterfaceName, pszName, (OPLNET_MAX_NAME_LEN-1) );
	}
	return TRUE;
}

BPL_BOOL	OplNetworkInterface::applyConfigure( void )
{
	APKS_NETIFADDR_t stNetAddr;
	HERROR eRet = ERR_FAIL;

	stNetAddr.bDhcpOn = m_bDhcpOn;
	stNetAddr.bDnsAuto = m_bDnsAuto;

	APK_NETWORK_CONF_AddrA2N( m_szIpAddr, &stNetAddr.ulIpAddr );
	APK_NETWORK_CONF_AddrA2N( m_szNetmask, &stNetAddr.ulNetmask );
	APK_NETWORK_CONF_AddrA2N( m_szGateway, &stNetAddr.ulGateway );
	APK_NETWORK_CONF_AddrA2N( m_szDns1, &stNetAddr.ulDns1 );
	APK_NETWORK_CONF_AddrA2N( m_szDns2, &stNetAddr.ulDns2 );

	//case 0: m_bIpv6DhcpOn   =  0;   m_bIpv6StatelessMode = 0; break;

	if( m_bIpv6StatelessMode == TRUE)
		stNetAddr.bIpv6Mode = 2;
	else if( m_bIpv6DhcpOn == TRUE)
		stNetAddr.bIpv6Mode = 1;
	else
		stNetAddr.bIpv6Mode = 0;

	stNetAddr.bIpv6 		= m_bIpv6Type;
	stNetAddr.bDns6		= m_bIpv6DnsAuto;
	stNetAddr.nPrefix 	= m_nIpv6Prefix;

	HxSTD_StrNCpy( stNetAddr.szIp6_addr,          m_szIpv6Addr, OPLNET_MAX_IPV6ADDR_LEN -1 );
	HxSTD_StrNCpy( stNetAddr.szIp6_gateway, 		m_szIpv6Gateway, OPLNET_MAX_IPV6ADDR_LEN -1 );
	HxSTD_StrNCpy( stNetAddr.szIp6_dns_addr_1,    m_szIpv6Dns1, OPLNET_MAX_IPV6ADDR_LEN -1 );
	HxSTD_StrNCpy( stNetAddr.szIp6_dns_addr_2, 	m_szIpv6Dns2, OPLNET_MAX_IPV6ADDR_LEN -1 );

	APK_NETWORK_CONF_AddrA2N( m_szIpv6TnlStr, &stNetAddr.ulTunnelStartAddr);
	APK_NETWORK_CONF_AddrA2N( m_szIpv6TnlEnd, &stNetAddr.ulTunnelEndAddr);
	eRet = APK_NETWORK_CONF_SetAddrInfo( m_nIndex, &stNetAddr );

	if(ERR_OK == eRet)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BPL_BOOL	OplNetworkInterface::stopConnecting( void )
{
	APKS_NETDEVINFO_t stDevInfo;

	HERROR eRet = ERR_OK;
	eRet = APK_NETWORK_CONF_GetDevInfo( m_nIndex, &stDevInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device(%d) is failed.\n", m_nIndex);
	}

	if( stDevInfo.eType == eINTERFACE_TYPE_WLAN )
	{
		eRet = APK_NETWORK_WIFI_DisconnectAP( m_nIndex );
		if ( eRet != ERR_OK )
		{
			HxLOG_Error("stopping connection of Network Device(%d) is failed.\n", m_nIndex);
		}

	}
	else if( stDevInfo.eType == eINTERFACE_TYPE_LAN )
	{
		eRet = APK_NETWORK_CONF_StopConnecting( m_nIndex );
		if ( eRet != ERR_OK )
		{
			HxLOG_Error("stopping connection of Network Device(%d) is failed.\n", m_nIndex);
		}
	}
	else if( stDevInfo.eType == eINTERFACE_TYPE_PPPOE )
	{
		eRet = APK_NETWORK_PPPOE_Disconnect( m_nIndex );
		if ( eRet != ERR_OK )
		{
			HxLOG_Error("stopping connection of Network Device(%d) is failed.\n", m_nIndex);
		}
	}

	return TRUE;
}

int	OplNetworkInterface::checkGatewayAccess( void )
{
	int devIdx = m_nIndex;
	//5000 (5 seconds) is the timeout value. It could be optimized later.
	m_gwReqId = APK_NETWORK_CONCHK_Gateway( devIdx, OplNetworkInterface::onEventNotifier, 5000 );
	return 0;
}

int	OplNetworkInterface::checkDnsAccess( void )
{
	int devIdx = m_nIndex;
	m_dnsReqId = APK_NETWORK_CONCHK_Dns( devIdx, OplNetworkInterface::onEventNotifier, 0 );
	return 0;
}

int	OplNetworkInterface::checkInternetConnection( void )
{
	HERROR eRet = ERR_FAIL;
	eRet = APK_NETWORK_CONF_CheckInternetConnection(30000); /* 30 seconds */
	return eRet == ERR_OK ? TRUE : FALSE;
}


//static
OPL_NETWORK_EventCallback *OplNetworkInterface::m_sfnNetworkEventCallback = NULL;

// static
void 		OplNetworkInterface::registerEventListener( OplNetworkInterface *netInterface, OPL_NETWORK_EventCallback callback )
{
	HxLOG_Print("[%s,%d]#################### Event ####################\n", __FUNCTION__, __LINE__ );
	OplNetworkInterface::m_sfnNetworkEventCallback = callback;
	//APK_NETWORK_RegisterNotifier( OplNetworkInterface::onEventNotifier );
}

void 		OplNetworkInterface::unRegisterEventListener( OplNetworkInterface *netInterface )
{
	OplNetworkInterface::m_sfnNetworkEventCallback = NULL;
	//APK_NETWORK_UnRegisterNotifier( OplNetworkInterface::onEventNotifier );
}

// static
void		OplNetworkInterface::onEventNotifier(HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	OplNetLinkStatus_e eStatus;
	HINT32	nDevIndex = ulParam1;
	if ( NULL == OplNetworkInterface::m_sfnNetworkEventCallback  || OplNetworkInterface::m_sInstanceList.size() == 0)
		return ;

	switch ( eEvent )
	{
		case eNETWORK_STATUS_CON_SUCCESS :
			eStatus = eNET_CONNECT_TRY_SUCCESS;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_CON_FAIL :
			eStatus = eNET_CONNECT_TRY_FAIL;
			OplNetworkInterface::refreshDataAll();
			break;
#if defined(CONFIG_OP_JAPAN)
		case eNETWORK_STATUS_CON_AUTOIP_SUCCESS :
			eStatus = eNET_CONNECT_TRY_SUCCESS;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_CON_AUTOIP_FAIL :
			eStatus = eNET_CONNECT_TRY_FAIL;
			OplNetworkInterface::refreshDataAll();
			break;
#endif
		case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
			eStatus = eNET_CABLE_LINK_CONNECTD;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
			eStatus = eNET_CABLE_LINK_DICONNECTED;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_WIFI_USB_INSERTED :
			eStatus = eNET_WIFI_USB_INSERTED;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_WIFI_USB_EXTRACTED :
			eStatus = eNET_WIFI_USB_EXTRACTED;
			OplNetworkInterface::refreshDataAll();
			break;
		case eNETWORK_STATUS_DHCPD_START :
		case eNETWORK_STATUS_DHCPD_STOP :
			OplNetworkInterface::refreshDataAll();
			return ;
		case eNETWORK_STATUS_LINK_UPDATED :
			OplNetworkInterface::refreshDataAll();
			return ;
		case eNETWORK_CONCHK_ARP_SUCCESS :
			eStatus = eNET_ACCESS_GW_SUCCESS;
			break;
		case eNETWORK_CONCHK_ARP_FAIL :
			eStatus = eNET_ACCESS_GW_FAIL;
			break;
		case eNETWORK_CONCHK_RESOLV_SUCCESS :
			eStatus = eNET_ACCESS_DNS_SUCCESS;
			break;
		case eNETWORK_CONCHK_RESOLV_FAIL :
			eStatus = eNET_ACCESS_DNS_FAIL;
			break;
		case eNETWORK_STATUS_INTERNET_CONNECTED:
			eStatus = eNET_ACCESS_INTERNET_SUCCESS;
			break;
		case eNETWORK_STATUS_INTERNET_DISCONNECTED:
			eStatus = eNET_ACCESS_INTERNET_FAIL;
			break;
		default :
			return ;
	}

	m_sfnNetworkEventCallback( eStatus, 0, nDevIndex );
}

void OplNetworkInterface::refreshDataAll()
{
	std::list<OplNetworkInterface *>::iterator it;

	for ( it  = m_sInstanceList.begin(); it != m_sInstanceList.end(); it++ )
	{
		(*it)->refreshData();
	}

}

void OplNetworkInterface::refreshDataAll(HINT32 nIndex)
{
	std::list<OplNetworkInterface *>::iterator it;

	for ( it  = m_sInstanceList.begin(); it != m_sInstanceList.end(); it++ )
	{
		if ( (*it)->getIndex() == nIndex )
		{
			(*it)->refreshData();
		}
	}
}
/********************************************************************************
 *	OplNetworkManager class
 ********************************************************************************
 */

/********************************************************************************
 *	OplNetworkAccessPoint class
 ********************************************************************************
 */


#if defined(CONFIG_OP_JAPAN)
#define NETWORK_SERVICE_MAX	 21
typedef bool(fnc_set)(char *, void *, void *);

struct service_cmd_set{
	char ServiceName[32];
    char CmdName[32];
    fnc_set *SetFnc;
};

bool __dms_start(char *szCmd, void *p1, void *p2)
{
	HCHAR szFriendlyName[128];
	HERROR hErr;
	HxSTD_MemSet(szFriendlyName, 0, 128);
	hErr = APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szFriendlyName, 128 );

	//APK_DLNADMS_RegisterNotifier((CBObjectHandler) _OPL_DMS_ApkListener );
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)TRUE );
	return APK_DLNADMS_Start(szFriendlyName, FALSE);

}

bool __dms_stop(char *szCmd, void *p1, void *p2)
{
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)FALSE );

	return APK_DLNADMS_Stop();
}

bool __dms_status(char *szCmd, void *p1, void *p2)
{
	int nErr, bOn;
	int *a = (int *)p1;
	char *t;

	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32*)&bOn);

	if(nErr != ERR_OK)
		return ERR_FAIL;

	if(bOn)
   		t = HLIB_STD_StrDup("OK");
	else
   		t = HLIB_STD_StrDup("FAIL");

    *a  = (int)t;
	return ERR_OK;
}

//	if you want to use FTP fuction, please call "APK_EXE_FTP_INIT() first"
bool __ftp_start(char *szCmd, void *p1, void *p2)
{
//	return APK_EXE_FTP_Start();
	return FALSE;	//2 not used func...
}

bool __ftp_stop(char *szCmd, void *p1, void *p2)
{
//	return APK_EXE_FTP_Stop();
	return FALSE; //2 not used func...
}

bool __ftp_status(char *szCmd, void *p1, void *p2)
{
	HBOOL	bOn;
	int nErr = ERR_FAIL;
	int *a = (int *)p1;
	char *t;

//	nErr = APK_EXE_FTP_GetStatus(&bOn);

	if(nErr != ERR_OK)
		return ERR_FAIL;

	if(bOn)
   		t = HLIB_STD_StrDup("OK");
	else
   		t = HLIB_STD_StrDup("FAIL");

    *a  = (int)t;
	return ERR_OK;
}

NetService::NetService( OPL_NETWORKSERVICE_EventCallback callback )
{
	m_szName = (char *)"";
	HxSTD_MemSet( m_szTarget, 0x00, sizeof(m_szTarget) );

	m_nIfIndex = -1;
}

bool	NetService::SetTarget( const char *pszTarget )
{
	int numOfIf = 0;
	int i = 0;
	APKS_NETDEVINFO_t stDevInfo;

	HxSTD_StrNCpy( m_szTarget, pszTarget, sizeof(m_szTarget)-1 );

	APK_NETWORK_CONF_GetNumOfNetDev( &numOfIf );

	for ( i = 0; i < numOfIf; i++ )
	{
		HxSTD_MemSet( &stDevInfo, 0x00, sizeof(APKS_NETDEVINFO_t) );
		APK_NETWORK_CONF_GetDevInfo( i, &stDevInfo );

		if ( !HxSTD_StrNCmp( stDevInfo.szDev_name, pszTarget, HxSTD_StrLen(pszTarget )) )
		{
			m_nIfIndex = i;
			break;
		}
	}
	return true;
}

std::list<WiFiScanningService*> WiFiScanningService::m_sInstanceList;
OPL_NETWORKSERVICE_EventCallback *WiFiScanningService::m_fnCallback = NULL;
int WiFiScanningService::m_sApCount = 0;
void * WiFiScanningService::m_sApList = NULL;
WiFiScanningService::WiFiScanningService( OPL_NETWORKSERVICE_EventCallback callback ) : NetService(callback)
{
	m_szName = (char *)"WiFiScanner";

	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_RegisterNotifier( WiFiScanningService::NotifyEvent );
	}
	m_sInstanceList.push_back(this);

	m_fnCallback = callback;
}

WiFiScanningService::~WiFiScanningService()
{
	m_sInstanceList.remove(this);
	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_UnRegisterNotifier( WiFiScanningService::NotifyEvent );
	}
}

bool WiFiScanningService::Set( char *szName, char *szValue )
{
	return true;
}

char * WiFiScanningService::Get( char *szName )
{
	return NULL;
}

int	WiFiScanningService::Start()
{
	HERROR result = ERR_FAIL;
	result = APK_NETWORK_WIFI_StartScan( m_nIfIndex );
	if ( ERR_OK != result )
	{
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

void * WiFiScanningService::GetApList()
{
	return (void *)m_sApList;
}


void WiFiScanningService::NotifyEvent( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HINT32	nIfIndex = ulParam1 ;
	SvcWiFiScanEvent_e eOplEvent = eSVC_WIFI_SCAN_MAX;

	if ( NULL == m_fnCallback )
	{
		return ;
	}

	switch ( eEvent )
	{
		case eNETWORK_STATUS_WIFI_SCANNING_END :
			{
				HINT32 nApCount = ulParam3;
				APKS_NETAP_Info_t *apList = (APKS_NETAP_Info_t *)ulParam2;
				OplNetworkAccessPoint 	*pstApInfo = NULL;

				pstApInfo = (OplNetworkAccessPoint *)HLIB_STD_MemAlloc( sizeof(OplNetworkAccessPoint) * nApCount );
				HxSTD_MemCopy( pstApInfo, (void *)apList, sizeof(OplNetworkAccessPoint) * nApCount );

				m_sApCount = nApCount;
				if ( m_sApList )
				{
					HLIB_STD_MemFree( m_sApList );
					m_sApList = NULL;
				}
				m_sApList = pstApInfo;
			}
			eOplEvent = eSVC_WIFI_SCAN_SUCCESS;
			break;
		case eNETWORK_STATUS_WIFI_SCANNING_FAIL :
			eOplEvent = eSVC_WIFI_SCAN_FAIL;
			break;
		default :
			return ;
	}

	m_fnCallback( eNetService_WiFiScanning, eOplEvent, nIfIndex, 0 );
}

std::list<WiFiConnectService*> WiFiConnectService::m_sInstanceList;
OPL_NETWORKSERVICE_EventCallback *WiFiConnectService::m_fnCallback = NULL;
bool WiFiConnectService::m_bFailed = false;
WiFiConnectService::WiFiConnectService( OPL_NETWORKSERVICE_EventCallback callback ) : NetService( callback )
{
	m_szName = (char *)"WiFiConnection";
	HxSTD_MemSet( m_szSSID, 0x00, sizeof(m_szSSID) );
	HxSTD_MemSet( m_szKey, 0x00, sizeof(m_szKey) );

	m_nSecureType = m_nAuthenType = m_nEncryptType = 0;
	m_nWaveStrength = m_nConnectSpeed = 0;
	HxSTD_MemSet( m_szWave, 0x00, sizeof(m_szWave) );
	HxSTD_MemSet( m_szSpeed, 0x00, sizeof(m_szSpeed) );

	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_RegisterNotifier( WiFiConnectService::NotifyEvent );
	}
	m_sInstanceList.push_back(this);

	m_fnCallback = callback;

#if 1
	{
		HINT32 numOfDev =0, i = 0;
		APKS_NETDEVINFO_t stDevInfo;
		APK_NETWORK_CONF_GetNumOfNetDev( &numOfDev );
		for ( i = 0; i <numOfDev; i++ )
		{
			APK_NETWORK_CONF_GetDevInfo( i, &stDevInfo );
			if ( eINTERFACE_TYPE_WLAN == stDevInfo.eType )
			{
				m_nIfIndex = i;
				if ( this->UpdateApInfo() )
				{
					HxSTD_StrNCpy(m_szTarget, stDevInfo.szDev_name, sizeof(m_szTarget) );
				}
				break;
			}
		}
	}
#endif
}

WiFiConnectService::~WiFiConnectService()
{
	m_sInstanceList.remove(this);
	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_UnRegisterNotifier( WiFiConnectService::NotifyEvent );
	}
}

bool WiFiConnectService::UpdateApInfo()
{
	APKS_NETAP_Info_t stApInfo;
	HERROR result = ERR_FAIL;
	HCHAR	szKey[128];

	result = APK_NETWORK_WIFI_GetConnectedInfo( m_nIfIndex, &stApInfo, szKey, sizeof(szKey) );

	HxSTD_StrNCpy( m_szSSID, stApInfo.szESSID, sizeof(m_szSSID)-1 );
	HxSTD_StrNCpy( m_szKey, szKey, sizeof(m_szKey)-1 );

	m_nSecureType = stApInfo.nSecureType;
	m_nAuthenType = stApInfo.nAuthenType;
	m_nEncryptType = stApInfo.nEncryptType;

	m_nWaveStrength = stApInfo.nWaveStrength;
	m_nConnectSpeed = stApInfo.nConnectSpeed;

	if ( ERR_OK != result )
	{
		return false;
	}

	return true;
}

bool WiFiConnectService::Set( char *szName, char *szValue )
{

	if ( !HxSTD_StrCmp( "ssid", szName  ) )
	{
		HxSTD_StrNCpy( m_szSSID, szValue, sizeof(m_szSSID)-1 );
	}
	else if ( !HxSTD_StrCmp( "network_key", szName  ) )
	{
		HxSTD_StrNCpy( m_szKey, szValue, sizeof(m_szKey)-1 );
	}
	else if ( !HxSTD_StrCmp( "security", szName  ) )
	{
		return setSecurity( szValue );
	}
	else if ( !HxSTD_StrCmp( "authen", szName  ) )
	{
		return setAuthen( szValue );
	}
	else if ( !HxSTD_StrCmp( "encrypt", szName  ) )
	{
		return setEncrypt( szValue );
	}
	else
	{
		return false;
	}
	return true;
}

char * WiFiConnectService::Get( char *szName )
{
	if ( true == m_bFailed )
	{
		std::list<WiFiConnectService *>::iterator it;
		for ( it  = m_sInstanceList.begin(); it != m_sInstanceList.end(); it++ )
		{
			(*it)->UpdateApInfo();
		}
		m_bFailed = false;
	}
	if ( 0 == HxSTD_StrLen( m_szSSID ) )
	{
		return NULL;
	}

	if ( !HxSTD_StrCmp( "ssid", szName  ) )
	{
		return m_szSSID;
	}
	else if ( !HxSTD_StrCmp( "security", szName  ) )
	{
		return getSecurity( m_nSecureType );
	}
	else if ( !HxSTD_StrCmp( "authen", szName  ) )
	{
		return getAuthen( m_nAuthenType );
	}
	else if ( !HxSTD_StrCmp( "encrypt", szName  ) )
	{
		return getEncrypt( m_nEncryptType );
	}
	else if ( !HxSTD_StrCmp( "bandWidth", szName  ) )
	{
	}
	else if ( !HxSTD_StrCmp( "waveStrength", szName  ) )
	{
	}
	else if ( !HxSTD_StrCmp( "network_key", szName  ) )
	{
		return m_szKey;
	}
	return NULL;
}

int	WiFiConnectService::Start()
{
	HERROR result = ERR_FAIL;
	APKS_NETAP_Info_t apInfo;

	HxLOG_Print("(*) [%s,%d] SSID : %s, security(%d), authen(%d), encrypt(%d).\n", __FUNCTION__, __LINE__, m_szSSID, m_nSecureType, m_nAuthenType, m_nEncryptType );
	if ( 0 > m_nSecureType || 0 > m_nAuthenType || 0 > m_nEncryptType )
	{
		return BPL_STATUS_ERROR;
	}
	if ( 0 == HxSTD_StrLen(m_szSSID) )
	{
		return BPL_STATUS_ERROR;
	}
	HxSTD_StrNCpy( apInfo.szESSID, m_szSSID, sizeof(apInfo.szESSID)-1 );
	apInfo.nSecureType = m_nSecureType;
	apInfo.nAuthenType = m_nAuthenType;
	apInfo.nEncryptType = m_nEncryptType;

	result = APK_NETWORK_WIFI_ConnectAP( m_nIfIndex ,&apInfo, m_szKey );
	if ( ERR_OK != result )
	{
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}


bool WiFiConnectService::setSecurity( const char *szValue )
{
	if ( !HxSTD_StrCmp( "OPEN", szValue  ) )
	{
		m_nSecureType = eInet_Wifi_Sec_Open;
	}
	else if ( !HxSTD_StrCmp( "WEP", szValue  ) )
	{
		m_nSecureType = eInet_Wifi_Sec_WEP;
	}
	else if ( !HxSTD_StrCmp( "WPA", szValue  ) )
	{
		m_nSecureType = eInet_Wifi_Sec_WPA;
	}
	else if ( !HxSTD_StrCmp( "WPA2", szValue  ) )
	{
		m_nSecureType = eInet_Wifi_Sec_WPA2;
	}
	else
	{
		m_nSecureType = -1;
		return false;
	}
	return true;
}

bool WiFiConnectService::setAuthen( const char *szValue )
{
	if ( !HxSTD_StrCmp( "NONE", szValue  ) )
	{
		m_nAuthenType = eInet_Wifi_Auth_Open;
	}
	else if ( !HxSTD_StrCmp( "PSK", szValue  ) )
	{
		m_nAuthenType = eInet_Wifi_Auth_WPAPSK;
	}
	else if ( !HxSTD_StrCmp( "802.1x", szValue  ) )
	{
		m_nAuthenType = eInet_Wifi_Auth_802_1x;
	}
	else
	{
		m_nAuthenType = -1;
		return false;
	}
	return true;
}

bool WiFiConnectService::setEncrypt( const char *szValue )
{
	if ( !HxSTD_StrCmp( "NONE", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_None;
	}
	else if ( !HxSTD_StrCmp( "WEP64_ASCII", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_WEP_64_Ascii;
	}
	else if ( !HxSTD_StrCmp( "WEP64_HEX", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_WEP_64_Hex;
	}
	else if ( !HxSTD_StrCmp( "WEP128_ASCII", szValue  ) )
	{
		m_nEncryptType = eInet_Wifi_Encrypt_WEP_128_Ascii;
	}
	else if ( !HxSTD_StrCmp( "WEP128_HEX", szValue  ) )
	{
		m_nEncryptType = eInet_Wifi_Encrypt_WEP_128_Hex;
	}
	else if ( !HxSTD_StrCmp( "TKIP", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_WPA_TKIP;
	}
	else if ( !HxSTD_StrCmp( "AES", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_WPA_AES;
	}
	else if ( !HxSTD_StrCmp( "TKIP/AES", szValue  ) )
	{
		m_nEncryptType = eINet_Wifi_Encrypt_WPA_TKIP_AES;
	}
	else
	{
		m_nEncryptType = -1;
		return false;
	}
	return true;
}

char *	WiFiConnectService::getSecurity( int type )
{
	char *security = NULL;
	switch ( type )
	{
		case eInet_Wifi_Sec_Open :
			security = (char *)"OPEN";
			break;
		case eInet_Wifi_Sec_WEP :
			security = (char *)"WEP";
			break;
		case eInet_Wifi_Sec_WPA :
			security = (char *)"WPA";
			break;
		case eInet_Wifi_Sec_WPA2 :
			security = (char *)"WPA2";
			break;
		case eInet_Wifi_Sec_WPA_WPA2 :
			security = (char *)"WPA/WPA2";
			break;
		default :
			break;
	}
	return security;
}
char *	WiFiConnectService::getAuthen( int type )
{
	char *authen = NULL;
	switch ( type )
	{
		case eInet_Wifi_Auth_Open :
			authen = (char *)"NONE";
			break;
		case eInet_Wifi_Auth_WPAPSK :
			authen = (char *)"PSK";
			break;
		case eInet_Wifi_Auth_802_1x :
			authen = (char *)"802.1x";
			break;
		default :
			break;
	}
	return authen;
}

char *	WiFiConnectService::getEncrypt( int type )
{
	char *encrypt = NULL;
	switch ( type )
	{
		case eINet_Wifi_Encrypt_None :
			encrypt = (char *)"NONE";
			break;
		case eINet_Wifi_Encrypt_WEP_64 :
			encrypt = (char *)"WEP64_ASCII";
			break;
		case eINet_Wifi_Encrypt_WEP_64_Ascii :
			encrypt = (char *)"WEP64_ASCII";
			break;
		case eINet_Wifi_Encrypt_WEP_64_Hex :
			encrypt = (char *)"WEP64_HEX";
			break;
		case eInet_Wifi_Encrypt_WEP_128_Ascii :
			encrypt = (char *)"WEP128_ASCII";
			break;
		case eInet_Wifi_Encrypt_WEP_128_Hex :
			encrypt = (char *)"WEP128_HEX";
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP :
			encrypt = (char *)"TKIP";
			break;
		case eINet_Wifi_Encrypt_WPA_AES :
			encrypt = (char *)"AES";
			break;
		case eINet_Wifi_Encrypt_WPA_TKIP_AES :
			encrypt = (char *)"TKIP/AES";
			break;
		default :
			break;
	}
	return encrypt;
}

char *	WiFiConnectService::getWaveStrength( int nStrength )
{
	return m_szWave;
}

char *	WiFiConnectService::getConnectSpeed( int nSpeed )
{
	return m_szSpeed;
}
void WiFiConnectService::NotifyEvent( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HINT32	nIfIndex = ulParam1 ;
	SvcWiFiConnectEvent_e eOplEvent = eSVC_WIFI_CONNECT_MAX;

	if ( NULL == m_fnCallback )
	{
		return ;
	}

	switch ( eEvent )
	{
		case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS :
			HxLOG_Print("\n=================== eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS =======================\n");
			eOplEvent = eSVC_WIFI_CONNECT_SUCCESS;
			break;
		case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL :
			m_bFailed = true;
			eOplEvent = eSVC_WIFI_CONNECT_FAIL;
			break;
		case eNETWORK_STATUS_WIFI_WPS_SUCCESS :
			m_bFailed = true;
			return;
		default :
			return ;
	}

	m_fnCallback( eNetService_WiFiConnect, eOplEvent, nIfIndex, 0 );
}

std::list<WiFiWPSService*> WiFiWPSService::m_sInstanceList;
OPL_NETWORKSERVICE_EventCallback *WiFiWPSService::m_fnCallback = NULL;
WiFiWPSService::WiFiWPSService( OPL_NETWORKSERVICE_EventCallback callback ) : NetService(callback)
{
	HCHAR szMac[32] = { 0 };
	m_szName = (char *)"WPS";

	m_nConnectType = -1;

	APK_NETWORK_CONF_GetMacAddr( 0, szMac, sizeof(szMac) );
	initPincodeByMac(szMac);

	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_RegisterNotifier( WiFiWPSService::NotifyEvent );
	}
	m_sInstanceList.push_back(this);

	m_fnCallback = callback;
}

WiFiWPSService::~WiFiWPSService()
{
	m_sInstanceList.remove(this);
	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_UnRegisterNotifier( WiFiWPSService::NotifyEvent );
	}
}

bool WiFiWPSService::Set( char *szName, char *szValue )
{
	if ( !HxSTD_StrCmp( "ConnectionType", szName  ) )
	{
		return setConnectType( szValue );
	}
	else
	{
		return false;
	}
	return true;
}

char * WiFiWPSService::Get( char *szName )
{
	if ( !HxSTD_StrCmp( "PinCode", szName  ) )
	{
		return m_szPinCode;
	}
	return NULL;
}

int	WiFiWPSService::Start()
{
	HERROR result = ERR_FAIL;

	HxLOG_Print("(*) WPS - Excute. ConnectionType(%d), PinCode(%s)\n", m_nConnectType, m_szPinCode );
	if ( 0 > m_nConnectType )
	{
		return BPL_STATUS_ERROR;
	}

	result = APK_NETWORK_WIFI_ConnectAPByWPS( m_nIfIndex, (APKE_NETAP_WPS_Type_e)m_nConnectType, m_szPinCode );
	if ( ERR_OK != result )
	{
		HxLOG_Print("(*)WPS - Excute Error(%d)\n", result );
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

bool WiFiWPSService::setConnectType( const char *szValue )
{
	if ( !HxSTD_StrCmp( "ButtonType", szValue  ) )
	{
		m_nConnectType = eNETAP_WPS_PBC;
	}
	else if ( !HxSTD_StrCmp( "PinType", szValue  ) )
	{
		m_nConnectType = eNETAP_WPS_PIN;
	}
	else
	{
		return false;
	}
	return true;
}

bool WiFiWPSService::initPincodeByMac( const char *szMac )
{
	HINT32 p[8] = { 0 };
	HINT32 checkSum = 0;
	HINT32 accum = 0;
	HINT32 i = 0;
	HCHAR tmp[3] = { 0 };
#if 0 // TEMP PinCode
	p[0] = 0; p[1] = 2; p[2] = 8; p[3] = 1; p[4] =4; p[5]=9; p[6]=6;
#else
	HxSTD_StrNCpy(tmp, &szMac[0], 2 );
	p[0] = HLIB_STD_HexToInt(tmp) % 10;
	HxSTD_StrNCpy(tmp, &szMac[3], 2 );
	p[1] = HLIB_STD_HexToInt(tmp) % 10;
	HxSTD_StrNCpy(tmp, &szMac[6], 2 );
	p[2] = HLIB_STD_HexToInt(tmp) % 10;
	HxSTD_StrNCpy(tmp, &szMac[9], 2 );
	p[3] = HLIB_STD_HexToInt(tmp) % 10;
	HxSTD_StrNCpy(tmp, &szMac[12], 2 );
	p[4] = HLIB_STD_HexToInt(tmp) % 10;
	HxSTD_StrNCpy(tmp, &szMac[15], 2 );
	p[5] = HLIB_STD_HexToInt(tmp) % 10;
	p[6] = p[0] * p[5] % 10;
#endif
	for ( i = 0; i < 7 ; i++ )
	{
		if ( (i+1) % 2 )
			accum += p[i] * 3;
		else
			accum += p[i] * 1;
	}

	checkSum = 10 - ( accum  % 10 );
	p[7] = (10 == checkSum) ? 0 : checkSum ;

	HxSTD_MemSet( m_szPinCode, 0x00, sizeof(m_szPinCode) );

	HxSTD_PrintToStrN( m_szPinCode, sizeof(m_szPinCode), "%d%d%d%d%d%d%d%d", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
	return true;
}

void WiFiWPSService::NotifyEvent( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HINT32	nIfIndex = ulParam1 ;
	SvcWiFiConnectEvent_e eOplEvent = eSVC_WIFI_CONNECT_MAX;

	if ( NULL == m_fnCallback )
	{
		return ;
	}
	switch ( eEvent )
	{
		case eNETWORK_STATUS_WIFI_WPS_SUCCESS :
			eOplEvent = eSVC_WIFI_CONNECT_SUCCESS;
			break;
		case eNETWORK_STATUS_WIFI_WPS_FAIL :
			eOplEvent = eSVC_WIFI_CONNECT_FAIL;
			break;
		default :
			return ;
	}
	m_fnCallback( eNetService_WiFiWPS, eOplEvent, nIfIndex, 0 );
}


#define ____DHCPServer_Service_____
std::list<DHCPServerService*> DHCPServerService::m_sInstanceList;
OPL_NETWORKSERVICE_EventCallback *DHCPServerService::m_fnCallback = NULL;
DHCPServerService::DHCPServerService( OPL_NETWORKSERVICE_EventCallback callback ) : NetService(callback)
{
	HBOOL bRunnig;
	m_szName = (char *)"DHCPServer";

	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_RegisterNotifier( DHCPServerService::NotifyEvent );
	}
	m_sInstanceList.push_back(this);

	m_fnCallback = callback;

	APK_NETWORK_DHCPSVR_UpdateInfo();
	APK_NETWORK_DHCPSVR_IsRunning(&bRunnig);
	if ( bRunnig )
	{
		APKS_NETDEVINFO_t stDevInfo;
		APK_NETWORK_DHCPSVR_GetDevIndex(&m_nIfIndex);
		APK_NETWORK_CONF_GetDevInfo( m_nIfIndex, &stDevInfo);
		HxSTD_StrNCpy(m_szTarget, stDevInfo.szDev_name, sizeof(m_szTarget) -1 );
	}
}

DHCPServerService::~DHCPServerService()
{
	m_sInstanceList.remove(this);
	if ( 0 == m_sInstanceList.size() )
	{
		APK_NETWORK_UnRegisterNotifier( DHCPServerService::NotifyEvent );
	}
}

bool DHCPServerService::Set( char *szName, char *szValue )
{
	return false;
}

char * DHCPServerService::Get( char *szName )
{
	return NULL;
}

int	DHCPServerService::Start()
{
	HERROR result = ERR_FAIL;
	result = APK_NETWORK_DHCPSVR_Start(m_nIfIndex);
	if ( ERR_OK != result )
	{
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

int	DHCPServerService::Stop()
{
	HERROR result = ERR_FAIL;
	result = APK_NETWORK_DHCPSVR_Stop(m_nIfIndex);
	if ( ERR_OK != result )
	{
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

void DHCPServerService::ClearTarget()
{
	HxSTD_MemSet( m_szTarget, 0x00, sizeof(m_szTarget) );
}

void DHCPServerService::NotifyEvent( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
//	HINT32	nIfIndex = ulParam1 ;
	std::list<DHCPServerService *>::iterator it;

	if ( NULL == m_fnCallback )
	{
		return ;
	}

	switch ( eEvent )
	{
		case eNETWORK_STATUS_DHCPD_START :
			{
				APKS_NETDEVINFO_t stDevInfo;
				HINT32 nIfIndex = 0;
				APK_NETWORK_DHCPSVR_GetDevIndex(&nIfIndex);
				APK_NETWORK_CONF_GetDevInfo( nIfIndex, &stDevInfo);

				for ( it  = m_sInstanceList.begin(); it != m_sInstanceList.end(); it++ )
				{
					(*it)->SetTarget( stDevInfo.szDev_name );
				}
			}
			break;
		case eNETWORK_STATUS_DHCPD_STOP :
			{
				for ( it  = m_sInstanceList.begin(); it != m_sInstanceList.end(); it++ )
				{
					(*it)->ClearTarget();
				}
			}
			break;
		default :
			return ;
	}

	//m_fnCallback( eNetService_DHCPServer, eOplEvent, nIfIndex, 0 );
}


#define ____NetService_Factory____
NetService * NetServiceFactory::CreateService( NetSvc_e svcType, OPL_NETWORKSERVICE_EventCallback callback )
{
	NetService *service = NULL;
	switch ( svcType )
	{
		case eNetService_WiFiScanning :
			service = new WiFiScanningService( callback );
			break;
		case eNetService_WiFiConnect :
			service = new WiFiConnectService( callback );
			break;
		case eNetService_WiFiWPS :
			service = new WiFiWPSService( callback );
			break;
		case eNetService_DHCPServer :
			service = new DHCPServerService( callback );
		default :
			break;
	}
	return service;
}

/********************************************************************************
 *	OplNetworkService class
 ********************************************************************************
 */

OPL_NETWORKSERVICE_EventCallback * OplNetworkService::s_pfnNetServiceCallback = NULL;
OplNetworkService::OplNetworkService( int nServiceIndex )
{
	m_nSvcIndex = nServiceIndex;
	m_pService  = NetServiceFactory::CreateService( (NetSvc_e)nServiceIndex, notifyEventToCallback );
}

OplNetworkService::~OplNetworkService()
{
	if ( m_pService )
	{
		delete m_pService;
		m_pService = NULL;
	}
}

int	OplNetworkService::getNumOfSupportService()
{
	return eNetService_MAX;
}

bool OplNetworkService::registerEventListener( OPL_NETWORKSERVICE_EventCallback callbackFunc )
{
	s_pfnNetServiceCallback = callbackFunc;

	return true;
}
bool OplNetworkService::unregisterEventListener( OPL_NETWORKSERVICE_EventCallback callbackFunc )
{
	s_pfnNetServiceCallback = NULL;

	return true;
}

void OplNetworkService::notifyEventToCallback( int nSvcIndex, int nEvent, int nParam1, int nParam2 )
{
	if ( s_pfnNetServiceCallback )
	{
		s_pfnNetServiceCallback( nSvcIndex, nEvent, nParam1, nParam2);
	}
}

char * OplNetworkService::getServiceName()
{
	if ( !m_pService)
	{
		return NULL;
	}
	return m_pService->GetName();
}

char * OplNetworkService::getTargetName()
{
	if ( !m_pService)
	{
		return NULL;
	}
	return m_pService->GetTarget();
}

bool OplNetworkService::setTargetName(char *p)
{
	if ( !m_pService)
	{
		return false;
	}
	return m_pService->SetTarget( p );
}

int OplNetworkService::getTargetIndex()
{
	if ( !m_pService )
	{
		return -1;
	}
	return m_pService->GetIfIndex();
}

bool OplNetworkService::setCmd(char *szCmd, char *szParam)
{
	if ( !m_pService )
	{
		return false;
	}
	return m_pService->Set( szCmd, szParam );
}

char *OplNetworkService::getCmd(char *szCmd)
{
	if ( !m_pService)
	{
		return NULL;
	}
	return m_pService->Get( szCmd );
}

int OplNetworkService::restart()
{
	if ( !m_pService )
	{
		return 0;
	}
	return m_pService->Start();
}

int OplNetworkService::stop()
{
	if ( !m_pService )
	{
		return 0;
	}
	return m_pService->Stop();
}
#endif
