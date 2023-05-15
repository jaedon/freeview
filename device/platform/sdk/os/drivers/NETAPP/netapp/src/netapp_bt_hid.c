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
 * $brcm_Workfile: netapp_bt_hid.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/netapp/src/netapp_bt_hid.c $
 *
 * 2   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 4   12/12/11 11:15p steven
 * SW7231-513: Various fixes
 *
 * 3   12/9/11 4:44p steven
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
#include "netapp_bt_hid.h"
#include "bsa_api.h"
#include "bthid.h"

#define NETAPP_HID_BTHID_PATH       "/dev/bthid"

// Auto pair timer to auto discover and connect to BT HID devices
//#define BT_HID_AUTO_PAIR

typedef struct
{
    NETAPP_BT_HID_VOICE_INFO    tVoiceInfo;
    uint32_t                    ulVoiceSize;
    int32_t                     lVoiceFd;
    BOOLEAN                     bIsVoiceStreamingActive;
    BOOLEAN                     bIsAudioDevice;
    uint16_t                    usReadCodecSettings;
} NETAPP_BT_HID_AUDIO_PRIV_INFO;

typedef struct
{
    uint16_t          usSeqNum;
    uint16_t          usTimestamp;
    uint16_t          usBufferState;
    uint16_t          usReserved;
    uint16_t          usDataCnt;
}NETAPP_BT_HID_AUDIO_DATA_HDR;

typedef struct
{
    BOOLEAN                       in_use;
    tBSA_HH_HANDLE                handle;
    int                           bthid_fd;
    NETAPP_BT_DEV_INFO            tDevInfo;
    NETAPP_BT_HID_AUDIO_PRIV_INFO tAudioFormat;
} NETAPP_HID_DEVICE;


struct sNETAPP_BT_HID
{
    tUIPC_CH_ID             tUipcChannel;
    NETAPP_HID_DEVICE       tHidTable[NETAPP_HID_DEVICE_MAX];
    NETAPP_SETTINGS_HANDLE  hNetAppSettings;
    uint32_t                ulSem;
#ifdef NETAPP_AUTOPAIR_HID
    uint32_t                ulAutoPairTimer;
#endif
};

static void NetAppBluetoothHid_P_HidCallback(
    tBSA_HH_EVT             event,
    tBSA_HH_MSG             *p_data);


static void NetAppBluetoothHid_P_UipcCallback(
    BT_HDR                  *p_msg);


static void NetAppBluetoothHid_P_AutoPairTimer(
    void                    *pParam);


static NETAPP_RETCODE NetAppBluetoothHid_P_AddDev(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo);


static NETAPP_RETCODE NetAppBluetoothHid_P_DescWrite(
    int                     fd,
    char                    *pBuf,
    int                     length);


static void NetAppBluetoothHid_P_AudioHandler(
        tBSA_HH_REPORT_DATA* p_report_data,
        int dev_index);


static void NetAppBluetoothHid_P_BsaAutoPairDiscoveryCallback(
    tBSA_DISC_EVT           event,
    tBSA_DISC_MSG           *pBsaDevInfo);


/* Need static handle for Bluetooth callbacks :-( */
static NETAPP_BT_HID_HANDLE    hStaticNetAppBTHid = NULL;

/*******************************************************************************
 *                 Public (but Internal to NetApp) Bluetooth HID API's
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothHidPrivateInit(
    NETAPP_BT_HID_HANDLE    *phNetAppBTHid,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_BT_HID_HANDLE    hNetAppBTHid = NULL;
    tBSA_HH_ENABLE          tParams;
    int                     index;


    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    NETAPP_PRIV_HANDLE_CHECK(phNetAppBTHid);

    hNetAppBTHid = (NETAPP_BT_HID_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_BT_HID));
    if (hNetAppBTHid == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
       return NETAPP_OUT_OF_MEMORY;
    }

    memset(hNetAppBTHid, 0, sizeof(struct sNETAPP_BT_HID));

    hNetAppBTHid->ulSem = NetAppOSSemBCreate();
    if (hNetAppBTHid->ulSem == 0)
    {
        NETAPP_ERR(("%s(): -- Cannot create sem!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    BSA_HhEnableInit(&tParams);
    tParams.sec_mask = BSA_SEC_NONE;
    tParams.p_cback = NetAppBluetoothHid_P_HidCallback;

    NETAPP_BT_BSA_CHECK(BSA_HhEnable(&tParams), tRetCode);

    hNetAppBTHid->tUipcChannel = tParams.uipc_channel; /* Save UIPC channel */

    UIPC_Open(hNetAppBTHid->tUipcChannel, NetAppBluetoothHid_P_UipcCallback);  /* Open UIPC channel */

#ifdef BT_HID_AUTO_PAIR
    NETAPP_PRIV_API_CHECK(NetAppOSTimerCreate(&hNetAppBTHid->ulAutoPairTimer, NetAppBluetoothHid_P_AutoPairTimer, hNetAppBTHid), tRetCode);
    NetAppOSTimerSetRepeatMsec(hNetAppBTHid->ulAutoPairTimer, (20*1000));
#endif

    hNetAppBTHid->hNetAppSettings   = hNetAppSettings;
    hStaticNetAppBTHid              = hNetAppBTHid;
    *phNetAppBTHid                  = hNetAppBTHid;

err_out:
    if ( (hNetAppBTHid != NULL) && (tRetCode != NETAPP_SUCCESS) )
    {
        NetAppBluetoothHidPrivateDeinit(hNetAppBTHid);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothHidPrivateDeinit(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid)
{
    tBSA_HH_DISABLE dis_param;
    unsigned int    i = 0;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTHid);

#ifdef BT_HID_AUTO_PAIR
    if (hNetAppBTHid->ulAutoPairTimer != 0)
    {
        NetAppOSTimerStop(hNetAppBTHid->ulAutoPairTimer);
        NetAppOSTimerDelete(hNetAppBTHid->ulAutoPairTimer);
        hNetAppBTHid->ulAutoPairTimer = 0;
    }
#endif

    for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
    {
        tBSA_HH_CLOSE   tCloseParams;
        if ( hNetAppBTHid->tHidTable[i].in_use )
        {
            NETAPP_MSG(("%s(): Disconnecting HID device:%d", __FUNCTION__, hNetAppBTHid->tHidTable[i].handle));
            tCloseParams.handle = hNetAppBTHid->tHidTable[i].handle;
            BSA_HhCloseInit(&tCloseParams);
            BSA_HhClose(&tCloseParams);
        }
    }


    UIPC_Close(hNetAppBTHid->tUipcChannel);  /* Close UIPC channel */

    BSA_HhDisableInit(&dis_param);
    BSA_HhDisable(&dis_param);

    if (hNetAppBTHid->ulSem)
    {
        NetAppOSSemDelete(hNetAppBTHid->ulSem);
    }

    NetAppOSMemFree(hNetAppBTHid);
    hStaticNetAppBTHid = NULL;

    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppBluetoothHidPrivateConnect(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    tBSA_HH_OPEN        tOpenParams;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_HID_DEVICE   *pHidDevInfo = NULL;
    unsigned int        i;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTHid);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    NETAPP_PRIV_SEM_TAKE(hNetAppBTHid->ulSem, tRetCode);

    /* Check for an open slot and if we are already connected with this HID device? */
    for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
    {
        if ( (hNetAppBTHid->tHidTable[i].in_use) &&
             (strncmp(hNetAppBTHid->tHidTable[i].tDevInfo.cAddr, pDevInfo->cAddr, sizeof(pDevInfo->cAddr)/sizeof(pDevInfo->cAddr[0])) == 0) )
        {
            NETAPP_MSG(("%s(): We are already connected so ignore connection request", __FUNCTION__));
            tRetCode = NETAPP_SUCCESS;
            goto err_out;
        }
        else if (!hNetAppBTHid->tHidTable[i].in_use)
        {
            pHidDevInfo = &hNetAppBTHid->tHidTable[i];
            memset(pHidDevInfo, 0, sizeof(NETAPP_HID_DEVICE));
            break;
        }
    }

    if (pHidDevInfo == NULL)
    {
        NETAPP_ERR(("%s(): Too many open devices!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    pHidDevInfo->in_use = true;

    pHidDevInfo->tDevInfo = *pDevInfo;

    /* We already connected to the device so skip the connection process */
    if (pDevInfo->tHidInfo.ulLength > 0)
    {
        NETAPP_MSG(("%s(): Reconnecting to saved device!", __FUNCTION__));
        NETAPP_PRIV_API_CHECK(NetAppBluetoothHid_P_AddDev(hNetAppBTHid, pDevInfo), tRetCode);
    }
    else
    {
        BSA_HhOpenInit(&tOpenParams);
        NETAPP_HW_ADDR tAddr;
        NetAppAToHwAddr(pDevInfo->cAddr, tAddr);

        memcpy(tOpenParams.bd_addr, tAddr, sizeof(tOpenParams.bd_addr));
        tOpenParams.mode        = BTA_HH_PROTO_RPT_MODE;
        tOpenParams.sec_mask    = BSA_SEC_NONE;

        NETAPP_BT_BSA_CHECK(BSA_HhOpen(&tOpenParams), tRetCode);
        NETAPP_MSG(("%s(): Connecting to HID device:%s", __FUNCTION__, pDevInfo->cName));
    }
err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppBTHid->ulSem);
    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothHidDisconnect(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    unsigned int    i = 0;
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    tBSA_HH_CLOSE   tCloseParams;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTHid);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    NETAPP_MSG(("%s(): Entered", __FUNCTION__));

    BSA_HhCloseInit(&tCloseParams);

    for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
    {
        if (strncmp(pDevInfo->cAddr, hNetAppBTHid->tHidTable[i].tDevInfo.cAddr, strlen(pDevInfo->cAddr)) == 0)
        {
            tCloseParams.handle = hNetAppBTHid->tHidTable[i].handle;
            printf("Found Hid device to disconnect [handle %d]\n",tCloseParams.handle);
            break;
        }
    }

    if (i < NETAPP_HID_DEVICE_MAX)
    {
        printf("CALL BSA CLOSE\n");
        NETAPP_BT_BSA_CHECK(BSA_HhClose(&tCloseParams), tRetCode);
    }

err_out:
    return tRetCode;
}

/*******************************************************************************
 *                       Static NetApp Bluetooth HID Functions
 ******************************************************************************/
static void NetAppBluetoothHid_P_HidCallback(
    tBSA_HH_EVT             event,
    tBSA_HH_MSG             *p_data)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_HID_DEVICE   *pHidDevInfo = NULL;
    unsigned int        i = 0;
    NETAPP_HW_ADDR      tHwAddr;

    if (hStaticNetAppBTHid == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }

    NETAPP_PRIV_SEM_TAKE(hStaticNetAppBTHid->ulSem, tRetCode);

    switch (event)
    {
    case BSA_HH_OPEN_EVT: /* Connection Open*/
    {
        tBSA_HH_GET get_dscpinfo;

        NETAPP_MSG(("%s(): BSA_HH_OPEN_EVT: status%d, handle:%d",  __FUNCTION__, p_data->open.status, p_data->open.handle));

        /* Look if this is an Open from a Connect() request */
        for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
        {
            NETAPP_HW_ADDR tAddr;
            memset(&tAddr, 0, sizeof(NETAPP_HW_ADDR));
            NetAppAToHwAddr(hStaticNetAppBTHid->tHidTable[i].tDevInfo.cAddr, tAddr);
            if ( (hStaticNetAppBTHid->tHidTable[i].in_use) &&
                 (memcmp(tAddr, p_data->open.bd_addr, sizeof(NETAPP_HW_ADDR)) == 0) )
            {
                pHidDevInfo = &hStaticNetAppBTHid->tHidTable[i];
                break;
            }
        }

        /* OK so we got an open request and there is no device we're trying to open
         * (no one called the Connect() API) which means the bluetooth device disconnected
         * to save power and now reconnected so lets see if we have room to look up the device
         * and open it if we already connected to it in the past.
         */
        if (pHidDevInfo == NULL)
        {
            for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
            {
                if (!hStaticNetAppBTHid->tHidTable[i].in_use)
                {
                    pHidDevInfo = &hStaticNetAppBTHid->tHidTable[i];
                    break;
                }
            }

            /* We have space so lets reconnect. */
            if (pHidDevInfo != NULL)
            {
                /* Read already saved settings if device already present in database */
                NetAppSettingsPrivateGetSavedBtDev(hStaticNetAppBTHid->hNetAppSettings,p_data->open.bd_addr, &pHidDevInfo->tDevInfo) ;
                pHidDevInfo->in_use     = true;
            }
            else
            {
                NETAPP_ERR(("%s(): Too many open devices ", __FUNCTION__));
                NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, NULL, 0, NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
                break;
            }
        }

        if (p_data->open.status != BSA_SUCCESS)
        {
            memset(pHidDevInfo, 0, sizeof(NETAPP_HID_DEVICE));
            NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
            break;
        }

        if ((pHidDevInfo->tDevInfo.ucMajorClassDev == BTM_COD_MAJOR_PERIPHERAL) &&
            ((pHidDevInfo->tDevInfo.ucMinorClassDev & ~BTM_COD_MINOR_COMBO) == BTM_COD_MINOR_REMOTE_CONTROL))
        {
             /* If Service audio or Capturing bits set, this is a Voice capable RC */
             if (pHidDevInfo->tDevInfo.usServiceClassDev & (BTM_COD_SERVICE_AUDIO | BTM_COD_SERVICE_CAPTURING))
             {
                 pHidDevInfo->tAudioFormat.bIsVoiceStreamingActive = FALSE;
                 pHidDevInfo->tAudioFormat.bIsAudioDevice = TRUE;
                 NETAPP_MSG(("DEVICE is AUDIO"));
             }
             else
             {
                 NETAPP_MSG(("DEVICE is NOT AUDIO %d %d %d",pHidDevInfo->tDevInfo.ucMajorClassDev,
                         pHidDevInfo->tDevInfo.ucMinorClassDev,pHidDevInfo->tDevInfo.usServiceClassDev));
                 pHidDevInfo->tAudioFormat.bIsAudioDevice = FALSE;
             }
        }
        else
        {
            NETAPP_MSG(("UNEXPECTED COD %d %d %d - device is not a remote",pHidDevInfo->tDevInfo.ucMajorClassDev,
                         pHidDevInfo->tDevInfo.ucMinorClassDev,pHidDevInfo->tDevInfo.usServiceClassDev));
        }

        pHidDevInfo->handle     = p_data->open.handle;

        pHidDevInfo->bthid_fd   = open(NETAPP_HID_BTHID_PATH, O_RDWR);
        if (pHidDevInfo->bthid_fd < 0)
        {
            NETAPP_ERR(("%s(): Failed to open %s : %d", __FUNCTION__, NETAPP_HID_BTHID_PATH, errno));
            NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
            break;
        }

        /* Check if HID descriptor is present */
        if ( (pHidDevInfo->tDevInfo.tHidInfo.ulLength > 0) &&
             (NetAppBluetoothHid_P_DescWrite(pHidDevInfo->bthid_fd, pHidDevInfo->tDevInfo.tHidInfo.usData, pHidDevInfo->tDevInfo.tHidInfo.ulLength) == NETAPP_SUCCESS) )
        {
            NetAppSettingsPrivateSaveBtDev(hStaticNetAppBTHid->hNetAppSettings, &pHidDevInfo->tDevInfo);

            NETAPP_MSG(("%s(): AddDev of saved descriptor was successful!", __FUNCTION__));
            NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_SUCCESS, NETAPP_IFACE_BLUETOOTH);
        }
        else
        {
            NETAPP_MSG(("%s(): No valid Descriptor or application of descriptor failed", __FUNCTION__));

            BSA_HhGetInit(&get_dscpinfo);
            get_dscpinfo.request = BSA_HH_DSCP_REQ;
            get_dscpinfo.handle = p_data->open.handle;

            if (BSA_HhGet(&get_dscpinfo) != BSA_SUCCESS)
            {
                NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
            }
        }
#ifdef BT_HID_AUTO_PAIR
        NetAppOSTimerStop(hStaticNetAppBTHid->ulAutoPairTimer);
#endif
        break;
    }


    case BSA_HH_CLOSE_EVT: /* Connection Closed */
        NETAPP_MSG(("BSA_HH_CLOSE_EVT: status:%d, handle:%d", p_data->close.status, p_data->close.handle));
        if(p_data->close.status == BSA_SUCCESS)
        {
            for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
            {
                if (hStaticNetAppBTHid->tHidTable[i].handle == p_data->close.handle)
                {
                    NETAPP_MSG(("%s(): Found open reference for %d", __FUNCTION__, i));
                    pHidDevInfo = &hStaticNetAppBTHid->tHidTable[i];
                    NETAPP_DO_CALLBACK(NETAPP_CB_DISCONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_SUCCESS, NETAPP_IFACE_BLUETOOTH);
                    break;
                }
            }
            if (pHidDevInfo != NULL)
            {
                if (pHidDevInfo->tAudioFormat.lVoiceFd > 0)
                {
                    close(pHidDevInfo->tAudioFormat.lVoiceFd);
                    pHidDevInfo->tAudioFormat.lVoiceFd = -1;
                    pHidDevInfo->tAudioFormat.bIsVoiceStreamingActive = FALSE;
                }
                NETAPP_MSG(("%s(): Closing bthid_fd", __FUNCTION__));
                if(pHidDevInfo->bthid_fd > 0)
                    close(pHidDevInfo->bthid_fd);
                memset(pHidDevInfo, 0, sizeof(NETAPP_HID_DEVICE));
            }
            else
            {
                NETAPP_DO_CALLBACK(NETAPP_CB_DISCONNECT, NULL, 0, NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
            }
        }
        else
        {
            NETAPP_MSG(("BSA_HhClose error status %d",p_data->close.status));
            NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
        }

        break;


    case BSA_HH_GET_DSCPINFO_EVT: /* DSCPINFO */
        NETAPP_MSG(("%s(): BSA_HH_GET_DSCPINFO_EVT: status:%d, handle:%d",
            __FUNCTION__, p_data->dscpinfo.status, p_data->dscpinfo.handle));

        for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
        {
            if (hStaticNetAppBTHid->tHidTable[i].handle == p_data->dscpinfo.handle)
            {
                pHidDevInfo = &hStaticNetAppBTHid->tHidTable[i];
                break;
            }
        }
        if (pHidDevInfo == NULL)
        {
            NETAPP_ERR(("%s(): Invalid State!", __FUNCTION__));
            break;
        }

        if (p_data->dscpinfo.status != BSA_SUCCESS)
        {
            NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, &pHidDevInfo->tDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, NETAPP_FAILURE, NETAPP_IFACE_BLUETOOTH);
            break;
        }

        /* Need to load the settings from the database to fetch the link key if any so we don't overwrite it */
        NetAppAToHwAddr(pHidDevInfo->tDevInfo.cAddr, tHwAddr);
        if (NetAppSettingsPrivateGetSavedBtDev(hStaticNetAppBTHid->hNetAppSettings, tHwAddr, &pHidDevInfo->tDevInfo) == NETAPP_SUCCESS)
        {
            NETAPP_MSG(("%s(): Found saved item, lets update it with the hid descriptor!", __FUNCTION__));
        }

        memcpy(pHidDevInfo->tDevInfo.tHidInfo.usData, p_data->dscpinfo.dscpdata.data, p_data->dscpinfo.dscpdata.length);
        pHidDevInfo->tDevInfo.tHidInfo.ulLength = p_data->dscpinfo.dscpdata.length;

        NETAPP_MSG(("%s(): BSA_HH_GET_DSCPINFO_EVT: writing descriptor to the Kernel driver, size=%d", __FUNCTION__, p_data->dscpinfo.dscpdata.length));

        /* Send HID descriptor to the kernel (via bthid module) */
        if (NetAppBluetoothHid_P_DescWrite(pHidDevInfo->bthid_fd,
            p_data->dscpinfo.dscpdata.data, p_data->dscpinfo.dscpdata.length) == NETAPP_SUCCESS)
        {
            pHidDevInfo->tDevInfo.ulTrustedServiceMask |= BSA_HID_SERVICE_MASK;
            NetAppSettingsPrivateSaveBtDev(hStaticNetAppBTHid->hNetAppSettings, &pHidDevInfo->tDevInfo);

            NetAppBluetoothHid_P_AddDev(hStaticNetAppBTHid, &pHidDevInfo->tDevInfo);
        }
        else
        {
            NETAPP_ERR(("%s(): NetAppBluetoothHid_P_DescWrite() Failed!!", __FUNCTION__));
        }
        break;


    case BSA_HH_GET_REPORT_EVT:
        NETAPP_MSG(("BSA_HH_GET_REPORT_EVT: status:%d, handle:%d, len:%d",
                p_data->get_report.status, p_data->get_report.handle, p_data->get_report.report.length));
        break;


    case BSA_HH_SET_REPORT_EVT:
        NETAPP_MSG(("BSA_HH_SET_REPORT_EVT: s=%d, h=%d", p_data->set_report.status, p_data->set_report.handle));
        break;

    case BSA_HH_GET_PROTO_EVT:
    case BSA_HH_SET_PROTO_EVT:
    case BSA_HH_GET_IDLE_EVT:
    case BSA_HH_SET_IDLE_EVT:
    case BSA_HH_VC_UNPLUG_EVT:
    case BSA_HH_MIP_START_EVT:
    case BSA_HH_MIP_STOP_EVT:
        NETAPP_MSG(("%s(): Event %d not handled", __FUNCTION__, event));
        break;

    default:
        NETAPP_ERR(("%s(): bad event:%d", __FUNCTION__, event));
        break;
    }

err_out:
    NETAPP_PRIV_SEM_GIVE(hStaticNetAppBTHid->ulSem);
}


static int NetAppBluetoothHid_P_SetAudioReport(tBSA_HH_HANDLE handle,
        UINT8* p_audio_data,
        UINT8 length)
{
    tBSA_HH_SET hh_set_param;
    int status;

    BSA_HhSetInit(&hh_set_param);

    hh_set_param.request = BSA_HH_REPORT_REQ;
    hh_set_param.handle = handle;
    hh_set_param.param.set_report.report_type = BSA_HH_RPTT_FEATURE;

    hh_set_param.param.set_report.report.length = length+1;
    hh_set_param.param.set_report.report.data[0] = NETAPP_HID_AUDIO_REPORT_ID;
    memcpy(&hh_set_param.param.set_report.report.data[1], p_audio_data, length);

    status = BSA_HhSet(&hh_set_param);

    if(status != BSA_SUCCESS)
    {
        NETAPP_ERR(("BSA_HhSet fail status:%d", status));
        return (-1);
    }
    return 0;
}


static void NetAppBluetoothHid_P_AudioHandler(
        tBSA_HH_REPORT_DATA* p_report_data,
        int dev_index)
{
    int hh_voice_size;
    NETAPP_BT_HID_AUDIO_DATA_HDR audiodata_hdr;
    int status;
    uint8_t audio_report[NETAPP_HID_AUDIO_MAX_REPORT_SIZE];
    uint8_t* p_data;
    uint8_t audio_format;
    uint8_t codec_setting_id;
    uint16_t codec_setting_len;
    uint8_t sample_freq;
    uint8_t bits_per_samp;



    p_data = (uint8_t*)&p_report_data->data[NETAPP_HID_AUDIO_FORMAT_OFFSET];
    STREAM_TO_UINT8(audio_format, p_data);

    switch(audio_format)
    {
     case NETAPP_HID_AUDIO_RC_MIC_START:
         strncpy(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename,
                 NETAPP_HID_AUDIO_FILE_NAME,
                 sizeof(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename) - 1);
         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename[
                         sizeof(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename) - 1] = '\0';

         /* Force the config as a 1st approach */
         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.nbChannels = 1;
         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.sampleRate = 16000;
         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.bitsPerSample =16;

         NETAPP_MSG(("Channels = %d, Sample rate = %d, BitPerSample = %d",
                   (int) hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.nbChannels,
                   (int) hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.sampleRate,
                   (int) hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.bitsPerSample));

         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.ulVoiceSize = 0;

         /* Create file in read/write mode */
         if(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd > 0)
         {
            close(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd);
         }
         remove(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename);

         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd =
                 open(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename,
                         O_RDWR | O_CREAT ,
                         0666);
         if (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd < 0)
         {
             NETAPP_ERR(("%s() Could not create file for audio dump", __FUNCTION__));
         }

         /* Send APP_HH_AUDIO_MIC_START */
         NETAPP_MSG(("Send NETAPP_HID_AUDIO_MIC_START"));
         p_data = &audio_report[0];
         UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_MIC_START); /* Report format */
         UINT8_TO_STREAM(p_data, 0); /* channel */
         UINT8_TO_STREAM(p_data, 1); /* mode 1: send start in response of start req from remote */
         UINT16_TO_STREAM(p_data, 0); /* usReserved */
         UINT16_TO_STREAM(p_data, 0); /* data count */

         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive = TRUE;

         status = NetAppBluetoothHid_P_SetAudioReport(
                 hStaticNetAppBTHid->tHidTable[dev_index].handle,&audio_report[0], 7);

         if (status != 0)
         {
             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive = FALSE;

             if (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd > 0)
             {
                close(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd);
                hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd = -1;
             }
         }
        break;

     case NETAPP_HID_AUDIO_DATA:
         if(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive == TRUE)
         {
             STREAM_TO_UINT16(audiodata_hdr.usSeqNum, p_data);
             STREAM_TO_UINT16(audiodata_hdr.usTimestamp, p_data);
             STREAM_TO_UINT16(audiodata_hdr.usBufferState, p_data);
             STREAM_TO_UINT16(audiodata_hdr.usReserved, p_data);
             STREAM_TO_UINT16(audiodata_hdr.usDataCnt, p_data);

             hh_voice_size = audiodata_hdr.usDataCnt;

             hh_voice_size = hh_voice_size *
                     (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.bitsPerSample / 8) ;

             write(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd, p_data ,hh_voice_size);
             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.ulVoiceSize += hh_voice_size;
         }
         break;


     case NETAPP_HID_AUDIO_RC_MIC_STOP:
         if (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd > 0)
         {
             close(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd);
             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd = -1;
         }

         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive = FALSE;

         /* Send APP_HH_AUDIO_MIC_STOP */
         p_data = &audio_report[0];
         UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_MIC_STOP); /* Report format */
         UINT8_TO_STREAM(p_data, 0); /* channel */
         UINT8_TO_STREAM(p_data, 1); /* mode 1: send stop in response of stop req from remote */
         UINT16_TO_STREAM(p_data, 0); /* usReserved */
         UINT16_TO_STREAM(p_data, 0); /* data count */

         NETAPP_MSG(("Send NETAPP_HID_AUDIO_MIC_STOP"));

         status = NetAppBluetoothHid_P_SetAudioReport(
                 hStaticNetAppBTHid->tHidTable[dev_index].handle,&audio_report[0], 7);

         NETAPP_DO_CALLBACK(NETAPP_CB_BT_HID_VOICE_INFO, &hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat,
             sizeof(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat), NETAPP_BT_SERVICE_HID, NETAPP_SUCCESS, NETAPP_IFACE_BLUETOOTH);
         break;

     case NETAPP_HID_AUDIO_CODEC_READ_ACK:
         p_data++ ; /* channel */
         STREAM_TO_UINT8(codec_setting_id, p_data); /* setting ID */
         STREAM_TO_UINT16(codec_setting_len, p_data); /* data count */

         switch (codec_setting_id)
         {
             case NETAPP_HID_AUDIO_SAMP_FREQ_ID:
                 /* Size is 2: 1 byte for sample_freq + 1 Reserved byte*/
                 if(codec_setting_len == 2)
                 {
                     STREAM_TO_UINT8(sample_freq, p_data);
                     switch (sample_freq)
                     {
                         case NETAPP_HID_AUDIO_SAMP_FREQ_8K:
                         case NETAPP_HID_AUDIO_SAMP_FREQ_16K:
                         case NETAPP_HID_AUDIO_SAMP_FREQ_22K5:
                         case NETAPP_HID_AUDIO_SAMP_FREQ_32K:
                         case NETAPP_HID_AUDIO_SAMP_FREQ_48K:
                             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.sampleRate =
                                       (unsigned long) sample_freq*250 ;
                             break;

                         default:
                             NETAPP_ERR(("Unsupported sample frequency (%d) reported in codec settings ",
                                   sample_freq));
                             return;
                             break;
                      }

                     NETAPP_MSG(("Sample rate = %d",
                             (int) hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.sampleRate));

                     p_data = &audio_report[0];

                     /* Read number of bits per samples */
                     UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_CODEC_READ_REQ); /* Report format */
                     UINT8_TO_STREAM(p_data, 0); /* channel */
                     UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_BITS_PER_SAMP_ID); /* Setting Id = bits per sample */
                     UINT16_TO_STREAM(p_data, 0); /* data count */
                     NETAPP_MSG(("Send NETAPP_HID_AUDIO_BITS_PER_SAMP_ID"));

                     status = NetAppBluetoothHid_P_SetAudioReport(
                             hStaticNetAppBTHid->tHidTable[dev_index].handle,&audio_report[0], 5);
                 }
                 else
                 {
                     NETAPP_ERR(("Wrong codec setting length %d",codec_setting_len));
                 }
                 break;

             case NETAPP_HID_AUDIO_BITS_PER_SAMP_ID:
                 if(codec_setting_len == 2)
                 {
                     STREAM_TO_UINT8(bits_per_samp, p_data);

                     hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.bitsPerSample =
                               bits_per_samp;
                     NETAPP_MSG(("BitPerSample = %d",
                           (int) hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.bitsPerSample));
                 }
                 else
                 {
                     NETAPP_ERR(("Wrong codec setting length %d",codec_setting_len));
                 }
                 break;

             case NETAPP_HID_AUDIO_SAMP_FORMAT_ID:
             case NETAPP_HID_AUDIO_PGA_GAIN_ID:
             case NETAPP_HID_AUDIO_HP_FILTER_SET_ID:
                 NETAPP_MSG(("Setting [%d] not yet implemented",codec_setting_id));
                 break;

             default:
                 NETAPP_MSG(("Wrong setting ID",codec_setting_id));
                 return;
                 break;
         }

         if( (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.usReadCodecSettings &
                 NETAPP_HID_AUDIO_MINIMAL_CODEC_CONFIG) != NETAPP_HID_AUDIO_MINIMAL_CODEC_CONFIG)
         {
             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.usReadCodecSettings |= (1 << codec_setting_id);

             if( (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.usReadCodecSettings &
                     NETAPP_HID_AUDIO_MINIMAL_CODEC_CONFIG) == NETAPP_HID_AUDIO_MINIMAL_CODEC_CONFIG)
             {
                 /* Create file in read/write mode */
                 if(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd > 0)
                 {
                    close(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd);
                 }
                 remove(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename);

                 hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd =
                         open(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.tVoiceInfo.hidAudioFilename,
                                 O_RDWR | O_CREAT ,
                                 0666);

                 if (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd < 0)
                 {
                     NETAPP_ERR(("%s() Could not create file for audio dump", __FUNCTION__));
                 }

                 NETAPP_MSG(("Send NETAPP_HID_AUDIO_MIC_START"));
                 p_data = &audio_report[0];
                 UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_MIC_START); /* Report format */
                 UINT8_TO_STREAM(p_data, 0); /* channel */
                 UINT8_TO_STREAM(p_data, 1); /* mode 1: send start in response of start req from remote */

                 hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive = TRUE;

                 status = NetAppBluetoothHid_P_SetAudioReport(
                         hStaticNetAppBTHid->tHidTable[dev_index].handle,&audio_report[0], 7);
             }
         }
         break;

     default:
         NETAPP_ERR(("Wrong audio format value %d - Close the file",p_report_data->data[NETAPP_HID_AUDIO_FORMAT_OFFSET]));
         if (hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd > 0)
         {
             close(hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd);
             hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.lVoiceFd = -1;
         }
         hStaticNetAppBTHid->tHidTable[dev_index].tAudioFormat.bIsVoiceStreamingActive = FALSE;

         /* Send APP_HH_AUDIO_MIC_STOP */
         p_data = &audio_report[0];
         UINT8_TO_STREAM(p_data, NETAPP_HID_AUDIO_MIC_STOP); /* Report format */
         UINT8_TO_STREAM(p_data, 0); /* channel */
         UINT8_TO_STREAM(p_data, 1); /* mode 1: send stop in response of stop req from remote */

         NETAPP_MSG(("Send NETAPP_HID_AUDIO_MIC_STOP"));
         status = NetAppBluetoothHid_P_SetAudioReport(
                 hStaticNetAppBTHid->tHidTable[dev_index].handle,&audio_report[0], 7);
         break;
    }

}

static void NetAppBluetoothHid_P_UipcCallback(
    BT_HDR                  *p_msg)
{
    unsigned int        i            = 0;
    unsigned int        j            = 0;
    tBSA_HH_UIPC_REPORT *pUipcReport = (tBSA_HH_UIPC_REPORT*)p_msg;
    unsigned int audio_size;
    char *p_data;

    if (hStaticNetAppBTHid == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }

    NETAPP_MSG(("%s() Handle:%d BdAddr:%02X-%02X-%02X-%02X-%02X-%02X", __FUNCTION__, pUipcReport->report.handle,
         pUipcReport->report.bd_addr[0], pUipcReport->report.bd_addr[1], pUipcReport->report.bd_addr[2],
         pUipcReport->report.bd_addr[3], pUipcReport->report.bd_addr[4], pUipcReport->report.bd_addr[5]));

    NETAPP_MSG(("\tMode:%d SubClass:0x%x CtryCode:%d len:%d event:%d", pUipcReport->report.mode, pUipcReport->report.sub_class,
            pUipcReport->report.ctry_code, pUipcReport->report.report_data.length, pUipcReport->hdr.event));


    for (i = 0; i < NETAPP_HID_DEVICE_MAX; i++)
    {
        if ( (hStaticNetAppBTHid->tHidTable[i].in_use) &&
             (hStaticNetAppBTHid->tHidTable[i].handle == pUipcReport->report.handle) )
        {
            if ( (hStaticNetAppBTHid->tHidTable[i].tAudioFormat.bIsAudioDevice == TRUE) &&
                    (pUipcReport->report.report_data.data[0] == NETAPP_HID_AUDIO_REPORT_ID) )
            {
                NetAppBluetoothHid_P_AudioHandler(&pUipcReport->report.report_data,i);
                break;
            }
#if 0
            printf("Report Dump:");
            for(j=0;j<pUipcReport->report.report_data.length;j++)
            {
                printf(" 0x%02x", pUipcReport->report.report_data.data[j]);
            }
            printf("\n");

#endif

            if (hStaticNetAppBTHid->tHidTable[i].bthid_fd > 0)
            {
                if (write(hStaticNetAppBTHid->tHidTable[i].bthid_fd, pUipcReport->report.report_data.data,
                        (size_t)pUipcReport->report.report_data.length) < 0 )
                {
                    NETAPP_ERR(("%s(): Problem writing the data to the HID Driver", __FUNCTION__));
                }
                break;
            }
        }
    }

    GKI_freebuf(p_msg);
}


static NETAPP_RETCODE NetAppBluetoothHid_P_AddDev(
    NETAPP_BT_HID_HANDLE    hNetAppBTHid,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    tBSA_HH_ADD_DEV tAddDevParams;
    NETAPP_HW_ADDR tAddr;

    BSA_HhAddDevInit(&tAddDevParams);

    NetAppAToHwAddr(pDevInfo->cAddr, tAddr);

    memcpy(tAddDevParams.bd_addr, tAddr, sizeof(tAddDevParams.bd_addr));
    tAddDevParams.attr_mask = 0;
    /* TODO: Need the sub class ??
    tAddDevParams.sub_class = 0; app_xml_remote_devices_db[index].class_of_device[2];
    */
#if 0
    /* The only way to know if this device supports UCD/BRR, is to check
     * its name => VIZIO RC or UEIC RC (name to be confirmed) */
    if ((strcmp((char *) app_xml_remote_devices_db[index].name, APP_HH_VIZIO_RC_NAME) == 0) ||
        (strcmp((char *) app_xml_remote_devices_db[index].name, APP_HH_UEIC_RC_NAME) == 0))
    {
        APP_INFO0("Enable UCD and BRR for this HID Device");
        tAddDevParams.feature_mask = BSA_HH_UCD_FEATURE | BSA_HH_BRR_FEATURE;
        /* Read BRR data file for this device */
        app_hh_read_brr_data(app_xml_remote_devices_db[index].bd_addr,
                &tAddDevParams.brr_cfg);
    }
#endif

    memcpy(tAddDevParams.dscp_data.data, pDevInfo->tHidInfo.usData, pDevInfo->tHidInfo.ulLength);
    tAddDevParams.dscp_data.length = pDevInfo->tHidInfo.ulLength;

    NETAPP_MSG(("HH ADD DEV [%s]",pDevInfo->cAddr));
    NETAPP_BT_BSA_CHECK(BSA_HhAddDev(&tAddDevParams), tRetCode);

    NETAPP_DO_CALLBACK(NETAPP_CB_CONNECT, pDevInfo, sizeof(NETAPP_BT_DEV_INFO), NETAPP_BT_SERVICE_HID, tRetCode, NETAPP_IFACE_BLUETOOTH);

err_out:
    return tRetCode;
}


static NETAPP_RETCODE NetAppBluetoothHid_P_DescWrite(
    int                     fd,
    char                    *pBuf,
    int                     length)
{
    BTHID_CONTROL ctrl;

    if ( (fd <= 0) || (pBuf == NULL) || (length > sizeof(ctrl.data)) )
    {
        return NETAPP_INVALID_PARAMETER;
    }

     /* Prepare ioctl structure */
     ctrl.size =  length;
     memcpy(&ctrl.data, pBuf, ctrl.size);

     return (ioctl(fd, 1, (uint32_t)&ctrl) < 0) ? NETAPP_FAILURE : NETAPP_SUCCESS;
}


static void NetAppBluetoothHid_P_AutoPairTimer(
    void            *pParam)
{
    tBSA_DISC_START     tDiscStartParam;

    NETAPP_MSG(("%s() -- Entered!", __FUNCTION__));

    BSA_DiscStartInit(&tDiscStartParam);

    tDiscStartParam.services = BSA_HID_SERVICE_MASK;
    tDiscStartParam.cback = NetAppBluetoothHid_P_BsaAutoPairDiscoveryCallback;
    tDiscStartParam.nb_devices = 0;
    tDiscStartParam.duration = 2;

    BSA_DiscStart(&tDiscStartParam);
}


static void NetAppBluetoothHid_P_BsaAutoPairDiscoveryCallback(
    tBSA_DISC_EVT           event,
    tBSA_DISC_MSG           *pBsaDevInfo)
{
    NETAPP_MSG(("%s() -- Entered!", __FUNCTION__));
    switch (event)
    {
    /* a New Device has been discovered */
    case BSA_DISC_NEW_EVT:
    case BSA_DISC_DEV_INFO_EVT: /* Discovery Device Info */
    {
        NETAPP_MSG(("%s(): Discovered %s", __FUNCTION__, pBsaDevInfo->disc_new.name));
        /* Auto Pairing HACK: Only auto-pair to certain remote(s) */
        if ( (strstr(pBsaDevInfo->disc_new.name, "GFRM100") != NULL) ||
             (strstr(pBsaDevInfo->disc_new.name, "Philips") != NULL) ||
             (strstr(pBsaDevInfo->disc_new.name, "Smart Touch Control") != NULL) )
        {
            NETAPP_BT_DEV_INFO  tDevInfo;
            memset(&tDevInfo, 0, sizeof(tDevInfo));

            NetAppHwAddrToA(pBsaDevInfo->disc_new.bd_addr, tDevInfo.cAddr, sizeof(tDevInfo.cAddr));
            strncpy(tDevInfo.cName, pBsaDevInfo->disc_new.name, sizeof(tDevInfo.cName)-1);
            tDevInfo.cName[sizeof(tDevInfo.cName)-1] = '\0';
            tDevInfo.lRssi = pBsaDevInfo->disc_new.rssi;
            tDevInfo.ulServiceMask = NETAPP_BT_SERVICE_HID;

            NETAPP_ERR(("%s(): Auto Pairing to %s(%s)", __FUNCTION__, tDevInfo.cName, tDevInfo.cAddr));
            NetAppBluetoothHidPrivateConnect(hStaticNetAppBTHid, &tDevInfo);
        }
        break;
    }

    default:
        break;
    }
    NETAPP_MSG(("%s() -- Leaving!", __FUNCTION__));
}

