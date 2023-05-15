/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform.h $
 * $brcm_Revision: Hydra_Software_Devel/70 $
 * $brcm_Date: 10/8/12 4:58p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_platform.h $
 * 
 * Hydra_Software_Devel/70   10/8/12 4:58p davidp
 * SW7445-16: Add support for 7445 platform.
 * 
 * Hydra_Software_Devel/69   6/25/12 4:24p davidp
 * SW7563-3: Add support for 7563 platform.
 * 
 * Hydra_Software_Devel/68   4/24/12 4:10p davidp
 * SW7584-20: Add support for 7584 platform.
 * 
 * Hydra_Software_Devel/67   12/20/11 3:45p davidp
 * SW7360-4: Add 7360 support.
 * 
 * Hydra_Software_Devel/66   12/5/11 2:55p davidp
 * SW7435-8: Add support for 7435 platform.
 * 
 * Hydra_Software_Devel/65   10/10/11 5:33p davidp
 * SW7429-17: Add support for 7429 platform.
 * 
 * Hydra_Software_Devel/64   4/4/11 4:39p davidp
 * SW7425-284: Define XPT Rave Context info symbolic contants.
 * 
 * Hydra_Software_Devel/63   3/21/11 1:53p davidp
 * SWDTV-5972: Add support for 35233 platform.
 * 
 * Hydra_Software_Devel/62   1/24/11 5:34p davidp
 * SWBLURAY-23545: Add support for 7640 platform.
 * 
 * Hydra_Software_Devel/61   12/17/10 9:30a davidp
 * SW7552-11: Add 7552 platform.
 * 
 * Hydra_Software_Devel/60   11/19/10 6:45p davidp
 * SW7344-5: Add support for 7344 and 7346 platforms.
 * 
 * Hydra_Software_Devel/59   11/15/10 2:50p davidp
 * SW35125-20: Add XVD support for 35125.
 * 
 * Hydra_Software_Devel/58   10/21/10 2:58p davidp
 * SW7231-8: Add support for 7231 and 7135 platforms.
 * 
 * Hydra_Software_Devel/57   10/20/10 1:32p davidp
 * SW7425-16: Remove FGT support.
 * 
 * Hydra_Software_Devel/56   9/10/10 3:17p davidp
 * SW7358-11: Add support for AVD Rev K chips, 7358, 7422 and 7425.
 * 
 * Hydra_Software_Devel/55   2/17/10 2:50p davidp
 * SW7468-74: Add support for IP licensing, save chip product revision
 * register address in xvd handle.
 * 
 * Hydra_Software_Devel/54   11/16/09 6:33p davidp
 * SW7468-14: Add support for 7468 and 7408 platforms.
 * 
 * Hydra_Software_Devel/53   9/21/09 2:53p davidp
 * SW35230-3: Add support for 35230 platform.
 * 
 * Hydra_Software_Devel/52   8/31/09 8:43a davidp
 * SW7630-31: Add support for 7630 platform.
 * 
 * Hydra_Software_Devel/51   8/14/09 10:44a pblanco
 * PR27168: Change UD offset to address conversion routine family
 * prototype from void return type to BERR_Code return type.
 * 
 * Hydra_Software_Devel/50   7/23/09 1:44p davidp
 * PR55546: Add 7125 support to xvd.
 * 
 * Hydra_Software_Devel/49   7/17/09 4:28p davidp
 * PR55227: Add support for 7340 and 7342 support.
 * 
 * Hydra_Software_Devel/48   6/4/09 4:05p davidp
 * PR54107: Add support for 35130 platform, create Rev J platform family.
 * 
 * Hydra_Software_Devel/47   4/3/09 5:40p davidp
 * PR53571: Add initial 7635 support.
 * 
 * Hydra_Software_Devel/46   1/30/09 1:15p davidp
 * PR51623: Add support for 7336 chip.
 * 
 * Hydra_Software_Devel/45   12/23/08 1:19p davidp
 * PR50640: Add support for 7405 C0.
 * 
 * Hydra_Software_Devel/44   10/15/08 5:17p davidp
 * PR47748: Add support for 7420 platform
 * 
 * Hydra_Software_Devel/43   8/6/08 12:36p davidp
 * PR43585: Add 7601 platform specific support to XVD.
 * 
 * Hydra_Software_Devel/42   3/7/08 10:38a nilesh
 * PR40349: Update memory tables according to AVD Mosaic API doc v2.19
 * 
 * Hydra_Software_Devel/41   2/20/08 6:48p davidp
 * PR39495: Add support for 3556, clone of 7405 using AVD core rev i0.
 * 
 * Hydra_Software_Devel/40   2/13/08 5:05p nilesh
 * PR39469: Update memory tables according to AVD Mosaic API doc v2.18
 * 
 * Hydra_Software_Devel/39   1/29/08 7:33p davidp
 * PR39098: 3548 support uses AVD core rev i0 version.
 * 
 * Hydra_Software_Devel/38   1/29/08 6:00p davidp
 * PR39098: Remove unused register items from revE0 register structure.
 * 
 * Hydra_Software_Devel/37   1/24/08 11:40a nilesh
 * PR38570: Merge CABAC worklist API changes to mainline
 * 
 * Hydra_Software_Devel/PR38570/1   1/21/08 5:11p nilesh
 * PR38570: Added support for cabac worklist buffer and size parameter to
 * AVD ChannelOpen
 * 
 * Hydra_Software_Devel/36   11/28/07 4:39p pblanco
 * PR36901: Clone 7325 from 7405.
 * 
 * Hydra_Software_Devel/35   11/13/07 5:38p davidp
 * PR36901: Add support for 97335, use BXVD_CHIP families, 97335 is same
 * as 97405.
 * 
 * Hydra_Software_Devel/34   11/13/07 3:09p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/33   11/6/07 5:53p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/32   11/6/07 2:20p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/31   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/30   9/24/07 8:57a pblanco
 * PR35020: Added 7325 platform case.
 * 
 * Hydra_Software_Devel/29   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   8/10/07 1:07p davidp
 * PR29915: Remove unneeded validate macro, use correct parameter for
 * validate_pdefsettings macro.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   7/10/07 7:40p nilesh
 * PR29915: Cleaned up 7405 platform code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   7/3/07 12:08p nilesh
 * PR29915: Added FGT support
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/28   6/11/07 10:56a pblanco
 * PR28215: Changed stripe width value constant to be the same as VDC enum
 * value.
 * 
 * Hydra_Software_Devel/27   5/15/07 1:20p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 * 
 * Hydra_Software_Devel/26   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 * 
 * Hydra_Software_Devel/PR30249/1   5/10/07 2:09p nilesh
 * PR30249: Added support for new stripe_multiple parameter in AVD Init
 * 
 * Hydra_Software_Devel/25   4/24/07 1:44p nilesh
 * PR22766: Added support for platform specific FGT buffer allocation
 * 
 * Hydra_Software_Devel/24   4/19/07 10:01a pblanco
 * PR27645: Added 7405 to platform list.
 * 
 * Hydra_Software_Devel/23   2/27/07 3:16p pblanco
 * PR22867: Changed core include to E1 for 7118
 * 
 * Hydra_Software_Devel/22   2/6/07 9:14a pblanco
 * PR26188: 7400B0 code changes.
 * 
 * Hydra_Software_Devel/21   11/17/06 4:05p davidp
 * PR25671: Add 7403 platform support.
 * 
 * Hydra_Software_Devel/20   10/19/06 2:56p nilesh
 * PR23113: Enforced include order of bxvd.h and bxvd_platform.h to
 * prevent incorrect #defines
 * 
 * Hydra_Software_Devel/19   9/8/06 10:38a nilesh
 * PR23113: Update to use core rev e1
 * 
 * Hydra_Software_Devel/18   8/21/06 3:20p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/17   8/9/06 12:53p nilesh
 * PR23113: Added AVD rev E0 include file for 7440
 * 
 * Hydra_Software_Devel/16   8/9/06 11:26a nilesh
 * PR22673: Added documentation about XVD code restructure
 * 
 * Hydra_Software_Devel/15   8/9/06 11:17a nilesh
 * PR23113: Add framework to support 7440 XVD
 * 
 * Hydra_Software_Devel/14   8/7/06 11:47a nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/13   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/12   7/31/06 7:50p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 * 
 * Hydra_Software_Devel/11   7/27/06 8:14p davidp
 * PR22673: Move chip specific send FW command code to platform files.
 * 
 * Hydra_Software_Devel/10   7/26/06 4:50p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/9   7/24/06 10:58a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/8   7/21/06 12:02p pblanco
 * PR22673: Fixed bug referencing the proper area for userdata memory.
 * 
 * Hydra_Software_Devel/7   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/6   7/20/06 6:33p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/5   7/20/06 2:23p nilesh
 * PR22673: Created BXVD_P_CONTEXT_PLATFORM macro to allow platform-
 * specific fields in BXVD_P_Context struct
 * 
 * Hydra_Software_Devel/4   7/19/06 11:49a pblanco
 * PR22673: Fix code for 7400.
 * 
 * Hydra_Software_Devel/3   7/19/06 11:23a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/2   7/18/06 3:30p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/1   7/18/06 12:10a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:19p nilesh
 * PR22673: Code Restructure
 *
 ***************************************************************************/

/***************************************************************************
XVD Code Stucture

Overview
--------

XVD was restructured in July/Aug 2006 (See PR22673) to better support
multiple chips and revisions using the same code base.  Prior to the
restructure, the code was littered with chip/revision #ifdefs that
made the code difficult to understand and maintain.

The restructure goals, though all very important, were prioritized as
follows:

  1) Readability - the code flow/logic should be easily readable to
     someone who is looking at the code for the first time.  We wanted
     to split the "what" from the "how".  i.e. We wanted the
     high-level functionality to be apparent by hiding the details of
     the actual implementation.  

  2) Debuggability - We wanted to minimize the impact on run-time
     debugging using tools like gdb.  e.g. it should be possible to
     know exactly which function implementation is being executed by
     looking at the function call trace.  The function names should
     not be ambigious.

  3) Maintainability - often times, the chip families share 90%+ of
     the code.  There are subtle differences in things such as
     register names or init values. We wanted an simple mechanism to
     share code among chip families and revisions.

  4) Modularity - multiple chips may need to be supported in parallel
     by different developers.  We wanted to minimize the possibility
     of changes in one chip affecting another.  Unfortunately, this
     goal is somewhat converse of the previous goal.

Approach
--------

The XVD implementation was categorized as either common XVD code,
platform specific code, or code specific to a AVD decoder core.  The
bulk of the code is common to all XVD decoders and platforms, and
forms the main line.  Platform and core code is broken out into
functions and macros which implement the specific requirements of the
respective hardware.


Definitions
-----------

 common code: non-platform specific XVD code.  Uses macros to call
               the chip-specific implementations.

 top-level platform header: bxvd_platform.h. Included by the common
                            code.  Defines the function prototypes and
                            ensures all required chip-specific
                            definitions have been made.  Includes the
                            appropriate platform and chip specific
                            platform headers based on the value of
                            BCHP_CHIP.

 chip-specific platform header: bxvd_platform_xxxx.h (where xxxx is
                                the chip, e.g. 7401).  This file is
                                never included directly by the common
                                code.  It defines all the required
                                macros.

 core-specific platform header: bxvd_core_xxxx.h (where xxxx is the
                                core, e.g. avd_reve0).  Defines
                                core-specific macros.


Implementation
--------------

The chip-specific implementations were abstracted out of the main XVD
code and moved to chip-specific files using function macros.  e.g.,
the old interrupt setup code (100+ lines) in BXVD_Open() was replaced
with the following 1 line macro:

   rc = BXVD_P_SETUP_INTERRUPTS(pXvd);

The chip-specific platform header needs *3* different definitions in
order for the function abstraction to work.  (We will use 7401B0, for
the examples below):

 1) The macro itself needs to be defined to specify which
    implementation to call: (in bxvd_platform_7401.h)

    #define BXVD_P_SETUP_INTERRUPTS BXVD_P_SetupInterrupts_7401B0 


 2) The implementation needs to be conditionally compiled in.  (For
    any platform, all source files are included as part of the build
    to enable sharing of code between platforms).  The header needs to
    enable a particular version to be compiled in: (in
    bxvd_platform_7401.h)

    #define USE_SETUP_INTERRUPTS_7401B0 1 

    The corresponding implementation is conditionally wrapped around
    this define: (in bxvd_platform_7401.c)

    #if USE_SETUP_INTERRUPTS_7401B0
    BERR_Code BXVD_P_SetupInterrupts_7401B0( BXVD_Handle hXvd)
    {
       ...
    }
    #endif

   
 3) In order for the common code to compile without "missing
    prototype" warnings, the function prototype for the specific
    implementation needs to be declared: (in bxvd_platform_7401.h)

    SETUP_INTERRUPTS_PROTOTYPE(7401B0);

    To enforce prototype consistency amongst various platforms, the
    prototype declaration in the chip-specific header is done via a
    macro that is defined in the top-level platform header: (in
    bxvd_platform.h)

    #define SETUP_INTERRUPTS_PROTOTYPE(family) \
    BERR_Code BXVD_P_SetupInterrupts_##family \
    ( \
      BXVD_Handle hXvd \
    )


The top-level header also verifies that the required definitions have
been declared by the chip-specific header: (in bxvd_platform.h)

    #ifndef BXVD_P_SETUP_INTERRUPTS
    #error BXVD_P_SETUP_INTERRUPTS is undefined!
    #endif

 ***************************************************************************/

#include "bxvd.h"

#include "bstd.h"
#include "bchp.h"

#ifndef _BXVD_PLATFORM_H_
#define _BXVD_PLATFORM_H_

#include "bxvd_userdata.h"

/* prototype definition macros */
#define BXVD_P_SETUP_INTERRUPTS_PROTOTYPE(family) \
BERR_Code BXVD_P_SetupInterrupts_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_SETUP_WATCHDOG_PROTOTYPE(family) \
BERR_Code BXVD_P_SetupWatchdog_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_FW_LOAD_PROTOTYPE(family) \
BERR_Code BXVD_P_FWLoad_##family \
( \
  BXVD_Handle hXvd, \
  uint32_t uiDecoderInstance \
)

#define BXVD_P_CHIP_ENABLE_PROTOTYPE(family) \
BERR_Code BXVD_P_ChipEnable_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_CHIP_RESET_PROTOTYPE(family) \
BERR_Code BXVD_P_ChipReset_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_INIT_REG_PTRS_PROTOTYPE(family) \
void BXVD_P_InitRegPtrs_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_SETUP_FW_MEMORY_PROTOTYPE(family) \
BERR_Code BXVD_P_SetupFWMemory_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_TEAR_DOWN_FW_MEMORY_PROTOTYPE(family) \
BERR_Code BXVD_P_TearDownFWMemory_##family \
( \
  BXVD_Handle hXvd \
)

#define BXVD_P_VERIFY_WATCHDOG_FIRED_PROTOTYPE(family) \
bool BXVD_P_VerifyWatchdogFired_##family \
( \
  BXVD_Handle hXvd, \
  int param2 \
)

#define BXVD_P_CONVERT_UD_OFF2ADDR_PROTOTYPE(family) \
BERR_Code BXVD_P_ConvertUDOff2Addr_##family \
( \
  BXVD_Userdata_Handle hUserData, \
  unsigned long  fwUserDataAddr, \
  unsigned long  *pulUserDataAddr \
)

#if ((BCHP_CHIP == 7405) && ((BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1))) || \
    (BCHP_CHIP == 7335) || \
    (BCHP_CHIP == 7325) 

/* 7405 A0 rev H core */
#define BXVD_CHIP 740510

#elif ((BCHP_CHIP == 7405) && (BCHP_VER >= BCHP_VER_B0)) || \
      (BCHP_CHIP == 3548) || \
      (BCHP_CHIP == 3556) || \
      (BCHP_CHIP == 7336) || \
      (BCHP_CHIP == 7340) || \
      (BCHP_CHIP == 7342) || \
      (BCHP_CHIP == 7125) || \
      (BCHP_CHIP == 7408) || \
      (BCHP_CHIP == 7468) 

/* 7405 B0 rev I core */
#define BXVD_CHIP 740520

#elif (BCHP_CHIP == 35230) || \
      (BCHP_CHIP == 7550)  || \
      (BCHP_CHIP == 7630)  || \
      (BCHP_CHIP == 35125)

/* Rev J core */
#define BXVD_CHIP 'J'

#elif (BCHP_CHIP == 7135) || \
      (BCHP_CHIP == 7231) || \
      (BCHP_CHIP == 7344) || \
      (BCHP_CHIP == 7346) || \
      (BCHP_CHIP == 7358) || \
      (BCHP_CHIP == 7360) || \
      (BCHP_CHIP == 7422) || \
      (BCHP_CHIP == 7425) || \
      (BCHP_CHIP == 7429) || \
      (BCHP_CHIP == 7435) || \
      (BCHP_CHIP == 7552) || \
      (BCHP_CHIP == 7563) || \
      (BCHP_CHIP == 7584) || \
      (BCHP_CHIP == 7640) || \
      (BCHP_CHIP == 35233) 
/* Rev K core */
#define BXVD_CHIP 'K'

#elif (BCHP_CHIP == 7445)
/* Rev K core */
#define BXVD_CHIP 'N'

#else
#define BXVD_CHIP BCHP_CHIP
#endif

/* chip specific includes */
#if (BXVD_CHIP == 7401)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7401.h"
#elif (BXVD_CHIP == 7403)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7403.h"
#elif (BXVD_CHIP == 7118)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7118.h"
#elif (BXVD_CHIP == 7400)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7400.h"
#elif (BXVD_CHIP == 7440)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7440.h"
#elif (BXVD_CHIP == 740510) 
#include "bxvd_core_avd_revh0.h"
#include "bxvd_platform_7405.h"
#elif (BXVD_CHIP == 740520) 
#include "bxvd_core_avd_revi0.h"
#include "bxvd_platform_7405.h"
#elif (BXVD_CHIP == 7601)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7601.h"
#elif (BXVD_CHIP == 7635)
#include "bxvd_core_avd_reve0.h"
#include "bxvd_platform_7601.h"
#elif (BXVD_CHIP == 7420) 
#include "bxvd_core_avd_revi0.h"
#include "bxvd_platform_7420.h"
#elif (BXVD_CHIP == 'J') 
#include "bxvd_core_avd_revj0.h"
#include "bxvd_platform_revj0.h"
#elif (BXVD_CHIP == 'K') 
#include "bxvd_core_avd_revk0.h"
#include "bxvd_platform_revk0.h"
#elif (BXVD_CHIP == 'N') 
#include "bxvd_core_avd_revn0.h"
#include "bxvd_platform_revn0.h"
#else
#error Unsupported BCHP_CHIP version!
#endif

/* make sure the required definitions exist */
#ifndef BXVD_P_SETUP_INTERRUPTS
#error BXVD_P_SETUP_INTERRUPTS is undefined!
#endif

#ifndef BXVD_P_SETUP_WATCHDOG
#error BXVD_P_SETUP_WATCHDOG is undefined!
#endif

#ifndef BXVD_P_SETUP_FW_MEMORY
#error BXVD_P_SETUP_FW_MEMORY is undefined!
#endif

#ifndef BXVD_P_TEAR_DOWN_FW_MEMORY
#error BXVD_P_TEAR_DOWN_FW_MEMORY is undefined!
#endif

#ifndef BXVD_P_FW_LOAD
#error BXVD_P_FW_LOAD is undefined!
#endif

#ifndef BXVD_P_CHIP_ENABLE
#error BXVD_P_CHIP_ENABLE is undefined!
#endif

#ifndef BXVD_P_RESET_CHIP
#error BXVD_P_RESET_CHIP is undefined!
#endif

#ifndef BXVD_P_INIT_REG_PTRS
#error BXVD_P_INIT_REG_PTRS is undefined!
#endif

#ifndef BXVD_P_CONVERT_UD_OFF2ADDR
#error BXVD_P_CONVERT_UD_OFF2ADDR is undefined!
#endif

#ifndef BXVD_P_WRITE_FWCMD_TO_MBX
#error  BXVD_P_WRITE_FWCMD_TO_MBX is undefined!
#endif

#ifndef BXVD_P_WRITE_FWRSP_MBX
#error BXVD_P_WRITE_FWRSP_MBX is indefined!
#endif

#ifndef BXVD_P_VERIFY_WATCHDOG_FIRED
#error BXVD_P_VERIFY_WATCHDOG_FIRED is undefined!
#endif 

#ifndef BXVD_P_VALIDATE_PDEFSETTINGS
#define BXVD_P_VALIDATE_PDEFSETTINGS(pDefSettings) pDefSettings = pDefSettings
#endif

#ifndef BXVD_P_CONTEXT_PLATFORM
/* BXVD_P_Context_Platform can be overridden to define platform
 * specific variables in the BXVD_P_Context structure. */
#define BXVD_P_CONTEXT_PLATFORM
#endif

#ifndef BXVD_P_AVD_INIT_STRIPE_MULTIPLE
/* BXVD_P_AVD_INIT_STRIPE_MULTIPLE can be overridden to specify a
 * different stripe multiple */
#define BXVD_P_AVD_INIT_STRIPE_MULTIPLE 0
#endif

#ifndef BXVD_P_AVD_INIT_STRIPE_WIDTH
/* BXVD_P_AVD_INIT_STRIPE_WIDTH can be overridden to specify a
 * different stripe multiple */
#define BXVD_P_AVD_INIT_STRIPE_WIDTH 0
#endif

#ifndef BXVD_P_AVD_CORE_BAUD_RATE
/* BXVD_P_AVD_CORE_BAUD_RATE can be overridden to specify a different
 * baud rate */
#define BXVD_P_AVD_CORE_BAUD_RATE 115200        /* OL & IL UART baud rate */
#endif

#ifndef BXVD_P_AVD_CORE_UART_FREQ
/* BXVD_P_AVD_CORE_UART_FREQ can be overridden to specify a different
 * frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (200*1000000) /* UART clock frequency */
#endif

#ifndef BXVD_P_WATCHDOG_TIMEOUT
/* BXVD_P_WATCHDOG_TIMEOUT can be overridden to specify a different
 * watchdog timeout value */
#define BXVD_P_WATCHDOG_TIMEOUT 0x0bebc200
#endif

#ifndef BXVD_P_FW_IMAGE_SIZE
/* BXVD_P_FW_IMAGE_SIZE can be overridden to specify a different
 * firmware image size */
#define BXVD_P_FW_IMAGE_SIZE 0x100000
#endif

#ifndef BXVD_P_CHIP_PRODUCT_REVISION 
/* BXVD_P_AVD_CHIP_PROD_REVISION can be overridden to specify a
 * platform specific value */
#define BXVD_P_CHIP_PRODUCT_REVISION  BCHP_SUN_TOP_CTRL_PROD_REVISION 
#endif

#ifndef BXVD_P_RAVE_CONTEXT_SIZE   
/* BXVD_P_RAVE_XXXX register info can be overridden to specify a
 * platform specific value */
#define BXVD_P_RAVE_CONTEXT_SIZE        0
#define BXVD_P_RAVE_CX_HOLD_CLR_STATUS  0
#define BXVD_P_RAVE_PACKET_COUNT        0
#endif

#endif /* _BXVD_PLATFORM_H_ */
