/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
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
 *  
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
 * $brcm_Workfile: bcmnexus_pool.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 10/5/11 12:25p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_pool.h $
 * 
 * DirectFB_1_4_15_Port/2   10/5/11 12:25p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/8   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 * 
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 * 
 * DirectFB_1_4_Port/7   3/17/11 12:39p robertwm
 * SW7422-378: DirectFB: Destroying a Palettised surface in a client
 *  application causes NEXUS assertion during free.
 * 
 * DirectFB_1_4_Port/6   3/1/11 8:48p robertwm
 * SW7420-1559: DirectFB: NEXUS surface assertion in multi-app mode when
 *  closing down a DFB client.
 * 
 * DirectFB_1_4_Port/5   2/23/11 6:04p christ
 * SW7425-108: DirectFB: Insignia LUT8 tests failing on 7425 packet blit
 *  build.
 * 
 * DirectFB_1_4_Port/4   2/17/11 11:13p robertwm
 * SW7420-1510: DirectFB: Reduce number of IPC calls for gfx operations
 *  using packet-blit interface.
 * 
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 * 
 * DirectFB_1_4_Port/3   8/16/10 11:40p robertwm
 * SW7405-4761: DirectFB: Optimise the surface pool locking code.
 * 
 * DirectFB_1_4_Port/2   7/22/10 4:18p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 * 
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 * 
 *****************************************************************************/
#ifndef BCMNEXUS_POOL_H_
#define BCMNEXUS_POOL_H_

#include <core/surface_pool.h>

typedef struct bcmAllocationData {
     CoreSurfaceAllocation         *allocation;         /* Pointer to allocation */
     void                          *address;            /* Virtual (cached) address of this allocation */
     uint32_t                       hwaddress;
     NEXUS_HeapHandle               heap;
     int                            offsetInHeap;
     int                            size;
     int                            pitch;
     NEXUS_PixelFormat              pixelformat;
     bool                           syncIssued;         /* set to true to indicate that a Checkpoint/Sync was issued */
     NEXUS_SurfaceHandle            bcmSurfaceHandle;   /* If not NULL, then holds Nexus surface handle*/
     NEXUS_Pixel                   *palette;            /* If not NULL, then points to palette */
     uint32_t                       hwpalette;
     void                          *hm_ctx;
     DFB_Platform_P_PoolHandle      gfx3DPoolHandle;    /* Handle for the 3D graphics pool */
} bcmAllocationData;

typedef struct {
    size_t                          numBytes;
    NEXUS_MemoryAllocationSettings  settings;
    void                           *pMemory;
} bcmPoolCallMemoryAllocate;

typedef struct {
    NEXUS_SurfaceHandle             surface;
    NEXUS_SurfaceCreateSettings     createSettings;
} bcmPoolCallSurfaceCreate;

extern const SurfacePoolFuncs bcmnexusPoolFunctions;
extern const SurfacePoolFuncs bcmnexusGraphics3DPoolFunctions;

#endif

