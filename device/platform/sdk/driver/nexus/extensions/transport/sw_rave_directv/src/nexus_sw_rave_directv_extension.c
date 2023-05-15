/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_sw_rave_directv_extension.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 11/1/11 10:45a $
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/transport/sw_rave_directv/src/nexus_sw_rave_directv_extension.c $
 * 
 * 5   11/1/11 10:45a cnovak
 * SW7346-535: Fix CDB buffer size calculation.
 * 
 * 4   9/14/11 11:25a mmussol
 * SW7346-144 : Modify API to get/write CDB only (not ITB)
 * 
 * 3   9/13/11 1:01p mmussol
 * SW7346-144 : Allow per-rave custom handler. Add APIs to get/set rave
 *  ITB/CDB.
 * 
 * 2   6/8/11 5:16p minlin
 * SW7346-144: Move XPT register read/write to Nexus level.
 * 
 * 1   5/5/11 3:39p minlin
 * SW7346-144: Create sw_rave_directv
 * 
 *
 **************************************************************************/
#include "nexus_sw_rave_extension.h"
#include "nexus_sw_rave_directv_extension.h"
#include "nexus_memory.h"
#include "priv/nexus_core.h"
#include "bchp_xpt_rave.h"

BDBG_MODULE(nexus_sw_rave_directv_extension);

#define BDBG_MSG_FLOW(X) /* BDBG_WRN(X) */

/* The follow is template code. It is assumed you will modify it freely for your custom SW RAVE implementation. */

#define CDB_READ_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define CDB_BASE_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define CDB_END_PTR_OFFSET      ( BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define CDB_WRAP_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define CDB_VALID_PTR_OFFSET    ( BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define CDB_DEPTH_OFFSET        ( BCHP_XPT_RAVE_CX0_AV_CDB_DEPTH - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )

#define ITB_READ_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define ITB_BASE_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define ITB_END_PTR_OFFSET      ( BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define ITB_WRAP_PTR_OFFSET     ( BCHP_XPT_RAVE_CX0_AV_ITB_WRAPAROUND_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define ITB_VALID_PTR_OFFSET    ( BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )
#define ITB_DEPTH_OFFSET        ( BCHP_XPT_RAVE_CX0_AV_ITB_DEPTH - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR )

struct private_data
{
    uint32_t src_base_addr;
    uint32_t dest_base_addr;
    sw_rave_custom_handler handler;
    void *handler_context;
};

static NEXUS_RaveHandle sw_rave_nexus_handles[NEXUS_NUM_RAVE_CONTEXTS] = { NULL };

NEXUS_Error NEXUS_P_SwRaveExtension_Init( NEXUS_RaveHandle rave )
{
    struct private_data *pData;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);

    rave->swRave.extensionData = pData = BKNI_Malloc(sizeof(*pData));
    if (!rave->swRave.extensionData) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(pData, 0, sizeof(*pData));

    /* "WRITE" is the base register for CDB/ITB blocks, but is not available from context map. */
    pData->src_base_addr = rave->xptContextMap.CDB_Read-4;
    pData->dest_base_addr = rave->swRave.xptContextMap.CDB_Read-4;
    pData->handler = NULL;
    pData->handler_context = NULL;

    sw_rave_nexus_handles[rave->index] = rave;
    return 0;
}

void NEXUS_P_SwRaveExtension_Uninit( NEXUS_RaveHandle rave )
{
    sw_rave_nexus_handles[rave->index] = NULL;

    if (rave->swRave.extensionData) {
        BKNI_Free(rave->swRave.extensionData);
        rave->swRave.extensionData = NULL;
    }
}

NEXUS_Error NEXUS_SwRaveExtension_Set_Custom_handler(unsigned rave_index, void *handler, void *handler_context)
{
    struct private_data *pData;

    if (rave_index >= NEXUS_NUM_RAVE_CONTEXTS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (sw_rave_nexus_handles[rave_index] != NULL) {
        pData = sw_rave_nexus_handles[rave_index]->swRave.extensionData;
        if (pData == NULL) {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
        pData->handler_context = handler_context;
        pData->handler = handler;
    }

    return NEXUS_SUCCESS;
}

static void NEXUS_SwRaveExtension_P_ReadRegs(struct private_data *pData, RAVE_REGS *regs)
{
    BREG_Handle regHandle = g_pCoreHandles->reg;

    /* CDB - always do this before ITB, and snapshot them as close as possible */
    regs->src_cdb_valid = BREG_Read32(regHandle, pData->src_base_addr + CDB_VALID_PTR_OFFSET );
    regs->src_itb_valid = BREG_Read32(regHandle, pData->src_base_addr + ITB_VALID_PTR_OFFSET );

    regs->src_cdb_read = BREG_Read32(regHandle, pData->src_base_addr + CDB_READ_PTR_OFFSET);
    regs->src_cdb_base = BREG_Read32(regHandle, pData->src_base_addr + CDB_BASE_PTR_OFFSET);
    regs->src_cdb_end  = BREG_Read32(regHandle, pData->src_base_addr + CDB_END_PTR_OFFSET);
    regs->src_cdb_wrap = BREG_Read32(regHandle, pData->src_base_addr + CDB_WRAP_PTR_OFFSET);

    regs->src_itb_read = BREG_Read32(regHandle, pData->src_base_addr + ITB_READ_PTR_OFFSET);
    regs->src_itb_base = BREG_Read32(regHandle, pData->src_base_addr + ITB_BASE_PTR_OFFSET);
    regs->src_itb_end  = BREG_Read32(regHandle, pData->src_base_addr + ITB_END_PTR_OFFSET);
    regs->src_itb_wrap = BREG_Read32(regHandle, pData->src_base_addr + ITB_WRAP_PTR_OFFSET );

    regs->dest_cdb_read = BREG_Read32(regHandle, pData->dest_base_addr + CDB_READ_PTR_OFFSET);
    regs->dest_cdb_base = BREG_Read32(regHandle, pData->dest_base_addr + CDB_BASE_PTR_OFFSET);
    regs->dest_cdb_end  = BREG_Read32(regHandle, pData->dest_base_addr + CDB_END_PTR_OFFSET);
    regs->dest_cdb_valid = BREG_Read32(regHandle, pData->dest_base_addr + CDB_VALID_PTR_OFFSET);

    regs->dest_itb_read = BREG_Read32(regHandle, pData->dest_base_addr + ITB_READ_PTR_OFFSET );
    regs->dest_itb_base = BREG_Read32(regHandle, pData->dest_base_addr + ITB_BASE_PTR_OFFSET);
    regs->dest_itb_end  = BREG_Read32(regHandle, pData->dest_base_addr + ITB_END_PTR_OFFSET);
    regs->dest_itb_valid = BREG_Read32(regHandle, pData->dest_base_addr + ITB_VALID_PTR_OFFSET );

    regs->dest_cdb_wrap = regs->src_cdb_wrap;
    regs->dest_itb_wrap = regs->src_itb_wrap;
}

/* Note: this function only support BXPT_RaveSoftMode_ePointersOnly, all other modes will cause unexpected result */
void NEXUS_P_SwRaveExtension_Advance( NEXUS_RaveHandle rave )
{
    struct private_data *pData;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    
    pData = rave->swRave.extensionData;

    if (pData) {
        BREG_Handle regHandle = g_pCoreHandles->reg;
        uint32_t reg, overflow;
        RAVE_REGS regs;

        /* Copy over the overflow bits. Decoder needs them. */
        /* CDB */
        reg = BREG_Read32(regHandle, pData->src_base_addr + CDB_DEPTH_OFFSET );
        overflow = BCHP_GET_FIELD_DATA( reg, XPT_RAVE_CX0_AV_CDB_DEPTH, CDB_OVERFLOW );
        reg = BREG_Read32(regHandle, pData->dest_base_addr + CDB_DEPTH_OFFSET );
        reg &= ~( BCHP_MASK( XPT_RAVE_CX0_AV_CDB_DEPTH, CDB_OVERFLOW ) );
        reg |= ( BCHP_FIELD_DATA( XPT_RAVE_CX0_AV_CDB_DEPTH, CDB_OVERFLOW, overflow ) );
        BREG_Write32(regHandle, pData->dest_base_addr + CDB_DEPTH_OFFSET, reg);

        /* ITB */
        reg = BREG_Read32(regHandle, pData->src_base_addr + ITB_DEPTH_OFFSET );
        overflow = BCHP_GET_FIELD_DATA( reg, XPT_RAVE_CX0_AV_ITB_DEPTH, ITB_OVERFLOW );
        reg = BREG_Read32(regHandle, pData->dest_base_addr + ITB_DEPTH_OFFSET );
        reg &= ~( BCHP_MASK( XPT_RAVE_CX0_AV_ITB_DEPTH, ITB_OVERFLOW ) );
        reg |= ( BCHP_FIELD_DATA( XPT_RAVE_CX0_AV_ITB_DEPTH, ITB_OVERFLOW, overflow ) );
        BREG_Write32(regHandle, pData->dest_base_addr + ITB_DEPTH_OFFSET, reg);

        /* CDB - always do this before ITB, and snapshot them as close as possible */
        NEXUS_SwRaveExtension_P_ReadRegs(pData, &regs);

        if (pData->handler && (pData->handler(&regs, pData->handler_context) == 0)){
            // Should update in the last
            /* produce: advance the dest VALID and WRAP pointers.
               always write WRAP before VALID, always produce on dest before consuming on src */
            BREG_Write32(regHandle, pData->dest_base_addr + CDB_WRAP_PTR_OFFSET, regs.dest_cdb_wrap);
            BREG_Write32(regHandle, pData->dest_base_addr + CDB_VALID_PTR_OFFSET, regs.dest_cdb_valid);

            /* consume: advance the src READ pointer */
            BREG_Write32(regHandle, pData->src_base_addr + CDB_READ_PTR_OFFSET, regs.dest_cdb_read);
    
            /* produce: advance the dest VALID and WRAP pointers */
            BREG_Write32(regHandle, pData->dest_base_addr + ITB_WRAP_PTR_OFFSET, regs.dest_itb_wrap);
            BREG_Write32(regHandle, pData->dest_base_addr + ITB_VALID_PTR_OFFSET, regs.dest_itb_valid);
    
            /* consume: advance the src READ pointer */
            BREG_Write32(regHandle, pData->src_base_addr + ITB_READ_PTR_OFFSET, regs.dest_itb_read);
        } else {
            /* Just copy source values to dest */
            BREG_Write32(regHandle, pData->dest_base_addr + CDB_WRAP_PTR_OFFSET, regs.src_cdb_wrap);
            BREG_Write32(regHandle, pData->dest_base_addr + CDB_VALID_PTR_OFFSET, regs.src_cdb_valid);

            /* consume: advance the src READ pointer */
            BREG_Write32(regHandle, pData->src_base_addr + CDB_READ_PTR_OFFSET, regs.dest_cdb_read);

            /* produce: advance the dest VALID and WRAP pointers */
            BREG_Write32(regHandle, pData->dest_base_addr + ITB_WRAP_PTR_OFFSET, regs.src_itb_wrap);
            BREG_Write32(regHandle, pData->dest_base_addr + ITB_VALID_PTR_OFFSET, regs.src_itb_valid);

            /* consume: advance the src READ pointer */
            BREG_Write32(regHandle, pData->src_base_addr + ITB_READ_PTR_OFFSET, regs.dest_itb_read);
        }
    }
}

NEXUS_Error
NEXUS_SwRaveExtension_GetDataBuffer(unsigned rave_index, void **pBuffer, size_t *pSize)
{
    struct private_data *pData;
    RAVE_REGS regs;

    if (rave_index >= NEXUS_NUM_RAVE_CONTEXTS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (sw_rave_nexus_handles[rave_index] != NULL) {
        pData = sw_rave_nexus_handles[rave_index]->swRave.extensionData;
        if (pData == NULL) {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
        NEXUS_SwRaveExtension_P_ReadRegs(pData, &regs);

        *pBuffer = NEXUS_OffsetToCachedAddr(regs.dest_cdb_base);
        *pSize = (regs.dest_cdb_end - regs.dest_cdb_base + 1);
    } else {
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    return NEXUS_SUCCESS;
}

static int
NEXUS_SwRaveExtension_P_CheckForStartcode(unsigned char data, int sccount)
{
    switch (data) {
    case 0:
        if (sccount >= 1) {
            sccount = 2;
        } else {
            sccount = 1;
        }
        break;
    case 1:
        if (sccount == 2) {
            /* we've got a start code! */ 
            sccount = 3;
        } else {
            sccount = 0;
        }
        break;
    default:
        sccount = 0;
        break;
    }
    return sccount;
}

static void
NEXUS_SwRaveExtension_P_GenerateItb(uint8_t startcode, unsigned long offset, uint32_t *buffer)
{
    /* construct Unify ITB */
    /* base address */
    buffer[0] = 0x20000000; /* Unify ITB Format - Base Address Entry Type = 0x20 */
    buffer[1] = offset - (offset % 8);
    buffer[2] = 0;
    buffer[3] = 0;

    /* video start code */
    buffer[4] = 0x00000000;         /* Unify ITB Format - INPUT_ES_FORMAT [27:24] = 0 for MPEG2/AVC Video */
    buffer[4] |= 1;                 /* Comparator # for pattern 0 */
    buffer[5] = (uint32_t)startcode << 24;  /* Comparator Pattern 0 extracted byte - one start code at offset 0 from base */
    buffer[5] |= (offset%8) << 16;  /* Comparator Pattern 0 offset */
    buffer[5] |= 0x000000FF;        /* terminate */
    buffer[6] = 0;
    buffer[7] = 0;
}

static NEXUS_Error
NEXUS_SwRaveExtension_P_FillVideoItb(void *es_data, unsigned es_data_count, uint32_t cdb_base_offset,
                                     void *itbAddr, unsigned itbSize, uint32_t *itbCount)
{
    unsigned offset = 0;
    int sccount = 0;

    *itbCount = 0;
    while (offset < es_data_count) {
        uint8_t sc;
        sc = ((uint8_t*) es_data)[offset];
        if (sccount == 3) {
            if ((*itbCount + 32) >= itbSize) {
                /* ITB overflow */
                return BERR_TRACE(NEXUS_UNKNOWN);
            }
            NEXUS_SwRaveExtension_P_GenerateItb(sc, cdb_base_offset + offset, (uint32_t*)&((uint8_t*)itbAddr)[*itbCount]);
            *itbCount += 32;
            sccount = 0;
        }
        sccount = NEXUS_SwRaveExtension_P_CheckForStartcode(sc, sccount);
        offset++;
    }

    return NEXUS_SUCCESS;
}


NEXUS_Error
NEXUS_SwRaveExtension_WriteComplete(unsigned rave_index, size_t amountUsed)
{
    struct private_data *pData;
    BREG_Handle regHandle = g_pCoreHandles->reg;
    RAVE_REGS regs;
    void *cdbAddr, *itbAddr;
    size_t itbSize = 0, itbCount = 0;
    
    if (rave_index >= NEXUS_NUM_RAVE_CONTEXTS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    if (sw_rave_nexus_handles[rave_index] != NULL) {
        pData = sw_rave_nexus_handles[rave_index]->swRave.extensionData;
        if (pData == NULL) {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
        NEXUS_SwRaveExtension_P_ReadRegs(pData, &regs);

        cdbAddr = NEXUS_OffsetToCachedAddr(regs.dest_cdb_base);
        itbAddr = NEXUS_OffsetToCachedAddr(regs.dest_itb_base);
        itbSize = (regs.dest_itb_end - regs.dest_itb_base);

        if (NEXUS_SwRaveExtension_P_FillVideoItb(cdbAddr, amountUsed, regs.dest_cdb_base,
                                                itbAddr, itbSize, &itbCount) != 0) {
            return BERR_TRACE(NEXUS_UNKNOWN);
        }

        NEXUS_Memory_FlushCache(cdbAddr, amountUsed);
        NEXUS_Memory_FlushCache(itbAddr, itbCount);

        /* Move CDB/ITB read pointers back to the base */
        BREG_Write32(regHandle, pData->dest_base_addr + CDB_READ_PTR_OFFSET, regs.dest_cdb_base);
        BREG_Write32(regHandle, pData->dest_base_addr + ITB_READ_PTR_OFFSET, regs.dest_itb_base);

        /* Set the CDB valid pointer */
        BREG_Write32(regHandle, pData->dest_base_addr + CDB_VALID_PTR_OFFSET, regs.dest_cdb_base + amountUsed - 1);

        /* Set the ITB valid pointer */
        BREG_Write32(regHandle, pData->dest_base_addr + ITB_VALID_PTR_OFFSET, regs.dest_itb_base + itbCount - 1);
    } else {
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    return NEXUS_SUCCESS;
}

