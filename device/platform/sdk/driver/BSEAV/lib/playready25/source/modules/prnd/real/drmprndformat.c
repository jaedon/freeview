/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDFORMAT_C
#include <drmprnd.h>
#include <drmxmlsigconstants.h>
#include <drmxbparser.h>
#include <oemaescommon.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_PRND_FOURCC   XB_DEFINE_DWORD_FORMAT_ID( 'P','R','N','D' )

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDSupported(DRM_VOID) { return DRM_PRND_IsPRNDTransmitterSupported() || DRM_PRND_IsPRNDReceiverSupported(); }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDUnsupported(DRM_VOID) { return !DRM_PRND_IsPRNDSupported(); }

static DRMFORCEINLINE DRM_RESULT _RemapPRNDFormatError( DRM_RESULT dr ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _RemapPRNDFormatError( DRM_RESULT dr )
{
    switch( dr )
    {
    case DRM_E_XB_OBJECT_NOTFOUND:
    case DRM_E_XB_INVALID_OBJECT:
    case DRM_E_XB_OBJECT_ALREADY_EXISTS:
    case DRM_E_XB_REQUIRED_OBJECT_MISSING:
    case DRM_E_XB_UNKNOWN_ELEMENT_TYPE:
        return DRM_E_PRND_MESSAGE_INVALID;
    default:
        return dr;
    }
}

static DRMFORCEINLINE DRM_RESULT _ValidateMessageType(
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _ValidateMessageType(
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat )
{
    DRM_RESULT                      dr                  = DRM_SUCCESS;
    DRM_UINT64                      qwMagicConstant     = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64                      qwPrndFourCC        = DRM_PRND_FOURCC;

    /*
    ** Check for DRM_E_PRND_MESSAGE_WRONG_TYPE.
    ** This error should be returned iff:
    ** 1. Message is long enough to be a valid message.
    ** 2. Message starts with DRM_PRND_FOURCC.
    ** 3. Message does not have the correct 8-byte magic constant.
    */
    ChkBOOL( f_cbMessage > XB_HEADER_LENGTH( f_pformat->pHeaderDescription->eFormatIdLength ), DRM_E_PRND_MESSAGE_INVALID );
    NETWORKBYTES_TO_QWORD( qwMagicConstant, f_pbMessage, 0 );
    ChkBOOL( DRM_UI64High32( qwMagicConstant ) == DRM_UI64Low32( qwPrndFourCC ), DRM_E_PRND_MESSAGE_INVALID );
    ChkBOOL( DRM_UI64Eql( f_pformat->pHeaderDescription->qwFormatIdentifier, qwMagicConstant ), DRM_E_PRND_MESSAGE_WRONG_TYPE );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** PRND Message Signatures
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndMessageSignatureDescription[] =
{
    /* Type                      offsets                                                      size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_MESSAGE_SIGNATURE, wSignatureType ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_MESSAGE_SIGNATURE, xbbaSignature ),  XB_SIZE_RELATIVE_WORD },
};

static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat )
{
    DRM_RESULT                   dr       = DRM_SUCCESS;
    DRM_PRND_MESSAGE_SIGNATURE  *pSig     = NULL;
    DRM_BYTE                    *pbSig    = NULL;
    DRM_DWORD                    cbSig    = 0;

    ChkArg( f_pbStack != NULL );

    switch( f_eSignatureType )
    {
    case DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY:
        cbSig = DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY;
        break;
    case DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        break;
    default:
        AssertChkArg( FALSE );
        break;
    }

    /* Use the first bytes to persist the struct in memory during building */
    ChkBOOL( f_cbStack >= SIZEOF( DRM_PRND_MESSAGE_SIGNATURE ), DRM_E_OUTOFMEMORY );
    pSig = (DRM_PRND_MESSAGE_SIGNATURE*) f_pbStack;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pbStack, SIZEOF( DRM_PRND_MESSAGE_SIGNATURE ), (DRM_DWORD_PTR*)&f_pbStack ) );
    ChkDR( DRM_DWordSub( f_cbStack, SIZEOF( DRM_PRND_MESSAGE_SIGNATURE ), &f_cbStack ) );

    /* Use the next bytes to persist the signature buffer in memory during building */
    ChkBOOL( f_cbStack >= cbSig, DRM_E_OUTOFMEMORY );
    pbSig = f_pbStack;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pbStack, cbSig, (DRM_DWORD_PTR*)&f_pbStack ) );
    ChkDR( DRM_DWordSub( f_cbStack, cbSig, &f_cbStack ) );

    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, f_pformat ) );

    ChkDR( DRM_DWordToWord( f_eSignatureType, &pSig->wSignatureType ) );
    pSig->xbbaSignature.cbData       = cbSig;
    pSig->xbbaSignature.pbDataBuffer = pbSig;
    pSig->xbbaSignature.iData        = 0;

    ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, f_wSignatureObjectType, pSig ) );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT                      dr                        = DRM_SUCCESS;
    DRM_DWORD                       iSignatureObject          = 0;
    DRM_DWORD                       cbSignatureObject         = 0;
    DRM_DWORD                       cbSig                     = 0;
    DRM_DWORD                       cbMessageWithoutSignature = 0;
    DRM_DWORD                       iSignature                = 0;
    DRM_PRND_SIGNATURE_OPERATION    eOp                       = DRM_PRND_SIGNATURE_OPERATION_INVALID;

    /* Handle the fact that the signature buffer is offset some bytes in from the object */
                                                /* Base offset          wSignatureType     xbbaSignature.cbData (with XB_SIZE_RELATIVE_WORD) */
    const DRM_DWORD iSignatureFromObjectStart = XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_WORD) + SIZEOF(DRM_WORD);

    switch( f_eSignatureType )
    {
    case DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY:
        cbSig = DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY;
        eOp   = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_ECC256_RECEIVER_SIGNING_KEY;
        break;
    case DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        eOp   = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI;
        break;
    default:
        AssertChkArg( FALSE );
        break;
    }

    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );

    ChkDR( DRM_XB_FindObject( f_wSignatureObjectType, f_pformat, f_pbMessage, *f_pcbMessage, &iSignatureObject, &cbSignatureObject ) );

    ChkDR( DRM_DWordSub( *f_pcbMessage, cbSignatureObject, &cbMessageWithoutSignature ) );
    ChkDR( DRM_DWordAdd( iSignatureObject, iSignatureFromObjectStart, &iSignature ) );
    ChkDR( f_pfnSignatureCallback(
        f_pvSignatureContext,
        eOp,
        f_pbMessage,
        cbMessageWithoutSignature,
        f_pbMessage,
        cbSig,
        iSignature,
        NULL,
        NULL ) );

    DRMASSERT( iSignatureObject + cbSignatureObject == *f_pcbMessage );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat,
    __in                            const DRM_WORD                                      *f_pwParsedSignatureType,
    __in                            const DRM_DWORD                                     *f_pcbParsedSignature,
    __out                                 DRM_VOID                                      *f_pStruct,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext,
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaReceiverCertificate ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _MessageWithSignature_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __in                                  DRM_PRND_SIGNATURE_TYPE                        f_eSignatureType,
    __in                                  DRM_WORD                                       f_wSignatureObjectType,
    __in                            const DRM_XB_FORMAT_DESCRIPTION                     *f_pformat,
    __in                            const DRM_WORD                                      *f_pwParsedSignatureType,
    __in                            const DRM_DWORD                                     *f_pcbParsedSignature,
    __out                                 DRM_VOID                                      *f_pStruct,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext,
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaReceiverCertificate )
{
    DRM_RESULT                      dr                        = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT     stack                     = {0};
    DRM_DWORD                       dwVersionFound            = 0;
    DRM_DWORD                       iSignatureObject          = 0;
    DRM_DWORD                       cbSignatureObject         = 0;
    DRM_DWORD                       cbSig                     = 0;
    DRM_DWORD                       cbMessageWithoutSignature = 0;
    DRM_DWORD                       iSignature                = 0;
    DRM_PRND_SIGNATURE_OPERATION    eOp                       = DRM_PRND_SIGNATURE_OPERATION_INVALID;
    DRM_RESULT                      drResult                  = DRM_SUCCESS;

    /* Handle the fact that the signature buffer is offset some bytes in from the object */
                                                /* Base offset          wSignatureType     xbbaSignature.cbData (with XB_SIZE_RELATIVE_WORD) */
    const DRM_DWORD iSignatureFromObjectStart = XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_WORD) + SIZEOF(DRM_WORD);

    switch( f_eSignatureType )
    {
    case DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY:
        cbSig = DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY;
        eOp   = DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY;
        break;
    case DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        eOp   = DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI;
        break;
    default:
        AssertChkArg( FALSE );
        break;
    }

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );

    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, f_pformat ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, f_pformat, 1, &dwVersionFound, f_pStruct );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );

    ChkBOOL( *f_pwParsedSignatureType   == f_eSignatureType, DRM_E_PRND_MESSAGE_INVALID );
    ChkBOOL( *f_pcbParsedSignature      == cbSig,            DRM_E_PRND_MESSAGE_INVALID );
    ChkDR( DRM_XB_FindObject( f_wSignatureObjectType, f_pformat, f_pbMessage, f_cbMessage, &iSignatureObject, &cbSignatureObject ) );

    ChkDR( DRM_DWordSub( f_cbMessage, cbSignatureObject, &cbMessageWithoutSignature ) );
    ChkDR( DRM_DWordAdd( iSignatureObject, iSignatureFromObjectStart, &iSignature ) );
    ChkDR( f_pfnSignatureCallback(
        f_pvSignatureContext,
        eOp,
        (DRM_BYTE*)f_pbMessage,
        cbMessageWithoutSignature,
        (DRM_BYTE*)f_pbMessage,
        cbSig,
        iSignature,
        f_pxbbaMIMKEncryptedWithReceiverPubkey,
        f_pxbbaReceiverCertificate ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** PRND CustomData
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndCustomDataDescription[] =
{
    /* Type                      offsets                                                                                                    size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_CUSTOM_DATA, customDataTypeID ),                                                   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_CUSTOM_DATA, xbbaCustomData ),                                                     XB_SIZE_RELATIVE_DWORD },
};

/**************************************************************************************************************************************************************
**
** PRND Registration Request Message [RX->TX]
**
**************************************************************************************************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndRegistrationRequestBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                                    size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, dwFlags ),                                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, guidPreviousSessionID ),                   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, xbbaReceiverCertificate ),                 XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, dwReceiverRIV ),                           XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, dwSupportedProximityDetectionTypes ),      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, guidInitialLicenseMessageID ),             XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_QWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION, qwReceiverCurrentTime ),                   XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndRegistrationRequestEntryDescriptions[DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                   Flags                       Parent Type                 Dups?   Opt?    offsets                                                                     size,                                                       sort,   entry/element descriptor                                            count */
    { XB_OBJECT_GLOBAL_HEADER,                                      XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                          SIZEOF(DRM_PRND_REGISTRATION_REQUEST_MESSAGE),              0,      NULL,                                                               0  },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_BASIC_INFORMATION,   XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_MESSAGE, BasicInformation ),   SIZEOF(DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION),    1,      (DRM_VOID*)s_prndRegistrationRequestBasicInformationDescription,    NO_OF( s_prndRegistrationRequestBasicInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_CUSTOM_DATA,         XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  TRUE,   DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_MESSAGE, CustomData ),         SIZEOF(DRM_PRND_CUSTOM_DATA),                               2,      (DRM_VOID*)s_prndCustomDataDescription,                             NO_OF( s_prndCustomDataDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE,           XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_REQUEST_MESSAGE, Signature ),          SIZEOF(DRM_PRND_MESSAGE_SIGNATURE),                         3,      (DRM_VOID*)s_prndMessageSignatureDescription,                       NO_OF( s_prndMessageSignatureDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndRegistrationRequestHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_REGISTRATION_REQUEST_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndRegistrationRequestFormatDescription =
{
    /* header description                           entry table                                 count of entries              */
    &s_prndRegistrationRequestHeaderDescription,    s_prndRegistrationRequestEntryDescriptions, NO_OF( s_prndRegistrationRequestEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Start(
        f_pbStack,
        f_cbStack,
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE,
        &g_prndRegistrationRequestFormatDescription ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Finish(
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE,
        &g_prndRegistrationRequestFormatDescription,
        f_pbMessage,
        f_pcbMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_REQUEST_MESSAGE         *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Parse(
        f_pbStack,
        f_cbStack,
        f_pbMessage,
        f_cbMessage,
        DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE,
        &g_prndRegistrationRequestFormatDescription,
        &f_pMessage->Signature.wSignatureType,
        &f_pMessage->Signature.xbbaSignature.cbData,
        f_pMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext,
        NULL,
        &f_pMessage->BasicInformation.xbbaReceiverCertificate ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/**************************************************************************************************************************************************************
**
** PRND Registration Response Message [TX->RX]
**
**************************************************************************************************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndRegistrationResponseBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                                        size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION, dwFlags ),                                    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION, guidSessionID ),                              XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION, xbbaTransmitterRevInfo ),                     XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION, wMIMKEncryptionType ),                        XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION, xbbaMIMKEncryptedWithReceiverPubkey ),        XB_SIZE_RELATIVE_WORD },
};

static const DRM_XB_ELEMENT_DESCRIPTION s_prndRegistrationResponseProximityInformationDescription[] =
{
    /* Type                      offsets                                                                                                            size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION, dwProximityDetectionType ),                   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION, xbbaTransmitterProximityDetectionChannel ),   XB_SIZE_RELATIVE_WORD },
};

static const DRM_XB_ELEMENT_DESCRIPTION s_prndRegistrationResponseTransmitterAuthDescription[] =
{
    /* Type                      offsets                                                                                            size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, xbbaTransmitterCertificate ),      XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, wTransmitterAuthSignatureType ),   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, xbbaTransmitterAuthSignature ),    XB_SIZE_RELATIVE_WORD },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndRegistrationResponseEntryDescriptions[DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                       Flags                       Parent Type                 Dups?   Opt?    offsets                                                                         size,                                                           sort,   entry/element descriptor                                                count */
    { XB_OBJECT_GLOBAL_HEADER,                                          XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                              SIZEOF(DRM_PRND_REGISTRATION_RESPONSE_MESSAGE),                 0,      NULL,                                                                   0  },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_BASIC_INFORMATION,      XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE, BasicInformation ),      SIZEOF(DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION),       1,      (DRM_VOID*)s_prndRegistrationResponseBasicInformationDescription,       NO_OF( s_prndRegistrationResponseBasicInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION,  XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE, ProximityInformation ),  SIZEOF(DRM_PRND_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION),   2,      (DRM_VOID*)s_prndRegistrationResponseProximityInformationDescription,   NO_OF( s_prndRegistrationResponseProximityInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_TRANSMITTER_AUTH,       XB_FLAGS_NONE,              XB_OBJECT_GLOBAL_HEADER,    FALSE,  TRUE,   DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE, TransmitterAuth ),       SIZEOF(DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH),        3,      (DRM_VOID*)s_prndRegistrationResponseTransmitterAuthDescription,        NO_OF( s_prndRegistrationResponseTransmitterAuthDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_CUSTOM_DATA,            XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  TRUE,   DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE, CustomData ),            SIZEOF(DRM_PRND_CUSTOM_DATA),                                   4,      (DRM_VOID*)s_prndCustomDataDescription,                                 NO_OF( s_prndCustomDataDescription ) },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE,              XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE, Signature ),             SIZEOF(DRM_PRND_MESSAGE_SIGNATURE),                             5,      (DRM_VOID*)s_prndMessageSignatureDescription,                           NO_OF( s_prndMessageSignatureDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndRegistrationResponseHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_REGISTRATION_RESPONSE_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndRegistrationResponseFormatDescription =
{
    /* header description                           entry table                                  count of entries              */
    &s_prndRegistrationResponseHeaderDescription,   s_prndRegistrationResponseEntryDescriptions, NO_OF( s_prndRegistrationResponseEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Start(
        f_pbStack,
        f_cbStack,
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE,
        &g_prndRegistrationResponseFormatDescription ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Finish(
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE,
        &g_prndRegistrationResponseFormatDescription,
        f_pbMessage,
        f_pcbMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_RESPONSE_MESSAGE        *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Parse(
        f_pbStack,
        f_cbStack,
        f_pbMessage,
        f_cbMessage,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE,
        &g_prndRegistrationResponseFormatDescription,
        &f_pMessage->Signature.wSignatureType,
        &f_pMessage->Signature.xbbaSignature.cbData,
        f_pMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext,
        &f_pMessage->BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey,
        NULL ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Registration Error Message [TX->RX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndRegistrationErrorBasicInformationDescription[] =
{
    /* Type                      offsets                                                                            size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_ERROR_BASIC_INFORMATION, dwFlags ),           XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_REGISTRATION_ERROR_BASIC_INFORMATION, dwResult ),          XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndRegistrationErrorEntryDescriptions[DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                       Flags                       Parent Type                 Dups?   Opt?    offsets                                                                         size,                                                       sort,   entry/element descriptor                                            count */
    { XB_OBJECT_GLOBAL_HEADER,                                          XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                              SIZEOF(DRM_PRND_REGISTRATION_ERROR_MESSAGE),                0,      NULL,                                                               0  },
    { DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_BASIC_INFORMATION,         XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_REGISTRATION_ERROR_MESSAGE, BasicInformation ),         SIZEOF(DRM_PRND_REGISTRATION_ERROR_BASIC_INFORMATION),      1,      (DRM_VOID*)s_prndRegistrationErrorBasicInformationDescription,      NO_OF( s_prndRegistrationErrorBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndRegistrationErrorHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_REGISTRATION_ERROR_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndRegistrationErrorFormatDescription =
{
    /* header description                               entry table                                     count of entries              */
    &s_prndRegistrationErrorHeaderDescription,          s_prndRegistrationErrorEntryDescriptions,       NO_OF( s_prndRegistrationErrorEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndRegistrationErrorFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_ERROR_MESSAGE    *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndRegistrationErrorFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndRegistrationErrorFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Proximity Detection Start Messasge [RX->TX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndProximityDetectionStartBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                    size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_START_BASIC_INFORMATION, dwFlags ),            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_START_BASIC_INFORMATION, guidSessionID ),      XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndProximityDetectionStartEntryDescriptions[DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                       Flags                       Parent Type                 Dups?   Opt?    offsets                                                                         size,                                                         sort,   entry/element descriptor                                             count */
    { XB_OBJECT_GLOBAL_HEADER,                                          XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                              SIZEOF(DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE),           0,      NULL,                                                                0  },
    { DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_BASIC_INFORMATION,  XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE, BasicInformation ),  SIZEOF(DRM_PRND_PROXIMITY_DETECTION_START_BASIC_INFORMATION), 1,      (DRM_VOID*)s_prndProximityDetectionStartBasicInformationDescription, NO_OF( s_prndProximityDetectionStartBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndProximityDetectionStartHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndProximityDetectionStartFormatDescription =
{
    /* header description                               entry table                                     count of entries              */
    &s_prndProximityDetectionStartHeaderDescription,    s_prndProximityDetectionStartEntryDescriptions, NO_OF( s_prndProximityDetectionStartEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndProximityDetectionStartFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE    *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndProximityDetectionStartFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndProximityDetectionStartFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Proximity Detection Challenge Messasge [TX->RX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndProximityDetectionChallengeBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                                        size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, dwFlags ),                            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, guidSessionID ),                      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, guidNonce ),                          XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndProximityDetectionChallengeEntryDescriptions[DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                           Flags                       Parent Type                 Dups?   Opt?    offsets                                                                             size,                                                             sort,   entry/element descriptor                                                 count */
    { XB_OBJECT_GLOBAL_HEADER,                                              XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                                  SIZEOF(DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE),           0,      NULL,                                                                    0  },
    { DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION,  XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE, BasicInformation ),  SIZEOF(DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION), 1,      (DRM_VOID*)s_prndProximityDetectionChallengeBasicInformationDescription, NO_OF( s_prndProximityDetectionChallengeBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndProximityDetectionChallengeHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndProximityDetectionChallengeFormatDescription =
{
    /* header description                                   entry table                                         count of entries              */
    &s_prndProximityDetectionChallengeHeaderDescription,    s_prndProximityDetectionChallengeEntryDescriptions, NO_OF( s_prndProximityDetectionChallengeEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndProximityDetectionChallengeFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                          *f_pbStack,
    __in                                  DRM_DWORD                                          f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                          *f_pbMessage,
    __in                                  DRM_DWORD                                          f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE    *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndProximityDetectionChallengeFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndProximityDetectionChallengeFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Proximity Detection Response Messasge [RX->TX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndProximityDetectionResponseBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                                       size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, dwFlags ),                            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, guidSessionID ),                      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, wNonceEncryptionType ),               XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, guidNonceEncryptedWithMK ),           XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndProximityDetectionResponseEntryDescriptions[DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                           Flags                       Parent Type                 Dups?   Opt?    offsets                                                                            size,                                                            sort,   entry/element descriptor                                                count */
    { XB_OBJECT_GLOBAL_HEADER,                                              XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                                 SIZEOF(DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE),           0,      NULL,                                                                   0  },
    { DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION,   XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE, BasicInformation ),  SIZEOF(DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION), 1,      (DRM_VOID*)s_prndProximityDetectionResponseBasicInformationDescription, NO_OF( s_prndProximityDetectionResponseBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndProximityDetectionResponseHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndProximityDetectionResponseFormatDescription =
{
    /* header description                                  entry table                                        count of entries              */
    &s_prndProximityDetectionResponseHeaderDescription,    s_prndProximityDetectionResponseEntryDescriptions, NO_OF( s_prndProximityDetectionResponseEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndProximityDetectionResponseFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndProximityDetectionResponseFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndProximityDetectionResponseFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Proximity Detection Result Messasge [TX->RX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndProximityDetectionResultBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                                     size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, dwFlags ),                            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, guidSessionID ),                      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, dwResult ),                           XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndProximityDetectionResultEntryDescriptions[DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                       Flags                       Parent Type                 Dups?   Opt?    offsets                                                                          size,                                                          sort,   entry/element descriptor                                              count */
    { XB_OBJECT_GLOBAL_HEADER,                                          XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                               SIZEOF(DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE),           0,      NULL,                                                                 0  },
    { DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE, BasicInformation ),  SIZEOF(DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION), 1,      (DRM_VOID*)s_prndProximityDetectionResultBasicInformationDescription, NO_OF( s_prndProximityDetectionResultBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndProximityDetectionResultHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndProximityDetectionResultFormatDescription =
{
    /* header description                                entry table                                      count of entries              */
    &s_prndProximityDetectionResultHeaderDescription,    s_prndProximityDetectionResultEntryDescriptions, NO_OF( s_prndProximityDetectionResultEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndProximityDetectionResultFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE   *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndProximityDetectionResultFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndProximityDetectionResultFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND License Request Messasge [RX->TX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndLicenseRequestBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION, dwFlags ),                  XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION, guidSessionID ),            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION, dwReceiverRIV ),            XB_SIZE_IGNORED },
};

static const DRM_XB_ELEMENT_DESCRIPTION s_prndLicenseRequestLicenseInformationDescription[] =
{
    /* Type                      offsets                                                                                         size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION, wRequestedAction ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION, wRequestedActionQualifier ),       XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,      DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION, wContentIdentifierType ),          XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION, xbbaContentIdentifier ),           XB_SIZE_RELATIVE_DWORD },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndLicenseRequestEntryDescriptions[DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                               Flags                       Parent Type                 Dups?   Opt?    offsets                                                                 size,                                                 sort,   entry/element descriptor                                      count */
    { XB_OBJECT_GLOBAL_HEADER,                                  XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                      SIZEOF(DRM_PRND_LICENSE_REQUEST_MESSAGE),             0,      NULL,                                                         0  },
    { DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_BASIC_INFORMATION,    XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_MESSAGE, BasicInformation ),    SIZEOF(DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION),   1,      (DRM_VOID*)s_prndLicenseRequestBasicInformationDescription,   NO_OF( s_prndLicenseRequestBasicInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_LICENSE_INFORMATION,  XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_MESSAGE, LicenseInformation ),  SIZEOF(DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION), 2,      (DRM_VOID*)s_prndLicenseRequestLicenseInformationDescription, NO_OF( s_prndLicenseRequestLicenseInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_CUSTOM_DATA,          XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  TRUE,   DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_MESSAGE, CustomData ),          SIZEOF(DRM_PRND_CUSTOM_DATA),                         3,      (DRM_VOID*)s_prndCustomDataDescription,                       NO_OF( s_prndCustomDataDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE,            XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_REQUEST_MESSAGE, Signature ),           SIZEOF(DRM_PRND_MESSAGE_SIGNATURE),                   4,      (DRM_VOID*)s_prndMessageSignatureDescription,                 NO_OF( s_prndMessageSignatureDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndLicenseRequestHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_LICENSE_REQUEST_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndLicenseRequestFormatDescription =
{
    /* header description                      entry table                            count of entries              */
    &s_prndLicenseRequestHeaderDescription,    s_prndLicenseRequestEntryDescriptions, NO_OF( s_prndLicenseRequestEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Start(
        f_pbStack,
        f_cbStack,
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE,
        &g_prndLicenseRequestFormatDescription ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Finish(
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE,
        &g_prndLicenseRequestFormatDescription,
        f_pbMessage,
        f_pcbMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_REQUEST_MESSAGE              *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Parse(
        f_pbStack,
        f_cbStack,
        f_pbMessage,
        f_cbMessage,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE,
        &g_prndLicenseRequestFormatDescription,
        &f_pMessage->Signature.wSignatureType,
        &f_pMessage->Signature.xbbaSignature.cbData,
        f_pMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext,
        NULL,
        NULL ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND License Transmit Messasge [TX->RX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndLicenseTransmitBasicInformationDescription[] =
{
    /* Type                      offsets                                                                                    size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION, dwFlags ),                     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION, guidSessionID ),               XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION, guidCurrentLicenseMessageID ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION, xbbaTransmitterRevInfo ),      XB_SIZE_RELATIVE_DWORD },
};

static const DRM_XB_ELEMENT_DESCRIPTION s_prndLicenseTransmitLicenseInformationDescription[] =
{
    /* Type                      offsets                                                                                            size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION, xbbaReceiverBoundOrLeafXMRLicense ), XB_SIZE_RELATIVE_WORD },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndLicenseTransmitEntryDescriptions[DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                Flags                                          Parent Type                                        Dups?   Opt?    offsets                                                                      size,                                                   sort,   entry/element descriptor                                       count */
    { XB_OBJECT_GLOBAL_HEADER,                                   XB_FLAGS_NONE,                                 XB_OBJECT_TYPE_INVALID,                            FALSE,  FALSE,  0,                                                                           SIZEOF(DRM_PRND_LICENSE_TRANSMIT_MESSAGE),              0,      NULL,                                                           0  },
    { DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_BASIC_INFORMATION,    XB_FLAGS_MUST_UNDERSTAND,                      XB_OBJECT_GLOBAL_HEADER,                           FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_MESSAGE,      BasicInformation ),   SIZEOF(DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION),    1,      (DRM_VOID*)s_prndLicenseTransmitBasicInformationDescription,    NO_OF( s_prndLicenseTransmitBasicInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_LIST,         XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER, XB_OBJECT_GLOBAL_HEADER,                           FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_MESSAGE,      LicenseList ),        SIZEOF(DRM_PRND_LICENSE_TRANSMIT_LICENSE_LIST),         2,      NULL,                                                           0 },
    { DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION,  XB_FLAGS_MUST_UNDERSTAND,                      DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_LIST, TRUE,   FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_LICENSE_LIST, pLicenseListHead ),   SIZEOF(DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION),  0,      (DRM_VOID*)s_prndLicenseTransmitLicenseInformationDescription,  NO_OF( s_prndLicenseTransmitLicenseInformationDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_CUSTOM_DATA,          XB_FLAGS_MUST_UNDERSTAND,                      XB_OBJECT_GLOBAL_HEADER,                           FALSE,  TRUE,   DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_MESSAGE,      CustomData ),         SIZEOF(DRM_PRND_CUSTOM_DATA),                           3,      (DRM_VOID*)s_prndCustomDataDescription,                         NO_OF( s_prndCustomDataDescription ) },
    { DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE,            XB_FLAGS_MUST_UNDERSTAND,                      XB_OBJECT_GLOBAL_HEADER,                           FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_TRANSMIT_MESSAGE,      Signature ),          SIZEOF(DRM_PRND_MESSAGE_SIGNATURE),                     4,      (DRM_VOID*)s_prndMessageSignatureDescription,                   NO_OF( s_prndMessageSignatureDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndLicenseTransmitHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_LICENSE_TRANSMIT_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndLicenseTransmitFormatDescription =
{
    /* header description                       entry table                             count of entries              */
    &s_prndLicenseTransmitHeaderDescription,    s_prndLicenseTransmitEntryDescriptions, NO_OF( s_prndLicenseTransmitEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Start(
        f_pbStack,
        f_cbStack,
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE,
        &g_prndLicenseTransmitFormatDescription ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Finish(
        f_pcontextBuilder,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE,
        &g_prndLicenseTransmitFormatDescription,
        f_pbMessage,
        f_pcbMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_TRANSMIT_MESSAGE             *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;

    ChkDR( _MessageWithSignature_Parse(
        f_pbStack,
        f_cbStack,
        f_pbMessage,
        f_cbMessage,
        DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI,
        DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE,
        &g_prndLicenseTransmitFormatDescription,
        &f_pMessage->Signature.wSignatureType,
        &f_pMessage->Signature.xbbaSignature.cbData,
        f_pMessage,
        f_pfnSignatureCallback,
        f_pvSignatureContext,
        NULL,
        NULL ) );

ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND License Error Message [TX->RX]
**
******************************************************************************/
static const DRM_XB_ELEMENT_DESCRIPTION s_prndLicenseErrorBasicInformationDescription[] =
{
    /* Type                      offsets                                                                            size (second param only necessary for absolute) */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION, dwFlags ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_GUID,      DRM_OFFSET_OF( DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION, guidSessionID ),          XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION, dwResult ),               XB_SIZE_IGNORED },
};

static const DRM_XB_ENTRY_DESCRIPTION s_prndLicenseErrorEntryDescriptions[DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_OBJECT_TYPE_COUNT] =
{
    /* Entry Type                                                       Flags                       Parent Type                 Dups?   Opt?    offsets                                                                         size,                                                       sort,   entry/element descriptor                                            count */
    { XB_OBJECT_GLOBAL_HEADER,                                          XB_FLAGS_NONE,              XB_OBJECT_TYPE_INVALID,     FALSE,  FALSE,  0,                                                                              SIZEOF(DRM_PRND_LICENSE_ERROR_MESSAGE),                     0,      NULL,                                                               0  },
    { DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_BASIC_INFORMATION,              XB_FLAGS_MUST_UNDERSTAND,   XB_OBJECT_GLOBAL_HEADER,    FALSE,  FALSE,  DRM_OFFSET_OF( DRM_PRND_LICENSE_ERROR_MESSAGE, BasicInformation ),              SIZEOF(DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION),           1,      (DRM_VOID*)s_prndLicenseErrorBasicInformationDescription,           NO_OF( s_prndLicenseErrorBasicInformationDescription ) },
};

static const DRM_XB_HEADER_DESCRIPTION s_prndLicenseErrorHeaderDescription = { XB_FORMAT_ID_LENGTH_QWORD, DRM_PRND_LICENSE_ERROR_MESSAGE_FORMAT_ID, DRM_PRND_CURRENT_VERSION, NULL, 0 };

static const DRM_XB_FORMAT_DESCRIPTION g_prndLicenseErrorFormatDescription =
{
    /* header description                               entry table                                     count of entries              */
    &s_prndLicenseErrorHeaderDescription,               s_prndLicenseErrorEntryDescriptions,            NO_OF( s_prndLicenseErrorEntryDescriptions )
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_StartFormat( f_pbStack, f_cbStack, DRM_PRND_CURRENT_VERSION, f_pcontextBuilder, &g_prndLicenseErrorFormatDescription ) );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder, f_pbMessage, f_pcbMessage ) );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_ERROR_MESSAGE                *f_pMessage )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT stack           = {0};
    DRM_DWORD                   dwVersionFound  = 0;
    DRM_RESULT                  drResult        = DRM_SUCCESS;

    ChkDR( DRM_STK_Init( &stack, f_pbStack, f_cbStack ) );
    ChkDR( _ValidateMessageType( f_pbMessage, f_cbMessage, &g_prndLicenseErrorFormatDescription ) );
    drResult = DRM_XB_UnpackBinary( f_pbMessage, f_cbMessage, &stack, &g_prndLicenseErrorFormatDescription, 1, &dwVersionFound, f_pMessage );
    ChkBOOL( dwVersionFound == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );
    ChkDR( drResult );
ErrorExit:
    return _RemapPRNDFormatError( dr );
}

/******************************************************************************
**
** PRND Message type functions (Tx and Rx)
**
******************************************************************************/

#define PRND_GET_MSG_TYPE( msgFormatID, msgType )                                  \
do {                                                                               \
    DRM_UINT64 qwMsgFormatID = msgFormatID;                                        \
    if( ( DRM_UI64Low32( qwMsgFormatID )  == DRM_UI64Low32( qwMagicConstant ) )    \
     && ( DRM_UI64High32( qwMsgFormatID ) == DRM_UI64High32( qwMagicConstant ) ) ) \
    {                                                                              \
        *f_peMessageType = (msgType);                                              \
        goto ErrorExit;                                                            \
    }                                                                              \
} while( FALSE )

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_TxRx_GetMessageType(
    __inout_bcount( f_cbUnknownMessage )                        const DRM_BYTE                                         *f_pbUnknownMessage,
    __in                                                              DRM_DWORD                                         f_cbUnknownMessage,
    __out                                                             DRM_PRND_MESSAGE_TYPE                            *f_peMessageType )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_UINT64  qwMagicConstant = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD   dwVersion       = 0;
    DRM_DWORD   icb             = 0;

    ChkArg( f_peMessageType    != NULL );
    *f_peMessageType = DRM_PRND_MESSAGE_TYPE_INVALID;

    ChkArg( f_pbUnknownMessage != NULL );
    ChkArg( f_cbUnknownMessage  > 0    );

    ChkBOOL( f_cbUnknownMessage > XB_HEADER_LENGTH( XB_FORMAT_ID_LENGTH_QWORD ), DRM_E_PRND_MESSAGE_INVALID );

    NETWORKBYTES_TO_QWORD( qwMagicConstant, f_pbUnknownMessage, icb );
    ChkDR( DRM_DWordAdd( icb, XB_FORMAT_ID_LENGTH_QWORD, &icb ) );
    NETWORKBYTES_TO_DWORD( dwVersion,       f_pbUnknownMessage, icb );
    ChkDR( DRM_DWordAdd( icb, SIZEOF(DRM_DWORD), &icb ) );

    ChkBOOL( dwVersion == DRM_PRND_CURRENT_VERSION, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /*
    ** Can't use a switch statement since it's a 64 bit type
    */
    PRND_GET_MSG_TYPE( DRM_PRND_REGISTRATION_REQUEST_MESSAGE_FORMAT_ID,          DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST          );
    PRND_GET_MSG_TYPE( DRM_PRND_REGISTRATION_RESPONSE_MESSAGE_FORMAT_ID,         DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE         );
    PRND_GET_MSG_TYPE( DRM_PRND_REGISTRATION_ERROR_MESSAGE_FORMAT_ID,            DRM_PRND_MESSAGE_TYPE_REGISTRATION_ERROR            );
    PRND_GET_MSG_TYPE( DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE_FORMAT_ID,     DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START     );
    PRND_GET_MSG_TYPE( DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE_FORMAT_ID, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_CHALLENGE );
    PRND_GET_MSG_TYPE( DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE_FORMAT_ID,  DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE  );
    PRND_GET_MSG_TYPE( DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE_FORMAT_ID,    DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT    );
    PRND_GET_MSG_TYPE( DRM_PRND_LICENSE_REQUEST_MESSAGE_FORMAT_ID,               DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST               );
    PRND_GET_MSG_TYPE( DRM_PRND_LICENSE_TRANSMIT_MESSAGE_FORMAT_ID,              DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT              );
    PRND_GET_MSG_TYPE( DRM_PRND_LICENSE_ERROR_MESSAGE_FORMAT_ID,                 DRM_PRND_MESSAGE_TYPE_LICENSE_ERROR                 );

    ChkDR( DRM_E_PRND_MESSAGE_INVALID );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_PRND_TxRx_IncrementLMID(
    __inout_ecount( 1 )                                               DRM_GUID                                         *f_pguidLMID )
{
    /*
    ** LMID is a 128-bit big-endian unsigned integer value.
    */
    DRM_DWORD   *pdw128     = (DRM_DWORD*)f_pguidLMID;
    DRM_DWORD   *pdwLowest  = &pdw128[3];
    DRM_DWORD   *pdwLow     = &pdw128[2];
    DRM_DWORD   *pdwHigh    = &pdw128[1];
    DRM_DWORD   *pdwHighest = &pdw128[0];

    /* Convert each DWORD to big endian (if required) */
#if TARGET_LITTLE_ENDIAN
    REVERSE_BYTES_DWORD(*pdwHighest);
    REVERSE_BYTES_DWORD(*pdwHigh);
    REVERSE_BYTES_DWORD(*pdwLow);
    REVERSE_BYTES_DWORD(*pdwLowest);
#endif /* TARGET_LITTLE_ENDIAN */

    if( *pdwLowest != MAX_UNSIGNED_TYPE( DRM_DWORD ) )
    {
        *pdwLowest = *pdwLowest + 1;
    }
    else if( *pdwLow != MAX_UNSIGNED_TYPE( DRM_DWORD ) )
    {
        *pdwLowest = 0;
        *pdwLow    = *pdwLow + 1;
    }
    else if( *pdwHigh != MAX_UNSIGNED_TYPE( DRM_DWORD ) )
    {
        *pdwLowest = 0;
        *pdwLow    = 0;
        *pdwHigh   = *pdwHigh + 1;
    }
    else if( *pdwHighest != MAX_UNSIGNED_TYPE( DRM_DWORD ) )
    {
        *pdwLowest  = 0;
        *pdwLow     = 0;
        *pdwHigh    = 0;
        *pdwHighest = *pdwHighest + 1;
    }
    else
    {
        /* All zeros for LMID is invalid so maximum value wraps to 1, not to 0 (per spec). */
        *pdwLowest  = 1;
        *pdwLow     = 0;
        *pdwHigh    = 0;
        *pdwHighest = 0;
    }

    /* Convert each DWORD back to native endian (if required) */
#if TARGET_LITTLE_ENDIAN
    REVERSE_BYTES_DWORD(*pdwHighest);
    REVERSE_BYTES_DWORD(*pdwHigh);
    REVERSE_BYTES_DWORD(*pdwLow);
    REVERSE_BYTES_DWORD(*pdwLowest);
#endif /* TARGET_LITTLE_ENDIAN */
}

/******************************************************************************
**
** PRND Blackbox forwarding methods (Tx and Rx)
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Tx_SignWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                        OEM_CRYPTO_HANDLE                f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                        *f_pbData,
    __in                                    DRM_DWORD                        f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )        DRM_BYTE                         f_rgbSignature[DRM_AES_BLOCKLEN] )
{
    return DRM_BBX_PRND_Tx_SignWithAESOMAC(
        f_poBBXContext,
        f_handleAESIntegrityKey,
        f_pbData,
        f_cbData,
        f_rgbSignature );
}

EXIT_PK_NAMESPACE_CODE;

