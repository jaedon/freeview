/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_output.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 2/21/12 1:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg_output.c $
 * 
 * Hydra_Software_Devel/12   2/21/12 1:59p erickson
 * SW7425-2130: add new DBG options for compact debug mode
 * 
 * Hydra_Software_Devel/11   8/4/11 8:03p vsilyaev
 * SW7420-1465: Removed dead code
 * 
 * Hydra_Software_Devel/10   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 * instead of syslog buffer
 * 
 * Hydra_Software_Devel/9   6/8/11 2:14p vsilyaev
 * SW7405-4477: Added missing new line
 * 
 * Hydra_Software_Devel/8   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/7   4/13/11 7:00p vsilyaev
 * SW7405-5221: Route BERR_TRACE through the debug log
 * 
 * Hydra_Software_Devel/6   8/15/07 2:25p vsilyaev
 * PR 34089: Optimized executuion speed and code size
 * 
 * Hydra_Software_Devel/5   1/16/07 3:15p erickson
 * PR27092: prepend <0> to get message out of linux kernel before dying
 * 
 * Hydra_Software_Devel/4   7/25/06 10:42a vsilyaev
 * PR 22695: Pulled the debug output routines into the release build
 * 
 * Hydra_Software_Devel/3   10/8/04 10:28a pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/2   3/28/03 5:58p vsilyaev
 * Added BDBG_P_PrintError and BDGB_P_AssertFailed.
 *
 * Hydra_Software_Devel/1   3/12/03 2:38p vsilyaev
 * Default debug output file.
 *
 * Hydra_Software_Devel/1   3/10/03 9:48a vsilyaev
 * Win32 implementation of the O/S specific debug interface functions.
 *
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "bdbg_priv.h"

void
BDBG_P_AssertFailed(const char *expr, const char *file, unsigned line)
{
#if B_REFSW_DEBUG_COMPACT_ERR
    BSTD_UNUSED(expr);
    BDBG_P_PrintString("!!! Assert at %s:%d\n", file, line);
#else
    BDBG_P_PrintString("!!! Assert '%s' Failed at %s:%d\n", expr, file, line);
#endif
#if !B_REFSW_DEBUG_ASSERT_NO_FAIL    
    BKNI_Fail();
#endif
}

BERR_Code
BDBG_P_PrintError(const char *file, unsigned lineno, const char *error, BERR_Code error_no)
{
    if (error_no != BERR_SUCCESS) {
#if B_REFSW_DEBUG_COMPACT_ERR
        BSTD_UNUSED(error);
        BDBG_P_PrintString("!!!Error at %s:%d\n", file, lineno);
#else
        BDBG_P_PrintString("!!!Error %s(%#x) at %s:%d\n", error, error_no, file, lineno);
#endif
    }
    return error_no;
}

int
BDBG_P_Vprintf(const char *fmt, va_list ap)
{
    return BDBG_P_Vprintf_Log(BDBG_ModulePrintKind_eString, fmt, ap);
}

void
BDBG_P_PrintString(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    BDBG_P_Vprintf(fmt, ap);

    va_end( ap );

    return;
}

/* End of file */
