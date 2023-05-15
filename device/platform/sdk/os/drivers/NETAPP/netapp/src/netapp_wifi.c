/***************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: netapp_wifi.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_wifi.c $
 *
 * 5   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 12   1/5/12 1:21p steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 11   12/7/11 2:08p steven
 * SW7231-514: Improvements to NetApp build to build standalone
 *
 * 10   12/1/11 8:56p steven
 * SW7425-1404: Moved kernel modules and fixes to BT support
 *
 * 9   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * 8   9/9/11 9:08a steven
 * SWDTV-8586: fixed an issue with shutdown of the lists
 *
 * 7   9/1/11 1:29p steven
 * SW7425-1223: Fixed memory Leaks in NetApp
 *
 * 6   8/26/11 4:02p steven
 * SWBLURAY-27043: WPS 2.0 Support
 *
 * 5   8/19/11 12:48p steven
 * SWBLURAY-26272: WoWL Integration
 *
 * SWBLURAY-26272/1   8/18/11 9:26p steven
 * SWBLURAY-26272: WoWL Integration
 *
 * 4   8/4/11 10:15a steven
 * SWBLURAY-26638: Moved Auto-reconnect down to NetApp
 *
 * fix_bdvd_v3.0_SWBLURAY-26638/3   8/1/11 3:02p steven
 * SWBLURAY-2663: Fix for Connection failing
 *
 * fix_bdvd_v3.0_SWBLURAY-26638/2   7/29/11 4:19p steven
 * SWBLURAY-26638: missed a change...
 *
 * fix_bdvd_v3.0_SWBLURAY-26638/1   7/29/11 4:08p steven
 * SWBLURAY-26638: Ported fix to mainline
 *
 * 3   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 *
 * dev_main_SW7425-749/2   6/28/11 2:24p steven
 * Check in initial P2P support
 *
 * dev_main_SW7425-749/1   6/21/11 12:55p steven
 * Added WoWL support
 *
 * 2   6/16/11 10:57a steven
 * SW7425-714: Fix for WPS and WiFi Invite and NDVD build
 *
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief Internal NetApp WiFi API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by netapp.c
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_wifi.h"
#include "bwl.h"
#ifndef NO_WIFI_INVITE
#include "wpscli_api.h"
#endif
#include "epivers.h"
#include <time.h>
#ifndef NO_WIFI_WPS
#include "netapp_wps.h"
#endif
#include "netapp_list.h"

/* Following function is found in wpscli.so but not exposed */
extern bool wps_validateChecksum(uint32_t ulPin);


#define NETAPP_WIFI_CONNECT_TIMEOUT     10000
#define NETAPP_WIFI_DISCONNECT_TIMEOUT  1000
#define NETAPP_WIFI_SCAN_TIMEOUT        20000
#define NETAPP_WIFI_WAIT_MS             50
#define NETAPP_WIFI_CONNECT_RETRY       1
#define NETAPP_WIFI_SUP_TIMEOUT         10000
#define NETAPP_WPS_PIN_SIZE             8
#define NETAPP_WPS_PB_DELAY_MS          2000
#define NETAPP_WPS_PB_TIMEOUT_MS        120000
#define NETAPP_WIFI_QUEUE_SIZE          1
#define NETAPP_WIFI_INIT_TIMEOUT		2000
#define NETAPP_BCMDL_CMD                "bcmdl"
#define NETAPP_FIRMWARE_PATH            "firmware/brcm/bcm4323x-firmware.bin"
#define NETAPP_NVRAM_PATH               "firmware/brcm/bcm4323x.nvm"
#define NETAPP_DHDMIPS_CMD              "/usr/local/bin/dhdmips"
#define NETAPP_DEFAULT_IFACE_NAME       "wln"

static struct
{
    NETAPP_WIFI_SECURITY tSecurity;
    const char           *pName;
} SecurityPrintTable[] =
{
    { NETAPP_WIFI_SECURITY_NONE,          "No Security"                   },
    { NETAPP_WIFI_SECURITY_WEP,           "WEP (Open & Shared)"           },
    { NETAPP_WIFI_SECURITY_WPA_PSK_AES,   "WPA-Personal, AES encryp."     },
    { NETAPP_WIFI_SECURITY_WPA_PSK_TKIP,  "WPA-Personal, TKIP encryp."    },
    { NETAPP_WIFI_SECURITY_WPA2_PSK_AES,  "WPA2-Personal, AES encryp."    },
    { NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP, "WPA2-Personal, TKIP encryp."   },
    { NETAPP_WIFI_SECURITY_NOT_SUPPORTED, "Security format not supported" },
};

typedef enum {
    NETAPP_WIFI_DRIVER_FALCON = 0,
    NETAPP_WIFI_DRIVER_KIRIN,
    NETAPP_WIFI_DRIVER_AARDVARK,
    NEXUS_WIFI_DRIVER_MAX
} NETAPP_WIFI_DRIVER;

/* RSSI Levels */
static int32_t lRSSILevels[NEXUS_WIFI_DRIVER_MAX][NETAPP_WIFI_RSSI_MAX] =
{
    /* NONE,    POOR,    FAIR,   GOOD,  EXCELLENT */
    { -90,      -80,      -70,    -50,  -40}, /* Falcon */
    { -90,      -70,      -60,    -50,  -40 }, /* Kirin */
    { -90,      -80,      -70,    -50,  -40 }, /* Aardvark */
};

static struct
{
    NETAPP_WIFI_RSSI    tRSSI;
    const char          *pName;
} RSSIPrintTable [] =
{
   {  NETAPP_WIFI_RSSI_NONE ,           "No signal (0 bar)"     },
   {  NETAPP_WIFI_RSSI_POOR,            "Poor (1 bar)"          },
   {  NETAPP_WIFI_RSSI_FAIR,            "Fair (2 bars)"         },
   {  NETAPP_WIFI_RSSI_GOOD,            "Good (3 bars)"         },
   {  NETAPP_WIFI_RSSI_EXCELLENT,       "Excellent (4 bars)"    },
};


#define WPSCLI_API_CHECK(api, tRetCode)                                             \
{                                                                                   \
    int result = api;                                                               \
    switch(result)                                                                  \
    {                                                                               \
    case WPS_STATUS_SUCCESS:                                                        \
        NETAPP_MSG(("%s WPSCLI API[%s] Success!", __FUNCTION__, #api));             \
        tRetCode = NETAPP_SUCCESS;                                                  \
        break;                                                                      \
    case WPS_STATUS_ABORTED:                                                        \
        tRetCode = NETAPP_CANCELED;                                                 \
        NETAPP_MSG(("%s WPSCLI API[%s] Canceled!", __FUNCTION__, #api));            \
        break;                                                                      \
    default:                                                                        \
        NETAPP_ERR(("%s WPSCLI API[%s] failed!", __FUNCTION__, #api));              \
        tRetCode = NETAPP_FAILURE;                                                  \
        break;                                                                      \
    }                                                                               \
    if (tRetCode != NETAPP_SUCCESS)                                                 \
    {                                                                               \
        goto err_out;                                                               \
    }                                                                               \
}


#define BWL_API_CHECK(api, tRetCode)                                                \
{                                                                                   \
    int result = api;                                                               \
    switch(result)                                                                  \
    {                                                                               \
    case 0:                                                                         \
        tRetCode = NETAPP_SUCCESS;                                                  \
        break;                                                                      \
    case BWL_ERR_PARAM:                                                             \
        tRetCode = NETAPP_INVALID_PARAMETER;                                        \
        break;                                                                      \
    case BWL_ERR_ALLOC:                                                             \
        tRetCode = NETAPP_OUT_OF_MEMORY;                                            \
        break;                                                                      \
    case BWL_ERR_CANCELED:                                                          \
        tRetCode = NETAPP_CANCELED;                                                 \
        break;                                                                      \
    default:                                                                        \
        tRetCode = NETAPP_FAILURE;                                                  \
        break;                                                                      \
    }                                                                               \
    if ( (tRetCode != NETAPP_SUCCESS) && (tRetCode != NETAPP_CANCELED) )            \
    {                                                                               \
        NETAPP_ERR(("%s BWL API failed, Line=%d!", __FUNCTION__, __LINE__));        \
        goto err_out;                                                               \
    }                                                                               \
}


#define NETAPP_WIFI_PRIV_API_CHECK(handle)                                          \
{                                                                                   \
    if (handle == NULL)                                                             \
    {                                                                               \
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));                      \
        return NETAPP_NULL_PTR;                                                     \
    }                                                                               \
                                                                                    \
    if (!handle->bWiFiInserted)                                                     \
    {                                                                               \
        NETAPP_MSG(("%s WiFi not connected!", __FUNCTION__));                       \
        return NETAPP_SUCCESS;                                                      \
    }                                                                               \
}


typedef enum
{
    NETAPP_STATE_IDLE,
    NETAPP_STATE_CONNECTING,
    NETAPP_STATE_SCANNING,
} NETAPP_STATE;


typedef enum
{
    NETAPP_WIFI_EVENT_NONE          = 0x00,
    NETAPP_WIFI_EVENT_LINK_DOWN     = 0x01,
    NETAPP_WIFI_EVENT_LINK_UP       = 0x02,
    NETAPP_WIFI_EVENT_SCAN_DONE     = 0x04,
    NETAPP_WIFI_EVENT_ALL           = 0xff,
} NETAPP_WIFI_EVENT;

typedef struct
{
    NETAPP_LIST_NODE    Node;
    NETAPP_WIFI_AP_INFO tApInfo;
} NETAPP_WIFI_AP_NODE;

struct sNETAPP_WIFI
{
    BWL_Handle                  hBwl;
    bool                        bWiFiInserted;

    uint32_t                    tScanTimer;
    struct timespec             tScanTime;
    int32_t                     lScanInterval;
    uint32_t                    ulScanCount;
    NETAPP_WIFI_AP_INFO         *pScanResultsList;
    uint32_t                    ulScanResultsCount;
    uint32_t                    ulScanSem;
    NETAPP_SCAN_TYPE            tScanType;
    int32_t                     lScanTimeMs;

    uint32_t                    ulBkgrndTaskId;
    bool                        bStopBkgrndTask;
    NETAPP_STATE                tState;
    int32_t                     lBwlSocket;
    int32_t                     lBridgeSocket;
    char                        cBridgeName[NETAPP_IFACE_NAME_LEN+1];
    fd_set                      tFDSet;
    uint32_t                    ulMsgQId;
    bool                        bBurstScanResults;
    bool                        bIsSoftAp;
    bool                        bWiFiBridgingMode;
    bool                        bHideDuplicateAPs;
    NETAPP_WFI_HANDLE           hNetAppWiFiInvite;
    NETAPP_PRIV_HANDLE          hNetAppPriv;
    char                        cIfaceName[NETAPP_IFACE_NAME_LEN+1];
    NETAPP_LINK_STATE           tLinkState;

    NETAPP_DHCPCD_HANDLE        hNetAppDhcpcd;

    char                        *pCountryCode;

#ifndef NO_WIFI_WPS
    NETAPP_WPS_HANDLE           hNetAppWPS;
#endif

    NETAPP_P2P_HANDLE           hNetAppP2P;

    NETAPP_WIFI_AP_INFO         tCurApInfo;
    NETAPP_SETTINGS_HANDLE      hNetAppSettings;
    NETAPP_MIMO_BW_CAP          tMimoBwCap;
    NETAPP_WIFI_DRIVER          tDriverName;
};


static NETAPP_RETCODE NetAppWiFi_P_Connect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo);


static NETAPP_RETCODE NetAppWiFi_P_ProcessConnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_MSG_INFO     *pMessage);


static void NetAppWiFi_P_BackgroundTask(
    void              *pParam);


static void NetAppWiFi_P_PrintApInfo(
    NETAPP_WIFI_AP_INFO *pApInfo);


static void NetAppWiFi_P_ScanTimer(
    void            *pParam);


static NETAPP_RETCODE NetAppWiFi_P_ConvertBwlInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    ScanInfo_t          *pBwlScanInfo,
    NETAPP_WIFI_AP_INFO *pApInfo);


static NETAPP_WIFI_RSSI NetAppWiFi_P_ConvertRSSI(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    int32_t             lRSSI);


static NETAPP_RETCODE NetAppWiFi_P_SaveScanResults(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    ScanInfo_t         *pData,
    uint32_t            ulCount);


static NETAPP_RETCODE NetAppWiFi_P_WaitForEvent(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_EVENT   tEvent,
    uint32_t            uTimeout);


static NETAPP_RETCODE NetAppWiFi_P_ShutdownBackgroundTask(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


// function prototype declarations
void NetAppWiFi_P_QuickSort(
    NETAPP_WIFI_AP_INFO *items,
    int                 left,
    int                 right);


NETAPP_RETCODE NetAppWiFiPrivateSettingsRefresh(
    NETAPP_WIFI_HANDLE      hNetAppWiFi)
{
    NETAPP_SETTINGS tSettings;
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings), tRetCode);

    if (tSettings.bAutoP2PDiscover)
    {
        hNetAppWiFi->tScanType |= NETAPP_SCAN_P2P;
    }

    hNetAppWiFi->bHideDuplicateAPs  = tSettings.bHideDuplicateAPs;

    /* Someone changed the SoftAp mode so we have to restart WiFi */
    if (hNetAppWiFi->bIsSoftAp != tSettings.bIsSoftAp)
    {
        hNetAppWiFi->bIsSoftAp   = tSettings.bIsSoftAp;
        if (NetAppWiFiPrivateIsEnabled(hNetAppWiFi))
        {
            NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateHotPlug(hNetAppWiFi,
                hNetAppWiFi->cIfaceName, strlen(hNetAppWiFi->cIfaceName), false), tRetCode);
            NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateHotPlug(hNetAppWiFi,
                hNetAppWiFi->cIfaceName, strlen(hNetAppWiFi->cIfaceName), true), tRetCode);
        }
    }


    if (hNetAppWiFi->bWiFiBridgingMode != tSettings.bWiFiBridgingMode)
    {
        hNetAppWiFi->bWiFiBridgingMode = tSettings.bWiFiBridgingMode;
        /* WET mode does not seem to be enabled or supported in Kirin for now */
        if (strcasestr(WIFI_DRIVER_VERSION, "falcon"))
        {
            BWL_API_CHECK(BWL_SetBridgeMode(hNetAppWiFi->hBwl,tSettings.bWiFiBridgingMode), tRetCode);
        }
    }
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateInit(
    NETAPP_WIFI_HANDLE      *phNetAppWiFi,
    NETAPP_WFI_HANDLE       hNetAppWiFiInvite,
    NETAPP_P2P_HANDLE       hNetAppP2P,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings)
{
    char                cIfaceName[NETAPP_IFACE_NAME_LEN+1];
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_WIFI_HANDLE  hNetAppWiFi = NULL;
    FILE                *pFD        = NULL;
    bool                bFound      = false;
    char                cTmp[128];

    NETAPP_MSG(("%s(): -- Entered!", __FUNCTION__));
    if ( (phNetAppWiFi == NULL) || (hNetAppPriv == NULL) || (pInitSettings == NULL) || (pSettings == NULL) )
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppWiFi = (struct sNETAPP_WIFI*)NetAppOSMemAlloc(sizeof(struct sNETAPP_WIFI));
    if (hNetAppWiFi == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppWiFi, 0, sizeof(struct sNETAPP_WIFI));
    memset(cIfaceName, 0, sizeof(cIfaceName));

    NETAPP_PRIV_API_CHECK(NetAppOSTimerCreate(&hNetAppWiFi->tScanTimer, NetAppWiFi_P_ScanTimer, hNetAppWiFi), tRetCode);

    hNetAppWiFi->ulScanSem = NetAppOSSemBCreate();
    if (hNetAppWiFi->ulScanSem == 0)
    {
        NETAPP_ERR(("%s(): -- Cannot create sem!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    hNetAppWiFi->hNetAppWiFiInvite  = hNetAppWiFiInvite;
    hNetAppWiFi->hNetAppPriv        = hNetAppPriv;
    hNetAppWiFi->hNetAppDhcpcd      = hNetAppDhcpcd;
    hNetAppWiFi->hNetAppP2P         = hNetAppP2P;
    hNetAppWiFi->hNetAppSettings    = hNetAppSettings;
    hNetAppWiFi->bBurstScanResults  = pInitSettings->bBurstScanResults;
    hNetAppWiFi->bHideDuplicateAPs  = pSettings->bHideDuplicateAPs;
    hNetAppWiFi->bWiFiBridgingMode  = pSettings->bWiFiBridgingMode;
    hNetAppWiFi->tMimoBwCap         = pInitSettings->tMimoBwCap;

    if (pInitSettings->pCountryCode != NULL)
    {
        hNetAppWiFi->pCountryCode = strdup(pInitSettings->pCountryCode);
    }

    if (pSettings->bAutoP2PDiscover)
    {
        hNetAppWiFi->tScanType |= NETAPP_SCAN_P2P;
    }
    if (pInitSettings->WiFiIfacePrefix != NULL)
    {
        strncpy(hNetAppWiFi->cIfaceName, pInitSettings->WiFiIfacePrefix, strlen(pInitSettings->WiFiIfacePrefix));
    }

#ifndef NO_WIFI_WPS
    NETAPP_PRIV_API_CHECK(NetAppWiFiWPSPrivateInit(&hNetAppWiFi->hNetAppWPS,
        hNetAppPriv, pInitSettings, &hNetAppWiFi->bStopBkgrndTask), tRetCode);
#endif

    NETAPP_ERR(("WiFi: Running with %s Driver\n", WIFI_DRIVER_VERSION));

    /* Need to do an extra step first of downloading the firmware */
    if (strcasestr(WIFI_DRIVER_VERSION, "falcon"))
    {
        char cArgs[256];
        char cCmd[256];
        hNetAppWiFi->tDriverName = NETAPP_WIFI_DRIVER_FALCON;
        snprintf(cCmd, sizeof(cCmd), "%s/bin/%s", TARGET_DIR, NETAPP_BCMDL_CMD);
        snprintf(cArgs, sizeof(cArgs), "-n %s/%s %s/%s", TARGET_DIR, NETAPP_NVRAM_PATH, TARGET_DIR, NETAPP_FIRMWARE_PATH);
        snprintf(cTmp, sizeof(cTmp), "iface_name=%s",hNetAppWiFi->cIfaceName);

        if (NetAppPrivateSystem(cCmd, cArgs, NULL, NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): %s Failed", __FUNCTION__, NETAPP_BCMDL_CMD));
            //tRetCode = NETAPP_FAILURE;
            //goto err_out;
        }

    }
    /* SDIO WiFi Driver */
    else if (strcasestr(WIFI_DRIVER_VERSION, "5_90_195"))
    {
#if 1
        char cArgs[256];
        if (NetAppPrivateInsmod(WIFI_MODULES, WIFI_MODULES_INSTALLED, "iface_name=wln") != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s() -- Failed to insmod WiFi USB kernel module(s) %s!", __FUNCTION__, WIFI_MODULES_INSTALLED));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
        hNetAppWiFi->tDriverName = NETAPP_WIFI_DRIVER_FALCON;
        snprintf(cArgs, sizeof(cArgs), " -i wln0 sd_divisor 0");

        if (NetAppPrivateSystem(NETAPP_DHDMIPS_CMD, cArgs, NULL, NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): %s Failed", __FUNCTION__, NETAPP_BCMDL_CMD));
            //tRetCode = NETAPP_FAILURE;
            //goto err_out;
        }
        snprintf(cArgs, sizeof(cArgs), "-i wln0 download /usr/local/bin/rtecdc.bin /usr/local/bin/bcm943362sdg.txt");

        if (NetAppPrivateSystem(NETAPP_DHDMIPS_CMD, cArgs, NULL, NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): %s Failed", __FUNCTION__, NETAPP_BCMDL_CMD));
            //tRetCode = NETAPP_FAILURE;
            //goto err_out;
        }
#endif
    }
    else if (strcasestr(WIFI_DRIVER_VERSION, "aardvark"))
    {
        snprintf(cTmp, sizeof(cTmp), "intf_name=%s",hNetAppWiFi->cIfaceName);
        hNetAppWiFi->tDriverName = NETAPP_WIFI_DRIVER_AARDVARK;
    }
    else
    {
        snprintf(cTmp, sizeof(cTmp), "name=%s",hNetAppWiFi->cIfaceName);
        hNetAppWiFi->tDriverName = NETAPP_WIFI_DRIVER_KIRIN;
    }

    if (NetAppPrivateInsmod(WIFI_MODULES, WIFI_MODULES_INSTALLED, cTmp) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() -- Failed to insmod WiFi USB kernel module(s) %s!", __FUNCTION__, WIFI_MODULES_INSTALLED));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }


    if (NetAppWiFiPrivateIsPresent(cIfaceName, sizeof(cIfaceName)) == NETAPP_SUCCESS)
    {
        /* Do hotplug */
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateHotPlug(hNetAppWiFi, cIfaceName, sizeof(cIfaceName), true), tRetCode);
    }

    *phNetAppWiFi = hNetAppWiFi;
err_out:
    if (pFD != NULL)
    {
        fclose(pFD);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateDeinit(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    if (hNetAppWiFi->pCountryCode != NULL)
    {
        NetAppOSMemFree(hNetAppWiFi->pCountryCode);
    }

    NetAppWiFi_P_ShutdownBackgroundTask(hNetAppWiFi);

    if (hNetAppWiFi->bWiFiInserted)
    {
        /* Stop the RSSI timer in the Driver */
        BWL_SetRSSIEventLevels(hNetAppWiFi->hBwl, NULL, 0);

        /* Configure the WoWL if the dongle is still connected and plugged in */
        if ( (NetAppWiFiPrivateIsPresent(NULL, 0) == NETAPP_SUCCESS) && (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN) )
        {
            NETAPP_SETTINGS tSettings;

#ifndef NO_WIFI_P2P
        NetAppWiFiP2PPrivateHotPlug(hNetAppWiFi->hNetAppP2P, hNetAppWiFi->cIfaceName, strlen(hNetAppWiFi->cIfaceName), false);
#endif

            if ( (NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings) == NETAPP_SUCCESS) &&
                 (tSettings.tWoWLSettings.ulMask != NETAPP_WOWL_EVENT_NONE) )
            {
                uint32_t ulMask = 0;

                ulMask |= (tSettings.tWoWLSettings.ulMask & NETAPP_WOWL_EVENT_MAGIC_PATTERN)    ? eWoWLMask_Magic       : 0;
                ulMask |= (tSettings.tWoWLSettings.ulMask & NETAPP_WOWL_EVENT_DISASSOC_DEAUTH)  ? eWoWLMask_Disassoc    : 0;
                ulMask |= (tSettings.tWoWLSettings.ulMask & NETAPP_WOWL_EVENT_LOSS_OF_BEACON)   ? eWoWLMask_LossOfBeacon: 0;
                ulMask |= (tSettings.tWoWLSettings.ulMask & NETAPP_WOWL_EVENT_NET_PATTERN)      ? eWoWLMask_NetPattern  : 0;

                NETAPP_MSG(("%s(): Setting mask to %d!", __FUNCTION__, ulMask));
                BWL_WoWLSetMask(hNetAppWiFi->hBwl, ulMask);

                if ( (tSettings.tWoWLSettings.ulMask & NETAPP_WOWL_EVENT_NET_PATTERN) == NETAPP_WOWL_EVENT_NET_PATTERN)
                {
                    int i;
                    for (i = 0; i < NETAPP_WOWL_MAX_NET_PATTERNS; i++)
                    {
                        if (tSettings.tWoWLSettings.tNetPattern[i].cValue[0] != '\0')
                        {
                            NETAPP_MSG(("%s(): Adding Net Pattern[%s] Mask[%s] Offset[%d]", __FUNCTION__,
                                tSettings.tWoWLSettings.tNetPattern[i].cValue, tSettings.tWoWLSettings.tNetPattern[i].cMask,
                                tSettings.tWoWLSettings.tNetPattern[i].ulOffset));

                            BWL_WoWLPatternAdd(hNetAppWiFi->hBwl, tSettings.tWoWLSettings.tNetPattern[i].ulOffset,
                                tSettings.tWoWLSettings.tNetPattern[i].cMask, tSettings.tWoWLSettings.tNetPattern[i].cValue);
                        }
                    }
                }
                NETAPP_ERR(("%s(): Activating WoWL!", __FUNCTION__));
                BWL_WoWLActivate(hNetAppWiFi->hBwl);
            }
        }
        NetAppWiFiPrivateHotPlug(hNetAppWiFi, hNetAppWiFi->cIfaceName, strlen(hNetAppWiFi->cIfaceName), false);
        hNetAppWiFi->tLinkState = NETAPP_LINK_DOWN;
    }

    if (hNetAppWiFi->tScanTimer != 0)
    {
        NetAppOSTimerDelete(hNetAppWiFi->tScanTimer);
    }

    if (hNetAppWiFi->ulScanSem)
    {
        NetAppOSSemDelete(hNetAppWiFi->ulScanSem);
    }

#ifndef NO_WIFI_WPS
    NetAppWiFiWPSPrivateDeInit(hNetAppWiFi->hNetAppWPS);
#endif

    NetAppOSMemFree(hNetAppWiFi);

    hNetAppWiFi = NULL;

    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppWiFiPrivateSetInterfaceUp(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    bool                bIsUp)
{
    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    return ((bIsUp ? BWL_Up(hNetAppWiFi->hBwl) : BWL_Down(hNetAppWiFi->hBwl)) == 0) ? NETAPP_SUCCESS : NETAPP_FAILURE;
}


NETAPP_RETCODE NetAppWiFiPrivateConnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_CONNECT_TYPE tConnectType,
    void                *pParam,
    uint32_t            ulWpsPin)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    NETAPP_MSG_INFO tMessage;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    /* Cancel any current scan or connect attempt */
    if (hNetAppWiFi->tState != NETAPP_STATE_IDLE)
    {
        hNetAppWiFi->bStopBkgrndTask = true;
    }

#ifndef NO_WIFI_INVITE
    NetAppWiFiInvitePrivateStop(hNetAppWiFi->hNetAppWiFiInvite);
    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(hNetAppWiFi, NETAPP_SCAN_INVITE), tRetCode);
#endif

    memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

    tMessage.tMsgType       = NETAPP_MSG_CONNECT;
    tMessage.tConnectType   = tConnectType;
    tMessage.ulData0        = ulWpsPin;

    switch(tConnectType)
    {
    case NETAPP_CONNECT_NORMAL:
        memcpy(&tMessage.tData.tApInfo, pParam, sizeof(NETAPP_WIFI_AP_INFO));
        break;

    case NETAPP_CONNECT_INVITE_ACCEPT:
    case NETAPP_CONNECT_INVITE_REJECT:
    case NETAPP_CONNECT_WPS_PIN_ENROLLEE:
    case NETAPP_CONNECT_WPS_PIN_REGISTRAR:
    case NETAPP_CONNECT_P2P:
        if (pParam != NULL)
        {
            strncpy(tMessage.tData.tApInfo.cSSID, pParam,
                sizeof(tMessage.tData.tApInfo.cSSID)/sizeof(tMessage.tData.tApInfo.cSSID[0]) -1 );
        }
        break;
    default:
        break;
    }

    if (hNetAppWiFi->tState == NETAPP_STATE_CONNECTING)
    {
        hNetAppWiFi->bStopBkgrndTask = true;
    }

    tRetCode = NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateStartScan(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    int32_t             lTickMs,
    int32_t             lScanTimeMs,
    NETAPP_SCAN_TYPE    tType,
    NETAPP_P2P_DISCOVER_PARAMS *pParams)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    NETAPP_PRIV_SEM_TAKE(hNetAppWiFi->ulScanSem, tRetCode);

    hNetAppWiFi->ulScanCount    = 0;

    // Make the ScanTime the highest for any active scans
    if (hNetAppWiFi->lScanTimeMs < lScanTimeMs)
    {
        hNetAppWiFi->lScanTimeMs = lScanTimeMs;
    }

    if (hNetAppWiFi->tState != NETAPP_STATE_SCANNING)
    {
        if (hNetAppWiFi->tScanType != NETAPP_SCAN_NONE)
        {
            NetAppOSTimerStop(hNetAppWiFi->tScanTimer);
        }

        NetAppOSTimerSetMsec(hNetAppWiFi->tScanTimer, NETAPP_WIFI_WAIT_MS);
    }

    hNetAppWiFi->lScanInterval = lTickMs;

    hNetAppWiFi->tScanType |= tType;

    if (hNetAppWiFi->tScanType == NETAPP_SCAN_P2P)
    {
        NETAPP_MSG_INFO     tMessage;

        memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

        tMessage.tMsgType = NETAPP_MSG_P2P;

        if (NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Failure sending scan request!", __FUNCTION__));
        }

    }

    hNetAppWiFi->tScanType |= tType;

err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppWiFi->ulScanSem);
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateFetchApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    char                *pSSID)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    NETAPP_MSG_INFO tMessage;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

    tMessage.tMsgType       = NETAPP_MSG_FETCH_APINFO;
    strncpy(tMessage.tData.tApInfo.cSSID, pSSID, sizeof(tMessage.tData.tApInfo.cSSID)/sizeof(tMessage.tData.tApInfo.cSSID[0]) -1);

    return NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER);
}


NETAPP_RETCODE NetAppWiFiPrivateStopScan(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_SCAN_TYPE    tType)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    NETAPP_PRIV_SEM_TAKE(hNetAppWiFi->ulScanSem, tRetCode);
    hNetAppWiFi->tScanType &= ~(tType);

    if (tType == NETAPP_SCAN_P2P)
    {
#ifndef NO_WIFI_P2P
        NetAppWiFiP2PPrivateStopDiscovery(hNetAppWiFi->hNetAppP2P);
#endif
    }

    if ( (hNetAppWiFi->tScanType & ~NETAPP_SCAN_P2P) == NETAPP_SCAN_NONE)
    {
        NETAPP_MSG(("%s(): Stopping scanning backend!", __FUNCTION__));
        if (hNetAppWiFi->tState == NETAPP_STATE_SCANNING)
        {
            hNetAppWiFi->bStopBkgrndTask = true;
            BWL_EScanAbort(hNetAppWiFi->hBwl);
            BWL_API_CHECK(BWL_ClearEvent(hNetAppWiFi->hBwl, BWL_E_SCAN_COMPLETE), tRetCode);
        }

        NetAppOSTimerStop(hNetAppWiFi->tScanTimer);

        if (hNetAppWiFi->pScanResultsList != NULL)
        {
            NetAppOSMemFree(hNetAppWiFi->pScanResultsList);
            hNetAppWiFi->pScanResultsList = NULL;
        }
        hNetAppWiFi->lScanInterval  = 0;
        hNetAppWiFi->lScanTimeMs    = -1;
    }

err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppWiFi->ulScanSem);
    return tRetCode;
}


bool NetAppWiFiPrivateIsEnabled(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    if (hNetAppWiFi != NULL)
    {
        return hNetAppWiFi->bWiFiInserted;
    }
    return false;
}


NETAPP_RETCODE NetAppWiFiPrivateIsPresent(
    char                *pIfName,
    uint32_t            ulLength)
{
    uint32_t       ulIsPresent = 0;

     return ( (BWL_IsPresent(&ulIsPresent, pIfName, ulLength) == BWL_ERR_SUCCESS) && ulIsPresent) ?
         NETAPP_SUCCESS : NETAPP_FAILURE;
}


NETAPP_RETCODE NetAppWiFiPrivateDisconnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    NETAPP_MSG_INFO tMessage;
    tMessage.tMsgType = NETAPP_MSG_DISCONNECT;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);


    if (hNetAppWiFi->tState == NETAPP_STATE_CONNECTING)
    {
        hNetAppWiFi->bStopBkgrndTask = true;
        BWL_EScanAbort(hNetAppWiFi->hBwl);
    }

    return NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER);
}


NETAPP_RETCODE NetAppWiFiPrivateGeneratePin(
    uint32_t            *pulPin)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    char            cPin[NETAPP_WPS_PIN_SIZE+1];

    if (pulPin == NULL)
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    memset(cPin, 0, sizeof(cPin));
#ifndef NO_WIFI_WPS
    if (brcm_wpscli_generate_pin(cPin, sizeof(cPin)) != 0)
    {
        NETAPP_ERR(("%s Failed to Generate pin", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }
#endif
    *pulPin = atoi(cPin);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateValidatePin(
    uint32_t            ulPin)
{
#ifndef NO_WIFI_WPS
    return (wps_validateChecksum(ulPin) == 0) ? NETAPP_SUCCESS : NETAPP_FAILURE;
#else
    return NETAPP_FAILURE;
#endif
}

NETAPP_RETCODE NetAppWiFiPrivateHotPlug(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    const char          *pIFaceName,
    uint32_t            ulIfaceLength,
    bool                bInserted)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_MSG(("%s(): Entered %s, %s!", __FUNCTION__, pIFaceName, bInserted ? "INSERTED" : "REMOVED"));

    if (hNetAppWiFi == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return NETAPP_NULL_PTR;
    }

    if (bInserted)
    {
        /* Ignore duplicate insertion notifications that happen on coming out of standby */
        if (!hNetAppWiFi->bWiFiInserted)
        {
            unsigned int i;

            strncpy(hNetAppWiFi->cIfaceName, pIFaceName, ulIfaceLength);

            /* Try to init the BWL interface */
            for (i = 0; i < NETAPP_BWL_INIT_RETRY; i++)
            {
                if (BWL_Init(&hNetAppWiFi->hBwl) == BWL_ERR_SUCCESS)
                {
                    break;
                }
                NetAppOSTaskDelayMsec(NETAPP_WIFI_WAIT_MS*10);
            }

            if (hNetAppWiFi->hBwl == NULL)
            {
                NETAPP_ERR(("%s() Failed to Init BWL!", __FUNCTION__));
                tRetCode = NETAPP_FAILURE;
                goto err_out;
            }
            else
            {
                RevInfo_t   tRevInfo;
                char        tCountry[4];
                int32_t     lRegRev = -1;
                uint32_t	ulIsUp = 1;
                NETAPP_SETTINGS tSettings;

                memset(&tRevInfo, 0, sizeof(RevInfo_t));
                BWL_API_CHECK(BWL_SetRSSIEventLevels(hNetAppWiFi->hBwl, lRSSILevels[hNetAppWiFi->tDriverName],
                    sizeof(lRSSILevels[hNetAppWiFi->tDriverName])/sizeof(lRSSILevels[hNetAppWiFi->tDriverName][0])), tRetCode);
                BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_RSSI), tRetCode);
                BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_ASSOC), tRetCode);
                BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_DISASSOC), tRetCode);

                BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_LINK), tRetCode);
                BWL_SetOBSSCoEx(hNetAppWiFi->hBwl, true);
                hNetAppWiFi->bWiFiInserted  = true;

                BWL_API_CHECK(BWL_GetRevInfo(hNetAppWiFi->hBwl, &tRevInfo), tRetCode);
                NETAPP_MSG(("*******************************"));
                NETAPP_MSG(("WiFi Driver Version Info:"));
                NETAPP_MSG(("Chip Number:       %d", tRevInfo.ulChipNum));
                NETAPP_MSG(("Driver Version:    %d.%d.%d.%d",
                    (uint8_t)(tRevInfo.ulDriverRev>>24),
                    (uint8_t)(tRevInfo.ulDriverRev>>16),
                    (uint8_t)(tRevInfo.ulDriverRev>>8),
                    (uint8_t)tRevInfo.ulDriverRev));
                NETAPP_MSG(("PCI vendor id:     0x%08x", tRevInfo.ulVendorId));
                NETAPP_MSG(("Device id of chip: 0x%08x", tRevInfo.ulDeviceId));
                NETAPP_MSG(("Radio Revision:    0x%08x", tRevInfo.ulRadioRev));
                NETAPP_MSG(("Chip Revision:     0x%08x", tRevInfo.ulChipRev));
                NETAPP_MSG(("Core Revision:     0x%08x", tRevInfo.ulCoreRev));
                NETAPP_MSG(("Board Identifier:  0x%08x", tRevInfo.ulBoardId));
                NETAPP_MSG(("Board Vendor:      0x%08x", tRevInfo.ulBoardVendor));
                NETAPP_MSG(("Board Revision:    0x%08x", tRevInfo.ulBoardRev));
                NETAPP_MSG(("Microcode Version: 0x%08x", tRevInfo.ulUcodeRev));
                NETAPP_MSG(("Bus Type:          0x%08x", tRevInfo.ulBus));
                NETAPP_MSG(("Phy Type:          0x%08x", tRevInfo.ulPhyType));
                NETAPP_MSG(("Phy Revision:      0x%08x", tRevInfo.ulPhyRev));
                NETAPP_MSG(("Anacore Rev:       0x%08x", tRevInfo.ulAnaRev));
                NETAPP_MSG(("Chip Package Info: 0x%08x", tRevInfo.ulChipPkg));

                if ( ((uint8_t)(tRevInfo.ulDriverRev>>24) != EPI_MAJOR_VERSION) ||
                    ((uint8_t)(tRevInfo.ulDriverRev>>16) != EPI_MINOR_VERSION) ||
                    ((uint8_t)(tRevInfo.ulDriverRev>>8) != EPI_RC_NUMBER) )
                {
                    NETAPP_ERR(("********************************************************"));
                    NETAPP_ERR(("WLAN DRIVER/LIBRARY Missmatch! \nDriver Ver:\t%d.%d.%d.%d \nLibrary Ver:\t%d.%d.%d.%d",
                        (uint8_t)(tRevInfo.ulDriverRev>>24), (uint8_t)(tRevInfo.ulDriverRev>>16),
                        (uint8_t)(tRevInfo.ulDriverRev>>8), (uint8_t)tRevInfo.ulDriverRev,
                        EPI_MAJOR_VERSION, EPI_MINOR_VERSION, EPI_RC_NUMBER, EPI_INCREMENTAL_NUMBER));
                    NETAPP_ERR(("********************************************************"));
                    tRetCode = NETAPP_FAILURE;
                    goto err_out;
                }

                /* Configure the Country Code (if one exists) */
                memset(&tCountry, 0, sizeof(tCountry));
                if (hNetAppWiFi->pCountryCode != NULL)
                {
                    if (BWL_SetCountry(hNetAppWiFi->hBwl, hNetAppWiFi->pCountryCode) != BWL_ERR_SUCCESS)
                    {
                        NETAPP_ERR(("%s(): Unable to set country code %s", __FUNCTION__, hNetAppWiFi->pCountryCode));
                    }
                }

                BWL_API_CHECK(BWL_GetCountry(hNetAppWiFi->hBwl, tCountry, &lRegRev), tRetCode);
                NETAPP_MSG(("Country Code:      %s/%d", tCountry, lRegRev));
                NETAPP_MSG(("*******************************"));

                i = 0;
                do
                {
                    NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, false);
                    NetAppOSTaskDelayMsec(NETAPP_WIFI_WAIT_MS);
                	BWL_IsUp(hNetAppWiFi->hBwl, &ulIsUp);
                    i++;
                } while (((i < 2) || ulIsUp) && ((i*NETAPP_WIFI_WAIT_MS) < NETAPP_WIFI_INIT_TIMEOUT));
                NETAPP_MSG(("%s(): Took %dms to go down", __FUNCTION__, (i*NETAPP_WIFI_WAIT_MS)));

                BWL_API_CHECK(BWL_SetMimoBwCap(hNetAppWiFi->hBwl, hNetAppWiFi->tMimoBwCap), tRetCode);
                NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings), tRetCode);

                /* WET mode does not seem to be enabled or supported in Kirin for now */
                if (strcasestr(WIFI_DRIVER_VERSION, "falcon"))
                {
                    BWL_API_CHECK(BWL_SetBridgeMode(hNetAppWiFi->hBwl,tSettings.bWiFiBridgingMode), tRetCode);
                }
#ifndef NO_WIFI_P2P
                NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateHotPlug(hNetAppWiFi->hNetAppP2P, pIFaceName, ulIfaceLength, bInserted), tRetCode);
#endif
                if (!hNetAppWiFi->bIsSoftAp)
                {
                    hNetAppWiFi->ulMsgQId = NetAppOSMsgQCreate(NETAPP_WIFI_QUEUE_SIZE, sizeof(NETAPP_MSG_INFO));
                    if (hNetAppWiFi->ulMsgQId == 0)
                    {
                        NETAPP_ERR(("%s(): Failed creating message queue!", __FUNCTION__));
                        tRetCode = NETAPP_FAILURE;
                        goto err_out;
                    }

                    hNetAppWiFi->ulBkgrndTaskId = NetAppOSTaskSpawn("NetAppWiFi_P_BackgroundTask", NETAPP_OS_PRIORITY_LOW,
                           256*1024, NetAppWiFi_P_BackgroundTask, hNetAppWiFi);
                    if (hNetAppWiFi->ulBkgrndTaskId == 0)
                    {
                        NETAPP_ERR(("%s() Cannot create BackgroundTask\n", __FUNCTION__));
                        tRetCode = NETAPP_FAILURE;
                        goto err_out;
                    }
                }
            }
        }
    }
    else
    {
        NetAppWiFi_P_ShutdownBackgroundTask(hNetAppWiFi);

#ifndef NO_WIFI_P2P
        NetAppWiFiP2PPrivateHotPlug(hNetAppWiFi->hNetAppP2P, pIFaceName, ulIfaceLength, bInserted);
#endif
        if (hNetAppWiFi->hBwl != NULL)
        {
            BWL_Uninit(hNetAppWiFi->hBwl);
            hNetAppWiFi->hBwl = NULL;
        }

        hNetAppWiFi->bWiFiInserted  = false;
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateGetConnectedApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_RETCODE       tRetCode     = NETAPP_SUCCESS;
    ScanInfo_t          tBwlScanInfo;


    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s, Null Handle!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    memset(&tBwlScanInfo, 0, sizeof(tBwlScanInfo));
    memset(pApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));
    BWL_API_CHECK(BWL_GetConnectedInfo(hNetAppWiFi->hBwl, &tBwlScanInfo), tRetCode);

    NetAppWiFi_P_ConvertBwlInfo(hNetAppWiFi, &tBwlScanInfo, pApInfo);

    BWL_API_CHECK(BWL_GetRate(hNetAppWiFi->hBwl, &pApInfo->lRate), tRetCode);

    if (hNetAppWiFi->tCurApInfo.cPassword[0] != '\0')
    {
        strncpy(pApInfo->cPassword, hNetAppWiFi->tCurApInfo.cPassword,
            sizeof(pApInfo->cPassword)/sizeof(pApInfo->cPassword[0]));
    }
    NetAppWiFi_P_PrintApInfo(pApInfo);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateGetScannedApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_RETCODE       tRetCode   = NETAPP_SUCCESS;
    ScanInfo_t          tBwlScanInfo;
    ScanInfo_t          *pData      = NULL;
    uint32_t            ulCount     = 0;
    uint32_t            i           = 0;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s, Null Handle!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    memset(&tBwlScanInfo, 0, sizeof(tBwlScanInfo));


    BWL_API_CHECK(BWL_GetScannedApNum(hNetAppWiFi->hBwl, &ulCount), tRetCode);
    NETAPP_MSG(("%s Scanned %d Access Points", __FUNCTION__, ulCount));

    if (ulCount <= 0)
    {
        NETAPP_ERR(("%s Nothing Scanned !", __FUNCTION__));
        tRetCode = NETAPP_SUCCESS;
        goto err_out;
    }

    pData = (ScanInfo_t*)NetAppOSMemAlloc(ulCount * sizeof(ScanInfo_t));

    if (pData == NULL)
    {
        NETAPP_ERR(("%s Out of memory!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(pData, 0x0, (ulCount * sizeof(ScanInfo_t)));

    BWL_API_CHECK(BWL_GetScanResults(hNetAppWiFi->hBwl, pData), tRetCode);

    /* Fill the NetApp structure */
    for (i = 0; i < ulCount; i++)
    {
        if (strncmp(pData[i].tCredentials.acSSID, pApInfo->cSSID, sizeof(pApInfo->cSSID)/sizeof(pApInfo->cSSID[0])-1) == 0)
        {
            NETAPP_PRIV_API_CHECK(NetAppWiFi_P_ConvertBwlInfo(hNetAppWiFi, &pData[i], pApInfo), tRetCode);
            break;
        }
    }

err_out:
    if (pData != NULL)
    {
        NetAppOSMemFree(pData);
    }
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateGetScanResults(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO **pApInfoList,
    uint32_t            *pulScanCount)
{
    ScanInfo_t          *pData      = NULL;
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    unsigned int        i           = 0;
    uint32_t            ulCount     = 0;
    NETAPP_WIFI_AP_INFO *pApInfo    = NULL;
    ScanInfo_t          tCurrentBwlApInfo;


    if ( (pApInfoList == NULL) || (pulScanCount == NULL) )
    {
        NETAPP_ERR(("%s(): Null Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    NETAPP_PRIV_SEM_TAKE(hNetAppWiFi->ulScanSem, tRetCode);

    if (hNetAppWiFi->pScanResultsList == NULL)
    {
        NETAPP_MSG(("%s(): No Results Available!!", __FUNCTION__));
        tRetCode = NETAPP_SCAN_EMPTY;
    }
    else
    {
        *pApInfoList = (NETAPP_WIFI_AP_INFO*)NetAppOSMemAlloc(hNetAppWiFi->ulScanResultsCount * sizeof(NETAPP_WIFI_AP_INFO));
        memcpy(*pApInfoList, hNetAppWiFi->pScanResultsList, (hNetAppWiFi->ulScanResultsCount * sizeof(NETAPP_WIFI_AP_INFO)) );
        *pulScanCount = hNetAppWiFi->ulScanResultsCount;
    }

    NETAPP_MSG(("%s(): Sending %d Scanned AP's!", __FUNCTION__, hNetAppWiFi->ulScanResultsCount));

    NETAPP_PRIV_SEM_GIVE(hNetAppWiFi->ulScanSem);
err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


char* NetAppWiFiPrivateGetIfaceName(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    return (hNetAppWiFi != NULL) ? hNetAppWiFi->cIfaceName : NULL;
}


NETAPP_LINK_STATE NetAppWiFiPrivateGetLinkState(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    return ((hNetAppWiFi != NULL) ? hNetAppWiFi->tLinkState : NETAPP_LINK_DOWN);
}



NETAPP_RETCODE NetAppWiFiPrivateSetNetworkSettings(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_MSG_INFO tMessage;
    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

    tMessage.tMsgType       = NETAPP_MSG_SET_SETTINGS;
    tMessage.ulData0        = tMode;

    if (pSettings != NULL)
    {
        tMessage.tData.tSettings= *pSettings;
    }
    return NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER);

}


NETAPP_RETCODE NetAppWiFiPrivateJoinBridge(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    const char          *pBridgeName)
{
    struct sockaddr_ll  sll;
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    NETAPP_PRIV_HANDLE_CHECK(pBridgeName);

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    if (hNetAppWiFi->lBridgeSocket != 0)
    {
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateLeaveBridge(hNetAppWiFi),tRetCode);
    }

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, true), tRetCode);

    strncpy(hNetAppWiFi->cBridgeName, pBridgeName, sizeof(hNetAppWiFi->cBridgeName)/sizeof(hNetAppWiFi->cBridgeName[0])-1);

    hNetAppWiFi->lBridgeSocket = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE_BRCM));
    if (hNetAppWiFi->lBridgeSocket < 0)
    {
        NETAPP_ERR(("%s() Error creating socket: %s ", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETHER_TYPE_BRCM);
    sll.sll_ifindex = if_nametoindex(pBridgeName);

    if (bind(hNetAppWiFi->lBridgeSocket, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        NETAPP_ERR(("%s() Unable to bind socket: %s ", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

#ifndef NO_WIFI_WPS
    NETAPP_PRIV_API_CHECK(NetAppWiFiWPSUpdateIfaceName(hNetAppWiFi->hNetAppWPS, pBridgeName), tRetCode);
#endif
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateLeaveBridge(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    if (hNetAppWiFi->lBridgeSocket != 0)
    {
        close(hNetAppWiFi->lBridgeSocket);
        hNetAppWiFi->lBridgeSocket = 0;
        memset(hNetAppWiFi->cBridgeName, 0, sizeof(hNetAppWiFi->cBridgeName));
    }
#ifndef NO_WIFI_WPS
    NetAppWiFiWPSUpdateIfaceName(hNetAppWiFi->hNetAppWPS, NULL);
#endif
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppWiFiPrivateSetAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t ulAntennas)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    uint32_t numAntennas;
    AntMode_t antMode;

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetAntennaCount(hNetAppWiFi, &numAntennas), tRetCode);

    if ((ulAntennas >> numAntennas) & 0xFF)
    {
        /* the requested combination includes an antenna id that is beyond what was found in the system */
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    antMode = (NETAPP_WIFI_ANTENNA_MODE_RX == tMode) ? eAntModeRx : eAntModeTx;
    BWL_API_CHECK(BWL_SetAntenna(hNetAppWiFi->hBwl, antMode, ulAntennas), tRetCode);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateGetAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t *pulAntennas)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    uint32_t antennas = *pulAntennas;
    AntMode_t antMode;

    antMode = (NETAPP_WIFI_ANTENNA_MODE_RX == tMode) ? eAntModeRx : eAntModeTx;
    BWL_API_CHECK(BWL_GetAntenna(hNetAppWiFi->hBwl, antMode, pulAntennas), tRetCode);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiPrivateGetAntennaCount(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    uint32_t *pulAntenna)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    BWL_API_CHECK(BWL_GetAntennaCount(hNetAppWiFi->hBwl, pulAntenna), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestPrivateStartTransmission(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_TXRX_SETTINGS *pTxSettings)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    ChanSpec_t chanSpec;
    PacketEngineParameters_t pktEng;
    Credential_t cred;

    BWL_API_CHECK(BWL_SetMimoBwCap(hNetAppWiFi->hBwl, eMimoBwCap40MHz_BothBands), tRetCode);

    if (NETAPP_WIFI_BANDWIDTH_20MHz == pTxSettings->bandwidth)
    {
        BWL_API_CHECK(BWL_SetMimoTx(hNetAppWiFi->hBwl, eMimoTxBw_20MHz), tRetCode);
    }
    else if (NETAPP_WIFI_BANDWIDTH_40MHz == pTxSettings->bandwidth)
    {
        BWL_API_CHECK(BWL_SetMimoTx(hNetAppWiFi->hBwl, eMimoTxBw_40MHz), tRetCode);
    }

    chanSpec.ulChan = pTxSettings->channel;
    chanSpec.eBand = eBand2G;
    chanSpec.eBandwidth = (NETAPP_WIFI_BANDWIDTH_20MHz == pTxSettings->bandwidth) ? eBandwidth20MHz : eBandwidth40MHz;
    if (eBandwidth20MHz == chanSpec.eBandwidth)
    {
        chanSpec.eSideband = eSideband_None;
    }
    else
    {
        chanSpec.eSideband = (NETAPP_WIFI_SIDEBAND_LOWER == pTxSettings->sideband) ? eSideband_Lower : eSideband_Upper;
    }
    BWL_API_CHECK(BWL_SetChanspec(hNetAppWiFi->hBwl, &chanSpec), tRetCode);

    BWL_API_CHECK(BWL_Up(hNetAppWiFi->hBwl), tRetCode);

    if (NETAPP_WIFI_802_11_MODE_A == pTxSettings->protocol)
    {
        BWL_API_CHECK(BWL_SetBand(hNetAppWiFi->hBwl, eBand5G), tRetCode);
    }
    else
    {
        BWL_API_CHECK(BWL_SetBand(hNetAppWiFi->hBwl, eBand2G), tRetCode);
    }

    BWL_API_CHECK(BWL_SetDataRate(hNetAppWiFi->hBwl, pTxSettings->data_rate), tRetCode);

    BWL_API_CHECK(BWL_SetMcsIndex(hNetAppWiFi->hBwl, pTxSettings->mcsindex), tRetCode);

    BWL_API_CHECK(BWL_SetTransmitPower(hNetAppWiFi->hBwl, pTxSettings->transmit_power), tRetCode);

    memset(&cred, 0, sizeof(Credential_t));
    strcpy(cred.acSSID, "aaa");
    BWL_API_CHECK(BWL_ConnectAp(hNetAppWiFi->hBwl, &cred), tRetCode);

    memset(&pktEng, 0, sizeof(PacketEngineParameters_t));
    pktEng.bStart = true;
    pktEng.bTxMode = true;
    pktEng.ulIFG = pTxSettings->inter_packet_gap_us;
    pktEng.ulPacketSize = pTxSettings->packet_size;
    pktEng.ulNumFrames = 0; /* indicates request for continuous TX mode */
    strncpy(pktEng.acMac, "00:11:22:33:44:55:66", MAC_ADDR_LEN); /* dummy MAC address used in TX mode */
    BWL_API_CHECK(BWL_ConfigurePacketEngine(hNetAppWiFi->hBwl, &pktEng), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestPrivateStopTransmission(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    PacketEngineParameters_t pktEng;

    memset(&pktEng, 0, sizeof(PacketEngineParameters_t));
    pktEng.bStart = false;
    pktEng.bTxMode = true;
    BWL_API_CHECK(BWL_ConfigurePacketEngine(hNetAppWiFi->hBwl, &pktEng), tRetCode);
    BWL_API_CHECK(BWL_Down(hNetAppWiFi->hBwl), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestPrivateStartReceiving(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_TXRX_SETTINGS *pRxSettings)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    ChanSpec_t chanSpec;
    PacketEngineParameters_t pktEng;

    BWL_API_CHECK(BWL_SetMimoBwCap(hNetAppWiFi->hBwl, eMimoBwCap40MHz_BothBands), tRetCode);

    if (NETAPP_WIFI_BANDWIDTH_20MHz == pRxSettings->bandwidth)
    {
        BWL_API_CHECK(BWL_SetMimoTx(hNetAppWiFi->hBwl, eMimoTxBw_20MHz), tRetCode);
    }
    else if (NETAPP_WIFI_BANDWIDTH_40MHz == pRxSettings->bandwidth)
    {
        BWL_API_CHECK(BWL_SetMimoTx(hNetAppWiFi->hBwl, eMimoTxBw_40MHz), tRetCode);
    }

    chanSpec.ulChan = pRxSettings->channel;
    chanSpec.eBand = eBand2G;
    chanSpec.eBandwidth = (NETAPP_WIFI_BANDWIDTH_20MHz == pRxSettings->bandwidth) ? eBandwidth20MHz : eBandwidth40MHz;
    if (eBandwidth20MHz == chanSpec.eBandwidth)
    {
        chanSpec.eSideband = eSideband_None;
    }
    else
    {
        chanSpec.eSideband = (NETAPP_WIFI_SIDEBAND_LOWER == pRxSettings->sideband) ? eSideband_Lower : eSideband_Upper;
    }
    BWL_API_CHECK(BWL_SetChanspec(hNetAppWiFi->hBwl, &chanSpec), tRetCode);

    if (NETAPP_WIFI_802_11_MODE_A == pRxSettings->protocol)
    {
        BWL_API_CHECK(BWL_SetBand(hNetAppWiFi->hBwl, eBand5G), tRetCode);
    }
    else
    {
        BWL_API_CHECK(BWL_SetBand(hNetAppWiFi->hBwl, eBand2G), tRetCode);
    }

    BWL_API_CHECK(BWL_SetDataRate(hNetAppWiFi->hBwl, pRxSettings->data_rate), tRetCode);

    BWL_API_CHECK(BWL_SetMcsIndex(hNetAppWiFi->hBwl, pRxSettings->mcsindex), tRetCode);

    memset(&pktEng, 0, sizeof(PacketEngineParameters_t));
    pktEng.bStart = true;
    pktEng.bTxMode = false;
    strncpy(pktEng.acMac, pRxSettings->acMac, MAC_ADDR_LEN);
    BWL_API_CHECK(BWL_ConfigurePacketEngine(hNetAppWiFi->hBwl, &pktEng), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestPrivateStopReceiving(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    PacketEngineParameters_t pktEng;

    memset(&pktEng, 0, sizeof(PacketEngineParameters_t));
    pktEng.bStart = false;
    pktEng.bTxMode = false;
    BWL_API_CHECK(BWL_ConfigurePacketEngine(hNetAppWiFi->hBwl, &pktEng), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiTestPrivateGetRssiPerAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_RSSI_PER_ANTENNA *pRssiPerAntenna)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    RssiPerAntenna_t rssiPerAntenna;
    int i;

    BWL_API_CHECK(BWL_GetRssiPerAntenna(hNetAppWiFi->hBwl, &rssiPerAntenna), tRetCode);
    pRssiPerAntenna->ulNumAntennas = rssiPerAntenna.ulNumAntennas;
    for (i=0; i<rssiPerAntenna.ulNumAntennas; i++)
    {
        pRssiPerAntenna->lRSSI_ant[i] = rssiPerAntenna.lRSSI_ant[i];
        pRssiPerAntenna->tRSSI_ant[i] = NetAppWiFi_P_ConvertRSSI(hNetAppWiFi, rssiPerAntenna.lRSSI_ant[i]);
    }

    BWL_API_CHECK(BWL_GetRSSI(hNetAppWiFi->hBwl, &(pRssiPerAntenna->lRSSI)), tRetCode);
    pRssiPerAntenna->tRSSI = NetAppWiFi_P_ConvertRSSI(hNetAppWiFi, rssiPerAntenna.lRSSI);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiTestPrivateControlWaveform(
    NETAPP_WIFI_HANDLE hNetAppWiFi,
    uint32_t ulChannel,
    bool bEnable)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    int32_t channel;

    BWL_API_CHECK(BWL_Down(hNetAppWiFi->hBwl), tRetCode);
    BWL_API_CHECK(BWL_SetBand(hNetAppWiFi->hBwl, eBand2G), tRetCode);
    BWL_API_CHECK(BWL_Up(hNetAppWiFi->hBwl), tRetCode);
    BWL_API_CHECK(BWL_Disassoc(hNetAppWiFi->hBwl), tRetCode);
    BWL_API_CHECK(BWL_Out(hNetAppWiFi->hBwl), tRetCode);

    if (false == bEnable)
    {
        channel = 0;
    }
    BWL_API_CHECK(BWL_ControlFrequencyAccuracy(hNetAppWiFi->hBwl, channel), tRetCode);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

/*******************************************************************************
 *                       Static NetApp WiFi Functions
 ******************************************************************************/
static NETAPP_RETCODE NetAppWiFi_P_Connect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;
    Credential_t        Credential;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NetAppWiFi_P_PrintApInfo(pApInfo);

    memset( &Credential, 0, sizeof( Credential ) );

    Credential.eNetOpMode = (pApInfo->bAdHoc) ? eNetOpModeAdHoc : eNetOpModeInfra;
    Credential.eWpaSup    = eWpaSupInternal;

    if (pApInfo->cPassword[0] != '\0')
    {
        memcpy(Credential.acKey, pApInfo->cPassword, strlen(pApInfo->cPassword));
    }

    memcpy(Credential.acSSID, pApInfo->cSSID, strlen(pApInfo->cSSID));

    if (pApInfo->cBSSID[0] != '\0')
    {
        Credential.peBSSID = ether_aton(pApInfo->cBSSID);
    }

    switch(pApInfo->tSecurity)
    {
    case NETAPP_WIFI_SECURITY_NONE:             //!< Disabled
        Credential.eWpaAuth = eWpaAuthDisabled;
        Credential.eWSec    = eWSecNone;
        Credential.eAuthType= eAuthTypeOpen;
        break;

    case NETAPP_WIFI_SECURITY_WEP:              //!< Open & Shared WEP
        Credential.eWpaAuth = eWpaAuthDisabled;
        Credential.eWSec    = eWSecWep;
        Credential.eAuthType= eAuthTypeOpenShare;
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_AES:      //!< WPA-Personal, AES encryp.
        Credential.eWpaAuth = eWpaAuthWpaPsk;
        Credential.eWSec    = eWSecAes;
        Credential.eAuthType= eAuthTypeOpen;
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP:     //!< WPA-Personal, TKIP encryp.
        Credential.eWpaAuth = eWpaAuthWpaPsk;
        Credential.eWSec    = eWSecTkip;
        Credential.eAuthType= eAuthTypeOpen;
        break;

    case NETAPP_WIFI_SECURITY_WPA2_PSK_AES:     //!< WPA2-Personal, AES encryp.
        Credential.eWpaAuth = eWpaAuthWpa2Psk;
        Credential.eWSec    = eWSecAes;
        Credential.eAuthType = eAuthTypeOpen;
        break;

    case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP:    //!< WPA-Personal, TKIP encryp.
        Credential.eWpaAuth = eWpaAuthWpa2Psk;
        Credential.eWSec    = eWSecTkip;
        Credential.eAuthType= eAuthTypeOpen;
        break;

    default:
        NETAPP_ERR(("%s Invalid Security mode!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    BWL_API_CHECK(BWL_ConnectAp(hNetAppWiFi->hBwl, &Credential), tRetCode);

err_out:
    if (tRetCode != NETAPP_SUCCESS)
    {
        BWL_DisconnectAp(hNetAppWiFi->hBwl);
    }
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;

}


static NETAPP_WIFI_RSSI NetAppWiFi_P_ConvertRSSI(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    int32_t             lRSSI)
{
    int i = 0;
    if (hNetAppWiFi == NULL)
    {
        return  NETAPP_WIFI_RSSI_MAX;
    }

    while ((lRSSILevels[hNetAppWiFi->tDriverName][i] < lRSSI) && (i < (int)NETAPP_WIFI_RSSI_EXCELLENT) )
    {
        i++;
    }

    return (NETAPP_WIFI_RSSI)i;
}


static NETAPP_RETCODE NetAppWiFi_P_ProcessConnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_MSG_INFO     *pMessage)
{
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    if (pMessage == NULL)
    {
        NETAPP_ERR(("%s(): Null Message! ", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }


    /* Perform Initial pre-connect operations */
    switch (pMessage->tConnectType)
    {
#ifndef NO_WIFI_P2P
    /* WiFi Direct Connection */
    case NETAPP_CONNECT_P2P:

        NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateConnect(hNetAppWiFi->hNetAppP2P,
            pMessage->tData.tApInfo.cSSID, pMessage->ulData0), tRetCode);
        break;
#endif

    /* WPS request */
    case NETAPP_CONNECT_WPS_PIN_REGISTRAR:
    case NETAPP_CONNECT_WPS_PIN_ENROLLEE:
    case NETAPP_CONNECT_WPS_PB:
    {
#ifndef NO_WIFI_WPS
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, true), tRetCode);
        /* Blocking API call to run the WPS registration loop */
        NETAPP_PRIV_API_CHECK(NetAppWiFiWPSConnect(hNetAppWiFi->hNetAppWPS,
            pMessage->tConnectType, &pMessage->tData.tApInfo, pMessage->ulData0, hNetAppWiFi->cIfaceName), tRetCode);

        /* Cache the password */
        strncpy(hNetAppWiFi->tCurApInfo.cPassword, pMessage->tData.tApInfo.cPassword,
            sizeof(hNetAppWiFi->tCurApInfo.cPassword)/sizeof(hNetAppWiFi->tCurApInfo.cPassword[0]));
        hNetAppWiFi->tCurApInfo.cPassword[sizeof(hNetAppWiFi->tCurApInfo.cPassword)/sizeof(hNetAppWiFi->tCurApInfo.cPassword[0]) -1] = '\0';

        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetConnectedApInfo(hNetAppWiFi, &pMessage->tData.tApInfo), tRetCode);
        hNetAppWiFi->tLinkState = NETAPP_LINK_ACQUIRING;
        goto err_out;
#endif
        break;
    }


#ifndef NO_WIFI_INVITE
    /* Reject an Invite */
    case NETAPP_CONNECT_INVITE_REJECT:
    {
        if (NetAppWiFiInvitePrivateReject(hNetAppWiFi->hNetAppWiFiInvite, pMessage->tData.tApInfo.cSSID) != NETAPP_SUCCESS)
        {
            NETAPP_MSG(("%s(): WiFi Invite Reject failed!", __FUNCTION__));
        }

        /* Wait for Link to go down */
        if (NetAppWiFi_P_WaitForEvent(hNetAppWiFi, NETAPP_WIFI_EVENT_LINK_DOWN, NETAPP_WIFI_DISCONNECT_TIMEOUT) != NETAPP_SUCCESS)
        {
            NETAPP_MSG(("%s(): Failed waiting for Event Link Down!", __FUNCTION__));
            goto err_out;
        }

        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);

        tRetCode = NETAPP_CANCELED;
        goto err_out;
        break;
    }


    /* Accept an invite request. */
    case NETAPP_CONNECT_INVITE_ACCEPT:
    {
        NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateAccept(hNetAppWiFi->hNetAppWiFiInvite,
            pMessage->tData.tApInfo.cSSID, &pMessage->tData.tApInfo), tRetCode);

        /* Wait for Link to go down */
        if (NetAppWiFi_P_WaitForEvent(hNetAppWiFi, NETAPP_WIFI_EVENT_LINK_DOWN, NETAPP_WIFI_DISCONNECT_TIMEOUT) != NETAPP_SUCCESS)
        {
            NETAPP_MSG(("%s(): Failed waiting for Event Link Down!", __FUNCTION__));
            goto err_out;
        }

        NETAPP_PRIV_API_CHECK(NetAppWiFi_P_Connect(hNetAppWiFi, &pMessage->tData.tApInfo), tRetCode);
        break;
    }
#endif

    case NETAPP_CONNECT_NORMAL:
    {
        /* Cache the password so we can pass it back to the app */
        strncpy(hNetAppWiFi->tCurApInfo.cPassword, pMessage->tData.tApInfo.cPassword,
            sizeof(hNetAppWiFi->tCurApInfo.cPassword)/sizeof(hNetAppWiFi->tCurApInfo.cPassword[0]));

        hNetAppWiFi->tCurApInfo.cPassword[sizeof(hNetAppWiFi->tCurApInfo.cPassword)/sizeof(hNetAppWiFi->tCurApInfo.cPassword[0]) -1] = '\0';

        NETAPP_PRIV_API_CHECK(NetAppWiFi_P_Connect(hNetAppWiFi, &pMessage->tData.tApInfo), tRetCode);
    }


    default:
        break;
    }


    /* Wait for connect to be established */
    if ( (tRetCode = NetAppWiFi_P_WaitForEvent(hNetAppWiFi, NETAPP_WIFI_EVENT_LINK_UP, NETAPP_WIFI_CONNECT_TIMEOUT)) == NETAPP_SUCCESS)
    {
        WpaAuth_t eWpaAuth;
        BWL_API_CHECK(BWL_GetWpaAuth(hNetAppWiFi->hBwl, &eWpaAuth), tRetCode);

        /* For WPA we need to check on the supplicant tRetCode to make sure the key exchange
         * was successful */
        if ( (eWpaAuth == eWpaAuthWpaPsk) || (eWpaAuth == eWpaAuthWpa2Psk) )
        {
            int retry = 0;
            SupStatus_t tSupStatus = eSupStatusDisconnected;

            for (retry = 0; ( (retry * NETAPP_WIFI_WAIT_MS) < NETAPP_WIFI_SUP_TIMEOUT) && !hNetAppWiFi->bStopBkgrndTask; retry++)
            {
                BWL_API_CHECK(BWL_GetWpaSupStatus(hNetAppWiFi->hBwl, &tSupStatus), tRetCode);

                if (tSupStatus == eSupStatusConnected)
                {
                    NETAPP_MSG(("%s(), took %d ms for WPA suplicant", __FUNCTION__, retry*NETAPP_WIFI_WAIT_MS));
                    break;
                }
                else if (tSupStatus == eSupStatuseConnecting)
                {
                    NetAppOSTaskDelayMsec(NETAPP_WIFI_WAIT_MS);
                    continue;
                }
                /* Reported an error or disconnected for more than 100ms then we exit */
                else if (retry > 2)
                {
                    NETAPP_ERR(("%s() WPA connection not established, tRetCode=%d!", __FUNCTION__, tSupStatus));
                    tRetCode = NETAPP_INCORRECT_PASSWORD;
                    goto err_out;
                }
                else
                {
                    NetAppOSTaskDelayMsec(NETAPP_WIFI_WAIT_MS);
                    continue;
                }
            }

            if (hNetAppWiFi->bStopBkgrndTask)
            {
                NETAPP_MSG(("%s Connect Canceled", __FUNCTION__));
                tRetCode = NETAPP_CANCELED;
                goto err_out;
            }
            else if (tSupStatus != eSupStatusConnected)
            {
                NETAPP_ERR(("%s() WPA connection not established after %d ms, tRetCode=%d!",
                    __FUNCTION__, (retry*NETAPP_WIFI_WAIT_MS), tSupStatus));
                tRetCode = NETAPP_INCORRECT_PASSWORD;
                goto err_out;
            }
        }

        /* Fetch the connected info */
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetConnectedApInfo(hNetAppWiFi, &pMessage->tData.tApInfo), tRetCode);
    }
    else
    {
        NETAPP_MSG(("%s() Failed to connect to AP, error=%s!", __FUNCTION__, NetAppPrivate_StrRetCode(tRetCode)));
    }

err_out:
    if ( (pMessage->tConnectType == NETAPP_CONNECT_NORMAL) &&
         (pMessage->tData.tApInfo.tSecurity == NETAPP_WIFI_SECURITY_WEP) &&
         ( (tRetCode == NETAPP_FAILURE) || (tRetCode == NETAPP_TIMEOUT) ) )
    {
        tRetCode = NETAPP_INCORRECT_PASSWORD;
    }

    return tRetCode;
}


static NETAPP_RETCODE NetAppWiFi_P_ConvertBwlInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    ScanInfo_t          *pBwlScanInfo,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    if ( (hNetAppWiFi == NULL) || (pBwlScanInfo == NULL) || (pApInfo == NULL) )
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    strncpy(pApInfo->cSSID, pBwlScanInfo->tCredentials.acSSID, sizeof(pApInfo->cSSID)/sizeof(pApInfo->cSSID[0])-1);
    pApInfo->cSSID[sizeof(pApInfo->cSSID)/sizeof(pApInfo->cSSID[0])-1] = '\0';

    snprintf (pApInfo->cBSSID, sizeof(pApInfo->cBSSID), "%02X:%02X:%02X:%02X:%02X:%02X",
           pBwlScanInfo->BSSID.ether_addr_octet[0], pBwlScanInfo->BSSID.ether_addr_octet[1], pBwlScanInfo->BSSID.ether_addr_octet[2],
           pBwlScanInfo->BSSID.ether_addr_octet[3], pBwlScanInfo->BSSID.ether_addr_octet[4], pBwlScanInfo->BSSID.ether_addr_octet[5]);

    pApInfo->cBSSID[sizeof(pApInfo->cBSSID)/sizeof(pApInfo->cBSSID[0])-1] = '\0';

    pApInfo->tRSSI      = NetAppWiFi_P_ConvertRSSI(hNetAppWiFi, pBwlScanInfo->lRSSI);
    pApInfo->lRSSI      = pBwlScanInfo->lRSSI;
    pApInfo->bAdHoc     = (pBwlScanInfo->tCredentials.eNetOpMode == 0);
    pApInfo->tMode      = NETAPP_WIFI_802_11_NONE;

    switch (pBwlScanInfo->tBandwidth)
    {
    case eBandwidth10MHz:
        pApInfo->tChanBandwidth = NETAPP_WIFI_BANDWIDTH_10MHz;
        break;
    case eBandwidth20MHz:
        pApInfo->tChanBandwidth = NETAPP_WIFI_BANDWIDTH_20MHz;
        break;
    case eBandwidth40MHz:
        pApInfo->tChanBandwidth = NETAPP_WIFI_BANDWIDTH_40MHz;
        break;
    default:
        pApInfo->tChanBandwidth = NETAPP_WIFI_BANDWIDTH_INVALID;
        break;
    }

    pApInfo->tMode     |= (pBwlScanInfo->ul802_11Modes & e802_11_a) ? NETAPP_WIFI_802_11_MODE_A : NETAPP_WIFI_802_11_NONE;
    pApInfo->tMode     |= (pBwlScanInfo->ul802_11Modes & e802_11_b) ? NETAPP_WIFI_802_11_MODE_B : NETAPP_WIFI_802_11_NONE;
    pApInfo->tMode     |= (pBwlScanInfo->ul802_11Modes & e802_11_g) ? NETAPP_WIFI_802_11_MODE_G : NETAPP_WIFI_802_11_NONE;
    pApInfo->tMode     |= (pBwlScanInfo->ul802_11Modes & e802_11_n) ? NETAPP_WIFI_802_11_MODE_N : NETAPP_WIFI_802_11_NONE;
    pApInfo->bWPS       = pBwlScanInfo->bWPS;
    pApInfo->ulChannel  = pBwlScanInfo->ulChan;
    pApInfo->lPhyNoise  = pBwlScanInfo->lPhyNoise;

    pApInfo->lRate      = pBwlScanInfo->lRate;

    /* Determine the security type */
    if ( (pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpa2Psk) && (pBwlScanInfo->tCredentials.eWSec & eWSecAes) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_AES;
    }
    else if ((pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpa2Psk) && (pBwlScanInfo->tCredentials.eWSec & eWSecTkip) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP;
    }
    else if ( (pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpaPsk) && (pBwlScanInfo->tCredentials.eWSec & eWSecAes) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_AES;
    }
    else if ((pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpaPsk) && (pBwlScanInfo->tCredentials.eWSec & eWSecTkip) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_TKIP;
    }
    /* Check for unsupported WPS/RSN authentication modes */
    else if ( (pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpaUnsp) ||
              (pBwlScanInfo->tCredentials.eWpaAuth & eWpaAuthWpa2Unsp) )
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_NOT_SUPPORTED;
    }
    else if (pBwlScanInfo->bLocked)
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WEP;
    }
    else
    {
        pApInfo->tSecurity = NETAPP_WIFI_SECURITY_NONE;
    }

    return NETAPP_SUCCESS;
}


static void NetAppWiFi_P_PrintApInfo(
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    unsigned int i;
    if (pApInfo == NULL)
    {
        return;
    }

#ifdef NETAPP_DEBUG
    NETAPP_MSG(("\n*****************************"));
    NETAPP_MSG(("SSID:     %s",   pApInfo->cSSID));
    NETAPP_MSG(("BSSID:    %s",     pApInfo->cBSSID));
    NETAPP_MSG(("Password: %s",     pApInfo->cPassword));
    NETAPP_MSG(("ulChan:   %d",     pApInfo->ulChannel));
    NETAPP_MSG(("Bandwidth:%s",     (pApInfo->tChanBandwidth == NETAPP_WIFI_BANDWIDTH_40MHz) ? "40MHz" :
            (pApInfo->tChanBandwidth == NETAPP_WIFI_BANDWIDTH_20MHz) ? "20MHz" : "Unknown"));
    NETAPP_MSG(("bAdHoc:   %s",     (pApInfo->bAdHoc) ? "YES" : "NO"));
    NETAPP_MSG(("bWPS:     %s",     (pApInfo->bWPS)   ? "YES" : "NO"));

    for (i = 0; i < sizeof(RSSIPrintTable)/sizeof(RSSIPrintTable[0]); i++)
    {
        if (RSSIPrintTable[i].tRSSI == pApInfo->tRSSI)
        {
            NETAPP_MSG(("tRSSI:    %s", RSSIPrintTable[i].pName));
            break;
        }
    }

    for (i = 0; i < sizeof(SecurityPrintTable)/sizeof(SecurityPrintTable[0]); i++)
    {
        if (SecurityPrintTable[i].tSecurity == pApInfo->tSecurity)
        {
            NETAPP_MSG(("tSecurity:%s", SecurityPrintTable[i].pName));
            break;
        }
    }

    NETAPP_MSG(("802.11:   %s%s%s%s",
        (pApInfo->tMode & NETAPP_WIFI_802_11_MODE_A) ? "a " : "",
        (pApInfo->tMode & NETAPP_WIFI_802_11_MODE_B) ? "b " : "",
        (pApInfo->tMode & NETAPP_WIFI_802_11_MODE_G) ? "g " : "",
        (pApInfo->tMode & NETAPP_WIFI_802_11_MODE_N) ? "n" : ""));

    NETAPP_MSG(("Rate:     %d%s Mbps", pApInfo->lRate/2, (pApInfo->lRate & 1) ? ".5" : ""));
    NETAPP_MSG(("Noise:    %d dBm", pApInfo->lPhyNoise));

    NETAPP_MSG(("*****************************"));
#endif
}


static void NetAppWiFi_P_ScanTimer(
    void            *pParam)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_WIFI_HANDLE  hNetAppWiFi = (NETAPP_WIFI_HANDLE)pParam;
    NETAPP_MSG_INFO     tMessage;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    if ( (hNetAppWiFi == NULL) || !hNetAppWiFi->bWiFiInserted)
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return;
    }

    memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

    tMessage.tMsgType = NETAPP_MSG_SCAN;

    if (NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_NO_WAIT) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s(): Failure sending scan request!", __FUNCTION__));
    }

    return;
}


static void NetAppWiFi_P_BackgroundTask(
    void              *pParam)
{
    struct sockaddr_ll  sll;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_MSG_INFO     tMessage;
    NETAPP_WIFI_HANDLE  hNetAppWiFi = (NETAPP_WIFI_HANDLE)pParam;
    NETAPP_IPV4_ADDR    tCurrIpAddr = 0;
    NETAPP_SETTINGS     tSettings;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    if (hNetAppWiFi == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return;
    }

    hNetAppWiFi->lBwlSocket = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE_BRCM));
    if (hNetAppWiFi->lBwlSocket < 0)
    {
        NETAPP_ERR(("%s() Error creating socket: %s ", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETHER_TYPE_BRCM);
    sll.sll_ifindex = if_nametoindex(hNetAppWiFi->cIfaceName);

    if (bind(hNetAppWiFi->lBwlSocket, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        NETAPP_ERR(("%s() Unable to bind socket: %s ", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

#ifndef NO_WIFI_INVITE
    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings), tRetCode);

    /* Check to see if we need to start WiFi Invite by default */
    if ( ( (tSettings.bForceWiFi) || (NetAppPrivateGetLinkState(hNetAppWiFi->hNetAppPriv, NETAPP_IFACE_ETH0) == NETAPP_LINK_DOWN) ) &&
         (tSettings.bAutoReConnect) &&
         (hNetAppWiFi->tCurApInfo.cSSID[0] == '\0') )
    {
                NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateStart(hNetAppWiFi->hNetAppWiFiInvite), tRetCode);
                NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStartScan(hNetAppWiFi,
                    NETAPP_WIFI_INVITE_INTERVAL, NETAPP_DEFAULT_SCANTIME_MS, NETAPP_SCAN_INVITE), tRetCode);
    }
#endif

    /* Load the saved AP if one into the tCurApInfo */
    NetAppSettingsPrivateGetCurrentWiFiApInfo(hNetAppWiFi->hNetAppSettings, &hNetAppWiFi->tCurApInfo);

    for(;;)
    {
        hNetAppWiFi->bStopBkgrndTask = false;
        NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings), tRetCode);

        if (NetAppOSMsgQReceive(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), 0) != NETAPP_SUCCESS)
        {
            if (NetAppWiFi_P_WaitForEvent(hNetAppWiFi, (NETAPP_WIFI_EVENT_LINK_DOWN | NETAPP_WIFI_EVENT_LINK_UP) , NETAPP_WIFI_WAIT_MS*4) == NETAPP_FAILURE)
            {
                NETAPP_ERR(("%s() NetAppWiFi_P_WaitForEvent() failed!", __FUNCTION__));
            }

            /* Check if link changes from acquiring to up or vice versa */
            if (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN)
            {
                NETAPP_IPV4_ADDR  tNewIpAddr = 0;
                NETAPP_LINK_STATE tNewState = NetAppPrivateIsIpAddressSet(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, &tNewIpAddr) ?
                    NETAPP_LINK_UP : NETAPP_LINK_ACQUIRING;

                if ( (hNetAppWiFi->tLinkState != tNewState) ||
                     ( (hNetAppWiFi->tLinkState == NETAPP_LINK_UP) && (tNewIpAddr != tCurrIpAddr) ) )
                {
                    hNetAppWiFi->tLinkState = tNewState;
                    tCurrIpAddr             = tNewIpAddr;
                    NETAPP_PRIV_API_CHECK(NetAppDhcpcdWaitForTaskCompelte(hNetAppWiFi->hNetAppDhcpcd, hNetAppWiFi->cIfaceName), tRetCode);
                    NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                }
                continue;
            }

            /* Should we re-connect to our saved AP if the link is down? */
            else if ( (tSettings.bAutoReConnect) &&
                      ( tSettings.bForceWiFi || (NetAppPrivateGetLinkState(hNetAppWiFi->hNetAppPriv, NETAPP_IFACE_ETH0) == NETAPP_LINK_DOWN) ) &&
                      (hNetAppWiFi->tCurApInfo.cSSID[0] != '\0') )
            {
                tMessage.tMsgType       = NETAPP_MSG_CONNECT;
                tMessage.tConnectType   = NETAPP_CONNECT_NORMAL;
                tMessage.tData.tApInfo  = hNetAppWiFi->tCurApInfo;
                /* Do not continue so we can process this internal message */
            }
            else
            {
                continue;
            }
        }

        NETAPP_MSG(("*********************************************"));

        if (hNetAppWiFi->tScanType & NETAPP_SCAN_P2P)
        {
#ifndef NO_WIFI_P2P
            NetAppWiFiP2PPrivateStopDiscovery(hNetAppWiFi->hNetAppP2P);
#endif
        }

        /* Parse the received request */
        switch(tMessage.tMsgType)
        {

        case NETAPP_MSG_CONNECT:
        case NETAPP_MSG_DISCONNECT:
        {
            uint32_t    ulConnectRetry = NETAPP_WIFI_CONNECT_RETRY;
            NETAPP_MSG(("WIFI Task: Received %s !",
                (tMessage.tMsgType == NETAPP_MSG_CONNECT) ? "NETAPP_MSG_CONNECT" : "NETAPP_MSG_DISCONNECT"));

            if (tMessage.tMsgType == NETAPP_MSG_CONNECT)
            {
                hNetAppWiFi->tState = NETAPP_STATE_CONNECTING;
            }

            /* If we are connected to a bridge we cannot bring down the driver */
            if (hNetAppWiFi->cBridgeName[0] != '\0')
            {
                BWL_DisconnectAp(hNetAppWiFi->hBwl);
            }
            else
            {
                if (NetAppPrivateSetNetworkSettings(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName,
                    NETAPP_IFACE_WIRELESS, NETAPP_IP_MODE_OFF, NULL) != NETAPP_SUCCESS)
                {
                    NETAPP_ERR(("%s(): Failure to apply IP settings!", __FUNCTION__));
                }
                NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, false), tRetCode);
            }

            hNetAppWiFi->tLinkState = NETAPP_LINK_DOWN;

            if (tMessage.tMsgType == NETAPP_MSG_DISCONNECT)
            {
                /* Clean the current AP info */
                memset(&hNetAppWiFi->tCurApInfo, 0, sizeof(hNetAppWiFi->tCurApInfo));
                NetAppSettingsPrivateClearCurrentWiFiApInfo(hNetAppWiFi->hNetAppSettings);

#ifndef NO_WIFI_INVITE
                NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateStart(hNetAppWiFi->hNetAppWiFiInvite), tRetCode);
                NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStartScan(hNetAppWiFi,
                    NETAPP_WIFI_INVITE_INTERVAL, NETAPP_DEFAULT_SCANTIME_MS, NETAPP_SCAN_INVITE), tRetCode);
#endif

                NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                break;
            }

            NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, true), tRetCode);
            NETAPP_PRIV_API_CHECK(NetAppPrivateSetInterfaceUp(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, true), tRetCode);


            /* Extra check for normal connect & WEP SHARED */
            if ( (tMessage.tConnectType == NETAPP_CONNECT_NORMAL) &&
                 ( (tMessage.tData.tApInfo.tSecurity == NETAPP_WIFI_SECURITY_WEP) || (tMessage.tData.tApInfo.tSecurity == NETAPP_WIFI_SECURITY_AUTO_DETECT) ) )
            {
                bool bFoundAp = false;
                ScanParams_t tScanParam = { NETAPP_DEFAULT_SCANTIME_MS, -1, -1, tMessage.tData.tApInfo.cSSID };

                BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT), tRetCode);

                if (BWL_EScan(hNetAppWiFi->hBwl, &tScanParam) != BWL_ERR_SUCCESS)
                {
                    NETAPP_ERR(("%s(): Scan request failed, should not happen!", __FUNCTION__));
                }
                else
                {
                    ScanInfo_t      *pData = NULL;
                    uint32_t        ulCount = 0;

                    if ( (BWL_EScanResults(hNetAppWiFi->hBwl, (hNetAppWiFi->cBridgeName[0] != '\0') ? hNetAppWiFi->cBridgeName :hNetAppWiFi->cIfaceName, &pData, &ulCount, &hNetAppWiFi->bStopBkgrndTask) == NETAPP_SUCCESS) &&
                         (NetAppWiFi_P_SaveScanResults(hNetAppWiFi, pData, ulCount) == NETAPP_SUCCESS) )
                    {

                        int i;
                        for (i = 0; i < ulCount; i++)
                        {
                            if (strcmp(tMessage.tData.tApInfo.cSSID, hNetAppWiFi->pScanResultsList[i].cSSID) == 0)
                            {
                                bFoundAp = true;
                                /* Auto-detect the security */
                                if (tMessage.tData.tApInfo.tSecurity == NETAPP_WIFI_SECURITY_AUTO_DETECT)
                                {
                                    /* Open WEP appears as Open, so if there is a password, we assume it is open WEP */
                                    tMessage.tData.tApInfo.tSecurity = ( (tMessage.tData.tApInfo.cPassword[0] != '\0') && (hNetAppWiFi->pScanResultsList[i].tSecurity == NETAPP_WIFI_SECURITY_NONE) ) ?
                                        NETAPP_WIFI_SECURITY_WEP : hNetAppWiFi->pScanResultsList[i].tSecurity;
                                }
                                break;
                            }
                        }
                    }
                    if (pData != NULL)
                    {
                        free(pData);
                    }
                }
                BWL_ClearEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT);

                if (tRetCode == NETAPP_CANCELED)
                {
                    NETAPP_MSG(("%s(): Connection Cancelled!", __FUNCTION__));
                    break;
                }

                if (!bFoundAp)
                {
                    NETAPP_ERR(("%s(): %s not found!", __FUNCTION__, tMessage.tData.tApInfo.cSSID));
                    NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &tMessage.tData.tApInfo,
                        sizeof(tMessage.tData.tApInfo), 0, NETAPP_FAILURE, NETAPP_IFACE_WIRELESS);
                    break;
                }
            }

            /* Loop trying to reconnect if first attempt fails with a timeout */
            do {
                /* Now connect */
                tRetCode = NetAppWiFi_P_ProcessConnect(hNetAppWiFi, &tMessage);

                if (tRetCode != NETAPP_SUCCESS)
                {
                    BWL_DisconnectAp(hNetAppWiFi->hBwl);
                    if (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN)
                    {
                        if (NetAppWiFi_P_WaitForEvent(hNetAppWiFi, NETAPP_WIFI_EVENT_LINK_DOWN, NETAPP_WIFI_CONNECT_TIMEOUT) != NETAPP_SUCCESS)
                        {
                            NETAPP_ERR(("%s(): Failed waiting for Event Link Down!", __FUNCTION__));
                        }
                    }
                }
                else
                {
                    NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                    break;
                }
            } while ( (tRetCode == NETAPP_TIMEOUT) && ulConnectRetry--);

            if (tRetCode != NETAPP_CANCELED)
            {
                if (tRetCode == NETAPP_SUCCESS)
                {
                    NETAPP_SETTINGS tSettings;
                    NetAppPrivateGetSettings(hNetAppWiFi->hNetAppPriv, &tSettings);

                    /* Save the connected AP info so we can auto-reconnect later*/
                    hNetAppWiFi->tCurApInfo = tMessage.tData.tApInfo;
                    tSettings.tSoftApSettings.tApInfo = hNetAppWiFi->tCurApInfo;
                    NetAppPrivateSetSettings(hNetAppWiFi->hNetAppPriv, &tSettings);

                    NetAppSettingsPrivateSaveWiFiApInfo(hNetAppWiFi->hNetAppSettings, &hNetAppWiFi->tCurApInfo);
                }
                NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &tMessage.tData.tApInfo,
                    sizeof(tMessage.tData.tApInfo), 0, tRetCode, NETAPP_IFACE_WIRELESS);
            }
            break;
        }


        /* Fetch AP info */
        case NETAPP_MSG_FETCH_APINFO:
        {
            ScanParams_t        tScanParam;
            NETAPP_WIFI_AP_INFO tApInfo;
            ScanInfo_t          *pData = NULL;
            uint32_t            ulCount = 0;

            NETAPP_MSG(("WIFI Task: Received NETAPP_MSG_FETCH_APINFO!"));

            hNetAppWiFi->tState             = NETAPP_STATE_SCANNING;
            tScanParam.lActiveTime          = NETAPP_DEFAULT_SCANTIME_MS;
            tScanParam.lPassiveTime         = -1;
            tScanParam.lHomeTime            = -1;
            tScanParam.pcSSID                = tMessage.tData.tApInfo.cSSID;

            NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, true), tRetCode);
            NETAPP_PRIV_API_CHECK(NetAppPrivateSetInterfaceUp(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, true), tRetCode);

            BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT), tRetCode);
            if (BWL_EScan(hNetAppWiFi->hBwl, &tScanParam) != BWL_ERR_SUCCESS)
            {
                NETAPP_ERR(("%s(): Scan request failed, should not happen!", __FUNCTION__));
            }

            if ( (tRetCode = BWL_EScanResults(hNetAppWiFi->hBwl, (hNetAppWiFi->cBridgeName[0] != '\0') ? hNetAppWiFi->cBridgeName :hNetAppWiFi->cIfaceName, &pData, &ulCount, &hNetAppWiFi->bStopBkgrndTask)) == NETAPP_SUCCESS)
            {
                if ( (tRetCode = NetAppWiFi_P_SaveScanResults(hNetAppWiFi, pData, ulCount)) == NETAPP_SUCCESS)
                {
                    uint32_t i;
                    for (i = 0; i < hNetAppWiFi->ulScanResultsCount; i++)
                    {
                        if (strcmp(tMessage.tData.tApInfo.cSSID, hNetAppWiFi->pScanResultsList[0].cSSID) == 0)
                        {
                            tApInfo = hNetAppWiFi->pScanResultsList[i];
                            break;
                        }
                    }
                    if (i == hNetAppWiFi->ulScanResultsCount)
                    {
                        NETAPP_MSG(("%s(): AP %s not found", __FUNCTION__, tMessage.tData.tApInfo.cSSID));
                        tRetCode = NETAPP_FAILURE;
                    }
                }
                if (pData != NULL)
                {
                    free(pData);
                }
            }

            BWL_ClearEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT);
            if (tRetCode != NETAPP_CANCELED)
            {
                NETAPP_DO_CALLBACK(NETAPP_CB_FETCHED_APINFO, &tApInfo,
                    sizeof(tMessage.tData.tApInfo), 0, tRetCode, NETAPP_IFACE_WIRELESS);
            }
            break;
        }

        case NETAPP_MSG_P2P:
        {
#ifndef NO_WIFI_P2P
            NETAPP_MSG(("WIFI Task: Received NETAPP_MSG_P2P!"));
            NetAppWiFiP2PPrivateDiscover(hNetAppWiFi->hNetAppP2P);
#endif
            break;
        }

        /* Scan request received */
        case NETAPP_MSG_SCAN:
        {
            struct timespec tCurrentTime;
            int32_t         lTimeDiffMs = 0;
            ScanInfo_t      *pData = NULL;
            uint32_t        ulCount = 0;

            if ( !(hNetAppWiFi->tScanType & (NETAPP_SCAN_NORMAL | NETAPP_SCAN_INVITE)) )
            {
                break;
            }

            NETAPP_MSG(("WIFI Task: Received NETAPP_MSG_SCAN!"));

            hNetAppWiFi->tState = NETAPP_STATE_SCANNING;

            NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, true), tRetCode);
            NETAPP_PRIV_API_CHECK(NetAppPrivateSetInterfaceUp(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, true), tRetCode);
            BWL_API_CHECK(BWL_SetEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT), tRetCode);

#ifndef NO_WIFI_INVITE
            if (hNetAppWiFi->tScanType & NETAPP_SCAN_INVITE)
            {
                if (NetAppWiFiInvitePrivateScan(hNetAppWiFi->hNetAppWiFiInvite) != NETAPP_SUCCESS)
                {
                    NETAPP_ERR(("%s(): Invite scan request failed, should not happen!", __FUNCTION__));
                }
            }
            else
#endif
            {
                ScanParams_t tScanParam;
                memset(&tScanParam, 0, sizeof(ScanParams_t));

                tScanParam.lActiveTime  = hNetAppWiFi->lScanTimeMs;
                tScanParam.lPassiveTime = -1;
                tScanParam.lHomeTime    = -1;
                tScanParam.pcSSID        = NULL;

                if (BWL_EScan(hNetAppWiFi->hBwl, &tScanParam) != BWL_ERR_SUCCESS)
                {
                    NETAPP_ERR(("%s(): Scan request failed, should not happen!", __FUNCTION__));
                }
                hNetAppWiFi->ulScanCount++;
            }

            clock_gettime(CLOCK_MONOTONIC, &hNetAppWiFi->tScanTime);

            if (BWL_EScanResults(hNetAppWiFi->hBwl, (hNetAppWiFi->cBridgeName[0] != '\0') ? hNetAppWiFi->cBridgeName :hNetAppWiFi->cIfaceName, &pData, &ulCount, &hNetAppWiFi->bStopBkgrndTask) == NETAPP_SUCCESS)
            {
#ifndef NO_WIFI_INVITE
                if (hNetAppWiFi->tScanType & NETAPP_SCAN_INVITE)
                {
                    NetAppWiFiInvitePrivateScanResults(hNetAppWiFi->hNetAppWiFiInvite);
                }
#endif
                tRetCode = NetAppWiFi_P_SaveScanResults(hNetAppWiFi, pData, ulCount);
                if (pData != NULL)
                {
                    free(pData);
                    pData = NULL;
                }
                if ( (tRetCode == NETAPP_SUCCESS) && (hNetAppWiFi->bBurstScanResults) )
                {
                    int i;
                    for (i = 0; i < hNetAppWiFi->ulScanResultsCount; i++)
                    {
                        NETAPP_DO_CALLBACK(NETAPP_CB_SCANNED_APINFO, &hNetAppWiFi->pScanResultsList[i],
                            sizeof(hNetAppWiFi->pScanResultsList[i]), hNetAppWiFi->ulScanCount, tRetCode, NETAPP_IFACE_WIRELESS);
                    }
                }
                NETAPP_DO_CALLBACK(NETAPP_CB_SCAN_DONE, NULL, 0, hNetAppWiFi->ulScanCount, tRetCode, NETAPP_IFACE_WIRELESS);
            }

            clock_gettime(CLOCK_MONOTONIC, &tCurrentTime);
            lTimeDiffMs = (tCurrentTime.tv_sec*1000 + tCurrentTime.tv_nsec/1000000) -
                (hNetAppWiFi->tScanTime.tv_sec*1000 + hNetAppWiFi->tScanTime.tv_nsec/1000000);

            NETAPP_MSG(("%s(): Scan took %d ms to complete", __FUNCTION__, lTimeDiffMs));

            /* Figure out how long to wait for next scan request should be sent */
            if (hNetAppWiFi->tScanType != NETAPP_SCAN_NONE)
            {
                if (hNetAppWiFi->lScanInterval != 0)
                {
                    NetAppOSTimerSetMsec(hNetAppWiFi->tScanTimer,
                            ((hNetAppWiFi->lScanInterval - lTimeDiffMs) > 0) ? (hNetAppWiFi->lScanInterval - lTimeDiffMs) : NETAPP_WIFI_WAIT_MS);
                }
                else
                {
                    hNetAppWiFi->tScanType &= NETAPP_SCAN_NORMAL;
                }
            }

            BWL_ClearEvent(hNetAppWiFi->hBwl, BWL_E_ESCAN_RESULT);
            break;
        }


        case NETAPP_MSG_SET_SETTINGS:
        {
            NETAPP_MSG(("WIFI Task: Received NETAPP_MSG_SET_SETTINGS!"));

            if (NetAppPrivateSetNetworkSettings(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, NETAPP_IFACE_WIRELESS,
                tMessage.ulData0, &tMessage.tData.tSettings) == NETAPP_SUCCESS)
            {
                switch (tMessage.ulData0)
                {
                case NETAPP_IP_MODE_OFF:
    #ifndef NO_WIFI_INVITE
                    NetAppWiFiInvitePrivateStop(hNetAppWiFi->hNetAppWiFiInvite);
    #endif
                    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(hNetAppWiFi, NETAPP_SCAN_ALL), tRetCode);
                    NetAppWiFiPrivateSetInterfaceUp(hNetAppWiFi, false);
                    hNetAppWiFi->tLinkState = NETAPP_LINK_DOWN;
                    break;

                case NETAPP_IP_MODE_STATIC:
                    if (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN)
                    {
                        hNetAppWiFi->tLinkState = NETAPP_LINK_UP;
                        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                    }
                    break;

                default:
                    if (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN)
                    {
                        hNetAppWiFi->tLinkState = NETAPP_LINK_ACQUIRING;
                    }
                    break;
                }
            }
            break;
        }


        case NETAPP_MSG_DIE:
            tRetCode = NETAPP_SUCCESS;
            /* Fallthrough */

        default:
            NETAPP_MSG(("%s(): Leaving the task now!", __FUNCTION__));
            goto err_out;
        }

        /* Re-enable P2P discovery after finishing a scan request */
        if (hNetAppWiFi->tScanType & NETAPP_SCAN_P2P)
        {
#ifndef NO_WIFI_P2P
            NetAppWiFiP2PPrivateDiscover(hNetAppWiFi->hNetAppP2P);
#endif
        }

        hNetAppWiFi->tState = NETAPP_STATE_IDLE;
        NETAPP_MSG(("WIFI Task(): Finished Processing Message"));
        NETAPP_MSG(("*********************************************"));
    }

err_out:
    if (hNetAppWiFi->lBwlSocket > 0)
    {
        close(hNetAppWiFi->lBwlSocket);
    }

    if (tRetCode != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("**************************************************"));
        NETAPP_ERR((" FATAL ERROR IN %s()! Thread Dying", __FUNCTION__));
        NETAPP_ERR(("**************************************************"));
    }
    hNetAppWiFi->ulBkgrndTaskId = 0;
    NetAppOSTaskExit();
    return;
}


static NETAPP_RETCODE NetAppWiFi_P_SaveScanResults(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    ScanInfo_t         *pData,
    uint32_t            ulCount)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    int                 i           = 0;
    NETAPP_WIFI_AP_INFO *pApInfo    = NULL;
    ScanInfo_t          tCurrentBwlApInfo;
    NETAPP_LIST_HANDLE  hList = NULL;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);
    NETAPP_PRIV_SEM_TAKE(hNetAppWiFi->ulScanSem, tRetCode);

    if (hNetAppWiFi->pScanResultsList != NULL)
    {
        NetAppOSMemFree(hNetAppWiFi->pScanResultsList);
        hNetAppWiFi->pScanResultsList = NULL;
    }

    if (ulCount <= 0)
    {
        NETAPP_ERR(("%s Nothing Scanned !", __FUNCTION__));
        tRetCode = NETAPP_SCAN_EMPTY;
        goto err_out;
    }

    /* Search through the scan results and if we find the connected one and it
     * does not have the SSID, we will it in, this happens when the AP is hidden */
    if (hNetAppWiFi->tLinkState != NETAPP_LINK_DOWN)
    {
        BWL_API_CHECK(BWL_GetConnectedInfo(hNetAppWiFi->hBwl, &tCurrentBwlApInfo), tRetCode);
        for (i = 0; i < ulCount; i++)
        {
            if (memcmp(&tCurrentBwlApInfo.BSSID, &pData[i].BSSID, sizeof(struct ether_addr)) == 0)
            {
                NETAPP_MSG(("%s(): Found connected AP[%s] in scan results!", __FUNCTION__, tCurrentBwlApInfo.tCredentials.acSSID));
                strncpy(pData[i].tCredentials.acSSID, tCurrentBwlApInfo.tCredentials.acSSID,
                    sizeof(tCurrentBwlApInfo.tCredentials.acSSID)/sizeof(tCurrentBwlApInfo.tCredentials.acSSID[0]) -1);
                break;
            }
        }
    }


    pApInfo = (NETAPP_WIFI_AP_INFO*)NetAppOSMemAlloc(ulCount * sizeof(NETAPP_WIFI_AP_INFO));
    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s Out of memory!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }
    memset(pApInfo, 0x0, (ulCount * sizeof(NETAPP_WIFI_AP_INFO)));

    /* Sort the list then get ready to remove duplicates */
    for (i = 0; i < ulCount; i++)
    {
        NetAppWiFi_P_ConvertBwlInfo(hNetAppWiFi, &pData[i], &pApInfo[i]);
        NetAppWiFi_P_PrintApInfo(&pApInfo[i]);
    }

    NetAppWiFi_P_QuickSort(pApInfo, 0, ulCount-1);

    /* To remove duplicate we have to copy everything to a link list checking if they
     * already exist or not and if not add the node to the list. Once we are done we
     * copy back to the scan results array if only this was c++ :-(*/
    if (hNetAppWiFi->bHideDuplicateAPs)
    {
        if (NetAppListCreate(&hList, sizeof(NETAPP_WIFI_AP_NODE)) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        /* Copy the highest signal strength first to filter out the lower signal strength duplicates */
        for (i = (ulCount-1); i >= 0; i--)
        {
            NETAPP_WIFI_AP_NODE  *pNode = NULL;
            NetAppListRewind(hList);
            while ((pNode = (NETAPP_WIFI_AP_NODE*)NetAppListNext(hList)) != NULL)
            {
                if (strncmp(pNode->tApInfo.cSSID, pApInfo[i].cSSID, sizeof(pApInfo[i].cSSID)) == 0)
                {
                    break;
                }
            }
            if (pNode == NULL)
            {
                pNode = (NETAPP_WIFI_AP_NODE*)NetAppListAppend(hList);
                if (pNode != NULL)
                {
                    pNode->tApInfo = pApInfo[i];
                }
            }
        }
        /* Now we need to free the existing list and create a new one */
        NetAppOSMemFree(pApInfo);
        hNetAppWiFi->ulScanResultsCount = NetAppListCount(hList);
        pApInfo = (NETAPP_WIFI_AP_INFO*)NetAppOSMemAlloc(hNetAppWiFi->ulScanResultsCount * sizeof(NETAPP_WIFI_AP_INFO));
        if (pApInfo == NULL)
        {
            NETAPP_ERR(("%s Out of memory!", __FUNCTION__));
            tRetCode = NETAPP_OUT_OF_MEMORY;
            goto err_out;
        }

        NetAppListRewind(hList);
        for (i = (hNetAppWiFi->ulScanResultsCount-1); i >= 0; i--)
        {
            NETAPP_WIFI_AP_NODE *pNode = (NETAPP_WIFI_AP_NODE*)NetAppListNext(hList);
            if (pNode != NULL)
            {
                pApInfo[i] = pNode->tApInfo;
            }
        }
    }
    else
    {
        hNetAppWiFi->ulScanResultsCount = ulCount;
    }

    hNetAppWiFi->pScanResultsList = pApInfo;

err_out:
    if (hList != NULL)
    {
        NetAppListDestroy(hList);
    }
    NETAPP_PRIV_SEM_GIVE(hNetAppWiFi->ulScanSem);
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


static NETAPP_RETCODE NetAppWiFi_P_WaitForEvent(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_EVENT   tEvent,
    uint32_t            ulTimeout)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_FAILURE;
    struct timeval      tm;
    int32_t             result = 0;
    uint32_t            i;
    int                 bytes;
    char                replybuf[1024];
    int                 lSocket = 0;

    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    lSocket = (hNetAppWiFi->lBridgeSocket != 0) ? hNetAppWiFi->lBridgeSocket : hNetAppWiFi->lBwlSocket;
    for (i = 0; ( (i * NETAPP_WIFI_WAIT_MS) < ulTimeout) && !hNetAppWiFi->bStopBkgrndTask; i++)
    {
        tm.tv_sec  = 0;
        tm.tv_usec = NETAPP_WIFI_WAIT_MS * 1000;

        FD_ZERO(&hNetAppWiFi->tFDSet);
        FD_SET(lSocket, &hNetAppWiFi->tFDSet);

        result =  select(lSocket + 1, &hNetAppWiFi->tFDSet, NULL, NULL, &tm);

        if (hNetAppWiFi->bStopBkgrndTask)
        {
            tRetCode = NETAPP_CANCELED;
            goto err_out;
        }

        if (result <= 0)
        {
            continue;
        }


        /* Check if we got something on the Wireless interface */
        if ( (FD_ISSET(lSocket, &hNetAppWiFi->tFDSet)) && (recv(lSocket, replybuf, sizeof(replybuf), 0) > 0) )
        {
            EventMessage_t  tBwlEvent;

            if (BWL_ParseEvent(hNetAppWiFi->hBwl, replybuf, sizeof(replybuf), &tBwlEvent) != BWL_ERR_SUCCESS)
            {
                continue;
            }

            switch(tBwlEvent.id)
            {
            /* Ignoring the message from the Kernel */
            case BWL_E_PROBREQ_MSG:
                break;

            case BWL_E_ESCAN_RESULT:
            {
                break;
            }
            case BWL_E_SCAN_COMPLETE:
            {
                NETAPP_MSG(("%s(): Received BWL_E_SCAN_COMPLETE", __FUNCTION__));
                break;
            }

            case BWL_E_ASSOC:
            case BWL_E_DISASSOC:
            case BWL_E_LINK:
            {
                uint32_t            ulIsConnected = 0;
                NETAPP_LINK_STATE   tNewState = NETAPP_LINK_DOWN;

                NETAPP_MSG(("%s(): Received %d", __FUNCTION__, tBwlEvent.id));

                BWL_GetLinkStatus(hNetAppWiFi->hBwl, &ulIsConnected);

                if (!ulIsConnected)
                {
                    tNewState = NETAPP_LINK_DOWN;
                }
                else
                {
                    tNewState = NetAppPrivateIsIpAddressSet(hNetAppWiFi->hNetAppPriv, hNetAppWiFi->cIfaceName, NULL) ? NETAPP_LINK_UP : NETAPP_LINK_ACQUIRING;
                }

                if (tNewState != hNetAppWiFi->tLinkState)
                {
                    NETAPP_MSG(("%s(): WIFI: %s!", __FUNCTION__, NetAppPrivatePrintLinkState(tNewState)));
                    hNetAppWiFi->tLinkState = tNewState;

                    if (hNetAppWiFi->tState != NETAPP_STATE_CONNECTING)
                    {
                        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppWiFi->tLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                    }
                }
                break;
            }


            case BWL_E_RSSI:
            {
                int32_t lRSSI = 0;
                BWL_API_CHECK(BWL_GetRSSI(hNetAppWiFi->hBwl, &lRSSI), tRetCode);
                NETAPP_MSG(("%s(): Received BWL_E_RSSI! RSSI=%ddB", __FUNCTION__, lRSSI));
                NETAPP_DO_CALLBACK(NETAPP_CB_RSSI_EVENT, NULL, 0, NetAppWiFi_P_ConvertRSSI(hNetAppWiFi, lRSSI),
                    NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                break;
            }


            default:
                NETAPP_MSG(("%s() Received unhandled event %d", __FUNCTION__, tBwlEvent.id));
                break;
            }

            if ( ( (tBwlEvent.id == BWL_E_SCAN_COMPLETE) && (tEvent & NETAPP_WIFI_EVENT_SCAN_DONE) ) ||
                 ( (tBwlEvent.id == BWL_E_ESCAN_RESULT)  && (tEvent & NETAPP_WIFI_EVENT_SCAN_DONE) ) ||
                 ( (tBwlEvent.id == BWL_E_ASSOC)         && (tEvent & NETAPP_WIFI_EVENT_LINK_UP)   ) ||
                 ( (tBwlEvent.id == BWL_E_DISASSOC)      && (tEvent & NETAPP_WIFI_EVENT_LINK_DOWN) ) )
            {
                NETAPP_MSG(("%s() Received the Event we were waiting for!", __FUNCTION__));

                switch(tBwlEvent.result)
                {
                case BWL_ERR_TIMEOUT:
                    tRetCode = NETAPP_TIMEOUT;
                    break;
                case BWL_ERR_SUCCESS:
                    tRetCode = NETAPP_SUCCESS;
                    break;
                default:
                    tRetCode = NETAPP_FAILURE;
                    break;
                }
                break;
            }
        }
    }

    if ((i * NETAPP_WIFI_WAIT_MS) >= ulTimeout)
    {
        tRetCode = NETAPP_TIMEOUT;
    }


err_out:
    return tRetCode;
}


static NETAPP_RETCODE NetAppWiFi_P_ShutdownBackgroundTask(
    NETAPP_WIFI_HANDLE  hNetAppWiFi)
{
    NETAPP_WIFI_PRIV_API_CHECK(hNetAppWiFi);

    /* Stop all scans */
    if (NetAppWiFiPrivateStopScan(hNetAppWiFi, NETAPP_SCAN_ALL) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Cannot Stop ALL SCANS\n", __FUNCTION__));
        return NETAPP_FAILURE;
    }

    if ( (hNetAppWiFi->ulBkgrndTaskId != 0) && (hNetAppWiFi->ulMsgQId != 0) )
    {
        NETAPP_MSG_INFO tMessage;

        memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));
        tMessage.tMsgType = NETAPP_MSG_DIE;
        hNetAppWiFi->bStopBkgrndTask = true;
        if (NetAppOSMsgQSend(hNetAppWiFi->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER) == NETAPP_SUCCESS)
        {
            NetAppOSTaskJoin(hNetAppWiFi->ulBkgrndTaskId);
        }
    }

    if (hNetAppWiFi->ulBkgrndTaskId != 0)
    {
        NetAppOSTaskDelete(hNetAppWiFi->ulBkgrndTaskId);
        hNetAppWiFi->ulBkgrndTaskId = 0;
    }

    if (hNetAppWiFi->ulMsgQId != 0)
    {
        NetAppOSMsgQDelete(hNetAppWiFi->ulMsgQId);
        hNetAppWiFi->ulMsgQId = 0;
    }
    return NETAPP_SUCCESS;
}


void NetAppWiFi_P_QuickSort(
    NETAPP_WIFI_AP_INFO *items,
    int                 left,
    int                 right)
{

  register int i, j;
  int x;
  NETAPP_WIFI_AP_INFO temp;

  i = left; j = right;
  x = items[(left+right)/2].tRSSI;

  do {
    while((items[i].tRSSI < x) && (i < right)) i++;
    while((items[j].tRSSI > x) && (j > left)) j--;
    if(i <= j) {
      temp = items[i];
      items[i] = items[j];
      items[j] = temp;
      i++; j--;
    }
  } while(i <= j);
    if(left < j) NetAppWiFi_P_QuickSort(items, left, j);
    if(i < right) NetAppWiFi_P_QuickSort(items, i, right);
}

