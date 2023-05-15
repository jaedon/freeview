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
 * $brcm_Workfile: netapp_priv.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_priv.c $
 *
 * 3   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 13   1/5/12 1:21p steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 12   12/12/11 11:15p steven
 * SW7231-513: Various fixes
 *
 * 11   12/2/11 3:04p steven
 * SW7425-1404: Fixing NTP for STB
 *
 * 10   12/1/11 8:56p steven
 * SW7425-1404: Moved kernel modules and fixes to BT support
 *
 * 9   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/5   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * 8   9/29/11 2:12p steven
 * SWBLURAY-27464: Add support to periodic NTP updates
 *
 * 7   9/16/11 9:06a steven
 * SWBLURAY-27357: Fix for getaddrinfo from Kefei
 *
 * 6   9/15/11 11:22a steven
 * SWBLURAY-27357: Support for /etc/resolv.conf
 *
 * 5   9/1/11 1:29p steven
 * SW7425-1223: Fixed memory Leaks in NetApp
 *
 * 4   8/29/11 10:21a steven
 * SWBLURAY-27043: Fixed bug caught by Kefei
 *
 * 3   8/26/11 4:02p steven
 * SWBLURAY-27043: WPS 2.0 Support
 *
 * 2   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 *
 * dev_main_SW7425-749/1   6/21/11 12:55p steven
 * Added WoWL support
 *
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp Private API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_priv.h"
#include <linux/sockios.h>
#include <linux/mii.h>
#include <ctype.h>
#include <libgen.h>


struct sNETAPP_PRIV
{
    int32_t                 lPacketSocket;
    uint32_t                ulBkgrndTaskId;
    uint32_t                ulDNSLookupTaskId;
    char                    *pDNSLookupHostName;
    uint32_t                ulNtpTaskId;
    bool                    bStopNtpTask;
    uint32_t                ulPingTaskId;
    int32_t                 lPingTimeout;
    char                    *pPingAddress;
    bool                    bStopPing;
    NETAPP_LINK_STATE       tLinkState[NETAPP_IFACE_WIRED_MAX]; /* Wired link state */
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd;

    NETAPP_SETTINGS        tSettings;

    uint32_t                ulMsgQId;
    uint32_t                ulNTPPeriodMs;
};



static NETAPP_RETCODE NetAppPrivate_P_SetGateWay(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IPV4_ADDR    tGateway);

static NETAPP_RETCODE NetAppPrivate_P_GetGateWay(
    NETAPP_IPV4_ADDR    *pGateway);


static void NetAppPrivate_P_DNSLookupTask(
    void        *pParam);


static void NetAppPrivate_P_PingTask(
    void        *pParam);


static void NetAppPrivate_P_BackgroundTask(
    void              *pParam);

static void NetAppPrivate_P_NtpDateTask(
    void              *pParam);


static void NetAppPrivate_P_KillNtpDate(
    NETAPP_PRIV_HANDLE  hNetAppPriv);


static NETAPP_RETCODE NetAppPrivate_P_UpdateLinkState(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IFACE        tIface);


static NETAPP_RETCODE NetAppPrivate_P_SetIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IP_SETTINGS  *pSettings);


NETAPP_RETCODE NetAppPrivateInit(
    NETAPP_PRIV_HANDLE      *phNetAppPriv,
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_SETTINGS     *pSettings)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE  hNetAppPriv = NULL;
    NETAPP_LINK_STATE   tLinkState;
    FILE                *pFD        = NULL;
    char                cLine[LINE_MAX];

    int i;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));
    if (phNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppPriv = (struct sNETAPP_PRIV*)NetAppOSMemAlloc(sizeof(struct sNETAPP_PRIV));
    if (hNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppPriv, 0, sizeof(struct sNETAPP_PRIV));

    hNetAppPriv->hNetAppDhcpcd = hNetAppDhcpcd;

    hNetAppPriv->ulMsgQId = NetAppOSMsgQCreate(1, sizeof(NETAPP_MSG_INFO));
    if (hNetAppPriv->ulMsgQId == 0)
    {
        NETAPP_ERR(("%s(): Failed creating message queue!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }


    /* Packet Socket to set/get settings */
    if ((hNetAppPriv->lPacketSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        NETAPP_ERR(("%s socket(AF_PACKET) Error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

    if (NetAppPrivateSetInterfaceUp(hNetAppPriv, NETAPP_IFACE_LO, true) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s Error Setting LOOPBACK Interface Up/Down", __FUNCTION__));
        goto err_out;
    }

#ifndef DISABLE_WIRED_SUPPORT
    if (NetAppPrivateSetInterfaceUp(hNetAppPriv, NETAPP_WIRED_IFACE_NAME[NETAPP_IFACE_ETH0], true) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Unable to Initializing the WIRED interface, disabling it...", __FUNCTION__));
        tRetCode = NETAPP_SUCCESS;
        goto err_out;
    }

    if ((pFD = fopen(NETAPP_PROC_NET_DEV, "r")) == NULL)
    {
        NETAPP_ERR(("%s(): Could not read /proc/modules!!", __FUNCTION__));
        return NETAPP_FAILURE;
    }

    while(fgets(cLine, sizeof(cLine), pFD) != NULL)
    {
        char *pEth = NULL;
        if ((pEth = strstr(cLine, NETAPP_IFACE_ETH)) != NULL)
        {
            NETAPP_IFACE tIface = atoi(pEth + NETAPP_IFACE_ETH_LEN);
            NetAppPrivate_P_UpdateLinkState(hNetAppPriv, tIface);
        }
    }
#endif

    hNetAppPriv->ulBkgrndTaskId = NetAppOSTaskSpawn("NetAppPrivate_P_BackgroundTask", NETAPP_OS_PRIORITY_LOW,
        256*1024, NetAppPrivate_P_BackgroundTask, (void*)hNetAppPriv);

    if (hNetAppPriv->ulBkgrndTaskId == 0)
    {
        NETAPP_ERR(("%s() Cannot create BackgroundTask\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    hNetAppPriv->tSettings = *pSettings;

    *phNetAppPriv = hNetAppPriv;
err_out:
    if (pFD != NULL)
    {
        fclose(pFD);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateDeInit(
    NETAPP_PRIV_HANDLE hNetAppPriv)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if (hNetAppPriv->ulPingTaskId != 0)
    {
        hNetAppPriv->bStopPing = true;
        NetAppOSTaskJoin(hNetAppPriv->ulPingTaskId);
        hNetAppPriv->bStopPing = false;
        NetAppOSTaskDelete(hNetAppPriv->ulPingTaskId);
        hNetAppPriv->ulPingTaskId = 0;
    }

    if (hNetAppPriv->ulDNSLookupTaskId != 0)
    {
        NetAppOSTaskDelete(hNetAppPriv->ulDNSLookupTaskId);
        hNetAppPriv->ulDNSLookupTaskId = 0;

        if (hNetAppPriv->pDNSLookupHostName)
        {
            free(hNetAppPriv->pDNSLookupHostName);
        }
    }

    if ( (hNetAppPriv->ulBkgrndTaskId != 0) && (hNetAppPriv->ulMsgQId != 0) )
    {
        NETAPP_MSG_INFO tMessage;

        memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));
        tMessage.tMsgType = NETAPP_MSG_DIE;
        if (NetAppOSMsgQSend(hNetAppPriv->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER) == NETAPP_SUCCESS)
        {
            NetAppOSTaskJoin(hNetAppPriv->ulBkgrndTaskId);
        }
    }

    if (hNetAppPriv->ulBkgrndTaskId != 0)
    {
        NetAppOSTaskDelete(hNetAppPriv->ulBkgrndTaskId);
        hNetAppPriv->ulBkgrndTaskId = 0;
    }

    if (hNetAppPriv->ulMsgQId != 0)
    {
        NetAppOSMsgQDelete(hNetAppPriv->ulMsgQId);
        hNetAppPriv->ulMsgQId = 0;
    }

    if (hNetAppPriv->lPacketSocket > 0)
    {
        close(hNetAppPriv->lPacketSocket);
    }

    NetAppPrivate_P_KillNtpDate(hNetAppPriv);

    NetAppOSMemFree(hNetAppPriv);
    hNetAppPriv = NULL;

    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppPrivateInsmod(
    char                *pcModulesNames,
    char                *pcModulesFullPath,
    char                *pOptions)
{
    FILE            *pFD        = NULL;
    bool            bFound      = false;
    char            cLine[LINE_MAX];
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    struct stat     statbuf;
    char*           pModules = NULL;
    char*           pModulePaths = NULL;
    char            *saveptr1;
    char            *token;
    if ( (pcModulesNames == NULL) || (pcModulesFullPath == NULL) )
    {
        NETAPP_ERR(("%s() Null Pointer!", __FUNCTION__));
        return NETAPP_NULL_PTR;
    }

    memset(cLine, 0, sizeof(cLine));
    NETAPP_MSG(("%s(): Entered, Modules[%s] Fullpath[%s]", __FUNCTION__, pcModulesNames, pcModulesFullPath));

    /* Check if WiFi Kernel module is loaded and the dongle is present or not */
    /* If wiFi is present when we init NetApp, Init the driver and don't wait for hotplug */
    pFD = fopen("/proc/modules", "r");
    if (pFD == NULL)
    {
        NETAPP_ERR(("%s(): Could not read /proc/modules!!", __FUNCTION__));
        return NETAPP_FAILURE;
    }
    pModules = strdup(pcModulesNames);
    pModulePaths = strdup(pcModulesFullPath);

    token = strtok_r(pModules, " ", &saveptr1);

    while(token != NULL)
    {
        while(fgets(cLine, sizeof(cLine), pFD) != NULL)
        {
            NETAPP_MSG(("%s(): Finding %s in %s!", __FUNCTION__, token, cLine));
            if (strncmp(token, cLine, strlen(token)) == 0)
            {
                NETAPP_MSG(("%s(): %s found in /proc/modules!!", __FUNCTION__, token));
                bFound = true;
                break;
            }
        }
        if (bFound)
            break;
        rewind(pFD);
        token = strtok_r(NULL, " ", &saveptr1);
    }


    /* Not found so insmod them all! */
    if (!bFound)
    {
        char *saveptr1;
        char *token = strtok_r(pModulePaths, " ", &saveptr1);

        while(token != NULL)
        {
            if (stat(token, &statbuf) != 0)
            {
                NETAPP_ERR(("%s(): Could not find %s!!", __FUNCTION__, token));
                tRetCode = NETAPP_FAILURE;
            }
            else
            {
                char cArgs[512];
                snprintf(cArgs, sizeof(cArgs), "%s ", token);
                /* hack to not pass the options to bcm_usbshim*/
                if ((pOptions != NULL) && (strstr(cArgs, "bcm_usbshim") == NULL) )
                {
                    strcat(cArgs, pOptions);
                }
                NETAPP_MSG(("%s(): Insmod %s", __FUNCTION__, token));
                tRetCode = NetAppPrivateSystem(NETAPP_INSMOD_CMD, cArgs, NULL, NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL);
            }
            token = strtok_r(NULL, " ", &saveptr1);
        }
    }
    fclose(pFD);
    free(pModulePaths);
    free(pModules);
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateSystem(
    char          *pcCommand,
    char                *pcArgs,
    const char          *pcSearchStr,
    int32_t             lTimeoutMs,
    bool                *pStopCondition,
    char                *pResults,
    uint32_t             ulLength,
    int                 *pChildPid)
{
    NETAPP_RETCODE  tRetCode= NETAPP_FAILURE;
    int             pid     = 0;
    int             pfd[2]  = { 0, 0 };
    int             i;
    int             maxFDs;
    char            *pBuf = NULL;
    char            *argv[20];
    int             argc = 0;

    if (pcCommand == NULL)
    {
        NETAPP_ERR(("%s Null Pointer!", __FUNCTION__));
        return NETAPP_NULL_PTR;
    }

    NETAPP_MSG(("%s Entered, CMD[%s] ARGS[%s] Search String[%s]",
        __FUNCTION__, pcCommand, (pcArgs == NULL) ? "NULL" : pcArgs, (pcSearchStr == NULL) ? "NULL" : pcSearchStr));

    /* Make sure all open fds are set to be closed when exec() is called */
    maxFDs = sysconf(_SC_OPEN_MAX);
    if (maxFDs == -1)
    {
        maxFDs = 8192;
    }

    for (i = 3; i < maxFDs; ++i)
    {
        int res = fcntl(i, F_GETFD);
        if (res >= 0)
        {
            fcntl(i, F_SETFD, res | FD_CLOEXEC);
        }
    }


    if (pcSearchStr != NULL)
    {
        /* Create a pipe to capture output */
        if (pipe(pfd) == -1)
        {
            NETAPP_ERR(("%s Pipe failure!", __FUNCTION__));
            return (NETAPP_FAILURE);
        }
    }
    memset(argv, 0, sizeof(argv));

    argv[0] = pcCommand;

    /* Prepare the buffer for parsing the arguments */
    if (pcArgs != NULL)
    {
        pBuf = NetAppOSMemAlloc(strlen(pcArgs)+1);
        if (pBuf == NULL)
        {
            NETAPP_ERR(("%s(): Ran out of memory parameter!", __FUNCTION__));
            tRetCode = NETAPP_OUT_OF_MEMORY;
            goto err_out;
        }
        memcpy(pBuf, pcArgs, strlen(pcArgs)+1);

        if (NetAppPrivate_BuildArgvList(&argv[1], sizeof(argv)/sizeof(argv[0])-1, pBuf, &argc) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s(): Failed to Build argument list! CMD:[%s]", __FUNCTION__, pcArgs));
            tRetCode = EXIT_FAILURE;
            goto err_out;
        }
    }

    pid = vfork();

    /* Child Process */
    if (pid == 0)
    {
        struct sched_param  param;

        NETAPP_MSG(("%s, Child Process about to Call CMD:[%s] ARG[1]=%s", __FUNCTION__,
            pcCommand, (argv[1] == NULL) ? "NULL" : argv[1]));

        if (pcSearchStr != NULL)
        {
            close(pfd[0]);
            if (dup2(pfd[1], STDOUT_FILENO) < 0)
            {
                NETAPP_ERR(("%s Failed to duplicating STDOUT error: %s",
                    __FUNCTION__, strerror(errno)));
                _exit(EXIT_FAILURE);
            }
        }

        /* Change the priority of the process to very low */
        param.sched_priority = sched_get_priority_min(SCHED_OTHER);
        if (sched_setscheduler(0, SCHED_OTHER, &param) < 0)
        {
            NETAPP_ERR(("%s Failed to set scheduling priority error: %s",
                __FUNCTION__, strerror(errno)));
            _exit(EXIT_FAILURE);
        }

        if (execv(pcCommand, argv) < 0)
        {
            NETAPP_ERR(("%s Failed to execute CMD:[%s] error: %s",
                __FUNCTION__, pcCommand, strerror(errno)));
            _exit(EXIT_FAILURE);
        }
    }


    /* Parent Process */
    else
    {
        fd_set          readfds;
        struct timeval  tm;
        char            cLine[LINE_MAX];
        int             status  = 0;
        int             result  = 0;

        if (pcSearchStr != NULL)
        {
            FD_ZERO(&readfds);
            FD_SET(pfd[0], &readfds);
            close(pfd[1]);
        }
        memset(cLine, 0, sizeof(cLine));

        tm.tv_sec  = lTimeoutMs / 1000;
        tm.tv_usec = (lTimeoutMs - (tm.tv_sec * 1000)) * 1000;

        NETAPP_MSG(("%s, Parent: Waiting for CMD", __FUNCTION__));

        /* Save the PID so we can send the TERM later */
        if (pChildPid != NULL)
        {
            *pChildPid = pid;
        }

        /* Now lets makes sure the process is gone, if not then kill it */
        if (lTimeoutMs == NETAPP_WAIT_FOREVER)
        {
            while ((pStopCondition == NULL) ? 1 : !(*pStopCondition))
            {
                result = waitpid(pid, &status, WNOHANG | WUNTRACED);
                if (result != 0)
                {
                    break;
                }
                NetAppOSTaskDelayMsec(10);
            }
            if ( (pStopCondition != NULL) && (*pStopCondition) )
            {
                NETAPP_MSG(("%s, Received stop condition, canceling...", __FUNCTION__));
                tRetCode = NETAPP_CANCELED;
            }
        }
        else
        {
            for (i = 0; i < lTimeoutMs; i+=10)
            {
                result = waitpid(pid, &status, WNOHANG | WUNTRACED);
                if (result != 0)
                {
                    break;
                }
                NetAppOSTaskDelayMsec(10);
            }
            if (result == 0)
            {
                NETAPP_MSG(("%s, Parent:[%s] Timeout", __FUNCTION__, pcCommand));
                tRetCode = NETAPP_TIMEOUT;
            }
        }

        if (result < 0 )
        {
            NETAPP_ERR(("%s waitpid failed! error: %s", __FUNCTION__, strerror(errno)));
        }

        /* Return value of 0 means that it is still running and we need to kill it */
        else if (result == 0)
        {
            NETAPP_MSG(("%s, Parent: Need to send kill to child", __FUNCTION__));
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
        }

        /* Terminated, lets see the return code */
        else if ( WIFEXITED(status) && (WEXITSTATUS(status) == 0) )
        {
            NETAPP_MSG(("%s, Parent: Command Finished gracefully", __FUNCTION__));

            /* Parse Results */
            if (pcSearchStr != NULL)
            {
                FILE *fd = fdopen(pfd[0], "r");
                if (fd != NULL)
                {
                    while (fgets(cLine, sizeof(cLine), fd) != NULL)
                    {
                        char *pcResult = NULL;

                        NETAPP_MSG(("%s", cLine));
                        if ((pcResult = strstr(cLine, pcSearchStr)) != NULL)
                        {
                            pcResult += strlen(pcSearchStr);
                            NETAPP_MSG(("%s, Parent: CMD[%s] Success!", __FUNCTION__, pcCommand));
                            if ( (pResults != NULL) && ulLength)
                            {
                                if (pcResult != NULL)
                                {
                                    strncpy(pResults, pcResult, ulLength);
                                }
                                else
                                {
                                    memset(pResults, 0, ulLength);
                                }
                            }
                            tRetCode = NETAPP_SUCCESS;
                            break;
                        }
                    }
                    fclose(fd);
                }
                if (tRetCode != NETAPP_SUCCESS)
                {
                    tRetCode = NETAPP_NOT_FOUND;
                }
                close(pfd[0]);
            }
            else
            {
                NETAPP_MSG(("%s, Parent: CMD[%s] Success!", __FUNCTION__, pcCommand));
                tRetCode = NETAPP_SUCCESS;
            }
        }
    }

err_out:
    if (pBuf != NULL)
    {
        NetAppOSMemFree(pBuf);
    }
    NETAPP_MSG(("%s(): Leaving, Result=%d", __FUNCTION__, tRetCode));
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateSetInterfaceUp(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    bool                bUp)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    struct ifreq    ifr;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIFaceName, sizeof(ifr.ifr_name)-1);

    /*INTERFACE CONTROL (Up/Down) */
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFFLAGS, &ifr), tRetCode);

    ifr.ifr_flags = (bUp) ? (ifr.ifr_flags | IFF_UP | IFF_RUNNING) : (ifr.ifr_flags & ~(IFF_UP | IFF_RUNNING));
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCSIFFLAGS, &ifr), tRetCode);


    /* If Wired and setting interface up, check cable link state */
    if ( bUp && (strncmp(pIFaceName, NETAPP_IFACE_ETH, NETAPP_IFACE_ETH_LEN) == 0) )
    {
        NETAPP_IFACE    tIface = atoi(pIFaceName + NETAPP_IFACE_ETH_LEN);
        NetAppPrivate_P_UpdateLinkState(hNetAppPriv, tIface);
    }

err_out:
    return tRetCode;
}


NETAPP_LINK_STATE NetAppPrivateGetLinkState(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IFACE        tIface)
{
    NETAPP_LINK_STATE   tState = NETAPP_LINK_DOWN;

    if ( (tIface > NETAPP_IFACE_WIRED_MAX) || (hNetAppPriv == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_LINK_DOWN;
    }

    if (tIface == NETAPP_IFACE_WIRED_MAX)
    {
        int i = 0;

        for (i = 0; i < NETAPP_IFACE_WIRED_MAX; i++)
        {
            if (hNetAppPriv->tLinkState[tIface] != NETAPP_LINK_DOWN)
            {
                tState = hNetAppPriv->tLinkState[tIface];
                break;
            }
        }
    }
    else
    {
        tState = hNetAppPriv->tLinkState[tIface];
    }
    return tState;
}


NETAPP_RETCODE NetAppPrivateDNSLookup(
    NETAPP_PRIV_HANDLE hNetAppPriv,
    const char          *pcHostname)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    /* If we have to kill it then send a notification to the effect */
    if (hNetAppPriv->ulDNSLookupTaskId != 0)
    {
        NetAppOSTaskDelete(hNetAppPriv->ulDNSLookupTaskId);
        hNetAppPriv->ulDNSLookupTaskId = 0;

        if (hNetAppPriv->pDNSLookupHostName)
        {
            free(hNetAppPriv->pDNSLookupHostName);
        }
        NETAPP_MSG(("%s Forcefully killed DNSLookup Request", __FUNCTION__));
    }

    hNetAppPriv->pDNSLookupHostName = strdup(pcHostname);

    hNetAppPriv->ulDNSLookupTaskId = NetAppOSTaskSpawn("NetAppPrivate_P_DNSLookupTask",
        NETAPP_OS_PRIORITY_LOW, 64*1024, NetAppPrivate_P_DNSLookupTask, hNetAppPriv);

    tRetCode = ( (hNetAppPriv->ulDNSLookupTaskId != 0) && (hNetAppPriv->pDNSLookupHostName != NULL) ) ? NETAPP_SUCCESS : NETAPP_FAILURE;

    if ( (tRetCode != NETAPP_SUCCESS) && (hNetAppPriv->pDNSLookupHostName != NULL))
    {
        free(hNetAppPriv->pDNSLookupHostName);
        hNetAppPriv->pDNSLookupHostName = NULL;
        NETAPP_ERR(("%s(): Error spawning DNS Lookup!", __FUNCTION__));
    }
    return tRetCode;
}

NETAPP_RETCODE NetAppPrivatePing(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pcHostname,
    int32_t             lTimeoutMs)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    /* If we have to kill it then send a notification to the effect */
    if (hNetAppPriv->ulPingTaskId != 0)
    {
        hNetAppPriv->bStopPing = true;
        NetAppOSTaskJoin(hNetAppPriv->ulPingTaskId);
        hNetAppPriv->bStopPing = false;
        NetAppOSTaskDelete(hNetAppPriv->ulPingTaskId);
        hNetAppPriv->ulPingTaskId = 0;

        NETAPP_MSG(("%s Forcefully killed Ping Request", __FUNCTION__));
    }

    hNetAppPriv->pPingAddress = strdup(pcHostname);
    hNetAppPriv->lPingTimeout = lTimeoutMs;

    hNetAppPriv->ulPingTaskId = NetAppOSTaskSpawn("NetAppPrivate_P_PingTask",
        NETAPP_OS_PRIORITY_LOW, 64*1024, NetAppPrivate_P_PingTask, hNetAppPriv);
    if (hNetAppPriv->ulPingTaskId == 0)
    {
        if (hNetAppPriv->pPingAddress != NULL)
        {
            free(hNetAppPriv->pPingAddress);
            hNetAppPriv->pPingAddress = NULL;
        }
        tRetCode = NETAPP_FAILURE;
        NETAPP_ERR(("%s(): Failure to do Ping Request", __FUNCTION__));
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateGetIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    struct sockaddr_nl  addr;
    struct ifreq        ifr;
    struct sockaddr_in  *p;
    struct ether_addr*  ea = (struct ether_addr*)&ifr.ifr_hwaddr.sa_data;

    NETAPP_MSG(("%s(): Entered! Interface[%s]", __FUNCTION__, (pIFaceName != NULL) ? pIFaceName : "NULL"));
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIFaceName, sizeof(ifr.ifr_name)-1);


    /* MAC ADDRESS */
    NETAPP_PRIV_API_CHECK(NetAppPrivateGetMacAddress(hNetAppPriv, pIFaceName, ea), tRetCode);
    snprintf(pSettings->cMacAddress, sizeof(pSettings->cMacAddress),"%02X:%02X:%02X:%02X:%02X:%02X",
          ea->ether_addr_octet[0], ea->ether_addr_octet[1], ea->ether_addr_octet[2],
          ea->ether_addr_octet[3], ea->ether_addr_octet[4], ea->ether_addr_octet[5]);


    /* IP ADDRESS */
    p = (struct sockaddr_in *)&(ifr.ifr_addr);
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFADDR, &ifr), tRetCode);

    pSettings->tIpAddress = p->sin_addr.s_addr;


    /* NETMASK ADDRESS */
    p = (struct sockaddr_in *)&(ifr.ifr_netmask);
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFNETMASK, &ifr), tRetCode);
    pSettings->tSubnetMask = p->sin_addr.s_addr;


    /* GATEWAY ADDRESS */
    if ((tRetCode = NetAppPrivate_P_GetGateWay(&pSettings->tGateway)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s NetApp_P_GetGateWay failed", __FUNCTION__));
        goto err_out;
    }


    /* PRIMARY & SECONDARY DNS */
    if ((tRetCode = NetAppPrivateGetDns(hNetAppPriv, &pSettings->tPrimaryDNS, &pSettings->tSecondaryDNS)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s NetApp_P_SetDns failed", __FUNCTION__));
        goto err_out;
    }

    NETAPP_MSG(("****************************\n %s Net Settings:", pIFaceName));
    NETAPP_MSG(("MAC:      %s", pSettings->cMacAddress));
    NETAPP_MSG(("IP:       %s", NetAppNtoA(pSettings->tIpAddress)));
    NETAPP_MSG(("NETMASK:  %s", NetAppNtoA(pSettings->tSubnetMask)));
    NETAPP_MSG(("Gateway:  %s", NetAppNtoA(pSettings->tGateway)));
    NETAPP_MSG(("Pri. DNS: %s", NetAppNtoA(pSettings->tPrimaryDNS)));
    NETAPP_MSG(("Sec. DNS: %s", NetAppNtoA(pSettings->tSecondaryDNS)));
    NETAPP_MSG(("****************************"));

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateNtpSetDate(
    NETAPP_PRIV_HANDLE hNetAppPriv,
    uint32_t            ulPeriodMs)
{
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    NETAPP_IPV4_ADDR    curGateway = 0;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    NetAppPrivate_P_KillNtpDate(hNetAppPriv);

    hNetAppPriv->ulNTPPeriodMs = ulPeriodMs;
    hNetAppPriv->ulNtpTaskId = NetAppOSTaskSpawn("NetAppNTPTask",
        NETAPP_OS_PRIORITY_LOW, 64*1024, NetAppPrivate_P_NtpDateTask, hNetAppPriv);

    return (hNetAppPriv->ulNtpTaskId) ? NETAPP_SUCCESS : NETAPP_FAILURE;
}


NETAPP_RETCODE NetAppPrivateSetMacAddress(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    char                *pMacAddress)
{
    struct sockaddr_nl  addr;
    struct ifreq        ifr;
    struct sockaddr_in  *p;
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    struct ether_addr   *pEtherAddr = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIfaceName, sizeof(ifr.ifr_name)-1);

    p = (struct sockaddr_in *)&(ifr.ifr_hwaddr);
    p->sin_family = AF_INET;

    if ( (pMacAddress == NULL) || ((pEtherAddr = ether_aton(pMacAddress)) == NULL) )
    {
        NETAPP_ERR(("%s() Invalid MAC Address", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        goto err_out;
    }

    memcpy(&ifr.ifr_hwaddr.sa_data, pEtherAddr, sizeof(struct ether_addr));

    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCSIFHWADDR, &ifr), tRetCode);

err_out:
    return tRetCode;
}



bool NetAppPrivateIsIpAddressSet(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    NETAPP_IPV4_ADDR    *pCurIpAddr)
{
    struct ifreq        ifr;
    struct sockaddr_in  *p = (struct sockaddr_in *)&(ifr.ifr_addr);

    if ( (pIfaceName == NULL) || (hNetAppPriv == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return false;
    }

    //NETAPP_MSG(("%s(): Entered %s!", __FUNCTION__, pIfaceName));
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIfaceName, sizeof(ifr.ifr_name)-1);
    p->sin_family = AF_INET;

    if ( (ioctl(hNetAppPriv->lPacketSocket, SIOCGIFADDR, &ifr) >= 0) && (p->sin_addr.s_addr != 0) )
    {
        if (pCurIpAddr != NULL)
        {
            *pCurIpAddr = p->sin_addr.s_addr;
        }
        return true;
    }
    return false;
}


char* NetAppPrivatePrintLinkState(
    NETAPP_LINK_STATE   tState)
{
    switch(tState)
    {
    case NETAPP_LINK_DOWN:
        return "LINK DOWN";
    case NETAPP_LINK_UP:
        return "LINK UP";
    case NETAPP_LINK_ACQUIRING:
        return "ACQUIRING IP ADDRESS";
    default:;
    }
    return "NULL";
}


NETAPP_RETCODE NetAppPrivateSetNetworkSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    NETAPP_IFACE        tIface,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_RETCODE  tRetCode   = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if (NetAppDhcpcdStop(hNetAppPriv->hNetAppDhcpcd, pIfaceName) != NETAPP_SUCCESS)
    {
        NETAPP_MSG(("%s() Could not Kill DHCP for %s \n", __FUNCTION__, pIfaceName));
    }

    NETAPP_PRIV_API_CHECK(NetAppPrivateClearIPSettings(hNetAppPriv, pIfaceName), tRetCode);

    switch (tMode)
    {
    case NETAPP_IP_MODE_STATIC:
        tRetCode = NetAppPrivate_P_SetIPSettings(hNetAppPriv, pIfaceName, pSettings);
        break;

    case NETAPP_IP_MODE_DYNAMIC:
    case NETAPP_IP_MODE_AUTO_IP:
        NETAPP_PRIV_API_CHECK(NetAppDhcpcdStart(hNetAppPriv->hNetAppDhcpcd, tMode,
            pIfaceName, tIface, NULL, NULL, false), tRetCode);
        break;

    case NETAPP_IP_MODE_OFF:
        NETAPP_PRIV_API_CHECK(NetAppPrivateSetInterfaceUp(hNetAppPriv, pIfaceName, false), tRetCode);
        break;

    default:
        NETAPP_ERR(("%s(): Invalid IP Mode", __FUNCTION__));
        tRetCode = NETAPP_INVALID_PARAMETER;
        break;
    }


err_out:
    /* NOTE: This function should not be called holding onto the mutex */
    if ( (tMode == NETAPP_IP_MODE_STATIC) || (tMode == NETAPP_IP_MODE_OFF) )
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_SETSETTINGS, NULL, 0, 0, tRetCode, tIface);
    }
    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateSetWiredSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IP_MODE      tMode,
    NETAPP_IFACE        tIface,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_MSG_INFO tMessage;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&tMessage, 0, sizeof(NETAPP_MSG_INFO));

    tMessage.tMsgType       = NETAPP_MSG_SET_SETTINGS;
    tMessage.ulData0        = tMode;
    tMessage.tIface         = tIface;

    if (pSettings != NULL)
    {
        tMessage.tData.tSettings= *pSettings;
    }

    return NetAppOSMsgQSend(hNetAppPriv->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), NETAPP_WAIT_FOREVER);
}


NETAPP_RETCODE NetAppPrivateGetDefaultInterface(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    char                *pIfaceName)
{
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    struct rtmsg        *rtMsg;
    struct rtattr       *rtAttr;
    int                 rtLen;
    char                cMsgBuffer[1024];
    int                 bytes = 0;
    struct in_addr      destAddr;
    uint32_t            ulIndex = 0;
    char                cIface[NETAPP_IFACE_NAME_LEN+1];
    struct nlmsghdr     *p = (struct nlmsghdr *) cMsgBuffer;
    int                 tmpSocket = 0;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if (pIfaceName == NULL)
    {
        NETAPP_ERR(("%s Nl Pointer!", __FUNCTION__));
        return (NETAPP_NULL_PTR);
    }


    /* Initialize the buffer */
    memset(cMsgBuffer, 0, sizeof(cMsgBuffer));
    memset(cIface, 0, sizeof(cIface));

    if ((tmpSocket = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        NETAPP_ERR(("%s socket(PF_NETLINK) Error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }


    /* Fill the nlmsghdr to tell the Kernel we want the routing table */
    p->nlmsg_len    = NLMSG_LENGTH(sizeof(struct rtmsg));
    p->nlmsg_type   = RTM_GETROUTE;
    p->nlmsg_flags  = NLM_F_DUMP | NLM_F_REQUEST;
    p->nlmsg_seq    = 0;
    p->nlmsg_pid    = getpid();

    /* Send the request */
    if(send(tmpSocket, p, p->nlmsg_len, 0) < 0)
    {
        NETAPP_ERR(("%s Socket Error %s ", __FUNCTION__, strerror(errno)));
        goto err_out;
    }

    /* Receive response from the kernel */
    bytes = recv(tmpSocket, cMsgBuffer, sizeof(cMsgBuffer), 0);
    for(;NLMSG_OK(p, bytes);p=NLMSG_NEXT(p, bytes))
    {
        if (p->nlmsg_type == NLMSG_DONE)
        {
            break;
        }

        rtMsg = (struct rtmsg *)NLMSG_DATA(p);
        if( (rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN) )
        {
            continue;
        }

        destAddr.s_addr  = 0;

        rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
        rtLen = RTM_PAYLOAD(p);
        for(; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
        {
            switch(rtAttr->rta_type)
            {
            case RTA_DST:
                destAddr.s_addr = *(uint32_t *)RTA_DATA(rtAttr);
                break;
            case RTA_OIF:
                ulIndex = *(uint32_t *)RTA_DATA(rtAttr);
                break;
            default:
                break;
            }
        }

        /* See if it is the default route, that has the gateway information */
        if (strcmp((char *)inet_ntoa(destAddr), "0.0.0.0") == 0)
        {
            struct ifreq    ifr;

            memset(&ifr, 0, sizeof(ifr));
            ifr.ifr_ifindex = ulIndex;

            NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFNAME, &ifr), tRetCode);

            strncpy(pIfaceName, ifr.ifr_name, strlen(ifr.ifr_name));
            NETAPP_MSG(("%s(): default iface[%s]", __FUNCTION__, pIfaceName));

            tRetCode = NETAPP_SUCCESS;
            break;
        }
    }

err_out:
    if (tmpSocket > 0)
    {
        close(tmpSocket);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateClearIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName)
{
    struct ifreq        ifr;
    struct sockaddr_in  *p = (struct sockaddr_in *)&(ifr.ifr_addr);

    if (pIFaceName == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIFaceName, sizeof(ifr.ifr_name)-1);
    p->sin_family = AF_INET;
    if ( (ioctl(hNetAppPriv->lPacketSocket, SIOCGIFADDR, &ifr) >= 0) && (p->sin_addr.s_addr != 0) )
    {
        /* Set IP Address to 0 to kill all routes, this might fail
         * if there are no routes so we ignore the error */
        p->sin_addr.s_addr = 0;
        NETAPP_MSG(("%s(): Clearing IP Settings for %s", __FUNCTION__, pIFaceName));
        if (ioctl(hNetAppPriv->lPacketSocket, SIOCSIFADDR, &ifr) < 0)
        {
            NETAPP_MSG(("%s(): Not routes in the table", __FUNCTION__));
        }
    }
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppPrivateGetMacAddress(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    struct ether_addr   *pAddress)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    struct ifreq        ifr;
    struct sockaddr_in  *p;
    struct ether_addr*  ea = (struct ether_addr*)&ifr.ifr_hwaddr.sa_data;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if ( (pIfaceName == NULL) || (pAddress == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIfaceName, sizeof(ifr.ifr_name)-1);

    /* MAC ADDRESS */
    p = (struct sockaddr_in *)&(ifr.ifr_hwaddr);
    p->sin_family = AF_INET;
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFHWADDR, &ifr), tRetCode);

    memcpy(pAddress, ea, sizeof(struct ether_addr));

err_out:
    return tRetCode;
}


/* TODO: Generate a settings refresh */
NETAPP_RETCODE NetAppPrivateSetSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_SETTINGS     *pSettings)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);
    NETAPP_PRIV_HANDLE_CHECK(pSettings);

    hNetAppPriv->tSettings = *pSettings;

    // Do the internal callback
    NETAPP_DO_CALLBACK(NETAPP_CB_INVALID, NULL, 0, NETAPP_CB_PRIV_SETTINGS_REFRESH, NETAPP_SUCCESS, NETAPP_IFACE_MAX);
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppPrivateGetSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_SETTINGS     *pSettings)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);
    NETAPP_PRIV_HANDLE_CHECK(pSettings);

    *pSettings = hNetAppPriv->tSettings;

    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppPrivate_BuildArgvList(
    char                *argv[],
    int                 argv_sz,
    char                *pString,
    int                 *pargc)
{
    int     i = 0;

    if ( (pString == NULL) || (argv == NULL) || (pargc == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    while( (i < argv_sz) && (*pString != '\0') )
    {
        argv[i] = pString;
        while ( (*pString != '\0') && !isspace(*pString) )
        {
            pString++;
        }
        if (*pString == '\0')
        {
            i+=1;
            break;
        }
        while (isspace(*pString))
        {
            *pString = '\0';
            pString++;
        }
        i++;
    }

    *pargc = i;
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppPrivateBlockingPing(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pcHostname,
    int32_t             lTimeoutMs,
    bool                *bStopPing)
{
    char                cArgs[64];
    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);
    NETAPP_PRIV_HANDLE_CHECK(pcHostname);

    NETAPP_MSG(("%s Entered, pinging %s!", __FUNCTION__, pcHostname));

    snprintf(cArgs, sizeof(cArgs), "%s %s", NETAPP_PING_ARGS, pcHostname);

    return NetAppPrivateSystem(NETAPP_PING_CMD, cArgs, "time=", lTimeoutMs, bStopPing, NULL, 0, NULL);
}

NETAPP_RETCODE NetAppPrivateSetDns(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IPV4_ADDR    tPrimary,
    NETAPP_IPV4_ADDR    tSecondary)
{
    int             fd = 0;
    char            cBuffer[40];
    struct in_addr  addr;
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;
    int             i;
    char            *pResolvFile = NULL;
    struct stat     statbuf;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);
    memset(cBuffer, 0, sizeof(cBuffer));

    if (stat(NETAPP_DNS_FILE, &statbuf) == 0)
    {
        pResolvFile = NETAPP_DNS_FILE;
    }
    else
    {
        if (stat(NETAPP_DNS_FILE2, &statbuf) == 0)
        {
            pResolvFile = NETAPP_DNS_FILE2;
        }
        else
        {
            pResolvFile = NETAPP_DNS_FILE;
        }
    }

    fd = open(pResolvFile, (O_TRUNC | O_WRONLY | O_CREAT));
    if (fd < 0)
    {
        NETAPP_ERR(("%s Could not open or create %s, error %s",
                __FUNCTION__, pResolvFile, strerror(errno)));
        goto error;
    }

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            addr.s_addr = tPrimary;
        }
        else
        {
            addr.s_addr = tSecondary;
        }
        sprintf(cBuffer, "nameserver %s\n", inet_ntoa(addr));

        if (write(fd, cBuffer, strlen(cBuffer)) < 0)
        {
            NETAPP_ERR(("%s Could not write line [%s] to file [%s], error [%s]",
                    __FUNCTION__, cBuffer, pResolvFile, strerror(errno)));
            close(fd);
            goto error;
        }
        NETAPP_MSG(("%s: %s Gateway Address: [%s]",
            __FUNCTION__, (i==0) ? "Primary" : "Secondary",  inet_ntoa(addr)));
    }
    close(fd);

    /* Restart the Linux Resolver to reload the new name server settings */
    res_init();

    tRetCode = NETAPP_SUCCESS;

error:
    return tRetCode;
}


NETAPP_RETCODE NetAppPrivateGetDns(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IPV4_ADDR    *pPrimary,
    NETAPP_IPV4_ADDR    *pSecondary)
{
    FILE            *fd  = NULL;
    char            cLine[LINE_MAX];
    int             i = 0;
    char*           cSearchStr = "nameserver ";
    struct in_addr  addr;
    NETAPP_RETCODE  tRetCode = NETAPP_FAILURE;
    char            *pResolvFile = NULL;
    struct stat     statbuf;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if ( (pPrimary == NULL) || (pSecondary == NULL) )
    {
        NETAPP_ERR(("%s Nl Pointer!", __FUNCTION__));
        tRetCode = NETAPP_NULL_PTR;
        goto error;
    }

    memset(cLine, 0, sizeof(cLine));

    if (stat(NETAPP_DNS_FILE, &statbuf) == 0)
    {
        pResolvFile = NETAPP_DNS_FILE;
    }
    else
    {
        if (stat(NETAPP_DNS_FILE2, &statbuf) == 0)
        {
            pResolvFile = NETAPP_DNS_FILE2;
        }
        else
        {
            pResolvFile = NETAPP_DNS_FILE;
        }
    }

    if ((fd = fopen(pResolvFile, "r")) == NULL)
    {
        if (errno == ENOENT)
        {
            NETAPP_MSG(("%s File %s, does not exist", __FUNCTION__, NETAPP_DNS_FILE));
            *pPrimary = *pSecondary = 0;
            tRetCode = NETAPP_SUCCESS;
            goto error;
        }
        else
        {
            NETAPP_ERR(("%s Cod not open  %s, error %s",
                __FUNCTION__, pResolvFile, strerror(errno)));
            goto error;
        }
    }

    while(fgets(cLine, LINE_MAX, fd) != NULL)
    {
        char *pc = NULL;
        pc = strstr(cLine, cSearchStr);
        if (pc)
        {
            pc += strlen(cSearchStr);
            inet_aton(strtok(pc, "\n"), &addr);
            if (i==0)
            {
                *pPrimary = addr.s_addr;
                i++;
            }
            else
            {
                *pSecondary = addr.s_addr;
                break;
            }
        }
    }

    tRetCode = NETAPP_SUCCESS;

error:
    if (fd)
        fclose(fd);

    NETAPP_MSG(("%s Leaving!", __FUNCTION__));
    return tRetCode;
}



const char* NetAppPrivate_StrRetCode(
    NETAPP_RETCODE      tRetCode)
{
    const char *pRetCode = NULL;

    switch(tRetCode)
    {
    case NETAPP_SUCCESS:
        pRetCode = "NETAPP_SUCCESS";
        break;

    case NETAPP_FAILURE:
        pRetCode = "NETAPP_FAILURE";
        break;

    case NETAPP_INVALID_PARAMETER:
        pRetCode = "NETAPP_INVALID_PARAMETER";
        break;

    case NETAPP_NULL_PTR:
        pRetCode = "NETAPP_NULL_PTR";
        break;

    case NETAPP_OUT_OF_MEMORY:
        pRetCode = "NETAPP_OUT_OF_MEMORY";
        break;

    case NETAPP_NOT_IMPLEMENTED:
        pRetCode = "NETAPP_NOT_IMPLEMENTED";
        break;

    case NETAPP_NETWORK_UNREACHABLE:
        pRetCode = "NETAPP_NETWORK_UNREACHABLE";
        break;

    case NETAPP_SOCKET_ERROR:
        pRetCode = "NETAPP_SOCKET_ERROR";
        break;

    case NETAPP_TIMEOUT:
        pRetCode = "NETAPP_TIMEOUT";
        break;

    case NETAPP_DHCP_FAILURE:
        pRetCode = "NETAPP_DHCP_FAILURE";
        break;

    case NETAPP_HOST_NOT_FOUND:
        pRetCode = "NETAPP_HOST_NOT_FOUND";
        break;

    case NETAPP_CANCELED:
        pRetCode = "NETAPP_HOST_NOT_FOUND";
        break;

    case NETAPP_INCORRECT_PASSWORD:
        pRetCode = "NETAPP_INCORRECT_PASSWORD";
        break;

    case NETAPP_INVALID_PIN:
        pRetCode = "NETAPP_INVALID_PIN";
        break;
    case NETAPP_NOT_FOUND:
        pRetCode = "NETAPP_NOT_FOUND";
        break;

    case NETAPP_NOT_SUPPORTED:
        pRetCode = "NETAPP_NOT_SUPPORTED";
        break;

    case NETAPP_WPS_MULTIPLE_AP_FOUND:
        pRetCode = "NETAPP_WPS_MULTIPLE_AP_FOUND";
        break;

    case NETAPP_SCAN_EMPTY:
        pRetCode = "NETAPP_SCAN_EMPTY";
        break;

    case NETAPP_INVALID_STATE:
        pRetCode = "NETAPP_INVALID_STATE";
        break;

    case NETAPP_WPS_2_ERR_INCOMPATIBLE:
        pRetCode = "NETAPP_WPS_2_ERR_INCOMPATIBLE";
        break;

    default:
        break;
    }
    return pRetCode;
}


/*******************************************************************************
 *                       Static NetApp Private Functions
 ******************************************************************************/
static NETAPP_RETCODE NetAppPrivate_P_SetIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IP_SETTINGS  *pSettings)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    struct sockaddr_nl  addr;
    struct ifreq        ifr;
    struct sockaddr_in  *p;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIFaceName, sizeof(ifr.ifr_name)-1);

    p = (struct sockaddr_in *)&(ifr.ifr_addr);

    /* Set IP Address to 0 to kill all routes, this might fail
     * if there are no routes so we ignore the error */
    p->sin_family = AF_INET;
    p->sin_addr.s_addr = 0;
    if (ioctl(hNetAppPriv->lPacketSocket, SIOCSIFADDR, &ifr) < 0)
    {
        NETAPP_MSG(("%s(): Not routes in the table", __FUNCTION__));
    }

    /* IP ADDRESS */
    p->sin_family = AF_INET;
    p->sin_addr.s_addr = pSettings->tIpAddress;
    NETAPP_MSG(("%s %s IP Address:%s ", __FUNCTION__, pIFaceName, inet_ntoa(p->sin_addr)));
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCSIFADDR, &ifr), tRetCode);


    /* NETMASK ADDRESS */
    p = (struct sockaddr_in *)&(ifr.ifr_netmask);
    p->sin_family = AF_INET;
    p->sin_addr.s_addr = pSettings->tSubnetMask;
    NETAPP_MSG(("%s %s NETMASK  Address:%s ", __FUNCTION__, pIFaceName, inet_ntoa(p->sin_addr)));
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCSIFNETMASK, &ifr), tRetCode);

    if (pSettings->tGateway != 0)
    {
        /* GATEWAY ADDRESS */
        if ((tRetCode = NetAppPrivate_P_SetGateWay(hNetAppPriv, pIFaceName, pSettings->tGateway)) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s NetApp_P_SetGateWay failed", __FUNCTION__));
            goto err_out;
        }
    }

    if ( (pSettings->tPrimaryDNS != 0) || (pSettings->tSecondaryDNS != 0) )
    {
        /* PRIMARY & SECONDARY DNS */
        if ((tRetCode = NetAppPrivateSetDns(hNetAppPriv, pSettings->tPrimaryDNS, pSettings->tSecondaryDNS)) != NETAPP_SUCCESS)
        {
            NETAPP_ERR(("%s NetApp_P_SetDns failed", __FUNCTION__));
            goto err_out;
        }
    }
err_out:
    return tRetCode;
}


static NETAPP_RETCODE NetAppPrivate_P_SetGateWay(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IPV4_ADDR    tGateway)
{
    struct  rtentry     rtent;
    struct sockaddr_in  *pDest  = (struct sockaddr_in *)&rtent.rt_dst;
    struct sockaddr_in  *pGW    = (struct sockaddr_in *)&rtent.rt_gateway;
    struct sockaddr_in  *pMask  = (struct sockaddr_in *)&rtent.rt_genmask;
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    NETAPP_IPV4_ADDR    curGateway = 0;

    NETAPP_MSG(("%s Entered", __FUNCTION__));

    memset(&rtent,0,sizeof(struct rtentry));
    rtent.rt_metric         = 1;
    rtent.rt_window         = 0;
    pDest->sin_family       = pGW->sin_family = pMask->sin_family = AF_INET;
    pDest->sin_addr.s_addr  = tGateway;
    pGW->sin_addr.s_addr    = 0;
    pMask->sin_addr.s_addr  = 0xffffffff;
    rtent.rt_flags          = RTF_UP | RTF_HOST;
    rtent.rt_dev            = pIFaceName;

    /* Add route for subnet */
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCADDRT, &rtent), tRetCode);

    pDest->sin_addr.s_addr  = 0;
    pGW->sin_addr.s_addr    = tGateway;
    pMask->sin_addr.s_addr  = 0;
    rtent.rt_flags          = RTF_UP | RTF_GATEWAY;

    /* Add route for default gateway */
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCADDRT, &rtent), tRetCode);
    tRetCode = NETAPP_SUCCESS;

err_out:
    return (tRetCode);
}


static NETAPP_RETCODE NetAppPrivate_P_GetGateWay(
    NETAPP_IPV4_ADDR    *pGateway)
{
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    struct rtmsg        *rtMsg;
    struct rtattr       *rtAttr;
    int                 rtLen;
    char                cMsgBuffer[1024];
    int                 bytes = 0;
    struct in_addr      destAddr;
    struct in_addr      gwAddr;
    struct nlmsghdr     *p = (struct nlmsghdr *) cMsgBuffer;
    int                 tmpSocket = 0;

    if (pGateway == NULL)
    {
        NETAPP_ERR(("%s Nl Pointer!", __FUNCTION__));
        return (NETAPP_NULL_PTR);
    }


    /* Initialize the buffer */
    memset(cMsgBuffer, 0, sizeof(cMsgBuffer));

    if ((tmpSocket = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        NETAPP_ERR(("%s socket(PF_NETLINK) Error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto error;
    }


    /* Fill the nlmsghdr to tell the Kernel we want the routing table */
    p->nlmsg_len    = NLMSG_LENGTH(sizeof(struct rtmsg));
    p->nlmsg_type   = RTM_GETROUTE;
    p->nlmsg_flags  = NLM_F_DUMP | NLM_F_REQUEST;
    p->nlmsg_seq    = 0;
    p->nlmsg_pid    = getpid();

    /* Send the request */
    if(send(tmpSocket, p, p->nlmsg_len, 0) < 0)
    {
        NETAPP_ERR(("%s Socket Error %s ", __FUNCTION__, strerror(errno)));
        goto error;
    }

    /* Receive response from the kernel */
    bytes = recv(tmpSocket, cMsgBuffer, sizeof(cMsgBuffer), 0);
    for(;NLMSG_OK(p, bytes);p=NLMSG_NEXT(p, bytes))
    {
        if (p->nlmsg_type == NLMSG_DONE)
        {
            break;
        }

        rtMsg = (struct rtmsg *)NLMSG_DATA(p);
        if( (rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN) )
        {
            NETAPP_MSG(("%s Message not AF_INET or RT_TABLE_MAIN", __FUNCTION__));
            continue;
        }

        destAddr.s_addr = gwAddr.s_addr = 0;

        rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
        rtLen = RTM_PAYLOAD(p);
        for(; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
        {
            switch(rtAttr->rta_type)
            {
            case RTA_GATEWAY:
                gwAddr.s_addr = *(uint32_t *)RTA_DATA(rtAttr);
                break;
            case RTA_DST:
                destAddr.s_addr = *(uint32_t *)RTA_DATA(rtAttr);
                break;
            default:
                break;
            }
        }

        /* See if it is the default route, that has the gateway information */
        if (strcmp((char *)inet_ntoa(destAddr), "0.0.0.0") == 0)
        {
            *pGateway = gwAddr.s_addr;
            break;
        }
    }
    tRetCode = NETAPP_SUCCESS;

error:
    if (tmpSocket > 0)
    {
        close(tmpSocket);
    }
    return tRetCode;
}


static void NetAppPrivate_P_DNSLookupTask(void *pParam)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_FAILURE;
    struct hostent      *pHostInfo = NULL;
    NETAPP_PRIV_HANDLE   hNetAppPriv = (NETAPP_PRIV_HANDLE)pParam;
    struct addrinfo     *pAddrInfo = NULL;
    struct addrinfo     tHintsAddr;

    if (hNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    if (hNetAppPriv->pDNSLookupHostName == NULL)
    {
        NETAPP_MSG(("%s() No Hostname Set!", __FUNCTION__));
        return;
    }

    memset(&tHintsAddr, 0, sizeof(struct addrinfo));
    tHintsAddr.ai_family = AF_INET; /* Only want to show the IPv4 addresses */

    switch(getaddrinfo(hNetAppPriv->pDNSLookupHostName, NULL, &tHintsAddr, &pAddrInfo))
    {
    case 0:
    {
        /* We now might have more than one IP Address for the looked up Name, lets display them all */
        struct addrinfo     *pAddr = NULL;
        for (pAddr = pAddrInfo; pAddr != NULL; pAddr = pAddr->ai_next)
        {
            struct sockaddr_in *pInAddr = (struct sockaddr_in*)pAddr->ai_addr;
            NETAPP_MSG(("%s() DNSLookup[%s] = %s", __FUNCTION__, hNetAppPriv->pDNSLookupHostName, inet_ntoa(pInAddr->sin_addr)));
        }
        tRetCode = NETAPP_SUCCESS;
        freeaddrinfo(pAddrInfo);
        break;
    }
    case EAI_NONAME:
        NETAPP_MSG(("%s() Host %s not found!", __FUNCTION__, hNetAppPriv->pDNSLookupHostName));
        tRetCode = NETAPP_HOST_NOT_FOUND;
        break;
    default:
        NETAPP_ERR(("%s() getaddrinfo error!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        break;
    }

    if (hNetAppPriv->pDNSLookupHostName != NULL)
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_DNSLOOKUP, hNetAppPriv->pDNSLookupHostName,
            strlen(hNetAppPriv->pDNSLookupHostName) +1, 0, tRetCode, NETAPP_IFACE_MAX);
    }


    if (hNetAppPriv->pDNSLookupHostName)
    {
        free(hNetAppPriv->pDNSLookupHostName);
        hNetAppPriv->pDNSLookupHostName = NULL;
    }
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    NetAppOSTaskExit();
    return;
}


static void NetAppPrivate_P_PingTask(void *pParam)
{
    NETAPP_RETCODE      tRetCode   = NETAPP_FAILURE;
    NETAPP_PRIV_HANDLE  hNetAppPriv = (NETAPP_PRIV_HANDLE)pParam;

    if (hNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    if (hNetAppPriv->pPingAddress == NULL)
    {
        NETAPP_ERR(("%s Null Pointer!", __FUNCTION__));
        goto err_out;
    }

    NETAPP_PRIV_API_CHECK(NetAppPrivateBlockingPing(hNetAppPriv, hNetAppPriv->pPingAddress, hNetAppPriv->lPingTimeout, &hNetAppPriv->bStopPing), tRetCode);

    if (!hNetAppPriv->bStopPing)
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_PING, hNetAppPriv->pPingAddress,
            strlen(hNetAppPriv->pPingAddress)+1, 0, tRetCode, NETAPP_IFACE_MAX);
    }

err_out:

    if (hNetAppPriv->pPingAddress)
    {
        free(hNetAppPriv->pPingAddress);
        hNetAppPriv->pPingAddress = NULL;
    }
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    NetAppOSTaskExit();
    return;
}


/* Task to monitor for link changes and scan results */
static void NetAppPrivate_P_BackgroundTask(
    void              *pParam)
{
    int                 result = 0;
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    int                 lNetlinkSocket = 0;
    int                 bytes;
    char                replybuf[1024];
    struct sockaddr_nl  tSockAddr;
    NETAPP_LINK_STATE   tOldState;
    NETAPP_PRIV_HANDLE  hNetAppPriv = (NETAPP_PRIV_HANDLE)pParam;
    NETAPP_MSG_INFO     tMessage;
    NETAPP_IPV4_ADDR    tCurrIpAddr = 0;

    NETAPP_MSG(("%s() Entered!", __FUNCTION__));

    if (hNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    memset(&tSockAddr, 0, sizeof(struct sockaddr_nl));
    tSockAddr.nl_family = AF_NETLINK;
    tSockAddr.nl_groups = RTMGRP_LINK;
    tSockAddr.nl_pid = -1;
    /* Netlink socket to fetch routing information */
    if ((lNetlinkSocket = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        NETAPP_ERR(("%s socket(PF_NETLINK) Error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }

    if (bind(lNetlinkSocket, (struct sockaddr*)&tSockAddr, sizeof(tSockAddr)) < 0)
    {
        NETAPP_MSG(("%s, bind error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = NETAPP_SOCKET_ERROR;
        goto err_out;
    }


    for(;;)
    {
        if (NetAppOSMsgQReceive(hNetAppPriv->ulMsgQId, &tMessage, sizeof(NETAPP_MSG_INFO), 0) != NETAPP_SUCCESS)
        {
            fd_set              sset;
            struct timeval      tm;
            tm.tv_sec  = 0;
            tm.tv_usec = NETAPP_SCAN_WAIT_TIME * 1000;

            FD_ZERO(&sset);
            FD_SET(lNetlinkSocket, &sset);

            result =  select(lNetlinkSocket+1, &sset, NULL, NULL, &tm);
            if (result <= 0)
            {

// Do not enable this feature for now
#if 0
                /* Check for IP Address changes */
                if (hNetAppPriv->tWiredLinkState != NETAPP_LINK_DOWN)
                {
                    NETAPP_IPV4_ADDR  tNewIpAddr = 0;
                    NETAPP_LINK_STATE tOldState  = hNetAppPriv->tWiredLinkState;
                    NETAPP_LINK_STATE tNewState  = NetAppPrivateIsIpAddressSet(hNetAppPriv, NETAPP_IFACE_ETH0, &tNewIpAddr) ?
                        NETAPP_LINK_UP : NETAPP_LINK_ACQUIRING;

                    if ( (hNetAppPriv->tWiredLinkState != tNewState) ||
                         ( (tNewState == NETAPP_LINK_UP) && (tNewIpAddr != tCurrIpAddr) ) )
                    {
                        hNetAppPriv->tWiredLinkState = tNewState;
                        NETAPP_MSG(("%s(): WIRED: %s!", __FUNCTION__, NetAppPrivatePrintLinkState(tNewState)));
                        tCurrIpAddr = tNewIpAddr;

                        NETAPP_PRIV_API_CHECK(NetAppDhcpcdWaitForTaskCompelte(hNetAppPriv->hNetAppDhcpcd, NETAPP_IFACE_ETH0), tRetCode);
                        NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppPriv->tWiredLinkState, NETAPP_SUCCESS, NETAPP_IFACE_WIRED);
                    }
                }
#endif
                continue;
            }

            NETAPP_MSG(("%s() Received something!", __FUNCTION__));

            /* Check if we got something on the Netlink from the WIRED interface */
            if (FD_ISSET(lNetlinkSocket, &sset))
            {
                struct nlmsghdr *p = (struct nlmsghdr *) replybuf;

                bytes = recv(lNetlinkSocket, &replybuf, sizeof(replybuf), MSG_DONTWAIT);

                for (; NLMSG_OK(p, bytes); p = NLMSG_NEXT(p, bytes))
                {
                    if ( (p->nlmsg_type == RTM_NEWLINK) || (p->nlmsg_type == RTM_DELLINK) )
                    {
                        char                cIface[NETAPP_IFACE_NAME_LEN+1];
                        struct ifinfomsg    *pinfomsg = (struct ifinfomsg *)NLMSG_DATA(p);

                        if_indextoname(pinfomsg->ifi_index, cIface);

                        NETAPP_MSG(("%s() NETLINK: [%s] Link %s!", __FUNCTION__, cIface, (p->nlmsg_type == RTM_NEWLINK) ? "UP" : "DOWN"));

                        /* Got a notification from an interface were monitoring! */
                        if (strncmp(cIface, NETAPP_IFACE_ETH, strlen(NETAPP_IFACE_ETH)) == 0)
                        {
                            NETAPP_IFACE tIface = atoi(cIface + NETAPP_IFACE_ETH_LEN);
                            tOldState = hNetAppPriv->tLinkState[tIface];

                            /* Fetch the current Link tRetCode */
                            if (NetAppPrivate_P_UpdateLinkState(hNetAppPriv, tIface) == NETAPP_SUCCESS)
                            {
                                if (hNetAppPriv->tLinkState[tIface] != tOldState)
                                {
                                    NETAPP_MSG(("%s(): WIRED[%s]: %s!", __FUNCTION__, NETAPP_WIRED_IFACE_NAME[tIface],
                                        NetAppPrivatePrintLinkState(hNetAppPriv->tLinkState[tIface])));
                                    NETAPP_DO_CALLBACK(NETAPP_CB_LINK, NULL, 0, hNetAppPriv->tLinkState[tIface], NETAPP_SUCCESS, tIface);
                                }
                            }
                        }
                        /* Got Netlink Message from another interface, assuming WiFi */
                        else
                        {
                            NETAPP_HOTPLUG_DEVICE_INFO tInfo;
                            memset(&tInfo, 0, sizeof(NETAPP_HOTPLUG_DEVICE_INFO));
                            tInfo.tAction   = (p->nlmsg_type == RTM_NEWLINK) ? NETAPP_HOTPLUG_ADD : NETAPP_HOTPLUG_REMOVE;
                            tInfo.tType     = NETAPP_HOTPLUG_DEVICE_WIFI;

                            NETAPP_DO_CALLBACK(NETAPP_CB_HOTPLUG, &tInfo, sizeof(tInfo), 0, NETAPP_SUCCESS, NETAPP_IFACE_WIRELESS);
                        }
                        

                        break;
                    }
                }
            }

            continue;
        }

        NETAPP_MSG(("*********************************************"));
        /* Parse the received request */
        switch(tMessage.tMsgType)
        {
        case NETAPP_MSG_SET_SETTINGS:
        {
            NETAPP_MSG(("WIRED Task: Received NETAPP_MSG_SET_SETTINGS!"));

            if (NetAppPrivateSetNetworkSettings(hNetAppPriv, NETAPP_WIRED_IFACE_NAME[tMessage.tIface],
                tMessage.tIface, tMessage.ulData0, &tMessage.tData.tSettings) == NETAPP_SUCCESS)
            {
                switch (tMessage.ulData0)
                {
                case NETAPP_IP_MODE_OFF:
                    hNetAppPriv->tLinkState[tMessage.tIface] = NETAPP_LINK_DOWN;
                    break;

                case NETAPP_IP_MODE_STATIC:
                    hNetAppPriv->tLinkState[tMessage.tIface] = NETAPP_LINK_UP;
                    break;

                default:
                    hNetAppPriv->tLinkState[tMessage.tIface] = NETAPP_LINK_ACQUIRING;
                    break;
                }
            }
            break;
        }


        default:
            NETAPP_MSG(("%s(): Leaving the task now!", __FUNCTION__));
            goto err_out;
        }
        NETAPP_MSG(("WIRED Task(): Finished Processing Message"));
        NETAPP_MSG(("*********************************************"));
    }

err_out:
    if (lNetlinkSocket > 0)
    {
        close(lNetlinkSocket);
    }
    NETAPP_MSG(("%s() Leaving Result:%d", __FUNCTION__, tRetCode));
    return;
}


static void NetAppPrivate_P_KillNtpDate(
    NETAPP_PRIV_HANDLE  hNetAppPriv)
{
    if ( (hNetAppPriv != NULL) && (hNetAppPriv->ulNtpTaskId != 0) )
    {
        hNetAppPriv->bStopNtpTask = true;
        NetAppOSTaskJoin(hNetAppPriv->ulNtpTaskId);
        hNetAppPriv->bStopNtpTask = false;
        NetAppOSTaskDelete(hNetAppPriv->ulNtpTaskId);
        hNetAppPriv->ulNtpTaskId = 0;
    }
}


static void NetAppPrivate_P_NtpDateTask(void *pParam)
{
    NETAPP_PRIV_HANDLE  hNetAppPriv = (NETAPP_PRIV_HANDLE)pParam;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    struct stat         statbuf;
    char                *pCommand = NULL;
    char                *pArgs = NULL;
    char                cNtpServers[512];

    NETAPP_MSG(("%s() Entered!", __FUNCTION__));

    if (hNetAppPriv == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    NETAPP_MSG(("%s Entered!", __FUNCTION__));

    /* System contains ntpdate */
    if (stat(NETAPP_NTPDATE_CMD, &statbuf) == 0)
    {
        FILE                *pNtpFile = NULL;
        char                cLine[LINE_MAX];
        uint32_t            count = 0;

        pNtpFile = fopen(NETAPP_NTP_CONFIG_FILE, "r");
        if (pNtpFile == NULL)
        {
            NETAPP_ERR(("%s Could not find file %s !", __FUNCTION__, NETAPP_NTP_CONFIG_FILE));
            return;
        }

        memset(cLine, 0, sizeof(cLine));
        snprintf(cNtpServers, sizeof(cNtpServers), "-b ");

        count = strlen(cNtpServers);

        do
        {
            char *pString;
            if (fgets(cLine, sizeof(cLine), pNtpFile) == NULL)
            {
                NETAPP_MSG(("%s No more servers in the file %s !", __FUNCTION__, NETAPP_NTP_CONFIG_FILE));
                break;
            }

            pString = strtok(cLine, "\n");
            if ((count + strlen(pString)) < sizeof(cNtpServers))
            {
                strncat(cNtpServers, cLine, strlen(pString));
                count += strlen(pString);

                if ( (count+1) < sizeof(cNtpServers))
                {
                    strcat(cNtpServers, " ");
                    count++;
                }
                NETAPP_MSG(("%s Added [%s] to server list !", __FUNCTION__, pString));
            }

        } while( (cLine[0] != '\0') && (count < sizeof(cNtpServers)) );

        pCommand    = NETAPP_NTPDATE_CMD;
        pArgs       = cNtpServers;
        fclose(pNtpFile);
    }
    /* assume ntpd */
    else
    {
        pCommand    = NETAPP_NTPDATE2_CMD;
        pArgs       = NETAPP_NTPDATE2_ARGS;
    }

    while (!hNetAppPriv->bStopNtpTask)
    {
        int i;
        tRetCode = NetAppPrivateSystem(pCommand, pArgs, NULL, NETAPP_WAIT_FOREVER, &hNetAppPriv->bStopNtpTask, NULL, 0, NULL);

        if (tRetCode != NETAPP_CANCELED)
        {
            NETAPP_DO_CALLBACK(NETAPP_CB_NTPDATE, NULL, 0, 0, tRetCode, NETAPP_IFACE_MAX);
        }

        /* Little loop to be idle but check for the stop condition */
        for (i = 0; (i*NETAPP_TICK_MS) < hNetAppPriv->ulNTPPeriodMs; i++)
        {
            NetAppOSTaskDelayMsec(NETAPP_TICK_MS);
            if (hNetAppPriv->bStopNtpTask)
                break;
        }
    }

    NetAppOSTaskExit();
    return;
}


static NETAPP_RETCODE NetAppPrivate_P_UpdateLinkState(
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_IFACE            tIface)
{
    struct ifreq            ifr;
    NETAPP_RETCODE          tRetCode    = NETAPP_SUCCESS;
    NETAPP_LINK_STATE       tLinkState  = NETAPP_LINK_DOWN;
    struct mii_ioctl_data   *pMIIData   = (struct mii_ioctl_data*)&ifr.ifr_data;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppPriv);

    if (tIface > NETAPP_IFACE_WIRED_MAX)
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, NETAPP_WIRED_IFACE_NAME[tIface], sizeof(ifr.ifr_name)-1);

    if (ioctl(hNetAppPriv->lPacketSocket, SIOCGMIIPHY, &ifr) != NETAPP_SUCCESS)
    {
        NETAPP_MSG(("%s(): SIOCGMIIPHY failed for %s", __FUNCTION__, ifr.ifr_name));
        return NETAPP_FAILURE;
    }

    /* Read MII state */
    pMIIData->reg_num = MII_BMSR;
    NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGMIIREG, &ifr), tRetCode);

    tLinkState = (pMIIData->val_out & BMSR_LSTATUS) ? NETAPP_LINK_UP : NETAPP_LINK_DOWN;


    /* Check if interface is Off, if so NETAPP_LINK_DOWN */
    if (tLinkState == NETAPP_LINK_UP)
    {
        NETAPP_PRIV_IOCTL_CHECK(ioctl(hNetAppPriv->lPacketSocket, SIOCGIFFLAGS, &ifr), tRetCode);
        tLinkState = (ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) ? NETAPP_LINK_UP : NETAPP_LINK_DOWN;
    }


    /* If Link is up and running, check if IP is set */
    if (tLinkState == NETAPP_LINK_UP)
    {
        tLinkState = NetAppPrivateIsIpAddressSet(hNetAppPriv, NETAPP_WIRED_IFACE_NAME[tIface], NULL) ? NETAPP_LINK_UP : NETAPP_LINK_ACQUIRING;
    }

    hNetAppPriv->tLinkState[tIface] = tLinkState;
err_out:
    return tRetCode;
}
