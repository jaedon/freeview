/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMPARSERS_UTF8_C

#include <oemparsers.h>

#include <drmutftypes.h>
#include <drmerr.h>
#include <drmprofile.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*******************************************************************************
** Function: DRM_UTF8_ReadCodePoint8
**
** Synopsis: Reads a single code point from a buffer of UTF-8 code units.
**
** Arguments:
**
** [f_pchSource]     -- Pointer to UTF-8 data buffer.
** [f_pdstrwSource]  -- Window covering a range of UTF-8 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
** [f_pcp]           -- Pointer that will receive the next code point. On
**                      error this is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid (such as a
**                   window with zero or negative width).
**                   DRM_E_UTF_INVALID_CODE - if UTF-8 sequence is illegal.
**                   DRM_E_UTF_UNEXPECTED_END - if parsing ended on a partial
**                       character.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTF8_ReadCodePoint8(
    __in     const DRM_CHAR          *f_pchSource,
    __inout        DRM_STRING_WINDOW *f_pdstrwSource,
    __out          DRM_CODE_POINT    *f_pcp )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_CODE_POINT  cp              = 0;
    DRM_BYTE        cLeadChar       = 0;
    DRM_DWORD       cchCodeUnits    = 0;
    DRM_DWORD       ich             = 0;
    DRM_DWORD       ichCurrentUnit  = 0;
    DRM_DWORD       ichMaxUnit      = 0;
    DRM_BYTE        rgbUnits[4]     = { 0 };

    ChkArg( f_pchSource != NULL );
    ChkArg( f_pdstrwSource != NULL );
    ChkArg( f_pcp != NULL );
    ChkArg( f_pdstrwSource->m_ichMin < f_pdstrwSource->m_ichMaxExclusive );

    ich = f_pdstrwSource->m_ichMin;

    cLeadChar = (DRM_BYTE) GET_CHAR( f_pchSource, ich ); /* grab one byte */

    /*
    ** The number of bits set at the top of the first code unit (aka first
    ** character if readers prefer) encodes the number of units that follow
    ** it. The below code does *some* range validation as well, but is not
    ** guaranteed to do all of it. Subsequent logic will do full validation!!
    */
    if( cLeadChar < 0x80UL )
    {
        /* It's an ASCII value, i.e. cchCodeUnits == 1. All validation is done
        ** at this point, so finish up. */
        *f_pcp = cLeadChar;
        f_pdstrwSource->m_ichMin = ich + 1;

        /* EXIT here (common case) */
        goto ErrorExit;
    }
    else if( cLeadChar >= 0xC2UL && cLeadChar < 0xE0UL )
    {
        cchCodeUnits = 2;
    }
    else if( cLeadChar >= 0xE0UL && cLeadChar < 0xF0UL )
    {
        cchCodeUnits = 3;
    }
    else if( cLeadChar >= 0xF0UL && cLeadChar < 0xF5UL )
    {
        cchCodeUnits = 4;
    }
    else
    {
        ChkDR( DRM_E_UTF_INVALID_CODE );
    }

    /*
    ** Ensure the buffer contains enough room for *all* characters. If
    ** cchCodeUnits is 2, the answer is one larger than the original ich.
    */
    ichMaxUnit = ich + cchCodeUnits - 1;
    ChkOverflow( ichMaxUnit, ich );

    ChkBOOL( ichMaxUnit < f_pdstrwSource->m_ichMaxExclusive, DRM_E_UTF_UNEXPECTED_END );

    /* Special case what's already been read. The for loop helps too by
    ** starting at 1 */
    ich++;
    rgbUnits[0] = cLeadChar;

    /* function min is used for rgbUnits array safety */
    for( ichCurrentUnit = 1; ichCurrentUnit < min(cchCodeUnits, 4); ichCurrentUnit++ )
    {
        /* Advance ich as well while in here */
        rgbUnits[ichCurrentUnit] = GET_CHAR( f_pchSource, ich++ );

        ChkBOOL( IS_UTF8_TRAILING_CH( rgbUnits[ichCurrentUnit] ),
                 DRM_E_UTF_INVALID_CODE );
    }

    switch ( cchCodeUnits )
    {
        case 2:
            cp = UTF_CP_FROM_TWO_CH( rgbUnits[0], rgbUnits[1] );
            ChkBOOL( cp >= UTF8_MIN_TWO_BYTE_CP && cp < UTF8_MIN_THREE_BYTE_CP,
                     DRM_E_UTF_INVALID_CODE );

            break;

        case 3:
            cp = UTF_CP_FROM_THREE_CH( rgbUnits[0], rgbUnits[1], rgbUnits[2] );
            ChkBOOL( ( cp >= UTF8_MIN_THREE_BYTE_CP &&
                       cp < UTF8_MIN_FOUR_BYTE_CP &&
                      !IS_UTF16_SURROGATE_CP(cp) ), DRM_E_UTF_INVALID_CODE );

            break;

        case 4:
            cp = UTF_CP_FROM_FOUR_CH( rgbUnits[0], rgbUnits[1], rgbUnits[2], rgbUnits[3] );
            ChkBOOL( cp >= UTF8_MIN_FOUR_BYTE_CP && cp <= UTF_MAX_CODE_POINT,
                     DRM_E_UTF_INVALID_CODE );

            break;

        default:
            DRMASSERT( FALSE ); /* we should never get here */

            ChkDR( DRM_E_FAIL );

            break;
    }

    *f_pcp = cp;
    f_pdstrwSource->m_ichMin = ich;

ErrorExit:

    return dr;
}

/*******************************************************************************
** Function: DRM_UTF8_VerifyBytes
**
** Synopsis: Function that verifies validity of UTF-8 data
**           according to unicode.org specification.
**
** Arguments:
**
** [f_pbData]            -- Pointer to a data buffer that contains UTF-8.
**
** [f_cbData]            -- Size of f_pbData in bytes.
** [f_iMin]              -- Position in data buffer to begin reading (in
**                          code units, aka storage characters).
** [f_iMax]              -- Position in data buffer to stop reading (in
**                          code units, aka storage characters), inclusive.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: The first valid UTF-8 char should start right at f_iStart.
** If this assumption is too strong I can make a change.
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTF8_VerifyBytes(
    __in_bcount( f_cbData ) const DRM_BYTE  *f_pbData,
    __in                    const DRM_DWORD  f_cbData,
    __in                    const DRM_DWORD  f_iMin,
    __in                    const DRM_DWORD  f_iMax )
{
    DRM_RESULT        dr    = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrw = EMPTY_DRM_STRING_WINDOW;
    DRM_CODE_POINT    cp    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_UTF8, PERF_FUNC_DRM_UTF8_VerifyBytes );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    ChkArg( f_iMax < f_cbData );
    ChkArg( f_iMax >= f_iMin );

    /* Simply read all the code points out to validate their correctness */
    dstrw.m_ichMin = f_iMin;
    dstrw.m_ichMaxExclusive = f_iMax + 1; /* f_iMax is included */

    while ( dstrw.m_ichMin < dstrw.m_ichMaxExclusive )
    {
        ChkDR( DRM_UTF8_ReadCodePoint8( ( const DRM_CHAR * ) f_pbData, &dstrw, &cp ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

