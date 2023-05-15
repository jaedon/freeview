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
 * $brcm_Workfile: netapp_settings.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_settings.c $
 *
 * 1   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 ***************************************************************************/
/**
 * @brief NetApp Settings API
 *
 * API is used to store and retrieve settings using the any backend.
 * Built-in settings storage and retrieval is optional feature of NetApp.
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by netapp
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_settings.h"

#define NETAPP_WIFI_AP_INFO_SSID            "SSID"
#define NETAPP_WIFI_AP_INFO_BSSID           "BSSID"
#define NETAPP_WIFI_AP_INFO_PASSWORD        "Password"
#define NETAPP_WIFI_AP_INFO_SECURITY        "Security"

#define NETAPP_WIFI_AP_INFO_SECURITY_INVALID           "Invalid"
#define NETAPP_WIFI_AP_INFO_SECURITY_OPEN              "Open"
#define NETAPP_WIFI_AP_INFO_SECURITY_WEP               "Wep"
#define NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_AES       "WpaPskAes"
#define NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_TKIP      "WpaPskTkip"
#define NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_AES      "Wpa2PskAes"
#define NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_TKIP     "Wpa2PskTkip"
#define NETAPP_WIFI_AP_INFO_SECURITY_NOT_SUPPORTED     "NotSupported"

#define NETAPP_DB_BT_INFO_ADDR              "Address"
#define NETAPP_DB_BT_INFO_NAME              "Name"
#define NETAPP_DB_BT_INFO_HID_INFO          "HID Descriptor"
#define NETAPP_DB_BT_INFO_SERVICES          "Services"
#define NETAPP_DB_BT_INFO_LINK_KEY          "Link Key"
#define NETAPP_DB_BT_INFO_MAJOR_COD         "Major COD"
#define NETAPP_DB_BT_INFO_MINOR_COD         "Minor COD"
#define NETAPP_DB_BT_INFO_SERVICE_COD       "Service COD"
#define NETAPP_DB_BT_INFO_HAS_LINK_KEY      "Has Link Key"
#define NETAPP_DB_BT_INFO_PID               "PID"
#define NETAPP_DB_BT_INFO_VID               "VID"
#define NETAPP_DB_BT_INFO_SERVICE_MASK      "Service Mask"
#define NETAPP_DB_BT_INFO_KEY_TYPE          "Key Type"
#define NETAPP_DB_BT_INFO_DEV_FEATURES      "Device Features"

#define NETAPP_DB_A_CURRENT_APINFO          "NETAPP_DB_A_CURRENT_APINFO"
#define NETAPP_DB_A_CURRENT_INTERFACE       "NETAPP_DB_A_CURRENT_INTERFACE"
#define NETAPP_DB_A_WPS_SETTINGS            "NETAPP_DB_A_WPS_SETTINGS"
#define NETAPP_DB_A_SAVED_AP_LIST           "NETAPP_DB_A_SAVED_AP_LIST"
#define NETAPP_DB_A_SAVED_BT_LIST           "NETAPP_DB_A_SAVED_BT_LIST"

#define NETAPP_DB_BT_SERVICE_HID            "HID"
#define NETAPP_DB_BT_SERVICE_HSP            "HSP"
#define NETAPP_DB_BT_SERVICE_HFP            "HFP"
#define NETAPP_DB_BT_SERVICE_OPP            "OPP"
#define NETAPP_DB_BT_SERVICE_FTP            "FTP"
#define NETAPP_DB_BT_SERVICE_A2DP           "A2DP"
#define NETAPP_DB_BT_SERVICE_AVRCP          "AVRCP"

typedef struct
{
    NETAPP_IFACE    tIface;
    const char      *pString;
} IFACE_STRING_TABLE;
static IFACE_STRING_TABLE IfaceStringTable[] =
{
    { NETAPP_IFACE_ETH0,        "NETAPP_IFACE_ETH0"},
    { NETAPP_IFACE_ETH1,        "NETAPP_IFACE_ETH1"},
    { NETAPP_IFACE_ETH2,        "NETAPP_IFACE_ETH2"},
    { NETAPP_IFACE_ETH3,        "NETAPP_IFACE_ETH3"},
    { NETAPP_IFACE_ETH4,        "NETAPP_IFACE_ETH4"},
    { NETAPP_IFACE_ETH5,        "NETAPP_IFACE_ETH5"},
    { NETAPP_IFACE_BR0,			"NETAPP_IFACE_BR0"},
    { NETAPP_IFACE_WIRELESS,    "NETAPP_IFACE_WIRELESS"},
    { NETAPP_IFACE_LOOPBACK,    "NETAPP_IFACE_LOOPBACK"},
    { NETAPP_IFACE_P2P,         "NETAPP_IFACE_P2P"},
    { NETAPP_IFACE_BLUETOOTH,   "NETAPP_IFACE_BLUETOOTH"},
    { NETAPP_IFACE_MAX,         "NETAPP_IFACE_BLUETOOTH"}
};

#ifndef NO_DATABASE

#define NETAPP_JSON_CHECK(api, object, tRetCode)                                \
{                                                                               \
    object = api;                                                               \
    if (object == NULL)                                                         \
    {                                                                           \
        NETAPP_ERR(("%s(): %s returned NULL!", __FUNCTION__, #api));            \
        tRetCode = NETAPP_FAILURE;                                              \
        goto err_out;                                                           \
    }                                                                           \
}

#define NETAPP_DB_GET_INT(jsonObj, tag, output)                                 \
{                                                                               \
    json_object *pKey = NULL;                                                   \
    if ((pKey = json_object_object_get(jsonObj, tag)) != NULL)                  \
    {                                                                           \
        if (json_object_get_type(pKey) == json_type_int)                        \
        {                                                                       \
            output = json_object_get_int(pKey);                                 \
        }                                                                       \
    }                                                                           \
}

#define NETAPP_DB_GET_BOOL(jsonObj, tag, output)                                \
{                                                                               \
    json_object *pKey = NULL;                                                   \
    if ((pKey = json_object_object_get(jsonObj, tag)) != NULL)                  \
    {                                                                           \
        if (json_object_get_type(pKey) == json_type_boolean)                    \
        {                                                                       \
            output = json_object_get_boolean(pKey);                             \
        }                                                                       \
    }                                                                           \
}

#define NETAPP_DB_GET_STRING(jsonObj, tag, output)                              \
{                                                                               \
    json_object *pKey = NULL;                                                   \
    if ((pKey = json_object_object_get(jsonObj, tag)) != NULL)                  \
    {                                                                           \
        if (json_object_get_type(pKey) == json_type_string)                     \
        {                                                                       \
            strncpy(output, json_object_get_string(pKey), sizeof(output)/sizeof(output[0])-1);\
            output[sizeof(output)/sizeof(output[0])-1] = '\0';                   \
        }                                                                       \
    }                                                                           \
}

#define NETAPP_DB_GET_ARRAY(jsonObj, tag, output)                               \
{                                                                               \
    json_object *pKey = NULL;                                                   \
    if ((pKey = json_object_object_get(pObject, tag)) != NULL)                  \
    {                                                                           \
        int i = 0;                                                              \
        if (json_object_get_type(pKey) == json_type_array)                      \
        {                                                                       \
            for (i = 0; i < sizeof(output); i++)                                \
            {                                                                   \
                json_object *pData = json_object_array_get_idx(pKey, i);        \
                if (pData != NULL)                                              \
                {                                                               \
                    output[i] = (uint8_t)json_object_get_int(pData);            \
                    json_object_put(pData);                                     \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }                                                                           \
}


#include "netapp_db.h"
#define __STRICT_ANSI__
#include "json.h"

struct sNETAPP_SETTINGS
{
    NETAPP_DB_HANDLE    hNetAppDB;
};


static json_object* NetAppSettingsPrivate_P_WiFiApInfoToJSONObj(
    NETAPP_WIFI_AP_INFO     *pApInfo);


static NETAPP_RETCODE NetAppSettingsPrivate_P_JSONToBtDevInfo(
    const char              *pString,
    NETAPP_BT_DEV_INFO      *pDevInfo);


static json_object* NetAppSettingsPrivate_P_BtDevInfoToJSONObj(
    NETAPP_BT_DEV_INFO     *pDevInfo);


static NETAPP_RETCODE NetAppSettingsPrivate_P_JSONToWiFiApInfo(
    const char              *pString,
    NETAPP_WIFI_AP_INFO     *pApInfo);


NETAPP_RETCODE NetAppSettingsPrivateInit(
    NETAPP_SETTINGS_HANDLE  *phNetAppSettings,
    const char              *pDbPath)
{
    NETAPP_SETTINGS_HANDLE  hNetAppSettings = NULL;
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    unsigned int            i;
    struct stat             statbuf;

    NETAPP_PRIV_HANDLE_CHECK(phNetAppSettings);

    hNetAppSettings = (NETAPP_SETTINGS_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_SETTINGS));
    if (hNetAppSettings == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppSettings, 0, sizeof(struct sNETAPP_SETTINGS));

    NETAPP_PRIV_API_CHECK(NetAppDBPrivateInit(&hNetAppSettings->hNetAppDB, pDbPath), tRetCode);
    *phNetAppSettings = hNetAppSettings;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppSettingsPrivateDeinit(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);

    NetAppDBPrivateDeinit(hNetAppSettings->hNetAppDB);

    NetAppOSMemFree(hNetAppSettings);
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppSettingsPrivateSaveWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE  tRetCode        = NETAPP_SUCCESS;
    json_object     *pJsonObjList   = NULL;
    json_object     *pJsonObjApInfo = NULL;
    char            *pSavedApList   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pApInfo);

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_AP_LIST, &pSavedApList);

    NETAPP_MSG(("%s(): Stored APInfoList=%s", __FUNCTION__, pSavedApList ? pSavedApList : "NULL"));

    /* Copy saved items minus the passed AP info to the new list*/
    if( (pSavedApList == NULL) || ((pJsonObjList=json_tokener_parse(pSavedApList)) == NULL) )
    {
        pJsonObjList = json_object_new_object();
    }
    pJsonObjApInfo = NetAppSettingsPrivate_P_WiFiApInfoToJSONObj(pApInfo);
    json_object_object_add(pJsonObjList, pApInfo->cBSSID, pJsonObjApInfo);

    NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_AP_LIST, json_object_to_json_string(pJsonObjList));
    NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_CURRENT_APINFO, json_object_to_json_string(pJsonObjApInfo));

    NETAPP_MSG(("%s(): Wrote APInfoList=%s", __FUNCTION__, json_object_to_json_string(pJsonObjList)));

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }

    if (pJsonObjApInfo != NULL)
    {
        json_object_put(pJsonObjApInfo);
    }
    if (pSavedApList != NULL)
    {
        free(pSavedApList);
    }

    return tRetCode;
}


void NetAppSettingsPrivateDeleteWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE  tRetCode        = NETAPP_SUCCESS;

    json_object     *pJsonObjList= NULL;
    char            *pSavedApList   = NULL;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));
    if ( (hNetAppSettings == NULL) || (pApInfo == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_AP_LIST, &pSavedApList);

    NETAPP_MSG(("%s(): Stored APInfoList=%s", __FUNCTION__, pSavedApList ? pSavedApList : "NULL"));

    /* Copy saved items minus the passed AP info to the new list*/
    if( (pSavedApList != NULL) && ((pJsonObjList=json_tokener_parse(pSavedApList)) != NULL) )
    {
        json_object_object_del(pJsonObjList, pApInfo->cBSSID);
        NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_AP_LIST, json_object_to_json_string(pJsonObjList));
        NETAPP_MSG(("%s(): Wrote APInfoList=%s", __FUNCTION__, json_object_to_json_string(pJsonObjList)));
    }

    NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_CURRENT_APINFO, "");

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }
    if (pSavedApList != NULL)
    {
        free(pSavedApList);
    }
}


NETAPP_RETCODE NetAppSettingsPrivateGetSavedWiFiApInfoList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pulCount)
{
    NETAPP_RETCODE      tRetCode            = NETAPP_SUCCESS;
    char                *pSavedApList       = NULL;
    json_object         *pJsonObjList       = NULL;
    int                 i                   = 0;
    NETAPP_WIFI_AP_INFO *pSavedApInfoList   = NULL;
    uint32_t            ulCount             = 0;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));
    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pApInfoList);
    NETAPP_PRIV_HANDLE_CHECK(pulCount);

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_AP_LIST, &pSavedApList);

    NETAPP_MSG(("%s(): Stored APInfoList=%s", __FUNCTION__, pSavedApList ? pSavedApList : "NULL"));

    if( (pSavedApList == NULL) || ((pJsonObjList=json_tokener_parse(pSavedApList)) == NULL) )
    {
        *pulCount = 0;
        goto err_out;
    }

    /* Find out how many items there are to allocate an array */
    {
        json_object_object_foreach(pJsonObjList, key, val)
        {
            ulCount++;
        }
    }

    pSavedApInfoList = (NETAPP_WIFI_AP_INFO *)NetAppOSMemAlloc(ulCount * sizeof(NETAPP_WIFI_AP_INFO));
    if (pSavedApInfoList == NULL)
    {
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    json_object_object_foreach(pJsonObjList, key, val)
    {
         NetAppSettingsPrivate_P_JSONToWiFiApInfo(json_object_to_json_string(val), &pSavedApInfoList[i]);
    }

    *pApInfoList = pSavedApInfoList;
    *pulCount = ulCount;

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }
    if (pSavedApList != NULL)
    {
        free(pSavedApList);
    }
    return tRetCode;
}


void NetAppSettingsPrivateReset(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    if (hNetAppSettings != NULL)
    {
        NetAppDBPrivateReset(hNetAppSettings->hNetAppDB);
    }
}


NETAPP_RETCODE NetAppSettingsPrivateGetCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE  tRetCode        = NETAPP_SUCCESS;
    char            *pSavedApInfo   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pApInfo);

    memset(pApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_CURRENT_APINFO, &pSavedApInfo);
    if (pSavedApInfo != NULL)
    {
        NetAppSettingsPrivate_P_JSONToWiFiApInfo(pSavedApInfo, pApInfo);
        NETAPP_MSG(("%s(): CurrentAP=%s", __FUNCTION__, pSavedApInfo));
    }
err_out:
    if (pSavedApInfo != NULL)
    {
        free(pSavedApInfo);
    }
    return tRetCode;
}


void NetAppSettingsPrivateClearCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    if (hNetAppSettings != NULL)
    {
        NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_CURRENT_APINFO, "");
    }
}

void NetAppSettingsPrivateSetCurrentInterface(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_IFACE            tCurIface)
{
    int i;
    if (hNetAppSettings == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }
    for (i = 0; i < sizeof(IfaceStringTable)/sizeof(IfaceStringTable[0]); i++)
    {
        if (tCurIface == IfaceStringTable[i].tIface)
        {
            NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_CURRENT_INTERFACE,
                IfaceStringTable[i].pString);
            break;
        }
    }
}


NETAPP_RETCODE NetAppSettingsPrivateSaveBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo)
{
    NETAPP_RETCODE  tRetCode            = NETAPP_SUCCESS;
    json_object     *pJsonObjList       = NULL;
    json_object     *pJsonObjBTDevInfo  = NULL;
    char            *pSavedBtList       = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pBtDevInfo);

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, &pSavedBtList);

    /* Copy saved items minus the passed AP info to the new list*/
    if( (pSavedBtList == NULL) || ((pJsonObjList=json_tokener_parse(pSavedBtList)) == NULL) )
    {
        pJsonObjList = json_object_new_object();
    }
    pJsonObjBTDevInfo = NetAppSettingsPrivate_P_BtDevInfoToJSONObj(pBtDevInfo);
    json_object_object_add(pJsonObjList, pBtDevInfo->cAddr, pJsonObjBTDevInfo);

    NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, json_object_to_json_string(pJsonObjList));

    NETAPP_MSG(("%s(): Wrote BTInfoList=%s", __FUNCTION__, json_object_to_json_string(pJsonObjList)));

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }

    if (pJsonObjBTDevInfo != NULL)
    {
        json_object_put(pJsonObjBTDevInfo);
    }
    if (pSavedBtList != NULL)
    {
        free(pSavedBtList);
    }

    return tRetCode;
}


void NetAppSettingsPrivateDeleteBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo)
{
    NETAPP_RETCODE  tRetCode        = NETAPP_SUCCESS;

    json_object     *pJsonObjList= NULL;
    char            *pSavedBtList   = NULL;

    if ( (hNetAppSettings == NULL) || (pBtDevInfo == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, &pSavedBtList);

    NETAPP_MSG(("%s(): Stored BTInfoList=%s", __FUNCTION__, pSavedBtList ? pSavedBtList : "NULL"));

    /* Copy saved items minus the passed AP info to the new list*/
    if( (pSavedBtList != NULL) && ((pJsonObjList=json_tokener_parse(pSavedBtList)) != NULL) )
    {
        json_object_object_del(pJsonObjList, pBtDevInfo->cAddr);
        NetAppDBPrivateSetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, json_object_to_json_string(pJsonObjList));
        NETAPP_MSG(("%s(): Wrote BTInfoList=%s", __FUNCTION__, json_object_to_json_string(pJsonObjList)));
    }

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }
    if (pSavedBtList != NULL)
    {
        free(pSavedBtList);
    }
}


NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDevList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     **pBtDevInfoList,
    uint32_t                *pulCount)
{
    NETAPP_RETCODE      tRetCode        = NETAPP_SUCCESS;
    char                *pSavedBtList   = NULL;
    json_object         *pJsonObjList   = NULL;
    int                 i               = 0;
    NETAPP_BT_DEV_INFO *pSavedBtDevList = NULL;
    int                 ulCount         = 0;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pBtDevInfoList);
    NETAPP_PRIV_HANDLE_CHECK(pulCount);
    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, &pSavedBtList);

    if( (pSavedBtList == NULL) || ((pJsonObjList=json_tokener_parse(pSavedBtList)) == NULL) )
    {
        *pulCount = 0;
        goto err_out;
    }

    /* Find out how many items there are to allocate an array */
    {
        json_object_object_foreach(pJsonObjList, key, val)
        {
            ulCount++;
        }
    }

    NETAPP_MSG(("%s(): Stored %d items:[%s]", __FUNCTION__, ulCount, pSavedBtList ? pSavedBtList : "NULL"));

    pSavedBtDevList = (NETAPP_BT_DEV_INFO *)NetAppOSMemAlloc(ulCount * sizeof(NETAPP_BT_DEV_INFO));
    if (pSavedBtDevList == NULL)
    {
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    json_object_object_foreach(pJsonObjList, key, val)
    {
         NetAppSettingsPrivate_P_JSONToBtDevInfo(json_object_to_json_string(val), &pSavedBtDevList[i]);
         i++;
    }

    *pBtDevInfoList = pSavedBtDevList;
    *pulCount       = ulCount;

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }
    if (pSavedBtList != NULL)
    {
        free(pSavedBtList);
    }
    return tRetCode;

}


NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_HW_ADDR          tAddr,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_RETCODE  tRetCode        = NETAPP_NOT_FOUND;
    char            *pSavedBtList   = NULL;
    json_object     *pJsonObjList   = NULL;
    char            cReqAddr[NETAPP_ENET_LEN+1];
    char            cTempAddr[NETAPP_ENET_LEN+1];
    NETAPP_BT_DEV_INFO tBtDevInfo;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppSettings);
    NETAPP_PRIV_HANDLE_CHECK(pBtDevInfo);

    NetAppHwAddrToA(tAddr, cReqAddr, sizeof(cReqAddr));

    NetAppDBPrivateGetValue(hNetAppSettings->hNetAppDB, NETAPP_DB_A_SAVED_BT_LIST, &pSavedBtList);

    if( (pSavedBtList == NULL) || ((pJsonObjList=json_tokener_parse(pSavedBtList)) == NULL) )
    {
        goto err_out;
    }

    json_object_object_foreach(pJsonObjList, key, val)
    {
        if (val != NULL)
        {
            NETAPP_DB_GET_STRING(val, NETAPP_DB_BT_INFO_ADDR, cTempAddr);
            if (strncmp(cReqAddr, cTempAddr, sizeof(cReqAddr)) == 0)
            {
                NetAppSettingsPrivate_P_JSONToBtDevInfo(json_object_to_json_string(val), pBtDevInfo);
                NETAPP_MSG(("%s(): Found:[%s]", __FUNCTION__, val));
                tRetCode = NETAPP_SUCCESS;
                break;
            }
        }
    }

err_out:
    if (pJsonObjList != NULL)
    {
        json_object_put(pJsonObjList);
    }
    if (pSavedBtList != NULL)
    {
        free(pSavedBtList);
    }
    return tRetCode;
}



/*******************************************************************************
 *                       Static NetApp Settings Functions
 ******************************************************************************/
static json_object* NetAppSettingsPrivate_P_WiFiApInfoToJSONObj(
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE  tRetCode    = NETAPP_SUCCESS;
    json_object     *pObject    = NULL;
    json_object     *pSSID      = NULL;
    json_object     *pBSSID     = NULL;
    json_object     *pPassword  = NULL;
    json_object     *pSecurity  = NULL;

    if (pApInfo == NULL)
    {
        return NULL;
    }

    NETAPP_JSON_CHECK(json_object_new_object(),                     pObject,    tRetCode);
    NETAPP_JSON_CHECK(json_object_new_string(pApInfo->cSSID),       pSSID,      tRetCode);
    NETAPP_JSON_CHECK(json_object_new_string(pApInfo->cBSSID),      pBSSID,     tRetCode);
    NETAPP_JSON_CHECK(json_object_new_string(pApInfo->cPassword),   pPassword,  tRetCode);

    json_object_object_add(pObject, NETAPP_WIFI_AP_INFO_SSID,       pSSID);
    json_object_object_add(pObject, NETAPP_WIFI_AP_INFO_BSSID,      pBSSID);
    json_object_object_add(pObject, NETAPP_WIFI_AP_INFO_PASSWORD,   pPassword);

    switch (pApInfo->tSecurity)
    {
    case NETAPP_WIFI_SECURITY_NONE:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_OPEN), pSecurity, tRetCode);
        break;

    case NETAPP_WIFI_SECURITY_WEP:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_WEP), pSecurity, tRetCode);
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_AES:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_AES), pSecurity, tRetCode);
        break;

    case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_TKIP), pSecurity, tRetCode);
        break;

    case NETAPP_WIFI_SECURITY_WPA2_PSK_AES:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_AES), pSecurity, tRetCode);
        break;

    case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_TKIP), pSecurity, tRetCode);
        break;

    default:
    case NETAPP_WIFI_SECURITY_NOT_SUPPORTED:
    case NETAPP_WIFI_SECURITY_INVALID:
        NETAPP_JSON_CHECK(json_object_new_string(NETAPP_WIFI_AP_INFO_SECURITY_INVALID), pSecurity, tRetCode);
        break;
    }

    json_object_object_add(pObject, NETAPP_WIFI_AP_INFO_SECURITY,   pSecurity);


err_out:
    return pObject;
}


static NETAPP_RETCODE NetAppSettingsPrivate_P_JSONToWiFiApInfo(
    const char              *pString,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_RETCODE      tRetCode= NETAPP_SUCCESS;
    json_object         *pObject= NULL;
    json_object         *pSecurity = NULL;

    NETAPP_PRIV_HANDLE_CHECK(pString);
    NETAPP_PRIV_HANDLE_CHECK(pApInfo);

    memset(pApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));

    NETAPP_JSON_CHECK(json_tokener_parse(pString), pObject, tRetCode);

    NETAPP_DB_GET_STRING(pObject, NETAPP_WIFI_AP_INFO_SSID,     pApInfo->cSSID);
    NETAPP_DB_GET_STRING(pObject, NETAPP_WIFI_AP_INFO_BSSID,    pApInfo->cBSSID);
    NETAPP_DB_GET_STRING(pObject, NETAPP_WIFI_AP_INFO_PASSWORD, pApInfo->cPassword);

    /* Get the security from the DB */
    if ( ((pSecurity = json_object_object_get(pObject, NETAPP_WIFI_AP_INFO_SECURITY)) != NULL) &&
         (json_object_get_type(pSecurity) == json_type_string) )
    {
        if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_INVALID) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_INVALID;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_OPEN) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_NONE;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_WEP) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WEP;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_AES) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_AES;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_WPA_PSK_TKIP) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_TKIP;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_AES) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_AES;
        }

        else if (strstr(json_object_get_string(pSecurity), NETAPP_WIFI_AP_INFO_SECURITY_WPA2_PSK_TKIP) != NULL)
        {
            pApInfo->tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP;
        }
        json_object_put(pSecurity);
    }

    json_object_put(pObject);

err_out:
    return tRetCode;
}


static NETAPP_RETCODE NetAppSettingsPrivate_P_JSONToBtDevInfo(
    const char              *pString,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE      tRetCode= NETAPP_SUCCESS;
    json_object         *pObject= NULL;
    json_object         *pKey   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(pString);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    memset(pDevInfo, 0, sizeof(NETAPP_BT_DEV_INFO));

    NETAPP_JSON_CHECK(json_tokener_parse(pString), pObject, tRetCode);

    NETAPP_DB_GET_STRING(pObject, NETAPP_DB_BT_INFO_ADDR,           pDevInfo->cAddr);
    NETAPP_DB_GET_STRING(pObject, NETAPP_DB_BT_INFO_NAME,           pDevInfo->cName);
    NETAPP_DB_GET_BOOL  (pObject, NETAPP_DB_BT_INFO_HAS_LINK_KEY,   pDevInfo->bHasLinkKey);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_MAJOR_COD,      pDevInfo->ucMajorClassDev);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_MINOR_COD,      pDevInfo->ucMinorClassDev);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_SERVICE_COD,    pDevInfo->usServiceClassDev);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_PID,            pDevInfo->usProductID);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_VID,            pDevInfo->usVendorID);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_SERVICE_MASK,   pDevInfo->ulServiceMask);
    NETAPP_DB_GET_INT   (pObject, NETAPP_DB_BT_INFO_KEY_TYPE,       pDevInfo->ucKeyType);
    NETAPP_DB_GET_ARRAY (pObject, NETAPP_DB_BT_INFO_DEV_FEATURES,   pDevInfo->ucDeviceFeatures);
    NETAPP_DB_GET_ARRAY (pObject, NETAPP_DB_BT_INFO_LINK_KEY,       pDevInfo->usLinkKey);

    /* Get HID Descriptor */
    if ((pKey = json_object_object_get(pObject, NETAPP_DB_BT_INFO_HID_INFO)) != NULL)
    {
        int i = 0;
        pDevInfo->tHidInfo.ulLength = (json_object_get_type(pKey) == json_type_array) ? json_object_array_length(pKey) : 0;

        for (i = 0; i < pDevInfo->tHidInfo.ulLength; i++)
        {
            json_object *pData = json_object_array_get_idx(pKey, i);
            if (pData != NULL)
            {
                pDevInfo->tHidInfo.usData[i] = (uint8_t)json_object_get_int(pData);
                json_object_put(pData);
            }
        }
        json_object_put(pKey);
    }

    /* Get Service List */
    if ((pKey = json_object_object_get(pObject, NETAPP_DB_BT_INFO_SERVICES)) != NULL)
    {
        if (json_object_get_type(pKey) == json_type_array)
        {
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_HID) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_HID;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_HSP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_HSP;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_HFP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_HFP;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_OPP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_OPP;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_FTP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_FTP;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_A2DP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_A2DP;
            }
            if (strstr(json_object_get_string(pKey), NETAPP_DB_BT_SERVICE_AVRCP) != NULL)
            {
                pDevInfo->ulServiceMask += NETAPP_BT_SERVICE_AVRCP;
            }
        }
        json_object_put(pKey);
    }

    json_object_put(pObject);

err_out:
    return tRetCode;
}


static json_object* NetAppSettingsPrivate_P_BtDevInfoToJSONObj(
    NETAPP_BT_DEV_INFO     *pDevInfo)
{
    NETAPP_RETCODE  tRetCode            = NETAPP_SUCCESS;
    json_object     *pObject            = NULL;
    json_object     *pAddr              = NULL;
    json_object     *pName              = NULL;
    json_object     *pHidData           = NULL;
    json_object     *pLinkKey           = NULL;
    json_object     *pMajorCOD          = NULL;
    json_object     *pMinorCOD          = NULL;
    json_object     *pServiceCOD        = NULL;
    json_object     *pHasLinkKey        = NULL;
    json_object     *pTrustedServiceMask= NULL;
    json_object     *pKeyType           = NULL;
    json_object     *pDevFeatures       = NULL;
    json_object     *pPID               = NULL;
    json_object     *pVID               = NULL;
    json_object     *pServices          = NULL;
    int             i                   = 0;

    if (pDevInfo == NULL)
    {
        return NULL;
    }

    NETAPP_JSON_CHECK(json_object_new_object(),                             pObject,            tRetCode);
    NETAPP_JSON_CHECK(json_object_new_string(pDevInfo->cAddr),              pAddr,              tRetCode);
    NETAPP_JSON_CHECK(json_object_new_string(pDevInfo->cName),              pName,              tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->ucMajorClassDev),       pMajorCOD,          tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->ucMinorClassDev),       pMinorCOD,          tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->usServiceClassDev),     pServiceCOD,        tRetCode);
    NETAPP_JSON_CHECK(json_object_new_boolean(pDevInfo->bHasLinkKey),       pHasLinkKey,        tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->usProductID),           pPID,               tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->usVendorID),            pVID,               tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->ulTrustedServiceMask),  pTrustedServiceMask,tRetCode);
    NETAPP_JSON_CHECK(json_object_new_int(pDevInfo->ucKeyType),             pKeyType,           tRetCode);
    NETAPP_JSON_CHECK(json_object_new_array(),                              pDevFeatures,       tRetCode);
    NETAPP_JSON_CHECK(json_object_new_array(),                              pHidData,           tRetCode);
    NETAPP_JSON_CHECK(json_object_new_array(),                              pLinkKey,           tRetCode);
    NETAPP_JSON_CHECK(json_object_new_array(),                              pServices,          tRetCode);

    for (i = 0; i < pDevInfo->tHidInfo.ulLength; i++)
    {
        json_object_array_add(pHidData, json_object_new_int(pDevInfo->tHidInfo.usData[i]));
    }

    for (i = 0; i < (sizeof(pDevInfo->usLinkKey)/sizeof(pDevInfo->usLinkKey[0])); i++)
    {
        json_object_array_add(pLinkKey, json_object_new_int(pDevInfo->usLinkKey[i]));
    }

    for (i = 0; i < (sizeof(pDevInfo->ucDeviceFeatures)/sizeof(pDevInfo->ucDeviceFeatures[0])); i++)
    {
        json_object_array_add(pDevFeatures, json_object_new_int(pDevInfo->ucDeviceFeatures[i]));
    }

    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HID)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_HID));
    }
    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HFP)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_HFP));
    }
    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_OPP)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_OPP));
    }
    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_FTP)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_FTP));
    }
    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_A2DP)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_A2DP));
    }
    if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_AVRCP)
    {
        json_object_array_add(pServices, json_object_new_string(NETAPP_DB_BT_SERVICE_AVRCP));
    }

    json_object_object_add(pObject, NETAPP_DB_BT_INFO_SERVICES,     pServices);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_ADDR,         pAddr);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_NAME,         pName);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_HID_INFO,     pHidData);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_LINK_KEY,     pLinkKey);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_MAJOR_COD,    pMajorCOD);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_MINOR_COD,    pMinorCOD);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_SERVICE_COD,  pServiceCOD);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_HAS_LINK_KEY, pHasLinkKey);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_PID,          pPID);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_VID,          pVID);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_SERVICE_MASK, pTrustedServiceMask);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_KEY_TYPE,     pKeyType);
    json_object_object_add(pObject, NETAPP_DB_BT_INFO_DEV_FEATURES, pDevFeatures);

err_out:
    return pObject;

}



#else
NETAPP_RETCODE NetAppSettingsPrivateInit(
    NETAPP_SETTINGS_HANDLE  *phNetAppSettings,
    const char              *pDbPath)
{
    NETAPP_UNUSED(phNetAppSettings);
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppSettingsPrivateDeinit(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_UNUSED(hNetAppSettings);
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppSettingsPrivateSaveWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pApInfo);
    return NETAPP_SUCCESS;
}


void NetAppSettingsPrivateDeleteWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pApInfo);
}


NETAPP_RETCODE NetAppSettingsPrivateGetSavedWiFiApInfoList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pulCount)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pApInfoList);
    NETAPP_UNUSED(pulCount);
    return NETAPP_SUCCESS;
}


void NetAppSettingsPrivateReset(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_UNUSED(hNetAppSettings);
}


NETAPP_RETCODE NetAppSettingsPrivateGetCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pApInfo);
    return NETAPP_SUCCESS;
}


void NetAppSettingsPrivateClearCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_UNUSED(hNetAppSettings);
}


void NetAppSettingsPrivateSetCurrentInterface(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_IFACE            tCurIface)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(tCurIface);
}


NETAPP_RETCODE NetAppSettingsPrivateSaveBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pBtDevInfo);
    return NETAPP_SUCCESS;
}


void NetAppSettingsPrivateDeleteBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pBtDevInfo);
}


NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDevList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     **pBtDevInfoList,
    uint32_t                *pulCount)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(pBtDevInfoList);
    NETAPP_UNUSED(pulCount);
    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_HW_ADDR          tAddr,
    NETAPP_BT_DEV_INFO      *pBtDevInfo)
{
    NETAPP_UNUSED(hNetAppSettings);
    NETAPP_UNUSED(tAddr);
    NETAPP_UNUSED(pBtDevInfo);
    return NETAPP_SUCCESS;
}
#endif
