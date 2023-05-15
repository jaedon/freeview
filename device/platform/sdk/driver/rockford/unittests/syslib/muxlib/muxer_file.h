/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_file.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/30/11 4:14p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_file.h $
 * 
 * Hydra_Software_Devel/4   11/30/11 4:14p nilesh
 * SW7425-324: Added scatter/gather file I/O support
 * 
 * Hydra_Software_Devel/3   9/28/11 5:13p delkert
 * SWCHECKFRAME-110: Increase size of storage for filename, since check
 * tests generate filenames based on test name
 *
 * Hydra_Software_Devel/2   6/28/11 10:04a delkert
 * SW7425-341: Minor cleanup of debug and APIs for use by mux_static_test
 *
 * Hydra_Software_Devel/1   5/11/11 12:01p nilesh
 * SW7425-447: Moved storage implementation to a separate file
 *
 ***************************************************************************/

#ifndef MUXER_FILE_H_
#define MUXER_FILE_H_

#include "bstd.h"
#include "bmuxlib_file.h"

#define MUXER_FILE_MAX_FNAME  256

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

typedef struct
{
   uint32_t uiFileNumber;
} App_StorageContext;

typedef struct
{
   char fname[MUXER_FILE_MAX_FNAME];
   FILE *fp;
   uint32_t uiDescCount;
   uint64_t uiExpectedOffset;
#ifdef BDBG_DEBUG_BUILD
   uint32_t uiIOVMax;
#endif
} App_StorageInterfaceContext;

BERR_Code App_StorageCreate(BMUXlib_StorageSystemInterface *pStorage);
void App_StorageDestroy(BMUXlib_StorageSystemInterface *pStorage);
BERR_Code App_StorageInterfaceAddDescriptors(App_StorageInterfaceContext *pContext, const BMUXlib_StorageDescriptor *pDescriptors, size_t uiNumDescriptors, size_t *puiQueuedCount);
BERR_Code App_StorageInterfaceGetCompletedDescriptors(App_StorageInterfaceContext *pContext, size_t *puiCompletedCount);
void App_StorageInterfaceGetDefaultSettings(BMUXlib_StorageSettings *pSettings);
BERR_Code App_StorageInterfaceCreate(App_StorageContext *pContext, BMUXlib_StorageObjectInterface *pStorageObject, const BMUXlib_StorageSettings *pStorageSettings);
void App_StorageInterfaceDestroy(App_StorageContext *pContext, App_StorageInterfaceContext *pInterfaceContext);

#ifdef __cplusplus
}
#endif

#endif /* MUXER_FILE_H_ */
