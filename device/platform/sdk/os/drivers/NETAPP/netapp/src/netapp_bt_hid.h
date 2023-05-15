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
 * $brcm_Workfile: netapp_bt_hid.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_bt_hid.h $
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
 * SW7425-1404/2   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 ***************************************************************************/
/**
 * @brief NetApp Bluetooth HID API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by API Thunk (netapp.c)
 *
 * @remarks This API should not be used by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#ifndef NETAPP_BLUETOOTH_HID_PRIVATE_H
#define NETAPP_BLUETOOTH_HID_PRIVATE_H
#include "netapp_bt.h"

#define NETAPP_HID_DEVICE_MAX                   7
#define NETAPP_HID_AUDIO_REPORT_ID              0xF7
/* audio format offset*/
#define NETAPP_HID_AUDIO_FORMAT_OFFSET          1


#define NETAPP_HID_AUDIO_DATA                   0x01

#define NETAPP_HID_AUDIO_MAX_REPORT_SIZE          16

/* From Host to HID device */
#define NETAPP_HID_AUDIO_MIC_START              0x02
#define NETAPP_HID_AUDIO_MIC_STOP               0x03
#define NETAPP_HID_AUDIO_SPK_START              0x04
#define NETAPP_HID_AUDIO_SPK_STOP               0x05
#define NETAPP_HID_AUDIO_CODEC_READ_REQ         0x06
#define NETAPP_HID_AUDIO_CODEC_WRITE_REQ        0x08
#define NETAPP_HID_AUDIO_CALL_START             0x10
#define NETAPP_HID_AUDIO_CALL_STOP              0x11
#define NETAPP_HID_AUDIO_MODE_READ_REQ          0x0A

/* From HID device to Host */
#define NETAPP_HID_AUDIO_RC_MIC_START           0x0C
#define NETAPP_HID_AUDIO_RC_MIC_STOP            0x0D
#define NETAPP_HID_AUDIO_RC_SPK_START           0x0E
#define NETAPP_HID_AUDIO_RC_SPK_STOP            0x0F
#define NETAPP_HID_AUDIO_CODEC_READ_ACK         0x07
#define NETAPP_HID_AUDIO_CODEC_WRITE_ACK        0x09
#define NETAPP_HID_AUDIO_MODE_READ_ACK          0x0B
#define NETAPP_HID_AUDIO_RC_CALL_START          0x12
#define NETAPP_HID_AUDIO_RC_CALL_STOP           0x13

#define NETAPP_HID_AUDIO_SAMP_FREQ_ID           0x00
#define NETAPP_HID_AUDIO_SAMP_FORMAT_ID         0x01
#define NETAPP_HID_AUDIO_PGA_GAIN_ID            0x02
#define NETAPP_HID_AUDIO_BITS_PER_SAMP_ID       0x03
#define NETAPP_HID_AUDIO_HP_FILTER_SET_ID       0x04

#define NETAPP_HID_AUDIO_MINIMAL_CODEC_CONFIG   ( (1 << NETAPP_HID_AUDIO_SAMP_FREQ_ID)| \
                                              (1 << NETAPP_HID_AUDIO_BITS_PER_SAMP_ID) )

#define NETAPP_HID_AUDIO_SAMP_FREQ_8K           0x20
#define NETAPP_HID_AUDIO_SAMP_FREQ_16K          0x40
#define NETAPP_HID_AUDIO_SAMP_FREQ_22K5         0x5A
#define NETAPP_HID_AUDIO_SAMP_FREQ_32K          0x80
#define NETAPP_HID_AUDIO_SAMP_FREQ_48K          0xC0

#define NETAPP_HID_AUDIO_FILE_NAME              "/tmp/voice_dump.pcm"

typedef struct sNETAPP_BT_HID *NETAPP_BT_HID_HANDLE;



/*******************************************************************************
 *                 Public (but Internal to NetApp) Bluetooth API's
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothHidPrivateInit(
    NETAPP_BT_HID_HANDLE    *phNetAppBTHid,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings);


NETAPP_RETCODE NetAppBluetoothHidPrivateConnect(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothHidPrivateDisconnect(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothHidPrivateDeinit(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid);


#endif //NETAPP_BLUETOOTH_HID_PRIVATE_H
