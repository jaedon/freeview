/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_input_port_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 5/10/12 6:03p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_input_port_priv.c $
 * 
 * Hydra_Software_Devel/18   5/10/12 6:03p gskerl
 * SW7425-2075: Clear InputPort's halted flag when attaching consumer.
 * Added BAPE_InputPort_P_Halt_isr() function.
 * 
 * Hydra_Software_Devel/17   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/4   5/3/12 3:44p gskerl
 * SW7425-2075: Moved inputPort format validation to after the
 * consumerAttached callback.
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/12/12 1:55p jgarrett
 * SW7425-2075: Revising InputPort format handling
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/11/12 3:24p gskerl
 * SW7425-2075: Removed unused BAPE_InputPort_P_FormatChanged_isr(), also
 * removed BERR_TRACE call from BAPE_InputPort_P_SetFormat_isr().
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/16   3/22/12 2:58p gskerl
 * SW7429-18: Added BAPE_InputPort_P_BurstPreambleToCodec() to eliminate
 * duplicate preamble parsing code.
 * 
 * Hydra_Software_Devel/15   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/14   2/2/12 7:10p gskerl
 * SW7429-18: Removed all SpdifRx_P_* functions becaused their
 * functionality has been moved into bape_spdif_input.c and
 * bape_mai_input.c.
 * 
 * Hydra_Software_Devel/13   1/17/12 2:50p gskerl
 * SW7429-18: Changed argument to BAPE_SpdifRx_P_GetRxStatus() from
 * BAPE_MaiInputFormatDetectionStatus to
 * BAPE_SpdifInputFormatDetectionStatus.
 * 
 * Hydra_Software_Devel/12   11/30/11 4:26p jgarrett
 * SW7429-18: Adding stubs for SPDIF/MAI inputs on 7429
 * 
 * Hydra_Software_Devel/11   8/12/11 7:05p sgadara
 * SWDTV-6584: [35233] Reorganize Input port FCI id extraction
 * 
 * Hydra_Software_Devel/10   8/12/11 4:20p venkatr
 * SWDTV-6584 : [35233] ADC input code cleanup
 * 
 * Hydra_Software_Devel/9   5/12/11 12:05p gskerl
 * SW7422-354: Fixed to support APE compilation on 35230
 * 
 * Hydra_Software_Devel/8   4/16/11 2:00p jgarrett
 * SW7425-371: Removing tabs
 * 
 * Hydra_Software_Devel/7   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/6   3/27/11 12:22p piyushg
 * SW7422-146: Removing dependency on AVC sample rate ENUM for getting the
 * MAI and SPDIF sample rate info.
 * 
 * Hydra_Software_Devel/5   3/24/11 12:10p piyushg
 * SW7422-364: HDMI input support - fixing some initial bugs and
 * compilation errors.
 * 
 * Hydra_Software_Devel/4   3/24/11 10:28a jgarrett
 * SW7422-364: Fixing compilation errors on systems without HDMI or SPDIF
 * input
 * 
 * Hydra_Software_Devel/3   3/23/11 11:07a piyushg
 * SW7422-364: Initial checking for HDMI input support.
 * 
 * Hydra_Software_Devel/2   3/1/11 3:25p jgarrett
 * SW7422-146: Moving sample rate to input port from input capture
 * 
 * Hydra_Software_Devel/1   1/7/11 5:21p jgarrett
 * SW7422-146: Adding SPDIF/HDMI input APIs
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bape_input.h"

BDBG_MODULE(bape_input_port);

BDBG_OBJECT_ID(BAPE_InputPort);

/* Preamble C defines */
#define  BURST_PREAM_C_ALGO_ID                              0x001f
#define  BURST_PREAM_C_ALGO_ID_AC3                               1
#define  BURST_PREAM_C_ALGO_ID_PAUSE                             3
#define  BURST_PREAM_C_ALGO_ID_MPEGL1                            4
#define  BURST_PREAM_C_ALGO_ID_MPEGL2L3                          5
#define  BURST_PREAM_C_ALGO_ID_MPEG2EXT                          6
#define  BURST_PREAM_C_ALGO_ID_AAC                               7
#define  BURST_PREAM_C_ALGO_ID_MPEG2L1                           8
#define  BURST_PREAM_C_ALGO_ID_MPEG2L2                           9
#define  BURST_PREAM_C_ALGO_ID_MPEG2L3                          10
#define  BURST_PREAM_C_ALGO_ID_DTSI                             11
#define  BURST_PREAM_C_ALGO_ID_DTSII                            12
#define  BURST_PREAM_C_ALGO_ID_DTSIII                           13
#define  BURST_PREAM_C_ALGO_ID_DTSIV                            17
#define  BURST_PREAM_C_ALGO_ID_MPEG2AAC_LSF                     19
#define  BURST_PREAM_C_ALGO_ID_MPEG4AAC                         20
#define  BURST_PREAM_C_ALGO_ID_AC3Enhanced                      21
#define  BURST_PREAM_C_ALGO_ID_MAT                              22

#define  BURST_PREAM_C_PAYLOAD_MAY_CONTAIN_ERRORS           0x0080
#define  BURST_PREAM_C_DATA_TYPE_DEPENDENT_INFO             0x1f00
#define  BURST_PREAM_C_BIT_STREAM_NUMBER                    0xD000


BERR_Code BAPE_InputPort_P_AttachConsumer(
    BAPE_InputPort inputPort,
    BAPE_PathNode *pConsumer,
    BAPE_FMT_Descriptor *pInputFormat   /* [out] Current format */
    )
{
    BERR_Code errCode = BERR_SUCCESS;
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BDBG_OBJECT_ASSERT(pConsumer, BAPE_PathNode);
    if ( NULL != inputPort->pConsumer )
    {
        BDBG_ERR(("Can only have one consumer for any input port"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    BDBG_MSG(("Attached consumer %s to input port %s", pConsumer->pName, inputPort->pName));
    BKNI_EnterCriticalSection();
    inputPort->halted = false;  /* just in case we had been halted previously. */
    inputPort->pConsumer = pConsumer;
    inputPort->consumerAttaching = true;
    if ( inputPort->consumerAttached_isr )
    {
        errCode = inputPort->consumerAttached_isr(inputPort, pConsumer, pInputFormat);
        if ( errCode )
        {
            inputPort->pConsumer = NULL;
            /* Will return error below after leaving critical section */
        }
    }
    else
    {
        *pInputFormat = inputPort->format;
    }
    inputPort->consumerAttaching = false;
    BKNI_LeaveCriticalSection();
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Now check to make sure that the consumer can handle the inputPort's current format. */
    if ( !BAPE_FMT_P_FormatSupported(&pConsumer->inputCapabilities, &inputPort->format) )
    {
        BDBG_ERR(("Node %s can not accept this InputPort's data format (%s)", pConsumer->pName, BAPE_FMT_P_GetTypeName(&inputPort->format)));

        errCode = BAPE_InputPort_P_DetachConsumer(inputPort, pConsumer);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_InputPort_P_DetachConsumer(
                                         BAPE_InputPort inputPort,
                                         BAPE_PathNode *pConsumer
                                         )
{
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BDBG_OBJECT_ASSERT(pConsumer, BAPE_PathNode);

    if ( pConsumer != inputPort->pConsumer )
    {

        BDBG_ERR(("Can only have one consumer for any input port"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_MSG(("Detached consumer %s from input port %s", pConsumer->pName, inputPort->pName));
    BKNI_EnterCriticalSection();
    if ( inputPort->consumerDetached_isr )
    {
        inputPort->consumerDetached_isr(inputPort, inputPort->pConsumer);
    }
    inputPort->pConsumer = NULL;
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;    
}

void BAPE_InputPort_P_GetFciIds(
    BAPE_InputPort inputPort,
    BAPE_FciIdGroup *pFciGroup      /* [out] */
    )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);

    for ( i = 0; i < BAPE_FMT_P_GetNumChannelPairs(&inputPort->format); i++ )
    {
        pFciGroup->ids[i] = (0x6<<6) | inputPort->streamId[i];
    }
    for ( ; i < BAPE_ChannelPair_eMax; i++ )
    {
        pFciGroup->ids[i] = BAPE_FCI_ID_INVALID;
    }
}

void BAPE_InputPort_P_BurstPreambleToCodec(uint32_t burstPreamble, BAVC_AudioCompressionStd *codec )
{
    switch (burstPreamble & BURST_PREAM_C_ALGO_ID)  /* Mask off unrelated bits. */
    {
    case BURST_PREAM_C_ALGO_ID_AC3:
        *codec = BAVC_AudioCompressionStd_eAc3;
        break;

    case BURST_PREAM_C_ALGO_ID_AAC:
    case BURST_PREAM_C_ALGO_ID_MPEG2AAC_LSF:
    case BURST_PREAM_C_ALGO_ID_MPEG4AAC:
        *codec = BAVC_AudioCompressionStd_eAac;
        break;

    case BURST_PREAM_C_ALGO_ID_MPEGL1:
    case BURST_PREAM_C_ALGO_ID_MPEGL2L3:
    case BURST_PREAM_C_ALGO_ID_MPEG2EXT:
    case BURST_PREAM_C_ALGO_ID_MPEG2L1:
    case BURST_PREAM_C_ALGO_ID_MPEG2L2:
    case BURST_PREAM_C_ALGO_ID_MPEG2L3:
        *codec = BAVC_AudioCompressionStd_eMpegL3;
        break;

    case BURST_PREAM_C_ALGO_ID_DTSI:
    case BURST_PREAM_C_ALGO_ID_DTSII:
    case BURST_PREAM_C_ALGO_ID_DTSIII:
        *codec = BAVC_AudioCompressionStd_eDts;
        break;

    case BURST_PREAM_C_ALGO_ID_DTSIV:
        *codec = BAVC_AudioCompressionStd_eDtshd;
        break;

    case BURST_PREAM_C_ALGO_ID_AC3Enhanced:
        *codec = BAVC_AudioCompressionStd_eAc3Plus;
        break;

    case BURST_PREAM_C_ALGO_ID_MAT:
        *codec = BAVC_AudioCompressionStd_eMlp;
        break;

    default:
        *codec = BAVC_AudioCompressionStd_eMax;
        break;
    } /* End Switch */  
}


void BAPE_InputPort_P_GetFormat_isr(
    BAPE_InputPort inputPort,
    BAPE_FMT_Descriptor *pFormat    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BDBG_ASSERT(NULL != pFormat);

    BKNI_ASSERT_ISR_CONTEXT();

    *pFormat = inputPort->format;
}

BERR_Code BAPE_InputPort_P_SetFormat_isr(
    BAPE_InputPort inputPort,
    const BAPE_FMT_Descriptor *pNewFormat
    )
{
    BERR_Code errCode;
    BAPE_FMT_Descriptor oldFormat;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BDBG_ASSERT(NULL != pNewFormat);

    BKNI_ASSERT_ISR_CONTEXT();

    oldFormat = inputPort->format;
    inputPort->format = *pNewFormat;

    if ( inputPort->pConsumer &&
         inputPort->pConsumer->inputPortFormatChange_isr &&
         !inputPort->consumerAttaching )
    {
        errCode = inputPort->pConsumer->inputPortFormatChange_isr(inputPort->pConsumer, inputPort);
        if ( errCode )
        {
            inputPort->format = oldFormat;
            return(errCode); /* BERR_TRACE intentionally omitted */
        }
    }

    return BERR_SUCCESS;
}
BERR_Code BAPE_InputPort_P_Halt_isr(
    BAPE_InputPort inputPort
    )
{
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BKNI_ASSERT_ISR_CONTEXT();

    inputPort->halted = true;

    return BERR_SUCCESS;
}

