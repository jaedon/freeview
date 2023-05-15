/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmhds.h"
#include "drmhds_impl.h"
#include <HDSTestAPI.h>
#include <drmhdsblockheadercache.h>

ENTER_PKTEST_NAMESPACE_CODE

/* 
** Most functions have additional argument using flags to
** decide which input should be overwritten:
**    1st bit overwrite HDS_CONTEXT_VAR
**    2nd bit overwrite HDS_NAMESPACE_CONTEXT_VAR
**    3rd bit overwrite HDS_QUERY_CONTEXT_VAR
**    4th bit overwrite HDS_ENUM_CONTEXT_VAR 
**    5th bit overwrite HDS_SLOT_CONTEXT_VAR 
**    6th bit overwrite HDS_SLOT_HINT_VAR
*/
#define HDS_CONTEXT_FLAG              1
#define HDS_NAMESPACE_CONTEXT_FLAG    2
#define HDS_QUERY_CONTEXT_FLAG        4
#define HDS_ENUM_CONTEXT_FLAG         8
#define HDS_SLOT_CONTEXT_FLAG         16
#define HDS_SLOT_HINT_FLAG            32


/********************************************************************************************
** Function:    TestHdsReinitialize
**
** Synopsis:    
**              
** Arguments:   argv[0] : Flags
**                        0 resets all
**                        1 resets HDS_CONTEXT_VAR
**                        2 resets all HDS_NAMESPACE_CONTEXT_VAR
**                        3 resets HDS_QUERY_CONTEXT_VAR
**                        4 resets DRM_DST_ENUM_CONTEXT
**                        5 resets HDS_SLOT_CONTEXT_VAR
**                        20 deletes the file with the name stored in HDS_NAME_VAR
**
** Returns:     DRM_SUCCESS     
**
** Note:        This does not use the standard flags for the HDS test functions
********************************************************************************************/  
DRM_RESULT DRM_CALL TestHdsReinitialize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_LONG                     i              = 0;
    DRM_LONG                     iOption        = 0;

    DRM_DST_CONTEXT             *poHdsContext   = NULL;
    DRM_DST_NAMESPACE_CONTEXT   *pNsContexts    = NULL;
    DRM_DST_ENUM_CONTEXT        *pEnumContext   = NULL;
    DRM_DST_SLOT_CONTEXT        *pSlotContext   = NULL;
    DRM_CHAR                    *pszHdsName     = NULL;

    MAP_TO_GLOBAL_TEST_VAR( poHdsContext, HDS_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pEnumContext, HDS_ENUM_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pSlotContext, HDS_SLOT_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pszHdsName, HDS_NAME_VAR );

    if ( argv != NULL )
    {
        iOption = (int)atoi( argv[0] );
    }
    if( iOption == 0 || iOption == 1 )
    {
        ChkDR( DRM_HDS_Uninit( poHdsContext ) );
        MEMSET( poHdsContext, 0, SIZEOF( DRM_DST_CONTEXT ) ); 
        ChkDR( DRM_HDS_Init( poHdsContext ) );
    }
    if( iOption == 0 || iOption == 2 )
    {
        for( i = 0; i < MAX_NAMESPACE_NUM; i++ )
        {
            MEMSET( &pNsContexts[i], 0, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ); 
        }
    }
    if( iOption == 0 || iOption == 3 )
    {
        dr = DRM_TST_FreeTestVar( HDS_QUERY_CONTEXT_VAR );
        if ( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
        {
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );
    }
    if( iOption == 0 || iOption == 4 )
    {
        MEMSET( pEnumContext, 0, SIZEOF( DRM_DST_ENUM_CONTEXT ) ); 
    }
    if( iOption == 0 || iOption == 5 )
    {
        MEMSET( pSlotContext, 0, SIZEOF( DRM_DST_SLOT_CONTEXT ) ); 
    }
    
    if( iOption == 0 || iOption == 20 )
    {
        /*
        ** delete store
        */
        if( DRMCRT_strlen( pszHdsName ) )
        {
            if DRM_FAILED( tRemoveFile( pszHdsName ) )
            {
                Log( "TestHdsReinitialize", "TestHdsReinitialize: Warning - Could not delete the store named \"%s\", continuing test case\n", pszHdsName );
            }
        }
    }
ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestHdsInit
**
** Synopsis:    Initializes the HDS context
**              
** Arguments:   argv[0] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsInit(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;

    if( argc > 0
     && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pLocalHdsContext, argv[0], SIZEOF( DRM_DST_CONTEXT ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
    }

    ChkDR( DRM_HDS_Init( pLocalHdsContext ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestHdsUninit
**
** Synopsis:    Uninitializes the HDS context
**              
** Arguments:   argv[0] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsUninit(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;

    if( argc > 0
     && argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[0] );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
    }

    ChkDR( DRM_HDS_Uninit( pLocalHdsContext ) );

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    TestHdsCreateStore
**
** Synopsis:    
**              
** Arguments:   argv[0] : Name of the HDS
**              argv[1] : block size
**              argv[2] : block number type
**              argv[3] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[4] : truncate flag
**              argv[5] : Flags
**              argv[6] : Grow-by size (optional, if NULL this calls TestHdsCreateStore)
**              argv[7] : Optional. HDS context variable name.
**              argv[8] : Optional. If not NULL, don't delete existing HDS file.
**
** Returns:     DRM_SUCCESS     
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsCreateStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iHdsNameNull       = 0;
    DRM_LONG                     iBlkNumType        = 2;
    DRM_DWORD                    wBlockSize         = 0;
    DRM_DST_BLKNUMTYPE                eBlkNumType        = eDRM_DSTBLKNUM_WORD;
    DRM_WCHAR                    wszHdsName[256]    = {0};
    DRM_BOOL                     fTruncateExist     = TRUE;
    DRM_CHAR                    *pszHdsName         = NULL;
    DRM_DWORD                    dwInitSize         = 0;
    DRM_DWORD                    cbLen              = 0;

    /*
    ** At least 6 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc >= 6 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
    ChkArg( IsDigitalOnlyString( argv[4] ) );
    ChkArg( IsDigitalOnlyString( argv[5] ) );

    iOption = (long)atol( argv[5] );

    MAP_TO_GLOBAL_TEST_VAR( pszHdsName, HDS_NAME_VAR );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[3] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[3], MIN( DRMCRT_strlen( argv[3] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 7
         && argv[7] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[7] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }

    /*
    ** block size
    */
    wBlockSize = atol( argv[1] );

    /*
    ** HDS name
    */
    if( argv[0] != NULL )
    {
        MEMCPY( pszHdsName, argv[0], DRMCRT_strlen( argv[0] ) + 1);
        TST_UTL_mbstowcs( wszHdsName, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
    }
    else
    {
        iHdsNameNull = 1;
    }

    /*
    ** block number type
    */
    iBlkNumType = (int)atoi( argv[2] );
    
    if( iBlkNumType == 4 )
    {
        eBlkNumType = eDRM_DSTBLKNUM_DWORD;
    }
    else
    {
        eBlkNumType = eDRM_DSTBLKNUM_WORD;
    }
   
    /*
    ** truncate flag
    */
    if( atoi( argv[4] ) != 0 )
    {
        fTruncateExist = TRUE;
    }
    else
    {
        fTruncateExist = FALSE;
    }

    /*
    ** delete store
    */
    if( DRMCRT_strlen( pszHdsName )
     && (argc <= 8 || argv[8] == NULL) )
    {
        ChkDR( tRemoveFile( pszHdsName ) )
    }

    if( argc == 7 )
    {
        ChkArg( argv[6] != NULL );
        dwInitSize = (DRM_DWORD)atol( argv[6] );        
    }
    cbLen = iHdsNameNull > 0 ? 0 : DRMCRT_wcslen( wszHdsName );
    ChkDR( DRM_HDS_CreateStore ( NULL,
                                ( DRM_BYTE * )( iHdsNameNull > 0 ? NULL : wszHdsName ),
                                cbLen,
                                wBlockSize,
                                eBlkNumType,                                     
                                fTruncateExist,
                                dwInitSize,
                                pLocalHdsContext ) );
ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}


#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif

/********************************************************************************************
** Function:    TestHdsOpenStore
**
** Synopsis:    
**              
** Arguments:   argv[0] : Name of the HDS
**              argv[1] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[2] : Flags
**              argv[3] : Grow-by size (optional, if NULL this calls TestHDSOpenStore)
**              argv[4] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS     
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsOpenStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iHdsNameNull       = 0;
    DRM_WCHAR                    wszHdsName[256]    = {0};
    DRM_DWORD                    dwGrowBySize       = 0;
    DRM_CHAR                    *pszHdsName         = NULL;
    DRM_DWORD                    cbLen              = 0;

    /*
    ** At least 3 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc >= 3 );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
  
    iOption = (long)atol( argv[2] );

    MAP_TO_GLOBAL_TEST_VAR( pszHdsName, HDS_NAME_VAR );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[1] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[1], MIN( DRMCRT_strlen( argv[1] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 4
         && argv[4] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[4] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }
    
    /*
    ** HDS name
    */
    if( argv[0] != NULL )
    {
        ChkArg( DRMCRT_strlen( argv[0] ) + 1 <= NO_OF( wszHdsName ) );

        TST_UTL_mbstowcs( wszHdsName, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
        MEMCPY( pszHdsName, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
    }
    else
    {
        iHdsNameNull = 1;
    }

    if( argc >= 4 && argv[3] != NULL )
    {
        dwGrowBySize = (DRM_DWORD)atol( argv[3] );
    }

    cbLen = iHdsNameNull > 0 ? 0 : DRMCRT_wcslen( wszHdsName );

    ChkDR( DRM_HDS_OpenStore ( NULL,
                               ( DRM_BYTE * )(iHdsNameNull > 0 ? NULL : wszHdsName),
                               cbLen,
                               dwGrowBySize,
                               pLocalHdsContext ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}

 
/********************************************************************************************
** Function:    TestHdsCloseStore
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[1] : Flags
**              argv[2] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS     
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsCloseStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_LONG                     iOption            = 0;
 
    /*
    ** At least 2 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc >= 2 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );

    iOption = (long)atol( argv[1] );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 2
         && argv[2] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[2] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }

    ChkDR( DRM_HDS_CloseStore( pLocalHdsContext ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestHdsCommitStore
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[1] : Flags
**              argv[2] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS     
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsCommitStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_LONG                     iOption            = 0;
 
    /*
    ** At least 2 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc >= 2 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );
  
    iOption = (long)atol( argv[1] );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 2
         && argv[2] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[2] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }

    ChkDR( DRM_HDS_CommitStore( pLocalHdsContext ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestHdsDeleteNamespace
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[1] : name space
**              argv[2] : Flags
**              argv[3] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS
**
** Note:        Currently, this function always uses a TRUE flag for waiting      
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsDeleteNamespace(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iHdsNamespaceNull  = 0;
    DRM_DST_NAMESPACE            HdsNamespace       = {0};

    
    /*
    ** At least 3 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc >= 3 );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
  
    iOption = (long)atol( argv[2] );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 3
         && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[3] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }

    /*
    ** name space
    */
    if( argv[1] != NULL )
    {
        MEMCPY( HdsNamespace.rgbOpaqueData, argv[1], min( strlen( argv[1] ) + 1, DRM_HDS_NSNAME_SIZE ) );
    }
    else
    {
        iHdsNamespaceNull = 1;
    }

    ChkDR( DRM_HDS_DeleteNamespace( pLocalHdsContext, iHdsNamespaceNull > 0 ? NULL : &HdsNamespace, TRUE ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestHdsOpenSlot
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[1] : name space
**              argv[2] : name space open mode
**              argv[3] : number of children
**              argv[4] : name space context
**              argv[5] : namespace index.  if overwriting, use 0
**              argv[6] : Flags
**              argv[7] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsOpenNamespace(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_DST_NAMESPACE_CONTEXT   *pNsContext         = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iHdsNamespaceNull  = 0;
    DRM_LONG                     iNsContextNull     = 0;
    DRM_WORD                     wMaxNumChildren    = 0;
    DRM_DST_NAMESPACE            HdsNamespace       = {0};
    DRM_DST_OPENMODE             namespaceOpenMode  = eDRM_DST_CREATE_NEW;

    ChkArg( argc > 6 );
    
    iOption = (long)atol( argv[6] );

    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 7
         && argv[7] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[7] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }


    /*
    ** name space
    */
    if( argv[1] != NULL )
    {
        MEMCPY( HdsNamespace.rgbOpaqueData, argv[1], MIN( strlen( argv[1] ) + 1, DRM_HDS_NSNAME_SIZE ) );
    }
    else
    {
        iHdsNamespaceNull = 1;
    }

    if( atoi( argv[2] ) == 2 )
    {
        namespaceOpenMode = eDRM_DST_OPEN_EXISTING;
    }
    else
    {
        namespaceOpenMode = eDRM_DST_CREATE_NEW;
    }

    /*
    ** number of children
    */
    wMaxNumChildren = (DRM_WORD)atol( argv[3] );
 
    /*
    ** name space context
    */
    if( IsDigitalOnlyString( argv[5] ) )
    {
        DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
        DRM_LONG                   iNamespaceIndex = 0;
        
        MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
        
        iNamespaceIndex=(int)atoi( argv[5] );
        ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
        
        pNsContext = &pNsContexts[iNamespaceIndex];
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pNsContext, argv[5], SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) );
    }

    if( argv[4] != NULL )
    {
         MEMCPY( pNsContext->rgbOpaqueData, argv[4], MIN( DRMCRT_strlen( argv[4] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
    }
    else
    {
        iNsContextNull = 1;
    }

    ChkDR( DRM_HDS_OpenNamespace( pLocalHdsContext,
                                  iHdsNamespaceNull > 0 ? NULL : &HdsNamespace,
                                  namespaceOpenMode,
                                  wMaxNumChildren,
                                  DRM_HDS_OPEN_NAMESPACE_DEFAULT_EXPECTED_SLOT_SIZE,
                                  iNsContextNull > 0 ? NULL : pNsContext ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif
 
/********************************************************************************************
** Function:    TestHdsCloseNamespace
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the namespace context if the namespace context overwrite flag is set
**              argv[1] : namespace index.  if overwriting, use 0
**              argv[2] : Flags
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsCloseNamespace(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext    = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iNsContextNull     = 0;
 
    /*
    ** At least 3 arguments. The last one is used to decide whether we need to overwrite the context
    ** second argument to decide which name space
    */
    ChkArg( argc > 2 );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
  
    iOption = (long)atol( argv[2] );

    /*
    ** The second bit is to overwrite flag for name space Context
    */
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalNsContext = (DRM_DST_NAMESPACE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
            MEMCPY( pLocalNsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
        }
        else
        {
            iNsContextNull = 1;
        }
    }
    else
    {
        if( IsDigitalOnlyString( argv[1] ) )
        {
            DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
            DRM_LONG                   iNamespaceIndex = 0;
            
            MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
            
            iNamespaceIndex=(int)atoi( argv[1] );
            ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
            
            pLocalNsContext = &pNsContexts[iNamespaceIndex];
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[1] );
        }
    }

    ChkDR( DRM_HDS_CloseNamespace( iNsContextNull > 0 ? NULL : pLocalNsContext ) );

ErrorExit:
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalNsContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestHdsOpenSlot
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the namespace context if the namespace context overwrite flag is set
**              argv[1] : open mode
**              argv[2] : hash key
**              argv[3] : unique key
**              argv[4] : slot size
**              argv[5] : slot context
**              argv[6] : namespace index.  if overwriting, use 0
**              argv[7] : Flags
**              argv[8] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsOpenSlot(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext    = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iNsContextNull     = 0;
    DRM_LONG                     iSizeNull          = 0;
    DRM_LONG                     iHashKeyNull       = 0;
    DRM_LONG                     iUniqueKeyNull     = 0;
    DRM_DST_KEY                  HashKey            = {0};
    DRM_DST_KEY                  UniqueKey          = {0};
    DRM_DWORD                    eMode              = 0;
    DRM_DWORD                    cbSlotSize         = 0;
    DRM_DST_SLOT_CONTEXT        *pSlotContext       = NULL;

    /*
    ** At least 8 arguments. The last one is used to decide whether we need to overwrite the context
    ** 7th argument to decide which name space
    */
    ChkArg( argc > 7 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );
    ChkArg( IsDigitalOnlyString( argv[7] ) );

    if( argc > 8
     && argv[8] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotContext, argv[8], SIZEOF( DRM_DST_SLOT_CONTEXT ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, HDS_SLOT_CONTEXT_VAR );
    }
    iOption = (long)atol( argv[7] );

    /*
    ** The second bit is to overwrite flag for name space Context
    */
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalNsContext = (DRM_DST_NAMESPACE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
            MEMCPY( pLocalNsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
        }
        else
        {
            iNsContextNull = 1;
        }
    }
    else
    {
        if( IsDigitalOnlyString( argv[6] ) )
        {
            DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
            DRM_LONG                   iNamespaceIndex = 0;
            
            MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
            
            iNamespaceIndex=(int)atoi( argv[6] );
            ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
            
            pLocalNsContext = &pNsContexts[iNamespaceIndex];
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[6] );
        }
    }

    /*
    ** open mode
    */
    eMode = (DRM_DWORD)atoi( argv[1] );
 
    /*
    ** hash key
    */
    if( argv[2] != NULL )
    {
        MEMCPY( HashKey.rgb, argv[2], MIN( strlen( argv[2] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iHashKeyNull = 1;
    }

    /*
    ** unique key
    */
    if( argv[3] != NULL )
    {
        MEMCPY( UniqueKey.rgb, argv[3], MIN( strlen( argv[3] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iUniqueKeyNull=1;
    }

    /*
    ** slot size
    */
    if( argv[4] != NULL )
    {
        cbSlotSize = atol( argv[4] ); 
    }
    else
    {
        iSizeNull = 1;
    }
 
    /*
    ** slot context
    */
    if( argv[5] != NULL )
    {
         MEMCPY( pSlotContext->rgbOpaqueData, argv[5], MIN( DRMCRT_strlen( argv[5] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
    }
    else
    {
        pSlotContext = NULL;
    }

    ChkDR( DRM_HDS_OpenSlot( iNsContextNull > 0 ? NULL : pLocalNsContext, 
                             eMode,
                             iHashKeyNull > 0 ? NULL : &HashKey, 
                             iUniqueKeyNull > 0 ? NULL : &UniqueKey, 
                             NULL, 
                             iSizeNull > 0 ? NULL : &cbSlotSize,
                             pSlotContext ) );

ErrorExit:
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalNsContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsCloseSlot
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the slot context if the slot context overwrite flag is set
**              argv[1] : Flags
**              argv[2] : (optional) Boolean indicating if the slot should be trashed 
**                        (in debug builds, DRM_HDS_CloseSlot trashes the context, but not in free.)
**              argv[3] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsCloseSlot(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    _SlotContext            *pInternalSlotCtx   = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_BOOL                 fTrashContext      = TRUE;
 
    /*
    ** At least 2 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc > 1 );
    ChkArg( IsDigitalOnlyString( argv[1] ) );
                                   
    iOption = (long)atol( argv[1] );

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 3
         && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[3] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }
    }

    pInternalSlotCtx = ( _SlotContext* )pLocalSlotContext;

    /*
    ** fTrashContext flag
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        ChkDR( StringToBool( argv[2], &fTrashContext ) );
    }

#if DBG
    if ( fTrashContext )
    {
        /* 
        ** DRM_HDS_CloseSlot trashes the slot context 
        */ 
        ChkDR( DRM_HDS_CloseSlot( pLocalSlotContext ) );
    }
    else
    {
        /* 
        ** Just indicate the slot is closed 
        */
        pInternalSlotCtx->eStatus = eSlotCtxUninit;
    }
    
#else
    ChkDR( DRM_HDS_CloseSlot( pLocalSlotContext ) );
    if ( fTrashContext )
    {
        /* 
        ** DRM_HDS_CloseSlot does not trash the context 
        */ 
        MEMSET(pInternalSlotCtx, 0xdb, pInternalSlotCtx->dwContextSize);
    }
#endif

    
ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsDeleteSlot
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the namespace context if the namespace context overwrite flag is set
**              argv[1] : hash key
**              argv[2] : unique key
**              argv[3] : namespace index.  if overwriting, use 0
**              argv[4] : Flags
**
** Returns:     DRM_SUCCESS
**
** Note:        Currently, this function always uses a NULL slot hint and a TRUE flag for waiting
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsDeleteSlot(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext    = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iNsContextNull     = 0;
    DRM_LONG                     iHashKeyNull       = 0;
    DRM_LONG                     iUniqueKeyNull     = 0;
    DRM_DST_KEY                  HashKey            = {0};
    DRM_DST_KEY                  UniqueKey          = {0};
 
    /*
    ** At least 5 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc > 4 );
    ChkArg( IsDigitalOnlyString( argv[4] ) );

    iOption = (long)atol( argv[4] );

    /*
    ** The second bit is to overwrite flag for name space Context
    */
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalNsContext = (DRM_DST_NAMESPACE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
            MEMCPY( pLocalNsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
        } 
        else
        {
            iNsContextNull = 1;
        }
    }
    else
    {
        if( IsDigitalOnlyString( argv[3] ) )
        {
            DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
            DRM_LONG                   iNamespaceIndex = 0;
            
            MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
            
            iNamespaceIndex=(int)atoi( argv[3] );
            ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
            
            pLocalNsContext = &pNsContexts[iNamespaceIndex];
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[3] );
        }
    }
 
    /*
    ** hash key
    */
    if( argv[1] != NULL )
    {
         MEMCPY( HashKey.rgb, argv[1], MIN( strlen( argv[1] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iHashKeyNull = 1;
    }

    /*
    ** unique key
    */
    if( argv[2] != NULL )
    {
         MEMCPY( UniqueKey.rgb, argv[2], MIN( strlen( argv[2] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iUniqueKeyNull = 1;
    }

    ChkDR( DRM_HDS_DeleteSlot( iNsContextNull > 0 ? NULL : pLocalNsContext, 
                               iHashKeyNull > 0 ? NULL : &HashKey, 
                               iUniqueKeyNull > 0 ? NULL : &UniqueKey, 
                               NULL, 
                               TRUE ) );

ErrorExit:
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalNsContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestHdsInitSlotEnum
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the namespace context if the namespace context overwrite flag is set
**              argv[1] : hash key
**              argv[2] : lock mode
**              argv[3] : enum context
**              argv[4] : namespace index.  if overwriting, use 0
**              argv[5] : Flags
**              argv[6] : Optional. Test variable name for the enum context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsInitSlotEnum(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext    = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iNsContextNull     = 0;
    DRM_LONG                     iHashKeyNull       = 0;
    DRM_DST_LOCKMODE             eLockMode          = eDRM_DST_LOCKSHARED;
    DRM_DST_KEY                  HashKey            = {0};
    DRM_DST_ENUM_CONTEXT        *pLocalEnumContext  = NULL;

    /*
    ** At least 6 arguments. The last one is used to decide whether we need to overwrite the context
    ** 6th argument to decide which name space to use
    */
    ChkArg( argc > 5 );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
    ChkArg( IsDigitalOnlyString( argv[5] ) );

    iOption = (long)atol( argv[5] );

    if( argc > 6
     && argv[6] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pLocalEnumContext, argv[6], SIZEOF( DRM_DST_ENUM_CONTEXT ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
    }

    /*
    ** The second bit is to overwrite flag for name space Context
    */
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalNsContext = (DRM_DST_NAMESPACE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
            MEMCPY( pLocalNsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
        }
        else
        {
            iNsContextNull = 1;
        }
    }
    else
    {
        if( IsDigitalOnlyString( argv[4] ) )
        {
            DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
            DRM_LONG                   iNamespaceIndex = 0;
            
            MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
            
            iNamespaceIndex=(int)atoi( argv[4] );
            ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
            
            pLocalNsContext = &pNsContexts[iNamespaceIndex];
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[4] );
        }
    }

    /*
    ** hash key
    */
    if( argv[1] != NULL )
    {
         MEMCPY( HashKey.rgb, argv[1], MIN( strlen( argv[1] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iHashKeyNull = 1;
    }

    /*
    ** lock mode
    */
    eLockMode = (DRM_DST_LOCKMODE)atoi( argv[2] ); 
  
    /*
    ** enum context
    */
    if( argv[3] != NULL )
    {
         MEMCPY( pLocalEnumContext->rgbOpaqueData, argv[3], MIN( DRMCRT_strlen( argv[3] ) + 1, SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
    }
    else
    {
        pLocalEnumContext = NULL;
    }

    ChkDR( DRM_HDS_InitSlotEnum( iNsContextNull > 0 ? NULL : pLocalNsContext, 
                                 iHashKeyNull > 0 ? NULL : &HashKey, 
                                 eLockMode, 
                                 pLocalEnumContext ) );

ErrorExit:
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalNsContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsSlotEnumNext
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the enum context if the enum context overwrite flag is set
**              argv[1] : slot context
**              argv[2] : hash key
**              argv[3] : unique key
**              argv[4] : slot size
**              argv[5] : hash key expected output
**              argv[6] : unique key expected output
**              argv[7] : size expected output
**              argv[8] : Flags
**              argv[9] : Optional. Test variable name for the slot context
**              argv[10]: Optional. Test variable name for the enum context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsSlotEnumNext(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_KEY              UniqueKey          = {0};
    DRM_DST_KEY              HashKey            = {0};
    DRM_DST_ENUM_CONTEXT    *pLocalEnumContext  = NULL;    
    _EnumContext            *pBadEnumContext    = NULL;
    DRM_UINT64               ui64BadTime        = {0};
    DRM_LONG                 iOption            = 0;
    DRM_LONG                 iSizeNull          = 0;
    DRM_LONG                 iHashKeyNull       = 0;
    DRM_LONG                 iUniqueKeyNull     = 0;
    DRM_DST_SLOT_CONTEXT    *pSlotContext       = NULL;        
    DRM_DWORD                cbSize             = 0;
  
    /*
    ** at least 9 arguments. The last one is used to decide whether we need to overwrite the context
    ** 8th argument is size of slot
    ** 7th argument is Unique Key
    ** 6th argument is Hash Key
    */
    ChkArg( argc >= 9 );
    ChkArg( IsDigitalOnlyString( argv[8] ) );

    if( argc > 9
     && argv[9] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotContext, argv[9], SIZEOF( DRM_DST_SLOT_CONTEXT ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, HDS_SLOT_CONTEXT_VAR );
    }
    iOption = (long)atol( argv[8] );

    /*
    ** The 4th bit is to overwrite flag for enum Context
    */
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        { 
            ChkMem( pLocalEnumContext = (DRM_DST_ENUM_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
            if( 0 == strncmp( argv[0], "BAD_ENUM_CONTEXT", 16 ) )
            {
                /*
                ** Use an invalid enum context
                */
                pBadEnumContext = ( _EnumContext* )pLocalEnumContext;
                MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
                MEMCPY( pBadEnumContext, pLocalEnumContext, SIZEOF( DRM_DST_ENUM_CONTEXT ) );
                (&pBadEnumContext->oCurrBlock)->File._image.nParentBlockNum = -1;
                MEMCPY( &( (&pBadEnumContext->oCurrBlock)->File._image.ui64TimeStamp ), &ui64BadTime, SIZEOF( DRM_UINT64 ) );
                pLocalEnumContext = ( DRM_DST_ENUM_CONTEXT* )pBadEnumContext;
            }
            else
            {
                MEMCPY( pLocalEnumContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
            }
        }
    }
    else
    {
        if( argc > 10
         && argv[10] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, argv[10] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
        }
    }

    /*
    ** slot context 
    */
    if( argv[1] != NULL )
    {
         MEMCPY( pSlotContext->rgbOpaqueData + SIZEOF(DRM_DWORD), argv[1], MIN( strlen( argv[1] ) + 1,  SIZEOF( DRM_DST_SLOT_CONTEXT ) - SIZEOF(DRM_DWORD) ) );
    }
    else
    {
        pSlotContext = NULL;
    }

    /*
    ** hash key
    */
    if( argv[2] != NULL )
    {
          MEMCPY( HashKey.rgb, argv[2], MIN( strlen( argv[2] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iHashKeyNull = 1;
    }

    /*
    ** unique key
    */
    if( argv[3] != NULL )
    {
          MEMCPY( UniqueKey.rgb, argv[3], MIN( strlen( argv[3] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iUniqueKeyNull = 1;
    }
  
    /*
    ** slot size
    */
    if( argv[4] != NULL )
    {
         cbSize = (DRM_DWORD)atol( argv[4] );
    }
    else
    {
        iSizeNull = 1;
    }

    ChkDR( DRM_HDS_SlotEnumNext( pLocalEnumContext,
                                 pSlotContext,
                                 iHashKeyNull > 0 ? NULL : &HashKey,
                                 iUniqueKeyNull > 0 ? NULL : &UniqueKey,
                                 iSizeNull > 0 ? NULL : &cbSize ) );

    /*
    ** verify the results
    */
    if( argv[7] != NULL )
    {
        if( cbSize != (DRM_DWORD)atol( argv[7] ) )
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    if( argv[6] != NULL ) 
    {
        if( strncmp( argv[6], (const DRM_CHAR *)UniqueKey.rgb, strlen( argv[6] ) ) )
        {
            Log( "Trace", "\t\tFrom TestHdsSlotEnumNext: bUniqueKey=%s", UniqueKey.rgb);
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    if( argv[5] != NULL )
    {
        if( strncmp( argv[5], (const DRM_CHAR *)HashKey.rgb, strlen( argv[5] ) ) )
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }

ErrorExit:
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalEnumContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsSlotResize
**
** Synopsis:    
**              
** Arguments:   argv[0] : Used to overwrite the HDS slot context if the context overwrite flag is set
**              argv[1] : Slot Size
**              argv[2] : Flags
**              argv[3] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
**
** Note:        This test function needs additional verification steps
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsSlotResize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_DWORD                cbSize             = 0;
 
    /*
    ** At least 3 arguments. The last one is used to decide whether we need to overwrite the context
    */
    if( argc < 3 )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotResize: error in number of arguments." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    if( !IsDigitalOnlyString( argv[1] ) || !IsDigitalOnlyString( argv[2] ) )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotResize: error in the argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
  
    iOption = (long)atol( argv[2] );

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 3
         && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[3] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }

    }
   
    /*
    ** slot size
    */
    cbSize = atol( argv[1] );
 
    ChkDR( DRM_HDS_SlotResize( pLocalSlotContext, cbSize ) );
    
ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsSlotSeek
**
** Synopsis:    
**              
** Arguments:   argv[0] : Used to overwrite the HDS slot context if the context overwrite flag is set
**              argv[1] : Offset
**              argv[2] : Seek mode
**              argv[3] : Seek pointer initial value
**              argv[4] : Flags
**              argv[5] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
**
** Note:        This test function needs additional verification steps
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsSlotSeek(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_LONG                 iSeekPointerNull   = 0;
    DRM_LONG                 iSeekMode          = 0;
    DRM_LONG                 nOffset            = 0;
    DRM_DWORD                wSeekPointer       = 0;
    DRM_DST_SEEKMODE         eOrigin            = eDRM_DST_SEEKCUR;
 
    /*
    ** At least 5 arguments. The last one is used to decide whether we need to overwrite the context
    */
    if( argc < 5 )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotSeek: error in number of arguments." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    if( !IsDigitalOnlyString( argv[1] ) || !IsDigitalOnlyString( argv[2] ) || !IsDigitalOnlyString( argv[4] ) )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotSeek: error in the argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
  
    iOption = (long)atol( argv[4] );
 
    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 5
         && argv[5] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[5] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }

    }
   
    /*
    ** offset
    */
    nOffset = atol( argv[1] );
 
    /*
    ** seek mode
    */
    iSeekMode = (int)atoi( argv[2] );
    if( iSeekMode == 1 )
    {
        eOrigin = eDRM_DST_SEEKCUR;
    }
    else if( iSeekMode == 2 )
    {
        eOrigin = eDRM_DST_SEEKEND;
    }
    else if( iSeekMode == 3 )
    {
        eOrigin = eDRM_DST_SEEKSET;
    }
    else
    {
        eOrigin = (DRM_DST_SEEKMODE)iSeekMode;
    }

    /*
    ** seek pointer
    */
    if( argv[3] != NULL )
    {
         wSeekPointer = atol( argv[3] ); 
    }
    else
    {
        iSeekPointerNull=1;
    }
 
    ChkDR( DRM_HDS_SlotSeek( pLocalSlotContext, nOffset, eOrigin, iSeekPointerNull > 0 ? NULL : &wSeekPointer ) );

ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestHdsSlotRead
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the HDS slot context if the context overwrite flag is set
**              argv[1] : data size
**              argv[2] : data initial value
**              argv[3] : size read initial value
**              argv[4] : data expected output
**              argv[5] : Flags
**              argv[6] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsSlotRead(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_BYTE                *pbData             = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_LONG                 iReadNull          = 0;
    DRM_DWORD                cbData             = 0;
    DRM_DWORD                cbRead             = 0;
  
    /*
    ** at least 6 arguments. The last one is used to decide whether we need to 
    ** overwrite the context
    ** The 5th argument is used to verify the read content
    */
    if( argc < 6 )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotRead: error in number of arguments." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    if( !IsDigitalOnlyString( argv[1] ) || !IsDigitalOnlyString( argv[5] ) )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotRead: error in the argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
  
    iOption = (long)atol( argv[5] );

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 6
         && argv[6] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[6] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }
    }

    /* 
    ** data size
    */
    cbData = atol( argv[1] ); 

    /*
    ** data
    */
    if( argv[2] != NULL )
    {
        ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( ( DRM_DWORD ) DRMCRT_strlen( argv[2] ) + 1 ) );
        MEMCPY( pbData, argv[2], DRMCRT_strlen( argv[2] ) + 1 );
    }
    
    /*
    ** size written
    */
    if( argv[3] != NULL )
    {
         cbRead = atol( argv[3] ); 
    }
    else
    {
        iReadNull = 1;
    }
 
    ChkDR( DRM_HDS_SlotRead( pLocalSlotContext, cbData, pbData, iReadNull > 0 ? NULL : &cbRead ) );
        
    /*
    ** check the read result
    */
    if( argv[4] != NULL )
    {
        if( cbRead != 0 && strncmp( (const DRM_CHAR *)pbData, argv[4], DRMCRT_strlen( argv[4] ) ) != 0 )
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }

ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    SAFE_OEM_FREE(pbData);
    return dr;
}

/********************************************************************************************
** Function:    TestHdsSlotWrite
**
** Synopsis:    
**              
** Arguments:   argv[0] : used to overwrite the HDS slot context if the context overwrite flag is set
**              argv[1] : data size
**              argv[2] : data
**              argv[3] : size written initial value
**              argv[4] : Flags
**              argv[5] : size written expected value
**              argv[6] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsSlotWrite(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_DST_SLOT_CONTEXT    *pTempSlotContext   = NULL;
    DRM_BYTE                *pbData             = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_LONG                 iWrittenNull       = 0;
    DRM_DWORD                cbData             = 0;
    DRM_DWORD                cbWritten          = 0;
    DRM_DWORD                cbWriteExpected    = 0;

    /*
    ** At least 5 arguments. The last one is used to decide whether we need to overwrite the context
    */
    if( argc < 5 )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotWrite: error in number of arguments." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    if( !IsDigitalOnlyString( argv[1] )|| !IsDigitalOnlyString( argv[4] ) )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotWrite: error in the argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
  
    iOption = (long)atol( argv[4] );

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        { 
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            if( 0 == strncmp( argv[0], "COPY", 16 ) )
            {
                /*
                ** Use a copy of the slot context
                */
                pTempSlotContext = pLocalSlotContext;
                MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
                MEMCPY( pTempSlotContext, pLocalSlotContext, SIZEOF( DRM_DST_SLOT_CONTEXT ) );
                pLocalSlotContext = pTempSlotContext;
            }
            else
            {
                MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            }
        }
    }
    else
    {
        if( argc > 6
         && argv[6] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[6] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }
    }

    /* 
    ** data size
    */
    cbData = atol( argv[1] ); 

    /*
    ** data
    */
    if( argv[2] != NULL )
    {
        ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( ( DRM_DWORD ) DRMCRT_strlen( argv[2] ) + 1 ) );
        MEMCPY( pbData, argv[2], DRMCRT_strlen( argv[2] ) + 1 );
    }
    
    /*
    ** size written
    */
    if( argv[3] != NULL )
    {
         cbWritten = atol( argv[3] ); 
    }
    else
    {
        iWrittenNull = 1;
    }

    if ( argc > 5  && argv[5] != NULL )
    {
        cbWriteExpected = atol( argv[5] ); 
    }
    else
    {
        cbWriteExpected = cbData;
    }
 
    ChkDR( DRM_HDS_SlotWrite( pLocalSlotContext, 
                              cbData, 
                              pbData, 
                              iWrittenNull > 0 ? NULL : &cbWritten ) );



    if ( iWrittenNull <= 0 )
    {
        ChkBOOL( cbWritten == cbWriteExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    
ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    SAFE_OEM_FREE( pbData );
    return dr;
}

/********************************************************************************************
** Function:    TestHdsBlockScanDelete
**
** Synopsis:    this function was introduced during metering QFE. Currently only meter used this function.
**              
** Arguments:   argv[0] : unique key. e,g, MID
**              argv[1] : number of KID
**              argv[2] : number of the namespace to use
**              argv[3] - argv[ number of KID + 2] : unique key. e.g. KID
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestHdsBlockScanDelete(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_KEY                  UniqueKey          = {0};
    DRM_DST_KEY                  HashKeys[100]      = {0};
    DRM_DWORD                    nKID               = 0;
    DRM_DWORD                    i                  = 0;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext         = NULL;    

    ChkArg( argc > 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    
    /*
    ** unique key. e,g, MID
    */
    MEMCPY( UniqueKey.rgb, argv[0], MIN( strlen( argv[0] ) + 1, DRM_ID_SIZE ) );
  
    /*
    ** number of KID
    */
    nKID=(DRM_DWORD)atol( argv[1] );

    /*
    ** number of the namespace to use
    */
    if( IsDigitalOnlyString( argv[2] ) )
    {
        DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
        DRM_LONG                   iNamespaceIndex = 0;
        
        MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
        
        iNamespaceIndex=(int)atoi( argv[2] );
        ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
        
        pLocalNsContext = &pNsContexts[iNamespaceIndex];
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[2] );
    }

    ChkArg( nKID <= 100 );
    ChkArg( argc >= (long) ( nKID + 3 ) );

    /*
    ** unique key. e.g. KID
    */
    for( i = 0; i < nKID; i++ )
    { 
        MEMCPY( HashKeys[i].rgb, argv[3+i], MIN( strlen( argv[3+i] ) + 1, DRM_ID_SIZE ) );
    }
  
    ChkDR( DRM_HDS_BlockScanDelete( pLocalNsContext, 
                                    HashKeys, 
                                    nKID,
                                    &UniqueKey,
                                    TRUE ) );/* Wait for locks */
     
ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestDeleteFile
**
** Synopsis:    remove the specified file
**              
** Arguments:   argv[0] : Name of the file to delete 
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestDeleteFile(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( argc    == 1 )
    ChkArg( argv[0] != NULL );
    if DRM_FAILED( tRemoveFile( argv[0] ) )
    {
        Log( "TestDeleteFile", "TestDeleteFile: Warning - Could not delete the file named \"%s\", continuing test case\n", argv[0] );
    }
ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestCleanupStore
**
** Synopsis:    DRM_HDS_CleanupStore
**              
** Arguments:   argv[0] : used to overwrite the hds context if the hds context overwrite flag is set
**              argv[1] : boolean indicating that the wait flag is turned on
**              argv[2] : Flags
**              argv[3] : Optional. HDS context variable name.
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestCleanupStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_CONTEXT             *pLocalHdsContext   = NULL;
    DRM_BOOL                     fWait              = TRUE;
    DRM_LONG                     iOption            = 0;
       
    ChkArg( argc > 2 );
    ChkArg( argv[2] != NULL );

    iOption = (long)atol( argv[2] );
    
    /*
    ** The first bit is to overwrite flag for Context
    */
    if( iOption&HDS_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalHdsContext = (DRM_DST_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
            MEMCPY( pLocalHdsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 3
         && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, argv[3] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalHdsContext, HDS_CONTEXT_VAR );
        }
    }

    /*
    ** fWait flag
    */
    if ( argv[1] != NULL )
    {
        ChkDR( StringToBool( argv[1], &fWait ) );
    }

    ChkDR( DRM_HDS_CleanupStore( pLocalHdsContext, fWait ) );

ErrorExit:
    if( iOption&HDS_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalHdsContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestCommitNamespace
**
** Synopsis:    DRM_HDS_CommitNamespace
**              
** Arguments:   argv[0] : used to overwrite the namespace context if the namespace context overwrite flag is set
**              argv[1] : namespace index.
**              argv[2] : Flags
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestCommitNamespace(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DST_NAMESPACE_CONTEXT   *pLocalNsContext    = NULL;
    DRM_LONG                     iOption            = 0;
    DRM_LONG                     iNsContextNull     = 0;

    /*
    ** At least 6 arguments. The last one is used to decide whether we need to overwrite the context
    ** 6th argument to decide which name space to use
    */
    ChkArg( argc > 2 );
    ChkArg( argv[2] != NULL );

    iOption = (long)atol( argv[2] );

    /*
    ** The second bit is to overwrite flag for name space Context
    */
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalNsContext = (DRM_DST_NAMESPACE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
            MEMCPY( pLocalNsContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) ) );
        }
        else
        {
            iNsContextNull = 1;
        }
    }
    else
    {
        if( IsDigitalOnlyString( argv[1] ) )
        {
            DRM_DST_NAMESPACE_CONTEXT *pNsContexts = NULL;
            DRM_LONG                   iNamespaceIndex = 0;
            
            MAP_TO_GLOBAL_TEST_VAR( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR );
            
            iNamespaceIndex=(int)atoi( argv[1] );
            ChkArg( iNamespaceIndex < MAX_NAMESPACE_NUM && iNamespaceIndex >= 0 );
            
            pLocalNsContext = &pNsContexts[iNamespaceIndex];
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalNsContext, argv[1] );
        }
    }

    ChkDR( DRM_HDS_CommitNamespace( iNsContextNull > 0 ? NULL : pLocalNsContext ) );

ErrorExit:
    if( iOption&HDS_NAMESPACE_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalNsContext );
    }
    return dr;
}

/********************************************************************************************
** Function:    TestMakeSlotHint
**
** Synopsis:    DRM_HDS_MakeSlotHint
**              
** Arguments:   argv[0] : used to overwrite the slot context if the slot context overwrite flag is set
**              argv[1] : Flags
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestMakeSlotHint(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_DST_SLOT_HINT       *pSlotHint          = NULL;

    /*
    ** At least 2 arguments. The last one is used to decide whether we need to overwrite the context
    */
    ChkArg( argc > 1 );
    ChkArg( argv[1] != NULL );
                                   
    iOption = (long)atol( argv[1] );

    MAP_TO_GLOBAL_TEST_VAR( pSlotHint, HDS_SLOT_HINT_VAR );

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
    }

    ChkDR( DRM_HDS_MakeSlotHint( pLocalSlotContext, pSlotHint ) );
    
ErrorExit:
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    return dr;
}


/********************************************************************************************
** Function:    TestSlotEnumReloadCurrent
**
** Synopsis:    DRM_HDS_SlotEnumReloadCurrent
**              
** Arguments:   argv[0] : used to overwrite the enum context if the enum context overwrite flag is set
**              argv[1] : slot context
**              argv[2] : hash key
**              argv[3] : unique key
**              argv[4] : slot size
**              argv[5] : hash key expected output
**              argv[6] : unique key expected output
**              argv[7] : size expected output
**              argv[8] : Flags
**              argv[9] : Optional. Test variable name for the enum context
**              argv[10]: Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestSlotEnumReloadCurrent(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_KEY              UniqueKey          = {0};
    DRM_DST_KEY              HashKey            = {0};
    DRM_DST_ENUM_CONTEXT    *pLocalEnumContext  = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_LONG                 iSizeNull          = 0;
    DRM_LONG                 iHashKeyNull       = 0;
    DRM_LONG                 iUniqueKeyNull     = 0;
    DRM_DST_SLOT_CONTEXT    *pSlotContext       = NULL;        
    DRM_DWORD                cbSize             = 0;

    if( argc > 10
     && argv[10] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, argv[10] );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, HDS_SLOT_CONTEXT_VAR );
    }
  
    /*
    ** at least 9 arguments. The last one is used to decide whether we need to overwrite the context
    ** 8th argument is size of slot
    ** 7th argument is Unique Key
    ** 6th argument is Hash Key
    */
    if( argc < 9 )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotEnumNext: error in number of arguments." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    if( !IsDigitalOnlyString( argv[8] ) )
    {
        Log( "Trace", "\t\tFrom TestHdsSlotEnumNext: error in the argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
  
    iOption = (long)atol( argv[8] );

    /*
    ** The 4th bit is to overwrite flag for enum Context
    */
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        {   
            ChkMem( pLocalEnumContext = (DRM_DST_ENUM_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
            MEMCPY( pLocalEnumContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 9
         && argv[9] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, argv[9] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
        }
    }

    /*
    ** slot context 
    */
    if( argv[1] != NULL )
    {
         MEMCPY( pSlotContext->rgbOpaqueData + SIZEOF(DRM_DWORD), argv[1], MIN( strlen( argv[1] ) + 1,  SIZEOF( DRM_DST_SLOT_CONTEXT ) - SIZEOF(DRM_DWORD) ) );
    }
    else
    {
        pSlotContext = NULL;
    }

    /*
    ** hash key
    */
    if( argv[2] != NULL )
    {
          MEMCPY( HashKey.rgb, argv[2], MIN( strlen( argv[2] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iHashKeyNull = 1;
    }

    /*
    ** unique key
    */
    if( argv[3] != NULL )
    {
          MEMCPY( UniqueKey.rgb, argv[3], MIN( strlen( argv[3] ) + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iUniqueKeyNull = 1;
    }
  
    /*
    ** slot size
    */
    if( argv[4] != NULL )
    {
         cbSize = (DRM_DWORD)atol( argv[4] );
    }
    else
    {
        iSizeNull = 1;
    }

    ChkDR( DRM_HDS_SlotEnumReloadCurrent( pLocalEnumContext,
                                 pSlotContext,
                                 iHashKeyNull > 0 ? NULL : &HashKey,
                                 iUniqueKeyNull > 0 ? NULL : &UniqueKey,
                                 iSizeNull > 0 ? NULL : &cbSize ) );

    /*
    ** verify the results
    */
    if( argv[7] != NULL )
    {
        if( cbSize != (DRM_DWORD)atol( argv[7] ) )
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    if( argv[6] != NULL ) 
    {
        if( strncmp( argv[6], (const DRM_CHAR *)UniqueKey.rgb, strlen( argv[6] ) ) )
        {
            Log( "Trace", "\t\tFrom TestHdsSlotEnumNext: bUniqueKey=%s", UniqueKey.rgb);
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    if( argv[5] != NULL )
    {
        if( strncmp( argv[5], (const DRM_CHAR *)HashKey.rgb, strlen( argv[5] ) ) )
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }

ErrorExit:
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalEnumContext );
    }
    return dr;
}
 
/********************************************************************************************
** Function:    TestSlotEnumDeleteCurrent
**
** Synopsis:    DRM_HDS_SlotEnumDeleteCurrent
**              
** Arguments:   argv[0] : used to overwrite the enum context if the enum context overwrite flag is set
**                        "BAD_TIMESTAMP" indicates that timestamp of 0 should be used in the enum context
**                        "BAD_ENUM_CONTEXT" indicates that an invalid enum context should be used
**              argv[1] : used to overwrite the slot context if the slot context overwrite flag is set
**              argv[2] : Flags
**              argv[3] : Optional. Test variable name for the enum context
**              argv[4] : Optional. Test variable name for the slot context
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL TestSlotEnumDeleteCurrent(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_DST_ENUM_CONTEXT    *pLocalEnumContext  = NULL;
    _EnumContext            *pBadEnumContext    = NULL;
    DRM_LONG                 iOption            = 0;
    DRM_DST_SLOT_CONTEXT    *pLocalSlotContext  = NULL;
    DRM_UINT64               ui64BadTime        = {0};

    ChkArg( argc >= 3 );
    ChkArg( IsDigitalOnlyString( argv[2] ) );
  
    iOption = (long)atol( argv[2] );

    /*
    ** The 4th bit is to overwrite flag for enum Context
    */
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        if( argv[0] != NULL )
        { 
            ChkMem( pLocalEnumContext = (DRM_DST_ENUM_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
            if( 0 == strncmp( argv[0], "BAD_TIMESTAMP", 13 ) )
            {
                /*
                ** Use an invalid timestamp
                */
                pBadEnumContext = ( _EnumContext* )pLocalEnumContext;
                MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
                MEMCPY( pBadEnumContext, pLocalEnumContext, SIZEOF( DRM_DST_ENUM_CONTEXT ) );
                MEMCPY( &( (&pBadEnumContext->oCurrBlock)->File._image.ui64TimeStamp ), &ui64BadTime, SIZEOF( DRM_UINT64 ) );
                pLocalEnumContext = ( DRM_DST_ENUM_CONTEXT* )pBadEnumContext;
            }
            else  if( 0 == strncmp( argv[0], "BAD_ENUM_CONTEXT", 16 ) )
            {
                /*
                ** Use an invalid enum context
                */
                pBadEnumContext = ( _EnumContext* )pLocalEnumContext;
                MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
                MEMCPY( pBadEnumContext, pLocalEnumContext, SIZEOF( DRM_DST_ENUM_CONTEXT ) );
                (&pBadEnumContext->oCurrBlock)->File._image.nParentBlockNum = -1;
                MEMCPY( &( (&pBadEnumContext->oCurrBlock)->File._image.ui64TimeStamp ), &ui64BadTime, SIZEOF( DRM_UINT64 ) );
                pLocalEnumContext = ( DRM_DST_ENUM_CONTEXT* )pBadEnumContext;
            }
            else
            {
                MEMCPY( pLocalEnumContext->rgbOpaqueData, argv[0], MIN( DRMCRT_strlen( argv[0] ) + 1, SIZEOF( DRM_DST_ENUM_CONTEXT ) ) );
            }
        }
    }
    else
    {
        if( argc > 3
         && argv[3] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, argv[3] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalEnumContext, HDS_ENUM_CONTEXT_VAR );
        }
    }

    /*
    ** The 5th bit is to overwrite flag for slot Context
    */
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        /*
        ** Context is from the input 
        */
        if( argv[1] != NULL )
        {   
            ChkMem( pLocalSlotContext = (DRM_DST_SLOT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
            MEMCPY( pLocalSlotContext->rgbOpaqueData, argv[1], MIN( DRMCRT_strlen( argv[1] ) + 1, SIZEOF( DRM_DST_SLOT_CONTEXT ) ) );
        }
    }
    else
    {
        if( argc > 4
         && argv[4] != NULL )
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, argv[4] );
        }
        else
        {
            MAP_TO_GLOBAL_TEST_VAR( pLocalSlotContext, HDS_SLOT_CONTEXT_VAR );
        }
    }

    ChkDR( DRM_HDS_SlotEnumDeleteCurrent( pLocalEnumContext, pLocalSlotContext ) );

    /*
    ** verify the results
    */

ErrorExit:
    if( iOption&HDS_ENUM_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalEnumContext );
    }
    if( iOption&HDS_SLOT_CONTEXT_FLAG )
    {
        SAFE_OEM_FREE( pLocalSlotContext );
    }
    return dr;

}

/********************************************************************************************
** Function:    PreTestCase
**
** Synopsis:    Pretest case for the HDS test module
**              
** Arguments:   none 
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE(HDS)(long lTCID, const char *strTCName)
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_DST_CONTEXT             *poHdsContext   = NULL;
    DRM_DST_NAMESPACE_CONTEXT   *pNsContexts    = NULL;
    DRM_DST_ENUM_CONTEXT        *pEnumContext   = NULL;
    DRM_DST_SLOT_CONTEXT        *pSlotContext   = NULL;
    DRM_DST_SLOT_HINT           *pSlotHint      = NULL;
    DRM_CHAR                    *pszHdsName     = NULL;

    /*
    ** Set shared variables
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poHdsContext, HDS_CONTEXT_VAR, SIZEOF( DRM_DST_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pNsContexts, HDS_NAMESPACE_CONTEXT_VAR, MAX_NAMESPACE_NUM * SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pEnumContext, HDS_ENUM_CONTEXT_VAR, SIZEOF( DRM_DST_ENUM_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotContext, HDS_SLOT_CONTEXT_VAR, SIZEOF( DRM_DST_SLOT_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotHint, HDS_SLOT_HINT_VAR, SIZEOF( DRM_DST_SLOT_HINT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pszHdsName, HDS_NAME_VAR, 256 * SIZEOF( DRM_CHAR ) );

    if( DRM_HBHC_IsHdsBlockHeaderCacheSupported() )
    {
        Oem_Hds_ClearBlockHeaderCache();
    }

    /*
    ** Initialize the shared context
    */
    ChkDR( DRM_HDS_Init( poHdsContext ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PostTestCase
**
** Synopsis:    Posttest case for the HDS test module
**              
** Arguments:   none 
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(HDS)(long lTCID, const char *strTCName)
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_DST_CONTEXT *poHdsContext   = NULL;
    DRM_CHAR        *pszHdsName     = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pszHdsName, HDS_NAME_VAR );
    MAP_TO_GLOBAL_TEST_VAR( poHdsContext, HDS_CONTEXT_VAR );

    if ( poHdsContext != NULL && ISVALIDCONTEXT( (_HdsContext*)poHdsContext, eHdsContextSignature ) )
    {
        ChkDR( DRM_HDS_CloseStore( poHdsContext ) );
    }
    if ( poHdsContext != NULL )
    {
        ChkDR( DRM_HDS_Uninit( poHdsContext ) );
    }

    /*
    ** delete store
    */
    if( DRMCRT_strlen( pszHdsName ) )
    {
        ( DRM_VOID )tRemoveFile( pszHdsName );
    }

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( HDS )
    API_ENTRY(TestHdsCreateStore)    
    API_ENTRY(TestHdsOpenStore)
    API_ENTRY(TestHdsCloseStore)
    API_ENTRY(TestHdsDeleteNamespace)
    API_ENTRY(TestHdsOpenNamespace)
    API_ENTRY(TestHdsCloseNamespace)
    API_ENTRY(TestHdsInitSlotEnum)
    API_ENTRY(TestHdsSlotEnumNext)
    API_ENTRY(TestHdsCommitStore)
    API_ENTRY(TestHdsOpenSlot)
    API_ENTRY(TestHdsCloseSlot)
    API_ENTRY(TestHdsDeleteSlot)
    API_ENTRY(TestHdsSlotResize)
    API_ENTRY(TestHdsSlotSeek)
    API_ENTRY(TestHdsSlotRead)
    API_ENTRY(TestHdsSlotWrite)
    API_ENTRY(TestHdsReinitialize)
    API_ENTRY(TestDeleteFile)
    API_ENTRY(TestHdsBlockScanDelete)
    API_ENTRY(TestCleanupStore)
    API_ENTRY(TestCommitNamespace)
    API_ENTRY(TestMakeSlotHint)
    API_ENTRY(TestSlotEnumReloadCurrent)
    API_ENTRY(TestSlotEnumDeleteCurrent)
    API_ENTRY(TestHdsInit)
    API_ENTRY(TestHdsUninit)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
