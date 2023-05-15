/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <tstutils.h>
#include <tgeneratorutils.h>

ENTER_PKTEST_NAMESPACE_CODE


/**************************************************************************************************
** Function:    TST_UTL_CopyDRMString
**
** Synopsis:    Copies a content from DRM string into another one.
**              New copy of a content buffer is allocated on the heap
**              and should be freed by a caller. Old content (if there was any) is discarded.
**
** Arguments:   f_pdstrTarget - pointer to a target string struct, should not be NULL
**              f_pdstrSource - pointer to a source string struct, should not be NULL
**                              but the string content can be empty.
**
**************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_UTL_CopyDRMString(  __out DRM_CONST_STRING      *f_pdstrTarget,
                                                    __in const DRM_CONST_STRING *f_pdstrSource )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  len = 0;

    ChkArg( f_pdstrSource != NULL );
    ChkArg( f_pdstrTarget != NULL );

    len = f_pdstrSource->cchString * SIZEOF( DRM_WCHAR );

    if ( len > 0 && NULL != f_pdstrSource->pwszString )
    {
        ChkMem( f_pdstrTarget->pwszString = ( DRM_WCHAR* )Oem_MemAlloc( len + SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( (DRM_BYTE*)f_pdstrTarget->pwszString, len + SIZEOF( DRM_WCHAR ) );
        f_pdstrTarget->cchString = f_pdstrSource->cchString;

        DRMCRT_memcpy( ( DRM_WCHAR* )f_pdstrTarget->pwszString, f_pdstrSource->pwszString, len );
    }
ErrorExit:
    return dr;
}



/**************************************************************************************************
** Function:    TST_UTL_MakeRandomKID
**
** Synopsis:    Generates random base64-encoded KID data for test purposes.
**
** Arguments:   f_pdstrKID - pointer to string to return KID data. Cannot be NULL, maybe empty.
**              Its current content will be discarded.
**************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_UTL_MakeRandomKID( __inout DRM_CONST_STRING *f_pdstrKID )
{
    DRM_RESULT dr   = DRM_SUCCESS;
    DRM_GUID   oKID = {0};

    ChkArg( f_pdstrKID != NULL );

    FREEDRMSTRINGPOINTER( f_pdstrKID );

    ChkMem( f_pdstrKID->pwszString = ( DRM_WCHAR * )Oem_MemAlloc( ( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ) * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( (DRM_BYTE*)f_pdstrKID->pwszString,( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ) * SIZEOF( DRM_WCHAR ) );
    f_pdstrKID->cchString = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKID, SIZEOF(DRM_GUID) ) );

    ChkDR( DRM_B64_EncodeW( (DRM_BYTE *)&oKID,
                            SIZEOF( DRM_GUID ),
                            (DRM_WCHAR *)f_pdstrKID->pwszString,
                           &f_pdstrKID->cchString,
                            0 ) );

ErrorExit:
    if ( DRM_FAILED( dr ) && f_pdstrKID != NULL )
    {
        FREEDRMSTRINGPOINTER( f_pdstrKID );
    }
    return dr;
}


EXIT_PKTEST_NAMESPACE_CODE

