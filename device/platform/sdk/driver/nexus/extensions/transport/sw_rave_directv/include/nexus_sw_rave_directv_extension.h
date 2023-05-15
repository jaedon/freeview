/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_sw_rave_directv_extension.h $
* $brcm_Revision: 4 $
* $brcm_Date: 9/14/11 11:25a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/extensions/transport/sw_rave_directv/include/nexus_sw_rave_directv_extension.h $
* 
* 4   9/14/11 11:25a mmussol
* SW7346-144 : Modify API to get/write CDB only (not ITB)
* 
* 3   9/13/11 1:01p mmussol
* SW7346-144 : Allow per-rave custom handler. Add APIs to get/set rave
*  ITB/CDB.
* 
* 2   6/8/11 5:13p minlin
* SW7346-144: Move XPT register read/write to Nexus level.
* 
* 1   5/5/11 3:39p minlin
* SW7346-144: Create sw_rave_directv
* 
*
***************************************************************************/
#ifndef NEXUS_SW_RAVE_DIRECTV_EXTENSION_H__
#define NEXUS_SW_RAVE_DIRECTV_EXTENSION_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RAVE_REGS {
        uint32_t src_cdb_valid,  src_cdb_wrap, src_cdb_read, src_cdb_base, src_cdb_end;
        uint32_t src_itb_valid,  src_itb_wrap, src_itb_read, src_itb_base, src_itb_end;
        uint32_t dest_cdb_valid, dest_cdb_wrap, dest_cdb_read, dest_cdb_base, dest_cdb_end;
        uint32_t dest_itb_valid, dest_itb_wrap, dest_itb_read, dest_itb_base, dest_itb_end;
} RAVE_REGS;

typedef int (*sw_rave_custom_handler)(RAVE_REGS *rave_regs, void *context);

/* Nexus SW RAVE customer handler */
NEXUS_Error NEXUS_SwRaveExtension_Set_Custom_handler(unsigned rave_index, void *handler, void *handler_context);

NEXUS_Error NEXUS_SwRaveExtension_GetDataBuffer(
    unsigned rave_index,
    void **pBuffer, /* [out] attr{memory=cached} pointer to memory mapped rave CDB */
    size_t *pSize   /* [out] total number of writeable, contiguous bytes which buffer is pointing to */
    );

NEXUS_Error NEXUS_SwRaveExtension_WriteComplete(
    unsigned rave_index,
    size_t amountUsed /* amountUsed is the number of bytes which should be fed to still decoder */
    );

#ifdef __cplusplus
}
#endif

#endif
