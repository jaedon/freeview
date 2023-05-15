/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <oemsha1.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

/*
    OEMNOTE: There are optimizations that could be done.
    1.  The loop ranges in SHA are static so the loop can be unrolled.  This will make larger code but will be faster.
    2.  If loop is unrolled the ft and K functions can be put inline as to not incur a function call overhead
*/

/* These macros are for readability and should make sense when looking at the standard */
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

/* Define S (circular shift operator) */
#define S ROTATE_LEFT

/******************************************************************************/
static DRM_VOID DRM_CALL _PackDRMDWORD(
    __in              const DRM_DWORD *dwFrom,
    __in                    DRM_DWORD  dwCount,
    __out_bcount(dwCount*4) DRM_BYTE  *bOut)
{
    DRM_DWORD i = 0;
    DRM_DWORD j = 0;
    for(; i<dwCount; i++)
    {
        PUT_BYTE(bOut, j, (DRM_BYTE)((dwFrom[i] >> 24) & 0xff)); j++;
        PUT_BYTE(bOut, j, (DRM_BYTE)((dwFrom[i] >> 16) & 0xff)); j++;
        PUT_BYTE(bOut, j, (DRM_BYTE)((dwFrom[i] >> 8 ) & 0xff)); j++;
        PUT_BYTE(bOut, j, (DRM_BYTE)((dwFrom[i]      ) & 0xff)); j++;
    }
}

/******************************************************************************/

static DRMFORCEINLINE DRM_VOID DRM_CALL _GetDRMDWORD(
    __out_ecount(dwCount) DRM_DWORD *dwTo,
    __in                  DRM_DWORD  dwCount,
    __in            const DRM_BYTE  *bIn )
{
    DRM_DWORD i = 0;
    DRM_DWORD j = 0;
    for( ; i < dwCount; i++, j+=4)
    {
        dwTo[i] = ( ( (DRM_DWORD) GET_BYTE(bIn,j)   ) << 24 ) |
                  ( ( (DRM_DWORD) GET_BYTE(bIn,j+1) ) << 16 ) |
                  ( ( (DRM_DWORD) GET_BYTE(bIn,j+2) ) << 8 )  |
                  (   (DRM_DWORD) GET_BYTE(bIn,j+3) );
    }
}


/******************************************************************************/

static DRM_LONG _ft(DRM_LONG b, DRM_LONG c, DRM_LONG d, DRM_LONG t)
{
    DRMASSERT(t<80);
    if(t >= 60)
        return (b^c^d);
    if(t>=40)
        return ((b&c)|(b&d)|(c&d));
    if(t>=20)
        return (b^c^d);
    if(t>=0)
        return ((b&c) | ((~b)&d));

    return 0;    /* If valid input we should never hit this */

}

/******************************************************************************/

static DRM_DWORD _K(DRM_SHORT t)
{

    DRMASSERT(t<80);
    if(t >= 60)
        return 0xCA62C1D6;
    if(t>=40)
        return 0x8F1BBCDC;
    if(t>=20)
        return 0x6ED9EBA1;
    if(t>=0)
        return 0x5A827999;

    return 0;    /* If valid input we should never hit this */
}

/******************************************************************************/

/* SHA operates on 512 bit blocks which is 16-dword sized blocks.  (16*32 = 512) */

/**********************************************************************
** Function:        _sha1
**
** Synopsis:    Meat of sha, this is the actual mathematical processing.
**
** Arguments:    [ABCDE] -- current state numbers of sha
**                [bInput] -- 64 byte block to put through the sha process
**
** Returns:        None
**
** Notes:        This version of SHA1 is written to reduce processor instructions
**                but the tradeoff is the 80 DWORD stack buffer.
**
***********************************************************************/
static DRM_NO_INLINE DRM_VOID _sha1(
    __in       DRM_DWORD ABCDE[5],
    __in const DRM_BYTE  bInput[__CB_DECL(DRM_SHA1_BLOCK_SIZE)] )
{
    DRM_DWORD Buf2[5];
    DRM_DWORD W[80];
    DRM_SHORT cCount;
    DRM_DWORD TEMP;


    _GetDRMDWORD(W,16,bInput);
    OEM_SECURE_DWORDCPY(Buf2, ABCDE, 5);
    for(cCount = 16; cCount < 80; cCount++)
    {
        TEMP = W[cCount-3] ^ W[cCount-8] ^ W[cCount-14] ^ W[cCount-16];
        W[cCount] = S(TEMP,1);
    }
    for(cCount = 0; cCount < 80; cCount++ )
    {
        TEMP = S(ABCDE[A],5);
        TEMP += _ft(ABCDE[B],ABCDE[C],ABCDE[D], cCount);
        TEMP += ABCDE[E];
        TEMP += W[cCount];
        TEMP += _K(cCount);
        ABCDE[E] = ABCDE[D];
        ABCDE[D] = ABCDE[C];
        ABCDE[C] = S(ABCDE[B],30);
        ABCDE[B] = ABCDE[A];
        ABCDE[A] = TEMP;
    }
    for (cCount = 0; cCount < 5; cCount++)
    {
        Buf2[cCount] += ABCDE[cCount];
    }
    OEM_SECURE_DWORDCPY(ABCDE, Buf2, 5);
}

/******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA1_Init(
    __inout DRM_SHA1_CONTEXT* pShaContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pShaContext != NULL );

    /*Zero out the buffer*/
    ZEROMEM((DRM_BYTE*)pShaContext, SIZEOF(DRM_SHA1_CONTEXT));

    /* Set the initial magic numbers */
    pShaContext->ABCDE[A] = 0x67452301;
    pShaContext->ABCDE[B] = 0xEFCDAB89;
    pShaContext->ABCDE[C] = 0x98BADCFE;
    pShaContext->ABCDE[D] = 0x10325476;
    pShaContext->ABCDE[E] = 0xC3D2E1F0;

ErrorExit:
    return dr;
}

/******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA1_Update(
    __in_bcount( cbData ) const DRM_BYTE         *pbData,
    __in                        DRM_DWORD         cbData,
    __inout                     DRM_SHA1_CONTEXT *pShaContext )
{
    return DRM_SHA1_UpdateOffset( pbData, 0, cbData, pShaContext );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA1_UpdateOffset(
    __in_bcount( ibData + cbData ) const DRM_BYTE         *pbData,
    __in                                 DRM_DWORD         ibData,
    __in                                 DRM_DWORD         cbData,
    __inout                              DRM_SHA1_CONTEXT *pShaContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwTempLen;

    ChkArg( pbData != NULL );
    ChkArg( pShaContext != NULL );

    /* How many bytes do we have remaining? */
    dwTempLen = pShaContext->dwLowByteCount & (DRM_SHA1_BLOCK_SIZE - 1);
    pShaContext->dwLowByteCount += cbData;

    if(pShaContext->dwLowByteCount < cbData)
    {
        /* We overflowed and wrapped around.  This means
        ** we need to increment the high order byte counter
        */
        pShaContext->dwHighByteCount++;
    }

    if (   dwTempLen > 0
        && cbData   >= (DRM_SHA1_BLOCK_SIZE - dwTempLen) )
    {
        /* We have enough to complete the last block.  Fill it and sha it */
        OEM_SECURE_MEMCPY_IDX(pShaContext->bTempBuffer, dwTempLen, pbData, ibData, DRM_SHA1_BLOCK_SIZE-dwTempLen);
        _sha1(pShaContext->ABCDE,pShaContext->bTempBuffer);

        ChkOverflow( ibData + (DRM_SHA1_BLOCK_SIZE - dwTempLen), ibData );
        ibData += DRM_SHA1_BLOCK_SIZE - dwTempLen;
        cbData -= DRM_SHA1_BLOCK_SIZE - dwTempLen;

        dwTempLen = 0;
    }

    /* Sha each portion of the buffer that is big enough */
    while(cbData>=DRM_SHA1_BLOCK_SIZE)
    {
#if SIXTEEN_BIT_ADDRESSING
        OEM_SECURE_MEMCPY_IDX(pShaContext->bTempBuffer, 0, pbData, ibData, DRM_SHA1_BLOCK_SIZE);
        _sha1( pShaContext->ABCDE,pShaContext->bTempBuffer);
#else
        _sha1( pShaContext->ABCDE,pbData + __CB_DECL(ibData));
#endif
        ibData += DRM_SHA1_BLOCK_SIZE;
        cbData -= DRM_SHA1_BLOCK_SIZE;
    }

    if(cbData)
    {
        OEM_SECURE_MEMCPY_IDX(pShaContext->bTempBuffer, dwTempLen, pbData, ibData, cbData);
    }
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA1_Finalize(
    __in                                DRM_SHA1_CONTEXT    *pShaContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE             rgbDigest[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwTempLen;
    DRM_DWORD  dwTotalBitLen[2];
    DRM_BYTE   bPaddingBuffer[__CB_DECL(DRM_SHA1_BLOCK_SIZE+12)] = {0};    /* Maximum block size we may need. */

    ChkArg( pShaContext != NULL );

    /* How many bytes do we need to make a SHA block? */
    dwTempLen = DRM_SHA1_BLOCK_SIZE - (pShaContext->dwLowByteCount & 63);

    /* This is there enough room for the padding?.*/
    if(dwTempLen<=8)
    {
        dwTempLen += DRM_SHA1_BLOCK_SIZE;
    }

    /*dwLowByteCount is the number of bytes so we have to multiply that by 8 to get the number of bits */
    dwTotalBitLen[1] = pShaContext->dwLowByteCount << 3;
    dwTotalBitLen[0] = (pShaContext->dwHighByteCount << 3) |
                        (pShaContext->dwLowByteCount >> 29); /* We could have overflowed so put the extra here */

    PUT_BYTE(bPaddingBuffer, 0, 0x80);    /* We add a 10000000 */

#if SIXTEEN_BIT_ADDRESSING
    {
        DRM_BYTE rgbTemp[__CB_DECL(2*SIZEOF(DRM_DWORD))];
        /*
        ** We do the packing to a temporary buffer because the offset into
        ** the padding buffer could be odd, and _PackDRMDWORD doesn't handle
        ** that case
        */
        _PackDRMDWORD(dwTotalBitLen,2,rgbTemp);
        DRM_BYT_CopyBytes( bPaddingBuffer, dwTempLen-8, rgbTemp, 0, 2*SIZEOF(DRM_DWORD) );
    }

#else

    /* Last 2 DWORDS are for the total bit length */
    _PackDRMDWORD(dwTotalBitLen,2,&(bPaddingBuffer[(dwTempLen-8)]));

#endif

    ChkDR( DRM_SHA1_Update(bPaddingBuffer, dwTempLen, pShaContext) );

    /* Extract the digest and save it. */
    _PackDRMDWORD(pShaContext->ABCDE,5,rgbDigest);

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
