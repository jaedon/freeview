/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_version.c $
* $brcm_Revision: 16 $
* $brcm_Date: 6/1/10 6:07p $
*
* API Description:
*   API name: Platform Software Release Version
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/nexus_platform_version.c $
* 
* 16   6/1/10 6:07p vsilyaev
* SW7405-4406: Don't reference NULL point
* 
* 15   3/19/10 10:12a erickson
* SW7405-3832: create standard method for specific
*  NEXUS_PLATFORM_VERSION. use tagged NEXUS_Platform_Init to enforce
*  binary compatibility. consolidate NEXUS_Platform_GetReleaseVersion
*  using standard macros.
*
* 14   2/23/10 2:02p vishk
* SW7335-681: Debug module used before being initialized in
*  NEXUS_Platform_Init().
*
* 13   2/23/10 11:39a vishk
* SW7335-681: Debug module used before being initialized in
*  NEXUS_Platform_Init().
*
* 12   2/22/10 4:17p vishk
* SW7335-681: Debug module used before being initialized in
*  NEXUS_Platform_Init().
*
* 11   2/12/10 3:12p randyjew
* SW7468-6:Update 7468/7208 versions
*
* 10   2/8/10 11:18a jrubio
* SW7342-11: Update Versions 734X
*
* 9   12/3/09 4:05p randyjew
* SW7468-6: Add 7468 support
*
* 8   8/31/09 8:42p agin
* SWNOOS-365: Add 97459.
*
* 7   8/28/09 9:37a gmohile
* SW7403-796 : Add 7403 platform version
*
* 6   8/25/09 12:23p erickson
* SW7420-330: temporary fix for 97420dvr2 kernel mode
*
* 5   8/21/09 12:26p rjlewis
* PR56569: needs bstd to get berr to define BERR_Code.
*
* 4   8/19/09 11:54a mward
* PR55545: Add 97125 family support.
*
* 3   8/6/09 2:45p jrubio
* PR55232: add 7340/7342 Support
*
* 2   8/5/09 4:39p katrep
* PR56569: add nexus platfrom get sw version api
*
* 1   8/5/09 12:04p katrep
* PR56569: platfrom sofftware release version
*
*
***************************************************************************/
#include "nexus_platform.h"
#include "nexus_platform_priv.h"

/* These macros are similar to the NEXUS_P_GET_VERSION macros in nexus_platform.h,
but these are only used internally. */
#define NEXUS_P_GET_PLATFORM_P(p) # p
#define NEXUS_P_GET_PLATFORM(p) NEXUS_P_GET_PLATFORM_P(p)
#define NEXUS_P_GET_CUSTOM_P(p) NEXUS_PLATFORM_ ## p  ## _CUSTOM
#define NEXUS_P_GET_CUSTOM(p)  NEXUS_P_GET_CUSTOM_P(p)

void NEXUS_Platform_GetReleaseVersion( char *pVersionString, unsigned size)
{
    NEXUS_Error errCode;

    errCode = NEXUS_Platform_P_Magnum_Init();
    if(errCode!=BERR_SUCCESS) {
        if(pVersionString && size > 0) {
            *pVersionString = 0; /* Null terminate the string so the app doesn't crash. */
        }
        return;
    }

    BKNI_Snprintf(pVersionString, size, "%s %d.%d %s",
        NEXUS_P_GET_PLATFORM(NEXUS_PLATFORM),
        NEXUS_P_GET_VERSION(NEXUS_PLATFORM) / NEXUS_PLATFORM_VERSION_UNITS,
        NEXUS_P_GET_VERSION(NEXUS_PLATFORM) % NEXUS_PLATFORM_VERSION_UNITS,
        NEXUS_P_GET_CUSTOM(NEXUS_PLATFORM));

    return;
}

