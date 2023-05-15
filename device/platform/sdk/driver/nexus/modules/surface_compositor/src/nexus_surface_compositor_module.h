/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_surface_compositor_module.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 10/17/12 5:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/src/nexus_surface_compositor_module.h $
 * 
 * 12   10/17/12 5:11p erickson
 * SW7435-429: add proc
 * 
 * 11   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 10   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 9   10/20/11 1:56p erickson
 * SW7420-1992: embed NEXUS_VideoWindowSettings in
 *  NEXUS_SurfaceCompositorDisplaySettings
 * 
 * 8   10/18/11 8:55a erickson
 * SW7420-1992: add #include nexus_base.h
 * 
 * 7   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 6   8/11/11 3:31p erickson
 * SW7420-1992: update
 * 
 * 5   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 * 
 * 4   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 * 
 * 3   8/4/11 10:41a erickson
 * SW7420-1992: implement direct mode
 * 
 * 2   8/3/11 12:37p erickson
 * SW7420-1992: determine if a client is fullscreen, then short circuit
 *  all blits below that
 * 
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 * 
 **************************************************************************/
#ifndef NEXUS_SURFACECMP_MODULE_H__
#define NEXUS_SURFACECMP_MODULE_H__

#include "nexus_base.h"
#include "nexus_surface_compositor_thunks.h"
#include "nexus_surface_compositor_init.h"
#include "nexus_surface_compositor.h"
#include "nexus_surface_client.h"
#include "nexus_surface_cursor.h"

#ifdef __cplusplus
#error
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME surface_compositor
#define NEXUS_MODULE_SELF g_NEXUS_surface_compositorModule
extern NEXUS_ModuleHandle g_NEXUS_surface_compositorModule;
extern NEXUS_SurfaceCompositorModuleSettings g_NEXUS_SurfaceCompositorModuleSettings;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SurfaceCompositor);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SurfaceClient);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SurfaceCursor);

void NEXUS_SurfaceCompositorModule_P_Print(void);

#endif
