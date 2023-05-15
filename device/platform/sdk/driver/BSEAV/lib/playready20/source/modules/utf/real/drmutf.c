/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutf.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**  This information is from unicode.org specification.
**
**  Terminology:
**  1. "Code Point" - that which is displayed on the screen. Also called a
**                    linguistic character in Windows documentation.
**  2. "Code Unit" - that which is iterated through via index, each of size
**                   size 1, 2, or 4 8bit bytes. Also called a storage
**                   character in Windows documenation. cch always refers to
**                   these.
**
**  The maximum Code Point in unicode is U+10FFFF.
**  The Code Points in the range U+D800..U+DFFF are illegal and reserved for
**  encodings.
**
**
**  UTF-8 Encoding's Bit Distribution:
**  ==================================
**
**  unicode                     bytes   bits   UTF-8 representation
**  --------------------------  -----   ----   -----------------------------------
**  00000000 0xxxxxxx           1       7      0xxxxxxx
**  00000yyy yyxxxxxx           2       11     110yyyyy 10xxxxxx
**  zzzzyyyy yyxxxxxx           3       16     1110zzzz 10yyyyyy 10xxxxxx
**  000uuuuu zzzzyyyy yyxxxxxx  4       21     11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
**  --------------------------  -----   ----   -----------------------------------
**
**  Well-Formed UTF-8 Byte Sequences:
**
**  Unicode ranges      1st Byte  2nd Byte  3rd Byte  4th Byte
**  ------------------  --------  --------  --------  --------
**  U+0000..U+007F      00..7F
**  U+0080..U+07FF      C2..DF    80..BF
**  U+0800..U+0FFF      E0        A0..BF    80..BF
**  U+1000..U+CFFF      E1..EC    80..BF    80..BF
**  U+D000..U+D7FF      ED        80..9F    80..BF
**  U+E000..U+FFFF      EE..EF    80..BF    80..BF
**  U+10000..U+3FFFF    F0        90..BF    80..BF    80..BF
**  U+40000..U+FFFFF    F1..F3    80..BF    80..BF    80..BF
**  U+100000..U+10FFFF  F4        80..8F    80..BF    80..BF
**
**  Notes:
**  1. The special cases for 3 bytes characters are due to the illegality of
**     the range U+D800..U+DFFF.
**  2. UTF-8 rules do not allow using a larger format to represent a unicode
**     value encode-able with fewer bytes.
**
**
**  UTF-16 Encoding Rules:
**  ======================
**  1. Code points in the range U+0000..U+FFFF are encoded using one UTF-16
**     value.
**  2. Code points in the range U+10000..U+10FFFF are encoded first by
**     subtracting 0x10000 to get a 20bit number, then by storing them in two
**     UTF-16 values, each containing 10bits:
**       1. The first "surrogate" is in the range 0xD800-0xDBFF.
**       2. The second "surrogate" is in the range 0xDC00-0xDFFF.
**
**  Notes:
**  1. There is no confusion between surrogates and U+D800..U+DFFF as the
**     latter is an illegal range.
**  2. UTF-16 *is* endian-specific, and both little endian (UTF-16LE), big
**     endian (UTF-16BE), and auto-detection (via BOM) are defined. This file
**     only supports characters in little endian format.
**     It neither supports characters in big endian
**     nor supports auto-detection.
**
**
**`    (UTF-16LE is used in this file regardless of native architecture)
**     UTF-16LE encoding scheme:
**     The Unicode encoding scheme that serializes a UTF-16
**     code unit sequence as a byte sequence in little-endian format.
**
**     In UTF-16LE, the UTF-16 code unit sequence <004D 0430 4E8C D800 DF02> is
**     serialized as <4D 00 30 04 8C 4E 00 D8 02 DF>.
**
**     In UTF-16LE, an initial byte sequence <FF FE> is interpreted as
**     U+FEFF zero width no-break space.
**
**
**     (UTF-16BE is never used in this file - description for information only)
**     UTF-16BE encoding scheme:
**     The Unicode encoding scheme that serializes a UTF-16
**     code unit sequence as a byte sequence in big-endian format.
**
**     In UTF-16BE, the UTF-16 code unit sequence <004D 0430 4E8C D800 DF02>
**     is serialized as <00 4D 04 30 4E 8C D8 00 DF 02>.
**
**     In UTF-16BE, an initial byte sequence <FE FF> is interpreted as U+FEFF zero
**     width no-break space.
**
**
**
**  UTF-32 Encoding Rules:
**  ======================
**  A 32bit code unit has all the space and more necessary to hold the max
**  code point. As such, no conversion is necessary.
**
**  Notes:
**     UTF-32 *is* endian-specific, and both little endian (UTF-32LE), big
**     endian (UTF-32BE), and auto-detection (via BOM) are defined. This file
**     only supports characters in little endian format.
**     It neither supports characters in big endian
**     nor supports auto-detection.
**
**
**`    (UTF-32LE is used in this file regardless of native architecture)
**     UTF-32LE encoding scheme:
**     The Unicode encoding scheme that serializes a UTF-32
**     code unit sequence as a byte sequence in little-endian format.
**
**     In UTF-32LE, the UTF-32 code unit sequence
**     <0000004D 00000430 00004E8C 00010302> is serialized as
**     <4D 00 00 00 30 04 00 00 8C 4E 00 00 02 03 01 00>.
**
**     In UTF-32LE, an initial byte sequence <FF FE 00 00> is interpreted as
**     U+FEFF zero width no-break space.
**
**
**`    (UTF-32BE is never used in this file - description for information only)
**     UTF-32BE encoding scheme:
**     The Unicode encoding scheme that serializes a UTF-32
**     code unit sequence as a byte sequence in big-endian format.
**
**     In UTF-32BE, the UTF-32 code unit sequence
**     <0000004D 00000430 00004E8C 00010302> is serialized as
**     <00 00 00 4D 00 00 04 30 00 00 4E 8C 00 01 03 02>.
**
**     In UTF-32BE, an initial byte sequence <00 00 FE FF> is interpreted as U+FEFF
**     zero width no-break space.
**
******************************************************************************/

/*
** This file internally deals in code points, which use native endian-ness.
** When converting the DRM_WCHAR32, an endian conversion may be necessary.
*/
typedef DRM_DWORD DRM_CODE_POINT;

/*
** Any code point greater than 0x10FFFF is ill-formed.
*/
#define UTF_MAX_CODE_POINT (DRM_WCHAR32)0x0010FFFFUL

/*
** Unicode has 17 "planes", each with 2^16 code points. The exception is the
** "Base Multilingual Plane" (#0) which lacks the values U+D800..U+DFFF.
*/
#define UTF_PLANE0_MAX     (DRM_WCHAR32)0x0000FFFFUL
#define UTF_PLANE1_MIN     (DRM_WCHAR32)0x00010000UL

/* ---------------------- UTF-8 constants and macros ---------------------- */

#define UTF8_MIN_TWO_BYTE_CP    0x80UL
#define UTF8_MIN_THREE_BYTE_CP  0x800UL
#define UTF8_MIN_FOUR_BYTE_CP   UTF_PLANE1_MIN

/*
** Test that this byte looks like 10vvvvvv
** so that its value is in 80..BF range
*/
#define IS_UTF8_TRAILING_CH(b) (((b) & 0xC0) == 0x80)

/*
** 110yyyyy 10xxxxxx -> 00000000 00000000 00000yyy yyxxxxxx
*/
#define UTF_CP_FROM_TWO_CH(b1,b2) \
    ( ( ( ( (b1) & 0x1FUL ) << 0x6 ) | ( (b2) & 0x3FUL ) ) & 0x7FF )
/*
** 1110zzzz 10yyyyyy 10xxxxxx -> 00000000 00000000 zzzzyyyy yyxxxxxx
*/
#define UTF_CP_FROM_THREE_CH(b1,b2,b3) \
    ( ( ( ( (b1) & 0xFUL ) << 0xC ) | ( ( (b2) & 0x3FUL ) << 0x6 ) | ( (b3) & 0x3FUL ) ) & 0xFFFFUL )

/*
** 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx -> 00000000 000uuuuu zzzzyyyy yyxxxxxx
*/
#define UTF_CP_FROM_FOUR_CH(b1,b2,b3,b4) \
    ( ( ( ( (b1) & 0x7UL ) << 0x12 ) | ( ( (b2) & 0x3FUL ) << 0xC ) | ( ( (b3) & 0x3FUL ) << 0x6 ) | ( (b4) & 0x3FUL ) ) & 0x1FFFFFUL )

/*
** Takes a 6 bit value and converts to a trailing UTF-8 code unit.
*/
#define UTF8_MAKE_TRAILING_CH(_b_) ( (_b_) | 0x80 )

/*
** Creates a lead ch for UTF8 given
** a) The number of code units /top bits (n = 2-4), and
** b) The value bits in the lead byte (max bits is 7-n, i.e. going from 5 to 3).
*/
#define UTF8_MAKE_LEAD_CH(_n_, _b_) \
    ( ( ( 0xF0 << ( 4 - (_n_) ) ) | (_b_) ) & 0xFF )


/* ---------------------- UTF-16 constants and macros --------------------- */

/*
** High-surrogate code unit: A 16-bit code unit in the range 0xD800 to 0xDBFF,
** used in UTF-16 as the leading code unit of a surrogate pair.
**
** Low-surrogate code unit: A 16-bit code unit in the range 0xDC00 to 0xDFFF,
** used in UTF-16 as the trailing code unit of a surrogate pair.
*/
#define UTF16_HIGH_SURROGATE_MIN_CP  (DRM_CODE_POINT)0xD800UL
#define UTF16_HIGH_SURROGATE_MAX_CP  (DRM_CODE_POINT)0xDBFFUL
#define UTF16_LOW_SURROGATE_MIN_CP   (DRM_CODE_POINT)0xDC00UL
#define UTF16_LOW_SURROGATE_MAX_CP   (DRM_CODE_POINT)0xDFFFUL


#if TARGET_LITTLE_ENDIAN

#define IS_UTF16_HIGH_SURROGATE_WCH(_wch16_) \
    ( (_wch16_) >= UTF16_HIGH_SURROGATE_MIN_CP && \
      (_wch16_) <= UTF16_HIGH_SURROGATE_MAX_CP )

#define IS_UTF16_LOW_SURROGATE_WCH(_wch16_) \
    ( (_wch16_) >= UTF16_LOW_SURROGATE_MIN_CP && \
      (_wch16_) <= UTF16_LOW_SURROGATE_MAX_CP )

/* Each half has 10 bits of the 20 bit whole */
#define UTF32_FROM_SURROGATES_WCH(_wch16High_, _wch16Low_) \
    ( ( ( (_wch16High_) - UTF16_HIGH_SURROGATE_MIN_CP ) << 10UL ) + \
      ( (_wch16Low_) - UTF16_LOW_SURROGATE_MIN_CP ) + \
      UTF_PLANE1_MIN )

/* Rebase plane1 to zero (yielding a 20bit number instead of a 20bit + 16bit
** number), then take the top ten bits */
#define UTF16_HIGH_SURROGATE_FROM_CP(_cp_) \
    ( ( ( (_cp_) - UTF_PLANE1_MIN ) >> 10 ) + UTF16_HIGH_SURROGATE_MIN_CP )

/* Same as above, but take the bottom ten bits */
#define UTF16_LOW_SURROGATE_FROM_CP(_cp_) \
    ( ( ( (_cp_) - UTF_PLANE1_MIN ) & 0x3FFUL ) + UTF16_LOW_SURROGATE_MIN_CP )

#define UTF_CP_FROM_UTF32(_wch32_) (_wch32_)
#define UTF_CP_FROM_UTF16(_wch16_) (_wch16_)
#define UTF32_FROM_CP(_cp_) (_cp_)
#define UTF16_FROM_CP(_cp_) (_cp_)

#else

/*
** UTF-16BE encoding scheme:
** The Unicode encoding scheme that serializes a UTF-16
** code unit sequence as a byte sequence in big-endian format.
**
** In UTF-16BE, the UTF-16 code unit sequence <004D 0430 4E8C D800 DF02> is
** serialized as <00 4D 04 30 4E 8C D8 00 DF 02>.
**
** In UTF-16BE, an initial byte sequence <FE FF> is interpreted as
** U+FEFF zero width no-break space
**
**
** UTF-32BE encoding scheme:
** The Unicode encoding scheme that serializes a UTF-32
** code unit sequence as a byte sequence in big-endian format.
**
** In UTF-32BE, the UTF-32 code unit sequence
** <0000004D 00000430 00004E8C 00010302> is serialized as
** <00 00 00 4D 00 00 04 30 00 00 4E 8C 00 01 03 02>.
**
** In UTF-32BE, an initial byte sequence <00 00 FE FF> is interpreted as
** U+FEFF zero width no-break space
**
*/

#define IS_UTF16_HIGH_SURROGATE_WCH(_wch16_) \
    ( ( FLIP_WORD_BYTES( _wch16_ ) ) >= UTF16_HIGH_SURROGATE_MIN_CP && \
      ( FLIP_WORD_BYTES( _wch16_ ) ) <= UTF16_HIGH_SURROGATE_MAX_CP )

#define IS_UTF16_LOW_SURROGATE_WCH(_wch16_) \
    ( ( FLIP_WORD_BYTES( _wch16_ ) ) >= UTF16_LOW_SURROGATE_MIN_CP && \
      ( FLIP_WORD_BYTES( _wch16_ ) ) <= UTF16_LOW_SURROGATE_MAX_CP )

/* Each half has 10 bits of the 20 bit whole */
#define UTF32_FROM_SURROGATES_WCH(_wch16High_, _wch16Low_) \
    FLIP_DWORD_BYTES( ( ( ( ( FLIP_WORD_BYTES( _wch16High_ ) ) - UTF16_HIGH_SURROGATE_MIN_CP ) << 10UL ) + \
                      ( ( FLIP_WORD_BYTES( _wch16Low_ ) ) - UTF16_LOW_SURROGATE_MIN_CP ) + \
                      UTF_PLANE1_MIN ) \
                    )

/* Rebase plane1 to zero (yielding a 20bit number instead of a 20bit + 16bit
** number), then take the top ten bits */
#define UTF16_HIGH_SURROGATE_FROM_CP(_cp_) \
    FLIP_WORD_BYTES( ( ( (_cp_) - UTF_PLANE1_MIN ) >> 10 ) + UTF16_HIGH_SURROGATE_MIN_CP )

/* Same as above, but take the bottom ten bits */
#define UTF16_LOW_SURROGATE_FROM_CP(_cp_) \
    FLIP_WORD_BYTES( ( ( (_cp_) - UTF_PLANE1_MIN ) & 0x3FFUL ) + UTF16_LOW_SURROGATE_MIN_CP )

#define UTF_CP_FROM_UTF32(_wch32_) FLIP_DWORD_BYTES(_wch32_)
#define UTF_CP_FROM_UTF16(_wch16_) FLIP_WORD_BYTES(_wch16_)
#define UTF32_FROM_CP(_cp_) FLIP_DWORD_BYTES(_cp_)
#define UTF16_FROM_CP(_cp_) FLIP_WORD_BYTES(_cp_)

#endif

/* Create a macro that tests for _both_ high and low surrogates.
** Note: high surrogates are always numerically less than low surrogates
*/
#define IS_UTF16_SURROGATE_CP(_cp_) \
    ( ( _cp_ ) >= UTF16_HIGH_SURROGATE_MIN_CP && \
      ( _cp_ ) <= UTF16_LOW_SURROGATE_MAX_CP )


/*******************************************************************************
** Function: _ReadCodePoint16
**
** Synopsis: Reads a single code point from a buffer of UTF-16 code units.
**
** Arguments:
**
** [f_pwchSource]    -- Pointer to UTF-16 data buffer.
** [f_pdstrwSource]  -- Window covering a range of UTF-16 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
** [f_pcp]           -- Pointer that will receive the next code point. On
**                      error this is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid (such as a
**                   window with zero or negative width).
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**                   DRM_E_UTF_UNEXPECTED_END - if parsing ended on a partial
**                       character.
**
*******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _ReadCodePoint16(
    __in     const DRM_WCHAR         *f_pwchSource,
    __inout        DRM_STRING_WINDOW *f_pdstrwSource,
    __out          DRM_CODE_POINT    *f_pcp )
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_DWORD   ich        = 0;
    DRM_WCHAR   wch16src   = 0;
    DRM_WCHAR32 wch32      = 0;
    DRM_WCHAR32 wch16_high = 0;
    DRM_WCHAR32 wch16_low  = 0;

    ChkArg( f_pwchSource != NULL );
    ChkArg( f_pdstrwSource != NULL );
    ChkArg( f_pcp != NULL );
    ChkArg( f_pdstrwSource->m_ichMin < f_pdstrwSource->m_ichMaxExclusive );

    ich = f_pdstrwSource->m_ichMin;

    /* Read the first code unit. It may or may not be part of a two code unit
    ** surrogate pair */
    wch16src = f_pwchSource[ich++];

    if( IS_UTF16_HIGH_SURROGATE_WCH( wch16src ) )
    {
        /* The high surrogate should always be followed by a low surrogate.
        ** First verify there is space. Then verify the subsequent value is
        ** in the correct range */
        ChkBOOL( ich < f_pdstrwSource->m_ichMaxExclusive, DRM_E_UTF_UNEXPECTED_END );

        wch16_high = wch16src;
        wch16_low = f_pwchSource[ich++];

        /* Check if it is an unpaired high surrogate */
        ChkBOOL( IS_UTF16_LOW_SURROGATE_WCH( wch16_low ),
                 DRM_E_UTF_INVALID_CODE );

        wch32 = UTF32_FROM_SURROGATES_WCH( wch16_high, wch16_low );

        /* Success, update the output arguments */
        f_pdstrwSource->m_ichMin = ich;
        *f_pcp = UTF_CP_FROM_UTF32(wch32);
    }
    else
    {
        /* Verify this isn't a low surrogate out on its own. This would
        ** represent an illegal code point */
        ChkBOOL( !IS_UTF16_LOW_SURROGATE_WCH( wch16src ), DRM_E_UTF_INVALID_CODE );

        /* Success, update the output arguments */
        f_pdstrwSource->m_ichMin = ich;
        *f_pcp = UTF_CP_FROM_UTF16(wch16src);
    }

ErrorExit:
    return dr;
}


/*******************************************************************************
** Function: _WriteCodePoint16
**
** Synopsis: Converts a single code point into one or more UTF-16 code units.
**
** Arguments:
**
** [f_cp]            -- Code point to convert.
** [f_pwchTarget]    -- Pointer UTF-16 data buffer that used for result.
** [f_pdstrwTarget]  -- Window covering a range of UTF-16 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid (such as a
**                   window with zero or negative width).
**                   DRM_E_BUFFERTOOSMALL if f_pwchTarget size is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if code point is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _WriteCodePoint16(
    __in     const DRM_CODE_POINT     f_cp,
    __out          DRM_WCHAR         *f_pwchTarget,
    __inout        DRM_STRING_WINDOW *f_pdstrwTarget )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdstrwTarget != NULL );
    ChkArg( f_pwchTarget != NULL );
    ChkBOOL( f_pdstrwTarget->m_ichMin < f_pdstrwTarget->m_ichMaxExclusive, DRM_E_BUFFERTOOSMALL );

    /* Target is a character <= 0xFFFF */
    if( f_cp <= UTF_PLANE0_MAX )
    {
        /* Verify this isn't an UTF-16 surrogate value. These are illegal code
        ** points */
        ChkBOOL( !IS_UTF16_SURROGATE_CP(f_cp), DRM_E_UTF_INVALID_CODE );

        /* normal case - process the symbol */
        f_pwchTarget[f_pdstrwTarget->m_ichMin++] = (DRM_WCHAR)UTF16_FROM_CP(f_cp);
    }
    else
    {
        /* check for legality of the code point */
        ChkBOOL( f_cp <= UTF_MAX_CODE_POINT, DRM_E_UTF_INVALID_CODE );

        /* As the target is in plane1 or later, it will take two UTF-16 code
        ** units, not one. Verify there is sufficient buffer size */
        ChkBOOL( f_pdstrwTarget->m_ichMin + 1 < f_pdstrwTarget->m_ichMaxExclusive, DRM_E_BUFFERTOOSMALL );

        /* Store the two values. High first, low second (the order is always
        ** the same, independent of endian */
        f_pwchTarget[f_pdstrwTarget->m_ichMin++] =
            (DRM_WCHAR)UTF16_HIGH_SURROGATE_FROM_CP(f_cp);

        f_pwchTarget[f_pdstrwTarget->m_ichMin++] =
            (DRM_WCHAR)UTF16_LOW_SURROGATE_FROM_CP(f_cp);
    }
ErrorExit:
    return dr;
}


/*******************************************************************************
** Function: _ReadCodePoint8
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
static DRM_NO_INLINE DRM_RESULT DRM_CALL _ReadCodePoint8(
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
** Function: _WriteCodePoint8
**
** Synopsis: Converts a single code point into one or more UTF-8 code units.
**
** Arguments:
**
** [f_cp]            -- Code point to convert.
** [f_pchTarget]     -- Pointer UTF-8 data buffer that used for result.
** [f_pdstrwTarget]  -- Window covering a range of UTF-8 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid (such as a
**                   window with zero or negative width).
**                   DRM_E_BUFFERTOOSMALL if f_pchTarget size is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if code point is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _WriteCodePoint8(
    __in     const DRM_CODE_POINT     f_cp,
    __out          DRM_CHAR          *f_pchTarget,
    __inout        DRM_STRING_WINDOW *f_pdstrwTarget )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_CHAR        ch              = 0;
    DRM_CODE_POINT  cpBits          = 0;
    DRM_DWORD       cchCodeUnits    = 0;
    DRM_DWORD       ich             = 0;
    DRM_DWORD       ichMaxUnit      = 0;

    ChkArg( f_pdstrwTarget != NULL );
    ChkArg( f_pchTarget != NULL );

    ich = f_pdstrwTarget->m_ichMin;

    if( f_cp < UTF8_MIN_TWO_BYTE_CP )
    {
        /* It's an ASCII value, i.e. cchCodeUnits == 1. */
        ChkBOOL( ich < f_pdstrwTarget->m_ichMaxExclusive, DRM_E_BUFFERTOOSMALL );

        PUT_CHAR( f_pchTarget, ich, ( DRM_CHAR )f_cp );

        f_pdstrwTarget->m_ichMin = ich + 1;

        goto ErrorExit;
    }
    else if( f_cp < UTF8_MIN_THREE_BYTE_CP )
    {
        cchCodeUnits = 2;
    }
    else if( f_cp < UTF8_MIN_FOUR_BYTE_CP )
    {
        cchCodeUnits = 3;
        ChkBOOL( !IS_UTF16_SURROGATE_CP(f_cp), DRM_E_UTF_INVALID_CODE );
    }
    else
    {
        cchCodeUnits = 4;
        ChkBOOL( f_cp <= UTF_MAX_CODE_POINT, DRM_E_UTF_INVALID_CODE );
    }

    /*
    ** Ensure the buffer contains enough room for *all* characters. If
    ** cchCodeUnits is 1, the answer is identical to a standard single-char
    ** test.
    */
    ichMaxUnit = ich + cchCodeUnits - 1;
    ChkOverflow( ichMaxUnit, ich );

    ChkBOOL( ichMaxUnit < f_pdstrwTarget->m_ichMaxExclusive, DRM_E_BUFFERTOOSMALL );

    /*
    ** The simplest solution that works for 2, 3, and 4 is to write the values
    ** backwards. The last char/unit written, no matter the number, is always
    ** the bottom 6 bits. The 2nd-to-last written is the next 6 bits. This
    ** pattern continues up until the lead char/unit, which gets some extra
    ** bits at the top.
    */
    cpBits = f_cp;
    while ( ichMaxUnit > ich )
    {
        ch = UTF8_MAKE_TRAILING_CH( cpBits & 0x3F );
        PUT_CHAR( f_pchTarget, ichMaxUnit, ch );
        ichMaxUnit--;
        cpBits >>= 6;
    }

    DRMASSERT( ( cchCodeUnits == 4 ) && ( cpBits <= 0x7 ) ||
               ( cchCodeUnits == 3 ) && ( cpBits <= 0xF ) ||
               ( cchCodeUnits == 2 ) && ( cpBits <= 0x1F ) );

    ch = UTF8_MAKE_LEAD_CH( cchCodeUnits, cpBits );
    PUT_CHAR( f_pchTarget, ich, ch );

    f_pdstrwTarget->m_ichMin += cchCodeUnits;

    DRMASSERT( f_pdstrwTarget->m_ichMin <= f_pdstrwTarget->m_ichMaxExclusive );

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
        ChkDR( _ReadCodePoint8( ( const DRM_CHAR * ) f_pbData, &dstrw, &cp ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*******************************************************************************
** Function: _GetUTF8UnitCount
**
** Synopsis: Calculates number of UTF-8 code units required to convert specified
**           code point
**
** Arguments:
**
** [f_cp]        -- UTF-32 code point
** [f_pcchOut]   -- Pointer to code units counter.
**                  If this value is NULL then DRM_E_INVALIDARG is returned.
**
** Returns:      DRM_SUCCESS
**               DRM_E_INVALIDARG if argument f_pcchOut is NULL.
**               DRM_E_UTF_INVALID_CODE if code point is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _GetUTF8UnitCount(
    __in     const DRM_CODE_POINT  f_cp,
    __out          DRM_DWORD      *f_pcchOut )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_pcchOut != NULL );

    if( f_cp < UTF8_MIN_TWO_BYTE_CP )
    {
        *f_pcchOut = 1;
    }
    else if( f_cp < UTF8_MIN_THREE_BYTE_CP )
    {
        *f_pcchOut = 2;
    }
    else if( f_cp < UTF8_MIN_FOUR_BYTE_CP )
    {
        *f_pcchOut = 3;
    }
    else if( f_cp <= UTF_MAX_CODE_POINT ) /* UTF_MAX_CODE_POINT is valid symbol */
    {
        *f_pcchOut = 4;
    }
    else
    {
        /* illegal UTF-32 code point */
        ChkDR( DRM_E_UTF_INVALID_CODE );
    }

ErrorExit:
    return dr;
}



/*******************************************************************************
** Function: _GetUTF16UnitCount
**
** Synopsis: Calculates number of 16-bit code units required to convert
**           specified code point
**
** Arguments:
**
** [f_cp]        --  Code point to convert
** [f_pcchOut]   --  Pointer to code units counter.
**                   If this value is NULL then DRM_E_INVALIDARG is returned.
**
** Returns:          DRM_SUCCESS
**                   DRM_E_INVALIDARG if arguments f_pcchOut is NULL.
**                   DRM_E_UTF_INVALID_CODE if code point cannot be converted
**
*******************************************************************************/
static DRM_DWORD DRM_CALL _GetUTF16UnitCount(
    __in     const DRM_CODE_POINT f_cp,
    __out          DRM_DWORD     *f_pcchOut )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcchOut != NULL );
    /* Target is a character <= 0xFFFF */
    if( f_cp <= UTF_PLANE0_MAX )
    {
        /* UTF-16 surrogate values are illegal in UTF-32 */
        ChkBOOL( !IS_UTF16_SURROGATE_CP(f_cp), DRM_E_UTF_INVALID_CODE );
        *f_pcchOut = 1;
    }
    else
    {
        /* check for legality of UTF-32 code */
        ChkBOOL( f_cp <= UTF_MAX_CODE_POINT, DRM_E_UTF_INVALID_CODE );
        *f_pcchOut = 2;
    }

ErrorExit:
    return dr;
}


/*******************************************************************************
** Function: _GetUTF16UnitCountForUTF32
**
** Synopsis: Calculates number of 16-bit code units required to convert
**           UTF-32 string. This function is agnostic as to NULL termination
**
** Arguments:
**
** [f_pw32szSource]  -- Pointer to UTF-32 data buffer.
** [f_cchSource]     -- Length of UTF-32 data buffer in UTF-32 code units
** [f_pcchTarget]    -- Pointer to code units counter. This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_UTF_INVALID_CODE - if UTF32 sequence is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _GetUTF16UnitCountForUTF32(
    __in     const DRM_WCHAR32 *f_pw32szSource,
    __in     const DRM_DWORD    f_cchSource,
    __out          DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT         dr                    = DRM_SUCCESS;
    DRM_CODE_POINT     cp                    = 0;
    const DRM_WCHAR32 *pw32szCurSourceSymbol = (DRM_WCHAR32*) f_pw32szSource;
    const DRM_WCHAR32 *pw32szSourceEnd       = NULL;
    DRM_DWORD          cchLocal              = 0;
    DRM_DWORD          cchOut                = 0;

    ChkArg( f_pw32szSource != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Initialize boundaries for source */
    pw32szSourceEnd = (DRM_WCHAR32*) (f_pw32szSource + f_cchSource);
    ChkOverflow( pw32szSourceEnd, f_pw32szSource );

    /* Do processing */
    while( pw32szCurSourceSymbol < pw32szSourceEnd )
    {
        cp = UTF_CP_FROM_UTF32( (*pw32szCurSourceSymbol) );
        pw32szCurSourceSymbol++;

        ChkDR( _GetUTF16UnitCount( cp, &cchLocal ) );

        /* Check for cchOut overflow */
        ChkOverflow( cchOut + cchLocal, cchOut );
        cchOut += cchLocal;
    }
    /* Set the length of converted string */
    *f_pcchTarget = cchOut;

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF32toUTF16
**
** Synopsis: Converts UTF-32 string to UTF-16
**
** Arguments:
**
** [f_pw32szSource]  -- Pointer to UTF-32 data buffer.
** [f_cchSource]     -- Length of data buffer in UTF-32 code units
** [f_pwszTarget]    -- Pointer to UTF-16 data buffer.
**                      After successful conversion this buffer will be NULL-terminated
**                      If value is NULL then method calculates number of code units
**                      that required for conversion.
** [f_pcchTarget]    -- Pointer to DWORD containing UTF-16 buffer length in UTF-16 code
**                      units. After successful conversion it contains length
**                      of UTF-16 string including NULL terminator.
**                      In case of DRM_E_BUFFERTOOSMALL error it contains required length
**                      of UTF-16 string including NULL terminator.
**                      In case of error other than DRM_E_BUFFERTOOSMALL it remains unchanged.
**                      This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pwszTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF32 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF32toUTF16(
    __in_ecount( f_cchSource )          const DRM_WCHAR32 *f_pw32szSource,
    __in                                const DRM_DWORD    f_cchSource,
    __out_ecount_z_opt( *f_pcchTarget )       DRM_WCHAR   *f_pwszTarget,
    __inout                                   DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_STRING dstrTarget = EMPTY_DRM_STRING;

    ChkArg( f_pw32szSource != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Prepare parameters for DRM_STR_UTF32toDSTR */
    dstrTarget.pwszString = f_pwszTarget;
    if ( (*f_pcchTarget) > 0 )
    {
        dstrTarget.cchString = (*f_pcchTarget) - 1; /* reserve 1 symbol for NULL-terminator*/
    }

    /* Delegate processing */
    ChkDR( DRM_STR_UTF32toDSTR( f_pw32szSource, f_cchSource, &dstrTarget ) );

    if ( f_pwszTarget != NULL )
    {
        f_pwszTarget[ dstrTarget.cchString ] = 0; /* set NULL terminator */
    }
    *f_pcchTarget = dstrTarget.cchString + 1; /* Set length including NULL terminator */

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pcchTarget = dstrTarget.cchString + 1; /* Set length including NULL terminator */
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF32toDSTR
**
** Synopsis: Converts UTF-32 string to UTF-16 DRM string
**
** Arguments:
**
** [f_pw32szSource]  -- Pointer to UTF-32 data buffer.
** [f_cchSource]     -- Length of data buffer in UTF-32 code units
** [f_pdstrTarget]   -- Pointer to result DRM string. This value cannot be NULL.
**                      If f_pdstrTarget->pwszString is NULL then method calculates
**                      number of code units that required for conversion.
**                      In case of error other than DRM_E_BUFFERTOOSMALL counter remains
**                      unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pdstrTarget->pwszString is NULL
**                   or length of the string is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF32 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF32toDSTR(
    __in_ecount( f_cchSource ) const DRM_WCHAR32 *f_pw32szSource,
    __in                       const DRM_DWORD    f_cchSource,
    __inout                          DRM_STRING  *f_pdstrTarget )
{
    DRM_RESULT         dr                    = DRM_SUCCESS;
    DRM_CODE_POINT     cp                    = 0;
    const DRM_WCHAR32 *pw32szCurSourceSymbol = (DRM_WCHAR32*) f_pw32szSource;
    const DRM_WCHAR32 *pw32szSourceEnd       = NULL;
    DRM_STRING_WINDOW  dstrwTarget           = EMPTY_DRM_STRING_WINDOW;

    /* Validate source data */
    ChkArg( f_pw32szSource != NULL );

    /* Initialize boundaries for source */
    pw32szSourceEnd = (DRM_WCHAR32*) (f_pw32szSource + f_cchSource);
    ChkOverflow( pw32szSourceEnd, f_pw32szSource );

    /* Validate target data */
    ChkArg( f_pdstrTarget != NULL );
    ChkBOOL( f_pdstrTarget->pwszString != NULL, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( f_pdstrTarget->cchString > 0, DRM_E_BUFFERTOOSMALL );

    /* Initialize target substring */
    dstrwTarget.m_ichMin = 0;
    dstrwTarget.m_ichMaxExclusive = f_pdstrTarget->cchString;

    /* Do processing */
    while( pw32szCurSourceSymbol < pw32szSourceEnd )
    {
        cp = UTF_CP_FROM_UTF32( (*pw32szCurSourceSymbol) );
        pw32szCurSourceSymbol++;

        ChkDR( _WriteCodePoint16( cp, f_pdstrTarget->pwszString, &dstrwTarget ) );
    }
    /* Set the length of converted string */
    f_pdstrTarget->cchString = dstrwTarget.m_ichMin;

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* Calculate required length */
        dr = _GetUTF16UnitCountForUTF32( f_pw32szSource, f_cchSource, &(f_pdstrTarget->cchString) );
        if( DRM_SUCCEEDED( dr ) )
        {
            /* Need to return error */
            dr = DRM_E_BUFFERTOOSMALL;
        }
    }
    return dr;
}

/*******************************************************************************
** Function: _GetUTF32UnitCountForUTF16
**
** Synopsis: Calculates number of 32-bit code units required to convert
**           UTF-16 string. This function is agnostic as to NULL termination
**
** Arguments:
**
** [f_pdstrSource]   -- Pointer to UTF-16 drm string.
**                      f_pdstrSource->pwszString contains length of drm string
**                      in UTF-16 code units
** [f_pcchTarget]    -- Pointer to code units counter. This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _GetUTF32UnitCountForUTF16 (
    __in     const DRM_STRING  *f_pdstrSource,
    __out          DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_CODE_POINT    cp           = 0;
    DRM_STRING_WINDOW dstrwSource  = EMPTY_DRM_STRING_WINDOW;
    DRM_DWORD         cch          = 0;

    ChkArg( f_pdstrSource != NULL );
    ChkArg( f_pdstrSource->pwszString != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Initialize source substring */
    dstrwSource.m_ichMin = 0;
    dstrwSource.m_ichMaxExclusive = f_pdstrSource->cchString;

    /* Do processing */
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint16( f_pdstrSource->pwszString, &dstrwSource, &cp ) );

        cch++; /* One UTF-32 symbol per code point */
    }
    *f_pcchTarget = cch;  /* Set the result */

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF16toUTF32
**
** Synopsis: Converts UTF-16 string to UTF-32
**
** Arguments:
**
** [f_pwszSource]    -- Pointer to UTF-16 data buffer.
** [f_cchSource]     -- Length of data buffer in UTF-16 code units
** [f_pw32szTarget]  -- Pointer to result UTF-32 data buffer.
**                      After successful conversion this buffer will be NULL-terminated
**                      If value is NULL then method calculates number of code units
**                      that required for conversion
** [f_pcchTarget]    -- Pointer to DWORD containing UTF-32 buffer length in UTF-32 code
**                      units. After successful conversion it contains length of UTF-32
**                      string including NULL terminator.
**                      In case of DRM_E_BUFFERTOOSMALL error it contains required length
**                      of UTF-16 string including NULL terminator.
**                      In case of error other than DRM_E_BUFFERTOOSMALL it remains unchanged.
**                      This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pw32szTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF32 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF16toUTF32(
    __in_ecount( f_cchSource )          const DRM_WCHAR   *f_pwszSource,
    __in                                const DRM_DWORD    f_cchSource,
    __out_ecount_z_opt( *f_pcchTarget )       DRM_WCHAR32 *f_pw32szTarget,
    __inout                                   DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_STRING  dstrSource = EMPTY_DRM_STRING;
    DRM_DWORD   cchTarget  = 0;

    ChkArg( f_pwszSource != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Prepare parameters for DRM_STR_DSTRtoUTF32 */
    dstrSource.cchString = f_cchSource;
    dstrSource.pwszString = (DRM_WCHAR*) f_pwszSource;
    if ( (*f_pcchTarget) > 0 )
    {
        cchTarget = (*f_pcchTarget) -1 ; /* reserve 1 symbol for NULL-terminator*/
    }

    /* Delegate processing */
    ChkDR( DRM_STR_DSTRtoUTF32( &dstrSource, f_pw32szTarget, &cchTarget ) );

    if ( f_pw32szTarget != NULL )
    {
        ChkBOOL( cchTarget < *f_pcchTarget, DRM_E_BUFFERTOOSMALL );

        f_pw32szTarget[ cchTarget ] = 0; /* set NULL terminator */
    }
    *f_pcchTarget = cchTarget + 1; /* Set length including NULL terminator */

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pcchTarget = cchTarget + 1; /* Set length including NULL terminator */
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_DSTRtoUTF32
**
** Synopsis: Converts UTF-16 DRM string to UTF-32
**
** Arguments:
**
** [f_pdstrSource]   -- Pointer to UTF-16 DRM string.
**                      f_pdstrSource->cchString should contain length of DRM string
**                      in UTF-16 code units
** [f_pw32szTarget]  -- Pointer to result UTF-32 data buffer.
**                      If value is NULL then method calculates length that required
**                      for conversion.
** [f_pcchTarget]    -- Pointer to DWORD containing UTF-32 buffer length in UTF-32 code
**                      units, After successful conversion it contains actual length
**                      of UTF-32 string (not including any NULLs).
**                      In case of DRM_E_BUFFERTOOSMALL error it contains required length
**                      of UTF-16 string (not including any NULLs).
**                      In case of error other than DRM_E_BUFFERTOOSMALL it remains unchanged.
**                      This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pw32szTarget is NULL or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_DSTRtoUTF32(
    __in                              const DRM_STRING  *f_pdstrSource,
    __out_ecount_opt( *f_pcchTarget )       DRM_WCHAR32 *f_pw32szTarget,
    __inout                                 DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT         dr                    = DRM_SUCCESS;
    DRM_CODE_POINT     cp                    = 0;
    DRM_STRING_WINDOW  dstrwSource           = EMPTY_DRM_STRING_WINDOW;
    DRM_WCHAR32       *pw32szCurTargetSymbol = f_pw32szTarget;
    const DRM_WCHAR32 *pw32szTargetEnd       = NULL;

    /* Validate source data */
    ChkArg( f_pdstrSource != NULL );
    ChkArg( f_pdstrSource->pwszString != NULL );

    /* Initialize source substring */
    dstrwSource.m_ichMin = 0;
    dstrwSource.m_ichMaxExclusive = f_pdstrSource->cchString;

    /* Validate target data */
    ChkArg( f_pcchTarget != NULL );
    ChkBOOL( f_pw32szTarget != NULL, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( (*f_pcchTarget) > 0, DRM_E_BUFFERTOOSMALL );

    /* Initialize boundaries for target */
    pw32szTargetEnd = f_pw32szTarget + (*f_pcchTarget);
    ChkOverflow( pw32szTargetEnd, f_pw32szTarget );

    /* Do processing */
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint16( f_pdstrSource->pwszString, &dstrwSource, &cp ) );

        ChkBOOL( pw32szCurTargetSymbol < pw32szTargetEnd, DRM_E_BUFFERTOOSMALL );
        *pw32szCurTargetSymbol++ = UTF32_FROM_CP( cp );
    }

    /* Set the length of converted string */
    *f_pcchTarget = (DRM_DWORD) (pw32szCurTargetSymbol - f_pw32szTarget);

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* Calculate required length */
        dr = _GetUTF32UnitCountForUTF16( f_pdstrSource, f_pcchTarget );
        if( DRM_SUCCEEDED( dr ) )
        {
            /* Need to return error */
            dr = DRM_E_BUFFERTOOSMALL;
        }
    }
    return dr;
}

/*******************************************************************************
** Function: _GetUTF8UnitCountForUTF16
**
** Synopsis: Calculates number of 8-bit code units required to convert
**           UTF-16 string. This function is agnostic as to NULL termination.
**
** Arguments:
**
** [f_pdstrSource]   -- Pointer to UTF-16 DRM string.
** [f_pcchTarget]    -- Pointer to code units counter. This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_UTF_INVALID_CODE - if UTF16 sequence is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _GetUTF8UnitCountForUTF16(
    __in     const DRM_STRING  *f_pdstrSource,
    __inout        DRM_DWORD   *f_pcchTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;
    DRM_CODE_POINT    cp          = 0;
    DRM_DWORD         cchLocal    = 0;
    DRM_DWORD         cchOut      = 0;

    ChkArg( f_pdstrSource != NULL );
    ChkArg( f_pdstrSource->pwszString != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Initialize source substring */
    dstrwSource.m_ichMin = 0;
    dstrwSource.m_ichMaxExclusive = f_pdstrSource->cchString;

    /* Do processing */
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint16( f_pdstrSource->pwszString, &dstrwSource, &cp ) );

        /* How many UTF-8 code units in current code point? */
        ChkDR( _GetUTF8UnitCount( cp, &cchLocal ) );

        /* Check for cchOut overflow */
        ChkOverflow( cchOut + cchLocal, cchOut );
        cchOut += cchLocal;
    }
    *f_pcchTarget = cchOut; /* Set the result */

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF16toUTF8
**
** Synopsis: Converts UTF-16 string to UTF-8
**
** Arguments:
**
** [f_pwszSource]    -- Pointer to UTF-16 data buffer.
** [f_cchSource]     -- Length of data buffer in UTF-16 code units
** [f_pchTarget]     -- Pointer to result UTF-8 data buffer.
**                      After successful conversion this string will be NULL-terminated
**                      If value is NULL then method calculates number of code units
**                      that required for conversion
** [f_ichTarget]     -- Position (offset in 8-bit code points) of UTF-8 symbol to start writing
** [f_pcchTarget]    -- Pointer to DWORD containing UTF-8 buffer length in UTF-8 code
**                      units. After successful conversion it contains length
**                      of UTF-8 string including NULL terminator.
**                      In case of DRM_E_BUFFERTOOSMALL error it contains required length
**                      of UTF-16 string including NULL terminator.
**                      In case of error other than DRM_E_BUFFERTOOSMALL it remains unchanged.
**                      This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pchTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF16toUTF8(
    __in_ecount( f_cchSource )          const DRM_WCHAR *f_pwszSource,
    __in                                const DRM_DWORD  f_cchSource,
    __out_ecount_z_opt( *f_pcchTarget )       DRM_CHAR  *f_pchTarget,
    __in                                const DRM_DWORD  f_ichTarget,
    __inout                                   DRM_DWORD *f_pcchTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_STRING        dstrSource  = EMPTY_DRM_STRING;
    DRM_STRING_WINDOW dstrwTarget = EMPTY_DRM_STRING_WINDOW;

    ChkArg( f_pwszSource != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Prepare parameters for DRM_STR_DSTRtoUTF8 */
    dstrSource.cchString = f_cchSource;
    dstrSource.pwszString = (DRM_WCHAR*) f_pwszSource; /* NULL is handled by DRM_STR_DSTRtoUTF8 */
    dstrwTarget.m_ichMin = f_ichTarget;
    if ( (*f_pcchTarget) > 0 )
    {
        dstrwTarget.m_ichMaxExclusive = (*f_pcchTarget) - 1 ; /* Reserve 1 symbol for NULL terminator */
    }

    /* Delegate the processing */
    ChkDR( DRM_STR_DSTRtoUTF8_ByWindow( &dstrSource, f_pchTarget, &dstrwTarget ) );

    if ( f_pchTarget != NULL )
    {
        ChkBOOL( dstrwTarget.m_ichMaxExclusive < *f_pcchTarget, DRM_E_BUFFERTOOSMALL );

        PUT_BYTE( f_pchTarget, dstrwTarget.m_ichMaxExclusive, 0); /* Set NULL terminator */
    }
    *f_pcchTarget = dstrwTarget.m_ichMaxExclusive + 1; /* Set length including NULL terminator */

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pcchTarget = dstrwTarget.m_ichMaxExclusive + 1 /* Set length including NULL terminator */;
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_DSTRtoUTF8_ByWindow
**
** Synopsis: Converts UTF-16 DRM substring to UTF-8 by using range window
**
** Arguments:
**
** [f_pdstrSource]   -- Pointer to UTF-16 DRM string.
** [f_pchTarget]     -- Pointer to UTF-8 buffer
** [f_pdstrwTarget]  -- Window covering a range of UTF-8 units.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pchTarget is NULL or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STR_DSTRtoUTF8_ByWindow(
    __in                                                  const DRM_STRING        *f_pdstrSource,
    __out_ecount_opt( f_pdstrwTarget->m_ichMaxExclusive )       DRM_CHAR          *f_pchTarget,
    __inout                                                     DRM_STRING_WINDOW *f_pdstrwTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_CODE_POINT    cp          = 0;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;
    DRM_STRING_WINDOW dstrwTarget = EMPTY_DRM_STRING_WINDOW;

    /* Validate source data */
    ChkArg( f_pdstrSource != NULL );
    ChkArg( f_pdstrSource->pwszString != NULL );

    /* Initialize source substring */
    dstrwSource.m_ichMin = 0;
    dstrwSource.m_ichMaxExclusive = f_pdstrSource->cchString;

    /* Validate target data */
    ChkBOOL( f_pchTarget != NULL, DRM_E_BUFFERTOOSMALL );
    ChkArg( f_pdstrwTarget != NULL );
    ChkBOOL( f_pdstrwTarget->m_ichMaxExclusive > 0, DRM_E_BUFFERTOOSMALL );
    ChkArg( f_pdstrwTarget->m_ichMin < f_pdstrwTarget->m_ichMaxExclusive );

    /* Initialize target substrings */
    dstrwTarget.m_ichMin = f_pdstrwTarget->m_ichMin;
    dstrwTarget.m_ichMaxExclusive = f_pdstrwTarget->m_ichMaxExclusive;

    /* Do processing */
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint16( f_pdstrSource->pwszString, &dstrwSource, &cp ) );

        ChkDR( _WriteCodePoint8( cp, f_pchTarget, &dstrwTarget ) );
    }

    /* Set the length of converted string */
    f_pdstrwTarget->m_ichMaxExclusive = dstrwTarget.m_ichMin;

ErrorExit:
    if( dr   == DRM_E_BUFFERTOOSMALL
     && NULL != f_pdstrwTarget )
    {
        /* Calculate required length */
        dr = _GetUTF8UnitCountForUTF16( f_pdstrSource, &(f_pdstrwTarget->m_ichMaxExclusive) );
        if( DRM_SUCCEEDED( dr ) )
        {
            /* Need to return error */
            dr = DRM_E_BUFFERTOOSMALL;
        }
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_DSTRtoUTF8
**
** Synopsis: Converts UTF-16 DRM string to UTF-8
**           This method is wrapper for method DRM_STR_DSTRtoUTF8_ByWindow.
**           It simplifies most useful case of output range window
**           when min position is zero
**
** Arguments:
**
** [f_pdstrSource]      -- Pointer to UTF-16 DRM string.
** [f_pchTarget]        -- Pointer to UTF-8 buffer
** [f_pcchTarget]       -- Size of output UTF-8 buffer in 8-bit code units
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pchTarget is null or
**                   it's size (*f_pcchTarget)is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STR_DSTRtoUTF8(
    __in                              const DRM_STRING *f_pdstrSource,
    __out_ecount_opt( *f_pcchTarget )       DRM_CHAR   *f_pchTarget,
    __inout                                 DRM_DWORD  *f_pcchTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrwTarget = EMPTY_DRM_STRING_WINDOW;

    ChkArg( f_pcchTarget != NULL );

    dstrwTarget.m_ichMaxExclusive = *f_pcchTarget;

    /* Call the main method */
    ChkDR( DRM_STR_DSTRtoUTF8_ByWindow( f_pdstrSource, f_pchTarget, &dstrwTarget ) );

    /* Set the length of converted string */
    *f_pcchTarget = dstrwTarget.m_ichMaxExclusive;

ErrorExit:
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pcchTarget = dstrwTarget.m_ichMaxExclusive;
    }
    return dr;
}

/*******************************************************************************
** Function: _GetUTF16UnitCountForUTF8
**
** Synopsis: Calculates number of 16-bit code units required to convert
**           UTF-8 string. This function is agnostic as to NULL termination.
**
** Arguments:
**
** [f_pchSource]     -- Pointer to UTF-8 data buffer.
** [f_dstrwSource]   -- Window covering a range of UTF-8 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
** [f_pcchTarget]    -- Pointer to code units counter. This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_UTF_INVALID_CODE - if UTF16 sequence is illegal
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _GetUTF16UnitCountForUTF8(
    __in     const DRM_CHAR          *f_pchSource,
    __in     const DRM_STRING_WINDOW *f_dstrwSource,
    __out          DRM_DWORD         *f_pcchTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_CODE_POINT    cp          = 0;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;
    DRM_DWORD         cchLocal    = 0;
    DRM_DWORD         cchOut      = 0;

    ChkArg( f_pchSource != NULL );
    ChkArg( f_dstrwSource != NULL );
    ChkArg( f_dstrwSource->m_ichMin < f_dstrwSource->m_ichMaxExclusive );
    ChkArg( f_pcchTarget != NULL );

    dstrwSource.m_ichMin = f_dstrwSource->m_ichMin;
    dstrwSource.m_ichMaxExclusive = f_dstrwSource->m_ichMaxExclusive;
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint8( f_pchSource, &dstrwSource, &cp ) );

        /* How many UTF-16 code units in current code point? */
        ChkDR( _GetUTF16UnitCount( cp, &cchLocal ) );
        cchOut += cchLocal;
    }
    *f_pcchTarget = cchOut; /* Set the result */

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF8toUTF16
**
** Synopsis: Converts UTF-8 string to UTF-16
**
** Arguments:
**
** [f_pchSource]     -- Pointer to UTF-8 data buffer.
** [f_ichSource]     -- Position (offset in 8-bit code points) of UTF-8 symbol to start reading
** [f_cchSource]     -- Length of 8 data buffer in 8-bit code points
** [f_pchTarget]     -- Pointer to UTF-16 data buffer.
**                      After successful conversion this buffer will be NULL-terminated
**                      If value is NULL then method calculates number of code units
**                      that required for conversion
** [f_pcchTarget]    -- Pointer to DWORD containing UTF-16 buffer size in 16-bit code points
**                      After successful conversion it contains length of UTF-16 string
**                      including NULL terminator.
**                      In case of DRM_E_BUFFERTOOSMALL error it contains required length
**                      of UTF-16 string including NULL terminator.
**                      In case of error other than DRM_E_BUFFERTOOSMALL it remains unchanged.
**                      This value cannot be NULL.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pchTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-16 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF8toUTF16(
    __in_ecount( f_ichSource + f_cchSource ) const DRM_CHAR  *f_pchSource,
    __in                                     const DRM_DWORD  f_ichSource,
    __in                                     const DRM_DWORD  f_cchSource,
    __out_ecount_z_opt( *f_pcchTarget )            DRM_WCHAR *f_pchTarget,
    __inout                                        DRM_DWORD *f_pcchTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;
    DRM_STRING        dstrTarget  = EMPTY_DRM_STRING;

    ChkArg( f_pchSource != NULL );
    ChkArg( f_pcchTarget != NULL );

    /* Prepare parameters for DRM_STR_UTF8toDSTR */
    dstrwSource.m_ichMin = f_ichSource;
    dstrwSource.m_ichMaxExclusive = f_cchSource;
    dstrTarget.pwszString = f_pchTarget; /* NULL is handled by DRM_STR_UTF8toDSTR */
    if ( (*f_pcchTarget) > 0 )
    {
        dstrTarget.cchString = (*f_pcchTarget) - 1 ; /* Reserve 1 symbol for NULL terminator */
    }

    /* Delegate processing */
    ChkDR( DRM_STR_UTF8toDSTR_ByWindow( f_pchSource, &dstrwSource, &dstrTarget ) );

    if ( f_pchTarget != NULL )
    {
        f_pchTarget[dstrTarget.cchString] = 0; /* Set NULL terminator */
    }
    *f_pcchTarget = dstrTarget.cchString + 1;   /* Set length including NULL terminator */

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pcchTarget = dstrTarget.cchString + 1; /* Set length including NULL terminator */
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF8toDSTR_ByWindow
**
** Synopsis: Converts UTF-8 string to UTF-16 DRM string by using window range
**
** Arguments:
**
** [f_pchSource]     -- Pointer to UTF-8 data buffer.
** [f_pdstrwSource]  -- Window covering a range of UTF-8 units.
** [f_pdstrTarget]   -- Pointer to UTF-16 DRM string. If pointer to string itself is NULL
**                      then method return length (in 16-bit code units) that required
**                      for conversion (not including any NULLs).
**                      On error other than DRM_E_BUFFERTOOSMALL this length is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pdstrTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-8 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF8toDSTR_ByWindow(
    __in_ecount( f_pdstrwSource->m_ichMaxExclusive ) const DRM_CHAR          *f_pchSource,
    __in                                             const DRM_STRING_WINDOW *f_pdstrwSource,
    __inout                                                DRM_STRING        *f_pdstrTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_CODE_POINT    cp          = 0;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;
    DRM_STRING_WINDOW dstrwTarget = EMPTY_DRM_STRING_WINDOW;

    /* Validate source data */
    ChkArg( f_pchSource != NULL );
    ChkArg( f_pdstrwSource != NULL );
    ChkArg( f_pdstrwSource->m_ichMin < f_pdstrwSource->m_ichMaxExclusive );

    /* Initialize source substring */
    dstrwSource.m_ichMin = f_pdstrwSource->m_ichMin;
    dstrwSource.m_ichMaxExclusive = f_pdstrwSource->m_ichMaxExclusive;

    /* Validate target data */
    ChkArg( f_pdstrTarget != NULL );
    ChkBOOL( f_pdstrTarget->pwszString != NULL, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( f_pdstrTarget->cchString > 0, DRM_E_BUFFERTOOSMALL );

    /* Initialize target substring */
    dstrwTarget.m_ichMin = 0;
    dstrwTarget.m_ichMaxExclusive = f_pdstrTarget->cchString;

    /* Do processing */
    while( dstrwSource.m_ichMin < dstrwSource.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint8( f_pchSource, &dstrwSource, &cp ) );

        ChkDR( _WriteCodePoint16( cp, f_pdstrTarget->pwszString, &dstrwTarget ) );
    }

    /* Set the length of converted string */
    f_pdstrTarget->cchString = dstrwTarget.m_ichMin;

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* Calculate required length */
        dr = _GetUTF16UnitCountForUTF8( f_pchSource, &dstrwSource, &(f_pdstrTarget->cchString) );
        if( DRM_SUCCEEDED( dr ) )
        {
            /* Need to return error */
            dr = DRM_E_BUFFERTOOSMALL;
        }
    }
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_UTF8toDSTR
**
** Synopsis: Converts UTF-8 string to UTF-16 DRM string
**           This method is wrapper for method DRM_STR_UTF8toDSTR_ByWindow.
**           It simplifies most useful case of input range window
**           when min position is zero
**
** Arguments:
**
** [f_pchSource]       -- Pointer to UTF-8 data buffer.
** [f_cchSource]       -- Size of UTF-8 data buffer in 8-bit code units
** [f_pdstrTarget]     -- Pointer to UTF-16 DRM string. If pointer to string itself is NULL
**                        then method return length (in 16-bit code units) that required
**                        for conversion (not including any NULLs).
**                        On error other than DRM_E_BUFFERTOOSMALL this length is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if f_pdstrTarget is null or it's size
**                   is insufficient for conversion
**                   DRM_E_UTF_INVALID_CODE - if UTF-8 sequence is illegal
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF8toDSTR(
    __in_ecount( f_cchSource ) const DRM_CHAR   *f_pchSource,
    __in                       const DRM_DWORD   f_cchSource,
    __inout                          DRM_STRING *f_pdstrTarget )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrwSource = EMPTY_DRM_STRING_WINDOW;

    ChkArg( f_pchSource != NULL &&
            f_cchSource > 0 &&
            f_pdstrTarget != NULL );

    dstrwSource.m_ichMaxExclusive = f_cchSource;

    /* Call the main method */
    ChkDR( DRM_STR_UTF8toDSTR_ByWindow( f_pchSource, &dstrwSource, f_pdstrTarget ) );

ErrorExit:

    return dr;
}

/*******************************************************************************
** Function: _ASCIItoUTF16
**
** Synopsis: Converts ASCII string to UTF-16 string
**
** Arguments:
**
** [f_pchSource]         -- Pointer to ASCII data buffer.
** [f_ichSource]         -- Start position of ASCII string
** [f_cchSource]         -- Number of characters, starting at f_pchSource + f_ichSource
** [f_pwchTarget]        -- Pointer to UTF-16 string.
** [f_pcchTarget]        -- Pointer to UTF-16 string length.
**                          Initially f_pcchTarget contains length of UTF-16 data buffer
**                          in characters. It should not be less than length of source string.
**                          When function is successful then f_pcchTarget contains length of
**                          converted string. On error this length is unchanged.
** Note:             Conversion can be done in-place (i.e. f_pchSource and f_pwchTarget
**                   point to the same address ) when size of target buffer allows
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if target's size is insufficient for conversion
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _ASCIItoUTF16(
    __in_ecount( f_ichSource + f_cchSource ) const DRM_CHAR  *f_pchSource,
    __in                                     const DRM_DWORD  f_ichSource,
    __in                                     const DRM_DWORD  f_cchSource,
    __out_ecount( *f_pcchTarget )                  DRM_WCHAR *f_pwchTarget,
    __inout                                        DRM_DWORD *f_pcchTarget )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  ich = 0;

    ChkArg( f_pchSource != NULL );
    ChkArg( f_pwchTarget != NULL );
    ChkArg( f_pcchTarget != NULL );

    ChkBOOL( f_cchSource <= *f_pcchTarget, DRM_E_BUFFERTOOSMALL );

    ich = f_cchSource;
    while( ich > 0 )
    {
        ich--;
        f_pwchTarget[ ich ] = WCHAR_CAST( GET_CHAR( f_pchSource, f_ichSource + ich ) );
    }
    *f_pcchTarget = f_cchSource;

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: _ASCIItoUTF16_NULL
**
** Synopsis: Converts ASCII string to UTF-16 string
**           This method is the same as _ASCIItoUTF16 except it adds NULL terminator
**
** Arguments:
**
** [f_pchSource]         -- Pointer to ASCII data buffer.
** [f_ichSource]         -- Start position of ASCII string
** [f_cchSource]         -- Number of characters, starting at f_pchSource + f_ichSource
** [f_pwchTarget]        -- Pointer to UTF-16 string.
** [f_pcchTarget]        -- Pointer to UTF-16 string length (including NULL terminator).
**                          Initially f_pcchTarget contains length of UTF-16 data buffer
**                          in characters. It should not be less than length of source string.
**                          When function is successful then f_pcchTarget contains length of
**                          converted string. On error this length is unchanged.
** Note:             Conversion can be done in-place (i.e. f_pchSource and f_pwchTarget
**                   point to the same address ) when size of target buffer allows
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if target's size is insufficient for conversion
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _ASCIItoUTF16_NULL(
    __in_ecount( f_ichSource + f_cchSource ) const DRM_CHAR  *f_pchSource,
    __in                                     const DRM_DWORD  f_ichSource,
    __in                                     const DRM_DWORD  f_cchSource,
    __out_ecount( *f_pcchTarget )                  DRM_WCHAR *f_pwchTarget,
    __inout                                        DRM_DWORD *f_pcchTarget )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cchTarget = 0;
    ChkArg( f_pchSource != NULL && f_cchSource > 0 );
    ChkArg( f_pcchTarget != NULL && *f_pcchTarget != 0 );

    ChkBOOL( f_cchSource < *f_pcchTarget, DRM_E_BUFFERTOOSMALL );
    cchTarget = (*f_pcchTarget) - 1; /* Reserve space for NULL terminator */

    ChkDR( _ASCIItoUTF16( f_pchSource, f_ichSource, f_cchSource, f_pwchTarget, &cchTarget ) );

    /*
    ** This ChkBOOL is required for PreFast only. It will not trigger because
    ** there is buffer validation above and _ASCIItoUTF16 can only reduce cchTarget
    */
    ChkBOOL( cchTarget < *f_pcchTarget, DRM_E_FAIL );

    f_pwchTarget[ cchTarget ] = g_wchNull; /* set NULL terminator */
    *f_pcchTarget = cchTarget + 1; /* set length */

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function: DRM_STR_ASCII_SUBSTRtoDSTR
**
** Synopsis: Converts ASCII string to UTF-16 DRM string
**
** Arguments:
**
** [f_pchSource]     -- Pointer to ASCII data buffer.
** [f_pdasstr]       -- Substring of ASCII data buffer representing string to process
** [f_pdstrTarget]   -- Pointer to UTF-16 DRM string.
**                      Initally f_pdstrTarget->cchString contains size of
**                      f_pdstrTarget->pwszString in characters and this value
**                      should not be less than length of source string.
**                      When function is successful then cchString contains length of
**                      converted string. On error this length is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if target's size is insufficient for conversion
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_ASCII_SUBSTRtoDSTR(
    __in_ecount( f_pdasstr->m_ich + f_pdasstr->m_cch ) const DRM_CHAR      *f_pchSource,
    __in                                               const DRM_SUBSTRING *f_pdasstr,
    __inout                                                  DRM_STRING    *f_pdstrTarget )
{
   DRM_RESULT dr = DRM_SUCCESS;

   ChkArg( f_pdasstr != NULL );
   ChkDRMString( f_pdstrTarget );

   ChkDR( _ASCIItoUTF16( f_pchSource,
                         f_pdasstr->m_ich,
                         f_pdasstr->m_cch,
                         f_pdstrTarget->pwszString,
                        &f_pdstrTarget->cchString ) );

ErrorExit:
   return dr;
}


/*******************************************************************************
** Function: DRM_STR_ASCIItoDSTR
**
** Synopsis: Converts ASCII string to UTF-16 DRM string
**           This method is wrapper for method _ASCIItoUTF16.
**           It simplifies most useful case of when start position
**           of ASCII string is zero
**
** Arguments:
**
** [f_pchSource]     -- Pointer to ASCII data buffer.
** [f_cchSource]     -- Length of ASCII string. Start position is 0.
** [f_pdstrTarget]   -- Pointer to UTF-16 DRM string. Length of string (in cchString)
**                      should not be less than length of source string.
**                      When function is successful then cchString contains length of
**                      converted string. On error this length is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if target's size is insufficient for conversion
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_ASCIItoDSTR(
    __in_ecount( f_cchSource ) const DRM_CHAR   *f_pchSource,
    __in                       const DRM_DWORD   f_cchSource,
    __inout                          DRM_STRING *f_pdstrTarget )
{
   return _ASCIItoUTF16( f_pchSource,
                         0,
                         f_cchSource,
                         f_pdstrTarget->pwszString,
                        &f_pdstrTarget->cchString );
}

/*******************************************************************************
** Function: DRM_STR_UTF16toASCII
**
** Synopsis: Converts UTF-16 DRM string to ASCII string
**           This method assumes that all UTF-16 characters are in ASCII range
**           This method forces NULL termination for target ASCII string
**
** Arguments:
**
** [f_pwszSource]    -- Pointer to UTF-16 string.
** [f_cchSource]     -- Length of UTF-16 string in characters
** [f_pszTarget]     -- Pointer to ASCII string.
** [f_cchSource]     -- Size of ASCII string buffer in characters
**                      This length should be greater than length of source string.
** Note:             Conversion can be done in-place, i.e. when
**                   f_pwszSource and f_pszTarget point to the same address
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid.
**                   DRM_E_BUFFERTOOSMALL if target's size is insufficient for conversion
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_UTF16toASCII(
    __in_ecount(f_cchSource)    const DRM_WCHAR *f_pwszSource,
    __in                        const DRM_DWORD  f_cchSource,
    __inout_ecount(f_cchTarget)       DRM_CHAR  *f_pszTarget,
    __in                        const DRM_DWORD  f_cchTarget )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  ich = 0;

    ChkArg( f_pwszSource != NULL );
    ChkArg( f_pszTarget != NULL );

    /* Reserve 1 space for NULL terminator */
    ChkBOOL( f_cchSource < f_cchTarget, DRM_E_BUFFERTOOSMALL );

    while( ich < f_cchSource )
    {
        PUT_BYTE( f_pszTarget, ich, (DRM_CHAR)NATIVE_WCHAR( f_pwszSource[ ich ] ) );
        ich++;
    }
    PUT_BYTE( f_pszTarget, ich, 0 ); /* Put NULL terminator */

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

