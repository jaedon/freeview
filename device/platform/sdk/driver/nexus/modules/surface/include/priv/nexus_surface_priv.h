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
 * $brcm_Workfile: nexus_surface_priv.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface/7400/include/priv/nexus_surface_priv.h $
 * 
 * 8   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 7   1/5/12 12:28p erickson
 * SW7435-30: add NEXUS_Surface_InitPlaneAndPaletteOffset
 * 
 * 6   10/7/11 1:49p vsilyaev
 * SW7420-2085: Added support for object reference counting
 * 
 * 5   5/23/11 1:34p erickson
 * SW7420-1200: add NEXUS_Surface_InitPlane_priv
 * 
 * 4   8/12/09 11:21a vsilyaev
 * PR 57629: Replaced NEXUS_Surface_AutoFlush_priv with
 *  NEXUS_Surface_GetSurfaceAutoFlush_priv
 * 
 * 3   5/5/08 2:54p erickson
 * PR40777: handle interlaced stills using M2MC, not software
 *
 * 2   3/14/08 10:10a erickson
 * PR40307: 40316
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   1/8/08 1:19p erickson
 * PR36159: added autoFlush and blockedSync
 *
 * Nexus_Devel/2   11/29/07 12:26p vsilyaev
 * PR 36159: Added mapping from magnum to nexus surface
 *
 * Nexus_Devel/1   9/26/07 1:37p vsilyaev
 * PR 34662:  API to return magnum surface handle
 *
 **************************************************************************/
#ifndef NEXUS_SURFACE_PRIV_H__
#define NEXUS_SURFACE_PRIV_H__
#include "bsur.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
Returns magnum surface handle for NEXUS Surface
*/
BSUR_Surface_Handle NEXUS_Surface_GetSurface_priv(NEXUS_SurfaceHandle surface);

/*
Summary:
Creates new NEXUS Surface from the magnum surface handle
*/
NEXUS_SurfaceHandle NEXUS_Surface_CreateFromMagnum_priv(BSUR_Surface_Handle surface);

/*
Summary:
Frees internal resources allocated by NEXUS_Surface_CreateFromMagnum_priv
*/
void NEXUS_Surface_ReleaseSurface_priv(NEXUS_SurfaceHandle surface);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Surface);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_SURFACE_PRIV_H__ */
