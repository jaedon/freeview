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
 * Module Description:
 *
 * Revision History:
 *
 *
 ***************************************************************************/
 /*
 * NetApp Bluetooth AV Source API
 */

#ifndef NETAPP_BLUETOOTH_AV_PRIVATE_H
#define NETAPP_BLUETOOTH_AV_PRIVATE_H
#include "netapp_bt.h"
#include "bsa_av_api.h"

#define NETAPP_AV_DEVICE_MAX                   1

typedef struct sNETAPP_BT_AV *NETAPP_BT_AV_HANDLE;

/*******************************************************************************
 *                 Public (but Internal to NetApp) Bluetooth API's
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothAvPrivateInit(
    NETAPP_BT_AV_HANDLE    *phNetAppBTAv,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);

NETAPP_RETCODE NetAppBluetoothAvPrivateConnect(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothAvPrivateDisconnect(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothAvPrivateDeinit(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv);


NETAPP_RETCODE NetAppBluetoothAvPrivateSendAudioBuffer(
    NETAPP_BT_HANDLE        hNetAppBT,
    void                    *pBuf,
    uint32_t                ulLength);


NETAPP_RETCODE NetAppBluetoothAvPrivateStart(
    NETAPP_BT_AV_HANDLE     hNetAppBTAv,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat);


NETAPP_RETCODE NetAppBluetoothAvPrivateStop(
    NETAPP_BT_AV_HANDLE     hNetAppBTAv);
#endif //NETAPP_BLUETOOTH_AV_PRIVATE_H
