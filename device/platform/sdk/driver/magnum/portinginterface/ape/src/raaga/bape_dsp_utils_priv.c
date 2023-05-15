/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_dsp_utils_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 6/26/12 3:45p $
 *
 * Module Description: DSP Utility Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_dsp_utils_priv.c $
 * 
 * Hydra_Software_Devel/37   6/26/12 3:45p jgarrett
 * SW7425-2911: Adding support for multichannel FLAC
 * 
 * Hydra_Software_Devel/36   6/1/12 5:08p jgarrett
 * SW7425-3065: Enabling FW SRC for LPCM-DVD/LPCM-BD/MLP/DTS
 * 
 * Hydra_Software_Devel/35   5/15/12 10:10a jgarrett
 * SW7425-3065: Changing LPCM-BD to use LPCM-DVD algorithm with framesync
 * option
 * 
 * Hydra_Software_Devel/34   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/4   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/33   4/27/12 1:37p jgarrett
 * SW7425-2802: Adding runtime capabilities routine
 * 
 * Hydra_Software_Devel/32   4/12/12 4:00p jgarrett
 * SW7425-2378: Adding Monkey's Audio
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/26/12 12:02p jgarrett
 * SW7425-2075: Fixing FMM buffer DistinctOPType values
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/9/12 2:04p jgarrett
 * SW7425-2075: Merging latest changes from main branch and enabling
 * 4x/16x OP Types
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/31   2/13/12 9:10a jgarrett
 * SW7425-2268: Refactoring mono output programming to use a single buffer
 * 
 * Hydra_Software_Devel/30   2/9/12 5:44p jgarrett
 * SW7425-2268: Correcting mono codec attributes, removing SRC for G.7xx
 * 
 * Hydra_Software_Devel/29   2/3/12 5:05p jgarrett
 * SW7425-2136: Mapping FLAC audio to the DSP
 * 
 * Hydra_Software_Devel/28   2/2/12 4:49p jgarrett
 * SW7425-2268: Adding EchoCanceller
 * 
 * Hydra_Software_Devel/27   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/26   12/29/11 1:14p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM
 * 
 * Hydra_Software_Devel/25   11/17/11 5:43p jgarrett
 * SW7425-1406: Merge to main branch
 * 
 * Hydra_Software_Devel/VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
 * SW7425-1406: Adding Vorbis support
 * 
 * Hydra_Software_Devel/24   8/24/11 11:53a jgarrett
 * SW7425-724: Adding RF Audio Encoder
 * 
 * Hydra_Software_Devel/23   8/17/11 11:11a jgarrett
 * SWDTV-8271: Reusing master DSP buffers if compressed/multichannel is
 * forced to PCM output
 * 
 * Hydra_Software_Devel/22   7/1/11 9:59a jgarrett
 * SW7405-5072: Adding WMA TS Support
 * 
 * Hydra_Software_Devel/21   6/28/11 4:11p jgarrett
 * SW7425-789: Programming 4x rate for DDP Passthrough
 * 
 * Hydra_Software_Devel/20   6/20/11 3:58p jgarrett
 * SW7425-408: Adding MS usage case restriction regarding AAC-HE simul
 * passthrough due to sample rate conversion in the decoder
 * 
 * Hydra_Software_Devel/19   6/14/11 3:45p jgarrett
 * SW7425-406: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-406/1   6/3/11 6:41p jgarrett
 * SW7425-406: Adding src details routines for rave/dfifo
 * 
 * Hydra_Software_Devel/18   6/1/11 6:31p jgarrett
 * SW7425-406: Adding Inter-Task buffer linkage
 * 
 * Hydra_Software_Devel/17   5/27/11 11:20a jgarrett
 * SW7425-408: Revising format propagation logic
 * 
 * Hydra_Software_Devel/16   5/26/11 6:09p jgarrett
 * SW7425-408: Disabling SRC for pulse and making generic passthru
 * optional
 * 
 * Hydra_Software_Devel/15   5/18/11 6:50p jgarrett
 * SW7425-408: Adding preliminary DDRE support
 * 
 * Hydra_Software_Devel/14   4/18/11 10:16p jgarrett
 * SW7425-361: Adding independent delay
 * 
 * Hydra_Software_Devel/13   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/12   4/17/11 12:29p jgarrett
 * SW7344-46: Adding error checks for licensing issues
 * 
 * Hydra_Software_Devel/11   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/10   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/9   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/8   3/9/11 4:07p jgarrett
 * SW7422-146: Coverity CID 273,274
 * 
 * Hydra_Software_Devel/7   3/4/11 2:18p jgarrett
 * SW7422-146: Initializing generic io buffer structure for outptus
 * 
 * Hydra_Software_Devel/6   3/1/11 5:13p jgarrett
 * SW7422-146: Fixing post-processing output modes
 * 
 * Hydra_Software_Devel/5   2/28/11 5:15p jgarrett
 * SW7422-146: Fixing warnings
 * 
 * Hydra_Software_Devel/4   2/28/11 4:50p jgarrett
 * SW7422-146: Adding TruVolume
 * 
 * Hydra_Software_Devel/3   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 ***************************************************************************/
#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_dsp_utils);

static const BAPE_CodecAttributes g_codecAttributes[] =
{
/*   AVC Codec                              DSP Type                        DSP Encode                     Name            Max multichannel format       SPDIF? SRC?  PassthruReq'd , Simul?, mono?,     4x?,   16x? */
    {BAVC_AudioCompressionStd_eMpegL1,      BDSP_AudioType_eMpeg,           BDSP_AudioEncode_eMpeg1Layer2, "MPEG",         BAPE_MultichannelFormat_e2_0, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eMpegL2,      BDSP_AudioType_eMpeg,           BDSP_AudioEncode_eMpeg1Layer2, "MPEG",         BAPE_MultichannelFormat_e2_0, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eMpegL3,      BDSP_AudioType_eMpeg,           BDSP_AudioEncode_eMpeg1Layer3, "MPEG",         BAPE_MultichannelFormat_e2_0, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eAc3,         BDSP_AudioType_eAc3,            BDSP_AudioEncode_eAc3,         "AC3",          BAPE_MultichannelFormat_e5_1, true,  false, false,         true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eAc3Plus,     BDSP_AudioType_eAc3Plus,        BDSP_AudioEncode_eMax,         "AC3+",         BAPE_MultichannelFormat_e5_1, true,  false, false,         true,   false,   false,   false},
#if BDSP_MS10_SUPPORT
    {BAVC_AudioCompressionStd_eAacAdts,     BDSP_AudioType_eAacAdts,        BDSP_AudioEncode_eAacHe,       "AAC ADTS",     BAPE_MultichannelFormat_e5_1, true,  false, true,          false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eAacLoas,     BDSP_AudioType_eAacLoas,        BDSP_AudioEncode_eAacHe,       "AAC LOAS",     BAPE_MultichannelFormat_e5_1, true,  false, true,          false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eAacPlusAdts, BDSP_AudioType_eAacSbrAdts,     BDSP_AudioEncode_eAacHe,       "AAC+ ADTS",    BAPE_MultichannelFormat_e5_1, true,  false, true,          false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eAacPlusLoas, BDSP_AudioType_eAacSbrLoas,     BDSP_AudioEncode_eAacHe,       "AAC+ LOAS",    BAPE_MultichannelFormat_e5_1, true,  false, true,          false,  false,   false,   false},
#else
    {BAVC_AudioCompressionStd_eAacAdts,     BDSP_AudioType_eAacAdts,        BDSP_AudioEncode_eAacHe,       "AAC ADTS",     BAPE_MultichannelFormat_e5_1, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eAacLoas,     BDSP_AudioType_eAacLoas,        BDSP_AudioEncode_eAacHe,       "AAC LOAS",     BAPE_MultichannelFormat_e5_1, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eAacPlusAdts, BDSP_AudioType_eAacSbrAdts,     BDSP_AudioEncode_eAacHe,       "AAC+ ADTS",    BAPE_MultichannelFormat_e5_1, true,  true,  true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eAacPlusLoas, BDSP_AudioType_eAacSbrLoas,     BDSP_AudioEncode_eAacHe,       "AAC+ LOAS",    BAPE_MultichannelFormat_e5_1, true,  true,  true,          true,   false,   false,   false},
#endif
    {BAVC_AudioCompressionStd_eDts,         BDSP_AudioType_eDtshd,          BDSP_AudioEncode_eDTS,         "DTS",          BAPE_MultichannelFormat_e5_1, true,  false, true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eDtshd,       BDSP_AudioType_eDtshd,          BDSP_AudioEncode_eMax,         "DTS-HD",       BAPE_MultichannelFormat_e7_1, true,  true,  true,          true,   false,   true,    true},
    {BAVC_AudioCompressionStd_eDtsLegacy,   BDSP_AudioType_eDtsBroadcast,   BDSP_AudioEncode_eMax,         "DTS-Legacy",   BAPE_MultichannelFormat_e5_1, true,  false, true,          true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eWmaStd,      BDSP_AudioType_eWmaStd,         BDSP_AudioEncode_eWma,         "WMA Std",      BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eWmaStdTs,    BDSP_AudioType_eWmaStd,         BDSP_AudioEncode_eWma,         "WMA Std TS",   BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eWmaPro,      BDSP_AudioType_eWmaPro,         BDSP_AudioEncode_eMax,         "WMA Pro",      BAPE_MultichannelFormat_e5_1, true,  false, false,         true,   false,   false,   false},
    {BAVC_AudioCompressionStd_eMlp,         BDSP_AudioType_eMlp,            BDSP_AudioEncode_eMax,         "MLP",          BAPE_MultichannelFormat_e7_1, true,  true,  true,          true,   false,   false,   true},
    {BAVC_AudioCompressionStd_ePcm,         BDSP_AudioType_ePcm,            BDSP_AudioEncode_eMax,         "PCM",          BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_ePcmWav,      BDSP_AudioType_ePcmWav,         BDSP_AudioEncode_eMax,         "PCM WAV",      BAPE_MultichannelFormat_e7_1, false, true,  false,         false,  true,    false,   false},
    {BAVC_AudioCompressionStd_eLpcmDvd,     BDSP_AudioType_eLpcmDvd,        BDSP_AudioEncode_eMax,         "LPCM DVD",     BAPE_MultichannelFormat_e5_1, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eLpcmBd,      BDSP_AudioType_eLpcmDvd,        BDSP_AudioEncode_eMax,         "LPCM BD",      BAPE_MultichannelFormat_e7_1, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eLpcm1394,    BDSP_AudioType_eLpcmDvd,        BDSP_AudioEncode_eMax,         "LPCM 1394",    BAPE_MultichannelFormat_e7_1, false, false, false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eAmr,         BDSP_AudioType_eAmr,            BDSP_AudioEncode_eMax,         "AMR",          BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eDra,         BDSP_AudioType_eDra,            BDSP_AudioEncode_eMax,         "DRA",          BAPE_MultichannelFormat_e5_1, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eCook,        BDSP_AudioType_eRealAudioLbr,   BDSP_AudioEncode_eMax,         "Cook",         BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eAdpcm,       BDSP_AudioType_eAdpcm,          BDSP_AudioEncode_eMax,         "ADPCM",        BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eVorbis,      BDSP_AudioType_eVorbis,         BDSP_AudioEncode_eMax,         "Vorbis",       BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eG711,        BDSP_AudioType_eG711G726,       BDSP_AudioEncode_eG711G726,    "G.711",        BAPE_MultichannelFormat_e2_0, false, false, false,         false,  true,    false,   false},
    {BAVC_AudioCompressionStd_eG723_1,      BDSP_AudioType_eG723_1,         BDSP_AudioEncode_eG723_1,      "G.723.1",      BAPE_MultichannelFormat_e2_0, false, false, false,         false,  true,    false,   false},
    {BAVC_AudioCompressionStd_eG726,        BDSP_AudioType_eG711G726,       BDSP_AudioEncode_eG711G726,    "G.726",        BAPE_MultichannelFormat_e2_0, false, false, false,         false,  true,    false,   false},
    {BAVC_AudioCompressionStd_eG729,        BDSP_AudioType_eG729,           BDSP_AudioEncode_eG729,        "G.729",        BAPE_MultichannelFormat_e2_0, false, false, false,         false,  true,    false,   false},
    {BAVC_AudioCompressionStd_eFlac,        BDSP_AudioType_eFlac,           BDSP_AudioEncode_eMax,         "FLAC",         BAPE_MultichannelFormat_e7_1, false, true,  false,         false,  false,   false,   false},
    {BAVC_AudioCompressionStd_eApe,         BDSP_AudioType_eMac,            BDSP_AudioEncode_eMax,         "APE Monkey",   BAPE_MultichannelFormat_e2_0, false, true,  false,         false,  false,   false,   false},
    /* This entry must be last */
    {BAVC_AudioCompressionStd_eMax,         BDSP_AudioType_eMax,            BDSP_AudioEncode_eMax,         "Unknown",      BAPE_MultichannelFormat_e2_0, false, false, false,         false,  false,   false,   false}
};

const BAPE_CodecAttributes *BAPE_P_GetCodecAttributes(
    BAVC_AudioCompressionStd codec
    )
{
    unsigned i, tableSize;

    tableSize = sizeof(g_codecAttributes)/sizeof(BAPE_CodecAttributes);

    for ( i = 0; i < tableSize; i++ )
    {
        if ( codec == g_codecAttributes[i].codec )
        {
            return &g_codecAttributes[i];
        }
    }

    return &g_codecAttributes[tableSize-1];
}

BAPE_ChannelMode BAPE_DSP_P_GetChannelMode(BAVC_AudioCompressionStd codec, BAPE_ChannelMode outputMode, bool multichannelOutput, BAPE_MultichannelFormat maxFormat)
{
    BAPE_MultichannelFormat multichannelFormat;
    BAPE_ChannelMode maxMode;

    multichannelFormat = BAPE_P_GetCodecMultichannelFormat(codec);

    switch ( multichannelFormat )
    {
    default:
    case BAPE_MultichannelFormat_e2_0:
        maxMode = BAPE_ChannelMode_e2_0;
        break;
    case BAPE_MultichannelFormat_e5_1:
        maxMode = BAPE_ChannelMode_e3_2;
        break;
    case BAPE_MultichannelFormat_e7_1:
        maxMode = BAPE_ChannelMode_e3_4;
        break;
    }

    /* Do we have any multichannel outputs? */
    if ( multichannelOutput )
    {
        /* Don't allow > the number of ringbuffers we allocate */
        switch ( maxFormat )
        {
        case BAPE_MultichannelFormat_e2_0:
            maxMode = BAPE_ChannelMode_e2_0;
            break;
        case BAPE_MultichannelFormat_e5_1:
            if ( maxMode > BAPE_ChannelMode_e3_2 )
            {
                maxMode = BAPE_ChannelMode_e3_2;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        maxMode = BAPE_ChannelMode_e2_0;
    }

    return (outputMode > maxMode)?maxMode:outputMode;
}

uint32_t BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode outputMode)
{
    switch ( outputMode )
    {
    case BAPE_ChannelMode_e1_1:
        return 0;
    case BAPE_ChannelMode_e1_0:
        return 1;
    default:
    case BAPE_ChannelMode_e2_0:
        return 2;
    case BAPE_ChannelMode_e3_0:
        return 3;
    case BAPE_ChannelMode_e2_1:
        return 4;
    case BAPE_ChannelMode_e3_1:
        return 5;
    case BAPE_ChannelMode_e2_2:
        return 6;
    case BAPE_ChannelMode_e3_2:
        return 7;
    }
}

void BAPE_DSP_P_GetChannelMatrix(
    BAPE_ChannelMode outputMode,
    bool lfe,
    uint32_t *pChannelMatrix
    )
{
    unsigned i=0;
    static const uint32_t defaultChannelMatrix[BAPE_ChannelMode_eMax][BDSP_AF_P_MAX_CHANNELS] = 
    {/*
     L,R,Ls,       ,Rs        ,C,        ,Lfe,      ,RLs       ,RRs  CHECKPOINT   */ 
    {4,4,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*1_0 */
    {0,1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*1_1 */
    {0,1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*2_0 */
    {0,1,0xFFFFFFFF,0xFFFFFFFF,4,         0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*3_0 */
    {0,1,2,         3,         0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*2_1 - Assumes mixing done in FW between LsRs */
    {0,1,0xFFFFFFFF,0xFFFFFFFF,4,         0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*3_1 */
    {0,1,2,         3,         0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*2_2 */
    {0,1,2,         3,         4,         0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},/*3_2 */
    {0,1,2,         3,         4,         0xFFFFFFFF,6,         7         },/*3_4 */
    };

    BDBG_ASSERT(outputMode < BAPE_ChannelMode_eMax);
    for(i = 0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    {
        pChannelMatrix[i] = defaultChannelMatrix[outputMode][i];
    }
    
    if(outputMode == BAPE_ChannelMode_e3_2)
    {
        if(true ==lfe)
        {
            pChannelMatrix[5] = 5;
        }
        else
        {
            pChannelMatrix[5] = 0xFFFFFFFF;
        }
    }
}

void BAPE_DSP_P_GetMonoChannelMatrix(uint32_t *pChannelMatrix)
{
    unsigned i;

    static const uint32_t monoChannelMatrix[BDSP_AF_P_MAX_CHANNELS] = 
        {0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
    
    for(i = 0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    {
        pChannelMatrix[i] = monoChannelMatrix[i];
    }
}

/*----------------------------------------------------------------------------*/
uint32_t BAPE_P_FloatToQ131(int32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    if (floatVar >= (int32_t)uiRange)
        return(uint32_t) 0x7FFFFFFF;

    if (floatVar <= (int32_t)-uiRange)
        return(uint32_t) 0x80000000;

    /* Conversion formula for float to Q1.31 is
     * q = float * 2^31,
     * Since we take float values scaled by uiRange from application, we need 
     * to scale it down
     * by uiRange. Hence above formula would become
     * q = float * ( 2^31/uiRange )
     * However this won't be a precise computation, as reminder of 
     * (2^31/uiRange) gets dropped
     * in this calculation. To compesate for this reminder formula needs to be 
     * modified as below
     * q = float * ( 2^31/uiRange ) + (float * (2^31 %uiRange))/uiRange
     */

    temp = floatVar * (0x80000000/uiRange) + 
           (floatVar * (0x80000000 % uiRange)) / uiRange;

    return temp; 
}

/*----------------------------------------------------------------------------*/
uint32_t BAPE_P_FloatToQ230(int16_t floatVar)
{
    int32_t     temp;

    BDBG_ASSERT(floatVar >= 0);
    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q2.30 is
     * q = float * 2^30,
     * Since we take float values scaled by 100 from application, we need to 
     * scale it down
     * by 100. Hence above formula would become
     * q = float * ( 2^30/100 )
     * However this won't be a precise computation, as reminder of (2^30/100) 
     * gets dropped
     * in this calculation. To compesate for this reminder formula needs to be 
     * modified as below
     * q = float * ( 2^30/100 ) + ( float * ( 2^30 %100))/100
     */
    if (floatVar >= 100)
        return(uint32_t) 0x40000000;

    temp = floatVar * (0x40000000/100) + ( floatVar * (0x40000000 % 100))/100;

    return temp; 
}


/*----------------------------------------------------------------------------*/
uint32_t BAPE_P_FloatToQ521(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q5.21 is
     * q = float * 2^26,
     * Since the entire range of values in PI is mapped to the range of 0 to 
     * 2^26 in FW, 
     * a given value in PI gets converted to corresponding Q5.21 value as below,
     * q = float * ( 2^26/uiRange )
     * However this won't be a precise computation, as remainder of 
     * (2^26/uiRange) gets dropped
     * in this calculation. To compensate for this remainder formula needs to be 
     * modified as below
     * q = float * ( 2^26/uiRange ) + ( float * ( 2^26 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns 
     * out to be 
     * a fractional value then the value gets rounded off to zero but if the 
     * value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula 
     * becomes,
     * q = float * ( 2^26/uiRange ) + (unsigned int)((float * ( 2^26 % uiRange) 
     * + (uiRange/2))/uiRange)
     */

    if (floatVar >= uiRange)
        return(uint32_t)0x03FFFFFF;

    temp = floatVar * (0x03FFFFFF/uiRange) + 
           (unsigned int)(( floatVar * (0x03FFFFFF % uiRange) + 
                            (uiRange/2))/uiRange);

    return temp; 
}


/*----------------------------------------------------------------------------*/
/*
    This function converts the floating point value to 8.24 fixed format
*/
int32_t BAPE_P_FloatToQ824(int32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /*Range should be multiple of 127 i.e. 127,254,381,508.....*/

    if (floatVar >= (int32_t)uiRange)
        return(uint32_t) 0x7F000000;

    if (floatVar <= -(int32_t)(uiRange + (uiRange/127)))
        return(uint32_t)0x80000000;

    temp = floatVar *   (0x7F000000/uiRange) + 
           (unsigned int)(( floatVar * (0x7F000000 % uiRange) + 
                            (uiRange/2))/uiRange);

    return temp; 
}


/*----------------------------------------------------------------------------*/
int32_t BAPE_P_FloatToQ923(uint32_t floatVar, unsigned int uiRange)
{
    int32_t  temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q9.23 is
     * q = float * 2^23,
     * Since the entire range of values in PI is mapped to the range of 0 to 
     * 2^23 in FW, 
     * a given value in PI gets converted to corresponding Q9.23 value as below,
     * q = float * ( 2^23/uiRange )
     * However this won't be a precise computation, as remainder of 
     * (2^23/uiRange) gets dropped
     * in this calculation. To compensate for this remainder, formula needs to 
     * be modified as below
     * q = float * ( 2^23/uiRange ) + ( float * ( 2^23 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns 
     * out to be 
     * a fractional value then the value gets rounded off to zero but if the 
     * value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula 
     * becomes,
     * q = float * ( 2^23/uiRange ) + (unsigned int)((float * ( 2^23 % uiRange) 
     * + (uiRange/2))/uiRange)
     */

    if (floatVar >= uiRange)
        return(uint32_t)0x007fffff;

    temp = floatVar *(0x007fffff/uiRange) + 
           (unsigned int)(((floatVar * (0x7fffff % uiRange)) + 
                           (uiRange/2)) / uiRange);

    return temp; 
}

/* Function to convert input floating point value into Q1.15 format
 * Intended floating point value to be converted, should be
 * multiplied by 100 and then passed to this function
 */
int32_t BAPE_P_FloatToQ1022(int32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q10.22 is
     * q = float * 2^22,
     * Since the entire range of values in PI is mapped to the range of 0 to 2^22 in FW, 
     * a given value in PI gets converted to corresponding Q10.22 value as below,
     * q = float * ( 2^22/uiRange )
     * However this won't be a precise computation, as remainder of (2^22/uiRange) gets dropped
     * in this calculation. To compensate for this remainder formula needs to be modified as below
     * q = float * ( 2^22/uiRange ) + ( float * ( 2^22 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns out to be 
     * a fractional value then the value gets rounded off to zero but if the value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula becomes,
     * q = float * ( 2^22/uiRange ) + (unsigned int)((float * ( 2^22 % uiRange) + (uiRange/2))/uiRange)
     */


    if (floatVar >= 0)
    {
        temp = floatVar * (0x003FFFFF/uiRange) + 
               (unsigned int)(( floatVar * (0x003FFFFF % uiRange) + 
                                (uiRange/2))/uiRange);
    }
    else
    {
        floatVar = (-1)*floatVar;

        temp = floatVar * (0x003FFFFF/uiRange) + 
               (unsigned int)(( floatVar * (0x003FFFFF % uiRange) + 
                                (uiRange/2))/uiRange);
        temp = (-1)*temp;
    }
    return temp;
}

/*
    This function converts the floating point value to 5.18 fixed format
*/
uint32_t BAPE_P_FloatToQ518(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q5.18 is
     * q = float * 2^23,
     * Since the entire range of values in PI is mapped to the range of 0 to 2^23 in FW, 
     * a given value in PI gets converted to corresponding Q5.18 value as below,
     * q = float * ( 2^23/uiRange )
     * However this won't be a precise computation, as remainder of (2^23/uiRange) gets dropped
     * in this calculation. To compensate for this remainder formula needs to be modified as below
     * q = float * ( 2^23/uiRange ) + ( float * ( 2^23 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns out to be 
     * a fractional value then the value gets rounded off to zero but if the value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula becomes,
     * q = float * ( 2^23/uiRange ) + (unsigned int)((float * ( 2^23 % uiRange) + (uiRange/2))/uiRange)
     */

    if (floatVar >= uiRange)
        return(uint32_t)0x007FFFFF;

    temp = floatVar * (0x007FFFFF/uiRange) + 
           (unsigned int)(( floatVar * (0x007FFFFF % uiRange) + 
                            (uiRange/2))/uiRange);

    return temp; 
}

/*
    This function converts the floating point value to 8.15 fixed format
*/
uint32_t BAPE_P_FloatToQ815(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q8.15 is
     * q = float * 2^23,
     * Since the entire range of values in PI is mapped to the range of 0 to 2^23 in FW, 
     * a given value in PI gets converted to corresponding Q8.15 value as below,
     * q = float * ( 2^23/uiRange )
     * However this won't be a precise computation, as remainder of (2^23/uiRange) gets dropped
     * in this calculation. To compensate for this remainder formula needs to be modified as below
     * q = float * ( 2^23/uiRange ) + ( float * ( 2^23 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns out to be 
     * a fractional value then the value gets rounded off to zero but if the value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula becomes,
     * q = float * ( 2^23/uiRange ) + (unsigned int)((float * ( 2^23 % uiRange) + (uiRange/2))/uiRange)
     */

    if (floatVar >= uiRange)
        return(uint32_t)0x007FFFFF;

    temp = floatVar * (0x007FFFFF/uiRange) + 
           (unsigned int)(( floatVar * (0x007FFFFF % uiRange) + 
                            (uiRange/2))/uiRange);

    return temp; 
}


/*
    This function converts the floating point value to 5.27 fixed format
*/
uint32_t BAPE_P_FloatToQ527(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* TODO: conversion for negative values */
    /* Conversion formula for float to Q5.27 is
     * q = float * 2^27,
     * Since the entire range of values in PI is mapped to the range of 0 to 2^27 in FW, 
     * a given value in PI gets converted to corresponding Q5.27 value as below,
     * q = float * ( 2^27/uiRange )
     * However this won't be a precise computation, as remainder of (2^27/uiRange) gets dropped
     * in this calculation. To compensate for this remainder formula needs to be modified as below
     * q = float * ( 2^27/uiRange ) + ( float * ( 2^27 % uiRange))/uiRange
     * But if the value corresponding to the multiplication of reminder turns out to be 
     * a fractional value then the value gets rounded off to zero but if the value is >= uiRange/2
     * it should be rounded off to 1(as per SRS). Hence forth the formula becomes,
     * q = float * ( 2^27/uiRange ) + (unsigned int)((float * ( 2^27 % uiRange) + (uiRange/2))/uiRange)
     * 0x08000000 = 2^27 + 1, the value 2^27 is being rounded off as that will be the precise value in
     * decoder implementation. Either ways of using 2^27 or 0x08000000 doesn't result in much difference.
     */

    temp = floatVar * (0x08000000/uiRange) + 
           (unsigned int)(( floatVar * (0x08000000 % uiRange) + 
                            (uiRange/2))/uiRange);

    return temp; 
}

/* Function to convert input floating point value into Q3.29 format
 * Intended floating point value to be converted, should be
 * multiplied by uiRange value and then passed to this function
 */
uint32_t BAPE_P_FloatToQ329(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* Conversion formula for float to Q3.29 is
     * q = float * 2^29,
     * Since we take float values scaled by uiRange from application, we need to scale it down
     * by uiRange. Hence above formula would become
     * q = float * ( 2^29/uiRange )
     * However this won't be a precise computation, as reminder of (2^29/uiRange) gets dropped
     * in this calculation. To compesate for this reminder formula needs to be modified as below
     * q = float * ( 2^29/uiRange ) + (float * (2^29 %uiRange))/uiRange
     */

    temp = floatVar * (0x1FFFFFFF/uiRange) + 
           ( floatVar * (0x1FFFFFFF % uiRange))/uiRange;

    return temp; 
}

/* Function to convert input floating point value into Q4.28 format
 * Intended floating point value to be converted, should be
 * multiplied by uiRange value and then passed to this function
 */
uint32_t BAPE_P_FloatToQ428(uint32_t floatVar, unsigned int uiRange)
{
    int32_t     temp;

    /* Conversion formula for float to Q4.28 is
     * q = float * 2^28,
     * Since we take float values scaled by uiRange from application, we need to scale it down
     * by uiRange. Hence above formula would become
     * q = float * ( 2^28/uiRange )
     * However this won't be a precise computation, as reminder of (2^28/uiRange) gets dropped
     * in this calculation. To compesate for this reminder formula needs to be modified as below
     * q = float * ( 2^28/uiRange ) + (float * (2^28 %uiRange))/uiRange
     */

    temp = floatVar * (0x10000000/uiRange) + 
           ( floatVar * (0x10000000 % uiRange))/uiRange;

    if (temp >= 0x40000000) return(uint32_t) 0x40000000;

    return temp; 
}

void BAPE_DSP_P_InitBranch(
                          BDSP_CIT_P_FwBranchInfo *pBranch
                          )
{
    BDBG_ASSERT(NULL != pBranch);
    BKNI_Memset(pBranch, 0, sizeof(BDSP_CIT_P_FwBranchInfo));
}

static BERR_Code BAPE_DSP_P_AddStageOutput(
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

/***************************************************************************
Summary:
Add FMM Buffer Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddFmmBuffer(
                                 BDSP_CIT_P_FwBranchInfo *pBranch,
                                 unsigned stageId,
                                 const BAPE_PathConnection *pConnection
                                 )
{    
    BERR_Code errCode;
    unsigned i, numChannels, numChannelPairs, outputTypeIndex, outputIndex;
    bool pcm;
    BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails;
    BDSP_AF_P_DistinctOpType outputType;
    BDSP_AF_P_FmmDstFsRate dstRate = BDSP_AF_P_FmmDstFsRate_eBaseRate;

    BDBG_ASSERT(NULL != pBranch);
    /* Sanity Checks */
    BDBG_ASSERT(stageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(pBranch->sFwStgInfo[stageId].ui32StageId == stageId);
    BDBG_ASSERT(NULL != pConnection);
    BDBG_ASSERT(NULL != pConnection->pSource);

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&pConnection->pSource->format);
    pcm = BAPE_FMT_P_IsLinearPcm(&pConnection->pSource->format);
    outputType = BAPE_FMT_P_GetDistinctOpType(&pConnection->pSource->format);

    if ( pcm )
    {
        numChannels = 2*numChannelPairs;
    }
    else
    {
        BDBG_ASSERT(numChannelPairs == 1);
        numChannels = 1;
        switch ( pConnection->pSource->format.type )
        {
        case BAPE_DataType_eIec61937x4:
        case BAPE_DataType_ePcmRf:
            outputType = BDSP_AF_P_DistinctOpType_eCompressed4x;
            dstRate = BDSP_AF_P_FmmDstFsRate_e4xBaseRate;
            break;
        case BAPE_DataType_eIec61937x16:
            outputType = BDSP_AF_P_DistinctOpType_eCompressedHBR;
            dstRate = BDSP_AF_P_FmmDstFsRate_e16xBaseRate;
            break;
        default:
            outputType = BDSP_AF_P_DistinctOpType_eCompressed;
            break;
        }
    }

    errCode = BAPE_DSP_P_AddStageOutput(pBranch, stageId, outputType, &outputTypeIndex, &outputIndex, &pDstDetails);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Initialize fields to invalid by default */
    BKNI_Memset(pDstDetails, 0, sizeof(*pDstDetails));
    BKNI_Memset(&pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId, 0xff, sizeof(pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId));   
    pDstDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
    if ( pConnection->pSink->type==BAPE_PathNodeType_eMixer )
    {
        BAPE_MixerHandle mixer = pConnection->pSink->pHandle;
        BDBG_ASSERT(NULL != mixer);
        pDstDetails->uDetails.sIoBuf.ui32IndepDelay = mixer->settings.outputDelay;
    }
    pDstDetails->uDetails.sIoBuf.eFmmDstFsRate = dstRate;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = numChannels;
    pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eFMM;
    BDBG_ASSERT(NULL != pConnection->sfifoGroup);
    if ( numChannels == 1 )
    {
        /* Non-PCM - Interleaved */
        pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = BAPE_P_SFIFO_TO_RINBGUFFER(BAPE_SfifoGroup_P_GetHwIndex(pConnection->sfifoGroup, 0));
    }
    else
    {
        /* PCM - Noninterleaved */
        for ( i = 0; i < numChannelPairs; i++ )
        {
            unsigned bufferId = BAPE_P_SFIFO_TO_RINBGUFFER(BAPE_SfifoGroup_P_GetHwIndex(pConnection->sfifoGroup, i));
            unsigned channelId = 2*i;
            pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[channelId] = bufferId;
            pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[channelId+1] = bufferId+1;
            pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[i] = BAPE_SfifoGroup_P_GetAdaptRateHwIndex(pConnection->sfifoGroup, i);
        }
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Add RAVE Buffer Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddRaveOutputBuffer(
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
    errCode = BAPE_DSP_P_AddStageOutput(pBranch, stageId, BDSP_AF_P_DistinctOpType_eCdb, &outputTypeIndex, &outputIndex, &pDstDetails);
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
    errCode = BAPE_DSP_P_AddStageOutput(pBranch, stageId, BDSP_AF_P_DistinctOpType_eItb, &outputTypeIndex, &outputIndex, &pDstDetails);
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

/***************************************************************************
Summary:
Add Inter Task Output to a stage
***************************************************************************/
BERR_Code BAPE_DSP_P_AddInterTaskBuffer(
                                       BDSP_CIT_P_FwBranchInfo *pBranch,
                                       unsigned stageId,
                                       const BAPE_PathConnection *pConnection
                                       )
{
    BERR_Code errCode;
    unsigned numChannels, numChannelPairs, outputTypeIndex, outputIndex;
    BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails;
    BDSP_AF_P_DistinctOpType outputType;
    bool pcm;

    BDBG_ASSERT(NULL != pBranch);
    /* Sanity Checks */
    BDBG_ASSERT(stageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(pBranch->sFwStgInfo[stageId].ui32StageId == stageId);
    BDBG_ASSERT(NULL != pConnection);
    BDBG_ASSERT(NULL != pConnection->pSource);

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&pConnection->pSource->format);
    pcm = BAPE_FMT_P_IsLinearPcm(&pConnection->pSource->format);
    outputType = BAPE_FMT_P_GetDistinctOpType(&pConnection->pSource->format);

    if ( pcm )
    {
        numChannels = BAPE_FMT_P_GetNumChannels(&pConnection->pSource->format);
    }
    else
    {
        BDBG_ASSERT(numChannelPairs == 1);
        numChannels = 1;
    }

    errCode = BAPE_DSP_P_AddStageOutput(pBranch, stageId, outputType, &outputTypeIndex, &outputIndex, &pDstDetails);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Initialize fields to invalid by default */
    BKNI_Memset(pDstDetails, 0, sizeof(*pDstDetails));
    pDstDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf;

    /* Descriptors are initialized in DSP Mixer/Echo Canceller Code */
    BMEM_Heap_ConvertAddressToOffset(pConnection->pSink->deviceHandle->memHandle, 
                                     pConnection->pInterTaskIoDescriptor,
                                     &pDstDetails->uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr);

    BMEM_Heap_ConvertAddressToOffset(pConnection->pSink->deviceHandle->memHandle, 
                                     pConnection->pInterTaskGenericDescriptor,
                                     &pDstDetails->uDetails.sInterTaskDramBuffCfg.ui32IoGenericBuffCfgAddr);

    return BERR_SUCCESS;
}

static bool BAPE_DSP_P_IsBranchRequired(
    BDSP_TaskCreateSettings *pSettings,     /* [in/out] Settings will be modified to add stage and possibly also a branch */
    unsigned sourceBranchId,                /* Source Branch ID */
    unsigned sourceStageId                  /* Source Stage ID */
    )
{
    unsigned opTypeNum, opNum;
    BDSP_CIT_P_FwStgConnectivity *pConnectivity;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(sourceBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sourceBranchId < pSettings->numBranches);
    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sourceBranchId]);
    BDBG_ASSERT(sourceStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(sourceStageId < pSettings->pBranchInfo[sourceBranchId]->ui32NumStages);

    /* If this is the last possible stage, a branch is always required. */
    if ( sourceStageId == (BDSP_P_MAX_FW_STG_PER_FW_BRANCH-1) )
    {
        return true;
    }
    /* If there are already more stages in this branch, we need a new branch */
    if ( sourceStageId < (pSettings->pBranchInfo[sourceBranchId]->ui32NumStages-1) )
    {
        return true;
    }
    /* If there are FMM outputs on this stage, we need a branch */
    pConnectivity = &pSettings->pBranchInfo[sourceBranchId]->sFwStgInfo[sourceStageId].sStgConnectivity;
    for ( opTypeNum = 0; opTypeNum < pConnectivity->ui32NumDistinctOp; opTypeNum++ )
    {
        for ( opNum = 0; opNum < pConnectivity->ui32NumDst[opTypeNum]; opNum++ )
        {
            switch ( pConnectivity->sDstDetails[opTypeNum][opNum].eType )
            {
            case BDSP_CIT_P_FwStgSrcDstType_eFwStg:
            case BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf:
                break;
            default:
                /* Must use a branch now. */
                return true;
            }
        }
    }

    /* No branch required if we reach here... */
    return false;
}

static BERR_Code BAPE_DSP_P_AddBranch(
    BDSP_TaskCreateSettings *pSettings,     /* [in/out] Settings will be modified to add stage and possibly also a branch */
    unsigned *pBranchId
    )
{
    unsigned branchId;
    BDBG_ASSERT(NULL != pSettings);
    if ( pSettings->numBranches >= BDSP_P_MAX_FW_BRANCH_PER_FW_TASK )
    {
        BDBG_ERR(("Maximum number of FW branches (%u) exceeded.", BDSP_P_MAX_FW_BRANCH_PER_FW_TASK));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    branchId = pSettings->numBranches;
    pSettings->pBranchInfo[branchId] = BKNI_Malloc(sizeof(BDSP_CIT_P_FwBranchInfo));
    if ( NULL == pSettings->pBranchInfo[branchId] )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BAPE_DSP_P_InitBranch(pSettings->pBranchInfo[branchId]);
    pSettings->numBranches = branchId+1;
    *pBranchId = branchId;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_DSP_P_LinkStages(
                                      BDSP_TaskCreateSettings *pSettings, 
                                      unsigned sourceBranchId, 
                                      unsigned sourceStageId, 
                                      unsigned sinkBranchId, 
                                      unsigned sinkStageId, 
                                      BDSP_AF_P_DistinctOpType outputType
                                      )
{
    BERR_Code errCode;
    unsigned outputTypeIndex, outputIndex, sourceIndex;
    BDSP_CIT_P_FwStageInfo *pStageInfo;
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails; 

    /* Lots of sanity checks... */
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(sourceBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sourceBranchId < pSettings->numBranches);
    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sourceBranchId]);
    BDBG_ASSERT(sourceStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(sourceStageId < pSettings->pBranchInfo[sourceBranchId]->ui32NumStages);
    BDBG_ASSERT(sinkBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sinkBranchId < pSettings->numBranches);
    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sourceBranchId]);
    BDBG_ASSERT(sinkStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(sinkStageId < pSettings->pBranchInfo[sinkBranchId]->ui32NumStages);

    /* Make sure we can add this as a source to the sink stage first */
    if ( pSettings->pBranchInfo[sinkBranchId]->sFwStgInfo[sinkStageId].sStgConnectivity.ui32NumSrc >= BDSP_P_MAX_FW_STG_INPUTS )
    {
        BDBG_ERR(("Maximum number of stage inputs reached on branch %u stage %u", sinkBranchId, sinkStageId));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Add this output to the source stage first */
    errCode = BAPE_DSP_P_AddStageOutput(pSettings->pBranchInfo[sourceBranchId], sourceStageId, outputType, &outputTypeIndex, &outputIndex, &pDetails);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    pDetails->uDetails.sFwStage.ui32BranchId = sinkBranchId;
    pDetails->uDetails.sFwStage.ui32StageId = sinkStageId;
    pDetails->uDetails.sFwStage.ui32DistinctOpNum = outputTypeIndex;
    pDetails->uDetails.sFwStage.ui32OpNum = outputIndex;

    /* Now add the input to the sink stage */
    pStageInfo = &pSettings->pBranchInfo[sinkBranchId]->sFwStgInfo[sinkStageId];
    sourceIndex = pStageInfo->sStgConnectivity.ui32NumSrc;
    pDetails = &pStageInfo->sStgConnectivity.sSrcDetails[sourceIndex];
    pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    pDetails->uDetails.sFwStage.ui32BranchId = sourceBranchId;
    pDetails->uDetails.sFwStage.ui32StageId = sourceStageId;
    pDetails->uDetails.sFwStage.ui32DistinctOpNum = outputTypeIndex;
    pDetails->uDetails.sFwStage.ui32OpNum = outputIndex;
    pStageInfo->sStgConnectivity.ui32NumSrc = sourceIndex+1;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Get Connector Data Type 
***************************************************************************/
BDSP_AF_P_DistinctOpType BAPE_DSP_P_GetDataTypeFromConnector(
                                                            BAPE_Connector connector
                                                            )
{
    BDBG_OBJECT_ASSERT(connector, BAPE_PathConnector);
    return BAPE_FMT_P_GetDistinctOpType(&connector->format);
}

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
    )
{
    unsigned sinkBranchId;
    unsigned sinkStageId;
    BERR_Code errCode;
    BDSP_CIT_P_FwStageInfo *pSinkStage;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(sourceBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sourceBranchId < pSettings->numBranches);
    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sourceBranchId]);
    BDBG_ASSERT(sourceStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(sourceStageId < pSettings->pBranchInfo[sourceBranchId]->ui32NumStages);

    /* Determine if we support this algorithm */
    if ( !BDSP_Raaga_IsAudioProcessingSupported(processingType) )
    {
        BDBG_ERR(("This platform does not support processing type %u", processingType));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Determine sink branch/stage */
    if ( forceBranch || BAPE_DSP_P_IsBranchRequired(pSettings, sourceBranchId, sourceStageId) )
    {
        errCode = BAPE_DSP_P_AddBranch(pSettings, &sinkBranchId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        sinkStageId = 0;
    }
    else
    {
        sinkBranchId = sourceBranchId;
        sinkStageId = sourceStageId+1;
    }

    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sinkBranchId]);
    BDBG_ASSERT(pSettings->pBranchInfo[sinkBranchId]->ui32NumStages == sinkStageId);
    BDBG_ASSERT(sinkBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sinkStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);

    /* Populate sink stage info */
    pSettings->pBranchInfo[sinkBranchId]->ui32NumStages = sinkStageId+1;
    pSinkStage = &pSettings->pBranchInfo[sinkBranchId]->sFwStgInfo[sinkStageId];
    pSinkStage->ui32BranchId = sinkBranchId;
    pSinkStage->ui32StageId = sinkStageId;
    pSinkStage->eStageType = BDSP_CIT_P_StageType_ePostProc;
    pSinkStage->uAlgorithm.eProcAudioAlgo = processingType;
    pSinkStage->uAudioMode.eProcessingAudioMode = BDSP_DSPCHN_ProcessingMode_ePP;
    /* Link source and sink stages */
    errCode = BAPE_DSP_P_LinkStages(pSettings, sourceBranchId, sourceStageId, sinkBranchId, sinkStageId, sourceDataType);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Done */
    *pSinkBranchId = sinkBranchId;
    *pSinkStageId = sinkStageId;
    return BERR_SUCCESS;
}

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
    )
{
    unsigned sinkBranchId;
    unsigned sinkStageId;
    BERR_Code errCode;
    BDSP_CIT_P_FwStageInfo *pSinkStage;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(sourceBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sourceBranchId < pSettings->numBranches);
    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sourceBranchId]);
    BDBG_ASSERT(sourceStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(sourceStageId < pSettings->pBranchInfo[sourceBranchId]->ui32NumStages);

    /* Determine if we support this algorithm */
    if ( !BDSP_Raaga_IsAudioEncodeSupported(encodeType) )
    {
        BDBG_ERR(("This platform does not support audio encode type %u", encodeType));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Determine sink branch/stage */
    if ( forceBranch || BAPE_DSP_P_IsBranchRequired(pSettings, sourceBranchId, sourceStageId) )
    {
        errCode = BAPE_DSP_P_AddBranch(pSettings, &sinkBranchId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        sinkStageId = 0;
    }
    else
    {
        sinkBranchId = sourceBranchId;
        sinkStageId = sourceStageId+1;
    }

    BDBG_ASSERT(NULL != pSettings->pBranchInfo[sinkBranchId]);
    BDBG_ASSERT(pSettings->pBranchInfo[sinkBranchId]->ui32NumStages == sinkStageId);
    BDBG_ASSERT(sinkBranchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(sinkStageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);

    /* Populate sink stage info */
    pSettings->pBranchInfo[sinkBranchId]->ui32NumStages = sinkStageId+1;
    pSinkStage = &pSettings->pBranchInfo[sinkBranchId]->sFwStgInfo[sinkStageId];
    pSinkStage->ui32BranchId = sinkBranchId;
    pSinkStage->ui32StageId = sinkStageId;
    pSinkStage->eStageType = BDSP_CIT_P_StageType_eEncode;
    pSinkStage->uAlgorithm.eEncAudioAlgo = encodeType;
    pSinkStage->uAudioMode.eEncAudioMode = encodeMode;
    /* Link source and sink stages */
    errCode = BAPE_DSP_P_LinkStages(pSettings, sourceBranchId, sourceStageId, sinkBranchId, sinkStageId, sourceDataType);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Done */
    *pSinkBranchId = sinkBranchId;
    *pSinkStageId = sinkStageId;
    return BERR_SUCCESS;
}

BERR_Code BAPE_DSP_P_AllocatePathToOutput(
                                         BAPE_PathNode *pNode,
                                         BAPE_PathConnection *pConnection
                                         )
{
    BAPE_PathConnector *pSource;
    BAPE_Handle deviceHandle;
    BAPE_PathNode *pSink;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    deviceHandle = pNode->deviceHandle;
    pSource = pConnection->pSource;
    pSink = pConnection->pSink;

    BDBG_ASSERT(NULL != pSource->pTaskCreateSettings);
    BDBG_ASSERT(pSource->branchId < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK);
    BDBG_ASSERT(pSource->stageId < BDSP_P_MAX_FW_STG_PER_FW_BRANCH);
    BDBG_ASSERT(NULL != pSource->pTaskCreateSettings->pBranchInfo[pSource->branchId]);

    if ( pSink->type == BAPE_PathNodeType_eMixer && pSink->subtype == BAPE_MixerType_eDsp )
    {
        /* FW Mixer is a special case. */
        errCode = BAPE_DSP_P_AddInterTaskBuffer(pSource->pTaskCreateSettings->pBranchInfo[pSource->branchId], pSource->stageId, pConnection);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else if ( pSink->type == BAPE_PathNodeType_eMixer || pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        /* Dropping into the FMM - need to add FMM buffers to the CIT output */
        errCode = BAPE_DSP_P_AddFmmBuffer(pSource->pTaskCreateSettings->pBranchInfo[pSource->branchId], pSource->stageId, pConnection);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else if ( pSink->type == BAPE_PathNodeType_eEchoCanceller )
    {
        BAPE_EchoCancellerHandle hEchoCanceller = pSink->pHandle;

        if ( pSource == hEchoCanceller->remoteInput )
        {
            /* Setup an inter-task link for the remote input to an echo canceller */
            errCode = BAPE_DSP_P_AddInterTaskBuffer(pSource->pTaskCreateSettings->pBranchInfo[pSource->branchId], pSource->stageId, pConnection);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }
    /* Other nodes don't require anything done here */

    return BERR_SUCCESS;
}

BERR_Code BAPE_DSP_P_ConfigPathToOutput(
                                       BAPE_PathNode *pNode,
                                       BAPE_PathConnection *pConnection
                                       )
{
    BAPE_PathConnector *pSource;
    BAPE_Handle deviceHandle;
    BAPE_PathNode *pSink;
    BERR_Code errCode;
    unsigned i, numChannelPairs;
    bool pcm;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    deviceHandle = pNode->deviceHandle;
    pSource = pConnection->pSource;
    pSink = pConnection->pSink;

    if ( pSink->type == BAPE_PathNodeType_eMixer && pSink->subtype == BAPE_MixerType_eDsp )
    {
        /* DSP mixers don't require anything */
    }
    else if ( pSink->type == BAPE_PathNodeType_eMixer || pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        BAPE_SfifoGroupSettings sfifoSettings;
        /* Dropping into the FMM - need to config SFIFO's.  DSP does not use master/slave, it will instead reuse ringbuffers in multiple
           master SFIFOs */

        pcm = BAPE_FMT_P_IsLinearPcm(&pSource->format);
        numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&pSource->format);

        BAPE_SfifoGroup_P_GetSettings(pConnection->sfifoGroup, &sfifoSettings);
        sfifoSettings.highPriority = false; /* TODO: How to handle this on the fly? */
        sfifoSettings.reverseEndian = false;
        sfifoSettings.signedData = true;
        sfifoSettings.wrpointEnabled = true;
        if ( pcm )
        {
            sfifoSettings.dataWidth = 32;
            sfifoSettings.sampleRepeatEnabled = true;
            sfifoSettings.interleaveData = false;
            /* Setup buffers from pool */
            for ( i = 0; i < numChannelPairs; i++ )
            {
                unsigned bufferId = 2*i;
                BAPE_BufferNode *pBuffer;
                
                if ( pSource->pParent->type == BAPE_PathNodeType_eDecoder )
                {
                    BAPE_DecoderHandle decoder;
                    BAPE_ConnectorFormat connectorFormat;
                    /* Decoders have the odd property of occasionally sending PCM data on the compressed path when a source
                       codec can't handle compressed output (e.g. PCM).  If that happens, the DSP will really only populate a
                       set of data buffers.  So, if you have both actual PCM outputs and fake compressed outputs receiving data
                       at the same time, you need to setup the fake compressed outputs to actually use the buffer populated on
                       the PCM path.  Same can also happen for multichannel data. */
                    decoder = (BAPE_DecoderHandle)pSource->pParent->pHandle;
                    BDBG_OBJECT_ASSERT(decoder, BAPE_Decoder);
                    
                    /* Determine connector format */
                    for ( connectorFormat = BAPE_ConnectorFormat_eStereo; connectorFormat < BAPE_ConnectorFormat_eMax; connectorFormat++ )
                    {
                        if ( pSource == &pSource->pParent->connectors[connectorFormat] )
                        {
                            break;                            
                        }
                    }                    
                    BDBG_ASSERT(connectorFormat < BAPE_ConnectorFormat_eMax);   /* If this fails something has gone seriously wrong */
                    
                    if ( (connectorFormat == BAPE_ConnectorFormat_eMultichannel && decoder->stereoOnMultichannel && 
                          decoder->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections > 0) ||
                         ((connectorFormat == BAPE_ConnectorFormat_eCompressed ||
                           connectorFormat == BAPE_ConnectorFormat_eCompressed4x ||
                           connectorFormat == BAPE_ConnectorFormat_eCompressed16x) && decoder->stereoOnCompressed && 
                          decoder->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections > 0) )
                    {
                        BDBG_MSG(("%s path of decoder %u reusing data buffers from stereo path", 
                                 (connectorFormat == BAPE_ConnectorFormat_eMultichannel)?"Multichannel":"Stereo", decoder->index));
                        /* Multichannel or Compressed sending same data as stereo path.  Reuse that. */
                        pBuffer = pSource->pParent->connectors[BAPE_ConnectorFormat_eStereo].pBuffers[i];
                    }
                    else if ( ((connectorFormat == BAPE_ConnectorFormat_eCompressed ||
                                connectorFormat == BAPE_ConnectorFormat_eCompressed4x ||
                                connectorFormat == BAPE_ConnectorFormat_eCompressed16x) && 
                               decoder->stereoOnMultichannel && decoder->stereoOnCompressed &&
                               decoder->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections > 0) )
                    {
                        BDBG_MSG(("Compressed path of decoder %u reusing data buffers from multichannel path", decoder->index));
                        /* Compressed sending same data as stereo data on multichannel path (no direct stereo consumers).  Reuse that. */
                        pBuffer = pSource->pParent->connectors[BAPE_ConnectorFormat_eMultichannel].pBuffers[i];                        
                    }
                    else
                    {
                        pBuffer = pSource->pBuffers[i]; /* Use this connector's buffers */
                    }
                }
                else
                {
                    pBuffer = pSource->pBuffers[i]; /* Not a decoder, use the connector's buffers */
                }
                 
                BDBG_ASSERT(NULL != pBuffer);
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset;
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize/2;
                sfifoSettings.bufferInfo[bufferId].wrpoint = sfifoSettings.bufferInfo[bufferId].base+(pBuffer->bufferSize/2)-1;
                bufferId++;
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset+(pBuffer->bufferSize/2);
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize/2;
                sfifoSettings.bufferInfo[bufferId].wrpoint = sfifoSettings.bufferInfo[bufferId].base+(pBuffer->bufferSize/2)-1;
            }
        }
        else
        {
            sfifoSettings.dataWidth = 16;
            sfifoSettings.sampleRepeatEnabled = false;
            sfifoSettings.interleaveData = true;
            /* Setup buffers from pool */
            for ( i = 0; i < numChannelPairs; i++ )
            {
                unsigned bufferId = 2*i;
                BAPE_BufferNode *pBuffer = pSource->pBuffers[i];
                BDBG_ASSERT(NULL != pBuffer);
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset;
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize;
                sfifoSettings.bufferInfo[bufferId].wrpoint = pBuffer->offset+pBuffer->bufferSize-1;
                bufferId++;
                sfifoSettings.bufferInfo[bufferId].base = 0;
                sfifoSettings.bufferInfo[bufferId].length = 0;
                sfifoSettings.bufferInfo[bufferId].wrpoint = 0;
            }
        }

        errCode = BAPE_SfifoGroup_P_SetSettings(pConnection->sfifoGroup, &sfifoSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    /* Other nodes don't require anything done here */

    return BERR_SUCCESS;
}

BERR_Code BAPE_DSP_P_StartPathToOutput(
                                      BAPE_PathNode *pNode,
                                      BAPE_PathConnection *pConnection
                                      )
{
    BAPE_PathNode *pSink;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    pSink = pConnection->pSink;

    if ( pSink->type == BAPE_PathNodeType_eMixer && pSink->subtype == BAPE_MixerType_eDsp )
    {
        /* FW Mixer is a special case. Nothing to do */
    }
    else if ( pSink->type == BAPE_PathNodeType_eMixer || pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        /* TODO: Sure would be nice to use PLAY_RUN instead of WRPOINT */
        errCode = BAPE_SfifoGroup_P_Start(pConnection->sfifoGroup, false);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BAPE_DSP_P_StopPathToOutput(
                                BAPE_PathNode *pNode,
                                BAPE_PathConnection *pConnection
                                )
{
    BAPE_PathNode *pSink;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    pSink = pConnection->pSink;

    if ( pSink->type == BAPE_PathNodeType_eMixer && pSink->subtype == BAPE_MixerType_eDsp )
    {
        /* FW Mixer is a special case. Nothing to do */
    }
    else if ( pSink->type == BAPE_PathNodeType_eMixer || pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        BAPE_SfifoGroup_P_Stop(pConnection->sfifoGroup);
    }
}

void BAPE_DSP_P_GetRaveSrcDetails(
    BAPE_Handle deviceHandle, 
    const BAVC_XptContextMap *pContextMap, 
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails
    )
{
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pContextMap);
    BDBG_ASSERT(NULL != pDetails);

    BKNI_Memset(pDetails, 0, sizeof(BDSP_CIT_P_FwStgSrcDstDetails));

    pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pContextMap->CDB_Base;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pContextMap->CDB_End;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pContextMap->CDB_Read;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pContextMap->CDB_Valid;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pContextMap->CDB_Wrap;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pContextMap->ITB_Base;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pContextMap->ITB_End;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pContextMap->ITB_Read;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pContextMap->ITB_Valid;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pContextMap->ITB_Wrap;      
}

void BAPE_DSP_P_GetDfifoSrcDetails(
    BAPE_Handle deviceHandle, 
    BAPE_DfifoGroupHandle dfifoGroup, 
    unsigned numChannelPairs,
    bool interleaveData,
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails
    )
{
    unsigned i;
    uint32_t rbufId;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != dfifoGroup);
    BDBG_ASSERT(numChannelPairs > 0);
    BDBG_ASSERT(numChannelPairs <= (BDSP_AF_P_MAX_CHANNELS/2));
    BDBG_ASSERT(NULL != pDetails);
    BDBG_ASSERT(interleaveData == false || numChannelPairs == 1);   /* Only support single buffer for compressed.  PCM must be non-interleaved */

    BKNI_Memset(pDetails, 0, sizeof(BDSP_CIT_P_FwStgSrcDstDetails));

    pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
    pDetails->uDetails.sIoBuf.ui32IndepDelay = 0;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = (interleaveData)?numChannelPairs:2*numChannelPairs;
    pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eFMM;
    for ( i = 0; i < numChannelPairs; i++ )
    {
        rbufId = BAPE_P_DFIFO_TO_RINBGUFFER(BAPE_DfifoGroup_P_GetHwIndex(dfifoGroup, i));
        pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[2*i] = rbufId;
        pDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[(2*i)+1] = rbufId+1;
    }
}

