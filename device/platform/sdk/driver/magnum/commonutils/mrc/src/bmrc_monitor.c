/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc_monitor.c $
 * $brcm_Revision: Hydra_Software_Devel/94 $
 * $brcm_Date: 10/1/12 6:29p $
 *
 * Module Description:
 *
 * Implementation of the Realtime Memory Monitor for 7038
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/src/bmrc_monitor.c $
 * 
 * Hydra_Software_Devel/94   10/1/12 6:29p albertl
 * SW7435-238: Optimized clientinfo lookup.
 * 
 * Hydra_Software_Devel/93   9/28/12 9:38p albertl
 * SW7435-238: Fixed memc_id for BMRC_P_Monitor_Program.
 * 
 * Hydra_Software_Devel/91   9/28/12 9:34p albertl
 * SW7435-238: Removed memc_id from debug only.
 * 
 * Hydra_Software_Devel/90   9/28/12 8:16p albertl
 * SW7435-238: Refined low memory protection code and gathered all related
 * debug messages in BMRC_P_Monitor_Update.
 * 
 * Hydra_Software_Devel/89   9/28/12 6:37p albertl
 * SW7435-238: Fixed low memory protection during optimization.
 * 
 * Hydra_Software_Devel/88   9/11/12 2:13a albertl
 * SW7435-10: Fixed 8 ARC support.
 * 
 * Hydra_Software_Devel/87   1/24/12 6:20p albertl
 * SW7425-2184: Fixed warning.
 * 
 * Hydra_Software_Devel/86   1/24/12 6:17p albertl
 * SW7425-2184: Fixed debug message warning for release builds.
 * 
 * Hydra_Software_Devel/85   1/23/12 5:04p albertl
 * SW7425-2184: Fixed incorrect address comparison when removing address
 * from previous addr table.
 * 
 * Hydra_Software_Devel/84   1/20/12 7:07p albertl
 * SW7425-2184: Fixed bug in removing addresses from previous addr table.
 * 
 * Hydra_Software_Devel/83   1/10/12 4:10p albertl
 * SW7405-5349: Updated messages for unknown files for clarity.
 * 
 * Hydra_Software_Devel/82   12/8/11 5:00p albertl
 * SW7408-305: Fixed BMRC_Monitor_JailDefault.
 * 
 * Hydra_Software_Devel/81   9/27/11 6:37p albertl
 * SW7408-305: Fixed optimization to handle overlapping regions.  Defaults
 * to standard update if no optimization found.
 * 
 * Hydra_Software_Devel/80   9/12/11 4:34p albertl
 * SW7346-479: Added asserts to check overflow of region prev array.
 * 
 * Hydra_Software_Devel/79   8/30/11 12:58p albertl
 * SW7405-5349: Fixed to use proper array size in
 * BMRC_P_Monitor_ListRemoveRegion.
 * 
 * Hydra_Software_Devel/78   8/24/11 5:35p albertl
 * SW7405-5349:  Added line parameter to memory monitor interface. Made
 * fixes to optimization code.
 * 
 * Hydra_Software_Devel/77   8/23/11 4:20p albertl
 * SW7405-5349:  Added and adjusted optimization factor constant.
 * 
 * Hydra_Software_Devel/76   8/23/11 3:59p albertl
 * SW7405-5349:  Rolled back monitor interface changes until mastership
 * issues are resolved.
 * 
 * Hydra_Software_Devel/74   8/18/11 7:39p albertl
 * SW7405-5349: Temporarily rolled back optimizations to resolve handling
 * of new violations.
 *
 * Hydra_Software_Devel/73   8/17/11 1:46p albertl
 * SW7405-5349: Made several optimizations to list handling and update to
 * speed up allocs and frees for large numbers of allocations.
 * 
 * Hydra_Software_Devel/72   3/22/11 3:50p albertl
 * SW7344-37: Added changes for SCB Protocal 4.2.  Fixed transfer size
 * message.
 * 
 * Hydra_Software_Devel/71   3/18/11 5:10p albertl
 * SW7422-168: Added mutexes to make BMRC_Monitor calls threadsafe.
 * 
 * Hydra_Software_Devel/70   1/13/11 6:36p albertl
 * SW7422-168: Fixed bug with -1 checkers in user settings.
 * 
 * Hydra_Software_Devel/69   1/13/11 5:20p albertl
 * SW7422-168: Default number of checkers set to -1 in
 * BMRC_Monitor_Settings, handled as maximum available.
 * 
 * Hydra_Software_Devel/68   1/13/11 2:05p albertl
 * SW7422-168: Changed debug messages to BDBG_MSG.
 * 
 * Hydra_Software_Devel/67   1/12/11 2:50p albertl
 * SW7422-168: Changed BMRC_Monitor_Open to take settings structure.
 * Removed BMRC_Monitor_SetSettings().  Added ability to configure how
 * many checkers BMRC_Monitor uses to support more than one monitor on
 * same MEMC.
 * 
 * Hydra_Software_Devel/66   1/5/11 9:02p albertl
 * SW7422-106, SW7422-168 : Fixed MRC bugs pertaining to handling multiple
 * MEMCs and rewrote portions of BMRC_Monitor logic.  Client masks now
 * stored to match hardware.
 * 
 * Hydra_Software_Devel/65   12/29/10 3:18p pntruong
 * SW7422-168: Remove bogus warning msg.
 *
 * Hydra_Software_Devel/64   12/14/10 7:45p albertl
 * SW7400-2973: Disabled memory access blocking temporarily to bypass
 * client permission issues on MEMCs other than 0.
 *
 * Hydra_Software_Devel/63   12/6/10 6:10p albertl
 * SW7405-4047: Removed printf messages.
 *
 * Hydra_Software_Devel/62   12/6/10 4:25p albertl
 * SWBLURAY-23672: Added 7640 support.
 *
 * Hydra_Software_Devel/61   11/19/10 7:56p albertl
 * SW7422-106: Added error message when high memory is not greater than
 * low memory.
 *
 * Hydra_Software_Devel/60   10/7/10 11:31a vsilyaev
 * SW7405-4924: Block write for all out-of-bound accesses
 *
 * Hydra_Software_Devel/59   7/8/10 6:28p albertl
 * SW7405-4047: Fixed build warning with new debug define.
 *
 * Hydra_Software_Devel/58   7/8/10 3:30p albertl
 * SW7405-4047: Removed warning.
 *
 * Hydra_Software_Devel/56   6/30/10 6:11p albertl
 * SW7405-4047: Added BMRC_Monitor_JailByFilename.
 *
 * Hydra_Software_Devel/55   6/16/10 6:23p albertl
 * SW3548-2836: Renamed client field in BMRC_MonitorContext to clients.
 *
 * Hydra_Software_Devel/54   3/29/10 1:00p albertl
 * SW3548-2836: Fixed logic bug in threshold comparison.
 *
 * Hydra_Software_Devel/53   3/26/10 7:22p albertl
 * SW7405-4059: Fixed pointer assignment for empty filenames.
 *
 * Hydra_Software_Devel/52   3/26/10 1:30p albertl
 * SW7405-4059: Updated debug messages to be more helpful.  Included link
 * to SCB specs documentation, also available in this JIRA.
 *
 *
 * Hydra_Software_Devel/51   3/17/10 5:07p erickson
 * SW7405-3892: improve ARC0 warning
 *
 * Hydra_Software_Devel/50   3/17/10 4:46p erickson
 * SW7405-3892: ARC0 now blocks writes to kernel memory
 *
 * Hydra_Software_Devel/49   3/17/10 2:19p erickson
 * SW7405-3892: BDBG_WRN whenever the ARC0 range is reprogrammed
 *
 * Hydra_Software_Devel/48   3/2/10 1:25p erickson
 * SW7405-3892: add BMRC_Monitor_PrintClients for debug, fix
 * BMRC_Monitor_RemoveCustomTag name, add sample code for blocking
 * unwanted kernel writes
 *
 * Hydra_Software_Devel/47   9/14/09 7:43p albertl
 * SW7630-7: Added 7630 support.
 *
 * Hydra_Software_Devel/46   6/16/09 6:21p albertl
 * PR55764, PR55875, PR55993: Refactored MRC to keep chip specific
 * definitions in bmrc_priv.h and for maintainability.  Updated support
 * for 7420 b0, 7430, 35130, and 7550.
 *
 * Hydra_Software_Devel/45   6/4/09 7:05p albertl
 * PR55641: Allowed MRC to report maximum number of checkers and changed
 * BMRC_Monitor to use that instead of hardcoded maximums.
 *
 * Hydra_Software_Devel/44   3/30/09 5:26p albertl
 * PR52965: Fixed missing 7635 MRC support.
 *
 * Hydra_Software_Devel/43   2/17/09 6:43p albertl
 * PR51612: Updated to correct naming conventions.
 *
 * Hydra_Software_Devel/42   7/23/08 1:34p albertl
 * PR33688:  Fixed 7601 mrc build errors.
 *
 * Hydra_Software_Devel/41   6/24/08 6:14p albertl
 * PR43294:  Optimized to run BMRC_P_Monitor_BuildClientIdMaskBits only
 * when necessary for debug messages.
 *
 * Hydra_Software_Devel/40   6/23/08 3:44p albertl
 * PR43294:  Changed client_masks array to be bitfields for more efficient
 * updating.  Previously caused too much delay during updates.
 *
 * Hydra_Software_Devel/39   4/18/08 10:22a mward
 * PR41160:  Don't return without freeing allocated memory.
 *
 * Hydra_Software_Devel/38   4/8/08 6:52p albertl
 * PR41160:  Fixed coverity issue by using heap memory for regions in
 * BMRC_Monitor_Update.
 *
 * Hydra_Software_Devel/37   4/4/08 6:31p albertl
 * PR41261:  Fixed coverity issue by initializing hChecker to NULL.
 *
 * Hydra_Software_Devel/36   2/25/08 7:04p albertl
 * PR36876:  Rewrote MRC to abstract client names and streamline adding of
 * future chips.  3548 support added.
 *
 * Hydra_Software_Devel/35   2/14/08 6:34p albertl
 * PR35322:  Fixed BMRC_P_Monitor_RemoveMask client mask bug that caused
 * JailRemove to fail.
 *
 * Hydra_Software_Devel/34   11/14/07 1:50p albertl
 * PR35322:  Updated mrc monitor to properly support 128 clients.
 *
 * Hydra_Software_Devel/32   3/9/07 7:19p albertl
 * PR28182:  No longer programs ARC when aligned size is 0.  Memc id now
 * reported on violations.
 *
 * Hydra_Software_Devel/31   2/7/07 7:39p albertl
 * PR25898:  Changed BMRC_MONITOR range checking to JWord (256 byte)
 * granularity to eliminate false violations from clients with JWord
 * minimum access sizes.
 *
 * Hydra_Software_Devel/30   2/1/07 12:54p mward
 * PR26646: Temporarily disable messages for GFX client for 7118, too, as
 * for 7401/7403.
 *
 * Hydra_Software_Devel/29   1/17/07 7:56p albertl
 * PR19101:  Fixed missing defines for viol_start and viol_end.
 *
 * Hydra_Software_Devel/28   1/17/07 6:39p albertl
 * PR19101:  Updated violation messages to indicate exclusive mode for
 * clarity.
 *
 * Hydra_Software_Devel/27   12/28/06 9:42a erickson
 * PR25898: temp checkin for release. the false error messages were
 * getting in the way of system debug. please remove this change when the
 * issue is fixed.
 *
 * Hydra_Software_Devel/26   11/27/06 7:23p albertl
 * PR13641:  Updated violation messages for clarity.
 *
 * Hydra_Software_Devel/25   10/26/06 2:39p syang
 * PR13641: fix compile error (aligned_end not defined)
 *
 * Hydra_Software_Devel/24   10/25/06 4:15p albertl
 * PR13641:  Corrected calculation of aligned_size.
 *
 * Hydra_Software_Devel/23   10/24/06 10:54p albertl
 * PR24997:  Fixed error calculating aligned starting address.
 *
 * Hydra_Software_Devel/21   10/19/06 3:13p albertl
 * PR25004:  Fixed improper splitting of macros with #if.
 *
 * Hydra_Software_Devel/20   10/13/06 9:27p albertl
 * PR23361:  Added mrc support for 3563.
 *
 * Hydra_Software_Devel/19   10/13/06 8:12p albertl
 * PR19101:   NMBX value now returned upon violation.
 *
 * Hydra_Software_Devel/18   6/16/06 3:29p albertl
 * PR20489:  Start addresses are now aligned in BMRC_P_Monitor_Program.
 *
 * Hydra_Software_Devel/17   4/11/06 2:51p albertl
 * PR20489:  Fixed bug updating incorrect client list with defaults.
 *
 * Hydra_Software_Devel/16   4/3/06 4:25p albertl
 * PR20489:  Added BMRC_Monitor_JailDefault function.
 *
 * Hydra_Software_Devel/15   3/7/06 6:36p albertl
 * PR19498:  Added SCB Command decoding in error messages, and NMB is
 * printed only when relevant.  Checker ranges no longer cover extra
 * bytes that exceed 8 byte size alignment.
 *
 * Hydra_Software_Devel/14   3/3/06 7:23p albertl
 * PR18701:  Added functions to disable and enable a checker's callback.
 * Fixed to disable a checker's interrupt upon violation.
 *
 * Hydra_Software_Devel/13   3/1/06 5:26p albertl
 * PR18701:  Updated to include RMM updates.
 *
 * Hydra_Software_Devel/12   2/16/06 7:39p albertl
 * PR19101:  Moved chip specific tables to bmrc_monitor_clients.c.
 *
 * Hydra_Software_Devel/11   1/17/06 4:53p hongtaoz
 * PR19082: support 7400;
 *
 * Hydra_Software_Devel/10   11/29/05 1:20p albertl
 * PR18307:  BMRC_Monitor_Open now immediately returns with an error when
 * BMRC_Checker_Create fails.
 *
 * Hydra_Software_Devel/9   11/7/05 5:05p albertl
 * PR17403:  Fixed build errors on 97401.  Renamed clients to be
 * consistent with RTS architecture client documentation.
 *
 * Hydra_Software_Devel/8   11/4/05 6:49p albertl
 * PR17403:  Added BMRC_Client_eAVD_ILA_0 and BMRC_Client_eAVD_OLA_0
 * clients to xpt module.  Renamed client enums to standard names.
 *
 * Hydra_Software_Devel/7   10/7/05 4:36p hongtaoz
 * PR17460: adapted to the new RDB header files;
 *
 * Hydra_Software_Devel/6   9/23/05 7:06p hongtaoz
 * PR17131: added BMRC support for 7401;
 *
 * Hydra_Software_Devel/5   8/8/05 9:52p albertl
 * PR13641:  Fixed debug module name.
 *
 * Hydra_Software_Devel/4   8/4/05 6:50p albertl
 * PR13641:  Fixed incorrect PR numbers in chagelog.
 *
 * Hydra_Software_Devel/2   8/3/05 8:34p albertl
 * PR13641:  Removed shifting of addresses.  Now handled in BMRC_Checker
 * module.
 *
 * Hydra_Software_Devel/1   8/1/05 10:14p albertl
 * PR13641:  Initial Revision, based on RMM module.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bmrc_monitor.h"
#include "bmrc_monitor_priv.h"
#include "bkni.h"
#include "bkni_multi.h"          /* for semaphores */
#include "blst_list.h"

BDBG_MODULE(BMRC_MONITOR);

#if (BCHP_CHIP == 7038) || (BCHP_CHIP == 7438) || (BCHP_CHIP == 3560)
    #define BMRC_P_MONITOR_USE_NMBX_ID 1
#else
    #define BMRC_P_MONITOR_USE_NMBX_ID 0
#endif
#define BMRC_P_MONITOR_MAX_RANGES 8
#define BMRC_P_MONITOR_CLIENT_MASK_ARRAY_ELEMENT_SIZE 32
#define BMRC_P_MONITOR_CLIENT_MASK_ARRAY_SIZE  (BMRC_Client_eMaxCount + (BMRC_P_MONITOR_CLIENT_MASK_ARRAY_ELEMENT_SIZE-1)) / BMRC_P_MONITOR_CLIENT_MASK_ARRAY_ELEMENT_SIZE

#define BMRC_P_MONITOR_DBG_TRACE 0

#if BMRC_P_MONITOR_DBG_TRACE
#define BDBG_MSG_TRACE(x) BDBG_MSG(x)
#else
#define BDBG_MSG_TRACE(x)
#endif

/* externs */
extern const BMRC_P_Monitor_FileClientInfo BMRC_P_Monitor_astFileClients[];
extern const uint32_t BMRC_P_Monitor_FileClientsTblSize;
extern const BMRC_Client BMRC_P_Monitor_astHwClients[];

/* Definitions for word sizes.

    Dword (or Word)  32 bits
    Oword  64 bits
    Gword  128 bits
    Jword  256 bits (J for Jumbo, if you like.)
*/
#define BMRC_P_MONITOR_DWORD_BYTES 4
#define BMRC_P_MONITOR_OWORD_BYTES 8
#define BMRC_P_MONITOR_GWORD_BYTES 16
#define BMRC_P_MONITOR_JWORD_BYTES 32

#if (BCHP_CHIP!=7440) && (BCHP_CHIP!=7601) && (BCHP_CHIP!=7635) && (BCHP_CHIP!=7630) && (BCHP_CHIP!=7640)
/* SCB Protocol Specifications the following table was derived from are available at
   http://www.blr.broadcom.com/projects/DVT_BLR/Memc_Arch/.  These specs are also used
   in BMRC_P_Monitor_Dump_isr below. */

/* SCB Command Table */
static const BMRC_P_Monitor_ScbCommandInfo g_ScbCommandInfoTbl[] =
{
    {BMRC_P_Monitor_ScbCommand_eLR,   BMRC_P_MONITOR_SCB_COMMAND_LR,   BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK,   "Linear Read - LR"},
    {BMRC_P_Monitor_ScbCommand_eLW,   BMRC_P_MONITOR_SCB_COMMAND_LW,   BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK,   "Linear Write - LW"},
    {BMRC_P_Monitor_ScbCommand_eREF,  BMRC_P_MONITOR_SCB_COMMAND_REF,  BMRC_P_MONITOR_SCB_INTERNAL_MASK,          "Refresh - REF"},
    {BMRC_P_Monitor_ScbCommand_eMRS,  BMRC_P_MONITOR_SCB_COMMAND_MRS,  BMRC_P_MONITOR_SCB_INTERNAL_MASK,          "Mode Register Set - MRS"},
    {BMRC_P_Monitor_ScbCommand_eEMRS, BMRC_P_MONITOR_SCB_COMMAND_EMRS, BMRC_P_MONITOR_SCB_INTERNAL_MASK,          "Extended Mode Reg Set - EMRS"},
    {BMRC_P_Monitor_ScbCommand_ePALL, BMRC_P_MONITOR_SCB_COMMAND_PALL, BMRC_P_MONITOR_SCB_INTERNAL_MASK,          "Precharge All Banks - PALL"},
    {BMRC_P_Monitor_ScbCommand_eDR,   BMRC_P_MONITOR_SCB_COMMAND_DR,   BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK,   "Video Raster Read - DR"},
    {BMRC_P_Monitor_ScbCommand_eDW,   BMRC_P_MONITOR_SCB_COMMAND_DW,   BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK,   "Video Raster Write - DW"},
    {BMRC_P_Monitor_ScbCommand_eMR,   BMRC_P_MONITOR_SCB_COMMAND_MR,   BMRC_P_MONITOR_SCB_MPEG_BLOCK_ACCESS_MASK, "MPEG Block Read - MR"},
    {BMRC_P_Monitor_ScbCommand_eMW,   BMRC_P_MONITOR_SCB_COMMAND_MW,   BMRC_P_MONITOR_SCB_MPEG_BLOCK_ACCESS_MASK, "MPEG Block Write - MW"},
    {BMRC_P_Monitor_ScbCommand_eCR4,  BMRC_P_MONITOR_SCB_COMMAND_CR4,  BMRC_P_MONITOR_SCB_CACHE_ACCESS_MASK,      "Cache Read, 4 Jwords/128 bytes - CR4"},
    {BMRC_P_Monitor_ScbCommand_eCR8,  BMRC_P_MONITOR_SCB_COMMAND_CR8,  BMRC_P_MONITOR_SCB_CACHE_ACCESS_MASK,      "Cache Read, 8 Jwords/256 bytes - CR8"}
};
#define BMRC_P_MONITOR_SCB_ACCESS_TABLE_SIZE (sizeof(g_ScbCommandInfoTbl)/sizeof(BMRC_P_Monitor_ScbCommandInfo))
#endif /* (BCHP_CHIP!=7440) && (BCHP_CHIP!=7601) && (BCHP_CHIP!=7635) */

#define BMRC_P_MONITOR_ALIGNED_RANGE_START(start_addr, exclusive) \
    ((start_addr + (exclusive ? 0 : ~(BMRC_P_MONITOR_CHECKER_ADDR_ALIGN))) & \
     BMRC_P_MONITOR_CHECKER_ADDR_ALIGN)

#define BMRC_P_MONITOR_ALIGNED_RANGE_END(end_addr, exclusive) \
    ((end_addr + (exclusive ? ~(BMRC_P_MONITOR_CHECKER_ADDR_ALIGN) : 0)) & \
     BMRC_P_MONITOR_CHECKER_ADDR_ALIGN)

#define BMRC_P_MONITOR_CLIENT_MASK(client_id)      (1 << ((client_id) % BMRC_P_MONITOR_CLIENT_MASK_ARRAY_ELEMENT_SIZE))
#define BMRC_P_MONITOR_CLIENT_MASK_IDX(client_id)  ((client_id) / BMRC_P_MONITOR_CLIENT_MASK_ARRAY_ELEMENT_SIZE)
#define BMRC_P_MONITOR_CLIENT_MASK_IS_SET(client_mask, client_id) \
    client_mask[BMRC_P_MONITOR_CLIENT_MASK_IDX(client_id)] & BMRC_P_MONITOR_CLIENT_MASK(client_id)
#define BMRC_P_MONITOR_CLIENT_MASK_SET(client_mask, client_id) \
    client_mask[BMRC_P_MONITOR_CLIENT_MASK_IDX(client_id)] |= BMRC_P_MONITOR_CLIENT_MASK(client_id);
#define BMRC_P_MONITOR_CLIENT_MASK_UNSET(client_mask, client_id) \
    client_mask[BMRC_P_MONITOR_CLIENT_MASK_IDX(client_id)] &= ~BMRC_P_MONITOR_CLIENT_MASK(client_id);

#define BMRC_P_MONITOR_OPTIMIZE_THRESHOLD 100
#define BMRC_P_MONITOR_OPTIMIZE_SKIP_FACTOR 10
#define BMRC_P_MONITOR_REGION_HASH_SIZE       (9999)  /* odd to avoid possible alignment issues spoiling hash */
#define BMRC_P_MONITOR_REGION_PREV_SIZE       (10000)
#define BMRC_P_MONITOR_REGION_PREV_SLOT_SIZE  (hMonitor->mem_high / BMRC_P_MONITOR_REGION_PREV_SIZE)
#define BMRC_P_MONITOR_REGION_PREV_GET_IDX(addr) ((((addr) / BMRC_P_MONITOR_REGION_PREV_SLOT_SIZE) >= BMRC_P_MONITOR_REGION_PREV_SIZE) ? \
                                                  (BMRC_P_MONITOR_REGION_PREV_SIZE - 1) : ((addr) / BMRC_P_MONITOR_REGION_PREV_SLOT_SIZE))


/* default settings */
static const BMRC_Monitor_Settings s_stDefaultSettings = {
    BMRC_AccessType_eWrite,  /* kernel violation access block */
	BMRC_AccessType_eNone,   /* regular violation access block */
	UINT32_C(-1)             /* maximum number of checkers to use. -1 means use all available. */
};

static void BMRC_P_Monitor_Update(BMRC_Monitor_Handle hMonitor, bool bCombine);
static void BMRC_P_Monitor_isr( void *cnxt, int no, BMRC_CheckerInfo *pInfo);
BERR_Code BMRC_Monitor_ValidateRegionList(BMRC_Monitor_Handle hMonitor);
BERR_Code BMRC_Monitor_DumpRegionPrevByAddrTable(BMRC_Monitor_Handle hMonitor);

typedef struct BMRC_Monitor_Clients {
    uint32_t client_masks[BMRC_P_MONITOR_CLIENT_MASK_ARRAY_SIZE];
}BMRC_Monitor_Clients;


typedef struct BMRC_Monitor_Region {
	const char *fname;
	int line;
    BLST_D_ENTRY(BMRC_Monitor_Region) list;
	BLST_D_ENTRY(BMRC_Monitor_Region) hash;
    uint32_t addr;
    size_t size;
    BMRC_Monitor_Clients clients;
    BMRC_Monitor_Clients user_clients_allowed;
}BMRC_Monitor_Region;

typedef struct BMRC_P_MonitorContext {
    BMRC_Settings mrcSettings;
    BMRC_Monitor_Settings stSettings;
    BMRC_Handle mrc;
    BREG_Handle reg;
    BINT_Handle isr;
    uint32_t mem_low, mem_high; /* memory region what would be monitored */
    uint32_t max_ranges; /* maximum number of checkers */
	uint32_t max_regions;
	uint32_t num_regions;
	uint32_t num_combined_regions;
    BLST_D_HEAD(parent, BMRC_Monitor_Region) regions; /* list of all known memory regions, sorted by their adreess, from low to high */
	BLST_D_HEAD(hash_parent, BMRC_Monitor_Region) region_hash[BMRC_P_MONITOR_REGION_HASH_SIZE];
    BLST_D_HEAD(custom_parent, BMRC_Monitor_Region) custom_regions; /* list of exclusive regions */
	BMRC_Monitor_Region *combined_regions;
	BMRC_Monitor_Region *region_prev_by_addr[BMRC_P_MONITOR_REGION_PREV_SIZE];
    BMRC_Monitor_Clients clients; /* current clients used to setup range checker */
	BMRC_ClientInfo client_infos[BMRC_Client_eMaxCount];
    BINT_CallbackHandle irq[BMRC_P_MONITOR_MAX_RANGES];
    BMRC_Monitor_Clients user_clients_jailed;
    BMRC_Checker_Handle ahCheckers[BMRC_P_MONITOR_MAX_RANGES];
	bool protect_mem_low;
	BKNI_MutexHandle pMutex; /* Semaphore to lock this monitor */
}BMRC_P_MonitorContext;


/* client mask manipulation functions */
static void
BMRC_P_Monitor_MasksClear(BMRC_Monitor_Clients *mask)
{
    BKNI_Memset(mask->client_masks, 0, sizeof(mask->client_masks));
}

static void
BMRC_P_Monitor_MasksFill(BMRC_Monitor_Clients *mask)
{
    BKNI_Memset(mask->client_masks, 0xFFFFFFFF, sizeof(mask->client_masks));
}


static void
BMRC_P_Monitor_MasksInverse(BMRC_Monitor_Clients *mask, const BMRC_Monitor_Clients *neg_mask)
{
    int i = 0;
    for (i=0;i<BMRC_P_MONITOR_CLIENT_MASK_ARRAY_SIZE;i++)
        mask->client_masks[i] = ~neg_mask->client_masks[i];
}


static bool
BMRC_P_Monitor_MasksIsClear(const BMRC_Monitor_Clients *mask)
{
    int i = 0;
    for (i=0;i<BMRC_P_MONITOR_CLIENT_MASK_ARRAY_SIZE;i++)
        if (mask->client_masks[i])
            return false;
    return true;
}


static bool
BMRC_P_Monitor_MasksHasCommon(const BMRC_Monitor_Clients *mask, const BMRC_Monitor_Clients *cmp_mask)
{
    int i = 0;
    for (i=0;i<BMRC_P_MONITOR_CLIENT_MASK_ARRAY_SIZE;i++)
        if (mask->client_masks[i] & cmp_mask->client_masks[i])
            return true;
    return false;
}


static void
BMRC_P_Monitor_MaskAdd(BMRC_Monitor_Clients *mask, BMRC_Client client)
{
    BMRC_P_MONITOR_CLIENT_MASK_SET(mask->client_masks, client);
}


static void
BMRC_P_Monitor_MaskRemove(BMRC_Monitor_Clients *mask, BMRC_Client client)
{
    BMRC_P_MONITOR_CLIENT_MASK_UNSET(mask->client_masks, client);
}


/* this function builds a client map using given client list */
static void
BMRC_P_Monitor_MaskBuild(BMRC_Monitor_Clients *mask, const BMRC_Client *clients)
{
    /* clear clients, and then get clients from the table */
    BMRC_P_Monitor_MasksClear(mask);

    for(;*clients!=BMRC_Client_eMaxCount;clients++) {
        BMRC_P_Monitor_MaskAdd(mask, *clients);
    }
    return;
}


/* this function builds the hardware client id bit mask */
static void
BMRC_P_Monitor_BuildClientIdMaskBits(BMRC_Monitor_Handle hMonitor, const BMRC_Monitor_Clients *mask, uint32_t mask_bits[4])
{
    int i;
    BKNI_Memset(mask_bits, 0xFFFFFFFF, sizeof(uint32_t) * 4);
    for (i = 0; i < BMRC_Client_eMaxCount; i++)
    {
        BMRC_ClientInfo *client_info = &hMonitor->client_infos[i];

        if ((client_info->usClientId != BMRC_Client_eInvalid) &&
            !(BMRC_P_MONITOR_CLIENT_MASK_IS_SET(mask->client_masks, i)))
        {
            BMRC_P_MONITOR_CLIENT_MASK_UNSET(mask_bits, client_info->usClientId);
        }
    }
}

/* this function builds the hardware client id bit string */
const char *BMRC_P_Monitor_BuildClientIdString(BMRC_Monitor_Handle hMonitor, const BMRC_Monitor_Clients *mask, char *buffer, uint32_t len)
{
	uint32_t mask_bits[4];
	BMRC_P_Monitor_BuildClientIdMaskBits(hMonitor, mask, mask_bits);
	BKNI_Snprintf(buffer, len, "%#.8x %#.8x %#.8x %#.8x", mask_bits[3], mask_bits[2], mask_bits[1], mask_bits[0]);

	return buffer;
}


BMRC_Monitor_Region *BMRC_P_Monitor_ListGetPrevRegionByAddr(BMRC_Monitor_Handle hMonitor, uint32_t addr)
{
	int32_t prev_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(addr - 1);
	BMRC_Monitor_Region *prev;
	BMRC_Monitor_Region *cur;

	prev = hMonitor->region_prev_by_addr[prev_idx];

	while((!prev || (prev->addr >= addr)) && (prev_idx > 0))
	{
		prev = hMonitor->region_prev_by_addr[--prev_idx];
	}

	if(!prev)
	{
		return NULL;
	}

	cur = BLST_D_NEXT(prev, list);

	while(cur && (cur->addr < addr))
	{
		prev = cur;
		cur = BLST_D_NEXT(prev, list);
	}

	return prev;
}

void BMRC_P_Monitor_ListAddRegion(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_Region *region)
{
	BMRC_Monitor_Region *prev = BMRC_P_Monitor_ListGetPrevRegionByAddr(hMonitor, region->addr);
	BMRC_Monitor_Region *next;
	uint32_t cur_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(region->addr);
	uint32_t prev_idx = 0;
	uint32_t next_idx = BMRC_P_MONITOR_REGION_PREV_SIZE; /* if no next, next would be just outside of array */
	uint32_t i;

    if (prev) {
        BLST_D_INSERT_AFTER(&hMonitor->regions, prev, region, list);
		prev_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(prev->addr);
    } else {
        BLST_D_INSERT_HEAD(&hMonitor->regions, region, list);
    }

	next = BLST_D_NEXT(region, list);

	if(next)
	{
		next_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(next->addr);
	}

	BDBG_ASSERT(next_idx <= BMRC_P_MONITOR_REGION_PREV_SIZE);

	/* now we add current region's address to previous address map */

	/* region is lowest in this address slot */
	if(!prev || (prev_idx < cur_idx))
	{
		hMonitor->region_prev_by_addr[cur_idx] = region;
	}

	/* mark all addresses between start of current and next region with current region as previous */
	for (i = cur_idx+1; i < next_idx; i++)
	{
		hMonitor->region_prev_by_addr[i] = region;
	}
}

void BMRC_P_Monitor_ListRemoveRegion(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_Region *region)
{
	BMRC_Monitor_Region *prev = BMRC_P_Monitor_ListGetPrevRegionByAddr(hMonitor, region->addr);
	BMRC_Monitor_Region *next = BLST_D_NEXT(region, list);
	uint32_t cur_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(region->addr);
	uint32_t prev_idx = 0;
	uint32_t next_idx = BMRC_P_MONITOR_REGION_PREV_SIZE;
	uint32_t i;

	BLST_D_REMOVE(&hMonitor->regions, region, list);

	if(prev)
	{
		prev_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(prev->addr);
	}

	if(next)
	{
		next_idx = BMRC_P_MONITOR_REGION_PREV_GET_IDX(next->addr);
	}

	BDBG_ASSERT(next_idx <= BMRC_P_MONITOR_REGION_PREV_SIZE);

	/* now we remove current region's address from previous address map */

	/* update if current is same as removed region */
	if ((uint32_t)hMonitor->region_prev_by_addr[cur_idx]->addr == (uint32_t)region->addr)
	{
		/* replace current with previous if appropriate, or with next otherwise */
		if(((prev_idx == cur_idx) && prev) || !next || (next_idx > cur_idx))
		{
			hMonitor->region_prev_by_addr[cur_idx] = prev;
		}
		else
		{
			hMonitor->region_prev_by_addr[cur_idx] = next;
		}
	}

	/* replace all addresses between start of current and next region with previous */
	for (i = cur_idx+1; i < next_idx; i++)
	{
		hMonitor->region_prev_by_addr[i] = prev;
	}

}

BMRC_Monitor_Region *BMRC_P_Monitor_HashGetRegionByAddr(BMRC_Monitor_Handle hMonitor, uint32_t addr)
{
	uint32_t hash_idx = addr % BMRC_P_MONITOR_REGION_HASH_SIZE;
    BMRC_Monitor_Region *cur;

    for(cur=BLST_D_FIRST(&hMonitor->region_hash[hash_idx]);cur;cur=BLST_D_NEXT(cur, hash))
	{
		if(cur->addr == addr)
		{
			return cur;
		}
	}

	return NULL;
}

void BMRC_P_Monitor_HashAddRegion(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_Region *region)
{
	uint32_t hash_idx = region->addr % BMRC_P_MONITOR_REGION_HASH_SIZE;
    BLST_D_INSERT_HEAD(&hMonitor->region_hash[hash_idx], region, hash);

}

void BMRC_P_Monitor_HashRemoveRegion(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_Region *region)
{
	uint32_t hash_idx = region->addr % BMRC_P_MONITOR_REGION_HASH_SIZE;
	BLST_D_REMOVE(&hMonitor->region_hash[hash_idx], region, hash);
}


BERR_Code
BMRC_Monitor_Open(BMRC_Monitor_Handle *phMonitor, BREG_Handle hReg, BINT_Handle hIsr, BCHP_Handle hChp, BMRC_Handle hMrc, uint32_t ulMemLow, uint32_t ulMemHigh, BMRC_Monitor_Settings *pSettings)
{
    BMRC_Monitor_Handle hMonitor;
    BERR_Code rc;
    unsigned i;
	uint32_t max_checkers;

    BSTD_UNUSED(hChp);
    BDBG_ASSERT(hReg);
    BDBG_ASSERT(hIsr);
    BDBG_ASSERT(hChp);

    if (ulMemLow >= ulMemHigh)
    {
        BDBG_ERR(("High memory address must be greater than low memory address."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    hMonitor = (BMRC_Monitor_Handle) BKNI_Malloc(sizeof(BMRC_P_MonitorContext));
    if (!hMonitor) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

	BMRC_GetMaxCheckers(hMrc, &max_checkers);
	if (max_checkers > BMRC_P_MONITOR_MAX_RANGES)
	{
		BDBG_WRN(("BMRC_P_MONITOR_MAX_RANGES less than actual checkers available. Max checkers: %d", max_checkers));
		BDBG_WRN(("Setting max checkers to BMRC_P_MONITOR_MAX_RANGES."));
		max_checkers = BMRC_P_MONITOR_MAX_RANGES;
	}

	if (pSettings)
	{
		if ((pSettings->ulNumCheckersToUse != UINT32_C(-1)) &&
			(pSettings->ulNumCheckersToUse > max_checkers))
		{
			rc = BERR_INVALID_PARAMETER;
			BDBG_ERR(("Not enough checkers available. Num checkers: %d, Max checkers: %d",
				      pSettings->ulNumCheckersToUse, max_checkers));
			BKNI_Free(hMonitor);
			return rc;
		}

		hMonitor->stSettings = *pSettings;
	}
	else
	{
	    hMonitor->stSettings = s_stDefaultSettings;
	}

	/* create mutex for re-entrant control */
	rc = BERR_TRACE(BKNI_CreateMutex(&(hMonitor->pMutex)));
	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to create mutex."));
		BKNI_Free(hMonitor);
		return rc;
	}

    hMonitor->mrc = hMrc;
    BMRC_GetSettings(hMonitor->mrc, &hMonitor->mrcSettings);
    hMonitor->reg = hReg;
    hMonitor->isr = hIsr;
    hMonitor->mem_high = ulMemHigh;
    hMonitor->mem_low = ulMemLow;
	hMonitor->max_ranges = (hMonitor->stSettings.ulNumCheckersToUse == UINT32_C(-1)) ?
						   max_checkers : hMonitor->stSettings.ulNumCheckersToUse;
	hMonitor->max_regions = hMonitor->max_ranges+1;
	hMonitor->combined_regions = BKNI_Malloc(sizeof(BMRC_Monitor_Region) * hMonitor->max_regions);
	hMonitor->num_combined_regions = 0;
	hMonitor->num_regions = 0;

	/* populate clientinfo array */
    for (i = 0; i < BMRC_Client_eMaxCount; i++)
    {
        BMRC_Checker_GetClientInfo(hMonitor->mrc, i, &(hMonitor->client_infos[i]));
    }

    BLST_D_INIT(&hMonitor->regions);
    BLST_D_INIT(&hMonitor->regions);
    BLST_D_INIT(&hMonitor->custom_regions);
	for (i=0; i<BMRC_P_MONITOR_REGION_HASH_SIZE; i++)
	{
	    BLST_D_INIT(&hMonitor->region_hash[i]);
	}

	BKNI_Memset(hMonitor->region_prev_by_addr, (uint32_t)NULL, sizeof(hMonitor->region_prev_by_addr));

    BMRC_P_Monitor_MaskBuild(&hMonitor->clients, BMRC_P_Monitor_astHwClients);
	/* inverse because we want clients mask bits to be on for allowed clients to match hardware */
    BMRC_P_Monitor_MasksInverse(&hMonitor->clients, &hMonitor->clients);
    BMRC_P_Monitor_MasksClear(&hMonitor->user_clients_jailed);
	hMonitor->protect_mem_low = false;

    for (i=0;i<hMonitor->max_ranges;i++) {
        BMRC_Checker_Handle hChecker = NULL;

        rc = BMRC_Checker_Create(hMrc, &hChecker);
        if (rc!=BERR_SUCCESS) {
            BKNI_Free(hMonitor);
			BDBG_ERR(("Out of checkers on checker create."));
            return rc;
        }

        rc = BMRC_Checker_SetCallback(hChecker, BMRC_P_Monitor_isr, hMonitor, i);
        if (rc!=BERR_SUCCESS) {
            BKNI_Free(hMonitor);
            return rc;
        }

        rc = BMRC_Checker_EnableCallback(hChecker);
        if (rc!=BERR_SUCCESS) {
            BKNI_Free(hMonitor);
            return rc;
        }

        hMonitor->ahCheckers[i] = hChecker;
    }

    BMRC_P_Monitor_Update(hMonitor, true);

    *phMonitor = hMonitor;

    return BERR_SUCCESS;
}

void
BMRC_Monitor_Close(BMRC_Monitor_Handle hMonitor)
{
    BMRC_Monitor_Region *region;
    unsigned i;
    BERR_Code rc;

    for (i=0;i<hMonitor->max_ranges;i++) {
        rc = BMRC_Checker_Destroy(hMonitor->ahCheckers[i]);
        if (rc!=BERR_SUCCESS) {
            BDBG_ERR(("BMRC_Checker_Destroy returned error %#x, ignored"));
        }
    }

	for (i=0;i<BMRC_P_MONITOR_REGION_HASH_SIZE; i++)
	{
	    while (NULL != (region = BLST_D_FIRST(&hMonitor->region_hash[i]))) {
		    BLST_D_REMOVE_HEAD(&hMonitor->region_hash[i], hash);
		}
    }

    while (NULL != (region = BLST_D_FIRST(&hMonitor->regions))) {
        BLST_D_REMOVE_HEAD(&hMonitor->regions, list);
        BKNI_Free(region);
    }

	BKNI_Free(hMonitor->combined_regions);

	/* destroy mutex */
	BKNI_DestroyMutex(hMonitor->pMutex);
    BKNI_Free(hMonitor);
    return;
}

BERR_Code
BMRC_Monitor_GetDefaultSettings(BMRC_Monitor_Settings *pDefSettings)
{
    if (!pDefSettings)
    {
        return BERR_INVALID_PARAMETER;
    }

    *pDefSettings = s_stDefaultSettings;

    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_GetSettings(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_Settings *pSettings)
{
    if (!pSettings)
    {
        return BERR_INVALID_PARAMETER;
    }

    *pSettings = hMonitor->stSettings;

    return BERR_SUCCESS;
}

/* cheap version of toupper */
#define B_TOUPPER(x) ((x)&(~0x20))

/* return true if client found */
static bool
BMRC_P_Monitor_GetClientsByFname(const char *fname, BMRC_Monitor_Clients *clients)
{
    const char *ptr;
    unsigned i,j;

    if (fname==NULL) { /* filename is unknown */
        goto not_found;
    }

    for(ptr=fname;*ptr!='\0';ptr++) { } /* scan to the end of string */

    for(;*ptr!='\\' && *ptr!='/';ptr--) { /* back track to found \ or / */
        if (ptr==fname) { /* no directory */
            goto no_dir;
        }
    }
    ptr++;
no_dir:
    for(fname = ptr, i=0;i<BMRC_P_Monitor_FileClientsTblSize;i++) {
        for(j=0;B_TOUPPER(BMRC_P_Monitor_astFileClients[i].prefix[j])==B_TOUPPER(fname[j]);) { /* do strncmp type of stuff */
            j++;
            if (BMRC_P_Monitor_astFileClients[i].prefix[j]=='\0') {
                /* bingo!!! found match */
				BDBG_MSG_TRACE(("allocation from %s matched to %s clients", fname, BMRC_P_Monitor_astFileClients[i].prefix));
                BMRC_P_Monitor_MaskBuild(clients, BMRC_P_Monitor_astFileClients[i].clients);
                return true;
            }
        }
    }
    BDBG_MSG_TRACE(("allocation from %s not matched to known file, access allowed for all HW clients", fname));
not_found:
	/* allow all clients to access memory allocated by unknown files. */
    BMRC_P_Monitor_MasksFill(clients);

    return false;
}


static void
BMRC_P_Monitor_Alloc(void *cnxt, uint32_t addr, size_t size, const char *fname, int line)
{
    BMRC_Monitor_Handle hMonitor = cnxt;
    BMRC_Monitor_Region *region;
	BMRC_Monitor_Clients jailed_clients;
	BMRC_Monitor_Clients clients;
    bool found;
	uint32_t i;

	static uint32_t iSkipCount = 0;

    BDBG_MSG_TRACE(("alloc: addr %#x size %#x file %s", (unsigned)addr, (unsigned)size, fname?fname:""));

    found = BMRC_P_Monitor_GetClientsByFname(fname, &clients);
    
    region = BKNI_Malloc(sizeof(*region));
    region->clients = clients;
    
    BMRC_P_Monitor_MasksClear(&region->user_clients_allowed);
    region->addr = addr;
    region->size = size;
	region->fname = fname;
	region->line = line;

	BKNI_AcquireMutex(hMonitor->pMutex);

	BMRC_P_Monitor_HashAddRegion(hMonitor, region);
	BMRC_P_Monitor_ListAddRegion(hMonitor, region);
	hMonitor->num_regions++;

/* validation code */
#if 0
	BMRC_Monitor_ValidateRegionList(hMonitor);
/*	
	BMRC_Monitor_DumpRegionPrevByAddrTable(hMonitor);
*/
#endif

/* optimization code */
#if 1
	/* inverse to get jailed clients */
	BMRC_P_Monitor_MasksInverse(&jailed_clients, &hMonitor->clients);

	if (BMRC_P_Monitor_MasksHasCommon(&region->clients, &jailed_clients))
	{
		/* optimize when number of regions get large */
		if ((hMonitor->num_combined_regions > 0) &&
			(hMonitor->num_regions >= BMRC_P_MONITOR_OPTIMIZE_THRESHOLD) &&
			(iSkipCount < (hMonitor->num_regions/BMRC_P_MONITOR_OPTIMIZE_THRESHOLD) * BMRC_P_MONITOR_OPTIMIZE_SKIP_FACTOR))
		{
			iSkipCount++;

			/* don't need full update if new region fits in existing regions */
			for (i=0; i < hMonitor->num_combined_regions; i++)
			{
				if ((region->addr >= hMonitor->combined_regions[i].addr) &&
					(region->addr + region->size <= hMonitor->combined_regions[i].addr + hMonitor->combined_regions[i].size))
				{
					goto done;
				}
			}

			/* grow expanded regions, simplified for speed when dealing with many regions */
			for (i = 0; i <= hMonitor->num_combined_regions; i++)
			{
				uint32_t prev_region_start = (i==0)? 0 : hMonitor->combined_regions[i-1].addr;
				uint32_t prev_region_end   = (i==0)? 0 : ((hMonitor->combined_regions[i-1].addr + hMonitor->combined_regions[i-1].size) - 1);
				uint32_t cur_region_start  = (i==hMonitor->num_combined_regions)? 0xFFFFFFFF : hMonitor->combined_regions[i].addr;
				uint32_t cur_region_end    = (i==hMonitor->num_combined_regions)? 0xFFFFFFFF : ((hMonitor->combined_regions[i].addr + hMonitor->combined_regions[i].size) - 1);
				uint32_t region_start = region->addr;
				uint32_t region_end = region->addr + region->size - 1;

				if (((region_start >= prev_region_start) && (region_end > prev_region_end) && (region_end < cur_region_start - 1)) ||
					((region_start > prev_region_end + 1) && (region_start < cur_region_start) && (region_end <= cur_region_end)))
				{
					if ((i > 0) &&
						((i == hMonitor->num_combined_regions) ||
						 (region_start <= prev_region_end) || /* new region starts in prev */
						 (region_start - (prev_region_end + 1)) < (cur_region_start - (region_end + 1))))  /* new region is closer to prev or cur region? */
					{
						/* add to end of previous region */
						hMonitor->combined_regions[i-1].size = (region_start + region->size) - hMonitor->combined_regions[i-1].addr;
					}
					else 
					{
						/* add to front of current region */
						hMonitor->combined_regions[i].size += hMonitor->combined_regions[i].addr - region_start;
						hMonitor->combined_regions[i].addr = region_start;
					}
					BMRC_P_Monitor_Update(hMonitor, false);
					goto done;
				}
			}
			BDBG_MSG(("New region not combined with any existing region."));
		}

		BMRC_P_Monitor_Update(hMonitor, true);
		iSkipCount = 0;
	}

#else
	BMRC_P_Monitor_Update(hMonitor, true);
#endif

done:
	BKNI_ReleaseMutex(hMonitor->pMutex);

    return;
}

static void
BMRC_P_Monitor_Free(void *cnxt, uint32_t addr)
{
    BMRC_Monitor_Handle hMonitor = cnxt;
    BMRC_Monitor_Region *region;
	static uint32_t iSkipCount = 0;

    BDBG_MSG_TRACE(("free: addr %#x", (unsigned)addr));

	BKNI_AcquireMutex(hMonitor->pMutex);

	region = BMRC_P_Monitor_HashGetRegionByAddr(hMonitor, addr);
	if (region)
	{
		BMRC_P_Monitor_HashRemoveRegion(hMonitor, region);
		BMRC_P_Monitor_ListRemoveRegion(hMonitor, region);
		BKNI_Free(region);
		hMonitor->num_regions--;

/* validation code */
#if 0
		BMRC_Monitor_ValidateRegionList(hMonitor);
/*
		BMRC_Monitor_DumpRegionPrevByAddrTable(hMonitor);
*/
#endif
	
		if ((hMonitor->num_regions < BMRC_P_MONITOR_OPTIMIZE_THRESHOLD) ||
			(iSkipCount >= (hMonitor->num_regions/BMRC_P_MONITOR_OPTIMIZE_THRESHOLD) * BMRC_P_MONITOR_OPTIMIZE_SKIP_FACTOR))
		{
		    BMRC_P_Monitor_Update(hMonitor, true);
			iSkipCount = 0;
		}
		else
		{
			iSkipCount++;
		}
	}
	else
	{
		BDBG_ERR(("Trying to free unknown block"));
	}

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return;
}


/* this function combines all memory regions, what are accessible to the given clients, into the set of continious regions, using selected threshold_ */
static bool
BMRC_P_Monitor_Combine(BMRC_Monitor_Handle hMonitor, const BMRC_Monitor_Clients *clients, uint32_t threshold, BMRC_Monitor_Region *regions, unsigned max_regions, unsigned *num_regions)
{
    int i;
    BMRC_Monitor_Region *cur;
#if BMRC_P_MONITOR_DBG_TRACE
    char buffer[256];
#endif

	for(i=-1, cur=BLST_D_FIRST(&hMonitor->regions);cur;cur=BLST_D_NEXT(cur, list))
	{
		/* check if clients exist in region */
        if ( !(BMRC_P_Monitor_MasksHasCommon(&cur->clients, clients))) {
			/* this region not matched */
            continue;
        }

        BDBG_MSG_TRACE(("region(%d) %p %s addr %#x size %#x", i, cur, BMRC_P_Monitor_BuildClientIdString(hMonitor, &cur->clients, mask_bits), cur->addr, cur->size, buffer, sizeof(buffer)));

        if (i<0) {
            regions[0] = *cur;
            i=0;
        } else {
            BDBG_MSG_TRACE(("region(%d) (%#x..%#x) <-> (%#x..%#x)", i, cur->addr, cur->addr+cur->size, regions[i].addr, regions[i].addr+regions[i].size+threshold));
			/* join two regions */
#if 0
			next = BMRC_P_Monitor_ListGetPrevRegionByAddr(hMonitor, regions[i].addr + regions[i].size + threshold);
			if(next && (next->addr+next->size > regions[i].addr+regions[i].size))
			{
				regions[i].size = (next->addr + next->size) - regions[i].addr;
				cur = next;
#else
            if ((regions[i].addr + regions[i].size+threshold) >= cur->addr ) {
                regions[i].size = (cur->addr + cur->size) - regions[i].addr;
#endif
            } else { /* create new region */
                i++;
                if ((unsigned)i>=max_regions) { /* too many regions */
                    *num_regions=(unsigned)i;
                    return false;
                }
                regions[i] = *cur;
            }
        }
    }
    *num_regions = i+1;

#if 0
	/* useful for seeing state of combined regions */
	{
		unsigned i;
		char buffer[256];
		BDBG_WRN(("Combined: max_regions %d, threshold 0x%x", max_regions, threshold));
		for(i=0; i < *num_regions; i++){
			BDBG_WRN(("          region %d, %x..%x, clients %s", i, regions[i].addr, regions[i].addr + regions[i].size,
				BMRC_P_Monitor_BuildClientIdString(hMonitor, &regions[i].clients, buffer, sizeof(buffer))));
		}
	}
#endif

    return true;
}

static void
BMRC_P_Monitor_Disable(BMRC_Monitor_Handle hMonitor, unsigned arc_no)
{
    BMRC_Checker_Handle hChecker = hMonitor->ahCheckers[arc_no];
    BMRC_Checker_Disable(hChecker);
    return;
}

static void
BMRC_P_Monitor_Program(BMRC_Monitor_Handle hMonitor, unsigned arc_no, uint32_t addr, size_t size, const BMRC_Monitor_Clients *clients, bool exclusive)
{
    BMRC_Checker_Handle hChecker = hMonitor->ahCheckers[arc_no];
    BMRC_Client client_id;
    uint32_t aligned_addr;
    uint32_t aligned_end;
    uint32_t aligned_size;
#if BDBG_DEBUG_BUILD
	uint32_t memc_id = hMonitor->mrcSettings.usMemcId;
#endif

    /* disable read and write check first */
    BMRC_Checker_Disable(hChecker);

    BDBG_MSG(("MEMC%d: programming ARC %u with %#x...%#x", memc_id, arc_no, (unsigned) addr, (unsigned) (addr+size-1)));
#if 0
    /* useful for seeing client state when programming */
    {
		char buffer[256];
		BDBG_WRN(("ARC %d %s", arc_no, BMRC_P_Monitor_BuildClientIdString(hMonitor, clients, buffer, sizeof(buffer))));
    }
#endif

    /*
    ** Program up to, but do not include, the last address in the range. In some cases, calls to alloc() will
    ** will return memory that starts at that last, protected address. Accesses to that address could be a
    ** violation. In the 7038/3560, a bug in the ARC allowed those violations to go undetected. That bug is
    ** fixed in the 7401. See PR 13737.
    */

    aligned_addr = BMRC_P_MONITOR_ALIGNED_RANGE_START(addr, exclusive);
    aligned_end = BMRC_P_MONITOR_ALIGNED_RANGE_END(addr + size, exclusive);

 /* alignment may have changed starting or end addresses, size adjusted */
    aligned_size = aligned_end - aligned_addr;

    if (!aligned_size)
    {
        /* size is 0, arc not enabled */
        return;
    }

    BMRC_Checker_SetRange(hChecker, aligned_addr, aligned_size);

    /* program clients */

    for (client_id = 0; client_id < BMRC_Client_eMaxCount; client_id++)
    {
        BMRC_ClientInfo *client_info = &hMonitor->client_infos[client_id];

		if (client_info->usClientId != BMRC_Client_eInvalid)
        {
            (BMRC_P_MONITOR_CLIENT_MASK_IS_SET(clients->client_masks, client_id)) ?
                BMRC_Checker_SetClient(hChecker, (BMRC_Client)client_id, BMRC_AccessType_eBoth) :
                BMRC_Checker_SetClient(hChecker, (BMRC_Client)client_id, BMRC_AccessType_eNone);
        }
    }

    /* activate range checker */
    BMRC_Checker_SetAccessCheck(hChecker, BMRC_AccessType_eBoth);

    /*
    By default, block mode is set to BMRC_AccessType_eWrite for all memory violations.

    BMRC_AccessType_eWrite prevents memory corruption by HW cores that shouldn't access it.
    If you believe write access was blocked in error, please inspect BMRC_P_Monitor_astHwClients[] in bmrc_monitor_clients.c,
    BMRC_P_astClientTbl[] in bmrc_clienttable_priv.c

    Read will not be blocked, but you will see a BDBG_ERR if a violation occurs.

    This can be configured separately for kernel and non-kernel memory to block only reads, only writes, both, or neither
    when a violation occurs with the BMRC_Monitor_Settings structure used in BMRC_Monitor_Open().
    */

	/* assuming kernel is only on MEMC 0 */
    if (hMonitor->mrcSettings.usMemcId == 0 && arc_no == 0 && hMonitor->protect_mem_low)
    {
        BMRC_Checker_SetBlock(hChecker, hMonitor->stSettings.eKernelBlockMode);
    }
    else
    {
        BMRC_Checker_SetBlock(hChecker, hMonitor->stSettings.eBlockMode);
    }

    BMRC_Checker_SetExclusive(hChecker, exclusive);
    BMRC_Checker_Enable(hChecker);

    return;
}

/* Monitor checker error messages are based on the SCB Protocol specifications at
   http://www.blr.broadcom.com/projects/DVT_BLR/Memc_Arch/. */
#if (BCHP_CHIP==7440) || (BCHP_CHIP==7601) || (BCHP_CHIP==7635) || (BCHP_CHIP==7630) || (BCHP_CHIP==7640)
static void
BMRC_P_Monitor_Dump_isr(BMRC_Monitor_Handle hMonitor, unsigned arc_no, BMRC_CheckerInfo *pCheckerInfo)
{
    uint32_t viol_start;
    uint32_t start, size;

    BSTD_UNUSED(hMonitor);
    BSTD_UNUSED(arc_no);

    start = pCheckerInfo->ulStart;
    size = pCheckerInfo->ulSize;
    viol_start = pCheckerInfo->ulAddress;

    BDBG_ERR(("Address Range Checker %d (ARC%d) has detected a memory access violation in MEMC%d", arc_no, arc_no, pCheckerInfo->usMemcId));

    if (pCheckerInfo->bExclusive)
    {
        BDBG_ERR(("violating access outside of exclusive range: %#x..%#x", (unsigned)start, (unsigned)(start+size)));
    }
    else
    {
        BDBG_ERR(("violation access in prohibited range: %#x..%#x", (unsigned)start, (unsigned)(start+size)));
    }

    BDBG_ERR(("violation start address: %#x", (unsigned)viol_start));
    BDBG_ERR(("violation client: %d(%s)", pCheckerInfo->usClientId, pCheckerInfo->pchClientName));
    BDBG_ERR(("transfer length:  %d", pCheckerInfo->ulLength));

    switch(pCheckerInfo->ulMode)
    {
    case BMRC_P_MONITOR_CCB_LINEAR_ACCESS:
        BDBG_ERR(("request type:     0x%03x(Linear Access)", pCheckerInfo->ulMode));
        break;

    case BMRC_P_MONITOR_CCB_CACHE_ACCESS:
        BDBG_ERR(("request type:     0x%03x(Cache Access)", pCheckerInfo->ulMode));
        break;

    case BMRC_P_MONITOR_CCB_DISPLAY_ACCESS:
        BDBG_ERR(("request type:     0x%03x(Display Access)",  pCheckerInfo->ulMode));
        break;

    default:
        BDBG_ERR(("request type:     0x%03x(Unknown Command Type)", pCheckerInfo->ulMode));
        break;
    }
}
#else

static void
BMRC_P_Monitor_Dump_isr(BMRC_Monitor_Handle hMonitor, unsigned arc_no, BMRC_CheckerInfo *pCheckerInfo)
{
    uint32_t viol_start, viol_end;
    uint32_t start, size;
    BMRC_P_Monitor_ScbCommand eScbCommand = BMRC_P_Monitor_ScbCommand_eUnknown;
    uint32_t i = 0;
	BMRC_Monitor_Region *cur;

    BSTD_UNUSED(hMonitor);
    BSTD_UNUSED(arc_no);

    start = pCheckerInfo->ulStart;
    size = pCheckerInfo->ulSize;
    viol_start = pCheckerInfo->ulAddress;
    viol_end = pCheckerInfo->ulAddressEnd;

    BDBG_ERR(("Address Range Checker %d (ARC%d) has detected a memory access violation in MEMC%d", arc_no, arc_no, pCheckerInfo->usMemcId));

    if (pCheckerInfo->bExclusive)
    {
        BDBG_ERR(("violating access outside of exclusive range: %#x..%#x", (unsigned)start, (unsigned)(start+size-1)));
    }
    else
    {
        BDBG_ERR(("violation access in prohibited range: %#x..%#x", (unsigned)start, (unsigned)(start+size-1)));
    }

    BDBG_ERR(("violation start address: %#x", (unsigned)viol_start));
#if BMRC_CHECKER_USE_NEW_NAME_SUFFIX
    BDBG_ERR(("violation end address:   %#x", (unsigned)viol_end));
#endif

    for (i = 0; i < BMRC_P_MONITOR_SCB_ACCESS_TABLE_SIZE; i++)
    {
        if ((pCheckerInfo->ulReqType & g_ScbCommandInfoTbl[i].ulMask) == g_ScbCommandInfoTbl[i].ulCommand)
        {
            eScbCommand = g_ScbCommandInfoTbl[i].eScbCommand;
            break;
        }
    }

    if (eScbCommand == BMRC_P_Monitor_ScbCommand_eUnknown)
    {
        BDBG_ERR(("violation client: %d(%s)  request type: 0x%03x(Unknown Command Type)", pCheckerInfo->usClientId, pCheckerInfo->pchClientName,  pCheckerInfo->ulReqType));
        return;
    }

    BDBG_ERR(("violation client: %d(%s)  request type: 0x%03x(%s)", pCheckerInfo->usClientId, pCheckerInfo->pchClientName,  pCheckerInfo->ulReqType, g_ScbCommandInfoTbl[i].pName));

    switch(eScbCommand)
    {
    uint32_t ulTransferSize, ulXSize, ulYLines;
    bool bFrameAccess;

    case BMRC_P_Monitor_ScbCommand_eLR:
    case BMRC_P_Monitor_ScbCommand_eLW:
        ulTransferSize = (pCheckerInfo->ulReqType & BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MASK);
		ulTransferSize = (ulTransferSize == 0)? BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MAX : ulTransferSize;

        BDBG_ERR(("transfer length %d bytes (%d Jwords)",
                 ulTransferSize * BMRC_P_MONITOR_JWORD_BYTES, ulTransferSize));
        break;

    case BMRC_P_Monitor_ScbCommand_eDR:
    case BMRC_P_Monitor_ScbCommand_eDW:
        ulTransferSize = (pCheckerInfo->ulReqType & BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MASK);
        ulTransferSize = (ulTransferSize == 0)? BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MAX : ulTransferSize;
#if BMRC_P_MONITOR_USE_NMBX_ID
        BDBG_ERR(("transfer length %d bytes (%d Gwords) %d NMBX_ID %d NMBX %d",
                 ulTransferSize * BMRC_P_MONITOR_GWORD_BYTES, ulTransferSize,
                 pCheckerInfo->ulNmbxId, pCheckerInfo->ulNmbx));
#else
        BDBG_ERR(("transfer length %d butes (%d Gwords) %d NMBX %d",
                 ulTransferSize * BMRC_P_MONITOR_GWORD_BYTES, ulTransferSize,
                 pCheckerInfo->ulNmbx));
#endif
        break;

    case BMRC_P_Monitor_ScbCommand_eMR:
    case BMRC_P_Monitor_ScbCommand_eMW:
        ulXSize      = (pCheckerInfo->ulReqType & BMRC_P_MONITOR_SCB_MPEG_X_BIT)? 3: 2;
        ulYLines     = (pCheckerInfo->ulReqType & BMRC_P_MONITOR_SCB_MPEG_Y_MASK);
		ulYLines     = (ulYLines == 0)? BMRC_P_MONITOR_SCB_YLINES_MAX : ulYLines;
        bFrameAccess = (pCheckerInfo->ulReqType & BMRC_P_MONITOR_SCB_MPEG_T_BIT);

#if BMRC_P_MONITOR_USE_NMBX_ID
        BDBG_ERR(("X:%d bytes (%d Owords) Y:%d lines T:%s NMBX_ID %d NMBX %d",
                 ulXSize * BMRC_P_MONITOR_OWORD_BYTES, ulXSize, ulYLines,
                 bFrameAccess ? "frame access" : "field access",
                 pCheckerInfo->ulNmbxId, pCheckerInfo->ulNmbx));
#else
        BDBG_ERR(("X:%d bytes (%d Owords) Y:%d lines T:%s NMBX %d",
                 ulXSize * BMRC_P_MONITOR_OWORD_BYTES, ulXSize, ulYLines,
                 bFrameAccess ? "frame access" : "field access",
                 pCheckerInfo->ulNmbx));
#endif
        break;

    default:
        break;
    }
	
	for(cur=BLST_D_FIRST(&hMonitor->regions);cur;cur=BLST_D_NEXT(cur, list))
	{
		if (((viol_start >= cur->addr) && (viol_start <= (cur->addr + cur->size-1))) ||
			((viol_end >= cur->addr) && (viol_end <= (cur->addr + cur->size-1))))
		{
			BDBG_ERR(("violated region: 0x%x..0x%x,", cur->addr, cur->addr + cur->size-1, cur->fname, cur->line));
			BDBG_ERR(("allocated by %s:%d", cur->fname, cur->line));
		}
	}

#if 0
    /* useful for seeing state of all regions at interrupt time */
    {
        int i;
        char buffer[256];
        const BMRC_Monitor_Region *cur;
        for(i=0,cur=BLST_D_FIRST(&hMonitor->regions);cur;cur=BLST_D_NEXT(cur, list)) {
            BDBG_WRN(("region %d, %x..%x, clients %s", i++, cur->addr, cur->addr + cur->size,
            BMRC_P_Monitor_BuildClientIdString(hMonitor, &cur->clients, buffer, sizeof(buffer))));
        }
    }
#endif

    return;
}
#endif /* (BCHP_CHIP==7440) || (BCHP_CHIP==7601) || (BCHP_CHIP==7635) */

/* this function updates current range checker programming based on current memory map and jail */
static void
BMRC_P_Monitor_Update(BMRC_Monitor_Handle hMonitor, bool bCombine)
{
    BMRC_Monitor_Region *combined_regions = hMonitor->combined_regions;
    bool success = false;
    uint32_t threshold=16;
    unsigned num_regions = hMonitor->num_combined_regions;
	unsigned max_regions = hMonitor->max_regions;
    unsigned i, j;
    uint32_t addr, size;
    unsigned last, first = 0;
	uint32_t memc_id = hMonitor->mrcSettings.usMemcId;

	BMRC_Monitor_Clients jailed_clients;

	/* inverse to get jailed clients */
	BMRC_P_Monitor_MasksInverse(&jailed_clients, &hMonitor->clients);

    for(i=0;i<hMonitor->max_ranges;i++) {
        BMRC_P_Monitor_Disable(hMonitor, i);
    }

    if (BLST_D_FIRST(&hMonitor->custom_regions)) {
        const BMRC_Monitor_Region *region;

        BDBG_MSG_TRACE(("use custom ARC mapping"));
        for(i=0,region=BLST_D_FIRST(&hMonitor->custom_regions);region;i++,region=BLST_D_NEXT(region,list)) {
            uint32_t size = BMRC_P_MONITOR_ALIGNED_RANGE_END(region->addr+region->size, false) -
							BMRC_P_MONITOR_ALIGNED_RANGE_START(region->addr, false);

            if (size == 0)
            {
                BDBG_WRN(("aligned size %#x is 0, ARC %u will not be enabled", size, i));
            }
            BMRC_P_Monitor_Program(hMonitor, i, region->addr, region->size, &hMonitor->clients, false);
        }
        goto done;
    }

	/* using first ARC to disable hardware access to low memory if necessary */
	if (combined_regions[0].addr - hMonitor->mem_low > 0)
	{
		hMonitor->protect_mem_low = true;
		first++;
	}
	else
	{
		hMonitor->protect_mem_low = false;
	}

	if(bCombine)
	{
		for(;threshold<hMonitor->mem_high - hMonitor->mem_low;threshold<<=1) {
			success = BMRC_P_Monitor_Combine(hMonitor, &jailed_clients, threshold, combined_regions, max_regions, &num_regions);

			BDBG_MSG_TRACE(("combine %u regions status %d(%#x)", num_regions, (int)success, threshold));

			for(i=0;i<num_regions;i++) {
				BDBG_MSG_TRACE(("region %d addr=%#x size=%#x", i, (unsigned)combined_regions[i].addr, (unsigned)combined_regions[i].size));
			}

			if (!success) {
				continue;
			}

			if (((num_regions-1)+first <= hMonitor->max_ranges) || (num_regions==0)) {
				break;
			}
			else
			{
				BDBG_MSG_TRACE(("not enough ranges. regions: %d, max_ranges %d, first %d\n", num_regions, hMonitor->max_ranges, first));
			}
		}
	}

#if 0
	for(i=0; i < num_regions; i++){
		BDBG_WRN(("region %d, %x..%x", i, combined_regions[i].addr, combined_regions[i].addr + combined_regions[i].size));
	}
#endif

	if (num_regions)
	{
		static uint32_t low_size = 0, high_size = 0; /* These are safe non-const statics. It is used for diagnostics only. */

		/* using first ARC to disable hardware access to low memory if necessary */
		if (hMonitor->protect_mem_low)
		{
			addr = hMonitor->mem_low;
			size = combined_regions[0].addr - hMonitor->mem_low;

			/* assuming kernel is only on MEMC 0 */
			if (memc_id == 0)
			{
				if (addr == hMonitor->mem_low) 
				{
					/* Ensure that the console shows when ARC0 is protecting the kernel. See below for protection code. */
					if(size != low_size)
					{
						low_size = size;
						BDBG_WRN(("MEMC%d: ARC0 is protecting low memory for range %#x...%#x (%d MB)", memc_id, hMonitor->mem_low, combined_regions[0].addr-1, size/1024/1024));
					}
					else
					{
						/* BMEM grows head toward high addresses, therefore BDBG_WRN causes to much noise */
			            BDBG_MSG(("MEMC%d: ARC0 is protecting low memory for range %#x...%#x (%d MB)", memc_id, hMonitor->mem_low, combined_regions[0].addr-1, size/1024/1024));
					}
				}
				else if (addr+size == hMonitor->mem_high && size != high_size) 
				{
					high_size = size;
					BDBG_MSG(("MEMC%d: ARC0 is protecting high memory for range %#x...%#x (%d MB)", memc_id, hMonitor->mem_low, combined_regions[0].addr-1, size/1024/1024));
				}
			}
			BMRC_P_Monitor_Program(hMonitor, 0, addr, size, &hMonitor->clients, false);
		}

		/* using last ARC to disable hardware access to high memory */
		last = hMonitor->max_ranges - 1;
		addr = (combined_regions[num_regions-1].addr + combined_regions[num_regions-1].size);

		BDBG_MSG_TRACE(("addr %#x mem_high %#x", addr, hMonitor->mem_high));
		if (addr+threshold < hMonitor->mem_high)
		{
			size = hMonitor->mem_high - addr;
            BDBG_MSG(("MEMC%d: ARC%d is protecting high memory for range %#x...%#x (%d MB)", memc_id, last, addr, addr+size-1, size/1024/1024)); /* BMEM grows head toward high addresses, therefore  BDBG_WRN causes to much noise */
			BMRC_P_Monitor_Program(hMonitor, last, addr, size, &hMonitor->clients, false);
			last--;
		}
		
		/* use other range checkers to program all regions which are not in the map.
		   we actually program to protect the memory in between regions using same client
		   list that protects kernel memory and high memory.  */
		for(i=first, j=0;i<=last && j<num_regions-1;i++, j++) {
			addr = combined_regions[j].addr + combined_regions[j].size;
			BMRC_P_Monitor_Program(hMonitor, i, addr, combined_regions[j+1].addr - addr, &hMonitor->clients, false);
		}

		hMonitor->num_combined_regions = num_regions;

        BDBG_MSG_TRACE(("use custom ARC mapping, threshold %#x", threshold));
	}
	else
	{
	    BDBG_MSG_TRACE(("use standard ARC mapping"));
	    /* using first ARC to disable hardware access to all memory */
		BMRC_P_Monitor_Program(hMonitor, 0, hMonitor->mem_low, hMonitor->mem_high - hMonitor->mem_low, &hMonitor->clients, false);
	}

done:
    return;
}

BERR_Code
BMRC_Monitor_GetMemoryInterface(BMRC_Monitor_Handle hMonitor, BMEM_MonitorInterface *pInterface)
{
    pInterface->cnxt = hMonitor;
    pInterface->alloc = BMRC_P_Monitor_Alloc;
    pInterface->free = BMRC_P_Monitor_Free;

    return BERR_SUCCESS;
}

static void
BMRC_P_Monitor_isr( void *cnxt, int no, BMRC_CheckerInfo *pInfo)
{
    BMRC_Monitor_Handle hMonitor = cnxt;
    BERR_Code rc;

    rc = BMRC_Checker_DisableCallback_isr(hMonitor->ahCheckers[no]);
    if (rc !=BERR_SUCCESS) {
        BDBG_ERR(("BMRC_Checker_DisableCallback_isr failed with rc %#x, ignored", (unsigned)rc));
    }
    BMRC_P_Monitor_Dump_isr(hMonitor, no, pInfo);

    return;
}

BERR_Code
BMRC_Monitor_JailDefault(BMRC_Monitor_Handle hMonitor)
{
    BMRC_Monitor_Clients clients;

	BKNI_AcquireMutex(hMonitor->pMutex);

	/* BMRC_P_Monitor_astHwClients are default allowed. inverse is default jailed. */
    BMRC_P_Monitor_MaskBuild(&clients, BMRC_P_Monitor_astHwClients);
	BMRC_P_Monitor_MasksInverse(&hMonitor->user_clients_jailed, &clients);

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_JailByFilename(BMRC_Monitor_Handle hMonitor, const char *filename)
{
    BMRC_Monitor_Clients clients;

	BKNI_AcquireMutex(hMonitor->pMutex);

    BMRC_P_Monitor_GetClientsByFname(filename, &clients);
    hMonitor->user_clients_jailed = clients;

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_JailAdd(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_MemoryClientId client)
{
	BKNI_AcquireMutex(hMonitor->pMutex);

	BMRC_P_Monitor_MaskAdd(&hMonitor->user_clients_jailed, client);

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_JailRemove(BMRC_Monitor_Handle hMonitor, BMRC_Monitor_MemoryClientId client)
{
	BKNI_AcquireMutex(hMonitor->pMutex);

	BMRC_P_Monitor_MaskRemove(&hMonitor->user_clients_jailed, client);

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_JailUpdate(BMRC_Monitor_Handle hMonitor)
{
    unsigned i;
    BERR_Code rc;

	BKNI_AcquireMutex(hMonitor->pMutex);

	if (BMRC_P_Monitor_MasksIsClear(&hMonitor->user_clients_jailed)) {
        BDBG_MSG_TRACE(("all clients were removed from the jail, using standard configuration"));
        BMRC_P_Monitor_MaskBuild(&hMonitor->clients, BMRC_P_Monitor_astHwClients);
    } else {
		/* inverse because we want clients mask bits to be on for allowed clients to match hardware */
		BMRC_P_Monitor_MasksInverse(&hMonitor->clients, &hMonitor->user_clients_jailed);
    }
    BMRC_P_Monitor_Update(hMonitor, true);

    BDBG_MSG_TRACE(("[re]enabling RMM interrupts"));
    for (i=0;i<hMonitor->max_ranges;i++) {
        rc = BMRC_Checker_EnableCallback(hMonitor->ahCheckers[i]);
        if (rc!=BERR_SUCCESS) {
			BKNI_ReleaseMutex(hMonitor->pMutex);
            return rc;
        }
    }

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_BlockTag(BMRC_Monitor_Handle hMonitor, uint32_t ulAddr, size_t size, BMRC_Monitor_MemoryClientId client)
{
    BMRC_Monitor_Region *region;

    BSTD_UNUSED(size);

	BKNI_AcquireMutex(hMonitor->pMutex);

	for(region = BLST_D_FIRST(&hMonitor->regions);region;region=BLST_D_NEXT(region, list)) {
        if (region->addr == ulAddr) {
            BMRC_P_Monitor_MaskAdd(&region->user_clients_allowed, client);
            region->clients = region->user_clients_allowed;
            BMRC_P_Monitor_Update(hMonitor, true);

			BKNI_ReleaseMutex(hMonitor->pMutex);
            return BERR_SUCCESS;
        }
    }

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}

BERR_Code
BMRC_Monitor_AddCustomTag(BMRC_Monitor_Handle hMonitor, uint32_t ulAddr, size_t size, BMRC_Monitor_MemoryClientId client)
{
    BMRC_Monitor_Region *region;
    unsigned i;

	BKNI_AcquireMutex(hMonitor->pMutex);

	for(i=0,region = BLST_D_FIRST(&hMonitor->custom_regions);region;region=BLST_D_NEXT(region, list),i++) {
        if (ulAddr == region->addr && size == region->size) {
            /* bingo */
            goto add_client;
        }
        if ( (ulAddr >= region->addr && ulAddr <= region->addr+region->size) ||
             (ulAddr+size >= region->addr && ulAddr+size <= region->addr+region->size)) {
            /* detected overlap */

			BKNI_ReleaseMutex(hMonitor->pMutex);
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }
    if (i>=hMonitor->max_ranges) {

		BKNI_ReleaseMutex(hMonitor->pMutex);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    region = BKNI_Malloc(sizeof(*region));
    if (!region) {

		BKNI_ReleaseMutex(hMonitor->pMutex);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BMRC_P_Monitor_MasksClear(&region->user_clients_allowed);

    region->addr = ulAddr;
    region->size = size;
    BLST_D_INSERT_HEAD(&hMonitor->custom_regions, region, list);
add_client:
    BMRC_P_Monitor_MaskAdd(&region->user_clients_allowed, client);
	region->clients = region->user_clients_allowed;

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_SUCCESS;
}

BERR_Code
BMRC_Monitor_RemoveCustomTag(BMRC_Monitor_Handle hMonitor, uint32_t ulAddr, size_t size,  BMRC_Monitor_MemoryClientId client)
{
    BMRC_Monitor_Region *region;

	BKNI_AcquireMutex(hMonitor->pMutex);

	for(region=BLST_D_FIRST(&hMonitor->custom_regions);region;region=BLST_D_NEXT(region, list)) {
        if (ulAddr == region->addr && size == region->size) {
            BMRC_P_Monitor_MaskRemove(&region->user_clients_allowed, client);
			region->clients = region->user_clients_allowed;

            if (BMRC_P_Monitor_MasksIsClear(&region->user_clients_allowed)) {
                BLST_D_REMOVE(&hMonitor->regions, region, list);
            }

			BKNI_ReleaseMutex(hMonitor->pMutex);
            return BERR_SUCCESS;
        }
    }
    /* block is nowhere to found */

	BKNI_ReleaseMutex(hMonitor->pMutex);
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}

BERR_Code
BMRC_Monitor_PrintClients(BMRC_Monitor_Handle hMonitor, uint32_t clients0, uint32_t clients1, uint32_t clients2, uint32_t clients3)
{
    BMRC_Client client_id;
    BKNI_Printf("Clients: ");
    for (client_id = 0; client_id < BMRC_Client_eMaxCount; client_id++)
    {
        BMRC_ClientInfo *client_info = &hMonitor->client_infos[client_id];

		if (client_info->usClientId != BMRC_Client_eInvalid)
        {
            bool set;
            if (client_info->usClientId < 32) {
                set = (clients0 & (1<<client_info->usClientId));
            }
            else if (client_info->usClientId < 64) {
                set = (clients1 & (1<<(client_info->usClientId-32)));
            }
            else if (client_info->usClientId < 96) {
                set = (clients2 & (1<<(client_info->usClientId-64)));
            }
            else {
                set = (clients3 & (1<<(client_info->usClientId-96)));
            }
            if (set) {
                BKNI_Printf("%s(%d) \n", client_info->pchClientName, client_info->usClientId);
            }
        }
    }
    BKNI_Printf("\n");
    return 0;
}

BERR_Code
BMRC_Monitor_ValidateRegionList(BMRC_Monitor_Handle hMonitor)
{
	/* validation code */
	uint32_t prev_addr = 0;
	BMRC_Monitor_Region *cur, *prev;

    for(cur=BLST_D_FIRST(&hMonitor->regions);cur;cur=BLST_D_NEXT(cur, list)) {
		if (prev_addr >= cur->addr)
		{
			BKNI_Printf("regions out of order.  prev: 0x%x cur: 0x%x\n", prev_addr, cur->addr);
		}

		prev_addr = cur->addr;
		prev = BMRC_P_Monitor_ListGetPrevRegionByAddr(hMonitor, cur->addr);
		if(prev && (prev->addr >= cur->addr))
		{
			BKNI_Printf("prev is bad. prev: 0x%x cur: 0x%x\n", prev->addr, cur->addr);
		}
	}

	return 0;
}

BERR_Code
BMRC_Monitor_DumpRegionPrevByAddrTable(BMRC_Monitor_Handle hMonitor)
{
	uint32_t prev_addr = 0;
	int i;

	BKNI_Printf("slot size %d\n", BMRC_P_MONITOR_REGION_PREV_SLOT_SIZE);
	BKNI_Printf("prev size %d\n", BMRC_P_MONITOR_REGION_PREV_SIZE);

	for (i = 0; i < BMRC_P_MONITOR_REGION_PREV_SIZE; i++)
	{
		if (hMonitor->region_prev_by_addr[i] && (uint32_t)hMonitor->region_prev_by_addr[i] != (uint32_t)prev_addr)
		{
			if (hMonitor->region_prev_by_addr[i])
			{
				BKNI_Printf("MEMC%d: slot idx: %d prev: 0x%x\n", hMonitor->mrcSettings.usMemcId, i, hMonitor->region_prev_by_addr[i]->addr);
			}
		}
		prev_addr = (uint32_t)hMonitor->region_prev_by_addr[i];
	}

	return 0;
}

/* End of file */
