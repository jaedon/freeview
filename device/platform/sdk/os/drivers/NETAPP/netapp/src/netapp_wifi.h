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
 * $brcm_Workfile: netapp_wifi.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_wifi.h $
 *
 * 2   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 3   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * 2   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 *
 * dev_main_SW7425-749/1   6/28/11 2:24p steven
 * Check in initial P2P support
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
 * Private Internal API's called by netapp.c
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#ifndef NETAPP_WIFI_PRIVATE_H
#define NETAPP_WIFI_PRIVATE_H
#include "netapp_priv.h"
#include "netapp_wfi.h"
#include "netapp_p2p.h"
#include "netapp_settings.h"

typedef struct sNETAPP_WIFI *NETAPP_WIFI_HANDLE;

#define NETAPP_DEFAULT_SCANTIME_MS      200

typedef enum
{
    NETAPP_SCAN_NONE            = 0x00,
    NETAPP_SCAN_NORMAL          = 0x01,
    NETAPP_SCAN_INVITE          = 0x02,
    NETAPP_SCAN_P2P             = 0x04,
    NETAPP_SCAN_ALL             = 0xff,
} NETAPP_SCAN_TYPE;


/*******************************************************************************
 *                 Public (but Internal to NetApp) WiFi API's
 ******************************************************************************/
NETAPP_RETCODE NetAppWiFiPrivateInit(
    NETAPP_WIFI_HANDLE      *phNetAppWiFi,
    NETAPP_WFI_HANDLE       hNetAppWifiInvite,
    NETAPP_P2P_HANDLE       hNetAppP2P,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings);


NETAPP_RETCODE NetAppWiFiPrivateSettingsRefresh(
    NETAPP_WIFI_HANDLE      hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateDeinit(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateSetInterfaceUp(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    bool                bIsUp);


NETAPP_RETCODE NetAppWiFiPrivateConnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_CONNECT_TYPE tConnectType,
    void                *pParam,
    uint32_t            ulWpsPin);


NETAPP_RETCODE NetAppWiFiPrivateStartScan(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    int32_t             lTickMs,
    int32_t             lScanTimeMs,
    NETAPP_SCAN_TYPE    tType,
    NETAPP_P2P_DISCOVER_PARAMS *pParams);


NETAPP_RETCODE NetAppWiFiPrivateFetchApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    char                *pSSID);


NETAPP_RETCODE NetAppWiFiPrivateStopScan(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_SCAN_TYPE    tType);


bool NetAppWiFiPrivateIsEnabled(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateIsPresent(
    char                *pIfName,
    uint32_t            ulLength);


NETAPP_RETCODE NetAppWiFiPrivateDisconnect(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateGeneratePin(
    uint32_t            *pulPin);


NETAPP_RETCODE NetAppWiFiPrivateValidatePin(
    uint32_t            ulPin);


NETAPP_RETCODE NetAppWiFiPrivateGetConnectedAp(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo);


NETAPP_RETCODE NetAppWiFiPrivateHotPlug(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    const char          *pIFaceName,
    uint32_t            ulIfaceLength,
    bool                bInserted);

NETAPP_RETCODE NetAppWiFiPrivateGetConnectedApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo);


NETAPP_RETCODE NetAppWiFiPrivateGetScanResults(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO **pApInfoList,
    uint32_t            *pulScanCount);


NETAPP_RETCODE NetAppWiFiPrivateGetScannedApInfo(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_AP_INFO *pApInfo);


char* NetAppWiFiPrivateGetIfaceName(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_LINK_STATE NetAppWiFiPrivateGetLinkState(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateSetNetworkSettings(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings);


NETAPP_RETCODE NetAppWiFiPrivateJoinBridge(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    const char          *pBridgeName);


NETAPP_RETCODE NetAppWiFiPrivateLeaveBridge(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiPrivateSetAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t ulAntennas);


NETAPP_RETCODE NetAppWiFiPrivateGetAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_ANTENNA_MODE tMode,
    uint32_t *pulAntennas);


NETAPP_RETCODE NetAppWiFiPrivateGetAntennaCount(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    uint32_t *pulAntennas);


NETAPP_RETCODE NetAppWiFiTestPrivateStartTransmission(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_TXRX_SETTINGS *pTxSettings);


NETAPP_RETCODE NetAppWiFiTestPrivateStopTransmission(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiTestPrivateStartReceiving(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_TXRX_SETTINGS *pRxSettings);


NETAPP_RETCODE NetAppWiFiTestPrivateStopReceiving(
    NETAPP_WIFI_HANDLE  hNetAppWiFi);


NETAPP_RETCODE NetAppWiFiTestPrivateGetRssiPerAntenna(
    NETAPP_WIFI_HANDLE  hNetAppWiFi,
    NETAPP_WIFI_RSSI_PER_ANTENNA *pRssiPerAntenna);


NETAPP_RETCODE NetAppWiFiTestPrivateControlWaveform(
    NETAPP_WIFI_HANDLE hNetAppWiFi,
    uint32_t ulChannel,
    bool bEnable);

#endif
