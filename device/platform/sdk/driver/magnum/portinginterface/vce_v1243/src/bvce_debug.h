/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_debug.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/11/12 12:46p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_debug.h $
 * 
 * Hydra_Software_Devel/5   10/11/12 12:46p nilesh
 * SW7425-2568: Added BVCE_Debug_P_WriteLogBuffer_isr
 * 
 * Hydra_Software_Devel/4   10/11/12 12:08p nilesh
 * SW7425-2568: Added logging functions
 * 
 * Hydra_Software_Devel/3   2/23/11 5:49p nilesh
 * SW7425-1: Utilize BDBG_FILE_MODULE to aid debug
 * 
 * Hydra_Software_Devel/2   11/18/10 12:10p nilesh
 * SW7425-1: Fixed Watchdog Handling and Mailbox Interrupt on boot.
 * 
 * Hydra_Software_Devel/1   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 *
 ***************************************************************************/

#ifndef BVCE_DEBUG_H_
#define BVCE_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* Enable Watchdog Handler */
#define BVCE_P_ENABLE_WATCHDOG 1

/* If FW Command MBX handshake polling desired, enable polling here */
#define BVCE_P_POLL_FW_MBX 0
#define BVCE_P_POLL_FW_COUNT 10000

#define BVCE_P_POLL_FW_DATAREADY 1

#if ( BVCE_P_DUMP_OUTPUT_CDB || BVCE_P_DUMP_OUTPUT_ITB || BVCE_P_DUMP_OUTPUT_ITB_DESC || BVCE_P_DUMP_ARC_DEBUG_LOG || BVCE_P_DUMP_USERDATA_LOG || BVCE_P_TEST_MODE )
#include <stdio.h>

typedef struct BVCE_Debug_P_Log_Context* BVCE_Debug_Log_Handle;

bool
BVCE_Debug_P_OpenLog(
   char* szFilename,
   BVCE_Debug_Log_Handle *phLog
   );

void
BVCE_Debug_P_CloseLog(
   BVCE_Debug_Log_Handle hLog
   );

void
BVCE_Debug_P_WriteLog_isr(
   BVCE_Debug_Log_Handle hLog,
   const char *fmt,
   ...
   );

void
BVCE_Debug_P_WriteLogBuffer_isr(
   BVCE_Debug_Log_Handle hLog,
   const void * pBuffer,
   size_t uiNumBytes
   );

#endif

#ifdef __cplusplus
}
#endif

#endif /* BVCE_DEBUG_H_ */
