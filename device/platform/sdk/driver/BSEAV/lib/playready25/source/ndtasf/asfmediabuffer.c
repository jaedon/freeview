/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_ASFMEDIABUFFER_C
#include <ndttypes.h>
#include <ndtcontextsizes.h>
#include <ndtasf_internal.h>
#include <ndtasftypes.h>
#include <asfmediabuffer.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define ASF_MEDIA_BUFFER_FLAGS_WRAPPER 1

DRM_RESULT ASFMediaBuffer_Initialize(
    __inout    ASF_MEDIA_BUFFER * const pAsfMediaBuffer,
    __in const DRM_BYTEBLOB             blobBuffer,
    __in const DRM_DWORD                cbOffset
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMediaBuffer_Initialize );

    ChkArg(pAsfMediaBuffer);
    ZEROMEM(pAsfMediaBuffer, SIZEOF(ASF_MEDIA_BUFFER));

    pAsfMediaBuffer->cRef            = 1;
    pAsfMediaBuffer->blobBuffer      = blobBuffer;
    pAsfMediaBuffer->cbCurrentOffset = cbOffset;
    pAsfMediaBuffer->cbCurrentLength = blobBuffer.cbBlob;
    pAsfMediaBuffer->dwFlags         = 0;

    pAsfMediaBuffer->pOriginalBuffer = NULL;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_RESULT ASFMediaBuffer_InitializeWrapper(
    __inout    ASF_MEDIA_BUFFER * const pAsfMediaBufferWrapper,
    __in       ASF_MEDIA_BUFFER * const pOriginalAsfMediaBuffer,
    __in const DRM_DWORD                cbOffset,
    __in const DRM_DWORD                cbCurrentLength
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMediaBuffer_InitializeWrapper );

    ChkArg(pAsfMediaBufferWrapper);
    ChkArg(pOriginalAsfMediaBuffer);
    ZEROMEM(pAsfMediaBufferWrapper, SIZEOF(ASF_MEDIA_BUFFER));

    pAsfMediaBufferWrapper->cRef            = 1;
    pAsfMediaBufferWrapper->blobBuffer      = pOriginalAsfMediaBuffer->blobBuffer,
    pAsfMediaBufferWrapper->cbCurrentOffset = cbOffset;
    pAsfMediaBufferWrapper->cbCurrentLength = cbCurrentLength;
    pAsfMediaBufferWrapper->dwFlags         = ASF_MEDIA_BUFFER_FLAGS_WRAPPER;

    pAsfMediaBufferWrapper->pOriginalBuffer = pOriginalAsfMediaBuffer;
    ChkDR( ASFMediaBuffer_AddRef( pOriginalAsfMediaBuffer ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_VOID ASFMediaBuffer_Uninitialize(
    __inout    ASF_MEDIA_BUFFER * const pAsfMediaBuffer
)
{
    if ( NULL != pAsfMediaBuffer )
    {
        if ( (pAsfMediaBuffer->dwFlags & ASF_MEDIA_BUFFER_FLAGS_WRAPPER) == 0 )
        {
            SAFE_OEM_FREE( pAsfMediaBuffer->blobBuffer.pbBlob );
        }

        if ( NULL != pAsfMediaBuffer->pOriginalBuffer )
        {
            ASFMediaBuffer_Release( pAsfMediaBuffer->pOriginalBuffer );
            pAsfMediaBuffer->pOriginalBuffer = NULL;
        }
    }
}



DRM_RESULT ASFMediaBuffer_AddRef(
    __inout    ASF_MEDIA_BUFFER * const pAsfMediaBuffer
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != pAsfMediaBuffer );

    pAsfMediaBuffer->cRef++;

 ErrorExit:
    return dr;
}



DRM_VOID ASFMediaBuffer_Release(
    __inout_opt    ASF_MEDIA_BUFFER        *pAsfMediaBuffer
)
{
    if ( NULL != pAsfMediaBuffer )
    {
        pAsfMediaBuffer->cRef--;

        if ( 0 == pAsfMediaBuffer->cRef )
        {
            ASFMediaBuffer_Uninitialize( pAsfMediaBuffer );
            SAFE_OEM_FREE( pAsfMediaBuffer );
        }
    }
}


EXIT_PK_NAMESPACE_CODE;
