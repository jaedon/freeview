/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint.c $
 * $brcm_Revision: Hydra_Software_Devel/70 $
 * $brcm_Date: 12/20/12 10:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/bint.c $
 * 
 * Hydra_Software_Devel/70   12/20/12 10:44p jgarrett
 * SW7425-4345: Merge to main
 * 
 * Hydra_Software_Devel/SW7425-4345/3   12/20/12 10:18a jgarrett
 * SW7425-4345: Fixing compiler warning
 * 
 * Hydra_Software_Devel/SW7425-4345/2   12/19/12 6:00p jgarrett
 * SW7425-4345: Incorporating review feedback
 * 
 * Hydra_Software_Devel/SW7425-4345/1   12/18/12 4:35p jgarrett
 * SW7425-4345: Adding BINT_ApplyL2State_isr
 * 
 * Hydra_Software_Devel/69   10/8/12 11:44a vsilyaev
 * SW7425-3963: If there is no 'enabled' interrupts per L2 registers,
 * don't access such register, the only exception is weak-mask and 'very-
 * special' interrupts
 * 
 * Hydra_Software_Devel/68   10/5/12 11:12a mphillip
 * SW7425-3963: Merge interrupt changes to main
 * 
 * Hydra_Software_Devel/SW7425-3963/1   9/20/12 4:15p mphillip
 * SW7425-3963: Update interrupt handling with optimizations and support
 * for 'weak' interrupt masking
 * 
 * Hydra_Software_Devel/67   6/29/12 2:49p jgarrett
 * SW7425-3401: Adding assertions to check for critical section when
 * required
 * 
 * Hydra_Software_Devel/66   3/1/11 12:27p jtna
 * SW7420-972: add comment about needing BCHP_PWR around BINT_Open
 * 
 * Hydra_Software_Devel/65   2/7/11 11:06a erickson
 * SW7340-249: clarify need for re-enabling L2's in BINT_Isr
 *
 * Hydra_Software_Devel/64   8/9/10 9:49a pntruong
 * SW3548-2526: Tighten the use of _isr function.
 *
 * Hydra_Software_Devel/63   8/2/10 1:33p erickson
 * SW7420-880: use BDBG_OBJECT to protect INT. do not BDBG_OBJECT_ASSERT
 * in BINT_Isr because it's a critical inner loop.
 *
 * Hydra_Software_Devel/62   10/15/09 12:50p erickson
 * SW7405-3221: add BDBG_MSG for callback->StatInfo if available. clear
 * stats on every timer. default off.
 *
 * Hydra_Software_Devel/61   7/24/09 6:11p pntruong
 * PR55861: Further refactored the new int macro.
 *
 * Hydra_Software_Devel/61   7/24/09 6:09p pntruong
 * PR55861: Further refactored the new int macro.
 *
 * Hydra_Software_Devel/61   7/24/09 6:05p pntruong
 * PR55861: Further refactored the new int macro to ease porting of new
 * chips.
 *
 * Hydra_Software_Devel/60   7/24/09 2:24p mward
 * PR55545: Add 7125 to BINT_NEW_INT_MODEL list.
 *
 * Hydra_Software_Devel/59   7/10/09 4:40p erickson
 * PR56517: add #ifndef BINT_OPEN_BYPASS_L2INIT option for power standby
 * driver
 *
 * Hydra_Software_Devel/58   4/23/09 10:56a jhaberf
 * PR53796: Adding BCM35130 support.
 *
 * Hydra_Software_Devel/57   4/8/09 4:23p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 *
 * Hydra_Software_Devel/56   1/31/09 1:55a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/55   12/3/08 9:25p nickh
 * PR49691: Enable interrupt support for 7420 for new user mode driver
 *
 * Hydra_Software_Devel/54   9/29/08 4:54p rpan
 * PR47411: Enable interrupt support for 3548/3556 for new user mode
 * driver.
 *
 * Hydra_Software_Devel/53   9/2/08 10:46a erickson
 * PR46415: added goto so we don't skip BINT_UNLOCK
 *
 * Hydra_Software_Devel/52   7/30/08 4:04p vishk
 * PR45177: uintptr_t is now defined in linux 2.6.18-5.1 header files
 *
 * Hydra_Software_Devel/51   4/25/08 10:27a vsilyaev
 * PR 41896: Added support to reentrant configuration
 *
 * Hydra_Software_Devel/PR41896/1   4/17/08 6:49p vsilyaev
 * PR 41896: Added support to reentrant configuration
 *
 * Hydra_Software_Devel/50   11/28/07 11:47a katrep
 * PR37430: fixed compiler error.
 *
 * Hydra_Software_Devel/49   11/28/07 10:58a katrep
 * PR37430: Extended interrupt interface to 128 bits for 7405,7325,7335
 *
 * Hydra_Software_Devel/48   7/30/07 1:45p vsilyaev
 * PR 33617: Added BINT_P_CreateCallback_Tag symbol for release build
 *
 * Hydra_Software_Devel/47   7/24/07 10:41a jgarrett
 * PR 33363: Revising check for missing interrupt
 *
 * Hydra_Software_Devel/46   2/15/07 12:01p erickson
 * PR26657: optimized BINT_Isr. added BINT_IS_STANDARD to allow standard
 * interrupts to be processed inside bint.c.
 *
 * Hydra_Software_Devel/45   2/9/07 5:23p albertl
 * PR24115:  Added warning messages for long executing interrupts and
 * runaway interrupts with adjustable compile time thresholds.
 *
 * Hydra_Software_Devel/43   5/25/06 4:13p albertl
 * PR21392:  BINT_Stats functions now split off into bint_stats.h to solve
 * BTMR circular dependency.
 *
 * Hydra_Software_Devel/42   5/24/06 6:56p albertl
 * PR21392:  Changed BINT stats tracking to use timers from TMR module.
 * Added BINT_Stats_Enable and BINT_Stats_Disable.
 *
 * Hydra_Software_Devel/41   2/15/06 5:30p vsilyaev
 * PR 19693: Added support for acquiring interrupt rate
 *
 * Hydra_Software_Devel/40   6/30/05 4:36p hongtaoz
 * PR15921: keep L2 handle's enable count consistent with number of
 * enabled callbacks;
 *
 * Hydra_Software_Devel/39   4/13/05 5:49p albertl
 * PR10596:  Added parenthesis to conditionals in BINT_Stats_AddBin.
 *
 * Hydra_Software_Devel/38   4/13/05 5:16p albertl
 * PR10596:  Fixed error checking.  Moved bDefaultBins to
 * BINT_Stats_CallbackStats.
 *
 * Hydra_Software_Devel/37   4/11/05 5:30p albertl
 * PR10596:  Added BINT_Stats_DestroyBins and BINT_Stats_Reset.
 * Implemented default bin configuration.
 *
 * Hydra_Software_Devel/36   4/7/05 4:17p albertl
 * PR10596:  Changed BINT_STATS_TRACK to BINT_STATS_ENABLE.  Added warning
 * message for calling BINT_Stats_Get without enabling stats tracking.
 *
 * Hydra_Software_Devel/35   4/6/05 2:29p albertl
 * PR10596:  pReadTimer and pGetElapsedTime function pointers now checked
 * before executing.  Removed unnecessary #ifdef BINT_STATS_TRACK cases
 * for better readability.
 *
 * Hydra_Software_Devel/34   4/5/05 7:12p albertl
 * PR10596:  Added new statistics tracking functionality.
 *
 * Hydra_Software_Devel/33   9/2/04 1:29p marcusk
 * PR12445: Check usage count before unmasking an interrupt.
 *
 * Hydra_Software_Devel/32   8/31/04 12:17p marcusk
 * PR12445: Updated to unmask interrupts required to support shared L1
 * between GFX and Softmodem
 *
 * Hydra_Software_Devel/31   8/11/04 1:57p marcusk
 * PR12255: UPdated to allow BINT_ClearCallback() to always clear
 * callback.
 *
 * Hydra_Software_Devel/30   8/9/04 5:12p marcusk
 * PR12233: UPdated so that disabling a callback that is already disabled
 * does nothing.
 *
 * Hydra_Software_Devel/29   5/24/04 3:01p marcusk
 * PR10666: Mergedown from B0
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/19/04 11:52a marcusk
 * PR10666: No longer clear previous interrupts when enabling a callback.
 *
 * Hydra_Software_Devel/28   4/16/04 5:12p marcusk
 * PR10666: Partially merged functionality from B0 specific branch.  Full
 * merge will occur once B0 is released.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/16/04 4:55p marcusk
 * PR10666: Do not clear interrupts when enabling and added
 * BINT_ClearCallback() routines.
 *
 * Hydra_Software_Devel/27   4/2/04 12:12p marcusk
 * PR10462: Removed while loop when processing interrupts.
 *
 * Hydra_Software_Devel/26   3/5/04 10:41a marcusk
 * PR9994: Updated to check both interrupt register and offset when
 * creating a callback.
 *
 * Hydra_Software_Devel/25   2/3/04 8:38p vsilyaev
 * PR 9606: Clear all managed L2 interrupts.
 *
 * Hydra_Software_Devel/24   1/9/04 12:00p marcusk
 * PR9241: Updated to insert on tail rather than on head.
 *
 * Hydra_Software_Devel/23   1/6/04 9:45a marcusk
 * PR9117: Properly look up interrupt ignore mask during isr() routine.
 *
 * Hydra_Software_Devel/22   1/5/04 4:26p marcusk
 * PR9117: Updated to support PI provided L2 interrupt handler (for
 * transport message and overflow interrupts).  Updated documentation.
 *
 * Hydra_Software_Devel/21   12/30/03 12:02p dlwin
 * PR 9117: Fixed a problem with creating Callback for UPG interrupts.
 *
 * Hydra_Software_Devel/20   12/29/03 4:16p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/19   12/23/03 11:13a marcusk
 * PR8985: No longer assert since this causes problems with the UPG
 * interrupts.
 *
 * Hydra_Software_Devel/18   12/18/03 2:55p marcusk
 * PR8985: Updated to assert if somebody is messing with the interrupt
 * mask.
 *
 * Hydra_Software_Devel/17   12/18/03 2:37p marcusk
 * PR8985: Removed bint_priv.h since it is no longer needed.
 *
 * Hydra_Software_Devel/16   12/18/03 2:08p marcusk
 * PR8985: Refactored to use single ISR() routine. Removed reserved names.
 * Placed all platform specific defines in bint_plat.h
 *
 * Hydra_Software_Devel/15   12/1/03 4:51p jasonh
 * Added interrupt clear when callback is enabled.
 *
 * Hydra_Software_Devel/14   11/24/03 3:27p marcusk
 * PR 8719: Clear and mask all managed interrupts at open time.
 *
 * Hydra_Software_Devel/13   9/16/03 10:30a marcusk
 * Updated to comply with DocJet requirements. Fixes for PR8055.
 *
 * Hydra_Software_Devel/12   8/26/03 10:43a marcusk
 * Removed default settings (they are not valid)
 *
 * Hydra_Software_Devel/11   8/22/03 3:00p erickson
 * added BINT_GetDefaultSettings
 *
 * Hydra_Software_Devel/10   6/18/03 3:26p dlwin
 * Added support to allow for more general implementation of Interrupt
 * manager.
 *
 * Hydra_Software_Devel/9   4/2/03 10:38a marcusk
 * Updated to support flag to specify if the interrupt can be triggered by
 * the CPU.
 *
 * Hydra_Software_Devel/7   3/31/03 4:07p marcusk
 * Detect interrupts that get enabled outside the interrupt interface
 * module.
 *
 * Hydra_Software_Devel/6   3/31/03 8:57a marcusk
 * Updated out of memory error returns.
 *
 * Hydra_Software_Devel/5   3/31/03 8:45a marcusk
 * Fixed a small bug when all 32 bits of an L2 interrupt register is used.
 *
 * Hydra_Software_Devel/4   3/28/03 10:22a marcusk
 * Fixed many bugs. Updated to trace when returning errors. Added support
 * for unit test environment (without hardware).
 *
 * Hydra_Software_Devel/3   3/25/03 4:08p marcusk
 * Updated some comments.
 *
 * Hydra_Software_Devel/2   3/21/03 6:29p marcusk
 * Initial version (that compiles).
 *
 * Hydra_Software_Devel/1   3/21/03 5:51p marcusk
 * In development.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_plat.h"  /* include other interrupt interface headers */
#include "bint_stats.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "blst_slist.h"
#include "bchp.h"
#include "btmr.h"

BDBG_MODULE(int);

#define BINT_REENTRANT	1
#define BINT_NON_REENTRANT	0

#ifndef BINT_REENTRANT_CONFIG
#define BINT_REENTRANT_CONFIG   BINT_REENTRANT
#endif

#ifndef BKNI_ASSERT_ISR_CONTEXT
#define BKNI_ASSERT_ISR_CONTEXT() (void)0
#endif

/* forward typedef to manage circular dependencies between structures */
typedef struct BINT_P_Callback *BINT_P_CallbackHandle;
typedef struct BINT_P_L2Register BINT_P_L2Register;

/*
Summary:
This structure defines the head element for creating
a linked list.
*/
typedef struct BINT_P_cblHead BINT_P_cblHead;
BLST_S_HEAD(BINT_P_cblHead, BINT_P_Callback);

/*
Summary:
This strcuture defines a single element in the
table used in the InterruptInterface.

Description:
One and only instance of this structure will exist for each L2 interrupt bit.
*/
typedef struct BINT_P_L2Int
{
    BLST_S_ENTRY(BINT_P_L2Int) link; /* linked list support */
    BINT_P_L2Register *L2Reg; /* pointer back to L2Register */
    BINT_Id intId; /* Interrupt ID (contains L2 base and L2 shift) */
    int enableCount; /* Number of callbacks that are enabled for this interrupt */
    BINT_P_cblHead callbackList; /* list of callbacks associated with this interrupt */
    unsigned count;     /* number of times when L2 interrupt was fired */
} BINT_P_L2Int, *BINT_P_L2Handle;

/*
Summary:
This structure defines the head element for creating
a linked list.
*/
typedef struct BINT_P_L2Head BINT_P_L2Head;
BLST_S_HEAD(BINT_P_L2Head, BINT_P_L2Int);

/*
Summary:
This structure defines the a callback handle/context.  It includes
linked list support.

Description:
One of these structures exists for each callback created.  There
may be multiple callbacks assigned for a single interrupt.
*/
BDBG_OBJECT_ID(BINT_Callback);
typedef struct BINT_P_Callback
{
    BDBG_OBJECT(BINT_Callback)
    BLST_S_ENTRY(BINT_P_Callback) link; /* linked list support */
    BLST_S_ENTRY(BINT_P_Callback) allCbLink; /* links all callbacks created for traversing */
    BINT_P_L2Handle L2Handle; /* L2 handle for this interrupt */
    BINT_CallbackFunc func; /* function to call when the interrupt triggers */
    void * pParm1; /* returned when callback is executed */
    int parm2; /* returned when callback is executed */
    const char *callbackName; /* callback name saved for the debug builds */

    bool enabled; /* false: callback will never be executed, true: callback will be executed when interrupt triggers */

    uint32_t ulStartTime;

    unsigned count; /* number of times when callback interrupt was fired */

    BINT_Stats_CallbackStats StatInfo;
} BINT_P_Callback;

struct BINT_P_L2Register {
    BLST_S_ENTRY(BINT_P_L2Register) link;
    BINT_P_IntMap intMapEntry; /* interrupt map entry (copy with 'correct' L1Shift)*/
    bool standard;
    bool weakMask;
    int enableCount; /* Number of callbacks that are enabled for this register */
    BINT_Handle intHandle; /* handle to the main InterruptInterface */
    BLST_S_HEAD(BINT_P_L2IntList, BINT_P_L2Int) intList; /* L2 Interrupt for this register */
};

/*
Summary:
This structure defines state per each L1 interrupt bit. 
*/
typedef struct BINT_P_L1 {
    BLST_S_HEAD(BINT_P_L2RegisterList, BINT_P_L2Register) L2RegList; /* list of all L2 Registers for this L1 bit */
} BINT_P_L1;

BDBG_OBJECT_ID(BINT);

typedef struct BINT_P_Context
{
    BDBG_OBJECT(BINT)
#if BINT_REENTRANT_CONFIG==BINT_REENTRANT
    BKNI_MutexHandle lock;
#define BINT_LOCK(h) BKNI_AcquireMutex((h)->lock)
#define BINT_UNLOCK(h) BKNI_ReleaseMutex((h)->lock)
#else
#define BINT_LOCK(h)
#define BINT_UNLOCK(h)
#endif
#if 0
    BINT_P_L2Head IntArray[BINT_P_L1_SIZE]; /* Array of L2 interrupt lists (one list per L1 interrupt). NULL if not used. */
#endif
    const BINT_P_IntMap *pIntMap; /* ptr to the interrupt map, REQUIRED */
    int callbackCount; /* Number of callbacks installed in InterruptInterface */
    unsigned numInts; /* Number of L2 interrupts managed by this instance of the InterruptInterface */
    BREG_Handle regHandle; /* regHandle for accessing interrupt registers */
    BINT_SetIntFunc pSetInt; /* ptr to Set Interrupt, NULL if none */
    BINT_ClearIntFunc pClearInt; /* ptr to Clear Interrupt, NULL if none */
    BINT_SetMaskFunc pSetMask; /* ptr to Set Interrupt Mask, REQUIRED */
    BINT_ClearMaskFunc pClearMask; /* ptr to Clear Interrupt Mask, REQUIRED */
    BINT_ReadStatusFunc pReadStatus; /* ptr to Read Status, REQUIRED */
    BINT_P_cblHead allCbList; /* list of all callbacks registered */
    BINT_Settings settings; /* BINT_Settings */
    BTMR_TimerHandle hTimer; /* timer used for stats tracking */
    bool bStatsEnable; /* enables stats tracking */

    BINT_P_L1 L1[BINT_P_L1_SIZE]; /* Array of L1 interrupt states */
    BINT_P_L2Register *L2RegisterData; /*  pointer to store data for all L2 registers */
} BINT_P_Context;

/* Default bin configuration */
static const BINT_Stats_CallbackBin g_aDefaultBins[] =
{
    /* range min, range max, bin hit count */
    {  0,         50,        0 },
    {  51,        100,       0 },
    {  101,       200,       0 },
    {  201,       500,       0 },
    {  501,       1000,      0 },
    {  1000,      2500,      0 },
    {  2501,      10000,     0 },
    {  10001,     50000,     0 },
    {  50001,     100000,    0 },
    {  100001,    500000,    0 },
};

#define BINT_P_STATS_DEFAULT_BINS_NUM \
    (sizeof (g_aDefaultBins) / sizeof (BINT_Stats_CallbackBin))

static BERR_Code BINT_P_Stats_ComputeStats( BINT_CallbackHandle cbHandle, uint32_t ulStart, uint32_t ulEnd );
static uint32_t BINT_P_GetElapsedTime( uint32_t ulTimerStart, uint32_t ulTimerEnd );
static void BINT_P_ClearIntNoOp( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
    return;
}

void BINT_ApplyL2State_isr( 
    BINT_Handle intHandle,
    uint32_t    L2Reg
    )
{
    unsigned L1Shift;
    const BINT_P_L2Register *pL2Reg;

    BDBG_OBJECT_ASSERT(intHandle, BINT);

    for(L1Shift = 0; L1Shift<BINT_P_L1_SIZE; L1Shift++) 
    {
        for(pL2Reg = BLST_S_FIRST(&intHandle->L1[L1Shift].L2RegList); pL2Reg ; pL2Reg = BLST_S_NEXT(pL2Reg, link)) 
        {
            BINT_P_L2Handle L2Handle; 
            if(pL2Reg->intMapEntry.L2RegOffset != L2Reg) 
            {
                continue;
            }
            for( L2Handle=BLST_S_FIRST(&pL2Reg->intList); L2Handle ; L2Handle=BLST_S_NEXT(L2Handle, link)) 
            {
                if(!L2Handle->enableCount) 
                {
                    continue;
                } 
                if(pL2Reg->standard)             
                {
                    BREG_Write32_isr( intHandle->regHandle, L2Reg+BINT_P_STD_MASK_CLEAR, 1<<BCHP_INT_ID_GET_SHIFT(L2Handle->intId));
                } 
                else 
                {
                    intHandle->pClearMask( intHandle->regHandle, L2Reg, BCHP_INT_ID_GET_SHIFT(L2Handle->intId));
                }
            }
            break;
        }
    }
}




static void BINT_P_ClearInt(BINT_Handle intHandle, const BINT_P_L2Register *L2Reg, unsigned L2Shift)
{
    if(L2Reg->standard) {
        BREG_Write32(intHandle->regHandle, L2Reg->intMapEntry.L2RegOffset+BINT_P_STD_CLEAR, 1<<L2Shift); 
    } else {
        intHandle->pClearInt( intHandle->regHandle, L2Reg->intMapEntry.L2RegOffset, L2Shift);
    }
    return;
}

static void BINT_P_SetMask(BINT_Handle intHandle, bool standard, uint32_t L2RegOffset, unsigned L2Shift)
{
    if(standard) {
        BREG_Write32(intHandle->regHandle, L2RegOffset+BINT_P_STD_MASK_SET, 1<<L2Shift);
    } else {
        intHandle->pSetMask(intHandle->regHandle, L2RegOffset, L2Shift);
    }
    return;
}

BERR_Code BINT_Open( BINT_Handle *pHandle, BREG_Handle regHandle, const BINT_Settings *pDefSettings )
{
    int i;
    BERR_Code rc = BERR_SUCCESS;
    BINT_Handle intHandle;
    unsigned countStandard=0;
    unsigned countWeakMask=0;
    unsigned countExternal=0;

    intHandle = BKNI_Malloc( sizeof( BINT_P_Context ) );
    if( intHandle == NULL )
    {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }

    BKNI_Memset( intHandle, 0, sizeof( *intHandle) );
    BDBG_OBJECT_SET(intHandle, BINT);

#if BINT_REENTRANT_CONFIG==BINT_REENTRANT
    rc = BKNI_CreateMutex(&intHandle->lock);
    if(rc!=BERR_SUCCESS)
    {
        rc=BERR_TRACE(rc);
        goto error;
    }
#endif

    intHandle->numInts = 0;
    intHandle->callbackCount = 0;
    intHandle->pIntMap = pDefSettings->pIntMap;
    intHandle->pSetInt = pDefSettings->pSetInt;
    intHandle->pClearInt = pDefSettings->pClearInt ? pDefSettings->pClearInt : BINT_P_ClearIntNoOp;
    intHandle->pSetMask = pDefSettings->pSetMask;
    intHandle->pClearMask = pDefSettings->pClearMask;
    intHandle->pReadStatus = pDefSettings->pReadStatus;
    intHandle->settings = *pDefSettings;

    intHandle->hTimer = NULL;
    intHandle->bStatsEnable = false;

    intHandle->regHandle = regHandle;

    if( intHandle->pReadStatus == NULL ||
        intHandle->pSetMask == NULL ||
        intHandle->pClearMask == NULL )
    {
        BDBG_ERR(("Invalid function points passed into BINT_Open()"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ASSERT(0);
        goto error;
    }


    /* If your platform has BCHP_PWR-based power management, then the SW layer that is calling BINT_Open 
       must also acquire/release power around BINT_Open. Otherwise, you'll likely hit GISB errors below,
       when registers are accessed without power to their respective 108M clocks.

       See /rockford/appframework/src/common/appframework/framework.c or nexus_core.c for examples 
       on how to do this */
       
#ifndef BINT_OPEN_BYPASS_L2INIT
    /* Clear all L2 interrupts */
    for( i=0;;i++) {
        unsigned bit;
        const BINT_P_IntMap *L2Register = &intHandle->pIntMap[i];
        if (L2Register->L1Shift<0) {
            break;
        }
        if(BINT_MAP_IS_EXTERNAL(L2Register)) {
            continue;
        }
        for(bit=0; bit<32; bit++) {
            if ( (L2Register->L2InvalidMask&(1<<bit))==0) {
                BINT_P_SetMask(intHandle, L2Register->L1Shift&BINT_IS_STANDARD, L2Register->L2RegOffset, bit);
            }
        }
    }
#endif
    /* count number of entries in the map */
    for( i=0; i<BINT_P_L1_SIZE; i++ )
    {
        BLST_S_INIT(&intHandle->L1[i].L2RegList);
    }
    /* count number of L2 Registers */
    for( i=0;;i++) {
        if(intHandle->pIntMap[i].L1Shift<0) {
            break;
        }
    }
    intHandle->L2RegisterData = BKNI_Malloc(sizeof(*intHandle->L2RegisterData)*i);
    if(intHandle->L2RegisterData==NULL) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}
    for( i=0;;i++) {
        int j;
        BINT_P_L2Register *L2Reg = intHandle->L2RegisterData + i;
        const BINT_P_IntMap *intMapEntry = &intHandle->pIntMap[i];
        int L1Shift = BINT_MAP_GET_L1SHIFT(intMapEntry);

        if(intMapEntry->L1Shift<0) {
            break;
        }
        if(BINT_MAP_IS_EXTERNAL(intMapEntry)) { /* external interrupt */
            countExternal ++;
            continue;
        }
        L2Reg->intMapEntry = *intMapEntry;
        L2Reg->intHandle = intHandle;
        L2Reg->intMapEntry.L1Shift = L1Shift; /* correct L1Shift */
        BLST_S_INIT(&L2Reg->intList);
        L2Reg->standard = (intMapEntry->L1Shift & BINT_IS_STANDARD)==BINT_IS_STANDARD;
        L2Reg->weakMask = (intMapEntry->L1Shift & BINT_P_MAP_MISC_WEAK_MASK)==BINT_P_MAP_MISC_WEAK_MASK;
        L2Reg->enableCount = 0;
        if(L2Reg->standard && L2Reg->weakMask) { rc = BERR_TRACE(BERR_NOT_SUPPORTED);goto error; } /* interrupt couldn't be both standard and with a 'weak' mask */
        if(L2Reg->intMapEntry.L2InvalidMask == BINT_DONT_PROCESS_L2 && (L2Reg->standard || L2Reg->weakMask)) { rc = BERR_TRACE(BERR_NOT_SUPPORTED);goto error; } /* interrupt couldn't be both don't process and either standard or with a 'weak' mask */

        countStandard += L2Reg->standard;
        countWeakMask += L2Reg->weakMask;
        BDBG_ASSERT(L1Shift<BINT_P_L1_SIZE);
        for(j=0;j<i;j++) {
            BINT_P_L2Register *L2RegOld = intHandle->L2RegisterData + j;
            BINT_P_IntMap *intMapEntryOld = &L2RegOld->intMapEntry;
            if(intMapEntryOld->L2RegOffset == intMapEntry->L2RegOffset) {
                if( ~(intMapEntryOld->L2InvalidMask | intMapEntry->L2InvalidMask) != 0 || intMapEntryOld->L2InvalidMask == BINT_DONT_PROCESS_L2 || intMapEntry->L2InvalidMask == BINT_DONT_PROCESS_L2) {
                    BDBG_ERR(("Duplicated mask bits used for %#x (%s,%u:%s,%u), mask %#x&%#x->%#x", intMapEntry->L2RegOffset, intMapEntryOld->L2Name, intMapEntryOld->L1Shift, intMapEntry->L2Name, L1Shift, intMapEntryOld->L2InvalidMask, intMapEntry->L2InvalidMask, ~(intMapEntryOld->L2InvalidMask | intMapEntry->L2InvalidMask)));
                }
                if(intMapEntryOld->L1Shift==L1Shift) {
                    BDBG_WRN(("found duplicated entry for %#x (%u:%s:%s), combining mask %#x&%#x->%#x", intMapEntry->L2RegOffset, L1Shift, intMapEntryOld->L2Name, intMapEntry->L2Name, intMapEntryOld->L2InvalidMask, intMapEntry->L2InvalidMask, intMapEntryOld->L2InvalidMask & intMapEntry->L2InvalidMask));
                    intMapEntryOld->L2InvalidMask = intMapEntryOld->L2InvalidMask & intMapEntry->L2InvalidMask;
                    break;
                }
            }
        }
        if(j!=i) {
            continue;
        }
        BLST_S_INSERT_HEAD(&intHandle->L1[L1Shift].L2RegList, L2Reg, link);
#ifndef BINT_OPEN_BYPASS_L2INIT
        if(L2Reg->weakMask) {
            unsigned bit;
            for(bit=0; bit<32; bit++) {
                if ( (L2Reg->intMapEntry.L2InvalidMask&(1<<bit))==0) {            
                    BINT_P_ClearInt(intHandle, L2Reg, bit);
                }
            }
        }
#endif
    }
    if(countStandard == 0 || countWeakMask == 0 || countExternal == 0) {
        /* if this warning get printed then it's likely that the chip specific interrupt map was not updated, this leads either to sub-optimal performace or possible interrupt live-lock */
        BDBG_WRN(("using BINT with %u external interrupts, %u standard interrupts and  %u interrups with a 'weak' mask", countExternal, countStandard, countWeakMask));
    }

    *pHandle = intHandle;

    return rc;

error:

    if( intHandle != NULL )
    {
#if BINT_REENTRANT_CONFIG==BINT_REENTRANT
        if(intHandle->lock)
        {
            BKNI_DestroyMutex(intHandle->lock);
        }
#endif
        if(intHandle->L2RegisterData) {
            BKNI_Free(intHandle->L2RegisterData);
        }
        BKNI_Free( intHandle );
    }
    *pHandle = NULL;

    return rc;
}

BERR_Code BINT_Close( BINT_Handle intHandle )
{
    int L1Shift;

    BDBG_OBJECT_ASSERT(intHandle, BINT);
    BDBG_ASSERT ( intHandle->bStatsEnable == false );

    for(L1Shift=0; L1Shift<BINT_P_L1_SIZE; L1Shift++ ) {
        BINT_P_L2Register *L2Reg;

        for(L2Reg = BLST_S_FIRST(&intHandle->L1[L1Shift].L2RegList); L2Reg ; L2Reg = BLST_S_NEXT(L2Reg, link)) {
            BINT_P_L2Handle L2Handle;

            while(NULL!=(L2Handle=BLST_S_FIRST(&L2Reg->intList))) {
                BINT_CallbackHandle cbHandle;

                while(NULL!=(cbHandle=BLST_S_FIRST(&L2Handle->callbackList))) {
                    BINT_DestroyCallback(cbHandle);
                }

                BKNI_EnterCriticalSection();
                BLST_S_REMOVE_HEAD(&L2Reg->intList, link);
                BKNI_LeaveCriticalSection();
                BKNI_Free( L2Handle );
            }
        }
    }

    if( intHandle->callbackCount != 0 ) {
        return BERR_TRACE(BERR_UNKNOWN);
    }

#if BINT_REENTRANT_CONFIG==BINT_REENTRANT
    BKNI_DestroyMutex(intHandle->lock);
#endif
    BKNI_Free(intHandle->L2RegisterData);
    BDBG_OBJECT_DESTROY(intHandle, BINT);
    BKNI_Free( intHandle );

    return BERR_SUCCESS;
}

static void BINT_P_ExecuteCallback(BINT_Handle intHandle, BINT_P_Callback *cbHandle)
{
#ifdef BINT_STATS_ENABLE
    uint32_t ulStart, ulEnd = 0;
    bool bStatsEnable = intHandle->bStatsEnable;
    BERR_Code rc;

    if (bStatsEnable)
    {
        rc = BTMR_ReadTimer_isr(intHandle->hTimer, &ulStart);
        if (rc != BERR_SUCCESS)
        {
            BDBG_WRN(("Error reading timer for statistics."));
        }
    }

    (*cbHandle->func)( cbHandle->pParm1, cbHandle->parm2 );

    if (bStatsEnable)
    {
        rc = BTMR_ReadTimer_isr(intHandle->hTimer, &ulEnd);
        if (rc != BERR_SUCCESS)
        {
            BDBG_WRN(("Error reading timer for statistics."));
        }

        BINT_P_Stats_ComputeStats( cbHandle, ulStart, ulEnd );
    }
#else
    (*cbHandle->func)( cbHandle->pParm1, cbHandle->parm2 );
#endif /* BINT_STATS_ENABLE */
    cbHandle->count++;
                
    return;
}

/**
BINT_Isr is the main inner loop of the entire refsw architecture.
Optimization of every line of code matters greatly.
**/
void BINT_Isr( BINT_Handle intHandle, int L1Shift )
{
    const BINT_P_L2Register *L2Reg;
    BREG_Handle regHandle = intHandle->regHandle;

#if 0
/* for performance, this ASSERT is compiled out. you can temporarily enable for debug. */
    BDBG_OBJECT_ASSERT(intHandle, BINT);
    BKNI_ASSERT_ISR_CONTEXT();
#endif
    for(L2Reg = BLST_S_FIRST(&intHandle->L1[L1Shift].L2RegList); L2Reg ; L2Reg = BLST_S_NEXT(L2Reg, link)) {
        uint32_t intStatus=0;
        uint32_t L2BaseRegister = L2Reg->intMapEntry.L2RegOffset;
        BINT_P_Callback *cbHandle;
        BINT_P_L2Handle L2Handle;

        if(L2Reg->enableCount==0 && !L2Reg->weakMask) { /* short-circular register access if there is no enabled interrupt handlers and no need to explicitly clears interrupt */
            continue;
        }
        L2Handle=BLST_S_FIRST(&L2Reg->intList);

        if (L2Reg->standard) {
            /* Standard registers can be handled internal to bint.c which results in dramatic performance improvement.
            Each chip must set BINT_IS_STANDARD as appropriate. */

            intStatus = BREG_Read32_isr(regHandle, L2BaseRegister); /* read status */
            intStatus &= ~L2Reg->intMapEntry.L2InvalidMask; /* only handle interrupts that are known */
            if(intStatus==0) {
                continue; /* short-circuit loop over L2 interrupts */
            }
            for( ; L2Handle ; L2Handle=BLST_S_NEXT(L2Handle, link)) {
                unsigned L2Bit = 1<<BCHP_INT_ID_GET_SHIFT( L2Handle->intId );
                if( (intStatus & L2Bit) && L2Handle->enableCount ) {
                    intStatus &= ~L2Bit; /* clear status bits */
                    L2Handle->count++; /* DumpInfo accounting */
                    BREG_Write32_isr(regHandle, L2BaseRegister + BINT_P_STD_CLEAR, L2Bit); /* clear status */
                    /* Call all callbacks that are enabled */
                    for(cbHandle=BLST_S_FIRST(&L2Handle->callbackList); cbHandle ; cbHandle=BLST_S_NEXT(cbHandle, link)) {
                        if( cbHandle->enabled ) {
                            BINT_P_ExecuteCallback(intHandle, cbHandle);
                        }
                    }
                    if(L2Handle->enableCount) { /* interrupt could be disabled by the interrupt handler */ 
                        /* Shared L1 interrupts require that the L2 be masked in bcmdriver.ko, so BINT unmasks
                        here to reverse that. For unshared L1's, this is harmless. */
                        BREG_Write32_isr(regHandle, L2BaseRegister + BINT_P_STD_MASK_CLEAR, L2Bit);
                    }
                }
            }
        } else if( L2Reg->intMapEntry.L2InvalidMask!=BINT_DONT_PROCESS_L2 ) { /* non-standard interrupts */ 
            unsigned L2Shift;

            intStatus = intHandle->pReadStatus(regHandle, L2BaseRegister);
            intStatus &= ~L2Reg->intMapEntry.L2InvalidMask; /* only handle interrupts that are known */
            if(intStatus==0) {
                continue; /* short-circuit loop over L2 interrupts */
            }
            for( ; L2Handle ; L2Handle=BLST_S_NEXT(L2Handle, link)) {
                unsigned L2Bit;
                L2Shift = BCHP_INT_ID_GET_SHIFT( L2Handle->intId );
                L2Bit = 1<<L2Shift;

                /* find any interrupts that are triggered and enabled */
                if( (intStatus & L2Bit) && L2Handle->enableCount ) {
                    intStatus &= ~L2Bit; /* clear status bits */
                    L2Handle->count++; /* DumpInfo accounting */
                    /* Since L2 interrupts are edge triggered they must be cleared before processing!! */
                    intHandle->pClearInt( regHandle, L2BaseRegister, L2Shift);
                    /* Call all callbacks that are enabled */
                    for(cbHandle=BLST_S_FIRST(&(L2Handle->callbackList)); cbHandle ; cbHandle=BLST_S_NEXT(cbHandle, link)) {
                        if( cbHandle->enabled ) {
                            BINT_P_ExecuteCallback(intHandle, cbHandle);
                        }
                    }
                    if( L2Handle->enableCount ) { /* interrupt could be disabled by the interrupt handler */ 
                        /* Shared L1 interrupts require that the L2 be masked in bcmdriver.ko, so BINT unmasks
                        here to reverse that. For unshared L1's, this is harmless. */
                        intHandle->pClearMask(regHandle, L2BaseRegister, L2Shift);
                    }
                }
            }
            if( intStatus && L2Reg->weakMask) {
                for(L2Shift=0;L2Shift<32;L2Shift++) {
                    if(intStatus&(1<<L2Shift)) {
                        intHandle->pClearInt( regHandle, L2BaseRegister, L2Shift);
                    }
                }
            }

        } else { /*  very special interrupts */
            /*
            If the L2InvalidMask is BINT_DONT_PROCESS_L2 that means that the interrupt interface
            does not process the L2 interrupts for this L1 shift.  Instead a separate L2
            isr routine should be installed as the callback for the specified L1
            shift that will handle this interrupt.
            */
            L2Handle->count++; /* DumpInfo accounting */
            cbHandle=BLST_S_FIRST(&(L2Handle->callbackList));
            if(cbHandle) {
                BINT_P_ExecuteCallback(intHandle, cbHandle);
            } else {
                BDBG_ERR(("L1 interrupt for not connected 'very special' %s", L2Reg->intMapEntry.L2Name));
            }
        }  
    }
    return;
}


#if BDBG_DEBUG_BUILD
#undef BINT_CreateCallback
BERR_Code BINT_CreateCallback( BINT_CallbackHandle *pCbHandle, BINT_Handle intHandle, BINT_Id intId, BINT_CallbackFunc func, void * pParm1, int parm2 )
{
    BDBG_WRN(("BINT_CallbackFunc shall be never called in the debug builds"));
    return BINT_P_CreateCallback_Tag(pCbHandle, intHandle, intId, func, pParm1, parm2, "");
}
BERR_Code BINT_P_CreateCallback_Tag( BINT_CallbackHandle *pCbHandle, BINT_Handle intHandle, BINT_Id intId, BINT_CallbackFunc func, void * pParm1, int parm2, const char *callbackName)
#else
BERR_Code BINT_P_CreateCallback_Tag( BINT_CallbackHandle *pCbHandle, BINT_Handle intHandle, BINT_Id intId, BINT_CallbackFunc func, void * pParm1, int parm2)
{
    return BINT_CreateCallback(pCbHandle, intHandle, intId, func, pParm1, parm2);
}
BERR_Code BINT_CreateCallback( BINT_CallbackHandle *pCbHandle, BINT_Handle intHandle, BINT_Id intId, BINT_CallbackFunc func, void * pParm1, int parm2 )
#endif
{
    uint32_t L2Shift = BCHP_INT_ID_GET_SHIFT(intId);
    int L1Shift;
    BINT_P_L2Handle L2Handle;
    BERR_Code rc;
    BINT_P_L2Register *L2Reg=NULL;
    BINT_CallbackHandle cbHandle;

    BDBG_OBJECT_ASSERT(intHandle, BINT);
    BINT_LOCK(intHandle);

    for(L1Shift = 0; L1Shift<BINT_P_L1_SIZE; L1Shift++) {
        for(L2Reg = BLST_S_FIRST(&intHandle->L1[L1Shift].L2RegList); L2Reg ; L2Reg = BLST_S_NEXT(L2Reg, link)) {
            /*
            We must find the matching L2 register offset and ensure that the specified L2 interrupt is actually
            handled by the specified L1->L2 mapping.  This is because some wacky L2 interrupt registers actually map to
            multiple L1 interrupts (i.e. 8 bits of the L2 register map to L1=X, while the other bits map to L1=Y).
            This also properly handles multiple L2 interrupt registers that are mapped to a single L1 bit.

            Also, if the L2InvalidMask is BINT_DONT_PROCESS_L2 for this register it means that the interrupt interface
            does not handle the L2 interrupts.  Rather it should just create the first callback associated with that
            L1 interrupt shift.
            */
            if( BCHP_INT_ID_GET_REG(intId) != L2Reg->intMapEntry.L2RegOffset) {
                continue;
            }
            if((L2Reg->intMapEntry.L2InvalidMask & (1<<L2Shift)) == 0 || L2Reg->intMapEntry.L2InvalidMask == BINT_DONT_PROCESS_L2) {
                break;
            }
        }
        if(L2Reg!=NULL) {
            break;
        }
    }

    if(L2Reg == NULL) { rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done; }

    /* Determine if we need to allocate a new L2 interrupt context */
    for( L2Handle=BLST_S_FIRST(&L2Reg->intList) ; L2Handle ; L2Handle=BLST_S_NEXT(L2Handle, link)) {
        if( L2Handle->intId == intId ) {
            break;
        }
    }
    if( L2Handle == NULL )
    {
        /* We need to create a new L2 element to manage this interrupt bit */
        L2Handle = BKNI_Malloc( sizeof(*L2Handle) );
        if( L2Handle == NULL ) { rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done; }

        BLST_S_INIT( &L2Handle->callbackList);
        L2Handle->enableCount = 0;
        L2Handle->intId = intId;
        L2Handle->L2Reg = L2Reg;
        L2Handle->count = 0;
        intHandle->numInts++;

        BKNI_EnterCriticalSection();
        BLST_S_INSERT_HEAD(&L2Reg->intList, L2Handle, link);
        BKNI_LeaveCriticalSection();

        /* clear previous status */
        BINT_P_ClearInt(intHandle, L2Reg, L2Shift);
    }
    if(L2Reg->intMapEntry.L2InvalidMask == BINT_DONT_PROCESS_L2) {
        if(BLST_S_FIRST(&L2Handle->callbackList)!=NULL) {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER); /* only one callback could be enabled for 'very special' interrupts */
            goto done;
        }
        L2Reg->enableCount = 1; /* set enable count to 1, to allow handling of such interrupt */
    }

    cbHandle = BKNI_Malloc( sizeof(*cbHandle) );
    if( cbHandle == NULL ) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done; }

    BKNI_Memset( cbHandle, 0, sizeof(*cbHandle) );
    BDBG_OBJECT_SET(cbHandle, BINT_Callback);

    cbHandle->func = func;
    cbHandle->pParm1 = pParm1;
    cbHandle->parm2 = parm2;
    cbHandle->L2Handle = L2Handle;

    cbHandle->count = 0;
#if BDBG_DEBUG_BUILD
    cbHandle->callbackName = callbackName;
#else
    cbHandle->callbackName = NULL;
#endif

    intHandle->callbackCount++;

    cbHandle->StatInfo.ulTimeMin = UINT32_MAX;
    cbHandle->StatInfo.ulTimeMax = 0;
    cbHandle->StatInfo.ulTimeAvg = 0;
    cbHandle->StatInfo.ulCbHitCount = 0;
    cbHandle->StatInfo.ulTimeStampStartIdx = 0;

    BKNI_Memset(cbHandle->StatInfo.aulTimeStamp, 0, sizeof(uint32_t) * BINT_P_STATS_RECENT_CB_HIT_COUNT);

    /* set up default bins */
    cbHandle->StatInfo.ulActiveBins = BINT_P_STATS_DEFAULT_BINS_NUM;
    BKNI_Memcpy(cbHandle->StatInfo.aBinInfo, g_aDefaultBins, sizeof(g_aDefaultBins));

    cbHandle->StatInfo.bDefaultBins = true;

    BKNI_EnterCriticalSection();
    BLST_S_INSERT_HEAD(&L2Handle->callbackList, cbHandle, link);
    BLST_S_INSERT_HEAD(&intHandle->allCbList, cbHandle, allCbLink);
    BKNI_LeaveCriticalSection();

    *pCbHandle = cbHandle;
    rc = BERR_SUCCESS;

done:
    BINT_UNLOCK(intHandle);
    return rc;
}

BERR_Code BINT_DestroyCallback( BINT_CallbackHandle cbHandle )
{
    BINT_P_Context *intHandle;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    intHandle = cbHandle->L2Handle->L2Reg->intHandle;
    BINT_LOCK(intHandle);

    if( cbHandle->enabled == true )
    {
        BINT_DisableCallback(cbHandle );
    }

    BKNI_EnterCriticalSection();
    BLST_S_REMOVE(&cbHandle->L2Handle->callbackList, cbHandle, BINT_P_Callback, link);
    BLST_S_REMOVE(&intHandle->allCbList, cbHandle, BINT_P_Callback, allCbLink);
    BKNI_LeaveCriticalSection();


    BDBG_OBJECT_DESTROY(cbHandle, BINT_Callback);
    BKNI_Free( cbHandle );
    intHandle->callbackCount--;
    BINT_UNLOCK(intHandle);

    return BERR_SUCCESS;
}

BERR_Code BINT_EnableCallback( BINT_CallbackHandle cbHandle )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    BKNI_EnterCriticalSection();
    rc = BINT_EnableCallback_isr( cbHandle );
    BKNI_LeaveCriticalSection();

    return rc;
}

BERR_Code BINT_EnableCallback_isr( BINT_CallbackHandle cbHandle )
{
    BINT_P_Context *intHandle;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    BKNI_ASSERT_ISR_CONTEXT();

    /* If enabled, we are already done... */
    if( cbHandle->enabled )
    {
        return BERR_SUCCESS;
    }

    intHandle = cbHandle->L2Handle->L2Reg->intHandle;

    /* Flag callback as enabled so that we execute it if we get an interrupt immediately after it is unmasked */
    cbHandle->enabled = true;

    cbHandle->L2Handle->enableCount++;
    cbHandle->L2Handle->L2Reg->enableCount ++;
    if( cbHandle->L2Handle->enableCount == 1 )
    {
        if(cbHandle->L2Handle->L2Reg->standard) {
		    BREG_Write32( intHandle->regHandle, BCHP_INT_ID_GET_REG(cbHandle->L2Handle->intId)+BINT_P_STD_MASK_CLEAR, 1<<BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId));
        } else {
            intHandle->pClearMask( intHandle->regHandle, BCHP_INT_ID_GET_REG(cbHandle->L2Handle->intId), BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId) );
	}
    }

    return BERR_SUCCESS;
}

BERR_Code BINT_DisableCallback( BINT_CallbackHandle cbHandle )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    BKNI_EnterCriticalSection();
    rc = BINT_DisableCallback_isr( cbHandle );
    BKNI_LeaveCriticalSection();

    return rc;
}

BERR_Code BINT_DisableCallback_isr( BINT_CallbackHandle cbHandle )
{
    BINT_P_Context *intHandle;
    BINT_P_L2Handle L2Handle;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    BKNI_ASSERT_ISR_CONTEXT();

    /* If not enabled, we are already done... */
    if( cbHandle->enabled == false )
    {
        return BERR_SUCCESS;
    }

    L2Handle = cbHandle->L2Handle;
    intHandle = L2Handle->L2Reg->intHandle;

    L2Handle->enableCount--;
    L2Handle->L2Reg->enableCount --;
    BDBG_ASSERT(L2Handle->enableCount >=0);
    BDBG_ASSERT(L2Handle->L2Reg->enableCount >=0);
    if( cbHandle->L2Handle->enableCount == 0 )
    {
        BINT_P_SetMask(intHandle, cbHandle->L2Handle->L2Reg->standard, BCHP_INT_ID_GET_REG(cbHandle->L2Handle->intId), BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId) );
    }

    /* Flag callback as disabled only after it is masked so that we can execute if we get an interrupt */
    cbHandle->enabled = false;

    return BERR_SUCCESS;
}

BERR_Code BINT_ClearCallback( BINT_CallbackHandle cbHandle )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    BKNI_EnterCriticalSection();
    rc = BINT_ClearCallback_isr( cbHandle );
    BKNI_LeaveCriticalSection();

    return rc;
}

BERR_Code BINT_ClearCallback_isr( BINT_CallbackHandle cbHandle )
{
    BERR_Code rc = BERR_SUCCESS;
    BINT_P_Context *intHandle;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    BKNI_ASSERT_ISR_CONTEXT();

    intHandle = cbHandle->L2Handle->L2Reg->intHandle;

    
    BINT_P_ClearInt(intHandle, cbHandle->L2Handle->L2Reg, BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId) );

    return rc;
}

BERR_Code BINT_TriggerInterruptByHandle( BINT_CallbackHandle cbHandle )
{
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    return BINT_TriggerInterruptByHandle_isr( cbHandle );
}

BERR_Code BINT_TriggerInterruptByHandle_isr( BINT_CallbackHandle cbHandle )
{
    BINT_P_Context *intHandle;
    const BINT_P_L2Register *L2Reg;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    BKNI_ASSERT_ISR_CONTEXT();

    L2Reg = cbHandle->L2Handle->L2Reg;
    intHandle = L2Reg->intHandle;

    if(L2Reg->standard) {
        BREG_Write32_isr(intHandle->regHandle, BCHP_INT_ID_GET_REG(cbHandle->L2Handle->intId)+BINT_P_STD_SET, 1<<BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId) );
    } else if(intHandle->pSetInt == NULL) {
        intHandle->pSetInt( intHandle->regHandle, BCHP_INT_ID_GET_REG(cbHandle->L2Handle->intId), BCHP_INT_ID_GET_SHIFT(cbHandle->L2Handle->intId) );
    } else {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    return BERR_SUCCESS;
}

#if (BINT_NEW_INT_MODEL)
void BINT_GetL1BitMask(BINT_Handle intHandle, uint32_t  *BitMask)
{
    int i;


    BDBG_OBJECT_ASSERT(intHandle, BINT);

    for(i=0;i<BINT_MAX_INTC_SIZE;i++)
    {
        BitMask[i]=0;
    }

    for(i=0; i<BINT_P_L1_SIZE; i++ ) {
        const BINT_P_L2Register *L2Reg = BLST_S_FIRST(&intHandle->L1[i].L2RegList);
        unsigned L1Shift;
        if(L2Reg==NULL) {
            continue;
        }
        L1Shift = L2Reg->intMapEntry.L1Shift;
        if(L1Shift >= 96)
        {
            BitMask[3] |= 1ul<<(L1Shift-96);
        }
        else if(L1Shift >= 64 && L1Shift < 96)
        {
            BitMask[2] |= 1ul<<(L1Shift-64);
        }
        else if( L1Shift >= 32 && L1Shift < 64 )
        {
            BitMask[1] |= 1ul<<(L1Shift-32);
        }
        else
        {
            BitMask[0] |= 1ul<<L1Shift;
        }
    }
}
#else
void BINT_GetL1BitMask( BINT_Handle intHandle, uint32_t *pBitMaskLo, uint32_t *pBitMaskHi )
{
    int i;

    BDBG_OBJECT_ASSERT(intHandle, BINT);

    *pBitMaskLo = 0;
    *pBitMaskHi = 0;

    for(i=0; i<BINT_P_L1_SIZE; i++ ) {
        const BINT_P_L2Register *L2Reg = BLST_S_FIRST(&intHandle->L1[i].L2RegList);
        unsigned L1Shift;
        if(L2Reg==NULL) {
            continue;
        }
        L1Shift = L2Reg->intMapEntry.L1Shift;
        if( L1Shift >= 32 )
        {
            *pBitMaskHi |= 1ul<<(L1Shift-32);
        }
        else
        {
            *pBitMaskLo |= 1ul<<L1Shift;
        }
    }
}
#endif

BINT_CallbackHandle BINT_GetCallbackFirst( BINT_Handle intHandle )
{
    BDBG_OBJECT_ASSERT(intHandle, BINT);
    return BLST_S_FIRST(&(intHandle->allCbList));
}

BINT_CallbackHandle BINT_GetCallbackNext( BINT_CallbackHandle cbHandle )
{
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    return BLST_S_NEXT(cbHandle, allCbLink);
}

BERR_Code BINT_GetInterruptId( BINT_CallbackHandle cbHandle, BINT_Id *pIntId )
{
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    if (( cbHandle == NULL ) || ( pIntId == NULL ))
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    *pIntId = cbHandle->L2Handle->intId;

    return BERR_SUCCESS;
}

BERR_Code BINT_GetCallbackStatus( BINT_CallbackHandle cbHandle, bool *pbEnabled)
{
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    *pbEnabled = cbHandle->enabled;

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_AddBin( BINT_CallbackHandle cbHandle, uint32_t ulRangeMin, uint32_t ulRangeMax )
{
    uint32_t ulActiveBins;
    BINT_Stats_CallbackBin *pCurBinInfo = NULL;
    BINT_Stats_CallbackBin aTmpBinInfo[BINT_P_STATS_BIN_MAX];
    uint16_t i = 0;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    if ( ulRangeMin > ulRangeMax)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( cbHandle->StatInfo.bDefaultBins )
    {
        BINT_Stats_DestroyBins(cbHandle);
        cbHandle->StatInfo.bDefaultBins = false;
    }

    ulActiveBins = cbHandle->StatInfo.ulActiveBins;

    if ( ulActiveBins == BINT_P_STATS_BIN_MAX )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    for (i = 0; i <= ulActiveBins; i++)
    {
        pCurBinInfo = &(cbHandle->StatInfo.aBinInfo[i]);

        /* check for range overlap with current bin */
        if (( ulActiveBins > 0 ) &&
            ((( ulRangeMin >= pCurBinInfo->ulBinRangeMin ) &&
              ( ulRangeMin <= pCurBinInfo->ulBinRangeMax )) ||
             (( ulRangeMax >= pCurBinInfo->ulBinRangeMin ) &&
              ( ulRangeMax <= pCurBinInfo->ulBinRangeMax ))))
        {
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        /* check for bin insertion here */
        if ( ulRangeMax < pCurBinInfo->ulBinRangeMin )
        {
            /* shift bins for insertion */
            uint32_t ulBinCopyNum = ulActiveBins - i;
            BKNI_Memcpy(aTmpBinInfo, pCurBinInfo, sizeof(BINT_Stats_CallbackBin) * ulBinCopyNum);
            BKNI_Memcpy(pCurBinInfo + 1, aTmpBinInfo, sizeof(BINT_Stats_CallbackBin) * ulBinCopyNum);
        }

        /* set bin */
        if (( ulRangeMax < pCurBinInfo->ulBinRangeMin ) || (i == ulActiveBins))
        {
            pCurBinInfo->ulBinRangeMin = ulRangeMin;
            pCurBinInfo->ulBinRangeMax = ulRangeMax;
            pCurBinInfo->ulBinHitCount = 0;

            cbHandle->StatInfo.ulActiveBins++;
            break;
        }
    }

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_DestroyBins( BINT_CallbackHandle cbHandle )
{
    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    cbHandle->StatInfo.ulActiveBins = 0;

    BKNI_Memset(cbHandle->StatInfo.aBinInfo, 0,
                sizeof(BINT_Stats_CallbackBin) * BINT_P_STATS_BIN_MAX);

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_Get( BINT_CallbackHandle cbHandle, BINT_Stats_CallbackStats **ppCbStats )
{
    BINT_P_Context *intHandle;

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    intHandle = cbHandle->L2Handle->L2Reg->intHandle;

#ifndef BINT_STATS_ENABLE
    BDBG_WRN(("Stats tracking not enabled in compile."));
#endif /* BINT_STATS_ENABLE */

    *ppCbStats = &(cbHandle->StatInfo);

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_Reset( BINT_CallbackHandle cbHandle )
{
    uint16_t i;
    BINT_Stats_CallbackStats *pStatInfo = &(cbHandle->StatInfo);

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);

    BKNI_EnterCriticalSection();
    pStatInfo->ulTimeMin = UINT32_MAX;
    pStatInfo->ulTimeMax = 0;
    pStatInfo->ulTimeAvg = 0;
    pStatInfo->ulCbHitCount = 0;

    for (i = 0; i < pStatInfo->ulActiveBins; i++)
    {
        pStatInfo->aBinInfo[i].ulBinHitCount = 0;
    }
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_Enable( BINT_Handle intHandle, BTMR_Handle hTmrHandle )
{
    BERR_Code rc = BERR_SUCCESS;
    BTMR_TimerHandle hTimer = NULL;

    BTMR_Settings stSettings = { BTMR_Type_eSharedFreeRun,
                                 NULL,
                                 NULL,
                                 0,
                                 false };

    BDBG_OBJECT_ASSERT(intHandle, BINT);

    if ( intHandle->bStatsEnable == true )
    {
        return BERR_TRACE(BINT_STATS_ERR_ALREADY_ENABLED);
    }

    rc = BTMR_CreateTimer( hTmrHandle, &hTimer, &stSettings );
    if (rc != BERR_SUCCESS)
    {
        return rc;
    }

    intHandle->hTimer = hTimer;
    intHandle->bStatsEnable = true;

    return BERR_SUCCESS;
}

BERR_Code BINT_Stats_Disable( BINT_Handle intHandle )
{
    BDBG_OBJECT_ASSERT(intHandle, BINT);

    if ( intHandle->bStatsEnable == false )
    {
        return BERR_TRACE(BINT_STATS_ERR_ALREADY_DISABLED);
    }

    BDBG_ASSERT( intHandle->hTimer != NULL );
    BTMR_DestroyTimer(intHandle->hTimer);

    intHandle->bStatsEnable = false;

    return BERR_SUCCESS;
}

/* returns elapsed time in microseconds */
static uint32_t BINT_P_GetElapsedTime( uint32_t ulTimerStart, uint32_t ulTimerEnd )
{
    uint32_t ulTimerMax;
    uint32_t ulTimerElapsed;

    ulTimerMax = BTMR_ReadTimerMax();

    if (ulTimerEnd < ulTimerStart)
    {
        ulTimerElapsed = ((ulTimerMax - ulTimerStart) + ulTimerEnd);
    }
    else
    {
        ulTimerElapsed = (ulTimerEnd - ulTimerStart);
    }

    return ulTimerElapsed;
}

static BERR_Code BINT_P_Stats_ComputeStats( BINT_CallbackHandle cbHandle, uint32_t ulStart, uint32_t ulEnd )
{
    uint16_t i;
    uint32_t ulSampleNum = BINT_P_STATS_SAMPLE_MAX;
    uint32_t ulElapsedTime = BINT_P_GetElapsedTime( ulStart, ulEnd );
    BINT_Stats_CallbackStats *pStatInfo = &(cbHandle->StatInfo);

    BDBG_OBJECT_ASSERT(cbHandle, BINT_Callback);
    pStatInfo->ulCbHitCount++;


    /* calculate min, max and average times */
    if ( ulElapsedTime < pStatInfo->ulTimeMin)
    {
        pStatInfo->ulTimeMin = ulElapsedTime;
    }

    if ( ulElapsedTime > pStatInfo->ulTimeMax)
    {
        pStatInfo->ulTimeMax = ulElapsedTime;
    }

    if (ulSampleNum > pStatInfo->ulCbHitCount)
    {
        ulSampleNum = pStatInfo->ulCbHitCount;
    }

    pStatInfo->ulTimeAvg = ((pStatInfo->ulTimeAvg * (ulSampleNum - 1)) +
                                  ulElapsedTime) / ulSampleNum;
    pStatInfo->aulTimeStamp[pStatInfo->ulTimeStampStartIdx] = ulStart;

    /* check for callbacks that take too long */
    if (ulElapsedTime > BINT_P_STATS_EXECUTION_TIME_MAX_THRESHOLD)
    {
        BDBG_WRN(("BINT_Isr(%s) took %d msec", cbHandle->L2Handle->L2Reg->intMapEntry.L2Name, ulElapsedTime/1000));
    }

    /* check for runaway interrupts */
    if (ulSampleNum >= BINT_P_STATS_RECENT_CB_HIT_COUNT)
    {
        uint32_t ulTotalPeriod, ulAvgPeriod;
        uint32_t ulTimeStampEndIdx;

        if (pStatInfo->ulTimeStampStartIdx == BINT_P_STATS_RECENT_CB_HIT_COUNT - 1)
        {
            ulTimeStampEndIdx = 0;
        }
        else
        {
            ulTimeStampEndIdx = pStatInfo->ulTimeStampStartIdx + 1;
        }

        ulTotalPeriod = BINT_P_GetElapsedTime(pStatInfo->aulTimeStamp[ulTimeStampEndIdx],
                                              pStatInfo->aulTimeStamp[pStatInfo->ulTimeStampStartIdx]);

        ulAvgPeriod = ulTotalPeriod / BINT_P_STATS_RECENT_CB_HIT_COUNT;

#if 0
/* Commenting out this code because some interrupts fire faster than this for a period of time. */
        if (ulAvgPeriod < BINT_P_STATS_AVG_PERIOD_MIN_THRESHOLD)
        {
            BDBG_WRN(("BINT_Isr(%s) overflow, %d msec between hits",
            cbHandle->L2Handle->intHandle->pIntMap[cbHandle->L2Handle->intMapIndex].L2Name,
            ulAvgPeriod/1000));
        }
#endif

        pStatInfo->ulTimeStampStartIdx++;
        pStatInfo->ulTimeStampStartIdx = pStatInfo->ulTimeStampStartIdx % BINT_P_STATS_RECENT_CB_HIT_COUNT;
    }

    /* mark bin according to elapsed time */
    for (i = 0; i < pStatInfo->ulActiveBins; i++)
    {
        if ((ulElapsedTime >= pStatInfo->aBinInfo[i].ulBinRangeMin) &&
            (ulElapsedTime <= pStatInfo->aBinInfo[i].ulBinRangeMax))
        {
            pStatInfo->aBinInfo[i].ulBinHitCount++;
            break;
        }
    }

    return BERR_SUCCESS;
}

void BINT_DumpInfo(BINT_Handle intHandle)
{
    BINT_P_L2Handle L2Handle;
    unsigned L1Shift;
    BINT_CallbackHandle callback;
    bool l1_head, l2_head, int_head;
    const BINT_P_L2Register *L2Reg;

    BDBG_OBJECT_ASSERT(intHandle, BINT);
    BINT_LOCK(intHandle);

    for(int_head=false,L1Shift=0;L1Shift<BINT_P_L1_SIZE;L1Shift++) {
        for(L2Reg = BLST_S_FIRST(&intHandle->L1[L1Shift].L2RegList); L2Reg ; L2Reg = BLST_S_NEXT(L2Reg, link)) {
            for(l1_head=false,L2Handle=BLST_S_FIRST(&L2Reg->intList); L2Handle ; L2Handle=BLST_S_NEXT(L2Handle, link)) {
                if (!L2Handle->count) {
                    continue;
                }
                if (!int_head) {
                    BDBG_MSG(("------[%s dump]--------", intHandle->settings.name?intHandle->settings.name:"XXX"));
                    int_head=true;
                }
                if(!l1_head) {
                    BDBG_MSG((" %#x:%s", (unsigned)L1Shift, L2Reg->intMapEntry.L2Name));
                    l1_head=true;
                }
                for(l2_head=false,callback=BLST_S_FIRST(&L2Handle->callbackList); callback; callback=BLST_S_NEXT(callback, link)) {
                    if (!callback->count) {
                        continue;
                    }
                    if (!l2_head) {
                        BDBG_MSG(("   %#x:%u %s[%#x]:(%#x,%#x) %u %s", BCHP_INT_ID_GET_REG(L2Handle->intId), BCHP_INT_ID_GET_SHIFT(L2Handle->intId), callback->callbackName, (unsigned)callback->func, (unsigned)callback->pParm1, (unsigned)callback->parm2, callback->count, callback->enabled?"":"disabled"));
                        l2_head=true;
                    } else {
                        BDBG_MSG(("   >>> %s[%#x]:(%#x,%#x) %u %s", callback->callbackName, (unsigned)callback->func, (unsigned)callback->pParm1, (unsigned)callback->parm2, callback->count, callback->enabled?"":"disabled"));
                    }
#if 0
#ifdef BINT_STATS_ENABLE
                    if (intHandle->bStatsEnable) {
                        BDBG_MSG(("     elapsed: min %d, max %d, avg %d (usec)",
                            callback->StatInfo.ulTimeMin, callback->StatInfo.ulTimeMax, callback->StatInfo.ulTimeAvg));
                        callback->StatInfo.ulTimeMin = 0;
                        callback->StatInfo.ulTimeMax = 0;
                        callback->StatInfo.ulTimeAvg = 0;
                        callback->StatInfo.ulCbHitCount = 0;
                    }L2Reg->
#endif
#endif
                    callback->count=0;
                }
                L2Handle->count=0;
            }
        }
    }
    BINT_UNLOCK(intHandle);
    return;
}

