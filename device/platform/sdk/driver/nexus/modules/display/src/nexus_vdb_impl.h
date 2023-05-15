/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_vdb_impl.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/2/10 10:17a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_vdb_impl.h $
 * 
 * 6   6/2/10 10:17a erickson
 * SW3548-2962: refactor NEXUS_DisplayModule_SetConfigurationId for settop
 *
 * 5   1/27/10 5:17p petlee
 * SW35230-66: Update stub definition of NEXUS_Vdb_P_SourceModeChanged()
 *  to use ((void)x) insetad of (i)=(i)
 *
 * 4   3/5/09 3:32p erickson
 * PR52266: fix warning
 *
 * 3   2/20/09 4:49p jgarrett
 * PR 52266: Adding memory requirements interface
 *
 * 2   2/10/09 11:47a jgarrett
 * PR 48984: Incorporating comments from vdb review
 *
 * 1   1/6/09 11:33a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding VDB stubs
 *
 **************************************************************************/
#ifndef NEXUS_VDB_IMPL_H__
#define NEXUS_VDB_IMPL_H__

#if (BCHP_CHIP == 3548 || BCHP_CHIP == 3556) && BCHP_VER >= BCHP_VER_B0

#define NEXUS_HAS_DYNAMIC_RTS 1

NEXUS_Error NEXUS_Vdb_P_Init(void);
void NEXUS_Vdb_P_Uninit(void);

bool NEXUS_Vdb_P_PixelFormatValid(NEXUS_PixelFormat requestedFormat, NEXUS_PixelFormat maxFormat);

unsigned int NEXUS_Vdb_P_GetBpp(NEXUS_PixelFormat format);

NEXUS_Error NEXUS_Vdb_P_HandleSourcePending(NEXUS_VideoInput input);
NEXUS_Error NEXUS_Vdb_P_CheckLayout(void);
NEXUS_Error NEXUS_Vdb_P_SourceModeChanged(NEXUS_VideoInput input);
NEXUS_Error NEXUS_Vdb_P_GetMemorySettings(
    unsigned configurationId,                           /* Configuration ID */
    uint32_t mask,                                      /* Must contain at least one window and at least one input */
    NEXUS_DisplayBufferTypeSettings *pFullHdBuffers,    /* [out] Full HD buffer requirements */
    NEXUS_DisplayBufferTypeSettings *pHdBuffers,        /* [out] HD buffer requirements */
    NEXUS_DisplayBufferTypeSettings *pSdBuffers,        /* [out] SD buffer requirements */
    unsigned *pHeapSize                                 /* [out] Heap size in bytes */
    );
NEXUS_Error NEXUS_DisplayModule_P_SetConfigurationId( unsigned id, bool recalcNow );

#else

/* Stubs for platforms without dynamic RTS */

#define NEXUS_Vdb_P_Init() (NEXUS_SUCCESS)
#define NEXUS_Vdb_P_Uninit() (void)0
#define NEXUS_Vdb_P_PixelFormatValid(r,m) (true)
#define NEXUS_Vdb_P_GetBpp(f) (0)
#define NEXUS_Vdb_P_HandleSourcePending(i) NEXUS_VideoInput_SetResumeMode((i), NEXUS_VideoInputResumeMode_eNow)
#define NEXUS_Vdb_P_CheckLayout() (NEXUS_SUCCESS)
#define NEXUS_Vdb_P_SourceModeChanged(i) ((void)i)

#endif

#endif /* !defined NEXUS_VDB_IMPL_H__ */


