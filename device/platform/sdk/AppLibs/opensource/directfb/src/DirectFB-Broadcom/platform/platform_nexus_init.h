/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_nexus_init.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 10/5/11 12:14p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_nexus_init.h $
 * 
 * DirectFB_1_4_15_Port/2   10/5/11 12:14p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 * 
 * DirectFB_1_4_14_Port/1   9/22/11 4:43p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 * 
 *****************************************************************************/
#ifndef __PLATFORM_NEXUS_INIT_H__
#define __PLATFORM_NEXUS_INIT_H__

#include "platform_init.h"

// For backwards compatibility
#ifndef NEXUS_OFFSCREEN_SURFACE
#define NEXUS_OFFSCREEN_SURFACE ((unsigned)-1)
#endif

void
DFB_Platform_P_ConfigNexusHeaps( DFB_PlatformSettings *pSettings,
                                 unsigned long         gfxHeapSize );

DFB_PlatformResult
DFB_Platform_P_Nexus_GetDisplaySize( int                    displayIndex,
                                     DFB_PlatformDimension *pSize,
                                     DFB_P_DisplaySettings *dfb_p_displaysettings );

DFB_PlatformResult
DFB_Platform_P_Nexus_Display_Init( unsigned                         displayIndex,
                                   DFB_PlatformDisplayInitSettings *pSettings,
                                   DFB_PlatformNexusHandle         *pHandle,
                                   DFB_P_DisplaySettings           *dfb_p_displaysettings);

void
DFB_Platform_P_Nexus_Display_Uninit( DFB_PlatformNexusHandle handle,
                                     DFB_P_DisplaySettings  *dfb_p_displaysettings );

DFB_PlatformResult
DFB_Platform_P_Nexus_GetDisplaySettings( DFB_PlatformNexusHandle         displayHandle,
                                         DFB_Platform_P_DisplaySettings *pSettings,
                                         DFB_P_DisplaySettings          *dfb_p_displaysettings);

void
DFB_Platform_P_Nexus_DisplayWindow_Uninit( DFB_PlatformNexusHandle handle,
                                           DFB_P_DisplaySettings  *dfb_p_displaysettings );

#endif /* __PLATFORM_NEXUS_INIT_H__ */
