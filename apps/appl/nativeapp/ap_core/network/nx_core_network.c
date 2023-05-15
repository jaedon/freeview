/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_network.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_network.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

HERROR  nx_network_GetNetDevIndex(NX_NET_NetIfType_e netIfType,HINT32 *netIndex)
{
    HERROR              hError = ERR_FAIL;
    HINT32              nNetDevCount = 0,nIndex = 0;
    NX_NET_NetDevInfo_t stDevInfo;

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);

    if (ERR_OK != hError)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (ERR_OK != hError)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == netIfType)
        {
            *netIndex = nIndex;
            return ERR_OK;
        }
    }
    return ERR_FAIL;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HERROR  NX_NETWORK_GetNetDevInfo(NX_NET_NetDevInfo_t **pstNetDevInfo, HINT32* nNetDevCount)
{
    HERROR              hError = ERR_FAIL;
    HINT32              nIndex = 0,nCount =0;
    NX_NET_NetDevInfo_t *stDevInfo;
    NX_NET_NetDevInfo_t *stDevInfoList;

    if(NULL != *pstNetDevInfo)
    {
        return ERR_FAIL;
    }

	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
        nCount = 1;
        stDevInfoList = (NX_NET_NetDevInfo_t*)NX_APP_Malloc(sizeof(NX_NET_NetDevInfo_t)*1);
        HxSTD_memset(stDevInfoList, 0x00, sizeof(NX_NET_NetDevInfo_t)*nCount);
        stDevInfoList[0].bConnected = TRUE;
        stDevInfoList[0].eType = eNxNet_INTERFACE_TYPE_LAN;

        *pstNetDevInfo = stDevInfoList;
        *nNetDevCount = nCount;
        return ERR_OK;
    #endif


    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nCount);

    if (ERR_OK != hError)
    {
        return ERR_FAIL;
    }

    stDevInfoList = (NX_NET_NetDevInfo_t*)NX_APP_Malloc(sizeof(NX_NET_NetDevInfo_t)*nCount);
    HxSTD_memset(stDevInfoList, 0x00, sizeof(NX_NET_NetDevInfo_t)*nCount);

    for (nIndex = 0; nIndex < nCount; nIndex++)
    {
        stDevInfo = &(stDevInfoList[nIndex]);
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, stDevInfo);

        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }
    }

    *pstNetDevInfo = stDevInfoList;
    *nNetDevCount = nCount;

    return ERR_OK;
}

HERROR  NX_NETWORK_SetLANNetInterface(NX_NET_NetIFADDR_t *pstNetAddr)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;

	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
        ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_STATUS_CON_SUCCESS, eNxNet_INTERFACE_TYPE_LAN, 0);
        return ERR_OK;
    #endif


    hError = nx_network_GetNetDevIndex(eNxNet_INTERFACE_TYPE_LAN,&netIndex);

    if(ERR_OK != hError )
    {
        return hError;
    }

    hError = NX_PORT_NETWORK_SetAddrInfo(netIndex, pstNetAddr);
    return hError;
}

HERROR  NX_NETWORK_EnableNetInterface(NX_NET_NetIfType_e netIfType)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;
    NX_NET_NetIFADDR_t    ptNetAddr = {0, };

	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
        ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_STATUS_CON_SUCCESS, netIfType, 0);
        return ERR_OK;
    #endif


    hError = nx_network_GetNetDevIndex(netIfType,&netIndex);

    if(ERR_OK != hError )
    {
        return hError;
    }

    hError = NX_PORT_NETWORK_GetAddrInfo(netIndex,&ptNetAddr);
    if(ERR_OK != hError )
    {
        return hError;
    }

    //check Dhcp ON - For NFS Setting

    ptNetAddr.bDhcpOn = TRUE;
	ptNetAddr.bDnsAuto = TRUE;
    hError = NX_PORT_NETWORK_SetAddrInfo(netIndex, &ptNetAddr );
    return hError;
}

HERROR  NX_NETWORK_GetLANConnected(HBOOL *bLANConnected)
{
    HINT32              nNetDevCount = 0,nIndex = 0;
    HERROR              hError = ERR_FAIL;
    NX_NET_NetDevInfo_t stDevInfo;

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    *bLANConnected = TRUE;
    return ERR_OK;
#endif

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);

    if (hError != ERR_OK)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        HxSTD_MemSet(&stDevInfo, 0x00, sizeof(NX_NET_NetDevInfo_t));
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == eNxNet_INTERFACE_TYPE_LAN)
        {
            *bLANConnected = stDevInfo.bConnected;
            return ERR_OK;
        }
    }

    return ERR_FAIL;
}

void NX_NETWORK_RegisterNotifier(void)
{
    NX_PORT_NETWORK_RegisterNotifier();
}

void NX_NETWORK_UnRegisterNotifier(void)
{
    NX_PORT_NETWORK_UnRegisterNotifier();
}

HERROR  NX_NETWORK_CheckGateway(NX_NET_NetIfType_e netIfType)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_CONCHK_ARP_SUCCESS, 0, 0);
    return ERR_OK;
#endif

    hError = nx_network_GetNetDevIndex(netIfType,&netIndex);
    if(ERR_OK != hError)
    {
        return hError;
    }

    return  NX_PORT_NETWORK_CheckGateway(netIndex);
}

HERROR  NX_NETWORK_CheckDNS(NX_NET_NetIfType_e netIfType)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_CONCHK_RESOLV_SUCCESS, 0, 0);
    return ERR_OK;
#endif

    hError = nx_network_GetNetDevIndex(netIfType,&netIndex);
    if(ERR_OK != hError)
    {
        return hError;
    }

    return  NX_PORT_NETWORK_CheckDNS(netIndex);
}




HERROR  NX_NETWORK_GetWLanInterface(NX_NET_NetIFADDR_t** pstNetIf)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;
    NX_NET_NetIFADDR_t  *pstTemp = NULL;
#if defined(CONFIG_PROD_OS_EMULATOR)
    HUINT8*             p;
#endif

    if(NULL != *pstNetIf)
    {
        return ERR_FAIL;
    }

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    pstTemp = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
    HxSTD_MemSet(pstTemp, 0x00, sizeof(NX_NET_NetIFADDR_t));
    pstTemp->bDhcpOn = TRUE;
    pstTemp->bDnsAuto= TRUE;
    pstTemp->bDns6 = FALSE;
    pstTemp->ulIpAddr = (HULONG)1000;
    pstTemp->ulNetmask = (HULONG)1000;
    pstTemp->ulGateway = (HULONG)1000;
    pstTemp->ulDns1 = (HULONG)1000;
    pstTemp->ulDns2 = (HULONG)1000;
    p = (HUINT8*)&(pstTemp->ulNetmask);
    HxLOG_Error("\n\n#### NX_NETWORK_GetLanInterface : : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]);
    *pstNetIf = pstTemp;
    return ERR_OK;
#endif

    hError = nx_network_GetNetDevIndex(eNxNet_INTERFACE_TYPE_WLAN,&netIndex);

    if(ERR_OK != hError )
    {
        return hError;
    }

    pstTemp = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
    HxSTD_MemSet(pstTemp, 0x00, sizeof(NX_NET_NetIFADDR_t));

    hError = NX_PORT_NETWORK_GetAddrInfo(netIndex,pstTemp);
    if(hError!=ERR_OK)
    {
        NX_APP_Free(pstTemp);
        return hError;
    }
    *pstNetIf = pstTemp;
    return ERR_OK;
}

HERROR  NX_NETWORK_GetLanInterface(NX_NET_NetIFADDR_t** pstNetIf)
{
    HERROR              hError = ERR_FAIL;
    HINT32              netIndex = 0;
    NX_NET_NetIFADDR_t  *pstTemp = NULL;
#if defined(CONFIG_PROD_OS_EMULATOR)
    HUINT8*             p;
#endif

    if(NULL != *pstNetIf)
    {
        return ERR_FAIL;
    }

#if defined(CONFIG_PROD_OS_EMULATOR)
    pstTemp = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
    HxSTD_MemSet(pstTemp, 0x00, sizeof(NX_NET_NetIFADDR_t));
    pstTemp->bDhcpOn = TRUE;
    pstTemp->bDnsAuto= TRUE;
    pstTemp->bDns6 = FALSE;
    pstTemp->ulIpAddr = (HULONG)1000;
    pstTemp->ulNetmask = (HULONG)1000;
    pstTemp->ulGateway = (HULONG)1000;
    pstTemp->ulDns1 = (HULONG)1000;
    pstTemp->ulDns2 = (HULONG)1000;
    p = (HUINT8*)&(pstTemp->ulNetmask);
    HxLOG_Error("\n\n#### NX_NETWORK_GetLanInterface : : %ld.%ld.%ld.%ld \n", p[0], p[1], p[2], p[3]);
    *pstNetIf = pstTemp;
    return ERR_OK;
#endif


    hError = nx_network_GetNetDevIndex(eNxNet_INTERFACE_TYPE_LAN,&netIndex);

    if(ERR_OK != hError )
    {
        return hError;
    }

    pstTemp = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
    HxSTD_MemSet(pstTemp, 0x00, sizeof(NX_NET_NetIFADDR_t));

    hError = NX_PORT_NETWORK_GetAddrInfo(netIndex,pstTemp);
    if(hError!=ERR_OK)
    {
        NX_APP_Free(pstTemp);
        return hError;
    }
    *pstNetIf = pstTemp;
    return ERR_OK;
}

#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
//TEST EMUL Data
#define AP_EMUL_DATA_LIST     11
static APKS_NETAP_Info_t apList[AP_EMUL_DATA_LIST]={
        {"TEST1","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_Open,eInet_Wifi_Auth_Open,100,100,FALSE},
        {"TEST2","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST3","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST4","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST5","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST6","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST7","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST8","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST9","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE},
        {"TEST10","34-E6-D7-26-78-E7",eInet_Wlan_Dev_Auto,eInet_Wifi_Sec_WEP,eInet_Wifi_Auth_WPA2PSK,100,100,FALSE}
};
#endif


HERROR  NX_NETWORK_ConnectAP(APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey )
{
    HINT32              nNetDevCount = 0;
    NX_NET_NetDevInfo_t stDevInfo;
    HINT32              nIndex = 0;
    HERROR              hError = ERR_FAIL;
	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
    HERROR              bUpdated = FALSE;
        for (nIndex = 0; nIndex < AP_EMUL_DATA_LIST; nIndex++)
        {
             if(HxSTD_StrCmp(apList[nIndex].szESSID, pstApInfo->szESSID) == 0)
             {
                apList[nIndex].bIsConnected = TRUE;
                bUpdated = TRUE;
             }
             else
             {
                apList[nIndex].bIsConnected = FALSE;
             }
        }
        if(TRUE == bUpdated)
        {
            ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS, 0, 0);
        }
        else
        {
            ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL, 0, 0);
        }
        return ERR_OK;
    #endif

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);
    if (hError != ERR_OK)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == eNxNet_INTERFACE_TYPE_WLAN)
        {
            return NX_PORT_NETWORK_ConnectAP(nIndex, pstApInfo, szKey);
        }
    }

    return ERR_FAIL;
}


HERROR  NX_NETWORK_StartScan(void)
{
    HINT32              nNetDevCount = 0;
    NX_NET_NetDevInfo_t stDevInfo;
    HINT32              nIndex = 0;
    HERROR              hError = ERR_FAIL;

	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
        ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eNETWORK_STATUS_WIFI_SCANNING_END, (HINT32)apList, 10);
        return ERR_OK;
    #endif

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);
    if (hError != ERR_OK)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == eNxNet_INTERFACE_TYPE_WLAN)
        {
            return NX_PORT_NETWORK_StartScan(nIndex);
        }
    }
    return ERR_FAIL;
}


HERROR  NX_NETWORK_GetLANMacAddr(HCHAR *pszMacAddr, HINT32 nMax)
{
    HERROR              hError = ERR_FAIL;
    HINT32              nNetDevCount = 0,nIndex = 0;
    NX_NET_NetDevInfo_t stDevInfo;

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);
    if (hError != ERR_OK)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        HxSTD_memset(&stDevInfo, 0x00, sizeof(NX_NET_NetDevInfo_t));
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == eNxNet_INTERFACE_TYPE_LAN)
        {
            hError = NX_PORT_NETWORK_GetMacAddr(nIndex, pszMacAddr, nMax);
            if (hError != ERR_OK)
            {
                return ERR_FAIL;
            }

            return ERR_OK;
        }
    }

    return ERR_FAIL;
}


HERROR  NX_NETWORK_GetWLANMacAddr(HCHAR *pszMacAddr, HINT32 nMax)
{
    HERROR              hError = ERR_FAIL;
    HINT32              nNetDevCount = 0,nIndex = 0;
    NX_NET_NetDevInfo_t stDevInfo;

    hError = NX_PORT_NETWORK_GetNumOfNetDev(&nNetDevCount);
    if (hError != ERR_OK)
    {
        return ERR_FAIL;
    }

    for (nIndex = 0; nIndex < nNetDevCount; nIndex++)
    {
        HxSTD_memset(&stDevInfo, 0x00, sizeof(NX_NET_NetDevInfo_t));
        hError = NX_PORT_NETWORK_GetDevInfo(nIndex, &stDevInfo);
        if (hError != ERR_OK)
        {
            return ERR_FAIL;
        }

        if (stDevInfo.eType == eNxNet_INTERFACE_TYPE_WLAN)
        {
            hError = NX_PORT_NETWORK_GetMacAddr(nIndex, pszMacAddr, nMax);
            if (hError != ERR_OK)
            {
                return ERR_FAIL;
            }

            return ERR_OK;
        }
    }

    return ERR_FAIL;
}


HERROR  NX_NETWORK_GetBluetoothMacAddr(HCHAR *pszMacAddr)
{
    return NX_PORT_NETWORK_GetBluetoothMacAddr(pszMacAddr);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

