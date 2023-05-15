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
* $brcm_Workfile: nexus_server_prologue.h $
* $brcm_Revision: 12 $
* $brcm_Date: 7/23/12 11:35a $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel drivers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/server/nexus_server_prologue.h $
* 
* 12   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 11   7/16/12 1:01p vsilyaev
* SW7420-2324: Use atomic verify and acquire for handle verification
* 
* 10   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 9   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 8   11/29/11 11:37a erickson
* SW7420-2129: store default heap in nexus_driver_client_state
* 
* 7   9/20/11 10:11a erickson
* SW7420-1123: store entire client configuration
* 
* 6   9/2/11 10:03a erickson
* SW7420-1148: allow nexus_register/unregister to objdb from nexus
*  module. enables more complex resource management.
* 
* 5   7/26/11 12:07p erickson
* SW7420-1992: callback database must take client id so that callbacks
*  can be disconnected when a client releases
* 
* 4   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 3   3/25/11 12:09p erickson
* SW7420-1123: remove unused code
*
* 2   1/21/11 3:11p erickson
* SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
*  communication
*
* 1   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
***************************************************************************/
#ifndef _NEXUS_SERVER_PROLOGUE_H_
#define _NEXUS_SERVER_PROLOGUE_H_

#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_ipc_api.h"
#include "bkni.h"
#include "b_objdb.h"

struct nexus_driver_callback_map;

/*
nexus_driver_client_state - one per client (including one for the server)
*/
BDBG_OBJECT_ID_DECLARE(nexus_driver_client_state);
struct nexus_driver_client_state {
    BDBG_OBJECT(nexus_driver_client_state)
    BLST_S_ENTRY(nexus_driver_client_state) link;
    NEXUS_Certificate certificate; /* set by the server, verified by join */
    NEXUS_ClientConfiguration config;
    NEXUS_HeapHandle defaultHeap, defaultFullyMappedHeap;
    unsigned pid; /* if zero, then not joined */
    unsigned refcnt; /* number of nexus_driver_module_driver_state's opened */
    struct b_objdb_client client; /* address of this is the client.
                              also, client.trusted counts whether it is trusted. */
};

/*
nexus_driver_module_header - one per module
*/
struct nexus_driver_module_header {
    int16_t entry_count;
    int16_t open_count;
    struct nexus_driver_callback_map *callback_map;
    NEXUS_ModuleHandle module; /* sync for callbacks & objdb */
    const char *name;
    struct b_objdb objdb;
};

/*
nexus_driver_module_driver_state - one per client per module
*/
struct nexus_driver_module_driver_state {
    struct nexus_driver_client_state *client;
    struct nexus_driver_slave_scheduler *slave_scheduler;
};

typedef struct nexus_driver_callback_desc {
    NEXUS_CallbackDesc desc;
    void *interfaceHandle; /* interface handle that is used in NEXUS_Start/StopCallbacks */
} nexus_driver_callback_desc;

void nexus_driver_send_addr(void **paddr); /* inplace convert address from virtual to physical */
void nexus_driver_recv_addr_cached(void **paddr); /* inplace convert address from physical to virtual */

/* nexus_driver_callbacks */
void nexus_driver_callback_to_driver(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id, 
    void *client, void *context);
void nexus_driver_callback_to_user(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);
void nexus_driver_callback_update(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *old_handle, unsigned id, void *new_handle);
void nexus_driver_callback_to_driver_commit(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);
void nexus_driver_callback_to_driver_cancel(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);

int nexus_driver_module_heap_verify( struct nexus_driver_client_state *client, NEXUS_HeapHandle heap, NEXUS_HeapHandle *pHeap );
struct b_objdb_client;
extern void nexus_driver_deactivate_callbacks(void *context, void *object,  const struct b_objdb_client *client);

#endif /* _NEXUS_SERVER_PROLOGUE_H_ */
