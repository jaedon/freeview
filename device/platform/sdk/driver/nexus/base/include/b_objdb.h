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
 * $brcm_Workfile: b_objdb.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 10/8/12 2:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/base/include/b_objdb.h $
 * 
 * 13   10/8/12 2:30p vsilyaev
 * SW7425-1792: Allow application to control life-cycle of objects
 * 
 * 12   8/7/12 10:35a erickson
 * SW7420-2332: fix race condition in b_objdb when cleaning up clients
 * 
 * 11   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 10   7/23/12 11:35a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 9   7/16/12 1:01p vsilyaev
 * SW7420-2324: Use atomic verify and acquire for handle verification
 * 
 * 8   7/11/12 3:33p erickson
 * SW7231-890: auto-remove objects in a module in reverse order
 * 
 * 7   6/21/12 4:54p vsilyaev
 * SW7420-2315: Use binary search tree to validate object
 * 
 * 6   6/19/12 12:41p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 5   9/16/11 3:40p erickson
 * SW7420-2064: clean up unprotected callbacks when client exits, allow
 *  re-registration when client connects
 * 
 * 4   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 3   9/2/11 10:01a erickson
 * SW7420-1148: allow implicit insert on acquire. allows non-thunked
 *  server to work with thunked client.
 * 
 * 2   7/26/11 12:07p erickson
 * SW7420-1992: callback database must take client id so that callbacks
 *  can be disconnected when a client releases
 * 
 * 1   4/1/11 9:59a erickson
 * SW7420-1671: refactor object database for multiprocess stress
 *
 ************************************************************/
#ifndef B_OBJDB_H__
#define B_OBJDB_H__

#include "bdbg.h"
#include "nexus_base_types.h"

/**
object database

this api is internal to nexus.
used by nexus kernel-mode proxy and user-mode multiprocess thunks.
it uses static allocation and transparent structs, not dynamic allocation and opaque handles.
**/

typedef enum b_objdb_verify
{
    b_objdb_verify_none,    /* do not verify anything (eUnprotected) */
    b_objdb_verify_partial, /* verify handles (eProtected) */
    b_objdb_verify_full,    /* verify handles and client->resources.allowed (eUntrusted) */   
    b_objdb_verify_max
} b_objdb_verify;

/* the address of the b_objdb_client instance is the id for the client */
struct b_objdb_client
{
    b_objdb_verify verify;
    bool limited_api;
    struct {
        NEXUS_ClientResources allowed;
        NEXUS_ClientResources used;
    } resources;
};

BDBG_OBJECT_ID_DECLARE(b_objdb);

struct b_objdb_class {
    const NEXUS_BaseClassDescriptor *base_class; /* used as a key to associate different information aboute the same object type (class) */
    void (*release)(void *); /* called when server/driver wants to release (dissasociate) object from a client/process */
    void (*destructor)(void *); /* called when server/driver wants to close object, it may not actually close object if there are further references to it */
};


/*
b_objdb is the object database. There is one instance of b_objdb per module.
There is one mutex (nexus module handle) per objdb.
*/
struct b_objdb {
    BDBG_OBJECT(b_objdb)
    BLST_D_HEAD(NEXUS_P_BaseObjectHead, NEXUS_BaseObject) object_list;
    unsigned order; /* ensure that objects in a module are auto-removed in reverse order of how they were inserted */
    struct NEXUS_P_BaseObjectTree object_tree;
    const char *name; /* module name for debug */
    const struct b_objdb_class *class_list;
    NEXUS_ModuleHandle module; /* provides synchronization for this DB. _locked functions already have this acquired. */

    /* callback to cancel callbacks for a handle */
    void (*cancel_callbacks_locked)(void *context, void *handle, const struct b_objdb_client *client); /* called with module lock already acquired */
    void *cancel_callbacks_context;
};

typedef void (*b_objdb_callback)(void *handle);


#define B_OBJDB_TABLE_BEGIN(NAME) static const struct b_objdb_class NAME[] = {
#define B_OBJDB_TABLE_END {NULL,NULL,NULL}};
#define B_OBJDB_TABLE_ENTRY(handletype,releasefunc,destructorfunc) {& handletype##_BaseObject_Descriptor, (void (*)(void *))releasefunc, (void (*)(void *))destructorfunc},

/* b_objdb_init will hook up the statically-declared class_table. the caller must still manually
set db->module, name, cancel_callbacks and cancel_callbacks_context */
int  b_objdb_init(struct b_objdb *db, const struct b_objdb_class *class_list, NEXUS_ModuleHandle module);
void b_objdb_uninit(struct b_objdb *db);

/* functions called from thunk. module lock is already acquired. */
int b_objdb_insert_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_Object_P_RegisterUnregister operation);
int b_objdb_remove_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_Object_P_RegisterUnregister operation);

int  b_objdb_verify_locked(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, const struct b_objdb_client *client);
int  b_objdb_verify_and_acquire(struct b_objdb *db, const NEXUS_BaseClassDescriptor *p_class, void *handle, NEXUS_ModuleHandle target, const char *file, unsigned lineno);

/* clear out all entries, but do not uninit */
void b_objdb_clear(struct b_objdb *db);

/* uninit all objects for this client in this db */
void b_objdb_uninit_client_objects(struct b_objdb *db, const struct b_objdb_client *client);

/* uninit all callbacks for this client */
void b_objdb_uninit_client_callbacks(struct b_objdb *db, const struct b_objdb_client *client);

/* this function sets default('server') client */
void b_objdb_set_default_client(struct b_objdb_client *client);

/* this function sets client _for_ a current process/thread */
void b_objdb_set_client(struct b_objdb_client *client);

/* this function return client _for_ a current process/thread or a default ('server') client if there is no active client set */
const struct b_objdb_client *b_objdb_get_client(void);

NEXUS_BaseObject  *b_objdb_find_object_in_module_and_lock(NEXUS_ModuleHandle module, const struct b_objdb_client *client, const char *type_name, void *object);

#endif /* B_OBJDB_H__ */
