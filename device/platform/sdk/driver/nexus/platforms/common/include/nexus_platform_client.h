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
* $brcm_Workfile: nexus_platform_client.h $
* $brcm_Revision: 9 $
* $brcm_Date: 8/1/12 4:00p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/include/nexus_platform_client.h $
* 
* 9   8/1/12 4:00p erickson
* SW7425-2734: allow server to limit resources for untrusted clients
* 
* 8   11/29/11 11:34a erickson
* SW7420-1148: clarify join of unauthenticated client
* 
* 7   9/13/11 1:23p erickson
* SW7420-1148: change client modes to unprotected/protected/untrusted
* 
* 6   8/25/11 10:54a erickson
* SW7420-1148: default server to using untrusted clients
* 
* 5   8/10/11 3:03p erickson
* SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
*  clients don't have to #include nexus_platform.h
* 
* 4   3/17/11 5:21p erickson
* SW7420-1123: clarify join
*
* 3   3/17/11 5:17p erickson
* SW7420-1123: clarify join
*
* 2   1/21/11 3:11p erickson
* SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
*  communication
*
* 1   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_CLIENT_H__
#define NEXUS_PLATFORM_CLIENT_H__

#include "nexus_types.h"
#include "nexus_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to identify a client
**/
typedef struct NEXUS_ClientAuthenticationSettings
{
    NEXUS_Certificate certificate;

    /* TODO: struct {
    } preferences; */
} NEXUS_ClientAuthenticationSettings;

/**
Summary:
Client modes

See nexus/docs/Nexus_MultiProcess.pdf for full discussion of process isolation and multi-process application design.
**/
typedef enum NEXUS_ClientMode
{
    NEXUS_ClientMode_eUnprotected, /* no client handle verification, resource management or cleanup. access to full API. client may compromise the server. */
    NEXUS_ClientMode_eProtected,   /* full handle verification. access to full API. if client crashes, server is protected. */
    NEXUS_ClientMode_eUntrusted,   /* full handle verification. access to limited API. see nexus/build/common/tools/nexus_untrusted_api.txt. if client crashes, server is protected. */
    NEXUS_ClientMode_eMax
} NEXUS_ClientMode;

/**
Summary:
Information provided by the server to the client
**/
typedef struct NEXUS_ClientConfiguration
{
    NEXUS_ClientMode mode; /* default is eProtected. */
    NEXUS_HeapHandle heap[NEXUS_MAX_HEAPS]; /* untrusted client will be restricted to these heaps. heap[0] will be its default. */
    NEXUS_ClientResources resources; /* resources granted by the server for untrusted clients */
} NEXUS_ClientConfiguration;

/**
Summary:
Get default settings for NEXUS_Platform_AuthenticatedJoin
**/
void NEXUS_Platform_GetDefaultClientAuthenticationSettings(
    NEXUS_ClientAuthenticationSettings *pSettings /* [out] */
    );

/**
Summary:
Join a client process to the Nexus server

Description:
A nexus client is a separate process or application which connects to the single Nexus server using
some form of inter-process communication (IPC).

The Nexus server must already be started in another process with NEXUS_Platform_Init. There can only be one
server in a system. There can be zero, one or more clients.

If you have already called NEXUS_Platform_Init from your process, then you cannot also join as a client.
If you have already called NEXUS_Platform_AuthenticatedJoin, then you cannot join again until calling NEXUS_Platform_Uninit.
Only one join at a time is allowed per process.

If pSettings is NULL, the client is unauthenticated. The server must explicitly allow unauthenticated clients it starts.

The NEXUS_Platform_Join macro is provided for backward compatibility.

When your client process wants to disconnect from Nexus it should call NEXUS_Platform_Uninit. Any connection
with the server will be terminated and all client nexus state will be destroyed. You can then join again
if you wish.

Clients should call NEXUS_Platform_GetClientConfiguration after joining to learn what permissions
and heaps they have access to.

See nexus/docs/Nexus_MultiProcess.pdf for more documentation.
**/
NEXUS_Error NEXUS_Platform_AuthenticatedJoin(
    const NEXUS_ClientAuthenticationSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Join a trusted client process (with default settings) to the Nexus server
**/
#define NEXUS_Platform_Join() NEXUS_Platform_AuthenticatedJoin(NULL)

/***************************************************************************
Summary:
For clients, disconnect from nexus, paired with NEXUS_Platform_AuthenticatedJoin.
For the server, uninitialized nexus, pair with NEXUS_Platform_Init.

Description:
The user is responsible for closing handles before calling NEXUS_Platform_Uninit.
Some handles can be automatically closed, but some cannot. It is best to explicitly
close all handles that you have opened.

After calling NEXUS_Platform_Uninit, the client may call NEXUS_Platform_AuthenticatedJoin again.
After calling NEXUS_Platform_Uninit, the server may call NEXUS_Platform_Init again.
***************************************************************************/
void NEXUS_Platform_Uninit(void);

/**
Summary:
Get client configuration settings provided by the server
**/
void NEXUS_Platform_GetClientConfiguration(
    NEXUS_ClientConfiguration *pSettings /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif
