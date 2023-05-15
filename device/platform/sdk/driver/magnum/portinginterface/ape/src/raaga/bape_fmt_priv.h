/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: bape_fmt_priv.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 9/11/12 3:35p $
*
* API Description:
*   API name: FMT
*    Audio Format Related Routines
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_fmt_priv.h $
* 
* Hydra_Software_Devel/2   9/11/12 3:35p jgarrett
* SW7425-3796: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-3796/1   8/23/12 6:05p mpeteri
* SW7425-3796: PI Changes to transmit original ES DTS-CD signal to
* SPDIF/HDMI
* 
* Hydra_Software_Devel/1   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/4   5/2/12 2:38p gskerl
* SW7425-2075: Added enum value for BAPE_DataType_eUnknown. Also
* corrected some comments.
* 
* Hydra_Software_Devel/SW7425-2075/3   4/27/12 6:31p gskerl
* SW7425-2075: Added BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS() and
* BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS() macros.
* 
* Hydra_Software_Devel/SW7425-2075/2   4/18/12 3:01p gskerl
* SW7425-2075: Added BAPE_FMT_P_LOG_FMT_CHANGE() and
* BAPE_FMT_TO_PRINTF_ARGS() macros.
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
***************************************************************************/
#ifndef BAPE_FMT_PRIV_H_
#define BAPE_FMT_PRIV_H_

/***************************************************************************
Summary:
Data Source Types
***************************************************************************/
typedef enum BAPE_DataSource
{
    BAPE_DataSource_eDspBuffer,
    BAPE_DataSource_eHostBuffer,
    BAPE_DataSource_eDfifo,
    BAPE_DataSource_eFci,           /* Other FCI source (SRC/Mixer/Splitter/Speaker Mgmt) */
    BAPE_DataSource_eRave,
    BAPE_DataSource_eMax
} BAPE_DataSource;

/***************************************************************************
Summary:
Data Type Identifiers
***************************************************************************/
typedef enum BAPE_DataType
{
    BAPE_DataType_ePcmMono,         /* True mono PCM Data.  Not supported in the FMM but can be used within the DSP in some limited fashion. */
    BAPE_DataType_ePcmStereo,       /* PCM 2.0 data */
    BAPE_DataType_ePcm5_1,          /* PCM 5.1 data */
    BAPE_DataType_ePcm7_1,          /* PCM 7.1 data */
    BAPE_DataType_ePcmRf,           /* RF Encoded PCM Data (e.g. BTSC encoder output).  Encoded as mono 32-bit PCM running at 4x the sample rate. */
    BAPE_DataType_eIec61937,        /* IEC-61937 compressed data for SPDIF/HDMI including preambles. */
    BAPE_DataType_eIec61937x4,      /* IEC-61937 compressed data for HDMI running at 4x the sample rate (e.g. AC3+ Passthrough) */
    BAPE_DataType_eIec61937x16,     /* HDMI HBR compressed data.  Runs at 4x the sample rate with 4 samples per clock.  */
    BAPE_DataType_eIec60958Raw,     /* RAW IEC-60958 data.  Data should be output directly without CBIT insertion in HW. (see SOURCECH_CFG_APPEND.RETAIN_FCI_TAG) */
    BAPE_DataType_eCompressedRaw,   /* RAW Compressed data without 61937 formatting (output from some encoders) */
    BAPE_DataType_eRave,            /* RAVE data, sent in separate CDB/ITB buffers. */
    BAPE_DataType_eUnknown,         /* Unknown or unrecognized data type. */
    BAPE_DataType_eMax
} BAPE_DataType;

/***************************************************************************
Summary:
Common Format Descriptor
***************************************************************************/
typedef struct BAPE_FMT_Descriptor
{
    BAPE_DataSource source;             /* Data Source (e.g. DSP buffer, FCI, Host Buffer, etc.) */
    BAPE_DataType   type;               /* Data Type (e.g. PCM Stereo, 61937 compressed, etc.) */
    union 
    {
        BAVC_AudioCompressionStd codec;     /* Useful for 61937 data formats */
        BAPE_RfAudioEncoding rfEncoding;    /* Useful for RfEncoded data */
    } subType;

    unsigned        sampleRate;         /* Sample Rate in Hz */
    bool            ppmCorrection;      /* True if PPM correction is enabled */
} BAPE_FMT_Descriptor;

/***************************************************************************
Summary:
Initialize a format descriptor
***************************************************************************/
void BAPE_FMT_P_InitDescriptor(
    BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the number of channel pairs required from a format descriptor
***************************************************************************/
unsigned BAPE_FMT_P_GetNumChannelPairs(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the number of channels required from a format descriptor
***************************************************************************/
unsigned BAPE_FMT_P_GetNumChannels(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Convert a BAPE_DataType enum to a text string.
***************************************************************************/
const char *BAPE_FMT_P_TypeToString(
    const BAPE_DataType type
    );

/***************************************************************************
Summary:
Get the name of a data type from a format descriptor
***************************************************************************/
const char *BAPE_FMT_P_GetTypeName(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Convert a BAPE_DataSource enum to a text string.
***************************************************************************/
const char *BAPE_FMT_P_SourceToString(
    const BAPE_DataSource source
    );

/***************************************************************************
Summary:
Get the name of a data source from a format descriptor
***************************************************************************/
const char *BAPE_FMT_P_GetSourceName(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the name of a subtype's type from a format descriptor
***************************************************************************/
const char *BAPE_FMT_P_GetSubTypeTypeName(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the name of a subtype from a format descriptor
***************************************************************************/
const char *BAPE_FMT_P_GetSubTypeName(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Determine if a data type is linear pcm
***************************************************************************/
bool BAPE_FMT_P_IsLinearPcm(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Determine if a data type is compressed
***************************************************************************/
bool BAPE_FMT_P_IsCompressed(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Determine if a data type is compressed and content is DTS-CD
***************************************************************************/
bool BAPE_FMT_P_IsDtsCdCompressed(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Determine if a data type is 16x compressed mode
***************************************************************************/
bool BAPE_FMT_P_IsHBR(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Determine if ramping should be enabled for a particular data type
***************************************************************************/
bool BAPE_FMT_P_RampingValid(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the current codec from a format descriptor
***************************************************************************/
BAVC_AudioCompressionStd BAPE_FMT_P_GetAudioCompressionStd(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Get the current RF encoding from a format descriptor
***************************************************************************/
BAPE_RfAudioEncoding BAPE_FMT_P_GetRfAudioEncoding(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Set the current codec in a format descriptor
***************************************************************************/
void BAPE_FMT_P_SetAudioCompressionStd(
    BAPE_FMT_Descriptor *pDesc, 
    BAVC_AudioCompressionStd codec
    );

/***************************************************************************
Summary:
Set the current RF encoding in a format descriptor
***************************************************************************/
void BAPE_FMT_P_SetRfAudioEncoding(
    BAPE_FMT_Descriptor *pDesc, 
    BAPE_RfAudioEncoding encoding
    );

/***************************************************************************
Summary:
Determine if two format descriptors are identical
***************************************************************************/
bool BAPE_FMT_P_IsEqual(
    const BAPE_FMT_Descriptor *pDesc1, 
    const BAPE_FMT_Descriptor *pDesc2
    );

/***************************************************************************
Summary:
Get the DSP Distinct Output Port Type from a format descriptor
***************************************************************************/
BDSP_AF_P_DistinctOpType BAPE_FMT_P_GetDistinctOpType(
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Common Format Capabilities
***************************************************************************/
typedef struct BAPE_FMT_Capabilities
{
    uint32_t sourceMask[(BAPE_DataSource_eMax+31)/32];
    uint32_t typeMask[(BAPE_DataType_eMax+31)/32];
    /* TODO: perhaps min/max sample rates as well */
} BAPE_FMT_Capabilities;

/***************************************************************************
Summary:
Initialize Capabilities
***************************************************************************/
void BAPE_FMT_P_InitCapabilities(
    BAPE_FMT_Capabilities *pCaps, 
    const BAPE_DataSource *pSources,    /* Optional.  Pass NULL for no default sources. */
    const BAPE_DataType *pTypes         /* Optional.  Pass NULL for no default types. */
    );

/***************************************************************************
Summary:
Enable an input data type
***************************************************************************/
void BAPE_FMT_P_EnableType(
    BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataType type
    );

/***************************************************************************
Summary:
Disable an input data type
***************************************************************************/
void BAPE_FMT_P_DisableType(
    BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataType type
    );

/***************************************************************************
Summary:
Enable an input data source
***************************************************************************/
void BAPE_FMT_P_EnableSource(
    BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataSource source
    );

/***************************************************************************
Summary:
Disable an input data source
***************************************************************************/
void BAPE_FMT_P_DisableSource(
    BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataSource source
    );

/***************************************************************************
Summary:
Check if a format descriptor is valid based on a set of capabilities
***************************************************************************/
bool BAPE_FMT_P_FormatSupported(
    const BAPE_FMT_Capabilities *pCaps, 
    const BAPE_FMT_Descriptor *pDesc
    );

/***************************************************************************
Summary:
Check if a data type is valid based on a set of capabilities
***************************************************************************/
bool BAPE_FMT_P_TypeSupported(
    const BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataType type
    );

/***************************************************************************
Summary:
Check if a data source is valid based on a set of capabilities
***************************************************************************/
bool BAPE_FMT_P_SourceSupported(
    const BAPE_FMT_Capabilities *pCaps, 
    BAPE_DataSource source
    );

/***************************************************************************
Summary:
Generate a printf-style format string and argument list for printing 
BAPE_DataSources that are enabled in a BAPE_FMT_Capabilities.sourceMask[]. 
 
Examples: 
        BDBG_MSG((BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS(&pConsumer->inputCapabilities)));
 
        BDBG_WRN(("pCaps: " BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS(pCaps)));
 
        printf(BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS(&myCaps)); printf("\n");
 
***************************************************************************/
#define BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS(pCaps)                                                                                             \
            "SourceMask: %s+%s+%s+%s+%s",                                                                                                       \
            BAPE_FMT_P_SourceSupported((pCaps), BAPE_DataSource_eDspBuffer)  ? BAPE_FMT_P_SourceToString(BAPE_DataSource_eDspBuffer)  : "-",    \
            BAPE_FMT_P_SourceSupported((pCaps), BAPE_DataSource_eHostBuffer) ? BAPE_FMT_P_SourceToString(BAPE_DataSource_eHostBuffer) : "-",    \
            BAPE_FMT_P_SourceSupported((pCaps), BAPE_DataSource_eDfifo)      ? BAPE_FMT_P_SourceToString(BAPE_DataSource_eDfifo)      : "-",    \
            BAPE_FMT_P_SourceSupported((pCaps), BAPE_DataSource_eFci)        ? BAPE_FMT_P_SourceToString(BAPE_DataSource_eFci)        : "-",    \
            BAPE_FMT_P_SourceSupported((pCaps), BAPE_DataSource_eRave)       ? BAPE_FMT_P_SourceToString(BAPE_DataSource_eRave)       : "-" 

/***************************************************************************
Summary:
Generate a printf-style format string and argument list for printing 
BAPE_DataTypes that are enabled in a BAPE_FMT_Capabilities.typeMask[]. 
 
Examples: 
        BDBG_MSG((BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS(&pConsumer->inputCapabilities)));
 
        BDBG_WRN(("pCaps: " BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS(pCaps)));
 
        printf(BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS(&myCaps)); printf("\n");
 
***************************************************************************/
#define BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS(pCaps)                                                                                               \
            "TypeMask: %s+%s+%s+%s+%s+%s+%s+%s+%s+%s+%s",                                                                                       \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_ePcmMono      )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_ePcmMono      )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_ePcmStereo    )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_ePcmStereo    )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_ePcm5_1       )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_ePcm5_1       )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_ePcm7_1       )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_ePcm7_1       )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_ePcmRf        )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_ePcmRf        )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eIec61937     )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eIec61937     )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eIec61937x4   )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eIec61937x4   )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eIec61937x16  )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eIec61937x16  )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eIec60958Raw  )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eIec60958Raw  )  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eCompressedRaw)  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eCompressedRaw)  : "-",    \
            BAPE_FMT_P_TypeSupported((pCaps), BAPE_DataType_eRave         )  ? BAPE_FMT_P_TypeToString(BAPE_DataType_eRave         )  : "-"    

/***************************************************************************
Summary:
Generate a printf-style format string and argument list for printing 
the contents of a BAPE_FMT_Descriptor. 
 
Examples: 
        BDBG_MSG((BAPE_FMT_P_TO_PRINTF_ARGS(&handle->inputPortFormat)));
 
        BDBG_WRN(("Warning!!! " BAPE_FMT_P_TO_PRINTF_ARGS(pDesc)));
 
        printf(BAPE_FMT_P_TO_PRINTF_ARGS(&myDesc)); printf("\n");
 
***************************************************************************/
#define BAPE_FMT_P_TO_PRINTF_ARGS(pDesc)                                                      \
            "BAPE_FMT_Desc: Source:%s Type:%s %s%s SampleRate:%u PPM Corr:%s",                \
            BAPE_FMT_P_GetSourceName(pDesc),                                                  \
            BAPE_FMT_P_GetTypeName(pDesc),                                                    \
            (BAPE_FMT_P_GetAudioCompressionStd(pDesc) != BAVC_AudioCompressionStd_eMax) ?     \
                    "Codec:" :                                                                \
                    (BAPE_FMT_P_GetRfAudioEncoding(pDesc) != BAPE_RfAudioEncoding_eMax) ?     \
                            "RF Aud Enc:"    :                                                \
                            "SubType:",                                                       \
            (BAPE_FMT_P_GetAudioCompressionStd(pDesc) != BAVC_AudioCompressionStd_eMax) ?     \
                    BAPE_P_GetCodecName(BAPE_FMT_P_GetAudioCompressionStd(pDesc)) :           \
                    (BAPE_FMT_P_GetRfAudioEncoding(pDesc) != BAPE_RfAudioEncoding_eMax) ?     \
                            "BTSC"    :                                                       \
                            "N/A",                                                            \
            (pDesc)->sampleRate,                                                              \
            ((pDesc)->ppmCorrection) ? "TRUE" : "false"

/***************************************************************************
Summary:
Do a multi-line print out of two BAPE_FMT_Descriptors, showing changes 
between them.  The BDBG_LOG will show something like this: 
 
    00:00:01.003 bape_fmt_priv: -------- BAPE_FMT_Descriptor Change ---- begin ----
    00:00:01.003 bape_fmt_priv:   Source Name     : RAVE
    00:00:01.003 bape_fmt_priv:   Type Name       : IEC-61937 Compressed
    00:00:01.003 bape_fmt_priv:   Subtype Name    : DTS-HD
    00:00:01.003 bape_fmt_priv:   Sample Rate     : 768000
    00:00:01.003 bape_fmt_priv:   PPM Correction  : false -> TRUE
    00:00:01.003 bape_fmt_priv: -------- BAPE_FMT_Descriptor Change ---- end ----
 
***************************************************************************/
void BAPE_FMT_P_LogFmtChange(
    BDBG_Level    bDbgLevel,
    const BAPE_FMT_Descriptor *pOld, 
    const BAPE_FMT_Descriptor *pNew
    );

/***************************************************************************
Summary:
Do a multi-line print out of two BAPE_FMT_Descriptors, showing changes 
between them.  A macro version of the function BAPE_FMT_P_LogFmtChange().
***************************************************************************/
#define BAPE_FMT_P_LOG_FMT_CHANGE(bdbgLogMacro, pOld, pNew)                                                                                 \
    do {                                                                                                                                    \
        BDBG_ASSERT(NULL!=(pOld));                                                                                                          \
        BDBG_ASSERT(NULL!=(pNew));                                                                                                          \
                                                                                                                                            \
        bdbgLogMacro(("-------- BAPE_FMT_Descriptor Change ---- begin ----"));                                                              \
                                                                                                                                            \
        BAPE_LOG_CHANGE(bdbgLogMacro, "  Source Name     ", "%s",  (pOld)->source,  BAPE_FMT_P_GetSourceName((pOld)),                       \
                                                                   (pNew)->source,  BAPE_FMT_P_GetSourceName((pNew)) );                     \
                                                                                                                                            \
        BAPE_LOG_CHANGE(bdbgLogMacro, "  Type Name       ", "%s",  (pOld)->type,  BAPE_FMT_P_GetTypeName((pOld)),                           \
                                                                   (pNew)->type,  BAPE_FMT_P_GetTypeName((pNew)) );                         \
                                                                                                                                            \
        BAPE_LOG_CHANGE(bdbgLogMacro, "  Subtype Name    ", "%s",  BAPE_FMT_P_GetSubTypeName((pOld)), BAPE_FMT_P_GetSubTypeName((pOld)),    \
                                                                   BAPE_FMT_P_GetSubTypeName((pNew)), BAPE_FMT_P_GetSubTypeName((pNew)));   \
                                                                                                                                            \
        BAPE_LOG_CHANGE(bdbgLogMacro, "  Sample Rate     ", "%u",  (pOld)->sampleRate, (pOld)->sampleRate,                                  \
                                                                   (pNew)->sampleRate, (pNew)->sampleRate);                                 \
                                                                                                                                            \
        BAPE_LOG_CHANGE(bdbgLogMacro, "  PPM Correction  ", "%s",  (pOld)->ppmCorrection, (pOld)->ppmCorrection ? "TRUE" : "false",         \
                                                                   (pNew)->ppmCorrection, (pNew)->ppmCorrection ? "TRUE" : "false");        \
                                                                                                                                            \
        bdbgLogMacro(("-------- BAPE_FMT_Descriptor Change ---- end ----"));                                                                \
    } while(0)

#endif /* #ifndef BAPE_FMT_PRIV_H_ */
