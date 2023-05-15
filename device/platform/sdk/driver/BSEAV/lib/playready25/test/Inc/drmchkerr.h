/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMCHKERR_H
#define __DRMCHKERR_H

#include <drmerr.h>

/* The dev version only trace in DBG mode. We replace check macros here to always log results for test code. */
/* Already included the dev version in drmerr.h. So undef them now. */
#undef ChkDR
#undef ChkMem
#undef ChkArg
#undef ChkDRMString
#undef ChkDRMANSIString
#undef ChkBOOL
#undef ChkFAIL
#undef ChkDRContinue
#undef ChkOverflow
#undef ChkDRMap
#undef MapDR

#define ChkDR(expr) {               \
            dr = (expr);            \
            if ( DRM_FAILED(dr) )   \
            {                       \
                Log( "ChkDR", "dr=%#X at %s : %d.\n%s\n", dr, __FILE__, __LINE__, #expr ); \
                goto ErrorExit;     \
            }                       \
        }

#define ChkMem(expr) {               \
            if ( NULL == (expr) )    \
            {                        \
                Log( "ChkMem", "Allocation failure at %s : %d.\n%s\n", __FILE__, __LINE__, #expr ); \
                dr = DRM_E_OUTOFMEMORY; \
                goto ErrorExit;     \
            }                       \
        }

#define ChkArg(expr) {               \
            if ( !(expr) )       \
            {                       \
                Log( "ChkArg", "Invalid argument at %s : %d.\n%s\n", __FILE__, __LINE__, #expr ); \
                dr = DRM_E_TEST_INVALIDARG; \
                goto ErrorExit;     \
            }                       \
        }

#define ChkDRMString(s) {    \
            if ( !(s) || (s)->pwszString==NULL || (s)->cchString==0 )   \
            {                       \
                Log( "ChkDRMString", "Invalid DRMString at %s : %d.\n%s\n", __FILE__, __LINE__, #s ); \
                dr = DRM_E_TEST_INVALIDARG; \
                goto ErrorExit;     \
            }                       \
        }
#define ChkDRMANSIString(s) {    \
            if ( !(s) || (s)->pszString==NULL || (s)->cchString==0 )   \
            {                       \
                Log( "ChkDRMANSIString", "Invalid DRMANSIString at %s : %d.\n%s\n", __FILE__, __LINE__, #s ); \
                dr = DRM_E_TEST_INVALIDARG; \
                goto ErrorExit;     \
            }                       \
        }

#define ChkBOOL(fExpr,err){  \
            if (!(fExpr))            \
            {\
                dr = (err);\
                Log( "ChkBOOL", "ChkBool( " #fExpr " ) failed, Error %#X at %s : %d.\n%s\n", dr, __FILE__, __LINE__, #fExpr ); \
                goto ErrorExit;     \
            }\
        }

#define ChkFAIL(expr) ChkBOOL(expr,DRM_E_FAIL)

#define ChkDRContinue(exp) \
{                          \
    dr=(exp);              \
    if (DRM_FAILED (dr))   \
    {                      \
       Log( "ChkDRContinue", "ChkDRContinue: dr=%#X at %s : %d.\n%s\n", dr, __FILE__, __LINE__, #exp ); \
       continue;          \
    }                      \
}
#define ChkOverflow(a,b) do {                                                                                   \
            if (a < b)                                                                                          \
            {                                                                                                   \
                Log( "ChkOverflow", "Arithmetic Overflow at %s : %d.\n%s < %s\n", __FILE__, __LINE__, #a, #b ); \
                ChkDR(DRM_E_ARITHMETIC_OVERFLOW);                                                               \
            }                                                                                                   \
        } while(FALSE)


#define ChkDRMap( expr, drOriginal, drMapped ) do {                                                                       \
            dr = ( expr );                                                                                                \
            if ( dr == ( drOriginal ) )                                                                                   \
            {                                                                                                             \
                Log( "ChkDRMap", "Error code 0x%X mapped at %s : %d. to 0x%X \n", dr,  __FILE__, __LINE__, drMapped );    \
                dr = ( drMapped );                                                                                        \
            }                                                                                                             \
            if ( DRM_FAILED( dr ) )                                                                                       \
            {                                                                                                             \
                goto ErrorExit;                                                                                           \
            }                                                                                                             \
     }while( FALSE )

#define MapDR( drOriginal, drMapped ) do {                                                                                        \
                Log( "MapDR", "Error code 0x%X mapped at %s : %d. to 0x%X \n", drOriginal,  __FILE__, __LINE__, drMapped );       \
                drOriginal = ( drMapped );                                                                                        \
     }while( FALSE )

#endif /* __DRMERR_H__ */
