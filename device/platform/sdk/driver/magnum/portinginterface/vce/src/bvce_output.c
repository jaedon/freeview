/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_output.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/11/12 12:09p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_output.c $
 * 
 * Hydra_Software_Devel/5   10/11/12 12:09p nilesh
 * SW7425-2568: Updated to use BVCE_Debug logging functions
 * 
 * Hydra_Software_Devel/4   10/5/12 10:17p nilesh
 * SW7445-39: Advanced ITB parser
 * 
 * Hydra_Software_Devel/3   10/3/12 4:26p nilesh
 * SW7425-2718: Added STC snapshot support
 * 
 * Hydra_Software_Devel/2   10/3/12 12:39p nilesh
 * SW7445-39: Added framework for updated ITB parser
 * 
 * Hydra_Software_Devel/1   10/1/12 2:03p nilesh
 * SW7445-39: Split power and output code into separate files
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "bvce.h"
#include "bvce_priv.h"

#include "bvce_debug.h"

#include "bvce_output.h"
#include "bvce_power.h"

BDBG_MODULE(BVCE_OUTPUT);
BDBG_FILE_MODULE(BVCE_OUTPUT_ITB);
BDBG_FILE_MODULE(BVCE_OUTPUT_ITB_INDEX);
BDBG_FILE_MODULE(BVCE_OUTPUT_DESC);
BDBG_FILE_MODULE(BVCE_OUTPUT_CACHE);

/**********/
/* Output */
/**********/

void
BVCE_Output_GetDefaultAllocBuffersSettings(
         BVCE_Output_AllocBuffersSettings *pstOutputAllocBuffersSettings
   )
{
   BDBG_ENTER( BVCE_Output_GetDefaultAllocBuffersSettings );

   BDBG_ASSERT( pstOutputAllocBuffersSettings );

   BKNI_Memset(
            pstOutputAllocBuffersSettings,
            0,
            sizeof( BVCE_Output_AllocBuffersSettings)
            );

   pstOutputAllocBuffersSettings->uiSignature = BVCE_P_SIGNATURE_ALLOCBUFFERSSETTINGS;
   pstOutputAllocBuffersSettings->stConfig.Cdb.Length = BVCE_P_DEFAULT_CDB_SIZE;
   pstOutputAllocBuffersSettings->stConfig.Cdb.Alignment = BVCE_P_DEFAULT_CDB_ALIGNMENT;
   pstOutputAllocBuffersSettings->stConfig.Itb.Length = BVCE_P_DEFAULT_ITB_SIZE;
   pstOutputAllocBuffersSettings->stConfig.Itb.Alignment = BVCE_P_DEFAULT_ITB_ALIGNMENT;

   BDBG_LEAVE( BVCE_Output_GetDefaultAllocBuffersSettings );

   return;
}

/* BVCE_Output_AllocBuffer - Allocates ITB/CDB data (in *addition* to any memory allocated in BVCE_Open) */
BERR_Code
BVCE_Output_AllocBuffers(
         BVCE_Handle hVce,
         BVCE_OutputBuffers_Handle *phVceOutputBuffers,
         const BVCE_Output_AllocBuffersSettings *pstOutputAllocBuffersSettings /* [in] VCE Output Alloc Buffer settings */
         )
{
   BERR_Code rc;
   BVCE_OutputBuffers_Handle hVceOutputBuffers = NULL;

   BDBG_ENTER( BVCE_Output_AllocBuffers );

   BDBG_ASSERT( phVceOutputBuffers );
   BDBG_ASSERT( pstOutputAllocBuffersSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_ALLOCBUFFERSSETTINGS == pstOutputAllocBuffersSettings->uiSignature );

   /* Allocate Output Context */
   hVceOutputBuffers = ( BVCE_OutputBuffers_Handle ) BKNI_Malloc( sizeof( BVCE_P_OutputBuffers ) );
   if ( NULL == hVceOutputBuffers )
   {
      BDBG_ERR( ("Error allocating output buffer context"));
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }
   BKNI_Memset(
            hVceOutputBuffers,
            0,
            sizeof( BVCE_P_OutputBuffers )
            );

   /* Copy Settings */
   hVceOutputBuffers->stSettings = *pstOutputAllocBuffersSettings;

   if ( 0 != ( hVceOutputBuffers->stSettings.stConfig.Cdb.Length % 32 ) )
   {
      BDBG_ERR( ("CDB Length must be a multiple of 32"));
      BKNI_Free( hVceOutputBuffers );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( 0 != ( hVceOutputBuffers->stSettings.stConfig.Itb.Length % 32 ) )
   {
      BDBG_ERR( ("ITB Length must be a multiple of 32"));
      BKNI_Free( hVceOutputBuffers );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   /* Select the heap to use */
   if ( NULL != hVceOutputBuffers->stSettings.hCDBMem )
   {
      hVceOutputBuffers->hCDBMem = hVceOutputBuffers->stSettings.hCDBMem;
   }
   else
   {
      hVceOutputBuffers->hCDBMem = hVce->channel_memory.hMem[BVCE_P_HeapId_eSecure];
   }

   if ( NULL != hVceOutputBuffers->stSettings.hITBMem )
   {
      hVceOutputBuffers->hITBMem = hVceOutputBuffers->stSettings.hITBMem;
   }
   else
   {
      hVceOutputBuffers->hITBMem = hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem];
   }

   /* Allocate memory */
   if ( 0 != hVceOutputBuffers->stSettings.stConfig.Cdb.Length )
   {
      hVceOutputBuffers->pCDBBufferUnCached = BMEM_AllocAligned(
               hVceOutputBuffers->hCDBMem,
               hVceOutputBuffers->stSettings.stConfig.Cdb.Length,
               hVceOutputBuffers->stSettings.stConfig.Cdb.Alignment,
               0
               );
      if ( NULL == hVceOutputBuffers->pCDBBufferUnCached )
      {
         BDBG_ERR( ("Error allocating CDB buffer"));
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }

      BKNI_Memset(
               hVceOutputBuffers->pCDBBufferUnCached,
               0,
               hVceOutputBuffers->stSettings.stConfig.Cdb.Length
               );

      /* Get offset */
      rc = BMEM_ConvertAddressToOffset(
               hVceOutputBuffers->hCDBMem,
               hVceOutputBuffers->pCDBBufferUnCached,
               &hVceOutputBuffers->uiCDBBufferOffset
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR( ("Error converting virtual address to offset"));
         BMEM_Free( hVceOutputBuffers->hCDBMem, hVceOutputBuffers->pCDBBufferUnCached );
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( rc );
      }

      /* Get cached address */
      rc = BMEM_ConvertAddressToCached(
               hVceOutputBuffers->hCDBMem,
               hVceOutputBuffers->pCDBBufferUnCached,
               &hVceOutputBuffers->pCDBBufferCached
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR( ("Error converting virtual address to cached address"));
         BMEM_Free( hVceOutputBuffers->hCDBMem, hVceOutputBuffers->pCDBBufferUnCached );
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( rc );
      }
   }

   if ( 0 != hVceOutputBuffers->stSettings.stConfig.Itb.Length )
   {
      hVceOutputBuffers->pITBBufferUnCached = BMEM_AllocAligned(
               hVceOutputBuffers->hITBMem,
               hVceOutputBuffers->stSettings.stConfig.Itb.Length,
               hVceOutputBuffers->stSettings.stConfig.Itb.Alignment,
               0
               );
      if ( NULL == hVceOutputBuffers->pITBBufferUnCached )
      {
         BDBG_ERR( ("Error allocating ITB buffer"));
         BMEM_Free( hVceOutputBuffers->hCDBMem, hVceOutputBuffers->pCDBBufferUnCached );
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }
      BKNI_Memset(
               hVceOutputBuffers->pITBBufferUnCached,
               0,
               hVceOutputBuffers->stSettings.stConfig.Itb.Length
               );

      /* Get offset */
      rc = BMEM_ConvertAddressToOffset(
               hVceOutputBuffers->hITBMem,
               hVceOutputBuffers->pITBBufferUnCached,
               &hVceOutputBuffers->uiITBBufferOffset
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR( ("Error converting virtual address to offset"));
         BMEM_Free( hVceOutputBuffers->hCDBMem, hVceOutputBuffers->pCDBBufferUnCached );
         BMEM_Free( hVceOutputBuffers->hITBMem, hVceOutputBuffers->pITBBufferUnCached );
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( rc );
      }

      /* Get cached address */
      rc = BMEM_ConvertAddressToCached(
               hVceOutputBuffers->hITBMem,
               hVceOutputBuffers->pITBBufferUnCached,
               &hVceOutputBuffers->pITBBufferCached
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR( ("Error converting virtual address to cached"));
         BMEM_Free( hVceOutputBuffers->hCDBMem, hVceOutputBuffers->pCDBBufferUnCached );
         BMEM_Free( hVceOutputBuffers->hITBMem, hVceOutputBuffers->pITBBufferUnCached );
         BKNI_Free( hVceOutputBuffers );
         return BERR_TRACE( rc );
      }
   }

   *phVceOutputBuffers = hVceOutputBuffers;

   BDBG_LEAVE( BVCE_Output_AllocBuffers );
   return BERR_TRACE( BERR_SUCCESS );
}

/* Should only be called when the buffer is not attached to a Output context */
BERR_Code
BVCE_Output_FreeBuffers(
         BVCE_OutputBuffers_Handle hVceOutputBuffers
         )
{
   BDBG_ENTER( BVCE_Output_FreeBuffers );

   BDBG_ASSERT( hVceOutputBuffers );

   if ( NULL != hVceOutputBuffers->pITBBufferUnCached )
   {
      BMEM_Free(
               hVceOutputBuffers->hITBMem,
               hVceOutputBuffers->pITBBufferUnCached
               );
   }

   if ( NULL != hVceOutputBuffers->pCDBBufferUnCached )
   {
      BMEM_Free(
               hVceOutputBuffers->hCDBMem,
               hVceOutputBuffers->pCDBBufferUnCached
               );
   }

   BKNI_Free(
            hVceOutputBuffers
            );

   BDBG_LEAVE( BVCE_Output_FreeBuffers );

   return BERR_TRACE( BERR_SUCCESS );
}

void
BVCE_Output_GetDefaultOpenSettings(
         BVCE_Output_OpenSettings *pstOutputOpenSettings
   )
{
   BDBG_ENTER( BVCE_Output_GetDefaultOpenSettings );

   BDBG_ASSERT( pstOutputOpenSettings );

   BKNI_Memset(
            pstOutputOpenSettings,
            0,
            sizeof( BVCE_Output_OpenSettings )
            );

   pstOutputOpenSettings->bEnableDataUnitDetection = true;

   pstOutputOpenSettings->uiSignature = BVCE_P_SIGNATURE_OUTPUTOPENSETTINGS;

   pstOutputOpenSettings->uiDescriptorQueueDepth = BVCE_P_MAX_VIDEODESCRIPTORS;

   BDBG_LEAVE( BVCE_Output_GetDefaultOpenSettings );

   return;
}

BERR_Code
BVCE_Output_Open(
   BVCE_Handle hVce,
   BVCE_Output_Handle *phVceOutput,
   const BVCE_Output_OpenSettings *pstOutputOpenSettings /* [in] VCE Output settings */
   )
{
   BERR_Code rc;
   BVCE_Output_Handle hVceOutput;

   BDBG_ENTER( BVCE_Output_Open );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( phVceOutput );
   BDBG_ASSERT( pstOutputOpenSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_OUTPUTOPENSETTINGS == pstOutputOpenSettings->uiSignature );

   if ( pstOutputOpenSettings->uiInstance >= BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS )
   {
      BDBG_ERR( ("Invalid output instance (%d) specified", pstOutputOpenSettings->uiInstance));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   hVceOutput = &hVce->outputs[pstOutputOpenSettings->uiInstance].context;

   if ( BVCE_P_Status_eIdle != hVceOutput->eStatus )
   {
      BDBG_ERR( ("Output instance (%d) already opened", pstOutputOpenSettings->uiInstance));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BKNI_Memset(
      hVceOutput,
      0,
      sizeof( BVCE_P_Output_Context )
      );

   hVceOutput->uiSignature = BVCE_P_SIGNATURE_OUTPUTHANDLE;
   hVceOutput->hVce = hVce;
   hVceOutput->eStatus = BVCE_P_Status_eOpened;
   hVceOutput->stOpenSettings = *pstOutputOpenSettings;

   /*
    * We're reserving a uiDescriptorQueueDepth of 0 for possible future
    * use to indicate that we want an automatically calculated depth. For
    * now if the value is specified as 0, we'll set it to the default value
    * in BVCE_P_MAX_VIDEODESCRIPTORS.
    */
   if ( 0 == pstOutputOpenSettings->uiDescriptorQueueDepth )
   {
      hVceOutput->stOpenSettings.uiDescriptorQueueDepth = BVCE_P_MAX_VIDEODESCRIPTORS;
   }

   /* Platform Init: Set Cabac Registers */
   switch ( hVceOutput->stOpenSettings.uiInstance )
   {
      case 0:
      case 1:
         hVceOutput->stRegisters.CDB_Read = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiReadPointer;
         hVceOutput->stRegisters.CDB_Base = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiBasePointer;
         hVceOutput->stRegisters.CDB_Valid = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiValidPointer;
         hVceOutput->stRegisters.CDB_End = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiEndPointer;

         hVceOutput->stRegisters.ITB_Read = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiReadPointer;
         hVceOutput->stRegisters.ITB_Base = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiBasePointer;
         hVceOutput->stRegisters.ITB_Valid = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiValidPointer;
         hVceOutput->stRegisters.ITB_End = hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiEndPointer;
         break;

      default:
         BDBG_ERR( ("Unsupported output instance (%d)", hVceOutput->stOpenSettings.uiInstance));
         return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   /* Allocate video buffer descriptors */
   hVceOutput->astDescriptorsUnCached = (BAVC_VideoBufferDescriptor *) BMEM_Alloc(
      hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
      ( hVceOutput->stOpenSettings.uiDescriptorQueueDepth * sizeof( BAVC_VideoBufferDescriptor ) )
      );

   if ( NULL == hVceOutput->astDescriptorsUnCached )
   {
      BDBG_ERR(("Error allocating memory"));
      BVCE_Output_Close( hVceOutput );
      return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
   }

   /* Get Cached Address */
   {
      void *pCached;

      rc = BMEM_ConvertAddressToCached(
    hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
    hVceOutput->astDescriptorsUnCached,
    &pCached
    );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting general virtual address to cached"));
         BVCE_Output_Close( hVceOutput );
         return BERR_TRACE( rc );
      }

      hVceOutput->astDescriptors = (BAVC_VideoBufferDescriptor *) pCached;
   }

   BKNI_Memset( hVceOutput->astDescriptors,
                0,
                ( hVceOutput->stOpenSettings.uiDescriptorQueueDepth * sizeof( BAVC_VideoBufferDescriptor ) )
      );

   /* Allocate metadata buffer descriptors */
   hVceOutput->astMetadataDescriptorsUnCached = (BAVC_VideoMetadataDescriptor *) BMEM_Alloc(
      hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
      ( BVCE_P_MAX_METADATADESCRIPTORS * sizeof( BAVC_VideoMetadataDescriptor ) )
      );

   if ( NULL == hVceOutput->astMetadataDescriptorsUnCached )
   {
      BDBG_ERR(("Error allocating memory"));
      BVCE_Output_Close( hVceOutput );
      return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
   }

   /* Get Cached Address */
   {
      void *pCached;

      rc = BMEM_ConvertAddressToCached(
    hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
    hVceOutput->astMetadataDescriptorsUnCached,
    &pCached
    );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting general virtual address to cached"));
         BVCE_Output_Close( hVceOutput );
         return BERR_TRACE( rc );
      }

      hVceOutput->astMetadataDescriptors = (BAVC_VideoMetadataDescriptor *) pCached;
   }

   BKNI_Memset( hVceOutput->astMetadataDescriptors,
                0,
                ( BVCE_P_MAX_METADATADESCRIPTORS * sizeof( BAVC_VideoMetadataDescriptor ) )
      );

   /* Allocate ITB Index */
   hVceOutput->astIndex = ( BVCE_P_Output_ITB_IndexEntry* ) BKNI_Malloc(
      ( hVceOutput->stOpenSettings.uiDescriptorQueueDepth * sizeof( BVCE_P_Output_ITB_IndexEntry ) ) );

   BKNI_Memset( hVceOutput->astIndex, 0, ( hVceOutput->stOpenSettings.uiDescriptorQueueDepth * sizeof( BVCE_P_Output_ITB_IndexEntry ) ) );

   /* Create Mutex */
   rc = BKNI_CreateMutex( &hVceOutput->hMutex );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error creating mutex"));
      BVCE_Output_Close( hVceOutput );
      return BERR_TRACE( rc );
   }

   *phVceOutput = hVceOutput;

   BDBG_LEAVE( BVCE_Output_Open );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Output_Close(
         BVCE_Output_Handle hVceOutput
         )
{
   BDBG_ENTER( BVCE_Output_Close );

   BDBG_ASSERT( hVceOutput );

   /* Destroy Mutex */
   if ( NULL != hVceOutput->hMutex )
   {
      BKNI_DestroyMutex( hVceOutput->hMutex );
      hVceOutput->hMutex= NULL;
   }

   if ( NULL != hVceOutput->astIndex )
   {
      BKNI_Free( hVceOutput->astIndex );

      hVceOutput->astIndex = NULL;
   }

   if ( NULL != hVceOutput->astMetadataDescriptorsUnCached )
   {
      BMEM_Free(
               hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceOutput->astMetadataDescriptorsUnCached
               );

      hVceOutput->astMetadataDescriptorsUnCached = NULL;
   }

   if ( NULL != hVceOutput->astDescriptorsUnCached )
   {
      BMEM_Free(
               hVceOutput->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceOutput->astDescriptorsUnCached
               );

      hVceOutput->astDescriptorsUnCached = NULL;
   }

#if BVCE_P_DUMP_OUTPUT_CDB
   BVCE_Debug_P_CloseLog( hVceOutput->hCDBDumpFile );
   hVceOutput->hCDBDumpFile = NULL;
#endif
#if BVCE_P_DUMP_OUTPUT_ITB
   BVCE_Debug_P_CloseLog( hVceOutput->hITBDumpFile );
   hVceOutput->hITBDumpFile = NULL;
#endif
#if BVCE_P_DUMP_OUTPUT_ITB_DESC
   BVCE_Debug_P_CloseLog( hVceOutput->hITBDescDumpFile );
   hVceOutput->hITBDescDumpFile = NULL;
#endif
#if BVCE_P_TEST_MODE
   BVCE_Debug_P_CloseLog( hVceOutput->hDescriptorLog );
   hVceOutput->hDescriptorLog = NULL;
#endif

   /* TODO: Do we need to do anything else? */
   hVceOutput->eStatus = BVCE_P_Status_eIdle;

   BDBG_LEAVE( BVCE_Output_Close );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Output_SetBuffers(
         BVCE_Output_Handle hVceOutput,
         const BVCE_OutputBuffers_Handle hVceOutputBuffers
         )
{
   BDBG_ENTER( BVCE_Output_SetBuffers );

   BDBG_ASSERT( hVceOutput );

   /* TODO: Do we need to handle NULL output buffer specially? E.g. snapshot read/write pointers? */
   hVceOutput->hOutputBuffers = hVceOutputBuffers;
   BVCE_Output_Reset(
           hVceOutput
           );

   BDBG_LEAVE( BVCE_Output_SetBuffers );
   return BERR_TRACE( BERR_SUCCESS );
}

void
BVCE_Output_P_DataUnitDetectReset(
         BVCE_Output_Handle hVceOutput,
         uint32_t uiCDBValidOffset
         )
{
   hVceOutput->state.uiCDBBufferShadowValidOffset = uiCDBValidOffset;
   hVceOutput->state.uiCDBBufferReadAheadValidOffset = hVceOutput->state.uiCDBBufferShadowValidOffset;
   hVceOutput->state.uiCDBBufferShadowValidOffsetDataUnitDetectionDelay = 0;
   hVceOutput->state.uiDataUnitDetectionShiftRegister = 0x03030303;
}

void
BVCE_Output_P_BufferCacheReset(
         BVCE_Output_Handle hVceOutput
         )
{
   BDBG_ASSERT( hVceOutput );

   BKNI_Memset(
            &hVceOutput->state.stBufferCache,
            0,
            sizeof( hVceOutput->state.stBufferCache )
            );

   if ( NULL != hVceOutput->hOutputBuffers )
   {
      hVceOutput->state.stBufferCache.uiITBCacheValidOffset = hVceOutput->hOutputBuffers->uiITBBufferOffset;
      hVceOutput->state.stBufferCache.uiCDBCacheValidOffset = hVceOutput->hOutputBuffers->uiCDBBufferOffset;
   }
}

/* BVCE_Output_P_BufferCacheUpdate -
 *
 * will flush the cache for the region of ITB/CDB that was written
 * since the last time this function was called
 */
void
BVCE_Output_P_BufferCacheUpdate(
         BVCE_Output_Handle hVceOutput,
         uint32_t uiITBValidOffset,
         uint32_t uiITBBaseOffset,
         uint32_t uiITBEndOffset,
         uint32_t uiCDBValidOffset,
         uint32_t uiCDBBaseOffset,
         uint32_t uiCDBEndOffset
         )
{
   void *pAddress;
   unsigned uiLength;

   if ( 0 == hVceOutput->state.stBufferCache.uiITBCacheValidOffset )
   {
      hVceOutput->state.stBufferCache.uiITBCacheValidOffset = uiITBBaseOffset;
   }

   while ( hVceOutput->state.stBufferCache.uiITBCacheValidOffset != uiITBValidOffset )
   {
      if ( uiITBValidOffset >= hVceOutput->state.stBufferCache.uiITBCacheValidOffset )
      {
         uiLength = uiITBValidOffset - hVceOutput->state.stBufferCache.uiITBCacheValidOffset;
      }
      else
      {
         uiLength = uiITBEndOffset - hVceOutput->state.stBufferCache.uiITBCacheValidOffset;
      }

      pAddress = (void*) ((unsigned) hVceOutput->hOutputBuffers->pITBBufferCached + ( hVceOutput->state.stBufferCache.uiITBCacheValidOffset - uiITBBaseOffset) );

      BMEM_Heap_FlushCache(
               hVceOutput->hOutputBuffers->hITBMem,
               pAddress,
               uiLength
               );

      BDBG_MODULE_MSG(BVCE_OUTPUT_CACHE, ( "ITB Flushed: %p %d bytes",
                  pAddress,
                  uiLength
                  ));

#if BVCE_P_DUMP_OUTPUT_ITB
                  if ( NULL == hVceOutput->hITBDumpFile )
                  {
                     char fname[256];
                     sprintf(fname, "BVCE_OUTPUT_%02d_%02d.itb", hVceOutput->hVceCh->hVce->stOpenSettings.uiInstance, hVceOutput->hVceCh->stOpenSettings.uiInstance);

                     if ( false == BVCE_Debug_P_OpenLog( fname, &hVceOutput->hITBDumpFile ) )
                     {
                        BDBG_ERR(("Error Creating BVCE Output ITB Dump File (%s)", fname));
                     }
                  }

                  if ( NULL != hVceOutput->hITBDumpFile )
                  {
                     fwrite( pAddress,
                             1,
                             uiLength,
                             hVceOutput->hITBDumpFile
                     );

                  }
#endif

      hVceOutput->state.stBufferCache.uiITBCacheValidOffset += uiLength;
      if ( hVceOutput->state.stBufferCache.uiITBCacheValidOffset >= uiITBEndOffset )
      {
         hVceOutput->state.stBufferCache.uiITBCacheValidOffset -= ( uiITBEndOffset - uiITBBaseOffset );
      }
   }

   if ( 0 == hVceOutput->state.stBufferCache.uiCDBCacheValidOffset )
   {
      hVceOutput->state.stBufferCache.uiCDBCacheValidOffset = uiCDBBaseOffset;
   }

   while ( hVceOutput->state.stBufferCache.uiCDBCacheValidOffset != uiCDBValidOffset )
   {
      if ( uiCDBValidOffset >= hVceOutput->state.stBufferCache.uiCDBCacheValidOffset )
      {
         uiLength = uiCDBValidOffset - hVceOutput->state.stBufferCache.uiCDBCacheValidOffset;
      }
      else
      {
         uiLength =  uiCDBEndOffset - hVceOutput->state.stBufferCache.uiCDBCacheValidOffset;
      }

      pAddress = (void*) ((unsigned) hVceOutput->hOutputBuffers->pCDBBufferCached + ( hVceOutput->state.stBufferCache.uiCDBCacheValidOffset - uiCDBBaseOffset) );

      BMEM_Heap_FlushCache(
               hVceOutput->hOutputBuffers->hCDBMem,
               pAddress,
               uiLength
               );

      BDBG_MODULE_MSG(BVCE_OUTPUT_CACHE, ( "CDB Flushed: %p %d bytes",
                  pAddress,
                  uiLength
                  ));

#if BVCE_P_DUMP_OUTPUT_CDB
            if ( NULL == hVceOutput->hCDBDumpFile )
            {
               char fname[256];
               sprintf(fname, "BVCE_OUTPUT_%02d_%02d.cdb", hVceOutput->state.hVceCh->hVce->stOpenSettings.uiInstance, hVceOutput->state.hVceCh->stOpenSettings.uiInstance);

               if ( false == BVCE_Debug_P_OpenLog( fname, &hVceOutput->hCDBDumpFile ) )
               {
                  BDBG_ERR(("Error Creating BVCE Output CDB Dump File (%s)", fname));
               }
            }

            if ( NULL != hVceOutput->hCDBDumpFile )
            {
               fwrite(  pAddress,
                        1,
                        uiLength,
                        hVceOutput->hCDBDumpFile
                        );
            }
#endif

      hVceOutput->state.stBufferCache.uiCDBCacheValidOffset += uiLength;
      if ( hVceOutput->state.stBufferCache.uiCDBCacheValidOffset >= uiCDBEndOffset )
      {
         hVceOutput->state.stBufferCache.uiCDBCacheValidOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
      }
   }
}

BERR_Code
BVCE_Output_Reset(
         BVCE_Output_Handle hVceOutput
         )
{
   BDBG_ENTER( BVCE_Output_Reset );

   /* TODO: need to figure out what to do here */
   BSTD_UNUSED( hVceOutput );

   BKNI_AcquireMutex( hVceOutput->hMutex );

   BKNI_Memset( &hVceOutput->state, 0, sizeof(hVceOutput->state) );

   if ( NULL != hVceOutput->hOutputBuffers )
   {
      hVceOutput->state.stITBBuffer.uiShadowReadOffset = hVceOutput->hOutputBuffers->uiITBBufferOffset;
      hVceOutput->state.uiCDBBufferShadowReadOffset = hVceOutput->hOutputBuffers->uiCDBBufferOffset;
   }
   BVCE_Output_P_DataUnitDetectReset( hVceOutput, hVceOutput->state.uiCDBBufferShadowReadOffset );
   BVCE_Output_P_BufferCacheReset( hVceOutput );

   hVceOutput->state.bFrameStart = true;

   BREG_Write32(
      hVceOutput->hVce->handles.hReg,
      hVceOutput->hVce->stPlatformConfig.stInterrupt.uiInterruptClearRegister,
      hVceOutput->hVce->stPlatformConfig.stInterrupt.stMask.uiDataReady[hVceOutput->stOpenSettings.uiInstance]
   );

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
   BVCE_Debug_P_CloseLog( hVceOutput->hITBDescDumpFile );
   hVceOutput->hITBDescDumpFile = NULL;
#endif

#if BVCE_P_TEST_MODE
   BVCE_Debug_P_CloseLog( hVceOutput->hDescriptorLog );
   hVceOutput->hDescriptorLog = NULL;
#endif

   BKNI_ReleaseMutex( hVceOutput->hMutex );

   BDBG_LEAVE( BVCE_Output_Reset );

   return BERR_TRACE( BERR_SUCCESS );
}

/* BVCE_Output_GetRegisters -
 * Returns the registers associated with the specified output hardware
 */
BERR_Code
BVCE_Output_GetRegisters(
         BVCE_Output_Handle hVceOutput,
         BAVC_VideoContextMap *pstVceOutputRegisters
         )
{
   BDBG_ENTER( BVCE_Output_GetRegisters);

   BDBG_ASSERT( hVceOutput );
   BDBG_ASSERT( pstVceOutputRegisters );

   *pstVceOutputRegisters = hVceOutput->stRegisters;

   BDBG_LEAVE( BVCE_Output_GetRegisters);
   return BERR_TRACE( BERR_SUCCESS );
}

/*
 * Moved ITB structure definition and accessor macros to bvce_priv.h on 7/18/11 - PAB
 */


void
BVCE_OUTPUT_P_ITB_UpdatePointers(
   BVCE_Output_Handle hVceOutput
   )
{ /* ITB Pointers */
   hVceOutput->state.stITBBuffer.uiBaseOffset = BREG_Read32(
            hVceOutput->hVce->handles.hReg,
            hVceOutput->stRegisters.ITB_Base
            );

   hVceOutput->state.stITBBuffer.uiEndOffset = BREG_Read32(
            hVceOutput->hVce->handles.hReg,
            hVceOutput->stRegisters.ITB_End
            );

   /* HW treats END as last valid byte before wrap.
    * It is easier for SW to treat END as the byte
    * AFTER the last valid byte before WRAP, so we
    * add one here
    */
   hVceOutput->state.stITBBuffer.uiEndOffset += 1;

   hVceOutput->state.stITBBuffer.uiValidOffset = BREG_Read32(
            hVceOutput->hVce->handles.hReg,
            hVceOutput->stRegisters.ITB_Valid
            );

   hVceOutput->state.stITBBuffer.uiValidOffset += 1;

   if ( hVceOutput->state.stITBBuffer.uiValidOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
   {
      hVceOutput->state.stITBBuffer.uiValidOffset = hVceOutput->state.stITBBuffer.uiBaseOffset + ( hVceOutput->state.stITBBuffer.uiValidOffset - hVceOutput->state.stITBBuffer.uiEndOffset );
   }

   hVceOutput->state.stITBBuffer.uiWriteOffset = BREG_Read32(
           hVceOutput->hVce->handles.hReg,
           hVceOutput->hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiWritePointer
           );

   hVceOutput->state.stITBBuffer.uiReadOffset = BREG_Read32(
           hVceOutput->hVce->handles.hReg,
           hVceOutput->stRegisters.ITB_Read
           );
}

void
BVCE_OUTPUT_P_ITB_CheckForEOS(
   BAVC_VideoBufferDescriptor *pVideoDescriptor
   )
{

   if ( ( 0 == pVideoDescriptor->stCommon.iSHR )
        && ( 0 == pVideoDescriptor->stCommon.uiTicksPerBit )
        && ( 0 == pVideoDescriptor->stCommon.uiPTS )
        && ( 0 == pVideoDescriptor->uiDTS )
        && ( 0 == pVideoDescriptor->stCommon.uiOriginalPTS )
      )
   {
      pVideoDescriptor->stCommon.uiFlags = BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS;

      /* SW7425-3360: Set EOC when EOS is seen */
      pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC;
   }
}

void
BVCE_OUTPUT_P_ITB_UpdateIndex(
   BVCE_Output_Handle hVceOutput
   )
{
   unsigned uiReadOffset = hVceOutput->state.stITBBuffer.uiReadOffset;

   if ( true == hVceOutput->state.stITBBuffer.bReadHackDone )
   {
      uiReadOffset++;
      if ( uiReadOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
      {
         uiReadOffset -= ( hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
      }
   }

   /* Skip ahead to the new ITB entries */
   {
      unsigned uiIndexReadOffset = hVceOutput->state.stITBBuffer.uiIndexReadOffset;

      while ( uiIndexReadOffset != hVceOutput->state.stITBBuffer.uiIndexWriteOffset )
      {
         uiReadOffset += hVceOutput->astIndex[uiIndexReadOffset].uiSizeInITB;

         if ( uiReadOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
         {
            uiReadOffset -= ( hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
         }

         uiIndexReadOffset++;
         uiIndexReadOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;
      }
   }

   /* Scan ITB for entries */
   {
      typedef enum BVCE_OUTPUT_P_ITB_IndexingState
      {
         BVCE_OUTPUT_P_ITB_IndexingState_eBaseEntry = 0x20, /* Indicates start of new frame (0x20) */
         BVCE_OUTPUT_P_ITB_IndexingState_eTimeStampEntry = 0x21, /* PTS/DTS Entry (0x21) */
         BVCE_OUTPUT_P_ITB_IndexingState_eBitrateEntry = 0x60, /* Bitrate Entry (0x60) */
         BVCE_OUTPUT_P_ITB_IndexingState_eESCREntry = 0x61, /* ESCR Entry (0x61) */
         BVCE_OUTPUT_P_ITB_IndexingState_eCRCEntry = 0x6C, /* CRC Entry (0x6C) */
         BVCE_OUTPUT_P_ITB_IndexingState_eSTCEntry = 0x68, /* STC Entry (0x68) */
         BVCE_OUTPUT_P_ITB_IndexingState_eNULLEntry = 0x00 /* NULL Entry (0x00) */
      } BVCE_OUTPUT_P_ITB_IndexingState;

      while ( uiReadOffset != hVceOutput->state.stITBBuffer.uiValidOffset ) /* Until end of ITB is reached */
      {
         void* pITBEntry = (void*) (((uint8_t *)hVceOutput->hOutputBuffers->pITBBufferCached) + (uiReadOffset - hVceOutput->state.stITBBuffer.uiBaseOffset));
         BVCE_OUTPUT_P_ITB_IndexingState eIndexState = BVCE_P_ITBEntry_GetEntryType(pITBEntry);
         BAVC_VideoBufferDescriptor *pVideoDescriptor = &hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].stFrameDescriptor;

         /* Check of index is full */
         {
            unsigned uiWriteOffsetNext = (hVceOutput->state.stITBBuffer.uiIndexWriteOffset + 1) % hVceOutput->stOpenSettings.uiDescriptorQueueDepth;
            if ( uiWriteOffsetNext == hVceOutput->state.stITBBuffer.uiIndexReadOffset )
            {
               break;
            }
         }

         switch ( eIndexState )
         {
            case BVCE_OUTPUT_P_ITB_IndexingState_eBaseEntry:
               if ( hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiSizeInITB > 0 )
               {
                  BVCE_OUTPUT_P_ITB_CheckForEOS(&hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].stFrameDescriptor);

                  hVceOutput->state.stITBBuffer.uiIndexWriteOffset++;
                  hVceOutput->state.stITBBuffer.uiIndexWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

                  BKNI_Memset( &hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset], 0, sizeof( BVCE_P_Output_ITB_IndexEntry ) );
                  pVideoDescriptor = &hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].stFrameDescriptor;
               }
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: BASE", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiCDBAddress = BVCE_P_ITBEntry_GetCDBAddress(pITBEntry);
               BDBG_ASSERT( 0 != hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiCDBAddress );
               hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].bError = BVCE_P_ITBEntry_GetError(pITBEntry);

               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START;
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eTimeStampEntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: TIMESTAMP", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               if ( 0 != BVCE_P_ITBEntry_GetIFrame(pITBEntry) )
               {
                  pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP;
               }
               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID;
               pVideoDescriptor->stCommon.uiPTS = BVCE_P_ITBEntry_GetPTS(pITBEntry);

               pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID;
               pVideoDescriptor->uiDTS = BVCE_P_ITBEntry_GetDTS(pITBEntry);
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eBitrateEntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: BITRATE", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );

               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID;
               pVideoDescriptor->stCommon.uiTicksPerBit = BVCE_P_ITBEntry_GetTicksPerBit(pITBEntry);

               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID;
               pVideoDescriptor->stCommon.iSHR = BVCE_P_ITBEntry_GetSHR(pITBEntry);
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eESCREntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: ESCR", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID;
               pVideoDescriptor->stCommon.uiOriginalPTS = BVCE_P_ITBEntry_GetOriginalPTS(pITBEntry);

               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID;
               pVideoDescriptor->stCommon.uiESCR = BVCE_P_ITBEntry_GetESCR(pITBEntry);

               hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiMetadata = BVCE_P_ITBEntry_GetMetadata(pITBEntry);

               /* SW7425-3360: Set End Of Chunk (EOC) flag */
               if ( 0 != ( ( BVCE_P_ITBEntry_GetMetadata(pITBEntry) >> 28 ) & 0x1 ) )
               {
                  pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC;
               }
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eCRCEntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: CRC", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eSTCEntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: STC", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_STCSNAPSHOT_VALID;
               pVideoDescriptor->stCommon.uiSTCSnapshot = BVCE_P_ITBEntry_GetStcSnapshot(pITBEntry);
               break;

            case BVCE_OUTPUT_P_ITB_IndexingState_eNULLEntry:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: NULL", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               break;

            default:
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB_INDEX,("%3d: UNKNOWN", hVceOutput->state.stITBBuffer.uiIndexWriteOffset) );
               break;
         }

         hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiSizeInITB += 16;

         uiReadOffset += 16;
         if ( uiReadOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
         {
            uiReadOffset -= ( hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
         }
      }

      if ( hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].uiSizeInITB > 0 )
      {
         BVCE_OUTPUT_P_ITB_CheckForEOS(&hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset].stFrameDescriptor);

         hVceOutput->state.stITBBuffer.uiIndexWriteOffset++;
         hVceOutput->state.stITBBuffer.uiIndexWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

         BKNI_Memset( &hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexWriteOffset], 0, sizeof( BVCE_P_Output_ITB_IndexEntry ) );
      }
   }
}

unsigned
BVCE_OUTPUT_P_ITB_Shadow_GetNumFrames(
   BVCE_Output_Handle hVceOutput
   )
{
   unsigned uiNumFrames = 0;

   if ( hVceOutput->state.stITBBuffer.uiIndexWriteOffset > hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset )
   {
      uiNumFrames = hVceOutput->state.stITBBuffer.uiIndexWriteOffset - hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset;
   }
   else
   {
      uiNumFrames = hVceOutput->stOpenSettings.uiDescriptorQueueDepth - hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset;
      uiNumFrames += hVceOutput->state.stITBBuffer.uiIndexWriteOffset;
   }

   return uiNumFrames;
}

void*
BVCE_OUTPUT_P_ITB_Shadow_GetFrameEntry(
   BVCE_Output_Handle hVceOutput,
   unsigned uiIndex
   )
{
   uint32_t uiOffsetToNext = ( hVceOutput->state.stITBBuffer.uiShadowReadOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
   unsigned uiShadowReadOffset = hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset;

   while( uiIndex )
   {
      uiOffsetToNext += hVceOutput->astIndex[uiShadowReadOffset].uiSizeInITB;
      uiShadowReadOffset++;
      uiShadowReadOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;
      uiIndex--;
   }

   uiOffsetToNext %= hVceOutput->hOutputBuffers->stSettings.stConfig.Itb.Length;

   return (void*) ((uint8_t *)hVceOutput->hOutputBuffers->pITBBufferCached + uiOffsetToNext );
}

BVCE_P_Output_ITB_IndexEntry*
BVCE_OUTPUT_P_ITB_Shadow_GetFrameIndexEntry(
   BVCE_Output_Handle hVceOutput,
   unsigned uiIndex
   )
{
   unsigned uiOffset = (hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset + uiIndex) % hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

   return &hVceOutput->astIndex[uiOffset];
}

void
BVCE_OUTPUT_P_ITB_Shadow_CalculateDepth(
   BVCE_Output_Handle hVceOutput
   )
{
   if ( hVceOutput->state.stITBBuffer.uiValidOffset >= hVceOutput->state.stITBBuffer.uiShadowReadOffset )
   {
      hVceOutput->state.stITBBuffer.uiShadowDepth = hVceOutput->state.stITBBuffer.uiValidOffset - hVceOutput->state.stITBBuffer.uiShadowReadOffset;
   }
   else
   {
      hVceOutput->state.stITBBuffer.uiShadowDepth = hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiShadowReadOffset;
      hVceOutput->state.stITBBuffer.uiShadowDepth += hVceOutput->state.stITBBuffer.uiValidOffset - hVceOutput->state.stITBBuffer.uiBaseOffset;
   }
}

void
BVCE_OUTPUT_P_ITB_Shadow_ConsumeEntry(
   BVCE_Output_Handle hVceOutput
   )
{
   BDBG_ASSERT( hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset != hVceOutput->state.stITBBuffer.uiIndexWriteOffset );

   hVceOutput->state.stITBBuffer.uiShadowReadOffset += hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset].uiSizeInITB;
   if ( hVceOutput->state.stITBBuffer.uiShadowReadOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
   {
      hVceOutput->state.stITBBuffer.uiShadowReadOffset -= ( hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
   }

   hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset++;
   hVceOutput->state.stITBBuffer.uiIndexShadowReadOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;
}

void
BVCE_OUTPUT_P_ITB_ConsumeEntry(
   BVCE_Output_Handle hVceOutput
   )
{
   BDBG_ASSERT( hVceOutput->state.stITBBuffer.uiIndexReadOffset != hVceOutput->state.stITBBuffer.uiIndexWriteOffset );

   hVceOutput->state.stITBBuffer.uiReadOffset += hVceOutput->astIndex[hVceOutput->state.stITBBuffer.uiIndexReadOffset].uiSizeInITB;
   /* 7425A0: Hack to prevent HW/SW confusion of full vs empty */
   if ( false == hVceOutput->state.stITBBuffer.bReadHackDone )
   {
      hVceOutput->state.stITBBuffer.uiReadOffset--;
      hVceOutput->state.stITBBuffer.bReadHackDone = true;
   }
   if ( hVceOutput->state.stITBBuffer.uiReadOffset >= hVceOutput->state.stITBBuffer.uiEndOffset )
   {
      hVceOutput->state.stITBBuffer.uiReadOffset -= ( hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiBaseOffset );
   }

   hVceOutput->state.stITBBuffer.uiIndexReadOffset++;
   hVceOutput->state.stITBBuffer.uiIndexReadOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;
}

/* NOTE: for AVC, This code only works as long as the encoder produces 4-byte start codes (i.e. with a leading zero).
   if the encoder ever produces standard start-codes, this code will need to be modified to deal with the leading zero
   For AVC, the start code will occupy the entire data unit detection shift register
   For mp4v, the start code will occupy only the lowest 3 bytes of the shift register
   (MPEG 4 start code is 00 00 01 XX where XX is the DU identifier) */
#define BVCE_P_DU_START_CODE      0x00000001

#define BVCE_P_NALU_START_CODE_MASK    0xFFFFFFFF
#define BVCE_P_MP4V_START_CODE_MASK    0x00FFFFFF

#define BVCE_P_NALU_START_CODE_SIZE    4
#define BVCE_P_MP4V_START_CODE_SIZE    3

#define BVCE_P_IS_START_CODE(x)  (BVCE_P_DU_START_CODE == ((x) & uiDataUnitShiftRegisterMask))


/* BVCE_Output_GetBufferDescriptors -
 * Returns video buffer descriptors for CDB content in the
 * BAVC_VideoBufferDescriptor array(s)
 */
BERR_Code
BVCE_Output_GetBufferDescriptors(
   BVCE_Output_Handle hVceOutput,
   const BAVC_VideoBufferDescriptor **astDescriptors0,
   size_t *puiNumDescriptors0,
   const BAVC_VideoBufferDescriptor **astDescriptors1,
   size_t *puiNumDescriptors1
   )
{
   BERR_Code getStatusRC = BERR_UNKNOWN;
   BVCE_Channel_Status stChannelStatus;

   BDBG_ENTER( BVCE_Output_GetBufferDescriptors );

   if ( ( NULL == astDescriptors0 )
        || ( NULL == puiNumDescriptors0 )
        || ( NULL == astDescriptors1 )
        || ( NULL == puiNumDescriptors1 ) )
   {
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   *astDescriptors0 = NULL;
   *puiNumDescriptors0 = 0;
   *astDescriptors1 = NULL;
   *puiNumDescriptors1 = 0;

   BKNI_AcquireMutex( hVceOutput->hMutex );

   BVCE_Power_P_AcquireResource(
         hVceOutput->hVce,
         BVCE_Power_Type_eClock
         );

   if ( ( false == hVceOutput->state.bMetadataSent )
        && ( NULL != hVceOutput->state.hVceCh ) )
   {
      getStatusRC = BERR_TRACE(
         BVCE_Channel_GetStatus(
            hVceOutput->state.hVceCh,
            &stChannelStatus
            ));
   }

#if BVCE_P_POLL_FW_DATAREADY
   /* Check to see if the Cabac Interrupt Fired */
   if ( ( false == hVceOutput->state.bCabacInitialized )
        && ( NULL != hVceOutput->state.hVceCh )
        && ( BVCE_P_Status_eStarted == hVceOutput->state.hVceCh->eStatus ) )
   {
      uint32_t uiViceInterruptStatus = BREG_Read32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->hVce->stPlatformConfig.stInterrupt.uiInterruptStatusRegister
               );

      BDBG_MSG( ("L2 Status[%d]: %08x", hVceOutput->stOpenSettings.uiInstance, uiViceInterruptStatus));

      hVceOutput->state.bCabacInitialized = (uiViceInterruptStatus & hVceOutput->hVce->stPlatformConfig.stInterrupt.stMask.uiDataReady[hVceOutput->stOpenSettings.uiInstance]) ? true : false;
   }
#endif

   /* Make sure cabac itb interrupt fired before even looking at the ITB for the first time */
   if ( true == hVceOutput->state.bCabacInitialized )
   {
      BVCE_P_Output_ITB_IndexEntry *pITBIndexEntry = NULL;
      BVCE_P_Output_ITB_IndexEntry *pITBIndexEntryNext = NULL;

      BAVC_VideoBufferDescriptor *pVideoDescriptor;
      uint32_t uiCDBBaseOffset;
      uint32_t uiCDBEndOffset;
      uint32_t uiCDBValidOffset;
      uint32_t uiCDBReadOffset;
      uint32_t uiCDBWriteOffset;
      uint32_t uiCDBEndOfPictureOffset;

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
      uint32_t uiCDBValidOffsetHW;
      uint32_t uiCDBReadOffsetHW;
      uint32_t uiCDBWriteOffsetHW;
      uint32_t uiCDBDepthHW;
#endif

      /* Snapshot the CDB/ITB read/valid pointers
       * NOTE: We MUST snapshot the CDB pointers first,
       * and then the ITB pointers so that we can properly
       * detect the end of the current frame.  If we read
       * the ITB first, and then the CDB, it is possible a
       * new ITB entry has been written in between the reads,
       * and the CDB write pointer now includes the beginning
       * of the next frame */
      { /* CDB Pointers */
         uiCDBBaseOffset = BREG_Read32(
                  hVceOutput->hVce->handles.hReg,
                  hVceOutput->stRegisters.CDB_Base
                  );

         uiCDBEndOffset = BREG_Read32(
                  hVceOutput->hVce->handles.hReg,
                  hVceOutput->stRegisters.CDB_End
                  );
         /* HW treats END as last valid byte before wrap.
          * It is easier for SW to treat END as the byte
          * AFTER the last valid byte before WRAP, so we
          * add one here
          */
         uiCDBEndOffset += 1;

         uiCDBValidOffset = BREG_Read32(
                  hVceOutput->hVce->handles.hReg,
                  hVceOutput->stRegisters.CDB_Valid
                  );

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
         uiCDBValidOffsetHW = uiCDBValidOffset;
#endif

         uiCDBValidOffset += 1;

         if ( uiCDBValidOffset >= uiCDBEndOffset )
         {
            uiCDBValidOffset = uiCDBBaseOffset + ( uiCDBValidOffset - uiCDBEndOffset );
         }

         uiCDBWriteOffset = BREG_Read32(
                 hVceOutput->hVce->handles.hReg,
                 hVceOutput->hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiWritePointer
                 );

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
         uiCDBWriteOffsetHW = uiCDBWriteOffset;
#endif

         uiCDBReadOffset = BREG_Read32(
                 hVceOutput->hVce->handles.hReg,
                 hVceOutput->stRegisters.CDB_Read
                 );

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
         uiCDBReadOffsetHW = uiCDBReadOffset;

         uiCDBDepthHW = BREG_Read32(
             hVceOutput->hVce->handles.hReg,
             hVceOutput->hVce->stPlatformConfig.stDebug.uiCDBDepth[hVceOutput->stOpenSettings.uiInstance] );
#endif

      }

      /* ITB Pointers MUST be updated AFTER CDB pointers to prevent race condition
       * If next picture is written to ITB/CDB after ITB has been updated but before
       * CDB has been updated, then when CDB pointers are updated, it will contain
       * data for the new picture, but ITB will not have a reference to the new picture.
       * So, the new picture's CDB data will incorrectly be treated as part of the
       * previous picture */
      BVCE_OUTPUT_P_ITB_UpdatePointers( hVceOutput );

      BDBG_MSG( ("Picture Index: %08x",
                  BREG_Read32(
                           hVceOutput->hVce->handles.hReg,
                           hVceOutput->hVce->stPlatformConfig.stDebug.uiCMEPictureIndex)
                  ));

      BDBG_MSG( ("ITB Base/End/sRead (Read)/Valid/Write = %08x/%08x/%08x (%08x)/%08x/%08x",
               hVceOutput->state.stITBBuffer.uiBaseOffset,
               hVceOutput->state.stITBBuffer.uiEndOffset,
               hVceOutput->state.stITBBuffer.uiShadowReadOffset,
               hVceOutput->state.stITBBuffer.uiReadOffset,
               hVceOutput->state.stITBBuffer.uiValidOffset,
               BREG_Read32(
                        hVceOutput->hVce->handles.hReg,
                        hVceOutput->hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stITB.uiWritePointer
                        )
               ));

      BDBG_MSG( ("CDB Base/End/sRead (Read)/Valid/Write = %08x/%08x/%08x (%08x)/%08x/%08x",
               uiCDBBaseOffset,
               uiCDBEndOffset,
               hVceOutput->state.uiCDBBufferShadowReadOffset,
               uiCDBReadOffset,
               uiCDBValidOffset,
               BREG_Read32(
                        hVceOutput->hVce->handles.hReg,
                        hVceOutput->hVce->stPlatformConfig.stOutput[hVceOutput->stOpenSettings.uiInstance].stCDB.uiWritePointer
                        )
               ));

      /* Update Output's Channel Cache */
      if ( ( NULL != hVceOutput->state.hVceCh )
           && ( BVCE_P_Status_eStarted == hVceOutput->state.hVceCh->eStatus )
         )
      {
         if ( false == hVceOutput->state.stChannelCache.bValid )
         {
            /* Start Encode Settings don't change, so only copy them once */
            hVceOutput->state.stChannelCache.stStartEncodeSettings = hVceOutput->state.hVceCh->stStartEncodeSettings;
         }
         hVceOutput->state.stChannelCache.stEncodeSettings = hVceOutput->state.hVceCh->stEncodeSettings;
         hVceOutput->state.stChannelCache.bValid = true;
      }

      /* Update ITB/CDB Cache */
      BVCE_Output_P_BufferCacheUpdate(
               hVceOutput,
               hVceOutput->state.stITBBuffer.uiValidOffset,
               hVceOutput->state.stITBBuffer.uiBaseOffset,
               hVceOutput->state.stITBBuffer.uiEndOffset,
               uiCDBValidOffset,
               uiCDBBaseOffset,
               uiCDBEndOffset
               );

      BVCE_OUTPUT_P_ITB_UpdateIndex( hVceOutput );

      while ( 1 )
      {
         /* Check for Available ITB Entries */
         {
#if BVCE_P_DUMP_OUTPUT_ITB_DESC
            uint32_t uiCDBDepth;
            uint32_t uiITBDepthActual;
            uint32_t uiCDBDepthActual;
            uint32_t uiSTC;
#endif

            BVCE_OUTPUT_P_ITB_Shadow_CalculateDepth( hVceOutput );

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
            {
               uiSTC = BREG_Read32(
                        hVceOutput->hVce->handles.hReg,
                        hVceOutput->hVce->stPlatformConfig.stDebug.uiSTC[hVceOutput->stOpenSettings.uiInstance]
                        );
            }

            if ( uiCDBValidOffset >= hVceOutput->state.uiCDBBufferShadowReadOffset )
            {
               uiCDBDepth = uiCDBValidOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
            }
            else
            {
               uiCDBDepth = uiCDBEndOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
               uiCDBDepth += uiCDBValidOffset - uiCDBBaseOffset;
            }

            if ( uiCDBValidOffset >= uiCDBReadOffset )
            {
               uiCDBDepthActual = uiCDBValidOffset - uiCDBReadOffset;
            }
            else
            {
               uiCDBDepthActual = uiCDBEndOffset - uiCDBReadOffset;
               uiCDBDepthActual += uiCDBValidOffset - uiCDBBaseOffset;
            }

            if ( hVceOutput->state.stITBBuffer.uiValidOffset >= hVceOutput->state.stITBBuffer.uiReadOffset )
            {
               uiITBDepthActual = hVceOutput->state.stITBBuffer.uiValidOffset - hVceOutput->state.stITBBuffer.uiReadOffset;
            }
            else
            {
               uiITBDepthActual = hVceOutput->state.stITBBuffer.uiEndOffset - hVceOutput->state.stITBBuffer.uiReadOffset;
               uiITBDepthActual += hVceOutput->state.stITBBuffer.uiValidOffset - hVceOutput->state.stITBBuffer.uiBaseOffset;
            }
#endif

            if ( 0 == BVCE_OUTPUT_P_ITB_Shadow_GetNumFrames( hVceOutput ) )
            {
               /* We ran out of ITB entries */
               BDBG_MSG( ("No more ITB Entries"));
               break;
            }

            pITBIndexEntry = BVCE_OUTPUT_P_ITB_Shadow_GetFrameIndexEntry( hVceOutput, 0 );

            /* SW7425-74: Data Unit Detection */
            if ( ( true == hVceOutput->stOpenSettings.bEnableDataUnitDetection )
                 && ( true == hVceOutput->state.stChannelCache.bValid )
               )
            {
               /* We only want to process new start codes if the all the previous data referenced by the
                * shadow valid offset has been queued. I.e. all the data associated with the previous data unit
                * has been muxed */
               if ( hVceOutput->state.uiCDBBufferShadowValidOffset == hVceOutput->state.uiCDBBufferShadowReadOffset )
               {
                  switch ( hVceOutput->state.stChannelCache.stStartEncodeSettings.stProtocolInfo.eProtocol )
                  {
                     case BAVC_VideoCompressionStd_eH264:
                     /* SW7425-518: add DU detection for MPEG 4 part 2 */
                     case BAVC_VideoCompressionStd_eMPEG4Part2:
                     {
                        if ( false == hVceOutput->state.bDataUnitDetected )
                        {
                           uint32_t uiDataUnitDetectionShiftRegister = hVceOutput->state.uiDataUnitDetectionShiftRegister;
                           uint32_t uiCDBBufferReadAheadValidOffset = hVceOutput->state.uiCDBBufferReadAheadValidOffset;
                           uint32_t uiCDBBufferShadowValidOffset = hVceOutput->state.uiCDBBufferShadowValidOffset;
                           uint8_t uiCDBBufferShadowValidOffsetDataUnitDetectionDelay = hVceOutput->state.uiCDBBufferShadowValidOffsetDataUnitDetectionDelay;
                           uint8_t *pCDBBufferCached =  (uint8_t*) hVceOutput->hOutputBuffers->pCDBBufferCached;
                           uint32_t uiStartCodeSize =
                                 (BAVC_VideoCompressionStd_eMPEG4Part2 == hVceOutput->state.stChannelCache.stStartEncodeSettings.stProtocolInfo.eProtocol)
                                    ?BVCE_P_MP4V_START_CODE_SIZE
                                    :BVCE_P_NALU_START_CODE_SIZE;
                          uint32_t uiDataUnitShiftRegisterMask =
                                 (BAVC_VideoCompressionStd_eMPEG4Part2 == hVceOutput->state.stChannelCache.stStartEncodeSettings.stProtocolInfo.eProtocol)
                                    ?BVCE_P_MP4V_START_CODE_MASK
                                    :BVCE_P_NALU_START_CODE_MASK;

                           while ( uiCDBBufferReadAheadValidOffset != uiCDBValidOffset )
                           {
                              uint8_t uiCurrentByte = pCDBBufferCached[(uiCDBBufferReadAheadValidOffset - uiCDBBaseOffset)];

                              if ( BVCE_P_IS_START_CODE(uiDataUnitDetectionShiftRegister) )
                              {
                                 hVceOutput->state.bDataUnitDetected = true;
                                 hVceOutput->state.uiDataUnitType = uiCurrentByte;
                              }

                              uiDataUnitDetectionShiftRegister <<= 8;
                              uiDataUnitDetectionShiftRegister |= uiCurrentByte;

                              /* check if shift register contains a partial start code ...
                                 (i.e. contains: XX XX XX 00, XX XX 00 00, or XX 00 00 01)  */
                              if ( ( 0x00 == uiCurrentByte )
                                   || (BVCE_P_IS_START_CODE(uiDataUnitDetectionShiftRegister))
                                 )
                              {
                                 /* if so, we need to hold off on generating a descriptor for these
                                  * bytes.  If the delay isn't already maxed out, then we increment
                                  * the delay.  Otherwise, we increment the shadow valid offset
                                  * directly.
                                  */
                                 if ( uiCDBBufferShadowValidOffsetDataUnitDetectionDelay < uiStartCodeSize )
                                 {
                                    uiCDBBufferShadowValidOffsetDataUnitDetectionDelay++;
                                 }
                                 else
                                 {
                                    uiCDBBufferShadowValidOffset++;
                                 }
                              }
                              else
                              {
                                 /* The shift register is not 1 and LSB not 0, so there isn't a
                                  * start code at all.  We increment the CDBShadowValidOffset
                                  * normally.  We also increment by whatever delay we had
                                  * accumulated */
                                 uiCDBBufferShadowValidOffset++;
                                 uiCDBBufferShadowValidOffset += uiCDBBufferShadowValidOffsetDataUnitDetectionDelay;
                                 uiCDBBufferShadowValidOffsetDataUnitDetectionDelay = 0;
                              }

                              uiCDBBufferReadAheadValidOffset++;
                              if ( uiCDBBufferReadAheadValidOffset >= uiCDBEndOffset )
                              {
                                 uiCDBBufferReadAheadValidOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
                              }

                              if ( BVCE_P_IS_START_CODE(uiDataUnitDetectionShiftRegister) )
                              {
                                 break;
                              }
                           }

                           if ( uiCDBBufferShadowValidOffset >= uiCDBEndOffset )
                           {
                              uiCDBBufferShadowValidOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
                           }

                           hVceOutput->state.uiDataUnitDetectionShiftRegister = uiDataUnitDetectionShiftRegister;
                           hVceOutput->state.uiCDBBufferReadAheadValidOffset = uiCDBBufferReadAheadValidOffset;
                           hVceOutput->state.uiCDBBufferShadowValidOffset = uiCDBBufferShadowValidOffset;
                           hVceOutput->state.uiCDBBufferShadowValidOffsetDataUnitDetectionDelay = uiCDBBufferShadowValidOffsetDataUnitDetectionDelay;
                        }
                     }
                        break;  /* end: Data-unit processing */

                     default:
                        /* Data Unit Detection Not Supported - Falling Through */
                        BVCE_Output_P_DataUnitDetectReset( hVceOutput, uiCDBValidOffset );
                        break;
                  }
               }
            }
            else
            {
               BVCE_Output_P_DataUnitDetectReset( hVceOutput, uiCDBValidOffset );
            }

            if ( ( false == hVceOutput->state.bEOSITBEntrySeen )
                 || ( ( true == hVceOutput->state.bEOSITBEntrySeen )
                       && ( true == hVceOutput->state.bEOSDescriptorSent )
                    )
               )
            {
               /* Check for Available CDB Data */
               if ( hVceOutput->state.uiCDBBufferShadowValidOffset == hVceOutput->state.uiCDBBufferShadowReadOffset )
               {
                  /* We ran out of CDB data */
                  BDBG_MSG( ("No more CDB Data"));
                  break;
               }
            }

            /* Check for Available Descriptors */
            {
               uint32_t uiTempWriteOffset = hVceOutput->state.uiDescriptorWriteOffset + 1;
               uiTempWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

               if (uiTempWriteOffset == hVceOutput->state.uiDescriptorReadOffset )
               {
                  /* We ran out of descriptors */
                  BDBG_MSG( ("No more descriptors"));
                  break;
               }

               /* Check to make sure we have another descriptor available for the metadata */
               if ( true == hVceOutput->state.bFrameStart )
               {
                  unsigned uiChunkId = ( ( pITBIndexEntry->uiMetadata >> 18 ) & 0x3FF );

                  if ( ( false == hVceOutput->state.bMetadataSent )
                       || ( uiChunkId != hVceOutput->state.uiMetadataPreviousChunkId ) ) /* SW7425-3360: Send metadata again if the chunk ID changed */
               {
                  uiTempWriteOffset++;
                  uiTempWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

                  if (uiTempWriteOffset == hVceOutput->state.uiDescriptorReadOffset )
                  {
                     /* We ran out of descriptors */
                     BDBG_MSG( ("No more descriptors"));
                     break;
                  }

                     /* SW7425-3360: Check to make sure we have enough metadata descriptors */
                     uiTempWriteOffset = hVceOutput->state.uiMetadataDescriptorWriteOffset + 1;
                     uiTempWriteOffset %= BVCE_P_MAX_METADATADESCRIPTORS;

                     if (uiTempWriteOffset == hVceOutput->state.uiMetadataDescriptorReadOffset )
                     {
                        /* We ran out of metadata descriptors */
                        BDBG_MSG( ("No more metadata descriptors"));
                        break;
                     }
                  }
               }
            }

            if ( true == hVceOutput->state.bFrameStart )
            {
               BDBG_MODULE_MSG( BVCE_OUTPUT_ITB, ("@%08x (%d), I=%d, d=%09llx (%d), p=%09llx, op=%08x, escr=%08x, tpb=%04x, shr=%6d, metadata=%08x",
                        pITBIndexEntry->uiCDBAddress,
                        pITBIndexEntry->bError,
                        (0 != ( pITBIndexEntry->stFrameDescriptor.uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP )),
                        pITBIndexEntry->stFrameDescriptor.uiDTS,
                        (0 != ( pITBIndexEntry->stFrameDescriptor.uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID )),
                        pITBIndexEntry->stFrameDescriptor.stCommon.uiPTS,
                        pITBIndexEntry->stFrameDescriptor.stCommon.uiOriginalPTS,
                        pITBIndexEntry->stFrameDescriptor.stCommon.uiESCR,
                        pITBIndexEntry->stFrameDescriptor.stCommon.uiTicksPerBit,
                        pITBIndexEntry->stFrameDescriptor.stCommon.iSHR,
                        pITBIndexEntry->uiMetadata
                        ));

#if BVCE_P_DUMP_OUTPUT_ITB_DESC
                  if ( NULL == hVceOutput->hITBDescDumpFile )
                  {
                     static unsigned uiInstance = 0;
                     char fname[256];
                     sprintf(fname, "BVCE_OUTPUT_DESC_%02d_%02d_%03d.csv", hVceOutput->state.hVceCh->hVce->stOpenSettings.uiInstance, hVceOutput->state.hVceCh->stOpenSettings.uiInstance, uiInstance);
                     uiInstance++;

                     if ( false == BVCE_Debug_P_OpenLog( fname, &hVceOutput->hITBDescDumpFile ) )
                     {
                        BDBG_ERR(("Error Creating BVCE Output ITB Desc Dump File (%s)", fname));
                     }
                     else
                     {
                        BVCE_Debug_P_WriteLog_isr(hVceOutput->hITBDescDumpFile, "address,dts(90Khz),dts valid,pts(90Khz),opts(45Khz),escr(27Mhz),tpb,shr,error,I frame,metadata,STC,ITB depth (shadow),ITB depth (actual),CDB depth (shadow),CDB depth (actual), CDB depth (hw), CDB write (hw), CDB valid (hw), CDB read (hw)\n");
                     }
                  }

                  if ( NULL != hVceOutput->hITBDescDumpFile )
                  {
                     BVCE_Debug_P_WriteLog_isr( hVceOutput->hITBDescDumpFile, "%u,%llu,%d,%llu,%u,%u,%u,%d,%d,%d,%08x,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
                              pITBIndexEntry->uiCDBAddress,
                              pITBIndexEntry->stFrameDescriptor.uiDTS,
                              (0 != ( pITBIndexEntry->stFrameDescriptor.uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID )),
                              pITBIndexEntry->stFrameDescriptor.stCommon.uiPTS,
                              pITBIndexEntry->stFrameDescriptor.stCommon.uiOriginalPTS,
                              pITBIndexEntry->stFrameDescriptor.stCommon.uiESCR,
                              pITBIndexEntry->stFrameDescriptor.stCommon.uiTicksPerBit,
                              pITBIndexEntry->stFrameDescriptor.stCommon.iSHR,
                              pITBIndexEntry->bError,
                              (0 != ( pITBIndexEntry->stFrameDescriptor.uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP )),
                              pITBIndexEntry->uiMetadata,
                              uiSTC,
                              hVceOutput->state.stITBBuffer.uiShadowDepth,
                              uiITBDepthActual,
                              uiCDBDepth,
                              uiCDBDepthActual,
                              uiCDBDepthHW,
                              uiCDBWriteOffsetHW,
                              uiCDBValidOffsetHW,
                              uiCDBReadOffsetHW
                              );
                  }
#endif
            }

            /* Figure out how much CDB data we have for the current picture */
            if ( BVCE_OUTPUT_P_ITB_Shadow_GetNumFrames( hVceOutput ) > 1 )
            {
               /* We have a next picture, so we know exactly how long the current picture is */
               uint32_t uiDepthToNext;
               uint32_t uiDepthToValid;
               pITBIndexEntryNext = BVCE_OUTPUT_P_ITB_Shadow_GetFrameIndexEntry( hVceOutput, 1 );

               if ( ( 0 != ( pITBIndexEntryNext->stFrameDescriptor.stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ) ) )
               {
                  hVceOutput->state.bEOSITBEntrySeen = true;
                  BDBG_MSG( ("[%d][%d] EOS ITB Seen", hVceOutput->hVce->stOpenSettings.uiInstance, hVceOutput->stOpenSettings.uiInstance));
                  if ( NULL != hVceOutput->state.hVceCh )
                  {
                     /* TODO: Clean this up.  We probably don't want BVCE_Output to set values in BVCE_Channel.
                      * Perhaps add a private API between the two. */
                     hVceOutput->state.hVceCh->eStatus = BVCE_P_Status_eOpened;

                     BVCE_Power_P_ReleaseResource(
                           hVceOutput->hVce,
                           BVCE_Power_Type_eClock
                           );

                     /* Detach Output Handle from Channel Handle */
                     hVceOutput->state.hVceCh = NULL;
                     /* TODO: Add EOS Callback */
                  }
               }

               BDBG_ASSERT( hVceOutput->hOutputBuffers->uiCDBBufferOffset <= pITBIndexEntryNext->uiCDBAddress );
               BDBG_ASSERT( ( hVceOutput->hOutputBuffers->uiCDBBufferOffset + hVceOutput->hOutputBuffers->stSettings.stConfig.Cdb.Length ) > pITBIndexEntryNext->uiCDBAddress );

               /* Goto next frame's ITB Entry */
               if ( pITBIndexEntryNext->uiCDBAddress == hVceOutput->state.uiCDBBufferShadowReadOffset )
               {
                  /* We have a next entry, and we've finished with the
                   * current entry, so move to the next entry
                   */
                  BVCE_OUTPUT_P_ITB_Shadow_ConsumeEntry( hVceOutput );
                  hVceOutput->state.bFrameStart = true;
                  BDBG_MSG( ("Goto Next Entry"));
                  continue;
               }

               /* It is possible that the CDB Valid doesn't, yet, contain any of the next picture and
                * may still be in the middle of the current picture, so we need use the depth that is the
                * lesser of depth(cdb_read,cdb_next) depth(cdb_read,cdb_valid)
                */
               if ( pITBIndexEntryNext->uiCDBAddress > hVceOutput->state.uiCDBBufferShadowReadOffset )
               {
                  uiDepthToNext = pITBIndexEntryNext->uiCDBAddress - hVceOutput->state.uiCDBBufferShadowReadOffset;
               }
               else
               {
                  uiDepthToNext = uiCDBEndOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
                  uiDepthToNext += pITBIndexEntryNext->uiCDBAddress - uiCDBBaseOffset;
               }

               if ( hVceOutput->state.uiCDBBufferShadowValidOffset > hVceOutput->state.uiCDBBufferShadowReadOffset )
               {
                  uiDepthToValid = hVceOutput->state.uiCDBBufferShadowValidOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
               }
               else
               {
                  uiDepthToValid = uiCDBEndOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
                  uiDepthToValid += hVceOutput->state.uiCDBBufferShadowValidOffset - uiCDBBaseOffset;
               }

               if ( uiDepthToValid < uiDepthToNext )
               {
                  uiCDBEndOfPictureOffset = hVceOutput->state.uiCDBBufferShadowValidOffset;
               }
               else
               {
                  uiCDBEndOfPictureOffset = pITBIndexEntryNext->uiCDBAddress;
               }
            }
            else
            {
               /* We don't have a next picture, so since the ITB entry for the next picture
                * is always written before the CDB data for the next picture, we know the
                * current CDB data is all for the current picture */
               pITBIndexEntryNext = NULL;
               uiCDBEndOfPictureOffset = hVceOutput->state.uiCDBBufferShadowValidOffset;
            }
         }

         if ( true == hVceOutput->state.bFrameStart )
         {
            unsigned uiChunkId = ( ( pITBIndexEntry->uiMetadata >> 18 ) & 0x3FF );

            if ( ( ( false == hVceOutput->state.bMetadataSent ) /* Always send metadata at least once */
                      || ( ( uiChunkId != hVceOutput->state.uiMetadataPreviousChunkId ) /* SW7425-3360: Send metadata again if the chunk ID changed */
                           && ( 0 == ( pITBIndexEntry->stFrameDescriptor.stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ) ) ) ) /* Not an EOS entry */
                 && ( true == hVceOutput->state.stChannelCache.bValid )
               )
            {
               BAVC_VideoMetadataDescriptor *pMetadataDescriptor = &hVceOutput->astMetadataDescriptors[hVceOutput->state.uiMetadataDescriptorWriteOffset];
               hVceOutput->state.uiMetadataDescriptorWriteOffset++;
               hVceOutput->state.uiMetadataDescriptorWriteOffset %= BVCE_P_MAX_METADATADESCRIPTORS;

               /* Get Video Descriptor for the metadata */
               pVideoDescriptor = &hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorWriteOffset];
               hVceOutput->state.uiDescriptorWriteOffset++;
               hVceOutput->state.uiDescriptorWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

               BKNI_Memset(
                        pVideoDescriptor,
                        0,
                        sizeof( BAVC_VideoBufferDescriptor )
                        );

               pVideoDescriptor->stCommon.uiOffset = (unsigned) pMetadataDescriptor - (unsigned)&hVceOutput->astMetadataDescriptors[0];
               pVideoDescriptor->stCommon.uiLength = sizeof( BAVC_VideoMetadataDescriptor );
               pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA;
               pVideoDescriptor->uiDataUnitType = BAVC_VideoMetadataType_eCommon;

               /* Populate metadata */
               pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID;
               pMetadataDescriptor->stBitrate.uiMax = hVceOutput->state.stChannelCache.stEncodeSettings.stBitRate.uiMax;

               /* Populate Frame Rate */
               {
                  uint8_t uiFrameRate = ( ( pITBIndexEntry->uiMetadata >> 14 ) & 0x0F );

                  if ( uiFrameRate < BVCE_P_EncoderFrameRate_eMax )
                  {
                     pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_FRAMERATE_VALID;
                     pMetadataDescriptor->stFrameRate.eFrameRateCode = BVCE_P_FrameRateReverseLUT[uiFrameRate];
                  }

                  /* Populate Coded Dimension */
                  pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_DIMENSION_CODED_VALID;
                  pMetadataDescriptor->stDimension.coded.uiHeight = ( ( pITBIndexEntry->uiMetadata >> 0 ) & 0x7F ) << 4;
                  pMetadataDescriptor->stDimension.coded.uiWidth = ( ( pITBIndexEntry->uiMetadata >> 7 ) & 0x7F ) << 4;
               }

               if ( true == hVceOutput->state.stChannelCache.bValid )
               {
                  /* Populate Buffer Info */
                  pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_BUFFERINFO_VALID;
                  pMetadataDescriptor->stBufferInfo = hVceOutput->state.stChannelCache.stStartEncodeSettings.stProtocolInfo;
               }

               switch ( hVceOutput->state.stChannelCache.stStartEncodeSettings.stProtocolInfo.eProtocol )
               {
                  case BAVC_VideoCompressionStd_eVC1SimpleMain:
                     /* Populate VC1 Info */
                     pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID;

                     /* Sequence Header B */
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderB.bCBR = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderB.uiHRDBuffer = ( ( pITBIndexEntry->uiMetadata >> 22 ) & 0x3FF ) << 17;

                     /* Sequence Header C */
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.uiQuantizedFrameRatePostProcessing = 0; /* TODO: */
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.uiQuantizedBitratePostProcessing = 0; /* TODO: */
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bLoopFilter = true;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bMultiResolution = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bFastUVMotionCompensation = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bExtendedMotionVector = true;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.uiMacroblockQuantization = ( ( pITBIndexEntry->uiMetadata >> 18 ) & 0x03 );
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bVariableSizedTransform = true;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bOverlappedTransform = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bSyncMarker = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bRangeReduction = false;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.uiMaxBFrames = 0;
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.uiQuantizer = ( ( pITBIndexEntry->uiMetadata >> 20 ) & 0x03 );
                     pMetadataDescriptor->uProtocolData.stVC1.sequenceHeaderC.bFrameInterpolation = false;
                     break;

                  default:
                     /* No Protocol Data */
                     break;
               }

               /* Populate STC Snapshot */
               {
                  if ( BERR_SUCCESS == getStatusRC )
                  {
                     pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_STC_VALID;

                     pMetadataDescriptor->stTiming.uiSTCSnapshot = stChannelStatus.uiSTCSnapshot;
                  }
               }

                  /* SW7425-3360: Populate Chunk ID */
                  pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_CHUNK_ID_VALID;
                  pMetadataDescriptor->stTiming.uiChunkId = uiChunkId;
                  hVceOutput->state.uiMetadataPreviousChunkId = pMetadataDescriptor->stTiming.uiChunkId;

               hVceOutput->state.bMetadataSent = true;
            }
         }

         /* Get Video Descriptor for this ITB entry */
         pVideoDescriptor = &hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorWriteOffset];
         hVceOutput->state.uiDescriptorWriteOffset++;
         hVceOutput->state.uiDescriptorWriteOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

         if ( ( true == hVceOutput->state.bFrameStart )
              && ( hVceOutput->state.uiCDBBufferShadowReadOffset == pITBIndexEntry->uiCDBAddress )
            )
         {
            *pVideoDescriptor = pITBIndexEntry->stFrameDescriptor;
            hVceOutput->state.bFrameStart = false;
         }
         else
         {
            BKNI_Memset( pVideoDescriptor, 0, sizeof( BAVC_VideoBufferDescriptor ) );
         }

         if ( 0 != ( pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ) )
         {
            hVceOutput->state.bEOSDescriptorSent = true;
            BDBG_MSG( ("EOS Video Descriptor Sent"));
         }
         else
         {
            /* Populate LLDInfo for PES */
            if ( uiCDBEndOfPictureOffset > hVceOutput->state.uiCDBBufferShadowReadOffset )
            {
               pVideoDescriptor->stCommon.uiLength = uiCDBEndOfPictureOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
            }
            else
            {
               /* CDB Wrap occurs, so we need to split this picture into two descriptors.  We handle the first one here. */
               pVideoDescriptor->stCommon.uiLength = uiCDBEndOffset - hVceOutput->state.uiCDBBufferShadowReadOffset;
            }

            if ( true == hVceOutput->state.bDataUnitDetected )
            {
               pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START;
               pVideoDescriptor->uiDataUnitType = hVceOutput->state.uiDataUnitType;
               hVceOutput->state.bDataUnitDetected = false;
            }

            /* Normalize the offset to 0 */
            pVideoDescriptor->stCommon.uiOffset = hVceOutput->state.uiCDBBufferShadowReadOffset - uiCDBBaseOffset;

            hVceOutput->state.uiCDBBufferShadowReadOffset += pVideoDescriptor->stCommon.uiLength;
            if ( hVceOutput->state.uiCDBBufferShadowReadOffset >= uiCDBEndOffset )
            {
               hVceOutput->state.uiCDBBufferShadowReadOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
            }
         }

#if BVCE_P_TEST_MODE
   if ( NULL == hVceOutput->hDescriptorLog )
   {
      char fname[256];
      sprintf(fname, "BVCE_BUFFER_DESC_%02d_%02d_%03d.csv", hVceOutput->state.hVceCh->hVce->stOpenSettings.uiInstance, hVceOutput->state.hVceCh->stOpenSettings.uiInstance, hVceOutput->uiDescriptorLogInstance);
      hVceOutput->uiDescriptorLogInstance++;

      if ( false == BVCE_Debug_P_OpenLog( fname, &hVceOutput->hDescriptorLog ) )
      {
         BDBG_ERR(("Error Creating BVCE Output Buffer Desc Dump File (%s)", fname));
      }
      else
      {
         BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, "count,flags,metadata,extended,frame start,eos,empty frame,offset,length,opts valid,opts,pts valid,pts,escr valid,escr,tpb valid,tpb, shr valid, shr, stc valid, stc" );
         BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, ",vflags,dts valid,dts,dut start,dut,rai" );
         BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, "\n" );
      }
   }

   if ( NULL != hVceOutput->hDescriptorLog )
   {
      /* Common */
      BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, "%u,0x%08x,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%llu,%u,%u,%u,%u,%u,%d,%u,%llu",
         hVceOutput->uiDescriptorCount++,
         pVideoDescriptor->stCommon.uiFlags,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA)),
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EXTENDED)),
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START)),
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS)),
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME)),
         pVideoDescriptor->stCommon.uiOffset,
         pVideoDescriptor->stCommon.uiLength,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID)),
         pVideoDescriptor->stCommon.uiOriginalPTS,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID)),
         pVideoDescriptor->stCommon.uiPTS,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID)),
         pVideoDescriptor->stCommon.uiESCR,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID)),
         pVideoDescriptor->stCommon.uiTicksPerBit,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID)),
         pVideoDescriptor->stCommon.iSHR,
         (0 != (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_STCSNAPSHOT_VALID)),
         pVideoDescriptor->stCommon.uiSTCSnapshot
         );

      BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, ",0x%08x,%u,%llu,%u,%u,%u",
         pVideoDescriptor->uiVideoFlags,
         (0 != (pVideoDescriptor->uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID)),
         pVideoDescriptor->uiDTS,
         (0 != (pVideoDescriptor->uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START)),
         pVideoDescriptor->uiDataUnitType,
         (0 != (pVideoDescriptor->uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP))
         );

      BVCE_Debug_P_WriteLog_isr( hVceOutput->hDescriptorLog, "\n" );
   }
#endif

         BDBG_MODULE_MSG( BVCE_OUTPUT_DESC, ("@%08x (%08x), f=%08x, p=%09llx, op=%08x, escr=%08x, tpb=%04x, shr=%6d, vf=%08x, d=%09llx du=%08x",
                  pVideoDescriptor->stCommon.uiOffset,
                  pVideoDescriptor->stCommon.uiLength,
                  pVideoDescriptor->stCommon.uiFlags,
                  pVideoDescriptor->stCommon.uiPTS,
                  pVideoDescriptor->stCommon.uiOriginalPTS,
                  pVideoDescriptor->stCommon.uiESCR,
                  pVideoDescriptor->stCommon.uiTicksPerBit,
                  pVideoDescriptor->stCommon.iSHR,
                  pVideoDescriptor->uiVideoFlags,
                  pVideoDescriptor->uiDTS,
                  pVideoDescriptor->uiDataUnitType
                  ));
      }

      /* Assign array(s) and count(s) */
      if ( hVceOutput->state.uiDescriptorWriteOffset >= hVceOutput->state.uiDescriptorReadOffset )
      {
         *astDescriptors0 = &hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset];
         *puiNumDescriptors0 = hVceOutput->state.uiDescriptorWriteOffset - hVceOutput->state.uiDescriptorReadOffset;

         *astDescriptors1 = NULL;
         *puiNumDescriptors1 = 0;
      }
      else
      {
         *astDescriptors0 = &hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset];
         *puiNumDescriptors0 = hVceOutput->stOpenSettings.uiDescriptorQueueDepth - hVceOutput->state.uiDescriptorReadOffset;

         *astDescriptors1 = &hVceOutput->astDescriptors[0];
         *puiNumDescriptors1 = hVceOutput->state.uiDescriptorWriteOffset;
      }
   }

   hVceOutput->state.uiPendingDescriptors = *puiNumDescriptors0 + *puiNumDescriptors1;
   hVceOutput->state.uiConsumedDescriptors = 0;

   BVCE_Power_P_ReleaseResource(
         hVceOutput->hVce,
         BVCE_Power_Type_eClock
         );

   BKNI_ReleaseMutex( hVceOutput->hMutex );

   BDBG_LEAVE( BVCE_Output_GetBufferDescriptors );
   return BERR_TRACE( BERR_SUCCESS );
}

/* BVCE_Output_ConsumeBufferDescriptors -
 * Reclaims the specified number of video buffer descriptors
 * The CDB read pointer is updated accordingly
 */
BERR_Code
BVCE_Output_ConsumeBufferDescriptors(
   BVCE_Output_Handle hVceOutput,
   size_t uiNumBufferDescriptors
   )
{
   uint32_t uiCDBReadOffset;
   uint32_t uiCDBEndOffset;
   uint32_t uiCDBBaseOffset;

   BDBG_ENTER( BVCE_Output_ConsumeBufferDescriptors );

   if ( hVceOutput->state.uiConsumedDescriptors != hVceOutput->state.uiPendingDescriptors )
   {
      BVCE_Power_P_AcquireResource(
            hVceOutput->hVce,
            BVCE_Power_Type_eClock
            );

      uiCDBReadOffset = BREG_Read32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->stRegisters.CDB_Read
               );

      uiCDBEndOffset = BREG_Read32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->stRegisters.CDB_End
               );

      uiCDBEndOffset++;

      uiCDBBaseOffset = BREG_Read32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->stRegisters.CDB_Base
               );

      hVceOutput->state.uiConsumedDescriptors += uiNumBufferDescriptors;

      while ( uiNumBufferDescriptors )
      {
         if ( 0 != ( hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
         {
            /* SW7425-3360: We have could have multiple metadata descriptors, so increment the metadata descriptor read offset */
            hVceOutput->state.uiMetadataDescriptorReadOffset++;
            hVceOutput->state.uiMetadataDescriptorReadOffset %= BVCE_P_MAX_METADATADESCRIPTORS;
         }
         else
         {
            /* Move CDB Read Offset */
            uiCDBReadOffset += hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset].stCommon.uiLength;
            /* 7425A0: Hack to prevent HW/SW confusion of full vs empty */
            if ( false == hVceOutput->state.bCDBReadHackDone )
            {
               uiCDBReadOffset--;
               hVceOutput->state.bCDBReadHackDone = true;
            }
            if ( uiCDBReadOffset >= uiCDBEndOffset )
            {
               uiCDBReadOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
            }

            if ( ( 0 != ( hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START ))
                 || ( 0 != ( hVceOutput->astDescriptors[hVceOutput->state.uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ))
               )
            {
               /* Move ITB Read Offset */
               BVCE_OUTPUT_P_ITB_ConsumeEntry( hVceOutput );
            }
         }

         /* Move Descriptor Read Offset */
         hVceOutput->state.uiDescriptorReadOffset++;
         hVceOutput->state.uiDescriptorReadOffset %= hVceOutput->stOpenSettings.uiDescriptorQueueDepth;

         uiNumBufferDescriptors--;
      }

      /* Update Actual ITB/CDB Read Pointers */
      BREG_Write32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->stRegisters.CDB_Read,
               uiCDBReadOffset
               );

      BREG_Write32(
               hVceOutput->hVce->handles.hReg,
               hVceOutput->stRegisters.ITB_Read,
               hVceOutput->state.stITBBuffer.uiReadOffset
               );

      BVCE_Power_P_ReleaseResource(
            hVceOutput->hVce,
            BVCE_Power_Type_eClock
            );
   }
   else
   {
      if ( 0 != uiNumBufferDescriptors )
      {
         BDBG_WRN(("BVCE_Output_ConsumeBufferDescriptors called but there aren't any pending descriptors!"));
      }
   }

   BDBG_LEAVE( BVCE_Output_ConsumeBufferDescriptors );
   return BERR_TRACE( BERR_SUCCESS );
}

/* BVCE_Output_GetBufferStatus -
 * Returns the output buffer status (e.g. the base virtual address)
 */
BERR_Code
BVCE_Output_GetBufferStatus(
   BVCE_Output_Handle hVceOutput,
   BAVC_VideoBufferStatus *pBufferStatus
   )
{
   BDBG_ENTER( BVCE_Output_GetBufferStatus );

   BDBG_ASSERT( pBufferStatus );

   BKNI_Memset(
            pBufferStatus,
            0,
            sizeof( BAVC_VideoBufferStatus )
            );

   pBufferStatus->stCommon.pFrameBufferBaseAddress = hVceOutput->hOutputBuffers->pCDBBufferCached;
   pBufferStatus->stCommon.pMetadataBufferBaseAddress = hVceOutput->astMetadataDescriptors;

   BDBG_LEAVE( BVCE_Output_GetBufferStatus );
   return BERR_TRACE( BERR_SUCCESS );
}
