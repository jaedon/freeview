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
 * $brcm_Workfile: netapp_priv.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_priv.h $
 *
 * 3   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 8   1/5/12 1:21p steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 7   12/2/11 3:04p steven
 * SW7425-1404: Fixing NTP for STB
 *
 * 6   11/24/11 5:14p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/4   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * SW7425-1404/1   10/7/11 5:33p steven
 * SW7425-1404: initial pass fo BT support
 *
 * 5   9/29/11 2:12p steven
 * SWBLURAY-27464: Add support to periodic NTP updates
 *
 * 4   9/15/11 11:22a steven
 * SWBLURAY-27357: Support for /etc/resolv.conf
 *
 * 3   8/26/11 4:02p steven
 * SWBLURAY-27043: WPS 2.0 Support
 *
 * 2   7/28/11 2:14p steven
 * SW7425-749: Initial WoWL and P2P development checkin
 *
 * dev_main_SW7425-749/2   6/28/11 2:24p steven
 * Check in initial P2P support
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
 * All the internal workings of NetApp that is included by all NetApp modules.
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#ifndef NETAPP_PRIVATE_H
#define NETAPP_PRIVATE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/route.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sched.h>
#include <netinet/ether.h>
#include <resolv.h>
#undef __USE_MISC
#include <net/if.h>
#include "netapp.h"
#include "netapp_os.h"
#include "netapp_dhcpcd.h"

#define NETAPP_ERR_WRAPPER(fmt, args...) \
    do \
    { \
        NetAppPrivateErrPrint("NetApp: "fmt, ##args); \
    } while (0)

#define NETAPP_ERR(x) NETAPP_ERR_WRAPPER x

#define NETAPP_DEBUG

#ifdef NETAPP_DEBUG

#define NETAPP_MSG_WRAPPER(fmt, args...) \
    do \
    { \
        NetAppPrivatePrint("NetApp: "fmt, ##args); \
    } while (0)

#define NETAPP_MSG(x) NETAPP_MSG_WRAPPER x

#else
#define NETAPP_MSG(a)
#endif

#define NETAPP_IFACE_LO                 "lo"
#define NETAPP_IFACE_ETH                "eth"
#define NETAPP_IFACE_ETH_LEN            3

#define NETAPP_NTP_CONFIG_FILE          "/etc/ntp/step-tickers"

#ifndef NETAPP_DNS_FILE
#define NETAPP_DNS_FILE                 "/etc/config/resolv.conf"
#define NETAPP_DNS_FILE2                "/etc/resolv.conf"
#endif

#define NETAPP_DEFAULT_DEVICE_NAME      "BRCM Client"

#define NETAPP_DHCPCD_CMD               "/sbin/dhcpcd"
#define NETAPP_UDHCPC_CMD               "/sbin/udhcpc"
#define NETAPP_NTPDATE_CMD              "/usr/sbin/ntpdate"
#define NETAPP_NTPDATE2_CMD             "/sbin/ntpd"
#define NETAPP_NTPDATE2_ARGS            "-n -q -p pool.ntp.org"
#define NETAPP_PING_CMD                 "/bin/ping"
#define NETAPP_PING_ARGS                "-c 1"

#define NETAPP_DHCPCD_CONFIG_PATH       "/etc/config/dhcpcd"

#define NETAPP_INSMOD_CMD               "/sbin/insmod"

#define NETAPP_BWL_INIT_RETRY           3
#define NETAPP_KILL_DHCPCD_LOOP_CNT     1000
#define NETAPP_KILL_DHCPCD_WAIT_MS      2
#define NETAPP_KILL_DHCPCD_TIMEOUT      (NETAPP_KILL_DHCPCD_LOOP_CNT * NETAPP_KILL_DHCPCD_WAIT_MS)
#define NETAPP_ICMP_DEF_DATA_LEN        56
#define NETAPP_DHCPCD_WAIT_TIME         999999
#define NETAPP_SCAN_WAIT_TIME           200
#define NETAPP_TICK_MS                  50

/* DHCPCD 5.x Settings */
#define NETAPP_DHCPCD5_CONFIG_PATH      "/var/run"
#define NETAPP_DHCPCD5_WAIT_TIME        180 /* 3 minutes: to meet DLNA Specification 4.1.1 */

#define NETAPP_PROC_NET_DEV             "/proc/net/dev"

typedef struct sNETAPP_PRIV *NETAPP_PRIV_HANDLE;

#define NETAPP_UNUSED(a) (void)a

#define NETAPP_DO_CALLBACK(cb, pData, size, data0, tRetCode, iface)             \
{                                                                               \
    NETAPP_MSG(("%s::%s()[%d]: CALLBACK:%s Status=%s", __FILE__, __FUNCTION__,  \
        __LINE__, #cb, NetAppPrivate_StrRetCode(tRetCode)));                    \
    NetAppPrivateDoCallback(cb, pData, size, data0, tRetCode, iface);           \
}

/* Internal callbacks not to be sent outside of NetApp */
typedef enum {
    NETAPP_CB_PRIV_SETTINGS_REFRESH,
} NETAPP_CB_PRIV_TYPE;


static const char *NETAPP_WIRED_IFACE_NAME[] =
{
     "eth0",
     "eth1",
     "eth2",
     "eth3",
     "eth4",
     "eth5",
     "br0",
 };

#define NETAPP_PRIV_API_CHECK(api, tRetCode)                                        \
{                                                                                   \
    tRetCode = api;                                                                 \
    if (tRetCode != NETAPP_SUCCESS)                                                 \
    {                                                                               \
        NETAPP_ERR(("%s(): [%s] Failed ", __FUNCTION__, #api));                     \
        goto err_out;                                                               \
    }                                                                               \
}

#define NETAPP_PRIV_HANDLE_CHECK(handle)                                            \
{                                                                                   \
    if (handle == NULL)                                                             \
    {                                                                               \
        NETAPP_ERR(("%s() -- Invalid Parameter!", __FUNCTION__));                   \
        return NETAPP_INVALID_PARAMETER;                                            \
    }                                                                               \
}

#define NETAPP_PRIV_IOCTL_CHECK(ioctl, tRetCode)                                    \
{                                                                                   \
    if (ioctl < 0)                                                                  \
    {                                                                               \
        NETAPP_ERR(("%s(): [%s] Failed error:%s", __FUNCTION__, #ioctl, strerror(errno)));\
        tRetCode = NETAPP_FAILURE;                                                  \
        goto err_out;                                                               \
    }                                                                               \
}


#define NETAPP_PRIV_SEM_GIVE(sem)                                                   \
{                                                                                   \
    if (NetAppOSSemGive(sem) != NETAPP_SUCCESS)                                     \
    {                                                                               \
        NETAPP_ERR(("%s() SemGive Failed! Line:%d", __FUNCTION__, __LINE__));       \
    }                                                                               \
}


#define NETAPP_PRIV_SEM_TAKE(sem, tRetCode)                                         \
{                                                                                   \
    if (NetAppOSSemTake(sem) != NETAPP_SUCCESS)                                     \
    {                                                                               \
        NETAPP_ERR(("%s() SemTake Failed! Line:%d", __FUNCTION__, __LINE__));       \
        tRetCode = NETAPP_FAILURE;                                                  \
        goto err_out;                                                               \
    }                                                                               \
}

static const NETAPP_HW_ADDR NETAPP_NULL_HW_ADDR = { 0, 0, 0, 0, 0, 0 };

typedef enum
{
    NETAPP_MSG_INVALID,
    NETAPP_MSG_CONNECT,
    NETAPP_MSG_DISCONNECT,
    NETAPP_MSG_SCAN,
    NETAPP_MSG_FETCH_APINFO,
    NETAPP_MSG_SET_SETTINGS,
    NETAPP_MSG_P2P,
    NETAPP_MSG_DIE,
} NETAPP_MSG_TYPE;


typedef enum
{
    NETAPP_CONNECT_WPS_PB,
    NETAPP_CONNECT_WPS_PIN_REGISTRAR,
    NETAPP_CONNECT_WPS_PIN_ENROLLEE,
    NETAPP_CONNECT_NORMAL,
    NETAPP_CONNECT_INVITE_ACCEPT,
    NETAPP_CONNECT_INVITE_REJECT,
    NETAPP_CONNECT_P2P,
}NETAPP_CONNECT_TYPE;


typedef struct
{
    NETAPP_MSG_TYPE     tMsgType;
    union
    {
        NETAPP_WIFI_AP_INFO         tApInfo;
        NETAPP_IP_SETTINGS          tSettings;
        NETAPP_P2P_DISCOVER_PARAMS  tDiscParams;
    } tData;
    NETAPP_CONNECT_TYPE tConnectType;
    NETAPP_IFACE        tIface;
    uint32_t            ulData0;
} NETAPP_MSG_INFO;

/*******************************************************************************
 *                 Public (but Internal to NetApp) API's
 ******************************************************************************/
NETAPP_RETCODE NetAppPrivateInit(
    NETAPP_PRIV_HANDLE      *phNetAppPriv,
    NETAPP_DHCPCD_HANDLE    hNetAppDhcpcd,
    NETAPP_SETTINGS     *pSettings);


NETAPP_RETCODE NetAppPrivateDeInit(
    NETAPP_PRIV_HANDLE hNetAppPriv);

void NetAppPrivatePrint(const char *fmt, ...);
void NetAppPrivateErrPrint(const char *fmt, ...);

NETAPP_RETCODE NetAppPrivateSetInterfaceUp(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    bool                bUp);

NETAPP_RETCODE NetAppPrivateSystem(
    char                *pcCommand,
    char                *pcArgs,
    const char          *pcSearchStr,
    int32_t             lTimeoutMs,
    bool                *pStopCondition,
    char                *pResults,
    uint32_t            ulLength,
    int                 *pChildPid);


NETAPP_RETCODE NetAppPrivateInsmod(
    char                *pcName,
    char                *pcFullPath,
    char                *pOptions);


void NetAppPrivateDoCallback(
    NETAPP_CB_TYPE      tCbType,
    const void          *pvBuffer,
    uint32_t            ulDataLength,
    uint32_t            ulData0,
    NETAPP_RETCODE      tResult,
    NETAPP_IFACE        tIFace);


NETAPP_LINK_STATE NetAppPrivateGetLinkState(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IFACE        tIface);


NETAPP_RETCODE NetAppPrivateDNSLookup(
    NETAPP_PRIV_HANDLE hNetAppPriv,
    const char          *pcHostname);

NETAPP_RETCODE NetAppPrivatePing(
    NETAPP_PRIV_HANDLE hNetAppPriv,
    const char          *pcHostname,
    int32_t             lTimeoutMs);


NETAPP_RETCODE NetAppPrivateBlockingPing(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pcHostname,
    int32_t             lTimeoutMs,
    bool                *bStopPing);

NETAPP_RETCODE NetAppPrivateGetIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName,
    NETAPP_IP_SETTINGS  *pSettings);


NETAPP_RETCODE NetAppPrivateSetNetworkSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    NETAPP_IFACE        tIface,
    NETAPP_IP_MODE      tMode,
    NETAPP_IP_SETTINGS  *pSettings);


NETAPP_RETCODE NetAppPrivateSetDns(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IPV4_ADDR    tPrimary,
    NETAPP_IPV4_ADDR    tSecondary);


NETAPP_RETCODE NetAppPrivateGetDns(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IPV4_ADDR    *pPrimary,
    NETAPP_IPV4_ADDR    *pSecondary);


NETAPP_RETCODE NetAppPrivateSetWiredSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_IP_MODE      tMode,
    NETAPP_IFACE        tIface,
    NETAPP_IP_SETTINGS  *pSettings);


NETAPP_RETCODE NetAppPrivateNtpSetDate(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    uint32_t            ulPeriodMs);


NETAPP_RETCODE NetAppPrivateSetMacAddress(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    char                *pMacAddress);


NETAPP_RETCODE NetAppPrivateGetMacAddress(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    struct ether_addr   *pAddress);


bool NetAppPrivateIsIpAddressSet(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIfaceName,
    NETAPP_IPV4_ADDR    *pCurIpAddr);


char* NetAppPrivatePrintLinkState(
    NETAPP_LINK_STATE   tState);


NETAPP_RETCODE NetAppPrivateGetDefaultInterface(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    char                *pIfaceName);


NETAPP_RETCODE NetAppPrivateClearIPSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    const char          *pIFaceName);


NETAPP_RETCODE NetAppPrivateSetSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_SETTINGS     *Settings);


NETAPP_RETCODE NetAppPrivateGetSettings(
    NETAPP_PRIV_HANDLE  hNetAppPriv,
    NETAPP_SETTINGS     *pSettings);


NETAPP_RETCODE NetAppPrivate_BuildArgvList(
    char                *argv[],
    int                 argv_sz,
    char                *pString,
    int                 *pargc);


const char* NetAppPrivate_StrRetCode(
    NETAPP_RETCODE      tRetCode);

#endif //NETAPP_PRIVATE_H
