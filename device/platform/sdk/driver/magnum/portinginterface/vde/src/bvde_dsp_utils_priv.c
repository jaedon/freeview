/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvde_dsp_utils_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/16/11 10:36a $
 *
 * Module Description: DSP Utility Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vde/7425/bvde_dsp_utils_priv.c $
 * 
 * Hydra_Software_Devel/2   6/16/11 10:36a speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/1   5/31/11 4:44p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 ***************************************************************************/
#include "bvde.h"
#include "bvde_priv.h"
#include "bdsp_raaga.h"
#include "bvde_dsp_utils_priv.h"

BDBG_MODULE(bvde_dsp_utils);

static const BVDE_CodecAttributes g_codecAttributes[] =
{
/*   AVC Codec                              DSP Type                        Name            */
    {BAVC_VideoCompressionStd_eVP6,         BDSP_VideoType_eVP6,            "VP6",          },
    
    /* This entry must be last */
    {BAVC_VideoCompressionStd_eMax,         BDSP_VideoType_eMax,            "Unknown",      }
};

const BVDE_CodecAttributes *BVDE_P_GetCodecAttributes(
    BAVC_VideoCompressionStd codec
    )
{
    unsigned i, tableSize;

    tableSize = sizeof(g_codecAttributes)/sizeof(BVDE_CodecAttributes);

    for ( i = 0; i < tableSize; i++ )
    {
        if ( codec == g_codecAttributes[i].codec )
        {
            return &g_codecAttributes[i];
        }
    }

    return &g_codecAttributes[tableSize-1];
}

void BVDE_DSP_P_InitBranch(
    BDSP_CIT_P_FwBranchInfo *pBranch
    )
{
    BDBG_ASSERT(NULL != pBranch);
    BKNI_Memset(pBranch, 0, sizeof(BDSP_CIT_P_FwBranchInfo));
}

static BERR_Code BVDE_DSP_P_AddStageOutput(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    BDSP_AF_P_DistinctOpType outputType,
    unsigned *pOutputTypeIndex,                 /* [out] */
    unsigned *pOutputIndex,                     /* [out] */
    BDSP_CIT_P_FwStgSrcDstDetails **pDstDetails /* [out] */
    )
{
    unsigned outputTypeIndex, outputIndex;
    BDSP_CIT_P_FwStageInfo *pStage;

    BDBG_ASSERT(NULL != pBranch);
    BDBG_ASSERT(NULL != pOutputTypeIndex);
    BDBG_ASSERT(NULL != pOutputIndex);
    BDBG_ASSERT(NULL != pDstDetails);
    BDBG_ASSERT(stageId < pBranch->ui32NumStages);

    pStage = &pBranch->sFwStgInfo[stageId];

    for ( outputTypeIndex = 0; outputTypeIndex < pStage->sStgConnectivity.ui32NumDistinctOp; outputTypeIndex++ )
    {
        if ( pStage->sStgConnectivity.eDistinctOpType[outputTypeIndex] == outputType )
        {
            if ( pStage->sStgConnectivity.ui32NumDst[outputTypeIndex] >= BDSP_P_MAX_FW_STG_OUTPUTS )
            {
                BDBG_ERR(("Max number of outputs of type %u reached", outputType));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
            /* Add a destination to the existing output type */
            outputIndex = pStage->sStgConnectivity.ui32NumDst[outputTypeIndex];
            goto success;
        }
    }
    /* If we reach here, a distinct output of this type was not found.  Add a new one if we can. */
    if ( pStage->sStgConnectivity.ui32NumDistinctOp >= BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS )
    {
        BDBG_ERR(("Maximum number of distinct outputs exceeded"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    /* Add new output type and make this the first destination */
    pStage->sStgConnectivity.eDistinctOpType[outputTypeIndex] = outputType;
    pStage->sStgConnectivity.ui32NumDistinctOp = outputTypeIndex+1;
    outputIndex = 0;
    success:
    pStage->sStgConnectivity.ui32NumDst[outputTypeIndex] = outputIndex+1;
    *pOutputTypeIndex = outputTypeIndex;
    *pOutputIndex = outputIndex;
    *pDstDetails = &pStage->sStgConnectivity.sDstDetails[outputTypeIndex][outputIndex];
    return BERR_SUCCESS;
}

BERR_Code BVDE_DSP_P_AddRaveOutputBuffer(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    const BAVC_XptContextMap *pContextMap
    )
{    
    BERR_Code errCode;
    unsigned outputTypeIndex, outputIndex;
    BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails;

    BDBG_ASSERT(NULL != pBranch);
    /* Sanity Checks */
    BDBG_ASSERT(stageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(pBranch->sFwStgInfo[stageId].ui32StageId == stageId);
    BDBG_ASSERT(NULL != pContextMap);

    /* Unlike when used as an input, RAVE outputs are split into two distinct output types.
       One for the CDB buffer and one for the ITB buffer */

    /* Add CDB buffer output */
    errCode = BVDE_DSP_P_AddStageOutput(pBranch, stageId, BDSP_AF_P_DistinctOpType_eCdb, &outputTypeIndex, &outputIndex, &pDstDetails);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Initialize fields to invalid by default */
    BKNI_Memset(pDstDetails, 0, sizeof(*pDstDetails));
    pDstDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
    pDstDetails->uDetails.sIoBuf.ui32IndepDelay = 0;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRAVE;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = pContextMap->CDB_Base + BCHP_PHYSICAL_OFFSET;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = pContextMap->CDB_End + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = pContextMap->CDB_Read + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = pContextMap->CDB_Valid + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = pContextMap->CDB_Wrap + BCHP_PHYSICAL_OFFSET;

    /* Add ITB buffer output */
    errCode = BVDE_DSP_P_AddStageOutput(pBranch, stageId, BDSP_AF_P_DistinctOpType_eItb, &outputTypeIndex, &outputIndex, &pDstDetails);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    /* Initialize fields to invalid by default */
    BKNI_Memset(pDstDetails, 0, sizeof(*pDstDetails));
    pDstDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
    pDstDetails->uDetails.sIoBuf.ui32IndepDelay = 0;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRAVE;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = pContextMap->ITB_Base + BCHP_PHYSICAL_OFFSET;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = pContextMap->ITB_End + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = pContextMap->ITB_Read + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = pContextMap->ITB_Valid + BCHP_PHYSICAL_OFFSET;                                
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = pContextMap->ITB_Wrap + BCHP_PHYSICAL_OFFSET;                                                                

    return BERR_SUCCESS;
}
