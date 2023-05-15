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
 * $brcm_Workfile: netapp_wfi.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/14/11 10:41a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/wlan/netapp_wfi.h $
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

#ifndef NETAPP_WIFI_INVITE_PRIV_H_
#define NETAPP_WIFI_INVITE_PRIV_H_
#include "netapp_priv.h"

#define NETAPP_WIFI_INVITE_INTERVAL     10000   //!< Scan every 10 seconds


typedef struct sNETAPP_WFI *NETAPP_WFI_HANDLE;

NETAPP_RETCODE NetAppWiFiInvitePrivateInit(
    NETAPP_WFI_HANDLE       *phNetAppWfiHandle,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_SETTINGS         *pSettings);

NETAPP_RETCODE NetAppWiFiInvitePrivateDeInit(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);


NETAPP_RETCODE NetAppWiFiInvitePrivateSettingsRefresh(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);


NETAPP_RETCODE NetAppWiFiInvitePrivateScan(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);


NETAPP_RETCODE NetAppWiFiInvitePrivateStart(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);


NETAPP_RETCODE NetAppWiFiInvitePrivateStop(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);

NETAPP_RETCODE NetAppWiFiInvitePrivateScanResults(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle);


NETAPP_RETCODE NetAppWiFiInvitePrivateAccept(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle,
    const char*             pSSID,
    NETAPP_WIFI_AP_INFO     *pApInfo);

NETAPP_RETCODE NetAppWiFiInvitePrivateReject(
    NETAPP_WFI_HANDLE       hNetAppWfiHandle,
    char                    *pSSID);

#endif /* NETAPP_WIFI_INVITE_PRIV_H_ */
