/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndttypes.h>
#include <ndtcontextsizes.h>
#include <ndtmanager_internal.h>
#include <ndtnetmsg.h>
#include <oemaes.h>
#include <ndtencrypt.h>

ENTER_PK_NAMESPACE_CODE;

#define WMDRMNET_LINK_DESCRIPTOR_SIZE    ( \
            2 * SIZEOF_U8      /* Flags */ \
            +                  /* Number Of Extensions */ \
            SIZEOF(DRM_DWORD) )                 /* Length */

#define WMDRMNET_LINK_EXTENSION_HEADER_SIZE  ( \
            SIZEOF_U8+                  /* Extension Type */ \
            SIZEOF(DRM_WORD) )                  /* Extension Length */ 

#define WMDRMNET_LINK_EXTENSION_KEYID_SIZE  ( \
            SIZEOF(DRM_ID) )                      /* Key ID */

#define WMDRMNET_LINK_EXTENSION_AES_IV_64_SIZE ( \
            SIZEOF(DRM_UINT64) )                  /* Data Segment ID */ 

#define WMDRMNET_LINK_EXTENSION_AES_IV_128_SIZE  ( \
            SIZEOF(DRM_UINT64) +                  /* Data Segment ID */ \
            SIZEOF(DRM_UINT64) )                  /* Block ID */

#define WMDRMNET_LINK_EXTENSION_KEYID_TYPE  0x01
#define WMDRMNET_LINK_EXTENSION_AES_IV_TYPE 0x02

#define WMDRMNET_LINK_ENCRYPTED_DATA        0x01
#define WMDRMNET_LINK_UNENCRYPTED_DATA      0x00

static DRM_RESULT _PackDataSegmentDescriptor(
    __inout  NDT_CRYPTO_DATA_LINK *pLinkData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbBuffer = WMDRMNET_LINK_DESCRIPTOR_SIZE;
    DRM_BYTE   cExtensions = 0;
    DRM_WORD   cbAesExtension = 0;
    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor;
    
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__PackDataSegmentDescriptor );

    MessageDescriptor.Buffer = NULL;

    ChkArg( pLinkData != NULL );
    
    if( !pLinkData->fNoEncrypt )
    {
        if( pLinkData->fNoBlockID )
        {
            cbAesExtension = WMDRMNET_LINK_EXTENSION_AES_IV_64_SIZE;
        }
        else
        {
            cbAesExtension = WMDRMNET_LINK_EXTENSION_AES_IV_128_SIZE;
        }
        cbBuffer += WMDRMNET_LINK_EXTENSION_HEADER_SIZE + cbAesExtension;
        cExtensions++;
    }

    if( pLinkData->pDataSegmentDescriptor->cbBlob < cbBuffer )
    {
        pLinkData->pDataSegmentDescriptor->cbBlob = cbBuffer;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    MessageDescriptor.BufferSize = cbBuffer;
    MessageDescriptor.Buffer = pLinkData->pDataSegmentDescriptor->pbBlob;
    MessageDescriptor.Offset = 0;

    if( pLinkData->fNoEncrypt )
    {
        WmdrmNetPackOctet( &MessageDescriptor , WMDRMNET_LINK_UNENCRYPTED_DATA );
    }
    else
    {
        WmdrmNetPackOctet( &MessageDescriptor , WMDRMNET_LINK_ENCRYPTED_DATA );
    }

    WmdrmNetPackOctet( &MessageDescriptor, cExtensions );

    if( !pLinkData->fNoEncrypt )
    {
        WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_LINK_EXTENSION_AES_IV_TYPE );
        WmdrmNetPackWord( &MessageDescriptor, cbAesExtension );
        WmdrmNetPackUint64( &MessageDescriptor, pLinkData->qwDataSegmentID );
        if( cbAesExtension == WMDRMNET_LINK_EXTENSION_AES_IV_128_SIZE )
        {
            WmdrmNetPackUint64( &MessageDescriptor, pLinkData->qwBlockID );
        }
    }

    WmdrmNetPackDword( &MessageDescriptor, cbBuffer );

    DRMASSERT( MessageDescriptor.BufferSize == MessageDescriptor.Offset );

    MessageDescriptor.Buffer = NULL;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
** Function :   NDTMGR_Encrypt
**
** Synopsis :   Encrypts the sample data in place using AES in CTR mode.
**              
** Arguments :  [f_pNdtMgrContext]  : Pointer to WMDRM-NDT manager context
**              [f_dwSessionID]     : Session ID
**              [f_EncryptionMode]  : NDT_ENCRYPTION_MODE - ASF, MPEG, LINK. Determines AES CTR Block ID
**              [f_CryptoData]      : Pointer to sample data and parameters for specific encryption mode
**                                        Mode                      | Struct
**                                        --------------------------+----------------------
**                                        NDT_ENCRYPTION_MODE_ASF   | NDT_CRYPTO_DATA_ASF
**                                        NDT_ENCRYPTION_MODE_MPEG  | NDT_CRYPTO_DATA_MPEG
**                                        NDT_ENCRYPTION_MODE_LINK  | NDT_CRYPTO_DATA_LINK
** 
** Returns:     DRM_SUCCESS 
** 
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL NDTMGR_Encrypt(
    __in        NDTMGR_CONTEXT      *f_pNdtMgrContext,
    __in const  DRM_ID               f_idSession,
    __in const  NDT_ENCRYPTION_MODE  f_EncryptionMode,
    __inout     DRM_VOID            *f_CryptoData
)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContentSessionContext = NULL;
    DRM_AES_KEY  *pKey = NULL;
    DRM_BYTEBLOB *pBlob = NULL;
    NDT_CRYPTO_DATA_LINK *pLinkData = NULL;
    DRM_UINT64 qwZero = DRM_UI64( 0 );
    DRM_AES_COUNTER_MODE_CONTEXT oCtrCtx = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Encrypt );

    ChkArg( f_pNdtMgrContext != NULL && f_CryptoData != NULL &&
           (f_EncryptionMode == NDT_ENCRYPTION_MODE_LINK ||
            f_EncryptionMode == NDT_ENCRYPTION_MODE_ASF ||
            f_EncryptionMode == NDT_ENCRYPTION_MODE_MPEG) );
    if(f_EncryptionMode == NDT_ENCRYPTION_MODE_LINK)
    {
        pLinkData = (NDT_CRYPTO_DATA_LINK*)f_CryptoData;
        if(pLinkData->fNoBlockID)
        {
            /* If there should be no block ID, both offset and block id
               must be zero */
            ChkArg( DRM_UI64Eql(pLinkData->qwOffset, qwZero) &&
                    DRM_UI64Eql(pLinkData->qwBlockID, qwZero) );

        }
        else
        {
            /* Otherwise, at least one of qwOffsetID and qwBlockID is zero. */
            ChkArg( DRM_UI64Eql(pLinkData->qwOffset, qwZero) ||
                    DRM_UI64Eql(pLinkData->qwBlockID, qwZero) );
        }
    }

    /* Get AES key from session */
    ChkDR( NDTMGR_Internal_FindSession( f_pNdtMgrContext, f_idSession, NDTMGR_SESSION_TYPE_CONTENT, (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContentSessionContext ) );
    DRMASSERT( NULL != pContentSessionContext );

    if ( MEDIA_STATE_SENDING_DATA  != pContentSessionContext->mediaSessionState
      && MEDIA_STATE_LICENSE_READY != pContentSessionContext->mediaSessionState )
    {
        ChkDR( DRM_E_ND_INVALID_SESSION );
    }

    DRMASSERT( NULL != pContentSessionContext->pLicenseContext);
    switch ( pContentSessionContext->licenseMode )
    {
    case NDT_LICENSE_MODE_SIMPLE:
        pKey = ((NDT_LICENSE_CONTEXT *) pContentSessionContext->pLicenseContext)->pAESKeyContentEncryption;
        break;
    case NDT_LICENSE_MODE_CHAINED:
        pKey = ((NDT_CHAINED_LICENSE_CONTEXT *) pContentSessionContext->pLicenseContext)->leafLicenseContext.pAESKeyContentEncryption;
        break;
    default:
        DRMASSERT(0);
    }
    DRMASSERT( NULL != pKey );

 
    switch( f_EncryptionMode )
    {
    case NDT_ENCRYPTION_MODE_ASF:
        pBlob = &( (NDT_CRYPTO_DATA_ASF*)f_CryptoData )->SampleData;
        oCtrCtx.qwInitializationVector = ( (NDT_CRYPTO_DATA_ASF*)f_CryptoData )->qwSampleID;
        oCtrCtx.qwBlockOffset = DRM_UI64Div( ((NDT_CRYPTO_DATA_ASF*)f_CryptoData )->qwOffset, DRM_UI64( DRM_AES_BLOCKLEN ) );
        oCtrCtx.bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( ((NDT_CRYPTO_DATA_ASF*)f_CryptoData )->qwOffset, DRM_UI64(DRM_AES_BLOCKLEN) ) );
        
        ChkDR( Oem_Aes_CtrProcessData( pKey, 
                                     pBlob->pbBlob, 
                                     pBlob->cbBlob, 
                                    &oCtrCtx ) );

        break;
    case NDT_ENCRYPTION_MODE_MPEG:
        pBlob = &( (NDT_CRYPTO_DATA_MPEG*)f_CryptoData )->Data;
        oCtrCtx.qwInitializationVector = ( (NDT_CRYPTO_DATA_MPEG*)f_CryptoData )->qwBaseCounter;
        oCtrCtx.qwBlockOffset = ( (NDT_CRYPTO_DATA_MPEG*)f_CryptoData )->qwMinorCounter;
        oCtrCtx.bByteOffset = 0;
        
        ChkDR( Oem_Aes_CtrProcessData( pKey, 
                                     pBlob->pbBlob, 
                                     pBlob->cbBlob, 
                                    &oCtrCtx ) );

        break;
    case NDT_ENCRYPTION_MODE_LINK:
        if( pLinkData->pDataSegmentDescriptor != NULL )
        {
            ChkDR( _PackDataSegmentDescriptor( pLinkData ) );
        }

        oCtrCtx.qwInitializationVector = pLinkData->qwDataSegmentID;
        oCtrCtx.qwBlockOffset = pLinkData->qwBlockID;
        oCtrCtx.bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( pLinkData->qwOffset, DRM_UI64(DRM_AES_BLOCKLEN) ) );
        
        if( !pLinkData->fNoEncrypt )
        {
            ChkDR( Oem_Aes_CtrProcessData( pKey, 
                                         pLinkData->Data.pbBlob, 
                                         pLinkData->Data.cbBlob, 
                                        &oCtrCtx ) );
        }
        break;
    default:
        /* Error: Bad Encryption Mode */
        break;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



EXIT_PK_NAMESPACE_CODE;
