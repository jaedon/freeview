/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/* test headers */
#include <tclib.h>

/* Required DRM headers */
#include <TestVariableArray.h>

ENTER_PKTEST_NAMESPACE_CODE


/**********************************************************************
** Function: _GetTestVar
** 
** Synopsis: Gets the variable in the test data array at the specified index
**
** Arguments:
**              [f_iVar]   -- Index of the variable to get
**              [f_ppoVar] -- Pointer to a pointer to hold the variable
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppoVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_iVar is outside the range of the list
***********************************************************************/
static DRM_RESULT DRM_CALL _GetTestVar( DRM_DWORD f_iVar, DRM_TEST_VARIABLE **f_ppoVar )
{
    DRM_RESULT dr = DRM_SUCCESS;
    static DRM_TEST_VARIABLE rgTestData[MAX_TEST_VARIABLES] = { EMPTY_DRM_TEST_VARIABLE };

    ChkArg( f_ppoVar != NULL );
    /*
    ** not using ChkBOOL because this doesn't need to be logged
    */
    if ( f_iVar < MAX_TEST_VARIABLES )
    {
        *f_ppoVar = &rgTestData[f_iVar];
    }
    else
    {
        dr = DRM_E_TEST_VARIABLE_NOTFOUND;
    }
ErrorExit:
    return dr;
}

/**********************************************************************
** Function: _FreeTestVar
** 
** Synopsis: Frees the data held in a test variable
**
** Arguments:
**              [f_poVar] -- Variable to free
**
** Returns:  VOID
***********************************************************************/
static void DRM_CALL _FreeTestVar( __inout DRM_TEST_VARIABLE *f_poVar )
{
    if ( f_poVar->pfnCallback )
    {
        (*f_poVar->pfnCallback)( f_poVar );
        
    }
    SAFE_OEM_FREE( f_poVar->pvVar );
    SAFE_OEM_FREE( f_poVar->szName );
    f_poVar->cbVar=0;

    return;
}

/**********************************************************************
** Function: _FreeTestVarAtIndex
** 
** Synopsis: Frees the data held in the test variable at the given index
**
** Arguments:
**              [f_iVar] -- Index of the variable to free
**
** Returns:     DRM_E_TEST_VARIABLE_NOTFOUND -- if f_iVar is outside the range of the list
***********************************************************************/
static DRM_RESULT DRM_CALL _FreeTestVarAtIndex( __in DRM_DWORD f_iVar )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poData = NULL;

    ChkDR( _GetTestVar( f_iVar, &poData ) );
    _FreeTestVar( poData );
ErrorExit:
    return dr;
}


/**********************************************************************
** Function: _GetNamedTestVar
** 
** Synopsis: Gets the variable in the test data array with the given name
**           If f_fAdd == TRUE then a variable will be created in the array
**           if it is not found and there is room.
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get
**              [f_ppoVar] -- Pointer to a pointer to hold the variable
**              [f_fAdd]   -- If TRUE then the variable is added if it
**                            was not found.
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppoVar or f_szVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              DRM_E_TEST_VARIABLE_LISTFULL -- if there is no room to add the variable
**              Various other errors bubbled up.
***********************************************************************/
static DRM_RESULT DRM_CALL _GetNamedTestVar( __in_z const DRM_CHAR *f_szVar,
                                             __out DRM_TEST_VARIABLE **f_ppoVar,
                                             __in DRM_BOOL f_fAdd )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  iCurrent    = 0;
    DRM_DWORD  iFirstEmpty = MAX_TEST_VARIABLES;
    DRM_DWORD  cchVar      = 0;
    DRM_DWORD  cchCurVar   = 0;

    ChkArg( f_szVar  != NULL );
    ChkArg( f_ppoVar != NULL );
    
    ChkDR( DRM_STR_StringCchLengthA( f_szVar, DRM_STRSAFE_MAX_CCH, &cchVar ) );

    while ( dr != DRM_E_TEST_VARIABLE_NOTFOUND )
    {
        dr = _GetTestVar( iCurrent, f_ppoVar );
        if ( dr != DRM_E_TEST_VARIABLE_NOTFOUND )
        {
            ChkDR(dr);
            if( (*f_ppoVar)->szName == NULL )
            {
                if (iFirstEmpty == MAX_TEST_VARIABLES)
                {
                    iFirstEmpty = iCurrent;
                }
                iCurrent++;
            }
            else
            {
                ChkDR( DRM_STR_StringCchLengthA( (*f_ppoVar)->szName, DRM_STRSAFE_MAX_CCH, &cchCurVar ) );
                if( cchCurVar != cchVar || MEMCMP( f_szVar, (*f_ppoVar)->szName, cchVar ) != 0 )
                {
                    iCurrent++;
                }
                else
                {
                    break;
                }
            }
        }
    }
    /*
    ** After the loop if we didn't find the variable
    ** we want lets add it if we can.
    */
    if ( f_fAdd && dr == DRM_E_TEST_VARIABLE_NOTFOUND )
    {
        ChkBOOL( iFirstEmpty < MAX_TEST_VARIABLES, DRM_E_TEST_VARIABLE_LISTFULL );
        ChkDR( _GetTestVar( iFirstEmpty, f_ppoVar ) );
        ChkMem( (*f_ppoVar)->szName = ( DRM_CHAR * ) Oem_MemAlloc( SIZEOF( DRM_CHAR ) * ( cchVar + 1 ) ) );
        ChkDR( DRM_STR_StringCchCopyA( (*f_ppoVar)->szName, SIZEOF( DRM_CHAR ) * ( cchVar + 1 ), f_szVar ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_FreeVarArray
** 
** Synopsis: Frees the variables in the test data array
****
** Returns:  Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_FreeVarArray( )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  lIndex = 0;

    while ( lIndex < MAX_TEST_VARIABLES )
    {
        ChkDR( _FreeTestVarAtIndex( lIndex ) );
        lIndex++;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_GetTestVar
** 
** Synopsis: Gets the variable in the test data array with the given name
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get, if NULL f_ppvVar is set
**                            to NULL and f_ppcbVar is set to zero.
**              [f_ppvVar] -- Pointer to a void pointer to reference the data
**              [f_ppcbVar]-- pointer to a pointer to a dword to hold the size
**                            A pointer to a pointer is used so that the size stored
**                            in the variable array can be updated if needed. This is optional
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar or *f_ppcbVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_GetTestVar( __in_z_opt const DRM_CHAR *f_szVar, 
                                        __inout DRM_VOID **f_ppvVar, 
                                        __inout_opt DRM_DWORD **f_ppcbVar )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poVar = NULL;

    ChkArg( f_ppvVar != NULL );
    if ( f_szVar == NULL )
    {
        *f_ppvVar = NULL;
        if ( f_ppcbVar != NULL )
        {
            ChkArg( *f_ppcbVar != NULL );
            **f_ppcbVar = 0;
        }
        goto ErrorExit;
    }
    dr = _GetNamedTestVar( f_szVar, &poVar, FALSE );
    if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
    {
        /*
        ** Explicit check and jump to avoid loging the DR all the time.
        ** Logging that the variable wasn't found isn't useful here and is handled upstream.
        */
        goto ErrorExit; 
    }
    ChkDR( dr );
    *f_ppvVar = poVar->pvVar;
    if ( f_ppcbVar != NULL )
    {
        *f_ppcbVar = &(poVar->cbVar);
    }
ErrorExit:
    return dr;
}


/**********************************************************************
** Function: DRM_TST_SetTestVar
** 
** Synopsis: Sets the variable in the test data array with the given name
**
** Arguments:
**              [f_szVar]       -- Name of the variable to set
**              [f_pvVar]       -- Pointer to the data to be set
**              [f_cbSize]      -- Size of data to set
**              [f_pfnCallback] -- Optional Pointer to callback function to clean up the testv variable
** 
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar is NULL
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_SetTestVar( __in    const DRM_CHAR *f_szVar, 
                                        __in    DRM_VOID *f_pvVar,
                                        __in    DRM_DWORD f_cbSize,
                                        __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poVar = NULL;

    ChkArg( f_pvVar != NULL );
    ChkDR( _GetNamedTestVar( f_szVar, &poVar, TRUE ) );

    SAFE_OEM_FREE( poVar->pvVar );
    poVar->pvVar = f_pvVar;
    poVar->cbVar = f_cbSize;
    poVar->pfnCallback = f_pfnCallback;   

ErrorExit:
    return dr;
}



/**********************************************************************
** Function: DRM_TST_AllocTestVarEx
** 
** Synopsis: Allocates the variable in the test data array with the given name
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get, if NULL f_ppvVar is set
**                            to NULL and f_ppcbVar is set to zero.
**              [f_cbSize] -- The size to allocate
**              [f_pfnCallback] -- pointer to a callback function to clean up test variable
**              [f_ppvVar] -- Pointer to a void pointer to reference the data
**              [f_ppcbVar]-- pointer to a pointer to a dword to hold the size
**                            A pointer to a pointer is used so that the size stored
**                            in the variable array can be updated if needed. This is optional
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar or *f_ppcbVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_AllocTestVarWithCallback( 
                                            __in_z   const DRM_CHAR  *f_szVar,
                                            __in     DRM_DWORD  f_cbSize,
                                            __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback,
                                            __inout  DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poVar = NULL;

    ChkArg( f_ppvVar != NULL );
    if ( f_szVar == NULL )
    {
        *f_ppvVar = NULL;
        if ( f_ppcbVar != NULL )
        {
            ChkArg( *f_ppcbVar != NULL );
            **f_ppcbVar = 0;
        }
        goto ErrorExit;
    }
    ChkDR( _GetNamedTestVar( f_szVar, &poVar, TRUE ) );
    SAFE_OEM_FREE( poVar->pvVar );
    ChkMem( poVar->pvVar = Oem_MemAlloc( max(1, f_cbSize) ) ); /* to accomodate requests for allocations with zero size */
    ZEROMEM( ( DRM_BYTE * ) poVar->pvVar, f_cbSize );
    poVar->cbVar = f_cbSize;
    poVar->pfnCallback = f_pfnCallback;

    *f_ppvVar = poVar->pvVar;
    if ( f_ppcbVar != NULL )
    {
        *f_ppcbVar = &(poVar->cbVar);
    }
    
ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_AllocTestVar
** 
** Synopsis: Allocates the variable in the test data array with the given name
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get, if NULL f_ppvVar is set
**                            to NULL and f_ppcbVar is set to zero.
**              [f_cbSize] -- The size to allocate
**              [f_ppvVar] -- Pointer to a void pointer to reference the data
**              [f_ppcbVar]-- pointer to a pointer to a dword to hold the size
**                            A pointer to a pointer is used so that the size stored
**                            in the variable array can be updated if needed. This is optional
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar or *f_ppcbVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_AllocTestVar( __in_z const DRM_CHAR *f_szVar,
                                          __in   DRM_DWORD f_cbSize, 
                                          __inout DRM_VOID **f_ppvVar, 
                                          __inout_opt DRM_DWORD **f_ppcbVar )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_TST_AllocTestVarWithCallback( f_szVar,
                                   f_cbSize,
                                   NULL,
                                   f_ppvVar, 
                                   f_ppcbVar ) );
    
ErrorExit:
    return dr;
}


/**********************************************************************
** Function: DRM_TST_ReallocTestVarWithCallback
** 
** Synopsis: Reallocates the variable in the test data array with the given name
**           The original content is maintained.
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get, if NULL f_ppvVar is set
**                            to NULL and f_ppcbVar is set to zero.
**              [f_cbSize] -- The size to allocate
**              [f_pfnCallback] -- pointer to a callback function to clean up test variable
**              [f_ppvVar] -- Pointer to a void pointer to reference the data
**              [f_ppcbVar]-- pointer to a pointer to a dword to hold the size
**                            A pointer to a pointer is used so that the size stored
**                            in the variable array can be updated if needed. This is optional
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar or *f_ppcbVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_ReallocTestVarWithCallback( 
                                            __in_z   const DRM_CHAR *f_szVar,
                                            __in     DRM_DWORD f_cbSize, 
                                            __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback,
                                            __inout  DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poVar = NULL;

    ChkArg( f_ppvVar != NULL );
    if ( f_szVar == NULL )
    {
        *f_ppvVar = NULL;
        if ( f_ppcbVar != NULL )
        {
            ChkArg( *f_ppcbVar != NULL );
            **f_ppcbVar = 0;
        }
        goto ErrorExit;
    }
    ChkDR( _GetNamedTestVar( f_szVar, &poVar, TRUE ) );
    ChkMem( poVar->pvVar = Oem_MemRealloc( poVar->pvVar, f_cbSize ) );
    poVar->cbVar = f_cbSize;
    poVar->pfnCallback = f_pfnCallback;

    *f_ppvVar = poVar->pvVar;
    if ( f_ppcbVar != NULL )
    {
        *f_ppcbVar = &(poVar->cbVar);
    }
    
ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_ReallocTestVar
** 
** Synopsis: Reallocates the variable in the test data array with the given name
**           The original content is maintained.
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get, if NULL f_ppvVar is set
**                            to NULL and f_ppcbVar is set to zero.
**              [f_cbSize] -- The size to allocate
**              [f_ppvVar] -- Pointer to a void pointer to reference the data
**              [f_ppcbVar]-- pointer to a pointer to a dword to hold the size
**                            A pointer to a pointer is used so that the size stored
**                            in the variable array can be updated if needed. This is optional
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_ppvVar or *f_ppcbVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_ReallocTestVar( __in_z const DRM_CHAR *f_szVar,
                                            __in   DRM_DWORD f_cbSize, 
                                            __inout DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_TST_ReallocTestVarWithCallback( f_szVar,
                                               f_cbSize, 
                                               NULL,
                                               f_ppvVar, 
                                               f_ppcbVar ) );
ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_FreeTestVar
** 
** Synopsis: Frees the variable in the test data array with the given name
**
** Arguments:
**              [f_szVar]  -- Name of the variable to get
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_szVar is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVar is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_FreeTestVar( __in_z const DRM_CHAR *f_szVar )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poVar = NULL;

    ChkArg( f_szVar != NULL );
    
    ChkDR( _GetNamedTestVar( f_szVar, &poVar, FALSE ) );
    _FreeTestVar( poVar );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: DRM_TST_SwapTestVars
** 
** Synopsis: Swaps the pointers held in the variables 
**           in the test data array with the given names
**
** Arguments:
**              [f_szVarLeft]  -- Name of the Left variable to get
**              [f_szVarRight] -- Name of the Right variable to get
**
** Returns:     DRM_E_TEST_INVALIDARG -- if f_szVarLeft or f_szVarRight is NULL
**              DRM_E_TEST_VARIABLE_NOTFOUND -- if f_szVarLeft or f_szVarRight is not found in the list
**              Various other errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL DRM_TST_SwapTestVars( __in_z const DRM_CHAR *f_szVarLeft,
                                          __in_z const DRM_CHAR *f_szVarRight )
{

    DRM_RESULT         dr         = DRM_SUCCESS;
    DRM_TEST_VARIABLE *poLeft  = NULL;
    DRM_TEST_VARIABLE *poRight = NULL;
    DRM_TEST_VARIABLE  oTemp   = EMPTY_DRM_TEST_VARIABLE;

    ChkArg( f_szVarLeft  != NULL );
    ChkArg( f_szVarRight != NULL );

    ChkDR( _GetNamedTestVar( f_szVarLeft, &poLeft, FALSE ) );

    ChkDR( _GetNamedTestVar( f_szVarRight, &poRight, FALSE ) );

    oTemp.pvVar = poLeft->pvVar;
    oTemp.cbVar = poLeft->cbVar;
    oTemp.pfnCallback = poLeft->pfnCallback;

    poLeft->pvVar = poRight->pvVar;
    poLeft->cbVar = poRight->cbVar;
    poLeft->pfnCallback = poRight->pfnCallback;

    poRight->pvVar = oTemp.pvVar;
    poRight->cbVar = oTemp.cbVar;
    poRight->pfnCallback = poLeft->pfnCallback;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: StoreStringInTestVariable
** 
** Synopsis: Stores the string in argv[1] in the variable in argv[0]
**
** Arguments:
**              argv[0]  -- Name of the variable
**              argv[1]  -- String to store
**
** Returns:     Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL StoreStringInTestVariable( long argc, __in_ecount( argc) char **argv )
{
    DRM_RESULT   dr         = DRM_SUCCESS;
    DRM_CHAR    *szVarData  = NULL;
    DRM_DWORD   *pcbVarData = NULL;
    DRM_DWORD    cchString  = 0;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRM_STR_StringCchLengthA( argv[1], DRM_STRSAFE_MAX_CCH, &cchString ) );

    MAP_TO_GLOBAL_TEST_VAR_Alloc_EX( szVarData, pcbVarData, argv[0], SIZEOF( DRM_CHAR ) * ( cchString + 1 ) );

    ChkDR( DRM_STR_StringCchCopyA( szVarData, *pcbVarData, argv[1] ) );


ErrorExit:
    return dr;
}

/**********************************************************************
** Function: SwapTestVariables
** 
** Synopsis: Stores the string in argv[1] in the variable in argv[0]
**
** Arguments:
**              argv[0]  -- Name of the variable 1
**              argv[1]  -- Name of the variable 2
**
** Returns:     Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL SwapTestVariables( long argc, __in_ecount( argc) char **argv )
{
    DRM_RESULT         dr         = DRM_SUCCESS;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRM_TST_SwapTestVars( argv[0], argv[1] ) );

ErrorExit:
    return dr;
}



/**********************************************************************
** Function: CompareTestVariableToString
** 
** Synopsis: Compares the string in argv[1] in the variable in argv[0]
**
** Arguments:
**              argv[0]  -- Name of the variable
**              argv[1]  -- String for comparison
**
** Returns:     DRM_S_FALSE -- if the strings don't match
**              Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL CompareTestVariableToString( long argc, __in_ecount( argc) char **argv )
{
    DRM_RESULT   dr         = DRM_SUCCESS;
    DRM_CHAR    *szVarData  = NULL;
    DRM_DWORD   *pcbVarData = NULL;
    DRM_DWORD    cchTestString = 0;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRM_STR_StringCchLengthA( argv[1], DRM_STRSAFE_MAX_CCH, &cchTestString ) );

    MAP_TO_GLOBAL_TEST_VAR_EX( szVarData, pcbVarData, argv[0] ); 

    ChkBOOL( MEMCMP( argv[1], szVarData, MIN( (cchTestString/SIZEOF(DRM_CHAR)), *pcbVarData ) ) == 0, DRM_S_FALSE );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: CompareTestVariables
** 
** Synopsis: Compares the variable in argv[1] in the variable in argv[0]
**
** Arguments:
**              argv[0]  -- Name of the variable 1
**              argv[1]  -- Name of the variable 2
**
** Returns:     DRM_S_FALSE -- if the variables don't match
**              Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL CompareTestVariables( long argc, __in_ecount( argc) char **argv )
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_BYTE    *pbVarData1  = NULL;
    DRM_DWORD   *pcbVarData1 = NULL;
    DRM_BYTE    *pbVarData2  = NULL;
    DRM_DWORD   *pcbVarData2 = NULL;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );


    MAP_TO_GLOBAL_TEST_VAR_EX( pbVarData1, pcbVarData1, argv[0] ); 
    MAP_TO_GLOBAL_TEST_VAR_EX( pbVarData2, pcbVarData2, argv[1] ); 

    ChkBOOL( *pcbVarData1 == *pcbVarData2, DRM_S_FALSE );
    
    ChkBOOL( MEMCMP( pbVarData1, pbVarData2, *pcbVarData1 ) == 0, DRM_S_FALSE );
ErrorExit:
    return dr;
}

/**********************************************************************
** Function: FreeTestVariable
** 
** Synopsis: Frees the variable in argv[0]
**
** Arguments:
**              argv[0]  -- Name of the variable
**
** Returns:     DRM_E_TEST_UNEXPECTED_OUTPUT -- if the variable is not deleted
**              Various errors bubbled up.
***********************************************************************/
DRM_RESULT DRM_CALL FreeTestVariable( long argc, __in_ecount( argc) char **argv )
{
    DRM_RESULT   dr         = DRM_SUCCESS;
    DRM_CHAR    *szVarData  = NULL;
    DRM_DWORD   *pcbVarData = NULL;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( DRM_TST_FreeTestVar( argv[0] ) );

    ChkBOOL( DRM_TST_GetTestVar( argv[0], ( DRM_VOID ** ) &szVarData, &pcbVarData ) 
          == DRM_E_TEST_VARIABLE_NOTFOUND , DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
