/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_reve0.c $
 * $brcm_Revision: Hydra_Software_Devel/111 $
 * $brcm_Date: 8/24/12 5:51p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_core_avd_reve0.c $
 * 
 * Hydra_Software_Devel/111   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/110   7/25/12 1:52p davidp
 * SW7425-3598: Added 512 to still picture compatibility mode general heap
 * size to handle default cache line change in bmem.
 * 
 * Hydra_Software_Devel/109   6/18/12 4:27p davidp
 * SW7435-164: Remove un-needed include file.
 * 
 * Hydra_Software_Devel/108   7/28/11 5:54p davidp
 * SW7231-127: Add support for the rev K cores that do not have
 * BCHP_PWR_RESOURCE_AVD0_PWR resource,
 * 
 * Hydra_Software_Devel/107   7/26/11 4:27p davidp
 * SW7231-127: Move PM2 7420 flavor to core reve0 support files, this
 * method is also used on other platforms.
 * 
 * Hydra_Software_Devel/106   7/25/11 3:08p davidp
 * SW7231-127: Move old style PM support from 7405 specific to Rev E0
 * specific support file.
 * 
 * Hydra_Software_Devel/105   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/104   3/25/11 3:57p davidp
 * SW7405-5101: Add checks to verify AVD code space, CDB/ITB and memory
 * used is in memory region below 768mb for ARC300 based cores.
 * 
 * Hydra_Software_Devel/103   2/23/11 7:17a pblanco
 * SW7425-114: Fixed error when attempting to create a custom FW heap too
 * small to contain the entire FW image.
 * 
 * Hydra_Software_Devel/102   1/14/11 2:29p davidp
 * SW7422-22: Fix code that determines if BLD is present.
 * 
 * Hydra_Software_Devel/101   11/9/10 6:34p davidp
 * SW7422-22: Disable VICH IL and BL callback creation for now.
 * 
 * Hydra_Software_Devel/100   10/27/10 5:31p davidp
 * SW7422-22: Add VICH IL and BL interrupt support.
 * 
 * Hydra_Software_Devel/99   10/20/10 2:14p davidp
 * SW7425-16: Remove FGT support.
 * 
 * Hydra_Software_Devel/98   7/13/10 4:26p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/97   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/96   12/8/09 1:53p davidp
 * SW7405-3531: Decoder EOC value incorrrectly calculated during
 * authenticated FW load.
 * 
 * Hydra_Software_Devel/95   11/30/09 4:38p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_*
 * at compile.
 * 
 * Hydra_Software_Devel/94   10/16/09 4:07p davidp
 * SW7401-4332: Implement new AVD reset procedure.
 * 
 * Hydra_Software_Devel/93   8/14/09 10:45a pblanco
 * PR27168: Add error checking to UD address to offset conversion routine.
 * 
 * Hydra_Software_Devel/92   8/7/09 7:32p davidp
 * PR56375: Disable FW MBX polling by default.
 * 
 * Hydra_Software_Devel/91   8/7/09 4:49p davidp
 * PR56375: Add poll FW MBX support for chip bringups.
 * 
 * Hydra_Software_Devel/90   6/3/09 3:07p davidp
 * PR54107: Support zero size image file loading, do not load anything,
 * just start decoder FW already loaded.
 * 
 * Hydra_Software_Devel/89   3/30/09 3:49p davidp
 * PR53585: Call BMEM_Free with VA of picture memory, not PA.
 * 
 * Hydra_Software_Devel/88   1/21/09 2:01p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/87   1/9/09 5:01p rayshap
 * PR50960: Add fw version printout on watchdog
 * 
 * Hydra_Software_Devel/86   8/25/08 5:22p davidp
 * PR46115: Adjust initial memory allocation for general and cabac heaps
 * if size is not a multiple of minimum platform alignment.
 * 
 * Hydra_Software_Devel/85   7/15/08 11:40a davidp
 * PR43119: Fix compiler warning from previous checkin.
 * 
 * Hydra_Software_Devel/84   7/14/08 4:57p davidp
 * PR43119: Use atomic register update routine for the software reset
 * register.
 * 
 * Hydra_Software_Devel/83   6/26/08 12:35p rayshap
 * PR44191:  Fix fw load when loading data only and data sections are not
 * chunk
 * 
 * Hydra_Software_Devel/82   5/20/08 4:48p davidp
 * PR42812: Remove C++ comment header
 * 
 * Hydra_Software_Devel/81   5/20/08 4:28p davidp
 * PR42812: Output FW PC in a loop when watch dog occurs.
 * 
 * Hydra_Software_Devel/80   5/14/08 12:32p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/79   4/11/08 4:53p davidp
 * PR40234: Merge decoder debug logging from branch to
 * Hydra_Software_Devel/.
 * 
 * Hydra_Software_Devel/PR40234/1   4/10/08 3:41p davidp
 * PR40234: Add support for decoder debug logging.
 * 
 * Hydra_Software_Devel/78   1/25/08 7:01p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/77   11/20/07 2:28p davidp
 * PR27168: Move setup of TEST_PORT_CTRL register to BSP module.
 * 
 * Hydra_Software_Devel/76   11/13/07 3:09p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/75   11/6/07 5:55p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/74   11/6/07 2:22p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/73   10/26/07 1:27p nilesh
 * PR36480: Added bFGTEnable setting in BXVD_Settings to enable/disable
 * FGT SEI message parsing for a particular decoder
 * 
 * Hydra_Software_Devel/72   9/28/07 12:56p nilesh
 * PR27168: Review and comment padding calculations
 * 
 * Hydra_Software_Devel/71   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/70   9/12/07 3:44p nilesh
 * PR34832: Fixed issue with mailbox interrupt handler being called during
 * AVD boot process
 * 
 * Hydra_Software_Devel/69   9/10/07 11:42a pblanco
 * PR29915: Fixed misleading variable name in prototype of
 * BXVD_P_LoadFWImageToMem.
 * 
 * Hydra_Software_Devel/68   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/67   9/4/07 12:34p pblanco
 * PR29915: Handle cached vs uncached memory a little more gracefully in
 * loading and chunking code.
 * 
 * Hydra_Software_Devel/66   8/31/07 4:23p nilesh
 * PR29915: Added check for minimum fw memory size when using ELF FW
 * images
 * 
 * Hydra_Software_Devel/65   8/31/07 3:23p nilesh
 * PR29915: Fix boot result code propogation
 * 
 * Hydra_Software_Devel/64   8/31/07 11:41a nilesh
 * PR29915: Fix FGT support for DVD platforms
 * 
 * Hydra_Software_Devel/63   8/30/07 12:20p nilesh
 * PR34430: Channel specific heaps are now used for address conversion
 * 
 * Hydra_Software_Devel/62   8/29/07 4:37p nilesh
 * PR34430: XVD now supports a general memory size of 0 specified at
 * BXVD_Open
 * 
 * Hydra_Software_Devel/61   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/60   8/21/07 3:05p davidp
 * PR34052: Move register address pointers to platform_info.stReg
 * structure.
 * 
 * Hydra_Software_Devel/59   8/13/07 4:06p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/15   8/9/07 3:50p nilesh
 * PR29915: Updated memory tables according to AVD Mosaic Doc v2.11 (Aug
 * 9, 2007)
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/14   8/1/07 10:46a pblanco
 * PR29915: Made selection of heap or temporary allocation automatic.
 * There's no need to control this by a compile time flag.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/13   7/31/07 12:31p pblanco
 * PR29915: Changed BXVD_ALLOCATED_TEMP to BXVD_RELOC_MEMORY_SYSTEM.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/12   7/31/07 11:19a pblanco
 * PR29915: Added the ability to temporarily allocate FW relocation memory
 * instead of using existing heap. Controlled by setting
 * BXVD_ALLOCATED_TEMP=1 (default is 0).
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/11   7/19/07 5:14p nilesh
 * PR29915: Update BXVD_GetBufferConfig() and BXVD_P_ParseDecodeMode() to
 * handle BXVD_DecodeMode_eCustom
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/10   7/19/07 2:43p nilesh
 * PR29915: Cleaned up variable naming style
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/9   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/8   7/10/07 7:40p nilesh
 * PR29915: Cleaned up 7405 platform code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   7/2/07 1:53p nilesh
 * PR29915: Consolidated platform memory allocation code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline.
 * 
 * Hydra_Software_Devel/57   6/5/07 1:39p davidp
 * PR31721: Clear L3 interrupt mask (enable) register when chip is reset
 * also clear pending interrupt in setupWatchdog.
 * 
 * Hydra_Software_Devel/56   5/31/07 3:04p pblanco
 * PR27168: Added a compile time switchable option for additional memory
 * caching. Off by default.
 * 
 * Hydra_Software_Devel/55   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 * 
 * Hydra_Software_Devel/PR30249/1   5/1/07 3:44p nilesh
 * PR30249: Changes to support UOD 2.x development
 * 
 * Hydra_Software_Devel/54   4/26/07 3:19p nilesh
 * PR29949: BAVD_MFD_Picture hHeap parameter is now explicitly set by DM
 * 
 * Hydra_Software_Devel/53   4/24/07 2:44p nilesh
 * PR22766: Fixed compilation bug caused by initialization of 7440
 * specific FGT Heap parameter in BAVC_MFD_Picture struct
 * 
 * Hydra_Software_Devel/52   4/24/07 1:43p nilesh
 * PR22766: Added support for platform specific FGT buffer allocation
 * 
 * Hydra_Software_Devel/51   4/10/07 2:34p davidp
 * PR29622: Change buffer widht and height being passed to FW, also
 * allocate less picture buffer memory.
 * 
 * Hydra_Software_Devel/50   3/26/07 12:26p pblanco
 * PR27168: Converted more hardcoded constants to defined constants.
 * 
 * Hydra_Software_Devel/49   3/23/07 2:30p pblanco
 * PR27168: Added bchp_common.h for BCHP_PHYSICAL_OFFSET.
 * 
 * Hydra_Software_Devel/48   3/23/07 1:27p pblanco
 * PR27168: Began conversion of hard coded value to appropriate #defined
 * constants.
 * 
 * Hydra_Software_Devel/47   3/19/07 10:22a pblanco
 * PR27168: Added defined constants for init timeout, baud rate and UART
 * frequency. Also cleaned up some code formatting.
 * 
 * Hydra_Software_Devel/46   3/2/07 1:35p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 * 
 * Hydra_Software_Devel/45   3/2/07 12:15p pblanco
 * PR27283: Removed conditionalized UART code.
 * 
 * Hydra_Software_Devel/44   3/1/07 2:57p pblanco
 * PR26188: Removed SD_PARAM initialization from ChipReset.
 * 
 * Hydra_Software_Devel/43   2/26/07 8:41p nilesh
 * PR26188: Fixed heap used to create temporary memory during firmware
 * load on 7400B0
 * 
 * Hydra_Software_Devel/42   2/26/07 5:44p nilesh
 * PR26188: Fix 7400B0 memory setup issues
 * 
 * Hydra_Software_Devel/41   2/6/07 11:07a nilesh
 * PR27575: Fix 7440 Watchdog and make watchdog more robust on all
 * platforms.
 * 
 * Hydra_Software_Devel/PR27575/1   2/2/07 2:11p nilesh
 * PR27575: Fix 7440 Watchdog
 * 
 * Hydra_Software_Devel/40   2/2/07 1:08p nilesh
 * PR 27283: Firmware with new UART initialization checked in as release
 * v7.3.5.
 * 
 * Hydra_Software_Devel/37   1/29/07 12:43p pblanco
 * PR27283: Backed out most recent change... FW isn't ready yet.
 * 
 * Hydra_Software_Devel/36   1/29/07 11:33a pblanco
 * PR27283: Checked in core code with new UART initialization enabled.
 * 
 * Hydra_Software_Devel/35   1/23/07 1:42p pblanco
 * PR27283: New UART initialization support. Turned off for now via
 * NEW_UART_SETUP until new FW is delivered.
 * 
 * Hydra_Software_Devel/34   1/9/07 3:10p pblanco
 * PR26648: Changes to support watchdog startup after decoder start.
 * 
 * Hydra_Software_Devel/33   12/12/06 2:46p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/32   12/4/06 12:02p nilesh
 * PR26289: Changed error reporting during firmware load
 * 
 * Hydra_Software_Devel/31   11/17/06 4:51p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/5   11/17/06 4:13p nilesh
 * PR25868: Fix Authenticated Watchdog Boot
 * 
 * Hydra_Software_Devel/PR25868/4   11/17/06 2:36p nilesh
 * PR25868: Code clean-up: comments, function names, etc.
 * 
 * Hydra_Software_Devel/PR25868/3   11/16/06 4:52p nilesh
 * PR25868: Improve debugging messages
 * 
 * Hydra_Software_Devel/PR25868/2   11/16/06 3:44p nilesh
 * PR25868: Added authenticated image support, Refactored BIMG FW Load
 * code
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:54p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/30   11/13/06 2:02p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/29   11/9/06 4:44p davidp
 * PR25720: Change logical test to bitwise test when determining if
 * watchdog actually fired.
 * 
 * Hydra_Software_Devel/28   11/1/06 4:16p gaurava
 * PR 24521: Fixed memory configuration for AVC SD.
 * 
 * Hydra_Software_Devel/27   11/1/06 4:00p gaurava
 * PR 24521: Fixed memory configuration for AVC SD.
 * 
 * Hydra_Software_Devel/26   10/20/06 3:23p davidp
 * PR22817: SD still picture decode picture buffer size increased.
 * 
 * Hydra_Software_Devel/25   10/9/06 5:35p gaurava
 * PR 24521: Increased CABAC bin buffer to pass worst case Allegro streams
 * and decreased context memory used for all protocols and sizes.
 * 
 * Hydra_Software_Devel/24   9/22/06 2:22p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 * 
 * Hydra_Software_Devel/23   9/13/06 6:39p davidp
 * PR22817: Update CDB/ITB memory usage table, also FW decode mode memory
 * usage table.
 * 
 * Hydra_Software_Devel/22   9/7/06 4:30p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/21   9/5/06 3:04p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/2   9/5/06 10:50a nilesh
 * PR22867: Fix CABAC and Video Buffer alignment
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:12p davidp
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/20   8/23/06 4:36p davidp
 * PR22673: Cleanup BERR_TRACE usage
 * 
 * Hydra_Software_Devel/19   8/21/06 3:32p pblanco
 * PR22673: Add private tag ("P") to relocation engine names since they're
 * only used internally.
 * 
 * Hydra_Software_Devel/18   8/21/06 3:20p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/17   8/21/06 2:55p pblanco
 * PR22673: Renamed relocation engine entry points to XVD standards.
 * 
 * Hydra_Software_Devel/16   8/21/06 1:51p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/15   8/17/06 12:03p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 2:01p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/14   8/14/06 5:30p davidp
 * PR22673: Move setup interrupts and setup watchdog routines to
 * bxvd_core_avd_reve0.c
 * 
 * Hydra_Software_Devel/13   8/7/06 1:57p nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/12   8/7/06 11:47a nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/11   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/10   8/1/06 12:26p davidp
 * PR22673: Init picture buffer heap pointer in xvd_to_vdc structure in
 * SetupFWMemory().
 * 
 * Hydra_Software_Devel/9   7/26/06 4:51p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/8   7/24/06 10:59a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/7   7/21/06 12:03p pblanco
 * PR22673: Fixed bug referencing the proper area for userdata memory.
 * 
 * Hydra_Software_Devel/6   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/5   7/20/06 6:20p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/4   7/20/06 9:29a pblanco
 * PR22673: Added debugging macros and cleaned up the code (format only).
 * 
 * Hydra_Software_Devel/3   7/19/06 12:00p pblanco
 * PR22673: Give core functions a standardized form of their names without
 * the Core prefix. This allows us to use the same names for core and
 * platform functions.
 * 
 * Hydra_Software_Devel/2   7/19/06 11:24a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/1   7/18/06 3:52p pblanco
 * PR22673: Initial check in.
 * 
 *
 ***************************************************************************/
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_userdata.h"
#include "bxvd_platform.h"
#include "bchp_common.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

/* Local constants */
#define BXVD_REVE0_OL_INIT_TIMEOUT 1000         /* One second init timeout */
#define BXVD_PRIV_USE_CACHED_MEMORY 0           /* Turn caching on or off */

/* Define BXVD_POLL_FW_MBX to bring up decoder witout the use in interrupts */
/* #define BXVD_POLL_FW_MBX 1 */

BDBG_MODULE(BXVD_CORE_AVD_REVE0);

extern BERR_Code BXVD_P_Relf_RelocateELF(BXVD_Handle hXvd,
                     uint8_t  *image,
                     uint32_t image_size,
                     uint32_t code_base,
                     uint32_t load_base,
                     uint32_t *end_of_code);

#if BXVD_P_USE_FW_LOAD_CORE_REVE0
#define BXVD_IMAGE_FW_CHUNK_SIZE 16384

#define BXVD_MSG_HEADER_FIELD(ptr, field) BXVD_DBG_MSG(hXvd, ("Image Header: %s = %d", #field, ((BXVD_AvdImgHdr *) ptr)->field))
#define BXVD_MSG_HEADER_FIELD_HEX(ptr, field) BXVD_DBG_MSG(hXvd, ("Image Header: %s = 0x%x", #field, ((BXVD_AvdImgHdr *) ptr)->field))

BERR_Code BXVD_P_ChunkFWDataToMem(uint8_t* pDestPtr,
                  uint32_t uiNumBytes,
                  uint32_t uiOffset,
                  BIMG_Method_Next ImageNextFunc,
                  void *pImage)
{
   BERR_Code rc;
   uint32_t uiStartChunk, uiEndChunk, i;
   const void *pImageDataChunk;
   uint32_t uiCopyOffset = 0;
   uint32_t uiBytesToCopy;
   uint32_t uiBytesLeft = uiNumBytes;

   BDBG_ENTER(BXVD_P_ChunkFWDataToMem);       

   if (uiNumBytes == 0){

      BDBG_MSG(("Zero size image, FW image NOT loaded."));

      return BERR_TRACE(BERR_SUCCESS);
   }

   /* We read the firmware image in chunks of
    * BXVD_IMAGE_FW_CHUNK_SIZE.  This function allows us to copy
    * any byte offset and size from the image to memory.  In
    * order to do this, we need to handle the various cases:
    *
    * - Start Condition
    *  - uiOffset is 0 or a multiple of a chunk (We can start
    *    copying the chunk from the beginning)
    *  - uiOffset is not a multiple of a chunk (We need to start
    *    copying from an offset into the first chunk)
    * - Middle Condition
    *  - If the bytes left to copy is greater than the chunk size,
    *    then we can just copy the entire chunk
    * - End Condition
    *  - (uiOffset + uiNumBytes) ends on a chunk boundary (We can
    *    copy the last chunk until the end)
    *  - (uiOffset + uiNumBytes) does NOT end on a chunk boundary
    *    (We copy only a portion of the last chunk)
    */

   uiStartChunk = (uiOffset / BXVD_IMAGE_FW_CHUNK_SIZE) + 1;
   uiEndChunk = ((uiOffset + uiNumBytes - 1) / BXVD_IMAGE_FW_CHUNK_SIZE) + 1;
   if (uiOffset % BXVD_IMAGE_FW_CHUNK_SIZE) {
      uiCopyOffset = (uiOffset % BXVD_IMAGE_FW_CHUNK_SIZE);
   }
    
   for (i = uiStartChunk; i <= uiEndChunk; i++) {
      /* Retrieve next chunk */
      rc = ImageNextFunc(pImage,
                         i, /* The firmware chunks
                             * start from 1 */
                         &pImageDataChunk,
                         BXVD_IMAGE_FW_CHUNK_SIZE);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("Error reading image chunk: %d", i));
         return BERR_TRACE(rc);
      }
        
      /* Determine how many bytes of this chunk we need to
       * copy */
      if (uiBytesLeft > BXVD_IMAGE_FW_CHUNK_SIZE - uiCopyOffset) {
         uiBytesToCopy = BXVD_IMAGE_FW_CHUNK_SIZE - uiCopyOffset;
      } else {
         uiBytesToCopy = uiBytesLeft;
      }

 
      /* Copy chunk to temporary region */
      BKNI_Memcpy(pDestPtr, 
                  (uint8_t*)pImageDataChunk + uiCopyOffset, 
                  uiBytesToCopy);

      pDestPtr += uiBytesToCopy;           
      uiBytesLeft -= uiBytesToCopy;
      uiCopyOffset = 0;
   }

   BDBG_LEAVE(BXVD_P_ChunkFWDataToMem);

   return BERR_TRACE(BERR_SUCCESS);
}
     
BERR_Code BXVD_P_LoadFWImageToMem(BXVD_Handle hXvd,
                  BXVD_IMAGE_FirmwareID firmwareID,
                  uint32_t uiDestPhysAddr,
                                  uint32_t *puiStartOfCode,
                  uint32_t *puiEndOfCode)
{
   BERR_Code rc = BERR_SUCCESS;
   bool bTempAlloc = false;
   const BIMG_Interface *pImgInterface;
   void *pImgContext;
   void *pImage;
   void *pLoadAddress;
   uint32_t uiTempImageSize;
   uint8_t *pTempImageData;
   const void *pImageDataChunk;
   uint32_t uiImageOffset;
    
   BDBG_ENTER(BXVD_P_LoadFWImageToMem);
   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(puiStartOfCode);
   BDBG_ASSERT(puiEndOfCode);

   pImgInterface = hXvd->stSettings.pImgInterface;
   pImgContext = hXvd->stSettings.pImgContext;

   BDBG_ASSERT(pImgInterface);
   BDBG_ASSERT(pImgContext);
        
   if (hXvd->uiFWMemSize < BXVD_P_FW_IMAGE_SIZE)
   {
      BXVD_DBG_ERR(hXvd, ("Error: insufficient firmware memory. Need at least %d bytes", BXVD_P_FW_IMAGE_SIZE));
      return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);     
   }

   /* Open Firmware Image */
   BXVD_DBG_MSG(hXvd, ("Opening firmware image ID: %d", firmwareID)); 
   rc = pImgInterface->open(pImgContext,
                            &pImage,
                            firmwareID);
   if (rc != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Error opening image"));
      return BERR_TRACE(rc);
   }
    
   BXVD_DBG_MSG(hXvd, ("Reading firmware image header: %d bytes", 
             sizeof(BXVD_AvdImgHdr)));
    
   /* Read Firmware Image Header */
   rc = pImgInterface->next(pImage,
                            0, /* Header is always chunk #0 */
                            &pImageDataChunk,
                            sizeof(BXVD_AvdImgHdr));
   if (rc != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Error reading image header"));
      pImgInterface->close(pImage);
      return BERR_TRACE(rc);
   }    

   /* Debug messages to print header */
   BXVD_MSG_HEADER_FIELD(pImageDataChunk, uiHeaderVersion);
   BXVD_MSG_HEADER_FIELD(pImageDataChunk, uiHeaderSize);
   BXVD_MSG_HEADER_FIELD(pImageDataChunk, uiImageSize);
   BXVD_MSG_HEADER_FIELD(pImageDataChunk, uiRelocationBase);

   /* Set up temporary memory for ELF image */
   uiTempImageSize = ((BXVD_AvdImgHdr *) pImageDataChunk)->uiImageSize;
   uiImageOffset = ((BXVD_AvdImgHdr *) pImageDataChunk)->uiRelocationBase;

   if (uiTempImageSize > hXvd->uiFWGenMemSize)
   {
      BXVD_DBG_MSG(hXvd, ("Heap too small, allocating temporary FW reloc memory"));
      pTempImageData = (uint8_t*)BKNI_Malloc(uiTempImageSize);
      if (pTempImageData == NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Error: Not enough temp memory for loading image"));
         pImgInterface->close(pImage);
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      BKNI_Memset((void *)pTempImageData, 0, uiTempImageSize);
      bTempAlloc = true;
      pLoadAddress = pTempImageData;
   }
   else
   {
      BXVD_DBG_MSG(hXvd, ("Temporarily using exising heap for FW relocation"));
      pTempImageData = (uint8_t*) hXvd->uiFWGenMemVirtAddr;
      bTempAlloc = false;
#if BXVD_PRIV_USE_CACHED_MEMORY
      BMEM_Heap_ConvertAddressToCached(hXvd->hSystemHeap, 
                                       pTempImageData, 
                                       (void **)&pLoadAddress);
#else
      pLoadAddress = pTempImageData;
#endif   
   }

   /* Get the firmware chunks */
   BXVD_DBG_MSG(hXvd, ("Reading firmware image data: %d bytes", uiTempImageSize));
   rc = BXVD_P_ChunkFWDataToMem(pLoadAddress,
                                uiTempImageSize,
                                0,
                                pImgInterface->next,
                                pImage);
   if (rc != BERR_SUCCESS) {
      pImgInterface->close(pImage);
      if (bTempAlloc == true)
         BKNI_Free(pLoadAddress);
      return BERR_TRACE(rc);
   }

#if BXVD_PRIV_USE_CACHED_MEMORY
      BMEM_Heap_FlushCache(hXvd->hSystemHeap, pLoadAddress, uiNumBytes);
#endif

   BXVD_DBG_MSG(hXvd, ("Closing firmware image"));    
   /* Close Firmware Image */
   pImgInterface->close(pImage);
    
   *puiStartOfCode = uiDestPhysAddr + uiImageOffset;
   BXVD_DBG_MSG(hXvd, ("Relocating ELF firmware image to 0x%x", *puiStartOfCode));    
   /* Relocate the firmware image to specified memory address */
   rc = BXVD_P_Relf_RelocateELF(hXvd,
                                pLoadAddress,
                                uiTempImageSize, 
                                BXVD_P_RELF_DEFAULT_CODE_BASE,
                                *puiStartOfCode,
                                puiEndOfCode);

   if (bTempAlloc == true)
   {
      BXVD_DBG_MSG(hXvd, ("Freeing temporary FW memory"));
      BKNI_Free(pLoadAddress);
   }

   BDBG_LEAVE(BXVD_P_LoadFWImageToMem);

   return BERR_TRACE(rc);
}
            
BERR_Code BXVD_P_LoadAuthenticatedFWImageToMem(BXVD_Handle hXvd,
                           uint32_t *puiCmdBufferVector,
                           uint32_t *puiInstructionBaseOL,
                           uint32_t *puiEndOfCodeOL,
                           uint32_t *puiInstructionBaseIL,
                           uint32_t *puiEndOfCodeIL)
{
   BERR_Code rc = BERR_SUCCESS;
   void *pImage;
   BXVD_AvdImgHdr imageHeader;
   const void *pImageDataChunk;
   const BIMG_Interface *pImgInterface = NULL;
   void *pImgContext = NULL;
   BXVD_IMAGE_FirmwareID firmwareID;
   uint32_t uiDestPhyAddr;
   uint32_t uiDestVirtAddr;
   uint32_t uiDestSize;

   BDBG_ENTER(BXVD_P_LoadAuthenticatedFWImageToMem);
   BDBG_ASSERT(hXvd);

   pImgInterface = hXvd->stSettings.pImgInterface;  
   pImgContext = hXvd->stSettings.pImgContext;
   firmwareID = hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID;
   uiDestPhyAddr = hXvd->uiFWMemPhyAddr;
   uiDestVirtAddr = hXvd->uiFWMemVirtAddr;
   uiDestSize = hXvd->uiFWMemSize;

   BDBG_ASSERT(pImgInterface);
   BDBG_ASSERT(pImgContext);
   BDBG_ASSERT(puiCmdBufferVector);
   BDBG_ASSERT(puiInstructionBaseOL);
   BDBG_ASSERT(puiEndOfCodeOL);
   BDBG_ASSERT(puiInstructionBaseIL);
   BDBG_ASSERT(puiEndOfCodeIL);

   /* Open Firmware Image */
   BXVD_DBG_MSG(hXvd, ("Opening firmware image ID: %d", firmwareID)); 
   rc = pImgInterface->open(pImgContext,
                            &pImage,
                            firmwareID);
   if (rc == BERR_INVALID_PARAMETER) {
      /* An invalid parameter means that authenticated
       * firmware does not exist.  We fail silently here and
       * expect this error to be reported by the caller
       * depending on the situation */
      return rc;
   } else if (rc != BERR_SUCCESS) {
      BXVD_DBG_ERR(hXvd, ("Error opening image"));
      return BERR_TRACE(rc);
   }
    
   /* Read header */
   BXVD_DBG_MSG(hXvd, ("Reading authenticated firmware image header: %d bytes",
             sizeof(BXVD_AvdImgHdr)));  

   /* Read Firmware Image Header */
   rc = pImgInterface->next(pImage,
                            0, /* Header is always chunk #0 */
                            &pImageDataChunk,
                            sizeof(BXVD_AvdImgHdr));
   if (rc != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Error reading image header"));
      pImgInterface->close(pImage);
      return BERR_TRACE(rc);
   }    

   BKNI_Memcpy(&imageHeader, pImageDataChunk, sizeof(BXVD_AvdImgHdr));
   /* Debug messages to print header */
   BXVD_MSG_HEADER_FIELD((&imageHeader), uiHeaderVersion);
   BXVD_MSG_HEADER_FIELD((&imageHeader), uiHeaderSize);
   BXVD_MSG_HEADER_FIELD((&imageHeader), uiImageSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), uiRelocationBase);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerCode.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerCode.uiAddress);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerCode.uiSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerCode.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerCode.uiAddress);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerCode.uiSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), commandBuffer.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), commandBuffer.uiAddress);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), commandBuffer.uiSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerData.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerData.uiAddress);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), outerData.uiSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerData.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerData.uiAddress);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), innerData.uiSize);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), signature.uiOffset);
   BXVD_MSG_HEADER_FIELD_HEX((&imageHeader), signature.uiAddress);
   BXVD_MSG_HEADER_FIELD((&imageHeader), signature.uiSize); 
        
   /* Verify that the specified destination address matches the
    * firmware's relocated base address */
   if (uiDestPhyAddr != imageHeader.uiRelocationBase) {
      BXVD_DBG_ERR(hXvd, ("Error: FW relocation address (0x%x) does not match FW memory heap (0x%x)", imageHeader.uiRelocationBase, uiDestPhyAddr));
      pImgInterface->close(pImage);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Verify that the image header is one we understand
    * firmware's relocated base address */
   if ((imageHeader.uiHeaderSize != sizeof(BXVD_AvdImgHdr)) ||
       (imageHeader.uiHeaderVersion != BXVD_AVD_IMG_HDR_VERSION)) {
      BXVD_DBG_ERR(hXvd, ("Error: incompatible firmware image header version/size"));
      pImgInterface->close(pImage);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Verify there's enough firmware memory to load the image */
   if (imageHeader.uiImageSize > uiDestSize) {
      BXVD_DBG_ERR(hXvd, ("Error: insufficient firmware memory. Need at least %d bytes", imageHeader.uiImageSize));
      pImgInterface->close(pImage);
      return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);     
   }
    
   /* Handle normal vs. watchdog boot.  If we are booted
    * externally and are in watchdog, we should just reload the
    * data region, since we have to assume the code region is
    * locked down */
   if ((hXvd->stSettings.pAVDBootCallback != NULL) && 
       (hXvd->eAVDBootMode != BXVD_AVDBootMode_eNormal)) {
      /* Re-load just the data regions of authenticated image */
      BXVD_DBG_MSG(hXvd, ("Authenticated Firmware Load: Watchdog - data regions only"));
      /* Load Outer Loop Data */
      rc = BXVD_P_ChunkFWDataToMem((uint8_t*)(hXvd->uiFWMemVirtAddr + 
                                              (imageHeader.outerData.uiAddress - hXvd->uiFWMemPhyAddr)),
                                   imageHeader.outerData.uiSize,
                                   imageHeader.outerData.uiOffset,
                                   pImgInterface->next,
                                   pImage);
      if (rc != BERR_SUCCESS) {
         pImgInterface->close(pImage);
         return BERR_TRACE(rc);
      }
        
      /* Load Inner Loop Data */
      rc = BXVD_P_ChunkFWDataToMem((uint8_t*) (hXvd->uiFWMemVirtAddr + 
                                               (imageHeader.innerData.uiAddress - hXvd->uiFWMemPhyAddr)),
                                   imageHeader.innerData.uiSize,
                                   imageHeader.innerData.uiOffset,
                                   pImgInterface->next,
                                   pImage);
      if (rc != BERR_SUCCESS) {
         pImgInterface->close(pImage);
         return BERR_TRACE(rc);
      }
   } else {
      BXVD_DBG_MSG(hXvd, ("Authenticated Firmware Load: Normal"));
      /* Get the firmware chunks */
      rc = BXVD_P_ChunkFWDataToMem((uint8_t*) uiDestVirtAddr,
                                   imageHeader.uiImageSize,
                                   0,
                                   pImgInterface->next,
                                   pImage);
        
      if (rc != BERR_SUCCESS) {
         pImgInterface->close(pImage);
         return BERR_TRACE(rc);
      }
   }
   BXVD_DBG_MSG(hXvd, ("Closing firmware image"));    
   /* Close Firmware Image */
   pImgInterface->close(pImage);

   /* Set CmdBufferAddr, OL EOC, and IL EOC */
   *puiCmdBufferVector = imageHeader.commandBuffer.uiAddress;

   *puiInstructionBaseOL = imageHeader.outerCode.uiAddress;
   *puiEndOfCodeOL = imageHeader.outerCode.uiSize;

   *puiInstructionBaseIL = imageHeader.innerCode.uiAddress;
   *puiEndOfCodeIL = imageHeader.innerCode.uiSize;

   BDBG_LEAVE(BXVD_P_LoadAuthenticatedFWImageToMem);

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_FWLoad_RevE0(BXVD_Handle hXvd, 
                  uint32_t uiDecoderInstance)
{
   uint32_t uiStartOfOLCode, uiStartOfILCode;
   uint32_t uiEndOfOLCode, uiEndOfILCode;
   void *pvCmdBuffer;
   void *pvCachedCmdBuffer;

   BERR_Code rc;

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(hXvd->stSettings.pImgInterface);
   BDBG_ASSERT(hXvd->stSettings.pImgContext);
   BSTD_UNUSED(uiDecoderInstance);

   BDBG_ENTER(BXVD_P_FWLoad_RevE0);

   uiEndOfOLCode = 0;
   uiEndOfILCode = 0;

   BXVD_DBG_MSG(hXvd, ("Attempting load of authenticated firmware"));
   rc = BXVD_P_LoadAuthenticatedFWImageToMem(hXvd,
                         &hXvd->uiCmdBufferVector,
                         &hXvd->uiOuterLoopInstructionBase,
                         &hXvd->uiOuterLoopEOC,
                         &hXvd->uiInnerLoopInstructionBase,
                         &hXvd->uiInnerLoopEOC);
   if (rc == BERR_SUCCESS) {
      BXVD_DBG_MSG(hXvd, ("Authenticated firmware loaded"));
   } else {
      BXVD_DBG_MSG(hXvd, ("Authenticated firmware NOT loaded"));
      BXVD_DBG_MSG(hXvd, ("Attempting load of normal firmware"));

      BXVD_DBG_MSG(hXvd, ("Loading and relocating outer loop ELF image"));
      rc = BXVD_P_LoadFWImageToMem(hXvd,
                                   hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID,
                                   hXvd->uiFWMemPhyAddr,
                                   &uiStartOfOLCode,
                                   &uiEndOfOLCode);
   
      if (rc != BERR_SUCCESS || uiEndOfOLCode == 0 || uiStartOfOLCode == 0)
      {
         BXVD_DBG_ERR(hXvd, ("Error loading outer loop"));
         return BERR_TRACE(rc);
      }

      hXvd->uiOuterLoopInstructionBase = uiStartOfOLCode;
      hXvd->uiCmdBufferVector = uiEndOfOLCode;
      hXvd->uiOuterLoopEOC = uiEndOfOLCode -  hXvd->uiOuterLoopInstructionBase; 

      BXVD_DBG_MSG(hXvd, ("End of outer loop code at %x", hXvd->uiOuterLoopEOC));

      BXVD_DBG_MSG(hXvd, ("Loading and relocating inner loop ELF image"));  
      rc = BXVD_P_LoadFWImageToMem(hXvd,
                                   hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID,
                                   hXvd->uiFWMemPhyAddr,
                                   &uiStartOfILCode,
                                   &uiEndOfILCode);

      if (rc != BERR_SUCCESS || uiEndOfILCode == 0 || uiStartOfILCode == 0)
      {
         BXVD_DBG_ERR(hXvd, ("Error loading inner loop"));
         return BERR_TRACE(rc);
      }

      hXvd->uiInnerLoopInstructionBase = uiStartOfILCode;
      hXvd->uiInnerLoopEOC = uiEndOfILCode - hXvd->uiInnerLoopInstructionBase;
      BXVD_DBG_MSG(hXvd, ("End of inner loop code at %x\n", hXvd->uiInnerLoopEOC));
   }

   BMEM_ConvertOffsetToAddress(hXvd->hCodeHeap, 
                   (uint32_t)hXvd->uiCmdBufferVector, 
                   &pvCmdBuffer); 

   rc = BMEM_Heap_ConvertAddressToCached(hXvd->hCodeHeap, (void *) pvCmdBuffer, &pvCachedCmdBuffer);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   hXvd->stDecoderContext.ulCmdBufferAddr = (uint32_t) pvCachedCmdBuffer;
   hXvd->stDecoderContext.ulRspBufferAddr = (uint32_t) pvCachedCmdBuffer;

   BDBG_LEAVE(BXVD_P_FWLoad_RevE0);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif /* BXVD_P_USE_FW_LOAD_CORE_REVE0 */

#if  BXVD_P_USE_SETUP_INTERRUPTS_REVE0
BERR_Code BXVD_P_SetupInterrupts_RevE0( BXVD_Handle hXvd)
{
   BERR_Code rc;

#if BXVD_P_AVD_ARC600
   bool bBLDPresent = false;
#endif

   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_SetupInterrupts_RevE0);

#if BXVD_P_AVD_ARC600

   /* Determine if Base Layer ARC is present in this decoder */
   if ( (BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_CPUId))
        & hXvd->stPlatformInfo.stReg.uiDecode_CPUId_BldIdMask)
   {
      bBLDPresent = true;
   }
#endif

   if (hXvd->stDecoderContext.pCbAVC_MBX_ISR) 
   {
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }
   else 
   {
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_MBX_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_Mailbox,
                               BXVD_P_AVD_MBX_isr,
                               (void*)(&hXvd->stDecoderContext),
                               0);
       
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }
       
   rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }
     
   if (hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR) {
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   } else {
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_StillPictureRdy,
                               BXVD_P_AVD_StillPictureRdy_isr, 
                               (void*)(&hXvd->stDecoderContext),
                               0);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }
   
   rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR);
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

   /* Video Instruction Checker interrupts */

   /* See if the interrupt callbacks already exist*/ 
   if (hXvd->stDecoderContext.pCbAVC_VICHReg_ISR)
   {
      /* Disable interrupts */

      /* VICH Register acccess address violation */
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }     

      /* VICH SCB write address violation */
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }     

      /* VICH Instruction read violation */
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
     
#if BXVD_P_AVD_ARC600
      /* VICH Inner Loop Instruction read violation */
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }     

      if (bBLDPresent == true)
      {
         /* VICH Base Layer Instruction read violation */
         rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }
      }
#endif

      /* If callback is valid, re-enable interrupt */
      if (hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eVidInstrChecker].BXVD_P_pAppIntCallbackPtr) 
      {
         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }

         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }

         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }

#if BXVD_P_AVD_ARC600
         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }

         if (bBLDPresent == true)
         {
            rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR);
            if (rc != BERR_SUCCESS)
            {
               return BERR_TRACE(rc);
            }
         }
#endif
      }
   } 
   else
   {
      /* Interrupts callbacks need to be created */
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_VICReg,
                               BXVD_P_VidInstrChkr_isr,
                               (void*)(hXvd),
                               BXVD_VICHInt_eRegAccess);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_VICSCBWr,
                               BXVD_P_VidInstrChkr_isr,
                               (void*)(hXvd),
                               BXVD_VICHInt_eSCBWrite);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_VICInstrRd,
                               BXVD_P_VidInstrChkr_isr,
                               (void*)(hXvd),
                               BXVD_VICHInt_eInstrRead);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

#if BXVD_P_AVD_ARC600
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_VICILInstrRd,
                               BXVD_P_VidInstrChkr_isr,
                               (void*)(hXvd),
                               BXVD_VICHInt_eInstrRead);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

      if (bBLDPresent == true)
      {
         rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR),
                                  hXvd->hInterrupt,
                                  hXvd->stPlatformInfo.stReg.uiInterrupt_VICBLInstrRd,
                                  BXVD_P_VidInstrChkr_isr,
                                  (void*)(hXvd),
                                  BXVD_VICHInt_eInstrRead);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }
      }
#endif
   }

   if (hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR) 
   {
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }     
        
      if (hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eStereoSeqError].BXVD_P_pAppIntCallbackPtr) {
         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }      
      }
   } 
   else
   {
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_StereoSeqError,
                               BXVD_P_StereoSeqError_isr,
                               (void*)(hXvd),
                               0);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }
    
   BDBG_LEAVE(BXVD_P_SetupInterrupts_RevE0);

   return BERR_TRACE(rc);
}
#endif /* BXVD_P_USE_SETUP_INTERRUPTS_REVE0 */

#if BXVD_P_USE_SETUP_WATCHDOG_REVE0
BERR_Code BXVD_P_SetupWatchdog_RevE0( BXVD_Handle hXvd)

{
   BERR_Code rc = BERR_SUCCESS;
    
   BDBG_ASSERT(hXvd);
    
   BDBG_ENTER(BXVD_P_SetupWatchdog_RevE0);
    
   if (hXvd->stDecoderContext.pCbAVC_Watchdog_ISR) {
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_Watchdog_ISR);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }     
        
      if (hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eWatchdog].BXVD_P_pAppIntCallbackPtr) {
         rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_Watchdog_ISR);
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }      
      }
   } else {
      rc = BINT_CreateCallback(&(hXvd->stDecoderContext.pCbAVC_Watchdog_ISR),
                               hXvd->hInterrupt,
                               hXvd->stPlatformInfo.stReg.uiInterrupt_OuterWatchdog,
                               BXVD_P_WatchdogInterrupt_isr,
                               (void*)(hXvd),
                               BXVD_P_OUTER_WATCHDOG);
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }
    
   BXVD_Reg_Read32(hXvd, 
                   hXvd->stPlatformInfo.stReg.uiDecode_OuterWatchdogTimer);
    
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear,
                    BXVD_P_INTR_CLEAR);

   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask,
                    BXVD_P_INTR_OL_MASK);

   BDBG_LEAVE(BXVD_P_SetupWatchdog_RevE0);
    
   return BERR_TRACE(rc);
}
#endif /* BXVD_P_USE_SETUP_WATCHDOG_REVE0 */


#ifdef BXVD_P_USE_SETUP_FW_MEMORY_REVE0

BERR_Code BXVD_P_SetupFWMemory_RevE0(BXVD_Handle hXvd)
{
   BXVD_DecodeMode eDecodeMode;
   BXVD_DecodeMode eSPDecodeMode;
   bool bSPEnabled;

   BMEM_HeapInfo heapInfo; 
   uint32_t      uiAlignment;

   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_P_SetupFWMemory_RevE0);

   BDBG_ASSERT(hXvd);

   /* Allocate FW programs memory */
   hXvd->uiFWMemVirtAddr = (uint32_t)BMEM_AllocAligned( hXvd->hCodeHeap, BXVD_P_FW_IMAGE_SIZE, 12, 0);

   if ((uint32_t)hXvd->uiFWMemVirtAddr == (uint32_t)NULL)
   {
      BXVD_DBG_ERR(hXvd, ("Not enough device memory for FW program"));
      return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
   }

   BMEM_ConvertAddressToOffset( hXvd->hCodeHeap, (void *)hXvd->uiFWMemVirtAddr, (uint32_t *)&hXvd->uiFWMemPhyAddr ); 

   /* We set the Allocated flag to true, so that we know to free
    * this region */
   hXvd->bFWMemAllocated = true;
   hXvd->uiFWMemSize = BXVD_P_FW_IMAGE_SIZE;

#if !BXVD_P_AVD_ARC600
   /* Pre rev K cores require FW code loaded below 768 MB */
   if ((hXvd->uiFWMemPhyAddr + BXVD_P_FW_IMAGE_SIZE) >= BXVD_P_ARC300_RAM_LIMIT)
   {
      BXVD_DBG_ERR(hXvd, ("AVD Firmware code memory (%0x) allocated in region greater than 768MB!", hXvd->uiFWMemPhyAddr));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
#endif

   BXVD_DBG_MSG(hXvd, ("FWMem:0x%x, Phys:0x%x", hXvd->uiFWMemVirtAddr, hXvd->uiFWMemPhyAddr));
   
   /* Initialize memory sizes */
   hXvd->uiFWGenMemSize = 0;
   hXvd->uiFWCabacMemSize = 0;
   hXvd->uiFWPicMemSize = 0;

   /* Determine how much memory to allocate for video decode */
   rc = BXVD_P_ParseDecodeMode(hXvd, hXvd->stSettings.pDecodeModes, &eDecodeMode);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
   
   if (eDecodeMode == BXVD_DecodeMode_eCustom)
   {
      /* Need to pad a bit since sub-allocations will need to be on 4k
       * boundaries. We need to account for 4K padding per channel
       * that we intend to have open.  Since we are in custom mode, we
       * will have at most BXVD_MAX_VIDEO_CHANNELS channels */
      hXvd->uiFWGenMemSize += hXvd->stSettings.stFWMemConfig.uiGeneralHeapSize;
      if (hXvd->uiFWGenMemSize > 0)
      {
         hXvd->uiFWGenMemSize +=  (BXVD_MAX_VIDEO_CHANNELS * 4096);
      }
      hXvd->uiFWCabacMemSize += hXvd->stSettings.stFWMemConfig.uiCabacHeapSize;
      hXvd->uiFWPicMemSize += hXvd->stSettings.stFWMemConfig.uiPictureHeapSize;            
   }
   else
   {
      hXvd->uiFWGenMemSize += hXvd->stFWMemCfg[eDecodeMode].general_memory_size;
      hXvd->uiFWCabacMemSize += hXvd->stFWMemCfg[eDecodeMode].cabac_bin_size;      
      hXvd->uiFWPicMemSize += (hXvd->stFWMemCfg[eDecodeMode].video_block_size * hXvd->stFWMemCfg[eDecodeMode].video_block_count);
   }

   /* Determine how much memory to allocate for still picture compatibility mode */
   rc = BXVD_P_StillPictureEnabled(hXvd, &bSPEnabled, &eSPDecodeMode);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
   
   if (bSPEnabled)
   {
      hXvd->bStillPictureCompatibilityMode = true;
      if (eSPDecodeMode == BXVD_DecodeMode_eStill_HD)
      {
         hXvd->bStillHDCapable = true;
      }
      else
      {
         hXvd->bStillHDCapable = false;
      }

      if (eDecodeMode != BXVD_DecodeMode_eCustom)
      {
         /* Need to pad a bit since sub-allocations will need to be on
          * 4k boundaries. We need to account for 4K padding per
          * channel that we intend to have open.  Since we are in a
          * compatibility mode, we will have at most 2 channels, a
          * video decode channel and an implicit still picture
          * channel */
         hXvd->uiFWGenMemSize += (2 * 4096);
      }
      else
      {
         /* 512 is the default cache alignment size. Needed for padding */
         hXvd->uiFWGenMemSize += 512;
      }
      
      hXvd->uiFWGenMemSize += hXvd->stFWMemCfg[eSPDecodeMode].general_memory_size;
      hXvd->uiFWCabacMemSize += hXvd->stFWMemCfg[eSPDecodeMode].cabac_bin_size;
      hXvd->uiFWPicMemSize += (hXvd->stFWMemCfg[eSPDecodeMode].video_block_size * hXvd->stFWMemCfg[eSPDecodeMode].video_block_count);
   }   
   
   /* Coalesce memory if heaps are same */
   if (hXvd->hSystemHeap == hXvd->hCabacHeap)
   {
      hXvd->uiFWGenMemSize += hXvd->uiFWCabacMemSize;
      hXvd->uiFWCabacMemSize = 0;
   }

   if (hXvd->hSystemHeap == hXvd->hPictureHeap)
   {
      hXvd->uiFWGenMemSize += hXvd->uiFWPicMemSize;
      hXvd->uiFWPicMemSize = 0;
   }

   /* Allocate context memory */  
   if (hXvd->uiFWGenMemSize != 0)
   {
      /* The allocated buffer needs to be a multiple of the alignment size.
       * Get heap info to determine heap alignment. Adjust size accordingly. */

      BMEM_Heap_GetInfo( hXvd->hSystemHeap, &heapInfo);
      
      uiAlignment = heapInfo.uiAlignment;

      if ((hXvd->uiFWGenMemSize % (1<<uiAlignment)) != 0)
      {
         hXvd->uiFWGenMemSize = (((hXvd->uiFWGenMemSize / (1<<uiAlignment)) + 1) * (1<<uiAlignment));
      }

      hXvd->uiFWGenMemVirtAddr = (uint32_t)BMEM_AllocAligned( hXvd->hSystemHeap, 
                                                              hXvd->uiFWGenMemSize, 12, 0); 

      if ((uint32_t)hXvd->uiFWGenMemVirtAddr == (uint32_t)NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Not enough context memory FW data heap"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      
      BMEM_ConvertAddressToOffset( hXvd->hSystemHeap, (void *)hXvd->uiFWGenMemVirtAddr, 
                                   (uint32_t *)&hXvd->uiFWGenMemPhyAddr ); 
      
      BXVD_DBG_MSG(hXvd, ("FWGenMem VA:0x%x, PA:0x%x, Size:0x%x", 
                          hXvd->uiFWGenMemVirtAddr, 
                          hXvd->uiFWGenMemPhyAddr, 
                          hXvd->uiFWGenMemSize));
   }
   
   /* Allocate cabac/secure memory */
   if (hXvd->uiFWCabacMemSize != 0)
   {
      /* The allocated buffer needs to be a multiple of the alignment size.
       * Get heap info to determine heap alignment. Adjust size accordingly. */

      BMEM_Heap_GetInfo( hXvd->hCabacHeap, &heapInfo);
      
      uiAlignment = heapInfo.uiAlignment;

      if ((hXvd->uiFWCabacMemSize % (1<<uiAlignment)) != 0)
      {
         hXvd->uiFWCabacMemSize = (((hXvd->uiFWCabacMemSize / (1<<uiAlignment)) + 1) * (1<<uiAlignment));
      }

      hXvd->uiFWCabacMemVirtAddr = (uint32_t)BMEM_AllocAligned(hXvd->hCabacHeap,
                                                               hXvd->uiFWCabacMemSize, 8, 0);
      
      if ((uint32_t)hXvd->uiFWCabacMemVirtAddr == (uint32_t)NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Not enough FW cabac memory data heap"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      BMEM_ConvertAddressToOffset( hXvd->hCabacHeap, (void *)hXvd->uiFWCabacMemVirtAddr, 
                                   (uint32_t *)&hXvd->uiFWCabacMemPhyAddr ); 
      
      BXVD_DBG_MSG(hXvd, ("FWCabacMem VA:0x%x, PA:0x%x, Size:0x%x", 
                          hXvd->uiFWCabacMemVirtAddr, 
                          hXvd->uiFWCabacMemPhyAddr, 
                          hXvd->uiFWCabacMemSize));
   }
   
   /* Allocate picture memory */
   if (hXvd->uiFWPicMemSize != 0)
   {
      hXvd->uiFWPicMemVirtAddr = (uint32_t)BMEM_AllocAligned(hXvd->hPictureHeap, hXvd->uiFWPicMemSize, 12, 0);       
      
      if ((uint32_t)hXvd->uiFWPicMemVirtAddr == (uint32_t)NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Not enough FW picture memory heap space"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      BMEM_ConvertAddressToOffset( hXvd->hPictureHeap, (void *)hXvd->uiFWPicMemVirtAddr, 
                                   (uint32_t *)&hXvd->uiFWPicMemPhyAddr ); 
      
      BXVD_DBG_MSG(hXvd, ("FWPictMem VA:0x%x, PA:0x%x, Size:0x%x", 
                          hXvd->uiFWPicMemVirtAddr, 
                          hXvd->uiFWPicMemPhyAddr, 
                          hXvd->uiFWPicMemSize));
   }

   if (hXvd->stSettings.uiDecoderDebugLogBufferSize != (uint32_t)NULL)
   {
      hXvd->uiDecoderDbgBufVirtAddr =
         (uint32_t)BMEM_AllocAligned( hXvd->hSystemHeap,
                                      hXvd->stSettings.uiDecoderDebugLogBufferSize,
                                      12, 0); 
      
      if ((uint32_t)hXvd->uiDecoderDbgBufVirtAddr == (uint32_t)NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Not enough context memory FW Debug Log"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      
      BMEM_ConvertAddressToOffset( hXvd->hSystemHeap, (void *)hXvd->uiDecoderDbgBufVirtAddr,
                                   (uint32_t *)&hXvd->uiDecoderDbgBufPhyAddr); 
      
      BXVD_DBG_MSG(hXvd, ("Decoder Debug Log VA:0x%x, PA:0x%x, Size:0x%x", 
                          hXvd->uiDecoderDbgBufVirtAddr, 
                          hXvd->uiDecoderDbgBufPhyAddr, 
                          hXvd->stSettings.uiDecoderDebugLogBufferSize));
   }
   
   BDBG_LEAVE(BXVD_P_SetupFWMemory_RevE0);

   return BERR_TRACE(rc);
}
#endif /* BXVD_P_USE_SETUP_FW_MEMORY_REVE0 */

#ifdef BXVD_P_USE_TEAR_DOWN_FW_MEMORY_REVE0
BERR_Code BXVD_P_TearDownFWMemory_RevE0(BXVD_Handle hXvd)
{
   BDBG_ENTER(BXVD_P_TearDownFWMemory_RevE0);

   BDBG_ASSERT(hXvd);

   if (hXvd->uiFWGenMemVirtAddr != (uint32_t)NULL)
   {
      BMEM_Free(hXvd->hSystemHeap, (void *)hXvd->uiFWGenMemVirtAddr);
   }

   if (hXvd->uiFWCabacMemVirtAddr != (uint32_t)NULL)
   {
      BMEM_Free(hXvd->hCabacHeap, (void *)hXvd->uiFWCabacMemVirtAddr);
   }

   if (hXvd->uiFWPicMemPhyAddr != (uint32_t)NULL)
   {
      BMEM_Free(hXvd->hPictureHeap, (void *)hXvd->uiFWPicMemVirtAddr);
   }
   
   /* We only free FW memory if it has been allocated */
   if (hXvd->bFWMemAllocated) {
      BMEM_Free(hXvd->hCodeHeap, (void *)hXvd->uiFWMemVirtAddr);
   }

   if ( hXvd->uiDecoderDbgBufVirtAddr != (uint32_t)NULL)
   {
      BMEM_Free(hXvd->hSystemHeap, (void *)hXvd->uiDecoderDbgBufVirtAddr);
   }
   
   BDBG_LEAVE(BXVD_P_TearDownFWMemory_RevE0);
   
   return BERR_TRACE(BERR_SUCCESS);
}
#endif /* BXVD_P_USE_TEAR_DOWN_FW_MEMORY_REVE0 */

#if BXVD_P_USE_CORE_CHIP_ENABLE_REVE0

BERR_Code BXVD_P_ChipEnable_RevE0(BXVD_Handle hXvd)
{
   volatile uint32_t uiFWBootStatus;

   BERR_Code rc = BERR_SUCCESS;

#if BXVD_POLL_FW_MBX
   uint32_t uiVal;
   int loopCount;
#endif

   BDBG_ASSERT(hXvd);
   BDBG_ENTER(BXVD_P_ChipEnable_RevE0);

   /* Write to VectorTB, CpuDbg registers and AuxRegs 
    * in THIS ORDER to begin 7401 operation 
    */
        
   /* program the relocation base address for outer-loop */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInstructionBase,
                    hXvd->uiOuterLoopInstructionBase); 
        
   /* program the outer loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterEndOfCode, 
                    hXvd->uiOuterLoopEOC);
        
   /* program the relocation base address for inner-loop */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerInstructionBase,
                    hXvd->uiInnerLoopInstructionBase); 
        
   /* program the inner loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerEndOfCode, 
                    hXvd->uiInnerLoopEOC);
        
   /* program global IO base register - Outer */
   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterGlobalIOBase,
                    BCHP_PHYSICAL_OFFSET);

   /* program global IO base register - Inner */
   BXVD_Reg_Write32(hXvd,
            hXvd->stPlatformInfo.stReg.uiDecode_InnerGlobalIOBase,
            BCHP_PHYSICAL_OFFSET);          

   /* Clear out any previously completed FW command done events */
   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);

   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox,
                    BXVD_P_AVD_CORE_UART_FREQ);

#if BXVD_POLL_FW_MBX

   /* Initialize MBX to non-zero */
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, 0xff);

#endif
        
   /* Boot the core */
   if (hXvd->stSettings.pAVDBootCallback)
   {
      /* Call external boot callback */
      rc = (*hXvd->stSettings.pAVDBootCallback)(hXvd->stSettings.pAVDBootCallbackData,
                                                hXvd->eAVDBootMode);
      if (rc != BERR_SUCCESS) 
      {
         BXVD_DBG_ERR(hXvd, ("Error #%d booting the AVD externally", rc));
         return BERR_TRACE(rc);
      }
   } 
   else 
   {
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       1);
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux,
                       0 ); 
      BXVD_Reg_Write32(hXvd,
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       0);
   }

#define FW_CMD_TIMEOUT 1000

#if !BXVD_POLL_FW_MBX

   rc = BERR_TRACE(BKNI_WaitForEvent(hXvd->stDecoderContext.hFWCmdDoneEvent, FW_CMD_TIMEOUT));

#else

   uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);

   loopCount = 0;
   rc = BERR_TIMEOUT;

   while (loopCount < FW_CMD_TIMEOUT)
   {
      if (uiVal != 0)
      {
         BKNI_Sleep(10); 

         loopCount++;

         uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);
      }
      else 
      {
         rc = BERR_SUCCESS;
         break;
      }
   }

#endif

   /* Read FW boot progress/status from CPU2HostStatus register that was written by FW */
   uiFWBootStatus = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostStatus);

   if(BERR_TIMEOUT == rc)
   {
      BXVD_DBG_ERR(hXvd, ("ARC FW command response timed out, FW Boot Status = %d", uiFWBootStatus));
      return BERR_TRACE(rc);
   }
   else
   {
      BXVD_DBG_MSG(hXvd, ("FW boot successful, FW Boot Status = %d", uiFWBootStatus));
   }

   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);
            
   BDBG_LEAVE(BXVD_P_ChipEnable_RevE0);
   return BERR_TRACE(rc);
}

#endif /* BXVD_P_USE_CORE_CHIP_ENABLE_REVE0 */

#ifdef BXVD_P_USE_CORE_RESET_CHIP_REVE0

BERR_Code BXVD_P_ChipReset_RevE0(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_ChipReset_RevE0);

   /* Stop OL AVD ARC */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                    1);
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux,
                    0x02000000); 

   /* Stop IL AVD ARC */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUDebug,
                    1);
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUAux,
                    0x02000000); 

   /* Reset AVD HW blocks */
   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_CabacBinCtl,
                      hXvd->stPlatformInfo.stReg.uiDecode_CabacBinCtl_ResetMask,
                      "CABAC");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_SintStrmSts,
                      hXvd->stPlatformInfo.stReg.uiDecode_SintStrmSts_ResetMask, 
                      "Stream");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OLSintStrmSts,
                      hXvd->stPlatformInfo.stReg.uiDecode_OLSintStrmSts_ResetMask, 
                      "OLoopStream");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_MainCtl,
                      hXvd->stPlatformInfo.stReg.uiDecode_MainCtl_ResetMask,
                      "Backend" )

   /* Reset Video Decoder */
   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask);

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       0);

   /* Clear interrupt mask (Enable) register to prevent interrupts before ISR is setup */
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask, 0);

   BDBG_LEAVE(BXVD_P_ChipReset_RevE0);

   return BERR_TRACE(BERR_SUCCESS);
}

#endif /* BXVD_P_USE_CORE_CHIP_RESET_REVE0 */

#ifdef BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVE0 

bool BXVD_P_VerifyWatchdogFired_RevE0(BXVD_Handle hXvd,
                     int param2)
{
   volatile uint32_t     uIntrVal;

   volatile uint32_t     uiOL_pc;
   int i;
   
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_VerifyWatchdogFired_RevE0);
   if ( param2 == BXVD_P_OUTER_WATCHDOG)
   {
      uIntrVal = BXVD_Reg_Read32(hXvd, 
                 hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear);

      BXVD_DBG_MSG(hXvd, ("L3IntClr:%08x", uIntrVal));

      if ( uIntrVal & BXVD_P_INTR_OL_MASK)
      {
        BXVD_DBG_MSG(hXvd, ("Outer Loop Watchdog"));
        BXVD_DBG_ERR(hXvd, ("OL watchdog - firmware revision: %x", hXvd->sRevisionInfo.ulDecoderFwRev));

         BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug, 1);

         /* Read FW OL PC to help with debugging watchdog timeouts */
         for (i = 0; i < 8; i++)
         {
             /* read the AVD OL PC */
             uiOL_pc = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux);

             BXVD_DBG_ERR(hXvd, ("[%d] AVD_%d: OL PC=%08x", i, hXvd->uDecoderInstance, uiOL_pc));
         }

     BXVD_Reg_Write32(hXvd,
              hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear,
              uIntrVal);

     BXVD_DBG_MSG(hXvd, ("Clear AVD0_INTR2_L3 intr"));
     /* Clear L2 interrupt, this may be necessary */
     BXVD_Reg_Write32(hXvd,
                          hXvd->stPlatformInfo.stReg.uiAvd_CPUL2InterruptClear,
                          hXvd->stPlatformInfo.stReg.uiAvd_CPUL2InterruptMailboxMask);
      }

      if (!(uIntrVal & BXVD_P_INTR_OL_MASK))
      {
     return false;
      }
   }
   else
   {
      BXVD_DBG_MSG(hXvd, ("Inner Loop Watchdog"));
   }

   BDBG_LEAVE(BXVD_P_VerifyWatchdogFired_RevE0);

   return true;
}

#endif /*  BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVE0 */

#ifdef BXVD_P_USE_CONVERT_UD_OFF2ADDR_REVE0

BERR_Code BXVD_P_ConvertUDOff2Addr_RevE0(BXVD_Userdata_Handle hUserData,
                   unsigned long fwUserDataAddr,
                   unsigned long *pulUserDataAddr)
{
   if (BMEM_ConvertOffsetToAddress(hUserData->hXvdCh->hSystemHeap, 
				   fwUserDataAddr, 
				   (void *)pulUserDataAddr) != BERR_SUCCESS)
   {
      *pulUserDataAddr = 0;
      return BERR_TRACE(BERR_UNKNOWN);
   }
   return BERR_TRACE(BERR_SUCCESS);
}

#endif /* BXVD_P_USE_CONVERT_UD_OFF2ADDR_REVE0 */

#if BXVD_P_USE_SET_POWER_STATE_REVE0

void BXVD_P_SetPowerState_RevE0(BXVD_Handle hXvd,
                                BXVD_P_PowerState PowerStateNew)
{

   /* Check to if PM reg pointers are NULL */
   if ( (hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl == 0) )
   {
      BXVD_DBG_WRN(hXvd, ("XVD Powermanagement is not supported on this platform!"));

      return;
   }

   if (PowerStateNew == BXVD_P_PowerState_ePwrOff)
   {
      /* PwrOff is not supported in this platform */
      PowerStateNew = BXVD_P_PowerState_eClkOff;
   }

   if ((hXvd->PowerStateCurrent == BXVD_P_PowerState_eOn) &&
       (PowerStateNew == BXVD_P_PowerState_eClkOff))
   {
      BXVD_DBG_MSG(hXvd, ("Turn off clocks"));
      
      /* AVD Coreclock */
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl_PwrDnMask,
                          hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl_PwrDnMask);

      if (hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl != (uint32_t)NULL)
      {
         /* Powerdown control for AVD PLL */
         BREG_AtomicUpdate32(hXvd->hReg, 
                             hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl,
                             hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl_PwrDnMask,
                             hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl_PwrDnMask);
      }

      /* PWRDN_CLOCK_216_CG_AVD SCB Memory clock */
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl_PwrDnMask,
                          hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl_PwrDnMask);

      /* PWRDN_CLOCK_108_CG_AVD GISB clock*/
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl_PwrDnMask,
                          hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl_PwrDnMask);

      hXvd->PowerStateCurrent = PowerStateNew;
   }
   else if ((hXvd->PowerStateCurrent == BXVD_P_PowerState_eClkOff) &&
            (PowerStateNew == BXVD_P_PowerState_eOn))
   {
      BXVD_DBG_MSG(hXvd, ("Turn on clocks"));

      /* PWRDN_CLOCK_108_CG_AVD */
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl_PwrDnMask,
                          0);

      /* PWRDN_CLOCK_216_CG_AVD */
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl_PwrDnMask,
                          0);

      if (hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl != (uint32_t)NULL)
      {
         /* Powerdown control for AVD PLL */
         BREG_AtomicUpdate32(hXvd->hReg, 
                             hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl,
                             hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl_PwrDnMask,
                             0);
      }

      /* Wait for PLLs to lock */
      BKNI_Sleep(1);

      /* CLKGEN_AVD_CTRL, Powerdown control for AVD PLL */
      BREG_AtomicUpdate32(hXvd->hReg, 
                          hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl,
                          hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl_PwrDnMask,
                          0);

      hXvd->PowerStateCurrent = PowerStateNew;
   }
}
#endif

#if BXVD_P_USE_SET_POWER_STATE_REVE0_PM2

void BXVD_P_SetPowerState_RevE0_PM2(BXVD_Handle hXvd,
                                    BXVD_P_PowerState PowerStateNew)
{
   BCHP_PWR_ResourceId clkResourceId = BCHP_PWR_RESOURCE_AVD0;

#if BCHP_PWR_RESOURCE_AVD1
   if (hXvd->uDecoderInstance == 1)
   {
      clkResourceId = BCHP_PWR_RESOURCE_AVD1;
   }
#endif

   BDBG_MSG(("BXVD_P_SetPowerState, Decoder:%d Cur: %d New: %d", 
             hXvd->uDecoderInstance, hXvd->PowerStateCurrent , PowerStateNew));

   if (PowerStateNew == BXVD_P_PowerState_ePwrOff)
   {
      /* PwrOff is not supported in this platform */
      PowerStateNew = BXVD_P_PowerState_eClkOff;
   }

   if (hXvd->PowerStateCurrent != PowerStateNew)
   {
      if ((hXvd->PowerStateCurrent == BXVD_P_PowerState_eOn) &&
          (PowerStateNew == BXVD_P_PowerState_eClkOff))
      {
         BDBG_MSG(("Release AVD_Clk Resource"));
         BCHP_PWR_ReleaseResource(hXvd->hChip, clkResourceId);   
      }

      if ((hXvd->PowerStateCurrent != BXVD_P_PowerState_eOn) &&
          (PowerStateNew == BXVD_P_PowerState_eOn))
      {
         BDBG_MSG(("Acquire AVD_CLK Resource"));
         BCHP_PWR_AcquireResource(hXvd->hChip, clkResourceId);   
      }

      hXvd->PowerStateCurrent = PowerStateNew;
   }
}

#endif /* BXVD_P_USE_SET_POWER_STATE_7420 */
