/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

// Internal values for NFErr error codes
// Includer of this file should define DEFINE_CATEGORY and DEFINE_ERROR to make
// use of these values
//
// IMPORTANT: We are currently report the NFErr error codes through field errorcode
//            at 'startplay' and 'endplay' messages. So each new error code should
//            be added at the end of each error category to avoid changing the
//            existing error codes.

/// ---------------------------------------------------------------------------
/// Category for all error codes indicating success.

DEFINE_CATEGORY(NFErr_Category_Success, NFERR_CATEGORY_SUCCESS_VAL)

/**
 * @name            OK
 * @directcause     The operation succeeded.
 * @rootcause       Thousands upon thousands of good decisions and actions.
 * @remedy          Do not fix this!
 */
DEFINE_ERROR(NFErr_OK)

/// ---------------------------------------------------------------------------
/// Category for all error codes indicating generic error.

DEFINE_CATEGORY(NFErr_Category_GenericError, NFERR_CATEGORY_GENERICERROR_VAL)

/**
 * @name            Bad
 * @directcause     -
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Bad)

/**
 * @name            No Memory
 * @directcause     A function call could not successfully acquire enough memory
 *                  to complete the requested operation.
 * @rootcause       1. The secure storage system cannot allocate enough room in
 *                  the secure storage on the device for a new account.
 *
 *                  2. There might exist DPI implementations that return this
 *                  from various DPI functions if they fail to allocate enough
 *                  memory.
 * @remedy          1. Either more secure storage space must be made available,
 *                  either by adding more (a DPI implementation detail), or
 *                  removing old accounts from the device.
 *
 *                  2. DPI failures are device specific, and the behavior
 *                  following this failure is unknown.
 */
DEFINE_ERROR(NFErr_NoMemory)

/**
 * @name            Not Implemented
 * @directcause     A DPI function whose implementation is optional was called
 *                  by the system, and is not currently being implemented by
 *                  this device.
 * @rootcause       -
 * @remedy          This behavior is expected and gracefully handled.
 */
DEFINE_ERROR(NFErr_NotImplemented)

/**
 * @name            Not Found
 * @directcause     1. A failure to lookup a domain in DNS.
 *
 *                  2. The playback device's event queue is empty.
 *
 *                  3. MessagePort object failed to remove a specific queued
 *                  message.
 *
 *                  4. MediaStream cannot find the download URL.
 *
 *                  5. The currently playing stream has the highest bitrate (a
 *                  stream with a higher bitrate was 'not found').
 * @rootcause       -
 * @remedy          1. Verify that the DNS settings are correct, and that the
 *                  domain being requested is correct.
 *
 *                  4. The CDN stream is invalid, and the device should
 *                  automatically choose another.
 */
DEFINE_ERROR(NFErr_NotFound)

/**
 * @name            Bad Parameter
 * @directcause     A function call received an invalid parameter.
 * @rootcause       1. OpenSSL is initialized with a random seed that is too
 *                  small.
 *
 *                  2. ConditionVariable::wait() is called with either no mutex
 *                  or an invalid mutex.
 *
 *                  3. A request to the DPI implementation of ISystem's
 *                  getChallenge() had an invalid parameter.
 *
 *                  4. NrdLib is ever passed a NULL nccp handler.
 *
 *                  5. IBufferManager::alloc() receives a bad parameter.
 *
 *                  6. libcurl returns with CURLE_UNSUPPORTED_PROTOCOL,
 *                  CURLE_URL_MALFORMAT, or CURLE_BAD_FUNCTION_ARGUMENT.
 *
 *                  7. A license for movie ID 0 is requested.
 *
 *                  8. Various DPI implementation internal usages.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_BadParameter)

/**
 * @name            File Not Opened
 * @directcause     SubtitleManager::Open() has no URLs to load.
 * @rootcause       SubtitleManager::rankUrls() wasn't called prior to a call to
 *                  Open().
 * @remedy          Call rankUrls() before calling Open().
 */
DEFINE_ERROR(NFErr_FileNotOpened)

/**
 * @name            DeadLock
 * @directcause     A mutex deadlock is detected.
 * @rootcause       The underlying pthread mutex has returned EDEADLK from a
 *                  call to pthread_mutex_lock(). As our usage of pthread
 *                  mutexes are set to be recursive, this is most likely a
 *                  programming error in which two threads have a circular
 *                  dependency on two mutexes.
 * @remedy          This is a programming error and cannot be fixed externally.
 */
DEFINE_ERROR(NFErr_DeadLock)

/**
 * @name            Uninitialized Mutex
 * @directcause     A Mutex::lock() or unlock() was called on an uninitialized
 *                  mutex.
 * @rootcause       As the constructor for Mutex properly initializes the
 *                  pthread mutex used internally, this should only occur when
 *                  memory is being overwritten randomly.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Uninitialized_Mutex)

/**
 * @name            Invalid Access
 * @directcause     A mutex unlock is being attempted by a thread that is not
 *                  the current owner.
 * @rootcause       The underlying pthread mutex is returning EPERM, which
 *                  indicates that the current thread does not have permissions
 *                  to unlock the mutex.
 * @remedy          This is a programming error and cannot be fixed externally.
 */
DEFINE_ERROR(NFErr_InvalidAccess)

/**
 * @name            Array Out Of Boundary
 * @directcause     Similar to BadParameter, this means that a parameter is
 *                  requesting access to an index of an array that is currently
 *                  invalid / non-existent.
 * @rootcause       ISystem::feedData() returns this if the stream index is
 *                  invalid.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ArrayOutOfBoundary)

/**
 * @name            Interrupted
 * @directcause     A system call was interrupted.
 * @rootcause       1. HttpStreamBuf will return this if the internal call to
 *                  select() returns an interruption (EINTR), and will abort the
 *                  connection.
 *
 *                  2. Thread::Wait() or Sleep() will return this if any of the
 *                  system calls used in the implementation (wait, nanosleep)
 *                  return EINTR.
 *
 *                  3. Used by StreamManager internally in now-deprecated (and
 *                  unused) audio switching code.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Interrupted)

/**
 * @name            TimedOut
 * @directcause     A waiting operation has reached its time limit and aborted.
 * @rootcause       If this error is being seen externally, this typically
 *                  involves a connection failure or stalled download.
 *
 *                  Internally, this error is used by condition variables and
 *                  threads to indicate timeouts on wait requests.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_TimedOut)

/**
 * @name            WaitingSelf
 * @directcause     A thread is requesting a Wait() on a Thread object that
 *                  represents itself.
 * @rootcause       -
 * @remedy          This is a programming error and cannot be fixed externally.
 */
DEFINE_ERROR(NFErr_WaitingSelf)

/**
 * @name            EndOfFile
 * @directcause     Used internally by the tool PlaybackDeviceTest.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_EndOfFile)

/**
 * @name            IOError
 * @directcause     A read operation failed or a buffer expected to have data in
 *                  it contained none.
 * @rootcause       This error is only used in the ASF demuxing code, and will
 *                  only occur if the stream ends prematurely in various places
 *                  in the code.
 * @remedy          Check for premature ending to ASF streams, either due to
 *                  invalid data or early termination of downloads.
 */
DEFINE_ERROR(NFErr_IOError)

/**
 * @name            MessageQueueFull
 * @directcause     Used internally in Message and MessagePort, which are
 *                  currently not utilized in the SDK.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MessageQueueFull)

/**
 * @name            MessageQueueEmpty
 * @directcause     Used internally in Message and MessagePort, which are
 *                  currently not utilized in the SDK.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MessageQueueEmpty)

/**
 * @name            NoReply
 * @directcause     Used by NrdLib to indicate that no an app trust store request is
 *                  pending.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NoReply)

/**
 * @name            EndOfStream
 * @directcause     Indicates that a stream (audio, video, subtitle) has no more
 *                  data. This is typically considered a successful error code.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_EndOfStream)

/**
 * @name            Rebuffer
 * @directcause     Indicates that a stream needs to rebuffer to continue playback
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Rebuffer)

/**
 * @name            Seek
 * @directcause     Indicates seek
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Seek)

/**
 * @name            DfxpError
 * @directcause     Returned from subtitle module regarding contents of dfxp file
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DfxpError)

/**
 * @name            ASFDataError
 * @directcause     Returned from the ASF demuxing code if the ASF data is
 *                  invalid.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ASFDataError)

/**
 * @name            MP4ParsedData
 * @directcause     Indicates that the MP4Demultiplexer has detected inconsistency
 *                  during MP4 parsing.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MP4ParsedData)

/**
 * @name            DRMFailed
 * @directcause     A DRM operation failed.
 * @rootcause       All possible DRM related issues currently map to this error.
 *                  This includes all failures to decrypt data, binding /
 *                  committing a DRM context, or initializing decryption
 *                  context.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DRMFailed)

/**
 * @name            InvalidStateTransition
 * @directcause     A request was made from an invalid state.
 * @rootcause       A majority of the InvalidStateTransition errors have been broken
 *                  into their own errors. This error may be a candidate for a specific
 *                  InvalidStateTransition error code.
 *
 * @remedy          -
 */
DEFINE_ERROR(NFErr_InvalidStateTransition)


/**
 * @name            InvalidStateTransition_OpenSSLLib
 * @directcause     init() should not be called twice.
 * @rootcause       This is the error used when OpenSSLLib's init() is
 *                  called twice.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_InvalidStateTransition_OpenSSLLib)


/**
 * @name            MalformedData
 * @directcause     (Deprecated) Currently not in use.
 *
 *                  It is currently referenced in an error lookup table in
 *                  MediaBridge.cpp for formatting purposes, but nothing returns
 *                  this value to MediaBridge.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MalformedData)

/**
 * @name            Stop
 * @directcause     A stop request has been received.
 * @rootcause       This is used in both the playback code and the traceroute
 *                  code to indicate that a stop request was received and that
 *                  the system gracefully stopped its processing.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Stop) // returned because of stop request

/**
 * @name            Not Allowed
 * @directcause     The current request is not allowed in the current context.
 * @rootcause       This is a generic error. Possible causes:
 *
 *                  1. Your DRM implementation does not support secure stops.
 *
 *                  2. Dynamic audio switch is not supported.
 *
 *                  3. A challenge request is made for a stream with no DRM.
 *
 *                  4. feedStreamHeader()'s data was rejected because
 *                  endOfStream was called.
 *
 *                  5. A request to switch audio tracks is made when the media
 *                  control device is not in the open/stopped state.
 *
 *                  6. A subtitle track change request is made with a closed
 *                  media control device.
 *
 *                  7. It can also mean the same as NFErr_NotImplemented.
 *
 *                  8. A call to tryLock() (Lock, Mutex) is made when the lock
 *                  is owned by another thread.
 *
 *                  9. A call to reauthorize() is made when nothing is currently
 *                  authorized.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NotAllowed)

/**
 * @name            Skip
 * @directcause     This ASF object should be skipped.
 * @rootcause       This is used on ASF Data Objects, and is expected.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Skip) // skip request to caller.

/**
 * @name            Pending
 * @directcause     There are currently no available resources; try again soon.
 * @rootcause       This is used in the video playback system as a way to signal
 *                  to other systems that more processing must take place before
 *                  this call will succeed again (basic flow control).
 * @remedy          This is expected and normal.
 */
DEFINE_ERROR(NFErr_Pending)

/**
 * @name            NetworkError
 * @directcause     Failed to connect.
 * @rootcause       The host cannot be reached (host is not there, no route to host, etc).
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NetworkError)

/**
 * @name            HTTPErrorCode
 * @directcause     An HTTP request did not return with a successful status code.
 * @rootcause       This is used by the NCCP layer if an HTTP request did not return a
 *                  status code of 200 or 206.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_HTTPErrorCode)

/**
 * @name            DNSTimedOut
 * @directcause     A DNS lookup timed out.
 * @rootcause       The DNS server is either not responding, or unreachable on the
 *                  current network.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DNSTimedOut)

/**
 * @name            ConnRefused
 * @directcause     A network connection was actively refused.
 * @rootcause       This occurs when libcurl returns ECONNREFUSED, or (since we
 *                  use curl's non-blocking socket implementation), EINPROGRESS
 *                  (a regular timeout).
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ConnRefused)

/**
 * @name            ConnReset
 * @directcause     The connection was reset by the remote peer.
 * @rootcause       This occurs when libcurl returns ECONNRESET.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ConnReset)

/**
 * @name            ConnSlow
 * @directcause     The connection was closed by the client because it was too slow.
 * @rootcause       This occurs when McplTrackDownloader closes a connection because it is too slow.
 * @remedy          - (this is not an error)
 */
DEFINE_ERROR(NFErr_ConnSlow)

/**
 * @name            Internal
 * @directcause     An internal system routine has failed.
 * @rootcause       1. The NTBA crypto factory failed to initialize.
 *
 *                  2. HttpLib failed to initialize.
 *
 *                  3. A call to curl_easy_setopt() did not return CURLE_OK.
 *
 *                  4. HttpStreamReaderBuf::open() returned false.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Internal) // internal error

/**
 * @name            SSLFailure
 * @directcause     A generic SSL failure has occurred.
 * @rootcause       1. An attempt was made to use a revoked cert.
 *
 *                  2. libcurl returned CURLE_SSL_CONNECT_ERROR. (this might be
 *                  overridden with a more generic failure).
 * @remedy          -
 */
DEFINE_ERROR(NFErr_SSLFailure)

/**
 * @name            SSLUntrusted
 * @directcause     The SSL peer cert has failed validation.
 * @rootcause       This can happen if the cert is expired, or the time on the
 *                  machine is incorrect, which causes the cert to appear to be
 *                  expired when it is actually still valid.
 * @remedy          Check the system time, and then the cert's validity.
 */
DEFINE_ERROR(NFErr_SSLUntrusted)

/**
 * @name            SSLCrlOcsp
 * @directcause     Cert verification ahs failed with a non-fatal error.
 * @rootcause       There might be more than one URI in the authority
 *                  information access extension. This can also occur if there
 *                  are any syntax problems in a CRL via X509.
 *
 *                  This error may be elevated to a fatal error (in
 *                  CertStatusUtils.cpp: processError()) if the function
 *                  considers the cert to be revoked.
 * @remedy          This is a non-fatal error.
 */
DEFINE_ERROR(NFErr_SSLCrlOcsp)

/**
 * @name            Uninitialized
 * @directcause     THe system is attempting to use an uninitalized object.
 * @rootcause       1. Mutex lock/unlock will return this if the underlying
 *                  implementation is uninitialized, but this should only ever
 *                  be the case if the Mutex was externally modified (memory
 *                  trashing).
 *
 *                  2. OpenSSL's SSL library is attempted to be initialized before
 *                  the crypto library.
 *
 *                  3. libcurl did not accept the filename given to
 *                  CURLOPT_CAINFO.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_Uninitialized)

/**
 * @name            NotReady
 * @directcause     The system is not ready to perform the requested function.
 * @rootcause       This is used internally as a flow control mechanism (similar to
 *                  NFErr_Pending).
 * @remedy          This is a non-fatal error.
 */
DEFINE_ERROR(NFErr_NotReady)

/**
 * @name            Aborted
 * @directcause     The current activity was successfully aborted.
 * @rootcause       The NCCP layer and playback system will return this error if
 *                  a request to abort occurs during the function call.
 * @remedy          This is expected.
 */
DEFINE_ERROR(NFErr_Aborted)

/**                 SysError
 * @directcause     a system call returned an errno
 */
DEFINE_ERROR(NFErr_SysError)

/**
 * @name            CurlError
 * @directcause     An error was returned from a libcurl function call
 * @rootcase        A call to libcurl ran into an error. The actual curl error code
 *                  will be in the error subclass
 * @remedy          Depends on the actual curl error
 */
DEFINE_ERROR(NFErr_CurlError)

/**
 * @name            ASError
 * @directcause     An error was returned from the adaptive streaming engine
 * @rootcase        -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ASError)

/**
 * @name            PlaybackDeviceError
 * @directcause     An error was returned from the playback device via EVENT_ERROR
 * @rootcase        -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_PlaybackDeviceError)

/**
 * @name            HostUnreachable
 * @directcause     connect() fails
 * @rootcause       network or host is unreachable
 * * @remedy          -
*/
DEFINE_ERROR(NFErr_HostUnreachable)

/**
 * @name            ConnRefused
 * @directcause     A network connection was actively refused.
 * @rootcause       This occurs when libcurl returns ECONNREFUSED, or (since we
 *                  use curl's non-blocking socket implementation), EINPROGRESS
 *                  (a regular timeout).
 * @remedy          -
 */
DEFINE_ERROR(NFErr_ConnTimedOut)

DEFINE_ERROR(NFErr_OutOfStorage)

/// ---------------------------------------------------------------------------------
/// Category for all error codes indicating MediaControl specific error.

DEFINE_CATEGORY(NFErr_Category_McError, NFERR_CATEGORY_MCERROR_VAL)

/**
 * @name            MC_AuthFailure
 * @directcause     A stream failed to authenticate.
 * @rootcause       This is the generic error that occurs when authorizeMovie()
 *                  fails for any reason
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_AuthFailure)

/**
 * @name            MC_InitStreamFailure
 * @directcause     Stream initialization has failed.
 * @rootcause       This happens when a new stream manager fails to open a movie
 *                  title (media open). (createStreamManager) This typically
 *                  happens when a call to MediaStream::init() does not return
 *                  NFErr_OK, or if a call to abort happens as the stream is
 *                  being initialized.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InitStreamFailure)

/**
 * @name            MC_StreamSetIncomplete
 * @directcause     The movie has either zero audio streams, or zero video streams.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_StreamSetIncomplete)

/**
 * @name            MC_OpenDeviceFailure
 * @directcause     Call to IPlaybackDevice::open failed
 * @rootcause       This is DPI-specific, but might occur if there is not enough
 *                  memory available.
 *                  The device's buffer manager may failed to open.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_OpenDeviceFailure)

/**
 * @name            MC_AcquireLicenseFailure
 * @directcause     An attempt to acquire a DRM license failed.
 * @rootcause       1. This can happen if a media open is aborted right as the
 *                  license is requested.
 *
 *                  2. The license retriever might not support this stream type
 *                  (WMDRM10, PLAY_READY, etc).
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_AcquireLicenseFailure)

/**
 * @name            MC_DevicePlaybackError
 * @directcause     A non-DRM-related error occurred in playback.
 * @rootcause       This is DPI specific, but typically means that a call to
 *                  feedStreamHeader or feedData returned with a failure.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_DevicePlaybackError)

/**
 * @name            MC_DevicePlaybackSetSpeedTimeout
 * @directcause     The device does not become ready to begin playback.
 * @rootcause       The device has media available to it but has not used
 *                  IESPlayerCallback to report that it is ready for playback start.
 *                  The AdaptiveStreamingPlayer has been requesting that the
 *                  playback device set speed but the device returns not
 *                  ready. The time allowed for the playback device to become
 *                  ready for playback has expired.
 * @remedy          DPI implementation needs to send
 *                  IESPlayerCallback::readyForPlaybackStart
 *                  for both the audio and video players.
 */
DEFINE_ERROR(NFErr_MC_DevicePlaybackSetSpeedTimeout)

/**
 * @name            MC_DeviceBufferingWaitCountExceeded
 * @directcause     The playback device does not accept blocks via feeData.
 * @rootcause       The maximum count of times that the device may returns
 *                  pending from feedData has been exceeded.  The
 *                  IPlaybackDevice implementation is not able to accept
 *                  additional media blocks and the device is not consuming
 *                  blocks so that it can accept more blocks.
 */
DEFINE_ERROR(NFErr_MC_DeviceBufferingWaitCountExceeded)

/**
 * @name            MC_DeviceDecryptionError
 * @directcause     A DRM-related error occurred during playback.
 * @rootcause       This is what the playback system will return if the DRM
 *                  layer ever returns NFErr_DRMFailed.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_DeviceDecryptionError)

/**
 * @name            MC_ConnectionFailure
 * @directcause     A generic connection failure has occurred.
 * @rootcause       This error returns when an already established connection is
 *                  dropped unexpectedly.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_ConnectionFailure)


/**
 * @name            MC_StreamingInitFailure
 * @directcause     Fail to initialize the streaming manager
 * @rootcause       This error returns when the streaming manager can not successfully
 *                  opened or started
 * @remedy          -
*/
DEFINE_ERROR(NFErr_MC_StreamingInitFailure)

/**
 * @name            MC_EncodingError
 * @directcause     A generic connection failure has occurred.
 * @rootcause       This error returns when there is a problem with the encoding of a
 *                  stream
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_EncodingError)

/**
 * @name            MC_MediaDurationError
 * @directcause     The audio and video streams have widely different durations.
 * @rootcause       This error returns when the audio and video streams of a
 *                  movie have durations that vary too widely. It is a type of
 *                  encoding error.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_MediaDurationError)

/**
 * @name            MC_HttpServerError
 * @directcause     An HTTP request returned with an error code 500-599.
 * @rootcause       This is a server-side failure.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_HttpServerError)

/**
 * @name            MC_HttpClientError
 * @directcause     An HTTP request returned with an error code of 400-499
 *                  (except 404).
 * @rootcause       This typically indicates a malformed request (asking for
 *                  incorrect missing data), but on some occasions might be the
 *                  server not having a file that is expected to be there.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_HttpClientError)

/**
 * @name            MC_ContentNotAvailable
 * @directcause     An HTTP request returned with an error code of 404.
 * @rootcause       The requested URL does not exist on the server.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_ContentNotAvailable)       // receive a 404 http error

/**
 * @name            MC_SubtitleFailure
 * @directcause     A failure has occurred in the subtitle system.
 * @rootcause       This can happen if the subtitle system is ever asked to
 *                  load/display a language it does not support.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_SubtitleFailure)           // unable to parse subtitles

/**
 * @name            MC_BufferAllocationFailure
 * @directcause     The BufferManager was not able to allocate media buffer pools.
 * @rootcause       This can happen if the system does not have enough available
 *                  memory to allocate pools of the desired size.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_BufferAllocationFailure)

/**
 * @name            MC_BadManifest
 * @directcause     The Manifest passed into open() fails to process
 * @rootcause       This can happen if there are no matching video tracks in the manifest
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_BadManifest)

/* start invalidstate additions */

/**
 * @name            InvalidStateTransition_ShouldBeClosed
 * @directcause     A request to state transition was made from a state other than closed.
 * @rootcause       The state was expected to be closed. If the state was not closed
 *                  it indicates some other state transition is occurring that is either in
 *                  error or should be accounted for but isn't being accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBeClosed)


/**
 * @name            InvalidStateTransition_ShouldBeOpening
 * @directcause     A request to state transition was made from a state other than opening.
 * @rootcause       The state was expected to be opening. If the state was not opening
 *                  it indicates some other state transition is occurring that is either in
 *                  error or should be accounted for but isn't being accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBeOpening)

/**
 * @name            InvalidStateTransition_ShouldBeStopped
 * @directcause     A request to state transition was made from a state other than stopped.
 * @rootcause       The state was expected to be stopped. If the state was not stopped
 *                  it indicates some other state transition is occurring that is either in
 *                  error or should be accounted for but isn't being accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBeStopped)

/**
 * @name            InvalidStateTransition_ShouldBePlaying
 * @directcause     A request to state transition was made from a state other than playing.
 * @rootcause       The state was expected to be playing. If the state was not playing
 *                  it indicates some other state transition is occurring that is either in
 *                  error or should be accounted for but isn't being accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBePlaying)

/**
 * @name            InvalidStateTransition_ShouldBePaused
 * @directcause     A request to state transition was made from a state other than paused.
 * @rootcause       The state was expected to be paused. If the state was not paused
 *                  it indicates some other state transition is occurring that is either in
 *                  error or should be accounted for but isn't being accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBePaused)

/**
 * @name            InvalidStateTransition_ShouldBePlayingOrPaused
 * @directcause     A request to state transition was made from a state other than playing
 *                  or paused.
 * @rootcause       The state was expected to be playing or paused. If the state was not
 *                  playing or paused it indicates some other state transition is occurring
 *                  that is either in error or should be accounted for but isn't being
 *                  accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBePlayingOrPaused)

/**
 * @name            InvalidStateTransition_ShouldBeStoppedOrPaused
 * @directcause     A request to state transition was made from a state other than stopped
 *                  or paused.
 * @rootcause       The state was expected to be stopped or paused. If the state was not
 *                  stopped or paused it indicates some other state transition is occurring
 *                  that is either in error or should be accounted for but isn't being
 *                  accounted for.
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_InvalidStateTransition_ShouldBeStoppedOrPaused)

/**
 * @name            StreamingFailure
 * @directcause     There was a failure reported from the streaming engine
 * @rootcause
 * @remedy          -
 */
DEFINE_ERROR(NFErr_MC_StreamingFailure)


/* end invalid state additions */

/// ---------------------------------------------------------------------------
/// Category for all error codes for Device Account Errors

DEFINE_CATEGORY(NFErr_Category_DeviceAccountError, NFERR_CATEGORY_DEVICEACCOUNT_VAL)

/**
 * @name            DeviceAccount_InvalidKey
 * @directcause     A request to set/delete the current account by account key
 *                  has failed because the key doesn't exist.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DeviceAccount_InvalidKey)

/**
 * @name            DeviceAccount_NoCurrentKey
 * @directcause     DeviceAccounts::getCurrentAccount will return this if there
 *                  is currently no account chosen/active.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DeviceAccount_NoCurrentKey)

/**
 * @name            DeviceAccount_TooManyAccounts
 * @directcause     There is no room in the secure store to create a new
 *                  account.
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_DeviceAccount_TooManyAccounts)

/// ---------------------------------------------------------------------------
/// Category for all error codes for NCCP Errors

DEFINE_CATEGORY(NFErr_Category_NCCPError, NFERR_CATEGORY_NCCP_VAL)

/**
 * @name            NCCP_RequestFailed
 * @directcause     Sending an NCCP request failed
 * @rootcause       This is a generic error code that can indicate many types of
 *                  underlying NCCP failures.
 *
 *                  The most likely causes are network issues
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_RequestFailed)

/**
 * @name            NCCP_ResponseParseFailure
 * @directcause     The server response could not be verified or failed to parse
 * @rootcause       _
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_ResponseParseFailure)

/**
 * @name            NCCP_ResponseFailed
 * @directcause     The NCCP server's response contains a failure.
 * @rootcause       This is a generic error code that can indicate many types of
 *                  underlying NCCP failures.
 *
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_ResponseFailed)

/**
 * @name            NCCP_StaleCredentials
 * @directcause     The NCCP server returned an actionID STALE_CREDENTIALS (12)
 * @rootcause       The client's NetflixID is stale
 * @remedy          The UI needs to renew the NetflixID
 */
DEFINE_ERROR(NFErr_NCCP_StaleCredentials)

/**
 * @name            NCCP_PlaintextResponse
 * @directcause     An NCCP response is plaintext not XML.
 * @rootcause       _
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_PlainTextResponse)

/**
 * @name            NCCP_MismatchedServerClientTime
 * @directcause     Parse error in NCCP response. Response time isn't clientTime +1.
 * @rootcause       This is an error in the NCCP server response.
 * * @remedy          -
*/
DEFINE_ERROR(NFErr_NCCP_MismatchedServerClientTime)

/**
 * @name            NCCP_InvalidRedirect
 * @directcause     Nccp was redirected from https to http or vice versa
 * @rootcause       -
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_InvalidRedirect)

/**
 * @name            NCCP_RegistrationRequired
 * @directcause     An attempt to send an Nccp transaction that requires registration
                    but no CTicket was available
 * @rootcause       _
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_RegistrationRequired)

/**
 * @name            NCCP_ParseablePlaintextResponse
 * @directcause     An NCCP response is plaintext not XML but is parseable for data
 * @rootcause       _
 * @remedy          -
 */
DEFINE_ERROR(NFErr_NCCP_ParseablePlainTextResponse)

/// ---------------------------------------------------------------------------
/// Category for all error codes for Crypto Errors

DEFINE_CATEGORY(NFErr_Category_CryptoError, NFERR_CATEGORY_CRYPTO_VAL)

/**
 * @name            CryptoValidationFailure
 * @directcause     Crypto signature does not match.
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_ValidationFailure)

/**
 * @name            CryptoSignatureNotFound
 * @directcause     HMAC verification fails because no signature can be found in
 *                  the HTTP header.
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_SignatureNotFound)

/**
 * @name            CryptoHmacCalculationFailure
 * @directcause     HMAC calculation fails because of an internal crypto error.
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_HmacCalculationFailure)

/**
 * @name            CryptoSignatureCalculationFailure
 * @directcause     Signature calculation fails because of an internal crypto error.
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_SignatureCalculationFailure)

/**
 * @name            Crypto_DHBadKey
 * @directcause     Failed to create a DiffieHellman key because of bad data
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_DHBadKey)

/**
 * @name            Crypto_FactorySetKeysFailed
 * @directcause     Call to ICryptoFactory::setKeys failed
 * @rootcause       -
 * @remedy          -
*/
DEFINE_ERROR(NFErr_Crypto_FactorySetKeysFailed)

