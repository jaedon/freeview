/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __PKTESTNAMESPACE_H__
#define __PKTESTNAMESPACE_H__

/*
** Set namespace, USE_PK_NAMESPACES is defined in build profile
*/
#ifdef __cplusplus
    #ifdef __MACINTOSH__
        #define USE_PKTEST_NAMESPACES 0
    #else
        #if USE_PK_NAMESPACES
            #define USE_PKTEST_NAMESPACES 1
        #else
            #define USE_PKTEST_NAMESPACES 0
        #endif
    #endif  /* __MACINTOSH__ */
#else       /* __cplusplus */
    #define USE_PKTEST_NAMESPACES 0
#endif      /* __cplusplus */

#if USE_PKTEST_NAMESPACES
/*
**  Place PKTEST functions in a specific namespace
*/
#define ENTER_PKTEST_NAMESPACE namespace PKTEST {
#define EXIT_PKTEST_NAMESPACE };
#define ENTER_PKTEST_NAMESPACE_CODE namespace PKTEST {
#define EXIT_PKTEST_NAMESPACE_CODE };
#define USING_PKTEST_NAMESPACE using namespace PKTEST;
#define PKTEST_NAMESPACE( expr ) PKTEST::expr

#else /* USE_PK_NAMESPACES */
/*
**  Building as C - no need to change prototypes
*/
#define ENTER_PKTEST_NAMESPACE
#define EXIT_PKTEST_NAMESPACE
#define USING_PKTEST_NAMESPACE
#define ENTER_PKTEST_NAMESPACE_CODE
#define EXIT_PKTEST_NAMESPACE_CODE
#define USING_PKTEST_NAMESPACE
#define PKTEST_NAMESPACE( expr ) expr

#endif /* USE_PK_NAMESPACES */


#endif  /* __PKTESTNAMESPACE_H__ */

