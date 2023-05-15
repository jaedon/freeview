/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmutilities.h>
#include <drmtoolsutils.h>
#include <drmm2tstypes.h>
#include <drmm2tsencryptor.h>
#include <drmlicgen.h>

ENTER_PK_NAMESPACE_CODE;

#define BUFFER_SIZE                         188 * 500
#define GET_CATEGORY_NAME( eCategory )      ( eCategory == eDRM_M2TS_LOG_CATEGORY_INFORMATION ? "Information" : ( eCategory == eDRM_M2TS_LOG_CATEGORY_WARNING ? "Warning" : "Error" ) )

#define MAX_ENCRYPTOR_ERROR                 500             /* This number should be large enought that M2TS encryptor will never have that many of error codes */

typedef struct __tagSummaryEntry
{
    DRM_M2TS_LOG_CATEGORY   eCategory;
    DRM_RESULT              drResult;
    DRM_DWORD               cInstances;
} SummaryEntry;

DRM_BOOL        g_fSummaryLog                           = TRUE;
DRM_DWORD       g_iSummaryLog                           = 0;
SummaryEntry    g_rgSummaryLog[ MAX_ENCRYPTOR_ERROR ];

/* Fixed content key and key ID for encryption */
DRM_KID         g_okeyID                = { 0x84, 0x19, 0xa7, 0x26, 0x82, 0xdd, 0x71, 0x4a, 0xa6, 0x83, 0xf0, 0xca, 0x8d, 0xd1, 0x84, 0xa8 };
DRM_BYTE        g_rgbContentkey[]       = { 0xb2, 0xc7, 0xee, 0x0a, 0xdd, 0x4e, 0x46, 0x94, 0xe3, 0xae, 0xcc, 0x2d, 0xa0, 0x6e, 0x60, 0x2e }; /* ssfuCt1ORpTjrswtoG5gLg== */
DRM_DWORD       g_dwIV                  = 1;

/* Fixed content key and key ID for key rotation (leaf KID = {0c1bd9be-4a0d-45d3-bd2f-59e4dec4fc07}, root KID = {fd263f33-e60f-4203-984f-831d48eef792})  */
DRM_KID         g_okeyIDLeaf            = { 0xbe, 0xd9, 0x1b, 0x0c, 0x0d, 0x4a, 0xd3, 0x45, 0xbd, 0x2f, 0x59, 0xe4, 0xde, 0xc4, 0xfc, 0x07 };
DRM_BYTE        g_rgbContentkeyKR[]     = { 0xbd, 0xfe, 0x94, 0x6c, 0x17, 0x62, 0x34, 0x54, 0x8e, 0x82, 0x2b, 0xef, 0xe0, 0x6c, 0xff, 0x6d }; /* vf6UbBdiNFSOgivv4Gz/bQ== */
DRM_BYTE        g_rgbPROBlob[]          = "IgEAAAEAAwAYAUVTVAAAAQABAAABGAAAAQq+2RsMDUrTRb0vWeTexPwHU9M7VYC1tEmAWabPGcZXgwAAAABYTVIAAAAAA1PTO1WAtbRJgFmmzxnGV4MAAwABAAAAygACAAQAAAAIAAMAAgAAADQAAAATAAAADE/0TDYAAQAyAAAADAAAAAAAAQA0AAAACgAAAAAAMwAAAAoAAgADAAkAAABqAAEACgAAAD6+2RsMDUrTRb0vWeTexPwHAAEAAgAg28f5ZAPdBjdzQF05CcfILYoeVyTiBzsM0ae+5mg5AqMAAQA7AAAAJDM/Jv0P5gNCmE+DHUju95IAAQAIZm6wUxoxpGUAAQALAAAAHAABABB0dYkOKJFBafN4kmmVhPFFAAA=";


/* Fixed content key and key ID for key rotation (Root KID = {3d4e6055-067e-4383-b38d-6d0380942616}, root KID = {fd263f33-e60f-4203-984f-831d48eef792})  */
DRM_KID         g_okeyIDLeaf2           = { 0x55, 0x60, 0x4e, 0x3d, 0x7e, 0x06, 0x83, 0x43, 0xb3, 0x8d, 0x6d, 0x03, 0x80, 0x94, 0x26, 0x16 };
DRM_BYTE        g_rgbContentkeyKR2[]    = { 0x20, 0x99, 0x8b, 0x10, 0xb1, 0xd7, 0x78, 0x34, 0xc6, 0x3e, 0x29, 0x16, 0x28, 0xc2, 0x35, 0xf4 }; /* IJmLELHXeDTGPikWKMI19A== */
DRM_BYTE        g_rgbPROBlob2[]         = "IgEAAAEAAwAYAUVTVAAAAQABAAABGAAAAQpVYE49fgaDQ7ONbQOAlCYWnhtWsj4kf0293hxfmfqnPAAAAABYTVIAAAAAA54bVrI+JH9Nvd4cX5n6pzwAAwABAAAAygACAAQAAAAIAAMAAgAAADQAAAATAAAADE/0SYIAAQAyAAAADAAAAAAAAQA0AAAACgAAAAAAMwAAAAoAAgADAAkAAABqAAEACgAAAD5VYE49fgaDQ7ONbQOAlCYWAAEAAgAgikePF/Mzj1UzHv9LaD/pWcpegBjfUTYgABDbUEV99WYAAQA7AAAAJDM/Jv0P5gNCmE+DHUju95IAAQAIZm6wUxoxpGUAAQALAAAAHAABABAd/45bX5Az5oh2WsLduCq+AAA=";

typedef struct __tagKeyData
{
    DRM_LICENSE_HANDLE      hLicense;
    DRM_KID                *poKID;
    DRM_DWORD               dwDuration;
    DRM_DWORD               cbContentKey;
    DRM_BYTE               *pbContentKey;
    DRM_DWORD               cbPROBlob;
    DRM_BYTE               *pbPROBlob;
} KeyData;

KeyData g_rgoKeyData[] = {
    { DRM_LICENSE_HANDLE_INVALID, &g_okeyID,      0, SIZEOF( DRM_KID ), g_rgbContentkey,    0,                           NULL          },
    { DRM_LICENSE_HANDLE_INVALID, &g_okeyIDLeaf,  2, SIZEOF( DRM_KID ), g_rgbContentkeyKR,  SIZEOF( g_rgbPROBlob )  - 1, g_rgbPROBlob  },
    { DRM_LICENSE_HANDLE_INVALID, &g_okeyIDLeaf2, 2, SIZEOF( DRM_KID ), g_rgbContentkeyKR2, SIZEOF( g_rgbPROBlob2 ) - 1, g_rgbPROBlob2 },
};


static void _PrintUsage()
{
    printf("usage: m2tsencryptor options\n\n");
    printf("Options: \n");
    printf("  -?                  display this help menu\n");
    printf("  -l:[s|d]            specify the type of logs, the options are s-summary(default) and d-detailed\n");
    printf("  -r:duration         specify key rotation and duration (in seconds) for each key\n");
    printf("  -i:inputTSFile      specify the input TS file to encrypt\n");
    printf("  -o:outputTSFile     specify the output and encrypted TS file\n");
}
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM, "Ignore const parameter" )
static DRM_RESULT _FindLicenseIndex(
    __in      const DRM_LICENSE_HANDLE         f_hLicense,
    __out           DRM_DWORD                 *f_piKey )
PREFAST_POP /* Ignore const parameter */
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_BOOL    fFound  = FALSE;
    DRM_DWORD   iKey    = 0;

    ChkArg( f_piKey != NULL );

    for( iKey = 0; iKey < NO_OF( g_rgoKeyData ) && !fFound; iKey++ )
    {
        if( g_rgoKeyData[ iKey ].hLicense == f_hLicense )
        {
            *f_piKey = iKey;
            fFound = TRUE;
        }
    }

    ChkBOOL( fFound, DRM_E_NOT_FOUND );

ErrorExit:
    return dr;
}

/*
** Fake local license generation APIs:
** To encrypt the content with a fixed key ID {26a71984-dd82-4a71-a683-f0ca8dd184a8}
*/

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM, "Ignore const parameter for all of these stub functions" )

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_InitializePolicyDescriptor(
    __inout     DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor )
{
    UNREFERENCED_PARAMETER( f_poDescriptor );
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateLicense(
    __inout                               DRM_APP_CONTEXT                     *f_poAppContext,
    __in                            const DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor,
    __in                            const DRM_LOCAL_LICENSE_TYPE               f_eLicensType,
    __in                            const DRM_KID                             *f_pKeyId,
    __in_opt                        const DRM_DWORD                            f_cbRemoteCert,
    __in_ecount_opt(f_cbRemoteCert) const DRM_BYTE                            *f_pbRemoteCert,
    __in_opt                        const DRM_LICENSE_HANDLE                   f_hRootLicense,
    __out                                 DRM_LICENSE_HANDLE                  *f_phLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_poDescriptor );
    UNREFERENCED_PARAMETER( f_eLicensType );
    UNREFERENCED_PARAMETER( f_pKeyId );
    UNREFERENCED_PARAMETER( f_cbRemoteCert );
    UNREFERENCED_PARAMETER( f_pbRemoteCert );
    UNREFERENCED_PARAMETER( f_hRootLicense );

    ChkArg( f_phLicense != NULL );
    *f_phLicense = DRM_LICENSE_HANDLE_INVALID;

    ChkMem( *f_phLicense = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_LOCAL_LICENSE_SESSION_CONTEXT ) ) );
    MEMSET( *f_phLicense, 0, SIZEOF( DRM_LOCAL_LICENSE_SESSION_CONTEXT ) );

    ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)(*f_phLicense))->dwRefCount = 1;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreatePlayReadyObject(
    __in_opt                      const DRM_LICENSE_HANDLE       f_hLicense,
    __in_opt                      const DRM_DWORD                f_cbRMHeader,
    __in_opt                      const DRM_BYTE                *f_pbRMHeader,
    __inout                             DRM_DWORD               *f_pcbPROBlob,
    __out_bcount( *f_pcbPROBlob )       DRM_BYTE               **f_ppbPROBlob )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iKey    = 0;

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );
    ChkArg( f_pcbPROBlob != NULL );
    ChkArg( f_ppbPROBlob != NULL );

    ChkDR( _FindLicenseIndex( f_hLicense, &iKey ) );

    ChkMem( *f_ppbPROBlob = (DRM_BYTE *)Drm_Prnd_MemAlloc( g_rgoKeyData[ iKey ].cbPROBlob ) );
    MEMCPY( *f_ppbPROBlob, g_rgoKeyData[ iKey ].pbPROBlob, g_rgoKeyData[ iKey ].cbPROBlob );
    *f_pcbPROBlob = g_rgoKeyData[ iKey ].cbPROBlob;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateRMHeader(
    __in_opt                      const DRM_KID         *f_pKeyId,
    __out                               DRM_DWORD       *f_pcbRMHeader,
    __out_bcount( *f_pcbRMHeader )      DRM_BYTE       **f_ppbRMHeader )
{
    UNREFERENCED_PARAMETER( f_pKeyId );

    *f_pcbRMHeader = 0;
    *f_ppbRMHeader = NULL;

    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_StoreLicense(
    __in              const DRM_LICENSE_HANDLE                   f_hLicense,
    __in              const DRM_LOCAL_LICENSE_STORE              f_eLicenseStore )
{
    UNREFERENCED_PARAMETER( f_hLicense );
    UNREFERENCED_PARAMETER( f_eLicenseStore );

    return DRM_SUCCESS;
}

PREFAST_POP /* Ignore const parameter for all of these stub functions */

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_EncryptSample(
    __in                          const DRM_LICENSE_HANDLE       f_hLicense,
    __in                          const DRM_DWORD                f_cSubsamples,
    __in_ecount( f_cSubsamples )  const DRM_DWORD               *f_pcbSubamples,
    __inout_ecount( f_cSubsamples )     DRM_BYTE               **f_ppbSubamples,
    __out                               DRM_UINT64              *f_pqwIV )
{
    DRM_RESULT                      dr              = DRM_SUCCESS;
    DRM_AES_COUNTER_MODE_CONTEXT    oCtrContext     = { 0 };
    DRM_CIPHER_CONTEXT              oCipherContext;
    DRM_DWORD                       iSubsample      = 0;
    DRM_DWORD                       dwOffset        = 0;
    DRM_DWORD                       cbBuffer        = 0;
    DRM_BYTE                       *pbBuffer        = NULL;
    DRM_DWORD                       iKey            = 0;

    ChkArg( f_hLicense != NULL );

    ChkDR( _FindLicenseIndex( f_hLicense, &iKey ) );

    oCtrContext.qwInitializationVector = DRM_UI64HL( 0, g_dwIV++ );

    for( iSubsample = 0; iSubsample < f_cSubsamples; iSubsample++ )
    {
        cbBuffer += f_pcbSubamples[ iSubsample ];
    }

    ChkMem( pbBuffer = (DRM_BYTE *)Oem_MemAlloc( cbBuffer ) );

    dwOffset = 0;
    for( iSubsample = 0; iSubsample < f_cSubsamples; iSubsample++ )
    {
        MEMCPY( pbBuffer + dwOffset, f_ppbSubamples[ iSubsample ], f_pcbSubamples[ iSubsample ] );
        dwOffset += f_pcbSubamples[ iSubsample ];
    }

    ChkDR( DRM_CPHR_Init( &oCipherContext,
                          eDRM_AES_COUNTER_CIPHER,
                          g_rgoKeyData[ iKey ].cbContentKey,
                          g_rgoKeyData[ iKey ].pbContentKey ) );


    ChkDR( DRM_CPHR_Encrypt( &oCipherContext,
                             &oCtrContext,
                             cbBuffer,
                             pbBuffer ) );

    dwOffset = 0;
    for( iSubsample = 0; iSubsample < f_cSubsamples; iSubsample++ )
    {
        MEMCPY( f_ppbSubamples[ iSubsample ], pbBuffer + dwOffset, f_pcbSubamples[ iSubsample ] );
        dwOffset += f_pcbSubamples[ iSubsample ];
    }

    *f_pqwIV = oCtrContext.qwInitializationVector;

ErrorExit:
    SAFE_OEM_FREE( pbBuffer );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_AddRef(
    __inout     DRM_LICENSE_HANDLE      f_hLicense )
{
    DRM_RESULT                          dr                  = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT  *pLicenseContext     = ( DRM_LOCAL_LICENSE_SESSION_CONTEXT * )f_hLicense;

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );

    pLicenseContext->dwRefCount++;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_Release(
    __inout     DRM_LICENSE_HANDLE      *f_phLicense )
{
    DRM_RESULT                          dr                  = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT  *pLicenseContext     = NULL;

    ChkArg( f_phLicense  != NULL );
    ChkArg( *f_phLicense != DRM_LICENSE_HANDLE_INVALID );

    pLicenseContext = ( DRM_LOCAL_LICENSE_SESSION_CONTEXT * )*f_phLicense;

    ChkArg( pLicenseContext->dwRefCount > 0 );

    pLicenseContext->dwRefCount--;
    if( pLicenseContext->dwRefCount == 0 )
    {
        SAFE_OEM_FREE( pLicenseContext );
        *f_phLicense = DRM_LICENSE_HANDLE_INVALID;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_BYTE* DRM_CALL Drm_Prnd_MemAlloc(
    __in DRM_DWORD f_cbToAlloc )
{
    if( f_cbToAlloc > 0 )
    {
        /* This API *must* be implemented using Oem_MemAlloc.  The PRND implementation requires it. */
        return (DRM_BYTE*)Oem_MemAlloc( f_cbToAlloc );
    }
    else
    {
        return NULL;
    }
}

DRM_API DRM_VOID DRM_CALL Drm_Prnd_MemFree(
    __in DRM_BYTE *f_pbToFree )
{
    if( f_pbToFree != NULL )
    {
        /* This API *must* be implemented using Oem_MemFree.  The PRND implementation requires it. */
        Oem_MemFree( f_pbToFree );
    }
}

static DRM_RESULT _GetNextKey(
    __in      const DRM_BOOL    f_fkeyRotation,
    __inout         DRM_DWORD  *f_piKey )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iKey    = 0;

    ChkArg( f_piKey != NULL );
    ChkArg( *f_piKey < NO_OF( g_rgoKeyData ) );

    for( iKey = 0; iKey < NO_OF( g_rgoKeyData ); iKey++ )
    {
        (*f_piKey)++;
        if( *f_piKey >= NO_OF( g_rgoKeyData ) )
        {
            *f_piKey = 0;
        }

        if( f_fkeyRotation && g_rgoKeyData[ *f_piKey ].dwDuration > 0 )
        {
            break;
        }
        else if( !f_fkeyRotation && g_rgoKeyData[ *f_piKey ].dwDuration == 0 )
        {
            break;
        }
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _CreateLocalLicenseHandles(
    __inout         DRM_APP_CONTEXT         *f_poAppContext,
    __in      const DRM_DWORD                f_dwDuration )
{
    DRM_RESULT                              dr                      = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     oLicensePolicyDecriptor = { 0 };
    DRM_DWORD                               iKey                    = 0;

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = TRUE;

    for( iKey = 0; iKey < NO_OF( g_rgoKeyData ); iKey++ )
    {
        /* Create license */
        ChkDR( Drm_LocalLicense_CreateLicense(
            f_poAppContext,
            &oLicensePolicyDecriptor,
            eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
            g_rgoKeyData[ iKey ].poKID,
            0,
            NULL,
            DRM_LICENSE_HANDLE_INVALID,
            &g_rgoKeyData[ iKey ].hLicense ) );

        if( g_rgoKeyData[ iKey ].cbPROBlob > 0 )
        {
            DRM_SUBSTRING dsstrTemp = EMPTY_DRM_SUBSTRING;

            /* B64 decode the PRO blob */
            dsstrTemp.m_ich = 0;
            dsstrTemp.m_cch = g_rgoKeyData[ iKey ].cbPROBlob;

            PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions.")

            ChkDR( DRM_B64_DecodeA( (DRM_CHAR *)g_rgoKeyData[ iKey ].pbPROBlob,
                &dsstrTemp,
                &g_rgoKeyData[ iKey ].cbPROBlob,
                NULL,
                DRM_BASE64_DECODE_IN_PLACE ) );

            PREFAST_POP /* ignore prefast warning about ANSI functions */

            /* Update the duration */
            g_rgoKeyData[ iKey ].dwDuration = f_dwDuration;
        }
    }

ErrorExit:
     return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM, "Ignore const parameter" )
static DRM_VOID DRM_CALL _LoggingCallback(
    __inout    DRM_VOID     *f_pCallbackContext,
    __in const DRM_M2TS_LOG *f_poLog )
PREFAST_POP /* ignore prefast warning for const parameter */
{
    UNREFERENCED_PARAMETER( f_pCallbackContext );

    if( f_poLog != NULL )
    {
        if( g_fSummaryLog )
        {
            DRM_BOOL    fProcessed  = FALSE;
            DRM_DWORD   iEntry      = 0;

            for( iEntry = 0; iEntry < g_iSummaryLog && !fProcessed; iEntry++ )
            {
                if( g_rgSummaryLog[ iEntry ].drResult == f_poLog->drResult )
                {
                    fProcessed = TRUE;
                    g_rgSummaryLog[ iEntry ].cInstances++;
                }
            }

            if( !fProcessed )
            {
                if( g_iSummaryLog < NO_OF( g_rgSummaryLog ) )
                {
                    g_rgSummaryLog[ g_iSummaryLog ].eCategory = f_poLog->eCategory;
                    g_rgSummaryLog[ g_iSummaryLog ].drResult = f_poLog->drResult;
                    g_rgSummaryLog[ g_iSummaryLog ].cInstances++;
                    g_iSummaryLog++;
                }
                else
                {
                    printf( "WARNING: Summary log is full!\r\n" );
                }
            }
        }
        else
        {
            switch( f_poLog->eDetailsType )
            {
            case eDRM_M2TS_LOG_DETAILS_TYPE_PACKET:
                printf( "%s: 0x%8.8x - PID=%d, Packet#=%d, Dropped=%s\r\n",
                    GET_CATEGORY_NAME( f_poLog->eCategory ),
                    f_poLog->drResult,
                    f_poLog->Details.Packet.wPID,
                    f_poLog->Details.Packet.dwPacketNumber,
                    f_poLog->Details.Packet.fDropped ? "Yes" : "No" );
                break;

            case eDRM_M2TS_LOG_DETAILS_TYPE_PES:
                printf( "%s: 0x%8.8x - PID=%d, StreamType=%d, DataAligned=%s, PTS=%u, FirstPacket#=%d, LastPacket#=%d, Dropped=%s\r\n",
                    GET_CATEGORY_NAME( f_poLog->eCategory ),
                    f_poLog->drResult,
                    f_poLog->Details.PES.wPID,
                    f_poLog->Details.PES.bStreamType,
                    f_poLog->Details.PES.bDataAligned == 1 ? "Yes" : "No",
                    DRM_UI64Low32( f_poLog->Details.PES.qwPTS ),
                    f_poLog->Details.PES.dwFirstPacketNumber,
                    f_poLog->Details.PES.dwLastPacketNumber,
                    f_poLog->Details.PES.fDropped ? "Yes" : "No" );
                break;

            case eDRM_M2TS_LOG_DETAILS_TYPE_KEY_ROTATION:
                printf( "%s: Key Rotation - Duration=%u, Aggregated Duration=%u, Latest Sample's PTS=%u\r\n",
                    GET_CATEGORY_NAME( f_poLog->eCategory ),
                    DRM_UI64Low32( f_poLog->Details.KeyRotation.qwCurrentKeyDuration ),
                    DRM_UI64Low32( f_poLog->Details.KeyRotation.qwAggregatedDuration ),
                    DRM_UI64Low32( f_poLog->Details.KeyRotation.qwPTS ) );
                break;
            }
        }
    }
    else
    {
        DRMASSERT( FALSE );
        printf( "ERROR - f_poLog is NULL!!!\r\n" );
    }
}

static DRM_VOID _PrintSummaryLog()
{
    if( g_fSummaryLog && g_iSummaryLog > 0 )
    {
        DRM_DWORD iEntry = 0;

        for( iEntry = 0; iEntry < g_iSummaryLog; iEntry++ )
        {
            printf( "%s: 0x%8.8x - %d times\r\n", GET_CATEGORY_NAME( g_rgSummaryLog[ iEntry ].eCategory ), g_rgSummaryLog[ iEntry ].drResult, g_rgSummaryLog[ iEntry ].cInstances );
        }
    }
}

static DRM_RESULT _EncryptFile(
    __inout         DRM_APP_CONTEXT     *f_poAppContext,
    __in      const DRM_BOOL             f_fKeyRotation,
    __in      const DRM_DWORD            f_dwDuration,
    __in      const DRM_WCHAR           *f_pwszInFile,
    __in      const DRM_WCHAR           *f_pwszOutFile )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_BYTE                    *pbInBuffer             = NULL;
    DRM_DWORD                    cbInBuffer             = BUFFER_SIZE;
    DRM_BYTE                    *pbOutBuffer            = NULL;
    DRM_DWORD                    cbOutBuffer            = BUFFER_SIZE;
    DRM_DWORD                    cbRead                 = 0;
    DRM_DWORD                    cbWritten              = 0;
    DRM_BOOL                     fLastBatch             = FALSE;
    DRM_DWORD                    dwTickCountBegin       = 0;
    DRM_DWORD                    dwTickCountEncStart    = 0;
    DRM_DWORD                    dwTickCountEncEnd      = 0;
    DRM_DWORD                    dwTickCountEncryption  = 0;
    DRM_DWORD                    iKey                   = 0;
    OEM_FILEHDL                  hInFile                = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL                  hOutFile               = OEM_INVALID_HANDLE_VALUE;
    DRM_M2TS_ENCRYPTOR_HANDLE    hEncryptor             = DRM_M2TS_ENCRYPTOR_HANDLE_INVALID;
    DRM_M2TS_ENCRYPTION_PROPERTY oProperty;

    ChkArg( f_pwszInFile != NULL );
    ChkArg( f_pwszOutFile != NULL );

    dwTickCountBegin = Oem_Clock_GetTickCount( NULL );

    ChkBOOL( ( hInFile = Oem_File_Open( NULL,
                                        f_pwszInFile,
                                        OEM_GENERIC_READ,
                                        OEM_FILE_SHARE_READ,
                                        OEM_OPEN_EXISTING,
                                        OEM_ATTRIBUTE_NORMAL ) ) != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    ChkBOOL( ( hOutFile = Oem_File_Open( NULL,
                                         f_pwszOutFile,
                                         OEM_GENERIC_WRITE,
                                         OEM_FILE_SHARE_NONE,
                                         OEM_CREATE_ALWAYS,
                                         OEM_ATTRIBUTE_NORMAL ) ) != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    ChkMem( pbInBuffer  = ( DRM_BYTE * )Oem_MemAlloc( cbInBuffer ) );
    ChkMem( pbOutBuffer = ( DRM_BYTE * )Oem_MemAlloc( cbOutBuffer ) );

    printf( "Encrypting %S...\r\n", f_pwszInFile );

    ChkDR( Drm_M2ts_Initialize( 0, &hEncryptor ) );   /* no limit for memory usage by the encryptor */

    MEMSET( &oProperty, 0, SIZEOF( oProperty ) );
    oProperty.ePropertyType = eDRM_M2TS_PROPERTY_TYPE_ENABLE_LOGGING;
    oProperty.Value.LoggingCallback.pfnLoggingCallback = _LoggingCallback;
    ChkDR( Drm_M2ts_SetEncryptionProperty( hEncryptor, &oProperty ) );

    ChkDR( _CreateLocalLicenseHandles( f_poAppContext, f_dwDuration ) );

    while( !fLastBatch )
    {
        /* Read a predefined number of packets from the input TS file */
        if( !Oem_File_Read( hInFile, ( DRM_VOID * )pbInBuffer, cbInBuffer, &cbRead ) )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        if( cbRead == 0 )
        {
            fLastBatch = TRUE;
        }

        /* Encrypt the bytes read */
        do
        {
            cbOutBuffer = BUFFER_SIZE;
            dwTickCountEncStart = Oem_Clock_GetTickCount( NULL );
            dr = Drm_M2ts_Encrypt(  hEncryptor,
                                    cbRead > 0 ? pbInBuffer : NULL,
                                    cbRead,
                                    pbOutBuffer,
                                    &cbOutBuffer,
                                    fLastBatch );
            dwTickCountEncEnd = Oem_Clock_GetTickCount( NULL );
            dwTickCountEncryption += dwTickCountEncEnd >= dwTickCountEncStart
                                     ? dwTickCountEncEnd - dwTickCountEncStart
                                     : MAX_UNSIGNED_TYPE( DRM_DWORD ) - dwTickCountEncStart + dwTickCountEncEnd;
            cbRead = 0;

            if( cbOutBuffer > 0 )
            {
                if( !Oem_File_Write( hOutFile, pbOutBuffer, cbOutBuffer, &cbWritten ) )
                {
                    ChkDR( DRM_E_FILE_WRITE_ERROR );
                }
            }

            if( dr == DRM_E_M2TS_NEED_KEY_DATA )
            {
                ChkDR( _GetNextKey( f_fKeyRotation, &iKey ) );
                ChkDR( Drm_M2ts_SetKeyData( hEncryptor,
                                            g_rgoKeyData[ iKey ].hLicense,
                                            (DRM_GUID *)g_rgoKeyData[ iKey ].poKID,
                                            f_fKeyRotation ? eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_LICENSE_IN_PRO : eDRM_M2TS_PRO_TYPE_NO_PRO,
                                            g_rgoKeyData[ iKey ].dwDuration ) );
            }
            else
            {
                ChkDR( dr );
                if( dr != DRM_S_MORE_DATA )
                {
                    break;
                }
            }

        } while ( TRUE );
    }

ErrorExit:
    {
        DRM_RESULT dr2 = DRM_SUCCESS;

#if DBG
        const DRM_M2TS_ENCRYPTOR_CONTEXT *pEncContext = (const DRM_M2TS_ENCRYPTOR_CONTEXT *)hEncryptor;
        if( pEncContext != NULL )
        {
            printf( "Memory usage: %d\r\n", pEncContext->cbActualMaxHeapAllocated );
        }
#endif /* DBG */

        dr2 = Drm_M2ts_Uninitialize( hEncryptor );
        if( DRM_FAILED( dr2 ) )
        {
            printf( "Error: Drm_M2ts_Uninitialize failed with 0x%8.8x\r\n", dr2 );
        }
    }

    SAFE_OEM_FREE( pbInBuffer );
    SAFE_OEM_FREE( pbOutBuffer );

    if( hInFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hInFile );
    }

    if( hOutFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hOutFile );
    }

    {
        iKey = 0;
        for( iKey = 0; iKey < NO_OF( g_rgoKeyData ); iKey++ )
        {
            if( g_rgoKeyData[ iKey ].hLicense != DRM_LICENSE_HANDLE_INVALID )
            {
                DRMASSERT( (( DRM_LOCAL_LICENSE_SESSION_CONTEXT * )g_rgoKeyData[ iKey ].hLicense)->dwRefCount == 1 );
                (DRM_VOID)Drm_LocalLicense_Release( &g_rgoKeyData[ iKey ].hLicense );
            }
        }
    }

    _PrintSummaryLog();

    if( DRM_SUCCEEDED( dr )  )
    {
        dwTickCountEncEnd = Oem_Clock_GetTickCount( NULL );
        printf( "Total Duration: %d Encryption: %d (in ticks)\r\n",
                dwTickCountEncEnd >= dwTickCountBegin ? dwTickCountEncEnd - dwTickCountBegin : MAX_UNSIGNED_TYPE( DRM_DWORD ) - dwTickCountBegin + dwTickCountEncEnd,
                dwTickCountEncryption );
    }

    return dr;
}
EXIT_PK_NAMESPACE_CODE;

/*************************************************************************
*
*   Console Program Main Entry  Point
*
**************************************************************************/

DRM_LONG DRM_CALL DRM_Main(DRM_LONG argc, DRM_WCHAR* argv[])
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_APP_CONTEXT     *pAppContext        = NULL;
    DRM_WCHAR            wchOptionChar[3];
    DRM_CONST_STRING     wszParam[3]        = { { 0 }, { 0 }, { 0 } };
    DRM_CONST_STRING     wszExtra           = { 0 };
    const DRM_WCHAR     *pszInFile          = NULL;
    const DRM_WCHAR     *pszOutFile         = NULL;
    DRM_BOOL             fKeyRotation       = FALSE;
    DRM_DWORD            dwDuration         = 2; /* default to two seconds if not specified */
    DRM_BOOL             fHelp              = FALSE;
    DRM_DWORD            iOption            = 0;

    if( argc < 3 )
    {
        _PrintUsage();
        ChkDR( DRM_E_INVALID_COMMAND_LINE );
    }

    ChkMem( pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
    MEMSET( pAppContext, 0, SIZEOF( DRM_APP_CONTEXT ) );

    /* parse the arguments */
    for( iOption = 0; iOption < ( DRM_DWORD )( argc - 1 ); iOption++ )
    {
        if( !DRM_CMD_ParseCmdLine( argv[ iOption + 1 ], &wchOptionChar[ iOption ], &wszParam[ iOption ], &wszExtra ) )
        {
            _PrintUsage();
            ChkDR( DRM_E_INVALID_COMMAND_LINE );
        }

        switch( DRMCRT_towlower( wchOptionChar[ iOption ] ) )
        {
        case L'i':
            pszInFile = wszParam[ iOption ].pwszString;
            break;

        case L'o':
            pszOutFile = wszParam[ iOption ].pwszString;
            break;

        case L'l':
            g_fSummaryLog = DRMCRT_towlower( wszParam[ iOption ].pwszString[0] ) != L'd';
            break;

        case L'r':
            fKeyRotation = TRUE;
            if( wszParam[ iOption ].pwszString != NULL )
            {
                dwDuration = (DRM_DWORD)_wtol( wszParam[ iOption ].pwszString );
            }
            break;

        case L'?':
            fHelp = TRUE;
            break;

        default:
            ChkDR( DRM_E_INVALID_COMMAND_LINE );
            break;
        }
    }

    if( fHelp )
    {
        _PrintUsage();
    }
    else
    {
        if( pszInFile != NULL &&  pszOutFile != NULL )
        {
            ChkDR( _EncryptFile( pAppContext, fKeyRotation, dwDuration, pszInFile, pszOutFile ) );
            printf( "Success!\r\n" );
        }
        else
        {
            _PrintUsage();
            ChkDR( DRM_E_INVALID_COMMAND_LINE );
        }
    }

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }

    SAFE_OEM_FREE( pAppContext );

    return dr;
}

