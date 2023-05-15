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
 * $brcm_Workfile: nexus_client_resources.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/10/12 10:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/base/src/nexus_client_resources.c $
 * 
 * 2   8/10/12 10:34a erickson
 * SW7425-2734: when system shuts down, client may be NULL
 * 
 * 1   8/1/12 4:03p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 ************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "nexus_base.h"
#include "nexus_base_priv.h"
#include "nexus_client_resources.h"
#include "b_objdb.h"

BDBG_MODULE(nexus_client_resources);

int nexus_client_resources_Count_acquire(const char *name, unsigned offset, unsigned id) 
{
    struct b_objdb_client *client = (struct b_objdb_client *)b_objdb_get_client();
    int rc = 0;
    BSTD_UNUSED(id);
    BDBG_ASSERT(client);
    if (client->verify == b_objdb_verify_full) {
        NEXUS_ClientResourceCount *pUsed = (NEXUS_ClientResourceCount *)&((uint8_t*)&client->resources.used)[offset];
        NEXUS_ClientResourceCount *pAllowed = (NEXUS_ClientResourceCount *)&((uint8_t*)&client->resources.allowed)[offset];
        NEXUS_LockModule();
        if (pUsed->total+1 <= pAllowed->total) { 
            pUsed->total += 1; 
        }
        else {
            BDBG_ERR(("%s acquire rejected: %d already used, %d allowed", name, pUsed->total, pAllowed->total));
            rc = NEXUS_NOT_AVAILABLE; 
        }
        NEXUS_UnlockModule();
    }
    return rc;
}

void nexus_client_resources_Count_release(const char *name, unsigned offset, unsigned id) 
{
    struct b_objdb_client *client = (struct b_objdb_client *)b_objdb_get_client();
    BSTD_UNUSED(id);
    BSTD_UNUSED(name);
    /* when system shuts down, client may be NULL */
    if (client && client->verify == b_objdb_verify_full) {
        NEXUS_ClientResourceCount *pUsed = (NEXUS_ClientResourceCount *)&((uint8_t*)&client->resources.used)[offset];
        NEXUS_LockModule();
        BDBG_ASSERT(pUsed->total);
        pUsed->total--;
        NEXUS_UnlockModule();
    }
}

int nexus_client_resources_IdList_acquire(const char *name, unsigned offset, unsigned id) 
{
    struct b_objdb_client *client = (struct b_objdb_client *)b_objdb_get_client();
    int rc = 0;
    BDBG_ASSERT(client);
    if (client->verify == b_objdb_verify_full) {
        NEXUS_ClientResourceIdList *pUsed = (NEXUS_ClientResourceIdList *)&((uint8_t*)&client->resources.used)[offset];
        NEXUS_ClientResourceIdList *pAllowed = (NEXUS_ClientResourceIdList *)&((uint8_t*)&client->resources.allowed)[offset];
        unsigned i;
        NEXUS_LockModule();
        for (i=0;i<NEXUS_MAX_IDS;i++) {
            if (id == pAllowed->id[i]) {
                if (pUsed->id[i]) {
                    BDBG_ERR(("%s acquire rejected: ID %d already used", name, id));
                    rc = NEXUS_NOT_AVAILABLE;
                }
                else {
                    pUsed->id[i] = 1; /* mark as used */
                    rc = 0;
                }
                break;
            }
        }
        if (i == NEXUS_MAX_IDS) {
            BDBG_ERR(("%s acquire rejected: ID %d not allowed", name, id));
            rc = NEXUS_NOT_AVAILABLE;
        }
        NEXUS_UnlockModule();
    }
    return rc;
}

void nexus_client_resources_IdList_release(const char *name, unsigned offset, unsigned id) 
{
    struct b_objdb_client *client = (struct b_objdb_client *)b_objdb_get_client();
    /* when system shuts down, client may be NULL */
    if (client && client->verify == b_objdb_verify_full) {
        NEXUS_ClientResourceIdList *pUsed = (NEXUS_ClientResourceIdList *)&((uint8_t*)&client->resources.used)[offset];
        NEXUS_ClientResourceIdList *pAllowed = (NEXUS_ClientResourceIdList *)&((uint8_t*)&client->resources.allowed)[offset];
        unsigned i;
        NEXUS_LockModule();
        for (i=0;i<NEXUS_MAX_IDS;i++) {
            /* don't count, just verify ID. module code will prevent multiple acquires if that's a problem */
            if (id == pAllowed->id[i] && pUsed->id[i]) {
                pUsed->id[i] = 0;
                break;
            }
        }
        if (i == NEXUS_MAX_IDS) {
            BDBG_ERR(("%s bad IdList release: ", name, id));
        }
        NEXUS_UnlockModule();
    }
}
