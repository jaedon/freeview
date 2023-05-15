/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#define DRM_BUILDING_DRMMATHSAFE_C

#include <drmmathsafe.h>
#include <drmresults.h>
#include <drmdebug.h>
#include <drmerr.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* BYTE -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ByteToChar(
    __in                      DRM_BYTE        f_bOperand,
    __deref_out               DRM_CHAR       *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( f_bOperand <= DRM_CHAR_MAX ) )
    {
        *f_pchResult = (DRM_CHAR)f_bOperand;
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* SHORT -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToChar(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out               DRM_CHAR       *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( ( f_sOperand >= DRM_CHAR_MIN ) && ( f_sOperand <= DRM_CHAR_MAX ) ) )
    {
        *f_pchResult = (DRM_CHAR)f_sOperand;
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* SHORT -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToByte(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out_ecount(1)     DRM_BYTE       *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( ( f_sOperand >= 0 ) && ( f_sOperand <= MAX_UNSIGNED_TYPE(DRM_BYTE) ) ) )
    {
        *f_pbResult = (DRM_BYTE)f_sOperand;
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* SHORT -> WORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToWord(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out               DRM_WORD       *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( f_sOperand >= 0 ) )
    {
        *f_pwResult = (DRM_WORD)f_sOperand;
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* SHORT -> DWORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToDWord(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out               DRM_DWORD      *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_sOperand >= 0 ) )
    {
        *f_pdwResult = (DRM_DWORD)f_sOperand;
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* SHORT -> UINT64 conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToUInt64(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out               DRM_UINT64     *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( f_sOperand >= 0 ) )
    {
        *f_pui64Result = DRM_UI64HL(0, f_sOperand);
    }
    else
    {
        *f_pui64Result = DRM_UINT64_ERROR;
         dr            = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* SHORT -> DWORD_PTR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortToDWordPtr(
    __in                      DRM_SHORT       f_sOperand,
    __deref_out               DRM_DWORD_PTR  *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_sOperand >= 0 ) )
    {
        *f_pdwResult = (DRM_DWORD_PTR)f_sOperand;
    }
    else
    {
        *f_pdwResult = DRM_DWORD_PTR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* WORD -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordToChar(
    __in                      DRM_WORD       f_wOperand,
    __deref_out               DRM_CHAR      *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( f_wOperand <= DRM_CHAR_MAX ) )
    {
        *f_pchResult = (DRM_CHAR)f_wOperand;
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* WORD -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordToByte(
    __in                      DRM_WORD       f_wOperand,
    __deref_out_ecount(1)     DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_wOperand <= MAX_UNSIGNED_TYPE(DRM_BYTE) ) )
    {
        *f_pbResult = (DRM_BYTE)f_wOperand;
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* WORD -> SHORT conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordToShort(
    __in                      DRM_WORD       f_wOperand,
    __deref_out               DRM_SHORT     *f_psResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_psResult != NULL );
    __analysis_assume( f_psResult != NULL );

    if( DRM_LIKELY( f_wOperand <= DRM_SHORT_MAX ) )
    {
        *f_psResult = (DRM_SHORT)f_wOperand;
    }
    else
    {
        *f_psResult = DRM_SHORT_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToChar(
    __in                      DRM_LONG       f_lOperand,
    __deref_out               DRM_CHAR      *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( ( f_lOperand >= DRM_CHAR_MIN ) && ( f_lOperand <= DRM_CHAR_MAX ) ) )
    {
        *f_pchResult = (DRM_CHAR)f_lOperand;
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToByte(
    __in                      DRM_LONG       f_lOperand,
    __deref_out_ecount(1)     DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( ( f_lOperand >= 0 ) && ( f_lOperand <= MAX_UNSIGNED_TYPE(DRM_BYTE) ) ) )
    {
        *f_pbResult = (DRM_BYTE)f_lOperand;
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> SHORT conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToShort(
    __in                      DRM_LONG       f_lOperand,
    __deref_out               DRM_SHORT     *f_psResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_psResult != NULL );
    __analysis_assume( f_psResult != NULL );

    if( DRM_LIKELY( ( f_lOperand >= DRM_SHORT_MIN ) && ( f_lOperand <= DRM_SHORT_MAX ) ) )
    {
       *f_psResult = (DRM_SHORT)f_lOperand;
    }
    else
    {
        *f_psResult = DRM_SHORT_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> WORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToWord(
    __in                      DRM_LONG       f_lOperand,
    __deref_out               DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( ( f_lOperand >= 0 ) && ( f_lOperand <= MAX_UNSIGNED_TYPE(DRM_WORD) ) ) )
    {
        *f_pwResult = (DRM_WORD)f_lOperand;
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}


/* LONG -> DWORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToDWord(
    __in                      DRM_LONG       f_lOperand,
    __deref_out               DRM_DWORD     *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_lOperand >= 0 ) )
    {
        *f_pdwResult = (DRM_DWORD)f_lOperand;
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> DWORD_PTR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongToDWordPtr(
    __in                      DRM_LONG       f_lOperand,
    __deref_out               DRM_DWORD_PTR *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_lOperand >= 0 ) )
    {
        *f_pdwResult = (DRM_DWORD_PTR)f_lOperand;
    }
    else
    {
        *f_pdwResult = DRM_DWORD_PTR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordToChar(
    __in                      DRM_DWORD     f_dwOperand,
    __deref_out               DRM_CHAR     *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_CHAR_MAX ) )
    {
        *f_pchResult = (DRM_CHAR)f_dwOperand;
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordToByte(
    __in                      DRM_DWORD     f_dwOperand,
    __deref_out_ecount(1)     DRM_BYTE     *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= MAX_UNSIGNED_TYPE(DRM_BYTE) ) )
    {
        *f_pbResult = (DRM_BYTE)f_dwOperand;
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}


/* DWORD -> SHORT conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordToShort(
    __in                      DRM_DWORD     f_dwOperand,
    __deref_out               DRM_SHORT    *f_psResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_psResult != NULL );
    __analysis_assume( f_psResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_SHORT_MAX ) )
    {
        *f_psResult = (DRM_SHORT)f_dwOperand;
    }
    else
    {
        *f_psResult = DRM_SHORT_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD -> WORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordToWord(
    __in                      DRM_DWORD     f_dwOperand,
    __deref_out               DRM_WORD     *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= MAX_UNSIGNED_TYPE(DRM_WORD) ) )
    {
        *f_pwResult = (DRM_WORD)f_dwOperand;
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* DWORD -> LONG conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordToLong(
    __in                      DRM_DWORD     f_dwOperand,
    __deref_out               DRM_LONG     *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_LONG_MAX ) )
    {
        *f_plResult = (DRM_LONG)f_dwOperand;
    }
    else
    {
        *f_plResult = DRM_LONG_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD_PTR -> DWORD conversion */
#ifndef DRM_64BIT_TARGET
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordPtrToDWord(
    __in                      DRM_DWORD_PTR  f_dwOperand,
    __deref_out               DRM_DWORD     *f_pdwResult )
{
    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    *f_pdwResult = (DRM_DWORD)f_dwOperand;
    return DRM_SUCCESS;
}
#endif /* DRM_64BIT_TARGET */

/* INT64 -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToChar(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_CHAR     *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64( DRM_CHAR_MIN ) )
                    && !DRM_I64Les( DRM_I64( DRM_CHAR_MAX ), f_i64Operand ) ) )
    {
        *f_pchResult = (DRM_CHAR)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> WORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToWord(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_WORD     *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                    && !DRM_I64Les( DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_WORD) ), f_i64Operand ) ) )
    {
        *f_pwResult = (DRM_WORD)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToByte(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out_ecount(1)     DRM_BYTE     *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                    && !DRM_I64Les( DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_BYTE) ), f_i64Operand ) ) )
    {
        *f_pbResult = (DRM_BYTE)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> SHORT conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToShort(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_SHORT    *f_psResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_psResult != NULL );
    __analysis_assume( f_psResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64( DRM_SHORT_MIN ) )
                    && !DRM_I64Les( DRM_I64( DRM_SHORT_MAX ), f_i64Operand ) ) )
    {
        *f_psResult = (DRM_SHORT)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        *f_psResult = DRM_SHORT_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> LONG conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToLong(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_LONG     *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64( DRM_LONG_MIN ) )
                    && !DRM_I64Les( DRM_I64( DRM_LONG_MAX ), f_i64Operand ) ) )
    {
        *f_plResult = (DRM_LONG)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        *f_plResult = DRM_LONG_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> DWORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToDWord(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_DWORD    *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY(    !DRM_I64Les( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                    && !DRM_I64Les( DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) ), f_i64Operand ) ) )
    {
        *f_pdwResult = (DRM_DWORD)DRM_I64ToUI32( f_i64Operand );
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> UINT64 conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64ToUInt64(
    __in                      DRM_INT64     f_i64Operand,
    __deref_out               DRM_UINT64   *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( !DRM_I64Les( f_i64Operand, DRM_I64Asgn( 0, 0 ) ) ) )
    {
        *f_pui64Result = DRM_I2UI64( f_i64Operand );
    }
    else
    {
        *f_pui64Result = DRM_UINT64_ERROR;
         dr            = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> CHAR conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToChar(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_CHAR      *f_pchResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pchResult != NULL );
    __analysis_assume( f_pchResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, DRM_CHAR_MAX ), f_ui64Operand ) ) )
    {
        *f_pchResult = (DRM_CHAR)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_pchResult = DRM_CHAR_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}


/* UINT64 -> WORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToWord(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_WORD) ), f_ui64Operand ) ) )
    {
        *f_pwResult = (DRM_WORD)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> BYTE conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToByte(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out_ecount(1)     DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_BYTE) ), f_ui64Operand ) ) )
    {
        *f_pbResult = (DRM_BYTE)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> SHORT conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToShort(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_SHORT     *f_psResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_psResult != NULL );
    __analysis_assume( f_psResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, DRM_SHORT_MAX ), f_ui64Operand ) ) )
    {
        *f_psResult = (DRM_SHORT)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_psResult = DRM_SHORT_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*  UINT64 -> LONG conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToLong(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_LONG      *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, DRM_LONG_MAX ), f_ui64Operand ) ) )
    {
        *f_plResult = (DRM_LONG)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_plResult = DRM_LONG_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> DWORD conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToDWord(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_DWORD     *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) ), f_ui64Operand ) ) )
    {
        *f_pdwResult = (DRM_DWORD)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> DWORD_PTR conversion */
#ifdef DRM_64BIT_TARGET
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToDWordPtr(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_DWORD_PTR *f_pdwResult )
{
    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    /* DWordPtr is defined to be DRM_UInt64 */
    *f_pdwResult = (DRM_DWORD_PTR)f_ui64Operand;
    return DRM_SUCCESS;
}
#endif /* DRM_64BIT_TARGET */

/* UINT64 -> INT64 conversion */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64ToInt64(
    __in                      DRM_UINT64     f_ui64Operand,
    __deref_out               DRM_INT64     *f_pi64Result )
{
    DRM_RESULT dr        = DRM_SUCCESS;

    DRMASSERT( f_pi64Result != NULL );
    __analysis_assume( f_pi64Result != NULL );

    if( DRM_LIKELY( DRM_UI64High32( f_ui64Operand ) <= DRM_LONG_MAX ) )
    {
        *f_pi64Result = DRM_UI2I64( f_ui64Operand );
    }
    else
    {
        *f_pi64Result = DRM_INT64_ERROR;
         dr           = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*
** Addition functions
**
*/

/* BYTE addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ByteAdd(
    __in                      DRM_BYTE       f_bAugend,
    __in                      DRM_BYTE       f_bAddend,
    __deref_out_ecount(1)     DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BYTE   bResult = (DRM_BYTE)(f_bAugend + f_bAddend);

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( bResult >= f_bAugend ) )
    {
        *f_pbResult = bResult;
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* WORD addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordAdd(
    __in                      DRM_WORD       f_wAugend,
    __in                      DRM_WORD       f_wAddend,
    __deref_out               DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_WORD   wResult = (DRM_WORD)(f_wAugend + f_wAddend);

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( wResult >= f_wAugend ) )
    {
        *f_pwResult = wResult;
    }
    else
    {
        *f_pwResult = DRM_WORD_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* DWORD addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordAdd(
    __in                      DRM_DWORD       f_dwAugend,
    __in                      DRM_DWORD       f_dwAddend,
    __deref_out               DRM_DWORD      *f_pdwResult )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwResult = f_dwAugend + f_dwAddend;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( dwResult >= f_dwAugend ) )
    {
        *f_pdwResult = dwResult;
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64Add(
    __in                      DRM_UINT64       f_ui64Augend,
    __in                      DRM_UINT64       f_ui64Addend,
    __deref_out               DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_UINT64 ui64Result = DRM_UI64Add( f_ui64Addend, f_ui64Augend );

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( ui64Result, f_ui64Augend ) ) )
    {
        *f_pui64Result = ui64Result;
    }
    else
    {
        *f_pui64Result = DRM_UINT64_ERROR;
         dr            = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*
** Subtraction functions
*/

/*BYTE subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ByteSub(
    __in                      DRM_BYTE       f_bMinuend,
    __in                      DRM_BYTE       f_bSubtrahend,
    __deref_out               DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_bMinuend >= f_bSubtrahend ) )
    {
        *f_pbResult = (DRM_BYTE)(f_bMinuend - f_bSubtrahend);
    }
    else
    {
        *f_pbResult = DRM_BYTE_ERROR;
         dr         = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}


/* WORD subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordSub(
    __in                      DRM_WORD       f_wMinuend,
    __in                      DRM_WORD       f_wSubtrahend,
    __deref_out               DRM_WORD      *f_wResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_wResult != NULL );
    __analysis_assume( f_wResult != NULL );

    if( DRM_LIKELY( f_wMinuend >= f_wSubtrahend ) )
    {
        *f_wResult = (DRM_WORD)(f_wMinuend - f_wSubtrahend);
    }
    else
    {
        *f_wResult = DRM_WORD_ERROR;
         dr        = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordSub(
    __in                      DRM_DWORD       f_dwMinuend,
    __in                      DRM_DWORD       f_dwSubtrahend,
    __deref_out               DRM_DWORD      *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_dwMinuend >= f_dwSubtrahend ) )
    {
        *f_pdwResult = (f_dwMinuend - f_dwSubtrahend);
    }
    else
    {
        *f_pdwResult = DRM_DWORD_ERROR;
         dr          = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UInt64Sub(
    __in            DRM_UINT64       f_ui64Minuend,
    __in            DRM_UINT64       f_ui64Subtrahend,
    __deref_out     DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( !DRM_UI64Les( f_ui64Minuend, f_ui64Subtrahend ) ) )
    {
        *f_pui64Result = DRM_UI64Sub( f_ui64Minuend, f_ui64Subtrahend );
    }
    else
    {
        *f_pui64Result = DRM_UINT64_ERROR;
         dr            = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*
** Multiplication functions
*/

/* WORD multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WordMult(
    __in                      DRM_WORD       f_wMultiplicand,
    __in                      DRM_WORD       f_wMultiplier,
    __deref_out               DRM_WORD      *f_pwResult )
{
    DRM_DWORD dwResult = ((DRM_DWORD)f_wMultiplicand) * ((DRM_DWORD)f_wMultiplier);

    return DRM_DWordToWord(dwResult, f_pwResult);
}


/* DWORD multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DWordMult(
    __in                      DRM_DWORD       f_dwMultiplicand,
    __in                      DRM_DWORD       f_dwMultiplier,
    __deref_out               DRM_DWORD      *f_pdwResult )
{
    DRM_UINT64 ui64Result = DRM_UInt32x32To64( f_dwMultiplicand, f_dwMultiplier );

    return DRM_UInt64ToDWord(ui64Result, f_pdwResult);
}

/* UINT64 multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_UInt64Mult(
    __in                      DRM_UINT64       f_ui64Multiplicand,
    __in                      DRM_UINT64       f_ui64Multiplier,
    __deref_out               DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** 64x64 into 128 is like 32.32 x 32.32.
    **
    ** a.b * c.d = a*(c.d) + .b*(c.d) = a*c + a*.d + .b*c + .b*.d
    ** back in non-decimal notation where A=a*2^32 and C=c*2^32:
    ** A*C + A*d + b*C + b*d
    ** So there are four components to add together.
    **   result = (a*c*2^64) + (a*d*2^32) + (b*c*2^32) + (b*d)
    **
    ** a * c must be 0 or there would be bits in the high 64-bits
    ** a * d must be less than 2^32 or there would be bits in the high 64-bits
    ** b * c must be less than 2^32 or there would be bits in the high 64-bits
    ** then there must be no overflow of the resulting values summed up.
    */

    DRM_DWORD  dw_a       = 0;
    DRM_DWORD  dw_b       = 0;
    DRM_DWORD  dw_c       = 0;
    DRM_DWORD  dw_d       = 0;
    DRM_UINT64 ad         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 bc         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 bd         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64Result = DRM_UI64LITERAL( 0, 0 );

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    dr   = DRM_E_ARITHMETIC_OVERFLOW;

    dw_a = DRM_UI64High32( f_ui64Multiplicand );
    dw_c = DRM_UI64High32( f_ui64Multiplier );

    /* common case -- if high dwords are both zero, no chance for overflow */
    if( DRM_LIKELY( (dw_a == 0) && (dw_c == 0) ) )
    {
        dw_b = DRM_UI64Low32( f_ui64Multiplicand );
        dw_d = DRM_UI64Low32( f_ui64Multiplier );

        *f_pui64Result = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_d ) );
        dr = DRM_SUCCESS;
    }
    else
    {
        /* a * c must be 0 or there would be bits set in the high 64-bits */
        if(    (dw_a == 0)
            || (dw_c == 0) )
        {
            dw_d = DRM_UI64Low32( f_ui64Multiplier );

            /* a * d must be less than 2^32 or there would be bits set in the high 64-bits */
            ad = DRM_UI64Mul( DRM_UI64HL( 0, dw_a ), DRM_UI64HL( 0, dw_d ) );

            if( DRM_UI64High32( ad ) == 0 )
            {
                dw_b = DRM_UI64Low32( f_ui64Multiplicand );

                /* b * c must be less than 2^32 or there would be bits set in the high 64-bits */
                bc = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_c ) );

                if( DRM_UI64High32( bc ) == 0 )
                {
                    /*
                    ** now sum them all up checking for overflow.
                    ** shifting is safe because we already checked for overflow above
                    */
                    if( DRM_SUCCEEDED( DRM_UInt64Add( DRM_UI64ShL(bc, 32),
                                                      DRM_UI64ShL(ad, 32), &ui64Result ) ) )
                    {
                        /* b * d */
                        bd = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_d ) );

                        if( DRM_SUCCEEDED( DRM_UInt64Add( ui64Result, bd, &ui64Result ) ) )
                        {
                             dr = DRM_SUCCESS;
                            *f_pui64Result = ui64Result;
                        }
                    }
                }
            }
        }
    }

    if( DRM_FAILED( dr ) )
    {
        *f_pui64Result = DRM_UINT64_ERROR;
    }

    return dr;
}


/*
**
** signed operations
**
** Strongly consider using unsigned numbers.
**
** Signed numbers are often used where unsigned numbers should be used.
** For example file sizes and array indices should always be unsigned.
** (File sizes should be 64bit integers; array indices should be SIZE_T.)
** Subtracting a larger positive signed number from a smaller positive
** signed number with IntSubwill succeed, producing a negative number,
** that then must not be used as an array index (but can occasionally be
** used as a pointer index.) Similarly for adding a larger magnitude
** negative number to a smaller magnitude positive number.
**
** drmmathsafe.h does not protect you from such errors. It tells you if your
** integer operations overflowed, not if you are doing the right thing
** with your non-overflowed integers.
**
** Likewise you can overflow a buffer with a non-overflowed unsigned index.
*/

/*
** Signed addition functions
*/

/* SHORT Addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortAdd(
    __in                      DRM_SHORT       f_sAugend,
    __in                      DRM_SHORT       f_sAddend,
    __deref_out               DRM_SHORT      *f_psResult )
{
    DRMCASSERT( SIZEOF(DRM_LONG) > SIZEOF(DRM_SHORT) );
    return DRM_LongToShort( ((DRM_LONG)f_sAugend) + ((DRM_LONG)f_sAddend), f_psResult );
}

/* LONG Addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongAdd(
    __in                      DRM_LONG       f_lAugend,
    __in                      DRM_LONG       f_lAddend,
    __deref_out               DRM_LONG      *f_plResult )
{
    DRMCASSERT( SIZEOF(DRM_INT64) > SIZEOF(DRM_LONG) );
    return DRM_Int64ToLong( DRM_I64Add( DRM_I64( f_lAugend ), DRM_I64( f_lAddend ) ), f_plResult );
}

/* INT64 Addition */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64Add(
    __in                      DRM_INT64       f_i64Augend,
    __in                      DRM_INT64       f_i64Addend,
    __deref_out               DRM_INT64      *f_pi64Result )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_INT64  i64Result = DRM_I64Add( f_i64Augend, f_i64Addend );

    DRMASSERT( f_pi64Result != NULL );
    __analysis_assume( f_pi64Result != NULL );

    /*
    ** Adding positive to negative never overflows.
    ** If you add two positive numbers, you expect a positive result.
    ** If you add two negative numbers, you expect a negative result.
    ** Overflow if inputs are the same sign and output is not that sign.
    */
    if( DRM_UNLIKELY(    ( DRM_I64Les( f_i64Augend, DRM_I64Asgn( 0, 0 ) ) == DRM_I64Les( f_i64Addend, DRM_I64Asgn( 0, 0 ) ) )
                      && ( DRM_I64Les( f_i64Augend, DRM_I64Asgn( 0, 0 ) ) != DRM_I64Les( i64Result, DRM_I64Asgn( 0, 0 ) ) ) ) )
    {
        *f_pi64Result = DRM_INT64_ERROR;
         dr           = DRM_E_ARITHMETIC_OVERFLOW;
    }
    else
    {
        *f_pi64Result = i64Result;
    }

    return dr;
}

/*
** Signed subtraction functions
*/

/* SHORT Subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortSub(
    __in                      DRM_SHORT       f_sMinuend,
    __in                      DRM_SHORT       f_sSubtrahend,
    __deref_out               DRM_SHORT      *f_psResult )
{
    DRMCASSERT( SIZEOF( DRM_LONG ) > SIZEOF( DRM_SHORT ) );
    return DRM_LongToShort( ((DRM_LONG)f_sMinuend) - ((DRM_LONG)f_sSubtrahend), f_psResult);
}

/* LONG Subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongSub(
    __in                      DRM_LONG       f_lMinuend,
    __in                      DRM_LONG       f_lSubtrahend,
    __deref_out               DRM_LONG      *f_plResult )
{
    DRMCASSERT( SIZEOF( DRM_INT64 ) > SIZEOF( DRM_LONG ) );
    return DRM_Int64ToLong( DRM_I64Sub( DRM_I64( f_lMinuend ), DRM_I64( f_lSubtrahend ) ), f_plResult );
}

/* INT64 Subtraction */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_Int64Sub(
    __in                      DRM_INT64       f_i64Minuend,
    __in                      DRM_INT64       f_i64Subtrahend,
    __deref_out               DRM_INT64      *f_pi64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_INT64  i64Result = DRM_I64Sub( f_i64Minuend, f_i64Subtrahend );

    DRMASSERT( f_pi64Result != NULL );
    __analysis_assume( f_pi64Result != NULL );

    /*
    ** Subtracting a positive number from a positive number never overflows.
    ** Subtracting a negative number from a negative number never overflows.
    ** If you subtract a negative number from a positive number, you expect a positive result.
    ** If you subtract a positive number from a negative number, you expect a negative result.
    ** Overflow if inputs vary in sign and the output does not have the same sign as the first input.
    */
    if( DRM_UNLIKELY(    ( DRM_I64Les( f_i64Minuend, DRM_I64Asgn( 0, 0 ) ) != DRM_I64Les( f_i64Subtrahend, DRM_I64Asgn( 0, 0 ) ) )
                      && ( DRM_I64Les( f_i64Minuend, DRM_I64Asgn( 0, 0 ) ) != DRM_I64Les( i64Result, DRM_I64Asgn( 0, 0 ) ) ) ) )
    {
        *f_pi64Result = DRM_INT64_ERROR;
         dr           = DRM_E_ARITHMETIC_OVERFLOW;
    }
    else
    {
        *f_pi64Result = i64Result;
    }

    return dr;
}

/*
** Signed multiplication functions
*/

/* SHORT multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ShortMult(
    __in                      DRM_SHORT       f_sMultiplicand,
    __in                      DRM_SHORT       f_sMultiplier,
    __deref_out               DRM_SHORT      *f_psResult )
{
    DRMCASSERT( SIZEOF( DRM_LONG ) > SIZEOF( DRM_SHORT ) );
    return DRM_LongToShort( ( (DRM_LONG)f_sMultiplicand ) * ( (DRM_LONG)f_sMultiplier ), f_psResult );
}

/* LONG multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LongMult(
    __in                      DRM_LONG       f_lMultiplicand,
    __in                      DRM_LONG       f_lMultiplier,
    __deref_out               DRM_LONG      *f_plResult )
{
    DRMCASSERT( SIZEOF( DRM_INT64 ) > SIZEOF( DRM_LONG ) );
    return DRM_Int64ToLong( DRM_I64Mul( DRM_I64(f_lMultiplicand), DRM_I64(f_lMultiplier) ), f_plResult );
}

/* INT64 multiplication */
DRM_API DRM_NO_INLINE DRM_RESULT DRM_Int64Mult(
    __in                      DRM_INT64       f_i64Multiplicand,
    __in                      DRM_INT64       f_i64Multiplier,
    __deref_out               DRM_INT64      *f_pi64Result )
{
    DRM_RESULT       dr                   = DRM_SUCCESS;
    DRM_UINT64       ui64Multiplicand     = DRM_UI64LITERAL(0,0);
    DRM_UINT64       ui64Multiplier       = DRM_UI64LITERAL(0,0);
    DRM_UINT64       ui64Result           = DRM_UI64LITERAL(0,0);
    DRM_INT64        i64NegativeResult    = DRM_I64LITERAL(0,0);
    DRM_UINT64       ui64MinMagnitude     = DRM_UI64LITERAL(0,0);

    DRMASSERT( f_pi64Result != NULL );
    __analysis_assume( f_pi64Result != NULL );

    i64NegativeResult = DRM_I64Sub( DRM_I64Asgn( 0, 0 ),
                                    DRM_I64Add( DRM_INT64_MIN, DRM_I64Asgn( 0, 1 ) ) );

    ui64MinMagnitude = DRM_UI64Add( DRM_I2UI64( i64NegativeResult ),
                                    DRM_UI64HL( 0, 1 ) );

    /*
    ** Split into sign and magnitude, do unsigned operation, apply sign.
    */
    if( DRM_I64Les( f_i64Multiplicand, DRM_I64Asgn( 0, 0 ) ) )
    {
        /* Avoid negating the most negative number. */
        i64NegativeResult = DRM_I64Sub( DRM_I64Asgn( 0, 0 ),
                                        DRM_I64Add( f_i64Multiplicand, DRM_I64Asgn( 0, 1 ) ) );
        ui64Multiplicand  = DRM_UI64Add( DRM_I2UI64( i64NegativeResult ),
                                         DRM_UI64HL( 0, 1 ) );
    }
    else
    {
        ui64Multiplicand = DRM_I2UI64( f_i64Multiplicand );
    }

    if( DRM_I64Les( f_i64Multiplier, DRM_I64Asgn( 0, 0 ) ) )
    {
        /* Avoid negating the most negative number. */
        i64NegativeResult = DRM_I64Sub( DRM_I64Asgn( 0, 0 ),
                                        DRM_I64Add( f_i64Multiplier, DRM_I64Asgn( 0, 1 ) ) );
        ui64Multiplier = DRM_UI64Add( DRM_I2UI64( i64NegativeResult ),
                                      DRM_UI64HL( 0, 1 ) );
    }
    else
    {
        ui64Multiplier = DRM_I2UI64( f_i64Multiplier );
    }

    dr = DRM_UInt64Mult( ui64Multiplicand, ui64Multiplier, &ui64Result );

    if( DRM_LIKELY( DRM_SUCCEEDED( dr ) ) )
    {
        if( DRM_I64Les( f_i64Multiplicand, DRM_I64Asgn( 0, 0 ) ) != DRM_I64Les( f_i64Multiplier, DRM_I64Asgn( 0, 0 ) ) )
        {
            if( DRM_UNLIKELY( DRM_UI64Les( ui64MinMagnitude, ui64Result ) ) )
            {
                *f_pi64Result = DRM_INT64_ERROR;
                 dr           = DRM_E_ARITHMETIC_OVERFLOW;
            }
            else
            {
                *f_pi64Result = DRM_I64Sub( DRM_I64Asgn( 0, 0 ), DRM_UI2I64( ui64Result ) );
            }
        }
        else
        {
            /* we need to assign the max to a value for I2UI64 conversion */
            i64NegativeResult = DRM_INT64_MAX;
            if( DRM_UNLIKELY( DRM_UI64Les( DRM_I2UI64( i64NegativeResult ), ui64Result ) ) )
            {
                *f_pi64Result = DRM_INT64_ERROR;
                 dr         = DRM_E_ARITHMETIC_OVERFLOW;
            }
            else
            {
                *f_pi64Result = DRM_UI2I64( ui64Result );
            }
        }
    }
    else
    {
        *f_pi64Result = DRM_INT64_ERROR;
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
