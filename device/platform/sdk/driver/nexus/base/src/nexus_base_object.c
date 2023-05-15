/***************************************************************************
*     (c)2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_object.c $
* $brcm_Revision: 3 $
* $brcm_Date: 8/3/12 4:42p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/nexus_base_object.c $
* 
* 3   8/3/12 4:42p vsilyaev
* SW7420-2330: Added control for object reference counting
* 
* 2   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 1   6/19/12 12:39p vsilyaev
* SW7420-2085: NEXUS_OBJECT framework
* 
***************************************************************************/
#include "nexus_base.h"
#include "nexus_base_priv.h"
#include "nexus_base_os_types.h"
#include "bkni_multi.h"

BDBG_FILE_MODULE(nexus_base_object);
#define BDBG_MSG_OBJECT(x) BDBG_MODULE_MSG(nexus_base_object,x)

BDBG_OBJECT_ID(NEXUS_BaseObject);

void NEXUS_BaseObject_P_Init(NEXUS_BaseObject *object, const NEXUS_BaseClassDescriptor *descriptor)
{
    BDBG_ASSERT(object);
    BDBG_ASSERT(descriptor);
    BDBG_MSG_OBJECT(("%p: Init %p", descriptor, ((uint8_t *)object)-descriptor->offset));
    BDBG_OBJECT_INIT(object, NEXUS_BaseObject);
#if NEXUS_P_BASE_OBJECT_USE_REFCNT 
    object->ref_cnt = 1;
#endif
    object->descriptor = descriptor;
    object->state.objdb_class = NULL;
    object->state.order = 0;
    object->state.client = NULL;
    object->state.acquired_client = NULL;
    return;
}

#if NEXUS_P_BASE_OBJECT_USE_REFCNT 
static int b_add_ref_cnf(NEXUS_BaseObject *object, int add, NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    int result;
    if(module) {
        NEXUS_Module_Lock_Tagged(module, pFileName, lineNumber);
    }
    result = object->ref_cnt + add;
    object->ref_cnt = result;
    if(module) {
        NEXUS_Module_Unlock_Tagged(module, pFileName, lineNumber);
    }
    return result;
}

/* when this function called, locks for owner object is already held */
void NEXUS_BaseObject_P_Acquire_Tagged(NEXUS_BaseObject *owner, void *object, const NEXUS_BaseClassDescriptor *descriptor, NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    int result;
    NEXUS_BaseObject *base_object;

    BDBG_ASSERT(descriptor);
    BSTD_UNUSED(owner);

    base_object = (void *)((uint8_t *)object + descriptor->offset);
    BDBG_MSG_OBJECT(("%p: Acquire %p %u %s:%u", base_object->descriptor, object, base_object->ref_cnt, pFileName, lineNumber));
    result = b_add_ref_cnf(base_object, 1, module, pFileName, lineNumber);
    BDBG_ASSERT(result>0);
    return;
}

/* when this function called, locks for owner object is already held */
void NEXUS_BaseObject_P_Release_Tagged(NEXUS_BaseObject *owner, void *object, const NEXUS_BaseClassDescriptor *descriptor, NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    int result;
    NEXUS_BaseObject *base_object;

    BDBG_ASSERT(descriptor);
    BSTD_UNUSED(owner);

    base_object = (void *)((uint8_t *)object + descriptor->offset);
    BDBG_MSG_OBJECT(("%p: Release %p %u %s:%u", base_object->descriptor, object, base_object->ref_cnt, pFileName, lineNumber));
    result = b_add_ref_cnf(base_object, -1, module, pFileName, lineNumber);
    BDBG_ASSERT(result>=0);
    if(result<=0) {
        void (*finalizer)(void *);
        if(module) {
            NEXUS_Module_Lock_Tagged(module, pFileName, lineNumber);
        }
        BDBG_MSG_OBJECT(("%p: Finalize %p %u %s:%u", base_object->descriptor, object, base_object->ref_cnt, pFileName, lineNumber));
        BDBG_ASSERT(descriptor->finalizer);
        BDBG_ASSERT(base_object->state.objdb_class==NULL); /* by this time object should be already removed from the database, and if it's not object database would get corrupted */
        finalizer = descriptor->finalizer;
        BDBG_OBJECT_DESTROY(base_object, NEXUS_BaseObject); 
        finalizer(object);
        if(module) {
            NEXUS_Module_Unlock_Tagged(module, pFileName, lineNumber);
        }
    }
    return;
}
#endif /* NEXUS_P_BASE_OBJECT_USE_REFCNT */


