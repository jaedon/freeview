/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMOVEFORMAT_C
#include <drmmoveformat.h>
#include <drmmoveconstants.h>
#include <drmxmlsigconstants.h>
#include <drmxbparser.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

const static DRM_XB_ELEMENT_DESCRIPTION s_mpBasicInformationDescription[] =
{
    /* Type                  offsets                                                       size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD, DRM_OFFSET_OF( MOVE_PAGE_BASIC_INFORMATION, dwSerialNumber ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD, DRM_OFFSET_OF( MOVE_PAGE_BASIC_INFORMATION, dwPageNumber   ), XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_mpLidListDescription[] =
{
    /* Type                     offsets                                                    size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_GUIDLIST, 0,                                                         XB_SIZE_RELATIVE_WORD },
};

/* Define the array of entries (objects and containers) that are found in move pages */
const static DRM_XB_ENTRY_DESCRIPTION s_mpEntryDescriptions[MP_ENTRY_TYPE_MAX_DEFINED] =
{
    /* Entry Type                         Flags                     Parent Type                Dups?  Opt?   offsets                                           size,                                sort,    entry/element descriptor                    count */
    { XB_OBJECT_GLOBAL_HEADER,            XB_FLAGS_NONE,            XB_OBJECT_TYPE_INVALID,    FALSE, FALSE, 0,                                                SIZEOF(DRM_MOVE_PAGE),               0,       NULL,                                       0  },
    { MP_ENTRY_TYPE_BASIC_INFORMATION,    XB_FLAGS_MUST_UNDERSTAND, XB_OBJECT_GLOBAL_HEADER,   FALSE, FALSE, DRM_OFFSET_OF( DRM_MOVE_PAGE, BasicInformation ), SIZEOF(MOVE_PAGE_BASIC_INFORMATION), 1,       (DRM_VOID*)s_mpBasicInformationDescription, NO_OF( s_mpBasicInformationDescription ) },
    { MP_ENTRY_TYPE_LID_LIST,             XB_FLAGS_MUST_UNDERSTAND, XB_OBJECT_GLOBAL_HEADER,   FALSE, FALSE, DRM_OFFSET_OF( DRM_MOVE_PAGE, LidList ),          SIZEOF(MOVE_LID_LIST),               2,       (DRM_VOID*)s_mpLidListDescription,          NO_OF( s_mpLidListDescription ) },
};

const static DRM_XB_HEADER_DESCRIPTION s_mpHeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'G','P','V','M' ),    1, NULL, 0 };

const static DRM_XB_FORMAT_DESCRIPTION g_mpFormatDescription =
{
    /* header description        entry table               count of entries              */
    &s_mpHeaderDescription,      s_mpEntryDescriptions,    NO_OF( s_mpEntryDescriptions )
};

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Page_Start(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_XB_StartFormat( f_pbStack,
                               f_cbStack,
                               1,
                               f_pcontextBuilder,
                              &g_mpFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Page_Finish(
    __inout_ecount( 1 )            const DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder,
    __inout_bcount(*f_pcbMovePage)       DRM_BYTE                *f_pbMovePage,
    __inout                              DRM_DWORD               *f_pcbMovePage )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder,
                                f_pbMovePage,
                                f_pcbMovePage) );
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MP_ParsePage(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbPage,
    __in                              DRM_DWORD                f_cbPage,
    __out                             DRM_MOVE_PAGE           *f_pPage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack = {0};

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( DRM_XB_UnpackBinary( f_pbPage,
                                f_cbPage,
                                &stack,
                                &g_mpFormatDescription,
                                1,
                                NULL,
                                f_pPage ) );
ErrorExit:
    return dr;
}




const static DRM_XB_ELEMENT_DESCRIPTION s_miBasicInformationDescription[] =
{
    /* Type                      offsets                                                         size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( MOVE_INDEX_BASIC_INFORMATION, dwSerialNumber ),  XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( MOVE_INDEX_BASIC_INFORMATION, xbbaClientId   ),  XB_SIZE_RELATIVE_WORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_miMovePageDescriptor[] =
{
    /* Type                      offsets                                                       size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_MOVE_PAGE_DESCRIPTOR, dwSerialNumber ),    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_MOVE_PAGE_DESCRIPTOR, dwPageNumber ),      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_MOVE_PAGE_DESCRIPTOR, dwPageHashType ),    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_MOVE_PAGE_DESCRIPTOR, xbbaHash ),          XB_SIZE_RELATIVE_WORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_miMovePageSignature[] =
{
    /* Type                      offsets                                                         size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_MOVE_INDEX_SIGNATURE, wSignatureType ),      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_MOVE_INDEX_SIGNATURE, xbbaSignature ),       XB_SIZE_RELATIVE_WORD },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_MOVE_INDEX_SIGNATURE, xbbaValidaingPubKey ), XB_SIZE_RELATIVE_WORD },
};


/* Define the array of entries (objects and containers) that are found in move pages */
const static DRM_XB_ENTRY_DESCRIPTION s_miEntryDescriptions[MI_ENTRY_TYPE_MAX_DEFINED] =
{
    /* Entry Type                         Flags                                          Parent Type                Dups?  Opt?   offsets                                                   size,                                   sort order, entry/element descriptor                    count */
    { XB_OBJECT_GLOBAL_HEADER,            XB_FLAGS_NONE,                                 XB_OBJECT_TYPE_INVALID,    FALSE, FALSE, 0,                                                        SIZEOF(DRM_MOVE_INDEX),                 0,          NULL,                                       0  },
    { MI_ENTRY_TYPE_BASIC_INFORMATION,    XB_FLAGS_MUST_UNDERSTAND,                      XB_OBJECT_GLOBAL_HEADER,   FALSE, FALSE, DRM_OFFSET_OF( DRM_MOVE_INDEX,     BasicInformation ),    SIZEOF(MOVE_INDEX_BASIC_INFORMATION),   1,          (DRM_VOID*)s_miBasicInformationDescription, NO_OF( s_miBasicInformationDescription ) },
    { MI_ENTRY_TYPE_PAGE_DESCRIPTOR,      XB_FLAGS_MUST_UNDERSTAND,                      MI_ENTRY_TYPE_PAGE_LIST,   TRUE,  TRUE,  DRM_OFFSET_OF( DRM_MOVE_PAGE_LIST, pHead ),               SIZEOF(DRM_MOVE_PAGE_DESCRIPTOR),       0,          (DRM_VOID*)s_miMovePageDescriptor,          NO_OF( s_miMovePageDescriptor ) },
    { MI_ENTRY_TYPE_PAGE_LIST,            XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER, XB_OBJECT_GLOBAL_HEADER,   FALSE, TRUE,  DRM_OFFSET_OF( DRM_MOVE_INDEX,     PageList ),            SIZEOF(DRM_MOVE_PAGE_LIST),             2,          NULL,                                       0 },
    { MI_ENTRY_TYPE_SIGNATURE,            XB_FLAGS_MUST_UNDERSTAND,                      XB_OBJECT_GLOBAL_HEADER,   FALSE, FALSE, DRM_OFFSET_OF( DRM_MOVE_INDEX,     Signature ),           SIZEOF(DRM_MOVE_INDEX_SIGNATURE),       3,          (DRM_VOID*)s_miMovePageSignature,           NO_OF( s_miMovePageSignature ) },
};

const static DRM_XB_HEADER_DESCRIPTION s_miHeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'X','D','N','I' ),    1, NULL, 0 };

const static DRM_XB_FORMAT_DESCRIPTION g_miFormatDescription =
{
    /* header description        entry table               count of entries              */
    &s_miHeaderDescription,      s_miEntryDescriptions,    NO_OF( s_miEntryDescriptions )
};

DRM_BYTE rgbSigTemp[ECDSA_P256_SIGNATURE_SIZE_IN_BYTES] = {0};
DRM_BYTE rgbPublicKeyTemp[ECC_P256_POINT_SIZE_IN_BYTES] = {0};

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Index_Start(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVE_INDEX_SIGNATURE *pmiSig = (DRM_MOVE_INDEX_SIGNATURE *)f_pbStack;

    if( f_cbStack < SIZEOF( DRM_MOVE_INDEX_SIGNATURE ) )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Use the first DRM_MOVE_INDEX_SIGNATURE bytes to persist the struct in memory during building */
    ChkDR( DRM_XB_StartFormat( f_pbStack + __CB_DECL(SIZEOF(DRM_MOVE_INDEX_SIGNATURE)),
                               f_cbStack - SIZEOF(DRM_MOVE_INDEX_SIGNATURE),
                               1,
                               f_pcontextBuilder,
                              &g_miFormatDescription ) );


    /*
    ** Add a signature node with a bogus sig & pubkey
    ** These will be filled in when DRM_MOV_Index_Finish is called.
    */
    pmiSig->wSignatureType = 1;

    pmiSig->xbbaSignature.cbData       = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
    pmiSig->xbbaSignature.pbDataBuffer = rgbSigTemp;
    pmiSig->xbbaSignature.iData        = 0;

    pmiSig->xbbaValidaingPubKey.cbData       = ECC_P256_POINT_SIZE_IN_BYTES;
    pmiSig->xbbaValidaingPubKey.pbDataBuffer = rgbPublicKeyTemp;
    pmiSig->xbbaValidaingPubKey.iData        = 0;

    ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, MI_ENTRY_TYPE_SIGNATURE, pmiSig ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Index_Finish(
    __inout_ecount( 1 )             const DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder,
    __inout_bcount(*f_pcbMoveIndex)       DRM_BYTE                *f_pbMoveIndex,
    __inout                               DRM_DWORD               *f_pcbMoveIndex,
    __in                                  DRM_MOV_CB_SIGNATURE     f_pfnSignatureCallback,
    __in                            const DRM_VOID                *f_pvSignatureContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  iObject = 0;
    DRM_DWORD  cbObject = 0;

    /* Handle the fact that the signature starts 6 bytes in from the object */
    const DRM_DWORD iSignatureFromObjectStart = XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_WORD) + SIZEOF(DRM_WORD);
    /* The public key starts after signature and 2-byte key length */
    const DRM_DWORD iPublicKeyFromObjectStart = iSignatureFromObjectStart + ECDSA_P256_SIGNATURE_SIZE_IN_BYTES + SIZEOF(DRM_WORD);

    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder,
                                f_pbMoveIndex,
                                f_pcbMoveIndex ) );

    ChkDR( DRM_XB_FindObject( MI_ENTRY_TYPE_SIGNATURE, &g_miFormatDescription, f_pbMoveIndex, *f_pcbMoveIndex, &iObject, &cbObject ) );

    ChkDR( f_pfnSignatureCallback( f_pvSignatureContext,
                                   f_pbMoveIndex,
                                  *f_pcbMoveIndex - cbObject,
                                   f_pbMoveIndex,
                                   ECDSA_P256_SIGNATURE_SIZE_IN_BYTES,
                                   iObject + iSignatureFromObjectStart,
                                   f_pbMoveIndex,
                                   ECC_P256_POINT_SIZE_IN_BYTES,
                                   iObject + iPublicKeyFromObjectStart ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MI_ParseIndex(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbIndex,
    __in                              DRM_DWORD                f_cbIndex,
    __out                             DRM_MOVE_INDEX          *f_pIndex)
{
    DRM_RESULT                  dr    = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack = {0};

    ChkArg( f_pbStack != NULL );
    ChkArg( f_pbIndex != NULL );
    ChkArg( f_pIndex != NULL );

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( DRM_XB_UnpackBinary( f_pbIndex,
                                f_cbIndex,
                                &stack,
                                &g_miFormatDescription,
                                 1,
                                 NULL,
                                 f_pIndex ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MOV_ParseIndex(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbIndex,
    __in                              DRM_DWORD                f_cbIndex,
    __out                             DRM_MOVE_INDEX          *f_pIndex,
    __inout                           DRM_BYTE                 f_rgbCryptoContext[__CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE)])
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iObject  = 0;
    DRM_DWORD  cbObject = 0;

    ChkArg( f_pbStack != NULL );
    ChkArg( f_pbIndex != NULL );
    ChkArg( f_pIndex != NULL );

    ChkDR( DRM_MI_ParseIndex( f_pbStack, f_cbStack, f_pbIndex, f_cbIndex, f_pIndex ) );

    /*
    **  Validate the signature of the binary
    */
    if( f_pIndex->Signature.wSignatureType             != 1
     || f_pIndex->Signature.xbbaSignature.cbData       != ECDSA_P256_SIGNATURE_SIZE_IN_BYTES
     || f_pIndex->Signature.xbbaValidaingPubKey.cbData != ECC_P256_POINT_SIZE_IN_BYTES )
    {
        ChkDR( DRM_E_MOVE_FORMAT_INVALID );
    }

    if( 0 != MEMCMP( &g_pubkeyMoveService, f_pIndex->Signature.xbbaValidaingPubKey.pbDataBuffer + f_pIndex->Signature.xbbaValidaingPubKey.iData, f_pIndex->Signature.xbbaSignature.cbData ) )
    {
        ChkDR( DRM_E_MOVE_FORMAT_INVALID );
    }

    ChkDR( DRM_XB_FindObject( MI_ENTRY_TYPE_SIGNATURE, &g_miFormatDescription, f_pbIndex, f_cbIndex, &iObject, &cbObject ) );

    if ( DRM_FAILED( OEM_ECDSA_Verify_P256( f_pbIndex,
                                            f_cbIndex - cbObject,
                                           &g_pubkeyMoveService,
                           (SIGNATURE_P256*)(f_pIndex->Signature.xbbaSignature.pbDataBuffer + f_pIndex->Signature.xbbaSignature.iData),
                        (struct bigctx_t *) f_rgbCryptoContext ) ) )
    {
        ChkDR( DRM_E_MOVE_SIGNATURE_INVALID );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
