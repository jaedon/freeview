/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TSTUTILS_H_
#define _TSTUTILS_H_

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <PKTestNameSpace.h>


DRM_RESULT DRM_CALL _TextTimeToFileTime(
    const DRM_WCHAR   *pText,
          DRMFILETIME *pFT );

DRM_BOOL DRM_CALL tFileTimeToDWORD(
    __in DRMFILETIME *f_pfiletime,
    __out DRM_DWORD  *f_pdwTime );

DRM_RESULT DRM_CALL _CalcRelativeDate(       DRM_WCHAR     *pDest,
                                       const DRM_WCHAR     *pDateOffset,
                                             DRM_DWORD      destLen,
                                       const DRMSYSTEMTIME *pBaseSystemTime );

DRM_RESULT DRM_CALL _GenerateContentKey( DRM_CONST_STRING *pszServerSeed,
                                         DRM_CONST_STRING *pszKid,
                                         DRM_DWORD        *pcbClearKey,
                                         DRM_BYTE        **ppbClearContentKey,
                                         DRM_DWORD         cbCheckSum,
                                         DRM_BYTE         *pbCheckSum,
                                         DRM_DWORD         dwXmrVersion,
                                         DRM_BOOL          fAESCrypto );

DRM_RESULT tDRM_KG_GenerateContentKey2(
    __in  DRM_CONST_STRING *pszKeySeed,
    __in  DRM_CONST_STRING *pszKid,
    __out DRM_CONST_STRING *pszKey,
    __in  DRM_BOOL          fAESCrypto );

DRM_RESULT DRM_CALL CompareDSTRToWCHAR( __in const DRM_CONST_STRING *pLhs, __in const DRM_WCHAR *pRhs );

const DRM_WCHAR * DRM_CALL TST_UTL_wcsistr( __in const DRM_WCHAR *f_pwszStr, __in const DRM_WCHAR *f_pwszStrSearch );

DRM_RESULT DRM_CALL TST_UTL_wcsncat( __out DRM_WCHAR *f_pwszTarget, __in const DRM_WCHAR *f_pwszSource, __in DRM_DWORD f_cchLen );

DRM_RESULT DRM_CALL TST_UTL_wcsncpy( __out DRM_WCHAR *f_pwszTarget, __in const DRM_WCHAR *f_pwszSource, __in DRM_DWORD f_cchLen );

DRM_API DRM_RESULT DRM_CALL DRMCRT_WtoWORD(
        __in_ecount( f_cchStringInput ) const DRM_WCHAR *f_pwszStringInput,
        __in                                  DRM_DWORD  f_cchStringInput,
        __in                                  DRM_DWORD  f_base,
        __out                                 DRM_WORD  *f_pwValue,
        __out_opt                             DRM_DWORD *f_pcchValue );


ENTER_PKTEST_NAMESPACE;

DRM_RESULT DRM_CALL CalculateCheckSum(
    __in  const DRM_BYTE  *pbData,
    __in        DRM_DWORD  cbData,
    __out       DRM_BYTE  *pbCheckSum,
    __in        DRM_DWORD  cbCheckSum,
    __in  const DRM_KID   *pKID );

DRM_RESULT TST_UTL_IsConditionTrue( __out DRM_BOOL *f_pfConditionTrue, __in long f_lConditionCount, __in_ecount( f_lConditionCount ) const char **f_ppszConditions );

DRM_DWORD DRM_CALL TST_UTL_mbstowcs( __inout_opt                    DRM_WCHAR *f_pwszDest,
                                     __in_ecount(f_cchLength) const DRM_CHAR  *f_pszSource,
                                     __in                           DRM_DWORD  f_cchLength );


#define CHECK_BIT_MASK( mask, value ) ( mask & value ) == value

DRM_RESULT DRM_CALL TST_UTL_MakePathString( __in_z_opt const DRM_CHAR         *f_pszFolder,
                                            __in_z const     DRM_CHAR         *f_pszFile,
                                            __out_ecount(1)  DRM_CONST_STRING *f_pdstrPath);

DRM_RESULT DRM_CALL TST_UTL_MakeExplicitPathString( __in_z const     DRM_CHAR         *f_pszFolder,
                                                    __in_z const     DRM_CHAR         *f_pszFile,
                                                    __out_ecount(1)  DRM_CONST_STRING *f_pdstrPath);

DRM_RESULT DRM_CALL TST_UTL_MakePathA( __in_z_opt const                    DRM_CHAR   *f_pszFolder,
                                      __in_z const                        DRM_CHAR   *f_pszFile,
                                      __deref_out_ecount_z(*f_pcchPath) DRM_CHAR  **f_ppszPath,
                                      __inout_ecount(1)                 DRM_DWORD  *f_pcchPath,
                                      DRM_BOOL f_fFuzzPath );

DRM_VOID DRM_CALL TST_UTL_FixPathSeparatorA( __inout_z DRM_CHAR  f_pszPath[DRM_MAX_PATH]);
DRM_VOID DRM_CALL TST_UTL_FixPathSeparatorW( __inout_z DRM_WCHAR f_pwszPath[DRM_MAX_PATH]);

/*
**    alloc a DRM_STRING to hold the ansi string. Caller must call using FREEDRMSTRING
*/
DRM_RESULT DRM_CALL MakeDRMString(DRM_CONST_STRING *pwszStr, const char *szStr);

#define FREEDRMSTRING( x )                  \
    do                                      \
    {                                       \
        SAFE_OEM_FREE( (x).pwszString );    \
        (x).cchString = 0;                  \
    } while( FALSE )

#define FREEDRMSTRINGPOINTER( x )           \
    do                                      \
    {                                       \
        if( (x) != NULL )                   \
        {                                   \
            FREEDRMSTRING( *(x) );          \
        }                                   \
    } while( FALSE )

/*
    Convert szStr to wide string and then compare to pwszStr. Returns 0 if same.
*/
int DRM_CALL CompDRMString(const DRM_CONST_STRING *pwszStr, const char *szStr);

/*
    Loads the file content into a memory blob. Callers must free *ppBuffer after use.
*/
DRM_BOOL DRM_CALL LoadTestFile(const DRM_CHAR* pszFolder, const DRM_CHAR* pszPath, DRM_BYTE **ppBuffer, DRM_DWORD *pSize );
DRM_BOOL DRM_CALL LoadTestFileW(const DRM_WCHAR* pwszFolder, const DRM_WCHAR* pwszPath, DRM_BYTE **ppBuffer, DRM_DWORD *pSize );

/* Save a blob to a file */
DRM_BOOL DRM_CALL SaveToFileW(const DRM_WCHAR* pwszPath, DRM_BYTE *pbData, DRM_DWORD cbData);


DRM_BOOL DRM_CALL IsDigitalOnlyString(const char *p);


DRM_RESULT DRM_CALL StringToBool(const char *psBoolAsString, DRM_BOOL* pbStringAsBool);

DRM_LONG DRM_CALL TST_UTL_CompareSystemTime(
    __in const DRMSYSTEMTIME *f_pLeftTime,
    __in const DRMSYSTEMTIME *f_pRightTime );

DRM_RESULT DRM_CALL TST_UTL_AtoUI64(
    __in_ecount( f_cchString ) const DRM_CHAR   *f_pszString,
    __in                             DRM_DWORD   f_cchString,
    __out_ecount( 1 )                DRM_UINT64 *f_pui64Value );

DRM_CHAR DRM_CALL TST_UTL_ByteValueToASCII( DRM_BYTE f_bVal );

DRM_DWORD DRM_CALL TST_UTL_NumberToAnsiString(
    __in    DRM_DWORD   f_dwValue,
    __in    DRM_DWORD   f_dwRadix,
    __in    DRM_DWORD   f_iWidth,
    __in    DRM_DWORD   f_cbString,
    __out_bcount_z(f_cbString) DRM_CHAR   *f_pszString );

DRM_RESULT DRM_CALL TST_UTL_StringCchCopyNA(
    __out_ecount_z(f_cchDstSize)DRM_CHAR * f_pszDst,
    __in DRM_DWORD f_cchDstSize,
    __in_z_opt const DRM_CHAR * f_pszSrc,
    __in DRM_DWORD f_cchMaxCount);


DRM_VOID DRM_CALL tResetSystemTimeOffset();
DRM_VOID DRM_CALL tSetFlagOEMTimeOffsetIsAbsolute( DRM_BOOL f_fOEMTimeOffsetIsAbsolute );

#define FILETIME_TO_SECONDS 10000000L
DRM_RESULT DRM_CALL tChangeSystemTime(long loffset); /*roll forward if loffset is positive; backward if negative.*/

DRM_RESULT DRM_CALL tChangeYearOnMachineClock(DRM_WORD wYear);

DRM_RESULT DRM_CALL TST_UTL_LogLicenseStateData( const DRM_CHAR *szOutputPrefix, const DRM_LICENSE_STATE_DATA *pDRMLicenseStateData );

DRM_RESULT DRM_CALL TST_UTL_CopyFileToFolder( __in const DRM_CHAR *f_pszFolder, __in const DRM_WCHAR *f_pwszFile );

DRM_RESULT DRM_CALL TST_UTL_CopyFileToDRMDirectoryA( __in_z const DRM_CHAR *f_szFile );
DRM_RESULT DRM_CALL TST_UTL_CopyFileFromDRMDirectoryA( __in_z const DRM_CHAR *f_szFile );


DRM_RESULT DRM_CALL TST_UTL_RestoreDeviceStore(DRM_CONST_STRING f_dstrDevStore, DRM_CONST_STRING f_dstrDevCert);

DRM_RESULT DRM_CALL TST_UTL_ConvertFileFromWMDRMToPlayReady(
    __in const DRM_CONST_STRING *f_pdstrInput,
    __in const DRM_CONST_STRING *f_pdstrOutput,
    __in const DRM_CONST_STRING *f_pdstrSilentURL,
    __in const DRM_CONST_STRING *f_pdstrNonSilentURL,
    __in const DRM_CONST_STRING *f_pdstrDSID );

DRM_RESULT DRM_CALL TST_UTL_GetWMDRMCert(
    __inout DRM_STRING          *f_pdstrDevCert,
    __inout DRM_CRYPTO_CONTEXT  *f_pcontextCRYP
);

DRM_RESULT DRM_CALL TST_UTL_SetWMDRMCertCachedValues(
        __inout DRM_BB_CONTEXT *f_poBBContext
);

DRM_RESULT DRM_CALL TST_UTL_GetPlayReadyCert(
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE            *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD           *f_pcbBinaryDevCertChain,
    __inout                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
);

DRM_RESULT DRM_CALL TST_UTL_SetPlayReadyCertCachedValues(
    __inout DRM_BB_CONTEXT *f_poBBContext
);

DRM_RESULT DRM_CALL TST_UTL_SaveDeviceStore(DRM_CONST_STRING f_dstrDevStore, DRM_CONST_STRING f_dstrDevCert);

DRM_RESULT DRM_CALL TST_UTL_BinaryFileCompare(
    __in const DRM_CHAR* f_pszFolder1,
    __in const DRM_CHAR* f_pszPath1,
    __in const DRM_CHAR* f_pszFolder2,
    __in const DRM_CHAR* f_pszPath2 );

DRM_RESULT DRM_CALL TST_UTL_TryAssignBase64DecodedString(
    __in_z      DRM_CHAR   *f_szSource,
    __out       DRM_DWORD  *f_pcbBuffer,
    __deref_out DRM_BYTE  **f_ppbBuffer);


#ifdef WINCE_TEST

DRM_LONG remove(const char* filename);

DRM_DWORD MyClock();

#endif /* WINCE_TEST */

#define ASSIGN_WSTR_TO_DRMSTRING( x, y ) { ( x ).pwszString = ((DRM_WCHAR *)( y )); ( x ).cchString= ( DRM_DWORD ) DRMCRT_wcslen( (DRM_WCHAR *)y ); }

EXIT_PKTEST_NAMESPACE;

#endif

