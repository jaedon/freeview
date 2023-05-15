/***************************************************************************
*     (c)2010-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_server.h $
* $brcm_Revision: 8 $
* $brcm_Date: 10/12/12 11:57a $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/include/nexus_platform_server.h $
* 
* 8   10/12/12 11:57a erickson
* SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
*
* 7   10/8/12 2:30p vsilyaev
* SW7425-1792: Allow application to control life-cycle of objects
*
* 6   8/16/12 4:35p erickson
* SW7435-303: add NEXUS_Platform_GetClientStatus
*
* 5   9/13/11 1:23p erickson
* SW7420-1148: change client modes to unprotected/protected/untrusted
*
* 4   8/25/11 10:54a erickson
* SW7420-1148: default server to using untrusted clients
*
* 3   8/22/11 2:16p erickson
* SW7420-1148: print error if trusted client terminates abnormally.
*  server can now specify defaultUntrustedHeap.
*
* 2   3/25/11 9:29a erickson
* SW7420-1671: add
*  NEXUS_PlatformStartServerSettings.trustUnauthenticatedClients
*
* 1   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 2   9/29/10 9:47a erickson
* SW7420-1123: consolidate IPC server header file for now
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_SERVER_H__
#define NEXUS_PLATFORM_SERVER_H__

#include "nexus_platform_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************
Server management
******************************/

/**
Summary:
**/
typedef struct NEXUS_PlatformStartServerSettings
{
    bool allowUnprotectedClientsToCrash; /* default is false. if false, nexus will only allow protected and untrusted clients to crash.
                                 if an unprotected client crashes, the server will terminate and the application should be fixed. */
    bool allowUnauthenticatedClients; /* default is false. if false, the server must call NEXUS_Platform_RegisterClient to allow clients to join.
                                 if true, unregistered clients can connect and unauthenticatedConfiguration will determine their status. */
    NEXUS_ClientConfiguration unauthenticatedConfiguration;
} NEXUS_PlatformStartServerSettings;

/**
Summary:
**/
void NEXUS_Platform_GetDefaultStartServerSettings(
    NEXUS_PlatformStartServerSettings *pSettings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_Platform_StartServer(
    const NEXUS_PlatformStartServerSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
**/
void NEXUS_Platform_StopServer(void);

/******************************
server-side client management

The following API's allow the server to manage which clients can connect
and what resources they are granted.
******************************/

/**
Summary:
Server-side handle per client.
**/
typedef struct NEXUS_Client *NEXUS_ClientHandle;

/**
Summary:
Server-side settings per client.
**/
typedef struct NEXUS_ClientSettings
{
    NEXUS_ClientAuthenticationSettings authentication;
    NEXUS_ClientConfiguration configuration;
} NEXUS_ClientSettings;

/**
Summary:
**/
void NEXUS_Platform_GetDefaultClientSettings(
    NEXUS_ClientSettings *pSettings /* [out] */
    );

/**
Summary:
Server allows this client to connect
**/
NEXUS_ClientHandle NEXUS_Platform_RegisterClient(
    const NEXUS_ClientSettings *pSettings
    );

/**
Summary:
Server disallows this client. If connected, the client will be disconnected immediately.
**/
void NEXUS_Platform_UnregisterClient(
    NEXUS_ClientHandle client
    );

/**
Summary:
**/
typedef struct NEXUS_ClientStatus
{
    bool connected; /* is the client currently connected? */
    unsigned numJoins; /* total number of times the client has joined */
} NEXUS_ClientStatus;

/**
Summary:
**/
NEXUS_Error NEXUS_Platform_GetClientStatus(
    NEXUS_ClientHandle client,
    NEXUS_ClientStatus *pStatus /* [out] */
    );

#define NEXUS_INTERFACE_NAME_MAX   32
/**
Summary:
**/
typedef struct NEXUS_InterfaceName {
    char name[NEXUS_INTERFACE_NAME_MAX];
} NEXUS_InterfaceName;

/**
Summary:
Initializes the NEXUS_Platform_InterfaceName structure
**/
void NEXUS_Platform_GetDefaultInterfaceName(
        NEXUS_InterfaceName *name
        );

/*
Summary:
Acquires object to prevent its destruction

Description:
Verifies that object is a valid instance of said type and it's accessible by a given client, and if test passes increases the object reference count
*/
NEXUS_Error NEXUS_Platform_AcquireObject(NEXUS_ClientHandle client, const NEXUS_InterfaceName *name, void *object);

/*
Summary:
Releases object to allow its destruction

Description:
Verifies that object is a valid instance of said type, and if test passes decreases the object reference count
*/
void NEXUS_Platform_ReleaseObject(const NEXUS_InterfaceName *name, void *object);

#ifdef __cplusplus
}
#endif

#endif
