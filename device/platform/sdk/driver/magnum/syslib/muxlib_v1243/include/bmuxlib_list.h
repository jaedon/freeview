/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_list.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/21/12 9:55p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_list.h $
 * 
 * Hydra_Software_Devel/5   6/21/12 9:55p nilesh
 * SW7425-3076: Revert to previous version to prevent nexus kernel/proxy
 * build failure
 *
 * Hydra_Software_Devel/4   6/20/12 12:50p nilesh
 * SW7425-3076: Added hMem support for list creation
 *
 * Hydra_Software_Devel/3   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1691/1   11/9/11 2:07p nilesh
 * SW7425-1691: Added BMUXlib_List_Reset()
 * 
 * Hydra_Software_Devel/2   6/14/11 5:29p nilesh
 * SW7425-699: Added BMUXlib_List_GetNumFree()
 * 
 * Hydra_Software_Devel/1   5/11/11 11:55a nilesh
 * SW7425-447: Moved list management functions to a separare shared
 * utility
 *
 ***************************************************************************/

#ifndef BMUXLIB_LIST_H_
#define BMUXLIB_LIST_H_

#include "bstd.h" /* also includes berr, bdbg, etc */

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

typedef struct BMUXlib_List_Context *BMUXlib_List_Handle;

typedef enum BMUXlib_List_Type
{
      BMUXlib_List_Type_eFIFO,
      BMUXlib_List_Type_eStack,

      BMUXlib_List_Type_eMax
} BMUXlib_List_Type;

typedef struct BMUXlib_List_CreateSettings
{
      size_t uiCount;
      BMUXlib_List_Type eType;
} BMUXlib_List_CreateSettings;

BERR_Code
BMUXlib_List_GetDefaultCreateSettings(
         BMUXlib_List_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_List_Create(
         BMUXlib_List_Handle *phList,
         const BMUXlib_List_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_List_Destroy(
         BMUXlib_List_Handle hList
         );

BERR_Code
BMUXlib_List_Reset(
         BMUXlib_List_Handle hList
         );

BERR_Code
BMUXlib_List_Push(
         BMUXlib_List_Handle hList,
         void *pEntry
         );

BERR_Code
BMUXlib_List_Pop(
         BMUXlib_List_Handle hList,
         void **pEntry
         );

BERR_Code
BMUXlib_List_Add(
         BMUXlib_List_Handle hList,
         void *pEntry
         );

BERR_Code
BMUXlib_List_Remove(
         BMUXlib_List_Handle hList,
         void **pEntry
         );

BERR_Code
BMUXlib_List_GetHead(
         BMUXlib_List_Handle hList,
         void **pEntry
         );

BERR_Code
BMUXlib_List_GetNumEntries(
         BMUXlib_List_Handle hList,
         size_t *puiCount
         );

BERR_Code
BMUXlib_List_GetNumFree(
         BMUXlib_List_Handle hList,
         size_t *puiCount
         );

bool BMUXlib_List_IsEmpty(
         BMUXlib_List_Handle hList
         );

bool BMUXlib_List_IsFull(
         BMUXlib_List_Handle hList
         );

BERR_Code
BMUXlib_List_GetEntries(
         BMUXlib_List_Handle hList,
         const void *astEntries0[], /* Can be NULL if uiNumEntries0=0. */
         size_t *puiNumEntries0,
         const void *astEntries1[], /* Needed to handle FIFO wrap. Can be NULL if uiNumEntries1=0. */
         size_t *puiNumEntries1
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_LIST_H_ */
