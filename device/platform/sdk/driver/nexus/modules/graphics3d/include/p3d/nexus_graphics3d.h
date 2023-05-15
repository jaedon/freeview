/***************************************************************************
 *     (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: nexus_graphics3d.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/8/09 10:36a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/7400/include/nexus_graphics3d.h $
 * 
 * 3   7/8/09 10:36a erickson
 * PR55968: add NEXUS_Graphics3DHandle and interface functions
 * 
 * 2   3/26/09 8:45a erickson
 * PR35457: add comment re: OpenGL-ES
 *
 * 1   7/10/08 4:00p vsilyaev
 * PR 40869: Graphics 3D API
 *
 *
 **************************************************************************/
#ifndef NEXUS_GRAPHICS3D_H__
#define NEXUS_GRAPHICS3D_H__

/*=***********************************
*************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*
Broadcom's 3D Graphics API is OpenGL-ES. This has been integrated with Nexus and is available upon request.

The Nexus Graphics3D module is used internally by the OpenGL-ES implementation.
It serves to hook the OpenGL-ES code into the Magnum system including the P3D porting interface and various Magnum base modules.
Nexus does not have its own a 3D API.
*/

/**
Summary:
Handle for the Graphics3D interface.
**/
typedef struct NEXUS_Graphics3D *NEXUS_Graphics3DHandle;

/**
Summary:
Settings used for NEXUS_Graphics3D_Open
**/
typedef struct NEXUS_Graphics3DOpenSettings
{
    int dummy; /* There are no members at this time */
} NEXUS_Graphics3DOpenSettings;

/**
Summary:
Get default settings for NEXUS_Graphics3D_Open
**/
void NEXUS_Graphics3D_GetDefaultOpenSettings(
   NEXUS_Graphics3DOpenSettings *pSettings /* [out] */
   );

/**
Summary:
Open a Graphics3D interface.
**/
NEXUS_Graphics3DHandle NEXUS_Graphics3D_Open(   /* attr{destructor=NEXUS_Graphics3D_Close}  */
    unsigned index, 
    const NEXUS_Graphics3DOpenSettings *pSettings
    );

/**
Summary:
Close a Graphics3D interface.
**/
void NEXUS_Graphics3D_Close(
    NEXUS_Graphics3DHandle gfx
    );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
