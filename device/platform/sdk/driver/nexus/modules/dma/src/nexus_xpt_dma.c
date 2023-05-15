/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_xpt_dma.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/1/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/dma/src/nexus_xpt_dma.c $
 * 
 * 1   10/1/12 3:30p jtna
 * SW7445-18: add stubs for nexus xpt dma
 * 
 **************************************************************************/
#include "nexus_dma_module.h"
#include "nexus_base.h"
#include "blst_squeue.h"
#include "nexus_class_verification.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_dma);
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

typedef struct NEXUS_DmaJob
{
    NEXUS_OBJECT(NEXUS_DmaJob);
} NEXUS_DmaJob;

typedef struct NEXUS_Dma
{
    NEXUS_OBJECT(NEXUS_Dma);
} NEXUS_Dma;

void NEXUS_Dma_GetDefaultSettings(
    NEXUS_DmaSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

static void NEXUS_Dma_P_Finalizer( NEXUS_DmaHandle handle)
{
    BSTD_UNUSED(handle);
}

static void NEXUS_Dma_P_Release(NEXUS_DmaHandle handle)
{
    BSTD_UNUSED(handle);
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_Dma, NEXUS_Dma_Close);

NEXUS_DmaHandle NEXUS_Dma_Open(
    unsigned index,
    const NEXUS_DmaSettings *pSettings  /* may be NULL for default settings */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    return 0;
}

NEXUS_Error NEXUS_Dma_GetSettings(
    NEXUS_DmaHandle handle,
    NEXUS_DmaSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Dma_SetSettings(
    NEXUS_DmaHandle handle,
    const NEXUS_DmaSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_SUCCESS;
}

void NEXUS_DmaJob_GetDefaultSettings(
    NEXUS_DmaJobSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

static void NEXUS_DmaJob_P_Finalizer(NEXUS_DmaJobHandle handle)
{
    BSTD_UNUSED(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_DmaJob, NEXUS_DmaJob_Destroy);

NEXUS_DmaJobHandle NEXUS_DmaJob_Create(
    NEXUS_DmaHandle dmaHandle,
    const NEXUS_DmaJobSettings *pSettings
    )
{
    BSTD_UNUSED(dmaHandle);
    BSTD_UNUSED(pSettings);
    return 0;
}

void NEXUS_DmaJob_GetSettings(
    NEXUS_DmaJobHandle handle,
    NEXUS_DmaJobSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_DmaJob_SetSettings(
    NEXUS_DmaJobHandle handle,
    const NEXUS_DmaJobSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
}

void NEXUS_DmaJob_GetDefaultBlockSettings(
    NEXUS_DmaJobBlockSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

void NEXUS_DmaJob_GetDefaultBlockOffsetSettings(
    NEXUS_DmaJobBlockOffsetSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_DmaJob_SetBlockSettings(
    NEXUS_DmaJobHandle handle,
    unsigned blockIndex,
    const NEXUS_DmaJobBlockSettings *pSettings
    )
{
    /* DEPRECATED: use NEXUS_DmaJob_ProcessBlocks() instead */
    BSTD_UNUSED(handle);
    BSTD_UNUSED(blockIndex);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_DmaJob_Start(
    NEXUS_DmaJobHandle handle
    )
{
    /* DEPRECATED: use NEXUS_DmaJob_ProcessBlocks() instead */
    BSTD_UNUSED(handle);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_DmaJob_GetStatus(
    NEXUS_DmaJobHandle handle,
    NEXUS_DmaJobStatus *pStatus
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_DmaJob_ProcessBlocks(
    NEXUS_DmaJobHandle handle, 
    const NEXUS_DmaJobBlockSettings *pSettings, 
    unsigned nBlocks
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(nBlocks);
    return NEXUS_SUCCESS;
}

NEXUS_Error 
NEXUS_DmaJob_ProcessBlocksOffset(
    NEXUS_DmaJobHandle handle,
    const NEXUS_DmaJobBlockOffsetSettings *pSettings,
    unsigned nBlocks
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(nBlocks);
    return NEXUS_SUCCESS;
}


/* used by nexus_playpump_crypto.c, nexus_recpump.c, etc */
NEXUS_Error
NEXUS_DmaJob_ProcessBlocks_priv(NEXUS_DmaJobHandle handle, const NEXUS_DmaJobBlockSettings *pSettings, unsigned nBlocks, BKNI_EventHandle event)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(nBlocks);
    BSTD_UNUSED(event);
    return NEXUS_SUCCESS;
}

