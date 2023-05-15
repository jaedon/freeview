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
* $brcm_Workfile: nexus_server_body.h $
* $brcm_Revision: 16 $
* $brcm_Date: 8/3/12 1:17p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/server/nexus_server_body.h $
* 
* 16   8/3/12 1:17p vsilyaev
* SW7435-267: Enable API statistics in IPC server
* 
* 15   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 14   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 13   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 12   5/8/12 2:46p erickson
* SW7435-27: improve debug
* 
* 11   12/7/11 10:41a erickson
* SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
*  perl code as possible
* 
* 10   11/29/11 11:37a erickson
* SW7420-2129: store current default heap per module
* 
* 9   9/2/11 10:03a erickson
* SW7420-1148: allow nexus_register/unregister to objdb from nexus
*  module. enables more complex resource management.
* 
* 8   8/5/11 5:14p erickson
* SW7420-1123: it is normal that a module is compiled in but not
*  initialized. don't fail in NEXUS_Platform_P_InitServer.
* 
* 7   7/26/11 12:07p erickson
* SW7420-1992: callback database must take client id so that callbacks
*  can be disconnected when a client releases
* 
* 6   5/13/11 12:18p erickson
* SW7420-1873: remove shutdown attribute
* 
* 5   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 4   2/16/11 2:17p erickson
* SW7420-1123: implement nelem/nelem_out for IPC thunk
*
* 3   2/15/11 2:28p erickson
* SW7420-1123: refactor IPC thunk to separate in/out params
*
* 2   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
*
***************************************************************************/

#include "nexus_base_statistics.h"

#if 0
#define NEXUS_IPC_ENTER(name) BDBG_WRN((">>%s", #name))
#define NEXUS_IPC_LEAVE(name) BDBG_WRN(("<<%s", #name))
#else
#define NEXUS_IPC_ENTER(name)
#define NEXUS_IPC_LEAVE(name) NEXUS_P_API_STATS_STOP(#name, NEXUS_MODULE_SELF)
#endif

/* global storage for the module */
static struct nexus_driver_module_state {
    struct nexus_driver_module_header header;
} nexus_driver_module_state;


/* route all address conversions through dedicated functions */
#define NEXUS_DRIVER_SEND_ADDR(addr,type)  nexus_driver_send_addr((void *)(&addr))
#define NEXUS_DRIVER_RECV_ADDR(addr,type)  nexus_driver_recv_addr_##type((void *)(&addr))

#define CLIENT_ID (&client_module_state->client->client)

#define NEXUS_DRIVER_CALLBACK_TO_DRIVER(callback, handle, id)  nexus_driver_callback_to_driver(module_header, (callback), (void *)(handle), (id), CLIENT_ID, client_module_state->slave_scheduler)
#define NEXUS_DRIVER_CALLBACK_TO_CLIENT(callback, handle, id) nexus_driver_callback_to_user(module_header, (callback), (void*)(handle), (id))
#define NEXUS_DRIVER_CALLBACK_UPDATE(callback, old_handle, id, new_handle) nexus_driver_callback_update(module_header, (callback), (void*)(old_handle), (id), (void *)(new_handle))
#define NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(callback, handle, id)  nexus_driver_callback_to_driver_commit(module_header, (callback), (void *)(handle),(id))
#define NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(callback, handle, id)  nexus_driver_callback_to_driver_cancel(module_header, (callback), (void *)(handle),(id))

#define NEXUS_DRIVER_CREATE_OBJECT(type, object) b_objdb_insert_locked(&module_header->objdb, &NEXUS_OBJECT_DESCRIPTOR(type), object, NEXUS_Object_P_RegisterUnregister_eAutoCreate)
#define NEXUS_DRIVER_DESTROY_OBJECT(type, object) b_objdb_remove_locked(&module_header->objdb, &NEXUS_OBJECT_DESCRIPTOR(type), object, NEXUS_Object_P_RegisterUnregister_eAutoDestroy)
#define NEXUS_DRIVER_ACQUIRE_OBJECT(type, object) b_objdb_insert_locked(&module_header->objdb, &NEXUS_OBJECT_DESCRIPTOR(type), object, NEXUS_Object_P_RegisterUnregister_eAutoAcquire)
#define NEXUS_DRIVER_RELEASE_OBJECT(type, object) b_objdb_remove_locked(&module_header->objdb, &NEXUS_OBJECT_DESCRIPTOR(type), object, NEXUS_Object_P_RegisterUnregister_eAutoRelease)

#define NEXUS_DRIVER_VERIFY_HEAP(heap) nexus_driver_module_heap_verify(client_module_state->client, (heap), &(heap))

int
nexus_server_module_open(struct nexus_driver_module_header **pHeader)
{
    b_objdb_init(&nexus_driver_module_state.header.objdb,
#ifdef NEXUS_DRIVER_MODULE_CLASS_TABLE
        NEXUS_DRIVER_MODULE_CLASS_TABLE,
#else
        NULL,
#endif
       NEXUS_MODULE_SELF); 
    nexus_driver_module_state.header.objdb.name = NEXUS_SERVER_MODULE_NAME;
    nexus_driver_module_state.header.objdb.module = NEXUS_MODULE_SELF;
    nexus_driver_module_state.header.objdb.cancel_callbacks_locked = nexus_driver_deactivate_callbacks;
    nexus_driver_module_state.header.objdb.cancel_callbacks_context = &nexus_driver_module_state.header;

    nexus_driver_module_state.header.name = NEXUS_SERVER_MODULE_NAME;

    /* nexus_platform_server needs the thunk layer's header storage */
    *pHeader = &nexus_driver_module_state.header;
    nexus_driver_module_state.header.module = NEXUS_MODULE_SELF;
    if (nexus_driver_module_state.header.module) {
        return 0;
    }
    else {
        /* it is normal that a module is compiled in but not initialized. */
        return -1;
    }
}

int
nexus_server_module_close(void)
{
    b_objdb_uninit(&nexus_driver_module_state.header.objdb);
    BKNI_Memset(&nexus_driver_module_state.header, 0, sizeof(nexus_driver_module_state.header));
    return 0;
}

int
nexus_server_process(void *vclient, void *vin_data, unsigned in_data_size, void *vout_data, unsigned out_mem_size, unsigned *p_out_data_size)
{
    int rc = 0;
    struct nexus_driver_module_driver_state *client_module_state = vclient;
    struct nexus_driver_module_header *module_header = &nexus_driver_module_state.header;
    nexus_server_args *in_data = vin_data;
    nexus_server_args *out_data = vout_data;
    unsigned function_id;
    unsigned __variable_out_offset = 0;
    struct nexus_driver_client_state *client = client_module_state->client;
    NEXUS_P_API_STATS_STATE();

    BSTD_UNUSED(__variable_out_offset); /* may be unused */
    NEXUS_P_API_STATS_START(); /* this statistics count all API call overhead (including synchronization overhead) */

    if (!in_data) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    function_id = in_data->header.function_id;

    if (in_data->header.version != NEXUS_IPC_HEADER_VERSION) {
        BDBG_ERR(("func %#x: wrong version: %#x != %#x", function_id, in_data->header.version, NEXUS_IPC_HEADER_VERSION));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    NEXUS_Module_Lock(module_header->module);
    b_objdb_set_client(&client->client); /* CLIENT_ID */
    NEXUS_Module_SetCurrentClient(NEXUS_MODULE_SELF, client->defaultHeap, client->defaultFullyMappedHeap, client->config.mode == NEXUS_ClientMode_eUntrusted);

    switch(in_data->header.function_id) {
    default:
        goto err_invalid_ipc;
        /* coverity[unreachable] - added to prevent warning in nexus_driver_epilogue.h */
        goto err_alloc;
        /* coverity[unreachable] - added to prevent warning in nexus_driver_epilogue.h */
        goto err_fault;

