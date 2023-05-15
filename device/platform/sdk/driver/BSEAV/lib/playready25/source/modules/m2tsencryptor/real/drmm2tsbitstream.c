/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSBITSTREAM_C
#include <drmm2tsbitstream.h>
#include <drmmathsafe.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* 
** For valiating the crc32 field in the PAT and PMT packet 
** (copied from xmf's HLS demux xmf\mediasources\mbr\HLS\tsdemux\Section.cpp)
*/
static DRM_DWORD s_crcTable[] =
{
    0x00000000,
    0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
    0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
    0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
    0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
    0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
    0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
    0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
    0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
    0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
    0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
    0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
    0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
    0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
    0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
    0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
    0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
    0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
    0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
    0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
    0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
    0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
    0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
    0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
    0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
    0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
    0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
    0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
    0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
    0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
    0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
    0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
    0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
    0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
    0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
    0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
    0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
    0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
    0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

#define DRM_M2TS_BSR_SHIFTED_BYTE( poBSRContext, nextByte )                         \
    do {                                                                            \
        if( (poBSRContext)->dwByteOffset >= (poBSRContext)->cbData )                \
        {                                                                           \
            if( !_RotateToNextPacket( (poBSRContext) ) )                            \
            {                                                                       \
                (poBSRContext)->fOutOfRange = TRUE;                                 \
                ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );                              \
            }                                                                       \
        }                                                                           \
        if( (poBSRContext)->fUseCounter )                                           \
        {                                                                           \
            if( (poBSRContext)->cCounter == 0 )                                     \
            {                                                                       \
                (poBSRContext)->fOutOfRange = TRUE;                                 \
                ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );                              \
            }                                                                       \
        }                                                                           \
        (nextByte) = (poBSRContext)->pbData[ (poBSRContext)->dwByteOffset ];        \
    } while( FALSE )

/*
** NOTE: The caller MUST NOT reference the next byte pointed by pbOneByteOnly
**       because the next byte may be in next packet
*/
#define DRM_M2TS_BSR_GET_CURRENT_BYTE_POINTER( poBSRContext, pbOneByteOnly )        \
    do {                                                                            \
        if( (poBSRContext)->dwByteOffset >= (poBSRContext)->cbData )                \
        {                                                                           \
            if( !_RotateToNextPacket( (poBSRContext) ) )                            \
            {                                                                       \
                (poBSRContext)->fOutOfRange = TRUE;                                 \
                ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );                              \
            }                                                                       \
        }                                                                           \
        (pbOneByteOnly) = &(poBSRContext)->pbData[ (poBSRContext)->dwByteOffset ];  \
    } while( FALSE )

#define DRM_M2TS_BSR_INC_BYTE_OFFSET( poBSRContext )                                \
    do {                                                                            \
        ChkDR( DRM_DWordIncOne( (poBSRContext)->dwByteOffset ) );                   \
        if( (poBSRContext)->fUseCounter )                                           \
        {                                                                           \
            ChkBOOL( (poBSRContext)->cCounter > 0, DRM_E_BUFFER_BOUNDS_EXCEEDED );  \
            (poBSRContext)->cCounter--;                                             \
        }                                                                           \
    } while( FALSE )

/**********************************************************************
**
** Function:    _RotateToNextPacket
**
** Synopsis:    Rotates to the next packet if there is one and updates
**              its states.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context
**
** Returns:                 TRUE if there is next packet to rotate to
**                          FALSE if end of stream 
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _RotateToNextPacket(
    __inout    DRM_M2TS_BIT_STREAM_CONTEXT  *f_poBSRContext ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _RotateToNextPacket(
    __inout    DRM_M2TS_BIT_STREAM_CONTEXT  *f_poBSRContext )
{
    DRMASSERT( f_poBSRContext != NULL );
    __analysis_assume( f_poBSRContext != NULL );

    if( ( f_poBSRContext->pPacket != NULL )
     && ( DRM_M2TS_PACKET_SIZE    >= f_poBSRContext->pPacket->cbPayload ) )
    {
        DRM_M2TS_PACKET *pPacket = f_poBSRContext->pPacket;

        f_poBSRContext->cbData        = SIZEOF( pPacket->rgbData );
        f_poBSRContext->pbData        = pPacket->rgbData;
        f_poBSRContext->dwByteOffset  = (DRM_DWORD)( DRM_M2TS_PACKET_SIZE - pPacket->cbPayload );   /* Start from the first byte of the payload */
        f_poBSRContext->pPacket       = pPacket->pNextPerUnit;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_Init
**
** Synopsis:    Initialize bit stream reader context for reading/updating
**              the bytes contained in the f_pbData only.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context to be initialized.
** [f_cbData]               -- Number of bytes in f_pbData
** [f_pbData]               -- Pointer to a byte array to be read/updated by 
**                             the stream reader/writer
** [f_dwOffset]             -- Byte offset of f_pdbData from where the stream reader
**                             or writer will start.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_Init(
    __inout                         DRM_M2TS_BIT_STREAM_CONTEXT  *f_poBSRContext,
    __in                      const DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )      DRM_BYTE                     *f_pbData,
    __in                      const DRM_DWORD                     f_dwOffset )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_dwOffset < f_cbData );

    MEMSET( f_poBSRContext, 0, SIZEOF( DRM_M2TS_BIT_STREAM_CONTEXT ) );

    f_poBSRContext->cbData        = f_cbData;
    f_poBSRContext->pbData        = f_pbData;
    f_poBSRContext->dwByteOffset  = f_dwOffset;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_Init_UsePacket
**
** Synopsis:    Initialize bit stream reader context for reading/updating 
**              payload in the packets on the per unit chain.  Once the 
**              reader reaches the end of a packet, it will continue with 
**              the next packet till the end of the chain.  
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context to be initialized.
** [f_pPacket]              -- To initialize the context with a packet.  The packet 
**                             has a pointer field to point to the next packet 
**                             that the reader or writer will use to rotate to 
**                             next packet when current packet's data is exhausted.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_Init_UsePacket(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __inout         DRM_M2TS_PACKET                 *f_pPacket )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_pPacket != NULL );

    MEMSET( f_poBSRContext, 0, SIZEOF( DRM_M2TS_BIT_STREAM_CONTEXT ) );

    f_poBSRContext->pPacket = f_pPacket;

    /* Set the states (data pointer, size, offset, ...) based on the first packet on the chain */
    (DRM_VOID)_RotateToNextPacket( f_poBSRContext );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_GetRemainingBytes
**
** Synopsis:    Get the number of remaining bytes.  It doesn't include the 
**              byte that is currently partial read (the bit offset does not
**              equal to 0.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
**
** Returns:                 The number of bytes remained on the stream reader/
**                          writer's buffer or linked list packets.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_M2TS_BSR_GetRemainingBytes(
    __inout    DRM_M2TS_BIT_STREAM_CONTEXT  *f_poBSRContext )
{
    DRM_RESULT               dr      = DRM_SUCCESS;
    DRM_DWORD                cBytes  = 0;
    const DRM_M2TS_PACKET   *pPacket = NULL;

    DRMASSERT( f_poBSRContext != NULL );
    __analysis_assume( f_poBSRContext != NULL );

    pPacket = f_poBSRContext->pPacket;

    if( !f_poBSRContext->fOutOfRange )
    {
        if( f_poBSRContext->fUseCounter )
        {
            cBytes = f_poBSRContext->cCounter;
        }
        else
        {
            ChkDR( DRM_DWordSub( f_poBSRContext->cbData, f_poBSRContext->dwByteOffset, &cBytes ) );

            while( pPacket != NULL )
            {
                dr = DRM_DWordAdd( cBytes, pPacket->cbPayload, &cBytes );
                if( DRM_FAILED( dr ) )
                {
                    f_poBSRContext->fOutOfRange = TRUE;
                    ChkDR( dr );
                }
                pPacket = pPacket->pNextPerUnit;
            }

            /* Minus the byte that is partially read */
            if( f_poBSRContext->wBitOffset > 0 && cBytes > 0 )
            {
                cBytes--;
            }
        }
    }

ErrorExit:
    return DRM_SUCCEEDED( dr ) ? cBytes : 0;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_SetCounter
**
** Synopsis:    Set the counter which sets a logical boundary to limit 
**              the bytes to read/update
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cbCounter]            -- The maximum number of bytes that
**                             the stream reader/writer can read/write
**                             from its current position.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFER_BOUNDS_EXCEEDED if the remaining bytes
**                          in the buffer or linked list of packets is less than 
**                          f_cbCounter.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_SetCounter(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cbCounter )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( f_poBSRContext != NULL );
    DRMASSERT( f_cbCounter > 0 && f_cbCounter <= DRM_M2TS_BSR_GetRemainingBytes( f_poBSRContext ) );
    ChkArg( f_cbCounter > 0 && f_cbCounter <= DRM_M2TS_BSR_GetRemainingBytes( f_poBSRContext ) );

    if( f_poBSRContext->fOutOfRange )
    {
        ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );
    }

    f_poBSRContext->fUseCounter   = TRUE;
    f_poBSRContext->cCounter      = f_cbCounter;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_ReadBitsToBYTE
**
** Synopsis:    Read the specified bits and return them in BYTE.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToRead]          -- Number of bits to read.
** [f_pbValue]              -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFER_BOUNDS_EXCEEDED if the remaining
**                          bits in the buffer or the linked list of packets
**                          is less than f_cBitsToRead.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_ReadBitsToBYTE(
    __inout                     DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in                  const DRM_DWORD                        f_cBitsToRead,
    __out_ecount( 1 )           DRM_BYTE                        *f_pbValue )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   dwValue = 0;

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_cBitsToRead <= SIZEOF( DRM_BYTE ) * BITS_PER_BYTE );
    ChkArg( f_pbValue != NULL );

    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( f_poBSRContext, f_cBitsToRead, &dwValue ) );

    ChkDR( DRM_DWordToByte( dwValue, f_pbValue ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_ReadBitsToWORD
**
** Synopsis:    Read the specified bits and return them in WORD.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToRead]          -- Number of bits to read.
** [f_pwValue]              -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFER_BOUNDS_EXCEEDED if the remaining
**                          bits in the buffer or the linked list of packets
**                          is less than f_cBitsToRead.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_ReadBitsToWORD(
    __inout                     DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in                  const DRM_DWORD                        f_cBitsToRead,
    __out_ecount( 1 )           DRM_WORD                        *f_pwValue )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   dwValue = 0;

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_cBitsToRead <= SIZEOF( DRM_WORD ) * BITS_PER_BYTE );
    ChkArg( f_pwValue != NULL );

    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( f_poBSRContext, f_cBitsToRead, &dwValue ) );

    DRMASSERT( dwValue <= MAX_UNSIGNED_TYPE( DRM_WORD ) );
    *f_pwValue = (DRM_WORD) dwValue;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_ReadBitsToDWORD
**
** Synopsis:    Read the specified bits and return them in DWORD.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToRead]          -- Number of bits to read.
** [f_pdwValue]             -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFER_BOUNDS_EXCEEDED if the remaining
**                          bits in the buffer or the linked list of packets
**                          is less than f_cBitsToRead.
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_ReadBitsToDWORD(
    __inout                     DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in                  const DRM_DWORD                        f_cBitsToRead,
    __out_ecount( 1 )           DRM_DWORD                       *f_pdwValue )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_DWORD   dwReturnValue       = 0;
    DRM_BYTE    bCurrentShiftedByte = 0;
    DRM_DWORD   iBit                = 0;

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_pdwValue != NULL );
    ChkArg( f_cBitsToRead <= SIZEOF( DRM_DWORD ) * BITS_PER_BYTE );

    ChkBOOL( !f_poBSRContext->fOutOfRange, DRM_E_BUFFER_BOUNDS_EXCEEDED );

    if( f_cBitsToRead > 0 )
    {
        DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );

        if( f_poBSRContext->wBitOffset == 0 && f_cBitsToRead == BITS_PER_BYTE )
        {
            dwReturnValue = bCurrentShiftedByte;
            DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );
        }
        else
        {
            bCurrentShiftedByte <<= f_poBSRContext->wBitOffset;

            for( iBit = 0; iBit < f_cBitsToRead; iBit++ )
            {
                if( f_poBSRContext->wBitOffset == 0 )
                {
                    DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
                }

                /* Copy left most bit */
                dwReturnValue = ( dwReturnValue << 1 ) | ( DRM_BYTE )( ( ( bCurrentShiftedByte & 0x80 ) >> 7 ) );
                bCurrentShiftedByte <<= 1;
                f_poBSRContext->wBitOffset++;

                DRMASSERT( f_poBSRContext->wBitOffset <= BITS_PER_BYTE );
                if( f_poBSRContext->wBitOffset >= BITS_PER_BYTE )
                {
                    f_poBSRContext->wBitOffset = 0;
                    DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );
                }
            }
        }
    }

    *f_pdwValue = dwReturnValue;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_SkipBits
**
** Synopsis:    Skip the number of bits specified.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToRead]          -- Number of bits to skip.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_SkipBits(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cBitsToSkip )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_BYTE    bCurrentShiftedByte = 0;
    DRM_DWORD   iBit                = 0;
    DRM_DWORD   cBytes              = 0;
    DRM_DWORD   cBitsRemaining      = f_cBitsToSkip;

    ChkArg( f_poBSRContext != NULL );
    if( f_poBSRContext->fOutOfRange
     || ( f_poBSRContext->fUseCounter && f_poBSRContext->cCounter == 0 ) )
    {
        f_poBSRContext->fOutOfRange = TRUE;
        ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );
    }

    /* To accelerate to process, first try to skip in bytes */
    cBytes = f_cBitsToSkip / BITS_PER_BYTE;
    if( cBytes > 0 )
    {
        ChkDR( DRM_M2TS_BSR_SkipBytes( f_poBSRContext, cBytes ) );
        cBitsRemaining = f_cBitsToSkip % BITS_PER_BYTE;
        if( f_poBSRContext->wBitOffset != 0 
         && cBitsRemaining )
        {
            DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
        }
    }

    /* Skip the remaining bits */
    for( iBit = 0; iBit < cBitsRemaining; iBit++ )
    {
        if( f_poBSRContext->wBitOffset == 0 )
        {
            DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
        }

        f_poBSRContext->wBitOffset++;

        DRMASSERT( f_poBSRContext->wBitOffset <= BITS_PER_BYTE );
        if( f_poBSRContext->wBitOffset >= BITS_PER_BYTE )
        {
            f_poBSRContext->wBitOffset = 0;

            /* Advance to next byte only when there are more bits to skip */
            DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );
        }
    }

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    DRM_M2TS_BSR_SkipBytes
**
** Synopsis:    Skip the number of bytes specified.  
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBytesToSkip]         -- Number of bytes to skip.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_SkipBytes(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cBytesToSkip )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_DWORD   cbBytesToSkip       = f_cBytesToSkip;
    DRM_BYTE    bCurrentShiftedByte = 0;

    ChkArg( f_poBSRContext != NULL );

    if( f_poBSRContext->fOutOfRange
     || ( !f_poBSRContext->fUseCounter && f_poBSRContext->pPacket == NULL && f_poBSRContext->dwByteOffset >= f_poBSRContext->cbData )
     || (  f_poBSRContext->fUseCounter && f_poBSRContext->cCounter == 0 ) )
    {
        f_poBSRContext->fOutOfRange = TRUE;
        ChkDR( DRM_E_BUFFER_BOUNDS_EXCEEDED );
    }

    while( cbBytesToSkip > 0 )
    {
        DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
        DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );

        cbBytesToSkip--;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_SkipPointerField 
**
** Synopsis:    Read the PAT or PMT's pointer_field and skip the bytes
**              specified by the pointer_field.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_M2TS_BSR_SkipPointerField(
    __inout     DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE    bPointer    = 0;

    ChkArg( f_poBSRContext != NULL );

    /* Read pointer_field(8) */
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( f_poBSRContext, 8, &bPointer ) );
    if( bPointer > 0 )
    {
        ChkDR( DRM_M2TS_BSR_SkipBytes( f_poBSRContext, bPointer ) ); 
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_WriteBitsFromBYTE
**
** Synopsis:    Updates the specified number of least significant bits from 
**              input value to current position.  The bits to be updated must
**              be on the current shifted byte.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToWrite]         -- Number of bits to write to current position.
** [f_bValue]               -- Contain the value to be written to the bit stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_WriteBitsFromBYTE(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cBitsToWrite,
    __in            DRM_BYTE                         f_bValue )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    DRM_BYTE                     bMask                      = 0x00;
    DRM_BYTE                    *pbOneByte                  = NULL;
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRContextShallowCloned   = { 0 };

    ChkArg( f_poBSRContext != NULL );
    /* Only allow to update bits in current shifted byte */
    ChkArg( f_cBitsToWrite <= BITS_PER_BYTE );
    ChkArg( f_poBSRContext->wBitOffset <= BITS_PER_BYTE );
    ChkArg( f_poBSRContext->wBitOffset + f_cBitsToWrite <= BITS_PER_BYTE );

    oBSRContextShallowCloned = *f_poBSRContext;

    if( f_cBitsToWrite > 0 )
    {
        DRM_M2TS_BSR_GET_CURRENT_BYTE_POINTER( &oBSRContextShallowCloned, pbOneByte );
    
        if( f_cBitsToWrite != BITS_PER_BYTE )
        {
            /* Shift the source bits to position */
            f_bValue <<= BITS_PER_BYTE - f_cBitsToWrite;
            f_bValue >>= oBSRContextShallowCloned.wBitOffset;

            /* 
            ** Set the mask so that all the bits are 1 except the bits to be updated to 0s 
            ** For example: To update 2nd and 3rd bits the mask should be 1001 1111 
            */
            bMask = (DRM_BYTE)( 0xFF << ( BITS_PER_BYTE - oBSRContextShallowCloned.wBitOffset ) );
            bMask |= 0xFF >> ( oBSRContextShallowCloned.wBitOffset + f_cBitsToWrite );
        }

        /* Reset the target bits that are going to be updated to 0s */
        *pbOneByte &= bMask;

        /* Update the requested bits */
        *pbOneByte |= f_bValue;

        /* Skip the updated bits using the original BSR context */
        ChkDR( DRM_M2TS_BSR_SkipBits( f_poBSRContext, f_cBitsToWrite ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_WriteBitsFromWORD
**
** Synopsis:    Update the specified number of least significant bits from 
**              input value to current position.
**              This function allows to update current and next byte only.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cBitsToWrite]         -- Number of bits to write to current position.
** [f_wValue]               -- Contain the value to be written to the bit stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_WriteBitsFromWORD(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cBitsToWrite,
    __in      const DRM_WORD                         f_wValue )
{
    DRM_RESULT                  dr                          = DRM_SUCCESS;
    DRM_BYTE                    bLeft                       = (DRM_BYTE)( f_wValue >> BITS_PER_BYTE );
    DRM_BYTE                    bRight                      = (DRM_BYTE)( (f_wValue << BITS_PER_BYTE) >> BITS_PER_BYTE );
    DRM_DWORD                   cRemainingBits              = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContextShallowCloned    = { 0 };

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_poBSRContext->wBitOffset <= (DRM_WORD)BITS_PER_BYTE );
    cRemainingBits = (DRM_DWORD)( BITS_PER_BYTE - f_poBSRContext->wBitOffset );
    ChkArg( f_cBitsToWrite <= ( cRemainingBits + BITS_PER_BYTE ) );

    oBSRContextShallowCloned = *f_poBSRContext;

    if( f_cBitsToWrite <= cRemainingBits )
    {
        ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContextShallowCloned, f_cBitsToWrite, bRight ) );
    }
    else
    {
        /* 
        ** if the bits to write is longer than the remaining bits of the current byte then 
        ** it has also to include the entire next byte.
        */
        ChkArg( f_cBitsToWrite == (DRM_DWORD)( cRemainingBits + BITS_PER_BYTE ) );
        ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContextShallowCloned, cRemainingBits, bLeft ) );
        ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContextShallowCloned, BITS_PER_BYTE, bRight ) );
    }

    /* Skip the updated bits using the original BSR context */
    ChkDR( DRM_M2TS_BSR_SkipBits( f_poBSRContext, f_cBitsToWrite ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_InsertBytes
**
** Synopsis:    Insert the number of bytes specified to the current position.
**              The caller should ensure that there are enough space to insert
**              the byte array to the per unit chain.  The bit offset has
**              to be zero to call this function.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cbBytesToInsert]      -- Number of bytes to insert to current position.
** [f_pbByteToInsert]       -- Pointer to the byte array to be inserted to the 
**                             bit stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_InsertBytes(
    __inout                                 DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in                              const DRM_DWORD                        f_cbBytesToInsert,
    __in_bcount( f_cbBytesToInsert )  const DRM_BYTE                        *f_pbByteToInsert)
{
    DRM_RESULT                  dr                                      = DRM_SUCCESS;
    DRM_WORD                    cbBuffer                                = 0;
    DRM_WORD                    cbToCopy                                = 0;
    DRM_BYTE                    rgbBuffer[ DRM_M2TS_PACKET_SIZE * 2 ]   = { 0 };
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContextShallowCloned                = { 0 };

    ChkArg( f_poBSRContext != NULL );
    ChkArg( f_poBSRContext->wBitOffset == 0 );
    ChkArg( f_cbBytesToInsert <= DRM_M2TS_PACKET_SIZE );
    ChkArg( f_pbByteToInsert != NULL );

    oBSRContextShallowCloned = *f_poBSRContext;

    /* Copy the bytes to be inserted to the buffer first */
    MEMCPY( rgbBuffer, f_pbByteToInsert, f_cbBytesToInsert );
    ChkDR( DRM_DWordToWord( f_cbBytesToInsert, &cbBuffer ) );

    do
    {
        DRM_DWORD cbTemp = 0;
        ChkDR( DRM_DWordSub( oBSRContextShallowCloned.cbData, oBSRContextShallowCloned.dwByteOffset, &cbTemp ) );
        ChkDR( DRM_DWordToWord( cbTemp, &cbToCopy ) );

        /* Copy the remaining bytes from current (packet) data to the buffer */
        AssertChkArg( cbToCopy + cbBuffer <= SIZEOF( rgbBuffer ) );
        MEMCPY( rgbBuffer + cbBuffer, oBSRContextShallowCloned.pbData + oBSRContextShallowCloned.dwByteOffset, cbToCopy );
        cbBuffer += cbToCopy;

        /* Update it */
        MEMCPY( oBSRContextShallowCloned.pbData + oBSRContextShallowCloned.dwByteOffset, rgbBuffer, cbToCopy );

        /* Shift the remaining bytes to the begining of the buffer */
        cbBuffer -= cbToCopy;
        MEMMOVE( rgbBuffer, rgbBuffer + cbToCopy, cbBuffer );
      
    } while( _RotateToNextPacket( &oBSRContextShallowCloned ) );

    /* Increase the counter by the same number of bytes inserted */
    if( f_poBSRContext->fUseCounter )
    {
        f_poBSRContext->cCounter += f_cbBytesToInsert;
    }

    /* Skip the inserted bytes using the original BSR context */
    ChkDR( DRM_M2TS_BSR_SkipBytes( f_poBSRContext, f_cbBytesToInsert ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_UpdateCRC
**
** Synopsis:    Calculates the CRC and updates it.  The caller should 
**              set the byte offset from where the CRC calculation will start
**              before calling this function.  The calculated CRC will be
**              written to where the CRC calculation ends.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cbBytesToCalculate]   -- Number of bytes from current position to 
**                             be included in calculating the CRC.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_UpdateCRC(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cbBytesToCalculate )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_DWORD   dwCRC               = MAX_UNSIGNED_TYPE( DRM_DWORD ); /* 0xFFFFFFFF */
    DRM_BYTE    bCurrentShiftedByte = 0;
    DRM_WORD    dwHighWord          = 0;
    DRM_WORD    dwLowWord           = 0;
    DRM_DWORD   iByte               = 0;

    ChkArg( f_poBSRContext != NULL );

    for( iByte = 0; iByte < f_cbBytesToCalculate; iByte++ )
    {
        DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
        DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );

        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[ ( dwCRC >> 24 ) ^ bCurrentShiftedByte ];
    }

    dwHighWord = dwCRC >> 16;
    dwLowWord  = ( dwCRC << 16 ) >> 16 ;

    ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( f_poBSRContext, 16, dwHighWord ) );
    ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( f_poBSRContext, 16, dwLowWord ) );

#ifdef DBG
    /* Verify the CRC - CRC should be a zero after the CRC_32 field is included. */
    {
        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[( dwCRC >> 24 ) ^ ( DRM_BYTE )( dwHighWord >> 8 ) ];
        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[( dwCRC >> 24 ) ^ ( DRM_BYTE )( ( dwHighWord << 8 ) >> 8 ) ];
        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[( dwCRC >> 24 ) ^ ( DRM_BYTE )( dwLowWord >> 8 ) ];
        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[( dwCRC >> 24 ) ^ ( DRM_BYTE )( ( dwLowWord << 8 ) >> 8 ) ];

        DRMASSERT( dwCRC == 0 );
    }
#endif /* DBG */

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_BSR_ValidateCRC
**
** Synopsis:    Validates the CRC.  The caller should set the byte offset 
**              from where the CRC calculation will start before calling
**              this function.  f_cbBytesToCalculate should not include 
**              the CRC field.
**
** Arguments:
**
** [f_poBSRContext]         -- Pointer to a bit stream context.
** [f_cbBytesToCalculate]   -- Number of bytes from current position to 
**                             be included in calculating the CRC.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_CRC_FIELD_INVALID if CRC doesn't match.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BSR_ValidateCRC(
    __inout         DRM_M2TS_BIT_STREAM_CONTEXT     *f_poBSRContext,
    __in      const DRM_DWORD                        f_cbBytesToCalculate )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_DWORD   dwCRC               = MAX_UNSIGNED_TYPE( DRM_DWORD ); /* 0xFFFFFFFF */
    DRM_BYTE    bCurrentShiftedByte = 0;
    DRM_DWORD   iByte               = 0;

    ChkArg( f_poBSRContext != NULL );
    ChkOverflow( f_cbBytesToCalculate + DRM_M2TS_CRC_SIZE, f_cbBytesToCalculate );
    ChkArg( DRM_M2TS_BSR_GetRemainingBytes( f_poBSRContext ) >= f_cbBytesToCalculate + DRM_M2TS_CRC_SIZE );

    for( iByte = 0; iByte < f_cbBytesToCalculate + DRM_M2TS_CRC_SIZE; iByte++ )
    {
        DRM_M2TS_BSR_SHIFTED_BYTE( f_poBSRContext, bCurrentShiftedByte );
        DRM_M2TS_BSR_INC_BYTE_OFFSET( f_poBSRContext );

        dwCRC = ( dwCRC << 8 ) ^ s_crcTable[ ( dwCRC >> 24 ) ^ bCurrentShiftedByte ];
    }

    ChkBOOL( dwCRC == 0, DRM_E_M2TS_CRC_FIELD_INVALID );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
