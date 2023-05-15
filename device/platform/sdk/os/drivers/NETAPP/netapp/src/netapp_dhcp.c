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
 * $brcm_Workfile: netapp_dhcp.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/28/11 2:14p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_dhcp.c $
 * 
 * 1   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 * 
 * dev_main_SW7425-749/1   6/28/11 2:24p steven
 * Check in initial P2P support
 * 
 *
 ***************************************************************************/
/**
 * @brief NetApp DHCP Server Implementation
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 */
#include "netapp_priv.h"
#include "netapp_dhcp.h"
#include "dhcp.h"

struct sNETAPP_DHCP
{
    uint32_t        ulTaskId;
    void            *hDHCPHandle;
    NETAPP_CALLBACK pCallback;
    void            *pCallbackParam;
    NETAPP_IFACE    tIFace;
};


static void NetAppDhcp_P_Task(
    void                *pParam);


static void NetAppDhcp_P_RequestCallback(
     unsigned long      ip_address,
     void               *pParam);


NETAPP_RETCODE NetAppDhcpInit(
    NETAPP_DHCP_HANDLE      *phNetAppDhcp)
{
    NETAPP_RETCODE          tRetCode    = NETAPP_SUCCESS;
    NETAPP_DHCP_HANDLE      hNetAppDhcp = NULL;
    int                     i;

    if (phNetAppDhcp == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppDhcp = (NETAPP_DHCP_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_DHCP));
    if (hNetAppDhcp == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppDhcp, 0, sizeof(struct sNETAPP_DHCP));

   *phNetAppDhcp = hNetAppDhcp;
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppDhcpDeInit(
    NETAPP_DHCP_HANDLE  hNetAppDhcp)
{
    int i;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcp);

    NetAppDhcpStop(hNetAppDhcp);

    NetAppOSMemFree(hNetAppDhcp);
    hNetAppDhcp = NULL;

    return NETAPP_SUCCESS;

}


NETAPP_RETCODE NetAppDhcpStop(
    NETAPP_DHCP_HANDLE      hNetAppDhcp)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcp);

    if (hNetAppDhcp->ulTaskId != 0)
    {
        DHCP_Shutdown();
        NetAppOSTaskJoin(hNetAppDhcp->ulTaskId);
        DHCP_Unload();
        NetAppOSTaskDelete(hNetAppDhcp->ulTaskId);
        hNetAppDhcp->ulTaskId = 0;
    }
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppDhcpStart(
    NETAPP_DHCP_HANDLE      hNetAppDhcp,
    char                    *pIfaceName,
    NETAPP_IFACE            tIface,
    NETAPP_IP_SETTINGS      tSettings,
    NETAPP_CALLBACK            pCallback,
    void                     *pParam)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_FAILURE;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcp);

    if (hNetAppDhcp->ulTaskId != 0)
    {
        NETAPP_MSG(("%s(): Already Running!", __FUNCTION__));
        return NETAPP_SUCCESS;
    }

    NETAPP_MSG(("%s(): Subnet=0x%x, IPAddress=0x%x PrimaryDNS=0x%x", __FUNCTION__,
        (tSettings.tIpAddress & tSettings.tSubnetMask), tSettings.tIpAddress, tSettings.tPrimaryDNS));

    hNetAppDhcp->hDHCPHandle = DHCP_init(htonl(tSettings.tIpAddress & tSettings.tSubnetMask), 100,
        200, htonl(tSettings.tIpAddress), htonl(tSettings.tPrimaryDNS), pIfaceName, NetAppDhcp_P_RequestCallback, hNetAppDhcp);

    hNetAppDhcp->pCallback      = pCallback;
    hNetAppDhcp->pCallbackParam = pParam;
    hNetAppDhcp->tIFace         = tIface;

    if (hNetAppDhcp->hDHCPHandle == NULL)
    {
        NETAPP_ERR(("%s(): Failed to spawn the dhcp server", __FUNCTION__));
        goto err_out;
    }

    hNetAppDhcp->ulTaskId = NetAppOSTaskSpawn("NetAppDhcp_P_Task", NETAPP_OS_PRIORITY_LOW,
        32*1024, NetAppDhcp_P_Task, hNetAppDhcp);

    if (hNetAppDhcp->ulTaskId == 0)
    {
        NETAPP_ERR(("%s()  Cannot create DHCP Task\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    tRetCode = NETAPP_SUCCESS;

err_out:
    if (tRetCode != NETAPP_SUCCESS)
    {
        if (hNetAppDhcp->hDHCPHandle != NULL)
        {
            DHCP_Unload();
        }
    }
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


/*******************************************************************************
 *                       Static NetApp DHCP Private Functions
 ******************************************************************************/
static void NetAppDhcp_P_Task(
    void                *pParam)
{
    NETAPP_DHCP_HANDLE hNetAppDhcp = (NETAPP_DHCP_HANDLE)pParam;

    if (hNetAppDhcp == NULL)
    {
        NETAPP_ERR(("%s(): Passed garbage", __FUNCTION__));
        goto err_out;
    }

    unsigned char   broadcastMacAddress[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    MACAllocate(broadcastMacAddress, NULL);

    DHCP_main(hNetAppDhcp->hDHCPHandle);
err_out:
    hNetAppDhcp->ulTaskId = 0;
    NetAppOSTaskExit();
    return;
}


static void NetAppDhcp_P_RequestCallback(
     unsigned long      ip_address,
     void               *pParam)
{
    NETAPP_DHCP_HANDLE hNetAppDhcp = (NETAPP_DHCP_HANDLE)pParam;
    if (hNetAppDhcp == NULL)
    {
        NETAPP_ERR(("%s(): Passed garbage", __FUNCTION__));
        return;
    }

    NETAPP_MSG(("%s(): Entered", __FUNCTION__));

    /* Send broadcast or internal callback? */
    if (hNetAppDhcp->pCallback)
    {
        (hNetAppDhcp->pCallback)(hNetAppDhcp->pCallbackParam, NETAPP_CB_DHCP_LEASE_RESPONSE, NULL, ip_address, NETAPP_SUCCESS, hNetAppDhcp->tIFace);
    }
    else
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_DHCP_LEASE_RESPONSE, NULL, 0, ip_address, NETAPP_SUCCESS, hNetAppDhcp->tIFace);
    }
}

