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
 * $brcm_Workfile: b_objdb.c $
 * $brcm_Revision: 25 $
 * $brcm_Date: 10/26/12 9:17a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/base/src/b_objdb.c $
 * 
 * 25   10/26/12 9:17a erickson
 * SW7435-457: clear module->objdb to support interfaces created during
 *  module init and destroyed during module uninit
 * 
 * 24   10/12/12 11:56a erickson
 * SW7425-1792: revise b_objdb_find_object_in_module_and_lock for modules
 *  without objdb, without classes. don't require client on release.
 * 
 * 23   10/8/12 2:30p vsilyaev
 * SW7425-1792: Allow application to control life-cycle of objects
 * 
 * 22   8/17/12 12:21p erickson
 * SW7425-2734: meaning of b_objdb_verify_partial changed. ownership can
 *  only be bypassed for b_objdb_verify_none.
 * 
 * 21   8/7/12 10:35a erickson
 * SW7420-2332: fix race condition in b_objdb when cleaning up clients
 * 
 * 20   7/30/12 3:40p erickson
 * SW7435-256: remove release build warning
 * 
 * 19   7/23/12 11:35a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 18   7/16/12 1:01p vsilyaev
 * SW7420-2324: Use atomic verify and acquire for handle verification
 * 
 * 17   7/11/12 3:33p erickson
 * SW7231-890: auto-remove objects in a module in reverse order
 * 
 * 16   7/9/12 3:48p vsilyaev
 * SW7435-256: Use BLST_AAT implementation of AA Tree
 * 
 * 15   6/21/12 4:54p vsilyaev
 * SW7420-2315: Use binary search tree to validate object
 * 
 * 14   6/19/12 12:41p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 13   10/5/11 12:39p erickson
 * SW7420-1148: backing out /main/8. insert must be explicit. non-proxied
 *  servers require explicit nexus_register_Xxx and nexus_unregister_Xxx.
 * 
 * 12   10/4/11 4:39p erickson
 * SW7420-1992: allow b_objdb_remove to receive NULL client. results in
 *  general release/removal.
 * 
 * 11   9/29/11 7:03p bandrews
 * SW7420-2078: objdb now accepts NULL clients for verification
 * 
 * 10   9/16/11 3:40p erickson
 * SW7420-2064: clean up unprotected callbacks when client exits, allow
 *  re-registration when client connects
 * 
 * 9   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 8   9/2/11 10:01a erickson
 * SW7420-1148: allow implicit insert on acquire. allows non-thunked
 *  server to work with thunked client.
 * 
 * 7   7/26/11 1:33p erickson
 * SW7420-1992: add surfacecmp module and examples
 * 
 * 6   7/26/11 12:07p erickson
 * SW7420-1992: callback database must take client id so that callbacks
 *  can be disconnected when a client releases
 * 
 * 5   6/2/11 8:57a erickson
 * SW7420-1671: add more #include's
 * 
 * 4   6/1/11 9:05a erickson
 * SW7420-1671: add #include
 * 
 * 3   5/13/11 12:18p erickson
 * SW7420-1873: remove shutdown attribute
 * 
 * 2   4/7/11 5:26p erickson
 * SW7420-1671: correct comment
 *
 * 1   4/1/11 9:59a erickson
 * SW7420-1671: refactor object database for multiprocess stress
 *
 ************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "nexus_base.h"
#include "nexus_base_priv.h"
#include "b_objdb.h"

/* b_objdb is a standalone piece of code. it is very important that there is no #include
into some part of a nexus driver. it should have no dependency other than BDBG and NEXUS_ModuleHandle. */

BDBG_MODULE(b_objdb);
BDBG_OBJECT_ID(b_objdb);

#define BDBG_MSG_TRACE(X) 
#define LOCK(P_DB)   NEXUS_Module_Lock((P_DB)->module)
#define UNLOCK(P_DB) NEXUS_Module_Unlock((P_DB)->module)

static int NEXUS_P_BaseObject_Compare(const struct NEXUS_BaseObject* node, const struct NEXUS_BaseObject *key)
{
    if(key > node) {
        return 1;
    } else if(key==node) {
        return 0;
    } else {
        return -1;
    }
}

BLST_AA_TREE_GENERATE_FIND(NEXUS_P_BaseObjectTree , void *, NEXUS_BaseObject, node, NEXUS_P_BaseObject_Compare)
BLST_AA_TREE_GENERATE_INSERT(NEXUS_P_BaseObjectTree, void *, NEXUS_BaseObject, node, NEXUS_P_BaseObject_Compare)
BLST_AA_TREE_GENERATE_REMOVE(NEXUS_P_BaseObjectTree, NEXUS_BaseObject, node)

int  b_objdb_init(struct b_objdb *db, const struct b_objdb_class *class_list, NEXUS_ModuleHandle module)
{
    BKNI_Memset(db, 0, sizeof(*db));
    BDBG_OBJECT_SET(db, b_objdb);
    db->class_list = class_list;
    module->objdb = db;
    BLST_D_INIT(&db->object_list);
    BLST_AA_TREE_INIT(NEXUS_P_BaseObjectTree, &db->object_tree);
    return 0;
}

void b_objdb_uninit(struct b_objdb *db)
{
    BDBG_OBJECT_ASSERT(db, b_objdb);
    if (db->module) {
        db->module->objdb = NULL;
    }
    BDBG_OBJECT_DESTROY(db, b_objdb);
    return;
}

int b_objdb_insert_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_Object_P_RegisterUnregister operation)
{
    NEXUS_BaseObject *base_object;
    bool acquiring = operation==NEXUS_Object_P_RegisterUnregister_eAutoAcquire || operation == NEXUS_Object_P_RegisterUnregister_eRegisterAcquire;
    const struct b_objdb_client *client = b_objdb_get_client();

    BDBG_OBJECT_ASSERT(db, b_objdb);
    BDBG_MSG(("%s %s:%p client=%p", acquiring?"acquire":"insert", p_class->type_name, handle, client));
    BDBG_ASSERT(db->class_list);
    if (!client) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    base_object = (void *)((uint8_t *)handle + p_class->offset);
    BDBG_OBJECT_ASSERT(base_object, NEXUS_BaseObject);
    if(base_object->state.objdb_class==NULL) { 
        if(!acquiring) {
            const struct b_objdb_class *objdb_class;
            /* match objdb class information (release/close) with the NEXUS_BaseClassDescriptor */
            for(objdb_class = db->class_list;;objdb_class++) {
                if(objdb_class->base_class == p_class) {
                    base_object->state.objdb_class = objdb_class;
                    base_object->state.client = client;
                    base_object->state.order = db->order++; /* simplistic order. TODO: add a threshold to do compaction to avoid overflow */
                    BLST_D_INSERT_HEAD(&db->object_list, base_object, link);
                    BLST_AA_TREE_INSERT(NEXUS_P_BaseObjectTree, &db->object_tree, base_object, base_object);
#if BDBG_DEBUG_BUILD
                    {
                        NEXUS_BaseObject *object = BLST_AA_TREE_FIND(NEXUS_P_BaseObjectTree, &db->object_tree, base_object);
                        BDBG_ASSERT(base_object == object);
                    }
#endif
                    break;
                } else if(objdb_class->base_class==NULL) {
                    BDBG_ERR(("acquiring object of unknown class %s:%p", p_class->type_name, handle));
                    goto error;
                }
            }
        } else {
            BDBG_ERR(("acquiring object that wasn't inserted %s:%p", p_class->type_name, handle));
            goto error;
        }
    } else {
        BDBG_ASSERT(base_object->state.objdb_class);
        if(acquiring) {
            if (base_object->state.acquired_client) {
                BDBG_WRN(("object already acquired %s:%p", p_class->type_name, handle));
                goto error;
            } else {
                base_object->state.acquired_client = client;
            }
        } else {
            BDBG_WRN(("object already inserted %s:%p", p_class->type_name, handle));
            goto error;
        }
    }
    return 0;
error:
    return -1;
}


int b_objdb_remove_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_Object_P_RegisterUnregister operation)
{
    NEXUS_BaseObject *base_object;
    const struct b_objdb_class *objdb_class;
    const struct b_objdb_client *client = b_objdb_get_client();
    bool releasing = operation==NEXUS_Object_P_RegisterUnregister_eAutoRelease || operation == NEXUS_Object_P_RegisterUnregister_eUnregisterRelease;

    BDBG_OBJECT_ASSERT(db, b_objdb);
    BDBG_MSG(("%s %s:%p client=%#x", releasing?"release":"remove", p_class->type_name, handle, client));

    base_object = (void *)((uint8_t *)handle + p_class->offset);
    BDBG_OBJECT_ASSERT(base_object, NEXUS_BaseObject);
    db->cancel_callbacks_locked(db->cancel_callbacks_context, handle, releasing?client:NULL);
    objdb_class = base_object->state.objdb_class;
    if(objdb_class) {
        if (releasing) {
            if (base_object->state.acquired_client == client || !client) {
                base_object->state.acquired_client = NULL;
            } else {
                BDBG_ERR(("releasing object that is not acquired %s:%p", p_class->type_name, handle));
            }
        } else {
            if (client==NULL || client->verify == b_objdb_verify_none || base_object->state.client == client) {
                if (base_object->state.acquired_client) {
                    BDBG_WRN(("removing object which is still acquired %s:%p", p_class->type_name, handle));
                }
                base_object->state.client = NULL;
                base_object->state.acquired_client = NULL;
            } else {
                BDBG_ERR(("attempting to remove object that is not owned %s:%p", p_class->type_name, handle));
                goto error;
            }
        }
        if(base_object->state.client==NULL && base_object->state.acquired_client==NULL) {
            base_object->state.objdb_class = NULL;
            BLST_D_REMOVE(&db->object_list, base_object, link);
            BLST_AA_TREE_REMOVE(NEXUS_P_BaseObjectTree, &db->object_tree, base_object);
        }
    } else /* objdb_class==NULL */ {
        if(db->class_list) { /* if objdb was activated */
            if(!releasing) {
                BDBG_ERR(("removing unknown object %s:%p", p_class->type_name, handle));
                goto error;
            }
        }
    }
    return 0;
error:
    return -1;
}

static int  b_objdb_verify_object_locked(const struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, const struct b_objdb_client *client)
{
    NEXUS_BaseObject *base_object;
    NEXUS_BaseObject *base_object_handle;

    BDBG_OBJECT_ASSERT(db, b_objdb);

    BDBG_MSG_TRACE(("verify: %s:%p client=%#x", p_class->type_name, handle, client));

    base_object_handle = (void *)((uint8_t *)handle + p_class->offset); /* this object not verified and could be invalid */
    base_object = BLST_AA_TREE_FIND(NEXUS_P_BaseObjectTree, &db->object_tree, base_object_handle);
    if(base_object) {
        BDBG_ASSERT(base_object == base_object_handle); /* verify that we got the same thing */
        if(base_object->descriptor == p_class) {
            if (client) {
                if (client->verify == b_objdb_verify_none) {
                    /* don't need to check ownership. it exists, so it's good. */
                    goto done;
                }
                /* if you are the owner or have it acquired, you are allowed to use this handle */
                if (base_object->state.client == client || base_object->state.acquired_client == client) {
                    goto done;
                }
            } else {
                /* no client check. the handle/type exists, so we pass. */
                goto done;
            }
        }
    } else {
        /* verify that tree worked properly */
        for(base_object=BLST_D_FIRST(&db->object_list);base_object;base_object=BLST_D_NEXT(base_object, link)) {
            if(base_object==base_object_handle) {
                BDBG_ASSERT(0); /* we shouldn't be in the list */
            }
        }
    }
    BDBG_WRN(("unknown object: %s:%p", p_class->type_name, handle));
    return -1;
done:
    BDBG_MSG_TRACE(("verify: %s:%p client=%#x success", p_class->type_name, handle, client));
    return 0;
}

int  b_objdb_verify_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, const struct b_objdb_client *client)
{
    if (client && client->verify == b_objdb_verify_none) {
        return 0;
    }
    return b_objdb_verify_object_locked(db, p_class, handle, client);
}


int  b_objdb_verify_and_acquire(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_ModuleHandle target, const char *file, unsigned lineno)
{
    int rc;
    const struct b_objdb_client *client = b_objdb_get_client();

    if (client && client->verify == b_objdb_verify_none) {
        return 0;
    }

    if(target) {
        NEXUS_Module_Lock(target);
    }
    rc = b_objdb_verify_locked(db, p_class, handle, client);
    if(rc==0) {
        NEXUS_BaseObject_P_Acquire_Tagged(NULL, handle, p_class, NULL, file, lineno);
    }
    if(target) {
        NEXUS_Module_Unlock(target);
    }
    return rc;
}

void b_objdb_clear(struct b_objdb *db)
{
    NEXUS_BaseObject *base_object;
    BDBG_OBJECT_ASSERT(db, b_objdb);
    LOCK(db);
    while(NULL!=(base_object=BLST_D_FIRST(&db->object_list))) {
        BDBG_OBJECT_ASSERT(base_object, NEXUS_BaseObject);
        base_object->state.objdb_class = NULL;
        BLST_D_REMOVE_HEAD(&db->object_list, link);
    }
    /* don't remove nodes from tree, just wipe out tree */
    BLST_AA_TREE_INIT(NEXUS_P_BaseObjectTree, &db->object_tree);
    UNLOCK(db);
    return;
}

static int b_objdb_get_newest_entry_lock(struct b_objdb *db, const struct b_objdb_client *client, NEXUS_BaseObject **object)
{
    NEXUS_BaseObject *newest=NULL;
    int rc = -1;
    NEXUS_BaseObject *base_object;

    BDBG_OBJECT_ASSERT(db, b_objdb);

    for(base_object=BLST_D_FIRST(&db->object_list);base_object;base_object=BLST_D_NEXT(base_object, link)) {
        BDBG_OBJECT_ASSERT(base_object, NEXUS_BaseObject);
        BDBG_ASSERT(base_object->state.objdb_class);
        if (base_object->state.client != client && base_object->state.acquired_client != client) {
            continue;
        }
        if (newest==NULL || newest->state.order < base_object->state.order ) { /* find entry with maximum order */
            newest = base_object;
        }
    }
    if (newest) {
        *object = newest;
        rc = 0;
    }

    return rc;
}

static void b_objdb_uninit_entry_lock(struct b_objdb *db, NEXUS_BaseObject *base_object, const struct b_objdb_client *client)
{
    void *handle;
    const struct b_objdb_class *objdb_class;
    const NEXUS_BaseClassDescriptor *descriptor;

    BDBG_OBJECT_ASSERT(db, b_objdb);

    BDBG_OBJECT_ASSERT(base_object, NEXUS_BaseObject);
    descriptor = base_object->descriptor;
    objdb_class = base_object->state.objdb_class;
    handle = (void *)((uint8_t *)base_object - descriptor->offset); /* this object not verified and could be invalid */
    BDBG_ASSERT(objdb_class);
    if (base_object->state.acquired_client == client) {
        /* if it was acquired, it must have a release function registered */
        BDBG_MSG(("auto-release: [order %u] %s:%p client=%#x", base_object->state.order, descriptor->type_name, handle, client));
        db->cancel_callbacks_locked(db->cancel_callbacks_context, handle, client);
        BDBG_ASSERT(objdb_class->release);
        objdb_class->release(handle);
        base_object->state.acquired_client = NULL;
    }
    if (base_object->state.client == client) {
        BDBG_MSG(("auto-remove: [order %u] %s:%p client=%#x", base_object->state.order, descriptor->type_name, handle, client));
        /* remove from database prior to calling destructor */
        base_object->state.objdb_class = NULL;
        BLST_D_REMOVE(&db->object_list, base_object, link);
        BLST_AA_TREE_REMOVE(NEXUS_P_BaseObjectTree, &db->object_tree, base_object);
        db->cancel_callbacks_locked(db->cancel_callbacks_context, handle, NULL);
        objdb_class->destructor(handle);
    }
    return;
}

void b_objdb_uninit_client_objects(struct b_objdb *db, const struct b_objdb_client *client)
{
    LOCK(db);
    for(;;) {
        NEXUS_BaseObject *object;
        if (!b_objdb_get_newest_entry_lock(db, client, &object)) {
            b_objdb_uninit_entry_lock(db, object, client);
        }
        else {
            break;
        }
    }
    UNLOCK(db);
}

void b_objdb_uninit_client_callbacks(struct b_objdb *db, const struct b_objdb_client *client)
{
    BDBG_OBJECT_ASSERT(db, b_objdb);
    LOCK(db);
    (db->cancel_callbacks_locked)(db->cancel_callbacks_context, NULL, (void*)client);
    UNLOCK(db);
    return;
}

static struct b_objdb_client *b_objdb_default_client = NULL;

void b_objdb_set_default_client(struct b_objdb_client *client)
{
    b_objdb_default_client = client;
    return;
}

void b_objdb_set_client(struct b_objdb_client *client)
{
    NEXUS_P_ThreadInfo *info = NEXUS_P_ThreadInfo_Get();
    if(info) {
        info->client = client;
    }
}

const struct b_objdb_client *b_objdb_get_client(void)
{
    NEXUS_P_ThreadInfo *info = NEXUS_P_ThreadInfo_Get();
    if(info && info->client) {
        return info->client;
    }
    return b_objdb_default_client;
}

NEXUS_Error NEXUS_BaseObject_P_RegisterUnregister(void *object, const NEXUS_BaseClassDescriptor *descriptor, NEXUS_Object_P_RegisterUnregister operation, NEXUS_ModuleHandle origin, NEXUS_ModuleHandle destination)
{
    NEXUS_Error rc=NEXUS_SUCCESS;

    if(origin==destination) {
        BDBG_ASSERT(NEXUS_Module_Assert(origin));
    } else {
        NEXUS_Module_Lock(destination);
    }
    if(destination->objdb) {
        switch(operation) {
            case NEXUS_Object_P_RegisterUnregister_eRegisterOpen:
            case NEXUS_Object_P_RegisterUnregister_eRegisterCreate:
            case NEXUS_Object_P_RegisterUnregister_eRegisterAcquire:
            case NEXUS_Object_P_RegisterUnregister_eAutoCreate:
            case NEXUS_Object_P_RegisterUnregister_eAutoAcquire:
                rc = b_objdb_insert_locked(destination->objdb, descriptor, object, operation);
                break;
            case NEXUS_Object_P_RegisterUnregister_eUnregisterClose:
            case NEXUS_Object_P_RegisterUnregister_eUnregisterDestroy:
            case NEXUS_Object_P_RegisterUnregister_eUnregisterRelease:
            case NEXUS_Object_P_RegisterUnregister_eAutoDestroy:
            case NEXUS_Object_P_RegisterUnregister_eAutoRelease:
                rc = b_objdb_remove_locked(destination->objdb, descriptor, object, operation);
                break;
            default:
                BDBG_ASSERT(0);
                break;
        }
    }
    
    if(origin!=destination) {
        NEXUS_Module_Unlock(destination);
    }
    return rc;
}

NEXUS_BaseObject  *b_objdb_find_object_in_module_and_lock(NEXUS_ModuleHandle module, const struct b_objdb_client *client, const char *type_name, void *object)
{
    const struct b_objdb *db;
    const struct b_objdb_class *class;

    if (!module->objdb) return NULL; /* local modules may not have objdb */
    BDBG_ASSERT(module->objdb);
    db = module->objdb;
    if (!db->class_list) return NULL; /* modules with no classes */
    for(class = db->class_list;;class++) {
        const NEXUS_BaseClassDescriptor *base_class = class->base_class;
        if(base_class==NULL) break; 
        if(NEXUS_P_Base_StrCmp(base_class->type_name, type_name)!=0) {
            continue;
        }
        NEXUS_Module_Lock(module); /* lock modules only after locating match for type_name, this prevents locking with module that doens't have requiested type. And it's expected that caller synchronizes calls tio this function with calls to b_objdb_init  */
        /* if client == NULL, we only want to find and lock the module. don't look up in objdb because it could be gone and we know the obj is locked already. */
        if(client == NULL || b_objdb_verify_object_locked(db, base_class, object, client)==0) {
            NEXUS_BaseObject *base_object;

            base_object= (void *)((uint8_t *)object + base_class->offset); /* this object was already verified and should valid */
            return base_object;
        }
        NEXUS_Module_Unlock(module);
        break;
    }
    return NULL;
}

