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
 * $brcm_Workfile: netapp_zeroconf.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/1/11 1:29p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_zeroconf.c $
 * 
 * 2   9/1/11 1:29p steven
 * SW7425-1223: Fixed memory Leaks in NetApp
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp Zero Configuration (Bonjour) Private APIs
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal Zeroconf Implementation using Avahi
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_priv.h"
#include "netapp_list.h"
#include "netapp_zeroconf.h"

#include <avahi-core/core.h>
#include <avahi-core/publish.h>
#include <avahi-core/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/alternative.h>
#include <avahi-common/error.h>
#include <avahi-core/log.h>

#define NETAPP_MAX_ZEROCONF_SERVICES    10
#define NETAPP_MESSAGE_QUEUE_LENGTH     2
#define NETAPP_ZEROCONF_TYPELEN         32
#define NETAPP_ZEROCONF_POLL_SLEEP_MS   100


typedef struct
{
    bool        bStart;
    char        cServiceType[NETAPP_ZEROCONF_TYPELEN];
} NETAPP_ZEROCONF_MSG;


typedef struct sSERVICE_BROWSER_NODE
{
    NETAPP_LIST_NODE                tNode;          // Must be first
    char                            cServiceType[NETAPP_ZEROCONF_TYPELEN]; // Must be second
    AvahiSServiceBrowser            *pServiceBrowser;
} SERVICE_BROWSE_NODE;


typedef struct sSERVICE_INFO_NODE
{
    NETAPP_LIST_NODE                tNode; // Must be first
    NETAPP_ZEROCONF_SERVICE_INFO    tInfo;
} SERVICE_INFO_NODE;


struct sNETAPP_ZEROCONF
{
    AvahiSEntryGroup                *pGroup;
    AvahiServer                     *pServer;
    AvahiServerState                tState;
    uint32_t                        ulServerThread;
    uint32_t                        ulMsgQId;
    AvahiSimplePoll                 *pSimplePoll;
    NETAPP_PRIV_HANDLE              hNetAppPriv;
    uint32_t                        ulSem;
    AvahiSServiceBrowser            *pServiceBrowser;
    NETAPP_LIST_HANDLE              hPublishedServiceList;
    NETAPP_LIST_HANDLE              hBrowsedServiceList;
    NETAPP_LIST_HANDLE              hBrowseList;
    char                            *pDeviceName;
    char                            *pManufacturer;
    char                            *pModelName;
    char                            *pModelNumber;
    char                            *pSerialNumber;
};

#define AVAHI_API_CHECK(api, server, tRetCode)                                      \
{                                                                                   \
    NETAPP_MSG(("%s() Calling:[%s]", __FUNCTION__, #api));                          \
    int result = api;                                                               \
    switch(result)                                                                  \
    {                                                                               \
    case AVAHI_OK:                                                                  \
        NETAPP_MSG(("%s() [%s] Returned Successful", __FUNCTION__, #api));          \
        tRetCode = NETAPP_SUCCESS;                                                  \
        break;                                                                      \
    default:                                                                        \
        tRetCode = NETAPP_FAILURE;                                                  \
        NETAPP_ERR(("%s() [%s] failed, Error '%s'!", __FUNCTION__, #api,            \
            avahi_strerror(avahi_server_errno(server))));                           \
        goto err_out;                                                               \
        break;                                                                      \
    }                                                                               \
}

/** Prototype for a user supplied log function */
static void NetAppZeroConf_P_LogFunc(
    AvahiLogLevel           level,
    const char               *txt);

static void NetAppZeroConf_P_Task(
    void                    *pParam);


static void NetAppZeroConf_P_ServerCB(
    AvahiServer             *pServer,
    AvahiServerState        tState,
    void                    *pData);


static void NetAppZeroConf_P_EntryGroupCB(
    AvahiServer             *pServer,
    AvahiSEntryGroup        *pGroup,
    AvahiEntryGroupState    tState,
    void                    *pData);


static NETAPP_RETCODE NetAppZeroConf_P_Publish(
    NETAPP_ZEROCONF_HANDLE          hNetAppZeroConf);


static NETAPP_RETCODE NetAppZeroConf_P_CreateServiceInfo(
    NETAPP_ZEROCONF_HANDLE      hNetAppZeroConf,
    const char                  *pName,
    const char                  *pType,
    uint32_t                    ulPort,
    char                        *pTxtRecord,
    uint32_t                    ulTxtLength,
    const char                  *pDomain,
    const char                  *pHost,
    NETAPP_IPV4_ADDR            tIpAddress,
    SERVICE_INFO_NODE           *pNode);


static NETAPP_RETCODE NetAppZeroConf_P_DeleteServiceInfo(
    NETAPP_ZEROCONF_HANDLE      hNetAppZeroConf,
    SERVICE_INFO_NODE           *pNode);


static void NetAppZeroConf_P_BrowseCB(
    AvahiSServiceBrowser    *pServiceBrowser,
    AvahiIfIndex            tInterface,
    AvahiProtocol           tProtocol,
    AvahiBrowserEvent       tEvent,
    const char              *name,
    const char              *type,
    const char              *domain,
    AvahiLookupResultFlags  flags,
    void                    *pData);


static void NetAppZeroConf_P_ResolveCB(
    AvahiSServiceResolver   *pServiceResolver,
    AvahiIfIndex            tInterface,
    AvahiProtocol           tProtocol,
    AvahiResolverEvent      tEvent,
    const char              *name,
    const char              *type,
    const char              *domain,
    const char              *host_name,
    const AvahiAddress      *address,
    uint16_t                port,
    AvahiStringList         *txt,
    AvahiLookupResultFlags  flags,
    void                    *pData);


NETAPP_RETCODE NetAppZeroConfPrivateInit(
    NETAPP_ZEROCONF_HANDLE  *phNetAppZeroConf,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings)
{
    NETAPP_RETCODE          tRetCode        = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = NULL;

    if ( (phNetAppZeroConf == NULL) || (pInitSettings == NULL) || (pSettings == NULL) )
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_ZEROCONF));
    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppZeroConf, 0, sizeof(struct sNETAPP_ZEROCONF));

    hNetAppZeroConf->ulSem = NetAppOSSemBCreate();
    if (hNetAppZeroConf->ulSem == 0)
    {
        NETAPP_ERR(("%s(): -- Cannot create sem!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    if (NetAppListCreate(&hNetAppZeroConf->hPublishedServiceList, sizeof(SERVICE_INFO_NODE)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    if (NetAppListCreate(&hNetAppZeroConf->hBrowsedServiceList, sizeof(SERVICE_INFO_NODE)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    if (NetAppListCreate(&hNetAppZeroConf->hBrowseList, sizeof(SERVICE_BROWSE_NODE)) != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s() Link List creation failed", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    hNetAppZeroConf->hNetAppPriv = hNetAppPriv;

    hNetAppZeroConf->ulMsgQId = NetAppOSMsgQCreate(NETAPP_MESSAGE_QUEUE_LENGTH, sizeof(NETAPP_ZEROCONF_MSG));
    if (hNetAppZeroConf->ulMsgQId == 0)
    {
        NETAPP_ERR(("%s(): Failed creating message queue!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    hNetAppZeroConf->pDeviceName     = strdup(pInitSettings->pDeviceName);
    hNetAppZeroConf->pManufacturer   = strdup(pInitSettings->pManufacturer);
    hNetAppZeroConf->pModelName      = strdup(pInitSettings->pModelName);
    hNetAppZeroConf->pModelNumber    = strdup(pInitSettings->pModelNumber);
    hNetAppZeroConf->pSerialNumber   = strdup(pInitSettings->pSerialNumber);
    *phNetAppZeroConf               = hNetAppZeroConf;

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppZeroConfPrivateDeInit(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);

    NetAppZeroConfPrivateStop(hNetAppZeroConf);

    if (hNetAppZeroConf->pDeviceName != NULL)
    {
        free(hNetAppZeroConf->pDeviceName);
    }

    if (hNetAppZeroConf->pManufacturer != NULL)
    {
        free(hNetAppZeroConf->pManufacturer);
    }

    if (hNetAppZeroConf->pModelName != NULL)
    {
        free(hNetAppZeroConf->pModelName);
    }

    if (hNetAppZeroConf->pSerialNumber != NULL)
    {
        free(hNetAppZeroConf->pSerialNumber);
    }

    if (hNetAppZeroConf->pModelNumber != NULL)
    {
        free(hNetAppZeroConf->pModelNumber);
    }

    if (hNetAppZeroConf->hPublishedServiceList != NULL)
    {
        NetAppListDestroy(hNetAppZeroConf->hPublishedServiceList);
    }

    if (hNetAppZeroConf->hBrowsedServiceList != NULL)
    {
        NetAppListDestroy(hNetAppZeroConf->hBrowsedServiceList);
    }

    if (hNetAppZeroConf->hBrowseList != NULL)
    {
        NetAppListDestroy(hNetAppZeroConf->hBrowseList);
    }

    if (hNetAppZeroConf->ulMsgQId != 0)
    {
        NetAppOSMsgQDelete(hNetAppZeroConf->ulMsgQId);
        hNetAppZeroConf->ulMsgQId = 0;
    }

    if (hNetAppZeroConf->ulSem != 0)
    {
        NetAppOSSemDelete(hNetAppZeroConf->ulSem);
        hNetAppZeroConf->ulSem = 0;
    }

    NetAppOSMemFree(hNetAppZeroConf);

    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppZeroConfPrivatePublish(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf,
    char                    *pName,
    char                    *pType,
    uint32_t                ulPort,
    char                    *pTxtRecord,
    uint32_t                ulTxtLength)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    SERVICE_INFO_NODE   *pNode   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    NETAPP_PRIV_HANDLE_CHECK(pName);
    NETAPP_PRIV_HANDLE_CHECK(pType);

    NETAPP_PRIV_SEM_TAKE(hNetAppZeroConf->ulSem, tRetCode);

    pNode = (SERVICE_INFO_NODE*)NetAppListAppend(hNetAppZeroConf->hPublishedServiceList);
    if (pNode)
    {
        NETAPP_PRIV_API_CHECK(NetAppZeroConf_P_CreateServiceInfo(hNetAppZeroConf, pName,
            pType, ulPort, pTxtRecord, ulTxtLength, NULL, NULL, 0, pNode), tRetCode);
    }


    if (hNetAppZeroConf->tState != AVAHI_SERVER_RUNNING)
    {
        NETAPP_MSG(("%s(): Avahi Servier is not running yet so queue up the service request!!", __FUNCTION__));
    }
    else
    {
        NETAPP_PRIV_API_CHECK(NetAppZeroConf_P_Publish(hNetAppZeroConf), tRetCode);
    }

err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppZeroConf->ulSem);
    return tRetCode;
}



NETAPP_RETCODE NetAppZeroConfPrivateBrowse(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf,
    char                    *pType)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_MSG tMessage;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    NETAPP_PRIV_HANDLE_CHECK(pType);

    NETAPP_PRIV_SEM_TAKE(hNetAppZeroConf->ulSem, tRetCode);

    tMessage.bStart = true;
    snprintf(tMessage.cServiceType, sizeof(tMessage.cServiceType), "%s", pType);

    NETAPP_PRIV_API_CHECK(NetAppOSMsgQSend(hNetAppZeroConf->ulMsgQId, &tMessage,
        sizeof(NETAPP_ZEROCONF_MSG), NETAPP_WAIT_FOREVER), tRetCode);

err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppZeroConf->ulSem);
    return tRetCode;
}


NETAPP_RETCODE NetAppZeroConfPrivateGetBrowseResults(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf)
{
    return NETAPP_NOT_SUPPORTED;
}


NETAPP_RETCODE NetAppZeroConfPrivateStart(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf)
{
    NETAPP_RETCODE      tRetCode        = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);

    if (hNetAppZeroConf->ulServerThread != 0)
    {
        NETAPP_ERR(("%s(): Server Already running!", __FUNCTION__));
        return NETAPP_FAILURE;
    }

    /* Start the background server */
    hNetAppZeroConf->ulServerThread = NetAppOSTaskSpawn("NetAppZeroConf_P_Task", NETAPP_OS_PRIORITY_LOW,
        128*1024, NetAppZeroConf_P_Task, hNetAppZeroConf);

    if (hNetAppZeroConf->ulServerThread == 0)
    {
        NETAPP_ERR(("%s()  Cannot create Zeroconf Poll thread Task\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }


err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppZeroConfPrivateStop(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    if (hNetAppZeroConf->ulServerThread == 0)
    {
        NETAPP_MSG(("%s(): Server Not Running", __FUNCTION__));
        return NETAPP_SUCCESS;
    }

    if (hNetAppZeroConf->pSimplePoll != NULL)
    {
        avahi_simple_poll_quit(hNetAppZeroConf->pSimplePoll);
    }

    NetAppOSTaskJoin(hNetAppZeroConf->ulServerThread);
    NetAppOSTaskDelete(hNetAppZeroConf->ulServerThread);
    hNetAppZeroConf->ulServerThread = 0;
    return NETAPP_SUCCESS;
}


static void NetAppZeroConf_P_Task(
    void                    *pParam)
{
    NETAPP_RETCODE          tRetCode        = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)pParam;
    AvahiServerConfig       tServerConfig;
    int                     lAvahiError     = 0;
    char                    *pTemp          = NULL;

    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    avahi_set_log_function(NetAppZeroConf_P_LogFunc);

    /* Allocate main loop object */
    hNetAppZeroConf->pSimplePoll = avahi_simple_poll_new();
    if (hNetAppZeroConf->pSimplePoll == NULL)
    {
        NETAPP_ERR(("%s(): Failed to create Avahi Simple Poll!", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    /* Let's set the host name for this server. */
    avahi_server_config_init(&tServerConfig);
    tServerConfig.publish_workstation = 0;

    /* Make the device name HostName Friendly by replacing spaces with underscores */
    tServerConfig.host_name = avahi_strdup(hNetAppZeroConf->pDeviceName);


    /* Now replace the spaces with underscores in the hostname */
    pTemp = tServerConfig.host_name;
    while ( (pTemp = strchr(pTemp, ' ')) != NULL)
        pTemp[0] = '_';

    /* Allocate a new server */
    hNetAppZeroConf->pServer = avahi_server_new(avahi_simple_poll_get(
        hNetAppZeroConf->pSimplePoll), &tServerConfig, NetAppZeroConf_P_ServerCB, hNetAppZeroConf, &lAvahiError);

    if ( (hNetAppZeroConf->pServer == NULL) || (lAvahiError != AVAHI_OK) )
    {
        NETAPP_ERR(("%s() Failed to Start Avahi Server error %d, hostname='%s'\n", __FUNCTION__, lAvahiError, tServerConfig.host_name));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }


    /* Free the configuration data */
    avahi_server_config_free(&tServerConfig);


    /* Run the main loop */
    while (tRetCode == NETAPP_SUCCESS)
    {
        NETAPP_ZEROCONF_MSG tMessage;
        if (NetAppOSMsgQReceive(hNetAppZeroConf->ulMsgQId, &tMessage, sizeof(NETAPP_ZEROCONF_MSG), 0) == NETAPP_SUCCESS)
        {
            SERVICE_BROWSE_NODE *pNode = NULL;

            /* Start Browsing a given service */
            if (tMessage.bStart)
            {
                pNode = (SERVICE_BROWSE_NODE*)NetAppListAppend(hNetAppZeroConf->hBrowseList);
                if (pNode == NULL)
                {
                    NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
                    goto err_out;
                }

                strncpy(pNode->cServiceType, tMessage.cServiceType, sizeof(pNode->cServiceType)-1);

                NETAPP_MSG(("%s(): Initiating Discovery of '%s'", __FUNCTION__, pNode->cServiceType));

                pNode->pServiceBrowser = avahi_s_service_browser_new(hNetAppZeroConf->pServer, AVAHI_IF_UNSPEC,
                    AVAHI_PROTO_UNSPEC, pNode->cServiceType, NULL, 0, NetAppZeroConf_P_BrowseCB, hNetAppZeroConf);
            }

            /* Stop Browsing a given service */
            else
            {
                NetAppListRewind(hNetAppZeroConf->hBrowseList);
                while ( (pNode = (SERVICE_BROWSE_NODE*)NetAppListNext(hNetAppZeroConf->hBrowseList)) != NULL)
                {
                    if (strncmp(pNode->cServiceType, tMessage.cServiceType, sizeof(pNode->cServiceType)) == 0)
                    {
                        NETAPP_MSG(("%s(): Removing Discovery Search of '%s'", __FUNCTION__, pNode->cServiceType));
                        avahi_s_service_browser_free(pNode->pServiceBrowser);
                        NetAppListRemove(hNetAppZeroConf->hBrowseList, (NETAPP_LIST_NODE*)pNode);
                        break;
                    }
                }
            }
        }

        tRetCode = (avahi_simple_poll_iterate(hNetAppZeroConf->pSimplePoll,
            NETAPP_ZEROCONF_POLL_SLEEP_MS) == AVAHI_OK) ? NETAPP_SUCCESS : NETAPP_FAILURE;
    }

err_out:
    if (hNetAppZeroConf->pGroup != NULL)
    {
        avahi_s_entry_group_free(hNetAppZeroConf->pGroup);
        hNetAppZeroConf->pGroup = NULL;
    }

    if (hNetAppZeroConf->pServer != NULL)
    {
        avahi_server_free(hNetAppZeroConf->pServer);
        hNetAppZeroConf->pServer = NULL;
    }

    if (hNetAppZeroConf->pSimplePoll != NULL)
    {
        avahi_simple_poll_free(hNetAppZeroConf->pSimplePoll);
        hNetAppZeroConf->pSimplePoll = NULL;
    }

    hNetAppZeroConf->ulServerThread = 0;
    NetAppOSTaskExit();
}


static void NetAppZeroConf_P_ServerCB(
    AvahiServer             *pServer,
    AvahiServerState        tState,
    void                    *pData)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    char                    *pName = NULL;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)pData;

    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    switch (tState)
    {
    case AVAHI_SERVER_RUNNING:
    {
        NETAPP_MSG(("%s(): Received AVAHI_SERVER_RUNNING", __FUNCTION__));
        SERVICE_INFO_NODE *pCurrent  = NULL;

        hNetAppZeroConf->pGroup = avahi_s_entry_group_new(hNetAppZeroConf->pServer,
            NetAppZeroConf_P_EntryGroupCB, hNetAppZeroConf);

        if (hNetAppZeroConf->pGroup == NULL)
        {
            NETAPP_ERR(("%s(): Failed to create a Avahi entry group!!", __FUNCTION__));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }

        NETAPP_PRIV_SEM_TAKE(hNetAppZeroConf->ulSem, tRetCode);

        NetAppListRewind(hNetAppZeroConf->hPublishedServiceList);

        if (NetAppListNext(hNetAppZeroConf->hPublishedServiceList) != NULL)
        {
            tRetCode = NetAppZeroConf_P_Publish(hNetAppZeroConf);
        }
        NETAPP_PRIV_SEM_GIVE(hNetAppZeroConf->ulSem);
        break;
    }


    case AVAHI_SERVER_COLLISION:
        NETAPP_MSG(("%s(): Received AVAHI_SERVER_COLLISION", __FUNCTION__));
        /* A host name collision happened. Let's pick a new name for the server */
        pName = avahi_alternative_host_name(avahi_server_get_host_name(pServer));
        NETAPP_ERR(("%s(): Host name collision, retrying with '%s'\n", __FUNCTION__, pName));

        AVAHI_API_CHECK(avahi_server_set_host_name(pServer, pName), pServer, tRetCode);
        /* Fall through */


    case AVAHI_SERVER_REGISTERING:
        NETAPP_MSG(("%s(): Received AVAHI_SERVER_REGISTERING", __FUNCTION__));

        if (hNetAppZeroConf->pGroup != NULL)
        {
            avahi_s_entry_group_reset(hNetAppZeroConf->pGroup);
        }
        break;


    case AVAHI_SERVER_FAILURE:
        NETAPP_MSG(("%s(): Received AVAHI_SERVER_FAILURE", __FUNCTION__));

        tRetCode = NETAPP_FAILURE;
        break;


    default:
        NETAPP_MSG(("%s(): Received AVAHI_SERVER_INVALID", __FUNCTION__));

        break;
    }

    hNetAppZeroConf->tState = tState;

err_out:
    if (pName != NULL)
    {
        avahi_free(pName);
    }

    if (tRetCode != NETAPP_SUCCESS)
    {
        NETAPP_ERR(("%s(): Failure in Avahi, Quiting now!", __FUNCTION__));
        avahi_simple_poll_quit(hNetAppZeroConf->pSimplePoll);
    }
}

static void NetAppZeroConf_P_EntryGroupCB(
    AvahiServer             *pServer,
    AvahiSEntryGroup        *pGroup,
    AvahiEntryGroupState    tState,
    void                    *pData)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)pData;

    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }
}


static NETAPP_RETCODE NetAppZeroConf_P_Publish(
    NETAPP_ZEROCONF_HANDLE          hNetAppZeroConf)
{
    NETAPP_RETCODE      tRetCode = NETAPP_SUCCESS;
    char                cTmpTxtManufacturer[64];
    char                cTmpTxtModelName[64];
    char                cTmpTxtModelNumber[64];
    char                cTmpTxtSerialNumber[64];
    SERVICE_INFO_NODE   *pCurrent = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf->hPublishedServiceList);

    avahi_s_entry_group_reset(hNetAppZeroConf->pGroup);

    snprintf(cTmpTxtManufacturer, sizeof(cTmpTxtManufacturer), "manufacturer=%s",  hNetAppZeroConf->pManufacturer);
    snprintf(cTmpTxtModelName,    sizeof(cTmpTxtModelName),    "model_name=%s",    hNetAppZeroConf->pModelName);
    snprintf(cTmpTxtModelNumber,  sizeof(cTmpTxtModelNumber),  "model_number=%s",  hNetAppZeroConf->pModelNumber);
    snprintf(cTmpTxtSerialNumber, sizeof(cTmpTxtSerialNumber), "serial_number=%s", hNetAppZeroConf->pSerialNumber);

    NetAppListRewind(hNetAppZeroConf->hPublishedServiceList);


    while ((pCurrent = (SERVICE_INFO_NODE*)NetAppListNext(hNetAppZeroConf->hPublishedServiceList))!= NULL)
    {
        AVAHI_API_CHECK(avahi_server_add_service(hNetAppZeroConf->pServer, hNetAppZeroConf->pGroup, AVAHI_IF_UNSPEC,
            AVAHI_PROTO_UNSPEC, 0, pCurrent->tInfo.pName, pCurrent->tInfo.pType, NULL, NULL, pCurrent->tInfo.ulPort,
            cTmpTxtManufacturer, cTmpTxtModelName, cTmpTxtModelNumber, cTmpTxtSerialNumber,
            pCurrent->tInfo.pTxtRecord, NULL), hNetAppZeroConf->pServer, tRetCode);
    }

    /* Tell the server to register the service */
    AVAHI_API_CHECK(avahi_s_entry_group_commit(hNetAppZeroConf->pGroup), hNetAppZeroConf->pServer, tRetCode);

err_out:
    return tRetCode;
}


static void NetAppZeroConf_P_BrowseCB(
    AvahiSServiceBrowser    *pServiceBrowser,
    AvahiIfIndex            tInterface,
    AvahiProtocol           tProtocol,
    AvahiBrowserEvent       tEvent,
    const char              *name,
    const char              *type,
    const char              *domain,
    AvahiLookupResultFlags  flags,
    void                    *pData)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)pData;

    NETAPP_ERR(("%s(): Entered!", __FUNCTION__));
    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    NETAPP_PRIV_SEM_TAKE(hNetAppZeroConf->ulSem, tRetCode);

    switch (tEvent)
    {
    case AVAHI_BROWSER_FAILURE:
        NETAPP_MSG(("%s(): Received AVAHI_BROWSER_FAILURE %s",
            __FUNCTION__, avahi_strerror(avahi_server_errno(hNetAppZeroConf->pServer))));

        tRetCode = NETAPP_FAILURE;
        break;


    case AVAHI_BROWSER_NEW:
    {
        AvahiSServiceResolver *pServiceResolver = NULL;

        NETAPP_MSG(("%s(): Received AVAHI_BROWSER_NEW, Service '%s' of type '%s' in domain '%s'",
            __FUNCTION__, name, type, domain));

        pServiceResolver = avahi_s_service_resolver_new(hNetAppZeroConf->pServer, tInterface, tProtocol, name,
            type, domain, AVAHI_PROTO_UNSPEC, 0, NetAppZeroConf_P_ResolveCB, hNetAppZeroConf);

        if (pServiceResolver == NULL)
        {
            NETAPP_ERR(("%s(): Unable to create the service resolver error '%s'!",
                __FUNCTION__, avahi_strerror(avahi_server_errno(hNetAppZeroConf->pServer))));
            tRetCode = NETAPP_FAILURE;
        }
        break;
    }


    case AVAHI_BROWSER_REMOVE:
    {
        SERVICE_INFO_NODE *pNode = NULL;

        NETAPP_MSG(("%s(): Received AVAHI_BROWSER_REMOVE service '%s' of type '%s' in domain '%s'",
            __FUNCTION__, name, type, domain));

        NETAPP_DO_CALLBACK(NETAPP_CB_ZEROCONF, name, strlen(name)+1, NETAPP_ZEROCONF_SERVICE_REMOVED, tRetCode, NETAPP_IFACE_MAX);

        pNode = (SERVICE_INFO_NODE*)NetAppListFind(hNetAppZeroConf->hBrowsedServiceList, (const void*)name, strlen(name));
        if (pNode != NULL)
        {
            NETAPP_PRIV_API_CHECK(NetAppZeroConf_P_DeleteServiceInfo(hNetAppZeroConf, pNode), tRetCode);
            NETAPP_PRIV_API_CHECK(NetAppListRemove(hNetAppZeroConf->hBrowsedServiceList, (NETAPP_LIST_NODE*)pNode), tRetCode);
        }
        break;
    }

    default:
    case AVAHI_BROWSER_ALL_FOR_NOW:
    case AVAHI_BROWSER_CACHE_EXHAUSTED:
        NETAPP_MSG(("%s(): (Browser) %s ", __FUNCTION__, (tEvent == AVAHI_BROWSER_CACHE_EXHAUSTED) ? "CACHE_EXHAUSTED" : "ALL_FOR_NOW"));
        break;
    }

err_out:
    NETAPP_PRIV_SEM_GIVE(hNetAppZeroConf->ulSem);
    if (tRetCode != NETAPP_SUCCESS)
    {
        NETAPP_DO_CALLBACK(NETAPP_CB_ZEROCONF, name, strlen(name)+1, NETAPP_ZEROCONF_SERVICE_FOUND, tRetCode, NETAPP_IFACE_MAX);
    }
}


static void NetAppZeroConf_P_ResolveCB(
    AvahiSServiceResolver   *pServiceResolver,
    AvahiIfIndex            tInterface,
    AvahiProtocol           tProtocol,
    AvahiResolverEvent      tEvent,
    const char              *name,
    const char              *type,
    const char              *domain,
    const char              *host_name,
    const AvahiAddress      *address,
    uint16_t                port,
    AvahiStringList         *txt,
    AvahiLookupResultFlags  flags,
    void                    *pData)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf = (NETAPP_ZEROCONF_HANDLE)pData;
    char                    *pTmpStr = NULL;

    if (hNetAppZeroConf == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    switch (tEvent)
    {
    case AVAHI_RESOLVER_FOUND:
    {
        SERVICE_INFO_NODE *pNode = NULL;

        NETAPP_MSG(("%s(): Received AVAHI_RESOLVER_FOUND service '%s' of type '%s' in domain '%s'",
            __FUNCTION__, name, type, domain));

        pTmpStr = avahi_string_list_to_string(txt);

        NETAPP_PRIV_SEM_TAKE(hNetAppZeroConf->ulSem, tRetCode);

        pNode = (SERVICE_INFO_NODE*)NetAppListAppend(hNetAppZeroConf->hBrowsedServiceList);
        if (pNode)
        {
            tRetCode = NetAppZeroConf_P_CreateServiceInfo(hNetAppZeroConf, name, type, port, pTmpStr,
                        avahi_string_list_length(txt), domain, host_name, address->data.ipv4.address, pNode);
        }

        NETAPP_PRIV_SEM_GIVE(hNetAppZeroConf->ulSem);
        break;
    }


    default:
    case AVAHI_RESOLVER_FAILURE:
        NETAPP_ERR(("%s(): (Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
         __FUNCTION__, name, type, domain, avahi_strerror(avahi_server_errno(hNetAppZeroConf->pServer))));

        tRetCode = NETAPP_FAILURE;
        break;
    }

err_out:
    if (pServiceResolver != NULL)
    {
        avahi_s_service_resolver_free(pServiceResolver);
    }
    if (pTmpStr != NULL)
    {
        avahi_free(pTmpStr);
    }
    NETAPP_DO_CALLBACK(NETAPP_CB_ZEROCONF, name, strlen(name)+1, NETAPP_ZEROCONF_SERVICE_FOUND, tRetCode, NETAPP_IFACE_MAX);
}

static NETAPP_RETCODE NetAppZeroConf_P_CreateServiceInfo(
    NETAPP_ZEROCONF_HANDLE      hNetAppZeroConf,
    const char                  *pName,
    const char                  *pType,
    uint32_t                    ulPort,
    char                        *pTxtRecord,
    uint32_t                    ulTxtLength,
    const char                  *pDomain,
    const char                  *pHost,
    NETAPP_IPV4_ADDR            tIpAddress,
    SERVICE_INFO_NODE           *pNode)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    NETAPP_PRIV_HANDLE_CHECK(pNode);
    NETAPP_PRIV_HANDLE_CHECK(pName);
    NETAPP_PRIV_HANDLE_CHECK(pType);

    pNode->tInfo.ulPort      = ulPort;
    pNode->tInfo.ulTxtLength = ulTxtLength;
    pNode->tInfo.pName       = strdup(pName);
    pNode->tInfo.pType       = strdup(pType);
    if (pTxtRecord != NULL)
    {
        pNode->tInfo.pTxtRecord = strdup(pTxtRecord);
    }

    return NETAPP_SUCCESS;
}


static NETAPP_RETCODE NetAppZeroConf_P_DeleteServiceInfo(
    NETAPP_ZEROCONF_HANDLE      hNetAppZeroConf,
    SERVICE_INFO_NODE           *pNode)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppZeroConf);
    NETAPP_PRIV_HANDLE_CHECK(pNode);

    /* Free all the memory Now */
    if (pNode->tInfo.pName != NULL)
        free(pNode->tInfo.pName);

    if (pNode->tInfo.pType != NULL)
        free(pNode->tInfo.pType);

    if (pNode->tInfo.pTxtRecord != NULL)
        free(pNode->tInfo.pTxtRecord);

    return NETAPP_SUCCESS;
}

static void NetAppZeroConf_P_LogFunc(
    AvahiLogLevel           level,
    const char               *txt)
{
    switch (level)
    {
    case AVAHI_LOG_ERROR:
        NETAPP_ERR(("%s()[ERROR]: %s", __FUNCTION__, txt));
        break;
    default:
        NETAPP_MSG(("%s()[MSG]: %s", __FUNCTION__, txt));
        break;
    }
}
