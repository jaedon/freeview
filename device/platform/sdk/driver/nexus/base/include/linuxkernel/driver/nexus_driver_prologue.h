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
* $brcm_Workfile: nexus_driver_prologue.h $
* $brcm_Revision: 36 $
* $brcm_Date: 8/16/12 4:35p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel drivers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxkernel/driver/nexus_driver_prologue.h $
* 
* 36   8/16/12 4:35p erickson
* SW7435-303: add NEXUS_Platform_GetClientStatus
* 
* 35   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 34   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 33   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 32   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 31   11/29/11 11:37a erickson
* SW7420-2129: store default heap in nexus_driver_client_state
* 
* 30   11/1/11 1:52p erickson
* SW7420-2116: extend nexus_driver_state locking to close race
*  conditions, add client->list to know state of client, refactor
*  uninit_pending/uninit_in_progress booleans
* 
* 29   9/20/11 10:13a erickson
* SW7420-1148: fix NEXUS_Platform_GetClientConfiguration, store entire
*  client config in driver's client state
* 
* 28   9/2/11 10:03a erickson
* SW7420-1148: allow nexus_register/unregister to objdb from nexus
*  module. enables more complex resource management.
* 
* 27   8/22/11 2:16p erickson
* SW7420-1148: print error if trusted client terminates abnormally.
*  server can now specify defaultUntrustedHeap.
* 
* 26   7/26/11 12:07p erickson
* SW7420-1992: callback database must take client id so that callbacks
*  can be disconnected when a client releases
* 
* 25   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 24   4/19/11 5:35p erickson
* SW7425-365: add linux 2.6.37 support
* 
* 23   4/8/11 11:14a erickson
* SW7420-1671: rework nexus_driver to force clients closed when server
*  uninits. greatly simplifies logic.
*
* 22   4/7/11 5:27p erickson
* SW7420-1671: refactor for driver stability under stress
*
* 21   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 20   1/21/11 3:11p erickson
* SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
*  communication
*
* 19   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 18   10/13/10 10:35a erickson
* SW7420-1148: switch verify return code to 0/-1
*
* 17   10/12/10 4:29p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 16   10/7/10 11:06a erickson
* SW7420-1148: add client_id
*
* 15   9/24/10 10:12a erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 14   8/12/10 8:14p vsilyaev
* SW7420-943: Added support for REFSW_FIRMWARE
*
* 13   7/14/10 12:27p erickson
* SW7405-4621: remove unneeded #include
*
* 12   9/11/09 9:56a erickson
* SW7420-228: switch to autoconf.h
*
* 11   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 10   7/24/08 6:09p vsilyaev
* PR 45137: Modified INIT ioctl to pass application type
*
* 9   6/10/08 3:29p vsilyaev
* PR 43184: Added support for Start/Stop callbacks in linux kernel/user
*  proxy mode
*
* 8   2/4/08 2:39p vsilyaev
* PR 38682: Added support for pointers with variable size
*
* 7   2/1/08 5:03p vsilyaev
* PR 38682: Added tracking of open/close calls
*
* 6   1/30/08 11:49a vsilyaev
* PR 38682: Added routing of callbacks into the user space
*
* 5   1/29/08 5:55p vsilyaev
* PR 38682: Support of callbacks in the linux kernel/user proxy
*  environment
*
* 4   1/28/08 12:35p vsilyaev
* PR 38682: Added support for attribute tags applied to structure
*  members.
*
* 3   1/25/08 4:24p vsilyaev
* PR 38682: Reduce code size of the generated code
*
* 2   1/24/08 4:31p vsilyaev
* PR 38682: Improved kernel/proxy environment
*
* 1   1/18/08 1:14p vsilyaev
* PR 38682: Nexus driver common files
*
*
***************************************************************************/

#ifndef _NEXUS_DRIVER_PROLOGUE_H_
#define _NEXUS_DRIVER_PROLOGUE_H_

#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_base_ioctl.h"
#include "priv/nexus_core_driver.h"
#include "nexus_platform_client.h"
#include "bkni.h"
#include "b_objdb.h"

struct nexus_driver_callback_map;

/*
nexus_driver_client_state - one per client (including one for the server)
*/
BDBG_OBJECT_ID_DECLARE(nexus_driver_client_state);
struct nexus_driver_client_state {
    BDBG_OBJECT(nexus_driver_client_state)
    enum {
        nexus_driver_clients_list,
        nexus_driver_allowed_clients_list,
        nexus_driver_cleanup_clients_list,
    } list;
    BLST_S_ENTRY(nexus_driver_client_state) link;
    NEXUS_Certificate certificate; /* set by the server, verified by join */
    NEXUS_ClientConfiguration config;
    NEXUS_HeapHandle defaultHeap, defaultFullyMappedHeap;
    unsigned pid; /* if zero, then not joined */
    unsigned refcnt; /* number of nexus_driver_module_driver_state's opened */
    struct b_objdb_client client;
    bool dynamic; /* dynamically created by driver */
    bool joined; /* set true on join, false on uninit */
    unsigned numJoins;
};

/* struct nexus_driver_slave_scheduler; */

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

/* route all copy_to/from_user to local functions, this is needed in order to reduce size of generated drivers */
unsigned long copy_to_user_small(void * to, const void * from, unsigned long n);
unsigned long copy_from_user_small(void * to, const void * from, unsigned long n);

/*
nexus_driver_module_driver_state - one per client per module
*/
struct nexus_driver_module_driver_state {
    struct nexus_driver_client_state *client;
    struct nexus_driver_slave_scheduler *slave_scheduler;
    unsigned pid; /* store pid before client created */
};

void nexus_driver_send_addr(void **paddr); /* inplace convert address from virtual to physical */
void nexus_driver_recv_addr_cached(void **paddr); /* inplace convert address from physical to virtual */
void nexus_driver_free(void *addr); /* special version of BKNI_Free which is safe to use for free NULL pointer */

/* nexus_driver_callbacks */
void nexus_driver_callback_to_driver(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id, 
    const struct b_objdb_client *client, struct nexus_driver_slave_scheduler *slave);
void nexus_driver_callback_to_user(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);
void nexus_driver_callback_update(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *old_handle, unsigned id, void *new_handle);
void nexus_driver_callback_to_driver_commit(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);
void nexus_driver_callback_to_driver_cancel(struct nexus_driver_module_header *header, NEXUS_CallbackDesc *callback, void *handle, unsigned id);

int nexus_driver_module_heap_verify( struct nexus_driver_client_state *client, NEXUS_HeapHandle heap, NEXUS_HeapHandle *pHeap );

/* nexus_generic_driver.c functions accessible from proxy */
int nexus_driver_module_init(unsigned index, struct nexus_driver_module_header *header, NEXUS_ModuleHandle module, const struct b_objdb_class *class_table, const char *name);
void nexus_driver_module_uninit(struct nexus_driver_module_header *header);
struct nexus_driver_client_state *nexus_driver_client_id(struct nexus_driver_module_driver_state *state);

#endif 
