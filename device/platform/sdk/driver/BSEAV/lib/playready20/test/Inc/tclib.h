/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TCLIB_H
#define __TCLIB_H

#include <stdio.h>
#include "drmcommon.h"
#include "drmchkerr.h"
#include "drmtestresults.h"
#include <drmcmdlnpars.h>

#include <TestVariableArray.h>
#include <apiparams.h>
#include <PKTestNameSpace.h>

#ifdef WINCE_TEST
#include <drmtypes.h>
#include <drmwindowsenv.h>
#endif

#include <toem_paths.h>

#include "toem_tclib.h" /* allow the OEM test implementation to override some things */

ENTER_PK_NAMESPACE;
    extern DRM_EXPORT_VAR DRM_CONST_STRING g_dstrDrmPath;
EXIT_PK_NAMESPACE;

ENTER_PKTEST_NAMESPACE
/************** 
*    You can optionally call this function in your test implementation to log your messages
*    szTag is the tag name in the xml log file. The usage of szMessageFmt and the variable arglist
*    is the same as sprintf.
***************/
extern void DRM_CALL Log( IN const char *szTag, IN const char *szMessageFmt, ...);
void DRM_CALL LogBlob(const char* f_szTag, const char* f_szMessage, const DRM_BYTE* f_pbBlob, DRM_DWORD f_cbBlob );



extern DRM_CONST_STRING g_dstrFuzzPath;
extern DRM_CONST_STRING g_dstrFilePath;

#define MAX_LINE_SIZE 2000

EXIT_PKTEST_NAMESPACE

/****************************************************
** Test Functionality is split into Test Modules and
** Test Areas.
**
** The Test Module is a set of associated Test APIs which
** can be included for use in a test area.
**
** A test Area is a set of test cases which call into test APIs
** in the test scripts.
**
** The test framework supports a separation of the module used
** and the area tested so that it is possible to have test scripts
** call into APIs in multiple areas. This is still a work in progress
** but that is the eventual end goal.
**
*****************************************************/

/****************************************************
** Building a Test Module
**
** In the following instructions replace "ModuleName" with
** the name of your module
**
** 1. insert the following line in the public header file
**    your library:
**      DECLARE_MODULE_DEFS(ModuleName)
**    It's included at the bottom of this file for the TestCore Module
**
** 2. Implement the PreTestCase and PostTestCase functions in a
**    source file for your module. Use MODULE_PRETESTCASE(ModuleName)
**    and MODULE_POSTTESTCASE(ModuleName) so that the function is decorated
**    correctly for inclusion in a module map.
**
**    IE:
**      DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ModuleName)(long lTCID, const char *strTCName)
**      DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ModuleName)(long lTCID, const char *strTCName)
**
**    You can use IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE(ModuleName) or 
**    IMPLEMENT_DEFAULT_POSTESTCASE_FOR_MODULE(ModuleName) if you module 
**    doesn't need to do any initialization.
**
** 3. In one of the source files for you module you need 
**    to set up the API map for that module. This is done 
**    using the following format:
**
**    BEGIN_TEST_API_LIST(ModuleName)
**        API_ENTRY(TestAPI1)
**        API_ENTRY(TestAPI2)
**        API_ENTRY(TestAPI3)
**    END_TEST_API_LIST
** 
***************/

/****************************************************
** Building a Test Area
**
** In the following instructions replace "AreaName" with
** the name of your area
**
** 1. Implement the WrapTestCase function. It is normally
**    sufficient to just use IMPLEMENT_DEFAULT_WRAPTEST
**
** 2. Declare your Test Script list:
**
**    BEGIN_TEST_SCRIPT_LIST
**        TEST_SCRIPT_ENTRY(TestScript1)
**        TEST_SCRIPT_ENTRY(TestScript2)
**        TEST_SCRIPT_ENTRY(TestScript3)
**    END_TEST_API_LIST
** 
** 3. Declare the Main method and module map
**    
**      BEGIN_TEST_MAIN(AreaName)                            
**            BEGIN_TEST_MODULE_LIST                           
**                TEST_MODULE_ENTRY(Module1)                  
**                TEST_MODULE_ENTRY(Module2)                  
**            END_TEST_MODULE_LIST                             
**        END_TEST_MAIN
**
**    The TestCore module is always included in the module map.
**
******************************************************/

typedef DRM_RESULT (DRM_CALL *LPFNPRETESTCASE)(long lTCID, const char *strTCName);
typedef DRM_RESULT (DRM_CALL *LPFNPOSTTESTCASE)(long lTCID, const char *strTCName);
typedef DRM_RESULT (DRM_CALL *LPFNWRAPTESTRUN)(long lTCID, DRM_BOOL fPreTestRun);

/*
** Use these macros to reference the PreTestCase 
** and PostTestCase Functions for a given test module
*/
#ifndef MODULE_PRETESTCASE
#define MODULE_PRETESTCASE(ModuleName) _##ModuleName##_PreTestCase
#endif /* MODULE_PRETESTCASE */

#ifndef MODULE_POSTTESTCASE
#define MODULE_POSTTESTCASE(ModuleName) _##ModuleName##_PostTestCase
#endif /* MODULE_POSTTESTCASE */

#ifndef PKTEST_NAMESPACE_MODULE_PRETESTCASE
#define PKTEST_NAMESPACE_MODULE_PRETESTCASE(ModuleName) PKTEST_NAMESPACE( _##ModuleName##_PreTestCase )
#endif /* PKTEST_NAMESPACE_MODULE_PRETESTCASE */

#ifndef PKTEST_NAMESPACE_MODULE_POSTTESTCASE
#define PKTEST_NAMESPACE_MODULE_POSTTESTCASE(ModuleName) PKTEST_NAMESPACE( _##ModuleName##_PostTestCase )
#endif /* PKTEST_NAMESPACE_MODULE_POSTTESTCASE */

#ifndef IMPLEMENT_DEFAULT_PRETESTCASE
#define IMPLEMENT_DEFAULT_PRETESTCASE \
    DRM_RESULT DRM_CALL PreTestCase(long lTCID, const char *strTCName){return DRM_SUCCESS;}
#endif /* IMPLEMENT_DEFAULT_PRETESTCASE */

#ifndef IMPLEMENT_DEFAULT_POSTTESTCASE
#define IMPLEMENT_DEFAULT_POSTTESTCASE \
    DRM_RESULT DRM_CALL PostTestCase(long lTCID, const char *strTCName){return DRM_SUCCESS;}
#endif /* IMPLEMENT_DEFAULT_POSTTESTCASE */

#ifndef IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE
#define IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE \
    IMPLEMENT_DEFAULT_PRETESTCASE \
    IMPLEMENT_DEFAULT_POSTTESTCASE
#endif /* IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE */

#ifndef IMPLEMENT_DEFAULT_WRAPTEST
#define IMPLEMENT_DEFAULT_WRAPTEST \
    static DRM_RESULT DRM_CALL WrapTestRun(long lTCID, DRM_BOOL fPreTestRun){return DRM_SUCCESS;}
#endif /* IMPLEMENT_DEFAULT_WRAPTEST */

/*
** The FOR_MODULE versions will be renamed and replace the previous macros once
** the conversion to the modular format is complete
*/
#ifndef IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE
#define IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE( ModuleName ) \
    DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ModuleName)(long lTCID, const char *strTCName){return DRM_SUCCESS;}
#endif /* IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE */

#ifndef IMPLEMENT_DEFAULT_POSTTESTCASE_FOR_MODULE
#define IMPLEMENT_DEFAULT_POSTTESTCASE_FOR_MODULE( ModuleName ) \
    DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ModuleName)(long lTCID, const char *strTCName){return DRM_SUCCESS;}
#endif /* IMPLEMENT_DEFAULT_POSTTESTCASE_FOR_MODULE */

#ifndef IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE
#define IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( ModuleName ) \
    IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE( ModuleName ) \
    IMPLEMENT_DEFAULT_POSTTESTCASE_FOR_MODULE( ModuleName )
#endif /* IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE */

/************** 
*    All your api test functions should use this prototype.
*    argc is the number of arguments. argv is an array of arguments.
***************/
typedef DRM_RESULT (DRM_CALL *TAPITester)(long argc, __in_ecount(argc) char **argv);

typedef struct __tagTAPIMapEntry
{
    char *m_szAPI;
    TAPITester m_pAPI;
} TAPIMapEntry;
#define EMPTY_API_MAP_ENTRY { NULL, NULL }

typedef struct __tagTModMapEntry
{
    LPFNPRETESTCASE  m_pfnPreTestCase;
    LPFNPOSTTESTCASE m_pfnPostTestCase;
    char *m_szModuleName;
    const TAPIMapEntry *m_oApiMap;
} TModMapEntry;
#define EMPTY_MOD_MAP_ENTRY { NULL, NULL, NULL, NULL }

typedef struct __tagTestScriptListEntry
{
    const char* m_szTestScriptFileName;
    DRM_BOOL (DRM_CALL *m_pfnIsSupported)(DRM_VOID);
} TestScriptListEntry;
#define EMPTY_TEST_SCRIPT_LIST_ENTRY { NULL, NULL }

#ifndef BEGIN_TEST_SCRIPT_LIST
#define BEGIN_TEST_SCRIPT_LIST \
    static const TestScriptListEntry g_rgoTestScriptList[] = {
#endif /* BEGIN_TEST_SCRIPT_LIST */

#ifndef TEST_SCRIPT_ENTRY
#define TEST_SCRIPT_ENTRY(f) \
    { f, NULL },
#endif /* TEST_SCRIPT_ENTRY */

#ifndef TEST_SCRIPT_ENTRY_CONDITIONAL
#define TEST_SCRIPT_ENTRY_CONDITIONAL(f, condfunc) \
    { f, condfunc },
#endif /* TEST_SCRIPT_ENTRY_CONDITIONAL */

#ifndef END_TEST_SCRIPT_LIST
#define END_TEST_SCRIPT_LIST \
    EMPTY_TEST_SCRIPT_LIST_ENTRY };

#endif /* END_TEST_SCRIPT_LIST */

/*
** This can be redefined if a particular module must be included first
*/
#ifndef DEFAULT_TEST_MODULE_LIST_START
#define DEFAULT_TEST_MODULE_LIST_START
#endif /* DEFAULT_TEST_MODULE_LIST_START */

#ifndef BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
#define BEGIN_TEST_MODULE_LIST_NO_DEFAULTS \
    TModMapEntry rgTestModules[]= { \

#endif /* BEGIN_TEST_MODULE_LIST_NO_DEFAULTS */

#ifndef BEGIN_TEST_MODULE_LIST_NO_DEFAULTS_GLOBAL
#define BEGIN_TEST_MODULE_LIST_NO_DEFAULTS_GLOBAL \
    TModMapEntry g_rgTestModules[]= { \

#endif /* BEGIN_TEST_MODULE_LIST_NO_DEFAULTS_GLOBAL */

#ifndef BEGIN_TEST_MODULE_LIST
#define BEGIN_TEST_MODULE_LIST \
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS \
        DEFAULT_TEST_MODULE_LIST_START
#endif /* BEGIN_TEST_MODULE_LIST */

#ifndef TEST_MODULE_ENTRY
#define TEST_MODULE_ENTRY(ModuleName) \
    { MODULE_PRETESTCASE(ModuleName), MODULE_POSTTESTCASE(ModuleName), #ModuleName, &g_rgApiMap_##ModuleName[0] },
#endif /* TEST_MODULE_ENTRY */

#ifndef PKTEST_MODULE_ENTRY
#define PKTEST_MODULE_ENTRY(ModuleName) \
    { PKTEST_NAMESPACE_MODULE_PRETESTCASE(ModuleName), PKTEST_NAMESPACE_MODULE_POSTTESTCASE(ModuleName), #ModuleName, &PKTEST_NAMESPACE( g_rgApiMap_##ModuleName )[0] },
#endif /* PKTEST_MODULE_ENTRY */

#ifndef OLD_FUNC_TEST_MODULE_ENTRY
#define OLD_FUNC_TEST_MODULE_ENTRY(ModuleName) \
    { PreTestCase, PostTestCase, #ModuleName, &g_rgApiMap_##ModuleName[0] },
#endif /* OLD_FUNC_TEST_MODULE_ENTRY */

#ifndef DEFAULT_TEST_MODULE_LIST_END
#define DEFAULT_TEST_MODULE_LIST_END \
    PKTEST_MODULE_ENTRY(TestCore)
#endif /* DEFAULT_TEST_MODULE_LIST_END */

#ifndef END_TEST_MODULE_LIST
#define END_TEST_MODULE_LIST \
    DEFAULT_TEST_MODULE_LIST_END \
    EMPTY_MOD_MAP_ENTRY };
#endif /* END_TEST_MODULE_LIST */

#ifndef BEGIN_TEST_MAIN
#ifdef REF_LIBRARY
#define BEGIN_TEST_MAIN(AreaName) \
DRM_LONG DRM_CALL _##AreaName##_main( DRM_LONG argc, __in_ecount(argc) DRM_WCHAR **argv)\
{                                                                                       \
    DRM_CHAR szAreaName[] = #AreaName;

#else
#define BEGIN_TEST_MAIN(AreaName) \
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, __in_ecount(argc) DRM_WCHAR **argv)\
{                                                                           \
    DRM_CHAR szAreaName[] = #AreaName;

#endif /* REF_LIBRARY */
#endif /* BEGIN_TEST_MAIN */

#ifndef BEGIN_TEST_API_LIST
#define BEGIN_TEST_API_LIST(ModuleName) \
    const TAPIMapEntry g_rgApiMap_##ModuleName[] = {
#endif /* BEGIN_TEST_API_LIST */

#ifndef API_ENTRY
#define API_ENTRY(f) {#f, f},
#endif /* API_ENTRY */

#ifndef PKTEST_API_ENTRY
#define PKTEST_API_ENTRY(f) {#f, PKTEST_NAMESPACE( f )},
#endif /* PKTEST_API_ENTRY */

#ifndef END_TEST_API_LIST
#define END_TEST_API_LIST \
    EMPTY_API_MAP_ENTRY };
#endif /* END_TEST_API_LIST */

#ifndef END_TEST_MAIN
#define END_TEST_MAIN \
    printf( "Microsoft (R) DRM Test Tool: %s\nCopyright (c) Microsoft Corporation. All rights reserved.\n\nArguments:\n", szAreaName ); \
/*    PKTEST_NAMESPACE( DemoteArgumentsToAnsi( argc, argv ) );  */                                                                          \
    {                                                                                                                                   \
        DRM_LONG n = 0;                                                                                                                  \
        for ( ; n < argc; n++ )                                                                                                         \
        {                                                                                                                               \
            printf( "\targv[%d]: %s\n", n, (char*) argv[n] );                                                                           \
        }                                                                                                                               \
    }                                                                                                                                   \
    PKTEST_NAMESPACE( TST_OEM_InitPaths() );                                                                                            \
    return PKTEST_NAMESPACE( DoTest( argc, (const char **)argv, g_rgoTestScriptList, WrapTestRun, rgTestModules ) );                    \
}
#endif /* END_TEST_MAIN */

/********************************************************
** NB: The BEGIN_APIMAP Macros are included for compatibility
** but they should not be used any longer.
********************************************************/
#ifndef BEGIN_APIMAP_WITH_SCRIPT_LIST
#define BEGIN_APIMAP_WITH_SCRIPT_LIST(AreaName)  \
        BEGIN_TEST_API_LIST(OldStyleArea)
#endif /* BEGIN_APIMAP_WITH_SCRIPT_LIST */

#ifndef BEGIN_APIMAP_WITH_SCRIPT_LIST_EX
#define BEGIN_APIMAP_WITH_SCRIPT_LIST_EX(AreaName)  \
        BEGIN_TEST_API_LIST(AreaName)
#endif /* BEGIN_APIMAP_WITH_SCRIPT_LIST_EX */

/*
** BEGIN_APIMAP is a special case of BEGIN_APIMAP_WITH_SCRIPT_LIST. It only has one test script file.
*/
#ifndef BEGIN_APIMAP
#define BEGIN_APIMAP(ModuleName, tcFileName) \
    BEGIN_TEST_SCRIPT_LIST \
        TEST_SCRIPT_ENTRY(tcFileName) \
    END_TEST_SCRIPT_LIST \
    BEGIN_APIMAP_WITH_SCRIPT_LIST(ModuleName)
#endif /* BEGIN_APIMAP */

#ifndef END_APIMAP
#define END_APIMAP    \
    END_TEST_API_LIST \
    BEGIN_TEST_MAIN(OldStyleArea)                \
        BEGIN_TEST_MODULE_LIST                   \
            OLD_FUNC_TEST_MODULE_ENTRY(OldStyleArea) \
        END_TEST_MODULE_LIST                     \
    END_TEST_MAIN
#endif /* END_APIMAP */

ENTER_PKTEST_NAMESPACE

/*
** Include this macro in the header for your test API module
*/
#ifndef DECLARE_MODULE_DEFS
#ifdef __MACINTOSH__
#define DECLARE_MODULE_DEFS(ModuleName)
#else
#define DECLARE_MODULE_DEFS(ModuleName)                                                    \
    extern const TAPIMapEntry g_rgApiMap_##ModuleName[];                                   \
    DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ModuleName)(long lTCID, const char *strTCName); \
    DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ModuleName)(long lTCID, const char *strTCName);
#endif /* __MACINTOSH__ */
#endif /* DECLARE_MODULE_DEFS */

/*
** This is the entry point implemented by the test framework
*/
int DRM_CALL DoTest
(                           int argc, 
    __in_ecount(argc) const char **argv, 
                      const TestScriptListEntry *f_rgoTestScriptList, 
                            LPFNWRAPTESTRUN lpfnWrapTestRun,
                            TModMapEntry *f_rgoModuleMap
);

DRM_RESULT DRM_CALL DemoteArgumentsToAnsi(DRM_LONG argc, __out_ecount( argc ) DRM_WCHAR **argv);

#define DEFAULT_RANDOM_SEED DRM_UI64LITERAL( 0x25B946EB, 0xC0B36173 )

DRM_RESULT DRM_CALL Test_EnsureAsserted( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_SetRandomSeed( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL StartProfiling(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL StopProfiling(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL GenerateRandomData( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestCopyFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestCopyFuzzFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL BinaryCompareFiles(long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL LogTestAPI( TAPIParams APIParams );
DRM_RESULT DRM_CALL RunTestAPI(      TAPIParams    APIParams,
                                     DRM_BOOL     *pfFail,
                                     DRM_BOOL     *pfSkipLines,
                                     DRM_DWORD    *pdwLinesToSkip,
                                     DRM_RESULT   *pdrOut,
__out_bcount_opt(cchMaxTraceMessage) DRM_CHAR     *pszTraceMessage,
                                     DRM_DWORD     cchMaxTraceMessage,
                                     DRM_BOOL      fNoSuccessLog,
                                     TModMapEntry *poModuleMap );

DECLARE_MODULE_DEFS(TestCore)

EXIT_PKTEST_NAMESPACE

#endif /* __TCLIB_H */
