/***************************************************************************
 *    (c)2005-2012 Broadcom Corporation
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
 * $brcm_Workfile: netapp.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp.c $
 *
 * 4   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 12   12/12/11 11:15p steven
 * SW7231-513: Various fixes
 *
 * 11   12/9/11 4:44p steven
 * SW7231-513: USB Hotplug Support in NetApp
 *
 * SW7231-513/2   12/9/11 4:26p steven
 * SW7231-513: Fix typeo
 *
 * SW7231-513/1   12/9/11 3:36p steven
 * SW7231-513: Finished working hotplug detection
 *
 * 9   12/2/11 10:40p steven
 * SW7425-1404: Fix building without bluetooth
 *
 * 8   11/24/11 5:13p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/6   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * 7   9/29/11 2:12p steven
 * SWBLURAY-27464: Add support to periodic NTP updates
 *
 * 6   9/9/11 9:08a steven
 * SWDTV-8586: fixed an issue with shutdown of the lists
 *
 * 5   8/26/11 4:02p steven
 * SWBLURAY-27043: WPS 2.0 Support
 *
 * 4   8/4/11 10:15a steven
 * SWBLURAY-26638: Moved Auto-reconnect down to NetApp
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
 * 2   6/15/11 5:27p steven
 * SW7425-714: Adding NetApp support to AppLibs
 *
 * 1   6/14/11 10:40a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * Network Application API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 */
#include "netapp_priv.h"
#include "netapp_list.h"
#include "netapp_settings.h"
#include <stdarg.h>
#include <mntent.h>
#include <syslog.h>

#ifndef HAS_WIFI
#include "netapp_wifi.h"
#endif

#ifndef NO_BLUETOOTH
#include "netapp_bt.h"
#endif

#ifndef NO_AVAHI
#include "netapp_zeroconf.h"
#endif

#ifndef NO_LIBUDEV
#include "netapp_udev.h"
#endif

#ifndef NO_VOICE_RECOGNITION
#include "netapp_http.h"
#endif

/* To allow for multiple open's we need to have a link list of callbacks */
typedef struct sNETAPP_ELEMENT
{
    NETAPP_LIST_NODE        tNode;
    NETAPP_CALLBACK         tCallback;
    void                    *pParam;
} NETAPP_ELEMENT;


typedef struct sNETAPP_CALLBACK_INFO
{
    NETAPP_CB_TYPE      tCbType;        //!< The Callback type.
    void                *pvBuffer;      //!< Pointer to data specific to the callback.
    uint32_t            ulData0;        //!< Callback specific data 0.
    NETAPP_RETCODE      tResult;        //!< Callback results from the.
    NETAPP_IFACE        tIface;         //!< Callback Info structure.
    void                *pParam;        //!< Callback param
    NETAPP_CALLBACK     tCbHandle;      //!< handle to the callback
} NETAPP_CALLBACK_INFO;

typedef struct sNETAPPHANDLE
{
    bool                    bRunningByNFS;
    uint32_t                ulSem;

    NETAPP_PRIV_HANDLE      hNetAppPriv;
#ifndef NO_BLUETOOTH
    NETAPP_BT_HANDLE        hNetAppBT;
#endif
#ifndef NO_WIFI
    NETAPP_WIFI_HANDLE      hNetAppWiFi;
    NETAPP_WFI_HANDLE       hNetAppWiFiInvite;
    NETAPP_P2P_HANDLE       hNetAppP2P;
#endif
#ifndef NO_LIBUDEV
    NETAPP_UDEV_HANDLE      hNetAppUdev;
#endif
    bool                    bDebugOn;          //!< Whether or not debug output goes to stdout
    bool                    bDebugSyslog;          //!< Whether or not debug output goes to syslog
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd;
    bool                    bClosing;

#ifndef NO_AVAHI
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf;
#endif
    NETAPP_LIST_HANDLE      hNetAppList;

    NETAPP_SETTINGS_HANDLE  hNetAppSettings;
} NETAPPHANDLE;

NETAPPHANDLE  *pNetAppHandle = NULL;


#define NETAPP_PRIV_CHECK()                                                         \
{                                                                                   \
    if (pNetAppHandle == NULL)                                                      \
    {                                                                               \
        NETAPP_ERR(("%s(): NetApp API not initialized!", __FUNCTION__));            \
        return NETAPP_NULL_PTR;                                                     \
    }                                                                               \
}

static const char *NETAPP_CALLBACK_STR[] =
{
    "NETAPP_CB_INVALID",          //!< Initialization for this enum
    "NETAPP_CB_LINK",                 //!< Link change
    "NETAPP_CB_CONNECT",              //!< Connection results (AP info passed)
    "NETAPP_CB_DISCONNECT",           //!< Disconnection results for Bluetooth
    "NETAPP_CB_INPUT_EVENT",          //!< AV Remote control from Bluetooth audio
    "NETAPP_CB_PING",                 //!< Results from a Ping request
    "NETAPP_CB_DNSLOOKUP",            //!< DNSLookup results
    "NETAPP_CB_INVITE",               //!< WiFi Invite
    "NETAPP_CB_SCAN_DONE",            //!< A scan is complete and results are available
    "NETAPP_CB_SCANNED_APINFO",       //!< A scan is complete and scanned AP info is included
    "NETAPP_CB_FETCHED_APINFO",       //!< @see NetAppWiFiGetApInfo()
    "NETAPP_CB_NTPDATE",              //!< NTPDate request is finished with results
    "NETAPP_CB_SETSETTINGS",          //!< The result from a call to NetAppSetNetworkSettings()
    "NETAPP_CB_HOTPLUG",              //!< Hotplug detected interface
    "NETAPP_CB_RSSI_EVENT",           //!< The RSSI of a connected AP changed levels
    "NETAPP_CB_ZEROCONF",             //!< NetApp has found a browse/discovery request
    "NETAPP_CB_P2P_PEER",             //!< Discovered WiFi Direct Peer information
    "NETAPP_CB_P2P_CONNECT",          //!< WiFi Direct Connection is established
    "NETAPP_CB_BT_DISCOVERY_RESULTS", //!< Bluetooth discovery is complete and results are available
    "NETAPP_CB_BT_GETDEVLIST_RESULTS",//!< Get saved bluetooth device list is complete and results are available
    "NETAPP_CB_BT_SP_CONFIRM_REQ",    //!< Simple pairing confirm request, @see NetAppBluetoothSimplePairingAck()
                                      //!< The user must then Accept or Reject the SP request.
    "NETAPP_CB_BT_SP_NOTIFY",         //!< Notify the application that the value passed in ulData0 of the
                                      //!< callback must be entered in the bluetooth device we are trying to
                                      //!< connect to.
    "NETAPP_CB_BT_AUTH_COMPLETE",     //!< Authentication complete event
    "NETAPP_CB_BT_HID_VOICE_INFO",    //!< Notify the application a file has been created for hid audio
    "NETAPP_CB_VOICE_REC_DONE",       //!< Finished a voice recognition request and a string is available
    "NETAPP_CB_DHCP_LEASE_RESPONSE",  //!< Responded to a DHCP lease request when we are a DHCP server (SoftAP or P2P)
    "NETAPP_CB_BT_AVK_STATE"          //!< AVK playback state
    "NETAPP_CB_BT_AVK_CHUNK",         //!< Received an AVK chunk that must be pushed to the playback engine.
    "NETAPP_CB_DYING"                 //!< NetApp encountered a fatal error and cannot recover.
};

#define NETAPP_API_CHECK(tHandle)                                                   \
{                                                                                   \
    NETAPP_ELEMENT *pCurrent = NULL;                                                \
    NETAPP_PRIV_CHECK();                                                            \
    if (tHandle == NULL)                                                            \
    {                                                                               \
        NETAPP_ERR(("%s(): Handle or API not initialized!", __FUNCTION__));         \
        return NETAPP_NULL_PTR;                                                     \
    }                                                                               \
    if (NetAppListIsValid(pNetAppHandle->hNetAppList, (NETAPP_LIST_NODE*)tHandle) != NETAPP_SUCCESS)\
    {                                                                               \
        NETAPP_ERR(("%s(): Invalid NetApp Handle!", __FUNCTION__));                 \
        return NETAPP_NULL_PTR;                                                     \
    }                                                                               \
}


/* Release the Mutex */
#define NETAPP_SEM_GIVE()                                                           \
{                                                                                   \
    if (NetAppOSSemGive(pNetAppHandle->ulSem) != NETAPP_SUCCESS)                    \
    {                                                                               \
        NETAPP_ERR(("%s() SemGive Failed! Line:%d", __FUNCTION__, __LINE__));       \
    }                                                                               \
}


#define NETAPP_SEM_TAKE(tRetCode)                                                   \
{                                                                                   \
    if (NetAppOSSemTake(pNetAppHandle->ulSem) != NETAPP_SUCCESS)                    \
    {                                                                               \
        NETAPP_ERR(("%s() SemTake Failed! Line:%d", __FUNCTION__, __LINE__));       \
        tRetCode = NETAPP_FAILURE;                                                  \
        goto err_out;                                                               \
    }                                                                               \
    if (pNetAppHandle == NULL)                                                      \
    {                                                                               \
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));                      \
        tRetCode = NETAPP_NULL_PTR;                                                 \
        goto err_out;                                                               \
    }                                                                               \
}

static NETAPP_INIT_SETTINGS DEFAULT_INIT_SETTINGS =
{
    "Broadcom NetApp",          //!< Device Name
    "wln",                      //!< WiFi interface Name
    false,                      //!< Allow NFS
    false,                      //!< Burst Scan Results
    NULL,                       //!< Country Code
    "Broadcom",                 //!< Manuactuer
    "NetApp",                   //!< Model
    "20110826",                 //!< ModelNumber
    "010203",                   //!< Serial Number
    { 0x22, 0x21, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xa, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }, //!< WPS UUID
    { 0x12, 0x32, 0x6F, 0xC9, 0xB3, 0xBB, 0x10, 0x0c, 0x9D, 0x37 }, //!< Transport UUID
    "/tmp",         /* Default Database path */
    NETAPP_MIMO_BW_CAP_20MHZ_40MHz, /* tMimoBwCap*/
};

static NETAPP_SETTINGS DEFAULT_SETTINGS =
{
    false,                      //!< bZeroconfOn
    true,                       //!< auto-reconnect
    false,                      //!< Force WiFi enabled
    true,                       //!< WPS 2.0 support
    true,                       //!< bHideDuplicateAPs
    {                           //!< WoWL support
        NETAPP_WOWL_EVENT_NONE, //!< Magic Pattern wakeup
        {},                              //!< Net Patter Info
        0,                               //!< Beacon time loss
    },
    /* Bluetooth Settings */
    {
        true,       /* Discoverable */
        true,       /* Auto Pair */
        {'0', '0', '0', '0'}, /* Test PIN */
        4,
    },
    /* P2P default settings */
    {
        -1,         /* lTimeoutSec default */
        -1,         /* lScanTimeMs default */
        NETAPP_P2P_SVC_ALL,  /* All service types */
        0           /* ulSocialCh the default */
    },
    8,              /* ucP2PGOIntent */
    false,           /* Enable Auto P2P Discovery */
    false,           /* bWiFiBridgingMode */
    false,          /* Is SoftAP yes or no */
    {               /* SoftAP Settings */
        {
            "Broadcom-NetApp",      //!< Service Set Identifier (BSSID)
            "",                     //! Basic Service set Identifier (BSSID)
            "1234567890",           //!< Password
            NETAPP_WIFI_RSSI_NONE,  //!< Received Signal Strength Indicator (generalized)
            0,                      //!< Received Signal Strength Indicator (-db)
            0,                      //!< Supported 802.11 modes (a, b, g, n, ac, etc...)
            NETAPP_WIFI_SECURITY_WPA2_PSK_AES,//!< Security modes supported
            false,                  //!< AdHoc network or not
            true,                   //!< AP supports/implements WPS
            11,                     //!< Channel AP is configured on
        },
        0x0110a8c0,                 //!< IP Address: 192.168.16.1
        0x00ffffff                  //!< Subnet Mask: 255.255.255.255.0
    },
};

static void NetApp_P_TestTimer(
    void                    *pParam);


static void NetApp_P_CallbackThread(
    void              *pParam);


NETAPP_RETCODE NetAppGetVersionInfo(
    NETAPP_VERSION_INFO * pVersionInfo)
{
    if (pVersionInfo == NULL)
    {
        return NETAPP_INVALID_PARAMETER;
    }

    pVersionInfo->ulMajor = NETAPP_VERSION_MAJOR;
    pVersionInfo->ulMinor = NETAPP_VERSION_MINOR;
    pVersionInfo->ulInc = NETAPP_VERSION_INC;

    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppGetDefaultSettings(
    NETAPP_SETTINGS     *pSettings)
{
    if (pSettings == NULL)
    {
        return NETAPP_INVALID_PARAMETER;
    }
    *pSettings = DEFAULT_SETTINGS;
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppGetDefaultInitSettings(
    NETAPP_INIT_SETTINGS     *pSettings)
{
    if (pSettings == NULL)
    {
        return NETAPP_INVALID_PARAMETER;
    }
    *pSettings = DEFAULT_INIT_SETTINGS;
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppSetSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_SETTINGS     tSettings)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppPrivateSetSettings(pNetAppHandle->hNetAppPriv, &tSettings), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}



NETAPP_RETCODE NetAppGetSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_SETTINGS     *pSettings)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;

    if (pSettings == NULL)
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(pNetAppHandle->hNetAppPriv, pSettings), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppOpen(
    NETAPP_HANDLE       *tHandle,
    NETAPP_OPEN_SETTINGS    *pOpenSettings,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;
    struct stat             statbuf;
    uint32_t                i;
    bool                    bInit = false;
    NETAPP_ELEMENT         *pNewElem = NULL;

    /* First time calling NetAppOpen */
    if (pNetAppHandle == NULL)
    {
        FILE    *pFd = NULL;
        bInit = true;

        pNetAppHandle = (NETAPPHANDLE*)NetAppOSMemAlloc(sizeof(NETAPPHANDLE));
        if (pNetAppHandle == NULL)
        {
            NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
            tRetCode = NETAPP_OUT_OF_MEMORY;
            goto err_out;
        }

        NETAPP_ERR(("%s(): Version %d.%d.%d", __FUNCTION__, NETAPP_VERSION_MAJOR, NETAPP_VERSION_MINOR, NETAPP_VERSION_INC));

        memset(pNetAppHandle, 0, sizeof(NETAPPHANDLE));

        pNetAppHandle->bDebugOn = (getenv("NETAPP_DEBUG") != NULL);
        pNetAppHandle->bDebugSyslog = (getenv("NETAPP_DEBUG_SYSLOG") != NULL);

        if (NetAppListCreate(&pNetAppHandle->hNetAppList, sizeof(NETAPP_ELEMENT)) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        pNetAppHandle->ulSem = NetAppOSSemBCreate();
        if (pNetAppHandle->ulSem == 0)
        {
            NETAPP_ERR(("%s() -- Cannot create sem!", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        if (NetAppOSSemTake(pNetAppHandle->ulSem) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s() SemTake Failed! Line:%d", __FUNCTION__, __LINE__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        /* Check if we have app mounted by NFS */
        if ( !((pInitSettings != NULL) && (pInitSettings->bAllowNFS)) )
        {
            pFd = setmntent("/etc/mtab", "r");
            if (pFd != NULL)
            {
                struct mntent *pMnt = NULL;
                while ( (pMnt = getmntent(pFd)) != NULL)
                {
                    if (strcmp(pMnt->mnt_type, "nfs") == 0)
                    {
                        NETAPP_MSG(("%s() Running by NFS!", __FUNCTION__));
                        pNetAppHandle->bRunningByNFS = true;
                        break;
                    }
                }
                fclose(pFd);
            }
        }


        if (NetAppDhcpcdInit(&pNetAppHandle->hNetAppDhcpcd) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init Private component\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }


        if (NetAppPrivateInit(&pNetAppHandle->hNetAppPriv, pNetAppHandle->hNetAppDhcpcd,
            (pSettings != NULL) ? pSettings : &DEFAULT_SETTINGS) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init Private component\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }


        if (NetAppSettingsPrivateInit(&pNetAppHandle->hNetAppSettings,
            (pInitSettings != NULL) ? pInitSettings->pDBPath : DEFAULT_INIT_SETTINGS.pDBPath) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init Settings backend!\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

#ifndef NO_BLUETOOTH
        if (NetAppBluetoothPrivateInit(&pNetAppHandle->hNetAppBT, pNetAppHandle->hNetAppPriv, pNetAppHandle->hNetAppSettings) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init Bluetooth!\n", __FUNCTION__));
            // Do not fail Init, we just disable Bluetooth;
        }
#endif

#ifndef NO_WIFI
#ifndef NO_WIFI_P2P
        if (NetAppWiFiP2PPrivateInit(&pNetAppHandle->hNetAppP2P, pNetAppHandle->hNetAppPriv,
            (pInitSettings != NULL) ? pInitSettings : &DEFAULT_INIT_SETTINGS,
            (pSettings != NULL) ? pSettings : &DEFAULT_SETTINGS, NETAPP_DEVICE_TYPE_BD,
            pNetAppHandle->hNetAppDhcpcd) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init WiFi Direct\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
#endif

#ifndef NO_WIFI_INVITE
        if (NetAppWiFiInvitePrivateInit(&pNetAppHandle->hNetAppWiFiInvite, pNetAppHandle->hNetAppPriv,
            (pSettings != NULL) ? pSettings : &DEFAULT_SETTINGS) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init WiFi component\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
#endif
        if (NetAppWiFiPrivateInit(&pNetAppHandle->hNetAppWiFi, pNetAppHandle->hNetAppWiFiInvite, pNetAppHandle->hNetAppP2P,
            pNetAppHandle->hNetAppPriv, pNetAppHandle->hNetAppDhcpcd, pNetAppHandle->hNetAppSettings,
            (pInitSettings != NULL) ? pInitSettings : &DEFAULT_INIT_SETTINGS,
            (pSettings != NULL) ? pSettings : &DEFAULT_SETTINGS) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Cannot Init WiFi component\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

#endif

#ifndef NO_AVAHI
        NETAPP_PRIV_API_CHECK(NetAppZeroConfPrivateInit(&pNetAppHandle->hNetAppZeroConf,
            pNetAppHandle->hNetAppPriv,
            (pInitSettings != NULL) ? pInitSettings : &DEFAULT_INIT_SETTINGS,
            (pSettings != NULL) ? pSettings : &DEFAULT_SETTINGS), tRetCode);
#endif

#ifndef NO_LIBUDEV
        NETAPP_PRIV_API_CHECK(NetAppUdevPrivateInit(&pNetAppHandle->hNetAppUdev), tRetCode);
#endif

        NETAPP_MSG(("%s() -- Finished creating pNetApp Handle!", __FUNCTION__));
    }
    else
    {
        if (NetAppOSSemTake(pNetAppHandle->ulSem) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s() SemTake Failed! Line:%d", __FUNCTION__, __LINE__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
    }


    if (pOpenSettings != NULL)
    {
        pNewElem = (NETAPP_ELEMENT*)NetAppListAppend(pNetAppHandle->hNetAppList);

        if (pNewElem == NULL)
        {
            NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
            tRetCode = NETAPP_OUT_OF_MEMORY;
            goto err_out;
        }

        pNewElem->tCallback = pOpenSettings->tCallback;
        pNewElem->pParam    = pOpenSettings->pParam;
    }

    *tHandle = pNewElem;

    NETAPP_MSG(("%s() -- Leaving!", __FUNCTION__));
    NETAPP_SEM_GIVE();

    return NETAPP_SUCCESS;

err_out:
    if (pNewElem != NULL)
    {
        NetAppListRemove(pNetAppHandle->hNetAppList, (NETAPP_LIST_NODE*)pNewElem);
    }

    if (bInit)
    {
        pNetAppHandle->bClosing = true;

        if (pNetAppHandle->hNetAppDhcpcd != NULL)
        {
            NetAppDhcpcdDeInit(pNetAppHandle->hNetAppDhcpcd);
            pNetAppHandle->hNetAppDhcpcd = NULL;
        }

#ifndef NO_LIBUDEV
        if (pNetAppHandle->hNetAppUdev != NULL)
        {
            NetAppUdevPrivateDeinit(pNetAppHandle->hNetAppUdev);
        }
#endif

#ifndef NO_WIFI
        if (pNetAppHandle->hNetAppWiFi != NULL)
        {
            NetAppWiFiPrivateDeinit(pNetAppHandle->hNetAppWiFi);
            pNetAppHandle->hNetAppWiFi = NULL;
        }
#ifndef NO_WIFI_INVITE
        if (pNetAppHandle->hNetAppWiFiInvite != NULL)
        {
            NetAppWiFiInvitePrivateDeInit(pNetAppHandle->hNetAppWiFiInvite);
            pNetAppHandle->hNetAppWiFiInvite = NULL;
        }
#endif
#endif
        if (pNetAppHandle->hNetAppPriv != NULL)
        {
            NetAppPrivateDeInit(pNetAppHandle->hNetAppPriv);
            pNetAppHandle->hNetAppPriv = NULL;
        }

        if (pNetAppHandle->hNetAppList != NULL)
        {
            NetAppListDestroy(pNetAppHandle->hNetAppList);
            pNetAppHandle->hNetAppList = NULL;
        }

        if (pNetAppHandle->ulSem)
        {
            NetAppOSSemDelete(pNetAppHandle->ulSem);
            pNetAppHandle->ulSem = 0;
        }

        NetAppOSMemFree(pNetAppHandle);
        pNetAppHandle = NULL;
    }
    else
    {
        NETAPP_SEM_GIVE();
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppSetDebugOutput(
    NETAPP_HANDLE tHandle,
    uint32_t ulDest)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);

    NETAPP_SEM_TAKE(tRetCode);

    pNetAppHandle->bDebugOn = ((NETAPP_DEBUG_DESTINATION_STDOUT & ulDest) == NETAPP_DEBUG_DESTINATION_STDOUT);
    pNetAppHandle->bDebugSyslog = ((NETAPP_DEBUG_DESTINATION_SYSLOG & ulDest) == NETAPP_DEBUG_DESTINATION_SYSLOG);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppClose(NETAPP_HANDLE tHandle)
{
    int         i;
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    struct sNETAPP_ELEMENT *pCurrent  = NULL;
    struct sNETAPP_ELEMENT *pPrevious = NULL;

    NETAPP_API_CHECK(tHandle);

    NETAPP_SEM_TAKE(tRetCode);

    NetAppListRemove(pNetAppHandle->hNetAppList, (NETAPP_LIST_NODE*)tHandle);

    /* No more references so tear down NetApp */
    if (NetAppListCount(pNetAppHandle->hNetAppList) == 0)
    {
        pNetAppHandle->bClosing = true;


        if (pNetAppHandle->hNetAppList != NULL)
        {
            NetAppListDestroy(pNetAppHandle->hNetAppList);
            pNetAppHandle->hNetAppList = NULL;
        }

#ifndef NO_LIBUDEV
        if (pNetAppHandle->hNetAppUdev != NULL)
        {
            NetAppUdevPrivateDeinit(pNetAppHandle->hNetAppUdev);
        }
#endif

#ifndef NO_BLUETOOTH
        if (pNetAppHandle->hNetAppBT != NULL)
        {
            NetAppBluetoothPrivateDeinit(pNetAppHandle->hNetAppBT);
        }
#endif

#ifndef NO_AVAHI
        if (pNetAppHandle->hNetAppZeroConf != NULL)
        {
            NetAppZeroConfPrivateDeInit(pNetAppHandle->hNetAppZeroConf);
            pNetAppHandle->hNetAppZeroConf = NULL;
        }
#endif

#ifndef NO_WIFI
        if (pNetAppHandle->hNetAppWiFi != NULL)
        {
            NetAppWiFiPrivateDeinit(pNetAppHandle->hNetAppWiFi);
            pNetAppHandle->hNetAppWiFi = NULL;
        }

#ifndef NO_WIFI_INVITE
        if (pNetAppHandle->hNetAppWiFiInvite != NULL)
        {
            NetAppWiFiInvitePrivateDeInit(pNetAppHandle->hNetAppWiFiInvite);
            pNetAppHandle->hNetAppWiFiInvite = NULL;
        }
#endif

#ifndef NO_WIFI_P2P
        if (pNetAppHandle->hNetAppP2P != NULL)
        {
            NetAppWiFiP2PPrivateDeInit(pNetAppHandle->hNetAppP2P);
            pNetAppHandle->hNetAppP2P = NULL;
        }
#endif

#endif
        if (!pNetAppHandle->bRunningByNFS)
        {
            NetAppPrivateClearIPSettings(pNetAppHandle->hNetAppPriv, NETAPP_IFACE_ETH0);
        }

        if (pNetAppHandle->hNetAppPriv != NULL)
        {
            NetAppPrivateDeInit(pNetAppHandle->hNetAppPriv);
            pNetAppHandle->hNetAppPriv = NULL;
        }

        if (pNetAppHandle->hNetAppDhcpcd != NULL)
        {
            NetAppDhcpcdDeInit(pNetAppHandle->hNetAppDhcpcd);
            pNetAppHandle->hNetAppDhcpcd = NULL;
        }

        if (pNetAppHandle->ulSem)
        {
            NetAppOSSemDelete(pNetAppHandle->ulSem);
        }

        NETAPP_MSG(("%s(): Finished and Exiting", __FUNCTION__));
        NetAppOSMemFree(pNetAppHandle);
        pNetAppHandle = NULL;

        return tRetCode;
    }

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppSetNetworkSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_RETCODE      tRetCode        = NETAPP_SUCCESS;
    char                *pIfaceName     = NULL;
    NETAPP_LINK_STATE   tLinkState      = NETAPP_LINK_DOWN;

    NETAPP_MSG(("%s Entered->%s!", __FUNCTION__,
            (tMode == NETAPP_IP_MODE_OFF) ? "OFF" : (tMode == NETAPP_IP_MODE_STATIC) ? "STATIC" : "DYNAMIC"));

    if ( (pSettings == NULL) && (tMode == NETAPP_IP_MODE_STATIC) )
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    switch (tIface)
    {
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        if (!pNetAppHandle->bRunningByNFS)
        {
            NETAPP_PRIV_API_CHECK(NetAppPrivateSetWiredSettings(pNetAppHandle->hNetAppPriv, tMode, tIface, pSettings), tRetCode);
        }
#ifndef NO_WIFI
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetNetworkSettings(pNetAppHandle->hNetAppWiFi, NETAPP_IP_MODE_OFF, NULL), tRetCode);
#ifndef NO_WIFI_P2P
        NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateDisconnect(pNetAppHandle->hNetAppP2P), tRetCode);
#endif
        break;

    case NETAPP_IFACE_WIRELESS:
        if (!pNetAppHandle->bRunningByNFS)
        {
// Do not disable other interfaces
//            NETAPP_PRIV_API_CHECK(NetAppPrivateSetWiredSettings(pNetAppHandle->hNetAppPriv, NETAPP_IP_MODE_OFF, NULL), tRetCode);
        }
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetNetworkSettings(pNetAppHandle->hNetAppWiFi, tMode, pSettings), tRetCode);
        break;
#ifndef NO_WIFI_P2P
    case NETAPP_IFACE_P2P:
        NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateSetNetworkSettings(pNetAppHandle->hNetAppP2P, tMode, pSettings), tRetCode);
#endif
#endif
        break;

    default:
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        break;
    }

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!, result=%d", __FUNCTION__, tRetCode));
    return (tRetCode);
}


NETAPP_RETCODE NetAppSetMacAddress(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    char                *pMacAddress)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    const char      *pIfaceName = NULL;

    if (pMacAddress == NULL)
    {
        NETAPP_ERR(("%s(): Null Pointer!", __FUNCTION__));
        return NETAPP_NULL_PTR;
    }

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    switch (tIface)
    {
#ifndef NO_WIFI
    case NETAPP_IFACE_WIRELESS:
        pIfaceName = NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi);
        break;
#endif
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        pIfaceName = NETAPP_WIRED_IFACE_NAME[tIface];
        break;
    case NETAPP_IFACE_LOOPBACK:
        pIfaceName = NETAPP_IFACE_LO;
        break;
    default:
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }
    tRetCode = NetAppPrivateSetMacAddress(pNetAppHandle->hNetAppPriv, pIfaceName, pMacAddress);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!, result=%d", __FUNCTION__, tRetCode));
    return (tRetCode);
}


NETAPP_RETCODE NetAppGetNetworkSettings(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    const char          *pIfaceName = NULL;

    NETAPP_MSG(("%s() Entered Interface:%d", __FUNCTION__,tIface));

    NETAPP_API_CHECK(tHandle);

    if (pSettings == NULL)
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        return (NETAPP_INVALID_PARAMETER);
    }
    NETAPP_SEM_TAKE(tRetCode);

    memset(pSettings, 0, sizeof(NETAPP_IP_SETTINGS));

    switch(tIface)
    {
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        pIfaceName = NETAPP_WIRED_IFACE_NAME[tIface];
        break;
#ifndef NO_WIFI
    case NETAPP_IFACE_WIRELESS:
        pIfaceName = NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi);
        break;

#ifndef NO_WIFI_P2P
    case NETAPP_IFACE_P2P:
        pIfaceName = NetAppWiFiP2PPrivateGetIfaceName(pNetAppHandle->hNetAppP2P);
        break;
#endif
#endif
    default:
        NETAPP_ERR(("%s() Interface Not Supported!", __FUNCTION__));
        tRetCode = NETAPP_NOT_SUPPORTED;
        goto err_out;
    }

    tRetCode = NetAppPrivateGetIPSettings(pNetAppHandle->hNetAppPriv, pIfaceName, pSettings);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}



NETAPP_RETCODE NetAppPing(
    NETAPP_HANDLE   tHandle,
    int32_t         lTimeoutMs,
    const char      *pcIpAddress)
{
    NETAPP_API_CHECK(tHandle);
    if (pcIpAddress == NULL)
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_MSG(("%s Entered, pinging %s!", __FUNCTION__, pcIpAddress));

    return NetAppPrivatePing(pNetAppHandle->hNetAppPriv, pcIpAddress, lTimeoutMs);
}


NETAPP_RETCODE NetAppDNSLookup(
    NETAPP_HANDLE       tHandle,
    const char          *pcHostname)
{
    NETAPP_RETCODE  tRetCode   = NETAPP_FAILURE;

    NETAPP_API_CHECK(tHandle);

    if (pcHostname == NULL)
    {
        NETAPP_ERR(("%s() Invalid Parameter!", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    tRetCode = NetAppPrivateDNSLookup(pNetAppHandle->hNetAppPriv, pcHostname);
err_out:
    return tRetCode;
}


char* NetAppNtoA(NETAPP_IPV4_ADDR tAddress)
{
    struct in_addr  addr;

    addr.s_addr = tAddress;
    return inet_ntoa(addr);
}

NETAPP_IPV4_ADDR NetAppAtoN(char *pcString)
{
    struct in_addr  addr;

    if (pcString == NULL)
    {
        NETAPP_ERR(("%s Null Pointer!", __FUNCTION__));
        return 0;
    }

    return (inet_aton(pcString, &addr) != 0) ? addr.s_addr : 0;
}


char* NetAppHwAddrToA(
    NETAPP_HW_ADDR      tHwAddr,
    char                *pcString,
    uint32_t            ulLength)
{
    if ( (pcString == NULL) || (ulLength <= NETAPP_ENET_LEN) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter ulLength=%d, NETAPP_ENET_LEN=%d", __FUNCTION__, ulLength, NETAPP_ENET_LEN));
        return NULL;
    }

    snprintf(pcString, ulLength, "%02X:%02X:%02X:%02X:%02X:%02X",
        tHwAddr[0], tHwAddr[1], tHwAddr[2], tHwAddr[3], tHwAddr[4], tHwAddr[5]);
    return pcString;
}


NETAPP_RETCODE NetAppAToHwAddr(
    char                *pcString,
    NETAPP_HW_ADDR      tHwAddr)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    char *pRunning = NULL;
    char *pBuffer = NULL;
    char *token = NULL;
    int j;

    memset(tHwAddr, 0, sizeof(tHwAddr));

    if ( (pcString == NULL) || (strlen(pcString) < NETAPP_ENET_LEN) )
    {
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    pBuffer = strdup(pcString);
    pRunning = pBuffer;
    for (j = 0; (token = strsep(&pRunning, ":")) != NULL; j++)
    {
        tHwAddr[j] = strtol(token, (char**) NULL, 16);
    }

err_out:
    if (pBuffer != NULL)
    {
        free(pBuffer);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppGetLinkState(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    NETAPP_LINK_STATE   *pLink)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);

    if ( (pLink == NULL) || (tIface >= NETAPP_IFACE_MAX) )
    {
        NETAPP_ERR(("%s(): Invalid Argument!", __FUNCTION__));
        return (NETAPP_NULL_PTR);
    }
    NETAPP_SEM_TAKE(tRetCode);

    switch(tIface)
    {
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        *pLink = NetAppPrivateGetLinkState(pNetAppHandle->hNetAppPriv, tIface);
        break;

#ifndef NO_WIFI
    case NETAPP_IFACE_WIRELESS:
        *pLink = NetAppWiFiPrivateGetLinkState(pNetAppHandle->hNetAppWiFi);
        break;

#ifndef NO_WIFI_P2P
    case NETAPP_IFACE_P2P:
        *pLink = NetAppWiFiP2PPrivateGetLinkState(pNetAppHandle->hNetAppP2P);
        break;
#endif
#endif
    default:
        NETAPP_ERR(("%s() Interface Not Supported!", __FUNCTION__));
        tRetCode = NETAPP_NOT_SUPPORTED;
        goto err_out;
    }

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppNtpSetDate(
    NETAPP_HANDLE       tHandle,
    uint32_t            ulPeriodMs)
{
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;

    NETAPP_API_CHECK(tHandle);

    NETAPP_SEM_TAKE(tRetCode);

    tRetCode = NetAppPrivateNtpSetDate(pNetAppHandle->hNetAppPriv, ulPeriodMs);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppSetIfaceUp(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    bool                bUp)
{
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;
    const char      *pIfaceName = NULL;

    NETAPP_API_CHECK(tHandle);

    NETAPP_SEM_TAKE(tRetCode);
    switch(tIface)
    {
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        pIfaceName = NETAPP_WIRED_IFACE_NAME[tIface];
        break;
    case NETAPP_IFACE_LOOPBACK:
        pIfaceName = NETAPP_IFACE_LO;
        break;
#ifndef NO_WIFI
    case NETAPP_IFACE_WIRELESS:
        pIfaceName = NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi);
        break;
#endif
    default:
        NETAPP_ERR(("%s() Interface Not Supported!", __FUNCTION__));
        tRetCode = NETAPP_NOT_SUPPORTED;
        goto err_out;
    }

    tRetCode = NetAppPrivateSetInterfaceUp(pNetAppHandle->hNetAppPriv, pIfaceName, bUp);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppGetIfaceName(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        tIface,
    char                **pString)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    const char      *pIfaceName = NULL;

    NETAPP_API_CHECK(tHandle);

    if (pString == NULL)
    {
        NETAPP_ERR(("%s() Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_SEM_TAKE(tRetCode);

    switch(tIface)
    {
    case NETAPP_IFACE_ETH0:
    case NETAPP_IFACE_ETH1:
    case NETAPP_IFACE_ETH2:
    case NETAPP_IFACE_ETH3:
    case NETAPP_IFACE_ETH4:
    case NETAPP_IFACE_ETH5:
    case NETAPP_IFACE_BR0:
        pIfaceName = NETAPP_WIRED_IFACE_NAME[tIface];
        break;
    case NETAPP_IFACE_LOOPBACK:
        pIfaceName = NETAPP_IFACE_LO;
        break;
#ifndef NO_WIFI
    case NETAPP_IFACE_WIRELESS:
        pIfaceName = NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi);
        break;

#ifndef NO_WIFI_P2P
    case NETAPP_IFACE_P2P:
        pIfaceName = NetAppWiFiP2PPrivateGetIfaceName(pNetAppHandle->hNetAppP2P);
        break;
#endif
#endif
    default:
        NETAPP_ERR(("%s() Interface Not Supported!", __FUNCTION__));
        tRetCode = NETAPP_NOT_SUPPORTED;
        goto err_out;
    }

    *pString = strdup(pIfaceName);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppGetDefaultIface(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE        *pIface)
{
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;
    char            cIfaceName[NETAPP_IFACE_NAME_LEN+1];

    NETAPP_API_CHECK(tHandle);

    if (pIface == NULL)
    {
        NETAPP_ERR(("%s() Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    memset(cIfaceName, 0, sizeof(cIfaceName));

    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetDefaultInterface(pNetAppHandle->hNetAppPriv, cIfaceName), tRetCode);

    if (strncmp(cIfaceName, NETAPP_IFACE_ETH, NETAPP_IFACE_ETH_LEN) == 0)
    {
        *pIface = atoi(cIfaceName + NETAPP_IFACE_ETH_LEN);
    }
    else
    {
#ifndef NO_WIFI
        *pIface = NETAPP_IFACE_WIRELESS;
#else
        *pIface = NETAPP_IFACE_MAX;
#endif
    }

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


void NetAppPrivateDoCallback(
    NETAPP_CB_TYPE      tCbType,
    const void          *pvBuffer,
    uint32_t            ulBufferLength,
    uint32_t            ulData0,
    NETAPP_RETCODE      tResult,
    NETAPP_IFACE        tIface)
{
    struct sNETAPP_ELEMENT  *pCurrent = NULL;
    NETAPP_RETCODE          tRetCode  = NETAPP_SUCCESS;
    uint32_t                ulCallbackThread = 0;
    NETAPP_CALLBACK_INFO    *pCbInfo = NULL;

    if (pNetAppHandle == NULL)
    {
        NETAPP_ERR(("%s(): NetApp API not initialized!", __FUNCTION__));
        tRetCode = NETAPP_NULL_PTR;
        goto err_out;
    }

    if (sizeof(NETAPP_CALLBACK_STR)/sizeof(NETAPP_CALLBACK_STR[0]) < NETAPP_CB_MAX)
    {
        NETAPP_ERR(("******************************************************************"));
        NETAPP_ERR((" STEVEN YOU NEED TO UPDATE NETAPP_CALLBACK_STR!!!"));
        NETAPP_ERR(("******************************************************************"));
        return;
    }

    /* Ensure that we do not send callbacks when we are closing */
    if (pNetAppHandle->bClosing)
    {
        NETAPP_MSG(("%s():Dropping [%s]", __FUNCTION__, NETAPP_CALLBACK_STR[tCbType]));
        return;
    }

    NETAPP_MSG(("%s(): Entered callback [%s]", __FUNCTION__, NETAPP_CALLBACK_STR[tCbType]));
    switch(tCbType)
    {
    /* We use this to secretly process internal broadcast callbacks defined in NETAPP_CB_PRIV_TYPE */
    case NETAPP_CB_INVALID:
        switch (ulData0)
        {
        case NETAPP_CB_PRIV_SETTINGS_REFRESH:
#ifndef NO_BLUETOOTH
            NetAppBluetoothPrivateSettingsRefresh(pNetAppHandle->hNetAppBT);
#endif
#ifndef NO_WIFI
#ifndef NO_WIFI_INVITE
            NetAppWiFiInvitePrivateSettingsRefresh(pNetAppHandle->hNetAppWiFiInvite);
#endif
            NetAppWiFiPrivateSettingsRefresh(pNetAppHandle->hNetAppWiFi);
#endif
            break;
        default:
            break;
        }
        return;
        break;


    case NETAPP_CB_HOTPLUG:
        NETAPP_SEM_TAKE(tRetCode);
#ifndef NO_BLUETOOTH
        if (tIface == NETAPP_IFACE_MAX)
        {
            NETAPP_HOTPLUG_DEVICE_INFO *pInfo = (NETAPP_HOTPLUG_DEVICE_INFO*)pvBuffer;
            if ( (pInfo != NULL) && (pInfo->tType == NETAPP_HOTPLUG_DEVICE_BLUETOOTH) )
            {
                NetAppBluetoothPrivateHotPlug(pNetAppHandle->hNetAppBT, !NetAppBluetoothPrivateIsEnabled(pNetAppHandle->hNetAppBT));
            }
        }
#endif
#ifndef NO_WIFI
        if (tIface == NETAPP_IFACE_WIRELESS)
        {
            char cIface[NETAPP_IFACE_NAME_LEN+1];
            bool bPresent = (NetAppWiFiPrivateIsPresent(cIface, sizeof(cIface)) == NETAPP_SUCCESS);
            bool bEnabled = NetAppWiFiPrivateIsEnabled(pNetAppHandle->hNetAppWiFi);

            if ( (bPresent && bEnabled) || (!bPresent && !bEnabled) )
            {
                NETAPP_MSG(("%s(): Drop this callback internally", __FUNCTION__));
                NETAPP_SEM_GIVE();
                return;
            }

            NetAppWiFiPrivateHotPlug(pNetAppHandle->hNetAppWiFi, cIface, strlen(cIface)+1, bPresent);
        }
#endif
        NETAPP_SEM_GIVE();
        break;

#ifndef NO_AVAHI
    case NETAPP_CB_LINK:
        if (ulData0 == NETAPP_LINK_UP)
        {
            NETAPP_SETTINGS tSettings;
            NetAppPrivateGetSettings(pNetAppHandle->hNetAppPriv, &tSettings);
            if (tSettings.bZeroconfOn)
            {
                NetAppZeroConfPrivateStart(pNetAppHandle->hNetAppZeroConf);
            }
        }
        else
        {
            if ( !( (tIface == NETAPP_IFACE_WIRELESS) && (NetAppPrivateGetLinkState(pNetAppHandle->hNetAppPriv, NETAPP_IFACE_WIRED_MAX) == NETAPP_LINK_UP) ) )
            {
                NetAppZeroConfPrivateStop(pNetAppHandle->hNetAppZeroConf);
            }
        }
        break;
#endif
    case NETAPP_CB_SETSETTINGS:

    default:
        break;
    }

    NetAppListRewind(pNetAppHandle->hNetAppList);

    while ((pCurrent = (NETAPP_ELEMENT*)NetAppListNext(pNetAppHandle->hNetAppList)) != NULL)
    {
        if (pCurrent->tCallback != NULL)
        {
            pCbInfo = (NETAPP_CALLBACK_INFO*)NetAppOSMemAlloc(sizeof(NETAPP_CALLBACK_INFO));
            if (pCbInfo == NULL)
            {
                NETAPP_ERR(("%s(): Cannot allocate callback context", __FUNCTION__));
                goto error;
            }

            memset(pCbInfo, 0, sizeof(NETAPP_CALLBACK_INFO));

            if (ulBufferLength > 0)
            {
                pCbInfo->pvBuffer = NetAppOSMemAlloc(ulBufferLength);
                if (pCbInfo->pvBuffer == NULL)
                {
                    NETAPP_ERR(("%s(): Cannot allocate callback context pointer", __FUNCTION__));
                    goto error;
                }
                memcpy(pCbInfo->pvBuffer, pvBuffer, ulBufferLength);
            }
            else
            {
                pCbInfo->pvBuffer = NULL;
            }

            pCbInfo->tCbType    = tCbType;
            pCbInfo->tIface     = tIface;
            pCbInfo->tResult    = tResult;
            pCbInfo->ulData0    = ulData0;
            pCbInfo->pParam     = pCurrent->pParam;
            pCbInfo->tCbHandle  = pCurrent->tCallback;
            ulCallbackThread = NetAppOSTaskSpawn("NetApp_P_CallbackThread", NETAPP_OS_PRIORITY_LOW,
                                       256*1024, NetApp_P_CallbackThread, pCbInfo);
            if (ulCallbackThread == 0)
            {
                tRetCode = NETAPP_FAILURE;
                goto error;
            }
        }
    }

err_out:
    return;

error:
    if (pCbInfo != NULL)
    {
        if (pCbInfo->pvBuffer != NULL)
        {
            NetAppOSMemFree(pCbInfo->pvBuffer);
        }
        NetAppOSMemFree(pCbInfo);
    }
    return;
}


void NetAppPrivatePrint(const char *fmt, ...)
{
    if (pNetAppHandle != NULL)
    {
        va_list ap;
        va_start(ap, fmt);
        if (pNetAppHandle->bDebugSyslog)
        {
            vsyslog(LOG_USER|LOG_INFO, fmt, ap);
        }
        if (pNetAppHandle->bDebugOn)
        {
            vfprintf(stdout, fmt, ap);
            printf("\n");
            fflush(stdout);
        }
        va_end( ap );
    }
}


void NetAppPrivateErrPrint(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if ((!pNetAppHandle && (getenv("NETAPP_DEBUG_SYSLOG") != NULL)) ||
        (pNetAppHandle && (pNetAppHandle->bDebugSyslog)))
    {
        vsyslog(LOG_USER|LOG_ERR, fmt, ap);
    }
    if (pNetAppHandle && pNetAppHandle->bDebugOn)
    {
        vfprintf(stdout, fmt, ap);
        printf("\n");
        fflush(stdout);
    }
    va_end( ap );
}


NETAPP_RETCODE NetAppGetIfaceInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_IFACE_INFO   **pIfaceInfo,
    uint32_t            *pListLength)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_IFACE_INFO   *pList = NULL;
    uint32_t            i = 0;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    if ((pList = NetAppOSMemAlloc(sizeof(NETAPP_IFACE_INFO) * NETAPP_IFACE_MAX)) == NULL)
    {
        NETAPP_ERR(("%s(): Ran out of memory", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(pList, 0, (sizeof(NETAPP_IFACE_INFO) * NETAPP_IFACE_MAX));

    for (i = 0; i < NETAPP_IFACE_MAX; i++)
    {
        pList[i].tIface = i;

        switch(i)
        {
        case NETAPP_IFACE_ETH0:
        case NETAPP_IFACE_ETH1:
        case NETAPP_IFACE_ETH2:
        case NETAPP_IFACE_ETH3:
        case NETAPP_IFACE_ETH4:
        case NETAPP_IFACE_ETH5:
        case NETAPP_IFACE_BR0:
        {
            FILE *pFD = NULL;
            char cLine[LINE_MAX];

            snprintf(pList[i].cName, sizeof(pList[i].cName), "%s%d",  NETAPP_IFACE_ETH, i);
            if ((pFD = fopen(NETAPP_PROC_NET_DEV, "r")) == NULL)
            {
                NETAPP_ERR(("%s(): Could not read /proc/modules!!", __FUNCTION__));
                goto err_out;
            }

            while(fgets(cLine, sizeof(cLine), pFD) != NULL)
            {
                char *pEth = NULL;
                if ((pEth = strstr(cLine, NETAPP_IFACE_ETH)) != NULL)
                {
                    if (i == atoi(pEth + NETAPP_IFACE_ETH_LEN))
                    {
                        pList[i].bPresent= true;
                        break;
                    }
                }
            }
            fclose(pFD);
            break;
        }
#ifndef NO_WIFI
        case NETAPP_IFACE_WIRELESS:
            pList[i].bPresent = NetAppWiFiPrivateIsEnabled(pNetAppHandle->hNetAppWiFi);
            snprintf(pList[i].cName, sizeof(pList[i].cName), "%s",  NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi));
            break;
#ifndef NO_WIFI_P2P
        case NETAPP_IFACE_P2P:
            pList[i].bPresent = (NetAppWiFiP2PPrivateGetLinkState(pNetAppHandle->hNetAppP2P) == NETAPP_LINK_UP);
            snprintf(pList[i].cName, sizeof(pList[i].cName), "%s",  NetAppWiFiP2PPrivateGetIfaceName(pNetAppHandle->hNetAppP2P));
            break;
#endif
#endif

#ifndef NO_BLUETOOTH
        case NETAPP_IFACE_BLUETOOTH:
            pList[i].bPresent = NetAppBluetoothPrivateIsEnabled(pNetAppHandle->hNetAppBT);
            snprintf(pList[i].cName, sizeof(pList[i].cName), "BLUETOOTH");
            break;
#endif
        default:
            break;
    }

    }

    *pIfaceInfo = pList;
    *pListLength = NETAPP_IFACE_MAX;

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


/*******************************************************************************
 *                 Public NetApp WiFi API's
 ******************************************************************************/
#ifndef NO_WIFI

#define NETAPP_WIFI_API_CHECK(handle)                                               \
{                                                                                   \
    NETAPP_API_CHECK(handle);                                                       \
    if (!NetAppWiFiPrivateIsEnabled(pNetAppHandle->hNetAppWiFi))                    \
    {                                                                               \
        NETAPP_ERR(("%s(): WiFi Not enabled!", __FUNCTION__));                      \
        return NETAPP_NOT_SUPPORTED;                                                \
    }                                                                               \
}

/* Fetch the mutex and verify that WiFi dongle is still inserted */
#define NETAPP_WIFI_SEM_TAKE(tRetCode)                                              \
{                                                                                   \
    NETAPP_SEM_TAKE(tRetCode);                                                      \
    if (!NetAppWiFiPrivateIsEnabled(pNetAppHandle->hNetAppWiFi))                    \
    {                                                                               \
        NETAPP_MSG(("%s WiFi not connected!", __FUNCTION__));                       \
        tRetCode = NETAPP_FAILURE;                                                  \
        goto err_out;                                                               \
    }                                                                               \
}


NETAPP_RETCODE NetAppWiFiStartScan(
    NETAPP_HANDLE       tHandle,
    int32_t             lTickMs,
    int32_t             lScanTimeMs)
{
    NETAPP_RETCODE  tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStartScan(pNetAppHandle->hNetAppWiFi, lTickMs, lScanTimeMs, NETAPP_SCAN_NORMAL, NULL), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiStopScan(
    NETAPP_HANDLE       tHandle)
{
    NETAPP_RETCODE  tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(pNetAppHandle->hNetAppWiFi, NETAPP_SCAN_NORMAL), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


#ifndef NO_WIFI_WPS
NETAPP_RETCODE NetAppWiFiConnectByPb(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_FAILURE;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi, NETAPP_CONNECT_WPS_PB, NULL, 0), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiConnectByPin(
    NETAPP_HANDLE                   tHandle,
    char                           *pSsid,
    uint32_t                        ulPin,
    bool                            bEnrollee)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_SUCCESS;

    NETAPP_MSG(("%s() Entered, PIN=%d!", __FUNCTION__, ulPin));

    NETAPP_WIFI_API_CHECK(tHandle);

    if ( !bEnrollee && (pSsid == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if (!NetAppWiFiPrivateValidatePin(ulPin))
    {
        NETAPP_ERR(("%s(): Invalid WPS Pin!", __FUNCTION__));
        return NETAPP_INVALID_PIN;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi,
        bEnrollee? NETAPP_CONNECT_WPS_PIN_ENROLLEE : NETAPP_CONNECT_WPS_PIN_REGISTRAR, pSsid, ulPin), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiGenerateWPSPin(
    uint32_t        *pulPin)
{
    NETAPP_RETCODE   tRetCode   = NETAPP_SUCCESS;
    if (pulPin == NULL)
    {
        NETAPP_ERR(("%s Invalid Parameter!", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGeneratePin(pulPin), tRetCode);

err_out:
    return tRetCode;
}

#else
NETAPP_RETCODE NetAppWiFiConnectByPb(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiConnectByPin(
    NETAPP_HANDLE                   tHandle,
    char                           *pSsid,
    uint32_t                        ulPin,
    bool                            bEnrollee)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pSsid);
    NETAPP_UNUSED(ulPin);
    NETAPP_UNUSED(bEnrollee);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGenerateWPSPin(
    uint32_t        *pulPin)
{
    NETAPP_UNUSED(pulPin);
    return NETAPP_NOT_SUPPORTED;
}
#endif

NETAPP_RETCODE NetAppWiFiConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi, NETAPP_CONNECT_NORMAL, pApInfo, 0), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    if ( (tRetCode != NETAPP_NULL_PTR) && (tRetCode != NETAPP_SUCCESS) )
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, pApInfo, sizeof(NETAPP_WIFI_AP_INFO), 0, (tRetCode == NETAPP_INVALID_PARAMETER) ?
            NETAPP_INCORRECT_PASSWORD : tRetCode, NETAPP_IFACE_WIRELESS);
    }

    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiGetConnectedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;
    NETAPP_WIFI_API_CHECK(tHandle);

    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }
    NETAPP_WIFI_SEM_TAKE(tRetCode);
    if(NetAppWiFiPrivateGetLinkState(pNetAppHandle->hNetAppWiFi) == NETAPP_LINK_DOWN)
    {
        NETAPP_MSG(("%s(): Link down!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
    }
    else
    {
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetConnectedApInfo(pNetAppHandle->hNetAppWiFi, pApInfo), tRetCode);
    }
err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiGetScanResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pulScanCount)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;
    NETAPP_WIFI_API_CHECK(tHandle);

    if ( (pApInfoList == NULL) || (pulScanCount == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetScanResults(pNetAppHandle->hNetAppWiFi, pApInfoList, pulScanCount), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiGetScannedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;
    NETAPP_WIFI_API_CHECK(tHandle);

    if (pApInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetScannedApInfo(pNetAppHandle->hNetAppWiFi, pApInfo), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiDisconnect(
    NETAPP_HANDLE        tHandle)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateDisconnect(pNetAppHandle->hNetAppWiFi), tRetCode);
err_out:
    NETAPP_SEM_GIVE();
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiIsEnabled(
    NETAPP_HANDLE       tHandle,
    bool                *pIsEnabled)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);

    if (pIsEnabled == NULL)
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    *pIsEnabled = NetAppWiFiPrivateIsEnabled(pNetAppHandle->hNetAppWiFi);
err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiIsConnected(
    NETAPP_HANDLE       tHandle,
    bool                *pIsConnected)
{
    NETAPP_RETCODE      tRetCode      = NETAPP_SUCCESS;
    uint32_t            ulIsUp = 0;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pIsConnected == NULL)
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    *pIsConnected = (NetAppWiFiPrivateGetLinkState(pNetAppHandle->hNetAppWiFi) != NETAPP_LINK_DOWN);

err_out:
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}



NETAPP_RETCODE NetAppWiFiGetApInfo(
    NETAPP_HANDLE           tHandle,
    char                    *pSSID)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pSSID == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

#ifndef NO_WIFI_INVITE
    NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateStop(pNetAppHandle->hNetAppWiFiInvite), tRetCode);
#endif
    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(pNetAppHandle->hNetAppWiFi, NETAPP_SCAN_INVITE), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateFetchApInfo(pNetAppHandle->hNetAppWiFi, pSSID), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiJoinBridge(
    NETAPP_HANDLE           tHandle,
    const char              *pBridgeName)
{

    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pBridgeName == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);


    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateJoinBridge(pNetAppHandle->hNetAppWiFi, pBridgeName), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiLeaveBridge(
    NETAPP_HANDLE           tHandle)
{

    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateLeaveBridge(pNetAppHandle->hNetAppWiFi), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiSetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t ulAntennas)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (ulAntennas == NETAPP_WIFI_ANTENNA_INVALID)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if ((tMode < NETAPP_WIFI_ANTENNA_MODE_RX) || (tMode > NETAPP_WIFI_ANTENNA_MODE_TX))
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetAntenna(pNetAppHandle->hNetAppWiFi, tMode, ulAntennas), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiGetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t *pulAntennas)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pulAntennas == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if ((tMode < NETAPP_WIFI_ANTENNA_MODE_RX) || (tMode > NETAPP_WIFI_ANTENNA_MODE_TX))
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetAntenna(pNetAppHandle->hNetAppWiFi, tMode, pulAntennas), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiGetAntennaCount(
    NETAPP_HANDLE tHandle,
    uint32_t *pulAntennas)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pulAntennas == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateGetAntennaCount(pNetAppHandle->hNetAppWiFi, pulAntennas), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}

/*******************************************************************************
 *                              Without WI-FI Support
 ******************************************************************************/
#else
NETAPP_RETCODE NetAppWiFiStartScan(
    NETAPP_HANDLE       tHandle,
    int32_t             lTickMs,
    int32_t             lScanTimeMs)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(lTickMs);
    NETAPP_UNUSED(lScanTimeMs);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiStopScan(
    NETAPP_HANDLE       tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiConnectByPb(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiConnectByPin(
    NETAPP_HANDLE                   tHandle,
    char                           *pSsid,
    uint32_t                        ulPin,
    bool                            bEnrollee)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pSsid);
    NETAPP_UNUSED(ulPin);
    NETAPP_UNUSED(bEnrollee);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGenerateWPSPin(
    uint32_t        *pulPin)
{
    NETAPP_UNUSED(pulPin);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pApInfo);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetConnectedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pApInfo);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetScanResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pulScanCount)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pApInfoList);
    NETAPP_UNUSED(pulScanCount);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetScannedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pApInfo);
    return NETAPP_NOT_SUPPORTED;
}



NETAPP_RETCODE NetAppWiFiStopConnect(
    NETAPP_HANDLE       tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiDisconnect(
    NETAPP_HANDLE        tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiIsEnabled(
    NETAPP_HANDLE       tHandle,
    bool                *pIsEnabled)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pIsEnabled);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiIsConnected(
    NETAPP_HANDLE       tHandle,
    bool                *pIsConnected)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pIsConnected);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetApInfo(
    NETAPP_HANDLE           tHandle,
    char                    *pSSID)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pSSID);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiJoinBridge(
    NETAPP_HANDLE           tHandle,
    const char              *pBridgeName)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBridgeName);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiLeaveBridge(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiSetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t ulAntennas)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(tMode);
    NETAPP_UNUSED(ulAntennas);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t *pulAntennas)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(tMode);
    NETAPP_UNUSED(pulAntennas);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiGetAntennaCount(
    NETAPP_HANDLE tHandle,
    uint32_t *pulAntennas)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pulAntennas);
    return NETAPP_NOT_SUPPORTED;
}

#endif

#ifndef NO_WIFI_TEST

NETAPP_RETCODE NetAppWiFiTestStartTransmission(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pTxSettings)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pTxSettings == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    /* these are very basic sanity checks and should not be considered exhaustive */
    if (((pTxSettings->inter_packet_gap_us < 20) || (pTxSettings->inter_packet_gap_us > 1000)) ||
        (pTxSettings->packet_size == 0) ||
        ((pTxSettings->transmit_power < 0) || (pTxSettings->transmit_power > 20)) ||
        ((pTxSettings->channel < 0) || (pTxSettings->channel > 224)) ||
        ((pTxSettings->bandwidth <= NETAPP_WIFI_BANDWIDTH_INVALID) || (pTxSettings->bandwidth > NETAPP_WIFI_BANDWIDTH_40MHz)) ||
        ((pTxSettings->sideband < NETAPP_WIFI_SIDEBAND_INVALID) || (pTxSettings->sideband > NETAPP_WIFI_SIDEBAND_UPPER)) ||
        ((pTxSettings->bandwidth != NETAPP_WIFI_BANDWIDTH_20MHz) && (pTxSettings->sideband == NETAPP_WIFI_SIDEBAND_INVALID)))
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateStartTransmission(pNetAppHandle->hNetAppWiFi, pTxSettings), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestStopTransmission(
    NETAPP_HANDLE tHandle)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateStopTransmission(pNetAppHandle->hNetAppWiFi), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestStartReceiving(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pRxSettings)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pRxSettings == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    /* these are very basic sanity checks and should not be considered exhaustive */
    if (((pRxSettings->channel < 0) || (pRxSettings->channel > 224)) ||
        ((pRxSettings->bandwidth <= NETAPP_WIFI_BANDWIDTH_INVALID) || (pRxSettings->bandwidth > NETAPP_WIFI_BANDWIDTH_40MHz)) ||
        ((pRxSettings->sideband < NETAPP_WIFI_SIDEBAND_INVALID) || (pRxSettings->sideband > NETAPP_WIFI_SIDEBAND_UPPER)) ||
        ((pRxSettings->bandwidth != NETAPP_WIFI_BANDWIDTH_20MHz) && (pRxSettings->sideband == NETAPP_WIFI_SIDEBAND_INVALID)))
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateStartReceiving(pNetAppHandle->hNetAppWiFi, pRxSettings), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestStopReceiving(
    NETAPP_HANDLE tHandle)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateStopReceiving(pNetAppHandle->hNetAppWiFi), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestGetRssiPerAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_RSSI_PER_ANTENNA *pRssiPerAntenna)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateGetRssiPerAntenna(pNetAppHandle->hNetAppWiFi, pRssiPerAntenna), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiTestControlWaveform(
    NETAPP_HANDLE tHandle,
    uint32_t ulChannel,
    bool bEnable)
{
    NETAPP_RETCODE tRetCode   = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if ((true == bEnable) && ((ulChannel < 1) || (ulChannel > 14)))
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiTestPrivateControlWaveform(pNetAppHandle->hNetAppWiFi, ulChannel, bEnable), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    return tRetCode;
}

#else

NETAPP_RETCODE NetAppWiFiTestStartTransmission(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pTxSettings)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pTxSettings);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiTestStopTransmission(
    NETAPP_HANDLE tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiTestStartReceiving(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_TXRX_SETTINGS *pRxSettings)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pRxSettings);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiTestStopReceiving(
    NETAPP_HANDLE tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppWiFiTestGetRssiPerAntenna(
    NETAPP_HANDLE tHandle,
    NETAPP_WIFI_RSSI_PER_ANTENNA *pRssiPerAntenna)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pRssiPerAntenna);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppWiFiTestControlWaveform(
    NETAPP_HANDLE tHandle,
    uint32_t ulChannel,
    bool bEnable)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(ulChannel);
    NETAPP_UNUSED(bEnable);
    return NETAPP_NOT_SUPPORTED;
}

#endif

#ifndef NO_WIFI_INVITE
NETAPP_RETCODE NetAppWiFiInviteStart(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    if ( (NetAppWiFiPrivateGetLinkState(pNetAppHandle->hNetAppWiFi) == NETAPP_LINK_DOWN) &&
#ifndef NO_WIFI_P2P
        (NetAppWiFiP2PPrivateGetLinkState(pNetAppHandle->hNetAppP2P) == NETAPP_LINK_DOWN) )
#else
        true)
#endif
    {
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateSetInterfaceUp(pNetAppHandle->hNetAppWiFi, true), tRetCode);
        NETAPP_PRIV_API_CHECK(NetAppPrivateSetInterfaceUp(pNetAppHandle->hNetAppPriv, NetAppWiFiPrivateGetIfaceName(pNetAppHandle->hNetAppWiFi), true), tRetCode);

        NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateStart(pNetAppHandle->hNetAppWiFiInvite), tRetCode);
        NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStartScan(pNetAppHandle->hNetAppWiFi, NETAPP_WIFI_INVITE_INTERVAL, NETAPP_DEFAULT_SCANTIME_MS, NETAPP_SCAN_INVITE, NULL), tRetCode);
    }
    else
    {
        NETAPP_ERR(("%s() Should not start Invite scan when we are connected!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
    }

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInviteStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiInvitePrivateStop(pNetAppHandle->hNetAppWiFiInvite), tRetCode);
    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(pNetAppHandle->hNetAppWiFi, NETAPP_SCAN_INVITE), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInviteAccept(
    NETAPP_HANDLE           tHandle,
    char                    *pSSID)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    if ( (pSSID == NULL) )
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi, NETAPP_CONNECT_INVITE_ACCEPT, pSSID, 0), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiInviteReject(
    NETAPP_HANDLE           tHandle,
    char                    *pSSID)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);

    if (pSSID == NULL)
    {
        NETAPP_ERR(("%s() invalid parameter", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi,
        NETAPP_CONNECT_INVITE_REJECT, pSSID, 0), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

#else
NETAPP_RETCODE NetAppWiFiInviteStart(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiInviteStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiInviteAccept(
    NETAPP_HANDLE           tHandle,
    char                    *pBSSID)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBSSID);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppWiFiInviteReject(
    NETAPP_HANDLE           tHandle,
    char                    *pBSSID)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBSSID);
    return NETAPP_NOT_SUPPORTED;
}
#endif

/*******************************************************************************
 *                              With WI-FI Direct Support
 ******************************************************************************/
#ifndef NO_WIFI_P2P
NETAPP_RETCODE NetAppWiFiP2PDiscover(
    NETAPP_HANDLE               tHandle)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStartScan(pNetAppHandle->hNetAppWiFi, -1, -1, NETAPP_SCAN_P2P, NULL), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PStopDiscovery(
    NETAPP_HANDLE               tHandle)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateStopScan(pNetAppHandle->hNetAppWiFi, NETAPP_SCAN_P2P), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;

}


NETAPP_RETCODE NetAppWiFiP2PConnect(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    uint32_t                ulTimeoutSec)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_PRIV_HANDLE_CHECK(pName);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiPrivateConnect(pNetAppHandle->hNetAppWiFi, NETAPP_CONNECT_P2P, pName, ulTimeoutSec), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PDisconnect(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppWiFiP2PPrivateDisconnect(pNetAppHandle->hNetAppP2P), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

NETAPP_RETCODE NetAppWiFiP2PGetSSID(
    NETAPP_HANDLE           tHandle,
    char                    *pBuf,
    uint32_t                ulBufSize)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_PRIV_HANDLE_CHECK(pBuf);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    tRetCode = NetAppWiFiP2PPrivateGetSSID(pNetAppHandle->hNetAppP2P, pBuf, ulBufSize);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppWiFiP2PWpsPushButton(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE  tRetCode      = NETAPP_SUCCESS;

    NETAPP_WIFI_API_CHECK(tHandle);
    NETAPP_WIFI_SEM_TAKE(tRetCode);

    tRetCode = NetAppWiFiP2PPrivateWpsPushButton(pNetAppHandle->hNetAppP2P);

err_out:
    NETAPP_SEM_GIVE();
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}

#else
/*******************************************************************************
 *                              Without WI-FI Direct Support
 ******************************************************************************/
NETAPP_RETCODE NetAppWiFiP2PDiscover(
    NETAPP_HANDLE               tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiP2PStopDiscovery(
    NETAPP_HANDLE               tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiP2PConnect(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    uint32_t                ulTimeoutSec)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pName);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiP2PDisconnect(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiP2PGetSSID(
    NETAPP_HANDLE           tHandle,
    char                    *pBuf,
    uint32_t                ulBufSize)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBuf);
    NETAPP_UNUSED(ulBufSize);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppWiFiP2PWpsPushButton(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}

#endif


/*******************************************************************************
 *                 With Avahi Support
 ******************************************************************************/
#ifndef NO_AVAHI
NETAPP_RETCODE NetAppZeroConfPublish(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    char                    *pType,
    uint32_t                ulPort,
    char                    *pTxtRecord,
    uint32_t                ulTxtLength)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);

    NETAPP_PRIV_HANDLE_CHECK(pName);
    NETAPP_PRIV_HANDLE_CHECK(pType);

    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppZeroConfPrivatePublish(pNetAppHandle->hNetAppZeroConf,
        pName, pType, ulPort, pTxtRecord, ulTxtLength), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppZeroConfBrowse(
    NETAPP_HANDLE           tHandle,
    char                    *pType)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_PRIV_HANDLE_CHECK(pType);
    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppZeroConfPrivateBrowse(pNetAppHandle->hNetAppZeroConf, pType), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

#else
/*******************************************************************************
 *                 Without Avahi Support
 ******************************************************************************/
NETAPP_RETCODE NetAppZeroConfPublish(
    NETAPP_HANDLE           tHandle,
    char                    *pName,
    char                    *pType,
    uint32_t                ulPort,
    char                    *pTxtRecord,
    uint32_t                ulTxtLength)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pName);
    NETAPP_UNUSED(pType);
    NETAPP_UNUSED(ulPort);
    NETAPP_UNUSED(pTxtRecord);
    NETAPP_UNUSED(ulTxtLength);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppZeroConfBrowse(
    NETAPP_HANDLE           tHandle,
    char                    *pType)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pType);
    return NETAPP_NOT_SUPPORTED;
}
#endif


/*******************************************************************************
 *                 With Bluetooth Support
 ******************************************************************************/
#ifndef NO_BLUETOOTH
#define NETAPP_BT_API_CHECK(handle)                                                 \
{                                                                                   \
    NETAPP_API_CHECK(handle);                                                       \
    if (!NetAppBluetoothPrivateIsEnabled(pNetAppHandle->hNetAppBT))                 \
    {                                                                               \
        NETAPP_ERR(("%s(): Bluetooth Not enabled!", __FUNCTION__));                 \
        return NETAPP_NOT_SUPPORTED;                                                \
    }                                                                               \
}

/* Fetch the mutex and verify that WiFi dongle is still inserted */
#define NETAPP_BT_SEM_TAKE(tRetCode)                                                \
{                                                                                   \
    NETAPP_SEM_TAKE(tRetCode);                                                      \
    if (!NetAppBluetoothPrivateIsEnabled(pNetAppHandle->hNetAppBT))                 \
    {                                                                               \
        NETAPP_MSG(("%s Bluetooth not connected!", __FUNCTION__));                  \
        tRetCode = NETAPP_FAILURE;                                                  \
        goto err_out;                                                               \
    }                                                                               \
}

NETAPP_RETCODE NetAppBluetoothDiscovery(
    NETAPP_HANDLE           tHandle,
    uint32_t                tServices)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);

    NETAPP_BT_SEM_TAKE(tRetCode);
    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateDiscover(pNetAppHandle->hNetAppBT, tServices), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothGetDiscoveryResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      **pDevInfo,
    uint32_t                *pulCount)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateGetDiscoveryResults(pNetAppHandle->hNetAppBT, pDevInfo, pulCount), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppBluetoothSimplePairingAck(
    NETAPP_HANDLE           tHandle,
    bool                    bAccept,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateSimplePairingAck(pNetAppHandle->hNetAppBT, bAccept, pDevInfo), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);

}


NETAPP_RETCODE NetAppBluetoothConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateConnect(pNetAppHandle->hNetAppBT, pBtDevInfo), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothGetDevList(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  **pBtDevInfoList,
    uint32_t            *ulCount)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateGetDevList(pNetAppHandle->hNetAppBT, pBtDevInfoList, ulCount), tRetCode);
err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothDisconnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateDisconnect(pNetAppHandle->hNetAppBT, pBtDevInfo), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothSendAudioBuffer(
    NETAPP_HANDLE           tHandle,
    void                    *pBuf,
    uint32_t                ulLength)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    //NETAPP_BT_API_CHECK(tHandle);
    //NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateSendAudioBuffer(pNetAppHandle->hNetAppBT, pBuf, ulLength), tRetCode);

err_out:
    //NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothAvStart(
    NETAPP_HANDLE           tHandle,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateAvStart(pNetAppHandle->hNetAppBT, bSynchronous, pBtAudioFormat), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothAvStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateAvStop(pNetAppHandle->hNetAppBT), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppBluetoothAvkStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    //NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateAvkStop(pNetAppHandle->hNetAppBT), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothAvkStart(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_BT_API_CHECK(tHandle);
    NETAPP_BT_SEM_TAKE(tRetCode);

    //NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateAvStart(pNetAppHandle->hNetAppBT, pBtAudioFormat), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

#else
/*******************************************************************************
 *                 Without Bluetooth Support
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothDiscovery(
    NETAPP_HANDLE           tHandle,
    uint32_t                tServices)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(tServices);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothGetDiscoveryResults(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      **pBtDevInfo,
    uint32_t                *pulCount)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBtDevInfo);
    NETAPP_UNUSED(pulCount);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothSimplePairingAck(
    NETAPP_HANDLE           tHandle,
    bool                    bAccept,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pDevInfo);
    NETAPP_UNUSED(bAccept);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothConnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBtDevInfo);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppBluetoothDisconnect(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBtDevInfo);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothSendAudioBuffer(
    NETAPP_HANDLE           tHandle,
    void                    *pBuf,
    uint32_t                ulLength)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBuf);
    NETAPP_UNUSED(ulLength);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothAvStart(
    NETAPP_HANDLE           tHandle,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(bSynchronous);
    NETAPP_UNUSED(pBtAudioFormat);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothAvStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothAvkStop(
    NETAPP_HANDLE           tHandle)
{
    NETAPP_UNUSED(tHandle);
    return NETAPP_NOT_SUPPORTED;
}

NETAPP_RETCODE NetAppBluetoothAvkStart(
    NETAPP_HANDLE           tHandle,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBtAudioFormat);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppBluetoothVoiceToStringSearch(
    NETAPP_HANDLE            tHandle,
    NETAPP_BT_HID_VOICE_INFO *pHidVoiceInfo,
    char                     **pString)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pHidVoiceInfo);
    NETAPP_UNUSED(pString);
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppBluetoothGetDevList(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  **pBtDevInfoList,
    uint32_t            *ulCount)
{
    NETAPP_UNUSED(tHandle);
    NETAPP_UNUSED(pBtDevInfoList);
    NETAPP_UNUSED(ulCount);
    return NETAPP_NOT_SUPPORTED;
}
#endif


NETAPP_RETCODE NetAppWiFiDeleteSavedApInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO *pApInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NetAppSettingsPrivateDeleteWiFiApInfo(pNetAppHandle->hNetAppSettings, pApInfo);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppWiFiGetSavedApInfoList(
    NETAPP_HANDLE       tHandle,
    NETAPP_WIFI_AP_INFO **pApInfoList,
    uint32_t            *pulCount)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppSettingsPrivateGetSavedWiFiApInfoList(
        pNetAppHandle->hNetAppSettings, pApInfoList, pulCount), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}

NETAPP_RETCODE NetAppBluetoothDeleteSavedDevInfo(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  *pDevInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NetAppSettingsPrivateDeleteBtDev(pNetAppHandle->hNetAppSettings, pDevInfo);
err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


NETAPP_RETCODE NetAppBluetoothGetSavedBtDevList(
    NETAPP_HANDLE       tHandle,
    NETAPP_BT_DEV_INFO  **pDevInfoList,
    uint32_t            *pulCount)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_API_CHECK(tHandle);
    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppSettingsPrivateGetSavedBtDevList(
        pNetAppHandle->hNetAppSettings, pDevInfoList, pulCount), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}


#ifndef NO_VOICE_RECOGNITION
NETAPP_RETCODE NetAppHttpVoiceSearch(
    NETAPP_BT_HID_VOICE_INFO *pHidVoiceInfo,
    const char               *pLanguage)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_SEM_TAKE(tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppHttpPrivateVoiceSearch(pHidVoiceInfo, pLanguage), tRetCode);

err_out:
    NETAPP_SEM_GIVE();
    return (tRetCode);
}
#else
NETAPP_RETCODE NetAppHttpVoiceSearch(
    NETAPP_BT_HID_VOICE_INFO *pHidVoiceInfo,
    const char               *pLanguage)
{
    NETAPP_UNUSED(pHidVoiceInfo);
    NETAPP_UNUSED(pLanguage);
    return NETAPP_NOT_SUPPORTED;
}
#endif
/*******************************************************************************
 *                       Static NetApp Functions
 ******************************************************************************/
static void NetApp_P_CallbackThread(
    void              *pParam)
{
    NETAPP_CALLBACK_INFO *pCbInfo = (NETAPP_CALLBACK_INFO *)pParam;
    if (pCbInfo == NULL)
    {
        NETAPP_ERR(("%s(): Passed garbage in the callback thread", __FUNCTION__));
        goto err_out;
    }

    NETAPP_MSG(("%s():Callback [%s] Calling callback", __FUNCTION__, NETAPP_CALLBACK_STR[pCbInfo->tCbType]));

    (pCbInfo->tCbHandle)(pCbInfo->pParam, pCbInfo->tCbType, pCbInfo->pvBuffer, pCbInfo->ulData0, pCbInfo->tResult, pCbInfo->tIface);

    NETAPP_MSG(("%s():Callback [%s] Completed, Result was %s", __FUNCTION__,
            NETAPP_CALLBACK_STR[pCbInfo->tCbType], (pCbInfo->tResult == NETAPP_SUCCESS) ? "SUCCESS" : "FAILURE"));

    if (pCbInfo->pvBuffer != NULL)
    {
        NetAppOSMemFree(pCbInfo->pvBuffer);
    }
    NetAppOSMemFree(pCbInfo);
err_out:
    NETAPP_MSG(("%s():Leaving callback thread", __FUNCTION__));
    NetAppOSTaskExit();
}


static void NetApp_P_TestTimer(
    void                    *pParam)
{
    NETAPP_INPUT_INFO input = {105, true};
    NETAPP_DO_CALLBACK(NETAPP_CB_INPUT_EVENT, &input, sizeof(input), 0, NETAPP_SUCCESS, NETAPP_IFACE_MAX);
    NetAppOSTaskDelayMsec(100);
    input.bPressed = false;
    NETAPP_DO_CALLBACK(NETAPP_CB_INPUT_EVENT, &input, sizeof(input), 0, NETAPP_SUCCESS, NETAPP_IFACE_MAX);

}
