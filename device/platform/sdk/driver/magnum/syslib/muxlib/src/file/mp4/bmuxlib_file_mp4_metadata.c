/***************************************************************************
 *     Copyright (c) 2010-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_mp4_metadata.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 10/17/12 10:23a $
 *
 * [File Description:]
 * Metadata processing functions for File-based MP4 software mux
 *
 * (here, "metadata" refers to the additional per-track information that is
 *  stored in boxes to describe the track's media data)
 *
 * There are 5 types of metadata collected for each input sample:
 *   DTS Delta (used in stts box)
 *   CTS DTS Offset (used in ctts box)
 *   Sample Size (used in stsc box)
 *   Sample Offset (used in stco /co64 box)
 *   RAP Indication (used in stss box)
 *
 ***************************************************************************
 * [Revision History:]
 * $brcm_Log: /magnum/syslib/muxlib/src/file/mp4/bmuxlib_file_mp4_metadata.c $
 * 
 * Hydra_Software_Devel/10   10/17/12 10:23a nilesh
 * SW7445-54: Fixed missing newline compiler warning
 * 
 * Hydra_Software_Devel/9   1/19/12 4:25p delkert
 * SW7425-1378: Refactor to use common muxlib input module. Remove support
 * for stream trucation for offset correction (edit list always used).
 * Remove stall timeout. Remove workarounds for SW7425-357 and SW7425-
 * 624.
 *
 * Hydra_Software_Devel/8   11/14/11 4:08p delkert
 * SW7425-1379: Refactor to use common output module:
 * Add Release Qs & callback data to support out-of-order "completion".
 * Remove uiNumActiveInputs (use interface to ecide if input is acive).
 * Split box processing so that only one descriptor output at-a-time.
 * Add cleanup if any output fails creation.
 * App "blocked" status is now dependent only on storage.
 *
 * Hydra_Software_Devel/7   7/7/11 9:54a delkert
 * SW7425-326: Clean up output descriptor logs (use common macro).
 *
 * Hydra_Software_Devel/6   6/28/11 12:54p delkert
 * SW7425-326: Improve Output (Storage I/O) debug
 *
 * Hydra_Software_Devel/5   6/17/11 9:41p delkert
 * SW7425-738: Fix up 64-bit offset handling for co64 box.
 *
 * Hydra_Software_Devel/4   6/1/11 5:10p delkert
 * SW7425-377: Add mdat relocation and offset adjustments to support
 * progressive download capability
 *
 * Hydra_Software_Devel/3   5/24/11 2:43p delkert
 * SW7425-491: Add missing check for audio max bitrate metadata.  Change
 * default audio decode buffer size to 2 * maximum frame size
 *
 * Hydra_Software_Devel/2   5/18/11 5:27p delkert
 * SW7425-491: Initial changes to audio state machine to support in-band
 * metadata descriptors
 *
 * Hydra_Software_Devel/1   5/13/11 11:52a delkert
 * SW7425-446: Split box and metadata processing into independent files
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_file_mp4_priv.h"

BDBG_MODULE(BMUXLIB_FILE_MP4_METADATA);

/****************************
     Static Prototypes
****************************/

static bool FlushMetadataCacheToStorage(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_MetadataInterface *pInterface);

/**************************
    Static Definitions
**************************/

/*************************
* P R I V A T E   A P I  *
**************************/

/*
   Process the collected metadata to make it suitable for writing to the relevant box
   If this is performed, it is done as an in-place modification of the data within
   the metadata cache

   Typically, this is only required for stco contents, since the offsets are stored as
   64-bits incase a co64 box is required, so these offsets are compacted to 32-bits
   before being written to the stco box. If a co64 box is used, data is written as-is.

   For boxes such as stts, ctts, stsz, and stss the data is already stored in the correct
   format for the relevant box
*/
void BMUXlib_File_MP4_P_ProcessMetadata(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BMUXlib_File_MP4_P_MetadataInterface *pMetadata = hMP4Mux->pCurrentMetadata;
   BMUXlib_File_MP4_P_MetadataCache *pCache = &pMetadata->stCache;

   BDBG_ENTER(BMUXlib_File_MP4_P_ProcessMetadata);
   if ((hMP4Mux->eCurrentBox == BMUXlib_File_MP4_P_BoxType_eStco) & !hMP4Mux->bCo64Required)
   {
      uint32_t *pBuffer = pCache->pBuffer;      /* always process data from start of cache */
      uint32_t uiEntriesToProcess = pCache->uiWriteIndex;
      uint32_t uiData;
      uint32_t uiIndex = 0; /* read index is (write index * 2) + 1 */

      /* compact offsets metadata to fit stco format ... */
      BDBG_MSG(("Compacting offsets for stco... "));
      while (0 != uiEntriesToProcess)
      {
         uiData = pBuffer[(uiIndex * 2) + 1];
         pBuffer[uiIndex++] = uiData;
         uiEntriesToProcess--;
      }
   }
   BDBG_LEAVE(BMUXlib_File_MP4_P_ProcessMetadata);
}

/*
   Adjust the offsets in the stco or co64 to account for relocation of the mdat

   Offsets are adjusted by the size of the moov, which is the new mdat location
   => this assumes that the original offsets were all based on the start of the
   mdat, i.e. are 0 based

   The offsets to adjust are contained in the relocation buffer.
   NOTE: all reads/writes in relocation buffer will be appropriately aligned
         Also, important to note that the offsets in the stco are stored in
         big-endian format

   NOTE: This adjustment will not work if the stco box is used and the last
         offset in the original stco + new offset results in an offset that
         is > 32 bits (this would require reformatting the stco as a co64)
*/
void BMUXlib_File_MP4_P_AdjustOffsets(BMUXlib_File_MP4_Handle hMP4Mux)
{
   BMUXlib_File_MP4_P_RelocationBuffer *pBuffer = &hMP4Mux->stRelocationBuffer;
   /* determine the number of entries contained in the buffer ... */
   uint32_t uiEntriesToProcess = (hMP4Mux->bCo64Required)?(pBuffer->uiBytesUsed >> 3):(pBuffer->uiBytesUsed >> 2);
   uint32_t uiOffset = hMP4Mux->uiNewMdatOffset;
   uint32_t *pData = (uint32_t *)(pBuffer->pBase);
   uint32_t i;

   BDBG_ENTER(BMUXlib_File_MP4_P_AdjustOffsets);

   if (hMP4Mux->bCo64Required)
   {
      uint64_t uiData;
      for (i = 0; i < uiEntriesToProcess; i++)
      {
         /* read 64-bit entry in two halves, add offset, then write back in two halves ... */
         uiData = MP4MUX_SWAP_U32(*pData);                  /* read upper 32 bits */
         uiData <<= 32;                                     /* shift them into place */
         uiData |= MP4MUX_SWAP_U32(*(pData+1));             /* combine with lower 32 bits */
         uiData += uiOffset;                                /* adjust the 64 bit offset */
         *pData++ = MP4MUX_SWAP_U32(uiData >> 32);          /* write back upper 32 bits */
         *pData++ = MP4MUX_SWAP_U32(uiData & 0xFFFFFFFF);   /* write back lower 32 bits */
      }
   }
   else
   {
      uint32_t uiData;
      for (i = 0; i < uiEntriesToProcess; i++)
      {
         uiData = MP4MUX_SWAP_U32(*pData);
         uiData += uiOffset;
         *pData++ = MP4MUX_SWAP_U32(uiData);
      }
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_AdjustOffsets);
}

/*
   Write collected metadata for the current sample to the relevant metadata cache(s)

   To prevent blocking, best to flush cache when half-full, while writing to the other half
   that way we're not waiting for the storage to finish before we can reuse the cache

   First half of cache flushed when num entries = half the buffer, second half flushed when buffer full.
   Then reset.
   Expects that storage operations complete by the time the half fills up again, else we're in trouble!
   If write index >= read index, then cache overflow

   Data is written to the cache in the big-endian format expected in the resulting file

   NOTE: by having one function to handle metadata, this can be easily altered to write
         all metadata to a single file in records if desired, instead of having split outputs
*/
void BMUXlib_File_MP4_P_StoreMetadataToCache(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_Metadata *pMetadata)
{
   uint32_t uiMetadataType;
   BMUXlib_File_MP4_P_TrackInfo *pTrack = hMP4Mux->stCurrentSample.pInput->pTrack;

   BDBG_ENTER(BMUXlib_File_MP4_P_StoreMetadataToCache);

   BDBG_MSG(("Storing Metadata :: deltaDTS = %d, CTSDTSDiff = %d, Sample Size = %d, Sample Offset = %lld %s",
               pMetadata->uiDTSDelta,
               pMetadata->uiCTSDTSDiff,
               pMetadata->uiSampleSize,
               pMetadata->uiOffset,
               (pMetadata->bRandomAccess)?"(RAP)":""));

   /* write each type of metadata to the relevant cache */
   for (uiMetadataType = 0; uiMetadataType < BMUXlib_File_MP4_P_MetadataType_eMax; uiMetadataType++)
   {
      BMUXlib_File_MP4_P_MetadataInterface *pInterface = &pTrack->aMetadata[uiMetadataType];
      BMUXlib_File_MP4_P_MetadataCache *pCache = &pInterface->stCache;
      uint32_t uiWriteIndex = pCache->uiWriteIndex;
      /* if read index is at start of cache, flush when write gets to halfway,
         else flush when write is reset to the start of the cache */
      uint32_t uiFlushIndex = (0 == pCache->uiReadIndex)?(pCache->uiNumEntries / 2):0;
      bool bMetadataWritten = false;

      switch (uiMetadataType)
      {
         case BMUXlib_File_MP4_P_MetadataType_eStts:
            {
               BMUXlib_File_MP4_DTSDelta *pBuffer = (BMUXlib_File_MP4_DTSDelta *)pCache->pBuffer;
               if (0 == pInterface->uiRunCount)
               {
                  pInterface->uiCurrentValue = pMetadata->uiDTSDelta;
                  pInterface->uiRunCount = 1;
               }
               else
               {
                  if (pInterface->uiCurrentValue == pMetadata->uiDTSDelta)
                  {
                     pInterface->uiRunCount++;
                  }
                  else
                  {
                     BDBG_MSG(("Writing stts metadata @ index %d: c:%d v:%d", uiWriteIndex, pInterface->uiRunCount, pInterface->uiCurrentValue));
                     pBuffer[uiWriteIndex].uiDelta = MP4MUX_SWAP_U32(pInterface->uiCurrentValue);
                     pBuffer[uiWriteIndex].uiCount = MP4MUX_SWAP_U32(pInterface->uiRunCount);
                     pInterface->uiCurrentValue = pMetadata->uiDTSDelta;
                     pInterface->uiRunCount = 1;
                     pInterface->uiEntryCount++;
                     bMetadataWritten = true;
                  }
               }
            }
            break;
         case BMUXlib_File_MP4_P_MetadataType_eCtts:
            {
               BMUXlib_File_MP4_CTSDTSOffset *pBuffer = (BMUXlib_File_MP4_CTSDTSOffset *)pCache->pBuffer;
               if (0 == pInterface->uiRunCount)
               {
                  pInterface->uiCurrentValue = pMetadata->uiCTSDTSDiff;
                  pInterface->uiRunCount = 1;
               }
               else
               {
                  if (pInterface->uiCurrentValue == pMetadata->uiCTSDTSDiff)
                  {
                     pInterface->uiRunCount++;
                  }
                  else
                  {
                     BDBG_MSG(("Writing ctts metadata @ index %d: c:%d v:%d", uiWriteIndex, pInterface->uiRunCount, pInterface->uiCurrentValue));
                     pBuffer[uiWriteIndex].uiOffset = MP4MUX_SWAP_U32(pInterface->uiCurrentValue);
                     pBuffer[uiWriteIndex].uiCount = MP4MUX_SWAP_U32(pInterface->uiRunCount);
                     pInterface->uiCurrentValue = pMetadata->uiCTSDTSDiff;
                     pInterface->uiRunCount = 1;
                     pInterface->uiEntryCount++;
                     bMetadataWritten = true;
                  }
               }
            }
            break;
         case BMUXlib_File_MP4_P_MetadataType_eStsz:
            {
               /* there is one sample size value per sample ... */
               BMUXlib_File_MP4_SampleSize *pBuffer = (BMUXlib_File_MP4_SampleSize *)pCache->pBuffer;
               BDBG_MSG(("Writing stsz metadata @ index %d: %d", uiWriteIndex, pMetadata->uiSampleSize));
               pBuffer[uiWriteIndex] = MP4MUX_SWAP_U32(pMetadata->uiSampleSize);
               if (pMetadata->uiSampleSize > pTrack->uiMaxSampleSize)
                  pTrack->uiMaxSampleSize = pMetadata->uiSampleSize;
               pInterface->uiEntryCount++;
               bMetadataWritten = true;
            }
            break;
         case BMUXlib_File_MP4_P_MetadataType_eStco:
            {
               /* for now, there is one sample per chunk, so one offset per sample ...
                  (if chunks are ever created, this needs to only output the offset of the
                   start sample in the chunk) */
               uint32_t *pBuffer = pCache->pBuffer;
               uint32_t uiUpper = (uint32_t)(pMetadata->uiOffset >> 32);
               uint32_t uiLower = (uint32_t)(pMetadata->uiOffset & 0xFFFFFFFF);
               BDBG_MSG(("Writing stco/co64 metadata @ index %d: %lld", uiWriteIndex, pMetadata->uiOffset));
               /* NOTE: Since this is writing the 64-bit value in two halves, the index
                  is actually half what it needs to be (the index is an index of 64-bit
                  values, and so should step 0, 1, 2 ... giving offsets of 0, 8, 16 ...\
               */
               pBuffer[uiWriteIndex * 2] = MP4MUX_SWAP_U32(uiUpper);
               pBuffer[(uiWriteIndex * 2) + 1] = MP4MUX_SWAP_U32(uiLower);
               pInterface->uiEntryCount++;
               bMetadataWritten = true;
            }
            break;
         case BMUXlib_File_MP4_P_MetadataType_eStss:
            {
               /* determine if this sample is a random access sample
                  - if so, store the current sample number ... */
               BMUXlib_File_MP4_RAPSampleNum *pBuffer = pCache->pBuffer;
               if (pMetadata->bRandomAccess)
               {
                  BDBG_MSG(("Writing stss metadata @ index %d: Sample %d is RAP", uiWriteIndex, pTrack->uiSampleCount));
                  pBuffer[uiWriteIndex] = MP4MUX_SWAP_U32(pTrack->uiSampleCount);
                  pInterface->uiEntryCount++;
                  bMetadataWritten = true;
               }
            }
            break;
      } /* end: switch metadata type */

      if ( true == bMetadataWritten )
      {
         uiWriteIndex++;

         if (uiWriteIndex == pCache->uiNumEntries)    /* check for wrap */
            uiWriteIndex = 0;
         BDBG_ASSERT(0 == pInterface->uiEntryCount || uiWriteIndex != pCache->uiReadIndex);     /* cache overflow */
         pCache->uiWriteIndex = uiWriteIndex;

         if (uiWriteIndex == uiFlushIndex)
            pCache->bFlushDone = false;               /* time to flush the cache */
      }

      /* if cache not flushed yet, then attempt to flush to storage ... */
      if (!pCache->bFlushDone)
      {
         BDBG_MSG(("Flushing cache %d (current write index: %d)", uiMetadataType, uiFlushIndex));
         /* this can fail if no storage descriptors available - if so wait until next time */
         pCache->bFlushDone = FlushMetadataCacheToStorage(hMP4Mux, pInterface);
      }
      /* NOTE: we cannot adjust the read pointer until the storage descriptor is returned */
   } /* end: for each metadata type */
   BDBG_LEAVE(BMUXlib_File_MP4_P_StoreMetadataToCache);
}


/*
   Retrieve the specified metadata (for the current box being created) into the relevant metadata cache
   If the data is already in the cache (i.e. the entire metadata fits within the cache)
   then do not read from storage, and simply read directly from cache

   Returns true when successful, else returns false if out of storage descriptors
*/
bool BMUXlib_File_MP4_P_RetrieveMetadataToCache(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_MetadataType eMetadataType, bool *pbInCacheFlag)
{
   BMUXlib_File_MP4_P_MetadataInterface *pMetadata;
   BMUXlib_File_MP4_P_TrackInfo *pTrack = hMP4Mux->pCurrentTrack;
   bool bResult = true;

   BDBG_ENTER(BMUXlib_File_MP4_P_RetrieveMetadataToCache);

   BDBG_ASSERT(BMUXlib_File_MP4_P_MetadataType_eMax != eMetadataType); /* should not happen */

   pMetadata = &pTrack->aMetadata[eMetadataType];
   hMP4Mux->pCurrentMetadata = pMetadata;

   if (!pMetadata->bInitialReadComplete)
   {
      /* first time processing this metadata ... */
      /* determine if the required data is already in the cache */
      *pbInCacheFlag = (pMetadata->uiEntryCount <= pMetadata->stCache.uiNumEntries);
      pMetadata->bInitialReadComplete = true;
      /* reset the metadata storage's byte counter, since we now need to read from the
         file's beginning (this will now be used as read offset) */
      BMUXlib_Output_SetCurrentOffset(pMetadata->pOutput->hOutput, 0, BMUXlib_Output_OffsetReference_eStart);
   }
   else
   {
      /* if this not the first time we come to read metadata, then it *must* be read from storage
         (since the cache is now dirty) */
      *pbInCacheFlag = false;
   }
   if (*pbInCacheFlag)
   {
      /* all required data is available in the cache */
      pMetadata->uiEntryCount = 0;              /* no more metadata to process */
   }
   else
   {
      BMUXlib_File_MP4_P_MetadataInterface *pMetadata = hMP4Mux->pCurrentMetadata;
      BMUXlib_File_MP4_P_MetadataCache *pCache = &pMetadata->stCache;
      BMUXlib_File_MP4_P_Output *pOutput = pMetadata->pOutput;
      uint32_t uiEntriesToFetch = (pMetadata->uiEntryCount >= pCache->uiNumEntries)?
                              pCache->uiNumEntries:
                              pMetadata->uiEntryCount;
      uint32_t uiLength = uiEntriesToFetch * pCache->uiEntrySize;

      /* fetch an output descriptor for this read transaction */
      if (!BMUXlib_Output_IsSpaceAvailable(pOutput->hOutput))
      {
         /* no descriptors available: wait and allocate the descriptors next time */
         BDBG_LEAVE(BMUXlib_File_MP4_P_RetrieveMetadataToCache);
         return false;
      }

      /* NOTE: no callback data entry required: cache reference is sufficient */
      InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eMetadataCache, pCache);
      DebugSetDescriptorSource(pOutput, "Metadata");
      /* read the specified metadata back from storage into the relevant metadata cache.
         NOTE: The data is always written to the start of the cache */
      BMUXlib_File_MP4_P_OutputDescriptorRead(hMP4Mux, pOutput, pCache->pBuffer, uiLength, BMUXlib_File_MP4_P_OutputCallback_eMetadataCache);

      pCache->uiWriteIndex = uiEntriesToFetch;        /* indicate that the cache is full */
      pMetadata->uiEntryCount -= uiEntriesToFetch;    /* determine how many entries remain to be retrieved */
      BDBG_MSG(("Retrieving metadata from storage (%d entries, %d bytes, %d entries remain) ... ", uiEntriesToFetch, uiLength, pMetadata->uiEntryCount));
   }
   BDBG_LEAVE(BMUXlib_File_MP4_P_RetrieveMetadataToCache);
   return bResult;
}

/*
   Stts and Ctts are run-length coded and therefore, prior to creating the moov, this function is called
   one last time to write any remaining run-length out to the cache for each active track.
   All other metadata types are not affected.

   Note: This function does not need to flush the caches after the update - this is done by the final
         flush of all the caches that occurs after this is complete
*/
void BMUXlib_File_MP4_P_FinalizeMetadata(BMUXlib_File_MP4_Handle hMP4Mux)
{
   uint32_t uiTrackIndex;
   uint32_t uiMetadataType;

   BDBG_ENTER(BMUXlib_File_MP4_P_FinalizeMetadata);
   for (uiTrackIndex = 0; uiTrackIndex < hMP4Mux->uiNumTracks; uiTrackIndex++)
   {
      BMUXlib_File_MP4_P_TrackInfo *pTrack = &hMP4Mux->aTracks[uiTrackIndex];
      for (uiMetadataType = BMUXlib_File_MP4_P_MetadataType_eStts; uiMetadataType <= BMUXlib_File_MP4_P_MetadataType_eCtts; uiMetadataType++)
      {
         BMUXlib_File_MP4_P_MetadataInterface *pInterface = &pTrack->aMetadata[uiMetadataType];
         BMUXlib_File_MP4_P_MetadataCache *pCache = &pInterface->stCache;
         uint32_t uiWriteIndex = pCache->uiWriteIndex;

         if (BMUXlib_File_MP4_P_MetadataType_eStts == uiMetadataType)
         { /* update the Stts metadata ... */
            BMUXlib_File_MP4_DTSDelta *pBuffer = (BMUXlib_File_MP4_DTSDelta *)pCache->pBuffer;
            BDBG_MSG(("Writing stts metadata @ index %d: c:%d v:%d", uiWriteIndex, pInterface->uiRunCount, pInterface->uiCurrentValue));
            pBuffer[uiWriteIndex].uiDelta = MP4MUX_SWAP_U32(pInterface->uiCurrentValue);
            pBuffer[uiWriteIndex++].uiCount = MP4MUX_SWAP_U32(pInterface->uiRunCount);
         }
         else
         {
            /* update the Ctts metadata ... */
            BMUXlib_File_MP4_CTSDTSOffset *pBuffer = (BMUXlib_File_MP4_CTSDTSOffset *)pCache->pBuffer;
            BDBG_MSG(("Writing ctts metadata @ index %d: c:%d v:%d", uiWriteIndex, pInterface->uiRunCount, pInterface->uiCurrentValue));
            pBuffer[uiWriteIndex].uiOffset = MP4MUX_SWAP_U32(pInterface->uiCurrentValue);
            pBuffer[uiWriteIndex++].uiCount = MP4MUX_SWAP_U32(pInterface->uiRunCount);
         }
         pInterface->uiEntryCount++;
         if (uiWriteIndex == pCache->uiNumEntries)
               uiWriteIndex = 0;                                  /* check for wrap */
         BDBG_ASSERT(0 == pInterface->uiEntryCount || uiWriteIndex != pCache->uiReadIndex);     /* cache overflow */
         pCache->uiWriteIndex = uiWriteIndex;
      } /* end: for metadata type */
   } /* end: for track */
   BDBG_LEAVE(BMUXlib_File_MP4_P_FinalizeMetadata);
}

/*
   Flush remaining metadata in all caches for all active tracks
   This is final cleanup before processing the metadata to create the moov

   returns true if all caches flushed successfully, else returns false
   (cache flush is blocked due to lack of storage descriptors).
*/
bool BMUXlib_File_MP4_P_FlushAllMetadata(BMUXlib_File_MP4_Handle hMP4Mux)
{
   uint32_t uiTrackIndex;
   uint32_t uiMetadataType;
   bool bDone = false;

   BDBG_ENTER(BMUXlib_File_MP4_P_FlushAllMetadata);

   for (uiTrackIndex = 0; uiTrackIndex < hMP4Mux->uiNumTracks; uiTrackIndex++)
   {
      BMUXlib_File_MP4_P_TrackInfo *pTrack = &hMP4Mux->aTracks[uiTrackIndex];
      for (uiMetadataType = 0; uiMetadataType < BMUXlib_File_MP4_P_MetadataType_eMax; uiMetadataType++)
      {
         BMUXlib_File_MP4_P_MetadataInterface *pInterface = &pTrack->aMetadata[uiMetadataType];

         /* no need to flush if all entries fit in cache - data will be read directly from cache, not from storage ... */
         if (pInterface->uiEntryCount <= pInterface->stCache.uiNumEntries)
         {
            bDone = true;
            continue;
         }

         bDone = FlushMetadataCacheToStorage(hMP4Mux, pInterface);
         if (!bDone)
            break;            /* this cache could not be flushed yet ... wait */

      }
      if (!bDone)
         break;               /* no point continuing (no more descriptors) ... */
   }

   BDBG_LEAVE(BMUXlib_File_MP4_P_FlushAllMetadata);
   return bDone;
}

/* Free data in the metadata cache being referred to by the current output descriptor
   This simply moves the Read Index to the next half of the cache (ping-pong) */
void BMUXlib_File_MP4_P_FreeMetadataCache(BMUXlib_File_MP4_P_MetadataCache *pMetadataCache)
{
   uint32_t uiReadIndex = pMetadataCache->uiReadIndex;
   BDBG_ENTER(BMUXlib_File_MP4_P_FreeMetadataCache);
   pMetadataCache->uiReadIndex = (0 == uiReadIndex)?(pMetadataCache->uiNumEntries /2): 0;
   BDBG_MSG(("Freeing Metadata Cache (read index %d -> %d)", uiReadIndex, pMetadataCache->uiReadIndex));
   /* NOTE: it does not matter for the last "flush" that this does not free the right amount of data;
            the cache will be reset after the last flush to allow read from storage into the cache */
   BDBG_LEAVE(BMUXlib_File_MP4_P_FreeMetadataCache);
}

/***********************************
* S T A T I C   F U N C T I O N S  *
***********************************/

/*
   Flush half the cache to storage from the current read index
   Returns true when descriptor queued, else returns false
   (blocked due to lack of descriptors).
*/
static bool FlushMetadataCacheToStorage(BMUXlib_File_MP4_Handle hMP4Mux, BMUXlib_File_MP4_P_MetadataInterface *pInterface)
{
   BMUXlib_File_MP4_P_MetadataCache *pCache = &pInterface->stCache;
   uint8_t *pData;
   uint32_t uiLength;
   uint32_t uiHalfEntries = pCache->uiNumEntries / 2;
   uint32_t uiEntriesToFlush;
   int32_t iEntriesInCache = pCache->uiWriteIndex - pCache->uiReadIndex;

   if (iEntriesInCache < 0)
      iEntriesInCache += pCache->uiNumEntries;
   BDBG_MSG(("Entries in Cache = %d", iEntriesInCache));
   if (0 == iEntriesInCache)
      return true;                  /* this cache does not need to be flushed (end-of-stream condition) */

   /* fetch a descriptor to describe for this transaction */
   if (!BMUXlib_Output_IsSpaceAvailable(pInterface->pOutput->hOutput))
      return false;           /* no more descriptors to write the cache - try again later */

   /* determine which half of the cache to flush (flush from the read index) ... */
   pData = (uint8_t *)pCache->pBuffer + (pCache->uiReadIndex * pCache->uiEntrySize);

   /* if reading from first half of buffer (read index = 0), then flush half the entries,
      else flush the rest (allows for odd-length cache) */
   uiEntriesToFlush = (0 == pCache->uiReadIndex)?uiHalfEntries:(pCache->uiNumEntries - uiHalfEntries);

   /* if amount of data in cache is < amount to flush, write what's there (final flush) */
   if ((uint32_t)iEntriesInCache < uiEntriesToFlush)
      uiEntriesToFlush = (uint32_t)iEntriesInCache;

   uiLength = uiEntriesToFlush * pCache->uiEntrySize;

   BDBG_MSG(("Writing cache %d entries (%d bytes) from index: %d", uiEntriesToFlush, uiLength, pCache->uiReadIndex));

   /* write the descriptor for the transaction ... */
   /* NOTE: no callback data entry required: cache reference is sufficient */
   InitOutputCallback(hMP4Mux, BMUXlib_File_MP4_P_OutputCallback_eMetadataCache, pCache);
   DebugSetDescriptorSource(pInterface->pOutput, "Metadata");
   /* append entries to the metadata storage ... */
   BMUXlib_File_MP4_P_OutputDescriptorAppend(hMP4Mux, pInterface->pOutput, pData, uiLength, BMUXlib_File_MP4_P_OutputCallback_eMetadataCache);

   return true;
}

/*****************************************************************************
* EOF
******************************************************************************/
