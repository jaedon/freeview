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
 * $brcm_Workfile: nexus_dma_init.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 10/16/12 1:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/dma/include/nexus_dma_init.h $
 * 
 * 8   10/16/12 1:33p erickson
 * SW7435-375: dma does not depend on security
 * 
 * 7   11/10/11 5:59p jtna
 * SW7425-1709: move NEXUS_DmaModule_Standby_priv prototype to separate,
 *  private header file
 * 
 * 6   11/8/11 6:00p jtna
 * SW7425-1709: make use of BMMD_Standby/Resume
 * 
 * 5   3/16/11 2:58p vsilyaev
 * SW7346-18: Moved totalBlocks and maxBlocksPerTransfer into DMA_Open
 *  settings
 * 
 * 4   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 * 
 * 3   4/19/10 12:11p erickson
 * SW7405-4221: add NEXUS_DmaModuleSettings.totalBlocks and
 *  .maxBlocksPerTransfer. use BDMA_Mem_Create2 to pass in those options.
 *
 * 2   7/3/08 4:46p erickson
 * PR44586: NEXUS_DmaModule_Init must require the security module handle
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   12/7/07 11:54a jgarrett
 * PR 37837: Removing security dependency
 *
 * Nexus_Devel/1   12/3/07 9:29p jgarrett
 * PR 37837: Adding DMA module
 *
 **************************************************************************/
#ifndef NEXUS_DMA_INIT_H__
#define NEXUS_DMA_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_DmaModuleSettings
{
    unsigned unused;
} NEXUS_DmaModuleSettings;


void NEXUS_DmaModule_GetDefaultSettings(
    NEXUS_DmaModuleSettings *pSettings    /* [out] */
    );

NEXUS_ModuleHandle NEXUS_DmaModule_Init(
    const NEXUS_DmaModuleSettings *pSettings  /* NULL for default settings is allowed only if there is not security module support */
    );

void NEXUS_DmaModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif

