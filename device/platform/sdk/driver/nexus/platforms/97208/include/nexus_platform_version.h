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
* $brcm_Log: /nexus/platforms/97208/include/nexus_platform_version.h $
* 
* SW7435-433/1   10/19/12 5:05p mward
* SW7435-433: Set version 12.4
* 
* 5   8/8/12 5:10p randyjew
* SW7208-112: update Version to 4.5
* 
* 4   2/14/11 4:47p randyjew
* SW7208-112: Update Version to 4.0
* 
* 3   7/22/10 4:52p randyjew
* SW7468-247:Add 7208C board support. 7208 with 3112 frontend support.
*  export NEXUS_PLATFORM_VER=C for use.
* 
* 1   4/26/10 4:52p randyjew
* SW7405-3832: Add 7208 support
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
#define NEXUS_PLATFORM_97208        NEXUS_PLATFORM_VERSION(12,4)
#define NEXUS_PLATFORM_97208_CUSTOM ""

/* For 7208 board with 3112 frontend */
#if  NEXUS_PLATFORM_VER  == C
#define NEXUS_PLATFORM_97208_3112    1
#endif 


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_VERSION_H__ */

