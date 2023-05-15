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
 * $brcm_Workfile: netapp_dhcpcd.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/24/11 5:14p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_dhcpcd.h $
 * 
 * 2   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 * 
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp DHCPCD Code
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Header file to defined functions used to control te dhcp client daemon
 *
 */
#ifndef NETAPP_DHCPCD_H
#define NETAPP_DHCPCD_H
#include "netapp.h"

typedef struct sNETAPP_DHCPCD *NETAPP_DHCPCD_HANDLE;


NETAPP_RETCODE NetAppDhcpcdInit(
    NETAPP_DHCPCD_HANDLE  *phNetAppDhcpcd);


NETAPP_RETCODE NetAppDhcpcdDeInit(
    NETAPP_DHCPCD_HANDLE  hNetAppDhcpcd);


bool NetAppDhcpcdIsRunning(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName);


NETAPP_RETCODE NetAppDhcpcdStop(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName);

NETAPP_RETCODE NetAppDhcpcdWaitForTaskCompelte(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName);


NETAPP_RETCODE NetAppDhcpcdStart(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_IP_MODE          tMode,
    const char              *pIfaceName,
    NETAPP_IFACE            tIface,
    NETAPP_CALLBACK         pCallback,
    void                    *pParam,
    bool                    bLite);

#endif //NETAPP_DHCPCD_H
