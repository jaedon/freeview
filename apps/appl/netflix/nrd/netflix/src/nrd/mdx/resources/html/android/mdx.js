nrdp.mdxjs = '1.0-m64';
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

mdx = {
    _isReady: false,
    _role:"",
    _commandMap:"",
    _remoteDeviceMap: {},
    _bothMode:"",
    _lastXid: 1,
    COMPLETE: nrdp?nrdp.COMPLETE:"COMPLETE",
    MYUUID:"nrdp0000000000000000000033339999",
    TARGET:"TARGET",
    CONTROLLER:"CONTROLLER",
    BOTH:"BOTH",
    _CTICKET:"",
    _uuidToken: "uuid:",
    advertisementConfig: { serviceType: "mdx-netflix-com:service:target:1",
                           uuid: "nrdp0000000000000000000033339999"
    },
    get version() { return nrdp.mdxjs; },
    get nativeVersion() { return nrdp.mdx.nativeVersion; },

    ERRORS : {       MDX_ERR_SSDP : 1,
                     MDX_ERR_PORT : 2,
                     MDX_ERR_NETWORK  : 3,
                     MDX_ERR_VERSION  : 4,
                     MDX_SESSION_INVALID_HMAC  : 5,
                     MDX_SESSION_DECRYPTION_FAILED  : 6,
                     MDX_PAIRING_NO_MATCH  : 10,
                     MDX_SESSION_INVALID_SID : 11,
                     MDX_ERR_NOT_PAIRED : 12
    },

    get isReady() { return mdx._isReady; },

    log: function(msg) {
        nrdp.log.trace(msg, "MDX");
    },

    nextTransactionId: function() {

// TODO: Spyder does not have nrdp.now()
//		var xid = nrdp.now();
        var xid = Date.now();

        if (xid <= this._lastXid) {
            xid = this._lastXid + 1;
        }

        this._lastXid = xid;

        return xid;
    },

    createMessage: function(cmd, body, uuid) {
        var message = {};
        message.cmd = cmd;
        message.body = body;
        message.transport = "HTTP";
        message.timeStamp = Date.now();
        mdx._remoteDeviceMap[uuid].msgQue.push(message);
        mdx._processMsgQueue(uuid);
    },
    _processMsgTimout: function(uuid, myxid) {
        // check the timestamp of the first msg in queue, if it is stale, drop it
        mdx.log("checking messages timed out for uuid: " + uuid);
        var myTarget = mdx._remoteDeviceMap[uuid];
        if (myTarget) {
          var evt = {
                                type: "messagingerror",
                                errorCode: mdx.session.ERRORS.MDX_SESSION_NETWORK_ERROR,
                                pairingContext: myTarget.pairingContext,
                                sid: myTarget.sessionId,
                                transactionId: myTarget.inFlightXid
                            };
            myTarget.inFlight = false;
            myTarget.inFlightXid = 0;
            delete myTarget.inFlightAction;
            nrdp._callEventListeners("mdx.session", evt);
          // Now go through the queue to check any other messages are relevant
          for ( var msg in myTarget.msgQue) {
            if ( ( Date.now() - myTarget.msgQue[msg].timeStamp ) > 5000 ) {
            	 mdx.log("message timed out for: " + uuid + " dropping...");
                var outgoingMessage = myTarget.msgQue.shift();
                var evt = {
                                type: "messagingerror",
                                errorCode: mdx.session.ERRORS.MDX_SESSION_NETWORK_ERROR,
                                pairingContext: myTarget.pairingContext,
                                sid: myTarget.sessionId,
                                transactionId: outgoingMessage.body.nonce
                            };
                myTarget.inFlight = false;
                nrdp._callEventListeners("mdx.session", evt);
                nrdp.log.error("Sending message timed out for " + uuid, "MDX");
            }
          }

          if (myTarget.msgQue.length > 0) {
            mdx._processMsgQueue(uuid);
          } else {
            myTarget.inFlight = false;

          }
        }
    },
    _processMsgQueue: function(uuid) {

        var myTarget = mdx._remoteDeviceMap[uuid];
        if (!myTarget) {
            mdx.log("remoteDevice not found uuid=" + uuid );
            return;
        }
        if (myTarget.msgQue.length > 0) {
            if (!myTarget.inFlight) {
                var outgoingMessage = myTarget.msgQue.shift();
                var myUrl =  myTarget.loc + outgoingMessage.cmd;
                var myxid = outgoingMessage.body.nonce?outgoingMessage.body.nonce:0;
                var stringBody = mdx.guard.processOutgoingMessage((outgoingMessage.body) ? outgoingMessage.body : {});
                mdx.log("Outgoing message body: " + stringBody);
                myTarget.inFlightXid = outgoingMessage.body.nonce;
                myTarget.inFlightAction = outgoingMessage.body.action;
                nrdp.mdx.SendMdxHttpRequest(myUrl, 0, myxid, "", stringBody);
                // Try sending the message after some time
                myTarget.timer = setTimeout(function(){mdx._processMsgTimout(uuid, myxid);}, 5000);
                myTarget.inFlight = true;
            } else {

                mdx.log("Cannot send the message yet");
            }
        }
    },

    _cTicketcb: function(ctick) {
        if (ctick.cticket == "") {
            mdx.log("invalid cticket");
            return;
        }
        mdx._CTICKET = ctick.cticket;
        var myuuid = nrdp.device.ESN.substr(nrdp.device.ESNPrefix.length);
        mdx.log("ESN UUID " + myuuid);
        if (myuuid.length >= 16)
            mdx.MYUUID = myuuid;
        mdx.log("mdx UUID = " + mdx.MYUUID);
        nrdp.mdx.MdxInit(nrdp.mdx.localIPAddress, mdx.advertisementConfig.port, "urn:" + mdx.advertisementConfig.serviceType, mdx.MYUUID);
    },

    _responseReceived: function(target) {
        mdx.log("mdx response received INFLIGHT= false");;
        target.inFlight = false;
        target.inFlightXid = 0;
        delete target.inFlightAction;
        clearTimeout(target.timer);
        delete target.timer;
        mdx._processMsgQueue(target.uuid);
    },

    addEventListener: function(evt, listener) { nrdp._addEventListener("mdxapi", evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener("mdxapi", evt, listener); },

    configure: function(advertisingPeriod, advertisingTTL, advertisingPort, listeningPort) {
        nrdp.mdx.MdxConfigure(advertisingPeriod, advertisingTTL, advertisingPort, listeningPort);
    },

    init: function(role, commandMap) {
        if ( !( (role == mdx.TARGET) || (role == mdx.CONTROLLER) || (role == mdx.BOTH))) {
            mdx.log("mdx.init invalid role: " + role);
            return;
        }

        window.addEventListener("unload", function(){mdx.exit();});

        if (!mdx._listenersAdded) {
            nrdp.mdx.addEventListener("statechanged", mdx._mdxDispatcher);
            nrdp.mdx.addEventListener("advertisingstatechanged", mdx._mdxDispatcher);
            nrdp.mdx.addEventListener("discoverystatechanged", mdx._mdxDispatcher);
            nrdp.mdx.addEventListener("error", mdx._mdxDispatcher);
            mdx._listenersAdded = true;
        }

        mdx.log("mdx.ini: version = " + mdx.version + " nativeVersion = " + mdx.nativeVersion + " pairing.version = " + mdx.pair._pairingVersion + " session.version = " + mdx.session._sessionVersion);

        if (nrdp.registration.registered)
            nrdp.registration.ping();
        mdx._role = role;
        mdx._commandMap = commandMap;
        mdx.guard.setCommandMap(commandMap);
        mdx._addInterfaceNames();
        var myuuid = nrdp.device.ESN.substr(nrdp.device.ESNPrefix.length);
        mdx.log("ESN UUID " + myuuid);
        if (myuuid.length >= 16)
            mdx.MYUUID = myuuid;
        mdx.log("mdx UUID = " + mdx.MYUUID);
        mdx.device.retrieveDeviceMap();
        nrdp.ntba.getCticket(mdx._cTicketcb);
    },

    _getUriInfo : function(uri, headers) {
        var reg = new RegExp("((https?://[^/]+)?/)([^?#]*)(\\?([^#]+))?(#.*)?");
        var arr = reg.exec(uri) || [];
        return {
            loc: arr[2] ? arr[1] : "http://" + headers.Host.trim() + "/",
            path: arr[3] || '',
            query: arr[5] || ''
        };
    },

    _mdxDispatcher:function(evt) {
        mdx.log("_mdxDispatcher received message : " + evt.type);
        switch(evt.type) {
        case "statechanged":
            if ( evt.state == nrdp.mdx.INITIALIZED ) {
                evt = {
                    type: "init",
                    status: "READY"
                };
                if ( (mdx._role == mdx.TARGET) || ( mdx._role == mdx.BOTH)) {
                    mdx.discovery.revealPresence();
                }
                if ((mdx._role == mdx.CONTROLLER) || ( mdx._role == mdx.BOTH))  {
                    mdx.discovery._discover();
                }
                mdx.guard.init();
                mdx.pair._init();
                mdx.session._init();
                nrdp._callEventListeners("mdxapi", evt);
            }
            break;
        case "error":
            if (evt.error == 1) { // 1 == MdxError_UpnpInitFailed
                evt = { type: "initerror",
                        errorCode: mdx.ERRORS.MDX_ERR_SSDP,
                        errorDesc: "UPnP Error" };
                nrdp._callEventListeners("mdxapi", evt);
                nrdp.log.error("MDX init error: UPnP Init Failed", "MDX");
            } else if (evt.error == 2) { // 2 == MdxError_MongooseInitFailed
                evt = { type: "initerror",
                        errorCode: mdx.ERRORS.MDX_ERR_PORT,
                        errorDesc: "Mongoose Init Fail" };
                nrdp._callEventListeners("mdxapi", evt);
                nrdp.log.error("MDX init error: Mongoose Init Failed", "MDX");
            } else if (evt.error == 3) { // 3 == MdxStartAdvertisementFailed
                // TODO: report to application?
                nrdp.log.error("MDX error: StartAdvertisement Failed", "MDX");
            } else if (evt.error == 4) { // 4 == MdxStartControlPointFailed
                // TODO: report to application?
                nrdp.log.error("MDX error: StartControlPoint Failed", "MDX");
            }
            break;
        case "advertisingstatechanged":
            if (evt.state == nrdp.mdx.ADVERTISING) {
            } else {
            }
            break;
        case "discoverystatechanged":
            if (evt.state == nrdp.mdx.DISCOVERING) {
            } else {
            }
            break;
        }
    },

    _addInterfaceNames: function() {
        ["ETHERNET", "WIFI"].map(nrdp.mdx.AddInterfaceName);
    },

    interfaceChanged: function(interfaceName, connected) {
        mdx.log("interface changed to: " + interfaceName);
        nrdp.mdx.InterfaceChanged.apply(nrdp.mdx, arguments);
    },

    exit: function() {
        mdx.log("mdx exit");
        // clean up session IDs
        mdx.session._endAllSessions();
        mdx.session._clearAllSessions();
        mdx.device.storeDeviceMap();
        nrdp.mdx.MdxDeinit();
    }
};
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

mdx.device = {
    DEVICEKEY:"REMOTE_DEVICE_MAP",

    addToRemoteDeviceMap: function(loc, serviceType, usn, uuid, activated, paired, fname, alias){ // TODO create remote device object
        var deviceInfo = {};
        var deviceMsgQue = [];
        if (loc && (loc.substr(0,5) == "http:")) {
            deviceInfo.loc = loc.substring(0, loc.indexOf(":", 5)) + ":9080/"; //TODO use the configured port
        } else {
            deviceInfo.loc = "http://" + loc + ":9080/";
        }
        deviceInfo.serviceType = serviceType;
        deviceInfo.usn = usn;
        deviceInfo.uuid = uuid;

        deviceInfo.activated = activated;
        deviceInfo.paired = paired;
        deviceInfo.fname = fname;
        deviceInfo.lastseen = Date.now();
        deviceInfo.pairingContext = "";
        deviceInfo.paired = 0;
        deviceInfo.inrange = true;
        deviceInfo.userId = "";
        deviceInfo.controllersharedsecret = "";
        deviceInfo.targetsharedsecret = "";
        deviceInfo.ntbaContext = "";
        deviceInfo.pairingMatch = 0;
        deviceInfo.localSessionId = 0;
        deviceInfo.sessionId = 0;
        deviceInfo.msgQue = deviceMsgQue;
        deviceInfo.sendQueue = [];
        deviceInfo.recvQueue = [];
        deviceInfo.inFlight = false;
        deviceInfo.xid = 0;
        deviceInfo.inFlightXid = 0;
        deviceInfo.msgObject = "";
        deviceInfo.hmac = "";
        deviceInfo.timer = {};
        deviceInfo.pairingTimer = undefined;
		// for anti-replay
		deviceInfo.sentList = [];
		deviceInfo.recvdList = [];
        mdx._remoteDeviceMap[uuid] = deviceInfo;
        return deviceInfo;
    },

    getPairedDevice: function(pairingContext) {
        for ( target in mdx._remoteDeviceMap) {
            if (mdx._remoteDeviceMap[target].pairingContext == pairingContext)
                return mdx._remoteDeviceMap[target];
        }
        return null;
    },

    getRemoteDevicefromXid: function(xid) {
        for ( target in mdx._remoteDeviceMap) {

            if (mdx._remoteDeviceMap[target].inFlightXid == xid)
                return mdx._remoteDeviceMap[target];
        }
        return null;
    },

    getRemoteDevicefromLocalSid: function(sessionId) {
        for ( target in mdx._remoteDeviceMap) {
            if (mdx._remoteDeviceMap[target].localSessionId == sessionId)
                return mdx._remoteDeviceMap[target];
        }
        return null;
    },

    getRemoteDevicefromSid: function(sessionId) {
        for ( target in mdx._remoteDeviceMap) {
            if (mdx._remoteDeviceMap[target].sessionId == sessionId)
                return mdx._remoteDeviceMap[target];
        }
        return null;
    },

    getPairingContextFromUuid: function(uuid) {
    	nrdp.registration.ping();
        return mdx._remoteDeviceMap[uuid].pairingContext;
    },

    storeDeviceMap: function() {
        var deviceMap = {};
        var devAcctKey = nrdp.registration.currentDeviceAccount;
        var uuid;
        for ( uuid in mdx._remoteDeviceMap) {
        	var deviceInfo = {};
            deviceInfo.uuid = mdx._remoteDeviceMap[uuid].uuid;
            deviceInfo.usn = mdx._remoteDeviceMap[uuid].usn;
            deviceInfo.serviceType = mdx._remoteDeviceMap[uuid].serviceType;
            deviceInfo.activated = mdx._remoteDeviceMap[uuid].activated;
            deviceInfo.paired = mdx._remoteDeviceMap[uuid].paired;
            deviceInfo.fname = mdx._remoteDeviceMap[uuid].fname;
            deviceInfo.pairingContext = mdx._remoteDeviceMap[uuid].pairingContext;
            deviceInfo.loc = mdx._remoteDeviceMap[uuid].loc;
            deviceInfo.lastseen = mdx._remoteDeviceMap[uuid].lastseen;
            deviceInfo.controllersharedsecret = mdx._remoteDeviceMap[uuid].controllersharedsecret;
            deviceInfo.targetsharedsecret = mdx._remoteDeviceMap[uuid].targetsharedsecret;
			deviceInfo.controlleruserid = mdx._remoteDeviceMap[uuid].controlleruserid;
			deviceInfo.targetuserid = mdx._remoteDeviceMap[uuid].targetuserid;
            deviceMap[deviceInfo.uuid] = deviceInfo;
        }
        nrdp.storage.setItem(devAcctKey, mdx.device.DEVICEKEY, deviceMap);
    },

    retrieveDeviceMap: function() {

        var devAcctKey = nrdp.registration.currentDeviceAccount;
        var deviceMap = nrdp.storage.getItem(devAcctKey, mdx.device.DEVICEKEY);
        if (!deviceMap) {
        	mdx._remoteDeviceMap = {};
        	return;
        }
        var target;
        var uuid;
        for ( uuid in deviceMap) {
	        deviceMap[uuid].inrange = false;
	        deviceMap[uuid].ntbaContext = "";
	        deviceMap[uuid].pairingMatch = 0;
	        deviceMap[uuid].localSessionId = 0;
	        deviceMap[uuid].sessionId = 0;
	        deviceMap[uuid].msgQue = [];
	        deviceMap[uuid].sendQueue = [];
	        deviceMap[uuid].recvQueue = [];
	        deviceMap[uuid].inFlight = false;
	        deviceMap[uuid].xid = 0;
	        deviceMap[uuid].inFlightXid = 0;
	        deviceMap[uuid].msgObject = "";
	        deviceMap[uuid].hmac = "";
	        deviceMap[uuid].timer = {};
	        deviceMap[uuid].pairingTimer = undefined;
			// for anti-replay
	        deviceMap[uuid].sentList = [];
			deviceMap[uuid].recvdList = [];

        }
        mdx._remoteDeviceMap = deviceMap;
    }
};
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

mdx.discovery = {
    _serviceType: {mdx: "urn:mdx-netflix-com:service:target:1", dial: "urn:restful-tv-org:service:launcher:1"},
    _ALL:2,  _SAMEONLY:2,  _BOTH : 2,
    _selectors: {},
    _searchTimer: {},
    _searchSTTimer: {},

    addEventListener: function(evt, listener) { nrdp._addEventListener("mdx.discovery", evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener("mdx.discovery", evt, listener); },

    _selectorKey: function(serviceType, activationState, paired, account) {
        return serviceType + "_" + activationState + ":" + paired + ":" + account;
    },

   _createSelector : function(st, as, p) {
        // don't assign st, assume it is set as an input
        // st = st ? st : mdx.discovery._serviceType;
        as = as ? as : mdx.discovery._ALL;
        p = p ? p : mdx.discovery._ALL;
        var key = mdx.discovery._selectorKey(st, as, p);
        if (!mdx.discovery._selectors[key]) {
            var Selector = {};
            Selector.serviceType = st;
            Selector.activationState = mdx.discovery._ALL;
            Selector.paired = mdx.discovery._ALL;
            Selector.account = mdx.discovery._ALL;
            mdx.discovery._selectors[key] = { sel : Selector };
        }
        return mdx.discovery._selectors[key].sel;
    },

    doSTSearch: function(st) {
        mdx.log("doSTSearch: " + st);
        delete mdx.discovery._searchSTTimer[st];
        nrdp.mdx.DiscoverMdxDevices(st);
        nrdp.mdx.SearchForMdxDevices(st);
    },

    doMSearch: function() {
        // WARN: The current timeout is 30 seconds.  Right now we have a search timeout
        // value of 5 seconds for each service.  If we add more than 6 services
        // (not likely) we will have issues here
        var timeout = 0;
        for (var st in mdx.discovery._serviceType) {
            mdx.discovery._searchSTTimer[mdx.discovery._serviceType[st]] =
                setTimeout( function(st) {
                        mdx.discovery.doSTSearch(st);
                    }, timeout, mdx.discovery._serviceType[st] );
            timeout += 5000;
        }
    },

    _discover: function() {
        // for each service, do a discover
        nrdp.mdx.addEventListener("devicefound", mdx.discovery._discDispatcher);
        nrdp.mdx.addEventListener("devicebyebye", mdx.discovery._discDispatcher);
        var timeout = 0;
        for (var st in mdx.discovery._serviceType) {
            mdx.log("createSelector " + mdx.discovery._serviceType[st]);
            var sel = mdx.discovery._createSelector(mdx.discovery._serviceType[st]);
            mdx.discovery._searchSTTimer[mdx.discovery._serviceType[st]] =
                setTimeout( function(st) {
                        mdx.discovery.doSTSearch(st);
                    }, timeout, mdx.discovery._serviceType[st] );
            timeout += 5000;
        }
        mdx.discovery._searchTimer = setInterval(function(){mdx.discovery.doMSearch();}, 30000);
    },

    getSelector: function(key) {
        return ( _selectors[key] ? _selectors[key] : null );
    },

    getUUID : function(p) {
        var offset = p.indexOf(mdx._uuidToken);
        return (offset > -1) ? p.substr(offset + mdx._uuidToken.length) : "";
    },

    //Argument may be a selector or uuid
    getTargetList: function(p) {
        var target;
        var targetList = [];
        for ( target in mdx._remoteDeviceMap) {
                if ((Date.now() - mdx._remoteDeviceMap[target].lastseen) < 60000){
                    var device = {};
                    device.usn = mdx._remoteDeviceMap[target].usn;
                    device.uuid = mdx._remoteDeviceMap[target].uuid;
                    device.location = mdx._remoteDeviceMap[target].loc;
                    device.serviceType = mdx._remoteDeviceMap[target].serviceType;
                    device.friendlyName = mdx._remoteDeviceMap[target].fname;
                    targetList.push(device);
                }

        }
        return targetList;
    },

    _discDispatcher:function(evt) {
        mdx.log("_discDispatcher received message : " + evt.type);
        var uuid;
		var USN;
		var friendlyName;
        switch(evt.type) {
        case "devicefound":
            uuid = mdx.discovery.getUUID(evt.USN);
			friendlyName = mdx.guard.base64_decode(evt.friendlyName);

            var newEvent = {
                type: "devicefound",
                USN: evt.USN,
                UUID: uuid,
                activated:1,
                location: evt.location,
                serviceType: evt.serviceType,
                friendlyName: friendlyName
            };

            if (mdx._remoteDeviceMap[uuid]) {
                mdx._remoteDeviceMap[uuid].lastseen = Date.now();
                var loc = evt.location;
                if (loc && (loc.substr(0,5) == "http:")) {
		            mdx._remoteDeviceMap[uuid].loc = loc.substring(0, loc.indexOf(":", 5)) + ":9080/"; //TODO use the configured port
		        } else {
		            mdx._remoteDeviceMap[uuid].loc = "http://" + loc + ":9080/";
                }
                if (!mdx._remoteDeviceMap[uuid].inrange) {
                    mdx._remoteDeviceMap[uuid].inrange = true;
                    nrdp._callEventListeners("mdx.discovery", newEvent);
                }
				mdx._remoteDeviceMap[uuid].fname = friendlyName;
            } else {
                // mdx.discovery._targetList.push(evt.USN);
                mdx.device.addToRemoteDeviceMap(evt.location, evt.serviceType, evt.USN, uuid, 1, 0, friendlyName?friendlyName:"", "");
                nrdp._callEventListeners("mdx.discovery", newEvent);
            }

            break;
        case "devicebyebye":
            if (evt.USN.indexOf(mdx.discovery._serviceType) != -1) {
                uuid = mdx.discovery.getUUID(decodeURIComponent(evt.USN));
                uuid = uuid.substr(0,mdx.MYUUID.length);
                if (mdx._remoteDeviceMap[uuid])
                    mdx._remoteDeviceMap[uuid].inrange = false;
                nrdp._callEventListeners("mdx.discovery", evt);

            }

/* leadams@netflix.com - suppress bye bye events for now MDX-230
			USN = decodeURIComponent(evt.USN);
			for (var st in mdx.discovery._serviceType) {
				if (USN.indexOf(mdx.discovery._serviceType[st]) != -1) {
					uuid = mdx.discovery.getUUID(USN);
					uuid = uuid.substr(0,mdx.MYUUID.length);
					if (mdx._remoteDeviceMap[uuid])
						mdx._remoteDeviceMap[uuid].inrange = false;
					nrdp._callEventListeners("mdx.discovery", evt);
				}
			}
*/
            if (evt.USN.indexOf(mdx.discovery._serviceType) != -1) {
                uuid = mdx.discovery.getUUID(evt.USN);
                uuid = uuid.substr(0,mdx.MYUUID.length);
                if (mdx._remoteDeviceMap[uuid])
                    mdx._remoteDeviceMap[uuid].inrange = false;
                nrdp._callEventListeners("mdx.discovery", evt);

            }

            break;
        }
    },

    revealPresence: function() {
        if ( (mdx._role == mdx.TARGET ) || (mdx._role  == mdx.BOTH) ) {
            nrdp.mdx.RevealTargetPresence();
        } else {
            mdx.log("Invalid operation");
        }

    },

    hidePresence: function() {
        if ( (mdx._role == mdx.TARGET ) || (mdx._role  == mdx.BOTH) ) {
            nrdp.mdx.HideTargetPresence();
        } else {
            mdx.log("Invalid operation");
        }

    }

};
/*global
mdx,
console
*/

/*jslint
bitwise: false,
regexp: false,
confusion: false,
undef: false,
continue: false,
unparam: false,
debug: false,
sloppy: false,
eqeq: false,
sub: false,
es5: false,
vars: false,
evil: false,
white: false,
forin: false,
css: false,
newcap: false,
cap: false,
nomen: false,
on: false,
plusplus: false,
fragment: false,
maxerr: 50,
indent: 4
*/

/*members
ASCII,
action,
applicationCommandMap,
applicationMsg,
controllerurl,
createMessage,
createSession,
error,
errorcode,
errorCode,
errorstring,
errorString,
errorTypes,
filterMap,
fromurl,
fromuserid,
fromuuid,
getErrorValue,
guard,
internal_error,
localMgsAddress,
log,
logEnabled,
logVerbose,
malformed_envelope,
malformed_payload,
mdxCommandMap,
MYUUID,
nonce,
nvPairsDelim,
nvValueDelim,
pairify,
pairingrequest,
processIncomingMessage,
processIncomingMessageBody,
processOutgoingMessage,
processOutgoingMessageBody,
recreateMessage,
sendErrorMessage,
setCommandMap,
startSessionResponse,
stringToPairs,
touserid,
touuid,
typeOf,
unpairify,
URL,
UTF8,
validate,
verbose,
version
*/

mdx.guard = {
    nvPairsDelim: "\r\n",
    nvValueDelim: "=",
    verbose: true,
    logEnabled: true,
	enableAntiReplay: true,
	replayHistoryLength: 32,
	initialDelta: 60000, // 1 minute
    applicationCommandMap: {},
	isInitialized: false,
	isStrict: true,

	init: function () {
		if (!this.isInitialized) {
			mdx.guard.isInitialized = true;
			nrdp.mdx.addEventListener("incomingmessage", mdx.guard.handleIncomingMessage);
		}
	},

    setCommandMap: function (commandMap) {
        "use strict";
        this.logVerbose("Command Map set " + JSON.stringify(commandMap));
        this.applicationCommandMap = commandMap;
    },

    processOutgoingMessage: function (msg) {
        "use strict";
        this.logVerbose("processOutgoingMessage \n" + JSON.stringify(msg));

		if (this.enableAntiReplay) {
			var receiverDevice = mdx._remoteDeviceMap[this.getToUuid(msg)];
			if (receiverDevice) {
				this.addMessageToSentList(msg, receiverDevice);
			}
		}

        return this.pairify(msg, null, 0, 5);
    },

    processOutgoingMessageBody: function (messageBody) {
        "use strict";
		var nameValuePairs;
		this.nvPairsDelim = ",";
        nameValuePairs = this.pairify(messageBody, null, 0, 5);
        this.nvPairsDelim = "\r\n";
		this.logVerbose("processOutgoingMessageBody :\n" + nameValuePairs);
		return nameValuePairs;
    },

    utf8_decode: function(str_data) {
    // Converts a UTF-8 encoded string to ISO-8859-1
    //
    // version: 1109.2015
    // discuss at: http://phpjs.org/functions/utf8_decode
    // +   original by: Webtoolkit.info (http://www.webtoolkit.info/)
    // +      input by: Aman Gupta
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +   improved by: Norman "zEh" Fuchs
    // +   bugfixed by: hitwork
    // +   bugfixed by: Onno Marsman
    // +      input by: Brett Zamir (http://brett-zamir.me)
    // +   bugfixed by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // *     example 1: utf8_decode('Kevin van Zonneveld');
    // *     returns 1: 'Kevin van Zonneveld'
    var tmp_arr = [],
        i = 0,
        ac = 0,
        c1 = 0,
        c2 = 0,
        c3 = 0;

    str_data += '';

    while (i < str_data.length) {
        c1 = str_data.charCodeAt(i);
        if (c1 < 128) {
            tmp_arr[ac++] = String.fromCharCode(c1);
            i++;
        } else if (c1 > 191 && c1 < 224) {
            c2 = str_data.charCodeAt(i + 1);
            tmp_arr[ac++] = String.fromCharCode(((c1 & 31) << 6) | (c2 & 63));
            i += 2;
        } else {
            c2 = str_data.charCodeAt(i + 1);
            c3 = str_data.charCodeAt(i + 2);
            tmp_arr[ac++] = String.fromCharCode(((c1 & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
            i += 3;
        }
    }

    return tmp_arr.join('');
    },

    base64_decode: function(data) {
    // Decodes string using MIME base64 algorithm
    //
    // version: 1109.2015
    // discuss at: http://phpjs.org/functions/base64_decode
    // +   original by: Tyler Akins (http://rumkin.com)
    // +   improved by: Thunder.m
    // +      input by: Aman Gupta
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +   bugfixed by: Onno Marsman
    // +   bugfixed by: Pellentesque Malesuada
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +      input by: Brett Zamir (http://brett-zamir.me)
    // +   bugfixed by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // -    depends on: utf8_decode
    // *     example 1: base64_decode('S2V2aW4gdmFuIFpvbm5ldmVsZA==');
    // *     returns 1: 'Kevin van Zonneveld'
    // mozilla has this native
    // - but breaks in 2.0.0.12!
    //if (typeof this.window['btoa'] == 'function') {
    //    return btoa(data);
    //}
    var b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    var o1, o2, o3, h1, h2, h3, h4, bits, i = 0,
        ac = 0,
        dec = "",
        tmp_arr = [];

    if (!data) {
        return data;
    }

    data += '';

    do { // unpack four hexets into three octets using index points in b64
        h1 = b64.indexOf(data.charAt(i++));
        h2 = b64.indexOf(data.charAt(i++));
        h3 = b64.indexOf(data.charAt(i++));
        h4 = b64.indexOf(data.charAt(i++));

        bits = h1 << 18 | h2 << 12 | h3 << 6 | h4;

        o1 = bits >> 16 & 0xff;
        o2 = bits >> 8 & 0xff;
        o3 = bits & 0xff;

        if (h3 == 64) {
            tmp_arr[ac++] = String.fromCharCode(o1);
        } else if (h4 == 64) {
            tmp_arr[ac++] = String.fromCharCode(o1, o2);
        } else {
            tmp_arr[ac++] = String.fromCharCode(o1, o2, o3);
        }
    } while (i < data.length);

    dec = tmp_arr.join('');
    dec = mdx.guard.utf8_decode(dec);

    return dec;
    },

    processIncomingMessage: function (nvpairs) {
        "use strict";
        var msg, command, member;
    //    var nvpairs = mdx.guard.base64_decode(encodednvpairs);
        this.logVerbose("processIncomingMessage \n" + nvpairs);

        msg = this.unpairify(nvpairs);

        // extract the command name
        command = msg.action;

		// pairingresponse can take on two forms, differentiate here
		if (command == "pairingresponse") {
			if (msg.hasOwnProperty("errorstring") || msg.hasOwnProperty("errorcode")) {
				command = "pairingresponse_error";
			}
		}

		// if this isn't a command from the pairing or session protocols, it's an application message
		if (!this.mdxCommandMap.hasOwnProperty(command)) {
			command = "applicationMsg";
		}

        if (this.mdxCommandMap.hasOwnProperty(command)) {
			// to validate the envelope, iterate through the expected envelope members for this command
			for (member in this.mdxCommandMap[command]) {
				if (this.isStrict && !msg.hasOwnProperty(member)) {
					this.log("Command \'" + command + "\' is missing required field \'" + member + "\'");
					
					this.sendErrorMessage(msg, this.errorTypes.malformed_message);

					return null;
				} else {
					if (!this.validate(this.mdxCommandMap, command, member, msg[member], this.isStrict)) {
                        this.log("Command \'" + command + "\' failed validation of \'" + member + "\'");

                        this.sendErrorMessage(msg, this.errorTypes.malformed_message);

                        return null;
					}
				}
			}
			// now, iterate through the members of the message to ensure there are no extras
			for (member in msg) {
				if (member != "action") {
					if(this.isStrict && !this.mdxCommandMap[command].hasOwnProperty(member)) {
						this.log("Command \'" + command + "\' has extra field: \'" + member + "\'");

						this.sendErrorMessage(msg, this.errorTypes.malformed_message);

						return null;
					}
				}
			}
        } else {
			this.logVerbose("Internal error, unknown command: \'" + command + "\'");
			return null;
		}

        this.logVerbose("Command \'" + command + "\' passed validation");

		if (this.enableAntiReplay) {
			var senderDevice = mdx._remoteDeviceMap[this.getFromUuid(msg)];
			// we only perform anti-replay if we are paired with this device,
			// otherwise the messages may not be authentic
			if (senderDevice && senderDevice.pairingContext && senderDevice.pairingContext != "") {
				if (this.messageIsResponse(command)) {
					if (!this.checkMessageAgainstSent(msg, senderDevice)) {
						return null;
					}
				} else if (this.messageNeedsReplayCheck(command)) {
					if (!this.checkMessageAgainstRecvd(msg, senderDevice)) {
						return null;
					}
					this.logVerbose("Recvd: " + senderDevice.recvdList);
				} else {
					// this message does not get checked for replay. Messages in this
					// category are ones that happen before pairing is complete, since
					// we cannot trust non encrypted/HMAC'd messages at all
				}
			} else {
				this.logVerbose("Not paired with device " + this.getFromUuid(msg) + " not checking for replay");
			}
		}

        return msg;
    },

    processIncomingMessageBody: function (msg, plainText) {
        "use strict";
        var command, messageBody, param, errorValue, appMessageAction, appMessageBody;
        this.logVerbose("processIncomingMessageBody " + plainText);
        command = msg.action;
        this.nvPairsDelim = ",";
        messageBody = this.unpairify(plainText);
        this.nvPairsDelim = "\r\n";
        this.logVerbose("messageBody " + JSON.stringify(messageBody));

		if (messageBody.sessionAction === "appMessage") {
			appMessageAction = messageBody.appAction;
			appMessageBody = messageBody.appBody;
			for (param in appMessageBody) {
				if (appMessageBody.hasOwnProperty(param)) {
					if (!this.validate(this.applicationCommandMap, appMessageAction, param, appMessageBody[param], false)) {
						this.log("Command \'" + appMessageAction + "\' failed validation of \'" + param + "\'");

						this.sendErrorMessage(msg, this.errorTypes.malformed_message);

						return null;
					}
				}
			}
		}
        this.logVerbose("Command \'" + command + "\' passed validation");
        return messageBody;
    },

    pairify: function (obj, parent, level, limit) {
        "use strict";
        var objType, result, member, curParent, child;

        if (this.typeOf(limit) !== "number") {
            limit = 1;
        }

        objType = this.typeOf(obj);
        if (objType === "array") {
            if (level < limit) {
                for (member in obj) {
                    if (obj.hasOwnProperty(member)) {
                        curParent = parent ? [parent, "[", member, "]"].join('') : member;
                        child = this.pairify(obj[member], curParent, level + 1, limit);
                        result = [result, child].join('');
                    }
                }
            }
            return result;
        } else if (objType === "object") {
            if (level < limit) {
                for (member in obj) {
                    if (obj.hasOwnProperty(member)) {
                        curParent = parent ? [parent, member].join('.') : member;
                        child = this.pairify(obj[member], curParent, level + 1, limit);
                        result = [result, child].join('');
                    }
                }
            }
            return result;
        }
        return [parent, "=", obj, this.nvPairsDelim].join('');
    },

    recreateMessage: function (parent, flatName, value) {
        "use strict";
        var name, index, i, j;

        // is this name a property?
        i = flatName.indexOf('.');
        if (i > 0) {
            name = flatName.substring(0, i);
            if (!parent.hasOwnProperty(name)) {
                parent[name] = {};//new Object();
            }
            this.recreateMessage(parent[name], flatName.slice(i + 1), value);
        } else {
            // is this name an array?
            i = flatName.indexOf('[');
            if (i > 0) {
                j = flatName.indexOf(']');
                if (j > 0) {
                    name = flatName.substring(0, i);
                    index = parseInt(flatName.substr(i + 1, j - 1), 10);
                    if (!parent.hasOwnProperty(name)) {
                        parent[name] = [];
                    }
                    parent[name][index] = value;
                }
            } else {
                parent[flatName] = value;
            }
        }
    },

    unpairify: function (string) {
        "use strict";
        // split the string into an array of name/value pairs
        var pairs, msg, pair, name, value;

        pairs = this.stringToPairs(string);

        // recreate the object structure from the name/value pairs
        msg = {};
        for (pair in pairs) {
            if (pairs.hasOwnProperty(pair)) {
                name = pairs[pair][0];
                value = pairs[pair][1];
                this.recreateMessage(msg, name, value);
            }
        }

        return msg;
    },

    stringToPairs: function (string) {
        "use strict";
        var pairs, i, s, pair, valueDelimIndex;

        pairs = [];
        // look for the first pair
        i = string.indexOf(this.nvPairsDelim);
        while (i > 0) {
            s = string.substr(0, i);

            // do not use String.split in case the value itself has an '=' in it
            //pair = s.split('=');

            // instead, create the pair by just looking for the first '='
            // that delimits the name from the value
            pair = [];
            valueDelimIndex = s.indexOf(this.nvValueDelim);
            pair.push(s.slice(0, valueDelimIndex));
            pair.push(s.slice(valueDelimIndex + 1));
            pairs.push(pair);

            // look for the next name-value pair
            string = string.slice(i + this.nvPairsDelim.length);
            i = string.indexOf(this.nvPairsDelim);
        }
        return pairs;
    },

    typeOf: function (obj) {
        "use strict";
        if (obj instanceof Array) {
            return "array";
        }
        var type = typeof obj;
        return type === "object" && !obj ? "null" : type;
    },

    mdxCommandMap: {
        pairingrequest:  {
			version: "VERSION",
			nonce: "NUMBER",
            controllerurl: "URL",
			controlleruuid: "ASCII",
			pairdatahmac: "BASE64",
			cticket: "ASCII"
        },

		pairingresponse: {
			version: "VERSION",
			nonce: "NUMBER",
			controllersharedsecret: "ASCII",
			targetuuid: "ASCII",
			targetuserid: "ASCII",
			controlleruuid: "ASCII",
			controlleruserid: "ASCII",
			grantdatahmac: "BASE64"
		},

		pairingresponse_error: {
			version: "VERSION",
			nonce: "NUMBER",
			targetuuid: "ASCII",
            targetuserid: "ASCII",
			errorcode: "ASCII",
			errorstring: "ASCII"
		},

		pairingpoll: {
			version: "VERSION",
			nonce: "NUMBER",
			controlleruuid: "ASCII"
		},

		pairingdeleted: {
			controllersharedsecret: "ASCII",
			nonce: "NUMBER",
			senderuuid: "ASCII"
		},

        session: {
			version: "VERSION",
			nonce: "NUMBER",
            fromurl: "URL",
			fromuuid: "ASCII",
			fromuserid: "ASCII",
			touuid: "ASCII",
			touserid: "ASCII",
			ciphertext: "BASE64",
			hmac: "BASE64"
        },

        messagingerror: {
            version: "ASCII",
            fromurl: "URL",
            fromuuid: "ASCII",
            fromuserid: "ASCII",
            touuid: "ASCII",
            touserid: "ASCII",
            nonce: "NUMBER",
            errorcode: "ASCII",
            errorstring: "ASCII"
        },
    },

    validate: function (map, command, name, value, mustexist) {
        "use strict";
        var filterName;
		if (mustexist && value == "") {
			this.logVerbose("param \'" + name + "\' of \'" + command + "\' is empty");
			return false;
		}
        if (map.hasOwnProperty(command)) {
            if (map[command].hasOwnProperty(name)) {
                filterName = map[command][name];
                if (this.filterMap.hasOwnProperty(filterName)) {
                    return this.filterMap[filterName](value);
                } else {
                    this.logVerbose("Unknown filter type \'" + filterName + "\' for \'" + command + "\' parameter \'" + name + "\', using ASCII");
                }
			} else if (mustexist) {
				this.logVerbose("param \'" + name + "\' is unexpected.");
				return null;
            } else {
                this.logVerbose("param \'" + name + "\' not in filter map of \'" + command + "\', using ASCII");
            }
        }
        return this.filterMap.ASCII(value);
    },

	// anti-replay helpers
	historySort: function(a, b) { return a - b; },

	// if the msg is a non-error message, add it to our list of sent messages so
	// that if/when we get a response, we can check that the response matches a
	// request
	addMessageToSentList: function(msg, receiverDevice) {
		// we don't add outgoing error messages to the sent list
		if (msg.action != "error" &&
			msg.action != "messagingerror") {
			receiverDevice.sentList.push(parseInt(msg.nonce, 10));
			receiverDevice.sentList.sort(this.historySort);
			while (receiverDevice.sentList.length > this.replayHistoryLength) {
				receiverDevice.sentList.shift();
			}
			this.logVerbose("Sent: " + receiverDevice.sentList);
		}
	},

	// returns true if the message type is a response and should be checked
	// against our sent messages list
	// returns false if the message type is not a response
	messageIsResponse: function(command) {
		return command === "error" ||
			command === "messagingerror";
	},

	// checks the message against our list of messages sent to that device
	// returns true if the message is OK
	// returns false if the message is duplicate or unknown
	checkMessageAgainstSent: function(msg, senderDevice) {
		// check the nonce of this error/response message and
		// if it's not in our sent list, it's bogus
		var found = false;
		var sent;
		for (sent in senderDevice.sentList) {
			if (senderDevice.sentList[sent] == msg.nonce) {
				this.logVerbose("Got a response to a known message " + msg.nonce);
				// remove this message from the list in order to only
				// process/accept responses once
				senderDevice.sentList.splice(sent, 1);
				return true;
			}
		}

		this.log("Got response to unknown message, or already processed message " + msg.nonce);
		return false;
	},

	// returns true if the message should be checked for replay
	// returns false if the message should not be checked for replay
	messageNeedsReplayCheck: function(command) {		
		return command != "pairingrequest" &&
			command != "pairingresponse" &&
			command != "pairingresponse_error" &&
			command != "pairingpoll" &&
			command != "error" &&
			command != "messagingerror";
	},

	// returns true if the message is OK
	// returns false if the message is a replay,
	// suspiciously old, or suspiciously in the future
	checkMessageAgainstRecvd: function(msg, senderDevice) {
		var seen;
		if (senderDevice.recvdList.length > 0) {
			// if you're going to do comparisons, make sure they're numbers
			var msgNonce = parseInt(msg.nonce, 10);
			if (msgNonce < senderDevice.recvdList[0]) { // replay, very old
				this.log("Replay detected: " + msg.nonce + " older than " + senderDevice.recvdList[0]);
				this.sendErrorMessage(msg, this.errorTypes.duplicate_nonce);
				return false;
			} else {
				for (seen in senderDevice.recvdList) {
					if (senderDevice.recvdList[seen] == msg.nonce) { // replay, in recent history
						this.log("Replay detected: " + msg.nonce);
						this.sendErrorMessage(msg, this.errorTypes.duplicate_nonce);
						return false;
					}
				}
			}
		} else {
			// this is the first message we've seen, reject it if
			// it is outside our time +/- delta before accepting it
			//var localTime = nrdp.now();
			var localTime = Date.now();
			if(msg.nonce < localTime - this.initialDelta || msg.nonce > localTime + this.initialDelta) {
/* leadams@netflix.com: don't reject old/future initial messages yet, nrdp.now() is not implemented on all platforms and clocks may not be synchronized
				this.log("Replay detected: " + msg.nonce + " is too old or to far in the future. Local time = " + localTime);
				this.sendErrorMessage(msg, this.errorTypes.duplicate_nonce);
				return false;
*/
			}						
		}

		// the message is not a replay, put it in our received message list for this device
		senderDevice.recvdList.push(parseInt(msg.nonce, 10));
		senderDevice.recvdList.sort(this.historySort);
		while (senderDevice.recvdList.length > this.replayHistoryLength) {
			senderDevice.recvdList.shift();
		}

		return true;
	},

    // filter methods return true on pass, false on fail
    filterMap: {

		VERSION: function (value) {
			"use strict";
			var blacklist, format;
			blacklist = /[^0-9.]/;
			if (blacklist.test(value)) {
				return false;
			}
			format = /\d{1,}\.\d{1,}/;
			return value.match(format) == value;
		},

        // value must contain only ASCII printable characters
        ASCII: function (value) {
            "use strict";
            // the blacklist is those characters  NOT  in the whitelist (use of ^)
			// space is the first printable character, tilde is the last
            var blacklist = /[^\ -~]/;
            return !blacklist.test(value);
        },

        // value must contain only UTF-8 Unicode ranges U+0021 - U+007E and U+00C0 - U+0192
        UTF8: function (value) {
            "use strict";
            // the blacklist is those characters  NOT  in the whitelist (use of ^)
            var blacklist = /[^\u0021-\u007E\u00C0-\u0192]/;
            return !blacklist.test(value);
        },

        // value must contain only ASCII printable characters and '.', ':', '/', '&', '?', ';', and '_'
        URL: function (value) {
            "use strict";
            // the blacklist is those characters  NOT  in the whitelist (use of ^)
            var blacklist = /[^a-zA-Z0-9\.:\/\&?;_]/;
            return !blacklist.test(value);
        },

		// Base64 encoded values will only have A-Z, a-z, 0-9, +, /, and =
		BASE64: function (value) {
			"use strict";
			var blacklist = /[^A-Za-z0-9\+\/\=]/;
			if (mdx.guard.isStrict) {
				return !blacklist.test(value);
			} else {
				return this.ASCII(value);
			}
		},

		NUMBER: function (value) {
			"use strict";
			var blacklist, format;
			blacklist = /[^0-9\-]/;
			if (blacklist.test(value)) {
				return false;
			}
			// numbers accepted in the form of an optional unary minus followed by one or more digits
			format = /-?\d{1,}/;
			return value.match(format) == value;
		},
    },

    errorTypes : {
        unsupported_version : { errorCode : 1, errorString : "Unsupported protocol version" },
        duplicate_nonce : { errorCode : 4, errorString : "Duplicate NONCE" },
        malformed_message : { errorCode : 7, errorString : "Malformed message" },
        internal_error : { errorCode : 99, errorString : "Internal Error" }
    },

    getErrorValue: function (errorType) {
        "use strict";
        var retVal = {};

        if (this.errorTypes.hasOwnProperty(errorType)) {
            retVal.errorCode = this.errorTypes[errorType].errorCode;
            retVal.errorString = this.errorTypes[errorType].errorString;
        } else {
            retVal.errorCode = this.errorTypes.internal_error.errorCode;
            retVal.errorString = this.errorTypes.internal_error.errorString;
        }

        return retVal;
    },

	getFromUrl: function(msg) {
		if (msg.controllerurl) {
			return msg.controllerurl;
		} else if (msg.fromurl) {
			return msg.fromurl;
		} else {
			return null;
		}
	},

	getFromUuid: function(msg) {
		if (msg.action == "pairingrequest") {
			return msg.controlleruuid;
		} else if (msg.action == "pairingresponse") {
			return msg.targetuuid;
		} else if (msg.action == "pairingpoll") {
			return msg.controlleruuid;
		} else if (msg.action == "pairingdeleted") {
			return msg.senderuuid;
		} else {
			return msg.fromuuid;
		}
	},

	getToUuid: function(msg) {
		if (msg.action == "pairingresponse") {
			return msg.controlleruuid;
		} else {
			return msg.touuid;
		}
	},

	sendErrorMessageToUrl: function(toUrl, nonce, fromUuid, toUuid, errorValue) {
		var data = {};
		data.action = "error";
		data.nonce = nonce;
		data.fromuuid = fromUuid;
		data.touuid = toUuid;
		data.errorcode = errorValue.errorCode;
		data.errorstring = errorValue.errorString;

		var message = this.processOutgoingMessage(data);

		this.logVerbose("sendErrorMessage " + toUrl + data.action + " -> " + data.errorcode + " " + data.errorstring);

		var xid = mdx.nextTransactionId();
		nrdp.mdx.SendMdxHttpRequest(toUrl + data.action, 0, xid, "", message);
	},

	sendErrorMessageToUuid: function(toUuid, nonce, fromUuid, errorValue) {

		var toDevice = mdx._remoteDeviceMap[toUuid];

		if (toDevice) {
			this.sendErrorMessageToUrl(toDevice.loc, nonce, fromUuid, toUuid, errorValue);
		} else {
			this.log("Device " + toUuid + " not known, cannot send error message");
		}
	},

    sendErrorMessage: function (msg, errorValue) {
        "use strict";
		var toUrl, toUuid, fromUuid, message;

		toUrl = this.getFromUrl(msg);
		toUuid = this.getFromUuid(msg);
		fromUuid = mdx.MYUUID;

        if (toUrl) {
			this.sendErrorMessageToUrl("http://" + toUrl + ":9080/", msg.nonce, fromUuid, toUuid, errorValue);
		} else if (toUuid) {
			this.sendErrorMessageToUuid(toUuid, msg.nonce, fromUuid, errorValue);
        } else {
			this.log("Unknown sender of message, cannot send error message");
		}
    },

	handleIncomingMessage: function(event) {
        if (event.type === "incomingmessage") {
            var request = {};
            request.info = mdx._getUriInfo(event.url, event.headers);
            if (request.info.path == "error") {
				var nvpairs = event.body;
				// we can't really do anything with error messages, what if they
				// fail the guard as well? we would end up ping ponging back and forth
				mdx.guard.log("Error message received: " + nvpairs);
			}
		}
	},

    // utility
    logVerbose: function (txt) {
        "use strict";
        if (this.verbose) {
            this.log(txt);
        }
    },

    log: function (txt) {
        "use strict";
        if (this.logEnabled) {
            console.log("----------------\nmdx.guard: \n" + txt + "\n----------------");
        }
    }
};
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

mdx.pair = {
    PAIRINGTIMEOUT: 30000,
    _dummyPairing:false,
    _isInitialized:false,
    _pairErrorMap : {  1: "UNSUPPORTED VERSION",
                       10: "INVALID CONTROLLER REQUEST",
                       11: "SERVER ERROR",
                       20: "TARGET ERROR",
                       21: "SERVER NOT REACHABLE",
                       20: "CONTROLLER REQUEST HMAC_FAILURE",
                       21: "CONTROLLER CTICKET EXPIRED",
                       22: "CONTROLLER_CTICKET_CORRUPTED"},
    _pairingVersion: "1.0",
    ERRORS : {       MDX_PAIRING_UNKNOWN_VERSION : 10,
                     MDX_PAIRING_SERVER_ERROR : 11,
                     MDX_PAIRING_TARGET_ERROR  : 12,
                     MDX_PAIRING_SERVER_NOT_REACHABLE  : 13,
                     MDX_PAIRING_CONTROLLER_HMAC_FAILURE  : 20,
                     MDX_PAIRING_CONTROLLER_CTICKET_EXPIRED  : 21,
                     MDX_PAIRING_CONTROLLER_CTICKET_CORRUPTED  : 22,
                     MDX_PAIRING_ALREADY_PAIRED : 31,
                     MDX_PAIRING_NETWORK_ERROR : 99
    },
    addEventListener: function(evt, listener) { nrdp._addEventListener("mdx.pair", evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener("mdx.pair", evt, listener); },

    _init: function() {
        if (!mdx.pair._isInitialized) {
            mdx.pair._isInitialized = true;
            nrdp.mdx.addEventListener("incomingmessage", mdx.pair._handleIncomingMessage);
        }
    },

    pairingRequest: function(remoteDevice) {
        mdx.log("mdx.pair.pairingRequest(" + remoteDevice + ")");

        if ( !( (mdx._role == mdx.CONTROLLER) || (mdx._role == mdx.BOTH))) {
            mdx.log("Invalid operation");
            return;
        }
        mdx.pair._init();
        var myTarget = mdx._remoteDeviceMap[remoteDevice];
        if (!myTarget) {
            mdx.log("remoteDevice not found");
            return;
        }
        nrdp.registration.ping();
        if (myTarget.pairingContext != "") {
            mdx.log("pairing already exists. Please depair and pair again.");
            var grant = {};
            grant.type = "pairingresponse";
            grant.result = nrdp.COMPLETE;
            grant.pairingContext = myTarget.pairingContext;
            grant.errorCode = 31;
            grant.errorDesc = "pairing already exists";
            grant.match = "";
            grant.remoteDevice = myTarget.uuid;
            nrdp._callEventListeners("mdx.pair", grant);
            return;
        }
        myTarget.targetPairingInProgress = true;
        if (mdx.pair._dummyPairing) {
            myTarget.xid = mdx.nextTransactionId();
            mdx.pair._pairReqHmacCb("dummy", myTarget);
        } else {
            nrdp.ntba.getCticket(function(cticket) {
                                           mdx.pair._cticketCb(cticket, myTarget);
                                       });
        }
    },

    _cticketCb: function(cticket, myTarget) {
        mdx.log("mdx.pair._cticketCb(" + cticket.cticket + ")");
        if (cticket.cticket == "") {
            mdx.log("invalid cticket");
            return;
        }
        mdx._CTICKET = cticket.cticket;
        nrdp.ntba.beginTransaction(nrdp.registration.currentDeviceAccount, "read",
                           function(ctx) {
                               mdx.pair._beginTransactionCb(ctx, myTarget);
                           });

    },

    _beginTransactionCb: function(ctx, myTarget) {
        mdx.log("mdx.pair._beginTransactionCb(" + ctx.context + ")");
        if (ctx && ctx.context) {
            myTarget.targetPairingContext = ctx.context;
            myTarget.xid = mdx.nextTransactionId();
            var data = "controlleruuid=" + encodeURIComponent(mdx.MYUUID) + "&nonce=" + encodeURIComponent(myTarget.xid);
            mdx.log("mdx.pair._beginTransactioCb calling nrdp.ntba.hmac(" + ctx.context + ", " + data + ")");
            nrdp.ntba.hmac(ctx.context, data,
                           function(hmacStr) {
                               mdx.pair._pairReqHmacCb(hmacStr, myTarget);
                           });
        } else {
            mdx.log("beginning custom context failed");
            delete myTarget.targetPairingInProgress;
        }
    },

    _pairReqHmacCb: function(hmacStr, myTarget) {
        mdx.log("mdx.pair._pairReqHmacCb(" + hmacStr + ")");
        mdx._bothMode = mdx.CONTROLLER; // TODO remove this after BOTH mode goes away
        var data = {};
        data.action = "pairingrequest";
        data.version = mdx.pair._pairingVersion;
        data.controllerurl = nrdp.mdx.localIPAddress;
        data.nonce = myTarget.xid;
        data.controlleruuid = mdx.MYUUID;
        data.pairdatahmac = hmacStr;
        data.nonce = myTarget.xid; // TODO to be removed
        data.cticket = mdx._CTICKET;

        mdx.createMessage("pairingrequest", data, myTarget.uuid);
    },

    _handlePairingResponse: function(messageBody) {

        if (messageBody.version != mdx.pair._pairingVersion) {
            mdx.log("Incorrect pairing protocol version. Expected " + mdx.pair._pairingVersion + " but got " + messageBody.version);
            nrdp.log.error("Incorrect pairing protocol version. Expected " + mdx.pair._pairingVersion + " but got " + messageBody.version, "MDX");

            // leadams@netflix.com - decided not to send an error to responder
        }

        var myTarget = mdx._remoteDeviceMap[messageBody.targetuuid];
        if (!myTarget) {
            mdx.log("Received pairingresponse for unknown device");
            return;
        }

        if (!myTarget.targetPairingInProgress) {
            mdx.log("Received pairingresponse for unexpected device");
            return;
        }

        myTarget.controlleruserid = messageBody.controlleruserid;
        myTarget.targetuserid = messageBody.targetuserid;

        if (mdx.pair._dummyPairing) {
            delete myTarget.targetPairingInProgress;
            myTarget.pairingContext = messageBody.controllersharedsecret;
            var grant = {};
            grant.type = "pairingresponse";
            grant.result = nrdp.COMPLETE;
            grant.pairingContext = messageBody.controllersharedsecret;
            grant.errorCode = messageBody.errorcode;
            grant.errorDesc = messageBody.errorstring;
            grant.match = 1;
            grant.remoteDevice = messageBody.targetuuid;
            // store the pairing context in remote device map
            nrdp._callEventListeners("mdx.pair", grant);
            return;
        }

        if (messageBody.errorcode) {
            delete myTarget.targetPairingInProgress;
            // pairing failed, send error to app
            var grant = {};
            grant.type = "pairingresponse";
            grant.result = nrdp.COMPLETE;
            grant.pairingContext = "";
            grant.errorCode = messageBody.errorcode;
            grant.errorDesc = messageBody.errorstring;
            grant.match = 0;
            nrdp._callEventListeners("mdx.pair", grant);
            return;
        }

        // store the pairing context in remote device map

        //TODO check nonce and see if it matches
        if (messageBody.nonce != myTarget.xid) {
            mdx.log("pairing responce nonce doesn't match");
            return;
        }

        var tempStr =
            "controllersharedsecret=" + encodeURIComponent(messageBody.controllersharedsecret) +
            "&controlleruserid=" + encodeURIComponent(messageBody.controlleruserid) +
            "&controlleruuid=" + encodeURIComponent(messageBody.controlleruuid) +
            "&nonce=" + encodeURIComponent(messageBody.nonce) +
            "&targetuserid=" + encodeURIComponent(messageBody.targetuserid) +
            "&targetuuid=" + encodeURIComponent(messageBody.targetuuid); // everything in xsd schema

        nrdp.ntba.hmac(myTarget.targetPairingContext, tempStr,
                       function(hmacStr) {
                           mdx.pair._pairRespHmacCb(hmacStr, messageBody, myTarget);
                       });
    },

    _pairRespHmacCb: function(hmacStr, messageBody, myTarget) {
        // check if hmacStr is same as messageBody.hmac;
        if (messageBody.grantdatahmac != hmacStr) {
            mdx.log("invalid hmac string in pairing response");
            return;
        }

        nrdp.ntba.decrypt(myTarget.targetPairingContext,
                          messageBody.controllersharedsecret,
                          function(dec) {
                              mdx.pair._pairRespDecryptCb(dec, messageBody, myTarget);
                          });
    },

    _pairRespDecryptCb: function(secret, messageBody, myTarget) {
        nrdp.ntba.endTransaction(myTarget.targetPairingContext);
        delete myTarget.targetPairingContext;
        delete myTarget.targetPairingInProgress;

        myTarget.controllersharedsecret = secret;
        myTarget.pairingContext = messageBody.grantdatahmac;
        var grant = {};
        grant.type = "pairingresponse";
        grant.result = nrdp.COMPLETE;
        grant.pairingContext = messageBody.grantdatahmac;
        grant.errorCode = 0;
        grant.errorDesc = "";
        grant.match = (messageBody.controlleruserid == messageBody.targetuserid) ? 1 : 0;
        grant.remoteDevice = messageBody.targetuuid;
        nrdp._callEventListeners("mdx.pair", grant);
        mdx.device.storeDeviceMap();
    },

    _handlePairingRequest: function(messageBody) {

        if (messageBody.version != mdx.pair._pairingVersion) {
            mdx.log("Incorrect pairing protocol version. Expected " + mdx.pair._pairingVersion + " but got " + messageBody.version);
            nrdp.log.error("Incorrect pairing protocol version. Expected " + mdx.pair._pairingVersion + " but got " + messagebody.version, "MDX");

            mdx.pair.sendPairingReject(messageBody.controlleruuid, {errorcode: mdx.pair.ERRORS.MDX_PAIRING_UNKNOWN_VERSION,
                                                                    errormesg: "MDX PAIRING UNKNOWN VERSION Got: " + messageBody.version + " expected: " + mdx.pair._pairingVersion});

            return;
        }

        var myController = mdx._remoteDeviceMap[messageBody.controlleruuid];
        if (myController) {
            // we already know this controller
            // we need to update the controller url if necessary
            myController.loc = "http://" + messageBody.controllerurl + ":9080/";
        } else {
            myController = mdx.device.addToRemoteDeviceMap(messageBody.controllerurl, "",
                                                           messageBody.controlleruuid,
                                                           messageBody.controlleruuid,
                                                           1, 0, "", "");
        }

        myController.pairingInProgress = true;
        myController.pairingRequestMessage = messageBody;
        // record the nonce for use in the response message
        myController.pairingResponseNonce = messageBody.nonce;
        var myeventdata = {};
        myeventdata.type = "pairingrequestreceived";
        myeventdata.uuid = messageBody.controlleruuid;
        nrdp._callEventListeners("mdx.pair", myeventdata);
    },

    allowPairing: function(remoteDevice) {
        mdx.log("allowPairing");
        if ( !( (mdx._role == mdx.TARGET) || (mdx._role == mdx.BOTH))) {
            mdx.log("Invalid operation");
            return;
        }
        var myController = mdx._remoteDeviceMap[remoteDevice];
        if (!myController.pairingInProgress || !myController.pairingRequestMessage) {
            mdx.log("invalid uuid passed for allowPairing");
            return;
        }
        if (mdx.pair._dummyPairing) {
            setTimeout(mdx.pair._sendTimedGrant(myController), 10000);
        } else {
            var reqMsg = myController.pairingRequestMessage;
            var uuid_c = reqMsg.controlleruuid;
            if (myController.pairingTimer) {
                // There is already a pairing request going on with this uuid
                // abort the request and create a new one
                mdx.pair._abortPairing(uuid_c, myController, false);
            }
            nrdp.mdxnccp.pair(reqMsg.cticket,
                              uuid_c,
                              reqMsg.pairdatahmac,
                              reqMsg.nonce,
                              mdx.MYUUID,
                              function(grantList) {
                                  mdx.pair._nccpPairingGrant(grantList, myController);
                              });
            myController.pairingInProgress = true;
            myController.pairingTimer = setTimeout(function() {
                                                       mdx.pair._abortPairing(uuid_c, myController, true);
                                                   }, mdx.pair.PAIRINGTIMEOUT);

        }
    },

    _sendTimedGrant: function (myController) {
        myController.pairingContext = myController.uuid.substr((myController.uuid.length - 4)) + mdx.MYUUID.substr( (mdx.MYUUID.length - 4)); //should be myuuid
        var grant = {};
        grant.type = "pairinggrant";
        grant.result = nrdp.COMPLETE;
        grant.pairingContext = myController.pairingContext;
        grant.actionID = 0;
        grant.message = "";
        grant.bcp47 = 0;
        grant.reasonCode = 0;
        grant.controlleruuid = myController.uuid;
        grant.match = 1;
        nrdp._callEventListeners("mdx.pair", grant);
    },

    _abortPairing: function(uuid_c, myController, timedOut) {
        if (myController.pairingTimer !== undefined) {
            // abort previous request
            clearTimeout(myController.pairingTimer);
            delete myController.pairingTimer;
            nrdp.mdxnccp.abort(uuid_c);
            nrdp.mdxnccp.finish(uuid_c);

            // notify the target
            if (timedOut)
                mdx.pair._nccpPairingGrant([{errorcode: 13, controlleruuid: uuid_c}], myController);
        }
    },

    _nccpPairingGrant: function(grantList, myController) {
        // grantList is a list of objects, the complete object is always the last.
        var grant = undefined;
        var lastGrantConsidered = undefined;
        while (grantList.length > 0) {
            lastGrantConsidered = grantList.pop();

            // now check if the current object contains grant data.
            if (lastGrantConsidered.data && lastGrantConsidered.data.controlleruuid) {
                // yes, we are done here.
                grant = lastGrantConsidered.data;
                break;
            } else {
                // is this an error message?
                if ((grant === undefined || (grant.result !== nrdp.NETWORK_ERROR)) &&
                    lastGrantConsidered.actionID || lastGrantConsidered.errorcode) {
                    grant = lastGrantConsidered;
                } else if (lastGrantConsidered.result === nrdp.NETWORK_ERROR) {
                    grant = lastGrantConsidered;
                }
            }
        }

        if (grant && grant.aborted) {
            return;
        }

        nrdp.mdxnccp.finish(myController.uuid);

        if (myController.pairingTimer !== undefined) {
            clearTimeout(myController.pairingTimer);
            delete myController.pairingTimer;
        }

        if (!myController.pairingInProgress || !myController.pairingRequestMessage) {
            mdx.log("Target not in pairing state- ignoring the pairing grant");
            return;
        }

        if ( grant === undefined || grant.result === nrdp.NETWORK_ERROR) {
            // grant.controlleruuid is the controller uuid
            // grant.endpoint is the NCCP url
            // grant.origin is "reportNetworkingError"
            // grant.result is "NETWORK_ERROR"
            // grant.stack is a string listing more detailed information about the error
            //   i.e. "(NFErr_NCCPRequestFailed:transaction=mdxpair,endpoint=https://nccp-nrdp-31.cloud.netflix.net:8443/nccp/controller),"
            //        "(NFErr_NetworkError),(NFErr_ConnRefused),(NFErr_SysError:errno=111)"
            // grant.transaction is "mdxpair"
            // grant.type is "pairing"
            var uigrant = {};
            uigrant.type = "pairinggrant";
            uigrant.result = nrdp.NETWORK_ERROR;
            uigrant.pairingContext = "";
            uigrant.actionID = grant?(grant.actionID ? grant.actionID : 0):0;
            uigrant.message = grant?(grant.message ? grant.message : ""):"";
            uigrant.bcp47 = grant?(grant.bcp47 ? grant.bcp47 : 0):0;
            uigrant.reasonCode = grant?(grant.reasonCode ? grant.reasonCode : 0):0;
            uigrant.controlleruuid = myController.uuid;
            uigrant.controllerErrCode = 13; //TODO get the right error code
            uigrant.controllerErrMesg = "Target not reachable";
            nrdp._callEventListeners("mdx.pair", uigrant);
            nrdp.log.error("NCCP pairing network error. Target not reachable", "MDX");
            return;
        }

        var uigrant = {};
        uigrant.type = "pairinggrant";
        uigrant.result = nrdp.COMPLETE;
        uigrant.pairingContext = grant.grantdatahmac ? grant.grantdatahmac : "";
        uigrant.actionID = grant.actionID ? grant.actionID : 0;
        uigrant.message = grant.message ? grant.message : "";
        uigrant.bcp47 = grant.bcp47 ? grant.bcp47 : 0;
        uigrant.reasonCode = grant.reasonCode ? grant.reasonCode : 0;
        uigrant.controlleruuid = grant.controlleruuid;
        uigrant.controllerErrCode = grant.errorcode ? grant.errorcode : 0;
        uigrant.controllerErrMesg = grant.errorcode ? mdx.pair._pairErrorMap[grant.errorcode] : "";

        // Does our grant object represent an NCCP error message or an mdxpair error message?
        if (grant.actionID || grant.errorcode)
            uigrant.match = 0;
        else {
            // No, assume everything is fine
            uigrant.match = ((grant.controlleruserid == grant.targetuserid) ? 1 : 0);
            myController.grantdatahmac = grant.grantdatahmac;
            myController.pairingContext = grant.grantdatahmac;
            myController.controllersharedsecret = grant.controllersharedsecret;
            myController.controlleruserid = grant.controlleruserid;
            myController.targetsharedsecret = grant.targetsharedsecret;
            myController.targetuserid = grant.targetuserid;
        }

        nrdp._callEventListeners("mdx.pair", uigrant);
        delete myController.pairingRequestMessage;
    },

    sendPairingGrant: function(pairingContext) {
        if ( !( (mdx._role == mdx.TARGET) || (mdx._role == mdx.BOTH))) {
            mdx.log("Invalid operation");
            return;
        }
        var myController = mdx.device.getPairedDevice(pairingContext);
        if (!myController) {
            mdx.log("not a valid pairing context");
            return;
        }
        var data = {};
        data.action = "pairingresponse";
        data.version = mdx.pair._pairingVersion;
        if (mdx.pair._dummyPairing) {
            data.controllersharedsecret = myController.pairingContext;
            data.nonce = myController.pairingResponseNonce;
            myController.xid = data.nonce;
            data.controlleruuid = myController.uuid;
            data.targetuuid = mdx.MYUUID;
            data.controlleruserid = myController.uuid;
            data.targetuserid = mdx.MYUUID;
            data.grantdatahmac = "dummy" ;
        } else {
            data.controllersharedsecret = myController.controllersharedsecret;
            data.nonce = myController.pairingResponseNonce;
            myController.xid = data.nonce;
            data.controlleruuid = myController.uuid;
            data.targetuuid = mdx.MYUUID;
            data.controlleruserid = myController.controlleruserid;
            data.targetuserid = myController.targetuserid ? myController.targetuserid : mdx.MYUUID;
            data.grantdatahmac = myController.grantdatahmac ;
        }
        delete myController.pairingResponseNonce;
        mdx.log("pairingresponse");
        mdx.createMessage("pairingresponse", data, myController.uuid);
        delete myController.pairingInProgress;
        if (myController.ntbaContext) {
            nrdp.ntba.endCustomContext(this.ntbaContext);
            delete myController.ntbaContext;
        }
        mdx.device.storeDeviceMap();
    },

    sendPairingReject: function(remoteDevice, errorObject) {
        if ( !( (mdx._role == mdx.TARGET) || (mdx._role == mdx.BOTH))) {
            mdx.log("Invalid operation");
            return;
        }
        var myController = mdx._remoteDeviceMap[remoteDevice];
        if (!myController) {
            mdx.log("remoteDevice not found");
            return;
        }
        mdx.log("sendPairingReject");
        var data = {};
        data.action = "pairingresponse";
        data.version = mdx.pair._pairingVersion;
        data.nonce = myController.pairingResponseNonce;
        myController.xid = data.nonce;
        data.targetuuid = mdx.MYUUID;
        data.targetuserid = myController.targetuserid ? myController.targetuserid : mdx.MYUUID;
        data.errorcode = errorObject.errorcode?errorObject.errorcode:1000;
        data.errorstring = (errorObject.errormesg)?errorObject.errormesg:"Error Message was not passed by caller of sendPairingReject";
        mdx.createMessage("pairingresponse", data, myController.uuid);
        delete myController.pairingInProgress;
        delete myController.pairingResponseNonce;
    },

    dePair: function(pairingContext) {
        var myTarget = mdx.device.getPairedDevice(pairingContext);
        if (!myTarget) {
            mdx.log("not a valid pairing context");
            return;
        }
        var data = {};
        data.action = "pairingdeleted";
        data.controllersharedsecret = myTarget.pairingContext;
        data.nonce = mdx.nextTransactionId();
        myTarget.xid = data.nonce;
        data.senderuuid = mdx.MYUUID;
        myTarget.pairingContext = "";
        mdx.log("pairingdeleted");
        mdx.createMessage("pairingdeleted", data, myTarget.uuid);
        if (myTarget.sessionId != 0 ) {
            myTarget.sessionId = 0;
        }
        mdx.device.storeDeviceMap();
        delete myTarget.targetPairingInProgress;

    },
    _sendNetworkErrorEvent: function(uuid) {
        var grant = {};
            grant.type = "pairingresponse";
            grant.result = nrdp.NETWORK_ERROR;
            grant.pairingContext = "";
            grant.errorCode = mdx.pair.ERRORS.MDX_PAIRING_NETWORK_ERROR;
            grant.errorDesc = "send failed, networking error";
            grant.match = "";
            grant.remoteDevice = uuid;
        nrdp._callEventListeners("mdx.pair", grant);
    },
    _handleIncomingMessage: function(evt) {
        var request = mdx._getUriInfo(evt.url, evt.headers);
        //       request.cmd = mdximpl.cfg.cmds[request.path];
        if (request.path == "pairingrequest") {
            var messageBody = mdx.guard.processIncomingMessage(evt.body);
            if (!messageBody) return;
            mdx.pair._handlePairingRequest(messageBody);
        } else if (request.path == "pairingresponse") {
            var messageBody = mdx.guard.processIncomingMessage(evt.body);
            if (!messageBody) return;
            mdx.pair._handlePairingResponse(messageBody);
        } else if (request.path == "pairingdeleted") {
            var messageBody = mdx.guard.processIncomingMessage(evt.body);
            if (!messageBody) return;
            var myTarget = mdx._remoteDeviceMap[messageBody.senderuuid];
            myTarget.sessionId = 0;
            var myeventdata = {};
            myeventdata.type = "pairingdeleted";
            myeventdata.uuid = messageBody.senderuuid;
            myeventdata.pairingContext = myTarget.pairingContext;
            myTarget.pairingContext = "";
            nrdp._callEventListeners("mdx.pair", myeventdata);
        }    //todo handle pairing error response
    }

};
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

mdx.session = {
    _messageListeners: {},
    _pairingData: {},
    _isInitialized:false,
    _targetSessionId:0,
    _localSessionId:0,
    _deviceInMessagingState:null,
    _sessionMsgbody:null,
    _sessionVersion: "1.0",
    ERRORS : {       MDX_SESSION_UNKNOWN_VERSION : 1,
                     MDX_SESSION_UNKNOWN_SENDER : 2,
                     MDX_SESSION_UNKNOWN_RECEIVER  : 3,
                     MDX_SESSION_INVALID_NONCE  : 4,
                     MDX_SESSION_INVALID_HMAC  : 5,
                     MDX_SESSION_DECRYPTION_FAILED  : 6,
                     MDX_SESSION_NETWORK_ERROR  : 10,
                     MDX_SESSION_INVALID_SID : 11
             },

    addEventListener: function(evt, listener) { nrdp._addEventListener("mdx.session", evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener("mdx.session", evt, listener); },

    addMessageListener: function(msgType, callback) { mdx.session._addMessageListener("mdx.session", msgType, callback); },
    removeMesageListener: function(msgType, callback) { mdx.session._removeMessageListener("mdx.session", msgType, callback); },

    _init: function() {
        if (!mdx.session._isInitialized) {
            mdx.session._isInitialized = true;
            nrdp.mdx.addEventListener("httpresponse", mdx.session._handleHttpResponse);
            nrdp.mdx.addEventListener("incomingmessage", mdx.session._handleIncomingMessage);
        }
    },

    _clearAllSessions: function() {
        for (target in mdx._remoteDeviceMap) {
            mdx._remoteDeviceMap[target].sessionId = 0;
            mdx._remoteDeviceMap[target].localSessionId = 0;
            mdx._remoteDeviceMap[target].msgQue = [];
        }
    },

    _sendSessionErrorMessage: function(touuid, nonce, errorcode, errorstring) {

        var data = {};
        data.action = "messagingerror";
        data.nonce = nonce;
        data.version = mdx.session._sessionVersion;
        data.touuid = touuid;
        data.touserid = touuid;
        data.fromurl = nrdp.mdx.localIPAddress;
        data.fromuserid = mdx.MYUUID;
        data.fromuuid = mdx.MYUUID;
        data.errorcode = errorcode;
        data.errorstring = errorstring;

        mdx.createMessage("session", data, touuid);
    },

    // error messages sent directly to url do not get hmac'd or encrypted
    sendErrorMessageToUrl: function(toUrl, nonce, fromUuid, toUuid, errorValue) {
        var data = {};
        data.action = "messagingerror";
        data.nonce = nonce;
        data.version = mdx.session._sessionVersion;
        data.fromuuid = fromUuid;
        data.fromuserid = fromUuid;
        data.touuid = toUuid;
        data.touserid = toUuid;
        data.fromurl = nrdp.mdx.localIPAddress;
        data.fromuuid = mdx.MYUUID;
        data.errorcode = errorValue.errorcode;
        data.errorstring = errorValue.errorstring;

        var message = mdx.guard.processOutgoingMessage(data);

        var xid = mdx.nextTransactionId();
        nrdp.mdx.SendMdxHttpRequest(toUrl + "session", 0, xid, "", message);
    },

    startSession: function(pairingContext) {
        if ( !( (mdx._role == mdx.CONTROLLER) || (mdx._role == mdx.BOTH))) {
            mdx.log("Invalid operation");
            return;
        }
        mdx.session._init();
        var myTarget = mdx.device.getPairedDevice(pairingContext);
        if (!myTarget) {
            mdx.log("pairing context \'" + pairingContext + "\' does not exist");
            return;
        }
        if (myTarget.sessionId != 0 ) {
            myTarget.sessionId = 0;
            myTarget.localSessionId = 0;
        }

        if (mdx.pair._dummyPairing) {
            mdx.session._controllerSessionContextCb({context: "<dummycontext>"}, myTarget);
        } else {
            nrdp.ntba.beginCustomContext(myTarget.controllersharedsecret, "",
                                         function(ctx) {
                                             mdx.session._controllerSessionContextCb(ctx, myTarget);
                                         });
        }
    },

    _controllerSessionContextCb: function(context, myTarget) {
        myTarget.ntbaContext = context.context;

        var createSessionMessage = {
            sessionAction: "createSession"
        };

        // Sending step 1. create the plaintext version of the message
        var plainText = mdx.guard.processOutgoingMessageBody(createSessionMessage);

        var xid = mdx.nextTransactionId()

        // Sending step 2. queue the plaintext for encryption
        mdx.session._queueOutgoingMessage(myTarget, xid, plainText);
    },

    _handleCreateSession: function(fromDevice) {
        // todo if my pairing context = remote pairing context

        mdx.session._targetSessionId++;

        // Fire the sessionstart JSAPI event
        var myeventdata = {};
        myeventdata.type = "sessionstart";
        myeventdata.pairingContext = fromDevice.pairingContext;
        myeventdata.sid = mdx.session._targetSessionId;
        fromDevice.localSessionId = mdx.session._targetSessionId;
        fromDevice.sessionId = mdx.session._targetSessionId;
        nrdp._callEventListeners("mdx.session", myeventdata);

        var startSessionResponseMessage = {
            sessionAction: "startSessionResponse",
            sessionId: mdx.session._targetSessionId
        };

        // Sending step 1. create the plaintext version of the message
        var plainText = mdx.guard.processOutgoingMessageBody(startSessionResponseMessage);

        var xid = mdx.nextTransactionId();

        // Sending step 2. queue the plaintext for encryption
        mdx.session._queueOutgoingMessage(fromDevice, xid, plainText);
    },

    _handleStartSessionResponse: function(fromDevice, sessionId) {
        // TODO check if xid matches. to document if session id is 0, then error
        fromDevice.sessionId = sessionId;
        fromDevice.localSessionId = ++mdx.session._localSessionId;
        var sessiondata = {};
        sessiondata.type = "startSessionResponse";
        sessiondata.sid = mdx.session._localSessionId;
        sessiondata.pairingContext = fromDevice.pairingContext;
        nrdp._callEventListeners("mdx.session", sessiondata);
    },

    endSession: function(sid) {
        var target;
        if ( (mdx._role == mdx.CONTROLLER || mdx._bothMode == mdx.CONTROLLER) )
            target = mdx.device.getRemoteDevicefromLocalSid(sid);
        else
            target = mdx.device.getRemoteDevicefromSid(sid);
        if (!target) {
            mdx.log("sessionId \'" + sid + "\' does not exist");
            return;
        }
        if (sid != 0) {

            var endSessionMessage = {
                sessionAction: "endSession",
                sessionId: target.sessionId
            };

            // Sending step 1. create the plaintext version of the message
            var plainText = mdx.guard.processOutgoingMessageBody(endSessionMessage);

            var xid = mdx.nextTransactionId();

            // Sending step 2. queue the plaintext for encryption
            mdx.session._queueOutgoingMessage(target, xid, plainText,
                                              function() {
                                                  // this function will be called
                                                  // with a msgInfo object as 'this',
                                                  // so we can use it to end the
                                                  // context only when the queued
                                                  // endsession message has completed
                                                  nrdp.ntba.endCustomContext(this.ntbaContext);
                                              });

            // Note, we can modify these values here because queued messages
            // store their own copies for use until the message has finished
            target.sessionId = 0;
            target.localSessionId = 0;
            delete target.ntbaContext;

        } else {
            mdx.log("invalid session ID");
        }
    },

    _handleEndSession: function(fromDevice, sessionId) {

        fromDevice.sessionId = 0;
        fromDevice.localSessionId = 0;
        if (!mdx.pair._dummyPairing) {
        	if (fromDevice.ntbaContext)
                nrdp.ntba.endCustomContext(fromDevice.ntbaContext);
        }
        delete fromDevice.ntbaContext;

        var sessiondata = {};
        sessiondata.type = "sessionended";
        sessiondata.sid = fromDevice.localSessionId;
        nrdp._callEventListeners("mdx.session", sessiondata);
    },

    _endAllSessions: function() {
        for (target in mdx._remoteDeviceMap) {
            mdx.session.endSession(mdx._remoteDeviceMap[target].localSessionId);
        }
    },

    sendMessage: function(sessionId, messageName, messageObject) {
        if ( (sessionId == 0) || (!messageName) || (!messageObject) ) {
            mdx.log("Invalid argument");
            return -1;
        }
        var target;
        if ( (mdx._role == mdx.CONTROLLER || mdx._bothMode == mdx.CONTROLLER) )
            target = mdx.device.getRemoteDevicefromLocalSid(sessionId);
        else
            target = mdx.device.getRemoteDevicefromSid(sessionId);
        if ( (!target) || (target.sessionId == 0) ) {
            mdx.log("sessionId " + sessionId + " does not exist");
            return -1;
        }

        var appMessage = {
            sessionAction: "appMessage",
            sessionId: target.sessionId,
            appAction: messageName,
            appBody: messageObject
        };

        // Sending step 1:
        var plainText = mdx.guard.processOutgoingMessageBody(appMessage);

        var xid = mdx.nextTransactionId();

        // Sending step 2:
        mdx.session._queueOutgoingMessage(target, xid, plainText);

        return xid;
    },

        _queueOutgoingMessage: function(target, xid, plainText, completionCallback) {

            // we copy the necessary information for sending the message we're
            // queueing, rather than referring directly to the target object's
            // members, because we may modify the target object's member values
            // asynchronously with the sending sequence completionCallback is
            // called at the end of the send process
            var msgInfo = {
                fromuserid: target.controlleruserid,
                touuid: target.uuid,
                touserid: target.targetuserid,
                ntbaContext: target.ntbaContext,
                xid: xid,
                sessionId: target.sessionId,
                targetuuid: target.uuid,
                completionCallback: completionCallback,

                // we do include the target in order to process the next message in
                // its queue or mark it as no longer sending messages upon
                // completion, do not refer to its other members!
                target: target
            };

        if (target.sendPending) {
            if (!target.sendQueue) target.sendQueue = [];
            target.sendQueue.push([msgInfo, plainText]);
        } else {
            // Sending step 3. process the next plaintext message in the queue
            mdx.session._sendCipherText(msgInfo, plainText);
        }
        },

        _processSendQueue: function(target) {
            if (target.sendQueue && target.sendQueue.length > 0) {
            var arr = target.sendQueue.shift();
            mdx.session._sendCipherText.apply(mdx.session, arr);
        }
    },

    _sendCipherText: function(msgInfo, plainText) {
        msgInfo.target.sendPending = true;

        // Sending step 4. encrypt the plaintext message
        if (mdx.pair._dummyPairing) {
            mdx.session._sessionMsgEncryptCb(msgInfo, {data: plainText});
        } else {
        	if (!msgInfo.ntbaContext) {
                delete msgInfo.target.sendPending;
                mdx.session._processSendQueue(msgInfo.target);
                return;
            }
            nrdp.ntba.encrypt(msgInfo.ntbaContext, plainText,
                              function(encrypt) {
                                  mdx.session._sessionMsgEncryptCb(msgInfo, encrypt);
                              });
        }
    },

    _sessionMsgEncryptCb: function(msgInfo, encrypt) {
        mdx.session._sendSessionMessage(msgInfo, encrypt.data);
    },

    _hmacCanonicalSort: function(a, b) {
        if (a.name < b.name) {
            return -1;
        } else if (a.name > b.name) {
            return 1;
        }
        return 0;
    },

    _createCanonicalForm: function(data) {
        var canonicalArray = [];
        for(param in data) {
            if (param !== undefined && data[param] !== undefined) {
                canonicalArray.push({name: param, value : data[param]});
            }
        }
        canonicalArray.sort(mdx.session._hmacCanonicalSort);

        var canonicalForm = "";
        for(pair in canonicalArray) {
            if(canonicalArray[pair].name && canonicalArray[pair].value) {
                if (pair > 0) {
                    canonicalForm += "&";
                }
                canonicalForm += canonicalArray[pair].name + "=" + encodeURIComponent(canonicalArray[pair].value);
            }
        }

        return canonicalForm;
    },

    _sendSessionMessage: function(msgInfo, ciphertext) {

        // Sending step 5. create the MDX session message
        var data = {};
        data.action = "session";
        data.version = mdx.session._sessionVersion;
        data.fromurl = nrdp.mdx.localIPAddress;
        data.fromuuid = mdx.MYUUID;
        data.fromuserid = msgInfo.fromuserid;
        data.touuid = msgInfo.touuid;
        data.touserid = msgInfo.touserid;
        data.nonce = msgInfo.xid;//mdx.nextTransactionId();//
        data.ciphertext = ciphertext;

        // All message parameters, both MDX envelope and application logic ones,
        // are transformed to canonical form as name1=value&name2=value2... in
        // alphabetical order by name, URL-encoded; HMAC is then computed over
        // this string. The ciphertext=value element, not the original
        // plaintext, is used for HMAC calculation; therefore, on the receiving
        // side HMAC validation is done before decryption.

        // Sending step 6. create the canonical form of the message for calculating hmac
        var canonicalForm = mdx.session._createCanonicalForm(data);

        // Sending step 7. calculate the hmac of the message
        if (mdx.pair._dummyPairing) {
            mdx.session._sendSessionMessageHmacCb("<dummyhmac>", data, msgInfo);
        } else {
        	if (!msgInfo.ntbaContext) {
        		delete msgInfo.target.sendPending;
                mdx.session._processSendQueue(msgInfo.target);
                return;
        	}
            nrdp.ntba.hmac(msgInfo.ntbaContext, canonicalForm,
                           function(hmacStr) {
                               mdx.session._sendSessionMessageHmacCb(hmacStr, data, msgInfo);
                           });
        }
    },

    _sendSessionMessageHmacCb: function(hmac, data, msgInfo) {

        // set the hmac parameter to be what was calculated
        data.hmac = hmac;

        // Sending step 8. send the MDX session message
        mdx.createMessage("session", data, data.touuid);

        if (msgInfo.completionCallback) {
            msgInfo.completionCallback.call(msgInfo);
        }

        delete msgInfo.target.sendPending;

        // Sending step 9. process the next message in the queue if it exists
        mdx.session._processSendQueue(msgInfo.target);
    },

    _addMessageListener: function(prefix, msgType, callback) {
        msgType = prefix + "." + msgType;
        mdx.log("add listener for " + msgType);

        if (callback) {
            mdx.session._messageListeners[msgType] = callback;
        }
    },

    _removeMessageListener: function(prefix, msgType, callback) {
        msgType = prefix + "." + msgType;
        mdx.log("remove listener for " + msgType);

        if (callback) {
            var myListener = this._messageListeners[msgType];
            if (myListener) {
                delete mdx.session._messageListeners[msgType];
            }
        }
    },

    _callMessageListeners: function(prefix, event) {
        var msgType = prefix + "." + event.type;
        var callback = mdx.session._messageListeners[msgType];
        if (callback) {
            callback(event);
        }
    },

    _handleAppMessage: function(fromDevice, nonce, sessionId, appAction, appMessageBody) {

        // TODO check if xid matches. to document if session id is 0, then error
        if (fromDevice.sessionId == sessionId) {
            var sessiondata = {};
            if ( (mdx._role == mdx.CONTROLLER || mdx._bothMode == mdx.CONTROLLER) ) {
                sessiondata.sid = fromDevice.localSessionId;
            } else {
                sessiondata.sid = sessionId;
            }
            sessiondata.type = appAction;
            sessiondata.transactioId = nonce;
            sessiondata.pairingContext = fromDevice.pairingContext;
            sessiondata.msgObject = appMessageBody;
            mdx.session._callMessageListeners("mdx.session", sessiondata);
        } else {
            // sid does not exist
            mdx.session._handleInvalidSession(fromDevice, sessionId, nonce);
        }
    },

    _handleMessagingErrorMessage: function(fromDevice, nonce) {

        mdx.log("Recieved messagingerror: " + errorcode + " " + errorstring);

        var event = {
            type: "messagingerror",
            errorCode: sessionMessage.errorcode,
            pairingContext: fromDevice.pairingContext,
            sid: fromDevice.sessionId,
            transactionId: nonce
        };
        nrdp._callEventListeners("mdx.session", event);
    },

    _handleInvalidSession: function(fromDevice, sessionId, nonce) {
        mdx.session._sendSessionErrorMessage(fromDevice.uuid,
                                             nonce,
                                             mdx.session.ERRORS.MDX_SESSION_INVALID_SID,
                                             "MDX SESSION INVALID SID");
    },

    _handleIncomingMessage: function(evt) {
        var request = mdx._getUriInfo(evt.url, evt.headers);
        if (request.path == "session") {

            var message = mdx.guard.processIncomingMessage(evt.body);
            if (!message) return;

            // todo: do we even want to queue messages for each receiver? should
            // we instead queue all messages to one queue for HMAC, and once
            // that passes, trust the fromuuid?
            var target = mdx._remoteDeviceMap[message.fromuuid];
            if (!target) {
                // send error message
                mdx.log("dropping message from unknown target: " +  message.fromuuid + " will send error to " + message.fromurl);
                var errorObj = {};
                errorObj.errorcode = mdx.session.ERRORS.MDX_SESSION_UNKNOWN_SENDER;
                errorObj.errorstring = "Unknown UUID. Please try to pair before starting session";
                mdx.session.sendErrorMessageToUrl("http://" + message.fromurl + ":9080/", message.nonce, message.fromuuid, message.touuid, errorObj);
                return;
            }

            mdx.session._queueReceived(target, message);
        }
    },

    _queueReceived: function(target, message) {
        if (target.recvPending) {
            if (!target.recvQueue) target.recvQueue = [];
            target.recvQueue.push(message);
        } else {
            mdx.session._processMessage(target, message);
        }
    },

    _processRecvQueue: function(target) {
        if (target.recvQueue && target.recvQueue.length > 0) {
            var message = target.recvQueue.shift();
            mdx.session._processMessage(target, message);
        }
    },

    _processMessage: function(target, message) {
    	target.recvPending = true;
        if (mdx.pair._dummyPairing) {
            // we if are dummy pairing, we don't perform an actual hmac
            target.ntbaContext = "<dummycontext>";
            mdx.session._sessionIncomingMsgHmacCb("<dummyhmac>", message.hmac, target, message);
        } else {
            if (message.action == "messagingerror") {
                var event = {
                    type: "messagingerror",
                    errorCode: message.errorcode,
                    pairingContext: target.pairingContext,
                    sid: target.sessionId,
                    transactionId: message.nonce
                };
                nrdp._callEventListeners("mdx.session", event);
                delete target.recvPending;
                mdx.session._processRecvQueue(target);
                return;
            }
            if (target.ntbaContext) {
                // we have a context with this target, we can validate the
                // message with hmac. Be sure NOT to include the hmac field
                // itself when creating the canonical form
                var messageHmac = message.hmac;
                delete message.hmac;

                var canonicalForm = mdx.session._createCanonicalForm(message);

                nrdp.ntba.hmac(target.ntbaContext, canonicalForm,
                               function(hmacStr) {
                                   mdx.session._sessionIncomingMsgHmacCb(hmacStr, messageHmac, target, message);
                               });
            } else {

                // we don't have a context with this target, create one before
                // we can validate this message
                mdx.log("Creating custom context with sharedsecret=" + target.targetsharedsecret);

                nrdp.ntba.beginCustomContext(target.targetsharedsecret, "",
                                             function(ctx) {
                                                 mdx.session._processMessageCustomContextCb(ctx, target, message);
                                             });
            }
        }
    },

    _processMessageCustomContextCb: function(ctx, target, message) {
        target.ntbaContext = ctx.context;

        if (target.ntbaContext && target.ntbaContext != "") {
            mdx.log("Custom context " + target.ntbaContext + " created");
            mdx.session._processMessage(target, message);
        } else {
            // there was an error creating an ntba context?
            // TODO: what is the appropriate error message?
            mdx.log("ntba.beginCustomContext failed");

            delete target.recvPending;
            mdx.session._processRecvQueue(target);
        }
    },

    _sessionIncomingMsgHmacCb: function(hmacStr, messageHmac, target, message) {

        var wasError = false;
        if (messageHmac != hmacStr) {
            mdx.log("hmac failure: ntba context=" + target.ntbaContext + " msg=" + messageHmac + " calculated=" + hmacStr);

            mdx.session._sendSessionErrorMessage(target.uuid,
                                                 message.nonce,
                                                 mdx.session.ERRORS.MDX_SESSION_INVALID_HMAC,
                                                 "MDX SESSION INVALID HMAC");
            wasError = true;
        } else if (message.touserid != target.controlleruserid) {
            mdx.session._sendSessionErrorMessage(target.uuid,
                                                 message.nonce,
                                                 mdx.session.ERRORS.MDX_SESSION_UNKNOWN_RECEIVER,
                                                 "MDX SESSION UNKNOWN RECEIVER");

            wasError = true;
        } else if (message.fromuserid != target.targetuserid) {
            mdx.session._sendSessionErrorMessage(target.uuid,
                                                 message.nonce,
                                                 mdx.session.ERRORS.MDX_SESSION_UNKNOWN_SENDER,
                                                 "MDX SESSION UNKNOWN SENDER");

            wasError = true;
        } else if (message.version != mdx.session._sessionVersion) {
            mdx.session._sendSessionErrorMessage(target.uuid,
                                                 message.nonce,
                                                 mdx.session.ERRORS.MDX_SESSION_UNKNOWN_VERSION,
                                                 "MDX SESSION UNKNOWN VERSION");
            wasError = true;
        }

        if (wasError) {
            // process the next message in this targets recv queue
            delete target.recvPending;
            mdx.session._processRecvQueue(target);
            return;
        }

        if(mdx.pair._dummyPairing) {
            // if we are dummy pairing, the ciphertext is not encrypted
            mdx.session._sessionIncomingMsgDecryptCb(message.ciphertext, target, message);
        } else {
        	if (!target.ntbaContext) {
                delete target.recvPending;
                mdx.session._processRecvQueue(target);
                return;
            }
            nrdp.ntba.decrypt(target.ntbaContext, message.ciphertext,
                              function(plaintxt) {
                                  mdx.session._sessionIncomingMsgDecryptCb(plaintxt, target, message);
                              });
        }
    },

    _sessionIncomingMsgDecryptCb: function(plaintext, target, message) {
        if (plaintext == "") {
            mdx.log("decryption failed");

            // TODO: if the message decryption failed we don't know what sessionId is
            mdx.session._sendSessionErrorMessage(target.uuid,
                                                 message.nonce,
                                                 mdx.session.ERRORS.MDX_SESSION_DECRYPTION_FAILED,
                                                 "MDX SESSION DECRYPTION FAILED");

            delete target.recvPending;
            mdx.session._processRecvQueue(target);
            return;
        }

        var sessionMessage = mdx.guard.processIncomingMessageBody(message, plaintext);

        mdx.session._handleSessionMessage(target, message.nonce, sessionMessage);

        delete target.recvPending;
        mdx.session._processRecvQueue(target);
    },

    _handleSessionMessage: function(target, nonce, sessionMessage) {			
        mdx.log("Session Message: " + JSON.stringify(sessionMessage));
        if (sessionMessage.sessionAction === "createSession") {
            mdx.session._handleCreateSession(target);
        } else if (sessionMessage.sessionAction === "endSession") {
            mdx.session._handleEndSession(target, sessionMessage.sessionId);
        } else if (sessionMessage.sessionAction === "startSessionResponse") {
            mdx.session._handleStartSessionResponse(target, sessionMessage.sessionId);
        } else if (sessionMessage.sessionAction === "appMessage") {
            mdx.session._handleAppMessage(target, nonce, sessionMessage.sessionId, sessionMessage.appAction, sessionMessage.appBody);
        } else if (sessionMessage.sessionAction == "messagingerror") {
            mdx.session._handleMessagingErrorMessage(target, nonce, sessionMessage.errorcode, sessionMessage.errorstring);
        } else {
            // internal error, unknown session message action
        }
    },

    _handleHttpResponse: function(evt) {
        mdx.log("mdx.session._handleHttpResponse(): Received httpresponse");
        var remoteDevice = mdx.device.getRemoteDevicefromXid(evt.xid);
        if (!remoteDevice) {
            mdx.log("received http response unknown xid =" + evt.xid );
            return;
        }
        // save this value because responseReceived will clear out in flight information from the device
        var inFlightAction = remoteDevice.inFlightAction;
        mdx._responseReceived(remoteDevice);
        if(inFlightAction.indexOf("error") != -1) {
            // don't send an event to the application for error messages' httpresponses
        } else if (evt.body === "error" && ( evt.url.indexOf("session") != -1 ) ) {
            mdx.log("error sending message");
            var appevt = {
                type: "messagingerror",
                errorCode: mdx.session.ERRORS.MDX_SESSION_NETWORK_ERROR ,
                pairingContext: remoteDevice.pairingContext?remoteDevice.pairingContext:"",
                sid: remoteDevice.sessionId?remoteDevice.sessionId:0,
                transactionId: evt.xid
            };
            nrdp._callEventListeners("mdx.session", appevt);
        } else if ( (evt.body === "error") && ( evt.url.indexOf("pairingrequest") != -1 ) ) {
        	mdx.pair._sendNetworkErrorEvent(remoteDevice.uuid);
        } else if ( evt.url.indexOf("pairing") == -1 ) {
            var messageAck = {};
            messageAck.type = "messagedelivered";
            messageAck.pairingContext = remoteDevice.pairingContext;
            //          messageAck.sid = remoteDevice.sessionId;
            messageAck.transactionId = evt.xid;
            nrdp._callEventListeners("mdx.session", messageAck);
        }
    }
};
