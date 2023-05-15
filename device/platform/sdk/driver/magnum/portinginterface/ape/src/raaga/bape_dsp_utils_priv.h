/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_dsp_utils_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 5/3/12 5:06p $
 *
 * Module Description: DSP Utility Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_dsp_utils_priv.h $
 * 
 * Hydra_Software_Devel/13   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/12   4/27/12 1:37p jgarrett
 * SW7425-2802: Adding runtime capabilities routine
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/11   2/13/12 9:10a jgarrett
 * SW7425-2268: Refactoring mono output programming to use a single buffer
 * 
 * Hydra_Software_Devel/10   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/9   6/20/11 3:58p jgarrett
 * SW7425-408: Adding MS usage case restriction regarding AAC-HE simul
 * passthrough due to sample rate conversion in the decoder
 * 
 * Hydra_Software_Devel/8   6/14/11 3:45p jgarrett
 * SW7425-406: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-406/1   6/3/11 6:40p jgarrett
 * SW7425-406: Adding src details routines for rave/dfifo
 * 
 * Hydra_Software_Devel/7   5/27/11 11:20a jgarrett
 * SW7425-408: Revising format propagation logic
 * 
 * Hydra_Software_Devel/6   5/26/11 6:10p jgarrett
 * SW7425-408: Making generic passthru optional
 * 
 * Hydra_Software_Devel/5   5/18/11 6:50p jgarrett
 * SW7425-408: Adding preliminary DDRE support
 * 
 * Hydra_Software_Devel/4   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/3   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/2   3/1/11 5:13p jgarrett
 * SW7422-146: Fixing post-processing output modes
 * 
 * Hydra_Software_Devel/1   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 ***************************************************************************/

#include "bape_types.h"
#include "bdsp.h"
#include "bdsp_task.h"
#include "bdsp_audio_task.h"

#ifndef BAPE_DSP_UTILS_PRIV_H_
#define BAPE_DSP_UTILS_PRIV_H_

/***************************************************************************
Summary:
Codec Attribute Table Entry
***************************************************************************/
typedef struct BAPE_CodecAttributes
{
    BAVC_AudioCompressionStd codec;
    BDSP_AudioType dspAudioType;
    BDSP_AudioEncode dspAudioEncode;
    const char *pName;
    BAPE_MultichannelFormat multichannelFormat;
    bool compressedOutputValid;
    bool srcRequired;               /* True if this codec supports non-standard sample rates such as LSF/QSF */
    bool genericPassthruRequired;   /* True if GenericPassthru should be added for this decode algorithm */
    bool simulValid;                /* True if this codec supports simul mode of decode + passthrough.  */
    bool monoOutputValid;           /* True if this codec supports mono output */
    bool compressed4xOutputValid;   /* True if compressed 4x is valid for this codec */
    bool compressed16xOutputValid;  /* True if compressed 16x is valid for this codec */
} BAPE_CodecAttributes;

/***************************************************************************
Summary:
Get Codec Attributes
***************************************************************************/
const BAPE_CodecAttributes *BAPE_P_GetCodecAttributes(
    BAVC_AudioCompressionStd codec
    );

#define BAPE_P_GetCodecName(codec) (BAPE_P_GetCodecAttributes((codec))->pName)
#define BAPE_P_GetCodecAudioType(codec) (BAPE_P_GetCodecAttributes((codec))->dspAudioType)
#define BAPE_P_GetCodecAudioEncode(codec) (BAPE_P_GetCodecAttributes((codec))->dspAudioEncode)
#define BAPE_P_GetCodecMultichannelFormat(codec) (BAPE_P_GetCodecAttributes((codec))->multichannelFormat)
#define BAPE_P_CodecSupportsPassthrough(codec) (BAPE_P_GetCodecAttributes((codec))->compressedOutputValid)
#define BAPE_P_CodecRequiresSrc(codec) (BAPE_P_GetCodecAttributes((codec))->srcRequired)
#define BAPE_P_CodecRequiresGenericPassthru(codec) (BAPE_P_GetCodecAttributes((codec))->genericPassthruRequired)
#define BAPE_P_CodecSupportsSimulMode(codec) (BAPE_P_GetCodecAttributes((codec))->compressedOutputValid && BAPE_P_GetCodecAttributes((codec))->simulValid)
#define BAPE_P_CodecSupportsMono(codec) (BAPE_P_GetCodecAttributes((codec))->monoOutputValid)
#define BAPE_P_CodecSupportsCompressed4x(codec) (BAPE_P_GetCodecAttributes((codec))->compressed4xOutputValid)
#define BAPE_P_CodecSupportsCompressed16x(codec) (BAPE_P_GetCodecAttributes((codec))->compressed16xOutputValid)

/***************************************************************************
Summary:
Define inter-task buffer size
***************************************************************************/
#define BAPE_P_INTER_TASK_BUFFER_SIZE (4+BDSP_AF_P_INTERTASK_IOBUFFER_SIZE)

/***************************************************************************
Summary:
Helper to print a variable name and value when assigning DSP settings structures
***************************************************************************/
#define BAPE_DSP_P_SET_VARIABLE(st,var,val) do { st.var = (val); BDBG_MSG(("%s: %s = %#x", __FUNCTION__, #var, (st.var))); } while (0)

/***************************************************************************
Summary:
Helper to determine if LFE should be enabled
***************************************************************************/
#define BAPE_DSP_P_IsLfePermitted(channelMode) ((channelMode) > BAPE_ChannelMode_e2_0?true:false)

/***************************************************************************
Summary:
Helper to determine appropriate decoder output mode
***************************************************************************/
BAPE_ChannelMode BAPE_DSP_P_GetChannelMode(BAVC_AudioCompressionStd codec, BAPE_ChannelMode outputMode, bool multichannelOutput, BAPE_MultichannelFormat maxFormat);

/***************************************************************************
Summary:
Helper to convert APE Channel Mode to DSP ACMOD
***************************************************************************/
uint32_t BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode outputMode);

/***************************************************************************
Summary:
Helper to setup channel matrix based on output mode and LFE
***************************************************************************/
void BAPE_DSP_P_GetChannelMatrix(BAPE_ChannelMode outputMode, bool lfe, uint32_t *pChannelMatrix);

/***************************************************************************
Summary:
Helper to setup true mono channel matrix, which is different from the 
typical mono->stereo output from a DSP node. 
***************************************************************************/
void BAPE_DSP_P_GetMonoChannelMatrix(uint32_t *pChannelMatrix);

/***************************************************************************
Summary:
Unit conversion routines
***************************************************************************/
uint32_t 
BAPE_P_FloatToQ131(
    int32_t floatVal, 
    unsigned int uiRange
    );

uint32_t 
BAPE_P_FloatToQ230(
    int16_t floatVar
    );

int32_t 
BAPE_P_FloatToQ923(
    uint32_t floatVar, 
    unsigned int uiRange
    );

uint32_t 
BAPE_P_FloatToQ521(
    uint32_t floatVar, 
    unsigned int uiRange
    );

int32_t 
BAPE_P_FloatToQ824(
    int32_t floatVar, 
    unsigned int uiRange
    );


int32_t 
BAPE_P_FloatToQ1022(
    int32_t floatVar,
    unsigned int uiRange
    );

uint32_t 
BAPE_P_FloatToQ518(
    uint32_t floatVar,
    unsigned int uiRange
    );

uint32_t 
    BAPE_P_FloatToQ815(
    uint32_t floatVar,
    unsigned int uiRange
    );

uint32_t 
    BAPE_P_FloatToQ527(
    uint32_t floatVar,
    unsigned int uiRange
    );

uint32_t 
    BAPE_P_FloatToQ329(
    uint32_t floatVal,
    unsigned int uiRange
    );

uint32_t 
    BAPE_P_FloatToQ428(
    uint32_t floatVal,
    unsigned int uiRange
    );

/***************************************************************************
Summary:
Initialize a DSP Branch Structure
***************************************************************************/
void BAPE_DSP_P_InitBranch(
    BDSP_CIT_P_FwBranchInfo *pBranch
    );

/***************************************************************************
Summary:
Add FMM Buffer Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddFmmBuffer(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    const BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Add Inter Task Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddInterTaskBuffer(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    const BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Add RAVE Buffer Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddRaveOutputBuffer(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    const BAVC_XptContextMap *pContextMap
    );

/***************************************************************************
Summary:
Get Connector Data Type 
***************************************************************************/
BDSP_AF_P_DistinctOpType BAPE_DSP_P_GetDataTypeFromConnector(
    BAPE_Connector connector
    );


/***************************************************************************
Summary:
Add a post-processing stage
 
Description: 
Important Note - This function can allocate memory due to the structure of 
the CIT.  If a branch is added, a new BRAP_CIT_P_FwBranchInfo structure 
will be added to the task create settings.  It is the responsibility of 
the caller to eventually free this structure. 
***************************************************************************/
BERR_Code BAPE_DSP_P_AddProcessingStage(
    BDSP_TaskCreateSettings *pSettings,     /* [in/out] Settings will be modified to add stage and possibly also a branch */
    unsigned sourceBranchId,                /* Source Branch ID */
    unsigned sourceStageId,                 /* Source Stage ID */
    BDSP_AF_P_DistinctOpType sourceDataType,/* Source Data Type */
    BDSP_AudioProcessing processingType,    /* Processing Type */
    bool forceBranch,                       /* If true, force a new branch for this stage */
    unsigned *pSinkBranchId,                /* [out] new stage branch id */
    unsigned *pSinkStageId                  /* [out] new stage stage id */
    );

/***************************************************************************
Summary:
Add an encode stage 
 
Description: 
Important Note - This function can allocate memory due to the structure of 
the CIT.  If a branch is added, a new BRAP_CIT_P_FwBranchInfo structure 
will be added to the task create settings.  It is the responsibility of 
the caller to eventually free this structure. 
***************************************************************************/
BERR_Code BAPE_DSP_P_AddEncodeStage(
    BDSP_TaskCreateSettings *pSettings,     /* [in/out] Settings will be modified to add stage and possibly also a branch */
    unsigned sourceBranchId,                /* Source Branch ID */
    unsigned sourceStageId,                 /* Source Stage ID */
    BDSP_AF_P_DistinctOpType sourceDataType,/* Source Data Type */
    BDSP_AudioEncode encodeType,            /* Encoding Type */
    BDSP_EncodeMode encodeMode,             /* Encoding Mode (realtime or non-realtime) */
    bool forceBranch,                       /* If true, force a new branch for this stage */
    unsigned *pSinkBranchId,                /* [out] new stage branch id */
    unsigned *pSinkStageId                  /* [out] new stage stage id */
    );

/***************************************************************************
Summary:
Build the path between a DSP node and one consumer 
***************************************************************************/
BERR_Code BAPE_DSP_P_AllocatePathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Configure the path between a DSP node and one consumer 
***************************************************************************/
BERR_Code BAPE_DSP_P_ConfigPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Start the path from a DSP node to one consumer 
***************************************************************************/
BERR_Code BAPE_DSP_P_StartPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Stop the path from a DSP node to one consumer 
***************************************************************************/
void BAPE_DSP_P_StopPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

/***************************************************************************
Summary:
Get DSP Input Details for a RAVE Context
***************************************************************************/
void BAPE_DSP_P_GetRaveSrcDetails(
    BAPE_Handle deviceHandle, 
    const BAVC_XptContextMap *pContextMap, 
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails
    );

/***************************************************************************
Summary:
Get DSP Input Details for a DFIFO
***************************************************************************/
void BAPE_DSP_P_GetDfifoSrcDetails(
    BAPE_Handle deviceHandle, 
    BAPE_DfifoGroupHandle dfifoGroup, 
    unsigned numChannelPairs,
    bool interleaveData,
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails
    );

#endif
