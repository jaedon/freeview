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
 * $brcm_Workfile: netapp_p2p.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/28/11 2:14p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_p2p.h $
 * 
 * 1   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 * 
 * dev_main_SW7425-749/1   6/28/11 2:24p steven
 * Check in initial P2P support
 *
 ***************************************************************************/
/**
 * @brief NetApp WiFi Direct (P2P) Private API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by netapp.c
 *
 * @remarks This API MUST NOT be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */

#ifndef NETAPP_P2P_PRIV_H
#define NETAPP_P2P_PRIV_H
#include "netapp_priv.h"

typedef struct sNETAPP_P2P *NETAPP_P2P_HANDLE;

NETAPP_RETCODE NetAppWiFiP2PPrivateInit(
    NETAPP_P2P_HANDLE           *phNetAppP2P,
    NETAPP_PRIV_HANDLE          hNetAppPriv,
    NETAPP_INIT_SETTINGS        *pInitSettings,
    NETAPP_SETTINGS             *pSettings,
    NETAPP_DEVICE_TYPE          tType,
    NETAPP_DHCPCD_HANDLE        hNetAppDhcpcd);


NETAPP_RETCODE NetAppWiFiP2PPrivateDeInit(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateHotPlug(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    const char                  *pIFaceName,
    uint32_t                    ulIfaceLength,
    bool                        bInserted);


NETAPP_RETCODE NetAppWiFiP2PPrivateDiscover(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateStopDiscovery(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateConnect(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    char                        *pName,
    uint32_t                    ulTimeoutSec);


NETAPP_RETCODE NetAppWiFiP2PPrivateDisconnect(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateGetSSID(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    char                        *pBuf,
    uint32_t                    ulBufSize);


NETAPP_LINK_STATE NetAppWiFiP2PPrivateGetLinkState(
    NETAPP_P2P_HANDLE           hNetAppP2P);


char* NetAppWiFiP2PPrivateGetIfaceName(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateSetNetworkSettings(
    NETAPP_P2P_HANDLE           hNetAppP2P,
    NETAPP_IP_MODE              tMode,
    NETAPP_IP_SETTINGS          *pSettings);


NETAPP_RETCODE NetAppWiFiP2PPrivateSettingsRefresh(
    NETAPP_P2P_HANDLE           hNetAppP2P);


NETAPP_RETCODE NetAppWiFiP2PPrivateWpsPushButton(
    NETAPP_P2P_HANDLE           hNetAppP2P);

#endif //NETAPP_P2P_PRIV_H
