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
* $brcm_Workfile: nexus_driver_body.h $
* $brcm_Revision: 29 $
* $brcm_Date: 7/25/12 7:06p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel drivers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxkernel/driver/nexus_driver_body.h $
* 
* 29   7/25/12 7:06p vsilyaev
* SW7435-267: Added API statistics for linux kernel
* 
* 28   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 27   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 26   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 25   4/20/12 2:10p erickson
* SW7231-748: allow init/uninit platform functions to be call unlocked,
*  all others are locked
* 
* 24   11/29/11 11:37a erickson
* SW7420-2129: store current default heap per module
* 
* 23   11/1/11 9:57a erickson
* SW7231-391: add typecast
* 
* 22   9/2/11 10:03a erickson
* SW7420-1148: allow nexus_register/unregister to objdb from nexus
*  module. enables more complex resource management.
* 
* 21   7/26/11 12:07p erickson
* SW7420-1992: callback database must take client id so that callbacks
*  can be disconnected when a client releases
* 
* 20   5/13/11 12:17p erickson
* SW7420-1873: remove shutdown attribute
* 
* 19   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 18   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 17   3/2/11 11:35a erickson
* SW7420-1148: fix NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL
*
* 16   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 15   10/12/10 4:29p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 14   10/7/10 11:06a erickson
* SW7420-1148: add client_id
*
* 13   9/24/10 10:12a erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 12   8/12/10 8:14p vsilyaev
* SW7420-943: Added support for REFSW_FIRMWARE
*
* 11   8/4/09 5:14p vsilyaev
* PR 57223: Use proper type casting
*
* 10   2/6/09 10:23a erickson
* PR51868: fix warning
*
* 9   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 8   7/24/08 6:09p vsilyaev
* PR 45137: Modified INIT ioctl to pass application type
*
* 7   2/4/08 2:40p vsilyaev
* PR 38682: Added support for pointers with variable size
*
* 6   2/1/08 5:03p vsilyaev
* PR 38682: Added tracking of open/close calls
*
* 5   1/30/08 11:49a vsilyaev
* PR 38682: Added routing of callbacks into the user space
*
* 4   1/29/08 5:55p vsilyaev
* PR 38682: Support of callbacks in the linux kernel/user proxy
*  environment
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

#include "nexus_base_statistics.h"

static struct nexus_driver_module_state {
    struct nexus_driver_module_header header;
    union nexus_driver_module_args args;
} nexus_driver_module_state;

#define NEXUS_IOCTL_FAULT(call, arg)
#define NEXUS_IOCTL_CLEAR(ptr)

#if 0
#define NEXUS_IOCTL_ENTER(name) BDBG_WRN((">>%s", #name))
#define NEXUS_IOCTL_LEAVE(name) BDBG_WRN(("<<%s", #name))
#else
#define NEXUS_IOCTL_ENTER(name)
#define NEXUS_IOCTL_LEAVE(name) NEXUS_P_API_STATS_STOP(#name, NEXUS_MODULE_SELF)
#endif


/* route all address conversions through dedicated functions */
#define NEXUS_DRIVER_SEND_ADDR(addr,type)  nexus_driver_send_addr((void *)(&addr))
#define NEXUS_DRIVER_RECV_ADDR(addr,type)  nexus_driver_recv_addr_##type((void *)(&addr))

#define CLIENT_ID (&client->client)

#define NEXUS_DRIVER_CALLBACK_TO_DRIVER(callback, handle, id)  nexus_driver_callback_to_driver(&module->header, (callback), (void *)(handle),(id), CLIENT_ID, (((struct nexus_driver_module_driver_state *)context)->slave_scheduler))
#define NEXUS_DRIVER_CALLBACK_TO_USER(callback, handle, id) nexus_driver_callback_to_user(&module->header, (callback), (void*)(handle), (id))
#define NEXUS_DRIVER_CALLBACK_UPDATE(callback, old_handle, id, new_handle) nexus_driver_callback_update(&module->header, (callback), (void*)(old_handle), (id), (void *)(new_handle))
#define NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(callback, handle, id)  nexus_driver_callback_to_driver_commit(&module->header, (callback), (void *)(handle),(id))
#define NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(callback, handle, id)  nexus_driver_callback_to_driver_cancel(&module->header, (callback), (void *)(handle),(id))

#define P_CLASS(module, class_no) (&module->header.objdb.class_table[class_no])

#define NEXUS_DRIVER_CREATE_OBJECT(type, object) b_objdb_insert_locked(&module->header.objdb, &NEXUS_OBJECT_DESCRIPTOR(type), (void *)object, NEXUS_Object_P_RegisterUnregister_eAutoCreate)
#define NEXUS_DRIVER_DESTROY_OBJECT(type, object) b_objdb_remove_locked(&module->header.objdb, &NEXUS_OBJECT_DESCRIPTOR(type), (void *)object, NEXUS_Object_P_RegisterUnregister_eAutoDestroy)
#define NEXUS_DRIVER_ACQUIRE_OBJECT(type, object) b_objdb_insert_locked(&module->header.objdb, &NEXUS_OBJECT_DESCRIPTOR(type), (void *)object, NEXUS_Object_P_RegisterUnregister_eAutoAcquire)
#define NEXUS_DRIVER_RELEASE_OBJECT(type, object) b_objdb_remove_locked(&module->header.objdb, &NEXUS_OBJECT_DESCRIPTOR(type), (void *)object, NEXUS_Object_P_RegisterUnregister_eAutoRelease)

#define NEXUS_DRIVER_VERIFY_HEAP(heap) nexus_driver_module_heap_verify(nexus_driver_client_id(context), (heap), &(heap))

#define NEXUS_DRIVER_ALLOC(size) BKNI_Malloc(size)
#define NEXUS_DRIVER_FREE(ptr) nexus_driver_free(ptr)

int
nexus_driver_module_open(unsigned index)
{
    return nexus_driver_module_init(index, &nexus_driver_module_state.header, NEXUS_MODULE_SELF,
#ifdef NEXUS_DRIVER_MODULE_CLASS_TABLE
            NEXUS_DRIVER_MODULE_CLASS_TABLE,
#else
            NULL,
#endif
            NEXUS_DRIVER_MODULE_NAME);
}

void
nexus_driver_module_close(void)
{
    nexus_driver_module_uninit(&nexus_driver_module_state.header);
}

int
nexus_driver_module_ioctl(void *context, unsigned int cmd, unsigned long arg, unsigned long type, bool unlocked)
{
    int rc = 0;
    struct nexus_driver_module_state *module = &nexus_driver_module_state;
    struct nexus_driver_client_state *client = nexus_driver_client_id(context);
    NEXUS_P_API_STATS_STATE();

    BSTD_UNUSED(module);

    NEXUS_P_API_STATS_START(); /* this statistics count all API call overhead (including synchronization overhead) */

    if (!unlocked) {
        NEXUS_LockModule();
        b_objdb_set_client(CLIENT_ID);/* CLIENT_ID */
        NEXUS_Module_SetCurrentClient(NEXUS_MODULE_SELF, client->defaultHeap, client->defaultFullyMappedHeap, client->config.mode == NEXUS_ClientMode_eUntrusted);
    }
    
    if(nexus_driver_module_state.header.entry_count != 0) {
        goto err_reentrant;
        /* coverity[unreachable] - added to prevent warning in nexus_driver_epilogue.h */
        goto err_alloc;
    }

