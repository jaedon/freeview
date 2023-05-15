/***************************************************************************
 *	   Copyright (c) 2006-2007, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_print.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/20/07 6:17p $
 *
 * Module Description: Non blocking printf support module.
 *
 * Revision History:
 ***************************************************************************/

#if !defined(_BKNI_PRINT_H_)
#define _BKNI_PRINT_H_

/**
   BKNI_Print_Init - background printf initialization. Must be called before 
   first use.
 */
void BKNI_Print_Init(void);
/**
   BKNI_Print_Printf - printf interface to the background printing. Must be
   called from critical section or interrupt context. Designed to be called
   directly from BKNI_Printf.
 */
int BKNI_Print_Printf(const char *fmt, ...);
/**
   BKNI_Print_Vprintf - vprintf interface to the background printing. Must be
   called from critical section or interrupt context. Designed to be called
   directly from BKNI_Printf.
 */
int BKNI_Print_Vprintf(const char *fmt, va_list ap);
/**
   BKNI_Print_Out - string output function pointer.
   This function should take a buffer and a size as a parameter and output
   buffer to desired destination like a serial port or a network socket.
 */
typedef size_t (*BKNI_Print_Out)(const void * buf, size_t count);
/**
   BKNI_Print_Worker - background worker function.
   This function muste be called from a separate thread and it never returns.
   Thread calling this function should be low a priority thread so the printing
   does not interfere with normal operations of the software.
 */
void BKNI_Print_Worker(BKNI_Print_Out Output);

#endif
