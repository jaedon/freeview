/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: bipc_server.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/16/12 1:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/ipc/bipc_server.c $
 * 
 * 3   7/16/12 1:28p vsilyaev
 * SW7231-902: stated size of the IPC buffer should be inclusive of the
 *  header size
 * 
 * 2   2/20/12 10:12a erickson
 * SW7125-1238: fix potentially bad dereference
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 *****************************************************************************/
#include "bstd.h"
#include "bipc_impl.h"

BDBG_MODULE(bipc_server);

BDBG_OBJECT_ID(bipc_server);
BDBG_OBJECT_ID(bipc_server_client);

struct bipc_server_client {
    BDBG_OBJECT(bipc_server_client)
    BLST_D_ENTRY(bipc_server_client) link;
    unsigned max_instances;
    unsigned generation;
    bipc_server_client_create_settings create_settings;
    b_ipc_server_instance instances[1]; /* variable length */
};

void bipc_server_get_default_create_settings(bipc_server_create_settings *create_settings)
{
    BDBG_ASSERT(create_settings);
    BKNI_Memset(create_settings, 0, sizeof(*create_settings));
    create_settings->max_instances = 4;
    return;
}

void bipc_server_get_default_client_create_settings(bipc_server_client_create_settings *create_settings)
{
    BDBG_ASSERT(create_settings);
    BKNI_Memset(create_settings, 0, sizeof(*create_settings));
    return;
}

bipc_t bipc_server_create(const bipc_server_create_settings *create_settings)
{
    bipc_t ipc;
    unsigned i;
    size_t size;
    BERR_Code rc;

    BDBG_ASSERT(create_settings);
    BDBG_ASSERT(create_settings->interfaces);
    BDBG_ASSERT(create_settings->interface_count>0);
    for(size=sizeof(b_ipc_pkt_in),i=0;i<create_settings->interface_count;i++) {
        BDBG_ASSERT(create_settings->interfaces[i]);
        BDBG_ASSERT(create_settings->interfaces[i]->process);
        size += create_settings->interfaces[i]->interface.buf_size;
    }

    ipc = BKNI_Malloc(size + sizeof(*ipc));
    if(!ipc) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}

    BDBG_OBJECT_INIT(&ipc->t.server, bipc_server);
    BLST_D_INIT(&ipc->t.server.clients);
    ipc->t.server.ipc_buf_size = size;
    ipc->t.server.create_settings = *create_settings;
    rc = BKNI_CreateMutex(&ipc->t.server.lock);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_mutex;}

    return ipc;
err_mutex:
    BKNI_Free(ipc);
err_alloc:
    return NULL;
}


static void b_ipc_server_clean_client(bipc_t ipc, bipc_server_client_t client)
{
    BSTD_UNUSED(ipc);
    for(;;) {
        unsigned i;
        b_ipc_server_instance *newest_instance = NULL;
        for(i=0;i<client->max_instances;i++) {
            b_ipc_server_instance *instance = &client->instances[i];
            if(instance->interface) {
                if(newest_instance==NULL || newest_instance->generation < instance->generation) {
                    newest_instance = instance;
                }
            }
        }
        if(newest_instance) {
            size_t send_offset, send_size;

            BDBG_WRN(("bipc_server_destroy:%#lx cleaning up object %#lx(%d,%d):%s(%#lx)",(unsigned long)ipc, (unsigned long)client, client->create_settings.recv_fd, client->create_settings.send_fd, newest_instance->interface->interface.name, (unsigned long)newest_instance->object));
            newest_instance->interface->process(&newest_instance->object, newest_instance->interface->destructor_entry, ipc->t.server.buf, sizeof(unsigned), &send_offset, &send_size);
            newest_instance->interface = NULL;
            newest_instance->object = NULL;
        } else {
            break;
        }
    }
    BDBG_OBJECT_DESTROY(client, bipc_server_client);
    BKNI_Free(client);
    return;
}

void bipc_server_destroy(bipc_t ipc)
{
    bipc_server_client_t client;

    BDBG_OBJECT_ASSERT(&ipc->t.server, bipc_server);
    BKNI_AcquireMutex(ipc->t.server.lock);
    while(NULL!=(client=BLST_D_FIRST(&ipc->t.server.clients))) {
        BLST_D_REMOVE_HEAD(&ipc->t.server.clients, link);
        BDBG_WRN(("bipc_server_destroy:%#lx leaked client %#lx(%d,%d)",(unsigned long)ipc, (unsigned long)client, client->create_settings.recv_fd, client->create_settings.send_fd));
        b_ipc_server_clean_client(ipc, client);
    }
    BKNI_ReleaseMutex(ipc->t.server.lock);
    BKNI_DestroyMutex(ipc->t.server.lock);
    BDBG_OBJECT_DESTROY(&ipc->t.server, bipc_server);
    BKNI_Free(ipc);
    return;
}


static bipc_server_client_t b_ipc_server_find_client(bipc_t ipc, int recv_fd, int send_fd)
{
    bipc_server_client_t client;
    for(client=BLST_D_FIRST(&ipc->t.server.clients);client;client=BLST_D_NEXT(client, link)) {
        if(client->create_settings.recv_fd==recv_fd && client->create_settings.send_fd==send_fd) {
            return client;
        }
    }
    return NULL;
}

bipc_server_client_t bipc_server_client_create(bipc_t ipc, bipc_server_client_create_settings *create_settings)
{
    int rc=-1;
    unsigned i;
    bipc_server_client_t client;

    BDBG_OBJECT_ASSERT(&ipc->t.server, bipc_server);
    BDBG_ASSERT(create_settings);
    BKNI_AcquireMutex(ipc->t.server.lock);
    client = b_ipc_server_find_client(ipc, create_settings->recv_fd, create_settings->send_fd);
    if(client) {
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ERR(("bipc_server_add_client:%lx duplicated client for %d,%d(%#lx)", (unsigned long)ipc, create_settings->recv_fd, create_settings->send_fd, (unsigned long)client));
        goto done;
    }
    client = BKNI_Malloc(sizeof(*client)+ipc->t.server.create_settings.max_instances*sizeof(client->instances));
    if(client==NULL) { (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done;}
    BDBG_OBJECT_INIT(client, bipc_server_client);
    client->create_settings = *create_settings;
    client->max_instances = ipc->t.server.create_settings.max_instances;
    client->generation = 0;
    for(i=0;i<ipc->t.server.create_settings.max_instances;i++) {
        client->instances[i].interface = NULL;
        client->instances[i].object = NULL;
    }
    BLST_D_INSERT_HEAD(&ipc->t.server.clients, client, link);
    rc = 0;
done:
    BKNI_ReleaseMutex(ipc->t.server.lock);
    return client;
}

void bipc_server_client_destroy(bipc_t ipc, bipc_server_client_t client)
{
    BDBG_OBJECT_ASSERT(&ipc->t.server, bipc_server);
    BDBG_OBJECT_ASSERT(client, bipc_server_client);
    BKNI_AcquireMutex(ipc->t.server.lock);
    BLST_D_REMOVE(&ipc->t.server.clients, client, link);
    b_ipc_server_clean_client(ipc, client);
    BKNI_ReleaseMutex(ipc->t.server.lock);
    return;
}


int bipc_server_client_process(bipc_t ipc, bipc_server_client_t client)
{
    int result = 0;
    b_ipc_pkt_in *in;
    b_ipc_pkt_out out;
    int rc;
    size_t payload;
    const bipc_server_descriptor *descriptor=NULL;
    b_ipc_server_instance *instance=NULL;
    void *process_object;
    size_t send_offset;
    size_t send_size;

    BDBG_OBJECT_ASSERT(&ipc->t.server, bipc_server);
    BDBG_OBJECT_ASSERT(client, bipc_server_client);

    BKNI_AcquireMutex(ipc->t.server.lock);
    in = (void *)ipc->t.server.buf;

    rc = b_safe_read(client->create_settings.recv_fd, in, sizeof(*in));
    if(rc==0) {
        goto done;
    } else if (rc==-(int)BERR_END_OF_FILE) {
        goto error;
    } else if(rc!=sizeof(*in)) { 
        (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto error;
    }

    if(in->pkt_size >= ipc->t.server.ipc_buf_size || in->pkt_size<sizeof(*in)) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    payload = in->pkt_size - sizeof(*in);
    rc = b_safe_read(client->create_settings.recv_fd, ipc->t.server.buf+sizeof(*in), payload);
    if(rc!=(int)payload) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    if(in->instance==BIPC_INSTANCE_ID_NEW) {
        unsigned i;
        bipc_interface_descriptor *id = (void *)(ipc->t.server.buf+sizeof(*in));

        if(payload<sizeof(bipc_interface_descriptor)) { (void)BERR_TRACE(BERR_NOT_SUPPORTED);goto error;}
        for(i=0;i<ipc->t.server.create_settings.interface_count;i++) {
            descriptor = ipc->t.server.create_settings.interfaces[i];
            if(BKNI_Memcmp(descriptor->interface.name, id->name, sizeof(id->name))==0) {
                if(BKNI_Memcmp(descriptor->interface.sha1, id->sha1, sizeof(id->sha1))!=0) {
                    BDBG_WRN(("bipc_server_process:%#lx mismatched protocol for interface %s client:%#lx", (unsigned long)ipc, descriptor->interface.name, (unsigned long)client));
                    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
                    goto error;
                }
                break;
            }
            descriptor = NULL;
        }
        if(descriptor == NULL) {
            BDBG_WRN(("bipc_server_process:%#lx unknown interface client:%#lx", (unsigned long)ipc, (unsigned long)client));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            BDBG_ASSERT(0);
            goto error;
        }
        if(in->method != descriptor->constructor_entry) {
            BDBG_WRN(("bipc_server_process:%#lx %s invalid constructor %u(%u) client:%#lx", (unsigned long)ipc, descriptor->interface.name, in->method, descriptor->constructor_entry, (unsigned long)client));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        for(i=0;i<client->max_instances;i++) {
            if(client->instances[i].interface==NULL) {
                instance = &client->instances[i];
                send_offset = i; /* used as object ID communicated back to the client */
                break;
            }
        }
        if(instance==NULL) {
            BDBG_WRN(("bipc_server_process:%#lx %s not enough space client:%lx", (unsigned long)ipc, descriptor->interface.name, (unsigned long)client));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        process_object = client->create_settings.ipc_context;
    } else {
        if(in->instance >= client->max_instances) {
            BDBG_WRN(("bipc_server_process:%#lx invalid instance %u:%u", (unsigned long)ipc, in->instance, client->max_instances));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        instance = &client->instances[in->instance];
        if(instance == NULL) {
            BDBG_WRN(("bipc_server_process:%#lx unknown instance %u", (unsigned long)ipc, in->instance));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        process_object = instance->object;
        descriptor = instance->interface;
        if(in->method == descriptor->constructor_entry) {
            BDBG_WRN(("bipc_server_process:%#lx invalid constructor call %s client:%#lx", (unsigned long)ipc, descriptor->interface.name, (unsigned long)client));
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
    }
    BDBG_ASSERT(descriptor);
    BDBG_ASSERT(instance);
    rc  =  descriptor->process(&process_object, in->method, ipc->t.server.buf+sizeof(*in),  payload, &send_offset, &send_size);
    if(rc!=0) {
        BDBG_WRN(("bipc_server_process:%#lx error executing method %s:%u client:%#lx", (unsigned long)ipc, descriptor->interface.name, in->method, (unsigned long)client));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        goto error;
    }
    out.pkt_size = send_size + sizeof(out);
    rc =  b_safe_write(client->create_settings.send_fd, &out, sizeof(out));
    if(rc!=sizeof(out)) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    if(in->method == descriptor->constructor_entry) {
        client->generation++;
        instance->generation = client->generation;
        instance->interface = descriptor;
        instance->object = process_object;
    } else if(in->method == descriptor->destructor_entry) {
        instance->interface = NULL;
        instance->object = NULL;
    }
    if(send_size) {
        rc = b_safe_write(client->create_settings.send_fd, ipc->t.server.buf + sizeof(*in) + send_offset , send_size);
        if(rc!=(int)send_size) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    }
done:
    BKNI_ReleaseMutex(ipc->t.server.lock);
    return result;
error:
    result = -1;
    goto done;
}

