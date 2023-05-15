/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndtcontextsizes.h>
#include <ndtasf_internal.h>
#include <ndtasftypes.h>
#include <asfsample.h>
#include <asfmediabuffer.h>

ENTER_PK_NAMESPACE_CODE;


DRM_RESULT ASFSample_Initialize(
    __out ASF_SAMPLE * const f_pAsfSample
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_Initialize );

    ChkArg(f_pAsfSample);
    ZEROMEM(f_pAsfSample, SIZEOF(ASF_SAMPLE));

    ChkDR( DRM_PtrList_Initialize( &f_pAsfSample->MediaBuffers, 128 ) );
    f_pAsfSample->cRef = 1;

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_VOID _ASFSample_RemoveAllBuffers( 
    __in ASF_SAMPLE * const f_pAsfSample
)
{
    ASF_MEDIA_BUFFER *pMediaBuffer;

    if ( NULL == f_pAsfSample )
    {
        goto ErrorExit;
    }

    while ( DRM_SUCCEEDED( DRM_PtrList_RemoveHead( &f_pAsfSample->MediaBuffers, (DRM_VOID **) &pMediaBuffer ) ) )
    {
        if ( NULL != pMediaBuffer )
        {
            ASFMediaBuffer_Release( pMediaBuffer );
            pMediaBuffer = NULL;
        }
    }
    f_pAsfSample->cbTotalLength = 0;

 ErrorExit:
    return;
}


static DRM_VOID _ASFSample_Uninitialize(
    __out ASF_SAMPLE * const f_pAsfSample
)
{
    if ( NULL != f_pAsfSample )
    {
        _ASFSample_RemoveAllBuffers( f_pAsfSample );
        DRM_PtrList_Uninitialize( &f_pAsfSample->MediaBuffers );
        ZEROMEM( f_pAsfSample, SIZEOF( ASF_SAMPLE ) );
    }
}


DRM_RESULT ASFSample_GetBufferCount(
    __in const  ASF_SAMPLE * const f_pAsfSample,
    __out       DRM_DWORD  * const f_pdwBufferCount
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pAsfSample);
    ChkArg(f_pdwBufferCount);
    
    *f_pdwBufferCount = f_pAsfSample->MediaBuffers.dwCurrentSize;

 ErrorExit:
    return dr;
}


DRM_RESULT ASFSample_GetBufferByIndex(
    __in const  ASF_SAMPLE  * const f_pAsfSample,
    __in const  DRM_DWORD           f_dwIndex,
    __out       ASF_MEDIA_BUFFER  **f_ppBuffer
)
{
    DRM_RESULT  dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_GetBufferByIndex );

    ChkArg(f_pAsfSample);
    ChkArg(f_ppBuffer);

    ChkDR( DRM_PtrList_GetByIndex( &f_pAsfSample->MediaBuffers, f_dwIndex, (DRM_VOID **) f_ppBuffer ) );
    ChkDR( ASFMediaBuffer_AddRef( *f_ppBuffer ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_RESULT ASFSample_AddBuffer(
    __inout  ASF_SAMPLE        * const f_pAsfSample,
    __in     ASF_MEDIA_BUFFER  * const f_pBuffer
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_AddBuffer );

    ChkArg( f_pAsfSample );
    ChkArg( f_pBuffer );

    ChkDR( DRM_PtrList_AddTail( &f_pAsfSample->MediaBuffers, f_pBuffer ) );
    ChkDR( ASFMediaBuffer_AddRef( f_pBuffer ) );
    f_pAsfSample->cbTotalLength += f_pBuffer->blobBuffer.cbBlob;

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_VOID ASFSample_RemoveBufferByIndex(
    __in        ASF_SAMPLE * const f_pAsfSample,
    __in const  DRM_DWORD          f_dwIndex
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MEDIA_BUFFER *pMediaBuffer = NULL;
    
    ChkArg(f_pAsfSample != NULL);

    ChkDR( DRM_PtrList_RemoveByIndex( &f_pAsfSample->MediaBuffers, f_dwIndex, (DRM_VOID **) &pMediaBuffer ) );
    
    if ( NULL != pMediaBuffer )
    {
        f_pAsfSample->cbTotalLength -= pMediaBuffer->blobBuffer.cbBlob;
        ASFMediaBuffer_Release( pMediaBuffer );
        pMediaBuffer = NULL;
    }

 ErrorExit:
    return;
}


DRM_RESULT ASFSample_GetTotalLength(
    __in const ASF_SAMPLE * const f_pAsfSample,
    __out      DRM_DWORD  * const f_pcbTotalLength
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pAsfSample);
    ChkArg(f_pcbTotalLength);
    
    *f_pcbTotalLength = f_pAsfSample->cbTotalLength;

 ErrorExit:
    return dr;
}


DRM_RESULT ASFSample_CopyToBuffer(
    __in const                 ASF_SAMPLE * const f_pAsfSample,
    __out_bcount(*f_pcbBuffer) DRM_BYTE   * const f_pbBuffer,
    __inout                    DRM_DWORD  * const f_pcbBuffer 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    ASF_MEDIA_BUFFER *pMediaBuffer;
    DRM_DWORD cbTotal = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_CopyToBuffer );

    ChkArg( NULL != f_pAsfSample );
    ChkArg( NULL != f_pcbBuffer );
    if ( *f_pcbBuffer < f_pAsfSample->cbTotalLength)
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *f_pcbBuffer = f_pAsfSample->cbTotalLength;
    ChkDR(dr);
    ChkArg( NULL != f_pbBuffer );

    for ( i = 0; i < f_pAsfSample->MediaBuffers.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &f_pAsfSample->MediaBuffers, i, (DRM_VOID **) &pMediaBuffer ) );
        ChkBOOL( cbTotal <= *f_pcbBuffer, DRM_E_BUFFERTOOSMALL );
        ChkBOOL( pMediaBuffer->blobBuffer.cbBlob <= (*f_pcbBuffer - cbTotal), DRM_E_BUFFERTOOSMALL );
        DRM_BYT_CopyBytes(f_pbBuffer, cbTotal, pMediaBuffer->blobBuffer.pbBlob, 0, pMediaBuffer->blobBuffer.cbBlob);
        cbTotal += pMediaBuffer->blobBuffer.cbBlob;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_RESULT ASFSample_AddRef(
    __in     ASF_SAMPLE * const f_pAsfSample
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pAsfSample );

    f_pAsfSample->cRef++;

 ErrorExit:
    return dr;
}


DRM_VOID ASFSample_Release(
    __in  ASF_SAMPLE *f_pAsfSample
)
{
    if ( NULL != f_pAsfSample )
    {
        f_pAsfSample->cRef--;

        if ( 0 == f_pAsfSample->cRef )
        {
            _ASFSample_Uninitialize( f_pAsfSample );
            SAFE_OEM_FREE( f_pAsfSample );
        }
    }
}


EXIT_PK_NAMESPACE_CODE;
