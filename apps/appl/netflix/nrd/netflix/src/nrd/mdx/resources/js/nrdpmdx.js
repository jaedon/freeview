/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp*/

nrdp.mdx = {
    _path: "mdx",
    addEventListener: function(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener(this, evt, listener); },
    _encodedList: [ { list: ['body', 'url', 'USN', 'friendlyName', 'serviceType', 'location'],
                      decodeFn: decodeURIComponent },
                    { list: [ 'responseHeaders' ],
                      decodeFn: function(obj) {
                          var decodedObj = {},
                          member;
                          for(member in obj) {
                              decodedObj[member] = decodeURIComponent(obj[member]);
                          }
                          return decodedObj;
                      }
                    }
                  ],
    get NOT_INITIALIZED() { return 0; },
    get INITIALIZED() { return 1; },

    get NOT_ADVERTISING() { return 0; },
    get ADVERTISING() { return 1; },

    get NOT_DISCOVERING() { return 0; },
    get DISCOVERING() { return 1; },

    get interfaceName() { return this._syncData ? this._syncData.interfaceName : undefined; },
    get localIPAddress() { return this._syncData ? this._syncData.localIPAddress : undefined; },
    get nativeVersion() { return this._syncData ? this._syncData.nativeVersion : undefined; },
    get state() { return this._syncData ? this._syncData.state : undefined; },

    MdxConfigure: function(advertisingPeriod,
                           advertisingTTL,
                           advertisingPort,
                           listeningPort,
                           numSsdpReplies,
                           msgLimit) {
        nrdp.mdx._fn("MdxConfigure",
                     {advertisingPeriod: advertisingPeriod,
                      advertisingTTL: advertisingTTL,
                      advertisingPort: advertisingPort,
                      listeningPort: listeningPort,
                      numSsdpReplies: numSsdpReplies,
                      msgLimit: msgLimit});
    },

    MdxInit: function(host, port, serviceType, uuid, asyncHttpRequests, webSocketServer) {
        nrdp.mdx._fn("MdxInit",
                     {host: encodeURIComponent(host),
                      port: port,
                      serviceType: encodeURIComponent(serviceType),
                      uuid: encodeURIComponent(uuid),
                      asyncHttpRequests: asyncHttpRequests,
                      webSocketServer: webSocketServer});
    },

    MdxDeinit: function() {
        nrdp.mdx._fn("MdxDeinit");
    },

    AddInterfaceName: function(name) {
        nrdp.mdx._fn("AddInterfaceName", {name: encodeURIComponent(name)});
    },

    InterfaceChanged: function(newInterface, connected, ipaddress, ssid) {
        nrdp.mdx._fn("InterfaceChanged", {newInterface: encodeURIComponent(newInterface), connected: connected, ipaddress: ipaddress, ssid:ssid});
    },

    SearchForMdxDevices: function(serviceType, headerPatterns, mx, numSsdpSearches) {
        nrdp.mdx._fn("SearchForMdxDevices",
                     {serviceType: encodeURIComponent(serviceType),
                      headerPatterns: nrdp.mdx._encodeArray(headerPatterns),
                      mx: mx,
                      numSsdpSearches: numSsdpSearches});
    },

    StopMdxDiscovery: function(cb) {
        nrdp.mdx._fn("StopMdxDiscovery", undefined, cb);
    },

    RevealTargetPresence: function(cb) {
        nrdp.mdx._fn("RevealTargetPresence", undefined, cb);
    },

    SetDeviceReplyHeaders: function(deviceReplyHeaders, cb) {
        nrdp.mdx._fn("SetDeviceReplyHeaders",
                     {deviceReplyHeaders: nrdp.mdx._encodeArray(deviceReplyHeaders)},
                    cb);
    },

    HideTargetPresence: function(cb) {
        nrdp.mdx._fn("HideTargetPresence", undefined, cb);
    },

    StartMdxAdvertising: function(cb) {
        nrdp.mdx._fn("StartMdxAdvertising", undefined, cb);
    },

    StopMdxAdvertising: function() {
        nrdp.mdx._fn("StopMdxAdvertising");
    },

    SendMdxHttpRequest: function(url, requestType, xid, curlTimeout, requestHeader, requestBody) {
        nrdp.mdx._fn("SendMdxHttpRequest",
                     {url: encodeURIComponent(url),
                      requestType: requestType,
                      xid: xid,
                      curltimeout: curlTimeout,
                      requestHeader: encodeURIComponent(requestHeader),
                      requestBody: encodeURIComponent(requestBody)});
    },

    SendSessionMessage: function(url, requestType, xid, curlTimeout, context, requestHeader, requestBody, message, plaintext) {
        nrdp.mdx._fn("SendSessionMessage",
                     {url: encodeURIComponent(url),
                      requestType: requestType,
                      xid: xid,
                      curltimeout: curlTimeout,
                      context: encodeURIComponent(context),
                      requestHeader: encodeURIComponent(requestHeader),
                      requestBody: encodeURIComponent(requestBody),
                      message: encodeURIComponent(message),
                      plaintext: encodeURIComponent(plaintext)});
    },

    SendWebSocketMessage: function(host, xid, body) {
        nrdp.mdx._fn("SendWebSocketMessage",
                     {host: encodeURIComponent(host),
                      xid : xid,
                      body: encodeURIComponent(body)});
    },

    ProcessSessionMessage: function(context, xid, message, messageHmac, ciphertext, cb) {
        nrdp.mdx._fn("ProcessSessionMessage",
                     { context: encodeURIComponent(context),
                       xid: xid,
                       message: encodeURIComponent(message),
                       messageHmac: encodeURIComponent(messageHmac),
                       ciphertext: encodeURIComponent(ciphertext)
                     },
                     cb);
    },

    DialGetDeviceInfo: function(url, USN, serviceType, timeout) {
        nrdp.mdx._fn("DialGetDeviceInfo",
                     {url: encodeURIComponent(url),
                      USN: encodeURIComponent(USN),
                      serviceType: encodeURIComponent(serviceType),
                      timeout: timeout});
    },

    beginContext: function(sharedSecret, context, cb) {
        // Note: the parameter "context" is only included in order to make the
        // function signature for nrdp.mdx.beginContext identical to the function
        // signature for nrdp.ntba.beginCustomContext, it is unused (it is unused
        // in the ntba version as well, but we don't bother to even pass it here)
        nrdp.mdx._fn("beginContext",
                     {sharedSecret: sharedSecret},
                     cb);
    },

    decrypt: function(context, data, cb) {
        nrdp.mdx._fn("decrypt",
                     { context: context,
                       data: data },
                     cb);
    },

    encrypt: function(context, data, cb) {
        nrdp.mdx._fn("encrypt",
                     { context: context,
                       data: data },
                     cb);
    },

    endContext: function(context, cb) {
        nrdp.mdx._fn("endContext",
                     {context: context},
                     cb);
    },

    hmac: function(context, data, cb) {
        nrdp.mdx._fn("hmac",
                     { context: context,
                       data: data },
                     cb);
    },

    _nextIdx: 1,
    _cbs: [],
    _fn: function(name, params, cb) {
        if (!this._syncData)
            return undefined;
        if (typeof cb === "function") {
            if (!params) params = {};
            params.idx = this._nextIdx++;
            this._cbs[params.idx] = cb;
        }
        nrdp._invoke("mdx", name, params);
        return params ? params.idx : undefined;
    },

    _isEncodedString: function(eventkey) {
        for ( var index in nrdp.mdx._encodedStringList) {
            if (eventkey == nrdp.mdx._encodedStringList[index])
                return true;
        }
        return false;
    },

    _decodeEventData: function(data, key) {
        var list, encodedList, item, compareKey, value;
        // set here, so if not found in any lists, the value is returned as-is
        value = data[key];
        if(typeof value != null) {
            for (list in nrdp.mdx._encodedList) {
                if(nrdp.mdx._encodedList.hasOwnProperty(list)) {
                    encodedList = nrdp.mdx._encodedList[list];
                    for (item in encodedList.list) {
                        if(encodedList.list.hasOwnProperty(item)) {
                            compareKey = encodedList.list[item];
                            if (compareKey === key) {
                                return encodedList.decodeFn(value);
                            }
                        }
                    }
                }
            }
        }
        return value;
    },

    _handleEvent: function(event) {
        var mydata = {};

        for (var key in event.data) {
            mydata[key] = nrdp.mdx._decodeEventData(event.data, key);
        }

        if (event.name === "returnValue") {
            if(!event.data || !event.data.idx) {
                return false;
            }

            if (typeof this._cbs[event.data.idx] === "function") {
                var cb = this._cbs[event.data.idx];
                delete this._cbs[event.data.idx];
                cb(mydata.data, (mydata.data ? mydata.data.idx : undefined));
            }

            return true;
        } else {
            nrdp._callEventListeners(this, mydata);
            return true;
        }
    },

    _encodeArray: function(arr) {
        var encodedArr = [],
        i;
        for(i in arr) {
            if(arr.hasOwnProperty(i)) {
                encodedArr.push(encodeURIComponent(arr[i]));
            }
        }
        return encodedArr;
    }
};
