/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __APIPARAMS_H
#define __APIPARAMS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define APIPARAMS_SKIP_TO_END 0xFFFFFFFF

typedef struct {
    char *m_szAPIName;
    long m_lParamCount;
    char **m_szParams;

    /*
        If m_fExcept is FALSE, then m_drExpected is the expected return value from the API.
        If m_fExcept is TRUE, then the API should return anything but the m_drExpected value.
    */
    DRM_BOOL m_fExcept;
    DRM_RESULT m_drExpected;

/*
    If m_fEndIfThisError is TRUE and m_drExpected is returned, end the test case early with a PASS, 
           or continue the test case assuming the return is success
*/
    DRM_BOOL m_fSkipIfThisError;
    DRM_DWORD m_dwSkipNumberOfLines;
} TAPIParams;

typedef enum { eUnknown, eBVT, eFunc, eDRT, ePerf } ENUM_TESTCASE_TYPE;
typedef enum _DRM_TEST_RUN_TYPE { eRunNone = 0, eRunDRT  = 1, eRunBVT  = 2, eRunFunc = 4, eRunPerf = 8, eRunAll  = 15 } DRM_TEST_RUN_TYPE;
        

void FreeAPIParams( TAPIParams *pParams );

int DRM_CALL ParseTCID( __in_z char* szLine, long *pTCID, __deref_out_z_opt char **ppTCTitle );
DRM_RESULT ParseTCType( __in_z char *ppTCTitle, ENUM_TESTCASE_TYPE *peTCType );
int DRM_CALL ParseAPI( __in_z char *pLine, TAPIParams *pParams );

char* TrimSpace( __inout_z_opt char *s );
char* StringDup( __in_z char *pStr );
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __APIPARAMS_H */
