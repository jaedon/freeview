/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_display.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/1 $
 * $brcm_Date: 11/22/11 4:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_display.h $
 * 
 * DirectFB_1_4_15_Port/1   11/22/11 4:32p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 * 
 *****************************************************************************/
#ifndef __BCMNEXUS__PLATFORM_DISPLAY_H__
#define __BCMNEXUS__PLATFORM_DISPLAY_H__

#include "platform_init.h"
    
#ifdef __cplusplus
extern "C" {
#endif

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle, int outputIndex );

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif
