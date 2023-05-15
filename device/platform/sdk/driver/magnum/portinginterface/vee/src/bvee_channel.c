/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvee_channel.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 9/13/12 2:46p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vee/src/bvee_channel.c $
 * 
 * Hydra_Software_Devel/23   9/13/12 2:46p venkatr
 * SW7346-1017 : [7346] CDB read pointer was getting updated when consume
 * buffer descriptor is called, even though the descriptor is related to
 * meta data
 * 
 * Hydra_Software_Devel/22   6/26/12 6:38p venkatr
 * SW7231-860 : [7231] Added code to detect access unit delimiter only
 * 
 * Hydra_Software_Devel/21   6/13/12 9:44a venkatr
 * SW7344-336 : [7344] Raaga Transcode support 30/29.97 and 15/14.98 frame
 * rates. Added 15 fps enum in LUT.
 * 
 * Hydra_Software_Devel/20   5/9/12 9:39a venkatr
 * SW7429-143 : [7429] Removed the element for 15fps in LUT,as it is not
 * supported in nexus code
 * 
 * Hydra_Software_Devel/19   5/4/12 7:07p venkatr
 * SW7429-143 : [7429] Added code to allocate reference buffer for HD
 * resolution
 * 
 * Hydra_Software_Devel/18   4/26/12 3:33p venkatr
 * SW7346-569 : [7346] Removing C bitfields for ITB entries.
 * 
 * Hydra_Software_Devel/17   3/21/12 2:55p venkatr
 * SW7231-569 : [7231] Modified the framerate LUT array length
 * 
 * Hydra_Software_Devel/16   3/21/12 2:40p venkatr
 * SW7231-569 : [7231] Adding FrameRate 7.493 and 14.985 into enum
 * 
 * Hydra_Software_Devel/15   3/15/12 2:42p venkatr
 * SW7231-585: [7231] Adding the code to populate IFrame entry and DTS
 * flags in VEE buffer descriptors
 * 
 * Hydra_Software_Devel/14   3/15/12 10:50a venkatr
 * SW7231-585: [7231] Get STC index from app and pass on the STC reg
 * address to DSP
 * 
 * Hydra_Software_Devel/13   3/7/12 10:20a venkatr
 * SW7231-585:[7231] Added the API's to support external interrupts to
 * interrupt dsp.
 * 
 * Hydra_Software_Devel/12   2/7/12 5:06p venkatr
 * SW7231-629 : [7231] Pass the ui32End2EndDelay from APP to BDSP
 * 
 * Hydra_Software_Devel/11   1/18/12 4:29p venkatr
 * SW7346-515:[7346] VEE PI code, code addition to get and set data sync
 * setting, inorder to get STC address to do a TSM based picture drop in
 * case of running in non real time mode
 * 
 * Hydra_Software_Devel/10   11/23/11 4:33p venkatr
 * SW7346-515:[7346] VEE PI code modifying memory alloc alignment to
 * 32bits
 * 
 * Hydra_Software_Devel/9   11/22/11 5:17p venkatr
 * SW7346-515:[7346] VEE PI code modification to get STC timestamp from
 * application
 * 
 * Hydra_Software_Devel/8   11/14/11 3:35p venkatr
 * SW7346-515:[7346] VEE PI code modification to retrieve picture buffer
 * address only rather than picture descriptor
 * 
 * Hydra_Software_Devel/7   11/11/11 3:27p venkatr
 * SW7346-515:[7346] VEE PI code code modifying VEE enums to AVC type
 * 
 * Hydra_Software_Devel/6   11/10/11 3:36p venkatr
 * SW7346-515:[7346] VEE PI code code cleanup
 * 
 * Hydra_Software_Devel/5   11/9/11 5:13p venkatr
 * SW7346-515:[7346] VEE PI code fix for back guard violation
 * 
 * Hydra_Software_Devel/4   11/8/11 3:52p venkatr
 * SW7346-515:[7346] VEE PI code for allocating reference buffers, and
 * modified for setting task user config
 * 
 * Hydra_Software_Devel/3   11/4/11 6:56p venkatr
 * SW7346-515:[7346] Removing malloc from isr context, hence static
 * allocation for picture parameter buffer during open
 * 
 * Hydra_Software_Devel/2   10/27/11 2:01p venkatr
 * SW7346-515:[7346] Removing surface handle from structure
 * BVEE_PictureDescriptor and adding the required parameter
 * 
 * Hydra_Software_Devel/1   10/19/11 8:29p venkatr
 * SW7346-515 : [7346] VEE PI code initial check in
 * 
 *
 ***************************************************************************/
 #include "bstd.h"
 #include "bkni.h"
 #include "bvee.h"
 #include "bvee_channel.h"
 #include "bvee_priv.h"
 #include "bdsp_raaga.h"
 #include "bvee_dsp_utils_priv.h"
 #include "bdsp_video_encode_task.h"
 
 BDBG_MODULE(bvee_channel);

 BDBG_OBJECT_ID(BVEE_Channel);

 #define DSP_INDEX 0
#define BVEE_DATA_UNIT_START_CODE      0x00000001
 static const  BAVC_VideoCompressionStd  g_codeclist [] =
 {
    BAVC_VideoCompressionStd_eH264
 };
 static const uint32_t BVEE_P_FrameRateLUT[BDSP_VF_P_EncodeFrameRate_eMax] =
 {
  BDSP_VF_P_EncodeFrameRate_eUnknown,  /* Unknown */
  BDSP_VF_P_EncodeFrameRate_e23_97,  /* 23.97 */
  BDSP_VF_P_EncodeFrameRate_e24,     /* 24 */
  BDSP_VF_P_EncodeFrameRate_e25,     /* 25 */
  BDSP_VF_P_EncodeFrameRate_e29_97,  /* 29.97 */
  BDSP_VF_P_EncodeFrameRate_e30,     /* 30 */
  BDSP_VF_P_EncodeFrameRate_e50,     /* 50 */
  BDSP_VF_P_EncodeFrameRate_e59_94,  /* 59.94 */
  BDSP_VF_P_EncodeFrameRate_e60,     /* 60 */
  BDSP_VF_P_EncodeFrameRate_e14_985, /* 14.985 */
  BDSP_VF_P_EncodeFrameRate_e7_493,  /* 7.493 */
  BDSP_VF_P_EncodeFrameRate_e10,     /* 10 */
  BDSP_VF_P_EncodeFrameRate_e15
 };
 void BVEE_Channel_GetDefaultOpenSettings(
    BVEE_ChannelOpenSettings *pSettings
    )
 {
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->maxQueuedPictures = BVEE_MAX_PICTURE_BUFFERS;
    pSettings->codec = BAVC_VideoCompressionStd_eH264;
    
    pSettings->resolution.width = BVEE_DEFAULT_PICT_WIDTH;
    pSettings->resolution.height = BVEE_DEFAULT_PICT_HEIGHT;
    pSettings->bufferHeap = NULL;
    pSettings->enableExternalTrigger = false;
    
    return;
 }
 static uint8_t BVEE_Channel_P_GetNextItbEntryType(BVEE_ChannelHandle handle, uint32_t readOffset)
 {
    void *pSource;
    BVEE_P_ITBEntry *pEntry;

    BMEM_Heap_ConvertOffsetToAddress((handle->opensettings.bufferHeap!= NULL)?
                                      handle->opensettings.bufferHeap:
                                      handle->devicehandle->memHandle, 
                                     readOffset, &pSource);
    
    pEntry = pSource;
    return BVEE_ITB_GET_FIELD(&pEntry->fields.baseAddress,GENERIC,ENTRY_TYPE);
 }
 static size_t BVEE_Channel_P_ReadItb(BVEE_ChannelHandle handle, uint32_t baseOffset, 
                        uint32_t readOffset, uint32_t endOffset, 
                        uint32_t depth, uint8_t *pDest, size_t length)
 {
    void *pSource;
    BMEM_Handle memHandle = NULL;

    if (handle->opensettings.bufferHeap!= NULL)
    {
        memHandle = handle->opensettings.bufferHeap;
    }
    else
    {
        memHandle = handle->devicehandle->memHandle;
    }

    if ( length <= depth )
    {
        BMEM_Heap_ConvertOffsetToAddress(memHandle, readOffset, &pSource);
        if ( readOffset + length > endOffset )
        {
            size_t preWrapAmount = endOffset-readOffset;
            size_t wrapAmount = length - preWrapAmount;
            /* Wraparound */
            BKNI_Memcpy(pDest, pSource, preWrapAmount);
            BMEM_Heap_ConvertOffsetToAddress(memHandle, baseOffset, &pSource);
            BKNI_Memcpy(pDest+preWrapAmount, pSource, wrapAmount);
        }
        else
        {
            /* No Wrap */
            BKNI_Memcpy(pDest, pSource, length);
        }
        
        return length;
    }
    else
    {
        return 0;
    }
}
 void BVEE_Channel_P_ParseItb(BVEE_ChannelHandle handle, BVEE_P_ITBEntry **pCurrent, BVEE_P_ITBEntry **pNext)
 {
    
    uint32_t uiITBDepth;
    uint32_t uiITBBaseOffset;
    uint32_t uiITBEndOffset;
    uint32_t uiITBValidOffset;
    uint32_t uiITBReadOffset;
    uint32_t uiShadowReadOffset;
    uint32_t uiNextEntryOffset;
    BVEE_ChannelOutputDescriptorInfo *psOutputDescDetails = &handle->veeoutput;
    BVEE_P_ITBEntry *pITBEntry, *pITBEntryNext;
    size_t uiAmountRead;
    uint8_t entryType;

    *pCurrent = NULL;
    *pNext = NULL;

     /* ITB Pointers */
     uiITBBaseOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.ITB_Base);
     uiITBReadOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.ITB_Read);
     uiITBEndOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.ITB_End);
     if ( uiITBEndOffset != 0 )
     {
         uiITBEndOffset += 1; /* end is inclusive */
     }
     uiITBValidOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.ITB_Valid);
     if ( uiITBValidOffset >= uiITBEndOffset )
     {
        uiITBValidOffset = uiITBBaseOffset + ( uiITBValidOffset - uiITBEndOffset );
     }

     uiShadowReadOffset = psOutputDescDetails->uiITBBufferShadowReadOffset;
     for ( ;; )
     {
         pITBEntry = NULL;
         pITBEntryNext = NULL;
 
         if ( uiITBValidOffset >= uiShadowReadOffset )
         {
             uiITBDepth = uiITBValidOffset - \
                          uiShadowReadOffset;
         }
         else
         {
             uiITBDepth = uiITBEndOffset - uiShadowReadOffset;
             uiITBDepth += uiITBValidOffset - uiITBBaseOffset;
         }
 
         BDBG_MSG(("ITB Depth: %d bytes (Valid: %08x, Shadow Read: %08x)",
                   uiITBDepth,
                   uiITBValidOffset,
                   uiShadowReadOffset
                  ));
 
         /* Check for odd ITB entries and drop them */
         entryType = BVEE_Channel_P_GetNextItbEntryType(handle, uiShadowReadOffset);
         if ( entryType != BVEE_ITB_ENTRY_TYPE_BASE_ADDRESS )
         {
             /* this should never happen. We should be in sync looking for this entry drops should happen looking for the next one */
             BDBG_WRN(("Dropping ITB Entry type 0x%02x looking for first", entryType));
             uiShadowReadOffset += 16;
             if ( uiShadowReadOffset >= uiITBEndOffset )
             {
                 uiShadowReadOffset -= (uiITBEndOffset-uiITBBaseOffset);
             }
             continue;
         }
         uiAmountRead = BVEE_Channel_P_ReadItb(handle, uiITBBaseOffset, uiShadowReadOffset, uiITBEndOffset, uiITBDepth, 
                                                 (uint8_t *)&psOutputDescDetails->itb.current, sizeof(BVEE_P_ITBEntry));
         if ( 0 == uiAmountRead )
         {
             /* We ran out of ITB entries */
             BDBG_MSG(("No more ITB Entries"));
             psOutputDescDetails->uiITBBufferShadowReadOffset = uiShadowReadOffset;
             return;
         }
     
         uiITBDepth -= uiAmountRead;
         pITBEntry = &psOutputDescDetails->itb.current;
        
         
         uiNextEntryOffset = uiShadowReadOffset + sizeof(BVEE_P_ITBEntry);
         if ( uiNextEntryOffset >= uiITBEndOffset )
         {
             uiNextEntryOffset -= (uiITBEndOffset-uiITBBaseOffset);
         }
 
         while ( uiITBDepth >= sizeof(BVEE_P_ITBEntry) && NULL == pITBEntryNext )
         {
             /* Check for odd ITB entries and drop them */
             entryType = BVEE_Channel_P_GetNextItbEntryType(handle, uiNextEntryOffset);
             if ( entryType != BVEE_ITB_ENTRY_TYPE_BASE_ADDRESS )
             {
                 uiNextEntryOffset += 16;
                 if ( uiNextEntryOffset >= uiITBEndOffset )
                 {
                     uiNextEntryOffset -= (uiITBEndOffset-uiITBBaseOffset);
                 }
                 uiITBDepth -= 16;
                 continue;
             }
             
             /* Found a base address entry.  Read the next entry. */
             uiAmountRead = BVEE_Channel_P_ReadItb(handle, uiITBBaseOffset, uiNextEntryOffset, uiITBEndOffset, uiITBDepth, 
                                                     (uint8_t *)&psOutputDescDetails->itb.next, sizeof(BVEE_P_ITBEntry));
             if ( 0 == uiAmountRead )
             {
                 /* We ran out of ITB entries */
                 BDBG_MSG(("Next ITB entry unavailable"));
                 return;
             }
 
             uiITBDepth -= uiAmountRead;
             pITBEntryNext = &psOutputDescDetails->itb.next;
         }
 
         /* Figure out how much CDB data we have for the current Frame */
         if ( NULL != pITBEntryNext )
         {    
             /* Goto next frame's ITB Entry */
             if ( BVEE_ITB_GET_FIELD(&pITBEntryNext->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS) == 
                  psOutputDescDetails->uiCDBBufferShadowReadOffset )
             {
                 /* We have a next entry, and we've finished with the
                  * current entry, so move to the next entry
                  */                
                 uiShadowReadOffset = uiNextEntryOffset;
                 psOutputDescDetails->uiITBBufferShadowReadOffset = uiNextEntryOffset;
                 BDBG_MSG(("Goto Next Entry"));
                 continue;
             }
     
             *pCurrent = pITBEntry;
             *pNext = pITBEntryNext;
             return;
         }
         else
         {
             /* We ran out of ITB entries */
             BDBG_MSG(("Next ITB entry unavailable"));
             return;
         }
    }
 }
 BERR_Code BVEE_Channel_P_AllocatePictParamBuffer(BVEE_ChannelHandle handle)
 {
    BERR_Code errCode;
    unsigned int i=0;
    void *pAddress;
    BMEM_Handle memHandle = NULL;
    uint32_t uiTempSize=0;

    if (handle->opensettings.bufferHeap!= NULL)
    {
        BDBG_MSG(("%s Frame buffer from %#x (Opensettings)",__FUNCTION__, handle->opensettings.bufferHeap));
        memHandle = handle->opensettings.bufferHeap;
    }
    else
    {
        BDBG_MSG(("%s Frame buffer from default", __FUNCTION__));
        memHandle = handle->devicehandle->memHandle;
    }
    handle->capturepicture = BKNI_Malloc(sizeof(BVEE_CapBufferMemory)* handle->opensettings.maxQueuedPictures);
    for(i=0;i<handle->opensettings.maxQueuedPictures;i++)
    {        
       handle->capturepicture[i].pPpBuffer = (BVENC_VF_sPicParamBuff *)BMEM_AllocAligned(
                            memHandle,sizeof(BVENC_VF_sPicParamBuff), 5, 0);
       if((void *) 0xFFFFFFFF == (void *)handle->capturepicture[i].pPpBuffer)
       {
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
       }
       handle->capturepicture[i].bValid = false;
       handle->capturepicture[i].pPDesc = NULL;
    }
    if((handle->opensettings.resolution.height>480)&&(handle->opensettings.resolution.width>640))
    {
        /* reference buffer size for HD resolution */
        uiTempSize = (BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE_HD+BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE_HD)
        				*BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;
        BDBG_ERR(("Allocating HD buffer size %d",uiTempSize));
    }
    else
    {
        /* reference buffer size */
        uiTempSize = (BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE+BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE)
        				*BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;
        BDBG_ERR(("Allocating SD buffer size %d",uiTempSize));
    }
    
    /* Video FW requires the buffers to be 256 byte alligned for DMA */
    handle->ui32RefFrameBaseAddr = (uint32_t) BMEM_AllocAligned(memHandle,uiTempSize, 8, 0);
    if((void *) 0xFFFFFFFF == (void *)handle->ui32RefFrameBaseAddr)
    {
    	BDBG_ERR(("%s: Unable to Allocate memory for Video encoder reference buffer!",__FUNCTION__));
    	return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    /* Reset shadow pointers */
    handle->veeoutput.uiDescriptorReadOffset = 0;
    handle->veeoutput.uiDescriptorWriteOffset = 0;
    handle->veeoutput.uiMetadataDescriptorReadOffset = 0;
    handle->veeoutput.uiMetadataDescriptorWriteOffset = 0;

    /* Allocate Video buffer descriptors */
    handle->veeoutput.pstDescriptorsUncached = (BAVC_VideoBufferDescriptor*)BMEM_AllocAligned(memHandle,sizeof(BAVC_VideoBufferDescriptor)*BVEE_MAX_VIDEODESCRIPTORS, 5,0);
    if ( NULL == handle->veeoutput.pstDescriptorsUncached ) 
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_alloc_descriptors;
    }
    errCode = BMEM_Heap_ConvertAddressToCached(memHandle, handle->veeoutput.pstDescriptorsUncached, &pAddress);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cache_descriptors;
    }    
    handle->veeoutput.pstDescriptorsCached = pAddress;
    
    /* Allocate Video Metadata descriptors */
    handle->veeoutput.pstMetadataUncached = (BAVC_VideoMetadataDescriptor*)BMEM_AllocAligned(memHandle, sizeof(BAVC_VideoMetadataDescriptor)*BVEE_MAX_METADATADESCRIPTORS, 5,0);
    if ( NULL == handle->veeoutput.pstMetadataUncached ) 
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_alloc_metadata;
    }    
    errCode = BMEM_Heap_ConvertAddressToCached(memHandle, handle->veeoutput.pstMetadataUncached, &pAddress);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cache_metadata;
    }
    handle->veeoutput.pstMetadataCached = pAddress;

    return BERR_SUCCESS;
    
err_cache_metadata:
    BMEM_Heap_Free(memHandle, handle->veeoutput.pstMetadataUncached);
err_alloc_metadata:
err_cache_descriptors:
    BMEM_Heap_Free(memHandle, handle->veeoutput.pstDescriptorsUncached);
err_alloc_descriptors:

    return errCode;
 }

 BERR_Code BVEE_Channel_P_DeAllocatePictParamBuffer(BVEE_ChannelHandle handle)
 { 
    BERR_Code errcode;
    unsigned int i=0;
    BMEM_Handle memHandle = NULL;

    if (handle->opensettings.bufferHeap!= NULL)
    {
        BDBG_MSG(("%s Frame buffer from %#x (Opensettings)",__FUNCTION__, handle->opensettings.bufferHeap));
        memHandle = handle->opensettings.bufferHeap;
    }
    else
    {
        BDBG_MSG(("%s Frame buffer from default",__FUNCTION__));
        memHandle = handle->devicehandle->memHandle;
    }        
    errcode=BMEM_Free(memHandle, (void*)handle->veeoutput.pstMetadataUncached);
    if(errcode != BERR_SUCCESS)
    {
        BDBG_WRN(("BMEM_Heap_Free:pstMetadataUncached:Failure"));
        return errcode;
    }
    errcode=BMEM_Free(memHandle, (void*)handle->veeoutput.pstDescriptorsUncached);
    if(errcode != BERR_SUCCESS)
    {
        BDBG_WRN(("BMEM_Heap_Free:pstDescriptorsUncached:Failure"));
        return errcode;
    }
    for(i=0;i<handle->opensettings.maxQueuedPictures;i++)
    {
       BMEM_Free(memHandle,handle->capturepicture[i].pPpBuffer);
    }
    BMEM_Free(memHandle,(void*)handle->ui32RefFrameBaseAddr);
    BKNI_Free(handle->capturepicture);
    return errcode;
 } 
 
 static void BVEE_Channel_P_CleanupTaskCreateSettings(BVEE_ChannelHandle handle)
 {
     unsigned i;
 
     /* The first branch is always static.  Others need to be BKNI_Free()'d. */
     /* This is not an ideal programming model, but workable */
     for ( i = 1; i < handle->taskcreatesettings.numBranches; i++ )
     {
         BDBG_ASSERT(NULL != handle->taskcreatesettings.pBranchInfo[i]);
         BKNI_Free(handle->taskcreatesettings.pBranchInfo[i]);
     }
 
     if (handle->taskcreatesettings.psVEncoderConfig)
         BKNI_Free(handle->taskcreatesettings.psVEncoderConfig);
     
     /* Reset structure to defaults */
     BDSP_Task_GetDefaultCreateSettings(handle->devicehandle->dspContext, &handle->taskcreatesettings);
 }
 static BERR_Code BVEE_Channel_P_ValidateSettings(
    BVEE_ChannelHandle handle,
    const BVEE_ChannelStartSettings *pSettings,
    BVEE_ChannelStartSettings *pOutputSettings
    )
 {
     BDSP_VideoEncode videoEncType;
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     BDBG_ASSERT(NULL != pSettings);
     BDBG_ASSERT(NULL != pOutputSettings);
     
     /* Start by copying the existing settings */
     BKNI_Memcpy(pOutputSettings, pSettings, sizeof(BVEE_ChannelStartSettings));
 
     /* Check for valid input */
     if ( NULL == pSettings->pContextMap )
     {
         BDBG_ERR(("Must specify an input to encode"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
     }
 
     /* Store local copy of the RAVE context map in case it goes out of scope after start. */
     if ( pSettings->pContextMap )
     {
         BKNI_Memcpy(&handle->contextmap, pSettings->pContextMap, sizeof(BAVC_XptContextMap));
         pOutputSettings->pContextMap = &handle->contextmap;
     }
     
 
     videoEncType = BVEE_P_GetCodecVideoType(pSettings->codec);
         
     if ( videoEncType == BDSP_VideoEncode_eMax )
     {
         return BERR_TRACE(BERR_NOT_SUPPORTED);
     }
 
     return BERR_SUCCESS;
 }

 BERR_Code BVEE_Channel_Open(
    BVEE_Handle deviceHandle,
    unsigned index,
    const BVEE_ChannelOpenSettings *pSettings, 
    BVEE_ChannelHandle *pHandle
    )
 {
     BERR_Code errCode;
     BVEE_ChannelOpenSettings defaults;
     BVEE_ChannelHandle handle;
 
     BDBG_OBJECT_ASSERT(deviceHandle, BVEE_Device);
     
     if ( NULL == pSettings )
     {
         BDBG_WRN (("pSettings is NULL. Using Defaults with memPicHandle as NULL"));
         BVEE_Channel_GetDefaultOpenSettings(&defaults);
         pSettings = &defaults;
     }
 
     if ( index >= BVEE_MAX_CHANNELS )
     {
         BDBG_ERR(("This chip only supports %u channels. Cannot open channel %u", BVEE_MAX_CHANNELS, index));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
     }
 
     if ( deviceHandle->channels[index] )
     {
         BDBG_ERR(("Channel %d already open", index));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
     }
 
     handle = BKNI_Malloc(sizeof(BVEE_Channel));
     if ( NULL == handle )
     {
         return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
     }
     BKNI_Memset(handle, 0, sizeof(BVEE_Channel));
 
     handle->devicehandle = deviceHandle;
     handle->index = index;
     BKNI_Snprintf(handle->name, sizeof(handle->name), "Channel %u", index);
     handle->state = BVEE_ChannelState_eStopped;
     BKNI_Memcpy(&handle->opensettings, pSettings, sizeof(handle->opensettings));
     BDBG_OBJECT_SET(handle, BVEE_Channel);
 
 
     /* Allocate Memory for descriptor Buffers*/
     errCode = BVEE_Channel_P_AllocatePictParamBuffer (handle);
     if ( errCode )
     {
         errCode = BERR_TRACE(errCode);
         goto err_alloc_framebuf;
     }
     
     /* Success */
     *pHandle = handle;
     deviceHandle->channels[index] = handle;
     return BERR_SUCCESS;
 
 err_alloc_framebuf:
     BKNI_Free (handle);
 
     *pHandle = NULL;
 
     return errCode;
 }

 BERR_Code BVEE_Channel_Close(BVEE_ChannelHandle handle)
 {
    BERR_Code errCode = BERR_SUCCESS;
    
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);

    if ( handle->state != BVEE_ChannelState_eStopped )
    {
        BDBG_WRN(("Implicitly stopping channel %u on shutdown.", handle->index));
        BVEE_Channel_Stop(handle);
    }

    errCode = BVEE_Channel_P_DeAllocatePictParamBuffer(handle);
    
    /* Cleanup */
    handle->devicehandle->channels[handle->index] = NULL;
    
    BDBG_OBJECT_DESTROY(handle, BVEE_Channel);
    BKNI_Free(handle);
    return errCode;
 }

 void BVEE_Channel_GetDefaultStartSettings(BVEE_ChannelStartSettings *pSettings)
 {
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->codec = BAVC_VideoCompressionStd_eH264;
    pSettings->pContextMap = NULL;
    pSettings->nonRealTime = false;
    pSettings->frameRate = BAVC_FrameRateCode_e30;
    pSettings->eAspectRatio = BVEE_AspectRatio_e4_3;
    /*Using graphics surface handle as output of BVN.
      Do we support planar format for it?*/
    pSettings->pxlformat = BAVC_YCbCrType_e4_2_2;
    pSettings->sendMetadata = false;
    pSettings->sendEos = false;
    /*Default dsp task user config*/
    pSettings->ui32TargetBitRate = 400000;
    pSettings->bDblkEnable = false;
    pSettings->bSubPelMvEnable = false;
    pSettings->bVarModeOptEnable = false;
    pSettings->eGopStruct = BVEE_VideoGopStruct_eIP;
    pSettings->eProfileIDC = BVEE_VideoH264Profile_eBaseline;
    pSettings->ui32LevelIdc = 40;
    pSettings->ui32IntraPeriod = 30;
    pSettings->ui32IDRPeriod = 30;
    pSettings->bRateControlEnable = 1;
    pSettings->ui32EncodPicHeight = 240;
    pSettings->ui32EncodPicWidth = 320;
    pSettings->stcIndx = 0;
    
    return;
 }
 
 void BVEE_Channel_GetSettings(BVEE_ChannelHandle handle, BVEE_ChannelStartSettings *pSettings)
 {
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->startsettings;
    
    return;
 }

 static BERR_Code BVEE_Channel_P_Start(
    BVEE_ChannelHandle handle
    )
 {
     BERR_Code errCode;
     unsigned i=0;
     uint32_t offset,ui32UsedSize,ui32LumaStripeHeight,ui32ChromaStripeHeight,ui32LumaBufferSize,ui32ChromaBufferSize;
     void *pAddress;
     size_t cdbLength;
     BDSP_TaskHandle task;
     BDSP_VF_P_sVEncoderConfig *psVencCfg;
     const BVEE_ChannelStartSettings *pSettings;
     BDSP_Raaga_VideoBH264UserConfig userConfig;
     BMEM_Handle memHandle = NULL;
     
     BDBG_MSG(("BVEE_Channel_P_Start(%#x) [index %u]", handle, handle->index));
 
     pSettings = &handle->startsettings;
     if (handle->opensettings.bufferHeap!= NULL)
     {
         BDBG_MSG(("%s Frame buffer from %#x (Opensettings)",__FUNCTION__, handle->opensettings.bufferHeap));
         memHandle = handle->opensettings.bufferHeap;
     }
     else
     {
         BDBG_MSG(("Frame buffer from default"));
         memHandle = handle->devicehandle->memHandle;
     }

     handle->veeoutput.uiCDBBufferShadowReadOffset = BREG_Read32(handle->devicehandle->regHandle, handle->startsettings.pContextMap->CDB_Base);
     handle->veeoutput.uiITBBufferShadowReadOffset = BREG_Read32(handle->devicehandle->regHandle, handle->startsettings.pContextMap->ITB_Base);

     /*Convert CDB_Base offset to Cached address*/
     offset = BREG_Read32(handle->devicehandle->regHandle, handle->startsettings.pContextMap->CDB_Base);
     errCode = BMEM_Heap_ConvertOffsetToAddress(memHandle, offset, &pAddress);
     if ( errCode )
     {
        errCode = BERR_TRACE(errCode);
        goto err_cdb_offset;
     }
     errCode = BMEM_Heap_ConvertAddressToCached(memHandle, pAddress, &handle->veeoutput.pCdbBaseCached);
     if ( errCode )
     {
        errCode = BERR_TRACE(errCode);
        goto err_cdb_offset;
     }    
     /* Flush the CDB data from the cache prior to the HW filling it */
     cdbLength = (1+BREG_Read32(handle->devicehandle->regHandle, handle->startsettings.pContextMap->CDB_End))-offset;
     BMEM_Heap_FlushCache(memHandle, handle->veeoutput.pCdbBaseCached, cdbLength);
     
     /* Setup DSP Task First */
     BDSP_Task_GetDefaultCreateSettings(handle->devicehandle->dspContext, &handle->taskcreatesettings);
 
     /* Setup default branch */
     handle->taskcreatesettings.realtimeMode = BDSP_TaskRealtimeMode_eNonRealTime;
     handle->taskcreatesettings.dspIndex = DSP_INDEX;
     handle->taskcreatesettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
     handle->taskcreatesettings.numBranches =1;
     handle->taskcreatesettings.pBranchInfo[0] = &handle->primarybranch;
     handle->taskcreatesettings.videoEncRdbConfig.stcAddress = BVEE_CHP_GET_STC_ADDR(pSettings->stcIndx);;
     
     BVEE_DSP_P_InitBranch(&handle->primarybranch);

     handle->primarybranch.ui32NumStages = 1;
     
     /* Setup Encode Stage */
     handle->primarybranch.sFwStgInfo[0].ui32BranchId = 0;
     handle->primarybranch.sFwStgInfo[0].ui32StageId = 0;
     handle->primarybranch.sFwStgInfo[0].eStageType = BDSP_CIT_P_StageType_eVideoEncode;
     handle->primarybranch.sFwStgInfo[0].uAlgorithm.eVideoEncAlgo = BVEE_P_GetCodecVideoType(handle->startsettings.codec);
     handle->taskcreatesettings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eEncAudioMode = BDSP_EncodeMode_eNonRealtime;
     
     handle->primarybranch.sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 
     BKNI_Memset(&handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0], 0, 
                 sizeof(handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0]));
     if ( pSettings->pContextMap )
     {         
         /* Configuring outputs */					
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 2;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.ui32NumDst[0] = 1;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.ui32NumDst[1] = 1;

         /* Set output type to be eCdb and eItb */
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eCdb;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.eDistinctOpType[1] = BDSP_AF_P_DistinctOpType_eItb;

         /* Configure Cdb Op type */
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRAVE;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Base;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_End;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Read;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Valid;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Wrap;

         /* Configure Itb Op type */
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRAVE;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Base;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_End;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Read;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Valid;
         handle->primarybranch.sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Wrap;  
     }

 	 handle->taskcreatesettings.psVEncoderConfig =(BDSP_VF_P_sVEncoderConfig*)BKNI_Malloc(sizeof(BDSP_VF_P_sVEncoderConfig));
     if (NULL == handle->taskcreatesettings.psVEncoderConfig)
     {
         return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
     }     
 	 BKNI_Memset(handle->taskcreatesettings.psVEncoderConfig, 0, sizeof(BDSP_VF_P_sVEncoderConfig));

     psVencCfg = handle->taskcreatesettings.psVEncoderConfig;
 
     psVencCfg->sEncoderParams.eEncodeFrameRate = BVEE_P_FrameRateLUT[pSettings->frameRate];
     psVencCfg->sEncoderParams.ui32Frames2Accum = 1; /*handle->opensettings.maxQueuedPictures;TBD:*/
     psVencCfg->ui32MaxFrameWidth = handle->opensettings.resolution.width;
     psVencCfg->ui32MaxFrameHeight = handle->opensettings.resolution.height;
     psVencCfg->ui32StripeWidth = BVEE_DEFAULT_STRIPE_WIDTH;
     
     for(i=0;i<BDSP_FWMAX_VIDEO_BUFF_AVAIL;i++)
     {
        errCode = BMEM_ConvertAddressToOffset(memHandle, (void *)handle->capturepicture[i].pPpBuffer, &offset);
        psVencCfg->sPPBs[i].ui32DramBufferAddress = offset;
        psVencCfg->sPPBs[i].ui32BufferSizeInBytes = sizeof(BVENC_VF_sPicParamBuff);
     }				

     if((handle->opensettings.resolution.height>480)&&(handle->opensettings.resolution.width>640))
     {  /*Luma,Chroma settings for HD resolution*/
        ui32LumaStripeHeight = BVEE_H264_ENCODE_REF_LUMASTRIPE_HEIGHT_HD;
        ui32ChromaStripeHeight = BVEE_H264_ENCODE_REF_CHROMASTRIPE_HEIGHT_HD;
        ui32LumaBufferSize = BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE_HD;
        ui32ChromaBufferSize = BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE_HD;
        if((handle->startsettings.bDblkEnable)||
            (handle->startsettings.bSubPelMvEnable))
        {
            BDBG_ERR((" %s Band width not enough to support HD encoding", __FUNCTION__));
        }
     }
     else
     {  /*Luma,Chroma settings for Tablet resolution*/
        ui32LumaStripeHeight = BVEE_H264_ENCODE_REF_LUMASTRIPE_HEIGHT;
        ui32ChromaStripeHeight = BVEE_H264_ENCODE_REF_CHROMASTRIPE_HEIGHT;
        ui32LumaBufferSize = BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE;
        ui32ChromaBufferSize = BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE;
     }
     ui32UsedSize = 0;
     psVencCfg->sReferenceBuffParams.ui32ChromaStripeHeight = ui32ChromaStripeHeight;
     psVencCfg->sReferenceBuffParams.ui32LumaStripeHeight = ui32LumaStripeHeight;
     psVencCfg->sReferenceBuffParams.ui32NumBuffAvl = BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;
     for(i=0; i<BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL ; i++)
     {
     	psVencCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
     		= (uint32_t)(handle->ui32RefFrameBaseAddr + ui32UsedSize);
     	psVencCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes
     		= ui32LumaBufferSize;
     	ui32UsedSize += ui32LumaBufferSize;
 
     	psVencCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress
     		= (uint32_t)(handle->ui32RefFrameBaseAddr + ui32UsedSize);
     	psVencCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes
     		= ui32ChromaBufferSize;						   
     	ui32UsedSize += ui32ChromaBufferSize;
     }
     /*copy the external interrupt configuration to task create settings*/
     BKNI_Memcpy(&handle->taskcreatesettings.extInterruptConfig, &pSettings->extIntCfg, sizeof(BDSP_ExtInterruptConfig));
     
     /* Ready */
     errCode = BDSP_Task_Create(handle->devicehandle->dspContext, &handle->taskcreatesettings, &task);
     if ( errCode )
     {
         errCode = BERR_TRACE(errCode);
         goto err_task_create;
     }
     /* Set any user config param before starting the task */
     BDBG_MSG(("Getting video encode user config "));
     errCode = BDSP_Task_GetStageSettings(task, 0, 0, &userConfig, sizeof(userConfig));
     if(errCode != BERR_SUCCESS)
     {
     	BERR_TRACE(errCode);
     }
     userConfig.ui32TargetBitRate = handle->startsettings.ui32TargetBitRate; /*400000*/
     userConfig.eDblkEnable = handle->startsettings.bDblkEnable;
     userConfig.eSubPelEnable = handle->startsettings.bSubPelMvEnable;
     userConfig.eVarModeDecsOptEnable = handle->startsettings.bVarModeOptEnable;
     userConfig.eGopStruct = handle->startsettings.eGopStruct; /*IP*/
     userConfig.eProfileIDC = handle->startsettings.eProfileIDC; /*66*/
     userConfig.ui32LevelIdc = handle->startsettings.ui32LevelIdc; /*40*/
     userConfig.ui32IntraPeriod = handle->startsettings.ui32IntraPeriod; /*30*/
     userConfig.ui32IDRPeriod = handle->startsettings.ui32IDRPeriod; /*30*/
     userConfig.eRateControlEnable = handle->startsettings.bRateControlEnable; /*1*/
     userConfig.ui32EncodPicHeight = handle->startsettings.ui32EncodPicHeight; /*240*/
     userConfig.ui32EncodPicWidth = handle->startsettings.ui32EncodPicWidth; /*320*/
     userConfig.ui32End2EndDelay = handle->startsettings.ui32End2EndDelay;
 
     BDBG_MSG(("Setting video encode user config "));
     errCode= BDSP_Task_SetStageSettings(task, 0, 0, &userConfig, sizeof(userConfig));		
     if(errCode != BERR_SUCCESS)
     {
     	BERR_TRACE(errCode);
     }
     BKNI_EnterCriticalSection();
     handle->task = task;
     BKNI_LeaveCriticalSection();
  
     errCode = BDSP_Task_Start(handle->task);
     if ( errCode )
     {
         (void)BERR_TRACE(errCode);
         goto err_start_task;
     } 
     
     handle->state = BVEE_ChannelState_eStarted;
     return BERR_SUCCESS;
 
 err_start_task:   
     task = handle->task;
     BKNI_EnterCriticalSection();
     handle->task = NULL;
     BKNI_LeaveCriticalSection();
     BDSP_Task_Destroy(task);
 err_task_create:
     BVEE_Channel_P_CleanupTaskCreateSettings(handle);    
 err_cdb_offset:
     return errCode;
 }

 BERR_Code BVEE_Channel_Start(BVEE_ChannelHandle handle, 
                                        const BVEE_ChannelStartSettings *pSettings)
 {
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    BDBG_ASSERT(NULL != pSettings);

    BDBG_MSG(("BVEE_Channel_Start(%#x) [index %u]", handle, handle->index));

    if ( NULL == handle->devicehandle->dspContext )
    {
        BDBG_ERR(("DSP Not avaliable"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->state != BVEE_ChannelState_eStopped )
    {
        BDBG_ERR(("Already running, cannot start"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Sanity check settings */
    errCode = BVEE_Channel_P_ValidateSettings(handle, pSettings, &handle->startsettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BVEE_Channel_P_Start(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Success */
    return BERR_SUCCESS;
 }
 
 static BERR_Code BVEE_Channel_P_Stop(
    BVEE_ChannelHandle handle
    )
 {
    BDSP_TaskHandle task;

    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);

    if ( NULL == handle->task )
    {
        BDBG_MSG(("BVEE_Channel_P_Stop: Channel %u already stopped.", handle->index));
        return BERR_SUCCESS;
    }

#if BVDE_DISABLE_DSP
    #warning Task Start is Disabled!
    BDBG_ERR(("NOT STOPPING DSP"));
#else
    BDSP_Task_Stop(handle->task);
#endif

    task = handle->task;
    BKNI_EnterCriticalSection();    /* Required to sync with display interrupts so they have finished prior to calling BDSP_Task_Destroy */
    handle->task = NULL;
    BKNI_LeaveCriticalSection();

    BDSP_Task_Destroy(task);
    BVEE_Channel_P_CleanupTaskCreateSettings(handle);

    return BERR_SUCCESS;
}
 
 void BVEE_Channel_Stop(BVEE_ChannelHandle handle)
 {
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);

    BDBG_MSG(("BVEE_Channel_Stop(%#x) [index %u]", handle, handle->index));

    switch ( handle->state )
    {
        case BVEE_ChannelState_eStopped:
            BDBG_WRN(("Channel %u Already Stopped.", handle->index));
            return;
        case BVEE_ChannelState_ePaused:
            /*TBD*/
            break;
        default:
            break;
    }

    /* Stop the task first */
    handle->state = BVEE_ChannelState_eStopped;
    handle->startsettings.sendEos = true;
    /* Serialize with critical section prior to stopping the task, 
    guarantees isrs are not updating while we stop (they check the state first) */
    BKNI_EnterCriticalSection();
    BKNI_LeaveCriticalSection();

    BVEE_Channel_P_Stop(handle);
}

 BERR_Code BVEE_Channel_EnqueuePicture_isr(
    BVEE_ChannelHandle handle,
    const BVEE_PictureDescriptor *pPicture 
    )
 {
    BERR_Code errCode=BERR_SUCCESS;
    unsigned i=0;
    uint32_t BuffYAddr;
    BVENC_VF_sPicParamBuff		*pPpb = NULL;    
    BVEE_CapBufferMemory *temp_cappict = NULL;
    BMEM_Handle memHandle = NULL; 

    BDBG_ENTER(BVEE_Channel_EnqueuePicture_isr);
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    BDBG_ASSERT(NULL != pPicture);

    if (handle->opensettings.bufferHeap!= NULL)
    {
        BDBG_MSG(("%s Frame buffer from %#x (Opensettings)",__FUNCTION__, handle->opensettings.bufferHeap));
        memHandle = handle->opensettings.bufferHeap;
    }
    else
    {
        BDBG_MSG(("%s Frame buffer from default",__FUNCTION__));
        memHandle = handle->devicehandle->memHandle;
    }
    for(i=0;i<handle->opensettings.maxQueuedPictures;i++)
    {
        if(handle->capturepicture[i].bValid == false)
        {
            temp_cappict = &handle->capturepicture[i];
            temp_cappict->bValid = true;
            break;
        }
    }
    if(i == handle->opensettings.maxQueuedPictures)
    {
       return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }
    
    temp_cappict->pPDesc = (void*)pPicture->pvaddress;
    pPpb = (BVENC_VF_sPicParamBuff *)temp_cappict->pPpBuffer;
    
    BDBG_MSG(("Picture:FrameRate %d\n" "\t\t ui32CurrentPTS %d \n" "\t\t height %d \n" "\t\t width %d \n"
              "\t\t polarity %d \n" "\t\t sarHorz %d \n" "\t\t sarVert %d \n" "\t\t repeat %d \n" "\t\t ignore %d \n",
              pPicture->frameRate,pPicture->originalPts.ui32CurrentPTS,pPicture->height,pPicture->width,pPicture->polarity
              ,pPicture->sarHorizontal,pPicture->sarVertical,pPicture->repeat,pPicture->ignore));
    pPpb->ui32CaptureTimeStamp = pPicture->STC_Lo;
    /*Assuming application always sends virtual address of captured buffer, 
      PI should convert it to physical before sending it to FW */
    BMEM_ConvertAddressToOffset(memHandle, pPicture->pvaddress, &BuffYAddr);
    pPpb->ui32BuffAddrY = (uint32_t)BuffYAddr;
    pPpb->sPictureMetaData.ui32PicHeight = pPicture->height;
    pPpb->sPictureMetaData.ui32PicWidth = pPicture->width;
    pPpb->sPictureMetaData.eFrameType = pPicture->polarity;
    pPpb->sPictureMetaData.eChromaSampling = handle->startsettings.pxlformat;
    pPpb->sPictureMetaData.eEncodeFrameRate = BVEE_P_FrameRateLUT[pPicture->frameRate];
    pPpb->sPictureMetaData.eAspectRatio = handle->startsettings.eAspectRatio;
    pPpb->sPictureMetaData.ui32OrignalPtsHigh = 0;/*since clock is 45KHz, PTS high will be zero*/
    pPpb->sPictureMetaData.ui32OrignalPtsLow = pPicture->originalPts.ui32CurrentPTS;       
    
    errCode = BDSP_VideoEncode_putPicture_isr( (void *)handle->task, (uint32_t) temp_cappict->pPpBuffer);
    if(errCode == BERR_SUCCESS)
    {
        BDBG_LEAVE(BVEE_Channel_EnqueuePicture_isr);
        return BERR_SUCCESS;
    }
        
    BDBG_LEAVE(BVEE_Channel_EnqueuePicture_isr);
    return errCode;
 }
 void * BVEE_Channel_DequeuePicture_isr(
                    BVEE_ChannelHandle handle)
 {
    BERR_Code errcode= BERR_SUCCESS;
    unsigned i=0;
    void *temp_ptr=NULL;
    BVENC_VF_sPicParamBuff	*pPpb = NULL;

    BDBG_ENTER(BVEE_Channel_DequeuePicture_isr);
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    
    errcode = BDSP_VideoEncode_getPictureBuffer_isr( (void *)handle->task, (BVENC_VF_sPicParamBuff**)&pPpb);
    for(i=0;i<handle->opensettings.maxQueuedPictures;i++)
    {
        if((handle->capturepicture[i].pPpBuffer == pPpb)&&
            (handle->capturepicture[i].bValid == true))
        {
            temp_ptr = handle->capturepicture[i].pPDesc;/*Give back the address*/
            handle->capturepicture[i].bValid = false;
            handle->capturepicture[i].pPDesc = NULL;
            break;
        }
    }
    
    return temp_ptr;
 }
 
 BERR_Code BVEE_Channel_GetBufferDescriptors(
   BVEE_ChannelHandle handle,
   const BAVC_VideoBufferDescriptor **pBuffer,
   size_t *pSize,
   const BAVC_VideoBufferDescriptor **pBuffer2,
   size_t *pSize2
   )
 {  
   BVEE_P_ITBEntry *pITBEntry;
   BVEE_P_ITBEntry *pITBEntryNext = NULL;
   
   BAVC_VideoBufferDescriptor *pVideoDescriptor;
   BVEE_ChannelOutputDescriptorInfo *psOutputDescDetails;
   uint32_t uiCDBBaseOffset;
   uint32_t uiCDBEndOffset;
   uint32_t uiCDBValidOffset;
   uint32_t uiCDBReadOffset;
   uint32_t uiCDBEndOfFrameOffset; 
   uint32_t uiTemp;
   
   BDBG_ENTER(BVEE_Channel_GetBufferDescriptors);
   
   BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
   BDBG_ASSERT(NULL != pBuffer);
   BDBG_ASSERT(NULL != pBuffer2);
   BDBG_ASSERT(NULL != pSize);
   BDBG_ASSERT(NULL != pSize2);

   *pBuffer = NULL;
   *pSize = 0;
   *pBuffer2 = NULL;
   *pSize2 = 0;
   psOutputDescDetails = &handle->veeoutput;
  /* Snapshot the CDB/ITB read/valid pointers
   * NOTE: We MUST snapshot the CDB pointers first,
   * and then the ITB pointers so that we can properly
   * detect the end of the current frame.  If we read
   * the ITB first, and then the CDB, it is possible a
   * new ITB entry has been written in between the reads,
   * and the CDB write pointer now includes the beginning
   * of the next frame */
  { /* CDB Pointers */
     uiCDBBaseOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.CDB_Base);
     uiCDBEndOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.CDB_End);         
     if ( uiCDBEndOffset != 0 )
     {
         uiCDBEndOffset += 1; /* end is inclusive */
     }
     uiCDBValidOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.CDB_Valid);
     if ( uiCDBValidOffset >= uiCDBEndOffset )
     {
        uiCDBValidOffset = uiCDBBaseOffset + ( uiCDBValidOffset - uiCDBEndOffset );
     }
     uiCDBReadOffset = BREG_Read32(handle->devicehandle->regHandle,handle->contextmap.CDB_Base);
  }
  
  while ( 1 )
  {
     /* Check for Available ITB Entries */
     /* TODO: We may want to copy ITB entries into local memory
      * so that we can handle ITB wrap easily
      */
      /* Check for Available CDB Data */
        if ( uiCDBValidOffset == \
             psOutputDescDetails->uiCDBBufferShadowReadOffset )
        {
            /* We ran out of CDB data */
            BDBG_MSG(("No more CDB Data"));
            break;
        }
        /* Check for Available ITB Entries */
        /* TODO: We may want to copy ITB entries into local memory
         * so that we can handle ITB wrap easily
         */

        BVEE_Channel_P_ParseItb(handle, &pITBEntry, &pITBEntryNext);
        if ( NULL == pITBEntry )
        {
            BDBG_MSG(("No more ITB Entries"));
            break;
        }
     
         if ( NULL == pITBEntryNext )
         {
             /* We can not work unless we have the next ITB entry to determine real frame size.
                Full frame of data is required. */
             BDBG_MSG(("Partial Frame in CDB"));
             break;
         }
         else
         {
             uint32_t uiDepthToNext;
             uint32_t uiDepthToValid;

             /* It is possible that the CDB Valid doesn't, yet, contain any of the next frame and
              * may still be in the middle of the current frame, so we need use the depth that is the
              * lesser of depth(cdb_read,cdb_next) depth(cdb_read,cdb_valid)
              */
             if ( BVEE_ITB_GET_FIELD(&pITBEntryNext->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS) > \
                  psOutputDescDetails->uiCDBBufferShadowReadOffset )
             {
                 uiDepthToNext = BVEE_ITB_GET_FIELD(&pITBEntryNext->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS) - \
                                 psOutputDescDetails->uiCDBBufferShadowReadOffset;
             }
             else
             {
                 uiDepthToNext = uiCDBEndOffset - \
                                 psOutputDescDetails->uiCDBBufferShadowReadOffset;
                 uiDepthToNext += BVEE_ITB_GET_FIELD(&pITBEntryNext->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS) - uiCDBBaseOffset;
             }

             if ( uiCDBValidOffset > \
                  psOutputDescDetails->uiCDBBufferShadowReadOffset )
             {
                 uiDepthToValid = uiCDBValidOffset - \
                                  psOutputDescDetails->uiCDBBufferShadowReadOffset;
             }
             else
             {
                 uiDepthToValid = uiCDBEndOffset - \
                                  psOutputDescDetails->uiCDBBufferShadowReadOffset;
                 uiDepthToValid += uiCDBValidOffset - uiCDBBaseOffset;
             }

             if ( uiDepthToValid < uiDepthToNext )
             {
                 uiCDBEndOfFrameOffset = uiCDBValidOffset;
             }
             else
             {
                 uiCDBEndOfFrameOffset = BVEE_ITB_GET_FIELD(&pITBEntryNext->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS);;
             }
         }

         /* Get Video Descriptor for this ITB entry */
         uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % BVEE_MAX_ITBDESCRIPTORS;
         if ( uiTemp == psOutputDescDetails->uiDescriptorReadOffset )
         {
             BDBG_MSG(("Out of descriptors"));
             break;
         }
         pVideoDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
         psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

         BKNI_Memset(pVideoDescriptor, 0, sizeof(BAVC_VideoBufferDescriptor));

         if ( uiCDBEndOfFrameOffset > psOutputDescDetails->uiCDBBufferShadowReadOffset )
         {
             pVideoDescriptor->stCommon.uiLength = uiCDBEndOfFrameOffset - \
                                                   psOutputDescDetails->uiCDBBufferShadowReadOffset;
         }
         else
         {
             /* CDB Wrap occurs, so we need to split this picture into two descriptors.  We handle the first one here. */
             pVideoDescriptor->stCommon.uiLength = uiCDBEndOffset - \
                                                   psOutputDescDetails->uiCDBBufferShadowReadOffset;
         }

         /* Populate other fields if this descriptor contains the beginning of the frame */
         if ( psOutputDescDetails->uiCDBBufferShadowReadOffset == \
              BVEE_ITB_GET_FIELD(&pITBEntry->fields.baseAddress, BASE_ADDRESS, CDB_ADDRESS) )
         {

             /* We're muxing the beginning of this frame */
             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START;

             if ( 0 != BVEE_P_ITBEntry_GetIFrame(pITBEntry) )
             {
                pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP;
             }
             
             pVideoDescriptor->stCommon.uiOriginalPTS = BVEE_ITB_GET_FIELD(&pITBEntry->fields.escrMetadata, ESCR_METADATA, ORIGINAL_PTS);

             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID;
             pVideoDescriptor->stCommon.uiPTS = BVEE_ITB_GET_FIELD(&pITBEntry->fields.ptsDts, PTS_DTS, PTS_32);
             pVideoDescriptor->stCommon.uiPTS <<= 32;
             pVideoDescriptor->stCommon.uiPTS |= BVEE_ITB_GET_FIELD(&pITBEntry->fields.ptsDts, PTS_DTS, PTS);

             pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID;
             pVideoDescriptor->uiDTS = BVEE_P_ITBEntry_GetDTS(pITBEntry);
             
             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID;
             pVideoDescriptor->stCommon.uiESCR = BVEE_ITB_GET_FIELD(&pITBEntry->fields.escrMetadata, ESCR_METADATA, ESCR);

             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID;
             pVideoDescriptor->stCommon.uiTicksPerBit = BVEE_ITB_GET_FIELD(&pITBEntry->fields.bitRate, BIT_RATE, TICKS_PER_BIT);

             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID;
             pVideoDescriptor->stCommon.iSHR = BVEE_ITB_GET_FIELD(&pITBEntry->fields.bitRate, BIT_RATE, SHR);
             
             /* DataUnit detection.TODO:Populate video descriptors for all NALU */
             switch ( handle->startsettings.codec )
             {
                case BAVC_VideoCompressionStd_eH264:
                     {
                         uint8_t *pCDBBufferCached =  (uint8_t*) handle->veeoutput.pCdbBaseCached;
                         uint8_t *uiCurrentByte = &pCDBBufferCached[psOutputDescDetails->uiCDBBufferShadowReadOffset-uiCDBBaseOffset];                
                         uint32_t uiDataUnitStartCode = (uiCurrentByte[0]<<24)|(uiCurrentByte[1]<<16)|(uiCurrentByte[2])<<16|uiCurrentByte[3];
                         if(uiDataUnitStartCode == BVEE_DATA_UNIT_START_CODE)
                         {
                             pVideoDescriptor->uiVideoFlags |= BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START;
                             pVideoDescriptor->uiDataUnitType = uiCurrentByte[4];                    
                         }
                    }
                    break;
                default:
                    break;
             }
         }

         /* Normalize the offset to 0 */
         pVideoDescriptor->stCommon.uiOffset = \
                                               psOutputDescDetails->uiCDBBufferShadowReadOffset - uiCDBBaseOffset;
         
         /* Invalidate this frame from the cache prior to the host accessing it */
         BMEM_Heap_FlushCache((handle->opensettings.bufferHeap!= NULL)?
                              handle->opensettings.bufferHeap:handle->devicehandle->memHandle, 
                              (char *)handle->veeoutput.pCdbBaseCached + pVideoDescriptor->stCommon.uiOffset, 
                              pVideoDescriptor->stCommon.uiLength);

         /* Advance read pointer appropriately */
         psOutputDescDetails->uiCDBBufferShadowReadOffset += \
                                                             pVideoDescriptor->stCommon.uiLength;
         if ( psOutputDescDetails->uiCDBBufferShadowReadOffset >= \
              uiCDBEndOffset )
         {
             psOutputDescDetails->uiCDBBufferShadowReadOffset -= \
                                                                 ( uiCDBEndOffset - uiCDBBaseOffset );
         }
         
         /* If we need to send metadata, send it on the first frame */
         if ( handle->startsettings.sendMetadata && (pVideoDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START) )
         {
             BAVC_VideoMetadataDescriptor *pMetadataDescriptor = &psOutputDescDetails->pstMetadataCached[0];
             BAVC_VideoBufferDescriptor *pFrameDescriptor;

             /* This is the first frame so we should always have another descriptor available. Assert for sanity. */
             uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % BVEE_MAX_VIDEODESCRIPTORS;
             BDBG_ASSERT(uiTemp != psOutputDescDetails->uiDescriptorReadOffset);
             pFrameDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
             psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

             /* The metadata descriptor must come before the first frame.  Swap them. */
             *pFrameDescriptor = *pVideoDescriptor;   /* Copy frame descriptor contents into second descriptor - we're about to overwrite the old one */

             BKNI_Memset(pVideoDescriptor, 0, sizeof(BAVC_VideoBufferDescriptor));
             
             pVideoDescriptor->stCommon.uiOffset = 0;
             pVideoDescriptor->stCommon.uiLength = sizeof( BAVC_VideoMetadataDescriptor );
             pVideoDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA;
             pVideoDescriptor->uiDataUnitType = BAVC_VideoMetadataType_eCommon;
             
             /* Populate metadata */
             BKNI_Memset(pMetadataDescriptor, 0, sizeof(*pMetadataDescriptor));
                          
             /* Set Protocol */
             BDBG_MSG(("Setting protocol to %u", handle->startsettings.codec));
             switch ( handle->startsettings.codec )
             {
                case BAVC_VideoCompressionStd_eH264:
                    /* Populate Coded Dimension */
                    pMetadataDescriptor->uiMetadataFlags |= BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_DIMENSION_CODED_VALID;
                    pMetadataDescriptor->stDimension.coded.uiHeight = ( ( BVEE_P_ITBEntry_GetMetadata(pITBEntry) >> 0 ) & 0x7F ) << 4;
                    pMetadataDescriptor->stDimension.coded.uiWidth = ( ( BVEE_P_ITBEntry_GetMetadata(pITBEntry) >> 7 ) & 0x7F ) << 4;
                    break;
                default:
                    break;
             }

             handle->startsettings.sendMetadata = false;
         }
     }

     if ( handle->startsettings.sendEos )
     {
         BDBG_MSG(("EOS Required"));
         /* Get a descriptor for EOS */
         uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % BVEE_MAX_VIDEODESCRIPTORS;
         if ( uiTemp == psOutputDescDetails->uiDescriptorReadOffset )
         {
             BDBG_MSG(("Out of descriptors, can't send EOS"));
         }
         else
         {
             pVideoDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
             psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

             BKNI_Memset(pVideoDescriptor, 0, sizeof(BAVC_VideoBufferDescriptor));

             /* Decoder is stopped and we have run out of data. Fill the EOS entry in Video descriptor */
             pVideoDescriptor->stCommon.uiFlags = BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS;
             BDBG_MSG(("EOS %#x", pVideoDescriptor));
             handle->startsettings.sendEos = false;
         }
     }

     /* Assign array(s) and count(s) */
     if ( psOutputDescDetails->uiDescriptorWriteOffset >= \
          psOutputDescDetails->uiDescriptorReadOffset )
     {
         *pBuffer = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset];
         *pSize = psOutputDescDetails->uiDescriptorWriteOffset - psOutputDescDetails->uiDescriptorReadOffset;

         *pBuffer2 = NULL;
         *pSize2 = 0;         
     }
     else
     {
         *pBuffer = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset];
         *pSize = BVEE_MAX_VIDEODESCRIPTORS - psOutputDescDetails->uiDescriptorReadOffset;

         *pBuffer2 = &psOutputDescDetails->pstDescriptorsCached[0];
         *pSize2 = psOutputDescDetails->uiDescriptorWriteOffset;
     }
   
   BDBG_LEAVE(BVEE_Channel_GetBufferDescriptors);
   return BERR_TRACE( BERR_SUCCESS );
}

 
 BERR_Code BVEE_Channel_ConsumeBufferDescriptors(
    BVEE_ChannelHandle handle,
    unsigned numBufferDescriptors
    )
 {
    BREG_Handle regHandle;
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t uiCDBReadOffset;
    uint32_t uiCDBEndOffset;
    uint32_t uiCDBBaseOffset;
    uint32_t uiCDBValidOffset;  
    uint32_t uiITBReadOffset;
    uint32_t uiITBEndOffset;
    uint32_t uiITBBaseOffset;
    uint32_t uiITBValidOffset;
    BVEE_ChannelOutputDescriptorInfo  *psOutputDescDetails;

    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);

    BDBG_ASSERT(numBufferDescriptors > 0);  

    BDBG_ENTER(BVEE_Channel_ConsumeBufferDescriptors);

    BDBG_MSG(("BVEE_Channel_ConsumeBufferDescriptors: uiNumBufferDescriptors = %d\n",numBufferDescriptors));
    regHandle = handle->devicehandle->regHandle;
    psOutputDescDetails = &handle->veeoutput;

    /* Read CDB Addresses */           
    uiCDBBaseOffset = BREG_Read32(regHandle, handle->contextmap.CDB_Base);
    uiCDBEndOffset = BREG_Read32(regHandle, handle->contextmap.CDB_End);           
    uiCDBValidOffset = BREG_Read32(regHandle, handle->contextmap.CDB_Valid);
    uiCDBReadOffset = BREG_Read32(regHandle, handle->contextmap.CDB_Read);
    uiCDBEndOffset++;


    /* Read ITB Addresses */
    uiITBBaseOffset = BREG_Read32(regHandle, handle->contextmap.ITB_Base);
    uiITBEndOffset = BREG_Read32(regHandle, handle->contextmap.ITB_End);
    uiITBValidOffset= BREG_Read32(regHandle, handle->contextmap.ITB_Valid);
    uiITBReadOffset= BREG_Read32(regHandle, handle->contextmap.ITB_Read); 
    uiITBEndOffset++;

    while ( numBufferDescriptors )
    {
        if( 0 != (psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA))
        {
             /*Do nothing for now*/
        }
        else if ( 0 == (psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS) )
        {
            /* Move CDB Read Offset */
            uiCDBReadOffset += psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiLength;
            if ( uiCDBReadOffset >= uiCDBEndOffset )
            {
                uiCDBReadOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
            }

            if ( 0 != (psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START ) )
            {
                /* Move ITB Read Offset */
                uiITBReadOffset += sizeof( BVEE_P_ITBEntry );
                if ( uiITBReadOffset >= uiITBEndOffset )
                {
                    uiITBReadOffset -= ( uiITBEndOffset - uiITBBaseOffset );
                }
            }
        }

        /* Move Descriptor Read Offset */
        psOutputDescDetails->uiDescriptorReadOffset++;
        psOutputDescDetails->uiDescriptorReadOffset %= BVEE_MAX_ITBDESCRIPTORS;

        numBufferDescriptors--;
    }

    BDBG_MSG(("UpdateBufferDescriptors :uiDescriptorReadOffset = %d\n",\
              psOutputDescDetails->uiDescriptorReadOffset));
    
    /* Update Actual ITB/CDB Read Pointers */
    BREG_Write32(regHandle, handle->contextmap.CDB_Read, uiCDBReadOffset);
    BREG_Write32(regHandle, handle->contextmap.ITB_Read, uiITBReadOffset);

    BDBG_LEAVE(BVEE_Channel_ConsumeBufferDescriptors);
    
    return ret;    
}
 BERR_Code BVEE_Channel_GetBufferStatus(
   BVEE_ChannelHandle handle,
   BAVC_VideoBufferStatus *pBufferStatus
   )
 {
     BDBG_ENTER(BVEE_Channel_GetBufferStatus);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     BDBG_ASSERT(NULL != pBufferStatus);
 
     pBufferStatus->stCommon.pFrameBufferBaseAddress = handle->veeoutput.pCdbBaseCached;
     pBufferStatus->stCommon.pMetadataBufferBaseAddress = handle->veeoutput.pstMetadataCached;
 
     BDBG_LEAVE(BVEE_Channel_GetBufferStatus);
     return BERR_SUCCESS;
 }   
 
 BERR_Code BVEE_Channel_GetDataSyncSettings(
   BVEE_ChannelHandle handle,
   BVEE_DatasyncSettings *EncDataSyncSettings
   )
 {
     BERR_Code   ret = BERR_SUCCESS;
     BDSP_VideoEncodeTaskDatasyncSettings  DataSyncSettings;
     BDBG_ENTER(BVEE_Channel_GetDataSyncSettings);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     
     ret = BDSP_VideoEncodeTask_GetDatasyncSettings(handle->task, 
                0, 0, &DataSyncSettings);
     if(ret != BERR_SUCCESS)
     {
        BDBG_ERR(("Err in BVEE_Channel_GetDataSyncSettings"));
     }
     
     EncDataSyncSettings->eEnableStc = DataSyncSettings.eEnableStc;
     EncDataSyncSettings->ui32StcAddress = DataSyncSettings.ui32StcAddress;
     BDBG_LEAVE(BVEE_Channel_GetDataSyncSettings);
     return ret;
 }
 
 BERR_Code BVEE_Channel_SetDataSyncSettings(
   BVEE_ChannelHandle handle,
   BVEE_DatasyncSettings EncDataSyncSettings
   )
 {
     BERR_Code   ret = BERR_SUCCESS;
     BDSP_VideoEncodeTaskDatasyncSettings  DataSyncSettings;
     BDBG_ENTER(BVEE_Channel_SetDataSyncSettings);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);

     DataSyncSettings.eEnableStc = EncDataSyncSettings.eEnableStc;
     DataSyncSettings.ui32StcAddress = EncDataSyncSettings.ui32StcAddress;
     
     ret = BDSP_VideoEncodeTask_SetDatasyncSettings(handle->task, 
                0, 0, &DataSyncSettings);
     if(ret != BERR_SUCCESS)
     {
        BDBG_ERR(("Err in BVEE_Channel_SetDataSyncSettings"));
     }
     
     BDBG_LEAVE(BVEE_Channel_SetDataSyncSettings);
     return ret;
 }
 
 BERR_Code BVEE_Channel_AllocExtInterrupt(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle *pIntHandle
   )
 {
     BERR_Code   ret = BERR_SUCCESS;
     BDBG_ENTER(BVEE_Channel_AllocExtInterrupt);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     
     ret = BDSP_AllocateExternalInterrupt(handle->devicehandle->dspHandle, DSP_INDEX,(BDSP_ExternalInterruptHandle *)pIntHandle);
     if(ret != BERR_SUCCESS)
     {
        BDBG_ERR(("Err in BVEE_Channel_AllocExtInterrupt"));
     }
     
     BDBG_LEAVE(BVEE_Channel_AllocExtInterrupt);
     return ret;
 }
 
 BERR_Code BVEE_Channel_GetExtInterruptInfo(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle pIntHandle,
   BVEE_ExtInterruptInfo *pExtIntInfo 
   )
 {
     BERR_Code   ret = BERR_SUCCESS;
     BVEE_ExtInterruptInfo *pInfo ;
     BDBG_ENTER(BVEE_Channel_GetExtInterruptInfo);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     
     ret = BDSP_GetExternalInterruptInfo((BDSP_ExternalInterruptHandle)pIntHandle, (BDSP_ExternalInterruptInfo**)&pInfo);
     if(ret != BERR_SUCCESS)
     {
        BDBG_ERR(("Err in BVEE_Channel_GetExtInterruptInfo"));
     }
     *pExtIntInfo = *pInfo;
     BDBG_LEAVE(BVEE_Channel_GetExtInterruptInfo);
     return ret;
 }
 
 BERR_Code BVEE_Channel_FreeExtInterrupt(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle pIntHandle
   )
 {
     BERR_Code   ret = BERR_SUCCESS;
     BDBG_ENTER(BVEE_Channel_FreeExtInterrupt);
     BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
     
     ret = BDSP_FreeExternalInterrupt((BDSP_ExternalInterruptHandle)pIntHandle);
     if(ret != BERR_SUCCESS)
     {
        BDBG_ERR(("Err in BVEE_Channel_FreeExtInterrupt"));
     }
     
     BDBG_LEAVE(BVEE_Channel_FreeExtInterrupt);
     return ret;
 }
 void BVEE_Channel_GetTriggerInfo(
    BVEE_ChannelHandle handle,
    BVEE_ChannelTriggerInfo *pInfo  /* [out] */
    )
 {
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    BSTD_UNUSED(pInfo);
 }
 
 void BVEE_Channel_InitPictureDescriptor(
    BVEE_ChannelHandle handle,
    BVEE_PictureDescriptor *pPicture    /* [out] */
    )
 {
    BDBG_OBJECT_ASSERT(handle, BVEE_Channel);
    BSTD_UNUSED(pPicture);
 }
