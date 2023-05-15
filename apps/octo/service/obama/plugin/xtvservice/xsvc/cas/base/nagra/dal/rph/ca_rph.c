/**************************************************************************************************/
#define ________CA_RPH_Private_Include_____________________________________________________
/**************************************************************************************************/
#include "na_glob.h"


/**************************************************************************************************/
#define ________CA_RPH_Golbal_Value________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________CA_RHP_Private_Define_______________________________________________________
/**************************************************************************************************/






/**************************************************************************************************/
#define ________CA_RPH_Private_Type_Define_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_RPH_Private_Static_Value_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_RPH_Private_Static_Prototype______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_RPH_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_RPH_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
/**
 *  @brief
 *    This function attempts to make a connection with a remote computer. It is
 *    asynchronous and must not block the calling task. This caters for systems
 *    where the connection is only available at discrete times, for instance
 *    when the set-top box connects through a PC-controlled modem that can only
 *    be used when the user of the PC has established a connection to the
 *    Internet.
 *
 *    To simplify the transition from the older synchronous \c rphConnect(), it
 *    is admitted that \c rphConnect() attempts to establish the connection
 *    synchronously and calls the callback from within the function's body,
 *    provided that the caller is not delayed for more than 30 seconds. New
 *    implementations must perform the connection asynchronously, i.e. it must
 *    return immediately and attempt to connect in background. As soon as the
 *    connection is established, data might be received, even if \c rphReceive()
 *    has not been called yet. These must not be lost and at least \c
 *    xInternalBufferSize bytes must be stored in an internal FIFO buffer. These
 *    will be returned to the forthcoming call to \c rphReceive(). Once this
 *    buffer is filled, it is acceptable that some bytes get lost.
 *
 *    Successful connection requests must be released by calling \c
 *    rphDisconnect(), even if the connection is not actually established.
 *
 *  @param[out] pxSessionHandle
 *                Pointer to the session handle; returned value is significant
 *                only when the function returns \c RPH_NO_ERROR
 *
 *  @param[in]  pxConnection
 *                Pointer to the descriptor of the connection
 *
 *  @param[in]  xInternalBufferSize
 *                Minimum internal receive buffer size in bytes that must be
 *                guarantied by the RPH driver
 *
 *  @param[in]  xConnectionCallback
 *                Callback used by the RPH to notify the CA software of the result
 *                of the connection attempt. It is called only if the function
 *                returns \c RPH_NO_ERROR.
 *
 *  @param[in]  pxPrivateData
 *                Private data used by the application for internal processing.
 *                This pointer is passed as a parameter to the callback function.
 *
 *  @retval RPH_NO_ERROR
 *            Connection succeded. In this case, the callback \c
 *            xConnectionCallback must be called.
 *
 *  @retval RPH_ERROR_LOW_MEMORY
 *            Memory allocation failed
 *
 *  @retval RPH_ERROR_BAD_PARAMETER
 *            Some parameter has an unsupported value
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *    -# If the function is called in PPP mode and another PPP connection type
 *       is already established to another PPP phone number, the function must
 *       call the \c xConnectionCallback callback with \c
 *       RPH_CONNECTION_LOCAL_FAILURE status.
 *    .
 *    -# For an IP connection, the IP stack already provides a reception buffer
 *       so that it is not necessary for the driver to manage its own internal
 *       buffer.
*/
TRphStatus rphConnect
(
        TRphSessionHandle*     pxSessionHandle,
        TRphConnection*        pxConnection,
        TSize                  xInternalBufferSize,
        TRphConnectionCallback xConnectionCallback,
  const void*                  pxPrivateData
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function attempts to make a connection with a remote computer. It is
 *    similar to the function \c rphConnect(), with an additional parameter: a
 *    callback that may be used to asynchronously receive data from the RPH.
 *
 *    Refer to the description of \c rphConnect() for further explanations about
 *    this function.
 *
 *  @param[out] pxSessionHandle
 *                Pointer to the session handle; returned value is significant
                  only when the function returns \c RPH_NO_ERROR
 *
 *  @param[in]  pxConnection
 *                Descriptor of the connection
 *
 *  @param[in]  xInternalBufferSize
 *                Minimum internal receive buffer size in bytes that must be
 *                guarantied by the RPH driver
 *
 *  @param[in]  xConnectionCallback
 *                Callback used by the RPH to notify the CA software of the result
 *                of the connection attempt. It is called only if the function
 *                returns \c RPH_NO_ERROR.
 *
 *  @param[in]  xReceptionCallback
 *                Callback used by the CA software to asynchronously receive
 *                data from the RPH through the function \c rphReceiveA(). If
 *                set to NULL, the function \c rphReceiveA() shall return an
 *                error and only the synchronous reception shall be used.
 *
 *  @param[in]  pxPrivateData
 *                Private data used by the application for internal processing.
 *                This pointer is passed as a parameter to the callback
 *                function.
 *
 *  @retval RPH_NO_ERROR
 *            Success. In this case, the callback \c xConnectionCallback must be
 *            called.
 *
 *  @retval RPH_ERROR_LOW_MEMORY
 *            Memory allocation failed
 *
 *  @retval RPH_ERROR_BAD_PARAMETER
 *            Some parameter has an unsupported value
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *    -# If the function is called in PPP mode and another PPP connection type
 *       is already established to another PPP phone number, the function must
 *       call the \c xConnectionCallback callback with \c
 *       RPH_CONNECTION_LOCAL_FAILURE status.
 *    .
 *    -# For an IP connection, the IP stack already provides a reception buffer
 *       so that it is not necessary for the driver to manage its own internal
 *       buffer.
 */
TRphStatus rphConnectA
(
        TRphSessionHandle*     pxSessionHandle,
        TRphConnection*        pxConnection,
        TSize                   xInternalBufferSize,
        TRphConnectionCallback  xConnectionCallback,
        TRphReceptionCallback   xReceptionCallback,
  const void*                  pxPrivateData
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function attempts to make a connection with a remote computer. It is
 *    similar to the function \c rphConnect(), with an additional parameter: a
 *    callback that may be used to asynchronously send data from the RPH.
 *
 *    Refer to the description of \c rphConnect() for further explanations about
 *    this function.
 *
 *  @param[out] pxSessionHandle
 *                Pointer to the session handle; returned value is significant
 *                only when the function returns \c RPH_NO_ERROR.
 *
 *  @param[in]  pxConnection
 *                Descriptor of the connection
 *
 *  @param[in]  xInternalBufferSize
 *                Minimum internal receive buffer size in bytes that must be
 *                guarantied by the RPH driver
 *
 *  @param[in]  xConnectionCallback
 *                Callback used by the RPH to notify the CA software of the result
 *                of the connection attempt. It is called only if the function
 *                returns \c RPH_NO_ERROR.
 *
 *  @param[in]  xSendCallback
 *                Callback used by the CA software to asynchronously send data
 *                from the RPH through the function \c rphSendA(). If set
 *                to NULL, the function \c rphSendA() shall return an error and
 *                only the synchronous send shall be used.
 *
 *  @param[in]  pxPrivateData
 *                Private data used by the application for internal processing.
 *                This pointer is passed as a parameter to the callback function.
 *
 *  @retval RPH_NO_ERROR
 *            Connection succeded
 *
 *  @retval RPH_ERROR_LOW_MEMORY
 *            Memory allocation failed
 *
 *  @retval RPH_ERROR_BAD_PARAMETER
 *            Some parameter has an unsupported value
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *    -# If the function is called in PPP mode and another PPP connection type
 *       is already established to another PPP phone number, the function must
 *       call the \c xConnectionCallback callback with \c
 *       RPH_CONNECTION_LOCAL_FAILURE status.
 *    .
 *    -# For an IP connection, the IP stack already provides a reception buffer
 *       so that it is not necessary for the driver to manage its own internal
 *       buffer.
 */
TRphStatus rphConnectAA
(
        TRphSessionHandle*       pxSessionHandle,
        TRphConnection*          pxConnection,
        TSize                     xInternalBufferSize,
        TRphConnectionCallback    xConnectionCallback,
        TRphSendCallback          xSendCallback,
  const void*                    pxPrivateData
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function closes the connection with the remote computer (if any) and
 *    cancels the connection request. It must be called for all successful calls
 *    to \c rphConnect() or \c rphConnectA(), whether the connection was
 *    actually established or not.
 *
 *    The callback given in \c rphConnect() or \c rphConnectA() shall no longer
 *    be called after \c rphDisconnect() is completed.
 *
 *    If this function is called during an asynchronous data reception, the RPH
 *    shall call the reception callback with the reception status set to \c
 *    RPH_RECEPTION_DISCONNECTED.
 *
 *  @param[in] xSessionHandle
 *               Handle to the session to disconnect
 *
 *  @retval RPH_NO_ERROR
 *            Disonnection succeded
 *
 *  @retval RPH_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation.
 *            Cancelling pending operations have no meaning for a disconnect.
 *            CAK will NOT handle this through retry.
 *
 *  @remarks
 *    -# If this function is called during an asynchronous data reception,
 *       the RPH shall call the reception callback with the reception status
 *       set to \c RPH_RECEPTION_DISCONNECTED.
 */
TRphStatus rphDisconnect
(
  TRphSessionHandle  xSessionHandle
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function gets a certain amount of bytes from a return path session.
 *    In \c RPH_RECEIVE_WAIT mode, if the internal buffer already contains the
 *    requested number of bytes, the function immediately returns with these
 *    bytes. Otherwise, it waits until the requested number of bytes is
 *    received or the timeout occurs.
 *
 *    In \c RPH_RECEIVE_PACKET mode, if the internal buffer already contains data,
 *    the function immediately returns with these bytes. Otherwise, it waits
 *    until the next packet is received or the timeout occurs.
 *
 *  @param[in]  xSessionHandle
 *                Handle to the current session
 *
 *  @param[out] pxData
 *                Pointer to the receive data buffer of \c xBufferSize bytes.
 *                This buffer is allocated by the application.
 *
 *  @param[in]  xBufferSize
 *                Size in bytes of the provided receive data buffer.
 *                If the flag \c RPH_RECEIVE_WAIT is set, the caller
 *                expects that this buffer will be completely filled.
 *
 *  @param[out] pxReceivedSize
 *                Number of bytes actually read
 *
 *  @param[in]  xTimeout
 *                Maximum time to wait for completion (in milliseconds). Only
 *                used if \c RPH_RECEIVE_WAIT respectively \c RPH_RECEIVE_PACKET
 *                is set. A zero value specifies that no time-out is required.
 *                In this case, the calling task must wait until it receives \c
 *                xBufferSize bytes respectively the next packet.
 *
 *  @param[in]  xFlags
 *                RPH receive flags:
 *                - \c RPH_RECEIVE_WAIT
 *                - \c RPH_RECEIVE_NO_WAIT
 *                - \c RPH_RECEIVE_PACKET
 *
 *  @retval RPH_NO_ERROR
 *            Data correctly received
 *
 *  @retval RPH_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid
 *
 *  @retval RPH_ERROR_TIMEOUT
 *            A timeout occurred during connection
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *    -# With the \c RPH_RECEIVE_WAIT flag, if the timeout occurs, the n bytes
 *       (with n < \c xBufferSize) received until the timeout must be copied
 *       into the reception buffer \c pxData and n must be returned in \c
 *       pxReceivedSize.
 *    .
 *    -# If another error occurs (\c RPH_ERROR_BAD_HANDLE or \c RPH_ERROR ), the
 *       other returned values are not used.
 */
TRphStatus rphReceive
(
  TRphSessionHandle xSessionHandle,
  TUnsignedInt8*    pxData,
  TSize             xBufferSize,
  TSize*            pxReceivedSize,
  TOsTime           xTimeout,
  TRphReceiveFlags  xFlags
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function gets a certain amount of bytes from a return path session.
 *
 *    This function shall immediately return. The data shall be asynchronously
 *    sent through the reception callback provided in the call to \c
 *    rphConnectA(). To each successful call to the function \c rphReceiveA()
 *    shall correspond exactly 1 call to the reception callback. If the function
 *    \c rphDisconnect() is called during the reception, the callback shall
 *    still be called with the status \c RPH_RECEIVE_DISCONNECTED.
 *
 *    In \c RPH_RECEIVE_WAIT mode, if the internal buffer already contains the
 *    requested number of bytes, the callback is immediately called with these
 *    bytes; otherwise, it shall be called when the requested number of bytes is
 *    received or the timeout occurs.
 *
 *    In \c RPH_RECEIVE_PACKET mode, if the internal buffer already contains
 *    data, the callback is immediately called with these bytes; otherwise, it
 *    shall be called when the next packet is received or the timeout occurs.
 *
 *    If the connection has been established through the function \c
 *    rphConnect() or if no reception callback has been specified in \c
 *    rphConnectA(), this function shall return \c RPH_ERROR.
 *
 *  @pre
 *    The handle has been obtained from a successful call to the function
 *    \c rphConnectA().
 *
 *  @post
 *    The reception callback is called exactly once.
 *
 *  @param[in] xSessionHandle
 *               Handle to the current session
 *
 *  @param[in] pxData
 *               Receive data buffer of \c xBufferSize bytes allocated by the CA
 *               software.
 *
 *  @param[in] xBufferSize
 *               Size in bytes of the provided receive data buffer.
 *               If the flag \c RPH_RECEIVE_WAIT is set, the caller
 *               expects that this buffer will be completely filled.
 *
 *  @param[in] xTimeout
 *               Maximum time to wait for completion (in milliseconds). Only
 *               used if \c RPH_RECEIVE_WAIT respectively \c RPH_RECEIVE_PACKET
 *               is set. A zero value specifies that no time-out is required. In
 *               this case, the callback must be called when it receives \c
 *               xBufferSize bytes respectively the next packet.
 *
 *  @param[in] xFlags
 *                RPH receive flags:
 *                - \c RPH_RECEIVE_WAIT
 *                - \c RPH_RECEIVE_NO_WAIT
 *                - \c RPH_RECEIVE_PACKET
 *
 *  @retval RPH_NO_ERROR
 *            Successful call. The callback will be called.
 *
 *  @retval RPH_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *    -# With the \c RPH_RECEIVE_WAIT flag, if the timeout occurs, the n bytes
 *       (with n < \c xBufferSize) received until the timeout must be copied
 *       into the reception buffer \c pxData and n must be returned in the
 *       parameter \c xDataSize of the reception callback.
 */
TRphStatus rphReceiveA
(
  TRphSessionHandle   xSessionHandle,
  TUnsignedInt8*     pxData,
  TSize               xBufferSize,
  TOsTime             xTimeout,
  TRphReceiveFlags    xFlags
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function sends a certain amount of bytes to a return path session.
 *    This call will always wait for the time specified in the time-out value
 *    for completion.
 *
 *    This function shall not be used for a UDP connection type.
 *
 *  @param[in]  xSessionHandle
 *                Handle to the current session
 *
 *  @param[in]   pxData
 *                 Send data buffer allocated by the CA software
 *
 *  @param[in]  xBufferSize
 *                Size in bytes of the send data buffer
 *
 *  @param[out] pxSentSize
 *                Number of bytes actually sent. Must be equal to \c xSize if
 *                the function returns \c RPH_NO_ERROR. Must be less than xSize
 *                if the function returns \c RPH_ERROR_TIMEOUT. Undefined in the
 *                other cases.
 *
 *  @param[in]  xTimeout
 *                Maximum time to wait for completion (in milliseconds). A zero
 *                value specifies that no time-out is required. In this case,
 *                the calling task must wait until all bytes are sent.
 *
 *  @retval RPH_NO_ERROR
 *            Data correctly sent
 *
 *  @retval RPH_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid
 *
 *  @retval RPH_ERROR_TIMEOUT
 *            A timeout occurred during connection
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 *
 *  @remarks
 *  -# If the timeout occurs, the number of bytes correctly sent must be
 *     returned in \c pxSentSize.
 *  .
 *  -# If another error occurs (\c RPH_ERROR_BAD_HANDLE or \c RPH_ERROR ), the
 *     value returned by \c pxSentSize is not used.
 *  .
 *  -# During a modem connection, once the data are sent, the driver must
 *     immediately toggle to reception mode and store the incoming data in its
 *     internal buffer.
 *
 */
TRphStatus rphSend
(
        TRphSessionHandle  xSessionHandle,
  const TUnsignedInt8*    pxData,
        TSize              xBufferSize,
        TSize*            pxSentSize,
        TOsTime            xTimeout
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function sends a certain amount of bytes to a return path session.
 *
 *    This function is asynchronous and must return immediately. The send
 *    process result shall be asynchronously sent through the reception callback
 *    provided in the call to \c rphConnectAA().  To each successful call to the
 *    function \c rphSendA() shall correspond exactly 1 call to the send callback.
 *    If the function \c rphDisconnect() is called during the sending, the callback
 *    shall still be called with the status \c RPH_SEND_DISCONNECTED.
 *
 *    This function shall not be used for a UDP connection type.
 *
 *  @param[in]  xSessionHandle
 *                Handle to the current session
 *
 *  @param[in]  pxData
 *                Send data buffer
 *
 *  @param[in]  xBufferSize
 *                Size in bytes of the send data buffer
 *
 *  @param[in]  xTimeout
 *                Maximum time to wait for completion (in milliseconds). A zero
 *                value specifies that no time-out is required. In this case, the
 *                calling task must wait until all bytes are sent.
 *
 *  @retval RPH_NO_ERROR
 *            No problem occurs
 *
 *  @retval RPH_ERROR
 *            Other error. This error status is returned if an unexpected error
 *            occurs in DALRPH implementation. On such an error CAK will try to
 *            cancel pending operations.
 *            (ex. if an operation fails because of an unexpected condition
 *             CAK will cancel that operation and any previous ones linked.)
 *            CAK will handle this by trigerring 'LOCAL FAILURE' retry.
 */
TRphStatus rphSendA
(
        TRphSessionHandle   xSessionHandle,
  const TUnsignedInt8*     pxData,
        TSize               xBufferSize,
        TOsTime             xTimeout
)
{
  return RPH_NO_ERROR;
}
/**
 *  @brief
 *    This function is customer-specific and shall be implemented upon request
 *    only.
 *
 *    It provides the RPH with the connection metadata given by the STB
 *    application in order to apply specific connection rules.
 *
 *    This function is systematically called right after rphConnect(), even in
 *    absence of metadata. In such a case \c xSizeOfMetadata is set to 0 and
 *    \c pxMetadata is set to \c NULL.
 *
 *  @pre
 *    The function rphConnect() has been called for this handle.
 *
 *  @param[in]  xSessionHandle
 *                Handle to the current session
 *
 *  @param[in]  xSizeOfMetadata
 *                Size in bytes of the metadata buffer
 *
 *  @param[in]  pxMetadata
 *                Buffer containing the connnection metadata
 *
 *  @retval RPH_NO_ERROR
 *            No problem occurs
 *
 *  @retval RPH_ERROR
 *            Other error
 */
TRphStatus rphSetMetadata
(
        TRphSessionHandle   xSessionHandle,
        TSize               xSizeOfMetadata,
  const TUnsignedInt8*     pxMetadata
)
{
  return RPH_NO_ERROR;
}


