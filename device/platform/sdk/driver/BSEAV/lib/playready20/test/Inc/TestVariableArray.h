/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TESTVARIABLEARRAY_H
#define __TESTVARIABLEARRAY_H

#include "PKTestNameSpace.h"


ENTER_PKTEST_NAMESPACE

#define MAX_TEST_VARIABLES 100

/* 
** Cleanup callback function can be assigned to a test variable
** so that system resources can be properly released and do not impede the next tests
** that may be used them.
** The callback does not return a value 
** and takes a ptr to the same DRM_TEST_VARIABLE as the only parameter.
*/
typedef DRM_VOID (DRM_CALL* DRM_TEST_VARIABLE_CLEANUPCALLBACK)
    ( IN const DRM_VOID *f_poTestVar );

typedef struct _tagDRM_TEST_VARIABLE
{
    DRM_VOID  *pvVar;
    DRM_DWORD  cbVar;
    DRM_CHAR  *szName;
    DRM_TEST_VARIABLE_CLEANUPCALLBACK pfnCallback;
} DRM_TEST_VARIABLE;

#define EMPTY_DRM_TEST_VARIABLE { NULL, 0, NULL, NULL }




DRM_RESULT DRM_CALL DRM_TST_GetTestVar( __in_z_opt const DRM_CHAR *f_szVar, 
                                        __inout DRM_VOID **f_ppvVar, 
                                        __inout_opt DRM_DWORD **f_ppcbVar );

DRM_RESULT DRM_CALL DRM_TST_SetTestVar( __in    const DRM_CHAR *f_szVar, 
                                        __in    DRM_VOID *f_pvVar,
                                        __in    DRM_DWORD f_cbSize,
                                        __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback );


DRM_RESULT DRM_CALL DRM_TST_AllocTestVarWithCallback(
                                            __in_z   const DRM_CHAR  *f_szVar,
                                            __in     DRM_DWORD  f_cbSize,
                                            __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback,
                                            __inout  DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar );

DRM_RESULT DRM_CALL DRM_TST_AllocTestVar( __in_z const DRM_CHAR *f_szVar,
                                          __in   DRM_DWORD f_cbSize, 
                                          __inout DRM_VOID **f_ppvVar, 
                                          __inout_opt DRM_DWORD **f_ppcbVar );

DRM_RESULT DRM_CALL DRM_TST_ReallocTestVar( __in_z const DRM_CHAR *f_szVar,
                                            __in   DRM_DWORD f_cbSize, 
                                            __inout DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar );

DRM_RESULT DRM_CALL DRM_TST_ReallocTestVarWithCallback( 
                                            __in_z   const DRM_CHAR *f_szVar,
                                            __in     DRM_DWORD f_cbSize, 
                                            __in_opt DRM_TEST_VARIABLE_CLEANUPCALLBACK f_pfnCallback,
                                            __inout  DRM_VOID **f_ppvVar, 
                                            __inout_opt DRM_DWORD **f_ppcbVar );

DRM_RESULT DRM_CALL DRM_TST_FreeTestVar( __in_z const DRM_CHAR *f_szVar );

DRM_RESULT DRM_CALL DRM_TST_FreeVarArray( );

DRM_RESULT DRM_CALL DRM_TST_SwapTestVars( __in_z const DRM_CHAR *f_szVarLeft,
                                          __in_z const DRM_CHAR *f_szVarRight );

DRM_RESULT DRM_CALL SwapTestVariables( long argc, __in_ecount( argc) char **argv );
DRM_RESULT DRM_CALL StoreStringInTestVariable( long argc, __in_ecount( argc) char **argv );
DRM_RESULT DRM_CALL CompareTestVariableToString( long argc, __in_ecount( argc) char **argv );
DRM_RESULT DRM_CALL CompareTestVariables( long argc, __in_ecount( argc) char **argv );
DRM_RESULT DRM_CALL FreeTestVariable( long argc, __in_ecount( argc) char **argv );

/*
** ---------------------------------------
** Test Macros
** ---------------------------------------
*/
#define MAP_TO_GLOBAL_TEST_VAR( pIn, szVar )                                \
do {                                                                        \
        ChkDR( DRM_TST_GetTestVar( szVar, ( DRM_VOID ** ) &pIn, NULL ) );   \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_EX( pIn, pcbIn, szVar )                          \
do {                                                                            \
        ChkDR( DRM_TST_GetTestVar( szVar, ( DRM_VOID ** ) &pIn, &pcbIn ) );     \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_Alloc( pIn, szVar, cbSize)                               \
do {                                                                                    \
        ChkDR( DRM_TST_AllocTestVar( szVar, cbSize, ( DRM_VOID ** ) &pIn, NULL ) );     \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_Alloc_EX( pIn, pcbIn, szVar, cbSize)                     \
do {                                                                                    \
        ChkDR( DRM_TST_AllocTestVar( szVar, cbSize, ( DRM_VOID ** ) &pIn, &pcbIn ) );   \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_Alloc_WithCallback( pIn, szVar, cbSize, pfnCallback )                            \
do {                                                                                                            \
        ChkDR( DRM_TST_AllocTestVarWithCallback( szVar, cbSize, pfnCallback, ( DRM_VOID ** ) &pIn, NULL ) );    \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_AllocIfNotExists( pIn, szVar, cbSize )       \
do {                                                                        \
        dr = DRM_TST_GetTestVar( szVar, ( DRM_VOID** )&pIn, NULL );         \
        if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )                            \
        {                                                                   \
            MAP_TO_GLOBAL_TEST_VAR_Alloc( pIn, szVar, cbSize );             \
        }                                                                   \
        else                                                                \
        {                                                                   \
            ChkDR( dr );                                                    \
        }                                                                   \
} while(FALSE)


#define MAP_TO_GLOBAL_TEST_VAR_Realloc( pIn, szVar, cbSize)                             \
do {                                                                                    \
        ChkDR( DRM_TST_ReallocTestVar( szVar, cbSize, ( DRM_VOID ** ) &pIn, NULL ) );   \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_Realloc_EX( pIn, pcbIn, szVar, cbSize)                   \
do {                                                                                    \
        ChkDR( DRM_TST_ReallocTestVar( szVar, cbSize, ( DRM_VOID ** ) &pIn, &pcbIn ) ); \
} while(FALSE)

#define MAP_TO_GLOBAL_TEST_VAR_Realloc_WithCallback( pIn, szVar, cbSize, pfnCallback)                           \
do {                                                                                                            \
        ChkDR( DRM_TST_ReallocTestVarWithCallback( szVar, cbSize, pfnCallback, ( DRM_VOID ** ) &pIn, NULL ) );  \
} while(FALSE)

EXIT_PKTEST_NAMESPACE

#endif /* __TESTVARIABLEARRAY_H */
