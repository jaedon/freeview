/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, NTRACE, ASSERT, nrdp_platform */

#ifdef NDEBUG
# define ASSERT(x)
#else
# define ASSERT(x) do { if(!x) { nrdp.log.fatal("ASSERT: " + #x); nrdp.assert(); } } while(0)
#endif
#ifdef NRDP_HAS_TRACING
# define NTRACE(...) nrdp.log.trace(__VA_ARGS__)
#else
# define NTRACE(...)
#endif

nrdp = {
    classname: "NrdpBridge",
    _init: false,
    _isError: false,
    _classes: { },
    _backchannels: [],
    _MAX_STACK_SIZE: 16384, // When we log the stack trace of an exception

    get isReady() { return this._init && !this._isError; },
    get debug() { return this._syncData.debug; },
    get options() { return this._syncData.options; },
    get uiQueryString() { return this.system.uiQueryString; },
    set uiQueryString(query) { nrdp.system.uiQueryString = query; },
    get bootURL() { return this.system.bootURL; },
    get started() { return this._syncData.started; },
    get trustStoreHash() { return this._syncData.trustStoreHash; },
    get capabilities() { return this._syncData.capabilities; },

    get READY() { return "READY"; },

    get COMPLETE() { return "COMPLETE"; },
    get NETWORK_ERROR() { return "NETWORK_ERROR"; },
    get ACTION_ID() { return "ACTION_ID"; },
    get ERROR() { return "ERROR"; },

    get hasSuspendHandler() { return this._syncData.hasSuspendHandler; },
    set hasSuspendHandler(v) { this._syncData.hasSuspendHandler=v; nrdp._setProperty(null, "hasSuspendHandler", v); },

    _mapProperty: function _mapProperty(object, property) {
        var classname = object;
        if (typeof classname !== "string") {
            classname = classname.classname;
            if(!classname)
                return undefined;
        }
        var classinfo = this._classes[classname];
        if(!classinfo)
            return undefined;

        if (typeof property === "string")
            return classinfo.properties.byName[property];
        return classinfo.properties.byIndex[property];
    },
    _mapMethod: function _mapMethod(object, method) {
        var classname = object;
        if (typeof classname !== "string") {
            classname = classname.classname;
            if(!classname) {
                nrdp.log.error("Could not find object classname " + object, "NRDP_SCRIPT");
                return undefined;
            }
        }
        var classinfo = this._classes[classname];
        if(!classinfo)
            return undefined;

        if (typeof method === "string")
            return classinfo.methods.byName[method];
        return classinfo.methods.byIndex[method];
    },


    _tryBackchannel: function _tryBackchannel(chan) {
        if (chan.init()) {
            NTRACE(" using backchannel " + chan.name);
            nrdp._backchannel = chan;
            return true;
        } else {
            return false;
        }
    },

    setupBackchannel: function setupBackchannel() {
        var i;
        if (nrdp._backchannel) {
            return;
        }
        NTRACE("nrdp.setupBackchannel called" + (typeof window !== "undefined" ? (", window.location = " + window.location) : ""));
        for (i = 0; i < nrdp._backchannels.length; i++) {
            if (typeof nrdp._backchannels[i] !== "object")
                continue;
            if (nrdp._tryBackchannel(nrdp._backchannels[i]))
                return;
        }
    },

    shutdownBackchannel: function shutdownBackchannel() {
        if (!nrdp._backchannel) {
            return;
        }
        NTRACE("nrdp.shutdownBackchannel called", "NRDP_SCRIPT");
        this._sendSyncdEvent(function() { this._callEventListeners(this, { type: "shutdown" } ); }, this);
        nrdp.storage.flush();
        if(nrdp._backchannel.shutdown)
            nrdp._backchannel.shutdown();
    },

    _sendSyncdEvent: function _sendSyncdEvent(fn, that, event) { //for gibbon to hook
        fn.call(that, event);
    },

    hookInit: function hookInit(fn) {
        nrdp._hookInitFunction = fn;
    },

    _sendInitEvent: function _sendInitEvent() {
        var sendInit = this._sendSyncdEvent.bind(this, function() { this._callEventListeners(nrdp, {type: "init", status: this.READY}, true); }, this);
        if (this._hookInitFunction) {
            this._hookInitFunction(sendInit);
            delete this._hookInitFunction;
        } else {
            sendInit();
        }
    },

    init: function init() {
        if (!nrdp._init) {
            nrdp._init = true;
            NTRACE("nrdp.init called", "NRDP_SCRIPT");
            nrdp.setupBackchannel();
        }

        nrdp._sendInitEvent();
    },

    now: function now() {
        if (nrdp._backchannel)
            return nrdp._invoke(null, "now");
        return Date.now();
    },

    objects: function objects(cb) {
        nrdp._fn("objects", {}, cb);
    },

    locks: function locks(cb, flags) {
        nrdp._fn("locks", {flags: flags ? flags : 0}, cb);
    },

    exit: function exit(code) {
        nrdp._invoke(null, "exit", {code: code});
    },

    ping: function ping(cb) {
        var sentTime = nrdp.mono();
        nrdp._fn("ping", {}, function(receivedTime) {
            var eventTime = nrdp.mono();
            if (cb)
                cb(sentTime, receivedTime, eventTime);
            else
                nrdp.log.warn('Bridge ping : JS -> bridge = ' + (receivedTime - sentTime) +
                              ' : bridge -> JS = ' + (eventTime - receivedTime) +
                              ' : roundtrip = ' + (eventTime - sentTime));
        });
    },

    assert: function assert(a, message) {
        if (!a) {
            message = 'JS ASSERTION FAILED' + (message ? ' : ' + message : "");
            var s = nrdp.stacktrace();
            nrdp.log.fatal(message + (s ? '\n' + s : "" ), "NRDP_SCRIPT");
            nrdp._invoke(null, "assert");
        }
    },

    stacktrace: function stacktrace() {
        try { throw new Error(); }
        catch(e) { return e.stack; }
    },

    mono: function mono() {
        return nrdp._backchannel ? nrdp._backchannel.mono() : undefined;
    },

    pmono: function pmono() {
        return nrdp._backchannel ? nrdp._backchannel.pmono() : undefined;
    },

    drmTime: function drmTime() {
        return nrdp.drmsystem.getDrmTime();
    },

    atob: function atob(s, returnTypedArray) {
        if (nrdp._backchannel && nrdp._backchannel.atob)
            return nrdp._backchannel.atob(s, returnTypedArray);
        return undefined;
    },

    btoa: function btoa(s, urlSafe, returnTypedArray) {
        if (nrdp._backchannel && nrdp._backchannel.btoa)
            return nrdp._backchannel.btoa(s, urlSafe, returnTypedArray);
        return undefined;
    },

    atoutf8: function atoutf8(s) {
        if (nrdp._backchannel && nrdp._backchannel.atoutf8)
            return nrdp._backchannel.atoutf8(s);
        return undefined;
    },

    utf8toa: function utf8toa(s) {
        if (nrdp._backchannel && nrdp._backchannel.utf8toa)
            return nrdp._backchannel.utf8toa(s);
        return undefined;
    },

    compress: function compress(data, type, binary) {
        if (nrdp._backchannel && nrdp._backchannel.compress)
            return nrdp._backchannel.compress(data, type, binary);
        return undefined;
    },

    uncompress: function compress(data, type, returnTypedArray) {
        if (nrdp._backchannel && nrdp._backchannel.compress)
            return nrdp._backchannel.uncompress(data, type, returnTypedArray);
        return undefined;
    },

    random: function random(data) {
        if (nrdp._backchannel && nrdp._backchannel.random)
            return nrdp._backchannel.random(data);
        return undefined;
    },

    gctag: function gctag(name) {
        if (nrdp._backchannel && nrdp._backchannel.gctag)
            return nrdp._backchannel.gctag(name);
        return {};
    },

    getConfigList: function getConfigList() {
        this._callEventListeners(this, {type:'config', list: this._syncData.configList});
    },

    get config() { return this._syncData.config; },

    setConfigData: function setConfigData(file, data) {
        nrdp.config[file] = data;
        nrdp._invoke(null, "setConfigData", {name: file, data: data});
        this._callEventListeners(this, {type:'configChanged', name: file, data: data});
    },

    _path: "nrdp",
    addEventListener: function addEventListener(evt, listener) {
        if(evt == "suspendChanged")
            this.hasSuspendHandler = true;
        nrdp._addEventListener(this, evt, listener);
    },
    removeEventListener: function removeEventListener(evt, listener) {
        return nrdp._removeEventListener(this, evt, listener);
    },

    _findObject: function _findObject(name) {
        if (name instanceof Object)
            return name;
        var result = nrdp;
        if (name != "nrdp") {
            var bits = name.split('.');
            for(var i = 1; result && i < bits.length; ++i) {  // skip beginning "nrdp"
                result = result[bits[i]];
            }
        }
        return result;
    },

    _setProperty: function _setProperty(subobj, prop, val) {
        if (!nrdp._backchannel) {
            nrdp.log.info("unhandled _setProperty " + subobj + " " + prop, "NRDP_SCRIPT");
        }
        var objName = subobj ? "nrdp." + subobj : "nrdp";

        var obj = nrdp._findObject(objName);
        if (!obj) {
            nrdp.log.error("could not find object " + objName + " to set property " + prop + " on", "NRDP_SCRIPT");
            return;
        }
        if (obj._setProperty_current && obj._setProperty_current[prop]) {
            if (!obj._setProperty_pending)
                obj._setProperty_pending = {};
            obj._setProperty_pending[prop] = {
                object: subobj,
                property: prop,
                value: val
            };
            return;
        }
        if(nrdp._backchannel.setProperty(subobj, prop, val)) {
            if (!obj._setProperty_current)
                obj._setProperty_current = {};
            obj._setProperty_current[prop] = true;
        }
    },

    _invoke: function _invoke(subobj, method, args) {
        if (!nrdp._backchannel) {
            nrdp.log.info("unhandled _invoke " + subobj + " " + method, "NRDP_SCRIPT");
        }
        return nrdp._backchannel.invoke(subobj, method, args);
    },

    _addEventListener: function _addEventListener(object, eventType, listener) {
        if (!listener)
            return false;
        var listeners, path;
        if (typeof object === "string") {
            if (!nrdp._oldStyleListeners)
                nrdp._oldStyleListeners = {};
            listeners = nrdp._oldStyleListeners;
            eventType = object + "." + eventType;
            path = eventType;
        } else {
            if (!object._eventListeners)
                object._eventListeners = {};
            listeners = object._eventListeners;
            path = object._path + "." + eventType;
        }
        NTRACE("add listener for " + path, "NRDP_SCRIPT");
        if (!listeners[eventType])
            listeners[eventType] = [];
        listeners[eventType].push(listener);
        return true;
    },

    _removeEventListener: function _removeEventListener(object, eventType, listener) {
        if (!listener)
            return false;

        var myListeners, parent, path;
        if (typeof object === "string") {
            eventType = object + "." + eventType;
            parent = nrdp._oldStyleListeners;
            path = eventType;
        } else if (typeof object === "object") {
            parent = object._eventListeners;
            path = object._path + "." + eventType;
        }

        if (!parent)
        {
            NTRACE("remove listener, no parent", "NRDP_SCRIPT");
            return false;
        }
        myListeners = parent[eventType];
        if (!myListeners)
        {
            NTRACE("remove listener, no listeners", "NRDP_SCRIPT");
            return false;
        }
        var index = myListeners.indexOf(listener);
        if (index < 0)
        {
            NTRACE("remove listener, listener not found", "NRDP_SCRIPT");
            return false;
        }

        if (index >= 0) {
            if (myListeners.length == 1)
                delete parent[eventType];
            else
                myListeners.splice(index, 1);
        }

        var len = parent[eventType] ? parent[eventType].length : 0;
        NTRACE("remove listener for " + path + ", length is " + len, "NRDP_SCRIPT");
        return true;
    },

    _hasEventListener: function _hasEventListener(object, eventType) {
        if (typeof object === "string") {
            eventType = object + "." + eventType;
            return (nrdp._oldStyleListeners && nrdp._oldStyleListeners[eventType] && nrdp._oldStyleListeners[eventType].length);
        } else {
            return (object._eventListeners &&
                    object._eventListeners[eventType] &&
                    object._eventListeners[eventType].length);
        }
    },

    _callEventListeners: function _callEventListeners(object, event, remove) {
        var myListeners;
        var path;
        if (typeof object === "string") {
            var eventType = object + "." + event.type;
            if (!nrdp._oldStyleListeners || !nrdp._oldStyleListeners[eventType])
                return;
            if (remove) {
                myListeners = nrdp._oldStyleListeners[eventType];
                delete nrdp._oldStyleListeners[eventType];
            } else {
                myListeners = nrdp._oldStyleListeners[eventType].slice(0);
            }
            path = object;
        } else {
            if (!object._eventListeners || !object._eventListeners[event.type])
                return;
            if (remove) {
                myListeners = object._eventListeners[event.type];
                delete object._eventListeners[event.type];
            } else {
                myListeners = object._eventListeners[event.type].slice(0);
            }
            path = object._path;
        }
        for (var i = 0; i < myListeners.length; i++) {
            var listener = myListeners[i];
            if (listener) {
                NTRACE("call listener for " + path + "." + event.type, "NRDP_SCRIPT");
                listener(event);
            }
        }
    },

    _urlEncode: function _urlEncode(obj) {
        var str = "";
        for (var p in obj) {
            if (str) str += "&";
            str += p + "=" + encodeURIComponent(obj[p]);
        }
        return str;
    },

    describeEvent: function describeEvent(event) { return nrdp._describeEvent(event); },
    _describeEvent: function _describeEvent(event) {
        if(!event || !event.type)
            return undefined;
        var result;
        var path;
        if (event.object instanceof Object) {
            path = event.object._path;
        } else {
            path = event.object;
        }
        if(event.type == "Event") {
            result = path + "::" + event.name;
        } else if(event.type == "PropertyUpdate") {
            result = path + "::" + event.type + JSON.stringify(event.properties);
        } else if(event.type == "SetProperty") {
            result = path + "::" + event.type + "(" + event.property + ")";
        } else {
            result = path + "::" + event.type;
        }
        return result;
    },

    _gotEvent: function _gotEvent(event) {
        var start, evt;
        if (nrdp._syncData && nrdp.debug && nrdp._init) {
            start = nrdp.mono();
            if (start - event.time > 50) {
                var log = "JS event waited " + (start - event.time) + "ms: " + nrdp.describeEvent(event);
                if (event.timerPreemption)
                    log += " timerPreemption: " + event.timerPreemption;
                nrdp.log.debug(log);
            }
        }
        try {
            if (event.type == "Event") {
                this._gotEventEvent(event);
            } else if (event.type == "ClassSync") {
                this._gotClassSyncEvent(event);
            } else if (event.type == "PropertyUpdate") {
                this._gotPropertyUpdateEvent(event);
            } else if (event.type == "SetProperty") {
                this._gotSetPropertyEvent(event);
            } else if (event.type == "Method") {
                this._gotMethodEvent(event);
            } else if (event.type == "EventSourceError") {
                nrdp._isError = true;
                nrdp.log.info("EventSource went away, sending fatalerror", "NRDP_SCRIPT");
                evt = {
                    type: "fatalerror"
                };
                this._callEventListeners(this, evt);
            } else {
                nrdp.log.error("unhandled eventsource type " + event.type, "NRDP_SCRIPT");
            }
        } catch (e) {
            var tags = {},
                length;
            if (typeof e !== "string") {
                for (var n in e) {
                    if (e.hasOwnProperty(n)) {
                        tags[n] = e[n];
                    }
                }
            }
            // chrome defines this
            if (!tags.stack && e.stack) {
                tags.stack = e.stack;
            }
            if (typeof tags.stack == 'string') {
                length = tags.stack.length;
                if (length && length > nrdp._MAX_STACK_SIZE) {
                    tags.stack = tags.stack.substr(0, nrdp._MAX_STACK_SIZE/2) +
                        '\n...[stack truncated, it was ' + length + ' characters]...\n' +
                        tags.stack.substr(-nrdp._MAX_STACK_SIZE/2);
                }
            }
            nrdp.log.error("JAVASCRIPT EXCEPTION: " + e.toString(), "NRDP_SCRIPT", undefined, tags);
            evt = {
                type: "exception",
                exception: e
            };
            this._callEventListeners(this, evt);
            ASSERT(!nrdp.options.js_assert && ("Uncaught exception: " + e.toString()));
        }
        if (start !== undefined) {
            var end = nrdp.mono();
            if (nrdp._syncData && (end - start > nrdp.options.js_delay)) {
                nrdp.log.warn("handling JS event took " + (end - start) + "ms: " + nrdp.describeEvent(event), "NRDP_SCRIPT");
            }
        }
    },

    _gotEventEvent: function _gotEventEvent(event) {
        if (!event.object) {
            if (event.name == "factoryReset") {
                var evt = {
                    type: event.name
                };
                this._callEventListeners(this, evt);
            }
        } else {
            var obj = this._findObject(event.object);
            if (!obj || !obj._handleEvent || !obj._handleEvent(event))
                nrdp.log.warn("unhandled event " + event.object + " " + event.name, "NRDP_SCRIPT");
        }
    },

    _gotClassSyncEvent: function _gotClassSyncEvent(event) {
        var classinfo = { methods: { byIndex: [], byName: {} }, properties: { byIndex: [], byName: {} } };
        for(var method in event.value.methods) {
            classinfo.methods.byIndex.push(event.value.methods[method]);
            classinfo.methods.byName[event.value.methods[method]] = parseInt(method);
        }
        for(var property in event.value.properties) {
            classinfo.properties.byIndex.push(event.value.properties[property]);
            classinfo.properties.byName[event.value.properties[property]] = parseInt(property);
        }
        this._classes[event.name] = classinfo;
    },

    _gotPropertyUpdateEvent: function _gotPropertyUpdateEvent(event) {
        var obj = this._findObject(event.object);
        if (!obj) {
            nrdp.log.error("Could not find object " + event.object + " for sync data", "NRDP_SCRIPT");
            return;
        }
        var fn;
        if (obj._updateProperty)
            fn = obj._updateProperty;
        else if (!obj._syncData)
            obj._syncData = {};
        for (var property in event.properties) {
            if (typeof property !== "string") {
                var propertyName = this._mapProperty(obj, property);
                if(!propertyName) {
                    nrdp.log.error("Could not map PropertyUpdate: " + event.object + " " +  property, "NRDP_SCRIPT");
                    continue;
                }
                property = propertyName;
            }
            if (typeof event.properties[property] !== "function") {
                if (fn)
                    fn.call(obj, property, event.properties[property]);
                else
                    obj._syncData[property] = event.properties[property];
            }
        }
    },

    _gotSetPropertyEvent: function _gotSetPropertyEvent(event) {
        var obj = this._findObject(event.object);
        if (obj) {
            var property = event.property;
            if (typeof property !== "string") {
                var propertyName = this._mapProperty(obj, property);
                if(!propertyName) {
                    nrdp.log.error("Could not map SetProperty: " + event.object + " " +  property, "NRDP_SCRIPT");
                    return;
                }
                property = propertyName;
            }
            if(obj._setProperty_current)
                delete obj._setProperty_current[property];
            if (obj._setProperty_pending && obj._setProperty_pending[property]) {
                var newset = obj._setProperty_pending[property];
                delete obj._setProperty_pending[property];
                nrdp._setProperty(newset.object, newset.property, newset.value);
            }
        }
    },

    _gotMethodEvent: function _gotMethodEvent(event) {
        // this only happens for an invalid argument error
        var method = event.method;
        if (typeof method !== "string") {
            var methodName = this._mapMethod(event.object, method);
            if(!methodName) {
                nrdp.log.error("Could not map Method: " + event.object + " " +  method, "NRDP_SCRIPT");
                return;
            }
            method = methodName;
        }
        var evt = {
            type: "invalidargument",
            object: event.object,
            method: method,
            argument: event.returnValue
        };
        this._callEventListeners(this, evt);
    },

    _nextIdx: 1,
    _cbs: {},
    _fn: function _fn(name, args, cb) {
        if (!args) args = {};
        args.id = this._nextIdx++;
        this._cbs[args.id] = cb;
        nrdp._invoke(null, name, args);
    },

    parseXml: function parseXml(xml, cb) {
        if (cb) {
            nrdp._fn("parseXML", {xml: xml}, cb);
        } else if (nrdp._backchannel && nrdp._backchannel.parseXML) {
            return nrdp._backchannel.parseXML(xml);
        }
        return undefined;
    },

    parseJSON: function parseJSON(json, cb) {
        if (cb) {
            nrdp._fn("parseJSON", {json: json}, cb);
        } else if (nrdp._backchannel && nrdp._backchannel.parseJSON) {
            return nrdp._backchannel.parseJSON(json);
        }
        return undefined;
    },

    setTrustStore: function setTrustStore(trustStore) {
        nrdp._invoke(null, "setTrustStore", {trustStore: trustStore});
    },

    setServerTime: function setServerTime(time) {
        nrdp._invoke(null, "setServerTime", {time: time});
    },

    setTestDriverIpAddress: function setTestDriverIpAddress(address) {
        nrdp._invoke(null, "setTestDriverIpAddress", {address: address});
    },

    get suspended() { return this._syncData.suspended; },
    requestSuspend: function requestSuspend(reason) {
        nrdp._invoke(null, "requestSuspend", {reason: reason});
    },
    suspendComplete: function suspendComplete() {
        nrdp._fn("suspendComplete");
    },

    _fixXml: function _fixXml(obj) {
        var children = obj["$children"];
        var len, child;
        if (!children || !children.length)
            return;
        len = children.length;
        for (var i = 0; i < len; i++) {
            child = children[i];
            child["$parent"] = obj;
            child["$sibling"] = children[i + 1];
            var name = child["$name"];
            if (name !== undefined && obj[name] === undefined)
                obj[name] = child;
            this._fixXml(child);
        }
    },

    console: {
        _log: function(func, args) {
            var msg = "";
            var offset = 0;
            while(offset < args.length) {
                var tmp = args[offset++];
                if(offset != 1)
                    msg += " ";
                if(tmp && tmp instanceof Object) {
                    try {
                        if(tmp instanceof Array)
                            msg += JSON.stringify(tmp);
                        else
                            msg += "Object " + JSON.stringify(tmp);
                    } catch(e) {
                        msg += tmp;
                    }
                } else {
                    msg += tmp;
                }
            }
            func(msg);
        },
        error: function() { this._log(nrdp.log.error, arguments); },
        debug: function() { this._log(nrdp.log.debug, arguments); },
        log: function() { this._log(nrdp.log.debug, arguments); },
        warn: function() { this._log(nrdp.log.warn, arguments); },
        info: function() { this._log(nrdp.log.info, arguments); }
    },

    _setConfigurationOverrides: function (xml) {
        return nrdp._invoke(null, "setConfigurationOverrides", { xml: xml }).success;
    },

    // If these return something that is not undefined, that value is passed to the callback, if any.

    _eventBrokers:
    {
        'commandReceived':
            function(event) {
                this._callEventListeners(this, {type:'command', parameters: event.data});
            },
        'parsedXML':
            function(event) {
                if (event.data.success)
                    this._fixXml(event.data.object);
                return {data: event.data};
            },
        'suspendChanged':
            function(event) {
                this._callEventListeners(this, {type:'suspendChanged',
                    data: event.data,
                    time: event.time
                });
            },
        'requestedResume':
            function(event) {
                return {data: event.data};
            },
        'parsedJSON':
            function(event) {
                return {data: event.data.object};
            },
        'pong':
            function(event) {
                return {data: event.data.received};
            },
        'objects':
            function(event) {
                return {data: event.data.objects};
            },
        'locks':
           function(event) {
                return {data: event.data.locks};
            }
    },

    _dumpEvent: function() {
        for (var i=0; i<arguments.length; ++i) {
            var json;
            try {
                json = JSON.stringify(arguments[i], null, 4);
            } catch (err) {
                json = arguments[i];
            }
            nrdp.log.error(json);
        }
    },

    _dumpKeys: function() {
        for (var i=0; i<arguments.length; ++i) {
            nrdp.log.error("object: " + i);
            for (var key in arguments[i]) {
                nrdp.log.error(key);
            }
        }
    },

    _handleEvent: function _handleEvent(event) {
        var broker = this._eventBrokers[event.name],
            result,
            id,
            cb;
        if (!broker)
            return false;
        result = broker.call(this, event);
        if (result) {
            id = event.data.id;
            cb = this._cbs[id];
            try {
                if (typeof cb === 'function')
                    cb(result.data);
            } finally {
                delete this._cbs[id];
            }
        }
        return true;
    },
    _suspend: function _suspend(mode) {
        nrdp.gibbon._runConsole("/suspend " + (mode || ""));
    }
};
