/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NRDP_IMDX_H
#define NRDP_IMDX_H

/** @file IMdx.h The mdx interface.
 *
 * IMdx.h declares the interface for application to use mdx:
 * - initializes/shutdowns mdx in preparation of content playback.
 * - navigates within the content for operations such as play, stop, pause.
 * - controls audio/subtitle track selection.
 * - queries playback related information.
 * - registers listener in order to receive mdx event callbacks.
 */

#include <vector>

#include "MdxError.h"
#include <nrdbase/tr1.h>
#include <nrdbase/Variant.h>

namespace netflix {

namespace mdx {

class CryptContext;

/**
 * @class IMdx IMdx.h
 * @brief The mdx provides content playback.
 */
class IMdx
{
public:
    /** Mdx state. */
    enum State {
        NOT_INITIALIZED,
        INITIALIZED
    };

     enum AdvertisingState
     {
         NOT_ADVERTISING,
         ADVERTISING
     };

     enum DiscoveryState
     {
         NOT_DISCOVERING,
         DISCOVERING
     };

    /**
     * @class Listener IMdx.h
     * @brief Mdx listeners are notified of changes and
     *        errors.
     */
    class Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        /**
         * Notified when the mdx state is changed.
         * @param[in] state the new mdx state.
         */
        virtual void stateChanged(State state, const std::string& ssid, int listeningPort_) = 0;
        virtual void advertisingStateChanged(AdvertisingState state) = 0;
        virtual void discoveryStateChanged(DiscoveryState state) = 0;

        /**
         * Notified when a MDX device is found on the network.
         */
        typedef std::vector<std::pair<std::string, std::string> > ResponseHeaders;
        virtual void DeviceFound(const std::string& location,
                                 const std::string& serviceType,
                                 const std::string& USN,
                                 const std::string& friendlyName,
                                 const ResponseHeaders &headers,
                                 bool activated,
                                 bool paired) = 0;

        /**
         * Notified when a MDX device leaves the network.
         */
        virtual void DeviceByeBye(const std::string& serviceType, const std::string& USN) = 0;

        /**
         * Notified when http response is received.
        */
        virtual void HttpResponse(const std::string& url, int method , llong xid, int32_t responseCode, const std::string& headers, const std::string& body) = 0;

        /**
         * Notified when incoming mongoos message is received.
         */
        virtual void IncomingMessage(const std::string& url, const Variant& headers, const std::string& body) = 0;

        /**
         * Notified when incoming websocket message is received.
         */
        virtual void WebSocketMessage(const Variant& headers, const std::string& body) = 0;

        /**
         * Notified when the network interface is changed.
         * @param[in] prevInterface the previous interface
         * @param[in] newInterface the new interface.
         */
        virtual void interfaceChanged(const std::string &prevInterface, const std::string &newInterface, bool connected) = 0;

        /**
         * Notified when an error condition happens.
         * @param[in] error the error code.
         */
        virtual void error(MdxErrorCode error) = 0;

        /**
         * Notified when Dial device description completes
         * @param[in] url - the url from which the device description was retrieved
         * @param[in] USN - the USN
         * @param[in] serviceType - the serviceType
         * @param[in] responseHeaders - the HTTP response headers
         * @param[in] desc - the description retrieved
         */
        virtual void DialDeviceInfoResponse(const std::string &url,
                                            const std::string &USN,
                                            const std::string &serviceType,
                                            const Variant &responseHeaders,
                                            const Variant &desc) = 0;

        virtual void webSocketConnect(const std::string &host) = 0;
        virtual void webSocketDisconnect(const std::string &host) = 0;
        virtual void webSocketSent(const std::string &host, ullong xid) = 0;
    };

    virtual ~IMdx() {}

    /**
     * Configure MDX ssdp advertisements
     *
     */
    virtual MdxErrorCode mdxConfigure(int advertisingPeriod,
                                      int advertisingTTL,
                                      int advertisingPort,
                                      int listeningPort,
                                      int numSsdpReplies,
                                      int msgLimit) = 0;

    /**
     * Initialize MDX
     *
     */
    virtual MdxErrorCode mdxInit(const std::string& host,
                                 const std::string& serviceType,
                                 const std::string& USN,
                                 const std::string& friendlyName,
                                 const std::string& manufacturer,
                                 const std::string& modelName,
                                 bool asyncHttpRequests,
                                 bool enableWebSocketServer) = 0;

    /**
     * Deinitialize MDX
     *
     */
    virtual MdxErrorCode mdxDeinit() = 0;

    /**
     * Add to the list of interface names that we should listen on and/or advertise through
     */
    virtual MdxErrorCode mdxAddInterfaceName(const std::string &name) = 0;

    /**
     * Issues an explicit search for the service type specified
     */
    virtual MdxErrorCode SearchForMdxDevices(const std::string& serviceType,
                                             const std::vector<std::string>& headerPatterns,
                                             int mx,
                                             int numSsdpSearches) = 0;

    /**
     * Stops discovering netflix targets
     *
     * @return MdxError_OK if the stopped;
     */
    virtual MdxErrorCode StopMdxDiscovery() = 0;

    /**
     * Reveals the presence of a netflix target
     *
     * @return NFErr_OK if successful
     *         NFErr_Bad on error
     */
    virtual MdxErrorCode RevealTargetPresence() = 0;

    /**
     * Sets headers included in advertisements and replies to searches
     *
     * @return MdxError_OK if successful
     *         NFErr_Bad on error
     */
    virtual MdxErrorCode SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders) = 0;

    /**
     * Hides the presence of a netflix target
     *
     * @return MdxError_OK if successful
     *         NFErr_Bad on error
     */
    virtual MdxErrorCode HideTargetPresence() = 0;

    /**
     * Starts Advertising netflix target
     *
     * @return MdxError_OK if the stopped;
     */
    virtual MdxErrorCode StartMdxAdvertising() = 0;

    /**
     * Stops Advertising netflix target
     *
     * @return MdxError_OK if the stopped;
     */
    virtual MdxErrorCode StopMdxAdvertising() = 0;

    /**
     * Sends HttpRequest
     *
     * @param[in] url.
     * @param[in] method
     * @param[in] transaction id
     * @return MdxError_OK if the open operation is in progress;
     *         NFErr_InvalidStateTransition if the current state is not CLOSED.
     */
    virtual MdxErrorCode SendMdxHttpRequest(const std::string& url,
                                            int method,
                                            llong xid,
                                            int curltimeout,
                                            const std::string& headers,
                                            const std::string& body) = 0;

    /**
     * Encrypts the parameter plaintext, inserts it into the body of the message
     * and the canonicalized message, calculates HMAC value of canonicalized
     * message, and finally sends the message as an HttpRequest
     *
     * @param[in] url
     * @param[in] method
     * @param[in] xid
     * @param[in] curltimeout
     * @param[in] context
     * @param[in] headers
     * @param[in] body
     * @param[in] message
     * @param[in] plaintext
     * @return MdxError_OK if the open operation is in progress;
     *         NFErr_Bad if the current state is not INITIALIZED
     */
    virtual MdxErrorCode SendSessionMessage(const std::string &url,
                                            int method, llong xid,
                                            int curltimeout,
                                            shared_ptr<CryptContext> context,
                                            const std::string &headers,
                                            const std::string &body,
                                            const std::string &message,
                                            const std::string &plaintext) = 0;

    /**
     * Encrypts the parameter plaintext, inserts it into the body of the message
     * and the canonicalized message, calculates HMAC value of canonicalized
     * message, and finally sends the message as an HttpRequest
     *
     * @param[in] context
     * @param[in] message
     * @param[in] messageHmac
     * @param[in] ciphertext
     * @param[out] outHmac - will contain the calculated HMAC value
     * @param[out] outPlaintext - if return value is MdxError_OK, contains decrypted ciphertext, else empty
     * @return MdxError_OK if the open operation succeeded.
     *         NFErr_Bad if the calculated HMAC does not match messageHmac.
     */
    virtual MdxErrorCode ProcessSessionMessage(shared_ptr<CryptContext> context,
                                               const std::string &message,
                                               const std::string &messageHmac,
                                               const std::string &ciphertext,
                                               std::string &outHmac,
                                               std::string &outPlaintext) = 0;

    /**
     * Attempts to retrieve a device description of a DIAL device at the given url
     * @param[in] url
     * @param[in] USN
     * @param[in] serviceType
     * @param[in] timeout
     * @return MdxError_OK
     */
    virtual MdxErrorCode GetDialDeviceInfo(const std::string &url, const std::string &USN, const std::string &serviceType, int timeout) = 0;

    /**
     * Indicates that the active network interface has changed
     *
     * @param[in] the name of the new interfance
     */
    virtual void InterfaceChanged(const std::string &newInterface, bool connected, const std::string & ipAddress, const std::string & ssid) = 0;

    virtual MdxErrorCode SendWebSocketMessage(const std::string &host, ullong xid, const std::string &body) = 0;

    /**
     * Add a listener to the mdx.
     *
     * @param[in] l the listener.
     * @return MdxError_OK if successfully added or the listener was
     *         already registered;
     *         NFErr_Bad if unsuccessful.
     */
    virtual MdxErrorCode addListener(Listener& l) = 0;

    /**
     * Remove a listener from the mdx.
     *
     * @param[in] l the listener.
     * @return MdxError_OK if successfully removed or the listener was
     *         not registered;
     *         NFErr_Bad if unsuccessful.
     */
    virtual MdxErrorCode removeListener(Listener& l) = 0;

    /**
     * Returns the mdx state.
     *
     * @return the mdx state.
     */
    virtual State getState() const = 0;
    virtual AdvertisingState getAdvertisingState() const = 0;
    virtual DiscoveryState getDiscoveryState() const = 0;

    /**
     * Returns the network interface name
     *
     * @return the network interface name
     */
    virtual std::string getInterface() const = 0;

    /**
     * Returns the network ip address
     *
     * @return the network ip address
     */
    virtual std::string getIpAddress() const = 0;
};

}} // namespace netflix::mdx

#endif
