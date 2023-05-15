/***************************************************************************
 *    (c)2005-2012 Broadcom Corporation
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
 * $brcm_Workfile: bcmnexus_utils.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 10/5/11 12:28p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/utils/bcmnexus/bcmnexus_utils.h $
 * 
 * DirectFB_1_4_15_Port/2   10/5/11 12:28p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:24a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 5:07p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/5   12/3/10 8:40p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 * 
 * DirectFB_1_4_Port/4   8/11/10 9:24p robertwm
 * SW7550-518: DirectFB: bcmnexus_utils.h tries to include
 *  "nexus_graphics2d.h" for 7550.
 * 
 * DirectFB_1_4_Port/3   5/27/10 9:13p robertwm
 * SW7420-784: DirectFB: SetPalette on a non-layer surface doesn't work.
 * 
 * DirectFB_1_4_Port/2   3/2/10 12:42p robertwm
 * SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
 *  system drivers not working.
 * 
 * DirectFB_1_4_Port/2   1/14/10 7:03p robertwm
 * SW3556-996: DirectFB: Implement ImageProvider for DirectFB-1.4.1 using
 *  SID.
 * 
 * DirectFB_1_4_Port/1   12/23/09 4:18a robertwm
 * SW7550-51: Support DirectFB on 7550.
 * 
 *
 ***************************************************************************/

#ifndef __CORE__BCMNEXUS__UTILS_H__
#define __CORE__BCMNEXUS__UTILS_H__

#include <directfb.h>
#include <platform_init.h>
#include <core/surface.h>
#include <nexus_types.h>
#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS  
#include <nexus_graphics2d.h>
#endif
#include <nexus_surface.h>
#include "bcmnexus_gfx_defines.h"

extern NEXUS_PixelFormat bcmnexus_getNexusPixelFormat(DFBSurfacePixelFormat format);
extern DFBSurfacePixelFormat bcmnexus_getDFBSurfacePixelFormat(NEXUS_PixelFormat format);
extern DFB_PlatformPixelFormat bcmnexus_getDFBPlatformPixelFormat(DFBSurfacePixelFormat format);
extern void copy_palette( CoreSurface *surface, NEXUS_SurfaceHandle hSurface );

#endif
