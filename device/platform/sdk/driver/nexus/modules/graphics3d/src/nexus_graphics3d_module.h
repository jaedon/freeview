/***************************************************************************
 *     (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_graphics3d_module.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/19/12 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/35230/src/nexus_graphics3d_module.h $
 * 
 * 3   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 2   8/24/11 9:28a erickson
 * SW7346-446: add brcm_Log tag to header file for clearcase checkin
 *  comments
 * 
 **************************************************************************/
#ifndef NEXUS_GRAPHICS3D_MODULE_H__
#define NEXUS_GRAPHICS3D_MODULE_H__

#include "nexus_base.h"
#include "nexus_graphics3d_thunks.h"
#include "nexus_graphics3d.h"
#include "nexus_graphics3d_init.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME graphics3d
#define NEXUS_MODULE_SELF NEXUS_P_Graphics3dModule

/* global handle. there is no global data. */
extern NEXUS_ModuleHandle NEXUS_P_Graphics3dModule;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Graphics3d);

#ifdef __cplusplus
}
#endif

#endif
