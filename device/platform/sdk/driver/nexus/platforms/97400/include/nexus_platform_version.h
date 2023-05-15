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
* $brcm_Date: 10/19/12 5:05p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/include/nexus_platform_version.h $
* 
* SW7435-433/1   10/19/12 5:05p mward
* SW7435-433: Set version 12.4
* 
* 7   9/13/11 6:11p jhaberf
* SWDTV-6551: bumped version number for 35233
* 
* 6   8/16/11 2:47p jhaberf
* SWDTV-6551: Updated version number for 35233 release
* 
* 5   7/28/11 1:24p jhaberf
* SWDTV-6551: bump version number for release
* 
* 4   7/19/11 2:07p jhaberf
* SWDTV-6551: bumped version number for 35233 a0 phase 2.0 release
* 
* 3   6/23/11 10:20a jhaberf
* SWDTV-6551: updated release version for the 35233
* 
* 2   5/17/11 1:06p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:34p mstefan
* SWDTV-6928: merge to main
* 
* 1   3/19/10 12:00p mward
* SW7405-3832:  Add nexus_platform_version.h for 97400.
* 
* 1   3/19/10 10:09a erickson
* SW7405-3832: add nexus_platform_version.h
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
#if (BCHP_CHIP==35230)
#define NEXUS_PLATFORM_935230        NEXUS_PLATFORM_VERSION(7,1)
#define NEXUS_PLATFORM_935230_CUSTOM ""
#elif (BCHP_CHIP==35125)
#define NEXUS_PLATFORM_935125        NEXUS_PLATFORM_VERSION(3,1)
#define NEXUS_PLATFORM_935125_CUSTOM ""
#elif (BCHP_CHIP==35233)
#define NEXUS_PLATFORM_935233        NEXUS_PLATFORM_VERSION(3,4)
#define NEXUS_PLATFORM_935233_CUSTOM ""
#else
#define NEXUS_PLATFORM_97400        NEXUS_PLATFORM_VERSION(12,4)
#define NEXUS_PLATFORM_97400_CUSTOM ""
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_VERSION_H__ */

