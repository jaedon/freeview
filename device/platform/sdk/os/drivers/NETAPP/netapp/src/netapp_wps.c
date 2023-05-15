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
 * $brcm_Workfile: netapp_wps.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/26/11 4:02p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_wps.c $
 * 
 * 2   8/26/11 4:02p steven
 * SWBLURAY-27043: WPS 2.0 Support
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp WiFi API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal WPS Implementation using WPSAPI
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_priv.h"
#include "netapp_wps.h"
#include <time.h>

#define SITE_TYPEDEFS
#include <typedefs.h>
#include "wps_sdk.h"

#define NETAPP_FIND_PBC_AP              120000
#define NETAPP_FIND_PIN_AP              50000
#define NETAPP_WPS_LOOP_WAIT            50
#define NETAPP_WPS_LOOP_TIMEOUT         1000


#define WPS_API_CHECK(api, tRetCode)                                            \
{                                                                               \
    bool result = api;                                                          \
    if (result)                                                                 \
    {                                                                           \
    NETAPP_MSG(("%s WPSAPI [%s] Success!", __FUNCTION__, #api));                \
        tRetCode = NETAPP_SUCCESS;                                              \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        NETAPP_ERR(("%s WPSAPI[%s] failed!", __FUNCTION__, #api));              \
        tRetCode = NETAPP_FAILURE;                                              \
        goto err_out;                                                           \
    }                                                                           \
}


#define WPS_API_STATUS_CHECK(api, tRetCode)                                     \
{                                                                               \
    switch(api)                                                                 \
    {                                                                           \
    case WPS_STATUS_SUCCESS:                                                    \
        NETAPP_MSG(("%s WPSAPI [%s] Success!", __FUNCTION__, #api));            \
        tRetCode = NETAPP_SUCCESS;                                              \
        break;                                                                  \
                                                                                \
    case WPS_STATUS_ERROR:                                                      \
        NETAPP_ERR(("%s WPSAPI[%s] failed!", __FUNCTION__, #api));              \
        tRetCode = wps_api_is_wep_incompatible() ?                              \
            NETAPP_WPS_2_ERR_INCOMPATIBLE : NETAPP_FAILURE;                     \
        break;                                                                  \
                                                                                \
    case WPS_STATUS_CANCELED:                                                   \
        NETAPP_MSG(("%s WPSAPI[%s] Canceled!", __FUNCTION__, #api));            \
        tRetCode =  NETAPP_CANCELED;                                            \
        break;                                                                  \
                                                                                \
    case WPS_STATUS_WRONG_PIN:                                                  \
        NETAPP_MSG(("%s WPSAPI[%s] Wrong Pin!", __FUNCTION__, #api));           \
        tRetCode = NETAPP_INVALID_PIN;                                          \
        break;                                                                  \
                                                                                \
    default:                                                                    \
        tRetCode = NETAPP_TIMEOUT;                                              \
        break;                                                                  \
    }                                                                           \
    if (tRetCode != NETAPP_SUCCESS)                                             \
    {                                                                           \
        goto err_out;                                                           \
    }                                                                           \
}


struct sNETAPP_WPS
{
    wps_devinf              tDevInfo;
    bool                    bWPS2_0;
    bool                    *pbStopWPS;
    NETAPP_PRIV_HANDLE      hNetAppPriv;
    NETAPP_IFACE            tUUIDIface;
    char                    cBuffer[4096];
    NETAPP_INIT_SETTINGS    tInitSettings;
    char                    cIfaceName[NETAPP_IFACE_NAME_LEN+1];
};


static wps_devinf DEFAULT_DEVIDE_INFO =
{
    "NetApp WPS Device",
    DEV_CAT_DISPLAYS,
    DEV_SUB_CAT_DISP_TV,
    "Broadcom",
    "CEG Device",
    "010203",
    "20031222",
};

static char* MY_WPS_MSG_STRS[] =
{
    " NONE",
    "(BEACON)",
    "(PROBE_REQ)",
    "(PROBE_RESP)",
    "(M1)",
    "(M2)",
    "(M2D)",
    "(M3)",
    "(M4)",
    "(M5)",
    "(M6)",
    "(M7)",
    "(M8)",
    "(ACK)",
    "(NACK)",
    "(DONE)",
    " / Identity",
    "(Start)",
    "(FAILURE)",
    "(FRAG)",
    "(FRAG_ACK)",
    "(EAPOL-Start)"
};

static char *
_wps_print_my_msg_string(int mtype)
{
    if (mtype >= 0 && mtype <= (sizeof(MY_WPS_MSG_STRS)/sizeof(MY_WPS_MSG_STRS[0])-1))
        return MY_WPS_MSG_STRS[mtype];

    return MY_WPS_MSG_STRS[0];
}


static void NetAppWiFiWPS_P_Callback(
    void                    *pHandle,
    unsigned int            uiStatus,
    void                    *pData);


static NETAPP_RETCODE NetAppWiFiWPS_P_FindAp(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    NETAPP_CONNECT_TYPE     tConnectType,
    NETAPP_WIFI_AP_INFO     *pApInfo,
    wps_apinf               *pWPSApInfo,
    struct timespec         tStartTime);


static NETAPP_RETCODE NetAppWiFiWPS_P_ProcessEvent(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    wps_apinf               tWPSApInfo);


NETAPP_RETCODE NetAppWiFiWPSPrivateInit(
    NETAPP_WPS_HANDLE       *phNetAppWPS,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    bool                    *pbStopFlag)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WPS_HANDLE   hNetAppWPS = NULL;

    if ( (phNetAppWPS == NULL) || (hNetAppPriv == NULL) || (pInitSettings == NULL) )
    {
        NETAPP_ERR(("%s() -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppWPS = (NETAPP_WPS_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_WPS));
    if (hNetAppWPS == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppWPS, 0, sizeof(struct sNETAPP_WPS));

    hNetAppWPS->hNetAppPriv     = hNetAppPriv;
    hNetAppWPS->pbStopWPS       = pbStopFlag;
    *phNetAppWPS                = hNetAppWPS;
    hNetAppWPS->tInitSettings   = *pInitSettings;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiWPSPrivateDeInit(
    NETAPP_WPS_HANDLE       hNetAppWPS)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppWPS);
    wps_api_close();
    NetAppOSMemFree(hNetAppWPS);
    hNetAppWPS = NULL;
    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppWiFiWPSConnect(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    NETAPP_CONNECT_TYPE     tConnectType,
    NETAPP_WIFI_AP_INFO     *pApInfo,
    uint32_t                ulPin,
    char                    *pIfaceName)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    wps_apinf               tWPSApInfo;
    enum eWPS_MODE          tWPSMode;
    char                    cPin[10];
    struct timespec         tStartTime;
    struct timespec         tCurrentTime;
    wps_credentials         tWPSCredentials;
    struct ether_addr       addr;
    int                     i;
    NETAPP_SETTINGS         tSettings;
    NETAPP_SETTINGS         tDefSettings;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppWPS);

    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    memset(&tWPSApInfo, 0, sizeof(tWPSApInfo));
    snprintf(cPin, sizeof(cPin), "%d", ulPin);
    memset(&hNetAppWPS->tDevInfo, 0, sizeof(hNetAppWPS->tDevInfo));

    switch(tConnectType)
    {
    case NETAPP_CONNECT_WPS_PB:
        tWPSMode = STA_ENR_JOIN_NW_PBC;
        break;
    case NETAPP_CONNECT_WPS_PIN_ENROLLEE:
        tWPSMode = STA_ENR_JOIN_NW_PIN;
        break;
    case NETAPP_CONNECT_WPS_PIN_REGISTRAR:
        tWPSMode = STA_ENR_JOIN_NW_PBC;
        break;
    default:
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }


    /* Step 0: Prepare the Device information */
    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppWPS->hNetAppPriv, &tSettings), tRetCode);

    strncpy(hNetAppWPS->tDevInfo.deviceName,   hNetAppWPS->tInitSettings.pDeviceName,   sizeof(hNetAppWPS->tDevInfo.deviceName)/sizeof(hNetAppWPS->tDevInfo.deviceName[0])-1);
    strncpy(hNetAppWPS->tDevInfo.manufacturer, hNetAppWPS->tInitSettings.pManufacturer, sizeof(hNetAppWPS->tDevInfo.manufacturer)/sizeof(hNetAppWPS->tDevInfo.manufacturer[0])-1);
    strncpy(hNetAppWPS->tDevInfo.modelName,    hNetAppWPS->tInitSettings.pModelName,    sizeof(hNetAppWPS->tDevInfo.modelName)/sizeof(hNetAppWPS->tDevInfo.modelName[0])-1);
    strncpy(hNetAppWPS->tDevInfo.modelNumber,  hNetAppWPS->tInitSettings.pModelNumber,  sizeof(hNetAppWPS->tDevInfo.modelNumber)/sizeof(hNetAppWPS->tDevInfo.modelNumber[0])-1);
    strncpy(hNetAppWPS->tDevInfo.serialNumber, hNetAppWPS->tInitSettings.pSerialNumber, sizeof(hNetAppWPS->tDevInfo.serialNumber)/sizeof(hNetAppWPS->tDevInfo.serialNumber[0])-1);
    memcpy(hNetAppWPS->tDevInfo.transport_uuid,hNetAppWPS->tInitSettings.cTransportUUID,sizeof(hNetAppWPS->tDevInfo.transport_uuid));

    hNetAppWPS->tDevInfo.primDeviceCategory     = DEV_CAT_DISPLAYS;
    hNetAppWPS->tDevInfo.primDeviceSubCategory  = DEV_SUB_CAT_DISP_TV;
    hNetAppWPS->bWPS2_0                         = tSettings.bWPS2_0;

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetMacAddress(hNetAppWPS->hNetAppPriv, pIfaceName, &addr), tRetCode);
//    memcpy(hNetAppWPS->tDevInfo.wps_uuid+10, addr.ether_addr_octet, sizeof(addr.ether_addr_octet));
    memcpy(hNetAppWPS->tDevInfo.transport_uuid+10, addr.ether_addr_octet, sizeof(addr.ether_addr_octet));

    NETAPP_MSG(("\n%s():", __FUNCTION__));
    NETAPP_MSG(("%s(): Device Name:         [%s]", __FUNCTION__, hNetAppWPS->tDevInfo.deviceName));
    NETAPP_MSG(("%s(): Device Category:     [%d]", __FUNCTION__, hNetAppWPS->tDevInfo.primDeviceCategory));
    NETAPP_MSG(("%s(): Device Sub Category: [%d]", __FUNCTION__, hNetAppWPS->tDevInfo.primDeviceSubCategory));
    NETAPP_MSG(("%s(): Manufacturer:        [%s]", __FUNCTION__, hNetAppWPS->tDevInfo.manufacturer));
    NETAPP_MSG(("%s(): Model Name:          [%s]", __FUNCTION__, hNetAppWPS->tDevInfo.modelName));
    NETAPP_MSG(("%s(): Model Number:        [%s]", __FUNCTION__, hNetAppWPS->tDevInfo.modelNumber));
    NETAPP_MSG(("%s(): Serial Number:       [%s]", __FUNCTION__, hNetAppWPS->tDevInfo.serialNumber));
    NETAPP_MSG(("%s(): WPS Mode:            [%s]", __FUNCTION__, tSettings.bWPS2_0 ? "2.0" : "1.0"));

    clock_gettime(CLOCK_MONOTONIC, &tStartTime);


    /* Step 1: Open WPS API Library */
    WPS_API_CHECK(wps_api_open(NULL, (hNetAppWPS->cIfaceName[0] != '\0') ? hNetAppWPS->cIfaceName : NULL, hNetAppWPS, NetAppWiFiWPS_P_Callback,
        &hNetAppWPS->tDevInfo, (tConnectType == NETAPP_CONNECT_WPS_PIN_REGISTRAR), tSettings.bWPS2_0), tRetCode);


    /* Step 2: Find an AP */
    NETAPP_PRIV_API_CHECK(NetAppWiFiWPS_P_FindAp(hNetAppWPS, tConnectType, pApInfo, &tWPSApInfo,
        tStartTime), tRetCode);


    /* Step 3: Associate */
    WPS_API_CHECK(wps_api_join(tWPSApInfo.bssid, tWPSApInfo.ssid, tWPSApInfo.wep), tRetCode);



    /* Step 4: Inform link up */
    wps_api_set_linkup();



    /* Step 5. Run WPS */
    WPS_API_CHECK(wps_api_run(tWPSMode, tWPSApInfo.bssid, tWPSApInfo.ssid, tWPSApInfo.wep,
        (tConnectType == NETAPP_CONNECT_WPS_PB) ? NULL : cPin, NULL, false), tRetCode);



    /* Step 6: Wait for the  AP to be connected */
    while( (tRetCode = NetAppWiFiWPS_P_ProcessEvent(hNetAppWPS, tWPSApInfo)) == NETAPP_TIMEOUT)
    {
        int32_t lTimeDiffms;

        int32_t ulResult = wps_api_process_timeout();
        if (ulResult == WPS_STATUS_REJOIN)
        {
            wps_api_join(tWPSApInfo.bssid, tWPSApInfo.ssid, tWPSApInfo.wep);
            wps_api_set_linkup();
        }
        else if (ulResult == WPS_STATUS_ERROR)
        {
            tRetCode = NETAPP_FAILURE;
            break;
        }

        NetAppOSTaskDelayMsec(50);

        clock_gettime(CLOCK_MONOTONIC, &tCurrentTime);
        lTimeDiffms = (tCurrentTime.tv_sec*1000 + tCurrentTime.tv_nsec/1000000) -
            (tStartTime.tv_sec*1000 + tStartTime.tv_nsec/1000000);

        if ( (lTimeDiffms > NETAPP_FIND_PBC_AP) || (*hNetAppWPS->pbStopWPS) )
        {
            tRetCode = (*hNetAppWPS->pbStopWPS) ? NETAPP_CANCELED : NETAPP_TIMEOUT;
            break;
        }
    }


    /* Step 7: Process the result of WPS */
    if (tRetCode == NETAPP_SUCCESS)
    {
        NETAPP_MSG(("%s(): WPS Successful!", __FUNCTION__));
        if (wps_api_get_credentials(&tWPSCredentials) == NULL)
        {
            NETAPP_ERR(("%s(): Unable to retrieve credentials!", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
        snprintf(pApInfo->cPassword, sizeof(pApInfo->cPassword), "%s", tWPSCredentials.nwKey);
        WPS_API_CHECK(wps_api_create_profile(&tWPSCredentials), tRetCode);
    }

err_out:
    if (tRetCode == NETAPP_CANCELED)
    {
        wps_api_abort();
    }
    wps_api_close();
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiWPSUpdateIfaceName(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    const char              *pIfaceName)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppWPS);

    memset(hNetAppWPS->cIfaceName, 0, sizeof(hNetAppWPS->cIfaceName));
    if (pIfaceName != NULL)
    {
        strncpy(hNetAppWPS->cIfaceName, pIfaceName, sizeof(hNetAppWPS->cIfaceName)/sizeof(hNetAppWPS->cIfaceName[0])-1);
    }
    return NETAPP_SUCCESS;
}

/*******************************************************************************
 *                       Static NetApp WiFi WPS Functions
 ******************************************************************************/
static void NetAppWiFiWPS_P_Callback(
    void                    *pHandle,
    unsigned int            uiStatus,
    void                    *pData)
{
    NETAPP_WPS_HANDLE       hNetAppWPS = (NETAPP_WPS_HANDLE)pHandle;

    if (pHandle == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter in Callback!", __FUNCTION__));
        return;
    }

    switch (uiStatus)
    {
    case WPS_STATUS_INIT:
        NETAPP_MSG(("NetApp WPS Status: INIT"));
        break;
    case WPS_STATUS_DISABLING_WIFI_MANAGEMENT:
        NETAPP_MSG(("NetApp WPS Status: DISABLING_WIFI_MANAGEMENT"));
        break;
    case WPS_STATUS_SCANNING:
        NETAPP_MSG(("NetApp WPS Status: SCANNING"));
        break;
    case WPS_STATUS_SCANNING_OVER:
        NETAPP_MSG(("NetApp WPS Status: SCANNING OVER"));
        break;
    case WPS_STATUS_ASSOCIATING:
        NETAPP_MSG(("NetApp WPS Status: ASSOCIATING TO %s", (char*) pData));
        break;
    case WPS_STATUS_ASSOCIATED:
        NETAPP_MSG(("NetApp WPS Status: ASSOCIATED TO %s", (char*) pData));
        break;
    case WPS_STATUS_STARTING_WPS_EXCHANGE:
        NETAPP_MSG(("NetApp WPS Status: STARTING_WPS_EXCHANGE"));
        break;
    case WPS_STATUS_SENDING_WPS_MESSAGE:
        NETAPP_MSG(("NetApp WPS Status: SENDING_WPS_MESSAGE %s", _wps_print_my_msg_string(*((int *)pData))));
        break;
    case WPS_STATUS_WAITING_WPS_RESPONSE:
        NETAPP_MSG(("NetApp WPS Status: WAITING_WPS_RESPONSE"));
        break;
    case WPS_STATUS_GOT_WPS_RESPONSE:
        NETAPP_MSG(("NetApp WPS Status: GOT_WPS_RESPONSE %s", _wps_print_my_msg_string(*((int *)pData))));
        break;
    case WPS_STATUS_DISCONNECTING:
        NETAPP_MSG(("NetApp WPS Status: DISCONNECTING"));
        break;
    case WPS_STATUS_ENABLING_WIFI_MANAGEMENT:
        NETAPP_MSG(("NetApp WPS Status: ENABLING_WIFI_MANAGEMENT"));
        break;
    case WPS_STATUS_SUCCESS:
        NETAPP_MSG(("NetApp WPS Status: SUCCESS"));
        break;
    case WPS_STATUS_CANCELED:
        NETAPP_MSG(("NetApp WPS Status: CANCELED"));
        break;
    case WPS_STATUS_WARNING_WPS_PROTOCOL_FAILED:
        NETAPP_MSG(("NetApp WPS Status: ERROR_WPS_PROTOCOL"));
        break;
    case WPS_STATUS_WRONG_PIN:
        NETAPP_MSG(("NetApp WPS Status: WPS_STATUS_WRONG_PIN %s", (char*) pData));
        break;
    case WPS_STATUS_WARNING_NOT_INITIALIZED:
        NETAPP_MSG(("NetApp WPS Status: WARNING_NOT_INITIALIZED"));
        break;
    case WPS_STATUS_ERROR:
        NETAPP_MSG(("NetApp WPS Status: ERROR"));
        break;
    case WPS_STATUS_CREATING_PROFILE:
        NETAPP_MSG(("NetApp WPS Status: WPS_STATUS_CREATING_PROFILE"));
        break;
    case WPS_STATUS_OVERALL_PROCESS_TIMEOUT:
        NETAPP_MSG(("NetApp WPS Status: WPS_STATUS_OVERALL_PROCESS_TIMEOUT"));
        break;
    case WPS_STATUS_REJOIN:
        NETAPP_MSG(("NetApp WPS Status: WPS_STATUS_REJOIN"));
        break;
    case WPS_STATUS_IDLE:
        NETAPP_MSG(("NetApp WPS Status: IDLE"));
        break;
    default:
        NETAPP_MSG(("NetApp WPS Status: Unknown %d", uiStatus));
    }
}


static NETAPP_RETCODE NetAppWiFiWPS_P_FindAp(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    NETAPP_CONNECT_TYPE     tConnectType,
    NETAPP_WIFI_AP_INFO     *pApInfo,
    wps_apinf               *pWPSApInfo,
    struct timespec         tStartTime)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    struct timespec         tCurrentTime;
    struct wps_ap_list_info *pApList;
    int32_t                 nAP = 0;
    bool                    bIsPbc = (tConnectType == NETAPP_CONNECT_WPS_PB);

    NETAPP_PRIV_HANDLE_CHECK(hNetAppWPS);
    if ( (pApInfo == NULL) || (pWPSApInfo == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    tCurrentTime = tStartTime;

    pApList = wps_api_surveying(bIsPbc, false, true);
    while (1)
    {
        int i = 0;
        int lTimeDiffMs = 0;

        if (pApList && wps_api_find_ap(pApList, &nAP, bIsPbc, NULL, false, NULL, false) )
        {
            NETAPP_MSG(("%s(): Found WPS enabled AP(s)!", __FUNCTION__));

            /* Searching for AP by SSID (used for PIN) */
            if ( !bIsPbc && (pApInfo->cSSID[0] != '\0') )
            {
                int     i = 0;
                bool    bFound = false;
                while (wps_api_get_ap(i, pWPSApInfo))
                {
                    NETAPP_MSG(("%s(): AP: %s!", __FUNCTION__, pWPSApInfo->ssid));
                    if (strncmp(pApInfo->cSSID, pWPSApInfo->ssid, sizeof(pApInfo->cSSID)) == 0)
                    {
                        bFound = true;
                        break;
                    }
                    i++;
                }
                if (bFound)
                {
                    NETAPP_MSG(("%s(): Found %s!", __FUNCTION__, pApInfo->cSSID));
                    break;
                }
            }
            else
            {
                /* Get AP info */
                WPS_API_CHECK(wps_api_get_ap(0, pWPSApInfo), tRetCode);
                break;
            }
        }


        /* Need active component */
        /* Busy loop */
        for (i = 0; (i*NETAPP_WPS_LOOP_WAIT) < NETAPP_WPS_LOOP_TIMEOUT; i++)
        {
            if (*hNetAppWPS->pbStopWPS)
            {
                tRetCode = NETAPP_CANCELED;
                goto err_out;
            }
            NetAppOSTaskDelayMsec(NETAPP_WPS_LOOP_WAIT);
        }

        clock_gettime(CLOCK_MONOTONIC, &tCurrentTime);
        lTimeDiffMs = (tCurrentTime.tv_sec*1000 + tCurrentTime.tv_nsec/1000000) -
                        (tStartTime.tv_sec*1000 + tStartTime.tv_nsec/1000000);

        if (lTimeDiffMs > NETAPP_FIND_PBC_AP)
        {
            NETAPP_MSG(("%s(): Timed out trying to find an AP!", __FUNCTION__));
            tRetCode = NETAPP_TIMEOUT;
            goto err_out;
        }

        pApList = wps_api_surveying(bIsPbc, hNetAppWPS->bWPS2_0, false);
    }

    if (bIsPbc && (nAP > 1) )
    {
        NETAPP_MSG(("%s(): More than one AP found in scan request!", __FUNCTION__));
        tRetCode = NETAPP_WPS_MULTIPLE_AP_FOUND;
        goto err_out;
    }

err_out:
    return tRetCode;

}


static NETAPP_RETCODE NetAppWiFiWPS_P_ProcessEvent(
    NETAPP_WPS_HANDLE       hNetAppWPS,
    wps_apinf               tWPSApInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    uint32_t        ulLength;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppWPS);

    ulLength = sizeof(hNetAppWPS->cBuffer);
    WPS_API_STATUS_CHECK(wps_api_poll_eapol_packet(hNetAppWPS->cBuffer, &ulLength), tRetCode);
    WPS_API_STATUS_CHECK(wps_api_process_data(hNetAppWPS->cBuffer, ulLength), tRetCode);

err_out:
    return tRetCode;
}


