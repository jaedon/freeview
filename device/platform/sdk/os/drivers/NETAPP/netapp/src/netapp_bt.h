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
 * $brcm_Workfile: netapp_bt.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/3/12 4:18p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /NetApp/netapp/src/netapp_bt.h $
 * 
 * 2   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 * 
 * 2   12/9/11 4:44p steven
 * SW7231-513: USB Hotplug Support in NetApp
 * 
 * SW7231-513/1   12/9/11 3:36p steven
 * SW7231-513: Finished working hotplug detection
 * 
 * 1   11/24/11 5:13p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 * 
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 ***************************************************************************/
/**
 * @brief NetApp Bluetooth API
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
#ifndef NETAPP_BLUETOOTH_PRIVATE_H
#define NETAPP_BLUETOOTH_PRIVATE_H
#include "netapp_priv.h"
#include "netapp_settings.h"

#define NETAPP_LINUX_INPUT_PATH         "/dev/input"
#define NETAPP_BTUSB_PATH               "/dev/btusb0"
#define NETAPP_HID_MAJOR                13
#define NETAPP_HID_MINOR_START          64

typedef struct sNETAPP_BT *NETAPP_BT_HANDLE;

#define NETAPP_BSA_SERVER   "bsa_server"

#define NETAPP_BT_BSA_CHECK(api, retcode)                                       \
{                                                                               \
    int status = 0;                                                             \
    if ((status = api) != BSA_SUCCESS)                                          \
    {                                                                           \
        NETAPP_ERR(("%s(): [%s] Failed, Error=%d", __FUNCTION__, #api, status));\
        tRetCode = NETAPP_FAILURE;                                              \
        goto err_out;                                                           \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        tRetCode = NETAPP_SUCCESS;                                              \
    }                                                                           \
}

/*******************************************************************************
 *                 Public (but Internal to NetApp) Bluetooth API's
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothPrivateInit(
    NETAPP_BT_HANDLE        *phNetAppBT,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);


NETAPP_RETCODE NetAppBluetoothPrivateDeinit(
    NETAPP_BT_HANDLE        hNetAppBT);


NETAPP_RETCODE NetAppBluetoothPrivateSettingsRefresh(
    NETAPP_BT_HANDLE        hNetAppBT);


NETAPP_RETCODE NetAppBluetoothPrivateDiscover(
    NETAPP_BT_HANDLE        hNetAppBT,
    uint32_t                tServices);


NETAPP_RETCODE NetAppBluetoothPrivateGetDiscoveryResults(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      **pBtDevInfo,
    uint32_t                *pulCount);

NETAPP_RETCODE NetAppBluetoothPrivateSimplePairingAck(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bAccept,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothPrivateConnect(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothPrivateDisconnect(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo);

NETAPP_RETCODE NetAppBluetoothPrivateAvStart(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat);

NETAPP_RETCODE NetAppBluetoothPrivateAvStop(
    NETAPP_BT_HANDLE        hNetAppBT);


bool NetAppBluetoothPrivateIsEnabled(
    NETAPP_BT_HANDLE        hNetAppBT);


NETAPP_RETCODE NetAppBluetoothPrivateHotPlug(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bInserted);

void NetAppBluetoothPrivePrintBTDevInfo(
	NETAPP_BT_DEV_INFO      *pDevInfo);

#endif //NETAPP_BLUETOOTH_PRIVATE_H
