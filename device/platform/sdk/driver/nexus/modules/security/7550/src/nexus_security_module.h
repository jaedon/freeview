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
 * $brcm_Workfile: nexus_security_module.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/19/12 6:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7550/src/nexus_security_module.h $
 * 
 * 3   6/19/12 6:21p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 2   8/6/10 4:38p mphillip
 * SW7405-4325: Update 7550/7408 to keyslot implementation in core
 * 
 * 1   12/7/09 11:33a qcheng
 * SW7550-102 : Add Security support for 7550
 * 
 * 11   8/25/09 3:28p atruong
 * SW7405-2938: NEXUS API calls for BHSM Random Number Generator
 * 
 * 10   8/25/09 11:12a haoboy
 * SW7405-2870:Add support for secure RSA operations in Nexus
 * 
 * 9   8/25/09 1:09a atruong
 * SW7420-223: 7420B0 ASKM Support
 * 
 * 8   7/20/09 8:54a erickson
 * PR56912: added secure access extension
 *
 * 7   4/14/09 10:35a erickson
 * PR53745: fix warning
 *
 * 6   3/16/09 11:18a yili
 * PR52990:Add region verification
 *
 * 5   9/3/08 3:29p erickson
 * PR45612: update
 *
 * 4   7/17/08 2:19p mphillip
 * PR44912: Keyladder includes for building
 *
 * 3   6/24/08 4:57p vsilyaev
 * PR 40027: Added security module to 93556/93549 platform
 *
 * 2   6/17/08 12:33a mphillip
 * PR40027: Keyslot changes for shim
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/1   11/15/07 3:54p erickson
 * PR37137: added Security module
 *
 **************************************************************************/
#ifndef NEXUS_SECURITY_MODULE_H__
#define NEXUS_SECURITY_MODULE_H__

#include "nexus_security_thunks.h"
#include "nexus_base.h"
#include "nexus_security.h"
#include "nexus_security_misc.h"
#include "nexus_security_init.h"

#include "priv/nexus_core_security.h"

#if NEXUS_KEYLADDER
#include "nexus_keyladder.h"
#endif

#if NEXUS_OTPMSP
#include "nexus_otpmsp.h"
#endif

#ifdef NEXUS_SECURITY_REGVER
#include "nexus_security_regver.h"
#endif

#if NEXUS_SECUREACCESS
#include "nexus_secureaccess.h"
#endif

#if NEXUS_SECURERSA
#include "nexus_security_rsa.h"
#endif   

#if NEXUS_USERCMD 
#include "nexus_random_number.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME security
#define NEXUS_MODULE_SELF NEXUS_P_SecurityModule

/* global handle. there is no global data. */
extern NEXUS_ModuleHandle NEXUS_P_SecurityModule;
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_KeySlot);

#ifdef __cplusplus
}
#endif

#endif
