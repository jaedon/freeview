/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSMEDIAFORMATPARSERS_C
#include <drmm2tsparsers.h>
#include <drmm2tsbuilders.h>
#include <drmm2tsbitstream.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tslogs.h>
#include <drmm2tsmacros.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define DEFINE_AUDIO_SEARCH_START_CODE_FUNCTIONS   0    /* These static functions are currently never called. */

#define H264_NAL_UNIT_SIZE                      4               /* H264 has either 3 or 4 NAL unit start code and one byte NAL unit type */
#define H264_3BYTE_STARTCODE_PLUS_TYPE          4
#define H264_NAL_UNIT_TYPE_MASK                 0x1F
#define H264_NAL_TYPE_NON_IDR_PICTURE           0x01
#define H264_NAL_TYPE_IDR_PICTURE               0x05
#define H264_NAL_TYPE_SEI                       0x06            /* Supplemental enhancement information */
#define H264_NAL_TYPE_SPS                       0x07            /* Sequence Parameter Set */
#define H264_NAL_TYPE_PPS                       0x08            /* Picture Parameter Set */
#define H264_NAL_TYPE_ACCESS_UNIT_DELIMITER     0x09
#define H264_NAL_DATA_IN_CLEAR                  111             /* 
                                                                ** The first 96 to 111 bytes of each NAL, which includes the NAL length and nal_unit_type fields,
                                                                ** SHALL be left unencrypted.  The exact number of unencrypted bytes is chosen so that the remainder
                                                                ** of the NAL is a multiple of 16 bytes.  Use the max here and reduce it when it is needed.
                                                                */
#define AESCTR_BLOCK_SIZE                       16

#define MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE   4               /* 3-byte start code and 1-byte start code value */
#define MPEG2_VIDEO_SCV_PICTURE_STRAT_CODE      0x00
#define MPEG2_VIDEO_SCV_SEQUENCE_HEADER_CODE    0xB3
#define MPEG2_VIDEO_SCV_FIRST_SLICE_START_CODE  0x01
#define MPEG2_VIDEO_SCV_LAST_SLICE_START_CODE   0xAF

#define MPEGA_AUDIO_MAX_HEADER_SIZE             6               /* 4-byte header and 2-byte CRC */
#define MPEGA_AUDIO_HEADER_ONLY_SIZE            4
#define MPEGA_AUDIO_SYNCWORD_SIZE               2
#define MPEGA_AUDIO_SYNCWORD_HIGHBYTE           0xFF
#define MPEGA_AUDIO_SYNCWORD_LOWBYTE            0xE0
#define MPEGA_AUDIO_LAYER_FIELD_MARK            0x06

#define ADTS_SYNCWORD_SIZE                      2
#define ADTS_SYNCWORD_HIGHBYTE                  0xFF
#define ADTS_SYNCWORD_LOWBYTE                   0xF0
#define ADTS_LAYER_FIELD_MASK                   0x06            /* Layer field is always zero and is the 2nd and third bit after the syncword */
#define ADTS_MAX_HEADER_SIZE                    9
#define ADTS_MIN_HEADER_SIZE                    7

#define DDPLUS_HEADER_SIZE                      126             /* The AC3 header is 63 * 2 = 126 bytes */
#define DDPLUS_SYNCWORD_SIZE                    2
#define DDPLUS_SYNCWORD_HIGHBYTE                0x0B
#define DDPLUS_SYNCWORD_LOWBYTE                 0x77

#define PES_PACKET_LENGTH_UNBOUNDED             0

/* Sampling rate codes as per ADTS spec */
static DRM_DWORD s_oADTS_SamplingRatesFromRateCode[] = {
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350
};

#define GBL_MAXFSCOD                     3              /* # defined sample rates */
#define GBL_MAXDDDATARATE               38              /* # defined data rates */

/* 
** Duration in hundred nano seconds (HNS = 10^-7 sec) of one Dolby frame, as a function
** of sample rate and number of blocks per frame.
** Index to this array is [fscod][numblkscod]
*/
static DRM_DWORD s_oDDPlusFrameDuration[ GBL_MAXFSCOD ][ 4 ] = {  
/* '256 Samples (1 block)'   '512 samples (2 block)'  '768 samples (3 blocks)'  '1536 samples (6 blocks)' */
       {   53333,                    106667,                  160000,                 320000    }, /*  48 kHz */
       {   58050,                    116100,                  174150,                 348299    }, /*  44.1 kHz */
       {   80000,                    160000,                  240000,                 480000    }, /*  32 kHz */
};

/* words per frame table; based on sample rate and data rate codes */
const DRM_WORD s_oGBLFrameSizeTable[ GBL_MAXFSCOD ][ GBL_MAXDDDATARATE ] =
{
    /* 48kHz */
    {    64,  64,  80,  80,  96,  96, 112, 112,
        128, 128, 160, 160, 192, 192, 224, 224,
        256, 256, 320, 320, 384, 384, 448, 448,
        512, 512, 640, 640, 768, 768, 896, 896,
        1024, 1024, 1152, 1152, 1280, 1280 },
    /* 44.1kHz */
    {    69,  70,  87,  88, 104, 105, 121, 122,
        139, 140, 174, 175, 208, 209, 243, 244,
        278, 279, 348, 349, 417, 418, 487, 488,
        557, 558, 696, 697, 835, 836, 975, 976,
        1114, 1115, 1253, 1254, 1393, 1394 },
    /* 32kHz */
    {    96,  96, 120, 120, 144, 144, 168, 168,
        192, 192, 240, 240, 288, 288, 336, 336,
        384, 384, 480, 480, 576, 576, 672, 672,
        768, 768, 960, 960, 1152, 1152, 1344, 1344,
        1536, 1536, 1728, 1728, 1920, 1920 } 
};

/* 
** Duration in hundred nano seconds (HNS = 10^-7 sec) of one Dolby Digital Plus frame
** for the lower sampling rate (which always has 1536 samples / frame)
*/
static DRM_DWORD s_oDDPlusLowSampleRateFrameDuration[] = { 
    640000, /* 24 kHz */
    696599, /* 22.05 kHz */
    960000, /* 16 kHz */
};

/*
**  MPEG audio sampling rate
*/
static DRM_DWORD s_oMPEGASamplingRate[3][4] = 
{
    { 22050, 24000, 16000, 22050 }, // ID=0: MPEG2
    { 44100, 48000, 32000, 44100 }, // ID=1: MPEGA
    { 11025, 12000,  8000, 11025 }  // ID=0: MPEG2.5
};

static DRM_DWORD s_oMPEGABitRate[2][3][15] =
{
    {
        /* ID = 0 */
        {0, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000, 176000, 192000, 224000, 256000},
        {0,  8000, 16000, 24000, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000},
        {0,  8000, 16000, 24000, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000},
    },
    {
        /* ID = 1 */
        {0, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000},
        {0, 32000, 48000, 56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000},
        {0, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000},
    }
};


#define DRM_M2TS_IS_MPEGA_SYNC_WORD( pbPayload, pwSyncOffset )                                                          \
    (        (pbPayload)[ *(pwSyncOffset) ] == MPEGA_AUDIO_SYNCWORD_HIGHBYTE                                            \
        && ( (pbPayload)[ ( *(pwSyncOffset) ) + 1 ] & MPEGA_AUDIO_SYNCWORD_LOWBYTE ) == MPEGA_AUDIO_SYNCWORD_LOWBYTE    \
        && ( (pbPayload)[ ( *(pwSyncOffset) ) + 1 ] & MPEGA_AUDIO_LAYER_FIELD_MARK ) != 0                               \
    ) 

/* Check the layer field because the layer field is always zero */
#define DRM_M2TS_IS_ADTS_SYNC_WORD( pbPayload, pwSyncOffset )                                                           \
    (                                                                                                                   \
             (pbPayload)[ *(pwSyncOffset) ] == ADTS_SYNCWORD_HIGHBYTE                                                   \
        && ( (pbPayload)[ ( *(pwSyncOffset) ) + 1 ] & ADTS_LAYER_FIELD_MASK ) == 0                                      \
        && ( (pbPayload)[ ( *(pwSyncOffset) ) + 1 ] & ADTS_SYNCWORD_LOWBYTE ) == ADTS_SYNCWORD_LOWBYTE                  \
    )

#define DRM_M2TS_IS_DDPLUS_SYNC_WORD( pbPayload, pwSyncOffset )                                                         \
    (                                                                                                                   \
           (pbPayload)[ *(pwSyncOffset) ] == DDPLUS_SYNCWORD_HIGHBYTE                                                   \
        && (pbPayload)[ ( *(pwSyncOffset) ) + 1 ] == DDPLUS_SYNCWORD_LOWBYTE                                            \
    )

#if DBG
#define VERIFY_BYTE_COUNT_OF_REMAINING_BYTES_IN_SUBSAMPLE_CONTEXT_BUFFER( poSubsampleContext, cbRemainingExpected )     \
    do {                                                                                                                \
        DRM_WORD cbRemainingCalculated = 0;                                                                             \
        ChkDR( DRM_WordSub( (poSubsampleContext)->cbBuffer, (poSubsampleContext)->wOffset, &cbRemainingCalculated ) );  \
        DRMASSERT( cbRemainingCalculated == (cbRemainingExpected) );                                                    \
    } while( FALSE )
#else
#define VERIFY_BYTE_COUNT_OF_REMAINING_BYTES_IN_SUBSAMPLE_CONTEXT_BUFFER( f_poSubsampleContext, cbRemainingExpected )
#endif /*DBG */

/**********************************************************************
**
** Function:    _AddNewSample
**
** Synopsis:    Get and initialize an index of a free sample entry in 
**              pSamples array in the encryption context then assign 
**              it to the PES.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPES]                 -- Pointer to a PES to add a sample to
** [f_ppSample]             -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _AddNewSample( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pPES,
    __out       DRM_M2TS_SAMPLE                **f_ppSample ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _AddNewSample( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pPES,
    __out       DRM_M2TS_SAMPLE                **f_ppSample )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pPES != NULL );
    DRMASSERT( f_ppSample != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pPES != NULL );
    __analysis_assume( f_ppSample != NULL );

    *f_ppSample = NULL;

    if( f_pEncContext->dwIndexNextFreeSample >= f_pEncContext->cSamplesAllocated )
    {
        /* The buffer is full, need a larger buffer. */
        ChkDR( DRM_M2TS_AllocateSamples( f_pEncContext ) );
    }

    *f_ppSample = &f_pEncContext->pSamples[ f_pEncContext->dwIndexNextFreeSample ];

    MEMSET( *f_ppSample, 0, SIZEOF( DRM_M2TS_SAMPLE ) );
    (*f_ppSample)->fHasSubsample = FALSE;

    if( !f_pPES->fHasSample )
    { 
        f_pPES->fHasSample = TRUE;
        f_pPES->dwIndexFirstSample = f_pEncContext->dwIndexNextFreeSample;
    } 
    f_pPES->dwIndexLastSample = f_pEncContext->dwIndexNextFreeSample;

    ChkDR( DRM_DWordIncOne( f_pEncContext->dwIndexNextFreeSample ) );

#if DBG
    f_pEncContext->cActualSamplesUsed = max( f_pEncContext->cActualSamplesUsed, f_pEncContext->dwIndexNextFreeSample );
#endif /* DBG */

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _AddNewSubsample
**
** Synopsis:    Get and initialize an index of a free subsample entry in 
**              pSubsamples array in the encryption context then assign 
**              it to the sample.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pSample]              -- Pointer to a Sample to add a subsample to
** [f_ppSubsample]          -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _AddNewSubsample( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_SAMPLE                 *f_pSample,
    __out       DRM_M2TS_SUBSAMPLE             **f_ppSubsample ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _AddNewSubsample( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_SAMPLE                 *f_pSample,
    __out       DRM_M2TS_SUBSAMPLE             **f_ppSubsample )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pSample != NULL );
    DRMASSERT( f_ppSubsample != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pSample != NULL );
    __analysis_assume( f_ppSubsample != NULL );

    *f_ppSubsample = NULL;

    if( f_pEncContext->dwIndexNextFreeSubsample >= f_pEncContext->cSubsamplesAllocated )
    {
        /* The buffer is full, need a larger buffer. */
        ChkDR( DRM_M2TS_AllocateSubsamples( f_pEncContext ) );
    }

    *f_ppSubsample = &f_pEncContext->pSubsamples[ f_pEncContext->dwIndexNextFreeSubsample ];

    MEMSET( *f_ppSubsample, 0 , SIZEOF( DRM_M2TS_SUBSAMPLE ) );

    if( !f_pSample->fHasSubsample )
    { 
        f_pSample->fHasSubsample = TRUE;
        f_pSample->dwIndexFirstSubsample = f_pEncContext->dwIndexNextFreeSubsample;
    } 
    f_pSample->dwIndexLastSubsample = f_pEncContext->dwIndexNextFreeSubsample;

    ChkDR( DRM_DWordIncOne( f_pEncContext->dwIndexNextFreeSubsample ) );

#if DBG
    f_pEncContext->cActualSubsamplesUsed = max( f_pEncContext->cActualSubsamplesUsed, f_pEncContext->dwIndexNextFreeSubsample );
#endif /* DBG */

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _EnsureProtectedOnDWordBoundary
**
** Synopsis:    Make each subsample's encrypted bytes on DWORD bounary 
**              by increasing the clear bytes so it is a multiple of four. 
**              This helps the perf of the decrypt function.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poSample]             -- Pointer to a sample
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _EnsureProtectedOnDWordBoundary( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_SAMPLE                 *f_poSample ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _EnsureProtectedOnDWordBoundary( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_SAMPLE                 *f_poSample )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_DWORD            iSubsample         = 0;
    DRM_M2TS_SUBSAMPLE  *pSubsample         = NULL;

    ChkArg( f_poSample != NULL );
    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );

    iSubsample = f_poSample->dwIndexFirstSubsample;
    while ( iSubsample <= f_poSample->dwIndexLastSubsample )
    {
        DRM_WORD  cbAdj         = 0;
        DRM_WORD  cbModulusOf4  = 0;

        pSubsample = &f_pEncContext->pSubsamples[ iSubsample ];

        ChkDR( DRM_LongToWord( pSubsample->cbClearBytes & 0x03, &cbModulusOf4 ) );
        ChkDR( DRM_LongToWord( cbModulusOf4 == 0 ? 0 : 4 - cbModulusOf4, &cbAdj ) );

        if( cbAdj > 0 && pSubsample->cbEncryptedBytes > cbAdj )
        {
            ChkDR( DRM_WordAdd( pSubsample->cbClearBytes, cbAdj, &pSubsample->cbClearBytes ) );
            ChkDR( DRM_DWordSub( pSubsample->cbEncryptedBytes, cbAdj, &pSubsample->cbEncryptedBytes ) );
        }

        iSubsample++;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _H264_SearchStartCode
**
** Synopsis:    Searches the start code 0x000001 followed by access
**              unit delimiter of H.264.  Note H.264 start code can
**              be 3 or 4 bytes, either 0x000001 or 0x00000001.
**
** Arguments:
**
** [f_pbPayload]            -- Pointer to a packet's payload
** [f_cbPayload]            -- The length of the payload to search.
** [f_pdwOffset]            -- The offset of the payload to start the search.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the start code's offset.
** [f_pbLast]               -- Out parameter to save the last byte searched in
**                             case this H264 uses 4-byte start code.
**
** Returns:                 TURE - found the start code 
**                          FALSE - didn't find the start code
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _H264_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )                 DRM_BYTE            *f_pbLast ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _H264_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )                 DRM_BYTE            *f_pbLast )  
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fFoundStartCode = FALSE;
    DRM_DWORD   cbRemaining     = 0;

    DRMASSERT( f_pbPayload != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    DRMASSERT( f_pdwStartCodeOffset != NULL );
    DRMASSERT( f_pbLast != NULL );
    __analysis_assume( f_pbPayload != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    __analysis_assume( f_pdwStartCodeOffset != NULL );
    __analysis_assume( f_pbLast != NULL );

    ChkDR( DRM_DWordSub( f_cbPayload, *f_pdwOffset, &cbRemaining ) );

    /* H.264 NAL unit start code is 0x000001 or 0x00000001 */
    for( ; cbRemaining >= H264_NAL_UNIT_SIZE; cbRemaining-- )
    {
        if( f_pbPayload[ *f_pdwOffset ] == 0x00
         && f_pbPayload[ ( *f_pdwOffset ) + 1 ] == 0x00
         && f_pbPayload[ ( *f_pdwOffset ) + 2 ] == 0x01
         && ( f_pbPayload[ ( *f_pdwOffset ) + 3 ] & H264_NAL_UNIT_TYPE_MASK ) == H264_NAL_TYPE_ACCESS_UNIT_DELIMITER )
        {
            if( *f_pbLast == 0x00 )
            {
                /* the start code is 4 bytes, so the NAL unit should start from previous byte */
                ChkDR( DRM_DWordDecOne( *f_pdwStartCodeOffset ) );
            }
            fFoundStartCode = TRUE;
            break;
        }
        *f_pbLast = f_pbPayload[ *f_pdwOffset ];
        ChkDR( DRM_DWordIncOne( *f_pdwStartCodeOffset ) );
        ChkDR( DRM_DWordIncOne( *f_pdwOffset ) );
    }

ErrorExit:
    return fFoundStartCode;
}


/**********************************************************************
**
** Function:    _MPEG2_VIDEO_SearchStartCode
**
** Synopsis:    Searches the picture start code 0x00000100 or sequence header
**              code 0x000001B3 of MPEG2 video
**
** Arguments:
**
** [f_pbPayload]            -- Pointer to a packet's payload
** [f_cbPayload]            -- The length of the payload to search.
** [f_pdwOffset]            -- The offset of the payload to start the search.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the start code's offset.
** [f_pbLast]               -- not used.
**
** Returns:                 TURE - found the start code 
**                          FALSE - didn't find the start code
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _MPEG2_VIDEO_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _MPEG2_VIDEO_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast )  
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fFoundStartCode = FALSE;
    DRM_DWORD   cbRemaining     = 0;

    DRMASSERT( f_pbPayload != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    DRMASSERT( f_pdwStartCodeOffset != NULL );
    DRMASSERT( f_pbLast != NULL );
    __analysis_assume( f_pbPayload != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    __analysis_assume( f_pdwStartCodeOffset != NULL );
    __analysis_assume( f_pbLast != NULL );

    ChkDR( DRM_DWordSub( f_cbPayload, *f_pdwOffset, &cbRemaining ) );

    /* MPEG2 video's start code is 0x000001 and one byte of start code value */
    for( ; cbRemaining >= MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE; cbRemaining-- )
    {
        if( f_pbPayload[ *f_pdwOffset ] == 0x00
         && f_pbPayload[ ( *f_pdwOffset ) + 1 ] == 0x00
         && f_pbPayload[ ( *f_pdwOffset ) + 2 ] == 0x01
         && (  f_pbPayload[ ( *f_pdwOffset ) + 3 ] == MPEG2_VIDEO_SCV_PICTURE_STRAT_CODE 
            || f_pbPayload[ ( *f_pdwOffset ) + 3 ] == MPEG2_VIDEO_SCV_SEQUENCE_HEADER_CODE  ) )
        {
            fFoundStartCode = TRUE;
            break;
        }
        ChkDR( DRM_DWordIncOne( *f_pdwStartCodeOffset ) );
        ChkDR( DRM_DWordIncOne( *f_pdwOffset ) );
    }

ErrorExit:
    UNREFERENCED_PARAMETER( f_pbLast );
    return fFoundStartCode;
}

#if DEFINE_AUDIO_SEARCH_START_CODE_FUNCTIONS

/**********************************************************************
**
** Function:    _MPEGA_AUDIO_SearchStartCode
**
** Synopsis:    Searches the syncword 0xFFE of MPEGA audio
**
** Arguments:
**
** [f_pbPayload]            -- Pointer to a packet's payload
** [f_cbPayload]            -- The length of the payload to search.
** [f_pdwOffset]            -- The offset of the payload to start the search.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the start code's offset.
** [f_pbLast]               -- not used.
**
** Returns:                 TURE - found the start code 
**                          FALSE - didn't find the start code
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _MPEGA_AUDIO_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _MPEGA_AUDIO_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast )  
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fFoundStartCode = FALSE;
    DRM_DWORD   cbRemaining     = 0;

    DRMASSERT( f_pbPayload != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    DRMASSERT( f_pdwStartCodeOffset != NULL );
    DRMASSERT( f_pbLast != NULL );
    __analysis_assume( f_pbPayload != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    __analysis_assume( f_pdwStartCodeOffset != NULL );
    __analysis_assume( f_pbLast != NULL );

    ChkDR( DRM_DWordSub( f_cbPayload, *f_pdwOffset, &cbRemaining ) );

    /* MPEG-1 AUDIO's syncword is a 11-bit field (0xFFE) */
    for( ; cbRemaining >= MPEGA_AUDIO_SYNCWORD_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_MPEGA_SYNC_WORD( f_pbPayload, f_pdwOffset ) )
        {
            fFoundStartCode = TRUE;
            break;
        }
        ChkDR( DRM_DWordIncOne( *f_pdwStartCodeOffset ) );
        ChkDR( DRM_DWordIncOne( *f_pdwOffset ) );
    }

ErrorExit:
    UNREFERENCED_PARAMETER( f_pbLast );
    return fFoundStartCode;
}

/**********************************************************************
**
** Function:    _ADTS_SearchStartCode
**
** Synopsis:    Searches syncword 0xFFF of ADTS
**
** Arguments:
**
** [f_pbPayload]            -- Pointer to a packet's payload
** [f_cbPayload]            -- The length of the payload to search.
** [f_pdwOffset]            -- The offset of the payload to start the search.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the start code's offset.
** [f_pbLast]               -- not used.
**
** Returns:                 TURE - found the start code 
**                          FALSE - didn't find the start code
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _ADTS_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _ADTS_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast )  
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fFoundStartCode = FALSE;
    DRM_DWORD   cbRemaining     = 0;

    DRMASSERT( f_pbPayload != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    DRMASSERT( f_pdwStartCodeOffset != NULL );
    DRMASSERT( f_pbLast != NULL );
    __analysis_assume( f_pbPayload != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    __analysis_assume( f_pdwStartCodeOffset != NULL );
    __analysis_assume( f_pbLast != NULL );

    ChkDR( DRM_DWordSub( f_cbPayload, *f_pdwOffset, &cbRemaining ) );
    /* ADTS's syncword is 12 bits, 1111 1111 1111 (0xFFF) */
    for( ; cbRemaining >= ADTS_SYNCWORD_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_ADTS_SYNC_WORD( f_pbPayload, f_pdwOffset ) )
        {
            fFoundStartCode = TRUE;
            break;
        }
        ChkDR( DRM_DWordIncOne( *f_pdwStartCodeOffset ) );
        ChkDR( DRM_DWordIncOne( *f_pdwOffset ) );
    }

ErrorExit:
    UNREFERENCED_PARAMETER( f_pbLast );
    return fFoundStartCode;
}

/**********************************************************************
**
** Function:    _DDPLUS_SearchStartCode
**
** Synopsis:    Searches syncword 0x0B77 of DDPlus
**
** Arguments:
**
** [f_pbPayload]            -- Pointer to a packet's payload
** [f_cbPayload]            -- The length of the payload to search.
** [f_pdwOffset]            -- The offset of the payload to start the search.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the start code's offset.
** [f_pbLast]               -- not used.
**
** Returns:                 TURE - found the start code 
**                          FALSE - didn't find the start code
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _DDPLUS_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _DDPLUS_SearchStartCode(
    __in_bcount( f_cbPayload )    const DRM_BYTE            *f_pbPayload,
    __in                          const DRM_DWORD            f_cbPayload,
    __inout                             DRM_DWORD           *f_pdwOffset, 
    __inout                             DRM_DWORD           *f_pdwStartCodeOffset,
    __inout_ecount( 1 )           const DRM_BYTE            *f_pbLast )  
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fFoundStartCode = FALSE;
    DRM_DWORD   cbRemaining     = 0;

    DRMASSERT( f_pbPayload != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    DRMASSERT( f_pdwStartCodeOffset != NULL );
    DRMASSERT( f_pbLast != NULL );
    __analysis_assume( f_pbPayload != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    __analysis_assume( f_pdwStartCodeOffset != NULL );
    __analysis_assume( f_pbLast != NULL );

    ChkDR( DRM_DWordSub( f_cbPayload, *f_pdwOffset, &cbRemaining ) );

    /* DDPlus' syncword is 0x0B77 */
    for( ; cbRemaining >= DDPLUS_SYNCWORD_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_DDPLUS_SYNC_WORD( f_pbPayload, f_pdwOffset ) )
        {
            fFoundStartCode = TRUE;
            break;
        }
        ChkDR( DRM_DWordIncOne( *f_pdwStartCodeOffset ) );
        ChkDR( DRM_DWordIncOne( *f_pdwOffset ) );
    }

ErrorExit:
    UNREFERENCED_PARAMETER( f_pbLast );
    return fFoundStartCode;
}

#endif /* DEFINE_AUDIO_SEARCH_START_CODE_FUNCTIONS */

/**********************************************************************
**
** Function:    _UpdateOveflowPackets
**
** Synopsis:    update the cbPayloadLastPES field in the packet data
**
** Arguments:
**
** [f_pStream]              -- Pointer to a stream that its current PES 
**                             packets' cbPayloadLastPES field will be updated.
** [f_cbOverflow]           -- The length of the overflow payload contained in 
**                             current PES packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _UpdateOveflowPackets(
    __inout         DRM_M2TS_STREAM         *f_pStream,
    __in            DRM_DWORD                f_cbOverflow ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _UpdateOveflowPackets(
    __inout         DRM_M2TS_STREAM         *f_pStream,
    __in            DRM_DWORD                f_cbOverflow )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_DWORD            cbOverflowedRemaining  = f_cbOverflow;
    DRM_M2TS_PACKET     *pPacket                = NULL;
    DRM_BYTE             cbData                 = 0;

    ChkArg( f_pStream != NULL );
    ChkArg( f_pStream->pLastPES != NULL );

    pPacket = f_pStream->oPacketList.pHead;
    f_pStream->pLastPES->pHeadOfOverflowedPackets = pPacket;
    while( pPacket != NULL && cbOverflowedRemaining > 0 )
    {
        ChkDR( DRM_ByteSub( pPacket->cbPayload, pPacket->cbHeader, &cbData ) );
        ChkDR( DRM_DWordToByte( min( cbData, cbOverflowedRemaining ), &pPacket->cbPayloadLastPES ) );
        ChkDR( DRM_DWordSub( cbOverflowedRemaining, pPacket->cbPayloadLastPES, &cbOverflowedRemaining ) );
        if( cbOverflowedRemaining == 0 )
        {
            pPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET;
        }

        pPacket = pPacket->pNextPerUnit;
    }

    DRMASSERT( cbOverflowedRemaining == 0 );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _UpdatePESResult
**
** Synopsis:    If the error is mapped to DRM_E_M2TS_DROP_PES, set the
**              error to drPESResult field in the PES and return DEM_S_FALSE
**              to the caller so the encryption can continue.  The PES
**              with DRM_E_M2TS_DROP_PES error will be dropped later
**              by ECM builder.
**
** Arguments:
**
** [f_pPES]                 -- Pointer to a PES that its drPESResult field will 
**                             be updated.
** [f_pdrResult]            -- The result of the PES.
**
** Returns:                 none
**
**********************************************************************/
static DRM_VOID _UpdatePESResult(
    __inout_opt     DRM_M2TS_PES        *f_pPES,
    __inout         DRM_RESULT          *f_pdrResult )
{
    DRMASSERT( f_pdrResult != NULL );
    __analysis_assume( f_pdrResult != NULL );

    if( f_pPES != NULL 
     && DRM_E_M2TS_DROP_PES == DRM_M2TS_TranslateError( *f_pdrResult ) )
    {
        /* Preserve the original error if this is not the first error against the PES */
        f_pPES->drPESResult = DRM_FAILED( f_pPES->drPESResult ) ? f_pPES->drPESResult : *f_pdrResult;
        f_pPES->fHasSample = FALSE;

        *f_pdrResult = DRM_S_FALSE;
    }
}

/**********************************************************************
**
** Function:    _CheckLastVideoPESCompleteness
**
** Synopsis:    Checks the completeness of previous video PES
**
** Arguments:
**
** [f_pfnSearchStartCode]   -- Pointer to a callback function to search 
**                             the start code from current PES' packets.
** [f_pStream]              -- Pointer to a stream that contains the PES packet.
** [f_pfPESCompleted]       -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_INTERNAL_ERROR if something unexpected
**                              happened.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CheckLastVideoPESCompleteness(
    __in      const DRM_pfnSearchStartCodeCallback       f_pfnSearchStartCode,
    __inout         DRM_M2TS_STREAM                     *f_pStream,
    __out           DRM_BOOL                            *f_pfPESCompleted ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CheckLastVideoPESCompleteness(
    __in      const DRM_pfnSearchStartCodeCallback       f_pfnSearchStartCode,
    __inout         DRM_M2TS_STREAM                     *f_pStream,
    __out           DRM_BOOL                            *f_pfPESCompleted )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    const DRM_M2TS_PACKET   *pPacket            = NULL;
    DRM_DWORD                dwStartCodeOffset  = 0;
    DRM_DWORD                cbData             = 0;
    DRM_DWORD                dwBufferOffset     = 0;
    DRM_DWORD                cbBuffer           = 0;
    DRM_BYTE                 rgbBuffer[ DRM_M2TS_PACKET_SIZE * 2 ] = { 0 };
    DRM_BYTE                 bLast              = DRM_M2TS_STUFFING_BYTE;     /* Any value except 0x00 */

    DRMASSERT( f_pfnSearchStartCode != NULL );
    DRMASSERT( f_pStream != NULL );
    DRMASSERT( f_pfPESCompleted != NULL );
    __analysis_assume( f_pfnSearchStartCode != NULL );
    __analysis_assume( f_pStream != NULL );
    __analysis_assume( f_pfPESCompleted != NULL );

    pPacket = f_pStream->oPacketList.pHead;

    DRMASSERT( f_pStream->ePESState != eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD );
    AssertChkArg( f_pStream->pLastPES != NULL );

    *f_pfPESCompleted = FALSE;

    while( !*f_pfPESCompleted
        && ( pPacket != NULL && !*f_pfPESCompleted ) )
    {
        /* Remove all the bytes that have been checked by shifting them out of the buffer */
        if( dwBufferOffset > 0 )
        {
            ChkDR( DRM_DWordSub( cbBuffer, dwBufferOffset, &cbBuffer ) );
            MEMMOVE( rgbBuffer, rgbBuffer + dwBufferOffset, cbBuffer );
            dwBufferOffset = 0;
        }

        if( DRM_M2TS_IS_START_WITH_PARTIAL_START_CODE( pPacket ) )
        {
            /*
            ** In a case where the head packet starts with a partial start code of last PES,
            ** we don't want to search the start code from the beginning because H264's start
            ** code can be either 3 or 4 bytes (0x000001 or 0x00000001).  If the start code is 4-byte
            ** and the head packet start with 0x000001, it will be mistaken as the start code of
            ** current PES.  So we want to set dwBufferOffset to 1 to skip the first byte.
            */
            ChkBOOL( dwBufferOffset == 0, DRM_E_M2TS_INTERNAL_ERROR );
            ChkBOOL( dwStartCodeOffset == 0, DRM_E_M2TS_INTERNAL_ERROR );
            ChkBOOL( pPacket == f_pStream->oPacketList.pHead, DRM_E_M2TS_INTERNAL_ERROR );
            dwBufferOffset = 1;
            dwStartCodeOffset = 1;
        }

        /* Copy the payload from packet to the buffer excluding the PES header */
        DRMASSERT( pPacket->cbPayloadLastPES == 0 );
        ChkDR( DRM_DWordSub( pPacket->cbPayload, pPacket->cbHeader, &cbData ) );
        if( cbData > 0 )
        {
            DRM_DWORD dwOffset = 0;
            ChkDR( DRM_DWordSub( DRM_M2TS_PACKET_SIZE, cbData, &dwOffset ) );
            MEMCPY( &rgbBuffer[ cbBuffer ], 
                    &pPacket->rgbData[ dwOffset ], 
                    cbData );
            ChkDR( DRM_DWordAdd( cbBuffer, cbData, &cbBuffer ) );

            /* Search the start code of current PES */
            if( f_pfnSearchStartCode( rgbBuffer, 
                                      cbBuffer, 
                                      &dwBufferOffset, 
                                      &dwStartCodeOffset, 
                                      &bLast ) )
            {
                /* Found the start code */
                ChkDR( _UpdateOveflowPackets( f_pStream, dwStartCodeOffset ) );
                *f_pfPESCompleted = TRUE;
            }
        }

        pPacket = pPacket->pNextPerUnit;
    }

ErrorExit:
    _UpdatePESResult( f_pStream->pLastPES, &dr );
    if( f_pStream->pLastPES != NULL && DRM_FAILED( f_pStream->pLastPES->drPESResult ) )
    {
        /* 
        ** Something wrong with the PES content.
        ** Treat it as completed so the encryptor can drop it later.
        */
        *f_pfPESCompleted = TRUE;
    }
   return dr;
}

/**********************************************************************
**
** Function:    _CheckLastAudioPESCompleteness
**
** Synopsis:    Checks the completeness of previous audio PES
**
** Arguments:
**
** [f_pfnSyncAndParseFrameHeader]   
**                          -- Pointer to a callback function to search 
**                             the start code from current PES' packets.
** [f_pStream]              -- Pointer to a stream that contains the PES packet.
** [f_pfPESCompleted]       -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_ARITHMETIC_OVERFLOW
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CheckLastAudioPESCompleteness(
    __in      const DRM_pfnSyncAndParseFrameHeaderCallback   f_pfnSyncAndParseFrameHeader,
    __inout         DRM_M2TS_STREAM                         *f_pStream,
    __out           DRM_BOOL                                *f_pfPESCompleted ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CheckLastAudioPESCompleteness(
    __in      const DRM_pfnSyncAndParseFrameHeaderCallback   f_pfnSyncAndParseFrameHeader,
    __inout         DRM_M2TS_STREAM                         *f_pStream,
    __out           DRM_BOOL                                *f_pfPESCompleted )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_RESULT                   drSyncAndParse     = DRM_SUCCESS;
    DRM_M2TS_PACKET             *pPacket            = NULL;
    DRM_M2TS_PACKET             *pOverflowPacket    = NULL;
    DRM_M2TS_SUBSAMPLE_CONTEXT   oSubsampleContext  = { 0 };
    DRM_WORD                     cbAdvance          = 0;
    DRM_WORD                     cbFrameRemaining   = 0;
    DRM_WORD                     wSyncOffset        = 0;
    DRM_BOOL                     fSynced            = FALSE;
    DRM_UINT64                   qwDuration         = DRM_UI64HL( 0, 0 );

    ChkArg( f_pStream != NULL );
    DRMASSERT( f_pfnSyncAndParseFrameHeader != NULL );
    DRMASSERT( f_pStream->pLastPES != NULL );
    DRMASSERT( f_pfPESCompleted != NULL );
    __analysis_assume( f_pfnSyncAndParseFrameHeader != NULL );
    __analysis_assume( f_pStream->pLastPES != NULL );
    __analysis_assume( f_pfPESCompleted != NULL );

    pPacket = f_pStream->pLastPES->oPacketList.pHead;
    pOverflowPacket = f_pStream->oPacketList.pHead;

    while( !*f_pfPESCompleted
        && ( cbAdvance > 0 || pPacket != NULL || pOverflowPacket != NULL ) )
    {
        cbAdvance = 0;

        ChkDR( DRM_M2TS_FillSubsampleContextBuffer( TRUE, &oSubsampleContext, &pPacket, &pOverflowPacket ) );

        if( cbFrameRemaining == 0 )
        {
            fSynced = FALSE;
            wSyncOffset = oSubsampleContext.wOffset;
            drSyncAndParse = f_pfnSyncAndParseFrameHeader( &oSubsampleContext,
                                                           &wSyncOffset,
                                                           &fSynced,
                                                           &qwDuration );
            if( fSynced )
            {
                DRM_DWORD dwNewOffset = 0;

                /* The last PES is completed when the overall pointer points to the byte inside the current PES */
                ChkDR( DRM_DWordAdd( oSubsampleContext.dwLastOffset, wSyncOffset, &dwNewOffset ) );
                if( dwNewOffset >= oSubsampleContext.cbCurrentPESOnly )
                {
                    ChkDR( _UpdateOveflowPackets( f_pStream, dwNewOffset - oSubsampleContext.cbCurrentPESOnly ) );
                    *f_pfPESCompleted = TRUE;

                    /* 
                    ** If f_pfnSyncAndParseFrameHeader found the syncword from current PES but failed to parse the frame header
                    ** we wanted to continue because last PES is good.  This is why we delay the ChkDR until now.
                    */
                    ChkDR( drSyncAndParse );
                }
                else
                {
                    ChkDR( drSyncAndParse );

                    DRMASSERT( oSubsampleContext.cbFrameSize != 0 );
                    ChkDR( DRM_DWordToWord( oSubsampleContext.cbFrameSize, &cbFrameRemaining ) );
                }
            }
            else
            {
                ChkDR( drSyncAndParse );

                /* 
                ** We shouldn't be here if the frame size calculation is correct and there is no missing packet. 
                ** Something wrong with the content. Continue searching the syncword.
                */
                ChkDR( DRM_WordAdd( oSubsampleContext.wOffset, wSyncOffset, &oSubsampleContext.wOffset ) );
            }
        }

        if( cbFrameRemaining > 0 )
        {
            ChkBOOL( oSubsampleContext.wOffset <= oSubsampleContext.cbBuffer, DRM_E_M2TS_INTERNAL_ERROR ); 
            cbAdvance = (DRM_WORD)min( cbFrameRemaining, oSubsampleContext.cbBuffer - oSubsampleContext.wOffset );

            ChkDR( DRM_WordSub( cbFrameRemaining, cbAdvance, &cbFrameRemaining ) );
            ChkDR( DRM_WordAdd( oSubsampleContext.wOffset, cbAdvance, &oSubsampleContext.wOffset ) );
        }
    }

ErrorExit:
    if( f_pStream != NULL 
     && ( DRM_FAILED( dr ) || DRM_FAILED( drSyncAndParse ) ) )
    {
        dr = DRM_FAILED( drSyncAndParse ) ? drSyncAndParse : dr;

        /* if *f_pfPESCompleted is true which means the error occured in the current PES, not last PES */
        _UpdatePESResult( *f_pfPESCompleted ? f_pStream->pCurrentPES : f_pStream->pLastPES, &dr );

        /* 
        ** Something wrong with the PES content.
        ** Treat it as completed so the encryptor can drop it later.
        */
        *f_pfPESCompleted = TRUE;
    }
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_HasPayload
**
** Synopsis:    Return true if any of the TS packets belong to the PES 
**              has its own  payload other than PES header and payload 
**              belong to last PES.
**
** Arguments:
**
** [f_pPES]                 -- Pointer to a PES to check its payload.
**
** Returns:                 TRUE - has payload
**                          FALSE - doesn't have payload
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_M2TS_HasPayload(
    __in  const DRM_M2TS_PES *f_pPES )
{
    DRM_BOOL                 fHasPayload    = FALSE;
    const DRM_M2TS_PACKET   *pPacket        = NULL;

    if( f_pPES != NULL )
    {
        pPacket = f_pPES->oPacketList.pHead;
        while( pPacket != NULL && !fHasPayload )
        {
            if( pPacket->cbPayload > pPacket->cbHeader + pPacket->cbPayloadLastPES )
            {
                fHasPayload = TRUE;
            }
            pPacket = pPacket->pNextPerUnit;
        }
    }

    return fHasPayload;
}

/**********************************************************************
**
** Function:    DRM_M2TS_FindVideoStartCode
**
** Synopsis:    Search the data for the start code of the video 
**
** Arguments:
**
** [f_pStream]              -- Pointer to a stream to find the start code
** [f_pbData]               -- Pointer to the byte array to find the start code.
** [f_cbDada]               -- Length of the byte array.
** [f_pdwStartCodeOffset]   -- Out parameter to contain the result.
**
** Returns:                 TRUE - found the start code
**                          FALSE - didn't find the start code
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_M2TS_FindVideoStartCode(
    __in                      const DRM_M2TS_STREAM             *f_pStream,
    __in_bcount( f_cbDada )   const DRM_BYTE                    *f_pbData,
    __in                      const DRM_DWORD                    f_cbDada,
    __out                           DRM_DWORD                   *f_pdwStartCodeOffset )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BOOL    fFound      = FALSE;
    DRM_DWORD   dwOffset    = 0;
    DRM_BYTE    bLast       = DRM_M2TS_STUFFING_BYTE;     /* Any value except 0x00 */

    ChkArg( f_pStream != NULL );
    ChkArg( f_cbDada > 0 );
    ChkArg( f_pbData != NULL );
    ChkArg( f_pdwStartCodeOffset != NULL );

    *f_pdwStartCodeOffset = 0;

    switch( f_pStream->bStreamType )
    {
    case DRM_M2TS_STREAM_TYPE_MPEG2_VIDEO:
    case DRM_M2TS_STREAM_TYPE_DIGICIPHER_II_VIDEO:
        fFound = _MPEG2_VIDEO_SearchStartCode( f_pbData, f_cbDada, &dwOffset, f_pdwStartCodeOffset, &bLast );
        break;

    case DRM_M2TS_STREAM_TYPE_H264:
        fFound = _H264_SearchStartCode( f_pbData, f_cbDada, &dwOffset, f_pdwStartCodeOffset, &bLast );
        break;

    default:
        ChkDR( DRM_E_M2TS_INTERNAL_ERROR );
        break;
    }

ErrorExit:
    return fFound;
}

/**********************************************************************
**
** Function:    DRM_M2TS_CheckLastPESContinuityCounter
**
** Synopsis:    Check whether there is a missing packet in the last
**              PES according to the continuity counter.  If there is
**              a missing packet we will set 
**              DRM_M2TS_PACKET_FLAG_DONT_INCREMENT_CONTINUITY_COUNTER or
**              DRM_M2TS_PACKET_FLAG_DISCONTINUITY_PACKET to drPESResult field 
**              and leave it to the demuxer to decide whether to drop it.
**
** Arguments:
**
** [f_pStream]              -- Pointer to a stream to verify the continuity
**                             counters in its last PES' packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CheckLastPESContinuityCounter(
    __inout         DRM_M2TS_STREAM         *f_pStream )
{
    DRM_RESULT               dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET         *pPacket                = NULL;
    const DRM_M2TS_PACKET   *pPrevPacket            = NULL;
    DRM_BYTE                 bContinuityCounter     = 0;
    DRM_BYTE                 bNextContinuityCounter = 0;
    DRM_BYTE                 bPrevContinuityCounter = 0;
    DRM_BOOL                 fFirstPacket           = TRUE;

    ChkArg( f_pStream != NULL );
    ChkArg( f_pStream->pLastPES != NULL );

    DRMASSERT( f_pStream->bDataAlignmentFlag == DRM_M2TS_PES_DATA_ALIGNED );

    pPacket = f_pStream->pLastPES->oPacketList.pHead;
    while( pPacket != NULL && DRM_SUCCEEDED( f_pStream->pLastPES->drPESResult ) )
    {
        bContinuityCounter = DRM_M2TS_GET_CONTINUITY_COUNTER( pPacket->rgbData );

        if( fFirstPacket )
        {
            fFirstPacket = FALSE;
        }
        else
        {
            if( bContinuityCounter == bPrevContinuityCounter )
            {
                /* 
                ** According to M2TS spec, the continuity counter should not be incremented for the following cases:
                ** 1) adapation field only packet
                ** 2) duplicated packets (for two consecutitive packets only)
                **
                ** If two consecutive TS packets with identical continuity counter but for other
                ** reason than above we will leave it to the demuxes to decide whether to drop it.
                */
                pPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_DONT_INCREMENT_CONTINUITY_COUNTER;
            }
            else if( bNextContinuityCounter != bContinuityCounter )
            {
                if( DRM_M2TS_ADAPTATION_FIELD_ONLY( pPacket->rgbData ) )
                {
                    /* Do not check the continuity counter if the packet is adaptation field only */
                    pPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_DONT_INCREMENT_CONTINUITY_COUNTER;
                }
                else
                {
                    /* 
                    ** Regardless of whether we have the discontinuity flag in the TS packet
                    ** we would leave it to the demuxer to decide whether to drop the PES packet.
                    **
                    ** Unfortunately, the Intel transcode pipeline often includes packets like this
                    ** even though they are non-conformant.  We don't want to drop the whole PES
                    ** packet in this case - let the demuxer decide whether to drop it.
                    */
                    pPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_DISCONTINUITY_PACKET;
                }
            }
            else
            {
                /* 
                ** We don't check to ensure that the packet is not an adaptation field only packet or
                ** a duplicated packet here.  This is for perf consideration and we would leave it to the 
                ** demuxer to decide whether to drop the PES packet if this packet is indeed 
                ** an adaptation field only packet or a duplicated packet.
                */
            }
        }
        bPrevContinuityCounter = bContinuityCounter;
        bNextContinuityCounter = (DRM_BYTE)( ( bContinuityCounter + 1 ) & DRM_M2TS_CONTINUITY_COUNTER_MASK );
        pPrevPacket = pPacket;

        pPacket = pPacket->pNextPerUnit;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _ConvertPESPacketToAligned
**
** Synopsis:    Convert the PES packets from unaligned to aligned.  This
**              function duplicates the overflowed packet in the current
**              PES and append them to the packet list of the previous 
**              PES packet.  The payload in the current and previous PES
**              are adjusted accordingly.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pLastPES]             -- Pointer to the last PES to realign
** [f_poCurrentPESPacketList]  Pointer to the current PES' packet list to realign
** [f_pbDataAlignmentFlag]  -- Pointer to the current PES' data alignment 
**                             indicator to be updated after realignment
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ConvertPESPacketToAligned(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pLastPES,
    __inout     DRM_M2TS_PACKET_LIST            *f_poCurrentPESPacketList,
    __inout     DRM_BYTE                        *f_pbDataAlignmentFlag) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ConvertPESPacketToAligned(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pLastPES,
    __inout     DRM_M2TS_PACKET_LIST            *f_poCurrentPESPacketList,
    __inout     DRM_BYTE                        *f_pbDataAlignmentFlag)
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_M2TS_PACKET         *pPacket        = NULL;
    DRM_M2TS_PACKET         *pNewPacket     = NULL;
    DRM_M2TS_PACKET_LIST     oPacketList    = { 0 };
    DRM_LONG                 cbOverflow     = 0;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pLastPES != NULL );
    DRMASSERT( f_poCurrentPESPacketList != NULL );
    DRMASSERT( f_pbDataAlignmentFlag != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pLastPES != NULL );
    __analysis_assume( f_poCurrentPESPacketList != NULL );
    __analysis_assume( f_pbDataAlignmentFlag != NULL );

    /* Duplicate the packets in the current PES that have the overflow payload */
    pPacket = f_poCurrentPESPacketList->pHead;
    while( pPacket != NULL )
    {
        if( pPacket->cbPayloadLastPES > 0 )
        {
            ChkDR( DRM_LongAdd( cbOverflow, pPacket->cbPayloadLastPES, &cbOverflow ) );

            ChkDR( DRM_M2TS_GetFreePacket( f_pEncContext,
                                           pPacket->cbPayload,
                                           eDRM_M2TS_PACKET_STATE_PENDING,
                                           FALSE,
                                           pPacket->rgbData,
                                           &pNewPacket ) );

            /* Share same packet number as the original packet */
            pNewPacket->dwPacketNumber = pPacket->dwPacketNumber;

            pNewPacket->cbHeader = pPacket->cbHeader;
            pNewPacket->cbPayloadLastPES = pPacket->cbPayloadLastPES;

            DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &oPacketList, pNewPacket );
        }

        if( DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket ) )
        {
            break;
        }

        pPacket = pPacket->pNextPerUnit;
    }

    /* First, update and insert the duplicated packts to the last PES' packet list and incoming packet list */
    if( cbOverflow > 0 && DRM_SUCCEEDED( f_pLastPES->drPESResult ) )
    {
        /* Pre-set an error to the PES in case something goes wrong */ 
        f_pLastPES->drPESResult = DRM_E_M2TS_INCOMPLETE_PES;

        ChkDR( DRM_M2TS_KeepOverflowPayload( f_pEncContext, f_pLastPES, &oPacketList ) );
        
        /* We need to adjust the PES packet lenght to include the overflow bytes */
        ChkDR( DRM_M2TS_UpdatePESPacketLength( f_pEncContext, cbOverflow, &f_pLastPES->oPacketList ) );

        /* Alignment conversion succeeded */
        f_pLastPES->drPESResult = DRM_SUCCESS;
    }

    /* Now, take care of the packets that contain the overflow payload by removing them from current PES. */
    if( cbOverflow > 0 )
    {
        ChkDR( DRM_M2TS_DropOverflowPayload( f_pEncContext, f_poCurrentPESPacketList ) );
        ChkDR( DRM_M2TS_UpdateAlignmentFlag( f_pEncContext, (DRM_BYTE)DRM_M2TS_PES_DATA_ALIGNED, f_poCurrentPESPacketList ) );

        /* We need to adjust the PES packet lenght to exclude the overflow bytes */
        ChkDR( DRM_M2TS_UpdatePESPacketLength( f_pEncContext, -1 * cbOverflow, f_poCurrentPESPacketList ) );
    }

    *f_pbDataAlignmentFlag = DRM_M2TS_PES_DATA_ALIGNED;

ErrorExit:

    if( f_pEncContext != NULL )
    {
        /* recycle the packets */
        while( oPacketList.cItems > 0 )
        {
            DRM_M2TS_POP_FROM_TOP_PER_UNIT_LIST_NO_FAIL( &oPacketList, DRM_M2TS_PACKET, pPacket );
            DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL( &f_pEncContext->oFreePacketList, pPacket );
        }
    }

    return dr;
}

/**********************************************************************
**
** Function:    _ConvertLastPESToAligned
**
** Synopsis:    Convert the last PES to aligned (data alignment indicator to 1)
**              The function is for the PES that doesn't have its own payload or
**              contains just partial of the start code of the next PES.  Since,
**              this type of PES can happen consecutively so this function is 
**              recursion.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pCurrentPES]          -- Pointer to current PES packet.
** [f_pLastPES]             -- Pointer to the last (previous) PES packet.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ConvertLastPESToAligned(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pCurrentPES,
    __inout     DRM_M2TS_PES                    *f_pLastPES) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ConvertLastPESToAligned(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_PES                    *f_pCurrentPES,
    __inout     DRM_M2TS_PES                    *f_pLastPES)
{
    DRM_RESULT          dr      = DRM_SUCCESS;
    DRM_M2TS_STREAM     oStream = { 0 };

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pCurrentPES != NULL );
    DRMASSERT( f_pLastPES != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pCurrentPES != NULL );
    __analysis_assume( f_pLastPES != NULL );

    if( f_pLastPES->pLastPESNeedAlignment != NULL )
    {
        /* 
        ** Occurs when there are consecutive PES packets that don't have its own payload or 
        ** just contains partial start code of the next PES packet.  We need to find the 
        ** the very first one of the consecutive PES packets and work its way down.
        */
        ChkDR( _ConvertLastPESToAligned( f_pEncContext, f_pLastPES, f_pLastPES->pLastPESNeedAlignment ) );
    }

    ChkBOOL( f_pLastPES->fNeedRealignment, DRM_E_M2TS_INTERNAL_ERROR );
    ChkDR( _ConvertPESPacketToAligned( f_pEncContext, 
                                       f_pLastPES, 
                                       &f_pCurrentPES->oPacketList, 
                                       &f_pCurrentPES->bDataAlignmentFlag ) );

    f_pLastPES->fNeedRealignment = FALSE;
    f_pCurrentPES->pLastPESNeedAlignment = NULL;

    DRMASSERT( f_pLastPES->bDataAlignmentFlag == DRM_M2TS_PES_DATA_ALIGNED );

    /* Check whether there is a missing packet in last PES according to the continuity counter */
    oStream.pLastPES = f_pLastPES;
    oStream.bDataAlignmentFlag = f_pLastPES->bDataAlignmentFlag;
    ChkDR( DRM_M2TS_CheckLastPESContinuityCounter( &oStream ) );

    f_pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_CheckPESCompleteness
**
** Synopsis:    Checks the compleness of previous PES in the stream.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a stream to verify the completeness
**                             of its last PES.
** [f_pfPESCompleted]       -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_INVALID_UNALIGNED_DATA when a PES'
**                              overflow payload spreads over more than one
**                              PES packet.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CheckPESCompleteness(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_STREAM                 *f_pStream,
    __out           DRM_BOOL                        *f_pfPESCompleted )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_pStream != NULL );
    ChkArg( f_pfPESCompleted != NULL );

    *f_pfPESCompleted = FALSE;

    /* Check the completeness of previous PES */

    AssertChkArg( f_pStream->pLastPES != NULL );
    DRMASSERT( f_pStream->pLastPES->ePESState == eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER );

    /* 
    ** For (last) PES with unbounded length which will be completeness if
    ** 1) Current PES's data alignmnet flag is '1', or
    ** 2) Start code or sync word is found in the current PES
    */
    if( f_pStream->bDataAlignmentFlag == DRM_M2TS_PES_DATA_ALIGNED )
    {
        /* Check whether there is a missing packet in last PES according to the continuity counter */
        ChkDR( DRM_M2TS_CheckLastPESContinuityCounter( f_pStream ) );

        /* Current PES' data alignment flag is '1' which means the last PES is completed now */
        f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;

        *f_pfPESCompleted = TRUE;
    }
    else
    {
        switch( f_pStream->bStreamType )
        {
        case DRM_M2TS_STREAM_TYPE_MPEG2_VIDEO:
        case DRM_M2TS_STREAM_TYPE_DIGICIPHER_II_VIDEO:
            ChkDR( _CheckLastVideoPESCompleteness( (DRM_pfnSearchStartCodeCallback)&_MPEG2_VIDEO_SearchStartCode,
                                                   f_pStream,
                                                   f_pfPESCompleted ) );
            break;

        case DRM_M2TS_STREAM_TYPE_H264:
            ChkDR( _CheckLastVideoPESCompleteness( &_H264_SearchStartCode,
                                                   f_pStream,
                                                   f_pfPESCompleted ) );
            break;

        case DRM_M2TS_STREAM_TYPE_MPEGA_AUDIO:
            ChkDR( _CheckLastAudioPESCompleteness( &DRM_M2TS_SyncAndParseMPEGAAudioFrameHeaderCallback,
                                                   f_pStream,
                                                   f_pfPESCompleted ) );
            break;

        case DRM_M2TS_STREAM_TYPE_ADTS:
            ChkDR( _CheckLastAudioPESCompleteness( &DRM_M2TS_SyncAndParseADTSFrameHeaderCallback,
                                                   f_pStream,
                                                   f_pfPESCompleted ) );
            break;

        case DRM_M2TS_STREAM_TYPE_DDPLUS:
            ChkDR( _CheckLastAudioPESCompleteness( &DRM_M2TS_SyncAndParseDDPlusFrameHeaderCallback,
                                                   f_pStream,
                                                   f_pfPESCompleted ) );
            break;

        default:
            ChkDR( DRM_E_M2TS_INTERNAL_ERROR );
            break;
        }
    }

    if( *f_pfPESCompleted )
    {
        if( f_pStream->pLastPES->pLastPESNeedAlignment != NULL )
        {
            ChkDR( _ConvertLastPESToAligned( f_pEncContext, f_pStream->pLastPES, f_pStream->pLastPES->pLastPESNeedAlignment ) );
        }

        if( f_pStream->bDataAlignmentFlag != DRM_M2TS_PES_DATA_ALIGNED )
        {
            DRMASSERT( f_pStream->pLastPES->pLastPESNeedAlignment == NULL );
            ChkDR( _ConvertPESPacketToAligned( f_pEncContext, 
                                               f_pStream->pLastPES, 
                                               &f_pStream->oPacketList, 
                                               &f_pStream->bDataAlignmentFlag ) );

            /* Check whether there is a missing packet in last PES according to the continuity counter */
            ChkDR( DRM_M2TS_CheckLastPESContinuityCounter( f_pStream ) );

            f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
        }

        f_pStream->pLastPES = NULL;
    }
    
ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_MPEG2_VIDEO_CreateSubsampleMappings
**
** Synopsis:    Create subsample mappings based on the TS packets on
**              the PES.  Only the picture slides are in protected subsamples, 
**              all the others are in clear subsamples.  It starts a new sample
**              whenever it finds picture start code (0x00) and sequence header
**              code (0xB3).
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pPES]                 -- Pointer to a PES from where we will create the 
**                             the subsamples.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_MPEG2_VIDEO_CreateSubsampleMappings(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_SUBSAMPLE_CONTEXT      *f_poSubsampleContext,
    __inout     DRM_M2TS_PES                    *f_pPES )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    const DRM_BYTE      *pbBuffer           = NULL;
    DRM_M2TS_SAMPLE     *pSample            = NULL;
    DRM_M2TS_SUBSAMPLE  *pSubsample         = NULL;
    DRM_WORD             wSyncOffset        = 0;
    DRM_WORD             cbRemaining        = 0;
    DRM_BYTE             bStartCodeValue    = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pPES != NULL );

    ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );

    pbBuffer = f_poSubsampleContext->rgbBuffer;

    if( !f_pPES->fHasSample )
    {
        /* Assign the first sample and subsample for the PES */
        ChkDR( _AddNewSample( f_pEncContext, f_pPES, &pSample ) );
        ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
    }
    else
    {
        /* Already have the sample and subsample assigned, continue to use the last sample and subsample */
        pSample = &f_pEncContext->pSamples[ f_pPES->dwIndexLastSample ];
        pSubsample = &f_pEncContext->pSubsamples[ pSample->dwIndexLastSubsample ];
    }

    wSyncOffset = f_poSubsampleContext->wOffset;

    while( cbRemaining >= MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE )
    {
        /* search for the start code 0x000001 */
        if( pbBuffer[ wSyncOffset ] == 0x00 
         && pbBuffer[ wSyncOffset + 1 ] == 0x00
         && pbBuffer[ wSyncOffset + 2 ] == 0x01 )
        {
            /* Found the start code. Next step is to check the start code value */

            bStartCodeValue = pbBuffer[ wSyncOffset + 3 ];

            if( bStartCodeValue >= MPEG2_VIDEO_SCV_FIRST_SLICE_START_CODE
             && bStartCodeValue <= MPEG2_VIDEO_SCV_LAST_SLICE_START_CODE )
            {
                if( f_poSubsampleContext->fProcessProtected )
                {
                    /* Remain in protected, the start code of the slice data should be encrypted */
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE );
                }
                else
                {
                    /* Switch to protected if it is not already */
                    f_poSubsampleContext->fProcessProtected = TRUE;

                    /* Add the start code to the current protected subsample */
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE );
                }
            }
            else if( bStartCodeValue == MPEG2_VIDEO_SCV_PICTURE_STRAT_CODE
                  || bStartCodeValue == MPEG2_VIDEO_SCV_SEQUENCE_HEADER_CODE )
            {
                if( pSubsample->cbEncryptedBytes > 0 )
                {
                    /* Start of a new sample.  This PES contains multiple samples. */
                    ChkDR( _EnsureProtectedOnDWordBoundary( f_pEncContext, pSample ) );
                    ChkDR( _AddNewSample( f_pEncContext, f_pPES, &pSample ) );
                    ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
                }

                /* Add the start code to the new subsample */
                f_poSubsampleContext->fProcessProtected = FALSE;
                DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE );
            }
            else
            {
                /* Other NAL unit types that should be in clear */
                if( f_poSubsampleContext->fProcessProtected )
                {
                    /* Switch to clear, so start a new subsample */
                    ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
                    f_poSubsampleContext->fProcessProtected = FALSE;
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE );
                }
                else
                {
                    /* Continue to be in clear, just add the start code to the last subsample's clear bytes */
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE );
                }
            }

            ChkDR( DRM_WordAdd( wSyncOffset, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE, &wSyncOffset ) );
            ChkDR( DRM_WordSub( cbRemaining, MPEG2_VIDEO_START_CODE_AND_VALUE_SIZE, &cbRemaining ) );
        }
        else
        {
            DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, 1 );
            ChkDR( DRM_WordIncOne( wSyncOffset ) );
            ChkDR( DRM_WordDecOne( cbRemaining ) );
        }
    }

    f_poSubsampleContext->wOffset = wSyncOffset; 

    /* 
    ** This is the last call, put all the remaining bytes to the last subsample.
    ** If it is in protected mode then all the remaining bytes will be counted as protected bytes,
    ** otherwise as clear bytes. 
    */
    if( f_poSubsampleContext->fLastCall )
    {
        VERIFY_BYTE_COUNT_OF_REMAINING_BYTES_IN_SUBSAMPLE_CONTEXT_BUFFER( f_poSubsampleContext, cbRemaining );
        DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbRemaining );

        ChkDR( _EnsureProtectedOnDWordBoundary( f_pEncContext, pSample ) );
    }

ErrorExit:
    _UpdatePESResult( f_pPES, &dr );
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_H264_CreateSubsampleMappings
**
** Synopsis:    Create subsample mappings based on the TS packets on the
**              PES.  Only the IDR and non-IDR NAL units are in protected
**              subsamples, all the others are in clear subsamples.  It 
**              starts a new sample when it finds the access unit delimiter.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pPES]                 -- Pointer to a PES from where we will create the 
**                             the subsamples.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_H264_CreateSubsampleMappings(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT      *f_poSubsampleContext,
    __inout         DRM_M2TS_PES                    *f_pPES )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    const DRM_BYTE      *pbBuffer       = NULL;
    DRM_M2TS_SAMPLE     *pSample        = NULL;
    DRM_M2TS_SUBSAMPLE  *pSubsample     = NULL;
    DRM_WORD             wSyncOffset    = 0;
    DRM_WORD             cbLast         = 0;
    DRM_WORD             cbRemaining    = 0;
    DRM_BYTE             bNalUnitType   = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pPES != NULL );

    pbBuffer       = f_poSubsampleContext->rgbBuffer;
    wSyncOffset    = f_poSubsampleContext->wOffset;
    ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );

    if( !f_pPES->fHasSample )
    {
        /* Assing the first sample and subsample for the PES */
        ChkDR( _AddNewSample( f_pEncContext, f_pPES, &pSample ) );
        ChkDR( _AddNewSubsample( f_pEncContext, pSample,  &pSubsample ) );
    }
    else
    {
        /* Already have the sample and subsample assigned, continue to use the last sample and subsample */
        pSample = &f_pEncContext->pSamples[ f_pPES->dwIndexLastSample ];
        pSubsample = &f_pEncContext->pSubsamples[ pSample->dwIndexLastSubsample ];
    }

    while( cbRemaining >= H264_NAL_UNIT_SIZE )
    {
        /* search for the start code 0x000001 */
        if( pbBuffer[ wSyncOffset ] == 0x00 
         && pbBuffer[ wSyncOffset + 1 ] == 0x00
         && pbBuffer[ wSyncOffset + 2 ] == 0x01 )
        {
            /* Found a start code.  The next step is to check the NAL unit type. */

            bNalUnitType = (DRM_BYTE)( pbBuffer[ wSyncOffset + 3 ] & H264_NAL_UNIT_TYPE_MASK );
            f_poSubsampleContext->cbCountdownToProtected = 0;

            cbLast = f_poSubsampleContext->bLast == 0x00 ? (DRM_WORD)1 : (DRM_WORD)0;
            if( cbLast == 1 
             && ( ( f_poSubsampleContext->fProcessProtected && pSubsample->cbEncryptedBytes > 0 ) || 
                  (!f_poSubsampleContext->fProcessProtected && pSubsample->cbClearBytes > 0 ) ) )
            {
                /* the start code is 4-byte, 0x00000001, so reduce the last subsample byte count by 1 */
                DRM_M2TS_DECREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbLast );
            }

            if( bNalUnitType == H264_NAL_TYPE_ACCESS_UNIT_DELIMITER )
            {
                if( pSubsample->cbEncryptedBytes > 0 )
                {
                    /* Start of a new sample.  This PES contains multiple samples. */
                    ChkDR( _AddNewSample( f_pEncContext, f_pPES, &pSample ) );
                    ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
                }

                /* Add the start code to the new subsample */
                f_poSubsampleContext->fProcessProtected = FALSE;
                DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, (DRM_DWORD)H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast );

                pSubsample->fAdjustable = TRUE;
                /* Since H264_NAL_DATA_IN_CLEAR includes NAL length and nal_unit_type fields so we need to subtract it. */
                ChkDR( DRM_DWordToByte( (DRM_DWORD)( (DRM_DWORD)H264_NAL_DATA_IN_CLEAR + 1 - (DRM_DWORD)( H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast ) ), &f_poSubsampleContext->cbCountdownToProtected ) );
            }
            else if( bNalUnitType == H264_NAL_TYPE_SEI
                  || bNalUnitType == H264_NAL_TYPE_SPS
                  || bNalUnitType == H264_NAL_TYPE_PPS )
            {
                /* NAL unit types that should be in clear */
                if( f_poSubsampleContext->fProcessProtected )
                {
                    /* Switch to clear, so start a new subsample */
                    ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
                    f_poSubsampleContext->fProcessProtected = FALSE;
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, (DRM_DWORD)H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast );
                }
                else
                {
                    /* Continue to be in clear, just add the start code to the last subsample */
                    DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, (DRM_DWORD)H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast );
                }
            }
            else
            {
                /* NAL unit types that should be in protected */
                if( f_poSubsampleContext->fProcessProtected )
                {
                    /* The start code needs to be in clear so we need to start a new subsample */
                    ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );

                    f_poSubsampleContext->fProcessProtected = FALSE;
                }

                /* Add the start code to the current subsample */
                DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, (DRM_DWORD)H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast );

                /* Remain in clear for the next number of bytes defined by H264_NAL_DATA_IN_CLEAR then switch to protected */
                pSubsample->fAdjustable = TRUE;
                /* Since H264_NAL_DATA_IN_CLEAR includes NAL length and nal_unit_type fields so we need to subtract it. */
                ChkDR( DRM_DWordToByte( (DRM_DWORD)( (DRM_DWORD)H264_NAL_DATA_IN_CLEAR + 1 - (DRM_DWORD)( H264_3BYTE_STARTCODE_PLUS_TYPE + cbLast ) ), &f_poSubsampleContext->cbCountdownToProtected ) );
            }
            
            f_poSubsampleContext->bLast = pbBuffer[ wSyncOffset + 3 ];
            ChkDR( DRM_WordAdd( wSyncOffset, H264_3BYTE_STARTCODE_PLUS_TYPE, &wSyncOffset ) );
            ChkDR( DRM_WordSub( cbRemaining, H264_3BYTE_STARTCODE_PLUS_TYPE, &cbRemaining ) );
        }
        else
        {
            /* Not a start code */
            if( f_poSubsampleContext->cbCountdownToProtected > 0 )
            {
                DRMASSERT( !f_poSubsampleContext->fProcessProtected );

                f_poSubsampleContext->cbCountdownToProtected--;
                if( f_poSubsampleContext->cbCountdownToProtected == 0 )
                {
                    /* Switch to protected because we have left the number of bytes defined by H264_NAL_DATA_IN_CLEAR in clear  */
                    f_poSubsampleContext->fProcessProtected = TRUE;
                }
            }

            DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, 1 );

            f_poSubsampleContext->bLast = pbBuffer[ wSyncOffset ];
            ChkDR( DRM_WordIncOne( wSyncOffset ) );
            ChkDR( DRM_WordDecOne( cbRemaining ) );
        }
    }

    f_poSubsampleContext->wOffset = wSyncOffset;

    /* 
    ** This is the last call, put all the remaining bytes to the last subsample.
    ** If it is in protected mode then all the remaining bytes will be counted as protected bytes,
    ** otherwise as clear bytes. 
    */
    if( f_poSubsampleContext->fLastCall )
    {
        DRM_DWORD iSample = 0;
        DRM_DWORD iSubsample = 0;

        VERIFY_BYTE_COUNT_OF_REMAINING_BYTES_IN_SUBSAMPLE_CONTEXT_BUFFER( f_poSubsampleContext, cbRemaining );
        DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbRemaining );

        /* 
        ** Adjust the protected subsamples to ensure the total length of the encrypted subsamples is aligned,
        ** a multiple of 16 bytes, so that decryption of a NAL start at the beginning of a cryptostream block.
        */
        for( iSample = f_pPES->dwIndexFirstSample; iSample <= f_pPES->dwIndexLastSample; iSample++ )
        {
            pSample = &f_pEncContext->pSamples[ iSample ];
            if( pSample->fHasSubsample )
            {
                DRM_DWORD cbEncrypted = 0;
                DRM_BYTE cbRemaining2 = 0;

                /* Count the encrypted bytes */
                for( iSubsample = pSample->dwIndexFirstSubsample; iSubsample <= pSample->dwIndexLastSubsample; iSubsample++ )
                {
                    pSubsample = &f_pEncContext->pSubsamples[ iSubsample ];
                    ChkDR( DRM_DWordAdd( cbEncrypted, pSubsample->cbEncryptedBytes, &cbEncrypted ) );
                    ChkOverflow( cbEncrypted, pSubsample->cbEncryptedBytes );
                }

                /* Adjsut the encrypted bytes to align it */
                cbRemaining2 = cbEncrypted % AESCTR_BLOCK_SIZE;
                for( iSubsample = pSample->dwIndexFirstSubsample; iSubsample <= pSample->dwIndexLastSubsample && cbRemaining2 > 0; iSubsample++ )
                {
                    pSubsample = &f_pEncContext->pSubsamples[ iSubsample ];
                    if( pSubsample->fAdjustable && pSubsample->cbEncryptedBytes > 0 )
                    {
                        cbRemaining2 = (DRM_BYTE)( AESCTR_BLOCK_SIZE - cbRemaining2 );
                        ChkBOOL( pSubsample->cbClearBytes > cbRemaining2, DRM_E_M2TS_INTERNAL_ERROR );
                        ChkDR( DRM_WordSub( pSubsample->cbClearBytes, cbRemaining2, &pSubsample->cbClearBytes ) );
                        ChkDR( DRM_DWordAdd( pSubsample->cbEncryptedBytes, cbRemaining2, &pSubsample->cbEncryptedBytes ) );
                        cbRemaining2 = 0;
                    }
                }

                /* 
                ** We should have at least one subsample with the fAdjustable == TRUE
                ** or there is no encrypted subsamples. So, cbRemaining2 must be zero
                ** at here.
                */
                ChkBOOL( cbRemaining2 == 0, DRM_E_M2TS_INTERNAL_ERROR );
            }
        }
    }

ErrorExit:
    _UpdatePESResult( f_pPES, &dr );
    return dr;
}

/**********************************************************************
**
** Function:    _ParseDDPlusFrameHeader
**
** Synopsis:    Parse DDPlus frame header to get the frame size and 
**              frame duration.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_wSyncOffset]          -- Offset to the frame header.
** [f_pqwFrameDuration]     -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_DDPLUS_FORMAT_INVALID if the frame
**                              header is not valid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParseDDPlusFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParseDDPlusFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration )
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_BYTE                    bStreamType         = 0;
    DRM_BYTE                    bSubStreamId        = 0;
    DRM_WORD                    cbFrame             = 0;
    DRM_BYTE                    bFSCode             = 0;
    DRM_BYTE                    bFSCode2            = 0;
    DRM_BYTE                    bBitstreamId        = 0;
    DRM_BOOL                    fIsDDPlus           = FALSE;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };

    DRMASSERT( f_poSubsampleContext != NULL );
    DRMASSERT( f_pqwFrameDuration != NULL );
    __analysis_assume( f_poSubsampleContext != NULL );
    __analysis_assume( f_pqwFrameDuration != NULL );

    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext,
                              f_poSubsampleContext->cbBuffer,
                              f_poSubsampleContext->rgbBuffer,
                              f_wSyncOffset ) );

    /* Skip over the syncword(16) */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 2 ) );

    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2,  &bStreamType ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 3,  &bSubStreamId ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 11, &cbFrame ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2,  &bFSCode ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2,  &bFSCode2 ) );

    /* skip audioCodingMode(3) and lowFrequencyEffectFlag(1) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 5, &bBitstreamId ) );

    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );

    fIsDDPlus = ( bBitstreamId <= 16 ) && ( bBitstreamId > 10 );
    if( !fIsDDPlus )
    {
        /* Parse AC3 syncinfo() */

        DRM_BYTE bFSCod = (DRM_BYTE)( ( f_poSubsampleContext->rgbBuffer[ f_wSyncOffset + 4 ] >> 6 ) & 0x3 ); /* 2-bit fscod */
        DRM_BYTE bFrmSizeCod = (DRM_BYTE)( f_poSubsampleContext->rgbBuffer[ f_wSyncOffset + 4 ] & 0x3f ); /* 6-bit frmsizecod */

        /*
        ** If the following check passes, we let AC3 frame
        ** pass through to the device. The device should handle bad AC3 frames.
        */
        ChkBOOL( bFSCod < GBL_MAXFSCOD && bFrmSizeCod < GBL_MAXDDDATARATE, DRM_E_M2TS_DDPLUS_FORMAT_INVALID );
            
        f_poSubsampleContext->cbFrameSize = (DRM_DWORD)( 2 * s_oGBLFrameSizeTable[ bFSCod ][ bFrmSizeCod ] );
        *f_pqwFrameDuration = DRM_UI64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, s_oDDPlusFrameDuration[ bFSCod ][ 3 ] ) );
        f_poSubsampleContext->cbFrameHeader = DDPLUS_HEADER_SIZE;
    }
    else
    {
        /* Parse DDPlus */

        if( bFSCode != 0x3 )
        {
            ChkDR( DRM_UInt64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, s_oDDPlusFrameDuration[ bFSCode ][ bFSCode2 ] ), f_pqwFrameDuration ) );
        }
        else
        {
            /* Bad AC3 Audio header fsCode == fsCode2 == 0x3 */
            ChkBOOL( bFSCode2 < 0x3, DRM_E_M2TS_DDPLUS_FORMAT_INVALID );

            ChkDR( DRM_UInt64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, s_oDDPlusLowSampleRateFrameDuration[ bFSCode2 ] ), f_pqwFrameDuration ) );
        }

        /* Not supported case in DDPlus parser */
        ChkBOOL( bStreamType == 0 && bSubStreamId == 0, DRM_E_M2TS_DDPLUS_FORMAT_INVALID );

        f_poSubsampleContext->cbFrameSize   = (DRM_DWORD)( 2 * ( cbFrame + 1 ) );
        f_poSubsampleContext->cbFrameHeader = DDPLUS_HEADER_SIZE;
    }

    ChkBOOL( f_poSubsampleContext->cbFrameHeader < f_poSubsampleContext->cbFrameSize, DRM_E_M2TS_DDPLUS_FORMAT_INVALID );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_SyncAndParseDDPlusFrameHeaderCallback
**
** Synopsis:    Finds the DDPlus syncword and calls _ParseDDPlusFrameHeader
**              to parse the DDPlus frame header.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pwSyncOffset]         -- Out parameter to contain an offset to the frame header.
** [f_pfSynced]             -- Out parameter to contain a flag indicating
**                             whether we have found the frame header .
** [f_pqwDuration]          -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_SyncAndParseDDPlusFrameHeaderCallback(
    __inout     DRM_M2TS_SUBSAMPLE_CONTEXT              *f_poSubsampleContext,
    __inout     DRM_WORD                                *f_pwSyncOffset,
    __out       DRM_BOOL                                *f_pfSynced,
    __out       DRM_UINT64                              *f_pqwDuration )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_WORD    cbRemaining = 0;

    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pwSyncOffset != NULL );
    ChkArg( f_pfSynced != NULL );
    ChkArg( f_pqwDuration != NULL );

    DRMASSERT( *f_pwSyncOffset == f_poSubsampleContext->wOffset );

    ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );

    *f_pfSynced = FALSE;

    /* search for valid syncword 0x0B77 */
    for( ; cbRemaining >= DDPLUS_HEADER_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_DDPLUS_SYNC_WORD( f_poSubsampleContext->rgbBuffer, f_pwSyncOffset ) )
        {
            *f_pfSynced = TRUE;
            ChkDR( _ParseDDPlusFrameHeader( f_poSubsampleContext, 
                                            *f_pwSyncOffset,
                                            f_pqwDuration ) );
            break;
        }
        ChkDR( DRM_WordIncOne( *f_pwSyncOffset ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _ParseADTSFrameHeader
**
** Synopsis:    Parses ADTS frame header to get the frame size and 
**              frame duration.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_wSyncOffset]          -- Offset to the frame header.
** [f_pqwFrameDuration]     -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_ADTS_FORMAT_INVALID if the frame
**                              header is not valid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParseADTSFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParseADTSFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration )
{
    DRM_RESULT                          dr                  = DRM_SUCCESS;
    DRM_M2TS_BIT_STREAM_CONTEXT         oBSRContext         = { 0 };
    DRM_BYTE                            fProtectionAbsent   = FALSE;
    DRM_DWORD                           dwSamplingRateCode  = 0;
    DRM_DWORD                           dwSamplingRate      = 0;

    DRMASSERT( f_poSubsampleContext != NULL );
    DRMASSERT( f_pqwFrameDuration != NULL );
    __analysis_assume( f_poSubsampleContext != NULL );
    __analysis_assume( f_pqwFrameDuration != NULL );

    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext,
                              f_poSubsampleContext->cbBuffer,
                              f_poSubsampleContext->rgbBuffer,
                              f_wSyncOffset ) );

    /* Skip syncword 0xFFF */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 12 ) );

    /* skip mpeg_version and mpeg_layer */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );

    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &fProtectionAbsent ) );

    /* skip profile_code(2) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 2 ) );

    /*
    ** Each ADTS frame contains 1024 raw PCM samples in encoded format. 
    ** Therefore, the duration of each frame in seconds is given by 
    ** 1024/(sampling frequency). The time stamps passed to MediaElement 
    ** are in Hns (100 nanosecond) increments. Therefore, frame duration 
    ** is given by  10,000,000 * 1024 / SamplingFrequency
    */
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 4, &dwSamplingRateCode ) );
    ChkBOOL( dwSamplingRateCode < NO_OF( s_oADTS_SamplingRatesFromRateCode ), DRM_E_M2TS_ADTS_FORMAT_INVALID );
    dwSamplingRate = s_oADTS_SamplingRatesFromRateCode[ dwSamplingRateCode ];
    ChkDR( DRM_UInt64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, (DRM_DWORD)( 10000000.00 * 1024.00 / ( double )dwSamplingRate ) ), f_pqwFrameDuration ) );

    /* skip private_stream(1), channel_config(3) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

    /* skip 4 1-bit indicators */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 13, &f_poSubsampleContext->cbFrameSize ) );

    f_poSubsampleContext->cbFrameHeader = fProtectionAbsent != 0 ? (DRM_WORD)7 : (DRM_WORD)9;
    DRMASSERT( f_poSubsampleContext->cbFrameSize > f_poSubsampleContext->cbFrameHeader );
    ChkBOOL( f_poSubsampleContext->cbFrameSize > f_poSubsampleContext->cbFrameHeader, DRM_E_M2TS_ADTS_FORMAT_INVALID );

ErrorExit:
    return dr;
}
 
/**********************************************************************
**
** Function:    DRM_M2TS_SyncAndParseADTSFrameHeaderCallback
**
** Synopsis:    Find ADTS syncword and call _ParseADTSFrameHeader to
**              parse the ADTS frame header.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pwSyncOffset]         -- Out parameter to contain an offset to the frame header.
** [f_pfSynced]             -- Out parameter to contain a flag indicating
**                             whether we have found the frame header .
** [f_pqwDuration]          -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_SyncAndParseADTSFrameHeaderCallback(
    __inout     DRM_M2TS_SUBSAMPLE_CONTEXT              *f_poSubsampleContext,
    __inout     DRM_WORD                                *f_pwSyncOffset,
    __out       DRM_BOOL                                *f_pfSynced,
    __out       DRM_UINT64                              *f_pqwDuration )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_WORD    cbRemaining = 0;

    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pwSyncOffset != NULL );
    ChkArg( f_pfSynced != NULL );
    ChkArg( f_pqwDuration != NULL );

    DRMASSERT( *f_pwSyncOffset == f_poSubsampleContext->wOffset );

    ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );

    *f_pfSynced = FALSE;

    /* search for valid sync bits(FFF), ignore FFFF which is invalid ATDS header and could be stuffing bits. */
    for( ; cbRemaining >= ADTS_MAX_HEADER_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_ADTS_SYNC_WORD( f_poSubsampleContext->rgbBuffer, f_pwSyncOffset ) )
        {
            *f_pfSynced = TRUE;
            ChkDR( _ParseADTSFrameHeader( f_poSubsampleContext, *f_pwSyncOffset, f_pqwDuration ) );
            break;
        }
        ChkDR( DRM_WordIncOne( *f_pwSyncOffset ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _ParseMPEGAASudioFrameHeader
**
** Synopsis:    Parses MPEGA audio frame header to get the frame size 
**              and frame duration.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_wSyncOffset]          -- Offset to the frame header.
** [f_pqwFrameDuration]     -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_MPEGA_FORMAT_INVALID if the frame
**                              header is not valid.
**                          DRM_E_ARITHMETIC_OVERFLOW
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParseMPEGAASudioFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParseMPEGAASudioFrameHeader( 
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT          *f_poSubsampleContext, 
    __in      const DRM_WORD                             f_wSyncOffset,
    __out           DRM_UINT64                          *f_pqwFrameDuration )
{
    DRM_RESULT                          dr                  = DRM_SUCCESS;
    DRM_M2TS_BIT_STREAM_CONTEXT         oBSRContext         = { 0 };
    DRM_BYTE                            bID                 = 0;
    DRM_BYTE                            bLayer              = 0;
    DRM_BYTE                            bProtectionBit      = 0;
    DRM_BYTE                            bBitrateIndex       = 0;
    DRM_BYTE                            bSamplingRateIndex  = 0;
    DRM_BYTE                            bPaddingBit         = 0;
    DRM_BYTE                            bMPEG25             = 0;
    DRM_DWORD                           dwSamplingRate      = 0;
    DRM_UINT64                          qwSamplingRate      = DRM_UI64HL( 0, 0 );
    DRM_UINT64                          qwAdjustedBitRate   = DRM_UI64HL( 0, 0 );
    DRM_UINT64                          qwFrameSize         = DRM_UI64HL( 0, 0 );


    DRMASSERT( f_poSubsampleContext != NULL );
    DRMASSERT( f_pqwFrameDuration != NULL );
    __analysis_assume( f_poSubsampleContext != NULL );
    __analysis_assume( f_pqwFrameDuration != NULL );

    ChkArg( f_poSubsampleContext->cbBuffer >= MPEGA_AUDIO_HEADER_ONLY_SIZE );

    *f_pqwFrameDuration = DRM_UI64HL( 0, 0 );

    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext,
                              f_poSubsampleContext->cbBuffer,
                              f_poSubsampleContext->rgbBuffer,
                              f_wSyncOffset ) );

    /* Skip syncword 0xFFE */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 11 ) );

    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bMPEG25 ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bID ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2, &bLayer ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bProtectionBit ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 4, &bBitrateIndex ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2, &bSamplingRateIndex ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bPaddingBit ) );

    /* skip private_but(1), mode(2), mode_extension(2), copyright(1), original/home(1), emphasis(2) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 9 ) );

    /* 
    ** Valid combinations of bMPEG25 and bID
    ** bMPEG25 = 0 and bID = 0 --> MPEG Audio version 2.5 (not an official version)
    ** bMPEG25 = 0 and bID = 1 --> Reserved
    ** bMPEG25 = 1 and bID = 0 --> MPEG Audio version 2 (ISO/IEC 13818-3)
    ** bMPEG25 = 1 and bID = 1 --> MPEG Audio version 1 (ISO/IEC 11172-3)
    */

    if( bLayer == 0                            /* 0 is reserved layer */
     || bBitrateIndex == 0                     /* The valid range is from 1 to 14 */
     || bBitrateIndex >= 0xf        
     || bSamplingRateIndex == 3                /* 3 is reserved sampling index */
     || ( ( ( bMPEG25 << 1 ) | bID ) == 1 ) )  /* MPEG25 = 0 and ID == 1 is reserved */
    {
        ChkDR( DRM_E_M2TS_MPEGA_FORMAT_INVALID );
    }

    /* Convert Layer I from value of 3 to 1, Layer II from value 2 to 2, and Layer III from value 1 to 3 */
    bLayer = (DRM_BYTE)( 4 - bLayer );

    /* Flip the value of bMPEG25 */
    bMPEG25 = bMPEG25 == 0 ? (DRM_BYTE)1 : (DRM_BYTE)0;

    dwSamplingRate      = s_oMPEGASamplingRate[ ( bMPEG25 << 1 ) | bID ][ bSamplingRateIndex ];
    qwSamplingRate      = DRM_UI64HL( 0, dwSamplingRate );
    qwAdjustedBitRate   = DRM_UI64HL( 0, s_oMPEGABitRate[ bID ][ bLayer - 1 ][ bBitrateIndex ] );

    if( bLayer > 1 )
    {
        /* For Layer II and III */
        ChkDR( DRM_UInt64Mult( qwAdjustedBitRate, bID == 0 ? DRM_UI64HL( 0, 72 ) : DRM_UI64HL( 0, 144 ), &qwAdjustedBitRate ) );
        qwAdjustedBitRate = DRM_UI64Div( qwAdjustedBitRate, qwSamplingRate );
        ChkDR( DRM_UInt64Add( qwAdjustedBitRate, DRM_UI64HL( 0, bPaddingBit ), &qwFrameSize ) );

        /*
        ** Each MEPGA Layer II and III frame contains 1152 samples in encoded format. 
        ** Therefore, the duration of each frame in seconds is given by 
        ** 1152/(sampling frequency). The time stamps passed to MediaElement 
        ** are in Hns (100 nanosecond) increments. Therefore, frame duration 
        ** is given by  10,000,000 * 1152 / SamplingFrequency
        */
        ChkDR( DRM_UInt64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, (DRM_DWORD)( 10000000.00 * 1152.00 / ( double )dwSamplingRate ) ), f_pqwFrameDuration ) );
    }
    else
    {
        /* For layer I */
        ChkDR( DRM_UInt64Mult( qwAdjustedBitRate, DRM_UI64HL( 0, 12 ), &qwAdjustedBitRate ) );
        qwAdjustedBitRate = DRM_UI64Div( qwAdjustedBitRate, qwSamplingRate );
        ChkDR( DRM_UInt64Add( qwAdjustedBitRate, DRM_UI64HL( 0, bPaddingBit ), &qwAdjustedBitRate ) );
        qwFrameSize = DRM_UI64ShL( qwAdjustedBitRate, 2 );

        /* Each MEPGA Layer I frame contains 384 samples in encoded format. */
        ChkDR( DRM_UInt64Add( *f_pqwFrameDuration, DRM_UI64HL( 0, (DRM_DWORD)( 10000000.00 * 384.00 / ( double )dwSamplingRate ) ), f_pqwFrameDuration ) );
    }

    ChkBOOL( DRM_UI64High32( qwFrameSize ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
    f_poSubsampleContext->cbFrameSize = DRM_UI64Low32( qwFrameSize );

    f_poSubsampleContext->cbFrameHeader = bProtectionBit == 0 ? (DRM_WORD)MPEGA_AUDIO_MAX_HEADER_SIZE : (DRM_WORD)MPEGA_AUDIO_HEADER_ONLY_SIZE;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_SyncAndParseMPEGAAudioFrameHeaderCallback
**
** Synopsis:    Finds the syncword for MPEGA audio and calls 
**              _ParseMPEGAASudioFrameHeader to parse the frame header.
**
** Arguments:
**
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pwSyncOffset]         -- Out parameter to contain an offset to the frame header.
** [f_pfSynced]             -- Out parameter to contain a flag indicating
**                             whether we have found the frame header .
** [f_pqwDuration]          -- Out parameter to contain the frame duration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_SyncAndParseMPEGAAudioFrameHeaderCallback(
    __inout     DRM_M2TS_SUBSAMPLE_CONTEXT              *f_poSubsampleContext,
    __inout     DRM_WORD                                *f_pwSyncOffset,
    __out       DRM_BOOL                                *f_pfSynced,
    __out       DRM_UINT64                              *f_pqwDuration )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_WORD    cbRemaining = 0;

    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pwSyncOffset != NULL );
    ChkArg( f_pfSynced != NULL );
    ChkArg( f_pqwDuration != NULL );

    DRMASSERT( *f_pwSyncOffset == f_poSubsampleContext->wOffset );

    ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );

    *f_pfSynced = FALSE;

    /* search for valid syncword (0xFFE) with a valid layer number */
    for( ; cbRemaining >= MPEGA_AUDIO_MAX_HEADER_SIZE; cbRemaining-- )
    {
        if( DRM_M2TS_IS_MPEGA_SYNC_WORD( f_poSubsampleContext->rgbBuffer, f_pwSyncOffset ) )
        {
            *f_pfSynced = TRUE;
            ChkDR( _ParseMPEGAASudioFrameHeader( f_poSubsampleContext, *f_pwSyncOffset, f_pqwDuration ) );
            break;
        }
        ChkDR( DRM_WordIncOne( *f_pwSyncOffset ) );
    }

ErrorExit:
    return dr;
}
                    
/**********************************************************************
**
** Function:    DRM_M2TS_CreateAudioSubsampleMappings
**
** Synopsis:    Create audio subsample mappings for the MPEGA, ADTS
**              DDPlus media formats.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pfnSyncAndParseFrameHeader]   
                            -- Pointer to a callback function to find the frame header
                               and parse the frame header.
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_pPES]                 -- Pointer to a PES that contains the audio samples
** [f_pStream]              -- Pointer to a stream that contains the audio PES packets
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CreateAudioSubsampleMappings(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __in      const DRM_pfnSyncAndParseFrameHeaderCallback   f_pfnSyncAndParseFrameHeader,
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT              *f_poSubsampleContext,
    __inout         DRM_M2TS_PES                            *f_pPES,
    __inout         DRM_M2TS_STREAM                         *f_pStream )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_M2TS_SAMPLE     *pSample        = NULL;
    DRM_M2TS_SUBSAMPLE  *pSubsample     = NULL;
    DRM_WORD             wSyncOffset    = 0;
    DRM_WORD             cbRemaining    = 0;
    DRM_BOOL             fSynced        = FALSE;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pfnSyncAndParseFrameHeader != NULL );
    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_pPES != NULL );
    ChkArg( f_pStream != NULL );

    if( f_pPES->fHasSample )
    {
        /* Already have the sample and subsample assigned, continue to use the last sample and subsample */
        pSample    = &f_pEncContext->pSamples[ f_pPES->dwIndexLastSample ];
        DRMASSERT( pSample->fHasSubsample );
        pSubsample = &f_pEncContext->pSubsamples[ pSample->dwIndexLastSubsample ];
    }

    while( f_poSubsampleContext->wOffset < f_poSubsampleContext->cbBuffer )
    {
        DRM_UINT64 qwDuration = DRM_UI64HL( 0, 0 );

        /* Find the clear subsample */
        if( !f_poSubsampleContext->fProcessProtected )
        {
            DRM_WORD cbClear = 0;

            /* Create a new sample and subsample if this is the first time that this function is called */
            if( !f_pPES->fHasSample )
            {
                ChkDR( _AddNewSample( f_pEncContext, f_pPES, &pSample ) );
                ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
            }
            else if( pSubsample != NULL && pSubsample->cbEncryptedBytes > 0 )
            {
                /* Create a new subsample when we switch from protected to clear */
                ChkDR( _AddNewSubsample( f_pEncContext, pSample, &pSubsample ) );
            }

            AssertChkArg( pSample != NULL );
            AssertChkArg( pSubsample != NULL );

            /* 
            ** f_pfnSyncAndParseFrameHeader must not change f_poSubsampleContext->wOffset
            ** druing Synchronizing and parsing because we need to know the bytes it had 
            ** processed after it returned.
            */
            wSyncOffset = f_poSubsampleContext->wOffset;
            ChkDR( f_pfnSyncAndParseFrameHeader( f_poSubsampleContext,
                                                 &wSyncOffset,
                                                 &fSynced,
                                                 &qwDuration ) );

            /* 
            ** Normally cbClearBytes should equal to the frame header size, but it is not always true.
            ** The data after the frame data (based on the frame size) does not always start with
            ** the syncword.
            */
            ChkDR( DRM_WordSub( wSyncOffset, f_poSubsampleContext->wOffset, &cbClear ) );
            DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbClear );
            f_poSubsampleContext->wOffset = wSyncOffset;

            if( !fSynced )
            {
                /* 
                ** Haven't found the syncword yet.  It must be that we ran out of data in the buffer 
                ** so return to the caller for more data.
                */
                break;
            }

            if( DRM_UI64Les( DRM_UI64HL( 0, 0 ), qwDuration ) )
            {
                /* Aggregate the frame duration */
                ChkDR( DRM_UInt64Add( f_pStream->qwAggregatedDuration, qwDuration, &f_pStream->qwAggregatedDuration ) );
            }

            /* add the size of the frame header to the clear subsample */
            DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, f_poSubsampleContext->cbFrameHeader );
            ChkDR( DRM_WordAdd( f_poSubsampleContext->wOffset, f_poSubsampleContext->cbFrameHeader, &f_poSubsampleContext->wOffset ) );

            f_poSubsampleContext->fProcessProtected = TRUE;
        }

        /* Get the protected subsample base on the frame size spcified in the frame header */
        if( f_poSubsampleContext->fProcessProtected )
        {
            DRM_WORD cbFrameToFill = 0;

            /* 
            ** pSample should not be NULL unless caller sets f_poSubsampleContext->fProcessProtected to TRUE
            ** on its first call to this function 
            */
            AssertChkArg( pSample != NULL );
            AssertChkArg( pSubsample != NULL );
            DRMASSERT( f_poSubsampleContext->cbFrameSize > 0 );

            /* Calcuate the remaining bytes needed to complete the frame data */
            ChkOverflow( f_poSubsampleContext->cbFrameSize, f_poSubsampleContext->cbFrameHeader + pSubsample->cbEncryptedBytes );
            ChkDR( DRM_DWordToWord( f_poSubsampleContext->cbFrameSize - f_poSubsampleContext->cbFrameHeader - pSubsample->cbEncryptedBytes, &cbFrameToFill ) );

            ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );
            if( cbFrameToFill <= cbRemaining )
            {
                /* Have all the bytes needed in the buffer to complete the frame data */
                DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbFrameToFill );
                ChkDR( DRM_WordAdd( f_poSubsampleContext->wOffset, cbFrameToFill, &f_poSubsampleContext->wOffset ) );

                /* Set the protected flag to false to switch to clear subsample */
                f_poSubsampleContext->fProcessProtected = FALSE;
            }
            else
            {
                /* Only have some of the bytes needed in the buffer to fill the frame */
                DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbRemaining );
                f_poSubsampleContext->wOffset = f_poSubsampleContext->cbBuffer;
                break;
            }
        }
    }


    /* This is the last call, put all the remaining bytes to the last subsample */
    if( f_poSubsampleContext->fLastCall )
    {
        AssertChkArg( pSample != NULL );
        AssertChkArg( pSubsample != NULL );

        ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &cbRemaining ) );
        DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( f_poSubsampleContext, pSubsample, cbRemaining );

        f_poSubsampleContext->wOffset = f_poSubsampleContext->cbBuffer;
    }

ErrorExit:
    _UpdatePESResult( f_pPES, &dr );
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_FillSubsampleContextBuffer 
**
** Synopsis:   Fill the buffer in the subsample context 
**
** Arguments:
**
** [f_fCheckPESCompleteness]-- Flag indicates whether this is for checking PES
**                             completeness.
** [f_poSubsampleContext]   -- Pointer to a subsample context.
** [f_ppPacket]             -- A linked list of packets that we need to copy their 
**                             payload to the subsample context's buffer for process.
** [f_ppOverflowPacket]     -- A linked list of packets that contain the overflow
**                             payload that we need to copy to the subsample
**                             context's buffer for process.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_FillSubsampleContextBuffer(
    __in      const DRM_BOOL                     f_fCheckPESCompleteness,
    __inout         DRM_M2TS_SUBSAMPLE_CONTEXT  *f_poSubsampleContext,
    __inout         DRM_M2TS_PACKET            **f_ppPacket,
    __inout_opt     DRM_M2TS_PACKET            **f_ppOverflowPacket )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_M2TS_PACKET *pPacket            = NULL;
    DRM_M2TS_PACKET *pOverflowPacket    = NULL;
    DRM_WORD         cbPayload          = 0;
    DRM_WORD         cbBufferNew        = 0;

    ChkArg( f_poSubsampleContext != NULL );
    ChkArg( f_ppPacket != NULL );

    pPacket = *f_ppPacket;
    pOverflowPacket = f_ppOverflowPacket != NULL ? *f_ppOverflowPacket : NULL;

    /* shift the bytes in the buffer to the beginning */
    if( f_poSubsampleContext->wOffset > 0 )
    {
        ChkDR( DRM_DWordAdd( f_poSubsampleContext->dwLastOffset, f_poSubsampleContext->wOffset, &f_poSubsampleContext->dwLastOffset ) );
        ChkDR( DRM_WordSub( f_poSubsampleContext->cbBuffer, f_poSubsampleContext->wOffset, &f_poSubsampleContext->cbBuffer ) );
        MEMMOVE( f_poSubsampleContext->rgbBuffer, 
                 &f_poSubsampleContext->rgbBuffer[ f_poSubsampleContext->wOffset ], 
                 f_poSubsampleContext->cbBuffer );
        f_poSubsampleContext->wOffset = 0;
    }

    /* Copy as much payload as possible from each packet on the list to the buffer */
    while( pPacket != NULL )
    {
        ChkDR( DRM_WordSub( pPacket->cbPayload, pPacket->cbHeader, &cbPayload ) );
        ChkDR( DRM_WordSub( cbPayload, pPacket->cbPayloadLastPES, &cbPayload ) );
        ChkDR( DRM_WordAdd( f_poSubsampleContext->cbBuffer, cbPayload, &cbBufferNew ) );
        if( cbBufferNew > SIZEOF( f_poSubsampleContext->rgbBuffer ) )
        {
            /* break becasue there isn't room to hold another payload */
            break;
        }

        /* Copy the payload from packet to the buffer excluding the PES header and last PES' payload */
        if( cbPayload > 0 )
        {
            ChkOverflow( DRM_M2TS_PACKET_SIZE, cbPayload );
            MEMCPY( f_poSubsampleContext->rgbBuffer + f_poSubsampleContext->cbBuffer, 
                    pPacket->rgbData + DRM_M2TS_PACKET_SIZE - cbPayload, 
                    cbPayload );
            ChkDR( DRM_WordAdd( f_poSubsampleContext->cbBuffer,          cbPayload, &f_poSubsampleContext->cbBuffer ) );
            ChkDR( DRM_DWordAdd( f_poSubsampleContext->cbCurrentPESOnly, cbPayload, &f_poSubsampleContext->cbCurrentPESOnly ) );
        }

        pPacket = pPacket->pNextPerUnit;
    }

    /* 
    ** Copy as much payload as possible from each packet on the overflow list to the buffer 
    ** for creating the subsample mappings but we have to exhaust the packet list first
    */
    if( pPacket == NULL )
    {
        while( pOverflowPacket != NULL )
        {
            if( f_fCheckPESCompleteness )
            {
                DRMASSERT( pOverflowPacket->cbPayloadLastPES == 0 );
            }

            cbPayload = f_fCheckPESCompleteness ?
                        (DRM_WORD)( pOverflowPacket->cbPayload - pOverflowPacket->cbHeader ) :
                        (DRM_WORD)( pOverflowPacket->cbPayloadLastPES );

            ChkDR( DRM_WordAdd( f_poSubsampleContext->cbBuffer, cbPayload, &cbBufferNew ) );
            if( cbBufferNew > SIZEOF( f_poSubsampleContext->rgbBuffer ) )
            {
                /* break becasue there isn't room to hold another payload */
                break;
            }

            /* Copy only the payload belong to this PES from packet to the buffer */
            if( cbPayload > 0 )
            {
                DRM_BYTE bOffset = 0;
                ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, pOverflowPacket->cbPayload, &bOffset ) );
                ChkDR( DRM_ByteAdd( bOffset, pOverflowPacket->cbHeader, &bOffset ) );

                MEMCPY( &f_poSubsampleContext->rgbBuffer[ f_poSubsampleContext->cbBuffer ], 
                        &pOverflowPacket->rgbData[ bOffset ], 
                        cbPayload );
                ChkDR( DRM_WordAdd( f_poSubsampleContext->cbBuffer, cbPayload, &f_poSubsampleContext->cbBuffer ) );
            }

            if( !DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pOverflowPacket ) )
            {
                pOverflowPacket = pOverflowPacket->pNextPerUnit;
            }
            else
            {
                /* Just processed the last packet that has the payload which belongs to this PES */
                pOverflowPacket = NULL;
            }
        }
    }

ErrorExit:
    if( DRM_SUCCEEDED( dr ) )
    {
        *f_ppPacket = pPacket;
        if( f_ppOverflowPacket != NULL )
        {
            *f_ppOverflowPacket = pOverflowPacket;
        }
    }
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
