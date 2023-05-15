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
 * $brcm_Revision: 29 $
 * $brcm_Date: 9/23/12 9:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/src/nexus_security_module.h $
 * 
 * 29   9/23/12 9:58p atruong
 * SW7360-32: Nexus Security Support for 7360
 * 
 * 28   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 27   3/30/12 4:09a atruong
 * SW7435-6: Add Nexus and HSM PI Security Support
 * 
 * 26   3/5/12 2:51p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 25   2/6/12 4:01p atruong
 * SW7429-6: HSM PI/Nexus Security Support for 7429A0
 * 
 * 24   10/17/11 5:35p atruong
 * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
 * 
 * 23   9/20/11 4:16p erickson
 * SW7346-495: add NSK2AVK
 * 
 * 22   7/12/11 12:37a atruong
 * SW7231-272: Undo some cleanup related to module inclusion
 * 
 * 21   7/1/11 5:11p yili
 * SW7231-272:Minor cleanup
 * 
 * 20   3/4/11 5:43p mphillip
 * SW7422-269: Merge module header bugfix to main
 * 
 * SW7422-269/1   3/3/11 4:57p mphillip
 * SW7422-269: Preliminary IV handling
 * 
 * 19   1/21/11 2:24p yili
 * SW7405-4221:Add support for new applications
 * 
 * 18   12/22/10 5:48p mphillip
 * SW7422-104: Merge 40nm support to /main
 * 
 * SW7422-104/1   12/16/10 6:14p mphillip
 * SW7422-104: Initial support for 7422
 * 
 * 17   10/14/10 1:38p erickson
 * SW7420-1168: Support for new BSP USER_HMAC_SHA command interface
 * 
 * 16   8/4/10 3:04p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 * 
 * SW7405-4325/1   8/4/10 2:31p mphillip
 * SW7405-4325: Move keyslot memory management into core
 * 
 * 15   6/30/10 3:25p mphillip
 * SW7550-415: Make security handle details internal to the module.
 * 
 * 14   3/14/10 11:44p atruong
 * SW7400-2678: Nexus AV Keyladder to support DirecTV
 * 
 * 13   10/27/09 2:32p yili
 * SW7340-42:Add raw command
 * 
 * 12   10/9/09 2:46a atruong
 * SW7405-3173: Nexus API call for reading OTP IDs
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
#include "priv/nexus_security_priv.h"

#if NEXUS_KEYLADDER
#include "nexus_keyladder.h"
#endif

#if NEXUS_OTPMSP
#include "nexus_otpmsp.h"
#endif

#if NEXUS_SECURITY_REGION_VERIFICATION
#include "nexus_security_regver.h"
#endif


#if NEXUS_USERCMD 
#include "nexus_random_number.h"
#include "nexus_read_otp_id.h"
#include "nexus_hmac_sha_cmd.h"
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || (BCHP_CHIP==7346) || \
	(BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7429) || (BCHP_CHIP==7435) || (BCHP_CHIP==7230) || \
	(BCHP_CHIP==7428) || (BCHP_CHIP==7360)
#include "nexus_bsp_config.h"
#endif
#endif

#if NEXUS_SECURITY_RAWCMD 
#include "nexus_security_rawcommand.h"
#endif

#if NEXUS_SECURITY_SECUREACCESS
#include "nexus_secureaccess.h"
#endif

#if NEXUS_SECURITY_SECURERSA
#include "nexus_security_rsa.h"
#endif   

#if NEXUS_SECURITY_IPLICENSING
#include "nexus_iplicensing.h"
#endif

#if NEXUS_SECURITY_AVKEYLADDER
#include "nexus_avkeyladder.h"
#endif

#ifdef NEXUS_SECURITY_MSIPTV
#include "nexus_security_msiptv.h"
#endif

#ifdef NEXUS_SECURITY_GENROOTKEY
#include "nexus_genrootkey.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME security
#define NEXUS_MODULE_SELF NEXUS_P_SecurityModule

#include "bhsm.h"
typedef struct NEXUS_Security_P_Handle
{
    BHSM_Handle hHsm;
    NEXUS_SecurityModuleSettings *pSettings;
} NEXUS_Security_P_Handle;



/* global handle. there is no global data. */
extern NEXUS_ModuleHandle NEXUS_P_SecurityModule;

#ifdef __cplusplus
}
#endif

#endif
