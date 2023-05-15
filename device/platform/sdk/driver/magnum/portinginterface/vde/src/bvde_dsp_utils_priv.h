/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvde_dsp_utils_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/31/11 4:44p $
 *
 * Module Description: DSP Utility Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vde/7425/bvde_dsp_utils_priv.h $
 * 
 * Hydra_Software_Devel/1   5/31/11 4:44p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 ***************************************************************************/

#include "bvde_priv.h"
#include "bdsp.h"
#include "bdsp_task.h"
#include "bdsp_audio_task.h"

#ifndef BVDE_DSP_UTILS_PRIV_H_
#define BVDE_DSP_UTILS_PRIV_H_

/***************************************************************************
Summary:
Codec Attribute Table Entry
***************************************************************************/
typedef struct BVDE_CodecAttributes
{
    BAVC_VideoCompressionStd codec;
    BDSP_VideoType dspVideoType;
    const char *pName;
} BVDE_CodecAttributes;

/***************************************************************************
Summary:
Get Codec Attributes
***************************************************************************/
const BVDE_CodecAttributes *BVDE_P_GetCodecAttributes(
    BAVC_VideoCompressionStd codec
    );


#define BVDE_P_GetCodecName(codec) (BVDE_P_GetCodecAttributes((codec))->pName)
#define BVDE_P_GetCodecVideoType(codec) (BVDE_P_GetCodecAttributes((codec))->dspVideoType)


/***************************************************************************
Summary:
Helper to print a variable name and value when assigning DSP settings structures
***************************************************************************/
#define BVDE_DSP_P_SET_VARIABLE(st,var,val) do { st.var = (val); BDBG_MSG(("%s: %s = %#x", __FUNCTION__, #var, (st.var))); } while (0)


/***************************************************************************
Summary:
Initialize a DSP Branch Structure
***************************************************************************/
void BVDE_DSP_P_InitBranch(
    BDSP_CIT_P_FwBranchInfo *pBranch
    );

/***************************************************************************
Summary:
Add RAVE Buffer Output to a stage
***************************************************************************/
BERR_Code BVDE_DSP_P_AddRaveOutputBuffer(
    BDSP_CIT_P_FwBranchInfo *pBranch,
    unsigned stageId,
    const BAVC_XptContextMap *pContextMap
    );


#endif
