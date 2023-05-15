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
 * Module Description:
 *
 * Revision History:
 *
 ***************************************************************************/
 /*
 * NetApp Bluetooth AV Source API
 */

#include "netapp_bt_av.h"
#include "bsa_api.h"
#include "linux/input.h"

/* TODO:  Grab from bsa_int.h somehow */
#ifndef BSA_AV_UIPC_BUFFER_SIZE
#define BSA_AV_UIPC_BUFFER_SIZE 20000
#endif

typedef struct
{
    BOOLEAN                       bInUse;
    BOOLEAN                       bIsStreamActive;
    tBSA_AV_HNDL                  handle;
    NETAPP_BT_DEV_INFO            tDevInfo;

} NETAPP_AV_DEVICE;

struct sNETAPP_BT_AV
{
    NETAPP_AV_DEVICE        tAvTable[NETAPP_AV_DEVICE_MAX];
    tUIPC_CH_ID             tUipcChannel;
    NETAPP_SETTINGS_HANDLE  hNetAppSettings;
    uint32_t                ulAutoPairTimer;
};

typedef struct {
    uint8_t ucBsaKey;
    uint8_t ucLinuxKey;
} INPUT_KEY_TABLE;

static INPUT_KEY_TABLE KeyLookupTable[] = {
    { BSA_AV_ID_SELECT,     KEY_ENTER},
    { BSA_AV_ID_UP,         KEY_UP},
    { BSA_AV_ID_DOWN,       KEY_DOWN},
    { BSA_AV_ID_LEFT,       KEY_LEFT},
    { BSA_AV_ID_RIGHT,      KEY_RIGHT},
    { BSA_AV_ID_RIGHT_UP,   KEY_RIGHT},
    { BSA_AV_ID_RIGHT_DOWN, KEY_RIGHT},
    { BSA_AV_ID_LEFT_UP,    KEY_LEFT},
    { BSA_AV_ID_LEFT_DOWN,  KEY_LEFT},
    { BSA_AV_ID_ROOT_MENU,  KEY_MENU},
    { BSA_AV_ID_SETUP_MENU, KEY_SETUP},
    //{ BSA_AV_ID_CONT_MENU,  KEY_},
    //{ BSA_AV_ID_FAV_MENU, KEY_},
    { BSA_AV_ID_EXIT,       KEY_ESC},
    { BSA_AV_ID_0,          KEY_0},
    { BSA_AV_ID_1,          KEY_1},
    { BSA_AV_ID_2,          KEY_2},
    { BSA_AV_ID_3,          KEY_3},
    { BSA_AV_ID_4,          KEY_4},
    { BSA_AV_ID_5,          KEY_5},
    { BSA_AV_ID_6,          KEY_6},
    { BSA_AV_ID_7,          KEY_7},
    { BSA_AV_ID_8,          KEY_8},
    { BSA_AV_ID_9,          KEY_9},
    { BSA_AV_ID_DOT,        KEY_DOT},
    { BSA_AV_ID_ENTER,      KEY_ENTER},
    { BSA_AV_ID_CLEAR,      KEY_BACK},
    { BSA_AV_ID_CHAN_UP,    KEY_UP},
    { BSA_AV_ID_CHAN_DOWN,  KEY_DOWN},
    { BSA_AV_ID_PREV_CHAN,  KEY_BACK},
    //{ BSA_AV_ID_SOUND_SEL,  KEY_},
    //{ BSA_AV_ID_INPUT_SEL,  KEY_},
    //{ BSA_AV_ID_DISP_INFO,  KEY_},
    //{ BSA_AV_ID_HELP,       KEY_},
    //{ BSA_AV_ID_PAGE_UP, KEY_},
    //{ BSA_AV_ID_PAGE_DOWN, KEY_},
    { BSA_AV_ID_POWER,      KEY_POWER},
    { BSA_AV_ID_VOL_UP,     KEY_VOLUMEUP},
    { BSA_AV_ID_VOL_DOWN,   KEY_VOLUMEDOWN},
    { BSA_AV_ID_MUTE,       KEY_MUTE},
    { BSA_AV_ID_PLAY,       KEY_PLAY},
    { BSA_AV_ID_STOP,       KEY_STOP},
    { BSA_AV_ID_PAUSE,      KEY_PAUSE},
    { BSA_AV_ID_RECORD,     KEY_RECORD},
    { BSA_AV_ID_REWIND,     KEY_REWIND},
    { BSA_AV_ID_FAST_FOR,   KEY_FASTFORWARD},
    { BSA_AV_ID_EJECT,      KEY_EJECTCD},
    { BSA_AV_ID_FORWARD,    KEY_FORWARD},
    { BSA_AV_ID_BACKWARD,   KEY_BACK},
    //{ BSA_AV_ID_ANGLE,      KEY_},
    //{ BSA_AV_ID_SUBPICT,    KEY_},
    { BSA_AV_ID_F1,         KEY_F1},
    { BSA_AV_ID_F2,         KEY_F2},
    { BSA_AV_ID_F3,         KEY_F3},
    { BSA_AV_ID_F4,         KEY_F4},
    { BSA_AV_ID_F5,         KEY_F5}
};

static void NetAppBluetoothAv_P_AvCallback(
    tBSA_AV_EVT             event,
    tBSA_AV_MSG             *p_data);


/* Default: Target (receive command) and Controller (send command) enabled */
#define APP_AV_FEAT_MASK (BSA_AV_FEAT_RCTG | BSA_AV_FEAT_RCCT)
#define APP_AV_USE_RC TRUE

/* Need static handle for Bluetooth callbacks :-( */
static NETAPP_BT_AV_HANDLE    hStaticNetAppBTAv = NULL;

/*******************************************************************************
 *                 Public (but Internal to NetApp) Bluetooth AV API's
 ******************************************************************************/
NETAPP_RETCODE NetAppBluetoothAvPrivateInit(
    NETAPP_BT_AV_HANDLE    *phNetAppBTAv,
    NETAPP_SETTINGS_HANDLE  hNetAppSettings)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_BT_AV_HANDLE     hNetAppBTAv = NULL;
    tBSA_AV_ENABLE          tParams_enable;
    tBSA_AV_REGISTER        tParams_register;
    int                     index;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    NETAPP_PRIV_HANDLE_CHECK(phNetAppBTAv);

    hNetAppBTAv = (NETAPP_BT_AV_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_BT_AV));
    if (hNetAppBTAv == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
       return NETAPP_OUT_OF_MEMORY;
    }

    memset(hNetAppBTAv, 0, sizeof(struct sNETAPP_BT_AV));

    BSA_AvEnableInit(&tParams_enable);
    tParams_enable.features = APP_AV_FEAT_MASK;
    tParams_enable.p_cback = NetAppBluetoothAv_P_AvCallback;

    NETAPP_BT_BSA_CHECK(BSA_AvEnable(&tParams_enable), tRetCode);

    for (index = 0; index < NETAPP_AV_DEVICE_MAX; index++)
    {
        BSA_AvRegisterInit(&tParams_register);

        NETAPP_BT_BSA_CHECK(BSA_AvRegister(&tParams_register), tRetCode);

        /* Save register data */
        hNetAppBTAv->tAvTable[index].handle  = tParams_register.handle;

        if(hNetAppBTAv->tUipcChannel != tParams_register.uipc_channel)
        {
            hNetAppBTAv->tUipcChannel = tParams_register.uipc_channel; /* Save UIPC channel */

            if(UIPC_Open(hNetAppBTAv->tUipcChannel, NULL) == FALSE)
            {
                NETAPP_ERR(("Unable to open UIPC channel"));
                goto err_out;
            }
        }
    }

    hNetAppBTAv->hNetAppSettings   = hNetAppSettings;
    hStaticNetAppBTAv              = hNetAppBTAv;
    *phNetAppBTAv                  = hNetAppBTAv;

err_out:
    if ( (hNetAppBTAv != NULL) && (tRetCode != NETAPP_SUCCESS) )
    {
        NetAppBluetoothAvPrivateDeinit(hNetAppBTAv);
    }

    return tRetCode;
}


NETAPP_RETCODE NetAppBluetoothAvPrivateDeinit(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv)
{
    tBSA_AV_DISABLE dis_param;
    tBSA_AV_DEREGISTER dereg_param;
    unsigned int    i = 0;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTAv);

    for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
    {
        tBSA_AV_CLOSE   tCloseParams;
        if ( hNetAppBTAv->tAvTable[i].bInUse )
        {
            NETAPP_MSG(("%s(): Disconnecting Av device:%d", __FUNCTION__, hNetAppBTAv->tAvTable[i].handle));
            BSA_AvCloseInit(&tCloseParams);

            tCloseParams.handle = hNetAppBTAv->tAvTable[i].handle;
            BSA_AvClose(&tCloseParams);
        }

        BSA_AvDeregisterInit(&dereg_param);
        dereg_param.handle = hNetAppBTAv->tAvTable[i].handle;

        BSA_AvDeregister(&dereg_param);

        UIPC_Close(hNetAppBTAv->tUipcChannel);  /* Close UIPC channel */
    }

    BSA_AvDisableInit(&dis_param);
    BSA_AvDisable(&dis_param);

    NetAppOSMemFree(hNetAppBTAv);
    hStaticNetAppBTAv = NULL;

    return NETAPP_SUCCESS;
}

NETAPP_RETCODE NetAppBluetoothAvPrivateConnect(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv,
    NETAPP_BT_DEV_INFO     *pDevInfo)
{
    tBSA_AV_OPEN        tOpenParams;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_AV_DEVICE    *pAvDevInfo = NULL;
    unsigned int        i;
    NETAPP_HW_ADDR tAddr;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTAv);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    BSA_AvOpenInit(&tOpenParams);

    NetAppAToHwAddr(pDevInfo->cAddr, tAddr);
    memcpy(tOpenParams.bd_addr, tAddr, sizeof(tOpenParams.bd_addr));

    for (i = 0 ; i < NETAPP_AV_DEVICE_MAX ; i++)
    {
        if(!hStaticNetAppBTAv->tAvTable[i].bInUse)
        {
            tOpenParams.handle        = hStaticNetAppBTAv->tAvTable[i].handle;
            tOpenParams.use_rc    = APP_AV_USE_RC;

            /* Flag we request a connection for this device index */
            hStaticNetAppBTAv->tAvTable[i].bInUse = true;
            hStaticNetAppBTAv->tAvTable[i].tDevInfo = *pDevInfo;

            NETAPP_MSG(("CALL BSA_AvOpen on handle [%d]",tOpenParams.handle));

            NETAPP_BT_BSA_CHECK(BSA_AvOpen(&tOpenParams), tRetCode);
            NETAPP_MSG(("%s(): Connecting to AV device:%s", __FUNCTION__, pDevInfo->cName));
            break;
        }
    }

    if(i == NETAPP_AV_DEVICE_MAX)
    {
        tRetCode = NETAPP_OUT_OF_MEMORY;
        NETAPP_ERR(("%s(): Maximum AV device number in used", __FUNCTION__));
    }

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothAvPrivateDisconnect(
    NETAPP_BT_AV_HANDLE    hNetAppBTAv,
    NETAPP_BT_DEV_INFO      *pDevInfo)
{
    unsigned int    i = 0;
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    tBSA_AV_CLOSE   tCloseParams;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTAv);
    NETAPP_PRIV_HANDLE_CHECK(pDevInfo);

    NETAPP_MSG(("%s(): Entered", __FUNCTION__));

    BSA_AvCloseInit(&tCloseParams);

    for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
    {
        if (strncmp(pDevInfo->cAddr, hNetAppBTAv->tAvTable[i].tDevInfo.cAddr, strlen(pDevInfo->cAddr)) == 0)
        {
            tCloseParams.handle = hNetAppBTAv->tAvTable[i].handle;
            NETAPP_MSG(("Found Av device to disconnect [handle %d]\n",tCloseParams.handle));
            NETAPP_MSG(("Reghandle = %d\n",hNetAppBTAv->tAvTable[i].handle));
            break;
        }
    }
    if (i < NETAPP_AV_DEVICE_MAX)
    {
        NETAPP_BT_BSA_CHECK(BSA_AvClose(&tCloseParams), tRetCode);
    }

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothAvPrivateStart(
    NETAPP_BT_AV_HANDLE     hNetAppBTAv,
    bool                    bSynchronous,
    NETAPP_BT_AUDIO_FORMAT  *pBtAudioFormat)
{
    tBSA_AV_START       tStartParams;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    unsigned int        i;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTAv);

    for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
    {
        if (hNetAppBTAv->tAvTable[i].bInUse && !hNetAppBTAv->tAvTable[i].bIsStreamActive)
        {
            /* start AV stream */
            BSA_AvStartInit(&tStartParams);

            tStartParams.media_feeding.format = BSA_AV_CODEC_PCM;
            tStartParams.media_feeding.cfg.pcm.sampling_freq = pBtAudioFormat->ulSampleRate;
            tStartParams.media_feeding.cfg.pcm.num_channel = (uint8_t)pBtAudioFormat->tMode;
            tStartParams.media_feeding.cfg.pcm.bit_per_sample = pBtAudioFormat->ucBitsPerSample;

            /* In this application we are not feeding the stream real time so we must be blocking */
            UIPC_Ioctl(hNetAppBTAv->tUipcChannel, bSynchronous ? UIPC_WRITE_NONBLOCK : UIPC_WRITE_BLOCK, NULL);

            tStartParams.feeding_mode = bSynchronous ? BSA_AV_FEEDING_SYNCHRONOUS : BSA_AV_FEEDING_ASYNCHRONOUS;
            tStartParams.latency = 40;
            NETAPP_BT_BSA_CHECK(BSA_AvStart(&tStartParams), tRetCode);
            NETAPP_MSG(("%s(): Starting stream to AV device:%s", __FUNCTION__, hNetAppBTAv->tAvTable[i].tDevInfo.cName));
        }
    }
err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothAvPrivateStop(
    NETAPP_BT_AV_HANDLE     hNetAppBTAv)
{
    tBSA_AV_STOP        tStopParams;
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    unsigned int        i;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppBTAv);

    /* stop AV stream */
    BSA_AvStopInit(&tStopParams);

    tStopParams.pause = FALSE;

    BSA_AvStop(&tStopParams);
    NETAPP_MSG(("%s(): Stopping stream to AV devices", __FUNCTION__));

err_out:
    return tRetCode;
}

NETAPP_RETCODE NetAppBluetoothAvPrivateSendAudioBuffer(
    NETAPP_BT_HANDLE        hNetAppBT,
    void                    *pBuf,
    uint32_t                ulLength)
{
    uint8_t         *ptempBuf = (unsigned char*)pBuf;
    uint32_t        ulSent = 0;
    uint32_t        ulWriteLen = (ulLength <= BSA_AV_UIPC_BUFFER_SIZE) ? ulLength : BSA_AV_UIPC_BUFFER_SIZE;

    NETAPP_RETCODE tRetCode = NETAPP_SUCCESS;
    NETAPP_PRIV_HANDLE_CHECK(hNetAppBT);
    NETAPP_PRIV_HANDLE_CHECK(pBuf);

    while (ulSent < ulLength)
    {
        if (UIPC_Send(hStaticNetAppBTAv->tUipcChannel, 0, (unsigned char*) ptempBuf, ulWriteLen) != TRUE)
        {
            NETAPP_ERR(("%s(): UIPC send could not send data length=%d", __FUNCTION__, ulWriteLen));
            tRetCode =  NETAPP_FAILURE;
            break;
        }
        ptempBuf += BSA_AV_UIPC_BUFFER_SIZE;
        ulSent+= ulWriteLen;
        ulWriteLen = ((ulLength - ulSent) <= BSA_AV_UIPC_BUFFER_SIZE) ? (ulLength - ulSent) : BSA_AV_UIPC_BUFFER_SIZE;
    }

err_out:
    return tRetCode;
}


/*******************************************************************************
 *                       Static NetApp Bluetooth AV Source Functions
 ******************************************************************************/
static void NetAppBluetoothAv_P_AvCallback(
    tBSA_AV_EVT             event,
    tBSA_AV_MSG             *p_data)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_AV_DEVICE    *pAvDevInfo = NULL;
    unsigned int        i = 0;
    tBSA_AV_START       tStartParams;
    int  status;
    NETAPP_CB_TYPE      tCallback = NETAPP_CB_INVALID;
    NETAPP_INPUT_INFO   keydata;
    void                *pData = NULL;
    NETAPP_HW_ADDR tAddr;
    int dev_index;
    DEV_CLASS class_of_device;
    uint32_t            ulDataLength = sizeof(NETAPP_AV_DEVICE);

    if (hStaticNetAppBTAv == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter", __FUNCTION__));
        return;
    }

    switch (event)
    {
    case BSA_AV_OPEN_EVT: /* Connection Open*/
    {
        tRetCode = (p_data->open.status == BSA_SUCCESS) ? NETAPP_SUCCESS : NETAPP_FAILURE;

        NETAPP_MSG(("%s(): BSA_AV_OPEN_EVT: status%d, handle:%d",  __FUNCTION__, p_data->open.status, p_data->open.handle));

        /* Look if this is an Open from a Connect() request */
        for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
        {
            if(hStaticNetAppBTAv->tAvTable[i].bInUse)
            {
                NetAppAToHwAddr(hStaticNetAppBTAv->tAvTable[i].tDevInfo.cAddr, tAddr);

                if ( memcmp(tAddr, p_data->open.bd_addr, sizeof(NETAPP_HW_ADDR)) == 0)
                {
                    pAvDevInfo = &hStaticNetAppBTAv->tAvTable[i];
                    break;
                }
            }
        }

        /* Open request from remote device */
        if (pAvDevInfo == NULL)
        {
            for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
            {
                if (!hStaticNetAppBTAv->tAvTable[i].bInUse)
                {
                    pAvDevInfo = &hStaticNetAppBTAv->tAvTable[i];
                    pAvDevInfo->bInUse = true;
                    break;
                }
            }
        }

        tCallback = NETAPP_CB_CONNECT;

        if ( (pAvDevInfo == NULL) || (p_data->open.status != BSA_SUCCESS) )
        {
            tRetCode = NETAPP_FAILURE;

            if(pAvDevInfo != NULL)
            {
                pAvDevInfo->bInUse = false;
            }
            break;
        }

        NetAppSettingsPrivateGetSavedBtDev( hStaticNetAppBTAv->hNetAppSettings, p_data->open.bd_addr, &pAvDevInfo->tDevInfo);
        pAvDevInfo->handle     = p_data->open.handle;
        pAvDevInfo->tDevInfo.ulTrustedServiceMask |= BSA_A2DP_SERVICE_MASK | BSA_AVRCP_SERVICE_MASK;
        NetAppSettingsPrivateSaveBtDev(hStaticNetAppBTAv->hNetAppSettings, &pAvDevInfo->tDevInfo);
        NETAPP_MSG(("%s(): Headset connection was successful!", __FUNCTION__));
        pData = &pAvDevInfo->tDevInfo;
       break;
    }


    case BSA_AV_CLOSE_EVT: /* Connection Closed */
        NETAPP_MSG(("BSA_AV_CLOSE_EVT: status:%d, handle:%d", p_data->close.status, p_data->close.handle));

        tCallback = NETAPP_CB_DISCONNECT;
        if (p_data->close.status == BSA_SUCCESS)
        {
            for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
            {
                if (hStaticNetAppBTAv->tAvTable[i].handle == p_data->close.handle)
                {
                    NETAPP_MSG(("%s(): Found open reference for %d", __FUNCTION__, i));
                    pData  = &hStaticNetAppBTAv->tAvTable[i].tDevInfo;
                    hStaticNetAppBTAv->tAvTable[i].bInUse = false;
                    break;
                }
            }
        }
        else
        {
            NETAPP_MSG(("BSA_AvClose error status %d",p_data->close.status));
            tRetCode = NETAPP_FAILURE;
        }
        break;


    case BSA_AV_START_EVT:
        NETAPP_MSG(("%s(): BSA_AV_START_EVT: status:0x%x",__FUNCTION__, p_data->start.status));
        if(p_data->start.status == BSA_SUCCESS)
        {
            for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
            {
                if (hStaticNetAppBTAv->tAvTable[i].bInUse)
                {
                    hStaticNetAppBTAv->tAvTable[i].bIsStreamActive = true;
                }
            }
        }
        break;


    case BSA_AV_STOP_EVT:
        NETAPP_MSG(("%s(): BSA_AV_STOP_EVT: status:0x%x",__FUNCTION__, p_data->stop.pause));
        if(p_data->start.status == BSA_SUCCESS)
        {
            for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
            {
                if (hStaticNetAppBTAv->tAvTable[i].bInUse)
                {
                    hStaticNetAppBTAv->tAvTable[i].bIsStreamActive = false;
                }
            }
        }
        break;


    case BSA_AV_RC_OPEN_EVT:
        /* Some headsets will only connect by AVRCP event so we need to handle that here */
        NETAPP_MSG(("%s(): BSA_AV_RC_OPEN_EVT: status:0x%x",__FUNCTION__, p_data->rc_open.status));

        /* Look if this is an Open from a Connect() request */
        for (i = 0; i < NETAPP_AV_DEVICE_MAX; i++)
        {
            if(hStaticNetAppBTAv->tAvTable[i].bInUse)
            {
                NetAppAToHwAddr(hStaticNetAppBTAv->tAvTable[i].tDevInfo.cAddr, tAddr);

                if (memcmp(tAddr, p_data->rc_open.peer_addr, sizeof(NETAPP_HW_ADDR)) == 0)
                {
                    pAvDevInfo = &hStaticNetAppBTAv->tAvTable[i];
                    break;
                }
            }
        }

        /* Open request from remote device */
        if (pAvDevInfo == NULL)
        {
            NETAPP_BT_DEV_INFO tDevInfo;
            if (NetAppSettingsPrivateGetSavedBtDev(hStaticNetAppBTAv->hNetAppSettings, p_data->rc_open.peer_addr, &tDevInfo) == NETAPP_SUCCESS)
            {
                NetAppBluetoothAvPrivateConnect(hStaticNetAppBTAv, &tDevInfo);
            }
        }
        break;


    case BSA_AV_RC_CLOSE_EVT:
        NETAPP_MSG(("%s(): BSA_AV_RC_CLOSE_EVT: handle:0x%x",__FUNCTION__, p_data->rc_close.rc_handle));
        break;


    case BSA_AV_REMOTE_CMD_EVT:
    {
        int i = 0;
        NETAPP_MSG(("%s(): BSA_AV_REMOTE_CMD_EVT: Key:%d State:%d",__FUNCTION__, p_data->remote_cmd.rc_id, p_data->remote_cmd.key_state));

        for (i = 0; i < (sizeof(KeyLookupTable)/sizeof(KeyLookupTable[0])); i++)
        {
            if (KeyLookupTable[i].ucBsaKey == p_data->remote_cmd.rc_id)
            {
                keydata.ulKey = KeyLookupTable[i].ucLinuxKey;
                keydata.bPressed =  (p_data->remote_cmd.key_state == 0);
                tCallback = NETAPP_CB_INPUT_EVENT;
                pData = &keydata;
                ulDataLength = sizeof(NETAPP_INPUT_INFO);
                break;
            }
        }
        if (i == (sizeof(KeyLookupTable)/sizeof(KeyLookupTable[0])))
        {
            NETAPP_ERR(("%s(): Received unhandled input %d", __FUNCTION__, p_data->remote_cmd.rc_id));
        }
        break;
    }


    case BSA_AV_REMOTE_RSP_EVT:
        NETAPP_MSG(("%s(): BSA_AV_REMOTE_RSP_EVT: handle:0x%x",__FUNCTION__, p_data->remote_rsp.rc_handle));
        break;


    case BSA_AV_META_MSG_EVT:
        NETAPP_MSG(("%s(): BSA_AV_META_MSG_EVT: handle:0x%x",__FUNCTION__, p_data->meta_msg.rc_handle));
        break;


    default:
        NETAPP_ERR(("%s(): bad event:%d", __FUNCTION__, event));
        break;
    }

err_out:
    if (tCallback != NETAPP_CB_INVALID)
    {
        NETAPP_DO_CALLBACK(tCallback, pData, ulDataLength, NETAPP_BT_SERVICE_A2DP, tRetCode, NETAPP_IFACE_BLUETOOTH);
    }
}
