/******************************************************************************
 *    (c)2009-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_security_misc.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/12/11 12:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/src/nexus_security_misc.c $
 * 
 * 5   7/12/11 12:55a atruong
 * SW7231-272: Code Cleanup
 * 
 * 4   1/21/11 6:10p katrep
 * SW7231-7:add support for 7231
 * 
 * 3   12/22/10 5:48p mphillip
 * SW7422-104: Merge 40nm support to /main
 * 
 * SW7422-104/2   12/22/10 5:36p mphillip
 * SW7422-104: Add error trace for unsupported version
 * 
 * SW7422-104/1   12/16/10 6:14p mphillip
 * SW7422-104: Initial support for 7422
 * 
 * 2   4/8/09 12:37p erickson
 * PR53745: fix warning
 *
 * 1   4/2/09 12:03p yili
 * PR53745:Add data section support
 *
 *****************************************************************************/

#include "nexus_security_module.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"

#include "bhsm.h"
#include "bsp_s_commands.h"
#include "bsp_s_misc.h"
#include "bsp_s_hw.h"
#include "bsp_s_keycommon.h"
#include "bhsm_keyladder.h"

#if (BCHP_CHIP!=7422) && (BCHP_CHIP!=7231) && (BCHP_CHIP!=7425) && (BCHP_CHIP!=7344) && (BCHP_CHIP!=7346) && (BCHP_CHIP!=7230) && (BCHP_CHIP!=7358) && (BCHP_CHIP!=7552)
#include "bhsm_misc.h"
#include "bhsm_user_cmds.h"
#endif

BDBG_MODULE(nexus_security);


NEXUS_Error NEXUS_Security_RouteDataSectionKey(NEXUS_KeySlotHandle keyHandle)
{
#ifdef NEXUS_SECURITY_DS2_ROUTING
    BERR_Code   errCode = BERR_SUCCESS;
    BHSM_RouteDataSectionKeyIO_t  inout_RouteDataSectionKeyIO;
    BHSM_Handle hHsm;

    NEXUS_Security_GetHsm_priv (&hHsm);
    if ( !hHsm )
        return NEXUS_NOT_INITIALIZED;

    inout_RouteDataSectionKeyIO.unKeySlotNumber = keyHandle->keySlotNumber;
    errCode = BHSM_RouteDataSectionKey (hHsm, &inout_RouteDataSectionKeyIO);


    if ( errCode!= BERR_SUCCESS )
    {
        return NEXUS_NOT_SUPPORTED;
    }

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(keyHandle);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif

}
