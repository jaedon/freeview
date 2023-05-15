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
 * $brcm_Workfile: netapp_wfi.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/14/11 10:41a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/wlan/netapp_wfi.c $
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp Private WiFi Invite API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * WiFi Invite Implementation for NetAppWiFi
 */

#include "netapp_wifi.h"
#include "netapp_wfi.h"
#include "wfi_api.h"

#define WFI_API_CHECK(api, tRetCode)                                                \
{                                                                                   \
    NETAPP_MSG(("%s() Calling:[%s]", __FUNCTION__, #api));                          \
    WFI_RET result = api;                                                           \
    switch(result)                                                                  \
    {                                                                               \
    case WFI_RET_SUCCESS:                                                           \
        NETAPP_MSG(("%s() [%s] Returned Successful", __FUNCTION__, #api));          \
        tRetCode = NETAPP_SUCCESS;                                                  \
        break;                                                                      \
    case WFI_RET_EXIST:                                                             \
    default:                                                                        \
        tRetCode = NETAPP_FAILURE;                                                  \
        NETAPP_ERR(("%s() [%s] failed!", __FUNCTION__, #api));                      \
        goto err_out;                                                               \
        break;                                                                      \
    }                                                                               \
}


typedef struct sNETAPP_WIFI_INVITE_INFO
{
    wfi_context_t                   tWfiContext;
    struct sNETAPP_WIFI_INVITE_INFO *pNext;
} NETAPP_WIFI_INVITE_INFO;


struct sNETAPP_WFI
{
    bool                        bInserted;
    p_wfi_param_t               pWfi;
    NETAPP_WIFI_INVITE_INFO     *pInviteInfoList;
    char                        *pDevName;
    bool                        bWPS2_0;
    NETAPP_PRIV_HANDLE          hNetAppPriv;
};


static void NetAppWiFiInvite_P_ReceiveInvite(
    wfi_context_t           *pInviteInfo,
    void                    *pParam);


static NETAPP_RETCODE NetAppWiFiInvite_P_RemoveInvite(
    NETAPP_WFI_HANDLE       hWiFiInvite,
    NETAPP_WIFI_INVITE_INFO *pInviteInfo);


static NETAPP_RETCODE NetAppWiFiInvite_P_ConvertWPSCLIInfo(
    brcm_wpscli_nw_settings *pWpscliSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo);


NETAPP_RETCODE NetAppWiFiInvitePrivateInit(
    NETAPP_WFI_HANDLE       *phWiFiInvite,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_SETTINGS         *pSettings)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WFI_HANDLE   hWiFiInvite = NULL;

    if (phWiFiInvite == NULL)
    {
        NETAPP_ERR(("%s() -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hWiFiInvite = (NETAPP_WFI_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_WFI));
    if (hWiFiInvite == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hWiFiInvite, 0, sizeof(struct sNETAPP_WFI));

    hWiFiInvite->pDevName = strdup( ( (pSettings != NULL) && (pSettings->pDeviceName != NULL) ) ?
        pSettings->pDeviceName : NETAPP_DEFAULT_DEVICE_NAME);

    hWiFiInvite->bWPS2_0 = (pSettings != NULL) ? pSettings->bWPS2_0 : false;
    hWiFiInvite->hNetAppPriv = hNetAppPriv;
    *phWiFiInvite = hWiFiInvite;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateSettingsRefresh(
    NETAPP_WFI_HANDLE       hWiFiInvite)
{
    NETAPP_SETTINGS tSettings;
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hWiFiInvite->hNetAppPriv, &tSettings), tRetCode);

    hWiFiInvite->bWPS2_0 = tSettings.bWPS2_0;
    if (hWiFiInvite->pDevName != NULL)
    {
        free(hWiFiInvite->pDevName);
    }

    hWiFiInvite->pDevName = strdup( (tSettings.pDeviceName != NULL) ?
        tSettings.pDeviceName : NETAPP_DEFAULT_DEVICE_NAME);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateDeInit(
    NETAPP_WFI_HANDLE   hWiFiInvite)
{
    NETAPP_WIFI_INVITE_INFO *pInviteInfo = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);

    pInviteInfo = hWiFiInvite->pInviteInfoList;

    while (pInviteInfo != NULL)
    {
        NETAPP_WIFI_INVITE_INFO *pNextInviteInfo = pInviteInfo->pNext;
        NetAppOSMemFree(pInviteInfo);
        pInviteInfo = pNextInviteInfo;
    }

    if (hWiFiInvite->pWfi != NULL)
    {
        wfi_deinit(hWiFiInvite->pWfi);
        hWiFiInvite->pWfi = NULL;
    }

    if (hWiFiInvite->pDevName)
    {
        free(hWiFiInvite->pDevName);
    }

    NetAppOSMemFree(hWiFiInvite);
    hWiFiInvite = NULL;

    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateScan(
    NETAPP_WFI_HANDLE   hWiFiInvite)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);
    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite->pWfi);

    WFI_API_CHECK(wfi_scan(hWiFiInvite->pWfi), tRetCode);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateScanResults(
    NETAPP_WFI_HANDLE       hWiFiInvite)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);
    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite->pWfi);

    WFI_API_CHECK(wfi_parse_scan_results(hWiFiInvite->pWfi), tRetCode);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateAccept(
    NETAPP_WFI_HANDLE       hWiFiInvite,
    const char*             pSSID,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_WIFI_INVITE_INFO *pInviteInfo = NULL;
    NETAPP_RETCODE          tRetCode     = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);
    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite->pWfi);

    if ( (pApInfo == NULL) || (pSSID == NULL) )
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    pInviteInfo = hWiFiInvite->pInviteInfoList;

    while (pInviteInfo != NULL)
    {
        if (strcmp((char*)pInviteInfo->tWfiContext.stBSS.ssid.SSID, pSSID) == 0)
        {
            NETAPP_MSG(("%s(): Found saved context! for %s !",
                __FUNCTION__, pInviteInfo->tWfiContext.stBSS.ssid.SSID));
            break;
        }
        pInviteInfo = pInviteInfo->pNext;
    }

    if (pInviteInfo == NULL)
    {
        NETAPP_ERR(("%s(): No Invite context Found for %s!",__FUNCTION__, pSSID));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    WFI_API_CHECK(wfi_accept(hWiFiInvite->pWfi, &pInviteInfo->tWfiContext), tRetCode);

    tRetCode = NetAppWiFiInvite_P_ConvertWPSCLIInfo(&hWiFiInvite->pWfi->wps_cred, pApInfo);

err_out:
    /* Need to free context */
    if ( (pInviteInfo != NULL) && (tRetCode == NETAPP_SUCCESS) )
    {
        NetAppWiFiInvite_P_RemoveInvite(hWiFiInvite, pInviteInfo);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateReject(
    NETAPP_WFI_HANDLE   hWiFiInvite,
    char                *pSSID)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_WIFI_INVITE_INFO *pInviteInfo = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);
    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite->pWfi);

    pInviteInfo = hWiFiInvite->pInviteInfoList;

    while (pInviteInfo != NULL)
    {
        if (strcmp((char*)pInviteInfo->tWfiContext.stBSS.ssid.SSID, pSSID) == 0)
        {
            NETAPP_MSG(("%s(): Found saved context! for %s !",
                __FUNCTION__, pInviteInfo->tWfiContext.stBSS.ssid.SSID));
            break;
        }
        pInviteInfo = pInviteInfo->pNext;
    }

    if (pInviteInfo == NULL)
    {
        NETAPP_ERR(("%s(): No Invite context Found for %s!",__FUNCTION__, pSSID));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    WFI_API_CHECK(wfi_reject(hWiFiInvite->pWfi, &pInviteInfo->tWfiContext), tRetCode);

err_out:
    /* Need to free context */
    if ( (pInviteInfo != NULL) && (tRetCode == NETAPP_SUCCESS) )
    {
        NetAppWiFiInvite_P_RemoveInvite(hWiFiInvite, pInviteInfo);
    }

    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInvitePrivateStart(
    NETAPP_WFI_HANDLE   hWiFiInvite)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);

    if (hWiFiInvite->pWfi != NULL)
    {
        wfi_deinit(hWiFiInvite->pWfi);
        hWiFiInvite->pWfi = NULL;
    }

    hWiFiInvite->pWfi = wfi_init(hWiFiInvite->pDevName, WFI_PIN_AUTO_GENERATE, true);
    if (hWiFiInvite->pWfi == NULL)
    {
        NETAPP_ERR(("%s(): Failure Creating WiFi Invite API\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    hWiFiInvite->pWfi->wfi_invite_rcvd_hndlr = &NetAppWiFiInvite_P_ReceiveInvite;
    hWiFiInvite->pWfi->param    = hWiFiInvite;
    //hWiFiInvite->pWfi->WSC2     = hWiFiInvite->bWPS2_0;

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiInvitePrivateStop(
    NETAPP_WFI_HANDLE   hWiFiInvite)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);

    if (hWiFiInvite->pWfi != NULL)
    {
        WFI_API_CHECK(wfi_stop_scan(hWiFiInvite->pWfi), tRetCode);
    }

err_out:
    return tRetCode;
}


static void NetAppWiFiInvite_P_ReceiveInvite(
    wfi_context_t       *pWfiContext,
    void                *pParam)
{
    NETAPP_RETCODE          tRetCode        = NETAPP_SUCCESS;
    NETAPP_WFI_HANDLE       hWiFiInvite     = (NETAPP_WFI_HANDLE)pParam;
    NETAPP_WIFI_INVITE_INFO *pInviteInfo    = NULL;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    if ( (hWiFiInvite == NULL) || (hWiFiInvite->pWfi == NULL) || (pWfiContext == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    NETAPP_MSG(("%s(): Received invite from [%s]!", __FUNCTION__, pWfiContext->stBSS.ssid.SSID));

    pInviteInfo = hWiFiInvite->pInviteInfoList;

    while (pInviteInfo != NULL)
    {
        if (memcmp(&pInviteInfo->tWfiContext.stBSS.bssid, &pWfiContext->stBSS.bssid, sizeof(bssid_t)) == 0)
        {
            NETAPP_MSG(("%s(): Already Received invite for %s !", __FUNCTION__, pWfiContext->stBSS.ssid.SSID));

            /* The router SSID changed, need to update context */
            if (strncmp((char*)&pInviteInfo->tWfiContext.stBSS.ssid.SSID,
                (char*)&pWfiContext->stBSS.ssid.SSID, pWfiContext->stBSS.ssid.SSID_len) != 0)
            {
                pInviteInfo->tWfiContext = *pWfiContext;
            }
            break;
        }
        pInviteInfo = pInviteInfo->pNext;
    }

    if (pInviteInfo == NULL)
    {
        NETAPP_MSG(("%s(): Creating new context for [%s]!", __FUNCTION__, pWfiContext->stBSS.ssid.SSID));
        /* Create the callers context handle */
        pInviteInfo = (NETAPP_WIFI_INVITE_INFO*)NetAppOSMemAlloc(sizeof(NETAPP_WIFI_INVITE_INFO));
        if (pInviteInfo == NULL)
        {
            NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
            tRetCode = NETAPP_OUT_OF_MEMORY;
            goto err_out;
        }
        memset(pInviteInfo, 0, sizeof(NETAPP_WIFI_INVITE_INFO));

        pInviteInfo->tWfiContext = *pWfiContext;
        pInviteInfo->pNext       = NULL;

        if (hWiFiInvite->pInviteInfoList == NULL)
        {
            hWiFiInvite->pInviteInfoList = pInviteInfo;
        }
        else
        {
            NETAPP_WIFI_INVITE_INFO *pCurrent = hWiFiInvite->pInviteInfoList;
            while (pCurrent->pNext != NULL)
            {
                pCurrent = pCurrent->pNext;
            }
            pCurrent->pNext = pInviteInfo;
        }
    }

    NETAPP_DO_CALLBACK(NETAPP_CB_INVITE, pWfiContext->stBSS.ssid.SSID,
        strlen(pWfiContext->stBSS.ssid.SSID)+1, 0, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return;
}


static NETAPP_RETCODE NetAppWiFiInvite_P_RemoveInvite(
    NETAPP_WFI_HANDLE       hWiFiInvite,
    NETAPP_WIFI_INVITE_INFO *pInviteInfo)
{
    NETAPP_WIFI_INVITE_INFO *pCurInviteInfo = NULL;
    NETAPP_WIFI_INVITE_INFO *pPrevInviteInfo = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hWiFiInvite);
    NETAPP_PRIV_HANDLE_CHECK(pInviteInfo);

    pPrevInviteInfo = pCurInviteInfo = hWiFiInvite->pInviteInfoList;
    while (pCurInviteInfo != NULL)
    {
        if (pCurInviteInfo == pInviteInfo)
        {
            if (pCurInviteInfo->pNext != NULL)
            {
                if (pCurInviteInfo == hWiFiInvite->pInviteInfoList)
                {
                    hWiFiInvite->pInviteInfoList = pCurInviteInfo->pNext;
                }
                else
                {
                    pPrevInviteInfo->pNext = pCurInviteInfo->pNext;
                }
            }
            else if (hWiFiInvite->pInviteInfoList == pCurInviteInfo)
            {
                hWiFiInvite->pInviteInfoList = NULL;
            }
            else
            {
                pPrevInviteInfo->pNext = NULL;
            }

            NETAPP_MSG(("%s(): Freeing Invite from [%s]!", __FUNCTION__, pInviteInfo->tWfiContext.stBSS.ssid.SSID));
            NetAppOSMemFree(pInviteInfo);
            pInviteInfo = NULL;
            break;
        }
        pPrevInviteInfo = pCurInviteInfo;
        pCurInviteInfo  = pCurInviteInfo->pNext;
    }

    return NETAPP_SUCCESS;
}


static NETAPP_RETCODE NetAppWiFiInvite_P_ConvertWPSCLIInfo(
    brcm_wpscli_nw_settings *pWpscliSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_PRIV_HANDLE_CHECK(pWpscliSettings);
    NETAPP_PRIV_HANDLE_CHECK(pApInfo);

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    memset(pApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));

    /* SSID */
    strncpy(pApInfo->cSSID, pWpscliSettings->ssid,
        sizeof(pApInfo->cSSID)/sizeof(pApInfo->cSSID[0])-1);

    /* BSSID */
    strncpy(pApInfo->cPassword, pWpscliSettings->nwKey,
        sizeof(pApInfo->cPassword)/sizeof(pApInfo->cPassword[0])-1);

    /* Password */
    strncpy(pApInfo->cPassword, pWpscliSettings->nwKey,
        sizeof(pApInfo->cPassword)/sizeof(pApInfo->cPassword[0])-1);

    pApInfo->bAdHoc = false;

    if ( (pWpscliSettings->encrType == BRCM_WPS_ENCRTYPE_AES) &&
         (pWpscliSettings->authType == BRCM_WPS_AUTHTYPE_WPA2PSK) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_AES;
    }
    else if ( (pWpscliSettings->encrType == BRCM_WPS_ENCRTYPE_AES) &&
              (pWpscliSettings->authType == BRCM_WPS_AUTHTYPE_WPAPSK) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_AES;
    }
    else if ( (pWpscliSettings->encrType == BRCM_WPS_ENCRTYPE_TKIP) &&
              (pWpscliSettings->authType == BRCM_WPS_AUTHTYPE_WPA2PSK) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP;
    }
    else if ( (pWpscliSettings->encrType == BRCM_WPS_ENCRTYPE_TKIP) &&
              (pWpscliSettings->authType == BRCM_WPS_AUTHTYPE_WPAPSK) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_TKIP;
    }
    else if (pWpscliSettings->encrType == BRCM_WPS_ENCRTYPE_WEP)
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WEP;
    }
    else
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_NONE;
    }

    return NETAPP_SUCCESS;
}
