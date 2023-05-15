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
 * $brcm_Workfile: netapp_dhcpcd.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 1/5/12 1:21p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_dhcpcd.c $
 * 
 * 6   1/5/12 1:21p steven
 * SW7429-38: NetApp Enhancements for CES
 * 
 * 5   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 * 
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 * 
 * 4   8/31/11 10:23a steven
 * SWBLURAY-27195: Ported fix from Ryan and bdvd_v3.3 to Mainline
 * 
 * 3   8/4/11 10:15a steven
 * SWBLURAY-26638: Moved Auto-reconnect down to NetApp
 * 
 * fix_bdvd_v3.0_SWBLURAY-26638/1   7/29/11 4:08p steven
 * SWBLURAY-26638: Ported fix to mainline
 * 
 * 2   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 * 
 * dev_main_SW7425-749/1   6/21/11 12:55p steven
 * Added WoWL support
 * 
 * 1   6/14/11 10:40a steven
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
#include "netapp_priv.h"
#include <signal.h>

typedef enum
{
    NETAPP_DHCPCD,
    NETAPP_DHCPCD5,
    NETAPP_UDHCPC,
} NETAPP_DHCPC_TYPE;

typedef struct
{
    char                cIfaceName[NETAPP_IFACE_NAME_LEN+1];
    uint32_t            ulTaskId;
    bool                bStop;
    NETAPP_DHCPC_TYPE   tType;
    NETAPP_IP_MODE      tMode;
    NETAPP_IFACE        tIface;
    int                 pid;
    NETAPP_CALLBACK     pCallback;
    void                *pParam;
    bool                bLite;
}NETAPP_DHCPCD_INFO;


struct sNETAPP_DHCPCD
{
    NETAPP_DHCPCD_INFO  tInfo[NETAPP_IFACE_MAX];
};


static inline NETAPP_DHCPCD_INFO* NetAppDhcpcd_P_GetContext(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName);


static void NetAppDhcpcd_P_Task(
    void                *pParam);


NETAPP_RETCODE NetAppDhcpcdInit(
    NETAPP_DHCPCD_HANDLE  *phNetAppDhcpcd)
{
    NETAPP_RETCODE          tRetCode        = NETAPP_SUCCESS;
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd   = NULL;
    int                     i;
    NETAPP_DHCPC_TYPE       tType;
    struct stat             statbuf;

    if (phNetAppDhcpcd == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppDhcpcd = (NETAPP_DHCPCD_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_DHCPCD));
    if (hNetAppDhcpcd == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppDhcpcd, 0, sizeof(struct sNETAPP_DHCPCD));

    /* Need to find out the version of dhcpcd that we have first */
   if (NetAppPrivateSystem(NETAPP_DHCPCD_CMD, "--version", "dhcpcd ", NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL) == NETAPP_SUCCESS)
   {
       tType = NETAPP_DHCPCD5;
   }
   else if (stat(NETAPP_UDHCPC_CMD, &statbuf) == 0)
   {
       tType = NETAPP_UDHCPC;
   }
   else
   {
       tType = NETAPP_DHCPCD;
   }

   for (i = 0; i < NETAPP_IFACE_MAX; i++)
   {
       hNetAppDhcpcd->tInfo[i].tType = tType;
   }

   *phNetAppDhcpcd = hNetAppDhcpcd;
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppDhcpcdDeInit(
    NETAPP_DHCPCD_HANDLE  hNetAppDhcpcd)
{
    int i;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcpcd);

    for (i = 0; i < NETAPP_IFACE_MAX; i++)
    {
        if (hNetAppDhcpcd->tInfo[i].cIfaceName[0] != '\0')
        {
            NetAppDhcpcdStop(hNetAppDhcpcd, hNetAppDhcpcd->tInfo[i].cIfaceName);
        }
    }

    NetAppOSMemFree(hNetAppDhcpcd);
    hNetAppDhcpcd = NULL;

    return NETAPP_SUCCESS;

}


bool NetAppDhcpcdIsRunning(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName)
{
    NETAPP_DHCPCD_INFO  *pInfo      = NULL;

    if (hNetAppDhcpcd == NULL)
    {
        return false;
    }

    if ( (pInfo = NetAppDhcpcd_P_GetContext(hNetAppDhcpcd, pIfaceName)) == NULL)
    {
        NETAPP_MSG(("%s(): DHCPCD Not Running for %s!", __FUNCTION__, pIfaceName));
        return false;
    }
    return true;
}


NETAPP_RETCODE NetAppDhcpcdStop(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_DHCPCD_INFO  *pInfo      = NULL;
    char                cPIDFile[128];
    struct stat         statbuf;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcpcd);

    if (pIfaceName == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }


    if ( (pInfo = NetAppDhcpcd_P_GetContext(hNetAppDhcpcd, pIfaceName)) == NULL)
    {
        NETAPP_MSG(("%s(): DHCPCD Not Running for %s!", __FUNCTION__, pIfaceName));
        return NETAPP_SUCCESS;
    }

    if (pInfo->tType == NETAPP_UDHCPC)
    {
        snprintf(cPIDFile, sizeof(cPIDFile), "/tmp/udhcp-%s.pid", pIfaceName);
    }
    else
    {
        snprintf(cPIDFile, sizeof(cPIDFile), "%s/dhcpcd-%s.pid",
            (pInfo->tType == NETAPP_DHCPCD5) ? NETAPP_DHCPCD5_CONFIG_PATH : NETAPP_DHCPCD_CONFIG_PATH, pIfaceName);
    }

    /* There is a background PID file */
    if (stat(cPIDFile, &statbuf) == 0)
    {
        int  i;

        if (pInfo->tType == NETAPP_DHCPCD5)
        {
            char cArgs[16];
            snprintf(cArgs, sizeof(cArgs), "%s -k",  pIfaceName);

            if (NetAppPrivateSystem(NETAPP_DHCPCD_CMD, cArgs, NULL, NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL) != NETAPP_SUCCESS)
            {
                NETAPP_ERR(("%s(): CMD[%s] failed", __FUNCTION__, cArgs));
                tRetCode = NETAPP_FAILURE;
                goto err_out;
            }
        }
        else
        {
            FILE *pFd = fopen(cPIDFile, "r");
            char cLine[24];
            if (pFd != NULL)
            {
                if (fgets(cLine, sizeof(cLine), pFd) != NULL)
                {
                    int32_t pid = atoi(cLine);
                    NETAPP_MSG(("%s(): Sending SIGKILL to dhcpcd[%s]",  __FUNCTION__, pIfaceName));
                    kill(pid, SIGTERM);
                }
                fclose(pFd);
            }
        }

        for (i = 0; i < NETAPP_KILL_DHCPCD_LOOP_CNT; i++)
        {
            if (stat(cPIDFile, &statbuf) != 0)
            {
                break;
            }
            NetAppOSTaskDelayMsec(NETAPP_KILL_DHCPCD_WAIT_MS);
        }
        if (i == NETAPP_KILL_DHCPCD_LOOP_CNT)
        {
            NETAPP_MSG(("%s Could not gracefully shutdown dhcpcd for %s after %d ms",
                __FUNCTION__, pIfaceName, NETAPP_KILL_DHCPCD_TIMEOUT));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
        NETAPP_MSG(("%s Graceful kill of DHCPCD on %s successful after %d ms",
            __FUNCTION__, pIfaceName, i));
    }
    else if (pInfo->pid)
    {
        NETAPP_MSG(("%s Sending TERM to dhcpcd %d", __FUNCTION__, pIfaceName));
        kill(pInfo->pid, SIGTERM);
    }

    pInfo->pid = 0;

    /* Is dhcpcd spawned but not forked? */
    if (pInfo->ulTaskId != 0)
    {
        pInfo->bStop = true;
        NetAppOSTaskJoin(pInfo->ulTaskId);
        NetAppOSTaskDelete(pInfo->ulTaskId);
        NETAPP_MSG(("%s Forcefully killed of DHCPCD successful", __FUNCTION__));
    }
    pInfo->bStop = false;
    pInfo->ulTaskId = 0;
    memset(pInfo->cIfaceName, 0, sizeof(pInfo->cIfaceName));

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppDhcpcdWaitForTaskCompelte(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_DHCPCD_INFO  *pInfo      = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcpcd);

    if (pIfaceName == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if ( (pInfo = NetAppDhcpcd_P_GetContext(hNetAppDhcpcd, pIfaceName)) == NULL)
    {
        NETAPP_MSG(("%s(): DHCPCD Not Running for %s!", __FUNCTION__, pIfaceName));
        return NETAPP_SUCCESS;
    }

    if (pInfo->ulTaskId != 0)
    {
        NetAppOSTaskJoin(pInfo->ulTaskId);
        NetAppOSTaskDelete(pInfo->ulTaskId);
        pInfo->ulTaskId = 0;
    }
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppDhcpcdStart(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_IP_MODE          tMode,
    const char              *pIfaceName,
    NETAPP_IFACE            tIface,
    NETAPP_CALLBACK         pCallback,
    void                    *pParam,
    bool                    bLite)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_DHCPCD_INFO  *pInfo      = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDhcpcd);

    if ( (pInfo = NetAppDhcpcd_P_GetContext(hNetAppDhcpcd, pIfaceName)) != NULL)
    {
        NetAppDhcpcdStop(hNetAppDhcpcd, pIfaceName);
    }
    else
    {
        int i;
        for (i = 0; i < NETAPP_IFACE_MAX; i++)
        {
            if (hNetAppDhcpcd->tInfo[i].cIfaceName[0] == '\0')
            {
                pInfo = &hNetAppDhcpcd->tInfo[i];
                break;
            }
        }
        if (pInfo == NULL)
        {
            NETAPP_ERR(("%s(): Cannot Find free DHCPCD slot, this should never happen\n", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
    }

    strncpy(pInfo->cIfaceName, pIfaceName, sizeof(pInfo->cIfaceName)/sizeof(pInfo->cIfaceName[0])-1);
    pInfo->cIfaceName[sizeof(pInfo->cIfaceName)/sizeof(pInfo->cIfaceName[0])-1] = '\0';

    pInfo->tMode        = tMode;
    pInfo->tIface       = tIface;
    pInfo->pCallback    = pCallback;
    pInfo->pParam       = pParam;
    pInfo->bLite        = bLite;

    pInfo->ulTaskId = NetAppOSTaskSpawn("NetAppDhcpcd_P_Task", NETAPP_OS_PRIORITY_LOW,
        32*1024, NetAppDhcpcd_P_Task, pInfo);

    if (pInfo->ulTaskId == 0)
    {
        NETAPP_ERR(("%s()  Cannot create DHCPCD Task\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    return NETAPP_SUCCESS;

err_out:
    if (pInfo != NULL)
    {
        NetAppOSMemFree(pInfo);
    }
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


/*******************************************************************************
 *                       Static NetApp DHCPCD Private Functions
 ******************************************************************************/
static void NetAppDhcpcd_P_Task(
    void                *pParam)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_SUCCESS;
    char                cArgs[128];
    char                tmp[128];
    struct stat         statbuf;
    bool                bNewDhcpcd = false;
    char                *pCommand= NULL;
    NETAPP_DHCPCD_INFO  *pInfo  = (NETAPP_DHCPCD_INFO*)pParam;


    if (pInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter", __FUNCTION__));
        return;
    }

    memset(cArgs, 0, sizeof(cArgs));


    if (pInfo->tType == NETAPP_UDHCPC)
    {
        pCommand = NETAPP_UDHCPC_CMD;
        snprintf(cArgs, sizeof(cArgs), "-i %s -p /tmp/udhcp-%s.pid", pInfo->cIfaceName, pInfo->cIfaceName);
        if (pInfo->bLite)
        {
            strcat(cArgs, " -s ");
            strcat(cArgs, UDHCPC_LITE_SCRIPT);
        }
    }
    else
    {
        pCommand = NETAPP_DHCPCD_CMD;

        snprintf(cArgs, sizeof(cArgs), "%s -t", pInfo->cIfaceName);

        snprintf(tmp, sizeof(tmp), " %d%s", (pInfo->tType == NETAPP_DHCPCD5) ? NETAPP_DHCPCD5_WAIT_TIME : NETAPP_DHCPCD_WAIT_TIME,
            ( (pInfo->tType == NETAPP_DHCPCD5) && (pInfo->tMode != NETAPP_IP_MODE_AUTO_IP) ) ? " --noarp --noipv4ll" : "");

        strcat(cArgs, tmp);

        if (pInfo->tType != NETAPP_DHCPCD5)
        {
            snprintf(tmp, sizeof(tmp), " -L %s", NETAPP_DHCPCD_CONFIG_PATH);
            strcat(cArgs, tmp);
        }
    }

    tRetCode = NetAppPrivateSystem(pCommand,
        cArgs, NULL, NETAPP_WAIT_FOREVER, &pInfo->bStop, NULL, 0, &pInfo->pid);

    if (tRetCode != NETAPP_SUCCESS)
    {
        NETAPP_MSG(("%s() dhcp server %s not responding", __FUNCTION__, pInfo->cIfaceName));
        tRetCode = NETAPP_DHCP_FAILURE;
        goto err_out;
    }

    /* Restart the Linux Resolver to reload the new name server settings */
    res_init();

err_out:

    /* Send broadcast or internal callback? */
    if (pInfo->pCallback)
    {
        (pInfo->pCallback)(pInfo->pParam, NETAPP_CB_SETSETTINGS, NULL, 0, tRetCode, pInfo->tIface);
    }
    else
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_SETSETTINGS, NULL, 0, 0, tRetCode, pInfo->tIface);
    }
    pInfo->ulTaskId = 0;
    NetAppOSTaskExit();
    return;
}


static inline NETAPP_DHCPCD_INFO* NetAppDhcpcd_P_GetContext(
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    const char              *pIfaceName)
{
    int                 i;
    NETAPP_DHCPCD_INFO  *pInfo = NULL;

    if ( (pIfaceName == NULL) || (hNetAppDhcpcd == NULL) )
    {
        NETAPP_MSG(("%s(): Invalid Parameter", __FUNCTION__));
        return NULL;
    }

    for (i = 0; i < NETAPP_IFACE_MAX; i++)
    {
        if ( (hNetAppDhcpcd->tInfo[i].cIfaceName[0] != '\0') &&
             (strncmp(pIfaceName, hNetAppDhcpcd->tInfo[i].cIfaceName, NETAPP_IFACE_NAME_LEN) == 0) )
        {
            pInfo = &hNetAppDhcpcd->tInfo[i];
            break;
        }
    }

    return pInfo;
}

