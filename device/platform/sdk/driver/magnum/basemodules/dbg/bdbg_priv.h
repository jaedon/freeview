/***************************************************************************
 *     Copyright (c) 2010-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/6/11 3:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg_priv.h $
 * 
 * Hydra_Software_Devel/2   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/1   4/13/11 6:57p vsilyaev
 * SW7405-5221: Private API for the debug modules
 * 
 *
 ***************************************************************************/
#ifndef BDBG_PRIV_H
#define BDBG_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif
/* Private declarations starts here */
#define BDBG_NOP() (void)0
int BDBG_P_Vprintf(const char *fmt, va_list ap);
void BDBG_P_PrintString(const char *fmt, ...);
int BDBG_P_Vprintf_Log(BDBG_ModulePrintKind kind, const char *fmt, va_list ap);
BERR_Code BDBG_P_PrintError(const char *file, unsigned lineno, const char *error, BERR_Code error_no);
void BDBG_P_AssertFailed(const char *expr, const char *file, unsigned line);

#ifdef __cplusplus
}
#endif

#endif /* BDBG_PRIV_H */

