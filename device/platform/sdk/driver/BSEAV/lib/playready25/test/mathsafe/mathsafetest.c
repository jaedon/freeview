/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmint64.h>
#include <drmmathsafe.h>

#include <mathsafetest.h>

ENTER_PKTEST_NAMESPACE_CODE;

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(mathsafetest)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(mathsafetest)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL TestMathSafeConversion( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr         = DRM_SUCCESS;

    DRM_BYTE      bVal       = 0;
    DRM_SHORT     sVal       = 0;
    DRM_WORD      wVal       = 0;
    DRM_DWORD     dwVal      = 0;
    DRM_LONG      lVal       = 0;
    DRM_DWORD_PTR dwpVal     = 0;
    DRM_INT64     i64Val     = DRM_I64LITERAL( 0, 0 );
    DRM_UINT64    ui64Val    = DRM_UI64LITERAL( 0, 0 );

    DRM_BYTE      bResult    = 0;
    DRM_CHAR      chResult   = '\0';
    DRM_SHORT     sResult    = 0;
    DRM_WORD      wResult    = 0;
    DRM_DWORD     dwResult   = 0;
    DRM_LONG      lResult    = 0;
    DRM_INT64     i64Result  = DRM_I64LITERAL( 0, 0 );
    DRM_UINT64    ui64Result = DRM_UI64LITERAL( 0, 0 );

    /* test ByteToChar */
    bVal = 4;
    ChkDR( DRM_ByteToChar( bVal, &chResult ) );
    ChkBOOL( (DRM_CHAR)bVal == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    bVal = DRM_CHAR_MAX + 1;
    dr = DRM_ByteToChar( bVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test ShortToChar */
    sVal = 4;
    ChkDR( DRM_ShortToChar( sVal, &chResult ) );
    ChkBOOL( (DRM_CHAR)sVal == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal = DRM_CHAR_MAX + 1;
    dr = DRM_ShortToChar( sVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal = DRM_CHAR_MIN - 1;
    dr = DRM_ShortToChar( sVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test ShortToByte */
    sVal = 234;
    ChkDR( DRM_ShortToByte( sVal, &bResult ) );
    ChkBOOL( (DRM_BYTE)sVal == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal = MAX_UNSIGNED_TYPE(DRM_BYTE) + 1;
    dr = DRM_ShortToByte( sVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal = -1234;
    dr = DRM_ShortToByte( sVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test ShortToWord */
    sVal = 232;
    ChkDR( DRM_ShortToWord( sVal, &wResult ) );
    ChkBOOL( (DRM_WORD)sVal == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal = -21;
    dr = DRM_ShortToWord( sVal, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test ShortToDWord */
    sVal = 212;
    ChkDR( DRM_ShortToDWord( sVal, &dwResult ) );
    ChkBOOL( (DRM_DWORD)sVal == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal = -21;
    dr = DRM_ShortToDWord( sVal, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test ShortToUInt64 */
    sVal = 223;
    ChkDR( DRM_ShortToUInt64( sVal, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 0, sVal ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal = -121;
    dr = DRM_ShortToUInt64( sVal, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test WordToChar */
    wVal = 24;
    ChkDR( DRM_WordToChar( wVal, &chResult ) );
    ChkBOOL( (DRM_CHAR)wVal == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal = DRM_CHAR_MAX + 1;
    dr = DRM_WordToChar( wVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test WordToByte */
    wVal = 42;
    ChkDR( DRM_WordToByte( wVal, &bResult ) );
    ChkBOOL( (DRM_BYTE)wVal == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal = MAX_UNSIGNED_TYPE(DRM_BYTE) + 1;
    dr = DRM_WordToByte( wVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test WordToShort */
    wVal = 42;
    ChkDR( DRM_WordToShort( wVal, &sResult ) );
    ChkBOOL( (DRM_SHORT)wVal == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal = DRM_SHORT_MAX + 1;
    dr = DRM_WordToShort( wVal, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test LongToChar */
    lVal = 14;
    ChkDR( DRM_LongToChar( lVal, &chResult ) );
    ChkBOOL( (DRM_CHAR)lVal == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = DRM_CHAR_MAX + 1;
    dr = DRM_LongToChar( lVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal = DRM_CHAR_MIN - 1;
    dr = DRM_LongToChar( lVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test LongToByte */
    lVal = 44;
    ChkDR( DRM_LongToByte( lVal, &bResult ) );
    ChkBOOL( (DRM_BYTE)lVal == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = MAX_UNSIGNED_TYPE(DRM_BYTE) + 1;
    dr = DRM_LongToByte( lVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal = -31;
    dr = DRM_LongToByte( lVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test LongToShort */
    lVal = 234;
    ChkDR( DRM_LongToShort( lVal, &sResult ) );
    ChkBOOL( (DRM_SHORT)lVal == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = DRM_SHORT_MAX + 1;
    dr = DRM_LongToShort( lVal, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal = -23;
    ChkDR( DRM_LongToShort( lVal, &sResult ) );
    ChkBOOL( (DRM_SHORT)lVal == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = DRM_SHORT_MIN - 1;
    dr = DRM_LongToShort( lVal, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test LongToWord */
    lVal = 24;
    ChkDR( DRM_LongToWord( lVal, &wResult ) );
    ChkBOOL( (DRM_WORD)lVal == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = MAX_UNSIGNED_TYPE(DRM_WORD) + 1;
    dr = DRM_LongToWord( lVal, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal = -31;
    dr = DRM_LongToWord( lVal, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test LongToDWord */
    lVal = 42;
    ChkDR( DRM_LongToDWord( lVal, &dwResult ) );
    ChkBOOL( (DRM_DWORD)lVal == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal = -13;
    dr = DRM_LongToDWord( lVal, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordToChar */
    dwVal = 42;
    ChkDR( DRM_DWordToChar( dwVal, &chResult ) );
    ChkBOOL( (DRM_CHAR)dwVal == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal = DRM_CHAR_MAX + 1;
    dr = DRM_DWordToChar( dwVal, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordToByte */
    dwVal = 44;
    ChkDR( DRM_DWordToByte( dwVal, &bResult ) );
    ChkBOOL( (DRM_BYTE)dwVal == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal = MAX_UNSIGNED_TYPE(DRM_BYTE) + 1;
    dr = DRM_DWordToByte( dwVal, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordToShort */
    dwVal = 234;
    ChkDR( DRM_DWordToShort( dwVal, &sResult ) );
    ChkBOOL( (DRM_SHORT)dwVal == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal = DRM_SHORT_MAX + 1;
    dr = DRM_DWordToShort( dwVal, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordToWord */
    dwVal = 43;
    ChkDR( DRM_DWordToWord( dwVal, &wResult ) );
    ChkBOOL( (DRM_WORD)dwVal == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal = MAX_UNSIGNED_TYPE(DRM_WORD) + 1;
    dr = DRM_DWordToWord( dwVal, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordToLong */
    dwVal = 24;
    ChkDR( DRM_DWordToLong( dwVal, &lResult ) );
    ChkBOOL( (DRM_WORD)dwVal == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal = (DRM_DWORD)DRM_LONG_MAX + 1;
    dr = DRM_DWordToLong( dwVal, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DWordPtrToDWord */
#ifdef DRM_64BIT_TARGET
    /* DWordPtr is defined to be UInt64 if DRM_64BIT_TARGET is defined
    ** therefore, it's tested in UInt64 scenerio and not here
    */
    UNREFERENCED_PARAMETER( dwpVal );
#else /* DRM_64BIT_TARGET */
    /* DWordPtr is defined to be DWord, there no need for extensive testing */
    dwpVal = 24;
    ChkDR( DRM_DWordPtrToDWord( dwpVal, &dwResult ) );
    ChkBOOL( (DRM_DWORD)dwpVal == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
#endif /* DRM_64BIT_TARGET */

    /* extensive test on Int64 and UInt64 */
    /* test Int64ToChar */
    i64Val = DRM_I64( DRM_CHAR_MAX + 1 );
    dr = DRM_Int64ToChar( i64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( DRM_CHAR_MAX );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MAX == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_CHAR_MAX - 1 );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MAX - 1 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 2 );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( 2 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( -2 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -54 );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( -54 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_CHAR_MIN + 1 );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MIN + 1 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_CHAR_MIN );
    ChkDR( DRM_Int64ToChar( i64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MIN == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_CHAR_MIN - 1 );
    dr = DRM_Int64ToChar( i64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -23423 );
    dr = DRM_Int64ToChar( i64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -8908098 );
    dr = DRM_Int64ToChar( i64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -8908098, 124134 );
    dr = DRM_Int64ToChar( i64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToWord */
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_WORD) + 1 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_WORD) );
    ChkDR( DRM_Int64ToWord( i64Val, &wResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_WORD) == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_WORD) - 1 );
    ChkDR( DRM_Int64ToWord( i64Val, &wResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_WORD) - 1 == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 5 );
    ChkDR( DRM_Int64ToWord( i64Val, &wResult ) );
    ChkBOOL( 5 == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -234242 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -89898, 124134 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToByte */
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_BYTE) + 1 );
    dr = DRM_Int64ToByte( i64Val, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_BYTE) );
    ChkDR( DRM_Int64ToByte( i64Val, &bResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_BYTE) == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( MAX_UNSIGNED_TYPE(DRM_BYTE) - 1 );
    ChkDR( DRM_Int64ToByte( i64Val, &bResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_BYTE) - 1 == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 5 );
    ChkDR( DRM_Int64ToByte( i64Val, &bResult ) );
    ChkBOOL( 5 == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    dr = DRM_Int64ToByte( i64Val, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -2342123 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -78979, 124134 );
    dr = DRM_Int64ToWord( i64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToShort */
    i64Val = DRM_I64( DRM_SHORT_MAX + 1 );
    dr = DRM_Int64ToShort( i64Val, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( DRM_SHORT_MAX );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MAX == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_SHORT_MAX - 1 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MAX - 1 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 2 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( 2 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( -2 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -54 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( -54 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_SHORT_MIN + 1 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MIN + 1 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_SHORT_MIN );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MIN == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_SHORT_MIN - 1 );
    dr = DRM_Int64ToShort( i64Val, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -23423 );
    ChkDR( DRM_Int64ToShort( i64Val, &sResult ) );
    ChkBOOL( -23423 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -2342342 );
    dr = DRM_Int64ToShort( i64Val, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -2123123, 124134 );
    dr = DRM_Int64ToShort( i64Val, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToLong */
    i64Val = DRM_I64Asgn( 13423, 23423 );
    dr = DRM_Int64ToLong( i64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Add( DRM_I64( DRM_LONG_MAX ), DRM_I64( 1 ) );
    dr = DRM_Int64ToLong( i64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( DRM_LONG_MAX );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MAX == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_LONG_MAX - 1 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MAX - 1 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 793423424 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( 793423424 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( 223 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( 223 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -23 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( -23 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -5423 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( -5423 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -42323423 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( -42323423 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_LONG_MIN + 1 );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MIN + 1 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( DRM_LONG_MIN );
    ChkDR( DRM_Int64ToLong( i64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MIN == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Sub( DRM_I64( DRM_LONG_MIN ), DRM_I64( 1 ) );
    dr = DRM_Int64ToLong( i64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Sub( DRM_I64( DRM_LONG_MIN ), DRM_I64( 23421 ) );
    dr = DRM_Int64ToLong( i64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Sub( DRM_I64( DRM_LONG_MIN ), DRM_I64( 79890 ) );
    dr = DRM_Int64ToLong( i64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToDWord */
    i64Val = DRM_I64Asgn( 134, 897798 );
    dr = DRM_Int64ToDWord( i64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( 1, 0 );
    dr = DRM_Int64ToDWord( i64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    /* NOTE: DWord is special here DRM_I64(a) does sign extension. Therefore,
    ** if the most significant bit of a DWORD is 1, then DRM_I64 will do a sign bit extension
    ** for High32 bits to make the all I64 negative. In general, use DRM_I64Asgn for unsigned conversion
    ** and DRM_I64 for signed conversion
    */
    i64Val = DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_Int64ToDWord( i64Val, &dwResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_DWORD) == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 );
    ChkDR( DRM_Int64ToDWord( i64Val, &dwResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 0, 23423 );
    ChkDR( DRM_Int64ToDWord( i64Val, &dwResult ) );
    ChkBOOL( 23423 == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    dr = DRM_Int64ToDWord( i64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -3422342 );
    dr = DRM_Int64ToDWord( i64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -890232398, 124134 );
    dr = DRM_Int64ToDWord( i64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64ToUInt64 */
    i64Val = DRM_I64Asgn( 134, 897798 );
    ChkDR( DRM_Int64ToUInt64( i64Val, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 134, 897798 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 1, 0 );
    ChkDR( DRM_Int64ToUInt64( i64Val, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 1, 0 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_Int64ToUInt64( i64Val, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 );
    ChkDR( DRM_Int64ToUInt64( i64Val, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64Asgn( 0, 23423 );
    ChkDR( DRM_Int64ToUInt64( i64Val, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64HL( 0, 23423), ui64Result), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val = DRM_I64( -2 );
    dr = DRM_Int64ToUInt64( i64Val, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64( -81238098 );
    dr = DRM_Int64ToUInt64( i64Val, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val = DRM_I64Asgn( -823248098, 124134 );
    dr = DRM_Int64ToUInt64( i64Val, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test UInt64ToChar */
    ui64Val = DRM_UI64( DRM_CHAR_MAX + 1 );
    dr = DRM_UInt64ToChar( ui64Val, &chResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64( DRM_CHAR_MAX );
    ChkDR( DRM_UInt64ToChar( ui64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MAX == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( DRM_CHAR_MAX - 1 );
    ChkDR( DRM_UInt64ToChar( ui64Val, &chResult ) );
    ChkBOOL( DRM_CHAR_MAX - 1 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 2 );
    ChkDR( DRM_UInt64ToChar( ui64Val, &chResult ) );
    ChkBOOL( 2 == chResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test UInt64ToWord */
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_WORD) + 1 );
    dr = DRM_UInt64ToWord( ui64Val, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_WORD) );
    ChkDR( DRM_UInt64ToWord( ui64Val, &wResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_WORD) == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_WORD) - 1 );
    ChkDR( DRM_UInt64ToWord( ui64Val, &wResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_WORD) - 1 == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 5 );
    ChkDR( DRM_UInt64ToWord( ui64Val, &wResult ) );
    ChkBOOL( 5 == wResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test UInt64ToByte */
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_BYTE) + 1 );
    dr = DRM_UInt64ToByte( ui64Val, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_BYTE) );
    ChkDR( DRM_UInt64ToByte( ui64Val, &bResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_BYTE) == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( MAX_UNSIGNED_TYPE(DRM_BYTE) - 1 );
    ChkDR( DRM_UInt64ToByte( ui64Val, &bResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_BYTE) - 1 == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 5 );
    ChkDR( DRM_UInt64ToByte( ui64Val, &bResult ) );
    ChkBOOL( 5 == bResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test UInt64ToShort */
    ui64Val = DRM_UI64( DRM_SHORT_MAX + 1 );
    dr = DRM_UInt64ToShort( ui64Val, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64( DRM_SHORT_MAX );
    ChkDR( DRM_UInt64ToShort( ui64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MAX == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( DRM_SHORT_MAX - 1 );
    ChkDR( DRM_UInt64ToShort( ui64Val, &sResult ) );
    ChkBOOL( DRM_SHORT_MAX - 1 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 2 );
    ChkDR( DRM_UInt64ToShort( ui64Val, &sResult ) );
    ChkBOOL( 2 == sResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test UInt64ToLong */
    ui64Val = DRM_UI64HL( 13423, 23423 );
    dr = DRM_UInt64ToLong( ui64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64Add( DRM_UI64( DRM_LONG_MAX ), DRM_UI64( 1 ) );
    dr = DRM_UInt64ToLong( ui64Val, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64( DRM_LONG_MAX );
    ChkDR( DRM_UInt64ToLong( ui64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MAX == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( DRM_LONG_MAX - 1 );
    ChkDR( DRM_UInt64ToLong( ui64Val, &lResult ) );
    ChkBOOL( DRM_LONG_MAX - 1 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 793423424 );
    ChkDR( DRM_UInt64ToLong( ui64Val, &lResult ) );
    ChkBOOL( 793423424 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64( 223 );
    ChkDR( DRM_UInt64ToLong( ui64Val, &lResult ) );
    ChkBOOL( 223 == lResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test UInt64ToDWord */
    ui64Val = DRM_UI64HL( 134, 897798 );
    dr = DRM_UInt64ToDWord( ui64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64HL( 1, 0 ); /* Note: Equivalent to MAX_UNSIGNED_TYPE(DWORD) + 1 */
    dr = DRM_UInt64ToDWord( ui64Val, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val = DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_UInt64ToDWord( ui64Val, &dwResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_DWORD) == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 );
    ChkDR( DRM_UInt64ToDWord( ui64Val, &dwResult ) );
    ChkBOOL( MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0, 23423 );
    ChkDR( DRM_UInt64ToDWord( ui64Val, &dwResult ) );
    ChkBOOL( 23423 == dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );

#ifdef DRM_64BIT_TARGET
    DRM_DWORD_PTR pdwResult = DRM_UI64LITERAL( 0, 0 );

    /* No need for extensive test since DRM_DWORD_PTR is defined to be DRM_UInt64 */
    ui64Val = DRM_UI64HL( 0x7FFFFFFF, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_UInt64ToDWordPtr( ui64Val, &pdwResult ) );
    ChkBOOL( DRM_UI64Eql( ui64Val, pdwResult ), DRM_E_TEST_UNEXPECTED_OUTPUT ); 
#endif /* DRM_64BIT_TARGET */

    /* test UInt64ToInt64 */
    ui64Val = DRM_UI64HL( 134, 897798 );
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 134, 897798 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 1, 0 ); /* Note: Equivalent to MAX_UNSIGNED_TYPE(DWORD) + 1 */
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 1, 0 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 );
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) - 1 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0, 23423 );
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 0, 23423 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0x7FFFFFFF, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_UInt64ToInt64( ui64Val, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Asgn( 0x7FFFFFFF, MAX_UNSIGNED_TYPE(DRM_DWORD) ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val = DRM_UI64HL( 0x80000000, 124134 ); /* highest bit set which is a overflow on signed */
    dr = DRM_UInt64ToInt64( ui64Val, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestMathSafeSubtraction( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_BYTE   bVal1      = 0;
    DRM_BYTE   bVal2      = 0;
    DRM_WORD   wVal1      = 0;
    DRM_WORD   wVal2      = 0;
    DRM_DWORD  dwVal1     = 0;
    DRM_DWORD  dwVal2     = 0;
    DRM_SHORT  sVal1      = 0;
    DRM_SHORT  sVal2      = 0;
    DRM_LONG   lVal1      = 0;
    DRM_LONG   lVal2      = 0;
    DRM_UINT64 ui64Val1   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64Val2   = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Val1    = DRM_I64LITERAL( 0, 0 );
    DRM_INT64  i64Val2    = DRM_I64LITERAL( 0, 0 );

    DRM_BYTE   bResult    = 0;
    DRM_WORD   wResult    = 0;
    DRM_DWORD  dwResult   = 0;
    DRM_SHORT  sResult    = 0;
    DRM_LONG   lResult    = 0;
    DRM_UINT64 ui64Result = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Result  = DRM_I64LITERAL( 0, 0 );

    /* test DRM_ByteSub */
    bVal1 = 255;
    bVal2 = 23;
    ChkDR( DRM_ByteSub( bVal1, bVal2, &bResult ) );
    ChkBOOL( bResult == bVal1 - bVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    bVal1 = 32;
    bVal2 = 43;
    dr = DRM_ByteSub( bVal1, bVal2, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    bVal1 = 0;
    bVal2 = 7;
    dr = DRM_ByteSub( bVal1, bVal2, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_WordSub */
    wVal1 = 234;
    wVal2 = 23;
    ChkDR( DRM_WordSub( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 - wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = 32;
    wVal2 = 43;
    dr = DRM_WordSub( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    wVal1 = 0;
    wVal2 = 7;
    dr = DRM_WordSub( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_DWordSub */
    dwVal1 = 434234;
    dwVal2 = 342342;
    ChkDR( DRM_DWordSub( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 - dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD);
    dwVal2 = 342342;
    ChkDR( DRM_DWordSub( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 - dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = 3;
    dwVal2 = 4;
    dr = DRM_DWordSub( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    dwVal1 = 0;
    dwVal2 = 332;
    dr = DRM_DWordSub( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_UInt64Sub */
    ui64Val1 = DRM_UI64HL( 0, 434234 );
    ui64Val2 = DRM_UI64HL( 0, 342342 );
    ChkDR( DRM_UInt64Sub( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Sub( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 3424142, 134234 );
    ui64Val2 = DRM_UI64HL( 1341124, 342342 );
    ChkDR( DRM_UInt64Sub( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Sub( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 323234, 134324 );
    ui64Val2 = DRM_UI64HL( 400000, 0 );
    dr = DRM_UInt64Sub( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val1 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD), 134324 );
    ui64Val2 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD), MAX_UNSIGNED_TYPE(DRM_DWORD) );
    dr = DRM_UInt64Sub( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_ShortSub */
    sVal1 = 1;
    sVal2 = DRM_SHORT_MAX;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 0;
    sVal2 = DRM_SHORT_MAX;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -1;
    sVal2 = DRM_SHORT_MAX;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -2;
    sVal2 = DRM_SHORT_MAX;
    dr = DRM_ShortSub( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = 234;
    sVal2 = 23;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 32;
    sVal2 = 43;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 0;
    sVal2 = 7;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -7;
    sVal2 = -9;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -7;
    sVal2 = DRM_SHORT_MIN;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN;
    sVal2 = -3;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN + 1;
    sVal2 = 1;
    ChkDR( DRM_ShortSub( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 - sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN;
    sVal2 = 5;
    dr = DRM_ShortSub( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_LongSub */
    lVal1 = 1;
    lVal2 = DRM_LONG_MAX;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = 0;
    lVal2 = DRM_LONG_MAX;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -1;
    lVal2 = DRM_LONG_MAX;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -2;
    lVal2 = DRM_LONG_MAX;
    dr = DRM_LongSub( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = 23243434;
    lVal2 = 223423243;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -234232;
    lVal2 = 23443;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -723423;
    lVal2 = -2342349;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -7;
    lVal2 = DRM_LONG_MIN;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MIN;
    lVal2 = -3;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MIN + 1;
    lVal2 = 1;
    ChkDR( DRM_LongSub( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 - lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MIN;
    lVal2 = 5;
    dr = DRM_LongSub( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_Int64Sub */
    i64Val1 = DRM_I64Asgn( 0, 1 );
    i64Val2 = DRM_INT64_MAX;
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 0, 0 );
    i64Val2 = DRM_INT64_MAX;
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -1 );
    i64Val2 = DRM_INT64_MAX;
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -2 );
    i64Val2 = DRM_INT64_MAX;
    dr = DRM_Int64Sub( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64Asgn( 234234, 43434 );
    i64Val2 = DRM_I64Asgn( 34232, 32423 );
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( -234232, 23424 );
    i64Val2 = DRM_I64Asgn( 23443, 2342 );
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( -234232, 23424 );
    i64Val2 = DRM_I64Asgn( 0x7FFFFFFF, 2342 );
    dr = DRM_Int64Sub( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64Asgn( -723423, 21342 );
    i64Val2 = DRM_I64Asgn( -2342349, 23421 );
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -7 );
    i64Val2 = DRM_INT64_MIN;
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_INT64_MIN;
    i64Val2 = DRM_I64( -3 );
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Add( DRM_INT64_MIN, DRM_I64( 1 ) );
    i64Val2 = DRM_I64( 1 );
    ChkDR( DRM_Int64Sub( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Sub( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_INT64_MIN;
    i64Val2 = DRM_I64( 5 );
    dr = DRM_Int64Sub( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestMathSafeAddition( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_BYTE   bVal1      = 0;
    DRM_BYTE   bVal2      = 0;
    DRM_WORD   wVal1      = 0;
    DRM_WORD   wVal2      = 0;
    DRM_DWORD  dwVal1     = 0;
    DRM_DWORD  dwVal2     = 0;
    DRM_SHORT  sVal1      = 0;
    DRM_SHORT  sVal2      = 0;
    DRM_LONG   lVal1      = 0;
    DRM_LONG   lVal2      = 0;
    DRM_UINT64 ui64Val1   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64Val2   = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Val1    = DRM_I64LITERAL( 0, 0 );
    DRM_INT64  i64Val2    = DRM_I64LITERAL( 0, 0 );

    DRM_BYTE   bResult    = 0;
    DRM_WORD   wResult    = 0;
    DRM_DWORD  dwResult   = 0;
    DRM_SHORT  sResult    = 0;
    DRM_LONG   lResult    = 0;
    DRM_UINT64 ui64Result = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Result  = DRM_I64LITERAL( 0, 0 );

    /* test DRM_ByteAdd */
    bVal1 = 3;
    bVal2 = 4;
    ChkDR( DRM_ByteAdd( bVal1, bVal2, &bResult ) );
    ChkBOOL( bResult == bVal1 + bVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    bVal1 = 4;
    bVal2 = MAX_UNSIGNED_TYPE(DRM_BYTE) - 5;
    ChkDR( DRM_ByteAdd( bVal1, bVal2, &bResult ) );
    ChkBOOL( bResult == bVal1 + bVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    bVal1 = MAX_UNSIGNED_TYPE(DRM_BYTE) - 5;
    bVal2 = 5;
    ChkDR( DRM_ByteAdd( bVal1, bVal2, &bResult ) );
    ChkBOOL( bResult == bVal1 + bVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    bVal1 = MAX_UNSIGNED_TYPE(DRM_BYTE);
    bVal2 = 4;
    dr = DRM_ByteAdd( bVal1, bVal2, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    bVal1 = MAX_UNSIGNED_TYPE(DRM_BYTE) - 3;
    bVal2 = 7;
    dr = DRM_ByteAdd( bVal1, bVal2, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    bVal1 = 6;
    bVal2 = MAX_UNSIGNED_TYPE(DRM_BYTE) - 4;
    dr = DRM_ByteAdd( bVal1, bVal2, &bResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_WordAdd */
    wVal1 = 3;
    wVal2 = 4;
    ChkDR( DRM_WordAdd( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 + wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = 3;
    wVal2 = MAX_UNSIGNED_TYPE(DRM_WORD) - 4;
    ChkDR( DRM_WordAdd( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 + wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = 4;
    wVal2 = MAX_UNSIGNED_TYPE(DRM_WORD) - 4;
    ChkDR( DRM_WordAdd( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 + wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD);
    wVal2 = 4;
    dr = DRM_WordAdd( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD) - 5;
    wVal2 = 7;
    dr = DRM_WordAdd( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD) - 3;
    wVal2 = 6;
    dr = DRM_WordAdd( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_DWordAdd */
    dwVal1 = 134234;
    dwVal2 = 342342;
    ChkDR( DRM_DWordAdd( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 + dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = 32;
    dwVal2 = MAX_UNSIGNED_TYPE(DRM_DWORD) - 82;
    ChkDR( DRM_DWordAdd( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 + dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = 82;
    dwVal2 = MAX_UNSIGNED_TYPE(DRM_DWORD) - 82;
    ChkDR( DRM_DWordAdd( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 + dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD);
    dwVal2 = 4;
    dr = DRM_DWordAdd( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD) - 5;
    dwVal2 = 332;
    dr = DRM_DWordAdd( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    dwVal1 = 878;
    dwVal2 = MAX_UNSIGNED_TYPE(DRM_DWORD) - 234;
    dr = DRM_DWordAdd( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_UInt64Add */
    ui64Val1 = DRM_UI64HL( 0, 134234 );
    ui64Val2 = DRM_UI64HL( 0, 342342 );
    ChkDR( DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Add( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD) - 234234,  MAX_UNSIGNED_TYPE(DRM_DWORD) - 134234 );
    ui64Val2 = DRM_UI64HL( 234234, 134234 );
    ChkDR( DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Add( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 2, 13 );
    ui64Val2 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD) - 23,  MAX_UNSIGNED_TYPE(DRM_DWORD) - 234);
    ChkDR( DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Add( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 3424142, 134234 );
    ui64Val2 = DRM_UI64HL( 13414124, 342342 );
    ChkDR( DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( DRM_UI64Add( ui64Val1, ui64Val2 ), ui64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 323234, 134324 );
    ui64Val2 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD), 0 );
    dr = DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val1 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD), 134324 );
    ui64Val2 = DRM_UI64HL( MAX_UNSIGNED_TYPE(DRM_DWORD), MAX_UNSIGNED_TYPE(DRM_DWORD) );
    dr = DRM_UInt64Add( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_ShortAdd */
    sVal1 = 3;
    sVal2 = 4;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MAX;
    sVal2 = 4;
    dr = DRM_ShortAdd( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 4;
    sVal2 = DRM_SHORT_MAX - 7;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 8;
    sVal2 = DRM_SHORT_MAX - 7;
    dr = DRM_ShortAdd( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = DRM_SHORT_MAX - 5;
    sVal2 = 7;
    dr = DRM_ShortAdd( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = -3;
    sVal2 = -4;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -3;
    sVal2 = 4;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN;
    sVal2 = 4;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN;
    sVal2 = -4;
    dr = DRM_ShortAdd( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = 3;
    sVal2 = DRM_SHORT_MIN + 6;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -4;
    sVal2 = DRM_SHORT_MIN;
    dr = DRM_ShortAdd( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = -6;
    sVal2 = DRM_SHORT_MIN + 6;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MAX - 5;
    sVal2 = 5;
    ChkDR( DRM_ShortAdd( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 + sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test DRM_LongAdd */
    lVal1 = 134234;
    lVal2 = 342342;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MAX;
    lVal2 = 4;
    dr = DRM_LongAdd( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = DRM_LONG_MAX - 5;
    lVal2 = 332;
    dr = DRM_LongAdd( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = -23;
    lVal2 = -2342;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -23;
    lVal2 = 2342;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -23;
    lVal2 = 2342;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -23;
    lVal2 = -2342;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MIN;
    lVal2 = -3;
    dr = DRM_LongAdd( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = DRM_LONG_MIN;
    lVal2 = 5;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -4;
    lVal2 = DRM_LONG_MIN + 2;
    dr = DRM_LongAdd( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = -3;
    lVal2 = DRM_LONG_MIN + 4;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -3;
    lVal2 = DRM_LONG_MIN + 3;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MAX - 8;
    lVal2 = 8;
    ChkDR( DRM_LongAdd( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 + lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test DRM_Int64Add */
    i64Val1 = DRM_I64Asgn( 0, 134234 );
    i64Val2 = DRM_I64Asgn( 0, 342342 );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 3424142, 134234 );
    i64Val2 = DRM_I64Asgn( 13414124, 342342 );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 323234, 134324 );
    i64Val2 = DRM_I64Asgn( MAX_UNSIGNED_TYPE(DRM_DWORD), 0 );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( MAX_UNSIGNED_TYPE(DRM_DWORD), 134324 );
    i64Val2 = DRM_I64Asgn( MAX_UNSIGNED_TYPE(DRM_DWORD), MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_INT64_MIN;
    i64Val2 = DRM_I64( -1 );
    dr = DRM_Int64Add( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_INT64_MIN;
    i64Val2 = DRM_I64( 4 );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 3 );
    i64Val2 = DRM_I64Sub( DRM_INT64_MIN, DRM_I64( -4 ) );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 7 );
    i64Val2 = DRM_I64Sub( DRM_INT64_MIN, DRM_I64( -3 ) );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -4 );
    i64Val2 = DRM_I64Sub( DRM_INT64_MIN, DRM_I64( -4 ) );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 4 );
    i64Val2 = DRM_I64Sub( DRM_INT64_MAX, DRM_I64( 4 ) );
    ChkDR( DRM_Int64Add( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( DRM_I64Add( i64Val1, i64Val2 ), i64Result ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 5 );
    i64Val2 = DRM_I64Sub( DRM_INT64_MAX, DRM_I64( 4 ) );
    dr = DRM_Int64Add( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestMathSafeMultiplication( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr         = DRM_SUCCESS;

    DRM_WORD   wVal1      = 0;
    DRM_WORD   wVal2      = 0;
    DRM_DWORD  dwVal1     = 0;
    DRM_DWORD  dwVal2     = 0;
    DRM_SHORT  sVal1      = 0;
    DRM_SHORT  sVal2      = 0;
    DRM_LONG   lVal1      = 0;
    DRM_LONG   lVal2      = 0;
    DRM_UINT64 ui64Val1   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64Val2   = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Val1    = DRM_I64LITERAL( 0, 0 );
    DRM_INT64  i64Val2    = DRM_I64LITERAL( 0, 0 );

    DRM_WORD   wResult    = 0;
    DRM_DWORD  dwResult   = 0;
    DRM_SHORT  sResult    = 0;
    DRM_LONG   lResult    = 0;
    DRM_UINT64 ui64Result = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64  i64Result  = DRM_I64LITERAL( 0, 0 );

    /* test DRM_WordMult */
    wVal1 = 3;
    wVal2 = 4;
    ChkDR( DRM_WordMult( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 * wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD) / 4;
    wVal2 = 4;
    ChkDR( DRM_WordMult( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 * wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = 3;
    wVal2 = 34;
    ChkDR( DRM_WordMult( wVal1, wVal2, &wResult ) );
    ChkBOOL( wResult == wVal1 * wVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD);
    wVal2 = 4;
    dr = DRM_WordMult( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    wVal1 = MAX_UNSIGNED_TYPE(DRM_WORD) - 5;
    wVal2 = 7;
    dr = DRM_WordMult( wVal1, wVal2, &wResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_DWordMult */
    dwVal1 = 233;
    dwVal2 = 2344;
    ChkDR( DRM_DWordMult( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 * dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD) / 2;
    dwVal2 = 2;
    ChkDR( DRM_DWordMult( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 * dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = 324;
    dwVal2 = 3789;
    ChkDR( DRM_DWordMult( dwVal1, dwVal2, &dwResult ) );
    ChkBOOL( dwResult == dwVal1 * dwVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD);
    dwVal2 = 4;
    dr = DRM_DWordMult( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    dwVal1 = MAX_UNSIGNED_TYPE(DRM_DWORD) - 5;
    dwVal2 = 7;
    dr = DRM_DWordMult( dwVal1, dwVal2, &dwResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test UInt64Mult */
    ui64Val1 = DRM_UI64HL( 1234, 4567 );
    ui64Val2 = DRM_UI64HL( 0, 7654 );
    ChkDR( DRM_UInt64Mult( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( ui64Result, DRM_UI64Mul( ui64Val1, ui64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 0x7FFFFFFF, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    ui64Val2 = DRM_UI64HL( 0, 2 );
    ChkDR( DRM_UInt64Mult( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( ui64Result, DRM_UI64Mul( ui64Val1, ui64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 0, 4567 );
    ui64Val2 = DRM_UI64HL( 0, 7654 );
    ChkDR( DRM_UInt64Mult( ui64Val1, ui64Val2, &ui64Result ) );
    ChkBOOL( DRM_UI64Eql( ui64Result, DRM_UI64Mul( ui64Val1, ui64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ui64Val1 = DRM_UI64HL( 0, 0xFFFFFFFF );
    ui64Val2 = DRM_UI64HL( 0xFFFFFFFF, 0xFFFFFFFF );
    dr = DRM_UInt64Mult( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    ui64Val1 = DRM_UI64HL( 0, 4 );
    ui64Val2 = DRM_UI64HL( 0x7FFFFFFF, MAX_UNSIGNED_TYPE(DRM_DWORD) );
    dr = DRM_UInt64Mult( ui64Val1, ui64Val2, &ui64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test DRM_ShortMult */
    sVal1 = 3;
    sVal2 = 4;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = 3;
    sVal2 = 34;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -33;
    sVal2 = -334;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -2;
    sVal2 = 23;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = -3;
    sVal2 = -43;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MAX;
    sVal2 = 4;
    dr = DRM_ShortMult( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = DRM_SHORT_MAX - 5;
    sVal2 = 7;
    dr = DRM_ShortMult( sVal1, sVal2, &sResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    sVal1 = DRM_SHORT_MAX / 7;
    sVal2 = 7;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    sVal1 = DRM_SHORT_MIN / 2;
    sVal2 = 2;
    ChkDR( DRM_ShortMult( sVal1, sVal2, &sResult ) );
    ChkBOOL( sResult == sVal1 * sVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* test DRM_LongMult */
    lVal1 = 3;
    lVal2 = 4;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = 3;
    lVal2 = 34;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = 212;
    lVal2 = -789;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -23;
    lVal2 = 234;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = -33;
    lVal2 = -334;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MIN / 4;
    lVal2 = 4;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MAX;
    lVal2 = 1;
    ChkDR( DRM_LongMult( lVal1, lVal2, &lResult ) );
    ChkBOOL( lResult == lVal1 * lVal2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    lVal1 = DRM_LONG_MAX;
    lVal2 = 4;
    dr = DRM_LongMult( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    lVal1 = DRM_LONG_MAX - 5;
    lVal2 = 7;
    dr = DRM_LongMult( lVal1, lVal2, &lResult );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

    /* test Int64Mult */
    i64Val1 = DRM_I64Asgn( -1234, 4567 ); /* == -5299989638697 */
    i64Val2 = DRM_I64Asgn( 0, 7654 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 234, 4567 ); /* == 1005022351831 */
    i64Val2 = DRM_I64Asgn( 0xFFFFFFFF, 0xFFFF7654 ); /* == -35244 */
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 0xFFFFFFFF, 0xFFFF4567 ); /* == -47769 */
    i64Val2 = DRM_I64Asgn( 0xFFFFFFFF, 0xFFFF7654 ); /* == -35244 */
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -234  );
    i64Val2 = DRM_I64( -342342 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 324324  );
    i64Val2 = DRM_I64( -378972 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -789324  );
    i64Val2 = DRM_I64( 90972 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( DRM_LONG_MIN  );
    i64Val2 = DRM_I64( 32 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( -2 );
    i64Val2 = DRM_I64Asgn( 0x40000000, 0x00000000 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 1 );
    i64Val2 = DRM_I64Asgn( 0xFFFFFFFF, 0xFFFFFFFF );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64( 1 );
    i64Val2 = DRM_I64Asgn( 0x7FFFFFFF, 0xFFFFFFFF );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 0x7FFFFFFF, 0xFFFFFFFF );
    i64Val2 = DRM_I64( 1 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 0x7FFFFFFF, 0xFFFFFFFF );
    i64Val2 = DRM_I64( 1 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( 0x80000000, 1 );
    i64Val2 = DRM_I64( -1 );
    ChkDR( DRM_Int64Mult( i64Val1, i64Val2, &i64Result ) );
    ChkBOOL( DRM_I64Eql( i64Result, DRM_I64Mul( i64Val1, i64Val2 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    i64Val1 = DRM_I64Asgn( -1234, 4567 );
    i64Val2 = DRM_I64Asgn( -1234, 7654 );
    dr = DRM_Int64Mult( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64Asgn( 0x40000000, 0 );
    i64Val2 = DRM_I64( 2 );
    dr = DRM_Int64Mult( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64( -2 );
    i64Val2 = DRM_I64Asgn( 0x40000000, 1 );
    dr = DRM_Int64Mult( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64Asgn( 0xBFFFFFFF, 0xFFFFFFFF );
    i64Val2 = DRM_I64( 2 );
    dr = DRM_Int64Mult( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;
    i64Val1 = DRM_I64Asgn( 0xC0000000, 0 );
    i64Val2 = DRM_I64( -2 );
    dr = DRM_Int64Mult( i64Val1, i64Val2, &i64Result );
    ChkBOOL( dr == DRM_E_ARITHMETIC_OVERFLOW, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dr = DRM_SUCCESS;

 ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(mathsafetest)
    API_ENTRY(TestMathSafeConversion)
    API_ENTRY(TestMathSafeSubtraction)
    API_ENTRY(TestMathSafeAddition)
    API_ENTRY(TestMathSafeMultiplication)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

