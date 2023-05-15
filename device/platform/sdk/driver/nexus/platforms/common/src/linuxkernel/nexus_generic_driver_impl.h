/***************************************************************************
 *     (c)2006-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_generic_driver_impl.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 11/27/12 9:38a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_generic_driver_impl.h $
 * 
 * 11   11/27/12 9:38a erickson
 * SW7435-519: prevent stale callbacks from crashing driver. release
 *  scheduler lock when a client terminates.
 * 
 * 10   2/1/12 10:38a erickson
 * SW7425-2231: fix kernel mode procfs support
 * 
 * 9   11/8/11 4:13p erickson
 * SW7405-5543: refactor callback dequeue to cancel callbacks in flight
 *  and to only synchronize with schedulers where callback is current
 * 
 * 8   9/20/11 10:13a erickson
 * SW7420-1148: fix NEXUS_Platform_GetClientConfiguration, store entire
 *  client config in driver's client state
 * 
 * 7   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 6   8/30/11 11:15a erickson
 * SW7420-1148: fix trusted=false even if certificate used. improve MSG-
 *  level debug for general system analysis.
 * 
 * 5   8/22/11 2:16p erickson
 * SW7420-1148: print error if trusted client terminates abnormally.
 *  server can now specify defaultUntrustedHeap.
 * 
 * 4   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 *  instead of syslog buffer
 * 
 * 3   7/26/11 12:05p erickson
 * SW7420-1992: NEXUS_Platform_StopServer must terminate all clients
 *  (except the server itself)
 * 
 * 2   5/17/11 10:04a erickson
 * SW7420-1819: simplify create/destroy client api, perform proper checks
 *  to pass stress test.
 * 
 * 1   5/3/11 5:10p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 * 
 * 20   4/8/11 11:14a erickson
 * SW7420-1671: rework nexus_driver to force clients closed when server
 *  uninits. greatly simplifies logic.
 *
 * 19   4/7/11 5:27p erickson
 * SW7420-1671: refactor for driver stability under stress
 *
 * 18   4/1/11 9:59a erickson
 * SW7420-1671: refactor object database for multiprocess stress
 *
 * 17   3/25/11 9:29a erickson
 * SW7420-1671: add
 *  NEXUS_PlatformStartServerSettings.trustUnauthenticatedClients
 *
 * 16   3/23/11 11:57a erickson
 * SW7420-1671: add nexus_driver_module_object_cleanup, clarify object
 *  leak, simplify nexus_driver_module_object_uninit params
 *
 * 15   2/17/11 4:07p erickson
 * SW7420-1148: fix rapid open/close of clients
 *
 * 14   12/22/10 2:28p erickson
 * SW7422-2: add support for multiple physical address regions within a
 *  single MEMC
 *
 * 13   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 * 12   10/12/10 4:31p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 * 11   9/23/10 2:23p erickson
 * SW7420-943: refactor for driver/firmware partitioning
 *
 * 10   8/25/09 12:24p erickson
 * SW7420-228: rework platform memory mapping to allow for multiple MEMC's
 *  on 7420
 *
 * PR55501/1   8/14/09 7:37p bandrews
 * PR55501: kernel mode support for 7420 with memc1 mappable from user
 *
 * 9   7/31/08 5:19p vsilyaev
 * PR 45137: Initial support for multi application mode
 *
 * 8   6/10/08 3:29p vsilyaev
 * PR 43184: Added support for Start/Stop callbacks in linux kernel/user
 *  proxy mode
 *
 * 7   6/5/08 2:19p vsilyaev
 * PR 42318: Rearranged scheduller to share the same context in user and
 *  kernel mode, this allows to serve callback from kernel to user with
 *  single transition kernel<->user
 *
 * 6   2/8/08 6:46p vsilyaev
 * PR 38682: Added support for 97405 platform
 *
 * 5   2/1/08 5:03p vsilyaev
 * PR 38682: Added tracking of open/close calls
 *
 * 4   1/30/08 4:28p vsilyaev
 * PR 38682: Improved handling of callbacks traversing user/kernel
 *  boundary
 *
 * 3   1/30/08 11:49a vsilyaev
 * PR 38682: Added routing of callbacks into the user space
 *
 * 2   1/29/08 5:55p vsilyaev
 * PR 38682: Support of callbacks in the linux kernel/user proxy
 *  environment
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 *
 ************************************************************/

#ifndef NEXUS_GENERIC_DRIVER_IMPL_H__
#define NEXUS_GENERIC_DRIVER_IMPL_H__

#include "nexus_generic_driver.h"
#include "nexus_generic_driver_impl.h"
#include "nexus_platform_server.h"
#include "driver/nexus_driver_prologue.h"
#include "nexus_driver_ioctl.h"
#include "blst_slist.h"
#include "bdbg_log.h"

void nexus_driver_callback_uninit(struct nexus_driver_module_header *header);

#define NEXUS_DRIVER_MAX_LOG_READERS    4

struct nexus_driver_state {
    struct nexus_generic_driver_init_settings settings;
    int16_t open_count;
    bool active; /* platform_init has been called */
    BKNI_MutexHandle lock; /* low level lock to protect driver state, no nexus functions shall be called whith lock held */
    NEXUS_PlatformStartServerSettings serverSettings;

    struct nexus_driver_client_state *server;
    bool uninit_pending; /* shutdown needs to be performed in non-signal_pending context */
    bool cleanup_pending; /* cleanup_clients has at least one entry */
    bool uninit_in_progress; /* driver is shutting down. no new opens allowed. */
    struct {
        BDBG_Fifo_Handle logWriter;
        struct {
            BDBG_FifoReader_Handle fifo;
        }readers[NEXUS_DRIVER_MAX_LOG_READERS];
    } debugLog;

    BLST_S_HEAD(nexus_driver_client_list, nexus_driver_client_state) clients; /* allowed, open */
    BLST_S_HEAD(nexus_driver_allowed_client_list, nexus_driver_client_state) allowed_clients; /* allowed, but not open */
    BLST_S_HEAD(nexus_driver_cleanup_client_list, nexus_driver_client_state) cleanup_clients; /* allowed, closed, need to be cleaned up */
};

extern struct nexus_driver_state nexus_driver_state;

BERR_Code nexus_driver_scheduler_init(void);
void nexus_driver_scheduler_uninit(void);
int nexus_driver_run_scheduler(NEXUS_ModulePriority priority, unsigned timeout, bool *p_has_callbacks, struct nexus_driver_slave_scheduler *slave);
unsigned nexus_driver_scheduler_dequeue(NEXUS_ModulePriority priority, nexus_driver_callback_desc *desc, unsigned nentries, struct nexus_driver_slave_scheduler *slave);
void nexus_driver_deactivate_callbacks(void *context, void *object, void *client);
void nexus_driver_scheduler_lock(void *client, NEXUS_ModulePriority priority, bool lock);
void nexus_driver_unlock_schedulers(void *client);

/* nexus_driver_objects */
struct nexus_driver_object_heap_entry *nexus_driver_module_object_get_newest(struct nexus_driver_module_header *header, unsigned class_no, void *client_id);
void nexus_driver_module_object_uninit(struct nexus_driver_module_header *header);
void nexus_driver_module_object_cleanup(struct nexus_driver_module_header *header);
void nexus_driver_module_object_uninit_one(struct nexus_driver_module_header *header, unsigned class_no, struct nexus_driver_object_heap_entry *object, void *client_id);

struct nexus_driver_slave_scheduler * nexus_driver_slave_scheduler_create(unsigned pid);
struct nexus_driver_slave_scheduler *nexus_driver_slave_scheduler_acquire(unsigned pid);
void nexus_driver_slave_scheduler_release(struct nexus_driver_module_header *header, struct nexus_driver_slave_scheduler *slave);

/* nexus_generic_driver.c funcs called from nexus_platform_server. */
struct nexus_driver_client_state *nexus_driver_create_client(const NEXUS_Certificate *pCertificate, const NEXUS_ClientConfiguration *pConfig);
void nexus_driver_destroy_client(struct nexus_driver_client_state *client);
void nexus_driver_disable_clients(bool including_server);
void nexus_driver_get_client_configuration(void *client_id, NEXUS_ClientConfiguration *pSettings);

void nexus_driver_module_init_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings);
void nexus_driver_module_uninit_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings);


#endif /* NEXUS_GENERIC_DRIVER_IMPL_H__ */


