/***************************************************************************
 *    (c)2010-2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: netapp_p2p.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/28/11 2:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_p2p.c $
 *
 * 1   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 *
 * dev_main_SW7425-749/1   6/28/11 2:24p steven
 * Check in initial P2P support
 *
 ***************************************************************************/
/**
 * @brief NetApp WiFi Direct (P2P) Private API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by netapp.c
 *
 * @remarks This API MUST NOT be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 *TODO: Persistent GO support
 *TODO:
 */
#include "netapp_priv.h"
#include "netapp_p2p.h"
#include "BcmP2PAPI.h"
#include "netapp_dhcp.h"
#include "netapp_dhcpcd.h"

#define NETAPP_P2P_CONNECT_TIME                 60
#define NETAPP_KEEP_ALIVE_MAX_PING_FAILURES     3
#define NETAPP_KEEP_ALIVE_PING_TIMEOUT          1000
#define NETAPP_KEEP_ALIVE_PING_INTERVAL         3000    /* msec */

#define NETAPP_P2P_GW_ADDR(address) ((address & 0x00FFFFFF) + (1 << 24))

#define NETAPP_BYTE_SWAP(value) (                                               \
    ((value >> 24) & 0x000000ff) | ((value >>  8) & 0x0000ff00) |               \
    ((value <<  8) & 0x00ff0000) | ((value << 24) & 0xff000000) )

#define BCMP2P_API_CHECK(api, tRetCode)                                         \
{                                                                               \
    NETAPP_MSG(("%s() Calling:[%s]", __FUNCTION__, #api));                      \
    BCMP2P_STATUS result = api;                                                 \
    switch(result)                                                              \
    {                                                                           \
    case BCMP2P_SUCCESS:                                                        \
        NETAPP_MSG(("%s() [%s] Returned Successful", __FUNCTION__, #api));      \
        tRetCode = NETAPP_SUCCESS;                                              \
        break;                                                                  \
    case BCMP2P_INVALID_PARAMS:                                                 \
        tRetCode = NETAPP_INVALID_PARAMETER;                                    \
        goto err_out;                                                           \
        break;                                                                  \
    case BCMP2P_NOT_ENOUGH_SPACE:                                               \
        tRetCode = NETAPP_OUT_OF_MEMORY;                                        \
        goto err_out;                                                           \
        break;                                                                  \
    case BCMP2P_UNIMPLEMENTED:                                                  \
        tRetCode = NETAPP_NOT_SUPPORTED;                                        \
        goto err_out;                                                           \
        break;                                                                  \
    default:                                                                    \
        tRetCode = NETAPP_FAILURE;                                              \
        NETAPP_ERR(("%s() [%s] failed!", __FUNCTION__, #api));                  \
        goto err_out;                                                           \
        break;                                                                  \
    }                                                                           \
}


#define NETAPP_P2P_PRIV_API_CHECK(handle)                                       \
{                                                                               \
    if ( (handle == NULL) || (handle->hP2P == NULL) )                           \
    {                                                                           \
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));                  \
        return NETAPP_NULL_PTR;                                                 \
    }                                                                           \
}

static const char *BCMP2P_STATUS_STRING[] = BCMP2P_STATUS_STR_TABLE;


struct sNETAPP_P2P
{

    NETAPP_DEVICE_TYPE      tDeviceType;
    char                    *pDevName;
    char                    cWiFiIfaceName[NETAPP_IFACE_NAME_LEN+1];
    char                    cIfaceName[NETAPP_IFACE_NAME_LEN+1];

    BCMP2PHandle            hP2P;
    BCMP2P_CONFIG           tP2PConfig;
    NETAPP_DEVICE_TYPE      tType;
    BCMP2P_DISCOVER_ENTRY   tPeerList[64 /* TODO; Change to dynamic list */];
    BCMP2P_DISCOVER_ENTRY   tProvisioningPeer;
    uint32_t                ulCurrentPeer;
    uint32_t                ulPeerCount;
    uint32_t                ulDiscoveryCount;
    NETAPP_LINK_STATE       tLinkState;
    NETAPP_PRIV_HANDLE      hNetAppPriv;
    uint32_t                tConnectRetryTimer;
    uint32_t                tAutoDiscoverTimer;
    uint32_t                tKeepAliveTimer;
    uint8_t                 ucPingFailureCount;
    uint32_t                ulRetryCount;
    bool                    bRetryingConnect;
    NETAPP_DHCP_HANDLE      hNetAppDhcp;    /* DHCP Server Handle */
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd;  /* DHCP Client Daemon Handle */
    bool                    bAutoP2PDiscover;

    NETAPP_IPV4_ADDR        tPrimaryDNS;                 //!< Primary DNS Address
    NETAPP_IPV4_ADDR        tSecondaryDNS;               //!< Secondary DNS Address
};


static void NetAppWiFiP2P_P_Callback(
    BCMP2P_NOTIFICATION_CODE    tCode,
    void                        *pContext,
    void                        *pData,
    int                         lDataLength);


static void NetAppWiFiP2P_P_GetPeerInfo(
    NETAPP_P2P_HANDLE           hNetAppP2P);


static void NetAppWiFiP2P_P_ReportConnectResults(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    NETAPP_RETCODE              tRetCode,
    uint32_t                    ulData0);


static void NetAppWiFiP2P_P_StartDhcp(
    NETAPP_P2P_HANDLE           hNetAppP2P);


static void NetAppWiFiP2P_P_RetryTimer(
    void                    *pParam);


static void NetAppWiFiP2P_P_AutoDiscoverTimer(
    void                    *pParam);


static void NetAppWiFiP2P_P_KeepAliveTimer(
    void                    *pParam);


static bool NetAppWiFiP2P_P_ProcRXInviteReq(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    void                    *pNotificationData);


static bool NetAppWiFiP2P_P_ProcRXInviteRes(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    void                    *pData);


static void NetAppWiFiP2P_P_DhcpCallback(
    void                    *pParam,
    NETAPP_CB_TYPE          tCbType,
    const void              *pvBuffer,
    uint32_t                ulData0,
    NETAPP_RETCODE          tResult,
    NETAPP_IFACE            tIFace);


NETAPP_RETCODE NetAppWiFiP2P_P_Disconnect(
    NETAPP_P2P_HANDLE       hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateInit(
    NETAPP_P2P_HANDLE           *phNetAppP2P,
    NETAPP_PRIV_HANDLE          hNetAppPriv,
    NETAPP_INIT_SETTINGS        *pInitSettings,
    NETAPP_SETTINGS             *pSettings,
    NETAPP_DEVICE_TYPE          tType,
    NETAPP_DHCPCD_HANDLE        hNetAppDhcpcd)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_P2P_HANDLE   hNetAppP2P = NULL;

    NETAPP_PRIV_HANDLE_CHECK(phNetAppP2P);
    NETAPP_PRIV_HANDLE_CHECK(pInitSettings);
    NETAPP_PRIV_HANDLE_CHECK(pSettings);

    hNetAppP2P = (NETAPP_P2P_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_P2P));
    if (hNetAppP2P == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppP2P, 0, sizeof(struct sNETAPP_P2P));

    hNetAppP2P->pDevName        = strdup(pInitSettings->pDeviceName);
    hNetAppP2P->tType           = tType;
    hNetAppP2P->tLinkState      = NETAPP_LINK_DOWN;
    hNetAppP2P->hNetAppPriv     = hNetAppPriv;
    hNetAppP2P->ulCurrentPeer   = -1;
    hNetAppP2P->hNetAppDhcpcd = hNetAppDhcpcd;

    BCMP2PLogEnable(BCMP2P_LOG_ERR);
    //BCMP2PLogEnable(BCMP2P_LOG_MED);
    BCMP2P_API_CHECK(BCMP2PInitialize(BRCMP2P_VERSION, NULL), tRetCode);

    BCMP2P_API_CHECK(BCMP2PRegisterNotification(BCMP2P_NOTIFY_ALL,
        NetAppWiFiP2P_P_Callback, hNetAppP2P, NULL), tRetCode);

    BCMP2P_INIT_BCMP2P_CONFIG(&hNetAppP2P->tP2PConfig);

    NETAPP_PRIV_API_CHECK(NetAppOSTimerCreate(&hNetAppP2P->tConnectRetryTimer,
        NetAppWiFiP2P_P_RetryTimer, hNetAppP2P), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppOSTimerCreate(&hNetAppP2P->tAutoDiscoverTimer,
            NetAppWiFiP2P_P_AutoDiscoverTimer, hNetAppP2P), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppOSTimerCreate(&hNetAppP2P->tKeepAliveTimer,
            NetAppWiFiP2P_P_KeepAliveTimer, hNetAppP2P), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppDhcpInit(&hNetAppP2P->hNetAppDhcp), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateSettingsRefresh(hNetAppP2P), tRetCode);

    *phNetAppP2P = hNetAppP2P;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateDeInit(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    if (hNetAppP2P == NULL)
    {
        NETAPP_ERR(("%s(): Line %d, Invalid Argument!", __FUNCTION__, __LINE__));
        return NETAPP_NULL_PTR;
    }

    if (hNetAppP2P->hNetAppDhcp != NULL)
    {
        NetAppDhcpDeInit(hNetAppP2P->hNetAppDhcp);
        hNetAppP2P->hNetAppDhcp = NULL;
    }

    if (hNetAppP2P->tConnectRetryTimer != 0)
    {
        NetAppOSTimerDelete(hNetAppP2P->tConnectRetryTimer);
    }

    if (hNetAppP2P->tAutoDiscoverTimer != 0)
    {
        NetAppOSTimerDelete(hNetAppP2P->tAutoDiscoverTimer);
    }

    if (hNetAppP2P->tKeepAliveTimer != 0)
    {
        NetAppOSTimerDelete(hNetAppP2P->tKeepAliveTimer);
    }

    if (hNetAppP2P->hP2P != NULL)
    {
        NetAppWiFiP2PPrivateHotPlug(hNetAppP2P, hNetAppP2P->cWiFiIfaceName,
            sizeof(hNetAppP2P->cWiFiIfaceName), false);
    }

    BCMP2P_API_CHECK(BCMP2PUnRegisterNotification(), tRetCode);
    BCMP2P_API_CHECK(BCMP2PUninitialize(), tRetCode);

    if (hNetAppP2P->pDevName != NULL)
    {
        free(hNetAppP2P->pDevName);
    }

    NetAppOSMemFree(hNetAppP2P);
    hNetAppP2P = NULL;

err_out:
    return tRetCode;
}



NETAPP_RETCODE NetAppWiFiP2PPrivateHotPlug(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    const char              *pIFaceName,
    uint32_t                ulIfaceLength,
    bool                    bInserted)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    if ( bInserted && (hNetAppP2P->hP2P == NULL) )
    {
        strncpy(hNetAppP2P->cWiFiIfaceName, pIFaceName, sizeof(hNetAppP2P->cWiFiIfaceName)/sizeof(hNetAppP2P->cWiFiIfaceName[0])-1);

        if ((hNetAppP2P->hP2P = BCMP2POpen(hNetAppP2P->cWiFiIfaceName, hNetAppP2P->cWiFiIfaceName)) == NULL)
        {
            NETAPP_ERR(("%s(): P2P API Create Failure", __FUNCTION__));
            return NETAPP_FAILURE;
        }

        NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateSettingsRefresh(hNetAppP2P), tRetCode);
    }


    else if ( !bInserted && (hNetAppP2P->hP2P != NULL) )
    {
        if (BCMP2PIsDiscovering(hNetAppP2P->hP2P))
        {
            BCMP2PCancelDiscover(hNetAppP2P->hP2P);
        }

        NetAppWiFiP2PPrivateDisconnect(hNetAppP2P);

        BCMP2P_API_CHECK(BCMP2PClose(hNetAppP2P->hP2P), tRetCode);
        hNetAppP2P->hP2P = NULL;
        memset(hNetAppP2P->cIfaceName, 0, sizeof(hNetAppP2P->cIfaceName));
        memset(hNetAppP2P->cWiFiIfaceName, 0, sizeof(hNetAppP2P->cWiFiIfaceName));
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateDiscover(
    NETAPP_P2P_HANDLE           hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    BCMP2P_DISCOVER_PARAM   tParams;
    NETAPP_SETTINGS         tSettings;

    BCMP2P_CHANNEL          tSocialChannel = { BCMP2P_LISTEN_CHANNEL_CLASS, 11 };

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);

    memset(&tParams, 0, sizeof(BCMP2P_DISCOVER_PARAM));

    NetAppPrivateGetSettings(hNetAppP2P->hNetAppPriv, &tSettings);

    tParams.social_timeout  = tSettings.tDefP2PParams.lTimeoutSec;
    tParams.scan_interval   = tSettings.tDefP2PParams.lScanTimeMs;
    tParams.services        = tSettings.tDefP2PParams.ulServices; // TODO: Convert this!
    tParams.ssidLength      = strlen(hNetAppP2P->pDevName);
    tParams.socialChannel   = tSocialChannel;
    tParams.reqDevType      = 0;    /* Take anyone!!! */
    tParams.reqDevSubCat    = 0;

    memcpy(tParams.ssid, hNetAppP2P->pDevName, sizeof(tParams.ssid));

    BCMP2P_API_CHECK(BCMP2PDiscover(hNetAppP2P->hP2P, &tParams), tRetCode);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateStopDiscovery(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);
    BCMP2P_API_CHECK(BCMP2PCancelDiscover(hNetAppP2P->hP2P), tRetCode);
    hNetAppP2P->ulDiscoveryCount = 0;
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateConnect(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    char                    *pName,
    uint32_t                ulTimeoutSec)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    int                     i;
    BCMP2P_STATUS           tResult = BCMP2P_SUCCESS;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);
    NETAPP_PRIV_HANDLE_CHECK(pName);

    for (i = 0; i < sizeof(hNetAppP2P->tPeerList)/sizeof(hNetAppP2P->tPeerList[0]); i++)
    {
        if ( (hNetAppP2P->tPeerList[i].ssid[0] != '\0') &&
             (strcmp((char*)hNetAppP2P->tPeerList[i].ssid, pName) == 0) )
        {
            break;
        }
    }
    if (i >= sizeof(hNetAppP2P->tPeerList)/sizeof(hNetAppP2P->tPeerList[0]))
    {
        NETAPP_ERR(("%s(): %s not found!", __FUNCTION__, pName));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    if (hNetAppP2P->tLinkState != NETAPP_LINK_DOWN)
    {
        NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateDisconnect(hNetAppP2P), tRetCode);
    }
    hNetAppP2P->ulCurrentPeer = i;
    if (BCMP2PSendProvisionDiscoveryRequest(hNetAppP2P->hP2P, BCMP2P_WPS_PUSHBUTTON,
            hNetAppP2P->tPeerList[i].is_p2p_group, hNetAppP2P->pDevName, strlen(hNetAppP2P->pDevName),
            &hNetAppP2P->tPeerList[i].channel, (BCMP2P_ETHER_ADDR *)hNetAppP2P->tPeerList[i].mac_address) != BCMP2P_SUCCESS)
    {
        NETAPP_MSG(("%s(): BCMP2PSendProvisionDiscoveryRequest Failed %d!!", __FUNCTION__, tResult));
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateDisconnect(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);
    if (hNetAppP2P->tLinkState != NETAPP_LINK_DOWN)
    {
        hNetAppP2P->tLinkState = NETAPP_LINK_DOWN;
        NETAPP_PRIV_API_CHECK(NetAppWiFiP2P_P_Disconnect(hNetAppP2P), tRetCode);
    }

    if (hNetAppP2P->bRetryingConnect)
    {
        NetAppOSTimerStop(hNetAppP2P->tConnectRetryTimer);
        hNetAppP2P->bRetryingConnect = false;
    }
    hNetAppP2P->ulCurrentPeer = -1;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateGetSSID(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    char                    *pBuf,
    uint32_t                ulBufSize)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);

    if (ulBufSize < BCMP2P_MAX_SSID_LEN)
    {
        NETAPP_ERR(("%s(): Buffer size too small", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if (hNetAppP2P->tLinkState != NETAPP_LINK_DOWN)
    {
        BCMP2P_API_CHECK(BCMP2PGetGOCredentials(hNetAppP2P->hP2P, (BCMP2P_UINT8*)pBuf, NULL, NULL), tRetCode);
    }
err_out:
    return tRetCode;
}


NETAPP_LINK_STATE NetAppWiFiP2PPrivateGetLinkState(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    return ((hNetAppP2P != NULL) ? hNetAppP2P->tLinkState : NETAPP_LINK_DOWN);
}


char* NetAppWiFiP2PPrivateGetIfaceName(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    return ((hNetAppP2P != NULL) ? hNetAppP2P->cIfaceName : NETAPP_LINK_DOWN);
}


NETAPP_RETCODE NetAppWiFiP2PPrivateSetNetworkSettings(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    NETAPP_IP_MODE              tMode,
    NETAPP_IP_SETTINGS          *pSettings)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;
    NETAPP_IP_SETTINGS      tSettings;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);

    memset(&tSettings, 0, sizeof(tSettings));

    BCMP2P_API_CHECK(NetAppPrivateSetInterfaceUp(hNetAppP2P->hNetAppPriv,
        hNetAppP2P->cIfaceName, (tMode != NETAPP_IP_MODE_OFF)), tRetCode);


    switch (tMode)
    {
    /* TODO: Add DHCP Client/server support */
    case NETAPP_IP_MODE_DYNAMIC:
    case NETAPP_IP_MODE_STATIC:
        if (BCMP2PIsSTA(hNetAppP2P->hP2P))
        {
            BCMP2P_IP_ADDR tIpAddr = (hNetAppP2P->tP2PConfig.ip_addr & 0xffffff00);
            tIpAddr |= 202;  // Give it a .128
            tSettings.tIpAddress    = NETAPP_BYTE_SWAP(tIpAddr);
        }
        else
        {
            tSettings.tIpAddress    = NETAPP_BYTE_SWAP(hNetAppP2P->tP2PConfig.ip_addr);
        }

        tSettings.tSubnetMask   = NETAPP_BYTE_SWAP(hNetAppP2P->tP2PConfig.netmask);
        tSettings.tGateway      = NETAPP_BYTE_SWAP(hNetAppP2P->tP2PConfig.ip_addr);

        BCMP2P_API_CHECK(NetAppPrivateSetNetworkSettings(hNetAppP2P->hNetAppPriv,
            hNetAppP2P->cIfaceName, NETAPP_IFACE_P2P, NETAPP_IP_MODE_STATIC, &tSettings), tRetCode);

        hNetAppP2P->tLinkState = NETAPP_LINK_UP;
        break;
    case NETAPP_IP_MODE_OFF:
        if (hNetAppP2P->tLinkState != NETAPP_LINK_DOWN)
        {
            NetAppWiFiP2PPrivateDisconnect(hNetAppP2P);
        }
        hNetAppP2P->tLinkState = NETAPP_LINK_DOWN;
    default:
        NETAPP_ERR(("%s(): Buffer size too small", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateSettingsRefresh(
    NETAPP_P2P_HANDLE           hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;
    NETAPP_SETTINGS         tSettings;
    BCMP2P_CHANNEL          tSocialChannel = { BCMP2P_LISTEN_CHANNEL_CLASS, 11 };
    BCMP2P_CHANNEL          tOpperatingChannel = { IEEE_5GHZ_40MHZ_CLASS_22, 36 };

    NETAPP_PRIV_HANDLE_CHECK(hNetAppP2P);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppP2P->hNetAppPriv, &tSettings), tRetCode);

    NetAppWiFiP2PPrivateDisconnect(hNetAppP2P);

    hNetAppP2P->bAutoP2PDiscover = tSettings.bAutoP2PDiscover;

    switch(tSettings.tSoftApSettings.tApInfo.tSecurity)
    {
    case NETAPP_WIFI_SECURITY_WEP:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_NONE;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_WEP128;
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_AES:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_WPAPSK;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_AES;
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_WPAPSK;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_TKIP;
        break;

    case NETAPP_WIFI_SECURITY_WPA2_PSK_AES:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_WPA2PSK;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_AES;
        break;
    case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_WPA2PSK;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_TKIP;
        break;

    case NETAPP_WIFI_SECURITY_INVALID:
    case NETAPP_WIFI_SECURITY_NONE:
    case NETAPP_WIFI_SECURITY_NOT_SUPPORTED:
    default:
        hNetAppP2P->tP2PConfig.authentication   = BCMP2P_WPA_AUTH_NONE;
        hNetAppP2P->tP2PConfig.encryption       = BCMP2P_ALGO_OFF;
        break;
    }




    snprintf(hNetAppP2P->tP2PConfig.WPSConfig.wpsPin, sizeof(hNetAppP2P->tP2PConfig.WPSConfig.wpsPin), tSettings.tSoftApSettings.tApInfo.cPassword);
    strncpy((char*)hNetAppP2P->tP2PConfig.keyWPA, "Steven Hartley",
        sizeof(hNetAppP2P->tP2PConfig.keyWPA)/sizeof(hNetAppP2P->tP2PConfig.keyWPA[0]) - 1);

    switch (tSettings.tSoftApSettings.tApInfo.tChanBandwidth)
    {
    case NETAPP_WIFI_BANDWIDTH_20MHz:
        if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 11)
        {
            tOpperatingChannel.channel_class = IEEE_2GHZ_20MHZ_CLASS_12;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 48)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_20MHZ_CLASS_1;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 64)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_20MHZ_CLASS_2_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 140)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_20MHZ_CLASS_4_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 161)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_20MHZ_CLASS_3;
        }
        else
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_20MHZ_CLASS_5;
        }
        break;

    case NETAPP_WIFI_BANDWIDTH_40MHz:
        if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 7)
        {
            tOpperatingChannel.channel_class = IEEE_2GHZ_40MHZ_CLASS_32;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 11)
        {
            tOpperatingChannel.channel_class = IEEE_2GHZ_40MHZ_CLASS_33;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 44)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_22;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 48)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_27;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 60)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_23_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 64)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_28_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 132)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_24_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 136)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_29_DFS;
        }
        else if (tSettings.tSoftApSettings.tApInfo.ulChannel <= 157)
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_25;
        }
        else
        {
            tOpperatingChannel.channel_class = IEEE_5GHZ_40MHZ_CLASS_30;
        }
        break;

    default:
        break;
    }

    tOpperatingChannel.channel                          = tSettings.tSoftApSettings.tApInfo.ulChannel;
    hNetAppP2P->tP2PConfig.ip_addr                      = htonl(tSettings.tSoftApSettings.tIpAddress);
    hNetAppP2P->tP2PConfig.netmask                      = htonl(tSettings.tSoftApSettings.tSubnetMask);
    hNetAppP2P->tP2PConfig.disableP2P                   = (tSettings.bIsSoftAp);
    hNetAppP2P->tP2PConfig.WPSConfig.wpsEnable          = true;
    hNetAppP2P->tP2PConfig.WPSConfig.wpsConfigMethods   = BCMP2P_WPS_PUSHBUTTON; // TODO: Support others later
    hNetAppP2P->tP2PConfig.operatingChannel             = tOpperatingChannel;
    hNetAppP2P->tP2PConfig.primaryDevType               = BCMP2P_DEVICE_TYPE_CAT_DISPLAYS;
    hNetAppP2P->tP2PConfig.primaryDevSubCat             = BCMP2P_DEVICE_TYPE_SUB_CAT_DISP_TV;
    hNetAppP2P->tP2PConfig.DHCPConfig.DHCPOption        = BCMP2P_DHCP_OFF;
    hNetAppP2P->tP2PConfig.grp_owner_intent             = tSettings.ucP2PGOIntent;

    NETAPP_MSG(("%s(): Settings Operating Channel to: %d class: %d",
            __FUNCTION__, tOpperatingChannel.channel, tOpperatingChannel.channel_class));

    /* If P2P is running then lets initialize it now */
    if (hNetAppP2P->hP2P != NULL)
    {
        if (tSettings.bIsSoftAp)
        {
            BCMP2P_API_CHECK(BCMP2PCreateSoftAP(hNetAppP2P->hP2P, hNetAppP2P->pDevName), tRetCode);
        }
        else
        {
            BCMP2P_API_CHECK(BCMP2PSetLinkConfig(hNetAppP2P->hP2P, &hNetAppP2P->tP2PConfig, hNetAppP2P->pDevName), tRetCode);
            /* End of BCMP2P_CONFIG setting */

            BCMP2P_API_CHECK(BCMP2PEnableIntraBss(hNetAppP2P->hP2P,         true), tRetCode);
            BCMP2P_API_CHECK(BCMP2PEnableConcurrent(hNetAppP2P->hP2P,       true), tRetCode);
            BCMP2P_API_CHECK(BCMP2PEnableInvitation(hNetAppP2P->hP2P,       true), tRetCode);
            BCMP2P_API_CHECK(BCMP2PEnableServiceDiscovery(hNetAppP2P->hP2P, true), tRetCode);
            BCMP2P_API_CHECK(BCMP2PEnableClientDiscovery(hNetAppP2P->hP2P,  true), tRetCode);

            if (hNetAppP2P->bAutoP2PDiscover)
            {
                NetAppWiFiP2PPrivateDiscover(hNetAppP2P);
            }
        }
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PPrivateWpsPushButton(
    NETAPP_P2P_HANDLE           hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);
    BCMP2P_API_CHECK(BCMP2PPushButton(hNetAppP2P->hP2P), tRetCode);

err_out:
    return tRetCode;
}

/*******************************************************************************
 *                       Static NetApp P2P Functions
 ******************************************************************************/
#define NETAPP_P2P_NOTIF_CASE(value)                                             \
    case value:                                                                  \
        NETAPP_MSG(("%s(): Received: %s", __FUNCTION__, #value));


#define NETAPP_P2P_NOTIF_CASE_BREAK(value)                                      \
    NETAPP_P2P_NOTIF_CASE(value)                                                \
    break;


static bool NetAppWiFiP2P_P_ProcRXInviteReq(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    void                    *pData)
{
    BCMP2P_BOOL status = BCMP2P_FALSE;
    BCMP2P_INVITE_PARAM invite_req;
    BCMP2P_INVITE_RESPONSE response = BCMP2P_INVITE_REJECT;
    BCMP2P_PERSISTENT persist;
    BCMP2P_BOOL isReinvoke = BCMP2P_FALSE;
    BCMP2P_BOOL isGo = BCMP2P_FALSE;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppP2P);
    NETAPP_PRIV_HANDLE_CHECK(pData);

    memcpy(&invite_req, pData, sizeof(invite_req));

    NETAPP_MSG(("%s(): ssid=%s,len=%d ch=%d:%d flags=%d", __FUNCTION__, invite_req.groupSsid, invite_req.groupSsidLength,
        invite_req.operatingChannel.channel_class, invite_req.operatingChannel.channel, invite_req.inviteFlags));

    if (BCMP2PIsSTA(hNetAppP2P->hP2P))
    {
        NETAPP_MSG(("rejecting invite - already a STA"));
        response = BCMP2P_INVITE_REJECT;
    }
    else if (BCMP2PIsGroupOwner(hNetAppP2P->hP2P))
    {
        NETAPP_MSG(("rejecting invite - already a GO\n"));
        response = BCMP2P_INVITE_REJECT;
    }
    else if (invite_req.inviteFlags & 0x1)
    {
        /* TODO:
        if (p2papp_enable_persistent &&
            p2papp_persist_find_ssid((char *)invite_req.groupSsid,
            &persist) != 0)
        {
            isReinvoke = true;
            if (persist.is_go)
            {
                p2papi_log(BCMP2P_LOG_MED, true,
                    "accept invite - reinvoke GO\n");
                response = BCMP2P_INVITE_ACCEPT;
                isGo = true;
            }
            else
            {
                p2papi_log(BCMP2P_LOG_MED, true,
                    "accept invite - reinvoke STA\n");
                response = BCMP2P_INVITE_ACCEPT;
                isGo = FALSE;
            }
        }
        else
            */
        {
            NETAPP_MSG(("%s(): reject invite - unknown group\n", __FUNCTION__));
            response = BCMP2P_INVITE_REJECT_UNKNOWN_GROUP;
        }
    }
    else {
        if (memcmp(&invite_req.groupBssid, NETAPP_NULL_HW_ADDR, sizeof(NETAPP_NULL_HW_ADDR)) == 0)
        {
            NETAPP_MSG(("%s(): reject invite - no group BSSID\n", __FUNCTION__));
            response = BCMP2P_INVITE_REJECT_UNKNOWN_GROUP;
        }
        else
        {
            NETAPP_MSG(("%s(): accept invite - join existing group\n", __FUNCTION__));
            response = BCMP2P_INVITE_ACCEPT;
        }
    }

    /* send invitation response before processing invite */
    BCMP2PSendInviteResponse(hNetAppP2P->hP2P, &invite_req, response, isGo);

    NETAPP_MSG(("%s Invitation Request from %02x:%02x:%02x:%02x:%02x:%02x",
        response == BCMP2P_INVITE_ACCEPT ? "Accepted" : "Rejected",
        invite_req.srcDevAddr.octet[0], invite_req.srcDevAddr.octet[1],
        invite_req.srcDevAddr.octet[2], invite_req.srcDevAddr.octet[3],
        invite_req.srcDevAddr.octet[4], invite_req.srcDevAddr.octet[5]));

    if (response == BCMP2P_INVITE_ACCEPT)
    {
        /* delay to allow tx of invite response before processing invite */
        NetAppOSTaskDelayMsec(500);

        if (isReinvoke)
        {
            if (isGo)
            {
#if 0
                p2papp_redraw("....... Reinvoking persistent GO .......",
                    p2papp_msg_buf2, "");
                BCMP2PUpdateWPAKey(p2papp_dev_hdl,
                    (char *)persist.pmk,
                    (char *)persist.passphrase);
                BCMP2PCreateGroup(p2papp_dev_hdl,
                    invite_req.groupSsid, true);
#endif
            }
            else
            {
                NETAPP_MSG(("%s(): wait for GO's config timeout %u ms\n", invite_req.goConfigTimeoutMs));
                NetAppOSTaskDelayMsec(invite_req.goConfigTimeoutMs);
                NETAPP_MSG(("%s(): Reinvoking persistent client", __FUNCTION__));

                BCMP2PJoinGroupWithCredentials(hNetAppP2P->hP2P, &invite_req.groupDevAddr, &invite_req.operatingChannel,
                    invite_req.groupSsid, invite_req.groupSsidLength, &invite_req.groupBssid,persist.pmk, NETAPP_P2P_CONNECT_TIME);
            }
        }
        else
        {
            /* receiving invite to join an existing group requires
             * invited peer to send provision discovery which requires
             * user input so join with WPS should not be invoked until
             * after user input
             * however, sigma requires and checks for provision so this
             * code is only to satisify sigma
             * proper app implmentation should invoke provision discovery,
             * wait for user input, then initiate connect back to peer
             */
            NETAPP_MSG(("%s(): wait for GO's config timeout %u ms\n", __FUNCTION__, invite_req.goConfigTimeoutMs));

            NetAppOSTaskDelayMsec(invite_req.goConfigTimeoutMs);

            BCMP2PSendProvisionDiscoveryRequest(hNetAppP2P->hP2P, BCMP2P_WPS_PUSHBUTTON, true, hNetAppP2P->pDevName,
                strlen(hNetAppP2P->pDevName), &invite_req.operatingChannel, &invite_req.groupDevAddr);

            BCMP2PCreateLinkToDevAddr(hNetAppP2P->hP2P, &invite_req.groupDevAddr, &invite_req.operatingChannel,
                true, &invite_req.groupBssid, BCMP2P_CONNECT_TMO_SECS);
        }
        status = BCMP2P_TRUE;
    }
    return status;
}

static bool NetAppWiFiP2P_P_ProcRXInviteRes(
    NETAPP_P2P_HANDLE       hNetAppP2P,
    void                    *pData)
{
    BCMP2P_BOOL         status = BCMP2P_FALSE;
    BCMP2P_INVITE_PARAM invite_rsp;
    char                *status_str;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppP2P);
    NETAPP_PRIV_HANDLE_CHECK(pData);

    memcpy(&invite_rsp, pData, sizeof(invite_rsp));

    if (invite_rsp.status == 1)
        status_str = "passed up";
    else if (invite_rsp.status == 0)
        status_str = "accepted";
    else
        status_str = "rejected";


    NETAPP_MSG(("%s(): Received Invite Responce (%s) from %02x:%02x:%02x:%02x:%02x:%02x", __FUNCTION__, status_str,
        invite_rsp.srcDevAddr.octet[0], invite_rsp.srcDevAddr.octet[1], invite_rsp.srcDevAddr.octet[2],
        invite_rsp.srcDevAddr.octet[3], invite_rsp.srcDevAddr.octet[4], invite_rsp.srcDevAddr.octet[5]));

    if (invite_rsp.status == 0)
    {
        if (memcmp(&invite_rsp.groupBssid, NETAPP_NULL_HW_ADDR, sizeof(NETAPP_NULL_HW_ADDR)) != 0)
        {
#if 0
            BCMP2P_PERSISTENT persist;
            if (p2papp_enable_persistent &&
                p2papp_persist_find_addr(&invite_rsp.srcDevAddr,
                &persist) != 0)
            {
                p2papi_log(BCMP2P_LOG_MED, true,
                    "wait for GO's config timeout %u ms\n",
                    invite_rsp.goConfigTimeoutMs);
                    p2papp_delay(invite_rsp.goConfigTimeoutMs);
                p2papp_redraw("....... Reinvoking persistent client .......",
                    p2papp_msg_buf2, "");
                BCMP2PJoinGroupWithCredentials(p2papp_dev_hdl,
                    &invite_rsp.srcDevAddr,
                    &invite_rsp.operatingChannel,
                    persist.ssid,
                    strlen((const char *)persist.ssid),
                    &invite_rsp.groupBssid,
                    persist.pmk,
                    NETAPP_P2P_CONNECT_TIME);
            }
#else
            NETAPP_MSG(("%s(): Persistent GO not supported Yet!", __FUNCTION__));
#endif
        }
        status = BCMP2P_TRUE;
    }
    return status;
}


static void NetAppWiFiP2P_P_Callback(
    BCMP2P_NOTIFICATION_CODE    tCode,
    void                        *pContext,
    void                        *pData,
    int                         lDataLength)
{
    NETAPP_P2P_HANDLE hNetAppP2P = (NETAPP_P2P_HANDLE)pContext;


    switch (tCode)
    {

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_START_80211_SCAN)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_START_SEARCH_LISTEN)
    //Too noisy: NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_SEARCH_LISTEN_ITERATION)


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_DISCOVER_FOUND_P2P_GROUPS)
        NetAppWiFiP2P_P_GetPeerInfo(hNetAppP2P);
        break;


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_DISCOVER_FOUND_PEERS)
        NetAppWiFiP2P_P_GetPeerInfo(hNetAppP2P);
        break;


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_CANCEL)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_FAIL)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_COMPLETE)

    /* If we suspend and we do not complete a connection then set restart discovery */
    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_DISCOVER_SUSPENDED)
        if (hNetAppP2P->bAutoP2PDiscover)
        {
            NetAppOSTimerSetMsec(hNetAppP2P->tAutoDiscoverTimer, NETAPP_P2P_CONNECT_TIME * 1000);
        }
        break;

    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_DISCOVER_RESUMED)
        if (hNetAppP2P->bAutoP2PDiscover)
        {
            NetAppOSTimerStop(hNetAppP2P->tAutoDiscoverTimer);
        }
        break;
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DISCOVER_START_LISTEN_ONLY)

    case BCMP2P_NOTIF_P2P_INVITE_REQ:
        NETAPP_MSG(("%s(): BCMP2P_NOTIF_P2P_INVITE_REQ", __FUNCTION__));

        if (!NetAppWiFiP2P_P_ProcRXInviteReq(hNetAppP2P, pData))
        {
            NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE, 0);
        }
        break;

    case BCMP2P_NOTIF_P2P_INVITE_RSP:
        NETAPP_MSG(("%s(): BCMP2P_NOTIF_P2P_INVITE_REQ", __FUNCTION__));
        if (!NetAppWiFiP2P_P_ProcRXInviteRes(hNetAppP2P, pData))
        {
            NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE, 0);
        }
        break;


    // Provision Discovery
    case BCMP2P_NOTIF_PROVISION_DISCOVERY_REQUEST:
    case BCMP2P_NOTIF_PROVISION_DISCOVERY_RESPONSE:
        memcpy(&hNetAppP2P->tProvisioningPeer, pData, sizeof(hNetAppP2P->tProvisioningPeer));

        NETAPP_MSG(("%s(): BCMP2P_NOTIF_PROVISION_DISCOVERY_%s from %s", __FUNCTION__,
            (tCode == BCMP2P_NOTIF_PROVISION_DISCOVERY_REQUEST) ? "REQUEST" : "RESPONSE", hNetAppP2P->tProvisioningPeer.ssid));

        if (tCode == BCMP2P_NOTIF_PROVISION_DISCOVERY_REQUEST)
        {
            /* send response before configuring PIN else peer may timeout waiting for response */
            BCMP2PSendProvisionDiscoveryResponse(hNetAppP2P->hP2P,
                hNetAppP2P->tProvisioningPeer.wps_cfg_methods & BCMP2PGetSupportedWpsConfigMethods(hNetAppP2P->hP2P));
        }
        //TODO: Generate a pin and display it
        if (hNetAppP2P->tProvisioningPeer.wps_cfg_methods == BCMP2P_WPS_DISPLAY)
        {
            NETAPP_ERR(("%s():WPS PIN Not supported Yet!", __FUNCTION__));
        }

        // TODO: Prompt user to enter the pin from the peer device
        else if (hNetAppP2P->tProvisioningPeer.wps_cfg_methods == BCMP2P_WPS_KEYPAD)
        {
            NETAPP_ERR(("%s():WPS PIN Not supported Yet!", __FUNCTION__));
        }

        // TODO: Generate a pin and ask the user to enter it on the peerdevice
        else if (hNetAppP2P->tProvisioningPeer.wps_cfg_methods == BCMP2P_WPS_LABEL)
        {
            char *pin = BCMP2PGetWPSPin(hNetAppP2P->hP2P);
            BCMP2PSetWPSPin(hNetAppP2P->hP2P, pin);
            NETAPP_ERR(("%s():WPS PIN Not supported Yet!", __FUNCTION__));
        }

        // Push Button
        else if (hNetAppP2P->tProvisioningPeer.wps_cfg_methods == BCMP2P_WPS_PUSHBUTTON)
        {
            NETAPP_MSG(("%s(): Starting WPS Push Button!", __FUNCTION__));
            BCMP2PPushButton(hNetAppP2P->hP2P);
        }

        //TODO; Check if the provisioned peer is the one we are trying to connect too
        if ( (tCode == BCMP2P_NOTIF_PROVISION_DISCOVERY_RESPONSE) && (hNetAppP2P->ulCurrentPeer != -1) )
        {
            hNetAppP2P->ulRetryCount = 0;
            NetAppOSTimerSetMsec(hNetAppP2P->tConnectRetryTimer, 1000);
        }
        break;


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_PROVISION_DISCOVERY_TIMEOUT)
       /* Provision discovery is only manditory for existing GO's so if the request
        * times out, we still want to try
        * and connect to do not report a connection failure.
        NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE);
        */
        BCMP2PSelectWpsConfigMethod(hNetAppP2P->hP2P, BCMP2P_WPS_PUSHBUTTON);
        BCMP2PPushButton(hNetAppP2P->hP2P);
        hNetAppP2P->ulRetryCount = 0;
        NetAppOSTimerSetMsec(hNetAppP2P->tConnectRetryTimer, 1000);
        break;


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_P2P_PRESENCE_REQ)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_P2P_PRESENCE_RSP)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_START)


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_FAIL)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_PRIMARY_IF_DISCONNECTED)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DEV_DISCOVERABILITY_REQ)

    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_GO_DISCOVERABILITY_REQ)
        // TODO: Do we automatically start another discovery request??
        break;

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DEV_DISCOVERABILITY_RSP)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_CREATE_LINK_START)


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_CREATE_LINK_CANCEL)
        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, NETAPP_LINK_DOWN, NETAPP_SUCCESS, NETAPP_IFACE_P2P);
        hNetAppP2P->tLinkState = NETAPP_LINK_DOWN;
        if (hNetAppP2P->bAutoP2PDiscover)
        {
            NetAppWiFiP2PPrivateDiscover(hNetAppP2P);
        }
        break;

    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_CREATE_LINK_FAIL)
        if (hNetAppP2P->bRetryingConnect)
        {
            NetAppOSTimerSetMsec(hNetAppP2P->tConnectRetryTimer, 1000);
        }
        else
        {
            if (hNetAppP2P->tLinkState != NETAPP_LINK_DOWN)
            {
                NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, NETAPP_LINK_DOWN, NETAPP_SUCCESS, NETAPP_IFACE_P2P);
            }
            else
            {
                NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE, 0);
            }
            hNetAppP2P->tLinkState = NETAPP_LINK_DOWN;
        }
        break;


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_CREATE_LINK_TIMEOUT)
        NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_TIMEOUT, 0);
        break;


    /* CONNECTED!!!! */
    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_CREATE_LINK_COMPLETE)
        NetAppWiFiP2P_P_StartDhcp(hNetAppP2P);
        break;


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_CREATE_LINK_AUTH_FAIL)
        NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE, 0);
        break;


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SOFTAP_START)

    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_SOFTAP_READY)
        NetAppWiFiP2P_P_StartDhcp(hNetAppP2P);
        break;

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SOFTAP_STOP)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SOFTAP_FAIL)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DHCP_START)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_DHCP_STOP)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SOFTAP_STA_ASSOC)


    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_SOFTAP_STA_DISASSOC)
        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, NETAPP_LINK_DOWN, NETAPP_SUCCESS, NETAPP_IFACE_P2P);
        if (hNetAppP2P->bAutoP2PDiscover)
        {
            NetAppWiFiP2PPrivateDiscover(hNetAppP2P);
        }
        break;


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SVC_REQ_RECEIVED)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SVC_RESP_RECEIVED)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SVC_COMEBACK_REQ_RECEIVED)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_SVC_COMEBACK_RESP_RECEIVED)

    /* WPS status */
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_START)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_SCANNING)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_SCANNING_OVER)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_ASSOCIATING)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_ASSOCIATED)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_WPS_MSG_EXCHANGE)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_STATUS_DISCONNECTING)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_PROTOCOL_FAIL)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_FAIL)


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_WRONG_PIN)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_TIMEOUT)

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_SESSION_OVERLAP)

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_WPS_COMPLETE)


    /* Group Owner Negotiation Successful/Finished  */
    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_COMPLETE)
        if (hNetAppP2P->bRetryingConnect)
        {
            NetAppOSTimerStop(hNetAppP2P->tConnectRetryTimer);
            hNetAppP2P->bRetryingConnect = false;
        }
        BCMP2PProcessIncomingConnection(hNetAppP2P->hP2P, NETAPP_P2P_CONNECT_TIME);
        break;


    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_AP_ACK)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_STA_ACK)
    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_NO_PIN)

    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_NO_PROV_INFO)
        BCMP2PPushButton(hNetAppP2P->hP2P);
        break;

    NETAPP_P2P_NOTIF_CASE_BREAK(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_FAIL_INTENT)


    /* The other side was not ready yet, re-try to connect */
    NETAPP_P2P_NOTIF_CASE(BCMP2P_NOTIF_GROUP_OWNER_NEGOTIATION_INFO_UNAVAIL)
        if (hNetAppP2P->ulRetryCount++ > NETAPP_P2P_CONNECT_TIME)
        {
            NetAppWiFiP2P_P_ReportConnectResults(hNetAppP2P, NETAPP_FAILURE, 0);
        }
        else
        {
            hNetAppP2P->bRetryingConnect = true;
        }
        NetAppWiFiP2P_P_Disconnect(hNetAppP2P);
        break;


     default:
         //NETAPP_ERR(("%s(): Unhandled event %d", __FUNCTION__, tCode));
        break;
    }

}


static void NetAppWiFiP2P_P_GetPeerInfo(
    NETAPP_P2P_HANDLE   hNetAppP2P)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    int             i;

    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return;
    }

    BCMP2P_API_CHECK(BCMP2PGetDiscoverResult(hNetAppP2P->hP2P, false, hNetAppP2P->tPeerList,
        sizeof(hNetAppP2P->tPeerList), &hNetAppP2P->ulPeerCount), tRetCode);

    for (i = 0; i < hNetAppP2P->ulPeerCount; i++)
    {
        NETAPP_P2P_PEER_INFO tPeerInfo;

        memset(&tPeerInfo, 0, sizeof(NETAPP_P2P_PEER_INFO));

        strncpy(tPeerInfo.tInfo.cSSID, (char*)hNetAppP2P->tPeerList[i].ssid,
            sizeof(tPeerInfo.tInfo.cSSID)/sizeof(tPeerInfo.tInfo.cSSID[0]) - 1);
        strncpy(tPeerInfo.tInfo.cBSSID, ether_ntoa((struct ether_addr *)hNetAppP2P->tPeerList[i].mac_address),
            sizeof(tPeerInfo.tInfo.cBSSID)/sizeof(tPeerInfo.tInfo.cBSSID[0])-1);
        tPeerInfo.tInfo.ulChannel = hNetAppP2P->tPeerList[i].channel.channel;

        NETAPP_DO_CALLBACK(NETAPP_CB_P2P_PEER, &tPeerInfo, sizeof(tPeerInfo),
            hNetAppP2P->ulDiscoveryCount, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
    }

    hNetAppP2P->ulDiscoveryCount++;
err_out:
    return;
}


static void NetAppWiFiP2P_P_ReportConnectResults(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    NETAPP_RETCODE              tRetCode,
    uint32_t                    ulData0)
{
    int                     i;
    NETAPP_P2P_PEER_INFO    tPeerInfo;

    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return;
    }

    NETAPP_MSG(("%s(): Entered", __FUNCTION__));

    memset(&tPeerInfo, 0, sizeof(tPeerInfo));

    /* TODO: For now hardcode this information */
    tPeerInfo.ulRtspPort    = 8554;     //!< RTSP port number found in WiFi
    tPeerInfo.ulRtpPort     = 6500;      //!< RTP port number
    tPeerInfo.ulVideoPid    = 4113;     //!< Video PID
    tPeerInfo.ulAudioPid    = 4352;     //!< Audio PID

    /* When we are GO, we get an IP address from the dhcp server, otherwise when we
     * are the client, we use dhcpcd and are given an IP address so the P2P device is the gateway */
    if ( (ulData0 != 0) && (BCMP2PIsAP(hNetAppP2P->hP2P) || BCMP2PIsSoftAPOn(hNetAppP2P->hP2P) ) )
    {
        tPeerInfo.tIpAddress    = ulData0;
    }
    else
    {
        NETAPP_IP_SETTINGS  tSettings;
        if (NetAppPrivateGetIPSettings(hNetAppP2P->hNetAppPriv,
                BCMP2PGetNetifName(hNetAppP2P->hP2P), &tSettings) == NETAPP_SUCCESS)
        {
            tPeerInfo.tIpAddress = NETAPP_P2P_GW_ADDR(tSettings.tIpAddress);
        }
        hNetAppP2P->ucPingFailureCount = 0;
        //NetAppOSTimerSetRepeatMsec(hNetAppP2P->tKeepAliveTimer, NETAPP_KEEP_ALIVE_PING_INTERVAL);
    }

    strncpy(tPeerInfo.tInfo.cSSID, (char*)hNetAppP2P->tPeerList[hNetAppP2P->ulCurrentPeer].ssid,
        sizeof(tPeerInfo.tInfo.cSSID)/sizeof(tPeerInfo.tInfo.cSSID[0]) - 1);
    strncpy(tPeerInfo.tInfo.cBSSID, ether_ntoa((struct ether_addr *)hNetAppP2P->tPeerList[hNetAppP2P->ulCurrentPeer].mac_address),
        sizeof(tPeerInfo.tInfo.cBSSID)/sizeof(tPeerInfo.tInfo.cBSSID[0])-1);

    tPeerInfo.bIsGO         = !BCMP2PIsAP(hNetAppP2P->hP2P);
    hNetAppP2P->tLinkState  = (tRetCode == NETAPP_SUCCESS) ? NETAPP_LINK_UP : NETAPP_LINK_DOWN;

    hNetAppP2P->bRetryingConnect = false;
    hNetAppP2P->ulCurrentPeer    = -1;

    NETAPP_DO_CALLBACK(NETAPP_CB_P2P_CONNECT, &tPeerInfo, sizeof(tPeerInfo), 0, tRetCode, NETAPP_IFACE_P2P);
    NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppP2P->tLinkState, tRetCode, NETAPP_IFACE_P2P);


    if (hNetAppP2P->bAutoP2PDiscover)
    {
        if (tRetCode == NETAPP_SUCCESS)
        {
            NetAppOSTimerStop(hNetAppP2P->tAutoDiscoverTimer);
            NetAppWiFiP2PPrivateStopDiscovery(hNetAppP2P);
        }
        else
        {
            NetAppWiFiP2PPrivateDiscover(hNetAppP2P);
        }
    }

err_out:
    return;
}

static void NetAppWiFiP2P_P_StartDhcp(
    NETAPP_P2P_HANDLE           hNetAppP2P)
{
    int                 i;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_IP_SETTINGS  tIpSettings;

    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return;
    }

    hNetAppP2P->tLinkState = NETAPP_LINK_ACQUIRING;
    strncpy (hNetAppP2P->cIfaceName, BCMP2PGetNetifName(hNetAppP2P->hP2P),
        sizeof(hNetAppP2P->cIfaceName)/sizeof(hNetAppP2P->cIfaceName[0]) -1);


    /* We need to set the IP settings if we are a SoftAP */
    if ( BCMP2PIsSoftAPOn(hNetAppP2P->hP2P))
    {
        memset(&tIpSettings, 0, sizeof(tIpSettings));
        tIpSettings.tIpAddress  = htonl(hNetAppP2P->tP2PConfig.ip_addr);
        tIpSettings.tSubnetMask = htonl(hNetAppP2P->tP2PConfig.netmask);

        NETAPP_PRIV_API_CHECK(NetAppPrivateSetNetworkSettings(hNetAppP2P->hNetAppPriv,
            BCMP2PGetNetifName(hNetAppP2P->hP2P), NETAPP_IFACE_P2P, NETAPP_IP_MODE_STATIC, &tIpSettings), tRetCode);
    }

    /* OK Now we start DHCP if we are GO or a SotAP, otherwise we start DHCPCD */
    if ( BCMP2PIsAP(hNetAppP2P->hP2P) || BCMP2PIsSoftAPOn(hNetAppP2P->hP2P) )
    {
        NETAPP_PRIV_API_CHECK(NetAppPrivateGetIPSettings(hNetAppP2P->hNetAppPriv,
            BCMP2PGetNetifName(hNetAppP2P->hP2P), &tIpSettings), tRetCode);

        NETAPP_PRIV_API_CHECK(NetAppDhcpStart(hNetAppP2P->hNetAppDhcp,
            BCMP2PGetNetifName(hNetAppP2P->hP2P), NETAPP_IFACE_P2P, tIpSettings,
            NetAppWiFiP2P_P_DhcpCallback, hNetAppP2P), tRetCode);
    }
    else
    {
        NETAPP_PRIV_API_CHECK(NetAppPrivateGetDns(hNetAppP2P->hNetAppPriv,
            &hNetAppP2P->tPrimaryDNS, &hNetAppP2P->tSecondaryDNS), tRetCode);

        NetAppDhcpcdStart(hNetAppP2P->hNetAppDhcpcd, NETAPP_IP_MODE_DYNAMIC,
            BCMP2PGetNetifName(hNetAppP2P->hP2P), NETAPP_IFACE_P2P,
            NetAppWiFiP2P_P_DhcpCallback, hNetAppP2P, true);
    }

    if (hNetAppP2P->bAutoP2PDiscover)
    {
        NetAppOSTimerSetMsec(hNetAppP2P->tAutoDiscoverTimer, NETAPP_P2P_CONNECT_TIME * 1000);
    }
err_out:
    return;
}


static void NetAppWiFiP2P_P_RetryTimer(
    void            *pParam)
{
    NETAPP_P2P_HANDLE hNetAppP2P = (NETAPP_P2P_HANDLE)pParam;
    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    if (!BCMP2PIsConnecting(hNetAppP2P->hP2P))
    {
        NETAPP_MSG(("%s(): Calling BCMP2PCreateLinkToDevAddr", __FUNCTION__));
        BCMP2PCreateLink(hNetAppP2P->hP2P, &hNetAppP2P->tPeerList[hNetAppP2P->ulCurrentPeer], NETAPP_P2P_CONNECT_TIME);
    }
    else
    {
        NETAPP_MSG(("%s(): Already connecting, do nothing.", __FUNCTION__));
    }
}

static void NetAppWiFiP2P_P_AutoDiscoverTimer(
    void                    *pParam)
{
    NETAPP_P2P_HANDLE   hNetAppP2P = (NETAPP_P2P_HANDLE)pParam;
    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    NetAppWiFiP2PPrivateDiscover(hNetAppP2P);
}


static void NetAppWiFiP2P_P_KeepAliveTimer(
    void                    *pParam)
{
    NETAPP_P2P_HANDLE   hNetAppP2P = (NETAPP_P2P_HANDLE)pParam;
    NETAPP_IP_SETTINGS  tSettings;

    if ( (hNetAppP2P == NULL) || (hNetAppP2P->hP2P == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    if (NetAppPrivateGetIPSettings(hNetAppP2P->hNetAppPriv,
            BCMP2PGetNetifName(hNetAppP2P->hP2P), &tSettings) != NETAPP_SUCCESS)
    {
        return;
    }

    /* Ping the GO, if we don't get an answer then he disconnected and we need to kill the connection */
    if (NetAppPrivateBlockingPing(hNetAppP2P->hNetAppPriv, NetAppNtoA(NETAPP_P2P_GW_ADDR(tSettings.tIpAddress)),
        NETAPP_KEEP_ALIVE_PING_TIMEOUT, NULL) != NETAPP_SUCCESS)
    {
        hNetAppP2P->ucPingFailureCount++;
        if (hNetAppP2P->ucPingFailureCount >= NETAPP_KEEP_ALIVE_MAX_PING_FAILURES)
        {
            NetAppWiFiP2P_P_Disconnect(hNetAppP2P);
        }
    }
    else
    {
        hNetAppP2P->ucPingFailureCount = 0;
    }
}


static void NetAppWiFiP2P_P_DhcpCallback(
    void                    *pParam,
    NETAPP_CB_TYPE          tCbType,
    const void              *pvBuffer,
    uint32_t                ulData0,
    NETAPP_RETCODE          tResult,
    NETAPP_IFACE            tIFace)
{
    NETAPP_UNUSED(pvBuffer);
    NETAPP_UNUSED(tCbType);
    NETAPP_UNUSED(tIFace);
    NetAppWiFiP2P_P_ReportConnectResults((NETAPP_P2P_HANDLE)pParam, tResult, ulData0);
}


NETAPP_RETCODE NetAppWiFiP2P_P_Disconnect(
    NETAPP_P2P_HANDLE       hNetAppP2P)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;

    NETAPP_P2P_PRIV_API_CHECK(hNetAppP2P);

    /* Restore the DNS settings after we disconnect */
    if (NetAppDhcpcdIsRunning(hNetAppP2P->hNetAppDhcpcd, hNetAppP2P->cIfaceName))
    {
        NetAppDhcpcdStop(hNetAppP2P->hNetAppDhcpcd, hNetAppP2P->cIfaceName);
        NETAPP_PRIV_API_CHECK(NetAppPrivateSetDns(hNetAppP2P->hNetAppPriv,
            hNetAppP2P->tPrimaryDNS, hNetAppP2P->tSecondaryDNS), tRetCode);
    }

    NetAppDhcpStop(hNetAppP2P->hNetAppDhcp);
    NetAppOSTimerStop(hNetAppP2P->tKeepAliveTimer);
    BCMP2P_API_CHECK(BCMP2PCancelCreateLink(hNetAppP2P->hP2P), tRetCode);


err_out:
    return tRetCode;
}
