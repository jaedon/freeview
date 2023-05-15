/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_resources.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 5/3/12 5:05p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_resources.c $
 * 
 * Hydra_Software_Devel/11   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/3   5/3/12 1:58p gskerl
 * SW7425-2075: Added garbage collection and retry loop to
 * BAPE_P_AllocateBuffers().
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 3:51p jgarrett
 * SW7425-2075: Adding debug, fixing buffer logic
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/10   11/14/11 3:41p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/9   9/19/11 3:24p gskerl
 * SW7231-129: Renamed BAPE_P_GarbageCollect() to
 * BAPE_P_ReleaseUnusedPathResources() and made it a non-static function.
 * 
 * Hydra_Software_Devel/8   4/16/11 12:16p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   4/14/11 6:18p jgarrett
 * SW7231-125: Adding debug for buffer allocation
 * 
 * Hydra_Software_Devel/6   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/5   3/11/11 6:02p jgarrett
 * SW7422-146: Decoder supports external inputs
 * 
 * Hydra_Software_Devel/4   3/4/11 10:37a jgarrett
 * SW7422-146: Fixing crash on start/stop/start
 * 
 * Hydra_Software_Devel/3   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/2   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_aud_fmm_src_ctrl0.h"
#include "bchp_aud_fmm_bf_ctrl.h"

BDBG_MODULE(bape_resources);

/*************************************************************************** 
  The resource pools are managed in a simple manner to permit grouping and
  attempt to avoid fragmentation of resources.  For any given resource,
  if only one channel pair is required (not grouped), resources are allocated
  from the bottom of the pool (0..n).  For grouped resources, they are
  allocated from the top of the pool (n..0).  
***************************************************************************/

void BAPE_P_ReleaseUnusedPathResources(BAPE_Handle handle)
{
    unsigned i;

#if BAPE_CHIP_MAX_DECODERS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
    {
        if ( NULL != handle->decoders[i] )
        {
            BAPE_PathNode_P_ReleasePathResources(&handle->decoders[i]->node);
        }
    }
#endif
#if BAPE_CHIP_MAX_INPUT_CAPTURES > 0
    for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++ )
    {
        if ( NULL != handle->inputCaptures[i] )
        {
            BAPE_PathNode_P_ReleasePathResources(&handle->inputCaptures[i]->node);
        }
    }
#endif
#if BAPE_CHIP_MAX_PLAYBACKS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++ )
    {
        if ( NULL != handle->playbacks[i] )
        {
            BAPE_PathNode_P_ReleasePathResources(&handle->playbacks[i]->node);
        }
    }
#endif
}

BERR_Code BAPE_P_AllocateInputBuffers(BAPE_Handle handle, BAPE_Connector input)
{
    unsigned i, numChannelPairs;
    bool needToAllocate=false;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    BDBG_ASSERT(input->useBufferPool);

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&input->format);

    /* Make sure we actually need buffers.  This may be called multiple times. */
    for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
    {
        if ( (i < numChannelPairs && NULL == input->pBuffers[i]) ||
             (i >= numChannelPairs && NULL != input->pBuffers[i]) )
        {
            needToAllocate=true;
            break;
        }
        else if ( input->pBuffers[i] && !BAPE_FMT_P_FormatSupported(&handle->buffers[input->pBuffers[i]->poolIndex].capabilities, &input->format))
        {
            needToAllocate=true;
            break;
        }
    }

    if ( needToAllocate )
    {
        BDBG_MSG(("Need to allocate buffers for %s %s", input->pParent->pName, input->pName));
        BAPE_P_FreeInputBuffers(handle, input);
    }
    else
    {
        BDBG_MSG(("Do not need to allocate buffers for %s %s", input->pParent->pName, input->pName));
        return BERR_SUCCESS;
    }

    errCode = BAPE_P_AllocateBuffers(handle, &input->format, input->pBuffers);
    if ( errCode )
    {
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    return BERR_SUCCESS;
}

void BAPE_P_FreeInputBuffers(BAPE_Handle handle, BAPE_Connector input)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    BDBG_ASSERT(input->useBufferPool);

    BDBG_MSG(("Free buffers for %s %s", input->pParent->pName, input->pName));
    BAPE_P_FreeBuffers(handle, input->pBuffers);
}

BERR_Code BAPE_P_AllocateBuffers(
    BAPE_Handle deviceHandle,
    const BAPE_FMT_Descriptor *pDesc, 
    BAPE_BufferNode *pBuffers[BAPE_ChannelPair_eMax]
    )
{
    unsigned pool, i, numChannelPairs;
    unsigned attempt;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pDesc);
    BDBG_ASSERT(NULL != pBuffers);

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(pDesc);

    /* Try twice to acquire resources and garbage collect in between if resources are unavailable */
    for ( attempt=0; attempt<2; attempt++ )
    {
        for ( pool = 0; pool < BAPE_MAX_BUFFER_POOLS; pool++ )
        {
            /* If this pool can support the format and has sufficient buffers... */
            if ( BAPE_FMT_P_FormatSupported(&deviceHandle->buffers[pool].capabilities, pDesc) && 
                 deviceHandle->buffers[pool].numFreeBuffers >= numChannelPairs )
            {
                /* Grab the required buffers */
                for ( i = 0; i < numChannelPairs; i++ )
                {
                    BDBG_ASSERT(NULL == pBuffers[i]);
                    pBuffers[i] = BLST_S_FIRST(&deviceHandle->buffers[pool].freeList);
                    BDBG_ASSERT(NULL != pBuffers[i]);

                    BLST_S_REMOVE_HEAD(&deviceHandle->buffers[pool].freeList, node);
                    BLST_S_INSERT_HEAD(&deviceHandle->buffers[pool].allocatedList, pBuffers[i], node);
                    deviceHandle->buffers[pool].numFreeBuffers--;
                    pBuffers[i]->allocated = true;

                    BDBG_MSG(("Allocated Buffer node %#x",pBuffers[i]));
                }
                return BERR_SUCCESS;
            }
        }

        /* If we reached here, the attempt failed.  Try to garbage collect. */
        BAPE_P_ReleaseUnusedPathResources(deviceHandle);
    }
    /* If we reach here, we don't have enough buffers to fulfill the request. */
    BDBG_ERR(("Insufficient buffers available to support output format %s.  Buffers can be increased in BAPE_Settings.", BAPE_FMT_P_GetTypeName(pDesc)));
    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
}

void BAPE_P_FreeBuffers(
    BAPE_Handle deviceHandle,
    BAPE_BufferNode *pBuffers[BAPE_ChannelPair_eMax]
    )
{
    unsigned i, pool;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pBuffers);

    for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
    {
        if ( pBuffers[i] )
        {
            BDBG_MSG(("Free Buffer node %#x",pBuffers[i]));
            BDBG_OBJECT_ASSERT(pBuffers[i], BAPE_BufferNode);
            pool = pBuffers[i]->poolIndex;
            BDBG_ASSERT(pool < BAPE_MAX_BUFFER_POOLS);

            BLST_S_REMOVE(&deviceHandle->buffers[pool].allocatedList, pBuffers[i], BAPE_BufferNode, node);
            BLST_S_INSERT_HEAD(&deviceHandle->buffers[pool].freeList, pBuffers[i], node);
            deviceHandle->buffers[pool].numFreeBuffers++;
            pBuffers[i]->allocated = false;
            pBuffers[i] = NULL;
        }
    }
}

static BERR_Code BAPE_P_AllocateResourceGroup(BAPE_Handle handle, bool *pResourceArray, unsigned arraySize, unsigned numRequired, unsigned *pFirstResource)
{
    unsigned i, j, attempt;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != pResourceArray);
    BDBG_ASSERT(arraySize > 0);
    BDBG_ASSERT(numRequired > 0);
    BDBG_ASSERT(NULL != pFirstResource);

    /* Try twice to acquire resources and garbage collect in between if resources are unavailable */
    for ( attempt=0; attempt<2; attempt++ )
    {
        /* Allocate grouped resources from the bottom of the pool and un-grouped resources from the top */
        if ( numRequired > 1 )
        {
            /* Grouped */
            for ( i = arraySize-1; i >= numRequired; i-- )
            {
                bool success = true;
                for ( j = 0; j < numRequired; j++ )
                {
                    if ( pResourceArray[i-j] )
                    {
                        success = false;
                        break;
                    }
                }
                if ( !success )
                {
                    /* Not enough consecutive resources */
                    continue;
                }
                /* mark them as used */
                for ( j = 0; j < numRequired; j++ )
                {
                    pResourceArray[i-j] = true;
                }
                *pFirstResource = i - (numRequired-1);
                return BERR_SUCCESS;
            }
        }
        else
        {
            /* Ungrouped */
            for ( i = 0; i < arraySize; i++ )
            {
                if ( !pResourceArray[i] )
                {
                    /* We found an available resource.  Mark it as used. */
                    pResourceArray[i] = true;
                    *pFirstResource = i;
                    return BERR_SUCCESS;
                }
            }
        }

        /* If we reached here, the attempt failed.  Try to garbage collect. */
        BAPE_P_ReleaseUnusedPathResources(handle);
    }

    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

static BERR_Code BAPE_P_GetFmmResourceArray(BAPE_Handle handle, BAPE_FmmResourceType resourceType, bool **ppArray, unsigned *pSize)
{
    bool *pResourceArray;
    unsigned arraySize;
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != ppArray);
    BDBG_ASSERT(NULL != pSize);

    switch ( resourceType )
    {
    case BAPE_FmmResourceType_eSfifo:
        pResourceArray = handle->sfifoAllocated;
        arraySize = BAPE_CHIP_MAX_SFIFOS;
        break;
    case BAPE_FmmResourceType_eDfifo:
        pResourceArray = handle->dfifoAllocated;
        arraySize = BAPE_CHIP_MAX_DFIFOS;
        break;
    case BAPE_FmmResourceType_eSrc:
        pResourceArray = handle->srcAllocated;
        arraySize = BAPE_CHIP_MAX_SRCS;
        break;
    case BAPE_FmmResourceType_eMixer:
        pResourceArray = handle->mixerAllocated;
        arraySize = BAPE_CHIP_MAX_MIXERS;
        break;
    case BAPE_FmmResourceType_ePlayback:
        pResourceArray = handle->playbackAllocated;
        arraySize = BAPE_CHIP_MAX_MIXER_PLAYBACKS;
        break;
    case BAPE_FmmResourceType_eDummysink:
        pResourceArray = handle->dummysinkAllocated;
        arraySize = BAPE_CHIP_MAX_DUMMYSINKS;
        break;
    case BAPE_FmmResourceType_eLoopback:
        pResourceArray = handle->loopbackAllocated;
        arraySize = BAPE_CHIP_MAX_LOOPBACKS;
        break;  
#if BAPE_CHIP_MAX_FS > 0
    case BAPE_FmmResourceType_eFs:
        pResourceArray = handle->fsAllocated;
        arraySize = BAPE_CHIP_MAX_FS;
        break;
#endif
    case BAPE_FmmResourceType_eAdaptiveRate:
        pResourceArray = handle->adaptRateAllocated;
        arraySize = BAPE_CHIP_MAX_ADAPTRATE_CONTROLLERS;
        break;
    default:
        BDBG_ERR(("Unrecognized resource type %u", resourceType));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        pResourceArray = NULL;
        arraySize = 0;
        break;
    }
    *ppArray = pResourceArray;
    *pSize = arraySize;
    return errCode;
}

BERR_Code BAPE_P_AllocateFmmResource_tagged(BAPE_Handle handle, BAPE_FmmResourceType resourceType, unsigned numChannelPairs, unsigned *pFirstResource, const char *pFile, unsigned line)
{
    bool *pResourceArray=NULL;
    unsigned arraySize=0;
    BERR_Code errCode;

    BDBG_MSG(("Allocate resource %s:%u type %u num %u", pFile, line, resourceType, numChannelPairs));

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(numChannelPairs > 0);
    BDBG_ASSERT(NULL != pFirstResource);

    /* TODO: Add resource tracking to determine current and max resource usage since BAPE_Open() */

    errCode = BAPE_P_GetFmmResourceArray(handle, resourceType, &pResourceArray, &arraySize);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_P_AllocateResourceGroup(handle, pResourceArray, arraySize, numChannelPairs, pFirstResource);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_P_FreeResourceGroup(BAPE_Handle handle, bool *pResourceArray, unsigned arraySize, unsigned numResources, unsigned firstResource)
{
    unsigned i;
    
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != pResourceArray);
    BDBG_ASSERT(arraySize > 0);
    BDBG_ASSERT(numResources > 0);
    BDBG_ASSERT((firstResource + numResources) <= arraySize);
    
    for ( i = 0; i < numResources; i++ )
    {
        /* Ensure the resource isn't being freed twice */
        BDBG_ASSERT(pResourceArray[firstResource+i] == true);
        pResourceArray[firstResource+i] = false;
    }
}

void BAPE_P_FreeFmmResource(BAPE_Handle handle, BAPE_FmmResourceType resourceType, unsigned numChannelPairs, unsigned firstResource)
{
    bool *pResourceArray=NULL;
    unsigned arraySize=0;
    BERR_Code errCode;
    
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(numChannelPairs > 0);
    
    /* TODO: Add resource tracking to determine current and max resource usage since BAPE_Open() */
    
    errCode = BAPE_P_GetFmmResourceArray(handle, resourceType, &pResourceArray, &arraySize);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return;
    }
    
    BAPE_P_FreeResourceGroup(handle, pResourceArray, arraySize, numChannelPairs, firstResource);
}

