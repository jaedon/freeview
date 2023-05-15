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
 * $brcm_Workfile: nexus_security.c $
 * $brcm_Revision: 110 $
 * $brcm_Date: 12/5/12 1:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/src/nexus_security.c $
 * 
 * 110   12/5/12 1:33a atruong
 * SW7563-16: HSM PI support for 7563A0
 * 
 * 109   11/19/12 11:15a atruong
 * SW7584-46: HSM PI/Nexus Security support for 7584A0 - Also SW7435-232
 * 
 * 108   11/6/12 12:20p gmohile
 * SW7425-1708 : No need to release HSM resource for Generic key slot
 * 
 * 107   9/23/12 9:56p atruong
 * SW7360-32: Nexus Security Support for 7360
 * 
 * 106   7/27/12 2:14p jgarrett
 * SW7425-3281: Merge to main branch
 * 
 * SW7425-3281/2   7/17/12 5:07p jgarrett
 * SW7425-3281: Correcting cryptoEngine logic for generic key slots
 * 
 * SW7425-3281/1   7/8/12 4:54p piyushg
 * SW7425-3281: Added support for playready licensce acquisition and Nexus
 *  fileio
 * 
 * 105   6/22/12 1:43p vsilyaev
 * SW7420-2316: Moved NEXUS_OBJECT_CLASS(NEXUS_KeySlot) into the core
 *  module
 * 
 * 104   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 103   3/28/12 2:33p mphillip
 * SW7425-2628: Map rather than cast nexus enums to hsm enums
 * 
 * 102   3/6/12 10:02a atruong
 * SWSECURITY-113: add NEXUS_SecurityAlgorithm_eAesCounter alias
 * 
 * 101   3/5/12 2:50p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 100   2/8/12 3:18p mphillip
 * SWGIGGSVIZIO-203: Cast assignment to enum type to bypass coverity
 *  warning
 * 
 * 99   2/6/12 2:44p atruong
 * SW7429-6: HSM PI/Nexus Security Support for 7429A0
 * 
 * 98   1/25/12 4:15p mphillip
 * SW7125-1216: Coverity fix
 * 
 * 97   11/30/11 2:18p atruong
 * SW7425-1753: Accommodate new settings for 40-nm B1
 * 
 * 96   11/23/11 4:04p jtna
 * SW7425-1708: add s3 power management for security
 * 
 * 95   11/7/11 11:51a atruong
 * SWSECURITY-53: Code maintenace - SC Values only for CA.
 * 
 * 94   11/3/11 5:40p mward
 * SW7435-7:  Add 7435.
 * 
 * 93   10/31/11 3:33p mphillip
 * SW7344-208: Guarded bConfigClear so that only non-40nm platforms will
 *  see it (fixes a coverity warning on 40nm platforms)
 * 
 * 92   10/17/11 5:37p atruong
 * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
 * 
 * 91   10/13/11 10:31a mphillip
 * SW7346-481: Guard SC value variable
 * 
 * 90   10/12/11 12:37p mphillip
 * SW7346-481: Merge B0 changes to main
 * 
 * SW7346-481/2   9/9/11 7:13p mphillip
 * SW7346-481: Add bRestrictSourceDropPktEnable for HSM's
 *  bDropRregionPackets
 * 
 * SW7346-481/1   9/9/11 6:28p mphillip
 * SW7346-481: Initial 7346 B0 and 7425 B0 support
 * 
 * 89   9/22/11 9:58a atruong
 * SW7346-495: Nexus NSK2AVK extension module support - code cleanup
 * 
 * 88   9/21/11 1:39p atruong
 * SW7346-495: Add NSK2AVK support
 * 
 * 87   9/9/11 5:49p mphillip
 * SW7346-481: Add default ASKM configuration
 * 
 * 86   8/9/11 3:09p mphillip
 * SW7425-1041: Add missing keydestblock to 40nm config algorithm
 * 
 * 85   8/3/11 11:09a gmohile
 * SW7125-1014 : Add module to active priority scheduler
 * 
 * 84   7/13/11 5:29p atruong
 * SW7231-272: CPD and CPS Support only for ASKM platforms
 * 
 * 83   7/13/11 3:33p atruong
 * SW7231-272: CPD and CPS Support only for ASKM platforms
 * 
 * 82   7/12/11 1:14a atruong
 * SW7231-272: Code Cleanup to include CPD and CPS support
 * 
 * 81   7/7/11 11:41a mphillip
 * SW7358-56: Correct the keydest block for m2m keyslots
 * 
 * 80   7/6/11 11:55a atruong
 * SW7425-812: Fixed compile/runtime error for heap pointer supporting 40-
 *  nm HMAC-SHA
 * 
 * 79   6/30/11 2:57p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * 78   6/30/11 2:49a atruong
 * SW7552-41: Nexus Security Support For BCM7552 A0
 * 
 * 77   6/21/11 4:12p bselva
 * SW7358-50:Enable Nexus security support for 7358 platform
 * 
 * 77   6/21/11 4:08p bselva
 * SW7358-50:Enable Nexus security support for 7358 platform
 * 
 * 76   6/20/11 3:38p mphillip
 * SW7422-253: Fix pre-ASKM build error
 * 
 * 75   6/15/11 7:35p mphillip
 * SW7422-253: Remove extraneous debug statements from previous commit
 * 
 * 74   6/15/11 7:28p mphillip
 * SW7422-253: Bring 7420 SC bit implementation in line with 7422
 *  implementation
 * 
 * 73   5/9/11 6:55p mphillip
 * SW7422-430: Merge API change to main
 * 
 * 72   5/3/11 4:09p mward
 * SW7125-905: SW7420-1412:  Power on HSM during
 *  NEXUS_Security_P_InitHsm().
 * 
 * SW7422-430/2   5/2/11 11:21a mphillip
 * SW7422-430: Provide for separate odd/even CA configuration while still
 *  defaulting to configuring both
 * 
 * SW7422-430/1   4/25/11 11:57a mphillip
 * SW7422-430: Refactor key-type selection, allow user to specify which
 *  keyslot type is allocated
 * 
 * 71   3/25/11 5:01p mphillip
 * SW7420-1736: Only update nexus-level keyslot tracking when overwriting
 *  a pidchannel/keyslot association
 * 
 * 70   3/17/11 5:19p mphillip
 * SW7420-1495: Add algorithm tracking for aes destination block
 *  differentiation
 * 
 * 69   3/17/11 4:53p mphillip
 * SW7422-335: Merge API change to main
 * 
 * SW7422-335/1   3/10/11 5:05p mphillip
 * SW7422-335: Update key2select for 40nm
 * 
 * 68   3/10/11 3:52p mphillip
 * SW7422-269: Merge changes to main
 * 
 * 67   3/9/11 7:03p mphillip
 * SW7400-2948: Internal pidchannel/keyslot tracking to fix memory leaks
 *  and ensure keyslots are properly freed and associations are broken
 * 
 * SW7422-269/3   3/9/11 5:09p mphillip
 * SW7422-264: Update per-key IV datatype name.
 * 
 * SW7422-269/1   3/3/11 4:57p mphillip
 * SW7422-269: Preliminary IV handling
 * 
 * 66   2/24/11 3:57p mphillip
 * SW7422-104: Cleanup destination block logic
 * 
 * 65   2/23/11 7:46p mphillip
 * SW7422-104: Update SC bit handling and destination block handling
 * 
 * 64   2/23/11 11:14a mphillip
 * SW7422-253: Merge API changes to main
 * 
 * SW7422-253/2   2/16/11 7:41p mphillip
 * SW7422-253: Allow SC bits to be controlled per-packet-type
 * 
 * SW7422-253/1   2/15/11 7:55p mphillip
 * SW7422-253: Update 7422 support for building with BSP_SC_VALUE_SUPPORT
 * 
 * 63   2/16/11 11:09p hongtaoz
 * SW7425-103: add security support for 7425;
 * 
 * 62   2/2/11 4:11p jrubio
 * SW7346-19: add 7346/7344
 * 
 * 61   1/21/11 6:10p katrep
 * SW7231-7:add support for 7231
 * 
 * 60   1/21/11 2:19p yili
 * SW7405-4221:Add support for new applications
 * 
 * 59   1/19/11 12:16p mphillip
 * SW7420-1374: Update targets for 7420/7422 Ca, Cp, and CaCp operations
 * 
 * 58   1/3/11 11:00a atruong
 * SW7422-104: Compilation bug fix - setMiscBitsIO members
 * 
 * 57   12/30/10 1:53p atruong
 * SW7422-104: Graceful handling of unprogrammed chips (blank OTP)
 * 
 * 57   12/30/10 1:36p atruong
 * SW7422-104: Graceful handling of unprogrammed chips (blank OTP)
 * 
 * 56   12/23/10 10:48a mphillip
 * SW7422-104: Fix the algorithm mapping for non-40nm ASKM HSM
 * 
 * 55   12/22/10 5:48p mphillip
 * SW7422-104: Merge 40nm support to /main
 * 
 * SW7422-104/3   12/22/10 5:34p mphillip
 * SW7422-104: 7422 extended data handled
 * 
 * SW7422-104/2   12/22/10 11:41a mphillip
 * SW7422-104: Structure updates/fixups
 * 
 * SW7422-104/1   12/16/10 6:14p mphillip
 * SW7422-104: Initial support for 7422
 * 
 * 54   11/4/10 5:13p mphillip
 * SW7400-2948: Switch all BCHP_CHIP defines to feature defines for better
 *  and more centralized control
 * 
 * 53   10/20/10 2:09p mphillip
 * SW7400-2948: Add debug messages to track keyslot associations
 * 
 * 52   8/6/10 4:39p mphillip
 * SW7405-4325: Destroy keyslot debug object in core, not security
 * 
 * 51   8/4/10 3:04p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 * 
 * SW7405-4325/2   8/4/10 2:31p mphillip
 * SW7405-4325: Move keyslot memory management into core
 * 
 * SW7405-4325/1   7/2/10 7:23p mphillip
 * SW7405-4325: Move keyslot allocation/deallocation to base to break DMA
 *  dependency
 * 
 * 50   6/30/10 6:03p mphillip
 * SW7550-415: Error cleanup, memory management cleanup, keyslot debug
 *  object additions
 * 
 * 49   6/23/10 2:01p mphillip
 * SW7550-415: Whitespace/formatting clean-up in preparation for other
 *  code cleanup
 * 
 * 48   5/10/10 11:04a erickson
 * SW7550-415: add NEXUS_Security_GetDefaultClearKey
 *
 * 47   4/21/10 3:44p yili
 * SW7420-457:External IV support
 *
 * 46   3/16/10 11:23a atruong
 * SW7400-2678: Fixed parameter naming for InvalidateKey function
 *
 * 45   3/15/10 12:14p atruong
 * SW7400-2678: Nexus AV Keyladder to support DirecTV
 *
 * 44   3/14/10 11:44p atruong
 * SW7400-2678: Nexus AV Keyladder to support DirecTV
 *
 * 43   2/3/10 11:39a yili
 * SW7420-457:Add SC moidfication support for clear packet
 *
 * 42   12/14/09 4:35p atruong
 * SW7468-16: Support For 7468A0 - bug fix
 *
 * 41   12/9/09 5:01p atruong
 * SW7468-16: Support For 7468A0
 *
 * 40   11/18/09 2:55p yili
 * SW7340-42:Remove region verification code
 *
 * 39   10/27/09 2:38p yili
 * SW7340-42:minor change
 *
 * 38   10/27/09 2:29p yili
 * SW7340-42:Added region verification
 *
 * 37   9/10/09 1:50p mward
 * SW7420-223: Initialize residualMode to prevent compiler warning.
 *
 * 36   8/25/09 1:09a atruong
 * SW7420-223: 7420B0 ASKM Support
 *
 * 35   8/21/09 3:45p mward
 * PR55545: Adding 7125.
 *
 * 34   8/12/09 3:41p atruong
 * PR55169: Support for 7420B0 ASKM
 *
 * 33   7/10/09 6:06p atruong
 * PR55169: NEXUS/HSM PI support for 7420A1
 *
 * 32   4/29/09 10:25a yili
 * PR53745:Fixed compiler warning
 *
 * 31   4/28/09 12:12p yili
 * PR53745:Minor bug fix.
 *
 * 30   4/27/09 2:14p yili
 * PR53745: Allow users to configure CA/CP key slot settings for Nexus
 *  security.
 *
 * 29   4/15/09 12:02p yili
 * PR53745:Add nexus security sample code
 *
 * 28   4/13/09 4:43p katrep
 * PR53733: MSDRM PD HW DECRYPT
 *
 * 27   4/2/09 11:36a mphillip
 * PR53809: Enable AES for 3548/3556 CA
 *
 * 26   4/1/09 3:18p katrep
 * PR53733: Fixed compilation issue when MSDRM_PD_SUPPORT is not enabled
 *
 * 25   4/1/09 1:20p katrep
 * PR53733: Added security algorithm type for MSDRM PD HGW DECRYPT
 *
 * 24   3/18/09 11:25a mphillip
 * PR53355: Free handle when freeing keyslot
 *
 * 23   3/10/09 8:50p mphillip
 * PR52786: Let power management know the hsm core should be powered on
 *  when keys have been allocated
 *
 * 22   3/9/09 12:15p mphillip
 * PR52786: Add timestamp support for keyslot configuration
 *
 * 21   1/27/09 11:29a erickson
 * PR50367: add memset after malloc of handle
 *
 * 20   1/7/09 2:46p mphillip
 * PR50093: Add Multi2 support
 *
 * 19   9/9/08 11:49a mphillip
 * PR44933: Clarify keyslot allocation to avoid multiple allocations on
 *  m2m
 *
 * 18   9/3/08 3:29p erickson
 * PR45612: update
 *
 * 17   8/20/08 11:28a mphillip
 * PR45884: Add Remux support
 *
 * 16   7/23/08 11:25a mphillip
 * PR45072: Fix switch conditional (thanks, Coverity!)
 *
 * 15   7/14/08 3:31p mphillip
 * PR44795: Accommodate type redefinition
 *
 * 14   6/24/08 4:57p vsilyaev
 * PR 40027: Added security module to 93556/93549 platform
 *
 * 13   6/24/08 11:51a mphillip
 * PR40027: Re-enable m2m key slot allocation
 *
 * 12   6/23/08 6:15p mphillip
 * PR40027: Tidy up naming conventions to match with Nexus
 *
 * 11   6/17/08 6:16p mphillip
 * PR40027: Retrieve more information from KeySlotHandle
 *
 * 10   6/17/08 12:33a mphillip
 * PR40027: Keyslot changes for shim
 *
 * 9   5/29/08 10:52a mphillip
 * PR38369: Add work-around for DTV chips missing some enums
 *
 * 8   5/28/08 2:32p mphillip
 * PR38369: Refactor keyslot configuration to Security module from Crypto
 *  module
 *
 * 7   5/27/08 5:05p erickson
 * PR34925: added b_get_reg, b_get_int and b_get_chp
 *
 * 6   5/22/08 3:31p erickson
 * PR34925: add b_get_hsm for debug
 *
 * 5   5/12/08 4:41p erickson
 * PR42628: call BHSM_Channel_Close when closing the module
 *
 * 4   5/6/08 4:44p erickson
 * PR39453: remove BHSM_SetMiscBitsIO_t for 3548/3556 as instructed
 *
 * 3   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 2   2/29/08 11:04a erickson
 * PR37137: create stub to remove sync thunk warning
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/15/07 4:01p erickson
 * PR37137: security update
 *
 * Nexus_Devel/1   11/15/07 3:54p erickson
 * PR37137: added Security module
 *
 **************************************************************************/
#include "nexus_security_module.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"
#include "priv/nexus_security_standby_priv.h"

#include "nexus_power_management.h"

#include "bhsm.h"
#include "bsp_s_commands.h"
#include "bsp_s_misc.h"
#include "bsp_s_hw.h"
#include "bsp_s_keycommon.h"
#include "bhsm_keyladder.h"

#include "nexus_base.h"

BDBG_MODULE(nexus_security);

/* Feature defines.  There should be NO #if BCHP_CHIP macros controlling features.
 * Instead, maintain the chip/version list here, and switch features via these defines. */


/* DigitalTV chip like 3563 has only ONE HSM channel, no cancel cmd support */
#if (BCHP_CHIP!=3563)
#define HAS_TWO_HSM_CMD_CHANNELS 1
#endif

/* This define controls whether to include the AES, CSS, and C2 remapping in the functions.
 * The 3563 and 3548/3556 HSM PI has commented certain enums out, which has a ripple effect
 * in generic code. */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556)
#define SUPPORT_NON_DTV_CRYPTO 1
#endif

/* A change requires AES to be broken out for these chips */
#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#define SUPPORT_AES_FOR_DTV 1
#endif

/* To be investigated to see if this option should be on for ASKM platforms */
#if (BCHP_CHIP!=3563) && (BCHP_CHIP!=3548) && (BCHP_CHIP!=3556) && (BCHP_CHIP!=7422) && \
     (BCHP_CHIP!=7425) && (BCHP_CHIP!=7231) && (BCHP_CHIP!=7344) && (BCHP_CHIP!=7346) && \
     (BCHP_CHIP!=7358) && (BCHP_CHIP!=7552) && (BCHP_CHIP!=7435) && (BCHP_CHIP!=7429) && \
    (BCHP_CHIP!=7360)  && (BCHP_CHIP!=7563)
#define SET_MISC_BITS 1
#endif

#if ((BCHP_CHIP == 7420) && (BCHP_VER >= BCHP_VER_A1)) || (BCHP_CHIP == 7340) || \
     (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7468)
#define HAS_TYPE7_KEYSLOTS 1
#endif

#if ((BCHP_CHIP == 7420) && (BCHP_VER >= BCHP_VER_A1)) || (BCHP_CHIP == 7340) || \
     (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7346) || \
     (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) || (BCHP_CHIP == 7344) || \
     (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7429) || \
	 (BCHP_CHIP == 7360) || (BCHP_CHIP == 7584) || (BCHP_CHIP == 7563)
#define HSM_IS_ASKM 1
#endif

#if !HSM_IS_ASKM_40NM
#define HAS_TYPE5_KEYSLOTS 1
#include "bhsm_misc.h"
#endif

#define MAKE_HSM_ERR(x) (NEXUS_SECURITY_HSM_ERROR | x)

#include "blst_slist.h"
typedef struct NEXUS_Security_P_PidChannelListNode {
    BLST_S_ENTRY(NEXUS_Security_P_PidChannelListNode) next;
    unsigned long pidchannel;
} NEXUS_Security_P_PidChannelListNode;
BLST_S_HEAD(NEXUS_Security_P_PidChannelList_t, NEXUS_Security_P_PidChannelListNode);

typedef struct NEXUS_Security_P_KeySlotListNode {
    BLST_S_ENTRY(NEXUS_Security_P_KeySlotListNode) next;
    NEXUS_KeySlotHandle handle;
    struct NEXUS_Security_P_PidChannelList_t pidchannels;
    NEXUS_SecurityAlgorithm algorithm;
} NEXUS_Security_P_KeySlotListNode;

BLST_S_HEAD(NEXUS_Security_P_KeySlotList_t, NEXUS_Security_P_KeySlotListNode);

#if 0
#define NEXUS_SECURITY_DUMP_KEYSLOTS \
    { \
        NEXUS_Security_P_KeySlotListNode *p; \
        for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) { \
            NEXUS_Security_P_PidChannelListNode *q; \
            BDBG_MSG(("%s: Keyslot [%p]",__FUNCTION__,p->handle)); \
            for (q=BLST_S_FIRST(&p->pidchannels);q!=NULL;q=BLST_S_NEXT(q,next)) { \
                BDBG_MSG(("%s:   keyslot [%p] has pidchannel [%d]",__FUNCTION__,p->handle,q->pidchannel)); \
            } \
            BDBG_MSG(("%s: end of Keyslot [%p]",__FUNCTION__,p->handle)); \
        } \
    }
#else
#define NEXUS_SECURITY_DUMP_KEYSLOTS
#endif

NEXUS_ModuleHandle NEXUS_P_SecurityModule = NULL;
static struct {
    NEXUS_SecurityModuleSettings settings;
    BHSM_Handle hsm;
    BHSM_ChannelHandle hsmChannel[BHSM_HwModule_eMax];
    struct NEXUS_Security_P_KeySlotList_t keyslots;
} g_security;

static BERR_Code NEXUS_Security_P_InitHsm(const NEXUS_SecurityModuleSettings * pSettings);
static void NEXUS_Security_P_UninitHsm(void);


/*
 * Helper functions
 */
static NEXUS_Security_P_KeySlotListNode *NEXUS_Security_P_LocateNodeByKeyslot(NEXUS_KeySlotHandle keyHandle)
{
    NEXUS_Security_P_KeySlotListNode *p = NULL;

    for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
        if (p->handle == keyHandle)
            break;
    }

    return p;
}

static void NEXUS_Security_P_RemovePidchannel(NEXUS_Security_P_KeySlotListNode *p, NEXUS_Security_P_PidChannelListNode *q) {
    BLST_S_REMOVE(&p->pidchannels,q,NEXUS_Security_P_PidChannelListNode,next);
    BKNI_Free(q);
}

/****************************************
 * Module functions
 ****************************************/



void NEXUS_SecurityModule_GetDefaultSettings(NEXUS_SecurityModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#if HSM_IS_ASKM_40NM
    pSettings->numKeySlotsForType[0] = 10;
    pSettings->numKeySlotsForType[1] = 7;
    pSettings->numKeySlotsForType[2] = 11;
    pSettings->numKeySlotsForType[3] = 5;
    pSettings->numKeySlotsForType[4] = 6;
#else
    pSettings->numKeySlotsForType[0] = 10;
    pSettings->numKeySlotsForType[1] = 7;
    pSettings->numKeySlotsForType[2] = 11;
    pSettings->numKeySlotsForType[3] = 5;
    pSettings->numKeySlotsForType[4] = 6;
    pSettings->numKeySlotsForType[5] = 2;
    pSettings->numKeySlotsForType[6] = 8;
#if HAS_TYPE7_KEYSLOTS
    pSettings->numKeySlotsForType[7] = 4;
#endif
#endif

}

NEXUS_ModuleHandle NEXUS_SecurityModule_Init(const NEXUS_SecurityModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_SecurityModuleSettings defaultSettings;
    BERR_Code rc;

    BDBG_ASSERT(!NEXUS_P_SecurityModule);

    if (!pSettings) {
        NEXUS_SecurityModule_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLowActiveStandby;
    NEXUS_P_SecurityModule = NEXUS_Module_Create("security", &moduleSettings);
    if (!NEXUS_P_SecurityModule) {
        return NULL;
    }

    g_security.settings = *pSettings;

    rc = NEXUS_Security_P_InitHsm(pSettings);
    if (rc) {
        NEXUS_Module_Destroy(NEXUS_P_SecurityModule);
        NEXUS_P_SecurityModule = NULL;
    } else {
        BLST_S_INIT(&g_security.keyslots);
    }
    return NEXUS_P_SecurityModule;
}

void NEXUS_SecurityModule_GetCurrentSettings(NEXUS_ModuleHandle module, NEXUS_SecurityModuleSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(module);
    *pSettings = g_security.settings;
}


void NEXUS_SecurityModule_Uninit(void)
{
    NEXUS_SECURITY_DUMP_KEYSLOTS;
    while (NULL != BLST_S_FIRST(&g_security.keyslots)) {
        NEXUS_Security_P_KeySlotListNode *p;
        p=BLST_S_FIRST(&g_security.keyslots);
        BDBG_WRN(("Keyslot %p was not freed, freeing",p->handle));
        NEXUS_Security_FreeKeySlot(p->handle);
    }
    NEXUS_Security_P_UninitHsm();
    NEXUS_Module_Destroy(NEXUS_P_SecurityModule);
    NEXUS_P_SecurityModule = NULL;
}

static BERR_Code NEXUS_Security_P_InitHsm(const NEXUS_SecurityModuleSettings * pSettings)
{
    BHSM_Settings hsmSettings;
    BHSM_ChannelSettings hsmChnlSetting;
    BHSM_InitKeySlotIO_t keyslot_io;
    BERR_Code rc;

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);

    BHSM_GetDefaultSettings(&hsmSettings, g_pCoreHandles->chp);
#if HSM_IS_ASKM_40NM
    hsmSettings.hHeap = g_pCoreHandles->heap[0];
#endif
    rc = BHSM_Open(&g_security.hsm, g_pCoreHandles->reg, g_pCoreHandles->chp, g_pCoreHandles->bint, &hsmSettings);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }

    rc = BHSM_GetChannelDefaultSettings(g_security.hsm, BHSM_HwModule_eCmdInterface1, &hsmChnlSetting);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }

#if HAS_TWO_HSM_CMD_CHANNELS
    rc = BHSM_Channel_Open(g_security.hsm, &g_security.hsmChannel[BHSM_HwModule_eCmdInterface1], BHSM_HwModule_eCmdInterface1, &hsmChnlSetting);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }
#endif

    rc = BHSM_Channel_Open(g_security.hsm, &g_security.hsmChannel[BHSM_HwModule_eCmdInterface2], BHSM_HwModule_eCmdInterface2, &hsmChnlSetting);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }

    BKNI_Memset(&keyslot_io, 0, sizeof(keyslot_io));

    keyslot_io.unKeySlotType0Num = pSettings->numKeySlotsForType[0];
    keyslot_io.unKeySlotType1Num = pSettings->numKeySlotsForType[1];
    keyslot_io.unKeySlotType2Num = pSettings->numKeySlotsForType[2];
    keyslot_io.unKeySlotType3Num = pSettings->numKeySlotsForType[3];
    keyslot_io.unKeySlotType4Num = pSettings->numKeySlotsForType[4];
#if HAS_TYPE5_KEYSLOTS
    keyslot_io.unKeySlotType5Num = pSettings->numKeySlotsForType[5];
    keyslot_io.unKeySlotType6Num = pSettings->numKeySlotsForType[6];
#if HAS_TYPE7_KEYSLOTS
    keyslot_io.unKeySlotType7Num = pSettings->numKeySlotsForType[7];
#endif
#endif
#if HSM_IS_ASKM_40NM
    keyslot_io.bConfigMulti2KeySlot = pSettings->enableMulti2Key;
#endif
    /* Disregard errors, as this can only be run once per board boot. */
    rc = BHSM_InitKeySlot(g_security.hsm, &keyslot_io);
    /* Print out warning and ignore the error */
    if (rc)
    {
        BDBG_WRN(("**********************************************"));
        BDBG_WRN(("If you see this warning and the HSM errors above, you need to perform some"));
        BDBG_WRN(("board reconfiguration. This is not required. If you want, you can ignore them."));
        BDBG_WRN(("Use BBS to check if BSP_GLB_NONSECURE_GLB_IRDY = 0x07."));
        BDBG_WRN(("If not, BSP/Aegis is not ready to accept a command."));
        BDBG_WRN(("SUN_TOP_CTRL_STRAP_VALUE[bit28:29 strap_test_debug_en] should be 0b'00 if you plan"));
        BDBG_WRN(("to use BSP ROM code. If not, check with board designer on your strap pin."));
        BDBG_WRN(("**********************************************"));
        rc = BERR_SUCCESS;
    }

#if SET_MISC_BITS
    {
        BHSM_SetMiscBitsIO_t setMiscBitsIO;

        setMiscBitsIO.setMiscBitsSubCmd = BCMD_SetMiscBitsSubCmd_eRaveBits;
        setMiscBitsIO.bEnableWriteIMem = 1;
        setMiscBitsIO.bEnableReadIMem = 1;
        setMiscBitsIO.bEnableReadDMem = 1;
        setMiscBitsIO.bDisableClear = 1;
#if HSM_IS_ASKM_40NM
        setMiscBitsIO.bRAVEEncryptionBypass = 0;
#else
        setMiscBitsIO.bEnableEncBypass = 0;
#endif
        rc = BHSM_SetMiscBits(g_security.hsm, &setMiscBitsIO);
        if (rc) {
            BDBG_WRN(("**********************************************"));
            BDBG_WRN(("If you see this warning and the HSM errors above, you need to perform some"));
            BDBG_WRN(("board reconfiguration. This is not required. If you want, you can ignore them."));
            BDBG_WRN(("Use BBS to check if BSP_GLB_NONSECURE_GLB_IRDY = 0x07."));
            BDBG_WRN(("If not, BSP/Aegis is not ready to accept a command."));
            BDBG_WRN(("SUN_TOP_CTRL_STRAP_VALUE[bit28:29 strap_test_debug_en] should be 0b'00 if you plan"));
            BDBG_WRN(("to use BSP ROM code. If not, check with board designer on your strap pin."));
            BDBG_WRN(("**********************************************"));
            rc = BERR_SUCCESS;
        }
    }
#endif
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, false);

    return rc;
}

void NEXUS_Security_P_UninitHsm()
{
#if HAS_TWO_HSM_CMD_CHANNELS
    (void)BHSM_Channel_Close(g_security.hsmChannel[BHSM_HwModule_eCmdInterface1]);
#endif
    (void)BHSM_Channel_Close(g_security.hsmChannel[BHSM_HwModule_eCmdInterface2]);
    BHSM_Close(g_security.hsm);
}

void NEXUS_Security_GetHsm_priv(BHSM_Handle *pHsm)
{
    NEXUS_ASSERT_MODULE();
    *pHsm = g_security.hsm;
}

void NEXUS_Security_GetDefaultKeySlotSettings(NEXUS_SecurityKeySlotSettings *pSettings)
{
    if (pSettings) {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
        pSettings->keySlotEngine = NEXUS_SecurityEngine_eCa;
#if HSM_IS_ASKM
        pSettings->keySlotSource = NEXUS_SecurityKeySource_eFirstRamAskm;
#else
        pSettings->keySlotSource = NEXUS_SecurityKeySource_eFirstRam;
#endif
        pSettings->keySlotType = NEXUS_SecurityKeySlotType_eAuto;
    }
}

void NEXUS_Security_GetKeySlotInfo(NEXUS_KeySlotHandle keyHandle, NEXUS_SecurityKeySlotInfo *pKeyslotInfo)
{
    NEXUS_P_Core_GetKeySlotInfo(keyHandle, pKeyslotInfo);
}

static NEXUS_Error NEXUS_Security_P_GetInvalidateKeyFlag(const NEXUS_SecurityInvalidateKeyFlag nexusKeyFlag, BCMD_InvalidateKey_Flag_e *hsmKeyFlag) {
    switch (nexusKeyFlag) {
    case NEXUS_SecurityInvalidateKeyFlag_eSrcKeyOnly:
        *hsmKeyFlag = BCMD_InvalidateKey_Flag_eSrcKeyOnly;
        break;
    case NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly:
        *hsmKeyFlag = BCMD_InvalidateKey_Flag_eDestKeyOnly;
        break;
    case NEXUS_SecurityInvalidateKeyFlag_eAllKeys:
        *hsmKeyFlag = BCMD_InvalidateKey_Flag_eBoth;
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return NEXUS_SUCCESS;
}

static BCMD_KeyDestEntryType_e NEXUS_Security_MapNexusKeyDestToHsm(NEXUS_KeySlotHandle keyHandle, NEXUS_SecurityKeyType keydest)
{
    BCMD_KeyDestEntryType_e rv = BCMD_KeyDestEntryType_eOddKey;
    BSTD_UNUSED(keyHandle);
    switch (keydest) {
    case NEXUS_SecurityKeyType_eOddAndEven:
    case NEXUS_SecurityKeyType_eOdd:
        rv = BCMD_KeyDestEntryType_eOddKey;
        break;
    case NEXUS_SecurityKeyType_eEven:
        rv = BCMD_KeyDestEntryType_eEvenKey;
        break;
    case NEXUS_SecurityKeyType_eClear:
#if HSM_IS_ASKM_40NM
        rv = BCMD_KeyDestEntryType_eReserved2;
#else
        rv = BCMD_KeyDestEntryType_eReserved0;
#endif
        break;
#if !HSM_IS_ASKM_40NM
    case NEXUS_SecurityKeyType_eIv:
        rv = BCMD_KeyDestEntryType_eIV;
        break;
#endif
    default:
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return rv;
}

#define NEXUS_SECURITY_CACP_INVALID_PIDCHANNEL 0xFFFFFFFF

static NEXUS_Error NEXUS_Security_AllocateKeySlotForType(NEXUS_KeySlotHandle *pKeyHandle, NEXUS_SecurityEngine engine, BCMD_XptSecKeySlot_e type)
{
    BERR_Code rc;
    NEXUS_KeySlotHandle pHandle;
    unsigned int keyslotNumber;

    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); }

    NEXUS_OBJECT_INIT(NEXUS_KeySlot, pHandle);

    rc = BHSM_AllocateCAKeySlot(g_security.hsm, type, &keyslotNumber);
    if (rc) goto error;

    pHandle->keyslotType = type;
    pHandle->pidChannel = NEXUS_SECURITY_CACP_INVALID_PIDCHANNEL;
    pHandle->keySlotNumber = keyslotNumber;
    pHandle->cryptoEngine = engine;

    *pKeyHandle = pHandle;

    BDBG_MSG(("%s: Allocated keyslot %p",__FUNCTION__,pHandle));

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);

    return NEXUS_SUCCESS;
error:
    NEXUS_P_Core_FreeKeySlotHandle(pHandle);
    *pKeyHandle = NULL;
    return BERR_TRACE(MAKE_HSM_ERR(rc));
}

NEXUS_KeySlotHandle NEXUS_Security_LocateCaKeySlotAssigned(unsigned long pidchannel)
{
    BERR_Code rc;
    NEXUS_Security_P_KeySlotListNode *p;
    unsigned int keyslotNumber;
    unsigned int keyslotType;

    NEXUS_SECURITY_DUMP_KEYSLOTS;

    rc = BHSM_LocateCAKeySlotAssigned(g_security.hsm, pidchannel, BHSM_PidChannelType_ePrimary, &keyslotType, &keyslotNumber);
    if (rc) {
        rc = BERR_TRACE(MAKE_HSM_ERR(rc));
        goto error;
    }
    for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
        NEXUS_Security_P_PidChannelListNode *q;
        for (q=BLST_S_FIRST(&p->pidchannels);q!=NULL;q=BLST_S_NEXT(q,next)) {
            if (q->pidchannel == pidchannel) {
                if (p->handle->keySlotNumber != keyslotNumber || p->handle->keyslotType != keyslotType) {
                    BDBG_WRN(("%s: number/type difference, keyslot tracking may be out of sync with HSM",__FUNCTION__));
                }
                return p->handle;
            }
        }
    }
error:
    BDBG_WRN(("%s: PID Channel %d is not yet associated with any key slot",__FUNCTION__,pidchannel));
    return NULL;
}

NEXUS_Error NEXUS_Security_AllocateCaKeySlot(NEXUS_KeySlotHandle *pKeyHandle)
{
#if HSM_IS_ASKM_40NM
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle, NEXUS_SecurityEngine_eCa, BCMD_XptSecKeySlot_eType0);
#else
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle, NEXUS_SecurityEngine_eCa, BCMD_XptSecKeySlot_eType1);
#endif
}

NEXUS_Error NEXUS_Security_AllocateCaCpKeySlot(NEXUS_KeySlotHandle *pKeyHandle)
{
#if HSM_IS_ASKM_40NM
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle, NEXUS_SecurityEngine_eCaCp, BCMD_XptSecKeySlot_eType0);
#else
#if HAS_TYPE7_KEYSLOTS
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle,NEXUS_SecurityEngine_eCaCp, BCMD_XptSecKeySlot_eType7);
#elif !HAS_TYPE5_KEYSLOTS
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle, NEXUS_SecurityEngine_eCaCp, BCMD_XptSecKeySlot_eType4);
#else
    return NEXUS_Security_AllocateKeySlotForType(pKeyHandle, NEXUS_SecurityEngine_eCaCp, BCMD_XptSecKeySlot_eType6);
#endif
#endif
}

NEXUS_Error NEXUS_Security_AddPidChannelToKeySlot(NEXUS_KeySlotHandle keyHandle, const unsigned int pidChannel)
{
    BHSM_ConfigPidKeyPointerTableIO_t configPidKeyPointerTableIO;
    BERR_Code rc;
    NEXUS_Security_P_KeySlotListNode *p;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    BDBG_MSG(("%s: %p(%d,%d)",__FUNCTION__,keyHandle,keyHandle->keySlotNumber,keyHandle->keyslotType));

    for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
        NEXUS_Security_P_PidChannelListNode *q = NULL;
        for (q=BLST_S_FIRST(&p->pidchannels);q!=NULL;q=BLST_S_NEXT(q,next)) {
            if ((q->pidchannel == pidChannel) && (p->handle != keyHandle)) {
                BDBG_MSG(("pidchannel %d already has keyslot %p associated, breaking association",pidChannel,p->handle));
                NEXUS_Security_P_RemovePidchannel(p,q);
                break;
            }
        }
    }
    p = NEXUS_Security_P_LocateNodeByKeyslot(keyHandle);

    configPidKeyPointerTableIO.unPidChannel = pidChannel;
    configPidKeyPointerTableIO.ucKeySlotType = keyHandle->keyslotType;
    configPidKeyPointerTableIO.unKeySlotNum = keyHandle->keySlotNumber;
    configPidKeyPointerTableIO.pidChannelType = BHSM_PidChannelType_ePrimary;
#if HSM_IS_ASKM_40NM
    configPidKeyPointerTableIO.spidProgType = BHSM_SPIDProg_ePIDPointerA;
    configPidKeyPointerTableIO.bResetPIDToDefault = false;
    configPidKeyPointerTableIO.unKeySlotBType = 0;
#else
    configPidKeyPointerTableIO.unKeySlotB = 0;
#endif
    configPidKeyPointerTableIO.unKeySlotNumberB = 0;
    configPidKeyPointerTableIO.unKeyPointerSel = 0;

    rc = BHSM_ConfigPidKeyPointerTable(g_security.hsm, &configPidKeyPointerTableIO);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }

    if (p) {
        NEXUS_Security_P_PidChannelListNode *q = NULL;
        bool has_pidchannel = false;
        for (q=BLST_S_FIRST(&p->pidchannels);q!=NULL;q=BLST_S_NEXT(q,next)) {
            if (q->pidchannel == pidChannel) {
                has_pidchannel = true;
            }
        }
        if (!has_pidchannel) {
            NEXUS_Security_P_PidChannelListNode *node;
            node = BKNI_Malloc(sizeof(*node));
            if (node) {
                BKNI_Memset(node,0,sizeof(*node));
                node->pidchannel = pidChannel;
                BLST_S_INSERT_HEAD(&p->pidchannels, node, next);
            }
        }
    }
    /* deprecated */
    keyHandle->pidChannel = pidChannel;

    NEXUS_SECURITY_DUMP_KEYSLOTS;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Security_RemovePidChannelFromKeySlot(NEXUS_KeySlotHandle keyHandle, const unsigned int pidChannel)
{
    BHSM_ConfigPidKeyPointerTableIO_t configPidKeyPointerTableIO;
    BERR_Code rc;
    NEXUS_Security_P_KeySlotListNode *p;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    BDBG_MSG(("%s: %p(%d,%d,%d)",__FUNCTION__,keyHandle,keyHandle->keySlotNumber,keyHandle->keyslotType,pidChannel));

    p = NEXUS_Security_P_LocateNodeByKeyslot(keyHandle);
    if (p) {
        NEXUS_Security_P_PidChannelListNode *q;
        for (q=BLST_S_FIRST(&p->pidchannels);q!=NULL;q=BLST_S_NEXT(q,next)) {
            if (q->pidchannel == pidChannel) {
                NEXUS_Security_P_RemovePidchannel(p,q);
                configPidKeyPointerTableIO.unPidChannel = pidChannel;
                configPidKeyPointerTableIO.ucKeySlotType = keyHandle->keyslotType;
                configPidKeyPointerTableIO.unKeySlotNum = keyHandle->keySlotNumber;
                configPidKeyPointerTableIO.pidChannelType = BHSM_PidChannelType_ePrimary;
#if HSM_IS_ASKM_40NM
                configPidKeyPointerTableIO.spidProgType = BHSM_SPIDProg_ePIDPointerA;
                configPidKeyPointerTableIO.bResetPIDToDefault = false;
                configPidKeyPointerTableIO.unKeySlotBType = 0;
#else
                configPidKeyPointerTableIO.unKeySlotB = 0;
#endif
                configPidKeyPointerTableIO.unKeySlotNumberB = 0;
                configPidKeyPointerTableIO.unKeyPointerSel = 0;

                rc = BHSM_ConfigPidChannelToDefaultKeySlot(g_security.hsm, &configPidKeyPointerTableIO);
                if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }
                p->handle->pidChannel = NEXUS_SECURITY_CACP_INVALID_PIDCHANNEL; /* deprecated */
                break;
            }
        }
    }

    NEXUS_SECURITY_DUMP_KEYSLOTS;

    return NEXUS_SUCCESS;
}

static BCMD_XptSecKeySlot_e NEXUS_Security_MapNexusKeySlotTypeToHsm(NEXUS_SecurityKeySlotType slotType, NEXUS_SecurityEngine engine)
{
    BCMD_XptSecKeySlot_e rvType = BCMD_XptSecKeySlot_eType0;
    if (slotType != NEXUS_SecurityKeySlotType_eAuto) {
        /* We were given a specific HSM keyslot type, therefore perform a straight mapping */
        switch(slotType) {
            /* The _eTypeX values map directly to the equivalent HSM variable
             * Macro trickery to avoid copy/paste errors */
#define NEXUS_REMAP_HSM_ETYPE(VAL) \
            case NEXUS_SecurityKeySlotType_eType##VAL : \
                rvType = BCMD_XptSecKeySlot_eType##VAL ; \
                break;
            NEXUS_REMAP_HSM_ETYPE(0)
            NEXUS_REMAP_HSM_ETYPE(1)
            NEXUS_REMAP_HSM_ETYPE(2)
            NEXUS_REMAP_HSM_ETYPE(3)
            NEXUS_REMAP_HSM_ETYPE(4)
#if HAS_TYPE5_KEYSLOTS
            NEXUS_REMAP_HSM_ETYPE(5)
            NEXUS_REMAP_HSM_ETYPE(6)
#endif
#if HAS_TYPE7_KEYSLOTS
            NEXUS_REMAP_HSM_ETYPE(7)
#endif
#undef NEXUS_REMAP_HSM_ETYPE
        case NEXUS_SecurityKeySlotType_eAuto:
        default:
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            BDBG_ASSERT(0);
        }
    } else {
        /* Determine the keyslot type based on... */
        switch (engine) {
        case NEXUS_SecurityEngine_eM2m:
#if HSM_IS_ASKM_40NM
            rvType = BCMD_XptSecKeySlot_eType0;
#else
            rvType = BCMD_XptSecKeySlot_eType1;
#endif
            break;
        case NEXUS_SecurityEngine_eCa:
#if HSM_IS_ASKM_40NM
            rvType = BCMD_XptSecKeySlot_eType0;
#else
            rvType = BCMD_XptSecKeySlot_eType1;
#endif
            break;
        case NEXUS_SecurityEngine_eCaCp:
        case NEXUS_SecurityEngine_eRmx:
#if HSM_IS_ASKM_40NM
            rvType = BCMD_XptSecKeySlot_eType0;
#else
#if HAS_TYPE7_KEYSLOTS
            rvType = BCMD_XptSecKeySlot_eType7;
#elif !HAS_TYPE5_KEYSLOTS
            rvType = BCMD_XptSecKeySlot_eType4;
#else
            rvType = BCMD_XptSecKeySlot_eType6;
#endif
#endif
            break;
        case NEXUS_SecurityEngine_eCp:
        default:
            /* an unsupported or invalid security engine value was passed in. */
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            BDBG_ASSERT(0);
        }
    }
    return rvType;
}

NEXUS_Error NEXUS_Security_AllocateM2mKeySlot(NEXUS_KeySlotHandle * pKeyHandle)
{
    BERR_Code rc;
    NEXUS_KeySlotHandle pHandle;
    unsigned int keyslotNumber;

    BDBG_MSG(("Allocating M2M keyslot"));

    BDBG_ASSERT(pKeyHandle);
    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { *pKeyHandle = NULL; return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); }

    NEXUS_OBJECT_INIT(NEXUS_KeySlot, pHandle);

    rc = BHSM_AllocateM2MKeySlot(g_security.hsm, &keyslotNumber);
    if (rc) {
        NEXUS_P_Core_FreeKeySlotHandle(pHandle);
        *pKeyHandle = NULL;
        BDBG_MSG(("Failed to allocate M2M keyslot (rc = %x)", rc));
        return BERR_TRACE(MAKE_HSM_ERR(rc));
    }

    pHandle->keyslotType = NEXUS_Security_MapNexusKeySlotTypeToHsm(pHandle->keyslotType,NEXUS_SecurityEngine_eM2m);
    pHandle->keySlotNumber = keyslotNumber;
    pHandle->cryptoEngine = NEXUS_SecurityEngine_eM2m;
    BDBG_MSG(("Allocated M2M keyslot %d", keyslotNumber));

    *pKeyHandle = pHandle;

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Security_AllocateGenericKeySlot(NEXUS_KeySlotHandle * pKeyHandle)
{
    NEXUS_KeySlotHandle pHandle;

    BDBG_MSG(("Allocating Generic keyslot"));

    BDBG_ASSERT(pKeyHandle);
    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { *pKeyHandle = NULL; return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); }

    *pKeyHandle = pHandle;

    pHandle->cryptoEngine = NEXUS_SecurityEngine_eGeneric;

    /* Generic key slot are just placeolders for drm context passed to the playpump. They can't be used
       to setup the BSP. No need to set the power management core state. */
    return NEXUS_SUCCESS;
}

#if HSM_IS_ASKM

void NEXUS_Security_GetDefaultAlgorithmSettings(
        NEXUS_SecurityAlgorithmSettings * pSettings
)
{

    if (pSettings)
    {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
        pSettings->algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
        pSettings->algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
        pSettings->terminationMode = NEXUS_SecurityTerminationMode_eClear;
        pSettings->aesCounterSize = NEXUS_SecurityAesCounterSize_e32Bits;
        pSettings->dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
        pSettings->operation = NEXUS_SecurityOperation_ePassThrough;
        pSettings->keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
        pSettings->testKey2Select = false;
        pSettings->bRestrictEnable = true;
        pSettings->bGlobalEnable = true;
        pSettings->bScAtscMode = false;
        pSettings->bAtscModEnable = false;
        pSettings->bGlobalDropPktEnable = false;
        pSettings->bRestrictDropPktEnable = false;
        pSettings->bGlobalRegionOverwrite = false;
        pSettings->bEncryptBeforeRave = false;
        pSettings->enableExtIv = false;
        pSettings->enableExtKey = false;
        pSettings->mscBitSelect = false;
        pSettings->bDisallowGG = false;
        pSettings->bDisallowGR = false;
        pSettings->bDisallowRG = false;
        pSettings->bDisallowRR = false;

        /* ASKM default virtual keyladder configuration -- this is suitable for apps which are not using
         * these features and are built for a generic part.  It is expected that the app will override
         * these values if actually using ASKM features.
         *
         * Added to NEXUS_Security_GetDefaultAlgorithmSettings to allow older apps to work on
         * newer parts without modification.
         */
        pSettings->ivMode = NEXUS_SecurityIVMode_eRegular;
        pSettings->solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
        pSettings->caVendorID = 0x1234;
        pSettings->askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_4;
        pSettings->otpId = NEXUS_SecurityOtpId_eOtpVal;
        pSettings->testKey2Select = 0;

    }
}

static BCMD_XptM2MSecCryptoAlg_e NEXUS_Security_MapNexusAlgorithmToHsm(NEXUS_SecurityAlgorithm algorithm)
{
    BCMD_XptM2MSecCryptoAlg_e rvAlgorithm = algorithm;

    switch (algorithm)
    {
    case NEXUS_SecurityAlgorithm_eMulti2:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eMulti2;
        break;
    case NEXUS_SecurityAlgorithm_eDes:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eDes;
        break;
    case NEXUS_SecurityAlgorithm_e3DesAba:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_e3DesAba;
        break;
    case NEXUS_SecurityAlgorithm_e3DesAbc:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_e3DesAbc;
        break;
    case NEXUS_SecurityAlgorithm_eAes:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAes128;
        break;
    case NEXUS_SecurityAlgorithm_eAes192:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAes192;
        break;
    case NEXUS_SecurityAlgorithm_eC2:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eC2;
        break;
    case NEXUS_SecurityAlgorithm_eCss:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eCss;
        break;
    case NEXUS_SecurityAlgorithm_eM6Ke:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eM6KE;
        break;
    case NEXUS_SecurityAlgorithm_eM6:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eM6;
        break;
    case NEXUS_SecurityAlgorithm_eWMDrmPd:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eWMDrmPd;
        break;
#if HSM_IS_ASKM_40NM
    case NEXUS_SecurityAlgorithm_eDvbCsa3:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eDVBCSA3;
        break;
    case NEXUS_SecurityAlgorithm_eAesCounter:
#if HSM_IS_ASKM_40NM_ZEUS_2_5
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAesCounter0;
#else
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eASF;
#endif
        break;
    case NEXUS_SecurityAlgorithm_eMSMultiSwapMac:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eMSMULTISWAPMAC;
        break;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
    case NEXUS_SecurityAlgorithm_eAsa:
        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eASA;
        break;
#endif
#endif
    /* The _eReservedX values should pass the literal value X into HSM, 
     * allowing direct control of custom modes
     * Macro trickery to avoid copy/paste errors */
#define NEXUS_REMAP_RESERVED(VAL) \
    case NEXUS_SecurityAlgorithm_eReserved##VAL : \
        rvAlgorithm = (BCMD_XptM2MSecCryptoAlg_e) VAL ; \
        break;
        NEXUS_REMAP_RESERVED(0)
        NEXUS_REMAP_RESERVED(1)
        NEXUS_REMAP_RESERVED(2)
        NEXUS_REMAP_RESERVED(3)
        NEXUS_REMAP_RESERVED(4)
        NEXUS_REMAP_RESERVED(5)
        NEXUS_REMAP_RESERVED(6)
        NEXUS_REMAP_RESERVED(7)
        NEXUS_REMAP_RESERVED(8)
#undef NEXUS_REMAP_RESERVED
    default:
        break;
    }

    return rvAlgorithm;
}

static void NEXUS_Security_GetHsmAlgorithmKeySetting(
        NEXUS_KeySlotHandle keyHandle,
        const NEXUS_SecurityAlgorithmSettings *pSettings,
        BCMD_XptM2MSecCryptoAlg_e *pCryptAlg,
        BCMD_CipherModeSelect_e *pCipherMode,
        BCMD_TerminationMode_e *pTerminationMode
)
{

    /* fixups/remapping needed since the enums do not always agree across all platforms */
    *pCryptAlg = NEXUS_Security_MapNexusAlgorithmToHsm(pSettings->algorithm);
    *pCipherMode = (BCMD_CipherModeSelect_e)pSettings->algorithmVar;
    *pTerminationMode = (BCMD_TerminationMode_e)pSettings->terminationMode;

    if ( keyHandle->cryptoEngine != NEXUS_SecurityEngine_eM2m )
    {
        if (pSettings->algorithm == NEXUS_SecurityAlgorithm_eDvb ||
            pSettings->algorithm == NEXUS_SecurityAlgorithm_eDvbCsa3)
        {
            /* overload cipherMode for CA/CP */
            if (pSettings->dvbScramLevel == NEXUS_SecurityDvbScrambleLevel_eTs)
                *pCipherMode = (BCMD_CipherModeSelect_e)NEXUS_SecurityAlgorithmVariant_eXpt;
            else
                *pCipherMode = (BCMD_CipherModeSelect_e)NEXUS_SecurityAlgorithmVariant_ePes;

        }

    }
    else /* Security Algorithm Key setting for M2M */
    {
        if (pSettings->algorithm == NEXUS_SecurityAlgorithm_eAes ||
                pSettings->algorithm == NEXUS_SecurityAlgorithm_eAes192)
        {
            if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eCounter)
            {
                /* overload termination mode for M2M */
                *pTerminationMode = pSettings->aesCounterSize;
            }
        }
    }

}

#else

void NEXUS_Security_GetDefaultAlgorithmSettings(NEXUS_SecurityAlgorithmSettings * pSettings)
{
    if (pSettings) {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
        pSettings->algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
        pSettings->algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
        pSettings->terminationMode = NEXUS_SecurityTerminationMode_eCipherStealing;
        pSettings->aesCounterSize = NEXUS_SecurityAesCounterSize_e32Bits;
        pSettings->dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
        pSettings->keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
        pSettings->operation = NEXUS_SecurityOperation_ePassThrough;
        pSettings->bRestrictEnable = true;
        pSettings->bGlobalEnable = true;
        pSettings->bScAtscMode = false;
        pSettings->bAtscModEnable = false;
        pSettings->bGlobalDropPktEnable = false;
        pSettings->bRestrictDropPktEnable = false;
        pSettings->bGlobalRegionOverwrite = false;
        pSettings->enableExtIv = false;
        pSettings->enableExtKey = false;
        pSettings->mscBitSelect = false;
        pSettings->bScPolarityEnable = false;
        pSettings->bSynEnable = false;
        pSettings->bCPDDisable = false;
        pSettings->bCPSDisable = false;
    }
}

static bool NEXUS_Security_GetHsmCaCpRmxAlgorithmKeySetting(const NEXUS_SecurityAlgorithmSettings * pSettings, unsigned int * pCryptAlg, BHSM_ResidueMode_e * pResidualMode)
{
    switch (pSettings->algorithm) {
    case NEXUS_SecurityAlgorithm_eDvb: {
        *pCryptAlg = BCMD_XptSecCryptoAlg_eDvb;
        if (pSettings->dvbScramLevel == NEXUS_SecurityDvbScrambleLevel_eTs)
            *pResidualMode = BHSM_DVBScrambleLevel_eTS;
        else
            *pResidualMode = BHSM_DVBScrambleLevel_ePes;
    }
        return true;

    case NEXUS_SecurityAlgorithm_eDes: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_XptSecCryptoAlg_eDesEcb;
        else
            *pCryptAlg = BCMD_XptSecCryptoAlg_eDesCbc;
    }
        break;
    case NEXUS_SecurityAlgorithm_e3DesAba: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_XptSecCryptoAlg_e3DesAbaEcb;
        else
            *pCryptAlg = BCMD_XptSecCryptoAlg_e3DesAbaCbc;
    }
        break;
#if SUPPORT_NON_DTV_CRYPTO || SUPPORT_AES_FOR_DTV
    case NEXUS_SecurityAlgorithm_eAes: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_XptSecCryptoAlg_eAesEcb;
        else
            *pCryptAlg = BCMD_XptSecCryptoAlg_eAesCbc;
    }
        break;
#endif /* SUPPORT_NON_DTV_CRYPTO */
    case NEXUS_SecurityAlgorithm_eMulti2: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_XptSecCryptoAlg_eMulti2Ecb;
        else
            *pCryptAlg = BCMD_XptSecCryptoAlg_eMulti2Cbc;
        break;
    }
    default:
#if SUPPORT_NON_DTV_CRYPTO
        *pCryptAlg = BCMD_XptSecCryptoAlg_eAesEcb;
#else
        * pCryptAlg = BCMD_XptSecCryptoAlg_eDesEcb;
#endif /* SUPPORT_NON_DTV_CRYPTO */
        BDBG_WRN(("Unrecognized or unsupported algorithm"));
        BERR_TRACE(BERR_INVALID_PARAMETER);
        break;
    }

    return false;
}

static bool NEXUS_Security_GetHsmM2MAlgorithmKeySetting(const NEXUS_SecurityAlgorithmSettings *pSettings, unsigned int * pCryptAlg, BHSM_ResidueMode_e * pResidualMode)
{
    switch (pSettings->algorithm) {
    case NEXUS_SecurityAlgorithm_e3DesAba: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_e3DesAbaEcb;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_e3DesAbaCbc;
    }
        break;
    case NEXUS_SecurityAlgorithm_e3DesAbc: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_e3DesAbcEcb;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_e3DesAbcCbc;
    }
        break;
    case NEXUS_SecurityAlgorithm_eDes: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eDesEcb;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eDesCbc;
    }
        break;
#if SUPPORT_NON_DTV_CRYPTO
    case NEXUS_SecurityAlgorithm_eC2: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eC2Ecb;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eC2Cbc;
    }
        break;
#endif /* SUPPORT_NON_DTV_CRYPTO */
#if SUPPORT_NON_DTV_CRYPTO
    case NEXUS_SecurityAlgorithm_eCss:
        *pCryptAlg = BCMD_M2MSecCryptoAlg_eCss;
        break;
#endif /* SUPPORT_NON_DTV_CRYPTO */
    case NEXUS_SecurityAlgorithm_eM6Ke:
        *pCryptAlg = BCMD_M2MSecCryptoAlg_eM6KE;
        break;
    case NEXUS_SecurityAlgorithm_eM6: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eM6Ecb;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eM6Cbc;
    }
        break;
    case NEXUS_SecurityAlgorithm_eAes: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes128Ecb;
        else if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eCbc)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes128Cbc;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAesCounter;
    }
        break;
    case NEXUS_SecurityAlgorithm_eAes192: {
        if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eEcb)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes192Ecb;
        else if (pSettings->algorithmVar == NEXUS_SecurityAlgorithmVariant_eCbc)
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes192Cbc;
        else
            *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes192Counter;
    }
        break;
#if MSDRM_PD_SUPPORT
        case NEXUS_SecurityAlgorithm_eWMDrmPd:
        * pCryptAlg = BCMD_M2MSecCryptoAlg_eWMDrmPd;
        break;
        case NEXUS_SecurityAlgorithm_eRc4:
        * pCryptAlg = BCMD_M2MSecCryptoAlg_eRc4;
        break;
#endif
    default:
        *pCryptAlg = BCMD_M2MSecCryptoAlg_eAes128Ecb;
        break;
    }

    if ( (*pCryptAlg == BCMD_M2MSecCryptoAlg_eAesCounter) || (*pCryptAlg == BCMD_M2MSecCryptoAlg_eAes192Counter)) {
        *pResidualMode = (BHSM_ResidueMode_e) pSettings->aesCounterSize;
        return true;
    }
    return false;
}

static void NEXUS_Security_GetHsmAlgorithmKeySetting(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityAlgorithmSettings *pSettings, unsigned int * pCryptAlg,
        BHSM_ResidueMode_e * pResidualMode)
{
    if (keyHandle->cryptoEngine != NEXUS_SecurityEngine_eM2m) {
        if (NEXUS_Security_GetHsmCaCpRmxAlgorithmKeySetting(pSettings, pCryptAlg, pResidualMode))
            return;
    } else {
        if (NEXUS_Security_GetHsmM2MAlgorithmKeySetting(pSettings, pCryptAlg, pResidualMode))
            return;
    }

    switch (pSettings->terminationMode) {
    case NEXUS_SecurityTerminationMode_eClear:
        *pResidualMode = BHSM_ResidueMode_eUnscrambled;
        break;
    case NEXUS_SecurityTerminationMode_eBlock:
        *pResidualMode = BHSM_ResidueMode_eResidueBlock;
        break;
    case NEXUS_SecurityTerminationMode_eCipherStealing:
        *pResidualMode = BHSM_ResidueMode_eCipherTextStealing;
        break;
#if 0 /*RRLee BHSM_ResidueMode_eCipherStealingComcast is not defined???*/
        case NEXUS_SecurityTerminationMode_eCipherStealingComcast:
        * pResidualMode = BHSM_ResidueMode_eCipherStealingComcast;
        break;
#endif
    default:
        *pResidualMode = BHSM_ResidueMode_eUnscrambled;
        break;
    }

}
#endif

static NEXUS_Error NEXUS_Security_GetHsmDestBlkType(NEXUS_Security_P_KeySlotListNode *p, NEXUS_SecurityAlgorithmConfigDestination dest, BCMD_KeyDestBlockType_e *pType)
{
    switch (p->handle->cryptoEngine) {
    case NEXUS_SecurityEngine_eCa:
        *pType = BCMD_KeyDestBlockType_eCA;
        break;
    case NEXUS_SecurityEngine_eM2m:
        *pType = BCMD_KeyDestBlockType_eMem2Mem;
        break;
    case NEXUS_SecurityEngine_eCaCp:
#if HSM_IS_ASKM
        if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCa)
            *pType = BCMD_KeyDestBlockType_eCA;
        else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd)
            *pType = BCMD_KeyDestBlockType_eCPDescrambler;
        else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCps)
            *pType = BCMD_KeyDestBlockType_eCPScrambler;
#else
#if 0
        /* If configuring an AES128 key for CPS is erroring, try this code instead: */
        if (p->algorithm == NEXUS_SecurityAlgorithm_eAes128) {
            /* AES128 requires CPS for scrambling on older chips */
            *pType = (dest==NEXUS_SecurityAlgorithmConfigDestination_eCa) ? BCMD_KeyDestBlockType_eCA : BCMD_KeyDestBlockType_eCPScrambler;
        } else {
            /* Everything else uses Remux on older chips */
            *pType = (dest==NEXUS_SecurityAlgorithmConfigDestination_eCa) ? BCMD_KeyDestBlockType_eCA : BCMD_KeyDestBlockType_eRmx;
        }
#else
        *pType = (dest==NEXUS_SecurityAlgorithmConfigDestination_eCa) ? BCMD_KeyDestBlockType_eCA : BCMD_KeyDestBlockType_eRmx;
#endif
#endif
        break;
    case NEXUS_SecurityEngine_eCp:
#if HSM_IS_ASKM
        if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd)
            *pType = BCMD_KeyDestBlockType_eCPDescrambler;
        else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCps)
            *pType = BCMD_KeyDestBlockType_eCPScrambler;

#else
#if 0
        if (p->algorithm == NEXUS_SecurityAlgorithm_eAes128) {
            /* AES128 requires CPS for scrambling on older chips */
            *pType = BCMD_KeyDestBlockType_eCPScrambler;
        } else {
            /* Everything else uses Remux on older chips */
            *pType = BCMD_KeyDestBlockType_eRmx;
        }
#else
        *pType = BCMD_KeyDestBlockType_eRmx;
#endif
#endif
        break;
    case NEXUS_SecurityEngine_eRmx:
#if HSM_IS_ASKM_40NM
        BDBG_ERR(("Remux is not supported on 40nm HSM"));
        *pType = BCMD_KeyDestBlockType_eCA;
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#else
        *pType = BCMD_KeyDestBlockType_eRmx;
#endif
        break;
    default:
        /* There is no meaningful default, error. */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        break;
    }
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Security_GetHsmDestEntrykType(NEXUS_SecurityKeyType keytype, BCMD_KeyDestEntryType_e *pType)
{
    switch (keytype) {
    case NEXUS_SecurityKeyType_eEven:
        *pType = BCMD_KeyDestEntryType_eEvenKey;
        break;
    case NEXUS_SecurityKeyType_eOdd:
        *pType = BCMD_KeyDestEntryType_eOddKey;
        break;
    case NEXUS_SecurityKeyType_eClear:
#if HSM_IS_ASKM_40NM
        *pType = BCMD_KeyDestEntryType_eReserved2;
#else
        *pType = BCMD_KeyDestEntryType_eReserved0;
#endif
        break;
#if !HSM_IS_ASKM_40NM
    case NEXUS_SecurityKeyType_eIv:
        *pType = BCMD_KeyDestEntryType_eIV;
        break;
#endif
    default:
        *pType = BCMD_KeyDestEntryType_eOddKey;
        break;
    }
    return NEXUS_SUCCESS;
}

#if HSM_IS_ASKM_40NM
static NEXUS_Error NEXUS_Security_GetHsmDestIVType(NEXUS_SecurityKeyIVType keyIVtype, BCMD_KeyDestIVType_e *pType)
{
    switch (keyIVtype) {
    case NEXUS_SecurityKeyIVType_eNoIV:
        *pType = BCMD_KeyDestIVType_eNoIV;
        break;
    case NEXUS_SecurityKeyIVType_eIV:
        *pType = BCMD_KeyDestIVType_eIV;
        break;
    case NEXUS_SecurityKeyIVType_eAesShortIV:
        *pType = BCMD_KeyDestIVType_eAesShortIV;
        break;
    default:
        *pType = BCMD_KeyDestIVType_eNoIV;
        break;
    }
    return NEXUS_SUCCESS;
}

#endif
#ifdef NEXUS_SECURITY_SC_VALUE
static int g_scValues[NEXUS_SecurityAlgorithmScPolarity_eMax] = { 0, 1, 2, 3 };
#endif
static void NEXUS_Security_P_SetScValues(BHSM_ConfigAlgorithmIO_t *pConfigAlgorithmIO, const NEXUS_SecurityAlgorithmSettings *pSettings, NEXUS_SecurityKeyType keyType) {
#if HSM_IS_ASKM_40NM && defined(NEXUS_SECURITY_SC_VALUE)
    int keytypeScValues[NEXUS_SecurityKeyType_eMax] = { 3, 2, 0, 0, 0 };
    if (pSettings->modifyScValue[NEXUS_SecurityPacketType_eGlobal]) {
        pConfigAlgorithmIO->cryptoAlg.caCryptAlg.globalSCVal = g_scValues[pSettings->scValue[NEXUS_SecurityPacketType_eGlobal]];
    } else {
        pConfigAlgorithmIO->cryptoAlg.caCryptAlg.globalSCVal = keytypeScValues[keyType];
    }
    if (pSettings->modifyScValue[NEXUS_SecurityPacketType_eRestricted]) {
        pConfigAlgorithmIO->cryptoAlg.caCryptAlg.restrSCVal = g_scValues[pSettings->scValue[NEXUS_SecurityPacketType_eRestricted]];
    } else {
        pConfigAlgorithmIO->cryptoAlg.caCryptAlg.restrSCVal = keytypeScValues[keyType];
    }
#else
    BSTD_UNUSED(pConfigAlgorithmIO);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(keyType);
#endif
}

NEXUS_Error NEXUS_Security_ConfigAlgorithm(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityAlgorithmSettings *pSettings)
{
    BHSM_ConfigAlgorithmIO_t configAlgorithmIO;
    BCMD_XptSecKeySlot_e keySlotType;
    unsigned int unKeySlotNum = 0;
    bool bConfigOddAndEven = false;
#if !HSM_IS_ASKM_40NM
    bool bConfigClear = false;
#endif
    NEXUS_Error rc = NEXUS_SUCCESS;
    BCMD_KeyDestBlockType_e blockType = BCMD_KeyDestBlockType_eCA;
#if HSM_IS_ASKM
    BHSM_ConfigKeySlotIDDataIO_t configKeySlotIDDataIO;
    BCMD_XptM2MSecCryptoAlg_e cryptAlg;
    BCMD_CipherModeSelect_e cipherMode;
    BCMD_TerminationMode_e terminationMode;
    BCMD_IVSelect_e ivModeSelect;
    BCMD_SolitarySelect_e solitarySelect;
#else
    unsigned int cryptAlg;
    BHSM_ResidueMode_e residualMode = BHSM_ResidueMode_eUnscrambled;
#endif
    NEXUS_SecurityKeySource keySrc;
    bool bAVKeyladder = false;
    NEXUS_Security_P_KeySlotListNode *p;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    if ( !keyHandle || !pSettings)
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);

    p = NEXUS_Security_P_LocateNodeByKeyslot(keyHandle);
    p->algorithm = pSettings->algorithm;

    BKNI_Memset(&configAlgorithmIO, 0, sizeof (configAlgorithmIO));

    keySlotType = keyHandle->keyslotType;
    unKeySlotNum = keyHandle->keySlotNumber;
    keySrc = pSettings->keySource;

#if HSM_IS_ASKM
    NEXUS_Security_GetHsmAlgorithmKeySetting(keyHandle, pSettings,
            &cryptAlg,
            &cipherMode,
            &terminationMode);
    ivModeSelect = pSettings->ivMode;
    solitarySelect = pSettings->solitarySelect;
#else
    NEXUS_Security_GetHsmAlgorithmKeySetting(keyHandle, pSettings, &cryptAlg, &residualMode);
#endif

    rc = NEXUS_Security_GetHsmDestBlkType(p, pSettings->dest, &blockType);
    if (rc) return BERR_TRACE(rc);

    /* coverity[const] */
    switch (blockType) {
    case BCMD_KeyDestBlockType_eCA:
#if HSM_IS_ASKM
    case BCMD_KeyDestBlockType_eCPScrambler:
    case BCMD_KeyDestBlockType_eCPDescrambler:
#endif
        bConfigOddAndEven = (pSettings->keyDestEntryType == NEXUS_SecurityKeyType_eOddAndEven);
        break;
#if !HSM_IS_ASKM_40NM
    case BCMD_KeyDestBlockType_eRmx:
        bConfigOddAndEven = (pSettings->keyDestEntryType == NEXUS_SecurityKeyType_eOddAndEven);
        bConfigClear = true;
        break;
#endif
    default:
        break;
    }

/*
    if (keyHandle->cryptoEngine==NEXUS_SecurityEngine_eCaCp)
#if !HSM_IS_ASKM_40NM
        bConfigClear = true;
#endif
*/
    /* if req. is for AV Keyladder, config only for KeyDestEntryType requested */
    if (keySrc == NEXUS_SecurityKeySource_eAvCPCW || keySrc == NEXUS_SecurityKeySource_eAvCW) {
        bConfigOddAndEven = false;
#if !HSM_IS_ASKM_40NM
        bConfigClear = false;
#endif
        bAVKeyladder = true;
    }

#if !HSM_IS_ASKM_40NM
    if (!bAVKeyladder) /* keep the keySource setting for AV Keyladder */
        configAlgorithmIO.keySource = BCMD_KeyRamBuf_eFirstRam; /*BCMD_KeyRamBuf_eKey5KeyLadder2; */
    else {
#if HSM_IS_ASKM
        configAlgorithmIO.keySource = keySrc;
#else
        if (keySrc == NEXUS_SecurityKeySource_eAvCPCW)
            configAlgorithmIO.keySource = BCMD_KeyRamBuf_eReserved0;
        else
            configAlgorithmIO.keySource = BCMD_KeyRamBuf_eReserved1;
#endif
    }
#endif
    configAlgorithmIO.keyDestBlckType = blockType;
    if (bAVKeyladder) /* keep the KeyDestEntryType as requested for AV Keyladder */
        configAlgorithmIO.keyDestEntryType = NEXUS_Security_MapNexusKeyDestToHsm(keyHandle, pSettings->keyDestEntryType);
    else {
        if (bConfigOddAndEven) {
            configAlgorithmIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
        } else {
            configAlgorithmIO.keyDestEntryType = NEXUS_Security_MapNexusKeyDestToHsm(keyHandle, pSettings->keyDestEntryType);
        }
    }
    configAlgorithmIO.caKeySlotType = keySlotType;
    configAlgorithmIO.unKeySlotNum = unKeySlotNum;
    if (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eM2m) 
    {
        BKNI_Memset(&(configAlgorithmIO.cryptoAlg.m2mCryptAlg), 0, sizeof(configAlgorithmIO.cryptoAlg.m2mCryptAlg));

        configAlgorithmIO.cryptoAlg.m2mCryptAlg.m2mSecAlg = cryptAlg;
#if HSM_IS_ASKM
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.m2mCipherMode = cipherMode;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.TerminationAESCounterKeyMode = terminationMode;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.IVModeSelect = ivModeSelect;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.SolitarySelect = solitarySelect;
#else
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode = residualMode;
#endif
        if (pSettings->operation == NEXUS_SecurityOperation_eEncrypt)
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_eScramble;
        else if (pSettings->operation == NEXUS_SecurityOperation_eDecrypt)
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_eDescramble;
        else
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_ePassThrough;

        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bEnableTimestamp = pSettings->enableTimestamps;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bMscCtrlSel = pSettings->mscBitSelect;

#if HSM_IS_ASKM
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallowGG = pSettings->bDisallowGG;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallowGR = pSettings->bDisallowGR;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallowRG = pSettings->bDisallowRG;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallowRR = pSettings->bDisallowRR;
#endif

        /*set key & IV to external if the algorithm is WMDRMPD*/
        if ((pSettings->algorithm == NEXUS_SecurityAlgorithm_eWMDrmPd) || (pSettings->algorithm == NEXUS_SecurityAlgorithm_eRc4)) {
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtKey = true;
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtIV = true;
        } else {
#ifdef NEXUS_SECURITY_EXT_KEY_IV
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtKey = pSettings->enableExtKey;
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtIV = pSettings->enableExtIv;
#else
            if ( (pSettings->enableExtKey) || (pSettings->enableExtIv)) {
                BDBG_ERR(("\nYou were trying to use external key or IV without turning on BSP_M2M_EXT_KEY_IV_SUPPORT\n" ));
                BDBG_ERR(("\nPlease rebuild nexus with BSP_M2M_EXT_KEY_IV_SUPPORT=ON\n" ));
                return NEXUS_NOT_SUPPORTED;
            }

#endif
        }

    } 
    else 
    {
        BKNI_Memset(&(configAlgorithmIO.cryptoAlg.caCryptAlg), 0, sizeof(configAlgorithmIO.cryptoAlg.caCryptAlg));

        configAlgorithmIO.cryptoAlg.caCryptAlg.caSecAlg = cryptAlg;
#if HSM_IS_ASKM
#if HSM_IS_ASKM_40NM
        configAlgorithmIO.cryptoAlg.caCryptAlg.cipherDVBCSA2Mode = cipherMode;
#else
        configAlgorithmIO.cryptoAlg.caCryptAlg.cipherDVBMode = cipherMode;
#endif
        configAlgorithmIO.cryptoAlg.caCryptAlg.terminationMode = terminationMode;
        configAlgorithmIO.cryptoAlg.caCryptAlg.IVMode = ivModeSelect;
        configAlgorithmIO.cryptoAlg.caCryptAlg.solitaryMode = solitarySelect;

#else
        if (cryptAlg != BCMD_XptSecCryptoAlg_eDvb)
            configAlgorithmIO.cryptoAlg.caCryptAlg.residueMode.residueMode = residualMode;
        else
            configAlgorithmIO.cryptoAlg.caCryptAlg.residueMode.dvbScrambleLevel = residualMode;
#endif
        configAlgorithmIO.cryptoAlg.caCryptAlg.bRestrictEnable = pSettings->bRestrictEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bGlobalEnable = pSettings->bGlobalEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.ucMulti2KeySelect = pSettings->multi2KeySelect;
#if HSM_IS_ASKM_40NM
        configAlgorithmIO.cryptoAlg.caCryptAlg.keyOffset = pSettings->keyOffset;
        configAlgorithmIO.cryptoAlg.caCryptAlg.ivOffset = pSettings->ivOffset;
        configAlgorithmIO.cryptoAlg.caCryptAlg.ucMSCLengthSelect = pSettings->mscLengthSelect;
        configAlgorithmIO.cryptoAlg.caCryptAlg.customerType = (BCMD_XptKeyTableCustomerMode_e)pSettings->customerType;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA2keyCtrl = pSettings->dvbCsa2keyCtrl;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA2ivCtrl = pSettings->dvbCsa2ivCtrl;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA2modEnabled = pSettings->dvbCsa2modEnabled;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA3dvbcsaVar = pSettings->dvbCsa3dvbcsaVar;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA3permutation = pSettings->dvbCsa3permutation;
        configAlgorithmIO.cryptoAlg.caCryptAlg.DVBCSA3modXRC = pSettings->dvbCsa3modXRC;
#ifdef NEXUS_SECURITY_EXT_KEY_IV  
        configAlgorithmIO.cryptoAlg.caCryptAlg.bUseExtKey = pSettings->enableExtKey;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bUseExtIV = pSettings->enableExtIv;
#else
        if ( (pSettings->enableExtKey) || (pSettings->enableExtIv)) 
        {
            BDBG_ERR(("\nYou were trying to use external key or IV without turning on BSP_M2M_EXT_KEY_IV_SUPPORT\n" ));
            BDBG_ERR(("\nPlease rebuild nexus with BSP_M2M_EXT_KEY_IV_SUPPORT=ON\n" ));
            return NEXUS_NOT_SUPPORTED;
        }
#endif

#else
        configAlgorithmIO.cryptoAlg.caCryptAlg.bAtscMod = pSettings->bAtscModEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bAtscScrambleCtrl = pSettings->bScAtscMode;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bGlobalDropPktCtrl = pSettings->bGlobalDropPktEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bRestrictDropPktCtrl = pSettings->bRestrictDropPktEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bGlobalRegOverwrite = pSettings->bGlobalRegionOverwrite;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bRestrictScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eRestricted];
        configAlgorithmIO.cryptoAlg.caCryptAlg.bGlobalScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eGlobal];
#endif

#if HSM_IS_ASKM
        configAlgorithmIO.cryptoAlg.caCryptAlg.bEncryptBeforeRave = pSettings->bEncryptBeforeRave;
#else
        configAlgorithmIO.cryptoAlg.caCryptAlg.bEncScPolarity = pSettings->bScPolarityEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bSynEnable = pSettings->bSynEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bCPDDisable = pSettings->bCPDDisable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bCPSDisable = pSettings->bCPSDisable;
#endif

#if HSM_IS_ASKM_40NM_ZEUS_2_0
        configAlgorithmIO.cryptoAlg.caCryptAlg.bDropRregionPackets = pSettings->bRestrictSourceDropPktEnable;
        configAlgorithmIO.cryptoAlg.caCryptAlg.bGpipePackets2Rregion = pSettings->bRoutePipeToRestrictedRegion[NEXUS_SecurityPacketType_eGlobal];
        configAlgorithmIO.cryptoAlg.caCryptAlg.bRpipePackets2Rregion = pSettings->bRoutePipeToRestrictedRegion[NEXUS_SecurityPacketType_eRestricted];
#endif
#ifdef NEXUS_SECURITY_SC_VALUE
#if !HSM_IS_ASKM_40NM
        configAlgorithmIO.cryptoAlg.caCryptAlg.uScValue = g_scValues[pSettings->scValue[NEXUS_SecurityPacketType_eGlobal]];
        if (pSettings->scValue[NEXUS_SecurityPacketType_eRestricted] != NEXUS_SecurityAlgorithmScPolarity_eClear) {
            BDBG_WRN(("SC polarity cannot be differentiated by packet type on this chip.  Ignoring the attempt to set a non-global packet type."));
        }
#endif
#else
        if (pSettings->modifyScValue[NEXUS_SecurityPacketType_eGlobal] || pSettings->modifyScValue[NEXUS_SecurityPacketType_eRestricted]) {
            BDBG_ERR(("You were trying to set SC value without turning on BSP_SC_VALUE_SUPPORT"));
            BDBG_ERR(("Please rebuild nexus with BSP_SC_VALUE_SUPPORT=ON" ));
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
#endif
    }

#if HSM_IS_ASKM
    /* Call BHSM_configKeySlotIDData() to set up ID part of  configuration odd key */
    configKeySlotIDDataIO.keyDestBlckType = blockType;
    if (bAVKeyladder) /* keep the KeyDestEntryType as requested for AV Keyladder */
        configKeySlotIDDataIO.keyDestEntryType = NEXUS_Security_MapNexusKeyDestToHsm(keyHandle, pSettings->keyDestEntryType);
    else {
        if (bConfigOddAndEven) {
            configKeySlotIDDataIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
        } else {
            configKeySlotIDDataIO.keyDestEntryType = NEXUS_Security_MapNexusKeyDestToHsm(keyHandle, pSettings->keyDestEntryType);
        }
    }
#if HSM_IS_ASKM_40NM
    configKeySlotIDDataIO.keyDestIVType = BCMD_KeyDestIVType_eNoIV;
#endif

    configKeySlotIDDataIO.unKeySlotNum = unKeySlotNum;
    configKeySlotIDDataIO.caKeySlotType = keySlotType;
    configKeySlotIDDataIO.CAVendorID = pSettings->caVendorID;
    configKeySlotIDDataIO.STBOwnerIDSelect = pSettings->otpId;
    configKeySlotIDDataIO.ModuleID = pSettings->askmModuleID;
#if HSM_IS_ASKM_40NM
    configKeySlotIDDataIO.key2Select = pSettings->key2Select;
#else
    configKeySlotIDDataIO.TestKey2Select = pSettings->testKey2Select;
#endif

    rc = BHSM_ConfigKeySlotIDData(g_security.hsm, &configKeySlotIDDataIO);
    if (rc)
    {
        return BERR_TRACE(MAKE_HSM_ERR(rc));
    }

#endif
    if (keyHandle->cryptoEngine != NEXUS_SecurityEngine_eM2m) 
    {
        NEXUS_Security_P_SetScValues(&configAlgorithmIO, pSettings, bConfigOddAndEven ? NEXUS_SecurityKeyType_eOdd : pSettings->keyDestEntryType );
    }
    rc = BHSM_ConfigAlgorithm(g_security.hsm, &configAlgorithmIO);
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }

#if HSM_IS_ASKM

#ifdef NEXUS_SECURITY_EXT_KEY_IV
    /* We must send a dummy route key command for algorithm setting to take effect on 7420 family chips */
    if (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eM2m)
    {
        if ( (configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtKey == true) ||
                (configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtIV == true))
        {
            BHSM_LoadRouteUserKeyIO_t loadRouteUserKeyIO;
            BKNI_Memset(&loadRouteUserKeyIO, 0, sizeof(loadRouteUserKeyIO));
            loadRouteUserKeyIO.bIsRouteKeyRequired = true;
            loadRouteUserKeyIO.keyDestBlckType = BCMD_KeyDestBlockType_eMem2Mem;
#if !HSM_IS_ASKM_40NM
            loadRouteUserKeyIO.keySource = BCMD_KeyRamBuf_eFirstRam;
#endif
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e128;
            loadRouteUserKeyIO.bIsRouteKeyRequired = true;
            loadRouteUserKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
            loadRouteUserKeyIO.caKeySlotType = keySlotType;
            loadRouteUserKeyIO.unKeySlotNum = unKeySlotNum;
            loadRouteUserKeyIO.keyMode= BCMD_KeyMode_eRegular;
            rc = BHSM_LoadRouteUserKey (g_security.hsm, &loadRouteUserKeyIO);
            if (rc)
            {
                BDBG_ERR(("External Key/IV may not be enabled by OTP\n"));
                return BERR_TRACE(MAKE_HSM_ERR(rc));
            }
        }
    }

#endif
#endif

    if (bConfigOddAndEven) {
#if HSM_IS_ASKM
        /* Call BHSM_configKeySlotIDData() to set up ID part of  configuration odd key */
        configKeySlotIDDataIO.keyDestEntryType = BCMD_KeyDestEntryType_eEvenKey;
        rc = BHSM_ConfigKeySlotIDData(g_security.hsm, &configKeySlotIDDataIO);
        if (rc)
        {
            return BERR_TRACE(MAKE_HSM_ERR(rc));
        }

#endif
        configAlgorithmIO.keyDestEntryType = BCMD_KeyDestEntryType_eEvenKey;
        if (keyHandle->cryptoEngine != NEXUS_SecurityEngine_eM2m) 
        {
            NEXUS_Security_P_SetScValues(&configAlgorithmIO, pSettings, NEXUS_SecurityKeyType_eEven );
        }
        rc = BHSM_ConfigAlgorithm(g_security.hsm, &configAlgorithmIO);
        if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }
    }

#if !HSM_IS_ASKM_40NM
    if (bConfigClear) {
#if HSM_IS_ASKM
        /* Call BHSM_configKeySlotIDData() to set up ID part of  configuration odd key */
#if HSM_IS_ASKM_40NM
        configKeySlotIDDataIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved2;
#else
        configKeySlotIDDataIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved0;
#endif
        rc = BHSM_ConfigKeySlotIDData(g_security.hsm, &configKeySlotIDDataIO);
        if (rc)
        {
            return BERR_TRACE(MAKE_HSM_ERR(rc));
        }

        /* If destination block is CA and 65-nm platform, we need to alter mode word for clear key slot */
        /* to configure the SC bit modification for CaCp and send out a dummy LRUK command.  This */
        /* works only for 65-nm BSECK 2.0 and later.   */

        if ( blockType == BCMD_KeyDestBlockType_eCA ) 
        {
            configAlgorithmIO.cryptoAlg.caCryptAlg.caSecAlg = BCMD_XptM2MSecCryptoAlg_eDes;
            configAlgorithmIO.cryptoAlg.caCryptAlg.IVMode = BCMD_IVSelect_eRegular;
#if HSM_IS_ASKM_40NM
            configAlgorithmIO.cryptoAlg.caCryptAlg.cipherDVBCSA2Mode = BCMD_CipherModeSelect_eECB;
            configAlgorithmIO.cryptoAlg.caCryptAlg.terminationMode = BCMD_TerminationMode_eCLEAR;
            configAlgorithmIO.cryptoAlg.caCryptAlg.solitaryMode = BCMD_SolitarySelect_eCLEAR;
#else
            configAlgorithmIO.cryptoAlg.caCryptAlg.cipherDVBMode = BCMD_CipherModeSelect_eECB;
            configAlgorithmIO.cryptoAlg.caCryptAlg.terminationMode = BCMD_TerminationMode_eClear;
            configAlgorithmIO.cryptoAlg.caCryptAlg.solitaryMode = BCMD_SolitarySelect_eClear;
#endif
            /* The following 2 settings are needed so that FW won't reject the key routing command */
            /* associated with this clear key slot for 65-nm platforms */
#if !HSM_IS_ASKM_40NM
            configAlgorithmIO.cryptoAlg.caCryptAlg.bRestrictEnable = false;
            configAlgorithmIO.cryptoAlg.caCryptAlg.bGlobalEnable = false;
#endif
        }

#endif

#if HSM_IS_ASKM_40NM
        configAlgorithmIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved2;
#else
        configAlgorithmIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved0;
#endif
        NEXUS_Security_P_SetScValues(&configAlgorithmIO, pSettings, NEXUS_SecurityKeyType_eClear );
        rc = BHSM_ConfigAlgorithm(g_security.hsm, &configAlgorithmIO);
        if (rc) 
        { 
            return BERR_TRACE(MAKE_HSM_ERR(rc)); 
        }
#if HSM_IS_ASKM
        /* Must call load key for algorithm setting to take effect */
        if ( blockType==BCMD_KeyDestBlockType_eCA )
        {
            BHSM_LoadRouteUserKeyIO_t loadRouteUserKeyIO;
            BKNI_Memset(&loadRouteUserKeyIO, 0, sizeof(loadRouteUserKeyIO));
#if !HSM_IS_ASKM_40NM
            loadRouteUserKeyIO.keySource = BCMD_KeyRamBuf_eFirstRam;
#endif
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e128;
            loadRouteUserKeyIO.bIsRouteKeyRequired = true;
#if HSM_IS_ASKM_40NM
            loadRouteUserKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved2;
            loadRouteUserKeyIO.keyDestIVType    = BCMD_KeyDestIVType_eNoIV;
#else
            loadRouteUserKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eReserved0;
#endif
            loadRouteUserKeyIO.caKeySlotType = keySlotType;
            loadRouteUserKeyIO.unKeySlotNum = unKeySlotNum;
            loadRouteUserKeyIO.keyMode= BCMD_KeyMode_eRegular;
            loadRouteUserKeyIO.keyDestBlckType = blockType;
            rc = BHSM_LoadRouteUserKey (g_security.hsm, &loadRouteUserKeyIO);
        if (rc) 
        { 
                BDBG_ERR(("Configure Clear key failed.  You may need to update new versions of BSECK"));
                /*return BERR_TRACE(MAKE_HSM_ERR(rc));*/
                rc = NEXUS_SUCCESS; /* Ignore error for now */
            }
        }
#endif

    }
#endif

    return rc;
}

void NEXUS_Security_GetDefaultClearKey(NEXUS_SecurityClearKey *pClearKey)
{
    BKNI_Memset(pClearKey, 0, sizeof(*pClearKey));
}

NEXUS_Error NEXUS_Security_LoadClearKey(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityClearKey * pClearKey)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BCMD_KeyDestBlockType_e   blockType;
    BCMD_KeyDestEntryType_e   entryType = BCMD_KeyDestEntryType_eOddKey;
    BHSM_LoadRouteUserKeyIO_t loadRouteUserKeyIO;
#if HSM_IS_ASKM_40NM
    BCMD_KeyDestIVType_e      ivType;
#endif
    NEXUS_Security_P_KeySlotListNode *p;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
#if HSM_IS_ASKM_40NM
    blockType = BCMD_KeyDestBlockType_eCA;
#else
    blockType = BCMD_KeyDestBlockType_eRmx;
#endif

    p = NEXUS_Security_P_LocateNodeByKeyslot(keyHandle);
    NEXUS_Security_GetHsmDestBlkType(p, pClearKey->dest, &blockType);
    NEXUS_Security_GetHsmDestEntrykType(pClearKey->keyEntryType, &entryType);
#if HSM_IS_ASKM_40NM
    NEXUS_Security_GetHsmDestIVType(pClearKey->keyIVType, &ivType);
#endif

	BKNI_Memset(&loadRouteUserKeyIO, 0, sizeof(loadRouteUserKeyIO));
    if (pClearKey->keySize) {
#if !HSM_IS_ASKM_40NM
        loadRouteUserKeyIO.keySource = BCMD_KeyRamBuf_eFirstRam;
#endif
        if (pClearKey->keySize==8)
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e64;
        else if (pClearKey->keySize==16)
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e128;
        else if (pClearKey->keySize==24)
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e192;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
        else if (pClearKey->keySize==32)
            loadRouteUserKeyIO.keySize.eKeySize = BCMD_KeySize_e256;
#endif
        else {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        BKNI_Memset(loadRouteUserKeyIO.aucKeyData, 0, sizeof(loadRouteUserKeyIO.aucKeyData));
        BKNI_Memcpy(loadRouteUserKeyIO.aucKeyData, pClearKey->keyData, pClearKey->keySize);
        loadRouteUserKeyIO.bIsRouteKeyRequired = true;
        loadRouteUserKeyIO.keyDestBlckType = blockType;
        loadRouteUserKeyIO.keyDestEntryType = entryType;
        loadRouteUserKeyIO.caKeySlotType = keyHandle->keyslotType;
        loadRouteUserKeyIO.unKeySlotNum = keyHandle->keySlotNumber;
        loadRouteUserKeyIO.keyMode= BCMD_KeyMode_eRegular;
#if HSM_IS_ASKM_40NM
        loadRouteUserKeyIO.keyDestIVType = ivType;
#endif
#if HSM_IS_ASKM_40NM_ZEUS_2_0
#ifdef NEXUS_SECURITY_SC_VALUE
        loadRouteUserKeyIO.GpipeSC01Val = g_scValues[pClearKey->sc01Polarity[NEXUS_SecurityPacketType_eGlobal]];
        loadRouteUserKeyIO.RpipeSC01Val = g_scValues[pClearKey->sc01Polarity[NEXUS_SecurityPacketType_eRestricted]];
#endif
#endif
        rc = BHSM_LoadRouteUserKey(g_security.hsm, &loadRouteUserKeyIO);
        if (rc) {
            return BERR_TRACE(MAKE_HSM_ERR(rc));
        }
    }

    return NEXUS_SUCCESS;
}

NEXUS_KeySlotHandle NEXUS_Security_AllocateKeySlot(const NEXUS_SecurityKeySlotSettings *pSettings)
{
    NEXUS_KeySlotHandle pKeyHandle = NULL;

    if (pSettings) {
        switch (pSettings->keySlotEngine) {
        case NEXUS_SecurityEngine_eM2m:
            BDBG_MSG(("Allocating M2M keyslot"));
            if (NEXUS_Security_AllocateM2mKeySlot(&pKeyHandle))
                return NULL;
            break;
        case NEXUS_SecurityEngine_eRmx:
        case NEXUS_SecurityEngine_eCaCp:
            BDBG_MSG(("Allocating RMX/CaCp keyslot"));
            if (NEXUS_Security_AllocateKeySlotForType(&pKeyHandle,pSettings->keySlotEngine,NEXUS_Security_MapNexusKeySlotTypeToHsm(pSettings->keySlotType,pSettings->keySlotEngine)))
                return NULL;
            break;
        case NEXUS_SecurityEngine_eCa:
            BDBG_MSG(("Allocating CA (or other) keyslot"));
            if (NEXUS_Security_AllocateKeySlotForType(&pKeyHandle,pSettings->keySlotEngine,NEXUS_Security_MapNexusKeySlotTypeToHsm(pSettings->keySlotType,pSettings->keySlotEngine)))
                return NULL;
            break;
        case NEXUS_SecurityEngine_eGeneric:
            BDBG_MSG(("Allocating Generic keyslot"));
            if (NEXUS_Security_AllocateGenericKeySlot(&pKeyHandle))
                return NULL;
            break;
        default:
            BDBG_WRN(("Unsupported key type"));
            return NULL;
        }
    }
    {
        NEXUS_Security_P_KeySlotListNode *node;
        node = BKNI_Malloc(sizeof(*node));
        if (node) {
            BKNI_Memset(node,0,sizeof(*node));
            node->handle = pKeyHandle;
            BLST_S_INIT(&node->pidchannels);
            BLST_S_INSERT_HEAD(&g_security.keyslots, node, next);
        } else {
            BDBG_WRN(("Unable to track handle %p",pKeyHandle));
        }
    }
    NEXUS_SECURITY_DUMP_KEYSLOTS;
    return pKeyHandle;
}

void NEXUS_Security_FreeKeySlot(NEXUS_KeySlotHandle keyHandle)
{
    BERR_Code rc = 0;

    NEXUS_OBJECT_ASSERT(NEXUS_KeySlot, keyHandle);
    switch (keyHandle->cryptoEngine)
    {
        case NEXUS_SecurityEngine_eCa:
        case NEXUS_SecurityEngine_eCaCp:
            BDBG_MSG(("Freeing CA keyslot (%d)", keyHandle->keySlotNumber));
            rc = BHSM_FreeCAKeySlot(g_security.hsm, keyHandle->pidChannel, BHSM_PidChannelType_ePrimary, keyHandle->keyslotType, keyHandle->keySlotNumber);
            if (rc) BDBG_ERR(("Error (%d) freeing CA keyslot", rc));
            break;
        case NEXUS_SecurityEngine_eM2m:
            BDBG_MSG(("Freeing m2m keyslot (%d)", keyHandle->keySlotNumber));
            rc = BHSM_FreeM2MKeySlot(g_security.hsm, keyHandle->keySlotNumber);
            if (rc) BDBG_ERR(("Error (%d) freeing M2M keyslot", rc));
            break;
        case NEXUS_SecurityEngine_eCp:
            BDBG_MSG(("Freeing cp keyslot"));
            BDBG_WRN(("CP keyslots are not currently supported"));
            rc = NEXUS_INVALID_PARAMETER; /* set an error so we don't decrease the power management refcount */
            break;
        case NEXUS_SecurityEngine_eRmx:
            BDBG_MSG(("Freeing rmx keyslot"));
            rc = BHSM_FreeCAKeySlot(g_security.hsm, keyHandle->pidChannel, BHSM_PidChannelType_ePrimary, keyHandle->keyslotType, keyHandle->keySlotNumber);
            if (rc) BDBG_ERR(("Error (%d) freeing CA keyslot", rc));
            break;
        case NEXUS_SecurityEngine_eGeneric:
        default:
            break;
    }
    if (!rc && (keyHandle->cryptoEngine != NEXUS_SecurityEngine_eGeneric))
        NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, false);

    {
        NEXUS_Security_P_KeySlotListNode *p;
        for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
            if (p->handle == keyHandle) {
                NEXUS_Security_P_PidChannelListNode *q;
                while ((q=BLST_S_FIRST(&p->pidchannels))) {
                    BDBG_MSG(("%s: removing pidchannel %d from keyslot %p",__FUNCTION__,q->pidchannel,p->handle));
                    NEXUS_Security_RemovePidChannelFromKeySlot(p->handle, q->pidchannel);
                }
                BLST_S_REMOVE(&g_security.keyslots,p,NEXUS_Security_P_KeySlotListNode,next);
                BKNI_Free(p);
                break;
            }
        }
    }

    NEXUS_SECURITY_DUMP_KEYSLOTS;

    NEXUS_P_Core_FreeKeySlotHandle(keyHandle);
}

void NEXUS_Security_GetDefaultMulti2Settings(NEXUS_SecurityMulti2Settings *pSettings /* [out] */ )
{
    if (pSettings) {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    }
}

NEXUS_Error NEXUS_Security_ConfigMulti2(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityMulti2Settings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHSM_ConfigMulti2IO_t config_multi2;

    BSTD_UNUSED(keyHandle);

    BKNI_Memset(&config_multi2, 0, sizeof(config_multi2));
    config_multi2.ucMulti2RndCnt = pSettings->multi2Rounds;
    BKNI_Memcpy(config_multi2.aucMulti2SysKey, pSettings->multi2SysKey, 32);
    config_multi2.ucSysKeyDest = pSettings->multi2KeySelect;
    BHSM_ConfigMulti2(g_security.hsm, &config_multi2);
    if (config_multi2.unStatus != 0) {
        BDBG_ERR(("NEXUS_Security_ConfigMulti2: Error configuring Multi2 (0x%02x)", config_multi2.unStatus));
        rc = NEXUS_INVALID_PARAMETER;
    }

    return rc;
}

void NEXUS_Security_GetDefaultInvalidateKeySettings(NEXUS_SecurityInvalidateKeySettings *pSettings /* [out] */ )
{
    if (pSettings) {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    }
}

static NEXUS_Error NEXUS_Security_P_InvalidateKey(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityInvalidateKeySettings *pSettings)
{
    BHSM_InvalidateKeyIO_t invalidateKeyIO;
    BERR_Code rc;
    NEXUS_Error nrc;
    NEXUS_Security_P_KeySlotListNode *p;

    BDBG_MSG(("NEXUS_Security_P_InvalidateKey: %p(%d,%d)",keyHandle,keyHandle->keySlotNumber,keyHandle->keyslotType));

    p = NEXUS_Security_P_LocateNodeByKeyslot(keyHandle);
    if (p) {
        nrc = NEXUS_Security_P_GetInvalidateKeyFlag(pSettings->invalidateKeyType, &invalidateKeyIO.invalidKeyType);
        if (nrc) { return BERR_TRACE(nrc); }
        nrc = NEXUS_Security_GetHsmDestBlkType(p, pSettings->keyDestBlckType, &invalidateKeyIO.keyDestBlckType);
        if (nrc) { return BERR_TRACE(nrc); }
        invalidateKeyIO.keyDestEntryType = NEXUS_Security_MapNexusKeyDestToHsm(keyHandle, pSettings->keyDestEntryType);
        invalidateKeyIO.caKeySlotType = (BCMD_XptSecKeySlot_e)keyHandle->keyslotType;
        invalidateKeyIO.unKeySlotNum = keyHandle->keySlotNumber;
#if HSM_IS_ASKM
        invalidateKeyIO.virtualKeyLadderID = (BCMD_VKLID_e)pSettings->virtualKeyLadderID;
        invalidateKeyIO.keyLayer = (BCMD_KeyRamBuf_e)pSettings->keySrc;
#else
        invalidateKeyIO.keySrc = (BCMD_KeyRamBuf_e)pSettings->keySrc;
#endif

        rc = BHSM_InvalidateKey(g_security.hsm, &invalidateKeyIO);
        if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }
    } else {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Security_InvalidateKey(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityInvalidateKeySettings *pSettings)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    BDBG_MSG(("NEXUS_Security_InvalidateKey: %p(%d,%d)",keyHandle,keyHandle->keySlotNumber,keyHandle->keyslotType));

    if (pSettings->keyDestEntryType == NEXUS_SecurityKeyType_eOddAndEven) {
        /* Special case: NEXUS_SecurityKeyType_eOddAndEven means all CA keys
         * Since pSettings is const, we make a local copy and iterate through the possible destinations.
         */
        NEXUS_SecurityInvalidateKeySettings keySettings;
        BKNI_Memcpy(&keySettings, pSettings, sizeof(keySettings));
        keySettings.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
        rc = NEXUS_Security_P_InvalidateKey(keyHandle, &keySettings);
        if (!rc) {
            keySettings.keyDestEntryType = NEXUS_SecurityKeyType_eEven;
            rc = NEXUS_Security_P_InvalidateKey(keyHandle, &keySettings);
        }
        if (!rc && ((keyHandle->cryptoEngine == NEXUS_SecurityEngine_eRmx) || (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCaCp))) {
            keySettings.keyDestEntryType = NEXUS_SecurityKeyType_eClear;
            rc = NEXUS_Security_P_InvalidateKey(keyHandle, &keySettings);
        }
    } else {
        rc = NEXUS_Security_P_InvalidateKey(keyHandle, pSettings);
    }
    if (rc) { return BERR_TRACE(MAKE_HSM_ERR(rc)); }
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SecurityModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BERR_Code rc;
    BSTD_UNUSED(pSettings);

    if (enabled) {
        if (pSettings->mode!=NEXUS_StandbyMode_eDeepSleep) { /* not S3 */
            /* NEXUS_PowerManagement_SetCoreState is called when keyslots are allocated and free'd. 
               in non-S3, this does not have to occur, so we power down as many times as the number of keyslots opened */
            NEXUS_Security_P_KeySlotListNode *p = NULL;
            for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
                if(p->handle->cryptoEngine != NEXUS_SecurityEngine_eGeneric){
                   NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, false);
                }
            }
            
        }
        else {
            /* enforce that apps free keyslots before standby and re-configure them after resume.
               this also takes care of NEXUS_PowerManagement_SetCoreState */
            if (BLST_S_FIRST(&g_security.keyslots)) {
                BDBG_ERR(("Keyslots must be freed before entering S3 standby"));
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }
            NEXUS_Security_P_UninitHsm();
            g_security.hsm = NULL;
        }
        
    }
    else {
        if (g_security.hsm) { /* not S3 */
            NEXUS_Security_P_KeySlotListNode *p = NULL;
            for (p=BLST_S_FIRST(&g_security.keyslots);p!=NULL;p=BLST_S_NEXT(p,next)) {
                if(p->handle->cryptoEngine != NEXUS_SecurityEngine_eGeneric){
                    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);
                }
            }
        }
        else {
            rc = NEXUS_Security_P_InitHsm(&g_security.settings);
            if (rc) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }
        }
    }
    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(enabled);
    return NEXUS_SUCCESS;
#endif
}

/* This function is dangerous and not meant for general use. It is needed for
 test code that needs the HSM handle. It must be extern'd. */
void b_get_hsm(BHSM_Handle *hsm)
{
    *hsm = g_security.hsm;
}
void b_get_reg(BREG_Handle *reg)
{
    *reg = g_pCoreHandles->reg;
}
void b_get_int(BINT_Handle *bint)
{
    *bint = g_pCoreHandles->bint;
}
void b_get_chp(BCHP_Handle *chp)
{
    *chp = g_pCoreHandles->chp;
}
