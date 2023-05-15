/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_list.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/21/12 9:55p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_list.c $
 * 
 * Hydra_Software_Devel/9   6/21/12 9:55p nilesh
 * SW7425-3076: Revert to previous version to prevent nexus kernel/proxy
 * build failure
 * 
 * Hydra_Software_Devel/8   6/20/12 12:50p nilesh
 * SW7425-3076: Added hMem support for list creation
 *
 * Hydra_Software_Devel/7   6/7/12 4:55p nilesh
 * SW7425-3076: Check for null in Reset()
 * 
 * Hydra_Software_Devel/6   12/15/11 8:28p delkert
 * SW7425-1494: Add error checking
 *
 * Hydra_Software_Devel/5   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/9/11 2:07p nilesh
 * SW7425-1691: Added BMUXlib_List_Reset()
 *
 * Hydra_Software_Devel/4   10/24/11 5:37p delkert
 * SW7425-1494: Fix bug in GetNumFree() where returned count was one
 * greater than it should be, for a FIFO
 *
 * Hydra_Software_Devel/3   6/14/11 5:29p nilesh
 * SW7425-699: Added BMUXlib_List_GetNumFree()
 *
 * Hydra_Software_Devel/2   6/14/11 1:55p nilesh
 * SW7425-699: Remove error messages
 *
 * Hydra_Software_Devel/1   5/11/11 11:55a nilesh
 * SW7425-447: Moved list management functions to a separare shared
 * utility
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_list.h"

BDBG_MODULE(BMUXLIB_LIST);

typedef struct BMUXlib_List_Context
{
   BMUXlib_List_CreateSettings stCreateSettings;
   void **astEntries;
   uint32_t uiReadOffset;
   uint32_t uiWriteOffset;
} BMUXlib_List_Context;

BERR_Code
BMUXlib_List_GetDefaultCreateSettings(
         BMUXlib_List_CreateSettings *pstSettings
         )
{
   BDBG_ENTER( BMUXlib_List_GetDefaultCreateSettings );

   BDBG_ASSERT( pstSettings );

   BKNI_Memset(
            pstSettings,
            0,
            sizeof( BMUXlib_List_CreateSettings )
            );

   BDBG_LEAVE( BMUXlib_List_GetDefaultCreateSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Create(
         BMUXlib_List_Handle *phList,
         const BMUXlib_List_CreateSettings *pstSettings
         )
{
   BMUXlib_List_Handle hList = NULL;

   BDBG_ENTER( BMUXlib_List_Create );

   BDBG_ASSERT( phList );
   BDBG_ASSERT( pstSettings );

   *phList = NULL;

   if ((0 == pstSettings->uiCount)
      || ((pstSettings->eType != BMUXlib_List_Type_eFIFO) && (pstSettings->eType != BMUXlib_List_Type_eStack)))
   {
      BDBG_LEAVE( BMUXlib_List_Create );
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   hList = ( BMUXlib_List_Handle ) BKNI_Malloc( sizeof( BMUXlib_List_Context ) );

   if ( NULL == hList )
   {
      BDBG_LEAVE( BMUXlib_List_Create );
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   BKNI_Memset(
            hList,
            0,
            sizeof( BMUXlib_List_Context )
            );

   hList->stCreateSettings = *pstSettings;

   /* for FIFO, one entry is unusable, so increase count to adjust for this
      (such that usable space is equal to amount requested by user) */
   if ( BMUXlib_List_Type_eFIFO == hList->stCreateSettings.eType )
   {
      hList->stCreateSettings.uiCount++;
   }

   hList->astEntries = ( void* ) BKNI_Malloc( sizeof( void* ) * hList->stCreateSettings.uiCount );
   if ( NULL == hList->astEntries )
   {
      BDBG_LEAVE( BMUXlib_List_Create );
      BMUXlib_List_Destroy( hList );
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   BKNI_Memset(
            hList->astEntries,
            0,
            sizeof( void* ) * hList->stCreateSettings.uiCount
            );

   *phList = hList;

   BDBG_LEAVE( BMUXlib_List_Create );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Destroy(
         BMUXlib_List_Handle hList
         )
{
   BDBG_ENTER( BMUXlib_List_Destroy );

   BDBG_ASSERT( hList );

   if ( NULL != hList->astEntries )
   {
      BKNI_Free( hList->astEntries );
   }

   BKNI_Free( hList );

   BDBG_LEAVE( BMUXlib_List_Destroy );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Reset(
         BMUXlib_List_Handle hList
         )
{
   BDBG_ENTER( BMUXlib_List_Reset );

   if ( NULL != hList )
   {
      hList->uiReadOffset = 0;
      hList->uiWriteOffset = 0;
   }

   BDBG_LEAVE( BMUXlib_List_Reset );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Push(
         BMUXlib_List_Handle hList,
         void *pEntry
         )
{
   BDBG_ENTER( BMUXlib_List_Push );

   BDBG_ASSERT( hList );

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eStack)
   {
      BDBG_LEAVE( BMUXlib_List_Push );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   if ( hList->uiWriteOffset == hList->stCreateSettings.uiCount )
   {
      BDBG_LEAVE( BMUXlib_List_Push );
      return BERR_TRACE( BERR_UNKNOWN );
   }
   else
   {
      hList->astEntries[hList->uiWriteOffset] = pEntry;
      hList->uiWriteOffset++;
   }

   BDBG_LEAVE( BMUXlib_List_Push );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Pop(
         BMUXlib_List_Handle hList,
         void **pEntry
         )
{
   BDBG_ENTER( BMUXlib_List_Pop );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( pEntry );

   *pEntry = NULL;

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eStack)
   {
      BDBG_LEAVE( BMUXlib_List_Pop );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   if ( 0 == hList->uiWriteOffset )
   {
      BDBG_LEAVE( BMUXlib_List_Pop );
      return BERR_TRACE( BERR_UNKNOWN );
   }
   else
   {
      hList->uiWriteOffset--;
      *pEntry = hList->astEntries[hList->uiWriteOffset];
   }

   BDBG_LEAVE( BMUXlib_List_Pop );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Add(
         BMUXlib_List_Handle hList,
         void *pEntry
         )
{
   uint32_t uiTempOffset;

   BDBG_ENTER( BMUXlib_List_Add );

   BDBG_ASSERT( hList );

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eFIFO)
   {
      BDBG_LEAVE( BMUXlib_List_Add );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   uiTempOffset = hList->uiWriteOffset + 1;
   uiTempOffset %= hList->stCreateSettings.uiCount;

   if ( uiTempOffset == hList->uiReadOffset )
   {
      BDBG_LEAVE( BMUXlib_List_Add );
      return BERR_TRACE( BERR_UNKNOWN );
   }
   else
   {
      hList->astEntries[hList->uiWriteOffset] = pEntry;
      hList->uiWriteOffset = uiTempOffset;
   }

   BDBG_LEAVE( BMUXlib_List_Add );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_Remove(
         BMUXlib_List_Handle hList,
         void **pEntry
         )
{
   BDBG_ENTER( BMUXlib_List_Remove );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( pEntry );

   *pEntry = NULL;

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eFIFO)
   {
      BDBG_LEAVE( BMUXlib_List_Remove );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   if ( hList->uiWriteOffset == hList->uiReadOffset )
   {
      BDBG_LEAVE( BMUXlib_List_Remove );
      return BERR_TRACE( BERR_UNKNOWN );
   }
   else
   {
      *pEntry = hList->astEntries[hList->uiReadOffset];
      hList->uiReadOffset++;
      hList->uiReadOffset %= hList->stCreateSettings.uiCount;
   }

   BDBG_LEAVE( BMUXlib_List_Remove );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_GetHead(
         BMUXlib_List_Handle hList,
         void **pEntry
         )
{
   BDBG_ENTER( BMUXlib_List_GetHead );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( pEntry );

   *pEntry = NULL;

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eFIFO)
   {
      BDBG_LEAVE( BMUXlib_List_GetHead );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   if ( hList->uiWriteOffset == hList->uiReadOffset )
   {
      BDBG_LEAVE( BMUXlib_List_GetHead );
      return BERR_TRACE( BERR_UNKNOWN );
   }
   else
   {
      *pEntry = hList->astEntries[hList->uiReadOffset];
   }

   BDBG_LEAVE( BMUXlib_List_GetHead );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_GetNumEntries(
         BMUXlib_List_Handle hList,
         size_t *puiCount
         )
{
   BDBG_ENTER( BMUXlib_List_GetNumEntries );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( puiCount );

   switch ( hList->stCreateSettings.eType )
   {
      case BMUXlib_List_Type_eStack:
         *puiCount = hList->uiWriteOffset;
         break;

      case BMUXlib_List_Type_eFIFO:
         if ( hList->uiReadOffset <= hList->uiWriteOffset )
         {
            *puiCount = hList->uiWriteOffset - hList->uiReadOffset;
         }
         else
         {
            *puiCount = hList->stCreateSettings.uiCount - hList->uiReadOffset;
            *puiCount += hList->uiWriteOffset;
         }
         break;

      default:
         *puiCount = 0;
         return BERR_TRACE( BERR_UNKNOWN );
   }

   BDBG_LEAVE( BMUXlib_List_GetNumEntries );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_List_GetNumFree(
         BMUXlib_List_Handle hList,
         size_t *puiCount
         )
{
   BDBG_ENTER( BMUXlib_List_GetNumFree );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( puiCount );

   switch ( hList->stCreateSettings.eType )
   {
      case BMUXlib_List_Type_eStack:
         *puiCount = hList->stCreateSettings.uiCount - hList->uiWriteOffset;
         break;

      case BMUXlib_List_Type_eFIFO:
      {
         int iAvail = hList->uiReadOffset - hList->uiWriteOffset - 1;
         if (iAvail < 0)
            iAvail += hList->stCreateSettings.uiCount;
         *puiCount = iAvail;
         break;
      }
      default:
         *puiCount = 0;
         return BERR_TRACE( BERR_UNKNOWN );
   }

   BDBG_LEAVE( BMUXlib_List_GetNumFree );

   return BERR_TRACE( BERR_SUCCESS );
}

bool BMUXlib_List_IsEmpty(
         BMUXlib_List_Handle hList
         )
{
   BDBG_ENTER( BMUXlib_List_IsEmpty );

   BDBG_ASSERT( hList );

   switch ( hList->stCreateSettings.eType )
   {
         case BMUXlib_List_Type_eStack:
            return ( 0 == hList->uiWriteOffset );

         case BMUXlib_List_Type_eFIFO:
            return ( hList->uiReadOffset == hList->uiWriteOffset );

         default:
            return true;
   }

   BDBG_LEAVE( BMUXlib_List_IsEmpty );
}

bool BMUXlib_List_IsFull(
         BMUXlib_List_Handle hList
         )
{
   BDBG_ENTER( BMUXlib_List_IsFull );

   BDBG_ASSERT( hList );

   switch ( hList->stCreateSettings.eType )
   {
         case BMUXlib_List_Type_eStack:
            return ( hList->uiWriteOffset == hList->stCreateSettings.uiCount );

         case BMUXlib_List_Type_eFIFO:
         {
            unsigned uiTempOffset = hList->uiWriteOffset + 1;
            uiTempOffset %= hList->stCreateSettings.uiCount;

            return ( uiTempOffset == hList->uiReadOffset );
         }
         default:
            return false;
   }

   BDBG_LEAVE( BMUXlib_List_IsFull );
}

BERR_Code
BMUXlib_List_GetEntries(
         BMUXlib_List_Handle hList,
         const void *astEntries0[], /* Can be NULL if uiNumEntries0=0. */
         size_t *puiNumEntries0,
         const void *astEntries1[], /* Needed to handle FIFO wrap. Can be NULL if uiNumEntries1=0. */
         size_t *puiNumEntries1
         )
{
   BDBG_ENTER( BMUXlib_List_GetEntries );

   BDBG_ASSERT( hList );
   BDBG_ASSERT( astEntries0 );
   BDBG_ASSERT( puiNumEntries0 );
   BDBG_ASSERT( astEntries1 );
   BDBG_ASSERT( puiNumEntries1 );

   if (hList->stCreateSettings.eType != BMUXlib_List_Type_eFIFO)
   {
      *astEntries0 = *astEntries1 = NULL;
      *puiNumEntries0 = *puiNumEntries1 = 0;

      BDBG_LEAVE( BMUXlib_List_GetEntries );
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   if ( hList->uiReadOffset <= hList->uiWriteOffset )
   {
      *puiNumEntries0 = hList->uiWriteOffset - hList->uiReadOffset;
      *astEntries0 = &hList->astEntries[hList->uiReadOffset];

      *puiNumEntries1 = 0;
      *astEntries1 = NULL;
   }
   else
   {
      *puiNumEntries0 = hList->stCreateSettings.uiCount - hList->uiReadOffset;
      *astEntries0 = &hList->astEntries[hList->uiReadOffset];

      *puiNumEntries1 = hList->uiWriteOffset;
      *astEntries1 = &hList->astEntries[0];
   }

   BDBG_LEAVE( BMUXlib_List_GetEntries );

   return BERR_TRACE( BERR_SUCCESS );
}

