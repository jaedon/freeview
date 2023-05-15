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
 * $brcm_Workfile: netapp_settings.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/3/12 4:18p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /NetApp/netapp/src/netapp_settings.h $
 * 
 * 1   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 ***************************************************************************/
/**
 * @brief NetApp Settings API
 *
 * API is used to store and retrieve settings using the any backend.
 * Built-in settings storage and retrieval is optional feature of NetApp. This
 * API will take a particular settings and string-a-fy it using cJSON to create
 * a value that can be stored in the database backend of netapp_db.h
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
#ifndef NETAPP_SETTINGS_H
#define NETAPP_SETTINGS_H
#include "netapp_priv.h"

typedef struct sNETAPP_SETTINGS *NETAPP_SETTINGS_HANDLE;

/*******************************************************************************
 *                 Public (but Internal to NetApp) Settings API's
 ******************************************************************************/
NETAPP_RETCODE NetAppSettingsPrivateInit(
    NETAPP_SETTINGS_HANDLE  *phNetAppSettings,
    const char              *pDbPath);


NETAPP_RETCODE NetAppSettingsPrivateDeinit(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);


NETAPP_RETCODE NetAppSettingsPrivateSaveWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo);


void NetAppSettingsPrivateDeleteWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo);


NETAPP_RETCODE NetAppSettingsPrivateGetSavedWiFiApInfoList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     **pApInfoList,
    uint32_t                *pCount);


NETAPP_RETCODE NetAppSettingsPrivateGetCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_WIFI_AP_INFO     *pApInfo);


void NetAppSettingsPrivateClearCurrentWiFiApInfo(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);


void NetAppSettingsPrivateSetCurrentInterface(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_IFACE            tCurIface);

void NetAppSettingsPrivateReset(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);


NETAPP_RETCODE NetAppSettingsPrivateSaveBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo);


void NetAppSettingsPrivateDeleteBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     *pBtDevInfo);


NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDevList(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_BT_DEV_INFO     **pBtDevInfoList,
    uint32_t                *pCount);


NETAPP_RETCODE NetAppSettingsPrivateGetSavedBtDev(
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_HW_ADDR          tAddr,
    NETAPP_BT_DEV_INFO      *pBtDevInfo);

#endif //NETAPP_SETTINGS_H
