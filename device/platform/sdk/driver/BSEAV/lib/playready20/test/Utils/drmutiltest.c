/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmbase64.h"
#include "drmcontract.h"
#include <utiltestapi.h>


ENTER_PKTEST_NAMESPACE_CODE

DRM_BYTE* g_buf[MAX_BUFF];
DRM_BYTE g_bufUsed; /* This is a bitmap to denote g_buf usage. If bit i is 1, then PostTestCase will free g_buf[i]*/

static DRM_BYTE *GetBuf( __in_z_opt char *szIndex )
{
    if (szIndex) {
        int i = atoi(szIndex);
        if (i >= 0 && i < MAX_BUFF)
            return g_buf[i];
    }
    return NULL;
}

/* Allocate a global buffer
    argv[0]: the index to the buffer to be allocated, from 0 to MAX_BUFF - 1
    argv[1]: the buffer size, in bytes or wchars
    argv[2]: how to initialize the allocated buffer: NULL, EMPTY, RANDOM, COOKIE, literal strings
    argv[3]: optional, "Wide" when the buffer is used as array of WCHARs
*/
DRM_RESULT DRM_CALL TestAllocBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr    = DRM_SUCCESS;
    int        index = 0;
    DRM_DWORD  cbBuf = 0;
    DRM_BOOL   fWide = FALSE;

    ChkArg(argc >= 3 && argv[0] && argv[1]);
    if (argc > 3 && argv[3] && !strcmp(argv[3], "Wide"))
        fWide = TRUE;

    index = atoi(argv[0]);
    ChkArg(index >= 0 && index < MAX_BUFF);

    cbBuf = strtoul( argv[1], NULL, 0 );
    ChkArg( cbBuf > 0 );

    if (fWide)
    {
        ChkOverflow( cbBuf * SIZEOF( DRM_WCHAR ), cbBuf );
        cbBuf *= SIZEOF(DRM_WCHAR);
    }

    SAFE_OEM_FREE(g_buf[index]);
    g_bufUsed |= (1<<index);
    ChkMem(g_buf[index] = (DRM_BYTE*)Oem_MemAlloc(cbBuf + 2));

    /* Null terminate it */
    g_buf[index][cbBuf] = 0;
    g_buf[index][cbBuf+1] = 0;

    if (!argv[2] || !strcmp(argv[2], "COOKIE"))
    {
        memset(g_buf[index], argv[2]? COOKIECHAR: 0, cbBuf);
    }
    else if (!strcmp(argv[2], "RANDOM"))
    {
        ChkDR(Oem_Random_GetBytes(NULL, g_buf[index], cbBuf));
    }
    else if (fWide)
    {
        TST_UTL_mbstowcs((DRM_WCHAR*)g_buf[index], argv[2], cbBuf / sizeof(DRM_WCHAR));
    }
    else
    {
        memcpy(g_buf[index], argv[2], cbBuf);
    }
ErrorExit:
    return dr;
}

/* Compare the contents of two global buffers. Returns DRM_SUCCESS if they match.
    argv[0]: the index to the first buffer
    argv[1]: the index to the second buffer
*/
DRM_RESULT DRM_CALL TestCompBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pb1 = NULL;
    DRM_BYTE *pb2 = NULL;
    DRM_DWORD len = 0;

    ChkArg( argc == 3 && argv[2] );
    ChkArg( ( pb1 = GetBuf(argv[0]) ) != NULL );
    ChkArg( ( pb2 = GetBuf(argv[1]) ) != NULL );

    len = atol(argv[2]);

    dr = memcmp(pb1, pb2, len) ? DRM_S_FALSE: DRM_SUCCESS;

ErrorExit:
    return dr;
}

/* Set a global buffer pointer to a memory location in another buffer
    argv[0]: the index to the target buffer to change
    argv[1]: the index to the source buffer as a base. If NULL, the target pointer is set to NULL.
    argv[2]: the offset in bytes from the base buffer
*/
DRM_RESULT DRM_CALL TestSetBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    int iTarget, iSource;
    DRM_DWORD dwOffset;

    ChkArg(argc == 3 && argv[0] && argv[2]);

    iTarget = atoi(argv[0]);
    ChkArg(iTarget >= 0 && iTarget < MAX_BUFF);

    if (argv[1]) {
        iSource = atoi(argv[1]);
        ChkArg(iSource >= 0 && iSource < MAX_BUFF);

        dwOffset = atol(argv[2]);

        g_buf[iTarget] = g_buf[iSource] + dwOffset;
    } else
        g_buf[iTarget] = NULL;
ErrorExit:
    return dr;
}

static DRM_BOOL _memcmpwm(DRM_BYTE *pw, DRM_BYTE *pm, DRM_DWORD cch, DRM_BOOL fpwIsWide)
{
    DRM_BOOL ret;
    if (fpwIsWide) {
        DRM_WCHAR *wsz = NULL;
        DRM_DWORD cchwsz;

        cchwsz = ( DRM_DWORD ) TST_UTL_mbstowcs(NULL, (const char *)pm, cch);
        wsz = (DRM_WCHAR*)Oem_MemAlloc((cchwsz + 1) * sizeof(DRM_WCHAR));
        TST_UTL_mbstowcs(wsz, (const char *)pm, cch);
        ret = memcmp(pw, wsz, cchwsz * sizeof(DRM_WCHAR));
        SAFE_OEM_FREE(wsz);
    } else
        ret = memcmp(pw, pm, cch);
    return ret;
}

/* Verify the content of a global buffer. Returns DRM_SUCCESS if the buffer matches the content.
    argv[0]: the index to the buffer to verify
    argv[1]: the size of the buffer to verify
    argv[2]: the content to be verify
    argv[3]: optional. If "Wide" then argv[2] is a wide string
*/
DRM_RESULT DRM_CALL TestVerifyBuffer(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pBuf;
    DRM_BOOL fWide = FALSE;
    int cbBuf;

    ChkArg(argc >= 3 && argv[1]);

    if (argc > 3 && argv[3] && !strcmp(argv[3], "Wide"))
        fWide = TRUE;

    pBuf = GetBuf(argv[0]);
    cbBuf = atoi(argv[1]);

    if (!argv[2] && pBuf || (argv[2] && !pBuf)) {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    if (pBuf && argv[2] && _memcmpwm(pBuf, (DRM_BYTE *)argv[2], cbBuf, fWide))
        dr = DRM_S_FALSE;

ErrorExit:
    return dr;
}

/* Test API DRMCRT_memmove
    argv[0]: index to the target buffer.
    argv[1]: index to the source buffer.
    argv[2]: the size to move in bytes
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_memmove(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pb1, *pb2;
    DRM_DWORD len;

    ChkArg(argc == 3 && argv[2]);
    pb1 = GetBuf(argv[0]);
    pb2 = GetBuf(argv[1]);
    len = atol(argv[2]);
    DRMCRT_memmove(pb1, pb2, len);
ErrorExit:
    return dr;
}

/* Test API DRMCRT_wcsntol
    argv[0]: the string to convert to long.
    argv[1]: the expected long value.
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcsntol(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_LONG lExpValue, lRetValue;
    DRM_CONST_STRING szIn;
    memset(&szIn, 0, sizeof(szIn));

    ChkArg(argc == 2);
    ChkDR( MakeDRMString(&szIn, argv[0]) );
    lExpValue = atol(argv[1]);

    ChkDR(DRMCRT_wcsntol(szIn.pwszString, szIn.cchString, &lRetValue));
    if (lRetValue != lExpValue)
        dr = DRM_S_FALSE;
ErrorExit:
    FREEDRMSTRING( szIn );
    return dr;
}

/* Test API DRMCRT_strncmp. Returns DRM_SUCCESS if it returns expected value
    argv[0]: index to the first string
    argv[1]: index to the second string
    argv[2]: string length to compare, in chars
    argv[3]: expected return value: Positive, Negative or Zero
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_strncmp(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *sz1, *sz2;
    DRM_DWORD count;
    DRM_LONG lRet;

    ChkArg(argc == 4 && argv[2] && argv[3]);
    sz1 = (DRM_CHAR*)GetBuf(argv[0]);
    sz2 = (DRM_CHAR*)GetBuf(argv[1]);
    count = atol(argv[2]);

    lRet = DRMCRT_strncmp(sz1, sz2, count);
    if ((strcmp(argv[3], "Positive") || lRet <= 0) &&
        (strcmp(argv[3], "Negative") || lRet >= 0) &&
        (strcmp(argv[3], "Zero") || lRet != 0))
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}

/* Test API DRMCRT_wcsncmp. Returns DRM_SUCCESS if it returns expected value
    argv[0]: index to the first string
    argv[1]: index to the second string
    argv[2]: string length to compare, in wchars
    argv[3]: expected return value: Positive, Negative or Zero
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcsncmp(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR *wsz1, *wsz2;
    DRM_DWORD count;
    DRM_LONG lRet;

    ChkArg(argc == 4 && argv[2] && argv[3]);
    wsz1 = (DRM_WCHAR*)GetBuf(argv[0]);
    wsz2 = (DRM_WCHAR*)GetBuf(argv[1]);
    count = atol(argv[2]);

    lRet = DRMCRT_wcsncmp(wsz1, wsz2, count);
    if ((strcmp(argv[3], "Positive") || lRet <= 0) &&
        (strcmp(argv[3], "Negative") || lRet >= 0) &&
        (strcmp(argv[3], "Zero") || lRet != 0))
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}

/* Test API DRMCRT_wcslen. Returns DRM_SUCCESS if the function returns the expected length.
    argv[0]: index to the wide string buffer
    argv[1]: the expected returned length
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_wcslen(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR *wsz;
    DRM_DWORD cchExpected;

    ChkArg(argc == 2 && argv[1]);
    wsz = (DRM_WCHAR*)GetBuf(argv[0]);
    cchExpected = atol(argv[1]);

    if (cchExpected != DRMCRT_wcslen(wsz))
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}

/* Test API DRM_STR_StringCchCopyW.
    argv[0]: index to the destination string buffer
    argv[1]: index to the source string buffer
    argv[2]: the length to copy, in wchars
*/
DRM_RESULT DRM_CALL Test_API_StringCchCopyW(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR *wszDest = NULL;
    DRM_WCHAR *wszSource = NULL;
    DRM_DWORD cchCopy = 0;

    ChkArg(argc == 3 && argv[2]);
    ChkArg( ( wszDest = (DRM_WCHAR*)GetBuf(argv[0]) ) != NULL );
    ChkArg( ( wszSource = (DRM_WCHAR*)GetBuf(argv[1]) ) != NULL );
    cchCopy = atol(argv[2]);

    ChkDR( DRM_STR_StringCchCopyW( wszDest, cchCopy, wszSource ) );

ErrorExit:
    return dr;
}

/* Test API DRMCRT_abs. Returns DRM_SUCCESS if the function returns the expected value.
    argv[0]: the number to pass to the DRMCRT_abs function
    argv[1]: the expected return value
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_abs(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG lRet;
    ChkArg(argc == 2 && argv[0] && argv[1]);
    lRet = DRMCRT_abs(atoi(argv[0]));
    if (lRet != atoi(argv[1]))
        dr = DRM_S_FALSE;
ErrorExit:
    return dr;
}

/*
    Test these functions work the same as the CRT equivalents:
    DRMCRT_iswspace, DRMCRT_iswxdigit, DRMCRT_iswdigit, DRMCRT_iswalpha, DRMCRT_towlower
    argv[0]: the first wchar to test
    argv[1]: the last wchar to test
*/
DRM_RESULT DRM_CALL Test_API_DRMCRT_isxxx(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR chFrom, chTo, chNow;
    ChkArg(argc == 2 && argv[0] && argv[1]);
    chFrom = (DRM_WCHAR)atol(argv[0]);
    chTo = (DRM_WCHAR)atol(argv[1]);

    for (chNow = chFrom; DRM_SUCCESS == dr && chNow < chTo; chNow++) {
        dr = DRM_S_FALSE;
        if (!DRMCRT_iswspace(chNow) != !iswspace(chNow)) {
            Log("DRMCRT_iswspace", "DRMCRT_iswspace and iswspace returns different values for %d", chNow);
        } else if (!DRMCRT_iswxdigit(chNow) != !iswxdigit(chNow)) {
            Log("DRMCRT_iswxdigit", "DRMCRT_iswxdigit and iswxdigit returns different values for %d", chNow);
        } else if (!DRMCRT_iswdigit(chNow) != !iswdigit(chNow)) {
            Log("DRMCRT_iswdigit", "DRMCRT_iswdigit and iswdigit returns different values for %d", chNow);
        } else if (!DRMCRT_iswalpha(chNow) != !iswalpha(chNow)) {
            Log("DRMCRT_iswalpha", "DRMCRT_iswalpha and iswalpha returns different values for %d", chNow);
        } else if (DRMCRT_towlower(chNow) != DRMCRT_towlower(chNow)) {
            Log("DRMCRT_towlower", "DRMCRT_towlower and DRMCRT_towlower returns different values for %d", chNow);
        } else
            dr = DRM_SUCCESS;
    }
ErrorExit:
    return dr;
}

/* Test API DRM_B64_Encode
    argv[0]: ANSI or WIDE
    argv[1]: index to the binary buffer to encode
    argv[2]: the binary buffer length
    argv[3]: index to the buffer to store the encoded string, can be NULL
    argv[4]: the initial value for the size of the encoded string
    argv[5]: the expected encoded string length
*/
DRM_RESULT DRM_CALL Test_API_DRM_Base64Encode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr            = DRM_E_FAIL;
    DRM_BYTE   *pBuf         = NULL;
    DRM_WCHAR  *pwszEncoded  = NULL;
    DRM_CHAR   *pszEncoded   = NULL;
    DRM_DWORD  cchEncoded    = 0;
    DRM_DWORD  cchMaxEncoded = 0;

    ChkArg(argc == 6 && argv[0]);
    pBuf = GetBuf(argv[1]);
    if (argv[4])
    {
        cchEncoded = cchMaxEncoded = atol(argv[4]);
    }

    if (!strcmp(argv[0], "ANSI")) {
        pszEncoded = (DRM_CHAR*)GetBuf(argv[3]);
        ChkDR( DRM_B64_EncodeA( pBuf,
                                atol(argv[2]),
                                pszEncoded,
                                argv[4]? &cchEncoded: NULL,
                                0 ) );
    } else {
        pwszEncoded = (DRM_WCHAR*)GetBuf(argv[3]);
        ChkDR( DRM_B64_EncodeW( pBuf,
                                atol(argv[2]),
                                pwszEncoded,
                                argv[4]? &cchEncoded: NULL,
                                0 ) );
    }
    if (argv[4]) {
        if (argv[5] && cchEncoded !=(DRM_DWORD) atol(argv[5]))
            dr = DRM_S_FALSE;
        if (pwszEncoded && cchEncoded < cchMaxEncoded)
            pwszEncoded[cchEncoded] = 0;
        if (pszEncoded && cchEncoded < cchMaxEncoded)
            pszEncoded[cchEncoded] = 0;
    }

ErrorExit:
    return dr;
}

/* Test API DRM_B64_Decode
    argv[0]: ANSI or WIDE
    argv[1]: index to the encoded string, can be NULL
    argv[2]: the initial value for the size of the decoded binary buffer
    argv[3]: index to the buffer to store the decoded binary
    argv[4]: the dwFlag value passed to the function
    argv[5]: the expected decoded binary length, can be NULL
*/
DRM_RESULT DRM_CALL Test_API_DRM_Base64Decode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_E_FAIL;
    DRM_CONST_STRING wszSource;
    DRM_ANSI_CONST_STRING dastrSource  = EMPTY_DRM_STRING;
    DRM_SUBSTRING         dasstrSource = { 0 };
    DRM_DWORD cbDest = 0;
    DRM_BYTE *pbDest = NULL;
    DRM_DWORD dwFlags = 0;

    ChkArg(argc == 6 && argv[0]);

    if (argv[2])
        cbDest = atol(argv[2]);

    pbDest = GetBuf(argv[3]);
    dwFlags = atol(argv[4]);

    if (!strcmp(argv[0], "ANSI")) {
        if ( ( dastrSource.pszString = (DRM_CHAR*)GetBuf(argv[1]) ) != NULL )
        {
            dastrSource.cchString = ( DRM_DWORD ) strlen(dastrSource.pszString);
        }

       dasstrSource.m_ich = 0;
       dasstrSource.m_cch = dastrSource.cchString;

        dr = DRM_B64_DecodeA(
            argv [1] ?  dastrSource.pszString : NULL,
            argv [1] ? &dasstrSource          : NULL,
            argv [2] ? &cbDest                : NULL,
            pbDest, dwFlags);
    } else {
        if ( ( wszSource.pwszString = (DRM_WCHAR*)GetBuf(argv[1]) ) != NULL )
            wszSource.cchString = ( DRM_DWORD ) DRMCRT_wcslen(wszSource.pwszString);

        dr = DRM_B64_DecodeW(
            argv[1]? &wszSource: NULL,
            argv[2]? &cbDest: NULL,
            pbDest, dwFlags);
    }

    if (argv[2] && argv[5] && cbDest !=(DRM_DWORD) atol(argv[5]))
        dr = DRM_S_FALSE;

ErrorExit:
    return dr;
}

/*    Test API DRM_UTL_DecodeKID
    argv[0]: KID string
    argv[1]: index to the buffer to store the decoded kid
*/
DRM_RESULT DRM_CALL Test_API_DecodeKID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING dstrKID = {0};
    DRM_KID *pKID = NULL;
    ChkArg(argc == 2);

    ChkDR( MakeDRMString(&dstrKID, argv[0]) );
    pKID = (DRM_KID*)GetBuf(argv[1]);

    ChkDR(DRM_UTL_DecodeKID(&dstrKID, pKID));
ErrorExit:
    FREEDRMSTRING( dstrKID );
    return dr;
}

/*********************************************************
**
** Function: TestUtilAreGUIDEqual
**
** Synopsis: This tests the dev API DRM_UTL_AreGUIDEqual.
** It takes in two GUIDs and an expected result (true or
** false).  It also takes an optional argument to pass NULL
** pointers into the API.
**
** Arguments:
**  argc    -- must be between 3 and 5
**  argv[0] -- first GUID
**  argv[1] -- second GUID
**  argv[2] -- expected result (TRUE or FALSE)
**  argv[3] -- optional flag to pass NULL pointer as GUID 1 (default is FALSE)
**  argv[4] -- optional flag to pass NULL pointer as GUID 2 (default is FALSE)
**
*********************************************************/
DRM_RESULT DRM_CALL TestUtilAreGUIDEqual(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_CONST_STRING dstrGuid1       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrGuid2       = EMPTY_DRM_STRING;
    DRM_BOOL         fExpectedResult = TRUE;
    DRM_BOOL         fUseNullGuid1   = FALSE;
    DRM_BOOL         fUseNullGuid2   = FALSE;
    DRM_GUID         dguid1          = {0,0,0, {0} };
    DRM_GUID         dguid2          = {0,0,0, {0} };

    /* Check arguments */
    ChkArg( argc >= 3 );
    ChkArg( argc <= 5 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    /* Convert input guids to DRM_CONST_STRINGs */
    ChkDR( MakeDRMString( &dstrGuid1, argv[0] ) );
    ChkDR( MakeDRMString( &dstrGuid2, argv[1] ) );

    /* Convert the DRM_CONST_STRING GUIDS to DRM_GUID */
    ChkDR( DRM_UTL_StringToGuid( &dstrGuid1, &dguid1 ) );
    ChkDR( DRM_UTL_StringToGuid( &dstrGuid2, &dguid2 ) );

    /* Convert expected result to DRM_BOOL */
    ChkDR( StringToBool( argv[2], &fExpectedResult ) );

    /* Check for optional arguments */
    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        ChkDR( StringToBool( argv[3], &fUseNullGuid1 ) );
    }
    if ( ( argc == 5 ) && ( argv[4] != NULL ) )
    {
        ChkDR( StringToBool( argv[4], &fUseNullGuid2 ) );
    }

    /* Check for GUID equality */
    ChkBOOL( fExpectedResult == DRM_UTL_AreGUIDEqual( fUseNullGuid1? NULL : &dguid1, fUseNullGuid2? NULL : &dguid2 ) , DRM_E_FAIL );

ErrorExit:
    FREEDRMSTRING( dstrGuid1 );
    FREEDRMSTRING( dstrGuid2 );
    return dr;
}

/*********************************************************
**
** Function: TestUtilGUIDToString
**
** Synopsis: This tests the dev API DRM_UTL_GUIDToString using
** the older dev API DRM_UTL_StringToGUID to convert an input
** string into a GUID, passing that GUID into the API to test,
** and then comparing the output string to the original.  This
** also takes Boolean flag inputs to indicate invalid inputs to
** pass to the API, and output files to dump the before and after
** strings to.
**
** Arguments:
**  argc    -- at least 1, no greater than 3
**  argv[0] -- GUID (as a string) to pass to DRM_UTL_GUIDToString
**  argv[1] -- optional Boolean flag indicating that null string buffer should be passed to API (default is FALSE)
**  argv[2] -- optional Boolean flag indicating that null GUID pointer should be passed to the API (default is FALSE)
**
*********************************************************/
DRM_RESULT DRM_CALL TestUtilGUIDToString(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING dstrGuid          = EMPTY_DRM_STRING;
    DRM_STRING       dstrOutput        = EMPTY_DRM_STRING;
    DRM_BOOL         fUseNullBuffer    = FALSE;
    DRM_BOOL         fUseNullGuid      = FALSE;
    DRM_GUID         dguid             = {0,0,0, {0} };
    DRM_WCHAR        rgwszOutput[DRM_GUID_STRING_LEN] = {0};

    /* Check arguments */
    ChkArg( argc >= 1 );
    ChkArg( argc <= 3 );
    ChkArg( argv[0] != NULL );

    /* Convert input guid to DRM_CONST_STRINGs */
    ChkDR( MakeDRMString( &dstrGuid, argv[0] ) );

    /* Convert the DRM_CONST_STRING GUID to DRM_GUID */
    ChkDR( DRM_UTL_StringToGuid( &dstrGuid, &dguid ) );

    /* Check for optional arguments */
    if ( (argc >= 2) && (argv[1] != NULL) )
    {
        ChkDR( StringToBool( argv[1], &fUseNullBuffer ) );
    }
    if ( (argc == 3) && (argv[2] != NULL) )
    {
        ChkDR( StringToBool( argv[2], &fUseNullGuid ) );
    }

    /* Call DRM_UTL_GUIDToString with specified inputs */
    ChkDR( DRM_UTL_GuidToString( fUseNullGuid? NULL : &dguid,
                                 fUseNullBuffer? NULL : rgwszOutput) );

    dstrOutput.pwszString = rgwszOutput;
    dstrOutput.cchString = DRM_GUID_STRING_LEN;

    /* Compare output to original string */
    ChkBOOL( DRM_UTL_DSTRStringsEqual( &dstrGuid, (DRM_CONST_STRING*)&dstrOutput), DRM_E_FAIL );

ErrorExit:
    FREEDRMSTRING( dstrGuid );
    return dr;
}

/*********************************************************
**
** Function: TestUtilReadGUID
**
** Synopsis: This tests the API DRM_UTL_ReadGUID.  It takes
** a GUID and a specified offset, converts these into an
** appropriate DRM_BYTE array, and passes them into the API,
** comparing the output with the original GUID.  This also
** takes optional  flags to indicate invalid inputs.  Due to
** the nature of DRM_UTL_ReadGUID, this MUST be tested on both
** little endian and big endian platforms.
**
** Arguments:
**  argc    -- must be 1-5
**  argv[0] -- GUID (as a string) to be read
**  argv[1] -- optional offset of DRM_BYTE array (default is 0)
**  argv[2] -- optional flag indicating that a NULL DRM_BYTE array should be passed to DRM_UTL_ReadGUID (default is FALSE)
**  argv[3] -- optional flag indicating that a NULL DRM_GUID output pointer should be passed to DRM_UTL_ReadGUID (default is FALSE)
**  argv[4] -- optional change in size of DRM_BYTE array (default is 0; Positive number will cause array to be bigger than <offset + size of GUID>, negative numbers will cause array to be smaller)
**
*********************************************************/
DRM_RESULT DRM_CALL TestUtilReadGUID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_CONST_STRING dstrGuid         = EMPTY_DRM_STRING;
    DRM_BYTE         *pbData          = NULL;
    DRM_DWORD        dwGuidOffset     = 0;
    DRM_LONG         iArraySizeChange = 0;
    DRM_BOOL         fUseNullArray    = FALSE;
    DRM_BOOL         fUseNullGuid     = FALSE;
    DRM_GUID         dguidIn          = {0,0,0, {0} };
    DRM_GUID         dguidOut         = {0,0,0, {0} };

    /* Check arguments */
    ChkArg( argc >= 1 );
    ChkArg( argc <= 5 );
    ChkArg( argv[0] != NULL );

    /* Check for optional arguments */
    if ( ( argc >= 2 ) && ( argv[1] != NULL ) )
    {
        ChkArg(IsDigitalOnlyString( argv[1] ) );
        dwGuidOffset = (DRM_DWORD) atoi( argv[1] );
    }
    if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
    {
        ChkDR( StringToBool(argv[2], &fUseNullArray ) );
    }
    if ( ( argc >= 4 ) && ( argv[3] != NULL ) )
    {
        ChkDR( StringToBool( argv[3], &fUseNullGuid ) );
    }
    if ( ( argc == 5 ) && ( argv[4] != NULL ) )
    {
        ChkArg(IsDigitalOnlyString( argv[4] ) );
        iArraySizeChange = atoi( argv[4] );
    }

    /* Convert input guid */
    ChkDR( MakeDRMString( &dstrGuid, argv[0] ) );
    ChkDR( DRM_UTL_StringToGuid( &dstrGuid, &dguidIn ) );

    /* Allocate memory for byte array to hold the GUID */
    ChkMem( pbData = (DRM_BYTE *)Oem_MemAlloc( SIZEOF(DRM_GUID) + dwGuidOffset + iArraySizeChange ) );
    OEM_SECURE_ZERO_MEMORY( pbData, SIZEOF(DRM_GUID) + dwGuidOffset + iArraySizeChange );

    /* Copy Guid to BYTE array with specified offset */
    if ( iArraySizeChange <= 0 )
    {
        DRM_BYT_CopyBytes( pbData, dwGuidOffset, &dguidIn, 0, SIZEOF(DRM_GUID) + iArraySizeChange );
    }
    else
    {
        DRM_BYT_CopyBytes( pbData, dwGuidOffset, &dguidIn, 0, SIZEOF(DRM_GUID) );
    }

    /* Read GUID from byte array using specified arguments */
    ChkDR( DRM_UTL_ReadGUID( fUseNullArray? NULL : pbData,
                             SIZEOF(DRM_GUID) + dwGuidOffset + iArraySizeChange,
                             dwGuidOffset,
                             fUseNullGuid? NULL : &dguidOut ) );

    /* Compare GUID read from array to original GUID */
    ChkBOOL( DRM_UTL_AreGUIDEqual( &dguidIn, &dguidOut), DRM_E_FAIL );

ErrorExit:
    FREEDRMSTRING( dstrGuid );
    SAFE_OEM_FREE( pbData );
    return dr;
}

/*********************************************************
**
** Function: TestCRTStringCchLengthW
**
** Synopsis: This tests the dev API DRM_STR_StringCchLengthW.
** It takes a string to pass into the API, a max value of the
** length of the string to measure, an optional flag indicating
** that the string to pass in should be NULL terminated (default
** is TRUE), and optional flags indicating that a NULL pointers
** be passed into the API.
**
** Arguments:
**  argc    -- must be at least 2, no greater than 5
**  argv[0] -- string to pass to API
**  argv[1] -- max value to pass to API
**  argv[2] -- optional flag indicating that string passed to API is NULL terminated (default is TRUE)
**  argv[3] -- optional flag indicating that NULL pointer is passed into the API as the string to measure (default is FALSE)
**  argv[4] -- optional flag indicating that NULL pointer is passed into the API as the output DWORD pointer
**
*********************************************************/
DRM_RESULT DRM_CALL TestCRTStringCchLengthW(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_CONST_STRING dstrString      = EMPTY_DRM_STRING;
    DRM_DWORD        dwMaxValue      = 0;
    DRM_DWORD        dwOutput        = 0;
    DRM_BOOL         fNullTerminated = TRUE;
    DRM_BOOL         fUseNullString  = FALSE;
    DRM_BOOL         fUseNullOutput  = FALSE;

    /* Check required arguments */
    ChkArg( argc >= 2 );
    ChkArg( argc <= 5 );
    ChkArg( argv[0] != NULL );
    ChkArg( IsDigitalOnlyString( argv[1] ) );

    /* Read required arguments */
    ChkDR( MakeDRMString( &dstrString, argv[0] ) );
    dwMaxValue = (DRM_DWORD) atoi( argv[1] );

    /* Check and read optional arguments */
    if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
    {
        ChkDR( StringToBool( argv[2], &fNullTerminated ) );
    }
    if ( ( argc >= 4 ) && ( argv[3] != NULL ) )
    {
        ChkDR( StringToBool( argv[3], &fUseNullString ) );
    }
    if ( ( argc == 5 ) && ( argv[4] != NULL ) )
    {
        ChkDR( StringToBool( argv[4], &fUseNullOutput ) );
    }

    /* Test getting the string length */
    ChkDR( DRM_STR_StringCchLengthW( fUseNullString? NULL : dstrString.pwszString,
                                    dwMaxValue,
                                    fUseNullOutput? NULL : &dwOutput ) );

    /* Compare output to expectation */
    if ( fNullTerminated && ( dstrString.cchString < dwMaxValue ) )
    {
        ChkBOOL( dwOutput == dstrString.cchString, DRM_E_FAIL );
    }
    else
    {
        ChkBOOL( dwOutput == dwMaxValue, DRM_E_FAIL );
    }

ErrorExit:
    FREEDRMSTRING( dstrString );
    return dr;
}

/*********************************************************
**
** Function: TestCRTwcstr
**
** Synopsis: This tests the dev API DRMCRT_wcsnstr.  It takes
** two strings to pass to the API, a non-negative number
** indicating the starting position of the second string
** inside the first string (0 means it starts at the beginning)
** to be used to check the answer, and some optional flags
** indicating invalid inputs
**
** Arguments:
**  argc    -- must be at least 3, no greater than 6
**  argv[0] -- the string to search within
**  argv[1] -- the string to search for
**  argv[2] -- the starting location, in units of characters, of the second string inside the first string.  If this equals 0, then the first string starts with the second string.  If this is greater than the length of the first string, then the second string is not a substring of the first string
**  argv[3] -- optional flag indicating that first string passed to API should be NULL (default is FALSE)
**  argv[4] -- optional flag indicating that the second string passed into the API should be NULL (default to FALSE)
**  argv[5] -- optional flag indicating that the expected output should be NULL (default to FALSE)
**
*********************************************************/
DRM_RESULT DRM_CALL TestCRTwcstr(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                  = DRM_SUCCESS;
    DRM_CONST_STRING dstrString          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSubString       = EMPTY_DRM_STRING;
    DRM_DWORD        dwSubStringLoc      = 0;
    DRM_WCHAR        *pwszExpectedOutput = NULL;
    DRM_WCHAR        *pwszOutput         = NULL;
    DRM_BOOL         fUseNullString      = FALSE;
    DRM_BOOL         fUseNullSubString   = FALSE;
    DRM_BOOL         fExpectNullOutput   = FALSE;
    DRM_DWORD        i                   = 0;

    /* Check required arguments */
    ChkArg( argc >= 3 );
    ChkArg( argc <= 6 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( IsDigitalOnlyString( argv[2] ) );

    /* Read required arguments */
    ChkDR( MakeDRMString( &dstrString, argv[0] ) );
    ChkDR( MakeDRMString( &dstrSubString, argv[1] ) );
    dwSubStringLoc = (DRM_DWORD) atoi( argv[2] );

    /* Determine expected output (based on argv[2]) */
    if ( dwSubStringLoc < dstrString.cchString )
    {
        pwszExpectedOutput = (DRM_WCHAR*)dstrString.pwszString;
        for( i=0; i<dwSubStringLoc; i++ )
        {
            pwszExpectedOutput++;
        }
    }

    /* Check and read optional arguments */
    if ( ( argc >= 4 ) && ( argv[3] != NULL ) )
    {
        ChkDR( StringToBool( argv[3], &fUseNullString ) );
    }
    if ( ( argc >= 5 ) && ( argv[4] != NULL ) )
    {
        ChkDR( StringToBool( argv[4], &fUseNullSubString ) );
    }

    if ( ( argc == 6 ) && ( argv[5] != NULL ) )
    {
        ChkDR( StringToBool( argv[5], &fExpectNullOutput ) );
    }

    /* Test getting the location of the substring in the string */
    pwszOutput =  DRMCRT_wcsnstr( fUseNullString? NULL : dstrString.pwszString,
                                 dstrString.cchString,
                                 fUseNullSubString? NULL : dstrSubString.pwszString,
                                 dstrSubString.cchString );

    /* Compare output to expectation */
    if ( fExpectNullOutput )
    {
        ChkBOOL( pwszOutput == NULL, DRM_E_FAIL );
    }
    else
    {
        ChkBOOL( pwszOutput == pwszExpectedOutput, DRM_E_FAIL );
    }

ErrorExit:
    pwszExpectedOutput = NULL;
    pwszOutput = NULL;
    FREEDRMSTRING( dstrString );
    FREEDRMSTRING( dstrSubString );
    return dr;
}

/*********************************************************
**
** Function: TestCRTCchSPrintFSingleParam
**
** Synopsis: This tests the dev API DRMCRT_Cchsprintf_SingleParam.
** It takes a string with a tag inside it, the same string with
** the tag replaced by a decimal DWORD, and a (decimal) DWORD.  It
** converts the first string into a DRM_CONST_STRING, passes it and
** the DWORD into the API, and then compares the result with the
** second string.
**
** Arguments:
**  argc    -- 3 < argc < 7
**  argv[0] -- a string with a tag inside (if no tag is included, then this should match the next argument.  The test API will not be checking for this though, so be careful or the test results will show false test failures)
**  argv[1] -- the same string as previous arg, but with tag replaced by DWORD.  This is the expected output of the dev API
**  argv[2] -- the DWORD to pass to the dev API
**  argv[3] -- size of the output string buffer to pass to the API.
**  argv[4] -- optional flag indicating output string buffer should be NULL (default is FALSE)
**  argv[5] -- optional flag indicating that output string size pointer is NULL (default is FALSE)
**
*********************************************************/
DRM_RESULT DRM_CALL TestCRTCchSPrintFSingleParam(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_CONST_STRING dstrInString       = EMPTY_DRM_STRING;
    DRM_STRING       dstrOutString      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExpectedOutput = EMPTY_DRM_STRING;
    DRM_DWORD        dwValue            = 0;
    DRM_DWORD        cchMax             = 0;
    DRM_DWORD        dwCChFilled        = 0;
    DRM_BOOL         fUseNullBuffer     = FALSE;
    DRM_BOOL         fUseNullSize       = FALSE;

    /* Check required arguments */
    ChkArg( argc >= 4 );
    ChkArg( argc <= 6 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
    ChkArg( IsDigitalOnlyString( argv[3] ) );

    /* Read required arguments */
    ChkDR( MakeDRMString( &dstrInString, argv[0] ) );
    ChkDR( MakeDRMString( &dstrExpectedOutput, argv[1] ) );
    dwValue = (DRM_DWORD) strtoul( argv[2], (char **)NULL, 0 );
    cchMax = (DRM_DWORD) strtoul( argv[3], (char **)NULL, 0 );

    /*
    ** Check that the expected output is not bigger than the specifed output buffer.
    ** For testing small output buffers (< DRMCRT_MAX_DWORD_STRING + input size - 2),
    ** expected output won't matter, so just be sure to use a small enough value for
    ** the expected output so that this ChkArg passes.
    */
    ChkArg( cchMax >= dstrExpectedOutput.cchString );

    /* Allocate output buffer */
    ChkMem( dstrOutString.pwszString = (DRM_WCHAR *)Oem_MemAlloc( SIZEOF(DRM_WCHAR) *  ( cchMax+1 ) ) );
    OEM_SECURE_ZERO_MEMORY( dstrOutString.pwszString, ( cchMax + 1 ) * SIZEOF( DRM_WCHAR )  ); /* zero out the output buffer */

    /* Check and read optional arguments */
    if ( ( argc >= 5 ) && ( argv[4] != NULL ) )
    {
        ChkDR( StringToBool( argv[4], &fUseNullBuffer ) );
    }
    if ( ( argc == 6 ) && ( argv[5] != NULL ) )
    {
        ChkDR( StringToBool( argv[5], &fUseNullSize ) );
    }

    /* Call DRMCRT_Cchsprintf_SingleParam */
    ChkDR( DRM_STR_StringCchPrintfExW_1( fUseNullBuffer? NULL : dstrOutString.pwszString,
                                          cchMax,
                                          NULL,
                                          fUseNullSize? NULL : &dwCChFilled,
                                          0,
                                          dstrInString.pwszString,
                                          dwValue ) );

    /* Compare output string to expected output */
    ChkBOOL( dstrExpectedOutput.pwszString == DRMCRT_wcsnstr( dstrExpectedOutput.pwszString, dstrExpectedOutput.cchString, dstrOutString.pwszString, dstrExpectedOutput.cchString ), DRM_E_FAIL );

ErrorExit:
    FREEDRMSTRING( dstrInString );
    FREEDRMSTRING( dstrOutString );
    FREEDRMSTRING( dstrExpectedOutput );
    return dr;
}

/*********************************************************
**
** Function: TestNumberToString
**
** Synopsis: This tests the DRM_UTL_NumberToString method
**
** Arguments:
**  argc    -- 5
**  argv[0] -- value to convert
**  argv[1] -- buffer size in DRM_WCHARs (NUL-terminator should be included)
**  argv[2] -- indicates if padding with zeros is used
**  argv[3] -- radix
**  argv[4] -- expected result, optional
**
**  Returns:
**  DRM_SUCCESS                  - if converted string is the same as expected result
**  DRM_E_TEST_UNEXPECTED_OUTPUT - otherwise
*********************************************************/

DRM_RESULT DRM_CALL TestNumberToString(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_DWORD        dwValue   = 0;
    DRM_BOOL         fPadZeros = TRUE;
    DRM_CONST_STRING strBuffer = EMPTY_DRM_STRING;
    DRM_DWORD        cchBuffer = 0;
    DRM_DWORD        dwRadix = 0;

    // 4 or 5 parameters are expected
    ChkArg( argc == 4 || argc == 5 );
    // 4 not null parameters are expected
    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );
    ChkArg( argv[ 2 ] != NULL );
    ChkArg( argv[ 3 ] != NULL );

    ChkArg( IsDigitalOnlyString( argv[0] ) );
    dwValue = strtoul( argv[0], (char **)NULL, 0 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );
    cchBuffer = strtoul( argv[1], (char **)NULL, 0 );
    ChkMem( strBuffer.pwszString = (DRM_WCHAR*)Oem_MemAlloc( max( 1, cchBuffer * sizeof(DRM_WCHAR) ) ) );
    ChkDR( StringToBool( argv[2], &fPadZeros ) );
    ChkArg( IsDigitalOnlyString( argv[3] ) );
    dwRadix = strtoul( argv[3], (char **)NULL, 0 );

    ChkDR( DRM_UTL_NumberToString( dwValue,
                                   (DRM_WCHAR*) strBuffer.pwszString,
                                   cchBuffer,
                                   fPadZeros ? cchBuffer-1 : 0,
                                   dwRadix,
                                   &(strBuffer.cchString) ) );

    if ( argc == 5 && argv[ 4 ] != NULL )
    {
        ChkBOOL( CompDRMString( &strBuffer, argv[ 4 ] ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    FREEDRMSTRING( strBuffer );
    return dr;
}

/*********************************************************
**
** Function: TestUTL_XMLEncode
**
**
** Arguments:
**  argc    -- must be between 4
**  argv[0] -- String to XML encode
**  argv[1] -- Boolean to indicate if this is an attribute
**  argv[2] -- expected results
**  argv[3] -- output buffer size to use
**
*********************************************************/
DRM_RESULT DRM_CALL TestUTL_XMLEncode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_BOOL         fAttributeText  = FALSE;
    DRM_CONST_STRING  dstrToEncode    = EMPTY_DRM_STRING;
    DRM_STRING        dstrEncoded     = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrShouldHaveEncoded = EMPTY_DRM_STRING;

    /* Check arguments */
    ChkArg( argc == 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );

    ChkDR( StringToBool( argv[1], &fAttributeText ) );
    ChkDR( MakeDRMString( &dstrToEncode, argv[0] ) );
    ChkDR( MakeDRMString( &dstrShouldHaveEncoded, argv[2] ) );

    ChkArg( IsDigitalOnlyString( argv[3] ) );
    dstrEncoded.cchString = strtoul( argv[3], (char **)NULL, 0 );
    if( dstrEncoded.cchString > 0 )
    {
        dstrEncoded.pwszString = (DRM_WCHAR*)Oem_MemAlloc(dstrEncoded.cchString  * SIZEOF(DRM_WCHAR));
    }

    ChkDR( DRM_UTL_XMLEncode(&dstrToEncode, fAttributeText, &dstrEncoded) );

    ChkFAIL( dstrEncoded.cchString == dstrShouldHaveEncoded.cchString );
    ChkFAIL( DRMCRT_wcsncmp( dstrEncoded.pwszString, dstrShouldHaveEncoded.pwszString, dstrEncoded.cchString ) == 0 );


ErrorExit:
    FREEDRMSTRING( dstrEncoded );
    FREEDRMSTRING( dstrToEncode );
    FREEDRMSTRING( dstrShouldHaveEncoded );
    return dr;
}


/*********************************************************
**
** Function: TestECCExpectedErrorCode
**
** Synopsis: This tests the dev API OEM_ECC_ExpectedErrorCode.
**
** Arguments:
**  argv[0] -- API ID (unsigned int, see drmcontract.h for list of ids)
**  argv[1] -- Return value (unsigned long int)
**  argv[2] -- Expected result, "TRUE" or "FALSE"
**
*********************************************************/
DRM_RESULT DRM_CALL TestECCExpectedErrorCode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_DWORD        uiAPIID            = 0;
    DRM_RESULT       drTestResult       = DRM_SUCCESS;
    DRM_BOOL         fExpectedResult    = TRUE;
    DRM_BOOL         fActualResult      = TRUE;

    /* If contract is not supported, there's nothing to test */
    if( DRM_CONTRACT_IsContractSupported() )
    {
        /* Check required arguments */
        ChkArg( argc == 3 );
        ChkArg( argv[0] != NULL );
        ChkArg( argv[1] != NULL );
        ChkArg( argv[2] != NULL );
        ChkArg( atoi( argv[0] ) >= 0 );

        /* Read required arguments */
        uiAPIID = (DRM_DWORD)atoi( argv[0] );
        drTestResult = strtoul( argv[1], (char **)NULL, 0 );
        ChkDR( StringToBool( argv[2], &fExpectedResult ) );

        /* Call OEM_ECC_ExpectedErrorCode */
        fActualResult = OEM_ECC_ExpectedErrorCode( uiAPIID, drTestResult );

        /* Compare output string to expected output */
        ChkBOOL( fExpectedResult == fActualResult, DRM_E_FAIL );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(UtilTest)(long lTCID, const char *strTCName)
{
    memset(g_buf, 0, MAX_BUFF * sizeof(DRM_BYTE*));
    g_bufUsed = 0;
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(UtilTest)(long lTCID, const char *strTCName)
{
    int i;
    for (i = 0; i < MAX_BUFF; i++)
        if ((g_bufUsed >> i) & 0x1)
            SAFE_OEM_FREE(g_buf[i]);
    return DRM_SUCCESS;
}


BEGIN_TEST_API_LIST( UtilTest )
    API_ENTRY(TestAllocBuffer)
    API_ENTRY(TestSetBuffer)
    API_ENTRY(TestCompBuffer)
    API_ENTRY(TestVerifyBuffer)
    API_ENTRY(Test_API_DRMCRT_memmove)
    API_ENTRY(Test_API_DRMCRT_wcsntol)
    API_ENTRY(Test_API_DRMCRT_strncmp)
    API_ENTRY(Test_API_DRMCRT_wcsncmp)
    API_ENTRY(Test_API_DRMCRT_wcslen)
    API_ENTRY(Test_API_StringCchCopyW)
    API_ENTRY(Test_API_DRMCRT_abs)
    API_ENTRY(Test_API_DRMCRT_isxxx)
    API_ENTRY(Test_API_DRM_Base64Encode)
    API_ENTRY(Test_API_DRM_Base64Decode)
    API_ENTRY(Test_API_DecodeKID)
    API_ENTRY(TestUtilAreGUIDEqual)
    API_ENTRY(TestUtilGUIDToString)
    API_ENTRY(TestUtilReadGUID)
    API_ENTRY(TestCRTStringCchLengthW)
    API_ENTRY(TestCRTwcstr)
    API_ENTRY(TestCRTCchSPrintFSingleParam)
    API_ENTRY(TestNumberToString)
    API_ENTRY(TestUTL_XMLEncode)
    API_ENTRY(TestECCExpectedErrorCode)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
