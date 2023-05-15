/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: security.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/16/12 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/security/security.c $
 * 
 * Hydra_Software_Devel/2   11/16/12 6:12p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/1   3/18/09 11:34a erickson
 * PR53355: add test app
 *
 *****************************************************************************/
#include <stdio.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
int main(void)
{
#if NEXUS_HAS_SECURITY
    unsigned loops = 25;
    NEXUS_Error rc;

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    while (--loops) {
        NEXUS_KeySlotHandle keySlot;
        NEXUS_SecurityKeySlotSettings keySlotSettings;
        NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);

        keySlot = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

        NEXUS_Security_FreeKeySlot(keySlot);
    }

    NEXUS_Platform_Uninit();
#endif /* NEXUS_HAS_SECURITY */
    return 0;
}

