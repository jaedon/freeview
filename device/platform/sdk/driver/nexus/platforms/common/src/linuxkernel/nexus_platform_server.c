/***************************************************************************
 *     (c)2004-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_platform_server.c $
 * $brcm_Revision: 24 $
 * $brcm_Date: 10/12/12 11:57a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_platform_server.c $
 * 
 * 24   10/12/12 11:57a erickson
 * SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
 * 
 * 23   10/8/12 2:30p vsilyaev
 * SW7425-1792: Allow application to control life-cycle of objects
 * 
 * 22   8/20/12 11:16a erickson
 * SW7420-2332: fix prototype
 * 
 * 21   8/16/12 4:35p erickson
 * SW7435-303: add NEXUS_Platform_GetClientStatus
 * 
 * 20   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 19   7/31/12 12:08p erickson
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 18   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 17   6/22/12 4:45p erickson
 * SW7420-2078: open/close objdb in
 *  NEXUS_Platform_P_InitServer/NEXUS_Platform_P_UninitServer
 * 
 * 16   5/10/12 4:46p erickson
 * SW7231-800: add #if NEXUS_SERVER_SUPPORT
 * 
 * 15   9/20/11 10:13a erickson
 * SW7420-1148: fix NEXUS_Platform_GetClientConfiguration, store entire
 *  client config in driver's client state
 * 
 * 14   9/13/11 1:23p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 13   8/30/11 11:15a erickson
 * SW7420-1148: fix trusted=false even if certificate used. improve MSG-
 *  level debug for general system analysis.
 * 
 * 12   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 * 
 * 11   8/22/11 2:16p erickson
 * SW7420-1148: print error if trusted client terminates abnormally.
 *  server can now specify defaultUntrustedHeap.
 * 
 * 10   7/26/11 12:05p erickson
 * SW7420-1992: NEXUS_Platform_StopServer must terminate all clients
 *  (except the server itself)
 * 
 * 9   5/17/11 10:04a erickson
 * SW7420-1819: simplify create/destroy client api, perform proper checks
 *  to pass stress test.
 * 
 * 8   5/3/11 5:09p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 * 
 * 7   4/14/11 3:09p erickson
 * SW7420-1671: unwind NEXUS_ClientHandles on failure
 *
 * 6   4/8/11 11:03a erickson
 * SW7420-1671: set nexus_driver_state.active based on full
 *  NEXUS_Platform_Init
 *
 * 5   4/7/11 5:27p erickson
 * SW7420-1671: refactor for driver stability under stress
 *
 * 4   3/25/11 11:37a erickson
 * SW7420-1671: default trustUnauthenticatedClients = true
 *
 * 3   3/25/11 9:29a erickson
 * SW7420-1671: add
 *  NEXUS_PlatformStartServerSettings.trustUnauthenticatedClients
 *
 * 2   1/19/11 10:42a erickson
 * SW7420-1391: default allowUnauthenticatedClients = true
 *
 * 1   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 ************************************************************/
#include "nexus_base.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_platform_server.h"
#include "nexus_generic_driver_impl.h"

BDBG_MODULE(nexus_platform_server);

BDBG_OBJECT_ID(NEXUS_Client);
struct NEXUS_Client
{
    BDBG_OBJECT(NEXUS_Client)
    NEXUS_ClientSettings settings;
    BLST_S_ENTRY(NEXUS_Client) link;
    struct nexus_driver_client_state *driver_client;
    unsigned numJoins;
};

static struct {
    BLST_S_HEAD(clientlist, NEXUS_Client) clients;
} g_server;

NEXUS_Error NEXUS_Platform_P_InitServer(void)
{
    int rc;

    if (nexus_driver_state.active) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    rc = nexus_driver_server_postinit();
    if (rc) return BERR_TRACE(rc);

    nexus_driver_state.active = true;
    return 0;
}

void NEXUS_Platform_P_UninitServer(void)
{
    NEXUS_ClientHandle client;
    nexus_driver_state.active = false;
    while ((client = BLST_S_FIRST(&g_server.clients))) {
        NEXUS_Platform_UnregisterClient(client);
    }
    
    nexus_driver_server_preuninit();
}

NEXUS_Error NEXUS_Platform_StartServer(const NEXUS_PlatformStartServerSettings *pSettings)
{
    NEXUS_PlatformStartServerSettings defaultSettings;

    if (!pSettings) {
        NEXUS_Platform_GetDefaultStartServerSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    nexus_driver_state.serverSettings = *pSettings;
    return 0;
}

void NEXUS_Platform_StopServer(void)
{
#if NEXUS_SERVER_SUPPORT
    /* disable all clients except the server */
    nexus_driver_disable_clients(false);
#endif
}

NEXUS_ClientHandle NEXUS_Platform_RegisterClient( const NEXUS_ClientSettings *pSettings )
{
#if NEXUS_SERVER_SUPPORT
    NEXUS_ClientHandle client;
    
    BDBG_ASSERT(pSettings);
    client = BKNI_Malloc(sizeof(*client));
    if (!client) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BDBG_OBJECT_SET(client, NEXUS_Client);
    client->settings = *pSettings;

    /* populate the driver with this client */
    client->driver_client = nexus_driver_create_client(&pSettings->authentication.certificate, &pSettings->configuration);
    if (!client->driver_client) {
        BKNI_Free(client);
        return NULL;
    }
    BLST_S_INSERT_HEAD(&g_server.clients, client, link);
    return client;
#else
    BSTD_UNUSED(pSettings);
    return NULL;
#endif
}

void NEXUS_Platform_UnregisterClient( NEXUS_ClientHandle client )
{
#if NEXUS_SERVER_SUPPORT
    BDBG_OBJECT_ASSERT(client, NEXUS_Client);
    BLST_S_REMOVE(&g_server.clients, client, NEXUS_Client, link);
    nexus_driver_destroy_client(client->driver_client);
    BDBG_OBJECT_DESTROY(client, NEXUS_Client);
    BKNI_Free(client);
#else
    BSTD_UNUSED(client);
#endif
}

void NEXUS_Platform_GetClientConfiguration( NEXUS_ClientConfiguration *pSettings )
{
    nexus_driver_get_client_configuration(b_objdb_get_client(), pSettings);
}

NEXUS_Error NEXUS_Platform_GetClientStatus( NEXUS_ClientHandle client, NEXUS_ClientStatus *pStatus )
{
#if NEXUS_SERVER_SUPPORT
    BDBG_OBJECT_ASSERT(client, NEXUS_Client);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->connected = client->driver_client->joined;
    pStatus->numJoins = client->driver_client->numJoins;
    return 0;
#else
    BSTD_UNUSED(client);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

/**
The following client functions only have meaning in the client, not in the server.
They exist here because the server is the superset of the client.
**/
NEXUS_Error NEXUS_Platform_AuthenticatedJoin(const NEXUS_ClientAuthenticationSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_Platform_GetDefaultClientAuthenticationSettings( NEXUS_ClientAuthenticationSettings *pSettings )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_Platform_AcquireObject(NEXUS_ClientHandle client, const NEXUS_InterfaceName *type, void *object)
{
    return NEXUS_Platform_P_AcquireObject(&client->driver_client->client, type, object);
}

void NEXUS_Platform_ReleaseObject(const NEXUS_InterfaceName *type, void *object)
{
    NEXUS_Platform_P_ReleaseObject(type, object);
    return;
}

