/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc.c $
 * $brcm_Revision: Hydra_Software_Devel/63 $
 * $brcm_Date: 10/24/12 3:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/src/bmrc.c $
 * 
 * Hydra_Software_Devel/63   10/24/12 3:51p vsilyaev
 * SW7435-444: Added Standby support
 * 
 * Hydra_Software_Devel/62   10/15/12 11:41p hongtaoz
 * SW7445-54: added support for 256 clients; fixed typo in client table;
 * 
 * Hydra_Software_Devel/61   10/3/12 5:51p albertl
 * SW7425-3721: Fixed warning.
 * 
 * Hydra_Software_Devel/60   9/11/12 2:12a albertl
 * SW7435-10: Fixed 8 ARC support.
 * 
 * Hydra_Software_Devel/59   8/15/12 5:43p albertl
 * SW7425-3721: Changed to return errors from callbacks.  Fixed coverity
 * issues.
 * 
 * Hydra_Software_Devel/58   8/14/12 3:56p albertl
 * SW7445-10: Added MRC support for 7445.
 * 
 * Hydra_Software_Devel/57   6/22/11 2:54p albertl
 * SW7405-5349: Changed to prevent MRC from making unnecessary register
 * writes if values have not changed.
 * 
 * Hydra_Software_Devel/56   1/5/11 9:15p albertl
 * SW7422-106, SW7422-168: Fixed macros for chips with less than 2 MEMCs.
 * 
 * Hydra_Software_Devel/55   1/5/11 8:15p albertl
 * SW7422-106, SW7422-168: Fixed MRC bugs pertaining to handling multiple
 * MEMCs and rewrote portions of BMRC_Monitor logic.  Client masks now
 * stored to match hardware.
 * 
 * Hydra_Software_Devel/54   12/29/10 3:18p pntruong
 * SW7422-168: Remove bogus warning msg.
 *
 * Hydra_Software_Devel/53   11/12/10 1:22p albertl
 * SW7125-27: Rolled back wrong error checking change and moved assert
 * after error checking.
 *
 * Hydra_Software_Devel/52   10/29/10 2:25p albertl
 * SW7125-27: Fixed to check eClient against BMRC_Client_eInvalid instead
 * of ulClientsIdx.
 *
 * Hydra_Software_Devel/51   3/2/10 1:23p erickson
 * SW7405-3892: convert #if and stale 3563 comment. clean up
 * BMRC_Checker_GetClientInfo failure.
 *
 * Hydra_Software_Devel/50   6/16/09 6:21p albertl
 * PR55764, PR55875, PR55993: Refactored MRC to keep chip specific
 * definitions in bmrc_priv.h and for maintainability.  Updated support
 * for 7420 b0, 7430, 35130, and 7550.
 *
 * Hydra_Software_Devel/49   6/4/09 7:05p albertl
 * PR55641: Allowed MRC to report maximum number of checkers and changed
 * BMRC_Monitor to use that instead of hardcoded maximums.
 *
 * Hydra_Software_Devel/48   4/23/09 2:42p jhaberf
 * PR53796: Added support for BCM35130 DTV chip
 *
 * Hydra_Software_Devel/47   4/16/09 2:54p nickh
 * PR54264: Add mrc for 7420 - 4 arc checkers for MEMC0 and 4 arc checkers
 * for MEMC1
 *
 * Hydra_Software_Devel/46   4/9/09 6:33p jrubio
 * PR36876: 7335/7325/7336 have 4 MEMC1 checkers
 *
 * Hydra_Software_Devel/45   4/8/09 5:06p erickson
 * PR53866: proper error recovery on BINT_CreateCallback and
 * BINT_ClearCallback
 *
 * Hydra_Software_Devel/44   2/17/09 6:42p albertl
 * PR51612: Updated to correct naming conventions.
 *
 * Hydra_Software_Devel/43   1/27/09 8:26p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/42   12/2/08 7:14p albertl
 * PR46761: Changed 7420 to use BCHP_INT_ID_MEMC_X_L2_ARC_X_INTR.
 *
 * Hydra_Software_Devel/41   7/23/08 1:44p katrep
 * PR45074:Fixed the typo error in conditional code
 *
 * Hydra_Software_Devel/40   4/11/08 5:29p albertl
 * PR17131:  Fixed missing XPT3 client id for 7401 and 7403.  Improved
 * client stringname handling in BMRC_P_Checker_Violation_isr.
 *
 * Hydra_Software_Devel/39   3/5/08 10:19p pntruong
 * PR34853: Added ifdef for 3556.
 *
 * Hydra_Software_Devel/38   2/26/08 12:20a albertl
 * PR36876:  Fixed compile errors with 7401 and 7403 not supporting
 * ARC_0_VIOLATION_INFO_CLEAR.
 *
 * Hydra_Software_Devel/37   2/25/08 7:04p albertl
 * PR36876:  Rewrote MRC to abstract client names and streamline adding of
 * future chips.  3548 support added.
 *
 * Hydra_Software_Devel/36   1/23/08 10:05p pntruong
 * PR37978: Synced up with central rdb.
 *
 * Hydra_Software_Devel/35   12/7/07 3:38p pntruong
 * PR37978: Add Intial support for 3563D0.
 *
 * Hydra_Software_Devel/35   12/7/07 3:35p pntruong
 * PR37978: Add Intial support for 3563D0.
 *
 * Hydra_Software_Devel/34   12/6/07 10:06a erickson
 * PR36876: convert NULL checks to BDBG_OBJECT which are more thorough.
 * eliminate duplicate #if BCHP_CHIP logic to keep in sync with header
 * file.
 *
 * Hydra_Software_Devel/33   11/19/07 3:22p tdo
 * PR36876: Add mrc support for 7335
 *
 * Hydra_Software_Devel/32   11/14/07 1:53p albertl
 * PR35322:  Updated mrc monitor to properly support 128 clients.
 *
 * Hydra_Software_Devel/31   10/19/07 4:14p jrubio
 * PR35015: adding support for 7325
 *
 * Hydra_Software_Devel/30   5/2/07 7:01p albertl
 * PR29633:  Fixed for 7400 a0.
 *
 * Hydra_Software_Devel/29   5/2/07 11:46a pntruong
 * PR29633: Fixed build error and commented out debug printfs.
 *
 * Hydra_Software_Devel/29   5/2/07 11:45a pntruong
 * PR29633: Fixed build error and commented out debug printfs.
 *
 * Hydra_Software_Devel/28   5/1/07 5:07p albertl
 * PR29633:  Updated and changed MRC for 7405.
 *
 * Hydra_Software_Devel/27   3/9/07 7:18p albertl
 * PR28182:  No longer programs ARC when aligned size is 0.  Memc id now
 * reported on violations.
 *
 * Hydra_Software_Devel/26   3/1/07 8:16p albertl
 * PR26116:  Fixed support for multiple memory modules for 7400 and 3563.
 *
 * Hydra_Software_Devel/25   1/17/07 6:38p albertl
 * PR19101:  Updated violation messages to indicate exclusive mode for
 * clarity.
 *
 * Hydra_Software_Devel/24   12/18/06 11:37p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   10/13/06 9:28p albertl
 * PR23361:  Added multiple mrc support for 3563.
 *
 * Hydra_Software_Devel/22   9/13/06 10:06p pntruong
 * PR24183, PR24184: Fixed 7118 build errors.
 *
 * Hydra_Software_Devel/21   9/12/06 4:12p pntruong
 * PR24183, PR24184: compile erros in bmrc.c for 97038 build.
 *
 * Hydra_Software_Devel/20   8/9/06 3:47p albertl
 * PR23361:  Fixed changes that broke 7401.
 *
 * Hydra_Software_Devel/19   8/8/06 6:19p albertl
 * PR23361:  Changed to compile for 3563.
 *
 * Hydra_Software_Devel/18   6/13/06 6:07p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/17   3/8/06 5:30p albertl
 * PR19101:  BMRC_Checker_SetRange now only accepts 8 byte alligned
 * addresses and sizes, and returns error otherwise.
 *
 * Hydra_Software_Devel/16   3/7/06 6:32p albertl
 * PR19101:  Fixed BMRC_Checker_SetRange so actual hardware range is only
 * as large as needed to cover specified size.
 *
 * Hydra_Software_Devel/15   3/7/06 3:54p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/14   3/3/06 7:24p albertl
 * PR18701:  Added functions to disable and enable a checker's callback.
 *
 * Hydra_Software_Devel/13   2/16/06 7:32p albertl
 * PR19565: Added support for 7438.
 *
 * Hydra_Software_Devel/12   2/16/06 7:28p albertl
 * PR19669:  Moved private defines from public bmrc.h file to bmrc.c.
 *
 * Hydra_Software_Devel/11   1/17/06 4:53p hongtaoz
 * PR19082: support 7400;
 *
 * Hydra_Software_Devel/10   11/29/05 1:16p albertl
 * PR18307:  usActiveCheckers now initialized properly in BMRC_Open.
 * BMRC_Create_Checker now leaves critical section when an error occurs.
 *
 * Hydra_Software_Devel/9   10/7/05 4:33p hongtaoz
 * PR17460: adapted to the new RDB header files;
 *
 * Hydra_Software_Devel/8   9/23/05 7:06p hongtaoz
 * PR17131: added BMRC support for 7401;
 *
 * Hydra_Software_Devel/7   9/22/05 4:34p albertl
 * PR17164:  Added missing write to ARC_0_CNTRL on disable and
 * added missing offset to handle all checkers.
 *
 * Hydra_Software_Devel/6   8/9/05 3:05p pntruong
 * PR13641:  Added BMRC error codes.
 *
 * Hydra_Software_Devel/5   8/8/05 9:49p albertl
 * PR13641:  Added BMRC error codes.  Callback status cleared when
 * enabling, disabling, and on violations.
 *
 * Hydra_Software_Devel/4   8/4/05 6:50p albertl
 * PR13641:  Fixed incorrect PR numbers in chagelog.
 *
 * Hydra_Software_Devel/2   8/3/05 8:34p albertl
 * PR13641:  Corrected addresses by shifting them the proper bits before
 * reading from or writing to hardware.
 *
 * Hydra_Software_Devel/1   8/1/05 10:11p albertl
 * PR13641:  Initial Revision.
 *
 ***************************************************************************/

#include "bmrc.h"
#include "bmrc_priv.h"
#include "bstd.h"                /* standard types */
#include "bkni.h"                /* Memory management */
#include "bdbg.h"                /* Debug message */

#if (BMRC_P_CHECKER_USE_MEMC_GEN_VER > 1)
#if (BMRC_P_MEMC_NUM > 2)
#include "bchp_memc_gen_2.h"
#include "bchp_memc_gen_2_2.h"
#endif
#if (BMRC_P_MEMC_NUM > 1)
#include "bchp_memc_gen_1.h"
#include "bchp_memc_gen_2_1.h"
#endif
#include "bchp_memc_gen_0.h"
#include "bchp_memc_gen_2_0.h"

#elif BMRC_P_CHECKER_USE_MEMC_GEN_VER
#if (BMRC_P_MEMC_NUM > 1)
#include "bchp_memc_gen_1.h"
#endif
#include "bchp_memc_gen_0.h"

#else
#if (BMRC_P_MEMC_NUM > 2)
#include "bchp_memc_2.h"
#endif
#if (BMRC_P_MEMC_NUM > 1)
#include "bchp_memc_1.h"
#endif
#include "bchp_memc_0.h"
#endif

#if (BMRC_P_MEMC_NUM > 2)
#define BMRC_P_BCHP_MEMC_REG(hMrc, reg) ((hMrc->usMemcId == 0) ? BMRC_P_BCHP_MEMC_0_REG(reg) : \
                                         (hMrc->usMemcId == 1) ? BMRC_P_BCHP_MEMC_1_REG(reg) : \
                                         BMRC_P_BCHP_MEMC_2_REG(reg))
#elif (BMRC_P_MEMC_NUM > 1)
#define BMRC_P_BCHP_MEMC_REG(hMrc, reg) ((hMrc->usMemcId == 0) ? BMRC_P_BCHP_MEMC_0_REG(reg) : \
                                         BMRC_P_BCHP_MEMC_1_REG(reg))
#else
#define BMRC_P_BCHP_MEMC_REG(hMrc, reg) (BMRC_P_BCHP_MEMC_0_REG(reg))
#endif

#if (BMRC_P_CHECKER_USE_MEMC_GEN_VER > 2)
#define BMRC_P_BCHP_MEMC_0_REG(reg) BCHP_MEMC_GEN_2_0##_##reg
#define BMRC_P_BCHP_MEMC_1_REG(reg) BCHP_MEMC_GEN_2_1##_##reg
#define BMRC_P_BCHP_MEMC_2_REG(reg) BCHP_MEMC_GEN_2_2##_##reg
#define BMRC_P_GET_FIELD_DATA(mem, reg, field) BCHP_GET_FIELD_DATA(mem, MEMC_GEN_2_0##_##reg, field)
#define BMRC_P_FIELD_DATA(reg, field, data)    BCHP_FIELD_DATA(MEMC_GEN_2_0##_##reg, field, data)
#define BMRC_P_FIELD_ENUM(reg, field, name)    BCHP_FIELD_ENUM(MEMC_GEN_2_0##_##reg, field, name)

#elif BMRC_P_CHECKER_USE_MEMC_GEN_VER
#define BMRC_P_BCHP_MEMC_0_REG(reg) BCHP_MEMC_GEN_0##_##reg
#define BMRC_P_BCHP_MEMC_1_REG(reg) BCHP_MEMC_GEN_1##_##reg
#define BMRC_P_BCHP_MEMC_2_REG(reg) BCHP_MEMC_GEN_2##_##reg
#define BMRC_P_GET_FIELD_DATA(mem, reg, field) BCHP_GET_FIELD_DATA(mem, MEMC_GEN_0##_##reg, field)
#define BMRC_P_FIELD_DATA(reg, field, data)    BCHP_FIELD_DATA(MEMC_GEN_0##_##reg, field, data)
#define BMRC_P_FIELD_ENUM(reg, field, name)    BCHP_FIELD_ENUM(MEMC_GEN_0##_##reg, field, name)

#else
#define BMRC_P_BCHP_MEMC_0_REG(reg) BCHP_MEMC_0##_##reg
#define BMRC_P_BCHP_MEMC_1_REG(reg) BCHP_MEMC_1##_##reg
#define BMRC_P_BCHP_MEMC_2_REG(reg) BCHP_MEMC_2##_##reg
#define BMRC_P_GET_FIELD_DATA(mem, reg, field) BCHP_GET_FIELD_DATA(mem, MEMC_0##_##reg, field)
#define BMRC_P_FIELD_DATA(reg, field, data)    BCHP_FIELD_DATA(MEMC_0##_##reg, field, data)
#define BMRC_P_FIELD_ENUM(reg, field, name)    BCHP_FIELD_ENUM(MEMC_0##_##reg, field, name)

/* workaround to build on chips where these fields are not symetrical across MRCs, like 7400 */
#ifndef BCHP_MEMC_1_ARC_0_READ_RIGHTS_HIGH
#define BCHP_MEMC_1_ARC_0_READ_RIGHTS_HIGH 0
#endif
#ifndef BCHP_MEMC_1_ARC_0_WRITE_RIGHTS_HIGH
#define BCHP_MEMC_1_ARC_0_WRITE_RIGHTS_HIGH 0
#endif
#ifndef BCHP_MEMC_2_ARC_0_READ_RIGHTS_HIGH
#define BCHP_MEMC_2_ARC_0_READ_RIGHTS_HIGH 0
#endif
#ifndef BCHP_MEMC_2_ARC_0_WRITE_RIGHTS_HIGH
#define BCHP_MEMC_2_ARC_0_WRITE_RIGHTS_HIGH 0
#endif

#endif


/* offsets */
#define BMRC_P_CHECKER_REG_OFFSET(hMrc) (BMRC_P_BCHP_MEMC_REG(hMrc, ARC_1_CNTRL) - BMRC_P_BCHP_MEMC_REG(hMrc, ARC_0_CNTRL))
#define BMRC_P_CHECKER_REG_IDX(hMrc, reg) ((BMRC_P_BCHP_MEMC_REG(hMrc, reg) - BMRC_P_BCHP_MEMC_REG(hMrc, ARC_0_CNTRL)) / sizeof(uint32_t))
#define BMRC_P_CHECKER_REG_SIZE  (BMRC_P_BCHP_MEMC_0_REG(ARC_1_CNTRL) - BMRC_P_BCHP_MEMC_0_REG(ARC_0_CNTRL))

#define BMRC_P_MEMC_0_REG_OFFSET 0

#if (BMRC_P_MEMC_NUM > 1)
    #define BMRC_P_MEMC_1_REG_OFFSET (BMRC_P_BCHP_MEMC_1_REG(ARC_0_CNTRL) - BMRC_P_BCHP_MEMC_0_REG(ARC_0_CNTRL))
#else
    #define BMRC_P_MEMC_1_REG_OFFSET 0
#endif

#if (BMRC_P_MEMC_NUM > 2)
    #define BMRC_P_MEMC_2_REG_OFFSET (BMRC_P_BCHP_MEMC_2_REG(ARC_0_CNTRL) - BMRC_P_BCHP_MEMC_0_REG(ARC_0_CNTRL))
#else
    #define BMRC_P_MEMC_2_REG_OFFSET 0
#endif

#if !BMRC_P_CHECKER_USE_NEW_NAME_SUFFIX
#define BMRC_P_NMBX_REG_OFFSET    (BCHP_MEMC_0_NMBX1 - BCHP_MEMC_0_NMBX0)
#endif

#define BMRC_P_Checker_Read32(hMrc, hChecker, reg)         BREG_Read32 (hMrc->hReg, BMRC_P_BCHP_MEMC_REG(hMrc, reg) + hChecker->ulRegOffset)
#define BMRC_P_Checker_Write32(hMrc, hChecker, reg, data)  if(hChecker->aulPrevRegTbl[BMRC_P_CHECKER_REG_IDX(hMrc, reg)] != data)                        \
															   BREG_Write32(hMrc->hReg, BMRC_P_BCHP_MEMC_REG(hMrc, reg) + hChecker->ulRegOffset, data);  \
	                                                       hChecker->aulPrevRegTbl[BMRC_P_CHECKER_REG_IDX(hMrc, reg)] = data


#define BMRC_P_MEMC_ADRS_SHIFT 3
#define BMRC_P_RANGE_ALIGNMENT_MASK ~0x00000007

#define BMRC_P_CHECKER_COUNT_MAX 8


BDBG_MODULE(BMRC);
BDBG_OBJECT_ID(BMRC);
BDBG_OBJECT_ID(BMRC_Checker);

extern const BMRC_P_ClientEntry BMRC_P_astClientTbl[];

typedef struct BMRC_P_CheckerContext
{
    BDBG_OBJECT(BMRC_Checker)
    BMRC_Handle hMrc;
    uint16_t usCheckerId;
    bool bExclusive;
    uint32_t ulRegOffset;
    uint32_t ulStart;
    uint32_t ulSize;
    BMRC_AccessType eCheckType;
    BMRC_AccessType eBlockType;
    uint32_t aulReadClients[BMRC_P_CLIENTS_ARRAY_SIZE];
    uint32_t aulWriteClients[BMRC_P_CLIENTS_ARRAY_SIZE];
    bool bEnabled;
    bool bActive;
    bool interruptEnabled;

	/* previous register values */
	uint32_t aulPrevRegTbl[BMRC_P_CHECKER_REG_SIZE];

    /* callback data */
    BINT_Id InterruptName;
    BINT_CallbackHandle hCallback;
    BMRC_CallbackFunc_isr pfCbFunc;
    void *pvCbData1;
    int iCbData2;
    BMRC_CheckerInfo stCheckerInfo;

} BMRC_P_CheckerContext;

typedef struct BMRC_P_Context
{
    BDBG_OBJECT(BMRC)
    BREG_Handle           hReg;
    BINT_Handle           hInt;
    BMRC_Settings         stSettings;
    int32_t               lRegOffset;
    uint16_t              usMemcId;
    uint16_t              usMaxCheckers;
    uint16_t              usActiveCheckers;
    bool                  suspended;
    BMRC_P_CheckerContext aCheckers[BMRC_P_CHECKER_COUNT_MAX];

} BMRC_P_Context;

typedef struct BMRC_P_MemcInfo
{
    int32_t  lRegOffset;
    uint16_t usMaxCheckers;

} BMRC_P_MemcInfo;

static const BMRC_Settings s_stDefaultSettings = {
    0 /* Memc Module Id */
};


static const BINT_Id s_saIntIdTbl[][BMRC_P_CHECKER_COUNT_MAX] =
{
    /* memc 0 */
    {
        BMRC_P_MEMC_0_ARC_0_INTR,
        BMRC_P_MEMC_0_ARC_1_INTR,
        BMRC_P_MEMC_0_ARC_2_INTR,
        BMRC_P_MEMC_0_ARC_3_INTR,
        BMRC_P_MEMC_0_ARC_4_INTR,
        BMRC_P_MEMC_0_ARC_5_INTR,
        BMRC_P_MEMC_0_ARC_6_INTR,
        BMRC_P_MEMC_0_ARC_7_INTR
    },

    /* memc 1 */
    {
        BMRC_P_MEMC_1_ARC_0_INTR,
        BMRC_P_MEMC_1_ARC_1_INTR,
        BMRC_P_MEMC_1_ARC_2_INTR,
        BMRC_P_MEMC_1_ARC_3_INTR,
        BMRC_P_MEMC_1_ARC_4_INTR,
        BMRC_P_MEMC_1_ARC_5_INTR,
        BMRC_P_MEMC_1_ARC_6_INTR,
        BMRC_P_MEMC_1_ARC_7_INTR
    },

    /* memc 2 */
    {
        BMRC_P_MEMC_2_ARC_0_INTR,
        BMRC_P_MEMC_2_ARC_1_INTR,
        BMRC_P_MEMC_2_ARC_2_INTR,
        BMRC_P_MEMC_2_ARC_3_INTR,
        BMRC_P_MEMC_2_ARC_4_INTR,
        BMRC_P_MEMC_2_ARC_5_INTR,
        BMRC_P_MEMC_2_ARC_6_INTR,
        BMRC_P_MEMC_2_ARC_7_INTR
    }
};

static const BMRC_P_MemcInfo st_aMemcInfo[] = {
    /* offset,                   max checker count */
    {BMRC_P_MEMC_0_REG_OFFSET, BMRC_P_MEMC_0_CHECKER_COUNT_MAX},
    {BMRC_P_MEMC_1_REG_OFFSET, BMRC_P_MEMC_1_CHECKER_COUNT_MAX},
    {BMRC_P_MEMC_2_REG_OFFSET, BMRC_P_MEMC_2_CHECKER_COUNT_MAX}
};

/* set BMRC_STUB to 1 if you want to test without MRC code */
#define BMRC_STUB 0

#if !BMRC_STUB
void BMRC_P_Checker_Violation_isr ( void *pvData1, int iData2 );
BERR_Code BMRC_P_Checker_WriteRegs ( BMRC_Checker_Handle hChecker );

/***************************************************************************
 *
 */
BERR_Code
BMRC_GetDefaultSettings
    ( BMRC_Settings *pDefSettings )
{
    if (!pDefSettings)
    {
        return BERR_INVALID_PARAMETER;
    }

    *pDefSettings = s_stDefaultSettings;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Open
    ( BMRC_Handle                     *phMrc,
      BREG_Handle                      hRegister,
      BINT_Handle                      hInterrupt,
      const BMRC_Settings             *pDefSettings )
{
    BMRC_Handle hMrc = NULL;
    BERR_Code err = BERR_SUCCESS;

    hMrc = (BMRC_Handle) BKNI_Malloc(sizeof(BMRC_P_Context));

    if (!hMrc)
    {
        BDBG_ERR(( "Out of System Memory" ));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BDBG_OBJECT_INIT(hMrc, BMRC);

    hMrc->hReg = hRegister;
    hMrc->hInt = hInterrupt;

    /* Take in default settings. */
    hMrc->stSettings = (pDefSettings) ? *pDefSettings : s_stDefaultSettings;

    if (hMrc->stSettings.usMemcId >= BMRC_P_MEMC_NUM)
    {
        BDBG_ERR(("MemcId %d not supported on this chipset", hMrc->stSettings.usMemcId));
        err = BERR_NOT_SUPPORTED;
        goto error;
    }

    hMrc->usMemcId = hMrc->stSettings.usMemcId;

    /* BMRC_P_MRC_1_REG_OFFSET is actually negative for 3563,
       hence signed long for lRegOffset */
    hMrc->lRegOffset = st_aMemcInfo[hMrc->usMemcId].lRegOffset;

    hMrc->usMaxCheckers = st_aMemcInfo[hMrc->usMemcId].usMaxCheckers;
    hMrc->usActiveCheckers = 0;
    hMrc->suspended = false;
    BKNI_Memset(hMrc->aCheckers, 0, sizeof(BMRC_P_CheckerContext) * BMRC_P_CHECKER_COUNT_MAX);

    *phMrc = hMrc;

    return BERR_SUCCESS;

error:
    BKNI_Free(hMrc);
    return err;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Close
    ( BMRC_Handle hMrc )
{
    BDBG_OBJECT_ASSERT(hMrc, BMRC);

    BDBG_OBJECT_DESTROY(hMrc, BMRC);
    BKNI_Free(hMrc);

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
void BMRC_GetSettings ( BMRC_Handle hMrc, BMRC_Settings *pSettings )
{
    BDBG_OBJECT_ASSERT(hMrc, BMRC);
    *pSettings = hMrc->stSettings;
    return;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_GetMaxCheckers
    ( BMRC_Handle hMrc,
      uint32_t *pulMaxChecker )
{
    BDBG_OBJECT_ASSERT(hMrc, BMRC);

    *pulMaxChecker = hMrc->usMaxCheckers;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_Create
    ( BMRC_Handle hMrc,
      BMRC_Checker_Handle *phChecker )
{
    int i = 0;
    uint32_t ulReg = 0;
    BMRC_P_CheckerContext *pCurChecker = NULL;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(hMrc, BMRC);
    if(hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BKNI_EnterCriticalSection();
    if (hMrc->usActiveCheckers >= hMrc->usMaxCheckers)
    {
        BDBG_ERR(("Maximum number of checkers reached.  Cannot create additional checkers."));
        BKNI_LeaveCriticalSection();

        return BERR_TRACE(BMRC_CHECKER_ERR_ALL_USED);
    }

    for (i = 0; i < hMrc->usMaxCheckers; i++)
    {
        pCurChecker = &(hMrc->aCheckers[i]);

        if (!pCurChecker->bActive)
        {
            hMrc->usActiveCheckers++;
            pCurChecker->hMrc = hMrc;
            pCurChecker->usCheckerId = i;
#if (BMRC_P_CHECKER_USE_MEMC_GEN_VER == 2) /* hack for 7435's MEMC ARC arraingment */
            pCurChecker->ulRegOffset = (i * BMRC_P_CHECKER_REG_OFFSET(hMrc)) + ((i >= 4)? 
				                       (BCHP_MEMC_GEN_2_0_ARC_4_CNTRL - BCHP_MEMC_GEN_0_MEMC64_MBIST_TM_CNTRL): 0);
#else
            pCurChecker->ulRegOffset = i * BMRC_P_CHECKER_REG_OFFSET(hMrc);
#endif
            pCurChecker->ulStart = 0;
            pCurChecker->ulSize = 0;
            pCurChecker->bExclusive = false;
            pCurChecker->eCheckType = BMRC_AccessType_eNone;
            pCurChecker->eBlockType = BMRC_AccessType_eNone;
            pCurChecker->bEnabled = false;
            pCurChecker->bActive = true;
            pCurChecker->interruptEnabled = false;

            pCurChecker->InterruptName = s_saIntIdTbl[hMrc->usMemcId][i];
            pCurChecker->hCallback = 0;
            pCurChecker->pfCbFunc = NULL;
            pCurChecker->pvCbData1 = NULL;
            pCurChecker->iCbData2 = 0;

            BKNI_Memset(pCurChecker->aulReadClients, 0, sizeof(pCurChecker->aulReadClients));
            BKNI_Memset(pCurChecker->aulWriteClients, 0, sizeof(pCurChecker->aulWriteClients));
            BKNI_Memset(pCurChecker->aulPrevRegTbl, 0, sizeof(pCurChecker->aulPrevRegTbl));

            *phChecker = (BMRC_Checker_Handle)pCurChecker;

            BDBG_OBJECT_SET(pCurChecker, BMRC_Checker);
            break;
        }
    }
    BKNI_LeaveCriticalSection();

    BDBG_ASSERT(*phChecker);

#if BMRC_P_CHECKER_USE_VIOLATION_INFO_CLEAR
    /* clear out any previous violation */
    ulReg = BMRC_P_FIELD_DATA(ARC_0_VIOLATION_INFO_CLEAR, WRITE_CLEAR, 1);
    BMRC_P_Checker_Write32(hMrc, pCurChecker, ARC_0_VIOLATION_INFO_CLEAR, ulReg);

    ulReg = BMRC_P_FIELD_DATA(ARC_0_VIOLATION_INFO_CLEAR, WRITE_CLEAR, 0);
    BMRC_P_Checker_Write32(hMrc, pCurChecker, ARC_0_VIOLATION_INFO_CLEAR, ulReg);
#else
    BSTD_UNUSED(ulReg);
#endif

    rc = BINT_CreateCallback(&(pCurChecker->hCallback),
        hMrc->hInt,
        pCurChecker->InterruptName,
        BMRC_P_Checker_Violation_isr,
        (void*)pCurChecker, pCurChecker->usCheckerId);
    if (rc) return BERR_TRACE(rc);

    rc = BINT_ClearCallback(pCurChecker->hCallback);
    if (rc) {
        BINT_DestroyCallback(pCurChecker->hCallback);
        return BERR_TRACE(rc);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_Destroy
    ( BMRC_Checker_Handle hChecker )
{
    BMRC_Handle hMrc;

    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    hMrc = hChecker->hMrc;
    BDBG_OBJECT_ASSERT(hMrc, BMRC);

    BMRC_Checker_Disable(hChecker);
    BMRC_Checker_DisableCallback(hChecker);
    BINT_DestroyCallback(hChecker->hCallback);

    hChecker->bActive = false;
    hMrc->usActiveCheckers--;
    BDBG_OBJECT_UNSET(hChecker, BMRC_Checker);

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetRange
    ( BMRC_Checker_Handle hChecker,
      uint32_t ulStart,
      uint32_t ulSize )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if (ulSize == 0)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ((ulSize != (ulSize & BMRC_P_RANGE_ALIGNMENT_MASK)) ||
        (ulStart != (ulStart & BMRC_P_RANGE_ALIGNMENT_MASK)))
    {
        BDBG_ERR(( "ulStart address and ulSize must be 8 byte aligned." ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    hChecker->ulStart = ulStart;
    hChecker->ulSize = ulSize;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetAccessCheck
    ( BMRC_Checker_Handle hChecker,
      BMRC_AccessType eAccessType )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    if (eAccessType == BMRC_AccessType_eNone)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    hChecker->eCheckType = eAccessType;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetBlock
    ( BMRC_Checker_Handle hChecker,
      BMRC_AccessType eBlockType )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    hChecker->eBlockType = eBlockType;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetExclusive
    ( BMRC_Checker_Handle hChecker,
      bool bExclusive )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    hChecker->bExclusive = bExclusive;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetClient
    ( BMRC_Checker_Handle hChecker,
      BMRC_Client eClient,
      BMRC_AccessType eAccessType )
{
    uint16_t usClientId = BMRC_P_GET_CLIENT_ID(hChecker->hMrc->usMemcId, eClient);
    uint32_t ulClientsIdx = usClientId / BMRC_P_CLIENTS_ARRAY_ELEMENT_SIZE;
    uint32_t ulClientsShift = (usClientId % BMRC_P_CLIENTS_ARRAY_ELEMENT_SIZE);
    uint32_t ulClientsMask = 1 << ulClientsShift;

    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);
    BDBG_ASSERT(BMRC_P_astClientTbl[eClient].eClient == eClient);

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    if (usClientId >= BMRC_Client_eInvalid)
    {
        BDBG_ERR(( "Client %s not supported on this platform.",  BMRC_P_astClientTbl[eClient].pchClientName));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_ASSERT(ulClientsIdx < BMRC_P_CLIENTS_ARRAY_SIZE);

    if (eAccessType == BMRC_AccessType_eRead)
    {
        hChecker->aulReadClients[ulClientsIdx] |= ulClientsMask;
        hChecker->aulWriteClients[ulClientsIdx] &= ~ulClientsMask;
    }

    if (eAccessType == BMRC_AccessType_eWrite)
	{
        hChecker->aulReadClients[ulClientsIdx] &= ~ulClientsMask;
        hChecker->aulWriteClients[ulClientsIdx] |= ulClientsMask;
	}
		
	if (eAccessType == BMRC_AccessType_eBoth)
    {
        hChecker->aulReadClients[ulClientsIdx] |= ulClientsMask;
        hChecker->aulWriteClients[ulClientsIdx] |= ulClientsMask;
    }

	if (eAccessType == BMRC_AccessType_eNone)
	{
        hChecker->aulReadClients[ulClientsIdx] &= ~ulClientsMask;
        hChecker->aulWriteClients[ulClientsIdx] &= ~ulClientsMask;
	}

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_Enable
    ( BMRC_Checker_Handle hChecker )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    hChecker->bEnabled = true;
    BMRC_P_Checker_WriteRegs(hChecker);

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_Disable
    ( BMRC_Checker_Handle hChecker )
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    hChecker->bEnabled = false;
    BMRC_P_Checker_WriteRegs(hChecker);

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_EnableCallback
    ( BMRC_Checker_Handle hChecker )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if (!hChecker->hCallback)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_NO_CALLBACK_SET);
    }

	rc = BINT_ClearCallback(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
	rc = BINT_EnableCallback(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    hChecker->interruptEnabled = true;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_DisableCallback
    ( BMRC_Checker_Handle hChecker )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    rc = BINT_DisableCallback(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    rc = BINT_ClearCallback(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    hChecker->interruptEnabled = false;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_EnableCallback_isr
    ( BMRC_Checker_Handle hChecker )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if (!hChecker->hCallback)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_NO_CALLBACK_SET);
    }

    rc = BINT_ClearCallback_isr(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    rc = BINT_EnableCallback_isr(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    hChecker->interruptEnabled = true;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_DisableCallback_isr
    ( BMRC_Checker_Handle hChecker )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if(hChecker->hMrc->suspended) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    rc = BINT_DisableCallback_isr(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    rc = BINT_ClearCallback_isr(hChecker->hCallback);
	if (rc) return BERR_TRACE(rc);
    hChecker->interruptEnabled = false;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_SetCallback
    ( BMRC_Checker_Handle hChecker,
      const BMRC_CallbackFunc_isr pfCbFunc,
      void *pvCbData1,
      int iCbData2)
{
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);

    if (hChecker->bEnabled)
    {
        return BERR_TRACE(BMRC_CHECKER_ERR_ENABLED_CANT_SET);
    }

    hChecker->pfCbFunc = pfCbFunc;
    hChecker->pvCbData1 = pvCbData1;
    hChecker->iCbData2 = iCbData2;

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
void BMRC_P_Checker_Violation_isr
    ( void *pvData1,
      int iData2 )
{
    uint32_t ulReg = 0;
    BMRC_Checker_Handle hChecker = (BMRC_Checker_Handle)pvData1;
    BMRC_CheckerInfo *pCheckerInfo = &(hChecker->stCheckerInfo);
    BMRC_Handle hMrc = hChecker->hMrc;
    BMRC_ClientInfo stClientInfo;
    BERR_Code rc;

    BSTD_UNUSED(iData2);
    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);
    BDBG_OBJECT_ASSERT(hMrc, BMRC);
    if(hMrc->suspended) {
        return ;
    }

    pCheckerInfo->usMemcId = hMrc->usMemcId;
    pCheckerInfo->usCheckerId = hChecker->usCheckerId;
    pCheckerInfo->ulStart = hChecker->ulStart;
    pCheckerInfo->ulSize = hChecker->ulSize;
    pCheckerInfo->bExclusive = hChecker->bExclusive;

#if !BMRC_P_CHECKER_USE_NEW_NAME_SUFFIX
    ulReg = BMRC_P_Checker_Read32(hMrc, hChecker, ARC_0_VIOLATION_INFO_LOW));
    pCheckerInfo->ulAddress = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_LOW, ADDRESS) << BMRC_P_MEMC_ADRS_SHIFT;

    ulReg = BMRC_P_Checker_Read32(hMrc, hChecker, ARC_0_VIOLATION_INFO_HIGH));
    pCheckerInfo->ulReqType = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_HIGH, REQ_TYPE);

#if BMRC_P_CHECKER_USE_NMBX_ID
    pCheckerInfo->usClientId = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_HIGH, CLIENT_ID);
    pCheckerInfo->ulNmbxId = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_HIGH, NMBX_ID);

    ulReg = BREG_Read32(hMrc->hReg, BCHP_MEMC_0_NMBX0 + (pCheckerInfo->ulNmbxId * BMRC_P_NMBX_REG_OFFSET) + hMrc->lRegOffset + hChecker->ulRegOffset);
    pCheckerInfo->ulNmbx = BMRC_P_GET_FIELD_DATA(ulReg, MEMC_0_NMBX0, NMBX);
#else
    pCheckerInfo->usClientId = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_HIGH, CLIENTID);
    pCheckerInfo->ulNmbx = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_HIGH, NMBX);
#endif

#else
    ulReg = BMRC_P_Checker_Read32(hMrc, hChecker, ARC_0_VIOLATION_INFO_START_ADDR);
    pCheckerInfo->ulAddress = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_START_ADDR, ADDRESS) << BMRC_P_MEMC_ADRS_SHIFT;

    ulReg = BMRC_P_Checker_Read32(hMrc, hChecker, ARC_0_VIOLATION_INFO_END_ADDR);
    pCheckerInfo->ulAddressEnd = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_END_ADDR, ADDRESS) << BMRC_P_MEMC_ADRS_SHIFT;

    ulReg = BMRC_P_Checker_Read32(hMrc, hChecker, ARC_0_VIOLATION_INFO_CMD);
    pCheckerInfo->ulReqType = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_CMD, REQ_TYPE);
    pCheckerInfo->usClientId = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_CMD, CLIENTID);
    pCheckerInfo->ulNmbxId = BMRC_P_GET_FIELD_DATA(ulReg, ARC_0_VIOLATION_INFO_CMD, NMB);
#endif

    BMRC_Checker_GetClientInfo(hMrc, BMRC_P_GET_CLIENT_ENUM(hMrc->usMemcId, pCheckerInfo->usClientId), &stClientInfo);
    pCheckerInfo->pchClientName = stClientInfo.pchClientName;

#if BMRC_P_CHECKER_USE_VIOLATION_INFO_CLEAR
    /* clear out violation */
    ulReg = BMRC_P_FIELD_DATA(ARC_0_VIOLATION_INFO_CLEAR, WRITE_CLEAR, 1);
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_VIOLATION_INFO_CLEAR, ulReg);

    ulReg = BMRC_P_FIELD_DATA(ARC_0_VIOLATION_INFO_CLEAR, WRITE_CLEAR, 0);
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_VIOLATION_INFO_CLEAR, ulReg);
#endif

    if (hChecker->pfCbFunc)
    {
        hChecker->pfCbFunc(hChecker->pvCbData1, hChecker->iCbData2, pCheckerInfo);
    }

    BDBG_MSG(("clear callback"));
    rc = BINT_ClearCallback_isr(hChecker->hCallback);
	if (rc) rc = BERR_TRACE(rc);

    return;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_P_Checker_WriteRegs
    ( BMRC_Checker_Handle hChecker )
{
    BMRC_Handle hMrc;
    uint32_t ulReg = 0;

    BDBG_OBJECT_ASSERT(hChecker, BMRC_Checker);
    hMrc = hChecker->hMrc;

    if(hMrc->suspended) {
        return  BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if (!hChecker->bEnabled)
    {
        /* disable checker and exit */
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CHECK, DISABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CHECK, DISABLED);

        BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_CNTRL, ulReg);

        return BERR_SUCCESS;
    }

    /* write clients */
#if BMRC_P_CLIENTS_MAX == 64
    ulReg = 0;
    ulReg |= hChecker->aulReadClients[0];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_LOW, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[0];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_LOW, ulReg);

	if (((hMrc->usMemcId == 0) && BCHP_MEMC_0_ARC_0_READ_RIGHTS_HIGH) ||
		((hMrc->usMemcId == 1) && BCHP_MEMC_1_ARC_0_READ_RIGHTS_HIGH) ||
		((hMrc->usMemcId == 2) && BCHP_MEMC_2_ARC_0_READ_RIGHTS_HIGH))
	{
		ulReg = 0;
		ulReg |= hChecker->aulReadClients[1];
		BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_HIGH, ulReg);

		ulReg = 0;
		ulReg |= hChecker->aulWriteClients[1];
		BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_HIGH, ulReg);
	}

#elif BMRC_P_CLIENTS_MAX >= 128
    ulReg = 0;
    ulReg |= hChecker->aulReadClients[0];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_0, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[1];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_1, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[2];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_2, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[3];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_3, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[0];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_0, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[1];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_1, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[2];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_2, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[3];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_3, ulReg);
#if BMRC_P_CLIENTS_MAX >= 256
    ulReg = 0;
    ulReg |= hChecker->aulReadClients[4];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_4, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[5];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_5, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[6];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_6, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulReadClients[7];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_READ_RIGHTS_7, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[4];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_4, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[5];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_5, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[6];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_6, ulReg);

    ulReg = 0;
    ulReg |= hChecker->aulWriteClients[7];
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_WRITE_RIGHTS_7, ulReg);
#endif
#else
#error not supported
#endif

    /* write range */
    ulReg = 0;
    ulReg |= BMRC_P_FIELD_DATA(ARC_0_ADRS_RANGE_LOW, ADDRESS,
                             hChecker->ulStart >> BMRC_P_MEMC_ADRS_SHIFT);
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_ADRS_RANGE_LOW, ulReg);

    /* subtract 1 from size to get correct end address offset */
    ulReg = 0;
    ulReg |= BMRC_P_FIELD_DATA(ARC_0_ADRS_RANGE_HIGH, ADDRESS,
                             (hChecker->ulStart + (hChecker->ulSize - 1)) >> BMRC_P_MEMC_ADRS_SHIFT);
    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_ADRS_RANGE_HIGH, ulReg);

    /* write exclusive mode */
    ulReg = 0;
    ulReg |= BMRC_P_FIELD_DATA(ARC_0_CNTRL, MODE, hChecker->bExclusive);

    /* write access check settings */
    if (hChecker->eCheckType == BMRC_AccessType_eWrite)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CHECK, ENABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CHECK, DISABLED);
    }
    else if (hChecker->eCheckType == BMRC_AccessType_eRead)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CHECK, DISABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CHECK, ENABLED);
    }
    else if (hChecker->eCheckType == BMRC_AccessType_eBoth)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CHECK, ENABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CHECK, ENABLED);
    }

    /* write block settings */
    if (hChecker->eBlockType == BMRC_AccessType_eWrite)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CMD_ABORT, ENABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CMD_ABORT, DISABLED);
    }
    else if (hChecker->eBlockType == BMRC_AccessType_eRead)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CMD_ABORT, DISABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CMD_ABORT, ENABLED);
    }
    else if (hChecker->eBlockType == BMRC_AccessType_eBoth)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CMD_ABORT, ENABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CMD_ABORT, ENABLED);
    }
    else if (hChecker->eBlockType == BMRC_AccessType_eNone)
    {
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, WRITE_CMD_ABORT, DISABLED);
        ulReg |= BMRC_P_FIELD_ENUM(ARC_0_CNTRL, READ_CMD_ABORT, DISABLED);
    }

    BMRC_P_Checker_Write32(hMrc, hChecker, ARC_0_CNTRL, ulReg);

    return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BMRC_Checker_GetClientInfo(BMRC_Handle hMrc, BMRC_Client eClient, BMRC_ClientInfo *pClientInfo)
{
    BDBG_OBJECT_ASSERT(hMrc, BMRC);

    if (eClient >= BMRC_Client_eInvalid)
    {
        pClientInfo->eClient = BMRC_Client_eInvalid;
        pClientInfo->pchClientName = "UNKNOWN";
        pClientInfo->usClientId = BMRC_Client_eInvalid; /* NOTE: this assumes that the max SW enum is > the max HW client id */
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
        pClientInfo->eClient = eClient;
        pClientInfo->pchClientName = BMRC_P_astClientTbl[eClient].pchClientName;
        pClientInfo->usClientId = BMRC_P_GET_CLIENT_ID(hMrc->stSettings.usMemcId, eClient);
        return BERR_SUCCESS;
    }
}

void BMRC_Standby( BMRC_Handle hMrc )
{
    unsigned i;
    BDBG_OBJECT_ASSERT(hMrc, BMRC);
    if(hMrc->suspended) {(void)BERR_TRACE(BERR_NOT_SUPPORTED);return;}
    BKNI_EnterCriticalSection(); /* set a barrier */
    hMrc->suspended = true; 
    BKNI_LeaveCriticalSection();
    for (i = 0; i < hMrc->usMaxCheckers; i++) {
        BMRC_P_CheckerContext *checker = &hMrc->aCheckers[i];
        if(checker->bActive && checker->interruptEnabled) {
            BINT_DisableCallback(checker->hCallback);
        }
    }
    return;
}

void BMRC_Resume( BMRC_Handle hMrc )
{
    unsigned i;
    BDBG_OBJECT_ASSERT(hMrc, BMRC);

    if(!hMrc->suspended) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); return;}

    BKNI_EnterCriticalSection(); /* set a barrier */
    hMrc->suspended = false;
    BKNI_LeaveCriticalSection();

    for (i = 0; i < hMrc->usMaxCheckers; i++)
    {
        BMRC_P_CheckerContext *checker = &hMrc->aCheckers[i];
        if(checker->bActive) {
            BKNI_Memset(checker->aulPrevRegTbl, 0, sizeof(checker->aulPrevRegTbl));
            BMRC_P_Checker_WriteRegs ( checker);
            if(checker->interruptEnabled) {
                BINT_EnableCallback(checker->hCallback);
            }
        }
    }
    return;
}



#else /* BMRC_STUB */
BERR_Code BMRC_Open
    ( BMRC_Handle                     *phMrc,        /* [out] MRC handle to be returned */
      BREG_Handle                      hRegister,    /* [in] Register access handle */
      BINT_Handle                      hInterrupt,   /* [in] Interrupt handle */
      const BMRC_Settings             *pDefSettings  /* [in] Default settings */
    )
{
    BSTD_UNUSED(phMrc);
    BSTD_UNUSED(hRegister);
    BSTD_UNUSED(hInterrupt);
    BSTD_UNUSED(pDefSettings);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Close
    ( BMRC_Handle hMrc )
{
    BSTD_UNUSED(hMrc);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_Create
    ( BMRC_Handle hMrc,               /* [in] MRC Module Handle */
      BMRC_Checker_Handle *phChecker  /* [out] Checker handle to be returned */
    )
{
    BSTD_UNUSED(hMrc);
    BSTD_UNUSED(phChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_Destroy
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle to be destroyed */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetRange
    ( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
      uint32_t ulStart,              /* [in] Memory range start address */
      uint32_t ulSize                /* [in] Memory range size */
    )
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(ulStart);
    BSTD_UNUSED(ulSize);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetAccessCheck
    ( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
      BMRC_AccessType eAccessType    /* [in] Access type to check */
    )
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(eAccessType);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetBlock
    ( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
      BMRC_AccessType eBlockType     /* [in] Access type to block on violations*/
    )
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(eBlockType);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetExclusive
    ( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
      bool bExclusive                /* [in] Enable/disable exclusive mode */
    )
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(bExclusive);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetClient
    ( BMRC_Checker_Handle hChecker,  /* [in] Checker handle */
      BMRC_Client eClient,           /* [in] The client to configure */
      BMRC_AccessType eAccessType    /* [in] The client's access rights */
    )
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(eClient);
    BSTD_UNUSED(eAccessType);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_Enable
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}


BERR_Code BMRC_Checker_Disable
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_EnableCallback
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_DisableCallback
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_EnableCallback_isr
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_DisableCallback_isr
    ( BMRC_Checker_Handle hChecker  /* [in] Checker handle */
    )
{
    BSTD_UNUSED(hChecker);
    return BERR_NOT_SUPPORTED;
}

BERR_Code BMRC_Checker_SetCallback
    ( BMRC_Checker_Handle hChecker,          /* [in] Checker handle */
      const BMRC_CallbackFunc_isr pfCbFunc,  /* [in] Pointer to the callback function */
      void *pvCbData1,                       /* [in] User defined callback data structure. */
      int iCbData2)                          /* [in] User defined callback value */
{
    BSTD_UNUSED(hChecker);
    BSTD_UNUSED(pfCbFunc);
    BSTD_UNUSED(pvCbData1);
    BSTD_UNUSED(iCbData2);
    return BERR_NOT_SUPPORTED;
}

void BMRC_Standby( BMRC_Handle hMrc )
{
    BSTD_UNUSED(hMrc);
    return;
}

void BMRC_Resume( BMRC_Handle hMrc )
{
    BSTD_UNUSED(hMrc);
    return;
}

#endif

/* End of File */
