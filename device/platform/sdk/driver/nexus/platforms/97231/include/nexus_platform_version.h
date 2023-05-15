/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_platform_version.h $
 * $brcm_Revision: SW7435-433/1 $
 * $brcm_Date: 10/19/12 5:04p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/platforms/97231/include/nexus_platform_version.h $
 * 
 * SW7435-433/1   10/19/12 5:04p mward
 * SW7435-433: Set version 12.4
 * 
 * 10   8/2/12 4:07p randyjew
 * SW7231-317: update version number
 * 
 * 9   4/3/12 6:56p katrep
 * SW7231-317:updated versionn number
 * 
 * 8   2/9/12 2:59p katrep
 * SW7231-317:phase 40
 * 
 * 7   10/10/11 4:57p katrep
 * SW7231-317:updated version number
 * 
 * 6   5/2/11 12:54p katrep
 * SW7231-4:updated version number for phase20
 * 
 * 5   3/7/11 4:01p katrep
 * SW7231-47:Updated version # for Phase 10
 * 
 * 4   2/7/11 11:24a katrep
 * SW7231-4:updated version numbber for Phase 0.5
 * 
 * 3   12/22/10 4:07p katrep
 * SW7231-26:add support for new heap configuration, add support for 7230
 * 
 * 2   12/2/10 10:10a katrep
 * SW7231-4:intial version
*
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_VERSION_H__
#define NEXUS_PLATFORM_VERSION_H__

#ifndef NEXUS_PLATFORM_H__
#error Do not inclue nexus_platform_version.h directly. Include nexus_platform.h instead.
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NEXUS_PLATFORM
#error NEXUS_PLATFORM is defined by the Nexus build system. If not defined, something must be wrong in the Makefiles.
#endif

/**
This section defines the version information for this Nexus software release.
This file is modified before each release to note the correct verion and any custom version information.
**/
#define NEXUS_PLATFORM_97231        NEXUS_PLATFORM_VERSION(12,4)
#define NEXUS_PLATFORM_97231_CUSTOM ""

#define NEXUS_PLATFORM_97230        NEXUS_PLATFORM_VERSION(12,4)
#define NEXUS_PLATFORM_97230_CUSTOM ""


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_VERSION_H__ */

