/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmdomainstore.h>
#include <DomainStoreTestAPI.h>

		
ENTER_PKTEST_NAMESPACE_CODE

DRM_APP_CONTEXT *m_pContextManagerDomainStore = {0};
DRM_DOMAINSTORE_CONTEXT g_oDomainStoreContext = { 0 };
DRM_DOMAINSTORE_ENUM_CONTEXT g_oDomainStoreEnumContext = { 0 };

DRM_BYTE *g_pbKey = NULL;
DRM_DWORD g_cbKey = 0;

DRM_BYTE *g_pbCert = NULL;
DRM_DWORD g_cbCert = 0;

DRM_GUID g_oAccountID = EMPTY_DRM_GUID;
DRM_GUID g_oServiceID = EMPTY_DRM_GUID;
DRM_DWORD g_dwRevision = 0;

/*********************************************************************
**
** Function: _GetFileLength
**
** Synopsis: Function that gets the size of a file.
**
** Arguments:
**
** [f_pdstrFilename]        -- Pointer to a DRM string that contains
**                             the full path of a file.
** [f_pcbData]              -- Pointer to a variable that contains the
**                             size (number of bytes) the file.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GetFileLength(
    IN DRM_CONST_STRING *f_pdstrFilename,
    OUT DRM_DWORD *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL fpData = OEM_INVALID_HANDLE_VALUE;

    ChkDRMString( f_pdstrFilename );
    ChkArg( f_pcbData != NULL );

    fpData = Oem_File_Open( NULL,
                           f_pdstrFilename->pwszString,
                           OEM_GENERIC_READ,
                           OEM_FILE_SHARE_READ,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL );

    if ( fpData == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_FILEOPEN );
    }

    /* Get the size of the input file */
    ChkBOOL( Oem_File_GetSize( fpData, f_pcbData ),
             DRM_E_FILESEEKERROR );

ErrorExit:

    if ( fpData != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close ( fpData );
    }

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_OpenStore
**
** Synopsis: Function that opens a domain store. It does not use any
**           argument.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_OpenStore(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )m_pContextManagerDomainStore;

    ChkDR( DRM_DOMST_OpenStore( &poAppContextInternal->oDatastoreHDS,
                                &g_oDomainStoreContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_CloseStore
**
** Synopsis: Function that closes a domain store. It does not use any
**           argument.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_CloseStore(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DOMST_CloseStore( &g_oDomainStoreContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_AddKey
**
** Synopsis: Function that adds a key into the domain store. The account
**           ID is passed in as argv[1] (B64 encoded). Revision is passed in
**           as argv[2]. The key data is read from a file, whose file name
**           is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_AddKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_DWORD dwRevision = 1;
    DRM_BYTE *pbKey = NULL;
    DRM_DWORD cbKey = 0;

    ChkArg( argc == 3 );
    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );
    ChkArg( argv[ 2 ] != NULL );

    
    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbKey, &cbKey ) );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 1 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    if ( argv[ 2 ] != NULL )
    {
        dwRevision = atol( argv[ 2 ] );
    }

    dr = DRM_DOMST_AddKey( &g_oDomainStoreContext,
                           &oAccountID,
                           dwRevision,
                           pbKey,
                           cbKey );

    ChkDR( dr );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrAccountID );
    SAFE_OEM_FREE( pbKey );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_GetKey
**
** Synopsis: Function that retrieves a key from the domain store into
**           a global buffer (g_pbKey). The account ID is passed in as
**           argv[0] (B64 encoded). The revision is passed in as argv[1].
**           The global buffer is dynamically allocated and it is the
**           caller's responsibility to release it.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_GetKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_DWORD dwRevision = 1;

    ChkArg( argc == 2 );

    if ( argv[ 0 ] == NULL )
    {
        MEMCPY( &oAccountID, &g_oAccountID, SIZEOF( DRM_GUID ) );
    }
    else
    {
        ChkDR( MakeDRMString( &dstrAccountID, argv[ 0 ] ) );

        ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                                &cbSize,
                                ( DRM_BYTE* )&oAccountID,
                                0 ) );
    }

    if ( argv[ 1 ] == NULL )
    {
        dwRevision = g_dwRevision;
    }
    else
    {
        dwRevision = atol( argv[ 1 ] );
    }

    SAFE_OEM_FREE( g_pbKey );

    g_cbKey = 0;
 
    dr = DRM_DOMST_GetKey( &g_oDomainStoreContext,
                           &oAccountID,
                           dwRevision,
                           NULL,
                           &g_cbKey );

    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( g_pbKey = ( DRM_BYTE * )Oem_MemAlloc( g_cbKey ) );

    dr = DRM_DOMST_GetKey( &g_oDomainStoreContext,
                           &oAccountID,
                           dwRevision,
                           g_pbKey,
                           &g_cbKey );

    ChkDR( dr );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_DeleteKey
**
** Synopsis: Function that deletes a key from the domain store. The 
**           account ID is passed in as argv[0] (B64 encoded). The
**           revision is passed in as argv[1].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_DWORD dwRevision = 1;

    ChkArg( argc == 2 );
    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 0 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    dwRevision = atol( argv[ 1 ] );

    ChkDR( DRM_DOMST_DeleteKey( &g_oDomainStoreContext,
                                &oAccountID,
                                dwRevision ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_DeleteKeys
**
** Synopsis: Function that deletes multiple keys of the same account ID
**           from the domain store. The account ID is passed in as argv[0]
**           (B64 encoded).
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKeys(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;

    ChkArg( argc == 1 );
    ChkArg( argv[ 0 ] != NULL );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 0 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    dr = DRM_DOMST_DeleteKeys( &g_oDomainStoreContext,
                               &oAccountID );

    ChkDR( dr );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_CompareKey
**
** Synopsis: Function that compares a key stored in the global buffer
**           (g_pbKey) against the data in a key file, whose file name
**           is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other errors.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbKey = NULL;
    DRM_DWORD cbKey = 0;

    ChkArg( argc == 1 );
    ChkArg( argv[ 0 ] != NULL );

    
    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbKey, &cbKey ) );

    ChkBOOL( cbKey == g_cbKey, DRM_E_FAIL );

    ChkBOOL( MEMCMP( pbKey, g_pbKey, cbKey ) == 0, DRM_E_FAIL );

ErrorExit:
    SAFE_OEM_FREE( pbKey );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_InitEnumKey
**
** Synopsis: Function that initializes an enumerator of all keys stored
**           in the domain store.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
   DRM_RESULT dr = DRM_SUCCESS;

   ChkArg( argc == 1 );

   ChkDR( DRM_DOMST_InitEnumKey( &g_oDomainStoreContext,
                                 &g_oDomainStoreEnumContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_EnumNextKey
**
** Synopsis: Function that enumerates the next key in the domain store.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextKey(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
   DRM_RESULT dr = DRM_SUCCESS;
   DRM_DWORD cbKey = 0;

   ChkArg( argc == 1 );

   ChkDR( DRM_DOMST_EnumNextKey( &g_oDomainStoreEnumContext,
                                 &g_oAccountID,
                                 &g_dwRevision,
                                 NULL,
                                 &cbKey ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_AddCert
**
** Synopsis: Function that adds a certificate into the domain store. The
**           account ID is passed in as argv[1] (B64 encoded). The service
**           ID is passed in as argv[2] (B64 encoded). The certificate
**           data is read from a file, whose file name is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_AddCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrServiceID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    DRM_BYTE rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = {0};

    ChkArg( argc == 3 );
    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );
    ChkArg( argv[ 2 ] != NULL );

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbCert, &cbCert ) );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 1 ] ) );
    ChkDR( MakeDRMString( &dstrServiceID, argv[ 2 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    cbSize = SIZEOF(DRM_GUID);

    ChkDR( DRM_B64_DecodeW( &dstrServiceID,
                            &cbSize,
                            ( DRM_BYTE* )&oServiceID,
                            0 ) );

    ChkDR( DRM_DOMST_CreateDomainStorePassword( &oAccountID,
                                                &(((DRM_APP_CONTEXT_INTERNAL*)m_pContextManagerDomainStore)->oBlackBoxContext),
                                                rgbPasswordSST ) );

    dr = DRM_DOMST_AddCert( rgbPasswordSST,
                            ((DRM_APP_CONTEXT_INTERNAL*)m_pContextManagerDomainStore)->oBlackBoxContext.pOEMContext,
                            &g_oDomainStoreContext,
                            &oAccountID,
                            &oServiceID,
                            pbCert,
                            cbCert );

    ChkDR( dr );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrAccountID );
    FREEDRMSTRING( dstrServiceID );
    SAFE_OEM_FREE( pbCert );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_GetCert
**
** Synopsis: Function that retrieves a certificate from the domain
**           store into a global buffer (g_pbCert). The account ID is
**           passed in as argv[0] (B64 encoded). The global buffer is
**           dynamically allocated and it is the caller's responsibility
**           to release it.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_GetCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;

    ChkArg( argc == 1 );

    if ( argv[ 0 ] == NULL )
    {
        MEMCPY( &oAccountID, &g_oAccountID, SIZEOF( DRM_GUID ) );
    }
    else
    {
        ChkDR( MakeDRMString( &dstrAccountID, argv[ 0 ] ) );

        ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                                &cbSize,
                                ( DRM_BYTE* )&oAccountID,
                                0 ) );
    }

    SAFE_OEM_FREE( g_pbCert );
    
    g_cbCert = 0;

    dr = DRM_DOMST_GetCert( &g_oDomainStoreContext,
                            &oAccountID,
                            &oServiceID,
                            NULL,
                            &g_cbCert );

    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( g_pbCert = ( DRM_BYTE * )Oem_MemAlloc( g_cbCert ) );

    dr = DRM_DOMST_GetCert( &g_oDomainStoreContext,
                            &oAccountID,
                            &oServiceID,
                            g_pbCert,
                            &g_cbCert );

    ChkDR( dr );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_DeleteCert
**
** Synopsis: Function that deletes a certificate from the domain store.
**           The account ID is passed in as argv[0] (B64 encoded).
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;

    ChkArg( argc == 1 );
    ChkArg( argv[ 0 ] != NULL );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 0 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    dr = DRM_DOMST_DeleteCert( &g_oDomainStoreContext,
                               &oAccountID );

    ChkDR( dr );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_CompareCert
**
** Synopsis: Function that compares a certificate stored in the global
**           buffer (g_pbCert) against the data in a certificate file,
**           whose file name is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other errors.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;

    ChkArg( argc == 1 );
    ChkArg( argv[ 0 ] != NULL );

    
    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbCert, &cbCert ) );

    ChkBOOL( cbCert == g_cbCert, DRM_E_FAIL );

    ChkBOOL( MEMCMP( pbCert, g_pbCert, cbCert ) == 0, DRM_E_FAIL );

ErrorExit:
    SAFE_OEM_FREE( pbCert );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_CompareCerts
**
** Synopsis: Function that compares portion of the content in g_pbCert
**           again the content of a specific file in the passed in file
**           list. Assumes g_pbCert already contains data from a cert
**           collection. If the passed in file list has N files, the
**           first N-1 files will be skipped, and the last file will be
**           compared with the cert data starting at a specific location
**           in g_pbCert.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other errors.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCerts(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrCertFile = EMPTY_DRM_STRING;
    DRM_BYTE *pbGCert = NULL;
    DRM_DWORD cbGCert = 0;
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    DRM_DWORD i = 0;

    ChkArg( argc >= 1 );

    ChkArg( g_pbCert != NULL );
    ChkArg( g_cbCert > 0 );

    pbGCert = g_pbCert;
    cbGCert = g_cbCert;

    /* Skip all files from argv[ 0 ] to argv[ argc - 2 ]. */
    for ( i = 0; i < ( DRM_DWORD )argc - 1; i++ )
    {
        
        ChkDR( TST_UTL_MakePathString( NULL, argv[ i ], &dstrCertFile ) );

        ChkDR( _GetFileLength( &dstrCertFile, &cbCert ) );

        ChkBOOL( cbCert <  cbGCert, DRM_E_FAIL );

        pbGCert += __CB_DECL( cbCert );
        cbGCert -= cbCert;
        FREEDRMSTRING( dstrCertFile );
    }

    ChkArg( LoadTestFile( NULL, argv[ i ], &pbCert, &cbCert ) );

    ChkBOOL( cbCert <= cbGCert, DRM_E_FAIL );

    ChkBOOL( MEMCMP( pbCert, pbGCert, cbCert ) == 0, DRM_E_FAIL );

ErrorExit:

    SAFE_OEM_FREE( pbCert );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_InitEnumCert
**
** Synopsis: Function that initializes an enumerator of all certificates
**           stored in the domain store.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
   DRM_RESULT dr = DRM_SUCCESS;

   ChkDR( DRM_DOMST_InitEnumCert( &g_oDomainStoreContext,
                                  &g_oDomainStoreEnumContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_DOMST_EnumNextCert
**
** Synopsis: Function that enumerates the next certificate in the domain
**           store.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextCert(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
   DRM_RESULT dr = DRM_SUCCESS;
   DRM_DWORD cbCert = 0;

   ChkDR( DRM_DOMST_EnumNextCert( &g_oDomainStoreEnumContext,
                                  &g_oAccountID,
                                  &g_oServiceID,
                                  NULL,
                                  &cbCert ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: PreTestCase
**
** Synopsis: Function that initializes the environment before the running
**           of each test case.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE(DomainStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    GET_SHARED_APP_CONTEXT( m_pContextManagerDomainStore );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: PostTestCase
**
** Synopsis: Function that cleans up the environment after the running
**           of each test case.
**
**           Any dynamically allocated memory is released here.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(DomainStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;
    SAFE_OEM_FREE( g_pbKey );
    g_cbKey = 0;

    SAFE_OEM_FREE( g_pbCert );
    g_cbCert = 0;

    RemoveDRMFile( RMFILE_STORE );

    return dr;
}

BEGIN_TEST_API_LIST(DomainStore)
    API_ENTRY( TestDRM_DOMST_OpenStore )
    API_ENTRY( TestDRM_DOMST_CloseStore )
    API_ENTRY( TestDRM_DOMST_AddKey )
    API_ENTRY( TestDRM_DOMST_GetKey )
    API_ENTRY( TestDRM_DOMST_DeleteKey )
    API_ENTRY( TestDRM_DOMST_DeleteKeys )
    API_ENTRY( TestDRM_DOMST_CompareKey )
    API_ENTRY( TestDRM_DOMST_InitEnumKey )
    API_ENTRY( TestDRM_DOMST_EnumNextKey )
    API_ENTRY( TestDRM_DOMST_AddCert )
    API_ENTRY( TestDRM_DOMST_GetCert )
    API_ENTRY( TestDRM_DOMST_DeleteCert )
    API_ENTRY( TestDRM_DOMST_CompareCert )
    API_ENTRY( TestDRM_DOMST_CompareCerts )
    API_ENTRY( TestDRM_DOMST_InitEnumCert )
    API_ENTRY( TestDRM_DOMST_EnumNextCert )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
