/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include "drmhds.h"
#include "drmcontextsizes.h"
#include "ndtnetmsg.h"
#include "oemaes.h"
#include "drmcrt.h"
#include "drmdecrypt.h"

ENTER_PKTEST_NAMESPACE_CODE

#if TARGET_LITTLE_ENDIAN

#define DRM_HTON_UINT64( UI64, pb ) \
    {   int _i = 0; \
        for( ; _i < sizeof(DRM_UINT64); _i++ ) \
            (pb)[ _i ] = ( (DRM_BYTE*)( &(UI64) ) )[ sizeof(DRM_UINT64) - _i - 1 ]; \
    }
    
#else

#define DRM_HTON_UINT64( UI64, pb ) \
    *( (DRM_UINT64*)(pb) ) = UI64;

#endif

#define WMDRMNET_LINK_DESCRIPTOR_SIZE    ( \
            sizeof(DRM_BYTE) +                  /* Flags */ \
            sizeof(DRM_BYTE) +                  /* Number Of Extensions */ \
            sizeof(DRM_DWORD) )                 /* Length */

#define WMDRMNET_LINK_EXTENSION_HEADER_SIZE  ( \
            sizeof(DRM_BYTE) +                  /* Extension Type */ \
            sizeof(DRM_WORD) )                  /* Extension Length */ 

#define WMDRMNET_LINK_EXTENSION_KEYID_SIZE  ( \
            WMDRMNET_LINK_EXTENSION_HEADER_SIZE + /* Extension Header */ \
            sizeof(DRM_ID) )                      /* Key ID */

#define WMDRMNET_LINK_EXTENSION_AES_IV_64_SIZE ( \
            WMDRMNET_LINK_EXTENSION_HEADER_SIZE + /* Extension Header */ \
            sizeof(DRM_UINT64) )                  /* Data Segment ID */ 

#define WMDRMNET_LINK_EXTENSION_AES_IV_128_SIZE  ( \
            WMDRMNET_LINK_EXTENSION_HEADER_SIZE + /* Extension Header */ \
            sizeof(DRM_UINT64) +                  /* Data Segment ID */ \
            sizeof(DRM_UINT64) )                  /* Block ID */

#define WMDRMNET_LINK_EXTENSION_KEYID_TYPE  0x01
#define WMDRMNET_LINK_EXTENSION_AES_IV_TYPE 0x02

#define WMDRMNET_LINK_ENCRYPTED_DATA        0x01
#define WMDRMNET_LINK_UNENCRYPTED_DATA      0x00

typedef struct tagWMDRMNET_CRYTPO_DATA_LINK_DESC
{
    DRM_BOOL    fNoEncrypt;
    DRM_BOOL    fNoKeyID;
    DRM_UINT64  qwSegmentID;
    DRM_UINT64  qwBlockID;
    DRM_ID      idKey;
    DRM_DWORD   cbDataSegmentDescriptor;
} WMDRMNET_CRYPTO_DATA_LINK_DESC;

DRM_BOOL
WmdrmNetUnpackUI64(
    __in WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor,
    __out_bcount(SIZEOF(DRM_UINT64))  DRM_UINT64 *Value
    )
{
    int i;

#if TARGET_LITTLE_ENDIAN
    for( i = SIZEOF(DRM_UINT64) - 1; i>=0; i--) {
#else
    for ( i=0; i<SIZEOF(DRM_UINT64) ; i++) {
#endif
        DRM_BYTE b;
        if ( !WmdrmNetUnpackOctet( MessageDescriptor, &b ) ){
            return FALSE;
        }
		PUT_BYTE((DRM_BYTE *)Value, i, b);
    }

    return TRUE;
}

DRM_RESULT _UnpackDataSegmentDescriptor(
    IN      DRM_BYTEBLOB                    *pBlob,
    OUT     WMDRMNET_CRYPTO_DATA_LINK_DESC  *pDescriptor
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   cExtensions = 0;
    DRM_BYTE   bExtensionType;
    DRM_WORD   cbExtension;
    DRM_BYTE   bFlags;

    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor;
    
    ChkArg( pBlob != NULL 
         && pBlob->pbBlob != NULL 
         && pDescriptor != NULL );

    MessageDescriptor.Buffer = pBlob->pbBlob;
    MessageDescriptor.BufferSize = pBlob->cbBlob;
    MessageDescriptor.Offset = 0;

    pDescriptor->fNoKeyID = TRUE;
    
    /*
     * Unpack the Flags
     */
    if ( !WmdrmNetUnpackOctet( &MessageDescriptor, &bFlags) ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Data Segment Descriptor message too short (Flags)\n" ));
        goto ErrorExit;
    }

    switch(bFlags)
    {
    case WMDRMNET_LINK_UNENCRYPTED_DATA :
        pDescriptor->fNoEncrypt = TRUE;
        break;
    case WMDRMNET_LINK_ENCRYPTED_DATA :
        pDescriptor->fNoEncrypt = FALSE;
        break;
    default:
        TRACE(( "Data Segment Descriptor message invalid (Flags)\n" ));
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }

    /*
     * Unpack the number of extensions
     */
    if ( !WmdrmNetUnpackOctet( &MessageDescriptor, &cExtensions) ) {
        TRACE(( "Data Segment Descriptor message too short (Sequence)\n" ));
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }
    
    while(cExtensions > 0)
    {
        /*
        * Unpack the extension type
        */
        if ( !WmdrmNetUnpackOctet( &MessageDescriptor, &bExtensionType) ) {
            TRACE(( "Data Segment Descriptor message too short (Extension Type)\n" ));
            ChkDR( DRM_E_ND_INVALID_MESSAGE );
        }

        /*
        * Unpack the extension length
        */
        if ( !WmdrmNetUnpackWord( &MessageDescriptor, &cbExtension) ) {
            TRACE(( "Data Segment Descriptor message too short (Extension Length)\n" ));
            ChkDR( DRM_E_ND_INVALID_MESSAGE );
        }

        switch(bExtensionType)
        {
        case WMDRMNET_LINK_EXTENSION_KEYID_TYPE:
            if( cbExtension != SIZEOF(DRM_ID) )
            {
                TRACE((" Data Segment Descriptor extension length invalid (KeyID)\n" ));
                ChkDR( DRM_E_ND_INVALID_MESSAGE );
            }

            /*
            * Unpack the key extension data
            */
            if( !WmdrmNetUnpackVal128( &MessageDescriptor, &pDescriptor->idKey ) )
            {
                TRACE(( "Data Segment Descriptor message too short (Key Extension Length)\n" ));
                ChkDR( DRM_E_ND_INVALID_MESSAGE );
            }

            pDescriptor->fNoKeyID = FALSE;

            break;
        case WMDRMNET_LINK_EXTENSION_AES_IV_TYPE:
            if( cbExtension != SIZEOF(DRM_UINT64) && cbExtension != 2*SIZEOF(DRM_UINT64) )
            {
                TRACE((" Data Segment Descriptor extension length invalid (AES IV)\n" ));
                ChkDR( DRM_E_ND_INVALID_MESSAGE );
            }

            /*
            * Unpack the AES IV extension High 64 bits
            */
            if( !WmdrmNetUnpackUI64( &MessageDescriptor, &pDescriptor->qwSegmentID) )
            {
                TRACE(( "Data Segment Descriptor message too short (AES IV Extension Length)\n" ));
                ChkDR( DRM_E_ND_INVALID_MESSAGE );
            }

            if( cbExtension == 2*SIZEOF(DRM_UINT64) )
            {
                /*
                * Unpack the AES IV extension low 64 bits
                */
                if( !WmdrmNetUnpackUI64( &MessageDescriptor, &pDescriptor->qwBlockID) )
                {
                    TRACE(( "Data Segment Descriptor message too short (AES IV Extension Length)\n" ));
                    ChkDR( DRM_E_ND_INVALID_MESSAGE );
                }
            }
            else
            {
                pDescriptor->qwBlockID = DRM_UI64(0);
            }
            break;
        default:
            /*
             * Unknown extension type - disregard
             */
            MessageDescriptor.Offset += cbExtension;
            break;
        }

        cExtensions--;
    }

    /*
    * Unpack the descriptor length
    */
    if( !WmdrmNetUnpackDword( &MessageDescriptor, &pDescriptor->cbDataSegmentDescriptor ) )
    {
        TRACE(( "Data Segment Descriptor message too short (Message Length)\n" ));
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }

ErrorExit:

    return dr;
}

DRM_RESULT DRM_API WmdrmNetLinkDecrypt(
    IN      DRM_AES_KEY              *f_pContentEncryptionKey,
    IN      WMDRMNET_ENCRYPTION_MODE  f_EncryptionMode,
    IN      DRM_VOID                 *f_pvCryptoDataDescriptor,
    IN OUT  DRM_BYTE                 *f_pbCryptoData,
    IN OUT  DRM_DWORD                *f_pcbCryptoData
    )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    WMDRMNET_CRYPTO_DATA_LINK_DESC linkDescriptor;
    DRM_BYTEBLOB *pBlob;
	DRM_AES_COUNTER_MODE_CONTEXT aesCtrContext = {0};

    ChkArg( f_pContentEncryptionKey != NULL 
         && f_pvCryptoDataDescriptor != NULL );
//         && f_pbCryptoData != NULL );

    switch( f_EncryptionMode )
    {
    case WMDRMNET_ENCRYPTION_MODE_ASF:
		aesCtrContext.qwInitializationVector = ( (WMDRMNET_CRYPTO_DATA_ASF*)f_pvCryptoDataDescriptor )->qwSampleID;
		aesCtrContext.qwBlockOffset = DRM_UI64Div( ((WMDRMNET_CRYPTO_DATA_ASF*)f_pvCryptoDataDescriptor )->qwOffset, DRM_UI64( DRM_AES_BLOCKLEN ) );
		aesCtrContext.bByteOffset = DRM_UI64Low32( DRM_UI64Mod( ((WMDRMNET_CRYPTO_DATA_ASF*)f_pvCryptoDataDescriptor )->qwOffset, DRM_UI64( DRM_AES_BLOCKLEN ) ) );

        ChkDR( Oem_Aes_CtrProcessData( f_pContentEncryptionKey, 
                                     f_pbCryptoData, 
                                     *f_pcbCryptoData, 
                                    &aesCtrContext));
        break;
    case WMDRMNET_ENCRYPTION_MODE_MPEG:
		aesCtrContext.qwInitializationVector = ( (WMDRMNET_CRYPTO_DATA_MPEG*)f_pvCryptoDataDescriptor )->qwBaseCounter;
		aesCtrContext.qwBlockOffset = ( (WMDRMNET_CRYPTO_DATA_MPEG*)f_pvCryptoDataDescriptor )->qwMinorCounter;
		aesCtrContext.bByteOffset = 0;

        ChkDR( Oem_Aes_CtrProcessData( f_pContentEncryptionKey, 
                                   f_pbCryptoData, 
                                   *f_pcbCryptoData, 
                                  &aesCtrContext) );
        break;
    case WMDRMNET_ENCRYPTION_MODE_LINK:
        pBlob = (DRM_BYTEBLOB*)f_pvCryptoDataDescriptor;
        ChkDR( _UnpackDataSegmentDescriptor( pBlob, &linkDescriptor ) );

        if(pBlob->cbBlob - linkDescriptor.cbDataSegmentDescriptor > *f_pcbCryptoData)
        {
            *f_pcbCryptoData = pBlob->cbBlob - linkDescriptor.cbDataSegmentDescriptor;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        if( !linkDescriptor.fNoEncrypt )
        {
			aesCtrContext.qwInitializationVector = linkDescriptor.qwSegmentID;
			aesCtrContext.qwBlockOffset = linkDescriptor.qwBlockID;
			aesCtrContext.bByteOffset = 0;

            ChkDR( Oem_Aes_CtrProcessData( f_pContentEncryptionKey, 
                                         f_pbCryptoData,
                                        *f_pcbCryptoData,
                                       &aesCtrContext) );
        }
        break;
    default:
        /* Error: Bad Encryption Mode */
        break;
    }

ErrorExit:

    return dr;

}

EXIT_PKTEST_NAMESPACE_CODE
