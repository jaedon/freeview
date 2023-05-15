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
 * $brcm_Workfile: nexus_security_priv.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/include/priv/nexus_security_priv.h $
 * 
 * 12   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 11   11/7/11 2:20p mphillip
 * SWSECURITY-53: Nexus Security code maintenance - Undoing NSK2AVK
 *  extension module changes
 * 
 * 10   9/20/11 4:16p erickson
 * SW7346-495: add NSK2AVK
 * 
 * 9   12/16/10 1:57p erickson
 * SWDEPRECATED-2425: add comment
 * 
 * 8   8/4/10 3:04p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 * 
 * SW7405-4325/1   8/4/10 2:31p mphillip
 * SW7405-4325: Move keyslot memory management into core
 * 
 * 7   6/30/10 3:25p mphillip
 * SW7550-415: Make security handle details internal to the module.
 * 
 * 6   5/12/10 2:25p erickson
 * SW7550-419: add TODO regarding pidChannel. it should be removed. it is
 *  not safe to use for deallocating keyslots.
 *
 * 5   5/10/10 10:45a erickson
 * SW7550-415: remove extraneous and incorrect comments. add
 *  BDBG_OBJECT_ID_DECLARE.
 *
 * 4   7/14/08 4:10p vsilyaev
 * PR 44795: Removed duplicate types
 *
 * 3   6/23/08 6:15p mphillip
 * PR40027: Tidy up naming conventions to match with Nexus
 *
 * 2   6/17/08 12:31a mphillip
 * PR40027: Keyslot changes for shim layer
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/1   11/15/07 3:54p erickson
 * PR37137: added Security module
 *
 **************************************************************************/
#ifndef NEXUS_SECURITY_PRIV_H__
#define NEXUS_SECURITY_PRIV_H__

/*=***********************************
*************************************/

#include "bhsm.h"
#include "nexus_security_datatypes.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
If you get the HSM handle, you must first lock the security module, get the handle, 
use the handle, then unlock the security module. After unlocking the security module
you may not use the hsm handle.

This is allowed:
    NEXUS_Module_Lock(securityModule);
    {
        BHSM_Handle hsm;
        NEXUS_Security_GetHsm_priv(&hsm);
        BHSM_Foo(hsm);
    }
    NEXUS_Module_Unlock(securityModule);

This is not allowed:
    BHSM_Handle hsm;
    NEXUS_Module_Lock(securityModule);
    NEXUS_Security_GetHsm_priv(&hsm);
    NEXUS_Module_Unlock(securityModule);
    BHSM_Foo(hsm); <-- this is a bug
    
To ensure you are following these rules, we recommend that you do not cache or otherwise store
the HSM handle. Only use a local variable that goes out of scope before unlocking the module (as above).
**/
void NEXUS_Security_GetHsm_priv(
    BHSM_Handle *pHsm /* [out] */
    );

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_KeySlot);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
