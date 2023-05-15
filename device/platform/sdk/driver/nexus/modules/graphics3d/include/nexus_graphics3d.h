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
 * $brcm_Workfile: nexus_graphics3d.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/18/12 8:54a $
 *
 * Module Description:
 *
 * Low level interface to the v3d hardware.  This module is used via the OpenGLES
 * & OpenVG driver.  For example code please look in
 *   /rockford/applications/opengles_v3d
 *
 * API documenation can be found at http://www.khronos.org
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/35230/include/nexus_graphics3d.h $
 * 
 * 4   5/18/12 8:54a erickson
 * SW7425-3086: extend NEXUS_Graphics3dCommandId
 * 
 * 3   11/17/11 3:36p erickson
 * SWVC4-211: add eSQRSV0
 * 
 * 2   3/17/11 4:23p erickson
 * SWVC4-133: merge
 * 
 * gsweet_multi_proc/1   3/17/11 2:10p gsweet
 * SWVC4-133: Multiprocess V3D support
 * 
 * 1   2/8/11 10:52a erickson
 * SWVC4-118: merge
 *
 * hauxwell_35230_v3d/3   1/14/11 1:21p gsweet
 * SWVC4-114: Change NEXUS_Graphics3d_Status from returning a uint32_t to
 *  returning it in an argument.
 *
 * hauxwell_35230_v3d/2   12/8/10 10:39a hauxwell
 * Add INDENT registers, remove function to get the HW version
 *
 * hauxwell_35230_v3d/1   12/6/10 11:51a gsweet
 * SWVC4-101: Allow abstract platform support for V3D driver.
 *
 * 1   7/10/08 4:00p vsilyaev
 * PR 40869: Graphics 3D API
 *
 *
 **************************************************************************/
#ifndef NEXUS_GRAPHICS3D_H__
#define NEXUS_GRAPHICS3D_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum NEXUS_Graphics3dCommandId
{
    NEXUS_Graphics3dCommandId_eNO_COMMAND = 0,
    NEXUS_Graphics3dCommandId_eL2CACTL,
    NEXUS_Graphics3dCommandId_eSLCACTL,
    NEXUS_Graphics3dCommandId_eINTCTL,
    NEXUS_Graphics3dCommandId_eINTENA,
    NEXUS_Graphics3dCommandId_eINTDIS,
    NEXUS_Graphics3dCommandId_eCT0CS,
    NEXUS_Graphics3dCommandId_eCT1CS,
    NEXUS_Graphics3dCommandId_eCT0EA,
    NEXUS_Graphics3dCommandId_eCT1EA,
    NEXUS_Graphics3dCommandId_eCT0CA,
    NEXUS_Graphics3dCommandId_eCT1CA,
    NEXUS_Graphics3dCommandId_ePCS,
    NEXUS_Graphics3dCommandId_eBFC,
    NEXUS_Graphics3dCommandId_eRFC,
    NEXUS_Graphics3dCommandId_eBPOA,
    NEXUS_Graphics3dCommandId_eBPOS,
    NEXUS_Graphics3dCommandId_eCACHE_CTRL,
    NEXUS_Graphics3dCommandId_ePCTRC,
    NEXUS_Graphics3dCommandId_ePCTRE,
    NEXUS_Graphics3dCommandId_ePCTR0,
    NEXUS_Graphics3dCommandId_ePCTRS0,
    NEXUS_Graphics3dCommandId_ePCTR1,
    NEXUS_Graphics3dCommandId_ePCTRS1,
    NEXUS_Graphics3dCommandId_ePCTR2,
    NEXUS_Graphics3dCommandId_ePCTRS2,
    NEXUS_Graphics3dCommandId_ePCTR3,
    NEXUS_Graphics3dCommandId_ePCTRS3,
    NEXUS_Graphics3dCommandId_ePCTR4,
    NEXUS_Graphics3dCommandId_ePCTRS4,
    NEXUS_Graphics3dCommandId_ePCTR5,
    NEXUS_Graphics3dCommandId_ePCTRS5,
    NEXUS_Graphics3dCommandId_ePCTR6,
    NEXUS_Graphics3dCommandId_ePCTRS6,
    NEXUS_Graphics3dCommandId_ePCTR7,
    NEXUS_Graphics3dCommandId_ePCTRS7,
    NEXUS_Graphics3dCommandId_ePCTR8,
    NEXUS_Graphics3dCommandId_ePCTRS8,
    NEXUS_Graphics3dCommandId_ePCTR9,
    NEXUS_Graphics3dCommandId_ePCTRS9,
    NEXUS_Graphics3dCommandId_ePCTR10,
    NEXUS_Graphics3dCommandId_ePCTRS10,
    NEXUS_Graphics3dCommandId_ePCTR11,
    NEXUS_Graphics3dCommandId_ePCTRS11,
    NEXUS_Graphics3dCommandId_ePCTR12,
    NEXUS_Graphics3dCommandId_ePCTRS12,
    NEXUS_Graphics3dCommandId_ePCTR13,
    NEXUS_Graphics3dCommandId_ePCTRS13,
    NEXUS_Graphics3dCommandId_ePCTR14,
    NEXUS_Graphics3dCommandId_ePCTRS14,
    NEXUS_Graphics3dCommandId_ePCTR15,
    NEXUS_Graphics3dCommandId_ePCTRS15,
    NEXUS_Graphics3dCommandId_eGCA_PM_SEL,
    NEXUS_Graphics3dCommandId_eGCA_BW_CNT,
    NEXUS_Graphics3dCommandId_eGCA_BW_MEM_CNT,
    NEXUS_Graphics3dCommandId_eIDENT0,
    NEXUS_Graphics3dCommandId_eIDENT1,
    NEXUS_Graphics3dCommandId_eIDENT2,
    NEXUS_Graphics3dCommandId_eSQRSV0,
    NEXUS_Graphics3dCommandId_eIDENT3,
    NEXUS_Graphics3dCommandId_eSQRSV1,
    NEXUS_Graphics3dCommandId_eSQCNTL,
    NEXUS_Graphics3dCommandId_eSQCSTAT,
    NEXUS_Graphics3dCommandId_eSRQPC,
    NEXUS_Graphics3dCommandId_eSRQUA,
    NEXUS_Graphics3dCommandId_eSRQUL,
    NEXUS_Graphics3dCommandId_eSRQCS,
    NEXUS_Graphics3dCommandId_eSCRATCH,
    NEXUS_Graphics3dCommandId_eENDSWP,
    NEXUS_Graphics3dCommandId_eVPACNTL,
    NEXUS_Graphics3dCommandId_eVPMBASE,
    NEXUS_Graphics3dCommandId_eBPCA,
    NEXUS_Graphics3dCommandId_eBPCS,
    NEXUS_Graphics3dCommandId_eBXCF,
    NEXUS_Graphics3dCommandId_eCACHE_ID,
    NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL,
    NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL_2,
    NEXUS_Graphics3dCommandId_eCACHE_STATUS,
    NEXUS_Graphics3dCommandId_ePM_CTRL,
    NEXUS_Graphics3dCommandId_eV3D_BW_CNT,
    NEXUS_Graphics3dCommandId_eMEM_BW_CNT,
    NEXUS_Graphics3dCommandId_eLOW_PRI_ID,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_0,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_1,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_2,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_3,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_4,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_5,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_6,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_7,
    NEXUS_Graphics3dCommandId_eMEM_PROTECT_STATUS,
    NEXUS_Graphics3dCommandId_eAXI_BRIDGE_STATUS,
    NEXUS_Graphics3dCommandId_eSCRATCH_0,
    NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN,
    NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN_ACK,
    NEXUS_Graphics3dCommandId_eDBCFG,
    NEXUS_Graphics3dCommandId_eDBSCS,
    NEXUS_Graphics3dCommandId_eDBSCFG,
    NEXUS_Graphics3dCommandId_eDBSSR,
    NEXUS_Graphics3dCommandId_eDBSDR0,
    NEXUS_Graphics3dCommandId_eDBSDR1,
    NEXUS_Graphics3dCommandId_eDBSDR2,
    NEXUS_Graphics3dCommandId_eDBSDR3,
    NEXUS_Graphics3dCommandId_eDBQRUN,
    NEXUS_Graphics3dCommandId_eDBQHLT,
    NEXUS_Graphics3dCommandId_eDBQSTP,
    NEXUS_Graphics3dCommandId_eDBQITE,
    NEXUS_Graphics3dCommandId_eDBQITC,
    NEXUS_Graphics3dCommandId_eDBQGHC,
    NEXUS_Graphics3dCommandId_eDBQGHG,
    NEXUS_Graphics3dCommandId_eDBQGHH,
    NEXUS_Graphics3dCommandId_eDBGE,
    NEXUS_Graphics3dCommandId_eFDBGO,
    NEXUS_Graphics3dCommandId_eFDBGB,
    NEXUS_Graphics3dCommandId_eFDBGR,
    NEXUS_Graphics3dCommandId_eFDBGS,
    NEXUS_Graphics3dCommandId_eERRSTAT,
    NEXUS_Graphics3dCommandId_eCT00RA0,
    NEXUS_Graphics3dCommandId_eCT01RA0,
    NEXUS_Graphics3dCommandId_eCT0LC,
    NEXUS_Graphics3dCommandId_eCT1LC,
    NEXUS_Graphics3dCommandId_eCT0PC,
    NEXUS_Graphics3dCommandId_eCT1PC,
    NEXUS_Graphics3dCommandId_eBRIDGE_REVISION,
    NEXUS_Graphics3dCommandId_eBRIDGE_CTRL,
    NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_0,
    NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_1,
    NEXUS_Graphics3dCommandId_eINTDONE,
    NEXUS_Graphics3dCommandId_eMax       /* Must be last */
} NEXUS_Graphics3dCommandId;

/**
Summary:
Handle for the Graphics3d interface.
**/
typedef struct NEXUS_Graphics3d *NEXUS_Graphics3dHandle;

/**
Summary:
Settings used for NEXUS_Graphics3d_Open
**/
typedef struct NEXUS_Graphics3dOpenSettings
{
   NEXUS_CallbackDesc   tryAgainCallback;     /* Called when you should try again, if acquire failed due to contention */
   NEXUS_CallbackDesc   controlCallback;      /* Called when the 3D core needs servicing by the driver */
} NEXUS_Graphics3dOpenSettings;

/**
Summary:
Settings used for NEXUS_Graphics3d_Acquire
**/
typedef struct NEXUS_Graphics3dAcquireSettings
{
    uint32_t            userVertexPipeMemory; /* Contains the amount of VPM memory reserved
                                                 for all user programs, in multiples of 256 bytes */
} NEXUS_Graphics3dAcquireSettings;

/**
Summary:
Status value filled out by NEXUS_Graphics3d_GetStatus.
**/
typedef struct NEXUS_Graphics3dStatus
{
    uint32_t status;
} NEXUS_Graphics3dStatus;

/**
Summary:
Structure containing command id and data value to send to the core.
**/
typedef struct NEXUS_Graphics3dCommand
{
    NEXUS_Graphics3dCommandId commandId;
    uint32_t                  data;
} NEXUS_Graphics3dCommand;


/**
Summary:
Get default settings for NEXUS_Graphics3d_Open
**/
void NEXUS_Graphics3d_GetDefaultOpenSettings(
    NEXUS_Graphics3dOpenSettings *pSettings /* [out] */
    );

/**
Summary:
Get default settings for NEXUS_Graphics3d_Acquire
**/
void NEXUS_Graphics3d_GetDefaultAcquireSettings(
   NEXUS_Graphics3dAcquireSettings *pSettings /* [out] */
   );

/**
Summary:
Get default command structure for NEXUS_Graphics3d_SendCommand
**/
void NEXUS_Graphics3d_GetDefaultCommand(
    NEXUS_Graphics3dCommand *pCommand /* [out] */
    );

/**
Summary:
Open a Graphics3d interface.

Description:
Open a Nexus 3d graphics interface. Only the 3D graphics driver code should actually call this.
This is not useful to users, who should always use EGL/OpenGL|ES to drive the 3D core.
A Graphics3d interface should only be opened once per client process.
**/
NEXUS_Graphics3dHandle NEXUS_Graphics3d_Open(     /* attr{destructor=NEXUS_Graphics3d_Close}  */
    const NEXUS_Graphics3dOpenSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close a Graphics3d interface.
**/
void NEXUS_Graphics3d_Close(
    NEXUS_Graphics3dHandle handle
    );

/**
Summary:
Acquire the Graphics3d core for exclusive use. This call can fail with NEXUS_NOT_AVAILABLE if the
core is currently acquired by another client. In this case you should try to re-acquire your
tryAgain callback (which you supplied during Open) is triggered (note : you are not guaranteed 
to acquire after the callback, you may receive another NEXUS_NOT_AVAILABLE).

Description:
Only the 3D graphics driver platform code should actually call this.
This is not useful to users, who should always use EGL/OpenGL|ES to drive the 3D core.

Acquires exclusive control of the 3D hardware until released with NEXUS_Graphics3d_Release.
When only a single client process is in use, nested (reference counted) acquires may succeed. 
You should ensure that there are an equivalent number of releases for correct behavior.
**/
NEXUS_Error NEXUS_Graphics3d_Acquire(
   NEXUS_Graphics3dHandle handle,
   const NEXUS_Graphics3dAcquireSettings *pSettings
   );

/**
Summary:
Release a previously acquired Graphics3d. Can return NEXUS_NOT_AVAILABLE if the core was not
actually released. This can happen if a nested acquire (for the same client) had 
previously occurred. No action is necessary when this occurs, the core will still be acquired.
Only when a matching number of acquires and releases have occurred will the core actually be
released.
**/
NEXUS_Error NEXUS_Graphics3d_Release(
   NEXUS_Graphics3dHandle handle
   );

/**
Summary:
Read back and return a particular status value from the 3D core
**/
NEXUS_Error NEXUS_Graphics3d_GetStatus(
    NEXUS_Graphics3dHandle    handle,
    NEXUS_Graphics3dCommandId commandId,
    NEXUS_Graphics3dStatus    *pStatus /* [out] */
    );

/**
Summary:
Send a particular command with data to the 3D core
**/
NEXUS_Error NEXUS_Graphics3d_SendCommand(
    NEXUS_Graphics3dHandle        handle,
    const NEXUS_Graphics3dCommand *pCommand
    );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
