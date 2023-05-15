/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_file.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 5/22/12 10:39a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_file.c $
 * 
 * Hydra_Software_Devel/4   5/22/12 10:39a delkert
 * SW7425-2567: Add asserts to fix coverity warnings
 *
 * Hydra_Software_Devel/3   11/30/11 4:14p nilesh
 * SW7425-324: Added scatter/gather file I/O support
 *
 * Hydra_Software_Devel/2   6/28/11 10:04a delkert
 * SW7425-341: Minor cleanup of debug and APIs for use by mux_static_test
 *
 * Hydra_Software_Devel/1   5/11/11 12:01p nilesh
 * SW7425-447: Moved storage implementation to a separate file
 *
 ***************************************************************************/
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/stat.h>      /* for stat() */
#include <sys/types.h>
#include <fcntl.h>
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "muxer_file.h"

BDBG_MODULE(MUXER_FILE);

/* NOTE: pDescriptors is NOT an array of BMUXlib_StorageDescriptors
         - it cannot be addressed with an index!! */
BERR_Code App_StorageInterfaceAddDescriptors(App_StorageInterfaceContext *pContext,
                                             const BMUXlib_StorageDescriptor *pDescriptors,
                                             size_t uiNumDescriptors,
                                             size_t *puiQueuedCount)
{
   int ofd;
   uint32_t i,j;
   ssize_t uiByteCount, uiAggregateSize;
   BMUXlib_StorageDescriptor *pDesc = (BMUXlib_StorageDescriptor *)pDescriptors;

   BDBG_ENTER(App_StorageInterfaceAddDescriptors);

   BDBG_MSG(("Adding %d Descriptors to %s", uiNumDescriptors, pContext->fname));

#ifdef BDBG_DEBUG_BUILD
   if (0 == pContext->uiIOVMax)
      pContext->uiIOVMax = sysconf(_SC_IOV_MAX);
#endif

   /*
    * Extract the file descriptor from the FILE stream
    */
   ofd = fileno(pContext->fp);

   for (i = 0; i < uiNumDescriptors; i++)
   {
      /* NOTE: ideally, this implementation should split an incoming vector into multiple
         writev or readv calls if the vector length exceeds the max supported by the FS/OS.
         For now, we assert since vectors this long should theoretically never occur */
      BDBG_ASSERT(pDesc->uiVectorCount < pContext->uiIOVMax);

      /* here, we also assume that the aggregate length does not exceed an ssize_t */
      BDBG_MSG(("[%2.2d]: offset: %lld (exp: %lld), [%c] %d vectors\n",
         i, pDesc->uiOffset, pContext->uiExpectedOffset, (pDesc->bWriteOperation)?'W':'R', pDesc->uiVectorCount));
      for (uiAggregateSize = 0, j = 0; j <pDesc->uiVectorCount; j++)
      {
         BDBG_MSG(("v[%2.2d]: %d bytes\n", j, pDesc->iov[j].uiLength));
         uiAggregateSize += pDesc->iov[j].uiLength;
      }
      BDBG_ASSERT(uiAggregateSize < SSIZE_MAX);

      /* if offsets are discontinuous ... */
      if (pDesc->uiOffset != pContext->uiExpectedOffset)
      {
         BDBG_MSG(("Seeking to %lld\n", pDesc->uiOffset));
         /* Seek to the specified offset in the file ... */
         if (-1 == lseek(ofd, pDesc->uiOffset, SEEK_SET))
         {
            BDBG_ERR(("Unable to seek to %lld", pDesc->uiOffset));
            *puiQueuedCount = 0;
            return BERR_TRACE(BERR_UNKNOWN);    /* seek error ! */
         }
      }

      /*
       * Based on the value of the write operation flag, either write or read
       * the entire supplied vector at once.
       */
      if (true == pDesc->bWriteOperation)
      {
         uiByteCount = writev(ofd, (const struct iovec *)pDesc->iov, pDesc->uiVectorCount);
         if (uiByteCount != uiAggregateSize)
         {
            BDBG_ERR(("0x%6.6lx bytes requested, 0x%6.6lx bytes written", uiAggregateSize, uiByteCount));
            *puiQueuedCount = 0;
            return BERR_TRACE(BERR_UNKNOWN);    /* write error ! */
         }
         BDBG_MSG(("%d bytes written to %s @ offset: %lld", uiByteCount, pContext->fname, pDesc->uiOffset));

         /*
          * Flush any remaining data to storage
          */
         fflush(pContext->fp);
      }
      else
      {
         uiByteCount = readv(ofd, (const struct iovec *)pDesc->iov, pDesc->uiVectorCount);
         if (uiByteCount != uiAggregateSize)
         {
            BDBG_ERR(("0x%6.6lx bytes requested, 0x%6.6lx bytes read", uiAggregateSize, uiByteCount));
            *puiQueuedCount = 0;
            return BERR_TRACE(BERR_UNKNOWN);    /* read error ! */
         }
         BDBG_MSG(("%d bytes read from %s @ offset: %lld", uiByteCount, pContext->fname, pDesc->uiOffset));
      }

      pContext->uiExpectedOffset = pDesc->uiOffset + uiByteCount;

      /* locate the next descriptor (remember, they must be contiguous in memory) ... */
      pDesc = BMUXLIB_STORAGE_GET_NEXT_DESC(pDesc);

   } /* end: for each descriptor */

   *puiQueuedCount = uiNumDescriptors;             /* We process all descriptors supplied to us */
   pContext->uiDescCount += uiNumDescriptors;

   BDBG_LEAVE(App_StorageInterfaceAddDescriptors);

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code App_StorageInterfaceGetCompletedDescriptors(App_StorageInterfaceContext *pContext, size_t *puiCompletedCount)
{
   *puiCompletedCount = pContext->uiDescCount;
   pContext->uiDescCount = 0;
   return BERR_TRACE(BERR_SUCCESS);
}

void App_StorageInterfaceGetDefaultSettings(BMUXlib_StorageSettings *pSettings)
{
   pSettings->uiExpectedSizeBytes = 0;
}

BERR_Code App_StorageInterfaceCreate(App_StorageContext *pContext, BMUXlib_StorageObjectInterface *pStorageInterface, const BMUXlib_StorageSettings *pStorageSettings)
{
   App_StorageInterfaceContext *pInterfaceContext;
   BSTD_UNUSED(pStorageSettings);    /* for now there's nothing in here we can use */

   /* create the storage interface context */
   pInterfaceContext = (App_StorageInterfaceContext *)BKNI_Malloc(sizeof(App_StorageInterfaceContext));
   if (NULL == pInterfaceContext)
      return BERR_OUT_OF_SYSTEM_MEMORY;
   pStorageInterface->pContext = pInterfaceContext;
   BKNI_Memset(pInterfaceContext, 0, sizeof(App_StorageInterfaceContext));
   pStorageInterface->pfAddDescriptors = (BMUXlib_AddStorageDescriptors)App_StorageInterfaceAddDescriptors;
   pStorageInterface->pfGetCompleteDescriptors = (BMUXlib_GetCompletedStorageDescriptors)App_StorageInterfaceGetCompletedDescriptors;
   /* open a temporary file for r/w binary */
   snprintf(pInterfaceContext->fname, MUXER_FILE_MAX_FNAME, "muxtmp%2.2d.dat", pContext->uiFileNumber++);
   BDBG_MSG(("Creating Storage for interface: %p (file: %s)\n", pStorageInterface, pInterfaceContext->fname));
   pInterfaceContext->fp = fopen(pInterfaceContext->fname, "w+b");
   if (NULL == pInterfaceContext->fp)
      return BERR_UNKNOWN;
   return BERR_SUCCESS;
}

void App_StorageInterfaceDestroy(App_StorageContext *pContext, App_StorageInterfaceContext *pInterfaceContext)
{
   BSTD_UNUSED(pContext);

   if (NULL == pInterfaceContext)
      return;

   BDBG_MSG(("Destroying Storage Object: %p (file: %s)", pInterfaceContext, pInterfaceContext->fname));
   /* if file open, close it, then destroy it */
   if (NULL != pInterfaceContext->fp)
   {
      fclose(pInterfaceContext->fp);
      /* remove the temporary file */
#ifndef KEEP_TEMP_FILES
      BDBG_MSG(("Removing File: %s\n", pInterfaceContext->fname));
      remove(pInterfaceContext->fname);
#endif
   }
   BKNI_Free(pInterfaceContext);
}

BERR_Code App_StorageCreate(BMUXlib_StorageSystemInterface *pStorage)
{
   App_StorageContext *pContext;
   /* create the storage context */
   pContext = (App_StorageContext *)BKNI_Malloc(sizeof(App_StorageContext));
   if (NULL == pContext)
      return BERR_OUT_OF_SYSTEM_MEMORY;
   pContext->uiFileNumber = 0;
   /* initialise the storage "object" */
   pStorage->pContext = pContext;
   pStorage->pfGetDefaultStorageSettings = (BMUXlib_GetDefaultStorageSettings)App_StorageInterfaceGetDefaultSettings;
   pStorage->pfCreateStorageObject = (BMUXlib_CreateStorageObject)App_StorageInterfaceCreate;
   pStorage->pfDestroyStorageObject = (BMUXlib_DestroyStorageObject)App_StorageInterfaceDestroy;
   return BERR_SUCCESS;
}

void App_StorageDestroy(BMUXlib_StorageSystemInterface *pStorage)
{
   if (NULL != pStorage->pContext)
      BKNI_Free(pStorage->pContext);
   BKNI_Memset(pStorage, 0, sizeof(BMUXlib_StorageSystemInterface));
}
