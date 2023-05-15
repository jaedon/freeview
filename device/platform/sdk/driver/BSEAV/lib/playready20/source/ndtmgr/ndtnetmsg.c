/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** This file implements the basic ND streaming message packing and unpacking.
** The algorithms are transport protocol agnostic.
*/

#include <drmcommon.h>
#include <drmcrt.h>
#include <oemndt.h>
#include <ndtnetmsg.h>
#include <ndtreg.h>
#include <oemrsaoaeppss.h>

ENTER_PK_NAMESPACE_CODE;


/*
 * Return the fixed size of any particuler message type.
 *  The fixed size doesn't include any variable length fields.
 *
 * Parameters:
 *
 *      MessageType - Type of the message being allocated
 *
 * Return Value:
 *      The fixed size of the message type.
 *      Returns 0 if the message type isn't recognized
 */
DRM_API DRM_DWORD DRM_CALL WmdrmNetFixedMessageSize(
    __in const DRM_BYTE f_bMessageType
)
{
    DRM_DWORD Size = 0;

    switch ( f_bMessageType )
    {
    case WMDRMNET_REGISTRATION_MESSAGE_TYPE:
        Size = WMDRMNET_REGISTRATION_MESSAGE_SIZE;
        break;
    case WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE:
        Size = WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_SIZE;
        break;
    case WMDRMNET_PROXIMITY_START_MESSAGE_TYPE:
        Size = WMDRMNET_PROXIMITY_START_MESSAGE_SIZE;
        break;
    case WMDRMNET_PROXIMITY_CHALLENGE_MESSAGE_TYPE:
        Size = WMDRMNET_PROXIMITY_CHALLENGE_MESSAGE_SIZE;
        break;
    case WMDRMNET_PROXIMITY_RESPONSE_MESSAGE_TYPE:
        Size = WMDRMNET_PROXIMITY_RESPONSE_MESSAGE_SIZE;
        break;
    case WMDRMNET_PROXIMITY_RESULT_MESSAGE_TYPE:
        Size = WMDRMNET_PROXIMITY_RESULT_MESSAGE_SIZE;
        break;
    case WMDRMNET_LICENSE_REQUEST_MESSAGE_TYPE:
        Size = WMDRMNET_LICENSE_REQUEST_MESSAGE_SIZE;
        break;
    case WMDRMNET_LICENSE_RESPONSE_MESSAGE_TYPE:
        Size = WMDRMNET_LICENSE_RESPONSE_MESSAGE_SIZE;
        break;
    case WMDRMNET_REVOCATION_LIST_REQUEST_MESSAGE_TYPE:
        Size = WMDRMNET_REVOCATION_LIST_REQUEST_MESSAGE_SIZE;
        break;
    case WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE:
        Size = WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_SIZE;
        break;
    default:
        break;
    }

    return Size;
}



/*
 * The WmdrmNetAllocateMessageDescriptor allocates a descriptor for unpacking a message
 *
 * This routine should be used with the WmdrmNetUnpack* routines where * is a data type to
 * be unpacked from the buffer. Each WmdrmNetUnpack* routine handles checking the buffer bounds,
 * unpacking the data type from the buffer, and converting from network byte order.
 *
 * Parameters:
 *
 *      Message - Specifies the message buffer to unpack; may be NULL
 *
 *      MessageSize - Specifies the size (in bytes) of the complete message
 *
 *      MessageDescriptor - Returns a descriptor to the message.
 *          The returned descriptor should be free via WmdrmNetFreeMessageDescriptor.
 *
 * Return Value:
 *  DRM_SUCCESS - The operation completed successfully
 *  DRM_E_ND_INVALID_MESSAGE_VERSION
 *  DRM_E_ND_INVALID_MESSAGE_TYPE
 *  DRM_E_ND_INVALID_MESSAGE
*/
DRM_API DRM_RESULT DRM_CALL WmdrmNetAllocateMessageDescriptor(
    __in_bcount_opt(MessageSize) DRM_BYTE                    * const Message,
    __in const                   DRM_DWORD                           MessageSize,
    __out                        WMDRMNET_MESSAGE_DESCRIPTOR       **MessageDescriptor
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *LocalMessageDescriptor = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetAllocateMessageDescriptor );

    /*
     * Initialization
     */
    *MessageDescriptor = NULL;

    /*
     * Allocate the descriptor for the message
     */
    LocalMessageDescriptor = (WMDRMNET_MESSAGE_DESCRIPTOR *)Oem_MemAlloc( SIZEOF(WMDRMNET_MESSAGE_DESCRIPTOR) );
    ChkMem( LocalMessageDescriptor );

    LocalMessageDescriptor->Buffer = Message;
    LocalMessageDescriptor->BufferSize = MessageSize;
    LocalMessageDescriptor->Offset = 0;

    /*
     * Return the descriptor to the caller
     */
    *MessageDescriptor = LocalMessageDescriptor;
    LocalMessageDescriptor = NULL;

ErrorExit:
    if ( LocalMessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( LocalMessageDescriptor, FALSE );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*
 * The _AllocateMessage routine allocates a buffer containing a WMDRMNET message.
 * The returned buffer has the message type and message version filled in.
 *
 * This routine should be used with the WmdrmNetPack* routines where * is a data type to
 * be packed into the buffer. Each WmdrmNetPack* routine handles checking the buffer bounds
 * and packing the data type into the buffer in network byte order.
 *
 * Parameters:
 *
 *      MessageType - Type of the message being allocated
 *
 *      VariableSize - Size (in bytes) of the variable length part of the message to allocate
 *
 * Return Value:
 *  Pointer to allocated message.  The returned buffer should be freed using WmdrmNetFreeMessageDescriptor.
 *  NULL - Not enough memory.
 */
static WMDRMNET_MESSAGE_DESCRIPTOR * _AllocateMessage(
    __in const DRM_BYTE  MessageType,
    __in const DRM_DWORD VariableSize
)
{
    DRM_RESULT dr;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    /*
     * Ensure the passed in size is reasonable
     *  Just a sanity check to prevent overflow.
     */
    if ( VariableSize > ((DRM_DWORD)128*(DRM_DWORD)1024) )
    {
        goto ErrorExit;
    }

    /*
     * Allocate an empty descriptor for the message
     */
    ChkDR( WmdrmNetAllocateMessageDescriptor( NULL, 0, &MessageDescriptor ) );

    /*
     * Allocate the buffer
     */
    MessageDescriptor->BufferSize = WmdrmNetFixedMessageSize( MessageType );

    if ( MessageDescriptor->BufferSize == 0 )
    {
        DRMASSERT( MessageDescriptor->BufferSize != 0 );
        goto ErrorExit;
    }

    MessageDescriptor->BufferSize += VariableSize;

    ChkMem( MessageDescriptor->Buffer = (DRM_BYTE*)Oem_MemAlloc( MessageDescriptor->BufferSize ) );

    /*
     * Pack the message version into the message
     */
    WmdrmNetPackOctet( MessageDescriptor, WMDRMNET_VERSION_2 );

    /*
     * Pack the message type into the message
     */
    WmdrmNetPackOctet( MessageDescriptor, MessageType );

    /*
     * Return the buffer to the caller
     */
    return MessageDescriptor;

ErrorExit:

    /*
     * Clean up on error
     */
    if ( MessageDescriptor != NULL ) {

        if ( MessageDescriptor->Buffer != NULL ) {
            Oem_MemFree( MessageDescriptor->Buffer );
        }

        /* Free the descriptor itself */
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, FALSE );
    }

    return NULL;
}



/*
 * The _PackOctets routine packs a sequence of octets into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the value into.
 *
 *      Value - Specifies the value to pack into the buffer.
 *
 *      ValueSize - Specifies the size (in bytes) of the value
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
static DRM_VOID _PackOctets(
    __out                        WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in_bcount(ValueSize) const DRM_BYTE                    * const Value,
    __in                   const DRM_DWORD                           ValueSize
)
{
    DRM_DWORD i;

    if( Value == NULL && ValueSize > 0 )
    {
        DRMASSERT( !( Value == NULL && ValueSize > 0 ) );
        return;
    }

    for ( i=0; i<ValueSize; i++)
    {
        WmdrmNetPackOctet( MessageDescriptor, GET_BYTE(Value, i) );
    }
}



/*
 * The WmdrmNetPackOctet routine packs a single octet into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the octet into.
 *
 *      Octet - Specifies the octet to pack into the buffer.
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
DRM_API DRM_VOID DRM_CALL WmdrmNetPackOctet(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in const DRM_BYTE                            Octet
)
{
    /*
     * Check to see if the octet fits
     *  This is always an internal error
     */
    if ( MessageDescriptor->Offset >= MessageDescriptor->BufferSize )
    {
        DRMASSERT( MessageDescriptor->Offset < MessageDescriptor->BufferSize);
        return;
    }

    /*
     * Push the octet into the buffer
     */
    PUT_BYTE(MessageDescriptor->Buffer, MessageDescriptor->Offset, Octet);
    MessageDescriptor->Offset++;

    return;
}



/*
 * The WmdrmNetPackWord routine packs a 2-byte unsigned integer into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the value into.
 *
 *      Value - Specifies the value to pack into the buffer.
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
DRM_API DRM_VOID DRM_CALL WmdrmNetPackWord(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in const DRM_WORD                            Value
)
{
    int i;

    /*
     * Pack each octet into the buffer
     */
#if TARGET_LITTLE_ENDIAN
    for ( i=SIZEOF(DRM_WORD)-1; i>=0; i-- )
#else
    for ( i=0; i<SIZEOF(DRM_WORD); i++ )
#endif
    {
        WmdrmNetPackOctet( MessageDescriptor, GET_BYTE((DRM_BYTE *)&Value, i) );
    }

}



/*
 * The WmdrmNetPackDword routine packs a 4-byte unsigned integer into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the value into.
 *
 *      Value - Specifies the value to pack into the buffer.
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
DRM_API DRM_VOID DRM_CALL WmdrmNetPackDword(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in const DRM_DWORD                           Value
)
{
    int i;

    /*
     * Pack each octet into the buffer
     */
#if TARGET_LITTLE_ENDIAN
    for ( i=SIZEOF(DRM_DWORD)-1; i>=0; i-- )
#else
    for ( i=0; i<SIZEOF(DRM_DWORD); i++ )
#endif
    {
        WmdrmNetPackOctet( MessageDescriptor, GET_BYTE( (DRM_BYTE *)&Value, i) );
    }

}



/*
 * The WmdrmNetPackUint64 routine packs a 4-byte unsigned integer into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the value into.
 *
 *      Value - Specifies the value to pack into the buffer.
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
DRM_API DRM_VOID DRM_CALL WmdrmNetPackUint64(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in const DRM_UINT64                          Value
)
{
    int i;

    /*
     * Pack each octet into the buffer
     */
#if TARGET_LITTLE_ENDIAN
    for ( i=SIZEOF(DRM_UINT64)-1; i>=0; i-- )
#else
    for ( i=0; i<SIZEOF(DRM_UINT64); i++ )
#endif
    {
        WmdrmNetPackOctet( MessageDescriptor, GET_BYTE( (DRM_BYTE *)&Value, i) );
    }

}



/*
 * The _PackVal128 routine packs a 128-bit value into a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to pack the value into.
 *
 *      Value - Specifies the value to pack into the buffer.
 *
 * Return Value:
 *  None - The routine asserts if there isn't enough room in the buffer
 */
static DRM_VOID _PackVal128(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __in const DRM_ID                      * const Value
)
{
    _PackOctets( MessageDescriptor, (DRM_BYTE *)Value, SIZEOF(DRM_ID)  );
}



/*
 * The WmdrmNetGetMessageType routine gets the message type from the message
 *
 * This routine verfies that the protocol version is understood and gets a version agnostic
 * message type.
 *
 * Parameters:
 *
 *      Message - Specifies the message to get the message type from
 *
 *      MessageSize - Specifies the size (in bytes) of the complete message
 *
 * Return Value:
 *  Returns the message type.
 *  Returns 0 if the message type cannot be determined
 *  May return values that don't correspond to a valid message type
 */
DRM_API DRM_BYTE DRM_CALL WmdrmNetGetMessageType(
    __in_bcount(MessageSize) const DRM_BYTE  * const Message,
    __in                     const DRM_DWORD         MessageSize
)
{
    /*
     * Ensure there is enough room for the message type in the message
     */
    if ( MessageSize < 2 )
    {
        return 0;
    }

    /*
     * Ensure the message version is understood
     */
    if ( Message[0] < WMDRMNET_VERSION_2 )
    {
        return 0;
    }

    /*
     * Return the message type
     */
    return Message[1];
}



/*
 * The _ValidateMessage routine verifies the header of the WMDRMNET message
 *
 * This routine verfies that the protocol version is understood and that the
 * message type is the expected one.
 *
 * This routine should be used with the WmdrmNetUnpack* routines where * is a data type to
 * be unpacked from the buffer. Each WmdrmNetUnpack* routine handles checking the buffer bounds,
 * unpacking the data type from the buffer, and converting from network byte order.
 *
 * Parameters:
 *
 *      Message - Specifies the message to validate
 *
 *      MessageSize - Specifies the size (in bytes) of the complete message
 *
 *      ExpectedMessageType - Specifies the type of message that is expected
 *
 *      MessageDescriptor - Returns a descriptor to the message.
 *          The returned descriptor should be freed via WmdrmNetFreeMessageDescriptor.
 *
 * Return Value:
 *  DRM_SUCCESS - The operation completed successfully
 *  DRM_E_ND_INVALID_MESSAGE_VERSION
 *  DRM_E_ND_INVALID_MESSAGE_TYPE
 *  DRM_E_ND_INVALID_MESSAGE
 */
static DRM_RESULT _ValidateMessage(
    __in_bcount(MessageSize) DRM_BYTE                    * const Message,
    __in const               DRM_DWORD                           MessageSize,
    __in const               DRM_BYTE                            ExpectedMessageType,
    __out                    WMDRMNET_MESSAGE_DESCRIPTOR       **MessageDescriptor
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *LocalMessageDescriptor = NULL;
    DRM_BYTE ProtocolVersion;
    DRM_BYTE MessageType;
    DRM_DWORD MinimumMessageSize;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC__ValidateMessage );

    *MessageDescriptor = NULL;

    /*
    ** Allocate the descriptor for the message
    */
    ChkDR( WmdrmNetAllocateMessageDescriptor( Message, MessageSize, &LocalMessageDescriptor ) );

    /*
    ** Unpack and check the protocol version
    */
    ChkBOOL( WmdrmNetUnpackOctet( LocalMessageDescriptor, &ProtocolVersion), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( ProtocolVersion >= WMDRMNET_VERSION_2, DRM_E_ND_INVALID_MESSAGE_VERSION );

    /*
    ** Unpack and check the message type
    */
    ChkBOOL( WmdrmNetUnpackOctet( LocalMessageDescriptor, &MessageType), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( MessageType == ExpectedMessageType, DRM_E_ND_INVALID_MESSAGE_TYPE );

    /*
    ** Check the message size
    */
    MinimumMessageSize = WmdrmNetFixedMessageSize( MessageType );
    if ( ( MinimumMessageSize == 0 ) || ( MessageSize < MinimumMessageSize ) )
    {
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }

    /*
    ** Return the descriptor to the caller
    */
    *MessageDescriptor = LocalMessageDescriptor;

ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        WmdrmNetFreeMessageDescriptor( LocalMessageDescriptor, FALSE );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*
 * The WmdrmNetUnpackOctet routine unpacks a single octet from a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to unpack the octet from
 *
 *      Octet - Returns the octet unpacked from the buffer.
 *
 * Return Value:
 *  TRUE - The octet was successfully unpacked from the buffer
 *  FALSE - The buffer was too small
 */
DRM_API DRM_BOOL DRM_CALL WmdrmNetUnpackOctet(
    __inout                        WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __out_bcount(SIZEOF(DRM_BYTE)) DRM_BYTE                    * const Octet
)
{

    /*
     * Check to see if an octet is available
     */
    if ( MessageDescriptor->Offset >= MessageDescriptor->BufferSize )
    {
        return FALSE;
    }

    /*
     * Pop the octet from the buffer
     */
    *Octet = GET_BYTE(MessageDescriptor->Buffer, MessageDescriptor->Offset);
    MessageDescriptor->Offset++;

    return TRUE;
}



/*
 * The WmdrmNetUnpackWord routine unpacks a 2-byte unsigned integer from a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to unpack the value from
 *
 *      Value - Returns the value unpacked from the buffer.
 *
 * Return Value:
 *  TRUE - The octet was successfully unpacked from the buffer
 *  FALSE - The buffer was too small
 */
DRM_API DRM_BOOL DRM_CALL WmdrmNetUnpackWord(
    __inout                        WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __out_bcount(SIZEOF(DRM_WORD)) DRM_WORD                    * const Value
)
{
    int i;

    /*
     * Unpack each octet from the buffer
     */
#if TARGET_LITTLE_ENDIAN
    for ( i=SIZEOF(DRM_WORD)-1; i>=0; i-- )
#else
    for ( i=0; i<SIZEOF(DRM_WORD); i++ )
#endif
    {
        DRM_BYTE b;
        if ( !WmdrmNetUnpackOctet( MessageDescriptor, &b ) )
        {
            return FALSE;
        }
        PUT_BYTE( (DRM_BYTE *)Value, i, b);
    }

    return TRUE;
}



/*
 * The WmdrmNetUnpackDword routine unpacks a 4-byte unsigned integer from a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to unpack the value from
 *
 *      Value - Returns the value unpacked from the buffer.
 *
 * Return Value:
 *  TRUE - The octet was successfully unpacked from the buffer
 *  FALSE - The buffer was too small
 */
DRM_API DRM_BOOL DRM_CALL WmdrmNetUnpackDword(
    __inout                         WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __out_bcount(SIZEOF(DRM_DWORD)) DRM_DWORD                   * const Value
)
{
    int i;

    /*
     * Unpack each octet from the buffer
     */
#if TARGET_LITTLE_ENDIAN
    for ( i=SIZEOF(DRM_DWORD)-1; i>=0; i-- )
#else
    for ( i=0; i<SIZEOF(DRM_DWORD); i++ )
#endif
    {
        DRM_BYTE b;
        if ( !WmdrmNetUnpackOctet( MessageDescriptor, &b ) )
        {
            return FALSE;
        }
        PUT_BYTE((DRM_BYTE *)Value, i, b);
    }

    return TRUE;
}



/*
 * The WmdrmNetUnpackVal128 routine unpacks a 128-bit unsigned integer from a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to unpack the value from
 *
 *      Value - Returns the value unpacked from the buffer.
 *
 * Return Value:
 *  TRUE - The octet was successfully unpacked from the buffer
 *  FALSE - The buffer was too small
 */
DRM_API DRM_BOOL DRM_CALL WmdrmNetUnpackVal128(
    __inout                      WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __out_bcount(SIZEOF(DRM_ID)) DRM_ID                      * const Value
)
{
    DRM_DWORD i;

    for ( i=0; i<SIZEOF(DRM_ID) ; i++)
    {
        DRM_BYTE b;
        if ( !WmdrmNetUnpackOctet( MessageDescriptor, &b ) )
        {
            return FALSE;
        }
        PUT_BYTE((DRM_BYTE *)Value, i, b);
    }

    return TRUE;
}



/*
 * The WmdrmNetUnpackOctets routine unpacks a sequence of octets from a message buffer.
 *
 * Parameters:
 *
 *      MessageDescriptor - Specifies the descriptor of the buffer to unpack the value from
 *
 *      Value - Returns the value unpacked from the buffer.
 *          caller must free
 *
 *      ValueSize - Specifies the number of bytes to unpack from the buffer
 *
 * Note: For ti 16-bit platform, storage for Value must be allocated by
 *       this function because it is not possible to address 8 bits of memory.
 *       The offset into the message may not be 16-bit aligned and hence
 *       assigning a pointer to a misaligned offset will fail.  Allocating
 *       storage and copying data into the new storage ensures 16-bit alignment.
 *
 * Return Value:
 *  TRUE - The octet was successfully unpacked from the buffer
 *  FALSE - The buffer was too small or memory allocation failed
 */
DRM_API DRM_BOOL DRM_CALL WmdrmNetUnpackOctets(
    __inout                       WMDRMNET_MESSAGE_DESCRIPTOR * const MessageDescriptor,
    __deref_out_bcount(ValueSize) DRM_BYTE                          **Value,
    __in const                    DRM_DWORD                           ValueSize
)
{
    DRM_BYTE *pbOctets = NULL;

    if ( NULL == Value )
    {
        goto ErrorExit;
    }
    *Value = NULL;

    /*
     * Check to see if there are enough octets in the buffer. Guard against overflow.
     */
    if( ( MessageDescriptor->Offset + ValueSize < MessageDescriptor->Offset ) ||
        ( MessageDescriptor->Offset + ValueSize > MessageDescriptor->BufferSize ) )
    {
        goto ErrorExit;
    }

    /*
     * Pop the octets from the buffer
     */
    pbOctets = (DRM_BYTE *) Oem_MemAlloc(ValueSize == 0 ? 1 : ValueSize);
    if ( NULL == pbOctets )
    {
        goto ErrorExit;
    }

    DRM_BYT_CopyBytes(pbOctets, 0, MessageDescriptor->Buffer, MessageDescriptor->Offset, ValueSize);
    MessageDescriptor->Offset += ValueSize;
    *Value = pbOctets;
    pbOctets = NULL;
    return TRUE;

 ErrorExit:
    SAFE_OEM_FREE(pbOctets);

    return FALSE;
}



/*
 * The _MoveToOffset routine sets the current offset into a message
 *
 * Parameters:
 *
 *      f_MessageDescriptor - Specifies the descriptor of the buffer to offset into
 *
 *      f_dwOffset - Specifies the number of bytes to offset from the start of the buffer
 *
 * Return Value:
 *  TRUE - The current offset into the buffer was set as per specified value
 *  FALSE - The buffer was too small
 */
static DRM_BOOL _MoveToOffset(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR * const f_MessageDescriptor,
    __in const DRM_DWORD                           f_dwOffset
)
{
    /*
     * Check to see if there are enough octets in the buffer
     */
    if ( f_dwOffset > f_MessageDescriptor->BufferSize )
    {
        return FALSE;
    }

    /*
     * Set the current offset into the buffer
     */
    f_MessageDescriptor->Offset = f_dwOffset;

    return TRUE;
}



/*
 * The WmdrmNetFreeMessageDescriptor routine frees the message descriptor
 *
 * Parameters:
 *
 *      f_MessageDescriptor - Specifies the descriptor to free
 *
 *      f_fDone - True if all octets were packed or unpacked from the buffer
 *
 * Return Value:
 *  None - Asserts if the byte count not totally consumed
 */
DRM_API DRM_VOID DRM_CALL WmdrmNetFreeMessageDescriptor(
    __out      WMDRMNET_MESSAGE_DESCRIPTOR *f_MessageDescriptor,
    __in const DRM_BOOL                     f_fDone
)
{
    /*
     * Ensure the byte count was computed correctly
     */
    if ( f_fDone )
    {
        DRMASSERT( f_MessageDescriptor->BufferSize == f_MessageDescriptor->Offset );
    }

    /*
     * Free the descriptor
     */
    SAFE_OEM_FREE( f_MessageDescriptor );
}



/**********************************************************************
**
** Function:    WmdrmNetUnpackRegistrationRequestMessage
**
** Synopsis:    Unpacks a WMDRM-ND Registration Request Message
**              into its component parts.
**
** Arguments:   [f_pbMessage] - Specifies the registration response message
**              [f_cbMessage] - Specifies the size (in bytes) of the registration response message
**              [f_pSerialNumber] - Returns the Serial Number from the message
**              [f_ppbDeviceCertificate] - Returns a pointer to the certificate within the response message
**                                         must be freed by caller
**              [f_pcbDeviceCertificate] - Returns the size (in bytes) of the returned certificate
**
** Returns:
**  DRM_SUCCESS - The operation completed successfully
**  DRM_E_ND_INVALID_MESSAGE
**  DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackRegistrationRequestMessage(
    __in_bcount(f_cbMessage)                    DRM_BYTE  * const f_pbMessage,
    __in const                                  DRM_DWORD         f_cbMessage,
    __out                                       DRM_ID    * const f_pSerialNumber,
    __deref_out_bcount(*f_pcbDeviceCertificate) DRM_BYTE        **f_ppbDeviceCertificate,
    __out                                       DRM_DWORD        *f_pcbDeviceCertificate
)
{
    DRM_RESULT dr;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRegistrationRequestMessage );

    ChkArg( f_pbMessage && f_pSerialNumber && f_ppbDeviceCertificate && f_pcbDeviceCertificate );
    *f_ppbDeviceCertificate = NULL;

    /*
    ** Validate the message
    */
    dr = _ValidateMessage( f_pbMessage,
                           f_cbMessage,
                           WMDRMNET_REGISTRATION_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate Registration Request Message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Serial Number from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, f_pSerialNumber ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Registration Request Message too short (Serial Number)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Device Certificate size from the message
    */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, f_pcbDeviceCertificate ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Registration Request Message too short (DeviceCertificateSize Unpack Failed)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Device Certificate from the message
    */
    if ( 0 == *f_pcbDeviceCertificate )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Registration Request Message too short (Device Certificate Size = 0)\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctets( MessageDescriptor, f_ppbDeviceCertificate, *f_pcbDeviceCertificate ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Registration Request Message too short (DeviceCertificate Unpack Failed)\n" ));
        goto ErrorExit;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        SAFE_OEM_FREE(*f_ppbDeviceCertificate);
    }
    if ( MessageDescriptor != NULL ) {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetPackRegistrationResponseMessage
**
** Synopsis:    Builds a WMDRM-ND registration response message from its component parts
**
** Arguments:   [f_pSerialNumber]    - Specifies the Serial Number to pack into the message
**              [f_pDevicePublicKey] - Specifies the Device RSA Public Key to encrypt the Seed
**              [f_cbPubKeyModulus]  - Specifies the size of the RSA Public Key Modulus
**              [f_pSessionId]       - Specifies the Session ID
**              [f_pSeed]            - Specifies the Seed
**              [f_pbAddress]        - Specifies the Address to use for Proximity Detection
**              [f_cbAddress]        - Size of Address, in bytes.
**              [f_pbMessage]        - Returns the registration response message
**              [f_pcbMessage]       - Returns the size (in bytes) of the registration response message
**
** Returns:
**  DRM_SUCCESS - The operation completed successfully
**  DRM_E_BUFFERTOOSMALL - The given buffer doesn't have enough space to contain the
**                         registration response message
**  DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackRegistrationResponseMessage(
    __in const                     DRM_ID             * const f_pSerialNumber,
    __in const                     DRM_RSA_PUBLIC_KEY * const f_pDevicePublicKey,
    __in const                     DRM_DWORD                  f_cbPubKeyModulus,
    __in const                     DRM_ID             * const f_pSessionId,
    __in const                     DRM_ID             * const f_pSeed,
    __in_bcount(f_cbAddress) const DRM_BYTE           * const f_pAddress,
    __in const                     DRM_WORD                   f_cbAddress,
    __out_bcount(*f_pcbMessage)    DRM_BYTE           * const f_pbMessage,
    __inout                        DRM_DWORD          * const f_pcbMessage
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor;
    DRM_DWORD dwMessageSize;
    DRM_DWORD dwSignOffset;
    DRM_DWORD dwBytesWritten;
    DRM_BYTE  rgbEncryptedSeed[__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) )] = {0};
    DRM_BYTE rgbSignature[__CB_DECL(WMDRMNET_SIGNATURE_AES_OMAC_LENGTH)];


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRegistrationResponseMessage );

    /*
    ** We currently only support a public key modulus size of 1024 bits
    */
    ChkArg( f_cbPubKeyModulus == DRM_RSA_CB_MODULUS(eDRM_RSA_1024) );

    ZEROMEM(rgbSignature, WMDRMNET_SIGNATURE_AES_OMAC_LENGTH);

    /*
     * Make sure the buffer can hold the registration response message.
     */
    ChkOverflow( WmdrmNetFixedMessageSize( WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE ) + f_cbAddress, f_cbAddress );
    ChkOverflow( WmdrmNetFixedMessageSize( WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE ) + f_cbAddress + f_cbPubKeyModulus, f_cbPubKeyModulus );
    ChkOverflow( WmdrmNetFixedMessageSize( WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE ) + f_cbAddress + f_cbPubKeyModulus + WMDRMNET_SIGNATURE_AES_OMAC_LENGTH, WMDRMNET_SIGNATURE_AES_OMAC_LENGTH );

    if ( (f_pbMessage == NULL)
      || ((WmdrmNetFixedMessageSize( WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE )
         + f_cbAddress + f_cbPubKeyModulus + WMDRMNET_SIGNATURE_AES_OMAC_LENGTH ) > *f_pcbMessage) )
    {
        *f_pcbMessage = WmdrmNetFixedMessageSize( WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE )
            + f_cbAddress + f_cbPubKeyModulus + WMDRMNET_SIGNATURE_AES_OMAC_LENGTH;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MessageDescriptor.Buffer     = f_pbMessage;
    MessageDescriptor.BufferSize = *f_pcbMessage;
    MessageDescriptor.Offset     = 0;

    /*
     * Pack the message vesion into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_VERSION_2 );

    /*
     * Pack the message type into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE );

    /*
     * Signature Offset: zero out this field for now and fill it later while signing
     */
    dwSignOffset = MessageDescriptor.Offset;
    WmdrmNetPackWord( &MessageDescriptor, 0 );

    /*
     * Pack the Serial Number into the message
     */
    _PackVal128( &MessageDescriptor, f_pSerialNumber );

    /*
     * Pack the Session ID into the message
     */
    _PackVal128( &MessageDescriptor, f_pSessionId );

    /*
     * Pack the Address into the message
     */
    WmdrmNetPackWord( &MessageDescriptor, f_cbAddress );
    _PackOctets( &MessageDescriptor, f_pAddress, f_cbAddress );

    /*
     * Encrypt and Copy Seed
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_ENCRYPT_RSA_OAEP );
    WmdrmNetPackWord( &MessageDescriptor, (DRM_WORD)f_cbPubKeyModulus );

    dr = NDT_EncryptSeed( f_pDevicePublicKey,
                          f_pSeed,
                          (DRM_WORD)f_cbPubKeyModulus,
                          rgbEncryptedSeed,
                          &dwBytesWritten );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Seed Encryption Failed %lx\n", dr ));
        goto ErrorExit;
    }
    if ( f_cbPubKeyModulus != dwBytesWritten )
    {
        TRACE(( "Unexpected Encrypted Seed length %u. Expected %u.\n", dwBytesWritten, f_cbPubKeyModulus ));
        dr = DRM_E_LOGICERR;
        goto ErrorExit;
    }

    DRM_BYT_CopyBytes( MessageDescriptor.Buffer, MessageDescriptor.Offset, rgbEncryptedSeed, 0, f_cbPubKeyModulus );

    dwMessageSize = MessageDescriptor.Offset + f_cbPubKeyModulus;

    /*
     * Set the offset for the sign.
     */
    MessageDescriptor.Offset = dwSignOffset;
    WmdrmNetPackWord( &MessageDescriptor, (DRM_WORD)dwMessageSize );

    MessageDescriptor.Offset = dwMessageSize;

    /*
     *  Write Signature Header
     */

    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_SIGNATURE_AES_OMAC_TYPE );
    WmdrmNetPackWord( &MessageDescriptor, WMDRMNET_SIGNATURE_AES_OMAC_LENGTH );

    /*
    * Compute signature and write to buffer. Set the offset for the sign.
    */
    ChkDR( NDT_SignRegistrationMessage( MessageDescriptor.Buffer,
                                        dwMessageSize,
                                        f_pSeed,
                                        WMDRMNET_SIGNATURE_AES_OMAC_LENGTH,
                                        rgbSignature,
                                        &dwBytesWritten ) );

    DRM_BYT_CopyBytes(MessageDescriptor.Buffer, MessageDescriptor.Offset, rgbSignature, 0, WMDRMNET_SIGNATURE_AES_OMAC_LENGTH);

    /*
     * Return to the caller
     */
    *f_pcbMessage = MessageDescriptor.Offset + WMDRMNET_SIGNATURE_AES_OMAC_LENGTH;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}




/**********************************************************************
**
** Function:  WmdrmNetPackRegistrationMessage
**
** Synopsis:  Builds a WMDRMNET registration message from its component parts.
**
** Arguments: [SerialNumber]          - Specifies the Serial Number to pack into the message
**            [DeviceCertificate]     - Specifies the device certificate to pack into the message
**            [DeviceCertificateSize] - Specifies the size (in bytes) of the Device Certificate
**            [Message]               - Returns the registration message
**            [MessageSize]           - Returns the size (in bytes) of the RegistrationMessage
**
** Returns:   DRM_SUCCESS       - The operation completed successfully
**            DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackRegistrationMessage(
    __in const                               DRM_ID   * const SerialNumber,
    __in_bcount(DeviceCertificateSize) const DRM_BYTE * const DeviceCertificate,
    __in const                               DRM_DWORD        DeviceCertificateSize,
    __deref_out_bcount_full(*MessageSize)    DRM_BYTE       **Message,
    __out DRM_DWORD                                   * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRegistrationMessage );

    /*
     * Allocate a buffer for the message
     */
    MessageDescriptor = _AllocateMessage( WMDRMNET_REGISTRATION_MESSAGE_TYPE,
                                          DeviceCertificateSize );  /* Additional space needed for the certificate */

    ChkMem( MessageDescriptor );

    /*
     * Pack the Serial Number into the message
     */
    _PackVal128( MessageDescriptor, SerialNumber );

    /* Pack the Certificate into the message */
    WmdrmNetPackDword( MessageDescriptor, DeviceCertificateSize );
    _PackOctets( MessageDescriptor, DeviceCertificate, DeviceCertificateSize );

    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
**
** Function:  WmdrmNetUnpackRegistrationResponseMessage
**
** Synopsis:  Unpacks a RegistrationResponseMessage into its component parts.
**            This routine assumes RSA_OEAP encryption of the seed.
**
** Arguments: [f_pPrivateKey]     - Specifies the RSA private key used to decrypt the seed in the message
**                                  NULL indicates that the private key baked into the device should be used
**            [f_pbMessage]       - Specifies the registration response message
**            [f_cbMessage]       - Specifies the size (in bytes) of the registration response message
**            [f_pidSeed]         - Returns the seed from the message
**            [f_pidSerialNumber] - Returns the Serial Number from the message
**            [f_pidSession]      - Returns the session ID from the message
**            [f_ppbAddress]      - Returns a pointer to the address field within the response message
**            [f_pcbAddress]      - Returns the size (in bytes) of the returned Address
**            [f_pCryptoCtx]      - Pointer to crypto context
**
** Returs:  DRM_SUCCESS - The operation completed successfully
**          DRM_E_ND_INVALID_MESSAGE
**          DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackRegistrationResponseMessage(
    __in const                        DRM_RSA_PRIVATE_KEY * const f_pPrivateKey,
    __in_bcount(f_cbMessage)          DRM_BYTE            * const f_pbMessage,
    __in const                        DRM_DWORD                   f_cbMessage,
    __out                             DRM_ID              * const f_pidSeed,
    __out                             DRM_ID              * const f_pidSerialNumber,
    __out                             DRM_ID              * const f_pidSession,
    __deref_out_bcount(*f_pcbAddress) DRM_BYTE                  **f_ppbAddress,
    __out                             DRM_WORD            * const f_pcbAddress,
    __in                              DRM_CRYPTO_CONTEXT         *f_pCryptoCtx
)
{
    DRM_RESULT   dr                 = DRM_SUCCESS;
    DRM_WORD     wSignatureOffset   = 0;
    DRM_BYTE     bEncryptionType    = 0;
    DRM_WORD     wEncryptedSeedSize = 0;
    DRM_BYTE    *pbEncryptedSeed    = NULL;
    DRM_BYTE     rgbPlainSeed[__CB_DECL( DRM_RSA_CB_MAX_PLAINTEXT( eDRM_RSA_1024 ) )] = {0};
    DRM_DWORD    cbPlainSeed        = SIZEOF(rgbPlainSeed);
    DRM_BYTE    *pbSignature        = NULL;
    DRM_BYTE     bSignatureType     = 0;
    DRM_WORD     wSignatureLength   = 0;
    DRM_AES_KEY *pAesKey            = NULL;
    WMDRMNET_MESSAGE_DESCRIPTOR *pMessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRegistrationResponseMessage );

    /*
     * Validate the message
     */
    dr = _ValidateMessage( f_pbMessage,
                           f_cbMessage,
                           WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE,
                           &pMessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate RegistrationResponseMessage %lx\n", dr ));
        goto ErrorExit;
    }

    /*
     * Unpack the Signature offset from the message
     */
    if ( !WmdrmNetUnpackWord( pMessageDescriptor, &wSignatureOffset ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Signature Offset)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the Serial Number from the message
     */
    if ( !WmdrmNetUnpackVal128( pMessageDescriptor, f_pidSerialNumber ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Serial Number)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the Session ID from the message
     */
    if ( !WmdrmNetUnpackVal128( pMessageDescriptor, f_pidSession ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Session ID)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the Address from the message
     */
    if ( !WmdrmNetUnpackWord( pMessageDescriptor, f_pcbAddress ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Address Size)\n" ));
        goto ErrorExit;
    }

    if( *f_pcbAddress == 0 )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage: Invalid Address Size\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctets( pMessageDescriptor, f_ppbAddress, *f_pcbAddress ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Address)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the encryption type and encrypted data size from the message
     */
    if ( !WmdrmNetUnpackOctet( pMessageDescriptor, &bEncryptionType ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Encryption Type)\n" ));
        goto ErrorExit;
    }

    if ( bEncryptionType != WMDRMNET_ENCRYPT_RSA_OAEP )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage encryption type not supported\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackWord( pMessageDescriptor, &wEncryptedSeedSize ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Encrypted Seed Size)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the encrypted seed
     *
     * Simply skip over the octets in the message buffer and decrypt directly from there
     */
    if ( !WmdrmNetUnpackOctets( pMessageDescriptor, &pbEncryptedSeed, wEncryptedSeedSize ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Encrypted Seed)\n" ));
        goto ErrorExit;
    }

    /*
     * Decrypt the seed from the message buffer
     */

    dr = OEM_RSA_OaepDecrypt( f_pPrivateKey,
                              wEncryptedSeedSize,
                              pbEncryptedSeed,
                              &cbPlainSeed,
                              rgbPlainSeed,
                              f_pCryptoCtx );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "RegistrationResponseMessage cannot decrypt seed %lx\n", dr ));
        goto ErrorExit;
    }

    /*
     * Copy the seed back to the caller
     */

    if ( cbPlainSeed < sizeof(*f_pidSeed) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage clear seed too short\n" ));
        goto ErrorExit;
    }

    MEMCPY( f_pidSeed, rgbPlainSeed, sizeof(*f_pidSeed) );

    /*
     * Jump to the signature section using the offset
     */
    if( !_MoveToOffset( pMessageDescriptor, wSignatureOffset ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Signature offset out of range)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the signature type
     */
    if ( !WmdrmNetUnpackOctet( pMessageDescriptor, &bSignatureType) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Signature Type)\n" ));
        goto ErrorExit;
    }
    if ( bSignatureType != WMDRMNET_SIGNATURE_AES_OMAC_TYPE )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage has invalid signature type\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the signature length
     */
    if ( !WmdrmNetUnpackWord( pMessageDescriptor, &wSignatureLength) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Signature Length)\n" ));
        goto ErrorExit;
    }
    if ( wSignatureLength != DRM_AES_BLOCKLEN )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage has invalid signature length\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the signature itself
     */
    if (!WmdrmNetUnpackOctets( pMessageDescriptor, &pbSignature, wSignatureLength ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage too short (Signature)\n" ));
        goto ErrorExit;
    }

    /* Compute the registration content integrity key to use for the signature */
    pAesKey = WmdrmNetGetRegistrationKey( f_pidSeed, TRUE );
    ChkMem( pAesKey );

    dr = Oem_Omac1_Verify( pAesKey,
                           pMessageDescriptor->Buffer,
                           0,
                           wSignatureOffset,
                           pbSignature,
                           0);

    if ( dr != DRM_SUCCESS )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "RegistrationResponseMessage invalid signature\n" ));
        goto ErrorExit;
    }

ErrorExit:

    if ( pMessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( pMessageDescriptor, (dr == DRM_SUCCESS) );
    }

    if ( pAesKey != NULL )
    {
        OEM_SECURE_ZERO_MEMORY( pAesKey, SIZEOF( DRM_AES_KEY ) );
    }

    SAFE_OEM_FREE( pbEncryptedSeed );
    SAFE_OEM_FREE( pbSignature );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackProximityStartMessage(
    __in_bcount(MessageSize) DRM_BYTE * const Message,
    __in const               DRM_DWORD        MessageSize,
    __out                    DRM_ID   * const SessionID
)
{
    DRM_RESULT dr;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityStartMessage );

    ChkArg( Message   != NULL );
    ChkArg( SessionID != NULL );

    /*
    ** Validate the message
    */
    dr = _ValidateMessage( Message,
                           MessageSize,
                           WMDRMNET_PROXIMITY_START_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate ProximityStartMessage %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Session ID from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, SessionID ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityStartMessage too short (Session ID)\n" ));
        goto ErrorExit;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetPackProximityChallengeMessage(
    __in const                       DRM_BYTE          bSequenceNumber,
    __in const                       DRM_ID    * const idSession,
    __in const                       DRM_ID    * const idChallenge,
    __deref_out_bcount(*MessageSize) DRM_BYTE        **Message,
    __out                            DRM_DWORD * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityChallengeMessage );

    ChkArg( idSession
            && Message
            && MessageSize );

    /*
     * Allocate a buffer for the message
     */
    MessageDescriptor = _AllocateMessage(WMDRMNET_PROXIMITY_CHALLENGE_MESSAGE_TYPE, 0);
    ChkMem( MessageDescriptor );

    /* Pack the Sequence Number */
    WmdrmNetPackOctet( MessageDescriptor, bSequenceNumber );

    /* Pack the SessionId */
    _PackVal128( MessageDescriptor, idSession );

    /* Pack the Challenge */
    _PackVal128( MessageDescriptor, idChallenge );

    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackProximityResponseMessage(
    __in_bcount(MessageSize) DRM_BYTE  * const Message,
    __in const               DRM_DWORD         MessageSize,
    __out                    DRM_BYTE  * const Sequence,
    __out                    DRM_ID    * const SessionID,
    __out                    DRM_ID    * const Nonce
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityResponseMessage );

    ChkArg( Message
            && Sequence
            && SessionID
            && Nonce );

    /*
    ** Validate the message
    */
    dr = _ValidateMessage( Message,
                           MessageSize,
                           WMDRMNET_PROXIMITY_RESPONSE_MESSAGE_TYPE,
                           &MessageDescriptor);

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate ProximityResponseMessage %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Sequence Number from the message
    */
    if ( !WmdrmNetUnpackOctet( MessageDescriptor, Sequence ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityResponseMessage too short (Sequence Number)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Session ID from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, SessionID ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityResponseMessage too short (Session ID)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Nonce from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, Nonce ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityResponseMessage too short (Nonce)\n" ));
        goto ErrorExit;
    }

ErrorExit:
    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetPackProximityResultMessage(
    __in const                       DRM_ID    * const SessionID,
    __in const                       DRM_WORD          Result,
    __deref_out_bcount(*MessageSize) DRM_BYTE        **Message,
    __out                            DRM_DWORD * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityResultMessage );

    ChkArg( SessionID
            && Message
            && MessageSize );

    /*
     * Allocate a buffer for the message
     */
    MessageDescriptor = _AllocateMessage(WMDRMNET_PROXIMITY_RESULT_MESSAGE_TYPE, 0);
    ChkMem( MessageDescriptor );

    /* Pack the SessionId */
    _PackVal128( MessageDescriptor, SessionID );

    /* Pack the Result */
    WmdrmNetPackWord( MessageDescriptor,Result );

    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:  WmdrmNetPackProximityResponseMessage
**
** Synopsis:  Builds the proximity detection response messge to return to the transmitter.
**
** Arguments: [pAesKey]     - Specifies the content encryption key computed from the registration response message
**            [Sequence]    - Specifies the sequence number from the proximity challenge message
**            [SessionId]   - Specifies the SessionId from the registration response message
**            [Challenge]   - Specifies the challege from the proximity challenge message
**            [Message]     - Returns the message
**            [MessageSize] - Returns the size (in bytes) of the Message
**
** Returns:   DRM_SUCCESS - The operation completed successfully
**            DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackProximityResponseMessage(
    __in const                       DRM_AES_KEY * const pAesKey,
    __in const                       DRM_BYTE            Sequence,
    __in const                       DRM_ID      * const SessionId,
    __in const                       DRM_ID      * const Challenge,
    __deref_out_bcount(*MessageSize) DRM_BYTE          **Message,
    __out                            DRM_DWORD   * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID EncryptedChallenge;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityResponseMessage );

    /*
     * Allocate a buffer for the return message
     */
    MessageDescriptor = _AllocateMessage( WMDRMNET_PROXIMITY_RESPONSE_MESSAGE_TYPE,
                                          DRM_AES_BLOCKLEN ); /* size of the encrypted challenge */

    ChkMem( MessageDescriptor );

    /*
     * Copy the sequence number into the message
     */
    WmdrmNetPackOctet( MessageDescriptor, Sequence );

    /*
     * Copy the Session ID into the message
     */
    _PackVal128( MessageDescriptor, SessionId );

    /*
     * Compute the encrypted challenge value
     */
    DRMSIZEASSERT( SIZEOF(EncryptedChallenge), DRM_AES_BLOCKLEN );
    DRMSIZEASSERT( SIZEOF(*Challenge), DRM_AES_BLOCKLEN );

    Oem_Aes_EncryptOne( pAesKey, (DRM_BYTE *)Challenge );

    /*
     * Pack the encrypted challenge into the message
     */
    _PackVal128( MessageDescriptor, Challenge );


    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    /*
     * Free locally used resources
     */
    if ( MessageDescriptor != NULL )
    {
        if ( MessageDescriptor->Buffer != NULL )
        {
            Oem_MemFree( MessageDescriptor->Buffer );
        }
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, dr == DRM_SUCCESS );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetUnpackProximityChallengeMessage
**
** Synopsis:    Unpacks a ProximityChallenge Message into its component parts.
**
** Arguments:   [Message]     - Specifies the proximity challenge message
**              [MessageSize] - Specifies the size (in bytes) of the proximity challenge message
**              [Sequence]    - Returns the sequence number from the message
**              [SessionId]   - Returns the session ID from the message
**              [Challenge]   - Returns the challenge from the message
**
** Returns:     DRM_SUCCESS - The operation completed successfully
**              DRM_E_ND_INVALID_MESSAGE
**              DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackProximityChallengeMessage(
    __in_bcount(MessageSize) DRM_BYTE  * const Message,
    __in const               DRM_DWORD         MessageSize,
    __out                    DRM_BYTE  * const Sequence,
    __out                    DRM_ID    * const SessionId,
    __out                    DRM_ID    * const Challenge
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityChallengeMessage );

    /*
     * Validate the message
     */
    dr = _ValidateMessage( Message,
                           MessageSize,
                           WMDRMNET_PROXIMITY_CHALLENGE_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate ProximityChallenge Message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
     * Unpack the sequence number from the message
     */
    if ( !WmdrmNetUnpackOctet( MessageDescriptor, Sequence) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityChallenge message too short (Sequence)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the session ID from the message
     */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, SessionId) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityStart message too short (Session ID)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the challenge from the message
     */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, Challenge) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityStart message too short (Challenge)\n" ));
        goto ErrorExit;
    }

ErrorExit:
    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetUnpackProximityResultMessage
**
** Synopsis:    Unpacks a ProximityResult Message into its component parts.
**
** Arguments:   [Message]     - Specifies the proximity result message
**              [MessageSize] - Specifies the size (in bytes) of the proximity result message
**              [SessionId]   - Returns the session ID from the message
**              [Result]      - Returns the result from the message
**
** Returns:     DRM_SUCCESS - The operation completed successfully
**              DRM_E_ND_INVALID_MESSAGE
**              DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackProximityResultMessage(
    __in_bcount(MessageSize) DRM_BYTE  * const Message,
    __in const               DRM_DWORD         MessageSize,
    __out                    DRM_ID    * const SessionId,
    __out                    DRM_WORD  * const Result
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityResultMessage );

    /*
     * Validate the message
     */
    dr = _ValidateMessage( Message,
                           MessageSize,
                           WMDRMNET_PROXIMITY_RESULT_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate ProximityResult Message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
     * Unpack the session ID from the message
     */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, SessionId) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityResult message too short (Session ID)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the result from the message
     */
    if ( !WmdrmNetUnpackWord( MessageDescriptor, Result) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "ProximityResult message too short (Result)\n" ));
        goto ErrorExit;
    }

ErrorExit:
    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetPackProximityStartMessage
**
** Synopsis:    Builds a proximity start message to be sent to the transmitter.
**
** Arguments:   [SessionId]   - Specifies the SessionId of the session to start proximity detection on
**              [Message]     - Returns the message
**              [MessageSize] - Returns the size (in bytes) of the Message
**
** Returns:     DRM_SUCCESS - The operation completed successfully
**              DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackProximityStartMessage(
    __in const                       DRM_ID     * const SessionId,
    __deref_out_bcount(*MessageSize) DRM_BYTE         **Message,
    __out                            DRM_DWORD  * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityStartMessage );

    /*
     * Allocate a buffer for the message
     */
    MessageDescriptor = _AllocateMessage( WMDRMNET_PROXIMITY_START_MESSAGE_TYPE,
                                          0 );        /* There is no variable size data */
    ChkMem( MessageDescriptor );

    /*
     * Pack the session ID into the message
     */
    _PackVal128( MessageDescriptor, SessionId );

    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL ) {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetUnpackLicenseRequestMessage
**
** Synopsis:    Parses a WMDRM-ND License request message from its component parts.
**
** Arguments:   [f_pbMessage]    - Specifies the License request message to unpack
**                                  The returned buffer must be freed using Oem_MemFree.
**              [f_cbMessage]     - Returns the size (in bytes) of the License request message
**              [f_pRightsId]     - Returns the Rights ID unpacked the message
**              [f_pSerialNumber] - Returns the Serial Number unpacked from the message
**              [f_ppbDeviceCertificate] - Returns the device certificate unpacked from the message
**                                         must be freed by caller
**              [f_pcbDeviceCertificate] - Returns the size (in bytes) of the Device Certificate
**              [f_ppbAction]     - Returns the action unpacked from the message (not null-terminated)
**                                  must be freed by caller
**              [f_pcbAction]     - Returns the size (in bytes) of the action
**              [f_pdwCRLVersion] - Returns the CRL Version
**
** Returns:
**  DRM_SUCCESS - The operation completed successfully
**  DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**  DRM_E_ND_INVALID_MESSAGE
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackLicenseRequestMessage(
    __in_bcount(f_cbMessage)         DRM_BYTE  * const f_pbMessage,
    __in const                       DRM_DWORD         f_cbMessage,
    __out                            DRM_ID    * const f_pRightsId,
    __out                            DRM_ID    * const f_pSerialNumber,
    __deref_out_bcount(*f_pcbDeviceCertificate) DRM_BYTE **f_ppbDeviceCertificate,
    __out                            DRM_DWORD * const f_pcbDeviceCertificate,
    __deref_out_bcount(*f_pcbAction) DRM_BYTE        **f_ppbAction,
    __out                            DRM_WORD  * const f_pcbAction,
    __out                            DRM_DWORD * const f_pdwCRLVersion
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackLicenseRequestMessage );

    ChkArg( f_pbMessage && f_pRightsId && f_pSerialNumber );
    ChkArg( f_ppbDeviceCertificate && f_pcbDeviceCertificate );
    ChkArg( f_ppbAction && f_pcbAction && f_pdwCRLVersion );

    *f_ppbDeviceCertificate = NULL;
    *f_ppbAction = NULL;

    TRACE(( "WmdrmNetUnpackLicenseRequestMessage\n" ));

    /*
    ** Validate the message
    */
    dr = _ValidateMessage( f_pbMessage,
                           f_cbMessage,
                           WMDRMNET_LICENSE_REQUEST_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate License Request Message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Rights ID from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, f_pRightsId ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (Rights Id)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the CRL Version from the message
    */
    if( !WmdrmNetUnpackDword( MessageDescriptor, f_pdwCRLVersion ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (CRL Version)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Serial Number from the message
    */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, f_pSerialNumber ))
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (Serial Number)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Device Certificate size from the message
    */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, f_pcbDeviceCertificate ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (DeviceCertificateSize Unpack Failed)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Device Certificate from the message
    */
    if ( 0 == *f_pcbDeviceCertificate )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (DeviceCertificateSize = 0)\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctets( MessageDescriptor, f_ppbDeviceCertificate, *f_pcbDeviceCertificate ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Request Message too short (DeviceCertificate Unpack Failed)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Action size from the message
    */
    if ( !WmdrmNetUnpackWord( MessageDescriptor, f_pcbAction ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Licnese Request Message too short (ActionSize Unpack Failed)\n" ));
        goto ErrorExit;
    }

    /*
    ** Unpack the Action from the message
    */
    if ( 0 == *f_pcbAction )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Licnese Request Message too short (ActionSize = 0)\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctets( MessageDescriptor, f_ppbAction, *f_pcbAction ) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Licnese Request Message too short (Action Unpack Failed)\n" ));
        goto ErrorExit;
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( *f_ppbDeviceCertificate );
        SAFE_OEM_FREE( *f_ppbAction );
    }

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    WmdrmNetPackLicenseResponseMessage
**
** Synopsis:    Packs a WMDRM-ND License Response Message into its component parts.
**
** Arguments:   [f_pbLicense] - Specifies a pointer to the License within the message
**              [f_cbLicense] - Specifies the size (in bytes) of the License
**              [f_pbCRLBuffer] - Specifies a pointer the CRL Buffer
**              [f_cbCRLBuffer] - Specifies the size (in bytes) of the CRL Buffer
**              [f_ppbMessage] - Returns the License response message
**              [f_pcbMessage] - Returns the size (in bytes) of the License response message
**
** Returns:
**  DRM_SUCCESS - The operation completed successfully
**  DRM_E_ND_INVALID_MESSAGE
**  DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackLicenseResponseMessage(
    __in_bcount(f_cbLicense)   const  DRM_BYTE  * const f_pbLicense,
    __in                       const  DRM_DWORD         f_cbLicense,
    __in_bcount(f_cbCRLBuffer) const  DRM_BYTE  * const f_pbCRLBuffer,
    __in const                        DRM_DWORD         f_cbCRLBuffer,
    __deref_out_bcount(*f_pcbMessage) DRM_BYTE        **f_ppbMessage,
    __out                             DRM_DWORD * const f_pcbMessage
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackLicenseResponseMessage );

    ChkArg( f_pbLicense && f_ppbMessage && f_pcbMessage );

    if( f_cbCRLBuffer != 0 )
    {
        ChkArg( f_pbCRLBuffer );
    }

    /*
     * Allocate a buffer for the message
     */
    ChkOverflow( f_cbLicense + f_cbCRLBuffer, f_cbLicense );

    MessageDescriptor = _AllocateMessage( WMDRMNET_LICENSE_RESPONSE_MESSAGE_TYPE,
                                          f_cbLicense + f_cbCRLBuffer ); /* Additional space needed for the license */
    ChkMem( MessageDescriptor );

    /* Pack the CRL Length */
    WmdrmNetPackDword( MessageDescriptor, f_cbCRLBuffer );
    _PackOctets( MessageDescriptor, f_pbCRLBuffer, f_cbCRLBuffer );

    /* Pack the License into the message */
    WmdrmNetPackDword( MessageDescriptor, f_cbLicense );
    _PackOctets( MessageDescriptor, f_pbLicense, f_cbLicense );

    /*
     * Return to the caller
     */
    *f_ppbMessage = MessageDescriptor->Buffer;
    *f_pcbMessage = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:  WmdrmNetPackLicenseRequestMessage
**
** Synopsis:  Builds a WMDRMNET License request message from its component parts.
**
** Arguments: [RightsId]              - Specifies the Rights ID to pack into the message
**            [SerialNumber]          - Specifies the Serial Number to pack into the message
**            [DeviceCertificate]     - Specifies the device certificate to pack into the message
**            [DeviceCertificateSize] - Specifies the size (in bytes) of the Device Certificate
**            [Action]                - Specifies the action to pack into the message
**            [ActionSize]            - Specifies the size (in bytes) of the action
**            [Message]               - Returns the License request message
**            [MessageSize]           - Returns the size (in bytes) of the License request message
**
** Returns:   DRM_SUCCESS - The operation completed successfully
**            DRM_E_OUTOFMEMORY - There was not enough memory to perform the operation
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetPackLicenseRequestMessage(
    __in const                               DRM_ID    * const RightsId,
    __in const                               DRM_ID    * const SerialNumber,
    __in_bcount(DeviceCertificateSize) const DRM_BYTE  * const DeviceCertificate,
    __in const                               DRM_DWORD         DeviceCertificateSize,
    __in_bcount(ActionSize) const            DRM_BYTE  * const Action,
    __in const                               DRM_WORD          ActionSize,
    __deref_out_bcount(*MessageSize)         DRM_BYTE        **Message,
    __out                                    DRM_DWORD * const MessageSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackLicenseRequestMessage );

    /*
     * Allocate a buffer for the message
     */
    ChkOverflow(DeviceCertificateSize + ActionSize, DeviceCertificateSize);
    MessageDescriptor = _AllocateMessage( WMDRMNET_LICENSE_REQUEST_MESSAGE_TYPE,
                                          DeviceCertificateSize + ActionSize );  /* Additional space needed for the certificate and action */
    ChkMem( MessageDescriptor );

    /*
     * Pack the Rights ID into the message
     */
    _PackVal128( MessageDescriptor, RightsId );

    /* Pack the Reserved field as 0x0 into the message */
    WmdrmNetPackDword( MessageDescriptor, 0 );

    /*
     * Pack the Serial Number into the message
     */
    _PackVal128( MessageDescriptor, SerialNumber );

    /* Pack the Certificate into the message */
    WmdrmNetPackDword( MessageDescriptor, DeviceCertificateSize );
    _PackOctets( MessageDescriptor, DeviceCertificate, DeviceCertificateSize );

    /* Pack the Action into the message */
    WmdrmNetPackWord( MessageDescriptor, ActionSize );
    _PackOctets( MessageDescriptor, Action, ActionSize );

    /*
     * Return to the caller
     */
    *Message = MessageDescriptor->Buffer;
    *MessageSize = MessageDescriptor->BufferSize;
    MessageDescriptor->Buffer = NULL;

ErrorExit:

    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:  WmdrmNetUnpackLicenseResponseMessage
**
** Synopsis:  Unpacks a License Response Message into its component parts.
**
** Arguments: [Message]     - Specifies the License response message
**            [MessageSize] - Specifies the size (in bytes) of the License response message
**            [ppbLicense]  - Returns a pointer to the License within the message
**            [cbLicense]   - Returns the size (in bytes) of the License
**
** Returns:   DRM_SUCCESS - The operation completed successfully
**            DRM_E_ND_INVALID_MESSAGE
**            DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackLicenseResponseMessage(
    __in_bcount(MessageSize)       DRM_BYTE  * const Message,
    __in const                     DRM_DWORD         MessageSize,
    __deref_out_bcount(*cbLicense) DRM_BYTE        **ppbLicense,
    __out                          DRM_DWORD * const cbLicense
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbReserved;
    DRM_BYTE *pbReserved;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackLicenseResponseMessage );

    /*
     * Validate the message
     */
    dr = _ValidateMessage( Message,
                           MessageSize,
                           WMDRMNET_LICENSE_RESPONSE_MESSAGE_TYPE,
                           &MessageDescriptor );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Cannot validate License Response Message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
     * Unpack the Reserved field size from the message
     */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, &cbReserved) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Response message too short (Reserved field size)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the reserved field from the message
     */
    if ( cbReserved != 0 )
    {
       if ( !WmdrmNetUnpackOctets( MessageDescriptor, &pbReserved, cbReserved) )
       {
           dr = DRM_E_ND_INVALID_MESSAGE;
           TRACE(( "License Response message too short (Reserved field)\n" ));
           goto ErrorExit;
       }
    }

    /*
     * Unpack the License size from the message
     */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, cbLicense) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Response message too short (License size)\n" ));
        goto ErrorExit;
    }

    /*
     * Unpack the License from the message
     */

    if ( *cbLicense == 0 )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Response message too short (LicenseSize)\n" ));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctets( MessageDescriptor, ppbLicense, *cbLicense) )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "License Response message too short (License)\n" ));
        goto ErrorExit;
    }

ErrorExit:
    if ( MessageDescriptor != NULL )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}




DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackRevocationListRequestMessage(
    __in_bcount(f_cbMessage)                      DRM_BYTE  * const f_pbMessage,
    __in const                                    DRM_DWORD         f_cbMessage,
    __out                                         DRM_DWORD * const f_pcguidsRevocationLists,
    __deref_out_ecount(*f_pcguidsRevocationLists) DRM_GUID        **f_ppguidsRevocationLists
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE cEntries = 0;
    DRM_GUID *pguidsRevocationLists = NULL;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRevocationListRequestMessage );

    ChkArg( f_pbMessage != NULL &&
            f_pcguidsRevocationLists != NULL &&
            f_ppguidsRevocationLists != NULL );

    dr = _ValidateMessage(f_pbMessage,
                          f_cbMessage,
                          WMDRMNET_REVOCATION_LIST_REQUEST_MESSAGE_TYPE,
                          &MessageDescriptor);
    if(dr != DRM_SUCCESS)
    {
        TRACE(("Cannot validate RevocationList Request Message %lx\n", dr));
        goto ErrorExit;
    }

    if ( !WmdrmNetUnpackOctet( MessageDescriptor, &cEntries ) )
    {
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }

    if ( !WmdrmNetUnpackOctets( MessageDescriptor,
                                (DRM_BYTE **)&pguidsRevocationLists,
                                cEntries * SIZEOF(DRM_GUID)) )
    {
        ChkDR( DRM_E_ND_INVALID_MESSAGE );
    }

    *f_ppguidsRevocationLists = pguidsRevocationLists;
    *f_pcguidsRevocationLists = cEntries;
    pguidsRevocationLists = NULL;

ErrorExit:
    SAFE_OEM_FREE(pguidsRevocationLists);
    if( NULL != MessageDescriptor )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, !DRM_FAILED(dr) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetPackRevocationListResponseMessage(
    __in const                            DRM_DWORD            f_cRevocationLists,
    __in_ecount(f_cRevocationLists) const DRM_RVK_LIST * const f_pRevocationLists,
    __inout_bcount(*f_pcbMessage)         DRM_BYTE     * const f_pbMessage,
    __inout                               DRM_DWORD    * const f_pcbMessage
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor;
    DRM_DWORD i;
    DRM_DWORD cbRevocationLists = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRevocationListResponseMessage );

    ChkArg( (f_cRevocationLists == 0 || f_pRevocationLists != NULL) &&
            f_pcbMessage &&
            (!*f_pcbMessage || f_pbMessage) );

    for(i = 0; i < f_cRevocationLists; i++)
    {
        cbRevocationLists += SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ) +
                             f_pRevocationLists[i].cbRevocationList;
    }

    if( WmdrmNetFixedMessageSize( WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE ) + cbRevocationLists > *f_pcbMessage )
    {
        *f_pcbMessage = WmdrmNetFixedMessageSize( WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE ) + cbRevocationLists;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MessageDescriptor.Buffer     = f_pbMessage;
    MessageDescriptor.BufferSize = *f_pcbMessage;
    MessageDescriptor.Offset     = 0;

    /*
     * Pack the message vesion into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_VERSION_2 );

    /*
     * Pack the message type into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE );


    for(i = 0; i < f_cRevocationLists; i++)
    {
        _PackVal128( &MessageDescriptor, (DRM_ID *)&f_pRevocationLists[i].guidRevocationList );
        WmdrmNetPackDword( &MessageDescriptor, f_pRevocationLists[i].cbRevocationList );
        _PackOctets( &MessageDescriptor, f_pRevocationLists[i].pbRevocationList, f_pRevocationLists[i].cbRevocationList );
    }

    *f_pcbMessage = MessageDescriptor.Offset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetPackRevocationListRequestMessage(
    __in const                                 DRM_DWORD         f_cguidsRevocationLists,
    __in_ecount(f_cguidsRevocationLists) const DRM_GUID  * const f_pguidsRevocationLists,
    __inout_bcount_opt(*f_pcbMessage)          DRM_BYTE  * const f_pbMessage,
    __inout                                    DRM_DWORD * const f_pcbMessage
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor;
    DRM_DWORD i;
    DRM_BYTE cRequest = (DRM_BYTE) f_cguidsRevocationLists;
    DRM_DWORD cbRevocationListRequest = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRevocationListRequestMessage );

    ChkArg( ( ( f_cguidsRevocationLists == 0    ) ||
            ( f_pguidsRevocationLists   != NULL ) ) &&
            ( f_pcbMessage              != NULL )   &&
            ( ( *f_pcbMessage           == 0    ) ||
            ( f_pbMessage               != NULL ) ) );

    cbRevocationListRequest =
      WmdrmNetFixedMessageSize(WMDRMNET_REVOCATION_LIST_REQUEST_MESSAGE_TYPE) +
      SIZEOF_U8 + f_cguidsRevocationLists * DRM_ID_SIZE;
    if( cbRevocationListRequest  > *f_pcbMessage )
    {
        *f_pcbMessage = cbRevocationListRequest;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MessageDescriptor.Buffer = f_pbMessage;
    MessageDescriptor.BufferSize = *f_pcbMessage;
    MessageDescriptor.Offset = 0;

    /*
     * Pack the message vesion into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_VERSION_2 );

    /*
     * Pack the message type into the message
     */
    WmdrmNetPackOctet( &MessageDescriptor, WMDRMNET_REVOCATION_LIST_REQUEST_MESSAGE_TYPE );

    WmdrmNetPackOctet( &MessageDescriptor, cRequest );
    for(i = 0; i < f_cguidsRevocationLists; i++)
    {
        _PackVal128( &MessageDescriptor, (DRM_ID *)&f_pguidsRevocationLists[i] );
    }

    *f_pcbMessage = MessageDescriptor.Offset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL WmdrmNetUnpackRevocationListResponseMessage(
    __in_bcount(f_cbMessage)                 DRM_BYTE     * const f_pbMessage,
    __in const                               DRM_DWORD            f_cbMessage,
    __inout                                  DRM_DWORD    * const f_pcRevocationLists,
    __inout_ecount_opt(*f_pcRevocationLists) DRM_RVK_LIST * const f_pRevocationLists
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;
    DRM_DWORD cbRevocationList = 0;
    DRM_DWORD cRevocationLists = 0;
    DRM_DWORD i = 0;
    DRM_DWORD dwSaveOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRevocationListResponseMessage );

    ChkArg( f_pbMessage != NULL &&
            f_pcRevocationLists != NULL &&
            ( *f_pcRevocationLists == 0 || f_pRevocationLists != NULL ) );

    dr = _ValidateMessage( f_pbMessage,
                           f_cbMessage,
                           WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE,
                           &MessageDescriptor );
    if(dr != DRM_SUCCESS)
    {
        TRACE(("Cannot validate RevocationList Response Message %lx\n", dr));
        goto ErrorExit;
    }

    dwSaveOffset = MessageDescriptor->Offset;
    while( MessageDescriptor->Offset < MessageDescriptor->BufferSize )
    {
        if( ! _MoveToOffset( MessageDescriptor,
                                    MessageDescriptor->Offset + DRM_ID_SIZE ) )
        {
            TRACE(("Revocation List Response Message too short (GUID)\n"));
            ChkDR( DRM_E_ND_INVALID_MESSAGE );
        }
        if ( ! WmdrmNetUnpackDword( MessageDescriptor, &cbRevocationList ) )
        {
            TRACE(("Revocation List Response Message too short (revocation list size)\n"));
            ChkDR( DRM_E_ND_INVALID_MESSAGE );
        }
        if(!_MoveToOffset(MessageDescriptor,
                                 MessageDescriptor->Offset + cbRevocationList))
        {
            TRACE(("Revocation List Response Message too short (Revocation List)\n"));
            ChkDR( DRM_E_ND_INVALID_MESSAGE );
        }
        cRevocationLists++;
    }

    ChkBOOL( cRevocationLists <= *f_pcRevocationLists, DRM_E_BUFFERTOOSMALL );

    ZEROMEM( f_pRevocationLists, SIZEOF( DRM_RVK_LIST ) * cRevocationLists );

    _MoveToOffset(MessageDescriptor, dwSaveOffset);

    for(i = 0; i < cRevocationLists; i++)
    {
        WmdrmNetUnpackVal128( MessageDescriptor,
                              (DRM_ID *)&f_pRevocationLists[i].guidRevocationList );
        WmdrmNetUnpackDword( MessageDescriptor,
                             &f_pRevocationLists[i].cbRevocationList );
        if(! WmdrmNetUnpackOctets( MessageDescriptor,
                                   &f_pRevocationLists[i].pbRevocationList,
                                   f_pRevocationLists[i].cbRevocationList ) )
        {
            ChkDR( DRM_E_OUTOFMEMORY );
        }
    }

ErrorExit:
    if(DRM_FAILED(dr) && f_pRevocationLists != NULL)
    {
        for(i = 0; (i < cRevocationLists) && (cRevocationLists <= *f_pcRevocationLists); i++)
        {
            SAFE_OEM_FREE( f_pRevocationLists[i].pbRevocationList );
        }
    }
    if ( dr == DRM_E_BUFFERTOOSMALL && NULL != f_pcRevocationLists )
    {
        *f_pcRevocationLists = cRevocationLists;
    }
    if( NULL != MessageDescriptor )
    {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, !DRM_FAILED(dr) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

