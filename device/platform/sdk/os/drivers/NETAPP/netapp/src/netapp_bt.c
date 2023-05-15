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
 * $brcm_Workfile: netapp_bt.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_bt.c $
 *
 * 4   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 9   1/7/12 7:08p steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 8   1/6/12 9:15a steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 7   1/5/12 1:21p steven
 * SW7429-38: NetApp Enhancements for CES
 *
 * 6   12/13/11 11:52a steven
 * SW7231-513: Fix BT reconnection that was not working, missing
 *  discoverable flag
 *
 * 5   12/12/11 11:15p steven
 * SW7231-513: Various fixes
 *
 * 4   12/9/11 4:44p steven
 * SW7231-513: USB Hotplug Support in NetApp
 *
 * SW7231-513/1   12/9/11 3:36p steven
 * SW7231-513: Finished working hotplug detection
 *
 * 2   12/1/11 8:56p steven
 * SW7425-1404: Moved kernel modules and fixes to BT support
 *
 * 1   11/24/11 5:13p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/3   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 ***************************************************************************/
/**
 * @brief Internal NetApp Bluetooth API
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
#include "netapp_list.h"
#include "netapp_bt.h"
#include "netapp_bt_hid.h"
#include "netapp_bt_av.h"
#include "bsa_api.h"
#include "bthid.h"

#define NETAPP_BLUETOOTH_CONNECTION_TIMEOUT     2000 /* 2 seconds */
#define NETAPP_BLUETOOTH_RETRY_DELAY            200 /* 2 seconds */


typedef struct sNETAPP_BT_List
{
    NETAPP_LIST_NODE   tNode;
    NETAPP_BT_DEV_INFO tDevInfo;
} NETAPP_BT_LIST;


struct sNETAPP_BT
{
    uint32_t                ulBsaServerId;
    bool                    bShutdown;
    NETAPP_PRIV_HANDLE      hNetAppPriv;
    uint32_t                ulSem;
    NETAPP_LIST_HANDLE      hDiscoveryList;
    NETAPP_BT_SETTINGS      tSettings;
    NETAPP_BT_HID_HANDLE    hNetAppBTHid;
    NETAPP_BT_AV_HANDLE     hNetAppBTAv;
    bool                    bEnabled;
    NETAPP_SETTINGS_HANDLE  hNetAppSettings;
    bool                    bDiscovering;
};


/* Unfortunately the BSA stack does not allow parameters to be passed to the
 * callback so I have no choice but to define this static variable :-(
 */
static NETAPP_BT_HANDLE hStaticNetAppBT = NULL;

static void NetAppBluetooth_P_ServerTask(
    void                    *pParam);


static NETAPP_RETCODE NetAppBluetooth_P_SetSecurity(
    bool                    bAutoPair);


static NETAPP_RETCODE NetAppBluetooth_P_Configure(
    NETAPP_BT_HANDLE        hNetAppBT);


static void NetAppBluetooth_P_BsaMgrCallback(
    tBSA_MGT_EVT            event,
    tBSA_MGT_MSG            *pBsaDevInfo);


static void NetAppBluetooth_P_BsaDiscoveryCallback(
    tBSA_DISC_EVT           event,
    tBSA_DISC_MSG           *pBsaDevInfo);


static void NetAppBluetooth_P_BsaSecurityCallback(
    tBSA_SEC_EVT            event,
    tBSA_SEC_MSG            *pBsaDevInfo);


static uint32_t NetAppBluetooth_P_BsatoNetAppServices(
    uint32_t                ulBsaServices);


NETAPP_RETCODE NetAppBluetoothPrivateSecAddDev(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo);


NETAPP_RETCODE NetAppBluetoothPrivateInit(
    NETAPP_BT_HANDLE        *phNetAppBT,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_BT_HANDLE        hNetAppBT = NULL;
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    unsigned int            i;
    struct stat             statbuf;

    NETAPP_PRIV_HANDLE_CHECK(phNetAppBT);

    hNetAppBT = (NETAPP_BT_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_BT));
    if (hNetAppBT == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppBT, 0, sizeof(struct sNETAPP_BT));

    hNetAppBT->hNetAppPriv      = hNetAppPriv;
    hNetAppBT->hNetAppSettings  = hNetAppSettings;

    hNetAppBT->ulSem = NetAppOSSemBCreate();
    if (hNetAppBT->ulSem == 0)
    {
        NETAPP_ERR(("%s(): -- Cannot create sem!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    if (NetAppListCreate(&hNetAppBT->hDiscoveryList, sizeof(NETAPP_BT_LIST)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    /* Create all the devices for HID if they don't already exist */
    if (stat(NETAPP_LINUX_INPUT_PATH, &statbuf) != 0)
    {
        mkdir(NETAPP_LINUX_INPUT_PATH, 700);
    }

    for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
    {
        char cBuf[128];
        snprintf(cBuf, sizeof(cBuf), "%s/event%d", NETAPP_LINUX_INPUT_PATH, i);

        if (stat(cBuf, &statbuf) != 0)
        {
            mknod(cBuf, 0766 | S_IFCHR, makedev(NETAPP_HID_MAJOR ,NETAPP_HID_MINOR_START+i));
        }
    }

    if (NetAppPrivateInsmod(BT_MODULES, BT_MODULES_INSTALLED, NULL) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() -- Failed to insmod Bluetooth USB kernel module!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    /* Wait to give the USB driver a chance to initialize */
    NetAppOSTaskDelayMsec(NETAPP_TICK_MS*4);

    if (NetAppBluetoothPrivateIsEnabled(hNetAppBT))
    {
        if (NetAppBluetoothPrivateHotPlug(hNetAppBT, true) != NETAPP_SUCCESS)
        {
            NETAPP_MSG(("%s() Bluetooth is not connected!", __FUNCTION__));
        }
    }

    *phNetAppBT = hNetAppBT;
    hStaticNetAppBT = hNetAppBT;

    return tRetCode;

err_out:
    if (hNetAppBT != NULL)
    {
        NetAppBluetoothPrivateDeinit(hNetAppBT);
    }
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothPrivateDeinit(
    NETAPP_BT_HANDLE        hNetAppBT)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NetAppBluetoothPrivateHotPlug(hNetAppBT, false);

    if (hNetAppBT->ulSem)
    {
        NetAppOSSemDelete(hNetAppBT->ulSem);
    }

    if (hNetAppBT->hDiscoveryList != NULL)
    {
        NetAppListDestroy(hNetAppBT->hDiscoveryList);
        hNetAppBT->hDiscoveryList = NULL;
    }

    NetAppOSMemFree(hNetAppBT);
    hStaticNetAppBT = NULL;
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothPrivateHotPlug(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bInserted)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NETAPP_MSG(("%s(): %s", __FUNCTION__, bInserted ? "INSERTED" : "REMOVED"));

    if (bInserted)
    {
        unsigned int    i;
        tBSA_MGT_OPEN   tBsaOpenParam;

        /* Spawn a task to monitor the BSA Server */
        hNetAppBT->ulBsaServerId = NetAppOSTaskSpawn("NetAppBluetooth_P_ServerTask",
            NETAPP_OS_PRIORITY_LOW, 64*1024, NetAppBluetooth_P_ServerTask, hNetAppBT);
        if (hNetAppBT->ulBsaServerId == 0)
        {
            NETAPP_ERR(("%s(): Failure Spawning Bluetooth Server thread", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        BSA_MgtOpenInit(&tBsaOpenParam);
        tBsaOpenParam.callback = NetAppBluetooth_P_BsaMgrCallback;

        snprintf(tBsaOpenParam.uipc_path, sizeof(tBsaOpenParam.uipc_path), "/tmp/");
        //tBsaOpenParam.uipc_path[sizeof(tBsaOpenParam.uipc_path) -1] = '\0';

        for (i = 0; (i*NETAPP_TICK_MS*4) < NETAPP_BLUETOOTH_CONNECTION_TIMEOUT; i++)
        {
            if (BSA_MgtOpen(&tBsaOpenParam) == BSA_SUCCESS)
            {
                NETAPP_MSG(("%s(): BSA_MgtOpen SUCCESS!", __FUNCTION__));
                break;
            }
            else
            {
                NetAppOSTaskDelayMsec(NETAPP_TICK_MS*4);
            }
        }
        if ((i * NETAPP_TICK_MS*4) >= NETAPP_BLUETOOTH_CONNECTION_TIMEOUT)
        {
            NETAPP_ERR(("%s(): Cannot connect to BSA Server!", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        /* Initialize HID Profile */
        NETAPP_PRIV_API_CHECK(NetAppBluetoothHidPrivateInit(&hNetAppBT->hNetAppBTHid, hNetAppBT->hNetAppSettings), tRetCode);

       /* Initialize AV source Profile */
        NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateInit(&hNetAppBT->hNetAppBTAv, hNetAppBT->hNetAppSettings), tRetCode);

        NETAPP_PRIV_API_CHECK(NetAppBluetoothPrivateSettingsRefresh(hNetAppBT), tRetCode);

        hNetAppBT->bEnabled = true;
    }
    else
    {
        tBSA_MGT_CLOSE  tBsaCloseParam;


        if (hNetAppBT->hNetAppBTHid != NULL)
        {
            NetAppBluetoothHidPrivateDeinit(hNetAppBT->hNetAppBTHid);
        }

        if (hNetAppBT->ulBsaServerId != 0)
        {
            tBSA_MGT_KILL_SERVER param;
            hNetAppBT->bShutdown = true;
            /* Send a BSA client message to kill the server */
            BSA_MgtKillServerInit(&param);
            BSA_MgtKillServer(&param);
            NetAppOSTaskJoin(hNetAppBT->ulBsaServerId);
            NetAppOSTaskDelete(hNetAppBT->ulBsaServerId);
        }

        BSA_MgtCloseInit(&tBsaCloseParam);
        BSA_MgtClose(&tBsaCloseParam);
        hNetAppBT->bEnabled = false;
    }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothPrivateSettingsRefresh(
    NETAPP_BT_HANDLE        hNetAppBT)
{
    NETAPP_SETTINGS tSettings;
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    NETAPP_BT_DEV_INFO  *pBtDevInfoList = NULL;
    uint32_t            ulCount         = 0;
    int i;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NETAPP_PRIV_API_CHECK(NetAppPrivateGetSettings(hNetAppBT->hNetAppPriv, &tSettings), tRetCode);
    hNetAppBT->tSettings = tSettings.tBtSettings;

    NETAPP_PRIV_API_CHECK(NetAppBluetooth_P_Configure(hNetAppBT), tRetCode);

    if ( (NetAppSettingsPrivateGetSavedBtDevList(hNetAppBT->hNetAppSettings,
            &pBtDevInfoList, &ulCount) == NETAPP_SUCCESS) && (ulCount > 0) )
    {
        for (i = 0; i < ulCount; i++)
        {
            NetAppBluetoothPrivateConnect(hNetAppBT, &pBtDevInfoList[i]);
        }
        if (pBtDevInfoList != NULL)
        {
            NetAppOSMemFree(pBtDevInfoList);
        }
    }


err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothPrivateDiscover(
    NETAPP_BT_HANDLE        hNetAppBT,
    uint32_t                tServices)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    tBSA_SERVICE_MASK   tBsaServices = 0;
    tBSA_DISC_START     tDiscStartParam;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    if (hNetAppBT->bDiscovering)
    {
        tBSA_DISC_ABORT tDiscAbort;
        NETAPP_MSG(("%s(): We are already discovering\n", __FUNCTION__));
        BSA_DiscAbortInit(&tDiscAbort);
        BSA_DiscAbort(&tDiscAbort);
        hNetAppBT->bDiscovering = false;
    }

    BSA_DiscStartInit(&tDiscStartParam);

    if (tServices == NETAPP_BT_SERVICE_ALL)
    {
        tBsaServices = BSA_ALL_SERVICE_MASK;
    }
    else
    {
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_HID)  ? BSA_HID_SERVICE_MASK  : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_HSP)  ? BSA_HSP_SERVICE_MASK  : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_HFP)  ? BSA_HFP_SERVICE_MASK  : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_OPP)  ? BSA_OPP_SERVICE_MASK  : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_FTP)  ? BSA_FTP_SERVICE_MASK  : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_A2DP) ? BSA_A2DP_SERVICE_MASK : 0;
        tBsaServices |= (tServices & NETAPP_BT_SERVICE_AVRCP)? BSA_AVRCP_SERVICE_MASK: 0;
    }

    tDiscStartParam.services = tBsaServices;
    tDiscStartParam.cback = NetAppBluetooth_P_BsaDiscoveryCallback;
    tDiscStartParam.nb_devices = 0;
    tDiscStartParam.duration = 4;

    NetAppOSSemTake(hNetAppBT->ulSem);
    NetAppListEmpty(hNetAppBT->hDiscoveryList);
    NetAppOSSemGive(hNetAppBT->ulSem);

    tRetCode = (BSA_DiscStart(&tDiscStartParam) == BSA_SUCCESS) ? NETAPP_SUCCESS : NETAPP_FAILURE;
    if (tRetCode == NETAPP_SUCCESS)
    {
        hNetAppBT->bDiscovering = true;
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothPrivateGetDiscoveryResults(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      **pBtDevInfo,
    uint32_t                *pulCount)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pBtDevInfo);

    NETAPP_BT_DEV_INFO      *pBtInfo = NULL;

    NetAppOSSemTake(hNetAppBT->ulSem);

    if (NetAppListCount(hNetAppBT->hDiscoveryList) > 0)
    {
        int             i = 0;
        NETAPP_BT_LIST  *pNode = NULL;

        pBtInfo = (NETAPP_BT_DEV_INFO*)NetAppOSMemAlloc(NetAppListCount(hNetAppBT->hDiscoveryList) * sizeof(NETAPP_BT_DEV_INFO));

        NetAppListRewind(hNetAppBT->hDiscoveryList);

        for (i = 0; (i < NetAppListCount(hNetAppBT->hDiscoveryList)) && ((pNode = (NETAPP_BT_LIST*)NetAppListNext(hNetAppBT->hDiscoveryList)) != NULL); i++)
        {
            memcpy(&pBtInfo[i], &pNode->tDevInfo, sizeof(NETAPP_BT_DEV_INFO));

        }
    }

    *pulCount = NetAppListCount(hNetAppBT->hDiscoveryList);
    *pBtDevInfo = pBtInfo;

    NetAppOSSemGive(hNetAppBT->ulSem);
    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppBluetoothPrivatePair(
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    tBSA_SEC_BOND       tSecBond;
    NETAPP_HW_ADDR tAddr;

    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    BSA_SecBondInit(&tSecBond);

    NetAppAToHwAddr(pDevInfo->cAddr, tSecBond.bd_addr);

    return (BSA_SecBond(&tSecBond) == BSA_SUCCESS) ? NETAPP_SUCCESS : NETAPP_FAILURE;
}

NETAPP_RETCODE NetAppBluetoothPrivateSimplePairingAck(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bAccept,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    tBSA_SEC_SP_CFM_REPLY tSPReply;
    NETAPP_HW_ADDR tAddr;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);


    BSA_SecSpCfmReplyInit(&tSPReply);
    tSPReply.accept = bAccept;

    NetAppAToHwAddr(pDevInfo->cAddr, tAddr);
    memcpy(tSPReply.bd_addr, tAddr, sizeof(tSPReply.bd_addr));
    return (BSA_SecSpCfmReply(&tSPReply) == BSA_SUCCESS) ? NETAPP_SUCCESS : NETAPP_FAILURE;
}

NETAPP_RETCODE NetAppBluetoothPrivateSecAddDev(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    tBSA_SEC_ADD_DEV    tSecAddParam;
    NETAPP_HW_ADDR      tAddr;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    NetAppAToHwAddr(pDevInfo->cAddr, tAddr);
    BSA_SecAddDeviceInit(&tSecAddParam);

    memcpy(tSecAddParam.bd_addr, tAddr, sizeof(tSecAddParam.bd_addr));
    strncpy(tSecAddParam.bd_name, pDevInfo->cName, sizeof(tSecAddParam.bd_name));

    tSecAddParam.class_of_device[2] = pDevInfo->ucMinorClassDev;
    tSecAddParam.class_of_device[1] = pDevInfo->ucMajorClassDev + ((pDevInfo->usServiceClassDev & 0xFF) >> 5);
    tSecAddParam.class_of_device[0] = pDevInfo->usServiceClassDev >> 8 ;
    NETAPP_MSG(("Call BSA_SecAddDev with COD[%d %d %d]",tSecAddParam.class_of_device[2] ,tSecAddParam.class_of_device[1], tSecAddParam.class_of_device[0]));

    memcpy(tSecAddParam.link_key, pDevInfo->usLinkKey, sizeof(tSecAddParam.link_key));

    tSecAddParam.link_key_present   = pDevInfo->bHasLinkKey;
    tSecAddParam.trusted_services   = pDevInfo->ulTrustedServiceMask;
    tSecAddParam.is_trusted         = true;
    tSecAddParam.key_type           = pDevInfo->ucKeyType;
    tSecAddParam.io_cap             = (hNetAppBT->tSettings.bAutoPair ? BTA_IO_CAP_OUT : BTA_IO_CAP_IO);

    BSA_SecAddDevice(&tSecAddParam);

err_out:
    NETAPP_MSG(("Leaving"));
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothPrivateConnect(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);


   if ( (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HID) == NETAPP_BT_SERVICE_HID)
   {
       NETAPP_PRIV_API_CHECK(NetAppBluetoothHidPrivateConnect(hNetAppBT->hNetAppBTHid, pDevInfo), tRetCode);
   }
   else if ( (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_A2DP) == NETAPP_BT_SERVICE_A2DP)
   {
       NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateConnect(hNetAppBT->hNetAppBTAv, pDevInfo), tRetCode);
   }
   else
   {
       NETAPP_ERR(("%s(): Service 0x%08X not supported", __FUNCTION__, pDevInfo->ulServiceMask));
       tRetCode =  NETAPP_NOT_SUPPORTED;
   }

   if (pDevInfo->bHasLinkKey)
   {
       NetAppBluetoothPrivateSecAddDev(hNetAppBT, pDevInfo);
   }

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothPrivateDisconnect(
    NETAPP_BT_HANDLE        hNetAppBT,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

   if (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HID)
   {
       NETAPP_PRIV_API_CHECK(NetAppBluetoothHidDisconnect(hNetAppBT->hNetAppBTHid, pDevInfo), tRetCode);
   }
   else if ( (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_A2DP) == NETAPP_BT_SERVICE_A2DP)
   {
       NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateDisconnect(hNetAppBT->hNetAppBTAv, pDevInfo), tRetCode);
   }
   else
   {
       NETAPP_ERR(("%s(): Service not supported", __FUNCTION__));
       tRetCode =  NETAPP_NOT_SUPPORTED;
   }
err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothPrivateAvStart(
    NETAPP_BT_HANDLE        hNetAppBT,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateStart(hNetAppBT->hNetAppBTAv, bSynchronous, pBtAudioFormat), tRetCode);

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothPrivateAvStop(
    NETAPP_BT_HANDLE        hNetAppBT)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);


    NETAPP_PRIV_API_CHECK(NetAppBluetoothAvPrivateStop(hNetAppBT->hNetAppBTAv), tRetCode);

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothPrivateGetDevList(NETAPP_BT_HANDLE hNetAppBT,
		NETAPP_BT_DEV_INFO  **pBtDevInfoList,uint32_t *ulCount)
{
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NETAPP_PRIV_API_CHECK(NetAppSettingsPrivateGetSavedBtDevList(hNetAppBT->hNetAppSettings,
            pBtDevInfoList, ulCount), tRetCode);
err_out:
    return tRetCode;
}

bool NetAppBluetoothPrivateIsEnabled(
    NETAPP_BT_HANDLE        hNetAppBT)
{
    if (hNetAppBT == NULL)
    {
        return false;
    }

    if (!hNetAppBT->bEnabled)
    {
        int fd = 0;
        if ((fd = open(NETAPP_BTUSB_PATH, O_RDWR)) > 0)
        {
            close(fd);
            return true;
        }
    }
    return hNetAppBT->bEnabled;
}


void NetAppBluetoothPrivePrintBTDevInfo(
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    if (pDevInfo == NULL)
    {
        return;
    }

    NETAPP_MSG(("\n******************************"));
    NETAPP_MSG(("Name:        %s", pDevInfo->cName));
    NETAPP_MSG(("Address:     %s", pDevInfo->cAddr));
    NETAPP_MSG(("RSSI:        %ddB", pDevInfo->lRssi));
    NETAPP_MSG(("Services:    [%s%s%s%s%s%s%s]%d",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HID)   ? "HID ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HSP)   ? "HSP ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_HFP)   ? "HFP ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_OPP)   ? "OPP ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_FTP)   ? "FTP ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_A2DP)  ? "A2DP ":"",
        (pDevInfo->ulServiceMask & NETAPP_BT_SERVICE_AVRCP) ? "AVRCP ":""));
    NETAPP_MSG(("Has LinkKey: %s", pDevInfo->bHasLinkKey ? "YES" : "NO:"));
    NETAPP_MSG(("Has HID Key: %s", (pDevInfo->tHidInfo.ulLength > 0) ? "YES" : "NO:"));
    NETAPP_MSG(("Major COS:   %d", pDevInfo->ucMajorClassDev));
    NETAPP_MSG(("Minor COD:   %d", pDevInfo->ucMinorClassDev));
    NETAPP_MSG(("Service COD: %d", pDevInfo->usServiceClassDev));
    NETAPP_MSG(("Product ID:  %d", pDevInfo->usProductID));
    NETAPP_MSG(("Vendor  ID:  %d", pDevInfo->usVendorID));
    NETAPP_MSG(("******************************"));

}

/*******************************************************************************
 *                       Static NetApp Bluetooth Functions
 ******************************************************************************/
static NETAPP_RETCODE NetAppBluetooth_P_SetSecurity(
    bool                    bAutoPair)
{
    tBSA_SEC_SET_SECURITY set_security;

    BSA_SecSetSecurityInit(&set_security);
    set_security.simple_pairing_io_cap = bAutoPair ? BTA_IO_CAP_OUT : BTA_IO_CAP_IO;
    set_security.sec_cback = NetAppBluetooth_P_BsaSecurityCallback;

    return (BSA_SecSetSecurity(&set_security) == BSA_SUCCESS) ? NETAPP_SUCCESS : NETAPP_FAILURE;
}


static void NetAppBluetooth_P_ServerTask(
    void        *pParam)
{
    char            cBuf[256];
    struct stat     statbuf;
    NETAPP_BT_HANDLE hNetAppBT = (NETAPP_BT_HANDLE)pParam;

    if (hNetAppBT == NULL)
    {
        NETAPP_ERR(("%s() -- Invalid Parameter!", __FUNCTION__));
        return;
    }
    snprintf(cBuf, sizeof(cBuf), "%s/bin/%s", TARGET_DIR, NETAPP_BSA_SERVER);

    if ( (stat(NETAPP_BSA_SERVER, &statbuf) != 0) && (stat(cBuf, &statbuf) != 0) )
    {
        NETAPP_ERR(("%s(): Could not find %s or %s!!", __FUNCTION__, NETAPP_BSA_SERVER, cBuf));
        return;
    }

    while(!hNetAppBT->bShutdown)
    {
        /* Start BSA-Server */
#if 1
        switch(NetAppPrivateSystem( (stat(NETAPP_BSA_SERVER, &statbuf) == 0) ? NETAPP_BSA_SERVER : cBuf, "-u /tmp/ -all=0",
            NULL, NETAPP_WAIT_FOREVER, &hNetAppBT->bShutdown, NULL, 0, NULL))
#else
        switch(NetAppPrivateSystem( (stat(NETAPP_BSA_SERVER, &statbuf) == 0) ? NETAPP_BSA_SERVER : cBuf, "-u /tmp/ -hci=0 -app=5",
            NULL, NETAPP_WAIT_FOREVER, &hNetAppBT->bShutdown, NULL, 0, NULL))
#endif
        {
        case NETAPP_SUCCESS:
            if (!hNetAppBT->bShutdown)
            {
                unsigned int i;
                for (i = 0; (i*NETAPP_BLUETOOTH_RETRY_DELAY) < NETAPP_BLUETOOTH_CONNECTION_TIMEOUT; i++)
                {
                    NetAppOSTaskDelayMsec(NETAPP_BLUETOOTH_RETRY_DELAY);
                }
                if ((i*NETAPP_BLUETOOTH_RETRY_DELAY) >= NETAPP_BLUETOOTH_CONNECTION_TIMEOUT)
                {
                    goto err_out;
                }
                /* Need to restart the client connection
                 * TODO: give up after a certain amount of time*/
            }
            break;
        default:
            /* TODO: Notify NetApp to Die! */
            goto err_out;
        }
        if (hNetAppBT->bShutdown)
            break;
    }

err_out:
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    NetAppOSTaskExit();
}


static void NetAppBluetooth_P_BsaMgrCallback(
    tBSA_MGT_EVT        event,
    tBSA_MGT_MSG        *pBsaDevInfo)
{
    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    switch(event)
    {
    case BSA_MGT_DISCONNECT_EVT:
        NETAPP_MSG(("%s(): Server Disconnect Reason %d", __FUNCTION__, pBsaDevInfo->disconnect.reason));
        break;
    default:
        break;
    }

}


static void NetAppBluetooth_P_BsaDiscoveryCallback(
    tBSA_DISC_EVT           event,
    tBSA_DISC_MSG           *pBsaDevInfo)
{
    NETAPP_MSG(("%s(): Entered !", __FUNCTION__));

    if (hStaticNetAppBT == NULL)
    {
        NETAPP_ERR(("%s() -- Invalid Parameter!", __FUNCTION__));
        return;
    }

    switch (event)
    {
    /* a New Device has been discovered */
    case BSA_DISC_NEW_EVT:
    case BSA_DISC_DEV_INFO_EVT: /* Discovery Device Info */
    {
        NETAPP_BT_LIST  *pNode = NULL;

        NetAppOSSemTake(hStaticNetAppBT->ulSem);

        NetAppListRewind(hStaticNetAppBT->hDiscoveryList);

        while ((pNode = (NETAPP_BT_LIST*)NetAppListNext(hStaticNetAppBT->hDiscoveryList)) != NULL)
        {
            NETAPP_HW_ADDR tAddr;
            NetAppAToHwAddr(pNode->tDevInfo.cAddr, tAddr);

            /* does it already exist? */
            if (memcmp(tAddr, pBsaDevInfo->disc_new.bd_addr, sizeof(tAddr)) == 0)
            {
                break;
            }
        }

        if (pNode == NULL)
        {
            pNode = (NETAPP_BT_LIST*)NetAppListAppend(hStaticNetAppBT->hDiscoveryList);
        }

        if (pNode != NULL)
        {
            memset(&pNode->tDevInfo, 0, sizeof(NETAPP_BT_DEV_INFO));

            NetAppHwAddrToA(pBsaDevInfo->disc_new.bd_addr, pNode->tDevInfo.cAddr, sizeof(pNode->tDevInfo.cAddr));

            strncpy(pNode->tDevInfo.cName, pBsaDevInfo->disc_new.name, sizeof(pNode->tDevInfo.cName)-1);

            pNode->tDevInfo.cName[sizeof(pNode->tDevInfo.cName)-1] = '\0';
            pNode->tDevInfo.lRssi = pBsaDevInfo->disc_new.rssi;

            pNode->tDevInfo.ulServiceMask = NetAppBluetooth_P_BsatoNetAppServices(pBsaDevInfo->disc_new.services);

            /* Extract class of device values */
            BTM_COD_MINOR_CLASS(pNode->tDevInfo.ucMinorClassDev, pBsaDevInfo->disc_new.class_of_device);
            BTM_COD_MAJOR_CLASS(pNode->tDevInfo.ucMajorClassDev, pBsaDevInfo->disc_new.class_of_device);
            BTM_COD_SERVICE_CLASS(pNode->tDevInfo.usServiceClassDev, pBsaDevInfo->disc_new.class_of_device);


            NETAPP_MSG(("%s(): Device[%s]", __FUNCTION__, pNode->tDevInfo.cName));
            NETAPP_MSG(("Address:  %s", pNode->tDevInfo.cAddr));
            NETAPP_MSG(("RSSI:     %ddB", pNode->tDevInfo.lRssi));
            NETAPP_MSG(("Services: 0x%08x", pNode->tDevInfo.ulServiceMask));
            NETAPP_MSG(("COD: MAJOR: 0x%02x MINOR: 0x%02x SERVICE: 0x%04x\n",
                pNode->tDevInfo.ucMinorClassDev, pNode->tDevInfo.ucMinorClassDev,  pNode->tDevInfo.usServiceClassDev));
        }

        NetAppOSSemGive(hStaticNetAppBT->ulSem);
        break;
    }


    /* Discovery complete. */
    case BSA_DISC_CMPL_EVT:
    {
        NETAPP_MSG(("%s(): Discovery complete", __FUNCTION__));
        hStaticNetAppBT->bDiscovering = false;
        NETAPP_DO_CALLBACK(NETAPP_CB_BT_DISCOVERY_RESULTS, NULL, 0, 0, NETAPP_SUCCESS, NETAPP_IFACE_BLUETOOTH);
        break;
    }

    default:
        NETAPP_ERR(("%s(): unknown event", __FUNCTION__));
        break;
    }
    NETAPP_MSG(("%s(): Leaving", __FUNCTION__));
}


static void NetAppBluetooth_P_BsaSecurityCallback(
    tBSA_SEC_EVT            event,
    tBSA_SEC_MSG            *pBsaDevInfo)
{
    int                     status;
    int                     indexDisc;
    tBSA_SEC_PIN_CODE_REPLY pin_code_reply;
    tBSA_SEC_AUTH_REPLY     autorize_reply;
    NETAPP_BT_DEV_INFO      tDevInfo;
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;

    NETAPP_MSG(("%s(): Entered !", __FUNCTION__));

    if (hStaticNetAppBT == NULL)
    {
        NETAPP_ERR(("%s() -- Invalid Parameter!", __FUNCTION__));
        return;
    }
    memset(&tDevInfo, 0, sizeof(NETAPP_BT_DEV_INFO));


    /* Pairing/Bonding security callbacks */
    switch(event)
    {
    case BSA_SEC_LINK_UP_EVT:
        NETAPP_MSG(("%s(): [%02x:%02x:%02x:%02x:%02x:%02x] Link UP", __FUNCTION__,
            pBsaDevInfo->link_up.bd_addr[0], pBsaDevInfo->link_up.bd_addr[1],
            pBsaDevInfo->link_up.bd_addr[2], pBsaDevInfo->link_up.bd_addr[3],
            pBsaDevInfo->link_up.bd_addr[4], pBsaDevInfo->link_up.bd_addr[5]));
        break;

    case BSA_SEC_LINK_DOWN_EVT:
        NETAPP_MSG(("%s(): [%02x:%02x:%02x:%02x:%02x:%02x] Link DOWN", __FUNCTION__,
            pBsaDevInfo->link_down.bd_addr[0], pBsaDevInfo->link_down.bd_addr[1],
            pBsaDevInfo->link_down.bd_addr[2], pBsaDevInfo->link_down.bd_addr[3],
            pBsaDevInfo->link_down.bd_addr[4], pBsaDevInfo->link_down.bd_addr[5]));
        break;


    case BSA_SEC_PIN_REQ_EVT:
        NETAPP_MSG(("BSA_SEC_PIN_REQ_EVT (Pin Code Request) received"));

        BSA_SecPinCodeReplyInit(&pin_code_reply);
        bdcpy(pin_code_reply.bd_addr, pBsaDevInfo->pin_req.bd_addr);
        pin_code_reply.pin_len = hStaticNetAppBT->tSettings.ulPinLength;
        strncpy((char *)pin_code_reply.pin_code, hStaticNetAppBT->tSettings.ucPinCode, hStaticNetAppBT->tSettings.ulPinLength);
        pin_code_reply.pin_code[PIN_CODE_LEN-1] = '\0';
        status = BSA_SecPinCodeReply(&pin_code_reply);
        break;


    case BSA_SEC_AUTH_CMPL_EVT:
        NETAPP_MSG(("BSA_SEC_AUTH_CMPL_EVT (name=%s,success=%d)",
                   pBsaDevInfo->auth_cmpl.bd_name, pBsaDevInfo->auth_cmpl.success));

        tRetCode = (pBsaDevInfo->auth_cmpl.success) ? NETAPP_SUCCESS : NETAPP_FAILURE;

        NetAppHwAddrToA(pBsaDevInfo->auth_cmpl.bd_addr, tDevInfo.cAddr, sizeof(tDevInfo.cAddr));

        if(tRetCode == NETAPP_SUCCESS)
        {
            NETAPP_BT_LIST *pNode = NULL;

            /* Get element info here */
            NetAppSettingsPrivateGetSavedBtDev(hStaticNetAppBT->hNetAppSettings, pBsaDevInfo->auth_cmpl.bd_addr, &tDevInfo);

            /* We need the COD from the discovery list, any better ideas? */
            NetAppOSSemTake(hStaticNetAppBT->ulSem);
            NetAppListRewind(hStaticNetAppBT->hDiscoveryList);
            while ((pNode = (NETAPP_BT_LIST*)NetAppListNext(hStaticNetAppBT->hDiscoveryList)) != NULL)
            {
                NETAPP_HW_ADDR tAddr;
                NetAppAToHwAddr(pNode->tDevInfo.cAddr, tAddr);

                if (memcmp(tAddr, pBsaDevInfo->auth_cmpl.bd_addr, sizeof(tAddr)) == 0)
                {
                    tDevInfo.ucMajorClassDev    = pNode->tDevInfo.ucMajorClassDev;
                    tDevInfo.ucMinorClassDev    = pNode->tDevInfo.ucMinorClassDev;
                    tDevInfo.usServiceClassDev  = pNode->tDevInfo.usServiceClassDev;
                    tDevInfo.ulServiceMask      = pNode->tDevInfo.ulServiceMask;
                    break;
                }
            }
            NetAppOSSemGive(hStaticNetAppBT->ulSem);

            strncpy(tDevInfo.cName, pBsaDevInfo->auth_cmpl.bd_name, sizeof(tDevInfo.cName)-1);
            tDevInfo.cName[sizeof(tDevInfo.cName)-1] = '\0';

            if (pBsaDevInfo->auth_cmpl.key_present)
            {
                memcpy(&tDevInfo.usLinkKey, pBsaDevInfo->auth_cmpl.key, sizeof(tDevInfo.usLinkKey));
                tDevInfo.ucKeyType = pBsaDevInfo->auth_cmpl.key_type;
                tDevInfo.bHasLinkKey = true;
            }
            NetAppBluetoothPrivePrintBTDevInfo(&tDevInfo);
            NetAppSettingsPrivateSaveBtDev(hStaticNetAppBT->hNetAppSettings, &tDevInfo);
        }

        NETAPP_DO_CALLBACK(NETAPP_CB_BT_AUTH_COMPLETE, &tDevInfo, sizeof(tDevInfo), 0, tRetCode, NETAPP_IFACE_BLUETOOTH);
        break;



    case BSA_SEC_AUTHORIZE_EVT:  /* Authorization request */
        NETAPP_MSG(("BSA_SEC_AUTHORIZE_EVT: Remote device:%s", pBsaDevInfo->authorize.bd_name));
        NETAPP_MSG(("    Request access to service:%x", (int)pBsaDevInfo->authorize.service));

        /* Always auto accept service conncetion request  */
        status = BSA_SecAuthorizeReplyInit(&autorize_reply);
        memcpy(autorize_reply.bd_addr, pBsaDevInfo->authorize.bd_addr, sizeof(autorize_reply.bd_addr));
        autorize_reply.trusted_service = pBsaDevInfo->authorize.service;
        autorize_reply.auth = BSA_SEC_AUTH_PERM;
        status = BSA_SecAuthorizeReply(&autorize_reply);

        if(status == BSA_SUCCESS)
        {
            /* get element info here*/
            if( NetAppSettingsPrivateGetSavedBtDev( hStaticNetAppBT->hNetAppSettings,
                    pBsaDevInfo->authorize.bd_addr, &tDevInfo) != NETAPP_NOT_FOUND )
            {
                tDevInfo.ulTrustedServiceMask |= pBsaDevInfo->authorize.service;
                NetAppSettingsPrivateSaveBtDev(hStaticNetAppBT->hNetAppSettings, &tDevInfo);
            }
            else
            {
                NETAPP_ERR((" BSA_SEC_AUTHORIZE_EVT - device not found in database"));
            }
        }
        break;


    case BSA_SEC_SP_CFM_REQ_EVT: /* Simple Pairing confirm request */
        NetAppHwAddrToA(pBsaDevInfo->cfm_req.bd_addr, tDevInfo.cAddr, sizeof(tDevInfo.cAddr));
        strncpy(tDevInfo.cName, pBsaDevInfo->cfm_req.bd_name, sizeof(tDevInfo.cName)-1);

        NETAPP_MSG(("BSA_SEC_SP_CFM_REQ_EVT: Remote device:%s", pBsaDevInfo->cfm_req.bd_name));

        if (hStaticNetAppBT->tSettings.bAutoPair)
        {
            NetAppBluetoothPrivateSimplePairingAck(hStaticNetAppBT, true, &tDevInfo);
        }
        else
        {
            NETAPP_DO_CALLBACK(NETAPP_CB_BT_SP_CONFIRM_REQ, &tDevInfo, sizeof(tDevInfo), 0, tRetCode, NETAPP_IFACE_BLUETOOTH);
        }
        break;


    case BSA_SEC_SP_KEY_NOTIF_EVT: /* Simple Pairing Passkey Notification */
        NETAPP_MSG(("BSA_SEC_SP_KEY_NOTIF_EVT"));
        NetAppHwAddrToA(pBsaDevInfo->key_notif.bd_addr, tDevInfo.cAddr, sizeof(tDevInfo.cAddr));

        NETAPP_DO_CALLBACK(NETAPP_CB_BT_SP_CONFIRM_REQ, &tDevInfo, sizeof(tDevInfo), pBsaDevInfo->key_notif.passkey, tRetCode, NETAPP_IFACE_BLUETOOTH);
        break;


    case BSA_SEC_SP_KEYPRESS_EVT: /* Simple Pairing Key press notification event. */
        NETAPP_MSG(("BSA_SEC_SP_KEYPRESS_EVT (type:%d)", pBsaDevInfo->key_press.notif_type));
        break;

    case BSA_SEC_SP_RMT_OOB_EVT: /* Simple Pairing Remote OOB Data request. */
        NETAPP_MSG(("BSA_SEC_SP_RMT_OOB_EVT received, Not Implemented"));
        break;

    default:
        NETAPP_ERR(("unknown event:%d", event));
        break;
    }
}


/* Reconfigure the BT device after the BT is brought up */
static NETAPP_RETCODE NetAppBluetooth_P_Configure(
    NETAPP_BT_HANDLE        hNetAppBT)
{
    tBSA_DM_SET_CONFIG  bsa_dm_set_config;
    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    NETAPP_BT_BSA_CHECK(BSA_DmSetConfigInit(&bsa_dm_set_config), tRetCode);

    bsa_dm_set_config.enable        = true;
    bsa_dm_set_config.discoverable  = hNetAppBT->tSettings.bDiscoverable;
    bsa_dm_set_config.connectable   = true;
    bsa_dm_set_config.config_mask   = BSA_DM_CONFIG_VISIBILITY_MASK;

    NETAPP_BT_BSA_CHECK(BSA_DmSetConfig(&bsa_dm_set_config), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppBluetooth_P_SetSecurity(hNetAppBT->tSettings.bAutoPair), tRetCode);

err_out:
    return tRetCode;
}


static uint32_t NetAppBluetooth_P_BsatoNetAppServices(
    uint32_t                ulBsaServices)
{
    uint32_t ulNetAppServices = 0;

    ulNetAppServices |= (ulBsaServices & BSA_HID_SERVICE_MASK)  ? NETAPP_BT_SERVICE_HID   : 0;
    ulNetAppServices |= (ulBsaServices & BSA_HSP_SERVICE_MASK)  ? NETAPP_BT_SERVICE_HSP   : 0;
    ulNetAppServices |= (ulBsaServices & BSA_HFP_SERVICE_MASK)  ? NETAPP_BT_SERVICE_HFP   : 0;
    ulNetAppServices |= (ulBsaServices & BSA_OPP_SERVICE_MASK)  ? NETAPP_BT_SERVICE_OPP   : 0;
    ulNetAppServices |= (ulBsaServices & BSA_FTP_SERVICE_MASK)  ? NETAPP_BT_SERVICE_FTP   : 0;
    ulNetAppServices |= (ulBsaServices & BSA_A2DP_SERVICE_MASK) ? NETAPP_BT_SERVICE_A2DP  : 0;
    ulNetAppServices |= (ulBsaServices & BSA_AVRCP_SERVICE_MASK)? NETAPP_BT_SERVICE_AVRCP : 0;

    return ulNetAppServices;
}

