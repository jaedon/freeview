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
 * @file	  		nx_port_network.c
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

#include <nx_port.h>

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
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HERROR  NX_PORT_NETWORK_GetNumOfNetDev(HINT32 *pnCount)
{
    return APK_NETWORK_CONF_GetNumOfNetDev(pnCount);
}


HERROR  NX_PORT_NETWORK_GetDevInfo(HINT32 nIndex, NX_NET_NetDevInfo_t *pstDevInfo)
{
    APKS_NETDEVINFO_t *pstApkDevInfo = (APKS_NETDEVINFO_t *)pstDevInfo;

    return APK_NETWORK_CONF_GetDevInfo(nIndex, pstApkDevInfo);
}


HERROR  NX_PORT_NETWORK_GetMacAddr(HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize)
{
    return APK_NETWORK_CONF_GetMacAddr(nIndex, pszMacAddr, nBufSize);
}


HERROR  NX_PORT_NETWORK_GetBluetoothMacAddr(HCHAR *szMacAddr)
{
    return APK_NETWORK_BT_GetMacAddrInfo(szMacAddr);
}

HERROR  NX_PORT_NETWORK_GetAddrInfo(HINT32 nIndex,NX_NET_NetIFADDR_t *pstNetAddr)
{
    APKS_NETIFADDR_t *pstApkNetInfo = (APKS_NETIFADDR_t *)pstNetAddr;
    return APK_NETWORK_CONF_GetAddrInfo( nIndex, pstApkNetInfo);
}

HERROR  NX_PORT_NETWORK_SetAddrInfo(HINT32 nIndex,NX_NET_NetIFADDR_t *pstNetAddr)
{
    APKS_NETIFADDR_t *pstApkNetInfo = (APKS_NETIFADDR_t *)pstNetAddr;
    return APK_NETWORK_CONF_SetAddrInfo( nIndex, pstApkNetInfo);
}

static void NX_PORT_NETWORK_EventListner(HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
    APKS_NETDEVINFO_t stApkDevInfo;
    HINT32	nIfIndex = ulParam1 ;

    APK_NETWORK_CONF_GetDevInfo(nIfIndex, &stApkDevInfo);
    switch(eEvent)
    {
        case eNETWORK_STATUS_WIFI_SCANNING_END:
            {
				HINT32 nApCount = ulParam3;
				APKS_NETAP_Info_t *apList = (APKS_NETAP_Info_t *)ulParam2;
                ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eEvent, (HUINT32)apList, nApCount);
            }
            break;
        case eNETWORK_STATUS_WIFI_SCANNING_FAIL:
            ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eEvent, 0, 0);
            break;
        default:
        	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_NETWORK, eEvent, stApkDevInfo.eType, 0);
            break;
    }
}

void    NX_PORT_NETWORK_RegisterNotifier(void)
{
    APK_NETWORK_RegisterNotifier(NX_PORT_NETWORK_EventListner);
}

void    NX_PORT_NETWORK_UnRegisterNotifier(void)
{
    APK_NETWORK_UnRegisterNotifier(NX_PORT_NETWORK_EventListner);
}

HERROR    NX_PORT_NETWORK_StartScan(HINT32 nIndex)
{
    return APK_NETWORK_WIFI_StartScan(nIndex);
}

HERROR    NX_PORT_NETWORK_ConnectAP( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey )
{
    return APK_NETWORK_WIFI_ConnectAP(nIndex , pstApInfo , szKey);
}

HERROR    NX_PORT_NETWORK_CheckGateway(HINT32 nIndex)
{
	APK_NETWORK_CONCHK_Gateway( nIndex, NX_PORT_NETWORK_EventListner, 5000 );
    return ERR_OK;
}

HERROR    NX_PORT_NETWORK_CheckDNS(HINT32 nIndex)
{
    APK_NETWORK_CONCHK_Dns( nIndex, NX_PORT_NETWORK_EventListner, 0 );
    return ERR_OK;
}




/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

